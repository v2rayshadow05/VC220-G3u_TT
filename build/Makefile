#**************************************************************************** 
# 
#  Copyright(c) 2010-2011 Shenzhen TP-LINK Technologies Co.Ltd. 
#  All Rights Reserved 
#  YangXv <yangxu@tp-link.com.cn> 
# 
#***************************************************************************

# Top-level Makefile for all linux platforms 

LAST_MODEL_COOKIE := .last_model
LAST_MODEL        := $(strip $(shell cat $(LAST_MODEL_COOKIE) 2>/dev/null))
ifeq ($(strip $(MODEL)),)
MODEL=$(LAST_MODEL)
endif

ifeq ($(strip $(MODEL)),)
$(error "You have to define MODEL, such as VG3631V1")
endif

$(shell	echo "$(MODEL)" > $(LAST_MODEL_COOKIE))
export MODEL
#
# MODEL2 does not care Annex A or Annex B, both MODELs should use the same datamodel
MODEL2 = $(strip $(shell echo $(MODEL) | sed 's/\([0-9]\)BV/\1V/g'))

include config/$(MODEL).config

TP_CFLAGS = 

ifeq ($(strip $(INCLUDE_CPU_TC3182)),y)
CPU_TYPE=tc3182
endif

ifeq ($(strip $(INCLUDE_CPU_TC3162U)),y)
CPU_TYPE=tc3162u
endif

#zl added 2011-10-18
ifeq ($(strip $(INCLUDE_CPU_RTL8672)),y)
CPU_TYPE=rtl8672
endif

ifeq ($(strip $(INCLUDE_CPU_VR268)),y)
CPU_TYPE=vr268
include config/config.wifi
endif

ifeq ($(strip $(INCLUDE_CPU_RT63368)),y)
CPU_TYPE=rt63368
endif

ifeq ($(strip $(INCLUDE_CPU_RT63365)),y)
CPU_TYPE=rt63365
endif

ifeq ($(strip $(INCLUDE_CPU_88F6550)),y)
CPU_TYPE=88f6550
include config/config.wifi
endif

ifeq ($(strip $(INCLUDE_CPU_BCM96318)),y)
CPU_TYPE=bcm96318
BRCM_CHIP=6318
endif

ifeq ($(strip $(INCLUDE_CPU_BCM63168)),y)
CPU_TYPE=bcm63168
BRCM_CHIP=63268
endif

ifeq ($(strip $(INCLUDE_CPU_BCM63167)),y)
CPU_TYPE=bcm63167
BRCM_CHIP=63268
endif


ifeq ($(strip $(INCLUDE_CPU_BCM6361)),y)
CPU_TYPE=bcm6361
BRCM_CHIP=6362
endif

ifeq ($(strip $(INCLUDE_CPU_BCM63381)),y)
CPU_TYPE=bcm63381
BRCM_CHIP=63381
endif

#ifeq ($(strip $(INCLUDE_CPU_MT7513)),y)
CPU_TYPE=en7512
#endif

#define basic variable
TOP_PATH = $(shell pwd)/..
BUILD_PATH = $(TOP_PATH)/build
TOOLS_PATH = $(TOP_PATH)/host_tools
FAKEROOT_PATH = $(TOOLS_PATH)/fakeroot

#Temporarily
ifeq ($(strip $(INCLUDE_LANGUAGE_EN)),y)
	ifeq ($(strip $(INCLUDE_NEW_WEB_UI)),y)
		WEBFILE_PATH = $(TOP_PATH)/sysfiles/new_web/en
	else
		WEBFILE_PATH = $(TOP_PATH)/sysfiles/web/en
	endif
	CONFFILE_PATH = $(TOP_PATH)/sysfiles/config/en
endif

ifeq ($(strip $(INCLUDE_LANGUAGE_CN)),y)
	ifeq (($(strip $(INCLUDE_NEW_WEB_UI)),y))
		WEBFILE_PATH = $(TOP_PATH)/sysfiles/new_web/cn
	else
		WEBFILE_PATH = $(TOP_PATH)/sysfiles/web/cn
	endif
	CONFFILE_PATH = $(TOP_PATH)/sysfiles/config/cn
endif

TARGETS_PATH = $(TOP_PATH)/targets
PUBLIC_APPS_PATH = $(TOP_PATH)/apps/public
PRIVATE_APPS_PATH = $(TOP_PATH)/apps/private
OS_LIB_PATH = $(PUBLIC_APPS_PATH)/os_libs
TP_MODULES_PATH = $(TOP_PATH)/modules/private



####################################################
#
#Get supplier         
#
####################################################	
ifeq ($(strip $(CPU_TYPE)),88f6550)
#ChenZhu add 20130402 for bigendian is not supported
export BIGENDIAN = 0
SUPPLIER = environment_marvel88f6560
SUPPLIER_MARVELL = environment_marvel88f6560
export ARCH = arm
export KERNELARCH = arm
TOOLROOTPATH :=/opt/marvell/armv5-marvell-linux-uclibcgnueabi-soft_i686
export CROSS_COMPILE = $(TOOLROOTPATH)/bin/arm-marvell-linux-uclibcgnueabi-
export PATH := $(TOOLROOTPATH)/bin:$(PATH)
TOOLPREFIX = arm-marvell-linux-uclibcgnueabi-
HOST = arm-marvell-linux-uclibcgnueabi
KERNELVERSION = 2.6.32.11
OBJ_DIR = marvell_2.6.25-RC10
export KERNELPATH = $(TOP_PATH)/$(SUPPLIER)/SDK_2.6.25-RC10/Source/Kernel/linux_feroceon-KW2

AVANTA_HGU_CFG_DIR = $(TOP_PATH)/$(SUPPLIER)/SDK_2.6.25-RC10/Source/ConfigFiles/Products/AVANTA_HGU
AVANTA_HGU_INITD_DIR = $(AVANTA_HGU_CFG_DIR)/FileSystem/init.d

KERNEL_CFG_DIR = $(AVANTA_HGU_CFG_DIR)/Kernel

MARVELL_KERNEL_IMAGE_DIR = $(KERNELPATH)/arch/arm/boot
MARVELL_KERNEL_IMAGE_NAME = uImage

MARVELL_SQAUSH_FS_DIR = $(TOP_PATH)/$(SUPPLIER)/SDK_2.6.25-RC10/Source
MARVELL_SQAUSH_FS_NAME = rootfs.squashfs
MARVELL_JFFS2_FS_NAME = jffs2.image

MARVELL_IMAGE_DIR = $(TOP_PATH)/$(SUPPLIER)/SDK_2.6.25-RC10/Output/Products/AVANTA_HGU/RecoveryImage
MARVELL_IMAGE_NAME = rootfs.squashfs.rcvr.img

export MAKEARCH = $(MAKE) ARCH=$(KERNELARCH) CROSS_COMPILE=$(TOOLPREFIX)

export HAL = $(TOP_PATH)/$(SUPPLIER)/modules/private/wlan/hal
export TARGETARCH = xscale-le-elf
export MODULEPATH = $(TARGETS_PATH)/fs.$(MODEL)/lib/modules
WIRELESSTOOLS = wireless_tools.29
WIFI_TOOL_TG_PATH = $(TARGETS_PATH)/$(SUPPLIER)_$(CPU_TYPE)_wifi_tool
WIFI_MODULE_TG_PATH = $(TARGETS_PATH)/$(SUPPLIER)_$(CPU_TYPE)_wifi_modules
INSTALL_WIRELESS = $(WIFI_TOOL_TG_PATH)
FS_WIRELESS_MODULE = $(MODULEPATH)
FS_WIRELESS_TOOLS = $(TARGETS_PATH)/fs.$(MODEL)/usr
WIRELESSTOOLSLIB = iwlib.so

export WIFI_MODULE_TG_PATH

#export ATH_CARR_DIR = marvel
export PLATFORM = $(TOP_PATH)/environment_marvel88f6560/modules/private/wlan
#export PLATFORM_INC = $(PLATFORM)/carrier/$(ATH_CARR_DIR)/include
export ATHEROSPATH=$(PLATFORM)

export MARVELL_SW_DIR = $(TOP_PATH)/$(SUPPLIER)/SDK_2.6.25-RC10/Source/Application/SW_TREE_PUB
#export MARVELL_SW_LIBS_INC = -I$(MARVELL_SW_DIR)/mng/i2c/appl/i2c_app/inc
export MARVELL_SW_LIBS_INC = -I$(MARVELL_SW_DIR)/mng/omci/core/h	#Added by YeZuopou @ 01Apr13
export MARVELL_SW_LIBS_LIB = -L$(MARVELL_SW_DIR)/mipc/obj -lmipc \
							-L$(MARVELL_SW_DIR)/mipc/api_defs/omci -lomci_mipc_client \
							-lm 
endif

ifeq ($(strip $(CPU_TYPE)),tc3182)
TOOLROOTPATH :=/opt/trendchip/mips-linux-uclibc/usr
export PATH :=$(TOOLROOTPATH)/bin:$(PATH)
SUPPLIER = ralink
TOOLPREFIX = mips-linux-uclibc-
HOST = mips-linux-uclibc
KERNELVERSION = 2.6.22.15
OBJ_DIR = ralink_4.3.4
SLIB_PATH = $(TOP_PATH)/$(SUPPLIER)/Project/lib/mips-linux-uclibc/
KERNELPATH = $(TOP_PATH)/$(SUPPLIER)/linux
ROOTFSTOOLS = $(SUPPLIER_TOOLS)/mksquashfs-lzma

ifeq ($(strip $(SUPPLIER)),ralink)

unexport TC_CFLAGS
ifeq ($(INCLUDE_VOIP), y)
TC_CFLAGS += $(VOIP_LOCALE)
ifeq ($(INCLUDE_DSP_SOCKET_OPEN), y)
TC_CFLAGS += -DINCLUDE_DSP_SOCKET_OPEN
endif 
endif

ifeq ($(strip $(TCSUPPORT_SDRAM_32M)),y)
TC_CFLAGS += -DTCSUPPORT_SDRAM_32M
endif

ifeq ($(INCLUDE_WATCHDOG), y) 
TC_CFLAGS += -DTCSUPPORT_WATCHDOG
endif

ifeq ($(strip $(TCSUPPORT_BRIDGE_FASTPATH)),y)
TC_CFLAGS += -DTCSUPPORT_BRIDGE_FASTPATH
endif
export TC_CFLAGS
endif

endif

ifeq ($(strip $(CPU_TYPE)),tc3162u)
export PATH :=/opt/trendchip/mips-linux-3.4.6/bin:$(PATH)
SUPPLIER = ralink
TOOLPREFIX = mips-linux-
HOST = mips-linux
KERNELVERSION = 2.6.22.15
OBJ_DIR = ralink_3.4.6
SLIB_PATH = $(TOP_PATH)/$(SUPPLIER)/Project/lib/mips-linux/
KERNELPATH = $(TOP_PATH)/$(SUPPLIER)/linux
ROOTFSTOOLS = $(SUPPLIER_TOOLS)/mksquashfs-lzma
endif

ifeq ($(strip $(CPU_TYPE)),rtl8672)
export PATH :=/opt/rtl/rsdk-1.3.6-5181-EB-2.6.30-0.9.30/bin:$(PATH)
SUPPLIER = realtek
TOOLPREFIX = rsdk-linux-
HOST = rsdk-linux
KERNELVERSION = 2.6.30.9
OBJ_DIR = realtek_1.3.6
SLIB_PATH = $(TOP_PATH)/$(SUPPLIER)/vendors/rsdk-linux/
KERNELPATH = $(TOP_PATH)/$(SUPPLIER)/linux-2.6.x
ROOTFSTOOLS = $(KERNELPATH)/mksquashfs
#For lzma when make rootfs
export PATH := $(KERNELPATH)/rtkload/:$(PATH)
endif

ifeq ($(strip $(CPU_TYPE)),vr268)
SUPPLIER = lantiq
TOOLPREFIX = mips-linux-uclibc-
HOST = mips-linux
KERNELVERSION = 2.6.32.32
OBJ_DIR = lantiq_4.3.3
SLIB_PATH = $(TOP_PATH)/$(SUPPLIER)/build_dir/target-mips_r2_uClibc-0.9.30.1_2_6_32_LANTIQ/root-ltqcpe/orilib
KERNELPATH = $(TOP_PATH)/$(SUPPLIER)/build_dir/linux-ltqcpe_2_6_32_LANTIQ/linux-2.6.32.32
ROOTFSTOOLS = $(TOP_PATH)/$(SUPPLIER)/staging_dir/host/bin/mksquashfs4
TOOLCHAIN_DIR = $(TOP_PATH)/$(SUPPLIER)/staging_dir/toolchain-mips_r2_gcc-4.3.3+cs_uClibc-0.9.30.1_2_6_32
export PATH :=$(TOP_PATH)/$(SUPPLIER)/staging_dir/toolchain-mips_r2_gcc-4.3.3+cs_uClibc-0.9.30.1_2_6_32/usr/bin:$(PATH)
export PATH :=$(TOP_PATH)/$(SUPPLIER)/staging_dir/host/bin:$(PATH)
export HAL = $(TOP_PATH)/$(SUPPLIER)/modules/private/wlan/hal
export KERNELARCH = mips
export MAKEARCH = $(MAKE) ARCH=$(KERNELARCH) CROSS_COMPILE=$(TOOLPREFIX)
export ATH_CARR_DIR = lantiq
export PLATFORM = $(TOP_PATH)/lantiq/modules/private/wlan
export PLATFORM_INC = $(PLATFORM)/carrier/$(ATH_CARR_DIR)/include
export WAVE300_DIR = $(TOP_PATH)/lantiq/modules/wave300
export INSTALL_WIRELESS = $(TOP_PATH)/$(SUPPLIER)/wireless
export MODULEPATH = $(INSTALL_WIRELESS)/lib/modules/
export ATHEROSPATH=$(PLATFORM)
endif

ifeq ($(strip $(CPU_TYPE)),rt63368)
TOOLROOTPATH :=/opt/trendchip/mips-linux-uclibc/usr
export PATH :=$(TOOLROOTPATH)/bin:$(PATH)
SUPPLIER = mtk
TOOLPREFIX = mips-linux-uclibc-
HOST = mips-linux-uclibc
KERNELVERSION = 2.6.36
OBJ_DIR = mtk_4.3.4
SLIB_PATH = $(TOP_PATH)/$(SUPPLIER)/Project/lib/mips-linux-uclibc/
KERNELPATH = $(TOP_PATH)/$(SUPPLIER)/linux-2.6.36
ROOTFSTOOLS = $(SUPPLIER_TOOLS)/mksquashfs4

unexport TC_CFLAGS

ifeq ($(INCLUDE_VOIP), y)
TC_CFLAGS += -DTCSUPPORT_VOIP $(VOIP_LOCALE)
ifeq ($(INCLUDE_DSP_SOCKET_OPEN), y)
TC_CFLAGS += -DINCLUDE_DSP_SOCKET_OPEN
endif 
endif

ifeq ($(INCLUDE_WATCHDOG), y) 
TC_CFLAGS += -DTCSUPPORT_WATCHDOG
endif

export TC_CFLAGS
export KERNELVERSION

endif

ifeq ($(strip $(CPU_TYPE)),rt63365)
TOOLROOTPATH :=/opt/trendchip/mips-linux-uclibc/usr
export PATH :=$(TOOLROOTPATH)/bin:$(PATH)
SUPPLIER = mtk
TOOLPREFIX = mips-linux-uclibc-
HOST = mips-linux-uclibc
KERNELVERSION = 2.6.36
OBJ_DIR = mtk_4.3.4
SLIB_PATH = $(TOP_PATH)/$(SUPPLIER)/Project/lib/mips-linux-uclibc/
KERNELPATH = $(TOP_PATH)/$(SUPPLIER)/linux-2.6.36
ROOTFSTOOLS = $(SUPPLIER_TOOLS)/mksquashfs4


unexport TC_CFLAGS
TC_CFLAGS += -DTCSUPPORT_IGMPSNOOPING_ENHANCE
export TC_CFLAGS

export KERNELVERSION

endif


