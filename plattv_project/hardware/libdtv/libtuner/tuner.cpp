
/* ----------------------------------------------------------------------------
 * @module 
 *			HAL module 
 * @routines 
 *			tuner device 
 * @date
 *			2011.3
 */
//#define LOG_NDEBUG 0
#define LOG_TAG "[plattv]tuner"

#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <cutils/log.h>
#include <cutils/properties.h>

#include <hardware/tuner.h>
#include "hi_type.h"
#include "hi_common.h"
#include "hi_unf_ecs.h"
#include "hi_unf_demux.h"
#include "hi_unf_sound.h"
#include "hi_unf_pdm.h"
#include "hi_unf_mce.h"

#include "hal_common.h"

static struct tuner_context_t *m_tuner_ctx = NULL;
#define IO_SHARE_BASE (0xf8a21000)

/**
 * Common hardware methods
 */
static void cfgRegister_GK7661F(void);
static void resetTuner_GK7661F(HI_U32 rstGpioNo);

static int open_tuner(const struct hw_module_t* module, const char* name,
		struct hw_device_t** device);

static int tuner_get_list(struct tuner_module_t *module, struct tuner_t const **list);

static struct hw_module_methods_t tuner_module_methods = {
	open: open_tuner
};

/*
 * The TUNER Module
 */
struct tuner_module_t HAL_MODULE_INFO_SYM = {
	common: {
		tag: HARDWARE_MODULE_TAG,
		version_major: 1,
		version_minor: 0,
		id: TUNER_HARDWARE_MODULE_ID,
		name: "TUNER Module",
		author: "iPanel 2011, Inc.",
		methods: &tuner_module_methods
	},
	get_tuner_list:tuner_get_list,
};

static bool mMceVideoPlay = 0;
static pthread_t mMceThread;

bool hasVideoCome(void)
{
    char buffer[PROPERTY_VALUE_MAX];
    FILE* file;
    bool ret = false;
    char *p = NULL;

    file = fopen("/proc/msp/sync00", "r");
    if (file == NULL) {
        return ret;
    }

    while ( fgets(buffer, sizeof(buffer), file)>0 ){
        if ( p = strstr(buffer, "VidFirstCome") ){
            while ( ':'!=*p && 0x0d!=*p) p++;
            if (':'==*p && '1'==(*(p+1))) ret = true;
            break;
        }
        memset(buffer, 0, sizeof(buffer));
    }
    fclose(file);
    return ret;
}

