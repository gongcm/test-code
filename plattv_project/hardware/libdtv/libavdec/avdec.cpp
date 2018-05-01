/* ----------------------------------------------------------------------------
 * @module
 *			HAL module
 * @routines
 *			video decoder device
 * @date
 *			2011.3
 */
//#define LOG_NDEBUG		0
#define LOG_TAG "[plattv]avdec"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <cutils/log.h>

#include "HA.AUDIO.G711.codec.h"
#include "HA.AUDIO.MP3.decode.h"
#include "HA.AUDIO.MP2.decode.h"
#include "HA.AUDIO.AAC.decode.h"
#include "HA.AUDIO.AC3PASSTHROUGH.decode.h"
//#include "HA.AUDIO.AC3.decode.h"
//#include "HA.AUDIO.DTS.decode.h"
#include "HA.AUDIO.DTSPASSTHROUGH.decode.h"
#include "HA.AUDIO.DTSHD.decode.h"
#include "HA.AUDIO.DRA.decode.h"
#include "HA.AUDIO.PCM.decode.h"
#include "HA.AUDIO.WMA9STD.decode.h"
#include "HA.AUDIO.AMRNB.codec.h"
//#include "HA.AUDIO.TRUEHD.decode.h"
#include "HA.AUDIO.TRUEHDPASSTHROUGH.decode.h"
//#include "HA.AUDIO.EAC3.decode.h"
#include "HA.AUDIO.DOLBYPLUS.decode.h"
#include "HA.AUDIO.FFMPEG_DECODE.decode.h"
#include "HA.AUDIO.AAC.encode.h"
#include "HA.AUDIO.DTSHD.decode.h"
#include "HA.AUDIO.COOK.decode.h"

#include "hi_unf_demux.h"
#include "hi_unf_avplay.h"
#include "hi_unf_sound.h"
#include "hi_unf_vo.h"
#include "hi_unf_video.h"

#include "hal_common.h"
#include "hardware/avdec.h"

DOLBYPLUS_STREAM_INFO_S g_stDDpStreamInfo;
HI_U32 g_u32DolbyAcmod = 0;
HI_BOOL g_bDrawChnBar = HI_TRUE;

static avdec_context_t *m_avdec_ctx = NULL;

static int m_avdec_pass_through = 0;

pthread_mutex_t mAvdecMutex;

/**
 * Common hardware methods
 */

int open_avdec(const struct hw_module_t* module, const char* name, struct hw_device_t** device);

static struct hw_module_methods_t avdec_module_methods = {
	open: open_avdec
};

int avdec_get_list(struct avdec_module_t *module, struct avdec_t const **list);

struct avdec_module_t HAL_MODULE_INFO_SYM = {
	common: {
		tag: HARDWARE_MODULE_TAG,
		version_major: 1,
		version_minor: 0,
		id: AVDEC_HARDWARE_MODULE_ID,
		name: "avdec Module",
		author: "iPanel 2011, Inc.",
		methods: &avdec_module_methods,
		dso: NULL,
		reserved: {0}
	},
	get_avdec_list:avdec_get_list,
};

#define GET_CODE(av) ((av)?((av->id%2)?"video":"audio"):"unknow")
#define GET_ID(av) ((av)?av->id:-1)

HI_S32 avdec_callback(HI_HANDLE hAvplay, HI_UNF_AVPLAY_EVENT_E enEvent, HI_U32 u32Para)
{
    avplayer_t *av,*av1;
	HI_S32 i;
	HI_S32 ret = HI_FAILURE;
    avplayer_t *av0 = &m_avdec_ctx->avdec[1];
	snd_win_t *sw,*sw1;
    pthread_mutex_lock(&mAvdecMutex);

	switch(enEvent)
	{
	case HI_UNF_AVPLAY_EVENT_NEW_VID_FRAME:
		for(i=1;i<AVDEC_MAX_NUM;i+=2)
		{
		    av = &m_avdec_ctx->avdec[i];
			sw = (snd_win_t*)av->out.win.handle;
		    if ( 1==av->stat && hAvplay==av->handle && sw->dispwind==av->out.win.id ){
                LOGD("callback_avdec handle=%x, %s, %d", av, GET_CODE (av), av->id);
                /*duanzh20160411 cable没停的去播ip，导致ip出不来，注释掉。说明 播放流程改了这里会有问题
                if ( 2!=av0->stat ) //if play 0 video not come,set window top,and others disable
                */
                {
                    LOGD("callback_avdec window move to top, %d", av->id);
                    ret = HI_UNF_VO_SetWindowZorder(sw->win[av->out.win.id].handle, HI_LAYER_ZORDER_MOVETOP);
                    if (HI_SUCCESS != ret) {
                        LOGE("HI_UNF_VO_SetWindowZorder, Ret=%x",ret);
                        goto out;
                    }
                }
                av->stat = 2;
                break;
            }
		}

		break;
    case HI_UNF_AVPLAY_EVENT_NEW_AUD_FRAME:
        av0 = av0->community;
		for(i=0;i<AVDEC_MAX_NUM;i+=2)
		{
		    av = &m_avdec_ctx->avdec[i];
			sw = (snd_win_t*)av->out.snd.handle;
			av1 = &m_avdec_ctx->avdec[i+1];
			sw1 = (snd_win_t*)av1->out.win.handle;
		    if ( 1==av->stat && hAvplay==av->handle && sw1->dispwind==av1->out.win.id ){
                HI_UNF_SND_GAIN_ATTR_S stMixWeightGain;
                stMixWeightGain.bLinearMode = HI_TRUE;
                LOGD("callback_avdec handle=%x, %s, %d", av, GET_CODE (av), av->id);
				/*duanzh20160411 cable没停的去播ip，导致ip出不来，注释掉。说明 播放流程改了这里会有问题
                if ( 2!=av0->stat ) //if player 0 audio not come,set weight 100,and others 0
                */
                {
                    stMixWeightGain.s32Gain = 100;
                    LOGD("callback_avdec track weight 100, %d", av->id);
                    ret = HI_UNF_SND_SetTrackWeight(sw->snd[av->out.snd.id-1-sw->id].handle, &stMixWeightGain);
                    if (HI_SUCCESS != ret) {
                        LOGE("HI_UNF_SND_SetTrackWeight, Ret=%x",ret);
                        goto out;
                    }
                    for(i=0;i<AVDEC_MAX_NUM;i+=2){
                        av0 = &m_avdec_ctx->avdec[i];
                        if ( av->id != av0->id && 0!=av0->out.snd.handle){
                            snd_win_t *sw = (snd_win_t*)av0->out.snd.handle;
                            if ( 0==sw->snd[av0->out.snd.id-1-sw->id].handle ) continue;
                            stMixWeightGain.s32Gain = 0;
                            LOGD("callback_avdec track weight   0, %d", av0->id);
                            ret = HI_UNF_SND_SetTrackWeight(sw->snd[av0->out.snd.id-1-sw->id].handle, &stMixWeightGain);
                            if (HI_SUCCESS != ret) {
                                LOGE("HI_UNF_SND_SetTrackWeight, Ret=%x",ret);
                                goto out;
                            }
                        }
                    }
                }
                av->stat = 2;
                break;
            }
		}
        break;
    case HI_UNF_AVPLAY_EVENT_RNG_BUF_STATE:
        LOGD("EVENT_RNG_BUF_STATE hAvplay:%#x, u32Para:%d", hAvplay, u32Para);
        break;
	default:
		break;
	}
out:
    pthread_mutex_unlock(&mAvdecMutex);
	return 0;
}

static int avdec_bind_demux(struct avdec_device_t *dev, int handle, struct demux_device_t *d,
        int dmxHandle, int chId)
{
    avplayer_t *av = (avplayer_t *)handle;
    LOGD("bind_demux handle=%d,%x, %s, d=%x, dmxHandle=%x, chn=%d", GET_ID (av), handle, GET_CODE(av), d, dmxHandle, chId);

    av->dmx.dev = d;
    av->dmx.handle = dmxHandle;
    av->dmx.chnid = chId;
    av->source = AV_WORK_MODE_BIND;

    return 0;
}

static int avdec_bind_data(struct avdec_device_t *dev, int handle, int flag)
{
    avplayer_t *av = (avplayer_t *)handle;
    LOGD("bind_data handle=%d,%x, %s, flag=%d", GET_ID (av), handle, GET_CODE(av), flag);

    av->dmx.handle = 0;
    av->dmx.chnid = 0;
    av->source = AV_WORK_MODE_PUSH;

    return 0;
}

static int avdec_unbind(struct avdec_device_t *dev, int handle)
{
    avplayer_t *av = (avplayer_t *)handle;

	LOGD("unbind handle=%d,%x, %s", GET_ID (av), handle, GET_CODE(av));

    av->dmx.handle = 0;
    av->dmx.chnid = 0;
    av->source = AV_WORK_MODE_UNDEF;

	return 0;
}

