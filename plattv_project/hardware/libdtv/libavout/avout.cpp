/* ----------------------------------------------------------------------------
 * @module
 *			HAL module
 * @routines
 *			demux device
 * @date
 *			2011.3
 */

#define LOG_TAG "[plattv]avout"

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <cutils/log.h>
#include <cutils/properties.h>

#include "hi_unf_hdmi.h"
#include "hi_unf_avplay.h"
#include "hi_unf_sound.h"
#include "hi_unf_vo.h"
#include "hi_unf_disp.h"
#include "hi_unf_gpio.h"//HuNan

#include "hardware/avout.h"

#include "hal_common.h"

static struct avout_context_t *m_avout_ctx = NULL;

extern HI_S32 avout_framer_picture2jpg(HI_UNF_VIDEO_FRAME_INFO_S *picture_info, char*bufaddr,
		int bufsize);

static int avout_get_list(struct avout_module_t *module, struct avout_t const **list);

static int avout_open(const struct hw_module_t* module, const char* name,
		struct hw_device_t** device);

static struct hw_module_methods_t avout_module_methods = {
	open: avout_open
};

struct avout_module_t HAL_MODULE_INFO_SYM = {
	common: {
		tag: HARDWARE_MODULE_TAG,
		version_major: 1,
		version_minor: 0,
		id: AVOUT_HARDWARE_MODULE_ID,
		name: "avout Module",
		author: "iPanel 2011, Inc.",
		methods: &avout_module_methods,
		dso:NULL,
		reserved: {0}
	},
	get_avout_list:avout_get_list,
};

/******************************************************************************/
static int avout_sounder_create(struct avout_device_t *dev, int handle)
{
    LOGD("sounder_create, handle=%x", handle);
    int i;
    snd_win_t * sw = (snd_win_t*)handle;

    for (i=0; i< AVOUT_MAX_NUM; i++)
    {
        if ( 0 == sw->snd[i].id ){
            sw->snd[i].id = i+1;
            LOGD("sounder_create, %d,%d", sw->id, i+1);
            return sw->id+sw->snd[i].id; //逗逼在不同handle上创建sounder,而在同一个handle上创建2个window
        }
    }
    LOGE("too many snd");
    return -1;
}

static int avout_sounder_destory(struct avout_device_t *dev, int handle, int sid)
{
    LOGD("sounder_destory, handle=%x, sid=%d", handle, sid);
    snd_win_t * sw = (snd_win_t*)handle;
    int ret;

    if ( sw->snd[sid-1-sw->id].adec ){
        LOGE("sounder not unbind form adec");
        return -1;
    }
    if ( sw->snd[sid-1-sw->id].handle ){
        ret = HI_UNF_SND_DestroyTrack (sw->snd[sid-1-sw->id].handle);
        if ( ret ){
            LOGE("HI_UNF_SND_DestroyTrack, Ret=%x", ret);
            return -1;
        }
    }
    sw->snd[sid-1-sw->id].id = 0;
    sw->snd[sid-1-sw->id].handle = 0;
	return 0;
}

static int avout_sounder_bind_avin_src(struct avout_device_t *dev, int handle, int sid,
		void *ai_dev, int ai_handle) {
	LOGE("sounder_bind_avin_src");
	return 0;
}

static int avout_sounder_bind_adec(struct avout_device_t *dev, int handle, int sid,
		void *avdec_dev, int avdec_handle)
{
    LOGD("sounder_bind_adec, handle=%x, trackid=%d, avdec_dev=%x, avdec_handle:%x",
        handle, sid, avdec_dev, avdec_handle);
    snd_win_t *sw = (snd_win_t*)handle;
    avplayer_t *av = (avplayer_t*)avdec_handle;

    if ( sw->snd[sid-1-sw->id].adec ){
        LOGE("already bind to %x", sw->snd[sid-1-sw->id].adec);
        return -1;
    }
    sw->snd[sid-1-sw->id].adec = av;
    av->out.snd.dev = dev;
    av->out.snd.handle = handle;
    av->out.snd.id = sid;

	return 0;
}

static int avout_sounder_unbind(struct avout_device_t *dev, int handle, int sid)
{
    LOGD("sounder_unbind, handle=%x, sid=%d", handle, sid);
    snd_win_t * sw = (snd_win_t*)handle;
    int ret;

    if ( sw->snd[sid-1-sw->id].adec ){
        avplayer_t *av = sw->snd[sid-1-sw->id].adec;
        if ( av->stat ){
            LOGE("adec not stop, can not unbind");
            return -1;
        }
        if ( sw->snd[sid-1-sw->id].handle && av->handle ){
            ret = HI_UNF_SND_Detach (sw->snd[sid-1-sw->id].handle, av->handle);
            if ( ret ){
                LOGE("HI_UNF_SND_Detach, Ret=%x", ret);
                return -1;
            }
        }
        av->out.snd.dev = NULL;
        av->out.snd.handle = 0;
        av->out.snd.id = 0;
        av->out.snd.attach = 0;
    }
    sw->snd[sid-1-sw->id].adec = NULL;

	return 0;
}

