/* 
   Unix SMB/CIFS implementation.

   Winbind child daemons

   Copyright (C) Andrew Tridgell 2002
   Copyright (C) Volker Lendecke 2004,2005
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * We fork a child per domain to be able to act non-blocking in the main
 * winbind daemon. A domain controller thousands of miles away being being
 * slow replying with a 10.000 user list should not hold up netlogon calls
 * that can be handled locally.
 */

#include "includes.h"
#include "winbindd.h"

#undef DBGC_CLASS
#define DBGC_CLASS DBGC_WINBIND

extern bool override_logfile;
extern struct winbindd_methods cache_methods;

/* Read some data from a client connection */

static void child_read_request(struct winbindd_cli_state *state)
{
	NTSTATUS status;

	/* Read data */

	status = read_data(state->sock, (char *)&state->request,
			   sizeof(state->request));

	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(3, ("child_read_request: read_data failed: %s\n",
			  nt_errstr(status)));
		state->finished = True;
		return;
	}

	if (state->request.extra_len == 0) {
		state->request.extra_data.data = NULL;
		return;
	}

	DEBUG(10, ("Need to read %d extra bytes\n", (int)state->request.extra_len));

	state->request.extra_data.data =
		SMB_MALLOC_ARRAY(char, state->request.extra_len + 1);

	if (state->request.extra_data.data == NULL) {
		DEBUG(0, ("malloc failed\n"));
		state->finished = True;
		return;
	}

	/* Ensure null termination */
	state->request.extra_data.data[state->request.extra_len] = '\0';

	status= read_data(state->sock, state->request.extra_data.data,
			  state->request.extra_len);

	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(0, ("Could not read extra data: %s\n",
			  nt_errstr(status)));
		state->finished = True;
		return;
	}
}

/*
 * Machinery for async requests sent to children. You set up a
 * winbindd_request, select a child to query, and issue a async_request
 * call. When the request is completed, the callback function you specified is
 * called back with the private pointer you gave to async_request.
 */

struct winbindd_async_request {
	struct winbindd_async_request *next, *prev;
	TALLOC_CTX *mem_ctx;
	struct winbindd_child *child;
	struct winbindd_request *request;
	struct winbindd_response *response;
	void (*continuation)(void *private_data, bool success);
	struct timed_event *reply_timeout_event;
	pid_t child_pid; /* pid of the child we're waiting on. Used to detect
			    a restart of the child (child->pid != child_pid). */
	void *private_data;
};

static void async_request_fail(struct winbindd_async_request *state);
static void async_main_request_sent(void *private_data, bool success);
static void async_request_sent(void *private_data, bool success);
static void async_reply_recv(void *private_data, bool success);
static void schedule_async_request(struct winbindd_child *child);

void async_request(TALLOC_CTX *mem_ctx, struct winbindd_child *child,
		   struct winbindd_request *request,
		   struct winbindd_response *response,
		   void (*continuation)(void *private_data, bool success),
		   void *private_data)
{
	struct winbindd_async_request *state;

	SMB_ASSERT(continuation != NULL);

	state = TALLOC_P(mem_ctx, struct winbindd_async_request);

	if (state == NULL) {
		DEBUG(0, ("talloc failed\n"));
		continuation(private_data, False);
		return;
	}

	state->mem_ctx = mem_ctx;
	state->child = child;
	state->reply_timeout_event = NULL;
	state->request = request;
	state->response = response;
	state->continuation = continuation;
	state->private_data = private_data;

	DLIST_ADD_END(child->requests, state, struct winbindd_async_request *);

	schedule_async_request(child);

	return;
}

static void async_main_request_sent(void *private_data, bool success)
{
	struct winbindd_async_request *state =
		talloc_get_type_abort(private_data, struct winbindd_async_request);

	if (!success) {
		DEBUG(5, ("Could not send async request\n"));
		async_request_fail(state);
		return;
	}

	if (state->request->extra_len == 0) {
		async_request_sent(private_data, True);
		return;
	}

	setup_async_write(&state->child->event, state->request->extra_data.data,
			  state->request->extra_len,
			  async_request_sent, state);
}

/****************************************************************
 Handler triggered if the child winbindd doesn't respond within
 a given timeout.
****************************************************************/

