export CC = $(TOOLPREFIX)gcc
export LD = $(TOOLPREFIX)ld
export AR = $(TOOLPREFIX)ar
export STRIP = $(TOOLPREFIX)strip

export APP_CMM_DIR = $(PRIVATE_APPS_PATH)/user
export APP_PJSIP_DIR = $(PUBLIC_APPS_PATH)/pjsip_1.10
export APP_VOIP_DIR = $(PRIVATE_APPS_PATH)/voip

VENDOR_CFLAGS := -DLINUX

ifeq ($(strip $(SUPPLIER)),broadcom)
include $(KERNELPATH)/$(MODEL)_config

SUPPLIER_VOIP_DIR := $(BUILD_DIR)/userspace/private/apps/vodsl
DSP_DYNAMIC_TARGET := $(SUPPLIER_VOIP_DIR)/bos
DSP_DYNAMIC_LIB := -L$(SUPPLIER_VOIP_DIR)/bos -lbos
DSP_DYNAMIC_LIB += -L$(INSTALL_DIR)/lib/private -liqctl

DSP_CFLAGS := -O2 -Wall -Werror -march=mips32 -DBOS_OS_LINUXUSER -fomit-frame-pointer -fno-strict-aliasing -mabi=32 -G 0 -msoft-float -pipe -Wa,-mips32 -DBRCM_IQCTL
DSP_INCLUDE += -I$(BUILD_DIR)/userspace/private/include
DSP_INCLUDE += -I$(SUPPLIER_VOIP_DIR)/voip/inc
DSP_INCLUDE += -I$(BUILD_DIR)/bcmdrivers/broadcom/include/bcm963xx
DSP_INCLUDE += -I$(BUILD_DIR)/bcmdrivers/opensource/include/bcm963xx
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/voice_res_gw/inc
DSP_INCLUDE += -D__EXPORTED_HEADERS__ -I$(KERNELPATH)
DSP_INCLUDE += -I$(BUILD_DIR)/shared/opensource/include/bcm963xx
DSP_INCLUDE += -I$(BUILD_DIR)/shared/broadcom/include/bcm963xx
DSP_INCLUDE += -I$(BUILD_DIR)/bcmdrivers/broadcom/char/endpoint/bcm9$(BRCM_CHIP)/inc 
DSP_INCLUDE += -I$(BUILD_DIR)/bcmdrivers/broadcom/char/endpoint/bcm9$(BRCM_CHIP)/inc/cfg
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx/src/mta/inc/cfg
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx/apps/Bcm$(BRCM_CHIP)uni/cfginc/mta
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx/apps/Bcm$(BRCM_CHIP)uni/cfginc/xchg_common
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx/apps/Bcm$(BRCM_CHIP)uni/cfginc/xdrv
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx/apps/Bcm$(BRCM_CHIP)uni/cfginc/vrg
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx/apps/cfginc
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx/apps/cfginc/vodsl
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx/apps/cfginc/mta
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx/apps/cfginc/default
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx/apps/cfginc/xchg_common
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx/apps/cfginc/xdrv
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx/apps/cfginc/vrg
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/xchg_common/bos/publicInc 
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/xchg_common/assert/cfginc 
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/xchg_common/assert/inc 
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/xchg_common/str 
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/voice_res_gw/inc 
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/voice_res_gw/casCtl/inc 
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/voice_res_gw/endpt/inc 
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/voice_res_gw/endpt/vrg/cfginc 
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/voice_res_gw/endpt/vrg/inc 
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/voice_res_gw/codec 
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/voice_res_gw/hdsp/inc 
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/voice_res_gw/hdsp/cfginc 
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/voice_res_gw/classStm
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/xchg_common/sme
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/xchg_drivers/inc 
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/xchg_drivers/arch/mips/inc 
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/xchg_drivers/bcm63268/chipinc 
DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/ldx_hausware/hausware/inc 
#DSP_INCLUDE += -I$(SUPPLIER_VOIP_DIR)/voip/vodslMain
DSP_INCLUDE += -I$(SUPPLIER_VOIP_DIR)/voip/inc
#DSP_INCLUDE += -I$(BUILD_DIR)/xChange/dslx_common/prot_cctk/apps/inc

