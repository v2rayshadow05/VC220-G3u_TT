# Makefile.in generated by automake 1.9.5 from Makefile.am.
# Makefile.  Generated from Makefile.in by configure.

# Copyright (C) 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002,
# 2003, 2004, 2005  Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.



#
#   $Id: //BBN_Linux/Branch/Branch_for_Rel_EN7512_SDK_20150916/tclinux_phoenix/apps/public/radvd-1.5/Makefile#1 $
#
#   Authors:
#    Lars Fenneberg		<lf@elemental.net>	 
#
#   This software is Copyright 1996,1997 by the above mentioned author(s), 
#   All Rights Reserved.
#
#   The license which is distributed with this software in the file COPYRIGHT
#   applies to this software. If your distribution is missing this file, you
#   may request it from <pekkas@netcore.fi>.
#

SOURCES = $(radvd_SOURCES) $(EXTRA_radvd_SOURCES) $(radvdump_SOURCES)

srcdir = .
top_srcdir = .

pkgdatadir = $(datadir)/radvd
pkglibdir = $(libdir)/radvd
pkgincludedir = $(includedir)/radvd
top_builddir = .
am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
INSTALL = /usr/bin/install -c
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_HEADER = $(INSTALL_DATA)
transform = $(program_transform_name)
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
build_triplet = i686-pc-linux-gnu
host_triplet = mips-mips-linux-gnu
target_triplet = mips-mips-linux-gnu
sbin_PROGRAMS = radvd$(EXEEXT) radvdump$(EXEEXT)
DIST_COMMON = README $(am__configure_deps) $(srcdir)/Makefile.am \
	$(srcdir)/Makefile.in $(srcdir)/config.h.in \
	$(top_srcdir)/configure TODO config.guess config.sub depcomp \
	gram.c gram.h install-sh missing mkinstalldirs scanner.c
subdir = .
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
am__aclocal_m4_deps = $(top_srcdir)/configure.in
am__configure_deps = $(am__aclocal_m4_deps) $(CONFIGURE_DEPENDENCIES) \
	$(ACLOCAL_M4)
am__CONFIG_DISTCLEAN_FILES = config.status config.cache config.log \
 configure.lineno configure.status.lineno
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = config.h
CONFIG_CLEAN_FILES =
am__installdirs = "$(DESTDIR)$(sbindir)" "$(DESTDIR)$(man5dir)" \
	"$(DESTDIR)$(man8dir)"
sbinPROGRAMS_INSTALL = $(INSTALL_PROGRAM)
PROGRAMS = $(sbin_PROGRAMS)
am__objects_1 = log.$(OBJEXT) socket.$(OBJEXT) recv.$(OBJEXT) \
	util.$(OBJEXT)
am_radvd_OBJECTS = $(am__objects_1) radvd.$(OBJEXT) timer.$(OBJEXT) \
	send.$(OBJEXT) process.$(OBJEXT) interface.$(OBJEXT) \
	device.$(OBJEXT) device-common.$(OBJEXT) privsep.$(OBJEXT) \
	gram.$(OBJEXT) scanner.$(OBJEXT)
radvd_OBJECTS = $(am_radvd_OBJECTS)
radvd_DEPENDENCIES = ../lib/
am_radvdump_OBJECTS = $(am__objects_1) radvdump.$(OBJEXT)
radvdump_OBJECTS = $(am_radvdump_OBJECTS)
radvdump_LDADD = $(LDADD)
DEFAULT_INCLUDES = -I. -I$(srcdir) -I.
depcomp = $(SHELL) $(top_srcdir)/depcomp
am__depfiles_maybe = depfiles
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
LEXCOMPILE = $(LEX) $(LFLAGS) $(AM_LFLAGS)
YACCCOMPILE = $(YACC) $(YFLAGS) $(AM_YFLAGS)
SOURCES = $(radvd_SOURCES) $(EXTRA_radvd_SOURCES) $(radvdump_SOURCES)
DIST_SOURCES = $(radvd_SOURCES) $(EXTRA_radvd_SOURCES) \
	$(radvdump_SOURCES)