static void async_request_timeout_handler(struct event_context *ctx,
					struct timed_event *te,
					const struct timeval *now,
					void *private_data)
{
	struct winbindd_async_request *state =
		talloc_get_type_abort(private_data, struct winbindd_async_request);

	DEBUG(0,("async_request_timeout_handler: child pid %u is not responding. "
		"Closing connection to it.\n",
		state->child_pid ));

	/* Deal with the reply - set to error. */
	async_reply_recv(private_data, False);
}

/**************************************************************
 Common function called on both async send and recv fail.
 Cleans up the child and schedules the next request.
**************************************************************/

static void async_request_fail(struct winbindd_async_request *state)
{
	DLIST_REMOVE(state->child->requests, state);

	TALLOC_FREE(state->reply_timeout_event);

	SMB_ASSERT(state->child_pid != (pid_t)0);

	/* If not already reaped, send kill signal to child. */
	if (state->child->pid == state->child_pid) {
		kill(state->child_pid, SIGTERM);

		/* 
		 * Close the socket to the child.
		 */
		winbind_child_died(state->child_pid);
	}

	state->response->length = sizeof(struct winbindd_response);
	state->response->result = WINBINDD_ERROR;
	state->continuation(state->private_data, False);
}

static void async_request_sent(void *private_data_data, bool success)
{
	struct winbindd_async_request *state =
		talloc_get_type_abort(private_data_data, struct winbindd_async_request);

	if (!success) {
		DEBUG(5, ("Could not send async request to child pid %u\n",
			(unsigned int)state->child_pid ));
		async_request_fail(state);
		return;
	}

	/* Request successfully sent to the child, setup the wait for reply */

	setup_async_read(&state->child->event,
			 &state->response->result,
			 sizeof(state->response->result),
			 async_reply_recv, state);

	/* 
	 * Set up a timeout of 300 seconds for the response.
	 * If we don't get it close the child socket and
	 * report failure.
	 */

	state->reply_timeout_event = event_add_timed(winbind_event_context(),
							NULL,
							timeval_current_ofs(300,0),
							"async_request_timeout",
							async_request_timeout_handler,
							state);
	if (!state->reply_timeout_event) {
		smb_panic("async_request_sent: failed to add timeout handler.\n");
	}
}

static void async_reply_recv(void *private_data, bool success)
{
	struct winbindd_async_request *state =
		talloc_get_type_abort(private_data, struct winbindd_async_request);
	struct winbindd_child *child = state->child;

	TALLOC_FREE(state->reply_timeout_event);

	state->response->length = sizeof(struct winbindd_response);

	if (!success) {
		DEBUG(5, ("Could not receive async reply from child pid %u\n",
			(unsigned int)state->child_pid ));

		cache_cleanup_response(state->child_pid);
		async_request_fail(state);
		return;
	}

	SMB_ASSERT(cache_retrieve_response(state->child_pid,
					   state->response));

	cache_cleanup_response(state->child_pid);
	
	DLIST_REMOVE(child->requests, state);

	schedule_async_request(child);

	state->continuation(state->private_data, True);
}

static bool fork_domain_child(struct winbindd_child *child);

static void schedule_async_request(struct winbindd_child *child)
{
	struct winbindd_async_request *request = child->requests;

	if (request == NULL) {
		return;
	}

	if (child->event.flags != 0) {
		return;		/* Busy */
	}

	if ((child->pid == 0) && (!fork_domain_child(child))) {
		/* Cancel all outstanding requests */

		while (request != NULL) {
			/* request might be free'd in the continuation */
			struct winbindd_async_request *next = request->next;
			request->continuation(request->private_data, False);
			request = next;
		}
		return;
	}

	/* Now we know who we're sending to - remember the pid. */
	request->child_pid = child->pid;

	setup_async_write(&child->event, request->request,
			  sizeof(*request->request),
			  async_main_request_sent, request);

	return;
}

struct domain_request_state {
	TALLOC_CTX *mem_ctx;
	struct winbindd_domain *domain;
	struct winbindd_request *request;
	struct winbindd_response *response;
	void (*continuation)(void *private_data_data, bool success);
	void *private_data_data;
};

static void domain_init_recv(void *private_data_data, bool success);

