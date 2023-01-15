/*	Copyright (c) 2009-2013 Shenzhen TP-LINK Technologies Co.Ltd
 *
 *	file		libipt_webWarn.c
 *	brief		user space iptables dynamic library
 *
 *	author		Wang Jianfeng
 *	version		1.0.0
 *	date		06Dec13
 *
 *	history		\arg 1.0.0, 06Dec13, Wang Jianfeng, Create the file
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <iptables.h>

#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_webWarn.h>



/************************************************************************/
/*								LOCAL_PROTOTYPES						*/
/************************************************************************/


static void init(struct xt_entry_target *t);

static void help(void);


static int parse(int c, char **argv, int invert, unsigned int *flags,
				const void *entry, struct xt_entry_target **target);

static void finalcheck(unsigned int flags);

static void save(const void *ip, const struct xt_entry_target *target);

static void print(const void *ip, const struct xt_entry_target *target, int numeric);


/************************************************************************/
/*								VARIABLES								*/
/************************************************************************/


/*
 * brief	options that this target support, currently it's empty
 */
static struct option opts[] =
{
	{ "", 0, 0, '1' },
	{ 0 }
};



/*
 * brief	used to register to iptables
 */
static struct xtables_target webwarn =
{
	.family			= AF_INET,
	.name			= "webWarn",
	.version		= XTABLES_VERSION,
	.size			= XT_ALIGN(sizeof(struct ipt_web_warn_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct ipt_web_warn_info)),
	.help			= help,
	.init			= init,
	.parse			= parse,
	.final_check	= finalcheck,
	.print			= print,
	.save			= save,
	.extra_opts		= opts,
};


/*
 * brief	used to register to iptables
 */
static struct xtables_target fakedns =
{
	.family			= AF_INET,
	.name			= "fakeDns",
	.version		= XTABLES_VERSION,
	.size			= XT_ALIGN(sizeof(struct ipt_web_warn_info)),
	.userspacesize	= XT_ALIGN(sizeof(struct ipt_web_warn_info)),
	.help			= help,
	.init			= init,
	.parse			= parse,
	.final_check	= finalcheck,
	.print			= print,
	.save			= save,
	.extra_opts		= opts,
};



/************************************************************************/
/*								LOCAL_FUNCTIONS							*/
/************************************************************************/


/* 
 *  fn			static void init(struct xt_entry_target *t)
 *  brief		init this dynamic library
 * 
 */
static void init(struct xt_entry_target *t)
{
}



/* 
 *  fn			static void help(void)
 *  brief		called by iptables when user input help command
 * 
 */
static void help(void)
{
	PDEBUG("this target catches the ip and tcp header of packet and return it to user space.\n");
}



/* 
 *  fn			static int parse(int c, char **argv, int invert, unsigned int *flags,
 *								const void *entry, struct xt_entry_target **target)
 *  brief		called when user input target command which is registered in this dynamic library 
 *	
 *	retval		1
 *
 */
static int parse(int c, char **argv, int invert, unsigned int *flags,
				const void *entry, struct xt_entry_target **target)
{
	return 1;
}



/* 
 *  fn			static void finalcheck(unsigned int flags)
 *  brief		called by iptables to do final check
 * 
 */
static void finalcheck(unsigned int flags)
{
	PDEBUG("webWarn check\n");
}



/* 
 *  fn			static void save(const void *ip, const struct xt_entry_target *target)
 *  brief		called by iptables when user invoke iptables-save 
 * 
 */
static void save(const void *ip, const struct xt_entry_target *target)
{
	PDEBUG("webWarn save\n");
}



/* 
 *  fn			static void print(const void *ip, const struct xt_entry_target *target, int numeric)
 *  brief		called by iptables when user invoke iptables -L
 * 
 */
static void print(const void *ip, const struct xt_entry_target *target, int numeric)
{
	PDEBUG("webWarn print\n");
}

/************************************************************************/
/*								PUBLIC_FUNCTIONS						*/
/************************************************************************/


/* 
 *  fn			void _init(void)
 *  brief		automatically called by iptables when loading this dynamic library
 * 
 *	retval		none
 *
 */
void _init(void)
{
	xtables_register_target(&fakedns);
	xtables_register_target(&webwarn);
}


