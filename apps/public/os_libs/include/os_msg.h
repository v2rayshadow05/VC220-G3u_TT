/*  Copyright(c) 2010-2011 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file	os_msg.h
 * brief	The msg lib declarations. 
 *
 * author	Yang Xv
 * version	1.0.0
 * date	28Apr11
 *
 * history 	\arg 1.0.0, 28Apr11, Yang Xv, Create the file.
 */

#ifndef __OS_MSG_H__
#define __OS_MSG_H__

/* NOTE : DO NOT include <cstd.h> */
#include <unistd.h>
#ifdef INCLUDE_PORT_MIRRORING
#include <time.h>
#endif


/* do not include <netinet/in.h> */

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/

#ifdef INCLUDE_TTNET
/* 
 * brief Message content size	
 */
#define MSG_CONTNET_SIZE	1024
/* 
 * brief Message size	
 */
#define MSG_SIZE			1032

#else
/* 
 * brief Message content size	
 */
#define MSG_CONTNET_SIZE	512
/* 
 * brief Message size	
 */
#define MSG_SIZE			520
#endif


/* for all message type */

#define SNTP_DM_SERVER_NUM	5

/* 
 * brief status of DHCP6C_INFO_MSG_BODY
 */
#ifdef INCLUDE_IPV6
#define DHCP6C_ASSIGNED_DNS 	0x1
#define DHCP6C_ASSIGNED_ADDR 	0x2
#define DHCP6C_ASSIGNED_PREFIX 	0x4
#define DHCP6C_ASSIGNED_DSLITE_ADDR		0x8  /* Add by YuanRui: support DS-Lite, 21Mar12 */
#endif	/* INCLUDE_IPV6 */

#if defined(INCLUDE_TT_SYSLOG) || defined(INCLUDE_PKTCAP_UPLOAD)
#define FILE_MAX_PATH 256
#define URL_MAX_PATH 256
#define HOST_MAX_PATH 64
#define UPLOAD_FTP "ftp"
#define UPLOAD_TFTP "tftp"
#endif /* INCLUDE_TT_SYSLOG or INCLUDE_PKTCAP_UPLOAD */

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/


#ifdef __LINUX_OS_FC__
#include <sys/un.h>

typedef struct
{
	int fd;
	struct sockaddr_un _localAddr;
	struct sockaddr_un _remoteAddr;
}CMSG_FD;
#endif	/* __LINUX_OS_FC__ */

#ifdef __VXWORKS_OS_FC__
typedef struct
{
	int fd;
}CMSG_FD;
#endif /* __VXWORKS_OS_FC__ */


/* 
 * brief 	Enumeration message type
 * 			Convention:
 *			System message types - 1000 ~ 1999
 *			Common application message types - 2000 ~ 2999
 */
