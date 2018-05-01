include $(CLEAR_VARS)

ifeq ($(strip $(TARGET_HAVE_APPLOADER)),true)
CHIP_TABLE := $(CHIPNAME)-emmc-loader.xml
else
ifeq ($(strip $(VMX_ADVANCED_SUPPORT)),true)
CHIP_TABLE := $(CHIPNAME)-emmc-vmx.xml
else
CHIP_TABLE := $(CHIPNAME)-emmc.xml
endif
endif

EMMC_PREBUILT_IMG :=$(EMMC_PRODUCT_OUT)/$(CHIP_TABLE) \
		$(EMMC_PRODUCT_OUT)/baseparam.img \
		$(EMMC_PRODUCT_OUT)/fastplay.img \
		$(EMMC_PRODUCT_OUT)/logo.img \
		$(EMMC_PRODUCT_OUT)/properties.bin

$(EMMC_PREBUILT_IMG) : $(EMMC_PRODUCT_OUT)/% : $(TOP)/device/hisilicon/$(CHIPNAME)/prebuilts/% | $(ACP)
	$(transform-prebuilt-to-target)

.PHONY:prebuilt-emmc
prebuilt-emmc:$(EMMC_PREBUILT_IMG)

EMMC_PREBUILT_BASEPARAM := $(PRODUCT_OUT)/system/etc/baseparam.img
$(EMMC_PREBUILT_BASEPARAM) : $(EMMC_PREBUILT_IMG)
ifneq (baseparam.img, $(notdir $(wildcard $(PRODUCT_OUT)/system/etc/baseparam.img)))
	$(hide) cp -af $(EMMC_PRODUCT_OUT)/baseparam.img $(PRODUCT_OUT)/system/etc/
endif

.PHONY:prebuilt-emmc-baseparam
prebuilt-emmc-baseparam:$(EMMC_PREBUILT_BASEPARAM)
#----------------------------------------------------------------------
# hiboot
#----------------------------------------------------------------------
include $(CLEAR_VARS)

EMMC_HIBOOT_IMG := fastboot-burn-emmc.bin
EMMC_HIBOOT_OBJ := $(TARGET_OUT_INTERMEDIATES)/EMMC_HIBOOT_OBJ
ifeq ($(strip $(TARGET_HAVE_APPLOADER)),false)
ifeq ($(strip $(HISILICON_SECURITY_L2)),true)
ifeq ($(strip $(HISILICON_TEE)),true)
EMMC_BOOT_ANDROID_CFG := $(HISI_SDK_TEE_CFG)
else
EMMC_BOOT_ANDROID_CFG := $(HISI_SDK_SECURE_CFG)
endif
else
ifeq ($(strip $(VMX_ADVANCED_SUPPORT)),true)
EMMC_BOOT_ANDROID_CFG := $(HISI_SDK_ANDROID_VMX_CFG)
else
EMMC_BOOT_ANDROID_CFG := $(HISI_SDK_ANDROID_CFG)
endif
endif
else #TARGET_HAVE_APPLOADER
ifeq ($(strip $(HISILICON_SECURITY_L2)),true)
ifeq ($(strip $(HISILICON_TEE)),true)
EMMC_BOOT_ANDROID_CFG := $(HISI_SDK_TEE_APPLOADER_CFG)
else
EMMC_BOOT_ANDROID_CFG := $(HISI_SDK_SECURE_APPLOADER_CFG)
endif
else
ifeq ($(strip $(VMX_ADVANCED_SUPPORT)),true)
EMMC_BOOT_ANDROID_CFG := $(HISI_SDK_ANDROID_VMX_APPLOADER_CFG)
else
EMMC_BOOT_ANDROID_CFG := $(HISI_SDK_ANDROID_APPLOADER_CFG)
endif
endif
endif

