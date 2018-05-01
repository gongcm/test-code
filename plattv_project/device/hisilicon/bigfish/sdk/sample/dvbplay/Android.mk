LOCAL_PATH := $(call my-dir)

###############################sample_dvbplay#################################
include $(CLEAR_VARS)
include $(SDK_DIR)/Android.def

LOCAL_MODULE := sample_dvbplay
LOCAL_MULTILIB := 32

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := $(CFG_HI_CFLAGS) $(CFG_HI_BOARD_CONFIGS)
LOCAL_CFLAGS += -DLOG_TAG=\"$(LOCAL_MODULE)\"

LOCAL_SRC_FILES := sample_dvbplay.c

LOCAL_C_INCLUDES := $(COMMON_UNF_INCLUDE)
LOCAL_C_INCLUDES += $(COMMON_DRV_INCLUDE)
LOCAL_C_INCLUDES += $(COMMON_API_INCLUDE)
LOCAL_C_INCLUDES += $(MSP_UNF_INCLUDE)
LOCAL_C_INCLUDES += $(MSP_DRV_INCLUDE)
LOCAL_C_INCLUDES += $(MSP_API_INCLUDE)
LOCAL_C_INCLUDES += $(SAMPLE_DIR)/common
LOCAL_C_INCLUDES += $(COMPONENT_DIR)/ha_codec/include

LOCAL_SHARED_LIBRARIES := liblog libcutils libdl libm libhi_common libhi_msp libhi_sample_common

ifeq ($(CFG_HI_KEYLED_SUPPORT),y)
LOCAL_CFLAGS += -DHI_KEYLED_SUPPORT
endif

include $(BUILD_EXECUTABLE)

###############################sample_dvbplay_ir#################################
include $(CLEAR_VARS)

include $(SDK_DIR)/Android.def

LOCAL_MODULE := sample_dvbplay_ir
LOCAL_MULTILIB := 32

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := $(CFG_HI_CFLAGS) $(CFG_HI_BOARD_CONFIGS)
LOCAL_CFLAGS += -DLOG_TAG=\"$(LOCAL_MODULE)\"

LOCAL_SRC_FILES := sample_dvbplay_ir.c

LOCAL_C_INCLUDES := $(COMMON_UNF_INCLUDE)
LOCAL_C_INCLUDES += $(COMMON_DRV_INCLUDE)
LOCAL_C_INCLUDES += $(COMMON_API_INCLUDE)
LOCAL_C_INCLUDES += $(MSP_UNF_INCLUDE)
LOCAL_C_INCLUDES += $(MSP_DRV_INCLUDE)
LOCAL_C_INCLUDES += $(MSP_API_INCLUDE)
LOCAL_C_INCLUDES += $(SAMPLE_DIR)/common
LOCAL_C_INCLUDES += $(COMPONENT_DIR)/ha_codec/include

LOCAL_SHARED_LIBRARIES := liblog libcutils libdl libm libhi_common \
                          libhi_msp libhi_sample_common

ifeq ($(CFG_HI_KEYLED_SUPPORT),y)
LOCAL_CFLAGS += -DHI_KEYLED_SUPPORT
endif

include $(BUILD_EXECUTABLE)

