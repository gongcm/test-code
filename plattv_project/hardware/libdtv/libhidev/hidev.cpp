/* ----------------------------------------------------------------------------
 * @module 
 *			HAL module human machine interface
 * @routines 
 *			 alternately device (use tuner,demux,av dec/out common call)
 * @date
 *			2013.7
 */

#define LOG_TAG "[plattv]hidev"

#include <cutils/log.h>
#include <linux/fb.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <linux/input.h>
#include <cutils/properties.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <linux/android_pmem.h>

#include "hal_common.h"
#include "hardware/hidev.h"
#include "hi_unf_ecs.h"
#include "hi_unf_gpio.h"
#include "hi_type.h"

#include "fpanel_led.h"

#define AVPORT_MAX_NUM		(1)
#define DISPLAY_MAX_NUM		(1)
#define SPEAKER_MAX_NUM		(1)
#define FPANEL_MAX_NUM 		(1)

enum {
	HIDEV_TYPE_AVPORT = 0,
	HIDEV_TYPE_DISPLAY,
	HIDEV_TYPE_SPEAKER,
	HIDEV_TYPE_FRONTPANEL,
};


#define HIDEV_VIRTUAL_BASE_HANDLE (0x110)
#define HIDEV_MAX_OFFSET_NUM 	  (10)
#define HISI_GONGBAN 1
#define DELAY_TIME 200000
typedef struct hidev_context_t {
	struct avport_device_t device[3];
	struct fpanel_device_t fdevice;
	struct sndbox_device_t sdevice;
	int open_mask;

	int avportHanldle[AVPORT_MAX_NUM];
	int displayHandle[DISPLAY_MAX_NUM];
	int speakerHandle[SPEAKER_MAX_NUM];
	int fpanelHandle[FPANEL_MAX_NUM];
} hidev_context_t;

static struct hidev_context_t *g_hidev_ctx = NULL;
static HI_BOOL TaskRunning = HI_FALSE;
static HI_BOOL LedUpdate = HI_FALSE;
static HI_BOOL KeyLedIsOpen = HI_FALSE;
static HI_U32 LedData;

pthread_t ledTaskid;

/*共阴数码管显示码值表*/
static HI_U32 DigDisCode[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};      /*0 - 9*/       
/*大写字母只能够显示'A'、'C'、'E'、'F'、'H'、'I'、'L'、'O'、'P'、'S'、'U'、'V'，从左至右分别对应此数组中的非0x00值*/
static HI_U8 UCharDisCode[]= {
	0x77, 0x00, 0x39, 0x00, 0x79, 0x71, 0x00, 0x76, 0x06, 0x00,/* A -> J */
	0x00, 0x38, 0x00, 0x00, 0x3f, 0x73, 0x00, 0x00, 0x6d, 0x00,/* K -> T */
	0x3e, 0x3e, 0x00, 0x00, 0x00, 0x00						   /* U -> Z */
};
/*小写字母只能够显示'b'、'c'、'd'、'e'、'g'、'h'、'i'、'l'、'n'、'o'、'p'、'q'、's'、't'、'u'，从左至右分别对应此数组中的非0x00值*/
static HI_U8 LCharDisCode[]= {
	0x00, 0x7c, 0x58, 0x5e, 0x7b, 0x00, 0x6f, 0x74, 0x06, 0x00,/* a -> j */
	0x00, 0x38, 0x00, 0x54, 0x5c, 0x73, 0x67, 0x00, 0x6d, 0x78,/* k -> t */
	0x1c, 0x00, 0x00, 0x00, 0x00, 0x00					     /* u -> z */
};

/******************************** hidev interface declare *********************/
static int hidev_get_avport_list(struct hidev_module_t *m,
		struct avport_t const **list);
static int hidev_get_dpanel_list(struct hidev_module_t *m,
		struct dpanel_t const **list);
static int hidev_get_sndboxer_list(struct hidev_module_t *m,
		struct sndbox_t const **list);
static int hidev_get_fpanel_list(struct hidev_module_t *m,
		struct fpanel_t const **list);

