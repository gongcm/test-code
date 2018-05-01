#define LOG_TAG "[plattv]transcode"

#include <cutils/log.h>

#include "hi_unf_demux.h"
//#include <linux/msm_mdp.h>
#include <linux/fb.h>

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <linux/android_pmem.h>
#include <hi_common.h>
#include <stdlib.h>
#include <cutils/properties.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <assert.h>

#include <hi_protocol.h>
#include <hi_muxer.h>
#include <hi_transcoder.h>
#include <hi_proto_intf.h>
#include <hi_type.h>
#include <hi_transcoder.h>
#include <hi_muxer.h>
#include <hi_protocol.h>
#include <hi_unf_sound.h>
#include <hi_unf_disp.h>
#include <hi_unf_demux.h>
//#include <hi_adp_audio.h>
#include <hi_adp_hdmi.h>
#include <hi_adp_mpi.h>
#include <hi_unf_venc.h>
#include <hi_unf_aenc.h>

#include <hardware/transcode.h>

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



#define TRSCD_MAX_NUM         	(1)
#define PLAY_DMX_ID 			(1)
#define PFT_BUF_LEN                      512*1024
#define HISI_TS_BUFF_SIZE         4194280
#define DEMUX_PUSH_DATA 			(1048476)  // 188 倍数
#define DEMUX_MIN_THRESHOLD_VALUES 	(2 *1024*1024)
int open_transcode (const struct hw_module_t* module, const char* name,struct hw_device_t** device);
int transcode_get_list(struct transcode_module_t *module, struct transcode_t const **list);

static struct hw_module_methods_t transcode_module_methods = {
	open: open_transcode
};

struct transcode_module_t HAL_MODULE_INFO_SYM = {
	common: {
		tag: HARDWARE_MODULE_TAG,
		version_major: 1,
		version_minor: 0,
		id: TRANSCODE_HARDWARE_MODULE_ID,
		name: "transcode Module",
		author: "iPanel 2014, Inc.",
		methods: &transcode_module_methods,
		dso: NULL,
		reserved: {0}
	},
	get_transcode_list:transcode_get_list,
};

typedef struct Transcode_handle
{
	HI_HANDLE hTranscoderHandle;
	HI_HANDLE hProtocolHandle;
	HI_HANDLE hMuxerHandle;
}Transcode_handle;

Transcode_handle psthandles[TRSCD_MAX_NUM];
typedef struct transcoder_context_t {
	struct transcode_device_t device;
	//Transcode_handle psthandles[TRSCD_MAX_NUM];

	//...
} demux_context_t;

typedef struct stSampleLocalInfo_S
{
    HI_CHAR pCharLocalPath[256];
    HI_CHAR pCharAudioPath[256];
    HI_HANDLE hTranHandle;
    pthread_t processPthreadID;
    Transcoder_MuxerType muxerType;
    HI_HANDLE hMuxerHandle;
    HI_HANDLE hReadHandle;
}SampleLocalInfo_S;

typedef struct STREAM_PARAM{
	int audio_pid_src;
	int video_pid_src;
	int video_type_dst;
	int audio_type_dst;
	int video_type_src;
	int audio_type_src;
	int video_width;
	int video_hight;
	int fps;
	int bps;
	
}STREAM_PARAM_T;

/* 检查指针是否有效 */
#define TRSC_CHECK_VALID_POINTER(ptr) \
do{ if(ptr==NULL){                     \
		ALOGE("[haldmx]DEMUX:invalid pointer(%s)\n",__FUNCTION__);\
		return -1; \
	}                   \
}while(0)

#define TRSC_CHECK_VALID_ID(u32TrscId)\
do{ if((u32TrscId<0)||(u32TrscId>=TRSCD_MAX_NUM )){   \
		ALOGE("[haldmx]DEMUX:invalid demux id(%s).\n",__FUNCTION__);\
		return -1; \
	}                   \
}while(0)

static Transcoder_HandleAttr stTranHandleAttr ;
static SampleLocalInfo_S s_SampleLocalInfo;
static HI_HANDLE g_TsBuf;
static pthread_mutex_t g_TsMutex;
static pthread_mutex_t g_BufMutex;

static HI_HANDLE hAvplay;
static HI_BOOL bFirstFlag = HI_FALSE;
static HI_BOOL rePraFlag = HI_FALSE;
	
#define TRANSCODER_VIRTUAL_BASE_HANDLE	(0x200)
struct transcode_t* mtranscode_list = NULL;
static HI_S32 m_max_transcode = 0;
HI_CHAR *pstMuxerBuffer = HI_NULL;
static HI_BOOL push_data_flag = HI_FALSE;
static hiPMT_COMPACT_TBL_S g_pProgTbl[1];
static PMT_COMPACT_PROG mproginfo[1];
STREAM_PARAM_T stream_param[TRSCD_MAX_NUM];

static int transfer_palyer_param(STREAM_PARAM_T* fparam, hiPMT_COMPACT_TBL_S* toparam);

static HI_S32 transcoder_handleToDriver(HI_S32 handle)
{
	return handle - TRANSCODER_VIRTUAL_BASE_HANDLE;
}


static HI_S32 Sample_Protocol_Create(HI_HANDLE* hProtocolHandle,const ProtocolAttr* pstTranAttr)
{
    HI_S32 s32Ret;

    if(pstTranAttr->u32ProtocolType  != TRANSCODER_PROTOCOL_SAMPLE)
    {
        return HI_FAILURE;
    }

    memset(&s_SampleLocalInfo,0,sizeof(ProtocolAttr));
    s_SampleLocalInfo.muxerType = TRANSCODER_MUTEX_TS;

    if(TRANSCODER_MUTEX_FLV == s_SampleLocalInfo.muxerType)
    {
        strcpy(s_SampleLocalInfo.pCharLocalPath,"/local.flv");
    }
    else if(TRANSCODER_MUTEX_TS == s_SampleLocalInfo.muxerType)
    {
        strcpy(s_SampleLocalInfo.pCharLocalPath,"/local.ts");
    }
    else if(TRANSCODER_MUTEX_ES == s_SampleLocalInfo.muxerType)
    {
        strcpy(s_SampleLocalInfo.pCharLocalPath,"/local.h264");
        strcpy(s_SampleLocalInfo.pCharAudioPath,"/local.aac");
        s32Ret = HI_Muxer_RegistMuxer("libmuxer_es.so");
        if(HI_SUCCESS != s32Ret)
        {
            ALOGE("[haltrsc]HI_Muxer_RegistMuxer error!!\n");
            return HI_NULL;
        }
    }
    *hProtocolHandle = ((HI_HANDLE)(&s_SampleLocalInfo));
    return HI_SUCCESS;
}

