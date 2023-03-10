# dnsmasq is Copyright (c) 2000-2013 Simon Kelley
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; version 2 dated June, 1991, or
#  (at your option) version 3 dated 29 June, 2007.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#    
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

# NOTE: Building the i18n targets requires GNU-make 


# Variables you may well want to override.
CURDIR		  = $(shell pwd)
#PREFIX        = /usr/local
#BINDIR        = $(PREFIX)/sbin
BUILDDIR      = $(CURDIR)/src
DESTDIR       = 
CFLAGS += -DNO_DHCP -DNO_TFTP -DNO_DHCP6 -DNO_DBUS -DNO_LOG -DNO_SCRIPT -DNO_IPSET
#################################################################

# Variables you might want to override.

PKG_CONFIG = pkg-config
MSGMERGE   = msgmerge
MSGFMT     = msgfmt
XGETTEXT   = xgettext

SRC = $(BUILDDIR)
PO  = po

#################################################################

dbus_cflags = `echo $(COPTS) | $(CURDIR)/bld/pkg-wrapper HAVE_DBUS $(PKG_CONFIG) --cflags dbus-1` 
dbus_libs =   `echo $(COPTS) | $(CURDIR)/bld/pkg-wrapper HAVE_DBUS $(PKG_CONFIG) --libs dbus-1` 
idn_cflags =  `echo $(COPTS) | $(CURDIR)/bld/pkg-wrapper HAVE_IDN $(PKG_CONFIG) --cflags libidn` 
idn_libs =    `echo $(COPTS) | $(CURDIR)/bld/pkg-wrapper HAVE_IDN $(PKG_CONFIG) --libs libidn` 
ct_cflags =   `echo $(COPTS) | $(CURDIR)/bld/pkg-wrapper HAVE_CONNTRACK $(PKG_CONFIG) --cflags libnetfilter_conntrack`
ct_libs =     `echo $(COPTS) | $(CURDIR)/bld/pkg-wrapper HAVE_CONNTRACK $(PKG_CONFIG) --libs libnetfilter_conntrack`
lua_cflags =  `echo $(COPTS) | $(CURDIR)/bld/pkg-wrapper HAVE_LUASCRIPT $(PKG_CONFIG) --cflags lua5.1` 
lua_libs =    `echo $(COPTS) | $(CURDIR)/bld/pkg-wrapper HAVE_LUASCRIPT $(PKG_CONFIG) --libs lua5.1` 
sunos_libs =  `if uname | grep SunOS >/dev/null 2>&1; then echo -lsocket -lnsl -lposix4; fi`
version =     -DVERSION='\"`$(CURDIR)/bld/get-version $(CURDIR)`\"'

objs = cache.o rfc1035.o util.o option.o forward.o network.o \
       dnsmasq.o  rfc2131.o netlink.o bpf.o \
       log.o conntrack.o rfc3315.o \
       outpacket.o auth.o domain.o

hdrs = dnsmasq.h config.h dhcp-protocol.h dhcp6-protocol.h \
       dns-protocol.h radv-protocol.h

ifeq ($(findstring -DNO_DHCP,$(CFLAGS)),)
	objs += dhcp.o lease.o dhcp-common.o
endif

ifeq ($(findstring -DNO_DHCP6,$(CFLAGS)),)
	objs += dhcp6.o
endif

ifeq ($(findstring -DNO_DBUS,$(CFLAGS)),)
	objs += dbus.o
endif

ifeq ($(findstring -DNO_DBUS,$(CFLAGS)),)
	objs += dbus.o
endif

ifeq ($(findstring -DNO_SCRIPT,$(CFLAGS)),)
	objs += helper.o
endif

ifeq ($(findstring -DNO_IPSET,$(CFLAGS)),)
	objs += ipset.o
endif

#objs += slaac.o radvd.o

all : $(BUILDDIR)
	@cd $(BUILDDIR) && $(MAKE) \
 build_cflags="$(version) $(dbus_cflags) $(idn_cflags) $(ct_cflags) $(lua_cflags)" \
 build_libs="$(dbus_libs) $(idn_libs) $(ct_libs) $(lua_libs) $(sunos_libs)" \
 -f $(CURDIR)/Makefile dnsmasq 

