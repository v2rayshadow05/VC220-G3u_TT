/* 
   Unix SMB/Netbios implementation.
   SMB client library implementation
   Copyright (C) Andrew Tridgell 1998
   Copyright (C) Richard Sharpe 2000, 2002
   Copyright (C) John Terpstra 2000
   Copyright (C) Tom Jansen (Ninja ISD) 2002 
   Copyright (C) Derrell Lipman 2003-2008
   Copyright (C) Jeremy Allison 2007, 2008
   
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

#include "includes.h"
#include "libsmbclient.h"
#include "libsmb_internal.h"


/* 
 * Check a server for being alive and well.
 * returns 0 if the server is in shape. Returns 1 on error 
 * 
 * Also useable outside libsmbclient to enable external cache
 * to do some checks too.
 */
int
SMBC_check_server(SMBCCTX * context,
                  SMBCSRV * server) 
{
        socklen_t size;
        struct sockaddr addr;
        
        size = sizeof(addr);
        return (getpeername(server->cli->fd, &addr, &size) == -1);
}

/* 
 * Remove a server from the cached server list it's unused.
 * On success, 0 is returned. 1 is returned if the server could not be removed.
 * 
 * Also useable outside libsmbclient
 */
int
SMBC_remove_unused_server(SMBCCTX * context,
                          SMBCSRV * srv)
{
	SMBCFILE * file;
        
	/* are we being fooled ? */
	if (!context || !context->internal->initialized || !srv) {
                return 1;
        }
        
	/* Check all open files/directories for a relation with this server */
	for (file = context->internal->files; file; file = file->next) {
		if (file->srv == srv) {
			/* Still used */
			DEBUG(3, ("smbc_remove_usused_server: "
                                  "%p still used by %p.\n",
				  srv, file));
			return 1;
		}
	}
        
	DLIST_REMOVE(context->internal->servers, srv);
        
	cli_shutdown(srv->cli);
	srv->cli = NULL;
        
	DEBUG(3, ("smbc_remove_usused_server: %p removed.\n", srv));
        
	smbc_getFunctionRemoveCachedServer(context)(context, srv);
        
        SAFE_FREE(srv);
	return 0;
}

/****************************************************************
 * Call the auth_fn with fixed size (fstring) buffers.
 ***************************************************************/
void
SMBC_call_auth_fn(TALLOC_CTX *ctx,
                  SMBCCTX *context,
                  const char *server,
                  const char *share,
                  char **pp_workgroup,
                  char **pp_username,
                  char **pp_password)
{
	fstring workgroup;
	fstring username;
	fstring password;
        smbc_get_auth_data_with_context_fn auth_with_context_fn;
        
	strlcpy(workgroup, *pp_workgroup, sizeof(workgroup));
	strlcpy(username, *pp_username, sizeof(username));
	strlcpy(password, *pp_password, sizeof(password));
        
        /* See if there's an authentication with context function provided */
        auth_with_context_fn = smbc_getFunctionAuthDataWithContext(context);
        if (auth_with_context_fn)
        {
            (* auth_with_context_fn)(context,
                                     server, share,
                                     workgroup, sizeof(workgroup),
                                     username, sizeof(username),
                                     password, sizeof(password));
        }
        else
        {
            smbc_getFunctionAuthData(context)(server, share,
                                              workgroup, sizeof(workgroup),
                                              username, sizeof(username),
                                              password, sizeof(password));
        }
        
	TALLOC_FREE(*pp_workgroup);
	TALLOC_FREE(*pp_username);
	TALLOC_FREE(*pp_password);
        
	*pp_workgroup = talloc_strdup(ctx, workgroup);
	*pp_username = talloc_strdup(ctx, username);
	*pp_password = talloc_strdup(ctx, password);
}


void
SMBC_get_auth_data(const char *server, const char *share,
                   char *workgroup_buf, int workgroup_buf_len,
                   char *username_buf, int username_buf_len,
                   char *password_buf, int password_buf_len)
{
        /* Default function just uses provided data.  Nothing to do. */
}