static HI_S32 Sample_Protocol_Destroy(HI_HANDLE hProtocolHandle)
{
    HI_S32 s32Ret;
    SampleLocalInfo_S* pSamLocalInfo = HI_NULL;
    pSamLocalInfo = (SampleLocalInfo_S*)hProtocolHandle;

    s32Ret = HI_Muxer_Destroy(pSamLocalInfo->hMuxerHandle);
    if(HI_SUCCESS != s32Ret)
    {
        ALOGE("[haltrsc]HI_Muxer_Destroy error!!!\n");
        return HI_FAILURE;
    }
    pSamLocalInfo->hMuxerHandle = HI_NULL;
    return HI_SUCCESS;
}

static HI_S32 Sample_Protocol_Regist(HI_HANDLE pProtocolHandle,HI_HANDLE hTranHandle)
{
    SampleLocalInfo_S *pstSampleInfo = (SampleLocalInfo_S*)pProtocolHandle;
    Transcoder_HandleAttr transcoder_Attr;
    Transcoder_MuxerAttr muxer_Attr;
    HI_S32 s32Ret;
    pstSampleInfo->hTranHandle = (HI_HANDLE)hTranHandle;
    s32Ret = HI_Transcoder_GetAttr(pstSampleInfo->hTranHandle,TRANSCODER_ALL_ATTR,&transcoder_Attr);
    if(HI_SUCCESS !=  s32Ret)
    {
        ALOGE("[haltrsc]Sample_Protocol_Regist HI_Transcoder_GetAttr error!!\n");
        return HI_FAILURE;
    }
    s32Ret = HI_Transcoder_RegisterHandle(pstSampleInfo->hTranHandle,&pstSampleInfo->hReadHandle);
    if(HI_SUCCESS !=  s32Ret)
    {
        ALOGE("[haltrsc]Sample_Protocol_Regist HI_Transcoder_RegisterHandle error!!\n");
        return HI_FAILURE;
    }
    ALOGE("[haltrsc]HI_Transcoder_RegisterHandle hTranHandle : %x,hReadHandle : %x\n",
                pstSampleInfo->hTranHandle,pstSampleInfo->hReadHandle);
    muxer_Attr.audioCodec = transcoder_Attr.audioCodec;
    muxer_Attr.s32AudioBandWidth = transcoder_Attr.s32AudioBandWidth;
    muxer_Attr.s32AudioChannels = transcoder_Attr.s32AudioChannels;
    muxer_Attr.s32AudioSampleRate = transcoder_Attr.s32AudioSampleRate;
    muxer_Attr.s32Height = transcoder_Attr.s32Height;
    muxer_Attr.u32VideoBitRate = transcoder_Attr.u32VideoBitRate;
    muxer_Attr.s32VideoFrameRate = transcoder_Attr.s32VideoFrameRate;
    muxer_Attr.s32Width = transcoder_Attr.s32Width;
    muxer_Attr.videoCodec = transcoder_Attr.videoCodec;
    s32Ret = HI_Muxer_Create(&pstSampleInfo->hMuxerHandle,pstSampleInfo->muxerType,&muxer_Attr);
    if(HI_SUCCESS != s32Ret)
    {
        ALOGE("[haltrsc]HI_Muxer_Create error!!!\n");
        return HI_FAILURE;
    }
    
    return HI_SUCCESS;
}

static HI_S32 Sample_Protocol_DeRegist(HI_HANDLE pProtocolHandle,HI_HANDLE hTranHandle)
{
    HI_S32 s32Ret;
    SampleLocalInfo_S *pstSampleInfo = (SampleLocalInfo_S*)pProtocolHandle;

    s32Ret = HI_Transcoder_Stop(pstSampleInfo->hTranHandle,HI_TRUE);
    if(HI_SUCCESS != s32Ret)
    {
        ALOGE("[haltrsc]HI_Transcoder_Stop error!!\n");
        return HI_FAILURE;
    }
    s32Ret = HI_Transcoder_DeRegisterHandle(pstSampleInfo->hReadHandle);
    if(HI_SUCCESS != s32Ret)
    {
        ALOGE("[haltrsc]HI_Transcoder_DeRegisterHandle error!!!\n");
        return HI_FAILURE;
    }
    pstSampleInfo->hReadHandle = HI_NULL;
    return HI_SUCCESS;
}

static HI_VOID Transcoder_SetDefaultAttr()
{    
	stTranHandleAttr.audioCodec = TRANSCODER_CODEC_ID_AACLC;
	stTranHandleAttr.videoCodec = TRANSCODER_CODEC_ID_H264;
	stTranHandleAttr.hSrc = hAvplay;
	stTranHandleAttr.pNodifySrcFunc = HI_NULL;
	stTranHandleAttr.s32AudioBandWidth = 16;
	stTranHandleAttr.s32AudioChannels = 2;
	stTranHandleAttr.s32AudioSampleRate = 48000;
	stTranHandleAttr.s32Height = 480;
	stTranHandleAttr.u32VideoBitRate = 512 * 1024;
	stTranHandleAttr.s32VideoFrameRate = 25;
	stTranHandleAttr.s32Width = 640;
	stTranHandleAttr.s32Gop = 10;
	stTranHandleAttr.SourceType = 0;
	stTranHandleAttr.bCycle = HI_FALSE;
}


static int trsc_get_capacity_list (struct transcode_device_t *dev, int handle,
			const struct transcode_list_t**list)
{
	return 0;
}

static int trsc_bind_demux_src(struct transcode_device_t *dev, int handle, struct demux_device_t *d_dev,
			int dmxHandle, int chi)
{
	return -1;
}

static int trsc_bind_demux_dst(struct transcode_device_t *dev, int handle, struct demux_device_t *d_dev,
			int dmxHandle, int chi)
{
	return -1;
}

static int trsc_unbind(struct transcode_device_t *dev, int handle)
{
	return -1;
}

static int trsc_bind_push_src(struct transcode_device_t *dev, int handle, int data_type)
{
	return -1;
}