typedef enum
/* if a message's size is sizeof(unsigned int)
 * only the member priv in CMSG_BUFF need to be used
 */
{
	CMSG_NULL = 0,
	CMSG_REPLY = 1,
	CMSG_LOG = 2000,
	CMSG_SNTP_CFG = 2001,
	CMSG_SNTP_STATUS = 2002,	/* only have one word value */
	CMSG_SNTP_START = 2003,

	CMSG_DNS_PROXY_CFG = 2004,
	CMSG_DNS_SERVER	= 2005,		/* only have one word value */
	CMSG_PPP_STATUS = 2006,

 	/* Added by Yang Caiyong, 11-Aug-29.
 	 * For auto PVC.
 	 */ 
	CMSG_AUTO_PVC_SEARCHED = 2007,
 	/* Ended by Yang Caiyong, 11-Aug-29. */
	
	/* Added by whb , 2013-03-27. */
	CMSG_IP_STATUS = 2008,
	/* Ended by whb. */
	
#if defined(INCLUDE_TTNET) && defined(INCLUDE_TR111_PART1)
	CMSG_DHCPS_ADD_TR111DEV = 2009,/*Add by YuChuwei*/
#endif
	/* Added by xcl, 2011-06-13.*/
	CMSG_DHCPS_RELOAD_CFG = 2010, 
	CMSG_DHCPS_WRITE_LEASE_TO_SHM = 2011,
	CMSG_DHCPS_WAN_STATUS_CHANGE = 2012,
	CMSG_DHCPC_STATUS = 2013,
	CMSG_DHCPC_START = 2014,
	CMSG_DHCPC_RENEW = 2015, 
	CMSG_DHCPC_RELEASE = 2016,
	CMSG_DHCPC_SHUTDOWN = 2017,
	CMSG_DHCPC_MAC_CLONE = 2018,
	/* End added by xcl, 2011-06-13.*/
#if defined(INCLUDE_TTNET) && defined(INCLUDE_TR111_PART1)
		CMSG_DHCPS_DEL_TR111DEV = 2019, /*Add by YuChuwei*/
#endif


	CMSG_DDNS_PH_RT_CHANGED = 2020,
	CMSG_DDNS_PH_CFG_CHANGED = 2021,
	CMSG_DDNS_PH_GET_RT	= 2022,
	/* Added by xcl, for dyndns, 24Nov11 */
	CMSG_DYNDNS_RT_CHANGED = 2023,
	CMSG_DYNDNS_CFG_CHANGED = 2024,
	CMSG_DYNDNS_STATE_CHANGED = 2025,
	/* Added by tpj, for noipdns, 17Jan12 */
	CMSG_NOIPDNS_RT_CHANGED = 2026,
	CMSG_NOIPDNS_CFG_CHANGED = 2027,
	CMSG_NOIPDNS_STATE_CHANGED = 2028,
	/*end (n & n)*/

	CMSG_HTTPD_SERVICE_CFG = 2030,
	CMSG_HTTPD_USER_CFG = 2031,
	CMSG_HTTPD_HOST_CFG = 2032,
	CMSG_HTTPD_CHALLENGE_CFG = 2033,
#ifdef INCLUDE_TTNET
		CMSG_HTTPD_XTTNET_SET_USER = 2034,
		CMSG_HTTPD_XTTNET_USER_CFG = 2035,
		CMSG_XTTNET_CHANGE_AUTH_INFO = 2036,
#endif /* INCLUDE_TTNET */
#ifdef INCLUDE_WEB_RESTRICTION
		CMSG_XTTNET_CHANGE_PAGE_LEVEL = 2037,
		CMSG_XTTNET_CHANGE_PAGE_LIMITATION = 2038,
		CMSG_XTTNET_CHANGE_PAGE_LOCK = 2039,
#endif /*INCLUDE_WEB_RESTRICTION*/


	CMSG_CLI_USRCFG_UPDATE	= 2040,

	/* Added by zj, for userdefined ddns, 28Jan14 */
	CMSG_DDNS_UD_RT_CHANGED = 2041,
	CMSG_DDNS_UD_CFG_CHANGED = 2042,
	CMSG_DDNS_UD_STATE_CHANGED = 2043,

	/*added by xianjiantao for 9970 ttnet 3.0,17-12-24*/
#ifdef INCLUDE_TTNET
	CMSG_IP_DOMAIN_TEST_START = 2044,
	CMSG_HTTPD_XTTNET_SET_LOCAL_DOMAIN_NAME = 2045,
	CMSG_CONNECTION_DIAG_TEST_START = 2046,
#endif
	/*added end xianjiantao*/

 	/* Added  by  Li Chenglong , 11-Jul-31.*/
	CMSG_UPNP_ENABLE = 2050,
	CMSG_DEFAULT_GW_CH = 2051,
 	/* Ended  by  Li Chenglong , 11-Jul-31.*/

	/* Add by chz, 2012-12-24 */
	CMSG_UPNP_DEL_ENTRY = 2052,
	/* end add */
	/* Add by Chen Zexian, 20130116 */
	CMSG_UPNP_LAN_IP_CH = 2053,
	/* End of add */
	/*added by xianjiantao for vr600 tt v1 11 ac new requirement issue 18,17-04-12*/
#ifdef INCLUDE_TTNET
	CMSG_UPNP_PORTMAPPING_ENABLE = 2054,
	CMSG_UPNP_DEL_ALL_ENTRY = 2055,
	CMSG_UPNP_UPDATE_RULES = 2056,
	CMSG_UPNP_ENABLE_ENTRY = 2057,
#endif
	/*added end xianjiantao*/

	/* added by Yuan Shang for diagTool, 2011-08-18 */
	CMSG_DIAG_TOOL_COMMAND = 2060,

	/* added by wuzhiqin, 2011-09-26 */
	CMSG_CWMP_CFG_UPDATE = 2070, 
	CMSG_CWMP_PARAM_VAL_CHANGED = 2071,
	CMSG_CWMP_WAN_CONNECTION_UP = 2072,
	CMSG_CWMP_TIMER = 2073,
#ifdef INCLUDE_IGMP_DIAG /* ZJ, 27Dec2013 */
	CMSG_CWMP_IGMPDIAG_COMPLETE	= 2075,
#endif /* INCLUDE_IGMP_DIAG */
#ifdef INCLUDE_DSL_DIAG
	CMSG_CWMP_DIAGNOSTIC_COMPLETED = 2076,
#endif /* INCLUDE_DSL_DIAG */
	CMSG_CWMP_WAN_CONNECTION_DOWN = 2077,
	/* ended by wuzhiqin, 2011-09-26 */
#ifdef INCLUDE_TTNET
	CMSG_CWMP_GET_CPE_STATE = 2078,
	CMSG_CWMP_SEND_RSC_STATE = 2079,
#endif /* INCLUDE_TTNET */
        CMSG_DIAG_LOAD_MULTI  = 2080,

	/* added by Yuan Shang for usb storage hotplug event, 2011-12-09 */
	/* delete by zjj ,2013.09 */
	/* CMSG_USB_HOTPLUG_EVENT = 2080, */

	/* added by Wang Wenhao for IGMPv3 Proxy, 2011-11-24 */
	CMSG_IGMPD_ADD_LAN_IF = 2100,
	CMSG_IGMPD_ADD_WAN_IF = 2101,
	CMSG_IGMPD_DEL_IF = 2102,
#ifdef INCLUDE_TTNET
	CMSG_IGMPD_CHG_VERSION = 2103,
#endif /* INCLUDE_TTNET */

	/* Added by LI CHENGLONG , 2011-Dec-15.*/
	CMSG_DLNA_MEDIA_SERVER_INIT = 2110,
	CMSG_DLNA_MEDIA_SERVER_MANUAL_SCAN = 2111,
	CMSG_DLNA_MEDIA_SERVER_OP_FOLDER = 2112,
	CMSG_DLNA_MEDIA_SERVER_RELOAD = 2113,
	CMSG_DLNA_MEDIA_DEVICE_STATE_CHANGE = 2114,
	/* Ended by LI CHENGLONG , 2011-Dec-15.*/
#ifdef INCLUDE_LAN_WLAN
	CMSG_WPS_CFG = 2700,
	CMSG_WLAN_SWITCH = 2701,
	CMSG_WPS_PIN_SWITCH = 2702,
#ifdef INCLUDE_VOIP
	CMSG_FXS_SET_WIFI = 2703,
#endif /* INCLUDE_VOIP */
#endif /* INCLUDE_LAN_WLAN */

	/* 
	 * for voice process. added by zhonglianbo 2011-8-10
	 */
#ifdef INCLUDE_VOIP
	CMSG_VOIP_CMCNXCOUNT 			= 2800,
	CMSG_VOIP_WAN_STS_CHANGED		= 2801,	/* WAN status changed
											 * priv means RSL_VOIP_INTF_STSCODE
											 * content means IP address
											 */
	CMSG_VOIP_CONFIG_CHANGED 		= 2802,
	CMSG_VOIP_RESTART_CALLMGR 		= 2803,
	CMSG_VOIP_CONFIG_WRITTEN 		= 2804,	/* notice VoIP CM writing flash */
	CMSG_VOIP_FLASH_WRITTEN 		= 2805,	/* notice VoIP CM that flash has been written */
	CMSG_VOIP_STATISTICS_RESET 		= 2806, 
#ifdef INCLUDE_USB_VOICEMAIL	
	CMSG_VOIP_UVM_USING_RECORDED_FILE	= 2807,		/* is USB voicemail using the recored file ? */
	CMSG_VOIP_USB_MOUNT_NEW = 2808,				/* A new disk that mounted can be used by usbvm  */
	CMSG_VOIP_USB_UMOUNT_CHANGE = 2809,			/* Change to another disk's path that is effective*/
	CMSG_VOIP_USB_UMOUNT_NULL = 2810,			/* There is not any disk can be used by usbvm  */
	CMSG_FXS_SET_USBVM = 2811, 	
#endif /* INCLUDE_USB_VOICEMAIL */												 
#ifdef INCLUDE_CALLLOG
	CMSG_VOIP_CALLLOG_CLEAR = 2812,				/* Clear call log */
#endif
#endif /* INCLUDE_VOIP */
	/* end of voice process */

    /* Added by xcl, 21Sep11 */
    CMSG_SNMP_CFG_CHANGED 	= 2850,
    CMSG_SNMP_LINK_UP       = 2851,
    CMSG_SNMP_LINK_DOWN     = 2852,
    CMSG_SNMP_WAN_UP		= 2853,
    /* End added by xcl, 21Sep11 */

#ifdef INCLUDE_IPV6	/* Add by CM, 16Nov11 */
	CMSG_IPV6_PPP_STATUS	= 2900,
	CMSG_IPV6_DHCP6C_STATUS	= 2901,
	CMSG_IPV6_STATUS = 2902,

#ifdef INCLUDE_IPV6_MLD	/* Add by HYY: MLDv2 Proxy, 10Jul13 */
	CMSG_MLDPROXY_ADD_LAN_IF	= 2903,
	CMSG_MLDPROXY_ADD_WAN_IF	= 2904,
	CMSG_MLDPROXY_DEL_IF		= 2905,
#endif /* INCLUDE_IPV6_MLD */

#ifdef INCLUDE_TTNET
	CMSG_DNS6_PROBE_CFG = 2950,
	CMSG_DNS6_SERVER = 2951,
#endif	/* INCLUDE_TTNET */

#endif	/* INCLUDE_IPV6 */

#ifdef INCLUDE_IPSEC
	CMSG_IPSEC_CFG_CHANGED = 3000,
	CMSG_IPSEC_WAN_CHANGED = 3001,
#endif

#ifdef INCLUDE_IGMP_DIAG /* ZJ, 27Dec2013 */
	CMSG_IGMPDIAG_CFG_MSG	= 3003,
#endif /* INCLUDE_IGMP_DIAG */


/* Add by zjj, 20120703, for usb 3g handle card event */
#ifdef INCLUDE_USB_3G_DONGLE
	CMSG_USB_3G_HANDLE_EVENT = 3100,
	CMSG_USB_3G_BACKUP		 = 3101,
#ifdef INCLUDE_3GBACKUP_PENDING_WITH_VOIP
	CMSG_USB_3G_BACKUP_PENDING,
#endif /* INCLUDE_3GBACKUP_PENDING_WITH_VOIP */

#ifdef INCLUDE_VOIP
	CMSG_FXS_SET_PIN_PUK,
#endif /* INCLUDE_VOIP */

#endif /* INCLUDE_USB_3G_DONGLE */

/* Add by zjj, 20130726, for samba notification when lan ip changed. */
#ifdef INCLUDE_USB_SAMBA_SERVER
	CMSG_USB_SMB_LAN_IP_CHANGED = 3200,
#endif /* INCLUDE_USB_SAMBA_SERVER */

#ifdef INCLUDE_VDSLWAN
	CMSG_DSL_TYPE_CHANGED = 3300,
#endif /* INCLUDE_VDSLWAN */


/*<< BosaZhong@20Sep2012, add, for SMP system. */
#ifdef SOCKET_LOCK 
	CMSG_SOCKET_LOCK_P                  = 4000,
	CMSG_SOCKET_LOCK_V                  = 4001,
	CMSG_SOCKET_LOCK_PROBE_DEAD_PROCESS = 4002,
#endif /* SOCKET_LOCK */
/*>> endof BosaZhong@20Sep2012, add, for SMP system. */

	/* Added by Wang Jianfeng, 2013.12.13, for webWarn */
#ifdef INCLUDE_WEB_WARN
	CMSG_WLAN_SECURITY_CHANGE = 4008,
	CMSG_DSL_STATUS_CHANGE = 4009,
	CMSG_INTF_GROUPING_CHANGE = 4010,
	CMSG_CURR_LANGUAGE_CHANGE = 4011,
	CMSG_EWAN_STATUS_CHANGE = 4012,
	CMSG_3G_STATUS_CHANGE = 4013,
	CMSG_Default_Conn_CHANGE = 4014,
#endif /* INCLUDE_WEB_WARN */
	/* end of added by Wang Jianfeng, for webWarn */
	
#ifdef INCLUDE_AUTO_FTP_UPDATE
	CMSG_FIRMWARE_DO_UPDATE = 4020,
	CMSG_FIRMWARE_SKIP_UPDATE = 4021,
	CMSG_AUTO_UPDATE_INFO = 4022,
	CMSG_FIRMWARE_UPDATE = 4023,
	CMSG_TTNET_FTP_START = 4024,
	CMSG_TTNET_FTP_STOP = 4025,
#endif /* INCLUDE_AUTO_FTP_UPDATE */

#ifdef INCLUDE_WAN_DETECT
	CMSG_WAN_DETECT_RESULT = 4101,
#endif

/* added by huangzhida 24Dec13 , for diagnostics*/
#ifdef INCLUDE_TR143
	CMSG_CWMP_TR143_START = 4206,
	CMSG_CWMP_TR143_COMPLETE = 4207,
#endif /* INCLUDE_TR143 */

#ifdef INCLUDE_IPPING_DIAG        
	CMSG_IPPING_CFG_MSG	= 4201,
#endif /* INCLUDE_IPPING_DIAG */
#ifdef INCLUDE_TRACEROUTE_DIAG
	CMSG_TRACEROUTE_CFG_MSG = 4202,
#endif /* INCLUDE_TRACEROUTE_DIAG */
#ifdef INCLUDE_DOWNLOAD_DIAG
	CMSG_DOWNLOAD_CFG_MSG = 4203,
#endif /* INCLUDE_DOWNLOAD_DIAG */
#ifdef INCLUDE_UPLOAD_DIAG
	CMSG_UPLOAD_CFG_MSG = 4204,
#endif /* INCLUDE_UPLOAD_DIAG */
#ifdef INCLUDE_UDPECHO_DIAG
	CMSG_UDPECHO_CFG_MSG = 4205,
#endif /* INCLUDE_UDPECHO_DIAG */

/*endof added by huangzhida*/

#if defined(INCLUDE_TTNET) || defined(INCLUDE_SOL)
	CMSG_HOMEPLUG_DETECT = 5005,	/* add listen dev for detect daemon */
	CMSG_HOMEPLUG_ADD_PORT = 5006,	/* add a home plug port */
	CMSG_HOMEPLUG_DEL_PORT = 5007,	/* del a home plug port */
	CMSG_DHCPS_PORT_STATUS = 5008,
	CMSG_HOMEPLUG_ADD_HOST = 5009,
	CMSG_HOMEPLUG_DEL_HOST = 5010,
#endif	/* INCLUDE_TTNET or INCLUDE_SOL */

#if defined(INCLUDE_TR111_PART1) || defined(INCLUDE_SOL)
	CMSG_DHCPS_VIVSIO_CFG = 5001,
	CMSG_DHCPC_VIVSIO_CFG = 5002,
#endif /*INCLUDE_TR111_PART1 or INCLUDE_SOL*/

#ifdef INCLUDE_TR111_PART1
	CMSG_DHCPS_ADD_MANAGE_DEV = 5003,
	CMSG_DHCPS_DEL_MANAGE_DEV = 5004,
#endif /* INCLUDE_TR111_PART1 */


#ifdef INCLUDE_PORT_MIRRORING
	CMSG_START_PORT_MIRROR = 5011,
	CMSG_STOP_PORT_MIRROR = 5012,
#endif /*INCLUDE_PORT_MIRRORING*/
#ifdef INCLUDE_TTNET
	/*added by xianjiantao for 9970 ttnet 3.0,17-11-20*/
	CMSG_SET_CONSOLE_PID = 5013,
	CMSG_KILL_CONSOLE = 5014,
	/*added end xianjiantao*/
#endif


/* added by wwj for tr64 app msg */
#ifdef INCLUDE_TR064
	CMSG_TR64_IPCMSG        = 6000,
#endif /* INCLUDE_TR064 */
	
/* Add ttnet macro by ranjie 20160930 */
#if defined(INCLUDE_USB_3G_DONGLE) && (defined(INCLUDE_SOL) || defined(INCLUDE_TTNET))
	CMSG_3G_DISCONN_WARN = 6100,
#endif
	
#ifdef INCLUDE_DSL_DIAG
	CMSG_DSL_DIAG_COMPLETE  = 7000,
#endif /* INCLUDE_DSL_DIAG */

#ifdef INCLUDE_FON
	CMSG_FON_WAN_UP         = 7001,    /* added by jy for fon init. */
	CMSG_FON_SSID_UP		= 7002,
#endif /*INCLUDE_FON*/

#ifdef INCLUDE_PKTCAP
	CMSG_PKTCAP_START_CAPTURE  = 8000,
	CMSG_PKTCAP_STOP_CAPTURE = 8001,
#if 0
	CMSG_PKTCAP_GET_STATUS   = 8002,
	CMSG_PKTCAP_INIT_ENGINE  = 8003,
	CMSG_PKTCAP_UI_START	 = 8004,
	CMSG_PKTCAP_SET_STATUS	 = 8005,
	CMSG_PKTCAP_UPDATE_LEFTIME = 8006,
#else
	CMSG_PKTCAP_SET_STATUS	 = 8002,
	CMSG_PKTCAP_UPDATE_LEFTIME = 8003,
#endif
#endif /* INCLUDE_PKTCAP */

	/*modify by xjt,16-08-26*/
#ifdef INCLUDE_TT_SYSLOG
	CMSG_SYSLOG_TIMER_RUN = 9000,
	CMSG_SYSLOG_TIMER_TERMINATE = 9001,
#endif /* INCLUDE_TT_SYSLOG */

#if defined(INCLUDE_TT_SYSLOG) || defined(INCLUDE_PKTCAP_UPLOAD)
	CMSG_UPLOAD_START = 10000,
	CMSG_UPLOAD_STATE_CHANGE = 10001,
#endif	/*INCLUDE_TT_SYSLOG*/

#ifdef INCLUDE_TTNET
	CMSG_RESET_BTN_PRESSED = 10002,
#endif /* INCLUDE_TTNET */
	/*end modify xjt*/

}CMSG_TYPE;


