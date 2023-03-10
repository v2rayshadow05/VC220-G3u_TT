/* vi: set sw=4 ts=4: */
/*
 * Simple telnet server
 * Bjorn Wesen, Axis Communications AB (bjornw@axis.com)
 *
 * Licensed under GPLv2 or later, see file LICENSE in this source tree.
 *
 * ---------------------------------------------------------------------------
 * (C) Copyright 2000, Axis Communications AB, LUND, SWEDEN
 ****************************************************************************
 *
 * The telnetd manpage says it all:
 *
 * Telnetd operates by allocating a pseudo-terminal device (see pty(4)) for
 * a client, then creating a login process which has the slave side of the
 * pseudo-terminal as stdin, stdout, and stderr. Telnetd manipulates the
 * master side of the pseudo-terminal, implementing the telnet protocol and
 * passing characters between the remote client and the login process.
 *
 * Vladimir Oleynik <dzo@simtreas.ru> 2001
 * Set process group corrections, initial busybox port
 */

//usage:#define telnetd_trivial_usage
//usage:       "[OPTIONS]"
//usage:#define telnetd_full_usage "\n\n"
//usage:       "Handle incoming telnet connections"
//usage:	IF_NOT_FEATURE_TELNETD_STANDALONE(" via inetd") "\n"
//usage:     "\n	-l LOGIN	Exec LOGIN on connect"
//usage:     "\n	-f ISSUE_FILE	Display ISSUE_FILE instead of /etc/issue"
//usage:     "\n	-K		Close connection as soon as login exits"
//usage:     "\n			(normally wait until all programs close slave pty)"
//usage:	IF_FEATURE_TELNETD_STANDALONE(
//usage:     "\n	-p PORT		Port to listen on"
//usage:     "\n	-b ADDR[:PORT]	Address to bind to"
//usage:     "\n	-F		Run in foreground"
//usage:     "\n	-i		Inetd mode"
//usage:	IF_FEATURE_TELNETD_INETD_WAIT(
//usage:     "\n	-w SEC		Inetd 'wait' mode, linger time SEC"
//usage:     "\n	-S		Log to syslog (implied by -i or without -F and -w)"
//usage:	)
//usage:	)
/*_______________________________________________________________________________________
** BosaZhong@10Nov2012, mark.
**	解决client socket异常断开，而telnetd却没有释放资源的情形。基本想法:当telnetd的session
** 资源不够用的时候，尝试判断以前的资源是否存活，如果不存活则直接释放它。按照这个想法以判断
** socket 是否处于连接状态作为存活依据，因此设置socket TCP层的参数以获取连接状态。一个意外
** 的收获是当设置TCP层的参数后，telnetd能在已有的框架上快速处理这种异常断开连接的情形。
*________________________________________________________________________________________
*/

#define DEBUG 0

#include "libbb.h"
#include <syslog.h>


/*<< BosaZhong@10Nov2012, add, for SOL_TCP. */
#include <netinet/tcp.h>
/*>> BosaZhong@10Nov2012, add, for SOL_TCP. */


#if DEBUG
# define TELCMDS
# define TELOPTS
#endif
#include <arpa/telnet.h>


/*telnetd 进程同时最多允许的连接数, Added by Li Chenglong , 2011-Oct-14.*/
#ifndef TELNETD_MAX_CLIENTS
#define TELNETD_MAX_CLIENTS 1
#endif
/* Ended by Li Chenglong , 2011-Oct-14.*/


struct tsession {
	struct tsession *next;
	pid_t shell_pid;
	/*socket  Added by Li Chenglong , 2011-Oct-13.*/
	int sockfd_read;
	int sockfd_write;
	/*master pty fd Added by Li Chenglong , 2011-Oct-13.*/
	int ptyfd;

	/* two circular buffers */
	/*char *buf1, *buf2;*/
/*#define TS_BUF1(ts) ts->buf1*/
/*#define TS_BUF2(ts) TS_BUF2(ts)*/
#define TS_BUF1(ts) ((unsigned char*)(ts + 1))
#define TS_BUF2(ts) (((unsigned char*)(ts + 1)) + BUFSIZE)
	int rdidx1, wridx1, size1;
	int rdidx2, wridx2, size2;
};

/* Two buffers are directly after tsession in malloced memory.
 * Make whole thing fit in 4k */
enum { BUFSIZE = (4 * 1024 - sizeof(struct tsession)) / 2 };


/* Globals */
struct globals {
	struct tsession *sessions;
	const char *loginpath;
	const char *issuefile;
	int maxfd;
	int maxclients;
} FIX_ALIASING;


#define G (*(struct globals*)&bb_common_bufsiz1)
/* telnetd的上层应用是CLI. Added by Li Chenglong , 2011-Oct-13.*/
/* Ended by Li Chenglong , 2011-Oct-13.*/
#define INIT_G() do { \
	G.loginpath = "cli"; \
	G.issuefile = "/etc/issue.net"; \
	G.maxclients = TELNETD_MAX_CLIENTS; \
} while (0)