EMMC_HIBOOT_CFLAGS :=
ifneq ($(findstring $(HISI_CHIPS), hi3796mv100 hi3798mv100),)
ifneq ($(strip $(EMMC_BOOT_REG_NAME)),)
EMMC_HIBOOT_CFLAGS += CFG_HI_BOOT_REG_NAME=${EMMC_BOOT_REG_NAME}
endif
else
ifneq ($(strip $(BOOT_REG_NAME)),)
EMMC_HIBOOT_CFLAGS += CFG_HI_BOOT_REG_NAME=${BOOT_REG_NAME}
endif
ifneq ($(strip $(BOOT_REG1_NAME)),)
EMMC_HIBOOT_CFLAGS += CFG_HI_BOOT_REG1_NAME=${BOOT_REG1_NAME}
endif
ifneq ($(strip $(BOOT_REG2_NAME)),)
EMMC_HIBOOT_CFLAGS += CFG_HI_BOOT_REG2_NAME=${BOOT_REG2_NAME}
endif
ifneq ($(strip $(BOOT_REG3_NAME)),)
EMMC_HIBOOT_CFLAGS += CFG_HI_BOOT_REG3_NAME=${BOOT_REG3_NAME}
endif
ifneq ($(strip $(BOOT_REG4_NAME)),)
EMMC_HIBOOT_CFLAGS += CFG_HI_BOOT_REG4_NAME=${BOOT_REG4_NAME}
endif
ifneq ($(strip $(BOOT_REG5_NAME)),)
EMMC_HIBOOT_CFLAGS += CFG_HI_BOOT_REG5_NAME=${BOOT_REG5_NAME}
endif
endif
ifneq ($(strip $(EMMC_BOOT_ENV_STARTADDR)),)
EMMC_HIBOOT_CFLAGS += CFG_HI_BOOT_ENV_STARTADDR=${EMMC_BOOT_ENV_STARTADDR}
endif
ifneq ($(strip $(EMMC_BOOT_ENV_SIZE)),)
EMMC_HIBOOT_CFLAGS += CFG_HI_BOOT_ENV_SIZE=${EMMC_BOOT_ENV_SIZE}
endif
ifneq ($(strip $(EMMC_BOOT_ENV_RANGE)),)
EMMC_HIBOOT_CFLAGS += CFG_HI_BOOT_ENV_RANGE=${EMMC_BOOT_ENV_RANGE}
endif
ifeq ($(strip $(HISILICON_TEE)),true)
EMMC_HIBOOT_CFLAGS += CFG_TVP_MEM_${HISILICON_TEE_MEM}=y  CFG_HI_TVP_MEM_LAYOUT=${HISILICON_TEE_MEM}
endif

emmc_fastboot_prepare:
	mkdir -p $(EMMC_HIBOOT_OBJ)
	mkdir -p $(EMMC_PRODUCT_OUT)

ifneq ($(NAND_HIBOOT_IMG),)
$(EMMC_HIBOOT_IMG): emmc_fastboot_prepare $(NAND_HIBOOT_IMG)
else
$(EMMC_HIBOOT_IMG): emmc_fastboot_prepare
endif
	
	cp $(SDK_DIR)/$(SDK_CFG_DIR)/$(EMMC_BOOT_ANDROID_CFG) $(EMMC_HIBOOT_OBJ);
	if [ "$(SUPPORT_REMOTE_RECOVERY)" = "true" ]; then \
	sed -i -e '/# CFG_HI_BUILD_WITH_IR /a\CFG_HI_BUILD_WITH_IR = y' -e '/# CFG_HI_BUILD_WITH_IR/d' \
	$(EMMC_HIBOOT_OBJ)/$(EMMC_BOOT_ANDROID_CFG); \
	fi
	cd $(SDK_DIR);$(MAKE) hiboot O=$(ANDROID_BUILD_TOP)/$(EMMC_HIBOOT_OBJ) $(EMMC_HIBOOT_CFLAGS) \
	SDK_CFGFILE=../../../../$(EMMC_HIBOOT_OBJ)/$(EMMC_BOOT_ANDROID_CFG); \
	if [ -f "$(ANDROID_BUILD_TOP)/$(EMMC_HIBOOT_OBJ)/miniboot.bin" ]; then \
	cp -avf $(ANDROID_BUILD_TOP)/$(EMMC_HIBOOT_OBJ)/miniboot.bin $(ANDROID_BUILD_TOP)/$(EMMC_PRODUCT_OUT)/fastboot.bin ;\
	elif [ -f "$(ANDROID_BUILD_TOP)/$(EMMC_HIBOOT_OBJ)/fastboot-burn.bin" ]; then \
	cp -avf $(ANDROID_BUILD_TOP)/$(EMMC_HIBOOT_OBJ)/fastboot-burn.bin $(ANDROID_BUILD_TOP)/$(EMMC_PRODUCT_OUT)/fastboot.bin ; \
	else \
	exit; \
	fi
	if [ -f "$(ANDROID_BUILD_TOP)/$(EMMC_HIBOOT_OBJ)/advca_programmer.bin" ]; then \
	cp -avf $(ANDROID_BUILD_TOP)/$(EMMC_HIBOOT_OBJ)/advca_programmer.bin $(ANDROID_BUILD_TOP)/$(EMMC_PRODUCT_OUT)/advca_programmer.bin;\
	fi
