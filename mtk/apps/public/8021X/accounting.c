/*
 * hostapd / RADIUS Accounting
 * Copyright (c) 2002-2009, 2012, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifdef ACCOUNTING_SUPPORT
//#include "utils/includes.h"

#include "common.h"
#include "rtmp_type.h"
#include "eloop.h"
#include "eapol_sm.h"

//#include "eapol_auth/eapol_auth_sm_i.h"
#include "radius.h"
#include "radius_client.h"
//#include "hostapd.h"
#include "ieee802_1x.h"
//#include "ap_config.h"
#include "sta_info.h"
//#include "ap_drv_ops.h"
#include "accounting.h"

#include <linux/wireless.h>
#include <string.h>


/* Default interval in seconds for polling TX/RX octets from the driver if
 * STA is not using interim accounting. This detects wrap arounds for
 * input/output octets and updates Acct-{Input,Output}-Gigawords. */
#define ACCT_DEFAULT_UPDATE_INTERVAL 300

#if 0 		//enable later
static void accounting_sta_interim(struct apd_data *rtapd,
				   struct sta_info *sta);
#endif

int os_get_time(struct os_time *t)
{
	int res;
	struct timeval tv;
	res = gettimeofday(&tv, NULL);
	t->sec = tv.tv_sec;
	t->usec = tv.tv_usec;
	return res;
}

static inline void os_reltime_sub(struct os_time *a, struct os_time *b,
				  struct os_time *res)
{
	res->sec = a->sec - b->sec;
	res->usec = a->usec - b->usec;
	if (res->usec < 0) {
		res->sec--;
		res->usec += 1000000;
	}
}

struct apd_radius_attr *
apd_config_get_radius_attr(struct apd_radius_attr *attr, u8 type)
{
	for (; attr; attr = attr->next) {
		if (attr->type == type)
			return attr;
	}
	return NULL;
}