/* 
 * brief	Message struct
 */
typedef struct
{
	CMSG_TYPE type;		/* specifies what message this is */
	unsigned int priv;		/* private data, one word of user data etc. */
	unsigned char content[MSG_CONTNET_SIZE];
}CMSG_BUFF;


/* 
 * brief	Message type identification	
 */
typedef enum
{
	CMSG_ID_NULL = 5,	/* start from 5 */
	CMSG_ID_COS = 6,
	CMSG_ID_SNTP = 7,
	CMSG_ID_HTTP = 8,
	CMSG_ID_DNS_PROXY = 9,
	CMSG_ID_DHCPS = 10, 	/* Added by xcl, 2011-06-13.*/
	CMSG_ID_DDNS_PH = 11,  	/* addde by tyz, 2011-07-21 */
	CMSG_ID_PH_RT = 12,
	CMSG_ID_CLI = 13,
	CMSG_ID_DHCPC = 14, 
	CMSG_ID_UPNP =15, 	/* Added  by  Li Chenglong , 11-Jul-31.*/
	CMSG_ID_DIAGTOOL =16, /*Added by Yuan Shang, 2011-08-18 */
	CMSG_ID_CWMP = 17, /* add by wuzhiqin, 2011-09-26 */
	CMSG_ID_SNMP = 18, /* Added by xcl, 21Sep11 */
	CMSG_ID_IGMP = 19,	/* Added by Wang Wenhao, 2011-11-18 */

#ifdef INCLUDE_VOIP
	CMSG_ID_VOIP = 20,  /* for voice process, added by zhonglianbo 2011-8-10 */
#endif /* INCLUDE_VOIP */
	CMSG_ID_DYNDNS = 21, /* Added by xcl, 24Nov11 */
	
	/* Added by LI CHENGLONG , 2011-Dec-15.*/
	CMSG_ID_DLNA_MEDIA_SERVER = 22,
	/* Ended by LI CHENGLONG , 2011-Dec-15.*/

	CMSG_ID_NOIPDNS = 23, /*added by tpj, 2012-2-1*/
	
	CMSG_ID_IPSEC = 24,

	CMSG_ID_LOG = 25,	/* added by yangxv, 2012.12.25, log message no longer shared cos_passive */

#ifdef SOCKET_LOCK
	CMSG_ID_SOCKET_LOCK = 26, /* BosaZhong@20Sep2012, add, for SMP system. */
	CMSG_ID_SOCKET_LOCK_ACCEPT = 27, /* BosaZhong@21Sep2012, add, for SMP system. */
#endif /* SOCKET_LOCK */

#ifdef INCLUDE_IPV6_MLD	/* Add by HYY: MLDv2 Proxy, 01Jul13 */
	CMSG_ID_MLD	= 28,
#endif /* INCLUDE_IPV6_MLD */

#ifdef INCLUDE_TR143    /*added by huangzhida, 2014-1-15*/
	CMSG_ID_TR143 = 29,
#endif /* INCLUDE_TR143 */

#ifdef INCLUDE_IGMP_DIAG /* ZJ, 27Dec2013 */
	CMSG_ID_IGMPDIAG = 30,
#endif /* INCLUDE_IGMP_DIAG */

#ifdef INCLUDE_TTNET
	CMSG_ID_TR64 = 31, /* added by wwj for ttnet tr64 app */
#endif /* INCLUDE_TTNET */

	/* Added by Wang Jianfeng, 2013.12.13, for webWarn */
#ifdef INCLUDE_WEB_WARN
	CMSG_ID_WEB_WARN = 32,
#endif /* INCLUDE_WEB_WARN */
	/* end of added by Wang Jianfeng, for webWarn*/

#ifdef INCLUDE_AUTO_FTP_UPDATE
	CMSG_ID_TTNET_FTP = 33,
#endif /* INCLUDE_AUTO_FTP_UPDATE */

#if defined(INCLUDE_TTNET) || defined(INCLUDE_SOL)
		CMSG_ID_HOMEPLUG_DETECT = 34,
#endif	/* INCLUDE_TTNET */

	CMSG_ID_DDNS_UD = 35, /* added by zj, for userdefine ddns, 28May14 */

#ifdef INCLUDE_TTNET
	CMSG_ID_PING = 36,
#endif	/* INCLUDE_TTNET */

#ifdef INCLUDE_PKTCAP
	CMSG_ID_PKTCAP = 37,
#endif /* INCLUDE_PKTCAP */

#ifdef INCLUDE_PORT_MIRRORING
	CMSG_ID_PORTMIRROR_TIMER = 38,
#endif

#ifdef INCLUDE_TT_SYSLOG
	CMSG_ID_SYSLOG_TIMER = 39,
#endif

#if defined(INCLUDE_TT_SYSLOG) || defined(INCLUDE_PKTCAP_UPLOAD)
	CMSG_ID_UPLOAD = 40,
#endif
	
#ifdef INCLUDE_TTNET
	CMSG_ID_CHECKRESETBTN = 41,
	/*added by xianjiantao for 9970 ttnet 3.0,17-12-24*/
	CMSG_ID_IP_DOMAIN_TEST = 43,
	CMSG_ID_CONNECTION_DIAG_TEST = 44,
	/*added end xianjiantao*/
#endif /* INCLUDE_TTNET */

	CMSG_ID_MAX,	
}CMSG_ID;


