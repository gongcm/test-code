#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>

#include "cutils/properties.h"
#include "jni.h"
#include <android/log.h>
#include "otp_properties.h"
#include "porting_partition_access_interface.h"
#include "RootShell.h"


#pragma once

#define LOG_TAG "OTP_properties"
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##args)

#define FLASH_PROPERTIES "properties"

#define NUM (64)
int prop_set_mac(void)
{

	int prop_handle;
	unsigned char buf[128*NUM] = {0};
	int ret = -1;
	int i = 0;
	char result[1024]={0};
	prop_handle = Porting_Flash_OpenByName((UINT_8*)FLASH_PROPERTIES);
	if(prop_handle == -1)
	{
		printf("Porting_Flash_OpenByName failed! ret = %d:\n", ret);
		return -1;
	}
	
	ret = Porting_Flash_Read(prop_handle,0, buf,sizeof(buf), FLASH_RW_FLAG_RAW); 
	if(ret == -1)
	{
		printf("Porting_Flash_Read failed! ret = %d:\n", ret);
		return -1;
	}
	Porting_Flash_Close(prop_handle);

	for(i=0; i<NUM; i++)
	{
		LOGE("buf :%s\n",(char*)buf+i*128);
		if ( 0==strcmp((char*)buf+i*128, "mac") )
		{
            char mac[128];
			char cmd[128];
			strcpy(mac,(char*)buf+i*128+32);
			sprintf(cmd,"busybox ifconfig eth0 hw ether %s up",mac);
			ret = RootShell_executeCommand(cmd, result, 1024);
			LOGI("executeCommand  %s ret = %d\n", cmd, ret);
        }
	}
	return 0;
}


int main(int agrc, char **argv)
{
    prop_set_mac();
    return 0;
}
