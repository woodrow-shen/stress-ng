/*
 * Copyright (C) 2024-2025 Colin Ian King.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */
#include "stress-ng.h"
#include "core-helper.h"
#include "core-seed-yaml.h"

/*
 *  Seeds for a single stressor, indexed by instance number. The seed
 *  yaml file format is self-defined and fixed-indentation, e.g.:
 *
 *  seeds:
 *    bigheap:
 *      instance_0: 12345678901234
 *      instance_1: 23456789012345
 *    vm:
 *      instance_0: 45678901234567
 *
 *  It is deliberately parsed line-by-line (no general yaml parser) since
 *  stress-ng both produces and consumes the file.
 */
typedef struct stress_seed_entry {
	struct stress_seed_entry *next;
	char		name[64];	/* munged stressor name */
	uint64_t	*seeds;		/* seed array, indexed by instance */
	bool		*present;	/* true if seeds[i] was set */
	int32_t		count;		/* number of seeds set */
	int32_t		capacity;	/* allocated entries in seeds/present */
} stress_seed_entry_t;

static stress_seed_entry_t *seed_entries;

/*
 *  stress_seed_entry_find()
 *	find a seed entry by munged stressor name
 */
static stress_seed_entry_t *stress_seed_entry_find(const char *name)
{
	stress_seed_entry_t *entry;

	for (entry = seed_entries; entry; entry = entry->next) {
		if (strcmp(entry->name, name) == 0)
			return entry;
	}
	return NULL;
}

/*
 *  stress_seed_entry_new()
 *	create a new named seed entry, prepended to the list
 */
static stress_seed_entry_t *stress_seed_entry_new(const char *name)
{
	stress_seed_entry_t *entry;

	entry = (stress_seed_entry_t *)calloc(1, sizeof(*entry));
	if (!entry)
		return NULL;
	(void)shim_strscpy(entry->name, name, sizeof(entry->name));
	entry->next = seed_entries;
	seed_entries = entry;
	return entry;
}

/*
 *  stress_seed_entry_set()
 *	set the seed for an instance, growing the arrays as required
 */
static int stress_seed_entry_set(
	stress_seed_entry_t *entry,
	const uint32_t instance,
	const uint64_t seed)
{
	if ((int32_t)instance >= entry->capacity) {
		const int32_t new_cap = (int32_t)instance + 1;
		uint64_t *new_seeds;
		bool *new_present;

		new_seeds = (uint64_t *)realloc(entry->seeds,
			(size_t)new_cap * sizeof(*new_seeds));
		if (!new_seeds)
			return -1;
		new_present = (bool *)realloc(entry->present,
			(size_t)new_cap * sizeof(*new_present));
		if (!new_present) {
			entry->seeds = new_seeds;
			return -1;
		}
		while (entry->capacity < new_cap) {
			new_seeds[entry->capacity] = 0;
			new_present[entry->capacity] = false;
			entry->capacity++;
		}
		entry->seeds = new_seeds;
		entry->present = new_present;
	}
	if (!entry->present[instance]) {
		entry->present[instance] = true;
		entry->count++;
	}
	entry->seeds[instance] = seed;
	return 0;
}

/*
 *  stress_seed_yaml_free()
 *	free the in-process seed lookup table
 */
void stress_seed_yaml_free(void)
{
	stress_seed_entry_t *entry = seed_entries;

	while (entry) {
		stress_seed_entry_t *next = entry->next;

		free(entry->seeds);
		free(entry->present);
		free(entry);
		entry = next;
	}
	seed_entries = NULL;
}

/*
 *  stress_seed_yaml_load()
 *	parse a seed yaml file into the in-process lookup table
 */
