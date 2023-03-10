/* vi: set sw=4 ts=4: */
/*
 * Small implementation of brctl for busybox.
 *
 * Copyright (C) 2008 by Bernhard Reutner-Fischer
 *
 * Some helper functions from bridge-utils are
 * Copyright (C) 2000 Lennert Buytenhek
 *
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 */
/* This applet currently uses only the ioctl interface and no sysfs at all.
 * At the time of this writing this was considered a feature.
 */

//usage:#define brctl_trivial_usage
//usage:       "COMMAND [BRIDGE [INTERFACE]]"
//usage:#define brctl_full_usage "\n\n"
//usage:       "Manage ethernet bridges\n"
//usage:     "\nCommands:"
//usage:	IF_FEATURE_BRCTL_SHOW(
//usage:     "\n	show			Show a list of bridges"
//usage:	)
//usage:     "\n	addbr BRIDGE		Create BRIDGE"
//usage:     "\n	delbr BRIDGE		Delete BRIDGE"
//usage:     "\n	addif BRIDGE IFACE	Add IFACE to BRIDGE"
//usage:     "\n	delif BRIDGE IFACE	Delete IFACE from BRIDGE"
//usage:	IF_FEATURE_BRCTL_FANCY(
//usage:     "\n	setageing BRIDGE TIME		Set ageing time"
//usage:     "\n	setfd BRIDGE TIME		Set bridge forward delay"
//usage:     "\n	sethello BRIDGE TIME		Set hello time"
//usage:     "\n	setmaxage BRIDGE TIME		Set max message age"
//usage:     "\n	setpathcost BRIDGE COST		Set path cost"
//usage:     "\n	setportprio BRIDGE PRIO		Set port priority"
//usage:     "\n	setbridgeprio BRIDGE PRIO	Set bridge priority"
//usage:     "\n	stp BRIDGE [1/yes/on|0/no/off]	STP on/off"
//usage:	)

#include "libbb.h"
#include <linux/sockios.h>
#include <net/if.h>
/* hzg modified 2013-4-16 */
#if 0
#define BRCTL_ENABLE_SNOOPING 23
#else
#define BRCTL_SET_PORT_SNOOPING 30
#define BRCTL_CLEAR_PORT_SNOOPING 31
#define BRCTL_ENABLE_SNOOPING 21
#define BRCTL_SHOW_SNOOPING 32
#define BRCTL_ENABLE_PROXY_MODE 33
#define SIOCBRSHOWSNOOPING 34

#define BRCTL_SET_SNOOPING_MAXGROUPS  35
#define BRCTL_SET_SNOOPING_FASTLEAVE  36
#define BRCTL_SET_SNOOPING_LEAVETIMEOUT 37
#define BRCTL_SET_SNOOPING_QUERYTIMEOUT 38
#define BRCTL_SET_SNOOPING_HOSTTIMEOUT  39
#define BRCTL_SET_SNOOPING_MROUTERTIMEOUT 40
#endif
/* end--modified */


#ifndef SIOCBRADDBR
# define SIOCBRADDBR BRCTL_ADD_BRIDGE
#endif
#ifndef SIOCBRDELBR
# define SIOCBRDELBR BRCTL_DEL_BRIDGE
#endif
#ifndef SIOCBRADDIF
# define SIOCBRADDIF BRCTL_ADD_IF
#endif
#ifndef SIOCBRDELIF
# define SIOCBRDELIF BRCTL_DEL_IF
#endif


/* Maximum number of ports supported per bridge interface.  */
#ifndef MAX_PORTS
# define MAX_PORTS 32
#endif

/* Use internal number parsing and not the "exact" conversion.  */
/* #define BRCTL_USE_INTERNAL 0 */ /* use exact conversion */
#define BRCTL_USE_INTERNAL 1

#if ENABLE_FEATURE_BRCTL_FANCY
# include <linux/if_bridge.h>

/* FIXME: These 4 funcs are not really clean and could be improved */
static ALWAYS_INLINE void strtotimeval(struct timeval *tv,
		const char *time_str)
{
	double secs;
# if BRCTL_USE_INTERNAL
	char *endptr;
	secs = /*bb_*/strtod(time_str, &endptr);
	if (endptr == time_str)
# else
	if (sscanf(time_str, "%lf", &secs) != 1)
# endif
		bb_error_msg_and_die(bb_msg_invalid_arg, time_str, "timespec");
	tv->tv_sec = secs;
	tv->tv_usec = 1000000 * (secs - tv->tv_sec);
}