// ---------------------------------------------------------------------------
static void *mce_proc(void*args)
{
	HI_U32 ret ;
	HI_U32 u32BER[3] = { 0 };
	HI_U32 u32SignalSNR = 0;
	HI_U32 u32SignalStrength = 0;
	HI_U32 u32SignalQuality = 0;
	while(1){	
			ret = HI_UNF_TUNER_GetBER(0, u32BER);
			if (HI_SUCCESS != ret) {
					LOGE("HI_UNF_TUNER_GetBER,Ret=%x", ret);
					//return ;
			}

			LOGI("u32BER[0]: 0x%x, u32BER[1]: 0x%x, u32BER[2]: 0x%x\n", u32BER[0], u32BER[1], u32BER[2]);
			usleep(300000);
			ret = HI_UNF_TUNER_GetSNR(0, &u32SignalSNR);
			if (HI_SUCCESS != ret) {
					LOGE("HI_UNF_TUNER_GetSNR,Ret=%x", ret);
					//return -1;
			}
			LOGI("u32SignalSNR  : %d \n",u32SignalSNR);
			usleep(300000);
			ret = HI_UNF_TUNER_GetSignalStrength(0, &u32SignalStrength);
			if (HI_SUCCESS != ret) {
						LOGE("HI_UNF_TUNER_GetSignalStrength,Ret=%x", ret);
					//return -1;
			}
			LOGI("u32SignalStrength  : %d \n",u32SignalStrength);
			usleep(300000);
	}
#if 0
    FILE * fp;
    unsigned int flag0, flag1, bootAnimExit=0, fastplayVideoCome=0, i;
    char value[PROPERTY_VALUE_MAX];
    char *p;
    HI_UNF_SND_GAIN_ATTR_S stGain;
    HI_UNF_PDM_SOUND_PARAM_S stSound;
    HI_UNF_MCE_STOPPARM_S stStopParam;
    int count=600;
    int Ret = -1;
    
    memset(&stStopParam, 0, sizeof(HI_UNF_MCE_STOPPARM_S));
    HI_UNF_MCE_Init (NULL);
    if ( !mMceVideoPlay ){
        HI_UNF_MCE_Stop (&stStopParam);
        HI_UNF_MCE_Exit (NULL);
        goto update_play_param;
    }
    flag0 = flag1 = 0;
    while (count--){
        system("echo hide > /proc/msp/hifb0");
        //property_get("service.bootanim.exit", value, "0");
		property_get("sys.ipanel.home_started", value, "0");
        bootAnimExit = atoi(value);
        fp = fopen("/proc/msp/sync00", "r");
        if ( !fp ){
            ALOGE("fopen /proc/msp/sync00 error");
            break;
        }
        while ( fgets(value, sizeof(value), fp)>0 ){
            if ( p = strstr(value, "VidLastPts") ){
                while ( ':'!=*p && 0x0d!=*p) p++;
                if (':'==*p){
                    if ( 0==flag0 )
                        flag0 = flag1 = atoi(p+1);
                    else
                        flag1 = atoi(p+1);
                }
                break;
            }
            memset(value, 0, sizeof(value));
        }
        fclose(fp);
        ALOGD("bootanimaexit:%d, pts0:%u, pts1:%u", bootAnimExit, flag0, flag1);
#if 0		
        if ( bootAnimExit && (flag1-flag0>10000 || flag0==flag1))
            break;
#else
        if ( bootAnimExit)
            break;
#endif
        flag0 = flag1;
        usleep(100000);
    }
	//sleep(3);
    //system("echo stoponly time 0 > /proc/msp/mce");
    stStopParam.enStopMode = HI_UNF_AVPLAY_STOP_MODE_BLACK;
    HI_UNF_MCE_Stop (&stStopParam);
    LOGD("stop mce");
    flag0 = flag1 = 0;
    while (1){
        if (flag0 && flag1 ) break; //fps exist,can show fb now
        fp = fopen("/proc/msp/hifb0", "r");
        if ( !fp ){
            ALOGE("fopen /proc/msp/hifb0 error");
            break;
        }
        while ( fgets(value, sizeof(value), fp)>0 ){
            if ( p = strstr(value, "Fps") ){
                while ( ':'!=*p && 0x0d!=*p) p++;
                if (':'==*p){
                    flag1 = atoi(p+1);
                    break;
                }
            }
            memset(value, 0, sizeof(value));
        }
        fclose(fp);
        flag0 = flag1;
        usleep(100000);
    }
    system("echo show > /proc/msp/hifb0");
    //system("echo exit > /proc/msp/mce");
    HI_UNF_MCE_Exit (NULL);
    
    mMceVideoPlay = 0;
    LOGD("exit mce");

update_play_param:
    Ret = HI_UNF_PDM_GetBaseParam (HI_UNF_PDM_BASEPARAM_SOUND0, &stSound);
    for (i=0;i<stSound.u32PortNum;i++)
        LOGD("au32Volume =%d", stSound.au32Volume[i]);

    while ( !Ret ){
        HI_UNF_SND_GetVolume (HI_UNF_SND_0, HI_UNF_SND_OUTPUTPORT_HDMI0, &stGain);
        if ( stSound.au32Volume[0] != stGain.s32Gain ){
            LOGD("mce Volume=%d to %d", stSound.au32Volume[0], stGain.s32Gain);
            for (i=0;i<stSound.u32PortNum;i++)
                stSound.au32Volume[i]=(HI_U32)stGain.s32Gain;
            Ret = HI_UNF_PDM_UpdateBaseParam (HI_UNF_PDM_BASEPARAM_SOUND0, &stSound);
            if ( Ret ) //reset if failed
                stSound.au32Volume[0] = -1;
            Ret = 0; //do not break out
        }
        usleep(1000000);
    }
    
    LOGD("mce no fastplay,exit thread");
#endif
    return NULL;
}

/******************************************************************************/

