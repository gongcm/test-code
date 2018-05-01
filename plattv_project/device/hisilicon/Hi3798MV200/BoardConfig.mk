#
# BoardConfig.mk
# Product-specific compile-time definitions.
#

#################################################################################
##  Variable configuration definition
################################################################################
# Define product line
HISI_PRODUCT_LINE := STB
#HISI_PRODUCT_LINE := DPT

# SDK configure
HISI_CHIPS := hi3798mv200
ifeq ($(strip $(HISI_TARGET_ARCH_SUPPORT)),32only)
HISI_SDK_ANDROID_CFG := hi3798mv2dmo_hi3798mv200_android_cfg.mak
HISI_SDK_SECURE_CFG := hi3798mv2dmo_hi3798mv200_android_security_cfg.mak
HISI_SDK_TEE_CFG := hi3798mv2dma_hi3798mv200_android_tee_cfg.mak
HISI_SDK_ANDROID_VMX_CFG :=
ifeq ($(strip $(VMX_ADVANCED_SUPPORT)),true)
HISI_SDK_RECOVERY_CFG :=
else
HISI_SDK_RECOVERY_CFG := hi3798mv2dmo_hi3798mv200_android_recovery_cfg.mak
endif
HISI_SDK_ANDROID_APPLOADER_CFG :=
HISI_SDK_SECURE_APPLOADER_CFG :=
HISI_SDK_TEE_APPLOADER_CFG :=
HISI_SDK_ANDROID_VMX_APPLOADER_CFG :=
HISI_APPLOADER_IMG_CFG :=

else #HISI_TARGET_ARCH_SUPPORT

HISI_SDK_ANDROID_CFG :=
HISI_SDK_SECURE_CFG :=
HISI_SDK_TEE_CFG :=
HISI_SDK_ANDROID_VMX_CFG :=
ifeq ($(strip $(VMX_ADVANCED_SUPPORT)),true)
HISI_SDK_RECOVERY_CFG :=
else
HISI_SDK_RECOVERY_CFG :=
endif

endif

SDK_CFG_DIR := configs/$(HISI_CHIPS)
ifeq ($(strip $(TARGET_HAVE_APPLOADER)),false)
ifeq ($(strip $(HISILICON_SECURITY_L2)),true)
ifeq ($(strip $(HISILICON_TEE)),true)
SDK_CFGFILE := $(SDK_CFG_DIR)/$(HISI_SDK_TEE_CFG)
else
SDK_CFGFILE := $(SDK_CFG_DIR)/$(HISI_SDK_SECURE_CFG)
endif
else
ifeq ($(strip $(VMX_ADVANCED_SUPPORT)),true)
SDK_CFGFILE := $(SDK_CFG_DIR)/$(HISI_SDK_ANDROID_VMX_CFG)
else
SDK_CFGFILE := $(SDK_CFG_DIR)/$(HISI_SDK_ANDROID_CFG)
endif
endif

else #TARGET_HAVE_APPLOADER

ifeq ($(strip $(HISILICON_SECURITY_L2)),true)
ifeq ($(strip $(HISILICON_TEE)),true)
SDK_CFGFILE := $(SDK_CFG_DIR)/$(HISI_SDK_TEE_APPLOADER_CFG)
else
SDK_CFGFILE := $(SDK_CFG_DIR)/$(HISI_SDK_SECURE_APPLOADER_CFG)
endif
else
ifeq ($(strip $(VMX_ADVANCED_SUPPORT)),true)
SDK_CFGFILE := $(SDK_CFG_DIR)/$(HISI_SDK_ANDROID_VMX_APPLOADER_CFG)
else
SDK_CFGFILE := $(SDK_CFG_DIR)/$(HISI_SDK_ANDROID_APPLOADER_CFG)
endif
endif
endif

# Kernel configure
RAMDISK_ENABLE := true
ANDROID_KERNEL_CONFIG := hi3798mv200_android_defconfig
RECOVERY_KERNEL_CONFIG := hi3798mv200_android_recovery_defconfig

# fastboot configure
# boot regfiles distinguished by voltage,
# divided into 3.3 V(BOOT_REG_NAME), 2.475 V(BOOT_REG1_NAME), 1.925 V(BOOT_REG2_NAME),
# 1.375 V(BOOT_REG3_NAME), 0.825 V(BOOT_REG4_NAME), 0 V(BOOT_REG5_NAME).
# However, if you choose security boot startup, only a reg table(BOOT_REG_NAME) will take effect.
#
BOOT_REG_NAME := hi3798mv2dma_hi3798mv200_DDR3-1866_1GB_16bitx2_2layers.reg
#BOOT_REG1_NAME := hi3798mv2dmb_hi3798mv200_DDR4-2133_2GB_16bitx2_2layers.reg
BOOT_REG1_NAME := hi3798mv2dmc_hi3798mv200_DDR4-2133_2GB_16bitx2_4layers.reg
BOOT_REG2_NAME :=
BOOT_REG3_NAME :=
BOOT_REG4_NAME :=
BOOT_REG5_NAME :=

