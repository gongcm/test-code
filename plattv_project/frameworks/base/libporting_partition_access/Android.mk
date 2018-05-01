LOCAL_PATH:= $(call my-dir)

SDK_PATH := $(ANDROID_BUILD_TOP)/device/hisilicon/bigfish/sdk/source

include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := eng

LOCAL_SRC_FILES:= \
	porting_partition_access_interface.c \

    
LOCAL_C_INCLUDES += $(SDK_PATH)/common/include/  
LOCAL_C_INCLUDES += $(SDK_PATH)/common/api/flash/include
LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libhi_common \
#	libhi_flash \


LOCAL_MODULE := libporting_partition_access_interface
include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := eng
LOCAL_SRC_FILES:= \
	flash_test.c
LOCAL_C_INCLUDES += .
LOCAL_SHARED_LIBRARIES := libporting_partition_access_interface
LOCAL_MODULE := porting_flash_demo
include $(BUILD_EXECUTABLE)