FILE *fp = NULL;
static int push_reach = 0;
static int trsc_push_data(struct transcode_device_t *dev, int handle, void*p, int size)
{ 
	TRSC_CHECK_VALID_POINTER(p);

	HI_S32 ret = 0;
	HI_S32 reqLen = 0;
	HI_UNF_STREAM_BUF_S StreamBuf = {0};
	HI_UNF_DMX_TSBUF_STATUS_S tsStatus={0};
	//ALOGI("[haltrsc]call %s in.size = %d\n",__FUNCTION__,size); 
	pthread_mutex_lock(&g_TsMutex); 
	ret = HI_UNF_DMX_GetTSBufferStatus(g_TsBuf,&tsStatus);
	if(ret != HI_SUCCESS){
		ALOGE("[haltrsc][%d: %s] HI_UNF_DMX_GetTSBufferStatus fail ret=%0x\n", __LINE__, __FUNCTION__,ret);
		pthread_mutex_unlock(&g_TsMutex); 
		return 0;
	}    
	if ((tsStatus.u32BufSize - tsStatus.u32UsedSize) < DEMUX_MIN_THRESHOLD_VALUES) {
		LOGE("%s: %d  %s threshold value is reach", __FILE__, __LINE__, __FUNCTION__);
		pthread_mutex_unlock(&g_TsMutex);
		push_reach++;
		if(push_reach > 10){
			LOGE("trsc_push_data reach > 10");
			push_reach = 0;
			return -1;
		}
		sleep(0);
		return 0;
	}
	push_reach = 0;
	reqLen = (size < DEMUX_PUSH_DATA) ? size : DEMUX_PUSH_DATA;
	ret = HI_UNF_DMX_GetTSBuffer(g_TsBuf,reqLen, &StreamBuf,0);      
	if (ret != HI_SUCCESS )   
	{           
		ALOGE("[haltrsc] [%d:%s]HI_UNF_DMX_GetTSBuffer failed!ret = 0x%x ",__LINE__,__FUNCTION__,ret);
		pthread_mutex_unlock(&g_TsMutex); 
		return 0;
	}
	if (size <= StreamBuf.u32Size)
	{
		memcpy(StreamBuf.pu8Data,(unsigned char *)p,size);
		ret = HI_UNF_DMX_PutTSBuffer(g_TsBuf, size); 
		if (ret != HI_SUCCESS ) 
		{
			ALOGE("[haltrsc] [%d:%s]call HI_UNF_DMX_PutTSBuffer failed!ret = %d ",__LINE__,__FUNCTION__,ret);
		}
		ret = size;
	}
	else
	{
		memcpy(StreamBuf.pu8Data,(unsigned char *)p, StreamBuf.u32Size);
		ret = HI_UNF_DMX_PutTSBuffer(g_TsBuf, StreamBuf.u32Size); 
		if (ret != HI_SUCCESS ) 
		{
			ALOGE("[haltrsc] [%d:%s]call HI_UNF_DMX_PutTSBuffer failed!ret = %d ",__LINE__,__FUNCTION__,ret);
		}
		ret = StreamBuf.u32Size;
	}
	//ALOGI("[haltrsc]call %s out ret = %d.\n",__FUNCTION__,ret); 
	pthread_mutex_unlock(&g_TsMutex);
	if(rePraFlag)
	{
		push_data_flag = HI_TRUE;
		rePraFlag = HI_FALSE;
	}
	#if 0
	if(fp == NULL){
		fp = fopen("/mnt/sda/sda1/tv_test/ac3.ts","wb");
	}
	if(fp)
		fwrite(p,ret,1,fp);
	#endif
	return ret;
	
}

static int trsc_acquire_data(  struct transcode_device_t *dev, int handle, unsigned char **p,int *pack_size)
{ 
	SampleLocalInfo_S* pstSampleInfo;
	TRANSCODER_STRAM stTranStream;
	HI_S32 s32Ret,s32MuxerLen;
	HI_S32 header_len = 0;
	*pack_size =188;
//  ALOGI("[haltrsc]call %s in.\n",__FUNCTION__); 

	pstSampleInfo = (SampleLocalInfo_S*)&s_SampleLocalInfo;
 
	s32MuxerLen = PFT_BUF_LEN;
	
  	if(!pstSampleInfo->hMuxerHandle || !pstSampleInfo->hTranHandle)
  	{
     		ALOGE("[haltrsc]hMuxerHandle : %x,hTranHandle : %x\n",pstSampleInfo->hMuxerHandle,pstSampleInfo->hTranHandle);
     		return -1;
  	}
  	if(HI_FALSE == bFirstFlag)
  	{
		s32Ret = HI_Transcoder_Start(pstSampleInfo->hTranHandle,HI_TRUE);
		pthread_mutex_lock(&g_BufMutex); 
		if(HI_SUCCESS != s32Ret)
		{
				ALOGE("[haltrsc]sample_local_fileProcess HI_Transcoder_Start error!!\n");
				pthread_mutex_unlock(&g_BufMutex); 
			  	return -1;
		}
		s32Ret = HI_Muxer_GetHeader(pstSampleInfo->hMuxerHandle,pstMuxerBuffer,&s32MuxerLen);
		if(HI_SUCCESS != s32Ret)
		{
			ALOGE("[haltrsc]HI_Muxer_GetHeader error!!\n");
			pthread_mutex_unlock(&g_BufMutex); 
			return -1;
		}
		header_len = s32MuxerLen;
		bFirstFlag = HI_TRUE;
		*p = (unsigned char *)pstMuxerBuffer;
		pthread_mutex_unlock(&g_BufMutex); 
		return header_len;
		
  	}
	
	s32Ret = HI_Transcoder_ReadStream(pstSampleInfo->hReadHandle,&stTranStream);
	if(HI_SUCCESS != s32Ret)
	{
		//ALOGE("[haltrsc][daijy]HI_Transcoder_ReadStream error!!\n");
		//	  ALOGE("[haltrsc]HI_Transcoder_RegisterHandle hTranHandle : %x,hReadHandle : %x\n",
     //           pstSampleInfo->hTranHandle,pstSampleInfo->hReadHandle);
		return 0;
	}
    if(TRANSCODER_MUTEX_ES != s_SampleLocalInfo.muxerType)
    {
    	pthread_mutex_lock(&g_BufMutex); 
        s32Ret = HI_Muxer_DoMuxer(pstSampleInfo->hMuxerHandle,pstMuxerBuffer, &s32MuxerLen,&stTranStream);
    	if(HI_SUCCESS != s32Ret)
        {
          	ALOGE("[haltrsc]HI_Muxer_DoMuxer error!!\n");
			pthread_mutex_unlock(&g_BufMutex); 
			return -1;
   		}
		*p =(unsigned char *) pstMuxerBuffer;
		//ALOGE("[haltrsc]s32MuxerLen = %d\n",s32MuxerLen);
		pthread_mutex_unlock(&g_BufMutex); 
		return s32MuxerLen;
    }

	return 0;
}

