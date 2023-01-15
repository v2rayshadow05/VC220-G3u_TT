/*
 *	Wireless Tools
 *
 *		Jean II - HPLB 97->99 - HPL 99->07
 *
 * Main code for "iwconfig". This is the generic tool for most
 * manipulations...
 * You need to link this code against "iwlib.c" and "-lm".
 *
 * This file is released under the GPL license.
 *     Copyright (c) 1997-2007 Jean Tourrilhes <jt@hpl.hp.com>
 */

#include "iwlib.h"		/* Header */
#if 0

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
//#include "wireless.h"
//#include <linux/if.h>

#define IFNAMSIZ	16
#define SIOCIWFIRSTPRIV      0x8BE0

struct  iw_point
{
	void	*pointer;       /* Pointer to the data  (in user space) */
	unsigned short	length;         /* number of fields or size in bytes */
	unsigned short	flags;          /* Optional params */
};

union   iwreq_data
{
	/* Config - generic */
	char            name[IFNAMSIZ];
	struct iw_point	data;
};

struct  iwreq
{
	char    ifr_name[IFNAMSIZ];    /* if name, e.g. "eth0" */

        /* Data part (defined just above) */
        union   iwreq_data      u;
};
#endif



//#define MAC_ADDR_LENGTH                 		6
#define MAC_ADDR_LEN					6
#define MAX_NUMBER_OF_MAC				75
#define MAX_LEN_OF_BSS_TABLE             		64
#define	RT_OID_DEVICE_NAME						0x0607
#define	RT_OID_VERSION_INFO						0x0608
#define	OID_802_11_BSSID_LIST						0x0609
#define	OID_802_3_CURRENT_ADDRESS					0x060A

#define RT_PRIV_IOCTL							(SIOCIWFIRSTPRIV + 0x01)
#define RTPRIV_IOCTL_GET_MAC_TABLE					(SIOCIWFIRSTPRIV + 0x0F)
#define OID_802_11_GET_CURRENT_CHANNEL_FALSE_CCA_AVG			0x0690	
#define OID_802_11_GET_CURRENT_CHANNEL_CST_TIME_AVG			0x0691
#define	OID_802_11_GET_CURRENT_CHANNEL_BUSY_TIME_AVG			0x0692
#define OID_802_11_GET_CURRENT_CHANNEL_STATS 				0x0693
#define OID_802_11_GET_CURRENT_CHANNEL_AP_ACTIVITY_AVG  		0x0694
#define OID_802_11_STA_STATISTICS					0x0695
#define OID_802_11_ASSOLIST						0x0534
#define OID_802_11_GET_CURRENT_CHANNEL_AP_TABLE				0x0696
#define OID_802_11_ASSOC_STA_MAX_CAP_LIST				0x069C
#define OID_802_11_GET_ACCESS_CATEGORY_TRAFFIC_STATUS			0x0697
#define OID_802_11_GET_SCAN_RESULTS					0x0698
#define OID_802_11_GET_RADIO_STATS_COUNT            0X0699
#define OID_802_11_MBSS_STATISTICS                                      0x069a
#define	OID_802_11_BTM_ACTION_FRAME_REQUEST		0x86b0
#define OID_STEER_ACTION				0x86b1  //34482
#define OID_802_11_GET_OPERATING_CLASS			0x06b2   //1716
#define OID_NON_ASSOCIATED_STA_REPORT_CAPABLE   0x06b3
#define OID_NON_ASSOCIATED_STA_REPORT_ENABLE	0x86b4
#define OID_NON_ASSOCIATED_STA_REPORT			0x06b5
#define OID_802_11_CHANNEL_INFO                 0x1805 // 6149
//#define OID_802_11_BTM_ACTION_FRAME_RESPONSE		0x86b3
#define NA_STA_PROBE_DATA_LIST_SIZE 		4
#define NA_STA_REPORT_SIZE 					2

///////////////////////////////////////////
typedef struct _DATE_TIME{
 unsigned int  year;
 unsigned int month;
 unsigned int day;
 unsigned int hour;
 unsigned int minute;
 unsigned int sec;
}DATE_TIME, *PDATE_TIME;

typedef struct _PROBE_DATA{
	char rssi;
	DATE_TIME dateTime;
//	unsigned int dataLen;
//	unsigned char data[1600];
}PROBE_DATA, *PPROBE_DATA;

typedef struct _STA_REPORT_DATA{
 unsigned char  MacAddr[MAC_ADDR_LEN];
 unsigned char 	dataCount;
 unsigned char  CyclicIndex;
 PROBE_DATA 	probeData[NA_STA_PROBE_DATA_LIST_SIZE];
}STA_REPORT_DATA, *PSTA_REPORT_DATA;

typedef struct _NA_STA_REPORT_LIST{
 STA_REPORT_DATA   reportData[NA_STA_REPORT_SIZE];
 unsigned char  reportSize;
}NA_STA_REPORT_LIST, *PNA_STA_REPORT_LIST;

///////////////////////////////////////////

typedef struct _channel_info{
	int channel;
	int central_channel;
}channel_info,*pchannel_info;

typedef struct _BLACKLIST_ACTION
{
	unsigned int Action;
	unsigned int Timer;
}BLACKLIST_ACTION, *PBLACKLIST_ACTION;

typedef struct _STEER_ACTION_TYPE
{
	unsigned char		StaMac[17];
	int   				Type;
	BLACKLIST_ACTION 	BlkListAction;
}STEER_ACTION_TYPE, *PSTEER_ACTION_TYPE;

typedef struct _BTM_REQUEST_ACTION_FRAME
{
	unsigned char CategoryType[4];
	unsigned char MacAddr[12];
	unsigned char Payload[1024];
}BTM_REQUEST_ACTION_FRAME, *PBTM_REQUEST_ACTION_FRAME;

typedef struct _CURRENT_CHANNEL_STATS{
	unsigned int	SamplePeriod;                 
	unsigned int	FalseCCACount;                               
 	unsigned int  	ChannelApActivity;       
 	unsigned int 	EdCcaBusyTime;                              
 	unsigned int  	ChannelBusyTime;                           
} CURRENT_CHANNEL_STATISTICS , *PCURRENT_CHANNEL_STATISTICS; 