/*
   Remove all IAC's from buf1 (received IACs are ignored and must be removed
   so as to not be interpreted by the terminal).  Make an uninterrupted
   string of characters fit for the terminal.  Do this by packing
   all characters meant for the terminal sequentially towards the end of buf.

   Return a pointer to the beginning of the characters meant for the terminal
   and make *num_totty the number of characters that should be sent to
   the terminal.

   Note - if an IAC (3 byte quantity) starts before (bf + len) but extends
   past (bf + len) then that IAC will be left unprocessed and *processed
   will be less than len.

   CR-LF ->'s CR mapping is also done here, for convenience.

   NB: may fail to remove iacs which wrap around buffer!
 */
static unsigned char *
remove_iacs(struct tsession *ts, int *pnum_totty)
{
	unsigned char *ptr0 = TS_BUF1(ts) + ts->wridx1;
	unsigned char *ptr = ptr0;
	unsigned char *totty = ptr;
	unsigned char *end = ptr + MIN(BUFSIZE - ts->wridx1, ts->size1);
	int num_totty;

	while (ptr < end) {
		if (*ptr != IAC) {
			char c = *ptr;

			*totty++ = c;
			ptr++;
			/* We map \r\n ==> \r for pragmatic reasons.
			 * Many client implementations send \r\n when
			 * the user hits the CarriageReturn key.
			 */
			if (c == '\r' && ptr < end && (*ptr == '\n' || *ptr == '\0'))
				ptr++;
			continue;
		}

		if ((ptr+1) >= end)
			break;
		if (ptr[1] == NOP) { /* Ignore? (putty keepalive, etc.) */
			ptr += 2;
			continue;
		}
		if (ptr[1] == IAC) { /* Literal IAC? (emacs M-DEL) */
			*totty++ = ptr[1];
			ptr += 2;
			continue;
		}

		/*
		 * TELOPT_NAWS support!
		 */
		if ((ptr+2) >= end) {
			/* Only the beginning of the IAC is in the
			buffer we were asked to process, we can't
			process this char */
			break;
		}
		/*
		 * IAC -> SB -> TELOPT_NAWS -> 4-byte -> IAC -> SE
		 */
		if (ptr[1] == SB && ptr[2] == TELOPT_NAWS) {
			struct winsize ws;
			if ((ptr+8) >= end)
				break;  /* incomplete, can't process */
			ws.ws_col = (ptr[3] << 8) | ptr[4];
			ws.ws_row = (ptr[5] << 8) | ptr[6];
			ioctl(ts->ptyfd, TIOCSWINSZ, (char *)&ws);
			ptr += 9;
			continue;
		}
		/* skip 3-byte IAC non-SB cmd */
#if DEBUG
		fprintf(stderr, "Ignoring IAC %s,%s\n",
				TELCMD(ptr[1]), TELOPT(ptr[2]));
#endif
		ptr += 3;
	}

	num_totty = totty - ptr0;
	*pnum_totty = num_totty;
	/* The difference between ptr and totty is number of iacs
	   we removed from the stream. Adjust buf1 accordingly */
	if ((ptr - totty) == 0) /* 99.999% of cases */
		return ptr0;
	ts->wridx1 += ptr - totty;
	ts->size1 -= ptr - totty;
	/* Move chars meant for the terminal towards the end of the buffer */
	return memmove(ptr - num_totty, ptr0, num_totty);
}

/*
 * Converting single IAC into double on output
 */
static size_t iac_safe_write(int fd, const char *buf, size_t count)
{
	const char *IACptr;
	size_t wr, rc, total;

	total = 0;
	while (1) {
		if (count == 0)
			return total;
		if (*buf == (char)IAC) {
			static const char IACIAC[] ALIGN1 = { IAC, IAC };
			rc = safe_write(fd, IACIAC, 2);
			if (rc != 2)
				break;
			buf++;
			total++;
			count--;
			continue;
		}
		/* count != 0, *buf != IAC */
		IACptr = memchr(buf, IAC, count);
		wr = count;
		if (IACptr)
			wr = IACptr - buf;
		rc = safe_write(fd, buf, wr);
		if (rc != wr)
			break;
		buf += rc;
		total += rc;
		count -= rc;
	}
	/* here: rc - result of last short write */
	if ((ssize_t)rc < 0) { /* error? */
		if (total == 0)
			return rc;
		rc = 0;
	}
	return total + rc;
}

/* Must match getopt32 string */
enum {
	OPT_WATCHCHILD = (1 << 2), /* -K */
	OPT_INETD      = (1 << 3) * ENABLE_FEATURE_TELNETD_STANDALONE, /* -i */
	OPT_PORT       = (1 << 4) * ENABLE_FEATURE_TELNETD_STANDALONE, /* -p PORT */
	OPT_FOREGROUND = (1 << 6) * ENABLE_FEATURE_TELNETD_STANDALONE, /* -F */
	OPT_SYSLOG     = (1 << 7) * ENABLE_FEATURE_TELNETD_INETD_WAIT, /* -S */
	OPT_WAIT       = (1 << 8) * ENABLE_FEATURE_TELNETD_INETD_WAIT, /* -w SEC */
};

