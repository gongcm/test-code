# ---------------------------------------------------------------------
# EXT4 Images Begin
# ------------------------------ota add by jn---------------------------------------
BOARD_OTAPACKAGE_PARTITION_SIZE := 629145600
TARGET_OUT_OTAPACKAGE := $(PRODUCT_OUT)/otapackage
INSTALLED_OTAPACKAGE_TARGET := $(PRODUCT_OUT)/otapackage.img

include $(CLEAR_VARS)
EXT4_IMG :=ext4img
MAKE_EXT4FS_TOOLS := $(HOST_OUT_EXECUTABLES)/make_ext4fs
$(EXT4_IMG):$(INSTALLED_SYSTEMIMAGE) $(INSTALLED_USERDATAIMAGE_TARGET) $(INSTALLED_CACHEIMAGE_TARGET)
	mkdir -p $(TARGET_OUT_PRIVATE)
	mkdir -p $(TARGET_OUT_SECURESTORE)
	mkdir -p $(TARGET_OUT_OTAPACKAGE)
	$(MAKE_EXT4FS_TOOLS) -s -l $(BOARD_OTAPACKAGE_PARTITION_SIZE) -a otapackage $(INSTALLED_OTAPACKAGE_TARGET) $(TARGET_OUT_OTAPACKAGE)
	$(MAKE_EXT4FS_TOOLS) -s -l $(BOARD_PRIVATEIMAGE_PARTITION_SIZE) -a private $(INSTALLED_PRIVATEIMAGE_TARGET) $(TARGET_OUT_PRIVATE)
	$(MAKE_EXT4FS_TOOLS) -s -l $(BOARD_SECURESTORE_PARTITION_SIZE) -a securestore  $(INSTALLED_SECURESTOREIMAGE_TARGET) $(TARGET_OUT_SECURESTORE)
	cp -r $(PRODUCT_OUT)/system.img $(EMMC_PRODUCT_OUT)/system.ext4
	cp -r $(PRODUCT_OUT)/userdata.img $(EMMC_PRODUCT_OUT)/userdata.ext4
	cp -r $(PRODUCT_OUT)/cache.img $(EMMC_PRODUCT_OUT)/cache.ext4
	
	cp -r $(PRODUCT_OUT)/otapackage.img $(EMMC_PRODUCT_OUT)/otapackage.ext4
	cp -r $(PRODUCT_OUT)/private.img $(EMMC_PRODUCT_OUT)/private.ext4
	cp -r $(PRODUCT_OUT)/securestore.img $(EMMC_PRODUCT_OUT)/securestore.ext4

.PHONY: ext4fs
ext4fs: $(EXT4_IMG)
# ---------------------------------------------------------------------
# EXT4 Images end
# ---------------------------------------------------------------------
