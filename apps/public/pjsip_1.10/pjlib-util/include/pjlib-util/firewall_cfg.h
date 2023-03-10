/*  Copyright(c) 2009-2011 Shenzhen TP-LINK Technologies Co.Ltd.
 *
 * file		firewall_cfg.h
 * brief		provide an Interface to set netfilter rule
 * details	
 *
 * author	Yu Chuwei
 * version	1.0.0
 * date		21Nov11
 *
 * warning	
 *
 * history \arg	
 */
 
#ifndef __FIREWALL_CFG_H__
#define __FIREWALL_CFG_H__

#include <pjlib-util/types.h>
/**************************************************************************************************/
/*                                           DEFINES                                              */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           TYPES                                                */
/**************************************************************************************************/

/* 
 * brief	specify where to set iptables rules	
 */
typedef enum _PJ_FIREWALLCFG_CTRL
{
	PJ_FIREWALLCFG_IN_NONE = 0, /*set none*/
	PJ_FIREWALLCFG_IN_NETFILTER = 1, /*set in netfilter table*/
	PJ_FIREWALLCFG_IN_NAT_NETFILTER = 2 /*set in netfilter table and nat table*/
}PJ_FIREWALLCFG_CTRL;

typedef enum _PJ_FIREWALLCFG_ACTION
{
	PJ_FIREWALLCFG_ADD = 0, /*Add a new rule*/
	PJ_FIREWALLCFG_DEL = 1  /*Delete a rule*/
}PJ_FIREWALLCFG_ACTION;

/* 
 * brief	Type of transport
 */
typedef enum _PJ_TRANSPORT_PROTO
{
	PJ_TRANSPORT_UDP = 0, /*UDP*/
	PJ_TRANSPORT_TCP = 1, /*TCP*/
	PJ_TRANSPORT_ALL = 2	/*UDP and TCP*/
}PJ_TRANSPORT_PROTO;

/* 
 * brief	Represent a netfilter rule.
 */
typedef struct _PJ_FIREWALL_RULE
{
	PJ_TRANSPORT_PROTO protocol;	/*Transport protocol*/
	pj_str_t destination;			/*Destination IP of this rule*/
	pj_str_t dstNetmask;				/*Destination Netmask of this rule*/
	pj_int32_t dport;				/*Destination Port, if no port, it is -1*/
	pj_str_t source;					/*Source IP of this rule*/
	pj_str_t srcNetmask;				/*Source Netmask of this rule*/
	pj_int32_t sport;					/*Source Port, if no port, it is -1*/
	char dstBuf[16];
	char dstNetMaskBuf[16];
	char srcBuf[16];
	char srcNetMaskBuf[16];
}PJ_FIREWALL_RULE;
/**************************************************************************************************/
/*                                           VARIABLES                                            */
/**************************************************************************************************/

/**************************************************************************************************/
/*                                           FUNCTIONS                                            */
/**************************************************************************************************/

/* 
 * fn		pj_status_t pj_firewall_set_rule_accept(PJ_FIREWALLCFG_ACTION action, PJ_TRANSPORT_PROTO proto,
 *																char* dst, char* dstNetmask, pj_uint16_t dport,
 *																char* src, pj_uint16_t sport) 
 * brief	Add a rule to netfilter and the target is "ACCEPT". Table is "filter", chains are "INPUT" and "FORWARD".
 * details	
 *
 * param[in]	action:	to add or delete this rule. 
 *					proto:	network protocol of this rule."udp" or "tcp" or "udp or tcp"
 *					dst:		destination IP of this rule. now is ipv4. must not be NULL or "0.0.0.0".
 *					dstNetmask: netmask of the destination network, may be NULL or "0.0.0.0".
 *					dport:	port of the destination host. If value is -1, then need not set port.
 *					src:		source IP of this rule. now is ipv4. must not be NULL or "0.0.0.0".
 *					srcNetmask: netmask of the source network, may be NULL or "0.0.0.0".
 *					sport:	port of the source host.If value is -1, then need not set port.
 * param[out]	
 *
 * return	PJ_SUCCESS if set successfully.
 *				PJ_EINVAL if arguments are invalid.
 *				PJ_
 * retval	
 *
 * note		
 */
pj_status_t pj_firewall_set_rule_accept(PJ_FIREWALLCFG_ACTION action, PJ_TRANSPORT_PROTO proto,
											const pj_str_t* dst, const pj_str_t* dstNetmask, pj_int32_t dport,
											const pj_str_t* src, const pj_str_t* srcNetmask, pj_int32_t sport,
											PJ_FIREWALLCFG_CTRL ctrl
#ifdef INCLUDE_SOL
											, pj_uint32_t mark
#endif /* INCLUDE_SOL */
											);

#endif	/* __FIREWALL_CFG.H_H__ */