static struct radius_msg * accounting_msg(struct apd_data *rtapd,
					  struct sta_info *sta,
					  int status_type, int ApIdx)
{
	struct radius_msg *msg;
	char buf[128];
	u8 *val;
	size_t len;
//	int i;
//	struct wpabuf *b;

	msg = Radius_msg_new(RADIUS_CODE_ACCOUNTING_REQUEST,
			     Radius_client_get_id(rtapd));
	if (msg == NULL) {
		DBGPRINT(RT_DEBUG_ERROR, "Could not create new RADIUS packet");
		return NULL;
	}

	if (sta) {
		Radius_msg_make_authenticator(msg, (u8 *) sta, sizeof(*sta));

		/*vikas:to do fix
		if ((rtapd->conf->wpa & 2) && 
		    !rtapd->conf->disable_pmksa_caching &&
		    sta->eapol_sm && sta->eapol_sm->acct_multi_session_id_hi) 
		{
			os_snprintf(buf, sizeof(buf), "%08X+%08X",
				    sta->eapol_sm->acct_multi_session_id_hi,
				    sta->eapol_sm->acct_multi_session_id_lo);
			if (!Radius_msg_add_attr(msg, RADIUS_ATTR_ACCT_MULTI_SESSION_ID,(u8 *) buf, strlen(buf))) 
			{
				DBGPRINT(RT_DEBUG_ERROR, "Could not add Acct-Multi-Session-Id");
				goto fail;
			}
		}
		*/
	} else {
		Radius_msg_make_authenticator(msg, (u8 *) rtapd, sizeof(*rtapd));
		/*DBGPRINT(RT_DEBUG_ERROR, "%s:Authenticator:%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x ",__FUNCTION__,
			msg->hdr->authenticator[0],msg->hdr->authenticator[1],msg->hdr->authenticator[2],msg->hdr->authenticator[3],
			msg->hdr->authenticator[4],msg->hdr->authenticator[5],msg->hdr->authenticator[6],msg->hdr->authenticator[7],
			msg->hdr->authenticator[8],msg->hdr->authenticator[9],msg->hdr->authenticator[10],msg->hdr->authenticator[11],
			msg->hdr->authenticator[12],msg->hdr->authenticator[13],msg->hdr->authenticator[14],msg->hdr->authenticator[15]);*/
	}

	if (!Radius_msg_add_attr_int32(msg, RADIUS_ATTR_ACCT_STATUS_TYPE,
				       status_type)) {
		DBGPRINT(RT_DEBUG_ERROR, "Could not add Acct-Status-Type");
		goto fail;
	}

	if (!Radius_msg_add_attr_int32(msg, RADIUS_ATTR_ACCT_AUTHENTIC,
				       rtapd->conf->radius_acct_authentic[ApIdx])) {
		DBGPRINT(RT_DEBUG_ERROR, "Could not add Acct-Authentic");
		goto fail;
	}

	if (sta) {
		/* Use 802.1X identity if available */
		val = ieee802_1x_get_identity(sta->eapol_sm, &len);

		/* Use RADIUS ACL identity if 802.1X provides no identity */
		if (!val && sta->identity) {
			val = (u8 *) sta->identity;
			//len = strlen(sta->identity);
			len = sta->identity_len;
		}

		/* Use STA MAC if neither 802.1X nor RADIUS ACL provided
		 * identity */
		if (!val) {
			os_snprintf(buf, sizeof(buf), RADIUS_ADDR_FORMAT,
				    MAC2STR(sta->addr));
			val = (u8 *) buf;
			len = os_strlen(buf);
		}

		if (!Radius_msg_add_attr(msg, RADIUS_ATTR_USER_NAME, val,
					 len)) {
			DBGPRINT(RT_DEBUG_ERROR, "Could not add User-Name");
			goto fail;
		}
	}

if (add_common_radius_attr(rtapd, sta,
				   msg, ApIdx) < 0)
		goto fail;

	if (sta) {
		/*vikas:todo fix
		for (i = 0; ; i++) {
			val = ieee802_1x_get_radius_class(sta->eapol_sm, &len,
							  i);
			if (val == NULL)
				break;

			if (!Radius_msg_add_attr(msg, RADIUS_ATTR_CLASS,
						 val, len)) {
				DBGPRINT(RT_DEBUG_INFO, "Could not add Class");
				goto fail;
			}
		}

		b = ieee802_1x_get_radius_cui(sta->eapol_sm);
		if (b &&
		    !Radius_msg_add_attr(msg,
					 RADIUS_ATTR_CHARGEABLE_USER_IDENTITY,
					 b->buf, b->used)) {
			DBGPRINT(RT_DEBUG_INFO, "Could not add CUI");
			goto fail;
		}

		if (!b && sta->radius_cui &&
		    !Radius_msg_add_attr(msg,
					 RADIUS_ATTR_CHARGEABLE_USER_IDENTITY,
					 (u8 *) sta->radius_cui,
					 os_strlen(sta->radius_cui))) {
			DBGPRINT(RT_DEBUG_INFO, "Could not add CUI from ACL");
			goto fail;
		}
		*/
	}

	return msg;

 fail:
	Radius_msg_free(msg);
	return NULL;
}


#if 1

#define MAX_BYTE 4294967295u			//2^32 - 1