/* for all message type 
 * NOTE : DO NOT USE the type likes UINT8
 */

/* 
 * brief	CMSG_SNTP_CFG message type content
 */
typedef struct
{
	char   	ntpServers[SNTP_DM_SERVER_NUM][32];
#ifdef INCLUDE_NTP_BIND
	char    ifname[16];
#endif /* INCLUDE_NTP_BIND */
	unsigned int primaryDns;
	unsigned int secondaryDns;
	unsigned int timeZone;
}SNTP_CFG_MSG;

/* Added by LI CHENGLONG , 2011-Dec-15.*/

/* 
 * brief: Added by LI CHENGLONG, 2011-Nov-21.
 * Vendor related message
 * 		INIT message is sent to DLNA_MEDIA_SERVER process if it is running
 *		DLNA_MEDIA_SERVER process advertise ssdp message including vendor's information
 */
typedef struct _MANUFACT_SPEC_INFO
{
	char 	devManufacturerURL[64];
	char 	manufacturer[64];
	char 	modelName[64];
	char 	devModelVersion[16];
	char 	description[256];
}MANUFACT_SPEC_INFO;


/* 
 * brief: Added by LI CHENGLONG, 2011-Dec-15.
 *		describe the structure of a shared folder 
 */
typedef struct _DMS_FOLDER_INFO
{
	char	dispName[32];
	char 	path[128];
}DMS_FOLDER_INFO;


/* 
 * brief: Added by LI CHENGLONG, 2011-Dec-16.
 *		  operating type to a folder
 */
