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
#ifndef CORE_SEED_YAML_H
#define CORE_SEED_YAML_H

#include "stress-ng.h"

/* Load a seed yaml file into the in-process lookup table, 0 = OK, -1 = error */
extern int stress_seed_yaml_load(const char *path);

/* Look up the seed for a munged stressor name + instance, returns true if found */
extern bool stress_seed_yaml_lookup(const char *munged_name,
	const uint32_t instance, uint64_t *seed);

/*
 *  Validate that each runnable stressor has exactly num_instances seeds
 *  defined in the loaded seed yaml file, 0 = OK, -1 = mismatch
 */
extern int stress_seed_yaml_validate(stress_stressor_t *stressors_list);

/* Dump per-instance seeds of all runnable stressors to a seed yaml file */
extern void stress_seed_yaml_dump(const char *path,
	stress_stressor_t *stressors_list);

/* Free the in-process seed lookup table */
extern void stress_seed_yaml_free(void);

#endif
