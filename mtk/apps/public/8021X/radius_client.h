#ifndef RADIUS_CLIENT_H
#define RADIUS_CLIENT_H
#include "time.h"
#include "common.h"


typedef enum {
	RADIUS_AUTH
#ifdef ACCOUNTING_SUPPORT
	,RADIUS_ACCT,
	RADIUS_ACCT_INTERIM
#endif
} RadiusType;

/* RADIUS message retransmit list */
struct radius_msg_list {
	struct radius_msg *msg;
	RadiusType msg_type;
	time_t first_try;
	time_t next_try;
	int attempts;
	int next_wait;

	u8 *shared_secret;
	size_t shared_secret_len;

	u8	ApIdx;	// Multiple SSID interface
	/* TODO: server config with failover to backup server(s) */

	struct radius_msg_list *next;
};


typedef enum {
	RADIUS_RX_PROCESSED,
	RADIUS_RX_QUEUED,
	RADIUS_RX_UNKNOWN,
	/**
	 * RADIUS_RX_INVALID_AUTHENTICATOR - Message has invalid Authenticator
	 */	
	RADIUS_RX_INVALID_AUTHENTICATOR
} RadiusRxResult;

struct radius_rx_handler {
	RadiusRxResult (*handler)(struct apd_data *apd, struct radius_msg *msg, struct radius_msg *req,
				  u8 *shared_secret, size_t shared_secret_len, void *data);
	void *data;
};

struct radius_client_data {

#if MULTIPLE_RADIUS
	int mbss_auth_serv_sock[MAX_MBSSID_NUM]; /* socket for authentication RADIUS messages */
#ifdef ACCOUNTING_SUPPORT
	int mbss_acct_serv_sock[MAX_MBSSID_NUM]; /* socket for accounting RADIUS messages */
#endif
#else
	int auth_serv_sock; /* socket for authentication RADIUS messages */
#ifdef ACCOUNTING_SUPPORT
	int acct_serv_sock; /* socket for accounting RADIUS messages */
#endif
#endif

	struct radius_rx_handler *auth_handlers;
#ifdef ACCOUNTING_SUPPORT
	struct radius_rx_handler *acct_handlers;
	size_t num_acct_handlers;
#endif
	size_t num_auth_handlers;	

	struct radius_msg_list *msgs;
	size_t num_msgs;

	u8 next_radius_identifier;

};

int Radius_client_register(struct apd_data *apd, RadiusType msg_type,
			   RadiusRxResult (*handler) (struct apd_data *apd,  struct radius_msg *msg, struct radius_msg *req,
			    u8 *shared_secret, size_t shared_secret_len, void *data),  void *data);
int Radius_client_send(struct apd_data *rtapd, struct radius_msg *msg, RadiusType msg_type, u8 ApIdx);
u8 Radius_client_get_id(struct apd_data *rtapd);
void Radius_client_flush(struct apd_data *rtapd);
int Radius_client_init(struct apd_data *rtapd);
void Radius_client_deinit(struct apd_data *rtapd);

#endif /* RADIUS_CLIENT_H */