static int trsc_start(struct transcode_device_t *dev, int handle)
{
	HI_HANDLE g_hTranHandle;
	HI_HANDLE hProtoHandle;
	HI_S32 s32Ret = 0;
	ProtocolAttr protoAttr; 
	HI_S32 i=0;
	HI_S32 transcoder_id = 0;
	ALOGE("[haltrsc]trsc_start in\n");
	memset(&stTranHandleAttr,0,sizeof(Transcoder_HandleAttr));  
	transcoder_id = transcoder_handleToDriver(handle);
	Transcoder_SetDefaultAttr();  
	s32Ret = HI_Transcoder_Create(&g_hTranHandle,&stTranHandleAttr);
	if(HI_SUCCESS != s32Ret) 
	{        
		ALOGE("[haltrsc]HI_Transcoder_Create error\n");  
		return -1;
	}   
	ALOGE("[haltrsc]HI_Transcoder_Create success\n");
	protoAttr.s32Port = 4098;  
	protoAttr.s32UserNum = 8;
	protoAttr.SourceHandle = hAvplay;  
	protoAttr.u32ProtocolType = TRANSCODER_PROTOCOL_SAMPLE; 
	ALOGE("[haltrsc]HI_Protocol_Create enter\n");
	s32Ret = Sample_Protocol_Create(&hProtoHandle,&protoAttr); 
	if(HI_SUCCESS != s32Ret)   
	{      
		ALOGE("[haltrsc]HI_Protocol_Create error\n"); 
		return -1;
	}    
	ALOGE("[haltrsc]HI_Protocol_Create success\n");  
	s32Ret = Sample_Protocol_Regist(hProtoHandle,g_hTranHandle);  
	if(HI_SUCCESS != s32Ret)   
	{       
		ALOGE("[haltrsc]HI_Protocol_RegistHandle error\n");  
		return -1;
	}  
	for(i=0;i<TRSCD_MAX_NUM;i++)
	{
		/*没有设备的open动作，做个假的好了*/
	    psthandles[transcoder_id].hProtocolHandle = hProtoHandle;
	    psthandles[transcoder_id].hTranscoderHandle = g_hTranHandle;
	}

	if (transfer_palyer_param( &(stream_param[transcoder_id]), g_pProgTbl) != 0)
	{
		ALOGE("[haltrsc] :%d %s  transfer_param failed!\n",__LINE__,__FUNCTION__);
		return -1;
	}
	/* set transcoder attr*/
	s32Ret = HI_Transcoder_GetAttr(psthandles[transcoder_id].hTranscoderHandle,TRANSCODER_ALL_ATTR, &stTranHandleAttr);
	if (0 != s32Ret)
	{
		ALOGE("[haltrsc] :%d %s  HI_Transcoder_GetAttr failed!\n",__LINE__,__FUNCTION__);
		return -1;
	}
#if 0
	stTranHandleAttr.s32Height = stream_param[transcoder_id].video_hight;
	stTranHandleAttr.u32VideoBitRate = stream_param[transcoder_id].bps;
	stTranHandleAttr.s32VideoFrameRate = stream_param[transcoder_id].fps;
	stTranHandleAttr.s32Width = stream_param[transcoder_id].video_width;
#endif
	stTranHandleAttr.s32Height = 480;
	stTranHandleAttr.u32VideoBitRate = 512 * 1024;
	stTranHandleAttr.s32VideoFrameRate = 25;
	stTranHandleAttr.s32Width = 640;
	
	s32Ret  = HI_Transcoder_SetAttr( psthandles[transcoder_id].hTranscoderHandle,TRANSCODER_ALL_ATTR,&stTranHandleAttr);
	if(HI_SUCCESS != s32Ret)
	{
		ALOGE("[haltrsc] :%d %s  HI_Transcoder_SetAttr failed\n",__LINE__,__FUNCTION__);
	}
	
	return 0;
}

static int trsc_stop(struct transcode_device_t *dev, int handle)
{
	HI_S32 transcoder_id = 0;
	HI_S32 ret = -1;
	struct transcoder_context_t* ctx = (struct transcoder_context_t*)dev;
	ALOGI("[haltrsc]call %s in.\n",__FUNCTION__); 
	TRSC_CHECK_VALID_POINTER(ctx);
	transcoder_id = transcoder_handleToDriver(handle);
	LOGD("[[trsc_stop]transcoder_id  = %d",transcoder_id);
	TRSC_CHECK_VALID_ID(transcoder_id);
	HI_S32 i = 0;
#if 0
	ret = HI_Transcoder_Stop(psthandles[transcoder_id].hTranscoderHandle,HI_TRUE);
	if(ret == HI_FAILURE)
	{
		ALOGE("[haltrsc]call %s failed.ret = %d\n",__FUNCTION__,ret);
		return -1;
	}
#endif	
	ret = Sample_Protocol_DeRegist(psthandles[transcoder_id].hProtocolHandle,psthandles[transcoder_id].hTranscoderHandle);
	if(HI_SUCCESS != ret )
	{
		ALOGE("[haltrsc]call %s failed.ret = %d\n",__FUNCTION__,ret);
		return -1;
	}
	
	ret = Sample_Protocol_Destroy(psthandles[transcoder_id].hProtocolHandle);
	if(HI_SUCCESS != ret )
	{
		ALOGE("[haltrsc]call %s failed.ret = %d\n",__FUNCTION__,ret);
		return -1;
	}
	
	ret = HI_Transcoder_Destroy(psthandles[transcoder_id].hTranscoderHandle); 
	if(HI_SUCCESS != ret)   
	{        
		ALOGE("[haltrsc]HI_Transcoder_Destroy failed\n"); 
	}
	bFirstFlag = HI_FALSE;
	for(i=0;i<TRSCD_MAX_NUM;i++)
	{
		/*没有设备的open动作，做个假的好了*/
	    psthandles[i].hProtocolHandle = 0;
	    psthandles[i].hTranscoderHandle = 0;
	}
	return 0;

}

static int trsc_release_data (struct transcode_device_t *dev, int handle, int size)
{
	//ALOGI("[haltrsc]call %s in.\n",__FUNCTION__); 
	return 0;
}

