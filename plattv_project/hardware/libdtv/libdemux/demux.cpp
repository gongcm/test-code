/* ----------------------------------------------------------------------------
 * @module
 *			HAL module
 * @routines
 *			demux device
 * @date
 *			2011.3
 */
//#define LOG_NDEBUG		0
#define LOG_TAG "[plattv]demux"

#include <cutils/log.h>

#include <linux/fb.h>

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <cutils/properties.h>


#include <linux/android_pmem.h>
#include <hardware/demux.h>
#include "hi_type.h"
#include "hi_unf_demux.h"
#include "hi_unf_descrambler.h"
#include "hi_audio_codec.h"
#include "hal_common.h"

static struct demux_context_t *m_demux_ctx = NULL;

/**
 * Common hardware methods
 */
static int demux_get_list(struct demux_module_t *module, struct demux_t const **list);

static int open_demux(const struct hw_module_t* module, const char* name,
		struct hw_device_t** device);

static struct hw_module_methods_t demux_module_methods = {
	open: open_demux
};

/*
 * The demux Module
 */
struct demux_module_t HAL_MODULE_INFO_SYM = {
	common: {
		tag: HARDWARE_MODULE_TAG,
		version_major: 1,
		version_minor: 0,
		id: DEMUX_HARDWARE_MODULE_ID,
		name: "demux Module",
		author: "iPanel 2011, Inc.",
		methods: &demux_module_methods,
		dso: NULL,
		reserved: {0}
	},
	get_demux_list:demux_get_list,
};

/** DEMUX ID定义*/
/*
 #define HI_UNF_DMX_ID_0_PLAY            0
 #define HI_UNF_DMX_ID_1_REC             1
 #define HI_UNF_DMX_ID_2_REC             2
 #define HI_UNF_DMX_ID_3_REC             3
 #define HI_UNF_DMX_ID_4_PLAY            4
 */

/******************************************************************************/

/**@function
 *		说明 ：  目前只设置demuxid号为0的设备与tuner绑定
 */
static int demux_bind_tuner(struct demux_device_t *dev, int dhandle, struct tuner_device_t*Tdev,
		int thandle)
{
    int ret, i, dmx0, dmx4;
    mdemux_t *dm = (mdemux_t *)dhandle;

    LOGD("bind_tuner, dhandle=%x, Tdev=%x, thandle=%x", dhandle, Tdev, thandle);

    if ( dm->ts_buf ){
		ret = HI_UNF_DMX_DestroyTSBuffer(dm->ts_buf);
		if (HI_SUCCESS != ret) {
			LOGE("HI_UNF_DMX_DestroyTSBuffer,Ret=%x",ret);
			return -1;
		}
        dm->ts_buf = 0;
    }

    if ( DEMUX_WORK_MODE_UNDEF != dm->mode ){
    	ret = HI_UNF_DMX_DetachTSPort(dm->dmxid);
    	if (HI_SUCCESS != ret) {
    		LOGE("HI_UNF_DMX_DetachTSPort,Ret=%x",ret);
    		return -1;
    	}
    }
    dm->dmxid = -1;

    //check if dmx 0 and 4 available
    dmx0 = 0; //HI_UNF_DMX_ID_0_PLAY
    dmx4 = 4; //HI_UNF_DMX_ID_4_PLAY
    for (i=0; i<DEMUX_MAX_NUM; i++){
        mdemux_t *t = &m_demux_ctx->demux[i];
        if ( dmx0==t->dmxid )
            dmx0 = -1;
        if ( dmx4==t->dmxid )
            dmx4 = -1;
    }

    if ( -1!=dmx0 )
        dm->dmxid = dmx0;
    else if (-1!=dmx4)
        dm->dmxid = dmx4;
    else{
        LOGE("no more free demux id");
        return -1;
    }

    mtuner_t *mt = (mtuner_t*)thandle;
    HI_UNF_DMX_PORT_ATTR_S PortAttr;
    if( 0==mt->id ){
        HI_UNF_DMX_GetTSPortAttr(HI_UNF_DMX_PORT_TSI_0, &PortAttr);
		    PortAttr.enPortMod = HI_UNF_DMX_PORT_MODE_EXTERNAL;
    		PortAttr.enPortType = HI_UNF_DMX_PORT_TYPE_PARALLEL_NOSYNC_188;//;
				PortAttr.u32TunerInClk	= 0; //default value is 0
				PortAttr.u32SerialBitSelector = 0; //Data[0]为数据位
				PortAttr.u32TunerErrMod = 1; //default value is 1        
        ret=HI_UNF_DMX_SetTSPortAttr(HI_UNF_DMX_PORT_TSI_0, &PortAttr);
        LOGI("HI_UNF_DMX_PORT_TSI_0 : %d \n",HI_UNF_DMX_PORT_TSI_0);
        LOGI("HI_UNF_DMX_PORT_MODE_EXTERNAL : %d \n",HI_UNF_DMX_PORT_MODE_EXTERNAL);
        LOGI("HI_UNF_DMX_PORT_TYPE_PARALLEL_NOSYNC_188 : %d \n",HI_UNF_DMX_PORT_TYPE_PARALLEL_NOSYNC_188);
    	if (HI_SUCCESS != ret) {
    		LOGE("HI_UNF_DMX_SetTSPortAttr,Ret=%x",ret);
    		return -1;
    	}
        dm->port = HI_UNF_DMX_PORT_TSI_0;
    }
    else if ( 1==mt->id ){
#if 0	
        /* For parallel TS */
        HI_UNF_DMX_GetTSPortAttr(HI_UNF_DMX_PORT_TSI_1, &PortAttr);
        PortAttr.enPortType = HI_UNF_DMX_PORT_TYPE_PARALLEL_VALID;
        PortAttr.enPortMod = HI_UNF_DMX_PORT_MODE_INTERNAL;
        PortAttr.u32SerialBitSelector = 0;
        ret= HI_UNF_DMX_SetTSPortAttr(HI_UNF_DMX_PORT_TSI_1, &PortAttr);
    	if (HI_SUCCESS != ret) {
    		LOGE("HI_UNF_DMX_SetTSPortAttr,Ret=%x",ret);
    		return -1;
    	}
        dm->port = HI_UNF_DMX_PORT_TSI_1;
#endif        
    }

	LOGI("HI_UNF_DMX_AttachTSPort: dm->dmxid:%x , (HI_UNF_DMX_PORT_E)dm->port):%x\n",dm->dmxid, (HI_UNF_DMX_PORT_E)dm->port);

    ret = HI_UNF_DMX_AttachTSPort(dm->dmxid, (HI_UNF_DMX_PORT_E)dm->port);
    if (HI_SUCCESS != ret) {
        LOGE("HI_UNF_DMX_AttachTSPort,Ret=%x",ret);
        return -1;
    }
    LOGD("bind_tuner, dmxid=%d, port=%d", dm->dmxid, dm->port);
    dm->mode = DEMUX_WORK_MODE_BIND;
    mt->demux = dhandle;

	return 0;
}

