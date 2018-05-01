LOCAL_PATH:= $(call my-dir)

SDK_PATH := $(ANDROID_BUILD_TOP)/device/hisilicon/bigfish/sdk/source

include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := eng

LOCAL_SRC_FILES:= \
	fpanel_led.c
	
LOCAL_C_INCLUDES += $(SDK_PATH)/common/include/
LOCAL_C_INCLUDES += $(SDK_PATH)/msp/include/

LOCAL_SHARED_LIBRARIES := \
	libhi_common \
	libhi_msp
	
LOCAL_MODULE := libfpanel_ledcontrol
include $(BUILD_SHARED_LIBRARY)



include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := eng

LOCAL_SRC_FILES:= \
	gpio_test.c
	
LOCAL_C_INCLUDES += .
LOCAL_C_INCLUDES += $(SDK_PATH)/common/include/
LOCAL_C_INCLUDES += $(SDK_PATH)/msp/include/
LOCAL_SHARED_LIBRARIES := libfpanel_ledcontrol 	\
	libhi_common \
	libhi_msp

LOCAL_MODULE := ipanel_gpio_led
include $(BUILD_EXECUTABLE)