SMBCSRV *
SMBC_find_server(TALLOC_CTX *ctx,
                 SMBCCTX *context,
                 const char *server,
                 const char *share,
                 char **pp_workgroup,
                 char **pp_username,
                 char **pp_password)
{
        SMBCSRV *srv;
        int auth_called = 0;
        
        if (!pp_workgroup || !pp_username || !pp_password) {
                return NULL;
        }
                
check_server_cache:
        
	srv = smbc_getFunctionGetCachedServer(context)(context,
                                                       server, share,
                                                       *pp_workgroup,
                                                       *pp_username);
        
	if (!auth_called && !srv && (!*pp_username || !(*pp_username)[0] ||
                                     !*pp_password || !(*pp_password)[0])) {
		SMBC_call_auth_fn(ctx, context, server, share,
                                  pp_workgroup, pp_username, pp_password);
                
		/*
                 * However, smbc_auth_fn may have picked up info relating to
                 * an existing connection, so try for an existing connection
                 * again ...
                 */
		auth_called = 1;
		goto check_server_cache;
                
	}
        
	if (srv) {
		if (smbc_getFunctionCheckServer(context)(context, srv)) {
			/*
                         * This server is no good anymore
                         * Try to remove it and check for more possible
                         * servers in the cache
                         */
			if (smbc_getFunctionRemoveUnusedServer(context)(context,
                                                                        srv)) { 
                                /*
                                 * We could not remove the server completely,
                                 * remove it from the cache so we will not get
                                 * it again. It will be removed when the last
                                 * file/dir is closed.
                                 */
				smbc_getFunctionRemoveCachedServer(context)(context,
                                                                            srv);
			}
                        
			/*
                         * Maybe there are more cached connections to this
                         * server
                         */
			goto check_server_cache;
		}
                
		return srv;
 	}
        
        return NULL;
}

/*
 * Connect to a server, possibly on an existing connection
 *
 * Here, what we want to do is: If the server and username
 * match an existing connection, reuse that, otherwise, establish a
 * new connection.
 *
 * If we have to create a new connection, call the auth_fn to get the
 * info we need, unless the username and password were passed in.
 */

