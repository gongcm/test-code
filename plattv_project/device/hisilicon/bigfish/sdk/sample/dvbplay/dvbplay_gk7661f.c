/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : mplayer.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2010/01/26
  Description   :
  History       :
  1.Date        : 2010/01/26
    Author      : w58735
    Modification: Created file

******************************************************************************/
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

#include "hi_unf_common.h"
#include "hi_unf_avplay.h"
#include "hi_unf_sound.h"
#include "hi_unf_disp.h"
#include "hi_unf_vo.h"
#include "hi_unf_demux.h"
#include "hi_unf_hdmi.h"
#include "HA.AUDIO.MP3.decode.h"
#include "HA.AUDIO.MP2.decode.h"
#include "HA.AUDIO.AAC.decode.h"
#include "HA.AUDIO.DRA.decode.h"
#include "HA.AUDIO.PCM.decode.h"
#include "HA.AUDIO.WMA9STD.decode.h"
#include "HA.AUDIO.AMRNB.codec.h"
#include "hi_adp.h"
//#include "hi_adp_audio.h"
#include "hi_adp_hdmi.h"
#include "hi_adp_boardcfg.h"
#include "hi_adp_mpi.h"
#include "hi_adp_tuner.h"
#include "hi_unf_frontend.h"
#include "hi_unf_gpio.h"

#define DBG_PRINT(fmt, args...) do { \
									printf("%s[%d]: " fmt "\n", __func__, __LINE__, ##args); \
								} while (0)

#define MAX_TUNER_NUM 4
#define IO_SHARE_BASE (0xf8a21000)
HI_U32 g_TunerFreq;
HI_U32 g_TunerSrate;
HI_U32 g_ThirdParam;
HI_U32 g_ModType;
static HI_UNF_ENC_FMT_E   g_enDefaultFmt = HI_UNF_ENC_FMT_1080i_50;

PMT_COMPACT_TBL    *g_pProgTbl = HI_NULL;
#define GK7661F_TUNER_PORT 0

#define GK7661F_TSI_PORT HI_UNF_DMX_PORT_TSI_0

void cfgRegister_GK7661F(void);
void resetTuner_GK7661F(HI_U32 rstGpioNo);

