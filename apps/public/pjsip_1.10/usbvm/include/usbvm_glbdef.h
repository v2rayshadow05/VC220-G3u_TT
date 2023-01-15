/*  Copyright(c) 2009-2011 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		usbvm_glbdef.h
 * brief		
 * details	
 *
 * author	Sirrain Zhang
 * version	
 * date		19Jul11
 *
 * history 	\arg	
 */


#ifndef __USBVM_GLBDEF_H__
#define __USBVM_GLBDEF_H__

#include <sched.h>
#include <pj/compat/m_auto.h>		/* for byte order */

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/
/* If USBVM_DEBUG==1, will print debug info and assertion is on
 *    USBVM_DEBUG==10, whole message
 */
#define USBVM_DEBUG 0


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef UNKNOWN
#define UNKNOWN -1
#endif

#ifndef PJ_IS_BIG_ENDIAN
#error  "======== usb voice mail MUST get byte order ======="
#endif

#ifndef ENDPT_NUM
#ifdef NUM_DECT_CHANNELS
#define ENDPT_NUM NUM_FXS_CHANNELS + NUM_DECT_CHANNELS
#else
#define ENDPT_NUM  NUM_FXS_CHANNELS	/* endpoint number */
#endif /* NUM_DECT_CHANNELS */ 
#endif

#define CMM_COMPILE  1                  /* Compile CMM or not */

#define TASK_PRIORITY_LOWEST    (sched_get_priority_min(SCHED_FIFO))
#define TASK_PRIORITY_MED_LOW   ((sched_get_priority_max(SCHED_FIFO) + 1)/4)
#define TASK_PRIORITY_MED       ((sched_get_priority_max(SCHED_FIFO) + 1)/2)
#define TASK_PRIORITY_MED_HIGH  ((sched_get_priority_max(SCHED_FIFO) + 1)/4 * 3)
#define TASK_PRIORITY_HIGHEST   (sched_get_priority_max(SCHED_FIFO))

#define USB_VOICEMAIL_ENDIS_MASK 2   // bits->1*
#define USB_VOICEMAIL_DISABLE    0   // bits->0*
#define USB_VOICEMAIL_ENABLE     2   // bits->1*

#define USB_VOICEMAIL_MODE_MASK  1   // bits->*1
#define USB_VOICEMAIL_NOANS      0   // bits->*0
#define USB_VOICEMAIL_UNCON      1   // bits->*1

#define USB_VOICEMAIL_ENDIS_MODE_MASK 3   // bits->11
#define USB_VOICEMAIL_DISABLE_NOANS   0   // bits->00
#define USB_VOICEMAIL_DISABLE_UNCON   1   // bits->01
#define USB_VOICEMAIL_ENABLE_NOANS    2   // bits->10
#define USB_VOICEMAIL_ENABLE_UNCON    3   // bits->11

#define USB_VOICEMAIL_PINNUM_LEN      16

#ifndef MIN
#define MIN(a,b)     (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b)     (((a) > (b)) ? (a) : (b))
#endif

#define RTP_PAYLOAD_PCMU      0
#define RTP_PAYLOAD_PCMA      8
#define RTP_PAYLOAD_G726_16   98
#define RTP_PAYLOAD_G726_24   97
#define RTP_PAYLOAD_G726_32   2
#define RTP_PAYLOAD_G726_40   96
#define RTP_PAYLOAD_G729A     18
#define RTP_PAYLOAD_G729      18
#define RTP_PAYLOAD_G7231_63  4 
#define RTP_PAYLOAD_G7231_53  4
#define RTP_PAYLOAD_NULL      255

#define CNG_PAYLOAD_TYPE  13
#define CNG_PAYLOAD_TYPE_OLD 19 	/* the old CNG pt is 19,before RFC3389 */
#define CNG_LEVEL 0x4e		/* the suggested cng level  in RFC3389 is 78. */
#define CNG_TIMEVAL 200		/* in ms */
#define PCMU_MUTE_DATA    0xFF /* raw data is 0 */
#define PCMA_MUTE_DATA    0xD5 /* raw data is 0 */


