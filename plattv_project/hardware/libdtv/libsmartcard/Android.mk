# Copyright (C) 2008 The Android Open Source Project
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


LOCAL_PATH := $(call my-dir)

# HAL module implemenation, not prelinked and stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../device/hisilicon/bigfish/sdk/source/common/include/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../device/hisilicon/bigfish/sdk/source/common/drv/include/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../device/hisilicon/bigfish/sdk/source/common/api/include/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../device/hisilicon/bigfish/sdk/source/msp/api/include/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../device/hisilicon/bigfish/sdk/source/msp/drv/include/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../device/hisilicon/bigfish/sdk/source/msp/include/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../device/hisilicon/bigfish/sdk/source/component/ha_codec/include/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../device/hisilicon/bigfish/sdk/source/msp/drv/jpeg/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../libhardware/include/hardware
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libcommon/


LOCAL_SHARED_LIBRARIES := liblog libcutils libhi_msp libhardware
LOCAL_SRC_FILES := smartcard.cpp
LOCAL_MODULE := smartcard.bigfish

base := $(LOCAL_PATH)/../..
LOCAL_MODULE_TAGS :=eng
include $(BUILD_SHARED_LIBRARY)