static int hidev_device_open(const hw_module_t* module, const char* name,
		hw_device_t** device);
static int frontpanel_device_close(struct hw_device_t *dev);


/*****************************************************************************/
static struct hw_module_methods_t hidev_module_methods = {
	open: hidev_device_open
};

struct hidev_module_t HAL_MODULE_INFO_SYM = {
	common: {
		tag: HARDWARE_MODULE_TAG,
		version_major: 1,
		version_minor: 0,
		id: HIDEV_HARDWARE_MODULE_ID,
		name: "Shanxi Manchine Interfcae Module",
		author: "The Android Open Source Project",
		methods: &hidev_module_methods
	},
	get_avport_list:hidev_get_avport_list,
	get_dpanel_list:hidev_get_dpanel_list,
	get_sndbox_list:hidev_get_sndboxer_list,
	get_fpanel_list:hidev_get_fpanel_list,
};

static int hardware_match(){
	  char prop[96];
	  memset(prop,0,sizeof(prop));
	  property_get("android.hardware.type",prop,NULL);
	  return strcmp(prop,"hisi.hardware.normal") == 0?0:-1;
}

static int hidev_get_avport_list(struct hidev_module_t *m,
		struct avport_t const **list) {
	int num, i;
	int base_handle = 0;
	static struct avport_t* avport_list = NULL;
	if (g_hidev_ctx == NULL) {
		g_hidev_ctx = (struct hidev_context_t*) malloc(sizeof(struct hidev_context_t));
		if (g_hidev_ctx == NULL) {
			LOGE("hidev device malloc out of memory.");
			return -1;
		}
	}
	struct hidev_context_t *hidev_ctx = (struct hidev_context_t*) g_hidev_ctx;
  
  num =  hardware_match()==0?AVPORT_MAX_NUM:0 ;
  num = AVPORT_MAX_NUM;
	LOGD("hidev_get_avport_list > nomal num = %d",num);
	
	if (avport_list == NULL) {
		avport_list = (struct avport_t*) calloc(1, num);
		if (avport_list == NULL) {
			LOGE("avport malloc list out of memory!");
			return -1;
		}

		base_handle = HIDEV_VIRTUAL_BASE_HANDLE;
		for (i = 0; i < num; ++i) {
			avport_list[i].handle = base_handle + i;
			hidev_ctx->avportHanldle[i] = base_handle + i;
		}

		/* only support hdmi output */
		avport_list[0].id = AVPORT_ID_IN_HDMI;
		avport_list[0].has_data_channel = 0;
		avport_list[0].has_command_channel = 0;
		avport_list[0].has_audio_channel = 0;
		avport_list[0].has_video_channel = 0;
		avport_list[0].support_avin_mode = 0;
//		avport_list[0].type = HIDEV_TYPE_AVPORT;
//		avport_list[0].support_switch_2d_3d = 1;
//		avport_list[0].support_dynamic_bind = 1;
//		avport_list[0].component[0].has_component = 1;
//		avport_list[0].component[0].type = AVPORT_COMPONENT_AUDIO;
//		avport_list[0].component[1].has_component = 1;
//		avport_list[0].component[1].type = AVPORT_COMPONENT_VIDEO;
	}

	(*list) = avport_list;

	return num;
}