man5dir = $(mandir)/man5
man8dir = $(mandir)/man8
NROFF = nroff
MANS = $(man_MANS)
ETAGS = etags
CTAGS = ctags
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)
distdir = $(PACKAGE)-$(VERSION)
top_distdir = $(distdir)
am__remove_distdir = \
  { test ! -d $(distdir) \
    || { find $(distdir) -type d ! -perm -200 -exec chmod u+w {} ';' \
         && rm -fr $(distdir); }; }
DIST_ARCHIVES = $(distdir).tar.gz
GZIP_ENV = --best
distuninstallcheck_listfiles = find . -type f -print
distcleancheck_listfiles = find . -type f -print
#ACLOCAL = ${SHELL} /mnt/data/socsw/jlliu_nj/CVS/phoenix_base/tclinux_phoenix/apps/radvd-1.5/missing --run aclocal-1.9
AMDEP_FALSE = #
AMDEP_TRUE = 
#AMTAR = ${SHELL} /mnt/data/socsw/jlliu_nj/CVS/phoenix_base/tclinux_phoenix/apps/radvd-1.5/missing --run tar
#AUTOCONF = ${SHELL} /mnt/data/socsw/jlliu_nj/CVS/phoenix_base/tclinux_phoenix/apps/radvd-1.5/missing --run autoconf
#AUTOHEADER = ${SHELL} /mnt/data/socsw/jlliu_nj/CVS/phoenix_base/tclinux_phoenix/apps/radvd-1.5/missing --run autoheader
#AUTOMAKE = ${SHELL} /mnt/data/socsw/jlliu_nj/CVS/phoenix_base/tclinux_phoenix/apps/radvd-1.5/missing --run automake-1.9
AWK = gawk
#CC = mips-linux-gcc
CCDEPMODE = depmode=gcc3
CFLAGS = -g -Wall -Wpointer-arith  -Wcast-qual -Wcast-align \
	-Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations
ifneq ($(strip $(TCSUPPORT_CODE_SHRINK)),)
CFLAGS += -Os
else
CFLAGS += -O2 
endif
ifneq ($(TC_CFLAGS), )
CFLAGS+=$(TC_CFLAGS)
endif
#CPP = mips-linux-gcc -E
CPPFLAGS = 
CYGPATH_W = echo
DEFS = -DPATH_RADVD_CONF=\"$(PATH_RADVD_CONF)\" \
	-DPATH_RADVD_LOG=\"$(PATH_RADVD_LOG)\" -DLOG_FACILITY=$(LOG_FACILITY) \
	-DPATH_RADVD_PID=\"$(PATH_RADVD_PID)\" \
	-DVERSION=\"$(VERSION)\" -DINET6=1