static struct avdec_format_t g_fmt_vlist[] = {
    { VCODEC_TYPE_MPEG1, AV_DATA_TYPE_PES },//
    { VCODEC_TYPE_MPEG2, AV_DATA_TYPE_PES },//
    { VCODEC_TYPE_H264, AV_DATA_TYPE_PES },//
    { VCODEC_TYPE_AVS, AV_DATA_TYPE_PES },//
    { VCODEC_TYPE_MPEG4, AV_DATA_TYPE_PES },//
    { VCODEC_TYPE_VC1, AV_DATA_TYPE_PES },//
    { VCODEC_TYPE_VC1_SIMPLEMAIN, AV_DATA_TYPE_PES },//
    { VCODEC_TYPE_DIVX3, AV_DATA_TYPE_PES },//
    { VCODEC_TYPE_H263, AV_DATA_TYPE_PES },//
    { VCODEC_TYPE_H263_SORENSON, AV_DATA_TYPE_PES },//
    { VCODEC_TYPE_VP60, AV_DATA_TYPE_PES },//
    { VCODEC_TYPE_VP61, AV_DATA_TYPE_PES },//
    { VCODEC_TYPE_VP62, AV_DATA_TYPE_PES },//
    { VCODEC_TYPE_REAL8, AV_DATA_TYPE_PES },//
    { VCODEC_TYPE_REAL9, AV_DATA_TYPE_PES },//
    { VCODEC_TYPE_MJPG, AV_DATA_TYPE_PES }, //
    { VCODEC_TYPE_H265, AV_DATA_TYPE_PES},//
};

static struct avdec_format_t g_fmt_alist[] = {
    { ACODEC_TYPE_MPEG1, AV_DATA_TYPE_ES },//
    { ACODEC_TYPE_MPEG2, AV_DATA_TYPE_ES },//
    { ACODEC_TYPE_MP3, AV_DATA_TYPE_ES },//
    { ACODEC_TYPE_AAC, AV_DATA_TYPE_ES },//
    { ACODEC_TYPE_AAC_PLUS, AV_DATA_TYPE_ES },//
    { ACODEC_TYPE_AC3, AV_DATA_TYPE_ES },//
    { ACODEC_TYPE_AC3_PLUS, AV_DATA_TYPE_ES },//
    { ACODEC_TYPE_WMAPRO, AV_DATA_TYPE_ES },//
    { ACODEC_TYPE_WMASTD, AV_DATA_TYPE_ES },//
    { ACODEC_TYPE_DTS, AV_DATA_TYPE_ES },//
    { ACODEC_TYPE_DTSHD, AV_DATA_TYPE_ES },//
    { ACODEC_TYPE_PCM, AV_DATA_TYPE_ES },//
    { ACODEC_TYPE_COOK, AV_DATA_TYPE_ES }, //
};

static int avdec_get_format_list(struct avdec_device_t *dev, int handle,
		const struct avdec_format_t **list)
{
    avplayer_t *av = (avplayer_t *)handle;
	LOGD("get_format_list handle=%d,%x, %s", GET_ID (av), handle, GET_CODE(av));

    if ( av->id%2 ){
        (*list) = g_fmt_vlist;
        return sizeof(g_fmt_vlist) / sizeof(avdec_format_t);
    }
    else {
        (*list) = g_fmt_alist;
        return sizeof(g_fmt_alist) / sizeof(avdec_format_t);
    }
    return 0;
}

static HI_VOID DDPlusCallBack(DOLBYPLUS_EVENT_E Event, HI_VOID *pUserData) {
	DOLBYPLUS_STREAM_INFO_S *pstInfo = (DOLBYPLUS_STREAM_INFO_S *) pUserData;

	LOGD("DDPlusCallBack");

	//g_u32DolbyAcmod = pstInfo->s16Acmod;

	if (HA_DOLBYPLUS_EVENT_SOURCE_CHANGE == Event) {
		g_bDrawChnBar = HI_TRUE;
		//printf("DDPlusCallBack enent !\n");
	}
	return;
}

static int avdec_set_format(struct avdec_device_t *dev, int handle, int format, int flags)
{
    avplayer_t *av = (avplayer_t *)handle;
	LOGD("set_format handle=%d,%x, %s, format=%x, flags=%d", GET_ID (av), handle, GET_CODE(av), format, flags);

    av->format = format;

    return 0;
}

static int avdec_set_sync(struct avdec_device_t *dev, int handle, int sync_handle, int pcrch_index)
{
    avplayer_t *av = (avplayer_t *)handle;
	LOGD("set_sync handle=%d,%x, sync_handle=%x, pcrch_index=%d", GET_ID (av), handle, sync_handle, pcrch_index);

    if ( -1 == sync_handle ){
        av->pcrchn = av->community->pcrchn = -1;
    }
    else{
        if ( sync_handle != (int)av->community )
            LOGW("handle:%x pair %x ", handle, av->community);
        av->pcrchn = av->community->pcrchn = pcrch_index;
    }
    return 0;
}