static int avout_sounder_set_volume(struct avout_device_t *dev, int handle, int sid, int db)
{
    int ret;
    snd_win_t * sw = (snd_win_t*)handle;
    LOGD("sounder_set_volume, handle=%x, sid=%d, db=%d", handle, sid, db);

	if (db < 0) db = 0;
    else if (db > 100) db = 100;

    sw->snd[sid-1-sw->id].volume = db;
	if(sw->snd[sid-1-sw->id].adec && sw->snd[sid-1-sw->id].adec->stat==2)
	{
		LOGD("sounder_set_volume,set change flag for avplay: %x", sw->snd[sid-1-sw->id].adec);
		sw->snd[sid-1-sw->id].adec->stat=1;
	}
    /*
    if ( sw->snd[sid-1-sw->id].handle ){
        HI_UNF_SND_GAIN_ATTR_S snd_attr;
        snd_attr.bLinearMode = HI_TRUE;
        snd_attr.s32Gain = db;
        ret = HI_UNF_SND_SetTrackWeight(sw->snd[sid-1-sw->id].handle, &snd_attr);
        if (HI_SUCCESS != ret) {
            LOGE("HI_UNF_SND_SetTrackWeight fail. ret=0x%0x\n", ret);
            return -1;
        }
    }
    */
    return 0;
}
#if 1
static int speaker_off() {
	LOGE("[HuNan off] %s\n", __FUNCTION__);
	
	HI_U32 BT5851S_PWROFF=9*8+0; 
	HI_U32 CPU_MUTE=2*8+1; 

	/*BT MUTE*/ 
	HI_UNF_GPIO_WriteBit(BT5851S_PWROFF, HI_FALSE); 
	HI_UNF_GPIO_WriteBit(CPU_MUTE, HI_TRUE); 

#if 0
	HI_SYS_WriteRegister(0x10203008, 0);

	HI_SYS_WriteRegister(0x1020300c, 0);
	HI_SYS_WriteRegister(0x101e7400, 0x14);

	HI_SYS_WriteRegister(0x101e73fc, 6);
#endif
	return 1;
}

static int speaker_on() {
	LOGE("[HuNan on] %s\n", __FUNCTION__);
	
		HI_U32 BT5851S_PWROFF=9*8+0; 
		HI_U32 CPU_MUTE=2*8+1; 


		/*BT Open*/ 
		HI_UNF_GPIO_WriteBit(BT5851S_PWROFF, HI_TRUE); 
		HI_UNF_GPIO_WriteBit(CPU_MUTE, HI_FALSE); 
#if 0	
	HI_SYS_WriteRegister(0x10203008, 0);

	HI_SYS_WriteRegister(0x1020300c, 0);
	HI_SYS_WriteRegister(0x101e7400, 0x15);

	HI_SYS_WriteRegister(0x101e73fc, 7);
#endif	
	return 1;
}
#endif
static int hdmi_audio_on() {
	HI_S32 Ret;
	HI_UNF_HDMI_ATTR_S hdmiAtrr;
	HI_UNF_HDMI_ID_E enHDMIId;
	enHDMIId = HI_UNF_HDMI_ID_0;

	LOGE("[yinhua] %s\n", __FUNCTION__);

	Ret = HI_UNF_HDMI_GetAttr(enHDMIId, &hdmiAtrr);
	if (Ret != HI_SUCCESS) {
		LOGE("HI_UNF_HDMI_GetAttr failed:%#x\n",Ret);
		return Ret;
	}
	hdmiAtrr.bEnableAudio = HI_TRUE;
	Ret = HI_UNF_HDMI_SetAttr(enHDMIId, &hdmiAtrr);
	if (Ret != HI_SUCCESS) {
		LOGE("HI_UNF_HDMI_SetAttr11 failed:%#x\n",Ret);
		return Ret;
	}
	return Ret;
}

static int hdmi_audio_off() {
	HI_S32 Ret;
	HI_UNF_HDMI_ATTR_S hdmiAtrr;
	HI_UNF_HDMI_ID_E enHDMIId;
	enHDMIId = HI_UNF_HDMI_ID_0;

	LOGE("[yinhua] %s\n", __FUNCTION__);

	Ret = HI_UNF_HDMI_GetAttr(enHDMIId, &hdmiAtrr);
	if (Ret != HI_SUCCESS) {
		LOGE("HI_UNF_HDMI_GetAttr failed:%#x\n",Ret);
		return Ret;
	}
	hdmiAtrr.bEnableAudio = HI_FALSE;
	Ret = HI_UNF_HDMI_SetAttr(enHDMIId, &hdmiAtrr);
	if (Ret != HI_SUCCESS) {
		LOGE("HI_UNF_HDMI_SetAttr22 failed:%#x\n",Ret);
		return Ret;
	}
	return Ret;
}