ifeq ($(strip $(HISILICON_SECURITY_L2)),true)
	$(hide) mkdir -p $(SECURE_OBJ_DIR)
	$(hide) chmod a+x $(SDK_LINUX_SIGN_TOOL_DIR)/CASignTool
	$(hide) $(SDK_LINUX_SIGN_TOOL_DIR)/CASignTool 0 $(SECURE_CONFIG_DIR)/Signboot_CV200_config.cfg -k $(SECURE_RSA_KEY_DIR) -r $(SECURE_INPUT_DIR) -o $(SECURE_OBJ_DIR)
	$(hide) cp -arv $(SECURE_OBJ_DIR)/FinalBoot.bin $(SECURE_OUTPUT_DIR)/fastboot.bin
endif
.PHONY: hiboot-emmc
hiboot-emmc: $(EMMC_HIBOOT_IMG)
#----------------------------------------------------------------------
# hiboot END
#


#----------------------------------------------------------------------
# Generate The Update Package
# 1: emmc update.zip
#----------------------------------------------------------------------

include $(CLEAR_VARS)

EMMC_UPDATE_PACKAGE :=$(EMMC_PRODUCT_OUT)/update.zip
ifeq ($(strip $(VMX_ADVANCED_SUPPORT)),true)
SQUASHFS := squashfs
EMMC_SOURCE_UPDATE_SCRIPT :=$(call include-path-for, recovery)/etc/META-INF/com/google/android/updater-script-emmc_vmx
else
EMMC_SOURCE_UPDATE_SCRIPT :=$(call include-path-for, recovery)/etc/META-INF/com/google/android/updater-script-emmc
endif
EMMC_OUT_UPDATE_SCRIPT :=$(EMMC_PRODUCT_OUT)/update/file/META-INF/com/google/android/updater-script

$(EMMC_UPDATE_PACKAGE): $(INSTALLED_SYSTEMIMAGE) $(INSTALLED_USERDATAIMAGE_TARGET) kernel $(RECOVERY_IMAGE) $(EMMC_PREBUILT_IMG) $(EMMC_HIBOOT_IMG) $(UPDATE_TOOLS) bootargs pq_param $(SQUASHFS)
	@echo ----- Making update package ------
	$(hide) rm -rf $(EMMC_PRODUCT_OUT)/update
	$(hide) mkdir -p $(EMMC_PRODUCT_OUT)/update
	$(hide) mkdir -p $(EMMC_PRODUCT_OUT)/update/file
	$(hide) mkdir -p $(EMMC_PRODUCT_OUT)/update/file/META
ifeq ($(strip $(SUPPROT_REMOTE_RECOVERY)),true)
	$(hide) cp -af $(call include-path-for, recovery)/etc/recovery.emmc.fstab.update $(EMMC_PRODUCT_OUT)/update/file/META/recovery.fstab
else
	$(hide) cp -af $(call include-path-for, recovery)/etc/recovery.emmc.fstab $(EMMC_PRODUCT_OUT)/update/file/META/recovery.fstab
