/* ----------------------------------------------------------------------------
 * @module 
 *			HAL module 
 * @routines 
 *			smartcard device 
 * @date
 *			2011.9
 */

#define LOG_TAG "[plattv]smartcard"

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
#include <cutils/properties.h>

#include <linux/android_pmem.h>
#include <hardware/smartcard.h>

#include "hi_type.h"
#include "hi_common.h"
#include "hi_unf_sci.h" //yanglb
#include "smartcard.h"
#include "hal_common.h"
#define SMARTCARD_VIRTUAL_BASE_HANDLE	(0x120)
#define SMARTCARD_MAX_NUM				(1)
#define MUTED_TIMEOUT 1000
#define MAX_PROTOCOL_NUM                (3)
/* 检查指针是否有效 */

#define SMARTCARD_CHECK_VALID_POINTER(ptr) \
do{ if(ptr==NULL){                     \
		LOGE("SMARTCARD:invalid pointer(%s)\n",__FUNCTION__);\
		return -1; \
	}                 \
}while(0)

/* 检查句柄是否有效 */

#define SMARTCARD_CHECK_VALID_TUNERID(u32smartcardId)\
do{ if((u32smartcardId>=SMARTCARD_MAX_NUM)||(u32smartcardId==-1)){ \
		LOGE("SMARTCARD:invalid tuner id(%s).\n",__FUNCTION__);\
		return -1; \
	}                 \
}while(0)

typedef struct smartcard_context_t {
	struct smartcard_device_t device;
	int smartcardHandle[SMARTCARD_MAX_NUM];

	//...
} smartcard_context_t;
static HI_UNF_SCI_PROTOCOL_E g_proto[MAX_PROTOCOL_NUM] =
		{ HI_UNF_SCI_PROTOCOL_T0, HI_UNF_SCI_PROTOCOL_T1,
				HI_UNF_SCI_PROTOCOL_T14 };
typedef struct smc_working_mode_t {
	HI_UNF_SCI_PROTOCOL_E proto;
	HI_UNF_SCI_MODE_E clkMode;
	HI_UNF_SCI_LEVEL_E vccen;
	HI_UNF_SCI_LEVEL_E detect;
} smc_working_mode;
typedef enum CA_VENDER{
	CA_TYPE_TIANBAI = 0,
	CA_TYPE_OTHER
}CA_VENDER_E;
static struct smartcard_t *m_psmartcardList = NULL;
static struct smartcard_context_t *m_smartcard_ctx = NULL;
static int m_max_smartcard = 0;
static HI_UNF_SCI_PORT_E g_u8SCIPort = HI_UNF_SCI_PORT0;
CA_VENDER_E ca_vender = CA_TYPE_OTHER;
static struct {
	int reset_once;
	int muted;
	long long present_time_millis;
	int creatChannel;
} g_cardstatus[8];

char g_recv_Buffer[256] = { 0 };
unsigned int g_recv_len = 0;
static smc_working_mode g_smc_working_mode[SMARTCARD_MAX_NUM];

/**
 * Common hardware methods
 */

#ifndef PROJ_DALIAN_JIULIAN
#define PROJ_DALIAN_JIULIAN
#endif
//#ifndef PROJ_HISI_BOARD
//#define PROJ_HISI_BOARD
//#endif

static int open_smartcard(const struct hw_module_t* module, const char* name,
		struct hw_device_t** device);

static int smartcard_get_list(struct smartcard_module_t *module,
		struct smartcard_t const **list);

static struct hw_module_methods_t smartcard_module_methods = {
	open: open_smartcard
};
/*
 * The smartcard Module
 */
struct smartcard_module_t HAL_MODULE_INFO_SYM = {
	common: {
		tag: HARDWARE_MODULE_TAG,
		version_major: 1,
		version_minor: 0,
		id: SMARTCARD_HARDWARE_MODULE_ID,
		name: "SMARTCARD Module",
		author: "iPanel 2011, Inc.",
		methods: &smartcard_module_methods,
		dso: NULL,
		reserved: {0}
	},
	get_smartcard_list:smartcard_get_list,
};

/******************************************************************************/
/*
 * smc_working_mode_set() 设置智能卡读卡器的工作模式
 * 主要工作模式有：
 * protocol type：     	HI_UNF_SCI_PROTOCOL_E proto
 * clk mode:		HI_UNF_SCI_CLK_MODE_E clkMode
 * config VccEn:	HI_UNF_SCI_LEVEL_E vccen
 * config Detect:	HI_UNF_SCI_LEVEL_E detect
 */
