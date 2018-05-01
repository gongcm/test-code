#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <string.h>   /* for NULL */
#include <unistd.h>

#include "hi_adp.h"
#include "hi_adp_tuner.h"
#include "hi_adp_boardcfg.h"
#include "hi_unf_frontend.h"
#include "hi_unf_demux.h"

//#define HI3138_SOCKET_1A
#define IO_SHARE_BASE (0xf8a21000)
static HI_UNF_TUNER_SIG_TYPE_E g_enSigType[HI_TUNER_NUMBER];
 void cfgRegister_GK7661F(void);
 void resetTuner_GK7661F(HI_U32 rstGpioNo);
/********************************* TUNER public function*******************************************/
HI_S32 HIADP_Tuner_GetConfig(HI_U32 TunerId, HI_UNF_TUNER_ATTR_S *TunerAttr)
{
    HI_U32  I2cNum;
    HI_U32  GpioSDA     = 0;
    HI_U32  GpioSCL     = 0;
    HI_BOOL UseI2c      = HI_FALSE;
    HI_BOOL UseGpio     = HI_FALSE;

    switch (TunerId)
    {
        case 0:
#if 0
            TunerAttr->enSigType        = HI_TUNER_SIGNAL_TYPE;
            TunerAttr->enTunerDevType   = HI_TUNER_TYPE;
            TunerAttr->u32TunerAddr     = HI_TUNER_DEV_ADDR;
            TunerAttr->enDemodDevType   = HI_DEMOD_TYPE;
            TunerAttr->u32DemodAddr     = HI_DEMOD_DEV_ADDR;
            TunerAttr->enOutputMode     = HI_DEMOD_TS_MODE;
            TunerAttr->u32ResetGpioNo   = HI_DEMOD_RESET_GPIO;
#endif            
            	TunerAttr->enSigType = HI_UNF_TUNER_SIG_TYPE_CAB/*HI_UNF_TUNER_SIG_TYPE_SAT*/;
							TunerAttr->enTunerDevType = HI_UNF_TUNER_DEV_TYPE_MXL254;
							TunerAttr->u32TunerAddr = 0x50 << 1;
							TunerAttr->enDemodDevType = HI_UNF_DEMOD_DEV_TYPE_MXL254;
							TunerAttr->u32DemodAddr = 0x50 << 1;
							//TunerAttr.enI2cChannel = 2;
							TunerAttr->u32ResetGpioNo = 6*8+2;
							TunerAttr->enOutputMode = HI_UNF_TUNER_OUTPUT_MODE_DEFAULT;
        #if defined(HI_DEMOD_USE_I2C)
            UseI2c  = HI_TRUE;
            I2cNum  = 2;
        #elif defined(HI_DEMOD_USE_GPIO)
            UseGpio = HI_TRUE;
            GpioSCL = HI_DEMOD_GPIO_SCL;
            GpioSDA = HI_DEMOD_GPIO_SDA;
        #endif

#ifdef GET_MULTIMODE_DEMOD_CONFIG
            {
                GET_MULTIMODE_DEMOD_CONFIG(0,TunerAttr->stPortAttr);
            }
#endif

            break;

        case 1:
            TunerAttr->enSigType        = HI_TUNER1_SIGNAL_TYPE;
            TunerAttr->enTunerDevType   = HI_TUNER1_TYPE;
            TunerAttr->u32TunerAddr     = HI_TUNER1_DEV_ADDR;
            TunerAttr->enDemodDevType   = HI_DEMOD1_TYPE;
            TunerAttr->u32DemodAddr     = HI_DEMOD1_DEV_ADDR;
            TunerAttr->enOutputMode     = HI_DEMOD1_TS_MODE;
            TunerAttr->u32ResetGpioNo   = HI_DEMOD1_RESET_GPIO;
        #if defined(HI_DEMOD1_USE_I2C)
            UseI2c  = HI_TRUE;
            I2cNum  = HI_DEMOD1_I2C_CHANNEL;
        #elif defined(HI_DEMOD1_USE_GPIO)
            UseGpio = HI_TRUE;
            GpioSCL = HI_DEMOD1_GPIO_SCL;
            GpioSDA = HI_DEMOD1_GPIO_SDA;
        #endif
#ifdef GET_MULTIMODE_DEMOD_CONFIG
            {
                GET_MULTIMODE_DEMOD_CONFIG(1,TunerAttr->stPortAttr);
            }
#endif
            break;

        case 2:
            TunerAttr->enSigType        = HI_TUNER2_SIGNAL_TYPE;
            TunerAttr->enTunerDevType   = HI_TUNER2_TYPE;
            TunerAttr->u32TunerAddr     = HI_TUNER2_DEV_ADDR;
            TunerAttr->enDemodDevType   = HI_DEMOD2_TYPE;
            TunerAttr->u32DemodAddr     = HI_DEMOD2_DEV_ADDR;
            TunerAttr->enOutputMode     = HI_DEMOD2_TS_MODE;
            TunerAttr->u32ResetGpioNo   = HI_DEMOD2_RESET_GPIO;
        #if defined(HI_DEMOD2_USE_I2C)
            UseI2c  = HI_TRUE;
            I2cNum  = HI_DEMOD2_I2C_CHANNEL;
        #elif defined(HI_DEMOD2_USE_GPIO)
            UseGpio = HI_TRUE;
            GpioSCL = HI_DEMOD2_GPIO_SCL;
            GpioSDA = HI_DEMOD2_GPIO_SDA;
        #endif
            break;

        case 3:
            TunerAttr->enSigType        = HI_TUNER3_SIGNAL_TYPE;
            TunerAttr->enTunerDevType   = HI_TUNER3_TYPE;
            TunerAttr->u32TunerAddr     = HI_TUNER3_DEV_ADDR;
            TunerAttr->enDemodDevType   = HI_DEMOD3_TYPE;
            TunerAttr->u32DemodAddr     = HI_DEMOD3_DEV_ADDR;
            TunerAttr->enOutputMode     = HI_DEMOD3_TS_MODE;
            TunerAttr->u32ResetGpioNo   = HI_DEMOD3_RESET_GPIO;
        #if defined(HI_DEMOD3_USE_I2C)
            UseI2c  = HI_TRUE;
            I2cNum  = HI_DEMOD3_I2C_CHANNEL;
        #elif defined(HI_DEMOD3_USE_GPIO)
            UseGpio = HI_TRUE;
            GpioSCL = HI_DEMOD3_GPIO_SCL;
            GpioSDA = HI_DEMOD3_GPIO_SDA;
        #endif
            break;

        default:
            sample_common_printf("[%s] tunerid %u error\n", __FUNCTION__, TunerId);
            return HI_FAILURE;
    }

    if (UseI2c)
    {
        TunerAttr->enI2cChannel = I2cNum;
    }
    else if (UseGpio)
    {
        HI_S32  ret;
        HI_U32  GpioI2cNum;

        ret = HI_UNF_I2C_CreateGpioI2c(&GpioI2cNum, GpioSCL, GpioSDA);
        if (HI_SUCCESS != ret)
        {
            sample_common_printf("[%s] HI_UNF_I2C_CreateGpioI2c failed 0x%x\n", __FUNCTION__, ret);
            return HI_FAILURE;
        }

        TunerAttr->enI2cChannel = GpioI2cNum;
    }

    sample_common_printf("[%s] TunerId=%u, TunerType=%u, TunerAddr=0x%x, I2cChannel=%u, DemodAddr=0x%x, DemodType=%u, OutputMode=%d\n",
        __FUNCTION__, TunerId, TunerAttr->enTunerDevType, TunerAttr->u32TunerAddr,
        TunerAttr->enI2cChannel, TunerAttr->u32DemodAddr, TunerAttr->enDemodDevType, TunerAttr->enOutputMode);

    return HI_SUCCESS;
}