/* 
 * fn		bool new_session_is_allowed()
 * brief	我们要求同时只能有maxclients个连接，所以提供一个接口来检查是否允许新到来的连接。	
 * details	我们要求同时只能有maxclients个连接，所以提供一个接口来检查是否允许新到来的连接。	
 *
 * param[in]	N/A
 * param[out]	N/A
 *
 * return	bool	
 * retval	false	不允许新到来的连接。
 *			true	允许新到来的连接
 *
 * note	written by  14Oct11, Li Chenglong	
 */
bool new_session_is_allowed()
{
	/*要用肯定的命名方式*/
	int client_count = 0;
	struct tsession *pTsession = NULL;
	
	pTsession = G.sessions;
	while (pTsession) 
	{
		client_count++;
		pTsession = pTsession->next;
	}

	if (client_count >= G.maxclients)
	{
		return false;
	}

	return true;

}


static struct tsession *
make_new_session(
		IF_FEATURE_TELNETD_STANDALONE(int sock)
		IF_NOT_FEATURE_TELNETD_STANDALONE(void)
) {

#if !ENABLE_FEATURE_TELNETD_STANDALONE
	enum { sock = 0 };
#endif
	const char *login_argv[2];
	struct termios termbuf;
	int fd, pid;
	char tty_name[GETPTY_BUFSIZE];
/* For INCLUDE_TTNET */
	char clientAddr[40] = {0};
	len_and_sockaddr clientSockaddr;
	int ret = 0;
/* End INCLUDE_TTNET*/
	
	/*分配,分配一个控制结构,加两个缓冲buf, 共4k大小空间, Added by Li Chenglong , 2011-Oct-13.*/

	/* Added by Li Chenglong , 2011-Oct-13.
	 * 每个tsession 都关联两个buffer:
	 *	buf1是缓冲 从 socket读入的数据,并在pty设备可输入时将其写到pty的缓冲区.
	 *	buf2是缓冲 从 pty写出到socket的数据，并在socket可写时将其写到socket
	 *
	 */

	/*
		   ^					+----------+
		|  |			G.ts--> + tsession +--->next
		|  |					+----------+
	   +V--|---+     wridx1++   | +------+ |    rdidx1++     +----------+
	   |       | <--------------| | buf1 | |<--------------  |          |
	   |       |     size1--    | +------+ |    size1++      |          |
	   |  pty  |                |          |                 |  socket  |
	   |       |     rdidx2++   | +------+ |    wridx2++     |          |
	   |       |  ------------->| | buf2 | | --------------> |          |
	   +-------+     size2++    | +------+ |    size2--      +----------+
								|__________|	

	Added by Li Chenglong , 2011-Oct-13.
	*/
	struct tsession *ts = xzalloc(sizeof(struct tsession) + BUFSIZE * 2);

	/*ts->buf1 = (char *)(ts + 1);*/
	/*ts->buf2 = ts->buf1 + BUFSIZE;*/

	/* Got a new connection, set up a tty */
	
	/*返回终端主设备文件描述符, tty_name 是从设备名 Added by Li Chenglong , 2011-Oct-13.*/
	fd = xgetpty(tty_name);
	
	/*设置globals的maxfd, Added by Li Chenglong , 2011-Oct-13.*/
	if (fd > G.maxfd)
		G.maxfd = fd;
	
	ts->ptyfd = fd;
	
	/*set fd with NONBLOCK flag, Added by Li Chenglong , 2011-Oct-13.*/
	ndelay_on(fd);
	
	/*when use execve() fd will be close . Added by Li Chenglong , 2011-Oct-13.*/
	close_on_exec_on(fd);

	/* SO_KEEPALIVE by popular demand */
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &const_int_1, sizeof(const_int_1));


	/*<< BosaZhong@10Nov2012, add, client disconnect session UNnormally. 
	** sock == 0 is the master socket. we just set client socket.
	*/
	if (sock != 0)
	{
		int td_tcp_keep_idle = 2; /* 开始探测前的空闭时间 */
		int td_tcp_keep_interval = 2; /* 两次探测之间的间隔 */
		int td_tcp_keep_count = 3; /* 判断断开的探测次数 */
		
		setsockopt(sock, SOL_TCP, TCP_KEEPIDLE, (void *)&td_tcp_keep_idle, sizeof(td_tcp_keep_idle));
		setsockopt(sock, SOL_TCP, TCP_KEEPINTVL, (void *)&td_tcp_keep_interval, sizeof(td_tcp_keep_interval));
		setsockopt(sock, SOL_TCP, TCP_KEEPCNT, (void *)&td_tcp_keep_count, sizeof(td_tcp_keep_count));
	}
	/*>> endof BosaZhong@10Nov2012, add, client disconnect session UNnormally. */
	