DEPDIR = .deps
ECHO_C = 
ECHO_N = -n
ECHO_T = 
EGREP = grep -E
EXEEXT = 
GZIP = /bin/gzip
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_STRIP_PROGRAM = ${SHELL} $(install_sh) -c -s
LDFLAGS = 
LEX = flex
LEXLIB = 
LEX_OUTPUT_ROOT = lex.yy
LIBOBJS = 
LIBS = -lc -L ../lib -ltcapi
LN = /bin/ln
LOG_FACILITY = LOG_DAEMON
LTLIBOBJS = 
#MAKEINFO = ${SHELL} /mnt/data/socsw/jlliu_nj/CVS/phoenix_base/tclinux_phoenix/apps/radvd-1.5/missing --run makeinfo
OBJEXT = o
PACKAGE = radvd
PACKAGE_BUGREPORT = 
PACKAGE_NAME = 
PACKAGE_STRING = 
PACKAGE_TARNAME = 
PACKAGE_VERSION = 
PATH_RADVD_CONF = ${prefix}/etc/radvd.conf
PATH_RADVD_LOG = /var/log/radvd.log
PATH_RADVD_PID = /var/run/radvd.pid
PATH_SEPARATOR = :
RM = /bin/rm
SED = /bin/sed
SET_MAKE = 
SHELL = /bin/sh
#STRIP = mips-linux-strip
TAR = /bin/tar
VERSION = 1.5
YACC = bison -y
ac_ct_CC = 
ac_ct_STRIP = 
am__fastdepCC_FALSE = #
am__fastdepCC_TRUE = 
am__include = include
am__leading_dot = .
am__quote = 
am__tar = ${AMTAR} chof - "$$tardir"
am__untar = ${AMTAR} xf -
bindir = ${exec_prefix}/bin
build = i686-pc-linux-gnu
build_alias = 
build_cpu = i686
build_os = linux-gnu
build_vendor = pc
datadir = ${prefix}/share
exec_prefix = ${prefix}
host = mips-mips-linux-gnu
host_alias = mips-linux
host_cpu = mips
host_os = linux-gnu
host_vendor = mips
includedir = ${prefix}/include
infodir = ${prefix}/info
#install_sh = /mnt/data/socsw/jlliu_nj/CVS/phoenix_base/tclinux_phoenix/apps/radvd-1.5/install-sh
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
localstatedir = ${prefix}/var
mandir = ${prefix}/man
mkdir_p = mkdir -p --
oldincludedir = /usr/include
prefix = /usr/local
program_transform_name = s,x,x,
sbindir = ${exec_prefix}/sbin
sharedstatedir = ${prefix}/com
sysconfdir = ${prefix}/etc
target = mips-mips-linux-gnu
target_alias = 
target_cpu = mips
target_os = linux-gnu
target_vendor = mips
AUTOMAKE_OPTIONS = 1.2 foreign
INCLUDES = -I$(srcdir) -I.

########################################################################
COMMON_SRC = log.c socket.c recv.c util.c radvd.h defaults.h pathnames.h \
	includes.h

radvd_SOURCES = $(COMMON_SRC) radvd.c timer.c send.c process.c interface.c \
	device.c device-common.c privsep.c gram.y gram.h scanner.l

radvd_LDADD = -L ../lib/ -lfl
EXTRA_radvd_SOURCES = device-linux.c device-bsd44.c privsep-linux.c privsep-bsd44.c
YFLAGS = -d
radvdump_SOURCES = $(COMMON_SRC) radvdump.c
man_MANS = radvd.8 radvdump.8 radvd.conf.5
EXTRA_DIST = radvd.8.man radvdump.8.man radvd.conf.5.man VERSION CHANGES \
	COPYRIGHT INTRO.html copyright.blurb radvd.conf.example

BUILT_SOURCES = device.c
CLEANFILES = radvd.8 radvdump.8 radvd.conf.5
DISTCLEANFILES = device.c
SUFFIXES = .man
all: $(BUILT_SOURCES) config.h
	$(MAKE) $(AM_MAKEFLAGS) all-am


distclean-hdr:
	-rm -f config.h stamp-h1
install-sbinPROGRAMS: $(sbin_PROGRAMS)
	@$(NORMAL_INSTALL)
	test -z "$(sbindir)" || $(mkdir_p) "$(DESTDIR)$(sbindir)"
	@list='$(sbin_PROGRAMS)'; for p in $$list; do \
	  p1=`echo $$p|sed 's/$(EXEEXT)$$//'`; \
	  if test -f $$p \
	  ; then \
	    f=`echo "$$p1" | sed 's,^.*/,,;$(transform);s/$$/$(EXEEXT)/'`; \
	   echo " $(INSTALL_PROGRAM_ENV) $(sbinPROGRAMS_INSTALL) '$$p' '$(DESTDIR)$(sbindir)/$$f'"; \
	   $(INSTALL_PROGRAM_ENV) $(sbinPROGRAMS_INSTALL) "$$p" "$(DESTDIR)$(sbindir)/$$f" || exit 1; \
	  else :; fi; \
	done