typedef struct __RT_MBSS_STAT_ENTRY{
	// DATA counter
	unsigned int RxCount;
	unsigned int TxCount;
	unsigned int ReceivedByteCount;
	unsigned int TransmittedByteCount;
	unsigned int RxErrorCount;
	unsigned int RxDropCount;
	unsigned int TxErrorCount;
	unsigned int TxDropCount;
	unsigned int UnicastPktsRx;
	unsigned int UnicastPktsTx;
	unsigned int MulticastPktsRx;
	unsigned int MulticastPktsTx;
	unsigned int BroadcastPktsRx;
	unsigned int BroadcastPktsTx;
	unsigned int TxRetriedPktCount;
	unsigned int ChannelUseTime;
	// MGMT counter
	unsigned int MGMTRxCount;
	unsigned int MGMTTxCount;
	unsigned int MGMTReceivedByteCount;
	unsigned int MGMTTransmittedByteCount;
	unsigned int MGMTRxErrorCount;
	unsigned int MGMTRxDropCount;
	unsigned int MGMTTxErrorCount;
	unsigned int MGMTTxDropCount;
} RT_MBSS_STAT_ENTRY, *PRT_MBSS_STAT_ENTRY;

typedef struct  __RT_STA_STAT_ENTRY
{
	unsigned char  ApIdx;
    	unsigned char  Addr[MAC_ADDR_LEN];
	unsigned int   RxCount;
	unsigned int   TxCount;
	unsigned int   ReceivedByteCount;
	unsigned int   TransmittedByteCount;
	unsigned int   RxErrorCount;
	unsigned int   RxDropCount;
	unsigned int   TxErrorCount;
	unsigned int   TxDropCount;
	unsigned int   TxRetriedPktCount;
	unsigned int   ChannelUseTime;
} RT_STA_STAT_ENTRY, *PRT_STA_STAT_ENTRY;

typedef struct  __RT_STA_STATISTICS_TABLE
{
	unsigned int Num;
	RT_STA_STAT_ENTRY	STAEntry[MAX_NUMBER_OF_MAC];
} RT_STA_STATISTICS_TABLE, *PRT_STA_STATISTICS_TABLE;

typedef struct __RT_STA_MAX_CAP                 
{  
        unsigned char Addr[6];
        unsigned char NSS;
        unsigned char MODE;                     
        unsigned char MCS;
        unsigned char BW;
        unsigned char ShortGI;
} RT_STA_MAX_CAP, *PRT_STA_MAX_CAP;

typedef struct  __RT_STA_MAX_CAP_TABLE
{               
        int Num;
        RT_STA_MAX_CAP       STACap[MAX_NUMBER_OF_MAC];
} RT_STA_MAX_CAP_TABLE,*PRT_STA_MAX_CAP_TABLE;

typedef struct  __RT_STA_ENTRY_INFO
{
        unsigned char  Addr[17];
        //unsigned int   EntryType;
        unsigned short Aid;
        unsigned char  func_tb_idx;
        unsigned char  PsMode;
        unsigned int   WMM_Cap;
        unsigned char  MmPsMode;
        int	       AvgRssi0;
        int 	       AvgRssi1;
        int  	       AvgRssi2;
        char 	       Mode[5];
        char           BW[3];
        unsigned char  MCS;
        unsigned char  SGI;
        unsigned char  STBC;
        unsigned int   Idle_Time;
        unsigned int   Data_Rate;
        unsigned int   Connect_Time;
} RT_STA_ENTRY_INFO;

typedef struct  __RT_MBSS_STATISTICS_TABLE{
//	unsigned int 			TotalBeaconSentCount;
//    unsigned int            TotalTxCount;
//    unsigned int            TotalRxCount;
	unsigned int			 Num;
	RT_MBSS_STAT_ENTRY	MbssEntry[8];
} RT_MBSS_STATISTICS_TABLE, *PRT_MBSS_STATISTICS_TABLE;

typedef union _MACHTTRANSMIT_SETTING {
	struct {
		unsigned short  MCS:6;	/* MCS */
		unsigned short  ldpc:1;
		unsigned short  BW:2;	/*channel bandwidth 20MHz or 40 MHz */
		unsigned short  ShortGI:1;
		unsigned short  STBC:1;	/*SPACE */
		unsigned short  eTxBF:1;
		unsigned short  iTxBF:1;
		unsigned short  MODE:3;	/* Use definition MODE_xxx. */
	} field;
	unsigned short word;
} MACHTTRANSMIT_SETTING, *PMACHTTRANSMIT_SETTING;

typedef struct _RT_802_11_MAC_ENTRY {
	unsigned char  ApIdx;
	unsigned char  Addr[MAC_ADDR_LEN];
	unsigned char  Aid;
	unsigned char  Psm;		/* 0:PWR_ACTIVE, 1:PWR_SAVE */
	unsigned char  MimoPs;		/* 0:MMPS_STATIC, 1:MMPS_DYNAMIC, 3:MMPS_Enabled */
	char AvgRssi0;
	char AvgRssi1;
	char AvgRssi2;
	unsigned int  ConnectedTime;
	MACHTTRANSMIT_SETTING TxRate;
	unsigned int AvgSnr;
	unsigned int  LastRxRate;
//	int StreamSnr[3];
//	int SoundingRespSnr[3];				/* RTMP_RBUS_SUPPORT */
} RT_802_11_MAC_ENTRY, *PRT_802_11_MAC_ENTRY;

typedef struct _RT_802_11_MAC_TABLE {
	unsigned long Num;
	RT_802_11_MAC_ENTRY Entry[MAX_NUMBER_OF_MAC];
} RT_802_11_MAC_TABLE, *PRT_802_11_MAC_TABLE;

typedef struct _BSS_ENTRY_TABLE
{
	unsigned char   Bssid[MAC_ADDR_LEN];
	unsigned short  SsidLen;
	unsigned char 	Ssid[33];
	unsigned char 	Channel;
	unsigned char	ChannelWidth;
	unsigned char	ExtChannel;
	char		RSSI;
	unsigned char	SNR;
	unsigned char	PhyMode;
	unsigned char	NumSpatialStream;
}BSS_ENTRY_TABLE, *PBSS_ENTRY_TABLE;

typedef struct _BEACON_TABLE
{
	unsigned char Num;
	BSS_ENTRY_TABLE BssTable[MAX_LEN_OF_BSS_TABLE];
} BEACON_TABLE, *PBEACON_TABLE;

typedef struct _SCAN_RESULTS
{
	unsigned int	ch_busy_time;
	unsigned int	cca_err_cnt;
	unsigned int	num_ap;
	BSS_ENTRY_TABLE BssTable[MAX_LEN_OF_BSS_TABLE];
}SCAN_RESULTS, *PSCAN_RESULTS;

typedef struct _RADIO_STATS_COUNTER
{
    unsigned int TotalBeaconSentCount;
    unsigned int TotalTxCount;
    unsigned int TotalRxCount;
    unsigned int TxDataCount;
    unsigned int RxDataCount;
    unsigned int TxRetriedPktCount;
    unsigned int TxRetriedCount;
  //  unsigned int TxProbRspCount;
//    unsigned int RxBeaconCount;
  //  unsigned int RxProbReqCount;
}RADIO_STATS_COUNTER, *PRADIO_STATS_COUNTER;