# emmc fastboot configure
EMMC_BOOT_ENV_STARTADDR :=0x100000
EMMC_BOOT_ENV_SIZE=0x10000
EMMC_BOOT_ENV_RANGE=0x10000

# nand fastboot configure
NAND_BOOT_ENV_STARTADDR :=0x800000
NAND_BOOT_ENV_SIZE=0x10000
NAND_BOOT_ENV_RANGE=0x10000

#
# ext4 file system configure
# the ext4 file system just use in the eMMC
#
# BOARD_FLASH_BLOCK_SIZE :              we do not need to change it,but needed
# BOARD_SYSTEMIMAGE_PARTITION_SIZE:     system size,
# BOARD_USERDATAIMAGE_PARTITION_SIZE:   userdata size,
# BOARD_CACHEIMAGE_PARTITION_SIZE :     cache size
# BOARD_SDCARDIMAGE_PARTITION_SIZE :    sdcard size
# 524288000 represent 524288000/1024/1024 = 500MB
# system,userdata,cache size should be consistent with the bootargs
#

TARGET_USERIMAGES_USE_EXT4 := true
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 838860800
BOARD_USERDATAIMAGE_PARTITION_SIZE := 3221225472
BOARD_CACHEIMAGE_PARTITION_SIZE := 524288000
BOARD_CACHEIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_PRIVATEIMAGE_PARTITION_SIZE := 52428800
BOARD_BACKUPIMAGE_PARTITION_SIZE := 838860800
BOARD_SECURESTORE_PARTITION_SIZE:= 8388608

BOARD_FLASH_BLOCK_SIZE := 4096
ifeq ($(strip $(HISI_TARGET_ARCH_SUPPORT)),32only)
BOARD_HAVE_BLUETOOTH := true
else
BOARD_HAVE_BLUETOOTH := false
endif

#
# ubifs file system configure
# the ubifs file system just use in Nand Flash
#
# the PAGE_BLOCK_SIZE parameter will be only used
# for making the ubifs img, for example system_4k_1M.ubi
# 4k_1M: 4k: the nand pagesize, 1M:the nand blocksize
#
# if you want make the system.ubi with 2k pagesize and 128k blocksize
# just add 2k_128k at the end of PAGE_BLOCK_SIZE,
# example:
#      PAGE_BLOCK_SIZE:=8k_2M 4k_1M 2k_128k

PAGE_BLOCK_SIZE :=16k_4M 8k_2M 4k_1M

# Enable WiFi module used in the board.
# Supported WiFi modules:
#   RealTek RTL8188EUS (1T1R 2.4G)(Supported mode: STA, AP, WiFi Direct)
#   RealTek RTL8188ETV (1T1R 2.4G)(Supported mode: STA, AP, WiFi Direct)
#   RealTek RTL8192EU  (2T2R 2.4G)(Supported mode: STA, AP, WiFi Direct)
#   RealTek RTL8192EE  (2T2R 2.4G PCI-e)(Supported mode: STA, AP, WiFi Direct)
#   RealTek RTL8812AU  (11ac 2T2R 2.4G+5G)(Supported mode: STA, AP, WiFi Direct)
# Set to 'y', enable the WiFi module, the driver will be compiled.
# Set to 'n', disable the WiFi module, the driver won't be compiled.
# Can set more than one module to 'y'.
# Example:
#   enable RTL8188EUS : BOARD_WLAN_DEVICE_RTL8188EUS = y
#   disable RTL8188EUS: BOARD_WLAN_DEVICE_RTL8188EUS = n

# RTL8188EUS
BOARD_WLAN_DEVICE_RTL8188EUS := y
# RTL8188ETV
BOARD_WLAN_DEVICE_RTL8188ETV := y
# RTL8192EU
BOARD_WLAN_DEVICE_RTL8192EU := n
# RTL8812AU
BOARD_WLAN_DEVICE_RTL8812AU := n
# RTL8192EE
BOARD_WLAN_DEVICE_RTL8192EE := n

