#!/bin/bash
#export PATH="/home/root/bin/x86-arm/arm-hisiv200-linux/target/bin:/home/joy/tools/1.6.0_23-ubuntu-10.04/x86_64/bin:$PATH"
set -e
source /home/joy/shell/setlocale.sh
source /home/joy/tools/jdk1.6.0_23.sh
source /home/joy/tools/arm-hisiv200-linux.sh

chmod 555 device/hisilicon/bigfish/etc/*.rc
chmod 555 system/core/rootdir/*.rc
chmod 555 device/hisilicon/Hi3798MV200/etc/*.rc

default_prop_file=out/target/product/Hi3798MV200/root/default.prop
if [ -f $default_prop_file ];then
	chmod 644 $default_prop_file
fi

init_rc_file=bootable/recovery/etc/init.rc
if [ -f $init_rc_file ];then
	chmod 555 $init_rc_file
fi
#chmod 555 bootable/recovery/etc/init.rc

rec_default_prop=out/target/product/Hi3798MV200/recovery/root/default.prop
if [ -f $rec_default_prop ];then
	chmod 644 $rec_default_prop
fi
#chmod 644 out/target/product/Hi3798MV200/recovery/root/default.prop

touch device/hisilicon/bigfish/etc/*.rc
touch system/core/rootdir/*.rc
touch device/hisilicon/Hi3798MV200/etc/*.rc


#touch out/target/product/Hi3798MV200/root/default.prop
if [ -f $default_prop_file ];then
	touch $default_prop_file
fi

#touch out/target/product/Hi3798MV200/recovery/root/default.prop
if [ -f $rec_default_prop ];then
	touch $rec_default_prop
fi


if [ ! -d out/target/product/Hi3798MV200/system/vendor/firmware ]
then
	mkdir -p out/target/product/Hi3798MV200/system/vendor/firmware
fi

mask_home_apk=development/samples/Home/Android.mk
if [ -f $mask_home_apk ];then
	mv $mask_home_apk development/samples/Home/Android.mkk
fi

rm -rf device/hisilicon/bigfish/development/apps/HardwareTest
rm -rf device/hisilicon/bigfish/development/apps/HiDebugKit
rm -rf device/hisilicon/bigfish/development/apps/HiFactoryTest
rm -rf device/hisilicon/bigfish/development/apps/HiErrorReport
rm -rf device/hisilicon/bigfish/development/apps/HiAniApp
rm -rf device/hisilicon/bigfish/development/*
rm -rf device/hisilicon/bigfish/packages/apps/HiMediaCenter
rm -rf device/hisilicon/bigfish/packages/apps/HiSetting
rm -rf device/hisilicon/bigfish/packages/apps/HiVideoPlayer
rm -rf device/hisilicon/bigfish/packages/apps/HiGallery
rm -rf device/hisilicon/bigfish/packages/apps/HiPinyinIME
rm -rf device/hisilicon/bigfish/packages/apps/HiPQSetting
rm -rf device/hisilicon/bigfish/hidolphin/component/himultiscreen
rm -rf device/hisilicon/bigfish/hidolphin/component/miracast

rm -rf device/hisilicon/bigfish/prebuilts/ThirdPartyApplications/data

rm -rf packages/apps/Nfc
if [ -f out/target/product/Hi3798MV200/system/etc/hosts ];then
	rm out/target/product/Hi3798MV200/system/etc/hosts
fi
rm -rf device/hisilicon/bigfish/packages/apps/HiMusic
if [ -f out/target/product/Hi3798MV200/system/app/HiMusic.apk ];then
	rm out/target/product/Hi3798MV200/system/app/HiMusic.apk
fi
if [ -f out/target/product/Hi3798MV200/system/priv-app/MediaProvider.apk ];then
	rm out/target/product/Hi3798MV200/system/priv-app/MediaProvider.apk
fi
rm -rf device/hisilicon/bigfish/packages/apps/HiFileManager
if [ -f out/target/product/Hi3798MV200/system/app/HiFileManager.apk ];then
	rm out/target/product/Hi3798MV200/system/app/HiFileManager.apk
fi
if [ -d device/hisilicon/bigfish/packages/apps/HiPingyinIME ];then
	rm out/target/product/Hi3798MV200/system/app/HiPinyinIME.apk
fi
if [ -f out/target/product/Hi3798MV200/system/app/HiPinyinIME.apk ];then
	rm out/target/product/Hi3798MV200/system/app/HiPinyinIME.apk
fi
#now service time
etc_use_time_file=out/target/product/Hi3798MV200/system/etc/now_img_time_file
if [ -f $etc_use_time_file ];then
	rm $etc_use_time_file
	tm=`date +%F`
	hour=`date +%R`
	echo "$tm $hour">>$etc_use_time_file
else
	tm=`date +%F`
	hour=`date +%R`
fi

apk_security_path=out/target/product/Hi3798MV200/system/etc/security
apk_certs_path=out/target/product/Hi3798MV200/system/etc/security/certs
if [ -d $apk_security_path ];then
if [ -d $apk_certs_path ];then
	echo "certs is exist"
else
	mkdir $apk_security_path/certs
fi
	cp build/target/product/security/*.x509.pem $apk_security_path/certs
fi

rec_build_prop=out/target/product/Hi3798MV200/system/build.prop
if [ -f $rec_build_prop ];then
	rm  $rec_build_prop
fi
#rm -rf out/target/product/Hi3798MV200/obj/KERNEL_OBJ/

source build/envsetup.sh

lunch Hi3798MV200-eng

chmod 777 install_imconing.sh

./install_imconing.sh

#make clean
echo "threamNum:$2"
make bigfish -j${2}
echo "make bigfish success">>$1
# make recoveryimg -j2
#make ext4fs -j2
#make hiboot -j2
# rm -r out/target/product/Hi3798MV200/obj/KERNEL_OBJ/drivers/msp/mce/
#make kernel -j2
#make userdataimage -j2
#python hy.panic.py err.txt
# if [ -f out/target/product/Hi3798MV200/Emmc/fastplay.img ];then
	# rm out/target/product/Hi3798MV200/Emmc/fastplay.img
# fi
cd ./out/target/product/Hi3798MV200/
mv Emmc/update.zip .
zip -r Emmc.zip Emmc
cd -

echo "success">>$1

echo "success..........."
