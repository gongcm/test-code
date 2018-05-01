
HISI_PRODUCT_LINE := STB
HISI_PLATFORM_PATH := device/hisilicon/bigfish
SDK_NAME := sdk
SDK_DIR  := $(HISI_PLATFORM_PATH)/$(SDK_NAME)
BUILD_DIR:= $(HISI_PLATFORM_PATH)/build
ETC_DIR  := $(HISI_PLATFORM_PATH)/etc

# Optimized Boot Support
# MUST set it before full_base.mk
OPTIMIZED_BOOT_SUPPORT := true

#if androidtv then set to true
SUPPORT_ANDROIDTV := false
ifeq ($(strip $(SUPPORT_ANDROIDTV)), true)
   $(call inherit-product-if-exists, $(BUILD_DIR)/androidtv.mk)
else
   $(call inherit-product, $(SRC_TARGET_DIR)/product/full_base.mk)
endif


CHIPNAME := Hi3798MV200
PRODUCT_NAME := Hi3798MV200
PRODUCT_DEVICE := Hi3798MV200
PRODUCT_BRAND := HiSTBAndroidV6
PRODUCT_MODEL := Hi3798MV200
PRODUCT_MANUFACTURER := Hisilicon

$(call inherit-product, device/hisilicon/Hi3798MV200/device.mk)