static int hidev_get_dpanel_list(struct hidev_module_t *m,
		struct dpanel_t const **list) {
	int num, i;
	int base_handle = 0;
	static struct dpanel_t* dpaneler_list = NULL;
	if (g_hidev_ctx == NULL) {
		g_hidev_ctx = (struct hidev_context_t*) malloc(sizeof(struct hidev_context_t));
		if (g_hidev_ctx == NULL) {
			LOGE("hidev device malloc out of memory.");
			return -1;
		}
	}
	struct hidev_context_t *hidev_ctx = (struct hidev_context_t*) g_hidev_ctx;
	
	num =  hardware_match()==0?DISPLAY_MAX_NUM:0 ;
	num = DISPLAY_MAX_NUM;
	LOGD("hidev_get_dpanel_list > nomal num = %d",num);
	

	if (dpaneler_list == NULL) {
		dpaneler_list = (struct dpanel_t*) calloc(1, num);
		if (dpaneler_list == NULL) {
			LOGE("displayer malloc list out of memory!");
			return -1;
		}

		base_handle = HIDEV_VIRTUAL_BASE_HANDLE + HIDEV_MAX_OFFSET_NUM;
		for (i = 0; i < num; ++i) {
			dpaneler_list[i].handle = base_handle + i;
			hidev_ctx->displayHandle[i] = base_handle + i;
		}

		dpaneler_list[0].dpi = 0;
		dpaneler_list[0].aspect_ratio = DPANEL_ASPECT_RATIO_16_9;
		dpaneler_list[0].resolution = DPANEL_RESOLUTION_OTHER;
		dpaneler_list[0].support_avin_mode = 1;
	}

	(*list) = dpaneler_list;

	return num;
}

static int hidev_get_sndboxer_list(struct hidev_module_t *m,
		struct sndbox_t const **list) {
	int num, i;
	int base_handle = 0;
	static struct sndbox_t* sndboxer_list = NULL;
	if (g_hidev_ctx == NULL) {
		g_hidev_ctx = (struct hidev_context_t*) malloc(sizeof(struct hidev_context_t));
		if (g_hidev_ctx == NULL) {
			LOGE("hidev device malloc out of memory.");
			return -1;
		}
	}
	struct hidev_context_t *hidev_ctx = (struct hidev_context_t*) g_hidev_ctx;
  
	
	num =  hardware_match()==0?DISPLAY_MAX_NUM:0 ;
	num = SPEAKER_MAX_NUM;
	LOGD("hidev_get_sndboxer_list > nomal num = %d",num);

	if (sndboxer_list == NULL) {
		sndboxer_list = (struct sndbox_t*) calloc(1, num);
		if (sndboxer_list == NULL) {
			LOGE("speaker malloc list out of memory!");
			return -1;
		}

		base_handle = HIDEV_VIRTUAL_BASE_HANDLE + HIDEV_MAX_OFFSET_NUM * 2;
		for (i = 0; i < num; ++i) {
			sndboxer_list[i].handle = base_handle + i;
			hidev_ctx->speakerHandle[i] = base_handle + i;
		}

		sndboxer_list[0].placement = SNDBOX_PLACEMENT_1_0;
		sndboxer_list[0].support_avin_mode = 0;
	}

	(*list) = sndboxer_list;

	return num;
}

static int hidev_get_fpanel_list(struct hidev_module_t *m,
		struct fpanel_t const **list) {
	int num, i;
	int base_handle = 0;
	static struct fpanel_t* fpanel_list = NULL;
	if (g_hidev_ctx == NULL) {
		g_hidev_ctx = (struct hidev_context_t*) malloc(sizeof(struct hidev_context_t));
		if (g_hidev_ctx == NULL) {
			LOGE("hidev device malloc out of memory.");
			return -1;
		}
	}
	struct hidev_context_t *hidev_ctx = (struct hidev_context_t*) g_hidev_ctx;
  
	if(hidev_ctx == NULL)
	{
		LOGE("hidev_ctx error");
		hidev_ctx = (struct hidev_context_t*) malloc(sizeof(struct hidev_context_t));
	}
		
	//num =  hardware_match()==0?FPANEL_MAX_NUM:0 ;
	num = FPANEL_MAX_NUM;
	LOGD("hidev_get_fpanel_list > nomal num = %d",num);

	if (fpanel_list == NULL) {
		fpanel_list = (struct fpanel_t*) calloc(1, num);
		if (fpanel_list == NULL) {
			LOGE("frontpanel malloc list out of memory!");
			return -1;
		}

		base_handle = HIDEV_VIRTUAL_BASE_HANDLE + HIDEV_MAX_OFFSET_NUM * 3;
		for (i = 0; i < num; i++) {
			fpanel_list[i].handle = base_handle + i;
			LOGI("i = %d, handle = %d.", i, fpanel_list[i].handle);
			hidev_ctx->fpanelHandle[i] = base_handle + i;
		}

		fpanel_list[0].led_max_num = 4;
		fpanel_list[0].is_remote = 0;
	}

	(*list) = fpanel_list;

	return num;
}

