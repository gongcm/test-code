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

#include "fpanel_led.h"

#define	KEYLED_MULTI_REGADDR	0xf8000044


#define LED_GPIO5_0 	(5*8+0)
#define	LED_GPIO5_4 	 44
#define	LED_GPIO5_3 	 43

#define	DIR_OUT		0
#define	DIR_IN			1
/*
*
*´ý»ú°´¼ü£ºGOPIO5_1 µÍµçÆ½ÓÐÐ§?
*ºìµÆ£ºGPIO5_4 ¸ßÁÁµÍÃð?
*ÂÌµÆ£ºGPIO5_0 ¸ßÁÁµÍÃð?
*À¶µÆ£ºGPIO5_3 ¸ßÁÁµÍÃð?
*
*
*
*/

//static int s_last_keyvalue = 0;
static int s_init_flag = 0;
int fpanel_ledcontrol_init(void)
{
	int ret;
	unsigned int regvalue = 0;
	if(s_init_flag)
	{
		return HI_SUCCESS;
	}
	ret = HI_SYS_Init();
#if 0	
	ret |= HI_SYS_ReadRegister(KEYLED_MULTI_REGADDR, &regvalue);
	regvalue = (regvalue & 0xfffffcff);
	regvalue = (regvalue | 0x00000100);
	
	regvalue = (regvalue & 0xffffffcf);	//ÉèÖÃGPIO5_5¡¢GPIO5_6ÎªGPIO¹¦ÄÜ
	ret |= HI_SYS_WriteRegister(KEYLED_MULTI_REGADDR, regvalue);
#endif	
	ret |= HI_UNF_GPIO_Init();
	
	ret |= HI_UNF_GPIO_SetDirBit(LED_GPIO5_0, DIR_OUT);
	ret |= HI_UNF_GPIO_SetDirBit(LED_GPIO5_4, DIR_OUT);
	ret |= HI_UNF_GPIO_SetDirBit(LED_GPIO5_3, DIR_OUT);
	
	ret |= HI_UNF_GPIO_WriteBit(LED_GPIO5_0, 1);//ÂÌµÆ
	if(ret != HI_SUCCESS)
	{
		printf("fpanel_ledcontrol_init fail.\n");
		return HI_FAILURE;
	}
	s_init_flag = 1;
	return HI_SUCCESS;
}

int fpanel_ledcontrol(LEVEL_TYPE value)
{
	int ret = -1;
	printf("[hetao][Porting_KeyLed_ledcontrol] IN. index = %d, value = %d\n",index, value);
	
	if(value == ON_GREEN_LEVEL)
	{

			ret |= HI_UNF_GPIO_WriteBit(LED_GPIO5_4, 0);//ºìµÆ
			ret |= HI_UNF_GPIO_WriteBit(LED_GPIO5_3, 0);//À¶µÆ
			ret |= HI_UNF_GPIO_WriteBit(LED_GPIO5_0, 1);//ÂÌµÆ
		if(ret != HI_SUCCESS)
		{
			return HI_FAILURE;
		}
	}
	else if(value == ON_RED_LEVEL)
	{
			ret |= HI_UNF_GPIO_WriteBit(LED_GPIO5_0, 0);//ÂÌµÆ
			ret |= HI_UNF_GPIO_WriteBit(LED_GPIO5_3, 0);//À¶µÆ
			ret |= HI_UNF_GPIO_WriteBit(LED_GPIO5_4, 1);//ºìµÆ

		if(ret != HI_SUCCESS)
		{
			return HI_FAILURE;
		}
	}
	else
	{
		ret |= HI_UNF_GPIO_WriteBit(LED_GPIO5_4, 0);//ºìµÆ
		ret |= HI_UNF_GPIO_WriteBit(LED_GPIO5_0, 0);//ÂÌµÆ
		ret = HI_UNF_GPIO_WriteBit(LED_GPIO5_3, 1);//À¶µÆ
		if(ret != HI_SUCCESS)
		{
			return HI_FAILURE;
		}
	}
	return HI_SUCCESS;
}