static int avdec_start(struct avdec_device_t *dev, int handle)
{
    int ret=0;
    avplayer_t *av = (avplayer_t *)handle;
    HI_UNF_AVPLAY_ATTR_S stAvAttr;
    HI_UNF_SYNC_ATTR_S stSynAttr;

	LOGD("start handle=%d,%x, %s", GET_ID (av), handle, GET_CODE(av));

    //check and create avplay
    if ( 0==av->handle ){
        HI_UNF_AVPLAY_GetDefaultConfig(&stAvAttr, HI_UNF_AVPLAY_STREAM_TYPE_TS);
        LOGD("stAvAttr.u32VidBufSize = %x",stAvAttr.stStreamAttr.u32VidBufSize);
        ret = HI_UNF_AVPLAY_Create(&stAvAttr, &av->handle);
        if ( ret ){
            LOGE("HI_UNF_AVPLAY_Create, Ret=%x", ret);
            return -1;
        }
        ret = HI_UNF_AVPLAY_ChnOpen(av->handle, HI_UNF_AVPLAY_MEDIA_CHAN_VID, HI_NULL);
        ret |= HI_UNF_AVPLAY_ChnOpen(av->handle, HI_UNF_AVPLAY_MEDIA_CHAN_AUD, HI_NULL);
        if ( ret ){
            LOGE("HI_UNF_AVPLAY_ChnOpen, Ret=%x", ret);
            HI_UNF_AVPLAY_Destroy (av->handle);
            av->handle = 0;
            return -1;
        }
        ret = HI_UNF_AVPLAY_RegisterEvent(av->handle, HI_UNF_AVPLAY_EVENT_NEW_VID_FRAME, (HI_UNF_AVPLAY_EVENT_CB_FN)avdec_callback);
        ret |= HI_UNF_AVPLAY_RegisterEvent(av->handle, HI_UNF_AVPLAY_EVENT_NEW_AUD_FRAME, (HI_UNF_AVPLAY_EVENT_CB_FN)avdec_callback);
        if ( ret ){
            LOGE("HI_UNF_AVPLAY_RegisterEvent, Ret=%x", ret);
        }
        LOGD("create avplayer, handle=%x", av->handle);
        //set community handle
        av->community->handle = av->handle;
    }

    //check and set stream mode
    HI_UNF_AVPLAY_GetAttr(av->handle, HI_UNF_AVPLAY_ATTR_ID_STREAM_MODE, &stAvAttr);
    if ( av->source == AV_WORK_MODE_BIND ){
        mdemux_t *dm = (mdemux_t*)av->dmx.handle;
        if ( HI_UNF_AVPLAY_STREAM_TYPE_ES==stAvAttr.stStreamAttr.enStreamType ||
            stAvAttr.u32DemuxId != dm->dmxid ){
            HI_UNF_AVPLAY_GetDefaultConfig(&stAvAttr, HI_UNF_AVPLAY_STREAM_TYPE_TS);
            stAvAttr.u32DemuxId = dm->dmxid;
            ret = HI_UNF_AVPLAY_SetAttr(av->handle, HI_UNF_AVPLAY_ATTR_ID_STREAM_MODE, &stAvAttr);
            if ( ret ){
                LOGE("HI_UNF_AVPLAY_SetAttr, Ret=%x", ret);
                return -1;
            }
        }
    }else if ( av->source == AV_WORK_MODE_PUSH && HI_UNF_AVPLAY_STREAM_TYPE_TS==stAvAttr.stStreamAttr.enStreamType){
        HI_UNF_AVPLAY_GetDefaultConfig(&stAvAttr, HI_UNF_AVPLAY_STREAM_TYPE_ES);
        ret = HI_UNF_AVPLAY_SetAttr(av->handle, HI_UNF_AVPLAY_ATTR_ID_STREAM_MODE, &stAvAttr);
        if ( ret ){
            LOGE("HI_UNF_AVPLAY_SetAttr, Ret=%x", ret);
            return -1;
        }
    }

    //check and attach snd or win
    if ( av->id%2 ){
        if ( 0==av->out.win.handle ){
            LOGE("vdec not bind window");
            return -1;
        }
        snd_win_t *sw = (snd_win_t*)av->out.win.handle;
        if ( 0==sw->win[av->out.win.id].handle ){
            HI_UNF_WINDOW_ATTR_S   WinAttr;
            memset(&WinAttr, 0, sizeof(HI_UNF_WINDOW_ATTR_S));
            WinAttr.enDisp = HI_UNF_DISPLAY1;
            WinAttr.bVirtual = HI_FALSE;
            WinAttr.stWinAspectAttr.enAspectCvrs = HI_UNF_VO_ASPECT_CVRS_IGNORE;
            WinAttr.stWinAspectAttr.bUserDefAspectRatio = HI_FALSE;
            WinAttr.stWinAspectAttr.u32UserAspectWidth  = 0;
            WinAttr.stWinAspectAttr.u32UserAspectHeight = 0;
            WinAttr.bUseCropRect = HI_FALSE;
            WinAttr.stInputRect.s32X = 0;
            WinAttr.stInputRect.s32Y = 0;
            WinAttr.stInputRect.s32Width = 0;
            WinAttr.stInputRect.s32Height = 0;
            WinAttr.stOutputRect.s32X = sw->win[av->out.win.id].r.x;
            WinAttr.stOutputRect.s32Y = sw->win[av->out.win.id].r.y;
            WinAttr.stOutputRect.s32Width = sw->win[av->out.win.id].r.w;
            WinAttr.stOutputRect.s32Height= sw->win[av->out.win.id].r.h;
            ret = HI_UNF_VO_CreateWindow(&WinAttr, &sw->win[av->out.win.id].handle);
            if (ret != HI_SUCCESS)
            {
                LOGE("HI_UNF_VO_CreateWindow, Ret=%x", ret);
                return -1;
            }
            LOGD("create window, handle=%x", sw->win[av->out.win.id].handle);
        }
        if ( 0==av->out.win.attach ){
            ret = HI_UNF_VO_AttachWindow (sw->win[av->out.win.id].handle, av->handle);
            if (ret != HI_SUCCESS)
            {
                LOGE("HI_UNF_VO_AttachWindow, Ret=%x", ret);
                return -1;
            }
            av->out.win.attach = 1;
            LOGD("attach window");
        }
        ret = HI_UNF_VO_SetWindowEnable (sw->win[av->out.win.id].handle, HI_TRUE);
        if (ret != HI_SUCCESS)
        {
            LOGE("HI_UNF_VO_SetWindowEnable, Ret=%x", ret);
            return -1;
        }
    }
    else {
        if ( 0==av->out.snd.handle ){
            LOGE("adec not bind sounder");
            return -1;
        }
        snd_win_t *sw = (snd_win_t*)av->out.snd.handle;
        if ( 0==sw->snd[av->out.snd.id-1-sw->id].handle ){
            HI_UNF_AUDIOTRACK_ATTR_S stTrackAttr;
            HI_UNF_SND_GAIN_ATTR_S stMixWeightGain;
            ret = HI_UNF_SND_GetDefaultTrackAttr((av->out.snd.id==1)?HI_UNF_SND_TRACK_TYPE_SLAVE:HI_UNF_SND_TRACK_TYPE_MASTER, &stTrackAttr);
            if (ret != HI_SUCCESS)
            {
                LOGE("HI_UNF_SND_GetDefaultTrackAttr,Ret=%x",ret);
                return -1;
            }
            ret = HI_UNF_SND_CreateTrack(HI_UNF_SND_0,&stTrackAttr, &sw->snd[av->out.snd.id-1-sw->id].handle);
            if (ret != HI_SUCCESS)
            {
                LOGE("HI_UNF_SND_CreateTrack,Ret=%x", ret);
                return -1;
            }
            stMixWeightGain.bLinearMode = HI_TRUE;
            stMixWeightGain.s32Gain = sw->snd[av->out.snd.id-1-sw->id].volume;
            ret = HI_UNF_SND_SetTrackWeight(sw->snd[av->out.snd.id-1-sw->id].handle, &stMixWeightGain);
            if (ret != HI_SUCCESS)
            {
                LOGE("HI_UNF_SND_SetTrackWeight,Ret=%x", ret);
                return -1;
            }
            LOGD("create track, handle=%x", sw->snd[av->out.snd.id-1-sw->id].handle);
        }
        if ( 0==av->out.snd.attach ){
            ret = HI_UNF_SND_Attach(sw->snd[av->out.snd.id-1-sw->id].handle, av->handle);
            if (ret != HI_SUCCESS)
            {
                LOGE("HI_UNF_SND_Attach,Ret=%x", ret);
                return -1;
            }
            av->out.snd.attach = 1;
            LOGD("attach track");
        }
    }
	
	LOGD("set sync : av->pcrchn:%x  stSynAttr.enSyncRef:%x\n");

    //set sync
    ret = HI_UNF_AVPLAY_GetAttr(av->handle, HI_UNF_AVPLAY_ATTR_ID_SYNC, &stSynAttr);
    if ( -1 == av->pcrchn && HI_UNF_SYNC_REF_NONE!=stSynAttr.enSyncRef ){
        stSynAttr.enSyncRef = HI_UNF_SYNC_REF_NONE;
        ret = HI_UNF_AVPLAY_SetAttr(av->handle, HI_UNF_AVPLAY_ATTR_ID_SYNC, &stSynAttr);
		LOGD("set sync :HI_UNF_SYNC_REF_NONE");
    }else if ( -1 != av->pcrchn ){//&& HI_UNF_SYNC_REF_NONE==stSynAttr.enSyncRef){
        stSynAttr.enSyncRef =HI_UNF_SYNC_REF_AUDIO;
        stSynAttr.stSyncStartRegion.s32VidPlusTime = 100;
        stSynAttr.stSyncStartRegion.s32VidNegativeTime = -100;
        stSynAttr.u32PreSyncTimeoutMs = 200;
		stSynAttr.bQuickOutput = HI_TRUE;
        ret = HI_UNF_AVPLAY_SetAttr(av->handle, HI_UNF_AVPLAY_ATTR_ID_SYNC, &stSynAttr);
		LOGD("set sync :HI_UNF_SYNC_REF_AUDIO");
    }
    if (ret != HI_SUCCESS)
    {
        LOGE("HI_UNF_AVPLAY_SetAttr,Ret=%x", ret);
        return -1;
    }

    //set pid
    if ( AV_WORK_MODE_BIND==av->source ){
        mdemux_t *dm = (mdemux_t *)av->dmx.handle;
        channel_t *ch = &dm->channel[av->dmx.chnid-1];
        LOGD("start, pid=%x", ch->pid);
        if ( av->id%2 ){
			ret = HI_UNF_AVPLAY_SetAttr(av->handle, HI_UNF_AVPLAY_ATTR_ID_VID_PID, &ch->pid);
			if (HI_SUCCESS != ret) {
				LOGE("HI_UNF_AVPLAY_SetAttr VID, Ret=%x",ret);
				return -1;
			}
        }
        else {
			ret = HI_UNF_AVPLAY_SetAttr(av->handle, HI_UNF_AVPLAY_ATTR_ID_AUD_PID, &ch->pid);
			if (HI_SUCCESS != ret) {
				LOGE("HI_UNF_AVPLAY_SetAttr AUD, Ret=%x",ret);
				return -1;
			}
            if ( av->pcrchn>0 ){
                ch = (channel_t*)&dm->channel[av->pcrchn-1];;
                ret = HI_UNF_AVPLAY_SetAttr(av->handle, HI_UNF_AVPLAY_ATTR_ID_PCR_PID, &ch->pid);
                if (HI_SUCCESS != ret) {
                    LOGE("HI_UNF_AVPLAY_SetAttr PCR, Ret=%x",ret);
                    //return -1;
                }
                LOGD("start, pid=%x, pcr", ch->pid);
            }
        }
    }

    //set format and start
    if ( av->id%2 ){
        HI_UNF_VCODEC_TYPE_E enVdecType = HI_UNF_VCODEC_TYPE_BUTT;
        HI_UNF_VCODEC_ATTR_S VdecAttr;
        switch ( av->format ) {
		case VCODEC_TYPE_MPEG1:
		case VCODEC_TYPE_MPEG2:
			enVdecType = HI_UNF_VCODEC_TYPE_MPEG2;
			break;
		case VCODEC_TYPE_H264:
			enVdecType = HI_UNF_VCODEC_TYPE_H264;
			break;
		case VCODEC_TYPE_H263:
		case VCODEC_TYPE_H263_SORENSON:
			enVdecType = HI_UNF_VCODEC_TYPE_H263;
			break;
		case VCODEC_TYPE_VC1:
		case VCODEC_TYPE_VC1_SIMPLEMAIN:
			enVdecType = HI_UNF_VCODEC_TYPE_VC1;
			break;
		case VCODEC_TYPE_VP60:
			enVdecType = HI_UNF_VCODEC_TYPE_VP6;
			break;
		case VCODEC_TYPE_VP61:
			enVdecType = HI_UNF_VCODEC_TYPE_VP6A;
			break;
		case VCODEC_TYPE_VP62:
			enVdecType = HI_UNF_VCODEC_TYPE_VP6F;
			break;
		case VCODEC_TYPE_MPEG4:
			enVdecType = HI_UNF_VCODEC_TYPE_MPEG4;
			break;
		case VCODEC_TYPE_REAL8:
			enVdecType = HI_UNF_VCODEC_TYPE_REAL8;
			break;
		case VCODEC_TYPE_REAL9:
			enVdecType = HI_UNF_VCODEC_TYPE_REAL9;
			break;
		case VCODEC_TYPE_DIVX3:
			enVdecType = HI_UNF_VCODEC_TYPE_DIVX3;
			break;
		case ACODEC_TYPE_AVS:
			enVdecType = HI_UNF_VCODEC_TYPE_AVS;
			break;
		case VCODEC_TYPE_MJPG:
			enVdecType = HI_UNF_VCODEC_TYPE_JPEG;
			break;
		case VCODEC_TYPE_H265:
            enVdecType = HI_UNF_VCODEC_TYPE_HEVC;
            break;
		default:
			enVdecType = HI_UNF_VCODEC_TYPE_MPEG2;
			LOGE("unsupport format,%d", av->format);
			return -1;
		}

		HI_UNF_AVPLAY_GetAttr(av->handle, HI_UNF_AVPLAY_ATTR_ID_VDEC, &VdecAttr);
		VdecAttr.enType = enVdecType;
		VdecAttr.enMode = HI_UNF_VCODEC_MODE_NORMAL;
		VdecAttr.u32ErrCover = 20;//100;
		VdecAttr.u32Priority = 3;
		ret = HI_UNF_AVPLAY_SetAttr(av->handle, HI_UNF_AVPLAY_ATTR_ID_VDEC, &VdecAttr);
		if (HI_SUCCESS != ret) {
			LOGE("HI_UNF_AVPLAY_SetAttr vdec, Ret=%x",ret);
			return -1;
		}
        ret = HI_UNF_AVPLAY_Start (av->handle, HI_UNF_AVPLAY_MEDIA_CHAN_VID, NULL);
		if (HI_SUCCESS != ret) {
			LOGE("HI_UNF_AVPLAY_Start vdec, Ret=%x",ret);
			return -1;
		}
        LOGD("start, format=%x", av->format);
    }
    else {
        HI_U8 u8DecOpenBuf[1024];
        HI_U8 u8EncOpenBuf[1024];
        WAV_FORMAT_S stWavFormat;
        HI_HA_DECODEMODE_E enAudioDecMode;
        HA_CODEC_ID_E enAdecType = HA_AUDIO_ID_MP2;
        HI_UNF_ACODEC_ATTR_S AdecAttr;
        switch ( av->format ) {
		case ACODEC_TYPE_MPEG1:
		case ACODEC_TYPE_MPEG2:
			enAdecType = HA_AUDIO_ID_MP2;
			break;
		case ACODEC_TYPE_MP3:
			enAdecType = HA_AUDIO_ID_MP3;
			break;
		case ACODEC_TYPE_AAC:
		case ACODEC_TYPE_AAC_PLUS:
			enAdecType = HA_AUDIO_ID_AAC;
			break;
		case ACODEC_TYPE_AC3:
		case ACODEC_TYPE_AC3_PLUS:
			enAdecType = HA_AUDIO_ID_DOLBY_PLUS;
			break;
		case ACODEC_TYPE_PCM:
			enAdecType = HA_AUDIO_ID_PCM;
			break;
		case ACODEC_TYPE_COOK:
			enAdecType = HA_AUDIO_ID_COOK;
			break;
		case ACODEC_TYPE_DTS:
			enAdecType = HA_AUDIO_ID_DTSPASSTHROUGH;
			break;
		case ACODEC_TYPE_DTSHD:
			enAdecType = HA_AUDIO_ID_DTSHD;
			break;
		case ACODEC_TYPE_WMAPRO:
		case ACODEC_TYPE_WMASTD:
			enAdecType = HA_AUDIO_ID_WMA9STD;
			break;
		default:
			enAdecType = HA_AUDIO_ID_MP2;
			LOGE("unsupport format,%d", av->format );
			return -1;
		}
		HI_UNF_AVPLAY_GetAttr(av->handle, HI_UNF_AVPLAY_ATTR_ID_ADEC, &AdecAttr);
		AdecAttr.enType = enAdecType;
		if (HA_AUDIO_ID_PCM == AdecAttr.enType) {
			/* set pcm wav format here base on pcm file */
			stWavFormat.nChannels = 1;
			stWavFormat.nSamplesPerSec = 48000;
			stWavFormat.wBitsPerSample = 16;
			HA_PCM_DecGetDefalutOpenParam(&(AdecAttr.stDecodeParam), &stWavFormat);
			LOGI("please make sure the attributes of PCM stream is tme same as defined in function of \"HIADP_AVPlay_SetAdecAttr\"? \n");
			LOGI("(nChannels = 1, wBitsPerSample = 16, nSamplesPerSec = 48000, isBigEndian = HI_FALSE) \n");
		} else if (HA_AUDIO_ID_MP2 == AdecAttr.enType) {
			HA_MP2_DecGetDefalutOpenParam(&(AdecAttr.stDecodeParam));
		} else if (HA_AUDIO_ID_AAC == AdecAttr.enType) {
			HA_AAC_DecGetDefalutOpenParam(&(AdecAttr.stDecodeParam));
		} else if (HA_AUDIO_ID_MP3 == AdecAttr.enType) {
			HA_MP3_DecGetDefalutOpenParam(&(AdecAttr.stDecodeParam));
		} else if (HA_AUDIO_ID_AMRNB == AdecAttr.enType) {
			AMRNB_DECODE_OPENCONFIG_S *pstConfig = (AMRNB_DECODE_OPENCONFIG_S *) u8DecOpenBuf;
			HA_AMRNB_GetDecDefalutOpenParam(&(AdecAttr.stDecodeParam), pstConfig);
			pstConfig->enFormat = AMRNB_MIME;
		} else if (HA_AUDIO_ID_DOLBY_PLUS == AdecAttr.enType) {
			DOLBYPLUS_DECODE_OPENCONFIG_S *pstConfig =
					(DOLBYPLUS_DECODE_OPENCONFIG_S *) u8DecOpenBuf;
			HA_DOLBYPLUS_DecGetDefalutOpenConfig(pstConfig);
			pstConfig->pfnEvtCbFunc[HA_DOLBYPLUS_EVENT_SOURCE_CHANGE] = DDPlusCallBack;
			pstConfig->pAppData[HA_DOLBYPLUS_EVENT_SOURCE_CHANGE] = &g_stDDpStreamInfo;
			/* Dolby DVB Broadcast default settings */
			pstConfig->enDrcMode = DOLBYPLUS_DRC_RF;
			pstConfig->enDmxMode = DOLBYPLUS_DMX_SRND;
			HA_DOLBYPLUS_DecGetDefalutOpenParam(&(AdecAttr.stDecodeParam), pstConfig);
			AdecAttr.stDecodeParam.enDecMode = HD_DEC_MODE_RAWPCM;

			#if 0
				AdecAttr.stDecodeParam.enDecMode = HD_DEC_MODE_RAWPCM;
				HI_UNF_SND_SetHdmiMode(HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_HDMI0,
								HI_UNF_SND_HDMI_MODE_LPCM);//yanglb
				HI_UNF_SND_SetSpdifMode(HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_SPDIF0,
								HI_UNF_SND_SPDIF_MODE_RAW);


			#endif
			
		} else if (HA_AUDIO_ID_AC3PASSTHROUGH == AdecAttr.enType) {
			HA_AC3PASSTHROUGH_DecGetDefalutOpenParam(&(AdecAttr.stDecodeParam));
			//AdecAttr.stDecodeParam.enDecMode = enAudioDecMode;
			AdecAttr.stDecodeParam.enDecMode = HD_DEC_MODE_THRU;
		} else if (HA_AUDIO_ID_TRUEHD == AdecAttr.enType) {
			HA_TRUEHD_DecGetDefalutOpenParam(&(AdecAttr.stDecodeParam));
			if (HD_DEC_MODE_THRU != enAudioDecMode) {
				LOGE(" MLP decoder enAudioDecMode(%d) error (mlp only support hbr Pass-through only).\n", enAudioDecMode);
				return -1;
			}
			AdecAttr.stDecodeParam.enDecMode = HD_DEC_MODE_THRU; /* truehd just support pass-through */
			LOGI(" TrueHD decoder(HBR Pass-through only).\n");
		} else if (HA_AUDIO_ID_DTSPASSTHROUGH == AdecAttr.enType) {
			HA_DTSPASSTHROUGH_DecGetDefalutOpenParam(&(AdecAttr.stDecodeParam));
			AdecAttr.stDecodeParam.enDecMode = HD_DEC_MODE_THRU; /* dsthd just support pass-through */
			LOGI(" DTS HD decoder(HBR Pass-through only).\n");
		} else if (HA_AUDIO_ID_DTSHD == AdecAttr.enType) {
			DTSHD_DECODE_OPENCONFIG_S *pstConfig = (DTSHD_DECODE_OPENCONFIG_S *) u8DecOpenBuf;
			HA_DTSHD_DecGetDefalutOpenConfig(pstConfig);
			HA_DTSHD_DecGetDefalutOpenParam(&(AdecAttr.stDecodeParam), pstConfig);
			AdecAttr.stDecodeParam.enDecMode = HD_DEC_MODE_SIMUL;

		} 
#if 0   // 和上面的分支重复了，注释掉
		else if (HA_AUDIO_ID_DOLBY_PLUS == AdecAttr.enType) {
			DOLBYPLUS_DECODE_OPENCONFIG_S *pstConfig =
					(DOLBYPLUS_DECODE_OPENCONFIG_S *) u8DecOpenBuf;
			HA_DOLBYPLUS_DecGetDefalutOpenConfig(pstConfig);
			/* Dolby DVB Broadcast default settings */
			pstConfig->enDrcMode = DOLBYPLUS_DRC_RF;
			pstConfig->enDmxMode = DOLBYPLUS_DMX_SRND;
			HA_DOLBYPLUS_DecGetDefalutOpenParam(&(AdecAttr.stDecodeParam), pstConfig);
			AdecAttr.stDecodeParam.enDecMode = HD_DEC_MODE_SIMUL;
		} 
#endif
		else if (HA_AUDIO_ID_DRA == AdecAttr.enType) {
			HA_DRA_DecGetDefalutOpenParam(&(AdecAttr.stDecodeParam));
		}
#if 0	//use sounder ioctl	
		LOGI("m_avdec_pass_through = %d", m_avdec_pass_through);
		if (m_avdec_pass_through != 0) {
			AdecAttr.stDecodeParam.enDecMode = HD_DEC_MODE_THRU;
			HI_UNF_SND_SetHdmiMode(HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_HDMI0,
					HI_UNF_SND_HDMI_MODE_RAW);//yanglb
			HI_UNF_SND_SetSpdifMode(HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_SPDIF0,
					HI_UNF_SND_SPDIF_MODE_RAW);
		} else {
			HI_UNF_SND_SetHdmiMode(HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_HDMI0,
					HI_UNF_SND_HDMI_MODE_LPCM);//yanglb
			HI_UNF_SND_SetSpdifMode(HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_SPDIF0,
					HI_UNF_SND_SPDIF_MODE_LPCM);
		}
#endif
		ret = HI_UNF_AVPLAY_SetAttr(av->handle, HI_UNF_AVPLAY_ATTR_ID_ADEC, &AdecAttr);
		if (HI_SUCCESS != ret) {
			LOGE("HI_UNF_AVPLAY_SetAttr adec,Ret=%x",ret);
			return -1;
		}
        ret = HI_UNF_AVPLAY_Start (av->handle, HI_UNF_AVPLAY_MEDIA_CHAN_AUD, NULL);
		if (HI_SUCCESS != ret) {
			LOGE("HI_UNF_AVPLAY_Start adec,Ret=%x",ret);
			return -1;
		}
        LOGD("start, format=%x", av->format);
    }

    av->stat = 1;

    return 0;
}