static int hidev_handletoindex(int handle, int *hi_type) {
	int index = -1;
	int i, n;
	struct hidev_context_t *hidev_ctx = (struct hidev_context_t*) g_hidev_ctx;

	for (i = 0; i < AVPORT_MAX_NUM; ++i) {
		if (handle == hidev_ctx->avportHanldle[i]) {
			*hi_type = HIDEV_TYPE_AVPORT;
			index = handle - HIDEV_VIRTUAL_BASE_HANDLE;
			return index;
		}
	}

	for (i = 0; i < DISPLAY_MAX_NUM; ++i) {
		if (handle == hidev_ctx->displayHandle[i]) {
			*hi_type = HIDEV_TYPE_DISPLAY;
			index = handle - (HIDEV_VIRTUAL_BASE_HANDLE + HIDEV_MAX_OFFSET_NUM);
			return index;
		}
	}

	for (i = 0; i < SPEAKER_MAX_NUM; ++i) {
		if (handle == hidev_ctx->speakerHandle[i]) {
			*hi_type = HIDEV_TYPE_SPEAKER;
			index = handle - (HIDEV_VIRTUAL_BASE_HANDLE + HIDEV_MAX_OFFSET_NUM
					* 2);
			return index;
		}
	}

	for (i = 0; i < FPANEL_MAX_NUM; ++i) {
		if (handle == hidev_ctx->fpanelHandle[i]) {
			*hi_type = HIDEV_TYPE_FRONTPANEL;
			index = handle - (HIDEV_VIRTUAL_BASE_HANDLE + HIDEV_MAX_OFFSET_NUM
					* 3);
			return index;
		}
	}

	return -1;
}
//net_lock  :1 net    :0 lock
static int hidev_fpanel_set_light(struct fpanel_device_t*d, int handle, const char*name, int color, int flags)
{
	const char * light1 =  "turning_light";
	const char * light2 = "network_light";
	int ret = -1;
	if(name == NULL)
		return ret;
	LOGI("hidev_fpanel_set_light name  = %s ,color = %d \n" , name,color);
	
#if 0
	if(!strcmp(name, "power_light"))
	{
		if(color)
			fpanel_ledcontrol(ON_GREEN_LEVEL);
		else if(color == 0)
			fpanel_ledcontrol(ON_RED_LEVEL);
	}
#endif

#if 0 //yanglb
	struct fpanel_light_info_t * t = (struct fpanel_light_info_t *)(iob->in);
	LOGI("hidev_fpanel_set_light name  = %s ,color = %d \n" , t->name,t->color);
	if(0 ==strcmp(t->name,light1))
	{
		if(1 == t->color)
		{
			
			ret = HI_UNF_LED_Lock_Stats(HI_TRUE);
			if(HI_SUCCESS != ret)
			{
				LOGE("HI_UNF_LED_Lock_Stats err ret = 0x%x\n",ret);
				return -1;
			}
			LOGI("set lock led is success");
		}
		else
		{
			ret = HI_UNF_LED_Lock_Stats(HI_FALSE);
			if(HI_SUCCESS != ret)
			{
				LOGE("HI_UNF_LED_Lock_Stats err ret = 0x%x\n",ret);
				return -1;
			}
		}
	}
	else if(0 == strcmp(t->name,light2))
	{
		if(1 == t->color)
		{
			ret = HI_UNF_LED_Net_Stats(HI_TRUE);
			if(HI_SUCCESS != ret)
			{
				LOGE("HI_UNF_LED_Lock_Stats err ret = 0x%x\n",ret);
				return -1;
			}
			LOGI("set net led is success");
		}
		else
		{	
			ret = HI_UNF_LED_Net_Stats(HI_FALSE);
			if(HI_SUCCESS != ret)
			{
				LOGE("HI_UNF_LED_Lock_Stats err ret = 0x%x\n",ret);
				return -1;
			}
		}	
	}
#endif
	return 0;
}