static int demux_bind_data(struct demux_device_t *dev, int handle)
{
    int ret, i, dmx0, dmx4;
    mdemux_t *dm = (mdemux_t *)handle;

    LOGD("bind_data, handle=%x", handle);

    if ( dm->ts_buf ){
		ret = HI_UNF_DMX_DestroyTSBuffer(dm->ts_buf);
		if (HI_SUCCESS != ret) {
			LOGE("HI_UNF_DMX_DestroyTSBuffer,Ret=%x",ret);
			return -1;
		}
        dm->ts_buf = 0;
    }

    if ( DEMUX_WORK_MODE_UNDEF != dm->mode ){
    	ret = HI_UNF_DMX_DetachTSPort(dm->dmxid);
    	if (HI_SUCCESS != ret) {
    		LOGE("HI_UNF_DMX_DetachTSPort,Ret=%x",ret);
    		return -1;
    	}
    }
    dm->dmxid = -1;
    //check if dmx 0 and 4 available
    dmx0 = 0; //HI_UNF_DMX_ID_0_PLAY
    dmx4 = 4; //HI_UNF_DMX_ID_4_PLAY
    for (i=0; i<DEMUX_MAX_NUM; i++){
        mdemux_t *t = &m_demux_ctx->demux[i];
        if ( dmx0==t->dmxid )
            dmx0 = -1;
        if ( dmx4==t->dmxid )
            dmx4 = -1;
    }

    if ( -1!=dmx0 )
        dm->dmxid = dmx0;
    else if (-1!=dmx4)
        dm->dmxid = dmx4;
    else{
        LOGE("no more free demux id");
        return -1;
    }

    dm->port = HI_UNF_DMX_PORT_RAM_0;
    ret = HI_UNF_DMX_AttachTSPort(dm->dmxid, (HI_UNF_DMX_PORT_E)dm->port);
    if (HI_SUCCESS != ret) {
        LOGE("HI_UNF_DMX_AttachTSPort,Ret=%x",ret);
        return -1;
    }

	if ( 0 == dm->ts_buf ) {
        ret = HI_UNF_DMX_CreateTSBuffer((HI_UNF_DMX_PORT_E)dm->port, DEMUX_TS_BUFSIZE, &dm->ts_buf);
        if (HI_SUCCESS != ret) {
            LOGE("HI_UNF_DMX_CreateTSBuffer, Ret=%x",ret);
            return -1;
        }
    }
    dm->mode = DEMUX_WORK_MODE_PUSH;
    LOGD("bind_tuner, dmxid=%d, port=%d", dm->dmxid, dm->port);
	return 0;
}

static int demux_unbind(struct demux_device_t *dev, int handle)
{
    int ret;
    mdemux_t *dm = (mdemux_t *)handle;

    LOGD("unbind, handle=%x", handle);

    if ( dm->ts_buf ){
		ret = HI_UNF_DMX_DestroyTSBuffer(dm->ts_buf);
		if (HI_SUCCESS != ret) {
			LOGE("HI_UNF_DMX_DestroyTSBuffer,Ret=%x",ret);
			return -1;
		}
        dm->ts_buf = 0;
    }

    if ( DEMUX_WORK_MODE_UNDEF != dm->mode ){
    	ret = HI_UNF_DMX_DetachTSPort(dm->dmxid);
    	if (HI_SUCCESS != ret) {
    		LOGE("HI_UNF_DMX_DetachTSPort,Ret=%x",ret);
    		return -1;
    	}
    }
    dm->dmxid = -1;
    dm->mode = DEMUX_WORK_MODE_UNDEF;

	return 0;
}