static int tuner_is_locked(struct tuner_device_t *dev, int handle, int *status, int *fd)
{
    LOGV("is_locked, handle=%x, status=%x, fd=%x", handle, status, fd);

	HI_S32 ret = HI_FAILURE;
	HI_UNF_TUNER_STATUS_S lockStatus;
    mtuner_t *mt = (mtuner_t *)handle;

    ret = HI_UNF_TUNER_GetStatus(mt->id, &lockStatus);
	if (HI_SUCCESS == ret) {
		if (lockStatus.enLockStatus == HI_UNF_TUNER_SIGNAL_LOCKED) {
			(*status) = 1;
			property_set("lock.stats", "1");
		} else {
			(*status) = 0;
			property_set("lock.stats", "0");
		}
		return 0;
	} else {
		(*status) = 0;
	}

	return -1;
}

static int tuner_get_signal_status(struct tuner_device_t *dev, int handle, tuner_signal_status_t*st)
{
    LOGV("get_signal_status, handle=%x, st=%x", handle, st);

	HI_S32 ret = HI_FAILURE;
	HI_U32 u32SignalSNR = 0;
	HI_U32 u32SignalStrength = 0;
	HI_U32 u32SignalQuality = 0;
	HI_U32 u32BER[3] = { 0 };
	HI_U32 u32low, u32high;
	HI_U32 u32SignalBER = 0;
	HI_DOUBLE sigber = 0.0;
	HI_U32 i = 0;
    mtuner_t *mt = (mtuner_t *)handle;

	ret = HI_UNF_TUNER_GetBER(mt->id, u32BER);
	if (HI_SUCCESS != ret) {
		LOGE("HI_UNF_TUNER_GetBER,Ret=%x", ret);
		return -1;
	}

	/*LOGI("[tuner_get_signal_status] u32BER[0]: 0x%x, u32BER[1]: 0x%x, u32BER[2]: 0x%x\n", u32BER[0], u32BER[1], u32BER[2]);*/

	ret = HI_UNF_TUNER_GetSNR(mt->id, &u32SignalSNR);
	if (HI_SUCCESS != ret) {
		LOGE("HI_UNF_TUNER_GetSNR,Ret=%x", ret);
		return -1;
	}

	ret = HI_UNF_TUNER_GetSignalStrength(mt->id, &u32SignalStrength);
	if (HI_SUCCESS != ret) {
		LOGE("HI_UNF_TUNER_GetSignalStrength,Ret=%x", ret);
		return -1;
	}

	u32SignalStrength = u32SignalStrength > 100 ? 100 : u32SignalStrength;

	u32SignalQuality = u32SignalSNR + 55;
	u32SignalQuality = u32SignalQuality > 100 ? 100 : u32SignalQuality;

	u32SignalSNR = u32SignalSNR + 20;
	u32SignalSNR = u32SignalSNR > 100 ? 100 : u32SignalSNR;

	i = u32BER[2];
	sigber = u32BER[0] + u32BER[1] / 1000;
	while (i != 0) {
		sigber *= 10;
		i--;
	}

	st->ber = (float) sigber;
	st->snr = u32SignalSNR;
	st->quality = u32SignalQuality;
	st->strength = u32SignalStrength;
	st->level = u32SignalStrength;

	LOGD("get_signal_status %d,%d,%d,%d,%f", st->snr, st->quality, st->strength, st->level,
			st->ber);

	return 0;
}

static int tuner_set_singal_mode(struct tuner_device_t *dev, int handle, int mode)
{
    LOGD("set_singal_mode, handle=%x, mode=%d", handle, mode);
    
	struct tuner_context_t* ctx = (struct tuner_context_t*) dev;

	if (TUNER_SIGNAL_MODE_DIGITAL != mode) {
		LOGE("[tuner_set_singal_mode] not suport mode(%d)", mode);
	}

	return 0;
}

static const char* get_for_name(char**params, const char*name) {
	char**p = params;
	LOGI("get_for_name name=%s\n", name);
	while (*p) {
		if (strcmp(*p, name) == 0) {
			++p;
			LOGI("get the value p =%s\n", *p);
			return *p;
		}

		if (++p == NULL)
			return NULL;
		p++;
	}
	return NULL;
}

static int get_symbol_rate(char**params, HI_U32*sr) {
	const char*p = get_for_name(params, "symbol_rate");
	LOGI("get_symbol_rate p=%s\n", p);
	if (p) {
		*sr = atoi(p);
		return 0;
	}
	return -1;
}