HI_S32 main(HI_S32 argc,HI_CHAR *argv[])
{
    HI_S32                  Ret;
    HI_HANDLE               hWin;
    HI_HANDLE               hAvplay;
	HI_UNF_TUNER_STATUS_S  stTunerStatus;
	
    HI_UNF_AVPLAY_ATTR_S        AvplayAttr;
    HI_UNF_SYNC_ATTR_S          SyncAttr;
    HI_UNF_AVPLAY_STOP_OPT_S    Stop;
    HI_CHAR                 InputCmd[32];
    HI_U32                  ProgNum;
	
    HI_HANDLE hTrack;
    HI_UNF_AUDIOTRACK_ATTR_S stTrackAttr;
	HI_UNF_TUNER_ATTR_S TunerAttr;

    HI_UNF_TUNER_CONNECT_PARA_S  ConnectPara;
	HI_UNF_WINDOW_ATTR_S   WinAttr;
    HI_UNF_DMX_PORT_ATTR_S PortAttr;

    HI_U32 i = 0, j = 0;

    if (5 == argc)
    {
        g_TunerFreq  = strtol(argv[1],NULL,0);
        g_TunerSrate = strtol(argv[2],NULL,0);
        g_ThirdParam = strtol(argv[3],NULL,0);
        g_enDefaultFmt = HIADP_Disp_StrToFmt(argv[4]);
    }
    else if (4 == argc)
    {
        g_TunerFreq  = strtol(argv[3],NULL,0);
        g_TunerSrate = strtol(argv[2],NULL,0);
        g_ThirdParam = strtol(argv[1],NULL,0);
        g_enDefaultFmt = HI_UNF_ENC_FMT_1080i_50;
    }
    else if(3 == argc)
    {
        g_TunerFreq  = strtol(argv[1],NULL,0);
        g_TunerSrate = strtol(argv[2],NULL,0);
        g_ThirdParam = (g_TunerFreq>1000) ? 0 : 64;
        g_enDefaultFmt = HI_UNF_ENC_FMT_1080i_50;
    }
    else if(2 == argc)
    {
        g_TunerFreq  = strtol(argv[1],NULL,0);
        g_TunerSrate = (g_TunerFreq>1000) ? 27500 : 6875;
        g_ThirdParam = (g_TunerFreq>1000) ? 0 : 64;
        g_enDefaultFmt = HI_UNF_ENC_FMT_1080i_50;
    }
    else
    {
        printf("Usage: %s freq [srate] [qamtype or polarization] [vo_format]\n"
                "       qamtype or polarization: \n"
                "           For cable, used as qamtype, value can be 16|32|[64]|128|256|512 defaut[64] \n"
                "           For satellite, used as polarization, value can be [0] horizontal | 1 vertical defaut[0] \n"
                "       vo_format:1080P_60|1080P_50|1080i_60|[1080i_50]|720P_60|720P_50  default HI_UNF_ENC_FMT_1080i_50\n",
                argv[0]);
        printf("Example: %s 610 6875 64 1080i_50\n", argv[0]);
        return HI_FAILURE;
    }

	DBG_PRINT("Freq: %dMHz, SymRate: %dKHz, QAM: %d", g_TunerFreq, g_TunerSrate, g_ThirdParam);

	switch (g_ThirdParam)
	{
		case 16:
			g_ModType = HI_UNF_MOD_TYPE_QAM_16;
			break;
		case 32:
			g_ModType = HI_UNF_MOD_TYPE_QAM_32;
			break;
		case 64:
			g_ModType = HI_UNF_MOD_TYPE_QAM_64;
			break;
		case 128:
			g_ModType = HI_UNF_MOD_TYPE_QAM_128;
			break;
		case 256:
			g_ModType = HI_UNF_MOD_TYPE_QAM_256;
			break;
		case 512:
			g_ModType = HI_UNF_MOD_TYPE_QAM_512;
			break;
		default:
			g_ModType = HI_UNF_MOD_TYPE_QAM_256;
			break;
	}

    HI_SYS_Init();
    HIADP_MCE_Exit();

	DBG_PRINT("HIADP_Disp_DeInit");
    HIADP_VO_DeInit();
    HIADP_Disp_DeInit();

	cfgRegister_GK7661F();
	
	HI_UNF_GPIO_Init();
    HI_UNF_I2C_Init();
	HI_UNF_I2C_SetRate(1, HI_UNF_I2C_RATE_100K);
	HI_UNF_I2C_SetRate(2, HI_UNF_I2C_RATE_100K);

	do
	{
		resetTuner_GK7661F(6*8+2);
	} while (0);

#if 1
    Ret = HIADP_Snd_Init();
    if (HI_SUCCESS != Ret)
    {
        DBG_PRINT("call HIADP_Snd_Init failed.\n");
        goto SYS_DEINIT;
    }
	
    Ret = HIADP_Disp_Init(g_enDefaultFmt);
    if (HI_SUCCESS != Ret)
    {
        DBG_PRINT("call HIADP_Disp_DeInit failed.\n");
        goto SND_DEINIT;
    }

    Ret = HIADP_VO_Init(HI_UNF_VO_DEV_MODE_NORMAL);
	
	memset(&WinAttr, 0, sizeof(HI_UNF_WINDOW_ATTR_S));
	WinAttr.enDisp = HI_UNF_DISPLAY1;
	WinAttr.bUseCropRect = 0;
	WinAttr.bVirtual = 0;	
	WinAttr.enVideoFormat = HI_UNF_FORMAT_YUV_SEMIPLANAR_422;	
	WinAttr.stWinAspectAttr.enAspectCvrs = HI_UNF_VO_ASPECT_CVRS_IGNORE; 
	WinAttr.stWinAspectAttr.bUserDefAspectRatio = HI_FALSE;  
	WinAttr.stWinAspectAttr.u32UserAspectWidth  = 0;   
	WinAttr.stWinAspectAttr.u32UserAspectHeight = 0;   
	WinAttr.bUseCropRect = HI_FALSE;   
	WinAttr.stInputRect.s32X = 0;   
	WinAttr.stInputRect.s32Y = 0;   
	WinAttr.stInputRect.s32Width = 1920;   
	WinAttr.stInputRect.s32Height = 1080;	

	Ret |= HIADP_VO_CreatWin(HI_NULL,&hWin);
    if (HI_SUCCESS != Ret)
    {
        printf("call HIADP_VO_Init failed.\n");
        HIADP_VO_DeInit();
        goto DISP_DEINIT;
    }
#endif

	DBG_PRINT("Tuner & Demux config...");

    Ret = HI_UNF_DMX_Init();

    HI_UNF_DMX_GetTSPortAttr(GK7661F_TSI_PORT, &PortAttr);// PORT = 0
	
    PortAttr.enPortMod = HI_UNF_DMX_PORT_MODE_EXTERNAL;
    PortAttr.enPortType = HI_UNF_DMX_PORT_TYPE_PARALLEL_NOSYNC_188;//;
	PortAttr.u32TunerInClk	= 0; //default value is 0
	PortAttr.u32SerialBitSelector = 0; //Data[0]为数据位
	PortAttr.u32TunerErrMod = 1; //default value is 1
	
	printf(" HI_UNF_DMX_PORT_MODE_EXTERNAL : %d \n",HI_UNF_DMX_PORT_MODE_EXTERNAL);
	printf("HI_UNF_DMX_PORT_TYPE_PARALLEL_NOSYNC_188 : %d \n",HI_UNF_DMX_PORT_TYPE_PARALLEL_NOSYNC_188);
    Ret = HI_UNF_DMX_SetTSPortAttr(GK7661F_TSI_PORT, &PortAttr);
    if (HI_SUCCESS != Ret)
    {
        DBG_PRINT("call HI_UNF_DMX_SetTSPortAttr failed.\n");
    }
    Ret = HI_UNF_DMX_AttachTSPort(GK7661F_TUNER_PORT, GK7661F_TSI_PORT);
    if (HI_SUCCESS != Ret)
    {
        DBG_PRINT("call HIADP_Demux_Init failed.\n");
        goto VO_DEINIT;
    }

    /*Initialize Tuner*/
    Ret = HI_UNF_TUNER_Init();
    if (HI_SUCCESS != Ret)
    {
        printf("[%s] HI_UNF_TUNER_Init failed 0x%x\n", __FUNCTION__, Ret);
        return Ret;
    }

	/*Open Tuner*/
	for(i = 0; i < 4; i++)
	{
		Ret = HI_UNF_TUNER_Open(i);
		if (HI_SUCCESS != Ret)
		{
			printf("[%s] HI_UNF_TUNER_Open failed 0x%x\n", __FUNCTION__, Ret);
		}
	}

	/*get default attribute in order to set attribute next*/
	Ret = HI_UNF_TUNER_GetDeftAttr(GK7661F_TUNER_PORT, &TunerAttr);
	if (HI_SUCCESS != Ret)
	{
		printf("[%s] HI_UNF_TUNER_GetDeftAttr failed 0x%x\n", __FUNCTION__, Ret);
	}

	TunerAttr.enSigType = HI_UNF_TUNER_SIG_TYPE_CAB/*HI_UNF_TUNER_SIG_TYPE_SAT*/;
	TunerAttr.enTunerDevType = HI_UNF_TUNER_DEV_TYPE_MXL254;
	TunerAttr.u32TunerAddr = 0x50 << 1;
	TunerAttr.enDemodDevType = HI_UNF_DEMOD_DEV_TYPE_MXL254;
	TunerAttr.u32DemodAddr = 0x50 << 1;
	TunerAttr.enI2cChannel = 2;
	TunerAttr.u32ResetGpioNo = 6*8+2;
	TunerAttr.enOutputMode = HI_UNF_TUNER_OUTPUT_MODE_DEFAULT;

	ConnectPara.enSigType = HI_UNF_TUNER_SIG_TYPE_CAB;
	ConnectPara.unConnectPara.stCab.u32Freq = g_TunerFreq*1000;
	ConnectPara.unConnectPara.stCab.u32SymbolRate = g_TunerSrate*1000;
	ConnectPara.unConnectPara.stCab.enModType = g_ModType;
		printf("HI_UNF_TUNER_SetAttr:\n");
		printf("TunerAttr.enSigType=%x\n",TunerAttr.enSigType);
		printf("TunerAttr.enTunerDevType=%x\n",TunerAttr.enTunerDevType);
		printf("TunerAttr.u32TunerAddr=%x\n",TunerAttr.u32TunerAddr);
		printf("TunerAttr.enDemodDevType=%x\n",TunerAttr.enDemodDevType);
		printf("TunerAttr.u32DemodAddr=%x\n",TunerAttr.u32DemodAddr);
		printf("TunerAttr.enOutputMode=%x\n",TunerAttr.enOutputMode);
		printf("TunerAttr.enI2cChannel=%x\n",TunerAttr.enI2cChannel);
		printf("TunerAttr.u32ResetGpioNo=%x\n",TunerAttr.u32ResetGpioNo);
	for(i = 0; i < 1; i++)
	//for(i = 0; i < 4; i++)
	{
		Ret = HI_UNF_TUNER_SetAttr(i, &TunerAttr);
		if (HI_SUCCESS != Ret)
		{
			printf("[%s] HI_UNF_TUNER_SetAttr failed 0x%x\n", __FUNCTION__, Ret);
			HI_UNF_TUNER_Close(GK7661F_TUNER_PORT);
			HI_UNF_TUNER_DeInit();
		}

		ConnectPara.unConnectPara.stCab.u32Freq = (g_TunerFreq+8*i)*1000;
		Ret = HI_UNF_TUNER_Connect(i, &ConnectPara, 500);
		if (HI_SUCCESS != Ret)
		{
			DBG_PRINT("call Tuner_Connect failed. ret = 0x%x\n",Ret);
			//goto TUNER_DEINIT;
		}
	}

	for(i = 0; i < 4; i++)
	{
		memset(&stTunerStatus, 0x00, sizeof(HI_UNF_TUNER_STATUS_S));
		HI_UNF_TUNER_GetStatus(GK7661F_TUNER_PORT, &stTunerStatus);
		printf("%s[%d]: Tuner %d Status: %s\n", __func__, __LINE__,  \
				GK7661F_TUNER_PORT, \
			(HI_UNF_TUNER_SIGNAL_LOCKED == stTunerStatus.enLockStatus)?"Lock":"Unlock");
		usleep(300*1000);
	}

    HIADP_Search_Init();

	do {
		if(HI_UNF_TUNER_SIGNAL_LOCKED == stTunerStatus.enLockStatus)
		{
			Ret = HIADP_Search_GetAllPmt(GK7661F_TUNER_PORT, &g_pProgTbl);
			if (HI_SUCCESS != Ret)
			{
				DBG_PRINT("call HIADP_Search_GetAllPmt failed\n");
				goto PSISI_FREE;
			}

			DBG_PRINT("Tuner %d: progNum: %d", GK7661F_TUNER_PORT, g_pProgTbl->prog_num);
			for(j = 0; j < g_pProgTbl->prog_num; j++)
			{
				printf("Video Pid: %x\n", g_pProgTbl->proginfo[j].VElementPid);
				printf("Audio Pid: %x\n", g_pProgTbl->proginfo[j].AElementPid);
			}
		}
		usleep(1000*1000);
	} while (0);

#if 1
    Ret = HIADP_AVPlay_RegADecLib();
    Ret |= HI_UNF_AVPLAY_Init();
    if (Ret != HI_SUCCESS)
    {
        DBG_PRINT("call HI_UNF_AVPLAY_Init failed.\n");
        goto PSISI_FREE;
    }

    Ret = HI_UNF_AVPLAY_GetDefaultConfig(&AvplayAttr, HI_UNF_AVPLAY_STREAM_TYPE_TS);
    AvplayAttr.stStreamAttr.u32VidBufSize = 0x300000;


	AvplayAttr.u32DemuxId = GK7661F_TUNER_PORT;
	Ret |= HI_UNF_AVPLAY_Create(&AvplayAttr, &hAvplay);
	if (Ret != HI_SUCCESS)
	{
		DBG_PRINT("call HI_UNF_AVPLAY_Create failed.\n");
		goto AVPLAY_DEINIT;
	}

	Ret = HI_UNF_AVPLAY_ChnOpen(hAvplay, HI_UNF_AVPLAY_MEDIA_CHAN_VID, HI_NULL);
	Ret |= HI_UNF_AVPLAY_ChnOpen(hAvplay, HI_UNF_AVPLAY_MEDIA_CHAN_AUD, HI_NULL);
	if (HI_SUCCESS != Ret)
	{
		DBG_PRINT("call HI_UNF_AVPLAY_ChnOpen failed.\n");
		goto CHN_CLOSE;
	}

	Ret = HI_UNF_VO_AttachWindow(hWin, hAvplay);
	if (HI_SUCCESS != Ret)
	{
		DBG_PRINT("call HI_UNF_VOttachWindow failed.\n");
		goto CHN_CLOSE;
	}
	Ret = HI_UNF_VO_SetWindowEnable(hWin, HI_TRUE);
	if (HI_SUCCESS != Ret)
	{
		DBG_PRINT("call HI_UNF_VO_SetWindowEnable failed.\n");
		goto WIN_DETACH;
	}


	Ret = HI_UNF_SND_GetDefaultTrackAttr(HI_UNF_SND_TRACK_TYPE_MASTER, &stTrackAttr);   
	if (Ret != HI_SUCCESS)    
	{        
		DBG_PRINT("call HI_UNF_SND_GetDefaultTrackAttr failed.\n"); 
		goto WIN_DETACH; 
	}    
	Ret = HI_UNF_SND_CreateTrack(HI_UNF_SND_0,&stTrackAttr,&hTrack); 
	if (Ret != HI_SUCCESS) 
	{       
		DBG_PRINT("call HI_UNF_SND_CreateTrack failed.\n");  
		goto WIN_DETACH;  
	}     
	Ret = HI_UNF_SND_Attach(hTrack, hAvplay); 
	if (Ret != HI_SUCCESS) 
	{      
		DBG_PRINT("call HI_UNF_SNDttach failed.\n");  
		goto TRACK_DESTROY;  
	}

	Ret = HI_UNF_AVPLAY_GetAttr(hAvplay, HI_UNF_AVPLAY_ATTR_ID_SYNC, &SyncAttr);
	SyncAttr.enSyncRef = HI_UNF_SYNC_REF_AUDIO;
	SyncAttr.stSyncStartRegion.s32VidPlusTime = 60;
	SyncAttr.stSyncStartRegion.s32VidNegativeTime = -20;
	SyncAttr.bQuickOutput = HI_FALSE;
	Ret = HI_UNF_AVPLAY_SetAttr(hAvplay, HI_UNF_AVPLAY_ATTR_ID_SYNC, &SyncAttr);
	if (Ret != HI_SUCCESS)
	{
		DBG_PRINT("call HI_UNF_AVPLAY_SetAttr failed.\n");
		goto SND_DETACH;
	}


	ProgNum = 0;
	if(NULL != g_pProgTbl)
		Ret = HIADP_AVPlay_PlayProg(hAvplay, g_pProgTbl, ProgNum, HI_TRUE);
	if (Ret != HI_SUCCESS)
	{
		DBG_PRINT("call SwitchProg failed.\n");
		goto AVPLAY_STOP;
	}
#endif

#if 1
    while(1)
    {
        printf("please input the q to quit!\n");
        SAMPLE_GET_INPUTCMD(InputCmd);

        if ('q' == InputCmd[0])
        {
            printf("prepare to quit!\n");
            break;
        }

    	ProgNum = atoi(InputCmd);

    	if(ProgNum == 0)
    	    ProgNum = 1;

		Ret = HIADP_AVPlay_PlayProg(hAvplay, g_pProgTbl, ProgNum-1, HI_TRUE);
		if (Ret != HI_SUCCESS)
		{
			DBG_PRINT("call SwitchProgfailed!\n");
			break;
		}
    }
#endif

#if 0
	HI_U32 signalStrength;
	HI_U32 signalQuality;

	while (1)
	{
		HI_UNF_TUNER_GetStatus(GK7661F_TUNER_PORT, &stTunerStatus);
		if (HI_UNF_TUNER_SIGNAL_LOCKED == stTunerStatus.enLockStatus) {
			HI_UNF_TUNER_GetSignalStrength(GK7661F_TUNER_PORT, &signalStrength);
			HI_UNF_TUNER_GetSignalQuality(GK7661F_TUNER_PORT, &signalQuality);
			printf("Tuner %d\n", GK7661F_TUNER_PORT);
			DBG_PRINT("Strength: %d", signalStrength);
			DBG_PRINT("Quality: %d", signalQuality);
		}
		sleep(1);
	}
#endif

AVPLAY_STOP:
    Stop.enMode = HI_UNF_AVPLAY_STOP_MODE_BLACK;
    Stop.u32TimeoutMs = 0;
    HI_UNF_AVPLAY_Stop(hAvplay, HI_UNF_AVPLAY_MEDIA_CHAN_VID | HI_UNF_AVPLAY_MEDIA_CHAN_AUD, &Stop);

SND_DETACH:
    HI_UNF_SND_Detach(hTrack, hAvplay);

TRACK_DESTROY:
    HI_UNF_SND_DestroyTrack(hTrack);         

WIN_DETACH:
    HI_UNF_VO_SetWindowEnable(hWin,HI_FALSE);
    HI_UNF_VO_DetachWindow(hWin, hAvplay);

CHN_CLOSE:
    HI_UNF_AVPLAY_ChnClose(hAvplay, HI_UNF_AVPLAY_MEDIA_CHAN_VID | HI_UNF_AVPLAY_MEDIA_CHAN_AUD);

    HI_UNF_AVPLAY_Destroy(hAvplay);

AVPLAY_DEINIT:
    HI_UNF_AVPLAY_DeInit();

PSISI_FREE:
    HIADP_Search_FreeAllPmt(g_pProgTbl);
    HIADP_Search_DeInit();

TUNER_DEINIT:
    HIADP_Tuner_DeInit();

DMX_DEINIT:
    HI_UNF_DMX_DetachTSPort(GK7661F_TUNER_PORT);
    HI_UNF_DMX_DeInit();

VO_DEINIT:
    HI_UNF_VO_DestroyWindow(hWin);
    HIADP_VO_DeInit();

DISP_DEINIT:
    HIADP_Disp_DeInit();

SND_DEINIT:
    HIADP_Snd_DeInit();

SYS_DEINIT:
    HI_SYS_DeInit();

    return Ret;
}

