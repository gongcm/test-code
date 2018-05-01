#
# Copyright (C) 2011 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
include device/hisilicon/$(TARGET_PRODUCT)/customer.mk

ifndef PRODUCT_TARGET
PRODUCT_TARGET := aosp
endif

#HiPlayer video output
PRODUCT_PROPERTY_OVERRIDES += \
    service.media.hiplayer.output=vo

#HiPlayer: video frame counts to be played before underrun while seeked
ifeq ($(strip $(PRODUCT_TARGET)),shcmcc)
PRODUCT_PROPERTY_OVERRIDES += \
    service.media.hiplayer.vfcnt=200
endif

#HiPlayer: for vo switch channel freeze
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.hiplayer.vo.freeze=false
#if open HiPlayer: for vo switch channel freeze

ifeq ($(PRODUCT_TARGET),shcmcc)
PRODUCT_PROPERTY_OVERRIDES += \
    ro.media.timeshift=1
PRODUCT_PROPERTY_OVERRIDES += \
    client.apk.name= wasu.app
PRODUCT_PROPERTY_OVERRIDES += \
    sys.settings.support=1
PRODUCT_PROPERTY_OVERRIDES += \
    sys.settings.support.net.flags=7
PRODUCT_PROPERTY_OVERRIDES += \
    sys.deepdiagnose.support=1
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.timezone=Asia/Shanghai
endif

#wifi disguise
ifeq ($(strip $(PRODUCT_TARGET)),telecom)
PRODUCT_PROPERTY_OVERRIDES += \
    persist.ethernet.wifidisguise=false
else ifeq ($(strip $(PRODUCT_TARGET)),unicom)
PRODUCT_PROPERTY_OVERRIDES += \
	persist.ethernet.wifidisguise=false
else ifeq ($(PRODUCT_TARGET),shcmcc)
PRODUCT_PROPERTY_OVERRIDES += \
	persist.ethernet.wifidisguise=false
else
PRODUCT_PROPERTY_OVERRIDES += \
    persist.ethernet.wifidisguise=true
endif

# ethernet switch status: eth0_on eth0_down
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.ethernet.status=eth0_on

# open 2D drawing
ifeq ($(PRODUCT_TARGET),shcmcc)
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.ui.hw=false
else
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.ui.hw=true
endif

#setup UI density
ifeq ($(strip $(PRODUCT_TARGET)),shcmcc)
PRODUCT_PROPERTY_OVERRIDES += \
     ro.sf.lcd_density=160
else ifeq ($(strip $(PRODUCT_TARGET)),telecom)
PRODUCT_PROPERTY_OVERRIDES += \
     ro.sf.lcd_density=160
else ifeq ($(strip $(PRODUCT_TARGET)),unicom)
PRODUCT_PROPERTY_OVERRIDES += \
     ro.sf.lcd_density=160
else
PRODUCT_PROPERTY_OVERRIDES += \
     ro.sf.lcd_density=240
endif

ifeq ($(strip $(PRODUCT_TARGET)), telecom)
UEVENTD_TYPE_NAME := telecom
else
UEVENTD_TYPE_NAME := default
endif

ifeq ($(strip $(PRODUCT_TARGET)), aosp)
PRODUCT_PROPERTY_OVERRIDES += \
    ro.product.target=aosp
endif
ifeq ($(strip $(PRODUCT_TARGET)), shcmcc)
PRODUCT_PROPERTY_OVERRIDES += \
    ro.product.target=shcmcc
endif
ifeq ($(strip $(PRODUCT_TARGET)), telecom)
PRODUCT_PROPERTY_OVERRIDES += \
    ro.product.target=telecom
endif

ifeq ($(strip $(PRODUCT_TARGET)), demo)
PRODUCT_PROPERTY_OVERRIDES += \
    ro.product.target=demo
endif

TARGET_BOARD_PLATFORM := bigfish

PRODUCT_PROPERTY_FOR_DRM_SERVICE := true
ifeq ($(strip $(PRODUCT_PROPERTY_FOR_DRM_SERVICE)), true)
# add drm enable property for PlayReady/Widevine
PRODUCT_PROPERTY_OVERRIDES += drm.service.enabled=true
else
PRODUCT_PROPERTY_OVERRIDES += drm.service.enabled=false
endif

PRODUCT_PROPERTY_FOR_DRM_CLIENT := false
ifeq ($(strip $(PRODUCT_PROPERTY_FOR_DRM_CLIENT)), true)
# add drm enable property for PlayReady/Widevine
PRODUCT_PROPERTY_OVERRIDES += drm.client.enabled=true
else
PRODUCT_PROPERTY_OVERRIDES += drm.client.enabled=false
endif

# Configure Widevine Stream Cache Size for HD content
PRODUCT_PROPERTY_OVERRIDES += ro.com.widevine.cachesize=16777216

# widevine debug fuse
PRODUCT_PROPERTY_OVERRIDES += drm.widevine.debug=false