static int get_frequency(char**params, long long*freq) {
	const char*p = get_for_name(params, "frequency");
	LOGI("get_frequency p=%s\n", p);
	if (p) {
		*freq = atoll(p);
		return 0;
	}
	return -1;
}

static int get_modulation(char**params, HI_U32*qam) {
	const char*p = get_for_name(params, "modulation");
	if (!p)
		return -1;
	if (strcmp(p, "qam16") == 0)
		*qam = HI_UNF_MOD_TYPE_QAM_16;
	else if (strcmp(p, "qam32") == 0)
		*qam = HI_UNF_MOD_TYPE_QAM_32;
	else if (strcmp(p, "qam64") == 0)
		*qam = HI_UNF_MOD_TYPE_QAM_64;
	else if (strcmp(p, "qam128") == 0)
		*qam = HI_UNF_MOD_TYPE_QAM_128;
	else if (strcmp(p, "qam256") == 0)
		*qam = HI_UNF_MOD_TYPE_QAM_256;
	else if (strcmp(p, "qam512") == 0)
		*qam = HI_UNF_MOD_TYPE_QAM_512;
	else if (strcmp(p, "bpsk") == 0)
		*qam = HI_UNF_MOD_TYPE_BPSK;
	else if (strcmp(p, "qpsk") == 0)
		*qam = HI_UNF_MOD_TYPE_QPSK;
	else
		return -1;
	return 0;
}

#define MAX_PARAM_NUM 80
static inline int to_lower_case(char*f) {
	char c;
	while ((c = *++f) != 0) {
		if (isspace((unsigned char) c))
			return -1;
		*f = tolower((unsigned char) c);
	}
	return 0;
}

static int tuner_param_check(char*f, char**t) {
	int n = 0;
	char*p, *pv, **pp = t;
	if (to_lower_case(f) != 0) {
		LOGE("tvss_tuner_param_check to_lower_case failed");
		return -1;
	}
	p = f;
	while (*p != '?')
		p++;
	p++;
	*t++ = p;
	while (*p != 0) {
		if ((*p == '=' || *p == '&')) {
			*p = 0;
			*t++ = ++p;
			if (++n > MAX_PARAM_NUM)
				return -1;
		} else {
			p++;
		}
	}
	t[0] = t[1] = 0;
	return 0;
}

int utils_uri_get_str_param(const char*s, const char*name, char*buf, int len) {
	int n;
	char str[128], *p;
	if (strlen(name) > 120)
		return -1;
	n = sprintf(str, "%s=", name);
	if ((s = strstr(s, str))) {
		s += n;
		n = ((p = strchr(s, '&')) != NULL) ? p - s : (int) strlen(s);
		if (len < n)
			return -1;
		memcpy(buf, s, n);
		buf[n] = 0;
		return n;
	}
	return -1;
}

int utils_uri_get_int_param(const char*s, const char*name, int*value) {
	if ((s = strstr(s, name)) == NULL)
		return 0;
	s += strlen(name);
	return (sscanf(s, "=%d", value) == 1) ? 0 : -1;
}

int utils_uri_get_int64_param(const char*s, const char*name, int64_t*value) {
	if ((s = strstr(s, name)) == NULL)
		return 0;
	s += strlen(name);
	return (sscanf(s, "=%lld", value) == 1) ? 0 : -1;
}

int utils_uri_get_s2i_param(const char*s, const char*n, const char**enu) {
	int i = 0;
	char buf[256];
	if (utils_uri_get_str_param(s, n, buf, sizeof(buf)) > 0) {
		for (i = 0; *enu; i++, enu++) {
			if (strcmp(buf, *enu) == 0) {
				return i;
			}
		}
	}
	return -1;
}

int utils_uri_get_int64_auth(const char*s, long long *auth) {
	if ((s = strstr(s, "://")) == NULL)
		return 0;
	return (sscanf(s, "://%lld", auth) == 1) ? 0 : -1;
}