void resetTuner_GK7661F(HI_U32 rstGpioNo)
{
	HI_S32 Ret;

    Ret = HI_UNF_GPIO_SetDirBit(rstGpioNo, HI_FALSE);
	if(HI_SUCCESS != Ret)
	{
		printf("%s[%d] <gpio> Err\n", __func__, __LINE__);
	}
	
	HI_UNF_GPIO_WriteBit(rstGpioNo, HI_FALSE);
	usleep(100*1000);
	HI_UNF_GPIO_WriteBit(rstGpioNo, HI_TRUE);
	usleep(100*1000);
}

void cfgRegister_GK7661F(void)
{
	/*GK7661F Frontend 信息*/
	HI_U32 val;
/*
	Reset_Pin: GPIO6_2
	I2C通道:	2
	I2C引脚:	SCL -- GPIO6_0/I2C2_SCL 
				SDA -- GPIO6_1/I2C2_SDA

	TSI通道:	TSI0
	TSI引脚:	CLK	-- GPIO4_6/TSI0_CLK 
				VALID	-- GPIO4_7/TSI0_VALID
				DATA0	-- GPIO4_5/TSI0_D0
				DATA1	-- GPIO4_4/TSI0_D1
				DATA2	-- GPIO4_3/TSI0_D2
				DATA3	-- GPIO2_7/TSI0_D3
				DATA4	-- GPIO2_6/TSI0_D4
				DATA5	-- GPIO2_5/TSI0_D5
				DATA6	-- GPIO2_4/TSI0_D6
				DATA7	-- GPIO2_3/TSI0_D7
	Demod Name: MXL254C		I2C Addr: 0xa6
	Tuner Name: MXL254C		I2C Addr: 0xf8				
*/

	HI_SYS_ReadRegister(IO_SHARE_BASE+0x0A8, &val);//GPIO6_2
	val = (val&0xfffffff8);
	HI_SYS_WriteRegister(IO_SHARE_BASE+0x0A8, val);

	HI_SYS_ReadRegister(IO_SHARE_BASE+0x0A4, &val); //GPIO6_1/I2C2_SDA
	val = (val&0xfffffff8)|0x5;
	HI_SYS_WriteRegister(IO_SHARE_BASE+0x0A4, val);
	HI_SYS_ReadRegister(IO_SHARE_BASE+0x0A0, &val); //GPIO6_0/I2C2_SCL
	val = (val&0xfffffff8)|0x5;
	HI_SYS_WriteRegister(IO_SHARE_BASE+0x0A0, val);

	HI_SYS_ReadRegister(IO_SHARE_BASE+0x09C, &val); //GPIO4_7/TSI0_VALID
	val = (val&0xfffffff8)|0x5;
	HI_SYS_WriteRegister(IO_SHARE_BASE+0x09C, val);
	HI_SYS_ReadRegister(IO_SHARE_BASE+0x098, &val); //GPIO4_6/TSI0_CLK
	val = (val&0xfffffff8)|0x5;
	HI_SYS_WriteRegister(IO_SHARE_BASE+0x098, val);
	HI_SYS_ReadRegister(IO_SHARE_BASE+0x094, &val); //GPIO4_5/TSI0_D0
	val = (val&0xfffffff8)|0x5;
	HI_SYS_WriteRegister(IO_SHARE_BASE+0x094, val);
	HI_SYS_ReadRegister(IO_SHARE_BASE+0x090, &val); //GPIO4_4/TSI0_D1
	val = (val&0xfffffff8)|0x5;
	HI_SYS_WriteRegister(IO_SHARE_BASE+0x090, val);
	HI_SYS_ReadRegister(IO_SHARE_BASE+0x08C, &val); //GPIO4_3/TSI0_D2
	val = (val&0xfffffff8)|0x5;
	HI_SYS_WriteRegister(IO_SHARE_BASE+0x08C, val);
	HI_SYS_ReadRegister(IO_SHARE_BASE+0x05C, &val); //GPIO2_7/TSI0_D3
	val = (val&0xfffffff8)|0x5;
	HI_SYS_WriteRegister(IO_SHARE_BASE+0x05C, val);
	HI_SYS_ReadRegister(IO_SHARE_BASE+0x058, &val); //GPIO2_6/TSI0_D4
	val = (val&0xfffffff8)|0x5;
	HI_SYS_WriteRegister(IO_SHARE_BASE+0x058, val);
	HI_SYS_ReadRegister(IO_SHARE_BASE+0x054, &val); //GPIO2_5/TSI0_D5
	val = (val&0xfffffff8)|0x5;
	HI_SYS_WriteRegister(IO_SHARE_BASE+0x054, val);
	HI_SYS_ReadRegister(IO_SHARE_BASE+0x050, &val); //GPIO2_4/TSI0_D6
	val = (val&0xfffffff8)|0x5;
	HI_SYS_WriteRegister(IO_SHARE_BASE+0x050, val);
	HI_SYS_ReadRegister(IO_SHARE_BASE+0x04C, &val); //GPIO2_3/TSI0_D7
	val = (val&0xfffffff8)|0x5;
	HI_SYS_WriteRegister(IO_SHARE_BASE+0x04C, val);
}

