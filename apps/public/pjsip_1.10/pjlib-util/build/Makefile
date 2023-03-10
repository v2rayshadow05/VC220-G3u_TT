# By default, the test application includes main.o.
# OS make file may override this with os-specific files
#export UTIL_TEST_OBJS = main.o

include ../../build.mak
include $(PJDIR)/build/common.mak

RULES_MAK := $(PJDIR)/build/rules.mak

PJLIB_LIB:=$(PJDIR)/pjlib/lib/libpj-$(TARGET_NAME)$(LIBEXT)
export PJLIB_UTIL_LIB:=../lib/libpjlib-util-$(TARGET_NAME)$(LIBEXT)

###############################################################################
# Gather all flags.
#
export _CFLAGS 	:= $(CC_CFLAGS) $(OS_CFLAGS) $(HOST_CFLAGS) $(M_CFLAGS) \
		   $(CFLAGS) $(CC_INC)../include $(CC_INC)../../pjlib/include
export _CXXFLAGS:= $(_CFLAGS) $(CC_CXXFLAGS) $(OS_CXXFLAGS) $(M_CXXFLAGS) \
		   $(HOST_CXXFLAGS) $(CXXFLAGS)
export _LDFLAGS := $(subst /,$(HOST_PSEP),$(PJLIB_UTIL_LIB)) \
		   $(subst /,$(HOST_PSEP),$(PJLIB_LIB)) \
		   $(CC_LDFLAGS) $(OS_LDFLAGS) $(M_LDFLAGS) $(HOST_LDFLAGS) \
		   $(LDFLAGS) 

###############################################################################
# Defines for building PJLIB-UTIL library
#
export PJLIB_UTIL_SRCDIR = ../src/pjlib-util
export PJLIB_UTIL_OBJS += $(OS_OBJS) $(M_OBJS) $(CC_OBJS) $(HOST_OBJS) \
		base64.o crc32.o errno.o dns.o dns_dump.o dns_server.o \
		getopt.o hmac_md5.o hmac_sha1.o http_client.o md5.o pcap.o resolver.o \
		scanner.o sha1.o srv_resolver.o string.o stun_simple.o \
		stun_simple_client.o xml.o firewall_cfg.o
export PJLIB_UTIL_CFLAGS += $(_CFLAGS) $(APP_CFLAGS)

###############################################################################
# Defines for building test application
#
#export UTIL_TEST_SRCDIR = ../src/pjlib-util-test
#export UTIL_TEST_OBJS += xml.o encryption.o stun.o resolver_test.o test.o \
		http_client.o
#export UTIL_TEST_CFLAGS += $(_CFLAGS)
#export UTIL_TEST_LDFLAGS += $(_LDFLAGS)
#export UTIL_TEST_EXE:=../bin/pjlib-util-test-$(TARGET_NAME)$(HOST_EXE)

	
#export CC_OUT CC AR RANLIB HOST_MV HOST_RM HOST_RMDIR HOST_MKDIR OBJEXT LD LDOUT 
###############################################################################
# Main entry
#
# $(TARGET) is defined in os-$(OS_NAME).mak file in current directory.
#
#TARGETS := pjlib-util pjlib-util-test
TARGETS := pjlib-util

all: $(TARGETS)

doc:
	cd .. && rm -rf docs/html docs/latex && doxygen docs/doxygen.cfg
	@if [ -n "$(WWWDIR)" ]; then \
		echo "Copying to $(WWWDIR)/pjlib-util/docs/html.." ; \
		cp -a ../docs/html/* $(WWWDIR)/pjlib-util/docs/html/ ; \
	fi

dep: depend
distclean: realclean

#.PHONY: dep depend pjlib pjlib-test clean realclean distclean
.PHONY: dep depend pjlib clean realclean distclean

pjlib-util:
	$(MAKE) -f $(RULES_MAK) APP=PJLIB_UTIL app=pjlib-util $(PJLIB_UTIL_LIB)

#pjlib-util-test:
#	$(MAKE) -f $(RULES_MAK) APP=UTIL_TEST app=pjlib-util-test $(UTIL_TEST_EXE)

.PHONY: ../lib/pjlib-util.ko
../lib/pjlib-util.ko:
	echo Making $@
	$(MAKE) -f $(RULES_MAK) APP=PJLIB_UTIL app=pjlib-util $@

#.PHONY: ../lib/pjlib-util-test.ko
#../lib/pjlib-util-test.ko:
#	$(MAKE) -f $(RULES_MAK) APP=UTIL_TEST app=pjlib-util-test $@

clean:
	$(MAKE) -f $(RULES_MAK) APP=PJLIB_UTIL app=pjlib-util $@
#	$(MAKE) -f $(RULES_MAK) APP=UTIL_TEST app=pjlib-util-test $@

realclean:
	$(subst @@,$(subst /,$(HOST_PSEP),.pjlib-util-$(TARGET_NAME).depend),$(HOST_RMR))
#	$(subst @@,$(subst /,$(HOST_PSEP),.pjlib-util-test-$(TARGET_NAME).depend),$(HOST_RMR))
	$(MAKE) -f $(RULES_MAK) APP=PJLIB_UTIL app=pjlib-util $@
#	$(MAKE) -f $(RULES_MAK) APP=UTIL_TEST app=pjlib-util-test $@

depend:
	$(MAKE) -f $(RULES_MAK) APP=PJLIB_UTIL app=pjlib-util $@
#	$(MAKE) -f $(RULES_MAK) APP=UTIL_TEST app=pjlib-util-test $@
#	echo '$(UTIL_TEST_EXE): $(PJLIB_UTIL_LIB) $(PJLIB_LIB)' >> .pjlib-util-test-$(TARGET_NAME).depend; \