ifeq ($(strip $(BOARD_QBSUPPORT)), true)
# isFirstBoot
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.firstboot.flag=true
# FastBoot
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.qb.flag=true
# quickboot enable
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.qb.enable=true

PRODUCT_COPY_FILES += \
    device/hisilicon/bigfish/etc/snapshot:root/sbin/snapshot \
    device/hisilicon/bigfish/etc/andsnap:root/sbin/andsnap
endif

# Optimized Boot
ifeq ($(OPTIMIZED_BOOT_SUPPORT),true)
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.laterscan.enable=true \
    persist.sys.preload.fork=true \
    persist.sys.zygote.optimize=true
    ## pms installer
    # persist.sys.boot.optimize=true
endif

PRODUCT_PROPERTY_OVERRIDES += \
    wifi.interface=wlan0

# For application to get the OpenGL ES version
PRODUCT_PROPERTY_OVERRIDES += \
    ro.opengles.version=131072

# When set to TRUE this flag sets EGL_SWAP_BEHAVIOR_PRESERVED_BIT in eglSwapBuffers
# which will end up preserving the whole frame causing a significant increase in memory bandwidth and decrease in performance
PRODUCT_PROPERTY_OVERRIDES += \
    debug.hwui.render_dirty_regions=false

# set ro.kernel.android.checkjni false for apk compatibility
PRODUCT_PROPERTY_OVERRIDES += \
    ro.kernel.android.checkjni=false

# Disable lockscreen by default
PRODUCT_PROPERTY_OVERRIDES += \
    ro.lockscreen.disable.default=true

# Disable hardware menu key
PRODUCT_PROPERTY_OVERRIDES += \
    qemu.hw.mainkeys=0

# MediaScanner
PRODUCT_PROPERTY_OVERRIDES += \
    ro.mediaScanner.enable=false

# DEFAULT STORAGE
PRODUCT_PROPERTY_OVERRIDES += \
    ro.defaultStorage.enable=true

# DLNA&Skyplay Toast
PRODUCT_PROPERTY_OVERRIDES += \
    ro.toastshow.enable=false

# fuse
PRODUCT_PROPERTY_OVERRIDES += \
    ro.fuse.support=false

# Dobly DMA Certification
PRODUCT_PROPERTY_OVERRIDES += \
    ro.dolby.dmacert.enable=false

# Dobly IPTV Certification
PRODUCT_PROPERTY_OVERRIDES += \
    ro.dolby.iptvcert.enable=false

# Dobly DVB Certification
PRODUCT_PROPERTY_OVERRIDES += \
    ro.dolby.dvbcert.enable=false

# add release version
PRODUCT_PROPERTY_OVERRIDES += \
    gsm.version.baseband=HiSTBAndroidV600_Version

PRODUCT_PROPERTY_OVERRIDES += \
    hibrowser.default.fullscreen=true

#support hwcursor
PRODUCT_PROPERTY_OVERRIDES += \
    ro.hwcursor.fb=/dev/graphics/fb2

#samba status
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.samba.status=true

#for karaoke
PRODUCT_PROPERTY_OVERRIDES += \
    persist.karaoke.enable=true

#for media soft detector
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.softdetector.enable=false

# Screen orientation: landscape or portrait
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.screenorientation=landscape

# enable PowerControl
PRODUCT_PROPERTY_OVERRIDES += \
    ro.pmqos.enable=true

# patch name
PRODUCT_PROPERTY_OVERRIDES += \
    ro.product.patch=SPC060+patch13

PRODUCT_PACKAGES += \
     libandroid_qb

# enable Widevine drm
PRODUCT_PACKAGES += \
    com.google.widevine.software.drm.xml \
    com.google.widevine.software.drm \
    libdrmwvmplugin libwvm libdrmdecrypt  \
    libWVStreamControlAPI_L$(BOARD_WIDEVINE_OEMCRYPTO_LEVEL) \
    libwvdrm_L$(BOARD_WIDEVINE_OEMCRYPTO_LEVEL)

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml 

PRODUCT_COPY_FILES += \
    device/hisilicon/bigfish/etc/init.default.rc:root/init.rc \
    device/hisilicon/bigfish/etc/init.hidolphin.rc:root/init.hidolphin.rc \
    device/hisilicon/bigfish/etc/ueventd.bigfish.${UEVENTD_TYPE_NAME}.rc:root/ueventd.bigfish.rc \
    device/hisilicon/bigfish/etc/media_codecs.xml:/system/etc/media_codecs.xml \
    device/hisilicon/bigfish/etc/media_profiles.xml:/system/etc/media_profiles.xml \
    device/hisilicon/bigfish/etc/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml

PRODUCT_COPY_FILES += \
    device/hisilicon/${CHIPNAME}/etc/init.${CHIPNAME}.rc:root/init.bigfish.rc \
    device/hisilicon/${CHIPNAME}/etc/init.${CHIPNAME}.sh:system/etc/init.bigfish.sh

