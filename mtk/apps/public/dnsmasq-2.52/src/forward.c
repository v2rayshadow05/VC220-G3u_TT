/* dnsmasq is Copyright (c) 2000-2010 Simon Kelley

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 dated June, 1991, or
   (at your option) version 3 dated 29 June, 2007.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
     
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dnsmasq.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../../../private/lib/libtcapi.h"


static struct frec *lookup_frec(unsigned short id, unsigned int crc);
static struct frec *lookup_frec_by_sender(unsigned short id,
					  union mysockaddr *addr,
					  unsigned int crc);
static unsigned short get_id(unsigned int crc);
static void free_frec(struct frec *f);
static struct randfd *allocate_rfd(int family);

#ifdef CT_COM_DEVICEREG 
int checkDevRegisterFlag(void);
#endif

//#ifdef TCSUPPORT_DNSEACHPVC
#if defined(TCSUPPORT_DNSEACHPVC) || defined(TCSUPPORT_TR69_BIND_PVC)
static int checkTR69Url(int *flag, char *packetUrl);
#if !defined(TCSUPPORT_CT) 
static int choose_dns_server(struct server** start, int iTR69Flag);
#endif
extern void printfList();
extern struct DNS_Param gDNS_Param;
#endif


#ifdef CT_COM_DEVICEREG 
extern int devreg;
#endif

/* Send a UDP packet with its source address set as "source" 
   unless nowild is true, when we just send it with the kernel default */
static void send_from(int fd, int nowild, char *packet, size_t len, 
		      union mysockaddr *to, struct all_addr *source,
		      unsigned int iface)
{
  struct msghdr msg;
  struct iovec iov[1]; 
  union {
    struct cmsghdr align; /* this ensures alignment */
#if defined(HAVE_LINUX_NETWORK)
    char control[CMSG_SPACE(sizeof(struct in_pktinfo))];
#elif defined(IP_SENDSRCADDR)
    char control[CMSG_SPACE(sizeof(struct in_addr))];
#endif
#ifdef HAVE_IPV6
    char control6[CMSG_SPACE(sizeof(struct in6_pktinfo))];
#endif
  } control_u;
  
  iov[0].iov_base = packet;
  iov[0].iov_len = len;

  msg.msg_control = NULL;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;
  msg.msg_name = to;
  msg.msg_namelen = sa_len(to);
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;
  
  if (!nowild)
    {
      struct cmsghdr *cmptr;
      msg.msg_control = &control_u;
      msg.msg_controllen = sizeof(control_u);
      cmptr = CMSG_FIRSTHDR(&msg);

      if (to->sa.sa_family == AF_INET)
	{
#if defined(HAVE_LINUX_NETWORK)
	  struct in_pktinfo *pkt = (struct in_pktinfo *)CMSG_DATA(cmptr);
	  pkt->ipi_ifindex = 0;
	  pkt->ipi_spec_dst = source->addr.addr4;
	  msg.msg_controllen = cmptr->cmsg_len = CMSG_LEN(sizeof(struct in_pktinfo));
	  cmptr->cmsg_level = SOL_IP;
	  cmptr->cmsg_type = IP_PKTINFO;
#elif defined(IP_SENDSRCADDR)
	  struct in_addr *a = (struct in_addr *)CMSG_DATA(cmptr);
	  *a = source->addr.addr4;
	  msg.msg_controllen = cmptr->cmsg_len = CMSG_LEN(sizeof(struct in_addr));
	  cmptr->cmsg_level = IPPROTO_IP;
	  cmptr->cmsg_type = IP_SENDSRCADDR;
#endif
	}
      else
#ifdef HAVE_IPV6
	{
	  struct in6_pktinfo *pkt = (struct in6_pktinfo *)CMSG_DATA(cmptr);
	  pkt->ipi6_ifindex = iface; /* Need iface for IPv6 to handle link-local addrs */
	  pkt->ipi6_addr = source->addr.addr6;
	  msg.msg_controllen = cmptr->cmsg_len = CMSG_LEN(sizeof(struct in6_pktinfo));
	  cmptr->cmsg_type = daemon->v6pktinfo;
	  cmptr->cmsg_level = IPV6_LEVEL;
	}
#else
      iface = 0; /* eliminate warning */
#endif
    }
  
 retry:
  if (sendmsg(fd, &msg, 0) == -1)
    {
      /* certain Linux kernels seem to object to setting the source address in the IPv6 stack
	 by returning EINVAL from sendmsg. In that case, try again without setting the
	 source address, since it will nearly alway be correct anyway.  IPv6 stinks. */
      if (errno == EINVAL && msg.msg_controllen)
	{
	  msg.msg_controllen = 0;
	  goto retry;
	}
      if (retry_send())
	goto retry;
    }
}
          
static unsigned short search_servers(time_t now, struct all_addr **addrpp, 
				     unsigned short qtype, char *qdomain, int *type, char **domain)
			      
{
  /* If the query ends in the domain in one of our servers, set
     domain to point to that name. We find the largest match to allow both
     domain.org and sub.domain.org to exist. */
  
  unsigned int namelen = strlen(qdomain);
  unsigned int matchlen = 0;
  struct server *serv;
  unsigned short flags = 0;
  
  for (serv = daemon->servers; serv; serv=serv->next)
    /* domain matches take priority over NODOTS matches */
    if ((serv->flags & SERV_FOR_NODOTS) && *type != SERV_HAS_DOMAIN && !strchr(qdomain, '.') && namelen != 0)
      {
	unsigned short sflag = serv->addr.sa.sa_family == AF_INET ? F_IPV4 : F_IPV6; 
	*type = SERV_FOR_NODOTS;
	if (serv->flags & SERV_NO_ADDR)
	  flags = F_NXDOMAIN;
	else if (serv->flags & SERV_LITERAL_ADDRESS) 
	  { 
	    if (sflag & qtype)
	      {
		flags = sflag;
		if (serv->addr.sa.sa_family == AF_INET) 
		  *addrpp = (struct all_addr *)&serv->addr.in.sin_addr;
#ifdef HAVE_IPV6
		else
		  *addrpp = (struct all_addr *)&serv->addr.in6.sin6_addr;
#endif 
	      }
	    else if (!flags || (flags & F_NXDOMAIN))
	      flags = F_NOERR;
	  } 
      }
    else if (serv->flags & SERV_HAS_DOMAIN)
      {
	unsigned int domainlen = strlen(serv->domain);
	char *matchstart = qdomain + namelen - domainlen;
	if (namelen >= domainlen &&
	    hostname_isequal(matchstart, serv->domain) &&
	    domainlen >= matchlen &&
	    (domainlen == 0 || namelen == domainlen || *(serv->domain) == '.' || *(matchstart-1) == '.' ))
	  {
	    unsigned short sflag = serv->addr.sa.sa_family == AF_INET ? F_IPV4 : F_IPV6;
	    *type = SERV_HAS_DOMAIN;
	    *domain = serv->domain;
	    matchlen = domainlen;
	    if (serv->flags & SERV_NO_ADDR)
	      flags = F_NXDOMAIN;
	    else if (serv->flags & SERV_LITERAL_ADDRESS)
	      {
		if (sflag & qtype)
		  {
		    flags = sflag;
		    if (serv->addr.sa.sa_family == AF_INET) 
		      *addrpp = (struct all_addr *)&serv->addr.in.sin_addr;
#ifdef HAVE_IPV6
		    else
		      *addrpp = (struct all_addr *)&serv->addr.in6.sin6_addr;
#endif
		  }
		else if (!flags || (flags & F_NXDOMAIN))
		  flags = F_NOERR;
	      }
	  } 
      }

  if (flags == 0 && !(qtype & F_BIGNAME) && 
      (daemon->options & OPT_NODOTS_LOCAL) && !strchr(qdomain, '.') && namelen != 0)
    /* don't forward simple names, make exception for NS queries and empty name. */
    flags = F_NXDOMAIN;
    
  if (flags == F_NXDOMAIN && check_for_local_domain(qdomain, now))
    flags = F_NOERR;

  if (flags)
    {
      int logflags = 0;
      
      if (flags == F_NXDOMAIN || flags == F_NOERR)
	logflags = F_NEG | qtype;
  
      log_query(logflags | flags | F_CONFIG | F_FORWARD, qdomain, *addrpp, NULL);
    }

  return  flags;
}