static int accounting_sta_update_stats(struct apd_data *rtapd,
				       struct sta_info *sta,
				       DOT1X_QUERY_STA_DATA *data,
				       int stop)
{
	//if (rtapd_drv_read_sta_data(rtapd, sta, &data))
		//return -1;
	//unsigned long inbytes, outbytes;
	if(stop)
	{
		data->rx_bytes = sta->final_rx_bytes;
		data->tx_bytes = sta->final_tx_bytes;
		data->rx_packets = sta->final_rx_packets;
		data->tx_packets = sta->final_tx_packets;
		memcpy(data->StaAddr, sta->addr, MAC_ADDR_LEN);
	}
	else
	{
		memset(data, 0, sizeof(DOT1X_QUERY_STA_DATA));
	    memcpy(data->StaAddr, sta->addr, MAC_ADDR_LEN);

		DBGPRINT(RT_DEBUG_TRACE, "%s::OID_802_DOT1X_QUERY_STA_DATA(%02x:%02x:%02x:%02x:%02x:%02x)\n",
						__FUNCTION__, MAC2STR(data->StaAddr));

		if (RT_ioctl(rtapd->ioctl_sock, RT_PRIV_IOCTL, (unsigned char *)data, sizeof(DOT1X_QUERY_STA_DATA),
	                	 			rtapd->prefix_wlan_name, sta->ApIdx, OID_802_DOT1X_QUERY_STA_DATA))
	    {
			DBGPRINT(RT_DEBUG_ERROR,"IOCTL ERROR with OID_802_DOT1X_QUERY_STA_DATA\n");
		}
		else
			DBGPRINT(RT_DEBUG_TRACE,"%s::rx_pkt:%lu  tx_pkt:%lu  rx_byte:%lu  tx_byte:%lu\n",
						__FUNCTION__, data->rx_packets,data->tx_packets, data->rx_bytes, data->tx_bytes);
	}

#if 0
	//use if exact bytes reqd for tx/rx, else upperlogic, does not reset driver data on acct_start, so added 2-3 kB in acct.
	
	if(data->rx_bytes < sta->last_rx_bytes)
		inbytes = (MAX_BYTE - sta->last_rx_bytes) + data->rx_bytes;
	else
		inbytes = data->rx_bytes - sta->last_rx_bytes;

	if(inbytes > (MAX_BYTE - sta->srvr_last_rx_byte))
	{
		sta->acct_input_gigawords++;
		sta->srvr_rx_byte = inbytes - (MAX_BYTE - sta->srvr_last_rx_byte);
	}
	else
		sta->srvr_rx_byte = inbytes + sta->srvr_last_rx_byte;


	if(data->tx_bytes < sta->last_tx_bytes)
		outbytes = (MAX_BYTE - sta->last_tx_bytes) + data->tx_bytes;
	else
		outbytes = data->tx_bytes - sta->last_tx_bytes;

	if(outbytes > (MAX_BYTE - sta->srvr_last_tx_byte))
	{
		sta->acct_output_gigawords++;
		sta->srvr_tx_byte = outbytes - (MAX_BYTE - sta->srvr_last_tx_byte);
	}
	else
		sta->srvr_tx_byte = outbytes + sta->srvr_last_tx_byte;

	
	DBGPRINT(RT_DEBUG_TRACE, "updated TX/RX stats: "
		       "Acct-Input-Octets=%lu Acct-Input-Gigawords=%u "
		       "Acct-Output-Octets=%lu Acct-Output-Gigawords=%u\n",
		       sta->srvr_rx_byte, sta->acct_input_gigawords,
		       sta->srvr_tx_byte, sta->acct_output_gigawords);
#else

	if (sta->last_rx_bytes > data->rx_bytes)
		sta->acct_input_gigawords++;
	if (sta->last_tx_bytes > data->tx_bytes)
		sta->acct_output_gigawords++;
	sta->last_rx_bytes = data->rx_bytes;
	sta->last_tx_bytes = data->tx_bytes;

	DBGPRINT(RT_DEBUG_TRACE, "%s: updated TX/RX stats: "
		       "Acct-Input-Octets=%lu Acct-Input-Gigawords=%u "
		       "Acct-Output-Octets=%lu Acct-Output-Gigawords=%u\n",
		       __FUNCTION__, data->rx_bytes, sta->acct_input_gigawords,
		       				 data->tx_bytes, sta->acct_output_gigawords);
#endif
	
	return 0;
}


static void accounting_interim_update(void *eloop_ctx, void *timeout_ctx)
{
	struct apd_data *rtapd = eloop_ctx;
	struct sta_info *sta = timeout_ctx;
	int interval;

	if (sta->acct_interim_interval) {
		accounting_sta_interim(rtapd, sta);
		interval = sta->acct_interim_interval;
	} else {
		DOT1X_QUERY_STA_DATA data;
		accounting_sta_update_stats(rtapd, sta, &data, 0);
		interval = ACCT_DEFAULT_UPDATE_INTERVAL;
	}

	eloop_register_timeout(interval, 0, accounting_interim_update,
			       rtapd, sta);
}


/**
 * accounting_sta_start - Start STA accounting
 * @hapd: hostapd BSS data
 * @sta: The station
 */