/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/
/* Voice codec types */
typedef enum
{
   USBVM_CODEC_NULL  = (0),        /* NULL */
   USBVM_CODEC_PCMU,               /* G.711 ulaw */
   USBVM_CODEC_PCMA,               /* G.711 alaw */
   USBVM_CODEC_G726_16,            /* G.726 - 16 kbps */
   USBVM_CODEC_G726_24,            /* G.726 - 24 kbps */
   USBVM_CODEC_G726_32,            /* G.726 - 32 kbps */
   USBVM_CODEC_G726_40,            /* G.726 - 40 kbps */
   USBVM_CODEC_G7231_53,           /* G.723.1 - 5.3 kbps */
   USBVM_CODEC_G7231_63,           /* G.723.1 - 6.3 kbps */
   USBVM_CODEC_G7231A_53,          /* G.723.1A - 5.3 kbps */
   USBVM_CODEC_G7231A_63,          /* G.723.1A - 6.3 kbps */
   USBVM_CODEC_G729A,              /* G.729A */
   USBVM_CODEC_G729B,              /* G.729B */
   USBVM_CODEC_G711_LINEAR,        /* Linear media queue data */
   USBVM_CODEC_G728,               /* G.728 */
   USBVM_CODEC_G729,               /* G.729 */
   USBVM_CODEC_G729E,              /* G.729E */
   USBVM_CODEC_BV16,               /* BRCM Broadvoice - 16 kbps */
   USBVM_CODEC_BV32,               /* BRCM Broadvoice - 32 kbps */
   USBVM_CODEC_NTE,                /* Named telephone events */
   USBVM_CODEC_ILBC_20,            /* iLBC speech coder - 20 ms frame / 15.2 kbps */
   USBVM_CODEC_ILBC_30,            /* iLBC speech coder - 30 ms frame / 13.3 kbps */
   USBVM_CODEC_G7231_53_VAR,       /* G723.1 variable rates (preferred=5.3) */
   USBVM_CODEC_G7231_63_VAR,       /* G723.1 variable rates (preferred=6.3) */
   USBVM_CODEC_G7231_VAR,          /* G723.1 variable rates */
   USBVM_CODEC_T38,                /* T.38 fax relay */
   USBVM_CODEC_T38_MUTE,           /* Mute before switching to T.38 fax relay */
   USBVM_CODEC_RED,                /* Redundancy - RFC 2198 */
   USBVM_CODEC_G722_MODE_1,        /* G.722 Mode 1 64 kbps */
   USBVM_CODEC_LINPCM128,          /* Narrowband linear PCM @ 128 Kbps */
   USBVM_CODEC_LINPCM256,          /* Wideband linear PCM @ 256 Kbps */

   USBVM_CODEC_GSMAMR_12K,         /* GSM AMR codec @ 12.2 kbps */
   USBVM_CODEC_GSMAMR_10K,         /* GSM AMR codec @ 10.2 kbps */
   USBVM_CODEC_GSMAMR_795,         /* GSM AMR codec @ 7.95 kbps */
   USBVM_CODEC_GSMAMR_740,         /* GSM AMR codec @ 7.4 kbps */
   USBVM_CODEC_GSMAMR_670,         /* GSM AMR codec @ 6.7 kbps */
   USBVM_CODEC_GSMAMR_590,         /* GSM AMR codec @ 5.9 kbps */
   USBVM_CODEC_GSMAMR_515,         /* GSM AMR codec @ 5.15 kbps */
   USBVM_CODEC_GSMAMR_475,         /* GSM AMR codec @ 4.75 kbps */

   USBVM_CODEC_AMRWB_66,           /* AMR WB codec @ 6.6 kbps */
   USBVM_CODEC_AMRWB_885,          /* AMR WB codec @ 8.85 kbps */
   USBVM_CODEC_AMRWB_1265,         /* AMR WB codec @ 12.65 kbps */
   USBVM_CODEC_AMRWB_1425,         /* AMR WB codec @ 14.25 kbps */
   USBVM_CODEC_AMRWB_1585,         /* AMR WB codec @ 15.85 kbps */
   USBVM_CODEC_AMRWB_1825,         /* AMR WB codec @ 18.25 kbps */
   USBVM_CODEC_AMRWB_1985,         /* AMR WB codec @ 19.85 kbps */
   USBVM_CODEC_AMRWB_2305,         /* AMR WB codec @ 23.05 kbps */
   USBVM_CODEC_AMRWB_2385,         /* AMR WB codec @ 23.85 kbps */
   
   /* Maximum number of codec types. */
   USBVM_CODEC_MAX_TYPES,

   /* Place-holder for dynamic codec types that haven't been mapped yet */
   USBVM_CODEC_DYNAMIC        = (0xffff),

   /* Place-holder for unknown codec types that should be ignored/removed from list */
   USBVM_CODEC_UNKNOWN        = (0xfffe)
} USBVM_CODEC_TYPE;