#if ENABLE_FEATURE_TELNETD_STANDALONE

	ts->sockfd_read = sock;
	ndelay_on(sock);

	/*don't use this*/
	if (sock == 0) { /* We are called with fd 0 - we are in inetd mode */
		sock++; /* so use fd 1 for output */
		ndelay_on(sock);
	}
	
	/*read sock == write sock*/
	ts->sockfd_write = sock;
	if (sock > G.maxfd)
		G.maxfd = sock;
#else
	/* ts->sockfd_read = 0; - done by xzalloc */
	ts->sockfd_write = 1;
	ndelay_on(0);
	ndelay_on(1);
#endif

	/* Make the telnet client understand we will echo characters so it
	 * should not do it locally. We don't tell the client to run linemode,
	 * because we want to handle line editing and tab completion and other
	 * stuff that requires char-by-char support. */
	{
		static const char iacs_to_send[] ALIGN1 = {
			IAC, DO, TELOPT_ECHO,
			IAC, DO, TELOPT_NAWS,
			/* This requires telnetd.ctrlSQ.patch (incomplete) */
			/*IAC, DO, TELOPT_LFLOW,*/
			IAC, WILL, TELOPT_ECHO,
			IAC, WILL, TELOPT_SGA
		};
		/* This confuses iac_safe_write(), it will try to duplicate
		 * each IAC... */
		//memcpy(TS_BUF2(ts), iacs_to_send, sizeof(iacs_to_send));
		//ts->rdidx2 = sizeof(iacs_to_send);
		//ts->size2 = sizeof(iacs_to_send);
		/* So just stuff it into TCP stream! (no error check...) */
#if ENABLE_FEATURE_TELNETD_STANDALONE
		safe_write(sock, iacs_to_send, sizeof(iacs_to_send));
#else
		safe_write(1, iacs_to_send, sizeof(iacs_to_send));
#endif
		/*ts->rdidx2 = 0; - xzalloc did it */
		/*ts->size2 = 0;*/
	}

	fflush_all();
	pid = vfork(); /* NOMMU-friendly */
	if (pid < 0) {
		free(ts);
		close(fd);
		/* sock will be closed by caller */
		bb_perror_msg("vfork");
		return NULL;
	}
	if (pid > 0) {
		/* Parent */
		/*shell process is pid Added by Li Chenglong , 2011-Oct-13.*/	
		ts->shell_pid = pid;
		return ts;
	}

	/* Child */
	/* Careful - we are after vfork! */

	/* Restore default signal handling ASAP */
	
	/*SIGCHLD :当子进程结束时会将SIGCHLD发送到其父进程,
	 * 我们不使用默认的handler来处理该信号, 对SIGPIPE 写到socket时 对方已经关闭掉tcp
	 * 则发送SIGPIPE信号到调用进程，我们也使用默认的handler来处理,
	 Added by Li Chenglong , 2011-Oct-13.*/
	bb_signals((1 << SIGCHLD) + (1 << SIGPIPE), SIG_DFL);



	pid = getpid();

 	/*don't use this below, Added by Li Chenglong , 2011-Oct-13.*/

	if (ENABLE_FEATURE_UTMP) {
		len_and_sockaddr *lsa = get_peer_lsa(sock);
		char *hostname = NULL;
		if (lsa) {
			hostname = xmalloc_sockaddr2dotted(&lsa->u.sa);
			free(lsa);
		}
		write_new_utmp(pid, LOGIN_PROCESS, tty_name, /*username:*/ "LOGIN", hostname);
		free(hostname);
	}
 	/* Ended by Li Chenglong , 2011-Oct-13.*/


	/* Make new session and process group */
	setsid();

	/* Open the child's side of the tty */
	/* NB: setsid() disconnects from any previous ctty's. Therefore
	 * we must open child's side of the tty AFTER setsid! */
	 
 	/*0,1,2---> tty_name ---> pty slave device  Added by Li Chenglong , 2011-Oct-13.*/
	close(0);

	/*execve 会继承本进程的文件描述符，所以CLI会直接输出到slavefd,
	 * 其它的进程的标准输入输出如何改变?
	 *
	 Added by Li Chenglong , 2011-Oct-13.*/
	xopen(tty_name, O_RDWR); /* becomes our ctty */
	xdup2(0, 1);
	xdup2(0, 2);
 	/* Ended by Li Chenglong , 2011-Oct-13.*/
