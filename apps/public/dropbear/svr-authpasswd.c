/*
 * Dropbear - a SSH2 server
 * 
 * Copyright (c) 2002,2003 Matt Johnston
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

/* Validates a user password */

#include "includes.h"
#include "session.h"
#include "buffer.h"
#include "dbutil.h"
#include "auth.h"
#include "runopts.h"
#include "md5.h"
#include "md5_interface.h"

enum TETHER_LOGIN_MODE
{
	TETHER_LOGIN_MODE_OLD = 1,
	TETHER_LOGIN_MODE_NEW = 2
};

#if DROPBEAR_SVR_PASSWORD_AUTH

/* not constant time when strings are differing lengths. 
 string content isn't leaked, and crypt hashes are predictable length. */
static int constant_time_strcmp(const char* a, const char* b) {
	size_t la = strlen(a);
	size_t lb = strlen(b);

	if (la != lb) {
		return 1;
	}

	return constant_time_memcmp(a, b, la);
}

/* Process a password auth request, sending success or failure messages as
 * appropriate */
void svr_auth_password() {
	
	char * passwdcrypt = NULL; /* the crypt from /etc/passwd or /etc/shadow */
	char * testcrypt = NULL; /* crypt generated from the user's password sent */
	char * password;
	unsigned int passwordlen;

	unsigned int changepw;

	passwdcrypt = ses.authstate.pw_passwd;

#ifdef DEBUG_HACKCRYPT
	/* debugging crypt for non-root testing with shadows */
	passwdcrypt = DEBUG_HACKCRYPT;
#endif

	/* check if client wants to change password */
	changepw = buf_getbool(ses.payload);
	if (changepw) {
		/* not implemented by this server */
		send_msg_userauth_failure(0, 1);
		return;
	}

	password = buf_getstring(ses.payload, &passwordlen);

	/* the first bytes of passwdcrypt are the salt */
	testcrypt = crypt(password, passwdcrypt);
	m_burn(password, passwordlen);
	m_free(password);

	if (testcrypt == NULL) {
		/* crypt() with an invalid salt like "!!" */
		dropbear_log(LOG_WARNING, "User account '%s' is locked",
				ses.authstate.pw_name);
		send_msg_userauth_failure(0, 1);
		return;
	}

	/* check for empty password */
	if (passwdcrypt[0] == '\0') {
		dropbear_log(LOG_WARNING, "User '%s' has blank password, rejected",
				ses.authstate.pw_name);
		send_msg_userauth_failure(0, 1);
		return;
	}

	if (constant_time_strcmp(testcrypt, passwdcrypt) == 0) {
		/* successful authentication */
		dropbear_log(LOG_NOTICE, 
				"Password auth succeeded for '%s' from %s",
				ses.authstate.pw_name,
				svr_ses.addrstring);
		send_msg_userauth_success();
	} else {
		dropbear_log(LOG_WARNING,
				"Bad password attempt for '%s' from %s",
				ses.authstate.pw_name,
				svr_ses.addrstring);
		send_msg_userauth_failure(0, 1);
	}
}

#if DROPBEAR_PWD

//输入字符串格式为"00010A..."，每两个数字一起共32字节
/* !!! Can NOT be static, why??? */
void hexString2Md5Digest(unsigned char *md5Pswd)
{
	unsigned int index = 0;
	unsigned int val = 0;
	unsigned char md5_password[MD5_DIGEST_LEN] = {0};
	
	for (index = 0; index < MD5_DIGEST_LEN; ++index)
	{
		/* sscanf requires md5Pswd be '\0' terminated */
		sscanf(md5Pswd + 2 * index, "%02x", &val);
		*(md5_password + index) = val;
	}
	
	memcpy(md5Pswd, md5_password, MD5_DIGEST_LEN);
}

void svr_chk_pwd(const char* username, const char* pwdfile)
{
	char svr_username[2][MD5_DIGEST_LEN + 1] = {0};
	unsigned char svr_password[2][MD5_DIGEST_LEN*2 + 1] = {0};
	int svr_userbitmap[2] = {1, 1};	// enabled default
	unsigned char *password;
	unsigned int passwordlen;
	FILE   *auth_file = NULL;
	char   *szLine = NULL;
	int len = 0;
	char *szPos = NULL;
	unsigned int changepw;
	int i = 0;
	if (!(auth_file = fopen(pwdfile, "r")))
       	return ;
	
	while (getline(&szLine, &len, auth_file) != -1)
	{
		if(szLine[strlen(szLine) - 1] == '\n')
		{
			szLine[strlen(szLine) - 1] = '\0';
		}
		szPos = strchr(szLine, ':');
		if( szPos != NULL)
		{
			szPos++;
			if(strncmp("adminName", szLine, strlen("adminName")) == 0)
			{
				strncpy(svr_username[0], szPos, MD5_DIGEST_LEN);
				svr_username[0][strlen(szPos)] = '\0';
			}
			else if(strncmp("adminPwd", szLine, strlen("adminPwd")) == 0)
			{
				strncpy(svr_password[0], szPos, MD5_DIGEST_LEN*2);
				svr_password[0][strlen(szPos)] = '\0';
				hexString2Md5Digest(svr_password[0]);
			}
			else if(strncmp("userName", szLine, strlen("userName")) == 0)
			{
				strncpy(svr_username[1], szPos, MD5_DIGEST_LEN);
				svr_username[1][strlen(szPos)] = '\0';
			}
			else if(strncmp("userPwd", szLine, strlen("userPwd")) == 0)
			{
				strncpy(svr_password[1], szPos, MD5_DIGEST_LEN*2);
				svr_password[1][strlen(szPos)] = '\0';
				hexString2Md5Digest(svr_password[1]);
			}
			else if(strncmp("adminEnable", szLine, strlen("adminEnable")) == 0)
			{
				svr_userbitmap[0] = atoi(szPos);
			}
			else if(strncmp("userEnable", szLine, strlen("userEnable")) == 0)
			{
				svr_userbitmap[1] = atoi(szPos);
			}
		}
    }
	fclose(auth_file);

	/* check if client wants to change password */
	changepw = buf_getbool(ses.payload);
	if (changepw) {
		/* not implemented by this server */
		send_msg_userauth_failure(0, 1);
		return;
	}
	
	password = buf_getstring(ses.payload, &passwordlen);		

	for(i=0;i<sizeof(svr_username)/sizeof(svr_username[0]);i++)
	{
		if(svr_userbitmap[i] == 0)
			continue;
		if(strlen(username) == strlen(svr_username[i]) &&
			!strncmp(username, svr_username[i], strlen(username)) &&
			md5_verify_digest(svr_password[i], password, strlen(password))
		   )
		{
			TRACE(("svr_chk_pwd send_msg_userauth_success"))
			dropbear_log(LOG_NOTICE, 
				"Password auth succeeded for '%s' from %s",
				username,
				svr_ses.addrstring);
			ses.authstate.cli_username = strdup(username);
			send_msg_userauth_success();
			break;
		}
	}

	if(i == sizeof(svr_username)/sizeof(svr_username[0]))
	{
		TRACE(("svr_chk_pwd send_msg_userauth_failure"))
		dropbear_log(LOG_WARNING,
				"Bad password attempt for '%s' from %s",
				username,
				svr_ses.addrstring);
		send_msg_userauth_failure(0, 1);
	}

out:
	m_burn(password, passwordlen);
	m_free(password);
}

#endif

#endif