static int trsc_clear_data (struct transcode_device_t *dev, int handle)
{
	ALOGI("[haltrsc]call %s in.\n",__FUNCTION__); 

	
	pthread_mutex_lock(&g_BufMutex); 
	memset(pstMuxerBuffer,0,PFT_BUF_LEN);
	pthread_mutex_unlock(&g_BufMutex); 
	
	pthread_mutex_lock(&g_TsMutex); 
	HI_UNF_DMX_ResetTSBuffer(g_TsBuf);
	pthread_mutex_unlock(&g_TsMutex); 

	return 0;
}

static int utils_uri_get_int_param(const char*s, const char*name, int*value) {
	if ((s = strstr(s, name)) == NULL)
		return 0;
	s += strlen(name);

	int ret = 0;
	ret = sscanf(s, "=%d", value) ;
	ALOGD("utils_uri_get_int_param %s:%d\n",name,*value);
	
	return (ret == 1) ? 0 : -1;
}

/*transfer local parameters to player parameters*/
static int transfer_palyer_param(STREAM_PARAM_T* fparam, hiPMT_COMPACT_TBL_S* toparam)
{
	if ((NULL==fparam) || (NULL==toparam))
	{
		LOGD("[trsc]%s param is null" , __FUNCTION__);
		return -1;
	}
	
	/*fixed parameters*/
  	toparam->prog_num = 1;
	toparam->proginfo[0].ProgID = 1;
	toparam->proginfo[0].VElementNum = 1;
	toparam->proginfo[0].AElementNum = 2;
	toparam->proginfo[0].PmtPid = 4096;

	
	toparam->proginfo[0].PcrPid = fparam->video_pid_src;
	toparam->proginfo[0].VElementPid = fparam->video_pid_src;
	toparam->proginfo[0].AElementPid = fparam->audio_pid_src;

	/*set video format*/
	switch (fparam->video_type_src){ 
		/*just need to surport H264 for Dalian project, implement other cases up to needs*/
		case VIDEO_TYPE_H264:
			toparam->proginfo[0].VideoType = HI_UNF_VCODEC_TYPE_H264;
			break;
		case VIDEO_TYPE_MPEG1:
		case VIDEO_TYPE_MPEG2:
			toparam->proginfo[0].VideoType = HI_UNF_VCODEC_TYPE_MPEG2;
			break;
		case VIDEO_TYPE_MPEG4:
			toparam->proginfo[0].VideoType = HI_UNF_VCODEC_TYPE_MPEG4;
			break;
		case VIDEO_TYPE_H265:
			toparam->proginfo[0].VideoType = HI_UNF_VCODEC_TYPE_HEVC;
			break;
		default:
			ALOGE("[haltrsc] :%d %s  unsurported dst video format: %d\n",
					__LINE__,__FUNCTION__, fparam->video_type_src);
			return -1;
			break;
	}
	/*set audio format*/
	switch (fparam->audio_type_src){
		/*just need to surport AAC for Dalian project, implement other cases up to needs*/
		case AUDIO_TYPE_AAC:
			toparam->proginfo[0].AudioType = HA_AUDIO_ID_AAC;
			break;
		case AUDIO_TYPE_MPEG1:
		case AUDIO_TYPE_MEPG2:
			//toparam->proginfo[0].AudioType = HA_AUDIO_ID_MP2;
			toparam->proginfo[0].AudioType = HA_AUDIO_ID_MP3;
			break;
		case AUDIO_TYPE_AC3:
			toparam->proginfo[0].AudioType = HA_AUDIO_ID_DOLBY_PLUS;
			break;
		case AUDIO_TYPE_DTS:
			toparam->proginfo[0].AudioType = HA_AUDIO_ID_DTSPASSTHROUGH;
			break;
		case AUDIO_TYPE_PCM:
			toparam->proginfo[0].AudioType = HA_AUDIO_ID_PCM;
			break;
		default:
			ALOGE("[haltrsc] :%d %s  unsurported dst audio format: %d",
					__LINE__,__FUNCTION__, fparam->audio_type_src);
			return -1;
			break;
	}
	rePraFlag = HI_TRUE;
	return 0;
	
}

/* 	"transcode://program_number=%d
 *  &audio_stream_pid=257
 *  &video_stream_pid=256
 *  &video_format_src=video_mpeg2
 *  &audio_format_src=audio_mpeg2
 * 	&video_format_dest=video_h264
 *  &audio_format_dest=audio_aac
 *  &video_width=1280
 *  &video_height=720
 *  &video_fps=25
 *  &video_bps=4194304"
 */

static int trsc_set_param(struct transcode_device_t *dev, int handle, const char*param)
{
	HI_S32 transcoder_id = 0;
	HI_S32 ret = -1;
	struct transcoder_context_t* ctx = (struct transcoder_context_t*)dev;
	ALOGI("[haltrsc]call %s in.%s\n",__FUNCTION__,param); 
	TRSC_CHECK_VALID_POINTER(ctx);
	TRSC_CHECK_VALID_POINTER(param);
	transcoder_id = transcoder_handleToDriver(handle);
	LOGD("[[trsc_set_param]transcoder_id  = %d",transcoder_id);
	TRSC_CHECK_VALID_ID(transcoder_id);

	ret = utils_uri_get_int_param(param, "audio_stream_pid", &(stream_param[transcoder_id].audio_pid_src));
	ret |= utils_uri_get_int_param(param, "video_stream_pid", &(stream_param[transcoder_id].video_pid_src));
	ret |= utils_uri_get_int_param(param, "audio_format_src", &(stream_param[transcoder_id].audio_type_src));
	ret |= utils_uri_get_int_param(param, "video_format_src", &(stream_param[transcoder_id].video_type_src));
	ret |= utils_uri_get_int_param(param, "video_format_dest", &(stream_param[transcoder_id].video_type_dst));
	ret |= utils_uri_get_int_param(param, "audio_format_dest", &(stream_param[transcoder_id].audio_type_dst));
	ret |= utils_uri_get_int_param(param, "video_width", &(stream_param[transcoder_id].video_width));
	ret |= utils_uri_get_int_param(param, "video_height", &(stream_param[transcoder_id].video_hight));
	ret |= utils_uri_get_int_param(param, "video_fps", &(stream_param[transcoder_id].fps));
	ret |= utils_uri_get_int_param(param, "video_bps", &(stream_param[transcoder_id].bps));
	if (0 != ret)
	{
		ALOGE("[haltrsc] :%d %s  utils_uri_get_int_param failed!\n%s\n",__LINE__,__FUNCTION__, param);
		return -1;
	}
#if 0
	if (transfer_palyer_param( &(stream_param[transcoder_id]), g_pProgTbl) != 0)
	{
		ALOGE("[haltrsc] :%d %s  transfer_param failed!\n",__LINE__,__FUNCTION__);
		return -1;
	}
	/* set transcoder attr*/
	ret = HI_Transcoder_GetAttr(psthandles[transcoder_id].hTranscoderHandle,TRANSCODER_ALL_ATTR, &stTranHandleAttr);
	if (0 != ret)
	{
		ALOGE("[haltrsc] :%d %s  HI_Transcoder_GetAttr failed!\n%s\n",__LINE__,__FUNCTION__, param);
		return -1;
	}
#if 0
	stTranHandleAttr.s32Height = stream_param[transcoder_id].video_hight;
	stTranHandleAttr.u32VideoBitRate = stream_param[transcoder_id].bps;
	stTranHandleAttr.s32VideoFrameRate = stream_param[transcoder_id].fps;
	stTranHandleAttr.s32Width = stream_param[transcoder_id].video_width;
#endif
	stTranHandleAttr.s32Height = 480;
	stTranHandleAttr.u32VideoBitRate = 512 * 1024;
	stTranHandleAttr.s32VideoFrameRate = 25;
	stTranHandleAttr.s32Width = 640;
	
	ret  = HI_Transcoder_SetAttr( psthandles[transcoder_id].hTranscoderHandle,TRANSCODER_ALL_ATTR,&stTranHandleAttr);
	if(HI_SUCCESS != ret)
	{
		ALOGE("[haltrsc] :%d %s  HI_Transcoder_SetAttr failed\n",__LINE__,__FUNCTION__);
	}
#endif	
	return 0;
}