static int avdec_resume(struct avdec_device_t *dev, int handle)
{
    int ret;
    avplayer_t *av = (avplayer_t *)handle;

    LOGD("resume handle=%d,%x, %s", GET_ID (av), handle, GET_CODE(av));

    if (av->id % 2) {
        ret = HI_UNF_AVPLAY_Resume(av->handle, NULL);
        if (HI_SUCCESS != ret) {
            LOGE("HI_UNF_AVPLAY_Resume, Ret=%x",ret);
            return -1;
        }
    }
    return 0;
}

static int avdec_pause(struct avdec_device_t *dev, int handle)
{
    int ret;
    avplayer_t *av = (avplayer_t *)handle;

    LOGD("pause handle=%d,%x, %s", GET_ID (av), handle, GET_CODE(av));
    if (av->id % 2) {
        ret = HI_UNF_AVPLAY_Pause(av->handle, NULL);
        if (HI_SUCCESS != ret) {
            LOGE("HI_UNF_AVPLAY_Pause, Ret=%x",ret);
            return -1;
        }
    }

    return 0;
}

static int avdec_stop(struct avdec_device_t *dev, int handle, int mode)
{
    int ret;
    avplayer_t *av = (avplayer_t *)handle;

    LOGD("stop handle=%d,%x, %s, mode=%d", GET_ID (av), handle, GET_CODE(av), mode);

    if ( 0==av->stat ){
        LOGD("decoder not start yet");
        return 0;
    }

    if ( av->id%2 ) {
        snd_win_t *sw = (snd_win_t*)av->out.win.handle;
        HI_UNF_AVPLAY_STOP_OPT_S stStopOpt;
        stStopOpt.u32TimeoutMs = 0;
        //if ( AVOUT_FLAG_LAST_FRAME == sw->win[av->out.win.id].freeze )
        if ( AVDEC_STOP_MODE_LAST_FRAME == mode )
            stStopOpt.enMode = HI_UNF_AVPLAY_STOP_MODE_STILL;
        else
            stStopOpt.enMode = HI_UNF_AVPLAY_STOP_MODE_BLACK;
        ret = HI_UNF_AVPLAY_Stop(av->handle, HI_UNF_AVPLAY_MEDIA_CHAN_VID, &stStopOpt);
        if (HI_SUCCESS != ret) {
            LOGE("HI_UNF_AVPLAY_Stop, Ret=%x",ret);
            return -1;
        }
    }
    else {
        ret = HI_UNF_AVPLAY_Stop(av->handle, HI_UNF_AVPLAY_MEDIA_CHAN_AUD, HI_NULL);
        if (HI_SUCCESS != ret) {
            LOGE("HI_UNF_AVPLAY_Stop, Ret=%x",ret);
            return -1;
        }
    }
    av->stat = 0;

    return 0;
}