static int avout_sounder_ioctl(struct avout_device_t *dev, int handle, int trackid, int op,
		struct io_block_t*iob) {
	LOGE("[yinhua] %s  op=%d\n", __FUNCTION__,op);

	HI_S32 ret = HI_FAILURE;
	char *pchar = (char*) iob->in;
	int plen = iob->inlen;
	LOGI("audio_switch = %s, plen = %d ", pchar, plen);
	if (strncmp(pchar, "hdmi:1", plen) == 0) {
		hdmi_audio_on();
		speaker_off();
	} else if (strncmp(pchar, "speaker:1", plen) == 0) {
		speaker_on();
		hdmi_audio_off();
	}
	switch(op)
	{
		case AOUT_SOUNDER_OP_SET_TRACK_MODE:
			if(AVOUT_SOUNDER_MODE_LEFT_TO_MONO== *(int *)iob->in)
			{
				HI_UNF_SND_SetTrackMode(HI_UNF_SND_0,HI_UNF_SND_OUTPUTPORT_ALL,HI_UNF_TRACK_MODE_ONLY_LEFT);
			}
			else if(AVOUT_SOUNDER_MODE_LEFT_TO_MONO== *(int *)iob->in)
			{
				HI_UNF_SND_SetTrackMode(HI_UNF_SND_0,HI_UNF_SND_OUTPUTPORT_ALL,HI_UNF_TRACK_MODE_ONLY_RIGHT);
			}
			else
			{
				HI_UNF_SND_SetTrackMode(HI_UNF_SND_0,HI_UNF_SND_OUTPUTPORT_ALL,HI_UNF_TRACK_MODE_STEREO);
			}
			break;
		case AOUT_SOUNDER_OP_SET_OUTPUT_MODE:
			LOGI("AOUT_SOUNDER_OP_SET_OUTPUT_MODE mode %d ", *(int *)iob->in);

			if(AVOUT_SOUNDER_OUTPUT_HMDI_PCM== *(int *)iob->in)
			{
				HI_UNF_SND_SetHdmiMode(HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_HDMI0,
					HI_UNF_SND_HDMI_MODE_LPCM);
			}
			else if(AVOUT_SOUNDER_OUTPUT_HMDI_RAW== *(int *)iob->in)
			{
				HI_UNF_SND_SetHdmiMode(HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_HDMI0,
					HI_UNF_SND_HDMI_MODE_RAW);
			}
			else if(AVOUT_SOUNDER_OUTPUT_SPDIF_PCM== *(int *)iob->in)
			{
				HI_UNF_SND_SetSpdifMode(HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_SPDIF0,
						HI_UNF_SND_SPDIF_MODE_LPCM);
			}
			else if(AVOUT_SOUNDER_OUTPUT_SPDIF_RAW== *(int *)iob->in)
			{
				HI_UNF_SND_SetSpdifMode(HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_SPDIF0,
					HI_UNF_SND_SPDIF_MODE_RAW);
			}
			else
			{
				HI_UNF_SND_SetHdmiMode(HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_HDMI0,
					HI_UNF_SND_HDMI_MODE_LPCM);
				HI_UNF_SND_SetSpdifMode(HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_SPDIF0,
						HI_UNF_SND_SPDIF_MODE_LPCM);
					
			}
			break;		
		default:
			break;
	}
	//TODO
	LOGI("avout_sounder_ioctl Set Success");
	return 0;
}

static int avout_framer_create(struct avout_device_t *dev, int handle, int layid, int regnum)
{
    LOGD("framer_create, handle=%x, layid=%d, regnum=%d", handle, layid, regnum);
    int i, c=0;
    snd_win_t * sw = (snd_win_t*)handle;

    if ( AVOUT_LAYER_ID_UNDERLAY != layid ){
        LOGE("layid error");
        return -1;
    }

    for (i=0; i< AVOUT_MAX_NUM; i++)
    {
        if ( 0 == sw->win[i].id && c<regnum){
            sw->win[i].id = i+1;
            c++;
            LOGD("framer_create, id=%d", i);
        }
    }
    if (c==regnum) return c;

    LOGE("too many win");
    return -1;
}