int transcode_get_list(struct transcode_module_t *module, struct transcode_t const **list)
{
	HI_S32 num = 0;
	HI_S32 i = 0;
	ALOGI("[haltrsc]call %s in.\n",__FUNCTION__); 
	if (mtranscode_list && m_max_transcode)
	{
		(*list) = (transcode_t *)mtranscode_list;
		return m_max_transcode;
	}

	num = TRSCD_MAX_NUM;
	if (0 == num)
	{
		ALOGE("[haltrsc] :%d %s  device number is ZERO!\n",__LINE__,__FUNCTION__);
		return -1;
	}
	
	mtranscode_list = (struct transcode_t *)malloc(sizeof(struct transcode_t) * num);
	if (NULL == mtranscode_list)
	{
		ALOGE("[haltrsc] :%d %s malloc tuner list fail.\n",__LINE__,__FUNCTION__);
		return -1;
	}

	for (i=0; i<num; i++)
	{
		mtranscode_list[i].handle = TRANSCODER_VIRTUAL_BASE_HANDLE + i;
		mtranscode_list[i].support_demux_src = 0;
		mtranscode_list[i].support_demux_dst = 0;
	}
	
	(*list) = mtranscode_list;
	m_max_transcode = num;
		
	return num;
}

static int trsc_close_transcode(struct hw_device_t *dev) 
{
	HI_S32 ret = 0;
	ALOGI("[haltrsc]call %s in.\n",__FUNCTION__); 
	Sample_Protocol_DeRegist(psthandles[0].hProtocolHandle,psthandles[0].hTranscoderHandle);
	ret = HI_Muxer_DeInit();  
	if(HI_SUCCESS != ret)    
	{       
		ALOGE("[haltrsc]HI_Muxer_DeInit failed\n"); 
	}
	ret = HI_Protocol_DeInit();
	if(HI_SUCCESS != ret)   
	{        
		ALOGE("[haltrsc]HI_Protocol_DeInit failed\n");  
	}
	ret = HI_Transcoder_Destroy(psthandles[0].hTranscoderHandle); 
	if(HI_SUCCESS != ret)   
	{        
		ALOGE("[haltrsc]HI_Transcoder_Destroy failed\n"); 
	}
	 ret = HI_Transcoder_DeInit();    
	 if(HI_SUCCESS != ret)  
	 {       
	 	ALOGE("[haltrsc]HI_Transcoder_DeInit failed\n");  
	}
	  HI_UNF_DMX_DetachTSPort(PLAY_DMX_ID);   
	  HI_UNF_DMX_DeInit();

	  return 0;
}

void *SearchTthread (void *param)
{
	HI_U32 ProgNum; 
	HI_U32 s32Ret = 0;
	ALOGI("[haltrsc]call %s in.\n",__FUNCTION__); 	

	while(1)
	{	
		//ALOGI("[haltrsc]call %s sleep.\n",__FUNCTION__); 	//哥！不要再放开这个打印了~~~
		if(push_data_flag)
		{
			ALOGI("****************************************************************\n");
			ALOGI(" g_pProgTbl->prog_num = %d\n",g_pProgTbl->prog_num);
			ALOGI("g_pProgTbl->proginfo[0].ProgID = %d\n",g_pProgTbl->proginfo[0].ProgID);
			ALOGI("g_pProgTbl->proginfo[0].PcrPid = %d\n",g_pProgTbl->proginfo[0].PcrPid);
			ALOGI("g_pProgTbl->proginfo[0].PmtPid = %d\n",g_pProgTbl->proginfo[0].PmtPid);
			ALOGI("g_pProgTbl->proginfo[0].VideoType = 0x%x\n",g_pProgTbl->proginfo[0].VideoType);
			ALOGI("g_pProgTbl->proginfo[0].VElementNum = %d\n",g_pProgTbl->proginfo[0].VElementNum);
			ALOGI("g_pProgTbl->proginfo[0].VElementPid = %d\n",g_pProgTbl->proginfo[0].VElementPid);
			ALOGI("g_pProgTbl->proginfo[0].AudioType = 0x%x\n",g_pProgTbl->proginfo[0].AudioType);
			ALOGI("g_pProgTbl->proginfo[0].AElementNum = %d\n",g_pProgTbl->proginfo[0].AElementNum);
			ALOGI("g_pProgTbl->proginfo[0].AElementPid = %d\n",g_pProgTbl->proginfo[0].AElementPid);
			ALOGI("****************************************************************\n");
			ProgNum = 0; 
			pthread_mutex_lock(&g_TsMutex);
			//s32Ret = HIADP_AVPlay_PlayProg(hAvplay, g_pProgTbl, ProgNum, HI_TRUE);  
			if (s32Ret != HI_SUCCESS)  
			{     
				ALOGE("[haltrsc]call SwitchProg failed.\n"); 
				pthread_mutex_unlock(&g_TsMutex); 
				return NULL;
			}   
			pthread_mutex_unlock(&g_TsMutex);  
			push_data_flag = HI_FALSE;
		}
		usleep(100*1000);
	}
	return NULL;
}

