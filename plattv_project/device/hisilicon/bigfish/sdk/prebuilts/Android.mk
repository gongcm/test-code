LOCAL_PATH := $(call my-dir)

include $(SDK_DIR)/Android.def

define hisdk_check_file_print
$(foreach var,$(1),\
  $(shell test ! -f $(LOCAL_PATH)/$(var).so || echo $(var)) \
)
endef

define addsuffix_so_list
$(addsuffix .so, $(1))
endef

ifneq ($(strip $(PLATFORM_SDK_VERSION)), 24)
ifeq (5.,$(findstring 5.,$(PLATFORM_VERSION)))
define get_src_files
$(eval LOCAL_SRC_FILES_32 := $(1))
endef
else
# android 4.4
define get_src_files
$(eval LOCAL_SRC_FILES := $(1))
endef
endif
else
# android n
define get_src_files
$(eval LOCAL_SRC_FILES_32 := $(1))
endef
endif

LOCAL_SRC_PRELIBS :=
ifeq ($(CFG_HI_HACODEC_MP3DECODE_SUPPORT),y)
LOCAL_SRC_PRELIBS += libHA.AUDIO.MP3.decode
LOCAL_SRC_PRELIBS += libHA.AUDIO.MP2.decode
endif
ifeq ($(CFG_HI_HACODEC_AACDECODE_SUPPORT),y)
LOCAL_SRC_PRELIBS += libHA.AUDIO.AAC.decode
endif
ifeq ($(CFG_HI_HACODEC_AACENCODE_SUPPORT),y)
LOCAL_SRC_PRELIBS += libHA.AUDIO.AAC.encode
endif
ifeq ($(CFG_HI_HACODEC_AC3PASSTHROUGH_SUPPORT),y)
LOCAL_SRC_PRELIBS += libHA.AUDIO.AC3PASSTHROUGH.decode
endif
ifeq ($(CFG_HI_HACODEC_AMRNBCODEC_SUPPORT),y)
LOCAL_SRC_PRELIBS += libHA.AUDIO.AMRNB.codec
endif
ifeq ($(CFG_HI_HACODEC_AMRWBCODEC_SUPPORT),y)
LOCAL_SRC_PRELIBS += libHA.AUDIO.AMRWB.codec
endif
ifeq ($(CFG_HI_HACODEC_BLURAYLPCMDECODE_SUPPORT),y)
LOCAL_SRC_PRELIBS += libHA.AUDIO.BLURAYLPCM.decode
endif
ifeq ($(CFG_HI_HACODEC_COOKDECODE_SUPPORT),y)
LOCAL_SRC_PRELIBS += libHA.AUDIO.COOK.decode
endif
ifeq ($(CFG_HI_HACODEC_DRADECODE_SUPPORT),y)
LOCAL_SRC_PRELIBS += libHA.AUDIO.DRA.decode
endif
ifeq ($(CFG_HI_HACODEC_DTSPASSTHROUGH_SUPPORT),y)
LOCAL_SRC_PRELIBS += libHA.AUDIO.DTSPASSTHROUGH.decode
endif
ifeq ($(CFG_HI_HACODEC_G711CODEC_SUPPORT),y)
LOCAL_SRC_PRELIBS += libHA.AUDIO.G711.codec
endif
ifeq ($(CFG_HI_HACODEC_G722CODEC_SUPPORT),y)
LOCAL_SRC_PRELIBS += libHA.AUDIO.G722.codec
endif
ifeq ($(CFG_HI_HACODEC_PCMDECODE_SUPPORT),y)
LOCAL_SRC_PRELIBS += libHA.AUDIO.PCM.decode
endif
ifeq ($(CFG_HI_HACODEC_TRUEHDPASSTHROUGH_SUPPORT),y)
LOCAL_SRC_PRELIBS += libHA.AUDIO.TRUEHDPASSTHROUGH.decode
endif
ifeq ($(CFG_HI_HACODEC_WMADECODE_SUPPORT),y)
LOCAL_SRC_PRELIBS += libHA.AUDIO.WMA.decode
endif

# ¶Å±È
LOCAL_SRC_PRELIBS += libHA.AUDIO.DOLBYPLUS.decode.so

LOCAL_MODULES_NAME := $(call hisdk_check_file_print, $(LOCAL_SRC_PRELIBS))

define mult-prebuilt-hisi-libs
$(foreach t,$(1), \
		$(eval include $(CLEAR_VARS)) \
		$(eval LOCAL_MODULE := $(t)) \
		$(eval LOCAL_MODULE_SUFFIX := .so) \
		$(eval LOCAL_MULTILIB := $(2)) \
		$(call get_src_files, $(t).so) \
		$(eval LOCAL_MODULE_CLASS := SHARED_LIBRARIES) \
		$(eval LOCAL_MODULE_TAGS := optional) \
		$(eval include $(BUILD_PREBUILT)) \
 )
endef

$(call mult-prebuilt-hisi-libs, $(LOCAL_MODULES_NAME), 32)