static int parse_uri(const char*uri, int* freq, int *sr, int* m) {
	int i;
	long long f = 0;
	static const char*e[] = { "qam16", "qam32", "qam64", "qam128", "qam256", "qam512", "bpsk",
			"qpsk","QAM16", "QAM32", "QAM64", "QAM128", "QAM256", "QAM512", "BQSK",
			"QPSK", NULL };
	static int qam[] = { HI_UNF_MOD_TYPE_QAM_16, HI_UNF_MOD_TYPE_QAM_32, HI_UNF_MOD_TYPE_QAM_64,
			HI_UNF_MOD_TYPE_QAM_128, HI_UNF_MOD_TYPE_QAM_256, HI_UNF_MOD_TYPE_QAM_512,
			HI_UNF_MOD_TYPE_BPSK, HI_UNF_MOD_TYPE_QPSK };
	if (utils_uri_get_int64_auth(uri, &f) != 0){
		LOGD("zhangzhy 11");
		return -1;
	}
	if (freq)
		*freq = (int) (f / 1000);
	if (utils_uri_get_int_param(uri, "symbol_rate", sr) != 0){
			LOGD("zhangzhy 22");
		return -1;
	}
	if ((i = utils_uri_get_s2i_param(uri, "modulation", e)) < 0){
			LOGD("zhangzhy 33");
		return -1;
	}
	*m = qam[i < 8?i:i-8];
	return 0;
}
// "symbol_rate" "3876000" "modulation" "qam64" NULL，NULL

/**
 * 最重要的三个参数：上层传下的参数值：频频单位hz,符号率基本就是 6875000,QAM一般为QAM64
 */