static int avdec_clear(struct avdec_device_t *dev, int handle)
{
    int ret;
    avplayer_t *av = (avplayer_t *)handle;

    LOGD("clear handle=%d,%x, %s", GET_ID (av), handle, GET_CODE(av));

    if ( 0==av->stat ){
        LOGD("decoder not start yet");
        return 0;
    }
	else if(2==av->stat)
	{
        LOGD("avdec_clear restart");
        av->stat = 1;	
	}

    if (av->id % 2) {
		/* demux 那边会掉clear data
        if ( AV_WORK_MODE_BIND == av->source ){
            mdemux_t *dm = (mdemux_t *)av->dmx.handle;
            if ( dm->ts_buf ){
                ret = HI_UNF_DMX_ResetTSBuffer(dm->ts_buf);
                if (HI_SUCCESS != ret) {
                    LOGE("HI_UNF_DMX_ResetTSBuffer, Ret=%x",ret);
                    return -1;
                }
            }
        }
        */
    #if 1
        ret = HI_UNF_AVPLAY_Reset(av->handle, NULL);//reset 不会改变播放器的状态
		#else
        HI_UNF_AVPLAY_STOP_OPT_S stStopOpt;
        stStopOpt.u32TimeoutMs = 0;
        stStopOpt.enMode = HI_UNF_AVPLAY_STOP_MODE_STILL;
        ret = HI_UNF_AVPLAY_Stop(av->handle, HI_UNF_AVPLAY_MEDIA_CHAN_E(HI_UNF_AVPLAY_MEDIA_CHAN_VID|HI_UNF_AVPLAY_MEDIA_CHAN_AUD), &stStopOpt);
        if (HI_SUCCESS != ret) {
            LOGE("HI_UNF_AVPLAY_Stop, Ret=%x",ret);
            return -1;
        }		
		ret = HI_UNF_AVPLAY_Start (av->handle, HI_UNF_AVPLAY_MEDIA_CHAN_E(HI_UNF_AVPLAY_MEDIA_CHAN_VID|HI_UNF_AVPLAY_MEDIA_CHAN_AUD), NULL);
		#endif
        if (HI_SUCCESS != ret) {
            LOGE("HI_UNF_AVPLAY_Reset, Ret=%x",ret);
            return -1;
        }
    }
	LOGD("clear out");
    return 0;
}

static int avdec_set_rate(struct avdec_device_t *dev, int handle, float rate)
{
    int ret;
    avplayer_t *av = (avplayer_t *)handle;

    LOGD("set_rate handle=%d,%x, %s, rate=%f", GET_ID (av), handle, GET_CODE(av), rate);

    if ( 0==av->stat ){
        LOGD("decoder not start yet");
        return 0;
    }
    //todo...
    return 0;
}
static int avdec_set_decode_mode(struct avdec_device_t *dev, int handle, int mode)
{
    int ret;
    HI_UNF_VCODEC_MODE_E enDecodeMode=HI_UNF_VCODEC_MODE_NORMAL;
    HI_UNF_VCODEC_ATTR_S VdecAttr;
    avplayer_t *av = (avplayer_t *)handle;

    LOGD("set_decode_mode handle=%d,%x, %s, mode=%d", GET_ID (av), handle, GET_CODE(av), mode);

    if ( 0==av->stat ){
        LOGD("decoder not start yet");
        //return 0;
    }

    switch (mode) {
    case VDEC_DECODER_IFRAME:
        enDecodeMode = HI_UNF_VCODEC_MODE_I;
        break;
    case VDEC_DECODER_IPFRAME:
        enDecodeMode = HI_UNF_VCODEC_MODE_IP;
        break;
    default:
    case VDEC_DECODER_NORMAL:
        enDecodeMode = HI_UNF_VCODEC_MODE_NORMAL;
        break;
    }

    if ( av->handle ){
        HI_UNF_AVPLAY_GetAttr(av->handle, HI_UNF_AVPLAY_ATTR_ID_VDEC, &VdecAttr);
        if ( VdecAttr.enMode != enDecodeMode){
            ret = HI_UNF_AVPLAY_SetDecodeMode (av->handle, enDecodeMode);
            if (HI_SUCCESS != ret) {
                LOGE("HI_UNF_AVPLAY_SetDecodeMode, Ret=%x",ret);
                return -1;
            }
        }
    }
    return 0;
}
static int avdec_get_pts(struct avdec_device_t *dev, int handle, long long*t)
{
    int ret;
    HI_UNF_AVPLAY_STATUS_INFO_S stStatusInfo;
    avplayer_t *av = (avplayer_t *)handle;

    LOGD("get_pts handle=%d,%x, %s, t=%x", GET_ID (av), handle, GET_CODE(av), t);

    if ( 0==av->stat ){
        LOGD("decoder not start yet");
        return -1;
    }

    ret = HI_UNF_AVPLAY_GetStatusInfo(av->handle, &stStatusInfo);
    if (HI_SUCCESS != ret) {
        LOGE("HI_UNF_AVPLAY_GetStatusInfo, Ret=%x",ret);
        return -1;
    }

    if (t){
        if (av->id%2)
            (*t) = stStatusInfo.stSyncStatus.u32LastVidPts;
        else
			(*t) = stStatusInfo.stSyncStatus.u32LastAudPts;
        LOGD("audio %d pts=%lld", stStatusInfo.stSyncStatus.u32LastAudPts,*t);
        if (*t == 0xffffffff){
            *t = 0;
            return -1;
        }
    }
	return 0;
}