static HI_S32 smc_working_mode_set(smc_working_mode* sm) {
	HI_S32 ret = 0;
	if (HI_UNF_SCI_PROTOCOL_T0 == sm->proto) {
		LOGI("%s: %d PORT_MODULE_SMC protocal:T0\n ", __FILE__, __LINE__);
		ret = HI_UNF_SCI_Open(g_u8SCIPort, sm->proto, 3570); //old is 4500
	} else if (HI_UNF_SCI_PROTOCOL_T1 == sm->proto) {
		LOGI("%s: %d PORT_MODULE_SMC protocal:T1\n ", __FILE__, __LINE__);
		ret = HI_UNF_SCI_Open(g_u8SCIPort, sm->proto, 3570); //old is 4500
	} else if (HI_UNF_SCI_PROTOCOL_T14 == sm->proto) {
		LOGI("%s: %d PORT_MODULE_SMC protocal:T14\n ", __FILE__, __LINE__);
		ret = HI_UNF_SCI_Open(g_u8SCIPort, sm->proto, 6000);
	}
	if (ret != HI_SUCCESS) {
		LOGE("[%s] smartcard open fail. \n", __FUNCTION__);
		goto LAB_ERR;
	}

	// set SCI clk mode:
	ret = HI_UNF_SCI_ConfigClkMode(g_u8SCIPort, sm->clkMode);
	if (HI_SUCCESS != ret) {
		LOGE("%s: %d ErrorCode=0x%x \n", __FILE__, __LINE__, ret);
		goto LAB_ERR;
	}

	//set SCI VCCEN
	ret = HI_UNF_SCI_ConfigVccEn(g_u8SCIPort, sm->vccen);
	if (HI_SUCCESS != ret) {
		LOGE("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, ret);
		goto LAB_ERR;
	}

	//set SCI DETECT
	ret = HI_UNF_SCI_ConfigDetect(g_u8SCIPort, sm->detect);
	if (HI_SUCCESS != ret) {
		LOGE("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, ret);
		goto LAB_ERR;
	}

#ifdef PROJ_DALIAN_JIULIAN
	//´óÁ¬¾ÅÁªºÐ×ÓÊ¹ÓÃÅäÖÃ
  	HI_UNF_SCI_ConfigVccEnMode(g_u8SCIPort, HI_UNF_SCI_MODE_CMOS);
	HI_UNF_SCI_ConfigResetMode(g_u8SCIPort, HI_UNF_SCI_MODE_CMOS);
#endif
#ifdef PROJ_HISI_BOARD
  	//HISI¿ª·¢°åÊ¹ÓÃÅäÖÃ
  	HI_UNF_SCI_ConfigVccEnMode(g_u8SCIPort, HI_UNF_SCI_MODE_OD);
	HI_UNF_SCI_ConfigResetMode(g_u8SCIPort, HI_UNF_SCI_MODE_CMOS);
#endif

	LOGI("smc_working_mode_set smartcard setting success!");
	return 0;
LAB_ERR:
	LOGE("smc_working_mode_set smartcard setting failed!");
	return -1;
}

static int smartcard_handleToDriver(int handle) {
	int index = 0;
	int u32sId = -1;

	index = handle - SMARTCARD_VIRTUAL_BASE_HANDLE;
	u32sId = m_smartcard_ctx->smartcardHandle[index];

	return u32sId;
}

static inline long long current_time_millis() {
	struct timeval tv[1];
	gettimeofday(tv, NULL);
	return ((long long) tv->tv_sec) * 1000 + tv->tv_usec / 1000;
}

int smartcard_get_atr(struct smartcard_device_t *dev, int handle, char*b,
		int len) {
	int i = 0, count = 100;
	HI_S32 u32smartcardId = 0, ret = 0;
	HI_U8 u8ATRCount;
	HI_U8 ATRBuf[255];

	struct smartcard_context_t* ctx = (struct smartcard_context_t*) dev;
	LOGI("[%s] start...\n", __FUNCTION__);
	SMARTCARD_CHECK_VALID_POINTER(ctx);
	u32smartcardId = smartcard_handleToDriver(handle);
	SMARTCARD_CHECK_VALID_TUNERID(u32smartcardId);

	ret = HI_UNF_SCI_GetATR(g_u8SCIPort, ATRBuf, 255, &u8ATRCount);
	if (HI_SUCCESS != ret) {
		LOGE("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, ret);
		return -1;
	}

	if (len < u8ATRCount) {
		memcpy((HI_U8 *) b, ATRBuf, len);
		u8ATRCount = len;
	} else {
		memcpy((HI_U8 *) b, ATRBuf, u8ATRCount);
	}

	for (int i = 0; i < u8ATRCount; i++) {
		LOGI("ATRBuf[%d]:%#x\n", i, ATRBuf[i]);
	}

#if 0  //wuxy add for jiulian
	LOGI("set HI_UNF_SCI_SetEtuFactor 372 1");
	ret = HI_UNF_SCI_SetEtuFactor(g_u8SCIPort, 372, 1);
	if (HI_SUCCESS != ret) {
	LOGE("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, ret);
	return -1;
    }
#endif
	if ((0x3b == ATRBuf[0]) && (0x7f == ATRBuf[1]))
	{
		LOGI("[halsmc]tianbai CA\n");
		ca_vender = CA_TYPE_TIANBAI;
		ret = HI_UNF_SCI_SetEtuFactor(g_u8SCIPort, 372, 2);
	    if (HI_SUCCESS != ret)
	    {
	        LOGE("[halsmc]%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, ret);
	        return ret;
	    }
	
	    ret = HI_UNF_SCI_SetGuardTime(g_u8SCIPort, 2);
	    if (HI_SUCCESS != ret)
	    {
	        LOGE("[halsmc]%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, ret);
	        return ret;
	    }
	}
	else{
		ca_vender = CA_TYPE_OTHER;
	}
	return u8ATRCount;
}

static int smartcard_ca_t0communication(unsigned char* uSendData,
		int uSendLength, unsigned char* uRelpyData,
		unsigned int* uRelpyLength) {
	int ret = HI_FAILURE, i;
	unsigned int u32Len = 0;
	unsigned int u32ReceiveLen = 0;
	unsigned char tmpByte = 0;
	unsigned char tmpCnt = 0;
	unsigned char tmpINS = 0;
	unsigned char tmpBuf[258] = { 0 }; /* max len ::  256bytes + SW1 SW2  */
	unsigned int u32ActualLength = 0;
	HI_UNF_SCI_PORT_E uSmcHandle = g_u8SCIPort;

	if (uSendLength < 5) /*  fix me!! sometimes length < 5*/
	{
		LOGE("smartcard_ca_t0communication length is less than 5");
		goto LAB_ERR;
	}
	if (*uSendData == 0xFF) /* CLA check */
	{
		LOGE("smartcard_ca_t0communication CLA check error");
		goto LAB_ERR;;
	}

	tmpINS = *(uSendData + 1);
	if (((tmpINS & 0x1) == 1) || ((tmpINS & 0xF0) == 0x60)
			|| ((tmpINS & 0xF0) == 0x90)) {
		LOGE("smartcard_ca_t0communication INS check error");
		goto LAB_ERR;
	}
	ret = HI_UNF_SCI_Send(uSmcHandle, uSendData, 5, &u32ActualLength, 4000);
	if (HI_SUCCESS != ret) {
		LOGE("smartcard_ca_t0communication HI_UNF_SCI_Send failed,ret=0x%x",
				ret);
		goto LAB_ERR;
	}
	if (5 == uSendLength) /* no procedure byte */
	{
		ret = HI_UNF_SCI_Receive(uSmcHandle, tmpBuf, 1, &u32ReceiveLen, 4000);
		if ((HI_SUCCESS != ret) || (1 != u32ReceiveLen)) {
			LOGE(
					"smartcard_ca_t0communication HI_UNF_SCI_Receive 111 failed,ret=0x%x",
					ret);
			goto LAB_ERR;
		}
		tmpCnt = 0;
		while (tmpBuf[0] == 0x60) {
			usleep(10 * 1000);
			ret = HI_UNF_SCI_Receive(uSmcHandle, tmpBuf, 1, &u32ReceiveLen,
					4000);
			tmpCnt++;
			if (HI_SUCCESS != ret) {
				LOGE(
						"smartcard_ca_t0communication HI_UNF_SCI_Receive 222 failed,ret=0x%x",
						ret);
				goto LAB_ERR;
			}
			if (1 != u32ReceiveLen) {
				goto LAB_ERR;
			}
			if (tmpByte != 0x60) {
				break;
			}
		}
		if (((tmpBuf[0] & 0xF0) == 0x60) || ((tmpBuf[0] & 0xF0) == 0x90)) {
			*uRelpyData = tmpBuf[0];
			ret = HI_UNF_SCI_Receive(uSmcHandle, uRelpyData + 1, 1,
					&u32ReceiveLen, 4000);
			if (HI_SUCCESS != ret) {
				LOGE(
						"smartcard_ca_t0communication HI_UNF_SCI_Receive 333 failed,ret=0x%x",
						ret);
				goto LAB_ERR;
			}
			if (1 != u32ReceiveLen) {
				goto LAB_ERR;
			}

			*uRelpyLength = 2;
			return 0;
		}
		if (tmpINS != (tmpBuf[0] & 0xFE)) {
		}
		if (0 == *(uSendData + 4)) {
			u32Len = 256;
		} else {
			u32Len = *(uSendData + 4);
		}
		ret = HI_UNF_SCI_Receive(uSmcHandle, tmpBuf, u32Len, &u32ReceiveLen,
				4000);
		if ((HI_SUCCESS != ret) || (u32Len != u32ReceiveLen)) {
			LOGE(
					"smartcard_ca_t0communication HI_UNF_SCI_Receive 444 failed,ret=0x%x",
					ret);
			goto LAB_ERR;
		}
		ret = HI_UNF_SCI_Receive(uSmcHandle, tmpBuf + u32Len, 2, &u32ReceiveLen,
				4000);
		if ((HI_SUCCESS != ret) || (2 != u32ReceiveLen)) {
			LOGE(
					"smartcard_ca_t0communication HI_UNF_SCI_Receive 555 failed,ret=0x%x",
					ret);
			goto LAB_ERR;
		}
		*uRelpyLength = u32Len + 2;
		memcpy((void *) uRelpyData, tmpBuf, *uRelpyLength);

		return 0;
	}
	ret = HI_UNF_SCI_Receive(uSmcHandle, &tmpByte, 1, &u32ReceiveLen, 4000);
	if (HI_SUCCESS != ret) {
		LOGE(
				"smartcard_ca_t0communication HI_UNF_SCI_Receive 666 failed,ret=0x%x",
				ret);
		goto LAB_ERR;
	}
	if (1 != u32ReceiveLen) {
		goto LAB_ERR;
	}

	tmpCnt = 0;
	while (tmpByte == 0x60) {
		usleep(10 * 1000);
		ret = HI_UNF_SCI_Receive(uSmcHandle, &tmpByte, 1, &u32ReceiveLen, 4000);
		tmpCnt++;
		if (HI_SUCCESS != ret) {
			LOGE(
					"smartcard_ca_t0communication HI_UNF_SCI_Receive 777 failed,ret=0x%x",
					ret);
			goto LAB_ERR;
		}
		if (1 != u32ReceiveLen) {
			goto LAB_ERR;
		}
		if (tmpByte != 0x60) {
			break;
		}
	}
	if (((tmpByte & 0xF0) == 0x60) || ((tmpByte & 0xF0) == 0x90)) {
		*uRelpyData = tmpByte;
		ret = HI_UNF_SCI_Receive(uSmcHandle, uRelpyData + 1, 1, &u32ReceiveLen,
				4000);
		if (HI_SUCCESS != ret) {
			LOGE(
					"smartcard_ca_t0communication HI_UNF_SCI_Receive 888 failed,ret=0x%x",
					ret);
			goto LAB_ERR;
		}
		if (1 != u32ReceiveLen) {
			goto LAB_ERR;
		}

		*uRelpyLength = 2;
		return 0;
	}
	if ((tmpByte & 0xFE) == tmpINS) {
		if (*(uSendData + 4) == 0) {
			ret = HI_UNF_SCI_Send(uSmcHandle, uSendData + 5, 256,
					&u32ActualLength, 4000);
		} else {
			ret = HI_UNF_SCI_Send(uSmcHandle, uSendData + 5, *(uSendData + 4),
					&u32ActualLength, 4000);
		}
		if (HI_SUCCESS != ret) {
			LOGE(
					"smartcard_ca_t0communication HI_UNF_SCI_Send 222 failed,ret=0x%x",
					ret);
			goto LAB_ERR;
		}
	} else if ((tmpByte & 0xFE) == (tmpINS ^ 0xFE)) {
		ret = HI_UNF_SCI_Send(uSmcHandle, uSendData + 5, 1, &u32ActualLength,
				4000);
		if (HI_SUCCESS != ret) {
			LOGE(
					"smartcard_ca_t0communication HI_UNF_SCI_Send 333 failed,ret=0x%x",
					ret);
			goto LAB_ERR;
		}
	} else {
		goto LAB_ERR;
	}
	ret = HI_UNF_SCI_Receive(uSmcHandle, &tmpByte, 1, &u32ReceiveLen, 4000);
	if (HI_SUCCESS != ret) {
		LOGE(
				"smartcard_ca_t0communication HI_UNF_SCI_Receive 999 failed,ret=0x%x",
				ret);
		goto LAB_ERR;
	}
	if (1 != u32ReceiveLen) {
		goto LAB_ERR;
	}
	tmpCnt = 0;
	while (tmpByte == 0x60) {
		usleep(10 * 1000);
		ret = HI_UNF_SCI_Receive(uSmcHandle, &tmpByte, 1, &u32ReceiveLen, 4000);
		tmpCnt++;
		if (HI_SUCCESS != ret) {
			LOGE(
					"smartcard_ca_t0communication HI_UNF_SCI_Receive AAA failed,ret=0x%x",
					ret);
			goto LAB_ERR;
		}
		if (1 != u32ReceiveLen) {
			goto LAB_ERR;
		}
		if (tmpByte != 0x60) {
			break;
		}
	}
	if (((tmpByte & 0xF0) == 0x60) || ((tmpByte & 0xF0) == 0x90)) {
		*uRelpyData = tmpByte;
		ret = HI_UNF_SCI_Receive(uSmcHandle, uRelpyData + 1, 1, &u32ReceiveLen,
				4000);
		if (HI_SUCCESS != ret) {
			LOGE(
					"smartcard_ca_t0communication HI_UNF_SCI_Receive BBB failed,ret=0x%x",
					ret);
			goto LAB_ERR;
		}
		if (1 != u32ReceiveLen) {
			goto LAB_ERR;
		}
		*uRelpyLength = 2;
		return 0;
	}

	LAB_ERR: LOGE("smartcard_ca_t0communication communication error");
	return -1;
}

static int smartcard_transfer(struct smartcard_device_t *dev, int handle,
		struct io_block_t*iob, unsigned short *StatusWord) {
	HI_S32 u32smartcardId = 0;
	HI_S32 ret = -1;
	HI_UNF_SCI_STATUS_E smc_status;
	unsigned char* reqdata = (unsigned char*) iob->in;
	unsigned char* repdata = (unsigned char*) iob->out;
	unsigned int req_len = (unsigned int) iob->inlen;
	unsigned int* rep_len = (unsigned int*) &(iob->outlen);
	unsigned int replen = 0;
	unsigned short SW1, SW2;
	struct smartcard_context_t* ctx = (struct smartcard_context_t*) dev;

	SMARTCARD_CHECK_VALID_POINTER(ctx);
	u32smartcardId = smartcard_handleToDriver(handle);
	SMARTCARD_CHECK_VALID_TUNERID(u32smartcardId);

	LOGI("[%s] start...\n", __FUNCTION__);

	ret = HI_UNF_SCI_GetCardStatus(g_u8SCIPort, &smc_status);
	if (ret != HI_SUCCESS || smc_status < HI_UNF_SCI_STATUS_INACTIVECARD) {
		LOGE("smartcard_transfer smartcard curr state = %d error.", smc_status);
		return -1;
	}

	ret = smartcard_ca_t0communication(reqdata, req_len, repdata, &replen);
	if (ret != 0) {
		LOGE("smartcard_ca_t0communication failed");
		return -2;
	} else {
		SW1 = (unsigned short) repdata[replen - 2];
		SW2 = (unsigned short) repdata[replen - 1];
		*StatusWord = (SW1 << 8) | SW2;
		//*rep_len = replen - 2;
		*rep_len = replen;
		g_recv_len = *rep_len;
	}

	LOGI("[%s] end...\n", __FUNCTION__);
	return 0;
}

static int smartcard_get_status(struct smartcard_device_t *dev, int handle,
		int *status, int* fd) {
	//LOGI("gongcm smartcard status 2017 3 13 \n");
	HI_S32 u32smartcardId = 0, ret = 0;
	HI_UNF_SCI_STATUS_E getStatus = HI_UNF_SCI_STATUS_UNINIT;
	static HI_UNF_SCI_STATUS_E lastStatus = HI_UNF_SCI_STATUS_UNINIT;
	static HI_U32 lastStatusRet = 0;
	struct smartcard_context_t* ctx = (struct smartcard_context_t*) dev;
	HI_U32 usd_us = 0, timeStart = 0;
	static int set_etu_flag = 0;
	LOGV("[%s] start...\n", __FUNCTION__);
	SMARTCARD_CHECK_VALID_POINTER(ctx);
	u32smartcardId = smartcard_handleToDriver(handle);
	SMARTCARD_CHECK_VALID_TUNERID(u32smartcardId);

	ret = HI_UNF_SCI_GetCardStatus((HI_UNF_SCI_PORT_E) u32smartcardId,
			&getStatus);
	if (ret != HI_SUCCESS) {
		LOGE("[%s] HI_UNF_SCI_GetCardStatus error ret = %d\n", __FUNCTION__,
				ret);
		goto SC_GET_STATUS_ERROR;
	}
	LOGI("[%s] HI_UNF_SCI_GetCardStatus getStatus  = %d\n", __FUNCTION__,
				getStatus);
	switch (getStatus) {
	case HI_UNF_SCI_STATUS_UNINIT:			//< SCI未初始化
	case HI_UNF_SCI_STATUS_NOCARD:          //
			LOGI("[%s] HI_UNF_SCI_STATUS_NOCARD\n",__FUNCTION__);
		memset(g_cardstatus, 0, sizeof(g_cardstatus));
		*status = SMARTCARD_CARD_ABSENT;  //SMARTCARD_CARD_ABSENT    = 0,
		break;

	case HI_UNF_SCI_STATUS_FIRSTINIT:
	case HI_UNF_SCI_STATUS_INACTIVECARD:
			LOGI("HI_UNF_SCI_STATUS_INACTIVECARD");
		if (g_cardstatus[u32smartcardId].reset_once == 0) {
			g_cardstatus[u32smartcardId].reset_once = 1;
			g_cardstatus[u32smartcardId].muted =
					HI_UNF_SCI_ResetCard(g_u8SCIPort, HI_FALSE) ? 1 : 0;
			g_cardstatus[u32smartcardId].present_time_millis =
					current_time_millis();
		} else {
			if (g_cardstatus[u32smartcardId].muted == 0) {
				long long t = current_time_millis();
				if (t - g_cardstatus[u32smartcardId].present_time_millis
						> MUTED_TIMEOUT) {
					g_cardstatus[u32smartcardId].muted = 1;
				}
			}
		}

		if (g_cardstatus[u32smartcardId].muted) {
			LOGI("SMARTCARD_CARD_MUTED");
			*status = SMARTCARD_CARD_MUTED;
			break;
		}
	case HI_UNF_SCI_STATUS_WAITATR:
	case HI_UNF_SCI_STATUS_READATR:
		LOGI("HI_UNF_SCI_STATUS_READATR");
		*status = SMARTCARD_CARD_PRESENT;  //SMARTCARD_CARD_PRESENT   = 1,
		break;

	case HI_UNF_SCI_STATUS_READY: {
		LOGI("HI_UNF_SCI_STATUS_READY");
		//*status = SMARTCARD_CARD_READY; // SMARTCARD_CARD_READY  = 3,

		if (set_etu_flag == 0) {
			//ret = HI_UNF_SCI_SetEtuFactor(g_u8SCIPort, 372, 2);
			if (HI_SUCCESS != ret) {
				LOGE("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, ret);
			} else {
				set_etu_flag = 1;
				//	LOGI("HI_UNF_SCI_SetEtuFactor sucess!");
			}
		}
		break;
	}
	case HI_UNF_SCI_STATUS_RX:
	case HI_UNF_SCI_STATUS_TX:
		break;
	}

	lastStatus = getStatus;
	return 0;  //TODO
	SC_GET_STATUS_ERROR: return -1;
}

static HI_UNF_SCI_PROTOCOL_E smartcard_atr_parse(unsigned char *atr) { //挺好的代码保留下来 给后来的伙伴用
	int atr_len = 0, index = 0;
	HI_UNF_SCI_PROTOCOL_E protocol = HI_UNF_SCI_PROTOCOL_T0;

	if (!atr || strlen((char*) atr) == 0) {
		LOGE("[smartcard_atr_parse]  invalid atr");
		return HI_UNF_SCI_PROTOCOL_BUTT;
	}

	/**************************************
	 * 通过atr解析atr长度,智能卡协议等信息 *
	 **************************************/
	atr_len = 0;
	atr_len += 2;

	atr_len += (atr[1] & 0x80) ? 1 : 0;
	atr_len += (atr[1] & 0x40) ? 1 : 0;
	atr_len += (atr[1] & 0x20) ? 1 : 0;
	atr_len += (atr[1] & 0x10) ? 1 : 0;

	if (atr[1] & 0x80) {
		if ((atr[atr_len - 1] & 0x0f) == 0)
			protocol = HI_UNF_SCI_PROTOCOL_T0;
		else if ((atr[atr_len - 1] & 0x0f) == 1)
			protocol = HI_UNF_SCI_PROTOCOL_T1;
		else if ((atr[atr_len - 1] & 0x0f) == 14)
			protocol = HI_UNF_SCI_PROTOCOL_T14;
		atr_len++;
	}

	atr_len += (atr[1] & 0x0f);

	LOGD("[smartcard_atr_parse] protocol = %d", protocol);
	return protocol;
}

static int smartcard_reset(struct smartcard_device_t *dev, int handle,
		int flags) {
HI_S32 u32smartcardId = 0, ret = 0;
	int count = 20;
	HI_UNF_SCI_STATUS_E getStatus = HI_UNF_SCI_STATUS_UNINIT;
	struct smartcard_context_t* ctx = (struct smartcard_context_t*)dev;

	LOGI("[%s] start...\n",__FUNCTION__);

	SMARTCARD_CHECK_VALID_POINTER(ctx);
	u32smartcardId = smartcard_handleToDriver(handle);
	SMARTCARD_CHECK_VALID_TUNERID(u32smartcardId);

	ret = HI_UNF_SCI_ResetCard(g_u8SCIPort, HI_FALSE);
	if (HI_SUCCESS != ret) {
		LOGE("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, ret);
		return -1;
	}
	
	while (--count > 0) {
		ret = HI_UNF_SCI_GetCardStatus((HI_UNF_SCI_PORT_E) u32smartcardId,
				&getStatus);
		if (ret == HI_SUCCESS && getStatus == HI_UNF_SCI_STATUS_READY) {
			LOGI("HI_UNF_SCI_GetCardStatus success and status is READY");
			break;
		}
		usleep(50*1000);
	}
	if (0 == count){
		LOGE("[%s] get card status timeout,getStatus:%d\n",__FUNCTION__,getStatus);
		return -1;
	}
	LOGI("[%s] end...\n",__FUNCTION__);
	return 0;
}

static int hardware_match() {
	char prop[96];
	memset(prop, 0, sizeof(prop));
	property_get("android.hardware.type", prop, NULL);
	return strcmp(prop, "hisi.hardware.tgd") == 0 ? 0 : -1;
}

static int smartcard_get_list(struct smartcard_module_t *module,
		struct smartcard_t const **list) {
	int num = 0, i = 0;
	/* already open */
	if (m_psmartcardList && m_max_smartcard) {
		(*list) = m_psmartcardList;
		return m_max_smartcard;
	}

	num = hardware_match() == 0 ? 0:SMARTCARD_MAX_NUM;
	LOGD("smartcard_get_list > nomal num = %d", num);

	if (num == 0) {
		LOGE("[smartcard_get_list] device number is ZERO!\n");
		return 0;
	}

	m_psmartcardList = (struct smartcard_t*) calloc(1,
			sizeof(struct smartcard_t) * num);
	if (NULL == m_psmartcardList) {
		LOGE("[smartcard_get_list] malloc smartcard list fail.\n");
		return -1;
	}

	memset(g_cardstatus, 0, sizeof(g_cardstatus));

	for (i = 0; i < num; i++) {
		/* 先使用虚拟句柄,真正句柄在open时打开 */
		
		m_psmartcardList[i].handle = SMARTCARD_VIRTUAL_BASE_HANDLE + i;
		//TODO
	}

	(*list) = m_psmartcardList;
	m_max_smartcard = num;

	LOGI("[smartcard_get_list] smartcard num=%d\n", num);
	return num;
}

int smartcard_send_data(struct smartcard_device_t *dev, int handle,
		const char*Buffer, int NumberToWrite, unsigned int* NumberWritten_p,
		int timeout) {
	int i = 0;
	HI_S32 u32smartcardId = 0;
	HI_S32 ret = HI_FAILURE;
	struct smartcard_context_t* ctx = (struct smartcard_context_t*) dev;
	
	SMARTCARD_CHECK_VALID_POINTER(ctx);
	u32smartcardId = smartcard_handleToDriver(handle);
	SMARTCARD_CHECK_VALID_TUNERID(u32smartcardId);

	LOGI("smartcard_send_data NumberToWrite = %d \n", NumberToWrite);

	/* 输出接收数据，调试用 */
	#if 0
	for (i = 0; i < NumberToWrite; ++i) {
		LOGI("0x%02x ", (unsigned char) Buffer[i]);
	}
	#endif
	if (CA_TYPE_TIANBAI == ca_vender){
		ret = HI_UNF_SCI_Send(g_u8SCIPort, (HI_U8 *) Buffer, NumberToWrite,
				NumberWritten_p, timeout);
		if (HI_SUCCESS != ret) {
			LOGE(" HI_UNF_SCI_Send failed ErrorCode:0x%x\n", ret);
			return ret;
			}
	}
	else{
		unsigned short StatusWord;
		struct io_block_t iob;
		iob.in=(void*)Buffer;
		iob.out=g_recv_Buffer;
		iob.inlen=NumberToWrite;
		iob.outlen=256;
		
		smartcard_transfer(dev, handle, &iob,&StatusWord);
	}	

	*NumberWritten_p = NumberToWrite;
	return HI_SUCCESS;
}

int smartcard_recv_data(struct smartcard_device_t *dev, int handle,
		char *Buffer, int NumberToRead, unsigned int *NumberRead_p,
		int timeout) {
	int i = 0, trytimes = 5;
	HI_S32 ret = HI_FAILURE;
	HI_S32 u32smartcardId = 0;
	struct smartcard_context_t* ctx = (struct smartcard_context_t*) dev;

	LOGI("smartcard_recv_data start NumberToRead = %d, *NumberRead_p=%d\n", NumberToRead, *NumberRead_p);

	SMARTCARD_CHECK_VALID_POINTER(ctx);
	u32smartcardId = smartcard_handleToDriver(handle);
	SMARTCARD_CHECK_VALID_TUNERID(u32smartcardId);
	
	if (CA_TYPE_TIANBAI == ca_vender){
		while (trytimes > 0) {
			if(NULL != NumberRead_p){
				*NumberRead_p = 0;
			}
			ret = HI_UNF_SCI_Receive(g_u8SCIPort, (HI_U8*) Buffer, NumberToRead,
					NumberRead_p, timeout);
			if (HI_SUCCESS != ret) {
				LOGE(" HI_UNF_SCI_Receive receive failed,ErrorCode:0x%x\n", ret);
			} else {
				/* 输出接收数据，调试用 */
				#if 0
				for (i = 0; i < *NumberRead_p; i++) {
					LOGI("0x%02x ", (unsigned char) Buffer[i]);
				}
				
				#endif
			}

			if (NumberToRead == *NumberRead_p) {
				LOGI("HI_UNF_SCI_Receive receive data length is OK,length=%d",
						NumberToRead);
				break;
			} else {
				LOGE("HI_UNF_SCI_Receive real length(%d)--%d", NumberToRead,*NumberRead_p);
			}

			--trytimes;
			usleep(50);
		}
	}
	else {
		memset(Buffer, 0x00, NumberToRead);
		//Buffer = &g_recv_Buffer[0];
		*NumberRead_p = g_recv_len;
		memcpy(Buffer, g_recv_Buffer, *NumberRead_p);
	}

	return HI_SUCCESS;
}
/*****************************************************************************/

int smartcard_ioctl(struct smartcard_device_t *dev, int handle, int op,
		struct io_block_t*iob) {
	LOGI("[%s] start...\n", __FUNCTION__);
	HI_S32 u32smartcardId = 0, ret = 0;
	HI_U8 u8ATRCount;
	HI_UNF_SCI_PARAMS_S SciParam;
	HI_U8 ATRBuf[255];
	struct smartcard_context_t* ctx = (struct smartcard_context_t*) dev;

	SMARTCARD_CHECK_VALID_POINTER(ctx);
	u32smartcardId = smartcard_handleToDriver(handle);
	SMARTCARD_CHECK_VALID_TUNERID(u32smartcardId);
	if ((op < SMARTCARD_GET_PROTOCOL) || (op > SMARTCARD_GET_MODE))
		return -1;

	switch (op) {
	case SMARTCARD_GET_PROTOCOL:
		memset(&SciParam, 0, sizeof(HI_UNF_SCI_PARAMS_S));
		ret = HI_UNF_SCI_GetParams(g_u8SCIPort, &SciParam);
		if (HI_SUCCESS != ret) {
			LOGI("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, ret);
			goto SMARTCARD_IOCTL_ERROR;
		}
		memcpy((HI_U8 *) iob->out, &SciParam.enProtocolType,
				sizeof(SciParam.enProtocolType));
		iob->outlen = sizeof(SciParam.enProtocolType);
		//LOGI("%s: %d SciParam.enProtocolType = %d\n", __FILE__, __LINE__, SciParam.enProtocolType);
		break;
	case SMARTCARD_GET_MODE:
		break;

	default:
		return -1;
	}

	return 0;
	SMARTCARD_IOCTL_ERROR: return -1;  //TODO
}

static int close_smartcard(struct hw_device_t *dev) {
//TODO
	HI_S32 ret = 0;
	LOGI("[%s] start...\n", __FUNCTION__);
	if (ret != HI_UNF_SCI_DeactiveCard(g_u8SCIPort))
		LOGE("HI_UNF_SCI_DeactiveCard failed\n");
	if (ret != HI_UNF_SCI_Close(g_u8SCIPort))
		LOGE("HI_UNF_SCI_Close failed\n");
	return 0;
}

static int open_smartcard(const struct hw_module_t* module, const char* name,
		struct hw_device_t** device) {
	HI_S32 ret = 0, i;
	smc_working_mode *sm = NULL;
	smartcard_context_t *ctx = NULL;

	LOGI("[%s] start...\n", __FUNCTION__);
	if (!strcmp(name, SMARTCARD_HARDWARE_CARD0)) {
		ctx = (smartcard_context_t *) malloc(sizeof(smartcard_context_t));
		memset(ctx, 0, sizeof(*ctx));

		ctx->device.common.tag = HARDWARE_DEVICE_TAG;
		ctx->device.common.version = 1;
		ctx->device.common.module = const_cast<hw_module_t*>(module);
		ctx->device.common.close = close_smartcard;

		ctx->device.reset = smartcard_reset;
		ctx->device.get_atr = smartcard_get_atr;
		ctx->device.transfer = smartcard_transfer;
		ctx->device.get_status = smartcard_get_status;
		ctx->device.ioctl = smartcard_ioctl;
		ctx->device.send_data = smartcard_send_data;
		ctx->device.recv_data = smartcard_recv_data;
		ret = HI_UNF_SCI_Init();
		if (ret != HI_SUCCESS) {
			LOGE("[%s] smartcard open fail.\n", __FUNCTION__);
			goto BAIL;
		}
#ifdef PROJ_DALIAN_JIULIAN
		LOGD("hetao use SCM_DALIAN_JIULIAN m_max_smartcard = %d",m_max_smartcard);
		for (i = 0; i < m_max_smartcard; i++) {
			sm = &g_smc_working_mode[i];
				LOGD("zhangzhy 4444444444444444444444444444444444444444444444444");
			sm->proto = g_proto[0];
			sm->clkMode = HI_UNF_SCI_MODE_CMOS;//HI_UNF_SCI_MODE_OD;
			sm->vccen = HI_UNF_SCI_LEVEL_HIGH; //HI_UNF_SCI_LEVEL_HIGH;
			sm->detect = HI_UNF_SCI_LEVEL_HIGH;//HI_UNF_SCI_LEVEL_HIGH;
			if (smc_working_mode_set(sm) != 0) {
				LOGE("open_smartcard smc_working_mode_set fail");
				goto BAIL;
			}
		}
#endif
#ifdef PROJ_HISI_BOARD
		for (i = 0; i < m_max_smartcard; i++) {
			sm = &g_smc_working_mode[i];
			sm->proto = g_proto[0];
			sm->clkMode = HI_UNF_SCI_MODE_OD;
			sm->vccen = HI_UNF_SCI_LEVEL_LOW;
			sm->detect = HI_UNF_SCI_LEVEL_HIGH;
			if (smc_working_mode_set(sm) != 0) {
				LOGE("open_smartcard smc_working_mode_set fail");
				goto BAIL;
			}
		}
#endif
		if (-1 == ret) {
			LOGI("%s: %d smc_working_mode_set error\n ", __FILE__, __LINE__);
			goto BAIL;
		}

		m_smartcard_ctx = ctx;
		for (i = 0; i < m_max_smartcard; i++) {
			/* 使用u32sId作为内部句柄 */
			ctx->smartcardHandle[i] = i;
			ret = 0;
		}

		if (ret == 0) {
			*device = &ctx->device.common;
		} else {
			close_smartcard(&ctx->device.common);
		}

		LOGI("[open_smartcard] open succ!\n");
		return 0;
	} else {
		LOGE("[open_smartcard] error name.\n");
		ret = -1;
	}

	LOGI("smc open success");
	BAIL: LOGE("[open_smartcard] open fail!\n");
	return ret;
}

