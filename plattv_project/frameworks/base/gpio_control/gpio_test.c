#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#include <assert.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h> 
#include <unistd.h>

#include "hi_unf_gpio.h"
#include "hi_common.h"
#include "hi_type.h"

#define	STANDBY_MULTI_REGADDR	0xf8000044


#define CM_STANDBY_GPIO_PORT	42	//cable modem and wifiap  turn to real_standby


#define LED_GPIO5_0 	(5*8+0)
#define	LED_GPIO5_4 	 44
#define	LED_GPIO5_3 	 43


#define	DIR_OUT		0
#define	DIR_IN			1
#if 0
void main(int argc, char *argv[])
{
	#if 1
	int ret;
	unsigned int regvalue = 0;
	printf("[gpio_test] in.\n");
	ret = HI_SYS_Init();
	ret |= HI_SYS_ReadRegister(STANDBY_MULTI_REGADDR, &regvalue);
	regvalue = (regvalue & 0xfffffcff);
	regvalue = (regvalue | 0x00000100);
	
	regvalue = (regvalue & 0xffffffcf);	//ÉèÖÃGPIO5_5¡¢GPIO5_6ÎªGPIO¹¦ÄÜ
	ret |= HI_SYS_WriteRegister(STANDBY_MULTI_REGADDR, regvalue);
    
	ret |= HI_UNF_GPIO_Init();

	ret |= HI_UNF_GPIO_SetDirBit(CM_STANDBY_GPIO_PORT, DIR_OUT);
	if(ret != HI_SUCCESS)
	{
		printf("[gpio_test] fail 1.\n");
		return HI_FAILURE;
	}
	
	ret |= HI_UNF_GPIO_SetDirBit(LED_GPIO5_5, DIR_OUT);
	if(ret != HI_SUCCESS)
	{
		printf("[gpio_test] fail 1.\n");
		return HI_FAILURE;
	}
	
	ret |= HI_UNF_GPIO_SetDirBit(LED_GPIO5_6, DIR_OUT);
	if(ret != HI_SUCCESS)
	{
		printf("[gpio_test] fail 1.\n");
		return HI_FAILURE;
	}
	
	/*ret = HI_UNF_GPIO_WriteBit(CM_STANDBY_GPIO_PORT, 1);
	if(ret != HI_SUCCESS)
	{
		printf("[gpio_test] fail 2.\n");
		return HI_FAILURE;
	}*/
	while(1)
	{
		ret = HI_UNF_GPIO_WriteBit(LED_GPIO5_5, 1);
		if(ret != HI_SUCCESS)
		{
			printf("[gpio_test] fail 2.\n");
			return HI_FAILURE;
		}
		
		ret = HI_UNF_GPIO_WriteBit(LED_GPIO5_6, 0);
		if(ret != HI_SUCCESS)
		{
			printf("[gpio_test] fail 2.\n");
			return HI_FAILURE;
		}
		printf("[gpio_test] 111 000.\n");
		sleep(3);
		
		ret = HI_UNF_GPIO_WriteBit(LED_GPIO5_5, 0);
		if(ret != HI_SUCCESS)
		{
			printf("[gpio_test] fail 2.\n");
			return HI_FAILURE;
		}
		
		ret = HI_UNF_GPIO_WriteBit(LED_GPIO5_6, 1);
		if(ret != HI_SUCCESS)
		{
			printf("[gpio_test] fail 2.\n");
			return HI_FAILURE;
		}
		printf("[gpio_test] 000111.\n");
		sleep(3);
		ret = HI_UNF_GPIO_WriteBit(LED_GPIO5_5, 1);
		if(ret != HI_SUCCESS)
		{
			printf("[gpio_test] fail 2.\n");
			return HI_FAILURE;
		}
		
		ret = HI_UNF_GPIO_WriteBit(LED_GPIO5_6, 1);
		if(ret != HI_SUCCESS)
		{
			printf("[gpio_test] fail 2.\n");
			return HI_FAILURE;
		}
		printf("[gpio_test] 111111.\n");
		sleep(3);
	}
	#endif
	printf("[gpio_test] success.\n");
	return 0;
}
#else

void main(int argc, char *argv[])
{
	unsigned int regvalue = 0;
	/* jiuzhou wifi standby */
	unsigned int value = 0;
	int ret;
	
	ret=HI_UNF_GPIO_Init();
	printf("[HI_UNF_GPIO_Init]%x\n",ret);
	//ret=HI_UNF_GPIO_SetDirBit(5*8+0,0);
	ret |= HI_UNF_GPIO_SetDirBit(LED_GPIO5_0, DIR_OUT);
	ret |= HI_UNF_GPIO_SetDirBit(LED_GPIO5_4, DIR_OUT);
	ret |= HI_UNF_GPIO_SetDirBit(LED_GPIO5_3, DIR_OUT);
	printf("[HI_UNF_GPIO_SetDirBit]%x\n",ret);
//	ret=HI_UNF_GPIO_WriteBit(5*8+0,1);
	ret |= HI_UNF_GPIO_WriteBit(LED_GPIO5_4, 0);//ºìµÆ
	ret |= HI_UNF_GPIO_WriteBit(LED_GPIO5_3, 0);//À¶µÆ
	ret |= HI_UNF_GPIO_WriteBit(LED_GPIO5_0, 1);//ÂÌµÆ
	printf("[HI_UNF_GPIO_WriteBit]%x\n",ret);
	return 0;
}
#endif
