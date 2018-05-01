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

# # NEXUS_TOP := $(LOCAL_PATH)/../../../vendor/broadcom/refsw/nexus
# include $(NEXUS_TOP)/nxclient/include/nxclient.inc
# LOCAL_C_INCLUDES += $(NEXUS_APP_INCLUDE_PATHS) \
					# $(NXCLIENT_INCLUDES) \
					# $(LOCAL_PATH)/../libcommon
# # $(warning LOCAL_C_INCLUDES=$(LOCAL_C_INCLUDES))
# LOCAL_CFLAGS += $(NEXUS_CFLAGS) $(addprefix -D,$(NEXUS_APP_DEFINES)) $(addprefix -D,$(BMEDIA_PROBE_DEFINES))
# # $(warning LOCAL_CFLAGS=$(LOCAL_CFLAGS))

LOCAL_CFLAGS += -DLOGD=ALOGD -DLOGE=ALOGE -DLOGW=ALOGW -DLOGV=ALOGV -DLOGI=ALOGI

LOCAL_SHARED_LIBRARIES := liblog libcutils libhardware

LOCAL_SRC_FILES := test.cpp
LOCAL_MODULE := dtvtest

base := $(LOCAL_PATH)/../..
LOCAL_MODULE_TAGS :=eng
include $(BUILD_EXECUTABLE)