typedef enum _DMS_FOLDER_OP
{
	DMS_INIT_FOLDER = 0,
	DMS_DEL_FOLDER = 1,
	DMS_ADD_FOLDER = 2
}DMS_FOLDER_OP;

/* 
 * brief: Added by LI CHENGLONG, 2011-Dec-15.
 *		  configuration is sent to DLNA_MEDIA_SERVER after it starts up
 */
typedef struct _DMS_INIT_INFO_MSG
{
	unsigned char		serverState;			/* ServerState */
	char				serverName[16];
	unsigned char		scanFlag;				/*scan*/
	unsigned int		scanInterval;		/*scan interval*/
	MANUFACT_SPEC_INFO	manuInfo;				/* different OEM factorys' information */
	unsigned int		folderCnt;			/*how many folde is shared now*/
}DMS_INIT_INFO_MSG;

/* 
 * brief: Added by LI CHENGLONG, 2011-Dec-15.
 *		  configuration is updated for DLNA_MEDIA_SERVER
 */
typedef struct _DMS_RELOAD_MSG
{
	unsigned char		serverState;			/* ServerState */
	char				serverName[16];
	unsigned char		scanFlag;				/*scan*/
	unsigned int		scanInterval;		/*scan interval*/	
}DMS_RELOAD_MSG;

/* 
 * brief: Added by LI CHENGLONG, 2011-Dec-16.
  */
typedef struct _DMS_OP_FOLDER_MSG
{
	 DMS_FOLDER_OP			op;
	 DMS_FOLDER_INFO		folder;
}DMS_OP_FOLDER_MSG;

/* Ended by LI CHENGLONG , 2011-Dec-15.*/


/* 
 * brief	CMSG_ID_CLI message type content
 */
typedef struct _CLI_USR_CFG_MSG
{
	char   		rootName[64];	/* RootName */
	char   		rootPwd[64];	/* RootPwd */
	char   		adminName[64];	/* AdminName */
	char   		adminPwd[64];	/* AdminPwd */
	char   		userName[64];	/* UserName */
	char   		userPwd[64];	/* UserPwd */
	char		manufact[64];/* Added by Li Chenglong , 2011-Oct-12.*/
}CLI_USR_CFG_MSG;


/* 
 * brief: Added by Li Chenglong, 11-Jul-31.
 *		  UPnP enable message
 */
typedef struct _UPNP_ENABLE_MSG
{
	unsigned int enable; 
}UPNP_ENABLE_MSG;

/* Add by chz, 2012-12-24 */
typedef struct _UPNP_DEL_MSG
{
	unsigned int port;
	char protocol[16];
}UPNP_DEL_MSG;
/* end add */

/*added by xianjiantao for vr600 tt v1,17-05-19*/
#ifdef INCLUDE_TTNET
typedef struct _UPNP_ENTRY_ENABLE_MSG
{
	char enable;
	unsigned int port;
	char protocol[16];
}UPNP_ENTRY_ENABLE_MSG;
#endif
/*added end xianjiantao*/


/* 
 * brief: Added by Li Chenglong, 11-Jul-31.
 *		  gateway changed
 */
typedef struct _UPNP_DEFAULT_GW_CH_MSG
{
	char gwName[16];
	char gwAddr[16];
	unsigned char natEnabled;
	unsigned char upDown;
}UPNP_DEFAULT_GW_CH_MSG;


/* 
 * brief CMSG_DNS_PROXY_CFG	message type content
 */
typedef struct
{
	unsigned int primaryDns;
	unsigned int secondaryDns;
}DNS_PROXY_CFG_MSG;

/* Added by Zeng Yi. 2011-07-08 */
typedef struct
{
	unsigned char isError;
#ifdef INCLUDE_LAN_WLAN_DUALBAND
	char iface[16];
#endif
	char SSID[34];
	int authMode;
	int encryMode;
	char key[65];
}WPS_CFG_MSG;
/* End added. Zeng Yi. */

/* Added by Yang Caiyong for PPP connection status changed, 2011-07-18 */
typedef struct _PPP_CFG_MSG
{
	unsigned int pppDevUnit;
	char connectionStatus[18];
	unsigned int pppLocalIp;
	unsigned int pppSvrIp;
	unsigned int uptime;
	char lastConnectionError[32];
	unsigned int dnsSvrs[2];
#ifdef INCLUDE_TTNET
	int currentMru;
#endif	/* INCLUDE_TTNET */
	/* save current session ID and srver MAC. Added by Wang Jianfeng 2014-05-06*/
	char peerETH[18];
	unsigned short sessionID;
}PPP_CFG_MSG;
/* YCY adds end */

/* Added by whb, 2013-03-27. */
typedef struct _IP_CFG_MSG
{
	char connectionStatus[18];
	char connName[32];
/* Add by RanJie, 23Mar2017.
 * Meeting TTNET's new requirement: build-in connection should be able to show&change all connection type.
 * There can be at most two different type of WAN_IP_CONN obj exist in dm. (one is bridge, one is dynamicIp/staticIp).
 * Use connection type to distinguish them.
 */	
#ifdef INCLUDE_TTNET
	char connectionType[14];
#endif /* INCLUDE_TTNET */
/* End add */
}IP_CFG_MSG;

/* Added by xcl, 2011-07-25 */
typedef struct 
{
    unsigned int delLanIp;
    unsigned int delLanMask;
}DHCPS_RELOAD_MSG_BODY;

#ifdef INCLUDE_IPV6
/* Add by HYY: instead of struct in6_addr in <netinet/in.h>, 17Jul13 */
typedef struct
{
	unsigned char in6Addr[16];
}IN6_ADDR;

/* Add by HYY: support dynamic 6RD, 20Mar12 */
typedef struct
{
	unsigned char ipv4MaskLen;
	unsigned char sit6rdPrefixLen;
	IN6_ADDR sit6rdPrefix;
	unsigned int sit6rdBRIPv4Addr;
}DHCPC_6RD_INFO;

#ifdef INCLUDE_TTNET
typedef struct
{
	IN6_ADDR primaryDnsv6;
	IN6_ADDR secondaryDnsv6;
}DNS6_PROBE_CFG_MSG;
#endif	/* INCLUDE_TTNET */

#endif /* INCLUDE_IPV6 */

/* Add by chz for L2TP/PPTP, 2013-04-24 */
#undef INCLUDE_L2TP_OR_PPTP

#ifdef INCLUDE_L2TP
#define INCLUDE_L2TP_OR_PPTP 1
#endif

#ifdef INCLUDE_PPTP
#define INCLUDE_L2TP_OR_PPTP 1
#endif

#ifdef INCLUDE_L2TP_OR_PPTP
typedef enum
{
	DHCP_IP = 0,
	DHCP_L2TP  = 1,
	DHCP_PPTP = 2
}IP_CONN_DHCP_TYPE;
#endif
/* end add */

typedef struct 
{
    unsigned char status; /* Have we been assigned an IP address ? */
    char ifName[16];  
    unsigned int ip;
    unsigned int mask;
    unsigned int gateway;
    unsigned int dns[2];
#ifdef INCLUDE_IPV6	/* Add by HYY: support dynamic 6RD, 20Mar12 */
	DHCPC_6RD_INFO sit6rdInfo;
#endif /* INCLUDE_IPV6 */
/* Add by chz for L2TP/PPTP, 2013-04-24 */
#ifdef INCLUDE_L2TP_OR_PPTP
	IP_CONN_DHCP_TYPE connType;
#endif
/* end add */
#if defined(INCLUDE_TTNET) || defined(INCLUDE_SOL)
	unsigned int uptime;
#endif	/* INCLUDE_TTNET || INCLUDE_SOL */
}DHCPC_INFO_MSG_BODY;