/*int (*set_text)(struct fpanel_device_t*d, int handle, const char*txt, int flags);*/

static int hidev_fpanel_set_text(struct fpanel_device_t*d, int handle, const char* txt, int flags)
{
	int ret = -1, hidev_type;		
	int index = -1;
	ALOGI("hidev_fpanel_set_text handle=%d", handle);
	index = hidev_handletoindex(handle, &hidev_type);
	if (index != 0) {
		LOGE("hidev ioctl> handle convert to index failed");
		return -1;
	}
	unsigned char DispCode[5] = {0};
	unsigned char chr[5] = {0};
	int j;
	memcpy((void *)chr,txt,5);
#if 1
	for(int i = 0;i < 4;i++)
	{
		ALOGI("led is %s\n" , chr);
		if(chr[i] == ' ')
		{
			chr[i] =chr[i+1] ;
			chr[i+1] = ' ';
		}
	}
	if(chr[2] == ' ' )
	{
		chr[2] = chr[3];
	}
	memcpy((void *)DispCode +1,chr,3);
#endif
	
	for (j = 0; j < 4; j++)
	{
		LOGI("DispCode[%d]:%c.",j,DispCode[j] );
	    if ((DispCode[j] >= 'A') && (DispCode[j] <= 'Z'))
	    {
	        DispCode[j] = UCharDisCode[DispCode[j] - 'A'];
	    }
	    else if ((DispCode[j] >= 'a') && (DispCode[j] <= 'z'))
	    {
	        DispCode[j] = LCharDisCode[DispCode[j] - 'a'];
	    }
	    else if ((DispCode[j] >= '0') && (DispCode[j] <= '9'))
	    {
	        DispCode[j] = DigDisCode[DispCode[j] - '0'];
	    }
	    else if (DispCode[j] == '-')
	    {
	         DispCode[j] = 0x40; 
	    }
	    else if (DispCode[j] == '_')
	    {
	        DispCode[j] = 0x10;
	    }
	    else if (DispCode[j] == ':' ||DispCode[j] == '.' )
	    {
	        DispCode[j]=0x01;
	    }
	    else  /* FIXME: may be should add symbol ':' and '.' */
	    {
	        DispCode[j] = 0x0;  /* default: not display */
	    }
	
	}
	LedUpdate = HI_FALSE;
	LedData = (DispCode[3]<<24 )|(DispCode[2]<<16 )|(DispCode[1]<<8 )|(DispCode[0]);
	LedUpdate = HI_TRUE;
	ALOGI("led discode is  0x%08x\n" , LedData);
	return 0;

}
static int hidev_fpanel_display_string(int index, struct io_block_t*iob, int time) 
{
	int ret = -1;
	unsigned char DispCode[5] = {0};
	int j;
	ALOGI("[halhidev] %s \n", __FUNCTION__);
	if(iob == NULL)
		return ret;

	struct fpanel_text_info_t * t =  (struct fpanel_text_info_t *)(iob->in);
	memcpy((void *)DispCode,t->text,4);
	LOGI("led discode is  %s \n" , DispCode );
    for (j = 0; j < 4; j++)
    {
        if ((DispCode[j] >= 'A') && (DispCode[j] <= 'Z'))
        {
            DispCode[j] = UCharDisCode[DispCode[j] - 'A'];
        }
        else if ((DispCode[j] >= 'a') && (DispCode[j] <= 'z'))
        {
            DispCode[j] = LCharDisCode[DispCode[j] - 'a'];
        }
        else if ((DispCode[j] >= '0') && (DispCode[j] <= '9'))
        {
            DispCode[j] = DigDisCode[DispCode[j] - '0'];
        }
        else if (DispCode[j] == '-')
        {
             DispCode[j] = 0x40; 
        }
        else if (DispCode[j] == '_')
        {
            DispCode[j] = 0x10;
        }
        else if (DispCode[j] == ':' ||DispCode[j] == '.' )
        {
            DispCode[j]=0x01;
        }
        else  /* FIXME: may be should add symbol ':' and '.' */
        {
            DispCode[j] = 0x0;  /* default: not display */
        }
    }
	LedUpdate = HI_FALSE;
	LedData = (DispCode[3]<<24 )|(DispCode[2]<<16 )|(DispCode[1]<<8 )|(DispCode[0]);
	LedUpdate = HI_TRUE;
	ALOGI("led discode is  0x%08x\n" , LedData);
	return 0;
}


