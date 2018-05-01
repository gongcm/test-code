#!/bin/bash

if [ -d imconing/rootfs ]
then
	echo "######## install_imconing start  ########"
	#chmod 755 imconing/rootfs/system/bin/*
	cp -arf imconing/rootfs/* out/target/product/Hi3798MV200/
	echo "######## install_imconing over ########"
else
  echo "######## no install_imconing ########"
fi

#mkdir out/target/product/Hi3798MV200/system -p
#chmod 777 -R out/target/product/Hi3798MV200/
#cp -f imconing/rootfs/* out/target/product/Hi3798MV200/