###############################dvbplay_gk7661a#################################
#include $(CLEAR_VARS)
#
#include $(SDK_DIR)/Android.def
#
#LOCAL_MODULE := dvbplay_gk7661a
#LOCAL_MULTILIB := 32
#
#LOCAL_MODULE_TAGS := optional
#
#LOCAL_CFLAGS := $(CFG_HI_CFLAGS) $(CFG_HI_BOARD_CONFIGS)
##
#LOCAL_SRC_FILES := dvbplay_gk7661a.c
#
#LOCAL_C_INCLUDES := $(COMMON_UNF_INCLUDE)
#LOCAL_C_INCLUDES += $(COMMON_DRV_INCLUDE)
#LOCAL_C_INCLUDES += $(COMMON_API_INCLUDE)
#LOCAL_C_INCLUDES += $(MSP_UNF_INCLUDE)
#LOCAL_C_INCLUDES += $(MSP_DRV_INCLUDE)
#LOCAL_C_INCLUDES += $(MSP_API_INCLUDE)
#LOCAL_C_INCLUDES += $(SAMPLE_DIR)/common
#LOCAL_C_INCLUDES += $(COMPONENT_DIR)/ha_codec/include
#
#LOCAL_SHARED_LIBRARIES := liblog libcutils libdl libm libhi_common \
#                          libhi_msp libhi_sample_common
#
#ifeq ($(CFG_HI_KEYLED_SUPPORT),y)
#LOCAL_CFLAGS += -DHI_KEYLED_SUPPORT
#endif
#
#include $(BUILD_EXECUTABLE)
#
###############################sample_dvbplay_pid#################################
include $(CLEAR_VARS)

include $(SDK_DIR)/Android.def

LOCAL_MODULE := sample_dvbplay_pid
LOCAL_MULTILIB := 32

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := $(CFG_HI_CFLAGS) $(CFG_HI_BOARD_CONFIGS)
LOCAL_CFLAGS += -DLOG_TAG=\"$(LOCAL_MODULE)\"

LOCAL_SRC_FILES := sample_dvbplay_pid.c

LOCAL_C_INCLUDES := $(COMMON_UNF_INCLUDE)
LOCAL_C_INCLUDES += $(COMMON_DRV_INCLUDE)
LOCAL_C_INCLUDES += $(COMMON_API_INCLUDE)
LOCAL_C_INCLUDES += $(MSP_UNF_INCLUDE)
LOCAL_C_INCLUDES += $(MSP_DRV_INCLUDE)
LOCAL_C_INCLUDES += $(MSP_API_INCLUDE)
LOCAL_C_INCLUDES += $(SAMPLE_DIR)/common
LOCAL_C_INCLUDES += $(COMPONENT_DIR)/ha_codec/include

LOCAL_SHARED_LIBRARIES := liblog libcutils libdl libm libhi_common \
                          libhi_msp libhi_sample_common

ifeq ($(CFG_HI_KEYLED_SUPPORT),y)
LOCAL_CFLAGS += -DHI_KEYLED_SUPPORT
endif

include $(BUILD_EXECUTABLE)

###############################dvbplay_gk7661f#################################
include $(CLEAR_VARS)

include $(SDK_DIR)/Android.def

LOCAL_MODULE := dvbplay_gk7661f
LOCAL_MULTILIB := 32

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := $(CFG_HI_CFLAGS) $(CFG_HI_BOARD_CONFIGS)
LOCAL_CFLAGS += -DLOG_TAG=\"$(LOCAL_MODULE)\"

LOCAL_SRC_FILES := dvbplay_gk7661f.c

LOCAL_C_INCLUDES := $(COMMON_UNF_INCLUDE)
LOCAL_C_INCLUDES += $(COMMON_DRV_INCLUDE)
LOCAL_C_INCLUDES += $(COMMON_API_INCLUDE)
LOCAL_C_INCLUDES += $(MSP_UNF_INCLUDE)
LOCAL_C_INCLUDES += $(MSP_DRV_INCLUDE)
LOCAL_C_INCLUDES += $(MSP_API_INCLUDE)
LOCAL_C_INCLUDES += $(SAMPLE_DIR)/common
LOCAL_C_INCLUDES += $(COMPONENT_DIR)/ha_codec/include

LOCAL_SHARED_LIBRARIES := liblog libcutils libdl libm libhi_common \
                          libhi_msp libhi_sample_common

ifeq ($(CFG_HI_KEYLED_SUPPORT),y)
LOCAL_CFLAGS += -DHI_KEYLED_SUPPORT
endif

include $(BUILD_EXECUTABLE)