# audio
PRODUCT_COPY_FILES += \
    device/hisilicon/bigfish/etc/audio_policy.conf:system/etc/audio_policy.conf \
    device/hisilicon/bigfish/etc/asound.conf:system/etc/asound.conf \
    device/hisilicon/bigfish/etc/alsa.conf:system/usr/share/alsa/alsa.conf

# pppoe
PRODUCT_COPY_FILES += \
    device/hisilicon/bigfish/etc/pppoe/ppp.conf:system/etc/ppp/ppp.conf \
    device/hisilicon/bigfish/etc/pppoe/ppp.connect:system/etc/ppp/ppp.connect \
    device/hisilicon/bigfish/etc/pppoe/ppp.disconnect:system/etc/ppp/ppp.disconnect

# nxp nfc
PRODUCT_PACKAGES += \
    libnfc-nci \
    libnfc_nci_jni \
    nfc_nci_pn54x.default\
    NfcNci \
    Tag \
    com.android.nfc_extras

#nxp nfc PRODUCT_COPY_FILES
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/com.nxp.mifare.xml:system/etc/permissions/com.nxp.mifare.xml \
    frameworks/native/data/etc/com.android.nfc_extras.xml:system/etc/permissions/com.android.nfc_extras.xml \
    frameworks/native/data/etc/android.hardware.nfc.xml:system/etc/permissions/android.hardware.nfc.xml \
    frameworks/native/data/etc/android.hardware.nfc.hce.xml:system/etc/permissions/android.hardware.nfc.hce.xml

NFC_CONFIG_PATCH := device/hisilicon/Hi3798MV200/libnfc-brcm.conf
PRODUCT_COPY_FILES += \
    $(NFC_CONFIG_PATCH):system/etc/libnfc-brcm.conf
    
NFC_CONFIG_NXP_PATCH := device/hisilicon/Hi3798MV200/libnfc-nxp.conf
PRODUCT_COPY_FILES += \
    $(NFC_CONFIG_NXP_PATCH):system/etc/libnfc-nxp.conf

#NFC_FW_PATCH := device/hisilicon/Hi3798MV200/libpn547_fw.so
#PRODUCT_COPY_FILES += \
#    $(NFC_FW_PATCH):system/vendor/firmware/libpn547ad_fw.so
    
NFC_DRIVER_PATCH := device/hisilicon/Hi3798MV200/hi_nfc.ko
PRODUCT_COPY_FILES += \
    $(NFC_DRIVER_PATCH):system/lib/modules/hi_nfc.ko



# Wifi
PRODUCT_COPY_FILES += \
    device/hisilicon/bigfish/etc/wifi/dhcpcd.conf:system/etc/dhcpcd/dhcpcd.conf \
    device/hisilicon/bigfish/etc/wifi/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf \
    device/hisilicon/bigfish/etc/wifi/p2p_supplicant.conf:system/etc/wifi/p2p_supplicant.conf \
    device/hisilicon/bigfish/etc/wifi/iperf:system/bin/iperf \
    device/hisilicon/bigfish/etc/wifi/iwlist:system/bin/iwlist \
    device/hisilicon/bigfish/etc/wifi/iwpriv:system/bin/iwpriv \
    device/hisilicon/bigfish/etc/wifi/iwconfig:system/bin/iwconfig \
    device/hisilicon/bigfish/etc/wifi/76x2_supplicant:system/bin/76x2_supplicant


PRODUCT_COPY_FILES += \
    device/hisilicon/bigfish/etc/restore:system/etc/restore \
    device/hisilicon/bigfish/etc/exitfastplay:system/etc/exitfastplay \
    device/hisilicon/bigfish/etc/security_l2_num:system/etc/security_l2_num
PRODUCT_COPY_FILES += \
    frameworks/base/cmds/preload/preloadclass.jar:system/framework/preloadclass.jar

#dtvlib_init.sh
PRODUCT_COPY_FILES += device/hisilicon/Hi3798MV200/etc/dtvlib_init.sh:system/etc/dtvlib_init.sh


#permissions.xml
PRODUCT_COPY_FILES += device/hisilicon/Hi3798MV200/etc/permissions.xml:system/etc/permissions/permissions.xml

#bootanimation.zip
PRODUCT_COPY_FILES += device/hisilicon/Hi3798MV200/prebuilts/bootanimation.zip:system/media/bootanimation.zip


PRODUCT_AAPT_PREF_CONFIG := xhdpi

PRODUCT_CHARACTERISTICS := tablet

# for ethernet and pppoe frameworks compile
FRAMEWORKS_BASE_SUBDIRS += \
    $(addsuffix /java, \
        pppoe\
        ethernet\
        g3\
    )

$(call inherit-product, frameworks/native/build/tablet-dalvik-heap.mk)
$(call inherit-product, $(BUILD_DIR)/product_package.mk)
$(call inherit-product-if-exists, device/hisilicon/bigfish/hidolphin/patch/hidolphin.mk)