void async_domain_request(TALLOC_CTX *mem_ctx,
			  struct winbindd_domain *domain,
			  struct winbindd_request *request,
			  struct winbindd_response *response,
			  void (*continuation)(void *private_data_data, bool success),
			  void *private_data_data)
{
	struct domain_request_state *state;

	if (domain->initialized) {
		async_request(mem_ctx, &domain->child, request, response,
			      continuation, private_data_data);
		return;
	}

	state = TALLOC_P(mem_ctx, struct domain_request_state);
	if (state == NULL) {
		DEBUG(0, ("talloc failed\n"));
		continuation(private_data_data, False);
		return;
	}

	state->mem_ctx = mem_ctx;
	state->domain = domain;
	state->request = request;
	state->response = response;
	state->continuation = continuation;
	state->private_data_data = private_data_data;

	init_child_connection(domain, domain_init_recv, state);
}

static void domain_init_recv(void *private_data_data, bool success)
{
	struct domain_request_state *state =
		talloc_get_type_abort(private_data_data, struct domain_request_state);

	if (!success) {
		DEBUG(5, ("Domain init returned an error\n"));
		state->continuation(state->private_data_data, False);
		return;
	}

	async_request(state->mem_ctx, &state->domain->child,
		      state->request, state->response,
		      state->continuation, state->private_data_data);
}

static void recvfrom_child(void *private_data_data, bool success)
{
	struct winbindd_cli_state *state =
		talloc_get_type_abort(private_data_data, struct winbindd_cli_state);
	enum winbindd_result result = state->response.result;

	/* This is an optimization: The child has written directly to the
	 * response buffer. The request itself is still in pending state,
	 * state that in the result code. */

	state->response.result = WINBINDD_PENDING;

	if ((!success) || (result != WINBINDD_OK)) {
		request_error(state);
		return;
	}

	request_ok(state);
}

void sendto_child(struct winbindd_cli_state *state,
		  struct winbindd_child *child)
{
	async_request(state->mem_ctx, child, &state->request,
		      &state->response, recvfrom_child, state);
}

void sendto_domain(struct winbindd_cli_state *state,
		   struct winbindd_domain *domain)
{
	async_domain_request(state->mem_ctx, domain,
			     &state->request, &state->response,
			     recvfrom_child, state);
}

static void child_process_request(struct winbindd_child *child,
				  struct winbindd_cli_state *state)
{
	struct winbindd_domain *domain = child->domain;
	const struct winbindd_child_dispatch_table *table = child->table;

	/* Free response data - we may be interrupted and receive another
	   command before being able to send this data off. */

	state->response.result = WINBINDD_ERROR;
	state->response.length = sizeof(struct winbindd_response);

	/* as all requests in the child are sync, we can use talloc_tos() */
	state->mem_ctx = talloc_tos();

	/* Process command */

	for (; table->name; table++) {
		if (state->request.cmd == table->struct_cmd) {
			DEBUG(10,("child_process_request: request fn %s\n",
				  table->name));
			state->response.result = table->struct_fn(domain, state);
			return;
		}
	}

	DEBUG(1 ,("child_process_request: unknown request fn number %d\n",
		  (int)state->request.cmd));
	state->response.result = WINBINDD_ERROR;
}

void setup_child(struct winbindd_child *child,
		 const struct winbindd_child_dispatch_table *table,
		 const char *logprefix,
		 const char *logname)
{
	if (logprefix && logname) {
		if (asprintf(&child->logfilename, "%s/%s-%s",
			     get_dyn_LOGFILEBASE(), logprefix, logname) < 0) {
			smb_panic("Internal error: asprintf failed");
		}
	} else {
		smb_panic("Internal error: logprefix == NULL && "
			  "logname == NULL");
	}

	child->domain = NULL;
	child->table = table;
}

struct winbindd_child *children = NULL;

void winbind_child_died(pid_t pid)
{
	struct winbindd_child *child;

	for (child = children; child != NULL; child = child->next) {
		if (child->pid == pid) {
			break;
		}
	}

	if (child == NULL) {
		DEBUG(5, ("Already reaped child %u died\n", (unsigned int)pid));
		return;
	}

	/* This will be re-added in fork_domain_child() */

	DLIST_REMOVE(children, child);
	
	remove_fd_event(&child->event);
	close(child->event.fd);
	child->event.fd = 0;
	child->event.flags = 0;
	child->pid = 0;

	schedule_async_request(child);
}

/* Ensure any negative cache entries with the netbios or realm names are removed. */

void winbindd_flush_negative_conn_cache(struct winbindd_domain *domain)
{
	flush_negative_conn_cache_for_domain(domain->name);
	if (*domain->alt_name) {
		flush_negative_conn_cache_for_domain(domain->alt_name);
	}
}