static int tuner_lock_delivery(struct tuner_device_t *dev, int handle, long long freq, const char*params)
{
	LOGD("lock_delivery, handle=%x, freq=%lld, params = %s", handle, freq, params);
    
	HI_S32 ret = HI_FAILURE;
    mtuner_t *mt = (mtuner_t *)handle;
	HI_U32  GpioI2cNum = 0; 
	static HI_U8 CalledOnce = 1;

#if 0
    while ( mMceVideoPlay ){
        LOGD("mce video playing, do not change freq");
        usleep(10000);
        continue;
    }
#endif
    
 /* I2C  放到open*/   
#if 0  
	if(CalledOnce)
	{
		#if 0
	    HI_UNF_I2C_Init();
	    ret = HI_UNF_I2C_CreateGpioI2c(&GpioI2cNum, 12, 11);
	    if (HI_SUCCESS != ret)
	    {
	        LOGE("[%s][%d] HI_UNF_I2C_CreateGpioI2c err ret=0x%x\n",__FUNCTION__,__LINE__,ret);
	        return HI_FAILURE;
	    }
		LOGD("GpioI2cNum:%x\n",GpioI2cNum);
	    //attr.enI2cChannel = GpioI2cNum;
	 #endif
	 	

	HI_UNF_GPIO_Init();
  HI_UNF_I2C_Init();
	 
		CalledOnce = 0;
	}
	
	
	HI_UNF_I2C_SetRate(1, HI_UNF_I2C_RATE_100K);
	HI_UNF_I2C_SetRate(2, HI_UNF_I2C_RATE_100K);
 
#endif
    if ( 0==mt->isopen ){
        HI_UNF_TUNER_ATTR_S stTunerAttr;
        ret = HI_UNF_TUNER_Open(mt->id);
        if (HI_SUCCESS != ret) {
            LOGE("HI_UNF_TUNER_Open,Ret=%x", ret);
            return -1;
        }
        HI_UNF_TUNER_GetAttr(mt->id, &stTunerAttr);


        if(0==mt->id){            
	       		 	stTunerAttr.enSigType = HI_UNF_TUNER_SIG_TYPE_CAB/*HI_UNF_TUNER_SIG_TYPE_SAT*/;
							stTunerAttr.enTunerDevType = HI_UNF_TUNER_DEV_TYPE_MXL254;
							stTunerAttr.u32TunerAddr = 0x50 << 1;
							stTunerAttr.enDemodDevType = HI_UNF_DEMOD_DEV_TYPE_MXL254;
							stTunerAttr.u32DemodAddr = 0x50 << 1;
							stTunerAttr.enI2cChannel = 2;
							stTunerAttr.u32ResetGpioNo = 6*8+2;
							stTunerAttr.enOutputMode = HI_UNF_TUNER_OUTPUT_MODE_DEFAULT;
        }else if(1==mt->id){   
           LOGI(" only one tuner \n");
        }


    
#if 1
		LOGD("HI_UNF_TUNER_SetAttr:\n");
		LOGD("stTunerAttr.enSigType=%x\n",stTunerAttr.enSigType);
		LOGD("stTunerAttr.enTunerDevType=%x\n",stTunerAttr.enTunerDevType);
		LOGD("stTunerAttr.u32TunerAddr=%x\n",stTunerAttr.u32TunerAddr);
		LOGD("stTunerAttr.enDemodDevType=%x\n",stTunerAttr.enDemodDevType);
		LOGD("stTunerAttr.u32DemodAddr=%x\n",stTunerAttr.u32DemodAddr);
		LOGD("stTunerAttr.enOutputMode=%x\n",stTunerAttr.enOutputMode);
		LOGD("stTunerAttr.enI2cChannel=%x\n",stTunerAttr.enI2cChannel);
		LOGD("stTunerAttr.u32ResetGpioNo=%x\n",stTunerAttr.u32ResetGpioNo);
#endif
        ret = HI_UNF_TUNER_SetAttr(mt->id, &stTunerAttr);
        if (HI_SUCCESS != ret) {
            LOGE("HI_UNF_TUNER_SetAttr,Ret=%x", ret);
            return -1;
        }
        mt->isopen = 1;
    }
    
	int qam, freqhz, symbolrate;
	HI_UNF_TUNER_CONNECT_PARA_S ConnectPara;
	memset(&ConnectPara, 0, sizeof(HI_UNF_TUNER_CONNECT_PARA_S));
	ret = parse_uri(params, NULL, &symbolrate, &qam);
	if (ret != 0) {
		LOGD("[parse_uri] fail");
		return -1;
	}

	freqhz = freq / 1000;	
	
	/* 设置参数时注意其单位转换化与低层一致*/
	ConnectPara.enSigType = HI_UNF_TUNER_SIG_TYPE_CAB;
	ConnectPara.unConnectPara.stCab.bReverse = (HI_BOOL) 0;
	ConnectPara.unConnectPara.stCab.u32Freq = freqhz;
	ConnectPara.unConnectPara.stCab.u32SymbolRate = symbolrate;
	ConnectPara.unConnectPara.stCab.enModType = (HI_UNF_MODULATION_TYPE_E) qam;
	
	LOGI("HI_UNF_TUNER_SIG_TYPE_CAB : %d \n",HI_UNF_TUNER_SIG_TYPE_CAB);
	LOGI("======= hardware lock param(%d,%d,%d)", freqhz, symbolrate, qam);


    
    static int flag = 1; //attach ts port first time lock,cause mce exit make it detach
    if ( flag && 0!=mt->demux ){
        mdemux_t *dm = (mdemux_t *)mt->demux;
        HI_UNF_DMX_AttachTSPort(dm->dmxid, (HI_UNF_DMX_PORT_E)dm->port);
        flag = 0;
		
   	 	LOGD("HI_UNF_DMX_AttachTSPort, dm->dmxid:%x",dm->dmxid);
    	LOGD("HI_UNF_DMX_AttachTSPort, dm->port:%x",dm->port);
    }

	ret = HI_UNF_TUNER_Connect(mt->id, &ConnectPara, 0);
    if (HI_SUCCESS != ret) {
        LOGE("HI_UNF_TUNER_Connect,Ret=%x", ret);
        return -1;
    }
	
    LOGD("HI_UNF_TUNER_Connect ok");
	return 0;
}

static int tuner_ioctl(struct tuner_device_t *dev, int handle, int op, struct io_block_t*iob) {
	//TODO
	return -1;
}