#Supported BT Only modules:
# RealTek RTL8761     (BT4.0)
#Supported WiFi + BT Combo modules:
# RealTek RTL8723BU   (1T1R 2.4G + BT4.0)(Supported mode: STA, AP, WiFi Direct)
# RealTek RTL8822BU   (11ac 2T2R + BT4.0)(Supported mode: STA, AP, WiFi Direct)
# MediaTek MT7632TU   (2T2R 2.4G + BT4.0)(Supported mode: STA, AP, WiFi Direct)
# MediaTek MT7662TU   (11ac 2T2R + BT4.0)(Supported mode: STA, AP, WiFi Direct)
# MediaTek MT7662TE   (11ac 2T2R + BT4.0)(Supported mode: STA, AP, WiFi Direct)()PCI-e interface)

#enable BT Only module or WiFi + BT Combo module used in the board
# Set to 'y', enable the BT Only module or WiFi+BT Combo module, the driver will be compiled.
# Set to 'n', disable the BT Only module or WiFi+BT Combo module, the driver won't be compiled.
# Can set more than one module to 'y'.
# Example:
# enable RTL8723BU WiFi+BT : BOARD_BLUETOOTH_WIFI_DEVICE_RTL8723BU := y
# if MT7632TU or MT7662TU is set to y, must modify device\hisilicon\Hi3798MV100\etc\init.Hi3798MV200.rc as follows:
# write /proc/sys/vm/min_free_kbytes 32768 (from 10240 to 32768 to get more memory)

# RTL8761 BT Only
BOARD_BLUETOOTH_DEVICE_RTL8761 := y
# RTL8723BU WiFi+BT Combo
BOARD_BLUETOOTH_WIFI_DEVICE_RTL8723BU := y
# RTL8822BU WiFi+BT Combo
BOARD_BLUETOOTH_WIFI_DEVICE_RTL8822BU := y
# MT7632TU WiFi+BT Combo
BOARD_BLUETOOTH_WIFI_DEVICE_MT7632TU := y
# MT7662TU WiFi+BT Combo
BOARD_BLUETOOTH_WIFI_DEVICE_MT7662TU := y
# MT7662TE
BOARD_BLUETOOTH_WIFI_DEVICE_MT7662TE := y

ifeq ($(BOARD_BLUETOOTH_DEVICE_RTL8761),y)
BOARD_BLUETOOTH_DEVICE_REALTEK := y
BOARD_HAVE_BLUETOOTH_RTK := true
BOARD_HAVE_BLUETOOTH_RTK_IF := usb
endif
ifeq ($(BOARD_BLUETOOTH_WIFI_DEVICE_RTL8723BU),y)
BOARD_BLUETOOTH_DEVICE_REALTEK := y
BOARD_HAVE_BLUETOOTH_RTK := true
BOARD_HAVE_BLUETOOTH_RTK_IF := usb
endif
ifeq ($(BOARD_BLUETOOTH_WIFI_DEVICE_RTL8822BU),y)
BOARD_BLUETOOTH_DEVICE_REALTEK := y
BOARD_HAVE_BLUETOOTH_RTK := true
BOARD_HAVE_BLUETOOTH_RTK_IF := usb
endif
ifeq ($(BOARD_BLUETOOTH_WIFI_DEVICE_MT7632TU),y)
BOARD_BLUETOOTH_WIFI_DEVICE_MT76X2TU := y
endif
ifeq ($(BOARD_BLUETOOTH_WIFI_DEVICE_MT7662TU),y)
BOARD_BLUETOOTH_WIFI_DEVICE_MT76X2TU := y
endif
ifeq ($(BOARD_BLUETOOTH_WIFI_DEVICE_MT7662TE),y)
BOARD_BLUETOOTH_WIFI_DEVICE_MT76X2TU := y
endif

################################################################################
##  Stable configuration definitions
################################################################################

# The generic product target doesn't have any hardware-specific pieces.
TARGET_NO_BOOTLOADER := true
ifeq ($(RAMDISK_ENABLE),false)
TARGET_NO_KERNEL := true
else
TARGET_NO_KERNEL := false
endif
BOARD_KERNEL_BASE :=0x3000000
BOARD_KERNEL_PAGESIZE :=16384
TARGET_NO_RECOVERY := true
TARGET_NO_RADIOIMAGE := true
TARGET_ARCH := arm