/* 
 * Parent winbindd process sets its own debug level first and then
 * sends a message to all the winbindd children to adjust their debug
 * level to that of parents.
 */

void winbind_msg_debug(struct messaging_context *msg_ctx,
 			 void *private_data,
			 uint32_t msg_type,
			 struct server_id server_id,
			 DATA_BLOB *data)
{
	struct winbindd_child *child;

	DEBUG(10,("winbind_msg_debug: got debug message.\n"));
	
	debug_message(msg_ctx, private_data, MSG_DEBUG, server_id, data);

	for (child = children; child != NULL; child = child->next) {

		DEBUG(10,("winbind_msg_debug: sending message to pid %u.\n",
			(unsigned int)child->pid));

		messaging_send_buf(msg_ctx, pid_to_procid(child->pid),
			   MSG_DEBUG,
			   data->data,
			   strlen((char *) data->data) + 1);
	}
}

/* Set our domains as offline and forward the offline message to our children. */

void winbind_msg_offline(struct messaging_context *msg_ctx,
			 void *private_data,
			 uint32_t msg_type,
			 struct server_id server_id,
			 DATA_BLOB *data)
{
	struct winbindd_child *child;
	struct winbindd_domain *domain;

	DEBUG(10,("winbind_msg_offline: got offline message.\n"));

	if (!lp_winbind_offline_logon()) {
		DEBUG(10,("winbind_msg_offline: rejecting offline message.\n"));
		return;
	}

	/* Set our global state as offline. */
	if (!set_global_winbindd_state_offline()) {
		DEBUG(10,("winbind_msg_offline: offline request failed.\n"));
		return;
	}

	/* Set all our domains as offline. */
	for (domain = domain_list(); domain; domain = domain->next) {
		if (domain->internal) {
			continue;
		}
		DEBUG(5,("winbind_msg_offline: marking %s offline.\n", domain->name));
		set_domain_offline(domain);
	}

	for (child = children; child != NULL; child = child->next) {
		/* Don't send message to internal childs.  We've already
		   done so above. */
		if (!child->domain || winbindd_internal_child(child)) {
			continue;
		}

		/* Or internal domains (this should not be possible....) */
		if (child->domain->internal) {
			continue;
		}

		/* Each winbindd child should only process requests for one domain - make sure
		   we only set it online / offline for that domain. */

		DEBUG(10,("winbind_msg_offline: sending message to pid %u for domain %s.\n",
			(unsigned int)child->pid, domain->name ));

		messaging_send_buf(msg_ctx, pid_to_procid(child->pid),
				   MSG_WINBIND_OFFLINE,
				   (uint8 *)child->domain->name,
				   strlen(child->domain->name)+1);
	}
}

/* Set our domains as online and forward the online message to our children. */

void winbind_msg_online(struct messaging_context *msg_ctx,
			void *private_data,
			uint32_t msg_type,
			struct server_id server_id,
			DATA_BLOB *data)
{
	struct winbindd_child *child;
	struct winbindd_domain *domain;

	DEBUG(10,("winbind_msg_online: got online message.\n"));

	if (!lp_winbind_offline_logon()) {
		DEBUG(10,("winbind_msg_online: rejecting online message.\n"));
		return;
	}

	/* Set our global state as online. */
	set_global_winbindd_state_online();

	smb_nscd_flush_user_cache();
	smb_nscd_flush_group_cache();

	/* Set all our domains as online. */
	for (domain = domain_list(); domain; domain = domain->next) {
		if (domain->internal) {
			continue;
		}
		DEBUG(5,("winbind_msg_online: requesting %s to go online.\n", domain->name));

		winbindd_flush_negative_conn_cache(domain);
		set_domain_online_request(domain);

		/* Send an online message to the idmap child when our
		   primary domain comes back online */

		if ( domain->primary ) {
			struct winbindd_child *idmap = idmap_child();
			
			if ( idmap->pid != 0 ) {
				messaging_send_buf(msg_ctx,
						   pid_to_procid(idmap->pid), 
						   MSG_WINBIND_ONLINE,
						   (uint8 *)domain->name,
						   strlen(domain->name)+1);
			}
			
		}
	}

	for (child = children; child != NULL; child = child->next) {
		/* Don't send message to internal childs. */
		if (!child->domain || winbindd_internal_child(child)) {
			continue;
		}

		/* Or internal domains (this should not be possible....) */
		if (child->domain->internal) {
			continue;
		}

		/* Each winbindd child should only process requests for one domain - make sure
		   we only set it online / offline for that domain. */

		DEBUG(10,("winbind_msg_online: sending message to pid %u for domain %s.\n",
			(unsigned int)child->pid, child->domain->name ));

		messaging_send_buf(msg_ctx, pid_to_procid(child->pid),
				   MSG_WINBIND_ONLINE,
				   (uint8 *)child->domain->name,
				   strlen(child->domain->name)+1);
	}
}