SMBCSRV *
SMBC_server(TALLOC_CTX *ctx,
            SMBCCTX *context,
            bool connect_if_not_found,
            const char *server,
            const char *share,
            char **pp_workgroup,
            char **pp_username,
            char **pp_password)
{
	SMBCSRV *srv=NULL;
	struct cli_state *c;
	struct nmb_name called, calling;
	const char *server_n = server;
	struct sockaddr_storage ss;
	int tried_reverse = 0;
        int port_try_first;
        int port_try_next;
        const char *username_used;
 	NTSTATUS status;
        
	zero_addr(&ss);
	ZERO_STRUCT(c);
        
	if (server[0] == 0) {
		errno = EPERM;
		return NULL;
	}
        
        /* Look for a cached connection */
        srv = SMBC_find_server(ctx, context, server, share,
                               pp_workgroup, pp_username, pp_password);
        
        /*
         * If we found a connection and we're only allowed one share per
         * server...
         */
        if (srv &&
            *share != '\0' &&
            smbc_getOptionOneSharePerServer(context)) {
                
                /*
                 * ... then if there's no current connection to the share,
                 * connect to it.  SMBC_find_server(), or rather the function
                 * pointed to by context->get_cached_srv_fn which
                 * was called by SMBC_find_server(), will have issued a tree
                 * disconnect if the requested share is not the same as the
                 * one that was already connected.
                 */
                if (srv->cli->cnum == (uint16) -1) {
                        /* Ensure we have accurate auth info */
			SMBC_call_auth_fn(ctx, context, server, share,
                                          pp_workgroup,
                                          pp_username,
                                          pp_password);
                        
			if (!*pp_workgroup || !*pp_username || !*pp_password) {
				errno = ENOMEM;
				cli_shutdown(srv->cli);
				srv->cli = NULL;
				smbc_getFunctionRemoveCachedServer(context)(context,
                                                                            srv);
				return NULL;
			}
                        
			/*
			 * We don't need to renegotiate encryption
			 * here as the encryption context is not per
			 * tid.
			 */
                        
			if (!cli_send_tconX(srv->cli, share, "?????",
                                            *pp_password,
                                            strlen(*pp_password)+1)) {
                                
                                errno = SMBC_errno(context, srv->cli);
                                cli_shutdown(srv->cli);
				srv->cli = NULL;
                                smbc_getFunctionRemoveCachedServer(context)(context,
                                                                            srv);
                                srv = NULL;
                        }
                        
                        /*
                         * Regenerate the dev value since it's based on both
                         * server and share
                         */
                        if (srv) {
                                srv->dev = (dev_t)(str_checksum(server) ^
                                                   str_checksum(share));
                        }
                }
        }
        
        /* If we have a connection... */
        if (srv) {
                
                /* ... then we're done here.  Give 'em what they came for. */
                return srv;
        }
        
        /* If we're not asked to connect when a connection doesn't exist... */
        if (! connect_if_not_found) {
                /* ... then we're done here. */
                return NULL;
        }
        
	if (!*pp_workgroup || !*pp_username || !*pp_password) {
		errno = ENOMEM;
		return NULL;
	}
        
	make_nmb_name(&calling, smbc_getNetbiosName(context), 0x0);
	make_nmb_name(&called , server, 0x20);
        
	DEBUG(4,("SMBC_server: server_n=[%s] server=[%s]\n", server_n, server));
        
	DEBUG(4,(" -> server_n=[%s] server=[%s]\n", server_n, server));
        
again:
        
	zero_addr(&ss);
        
	/* have to open a new connection */
	if ((c = cli_initialise()) == NULL) {
		errno = ENOMEM;
		return NULL;
	}
        
        if (smbc_getOptionUseKerberos(context)) {
		c->use_kerberos = True;
	}

        if (smbc_getOptionFallbackAfterKerberos(context)) {
		c->fallback_after_kerberos = True;
	}
        
	c->timeout = smbc_getTimeout(context);
        
        /*
         * Force use of port 139 for first try if share is $IPC, empty, or
         * null, so browse lists can work
         */
        if (share == NULL || *share == '\0' || strcmp(share, "IPC$") == 0) {
                port_try_first = 139;
                port_try_next = 445;
        } else {
                port_try_first = 445;
                port_try_next = 139;
        }
        
        c->port = port_try_first;
        
	status = cli_connect(c, server_n, &ss);
	if (!NT_STATUS_IS_OK(status)) {
                
                /* First connection attempt failed.  Try alternate port. */
                c->port = port_try_next;
                
                status = cli_connect(c, server_n, &ss);
		if (!NT_STATUS_IS_OK(status)) {
			cli_shutdown(c);
			errno = ETIMEDOUT;
			return NULL;
		}
	}
        
	if (!cli_session_request(c, &calling, &called)) {
		cli_shutdown(c);
		if (strcmp(called.name, "*SMBSERVER")) {
			make_nmb_name(&called , "*SMBSERVER", 0x20);
			goto again;
		} else {  /* Try one more time, but ensure we don't loop */
                        
			/* Only try this if server is an IP address ... */
                        
			if (is_ipaddress(server) && !tried_reverse) {
				fstring remote_name;
				struct sockaddr_storage rem_ss;
                                
				if (!interpret_string_addr(&rem_ss, server,
                                                           NI_NUMERICHOST)) {
					DEBUG(4, ("Could not convert IP address "
                                                  "%s to struct sockaddr_storage\n",
                                                  server));
					errno = ETIMEDOUT;
					return NULL;
				}
                                
				tried_reverse++; /* Yuck */
                                
				if (name_status_find("*", 0, 0,
                                                     &rem_ss, remote_name)) {
					make_nmb_name(&called,
                                                      remote_name,
                                                      0x20);
					goto again;
				}
			}
		}
		errno = ETIMEDOUT;
		return NULL;
	}
        
	DEBUG(4,(" session request ok\n"));
        
	if (!cli_negprot(c)) {
		cli_shutdown(c);
		errno = ETIMEDOUT;
		return NULL;
	}
        
        username_used = *pp_username;
        
	if (!NT_STATUS_IS_OK(cli_session_setup(c, username_used,
					       *pp_password,
                                               strlen(*pp_password),
					       *pp_password,
                                               strlen(*pp_password),
					       *pp_workgroup))) {
                
                /* Failed.  Try an anonymous login, if allowed by flags. */
                username_used = "";
                
                if (smbc_getOptionNoAutoAnonymousLogin(context) ||
                    !NT_STATUS_IS_OK(cli_session_setup(c, username_used,
                                                       *pp_password, 1,
                                                       *pp_password, 0,
                                                       *pp_workgroup))) {
                        
                        cli_shutdown(c);
                        errno = EPERM;
                        return NULL;
                }
	}
        
	DEBUG(4,(" session setup ok\n"));
        
	if (!cli_send_tconX(c, share, "?????",
			    *pp_password, strlen(*pp_password)+1)) {
		errno = SMBC_errno(context, c);
		cli_shutdown(c);
		return NULL;
	}
        
	DEBUG(4,(" tconx ok\n"));
        
	if (context->internal->smb_encryption_level) {
		/* Attempt UNIX smb encryption. */
		if (!NT_STATUS_IS_OK(cli_force_encryption(c,
                                                          username_used,
                                                          *pp_password,
                                                          *pp_workgroup))) {
                        
			/*
			 * context->smb_encryption_level == 1
			 * means don't fail if encryption can't be negotiated,
			 * == 2 means fail if encryption can't be negotiated.
			 */
                        
			DEBUG(4,(" SMB encrypt failed\n"));
                        
			if (context->internal->smb_encryption_level == 2) {
	                        cli_shutdown(c);
				errno = EPERM;
				return NULL;
			}
		}
		DEBUG(4,(" SMB encrypt ok\n"));
	}
        
	/*
	 * Ok, we have got a nice connection
	 * Let's allocate a server structure.
	 */
        
	srv = SMB_MALLOC_P(SMBCSRV);
	if (!srv) {
		errno = ENOMEM;
		goto failed;
	}
        
	ZERO_STRUCTP(srv);
	srv->cli = c;
	srv->dev = (dev_t)(str_checksum(server) ^ str_checksum(share));
        srv->no_pathinfo = False;
        srv->no_pathinfo2 = False;
        srv->no_nt_session = False;
        
	/* now add it to the cache (internal or external)  */
	/* Let the cache function set errno if it wants to */
	errno = 0;
	if (smbc_getFunctionAddCachedServer(context)(context, srv,
                                                     server, share,
                                                     *pp_workgroup,
                                                     *pp_username)) {
		int saved_errno = errno;
		DEBUG(3, (" Failed to add server to cache\n"));
		errno = saved_errno;
		if (errno == 0) {
			errno = ENOMEM;
		}
		goto failed;
	}
        
	DEBUG(2, ("Server connect ok: //%s/%s: %p\n",
		  server, share, srv));
        
	DLIST_ADD(context->internal->servers, srv);
	return srv;
        
failed:
	cli_shutdown(c);
	if (!srv) {
		return NULL;
	}
        
	SAFE_FREE(srv);
	return NULL;
}