static int avout_framer_set_region_bounds(struct avout_device_t *dev, int handle, int fid, int rid,
		struct int16_rect_t*r)
{
    LOGD("framer_set_region_bounds, handle=%x, fid=%d, rid=%d", handle, fid, rid);
    snd_win_t * sw = (snd_win_t*)handle;
	HI_UNF_WINDOW_ATTR_S WinAttr;
    int i, ret;
	char value[PROPERTY_VALUE_MAX] = {'\0'};
    i = rid;
	sw->dispwind=rid;
	if(sw->win[i].vdec && sw->win[i].vdec->stat==2)
	{
    	LOGD("framer_set_region_bounds, set change flag:%x", sw->win[i].vdec);
		sw->win[i].vdec->stat=1;
	}
    //for (i=0; i<=rid; i++){
        memcpy(&sw->win[i].r, r, sizeof(int16_rect_t));
        LOGD("[%d,%d,%d,%d]", r->x, r->y, r->w, r->h);
        if ( sw->win[i].handle ){
            ret = HI_UNF_VO_GetWindowAttr(sw->win[i].handle, &WinAttr);
            if ( ret ){
                LOGE("HI_UNF_VO_GetWindowAttr, Ret=%x", ret);
                return -1;
            }
			//本来是通过ioctl传下来的，应用和jointv没对好，这里就直接获取了
			property_get("persist.sys.video.ratio", value, "1");
			LOGD("avout_framer_set_region_bounds get persist.sys.video.ratio %s",value);
			if(strcmp(value,"0")==0)
            	WinAttr.stWinAspectAttr.enAspectCvrs = HI_UNF_VO_ASPECT_CVRS_LETTERBOX;
			else
				WinAttr.stWinAspectAttr.enAspectCvrs =HI_UNF_VO_ASPECT_CVRS_IGNORE;
            WinAttr.stOutputRect.s32X = r->x;
            WinAttr.stOutputRect.s32Y = r->y;
            WinAttr.stOutputRect.s32Width = r->w;
            WinAttr.stOutputRect.s32Height = r->h;
            ret = HI_UNF_VO_SetWindowAttr(sw->win[i].handle, &WinAttr);
            if (HI_SUCCESS != ret) {
                LOGE("HI_UNF_VO_SetWindowAttr failed. ret=0x%0x\n", ret);
                return -1;
            }
        }
        r++;
    //}
    /*
    LOGD("handle=%x, %d, window move to bottom", sw->win[i].vdec, sw->win[i].vdec->id);
    ret = HI_UNF_VO_SetWindowZorder(sw->win[i].handle, HI_LAYER_ZORDER_MOVEBOTTOM);
    if (HI_SUCCESS != ret) {
        LOGE("HI_UNF_VO_SetWindowZorder, Ret=%x",ret);
        return -1;
    }
    */
    return 0;
}

static int avout_framer_swap_region_zorder(struct avout_device_t *dev, int handle, int fid,
		int id1, int id2) {
	return -1;
}

static int avout_framer_set_region_alpha(struct avout_device_t *dev, int handle, int fid, int rid,
		int alpha) {
	return -1;
}

static int avout_framer_flush_regions(struct avout_device_t *dev, int handle, int fid) {
	return 0;
}

static int avout_framer_bind_avin_src(struct avout_device_t *dev, int handle, int fid, int ri,
		void *vi_dev, int vi_handle) {
	LOGE("[yinhua] %s\n", __FUNCTION__);
	return 0;
}

static int avout_framer_set_freeze(struct avout_device_t *dev, int handle, int fid,int ri, int b)
{
    LOGD("framer_set_freeze, handle=%x, fid=%d, rid=%d, b=%d", handle, fid, ri, b);
    snd_win_t * sw = (snd_win_t*)handle;

    sw->win[ri].freeze = b;

	return 0;
}

/**
 *@factions
 *			窗口bind 指定的avplay/vi
 *@param hVdec
 *			hVdec 暂设定为avplay句柄
 *@param index
 *			index 窗口索引，用于马赛克多窗口时使用。待扩展。
 */
static int avout_framer_bind_vdec(struct avout_device_t *dev, int handle, int fid, int ri,
		void *avdec_dev, int avdec_handle)
{
    LOGD("framer_bind_vdec, handle=%x, fid=%d, rid=%d, avdec_dev=%x, avdec_handle=%x", handle, fid, ri, avdec_dev, avdec_handle);
    snd_win_t * sw = (snd_win_t*)handle;
    avplayer_t *av = (avplayer_t*)avdec_handle;

    if ( sw->win[ri].vdec ){
        LOGE("already bind to %x", sw->win[ri].vdec);
        return -1;
    }

    sw->win[ri].vdec = av;
    av->out.win.dev = dev;
    av->out.win.handle = handle;
    av->out.win.framer = fid;
    av->out.win.id = ri;
    return 0;
}

static int avout_framer_unbind(struct avout_device_t *dev, int handle, int fid, int ri)
{
    LOGD("framer_unbind, handle=%x, fid=%d, rid=%d", handle, fid, ri);
    snd_win_t * sw = (snd_win_t*)handle;
    int ret;

    if ( sw->win[ri].vdec ){
        avplayer_t *av = sw->win[ri].vdec;
        if ( av->stat ){
            LOGE("vdec not stop, can not unbind");
            return -1;
        }
        if ( sw->win[ri].handle && av->handle ){
            ret = HI_UNF_VO_SetWindowEnable ( sw->win[ri].handle, HI_FALSE);
            ret |= HI_UNF_VO_DetachWindow ( sw->win[ri].handle, av->handle);
            if ( ret ){
                LOGE("HI_UNF_VO_DetachWindow, Ret=%x", ret);
                return -1;
            }
        }
        av->out.win.dev = NULL;
        av->out.win.handle = 0;
        av->out.win.framer = 0;
        av->out.win.id = 0;
        av->out.win.attach = 0;
    }
    sw->win[ri].vdec = NULL;
    return 0;
}