/* Forward the online/offline messages to our children. */
void winbind_msg_onlinestatus(struct messaging_context *msg_ctx,
			      void *private_data,
			      uint32_t msg_type,
			      struct server_id server_id,
			      DATA_BLOB *data)
{
	struct winbindd_child *child;

	DEBUG(10,("winbind_msg_onlinestatus: got onlinestatus message.\n"));

	for (child = children; child != NULL; child = child->next) {
		if (child->domain && child->domain->primary) {
			DEBUG(10,("winbind_msg_onlinestatus: "
				  "sending message to pid %u of primary domain.\n",
				  (unsigned int)child->pid));
			messaging_send_buf(msg_ctx, pid_to_procid(child->pid), 
					   MSG_WINBIND_ONLINESTATUS,
					   (uint8 *)data->data,
					   data->length);
			break;
		}
	}
}

void winbind_msg_dump_event_list(struct messaging_context *msg_ctx,
				 void *private_data,
				 uint32_t msg_type,
				 struct server_id server_id,
				 DATA_BLOB *data)
{
	struct winbindd_child *child;

	DEBUG(10,("winbind_msg_dump_event_list received\n"));

	dump_event_list(winbind_event_context());

	for (child = children; child != NULL; child = child->next) {

		DEBUG(10,("winbind_msg_dump_event_list: sending message to pid %u\n",
			(unsigned int)child->pid));

		messaging_send_buf(msg_ctx, pid_to_procid(child->pid),
				   MSG_DUMP_EVENT_LIST,
				   NULL, 0);
	}

}

void winbind_msg_dump_domain_list(struct messaging_context *msg_ctx,
				  void *private_data,
				  uint32_t msg_type,
				  struct server_id server_id,
				  DATA_BLOB *data)
{
	TALLOC_CTX *mem_ctx;
	const char *message = NULL;
	struct server_id *sender = NULL;
	const char *domain = NULL;
	char *s = NULL;
	NTSTATUS status;
	struct winbindd_domain *dom = NULL;

	DEBUG(5,("winbind_msg_dump_domain_list received.\n"));

	if (!data || !data->data) {
		return;
	}

	if (data->length < sizeof(struct server_id)) {
		return;
	}

	mem_ctx = talloc_init("winbind_msg_dump_domain_list");
	if (!mem_ctx) {
		return;
	}

	sender = (struct server_id *)data->data;
	if (data->length > sizeof(struct server_id)) {
		domain = (const char *)data->data+sizeof(struct server_id);
	}

	if (domain) {

		DEBUG(5,("winbind_msg_dump_domain_list for domain: %s\n",
			domain));

		message = NDR_PRINT_STRUCT_STRING(mem_ctx, winbindd_domain,
						  find_domain_from_name_noinit(domain));
		if (!message) {
			talloc_destroy(mem_ctx);
			return;
		}

		messaging_send_buf(msg_ctx, *sender,
				   MSG_WINBIND_DUMP_DOMAIN_LIST,
				   (uint8_t *)message, strlen(message) + 1);

		talloc_destroy(mem_ctx);

		return;
	}

	DEBUG(5,("winbind_msg_dump_domain_list all domains\n"));

	for (dom = domain_list(); dom; dom=dom->next) {
		message = NDR_PRINT_STRUCT_STRING(mem_ctx, winbindd_domain, dom);
		if (!message) {
			talloc_destroy(mem_ctx);
			return;
		}

		s = talloc_asprintf_append(s, "%s\n", message);
		if (!s) {
			talloc_destroy(mem_ctx);
			return;
		}
	}

	status = messaging_send_buf(msg_ctx, *sender,
				    MSG_WINBIND_DUMP_DOMAIN_LIST,
				    (uint8_t *)s, strlen(s) + 1);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(0,("failed to send message: %s\n",
		nt_errstr(status)));
	}

	talloc_destroy(mem_ctx);
}