#if 1
static int speaker_off() {
	LOGE("[HuNan off] %s\n", __FUNCTION__);
	
	HI_U32 BT5851S_PWROFF=9*8+0; 
	HI_U32 CPU_MUTE=2*8+1;
	int ret;

	/*BT MUTE*/
	ret = HI_UNF_GPIO_SetDirBit(BT5851S_PWROFF, HI_FALSE);
	ret |= HI_UNF_GPIO_SetDirBit(CPU_MUTE, HI_FALSE);
	ret |= HI_UNF_GPIO_WriteBit(BT5851S_PWROFF, HI_FALSE); 
	ret |= HI_UNF_GPIO_WriteBit(CPU_MUTE, HI_TRUE);
	if(ret != HI_SUCCESS)
	{
		printf("speaker_off fail.\n");
	}
	return 1;
}

static int speaker_on() {
	LOGE("[HuNan on] %s\n", __FUNCTION__);

	HI_U32 BT5851S_PWROFF=9*8+0; 
	HI_U32 CPU_MUTE=2*8+1;
	int ret;

	/*BT Open*/
	ret = HI_UNF_GPIO_SetDirBit(BT5851S_PWROFF, HI_FALSE);
	ret |= HI_UNF_GPIO_SetDirBit(CPU_MUTE, HI_FALSE);
	ret |= HI_UNF_GPIO_WriteBit(BT5851S_PWROFF, HI_TRUE); 
	ret |= HI_UNF_GPIO_WriteBit(CPU_MUTE, HI_FALSE); 
	if(ret != HI_SUCCESS)
	{
		printf("speaker_on fail.\n");
	}

	return 1;
}
#endif

static int hidev_sndbox_ioctl(struct sndbox_device_t*d, int handle, int op,struct io_block_t*iob){
	int ret = -1, hidev_type;		
	int index = -1;
	ALOGI("hidev_ioctl handle=%d op=%d iob=%s", handle, op, iob->in);
	index = hidev_handletoindex(handle, &hidev_type);
	if (index != 0) {
		LOGE("hidev ioctl> handle convert to index failed");
		return -1;
	}
	switch(op)
	{
	case 3:
	{
		int* sndmode = (int*)(iob->in);
		if(*sndmode==1)
		{
			speaker_on();
		}
		else
		{
			speaker_off();
		}
		break;
	}
	default:
		LOGW("op no support ");
		break;
	}	
	return 0;
} 



static int hidev_ioctl(struct fpanel_device_t*d, int handle, int op,
		struct io_block_t*iob) {
		
	int ret = -1, hidev_type;		
	int index = -1;
	ALOGI("hidev_ioctl handle=%d op=%d iob=%s", handle, op, iob->in);
	index = hidev_handletoindex(handle, &hidev_type);
	if (index != 0) {
		LOGE("hidev ioctl> handle convert to index failed");
		return -1;
	}

#if 1
	switch (op) {
	case FPANEl_IOCTL_SET_LIGHT:
		assert(hidev_type == HIDEV_TYPE_FRONTPANEL);
		//ret = hidev_fpanel_set_light(index, iob);
		break;		
	case FPANEl_IOCTL_SET_TEXT:
		assert(hidev_type == HIDEV_TYPE_FRONTPANEL);
		ret = hidev_fpanel_display_string(index, iob, 0);
		break;

	case FPANEl_IOCTL_UNKNOWN:
	default:
		LOGE("hidev ioctl> illegal args(%d)", op);
		break;
	}
#endif

	return 0;
}