ifeq ($(strip $(CPU_TYPE)), en7512)
TOOLROOTPATH :=/opt/trendchip/mips-linux-uclibc-4.3.6-v2/usr
export PATH :=$(TOOLROOTPATH)/bin:$(PATH)
SUPPLIER = mtk
TOOLPREFIX = mips-unknown-linux-uclibc-
#HOST = mips-linux-uclibc
HOST = mips-linux
CROSS_COMPILE = mips-unknown-linux-uclibc-
export KERNELARCH = mips
KERNELVERSION = 2.6.36
ifeq ($(INCLUDE_ISP_M2), y)
OBJ_DIR = pvw422t1200_isp_m2
else
OBJ_DIR = mtk_4.3.4
endif
SLIB_PATH = $(TOP_PATH)/$(SUPPLIER)/Project/lib/mips-unknown-linux-uclibc/
KERNELPATH = $(TOP_PATH)/$(SUPPLIER)/linux-2.6.36
ROOTFSTOOLS = $(SUPPLIER_TOOLS)/mksquashfs4


unexport TC_CFLAGS
TC_CFLAGS += -DTCSUPPORT_IGMPSNOOPING_ENHANCE
TC_CFLAGS += -DTCSUPPORT_CPU_EN7512
export TC_CFLAGS

export KERNELVERSION

PROFILE_PATH = $(TOP_PATH)/$(SUPPLIER)/Project/profile/RT/$(CPU_TYPE)_demo/
#copy model profile as en7512_demo.profile
COPY_PROFILE := $(shell cp -f $(PROFILE_PATH)/$(MODEL).profile $(PROFILE_PATH)/$(CPU_TYPE)_demo.profile)
export DMT_PATH = $(TOP_PATH)/$(SUPPLIER)/modules/private/dmt/7512
export SYSROOT = /opt/trendchip/mips-linux-uclibc-4.3.6-v2/usr/mips-unknown-linux-uclibc/sysroot

#add by Zhao Mengqing, 2016-8-3, when build kernel, we need these path
MODULES_RA_HWNAT_7510_DIR = $(TOP_PATH)/$(SUPPLIER)/modules/private/ra_hwnat_7510
GLOBAL_INC_DIR = $(TOP_PATH)/$(SUPPLIER)/global_inc
#big endian
CMM_TARGET_CPU_ENDIAN=0   
endif



ifeq ($(shell cat config/$(MODEL).config | sed -n 's/\(^INCLUDE_CPU_BCM\)[0-9]\{4,5\}=y/\1/p'), INCLUDE_CPU_BCM)
BCM_CFLAGS := -DCONFIG_TP_IMAGE -DCONFIG_BCM9$(BRCM_CHIP) -DCHIP_$(BRCM_CHIP)
ifeq ($(INCLUDE_VOIP), y)
SUFFIX_V=V
else
SUFFIX_V=
endif

ifeq ($(strip $(CPU_TYPE)),bcm96318)
export PROFILE=96318GW
endif
ifeq ($(strip $(CPU_TYPE)),bcm63168)
export PROFILE=963268GW$(SUFFIX_V)
endif
ifeq ($(strip $(CPU_TYPE)),bcm63167)
export PROFILE=963268GW$(SUFFIX_V)
BCM_CFLAGS += -DCONFIG_VR600_TT_V1_BCM63167
endif
ifeq ($(strip $(CPU_TYPE)),bcm6361)
export PROFILE=96362GW$(SUFFIX_V)
endif
ifeq ($(strip $(CPU_TYPE)),bcm63381)
export PROFILE=963381GW$(SUFFIX_V)
endif
SUPPLIER = broadcom
HOST = mips-linux
OBJ_DIR = $(MODEL)
ifeq ($(strip $(INCLUDE_LINUX_VERSION_3)),y)
export BUILD_DIR = $(TOP_PATH)/4.16L.03
export INCLUDE_LINUX_VERSION_3
TOOLPREFIX = mips-unknown-linux-uclibc-
KERNELVERSION = 3.4.11-rt19
SLIB_PATH = /opt/toolchains/crosstools-mips-gcc-4.6-linux-3.4-uclibc-0.9.32-binutils-2.21/lib
KERNELPATH = $(BUILD_DIR)/kernel/linux-3.4rt
export PATH := /opt/toolchains/crosstools-mips-gcc-4.6-linux-3.4-uclibc-0.9.32-binutils-2.21/usr/bin:$(PATH)
else
export BUILD_DIR = $(TOP_PATH)/broadcom
export INCLUDE_LINUX_VERSION_2
TOOLPREFIX = mips-linux-uclibc-
KERNELVERSION = 2.6.30
SLIB_PATH = $(BUILD_DIR)/toolchain/lib/
KERNELPATH = $(BUILD_DIR)/kernel/linux
export PATH :=/opt/toolchains/uclibc-crosstools-gcc-4.4.2-1/usr/bin:$(PATH)
endif
export PATH:= $(BUILD_DIR)/hostTools/:$(PATH)
export INSTALL_WIRELESS = $(BUILD_DIR)/wireless
ifneq ($(MAKECMDGOALS), menuconfig)
ifneq ($(strip $(shell test -f $(BUILD_DIR)/tp/$(PROFILE)_$(MODEL) && echo "yes" || echo "no")), yes)
$(error '$(BUILD_DIR)/tp/$(PROFILE)_$(MODEL)'  not found)
endif
$(shell cp $(BUILD_DIR)/tp/$(PROFILE)_$(MODEL) $(BUILD_DIR)/targets/$(PROFILE)/$(PROFILE))
# deal with Annex A or Annex B
LASTANNEX_COOKIE := $(PRIVATE_APPS_PATH)/.lastannex
LASTANNEX        := $(strip $(shell cat $(LASTANNEX_COOKIE) 2>/dev/null))
ifeq ($(INCLUDE_ANNEXB), y)
NEWANNEX         := B
else
NEWANNEX         := A
endif
ifneq ($(LASTANNEX),)
ifneq ($(LASTANNEX),$(NEWANNEX))
ifneq ($(NEWANNEX),A)
$(shell find $(BUILD_DIR)/bcmdrivers/broadcom/char/adsl/impl1/adslcore$(BRCM_CHIP) -name "AdslPhyBld"|xargs rm -f)
$(shell sed -i 's/BRCM_ADSL_ANNEX_A=y/# BRCM_ADSL_ANNEX_A is not set/g' $(BUILD_DIR)/targets/$(PROFILE)/$(PROFILE))
$(shell sed -i 's/# BRCM_ADSL_ANNEX_B is not set/BRCM_ADSL_ANNEX_B=y/g' $(BUILD_DIR)/targets/$(PROFILE)/$(PROFILE))
$(shell sed -i 's/BRCM_ADSL_STANDARD=ANNEX_A/BRCM_ADSL_STANDARD=ANNEX_B/g' $(BUILD_DIR)/targets/$(PROFILE)/$(PROFILE))
$(shell sed -i 's/CPE_ANNEX_A=y/CPE_ANNEX_B=y/g' $(BUILD_DIR)/targets/$(PROFILE)/$(PROFILE))
else
$(shell find $(BUILD_DIR)/bcmdrivers/broadcom/char/adsl/impl1/adslcore$(BRCM_CHIP)B -name "AdslPhyBld"|xargs rm -f)
$(shell sed -i 's/# BRCM_ADSL_ANNEX_A is not set/BRCM_ADSL_ANNEX_A=y/g' $(BUILD_DIR)/targets/$(PROFILE)/$(PROFILE))
$(shell sed -i 's/BRCM_ADSL_ANNEX_B=y/# BRCM_ADSL_ANNEX_B is not set/g' $(BUILD_DIR)/targets/$(PROFILE)/$(PROFILE))
$(shell sed -i 's/BRCM_ADSL_STANDARD=ANNEX_B/BRCM_ADSL_STANDARD=ANNEX_A/g' $(BUILD_DIR)/targets/$(PROFILE)/$(PROFILE))
$(shell sed -i 's/CPE_ANNEX_B=y/CPE_ANNEX_A=y/g' $(BUILD_DIR)/targets/$(PROFILE)/$(PROFILE))
endif # ifneq ($(NEWANNEX),A)
endif # ifneq ($(LASTANNEX),$(NEWANNEX))
endif # ifneq ($(LASTANNEX),)
$(shell echo "$(NEWANNEX)" > $(LASTANNEX_COOKIE))

include $(BUILD_DIR)/make_tp.common
ifeq ($(strip $(BRCM_KERNEL_ROOTFS)),squashfs)
ROOTFSTOOLS = $(BUILD_DIR)/hostTools/mksquashfs
else
ROOTFSTOOLS = $(BUILD_DIR)/hostTools/mkfs.jffs2
endif
# VDSL flags have to be turned on for VDSL and Multimode phy
ifneq ($(strip $(BUILD_PHY_VDSL)),)
BCM_CFLAGS += -DDMP_VDSL2WAN_1 -DDMP_X_BROADCOM_COM_VDSL2WAN_1
endif
ifneq ($(strip $(BUILD_PHY_MULTIMODE)),)
BCM_CFLAGS += -DDMP_VDSL2WAN_1 -DDMP_X_BROADCOM_COM_VDSL2WAN_1
endif
export BUILD_PHY_ADSL BUILD_PHY_VDSL BUILD_PHY_MULTIMODE
export BCM_CFLAGS
export BUILD_PHY_ADSL BRCM_ADSL_ANNEX_A BRCM_ADSL_STANDARD
#Set up ADSL standard
export ADSL=$(BRCM_ADSL_STANDARD)
#Set up ADSL_PHY_MODE  {file | obj}
export ADSL_PHY_MODE=file
#Set up ADSL_SELF_TEST
export ADSL_SELF_TEST=$(BRCM_ADSL_SELF_TEST)
export RELEASE_BUILD=1
endif
endif

####################################################
#
#Supplier PATH variable        
#
####################################################	

INSTALL_MOD_PATH = $(KERNELPATH)/..
ifeq ($(strip $(SUPPLIER)),broadcom)
export MODULES_PATH = $(BUILD_DIR)/modules/private
SUPPLIER_TOOLS = $(BUILD_DIR)/tools
else
export MODULES_PATH = $(TOP_PATH)/$(SUPPLIER)/modules/private
SUPPLIER_TOOLS = $(TOP_PATH)/$(SUPPLIER)/tools
endif

ifeq ($(strip $(SUPPLIER)),realtek)
ifeq ($(INCLUDE_LAN_WLAN), y)
WIRELESS_TOOLS = iwpriv
endif
endif

#TG means target
export BOOT_TG_PATH = $(TARGETS_PATH)/$(SUPPLIER)_boot
KERNEL_TG_PATH = $(TARGETS_PATH)/$(SUPPLIER)_$(CPU_TYPE)_kernel
MODULE_TG_PATH = $(TARGETS_PATH)/$(SUPPLIER)_$(CPU_TYPE)_modules
export MODEL_FS_PATH = $(TARGETS_PATH)/fs.$(MODEL)

PRIVATE_LIB_BIN_PATH = $(PRIVATE_APPS_PATH)/bins/$(OBJ_DIR)/libs/
PRIVATE_APPS_BIN_PATH = $(PRIVATE_APPS_PATH)/bins/$(OBJ_DIR)/apps/

####################################################
#
#Supplier compiler variable        
#
####################################################	


CC = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld
AR = $(TOOLPREFIX)ar
STRIP = $(TOOLPREFIX)strip
RANLIB = $(TOOLPREFIX)ranlib

####################################################
#
#PJSIP Variable
#
####################################################
PJSIP_CONFCACHE_EXIST = $(wildcard $(PUBLIC_APPS_PATH)/pjsip_1.10/config_cache_file) 

####################################################
#
#Function conditional compilation        
#
####################################################	

DFLAGS = 
WEBFLAGS = 
DF_FLAGS = 

ifeq ($(INCLUDE_WATCHDOG), y) 
DFLAGS += -DINCLUDE_WATCHDOG
endif 

ifeq ($(INCLUDE_PPA_ACCELERATE), y)
DFLAGS += -DINCLUDE_PPA_ACCELERATE
endif

ifeq ($(INCLUDE_MTD_TYPE1), y)
DFLAGS += -DINCLUDE_MTD_TYPE1
MTD_PART_TYPE = 1
endif

ifeq ($(INCLUDE_MTD_TYPE2), y)
DFLAGS += -DINCLUDE_MTD_TYPE2
MTD_PART_TYPE = 2
endif

ifeq ($(INCLUDE_MTD_TYPE3), y)
DFLAGS += -DINCLUDE_MTD_TYPE3
MTD_PART_TYPE = 3
endif

ifeq ($(INCLUDE_MTD_TYPE4), y)
DFLAGS += -DINCLUDE_MTD_TYPE4
MTD_PART_TYPE = 4
endif

ifeq ($(INCLUDE_MTD_TYPE5), y)
DFLAGS += -DINCLUDE_MTD_TYPE5
MTD_PART_TYPE = 5
endif

ifeq ($(INCLUDE_FLASH_8M), y)
DFLAGS += -DINCLUDE_FLASH_8M
TP_CFLAGS += -DINCLUDE_FLASH_8M
FLASH_SIZE = 0x800000
IMAGE_SIZE = 0x800000
endif

ifeq ($(INCLUDE_FLASH_16M), y)
DFLAGS += -DINCLUDE_FLASH_16M
TP_CFLAGS += -DINCLUDE_FLASH_16M
FLASH_SIZE = 0x1000000
IMAGE_SIZE = 0x1000000
endif

ifeq ($(INCLUDE_FLASH_128M), y)
DFLAGS += -DINCLUDE_FLASH_128M
TP_CFLAGS += -DINCLUDE_FLASH_128M
FLASH_SIZE = 0x8000000
IMAGE_SIZE = 0x1200000
endif
ifneq ($(KERNEL_MAX_SIZE), )
TP_CFLAGS += -DKERNEL_MAX_SIZE=$(KERNEL_MAX_SIZE)
DFLAGS += -DKERNEL_MAX_SIZE=$(KERNEL_MAX_SIZE)
endif
ifeq ($(INCLUDE_CLI_FULL_FEATURE), y)
DFLAGS += -DINCLUDE_CLI_FULL_FEATURE
endif

ifeq ($(INCLUDE_USER_ACCOUNT), y) 
DFLAGS += -DINCLUDE_USER_ACCOUNT
endif

ifeq ($(INCLUDE_CHGINIT_PWD), y) 
DFLAGS += -DINCLUDE_CHGINIT_PWD
endif

ifeq ($(INCLUDE_AUTH_PASSWORD), y) 
DFLAGS += -DINCLUDE_AUTH_PASSWORD
endif

ifeq ($(INCLUDE_KERNEL_MONITOR), y)
DFLAGS += -DINCLUDE_KERNEL_MONITOR # For BBA
TP_CFLAGS += -DINCLUDE_KERNEL_MONITOR # For ECONET Kernel
TP_CFLAGS += -I$(TP_MODULES_PATH)/kmonitor
endif

