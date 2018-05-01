#!/bin/bash
set -e
#export PATH="/home/root/bin/x86-arm/arm-hisiv200-linux/target/bin:/home/joy/tools/1.6.0_23-ubuntu-10.04/x86_64/bin:$PATH"
source /home/joy/shell/setlocale.sh
source /home/joy/tools/jdk1.6.0_23.sh
source /home/joy/tools/arm-hisiv200-linux.sh
#export PATH="/home/root/bin/x86-arm/arm-hisiv200-linux/target/bin:/home/joy/tools/jdk1.7.0_60/bin:$PATH"
echo "java -version"
java -version
source build/envsetup.sh
lunch Hi3798MV200-eng


MMM_PATH=$1
LOG_FILE=$2


echo $MMM_PATH
echo $LOG_FILE

mmm $MMM_PATH>>$LOG_FILE 2>>$LOG_FILE

echo "success"  >> $3