DSP_OBJ := $(SUPPLIER_VOIP_DIR)/endpoint/endpoint_user.o
DSP_OBJ += $(SUPPLIER_VOIP_DIR)/voip/util/log/cmgrLog.o
DSP_OBJ += $(SUPPLIER_VOIP_DIR)/voip/util/log/dispLog.o
DSP_OBJ += $(SUPPLIER_VOIP_DIR)/voip/util/log/vodslLog.o
DSP_OBJ += $(SUPPLIER_VOIP_DIR)/voip/rtp/rtpapi.o
DSP_OBJ += $(SUPPLIER_VOIP_DIR)/voip/rtp/rtcp.o
DSP_OBJ += $(SUPPLIER_VOIP_DIR)/voip/rtp/udp.o

VENDOR_CFLAGS += -DVOIP_BROADCOM

endif  # ifeq ($(strip $(SUPPLIER)),broadcom)

ifneq ($(strip $(CONFIG_NR_CPUS)), )
VENDOR_CFLAGS += -DCONFIG_NR_CPUS=$(CONFIG_NR_CPUS)
endif # CONFIG_NR_CPUS

export DSP_DYNAMIC_TARGET
export DSP_DYNAMIC_LIB
export DSP_STATIC_LIB
export DSP_OBJ
export DSP_CFLAGS
export DSP_INCLUDE

ifneq ($(strip $(INCLUDE_VOIP)),)
export INCLUDE_VOIP
export SUPPLIER

# now configuring voip locale settings

ifneq ($(strip $(VOIP_LOCALE_ALL)),)
VOIP_LOCALE := -DVOIP_CFG_ALL
ifeq ($(strip $(SUPPLIER)),broadcom)
export BRCM_VRG_COUNTRY_ALL_PROFILES=y
endif
else
VOIP_LOCALE := $(shell cat config/$(MODEL).config | sed -n 's/=y$$//p' | sed -n 's/^VOIP_CFG/-D&/p')
ifeq ($(strip $(SUPPLIER)),broadcom)
VENDOR_CFLAGS += -DBRCM_VRG_COUNTRY_CFG_CUSTOM_PROFILES
VENDOR_CFLAGS += $(shell cat config/$(MODEL).config | sed -n 's/=y$$//p' | sed -n 's/^VOIP_CFG/-DBRCM_VRG_COUNTRY_CFG/p')
export BRCM_VRG_COUNTRY_ALL_PROFILES=n
# for customized locale, it is defined as VOIP_CFG_xxx in the config
# to build modules, BCM source need BRCM_VRG_COUNTRY_CFG_xxx
$(warning *** You MUST export BRCM_VRG_COUNTRY_CFG_xxx to build voice modules ***)
endif
endif # VOIP_LOCALE_ALL

export VOIP_LOCALE += -I$(APP_VOIP_DIR)/inc

VOIP_DFLAGS := -DINCLUDE_VOIP -DINCLUDE_NEWUI
VOIP_CFLAGS := -DINCLUDE_VOIP $(VOIP_LOCALE)
VOIP_CFLAGS += -I$(APP_VOIP_DIR)/inc/client -I$(SUPPLIER_VOIP_DIR)/include
VOIP_CFLAGS += -I$(APP_PJSIP_DIR)/pjlib/include
VOIP_CFLAGS += -I$(APP_PJSIP_DIR)/cmsip/include
VOIP_CFLAGS += -I$(OS_LIB_PATH)/include -I$(TP_MODULES_PATH)/voip

ifeq ($(strip $(CONFIG_IP_MULTIPLE_TABLES)), y)
VOIP_CFLAGS += -DCONFIG_IP_MULTIPLE_TABLES
endif
ifeq ($(strip $(CONFIG_CPU_BIG_ENDIAN)), y)
VOIP_CFLAGS += -DCONFIG_CPU_BIG_ENDIAN
endif