endif
ifeq ($(strip $(VMX_ADVANCED_SUPPORT)),true)
ifeq ($(strip $(CFG_HI_ADVCA_VMX_3RD_SIGN)),y)
	$(TOP)/device/hisilicon/bigfish/security/vmx/tool/vmx_3rd_sign.sh $(CHIPNAME) $(EMMC_PRODUCT_OUT)
else
	$(TOP)/device/hisilicon/bigfish/security/vmx/tool/vmx_origin_sign.sh $(CHIPNAME) $(EMMC_PRODUCT_OUT)
endif
	$(hide) cp -a $(EMMC_PRODUCT_OUT)/signed_bootargs.bin $(EMMC_PRODUCT_OUT)/update/file/bootargs.img
	$(hide) cp -a $(EMMC_PRODUCT_OUT)/signed_recovery.img $(EMMC_PRODUCT_OUT)/update/file/recovery.img
	$(hide) cp -a $(EMMC_PRODUCT_OUT)/signed_kernel.img $(EMMC_PRODUCT_OUT)/update/file/boot.img
	$(hide) cp -a $(EMMC_PRODUCT_OUT)/signed_system.squashfs $(EMMC_PRODUCT_OUT)/update/file/system.squashfs
else
	$(hide) cp -a $(EMMC_PRODUCT_OUT)/bootargs.bin $(EMMC_PRODUCT_OUT)/update/file/bootargs.img
	$(hide) cp -a $(EMMC_PRODUCT_OUT)/recovery.img $(EMMC_PRODUCT_OUT)/update/file/recovery.img
	$(hide) cp -a $(EMMC_PRODUCT_OUT)/kernel.img $(EMMC_PRODUCT_OUT)/update/file/boot.img
	$(hide) cp -af $(PRODUCT_OUT)/system $(EMMC_PRODUCT_OUT)/update/file/system
endif
	$(hide) cp -a $(PRODUCT_OUT)/bootargs_emmc.txt $(EMMC_PRODUCT_OUT)/update/file/META/bootargs.txt
	$(hide) cp -a $(EMMC_PRODUCT_OUT)/fastboot.bin $(EMMC_PRODUCT_OUT)/update/file/fastboot.img
	$(hide) cp -a $(EMMC_PRODUCT_OUT)/baseparam.img $(EMMC_PRODUCT_OUT)/update/file/baseparam.img
ifneq ($(strip $(BOARD_QBSUPPORT)),true)
	$(hide) cp -a $(EMMC_PRODUCT_OUT)/fastplay.img $(EMMC_PRODUCT_OUT)/update/file/fastplay.img
endif
	$(hide) cp -a $(EMMC_PRODUCT_OUT)/logo.img $(EMMC_PRODUCT_OUT)/update/file/logo.img
	$(hide) cp -a $(EMMC_PRODUCT_OUT)/pq_param.bin $(EMMC_PRODUCT_OUT)/update/file/pq_param.img
ifeq ($(strip $(BOARD_QBSUPPORT)),true)
	$(hide) cp -a $(EMMC_PRODUCT_OUT)/qbboot.bin $(EMMC_PRODUCT_OUT)/update/file/qbboot.img
endif
	$(hide) mkdir -p $(EMMC_PRODUCT_OUT)/update/file/META-INF/com/google/android
	$(hide) cp -a $(PRODUCT_OUT)/system/bin/updater $(EMMC_PRODUCT_OUT)/update/file/META-INF/com/google/android/update-binary
	$(hide) cp -af $(PRODUCT_OUT)/data $(EMMC_PRODUCT_OUT)/update/file/userdata
	$(hide) cp -a $(EMMC_SOURCE_UPDATE_SCRIPT) $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) find $(PRODUCT_OUT)/system -type l -printf "symlink(\"%l\", \"/system/%P\");\n" >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "ui_print(\"set system symlink ok.....\");  unmount(\"/system\"); " >> $(EMMC_OUT_UPDATE_SCRIPT)