static int tuner_get_list(struct tuner_module_t *module, struct tuner_t const **list)
{
	int i = 0, j;
    
	tuner_context_t *ctx = m_tuner_ctx;
    
    if ( !ctx ){
        ctx = m_tuner_ctx = (tuner_context_t *)calloc(1, sizeof(tuner_context_t));
        if ( !ctx ){
            LOGE("calloc ctx failed");
            return -1;
        }
    }
    if ( !ctx->list ){
    	ctx->list = (struct tuner_t*) calloc(1, sizeof(struct tuner_t) * TUNER_MAX_NUM);
    	if ( !ctx->list ) {
            LOGE("calloc list failed");
    		return -1;
    	}
        for (i = 0; i < TUNER_MAX_NUM; i++) {
            ctx->tuner[i].id = i;
            ctx->list[i].handle = (int)&ctx->tuner[i];
            ctx->list[i].deliveryType = TUNER_DELIVERY_CABLE;
            ctx->list[i].minFreq = TUNER_MIN_FREQUENCY;
            ctx->list[i].maxFreq = TUNER_MAX_FREQUENCY;
            ctx->list[i].signalMode = TUNER_SIGNAL_MODE_DIGITAL;
            ctx->list[i].interfaceId = i; //仅有一个接口
            ctx->list[i].fullbandSize = 0;
            ctx->list[i].isFullband = 0;
            ctx->list[i].isRemote = 0;
    		LOGD("get_list, %d,%x", i, ctx->list[i].handle);
        }
    }

    (*list) = ctx->list;
	LOGD("get_list num=%d", TUNER_MAX_NUM);

	return TUNER_MAX_NUM;
}

/*****************************************************************************/

static int close_tuner(struct hw_device_t *dev)
{
    LOGD("close");
	int ret = 0, i;
	struct tuner_context_t* ctx = (struct tuner_context_t*) dev;
	if (ctx == NULL) {
		LOGE("[close_tuner] null ctx.\n");
		return -1;
	}
    //do close imp
	if ( ctx->list ) {
		free(ctx->list);
		ctx->list = NULL;
	}
	if ( ctx ) {
		free(ctx);
		ctx = NULL;
	}
    m_tuner_ctx = NULL;
    HI_UNF_TUNER_DeInit ();

	return ret;

}

static int open_tuner(const struct hw_module_t* module, const char* name,
		struct hw_device_t** device)
{
    LOGD("open_tuner");
    int ret;
	tuner_context_t *ctx = m_tuner_ctx;

	if (!strcmp(name, TUNER_HARDWARE_TUNER0)) {
        if ( !ctx ){
            ctx = m_tuner_ctx = (tuner_context_t *)calloc(1, sizeof(tuner_context_t));
            if ( !ctx ){
                LOGE("calloc ctx failed");
                return -1;
            }
        }
		ctx->device.common.tag = HARDWARE_DEVICE_TAG;
		ctx->device.common.version = 1;
		ctx->device.common.module = const_cast<hw_module_t*>(module);
		ctx->device.common.close = close_tuner;

		ctx->device.is_locked = tuner_is_locked;
		ctx->device.lock_delivery = tuner_lock_delivery;
		ctx->device.set_singal_mode = tuner_set_singal_mode;
		ctx->device.get_signal_status = tuner_get_signal_status;
		ctx->device.ioctl = tuner_ioctl;

	  HI_SYS_Init();

		cfgRegister_GK7661F();
	
   HI_UNF_I2C_Init();
  	
   HI_UNF_I2C_SetRate(1, HI_UNF_I2C_RATE_100K);
  	
	 HI_UNF_I2C_SetRate(2, HI_UNF_I2C_RATE_100K);
	do
	{
		resetTuner_GK7661F(6*8+2);
	} while (0);
	
		ret = HI_UNF_TUNER_Init();
		if (ret != HI_SUCCESS) {
			LOGE("HI_UNF_TUNER_Init,Ret=%x", ret);
			return -1;
		}
		
#if 1
     //   mMceVideoPlay = hasVideoCome ();
      //  if ( mMceVideoPlay )
      //  {
            pthread_create(&mMceThread, NULL, mce_proc, (void*)NULL);
       // }
        //system("echo stoponly time 0 > /proc/msp/mce");
        //system("echo exit > /proc/msp/mce");
#endif

		if (ret == 0) {
			*device = &ctx->device.common;
		} else {
			close_tuner(&ctx->device.common);
			return -1;
		}
        
		LOGI("[open_tuner] open succ!\n");
		return 0;
	} else {
		LOGE("[open_tuner] error name.\n");
		ret = -1;
	}

	BAIL: LOGE("[open_tuner] open fail!\n");
	return ret;
}

/* 湖南克拉视频 tuner 配置*/
static void resetTuner_GK7661F(HI_U32 rstGpioNo)
{
	HI_S32 Ret;

    Ret = HI_UNF_GPIO_SetDirBit(rstGpioNo, HI_FALSE);
	if(HI_SUCCESS != Ret)
	{
		LOGI("%s[%d] <gpio> Err\n", __func__, __LINE__);
	}
	
	Ret = HI_UNF_GPIO_WriteBit(rstGpioNo, HI_FALSE);

	usleep(100*1000);
	Ret = HI_UNF_GPIO_WriteBit(rstGpioNo, HI_TRUE);
		if(HI_SUCCESS != Ret)
	{
		LOGI("%s[%d] <gpio> Err\n", __func__, __LINE__);
	}
	usleep(100*1000);
}