static void account_lockout_policy_handler(struct event_context *ctx,
					   struct timed_event *te,
					   const struct timeval *now,
					   void *private_data)
{
	struct winbindd_child *child =
		(struct winbindd_child *)private_data;
	TALLOC_CTX *mem_ctx = NULL;
	struct winbindd_methods *methods;
	struct samr_DomInfo12 lockout_policy;
	NTSTATUS result;

	DEBUG(10,("account_lockout_policy_handler called\n"));

	TALLOC_FREE(child->lockout_policy_event);

	if ( !winbindd_can_contact_domain( child->domain ) ) {
		DEBUG(10,("account_lockout_policy_handler: Removing myself since I "
			  "do not have an incoming trust to domain %s\n", 
			  child->domain->name));

		return;		
	}

	methods = child->domain->methods;

	mem_ctx = talloc_init("account_lockout_policy_handler ctx");
	if (!mem_ctx) {
		result = NT_STATUS_NO_MEMORY;
	} else {
		result = methods->lockout_policy(child->domain, mem_ctx, &lockout_policy);
	}
	TALLOC_FREE(mem_ctx);

	if (!NT_STATUS_IS_OK(result)) {
		DEBUG(10,("account_lockout_policy_handler: lockout_policy failed error %s\n",
			 nt_errstr(result)));
	}

	child->lockout_policy_event = event_add_timed(winbind_event_context(), NULL,
						      timeval_current_ofs(3600, 0),
						      "account_lockout_policy_handler",
						      account_lockout_policy_handler,
						      child);
}

/* Deal with a request to go offline. */

static void child_msg_offline(struct messaging_context *msg,
			      void *private_data,
			      uint32_t msg_type,
			      struct server_id server_id,
			      DATA_BLOB *data)
{
	struct winbindd_domain *domain;
	const char *domainname = (const char *)data->data;

	if (data->data == NULL || data->length == 0) {
		return;
	}

	DEBUG(5,("child_msg_offline received for domain %s.\n", domainname));

	if (!lp_winbind_offline_logon()) {
		DEBUG(10,("child_msg_offline: rejecting offline message.\n"));
		return;
	}

	/* Mark the requested domain offline. */

	for (domain = domain_list(); domain; domain = domain->next) {
		if (domain->internal) {
			continue;
		}
		if (strequal(domain->name, domainname)) {
			DEBUG(5,("child_msg_offline: marking %s offline.\n", domain->name));
			set_domain_offline(domain);
		}
	}
}

/* Deal with a request to go online. */

static void child_msg_online(struct messaging_context *msg,
			     void *private_data,
			     uint32_t msg_type,
			     struct server_id server_id,
			     DATA_BLOB *data)
{
	struct winbindd_domain *domain;
	const char *domainname = (const char *)data->data;

	if (data->data == NULL || data->length == 0) {
		return;
	}

	DEBUG(5,("child_msg_online received for domain %s.\n", domainname));

	if (!lp_winbind_offline_logon()) {
		DEBUG(10,("child_msg_online: rejecting online message.\n"));
		return;
	}

	/* Set our global state as online. */
	set_global_winbindd_state_online();

	/* Try and mark everything online - delete any negative cache entries
	   to force a reconnect now. */

	for (domain = domain_list(); domain; domain = domain->next) {
		if (domain->internal) {
			continue;
		}
		if (strequal(domain->name, domainname)) {
			DEBUG(5,("child_msg_online: requesting %s to go online.\n", domain->name));
			winbindd_flush_negative_conn_cache(domain);
			set_domain_online_request(domain);
		}
	}
}

static const char *collect_onlinestatus(TALLOC_CTX *mem_ctx)
{
	struct winbindd_domain *domain;
	char *buf = NULL;

	if ((buf = talloc_asprintf(mem_ctx, "global:%s ", 
				   get_global_winbindd_state_offline() ? 
				   "Offline":"Online")) == NULL) {
		return NULL;
	}

	for (domain = domain_list(); domain; domain = domain->next) {
		if ((buf = talloc_asprintf_append_buffer(buf, "%s:%s ", 
						  domain->name, 
						  domain->online ?
						  "Online":"Offline")) == NULL) {
			return NULL;
		}
	}

	buf = talloc_asprintf_append_buffer(buf, "\n");

	DEBUG(5,("collect_onlinestatus: %s", buf));

	return buf;
}