typedef struct _MAC_ADDR
{
	unsigned char	mac[6];
}MAC_ADDR, *PMAC_ADDR;

typedef struct _AVG_VALUE
{
	unsigned int	value;
}AVG_VALUE, *PAVG_VALUE;

typedef struct _OPT_CLASS
{
	unsigned int	value;
}OPT_CLASS, *POPT_CLASS;

typedef struct _STREAMING_STATUS
{
	unsigned int status;
}STREAMING_STATUS, *PSTREAMING_STATUS;
typedef unsigned char NDIS_802_11_MAC_ADDRESS[6];
typedef enum _NDIS_802_11_NETWORK_TYPE {
	Ndis802_11FH,
	Ndis802_11DS,
	Ndis802_11OFDM5,
	Ndis802_11OFDM24,
	Ndis802_11Automode,
	Ndis802_11OFDM5_N,
	Ndis802_11OFDM24_N,
	Ndis802_11OFDM5_AC,
	Ndis802_11NetworkTypeMax	/* not a real type, defined as an upper bound */
} NDIS_802_11_NETWORK_TYPE, *PNDIS_802_11_NETWORK_TYPE;
typedef enum _NDIS_802_11_AUTHENTICATION_MODE {
	Ndis802_11AuthModeOpen,
	Ndis802_11AuthModeShared,
	Ndis802_11AuthModeAutoSwitch,
	Ndis802_11AuthModeWPA,
	Ndis802_11AuthModeWPAPSK,
	Ndis802_11AuthModeWPANone,
	Ndis802_11AuthModeWPA2,
	Ndis802_11AuthModeWPA2PSK,
	Ndis802_11AuthModeWPA1WPA2,
	Ndis802_11AuthModeWPA1PSKWPA2PSK,
	Ndis802_11AuthModeMax	/* Not a real mode, defined as upper bound */
} NDIS_802_11_AUTHENTICATION_MODE, *PNDIS_802_11_AUTHENTICATION_MODE;
/* Also aliased typedef to new name */
typedef enum _NDIS_802_11_WEP_STATUS {
    Ndis802_11WEPEnabled,
    Ndis802_11Encryption1Enabled = Ndis802_11WEPEnabled,
    Ndis802_11WEPDisabled,
    Ndis802_11EncryptionDisabled = Ndis802_11WEPDisabled,
    Ndis802_11WEPKeyAbsent,
    Ndis802_11Encryption1KeyAbsent = Ndis802_11WEPKeyAbsent,
    Ndis802_11WEPNotSupported,
    Ndis802_11EncryptionNotSupported = Ndis802_11WEPNotSupported,
    Ndis802_11TKIPEnable,
    Ndis802_11Encryption2Enabled = Ndis802_11TKIPEnable,
    Ndis802_11Encryption2KeyAbsent,
    Ndis802_11AESEnable,
    Ndis802_11Encryption3Enabled = Ndis802_11AESEnable,
    Ndis802_11CCMP256Enable,
    Ndis802_11GCMP128Enable,
    Ndis802_11GCMP256Enable,
    Ndis802_11Encryption3KeyAbsent,
    Ndis802_11TKIPAESMix,
    Ndis802_11Encryption4Enabled = Ndis802_11TKIPAESMix,    /* TKIP or AES mix */
    Ndis802_11Encryption4KeyAbsent,
    Ndis802_11GroupWEP40Enabled,
    Ndis802_11GroupWEP104Enabled,
} NDIS_802_11_WEP_STATUS, *PNDIS_802_11_WEP_STATUS, NDIS_802_11_ENCRYPTION_STATUS, *PNDIS_802_11_ENCRYPTION_STATUS;

typedef struct _DCC_CH
{
	unsigned char	ChannelNo;
	unsigned int 	FalseCCA;
	unsigned int	chanbusytime;
} DCC_CH, *PDCC_CH;
typedef struct _NDIS_802_11_SSID {
	unsigned int SsidLength;	/* length of SSID field below, in bytes; */
	/* this can be zero. */
	unsigned char Ssid[32];	/* SSID information field */
} NDIS_802_11_SSID, *PNDIS_802_11_SSID;

typedef struct _WLAN_BSSID_LIST
{   
   NDIS_802_11_MAC_ADDRESS        	Bssid;         // BSSID
   unsigned char 							WpsAP; 		/* 0x00: not support WPS, 0x01: support normal WPS, 0x02: support Ralink auto WPS, 0x04: support Samsung WAC */
   unsigned short							WscDPIDFromWpsAP;   
   unsigned char							Privacy;    // WEP encryption requirement
   unsigned int                   			Signal;       // receive signal strength in dBm
   NDIS_802_11_NETWORK_TYPE    		wireless_mode;
   NDIS_802_11_AUTHENTICATION_MODE	AuthMode;	
   NDIS_802_11_WEP_STATUS			Cipher;				// Unicast Encryption Algorithm extract from VAR_IE
   unsigned char							Channel;
   unsigned char							ExtChannel;
   unsigned char   							BssType;
   unsigned char							SupportedRates[12];
   unsigned char    						bQbssLoadValid;                     // 1: variable contains valid value
   unsigned short      						QbssLoadStaNum;
   unsigned char       						QbssLoadChannelUtilization;
   unsigned char  							Snr[4];
   unsigned char							NumSpatialStream;
   char							rssi[4];
   unsigned char  							vendorOUI0[3];
   unsigned char  							vendorOUI1[3];
   unsigned char						   	ChannelWidth;
   NDIS_802_11_SSID					Ssid;       // SSID
} WLAN_BSSID_LIST, *PWLAN_BSSID_LIST;

typedef struct  _BSSID_LIST_RESULTS
{
   unsigned int          					NumberOfItems;      // in list below, at least 1
   WLAN_BSSID_LIST 					BssidTable[MAX_LEN_OF_BSS_TABLE];
   DCC_CH							DccCh[26]; //anand to handle 5g and 2g both channels
}BSSID_LIST_RESULTS, *PBSSID_LIST_RESULTS;


#ifndef os_malloc
#define os_malloc(s) malloc((s))
#endif
#ifndef os_realloc
#define os_realloc(p, s) realloc((p), (s))
#endif