# Note: we build the platform images for ARMv7-A _without_ NEON.
#
# Technically, the emulator supports ARMv7-A _and_ NEON instructions, but
# emulated NEON code paths typically ends up 2x slower than the normal C code
# it is supposed to replace (unlike on real devices where it is 2x to 3x
# faster).
#
# What this means is that the platform image will not use NEON code paths
# that are slower to emulate. On the other hand, it is possible to emulate
# application code generated with the NDK that uses NEON in the emulator.
#
ifeq ($(strip $(HISI_TARGET_ARCH_SUPPORT)),32only)
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_VARIANT := cortex-a9
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
else ifeq ($(strip $(HISI_TARGET_ARCH_SUPPORT)),64kernel)
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_VARIANT := cortex-a9
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_USES_64_BIT_BINDER := true
else ifeq ($(strip $(HISI_TARGET_ARCH_SUPPORT)),32prefer)
TARGET_ARCH := arm64
TARGET_ARCH_VARIANT := armv8-a
TARGET_CPU_VARIANT := generic
TARGET_CPU_ABI := arm64-v8a
TARGET_2ND_ARCH := arm
TARGET_2ND_ARCH_VARIANT := armv7-a-neon
TARGET_2ND_CPU_VARIANT := cortex-a9
TARGET_2ND_CPU_ABI := armeabi-v7a
TARGET_2ND_CPU_ABI2 := armeabi
TARGET_USES_64_BIT_BINDER := true
TARGET_PREFER_32_BIT := true
TARGET_SUPPORTS_64_BIT_APPS := true
TARGET_SUPPORTS_32_BIT_APPS := true
PRODUCT_DEFAULT_PROPERTY_OVERRIDES := $(subst zygote32,zygote32_64,$(PRODUCT_DEFAULT_PROPERTY_OVERRIDES))
else ifeq ($(strip $(HISI_TARGET_ARCH_SUPPORT)),64prefer)
TARGET_ARCH := arm64
TARGET_ARCH_VARIANT := armv8-a
TARGET_CPU_VARIANT := generic
TARGET_CPU_ABI := arm64-v8a
TARGET_2ND_ARCH := arm
TARGET_2ND_ARCH_VARIANT := armv7-a-neon
TARGET_2ND_CPU_VARIANT := cortex-a9
TARGET_2ND_CPU_ABI := armeabi-v7a
TARGET_2ND_CPU_ABI2 := armeabi
TARGET_USES_64_BIT_BINDER := true
TARGET_PREFER_64_BIT := true
TARGET_SUPPORTS_64_BIT_APPS := true
TARGET_SUPPORTS_32_BIT_APPS := true
PRODUCT_DEFAULT_PROPERTY_OVERRIDES := $(subst zygote32,zygote64_32,$(PRODUCT_DEFAULT_PROPERTY_OVERRIDES))
endif

ARCH_ARM_HAVE_TLS_REGISTER := true

BOARD_USES_GENERIC_AUDIO := true

# no hardware camera
USE_CAMERA_STUB := true

# Enable dex-preoptimization to speed up the first boot sequence
# of an SDK AVD. Note that this operation only works on Linux for now
#ifeq ($(HOST_OS),linux)
#  ifeq ($(WITH_DEXPREOPT),)
#    WITH_DEXPREOPT := true
#  endif
#endif

# Build OpenGLES emulation guest and host libraries
#BUILD_EMULATOR_OPENGL := true

# Build and enable the OpenGL ES View renderer. When running on the emulator,
# the GLES renderer disables itself if host GL acceleration isn't available.
USE_OPENGL_RENDERER := true

#
#  Hisilicon Platform
#

# Buildin Hisilicon GPU gralloc and GPU libraries.
BUILDIN_HISI_GPU := true
# Buildin Hisilicon NDK extensions
BUILDIN_HISI_EXT := true

# Configure buildin Hisilicon smp
TARGET_CPU_SMP := true

# Disable use system/core/rootdir/init.rc
# HiSilicon use device/hisilicon/bigfish/etc/init.rc
TARGET_PROVIDES_INIT_RC := true

# Configure Board Platform name
TARGET_BOARD_PLATFORM := bigfish
TARGET_BOOTLOADER_BOARD_NAME := bigfish

# Define sdk boot table configures directory
SDK_BOOTCFG_DIR := $(SDK_DIR)/source/boot/sysreg/$(HISI_CHIPS)
# Configure Hisilicon Linux Kernel Version
HISI_LINUX_KERNEL := linux-3.18.y
# Define Hisilicon linux kernel source path.
HISI_KERNEL_SOURCE := $(SDK_DIR)/source/kernel/$(HISI_LINUX_KERNEL)

# wpa_supplicant and hostapd build configuration
# wpa_supplicant is used for WiFi STA, hostapd is used for WiFi SoftAP
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_bcmdhd
BOARD_HOSTAPD_DRIVER := NL80211
BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_bcmdhd
BOARD_WLAN_DEVICE := bcmdhd

# Set /system/bin/sh to mksh, not ash, to test the transition.
TARGET_SHELL := mksh
ENHANCE_APPLICATION_COMPATIBILITY := false
SUPPORT_IPV6 := true

# widevine Level setting
ifeq ($(HISILICON_TEE),true)
BOARD_WIDEVINE_OEMCRYPTO_LEVEL := 1
else
BOARD_WIDEVINE_OEMCRYPTO_LEVEL := 3
endif