void accounting_sta_start(struct apd_data *rtapd, struct sta_info *sta)
{
	struct radius_msg *msg;
	DOT1X_QUERY_STA_DATA data;
		
	int interval;

	//data = malloc(sizeof(*data));
	
	if (sta->acct_session_started)
		return;

	DBGPRINT(RT_DEBUG_OFF, "STA[%d]:starting accounting session %08X-%08X\n",
		       sta->aid, sta->acct_session_id_hi, sta->acct_session_id_lo);

	os_get_time(&sta->acct_session_start);
	
	sta->last_rx_bytes = sta->last_tx_bytes = 0;
	sta->last_rx_packets = sta->last_tx_packets = 0;
	sta->acct_input_gigawords = sta->acct_output_gigawords = 0;
	//sta->init_rx_bytes = sta->init_tx_bytes = 0;
	//sta->init_rx_packets = sta->init_tx_packets = 0;

	//hostapd_drv_sta_clear_stats(rtapd, sta->addr);
	//accounting_sta_update_stats(rtapd, sta, &data, 0);
	
#if 0
	/*memset(&data, 0, sizeof(struct sta_driver_data));
    memcpy(data.addr, sta->addr, MAC_ADDR_LEN);

	if (RT_ioctl(rtapd->ioctl_sock, RT_PRIV_IOCTL, (char *)&data, sizeof(struct sta_driver_data),
                	 			rtapd->prefix_wlan_name, sta->ApIdx, OID_802_DOT1X_QUERY_STA_DATA))
    {
		DBGPRINT(RT_DEBUG_ERROR,"IOCTL ERROR with OID_802_DOT1X_QUERY_STA_DATA\n");
	}

	sta->last_rx_bytes = data->rx_bytes;
	sta->last_tx_bytes = data->tx_bytes;*/
#endif
	sta->last_rx_packets= data.rx_packets;
	sta->last_tx_packets= data.tx_packets;
	
	//sta->init_rx_bytes = data->rx_bytes;
	//sta->init_tx_bytes = data->tx_bytes;
	//sta->init_rx_packets = data.rx_packets;
	//sta->init_tx_packets = data.tx_packets;

#if MULTIPLE_RADIUS
	if (!rtapd->conf->mbss_acct_servers[sta->ApIdx])
		return;
#else
	if (!rtapd->conf->acct_server)
		return;
#endif

	if (sta->acct_interim_interval)
		interval = sta->acct_interim_interval;
	else
		interval = ACCT_DEFAULT_UPDATE_INTERVAL;
	/*vikas:todo enable later*/
	eloop_register_timeout(interval, 0, accounting_interim_update,
			       rtapd, sta);

	msg = accounting_msg(rtapd, sta, RADIUS_ACCT_STATUS_TYPE_START, sta->ApIdx);
	if (msg &&
	    Radius_client_send(rtapd, msg, RADIUS_ACCT, sta->ApIdx) < 0)
		Radius_msg_free(msg);

	sta->acct_session_started = 1;
}


static void accounting_sta_report(struct apd_data *rtapd,
				  struct sta_info *sta, int stop)
{
	struct radius_msg *msg;
	int cause = sta->acct_terminate_cause;
	DOT1X_QUERY_STA_DATA data;
	struct os_time now_r, diff;
	struct os_time now;
	u32 gigawords;

#if MULTIPLE_RADIUS
	if (!rtapd->conf->mbss_acct_servers[sta->ApIdx])
			return;
#else
	if (!rtapd->conf->acct_server)
		return;
#endif

	msg = accounting_msg(rtapd, sta,
			     stop ? RADIUS_ACCT_STATUS_TYPE_STOP :
			     RADIUS_ACCT_STATUS_TYPE_INTERIM_UPDATE, sta->ApIdx);
	if (!msg) {
		DBGPRINT(RT_DEBUG_TRACE, "Could not create RADIUS Accounting message");
		return;
	}

	os_get_time(&now_r);
	os_get_time(&now);
	os_reltime_sub(&now_r, &sta->acct_session_start, &diff);
	if (!Radius_msg_add_attr_int32(msg, RADIUS_ATTR_ACCT_SESSION_TIME,
				       diff.sec)) {
		DBGPRINT(RT_DEBUG_TRACE, "Could not add Acct-Session-Time");
		goto fail;
	}