HI_S32 HIADP_Tuner_Init(HI_VOID)
{
    HI_S32                          Ret;
    HI_UNF_TUNER_ATTR_S             TunerAttr;
    HI_UNF_TUNER_FE_LNB_CONFIG_S    LnbConfig;
    HI_UNF_TUNER_SAT_ATTR_S         SatTunerAttr;
    HI_UNF_TUNER_TER_ATTR_S         TerTunerAttr;
    HI_UNF_TUNER_CAB_ATTR_S         stCabTunerAttr;
    HI_UNF_TUNER_TSOUT_SET_S        stTSOut;
    HI_U32                          i;
    HI_BOOL                         SetTSOut = HI_FALSE;

			cfgRegister_GK7661F();
	
	HI_UNF_GPIO_Init();
  HI_UNF_I2C_Init();
	HI_UNF_I2C_SetRate(1, HI_UNF_I2C_RATE_100K);
	HI_UNF_I2C_SetRate(2, HI_UNF_I2C_RATE_100K);

	do
	{
		resetTuner_GK7661F(6*8+2);
	} while (0);
   // Ret = HI_UNF_I2C_Init();
    //if (HI_SUCCESS != Ret)
    //{
      //  sample_common_printf("[%s] HI_UNF_I2C_Init failed 0x%x\n", __FUNCTION__, Ret);
       // return Ret;
    //}

    Ret = HI_UNF_TUNER_Init();
    if (HI_SUCCESS != Ret)
    {
        sample_common_printf("[%s] HI_UNF_TUNER_Init failed 0x%x\n", __FUNCTION__, Ret);
        return Ret;
    }

    for (i = 0; i < HI_TUNER_NUMBER; i++)
    {
        Ret = HI_UNF_TUNER_Open(i);
        if (HI_SUCCESS != Ret)
        {
            sample_common_printf("[%s] HI_UNF_TUNER_Open failed 0x%x\n", __FUNCTION__, Ret);
            break;
        }

        /*get default attribute in order to set attribute next*/
        Ret = HI_UNF_TUNER_GetDeftAttr(i, &TunerAttr);
        if (HI_SUCCESS != Ret)
        {
            sample_common_printf("[%s] HI_UNF_TUNER_GetDeftAttr failed 0x%x\n", __FUNCTION__, Ret);
            break;
        }


        Ret = HIADP_Tuner_GetConfig(i, &TunerAttr);
        if (HI_SUCCESS != Ret)
        {
            sample_common_printf("[%s] HIADP_Tuner_GetConfig failed 0x%x\n", __FUNCTION__, Ret);
            break;
        }
        printf("****************************************************************************\n");
        printf("TunerId=%u, TunerType=%u, TunerAddr=0x%x, I2cChannel=%u, DemodAddr=0x%x, \nDemodType=%u,SigType:%u,OutputMode:%u,ResetGpioNo:%u\n",
                i,
                TunerAttr.enTunerDevType,
                TunerAttr.u32TunerAddr,
                TunerAttr.enI2cChannel,
                TunerAttr.u32DemodAddr,
                TunerAttr.enDemodDevType,
                TunerAttr.enSigType,
                TunerAttr.enOutputMode,
                TunerAttr.u32ResetGpioNo);
        printf("****************************************************************************\n");

        Ret = HI_UNF_TUNER_SetAttr(i, &TunerAttr);
        if (HI_SUCCESS != Ret)
        {
            sample_common_printf("[%s] HI_UNF_TUNER_SetAttr failed 0x%x\n", __FUNCTION__, Ret);
            break;
        }

        g_enSigType[i] = TunerAttr.enSigType;

    }

    for (i = 0; i < HI_TUNER_NUMBER; i++)
    {
        Ret = HIADP_Tuner_GetConfig(i, &TunerAttr);
        if (HI_SUCCESS != Ret)
        {
            sample_common_printf("[%s] HIADP_Tuner_GetConfig failed 0x%x\n", __FUNCTION__, Ret);
            break;
        }

        /* Default use dual freqency, C band, 5150/5750 */
        if (HI_UNF_TUNER_SIG_TYPE_SAT == TunerAttr.enSigType)
        {
#if (HI_TUNER_SIGNAL_TYPE == 2)
            GET_SAT_TUNER_CONFIG(i, SatTunerAttr);
#endif
#if (HI_TUNER1_SIGNAL_TYPE == 2)
            GET_SAT_TUNER1_CONFIG(i, SatTunerAttr);
#endif
#if (HI_TUNER2_SIGNAL_TYPE == 2)
            GET_SAT_TUNER2_CONFIG(i, SatTunerAttr);
#endif
#if (HI_TUNER3_SIGNAL_TYPE == 2)
            GET_SAT_TUNER3_CONFIG(i, SatTunerAttr);
#endif
            Ret = HI_UNF_TUNER_SetSatAttr(i, &SatTunerAttr);
            if (HI_SUCCESS != Ret)
            {
                printf("[%s] HI_UNF_TUNER_SetSatAttr failed 0x%x\n", __FUNCTION__, Ret);
                return Ret;
            }
            LnbConfig.enLNBType = HI_UNF_TUNER_FE_LNB_DUAL_FREQUENCY;
            LnbConfig.enLNBBand = HI_UNF_TUNER_FE_LNB_BAND_C;
            LnbConfig.u32LowLO = 5150;
            LnbConfig.u32HighLO = 5750;
            HI_UNF_TUNER_SetLNBConfig(i, &LnbConfig);
        }
        else if ((HI_UNF_TUNER_SIG_TYPE_DVB_T == TunerAttr.enSigType) ||
                 (HI_UNF_TUNER_SIG_TYPE_DVB_T2 == TunerAttr.enSigType) ||
                 (HI_UNF_TUNER_SIG_TYPE_ISDB_T == TunerAttr.enSigType) ||
                 (HI_UNF_TUNER_SIG_TYPE_DTMB == TunerAttr.enSigType))
        {
#if (HI_TUNER_SIGNAL_TYPE == 4) || (HI_TUNER_SIGNAL_TYPE == 8) || (HI_TUNER_SIGNAL_TYPE == 16)
            GET_TER_TUNER_CONFIG(i, TerTunerAttr);
#endif
#if (HI_TUNER1_SIGNAL_TYPE == 4) || (HI_TUNER1_SIGNAL_TYPE == 8) || (HI_TUNER1_SIGNAL_TYPE == 16)
            GET_TER_TUNER1_CONFIG(i, TerTunerAttr);
#endif
#if (HI_TUNER2_SIGNAL_TYPE == 4) || (HI_TUNER2_SIGNAL_TYPE == 8) || (HI_TUNER2_SIGNAL_TYPE == 16)
            GET_TER_TUNER2_CONFIG(i, TerTunerAttr);
#endif
#if (HI_TUNER3_SIGNAL_TYPE == 4) || (HI_TUNER3_SIGNAL_TYPE == 8) || (HI_TUNER3_SIGNAL_TYPE == 16)
            GET_TER_TUNER3_CONFIG(i, TerTunerAttr);
#endif
            Ret = HI_UNF_TUNER_SetTerAttr(i, &TerTunerAttr);
            if (HI_SUCCESS != Ret)
            {
                printf("[%s] HI_UNF_TUNER_SetTerAttr failed 0x%x\n", __FUNCTION__, Ret);
                return Ret;
            }
        }
        else if ((HI_UNF_TUNER_SIG_TYPE_CAB == TunerAttr.enSigType) ||
             (HI_UNF_TUNER_SIG_TYPE_J83B == TunerAttr.enSigType))
        {
            if(TunerAttr.enDemodDevType == HI_UNF_DEMOD_DEV_TYPE_3138)
            {
#if ((HI_TUNER_SIGNAL_TYPE == 1) || (HI_TUNER_SIGNAL_TYPE == 128))
            GET_CAB_TUNER_CONFIG(i, stCabTunerAttr);
#endif
#if ((HI_TUNER1_SIGNAL_TYPE == 1) || (HI_TUNER1_SIGNAL_TYPE == 128))
            GET_CAB_TUNER1_CONFIG(i, stCabTunerAttr);
#endif
            Ret = HI_UNF_TUNER_SetCabAttr(i, &stCabTunerAttr);
            if (HI_SUCCESS != Ret)
            {
                printf("call HI_UNF_TUNER_SetCabAttr failed.\n");
                return Ret;
            }
            }
        }
        else if(HI_UNF_TUNER_SIG_TYPE_ABSS == TunerAttr.enSigType)
        {
            LnbConfig.enLNBType = HI_UNF_TUNER_FE_LNB_SINGLE_FREQUENCY;
            LnbConfig.enLNBBand = HI_UNF_TUNER_FE_LNB_BAND_KU;
            LnbConfig.u32LowLO = 10750;
            LnbConfig.u32HighLO = 10750;
            HI_UNF_TUNER_SetLNBConfig(i, &LnbConfig);
        }
        
        switch (i)
        {
        case 0:
#ifdef GET_TUNER0_TSOUT_CONFIG
            GET_TUNER0_TSOUT_CONFIG(stTSOut);
            SetTSOut = HI_TRUE;
#endif
            break;

        case 1:
#ifdef GET_TUNER1_TSOUT_CONFIG
            GET_TUNER1_TSOUT_CONFIG(stTSOut);
            SetTSOut = HI_TRUE;
#endif
            break;

        case 2:
#ifdef GET_TUNER2_TSOUT_CONFIG
            GET_TUNER2_TSOUT_CONFIG(stTSOut);
            SetTSOut = HI_TRUE;
#endif
            break;

        case 3:
#ifdef GET_TUNER3_TSOUT_CONFIG
            GET_TUNER3_TSOUT_CONFIG(stTSOut);
            SetTSOut = HI_TRUE;
#endif
            break;
        }

        if (SetTSOut)
        {
            SetTSOut = HI_FALSE;
            Ret = HI_UNF_TUNER_SetTSOUT(i, &stTSOut);
            if (HI_SUCCESS != Ret)
            {
                sample_common_printf("[%s] HI_UNF_TUNER_SetTSOUT failed 0x%x\n", __FUNCTION__, Ret);
                break;
            }
        }
    }

    if (HI_SUCCESS != Ret)
    {
        for (i = 0; i < HI_TUNER_NUMBER; i++)
        {
            HI_UNF_TUNER_Close(i);
        }

        HI_UNF_TUNER_DeInit();
    }

    return Ret;
}