#ifdef TC_SUPPORT_3G //johnma
/* Add function to get iface address, including ipv4 and ipv6 */
/*return  0:successful ; otherwise :fail*/
int getifaddr_ipv4(char *ifname, struct in_addr *ifaddr) 
{ 
    struct ifreq ifreq; 
    int sockfd; 
    int ret = -1; 
                                                                                 
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) > 0) 
    { 
        strncpy(ifreq.ifr_name, ifname, IFNAMSIZ); 
        if (ioctl(sockfd, SIOCGIFADDR, &ifreq) >= 0) { 
			if (ifaddr)
            	*ifaddr = ((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr; 
			ret = 0;
		}
        close(sockfd); 
    } 
    return ret; 
} 
/*return  0:successful ; otherwise :fail*/
int getifaddr(int family, char *ifname, void *ifaddr,int type) 
{ 
	FILE *fp;
	char pppUptimePath[30] = {0};
	int ret = -1;

	if(!ifname){
		tcdbg_printf("get_if_addr:iface is NULL\n");
		return -1;
	}

	/*check pppuptim for ppp on-demand mode*/
	if(strstr(ifname, "ppp")){
		sprintf(pppUptimePath,"/tmp/pppuptime-%s",ifname);
		if((fp = fopen(pppUptimePath,"r")) == NULL)
		{			
			return -1;
		}
		else{
			fclose(fp);
		}
	}
	/*there is no /tmp/PPP11_demand_auth_info for ppp11*/
	if(strcmp(ifname,"ppp11"))
	{
		/*add check for PPP%d_demand_auth_info for ppp on-demand mode*/
		if(strstr(ifname, "ppp"))
		{
#if (defined(TCSUPPORT_WAN_ETHER) || defined(TCSUPPORT_WAN_PTM)) && defined(TCSUPPORT_MULTISERVICE_ON_WAN)
			// ppp device name pppXX. for ex: ppp82
			sprintf(pppUptimePath,"/tmp/PPP%s_demand_auth_info",&ifname[3]);
#else
			sprintf(pppUptimePath,"/tmp/PPP%c_demand_auth_info",*(ifname + 3));
#endif		
			if((fp = fopen(pppUptimePath,"r")) == NULL)
			{			
				return -1;
			}
			else
			{
				fclose(fp);
			}
		}
	}

    switch (family) 
    { 
    case AF_INET:
         ret = getifaddr_ipv4(ifname, (struct in_addr *)ifaddr); 

	 return ret;

    default:
        return -1; 
    } 
}  
#endif

#if defined(TCSUPPORT_URL_ROUTEPOLICY)
#define ROUTEPOLICY_MAX_ENTRY_NUM 8

/*************************
input parameter:
	char *domainName: domain name string
return value: 
	-1: get fail
	others: bind pvc index
************************/
int getBindIfByURL(char *domainName)
{
	char routeURL[64] = {0};
	int i=0;
	char nodeName[32]={0};
	char urlIP[32] = {0};
	char active[4]={0};
	char type[4]={0};
	int findMatchURL = 0;
	int bindPVC = -1;
	char pvcIndex[4]={0};	
	int ret = 0;
	
//	tcdbg_printf("getBindIfByURL:---domainName = %s\n", domainName);

	if((strlen(domainName) == 0))
	{
		return 1;
	}

	for(i=0; i<ROUTEPOLICY_MAX_ENTRY_NUM ; i++)
	{
		memset(nodeName, 0, sizeof(nodeName));
		sprintf(nodeName, "RoutePolicy_Entry%d", i);

		memset(active, 0, sizeof(active));
		if(tcapi_get(nodeName,"Active",active) != 0)
		{
//			tcdbg_printf("getBindIfByURL:--get RoutePolicy_Entry type error!\n");		
			continue;
		}	
		if(atoi(active) == 0)
		{
			//0: deactive,	1: active
			continue;
		}		
		
		memset(type, 0, sizeof(type));
		if(tcapi_get(nodeName,"Type",type) != 0)
		{
//			tcdbg_printf("getBindIfByURL:--get RoutePolicy_Entry type error!\n");		
			continue;
		}
//		tcdbg_printf("getBindIfByURL:---type=%d\n", atoi(type));
		
		if(atoi(type) != 0)
		{
			//1: IP mode,  0: URL mode
			continue;
		}

		memset(routeURL, 0, sizeof(routeURL));
		tcapi_get(nodeName,"URL",routeURL);
//		tcdbg_printf("getBindIfByURL:---routeURL = %s\n", routeURL);
		if(strlen(routeURL) != 0)
		{
			if(doRegularMatch(domainName, routeURL))
			{
	//			tcdbg_printf("getBindIfByURL:---findMatchURL\n");		
				findMatchURL = 1;
				memset(pvcIndex,0,sizeof(pvcIndex));
				if(tcapi_get(nodeName,"PolicyPVC",pvcIndex) == 0)
				{
					bindPVC = atoi(pvcIndex);	
				}	
				break;
			}
		}
		else
		{
			continue;
		}
	}

	return bindPVC;
}

/*************************
input parameter:
	int pvcIndex: pvc index
	struct server **dnsServer: used to store dnsServer1
	struct server **dnsServer: used to store dnsServer2

return value: 
	return dns server number of this pvcs,(0~2)
************************/
int getDNSServerByIf(int pvcIndex, struct server **urlDnsServer1, struct server **urlDnsServer2 )
{
	struct server *start = NULL;
	int serverNum = 0;
	start = daemon->servers;
	char ifName[8]={0};

	while(NULL != start)
	{
		strncpy(ifName, start->interface, sizeof(ifName)-1);
		if((((ifName[0]=='n')&&(ifName[1]=='a')&&(ifName[2]=='s'))
			||((ifName[0]=='p')&&(ifName[1]=='p')&&(ifName[2]=='p')))
			&&((ifName[3]-'0')== pvcIndex))
		{
			serverNum++;
			if(serverNum == 1)			
				*urlDnsServer1 = start;
			else if(serverNum >= 2)
			{
				*urlDnsServer2 = start;
				break;
			}
		}
		start = start->next;
	}

	return serverNum;
}


#endif

#if !defined(TCSUPPORT_CT_DNSBIND) 
static int forward_query(int udpfd, union mysockaddr *udpaddr,
			 struct all_addr *dst_addr, unsigned int dst_iface,
			 HEADER *header, size_t plen, time_t now, struct frec *forward)
#endif
{
  char *domain = NULL;
  int type = 0;
  struct all_addr *addrp = NULL;
  unsigned int crc = questions_crc(header, plen, daemon->namebuff);
  unsigned short flags = 0;
  unsigned short gotname = extract_request(header, plen, daemon->namebuff, NULL);
  struct server *start = NULL;
#if defined(TCSUPPORT_URL_ROUTEPOLICY)
  int pvcIndex = -1;
  struct server *urlDnsServer1 = NULL;
  struct server *urlDnsServer2 = NULL;
  int urlBindSerNum = 0;
#endif



 //#ifdef TCSUPPORT_DNSEACHPVC
#if defined(TCSUPPORT_DNSEACHPVC) || defined(TCSUPPORT_TR69_BIND_PVC)
//Add for dns for each pvc
  //open file and check if the url come from tr69
#if !defined(TCSUPPORT_CT)
  int iTR69Flag = 0;
#if !defined(TCSUPPORT_TR69_BIND_PVC) 
	checkTR69Url(&iTR69Flag, daemon->namebuff);
#endif
#endif


  #endif



  /* may be no servers available. */
  if (!daemon->servers)
    forward = NULL;
  else if (forward || (forward = lookup_frec_by_sender(ntohs(header->id), udpaddr, crc)))
    {
      /* retry on existing query, send to all available servers  */
      domain = forward->sentto->domain;
      forward->sentto->failed_queries++;
      if (!(daemon->options & OPT_ORDER))
	{
	  forward->forwardall = 1;
	  daemon->last_server = NULL;
	}
      type = forward->sentto->flags & SERV_TYPE;
      if (!(start = forward->sentto->next))
	start = daemon->servers; /* at end of list, recycle */
#if !defined(TCSUPPORT_CT)
	#ifdef TCSUPPORT_DNSEACHPVC
	//choose dns server by application
	if (0 == choose_dns_server(&start, iTR69Flag))
	{
		goto	notFindServer;
		//return 0;
	}
	#endif
#endif
      header->id = htons(forward->new_id);
    }
  else 
    {
#if defined(TCSUPPORT_URL_ROUTEPOLICY)
    pvcIndex = getBindIfByURL(daemon->namebuff);
    urlBindSerNum = getDNSServerByIf(pvcIndex, &urlDnsServer1, &urlDnsServer2);	  	  
#endif
      if (gotname)
	flags = search_servers(now, &addrp, gotname, daemon->namebuff, &type, &domain);
      
      if (!flags && !(forward = get_new_frec(now, NULL)))
	/* table full - server failure. */
	flags = F_NEG;
      
      if (forward)
	{
	  forward->source = *udpaddr;
	  forward->dest = *dst_addr;
	  forward->iface = dst_iface;
	  forward->orig_id = ntohs(header->id);
	  forward->new_id = get_id(crc);
	  forward->fd = udpfd;
	  forward->crc = crc;
	  forward->forwardall = 0;

	  header->id = htons(forward->new_id);
#if defined(TCSUPPORT_URL_ROUTEPOLICY)
		forward->urlDnsServer1 = urlDnsServer1;
		forward->urlDnsServer2 = urlDnsServer2;
		struct server *tmp=NULL;
		tmp = daemon->servers;
		if(tmp != NULL)
		{
			while(tmp->next)
			{
				if((tmp->next == forward->urlDnsServer1)&&(forward->urlDnsServer1))
				{
					forward->pre_server = tmp;
				}
			  	tmp = tmp->next;
			}		
		}
		forward->last_server=tmp;
#endif


	  /* In strict_order mode, or when using domain specific servers
	     always try servers in the order specified in resolv.conf,
	     otherwise, use the one last known to work. */
	  
	  if (type != 0  || (daemon->options & OPT_ORDER)){
#if defined(TCSUPPORT_URL_ROUTEPOLICY)	  	
	if(urlDnsServer1 != NULL)
		start = urlDnsServer1;
	else
#endif
	    start = daemon->servers;
#if !defined(TCSUPPORT_CT)
		#ifdef TCSUPPORT_DNSEACHPVC
		//choose dns server by application
		if(0 == choose_dns_server(&start, iTR69Flag))
		{
			goto	notFindServer;
			//return 0;	
		}	
		#endif	
#endif
	  }
	  else if (!(start = daemon->last_server) ||
		   daemon->forwardcount++ > FORWARD_TEST ||
		   difftime(now, daemon->forwardtime) > FORWARD_TIME)
	    {
#if defined(TCSUPPORT_URL_ROUTEPOLICY)	  	
	      if(urlDnsServer1 != NULL)
	        start = urlDnsServer1;
	      else
#endif			
	      start = daemon->servers;
	      forward->forwardall = 1;
	      daemon->forwardcount = 0;
	      daemon->forwardtime = now;
	    }
	}
    }

  /* check for send errors here (no route to host) 
     if we fail to send to all nameservers, send back an error
     packet straight away (helps modem users when offline)  */
  
  if (!flags && forward)
    {
      struct server *firstsentto = start;
      int forwarded = 0;

      while (1)
	{ 

		
	  /* only send to servers dealing with our domain.
	     domain may be NULL, in which case server->domain 
	     must be NULL also. */
	  
	  if (type == (start->flags & SERV_TYPE) &&
	      (type != SERV_HAS_DOMAIN || hostname_isequal(domain, start->domain)) &&
	      !(start->flags & SERV_LITERAL_ADDRESS))
	    {
	      int fd;

	      /* find server socket to use, may need to get random one. */
	      if (start->sfd){
		fd = start->sfd->fd;
	      	}
	      else 
		{
		    {
		      if (!forward->rfd4 &&
			  !(forward->rfd4 = allocate_rfd(AF_INET)))
			break;
		      daemon->rfd_save = forward->rfd4;
		      fd = forward->rfd4->fd;
		    }
		}
		

	      if (sendto(fd, (char *)header, plen, 0,
			 &start->addr.sa,
			 sa_len(&start->addr)) == -1)
		{
		  if (retry_send())
		    continue;
		}
	      else
		{
		  /* Keep info in case we want to re-send this packet */
		  daemon->srv_save = start;
		  daemon->packet_len = plen;
		  
		  if (!gotname)
		    strcpy(daemon->namebuff, "query");
		  if (start->addr.sa.sa_family == AF_INET)
		    log_query(F_SERVER | F_IPV4 | F_FORWARD, daemon->namebuff, 
			      (struct all_addr *)&start->addr.in.sin_addr, NULL); 
		  start->queries++;
		  forwarded = 1;

#if defined(TCSUPPORT_URL_ROUTEPOLICY)	  	
	if((forward->urlDnsServer1 == NULL)||(forward->urlDnsServer1 == daemon->servers)
		||(forward->urlDnsServer1 == forward->last_server)||(forward->urlDnsServer2 == forward->last_server))
	{
		
	//	tcdbg_printf("dnsmasq:%s, line 842--url dns server is head\n", __FUNCTION__);
		forward->sentto = start;
	}
	else
	{	
		if((start == forward->urlDnsServer1))
		{

			if(forward->urlDnsServer2 == NULL)
			{
				forward->sentto=forward->last_server;
			}
			else
			{
		  		forward->sentto = start;
			}

	//		tcdbg_printf("dnsmasq:%s, line 842--1111\n", __FUNCTION__);
		}
		else if((start == forward->urlDnsServer2)&&(forward->urlDnsServer2 != NULL))
		{
		
			tcdbg_printf("dnsmasq:%s, line 842--2222\n", __FUNCTION__);
			forward->sentto=forward->last_server;
		}
		else if((start->next == forward->urlDnsServer1))
		{
		
	//		tcdbg_printf("dnsmasq:%s, line 842--3333\n", __FUNCTION__);
			if(forward->urlDnsServer2 == NULL)
			{
				forward->sentto=forward->urlDnsServer1;
			}
			else
			{
				forward->sentto=forward->urlDnsServer2;
			}

		}		
		else if((start == forward->last_server))
		{
	//		tcdbg_printf("dnsmasq:%s, line 842--555\n", __FUNCTION__);
			forward->sentto=forward->pre_server;
		}			
		else
		{
			
	//		tcdbg_printf("dnsmasq:%s, line 842--4444\n", __FUNCTION__);
		  forward->sentto = start;
		}
	}
#else
		  forward->sentto = start;
#endif

		  /* Keep info in case we want to re-send this packet */		  
		  if (!forward->forwardall){ 
		    break;
		  }
		  forward->forwardall++;
		}
	    } 
	  if (!(start = start->next))
 	    start = daemon->servers;//have not find a server!
	   if (start == firstsentto)
	  	{
			
			break;
	  	}
	}
      



      if (forwarded){
	return 1;
      	}
      
      /* could not send on, prepare to return */ 
      header->id = htons(forward->orig_id);
      free_frec(forward); /* cancel */
    }	  
  
  /* could not send on, return empty answer or address if known for whole domain */
  if (udpfd != -1)
    {
      plen = setup_reply(header, plen, addrp, flags, daemon->local_ttl);
      send_from(udpfd, daemon->options & OPT_NOWILD, (char *)header, plen, udpaddr, dst_addr, dst_iface);
    }

  return 0;
  
#ifdef TCSUPPORT_DNSEACHPVC
notFindServer:  
    if (!flags && forward)
    {
       free_frec(forward); /* cancel */
   }

	return 0;
#endif
}

static size_t process_reply(HEADER *header, time_t now, 
			    struct server *server, size_t n)
{
  unsigned char *pheader, *sizep;
  int munged = 0, is_sign;
  size_t plen; 

  /* If upstream is advertising a larger UDP packet size
     than we allow, trim it so that we don't get overlarge
     requests for the client. We can't do this for signed packets. */

  if ((pheader = find_pseudoheader(header, n, &plen, &sizep, &is_sign)) && !is_sign)
    {
      unsigned short udpsz;
      unsigned char *psave = sizep;
      
      GETSHORT(udpsz, sizep);
      if (udpsz > daemon->edns_pktsz)
	PUTSHORT(daemon->edns_pktsz, psave);
    }

  if (header->opcode != QUERY || (header->rcode != NOERROR && header->rcode != NXDOMAIN))
    return n;
  
  /* Complain loudly if the upstream server is non-recursive. */
  if (!header->ra && header->rcode == NOERROR && ntohs(header->ancount) == 0 &&
      server && !(server->flags & SERV_WARNED_RECURSIVE))
    {
      prettyprint_addr(&server->addr, daemon->namebuff);
      my_syslog(LOG_WARNING, _("nameserver %s refused to do a recursive query"), daemon->namebuff);
      if (!(daemon->options & OPT_LOG))
	server->flags |= SERV_WARNED_RECURSIVE;
    }  
    
  if (daemon->bogus_addr && header->rcode != NXDOMAIN &&
      check_for_bogus_wildcard(header, n, daemon->namebuff, daemon->bogus_addr, now))
    {
      munged = 1;
      header->rcode = NXDOMAIN;
      header->aa = 0;
    }
  else 
    {
      if (header->rcode == NXDOMAIN && 
	  extract_request(header, n, daemon->namebuff, NULL) &&
	  check_for_local_domain(daemon->namebuff, now))
	{
	  /* if we forwarded a query for a locally known name (because it was for 
	     an unknown type) and the answer is NXDOMAIN, convert that to NODATA,
	     since we know that the domain exists, even if upstream doesn't */
	  munged = 1;
	  header->aa = 1;
	  header->rcode = NOERROR;
	}
      
      if (extract_addresses(header, n, daemon->namebuff, now))
	{
	  my_syslog(LOG_WARNING, _("possible DNS-rebind attack detected"));
	  munged = 1;
	}
    }
  
  /* do this after extract_addresses. Ensure NODATA reply and remove
     nameserver info. */
  
  if (munged)
    {
      header->ancount = htons(0);
      header->nscount = htons(0);
      header->arcount = htons(0);
    }
  
  /* the bogus-nxdomain stuff, doctor and NXDOMAIN->NODATA munging can all elide
     sections of the packet. Find the new length here and put back pseudoheader
     if it was removed. */
  return resize_packet(header, n, pheader, plen);
}

/* sets new last_server */
void reply_query(int fd, int family, time_t now)
{
  /* packet from peer server, extract data for cache, and send to
     original requester */
  HEADER *header;
  union mysockaddr serveraddr;
  struct frec *forward;
  socklen_t addrlen = sizeof(serveraddr);
  ssize_t n = recvfrom(fd, daemon->packet, daemon->edns_pktsz, 0, &serveraddr.sa, &addrlen);
  size_t nn;
  struct server *server;
  
  /* packet buffer overwritten */
  daemon->srv_save = NULL;
  
  /* Determine the address of the server replying  so that we can mark that as good */
  serveraddr.sa.sa_family = family;
#ifdef HAVE_IPV6
  if (serveraddr.sa.sa_family == AF_INET6)
    serveraddr.in6.sin6_flowinfo = 0;
#endif
  
  /* spoof check: answer must come from known server, */
  for (server = daemon->servers; server; server = server->next)
    if (!(server->flags & (SERV_LITERAL_ADDRESS | SERV_NO_ADDR)) &&
	sockaddr_isequal(&server->addr, &serveraddr))
      break;
   
  header = (HEADER *)daemon->packet;
  
  if ((!server
  		) ||
      n < (int)sizeof(HEADER) || !header->qr ||
      !(forward = lookup_frec(ntohs(header->id), questions_crc(header, n, daemon->namebuff))))
    return;
   
  server = forward->sentto;
  
  if ((header->rcode == SERVFAIL || header->rcode == REFUSED) &&
      !(daemon->options & OPT_ORDER) &&
      forward->forwardall == 0)
    /* for broken servers, attempt to send to another one. */
    {
      unsigned char *pheader;
      size_t plen;
      int is_sign;
      
      /* recreate query from reply */
      pheader = find_pseudoheader(header, (size_t)n, &plen, NULL, &is_sign);
      if (!is_sign)
	{
	  header->ancount = htons(0);
	  header->nscount = htons(0);
	  header->arcount = htons(0);
	  if ((nn = resize_packet(header, (size_t)n, pheader, plen)))
	    {
	      header->qr = 0;
	      header->tc = 0;

#if !defined(TCSUPPORT_CT_DNSBIND) 
	      forward_query(-1, NULL, NULL, 0, header, nn, now, forward);
#endif
	      return;
	    }
	}
    }   
  
  if ((forward->sentto->flags & SERV_TYPE) == 0)
    {
      if (header->rcode == SERVFAIL || header->rcode == REFUSED)
	server = NULL;
      else
	{
	  struct server *last_server;
	  
	  /* find good server by address if possible, otherwise assume the last one we sent to */ 
	  for (last_server = daemon->servers; last_server; last_server = last_server->next)
	    if (!(last_server->flags & (SERV_LITERAL_ADDRESS | SERV_HAS_DOMAIN | SERV_FOR_NODOTS | SERV_NO_ADDR)) &&
		sockaddr_isequal(&last_server->addr, &serveraddr))
	      {
		server = last_server;
		break;
	      }
	} 
      if (!(daemon->options & OPT_ALL_SERVERS))
	daemon->last_server = server;
    }
  
  /* If the answer is an error, keep the forward record in place in case
     we get a good reply from another server. Kill it when we've
     had replies from all to avoid filling the forwarding table when
     everything is broken */
#if defined(TCSUPPORT_CZ_GENERAL) || defined(TCSUPPORT_CT)
  if (forward->forwardall == 0 || --forward->forwardall == 1 || 
      (header->rcode != REFUSED && header->rcode != SERVFAIL  && header->rcode != 3))
#else
if (forward->forwardall == 0 || --forward->forwardall == 1 || 
	(header->rcode != REFUSED && header->rcode != SERVFAIL	))
#endif
    {

      if ((nn = process_reply(header, now, server, (size_t)n)))
	{
	  header->id = htons(forward->orig_id);
	  header->ra = 1; /* recursion if available */
	  send_from(forward->fd, daemon->options & OPT_NOWILD, daemon->packet, nn, 
		    &forward->source, &forward->dest, forward->iface);
	}
      free_frec(forward); /* cancel */
    }
}


void receive_query(struct listener *listen, time_t now)
{
  HEADER *header = (HEADER *)daemon->packet;
  union mysockaddr source_addr;
  unsigned short type;
  struct all_addr dst_addr;
  struct in_addr netmask, dst_addr_4;
  size_t m;
  ssize_t n;
  int if_index = 0;
  struct iovec iov[1];
  struct msghdr msg;
  struct cmsghdr *cmptr;
  union {
    struct cmsghdr align; /* this ensures alignment */
#ifdef HAVE_IPV6
#if !defined(TCSUPPORT_CT_DNSBIND) 
    char control6[CMSG_SPACE(sizeof(struct in6_pktinfo))];
#endif
#endif
#if defined(HAVE_LINUX_NETWORK)
#if !defined(TCSUPPORT_CT_DNSBIND) 
    char control[CMSG_SPACE(sizeof(struct in_pktinfo))];
#endif
#elif defined(IP_RECVDSTADDR) && defined(HAVE_SOLARIS_NETWORK)
    char control[CMSG_SPACE(sizeof(struct in_addr)) +
		 CMSG_SPACE(sizeof(unsigned int))];
#elif defined(IP_RECVDSTADDR)
    char control[CMSG_SPACE(sizeof(struct in_addr)) +
		 CMSG_SPACE(sizeof(struct sockaddr_dl))];
#endif
  } control_u;


  /* packet buffer overwritten */
  daemon->srv_save = NULL;
  
  if (listen->family == AF_INET && (daemon->options & OPT_NOWILD))
    {
      dst_addr_4 = listen->iface->addr.in.sin_addr;
      netmask = listen->iface->netmask;
    }
  else
    {
      dst_addr_4.s_addr = 0;
      netmask.s_addr = 0;
    }

  iov[0].iov_base = daemon->packet;
  iov[0].iov_len = daemon->edns_pktsz;
    
  msg.msg_control = control_u.control;
  msg.msg_controllen = sizeof(control_u);
  msg.msg_flags = 0;
  msg.msg_name = &source_addr;
  msg.msg_namelen = sizeof(source_addr);
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;
  
  if ((n = recvmsg(listen->fd, &msg, 0)) == -1)
    return;
  
  if (n < (int)sizeof(HEADER) || 
      (msg.msg_flags & MSG_TRUNC) ||
      header->qr)
    return;
  
  source_addr.sa.sa_family = listen->family;
#ifdef HAVE_IPV6
  if (listen->family == AF_INET6)
    source_addr.in6.sin6_flowinfo = 0;
#endif
  
  if (!(daemon->options & OPT_NOWILD))
    {
      struct ifreq ifr;

      if (msg.msg_controllen < sizeof(struct cmsghdr))
	return;

#if defined(HAVE_LINUX_NETWORK)
      if (listen->family == AF_INET)
	for (cmptr = CMSG_FIRSTHDR(&msg); cmptr; cmptr = CMSG_NXTHDR(&msg, cmptr)) {
	  if (cmptr->cmsg_level == SOL_IP && cmptr->cmsg_type == IP_PKTINFO)
	    {
	      dst_addr_4 = dst_addr.addr.addr4 = ((struct in_pktinfo *)CMSG_DATA(cmptr))->ipi_spec_dst;
	      if_index = ((struct in_pktinfo *)CMSG_DATA(cmptr))->ipi_ifindex;
	    }
	  


    }

#elif defined(IP_RECVDSTADDR) && defined(IP_RECVIF)
      if (listen->family == AF_INET)
	{
	  for (cmptr = CMSG_FIRSTHDR(&msg); cmptr; cmptr = CMSG_NXTHDR(&msg, cmptr))
	    if (cmptr->cmsg_level == IPPROTO_IP && cmptr->cmsg_type == IP_RECVDSTADDR)
	      dst_addr_4 = dst_addr.addr.addr4 = *((struct in_addr *)CMSG_DATA(cmptr));
	    else if (cmptr->cmsg_level == IPPROTO_IP && cmptr->cmsg_type == IP_RECVIF)
#ifdef HAVE_SOLARIS_NETWORK
	      if_index = *((unsigned int *)CMSG_DATA(cmptr));
#else
	      if_index = ((struct sockaddr_dl *)CMSG_DATA(cmptr))->sdl_index;
#endif
	}
#endif
      
#ifdef HAVE_IPV6
      if (listen->family == AF_INET6)
	{
	  for (cmptr = CMSG_FIRSTHDR(&msg); cmptr; cmptr = CMSG_NXTHDR(&msg, cmptr)) {
	    if (cmptr->cmsg_level == IPV6_LEVEL && cmptr->cmsg_type == daemon->v6pktinfo)
	      {
		dst_addr.addr.addr6 = ((struct in6_pktinfo *)CMSG_DATA(cmptr))->ipi6_addr;
		if_index =((struct in6_pktinfo *)CMSG_DATA(cmptr))->ipi6_ifindex;
	      }

	  }
	}
#endif
      
      /* enforce available interface configuration */
      
      if (!indextoname(listen->fd, if_index, ifr.ifr_name) ||
	  !iface_check(listen->family, &dst_addr, ifr.ifr_name, &if_index))
	return;
      
      if (listen->family == AF_INET &&
	  (daemon->options & OPT_LOCALISE) &&
	  ioctl(listen->fd, SIOCGIFNETMASK, &ifr) == -1)
	return;
      
      netmask = ((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr;
    }




  if (extract_request(header, (size_t)n, daemon->namebuff, &type))
    {
      char types[20];

      querystr(types, type);

      if (listen->family == AF_INET) 
	log_query(F_QUERY | F_IPV4 | F_FORWARD, daemon->namebuff, 
		  (struct all_addr *)&source_addr.in.sin_addr, types);
#ifdef HAVE_IPV6
      else
	log_query(F_QUERY | F_IPV6 | F_FORWARD, daemon->namebuff, 
		  (struct all_addr *)&source_addr.in6.sin6_addr, types);
#endif
    }

  

  m = answer_request (header, ((char *) header) + PACKETSZ, (size_t)n, 
		      dst_addr_4, netmask, now);
  if (m >= 1)
    {
      send_from(listen->fd, daemon->options & OPT_NOWILD, (char *)header, 
		m, &source_addr, &dst_addr, if_index);
      daemon->local_answer++;
    }

#if !defined(TCSUPPORT_CT_DNSBIND) 
  else if (forward_query(listen->fd, &source_addr, &dst_addr, if_index,
			 header, (size_t)n, now, NULL))
#endif
    daemon->queries_forwarded++;
  else
    daemon->local_answer++;
}

/* The daemon forks before calling this: it should deal with one connection,
   blocking as neccessary, and then return. Note, need to be a bit careful
   about resources for debug mode, when the fork is suppressed: that's
   done by the caller. */
unsigned char *tcp_request(int confd, time_t now,
			   struct in_addr local_addr, struct in_addr netmask)
{
  int size = 0;
  size_t m;
  unsigned short qtype, gotname;
  unsigned char c1, c2;
  /* Max TCP packet + slop */
  unsigned char *packet = whine_malloc(65536 + MAXDNAME + RRFIXEDSZ);
  HEADER *header;
  struct server *last_server;
  
  while (1)
    {
      if (!packet ||
	  !read_write(confd, &c1, 1, 1) || !read_write(confd, &c2, 1, 1) ||
	  !(size = c1 << 8 | c2) ||
	  !read_write(confd, packet, size, 1))
       	return packet; 
  
      if (size < (int)sizeof(HEADER))
	continue;
      
      header = (HEADER *)packet;
      
      if ((gotname = extract_request(header, (unsigned int)size, daemon->namebuff, &qtype)))
	{
	  union mysockaddr peer_addr;
	  socklen_t peer_len = sizeof(union mysockaddr);
	  
	  if (getpeername(confd, (struct sockaddr *)&peer_addr, &peer_len) != -1)
	    {
	      char types[20];

	      querystr(types, qtype);

	      if (peer_addr.sa.sa_family == AF_INET) 
		log_query(F_QUERY | F_IPV4 | F_FORWARD, daemon->namebuff, 
			  (struct all_addr *)&peer_addr.in.sin_addr, types);
#ifdef HAVE_IPV6
	      else
		log_query(F_QUERY | F_IPV6 | F_FORWARD, daemon->namebuff, 
			  (struct all_addr *)&peer_addr.in6.sin6_addr, types);
#endif
	    }
	}
      
      /* m > 0 if answered from cache */
      m = answer_request(header, ((char *) header) + 65536, (unsigned int)size, 
			 local_addr, netmask, now);

      /* Do this by steam now we're not in the select() loop */
      check_log_writer(NULL); 
      
      if (m == 0)
	{
	  unsigned short flags = 0;
	  struct all_addr *addrp = NULL;
	  int type = 0;
	  char *domain = NULL;
	  
	  if (gotname)
	    flags = search_servers(now, &addrp, gotname, daemon->namebuff, &type, &domain);
	  
	  if (type != 0  || (daemon->options & OPT_ORDER) || !daemon->last_server)
	    last_server = daemon->servers;
	  else
	    last_server = daemon->last_server;
      
	  if (!flags && last_server)
	    {
	      struct server *firstsendto = NULL;
	      unsigned int crc = questions_crc(header, (unsigned int)size, daemon->namebuff);

	      /* Loop round available servers until we succeed in connecting to one.
	         Note that this code subtley ensures that consecutive queries on this connection
	         which can go to the same server, do so. */
	      while (1) 
		{
		  if (!firstsendto)
		    firstsendto = last_server;
		  else
		    {
		      if (!(last_server = last_server->next))
			last_server = daemon->servers;
		      
		      if (last_server == firstsendto)
			break;
		    }
	      
		  /* server for wrong domain */
		  if (type != (last_server->flags & SERV_TYPE) ||
		      (type == SERV_HAS_DOMAIN && !hostname_isequal(domain, last_server->domain)))
		    continue;
		  
		  if ((last_server->tcpfd == -1) &&
		      (last_server->tcpfd = socket(last_server->addr.sa.sa_family, SOCK_STREAM, 0)) != -1 &&
		      (!local_bind(last_server->tcpfd,  &last_server->source_addr, last_server->interface, 1) ||
		       connect(last_server->tcpfd, &last_server->addr.sa, sa_len(&last_server->addr)) == -1))
		    {
		      close(last_server->tcpfd);
		      last_server->tcpfd = -1;
		    }
		  
		  if (last_server->tcpfd == -1)	
		    continue;

		  c1 = size >> 8;
		  c2 = size;
		  
		  if (!read_write(last_server->tcpfd, &c1, 1, 0) ||
		      !read_write(last_server->tcpfd, &c2, 1, 0) ||
		      !read_write(last_server->tcpfd, packet, size, 0) ||
		      !read_write(last_server->tcpfd, &c1, 1, 1) ||
		      !read_write(last_server->tcpfd, &c2, 1, 1))
		    {
		      close(last_server->tcpfd);
		      last_server->tcpfd = -1;
		      continue;
		    } 
		  
		  m = (c1 << 8) | c2;
		  if (!read_write(last_server->tcpfd, packet, m, 1))
		    return packet;
		  
		  if (!gotname)
		    strcpy(daemon->namebuff, "query");
		  if (last_server->addr.sa.sa_family == AF_INET)
		    log_query(F_SERVER | F_IPV4 | F_FORWARD, daemon->namebuff, 
			      (struct all_addr *)&last_server->addr.in.sin_addr, NULL); 
#ifdef HAVE_IPV6
		  else
		    log_query(F_SERVER | F_IPV6 | F_FORWARD, daemon->namebuff, 
			      (struct all_addr *)&last_server->addr.in6.sin6_addr, NULL);
#endif 
		  
		  /* There's no point in updating the cache, since this process will exit and
		     lose the information after a few queries. We make this call for the alias and 
		     bogus-nxdomain side-effects. */
		  /* If the crc of the question section doesn't match the crc we sent, then
		     someone might be attempting to insert bogus values into the cache by 
		     sending replies containing questions and bogus answers. */
		  if (crc == questions_crc(header, (unsigned int)m, daemon->namebuff))
		    m = process_reply(header, now, last_server, (unsigned int)m);
		  
		  break;
		}
	    }
	  
	  /* In case of local answer or no connections made. */
	  if (m == 0)
	    m = setup_reply(header, (unsigned int)size, addrp, flags, daemon->local_ttl);
	}

      check_log_writer(NULL);
      
      c1 = m>>8;
      c2 = m;
      if (!read_write(confd, &c1, 1, 0) ||
	  !read_write(confd, &c2, 1, 0) || 
	  !read_write(confd, packet, m, 0))
	return packet;
    }
}

static struct frec *allocate_frec(time_t now)
{
  struct frec *f;
  
  if ((f = (struct frec *)whine_malloc(sizeof(struct frec))))
    {
      f->next = daemon->frec_list;
      f->time = now;
      f->sentto = NULL;
      f->rfd4 = NULL;
#ifdef HAVE_IPV6
      f->rfd6 = NULL;
#endif
      daemon->frec_list = f;
    }

  return f;
}

static struct randfd *allocate_rfd(int family)
{
  static int finger = 0;
  int i;

  /* limit the number of sockets we have open to avoid starvation of 
     (eg) TFTP. Once we have a reasonable number, randomness should be OK */

  for (i = 0; i < RANDOM_SOCKS; i++)
    if (daemon->randomsocks[i].refcount == 0)
      {
	if ((daemon->randomsocks[i].fd = random_sock(family)) == -1)
	  break;
      
	daemon->randomsocks[i].refcount = 1;
	daemon->randomsocks[i].family = family;
	return &daemon->randomsocks[i];
      }

  /* No free ones or cannot get new socket, grab an existing one */
  for (i = 0; i < RANDOM_SOCKS; i++)
    {
      int j = (i+finger) % RANDOM_SOCKS;
      if (daemon->randomsocks[j].refcount != 0 &&
	  daemon->randomsocks[j].family == family && 
	  daemon->randomsocks[j].refcount != 0xffff)
	{
	  finger = j;
	  daemon->randomsocks[j].refcount++;
	  return &daemon->randomsocks[j];
	}
    }

  return NULL; /* doom */
}


static void free_frec(struct frec *f)
{
  if (f->rfd4 && --(f->rfd4->refcount) == 0)
    close(f->rfd4->fd);
    
  f->rfd4 = NULL;
  f->sentto = NULL;
  
#ifdef HAVE_IPV6
  if (f->rfd6 && --(f->rfd6->refcount) == 0)
    close(f->rfd6->fd);
    
  f->rfd6 = NULL;
#endif
}

/* if wait==NULL return a free or older than TIMEOUT record.
   else return *wait zero if one available, or *wait is delay to
   when the oldest in-use record will expire. Impose an absolute
   limit of 4*TIMEOUT before we wipe things (for random sockets) */
struct frec *get_new_frec(time_t now, int *wait)
{
  struct frec *f, *oldest, *target;
  int count;
  
  if (wait)
    *wait = 0;

  for (f = daemon->frec_list, oldest = NULL, target =  NULL, count = 0; f; f = f->next, count++)
    if (!f->sentto)
      target = f;
    else 
      {
	if (difftime(now, f->time) >= 4*TIMEOUT)
	  {
	    free_frec(f);
	    target = f;
	  }
	
	if (!oldest || difftime(f->time, oldest->time) <= 0)
	  oldest = f;
      }

  if (target)
    {
      target->time = now;
      return target;
    }
  
  /* can't find empty one, use oldest if there is one
     and it's older than timeout */
  if (oldest && ((int)difftime(now, oldest->time)) >= TIMEOUT)
    { 
      /* keep stuff for twice timeout if we can by allocating a new
	 record instead */
      if (difftime(now, oldest->time) < 2*TIMEOUT && 
	  count <= daemon->ftabsize &&
	  (f = allocate_frec(now)))
	return f;

      if (!wait)
	{
	  free_frec(oldest);
	  oldest->time = now;
	}
      return oldest;
    }
  
  /* none available, calculate time 'till oldest record expires */
  if (count > daemon->ftabsize)
    {
      if (oldest && wait)
	*wait = oldest->time + (time_t)TIMEOUT - now;
      return NULL;
    }
  
  if (!(f = allocate_frec(now)) && wait)
    /* wait one second on malloc failure */
    *wait = 1;

  return f; /* OK if malloc fails and this is NULL */
}
 
/* crc is all-ones if not known. */
static struct frec *lookup_frec(unsigned short id, unsigned int crc)
{
  struct frec *f;

  for(f = daemon->frec_list; f; f = f->next)
    if (f->sentto && f->new_id == id && 
	(f->crc == crc || crc == 0xffffffff))
      return f;
      
  return NULL;
}

static struct frec *lookup_frec_by_sender(unsigned short id,
					  union mysockaddr *addr,
					  unsigned int crc)
{
  struct frec *f;
  
  for(f = daemon->frec_list; f; f = f->next)
    if (f->sentto &&
	f->orig_id == id && 
	f->crc == crc &&
	sockaddr_isequal(&f->source, addr))
    {
      return f;
    }
   
  return NULL;
}

/* A server record is going away, remove references to it */
void server_gone(struct server *server)
{
  struct frec *f;
  
  for (f = daemon->frec_list; f; f = f->next)
    if (f->sentto && f->sentto == server)
      free_frec(f);
  
  if (daemon->last_server == server)
    daemon->last_server = NULL;

  if (daemon->srv_save == server)
    daemon->srv_save = NULL;
}

/* return unique random ids. */
static unsigned short get_id(unsigned int crc)
{
  unsigned short ret = 0;
  
  do 
    ret = rand16();
  while (lookup_frec(ret, crc));
  
  return ret;
}

#ifdef CT_COM_DEVICEREG 
int checkDevRegisterFlag(void)
{
  char buf[5] = {0};
  int status, result, no_landingpage = 0;

  tcapi_get("deviceAccount_Entry","registerStatus",buf);
  status = atoi(buf);

  memset(buf, 0, sizeof(buf) );
  tcapi_get("deviceAccount_Entry","registerResult",buf);
  result = atoi(buf);

  memset(buf, 0, sizeof(buf) );
  if ( 0 != tcapi_get("deviceAccount_Entry", "isNOLandingPage", buf) )
  	strcpy(buf, "0");
  no_landingpage = atoi(buf);

  if ( 1 == no_landingpage )
  	return 0;

  if(status == 99 || (status == 0 && result == 2) || (status == 0 && result == 99) 	
 	 || (status == 1) || (status == 2) || (status == 3) || (status == 4) || (status == 0 && result == 0))
        return 1;
  else
        return 0;
}
#endif

//#ifdef TCSUPPORT_DNSEACHPVC
#if defined(TCSUPPORT_DNSEACHPVC) || defined(TCSUPPORT_TR69_BIND_PVC)
#ifdef HAVE_IPV6
/* Utility function for making IPv6 address string. */
const char *
inet6_ntoa (struct in6_addr addr)
{
  static char buf[INET6_ADDRSTRLEN];

  inet_ntop (AF_INET6, &addr, buf, INET6_ADDRSTRLEN);
  return buf;
}
#endif /* HAVE_IPV6 */

static int checkTR69Url(int *flag, char *packetUrl)
{
  FILE *fp = NULL;
  char dnsValue[50] = {0};

  fp = fopen("/etc/tr69Url.conf", "r");
  if(fp == NULL){
	  printf("\n tr69Url.conf not exit or open file failed");
  }else{
	  while(fgets(dnsValue, 2048, fp)){
	  	if(strlen(dnsValue) == 0)
			  printf("\n content is empty.");
	  	else{
			  if(strcmp(dnsValue, packetUrl) == 0){
				  *flag = 1;
				  break;
			  }
	  	 }
	  }
	  fclose(fp);
  }



}


#if !defined(TCSUPPORT_CT)
#if !defined(TCSUPPORT_TR69_BIND_PVC)
static int choose_dns_server(struct server** start, int iTR69Flag)
{	
	struct DNS_Info* currNode = NULL;
	int iMatch = 0; //indicate whether dns server match conf file
	//struct server* firstsendto = *start;
	int recycle = 0;
	//printfList(); //for debug
	
	if(1 == iTR69Flag) //request come from tr69
	{
		while(NULL != *start)
	    	{
	    		currNode = gDNS_Param.pHead->next;
		    	while(NULL != currNode)
		    	{
		    		if(0 == strcmp(currNode->strDNS, inet_ntoa((*start)->addr.in.sin_addr)) && 
							0 == strcmp(currNode->strInterface, (*start)->interface) &&
					0 == strcmp(currNode->strApplication, "TR69"))
		    		{
		    			iMatch = 1;
					break;
		    		}
				currNode = currNode->next;
		    	}
			if(1 == iMatch)
				break;
			*start = (*start)->next;
			if(recycle == 0 && *start == NULL){	/* at end of list, recycle */
				*start = daemon->servers;
				recycle = 1;
			}
	    	}
	    }else //request come from other application
	    {
	    	while(NULL != *start)
	    	{
	    		currNode = gDNS_Param.pHead->next;
		    	while(NULL != currNode)
		    	{
		    		if(0 == strcmp(currNode->strDNS, inet_ntoa((*start)->addr.in.sin_addr)) &&
							0 == strcmp(currNode->strInterface, (*start)->interface) && 
							0 != strcmp(currNode->strApplication, "TR69"))
		    		{
		    			iMatch = 1;
					break;
		    		}
				currNode = currNode->next;
		    	}
			if(1 == iMatch)
				break;
			*start = (*start)->next;
			if(recycle == 0 && *start == NULL){	/* at end of list, recycle */
				*start = daemon->servers;
				recycle = 1;
			}
	    	}
	    }
	//start is null means cannot find match dns server
	if(NULL == *start)
	{
		printf("\n Cannot find match dns server.");
		return 0;
	}
	return 1;
}
#endif
#endif




#endif