static int avdec_get_video_size(struct avdec_device_t *dev, int handle, int*w, int*h)
{
    int ret;
	HI_UNF_AVPLAY_STREAM_INFO_S stStreamInfo;
    avplayer_t *av = (avplayer_t *)handle;

	LOGD("get_video_size handle=%d,%x, %s, w=%x, h=%x", GET_ID (av), handle, GET_CODE(av), w, h);

    if ( 0==av->stat ){
        LOGD("decoder not start yet");
        return -1;
    }

	ret = HI_UNF_AVPLAY_GetStreamInfo(av->handle, &stStreamInfo);
    if (HI_SUCCESS != ret) {
        LOGE("HI_UNF_AVPLAY_GetStreamInfo, Ret=%x",ret);
        return -1;
    }

	if (w)
		*w = stStreamInfo.stVidStreamInfo.u32DisplayWidth;
	if (h)
		*h = stStreamInfo.stVidStreamInfo.u32DisplayHeight;

	return 0;
}

static int avdec_check_rate(struct avdec_device_t *dev, int handle, float rate, float*real)
{
    int ret;
	HI_UNF_AVPLAY_STREAM_INFO_S stStreamInfo;
    avplayer_t *av = (avplayer_t *)handle;

	LOGD("get_video_size handle=%d,%x, %s, rate=%f, real=%x", GET_ID (av), handle, GET_CODE(av), rate, real);

    if ( 0==av->stat ){
        LOGD("decoder not start yet");
        return -1;
    }

    ret = HI_UNF_AVPLAY_GetStreamInfo(av->handle, &stStreamInfo);
    if (HI_SUCCESS != ret) {
        LOGE("HI_UNF_AVPLAY_GetStreamInfo, Ret=%x",ret);
        return -1;
    }
    if ( real )
    	*real = stStreamInfo.stVidStreamInfo.u32fpsInteger
    			+ stStreamInfo.stVidStreamInfo.u32fpsDecimal;

	return 0;
}

static unsigned int avdec_get_pts_value(unsigned char*buf, int len) {
	unsigned char *p = buf;
	unsigned int pts = 0;

	LOGE("[yinhua] %s\n", __FUNCTION__);

	/* 标准的pes header音频数据头 */
	if ((p[0] == 0x00) && (p[1] == 0x00) && (p[2] == 0x01) && (p[3] == 0xc0 || p[3] == 0xe0)) //magic 0xC0(audio) 0xE0(video)
	{
		unsigned char pts_dts_flag = 0;

		pts_dts_flag = p[7] & 0xc0;

		if (pts_dts_flag == 0x80) {
			if ((p[9] & 0xf0) != 0x20) {
				LOGE("this may be a bad pts !");
				return -1;
			}

			/* 计算pts value 时间 */
			pts = 0;
			pts = (unsigned int) (p[9] & 0x0e) << 29;
			pts += (unsigned int) p[10] << 22;
			pts += (unsigned int) (p[11] & 0xfe) << 14;
			pts += (unsigned int) p[12] << 7;
			pts += (unsigned int) (p[13] >> 1) & 0x7f;

			/* 转换成 ms */
			pts = pts / 90;
		} else if (pts_dts_flag == 0xc0) {
			if ((p[9] & 0xf0) != 0x30) {
				LOGE("this may be a bad pts !");
				return -1;
			}

			/* 计算pts value 时间 */
			pts = 0;
			pts = (unsigned int) (p) << 29;
			pts += (unsigned int) p[10] << 22;
			pts += (unsigned int) (p[11] & 0xfe) << 14;
			pts += (unsigned int) p[12] << 7;
			pts += (unsigned int) (p[13] >> 1) & 0x7f;

			/* 转换成 ms */
			pts = pts / 90;
		} else if ((pts_dts_flag == 0x00) || (pts_dts_flag == 0x40)) {
			LOGE("error dts value ..");
			return -1;
		}
	}

	return pts;
}

static unsigned int avdec_get_pts_value2(unsigned char*buf, int len) {
	unsigned char *p = buf;
	unsigned int pts = 0, dts = 0;
	unsigned char *sp = NULL;

	LOGE("[yinhua] %s\n", __FUNCTION__);

	/* 标准的pes header音频数据头 */
	if ((p[0] == 0x00) && (p[1] == 0x00) && (p[2] == 0x01) && (p[3] == 0xc0 || p[3] == 0xe0)) //magic 0xC0(audio) 0xE0(video)
	{
		p += 7;
		sp = p;

		//开始计算pts时间,其实际为PTS[32..0]
		{
			unsigned char PTS_DTS_flags = (*sp >> 6) & 0x3;
			unsigned char PES_header_data_length;

			sp++;
			PES_header_data_length = *sp;
			sp++;

			if (PTS_DTS_flags == 0x2)//只存在PTS时间
			{
				//unsigned int pts;
				pts = (*sp >> 1) & 0x7;
				pts = pts << 30;
				sp++;
				pts += (*sp) << 22;
				sp++;
				pts += ((*sp) >> 1) << 15;
				sp++;
				pts += (*sp) << 7;
				sp++;
				pts += (*sp) >> 1;
				sp++;

				//换算成ms毫秒
				pts = pts / 45;
			} else if (PTS_DTS_flags == 0x3) //存在PTS&DTS时间
			{
				pts = (*sp >> 1) & 0x7;
				pts = pts << 30;
				sp++;
				pts += (*sp) << 22;
				sp++;
				pts += ((*sp) >> 1) << 15;
				sp++;
				pts += (*sp) << 7;
				sp++;
				pts += (*sp) >> 1;
				sp++;

				dts = (*sp >> 1) & 0x7;
				dts = dts << 30;
				sp++;
				dts += (*sp) << 22;
				sp++;
				dts += ((*sp) >> 1) << 15;
				sp++;
				dts += (*sp) << 7;
				sp++;
				dts += (*sp) >> 1;
				sp++;

				//换算成ms毫秒
				pts = pts / 45;
			} else if (PTS_DTS_flags != 0) //ERROR
			{
				LOGE( "error flags = %d\n",PTS_DTS_flags );
			}
		}
	}

	return pts;
}

#define BACKUP_AUDIO_MEM_SIZE 		(256*1024)
#define BACKUP_VIDEO_MEM_SIZE		(1*1024*1024)

int g_audio_datalen0 = 0;
int g_video_datalen0 = 0;
unsigned char *g_audio_buf0 = (unsigned char *) malloc(BACKUP_AUDIO_MEM_SIZE);
unsigned char *g_video_buf0 = (unsigned char *) malloc(BACKUP_VIDEO_MEM_SIZE);

