# -*- Makefile -*-

top_builddir := ..
builddir     := .
top_srcdir  := ..
srcdir      := .
ksourcedir  := /home/dev/linux_mtk_VR410v1/mtk/linux-2.6.36
prefix      := /home/dev/linux_mtk_VR410v1/mtk/apps/public/iptables-1.4.10/install
exec_prefix := ${prefix}
libdir      := ${exec_prefix}/lib
libexecdir  := ${exec_prefix}/libexec
xtlibdir    := ${exec_prefix}/libexec/xtables

CC             := mips-unknown-linux-uclibc-gcc -mips32r2 -msoft-float
CCLD           := ${CC}
CFLAGS         := -msoft-float -fomit-frame-pointer -mips32r2 -muclibc -O2
LDFLAGS        := 
regular_CFLAGS := -D_LARGEFILE_SOURCE=1 -D_LARGE_FILES -D_FILE_OFFSET_BITS=64 	-D_REENTRANT -Wall -Waggregate-return -Wmissing-declarations 	-Wmissing-prototypes -Wredundant-decls -Wshadow -Wstrict-prototypes 	-Winline -pipe 	-DXTABLES_LIBDIR=\"${xtlibdir}\" -DXTABLES_INTERNAL
kinclude_CFLAGS :=  -I /home/dev/linux_mtk_VR410v1/mtk/linux-2.6.36/include -I /home/dev/linux_mtk_VR410v1/mtk/linux-2.6.36/include

AM_CFLAGS      := ${regular_CFLAGS} -I${top_builddir}/include -I${top_srcdir}/include ${kinclude_CFLAGS}
AM_DEPFLAGS     = -Wp,-MMD,$(@D)/.$(@F).d,-MT,$@

ifeq (${V},)
AM_LIBTOOL_SILENT = --silent
AM_VERBOSE_CC     = @echo "  CC      " $@;
AM_VERBOSE_CCLD   = @echo "  CCLD    " $@;
AM_VERBOSE_CXX    = @echo "  CXX     " $@;
AM_VERBOSE_CXXLD  = @echo "  CXXLD   " $@;
AM_VERBOSE_AR     = @echo "  AR      " $@;
AM_VERBOSE_GEN    = @echo "  GEN     " $@;
endif

#
#	Wildcard module list
#
pfx_build_mod := $(patsubst ${srcdir}/libxt_%.c,%,$(wildcard ${srcdir}/libxt_*.c))
 pf4_build_mod := $(patsubst ${srcdir}/libipt_%.c,%,$(wildcard ${srcdir}/libipt_*.c))
 pf6_build_mod := $(patsubst ${srcdir}/libip6t_%.c,%,$(wildcard ${srcdir}/libip6t_*.c))
pfx_build_mod := $(filter-out ,${pfx_build_mod})
pf4_build_mod := $(filter-out ,${pf4_build_mod})
pf6_build_mod := $(filter-out ,${pf6_build_mod})
pfx_objs      := $(patsubst %,libxt_%.o,${pfx_build_mod})
pf4_objs      := $(patsubst %,libipt_%.o,${pf4_build_mod})
pf6_objs      := $(patsubst %,libip6t_%.o,${pf6_build_mod})
pfx_solibs    := $(patsubst %,libxt_%.so,${pfx_build_mod})
pf4_solibs    := $(patsubst %,libipt_%.so,${pf4_build_mod})
pf6_solibs    := $(patsubst %,libip6t_%.so,${pf6_build_mod})


#
# Building blocks
#
targets := libext4.a libext6.a matches4.man matches6.man \
           targets4.man targets6.man
targets_install :=
 libext4_objs := ${pfx_objs} ${pf4_objs}
 libext6_objs := ${pfx_objs} ${pf6_objs}
# targets += ${pfx_solibs} ${pf4_solibs} ${pf6_solibs}
# targets_install += ${pfx_solibs} ${pf4_solibs} ${pf6_solibs}

.SECONDARY:

.PHONY: all install clean distclean FORCE

all: ${targets}

install: ${targets_install}
	@mkdir -p "${DESTDIR}${xtlibdir}";
	if test -n "${targets_install}"; then install -pm0755 $^ "${DESTDIR}${xtlibdir}/"; fi;

clean:
	rm -f *.o *.oo *.so *.a {matches,targets}[46].man initext4.c initext6.c;

distclean: clean
	rm -f .*.d .*.dd;

init%.o: init%.c
	${AM_VERBOSE_CC} ${CC} ${AM_DEPFLAGS} ${AM_CFLAGS} -D_INIT=$*_init ${CFLAGS} -o $@ -c $<;

-include .*.d