	if (accounting_sta_update_stats(rtapd, sta, &data, stop) == 0) 
	{
		if (!Radius_msg_add_attr_int32(msg,
					       RADIUS_ATTR_ACCT_INPUT_PACKETS,
					       data.rx_packets)) 		//data.rx_packets - sta->init_rx_packets
		{
			DBGPRINT(RT_DEBUG_TRACE, "Could not add Acct-Input-Packets");
			goto fail;
		}
		if (!Radius_msg_add_attr_int32(msg,
					       RADIUS_ATTR_ACCT_OUTPUT_PACKETS,
					       data.tx_packets)) 			//data.tx_packets - sta->init_tx_packets
		{
			DBGPRINT(RT_DEBUG_TRACE, "Could not add Acct-Output-Packets");
			goto fail;
		}
		if (!Radius_msg_add_attr_int32(msg,
					       RADIUS_ATTR_ACCT_INPUT_OCTETS,
					       data.rx_bytes))				//sta->srvr_rx_byte
		{
			DBGPRINT(RT_DEBUG_TRACE, "Could not add Acct-Input-Octets");
			goto fail;
		}
		gigawords = sta->acct_input_gigawords;
#if __WORDSIZE == 64
		gigawords += data.rx_bytes >> 32;
#endif
		if (gigawords &&
		    !Radius_msg_add_attr_int32(
			    msg, RADIUS_ATTR_ACCT_INPUT_GIGAWORDS,
			    gigawords)) 
		{
			DBGPRINT(RT_DEBUG_TRACE, "Could not add Acct-Input-Gigawords");
			goto fail;
		}
		if (!Radius_msg_add_attr_int32(msg,
					       RADIUS_ATTR_ACCT_OUTPUT_OCTETS,
					       (data.tx_bytes))) 		//sta->srvr_tx_byte
		{
			DBGPRINT(RT_DEBUG_TRACE, "Could not add Acct-Output-Octets");
			goto fail;
		}
		gigawords = sta->acct_output_gigawords;
#if __WORDSIZE == 64
		gigawords += data.tx_bytes >> 32;
#endif
		if (gigawords &&
		    !Radius_msg_add_attr_int32(
			    msg, RADIUS_ATTR_ACCT_OUTPUT_GIGAWORDS,
			    gigawords)) 
		{
			DBGPRINT(RT_DEBUG_TRACE, "Could not add Acct-Output-Gigawords");
			goto fail;
		}
	}

	if (!Radius_msg_add_attr_int32(msg, RADIUS_ATTR_EVENT_TIMESTAMP,
				       now.sec)) {
		DBGPRINT(RT_DEBUG_TRACE, "Could not add Event-Timestamp");
		goto fail;
	}

	if (eloop_terminated())
		cause = RADIUS_ACCT_TERMINATE_CAUSE_ADMIN_REBOOT;

	if (stop && cause &&
	    !Radius_msg_add_attr_int32(msg, RADIUS_ATTR_ACCT_TERMINATE_CAUSE,
				       cause)) {
		DBGPRINT(RT_DEBUG_TRACE, "Could not add Acct-Terminate-Cause");
		goto fail;
	}

	if (Radius_client_send(rtapd, msg,
			       stop ? RADIUS_ACCT : RADIUS_ACCT_INTERIM,
			       sta->ApIdx) < 0)
		goto fail;
	return;

 fail:
	Radius_msg_free(msg);
}


/**
 * accounting_sta_interim - Send a interim STA accounting report
 * @hapd: hostapd BSS data
 * @sta: The station
 */
void accounting_sta_interim(struct apd_data *hapd,
				   struct sta_info *sta)
{
	if (sta->acct_session_started)
		accounting_sta_report(hapd, sta, 0);
}


/**
 * accounting_sta_stop - Stop STA accounting
 * @hapd: hostapd BSS data
 * @sta: The station
 */
void accounting_sta_stop(struct apd_data *hapd, struct sta_info *sta)
{
	if (sta->acct_session_started) {
		accounting_sta_report(hapd, sta, 1);
		eloop_cancel_timeout(accounting_interim_update, hapd, sta); // vikas:todo enable later
		DBGPRINT(RT_DEBUG_TRACE, "stopped accounting session %08X-%08X",
			       sta->acct_session_id_hi,
			       sta->acct_session_id_lo);
		sta->acct_session_started = 0;
	}
}

#endif

void accounting_sta_get_id(struct apd_data *rtapd,
				  struct sta_info *sta)
{
	sta->acct_session_id_lo = rtapd->conf->acct_session_id_lo++;
	if (rtapd->conf->acct_session_id_lo == 0) {
		rtapd->conf->acct_session_id_hi++;
	}
	sta->acct_session_id_hi = rtapd->conf->acct_session_id_hi;
}


/**
 * accounting_receive - Process the RADIUS frames from Accounting Server
 * @msg: RADIUS response message
 * @req: RADIUS request message
 * @shared_secret: RADIUS shared secret
 * @shared_secret_len: Length of shared_secret in octets
 * @data: Context data (struct hostapd_data *)
 * Returns: Processing status
 */
static RadiusRxResult
accounting_receive(struct apd_data *rtapd, struct radius_msg *msg, struct radius_msg *req,
		   u8 *shared_secret, size_t shared_secret_len,
		   void *data)
{
	if (msg->hdr->code != RADIUS_CODE_ACCOUNTING_RESPONSE) {
		DBGPRINT(RT_DEBUG_ERROR, "Unknown RADIUS message code");
		return RADIUS_RX_UNKNOWN;
	}

	if (Radius_msg_verify(msg, shared_secret, shared_secret_len, req, 0)) {
		DBGPRINT(RT_DEBUG_ERROR, "Incoming RADIUS packet did not have correct Authenticator - dropped");
		return RADIUS_RX_INVALID_AUTHENTICATOR;
	}

	return RADIUS_RX_PROCESSED;
}