static int avdec_push_data(struct avdec_device_t *dev, int handle, void*buf, int len, unsigned int pts)
{
    int ret;
    avplayer_t *av = (avplayer_t *)handle;

	LOGD("push_data handle=%d,%x, %s, buf=%x, len=%d, pts=%u", GET_ID (av), handle, GET_CODE(av), buf, len, pts);

    if ( 0==av->stat ){
        LOGD("decoder not start yet");
        return -1;
    }

	HI_UNF_STREAM_BUF_S StreamBuf = { 0 };
	HI_UNF_AVPLAY_ATTR_S avplay_attr;
	unsigned int pes_length;
	unsigned int pts_value = 0;
	unsigned char *ptr = (unsigned char *) buf; //记录缓冲区首地址
	unsigned char *p = (unsigned char *) buf; //消耗数据缓冲区首地址
	unsigned char *q; //数据注入临时缓冲区首地址
	HI_UNF_AVPLAY_BUFID_E enBufId;
	unsigned int consume_byte = 0;
	int first_push = 1;
	int fake_len = 8 * 1024;
	int total_len = len;

	HI_HANDLE s_hAvPlay = av->handle;

	if ( 0==(av->id%2) )//音频
	{
		enBufId = HI_UNF_AVPLAY_BUF_ID_ES_AUD;
		if (g_audio_datalen0 != 0 && BACKUP_AUDIO_MEM_SIZE - g_audio_datalen0 >= len) {
			memcpy(g_audio_buf0 + g_audio_datalen0, buf, len);
			g_audio_datalen0 += len;
			buf = g_audio_buf0;
			len = g_audio_datalen0;
			ptr = (unsigned char *) buf;
			p = (unsigned char *) buf;
		}
	} else if (av->id%2) {
		enBufId = HI_UNF_AVPLAY_BUF_ID_ES_VID;
		if (g_video_datalen0 != 0 && BACKUP_VIDEO_MEM_SIZE - g_video_datalen0 >= len) {
			memcpy(g_video_buf0 + g_video_datalen0, buf, len);
			g_video_datalen0 += len;
			buf = g_video_buf0;
			len = g_video_datalen0;
			ptr = (unsigned char *) buf;
			p = (unsigned char *) buf;
		}
	}

	if (0==(av->id%2)) {
		pes_length = len;
		pts_value = pts;
		ret = HI_UNF_AVPLAY_GetBuf(s_hAvPlay, enBufId, pes_length + fake_len, &StreamBuf, 0);
		if (HI_SUCCESS != ret) {
			HI_UNF_AVPLAY_STATUS_INFO_S stStatusInfo;
			LOGW("HI_UNF_AVPLAY_GetBuf failed!!!avdecHandle= %x ret= 0x%x StreamBuf_size= %d",
					handle,ret,StreamBuf.u32Size);

			LOGI("decoder audio status info:=================");
			HI_UNF_AVPLAY_GetStatusInfo(s_hAvPlay, &stStatusInfo);
			LOGI("stSyncStatus u32LastAudPts=%d,u32LastVidPts=%d",stStatusInfo.stSyncStatus.u32LastAudPts,stStatusInfo.stSyncStatus.u32LastVidPts);
			LOGI("enRunStatus = %d",stStatusInfo.enRunStatus);
			LOGI("u32VidFrameCount = %d",stStatusInfo.u32VidFrameCount);
			LOGI("u32AuddFrameCount = %d",stStatusInfo.u32AuddFrameCount);
			LOGI("u32UsedSize = %d",stStatusInfo.stBufStatus[enBufId].u32UsedSize);
			LOGI("u32VidErrorFrameCount = %d",stStatusInfo.u32VidErrorFrameCount);

			goto LABEL_EXIT;
		}

		memcpy(StreamBuf.pu8Data, p, pes_length);
		ret = HI_UNF_AVPLAY_PutBuf(s_hAvPlay, enBufId, pes_length, pts_value);
		if (HI_SUCCESS != ret) {
			LOGE("impossible HI_UNF_AVPLAY_PutBuf failed ...avdecHandle=%x ret= 0x%x",handle,ret);//一般来说不会出现这个错误
			goto LABEL_EXIT;
		}

		consume_byte = len;
	} else if ( av->id%2 ) {
		while (1) {
			if (len - (p - ptr) < 14)//剩余数据量小于一个pes header的时候退出,防止越界
			{
				break;
			}

			/*获取PES-HEADER: 0X000001C0 & 0X000001E0 */
			while (p[0] != 0 || p[1] != 0 || p[2] != 0x01 || //packet_start_code_prefix
					((p[3] & 0xe0) != 0xe0 && (p[3] & 0xe0) != 0xc0)) //判断是否为视频流(e0) 1110 或音频流(c0) 1101
			{
				//LOGE("error not find PES header ..........avdecHandle = %d",avdecHandle);
				p++;
				consume_byte++; //丢弃这块数据，避免陷入死循环
				if (len - (p - ptr) < 14) {
					goto LABEL_EXIT;
				}
			}

			pes_length = (p[4] << 8) | p[5]; //PES_packet_length=length 这个length包含了8字节的header部分
			if (pes_length == 0)//结构不固定，长度未指定
			{
				LOGW("waring ....the pes is not stable,length is not stable pes_length =%d",pes_length);
				q = p + 6;

				//寻找下一个流的开头
				while (q[0] != 0 || q[1] != 0 || q[2] != 0x01 || ((q[3] & 0xe0) != 0xe0 && (q[3]
						& 0xc0) != 0xc0))///(p[3] & 0xf0) != 0xe0
				{
					q++;
					if (len - (p - ptr) < 14) {
						goto LABEL_EXIT;
					}
				}
				//求的pes的长度
				pes_length = q - (p + 6);
				LOGW("waring ....the pes is not stable,length is not stable pes_length =%d",pes_length);
			}

			pes_length = pes_length - 8;//真正的pes包的长度应该减去8字节的header部分

			if (len - (p - ptr) < pes_length + 14) {
				if (first_push == 1) {
					LOGW("avdec_push_data drop data len= %d",len);
					goto LABEL_EXIT;
					//return AVDEC_INCOMPLETE_PACKAGE;//不能丢数据，这个枚举作废,使用全局buffer在这里缓存数据，
				}
				LOGI("not enough pes data and wait next package");
				break;
			}

			pts_value = avdec_get_pts_value(p, 14);

			LOGV("avdec_push_data avdecHandle = %x pts_value = %d pes_len= %d",handle,pts_value,pes_length);

			if (pts_value < 0) {
				LOGV("error pts value");
				pts_value = -1;
			}

			/*
			 往底层推buffer的策略：
			 如果实际只需要8k的数据，咱们要个虚假空间16k，假设成功返回，我们只推8k的数据,保证给下面留空间
			 typedef struct hiUNF_STREAM_BUF_S
			 {
			 HI_U8   *pu8Data;       数据指针
			 HI_U32  u32Size;        数据长度
			 } HI_UNF_STREAM_BUF_S;
			 这里有两种情形会导致getbuf失败，内存不够 or 底层缓存满
			 HI_ERR_ADEC_IN_BUF_FULL   (HI_S32)(0x80140004)
			 HI_ERR_ADEC_OUT_BUF_EMPTY   (HI_S32)(0x80140005)
			 为了避免一个pes包只推一半的尴尬情形，这里一次性要求获取到当前pes包的buffer pes包最大长度 < 64k?
			 */
			ret = HI_UNF_AVPLAY_GetBuf(s_hAvPlay, enBufId, pes_length + fake_len, &StreamBuf, 0);
			if (HI_SUCCESS != ret) {
				HI_UNF_AVPLAY_STATUS_INFO_S stStatusInfo;
				LOGW("HI_UNF_AVPLAY_GetBuf failed!!!avdecHandle= %d ret= 0x%x StreamBuf_size= %d",
						handle,ret,StreamBuf.u32Size);

				LOGI("decoder video status info:=================");
				HI_UNF_AVPLAY_GetStatusInfo(s_hAvPlay, &stStatusInfo);
				LOGI("stSyncStatus u32LastAudPts=%d,u32LastVidPts=%d",stStatusInfo.stSyncStatus.u32LastAudPts,stStatusInfo.stSyncStatus.u32LastVidPts);
				LOGI("enRunStatus = %d",stStatusInfo.enRunStatus);
				LOGI("u32VidFrameCount = %d",stStatusInfo.u32VidFrameCount);
				LOGI("u32AuddFrameCount = %d",stStatusInfo.u32AuddFrameCount);
				LOGI("u32UsedSize = %d",stStatusInfo.stBufStatus[enBufId].u32UsedSize);
				LOGI("u32VidErrorFrameCount = %d",stStatusInfo.u32VidErrorFrameCount);

				goto LABEL_EXIT;
			}

			LOGW("HI_UNF_AVPLAY_PutBuf pts_value = %x",pts_value);
			memcpy(StreamBuf.pu8Data, p + 14, pes_length);
			ret = HI_UNF_AVPLAY_PutBuf(s_hAvPlay, enBufId, pes_length, pts_value);
			if (HI_SUCCESS != ret) {
				LOGE("impossible HI_UNF_AVPLAY_PutBuf failed ...avdecHandle=%x ret= 0x%x",handle,ret);//一般来说不会出现这个错误
				goto LABEL_EXIT;
			}

			first_push = 0;

			//move next pes package
			p += (pes_length + 14);
			consume_byte += (pes_length + 14);
		}
	}

	LABEL_EXIT:
        LOGV("avdec_push_data %x consume_byte %d",handle,consume_byte);
	if (consume_byte != (unsigned int) len) {
		if ((av->id%2)==0)//音频
		{
			if (buf == g_audio_buf0) {
				memmove((unsigned char*) g_audio_buf0, (unsigned char*) buf + consume_byte,
						len - consume_byte);
				g_audio_datalen0 = len - consume_byte;
			} else {
				memcpy((unsigned char*) g_audio_buf0, (unsigned char*) buf + consume_byte,
						len - consume_byte);
				g_audio_datalen0 = len - consume_byte;
			}
		} else if (av->id%2) {
			if (buf == g_video_buf0) {
				memmove((unsigned char*) g_video_buf0, (unsigned char*) buf + consume_byte,
						len - consume_byte);
				g_video_datalen0 = len - consume_byte;
			} else {
				memcpy((unsigned char*) g_video_buf0, (unsigned char*) buf + consume_byte,
						len - consume_byte);
				g_video_datalen0 = len - consume_byte;
			}
		}
	} else {
		if (0==(av->id%2))//音频
		{
			g_audio_datalen0 = 0;
		} else if (av->id%2) {
			g_video_datalen0 = 0;
		}
	}
	return total_len;
}

