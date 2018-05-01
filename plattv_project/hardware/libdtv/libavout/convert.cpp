#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <cutils/log.h>

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
#include "hi_unf_avplay.h"
#include "hi_unf_sound.h"
#include "hi_unf_vo.h"
#include "hi_unf_disp.h"
#include "hi_jpge_type.h"
#include "hi_jpge_api.h"
#include "hi_tde_api.h"
//capture 专用
#define MAX_PARTPIXELDATA 3
typedef struct {
	HI_S32 Width; /**<Surface width*//**<CNcomment:surface宽度 */
	HI_S32 Height; /**<Surface height*//**<CNcomment:surface高度 */
	HI_UNF_VIDEO_FORMAT_E PixelFormat; /**<Pixel format of a surface*//**<CNcomment:surface像素格式*/
	HI_U32 Pitch[MAX_PARTPIXELDATA]; /**<Pitch of a surface*//**<CNcomment:surface行间距离*/
	HI_CHAR* pPhyAddr[MAX_PARTPIXELDATA]; /**<Physical address of a surface*//**<CNcomment:surface物理地址*/
	HI_CHAR* pVirAddr[MAX_PARTPIXELDATA]; /**<Virtual address of a surface*//**<CNcomment:surface虚拟地址*/
} ENCODER_SURINFO_S;
//
static HI_S32 JpegEncoder(HI_U32 hEncHandle, ENCODER_SURINFO_S *InputSurInfo, HI_CHAR *bufaddr,
		int bufsize) {
	HI_S32 ret = HI_SUCCESS;
	Jpge_EncIn_S EncIn;
	Jpge_EncOut_S EncOut;
	FILE *pFile = HI_NULL;
	HI_U32 PhysAddr;
	HI_VOID* pVirtAddr;
	ENCODER_SURINFO_S SurInfo;
	SurInfo = *InputSurInfo;
	EncIn.BusViY = (HI_U32) SurInfo.pPhyAddr[0];
	EncIn.BusViC = (HI_U32) SurInfo.pPhyAddr[1];
	EncIn.BusViV = (HI_U32) SurInfo.pPhyAddr[2];
	EncIn.ViYStride = SurInfo.Pitch[0];
	EncIn.ViCStride = SurInfo.Pitch[1];
	//EncIn.ViVStride = SurInfo.Pitch[2];
	EncIn.OutBufSize = (HI_U32)(SurInfo.Width * SurInfo.Height * 3);
	LOGD("offset=0x%x,0x%x,0x%x,size=0x%x\n",EncIn.BusViC - EncIn.BusViY,EncIn.ViYStride,EncIn.ViCStride,EncIn.OutBufSize);
	PhysAddr = (HI_U32) HI_MMZ_New(EncIn.OutBufSize, 4, HI_NULL, "JpegEncoderBuf");
	if (0 == PhysAddr) {
		return HI_FAILURE;
	}

	pVirtAddr = (HI_VOID*) HI_MMZ_Map(PhysAddr, 0);
	if (HI_NULL == pVirtAddr) {
		ret = HI_FAILURE;
		goto err2;
	}
	memset(pVirtAddr, 0xff, EncIn.OutBufSize);

	EncIn.pOutBuf = (HI_U8*) pVirtAddr;
	EncIn.BusOutBuf = PhysAddr;
	memset(&EncOut, 0, sizeof(Jpge_EncOut_S));
	HI_TDE2_WaitAllDone();
	if (HI_SUCCESS != HI_JPGE_Encode(hEncHandle, &EncIn, &EncOut)) {
		ret = HI_FAILURE;
		LOGD("HI_JPGE_Encode error\n");
		goto err2;
	}
	if (bufsize < EncOut.StrmSize) {
		ret = HI_FAILURE;
		LOGE("out of bufsize");
		goto err2;
	}
	memcpy(bufaddr, EncOut.pStream, EncOut.StrmSize);
	ret = EncOut.StrmSize;
	err2: HI_MMZ_Unmap(PhysAddr);
	HI_MMZ_Delete(PhysAddr);
	return ret;
}
static HI_S32 DestroyJpegEncoder(HI_U32 hEncHandle) {
	HI_S32 ret = HI_SUCCESS;

	ret = HI_JPGE_Destroy(hEncHandle);

	HI_JPGE_Close();
	return ret;
}
static HI_S32 TdeInit(HI_VOID) {
	HI_S32 s32Ret;
	/** tde 初始化 */

	s32Ret = HI_TDE2_Open();
	if (HI_SUCCESS != s32Ret) /** attention if reopen need to be deal*/
	{
		LOGD("open tde failed\n");
		return HI_FAILURE;
	}
	s32Ret = HI_TDE2_SetAlphaThresholdState(HI_TRUE);
	if (s32Ret != HI_SUCCESS) {
		LOGD("set threshhold enable failed 0x%x\n", s32Ret);
		return HI_FAILURE;
	}

	/** 目标格式为1555的颜色时需要修改输出判决*/

	s32Ret = HI_TDE2_SetAlphaThresholdValue(0x80);
	if (s32Ret != HI_SUCCESS) {
		LOGD("set threshhold failed 0x%x\n", s32Ret);
		return HI_FAILURE;
	}

	return HI_SUCCESS;
}
static HI_S32 CreateJpegEncoder(HI_U32 *hEncHandle, ENCODER_SURINFO_S *SurInfo) {
	HI_S32 ret;
	Jpge_EncCfg_S EncCfg;

	EncCfg.FrameWidth = (HI_U32)(SurInfo->Width) & 0xfffffffc;
	EncCfg.FrameHeight = (HI_U32) SurInfo->Height;
	EncCfg.YuvSampleType = JPGE_YUV444;
	EncCfg.YuvStoreType = JPGE_PACKAGE;
	EncCfg.RotationAngle = 0;
	EncCfg.SlcSplitEn = 0;
	EncCfg.SplitSize = 0;
	EncCfg.Qlevel = 50;

	switch (SurInfo->PixelFormat) {
	case HI_UNF_FORMAT_YUV_SEMIPLANAR_422:
		EncCfg.YuvSampleType = JPGE_YUV422;
		EncCfg.YuvStoreType = JPGE_SEMIPLANNAR;
		break;
	case HI_UNF_FORMAT_YUV_SEMIPLANAR_420:
		EncCfg.YuvSampleType = JPGE_YUV420;
		EncCfg.YuvStoreType = JPGE_SEMIPLANNAR;
		break;
	case HI_UNF_FORMAT_YUV_PACKAGE_UYVY:
		EncCfg.YuvSampleType = JPGE_YUV444;
		EncCfg.YuvStoreType = JPGE_SEMIPLANNAR;
		break;
	case HI_UNF_FORMAT_YUV_PACKAGE_YUYV:
		EncCfg.YuvSampleType = JPGE_YUV420;
		EncCfg.YuvStoreType = JPGE_PLANNAR;
		break;
	case HI_UNF_FORMAT_YUV_PACKAGE_YVYU:
		EncCfg.YuvSampleType = JPGE_YUV444;
		EncCfg.YuvStoreType = JPGE_PLANNAR;
		break;
	default:
		LOGD("fmt not support\n");
		return HI_FAILURE;
	}
	//打开jpge编码设备
	ret = HI_JPGE_Open();
	if (HI_SUCCESS != ret) {
		LOGD("HI_JPGE_Open error:0x%x\n",ret);
		return HI_FAILURE;
	}

	ret = HI_JPGE_Create(hEncHandle, &EncCfg);
	if (HI_SUCCESS != ret) {
		LOGD("HI_JPGE_Create error:0x%x\n",ret);
		return HI_FAILURE;
	}
	return HI_SUCCESS;
}
HI_S32 avout_framer_picture2jpg(HI_UNF_VIDEO_FRAME_INFO_S *picture_info, char*bufaddr, int bufsize) {

	HI_S32 ret = HI_FAILURE;
	int out_len = -1;
	ENCODER_SURINFO_S MemSurfaceInfo;
	memset(&MemSurfaceInfo, 0, sizeof(MemSurfaceInfo));

	MemSurfaceInfo.Width = picture_info->u32Width;
	MemSurfaceInfo.Height = picture_info->u32Height;
	LOGD("cpinfo.u32Width=%u,cpinfo.u32Height=%u\n", picture_info->u32Width,
			picture_info->u32Height);
	MemSurfaceInfo.PixelFormat = picture_info->enVideoFormat;
	LOGD("src fmt=%d\n", MemSurfaceInfo.PixelFormat);

	MemSurfaceInfo.Pitch[0] = picture_info->stVideoFrameAddr[0].u32YStride;
	MemSurfaceInfo.Pitch[1] = picture_info->stVideoFrameAddr[0].u32CStride;
		MemSurfaceInfo.Pitch[2] = picture_info->stVideoFrameAddr[0].u32CrStride;
	MemSurfaceInfo.pPhyAddr[0] = (HI_CHAR*) picture_info->stVideoFrameAddr[0].u32YAddr;
	MemSurfaceInfo.pPhyAddr[1] = (HI_CHAR*) picture_info->stVideoFrameAddr[0].u32CAddr;
	MemSurfaceInfo.pPhyAddr[2] = (HI_CHAR*) picture_info->stVideoFrameAddr[0].u32CrAddr;

	MemSurfaceInfo.pVirAddr[0] = HI_NULL;
	MemSurfaceInfo.pVirAddr[1] = HI_NULL;
	MemSurfaceInfo.pVirAddr[2] = HI_NULL;
	LOGD("u32YStride 0x%x\n", picture_info->stVideoFrameAddr[0].u32YStride);
	HI_U32 pEncHandle;

	ret = CreateJpegEncoder(&pEncHandle, &MemSurfaceInfo);
	if (HI_SUCCESS != ret) {
		LOGD("\n CreateJpegEncoder failed\n");
		return -1;
	}
	TdeInit();
	out_len = JpegEncoder(pEncHandle, &MemSurfaceInfo, bufaddr, bufsize);
	if (out_len <= 0) {
		LOGD("\n JpegEncoder failed Ret=0x%x\n", ret);
		goto err1;
	}

	HI_TDE2_Close();
	err1: ret = DestroyJpegEncoder(pEncHandle);
	if (HI_SUCCESS != ret) {
		LOGD("\n DestroyJpegEncoder failed\n");
		return -1;
	}
	if (out_len > 0)
		return out_len;
	else
		return -1;
}