#ifndef os_memcpy
#define os_memcpy(d, s, n) memcpy((d), (s), (n))
#endif
#ifndef os_memmove
#define os_memmove(d, s, n) memmove((d), (s), (n))
#endif
#ifndef os_memset
#define os_memset(s, c, n) memset(s, c, n)
#endif
#ifndef os_memcmp
#define os_memcmp(s1, s2, n) memcmp((s1), (s2), (n))
#endif


void * os_zalloc(size_t size)
{
	void *n = os_malloc(size);
	if (n)
		os_memset(n, 0, size);
	return n;
}

typedef unsigned char UCHAR;

typedef UCHAR *PUCHAR;
typedef char RTMP_STRING;

unsigned char BtoH(char ch)
{
	if (ch >= '0' && ch <= '9') return (ch - '0');        /* Handle numerals*/
	if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 0xA);  /* Handle capitol hex digits*/
	if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 0xA);  /* Handle small hex digits*/
	return(255);
}

void AtoH(RTMP_STRING *src, PUCHAR dest, int destlen)
{
	RTMP_STRING *srcptr;
	PUCHAR destTemp;

	srcptr = src;
	destTemp = (PUCHAR) dest;

	while(destlen--)
	{
		*destTemp = BtoH(*srcptr++) << 4;    /* Put 1st ascii byte in upper nibble.*/
		*destTemp += BtoH(*srcptr++);      /* Add 2nd ascii byte to above.*/
		destTemp++;
	}
}

#if 0
static int driver_wext_set_oid(struct driver_wext_data *drv_data, const char *ifname,
              				   unsigned short oid, char *data, size_t len)    
{
    char *buf;                             
    struct iwreq iwr;
	
    buf = os_zalloc(len);

    os_memset(&iwr, 0, sizeof(iwr));       
    os_strncpy(iwr.ifr_name, ifname, IFNAMSIZ);
    iwr.u.data.flags = oid;
    iwr.u.data.flags |= OID_GET_SET_TOGGLE;

    if (data)
        os_memcpy(buf, data, len);

	if (buf) {
    	iwr.u.data.pointer = (caddr_t)buf;    
    	iwr.u.data.length = len;
	} else {
    	iwr.u.data.pointer = NULL;    
    	iwr.u.data.length = 0;
	}

    if (ioctl(drv_data->ioctl_sock, RT_PRIV_IOCTL, &iwr) < 0) {
        DBGPRINT(DEBUG_ERROR, "%s: oid=0x%x len (%d) failed",
               __FUNCTION__, oid, len);
        os_free(buf);
        return -1;
    }

    os_free(buf);
    return 0;
}

#endif
int string_to_int(char *str)
{
	int val;
	val=atoi(str);
	return val;
}
static unsigned char *phy_bw_str[] = {"20M","40M","80M","BOTH","10M"};
static unsigned char *phy_mode_str[] = {"CCK","OFDM","HTMIX","GF","VHT"};
main(int	argc,
     char **	argv)
{
	int skfd;		/* generic raw socket desc.	*/
	int subcmd;
	CURRENT_CHANNEL_STATISTICS ChannelStats;
	RT_MBSS_STATISTICS_TABLE mbss_stat;
	RT_STA_STATISTICS_TABLE Sta_stat;
	RT_STA_MAX_CAP_TABLE  Sta_cap;
	RT_802_11_MAC_TABLE MacTable;
	char *buf = NULL;
//	BEACON_TABLE beaconTable;
//	SCAN_RESULTS scanResult;

	
	int AvgValue, i;
	struct iwreq	wrq;
	u_char	buffer[8192];	/* Only that big in v25 and later */

	/* Create a channel to the NET kernel. */
	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd < 0)
	{
		perror("socket");
		return(-1);
	}

	if (argc < 3)
	{
		printf("Usage: %s wlan0 subcmd\n", argv[0]);
		return -1;
	}

	subcmd = atoi(argv[2]);
	strncpy(wrq.ifr_name, argv[1], IFNAMSIZ);

