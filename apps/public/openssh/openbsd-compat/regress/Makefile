# $Id: Makefile.in,v 1.4 2006/08/19 09:12:14 dtucker Exp $

sysconfdir=${prefix}/etc
piddir=/var/run
srcdir=.
top_srcdir=../..


CC=mips-unknown-linux-uclibc-gcc
LD=mips-unknown-linux-uclibc-ld
CFLAGS=-g -O2 -Wall -Wpointer-arith -Wuninitialized -Wsign-compare -Wformat-security -Wno-pointer-sign -Wno-unused-result -fno-strict-aliasing -D_FORTIFY_SOURCE=2 -fno-builtin-memset 
CPPFLAGS=-I. -I.. -I$(srcdir) -I$(srcdir)/.. -I/home/tplink/VG5612_ALGAR/linux_bcm_UIv1/build/../apps/public/openssl/include -I/home/tplink/VG5612_ALGAR/linux_bcm_UIv1/build/../apps/public/zlib-1.2.6  -DHAVE_CONFIG_H
EXEEXT=
LIBCOMPAT=../libopenbsd-compat.a
LIBS=-lcrypto -ldl -lutil -lz  -lcrypt
LDFLAGS=-L/home/tplink/VG5612_ALGAR/linux_bcm_UIv1/build/../apps/public/openssl/lib -L/home/tplink/VG5612_ALGAR/linux_bcm_UIv1/build/../apps/public/zlib-1.2.6  $(LIBCOMPAT)

TESTPROGS=closefromtest$(EXEEXT) snprintftest$(EXEEXT) strduptest$(EXEEXT) \
	strtonumtest$(EXEEXT)

all:	t-exec ${OTHERTESTS}

%$(EXEEXT):	%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $< $(LIBCOMPAT) $(LIBS)

t-exec:	$(TESTPROGS)
	@echo running compat regress tests
	@for TEST in ""$?; do \
		echo "run test $${TEST}" ... 1>&2; \
		./$${TEST}$(EXEEXT) || exit $$? ; \
	done
	@echo finished compat regress tests

clean:
	rm -f *.o *.a core $(TESTPROGS) valid.out

distclean: clean
	rm -f Makefile *~
