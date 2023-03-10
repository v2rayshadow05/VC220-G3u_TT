#ifndef _GATEDEVICE_H_
#define _GATEDEVICE_H_ 1

#include <upnp.h>




/*  by lcl, 05May11 
---------------------------
IGDdevice
	|
	|-----WanDevice
	|		|
	|		|-----WANConnectionDevice
	|
	|-----WFADevice

*/
/* interface statistics */
typedef enum {
        STATS_TX_BYTES,
        STATS_RX_BYTES,
        STATS_TX_PACKETS,
        STATS_RX_PACKETS,
        STATS_LIMIT
} stats_t;

// IGD Device Globals
UpnpDevice_Handle deviceHandle;
char *gateUDN;
long int startup_time;

// State Variables
char ConnectionType[50];
char PossibleConnectionTypes[50];
char ConnectionStatus[20];
long int StartupTime;
char LastConnectionError[35];
long int AutoDisconnectTime;
long int IdleDisconnectTime;
long int WarnDisconnectDelay;
int RSIPAvailable;
int NATEnabled;
char ExternalIPAddress[20];
int PortMappingNumberOfEntries;
int PortMappingEnabled;


/*add by lcl*/
/*Layer3Forwarding service*/
int SetDefaultConnectionService(struct Upnp_Action_Request *ca_event);
int GetDefaultConnectionService(struct Upnp_Action_Request *ca_event);


// Helper routines
char* GetFirstDocumentItem( IN IXML_Document *doc, const char *item );

// Linked list for portmapping entries
struct portMap *pmlist_Head;
struct portMap *pmlist_Current;

// WanIPConnection Actions 
int EventHandler(Upnp_EventType EventType, void *Event, void *Cookie);
int StateTableInit(char *descDocUrl);
int HandleSubscriptionRequest(struct Upnp_Subscription_Request *sr_event);
int HandleGetVarRequest(struct Upnp_State_Var_Request *gv_event);
int HandleActionRequest(struct Upnp_Action_Request *ca_event);

int GetConnectionTypeInfo(struct Upnp_Action_Request *ca_event);
int GetNATRSIPStatus(struct Upnp_Action_Request *ca_event);
int SetConnectionType(struct Upnp_Action_Request *ca_event);
int RequestConnection(struct Upnp_Action_Request *ca_event);
int GetTotal(struct Upnp_Action_Request *ca_event, stats_t stat);
int GetCommonLinkProperties(struct Upnp_Action_Request *ca_event);
int InvalidAction(struct Upnp_Action_Request *ca_event);
int GetStatusInfo(struct Upnp_Action_Request *ca_event);
int AddPortMapping(struct Upnp_Action_Request *ca_event);
int GetGenericPortMappingEntry(struct Upnp_Action_Request *ca_event);
int GetSpecificPortMappingEntry(struct Upnp_Action_Request *ca_event);
int GetExternalIPAddress(struct Upnp_Action_Request *ca_event);
int DeletePortMapping(struct Upnp_Action_Request *ca_event);


// Definitions for mapping expiration timer thread
#define THREAD_IDLE_TIME 5000
#define JOBS_PER_THREAD 10
#define MIN_THREADS 2 
#define MAX_THREADS 12 

/* Add by chz to avoid port conflict with FTP. 2012-12-21 */
#define FTP_PORT_FILE "/var/vsftp/var/port"
/* end add */

/*added by xianjiantao for 9970 ttnet 3.0,17-05-25*/
#ifdef INCLUDE_TTNET
#define HTTP_REMOTE_PORT_FILE "/var/tmp/httpRemotePort"
#define ACS_PORT_FILE   "/var/tmp/acs_port.txt"
#endif

int ExpirationTimerThreadInit(void);
int ExpirationTimerThreadShutdown(void);
int ScheduleMappingExpiration(struct portMap *mapping, char *DevUDN, char *ServiceID);
int CancelMappingExpiration(int eventId);
void DeleteAllPortMappings(void);
/*added by xianjiantao for vr600 tt v1,17-05-22*/
void enableSpecificRule(char enable, unsigned int extPort, const char *proto);
/*added end xianjiantao*/

#endif //_GATEDEVICE_H
