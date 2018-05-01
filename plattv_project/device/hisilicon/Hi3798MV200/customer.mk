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

#apploader config
#true: build system with loader
#false: build system with recovery
TARGET_HAVE_APPLOADER := false

#Product usage selection, possible values are: shcmcc/telecom/aosp/demo
#shcmcc stands for the ShangHai Mobile video base mode.
#telecom stands for the China iptv and cable  centralized procurement mode.
#aosp stands for android ott and oversea project
#Please modify here before compilation
PRODUCT_TARGET := telecom

#Setup SecurityL1
HISILICON_SECURITY_L1 := false

#setup android target arch, such as, 32only/64kernel/32prefer/64prefer
HISI_TARGET_ARCH_SUPPORT := 32only

HISI_WDG_ENABLE :=false

ifeq ($(strip $(HISI_WDG_ENABLE)),true)
PRODUCT_PROPERTY_OVERRIDES += \
    ro.wdg.enable=true
endif
#Setup SecurityL2
HISILICON_SECURITY_L2 := false

#Setup SecurityL3
HISILICON_SECURITY_L3 := false

ifeq ($(HISILICON_SECURITY_L1),true)
PRODUCT_PROPERTY_OVERRIDES += \
    ro.hs.security.l1.enable=$(HISILICON_SECURITY_L1)
ifneq (,$(wildcard device/hisilicon/${CHIPNAME}/security/releasekey.x509.pem))
ifneq (,$(wildcard device/hisilicon/${CHIPNAME}/security/releasekey.pk8))
PRODUCT_DEFAULT_DEV_CERTIFICATE := \
    device/hisilicon/${CHIPNAME}/security/releasekey
else
$(warning device/hisilicon/${CHIPNAME}/security/releasekey.pk8 does not exist!)
endif
else
$(warning device/hisilicon/${CHIPNAME}/security/releasekey.x509.pem does not exist!)
endif
endif

ifeq ($(strip $(HISILICON_SECURITY_L3)),true)
HISILICON_SECURITY_L2 := true
endif

ifeq ($(strip $(HISILICON_SECURITY_L2)),true)
PRODUCT_DEFAULT_DEV_CERTIFICATE := \
    device/hisilicon/${CHIPNAME}/security/releasekey
PRODUCT_PROPERTY_OVERRIDES += \
    ro.hs.security.l1.enable=true
HISILICON_SECURITY_L2_SYSTEM_CHECK := false
endif

# apk control mode: none/whitelist
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sys.apkcontrol.mode=none

#Quick Boot Support
BOARD_QBSUPPORT := false

#Unified update.zip for BGA and QFP fastboots
SUPPORT_UNIFIED_UPDATE := false

#SUPPROT UI for wipe data or update from USB when pressed remote control power button on boot
SUPPORT_REMOTE_RECOVERY := false

# Whether fastplay should be played completely or not: true or false
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.fastplay.fullyplay=false

# Enable low RAM config or not: true or false
PRODUCT_PROPERTY_OVERRIDES += \
    ro.config.low_ram=false

# Enable wallpaper or not for low_ram: true or false
PRODUCT_PROPERTY_OVERRIDES += \
    persist.low_ram.wp.enable=false

# Whether bootanimation should be played or not: true or false
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.bootanim.enable=true

# Enable TDE compose or not: true or false
PRODUCT_PROPERTY_OVERRIDES += \
    ro.config.tde_compose=true

#set video output rate for telecom and unicom, defalt 4:3
ifeq ($(strip $(PRODUCT_TARGET)),telecom)
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.video.cvrs= 1
else ifeq ($(strip $(PRODUCT_TARGET)),unicom)
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.video.cvrs= 1
else
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.video.cvrs= 0
endif

#if thirdparty dhcp service is needed to obtain ip, please set this property true
# default value is false
PRODUCT_PROPERTY_OVERRIDES += \
    ro.thirdparty.dhcp=false

# smart_suspend, deep_launcher, deep_restart, deep_resume;
PRODUCT_PROPERTY_OVERRIDES += \
     persist.suspend.mode=deep_restart

# Output format adaption for 2D streams
# false -> disable; true -> enable
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.video.adaptformat=false

# Whether CVBS is enabled or not when HDMI is plugged in
ifeq ($(strip $(PRODUCT_TARGET)), telecom)
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.cvbs.and.hdmi=true
else ifeq ($(strip $(PRODUCT_TARGET)), unicom)
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.cvbs.and.hdmi=true
else
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.cvbs.and.hdmi=false
endif

# Whether display format self-adaption is enabled or not when HDMI is plugged in
# 0 -> disable; 1 -> enable
ifeq ($(strip $(PRODUCT_TARGET)), telecom)
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.optimalfmt.enable=0
else ifeq ($(strip $(PRODUCT_TARGET)), unicom)
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.optimalfmt.enable=0
else ifeq ($(strip $(PRODUCT_TARGET)), shcmcc)
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.optimalfmt.enable=0
else
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.optimalfmt.enable=1
endif

# Preferential display format: native, i50hz, p50hz, i60hz, p60hz or max_fmt
# persist.sys.optimalfmt.perfer is valid only if persist.sys.optimalfmt.enable=1
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.optimalfmt.perfer=native

# Preferential hiplayer cache setting
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.player.cache.type=time \
    persist.sys.player.cache.low=500 \
    persist.sys.player.cache.high=12000 \
    persist.sys.player.cache.total=20000 \
    persist.sys.player.bufmaxsize=80

# Preferential hiplayer buffer seek
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.player.buffer.seek=0 \
    persist.sys.player.leftbuf.min=10 \
    persist.sys.player.avsync.min=500

# Preferential hiplayer rtsp timeshift support for sichuan mobile
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.hiplayer.rtspusetcp=false

# display hdmi cec
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.hdmi.cec=false

# onekey to switch ime key
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.imeswitch.key=18

# hdrmode set 0:SDR, 1:Dolby, 2:HDR10, 3:auto
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.hdrmode=3

# colorspace and deepcolor flag for user set
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.colorspacesettype= -1

# HI_UNF_HDMI_GetSinkCapability status flag

PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.capgetfailflag = -1
# HISILICON add begin
# wifi check internet,according to the systemproperty of "persist.wifi.checkinternet"
# to determin check internet access or not, false as defaul
# true->check internet access, false->do not check internet access
PRODUCT_PROPERTY_OVERRIDES += \
    persist.wifi.checkinternet=false

# ctc,presynctimeout set 1000: PreSyncTimeOut value(default); 0:close Pre sync fuction
# ctc,quickoutput set 1:open quick output fuction(default); 0:close quick output fuction
# ctc,support set errcover for playfirstfrm,fast,stopfast,seek,resume scenarios
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.ctc.presynctimeout = 1000 \
    persist.sys.ctc.quickoutput = 1 \
    persist.sys.ctc.errcover.playfirstfrm = 20 \
    persist.sys.ctc.errcover.fast = 100 \
    persist.sys.ctc.errcover.stopfast = 20 \
    persist.sys.ctc.errcover.seek = 100 \
    persist.sys.ctc.errcover.resume = 20
# HISILICON add end