ifeq ($(strip $(INCLUDE_DSP_SOCKET_OPEN)), y)
VOIP_CFLAGS += -DINCLUDE_DSP_SOCKET_OPEN
export INCLUDE_DSP_SOCKET_OPEN
endif

export NUM_FXS_CHANNELS
VOIP_DFLAGS += -DINCLUDE_FXS_NUM=$(NUM_FXS_CHANNELS)
VOIP_CFLAGS += -DNUM_FXS_CHANNELS=$(NUM_FXS_CHANNELS)
ifeq ($(strip $(INCLUDE_CPU_AR9344)),y)
export CHANNEL=$(NUM_FXS_CHANNELS)
endif

ifneq ($(strip $(INCLUDE_DMZ)),)
VOIP_CFLAGS += -DINCLUDE_DMZ
endif

ifneq ($(strip $(INCLUDE_EMERGENCY_CALL)),)
VOIP_DFLAGS += -DINCLUDE_EMERGENCY_CALL
endif

ifneq ($(strip $(INCLUDE_P2P_CALL)),)
VOIP_DFLAGS += -DINCLUDE_P2P_CALL
endif

ifneq ($(strip $(INCLUDE_DIGITMAP)),)
export INCLUDE_DIGITMAP=y
VOIP_DFLAGS += -DINCLUDE_DIGITMAP
endif

ifneq ($(strip $(INCLUDE_USB_VOICEMAIL)),)
export INCLUDE_USB_VOICEMAIL=y
VOIP_DFLAGS += -DINCLUDE_USB_VOICEMAIL

ifneq ($(strip $(INCLUDE_USBVM_MODULE)),)
ifeq ($(strip $(CONFIG_HZ)), $(shell echo $(CONFIG_HZ) | awk '{v1=$$1/100*100; printf("%d", v1);}'))
export INCLUDE_USBVM_MODULE=y
VOIP_CFLAGS += -DINCLUDE_USBVM_MODULE
endif
endif

endif

ifneq ($(strip $(INCLUDE_CALLLOG)),)
export INCLUDE_CALLLOG=y
VOIP_DFLAGS += -DINCLUDE_CALLLOG
endif

ifneq ($(strip $(INCLUDE_PSTN)),)
NUM_FXO_CHANNELS = 1
export INCLUDE_PSTN = y
VOIP_DFLAGS += -DINCLUDE_PSTN
ifneq ($(strip $(INCLUDE_PSTN_LIFELINE)),)
export INCLUDE_PSTN_LIFELINE=y
VOIP_DFLAGS += -DINCLUDE_PSTN_LIFELINE
endif
ifneq ($(strip $(INCLUDE_PSTN_POLREV)),)
export INCLUDE_PSTN_POLREV=y
VOIP_DFLAGS += -DINCLUDE_PSTN_POLREV
endif
ifneq ($(strip $(INCLUDE_PSTN_GATEWAY)),)
export INCLUDE_PSTN_GATEWAY=y
VOIP_DFLAGS += -DINCLUDE_PSTN_GATEWAY
endif
else
NUM_FXO_CHANNELS = 0
endif  # INCLUDE_PSTN

export NUM_FXO_CHANNELS
VOIP_DFLAGS += -DINCLUDE_FXO_NUM=$(NUM_FXO_CHANNELS)
VOIP_CFLAGS += -DNUM_FXO_CHANNELS=$(NUM_FXO_CHANNELS)

export NUM_CHANNELS=$(shell echo $(NUM_FXS_CHANNELS) $(NUM_FXO_CHANNELS) | awk '{v1=$$1+$$2; printf("%d", v1);}')
VOIP_CFLAGS += -DNUM_CHANNELS=$(NUM_CHANNELS)

export VOIP_CFLAGS += $(VENDOR_CFLAGS)
export VOIP_DFLAGS

endif  # INCLUDE_VOIP