static int avout_framer_destroy(struct avout_device_t *dev, int handle, int fid)
{
    LOGD("framer_destroy, handle=%x, fid=%d", handle, fid);
    snd_win_t * sw = (snd_win_t*)handle;
    int i, ret;

    for (i=0; i<AVOUT_MAX_NUM; i++){
        if ( sw->win[i].vdec ){
            LOGE("window %d not unbind from vdec", i);
            return -1;
        }
        if ( sw->win[i].handle ){
            ret = HI_UNF_VO_DestroyWindow ( sw->win[i].handle );
            if ( ret ){
                LOGE("HI_UNF_VO_DestroyWindow, Ret=%x", ret);
                return -1;
            }
        }
        sw->win[i].id = 0;
        sw->win[i].handle = 0;
    }
    return 0;
}

static int avout_framer_ioctl(struct avout_device_t *dev, int handle, int framer, int op,
		struct io_block_t*iob) {
	LOGE("[yinhua] %s\n", __FUNCTION__);

	return -1;
}

static int avout_framer_capture(struct avout_device_t *dev, int handle, int fid, int ri,
		void*param)
{
    LOGD("framer_destroy, handle=%x, fid=%d, ri=%d, param=%x", handle, fid, ri, param);
    snd_win_t * sw = (snd_win_t*)handle;
	HI_S32 ret = HI_FAILURE;
	HI_UNF_VIDEO_FRAME_INFO_S cp;
	char* yuv_addr = NULL;
	char* RGB24 = NULL;
	int len = 0;
	HI_HANDLE whandle = sw->win[ri].handle;

	struct capturer_info_t *capinfo = (struct capturer_info_t*) param;
	if (!capinfo) {
		LOGD("avout_framer_capture_acquire param is null!");
		return -1;
	}
    if ( 0==sw->win[ri].handle ){
        LOGE("window not create yet");
        return -1;
    }
	memset(&cp, 0, sizeof(cp));
	ret = HI_UNF_VO_CapturePicture(whandle, &cp);
	if (HI_SUCCESS != ret) {
		LOGE("HI_UNF_VO_CapturePicture failed. ret=0x%0x\n", ret);
        return -1;
	}
	ret = avout_framer_picture2jpg(&cp, (char*) capinfo->bufaddr, capinfo->bufsize);
	LOGD("avout_framer_picture2jpg ret = %d",ret);

	capinfo->pic_format = AVOUT_RAW_FORMAT_JPG;
	capinfo->width = cp.u32Width;
	capinfo->height = cp.u32Height;
	capinfo->stride = cp.stVideoFrameAddr[0].u32YStride;
	capinfo->outsize = ret;
	LOGD("capinfo->outsize = %d",capinfo->outsize);
	LOGD("capinfo->bufsize = %d",capinfo->bufsize);
	LOGD("cp.u32Width = %u",cp.u32Width);
	LOGD("cp.u32Height = %u",cp.u32Height);
	LOGD("cp.enVideoFormat = %d",cp.enVideoFormat);

	ret = HI_UNF_VO_CapturePictureRelease(whandle, &cp);
	if (HI_SUCCESS != ret) {
		LOGE("HI_UNF_VO_CapturePictureRelease failed. ret=0x%0x\n", ret);
        return -1;
	}
	return 0;
}

static int framer_capture_release(struct avout_device_t *dev, int handle, int fid) {
	//TODO
	LOGE("[yinhua] %s\n", __FUNCTION__);
	return 0;
}

static int avout_set_margin(struct avout_device_t *dev, int handle, int left, int top, int right,
		int bottom) {
	//TODO
	LOGE("[yinhua] %s\n", __FUNCTION__);
	return 0;
}

static int avout_param_find_int(char* split, const char*name, int*value) {
	char *ptr = NULL;
	LOGE("[yinhua] %s\n", __FUNCTION__);
	LOGI("avout_param_find_intx split = %s,name = %s \n",split,name);
	ptr = strstr(split, name);
	if (ptr != NULL) {
		printf("ptr 111 = %s \n", ptr);
		ptr += strlen(name) + 1;
		printf("ptr 222 = %s \n", ptr);
		*value = atol(ptr);
		printf("value = %d \n", *value);
		return 0;
	}
	return -1;
}