static void child_msg_onlinestatus(struct messaging_context *msg_ctx,
				   void *private_data,
				   uint32_t msg_type,
				   struct server_id server_id,
				   DATA_BLOB *data)
{
	TALLOC_CTX *mem_ctx;
	const char *message;
	struct server_id *sender;
	
	DEBUG(5,("winbind_msg_onlinestatus received.\n"));

	if (!data->data) {
		return;
	}

	sender = (struct server_id *)data->data;

	mem_ctx = talloc_init("winbind_msg_onlinestatus");
	if (mem_ctx == NULL) {
		return;
	}
	
	message = collect_onlinestatus(mem_ctx);
	if (message == NULL) {
		talloc_destroy(mem_ctx);
		return;
	}

	messaging_send_buf(msg_ctx, *sender, MSG_WINBIND_ONLINESTATUS, 
			   (uint8 *)message, strlen(message) + 1);

	talloc_destroy(mem_ctx);
}

static void child_msg_dump_event_list(struct messaging_context *msg,
				      void *private_data,
				      uint32_t msg_type,
				      struct server_id server_id,
				      DATA_BLOB *data)
{
	DEBUG(5,("child_msg_dump_event_list received\n"));

	dump_event_list(winbind_event_context());
}


static bool fork_domain_child(struct winbindd_child *child)
{
	int fdpair[2];
	struct winbindd_cli_state state;
	struct winbindd_domain *domain;
	struct winbindd_domain *primary_domain = NULL;

	if (child->domain) {
		DEBUG(10, ("fork_domain_child called for domain '%s'\n",
			   child->domain->name));
	} else {
		DEBUG(10, ("fork_domain_child called without domain.\n"));
	}

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, fdpair) != 0) {
		DEBUG(0, ("Could not open child pipe: %s\n",
			  strerror(errno)));
		return False;
	}

	ZERO_STRUCT(state);
	state.pid = sys_getpid();

	child->pid = sys_fork();

	if (child->pid == -1) {
		DEBUG(0, ("Could not fork: %s\n", strerror(errno)));
		return False;
	}

	if (child->pid != 0) {
		/* Parent */
		close(fdpair[0]);
		child->next = child->prev = NULL;
		DLIST_ADD(children, child);
		child->event.fd = fdpair[1];
		child->event.flags = 0;
		child->requests = NULL;
		add_fd_event(&child->event);
		return True;
	}

	/* Child */

	/* Stop zombies in children */
	CatchChild();

	state.sock = fdpair[0];
	close(fdpair[1]);

	if (!reinit_after_fork(winbind_messaging_context(), true)) {
		DEBUG(0,("reinit_after_fork() failed\n"));
		_exit(0);
	}

	close_conns_after_fork();

	if (!override_logfile) {
		lp_set_logfile(child->logfilename);
		reopen_logs();
	}

	/*
	 * For clustering, we need to re-init our ctdbd connection after the
	 * fork
	 */
	if (!NT_STATUS_IS_OK(messaging_reinit(winbind_messaging_context())))
		exit(1);

	/* Don't handle the same messages as our parent. */
	messaging_deregister(winbind_messaging_context(),
			     MSG_SMB_CONF_UPDATED, NULL);
	messaging_deregister(winbind_messaging_context(),
			     MSG_SHUTDOWN, NULL);
	messaging_deregister(winbind_messaging_context(),
			     MSG_WINBIND_OFFLINE, NULL);
	messaging_deregister(winbind_messaging_context(),
			     MSG_WINBIND_ONLINE, NULL);
	messaging_deregister(winbind_messaging_context(),
			     MSG_WINBIND_ONLINESTATUS, NULL);
	messaging_deregister(winbind_messaging_context(),
			     MSG_DUMP_EVENT_LIST, NULL);
	messaging_deregister(winbind_messaging_context(),
			     MSG_WINBIND_DUMP_DOMAIN_LIST, NULL);
	messaging_deregister(winbind_messaging_context(),
			     MSG_DEBUG, NULL);

	/* Handle online/offline messages. */
	messaging_register(winbind_messaging_context(), NULL,
			   MSG_WINBIND_OFFLINE, child_msg_offline);
	messaging_register(winbind_messaging_context(), NULL,
			   MSG_WINBIND_ONLINE, child_msg_online);
	messaging_register(winbind_messaging_context(), NULL,
			   MSG_WINBIND_ONLINESTATUS, child_msg_onlinestatus);
	messaging_register(winbind_messaging_context(), NULL,
			   MSG_DUMP_EVENT_LIST, child_msg_dump_event_list);
	messaging_register(winbind_messaging_context(), NULL,
			   MSG_DEBUG, debug_message);

	if ( child->domain ) {
		child->domain->startup = True;
		child->domain->startup_time = time(NULL);
	}

	/* Ensure we have no pending check_online events other
	   than one for this domain or the primary domain. */

	for (domain = domain_list(); domain; domain = domain->next) {
		if (domain->primary) {
			primary_domain = domain;
		}
		if ((domain != child->domain) && !domain->primary) {
			TALLOC_FREE(domain->check_online_event);
		}
	}

	/* Ensure we're not handling an event inherited from
	   our parent. */

	cancel_named_event(winbind_event_context(),
			   "krb5_ticket_refresh_handler");

	/* We might be in the idmap child...*/
	if (child->domain && !(child->domain->internal) &&
	    lp_winbind_offline_logon()) {

		set_domain_online_request(child->domain);

		if (primary_domain != child->domain) {
			/* We need to talk to the primary
			 * domain as well as the trusted
			 * domain inside a trusted domain
			 * child.
			 * See the code in :
			 * set_dc_type_and_flags_trustinfo()
			 * for details.
			 */
			set_domain_online_request(primary_domain);
		}

		child->lockout_policy_event = event_add_timed(
			winbind_event_context(), NULL, timeval_zero(),
			"account_lockout_policy_handler",
			account_lockout_policy_handler,
			child);
	}

	while (1) {

		int ret;
		fd_set read_fds;
		struct timeval t;
		struct timeval *tp;
		struct timeval now;
		TALLOC_CTX *frame = talloc_stackframe();

		/* check for signals */
		winbind_check_sigterm(false);
		winbind_check_sighup(override_logfile ? NULL :
				child->logfilename);

		run_events(winbind_event_context(), 0, NULL, NULL);

		GetTimeOfDay(&now);

		if (child->domain && child->domain->startup &&
				(now.tv_sec > child->domain->startup_time + 30)) {
			/* No longer in "startup" mode. */
			DEBUG(10,("fork_domain_child: domain %s no longer in 'startup' mode.\n",
				child->domain->name ));
			child->domain->startup = False;
		}

		tp = get_timed_events_timeout(winbind_event_context(), &t);
		if (tp) {
			DEBUG(11,("select will use timeout of %u.%u seconds\n",
				(unsigned int)tp->tv_sec, (unsigned int)tp->tv_usec ));
		}

		/* Handle messages */

		message_dispatch(winbind_messaging_context());

		FD_ZERO(&read_fds);
		FD_SET(state.sock, &read_fds);

		ret = sys_select(state.sock + 1, &read_fds, NULL, NULL, tp);

		if (ret == 0) {
			DEBUG(11,("nothing is ready yet, continue\n"));
			TALLOC_FREE(frame);
			continue;
		}

		if (ret == -1 && errno == EINTR) {
			/* We got a signal - continue. */
			TALLOC_FREE(frame);
			continue;
		}

		if (ret == -1 && errno != EINTR) {
			DEBUG(0,("select error occured\n"));
			TALLOC_FREE(frame);
			perror("select");
			return False;
		}

		/* fetch a request from the main daemon */
		child_read_request(&state);

		if (state.finished) {
			/* we lost contact with our parent */
			exit(0);
		}

		DEBUG(4,("child daemon request %d\n", (int)state.request.cmd));

		ZERO_STRUCT(state.response);
		state.request.null_term = '\0';
		child_process_request(child, &state);

		SAFE_FREE(state.request.extra_data.data);

		cache_store_response(sys_getpid(), &state.response);

		SAFE_FREE(state.response.extra_data.data);

		/* We just send the result code back, the result
		 * structure needs to be fetched via the
		 * winbindd_cache. Hmm. That needs fixing... */

		if (write_data(state.sock,
			       (const char *)&state.response.result,
			       sizeof(state.response.result)) !=
		    sizeof(state.response.result)) {
			DEBUG(0, ("Could not write result\n"));
			exit(1);
		}
		TALLOC_FREE(frame);
	}
}