static int hidev_poll_states(struct avport_device_t*d, int*ids_buf, int n){
	return -1; /*TODO*/
}

static int hidev_get_state(struct avport_device_t*d, int id){
	return -1; /*TODO*/
}
static int hidev_set_avin_mode(struct avport_device_t*d, int handle, int b){
	return -1; /*TODO*/
}
static int hidev_get_avin_info(struct avport_device_t*d, int handle) {
	return -1;/*TODO*/
}

static int hidev_avin_open(struct avport_device_t*d, int handle) {
	return -1;/*TODO*/
}

static int hidev_avin_close(struct avport_device_t*d, int handle) {
	frontpanel_device_close(&d->common);
	return -1;/*TODO*/
}

static int hidev_avin_start(struct avport_device_t*d, int handle) {
	return -1;/*TODO*/
}

static int hidev_avin_stop(struct avport_device_t*d, int handle) {
	return -1;/*TODO*/
}

static int hidev_avin_ioctl(struct avport_device_t*d, int index, int op,
		struct io_block_t*iob) {
	return -1;/*TODO*/
}

static int avport_device_close(struct hw_device_t *dev) {
	return 0;
}

static int display_device_close(struct hw_device_t *dev) {
	return 0;
}

static int speaker_device_close(struct hw_device_t *dev) {
	return 0;
}

static int frontpanel_device_close(struct hw_device_t *dev) {
	HI_S32 ret = -1;

	if (g_hidev_ctx != NULL) {
		g_hidev_ctx->open_mask &= ~0x08;
		if (g_hidev_ctx->open_mask == 0) {
			free(g_hidev_ctx);
			g_hidev_ctx = NULL;
		}
	}

	TaskRunning = HI_FALSE;
	pthread_join(ledTaskid, 0);
	#ifdef PROJ_DALIAN_JIULIAN
	pthread_join(lockDetectid, 0);
	#endif
	
#if 1 //yanglb
	ret = HI_UNF_KEY_Close();
	if (ret != HI_SUCCESS) 
	{
		LOGE("HI_UNF_KEY_Close failed ret = 0x%x\n",ret);
		return -1;
	}

	
	ret = HI_UNF_LED_Close();
	if (ret != HI_SUCCESS)
	{
		LOGE("HI_UNF_LED_Close failed ret = 0x%x\n",ret);
		return -1;
	}
	ret = HI_UNF_KEYLED_DeInit();
	if (ret != HI_SUCCESS)
	{
		LOGE("HI_UNF_KEYLED_DeInit failed ret = 0x%x\n",ret);
		return -1;
	}
	KeyLedIsOpen = HI_FALSE;
#endif

	return 0;
}

static int avport_device_open(hw_module_t const* module, const char* name,
		avport_device_t** device) {
	return 0;
}

static int display_device_open(hw_module_t const* module, const char* name,
		avport_device_t** device) {
	return 0;
}

static int speaker_device_open(hw_module_t const* module, const char* name, sndbox_device_t** device) 
{
	(*device)->ioctl = hidev_sndbox_ioctl;
	return 0;
}

void * LED_DisplayTask(void *args)
{
	HI_S32 ret;
	while(TaskRunning == HI_TRUE)
	{
		//if(LedUpdate)
		{
			ret = HI_UNF_LED_Display(LedData);
			if(HI_SUCCESS != ret)
			{
				ALOGE("HI_UNF_LED_Display err ret = 0x%x\n",ret);
				break;
			}
			LedUpdate = HI_FALSE;
		}
		usleep(DELAY_TIME);
	}
	TaskRunning = HI_FALSE;
	return 0;
}