static int avout_ioctl_param(struct avout_device_t *dev, int handle, int op, struct io_block_t*iob)
{
	int ret = -1;
	HI_S32 hiRet = HI_SUCCESS;
	int value;
	char *ptr = (char*) iob->in;
	int slen = (int) iob->inlen;
	char *split = ptr;
	HI_UNF_DISP_E enDisp = HI_UNF_DISPLAY1;

	LOGE("[yinhua] %s\n", __FUNCTION__);

	LOGI("avout_ioctl_param start(in buf = %s,len=%d)",ptr,slen);

	/* 设置DISP对比度 */
	ret = avout_param_find_int(split, "contrast", &value);
	LOGI("contrast = %d, ret = %d.",value,ret);
	if (ret == 0) {
		value = (value < 0) ? 0 : value;
		value = (value > 100) ? 100 : value;
		hiRet = HI_UNF_DISP_SetContrast(enDisp, value);
		if (hiRet != HI_SUCCESS) {
			LOGE("HI_UNF_DISP_SetContrast failed(value=%d,hiRet=0x%x)",value,hiRet);
		}
	}

	/* 设置DISP色调 */
	ret = avout_param_find_int(split, "hue", &value);
	LOGI("hue = %d, ret = %d.",value,ret);
	if (ret == 0) {
		value = (value < 0) ? 0 : value;
		value = (value > 100) ? 100 : value;
		hiRet = HI_UNF_DISP_SetHuePlus(enDisp, value);
		if (hiRet != HI_SUCCESS) {
			LOGE("HI_UNF_DISP_SetHuePlus failed(value=%d,hiRet=0x%x)",value,hiRet);
		}
	}

	/* 设置DISP亮度 */
	ret = avout_param_find_int(split, "brightness", &value);
	LOGI("brightness = %d, ret = %d.",value,ret);
	if (ret == 0) {
		value = (value < 0) ? 0 : value;
		value = (value > 100) ? 100 : value;
		hiRet = HI_UNF_DISP_SetBrightness(enDisp, value);
		if (hiRet != HI_SUCCESS) {
			LOGE("HI_UNF_DISP_SetBrightness failed(value=%d,hiRet=0x%x)",value,hiRet);
		}
	}

	/* 设置DISP饱和度 */
	ret = avout_param_find_int(split, "saturation", &value);
	LOGI("saturation = %d, ret = %d.",value,ret);
	if (ret == 0) {
		value = (value < 0) ? 0 : value;
		value = (value > 100) ? 100 : value;
		hiRet = HI_UNF_DISP_SetSaturation(enDisp, value);
		if (hiRet != HI_SUCCESS) {
			LOGE("HI_UNF_DISP_SetSaturation failed(value=%d,hiRet=0x%x)",value,hiRet);
		}
	}

	ret = avout_param_find_int(split, "sharpness", &value);

	LOGI("sharpness = %d, ret = %d.",value,ret);
	if (ret == 0) {
		value = (value < 0) ? 0 : value;
		value = (value > 100) ? 100 : value;
		LOGV("sharpness setting NOT support!@(value=%d,hiRet=0x%x)",value,hiRet);
	}

	/* 设置DISP背景色 */
	ret = avout_param_find_int(split, "bgcolor", &value);
	LOGI("bgcolor = %d, ret = %d.",value,ret);
	if (ret == 0) {
		HI_UNF_DISP_BG_COLOR_S bgColor;
		bgColor.u8Red = (value >> 16) & 0xff;
		bgColor.u8Green = (value >> 8) & 0xff;
		bgColor.u8Blue = (value) & 0xff;
		hiRet = HI_UNF_DISP_SetBgColor(enDisp, &bgColor);
		if (hiRet != HI_SUCCESS) {
			LOGE("HI_UNF_DISP_SetBgColor failed(bgColor=0x%x,hiRet=0x%x)",bgColor.u8Red,hiRet);
		}
	}

	LOGI("avout_ioctl_param end success");
	return 0;
}