/* For INCLUDE_TTNET */	
	/* How to let this be compatible with TELNETD_STANDLONE */
	if (sock != 0)
	{
		memset(&clientSockaddr, 0, sizeof(clientSockaddr));
		clientSockaddr.len = sizeof(clientSockaddr.u);
		getpeername(sock, (struct sockaddr *)&(clientSockaddr.u), &(clientSockaddr.len));
		if (clientSockaddr.u.sa.sa_family == AF_INET)
		{
			inet_ntop(AF_INET, &(clientSockaddr.u.sin.sin_addr), clientAddr, sizeof(clientAddr));
		}
#if ENABLE_FEATURE_IPV6
		else if (clientSockaddr.u.sa.sa_family == AF_INET6)
		{
			unsigned char ipv4_mapped_addr_prefix[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff};
			if (memcmp(clientSockaddr.u.sin6.sin6_addr.s6_addr, ipv4_mapped_addr_prefix, sizeof(ipv4_mapped_addr_prefix)) == 0)
			{
				struct in_addr ipv4addr = {0};
				memcpy(&(ipv4addr.s_addr), &(clientSockaddr.u.sin6.sin6_addr.s6_addr[12]), sizeof(ipv4addr.s_addr));
				inet_ntop(AF_INET, &(ipv4addr), clientAddr, sizeof(clientAddr));
			}
			else
			{
				inet_ntop(AF_INET6, &(clientSockaddr.u.sin6.sin6_addr), clientAddr, sizeof(clientAddr));
			}
		}
#endif /* ENABLE_FEATURE_IPV6 */
	}
/* End INCLUDE_TTNET */

	
	tcsetpgrp(0, pid); /* switch this tty's process group to us */

	/* The pseudo-terminal allocated to the client is configured to operate
	 * in cooked mode, and with XTABS CRMOD enabled (see tty(4)) */
	tcgetattr(0, &termbuf);
	termbuf.c_lflag |= ECHO; /* if we use readline we dont want this */
	termbuf.c_oflag |= ONLCR | XTABS;
	termbuf.c_iflag |= ICRNL;
	termbuf.c_iflag &= ~IXOFF;
	/*termbuf.c_lflag &= ~ICANON;*/
	tcsetattr_stdin_TCSANOW(&termbuf);

	/* Uses FILE-based I/O to stdout, but does fflush_all(),
	 * so should be safe with vfork.
	 * I fear, though, that some users will have ridiculously big
	 * issue files, and they may block writing to fd 1,
	 * (parent is supposed to read it, but parent waits
	 * for vforked child to exec!) */

	/*print out system info. Added by Li Chenglong , 2011-Oct-13.*/
	print_login_issue(G.issuefile, tty_name);

	/* Exec shell / login / whatever */
	login_argv[0] = G.loginpath;
	login_argv[1] = NULL;
/* For INCLUDE_TTNET */
	if (sock != 0)
	{
		login_argv[1] = clientAddr;
	}
/* End INCLUDE_TTNET */

	/* exec busybox applet (if PREFER_APPLETS=y), if that fails,
	 * exec external program.
	 * NB: sock is either 0 or has CLOEXEC set on it.
	 * fd has CLOEXEC set on it too. These two fds will be closed here.
	 */
	BB_EXECVP(G.loginpath, (char **)login_argv);
	/* _exit is safer with vfork, and we shouldn't send message
	 * to remote clients anyway */
	_exit(EXIT_FAILURE); /*bb_perror_msg_and_die("execv %s", G.loginpath);*/
}

#if ENABLE_FEATURE_TELNETD_STANDALONE




static void
free_session(struct tsession *ts)
{
	struct tsession *t;

 	/*don't use this ,inetd Added by Li Chenglong , 2011-Oct-13.*/
	if (option_mask32 & OPT_INETD)
		exit(EXIT_SUCCESS);
 	/* Ended by Li Chenglong , 2011-Oct-13.*/

	/* Unlink this telnet session from the session list */
	t = G.sessions;
	if (t == ts)
		G.sessions = ts->next;
	else {
		while (t->next != ts)
			t = t->next;
		t->next = ts->next;
	}




 	/* Added by Li Chenglong , 2011-Oct-18.*/
	/*       
		SIGHUP        man 7 signal         
		Hangup detected on controlling terminal or death of controlling process.

		当前台进程的控制终端被关闭掉时，该进程会收到内核发送的SIGHUP消息，而进程默认的处理是
		退出当前进程(exit)，这样就保证了在linux下当telnet连接关闭掉时只需要关闭掉CLI的控制终端的
		描述符就达到了退出CLI进程的目的。
     */
 	/* Ended by Li Chenglong , 2011-Oct-18.*/
#if 0
	/* It was said that "normal" telnetd just closes ptyfd,
	 * doesn't send SIGKILL. When we close ptyfd,
	 * kernel sends SIGHUP to processes having slave side opened. */
	kill(ts->shell_pid, SIGKILL);
	waitpid(ts->shell_pid, NULL, 0);
#endif

	close(ts->ptyfd);
	close(ts->sockfd_read);
	
	/* We do not need to close(ts->sockfd_write), it's the same
	 * as sockfd_read unless we are in inetd mode. But in inetd mode
	 * we do not reach this */
	free(ts);

	/* Scan all sessions and find new maxfd */
	G.maxfd = 0;
	ts = G.sessions;
	while (ts) {
		if (G.maxfd < ts->ptyfd)
			G.maxfd = ts->ptyfd;
		if (G.maxfd < ts->sockfd_read)
			G.maxfd = ts->sockfd_read;
#if 0
		/* Again, sockfd_write == sockfd_read here */
		if (G.maxfd < ts->sockfd_write)
			G.maxfd = ts->sockfd_write;
#endif
		ts = ts->next;
	}
}

