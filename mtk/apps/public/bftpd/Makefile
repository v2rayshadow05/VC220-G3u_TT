# Generated automatically from Makefile.in by configure.
VERSION=2.2
#CC=$(CROSS)gcc
INSTALL=/usr/bin/install -c
CFLAGS=-g -O2 -DHAVE_CONFIG_H -Wall -I. -DVERSION=\"$(VERSION)\" -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE 
LIBS= -lcrypt
#LIBTCAPI= -lm -L ../lib -ltcapi
LIBTCAPI= -L ../lib -ltcapi
prefix=/usr
mandir=${prefix}/man
sbindir=${exec_prefix}/sbin
DESTDIR=
DIRPAX=
PAX=
ifeq ($(WITHVOIP),1)
CFLAGS += -DWITHVOIP
endif

ifneq ($(strip $(TCSUPPORT_CODE_SHRINK)),)
CFLAGS += -Os
endif

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_C1_NEW_GUI)
ifneq ($(TC_CFLAGS), )
CFLAGS += $(TC_CFLAGS)
endif
ifeq ($(TCSUPPORT_C1_NEW_GUI),1)
    CFLAGS+= -DTCSUPPORT_C1_NEW_GUI
endif
#endif/*TCSUPPORT_COMPILE*/

#if/*TCSUPPORT_COMPILE*/ defined(TCSUPPORT_TTNET)
ifneq ($(TC_CFLAGS), )
CFLAGS += $(TC_CFLAGS)
endif
ifeq ($(TCSUPPORT_TTNET),1)
    CFLAGS+= -DTCSUPPORT_TTNET
endif
#endif/*TCSUPPORT_COMPILE*/
ifeq ($(TCSUPPORT_IPV6_FTP),1)
    CFLAGS+= -DTCSUPPORT_IPV6_FTP
endif

HEADERS=bftpdutmp.h commands.h cwd.h dirlist.h list.h login.h logging.h main.h mystring.h options.h targzip.h mypaths.h
OBJS=bftpdutmp.o commands.o cwd.o dirlist.o list.o login.o logging.o main.o mystring.o options.o 
SRCS=bftpdutmp.c commands.c cwd.c dirlist.c list.c login.c logging.c main.c mystring.c options.c 

ifeq ($(TCSUPPORT_FTP_CMD),1)
OBJS += md5.o commands_admin.o
SRCS += md5.c commands_admin.c
endif

OBJS2LINK=$(OBJS) $(PAX)
LDFLAGS=

all: tcapi_lib bftpd

tcapi_lib:
	$(MAKE) -C $(APP_TCAPILIB_DIR) clean
	$(MAKE) -C $(APP_TCAPILIB_DIR)
	$(MAKE) -C $(APP_TCAPILIB_DIR) install

bftpd: $(OBJS)
	./mksources $(DIRPAX)
	$(CC) $(OBJS2LINK) $(LDFLAGS) $(LIBS) $(LIBTCAPI) -o bftpd
	$(STRIP) bftpd

$(OBJS): $(HEADERS) Makefile

install: all
	$(INSTALL) -g 0 -m 700 -o 0 bftpd $(DESTDIR)/$(prefix)/sbin
	$(INSTALL) -g 0 -m 644 -o 0 bftpd.8 $(DESTDIR)/$(mandir)/man8
	[ -f $(DESTDIR)/etc/bftpd.conf ] || \
		$(INSTALL) -g 0 -m 600 -o 0 bftpd.conf $(DESTDIR)/etc
	touch /var/log/bftpd.log
	chmod 644 /var/log/bftpd.log
	mkdir -p /var/run/bftpd
	chmod 755 /var/run/bftpd

clean distclean:
	rm -f *~ $(OBJS) bftpd mksources.finished config.cache
	[ "$(DIRPAX)" = "" ] || make -C $(DIRPAX) clean

newversion: clean
	cat Makefile.in | sed -e s/$(VERSION)/$(NEWVERSION)/g > Makefile.foo
	mv Makefile.foo Makefile.in
	./configure --enable-pax=pax --enable-libz --enable-pam

uninstall:
	rm -f $(DESTDIR)/$(prefix)/sbin/bftpd $(DESTDIR)/$(mandir)/man8/bftpd.8 \
		$(DESTDIR)/etc/bftpd.conf

distribute: install
	rm -rf dist
	mkdir dist
	# Build source tarball
	rm -rf bftpd-$(VERSION)
	mkdir bftpd-$(VERSION)
	autoconf
	autoheader configure.in > config.h.in
	mkdir bftpd-$(VERSION)/doc
	cp CHANGELOG COPYING Makefile.in $(SRCS) $(HEADERS) \
	configure.in configure config.h.in bftpd.conf install-sh mksources \
	bftpd.spec.in acconfig.h bftpd.8 bftpd-$(VERSION)
	cp -r doc bftpd-$(VERSION)
	cd bftpd-$(VERSION)/doc/en && sgml2txt bftpddoc-en.sgml
	cd bftpd-$(VERSION) && ln -s doc/en/bftpddoc-en.txt INSTALL
	cd bftpd-$(VERSION) && ln -s doc/en/bftpddoc-en.txt README
	rm -f -r debian/tmp
	cp -a debian bftpd-$(VERSION)
	tar c bftpd-$(VERSION) | gzip -v9 > dist/bftpd-$(VERSION).tar.gz
	cp -Lr pax bftpd-$(VERSION)
	tar c bftpd-$(VERSION) | gzip -v9 > bftpd-$(VERSION)-pax.tar.gz
	# Build binary RPM
	./configure --enable-pam --enable-libz --enable-pax=pax && make install
	cat bftpd.spec.in | sed -e 's/VERSION/$(VERSION)/g' \
	> bftpd-$(VERSION)-1.spec
	rpm -bb bftpd-$(VERSION)-1.spec
	rm -f bftpd-$(VERSION)-1.spec
	cp -a /usr/src/rpm/RPMS/i386/bftpd-$(VERSION)-1.i386.rpm \
	dist/bftpd-$(VERSION).i386.rpm
	# Build binary DEB
	mv bftpd-$(VERSION)-pax.tar.gz bftpd-$(VERSION).tar.gz
	cd bftpd-$(VERSION) && (\
		cat debian/changelog.in | sed -e 's/VERSION/$(VERSION)/g' \
			> debian/changelog; \
		dpkg-buildpackage; \
	) && cd ..
	rm -f bftpd-$(VERSION).tar.gz
	cp bftpd_$(VERSION)-1_i386.deb dist
	rm -f bftpd_*
	# Remove temporary directory
	rm -rf bftpd-$(VERSION)