static int demux_create_channel(struct demux_device_t *dev, int handle, int poolSize, int type,
        int flags)
{
    int ret, i;
    mdemux_t *dm = (mdemux_t*)handle;
    channel_t *ch;
	HI_UNF_DMX_CHAN_ATTR_S stChAttr;

    LOGD("create_channel, handle=%x, poolsize=%d, type=%d, flags=%d", handle, poolSize, type, flags);
    HI_UNF_DMX_GetChannelDefaultAttr(&stChAttr);
    //stChAttr.u32BufSize = (poolSize>DEMUX_POOLSIZE_MIN)?((poolSize<DEMUX_POOLSIZE_MAX)?poolSize:DEMUX_POOLSIZE_MAX):DEMUX_POOLSIZE_MIN;
	stChAttr.u32BufSize = (poolSize>DEMUX_POOLSIZE_MIN)?poolSize:DEMUX_POOLSIZE_MIN;

    for (i=0; i<DEMUX_MAX_CHANNEL; i++){
        ch = &dm->channel[i];
        if ( 0==ch->id ){
            switch (type)
            {
            case DEMUX_CH_TYPE_AUDIO:
            case DEMUX_CH_TYPE_VIDEO:
            case DEMUX_CH_TYPE_PCR:
                break;
            case DEMUX_CH_TYPE_SECTION:
            case DEMUX_CH_TYPE_PES:
            case DEMUX_CH_TYPE_TS:
                {
                    if (DEMUX_CH_TYPE_PES==type)
                        stChAttr.enChannelType = HI_UNF_DMX_CHAN_TYPE_PES;
                    else if (DEMUX_CH_TYPE_TS==type)
		                stChAttr.enChannelType = HI_UNF_DMX_CHAN_TYPE_POST;
		            stChAttr.enOutputMode = HI_UNF_DMX_CHAN_OUTPUT_MODE_PLAY;
		            stChAttr.enCRCMode = HI_UNF_DMX_CHAN_CRC_MODE_FORBID;
		            ret = HI_UNF_DMX_CreateChannel(dm->dmxid, &stChAttr, &ch->handle);
                    if (HI_SUCCESS != ret) {
                        LOGE("HI_UNF_DMX_CreateChannel,Ret=%x",ret);
                        return -1;
                    }
                    ch->buflen = 0;
                    ch->pbuf = (unsigned char*)calloc(1, stChAttr.u32BufSize);
            		break;
            	}
	        case DEMUX_CH_TYPE_PVR:
	        case DEMUX_CH_TYPE_PVR_SLAVE:
        	default:
                {
            		LOGE("unsupport channel type=%d", type);
            		return -1;
            	}
            }
            ch->id = i+1;
            ch->type = type;
            LOGD("create_channel, %d,%x", i+1, ch);
            return ch->id; //接口demux_poll_channels的参数是个逗逼unsigned char*
        }
    }
    LOGE("too many channel");
    return -1;
}

static int demux_destroy_channel(struct demux_device_t *dev, int handle, int chid)
{
    int ret;
    mdemux_t *dm = (mdemux_t*)handle;
    LOGD("destroy_channel, handle=%x, chid=%x", handle, chid);
    channel_t *ch = &dm->channel[chid-1];

    switch (ch->type)
    {
    case DEMUX_CH_TYPE_AUDIO:
    case DEMUX_CH_TYPE_VIDEO:
    case DEMUX_CH_TYPE_PCR:
    case DEMUX_CH_TYPE_SECTION:
    case DEMUX_CH_TYPE_PES:
    case DEMUX_CH_TYPE_TS:
        {
            if ( ch->handle ){
                ret = HI_UNF_DMX_DestroyChannel(ch->handle);
                if (HI_SUCCESS != ret) {
                    LOGE("HI_UNF_DMX_CreateChannel,Ret=%x",ret);
                    return -1;
                }
            }
            if ( ch->pbuf ){
                free(ch->pbuf);
                ch->pbuf = NULL;
            }
            ch->handle = 0;
            ch->id = 0;
            ch->pid = 0;
            ch->type = 0;
            break;
        }
    case DEMUX_CH_TYPE_PVR:
    case DEMUX_CH_TYPE_PVR_SLAVE:
    default:
        {
            LOGE("unsupport channel type=%d", ch->type);
            return -1;
        }
    }
    return 0;
}

static int demux_start_channel(struct demux_device_t *dev, int handle, int chid, int pid)
{
    int ret;
    mdemux_t *dm = (mdemux_t*)handle;
    LOGD("start_channel, handle=%x, chid=%x, pid=%x", handle, chid, pid);
    channel_t *ch = &dm->channel[chid-1];


    switch (ch->type)
    {
    case DEMUX_CH_TYPE_AUDIO:
    case DEMUX_CH_TYPE_VIDEO:
    case DEMUX_CH_TYPE_PCR:
    case DEMUX_CH_TYPE_SECTION:
    case DEMUX_CH_TYPE_PES:
    case DEMUX_CH_TYPE_TS:
        {
            if ( ch->handle ){
                ret = HI_UNF_DMX_SetChannelPID(ch->handle, pid);
                if (HI_SUCCESS != ret) {
                    LOGE("HI_UNF_DMX_SetChannelPID,Ret=%x",ret);
                    return -1;
                }
                ret = HI_UNF_DMX_OpenChannel(ch->handle);
                if (HI_SUCCESS != ret) {
                    LOGE("HI_UNF_DMX_OpenChannel,Ret=%x",ret);
                    return -1;
                }
            }
            ch->pid = pid;
            break;
        }
    case DEMUX_CH_TYPE_PVR:
    case DEMUX_CH_TYPE_PVR_SLAVE:
    default:
        {
            LOGE("unsupport channel type=%d", ch->type);
            return -1;
        }
    }
    return 0;
}