static void cfgRegister_GK7661F(void)
{
	/*GK7661F Frontend 信息*/
	HI_U32 val;
	HI_U32 Ret;
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

	Ret = HI_SYS_ReadRegister(IO_SHARE_BASE+0x0A8, &val);//GPIO6_2

	val = (val&0xfffffff8);
	Ret = HI_SYS_WriteRegister(IO_SHARE_BASE+0x0A8, val);

	Ret = HI_SYS_ReadRegister(IO_SHARE_BASE+0x0A4, &val); //GPIO6_1/I2C2_SDA
	val = (val&0xfffffff8)|0x5;
	Ret = HI_SYS_WriteRegister(IO_SHARE_BASE+0x0A4, val);

	Ret = HI_SYS_ReadRegister(IO_SHARE_BASE+0x0A0, &val); //GPIO6_0/I2C2_SCL
	val = (val&0xfffffff8)|0x5;
	Ret = HI_SYS_WriteRegister(IO_SHARE_BASE+0x0A0, val);

	Ret = HI_SYS_ReadRegister(IO_SHARE_BASE+0x09C, &val); //GPIO4_7/TSI0_VALID
	val = (val&0xfffffff8)|0x5;
	Ret = HI_SYS_WriteRegister(IO_SHARE_BASE+0x09C, val);

	Ret = HI_SYS_ReadRegister(IO_SHARE_BASE+0x098, &val); //GPIO4_6/TSI0_CLK
	val = (val&0xfffffff8)|0x5;
	Ret = HI_SYS_WriteRegister(IO_SHARE_BASE+0x098, val);

	Ret = HI_SYS_ReadRegister(IO_SHARE_BASE+0x094, &val); //GPIO4_5/TSI0_D0
	val = (val&0xfffffff8)|0x5;
	Ret = HI_SYS_WriteRegister(IO_SHARE_BASE+0x094, val);

	Ret = HI_SYS_ReadRegister(IO_SHARE_BASE+0x090, &val); //GPIO4_4/TSI0_D1
	val = (val&0xfffffff8)|0x5;
	Ret = HI_SYS_WriteRegister(IO_SHARE_BASE+0x090, val);

	Ret = HI_SYS_ReadRegister(IO_SHARE_BASE+0x08C, &val); //GPIO4_3/TSI0_D2
	val = (val&0xfffffff8)|0x5;
	Ret = HI_SYS_WriteRegister(IO_SHARE_BASE+0x08C, val);

	Ret = HI_SYS_ReadRegister(IO_SHARE_BASE+0x05C, &val); //GPIO2_7/TSI0_D3
	val = (val&0xfffffff8)|0x5;
	Ret = HI_SYS_WriteRegister(IO_SHARE_BASE+0x05C, val);

	Ret = HI_SYS_ReadRegister(IO_SHARE_BASE+0x058, &val); //GPIO2_6/TSI0_D4
	val = (val&0xfffffff8)|0x5;
	Ret = HI_SYS_WriteRegister(IO_SHARE_BASE+0x058, val);

	Ret = HI_SYS_ReadRegister(IO_SHARE_BASE+0x054, &val); //GPIO2_5/TSI0_D5
	val = (val&0xfffffff8)|0x5;
	Ret = HI_SYS_WriteRegister(IO_SHARE_BASE+0x054, val);

	Ret = HI_SYS_ReadRegister(IO_SHARE_BASE+0x050, &val); //GPIO2_4/TSI0_D6
	val = (val&0xfffffff8)|0x5;
	Ret = HI_SYS_WriteRegister(IO_SHARE_BASE+0x050, val);

	Ret = HI_SYS_ReadRegister(IO_SHARE_BASE+0x04C, &val); //GPIO2_3/TSI0_D7
	val = (val&0xfffffff8)|0x5;
	Ret = HI_SYS_WriteRegister(IO_SHARE_BASE+0x04C, val);

}