static ALWAYS_INLINE unsigned long tv_to_jiffies(const struct timeval *tv)
{
	unsigned long long jif;

	jif = 1000000ULL * tv->tv_sec + tv->tv_usec;

	return jif/10000;
}
# if 0
static void jiffies_to_tv(struct timeval *tv, unsigned long jiffies)
{
	unsigned long long tvusec;

	tvusec = 10000ULL*jiffies;
	tv->tv_sec = tvusec/1000000;
	tv->tv_usec = tvusec - 1000000 * tv->tv_sec;
}
# endif
static unsigned long str_to_jiffies(const char *time_str)
{
	struct timeval tv;
	strtotimeval(&tv, time_str);
	return tv_to_jiffies(&tv);
}

static void arm_ioctl(unsigned long *args,
		unsigned long arg0, unsigned long arg1, unsigned long arg2)
{
	args[0] = arg0;
	args[1] = arg1;
	args[2] = arg2;
	args[3] = 0;
}
#endif

/* added by yangxv for showmacs */
static int compare_fdbs(const void *_f0, const void *_f1);
static void br_show_timer(unsigned long at);
static void __dump_fdb_entry(struct __fdb_entry *f) ;

static int compare_fdbs(const void *_f0, const void *_f1)
{
    const struct __fdb_entry *f0 = _f0;
    const struct __fdb_entry *f1 = _f1;
 
    return memcmp(f0->mac_addr, f1->mac_addr, 6);
}

static void br_show_timer(unsigned long at)
{
    printf("%2i.%.2i", (int)at / 100, (int)at % 100);
}

static void __dump_fdb_entry(struct __fdb_entry *f) 
{ 
    printf("%3i\t", f->port_no); 
    printf("%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\t", 
           f->mac_addr[0], f->mac_addr[1], f->mac_addr[2], 
           f->mac_addr[3], f->mac_addr[4], f->mac_addr[5]); 
    printf("%s\t\t", f->is_local ? "yes" : "no"); 
    br_show_timer(f->ageing_timer_value); 
    printf("\n"); 
}
/* end added */