static int avout_get_list(struct avout_module_t *module, struct avout_t const **list)
{
	int i = 0, j;

	avout_context_t *ctx = m_avout_ctx;

    if ( !ctx ){
        ctx = m_avout_ctx = (avout_context_t *)calloc(1, sizeof(avout_context_t));
        if ( !ctx ){
            LOGE("calloc ctx failed");
            return -1;
        }
    }
    if ( !ctx->list ){
    	ctx->list = (struct avout_t*) calloc(1,sizeof(struct avout_t) * AVOUT_MAX_NUM);
    	if ( !ctx->list ) {
            LOGE("calloc list failed");
    		return -1;
    	}

        for (i = 0; i < AVOUT_MAX_NUM; i++) {
            ctx->avout[i].id = i*AVOUT_MAX_NUM;
            ctx->list[i].handle = (int)&ctx->avout[i];
            ctx->list[i].display_width = 1280;
            ctx->list[i].display_height = 720;
            ctx->list[i].support_margin = 1;
            ctx->list[i].support_sounder = 1;
            ctx->list[i].max_sounder_volume = 100;
    		LOGD("get_list, %d,%x", i, ctx->list[i].handle);
            for (j=0;j<AVOUT_LAYER_IDMAXSIZE;j++)
            {
                switch (j)
                {
                /* 背景层 */
                case AVOUT_LAYER_ID_BACKGROUND:
                case AVOUT_LAYER_ID_BACKGROUND2:
                    {
                        ctx->list[i].layer[j].is_valid_layer = 1;
                        ctx->list[i].layer[j].layer_type = AVOUT_LAYER_TYPE_GRAPHIC;
                        ctx->list[i].layer[j].max_region_num = 16;
                        ctx->list[i].layer[j].display_bounds.x = 0;
                        ctx->list[i].layer[j].display_bounds.y = 0;
                        ctx->list[i].layer[j].display_bounds.w = 1280;
                        ctx->list[i].layer[j].display_bounds.h = 720;
                        ctx->list[i].layer[j].buffer_width = 1280;
                        ctx->list[i].layer[j].buffer_height = 720;
                        break;
                    }
                /* 下视频层 */
                case AVOUT_LAYER_ID_UNDERLAY:
                case AVOUT_LAYER_ID_UNDERLAY2:
                    {
                        ctx->list[i].layer[j].is_valid_layer = 1;
                        ctx->list[i].layer[j].layer_type = AVOUT_LAYER_TYPE_VIDEO;
                        ctx->list[i].layer[j].max_region_num = 16;
                        ctx->list[i].layer[j].display_bounds.x = 0;
                        ctx->list[i].layer[j].display_bounds.y = 0;
                        ctx->list[i].layer[j].display_bounds.w = 1920;
                        ctx->list[i].layer[j].display_bounds.h = 1080;
                        ctx->list[i].layer[j].buffer_width = 1920;
                        ctx->list[i].layer[j].buffer_height = 1080;
                        break;
                    }
                /* 图形层 */
                case AVOUT_LAYER_ID_GRAPHICS:
                case AVOUT_LAYER_ID_GRAPHICS2:
                    {
                        ctx->list[i].layer[j].is_valid_layer = 1;
                        ctx->list[i].layer[j].layer_type = AVOUT_LAYER_TYPE_GRAPHIC;
                        ctx->list[i].layer[j].max_region_num = 16;
                        ctx->list[i].layer[j].display_bounds.x = 0;
                        ctx->list[i].layer[j].display_bounds.y = 0;
                        ctx->list[i].layer[j].display_bounds.w = 1920;
                        ctx->list[i].layer[j].display_bounds.h = 1080;
                        ctx->list[i].layer[j].buffer_width = 1920;
                        ctx->list[i].layer[j].buffer_height = 1080;
                        break;
                    }
                /* 上视频层 */
                case AVOUT_LAYER_ID_OVERLAY:
                case AVOUT_LAYER_ID_OVERLAY2:
                    {
                        ctx->list[i].layer[j].is_valid_layer = 0;
                        ctx->list[i].layer[j].layer_type = AVOUT_LAYER_TYPE_VIDEO;
                        ctx->list[i].layer[j].max_region_num = 16;
                        ctx->list[i].layer[j].display_bounds.x = 0;
                        ctx->list[i].layer[j].display_bounds.y = 0;
                        ctx->list[i].layer[j].display_bounds.w = 1920;
                        ctx->list[i].layer[j].display_bounds.h = 1080;
                        ctx->list[i].layer[j].buffer_width = 1920;
                        ctx->list[i].layer[j].buffer_height = 1080;
                        break;
                    }
                /* 光标层 */
                case AVOUT_LAYER_ID_CURSOR:
                case AVOUT_LAYER_ID_CURSOR2:
                    {
                        ctx->list[i].layer[j].is_valid_layer = 1;
                        ctx->list[i].layer[j].layer_type = AVOUT_LAYER_TYPE_BITMAP;
                        ctx->list[i].layer[j].max_region_num = 16;
                        ctx->list[i].layer[j].display_bounds.x = 0;
                        ctx->list[i].layer[j].display_bounds.y = 0;
                        ctx->list[i].layer[j].display_bounds.w = 480;
                        ctx->list[i].layer[j].display_bounds.h = 320;
                        ctx->list[i].layer[j].buffer_width = 480;
                        ctx->list[i].layer[j].buffer_height = 320;
                        break;
                    }
                default:
                    LOGE("layer not support:%d", j);
                    break;
                }
            }
        }
    }
    (*list) = ctx->list;
	LOGD("get_list num=%d\n", AVOUT_MAX_NUM);
    return AVOUT_MAX_NUM;
}

/*****************************************************************************/

static int avout_close(struct hw_device_t *dev)
{
	int ret = 0;
	struct avout_context_t* ctx = (struct avout_context_t*) dev;

	if (ctx == NULL) {
		LOGE("[avout_close] null ctx.\n");
		return -1;
	}

	HI_UNF_VO_DeInit();

	HI_UNF_SND_Close( HI_UNF_SND_0);
	HI_UNF_SND_DeInit();

	if ( ctx->list ) {
		free(ctx->list);
		ctx->list = NULL;
	}

	if (ctx) {
		free(ctx);
		m_avout_ctx = NULL;
	}
    LOGD("close");
	return ret;
}