/*
 * Connect to a server for getting/setting attributes, possibly on an existing
 * connection.  This works similarly to SMBC_server().
 */
SMBCSRV *
SMBC_attr_server(TALLOC_CTX *ctx,
                 SMBCCTX *context,
                 const char *server,
                 const char *share,
                 char **pp_workgroup,
                 char **pp_username,
                 char **pp_password)
{
        int flags;
        struct sockaddr_storage ss;
	struct cli_state *ipc_cli;
	struct rpc_pipe_client *pipe_hnd;
        NTSTATUS nt_status;
	SMBCSRV *ipc_srv=NULL;
        
        /*
         * See if we've already created this special connection.  Reference
         * our "special" share name '*IPC$', which is an impossible real share
         * name due to the leading asterisk.
         */
        ipc_srv = SMBC_find_server(ctx, context, server, "*IPC$",
                                   pp_workgroup, pp_username, pp_password);
        if (!ipc_srv) {
                
                /* We didn't find a cached connection.  Get the password */
		if (!*pp_password || (*pp_password)[0] == '\0') {
                        /* ... then retrieve it now. */
			SMBC_call_auth_fn(ctx, context, server, share,
                                          pp_workgroup,
                                          pp_username,
                                          pp_password);
			if (!*pp_workgroup || !*pp_username || !*pp_password) {
				errno = ENOMEM;
				return NULL;
			}
                }
                
                flags = 0;
                if (smbc_getOptionUseKerberos(context)) {
                        flags |= CLI_FULL_CONNECTION_USE_KERBEROS;
                }
                
                zero_addr(&ss);
                nt_status = cli_full_connection(&ipc_cli,
						global_myname(), server,
						&ss, 0, "IPC$", "?????",
						*pp_username,
						*pp_workgroup,
						*pp_password,
						flags,
						Undefined, NULL);
                if (! NT_STATUS_IS_OK(nt_status)) {
                        DEBUG(1,("cli_full_connection failed! (%s)\n",
                                 nt_errstr(nt_status)));
                        errno = ENOTSUP;
                        return NULL;
                }
                
		if (context->internal->smb_encryption_level) {
			/* Attempt UNIX smb encryption. */
			if (!NT_STATUS_IS_OK(cli_force_encryption(ipc_cli,
                                                                  *pp_username,
                                                                  *pp_password,
                                                                  *pp_workgroup))) {
                                
				/*
				 * context->smb_encryption_level ==
				 * 1 means don't fail if encryption can't be
				 * negotiated, == 2 means fail if encryption
				 * can't be negotiated.
				 */
                                
				DEBUG(4,(" SMB encrypt failed on IPC$\n"));
                                
				if (context->internal->smb_encryption_level == 2) {
		                        cli_shutdown(ipc_cli);
					errno = EPERM;
					return NULL;
				}
			}
			DEBUG(4,(" SMB encrypt ok on IPC$\n"));
		}
                
                ipc_srv = SMB_MALLOC_P(SMBCSRV);
                if (!ipc_srv) {
                        errno = ENOMEM;
                        cli_shutdown(ipc_cli);
                        return NULL;
                }
                
                ZERO_STRUCTP(ipc_srv);
                ipc_srv->cli = ipc_cli;
                
                pipe_hnd = cli_rpc_pipe_open_noauth(ipc_srv->cli,
                                                    PI_LSARPC,
                                                    &nt_status);
                if (!pipe_hnd) {
                        DEBUG(1, ("cli_nt_session_open fail!\n"));
                        errno = ENOTSUP;
                        cli_shutdown(ipc_srv->cli);
                        free(ipc_srv);
                        return NULL;
                }
                
                /*
                 * Some systems don't support
                 * SEC_RIGHTS_MAXIMUM_ALLOWED, but NT sends 0x2000000
                 * so we might as well do it too.
                 */
                
                nt_status = rpccli_lsa_open_policy(
                        pipe_hnd,
                        talloc_tos(),
                        True,
                        GENERIC_EXECUTE_ACCESS,
                        &ipc_srv->pol);
                
                if (!NT_STATUS_IS_OK(nt_status)) {
                        errno = SMBC_errno(context, ipc_srv->cli);
                        cli_shutdown(ipc_srv->cli);
                        return NULL;
                }
                
                /* now add it to the cache (internal or external) */
                
                errno = 0;      /* let cache function set errno if it likes */
                if (smbc_getFunctionAddCachedServer(context)(context, ipc_srv,
                                                             server,
                                                             "*IPC$",
                                                             *pp_workgroup,
                                                             *pp_username)) {
                        DEBUG(3, (" Failed to add server to cache\n"));
                        if (errno == 0) {
                                errno = ENOMEM;
                        }
                        cli_shutdown(ipc_srv->cli);
                        free(ipc_srv);
                        return NULL;
                }
                
                DLIST_ADD(context->internal->servers, ipc_srv);
        }
        
        return ipc_srv;
}