ifneq ($(INCLUDE_CUSTOMIZED_SP), "")
export CSP=$(subst ",,$(INCLUDE_CUSTOMIZED_SP))
DFLAGS += -DINCLUDE_$(CSP)
endif 

ifeq ($(strip $(CSP)), EU)
DFLAGS += -DINCLUDE_REGION_SELECT
endif

ifeq ($(strip $(MTD_PART_TYPE)),4)
FLASH_RESTRICT_MODELS = W9970V1 W9970_TR_V1 W8970V3 W9970V2 W9970_TR_V2
else
FLASH_RESTRICT_MODELS =
endif

#ifeq ($(strip $(SUPPLIER)),broadcom)
ifeq ($(strip $(BUILD_DSLDIAGD)),y)
DFLAGS += -DINCLUDE_BCM_DSLDIAGD
endif
LASTTEST_COOKIE := $(PRIVATE_APPS_PATH)/.lasttest
LASTTEST        := $(strip $(shell cat $(LASTTEST_COOKIE) 2>/dev/null))
TESTONLY ?= n
ifneq ($(strip $(TESTONLY)), n)
DFLAGS += -DTESTONLY=\"$(TESTONLY)\"
 ifeq ($(strip $(TESTONLY)), TESTONLY)
  ifeq ($(findstring W9970_TTNET, $(MODEL2)), W9970_TTNET)
  DFLAGS += -DFR_INVOLUNTARY
  endif # W9970_TTNET
 endif
endif

LASTSHELL_COOKIE := $(PRIVATE_APPS_PATH)/.lastshell
LASTSHELL        := $(strip $(shell cat $(LASTSHELL_COOKIE) 2>/dev/null))
CLISHELL ?= n
ifeq ($(strip $(CLISHELL)), y)
DFLAGS += -D__CLI_SH__
endif

LASTPROOF_COOKIE := $(PRIVATE_APPS_PATH)/.lastproof
LASTPROOF        := $(strip $(shell cat $(LASTPROOF_COOKIE) 2>/dev/null))
FOOLPROOF ?= 1
ifeq ($(strip $(FOOLPROOF)), 0)
DFLAGS += -DBCM_WLAN_FOOLPROOF=0
else
DFLAGS += -DBCM_WLAN_FOOLPROOF=1
endif
DFLAGS += -DMTD_PART_TYPE=$(MTD_PART_TYPE)
export MTD_PART_TYPE
DFLAGS += -DINCLUDE_$(MODEL2)
TP_CFLAGS += -DINCLUDE_$(MODEL2)
ifeq ($(strip $(INCLUDE_CPU_BCM63167)),y)
DFLAGS += -DINCLUDE_$(MODEL2)_63167
endif
DFLAGS += -DCONFIG_BCM9$(BRCM_CHIP)
ifneq ($(BRCM_EXT_SWITCH_TYPE),)
DFLAGS += -DBRCM_EXT_SWITCH_TYPE=$(BRCM_EXT_SWITCH_TYPE)
endif
ifneq ($(strip $(BRCM_KERNEL_ROOTFS)),squashfs)
  ifeq ($(strip $(CONFIG_ECC_BITS)),)
    CONFIG_ECC_BITS=0
  endif
DFLAGS += -DINCLUDE_NAND_FLASH
endif
ifeq ($(strip $(INCLUDE_LINUX_VERSION_3)),y)
DFLAGS += -DINCLUDE_KERNELVERSION=3
else
DFLAGS += -DINCLUDE_KERNELVERSION=2
endif
TP_CFLAGS += -DCONFIG_TP_IMAGE
DFLAGS += -DCONFIG_TP_IMAGE
ifeq ($(strip $(CPU_TYPE)),bcm63167)
TP_CFLAGS += -DCONFIG_VR600_TT_V1_BCM63167
DFLAGS += -DCONFIG_VR600_TT_V1_BCM63167
export CPU_TYPE
endif
export CONFIG_MODEL=$(MODEL2)
export CONFIG_PRODUCT_ID=$(strip $(shell cat $(CONFFILE_PATH)/oem/$(MODEL)/reduced_data_model.xml | sed -n 's/<X_TP_ProductID[a-zA-Z =]*d=\(0x[0-9a-fA-F]*\)[a-zA-Z0-9 =]*\/>/\1/p'))
ifeq ($(INCLUDE_ANNEXB), y)
export INCLUDE_ANNEXB=y
endif
ifeq ($(INCLUDE_DUAL_CONFIG), y)
DFLAGS += -DINCLUDE_DUAL_CONFIG
export INCLUDE_DUAL_CONFIG=y
endif
ifeq ($(INCLUDE_DUAL_IMAGE), y)
export INCLUDE_DUAL_IMAGE=y
TP_CFLAGS += -DINCLUDE_DUAL_IMAGE
DFLAGS += -DINCLUDE_DUAL_IMAGE
endif
ifeq ($(INCLUDE_BCM_WLAN_FEATURE), y)
TP_CFLAGS += -D INCLUDE_BCM_WLAN_FEATURE
BCM_CFLAGS += -DINCLUDE_BCM_WLAN_FEATURE
endif

ifeq ($(CONFIG_WL0_FOR_5G), y)
DFLAGS += -DCONFIG_WL0_FOR_5G
TP_CFLAGS += -DCONFIG_WL0_FOR_5G
endif

ifneq ($(CSP), )
TP_CFLAGS += -D INCLUDE_$(CSP)
export BCM_CFLAGS += -DINCLUDE_$(CSP)
endif
ifeq ($(INCLUDE_WEB_WARN), y)
export INCLUDE_WEB_WARN=y
endif
ifeq ($(INCLUDE_PKTCAP), y)
export INCLUDE_PKTCAP=y
ifeq ($(INCLUDE_PKTCAP_UPLOAD), y)
export INCLUDE_PKTCAP_UPLOAD=y
endif
endif
ifeq ($(INCLUDE_PORT_MIRRORING), y)
export INCLUDE_PORT_MIRRORING=y
endif
ifeq ($(INCLUDE_WEB_RESTRICTION), y)
DFLAGS += -DINCLUDE_WEB_RESTRICTION
endif
ifneq ($(strip $(BUILD_SWMDK)),)
DFLAGS += -DINCLUDE_BCM_SWMDK
export BUILD_SWMDK
endif
ifneq ($(strip $(BUILD_ETHTOOL)),)
export BUILD_ETHTOOL
endif
ifeq ($(INCLUDE_L2TP), y)
export INCLUDE_FASTPATH=y
endif
ifeq ($(INCLUDE_PPTP), y)
export INCLUDE_FASTPATH=y
endif
ifeq ($(INCLUDE_FASTPATH), y)
DFLAGS += -DINCLUDE_FASTPATH
endif
ifeq ($(INCLUDE_ENCRYPT_PASSWORD), y)
DFLAGS += -DINCLUDE_ENCRYPT_PASSWORD
endif
ifeq ($(INCLUDE_EWAN_VLAN), y)
DFLAGS += -DINCLUDE_EWAN_VLAN
endif
ifeq ($(SUPPORT_MODIFY_LAYER2_PARAMS), y)
DFLAGS += -DSUPPORT_MODIFY_LAYER2_PARAMS
endif
ifeq ($(INCLUDE_FR_CODE), y)
TP_CFLAGS += -DINCLUDE_FR_CODE
endif
ifeq ($(INCLUDE_RB_CODE), y)
TP_CFLAGS += -DINCLUDE_RB_CODE
endif
ifeq ($(INCLUDE_TR098_STANDARD), y)
TP_CFLAGS += -DINCLUDE_TR098_STANDARD
endif
ifeq ($(INCLUDE_FON), y)
DFLAGS += -DINCLUDE_FON
TP_CFLAGS += -DINCLUDE_FON
endif

export CONFIG_TP_IMAGE=y
export TP_CFLAGS
export BRCM_BOARD_ID
export BRCM_FLASHBLK_SIZE
export BRCM_NUM_MAC_ADDRESSES
export BRCM_BASE_MAC_ADDRESS
export BRCM_PSI_SIZE
export BRCM_MAIN_TP_NUM
export BRCM_VERSION
export BRCM_RELEASE
export BRCM_EXTRAVERSION
#endif

#################### ETH Type ######################
ifeq ($(INCLUDE_ETH_LTQ), y) 
DFLAGS += -DINCLUDE_ETH_LTQ
endif 

ifeq ($(INCLUDE_ETH_RA), y) 
DFLAGS += -DINCLUDE_ETH_RA
endif 

ifeq ($(INCLUDE_ETH_BCM), y) 
DFLAGS += -DINCLUDE_ETH_BCM
endif 

ifeq ($(INCLUDE_ETH_ATH), y) 
DFLAGS += -DINCLUDE_ETH_ATH
endif 

ifeq ($(INCLUDE_GMAC1_ONLY), y)
DFLAGS += -DINCLUDE_GMAC1_ONLY
endif

###################### WIFI  ########################
ifeq ($(INCLUDE_LAN_WLAN), y) 
DFLAGS += -DINCLUDE_LAN_WLAN 
endif 

ifeq ($(INCLUDE_WLAN_RA), y) 
DFLAGS += -DINCLUDE_WLAN_RA
endif 

ifeq ($(INCLUDE_WLAN_RTL), y) 
DFLAGS += -DINCLUDE_WLAN_RTL
endif 

ifeq ($(INCLUDE_WLAN_ATH), y) 
DFLAGS += -DINCLUDE_WLAN_ATH
endif 

ifeq ($(INCLUDE_WLAN_BCM), y) 
DFLAGS += -DINCLUDE_WLAN_BCM
BCMNETDRV_DIR=$(BUILD_DIR)/bcmdrivers/broadcom/net
ifeq ($(strip $(CSP)), TTNET)
WL_VER=7.14.89.3303
WL_NAME=wl.3303
else
WL_VER=7.14.89.14
WL_NAME=wl.14
endif

ifeq ($(strip $(SUPPLIER)),broadcom)
export BCM_WLAN_IMPL=$(strip $(shell cat $(KERNELPATH)/$(MODEL)_config | sed -n 's/^CONFIG_BCM_WLAN_IMPL=\([0-9]*\)/\1/p'))
WL_CURR_VER=$(shell cat $(BCMNETDRV_DIR)/wl/impl$(BCM_WLAN_IMPL)/dslcpe_wlan_minor_version|sed -n 's/^\# EPI_VERSION_STR=\([0-9\.]*\)/\1/p')
WL_CURR_NAME=wl.$(shell echo $(WL_CURR_VER)|awk -F. '{print $$NF}')
endif

ifneq ($(strip $(WL_VER)),$(strip $(WL_CURR_VER)))
ifneq ($(BCMNETDRV_DIR)/$(WL_NAME), $(wildcard $(BCMNETDRV_DIR)/$(WL_NAME)))
$(error $(BUILD_DIR)/bcmdrivers/broadcom/net/$(WL_NAME) does not exit)
endif
ifneq ($(shell cat $(BCMNETDRV_DIR)/$(WL_NAME)/impl$(BCM_WLAN_IMPL)/dslcpe_wlan_minor_version|sed -n 's/^\# EPI_VERSION_STR=\([0-9\.]*\)/\1/p'),$(strip $(WL_VER)))
$(error $(BUILD_DIR)/bcmdrivers/broadcom/net/$(WL_NAME) version is not $(WL_VER))
endif
$(warning Current WLAN driver version is $(WL_CURR_VER), $(MODEL) need version $(WL_VER), changing driver directory...)
$(shell mv $(BCMNETDRV_DIR)/wl $(BCMNETDRV_DIR)/$(WL_CURR_NAME))
$(shell mv $(BCMNETDRV_DIR)/$(WL_NAME) $(BCMNETDRV_DIR)/wl)
endif
endif

ifeq ($(ATH_A_BAND_MIMO_NUM_3_3), y)
DFLAGS += -DWLAN_ATH_A_BAND_MIMO_NUM=3
endif

ifeq ($(ATH_G_BAND_MIMO_NUM_2_2), y)
DFLAGS += -DWLAN_ATH_G_BAND_MIMO_NUM=2
endif

ifeq ($(ATH_G_BAND_MIMO_NUM_3_3), y)
DFLAGS += -DWLAN_ATH_G_BAND_MIMO_NUM=3
endif

ifeq ($(INCLUDE_LAN_WLAN_MSSID), y) 
DFLAGS += -DINCLUDE_LAN_WLAN_MSSID 
endif 

ifeq ($(BCM_A_BAND_MIMO_NUM_1_1), y)
DFLAGS += -DWLAN_A_BAND_MIMO_NUM=1
endif

ifeq ($(BCM_A_BAND_MIMO_NUM_2_2), y)
DFLAGS += -DWLAN_A_BAND_MIMO_NUM=2
endif

ifeq ($(BCM_A_BAND_MIMO_NUM_3_3), y)
DFLAGS += -DWLAN_A_BAND_MIMO_NUM=3
endif

ifeq ($(BCM_B_BAND_MIMO_NUM_1_1), y)
DFLAGS += -DWLAN_B_BAND_MIMO_NUM=1
endif

ifeq ($(BCM_B_BAND_MIMO_NUM_2_2), y)
DFLAGS += -DWLAN_B_BAND_MIMO_NUM=2
endif

ifeq ($(BCM_B_BAND_MIMO_NUM_3_3), y)
DFLAGS += -DWLAN_B_BAND_MIMO_NUM=3
endif

ifeq ($(RA_A_BAND_MIMO_NUM_1_1), y)
DFLAGS += -DWLAN_A_BAND_MIMO_NUM=1
endif

ifeq ($(RA_A_BAND_MIMO_NUM_2_2), y)
DFLAGS += -DWLAN_A_BAND_MIMO_NUM=2
endif

ifeq ($(RA_A_BAND_MIMO_NUM_3_3), y)
DFLAGS += -DWLAN_A_BAND_MIMO_NUM=3
endif

ifeq ($(RA_B_BAND_MIMO_NUM_1_1), y)
DFLAGS += -DWLAN_B_BAND_MIMO_NUM=1
endif

ifeq ($(RA_B_BAND_MIMO_NUM_2_2), y)
DFLAGS += -DWLAN_B_BAND_MIMO_NUM=2
endif

ifeq ($(RA_B_BAND_MIMO_NUM_3_3), y)
DFLAGS += -DWLAN_B_BAND_MIMO_NUM=3
endif

ifdef WLAN_VAP_NUM
DFLAGS += -DWLAN_VAP_NUM=$(WLAN_VAP_NUM)
endif

ifeq ($(INCLUDE_LAN_WLAN_GUESTNETWORK), y) 
DFLAGS += -DINCLUDE_LAN_WLAN_GUESTNETWORK 
endif 

ifeq ($(INCLUDE_LAN_WLAN_WDS), y) 
DFLAGS += -DINCLUDE_LAN_WLAN_WDS
endif 

ifeq ($(INCLUDE_LAN_WLAN_WDS_DETECT), y)
DFLAGS += -DINCLUDE_LAN_WLAN_WDS_DETECT
endif

ifeq ($(INCLUDE_LAN_WLAN_BANDSTEERING), y) 
DFLAGS += -DINCLUDE_LAN_WLAN_BANDSTEERING
endif 

ifeq ($(INCLUDE_LAN_WLAN_8021X), y) 
DFLAGS += -DINCLUDE_LAN_WLAN_802_1X
endif 

ifeq ($(INCLUDE_RADIUS_ACCOUNTING), y) 
DFLAGS += -DINCLUDE_RADIUS_ACCOUNTING
BCM_CFLAGS += -DINCLUDE_RADIUS_ACCOUNTING
endif 

ifeq ($(INCLUDE_LAN_WLAN_DUALBAND),y)
DFLAGS+= -DINCLUDE_LAN_WLAN_DUALBAND
TP_CFLAGS += -DINCLUDE_LAN_WLAN_DUALBAND
BCM_CFLAGS += -DINCLUDE_LAN_WLAN_DUALBAND
endif

ifeq ($(INCLUDE_LAN_WLAN_AC),y)
DFLAGS+= -DINCLUDE_LAN_WLAN_AC
endif

ifeq ($(INCLUDE_LAN_WLAN_SCHEDULE), y)
DFLAGS += -DINCLUDE_LAN_WLAN_SCHEDULE
endif

ifeq ($(INCLUDE_LAN_WLAN_ASSOC_SAVE_NO_OBJECT), y)
DFLAGS += -DINCLUDE_LAN_WLAN_ASSOC_SAVE_NO_OBJECT
endif

ifeq ($(INCLUDE_WIFI_5G_CHIP_MT7613E), y)
DFLAGS += -DINCLUDE_WIFI_5G_CHIP_MT7613E
endif
###################### PON  ########################

ifeq ($(strip $(INCLUDE_CPU_88F6550)),y)
DFLAGS += -DINCLUDE_PON_VG1530_ENV
DFLAGS += -DINCLUDE_PON_MARVELL_SW_LIB
endif

ifeq ($(INCLUDE_PON), y) 
DFLAGS += -DINCLUDE_PON
endif 

ifeq ($(INCLUDE_PON_EPON), y) 
DFLAGS += -DINCLUDE_PON_EPON
endif 

ifeq ($(INCLUDE_PON_GPON), y)
DFLAGS += -DINCLUDE_PON_GPON
endif

#################### Function ######################

ifeq ($(INCLUDE_TT_SYSLOG), y)
DFLAGS += -DINCLUDE_TT_SYSLOG
endif

ifeq ($(INCLUDE_MULTI_EWAN), y)
DFLAGS += -DINCLUDE_MULTI_EWAN
endif

ifeq ($(INCLUDE_FR_CODE), y)
DFLAGS += -DINCLUDE_FR_CODE
endif

ifeq ($(INCLUDE_RB_CODE), y)
DFLAGS += -DINCLUDE_RB_CODE
endif

ifeq ($(INCLUDE_TR098_STANDARD), y)
DFLAGS += -DINCLUDE_TR098_STANDARD
endif

ifeq ($(INCLUDE_AUTO_GATEWAY), y)
DFLAGS += -DINCLUDE_AUTO_GATEWAY
endif

ifeq ($(INCLUDE_BRIDGING), y)
DFLAGS += -DINCLUDE_BRIDGING
endif

ifeq ($(INCLUDE_LAN_WAN_BINDING), y)
ifeq ($(strip $(SUPPLIER)), mtk)
ifneq ($(strip $(shell grep "^CONFIG_IP_MULTIPLE_TABLES=y" $(KERNELPATH)/$(CPU_TYPE)_$(MODEL)_config|wc -l ;)),0)
DFLAGS += -DINCLUDE_LAN_WAN_BINDING
TP_CFLAGS += -DINCLUDE_LAN_WAN_BINDING
endif
else
ifneq ($(strip $(shell grep "^CONFIG_IP_MULTIPLE_TABLES=y" $(KERNELPATH)/$(MODEL2)_config|wc -l ;)),0)
DFLAGS += -DINCLUDE_LAN_WAN_BINDING
endif
endif
endif

ifeq ($(INCLUDE_ADSLWAN), y)
DFLAGS += -DINCLUDE_ADSLWAN
endif

ifeq ($(INCLUDE_VDSLWAN), y)
DFLAGS += -DINCLUDE_VDSLWAN
ifeq ($(strip $(SUPPLIER)),broadcom)
DFLAGS += -DCONFIG_VDSL_SUPPORTED
endif
endif

ifeq ($(INCLUDE_XTM_DUAL_CFG), y)
DFLAGS += -DINCLUDE_XTM_DUAL_CFG
endif

ifeq ($(INCLUDE_MACVLAN), y)
DFLAGS += -DINCLUDE_MACVLAN
endif

ifeq ($(INCLUDE_ADSL_RA), y)
DFLAGS += -DINCLUDE_ADSL_RA
endif

ifeq ($(INCLUDE_ADSL_RTL), y)
DFLAGS += -DINCLUDE_ADSL_RTL
endif

ifeq ($(INCLUDE_ADSL_LTQ), y)
DFLAGS += -DINCLUDE_ADSL_LTQ
endif

ifeq ($(INCLUDE_ADSL_BCM), y)
DFLAGS += -DINCLUDE_ADSL_BCM
endif

ifeq ($(INCLUDE_ETHERNET_WAN), y)
DFLAGS += -DINCLUDE_ETHERNET_WAN
endif

ifeq ($(INCLUDE_IGMP), y)
DFLAGS += -DINCLUDE_IGMP
ifeq ($(INCLUDE_IGMP_DIAG), y)
DFLAGS += -DINCLUDE_IGMP_DIAG
endif
ifeq ($(INCLUDE_IGMP_FASTLEAVE), y)
DFLAGS += -DINCLUDE_IGMP_FASTLEAVE
TP_CFLAGS += -DINCLUDE_IGMP_FASTLEAVE
endif
endif

ifeq ($(INCLUDE_DHCP_OPTION_121), y)
DFLAGS += -DINCLUDE_DHCP_OPTION_121
endif

ifeq ($(INCLUDE_IPTV), y)
DFLAGS += -DINCLUDE_IPTV
endif

ifeq ($(INCLUDE_AUTO_PVC), y)
DFLAGS += -DINCLUDE_AUTO_PVC
endif

ifeq ($(INCLUDE_RIP), y)
DFLAGS += -DINCLUDE_RIP
endif

ifeq ($(INCLUDE_SNMP), y)
DFLAGS += -DINCLUDE_SNMP
endif

ifeq ($(INCLUDE_OAM), y)
DFLAGS += -DINCLUDE_OAM
endif

ifeq ($(INCLUDE_SSL), y)
DFLAGS += -DINCLUDE_SSL
OPENSSL_DIR = $(PUBLIC_APPS_PATH)/openssl/
ifeq ($(INCLUDE_TLS_NEW), y)
OPENSSL_DIR = $(PUBLIC_APPS_PATH)/openssl-1.0.2n/
endif
export OPENSSL_DIR
endif

ifeq ($(INCLUDE_CWMP), y)
DFLAGS += -DINCLUDE_CWMP

ifeq ($(INCLUDE_CWMP_SSL), y)
DFLAGS += -DINCLUDE_CWMP_SSL
export INCLUDE_CWMP_SSL
ifeq ($(INCLUDE_CWMP_SSL_VERIFY_PEER), y)
DFLAGS += -DINCLUDE_CWMP_SSL_VERIFY_PEER
export INCLUDE_CWMP_SSL_VERIFY_PEER
endif
endif
ifeq ($(INCLUDE_TR111_PART1), y)
DFLAGS += -DINCLUDE_TR111_PART1
endif
ifeq ($(INCLUDE_TR111_PART2), y)
DFLAGS += -DINCLUDE_TR111_PART2
endif
ifeq ($(INCLUDE_TR143), y)
DFLAGS += -DINCLUDE_TR143
endif
ifeq ($(INCLUDE_TR143_V1_1_0), y)
DFLAGS += -DINCLUDE_TR143_V1_1_0
endif
ifeq ($(INCLUDE_TR143_V1_0_2), y)
DFLAGS += -DINCLUDE_TR143_V1_0_2
endif

ifeq ($(INCLUDE_ECONET_TR143_SPEEDUP), y)
DFLAGS += -DINCLUDE_ECONET_TR143_SPEEDUP
endif

ifeq ($(INCLUDE_IPPING_DIAG), y)
DFLAGS += -DINCLUDE_IPPING_DIAG
endif

ifeq ($(INCLUDE_TRACEROUTE_DIAG), y)
DFLAGS += -DINCLUDE_TRACEROUTE_DIAG
endif

ifeq ($(INCLUDE_DOWNLOAD_DIAG), y)
DFLAGS += -DINCLUDE_DOWNLOAD_DIAG
endif

ifeq ($(INCLUDE_UPLOAD_DIAG), y)
DFLAGS += -DINCLUDE_UPLOAD_DIAG
endif

ifeq ($(INCLUDE_UDPECHO_DIAG), y)
DFLAGS += -DINCLUDE_UDPECHO_DIAG
endif

ifeq ($(INCLUDE_TR064), y)
DFLAGS += -DINCLUDE_TR064
endif

ifeq ($(INCLUDE_DSL_DIAG), y)
DFLAGS += -DINCLUDE_DSL_DIAG
TP_CFLAGS += -DINCLUDE_DSL_DIAG
endif

endif

ifeq ($(INCLUDE_DIGITAL_SIGNATURE), y)
DFLAGS += -DINCLUDE_DIGITAL_SIGNATURE
endif

ifeq ($(INCLUDE_DDNS), y)
DFLAGS += -DINCLUDE_DDNS
endif

ifeq ($(INCLUDE_DDNS_PH), y)
DFLAGS += -DINCLUDE_DDNS_PH
endif

ifeq ($(INCLUDE_DYNDNS), y)
DFLAGS += -DINCLUDE_DYNDNS
endif

ifeq ($(INCLUDE_NOIPDNS), y)
DFLAGS += -DINCLUDE_NOIPDNS
endif

ifeq ($(INCLUDE_DDNS_USERDEFINE), y)
DFLAGS += -DINCLUDE_DDNS_USERDEFINE

ifeq ($(INCLUDE_DDNS_USERDEFINE_DETAIL), y)
DFLAGS += -DINCLUDE_DDNS_USERDEFINE_DETAIL
endif

endif

ifeq ($(INCLUDE_ALG), y)
DFLAGS += -DINCLUDE_ALG
endif

ifeq ($(INCLUDE_ALG_H323), y)
DFLAGS += -DINCLUDE_ALG_H323
endif

ifeq ($(INCLUDE_ALG_SIP), y)
DFLAGS += -DINCLUDE_ALG_SIP
endif

ifeq ($(INCLUDE_ALG_RTSP), y)
DFLAGS += -DINCLUDE_ALG_RTSP
endif

ifeq ($(INCLUDE_WAN_DETECT), y)
DFLAGS += -DINCLUDE_WAN_DETECT
endif

ifeq ($(INCLUDE_DNS_PROXY), y)
DFLAGS += -DINCLUDE_DNS_PROXY
# add by xianjiantao,12-16-19,fix bug 150640
TP_CFLAGS += -DINCLUDE_DNS_PROXY
# end add xianjiantao
endif


ifeq ($(INCLUDE_ENHANCE_DNS_PROXY), y)
DFLAGS += -DINCLUDE_ENHANCE_DNS_PROXY
endif

ifeq ($(INCLUDE_WEB_WARN), y)
DFLAGS += -DINCLUDE_WEB_WARN
endif

ifeq ($(INCLUDE_PKTCAP), y)
DFLAGS += -DINCLUDE_PKTCAP
ifeq ($(INCLUDE_PKTCAP_UPLOAD), y)
DFLAGS += -DINCLUDE_PKTCAP_UPLOAD
endif
endif

ifeq ($(INCLUDE_PORT_MIRRORING), y)
DFLAGS += -DINCLUDE_PORT_MIRRORING
endif

#add by huangjx
ifeq ($(strip $(INCLUDE_NEW_WEB_UI)),y)
DFLAGS += -DINCLUDE_NEW_WEB_UI
endif

ifeq ($(strip $(INCLUDE_MIC)),y)
DFLAGS += -DINCLUDE_MIC
BCM_CFLAGS += -DINCLUDE_MIC
TP_CFLAGS += -DINCLUDE_MIC
endif

#NOTE:INCLUDE_VOIP in voip.mak
ifeq ($(INCLUDE_VOIP), y)
DFLAGS += $(VOIP_DFLAGS)
DF_FLAGS += $(VOIP_CFLAGS)
ifeq ($(INCLUDE_3GBACKUP_PENDING_WITH_VOIP), y)
DFLAGS += -DINCLUDE_3GBACKUP_PENDING_WITH_VOIP
endif
endif

ifeq ($(INCLUDE_IPV6), y)
DFLAGS += -DINCLUDE_IPV6

ifeq ($(INCLUDE_IPV6_MLD), y)
DFLAGS += -DINCLUDE_IPV6_MLD
endif

ifeq ($(INCLUDE_IPV6_SLAAC), y)
DFLAGS += -DINCLUDE_IPV6_SLAAC
endif

ifeq ($(INCLUDE_IPV6_HTTP), y)
DFLAGS += -DINCLUDE_IPV6_HTTP
endif

ifeq ($(INCLUDE_IPV6_CE_ROUTER_AUTH), y)
DFLAGS += -DINCLUDE_IPV6_CE_ROUTER_AUTH
endif

endif

ifeq ($(INCLUDE_USB), y)
DFLAGS += -DINCLUDE_USB
endif

ifeq ($(INCLUDE_USB_OVER_IP), y)
DFLAGS += -DINCLUDE_USB_OVER_IP
endif

ifeq ($(INCLUDE_USB_OVER_IP_TPLINK), y)
DFLAGS += -DINCLUDE_USB_OVER_IP_TPLINK
endif

ifeq ($(INCLUDE_USB_OVER_IP_KCODES), y)
DFLAGS += -DINCLUDE_USB_OVER_IP_KCODES
endif

ifeq ($(INCLUDE_IPP), y)
DFLAGS += -DINCLUDE_IPP
endif

ifeq ($(INCLUDE_USB_STORAGE), y)
DFLAGS += -DINCLUDE_USB_STORAGE
endif

ifeq ($(INCLUDE_USB_MEDIA_SERVER), y)
DFLAGS += -DINCLUDE_USB_MEDIA_SERVER
endif

ifeq ($(INCLUDE_USB_SAMBA_SERVER), y)
DFLAGS += -DINCLUDE_USB_SAMBA_SERVER
ifeq ($(INCLUDE_USB_SAMBA_NMBD), y)
DFLAGS += -DINCLUDE_USB_SAMBA_NMBD
endif
ifeq ($(INCLUDE_USB_SAMBA4), y)
DFLAGS += -DINCLUDE_USB_SAMBA4
USB_SAMBA_DIR=$(PUBLIC_APPS_PATH)/samba-4.6.6
else
USB_SAMBA_DIR=$(PUBLIC_APPS_PATH)/samba-3.0.14a
endif
endif

ifeq ($(INCLUDE_USB_FTP_SERVER), y)
DFLAGS += -DINCLUDE_USB_FTP_SERVER
endif

ifeq ($(INCLUDE_SFTP_SERVER), y)
DFLAGS += -DINCLUDE_SFTP_SERVER
endif

ifeq ($(INCLUDE_SSH_ACCESS), y)
DFLAGS += -DINCLUDE_SSH_ACCESS
endif

ifeq ($(INCLUDE_ENHANCE_PAGE_LOCK), y)
DFLAGS += -DINCLUDE_ENHANCE_PAGE_LOCK
endif

ifeq ($(INCLUDE_INDEPENDENT_ETH_WAN),y)
DFLAGS +=-DINCLUDE_INDEPENDENT_ETH_WAN
endif

ifeq ($(INCLUDE_TCSMUX_MTK),y)
DFLAGS +=-DINCLUDE_TCSMUX_MTK
endif

ifeq ($(INCLUDE_EWANIPTV_DEFAULT),y)
DFLAGS +=-DINCLUDE_EWANIPTV_DEFAULT
endif

ifeq ($(INCLUDE_USB_3G_DONGLE), y)
DFLAGS += -DINCLUDE_USB_3G_DONGLE
endif

ifeq ($(INCLUDE_IPSEC), y)
DFLAGS += -DINCLUDE_IPSEC
endif

ifeq ($(INCLUDE_WAN_MODE), y)
DFLAGS += -DINCLUDE_WAN_MODE
endif

ifeq ($(INCLUDE_QOS), y)
DFLAGS += -DINCLUDE_QOS
endif

ifeq ($(INCLUDE_ANNEXB), y)
DFLAGS += -DINCLUDE_ANNEXB
endif

ifeq ($(INCLUDE_WAN_VLAN), y)
DFLAGS += -DINCLUDE_WAN_VLAN
endif

ifeq ($(INCLUDE_POLICY_ROUTE), y)
DFLAGS += -DINCLUDE_POLICY_ROUTE
endif

ifeq ($(INCLUDE_L2TP), y)
DFLAGS += -DINCLUDE_L2TP
endif

ifeq ($(INCLUDE_PPTP), y)
DFLAGS += -DINCLUDE_PPTP
endif

ifeq ($(INCLUDE_DUAL_ACCESS), y)
DFLAGS += -DINCLUDE_DUAL_ACCESS
endif

ifeq ($(INCLUDE_ACL), y)
DFLAGS += -DINCLUDE_ACL
ifeq ($(INCLUDE_ACL_ADVANCE), y)
DFLAGS += -DINCLUDE_ACL_ADVANCE
endif
endif

ifeq ($(INCLUDE_AUTO_FTP_UPDATE), y)
DFLAGS += -DINCLUDE_AUTO_FTP_UPDATE
endif
ifeq ($(INCLUDE_HTTPS), y)
DFLAGS += -DINCLUDE_HTTPS
endif

ifeq ($(INCLUDE_GERMANY_SPEC), y)
DFLAGS += -DINCLUDE_GERMANY_SPEC
endif
ifeq ($(INCLUDE_PPP_RECONN), y)
DFLAGS += -DINCLUDE_PPP_RECONN
endif

ifeq ($(INCLUDE_FACTORY_DEFAULT), y)
DFLAGS += -DINCLUDE_FACTORY_DEFAULT
endif

ifeq ($(INCLUDE_MER), y)
DFLAGS += -DINCLUDE_MER
endif

ifeq ($(INCLUDE_LAN_VLAN), y)
DFLAGS += -DINCLUDE_LAN_VLAN
endif
ifeq ($(INCLUDE_CLASSIFY_AUTH),y)
DFLAGS += -DINCLUDE_CLASSIFY_AUTH
endif

ifeq ($(INCLUDE_NTP_BIND), y)
DFLAGS += -DINCLUDE_NTP_BIND
endif

ifeq ($(INCLUDE_WORK_AROUND), y)
DFLAGS += -DINCLUDE_WORK_AROUND
endif

DF_FLAGS += $(DFLAGS)


WEBFLAGS += WEB_INCLUDE_TEST=0

ifeq ($(WEB_INCLUDE_MULTI_EWAN), y)
WEBFLAGS += WEB_INCLUDE_MULTI_EWAN=1
else
WEBFLAGS += WEB_INCLUDE_MULTI_EWAN=0
endif

ifeq ($(WEB_INCLUDE_DST), y)
WEBFLAGS += WEB_INCLUDE_DST=1
else
WEBFLAGS += WEB_INCLUDE_DST=0
endif

ifeq ($(INCLUDE_MACVLAN),y)
WEBFLAGS += WEB_INCLUDE_MACVLAN=1
else
WEBFLAGS += WEB_INCLUDE_MACVLAN=0
endif

ifeq ($(INCLUDE_WEB_WATERMARK_DATE_CODE),y)
WEBFLAGS += INCLUDE_WEB_WATERMARK_DATE_CODE=1
else
WEBFLAGS += INCLUDE_WEB_WATERMARK_DATE_CODE=0
endif

ifeq ($(INCLUDE_WEB_WATERMARK_BETA_FIRMWARE),y)
WEBFLAGS += INCLUDE_WEB_WATERMARK_BETA_FIRMWARE=1
else
WEBFLAGS += INCLUDE_WEB_WATERMARK_BETA_FIRMWARE=0
endif

####################################################
#
#Export some variable        
#
####################################################	
DM_H_FILES := dm_objId.h dm_objIdStr.h dm_objType.h dm_instanceMaxNum.h dm_validStrings.h dm_paramLen.h
DM_C_FILES := dm_strTbl.c
DM_RSL_FILES := rsl_func.c rsl_func.h
DM_OS_FILES := dm_paramLen.h
STRIP = $(TOOLPREFIX)strip
export TOOLPREFIX
export HOST
#For modules build & br2684ctl build
export KERNELPATH
#For install kernel modules
export INSTALL_MOD_PATH
#For code control, placed different target file
export OBJ_DIR
#For function conditional compilation
export DF_FLAGS

export TOP_PATH

export OS_LIB_PATH

export PUBLIC_APPS_PATH

export PRIVATE_LIB_BIN_PATH

export PRIVATE_APPS_BIN_PATH

export TP_MODULES_PATH

#For complier
export CC
export LD
export AR
export STRIP
export RANLIB

####################################################
#
#Menuconfig         
#
####################################################
menuconfig:
	cp $(BUILD_PATH)/config/$(MODEL).config $(BUILD_PATH)/.config
	$(BUILD_PATH)/scripts/config/mconf $(BUILD_PATH)/sysdeps/linux/Config.in
	cp $(BUILD_PATH)/.config $(BUILD_PATH)/config/$(MODEL).config
	@if egrep "^INCLUDE_CPU_TC3182=y" .config > /dev/null; then \
		cd $(TOP_PATH)/ralink && $(MAKE) PROFILE=tc3182 menuconfig; \
	fi
	@if egrep "^INCLUDE_CPU_TC3162U=y" .config > /dev/null; then \
		cd $(TOP_PATH)/ralink && $(MAKE) PROFILE=tc3162u menuconfig; \
	fi
	@if egrep "^INCLUDE_CPU_RTL8672=y" .config > /dev/null; then \
		cd $(TOP_PATH)/realtek && $(MAKE) preconfig2630_01 && $(MAKE) menuconfig;\
	fi
	@if egrep "^INCLUDE_CPU_VR268=y" .config > /dev/null; then \
		cd $(TOP_PATH)/lantiq && $(MAKE) menuconfig;\
	fi
	@if egrep "^INCLUDE_CPU_RT63368=y" .config > /dev/null; then \
		cd $(TOP_PATH)/mtk && $(MAKE) PROFILE=rt63368_demo menuconfig; \
	fi
	@if egrep "^INCLUDE_CPU_RT63365=y" .config > /dev/null; then \
		cd $(TOP_PATH)/mtk && $(MAKE) PROFILE=rt63365_demo menuconfig; \
	fi
ifeq ($(strip $(SUPPLIER)),broadcom)
	cd $(BUILD_DIR) && $(MAKE) PROFILE=$(PROFILE) menuconfig
	cp $(BUILD_DIR)/targets/$(PROFILE)/$(PROFILE) $(BUILD_DIR)/tp/$(PROFILE)_$(MODEL)
endif
ifeq ($(strip $(SUPPLIER)),mtk)
	cd $(TOP_PATH)/mtk && $(MAKE) PROFILE=$(CPU_TYPE)_demo menuconfig; \
	cp $(PROFILE_PATH)/$(CPU_TYPE)_demo.profile $(PROFILE_PATH)/$(MODEL).profile
endif

####################################################
#
#Build apps
#
####################################################

#openssl should before pjsip case pjsip need it
#libusb&libcompat should before handle_card&usb_modeswitch
.PHONY: apps_build
apps_build: public_apps_build 

####################################################
#
#Build public apps
#
####################################################
# here define the unused files under iptables/extension and they will not be compiled into iptables
#
IPT_EXTENSION_UNUSED :=

ifeq ($(strip $(SUPPLIER)),broadcom)
ifneq ($(KERNELVERSION), 2.6.30)
IPT_EXTENSION_UNUSED +=	libxt_gre.c
endif
endif
ifneq ($(filter $(MODEL2), $(FLASH_RESTRICT_MODELS)),)
IPT_EXTENSION_UNUSED +=	libipt_ah.c libipt_CLUSTERIP.c libipt_ECN.c libipt_LOG.c libipt_MIRROR.c libipt_NETMAP.c
IPT_EXTENSION_UNUSED +=	libipt_realm.c libipt_REDIRECT.c libipt_SAME.c libipt_ttl.c libipt_TTL.c libipt_ULOG.c libipt_unclean.c
IPT_EXTENSION_UNUSED += libipt_webWarn.c libxt_addrtype.c libxt_AUDIT.c libxt_CHECKSUM.c libxt_CLASSIFY.c libxt_cluster.c
IPT_EXTENSION_UNUSED += libxt_connbytes.c libxt_connlimit.c libxt_CONNMARK.c libxt_CONNSECMARK.c libxt_cpu.c libxt_CT.c
IPT_EXTENSION_UNUSED += libxt_dccp.c libxt_devgroup.c libxt_ecn.c libxt_hashlimit.c libxt_HMARK.c libxt_IDLETIMER.c libxt_ipvs.c libxt_LED.c
IPT_EXTENSION_UNUSED += libxt_length.c libxt_limit.c libxt_nfacct.c libxt_NFLOG.c libxt_NFQUEUE.c libxt_osf.c libxt_owner.c libxt_physdev.c
IPT_EXTENSION_UNUSED += libxt_pkttype.c libxt_policy.c libxt_quota.c libxt_rateest.c libxt_RATEEST.c libxt_recent.c libxt_rpfilter.c
IPT_EXTENSION_UNUSED += libxt_sctp.c libxt_SECMARK.c libxt_set.c libxt_SET.c libxt_socket.c libxt_statistic.c
IPT_EXTENSION_UNUSED +=	libxt_string.c libxt_TCPOPTSTRIP.c libxt_TEE.c libxt_time.c libxt_tos.c libxt_TOS.c libxt_TPROXY.c libxt_TRACE.c
endif

.PHONY: public_apps_build
public_apps_build: oslibs libusb libcompat openssl pjsip \
					br2684ctl busybox tc iptables ebtables pppd ripd \
					upnplib upnp flex radvd dhcpv6 samba ntfs-3g arpd \
					usbip vsftp ffmpeg libdlna ushare ip ipsectools \
					chat usb_modeswitch xl2tpd strace dnsmasq libpcap tcpdump traceroute \
					zlib dropbear curl

.PHONY:oslibs
oslibs:
	cd $(PUBLIC_APPS_PATH)/os_libs && $(MAKE)
					
.PHONY: busybox
busybox:
	@if [ -e $(PUBLIC_APPS_PATH)/busybox-1.19.2/$(MODEL2).config ]; then \
		cd $(PUBLIC_APPS_PATH)/busybox-1.19.2 && cp $(MODEL2).config .config; \
	fi
	cd $(PUBLIC_APPS_PATH)/busybox-1.19.2 && $(MAKE)
	
.PHONY: tc
tc:
	cd $(PUBLIC_APPS_PATH)/iproute2-2.6.24-rc7 && $(MAKE)

.PHONY: iptables
iptables:
ifneq ($(strip $(IPT_EXTENSION_UNUSED)), )
	@if [ -e $(PUBLIC_APPS_PATH)/iptables-1.4.17/Makefile]; then \
		$(MAKE) -C $(PUBLIC_APPS_PATH)/iptables-1.4.17 clean; \
	fi
	@rm -fr $(PUBLIC_APPS_PATH)/iptables-1.4.17/.tmp
	@mkdir -p $(PUBLIC_APPS_PATH)/iptables-1.4.17/.tmp
	@for file in $(IPT_EXTENSION_UNUSED); do \
		if [ -e $(PUBLIC_APPS_PATH)/iptables-1.4.17/extensions/$$file ]; then \
			mv $(PUBLIC_APPS_PATH)/iptables-1.4.17/extensions/$$file $(PUBLIC_APPS_PATH)/iptables-1.4.17/.tmp; \
		fi \
	done
endif

ifeq ($(INCLUDE_FASTPATH), y) 
	cd $(PUBLIC_APPS_PATH)/iptables-1.4.17 && test -e Makefile || ./configure --host=$(HOST) --enable-static --disable-shared \
		CFLAGS="-DINCLUDE_FASTPATH -D__EXPORTED_HEADERS__ -I$(KERNELPATH)/include" 
else
	cd $(PUBLIC_APPS_PATH)/iptables-1.4.17 && test -e Makefile || ./configure --host=$(HOST) --enable-static --disable-shared  
endif
	cd $(PUBLIC_APPS_PATH)/iptables-1.4.17 && $(MAKE) AR=$(TOOLPREFIX)ar
	$(STRIP) $(PUBLIC_APPS_PATH)/iptables-1.4.17/iptables/xtables-multi

ifneq ($(strip $(IPT_EXTENSION_UNUSED)), )
	@for file in $(IPT_EXTENSION_UNUSED); do \
		if [ -e $(PUBLIC_APPS_PATH)/iptables-1.4.17/.tmp/$$file ]; then \
			mv $(PUBLIC_APPS_PATH)/iptables-1.4.17/.tmp/$$file $(PUBLIC_APPS_PATH)/iptables-1.4.17/extensions; \
		fi \
	done
	@rm -rf $(PUBLIC_APPS_PATH)/iptables-1.4.17/.tmp
endif

.PHONY: ebtables
ebtables:
	cd $(PUBLIC_APPS_PATH)/ebtables-v2.0.10-4 && $(MAKE) static
	
.PHONY: pppd
# modify by xianjiantao for vr600_ttnet,16-10-18
pppd:
ifeq ($(strip $(CSP)), TTNET)
	cd $(PUBLIC_APPS_PATH)/ppp-2.4.5/pppd && $(MAKE) INCLUDE_TTNET=y INCLUDE_LINUX_VERSION_3=$(INCLUDE_LINUX_VERSION_3) INCLUDE_ADSL_RA=$(INCLUDE_ADSL_RA)
else
	cd $(PUBLIC_APPS_PATH)/ppp-2.4.5/pppd && $(MAKE) INCLUDE_LINUX_VERSION_3=$(INCLUDE_LINUX_VERSION_3) INCLUDE_ADSL_RA=$(INCLUDE_ADSL_RA)
endif #CSP
# end modify xianjiantao
	
.PHONY: br2684ctl
br2684ctl:
ifneq ($(strip $(SUPPLIER)),broadcom)
	cd $(PUBLIC_APPS_PATH)/br2684ctl && $(MAKE)
else
	@echo no br2684ctl
endif
	
.PHONY: arpd
arpd:
ifneq ($(strip $(SUPPLIER)),broadcom)
	cd $(PUBLIC_APPS_PATH)/linux-atm && $(MAKE)
endif
	
.PHONY: upnplib
upnplib:
	$(MAKE) -C $(PUBLIC_APPS_PATH)/upnp_libs
	
.PHONY: upnp
upnp:
	$(MAKE) -C $(PUBLIC_APPS_PATH)/IGD

.PHONY: ripd
ripd:
ifeq ($(INCLUDE_RIP), y)
	$(MAKE) -C $(PUBLIC_APPS_PATH)/zebra-0.95a
else
	@echo no ripd
endif

.PHONY: pjsip
pjsip:
ifneq ($(strip $(INCLUDE_VOIP)),)
ifneq ($(strip $(PJSIP_CONFCACHE_EXIST)),)
	cd $(PUBLIC_APPS_PATH)/pjsip_1.10 && $(MAKE)
else
	cd $(PUBLIC_APPS_PATH)/pjsip_1.10 && ./configure --cache-file=./config_cache_file --srcdir=./ --prefix=$(TOOLROOTPATH) --exec-prefix=$(TOOLROOTPATH) --host=$(HOST) --target=$(HOST) --disable-speex-aec --disable-l16-codec --disable-gsm-codec --disable-speex-codec --disable-ilbc-codec --disable-g7221-codec --disable-ssl --disable-oss --disable-sound && $(MAKE)
endif
else
	@echo no pjsip 
endif

.PHONY: usbip
usbip:
ifeq ($(INCLUDE_USB_OVER_IP_TPLINK), y)
	$(MAKE) -C $(PUBLIC_APPS_PATH)/usbip-0.1.7 clean
	cd $(PUBLIC_APPS_PATH)/usbip-0.1.7 && $(MAKE)
endif

.PHONY: vsftp
vsftp:
ifeq ($(INCLUDE_USB_FTP_SERVER), y)
	cd $(PUBLIC_APPS_PATH)/vsftpd-2.3.2 && $(MAKE)
else 
	@echo no vsftp
endif


.PHONY: zlib
zlib:
	echo "Building zlib"
ifeq ($(INCLUDE_SSH_ACCESS), y)
	cd $(PUBLIC_APPS_PATH)/zlib-1.2.11 && test -e Makefile || ./configure --prefix=./install
	cd $(PUBLIC_APPS_PATH)/zlib-1.2.11 && $(MAKE) && $(MAKE) install
else
	@echo no zlib
endif

.PHONY: dropbear
dropbear:
	echo "Building dropbear"
ifeq ($(INCLUDE_SSH_ACCESS), y)
	cd $(PUBLIC_APPS_PATH)/dropbear && test -e Makefile || ./configure --host=$(HOST) --prefix=$(PUBLIC_APPS_PATH)/dropbear/install/ \
		--with-zlib=../zlib-1.2.11/install --disable-harden
	#cd $(PUBLIC_APPS_PATH)/dropbear && $(MAKE) PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" \
	#	&& $(MAKE) PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" install
	cd $(PUBLIC_APPS_PATH)/dropbear && $(MAKE) MULTI=1 SCPPROGRESS=1
else
	@echo no dropbear
endif

.PHONY: flex
flex:
ifeq ($(INCLUDE_IPV6), y)
	cd $(PUBLIC_APPS_PATH)/flex-2.5.35 && test -e Makefile || ./configure --host=$(HOST)
	cd $(PUBLIC_APPS_PATH)/flex-2.5.35 && $(MAKE)
	mkdir -p $(PUBLIC_APPS_PATH)/lib
	cp -p $(PUBLIC_APPS_PATH)/flex-2.5.35/libfl.a $(PUBLIC_APPS_PATH)/lib
else 
	@echo no ipv6 flex
endif

.PHONY: radvd
radvd:
ifeq ($(INCLUDE_IPV6), y)
	cd $(PUBLIC_APPS_PATH)/radvd-1.5 && test -e Makefile || ./configure --host=$(HOST) 
	cd $(PUBLIC_APPS_PATH)/radvd-1.5 && $(MAKE)
else 
	@echo no ipv6 radvd
endif

.PHONY: dhcpv6
dhcpv6:
ifeq ($(INCLUDE_IPV6), y)
	cd $(PUBLIC_APPS_PATH)/wide-dhcpv6-20080615 && test -e Makefile || ./configure --host=$(HOST) 
	cd $(PUBLIC_APPS_PATH)/wide-dhcpv6-20080615 && $(MAKE)
else 
	@echo no ipv6 dhcpv6
endif

.PHONY: ip
ip: 
ifeq ($(INCLUDE_IPV6), y)
	cd $(PUBLIC_APPS_PATH)/iproute2-2.6.39 && $(MAKE)
else
	@echo no ipv6 ip
endif

#for samba and ntfs-3g, zjj, 20111129
.PHONY: samba
ifeq ($(INCLUDE_USB_SAMBA4), y)
include Makefile.smb
samba:smb4_build
else
samba:samba_old
endif

samba_old:
ifeq ($(INCLUDE_USB_SAMBA_SERVER), y)
#replace with samba 3.6.25. Modified by YuChuwei-20170622
	cd $(PUBLIC_APPS_PATH)/samba-3.6.25 && $(MAKE) samba
else 
	@echo no samba
endif
	
.PHONY: ntfs-3g
ntfs-3g:
ifeq ($(INCLUDE_USB_STORAGE), y)
	cd $(PUBLIC_APPS_PATH)/ntfs-3g_ntfsprogs-2012.1.15 && test -e Makefile || ./configure --build=i686-linux --host=$(HOST) --disable-nfconv
	cd $(PUBLIC_APPS_PATH)/ntfs-3g_ntfsprogs-2012.1.15 && $(MAKE)
	$(STRIP) $(PUBLIC_APPS_PATH)/ntfs-3g_ntfsprogs-2012.1.15/libntfs-3g/.libs/libntfs-3g.so.83.0.0
	$(STRIP) $(PUBLIC_APPS_PATH)/ntfs-3g_ntfsprogs-2012.1.15/src/.libs/ntfs-3g
	$(STRIP) $(PUBLIC_APPS_PATH)/ntfs-3g_ntfsprogs-2012.1.15/src/.libs/lowntfs-3g
else 
	@echo no ntfs-3g+ntfsprogs
endif
#end by zjj

#for dlna, LI CHENGLONG
.PHONY: ffmpeg
ffmpeg:
ifeq ($(INCLUDE_USB_MEDIA_SERVER), y)
	$(MAKE) -C $(PUBLIC_APPS_PATH)/ffmpeg-0.6/
endif

.PHONY: libdlna
libdlna:
ifeq ($(INCLUDE_USB_MEDIA_SERVER), y)
	$(MAKE) -C $(PUBLIC_APPS_PATH)/libdlna-0.2.3/
endif

.PHONY: ushare
ushare:
ifeq ($(INCLUDE_USB_MEDIA_SERVER), y)
	$(MAKE) -C $(PUBLIC_APPS_PATH)/ushare-1.1a/
endif
#end by LI CHENGLONG

.PHONY: openssl
openssl:
ifeq ($(INCLUDE_SSL), y)
ifeq ($(INCLUDE_TLS_NEW), y)
	cd $(OPENSSL_DIR) && test -e Makefile || (export -n CROSS_COMPILE && ./Configure shared \
		no-threads no-zlib no-asm no-gmp no-krb5 no-hw no-cipher no-rfc3779 no-sse2 no-speed \
		no-md2 no-rc5 no-ssl2 no-ssl3 \
		linux-generic32 --prefix=$(OPENSSL_DIR)/install)\
	&& sed -i 's/-O3/-Os -DGNU/g' Makefile
else
	cd $(OPENSSL_DIR) && test -e Makefile || ./Configure shared \
		no-threads no-zlib no-asm no-gmp no-krb5 no-hw no-cipher no-rfc3779 no-sse2 no-speed \
		no-md2 no-md4 no-mdc2 no-ripemd no-rc5 no-idea no-bf no-cast no-camellia \
		no-ec no-dsa no-ecdsa no-store linux-mips \
	&& sed -i 's/-O3/-Os -DGNU/g' Makefile
endif
	cd $(OPENSSL_DIR)/ \
	&& $(MAKE) CC=$(CC) LD=$(LD) SHARED_LDFLAGS="-s"\
	&& $(MAKE) install \
	&& mkdir -p ./lib \
	&& cp -p ./libcrypto.so.* ./lib/ \
	&& cp -p ./libssl.so.* ./lib/ \
	&& cd ./lib/ \
	&& ln -sf libcrypto.so.* libcrypto.so \
	&& ln -sf libssl.so.* libssl.so
	$(STRIP) $(OPENSSL_DIR)/install/bin/openssl
endif

.PHONY: ipsectools
ipsectools:
ifeq ($(INCLUDE_IPSEC), y)
	cd $(PUBLIC_APPS_PATH)/ipsectools && test -e Makefile || ./configure --host=$(HOST) --with-kernel-headers=$(KERNELPATH)/include \
	--with-openssl=$(OPENSSL_DIR)/ --enable-security-context=no --enable-shared=yes \
	--enable-dpd=yes CFLAGS="-Os -DGNU -D__EXPORTED_HEADERS__" 
	cd $(PUBLIC_APPS_PATH)/ipsectools && $(MAKE)
	$(STRIP) $(PUBLIC_APPS_PATH)/ipsectools/src/racoon/.libs/racoon
	$(STRIP) $(PUBLIC_APPS_PATH)/ipsectools/src/setkey/.libs/setkey
	$(STRIP) $(PUBLIC_APPS_PATH)/ipsectools/src/racoon/.libs/libracoon.so.0.0.0
	$(STRIP) $(PUBLIC_APPS_PATH)/ipsectools/src/libipsec/.libs/libipsec.so.0.0.1
endif

.PHONY: strace
strace:
ifeq ($(INCLUDE_STRACE), y)
	cd $(PUBLIC_APPS_PATH)/strace-4.8 && test -e Makefile || ./configure --host=$(HOST)
	cd $(PUBLIC_APPS_PATH)/strace-4.8 && $(MAKE) && $(STRIP) strace
else
	@echo no strace 
endif

.PHONY: chat
chat:
ifeq ($(INCLUDE_USB_3G_DONGLE), y)
	@echo building chat...
	cd $(PUBLIC_APPS_PATH)/ppp-2.4.5/chat && $(MAKE)
	$(STRIP) $(PUBLIC_APPS_PATH)/ppp-2.4.5/chat/chat
else
	@echo no chat
endif

.PHONY: libusb
libusb:
ifeq ($(INCLUDE_USB_3G_DONGLE), y)
	echo "Building usb library"
	cd $(PUBLIC_APPS_PATH)/libusb-1.0.18 && test -e Makefile || ./configure --build=i686-linux --host=$(HOST) --disable-udev --prefix=$(PUBLIC_APPS_PATH)/libusb-1.0.18/install
	cd $(PUBLIC_APPS_PATH)/libusb-1.0.18 && $(MAKE) install
else
	@echo no libusb
endif

.PHONY: libcompat
libcompat:
ifeq ($(INCLUDE_USB_3G_DONGLE), y)
	echo "Building usb compat"
	cd $(PUBLIC_APPS_PATH)/libusb-compat-0.1.5 && test -e Makefile || ./configure --build=i686-linux --host=$(HOST) --prefix=$(PUBLIC_APPS_PATH)/libusb-compat-0.1.5/install LIBUSB_1_0_CFLAGS=-I$(PUBLIC_APPS_PATH)/libusb-1.0.18/install/include/libusb-1.0 LIBUSB_1_0_LIBS=-L$(PUBLIC_APPS_PATH)/libusb-1.0.18/install/lib\ -lusb-1.0
	cd $(PUBLIC_APPS_PATH)/libusb-compat-0.1.5 && $(MAKE) install
else
	@echo no libcompat
endif

.PHONY: usb_modeswitch
usb_modeswitch:
ifeq ($(INCLUDE_USB_3G_DONGLE), y)
	echo "Building usb_modeswitch"
	cd $(PUBLIC_APPS_PATH)/usb-modeswitch-1.2.3 && $(MAKE)
	$(STRIP) $(PUBLIC_APPS_PATH)/usb-modeswitch-1.2.3/usb_modeswitch
else
	@echo no usb_modeswitch
endif

.PHONY: xl2tpd
xl2tpd:
ifeq ($(INCLUDE_L2TP), y)
	cd $(PUBLIC_APPS_PATH)/xl2tpd-1.1.12 && $(MAKE)
endif
ifeq ($(strip $(CSP)), TTNET)
	cd $(PUBLIC_APPS_PATH)/xl2tpd-1.1.12 && $(MAKE)
endif

.PHONY: dnsmasq
dnsmasq:
ifeq ($(INCLUDE_DNS_PROXY), y)
	cd $(PUBLIC_APPS_PATH)/dnsmasq-2.80 && $(MAKE)
endif

.PHONY: curl
curl:
ifeq ($(strip $(CSP)), TTNET)
	cd $(PUBLIC_APPS_PATH)/curl-7.65.1 && test -e Makefile || ./configure --build=i686-linux --host=$(HOST) --prefix=$(PUBLIC_APPS_PATH)/curl-7.65.1/install --with-ssl=$(OPENSSL_DIR) \
	--enable-shared --enable-static --disable-dict --disable-ftp --disable-imap --disable-ldap --disable-ldaps --disable-pop3 --disable-proxy --disable-rtsp --disable-smtp \
	--disable-telnet --disable-tftp --disable-zlib --without-ca-bundls --without-gnutls --without-libidn --without-librtmp --without-libssh2 --without-nss --without-zlib
	cd $(PUBLIC_APPS_PATH)/curl-7.65.1 && $(MAKE) LIBS="-ldl -lssl -lcrypto" && $(MAKE) install
endif

.PHONY: traceroute
traceroute:
ifeq ($(strip $(CSP)), TTNET)
	cd $(PUBLIC_APPS_PATH)/traceroute-2.1.0 && $(MAKE) env=yes
else
	@echo no traceroute
endif


.PHONY: libpcap
libpcap:
	echo "Building libpcap"
ifeq ($(INCLUDE_LIBPCAP), y)
	cd $(PUBLIC_APPS_PATH)/libpcap-1.8.1 && test -e Makefile || ac_cv_linux_vers=$(KERNELVERSION) ./configure --build=i686-linux --host=$(HOST) --with-pcap=linux --prefix=$(PUBLIC_APPS_PATH)/libpcap-1.8.1/install
	cd $(PUBLIC_APPS_PATH)/libpcap-1.8.1 && $(MAKE) && $(MAKE) install
else
	@echo no libpcap
endif

.PHONY: tcpdump
tcpdump:
	echo "Building tcpdump"
ifeq ($(INCLUDE_TCPDUMP), y)
	cd $(PUBLIC_APPS_PATH)/tcpdump-4.1.1 && test -e Makefile || ac_cv_linux_vers=$(KERNELVERSION) ./configure --build=i686-linux --host=$(HOST) --with-pcap=linux --prefix=$(PUBLIC_APPS_PATH)/tcpdump-4.1.1/install
	cd $(PUBLIC_APPS_PATH)/tcpdump-4.1.1 && $(MAKE) && $(MAKE) install
else
	@echo no tcpdump
endif
####################################################
#
#Build filesystem, prepare basic fs first
#
####################################################
.PHONY: fs_build
fs_build: fs_clean fs_create fs_lib fs_dev fs_bin fs_sysfiles fs_rootfs

fs_clean:
	rm -rf $(MODEL_FS_PATH)/../rootfs.$(MODEL)
	rm -rf $(MODEL_FS_PATH)
	rm -rf $(TOP_PATH)/targets/reduced_data_model_plaintext_$(MODEL)*
	
fakeroot_clean:    
	if [ -e $(FAKEROOT_PATH)/Makefile ];  then make -C $(FAKEROOT_PATH) distclean; fi
	rm -f $(FAKEROOT_PATH)/fakeroot
	rm -f $(FAKEROOT_PATH)/.configured
	
.PHONY: fs_create
fs_create: 
	cp -pR $(TOP_PATH)/targets/fs.dir $(MODEL_FS_PATH)
	@rm -f $(MODEL_FS_PATH)/etc/MFG_conf.bin
	find  $(MODEL_FS_PATH) -name ".svn" | xargs rm -rf
ifeq ($(strip $(SUPPLIER)),broadcom)
	-rm -rf $(MODEL_FS_PATH)/dev/mei_cpe
	-rm -rf $(MODEL_FS_PATH)/dev/dsl_cpe_api
	-rm -rf $(MODEL_FS_PATH)/dev/switch_api
endif
ifneq ($(INCLUDE_CWMP_SSL_VERIFY_PEER), y)
	@rm -fr $(MODEL_FS_PATH)/etc/cert
endif

	#for rcS, yuanshang, 20111026
	rm -fr $(MODEL_FS_PATH)/etc/init.d/*
	cp $(TOP_PATH)/targets/fs.dir/etc/init.d/rcS_$(MODEL2) $(MODEL_FS_PATH)/etc/init.d/rcS

ifeq ($(INCLUDE_SSH_ACCESS), y)
	cp $(TOP_PATH)/targets/fs.dir/etc/shells $(MODEL_FS_PATH)/etc/shells
endif

#ifeq ($(strip $(SUPPLIER)),broadcom)
	#for wps_monitor, jiyu, 20131111
	cd $(MODEL_FS_PATH) && ln -s /var/tmp tmp
#endif

ifneq ($(INCLUDE_USB_VOICEMAIL), y)
	@rm -fr $(MODEL_FS_PATH)/etc/voiceNotify
endif	
ifneq ($(INCLUDE_HTTPS), y)
	@rm -fr $(MODEL_FS_PATH)/etc/ssl
endif

.PHONY: fs_lib
fs_lib:
	cp $(PUBLIC_APPS_PATH)/upnp_libs/*.so $(MODEL_FS_PATH)/lib/
	cp $(OS_LIB_PATH)/libos.so $(MODEL_FS_PATH)/lib

ifeq ($(INCLUDE_SSL), y)
	cp -p $(OPENSSL_DIR)/libcrypto.so.* $(MODEL_FS_PATH)/lib/
	cd $(MODEL_FS_PATH)/lib/ && ln -sf libcrypto.so.* libcrypto.so
	cp -p $(OPENSSL_DIR)/libssl.so.* $(MODEL_FS_PATH)/lib/
	cd $(MODEL_FS_PATH)/lib/ && ln -sf libssl.so.* libssl.so
endif
	
ifneq ($(strip $(INCLUDE_USB_VOICEMAIL)),)
	cp $(APP_PJSIP_DIR)/usbvm/lib/libusbvmcommon.so $(MODEL_FS_PATH)/lib/
endif

	#for the libs of samba and ntfs-3g, zjj, 20111129
ifeq ($(INCLUDE_USB_STORAGE), y)
	cp -p $(PUBLIC_APPS_PATH)/ntfs-3g_ntfsprogs-2012.1.15/libntfs-3g/.libs/libntfs-3g.so.83.0.0 $(MODEL_FS_PATH)/lib/
	cd $(MODEL_FS_PATH)/lib/ && ln -sf libntfs-3g.so.83.0.0 libntfs-3g.so.83
	cd $(MODEL_FS_PATH)/lib/ && ln -sf libntfs-3g.so.83.0.0 libntfs-3g.so
#For upgrading to samba 3.6.25. Delete this code block. By YuChuwei-20170622
#ifeq ($(INCLUDE_USB_SAMBA_SERVER), y)
#	cp -p $(PUBLIC_APPS_PATH)/samba-3.0.14a/source/bin/libbigballofmud.so $(MODEL_FS_PATH)/lib/
#	cd $(MODEL_FS_PATH)/lib/ && ln -sf libbigballofmud.so libbigballofmud.so.0
#	# support Turkish shared folder, maybe other languages can be supported also
#ifeq ($(filter $(MODEL2), $(FLASH_RESTRICT_MODELS)),)
#	cp -p $(PUBLIC_APPS_PATH)/samba-3.0.14a/source/codepages/*.dat $(MODEL_FS_PATH)/etc/samba/
#endif
#endif
endif
	#end by zjj
 
ifeq ($(INCLUDE_USB_3G_DONGLE),y)
	cp --no-dereference -p $(PUBLIC_APPS_PATH)/libusb-1.0.18/install/lib/libusb-1.0.so* $(MODEL_FS_PATH)/lib/
	cp --no-dereference -p $(PUBLIC_APPS_PATH)/libusb-compat-0.1.5/install/lib/libusb-0.1.so* $(MODEL_FS_PATH)/lib/
	cp --no-dereference -p $(PUBLIC_APPS_PATH)/libusb-compat-0.1.5/install/lib/libusb.so $(MODEL_FS_PATH)/lib/
endif #INCLUDE_USB_3G_DONGLE
 
ifeq ($(INCLUDE_IPSEC), y)	
	cp -p $(PUBLIC_APPS_PATH)/ipsectools/src/racoon/.libs/libracoon.so.0.0.0 $(MODEL_FS_PATH)/lib/
	cd $(MODEL_FS_PATH)/lib/ && ln -sf libracoon.so.0.0.0 libracoon.so.0
	cd $(MODEL_FS_PATH)/lib/ && ln -sf libracoon.so.0.0.0 libracoon.so
	cp -p $(PUBLIC_APPS_PATH)/ipsectools/src/libipsec/.libs/libipsec.so.0.0.1 $(MODEL_FS_PATH)/lib/
	cd $(MODEL_FS_PATH)/lib/ && ln -sf libipsec.so.0.0.1 libipsec.so.0
	cd $(MODEL_FS_PATH)/lib/ && ln -sf libipsec.so.0.0.1 libipsec.so
endif
 
ifeq ($(strip $(SUPPLIER)),broadcom)
	cp $(BUILD_DIR)/userspace/private/libs/xdslctl/libxdslctl.so $(MODEL_FS_PATH)/lib
	cp $(BUILD_DIR)/userspace/private/libs/vlanctl/libvlanctl.so $(MODEL_FS_PATH)/lib
ifneq ($(strip $(BUILD_FAPCTL)),)
	cd $(BUILD_DIR)/userspace/private/libs/fapctl && cp libfapctl.so $(MODEL_FS_PATH)/lib 
endif
ifneq ($(strip $(BUILD_FCCTL)),)
	cd $(BUILD_DIR)/userspace/private/libs/fcctl && cp libfcctl.so $(MODEL_FS_PATH)/lib 
endif
ifneq ($(strip $(BUILD_IQCTL))_$(strip $(INCLUDE_VOIP)),_)
	cd $(BUILD_DIR)/userspace/private/libs/iqctl && cp libiqctl.so $(MODEL_FS_PATH)/lib 
endif
	cp $(INSTALL_DIR)/lib/*.so $(MODEL_FS_PATH)/lib
ifneq ($(strip $(BUILD_SWMDK)),)
ifneq ($(strip $(BRCM_EXT_SWITCH)),)
	test -d $(INSTALL_DIR)/lib/private && cp $(INSTALL_DIR)/lib/private/lib*.so $(MODEL_FS_PATH)/lib
else
	test -d $(INSTALL_DIR)/lib/private && cp $(INSTALL_DIR)/lib/private/libethswctl.so $(MODEL_FS_PATH)/lib
endif
else
ifneq ($(strip $(BUILD_ETHSWCTL)),)
	test -d $(INSTALL_DIR)/lib/private && cp $(INSTALL_DIR)/lib/private/libethswctl.so $(MODEL_FS_PATH)/lib
endif
endif
ifneq ($(strip $(INCLUDE_VOIP)),)
	cp $(INSTALL_DIR)/lib/private/libbos.so $(MODEL_FS_PATH)/lib
endif
endif
ifeq ($(INCLUDE_LIBPCAP), y)
	cp --no-dereference $(PUBLIC_APPS_PATH)/libpcap-1.8.1/install/lib/libpcap.so* $(MODEL_FS_PATH)/lib/
endif
ifeq ($(INCLUDE_TCPDUMP), y)
	cp $(PUBLIC_APPS_PATH)/tcpdump-4.1.1/install/sbin/* $(MODEL_FS_PATH)/usr/bin/
endif

ifeq ($(INCLUDE_SSH_ACCESS), y)
	cp -p $(PUBLIC_APPS_PATH)/zlib-1.2.11/install/lib/*.so* $(MODEL_FS_PATH)/lib/
	cd $(MODEL_FS_PATH)/lib/ && ln -sf libz.so.1.2.11 libz.so.1
	cd $(MODEL_FS_PATH)/lib/ && ln -sf libz.so.1 libz.so
endif


.PHONY: fs_dev
fs_dev:
#	$(TOOLS_PATH)/makedevlinks $(MODEL_FS_PATH)/	


.PHONY:fs_bin
fs_bin:
	
	cd $(PUBLIC_APPS_PATH)/busybox-1.19.2 && $(MAKE) install CONFIG_PREFIX=$(MODEL_FS_PATH)
	cp -p $(PUBLIC_APPS_PATH)/iproute2-2.6.24-rc7/tc/tc $(MODEL_FS_PATH)/usr/bin
	
	cp -p $(PUBLIC_APPS_PATH)/iptables-1.4.17/iptables/xtables-multi $(MODEL_FS_PATH)/usr/bin
	cd $(MODEL_FS_PATH)/usr/bin && ln -s xtables-multi iptables
	
ifeq ($(INCLUDE_IPV6), y)
	cd $(MODEL_FS_PATH)/usr/bin && ln -s xtables-multi ip6tables
endif
ifneq ($(strip $(SUPPLIER)),broadcom)
	cp -p $(PUBLIC_APPS_PATH)/br2684ctl/br2684ctl $(MODEL_FS_PATH)/usr/bin
endif
	cp -p $(PUBLIC_APPS_PATH)/ppp-2.4.5/pppd/pppd $(MODEL_FS_PATH)/usr/sbin
	cp -p $(PUBLIC_APPS_PATH)/ebtables-v2.0.10-4/ebtables $(MODEL_FS_PATH)/usr/bin
	cp -p $(PUBLIC_APPS_PATH)/ebtables-v2.0.10-4/ethertypes $(MODEL_FS_PATH)/etc/
	cp -p $(PUBLIC_APPS_PATH)/IGD/upnpd $(MODEL_FS_PATH)/usr/bin
ifneq ($(strip $(SUPPLIER)),broadcom)
	cp -p $(PUBLIC_APPS_PATH)/linux-atm/arpd/atmarp $(MODEL_FS_PATH)/usr/sbin
	cp -p $(PUBLIC_APPS_PATH)/linux-atm/arpd/atmarpd $(MODEL_FS_PATH)/usr/sbin
endif
	
ifeq ($(INCLUDE_L2TP), y)
	cp -p $(PUBLIC_APPS_PATH)/xl2tpd-1.1.12/xl2tpd $(MODEL_FS_PATH)/usr/sbin
endif

ifeq ($(strip $(CSP)), TTNET)
	cp -p $(PUBLIC_APPS_PATH)/xl2tpd-1.1.12/xl2tpd $(MODEL_FS_PATH)/usr/sbin
endif

ifeq ($(INCLUDE_USB_OVER_IP_TPLINK), y)
	cp -p $(PUBLIC_APPS_PATH)/usbip-0.1.7/usbipd $(MODEL_FS_PATH)/usr/bin
endif


ifeq ($(INCLUDE_USB_FTP_SERVER), y)
	cp -p $(PUBLIC_APPS_PATH)/vsftpd-2.3.2/vsftpd $(MODEL_FS_PATH)/usr/bin
	cp -p $(PUBLIC_APPS_PATH)/vsftpd-2.3.2/vsftpd.conf $(MODEL_FS_PATH)/etc
	cp -p $(PUBLIC_APPS_PATH)/vsftpd-2.3.2/passwd $(MODEL_FS_PATH)/etc/vsftpd_passwd
endif

ifeq ($(INCLUDE_SSH_ACCESS), y)
	cp -p $(PUBLIC_APPS_PATH)/dropbear/dropbearmulti $(MODEL_FS_PATH)/usr/bin
	cd $(MODEL_FS_PATH)/usr/bin && ln -s dropbearmulti dropbear
	cd $(MODEL_FS_PATH)/usr/bin && ln -s dropbearmulti dropbearkey
	cd $(MODEL_FS_PATH)/usr/bin && ln -s dropbearmulti scp
endif
	
ifeq ($(INCLUDE_RIP), y)
	cp -p $(PUBLIC_APPS_PATH)/zebra-0.95a/ripd/ripd $(MODEL_FS_PATH)/usr/sbin
	cp -p $(PUBLIC_APPS_PATH)/zebra-0.95a/zebra/zebra $(MODEL_FS_PATH)/usr/sbin
endif

ifneq ($(strip $(INCLUDE_VOIP)),)

	cp $(APP_PJSIP_DIR)/pjsip-apps/bin/pjsua $(MODEL_FS_PATH)/usr/bin
ifeq ($(INCLUDE_DECT), y)
	cp $(TOP_PATH)/$(SUPPLIER)/build_dir/target-mips_r2_uClibc-0.9.30.1_AC750/voip_dectcli/bin/Dectcli $(MODEL_FS_PATH)/usr/bin
endif
endif	

ifeq ($(INCLUDE_IPV6), y)
	cp -p $(PUBLIC_APPS_PATH)/radvd-1.5/radvd $(MODEL_FS_PATH)/usr/sbin
endif

ifeq ($(INCLUDE_IPV6), y)
	cp -p $(PUBLIC_APPS_PATH)/wide-dhcpv6-20080615/dhcp6s $(MODEL_FS_PATH)/usr/sbin
	cp -p $(PUBLIC_APPS_PATH)/wide-dhcpv6-20080615/dhcp6c $(MODEL_FS_PATH)/usr/sbin
endif	
	
ifeq ($(INCLUDE_IPV6), y)
	cp -p $(PUBLIC_APPS_PATH)/iproute2-2.6.39/ip/ip $(MODEL_FS_PATH)/usr/bin
endif	
	
ifeq ($(INCLUDE_USB_STORAGE),y)
	#for the bins of hotplug, zjj, 20111129

	#end by zjj
	#for the bins of samba and ntfs-3g, zjj, 20111129
	cp -p $(PUBLIC_APPS_PATH)/ntfs-3g_ntfsprogs-2012.1.15/src/.libs/ntfs-3g $(MODEL_FS_PATH)/usr/bin/ntfs-3g
ifeq ($(INCLUDE_USB_SAMBA_SERVER), y)
ifeq ($(INCLUDE_USB_SAMBA4), y)
	cp -p $(USB_SAMBA_DIR)/multicall $(MODEL_FS_PATH)/usr/sbin
	ln -s /var/samba/lib/smb.conf $(MODEL_FS_PATH)/etc/samba/smb.conf
	ln -s multicall $(MODEL_FS_PATH)/usr/sbin/smbd
	ln -s multicall $(MODEL_FS_PATH)/usr/sbin/nmbd
	ln -s multicall $(MODEL_FS_PATH)/usr/sbin/smbpasswd
	ln -s ../sbin/multicall $(MODEL_FS_PATH)/usr/bin/testparm
else
	cp -p $(PUBLIC_APPS_PATH)/samba-3.6.25/source3/bin/samba_multicall $(MODEL_FS_PATH)/usr/bin
	cd $(MODEL_FS_PATH)/usr/bin/ && ln -sf samba_multicall $(MODEL_FS_PATH)/usr/bin/smbd
ifeq ($(INCLUDE_USB_SAMBA_NMBD), y)
	cd $(MODEL_FS_PATH)/usr/bin/ && ln -sf samba_multicall $(MODEL_FS_PATH)/usr/bin/nmbd
endif
	cd $(MODEL_FS_PATH)/usr/bin/ && ln -sf samba_multicall $(MODEL_FS_PATH)/usr/bin/smbpasswd
	cd $(MODEL_FS_PATH)/usr/bin/ && ln -sf samba_multicall $(MODEL_FS_PATH)/usr/bin/smbcontrol
endif
endif #INCLUDE_USB_SAMBA_SERVER
	#end by zjj

#for dlna ,LI CHENGLONG
ifeq ($(INCLUDE_USB_MEDIA_SERVER), y)
	cp -p $(PUBLIC_APPS_PATH)/ushare-1.1a/src/ushare $(MODEL_FS_PATH)/usr/bin
	cp -p $(PUBLIC_APPS_PATH)/ushare-1.1a/scripts/ushare.conf $(MODEL_FS_PATH)/etc/ushare.conf
endif #INCLUDE_USB_MEDIA_SERVER
#end by LI CHENGLONG
else

endif #INCLUDE_USB_STORAGE
	
ifeq ($(INCLUDE_USB_3G_DONGLE),y)
	cp $(PUBLIC_APPS_PATH)/usb-modeswitch-1.2.3/usb_modeswitch $(MODEL_FS_PATH)/usr/bin
	cp $(PUBLIC_APPS_PATH)/ppp-2.4.5/chat/chat $(MODEL_FS_PATH)/usr/bin
endif #INCLUDE_USB_3G_DONGLE
	
ifeq ($(INCLUDE_IPSEC),y)
	cp -p $(PUBLIC_APPS_PATH)/ipsectools/src/racoon/.libs/racoon $(MODEL_FS_PATH)/usr/bin
	cp -p $(PUBLIC_APPS_PATH)/ipsectools/src/setkey/.libs/setkey $(MODEL_FS_PATH)/usr/bin
	cp -p $(PUBLIC_APPS_PATH)/ipsectools/src/racoon/.libs/racoonctl $(MODEL_FS_PATH)/usr/bin
	#copy ipsecVpn in user cmm Makefile
endif

ifeq ($(strip $(SUPPLIER)),mtk)
		
ifeq ($(INCLUDE_LAN_WLAN), y) 
	cp $(TOP_PATH)/$(SUPPLIER)/apps/public/wireless_tools.29/wlNetlinkTool $(MODEL_FS_PATH)/usr/bin
	cp $(TOP_PATH)/$(SUPPLIER)/apps/public/wireless_tools.29/ated_tp $(MODEL_FS_PATH)/usr/bin
	cp $(TOP_PATH)/$(SUPPLIER)/apps/public/wireless_tools.29/iwconfig $(MODEL_FS_PATH)/usr/bin

	chmod +x $(MODEL_FS_PATH)/usr/bin/*
else	#INCLUDE_LAN_WLAN
	@echo no wifi
endif	#INCLUDE_LAN_WLAN
endif	#SUPPLIER-ralink
	
#	cp -f $(TOP_PATH)/$(SUPPLIER)/apps/public/bridge-utils-1.0.6/brctl/brctl $(MODEL_FS_PATH)/usr/sbin/
	

# end--added

ifeq ($(strip $(SUPPLIER)),lantiq)
ifeq ($(INCLUDE_LAN_WLAN), y)
	cp $(INSTALL_WIRELESS)/lib/libiw.so.28 $(MODEL_FS_PATH)/lib/
	cp -p $(INSTALL_WIRELESS)/sbin/* $(MODEL_FS_PATH)/sbin
	cp -p $(INSTALL_WIRELESS)/lib/modules/*.ko $(MODEL_FS_PATH)/lib/modules
	cp -p $(INSTALL_WIRELESS)/lib/*.so $(MODEL_FS_PATH)/lib/
endif	#INCLUDE_LAN_WLAN

ifeq ($(INCLUDE_ADSLWAN), y)
#prepare
	mkdir $(MODEL_FS_PATH)/firmware
	mkdir $(MODEL_FS_PATH)/tmp
	mkdir $(MODEL_FS_PATH)/tmp/pipe
	echo "ramfs /tmp ramfs defaults 0 0" >> $(MODEL_FS_PATH)/etc/fstab
	echo "none /sys sysfs defaults 0 0" >> $(MODEL_FS_PATH)/etc/fstab
#ko
	cp $(SLIB_PATH)/../opt/lantiq/bin/drv_dsl_cpe_api.ko $(MODEL_FS_PATH)/lib/modules
	cp $(SLIB_PATH)/../opt/lantiq/bin/drv_ifxos.ko $(MODEL_FS_PATH)/lib/modules
#lib
	cp $(SLIB_PATH)/../usr/lib/libdti_agent.so.0.0.0 $(MODEL_FS_PATH)/lib
	cd $(MODEL_FS_PATH)/lib/ && ln -sf libdti_agent.so.0.0.0 libdti_agent.so.0
#app bin
	cp $(SLIB_PATH)/../usr/sbin/br2684ctl $(MODEL_FS_PATH)/usr/sbin
	cp $(SLIB_PATH)/../usr/sbin/br2684ctld $(MODEL_FS_PATH)/usr/sbin
	cp $(SLIB_PATH)/../opt/lantiq/bin/dsl_cpe_control $(MODEL_FS_PATH)/usr/sbin
	cp $(SLIB_PATH)/../sbin/ppacmd $(MODEL_FS_PATH)/usr/sbin
	cp $(SLIB_PATH)/../usr/bin/switch_utility $(MODEL_FS_PATH)/usr/sbin
#firmware
	cp $(SLIB_PATH)/../opt/lantiq/bin/adsl.scr $(MODEL_FS_PATH)/firmware
	cp $(SLIB_PATH)/../opt/lantiq/bin/vdsl.scr $(MODEL_FS_PATH)/firmware
	cp $(SLIB_PATH)/../firmware/dsl_wan_setting.sh $(MODEL_FS_PATH)/firmware
	cp $(SLIB_PATH)/../firmware/eth_wan_setting.sh $(MODEL_FS_PATH)/firmware
	cp $(SLIB_PATH)/../opt/lantiq/bin/dsl_cpe_pipe.sh $(MODEL_FS_PATH)/firmware
#	cp -d $(SLIB_PATH)/../firmware/*.bin $(MODEL_FS_PATH)/firmware
ifeq ($(INCLUDE_ANNEXB), y)
	cp -d $(SLIB_PATH)/../firmware/xcpe_533D06_535902_AnnexB.bin $(MODEL_FS_PATH)/firmware
	cd $(MODEL_FS_PATH)/firmware/ && ln -s xcpe_533D06_535902_AnnexB.bin xcpe_hw.bin
else
	cp -d $(SLIB_PATH)/../firmware/dsl_vr9_firmware_xdsl-05.04.08.00.00.06_05.04.04.04.00.01.bin $(MODEL_FS_PATH)/firmware
	cd $(MODEL_FS_PATH)/firmware/ && ln -s dsl_vr9_firmware_xdsl-05.04.08.00.00.06_05.04.04.04.00.01.bin xcpe_hw.bin
endif
	cp -d $(SLIB_PATH)/../firmware/xdslled.sh $(MODEL_FS_PATH)/firmware
endif	#INCLUDE_ADSLWAN

endif	#SUPPLIER-lantiq

ifeq ($(INCLUDE_DNS_PROXY), y)
	cp -p $(PUBLIC_APPS_PATH)/dnsmasq-2.80/src/dnsmasq $(MODEL_FS_PATH)/usr/sbin
endif

ifeq ($(strip $(CSP)), TTNET)
	cp -a --no-dereference $(PUBLIC_APPS_PATH)/curl-7.65.1/install/lib/libcurl.so* $(MODEL_FS_PATH)/lib/	
	cp -p $(PUBLIC_APPS_PATH)/curl-7.65.1/install/bin/curl $(MODEL_FS_PATH)/usr/sbin
endif

ifeq ($(strip $(CSP)), TTNET)
	cp -p $(PUBLIC_APPS_PATH)/traceroute-2.1.0/traceroute/traceroute $(MODEL_FS_PATH)/usr/sbin
endif

ifeq ($(INCLUDE_STRACE), y)
	cp -p $(PUBLIC_APPS_PATH)/strace-4.8/strace $(MODEL_FS_PATH)/usr/sbin
endif

ifeq ($(INCLUDE_SSL), y)
	cp $(OPENSSL_DIR)/apps/openssl $(MODEL_FS_PATH)/usr/sbin
endif


.PHONY:fs_sysfiles
fs_sysfiles: 


.PHONY:fs_rootfs
fs_rootfs: fakeroot hosttools get_kernel_size
	find  $(MODEL_FS_PATH) -name ".svn" | xargs rm -rf
	find  $(MODEL_FS_PATH) -name ".gitignore" | xargs rm -rf
ifneq ($(strip $(INCLUDE_OAM)),y)
	@rm -f $(MODEL_FS_PATH)/lib/modules/kmdir/extra/*TMS*.ko
	@rm -f $(MODEL_FS_PATH)/usr/sbin/tms*
endif
ifeq ($(strip $(SUPPLIER)),broadcom)
	rm -f $(MODEL_FS_PATH)/lib/modules/kmdir/extra/wlobj*.ko
	rm -f $(MODEL_FS_PATH)/lib/modules/kmdir/extra/dhdobj*.ko
endif
ifeq ($(strip $(SUPPLIER)),broadcom)
ifneq ($(strip $(BRCM_KERNEL_ROOTFS)),squashfs)
# create a file in the fs/etc to contain the KERNEL_TAG
	@if test -e $(CONFFILE_PATH)/oem/$(MODEL)/MFG_conf.bin; \
	then $(TOOLS_PATH)/mktag_jffs2 -m $(KERNEL_MAX_SIZE) -T jffs2 -s 0x1000000 -t $(MTD_PART_TYPE) -p $(MODEL_FS_PATH)/../reduced_data_model_plaintext_$(MODEL).xml -k $(KERNEL_TG_PATH)/linux.7z -v $(KERNEL_TG_PATH)/vmlinux -M $(CONFFILE_PATH)/oem/$(MODEL)/MFG_conf.bin -i $(TARGETS_PATH)/fs.$(MODEL); \
	else $(TOOLS_PATH)/mktag_jffs2 -m $(KERNEL_MAX_SIZE) -T jffs2 -s 0x1000000 -t $(MTD_PART_TYPE) -p $(MODEL_FS_PATH)/../reduced_data_model_plaintext_$(MODEL).xml -k $(KERNEL_TG_PATH)/linux.7z -v $(KERNEL_TG_PATH)/vmlinux -i $(TARGETS_PATH)/fs.$(MODEL); \
	fi
endif
endif


.PHONY:fakeroot
fakeroot: $(FAKEROOT_PATH)/.configured
	@sed -e 's,^PREFIX=.*,PREFIX=$(FAKEROOT_PATH),g' -e 's,^BINDIR=.*,BINDIR=$(FAKEROOT_PATH),g' -e 's,^PATHS=.*,PATHS=$(FAKEROOT_PATH)/.libs:/lib:/usr/lib,g' $(FAKEROOT_PATH)/scripts/fakeroot > $(FAKEROOT_PATH)/fakeroot
	@chmod u+x $(FAKEROOT_PATH)/fakeroot

.PHONY:hosttools
hosttools:
ifeq ($(strip $(SUPPLIER)),broadcom)
	cd $(BUILD_DIR)/hostTools && $(MAKE) BRCM_KERNEL_ROOTFS=$(BRCM_KERNEL_ROOTFS) 
endif
	
####################################################
#
# Clear object               
#
###################################################
clean:apps_clean
	@echo "Clear All Object"
	rm -f $(LAST_MODEL_COOKIE)
ifeq ($(strip $(SUPPLIER)),broadcom)
	rm -f $(LAST_PROFILE_COOKIE)
endif

.PHONY:apps_clean
apps_clean:pjsip_clean public_apps_clean

.PHONY:public_apps_clean
public_apps_clean:
	$(MAKE) -C $(PUBLIC_APPS_PATH)/busybox-1.19.2 clean
	$(MAKE) -C $(PUBLIC_APPS_PATH)/iproute2-2.6.24-rc7  clean
	$(MAKE) -C $(PUBLIC_APPS_PATH)/iptables-1.4.17 clean
ifneq ($(strip $(SUPPLIER)),broadcom)
	$(MAKE) -C $(PUBLIC_APPS_PATH)/br2684ctl clean
endif
	$(MAKE) -C $(PUBLIC_APPS_PATH)/ebtables-v2.0.10-4 clean
	$(MAKE) -C $(PUBLIC_APPS_PATH)/ppp-2.4.5/pppd clean
	$(MAKE) -C $(PUBLIC_APPS_PATH)/linux-atm clean
	$(MAKE) -C $(PUBLIC_APPS_PATH)/upnp_libs clean
	$(MAKE) -C $(PUBLIC_APPS_PATH)/IGD clean
	$(MAKE) -C $(PUBLIC_APPS_PATH)/os_libs clean

ifeq ($(INCLUDE_TCPDUMP), y)
	$(MAKE) -C $(PUBLIC_APPS_PATH)/tcpdump-4.1.1 clean
endif

ifeq ($(INCLUDE_SSL), y)
	$(MAKE) -C $(OPENSSL_DIR)/ clean
endif
	
ifeq ($(INCLUDE_USB_OVER_IP_TPLINK), y)
	$(MAKE) -C $(PUBLIC_APPS_PATH)/usbip-0.1.7 clean
endif

ifeq ($(INCLUDE_USB_FTP_SERVER), y)
	$(MAKE) -C $(PUBLIC_APPS_PATH)/vsftpd-2.3.2 clean
endif

ifeq ($(INCLUDE_SSH_ACCESS), y)
	@if test -e $(PUBLIC_APPS_PATH)/zlib-1.2.11/Makefile; \
	then cd $(PUBLIC_APPS_PATH)/zlib-1.2.11 && $(MAKE) clean && rm Makefile; \
	fi
	@if test -e $(PUBLIC_APPS_PATH)/dropbear/Makefile; \
	then cd $(PUBLIC_APPS_PATH)/dropbear && $(MAKE) clean && rm Makefile; \
	fi
endif

ifeq ($(INCLUDE_RIP), y)
	$(MAKE) -C $(PUBLIC_APPS_PATH)/zebra-0.95a clean
endif

ifeq ($(INCLUDE_IPV6), y)
	@if test -e $(PUBLIC_APPS_PATH)/radvd-1.5/Makefile; \
	then cd $(PUBLIC_APPS_PATH)/radvd-1.5 && $(MAKE) clean && rm Makefile; \
	fi
endif

ifeq ($(INCLUDE_IPV6), y)
	$(MAKE) -C $(PUBLIC_APPS_PATH)/wide-dhcpv6-20080615 clean
endif

ifeq ($(INCLUDE_IPV6), y)
	@if test -e $(PUBLIC_APPS_PATH)/flex-2.5.35/Makefile; \
	then cd $(PUBLIC_APPS_PATH)/flex-2.5.35 && $(MAKE) clean && rm Makefile; \
	fi
	rm -rf $(PUBLIC_APPS_PATH)/lib
endif
	
ifeq ($(INCLUDE_IPV6), y)
	$(MAKE) -C $(PUBLIC_APPS_PATH)/iproute2-2.6.39 clean
endif

ifeq ($(INCLUDE_USB_STORAGE), y)
	#for the clean of samba and ntfs-3g, zjj, 20111129
ifeq ($(INCLUDE_USB_SAMBA_SERVER), y)
ifeq ($(INCLUDE_USB_SAMBA4), y)
	cd $(USB_SAMBA_DIR) && $(MAKE) clean
else
	cd $(PUBLIC_APPS_PATH)/samba-3.6.25/source3 && $(MAKE) clean
endif
endif	#INCLUDE_USB_SAMBA_SERVER
	cd $(PUBLIC_APPS_PATH)/ntfs-3g_ntfsprogs-2012.1.15 && $(MAKE) clean
	#end by zjj
else

endif	#INCLUDE_USB_STORAGE

ifeq ($(INCLUDE_USB_3G_DONGLE), y)
	cd $(PUBLIC_APPS_PATH)/ppp-2.4.5/chat && $(MAKE) clean
	cd $(PUBLIC_APPS_PATH)/usb-modeswitch-1.2.3 && $(MAKE) clean
	cd $(PUBLIC_APPS_PATH)/libusb-1.0.18 && $(MAKE) clean && rm -rf Makefile install/*
	cd $(PUBLIC_APPS_PATH)/libusb-compat-0.1.5 && $(MAKE) clean && rm -rf Makefile install/*
endif

#for dlna , LI CHENGLONG
ifeq ($(INCLUDE_USB_MEDIA_SERVER), y)
	$(MAKE) -C $(PUBLIC_APPS_PATH)/ffmpeg-0.6 clean
	$(MAKE) -C $(PUBLIC_APPS_PATH)/libdlna-0.2.3 clean
	$(MAKE) -C $(PUBLIC_APPS_PATH)/ushare-1.1a clean
endif
#end by LI CHENGLONG


ifeq ($(INCLUDE_IPSEC), y)
	@if test -e $(PUBLIC_APPS_PATH)/ipsectools/Makefile; \
	then cd $(PUBLIC_APPS_PATH)/ipsectools && $(MAKE) clean && rm Makefile; \
	fi
	cd $(OPENSSL_DIR)/ && $(MAKE) clean
	cd $(OPENSSL_DIR)/ && rm -rf ./lib
	rm -rf $(PUBLIC_APPS_PATH)/myipsec 
	rm -rf $(PUBLIC_APPS_PATH)/myssl
endif

ifeq ($(INCLUDE_STRACE), y)
	cd $(PUBLIC_APPS_PATH)/strace-4.8 && $(MAKE) clean 
endif

ifeq ($(INCLUDE_ADSLWAN), y)


ifeq ($(strip $(SUPPLIER)),lantiq)
	#cd $(TOP_PATH)/$(SUPPLIER) && make package/feeds/feed_opensource_apps/linux-atm/clean
	#cd $(TOP_PATH)/$(SUPPLIER) && make package/feeds/ifx_common/lib-dti/clean
	#cd $(TOP_PATH)/$(SUPPLIER) && make package/feeds/ifx_dsl_cpe/ifx-dsl-cpe-api-vrx/clean
	#cd $(TOP_PATH)/$(SUPPLIER) && make package/feeds/ifx_dsl_cpe/ifx-dsl-cpe-control-vrx/clean
	#cd $(TOP_PATH)/$(SUPPLIER) && make package/feeds/ifx_dsl_cpe/ifx-dsl-vr9-firmware-xdsl/clean
endif

else	#INCLUDE_ADSLWAN
	@echo no dsl_app
endif	#INCLUDE_ADSLWAN

ifeq ($(INCLUDE_L2TP), y)
	cd $(PUBLIC_APPS_PATH)/xl2tpd-1.1.12 && $(MAKE) clean
endif
ifeq ($(strip $(CSP)), TTNET)
	cd $(PUBLIC_APPS_PATH)/xl2tpd-1.1.12 && $(MAKE) clean
endif

ifeq ($(INCLUDE_DNS_PROXY), y)
	cd $(PUBLIC_APPS_PATH)/dnsmasq-2.80 && $(MAKE) clean
endif

ifeq ($(strip $(CSP)), TTNET)
	cd $(PUBLIC_APPS_PATH)/curl-7.65.1 && $(MAKE) clean
endif

ifeq ($(strip $(CSP)), TTNET)
	cd $(PUBLIC_APPS_PATH)/traceroute-2.1.0 && $(MAKE) clean env=yes
endif

	
.PHONY: pjsip_clean
pjsip_clean:
ifneq ($(strip $(INCLUDE_VOIP)),)
	rm -f $(PUBLIC_APPS_PATH)/pjsip_1.10/config_cache_file
	$(MAKE) -C $(PUBLIC_APPS_PATH)/pjsip_1.10 clean
else	
	@echo "skipped sip_clean"
endif