typedef struct 
{
    unsigned char unicast;
    char ifName[16];
    char hostName[64];
#ifdef INCLUDE_IPV6	/* Add by HYY: support dynamic 6RD, 19Mar12 */
	unsigned char sit6rdEnabled;
#endif /* INCLUDE_IPV6 */
/* Add by chz for L2TP/PPTP, 2013-04-24 */
#ifdef INCLUDE_L2TP_OR_PPTP
	IP_CONN_DHCP_TYPE connType;
#endif
/* end add */
/* yanglx, 2015-8-7, port from 9980 */
#ifdef INCLUDE_MER
	unsigned int merEnabled;
    char merString[WAN_IP_CONN_X_TP_USERNAME_L + WAN_IP_CONN_X_TP_PASSWORD_L + 1];
#endif
/* end, yanglx */
#ifdef INCLUDE_TTNET
	char op60VendorId[32];
	char name[32];
	int vid;
#endif
}DHCPC_CFG_MSG_BODY;
/* End added by xcl, 2011-07-25 */

/* Added by tyz 2011-08-02 (n & n) */
/* the msg of the interface */
typedef struct
{
	int ifUp;
	unsigned int ip;
	unsigned int gateway;
	unsigned int mask;
	unsigned int dns[2];
	char ifName[16];
}DDNS_RT_CHAGED_MSG;

/*
the msg of the ph running time
*/
typedef struct 
{
	unsigned char state;
	unsigned char sevType;
	unsigned short isEnd;
}DDNS_RT_PRIV_MSG;
/*
the msg of the cfg 
*/
typedef struct
{	
	int enabled;
	int reg;
	int userLen;
	char phUserName[64];
	int pwdLen;
	char phPwd[64];	
}DDNS_PH_CFG_MSG;

/* Added by xcl, 24Nov11 */
/* dynDns config msg struct */
typedef struct 
{
    unsigned char   enable;
    char            userName[64];
    char            password[64];
    char            domain[128];
    char            server[64];
    unsigned char   login;
}DYN_DNS_CFG_MSG;

typedef struct 
{
    unsigned int state;
}DYN_DNS_STATE_MSG;
/* End added by xcl */

/* Added by tpj, 17Jan12 */
/* noipDns config msg struct */
typedef struct 
{
    unsigned char   enable;
    char            userName[64];
    char            password[64];
    char            domain[128];
    char            server[64];
    unsigned char   login;
}NOIP_DNS_CFG_MSG;

typedef struct 
{
    unsigned int state;
}NOIP_DNS_STATE_MSG;
/* End added by tpj */

/* Add by ZJ, 28May14 */
#ifdef INCLUDE_DDNS_USERDEFINE
typedef struct 
{
    unsigned char   enable;
    unsigned char   login;
	unsigned short	IPStartOffset;
	unsigned short	IPEndOffset;
	char			grabServer[USERDEFINE_DDNS_CFG_SERVER_L];
	char			grabAuth[USERDEFINE_DDNS_CFG_SERVER_L];
    char            grabRequest[USERDEFINE_DDNS_CFG_GRABREQUEST_L];
}DDNS_UD_CFG_MSG;
#endif /* INCLUDE_DDNS_USERDEFINE */
/* End add */


typedef struct
{
	unsigned int command;
	char host[32];
#ifdef INCLUDE_TTNET
	char dns1[32];
	char dns2[32];
	unsigned int repliNum;
	unsigned int succNum;
	char srv1[128];
	char srv2[128];
	char ipaddr1[32];
	char ipaddr2[32];
	char ifname[16];
	char gw[16];
	unsigned char bMark;
#endif /* INCLUDE_TTNET */
	unsigned int result;
}DIAG_COMMAND_MSG;

/* end (n & n) */
typedef struct
{
	unsigned char vpi;
	unsigned short vci;
	char connName[32];
}AUTO_PVC_MSG;

/* Added by xcl, 17Oct11, snmp msg struct */
typedef struct 
{
    unsigned short ifIndex;
}SNMP_LINK_STAUS_CHANGED_MSG;
/* End added */

#ifdef INCLUDE_IPV6	/* Add by HYY: IPv6 support, 16Nov11 */
typedef struct _PPP6_CFG_MSG
{
	unsigned int pppDevUnit;
	unsigned char pppIPv6CPUp;
	unsigned long long remoteID;
	unsigned long long localID;
}PPP6_CFG_MSG;

typedef struct 
{
	IN6_ADDR addr;
	unsigned int pltime;
	unsigned int vltime;
	unsigned char plen;
}DHCP6C_IP_INFO;

typedef struct 
{
	unsigned char status;
	char ifName[16];  
	DHCP6C_IP_INFO ip;
	DHCP6C_IP_INFO prefix;
	IN6_ADDR dns[2];
	IN6_ADDR dsliteAddr;   /* Add by YuanRui: support DS-Lite, 21Mar12 */
}DHCP6C_INFO_MSG_BODY;
#endif /* INCLUDE_IPV6 */

#ifdef INCLUDE_IPSEC
typedef struct
{
	unsigned short currDepth;								
	unsigned short numInstance[6];	
}IPSEC_OLD_NUM_STACK;

typedef struct 
{
	int state;
    char default_gw_ip[16];
	/*added by xianjiantao for 9970 ttnet 3.0*/
#ifdef INCLUDE_TTNET
	char ifname[16];
#endif
	/*added end xianjiantao*/
}IPSEC_WAN_STATE_CHANGED_MSG;

typedef struct
{
	char local_ip[16];
	char local_mask[16];
	unsigned int  local_ip_mode;
	char remote_ip[16];
	char remote_mask[16];
	unsigned int  remote_ip_mode;
	char real_remote_gw_ip[16];
	char spi[16];
	char second_spi[16];
	unsigned int entryID;
	unsigned int  op;
	unsigned char  enable;
	unsigned int key_ex_type; /*Added for vxWorks*/
	IPSEC_OLD_NUM_STACK stack;
}IPSEC_CFG_CHANGED_MSG;
#endif

#ifdef INCLUDE_VDSLWAN
typedef enum
{
	CMSG_DSL_SWITCH_TO_ADSL = 0,
	CMSG_DSL_SWITCH_TO_VDSL = 1,
}CMSG_DSL_TYPE;

typedef struct 
{
	CMSG_DSL_TYPE dslType;
}DSL_TYPE_SWITCH_MSG;
#endif /* INCLUDE_VDSLWAN */

/* Added by huangzhida 06Jan14  */
#ifdef INCLUDE_IPPING_DIAG
typedef struct _IPPING_PARAM_MSG
{
	unsigned int    DSCP;	/* DSCP */
	unsigned int    dataBlockSize;	/* DataBlockSize */
	unsigned int    timeout;	/* Timeout */
	unsigned int    numberOfRepetitions;	/* NumberOfRepetitions */
	char 			ifName[16];
	char 	    	host[256];
	char			dns[64];
	char 			diagnosticsState[28];
}IPPING_PARAM_MSG;

typedef struct _IPPING_RESULT_MSG
{
	unsigned int    maximumResponseTime;	/* MaximumResponseTime */
	unsigned int    minimumResponseTime;	/* MinimumResponseTime */
	unsigned int    averageResponseTime;	/* AverageResponseTime */
	unsigned int    failureCount;	/* FailureCount */
	unsigned int    successCount;	/* SuccessCount */
	char   			diagnosticsState[28];	/* DiagnosticsState */
}IPPING_RESULT_MSG;
#endif /* INCLUDE_IPPING_DIAG */