int stress_seed_yaml_load(const char *path)
{
	FILE *fp;
	char line[256];
	stress_seed_entry_t *cur = NULL;

	fp = fopen(path, "r");
	if (!fp) {
		pr_err("seed-yaml: cannot open seed yaml file '%s', errno=%d (%s)\n",
			path, errno, strerror(errno));
		return -1;
	}

	while (fgets(line, sizeof(line), fp)) {
		char name[64];
		unsigned int instance;
		unsigned long long seed;

		/* skip comments and blank lines */
		if (line[0] == '#' || line[0] == '\n' || line[0] == '\0')
			continue;

		/* 4-space indented "instance_N: seed" line */
		if ((line[0] == ' ') && (line[1] == ' ') &&
		    (line[2] == ' ') && (line[3] == ' ')) {
			if (!cur)
				continue;
			if (sscanf(line + 4, "instance_%u: %llu",
				   &instance, &seed) == 2) {
				if (stress_seed_entry_set(cur, (uint32_t)instance,
				    (uint64_t)seed) < 0) {
					pr_err("seed-yaml: out of memory parsing '%s'\n", path);
					(void)fclose(fp);
					return -1;
				}
			}
			continue;
		}

		/* 2-space indented "stressor:" line */
		if ((line[0] == ' ') && (line[1] == ' ') && (line[2] != ' ')) {
			if (sscanf(line + 2, "%63[^:]:", name) == 1) {
				cur = stress_seed_entry_find(name);
				if (!cur) {
					cur = stress_seed_entry_new(name);
					if (!cur) {
						pr_err("seed-yaml: out of memory parsing '%s'\n", path);
						(void)fclose(fp);
						return -1;
					}
				}
			}
			continue;
		}

		/* top-level "seeds:" header or anything else is ignored */
	}
	(void)fclose(fp);
	return 0;
}

/*
 *  stress_seed_yaml_lookup()
 *	look up a seed for a munged stressor name + instance
 */
bool stress_seed_yaml_lookup(
	const char *munged_name,
	const uint32_t instance,
	uint64_t *seed)
{
	const stress_seed_entry_t *entry = stress_seed_entry_find(munged_name);

	if (!entry)
		return false;
	if ((int32_t)instance >= entry->capacity)
		return false;
	if (!entry->present[instance])
		return false;
	*seed = entry->seeds[instance];
	return true;
}

/*
 *  stress_seed_yaml_validate()
 *	ensure each runnable stressor has exactly num_instances seeds defined
 */
int stress_seed_yaml_validate(stress_stressor_t *stressors_list)
{
	stress_stressor_t *ss;
	int rc = 0;

	for (ss = stressors_list; ss; ss = ss->next) {
		char munged[64];
		const stress_seed_entry_t *entry;

		if (ss->ignore.run || ss->ignore.permute)
			continue;
		if (!ss->stats)
			continue;

		(void)stress_munge_underscore(munged, ss->stressor->name, sizeof(munged));
		entry = stress_seed_entry_find(munged);
		if (!entry) {
			pr_err("seed-yaml: '%s' is missing from the seed yaml file, "
				"run needs %" PRId32 " instances\n",
				munged, ss->num_instances);
			rc = -1;
			continue;
		}
		if (entry->count != ss->num_instances) {
			pr_err("seed-yaml: '%s' has %" PRId32 " instances, "
				"run needs %" PRId32 "\n",
				munged, entry->count, ss->num_instances);
			rc = -1;
		}
	}
	return rc;
}

/*
 *  stress_seed_yaml_dump()
 *	dump per-instance seeds of all runnable stressors to a seed yaml file
 */
void stress_seed_yaml_dump(const char *path, stress_stressor_t *stressors_list)
{
	FILE *fp;
	stress_stressor_t *ss;

	fp = fopen(path, "w");
	if (!fp) {
		pr_err("seed-yaml: cannot open seed yaml file '%s' for writing, "
			"errno=%d (%s)\n", path, errno, strerror(errno));
		return;
	}

	(void)fprintf(fp, "seeds:\n");
	for (ss = stressors_list; ss; ss = ss->next) {
		int32_t j;
		bool any_seed = false;
		char munged[64];

		if (ss->ignore.run || ss->ignore.permute)
			continue;
		if (!ss->stats)
			continue;

		/*
		 *  Only dump instances that actually started and recorded
		 *  their own seed. seed_set stays false for an instance never
		 *  reached (e.g. run interrupted before its turn in
		 *  --sequential), so its zero-initialised stats->seed is
		 *  skipped rather than dumped as a bogus seed of 0. A stressor
		 *  with no started instances is omitted entirely so the file
		 *  has no empty headers. Note stats->pid cannot be used here:
		 *  it is reset to 0 once an instance is reaped.
		 */
		(void)stress_munge_underscore(munged, ss->stressor->name, sizeof(munged));
		for (j = 0; j < ss->num_instances; j++) {
			const stress_stats_t *const stats = ss->stats[j];

			if (!stats->seed_set)
				continue;
			if (!any_seed) {
				(void)fprintf(fp, "  %s:\n", munged);
				any_seed = true;
			}
			(void)fprintf(fp, "    instance_%" PRId32 ": %" PRIu64 "\n",
				j, stats->seed);
		}
	}
	(void)fclose(fp);
}