//	if(argc == 4)
//	strncpy(buffer, argv[3], strlen(argv[3]));
	
	wrq.u.data.flags = subcmd;
	wrq.u.data.pointer = buffer;
	wrq.u.data.length = sizeof(buffer);
	printf("Before: iface %s, subcmd %d, buffer %p, length %d\n",
			wrq.ifr_name, wrq.u.data.flags, wrq.u.data.pointer, wrq.u.data.length);

	if(subcmd == 9999)
	{
	 	if (ioctl(skfd,RTPRIV_IOCTL_GET_MAC_TABLE, &wrq) < 0)
        	{
                	perror("ioctl");
                	return -1;
        	}
	}
	else if(subcmd == OID_NON_ASSOCIATED_STA_REPORT_ENABLE)
	{
		unsigned char stareportenable;
		stareportenable = atoi(argv[3]);
		printf("stareportenable %d \n", stareportenable);
		wrq.u.data.pointer = &stareportenable;      //(caddr_t)buffer;
		wrq.u.data.length = sizeof(stareportenable);
		if (ioctl(skfd, RT_PRIV_IOCTL, &wrq) < 0)
		{
			perror("ioctl");
			return -1;
		}	
		
	}
	else if(subcmd == OID_NON_ASSOCIATED_STA_REPORT)
	{
		unsigned char macaddr[6];
		int i;
		NA_STA_REPORT_LIST stareportList;
			int j, k;
		for (i = 3; i< 9 ; i++)
		{
		
			buffer[i-3] = (int)strtol(argv[i], NULL, 16);
		}
		wrq.u.data.pointer = buffer;      //(caddr_t)buffer;
		wrq.u.data.length = sizeof(NA_STA_REPORT_LIST);
	//	wrq.u.data.length = sizeof(unsigned char)*6;
		if (ioctl(skfd, RT_PRIV_IOCTL, &wrq) < 0)
		{
			perror("ioctl");
			return -1;
		}
		//sleep(1);
		{
			//printf("OID NA STA Report \n");
			//if(wrq.u.data.length > 6)
			{
			//printf("OID NA STA Report 1\n");
			stareportList = *((PNA_STA_REPORT_LIST)wrq.u.data.pointer);
			printf("dump %x:%x:%x \n",((char*)wrq.u.data.pointer)[0], ((char*)wrq.u.data.pointer)[1], ((char*)wrq.u.data.pointer)[3]);
			printf("report size %d \n", stareportList.reportSize);
			for(j = 0 ; j < stareportList.reportSize; j++)
			{
				PSTA_REPORT_DATA stareportdata = &stareportList.reportData[j];
				printf("report STA MAC %2x:%2x:%2x:%2x:%2x:%2x \n", stareportdata->MacAddr[0], stareportdata->MacAddr[1],
					stareportdata->MacAddr[2], stareportdata->MacAddr[3], stareportdata->MacAddr[4], stareportdata->MacAddr[5]);
				for(k = 0; k < stareportdata->dataCount ; k++)
				{
					printf(" RSSI %d \n", stareportdata->probeData[k].rssi);
					printf(" Time hour %d, minute %d, sec %d \n", stareportdata->probeData[k].dateTime.hour, stareportdata->probeData[k].dateTime.minute,
						stareportdata->probeData[k].dateTime.sec);
				}
			}
			}
		}
	}
	else if(subcmd == OID_STEER_ACTION)
	{
		STEER_ACTION_TYPE action;
	//	char *buf;
		printf("oid steer action \n");

	//	os_alloc_mem(pAd, (char **)&action,sizeof(*action));
		
		strncpy(action.StaMac, argv[3], 17);
		action.Type = atoi(argv[4]);
		action.BlkListAction.Action = atoi(argv[5]);
		action.BlkListAction.Timer = atoi(argv[6]);
	//	memcpy(buffer, (char *) &action, sizeof(action));
		wrq.u.data.pointer = &action;      //(caddr_t)buffer;
		wrq.u.data.length = sizeof(action);
		if (ioctl(skfd, RT_PRIV_IOCTL, &wrq) < 0)
		{
			perror("ioctl");
			return -1;
		}	
		
	}
	else if(subcmd == OID_802_11_BTM_ACTION_FRAME_REQUEST)
	{
		BTM_REQUEST_ACTION_FRAME frame;
		int i, idx;
		char temp[3];
	//	char *buf;
		printf("oid BTM Action Frame Request \n");

	//	os_alloc_mem(pAd, (char **)&action,sizeof(*action));
		
		strncpy(frame.MacAddr, argv[3], 12);

	/*	for( i = 0; i < strlen(argv[4])/2; i++)
		{
			idx = i*2;
			temp[0] = argv[idx++];
			temp[1] = argv[idx];
			temp[2] = '\0';

			AtoH(temp, (UCHAR *)&frame.Payload[i], 1);
		//	frame.Payload[i] = (unsigned char)strtol(temp, NULL, 16); 
		}

	*/
		frame.Payload[0]=0x0a;
		frame.Payload[1]=0x07;
		frame.Payload[2]=0x01;
		frame.Payload[3]=0x07;
		frame.Payload[4]=0x60;
		frame.Payload[5]=0x00;
		frame.Payload[6]=0xc8;
		frame.Payload[7]=0x34;
		frame.Payload[8]=0x10;
		frame.Payload[9]=0x5c;
		frame.Payload[10]=0x8f;
		frame.Payload[11]=0xe0;
		frame.Payload[12]=0x29;
		frame.Payload[13]=0x9b;
		frame.Payload[14]=0x48;
		frame.Payload[15]=0x4f;
		frame.Payload[16]=0x00;
		frame.Payload[17]=0x00;
		frame.Payload[18]=0x00;
		frame.Payload[19]=0x21;
		frame.Payload[20]=0x06;
		frame.Payload[21]=0x07;
		frame.Payload[22]=0x03;
		frame.Payload[23]=0x01;
		frame.Payload[24]=0xff;
		frame.Payload[25]=0x5c;
		frame.Payload[26]=0x07;
		frame.Payload[27]=0xd7;
		frame.Payload[28]=0x18;
		//strncpy(frame.Payload, argv[4], strlen(argv[4])+1);
		frame.Payload[29] = '\0';

	//	printf("payload ---- \n");
	//	for(i=0; i <= 30 ; i++)
	//		printf("0x%02X\n", frame.Payload[i]);
	//	printf("\n");
		
		wrq.u.data.pointer = &frame;      //(caddr_t)buffer;
		wrq.u.data.length = sizeof(frame);
		if (ioctl(skfd, RT_PRIV_IOCTL, &wrq) < 0)
		{
			perror("ioctl");
			return -1;
		}
	}
	else
	{
		if (ioctl(skfd, RT_PRIV_IOCTL, &wrq) < 0)
		{
			perror("ioctl");
			return -1;
		}	
		//printf("ERR NO = %d\n", errno);
	}
	close(skfd);