#ifdef INCLUDE_TRACEROUTE_DIAG
typedef struct _TRACEROUTE_PARAM_MSG
{
	char 			diagnosticsState[28];	/* DiagnosticsState */
	char			ifName[16];			/* Interface */
	char			host[256];			/* Host */
	unsigned int 	numberOfTries;		/* NumberOfTries */
	unsigned int 	timeout;			/* Timeout */
	unsigned int 	dataBlockSize;		/* DataBlockSize */
	unsigned int 	DSCP;				/* DSCP */
	unsigned int 	maxHopCount;		/* MaxHopCount */	
	char			dns[64];
}TRACEROUTE_PARAM_MSG;

typedef struct _TRACEROUTE_RESULT_MSG
{
	unsigned int 	responseTime;		/* ResponseTime */
	unsigned int 	routeHopsNumberOfEntries;	/* RouteHopsNumberOfEntries */
	unsigned int 	hopErrorCode;		/* HopErrorCode */
	char			hopHost[256];		/* HopHost */
	char			hopHostAddress[16]; /* HopHostAddress */
	char			hopRTTimes[16];		/* HopRTTimes */
	char 			diagnosticsState[28];	/* DiagnosticsState */
	char			X_TP_Finish;			/* X_TP_Finish */
}TRACEROUTE_RESULT_MSG;
#endif /* INCLUDE_TRACEROUTE_DIAG */

#if defined(INCLUDE_DOWNLOAD_DIAG) || defined(INCLUDE_UPLOAD_DIAG)
typedef struct _LOAD_PARAM_MSG
{
	unsigned int 	DSCP;				/* DSCP */
	unsigned int 	ethernetPriority;	/* EthernetPriority */
	unsigned int 	testFileLength;		/* testFileLength */
	unsigned int 	timeBasedTestDuration;	   /*timeBasedTestDuration*/
	char			ifName[16];		
	char			url[256];			/* DownloadURL */
	char			dns[64];
	char			diagnosticsState[28];/* DiagnosticsState */
}LOAD_PARAM_MSG;

typedef struct _LOAD_RESULT_MSG
{
	unsigned int 	testBytes;			/* TestBytes */
	unsigned int 	totalBytes;			/* TotalBytes */
	char 			ROMTime[32];	/* ROMTime */
	char 			BOMTime[32];	/* BOMTime */
	char 			EOMTime[32];	/* EOMTime */
	char 			TCPOpenRequestTime[32];		/* TCPOpenRequestTime */
	char 			TCPOpenResponseTime[32];	/* TCPOpenResponseTime */
	char			diagnosticsState[28];/* DiagnosticsState */
}LOAD_RESULT_MSG;
#endif /* defined(INCLUDE_DOWNLOAD_DIAG) || defined(INCLUDE_UPLOAD_DIAG) */

#ifdef INCLUDE_UDPECHO_DIAG
typedef struct _UDPECHO_PARAM_MSG
{
	char				ifName[16];;		/* Interface */
	char				sourceIPAddress[16];/* SourceIPAddress */
	unsigned int 		udpPort;			/* UDPPort */
	unsigned char		echoPlusEnabled;	/* EchoPlusEnabled */
	unsigned char		enable;				/* Enable */
}UDPECHO_PARAM_MSG;

typedef struct _UDPECHO_RESULT_MSG
{
	unsigned int 	packetsReceived;	/* PacketsReceived */
	unsigned int 	packetResponsed;	/* PacketResponsed */
	unsigned int 	bytesReceived;		/* BytesReceived */
	unsigned int 	bytesResponsed;		/* BytesResponsed */
	char			timeFirstPacketReceived[32];	/* TimeFirstPacketReceived */
	char			timeLastPacketReceived[32];		/* TimeLastPacketReceived */
	unsigned char	echoPlusSupported;				/* EchoPlusSupported */
}UDPECHO_RESULT_MSG;
#endif /* INCLUDE_UDPECHO_DIAG */
/* end of added by huangzhida  */

#ifdef INCLUDE_IGMP_DIAG /* ZJ, 27Dec2013 */
typedef struct _IGMPDIAG_CFG_MSG
{
	char			diagnosticsState[15];
	char			channelAddr[16];
	char			channelPort[6];
	char			channelStatus[5];
	char			serverAddr[16];
	char			ifName[16];
	unsigned int	timeDelay;
	unsigned int	numJoins;
	unsigned int	numLeaves;
	unsigned int	joinDelay;
	int	timeOut;
	unsigned int	phase;
}IGMPDIAG_CFG_MSG;
#endif /* INCLUDE_IGMP_DIAG */

/* Added by Wang Jianfeng, 2013.12.23, for webWarn */
#ifdef INCLUDE_WEB_WARN
typedef struct __LAN_IP_INTERFACE
{
    char IPInterfaceIPAddress[16];
	char IPInterfaceSubnetMask[16];
	char __ifName[16];
} LAN_IP_INTERFACE;

typedef struct __INTERFACE_CFG
{
	char name[16];
} INTERFACE_CFG;
#endif /* INCLUDE_WEB_WARN */
/* end of added by Wang Jianfeng, for webWarn */

#ifdef INCLUDE_AUTO_FTP_UPDATE
/* for ftp update daemon */
typedef struct _TTNET_FTP_CFG_MSG
{
	unsigned int primaryDns;
	unsigned int secondaryDns;
	char modelName[64];
	char additionalHardwareVersion[64];
	unsigned int productVersion;
	unsigned int productId;
	unsigned int softRevision;
}TTNET_FTP_CFG_MSG;

/* this message used for process to get ftp auto update info */
typedef struct _FTP_AUTO_UPDATE_INFO
{
	char serverURL[64];
	char ftpUsrName[32];
	char ftpPwd[32];
	char modelName[64];
}FTP_AUTO_UPDATE_INFO;

#endif /* INCLUDE_AUTO_FTP_UPDATE */

#if defined(INCLUDE_TR111_PART1) || defined(INCLUDE_SOL)
/* for dhcp option 125 config */
typedef struct _DHCP_VIVSIO_CFG_MSG
{
	char manufacturerOUI[7];
	char productClass[64];
	char serialNumber[64];

	/*add by huangjx,2014-12-22*/
	unsigned int ifIp;
	char portName[16];
	unsigned int hostIp;
	char mac[18];
		
}DHCP_VIVSIO_CFG_MSG;


typedef struct _ARP_CFG_INFO
{
	unsigned int ifIp;
	char portName[16];
	unsigned hostIp;

	char mac[18];
}ARP_CFG_INFO;
#endif	/* INCLUDE_TR111_PART1 or INCLUDE_SOL */

#if defined(INCLUDE_TTNET) && defined(INCLUDE_TR111_PART1)
typedef struct _Tr111Dev_OfferedAddr {
	char	ifName[16];
	unsigned int yiaddr;
	char chaddr[18];
}Tr111DevOfferedAddr;
#endif


#ifdef INCLUDE_TTNET

enum DHCPS_HOST_TYPE
{
	HOST_ETHERNET,
	HOST_WIRELESS,
	HOST_HOMEPLUG,
	HOST_OTHER
};