#
#	Shared libraries
#
lib%.so: lib%.oo
	${AM_VERBOSE_CCLD} ${CCLD} ${AM_LDFLAGS} -shared ${LDFLAGS} -o $@ $<;

lib%.oo: ${srcdir}/lib%.c
	${AM_VERBOSE_CC} ${CC} ${AM_DEPFLAGS} ${AM_CFLAGS} -D_INIT=lib$*_init -DPIC -fPIC ${CFLAGS} -o $@ -c $<;


#
#	Static bits
#
#	If static building is disabled, libext*.a will still be generated,
#	but will be empty. This is good since we can do with less case
#	handling code in the Makefiles.
#
lib%.o: ${srcdir}/lib%.c
	${AM_VERBOSE_CC} ${CC} ${AM_DEPFLAGS} ${AM_CFLAGS} -DNO_SHARED_LIBS=1 -D_INIT=lib$*_init ${CFLAGS} -o $@ -c $<;

libext4.a: initext4.o ${libext4_objs}
	${AM_VERBOSE_AR} ${AR} crs $@ $^;

libext6.a: initext6.o ${libext6_objs}
	${AM_VERBOSE_AR} ${AR} crs $@ $^;

initext_func  := $(addprefix xt_,${pfx_build_mod}) $(addprefix ipt_,${pf4_build_mod})
initext6_func := $(addprefix xt_,${pfx_build_mod}) $(addprefix ip6t_,${pf6_build_mod})

.initext4.dd: FORCE
	@echo "${initext_func}" >$@.tmp; \
	cmp -s $@ $@.tmp || mv $@.tmp $@; \
	rm -f $@.tmp;

.initext6.dd: FORCE
	@echo "${initext6_func}" >$@.tmp; \
	cmp -s $@ $@.tmp || mv $@.tmp $@; \
	rm -f $@.tmp;

initext4.c: .initext4.dd
	${AM_VERBOSE_GEN}
	@( \
	echo "" >$@; \
	for i in ${initext_func}; do \
		echo "extern void lib$${i}_init(void);" >>$@; \
	done; \
	echo "void init_extensions(void);" >>$@; \
	echo "void init_extensions(void)" >>$@; \
	echo "{" >>$@; \
	for i in ${initext_func}; do \
		echo  " ""lib$${i}_init();" >>$@; \
	done; \
	echo "}" >>$@; \
	);

initext6.c: .initext6.dd
	${AM_VERBOSE_GEN}
	@( \
	echo "" >$@; \
	for i in ${initext6_func}; do \
		echo "extern void lib$${i}_init(void);" >>$@; \
	done; \
	echo "void init_extensions(void);" >>$@; \
	echo "void init_extensions(void)" >>$@; \
	echo "{" >>$@; \
	for i in ${initext6_func}; do \
		echo " ""lib$${i}_init();" >>$@; \
	done; \
	echo "}" >>$@; \
	);

#
#	Manual pages
#
ex_matches = $(sort $(shell echo $(1) | grep -Eo '\b[a-z0-9]+\b'))
ex_targets = $(sort $(shell echo $(1) | grep -Eo '\b[A-Z0-9]+\b'))
man_run    = \
	${AM_VERBOSE_GEN} \
	for ext in $(1); do \
		f="${srcdir}/libxt_$$ext.man"; \
		cf="${srcdir}/libxt_$$ext.c"; \
		if [ -f "$$f" ] && grep -Eq "$(3)|NFPROTO_UNSPEC" "$$cf"; then \
			echo -e "\t+ $$f" >&2; \
			echo ".SS $$ext"; \
			cat "$$f"; \
			continue; \
		fi; \
		f="${srcdir}/lib$(2)t_$$ext.man"; \
		if [ -f "$$f" ]; then \
			echo -e "\t+ $$f" >&2; \
			echo ".SS $$ext"; \
			cat "$$f"; \
			continue; \
		fi; \
	done >$@;

matches4.man: .initext4.dd $(wildcard ${srcdir}/lib*.man)
	$(call man_run,$(call ex_matches,${pfx_build_mod} ${pf4_build_mod}),ip,NFPROTO_IPV4)

matches6.man: .initext6.dd $(wildcard ${srcdir}/lib*.man)
	$(call man_run,$(call ex_matches,${pfx_build_mod} ${pf6_build_mod}),ip6,NFPROTO_IPV6)

targets4.man: .initext4.dd $(wildcard ${srcdir}/lib*.man)
	$(call man_run,$(call ex_targets,${pfx_build_mod} ${pf4_build_mod}),ip,NFPROTO_IPV4)

targets6.man: .initext6.dd $(wildcard ${srcdir}/lib*.man)
	$(call man_run,$(call ex_targets,${pfx_build_mod} ${pf6_build_mod}),ip6,NFPROTO_IPV6)