static int careate_search_pthread()
{
	int ret =0;
	pthread_attr_t attr;
	pthread_t   g_TsThd;
	
	ret = pthread_attr_init(&attr);
	if(0 != ret)	
	{		
		ALOGE("[haltrsc]pthread_attr_init error,ret = %d \n",ret);
		return -1;
	}

	ret = pthread_attr_setstacksize(&attr, 20*1024*1024);	
	if(0 != ret)	
	{		
		ALOGE("[haltrsc]pthread_attr_init error,ret = %d \n",ret);
		return -1;
	}
	
	ret = pthread_create(&g_TsThd, &attr, SearchTthread, HI_NULL); 
	if(0 != ret)	
	{		
		ALOGE("[haltrsc]pthread_create error,ret = %d \n",ret);
		return -1;
	}

	return 0;
}


int open_transcode (const struct hw_module_t* module, const char* name,struct hw_device_t** device)
{
	transcoder_context_t *ctx=NULL;
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 i=0;
	HI_HANDLE hWin = 0;  
	HI_UNF_AVPLAY_ATTR_S AvplayAttr; 
	HI_UNF_SYNC_ATTR_S SyncAttr;   
	HI_UNF_AVPLAY_STOP_OPT_S Stop;
	HI_CHAR InputCmd[32];
	HI_HANDLE g_hTranHandle = 0;
	HI_HANDLE hProtoHandle = 0;
	HI_UNF_AUDIOTRACK_ATTR_S stTrackAttr;  
	HI_HANDLE hTrack;   
	HI_UNF_AENC_ATTR_S stAencAttr;  
	ProtocolAttr protoAttr; 
	HI_U32 value;
	HI_CHAR InputValue[32];  
	HI_UNF_WINDOW_ATTR_S   WinAttr;
#if 0
	pstMuxerBuffer = (char*)malloc(PFT_BUF_LEN);
#ifdef DEBUG_INIT 	
	HI_SYS_Init();
	HIADP_MCE_Exit();
	s32Ret = HIADP_Snd_Init(); 
	if (HI_SUCCESS != s32Ret)
	{      
		ALOGE("[haltrsc]call SndInit failed.\n");  
		return -1;
	}  
#endif
	memset(&WinAttr, 0, sizeof(HI_UNF_WINDOW_ATTR_S));
   WinAttr.enDisp = HI_UNF_DISPLAY1;
   WinAttr.bVirtual = HI_TRUE;
	WinAttr.enVideoFormat = HI_UNF_FORMAT_YUV_SEMIPLANAR_420; /*随意指定的, 解决虚拟窗口报错*/
   WinAttr.stWinAspectAttr.enAspectCvrs = HI_UNF_VO_ASPECT_CVRS_IGNORE;
   WinAttr.stWinAspectAttr.bUserDefAspectRatio = HI_TRUE;
   WinAttr.stWinAspectAttr.u32UserAspectWidth  = 0;
   WinAttr.stWinAspectAttr.u32UserAspectHeight = 0;
   WinAttr.bUseCropRect = HI_FALSE;
   WinAttr.stInputRect.s32X = 0;
   WinAttr.stInputRect.s32Y = 0;
   WinAttr.stInputRect.s32Width = 0;
   WinAttr.stInputRect.s32Height = 0;
   memcpy(&WinAttr.stOutputRect, &WinAttr.stInputRect, sizeof(HI_RECT_S));
	 WinAttr.stOutputRect.s32X = 0;
	 WinAttr.stOutputRect.s32Y = 0;
   WinAttr.stOutputRect.s32Width = 0;
   WinAttr.stOutputRect.s32Height = 0;
	
	
	s32Ret = HI_UNF_VO_Init(HI_UNF_VO_DEV_MODE_MOSAIC);
	s32Ret |= HI_UNF_VO_CreateWindow(&WinAttr, &hWin);
	if (HI_SUCCESS != s32Ret)   
	{       
		ALOGE("[haltrsc]call VoInit and create failed.\n"); 
		HIADP_VO_DeInit();      
		return -1;
	}   
	s32Ret = HI_UNF_DMX_Init();  
	if (HI_SUCCESS != s32Ret)   
	{       
		ALOGE("[haltrsc]call HI_UNF_DMX_Init failed.\n");
		return -1;
	}   
	s32Ret |= HI_UNF_DMX_AttachTSPort(PLAY_DMX_ID,HI_UNF_DMX_PORT_RAM_1);  
	if (HI_SUCCESS != s32Ret)  
	{       
		ALOGE("[haltrsc]call HI_UNF_DMX_AttachTSPort failed.\n");
		 return -1;
	}   
	s32Ret = HI_UNF_DMX_CreateTSBuffer(HI_UNF_DMX_PORT_RAM_1, HISI_TS_BUFF_SIZE, &g_TsBuf); 
	if (s32Ret != HI_SUCCESS) 
	{  
		ALOGE("[haltrsc]call HI_UNF_DMX_CreateTSBuffer failed.\n");  
		return -1;
	} 
	
#ifdef DEBUG_INIT
#if 1
	s32Ret = HIADP_AVPlay_RegADecLib(); 
	if (HI_SUCCESS != s32Ret) 
	{       
		ALOGE("[haltrsc]call RegADecLib failed.\n");  
		return -1;
	}   
#else
	/* 加载必要的音频动态库 */	
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.MP2.decode.so");
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.AAC.decode.so");	
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.AMRNB.codec.so");	
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.AMRWB.codec.so");
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.BLURAYLPCM.decode.so");
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.COOK.decode.so");
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.FFMPEG_ADEC.decode.so");
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.FFMPEG_WMAPRO.decode.so");
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.MP3.decode.so");
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.PCM.decode.so");
	//wuxy open for test new ac3.so	
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.AC3PASSTHROUGH.decode.so");	
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.DOLBYPLUS.decode.so");
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.DTSHD.decode.so");
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.DRA.decode.so");	
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.TRUEHD.decode.so");	
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.EAC3.decode.so");
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.G711.codec.so");
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.G722.codec.so");
	HI_UNF_AVPLAY_RegisterAcodecLib("/system/lib/libHA.AUDIO.WMA.decode.so");
	
	

#endif

	s32Ret = HI_UNF_AVPLAY_Init();  
	if (s32Ret != HI_SUCCESS)  
	{       
		ALOGE("[haltrsc]call HI_UNF_AVPLAY_Init failed.\n");
		return -1;
	} 
#endif
	s32Ret = HI_UNF_AVPLAY_GetDefaultConfig(&AvplayAttr, HI_UNF_AVPLAY_STREAM_TYPE_TS); 
	AvplayAttr.u32DemuxId = PLAY_DMX_ID; 
	AvplayAttr.stStreamAttr.u32VidBufSize = (3*1024*1024);
	s32Ret |= HI_UNF_AVPLAY_Create(&AvplayAttr, &hAvplay); 
	if (s32Ret != HI_SUCCESS)
	{        
		ALOGE("[haltrsc]call HI_UNF_AVPLAY_Create failed.\n");
		return -1;
	}    
		HI_UNF_AVPLAY_OPEN_OPT_S stVidOpenOpt;
		stVidOpenOpt.enDecType = HI_UNF_VCODEC_DEC_TYPE_NORMAL;
		stVidOpenOpt.enCapLevel = HI_UNF_VCODEC_CAP_LEVEL_FULLHD;//HI_UNF_VCODEC_CAP_LEVEL_FULLHD;
		stVidOpenOpt.enProtocolLevel = HI_UNF_VCODEC_PRTCL_LEVEL_H264;//HI_UNF_VCODEC_PRTCL_LEVEL_H264;
	s32Ret = HI_UNF_AVPLAY_ChnOpen(hAvplay, HI_UNF_AVPLAY_MEDIA_CHAN_VID, &stVidOpenOpt); 
	if (s32Ret != HI_SUCCESS)
	{        
		ALOGE("[haltrsc]call HI_UNF_AVPLAY_ChnOpen failed.\n");
		return -1;  
	}   
	s32Ret = HI_UNF_AVPLAY_ChnOpen(hAvplay, HI_UNF_AVPLAY_MEDIA_CHAN_AUD, HI_NULL);  
	if (s32Ret != HI_SUCCESS)
	{      
		ALOGE("[haltrsc]call HI_UNF_AVPLAY_ChnOpen failed.\n");   
		return -1;
	}    
	
	#if 1
	s32Ret = HI_UNF_VO_AttachWindow(hWin, hAvplay); 
	if (HI_SUCCESS != s32Ret)   
	{      
		ALOGE("[haltrsc]call HI_UNF_VO_AttachWindow failed:%#x.\n",s32Ret); 
		return -1;
	}    
	
	s32Ret = HI_UNF_VO_SetWindowEnable(hWin, HI_FALSE);
	if (s32Ret != HI_SUCCESS)  
	{        
		ALOGE("[haltrsc]call HI_UNF_VO_SetWindowEnable failed.\n");
		return -1;
	} 
 	#endif  
	s32Ret = HI_UNF_SND_GetDefaultTrackAttr(HI_UNF_SND_TRACK_TYPE_VIRTUAL, &stTrackAttr); 
	if (s32Ret != HI_SUCCESS)    
	{ 
		ALOGE("[haltrsc]call HI_UNF_SND_GetDefaultTrackAttr failed.\n"); 
		return -1;
	}  
	s32Ret = HI_UNF_SND_CreateTrack(HI_UNF_SND_0, &stTrackAttr, &hTrack);  
	if (s32Ret != HI_SUCCESS)  
	{      
		ALOGE("[haltrsc]call HI_UNF_SND_CreateTrack failed.\n"); 
		return -1;
	}    
	s32Ret = HI_UNF_SND_Attach(hTrack, hAvplay);
	if (s32Ret != HI_SUCCESS)  
	{        
		ALOGE("[haltrsc]call HI_SND_Attach failed.\n"); 
		return -1;
	}   
	pthread_mutex_init(&g_TsMutex,NULL); 
	pthread_mutex_init(&g_BufMutex,NULL);
	//*******************************

	memset(&stTranHandleAttr,0,sizeof(Transcoder_HandleAttr));  
	s32Ret = HI_Transcoder_Init(); 
	if(HI_SUCCESS != s32Ret)  
	{     
		ALOGE("[haltrsc]Test_Trans_Init error\n");
		return -1;
	}    

	s32Ret = HI_Protocol_Init(); 
	if(HI_SUCCESS != s32Ret)  
	{        
		ALOGE("[haltrsc]HI_Protocol_Init error\n");
		return -1;
	} 
	
	s32Ret = HI_Muxer_Init();
	if(HI_SUCCESS != s32Ret)
	{   
		ALOGE("[haltrsc]HI_Muxer_Init error\n");  
		return -1;
	} 
#endif

	if(!strcmp(name, TRANSCODE_HARDWARE_TC0))
	{
		ctx = (demux_context_t *)malloc(sizeof(transcoder_context_t));
		memset(ctx, 0, sizeof(*ctx));
		ctx->device.common.tag     = HARDWARE_DEVICE_TAG;
		ctx->device.common.version = 1;
		ctx->device.common.module  = const_cast<hw_module_t*>(module);
		ctx->device.common.close   = trsc_close_transcode;
		ctx->device.bind_demux_src = trsc_bind_demux_src;
		ctx->device.bind_demux_dst = trsc_bind_demux_dst;
		ctx->device.bind_push_src  = trsc_bind_push_src;
		ctx->device.unbind         = trsc_unbind;
		ctx->device.push_data      = trsc_push_data;
		ctx->device.acquire_data   = trsc_acquire_data;
		ctx->device.release_data   = trsc_release_data;
		ctx->device.clear_data     = trsc_clear_data;
		ctx->device.set_transcode_param = trsc_set_param;
		ctx->device.get_capacity_list = trsc_get_capacity_list;
		ctx->device.start = trsc_start;
		ctx->device.stop = trsc_stop;
		for(i=0;i<TRSCD_MAX_NUM;i++)
		{
			/*没有设备的open动作，做个假的好了*/
		    psthandles[i].hProtocolHandle = hProtoHandle;
		    psthandles[i].hTranscoderHandle = g_hTranHandle;
		}
	
	}
	memset(g_pProgTbl,0,sizeof(g_pProgTbl));
	memset(mproginfo,0,sizeof(mproginfo));
	g_pProgTbl->proginfo = mproginfo;
  LOGD("g_pProgTbl->proginfo = %p,mproginfo = %p",g_pProgTbl->proginfo,mproginfo);
	*device = &ctx->device.common;
	//careate_search_pthread();
	return 0;
}