static int demux_stop_channel(struct demux_device_t *dev, int handle, int chid)
{
    int ret;
    mdemux_t *dm = (mdemux_t*)handle;
    LOGD("stop_channel, handle=%x, chid=%x", handle, chid);
    channel_t *ch = &dm->channel[chid-1];


    switch (ch->type)
    {
    case DEMUX_CH_TYPE_AUDIO:
    case DEMUX_CH_TYPE_VIDEO:
    case DEMUX_CH_TYPE_PCR:
    case DEMUX_CH_TYPE_SECTION:
    case DEMUX_CH_TYPE_PES:
    case DEMUX_CH_TYPE_TS:
        {
            descrambler_t *ds = (descrambler_t*)ch->dsm;
            if ( ds ){
                ret = HI_UNF_DMX_DetachDescrambler(ds->handle, ch->handle);
                if (HI_SUCCESS != ret) {
                    LOGE("HI_UNF_DMX_DetachDescrambler,Ret=%x",ret);
                    return -1;
                }
                //音视频通道是解码器创建的，不能在这close
                if ( ch->type == DEMUX_CH_TYPE_AUDIO||ch->type==DEMUX_CH_TYPE_VIDEO)
                    ch->handle = 0;
                ch->dsm = 0;
                ds->attach = 0;
                ds->chi = 0;
            }
            if ( ch->handle ){
                ret = HI_UNF_DMX_CloseChannel(ch->handle);
                if (HI_SUCCESS != ret) {
                    LOGE("HI_UNF_DMX_CloseChannel,Ret=%x",ret);
                    return -1;
                }
            }
            break;
        }
    case DEMUX_CH_TYPE_PVR:
    case DEMUX_CH_TYPE_PVR_SLAVE:
    default:
        {
            LOGE("unsupport channel type=%d", ch->type);
            return -1;
        }
    }
    ch->pid = 0;
    return 0;
}

static int demux_add_filter(struct demux_device_t *dev, int handle, int chid)
{
    LOGD("add_filter, handle=%x, chid=%x", handle, chid);
    int ret, i;
    mdemux_t *dm = (mdemux_t*)handle;
    channel_t *ch = &dm->channel[chid-1];
    filter_t *ft;


    if ( DEMUX_CH_TYPE_SECTION != ch->type ){
        LOGE("channel type %d can not add filter", ch->type);
        return -1;
    }

    for (i=0; i<DEMUX_MAX_CHANNEL; i++)
    {
        ft = &dm->filter[i];
        if ( 0==ft->id ){
            HI_UNF_DMX_FILTER_ATTR_S stFilterAttr;
            memset(&stFilterAttr, 0, sizeof(HI_UNF_DMX_FILTER_ATTR_S));
            ret = HI_UNF_DMX_CreateFilter(dm->dmxid, &stFilterAttr, &ft->handle);
            if (HI_SUCCESS != ret) {
                LOGE("HI_UNF_DMX_CloseChannel,Ret=%x",ret);
                return -1;
            }
            ft->id = i+1;
            LOGD("add_filter, %d,%x", i+1, ft);
            return ft->id;//(int)ft;
        }
    }

    LOGE("too many filter");
    return -1;
}

static int demux_remove_filter(struct demux_device_t *dev, int handle, int chid, int fid)
{
    LOGD("remove_filter, handle=%x, chid=%x, fid=%x", handle, chid, fid);
    int ret;
    mdemux_t *dm = (mdemux_t*)handle;
    channel_t *ch = &dm->channel[chid-1];
    filter_t *ft = &dm->filter[fid-1];


    if ( DEMUX_CH_TYPE_SECTION != ch->type ){
        LOGE("channel type %d error", ch->type);
        return -1;
    }

    if ( ft->handle ){

        ret = HI_UNF_DMX_DestroyFilter(ft->handle);
        if (HI_SUCCESS != ret) {
            LOGE("HI_UNF_DMX_DestroyFilter,Ret=%x",ret);
            return -1;
        }
    }
    ft->handle = 0;
    ft->id = 0;
    return 0;
}

static int demux_start_filter(struct demux_device_t *dev, int handle, int chid, int fid,
		struct demux_filter_param_t*p)
{
    LOGD("start_filter, handle=%x, chid=%x, fid=%x, p=%x", handle, chid, fid, p);
    int ret, i;
    mdemux_t *dm = (mdemux_t*)handle;
    channel_t *ch = &dm->channel[chid-1];
    filter_t *ft = &dm->filter[fid-1];
    HI_UNF_DMX_FILTER_ATTR_S stFilterAttr;


    if ( DEMUX_CH_TYPE_SECTION != ch->type ){
        LOGE("channel type %d error", ch->type);
        return -1;
    }

	LOGD("demux_filter_param_t. %d\ncoef\tmask\texcl", p->depth);
	for (i = 0; i < p->depth; i++) {
		LOGD("0x%02x\t0x%02x\t0x%02x", p->coef[i], p->mask[i], p->excl[i]);
	}
	memset(&stFilterAttr, 0, sizeof(stFilterAttr));
	for (i = 0; i < DMX_FILTER_MAX_DEPTH && i<p->depth; i++) {
		if (i == 0) {
			stFilterAttr.au8Match[i] = p->coef[i];
			stFilterAttr.au8Mask[i]  = ~p->mask[i];
			stFilterAttr.au8Negate[i]= p->excl[i];
		}
		if (i == 1 || i == 2)
			continue;

		if (i >= 3) {
			stFilterAttr.au8Match[i-2] = p->coef[i];
			stFilterAttr.au8Mask[i-2]  = ~p->mask[i];
			stFilterAttr.au8Negate[i-2]= p->excl[i];
		}
	}
    stFilterAttr.u32FilterDepth = (p->depth<3)?1:p->depth-2;

