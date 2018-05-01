#!/system/bin/sh
busybox date -s 201501010000.00
source /system/etc/dtvlib_init.sh
# OptimizedNetwork
test -f /system/bin/hioptimizednetwork && /system/bin/hioptimizednetwork

QBENABLE=`getprop persist.sys.qb.enable`
case $QBENABLE in
 true)
  ;;
 *)
  setprop sys.insmod.ko 1
  ;;
 esac

#GPU
echo 0 > /sys/module/mali/parameters/mali_avs_enable
echo 750000 > /sys/module/mali/parameters/mali_dvfs_max_freqency


echo "\n\nWelcome to HiAndroid\n\n" > /dev/kmsg
LOW_RAM=`getprop ro.config.low_ram`
case $LOW_RAM in
 true)
  echo "\n\nenter low_ram mode\n\n" > /dev/kmsg
  #modules(memopt): Enable KSM in low ram device
  echo 1 > /sys/kernel/mm/ksm/run
  echo 300 > /sys/kernel/mm/ksm/sleep_millisecs
  ;;
 *)
  ;;
 esac
