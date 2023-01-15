/*	Copyright (c) 2009-2013 Shenzhen TP-LINK Technologies Co.Ltd
 *
 *	file		libipt_webWarn.h
 *	brief		header file of libipt_webWarn.c
 *
 *	author		Wang Jianfeng
 *	version		1.0.0
 *	date		06Dec13
 *
 *	history		\arg 1.0.0, 06Dec13, Wang Jianfeng, Create the file
 */

#ifndef __WEB_WARN_KERNEL_H__
#define __WEB_WARN_KERNEL_H__


/************************************************************************/
/*							CONFIGURATIONS								*/
/************************************************************************/


#define WEB_WARN_IPT_DEBUG 0


/************************************************************************/
/*								DEFINES									*/
/************************************************************************/


#undef PDEBUG
#if WEB_WARN_IPT_DEBUG
	#ifdef __KERNEL__
		#define PDEBUG(fmt, args...) printk(KERN_DEBUG fmt, ##args)
	#else
		#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ##args)
	#endif
#else
	#define PDEBUG(fmt, args...)
#endif


/************************************************************************/
/*								TYPES									*/
/************************************************************************/


struct ipt_web_warn_info
{
	char empty;
};


#endif /* end of __WEB_WARN_KERNEL_H__ */
