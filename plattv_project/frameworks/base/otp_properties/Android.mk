LOCAL_PATH:= $(call my-dir)

SDK_PATH := $(ANDROID_BUILD_TOP)/device/hisilicon/bigfish/sdk/source

include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := eng
LOCAL_SRC_FILES:= otp_properties.cpp
				
LOCAL_SHARED_LIBRARIES := libutils \
							libcutils \
							libRootShell \
							libporting_partition_access_interface
	
	
LOCAL_C_INCLUDES += $(ANDROID_BUILD_TOP)/frameworks/base/libporting_partition_access \
					$(ANDROID_BUILD_TOP)/frameworks/base/tools/rootshell
	
LOCAL_MODULE := otp_properties
include $(BUILD_EXECUTABLE)