int brctl_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int brctl_main(int argc UNUSED_PARAM, char **argv)
{
	static const char keywords[] ALIGN1 =
		"addbr\0" "delbr\0" "addif\0" "delif\0"
	IF_FEATURE_BRCTL_FANCY(
		"stp\0"
		"setageing\0" "setfd\0" "sethello\0" "setmaxage\0"
		"setpathcost\0" "setportprio\0" "setbridgeprio\0" "setbrsnooping\0"
/* hzg added 2013-4-15 */
		"snp_fastleave\0" "snp_maxgroups\0" "snp_leavetimeout\0"
		"snp_querytimeout\0" "snp_hosttimeout\0" "snp_mroutertimeout\0"
/* end--added */
	)
	IF_FEATURE_BRCTL_SHOW("showmacs\0" "show\0");

	enum { ARG_addbr = 0, ARG_delbr, ARG_addif, ARG_delif
		IF_FEATURE_BRCTL_FANCY(,
		   ARG_stp,
		   ARG_setageing, ARG_setfd, ARG_sethello, ARG_setmaxage,
		   ARG_setpathcost, ARG_setportprio, ARG_setbridgeprio, ARG_setbrsnooping,
/* hzg added 2013-4-15 */
		   ARG_snp_fastleave, ARG_snp_maxgroups, ARG_snp_leavetimeout,
		   ARG_snp_querytimeout, ARG_snp_hosttimeout, ARG_snp_mroutertimeout
/* end--added */
		)
		IF_FEATURE_BRCTL_SHOW(, ARG_showmacs, ARG_show)
	};

	int fd;
	smallint key;
	struct ifreq ifr;
	char *br, *brif;

	argv++;
	while (*argv) {
#if ENABLE_FEATURE_BRCTL_FANCY
		int ifidx[MAX_PORTS];
		unsigned long args[4];
		ifr.ifr_data = (char *) &args;
#endif

		key = index_in_strings(keywords, *argv);
		if (key == -1) /* no match found in keywords array, bail out. */
			bb_error_msg_and_die(bb_msg_invalid_arg, *argv, applet_name);
		argv++;
		fd = xsocket(AF_INET, SOCK_STREAM, 0);

#if ENABLE_FEATURE_BRCTL_SHOW
		if (key == ARG_show) { /* show */
			char brname[IFNAMSIZ];
			int bridx[MAX_PORTS];
			int i, num;
			arm_ioctl(args, BRCTL_GET_BRIDGES,
						(unsigned long) bridx, MAX_PORTS);
			num = xioctl(fd, SIOCGIFBR, args);
			printf("bridge name\tbridge id\t\tSTP enabled\tinterfaces\n");
			for (i = 0; i < num; i++) {
				char ifname[IFNAMSIZ];
				int j, tabs;
				struct __bridge_info bi;
				unsigned char *x;

				if (!if_indextoname(bridx[i], brname))
					bb_perror_msg_and_die("can't get bridge name for index %d", i);
				strncpy_IFNAMSIZ(ifr.ifr_name, brname);

				arm_ioctl(args, BRCTL_GET_BRIDGE_INFO,
							(unsigned long) &bi, 0);
				xioctl(fd, SIOCDEVPRIVATE, &ifr);
				printf("%s\t\t", brname);

				/* print bridge id */
				x = (unsigned char *) &bi.bridge_id;
				for (j = 0; j < 8; j++) {
					printf("%.2x", x[j]);
					if (j == 1)
						bb_putchar('.');
				}
				printf(bi.stp_enabled ? "\tyes" : "\tno");

				/* print interface list */
				arm_ioctl(args, BRCTL_GET_PORT_LIST,
							(unsigned long) ifidx, MAX_PORTS);
				xioctl(fd, SIOCDEVPRIVATE, &ifr);
				tabs = 0;
				for (j = 0; j < MAX_PORTS; j++) {
					if (!ifidx[j])
						continue;
					if (!if_indextoname(ifidx[j], ifname))
						bb_perror_msg_and_die("can't get interface name for index %d", j);
					if (tabs)
						printf("\t\t\t\t\t");
					else
						tabs = 1;
					printf("\t\t%s\n", ifname);
				}
				if (!tabs)  /* bridge has no interfaces */
					bb_putchar('\n');
			}
			goto done;
		}
#endif

		if (!*argv) /* all but 'show' need at least one argument */
			bb_show_usage();

		br = *argv++;

		if (key == ARG_addbr || key == ARG_delbr) { /* addbr or delbr */
			ioctl_or_perror_and_die(fd,
					key == ARG_addbr ? SIOCBRADDBR : SIOCBRDELBR,
					br, "bridge %s", br);
			goto done;
		}

		/* added by yangxv for showmacs */
		if (key == ARG_showmacs) { /* show */
			int offset = 0;
			struct __fdb_entry fdb[1024];

			printf("port\tmac addr\t\tlocal\tageing timer\n");

			offset = 0;
			strncpy_IFNAMSIZ(ifr.ifr_name, br);

			while(1)
			{
				int i = 0;
				int num = 0;

				args[0] = BRCTL_GET_FDB_ENTRIES;
				args[1] = (unsigned long)&fdb;
				args[2] = 1024;
				args[3] = offset;	

				num = xioctl(fd, SIOCDEVPRIVATE, &ifr);


				if (!num)
				break;

				qsort(fdb, num, sizeof(struct __fdb_entry), compare_fdbs);

				for (i = 0; i < num; i++)
					__dump_fdb_entry(fdb + i);

				offset += num;
			}
			goto done;
		}
		/* end added */

		if (!*argv) /* all but 'addbr/delbr' need at least two arguments */
			bb_show_usage();

		strncpy_IFNAMSIZ(ifr.ifr_name, br);
		if (key == ARG_addif || key == ARG_delif) { /* addif or delif */
			brif = *argv;
			ifr.ifr_ifindex = if_nametoindex(brif);
			if (!ifr.ifr_ifindex) {
				bb_perror_msg_and_die("iface %s", brif);
			}
			ioctl_or_perror_and_die(fd,
					key == ARG_addif ? SIOCBRADDIF : SIOCBRDELIF,
					&ifr, "bridge %s", br);
			goto done_next_argv;
		}
#if ENABLE_FEATURE_BRCTL_FANCY
		if (key == ARG_stp) { /* stp */
			static const char no_yes[] ALIGN1 =
				"0\0" "off\0" "n\0" "no\0"   /* 0 .. 3 */
				"1\0" "on\0"  "y\0" "yes\0"; /* 4 .. 7 */
			int onoff = index_in_strings(no_yes, *argv);
			if (onoff < 0)
				bb_error_msg_and_die(bb_msg_invalid_arg, *argv, applet_name);
			onoff = (unsigned)onoff / 4;
			arm_ioctl(args, BRCTL_SET_BRIDGE_STP_STATE, onoff, 0);
			goto fire;
		}
		if ((unsigned)(key - ARG_setageing) < 4) { /* time related ops */
			static const uint8_t ops[] ALIGN1 = {
				BRCTL_SET_AGEING_TIME,          /* ARG_setageing */
				BRCTL_SET_BRIDGE_FORWARD_DELAY, /* ARG_setfd     */
				BRCTL_SET_BRIDGE_HELLO_TIME,    /* ARG_sethello  */
				BRCTL_SET_BRIDGE_MAX_AGE        /* ARG_setmaxage */
			};
			arm_ioctl(args, ops[key - ARG_setageing], str_to_jiffies(*argv), 0);
			goto fire;
		}
		if (key == ARG_setpathcost
		 || key == ARG_setportprio
		 || key == ARG_setbridgeprio
		 || key == ARG_setbrsnooping
/* hzg added 2013-4-15 */
		 || key == ARG_snp_fastleave
		 || key == ARG_snp_maxgroups
		 || key == ARG_snp_leavetimeout
		 || key == ARG_snp_querytimeout
		 || key == ARG_snp_hosttimeout
		 || key == ARG_snp_mroutertimeout
/* end--added */
		) {
			static const uint8_t ops[] ALIGN1 = {
				BRCTL_SET_PATH_COST,      /* ARG_setpathcost   */
				BRCTL_SET_PORT_PRIORITY,  /* ARG_setportprio   */
				BRCTL_SET_BRIDGE_PRIORITY, /* ARG_setbridgeprio */
				BRCTL_ENABLE_SNOOPING, /*ARG_setbrsnooping*/
/* hzg added 2013-4-15 */
				BRCTL_SET_SNOOPING_FASTLEAVE,
				BRCTL_SET_SNOOPING_MAXGROUPS,
				BRCTL_SET_SNOOPING_LEAVETIMEOUT,
				BRCTL_SET_SNOOPING_QUERYTIMEOUT,
				BRCTL_SET_SNOOPING_HOSTTIMEOUT,
				BRCTL_SET_SNOOPING_MROUTERTIMEOUT
/* end--added */
			};
			int port = -1;
			unsigned long arg1, arg2;

			if ((key != ARG_setbridgeprio)
				&& (key != ARG_setbrsnooping)
/* hzg added 2013-4-15 */
				&& (key != ARG_snp_fastleave)
				&& (key != ARG_snp_maxgroups)
				&& (key != ARG_snp_leavetimeout)
				&& (key != ARG_snp_querytimeout)
				&& (key != ARG_snp_hosttimeout)
				&& (key != ARG_snp_mroutertimeout)
/* end--added */       ){
				/* get portnum */
				unsigned i;

				port = if_nametoindex(*argv++);
				if (!port)
					bb_error_msg_and_die(bb_msg_invalid_arg, *argv, "port");
				memset(ifidx, 0, sizeof ifidx);
				arm_ioctl(args, BRCTL_GET_PORT_LIST, (unsigned long)ifidx,
						  MAX_PORTS);
				xioctl(fd, SIOCDEVPRIVATE, &ifr);
				for (i = 0; i < MAX_PORTS; i++) {
					if (ifidx[i] == port) {
						port = i;
						break;
					}
				}
			}
			arg1 = port;
			arg2 = xatoi_positive(*argv);
			
			if (key == ARG_setbridgeprio) {
				arg1 = arg2;
				arg2 = 0;
			}
			if (key == ARG_setbrsnooping 
/* hzg added 2013-4-15 */
			|| key == ARG_snp_fastleave
			|| key == ARG_snp_maxgroups
			|| key == ARG_snp_leavetimeout
			|| key == ARG_snp_querytimeout
			|| key == ARG_snp_hosttimeout
			|| key == ARG_snp_mroutertimeout
/* end--added */        )
			{
				arg1 = br;
			}
			arm_ioctl(args, ops[key - ARG_setpathcost], arg1, arg2);
		}
 fire:
		/* Execute the previously set command */
		xioctl(fd, SIOCDEVPRIVATE, &ifr);
#endif
 done_next_argv:
		argv++;
 done:
		close(fd);
	}

	return EXIT_SUCCESS;
}
