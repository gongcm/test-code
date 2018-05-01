#!/bin/sh
#otp_properties gethwtype
#hwtype=`getprop sys.ipanel.hwtype`
#if [ $hwtype = "1" ]
#then
#setprop eth0.login CNTSTBC
#setprop eth0.login.ext CNTSTBC
#else
#busybox ifconfig eth0 192.168.88.2 up > /dev/kmsg
# busybox route add -net 192.168.100.0 netmask 255.255.255.0 dev eth0 > /dev/kmsg
#busybox vconfig add eth0 4094 > /dev/kmsg
#busybox ifconfig eth0.4094 up > /dev/kmsg
#busybox vconfig set_flag eth0.4094 1 0 > /dev/kmsg
#busybox ifconfig eth0.4094 mtu 1490 > /dev/kmsg
#busybox ifconfig eth0.4094:1 192.168.100.20
#setprop eth0.4094.login CNTSTBC
#setprop eth0.4094.login.ext CNTSTBC
#fi
busybox ifconfig eth0 up > /dev/kmsg


chmod 777 /system/bin/RootShellServer
/system/bin/RootShellServer &

chmod 777 /system/bin/tvsserver
/system/bin/tvsserver &

chmod 777 /system/bin/tvmediaserver
/system/bin/tvmediaserver &

chmod 777 /system/bin/camanagerserver
/system/bin/camanagerserver &

#for OC adv
#chmod 777 /system/bin/carouselserver
#/system/bin/carouselserver &

#check and burn secboot
#chmod 777 /system/bin/burnsecboot
#/system/bin/burnsecboot &

setprop persist.sys.cvbs.and.hdmi "true"
setprop ro.config.max_starting_bg "6"
#setprop persist.sys.usb.config  "none"
#setprop ro.di.stb_factorytest_finish success
