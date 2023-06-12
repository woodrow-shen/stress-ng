/*
 * Copyright (C) 2013-2021 Canonical, Ltd.
 * Copyright (C) 2022-2023 Colin Ian King.
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
#include "core-builtin.h"
#include "core-net.h"

#if defined(HAVE_LINUX_SOCKIOS_H)
#include <linux/sockios.h>
#else
UNEXPECTED
#endif

#define MAX_UDP_SIZE	(2048)

static const stress_help_t help[] = {
	{ NULL,	"udp-flood N",		"start N workers that performs a UDP flood attack" },
	{ NULL,	"udp-flood-domain D",	"specify domain, default is ipv4" },
	{ NULL, "udp-flood-if I",	"use network interface I, e.g. lo, eth0, etc." },
	{ NULL,	"udp-flood-ops N",	"stop after N udp flood bogo operations" },
	{ NULL,	NULL,			NULL }
};

/*
 *  stress_set_udp_domain()
 *      set the udp domain option
 */
static int stress_set_udp_flood_domain(const char *name)
{
	int ret, udp_flood_domain;

	ret = stress_set_net_domain(DOMAIN_INET_ALL, "udp-flood-domain",
		name, &udp_flood_domain);
	stress_set_setting("udp-flood-domain", TYPE_ID_INT, &udp_flood_domain);

	return ret;
}

static int stress_set_udp_flood_if(const char *name)
{
	return stress_set_setting("udp-flood-if", TYPE_ID_STR, name);
}

static const stress_opt_set_func_t opt_set_funcs[] = {
	{ OPT_udp_flood_domain,	stress_set_udp_flood_domain },
	{ OPT_udp_flood_if,	stress_set_udp_flood_if },
	{ 0,			NULL }
};

#if defined(AF_PACKET)

/*
 *  stress_udp_flood
 *	UDP flood
 */
static int OPTIMIZE3 stress_udp_flood(const stress_args_t *args)
{
	int fd, rc = EXIT_SUCCESS, j = 0;
	int udp_flood_domain = AF_INET;
	int port = 1024;
	struct sockaddr *addr;
	socklen_t addr_len;
	const size_t sz_max = STRESS_MINIMUM(23 + args->instance, MAX_UDP_SIZE);
	size_t sz = 1;
	char *udp_flood_if = NULL;
	double bytes = 0.0, duration, t, rate;

	(void)stress_get_setting("udp-flood-domain", &udp_flood_domain);
	(void)stress_get_setting("udp-flood-if", &udp_flood_if);

	if (udp_flood_if) {
		int ret;
		struct sockaddr if_addr;

		ret = stress_net_interface_exists(udp_flood_if, udp_flood_domain, &if_addr);
		if (ret < 0) {
			pr_inf("%s: interface '%s' is not enabled for domain '%s', defaulting to using loopback\n",
				args->name, udp_flood_if, stress_net_domain(udp_flood_domain));
			udp_flood_if = NULL;
		}
	}

	if ((fd = socket(udp_flood_domain, SOCK_DGRAM, AF_PACKET)) < 0) {
		if (errno == EPROTONOSUPPORT) {
			if (args->instance == 0)
				pr_inf_skip("%s: skipping stressor, protocol not supported\n",
					args->name);
			return EXIT_NOT_IMPLEMENTED;
		}
		pr_fail("%s: socket failed, errno=%d (%s)\n",
			args->name, errno, strerror(errno));
		return EXIT_FAILURE;
	}
	if (stress_set_sockaddr_if(args->name, args->instance, args->pid,
			udp_flood_domain, port, udp_flood_if,
			&addr, &addr_len, NET_ADDR_ANY) < 0) {
	}

	stress_set_proc_state(args->name, STRESS_STATE_RUN);

	t = stress_time_now();
	for (;;) {
		char buf[MAX_UDP_SIZE];
		int rand_port, reserved_port;
		ssize_t n;

		if (UNLIKELY(++port > 65535))
			port = 1024;

		reserved_port = stress_net_reserve_ports(port, port);
		if (UNLIKELY(reserved_port < 0))
			goto next;	/* try again */
		port = reserved_port;

		stress_set_sockaddr_port(udp_flood_domain, port, addr);
		(void)shim_memset(buf, stress_ascii64[j++ & 63], sz);
		n = sendto(fd, buf, sz, 0, addr, addr_len);
		if (LIKELY(n > 0)) {
			inc_counter(args);
			bytes += (double)n;
		}

#if defined(SIOCOUTQ)
		if (UNLIKELY((port & 0x1f) == 0)) {
			int pending;

			VOID_RET(int, ioctl(fd, SIOCOUTQ, &pending));
		}
#else
		UNEXPECTED
#endif
		stress_net_release_ports(port, port);

		if (!keep_stressing(args))
			break;

		rand_port = 1024 + stress_mwc16modn(65535 - 1024);
		reserved_port = stress_net_reserve_ports(rand_port, rand_port);
		if (UNLIKELY(reserved_port < 0))
			goto next;	/* try again */
		rand_port = reserved_port;
		stress_set_sockaddr_port(udp_flood_domain, rand_port, addr);
		n = sendto(fd, buf, sz, 0, addr, addr_len);
		if (LIKELY(n > 0)) {
			inc_counter(args);
			bytes += (double)n;
		}
		stress_net_release_ports(rand_port, rand_port);
		if (UNLIKELY(++sz >= sz_max))
			sz = 1;
next:
		if (UNLIKELY(!keep_stressing(args)))
			break;
	}
	duration = stress_time_now() - t;

	rate = (duration > 0.0) ? (bytes / duration) / (double)MB : 0.0;
	stress_metrics_set(args, 0, "MB per sec sendto rate", rate);
	rate = (duration > 0.0) ? (get_counter(args) / duration) : 0.0;
	stress_metrics_set(args, 1, "sendto calls per sec", rate);

	stress_set_proc_state(args->name, STRESS_STATE_DEINIT);

	(void)close(fd);

	return rc;
}

stressor_info_t stress_udp_flood_info = {
	.stressor = stress_udp_flood,
	.class = CLASS_NETWORK | CLASS_OS,
	.opt_set_funcs = opt_set_funcs,
	.help = help
};
#else
stressor_info_t stress_udp_flood_info = {
	.stressor = stress_unimplemented,
	.class = CLASS_NETWORK | CLASS_OS,
	.opt_set_funcs = opt_set_funcs,
	.help = help,
	.unimplemented_reason = "built with undefined AF_PACKET"
};
#endif