static int frontpanel_device_open(hw_module_t const* module, const char* name,
		fpanel_device_t** device) {
	HI_S32 ret = -1;
	char value[128];
	LOGI("[LINE:%d] %s",__LINE__,__FUNCTION__);
	//fpanel_light_info_t ft;
	property_set("lock.stats","0");//tuner灯一般情况下为亮
	property_set("net.stats","0");//ConnectivityService.java开机如果网线断开了 不会检测网络的状态 

	if(KeyLedIsOpen == HI_TRUE)
	{
        LOGE("frontpanel_device_open is opend\n");
        return 0;		
	}
	ret = HI_SYS_Init();
	if(HI_SUCCESS != ret)
	{ 
		ALOGE("HI_SYS_Init err  ret = %d \n",ret);
		return -1;
	}
	//fpanel_ledcontrol_init();
	KeyLedIsOpen = HI_TRUE;

	LedUpdate = HI_TRUE;

	(*device)->common.tag = HARDWARE_DEVICE_TAG;
	(*device)->common.version = 1;
	(*device)->common.module = const_cast<hw_module_t*> (module);
	(*device)->common.close = frontpanel_device_close;
	(*device)->set_text = hidev_fpanel_set_text;
	(*device)->set_light = hidev_fpanel_set_light;
	(*device)->ioctl = hidev_ioctl;
	ALOGI("Exit frontpanel_device_open.");

	return 0;
ERR1:
	HI_UNF_LED_Close();
	HI_UNF_KEYLED_DeInit();
ERR2:
	HI_UNF_KEY_Close();
ERR3:
	TaskRunning = HI_FALSE;
	return -1;
}

static int hidev_device_open(const hw_module_t* module, const char* name,
		hw_device_t** device) {
	int status = -EINVAL;
	struct avport_device_t* hidev_device = NULL;
	struct fpanel_device_t* hidev_device_fpanel = NULL;
	struct sndbox_device_t* hidev_device_sndbox = NULL;
	LOGI("[LINE:%d] %s, name=%s",__LINE__,__FUNCTION__, name);
	if (g_hidev_ctx == NULL) {
		g_hidev_ctx = (struct hidev_context_t*) malloc(sizeof(struct hidev_context_t));
		if (g_hidev_ctx == NULL) {
			LOGE("hidev device malloc out of memory.");
			return status;
		}
	}

	if (!strcmp(name, HIDEV_HARDWARE_AVPORT0)) {
		g_hidev_ctx->open_mask |= 0x01;
		hidev_device = &g_hidev_ctx->device[HIDEV_TYPE_AVPORT];
		status = avport_device_open(module, name, &hidev_device);
		//hidev_device->ioctl = hidev_ioctl;
		hidev_device->poll_states = hidev_poll_states;
		hidev_device->get_state = hidev_get_state;
		hidev_device->set_avin_mode = hidev_set_avin_mode;
		*device = &(hidev_device->common);
	} else if (!strcmp(name, HIDEV_HARDWARE_DPANEL0)) {
		g_hidev_ctx->open_mask |= 0x02;
		hidev_device = &g_hidev_ctx->device[HIDEV_TYPE_DISPLAY];
		status = display_device_open(module, name, &hidev_device);
		//hidev_device->ioctl = hidev_ioctl;
		hidev_device->poll_states = hidev_poll_states;
		hidev_device->get_state = hidev_get_state;
		hidev_device->set_avin_mode = hidev_set_avin_mode;
		*device = &(hidev_device->common);
	} else if (!strcmp(name, HIDEV_HARDWARE_SNDBOX0)) {
		g_hidev_ctx->open_mask |= 0x04;
		hidev_device_sndbox = &g_hidev_ctx->sdevice;
		status = speaker_device_open(module, name, &hidev_device_sndbox);
		if (status == 0) {
			*device = &(hidev_device_sndbox->common);
		} else {
			LOGE("hidev device open failed!");
		}
	} 
	 if (!strcmp(name, HIDEV_HARDWARE_FPANEL0)) {
		g_hidev_ctx->open_mask |= 0x08;
		hidev_device_fpanel = &g_hidev_ctx->fdevice;
		status = frontpanel_device_open(module, name, &hidev_device_fpanel);
		if (status == 0) {
			*device = &(hidev_device_fpanel->common);
		} else {
			LOGE("hidev device open failed!");
		}
	}
	return status;
}