uninstall-sbinPROGRAMS:
	@$(NORMAL_UNINSTALL)
	@list='$(sbin_PROGRAMS)'; for p in $$list; do \
	  f=`echo "$$p" | sed 's,^.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	  echo " rm -f '$(DESTDIR)$(sbindir)/$$f'"; \
	  rm -f "$(DESTDIR)$(sbindir)/$$f"; \
	done

clean-sbinPROGRAMS:
	-test -z "$(sbin_PROGRAMS)" || rm -f $(sbin_PROGRAMS)
gram.h: gram.c
	@if test ! -f $@; then \
	  rm -f gram.c; \
	  $(MAKE) gram.c; \
	else :; fi
radvd$(EXEEXT): $(radvd_OBJECTS) $(radvd_DEPENDENCIES) 
	@rm -f radvd$(EXEEXT)
	$(LINK) $(radvd_LDFLAGS) $(radvd_OBJECTS) $(radvd_LDADD) $(LIBS)
radvdump$(EXEEXT): $(radvdump_OBJECTS) $(radvdump_DEPENDENCIES) 
	@rm -f radvdump$(EXEEXT)
	$(LINK) $(radvdump_LDFLAGS) $(radvdump_OBJECTS) $(radvdump_LDADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT)

distclean-compile:
	-rm -f *.tab.c

include ./$(DEPDIR)/device-bsd44.Po
include ./$(DEPDIR)/device-common.Po
include ./$(DEPDIR)/device-linux.Po
include ./$(DEPDIR)/device.Po
include ./$(DEPDIR)/gram.Po
include ./$(DEPDIR)/interface.Po
include ./$(DEPDIR)/log.Po
include ./$(DEPDIR)/privsep-bsd44.Po
include ./$(DEPDIR)/privsep-linux.Po
include ./$(DEPDIR)/privsep.Po
include ./$(DEPDIR)/process.Po
include ./$(DEPDIR)/radvd.Po
include ./$(DEPDIR)/radvdump.Po
include ./$(DEPDIR)/recv.Po
include ./$(DEPDIR)/scanner.Po
include ./$(DEPDIR)/send.Po
include ./$(DEPDIR)/socket.Po
include ./$(DEPDIR)/timer.Po
include ./$(DEPDIR)/util.Po

.c.o:
	if $(COMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" -c -o $@ $<; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Po"; else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; fi
#	source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) \
#	$(COMPILE) -c $<

.c.obj:
	if $(COMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" -c -o $@ `$(CYGPATH_W) '$<'`; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Po"; else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; fi
#	source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) \
#	$(COMPILE) -c `$(CYGPATH_W) '$<'`

.l.c:
	$(LEXCOMPILE) $<
	sed '/^#/ s|$(LEX_OUTPUT_ROOT)\.c|$@|' $(LEX_OUTPUT_ROOT).c >$@
	rm -f $(LEX_OUTPUT_ROOT).c

.y.c:
	$(YACCCOMPILE) $<
	if test -f y.tab.h; then \
	  to=`echo "$*_H" | sed \
                -e 'y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/' \
                -e 's/[^ABCDEFGHIJKLMNOPQRSTUVWXYZ]/_/g'`; \
	  sed -e "/^#/!b" -e "s/Y_TAB_H/$$to/g" -e "s|y\.tab\.h|$*.h|" \
            y.tab.h >$*.ht; \
	  rm -f y.tab.h; \
	  if cmp -s $*.ht $*.h; then \
	    rm -f $*.ht ;\
	  else \
	    mv $*.ht $*.h; \
	  fi; \
	fi
	if test -f y.output; then \
	  mv y.output $*.output; \
	fi
	sed '/^#/ s|y\.tab\.c|$@|' y.tab.c >$@t && mv $@t $@
	rm -f y.tab.c
uninstall-info-am:
install-man5: $(man5_MANS) $(man_MANS)
	@$(NORMAL_INSTALL)
	test -z "$(man5dir)" || $(mkdir_p) "$(DESTDIR)$(man5dir)"
	@list='$(man5_MANS) $(dist_man5_MANS) $(nodist_man5_MANS)'; \
	l2='$(man_MANS) $(dist_man_MANS) $(nodist_man_MANS)'; \
	for i in $$l2; do \
	  case "$$i" in \
	    *.5*) list="$$list $$i" ;; \
	  esac; \
	done; \
	for i in $$list; do \
	  if test -f $(srcdir)/$$i; then file=$(srcdir)/$$i; \
	  else file=$$i; fi; \
	  ext=`echo $$i | sed -e 's/^.*\\.//'`; \
	  case "$$ext" in \
	    5*) ;; \
	    *) ext='5' ;; \
	  esac; \
	  inst=`echo $$i | sed -e 's/\\.[0-9a-z]*$$//'`; \
	  inst=`echo $$inst | sed -e 's/^.*\///'`; \
	  inst=`echo $$inst | sed '$(transform)'`.$$ext; \
	  echo " $(INSTALL_DATA) '$$file' '$(DESTDIR)$(man5dir)/$$inst'"; \
	  $(INSTALL_DATA) "$$file" "$(DESTDIR)$(man5dir)/$$inst"; \
	done
uninstall-man5:
	@$(NORMAL_UNINSTALL)
	@list='$(man5_MANS) $(dist_man5_MANS) $(nodist_man5_MANS)'; \
	l2='$(man_MANS) $(dist_man_MANS) $(nodist_man_MANS)'; \
	for i in $$l2; do \
	  case "$$i" in \
	    *.5*) list="$$list $$i" ;; \
	  esac; \
	done; \
	for i in $$list; do \
	  ext=`echo $$i | sed -e 's/^.*\\.//'`; \
	  case "$$ext" in \
	    5*) ;; \
	    *) ext='5' ;; \
	  esac; \
	  inst=`echo $$i | sed -e 's/\\.[0-9a-z]*$$//'`; \
	  inst=`echo $$inst | sed -e 's/^.*\///'`; \
	  inst=`echo $$inst | sed '$(transform)'`.$$ext; \
	  echo " rm -f '$(DESTDIR)$(man5dir)/$$inst'"; \
	  rm -f "$(DESTDIR)$(man5dir)/$$inst"; \
	done
install-man8: $(man8_MANS) $(man_MANS)
	@$(NORMAL_INSTALL)
	test -z "$(man8dir)" || $(mkdir_p) "$(DESTDIR)$(man8dir)"
	@list='$(man8_MANS) $(dist_man8_MANS) $(nodist_man8_MANS)'; \
	l2='$(man_MANS) $(dist_man_MANS) $(nodist_man_MANS)'; \
	for i in $$l2; do \
	  case "$$i" in \
	    *.8*) list="$$list $$i" ;; \
	  esac; \
	done; \
	for i in $$list; do \
	  if test -f $(srcdir)/$$i; then file=$(srcdir)/$$i; \
	  else file=$$i; fi; \
	  ext=`echo $$i | sed -e 's/^.*\\.//'`; \
	  case "$$ext" in \
	    8*) ;; \
	    *) ext='8' ;; \
	  esac; \
	  inst=`echo $$i | sed -e 's/\\.[0-9a-z]*$$//'`; \
	  inst=`echo $$inst | sed -e 's/^.*\///'`; \
	  inst=`echo $$inst | sed '$(transform)'`.$$ext; \
	  echo " $(INSTALL_DATA) '$$file' '$(DESTDIR)$(man8dir)/$$inst'"; \
	  $(INSTALL_DATA) "$$file" "$(DESTDIR)$(man8dir)/$$inst"; \
	done
uninstall-man8:
	@$(NORMAL_UNINSTALL)
	@list='$(man8_MANS) $(dist_man8_MANS) $(nodist_man8_MANS)'; \
	l2='$(man_MANS) $(dist_man_MANS) $(nodist_man_MANS)'; \
	for i in $$l2; do \
	  case "$$i" in \
	    *.8*) list="$$list $$i" ;; \
	  esac; \
	done; \
	for i in $$list; do \
	  ext=`echo $$i | sed -e 's/^.*\\.//'`; \
	  case "$$ext" in \
	    8*) ;; \
	    *) ext='8' ;; \
	  esac; \
	  inst=`echo $$i | sed -e 's/\\.[0-9a-z]*$$//'`; \
	  inst=`echo $$inst | sed -e 's/^.*\///'`; \
	  inst=`echo $$inst | sed '$(transform)'`.$$ext; \
	  echo " rm -f '$(DESTDIR)$(man8dir)/$$inst'"; \
	  rm -f "$(DESTDIR)$(man8dir)/$$inst"; \
	done

ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	mkid -fID $$unique
tags: TAGS

TAGS:  $(HEADERS) $(SOURCES) config.h.in $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS) config.h.in $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	if test -z "$(ETAGS_ARGS)$$tags$$unique"; then :; else \
	  test -n "$$unique" || unique=$$empty_fix; \
	  $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	    $$tags $$unique; \
	fi
ctags: CTAGS
CTAGS:  $(HEADERS) $(SOURCES) config.h.in $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS) config.h.in $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(CTAGS_ARGS)$$tags$$unique" \
	  || $(CTAGS) $(CTAGSFLAGS) $(AM_CTAGSFLAGS) $(CTAGS_ARGS) \
	     $$tags $$unique

GTAGS:
	here=`$(am__cd) $(top_builddir) && pwd` \
	  && cd $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) $$here

distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH tags

distdir: $(DISTFILES)
	$(am__remove_distdir)
	mkdir $(distdir)
	@srcdirstrip=`echo "$(srcdir)" | sed 's|.|.|g'`; \
	topsrcdirstrip=`echo "$(top_srcdir)" | sed 's|.|.|g'`; \
	list='$(DISTFILES)'; for file in $$list; do \
	  case $$file in \
	    $(srcdir)/*) file=`echo "$$file" | sed "s|^$$srcdirstrip/||"`;; \
	    $(top_srcdir)/*) file=`echo "$$file" | sed "s|^$$topsrcdirstrip/|$(top_builddir)/|"`;; \
	  esac; \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  dir=`echo "$$file" | sed -e 's,/[^/]*$$,,'`; \
	  if test "$$dir" != "$$file" && test "$$dir" != "."; then \
	    dir="/$$dir"; \
	    $(mkdir_p) "$(distdir)$$dir"; \
	  else \
	    dir=''; \
	  fi; \
	  if test -d $$d/$$file; then \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -pR $(srcdir)/$$file $(distdir)$$dir || exit 1; \
	    fi; \
	    cp -pR $$d/$$file $(distdir)$$dir || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
	$(MAKE) $(AM_MAKEFLAGS) \
	  top_distdir="$(top_distdir)" distdir="$(distdir)" \
	  dist-hook
	-find $(distdir) -type d ! -perm -777 -exec chmod a+rwx {} \; -o \
	  ! -type d ! -perm -444 -links 1 -exec chmod a+r {} \; -o \
	  ! -type d ! -perm -400 -exec chmod a+r {} \; -o \
	  ! -type d ! -perm -444 -exec $(SHELL) $(install_sh) -c -m a+r {} {} \; \
	|| chmod -R a+r $(distdir)
dist-gzip: distdir
	tardir=$(distdir) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	$(am__remove_distdir)

dist-bzip2: distdir
	tardir=$(distdir) && $(am__tar) | bzip2 -9 -c >$(distdir).tar.bz2
	$(am__remove_distdir)

dist-tarZ: distdir
	tardir=$(distdir) && $(am__tar) | compress -c >$(distdir).tar.Z
	$(am__remove_distdir)

dist-shar: distdir
	shar $(distdir) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).shar.gz
	$(am__remove_distdir)

dist-zip: distdir
	-rm -f $(distdir).zip
	zip -rq $(distdir).zip $(distdir)
	$(am__remove_distdir)

dist dist-all: distdir
	tardir=$(distdir) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	$(am__remove_distdir)

# This target untars the dist file and tries a VPATH configuration.  Then
# it guarantees that the distribution is self-contained by making another
# tarfile.
distcheck: dist
	case '$(DIST_ARCHIVES)' in \
	*.tar.gz*) \
	  GZIP=$(GZIP_ENV) gunzip -c $(distdir).tar.gz | $(am__untar) ;;\
	*.tar.bz2*) \
	  bunzip2 -c $(distdir).tar.bz2 | $(am__untar) ;;\
	*.tar.Z*) \
	  uncompress -c $(distdir).tar.Z | $(am__untar) ;;\
	*.shar.gz*) \
	  GZIP=$(GZIP_ENV) gunzip -c $(distdir).shar.gz | unshar ;;\
	*.zip*) \
	  unzip $(distdir).zip ;;\
	esac
	chmod -R a-w $(distdir); chmod a+w $(distdir)
	mkdir $(distdir)/_build
	mkdir $(distdir)/_inst
	chmod a-w $(distdir)
	dc_install_base=`$(am__cd) $(distdir)/_inst && pwd | sed -e 's,^[^:\\/]:[\\/],/,'` \
	  && dc_destdir="$${TMPDIR-/tmp}/am-dc-$$$$/" \
	  && cd $(distdir)/_build \
	  && ../configure --srcdir=.. --prefix="$$dc_install_base" \
	    $(DISTCHECK_CONFIGURE_FLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) dvi \
	  && $(MAKE) $(AM_MAKEFLAGS) check \
	  && $(MAKE) $(AM_MAKEFLAGS) install \
	  && $(MAKE) $(AM_MAKEFLAGS) installcheck \
	  && $(MAKE) $(AM_MAKEFLAGS) uninstall \
	  && $(MAKE) $(AM_MAKEFLAGS) distuninstallcheck_dir="$$dc_install_base" \
	        distuninstallcheck \
	  && chmod -R a-w "$$dc_install_base" \
	  && ({ \
	       (cd ../.. && umask 077 && mkdir "$$dc_destdir") \
	       && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$$dc_destdir" install \
	       && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$$dc_destdir" uninstall \
	       && $(MAKE) $(AM_MAKEFLAGS) DESTDIR="$$dc_destdir" \
	            distuninstallcheck_dir="$$dc_destdir" distuninstallcheck; \
	      } || { rm -rf "$$dc_destdir"; exit 1; }) \
	  && rm -rf "$$dc_destdir" \
	  && $(MAKE) $(AM_MAKEFLAGS) dist \
	  && rm -rf $(DIST_ARCHIVES) \
	  && $(MAKE) $(AM_MAKEFLAGS) distcleancheck
	$(am__remove_distdir)
	@(echo "$(distdir) archives ready for distribution: "; \
	  list='$(DIST_ARCHIVES)'; for i in $$list; do echo $$i; done) | \
	  sed -e '1{h;s/./=/g;p;x;}' -e '$${p;x;}'
distuninstallcheck:
	@cd $(distuninstallcheck_dir) \
	&& test `$(distuninstallcheck_listfiles) | wc -l` -le 1 \
	   || { echo "ERROR: files left after uninstall:" ; \
	        if test -n "$(DESTDIR)"; then \
	          echo "  (check DESTDIR support)"; \
	        fi ; \
	        $(distuninstallcheck_listfiles) ; \
	        exit 1; } >&2
distcleancheck: distclean
	@if test '$(srcdir)' = . ; then \
	  echo "ERROR: distcleancheck can only run from a VPATH build" ; \
	  exit 1 ; \
	fi
	@test `$(distcleancheck_listfiles) | wc -l` -eq 0 \
	  || { echo "ERROR: files left in build directory after distclean:" ; \
	       $(distcleancheck_listfiles) ; \
	       exit 1; } >&2
check-am: all-am
check: $(BUILT_SOURCES)
	$(MAKE) $(AM_MAKEFLAGS) check-am
all-am: Makefile $(PROGRAMS) config.h
installdirs:
	for dir in "$(DESTDIR)$(sbindir)" "$(DESTDIR)$(man5dir)" "$(DESTDIR)$(man8dir)"; do \
	  test -z "$$dir" || $(mkdir_p) "$$dir"; \
	done
install: $(BUILT_SOURCES)
	$(MAKE) $(AM_MAKEFLAGS) install-am
install-exec: install-exec-am
install-data: install-data-am
uninstall: uninstall-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	  install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	  `test -z '$(STRIP)' || \
	    echo "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'"` install
mostlyclean-generic:

clean-generic:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean-generic:
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)
	-test -z "$(DISTCLEANFILES)" || rm -f $(DISTCLEANFILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
	-rm -f gram.c
	-rm -f gram.h
	-rm -f scanner.c
	-test -z "$(BUILT_SOURCES)" || rm -f $(BUILT_SOURCES)
clean: clean-am

clean-am: clean-generic clean-sbinPROGRAMS mostlyclean-am

distclean: distclean-am
	-rm -f $(am__CONFIG_DISTCLEAN_FILES)
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
distclean-am: clean-am distclean-compile distclean-generic \
	distclean-hdr distclean-tags

dvi: dvi-am

dvi-am:

html: html-am

info: info-am

info-am:

install-data-am: install-man

install-exec-am: install-sbinPROGRAMS

install-info: install-info-am

install-man: install-man5 install-man8

installcheck-am:

maintainer-clean: maintainer-clean-am
	-rm -f $(am__CONFIG_DISTCLEAN_FILES)
	-rm -rf $(top_srcdir)/autom4te.cache
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic

pdf: pdf-am

pdf-am:

ps: ps-am

ps-am:

uninstall-am: uninstall-info-am uninstall-man uninstall-sbinPROGRAMS

uninstall-man: uninstall-man5 uninstall-man8

.PHONY: CTAGS GTAGS all all-am am--refresh check check-am clean \
	clean-generic clean-sbinPROGRAMS ctags dist dist-all \
	dist-bzip2 dist-gzip dist-hook dist-shar dist-tarZ dist-zip \
	distcheck distclean distclean-compile distclean-generic \
	distclean-hdr distclean-tags distcleancheck distdir \
	distuninstallcheck dvi dvi-am html html-am info info-am \
	install install-am install-data install-data-am install-exec \
	install-exec-am install-info install-info-am install-man \
	install-man5 install-man8 install-sbinPROGRAMS install-strip \
	installcheck installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-compile \
	mostlyclean-generic pdf pdf-am ps ps-am tags uninstall \
	uninstall-am uninstall-info-am uninstall-man uninstall-man5 \
	uninstall-man8 uninstall-sbinPROGRAMS


.man:
	$(SED) 	-e 's,@''VERSION''@,$(VERSION),g' \
		-e 's,@''sbindir''@,$(sbindir),g' \
		-e 's,@''PATH_RADVD_CONF''@,$(PATH_RADVD_CONF),g' \
		-e 's,@''PATH_RADVD_PID''@,$(PATH_RADVD_PID),g' \
		-e 's,@''PATH_RADVD_LOG''@,$(PATH_RADVD_LOG),g' \
		-e 's,@''LOG_FACILITY''@,$(LOG_FACILITY),g' \
		$< > $@

dist-hook:
	rm -f $(distdir)/device.c	
	mkdir $(distdir)/redhat
	cp -p $(srcdir)/redhat/radvd.init $(srcdir)/redhat/radvd.spec \
		$(srcdir)/redhat/radvd.sysconfig $(distdir)/redhat

scanner.o: gram.h
# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