	ret = HI_UNF_DMX_SetFilterAttr(ft->handle, &stFilterAttr);
    if (HI_SUCCESS != ret) {
        LOGE("HI_UNF_DMX_SetFilterAttr,Ret=%x",ret);
        return -1;
    }

	ret = HI_UNF_DMX_AttachFilter(ft->handle, ch->handle);
    if (HI_SUCCESS != ret) {
        LOGE("HI_UNF_DMX_AttachFilter,Ret=%x",ret);
        return -1;
    }
    LOGD("start_filter, %d", stFilterAttr.u32FilterDepth);
	return 0;
}
static int demux_stop_filter(struct demux_device_t *dev, int handle, int chid, int fid)
{
    LOGD("stop_filter, handle=%x, chid=%x, fid=%x", handle, chid, fid);

    int ret;
    mdemux_t *dm = (mdemux_t*)handle;
    channel_t *ch = &dm->channel[chid-1];
    filter_t *ft = &dm->filter[fid-1];

    if ( DEMUX_CH_TYPE_SECTION != ch->type ){
        LOGE("channel type %d error", ch->type);
        return -1;
    }

    ret = HI_UNF_DMX_DetachFilter(ft->handle, ch->handle);
    if (HI_SUCCESS != ret) {
        LOGE("HI_UNF_DMX_DetachFilter,Ret=%x",ret);
        return -1;
    }

	return 0;
}

/**@function
 *			获取有新数据到达的所有通道中数据情况
 *@param dev
 *			demux设备控制句柄
 *@param handle
 *			demux设备打开句柄，实现为转换后就是dmxId值
 *@param e
 *			接收事件的数组起始地址
 *@param size
 *			接收事件的数组大小
 *@param timeout
 *			进行poll数据通道时超时时间，单位:ms,-1(0xffffffff)表示无限等待
 *@return
 *			返回有多少个数据通道数据到达的总数
 */
static int demux_poll_channels(struct demux_device_t *dev, int handle, unsigned char*idxs, int size)
{
    int ret, i, j, c=2;
    HI_HANDLE ChannelHandle[DEMUX_MAX_CHANNEL];
    HI_U32 u32ChNum = DEMUX_MAX_CHANNEL, TimeOutMs = 0;
    mdemux_t *dm = (mdemux_t*)handle;
    channel_t *ch;

    LOGI("poll_channels, handle=%x, idxs=%x, size=%d", handle, idxs, size);
#if 1
    c=0;
    ret = HI_UNF_DMX_GetDataHandle(ChannelHandle, &u32ChNum, TimeOutMs);
    if (HI_SUCCESS == ret) {
        for (i=0; i<u32ChNum; i++){
            for (j=0; j<DEMUX_MAX_CHANNEL; j++){
                ch = &dm->channel[j];
                if ( ChannelHandle[i]==ch->handle ){
                    if (c<size)
                        idxs[c++] = ch->id;
                    break;
                }
            }
        }
    }
    else
        LOGI("HI_UNF_DMX_GetDataHandle,Ret=%x",ret);
    LOGI("poll_channels, return %d", c);
#endif    
    return c;
}

int64_t sys_utc_mtime() {
	struct timeval tm;
	gettimeofday(&tm, NULL);
	return ((int64_t) tm.tv_sec) * 1000 + ((int64_t) tm.tv_usec) / 1000;
}

static int demux_acquire_channel_data(struct demux_device_t *dev, int handle, int chi,
		unsigned char**p, int*pack_size)
{
    LOGI("acquire_channel_data, handle=%x, chi=%d, p=%x, pack_size=%x", handle, chi, p, pack_size);

    int ret, i ,j;
    mdemux_t *dm = (mdemux_t*)handle;
    channel_t *ch = &dm->channel[chi-1];
    static HI_UNF_DMX_DATA_S stBuf[DEMUX_MAX_SECTION];

    if ( DEMUX_CH_TYPE_PVR==ch->type ){
        //todo..
    }
    else{
        HI_U32 u32AcquireNum=DEMUX_MAX_SECTION, u32AcquiredNum, u32TimeOutMs=0;
        ret = HI_UNF_DMX_AcquireBuf(ch->handle, u32AcquireNum, &u32AcquiredNum, stBuf, 0);
        if (HI_SUCCESS != ret || 0==u32AcquiredNum) {
            LOGE("HI_UNF_DMX_AcquireBuf, Ret=%x",ret);
            return -1;
        }
        for ( i=0; i<u32AcquiredNum; i++){
            memcpy(ch->pbuf+ch->buflen, stBuf[i].pu8Data, stBuf[i].u32Size);
            ch->buflen += stBuf[i].u32Size;
			if(stBuf[i].pu8Data[0] == 0x80 || stBuf[i].pu8Data[0] == 0x81)
			{
				LOGE("zfs ecm data:%02x %02x %02x %02x %02x %02x %02x %02x ",stBuf[i].pu8Data[0],stBuf[i].pu8Data[1],stBuf[i].pu8Data[2],stBuf[i].pu8Data[3],stBuf[i].pu8Data[4],stBuf[i].pu8Data[5],stBuf[i].pu8Data[6],stBuf[i].pu8Data[7]);					
			}
        }
        ret = HI_UNF_DMX_ReleaseBuf(ch->handle, u32AcquiredNum, stBuf);
        if (HI_SUCCESS != ret ) {
            LOGE("HI_UNF_DMX_ReleaseBuf, Ret=%x",ret);
            return -1;
        }
        *p = ch->pbuf;
        *pack_size = ch->buflen;
        return ch->buflen;
    }
    return 0;
}