/* Freq:MHZ  SymbolRate:KHZ  */
HI_S32 HIADP_Tuner_Connect(HI_U32 TunerID, HI_U32 Freq, HI_U32 SymbolRate, HI_U32 ThirdParam)
{
    HI_UNF_TUNER_CONNECT_PARA_S  ConnectPara = {0};

    /* DVB-S/S2 demod: AVL6211 / 3136 */
    if (HI_UNF_TUNER_SIG_TYPE_SAT == g_enSigType[TunerID])
    {
        ConnectPara.enSigType = HI_UNF_TUNER_SIG_TYPE_SAT;
        if (ThirdParam >= HI_UNF_TUNER_FE_POLARIZATION_BUTT)
        {
            ThirdParam = HI_UNF_TUNER_FE_POLARIZATION_H;
        }
        ConnectPara.unConnectPara.stSat.enPolar = ThirdParam;
        ConnectPara.unConnectPara.stSat.u32Freq = Freq * 1000;
        ConnectPara.unConnectPara.stSat.u32SymbolRate = SymbolRate * 1000;
        ConnectPara.unConnectPara.stSat.u32ScrambleValue = 0;
    }
    else if(HI_UNF_TUNER_SIG_TYPE_ABSS == g_enSigType[TunerID])
    {
        ConnectPara.enSigType = HI_UNF_TUNER_SIG_TYPE_ABSS;
        if (ThirdParam >= HI_UNF_TUNER_FE_POLARIZATION_BUTT)
        {
            ThirdParam = HI_UNF_TUNER_FE_POLARIZATION_L;
        }
        ConnectPara.unConnectPara.stSat.enPolar = ThirdParam;
        ConnectPara.unConnectPara.stSat.u32Freq = Freq * 1000;
        ConnectPara.unConnectPara.stSat.u32SymbolRate = SymbolRate * 1000;
        ConnectPara.unConnectPara.stSat.u32ScrambleValue = 0;

    }
    /* DVB-C demod */
    else if ((HI_UNF_TUNER_SIG_TYPE_CAB == g_enSigType[TunerID]) || (HI_UNF_TUNER_SIG_TYPE_J83B == g_enSigType[TunerID]))
    {
        ConnectPara.enSigType = HI_UNF_TUNER_SIG_TYPE_CAB;
        ConnectPara.unConnectPara.stCab.bReverse = 0;

        switch (ThirdParam)
        {
            case 16 :
                ConnectPara.unConnectPara.stCab.enModType = HI_UNF_MOD_TYPE_QAM_16;
                break;
            case 32 :
                ConnectPara.unConnectPara.stCab.enModType = HI_UNF_MOD_TYPE_QAM_32;
                break;
            case 64 :
                ConnectPara.unConnectPara.stCab.enModType = HI_UNF_MOD_TYPE_QAM_64;
                break;
            case 128 :
                ConnectPara.unConnectPara.stCab.enModType = HI_UNF_MOD_TYPE_QAM_128;
                break;
            case 256 :
                ConnectPara.unConnectPara.stCab.enModType = HI_UNF_MOD_TYPE_QAM_256;
                break;
            case 512 :
                ConnectPara.unConnectPara.stCab.enModType = HI_UNF_MOD_TYPE_QAM_512;
                break;
            default:
                ConnectPara.unConnectPara.stCab.enModType = HI_UNF_MOD_TYPE_QAM_64;
        }

        ConnectPara.unConnectPara.stCab.u32Freq = Freq * 1000;
        ConnectPara.unConnectPara.stCab.u32SymbolRate = SymbolRate * 1000;
    }
    /* DVB_T/T2... demod */
    else if ((HI_UNF_TUNER_SIG_TYPE_DVB_T == g_enSigType[TunerID])
                || (HI_UNF_TUNER_SIG_TYPE_DVB_T2 == g_enSigType[TunerID])
                || (HI_UNF_TUNER_SIG_TYPE_ISDB_T == g_enSigType[TunerID])
                || (HI_UNF_TUNER_SIG_TYPE_DTMB == g_enSigType[TunerID]))
    {
        ConnectPara.enSigType = g_enSigType[TunerID];
        ConnectPara.unConnectPara.stTer.bReverse = 0;
        ConnectPara.unConnectPara.stTer.enChannelMode = HI_UNF_TUNER_TER_MODE_BASE;
        ConnectPara.unConnectPara.stTer.enDVBTPrio = HI_UNF_TUNER_TS_PRIORITY_HP;

        switch (ThirdParam)
        {
            case 16 :
                ConnectPara.unConnectPara.stTer.enModType = HI_UNF_MOD_TYPE_QAM_16;
                break;
            case 32 :
                ConnectPara.unConnectPara.stTer.enModType = HI_UNF_MOD_TYPE_QAM_32;
                break;
            case 64 :
                ConnectPara.unConnectPara.stTer.enModType = HI_UNF_MOD_TYPE_QAM_64;
                break;
            case 128 :
                ConnectPara.unConnectPara.stTer.enModType = HI_UNF_MOD_TYPE_QAM_128;
                break;
            case 256 :
                ConnectPara.unConnectPara.stTer.enModType = HI_UNF_MOD_TYPE_QAM_256;
                break;
            case 512 :
                ConnectPara.unConnectPara.stTer.enModType = HI_UNF_MOD_TYPE_QAM_512;
                break;
            default:
                ConnectPara.unConnectPara.stTer.enModType = HI_UNF_MOD_TYPE_QAM_64;
        }

        ConnectPara.unConnectPara.stTer.u32Freq = Freq * 1000;
        ConnectPara.unConnectPara.stTer.u32BandWidth = SymbolRate;
    }

    /* connect Tuner*/
    return HI_UNF_TUNER_Connect(TunerID, &ConnectPara, 500);
}


HI_VOID HIADP_Tuner_DeInit(HI_VOID)
{
    HI_U32 i;
    HI_S32 ret;
    HI_U32 MaxI2cNum;
    HI_UNF_TUNER_ATTR_S TunerAttr;

    ret = HI_UNF_I2C_GetCapability(&MaxI2cNum);
    if (HI_SUCCESS != ret)
    {
        sample_common_printf("[%s] HI_UNF_I2C_GetCapability failed 0x%x\n", __FUNCTION__, ret);
        return;
    }

    for (i = 0; i < HI_TUNER_NUMBER; i++)
    {
        ret = HI_UNF_TUNER_GetAttr(i, &TunerAttr);
        if (HI_SUCCESS != ret)
        {
            sample_common_printf("[%s] HI_UNF_TUNER_GetAttr failed 0x%x\n", __FUNCTION__, ret);
        }
        else
        {
            if (TunerAttr.enI2cChannel >= MaxI2cNum)
            {
                HI_UNF_I2C_DestroyGpioI2c(TunerAttr.enI2cChannel);
            }
        }

        HI_UNF_TUNER_Close(i);
    }

    HI_UNF_I2C_DeInit();
    
    HI_UNF_TUNER_DeInit();
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