#else /* !FEATURE_TELNETD_STANDALONE */

/* Used in main() only, thus "return 0" actually is exit(EXIT_SUCCESS). */
#define free_session(ts) return 0

#endif

static void handle_sigchld(int sig UNUSED_PARAM)
{
	pid_t pid;
	struct tsession *ts;
	int save_errno = errno;

	/* Looping: more than one child may have exited */
	while (1) {
		pid = wait_any_nohang(NULL);
		if (pid <= 0)
			break;
		ts = G.sessions;
		while (ts) {
			if (ts->shell_pid == pid) {
				ts->shell_pid = -1;
// man utmp:
// When init(8) finds that a process has exited, it locates its utmp entry
// by ut_pid, sets ut_type to DEAD_PROCESS, and clears ut_user, ut_host
// and ut_time with null bytes.
// [same applies to other processes which maintain utmp entries, like telnetd]
//
// We do not bother actually clearing fields:
// it might be interesting to know who was logged in and from where
				update_utmp(pid, DEAD_PROCESS, /*tty_name:*/ NULL, /*username:*/ NULL, /*hostname:*/ NULL);
				break;
			}
			ts = ts->next;
		}
	}

	errno = save_errno;
}

int telnetd_main(int argc, char **argv) MAIN_EXTERNALLY_VISIBLE;
int telnetd_main(int argc UNUSED_PARAM, char **argv)
{
	fd_set rdfdset, wrfdset;
	unsigned opt;
	int count;
	struct tsession *ts;
#if ENABLE_FEATURE_TELNETD_STANDALONE
#define IS_INETD (opt & OPT_INETD)
	int master_fd = master_fd; /*socket for all incoming conn, for compiler */
	int sec_linger = sec_linger;
	char *opt_bindaddr = NULL;
	char *opt_portnbr;
#else
	enum {
		IS_INETD = 1,
		master_fd = -1,
	};
#endif
	INIT_G();

	/* -w NUM, and implies -F. -w and -i don't mix */
	IF_FEATURE_TELNETD_INETD_WAIT(opt_complementary = "wF:w+:i--w:w--i";)
	/* Even if !STANDALONE, we accept (and ignore) -i, thus people
	 * don't need to guess whether it's ok to pass -i to us */


 	/*设置telnetd 关联的上层应用,loginpath即是其路径. Added by Li Chenglong , 2011-Oct-13.*/
	opt = getopt32(argv, "f:l:Ki"
			IF_FEATURE_TELNETD_STANDALONE("p:b:F")
			IF_FEATURE_TELNETD_INETD_WAIT("Sw:"),
			&G.issuefile, &G.loginpath
			IF_FEATURE_TELNETD_STANDALONE(, &opt_portnbr, &opt_bindaddr)
			IF_FEATURE_TELNETD_INETD_WAIT(, &sec_linger)
	);
 	/* Ended by Li Chenglong , 2011-Oct-13.*/

	if (!IS_INETD /*&& !re_execed*/) {
		/* inform that we start in standalone mode?
		 * May be useful when people forget to give -i */
		/*bb_error_msg("listening for connections");*/
		if (!(opt & OPT_FOREGROUND)) {
			/* DAEMON_CHDIR_ROOT was giving inconsistent
			 * behavior with/without -F, -i */
			bb_daemonize_or_rexec(0 /*was DAEMON_CHDIR_ROOT*/, argv);
		}
	}
	/* Redirect log to syslog early, if needed */
	if (IS_INETD || (opt & OPT_SYSLOG) || !(opt & OPT_FOREGROUND)) {
		openlog(applet_name, LOG_PID, LOG_DAEMON);
		logmode = LOGMODE_SYSLOG;
	}
#if ENABLE_FEATURE_TELNETD_STANDALONE
	if (IS_INETD) {
		G.sessions = make_new_session(0);
		if (!G.sessions) /* pty opening or vfork problem, exit */
			return 1; /* make_new_session printed error message */
	} else {
	
 	/*here, portnbr Added by Li Chenglong , 2011-Oct-13.*/
		master_fd = 0;
		if (!(opt & OPT_WAIT)) {
			unsigned portnbr = 23;
			if (opt & OPT_PORT)
				portnbr = xatou16(opt_portnbr);

			
			/*create a socket to listen all incoming conn, Added by Li Chenglong , 2011-Oct-13.*/
			master_fd = create_and_bind_stream_or_die(opt_bindaddr, portnbr);
			
		/*master_fd 是监听所有的连接,有新连接时创建新的socket Added by Li Chenglong , 2011-Oct-13.*/
			xlisten(master_fd, 1);
 		/* Ended by Li Chenglong , 2011-Oct-13.*/
		}
		close_on_exec_on(master_fd);
 	/* Ended by Li Chenglong , 2011-Oct-13.*/
	}
#else
	G.sessions = make_new_session();
	if (!G.sessions) /* pty opening or vfork problem, exit */
		return 1; /* make_new_session printed error message */
#endif
	/*we must have this flag to ensure that tsession is closed when cli exit.li chenglong*/
	opt |= OPT_WATCHCHILD;

	/* We don't want to die if just one session is broken */
	signal(SIGPIPE, SIG_IGN);

	if (opt & OPT_WATCHCHILD)
		signal(SIGCHLD, handle_sigchld);
	else /* prevent dead children from becoming zombies */
		signal(SIGCHLD, SIG_IGN);

/*
   This is how the buffers are used. The arrows indicate data flow.

   +-------+     wridx1++     +------+     rdidx1++     +----------+
   |       | <--------------  | buf1 | <--------------  |          |
   |       |     size1--      +------+     size1++      |          |
   |  pty  |                                            |  socket  |
   |       |     rdidx2++     +------+     wridx2++     |          |
   |       |  --------------> | buf2 |  --------------> |          |
   +-------+     size2++      +------+     size2--      +----------+

   size1: "how many bytes are buffered for pty between rdidx1 and wridx1?"
   size2: "how many bytes are buffered for socket between rdidx2 and wridx2?"

   Each session has got two buffers. Buffers are circular. If sizeN == 0,
   buffer is empty. If sizeN == BUFSIZE, buffer is full. In both these cases
   rdidxN == wridxN.
*/
 again:
	FD_ZERO(&rdfdset);
	FD_ZERO(&wrfdset);

	/* Select on the master socket, all telnet sockets and their
	 * ptys if there is room in their session buffers.
	 * NB: scalability problem: we recalculate entire bitmap
	 * before each select. Can be a problem with 500+ connections. */
	ts = G.sessions;
	while (ts) {
		struct tsession *next = ts->next; /* in case we free ts */
		if (ts->shell_pid == -1) {
			/*handle_sigchld*/
			/* Child died and we detected that */
			free_session(ts);
		} else {
			if (ts->size1 > 0)       /* can write to pty */
				FD_SET(ts->ptyfd, &wrfdset);
			if (ts->size1 < BUFSIZE) /* can read from socket */
				FD_SET(ts->sockfd_read, &rdfdset);
			if (ts->size2 > 0)       /* can write to socket */
				FD_SET(ts->sockfd_write, &wrfdset);
			if (ts->size2 < BUFSIZE) /* can read from pty */
				FD_SET(ts->ptyfd, &rdfdset);
		}
		ts = next;
	}

	/*不使用INETD来管理TELNETD, Added by Li Chenglong , 2011-Oct-13.*/
	if (!IS_INETD) {
		FD_SET(master_fd, &rdfdset);
		/* This is needed because free_session() does not
		 * take master_fd into account when it finds new
		 * maxfd among remaining fd's */
		if (master_fd > G.maxfd)
			G.maxfd = master_fd;
	}

	{
		struct timeval *tv_ptr = NULL;
#if ENABLE_FEATURE_TELNETD_INETD_WAIT
		struct timeval tv;
		if ((opt & OPT_WAIT) && !G.sessions) {
			tv.tv_sec = sec_linger;
			tv.tv_usec = 0;
			tv_ptr = &tv;
		}
#endif


		count = select(G.maxfd + 1, &rdfdset, &wrfdset, NULL, tv_ptr);
	}
	if (count == 0) /* "telnetd -w SEC" timed out */
		return 0;
 	/*select 被信号中断了,重新设置参数再调用, Added by Li Chenglong , 2011-Oct-13.*/
	if (count < 0)
		goto again; /* EINTR or ENOMEM */
 	/* Ended by Li Chenglong , 2011-Oct-13.*/

#if ENABLE_FEATURE_TELNETD_STANDALONE
	/* Check for and accept new sessions */

 	/*有新的连接到来,创建新的会话. Added by Li Chenglong , 2011-Oct-13.*/
	if (!IS_INETD && FD_ISSET(master_fd, &rdfdset)) {
		int fd;
		struct tsession *new_ts;

		fd = accept(master_fd, NULL, NULL);
		
		if (fd < 0)
			goto again;
		
		close_on_exec_on(fd);

		if (false == new_session_is_allowed())
		{
			close(fd);
			goto again;
		}
		/* Create a new session and link it into active list */
		new_ts = make_new_session(fd);
		if (new_ts) {
			new_ts->next = G.sessions;
			G.sessions = new_ts;
		} else {
			close(fd);
		}
	}
 	/* Ended by Li Chenglong , 2011-Oct-13.*/
#endif

	/*check DATA flow stuff, Added by Li Chenglong , 2011-Oct-13.*/

	/* Then check for data tunneling */
	ts = G.sessions;
	while (ts) { /* For all sessions... */
		struct tsession *next = ts->next; /* in case we free ts */

	 	/*伪终端主设备可写, Added by Li Chenglong , 2011-Oct-13.*/
		if (/*ts->size1 &&*/ FD_ISSET(ts->ptyfd, &wrfdset)) {
			int num_totty;
			unsigned char *ptr;
			/* Write to pty from buffer 1 */
			ptr = remove_iacs(ts, &num_totty);
			count = safe_write(ts->ptyfd, ptr, num_totty);
			if (count < 0) {

		/* EAGAIN: Non-blocking I/O has been selected using O_NONBLOCK and the write would block.
		   Added by Li Chenglong , 2011-Oct-13.*/

				/*what about EINTR Added by Li Chenglong , 2011-Oct-13.*/
				if (errno == EAGAIN)
					goto skip1;

				
				goto kill_session;
			}
			
 			/*更新size1 和 wridx1, Added by Li Chenglong , 2011-Oct-13.*/
			ts->size1 -= count;
			ts->wridx1 += count;
			if (ts->wridx1 >= BUFSIZE) /* actually == BUFSIZE */
				ts->wridx1 = 0;
 			/* Ended by Li Chenglong , 2011-Oct-13.*/
			
		}
 		/* Ended by Li Chenglong , 2011-Oct-13.*/
 skip1:
	 	/*socket可写, Added by Li Chenglong , 2011-Oct-13.*/
		if (/*ts->size2 &&*/ FD_ISSET(ts->sockfd_write, &wrfdset)) {
			/* Write to socket from buffer 2 */
			count = MIN(BUFSIZE - ts->wridx2, ts->size2);
			count = iac_safe_write(ts->sockfd_write, (void*)(TS_BUF2(ts) + ts->wridx2), count);
			if (count < 0) {
				if (errno == EAGAIN)
					goto skip2;

				
				goto kill_session;
			}
		 	/*更新环形缓冲区的指针, Added by Li Chenglong , 2011-Oct-13.*/
			ts->size2 -= count;
			ts->wridx2 += count;
			if (ts->wridx2 >= BUFSIZE) /* actually == BUFSIZE */
				ts->wridx2 = 0;
 			/* Ended by Li Chenglong , 2011-Oct-13.*/
		}
 		/* Ended by Li Chenglong , 2011-Oct-13.*/
 skip2:
		/* Should not be needed, but... remove_iacs is actually buggy
		 * (it cannot process iacs which wrap around buffer's end)!
		 * Since properly fixing it requires writing bigger code,
		 * we rely instead on this code making it virtually impossible
		 * to have wrapped iac (people don't type at 2k/second).
		 * It also allows for bigger reads in common case. */
		if (ts->size1 == 0) {
			ts->rdidx1 = 0;
			ts->wridx1 = 0;
		}
		if (ts->size2 == 0) {
			ts->rdidx2 = 0;
			ts->wridx2 = 0;
		}

	 	/*socket可读, Added by Li Chenglong , 2011-Oct-13.*/
		if (/*ts->size1 < BUFSIZE &&*/ FD_ISSET(ts->sockfd_read, &rdfdset)) {
			/* Read from socket to buffer 1 */
			count = MIN(BUFSIZE - ts->rdidx1, BUFSIZE - ts->size1);
			count = safe_read(ts->sockfd_read, TS_BUF1(ts) + ts->rdidx1, count);
			if (count <= 0) {
				
			 	/*读socket会阻塞, do sth else Added by Li Chenglong ,2011-Oct-13.*/
				if (count < 0 && errno == EAGAIN)
					goto skip3;
				
			 	/* Ended by Li Chenglong , 2011-Oct-13.*/
				
				goto kill_session;
			}
			
			/* Ignore trailing NUL if it is there */
			if (!TS_BUF1(ts)[ts->rdidx1 + count - 1]) {
				--count;
			}
			
			ts->size1 += count;
			ts->rdidx1 += count;
			if (ts->rdidx1 >= BUFSIZE) /* actually == BUFSIZE */
				ts->rdidx1 = 0;
		}
 		/* Ended by Li Chenglong , 2011-Oct-13.*/
 skip3:
	 	/*伪终端主设备可读, Added by Li Chenglong , 2011-Oct-13.*/
		if (/*ts->size2 < BUFSIZE &&*/ FD_ISSET(ts->ptyfd, &rdfdset)) {
			/* Read from pty to buffer 2 */
			count = MIN(BUFSIZE - ts->rdidx2, BUFSIZE - ts->size2);
			count = safe_read(ts->ptyfd, TS_BUF2(ts) + ts->rdidx2, count);
			if (count <= 0) {
				
				if (count < 0 && errno == EAGAIN)
					goto skip4;

				goto kill_session;
			}
			ts->size2 += count;
			ts->rdidx2 += count;
			if (ts->rdidx2 >= BUFSIZE) /* actually == BUFSIZE */
				ts->rdidx2 = 0;
		}
 		/* Ended by Li Chenglong , 2011-Oct-13.*/
 skip4:
		ts = next;
		continue;
 kill_session:
		if (ts->shell_pid > 0)
			update_utmp(ts->shell_pid, DEAD_PROCESS, /*tty_name:*/ NULL, /*username:*/ NULL, /*hostname:*/ NULL);
		free_session(ts);
		ts = next;
	}

	goto again;
}