static int avdec_ioctl(struct avdec_device_t *dev, int handle, int op, struct io_block_t*iob)
{
    int ret;
    avplayer_t *av = (avplayer_t *)handle;

	LOGD("push_data handle=%d,%x, %s, op=%d, iob=%x", GET_ID (av), handle, GET_CODE(av), op, iob);

    if ( 0==av->stat ){
        LOGD("decoder not start yet");
        return -1;
    }

	HI_HANDLE s_hAvPlay = av->handle;

	if (0==(av->id%2)) {
		switch (op) {
		case ADEC_CLEAR_BUFFER:
			ret = avdec_clear(dev, handle);
			break;

		case ADEC_GET_BUFFER_RATE: {
			HI_UNF_AVPLAY_STATUS_INFO_S audio_info;
			ret = HI_UNF_AVPLAY_GetStatusInfo(s_hAvPlay, &audio_info);
			if (ret != HI_SUCCESS) {
				LOGE("HI_UNF_AVPLAY_GetStatusInfo failed ret=0x%x! in[avdec_ioctl]",ret);
				return -1;
			}
			if (audio_info.stBufStatus[1].u32UsedSize == 0)
				ret = 0;
			else
				ret = (audio_info.stBufStatus[1].u32UsedSize * 100)
						/ audio_info.stBufStatus[1].u32BufSize;

			int *pInt = (int*) iob->out;
			*pInt = ret;
		}
			break;

		case ADEC_SET_PASS_THROUGH:
		{
			int *pInt = (int*) iob->in;
			LOGD("m_avdec_pass_through = %d ",m_avdec_pass_through);
			m_avdec_pass_through = *pInt;
			ret = 0;
			LOGD("m_avdec_pass_through = %d ",m_avdec_pass_through);
		}
			break;

		case ADEC_STOP_MODE: {
			int *pInt = (int*) iob->in;
			ret = 0;
		}
		default:
			break;
		}
	} else if (av->id%2) {
		switch (op) {
		case VDEC_CLEAR_BUFFER:
			ret = avdec_clear(dev, handle);
			break;
		case VDEC_GET_BUFFER_RATE: {
			HI_UNF_AVPLAY_STATUS_INFO_S vedio_info;
			ret = HI_UNF_AVPLAY_GetStatusInfo(s_hAvPlay, &vedio_info);
			if (ret != HI_SUCCESS) {
				LOGE("HI_UNF_AVPLAY_GetStatusInfo failed ret=0x%x! in[avdec_ioctl]",ret);
				return -1;
			}
			if (vedio_info.stBufStatus[1].u32UsedSize == 0)
				ret = 0;
			else
				ret = (vedio_info.stBufStatus[1].u32UsedSize * 100)
						/ vedio_info.stBufStatus[1].u32BufSize;

			LOGD("vedio_info.stBufStatus[1].u32BufSize = 0x%x ",vedio_info.stBufStatus[1].u32BufSize);
			LOGD("ret = %d ",ret);
			LOGD("vedio_info.stBufStatus[1].u32UsedSize = 0x%x ",vedio_info.stBufStatus[1].u32UsedSize);
			LOGD("vedio_info.stBufStatus[0].u32BufSize = 0x%x ",vedio_info.stBufStatus[0].u32BufSize);
			LOGD("vedio_info.stBufStatus[0].u32UsedSize = 0x%x ",vedio_info.stBufStatus[0].u32UsedSize);
			int *pInt = (int*) iob->out;
			*pInt = ret;
			LOGD("*pInt = %d ",*pInt);
		}
			break;

		default:
			break;
		}
	}

	return ret;
}

int avdec_get_list(struct avdec_module_t *module, struct avdec_t const **list)
{
    int i;
	avdec_context_t *ctx = m_avdec_ctx;

    if ( !ctx ){
        ctx = m_avdec_ctx = (avdec_context_t *)calloc(1, sizeof(avdec_context_t));
        if ( !ctx ){
            LOGE("calloc ctx failed");
            return -1;
        }
    }
    if ( !ctx->list ){
    	ctx->list = (struct avdec_t*) calloc(1, sizeof(struct avdec_t) * AVDEC_MAX_NUM);
    	if ( !ctx->list ) {
            LOGE("calloc list failed");
    		return -1;
    	}
    	for (i = 0; i < AVDEC_MAX_NUM; i++) {
            ctx->avdec[i].id = i;
            ctx->avdec[i].community = &ctx->avdec[(i%2)?(i-1):(i+1)];
            ctx->list[i].handle = (int)&ctx->avdec[i];
            ctx->list[i].type = (i%2)?MEDIA_TYPE_VIDEO:MEDIA_TYPE_AUDIO;
            ctx->list[i].support_push_src = 1;
            ctx->list[i].support_demux_src = 1;
    		LOGD("get_list, %d,%x", i, ctx->list[i].handle);
    	}
    }
    (*list) = ctx->list;
	LOGD("get_list num=%d\n", AVDEC_MAX_NUM);
    return AVDEC_MAX_NUM;
}

/*****************************************************************************/

static int close_avdec(struct hw_device_t *dev)
{
	int ret = 0, i;
	struct avdec_context_t* ctx = (struct avdec_context_t*) dev;

	if (ctx == NULL) {
		LOGE("[close_avdec] null ctx.\n");
		return -1;
	}
    //todo...destroy avplayer
	if ( ctx->list ) {
		free(ctx->list);
		ctx->list = NULL;
	}
	if ( ctx ) {
		free(ctx);
		ctx = NULL;
	}
    pthread_mutex_destroy(&mAvdecMutex);

    m_avdec_ctx = NULL;
    LOGD("close");
	return ret;
}

int open_avdec(const struct hw_module_t* module, const char* name, struct hw_device_t** device)
{
	HI_S32 ret = HI_SUCCESS;
	avdec_context_t *ctx = m_avdec_ctx;

	LOGD("open_avdec");

	if (!strcmp(name, AVDEC_HARDWARE_AVDEC0)) {
        if ( !ctx ){
            ctx = m_avdec_ctx = (avdec_context_t *)calloc(1, sizeof(avdec_context_t));
            if ( !ctx ){
                LOGE("calloc ctx failed");
                return -1;
            }
        }
		ctx->device.common.tag = HARDWARE_DEVICE_TAG;
		ctx->device.common.version = 1;
		ctx->device.common.module = const_cast<hw_module_t*> (module);
		ctx->device.common.close = close_avdec;

		ctx->device.bind_demux_src = avdec_bind_demux;
		ctx->device.bind_push_src = avdec_bind_data;
		ctx->device.unbind = avdec_unbind;

		ctx->device.get_format_list = avdec_get_format_list;
		ctx->device.set_format = avdec_set_format;
		ctx->device.set_sync = avdec_set_sync;

		ctx->device.start = avdec_start;
		ctx->device.resume = avdec_resume;
		ctx->device.pause = avdec_pause;
		ctx->device.stop = avdec_stop;
		ctx->device.clear_data = avdec_clear;

		ctx->device.get_pts = avdec_get_pts;
		ctx->device.get_video_size = avdec_get_video_size;
		ctx->device.set_rate = avdec_set_rate;
		ctx->device.check_rate = avdec_check_rate;
		ctx->device.set_decode_mode = avdec_set_decode_mode;

		ctx->device.push_data = avdec_push_data;

		ctx->device.ioctl = avdec_ioctl;

        pthread_mutex_init(&mAvdecMutex, NULL);
#if 1
        ret = HI_SYS_Init ();
		ret |= HI_UNF_AVPLAY_Init();
		if (HI_SUCCESS != ret) {
			LOGE("[open_avdec]HI_UNF_AVPLAY_Init fail,ret=0x%x\n",ret);
			ret = -1;
			goto BAIL;
		}
		/* 加载必要的音频动态库 */

		/*********************************************************************/
        HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.AAC.decode.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.AC3PASSTHROUGH.decode.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.AMRNB.codec.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.AMRWB.codec.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.BLURAYLPCM.decode.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.COOK.decode.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.DRA.decode.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.DTSPASSTHROUGH.decode.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.FFMPEG_ADEC.decode.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.FFMPEG_WMAPRO.decode.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.G711.codec.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.MP2.decode.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.MP3.decode.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.PCM.decode.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.TRUEHDPASSTHROUGH.decode.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.WMA.decode.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.G711.codec.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.WMA.decode.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.DOLBYPLUS.decode.so");
		HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.DTSHD.decode.so");
#endif
		if (ret == 0) {
			*device = &ctx->device.common;
		} else {
			close_avdec(&ctx->device.common);
		}

		LOGI("[open_avdec] open succ!\n");

		return 0;
	} else {
		LOGE("[open_avdec] error name.\n");
		ret = -1;
	}

	BAIL: LOGE("[open_avdec] open fail!\n");
	return ret;
}

