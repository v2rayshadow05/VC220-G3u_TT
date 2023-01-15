/* vi: set sw=4 ts=4: */
/*
 * Mini reboot implementation for busybox
 *
 * Copyright (C) 1999-2004 by Erik Andersen <andersen@codepoet.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/reboot.h>
#include "busybox.h"
#include "init_shared.h"

#if defined(TCSUPPORT_START_TRAP) || defined(TCSUPPORT_SYSLOG_ENHANCE)
#define		SIGNAL_PATH		"/var/tmp/signal_reboot"
static int quit_signal(void)
{
	int ret;
	FILE *fp = NULL;
	char buf[8];

	memset(buf, 0, sizeof(buf));
	
	fp = fopen(SIGNAL_PATH, "r");

	if (fp == NULL)
		ret = 0;
	else {
		fgets(buf, 8, fp);
		
		if (buf[0] == '1')
			ret = 1;
		else 
			ret = 0;

		fclose(fp);
		unlink(SIGNAL_PATH);
	}

	return ret;
}
#endif

extern int reboot_main(int argc, char **argv)
{
	char *delay; /* delay in seconds before rebooting */
#if defined(MT7601E)
	FILE *fp = NULL;
	char buf[65];
	char APOn[4] = {0};
	char BssidNum[4] = {0};
	char tmp[32] = {0};
	int i;
#endif

#if defined(TCSUPPORT_START_TRAP) || defined(TCSUPPORT_SYSLOG_ENHANCE)
	int count = 0;
#endif
	if(bb_getopt_ulflags(argc, argv, "d:", &delay)) {
		sleep(atoi(delay));
	}

#if defined(TCSUPPORT_START_TRAP) || defined(TCSUPPORT_SYSLOG_ENHANCE)	
	system("killall -SIGUSR1 cfg_manager");
	/* wait cfg_manager done */
	while (!quit_signal() && count++ < 10)
		sleep(1);
	
#endif
#ifdef TCSUPPORT_SYSLOG_ENHANCE
	system("killall -9 syslogd");
#endif

#if defined(MT7601E)
	/* When system reboot, we do wlan interface down according to WCN's suggestion */
	/* to avoid WLan Led abnormal when system up only for MT7601E WiFi chip */
	fp = fopen("/etc/Wireless/WLAN_APOn", "r");
	if(!fp){
		printf("\ncan't open /etc/Wireless/WLAN_APOn");
		for (i=0; i<8; i++) {
			sprintf(tmp, "ifconfig ra%d down",i);
			system(tmp);
		}
		//return -1;
	}
	else{
		while(fgets(buf, 64, fp) != NULL){
			if(strstr(buf, "APOn=") != NULL)
				sscanf(buf, "APOn=%s", APOn);
			else if(strstr(buf, "Bssid_num=") != NULL)
				sscanf(buf, "Bssid_num=%s", BssidNum);
			memset(buf,0,sizeof(buf));
		}
		fclose(fp);
		
		if (!strcmp(APOn,"1")){
			for (i=0; i<atoi(BssidNum); i++) {
				sprintf(tmp, "ifconfig ra%d down",i);
				system(tmp);
			}
		}
	}
#endif

#ifndef CONFIG_INIT
#ifndef RB_AUTOBOOT
#define RB_AUTOBOOT		0x01234567
#endif
	return(bb_shutdown_system(RB_AUTOBOOT));
#else
#if !defined(TCSUPPORT_CT)
	return kill_init(SIGTERM);
#endif
#endif
}

/*
Local Variables:
c-file-style: "linux"
c-basic-offset: 4
tab-width: 4
End:
*/