/* RTP named telephone events, as defined in RFC2833 */
typedef enum
{
   /* DTMF named events */
   USBVM_RTP_NTE_DTMF0     = 0,           /* DTMF Tone 0 event */
   USBVM_RTP_NTE_DTMF1     = 1,           /* DTMF Tone 1 event */
   USBVM_RTP_NTE_DTMF2     = 2,           /* DTMF Tone 2 event */
   USBVM_RTP_NTE_DTMF3     = 3,           /* DTMF Tone 3 event */
   USBVM_RTP_NTE_DTMF4     = 4,           /* DTMF Tone 4 event */
   USBVM_RTP_NTE_DTMF5     = 5,           /* DTMF Tone 5 event */
   USBVM_RTP_NTE_DTMF6     = 6,           /* DTMF Tone 6 event */
   USBVM_RTP_NTE_DTMF7     = 7,           /* DTMF Tone 7 event */
   USBVM_RTP_NTE_DTMF8     = 8,           /* DTMF Tone 8 event */
   USBVM_RTP_NTE_DTMF9     = 9,           /* DTMF Tone 9 event */
   USBVM_RTP_NTE_DTMFS     = 10,          /* DTMF Tone * event */
   USBVM_RTP_NTE_DTMFH     = 11,          /* DTMF Tone # event */
   USBVM_RTP_NTE_DTMFA     = 12,          /* DTMF Tone A event */
   USBVM_RTP_NTE_DTMFB     = 13,          /* DTMF Tone B event */
   USBVM_RTP_NTE_DTMFC     = 14,          /* DTMF Tone C event */
   USBVM_RTP_NTE_DTMFD     = 15,          /* DTMF Tone D event */

   /* LCS named events */
   USBVM_RTP_NTE_RING      = 144,         /* Ringing event */
   USBVM_RTP_NTE_ONHOOK    = 149,         /* Onhook event */
   USBVM_RTP_NTE_OSI       = 159,         /* Open switch interval event */

   /* GR303 named events */
   USBVM_RTP_NTE_RINGOFF   = 149,         /* Ringing off event */
   USBVM_RTP_NTE_OFFHOOK   = 159          /* Offhook event */

} USBVM_RTP_NTE;

/*20130618_update*/
#ifndef __FILE_NO_DIR__
#define __FILE_NO_DIR__		((strrchr(__FILE__, '/') ?: __FILE__-1)+1)
#endif /* __FILE_NO_DIR__ */
#if defined(USBVM_DEBUG) && (USBVM_DEBUG >= 7)
#define TT_PRINT(format, args...)		printf("%s:%d " format "\n", __FILE_NO_DIR__, __LINE__, ##args)
#else
#define TT_PRINT(format, args...) 
#endif /* defined(USBVM_DEBUG) && (USBVM_DEBUG >= 7) */


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */


#endif	/* __USBVM_GLBDEF_H__ */