mostly_clean :
	rm -f $(BUILDDIR)/*.mo $(BUILDDIR)/*.pot 
	rm -f $(BUILDDIR)/.configured $(BUILDDIR)/*.o $(BUILDDIR)/dnsmasq.a $(BUILDDIR)/dnsmasq 

clean : mostly_clean
	rm -f $(BUILDDIR)/dnsmasq_baseline
	rm -f core */core
	rm -f *~ contrib/*/*~ */*~

install : all install-common

install-common :
	$(INSTALL) -m 755 $(BUILDDIR)/dnsmasq $(DESTDIR)$(BINDIR)

all-i18n : $(BUILDDIR)
	@cd $(BUILDDIR) && $(MAKE) \
 i18n=-DLOCALEDIR=\'\"$(LOCALEDIR)\"\' \
 build_cflags="$(version) $(dbus_cflags) $(ct_cflags) $(lua_cflags) `$(PKG_CONFIG) --cflags libidn`" \
 build_libs="$(dbus_libs) $(ct_libs) $(lua_libs) $(sunos_libs) `$(PKG_CONFIG) --libs libidn`"  \
 -f $(CURDIR)/Makefile dnsmasq
	for f in `cd $(PO); echo *.po`; do \
		cd $(CURDIR) && cd $(BUILDDIR) && $(MAKE) -f $(CURDIR)/Makefile $${f%.po}.mo; \
	done

install-i18n : all-i18n install-common
	cd $(BUILDDIR); $(CURDIR)/bld/install-mo $(DESTDIR)$(LOCALEDIR) $(INSTALL)
	cd $(MAN); ../bld/install-man $(DESTDIR)$(MANDIR) $(INSTALL)

merge : 
	@cd $(BUILDDIR) && $(MAKE) -f $(CURDIR)/Makefile dnsmasq.pot
	for f in `cd $(PO); echo *.po`; do \
		echo -n msgmerge $(PO)/$$f && $(MSGMERGE) --no-wrap -U $(PO)/$$f $(BUILDDIR)/dnsmasq.pot; \
	done

# Cannonicalise .po file.
%.po : 
	@cd $(BUILDDIR) && $(MAKE) -f $(CURDIR)/Makefile dnsmasq.pot
	mv $(PO)/$*.po $(PO)/$*.po.orig && $(MSGMERGE) --no-wrap $(PO)/$*.po.orig $(BUILDDIR)/dnsmasq.pot >$(PO)/$*.po; 

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# rules below are helpers for size tracking

baseline : mostly_clean all
	@cd $(BUILDDIR) && \
	   mv dnsmasq dnsmasq_baseline

bloatcheck : $(BUILDDIR)/dnsmasq_baseline mostly_clean all
	@cd $(BUILDDIR) && \
           $(CURDIR)/bld/bloat-o-meter dnsmasq_baseline dnsmasq; \
           size dnsmasq_baseline dnsmasq

# rules below are targets in recusive makes with cwd=$(BUILDDIR)

.configured: $(hdrs)
	@rm -f *.o
	@touch $@

$(objs:.o=.c) $(hdrs):
	ln -s $(CURDIR)/$(SRC)/$@ .

.c.o:
	$(CC) $(CFLAGS) $(COPTS) $(i18n) $(build_cflags) $(RPM_OPT_FLAGS) -c $<	

dnsmasq : .configured $(hdrs) $(objs)
#	$(CC) $(LDFLAGS) -o $@ $(objs) $(build_libs) $(LIBS) && $(STRIP) $@
	$(CC) $(LDFLAGS) -o $@ $(objs) $(build_libs) $(LIBS) && $(STRIP) $@ && ls -l $@

dnsmasq.pot : $(objs:.o=.c) $(hdrs)
	$(XGETTEXT) -d dnsmasq --foreign-user --omit-header --keyword=_ -o $@ -i $(objs:.o=.c)

%.mo : $(CURDIR)/$(PO)/%.po dnsmasq.pot
	$(MSGMERGE) -o - $(CURDIR)/$(PO)/$*.po dnsmasq.pot | $(MSGFMT) -o $*.mo -

.PHONY : all clean mostly_clean install install-common all-i18n install-i18n merge baseline bloatcheck