typedef struct _HTTPD_XTTNET_USER_CFG_MSG
{
	unsigned char enLocal;
	unsigned char enLocalSecured;
	unsigned char enRemote;
	unsigned char enRemoteSecured;
	char username[64];
	char password[64];
}HTTPD_XTTNET_USER_CFG_MSG;

typedef struct _XTTNET_USER_AUTH_INFO_MSG
{
	char newUsername[64];
	char newPassword[64];
	char curUsername[64];
	char curPassword[64];
	int bUseDftPwd;
}XTTNET_USER_AUTH_INFO_MSG;

/*added by xianjiantao for hgw,17-07-11*/
typedef struct _XTTNET_LOCAL_DOMAIN_NAME_MSG
{
        char localDomainName[32];
}XTTNET_LOCAL_DOMAIN_NAME_MSG;
/*added end xianjiantao*/
/*added by xianjiantao for 9970 ttnet 3.0,17-12-24*/
typedef struct _IP_DOMAIN_TEST_MSG
{
	unsigned char type;
	char host[32];
	char intf[16];
	unsigned int priDns;
	unsigned int secDns;
}IP_DOMAIN_TEST_MSG;

typedef struct _CONNECTION_DIAG_TEST_MSG
{
	unsigned char type;
}CONNECTION_DIAG_TEST_MSG;

/*added end xianjiantao*/
#endif	/* INCLUDE_TTNET */

#ifdef INCLUDE_PKTCAP
typedef struct _PKTCAP_MSG_BODY
{
	char ifname[16];
	unsigned char bFilter;
	char filter[8][64];
	char direction[8];
	char storage[9];
	char directory[128];
	char fileName[64];
	unsigned int timeout;
}PKTCAP_MSG_BODY;

typedef struct _PKTCAP_STATUS_ERROR_MSG
{
	char realFileName[64];
	unsigned char errorNum;
}PKTCAP_STATUS_ERROR_MSG;
#endif /* INCLUDE_PKTCAP */

/*modify by xjt,16-08-26*/
#if defined(INCLUDE_TT_SYSLOG) || defined(INCLUDE_PKTCAP_UPLOAD)
typedef struct _UPLOAD_MSG_BODY
{
	char url[URL_MAX_PATH];
	char filePath[FILE_MAX_PATH];
	char username[32];
	char password[32];
	char protocol[8];
	char dns[64];
	int needTimer;
	int overwrite;
	unsigned int establishedTime;
	unsigned int requestOid;
}UPLOAD_MSG_BODY;

typedef struct _UPLOAD_STATE_CHANGE_MSG
{
	char newState[16];
	unsigned int establishedTime;
	unsigned int requestOid;
	unsigned char errorNum;
}UPLOAD_STATE_CHANGE_MSG;
#endif	/*INCLUDE_TT_SYSLOG || INCLUDE_PKTCAP_UPLOAD*/
/*end modify xjt*/

#ifdef INCLUDE_PORT_MIRRORING
typedef struct _PORT_MIRROR_CFG_MSG
{
	char monitorIntf[16 * 8];
	unsigned int lanPort;
	time_t timeout;
}PORT_MIRROR_CFG_MSG;
#endif /*INCLUDE_PORT_MIRRORING*/

#ifdef INCLUDE_TR143
typedef struct
{
	int  oid;
	int  moreFragFlag;
	char frag[1]; /* [PARAM_MSG_FRAG_SIZE] */
} DIAG_PARAM_MSG;
#define PARAM_MSG_FRAG_SIZE (MSG_CONTNET_SIZE - ((size_t)&((DIAG_PARAM_MSG *)0)->frag))
#endif /* INCLUDE_TR143 */

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/

/* 
 * fn		int msg_init(CMSG_FD *pMsgFd)
 * brief	Create an endpoint for msg
 *	
 * param[out]	pMsgFd - return msg descriptor that has been create	
 *
 * return	-1 is returned if an error occurs, otherwise is 0
 *
 * note 	Need call msg_cleanup() when you no longer use this msg which is created by msg_init()
 */
int msg_init(CMSG_FD *pMsgFd);


/* 
 * fn		int msg_srvInit(CMSG_ID msgId, CMSG_FD *pMsgFd)
 * brief	Init an endpoint as a server and bind a name to this endpoint msg	
 *
 * param[in]	msgId - server name	
 * param[in]	pMsgFd - server endpoint msg fd
 *
 * return	-1 is returned if an error occurs, otherwise is 0	
 */
int msg_srvInit(CMSG_ID msgId, CMSG_FD *pMsgFd);



/* 
 * fn		int msg_connSrv(CMSG_ID msgId, CMSG_FD *pMsgFd)
 * brief	Init an endpoint as a client and specify a server name	
 *
 * param[in]		msgId - server name that we want to connect	
 * param[in/out]	pMsgFd - client endpoint msg fd	
 *
 * return	-1 is returned if an error occurs, otherwise is 0
 */
int msg_connSrv(CMSG_ID msgId, CMSG_FD *pMsgFd);


/* 
 * fn		int msg_recv(const CMSG_FD *pMsgFd, CMSG_BUFF *pMsgBuff)
 * brief	Receive a message form a msg	
 *
 * param[in]	pMsgFd - msg fd that we want to receive message
 * param[out]	pMsgBuff - return recived message
 *
 * return	-1 is returned if an error occurs, otherwise is 0
 *
 * note		we will clear msg buffer before recv
 */
int msg_recv(const CMSG_FD *pMsgFd, CMSG_BUFF *pMsgBuff);


/* 
 * fn		int msg_send(const CMSG_FD *pMsgFd, const CMSG_BUFF *pMsgBuff)
 * brief	Send a message from a msg	
 *
 * param[in]	pMsgFd - msg fd that we want to send message	
 * param[in]	pMsgBuff - msg that we wnat to send
 *
 * return	-1 is returned if an error occurs, otherwise is 0
 *
 * note 	This function will while call sendto() if sendto() return ENOENT error
 */
int msg_send(const CMSG_FD *pMsgFd, const CMSG_BUFF *pMsgBuff);


/* 
 * fn		int msg_cleanup(CMSG_FD *pMsgFd)
 * brief	Close a message fd
 * details	
 *
 * param[in]	pMsgFd - message fd that we want to close		
 *
 * return	-1 is returned if an error occurs, otherwise is 0		
 */
int msg_cleanup(CMSG_FD *pMsgFd);


/* 
 * fn		int msg_connCliAndSend(CMSG_ID msgId, CMSG_FD *pMsgFd, CMSG_BUFF *pMsgBuff)
 * brief	init a client msg and send msg to server which is specified by msgId	
 *
 * param[in]	msgId -	server ID that we want to send
 * param[in]	pMsgFd - message fd that we want to send
 * param[in]	pMsgBuff - msg that we wnat to send
 *
 * return	-1 is returned if an error occurs, otherwise is 0	
 */
int msg_connCliAndSend(CMSG_ID msgId, CMSG_FD *pMsgFd, CMSG_BUFF *pMsgBuff);


/* 
 * fn		int msg_sendAndGetReply(CMSG_FD *pMsgFd, CMSG_BUFF *pMsgBuff)
 * brief	
 *
 * param[in]	pMsgFd - msg fd that we want to use
 * param[in/out]pMsgBuff - send msg and get reply
 * param[in]	timeSeconds - timeout in second
 *
 * return	-1 is returned if an error occurs, otherwise is 0	
 */
int msg_sendAndGetReplyWithTimeout(CMSG_FD *pMsgFd, CMSG_BUFF *pMsgBuff, int timeSeconds);


#endif /* __OS_MSG_H__ */