//	printf("\n iwprivget done ");
	switch(subcmd) 
	{
		/*
		case OID_NON_ASSOCIATED_STA_REPORT:
		{
			NA_STA_REPORT_LIST stareportList;
			int j, k;
			printf("OID NA STA Report \n");
			if(wrq.u.data.length > 6)
			{
			printf("OID NA STA Report 1\n");
			stareportList = *((PNA_STA_REPORT_LIST)wrq.u.data.pointer);
			printf("report size %d \n", stareportList.reportSize);
			for(j = 0 ; j < stareportList.reportSize; j++)
			{
				PSTA_REPORT_DATA stareportdata = &stareportList.reportData[i];
				printf("report STA MAC %2x:%2x:%2x:%2x:%2x:%2x \n", stareportdata->MacAddr[0], stareportdata->MacAddr[1],
					stareportdata->MacAddr[2], stareportdata->MacAddr[3], stareportdata->MacAddr[4], stareportdata->MacAddr[5]);
				for(k = 0; k < stareportdata->dataCount ; k++)
				{
					printf(" RSSI %d \n", stareportdata->probeData[k].rssi);
					printf(" Time hour %d, minute %d, sec %d \n", stareportdata->probeData[k].dateTime.hour, stareportdata->probeData[k].dateTime.minute,
						stareportdata->probeData[k].dateTime.sec);
				}
			}
			}
		}
		break;
		*/

		case OID_NON_ASSOCIATED_STA_REPORT_CAPABLE:
		{
			OPT_CLASS opValue;
			opValue = *((POPT_CLASS)wrq.u.data.pointer);
		 	 //	printf(" After: iface %s, subcmd %d, length %d\n",
                  	//      wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
			printf("NA STA REPORT Capable: %u \n", opValue.value);	
		}
		case OID_802_11_GET_OPERATING_CLASS:
			{
				OPT_CLASS opValue;
				opValue = *((POPT_CLASS)wrq.u.data.pointer);
		 	 //	printf(" After: iface %s, subcmd %d, length %d\n",
                  	//      wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
				printf("operating Class: %u \n", opValue.value);
			}
		break;
			
		case OID_802_11_GET_CURRENT_CHANNEL_STATS:
			ChannelStats = *((PCURRENT_CHANNEL_STATISTICS)wrq.u.data.pointer);
		  //	printf(" After: iface %s, subcmd %d, length %d\n",
                  //      wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
			printf(" TotalDuration: %u  FalseCCACount: %u ChannelAPActivity: %u  EDCccaBusyTime: %u ChannlBusyTime: %u \n", ChannelStats.SamplePeriod, ChannelStats.FalseCCACount, ChannelStats.ChannelApActivity, ChannelStats.EdCcaBusyTime, ChannelStats.ChannelBusyTime);
			//free(wrq.u.data.pointer);
			break;
		case OID_802_11_GET_CURRENT_CHANNEL_FALSE_CCA_AVG:
			{
				AVG_VALUE avgValue;
				avgValue = *((PAVG_VALUE)wrq.u.data.pointer);
		 	 //	printf(" After: iface %s, subcmd %d, length %d\n",
                  	//      wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
				printf("AVG FALSE CCA: %u \n", avgValue.value);
			}
			break;

		case OID_802_11_GET_CURRENT_CHANNEL_CST_TIME_AVG:
			AvgValue = *((int*)wrq.u.data.pointer);
                    //    printf(" After: iface %s, subcmd %d, length %d\n",
                    //    wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
                        printf("CCA BUSY TIME: %u \n", AvgValue);
                        break;
			
		case OID_802_11_GET_CURRENT_CHANNEL_BUSY_TIME_AVG:
			AvgValue = *((int*)wrq.u.data.pointer);
                     //   printf(" After: iface %s, subcmd %d, length %d\n",
                    //    wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
                        printf("Channel BUSY TIME: %u \n", AvgValue);
                        break;
		case OID_802_11_GET_CURRENT_CHANNEL_AP_ACTIVITY_AVG:
			AvgValue = *((int*)wrq.u.data.pointer);
                     //   printf(" After: iface %s, subcmd %d, length %d\n",
                     //   wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
                        printf("AVG AP Activity TIME: %u \n", AvgValue);
                        break;
		case OID_802_11_MBSS_STATISTICS:
			mbss_stat = *((PRT_MBSS_STATISTICS_TABLE)wrq.u.data.pointer);
		//	printf(" After: iface %s, subcmd %d, length %d\n",
                 //       wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
	//		printf(" TotalBeaconSentCount %u \n",mbss_stat.TotalBeaconSentCount); 
      //      printf(" TotalTxCount %u \n", mbss_stat.TotalTxCount);
       //     printf(" TotalRxCount %u \n", mbss_stat.TotalRxCount);
			for(i=0; i < mbss_stat.Num; i++)
			{
				printf("BSS %d RxCount: %u  \n",i+1, mbss_stat.MbssEntry[i].RxCount);
				printf("BSS %d TxCount: %u  \n",i+1, mbss_stat.MbssEntry[i].TxCount);
				printf("BSS %d ReceivedByteCount: %u  \n",i+1, mbss_stat.MbssEntry[i].ReceivedByteCount);
				printf("BSS %d TransmittedByteCount: %u  \n",i+1, mbss_stat.MbssEntry[i].TransmittedByteCount);
				printf("BSS %d RxErrorCount: %u  \n",i+1, mbss_stat.MbssEntry[i].RxErrorCount);
				printf("BSS %d RxDropCount: %u  \n",i+1, mbss_stat.MbssEntry[i].RxDropCount);
				printf("BSS %d TxErrorCount: %u  \n",i+1, mbss_stat.MbssEntry[i].TxErrorCount);
				printf("BSS %d TxDropCount: %u  \n",i+1, mbss_stat.MbssEntry[i].TxDropCount);
				printf("BSS %d TxRetriedPktCount: %u \n",i+1, mbss_stat.MbssEntry[i].TxRetriedPktCount);
				printf("BSS %d UnicastPktsRx: %u \n",i+1, mbss_stat.MbssEntry[i].UnicastPktsRx);
				printf("BSS %d UnicastPktsTx: %u \n",i+1, mbss_stat.MbssEntry[i].UnicastPktsTx);
				printf("BSS %d MulticastPktsRx: %u \n",i+1, mbss_stat.MbssEntry[i].MulticastPktsRx);
				printf("BSS %d MulticastPktsTx: %u \n",i+1, mbss_stat.MbssEntry[i].MulticastPktsTx);
				printf("BSS %d BroadcastPktsRx: %u \n",i+1, mbss_stat.MbssEntry[i].BroadcastPktsRx);
				printf("BSS %d BroadcastPktsTx: %u \n",i+1, mbss_stat.MbssEntry[i].BroadcastPktsTx);
				printf("BSS %d ManagementTXcount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTTxCount);
				printf("BSS %d ManagementRxcount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTRxCount);
				printf("BSS %d MGMTReceivedByteCount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTReceivedByteCount);
				printf("BSS %d MGMTTransmittedByteCount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTTransmittedByteCount);
				printf("BSS %d MGMTRxErrorCount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTRxErrorCount);
				printf("BSS %d MGMTTxErrorCount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTTxErrorCount);
				printf("BSS %d MGMTRxDropCount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTRxDropCount);
				printf("BSS %d MGMTTxDropCount: %u \n",i+1, mbss_stat.MbssEntry[i].MGMTTxDropCount);	
				printf("BSS %d ChannelUseTime: %u \n",i+1, mbss_stat.MbssEntry[i].ChannelUseTime);
				printf("------------------------------\n");	
			//	printf("BSS %d FifoTxCnt: %u \n",i+1, mbss_stat.MbssEntry[i].FifoTxCnt);
			//	printf("BSS %d FifoFailCnt: %u \n",i+1, mbss_stat.MbssEntry[i].FifoFailCnt);
			//	printf("BSS %d FifoSucCnt: %u \n",i+1, mbss_stat.MbssEntry[i].FifoSucCnt);
			//	printf("BSS %d FifoRetryCnt: %u \n",i+1, mbss_stat.MbssEntry[i].FifoRetryCnt);
			//	printf("BSS %d FifoRetriedPktCnt: %u \n",i+1, mbss_stat.MbssEntry[i].FifoRetriedPktCnt);
			}
			break;
        case OID_802_11_GET_RADIO_STATS_COUNT:
             {
                 RADIO_STATS_COUNTER radiostatscounter;
                 radiostatscounter = *((PRADIO_STATS_COUNTER)wrq.u.data.pointer);
                 printf("TotalBeaconCount %u \n", radiostatscounter.TotalBeaconSentCount);
                 printf("TotalTxCount %u \n", radiostatscounter.TotalTxCount);
                 printf("TotalRxCount %u \n", radiostatscounter.TotalRxCount);
                 printf("TxDataCount %u \n", radiostatscounter.TxDataCount);
                 printf("RxDataCount %u \n", radiostatscounter.RxDataCount);
                 printf("TxRetriedPktCount %u \n", radiostatscounter.TxRetriedPktCount);
                 printf("TxRetriedCount %u \n", radiostatscounter.TxRetriedCount);
           //      printf("RxBeaconCount %u \n", radiostatscounter.RxBeaconCount);
            //     printf("RxProbReqCount %u \n", radiostatscounter.RxProbReqCount);           
             }
             break;
		case OID_802_11_ASSOC_STA_MAX_CAP_LIST:
			Sta_cap = *((PRT_STA_MAX_CAP_TABLE)wrq.u.data.pointer);
		
			for(i = 0; i < Sta_cap.Num; i++)
			{	
				printf("\nMAC: %02x:%02x:%02x:%02x:%02x:%02x\n", Sta_cap.STACap[i].Addr[0],Sta_cap.STACap[i].Addr[1],Sta_cap.STACap[i].Addr[2],Sta_cap.STACap[i].Addr[3],Sta_cap.STACap[i].Addr[4],Sta_cap.STACap[i].Addr[5]);
				printf("Num of Antenna = %d\n",Sta_cap.STACap[i].NSS);
				printf("Phy Mode = %s\n",phy_mode_str[Sta_cap.STACap[i].MODE]);
				printf("MCS = %d\n",Sta_cap.STACap[i].MCS);
				printf("BW = %s\n",phy_bw_str[Sta_cap.STACap[i].BW]);
				printf("Short GI = %d\n",Sta_cap.STACap[i].ShortGI);
			}
		break;
		case OID_802_11_STA_STATISTICS:
			Sta_stat = *((PRT_STA_STATISTICS_TABLE)wrq.u.data.pointer);
		//	printf(" After: iface %s, subcmd %d, length %d\n",
                 //       wrq.ifr_name, wrq.u.data.flags, wrq.u.data.length);
			for(i = 0; i < Sta_stat.Num; i++)
			{
				printf("APIDX: %u \n", Sta_stat.STAEntry[i].ApIdx);
				printf("MAC: %x:%x:%x:%x:%x:%x \n", Sta_stat.STAEntry[i].Addr[0], Sta_stat.STAEntry[i].Addr[1], Sta_stat.STAEntry[i].Addr[2], Sta_stat.STAEntry[i].Addr[3], Sta_stat.STAEntry[i].Addr[4], Sta_stat.STAEntry[i].Addr[5]);
				printf("RxCount: %u \n", Sta_stat.STAEntry[i].RxCount);
				printf("TxCount: %u \n", Sta_stat.STAEntry[i].TxCount);
				printf("ReceivedByteCount: %u \n", Sta_stat.STAEntry[i].ReceivedByteCount);
				printf("TransmittedByteCount: %u \n", Sta_stat.STAEntry[i].TransmittedByteCount);
				printf("RxErrorCount: %u \n", Sta_stat.STAEntry[i].RxErrorCount);
				printf("RxDropCount: %u \n", Sta_stat.STAEntry[i].RxDropCount);
				printf("TxErrorCount: %u \n", Sta_stat.STAEntry[i].TxErrorCount);
                                printf("TxDropCount: %u \n", Sta_stat.STAEntry[i].TxDropCount);
				printf("TxRetriedPktCount: %u \n", Sta_stat.STAEntry[i].TxRetriedPktCount);
				printf("ChannelUseTime: %u \n", Sta_stat.STAEntry[i].ChannelUseTime);
				printf("------------------------------\n");	
			}
			break;
		case OID_802_11_ASSOLIST:
			
			buf = os_malloc(wrq.u.data.length);
			memcpy(buf,wrq.u.data.pointer,wrq.u.data.length);
			printf("\n%s\n",(buf));//145
#if 0
			RT_STA_ENTRY_INFO STA_Entry;
			memset(&STA_Entry,0,sizeof(RT_STA_ENTRY_INFO));
			
			memcpy(STA_Entry.Addr,buf+128,sizeof(STA_Entry.Addr));	
			STA_Entry.EntryType = string_to_int(buf+147);
			STA_Entry.Aid = (unsigned short)string_to_int(buf+154);
			memcpy(&STA_Entry.func_tb_idx,buf+158,sizeof(char));
			memcpy(&STA_Entry.PsMode,buf+162,sizeof(char));
			STA_Entry.WMM_Cap = string_to_int(buf+166);
			memcpy(&STA_Entry.MmPsMode,buf+170,sizeof(char));
			STA_Entry.AvgRssi0 = string_to_int(buf+178);
			STA_Entry.AvgRssi1 = string_to_int(buf+185);
			STA_Entry.AvgRssi2 = string_to_int(buf+192);
			memcpy(STA_Entry.Mode,buf+199,4);
			memcpy(STA_Entry.BW,buf+209,3);
			memcpy(&STA_Entry.MCS,buf+215,sizeof(char));
			memcpy(&STA_Entry.SGI,buf+221,sizeof(char));
			memcpy(&STA_Entry.STBC,buf+227,sizeof(char));
			STA_Entry.Idle_Time = string_to_int(buf+233);
			STA_Entry.Data_Rate = string_to_int(buf+240);
			STA_Entry.Connect_Time = string_to_int(buf+247);
			printf("%s, ",STA_Entry.Addr);
			printf("%d, ",STA_Entry.EntryType);
			printf("%d, ",STA_Entry.Aid);
			printf("%c, ",STA_Entry.func_tb_idx);
			printf("%c, ",STA_Entry.PsMode);
			printf("%d, ",STA_Entry.WMM_Cap);
			printf("%c, ",STA_Entry.MmPsMode);
			printf("%d, ",STA_Entry.AvgRssi0);
			printf("%d, ",STA_Entry.AvgRssi1);
			printf("%d, ",STA_Entry.AvgRssi2);
			printf("%s, ",STA_Entry.Mode);
			printf("%.3s, ",STA_Entry.BW);
			printf("%c, ",STA_Entry.MCS);
			printf("%c, ",STA_Entry.SGI);
			printf("%c, ",STA_Entry.STBC);
			printf("%d, ",STA_Entry.Idle_Time);
			printf("%d, ",STA_Entry.Data_Rate);
			printf("%d\n",STA_Entry.Connect_Time);
			//printf("\dn%s\n",(buf+128));//145
			//printf("\n%s\n",(buf+147));//152
			//printf("\n%s\n",(buf+154));
			//printf("\n%s\n",(buf+158));
			//printf("\n%s\n",(buf+162));
			//printf("\n%s\n",(buf+166));
			//printf("\n%s\n",(buf+170));
			//printf("\n%s\n",(buf+178));
			//printf("\n%s\n",(buf+185));
			//printf("\n%s\n",(buf+192));
			//printf("\n%s\n",(buf+199));
			//printf("\n%s\n",(buf+209));
			//printf("\n%s\n",(buf+215));
			//printf("\n%s\n",(buf+221));
			//printf("\n%s\n",(buf+227));
			//printf("\n%s\n",(buf+233));
			//printf("\n%s\n",(buf+240));
			//printf("\n%s\n",(buf+247));
#endif
			free(buf);
			buf=NULL;
		break;
		 case 9999:
			MacTable = *((PRT_802_11_MAC_TABLE)wrq.u.data.pointer);
			for(i=0; i < MacTable.Num; i++)
			{
				printf("APIDX: %u \n", MacTable.Entry[i].ApIdx);
				printf("Mac: %x:%x:%x:%x:%x;%x \n", MacTable.Entry[i].Addr[0],MacTable.Entry[i].Addr[1], MacTable.Entry[i].Addr[2], MacTable.Entry[i].Addr[3], MacTable.Entry[i].Addr[4], MacTable.Entry[i].Addr[5]);
				printf("Aid: %u \n", MacTable.Entry[i].Aid);
				printf("Psm: %u \n", MacTable.Entry[i].Psm);
				printf("MimoPs: %u \n", MacTable.Entry[i].MimoPs);
				printf("AvgRssi0: %d \n", MacTable.Entry[i].AvgRssi0);
				printf("AvgRssi1: %d \n", MacTable.Entry[i].AvgRssi1);
				printf("AvgSnr: %u \n", MacTable.Entry[i].AvgSnr);
				printf("ConnectedTime: %u \n", MacTable.Entry[i].ConnectedTime);
				printf("MCS: %u \n", MacTable.Entry[i].TxRate.field.MCS);
				printf("BW: %u \n", MacTable.Entry[i].TxRate.field.BW);
				printf("GI: %u \n", MacTable.Entry[i].TxRate.field.ShortGI);
				printf("STBC: %u \n", MacTable.Entry[i].TxRate.field.STBC);
			//	printf("rsv: %x \n", MacTable.Entry[i].TxRate.field.rsv);
				printf("MODE: %u \n", MacTable.Entry[i].TxRate.field.MODE);
				//printf("Word: %u \n", MacTable.Entry[i].TxRate.word);
				printf("LastRxRate: %u \n", MacTable.Entry[i].LastRxRate);
	
			}
			break;
		 
		case	OID_802_11_GET_CURRENT_CHANNEL_AP_TABLE:
			
			{
				BEACON_TABLE beaconTable;
				beaconTable = *((PBEACON_TABLE)wrq.u.data.pointer);
				
				printf("BSSID             SSIDLEN SSID                               CHANNEL CW ExCH RSSI SNR PHYMOD Nss \n");
				for(i=0; i < beaconTable.Num; i++)
				{
				//	printf(" BSSID			SSIDLEN	SSID				CHANNEL CW ExCH	RSSI  SNR PHYMOD  Nss");
					printf("%02x:%02x:%02x:%02x:%02x:%02x ", beaconTable.BssTable[i].Bssid[0],  beaconTable.BssTable[i].Bssid[1],  beaconTable.BssTable[i].Bssid[2],  beaconTable.BssTable[i].Bssid[3],  beaconTable.BssTable[i].Bssid[4],  beaconTable.BssTable[i].Bssid[5]);
					printf(" %6u ", beaconTable.BssTable[i].SsidLen);
					printf(" %33s ", beaconTable.BssTable[i].Ssid);
					printf(" %6u ", beaconTable.BssTable[i].Channel);
					printf("%2u ", beaconTable.BssTable[i].ChannelWidth);
					printf(" %3u ", beaconTable.BssTable[i].ExtChannel);
					printf(" %3d ", beaconTable.BssTable[i].RSSI);
					printf(" %2u ", beaconTable.BssTable[i].SNR);
					printf(" %5u ", beaconTable.BssTable[i].PhyMode);
					printf(" %2u  \n", beaconTable.BssTable[i].NumSpatialStream);
				}	
			} 
			break;

		case	OID_802_11_GET_SCAN_RESULTS:
			{
				SCAN_RESULTS scanResult;
				//printf("\n anand 12");
				scanResult = *((PSCAN_RESULTS)wrq.u.data.pointer);
				printf("Ch_BUSY_Time: %u \n", scanResult.ch_busy_time);
				printf("CCA Error Count: %u \n", scanResult.cca_err_cnt);
				printf("AP NUM: %u \n\n\n",scanResult.num_ap);
				printf("BSSID             SSIDLEN SSID                               CHANNEL CW ExCH RSSI SNR PHYMOD Nss \n");
				for(i=0; i < scanResult.num_ap; i++)
                                {
                                        printf("%02x:%02x:%02x:%02x:%02x:%02x ", scanResult.BssTable[i].Bssid[0], scanResult.BssTable[i].Bssid[1],  scanResult.BssTable[i].Bssid[2],  scanResult.BssTable[i].Bssid[3], scanResult.BssTable[i].Bssid[4],  scanResult.BssTable[i].Bssid[5]);
                                        printf(" %6u ", scanResult.BssTable[i].SsidLen);
                                        printf(" %33s ", scanResult.BssTable[i].Ssid);
                                        printf(" %6u ", scanResult.BssTable[i].Channel);
                                        printf(" %2u ", scanResult.BssTable[i].ChannelWidth);
                                        printf(" %3u ", scanResult.BssTable[i].ExtChannel);
                                        printf(" %3d ", scanResult.BssTable[i].RSSI);
                                        printf(" %2u ", scanResult.BssTable[i].SNR);
                                        printf(" %5u ",scanResult.BssTable[i].PhyMode);
                                        printf(" %2u  \n", scanResult.BssTable[i].NumSpatialStream);
                                } 
			}
			break;
		
		case 	OID_802_11_GET_ACCESS_CATEGORY_TRAFFIC_STATUS:
			{
				STREAMING_STATUS streamingStatus;
				streamingStatus = *((PSTREAMING_STATUS)wrq.u.data.pointer);
				printf("Streaming Status is: %u \n", streamingStatus.status);	
				
			}
			break;
		case OID_802_11_CHANNEL_INFO:
			{
					channel_info ch_info;
					ch_info = *((pchannel_info)wrq.u.data.pointer);
					printf("channel = %d\n",ch_info.channel);
					printf("central channel = %d\n",ch_info.central_channel);
			}
			break;
		default:
			break;
				
	}
//	free(wrq.u.data.pointer);

//	printf(" After: iface %s, subcmd %d, buffer %p, length %d\n",
//			wrq.ifr_name, wrq.u.data.flags, wrq.u.data.pointer, wrq.u.data.length);

	return 0;
}