static int demux_release_channel_data(struct demux_device_t *dev, int handle, int chi, int len)
{
    LOGV("release_channel_data, handle=%x, chi=%d, len=%d", handle, chi, len);
    int ret, i;
    mdemux_t *dm = (mdemux_t*)handle;
    channel_t *ch = &dm->channel[chi-1];

    if (DEMUX_CH_TYPE_PVR == ch->type){
    }
    else{
        if ( ch->buflen!=len ){
            LOGE("release len error, to be %d, now %d", ch->buflen, len);
            return -1;
        }
        ch->buflen -= len;
    }
    return 0;
}

static int64_t mLastCalcTime=0;
static int64_t mPushCalcSize=0;
char filename[100];
FILE * rec_fp=NULL ;
static int rec_index=0;
int demux_push_data(struct demux_device_t *dev, int handle, unsigned char*buf, int len)
{
    LOGD("push_data, handle=%x, buf=%x, len=%d", handle, buf, len);
    int ret;
    mdemux_t *dm = (mdemux_t*)handle;
    HI_UNF_STREAM_BUF_S stData;

	if ( buf == NULL || len <= 0) {
		LOGE("%s: %d  %s buf is NULL", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
    if ( 0==dm->ts_buf ){
		LOGE("ts null, demux mode %d", dm->mode);
		return -1;
    }

	ret = HI_UNF_DMX_GetTSBuffer(dm->ts_buf, len, &stData, 0);
    if (HI_SUCCESS != ret ) {
        LOGE("HI_UNF_DMX_GetTSBuffer, Ret=%x",ret);
        return -1;
    }

	memcpy(stData.pu8Data, buf, stData.u32Size);
	ret = HI_UNF_DMX_PutTSBuffer(dm->ts_buf, stData.u32Size);
    if (HI_SUCCESS != ret ) {
        LOGE("HI_UNF_DMX_PutTSBuffer, Ret=%x",ret);
        return -1;
    }
    if(stData.u32Size != len)
        LOGD("demux_push_data handle=%x, reqLen=%d len =%d ", handle, stData.u32Size, len);
    int64_t timeNow;
    timeNow = sys_utc_mtime();
    mPushCalcSize += len;
    if ( timeNow-mLastCalcTime >= 1000 ){
        int bytes = mPushCalcSize*1000/(timeNow-mLastCalcTime);
        LOGD("calcspeed:%d byte/s", bytes);
        mLastCalcTime = timeNow;
        mPushCalcSize = 0;
    }

    char propvalue[PROPERTY_VALUE_MAX];
    property_get("sys.plattv.savets", propvalue, "");
	if(propvalue[0] && len > 0 ) {
		if(rec_fp == NULL) {
            //strcat(propvalue, "/dmx.ts");
            sprintf(propvalue+strlen(propvalue),"/dmx_%d.ts",rec_index);
			rec_fp = fopen(propvalue, "wb");
			if(rec_fp == NULL)
			    LOGE("create %S failed ", propvalue);
		}
        if( rec_fp )
		    fwrite(buf, 1, len, rec_fp);
	}
	LOGD("push_data,push len=%d",stData.u32Size);
	return stData.u32Size;
}

static int demux_clear_data(struct demux_device_t *dev, int handle)
{
    LOGD("clear_data, handle=%x", handle);
    int ret;
    mdemux_t *dm = (mdemux_t*)handle;

    if ( dm->ts_buf ){
	    ret = HI_UNF_DMX_ResetTSBuffer(dm->ts_buf);
        if (HI_SUCCESS != ret ) {
            LOGE("HI_UNF_DMX_ResetTSBuffer, Ret=%x",ret);
            return -1;
        }
    }
    char propvalue[PROPERTY_VALUE_MAX];
    property_get("sys.plattv.savets", propvalue, "");
	if(propvalue[0]) {
		if(rec_fp != NULL) {
			rec_index++;
			fclose(rec_fp);
			rec_fp=NULL;
			LOGD("clear_data, rec_index=%d", rec_index);
		}
	}	
	return 0;
}

 int demux_attach_descrambler(struct demux_device_t *dev, int handle, int chi)
{
    LOGD("attach_descrambler, handle=%x, chi=%d", handle, chi);
    int ret, i;
    mdemux_t *dm = (mdemux_t*)handle;
    descrambler_t *ds;

    for (i=0; i<DEMUX_MAX_CHANNEL; i++){
        ds = &dm->desc[i];
        if ( 0 == ds->id ){
            //u32DmxId does not matter
            ret = HI_UNF_DMX_CreateDescrambler(0/*dm->dmxid*/, &ds->handle);
            if (HI_SUCCESS != ret ) {
                LOGE("HI_UNF_DMX_CreateDescrambler, Ret=%x",ret);
                return -1;
            }
            ds->id = i+1;
            ds->chi=chi;
            ds->attach = 0;
            LOGD("attach_descrambler, %d,%x", i+1, ds);
            return (int)ds;
        }
    }

    LOGE("too many desc");
    return -1;
}

int demux_detach_descrambler(struct demux_device_t *dev, int handle, int chi,int id)
{
    LOGD("detach_descrambler, handle=%x, chi=%d, id=%x", handle, chi, id);
    int ret;
    mdemux_t *dm = (mdemux_t*)handle;
    channel_t *ch = &dm->channel[chi-1];
    descrambler_t *ds = (descrambler_t *)id;;
    HI_HANDLE hChannel;

    if ( ds->attach ){
        //if desc attach more than one channel
        //todo...
        ret = HI_UNF_DMX_GetChannelHandle (dm->dmxid, ch->pid, &hChannel);
        if (HI_SUCCESS != ret ) {
            LOGW("HI_UNF_DMX_GetChannelHandle, Ret=%x",ret);
        }
        //else
        {
            ret = HI_UNF_DMX_DetachDescrambler(ds->handle, ch->handle);
            if (HI_SUCCESS != ret ) {
                LOGE("HI_UNF_DMX_DetachDescrambler, Ret=%x",ret);
                return -1;
            }
        }
        ds->attach = 0;
        ch->handle = 0;
        ch->dsm = 0;
    }
    ret = HI_UNF_DMX_DestroyDescrambler (ds->handle);
    if (HI_SUCCESS != ret ) {
        LOGE("HI_UNF_DMX_DestroyDescrambler, Ret=%x",ret);
        return -1;
    }
    ds->id = 0;
    ds->handle = 0;
    ds->chi = 0;

    return 0;
}

static int demux_set_descrambler_pid(struct demux_device_t *dev, int handle,int chi,int id,
        int pid)
{
    LOGD("set_descrambler_pid, handle=%x, chi=%d, id=%x, pid=%x", handle, chi, id, pid);
    int ret;
    mdemux_t *dm = (mdemux_t*)handle;
    channel_t *ch = &dm->channel[chi-1];
    descrambler_t *ds = (descrambler_t *)id;;
    channel_t *ch0 = &dm->channel[ds->chi-1];
    HI_HANDLE hChannel;

    if (ch->pid != pid){
        LOGW("what is this, channelpid=%x, pid=%x", ch->pid, pid);
    }
    //if (ch->pid != ch0->pid ){
        //LOGD("change desc's channel, form pid %x to %x", ch0->pid, ch->pid);
    if ( ds->attach ){ //如果之前已有绑定，先解除
        //ret = HI_UNF_DMX_GetChannelHandle (dm->dmxid, ch->pid, &hChannel);
        //if (HI_SUCCESS != ret ) {
        //    LOGW("HI_UNF_DMX_GetChannelHandle, Ret=%x",ret);
        //}
        //else
        {
            ret = HI_UNF_DMX_DetachDescrambler(ds->handle, ch0->handle);
            if (HI_SUCCESS != ret ) {
                LOGE("HI_UNF_DMX_DetachDescrambler, Ret=%x",ret);
                return -1;
            }
        }
        ds->attach = 0;
        ch0->handle = 0;
    }
    ds->chi = chi;
    //}
    return 0;
}

static int demux_set_descrambler_key(struct demux_device_t *dev, int handle, int chi, int id,
        int odd, const unsigned char *keyValue, int len)
{
    LOGD("set_descrambler_key, handle=%x, chi=%d, id=%x, idd=%d, keyValue=%x, len=%d", handle, chi, id, odd, keyValue, len);
    int ret;
    mdemux_t *dm = (mdemux_t*)handle;
    channel_t *ch = &dm->channel[chi-1];
    descrambler_t *ds = (descrambler_t *)id;;
    HI_HANDLE hChannel;
    HI_U8 u8key[16];

    if ( 0==ds->attach ){
        ret = HI_UNF_DMX_GetChannelHandle (dm->dmxid, ch->pid, &hChannel);
        if (HI_SUCCESS != ret ) {
            LOGE("HI_UNF_DMX_GetChannelHandle, Ret=%x",ret);
        }
        else{
            ret = HI_UNF_DMX_AttachDescrambler(ds->handle, hChannel);
            if (HI_SUCCESS != ret ) {
                LOGE("HI_UNF_DMX_AttachDescrambler, Ret=%x",ret);
                return -1;
            }
			LOGW("HI_UNF_DMX_AttachDescrambler, Ret=%x",ret);
            ch->handle = hChannel;
            ch->dsm = (int)ds;  //通道记录绑定的解扰器，在通道停止前先解除绑定
            ds->attach = 1;
        }
    }

    memset(u8key, 0, sizeof(u8key));
    memcpy(u8key, keyValue, len);
    if (odd == 0)//even
    {
    	LOGD("set_descrambler_key, even:%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",\
			u8key[0],u8key[1],u8key[2],u8key[3],u8key[4],u8key[5],u8key[6],u8key[7],u8key[8],u8key[9],u8key[10],u8key[11],u8key[12],u8key[13],u8key[14],u8key[15]);
        ret = HI_UNF_DMX_SetDescramblerEvenKey(ds->handle, u8key);
        if (HI_SUCCESS != ret ) {
            LOGE("HI_UNF_DMX_SetDescramblerEvenKey, Ret=%x",ret);
            return -1;
        }
    } else {
    	LOGD("set_descrambler_key, odd:%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",\
			u8key[0],u8key[1],u8key[2],u8key[3],u8key[4],u8key[5],u8key[6],u8key[7],u8key[8],u8key[9],u8key[10],u8key[11],u8key[12],u8key[13],u8key[14],u8key[15]);
        ret = HI_UNF_DMX_SetDescramblerOddKey(ds->handle, u8key);
        if (HI_SUCCESS != ret ) {
            LOGE("HI_UNF_DMX_SetDescramblerOddKey, Ret=%x",ret);
            return -1;
        }
    }

    return 0;
}

static int demux_ioctl(struct demux_device_t *dev, int handle, int op, struct io_block_t*iob)
{
    LOGD("ioctl, handle=%x, op=%d, iob=%x", handle, op, iob);
	return 0;
}

static int demux_get_list(struct demux_module_t *module, struct demux_t const **list)
{
	int i = 0, j;

	demux_context_t *ctx = m_demux_ctx;

    if ( !ctx ){
        ctx = m_demux_ctx = (demux_context_t *)calloc(1, sizeof(demux_context_t));
        if ( !ctx ){
            LOGE("calloc ctx failed");
            return -1;
        }
    }
    if ( !ctx->list ){
    	ctx->list = (struct demux_t*) malloc(sizeof(struct demux_t) * DEMUX_MAX_NUM);
    	if ( !ctx->list ) {
            LOGE("calloc list failed");
    		return -1;
    	}
        for (i = 0; i < DEMUX_MAX_NUM; i++) {
            ctx->demux[i].id = i;
            ctx->demux[i].dmxid = (i==0)?0:4;
            ctx->list[i].handle = (int)&ctx->demux[i];
            ctx->list[i].max_audio_channel_size = DEMUX_MAX_NUM;
            ctx->list[i].max_video_channel_size = DEMUX_MAX_NUM;
            ctx->list[i].max_pcr_channel_size = DEMUX_MAX_NUM;
            ctx->list[i].max_data_channel_size = DEMUX_MAX_CHANNEL;
            ctx->list[i].support_push_src = 1;
            ctx->list[i].support_tuner_src = 1;
            ctx->list[i].support_descrambling = 1;
    		LOGD("get_list, %d,%x", i, ctx->list[i].handle);
        }
    }

    (*list) = ctx->list;
	LOGD("get_list num=%d", DEMUX_MAX_NUM);

	return DEMUX_MAX_NUM;
}

/*****************************************************************************/

static int close_demux(struct hw_device_t *dev)
{
	int ret = 0, i;
	struct demux_context_t* ctx = (struct demux_context_t*) dev;

	LOGD("close");

	if (ctx == NULL) {
		LOGE("[close_demux] null ctx.\n");
		return -1;
	}
	if ( ctx->list ) {
		free(ctx->list);
		ctx->list = NULL;
	}
	if ( ctx ) {
		free(ctx);
		ctx = NULL;
	}
    m_demux_ctx = NULL;
    HI_UNF_DMX_DeInit();

	return ret;
}

static int open_demux(const struct hw_module_t* module, const char* name, struct hw_device_t** device)
{
	int ret = 0, i;
	demux_context_t *ctx = m_demux_ctx;

	LOGD("open_demux");

	if (!strcmp(name, DEMUX_HARDWARE_DEMUX0)) {
        if ( !ctx ){
            ctx = m_demux_ctx = (demux_context_t *)calloc(1, sizeof(demux_context_t));
            if ( !ctx ){
                LOGE("calloc ctx failed");
                return -1;
            }
        }

		ctx->device.common.tag = HARDWARE_DEVICE_TAG;
		ctx->device.common.version = 1;
		ctx->device.common.module = const_cast<hw_module_t*> (module);
		ctx->device.common.close = close_demux;

		ctx->device.bind_tuner_src = demux_bind_tuner;
		ctx->device.bind_push_src = demux_bind_data;
		ctx->device.unbind = demux_unbind;
		ctx->device.clear_data = demux_clear_data;
		ctx->device.push_data = demux_push_data;
		ctx->device.ioctl = demux_ioctl;

		ctx->device.create_channel = demux_create_channel;
		ctx->device.start_channel = demux_start_channel;
		ctx->device.stop_channel = demux_stop_channel;
		ctx->device.destroy_channel = demux_destroy_channel;
		ctx->device.poll_channels = demux_poll_channels;
		ctx->device.acquire_channel_data = demux_acquire_channel_data;
		ctx->device.release_channel_data = demux_release_channel_data;

		ctx->device.add_filter = demux_add_filter;
		ctx->device.remove_filter = demux_remove_filter;
		ctx->device.start_filter = demux_start_filter;
		ctx->device.stop_filter = demux_stop_filter;

		ctx->device.attach_descrambler = demux_attach_descrambler;
		ctx->device.set_descrambler_pid = demux_set_descrambler_pid;
		ctx->device.detach_descrambler = demux_detach_descrambler;
		ctx->device.set_descrambler_key = demux_set_descrambler_key;

		ret = HI_UNF_DMX_Init();
		if (ret != HI_SUCCESS) {
			LOGE("HI_UNF_DMX_Init,Ret=%x", ret);
			return -1;
		}

		if (ret == 0) {
			*device = &ctx->device.common;
		} else {
			close_demux(&ctx->device.common);
		}

		LOGI("[open_demux] open succ!\n");
		return 0;
	} else {
		LOGE("[open_demux] error name.\n");
		ret = -1;
	}
    return ret;
}