static void accounting_report_state(struct apd_data *rtapd, int on)
{
	struct radius_msg *msg;
#if MULTIPLE_RADIUS
	int i, s;
#endif
	//int i, s, res;
	//unsigned char buf[3000];
	struct sockaddr_in serv;
	memset(&serv, 0, sizeof(serv));
	serv.sin_family = AF_INET;	
	
#if MULTIPLE_RADIUS
	for(i = 0; i < rtapd->conf->SsidNum; i++)
	{
	if(
		#ifdef BSS_SKIP_SUPPORT
		rtapd->conf->SsidEnabled[i] && 
		#endif
		rtapd->conf->acct_enable[i])
	{
		if (!rtapd->conf->mbss_acct_server[i] || rtapd->radius == NULL)
		return;

		/* Inform RADIUS server that accounting will start/stop so that the
		 * server can close old accounting sessions. */
		msg = accounting_msg(rtapd, NULL,
				     on ? RADIUS_ACCT_STATUS_TYPE_ACCOUNTING_ON :
				     RADIUS_ACCT_STATUS_TYPE_ACCOUNTING_OFF, i);
		if (!msg)
			return;

		if (!Radius_msg_add_attr_int32(msg, RADIUS_ATTR_ACCT_TERMINATE_CAUSE,
					       RADIUS_ACCT_TERMINATE_CAUSE_NAS_REBOOT))
		{
			DBGPRINT(RT_DEBUG_ERROR, "Could not add Acct-Terminate-Cause");
			Radius_msg_free(msg);
			return;
		}

		if (Radius_client_send(rtapd, msg, RADIUS_ACCT, i) < 0)
			Radius_msg_free(msg);

		s = rtapd->radius->mbss_acct_serv_sock[i];
		serv.sin_addr.s_addr = rtapd->conf->mbss_acct_server[i]->addr.s_addr;
		serv.sin_port = htons(rtapd->conf->mbss_acct_server[i]->port);
		
		/*res = recvfrom( s, msg->buf, msg->buf_used, 0, ( struct sockaddr * )&serv,
				sizeof(serv) );
		res = recv(s, buf, sizeof(buf), 0);
		if (res < 0)
		{
			perror("recv_vikas[RADIUS]");
			return;
		}
		else
		{
			printf("chal gya recv size:%d\n", res);
		}*/
	}	
	}
#else
	if (!rtapd->conf->acct_server || rtapd->radius == NULL)
		return;

	/* Inform RADIUS server that accounting will start/stop so that the
	 * server can close old accounting sessions. */
	msg = accounting_msg(rtapd, NULL,
			     on ? RADIUS_ACCT_STATUS_TYPE_ACCOUNTING_ON :
			     RADIUS_ACCT_STATUS_TYPE_ACCOUNTING_OFF,0);
	if (!msg)
		return;

	if (!Radius_msg_add_attr_int32(msg, RADIUS_ATTR_ACCT_TERMINATE_CAUSE,
				       RADIUS_ACCT_TERMINATE_CAUSE_NAS_REBOOT))
	{
		DBGPRINT(RT_DEBUG_ERROR, "Could not add Acct-Terminate-Cause");
		Radius_msg_free(msg);
		return;
	}

	if (Radius_client_send(rtapd, msg, RADIUS_ACCT, 0) < 0)
		Radius_msg_free(msg);
#endif
}


/**
 * accounting_init: Initialize accounting
 * @hapd: hostapd BSS data
 * Returns: 0 on success, -1 on failure
 */
int accounting_init(struct apd_data *rtapd)
{
	struct os_time now;

	/* Acct-Session-Id should be unique over reboots. If reliable clock is
	 * not available, this could be replaced with reboot counter, etc. */
	os_get_time(&now);
	rtapd->conf->acct_session_id_hi = now.sec;

	if (Radius_client_register(rtapd, RADIUS_ACCT,accounting_receive, rtapd))
		return -1;

	accounting_report_state(rtapd, 1);

	return 0;
}


/**
 * accounting_deinit: Deinitilize accounting
 * @hapd: hostapd BSS data
 */
void accounting_deinit(struct apd_data *rtapd)
{
	accounting_report_state(rtapd, 0);
}
#endif
