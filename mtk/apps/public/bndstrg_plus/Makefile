#NVDIR = $(ROOTDIR)/lib/libnvram
#LIBNVRAM = $(ROOTDIR)/lib/lib/libnvram-0.9.28.so
LDFLAGS += -lpthread #-lnvram
BNDSTRG_EXEC = bndstrg2
#CONF_H	= $(ROOTDIR)/$(LINUXDIR)/include/linux/autoconf.h
#UCONF_H	= $(ROOTDIR)/config/autoconf.h
#PLATFORM = x86
PLATFORM = mipsel

ifeq ($(PLATFORM),x86)
CROSS_COMPILE=""
endif

ifeq ($(PLATFORM),mipsel)
CROSS_COMPILE = mips-linux-
endif

CFLAGS += -O2 -Wall -Werror -g -lrt #-I$(NVDIR)
#CFLAGS += -DVENDOR_IOS_SUPPORT
CFLAGS += -DWPS_SUPPORT
#CFLAGS += -DLIMIT_5G_NUM_SUPPORT
ifneq  ($(TCSUPPORT_WLAN_BNDSTRG_CUSTOM2),)
CFLAGS += -DTCSUPPORT_WLAN_BNDSTRG_CUSTOM2
CFLAGS += -DTCSUPPORT_WLAN_BNDSTRG_CUSTOM1
CFLAGS += -DVENDOR_IOS_SUPPORT
CFLAGS += -DWPS_SUPPORT
CFLAGS += -DLIMIT_5G_NUM_SUPPORT
endif
ifneq  ($(TCSUPPORT_WLAN_BNDSTRG_CUSTOM1),)
CFLAGS += -DTCSUPPORT_WLAN_BNDSTRG_CUSTOM1
CFLAGS += -DVENDOR_IOS_SUPPORT
CFLAGS += -DWPS_SUPPORT
CFLAGS += -DLIMIT_5G_NUM_SUPPORT
endif
ifneq  ($(TCSUPPORT_WLAN_BNDSTRG_JOYME3),)
CFLAGS += -DTCSUPPORT_WLAN_BNDSTRG_JOYME3
endif
#CFLAGS += -DBNDSTRG_NVRAM_SUPPORT
#CFLAGS += -DVENDOR_FEATURE7_SUPPORT
#CFLAGS += -DNVRAM_LIB_SUPPORT
#CFLAGS += -DBTM_DEBUG_SUPPORT
#CC = $(CROSS_COMPILE)gcc
TCAPILIB=-L ../lib -ltcapi

BNDSTRG_OBJS = driver_wext.o eloop.o os_internal.o main.o util.o debug.o netlink.o
#ctrl_iface_unix.o
BNDSTRG_OBJS += bndstrg.o

all: $(BNDSTRG_EXEC)

$(BNDSTRG_EXEC): $(BNDSTRG_OBJS)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) -o $(BNDSTRG_EXEC) $(BNDSTRG_OBJS)	$(TCAPILIB)

bndstrg.o: bndstrg.c $(CONF_H) $(UCONF_H) 
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $@ -c bndstrg.c

romfs_dir = $(ROOTDIR)/romfs

romfs_dirs = etc_ro/wifi
   
romfs:
	[ -d $(romfs_dir)/$$i ] || mkdir -p $(romfs_dir)
	for i in $(romfs_dirs); do \
		[ -d $(romfs_dir)/$$i ] || mkdir -p $(romfs_dir)/$$i; \
	done 
	$(ROMFSINST) /bin/$(BNDSTRG_EXEC)
	$(ROMFSINST) /etc_ro/bndstrg.conf

clean:
	rm -f *.o $(BNDSTRG_EXEC)