static int hdmi_init() {
	HI_S32 Ret;
	//HI_UNF_ENC_FMT_E enFormat;
	HI_UNF_HDMI_OPEN_PARA_S pstOpenPara;//yanglb sdk接口有两个版本的定义。暂时用这个编译通过，待续
	HI_UNF_HDMI_ID_E enHDMIId;

	LOGD("[yinhua] %s\n", __FUNCTION__);

	Ret = HI_UNF_DISP_Init();
	if (Ret != HI_SUCCESS) {
		LOGE("call HI_UNF_DISP_Init failed.%#x\n",Ret);
		return Ret;
	}

	pstOpenPara.enDefaultMode = HI_UNF_HDMI_DEFAULT_ACTION_HDMI;//yanglb
	enHDMIId = HI_UNF_HDMI_ID_0;

	Ret = HI_UNF_HDMI_Init();//yanglb 新接口里面声音空参数。
	if (HI_SUCCESS != Ret) {
		LOGE("HI_UNF_HDMI_Init failed:%#x\n",Ret);
		return Ret;
	}

	Ret = HI_UNF_HDMI_Open(enHDMIId, &pstOpenPara);//yanglb
	if (Ret != HI_SUCCESS) {
		LOGE("HI_UNF_HDMI_Open failed:%#x\n",Ret);
		return Ret;
	}
	return 0;
}

int avout_open(const struct hw_module_t* module, const char* name, struct hw_device_t** device)
{
	int ret = 0;
	avout_context_t *ctx = NULL;
    HI_UNF_SND_ATTR_S stAttr;

    hdmi_init();

    if (!strcmp(name, AVOUT_HARDWARE_AVOUT0)) {
        if ( !ctx ){
            ctx = m_avout_ctx = (avout_context_t *)calloc(1, sizeof(avout_context_t));
            if ( !ctx ){
                LOGE("calloc ctx failed");
                return -1;
            }
        }

		ctx->device.common.tag = HARDWARE_DEVICE_TAG;
		ctx->device.common.version = 1;
		ctx->device.common.module = const_cast<hw_module_t*> (module);
		ctx->device.common.close = avout_close;

		ctx->device.set_margin = avout_set_margin;
		ctx->device.ioctl = avout_ioctl_param;

		ctx->device.sounder_create = avout_sounder_create;
		ctx->device.sounder_destory = avout_sounder_destory;
		ctx->device.sounder_bind_avin_src = avout_sounder_bind_avin_src;
		ctx->device.sounder_bind_adec_src = avout_sounder_bind_adec;
		ctx->device.sounder_unbind_src = avout_sounder_unbind;
		ctx->device.sounder_set_volume = avout_sounder_set_volume;
		ctx->device.sounder_ioctl = avout_sounder_ioctl;

		ctx->device.framer_create = avout_framer_create;
		ctx->device.framer_destroy = avout_framer_destroy;
		ctx->device.framer_set_region_bounds = avout_framer_set_region_bounds;
		ctx->device.framer_swap_region_zorder = avout_framer_swap_region_zorder;
		ctx->device.framer_set_region_alpha = avout_framer_set_region_alpha;
		ctx->device.framer_flush_regions = avout_framer_flush_regions;
		ctx->device.framer_bind_avin_src = avout_framer_bind_avin_src;
		ctx->device.framer_set_freeze = avout_framer_set_freeze;
		ctx->device.framer_bind_vdec_src = avout_framer_bind_vdec;
		ctx->device.framer_unbind_src = avout_framer_unbind;
		ctx->device.framer_capture = avout_framer_capture;
		ctx->device.framer_ioctl = avout_framer_ioctl;
		
		ret = HI_UNF_GPIO_Init();
		if(ret != HI_SUCCESS){
			LOGE("[avout_GPIO] HI_UNF_GPIO_Init fail \n");	
		}
		ret = HI_UNF_SND_Init();
		if (ret != HI_SUCCESS) {
			LOGE("[avout_open] HI_UNF_SND_Init fail.\n");
			ret = -1;
			goto BAIL;
		}

		ret = HI_UNF_SND_GetDefaultOpenAttr(HI_UNF_SND_0, &stAttr);
		if (ret != HI_SUCCESS) {
			printf("call HI_UNF_SND_GetDefaultOpenAttr failed.\n");
			return ret;
		}

		ret = HI_UNF_SND_Open(HI_UNF_SND_0, &stAttr);
		if (ret != HI_SUCCESS) {
			LOGE("[avout_open]HI_UNF_SND_Open fail.\n");
			ret = -1;
		}

		LOGD("HI_UNF_VO_Init HI_UNF_VO_DEV_MODE_MOSAIC.");
		ret = HI_UNF_VO_Init(HI_UNF_VO_DEV_MODE_MOSAIC);
		if (HI_SUCCESS != ret) {
			LOGE("[avout_open] HI_UNF_VO_Init fail.\n");
			ret = -1;
			goto BAIL;
		}
		if (ret == 0) {
			*device = &ctx->device.common;
		} else {
			avout_close(&ctx->device.common);
		}

		LOGI("[avout_open] open succ!\n");
		return 0;
	}

	BAIL: LOGI("[avout_open] open fail!\n");
	return ret;
}