#	$(hide) find $(PRODUCT_OUT)/data -type l -printf "symlink(\"%l\", \"/data/%P\");\n" >> $(EMMC_OUT_UPDATE_SCRIPT)
#	$(hide) echo "ui_print(\"set data symlink ok.....\");  unmount(\"/data\"); " >> $(EMMC_OUT_UPDATE_SCRIPT)
ifeq ($(backup_upgrade),y)
	$(hide) echo "ifelse(isexistupdatepackage("/backup")," >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "ui_print(\"jump over backup ......\")," >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "ui_print(\"format backup......\");"  >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "format(\"ext4\", \"EMMC\", \"/dev/block/platform/soc/by-name/backup\", \"0\", \"/backup\");" >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "ui_print(\"mount backup......\");" >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "mount(\"ext4\", \"EMMC\", \"/dev/block/platform/soc/by-name/backup\", \"/backup\");" >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "ui_print(\"write backup......\");" >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "copy_file(\"update.zip\",\"/backup/update.zip\");" >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "unmount(\"/backup\");" >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo ");" >> $(EMMC_OUT_UPDATE_SCRIPT)
endif
	$(hide) echo "ifelse(isexistupdatepackage("/cache")," >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "ui_print(\"update.zip in cache......\");" >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "delete_recursive(\"/cache\")," >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "ui_print(\"format cache......\");"  >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "unmount(\"/cache\");" >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "format(\"ext4\", \"EMMC\", \"/dev/block/platform/soc/by-name/cache\", \"0\", \"/cache\");" >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo ");" >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "close_led();" >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "ui_print(\"update ok.....\");" >> $(EMMC_OUT_UPDATE_SCRIPT)
	$(hide) echo "recovery_api_version=$(RECOVERY_API_VERSION)" >$(EMMC_PRODUCT_OUT)/update/file/META/misc_info.txt
	$(hide) echo "fstab_version=$(RECOVERY_FSTAB_VERSION)" >> $(EMMC_PRODUCT_OUT)/update/file/META/misc_info.txt
	$(hide) (cd $(EMMC_PRODUCT_OUT)/update/file && zip -qry ../sor_update.zip .)
	zipinfo -1 $(EMMC_PRODUCT_OUT)/update/sor_update.zip | awk '/^system\// {print}' | $(HOST_OUT_EXECUTABLES)/fs_config  > $(EMMC_PRODUCT_OUT)/update/file/META/filesystem_config.txt
	$(hide) (cd $(EMMC_PRODUCT_OUT)/update/file && zip -q ../sor_update.zip META/*)
	java -jar $(SIGNAPK_JAR) -w  $(DEFAULT_SYSTEM_DEV_CERTIFICATE).x509.pem $(DEFAULT_SYSTEM_DEV_CERTIFICATE).pk8 $(EMMC_PRODUCT_OUT)/update/sor_update.zip $(EMMC_PRODUCT_OUT)/update/update.zip
	$(hide) cp -a $(EMMC_PRODUCT_OUT)/update/update.zip  $(EMMC_PRODUCT_OUT)/
	$(hide) mkdir -p $(TARGET_OUT_BACKUP)
ifeq ($(strip $(SUPPORT_REMOTE_RECOVERY)),true)
	$(hide) cp -a $(EMMC_PRODUCT_OUT)/update/update.zip  $(TARGET_OUT_BACKUP)/
endif
	$(hide) $(HOST_OUT_EXECUTABLES)/make_ext4fs -s -l $(BOARD_BACKUPIMAGE_PARTITION_SIZE) -a backup $(INSTALLED_BACKUPIMAGE_TARGET) $(TARGET_OUT_BACKUP)
	$(hide) cp -r $(INSTALLED_BACKUPIMAGE_TARGET) $(EMMC_PRODUCT_OUT)/backup.ext4
	$(hide) rm -rf $(EMMC_PRODUCT_OUT)/update
	@echo ----- Made update package: $@ --------



.PHONY: updatezip-emmc
updatezip-emmc: $(EMMC_UPDATE_PACKAGE)

#----------------------------------------------------------------------
# Generate The Update Package End
#----------------------------------------------------------------------
