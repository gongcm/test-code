/* ----------------------------------------------------------------------------
 * @module
 *			HAL module
 * @routines
 *			demux device
 * @date
 *			2011.3
 */
#define LOG_NDEBUG  0
#define LOG_TAG "avdecTest"

#include <cutils/log.h>
#include <cutils/properties.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "hardware/hardware.h"
#include "hardware/avout.h"
#include "hardware/demux.h"
#include "hardware/tuner.h"
#include "hardware/avdec.h"
#if 0
#define VPID 0x42 //0x206
#define APID 0x41 //0x2c6
#define PPID VPID //0x35
#define VFORMAT VIDEO_CODEC_MAX_NUMS
#define AFORMAT ACODEC_TYPE_AAC
#define FILE_PATH "/system/mount/iPanel_4430000_4K.ts"//"/system/mount/program2_split.ts"
#else
#define VPID 0x206
#define APID 0x2c6
#define PPID 0x35
#define VFORMAT VCODEC_TYPE_MPEG2
#define AFORMAT ACODEC_TYPE_MPEG2
#define FILE_PATH "/storage/sdcard1/program2_split.ts"
#endif
/********************Descriptor flag definition******************/
/********************CNcomment:描述符标识符定义******************/

#define STREAM_TYPE_11172_VIDEO 0x01
#define STREAM_TYPE_13818_VIDEO 0x02
#define STREAM_TYPE_11172_AUDIO 0x03
#define STREAM_TYPE_13818_AUDIO 0x04
#define STREAM_TYPE_14496_2_VIDEO 0x10  //MPEG4
#define STREAM_TYPE_14496_10_VIDEO 0x1B  //H264
#define STREAM_TYPE_AVS_VIDEO   0x42    // AVS
#define STREAM_TYPE_13818_7_AUDIO 0x0F  //AAC
#define STREAM_TYPE_14496_3_AUDIO 0x11  //AAC
#define STREAM_TYPE_AC3_AUDIO 0x81  //AC3

#define STREAM_TYPE_PRIVATE 0x06 /* PES packets containing private data */


#define VIDEO_STREAM_DESCRIPTOR 0x02
#define AUDIO_STREAM_DESCRIPTOR 0x03
#define HIERACHY_DESCRIPTOR 0x04
#define REGISTRATION_DESCRIPTOR 0x05
#define DATA_STREAM_ALIGNMENT_DESCRIPTOR 0x06
#define TARGET_BACKGROUND_GRID_DESCRIPTOR 0x07
#define VIDEO_WINDOW_DESCRIPTOR 0x08
#define CA_DESCRIPTOR 0x09
#define LANGUAGE_DESCRIPTOR 0x0A
#define SYSTEM_CLOCK_DESCRIPTOR 0x0B
#define MULTIPLEX_BUFFER_USAGE_DESCRIPTOR 0x0C
#define COPYRIGHT_DESCRIPTOR 0x0D
#define MAXIMUM_BITRATE_DESCRIPTOR 0x0E
#define PRIVATE_DATA_INDICATOR_DESCRIPTOR 0x0F
#define SMOOTHING_BUFFER_DESCRIPTOR 0x10
#define STD_DESCRIPTOR 0x11
#define IBP_DESCRIPTOR 0x12

#define NETWORK_NAME_DESCRIPTOR 0x40
#define SERVICE_LIST_DESCRIPTOR 0x41
#define STUFFING_DESCRIPTOR 0x42
#define SATELLITE_DELIVERY_DESCRIPTOR 0x43
#define CABLE_DELIVERY_DESCRIPTOR 0x44
#define BOUQUET_NAME_DESCRIPTOR 0x47
#define SERVICE_DESCRIPTOR 0x48
#define COUNTRY_AVAILABILITY_DESCRIPTOR 0x49
#define LINKAGE_DESCRIPTOR 0x4A
#define NVOD_REFERENCE_DESCRIPTOR 0x4B
#define TIME_SHIFTED_SERVICE_DESCRIPTOR 0x4C
#define SHORT_EVENT_DESCRIPTOR 0x4D
#define EXTENDED_EVENT_DESCRIPTOR 0x4E
#define TIME_SHIFTED_EVENT_DESCRIPTOR 0x4F
#define COMPONENT_DESCRIPTOR 0x50
#define MOSAIC_DESCRIPTOR 0x51
#define STREAM_IDENTIFIER_DESCRIPTOR 0x52
#define CA_IDENTIFIER_DESCRIPTOR 0x53
#define CONTENT_DESCRIPTOR 0x54
#define PARENTAL_RATING_DESCRIPTOR 0x55
#define TELETEXT_DESCRIPTOR 0x56
#define TELEPHONE_DESCRIPTOR 0x57
#define LOCAL_TIME_OFFSET_DESCRIPTOR 0x58
#define SUBTITLING_DESCRIPTOR 0x59
#define TERRESTRIAL_DELIVERY_DESCRIPTOR 0x5A
#define MULTILINGUAL_NETWORK_NAME_DESCRIPTOR 0x5B
#define MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR 0x5C
#define MULTILINGUAL_SERVICE_NAME_DESCRIPTOR 0x5D
#define MULTILINGUAL_COMPONENT_DESCRIPTOR 0x5E
#define PRIVATE_DATA_SPECIFIER_DESCRIPTOR 0x5F
#define SERVICE_MOVE_DESCRIPTOR 0x60
#define SHORT_SMOOTHING_BUFFER_DESCRIPTOR 0x61
#define FREQUENCY_LIST_DESCRIPTOR 0x62
#define PARTIAL_TRANSPORT_STREAM_DESCRIPTOR 0x63
#define DATA_BROADCAST_DESCRIPTOR 0x64
#define CA_SYSTEM_DESCRIPTOR 0x65
#define DATA_BROADCAST_ID_DESCRIPTOR 0x66
#define TRANSPORT_STREAM_DESCRIPTOR 0x67
#define DSNG_DESCRIPTOR 0x68
#define PDC_DESCRIPTOR 0x69
#define AC3_DESCRIPTOR 0x6A
#define AC3_PLUS_DESCRIPTOR 0x7A
#define ANCILLARY_DATA_DESCRIPTOR 0x6B
#define CELL_LIST_DESCRIPTOR 0x6C
#define CELL_FREQUENCY_LINK_DESCRIPTOR 0x6D
#define ANNOUNCEMENT_SUPPORT_DESCRIPTOR 0x6E

#define AC3_EXT_DESCRIPTOR 0x52

/***********TS PID defintion**************/

#define PAT_TSPID (0x0000)
#define CAT_TSPID (0x0001)
#define NIT_TSPID (0x0010)
#define EIT_TSPID (0x0012)
#define TOT_TSPID (0x0014)
#define TDT_TSPID (0x0014)
#define SDT_TSPID (0x0011)
#define BAT_TSPID (0x0011)

#define INVALID_TSPID (0x1fff)

/***********Table ID defintion**************/
#define PAT_TABLE_ID (0x00)
#define CAT_TABLE_ID (0x01)
#define PMT_TABLE_ID (0x02)
#define NIT_TABLE_ID_ACTUAL (0x40)
#define NIT_TABLE_ID_OTHER (0x41)

#define SDT_TABLE_ID_ACTUAL (0x42)
#define SDT_TABLE_ID_OTHER (0x46)

#define BAT_TABLE_ID (0x4A)
#define EIT_TABLE_ID_PF_ACTUAL (0x4E)
#define EIT_TABLE_ID_PF_OTHER (0x4F)
#define EIT_TABLE_ID_SCHEDULE_ACTUAL_LOW (0x50)
#define EIT_TABLE_ID_SCHEDULE_ACTUAL_HIGH (0x5F)

#define EIT_TABLE_ID_SCHEDULE_OTHER_LOW (0x60)
#define EIT_TABLE_ID_SCHEDULE_OTHER_HIGH (0x6F)

#define TDT_TABLE_ID (0x70)
#define TOT_TABLE_ID (0x73)

#define INVALID_TABLE_ID (0xff)
#define CHANNEL_MAX_PROG	96
#define PROG_MAX_VIDEO	 	8
#define PROG_MAX_AUDIO 	8

#define SUBTDES_INFO_MAX 10
#define SUBTITLING_MAX 15

#define MAX_PROGNAME_LENGTH 32
#define MAX_AUDIO_LANGUAGE 5
#define MAX_PROG_COUNT 200
#define MAX_FRONTEND_COUNT 30

#define SEARCHING_FRONTEND_ID	0xffff

//#define SRCH_DEBUG

typedef struct hiPAT_INFO_S
{
    unsigned short u16ServiceID;    /*Progam 's SERVICE ID*/
    unsigned short u16PmtPid;        /*Progam 's PMT ID*/
} PAT_INFO;

typedef struct hiPAT_TB_S
{
    unsigned short u16ProgNum;
    unsigned short u16TsID;
    PAT_INFO PatInfo[CHANNEL_MAX_PROG];
} PAT_TB;

typedef struct hiPMT_VIDEO_S
{
    unsigned int u32VideoEncType;
    unsigned short u16VideoPid;
} PMT_VIDEO;

typedef struct hiPMT_AUDIO_S
{
    unsigned int u32AudioEncType;
    unsigned short u16AudioPid;
} PMT_AUDIO;

typedef struct hiPMP_SUBTITLE_DES_S
{
    unsigned int u32LangCode; /* low 24-bit valid */
    unsigned char  u8SubtitleType;
    unsigned short u16PageID;
    unsigned short u16AncillaryPageID;

}PMP_SUBTITLE_DES;

typedef struct hiPMT_SUBTITLE_S
{
    unsigned short u16SubtitlingPID;

    unsigned char u8DesTag; /*  */
    unsigned char u8DesLength;

    unsigned char u8DesInfoCnt;

    PMP_SUBTITLE_DES DesInfo[SUBTDES_INFO_MAX];

}PMT_SUBTITLE;

typedef struct hiPMT_TB_S
{
    unsigned short u16ServiceID;
    unsigned short u16PcrPid;
    unsigned short u16VideoNum;
    unsigned short u16AudoNum;
    PMT_VIDEO Videoinfo[PROG_MAX_VIDEO];
    PMT_AUDIO Audioinfo[PROG_MAX_AUDIO];

    unsigned short u16SubtitlingNum;
    PMT_SUBTITLE SubtitingInfo[SUBTITLING_MAX];
} PMT_TB;

typedef enum hiRUN_STATE_E
{
    UnDefined = 0,
    NotRun,
    StartInSeconds,
    Pause,
    Running,
    Run_Reserved1,
    Run_Reserved2,
    Run_Reserved3
} RUN_STATE_E;

typedef enum hiCA_MODE_E
{
    CA_NotNeed = 0,
    CA_Need
} CA_MODE_E;

typedef struct hiSDT_INFO_S
{
    unsigned short      	u16ServiceID;
    unsigned char 		u8EitFlag;
    unsigned char 		u8EitFlag_PF;
    RUN_STATE_E RunState;
    CA_MODE_E   CAMode;

    unsigned int      u32ServiceType;
    char  	s8ProgName[32];
} SDT_INFO;

typedef struct hiSDT_TB_S
{
    unsigned int u32ProgNum;
    unsigned short u16TsId;
    unsigned short u16NetID;
    SDT_INFO SdtInfo[CHANNEL_MAX_PROG];
} SDT_TB;

/* program struct */
typedef struct hiDB_VIDEOEX_S
{
    unsigned short u16VideoPid;
    unsigned int u32VideoEncType;
} DB_VIDEOEX_S;

typedef struct hiDB_AUDIOEX_S
{
    unsigned short u16audiopid;
    unsigned short u16audiolan;
    unsigned int u32AudioEncType;
} DB_AUDIOEX_S;

typedef struct  hiDB_PROGRAM_S
{
    unsigned short u16FrontendID;
    unsigned short u16NetworkID;
    unsigned short u16TsID;
    unsigned short u16ServiceID;

    unsigned char enServiceType;

    char ProgramName[MAX_PROGNAME_LENGTH];

    unsigned short u16PmtPid;
    unsigned short u16PcrPid;

    unsigned int u16ProgProperty;

    unsigned short u16AudioVol;
    unsigned char  u8AudioChannel;
    unsigned char  u8VideoChannel;

    DB_VIDEOEX_S VideoEX;
    DB_AUDIOEX_S AudioEX[MAX_AUDIO_LANGUAGE];

    unsigned short Reserved;
} DB_PROGRAM_S;

static void ParseSDTDesc( unsigned char* pu8DesData, int s32DescLen, SDT_INFO *pstruProg)
{
    unsigned int u32NameLength = 0;
    unsigned char u8Tag = 0;
    unsigned char u8Length, linktype = 0;
    unsigned short u16TsId, u16NetworkId, u16serviceId, u16refserviceId;

    const unsigned char* pu8Data = NULL;

    if ((NULL == pu8DesData) || (NULL == pstruProg))
    {
        return;
    }

    while (s32DescLen > 0)
    {
        u8Tag = *pu8DesData++;
        u8Length = *pu8DesData++;

        if (u8Length == 0)
        {
            return;
        }

        pu8Data = pu8DesData;
        pu8DesData += u8Length;

        s32DescLen -= (u8Length + 2);

        switch (u8Tag)
        {
        case SERVICE_DESCRIPTOR:
        {
            pstruProg->u32ServiceType = *pu8Data++;

            u32NameLength = *pu8Data++;
            pu8Data += u32NameLength;

            u32NameLength = *pu8Data++;

            if (0 == u32NameLength)
            {
                break;
            }


            /*Parse service name*/
            if (u32NameLength > MAX_PROGNAME_LENGTH - 1)
            {
                u32NameLength = (unsigned int)(MAX_PROGNAME_LENGTH - 1);
            }

            strncpy((char*)pstruProg->s8ProgName, (char*)pu8Data, u32NameLength);

            break;
        }
        case NVOD_REFERENCE_DESCRIPTOR:
        {
            while (u8Length > 0)
            {
                u16TsId = (unsigned short)((pu8Data[0] << 8) | pu8Data[1]);
                u16NetworkId = (unsigned short)((pu8Data[2] << 8) | pu8Data[3]);
                u16serviceId = (unsigned short)((pu8Data[4] << 8) | pu8Data[5]);

                u16refserviceId = pstruProg->u16ServiceID;

                pstruProg->u32ServiceType = 0x04;

                //printf("\n +++ NVOD_REFERENCE_DESCRIPTOR networkid=%x tsid =%x, refserviceid = %x  service =%x +++\n", u16NetworkId, u16TsId, u16refserviceId, u16serviceId);

                //debug
                //DB_PsiNvodRefServiceAdd(u16NetworkId, u16TsId, u16refserviceId);

                u8Length = u8Length - 6;

                if (u8Length <= 0)
                {
                    break;
                }

                pu8Data += 6;
            }

            break;
        }
        case COUNTRY_AVAILABILITY_DESCRIPTOR:
        case LINKAGE_DESCRIPTOR:

            u16TsId = (unsigned short)((pu8Data[0] << 8) | pu8Data[1]);
            u16NetworkId = (unsigned short)((pu8Data[2] << 8) | pu8Data[3]);
            u16serviceId = (unsigned short)((pu8Data[4] << 8) | pu8Data[5]);

            linktype = pu8Data[6];

            //printf("\n +++ SDT LINKAGE linktype =0x%02x +++\n", linktype);

        case MOSAIC_DESCRIPTOR:

            //printf("\n +++ SDT MOSAIC_DESCRIPTOR +++\n");
            break;
        case CA_IDENTIFIER_DESCRIPTOR:
        case TELEPHONE_DESCRIPTOR:
        case MULTILINGUAL_SERVICE_NAME_DESCRIPTOR:
        case PRIVATE_DATA_SPECIFIER_DESCRIPTOR:
        case DATA_BROADCAST_DESCRIPTOR:
        case STUFFING_DESCRIPTOR:
        case BOUQUET_NAME_DESCRIPTOR:
            break;
        default:
            break;
        }
    }
}

static void ParsePMTDesc( unsigned char* pu8DesData, int s32DescLength, PMT_TB* pstruProg)
{
    unsigned char u8Tag = 0;
    unsigned char u8Length = 0;
    unsigned char u8CntIndex = 0;

    if ((NULL == pu8DesData) || (NULL == pstruProg))
    {
        return;
    }

    while (s32DescLength > 0)
    {
        u8Tag = *pu8DesData++;
        u8Length = *pu8DesData++;
        if (u8Length == 0)
        {
            return;
        }

        switch (u8Tag)
        {
        case LANGUAGE_DESCRIPTOR:
        {
            break;
        }
        case CA_DESCRIPTOR:
        {
            break;
        }
        case TELETEXT_DESCRIPTOR:
            break;
        case SUBTITLING_DESCRIPTOR:
        {
            unsigned short ii = 0;
            unsigned short nCount = u8Length / 8; /*8 =  3 + 1 + 2 + 2*/
            unsigned short u16SubtIndex = pstruProg->u16SubtitlingNum;

            for(ii=0; ii<nCount; ii++)
            {
                u8CntIndex = pstruProg->SubtitingInfo[u16SubtIndex].u8DesInfoCnt;
               // printf("Subtitle tag s32DescLength:%u.............pstruProg->u16SubtitlingNum = %d, u8CntIndex = %d\n",s32DescLength, pstruProg->u16SubtitlingNum, u8CntIndex);
                if ((s32DescLength >= 8) && u8CntIndex < SUBTDES_INFO_MAX)
                {
                    /* 8859-1 language code */
                    pstruProg->SubtitingInfo[u16SubtIndex].DesInfo[u8CntIndex].u32LangCode =
                                       (pu8DesData[ii*8+0] << 16) | (pu8DesData[ii*8 + 1] << 8) | (pu8DesData[ii*8 + 2]);

                    /* page id */
                    pstruProg->SubtitingInfo[u16SubtIndex].DesInfo[u8CntIndex].u16PageID = (pu8DesData[ii*8 + 4] << 8) | (pu8DesData[ii*8 + 5]);

                    /* ancilary page id */
                    pstruProg->SubtitingInfo[u16SubtIndex].DesInfo[u8CntIndex].u16AncillaryPageID = (pu8DesData[ii*8 + 6] << 8) | (pu8DesData[ii*8 + 7]);

                    pstruProg->SubtitingInfo[u16SubtIndex].u8DesInfoCnt++;
                }
#if 0
                printf("[%d]lang code:%#x\n", pstruProg->u16SubtitlingNum, pstruProg->SubtitingInfo[pstruProg->u16SubtitlingNum].DesInfo[u8CntIndex].u32LangCode);
                printf("[%d]page id :%u\n", pstruProg->u16SubtitlingNum, pstruProg->SubtitingInfo[pstruProg->u16SubtitlingNum].DesInfo[u8CntIndex].u16PageID);
                printf("[%d]ancilary page id : %u\n", pstruProg->u16SubtitlingNum, pstruProg->SubtitingInfo[pstruProg->u16SubtitlingNum].DesInfo[u8CntIndex].u16AncillaryPageID);
                printf("[%d]count is %u\n\n", pstruProg->u16SubtitlingNum, pstruProg->SubtitingInfo[pstruProg->u16SubtitlingNum].u8DesInfoCnt);
                printf("[%d]pid is 0x%x\n\n", pstruProg->u16SubtitlingNum,pstruProg->SubtitingInfo[pstruProg->u16SubtitlingNum].u16SubtitlingPID);
#endif
            }
        }
        break;

        case AC3_DESCRIPTOR:
		case AC3_PLUS_DESCRIPTOR:
			break;

        case COPYRIGHT_DESCRIPTOR:
        case MOSAIC_DESCRIPTOR:
            break;

        case STREAM_IDENTIFIER_DESCRIPTOR:
        case PRIVATE_DATA_SPECIFIER_DESCRIPTOR:
        case SERVICE_MOVE_DESCRIPTOR:
        case CA_SYSTEM_DESCRIPTOR:
        case DATA_BROADCAST_ID_DESCRIPTOR:
            break;
        default:
            break;
        }

        pu8DesData    += u8Length;
        s32DescLength -= (u8Length + 2);
    }
}

int SRH_ParsePAT( unsigned char  *pu8SectionData, int s32Length, unsigned char *pSectionStruct)
{
    unsigned short u16ProgrameNumber = 0;
    unsigned short u16Pid  = 0x1FFF;
    unsigned short u16TsId = 0;
    PAT_TB *pPatTb = (PAT_TB *)pSectionStruct;
    PAT_INFO *pPatInfo;

    if ((NULL == pu8SectionData) || (NULL == pSectionStruct))
    {
        return -1;
    }

#ifdef SRCH_DEBUG
    int index;
    printf("\n");
    for (index = 0; index < s32Length; index++)
    {
        //printf("0x%x ",pu8SectionData[index]);
    }

    printf("\n");
#endif

    if (PAT_TABLE_ID != pu8SectionData[0])
    {
        return -1;
    }

    s32Length = (int)((pu8SectionData[1] << 8) | pu8SectionData[2]) & 0x0fff;

    u16TsId = (unsigned short)((pu8SectionData[3] << 8) | pu8SectionData[4]);

    pPatTb->u16TsID = u16TsId;

    pu8SectionData += 8;
    s32Length -= 9;

    while (s32Length >= 4)
    {
        pPatInfo = &(pPatTb->PatInfo[pPatTb->u16ProgNum]);

        u16ProgrameNumber = (unsigned short)((pu8SectionData[0] << 8) | pu8SectionData[1]);

        u16Pid = (unsigned short)(((pu8SectionData[2] & 0x1F) << 8) | pu8SectionData[3]);

        if (u16ProgrameNumber != 0x0000)
        {
            pPatInfo->u16ServiceID = u16ProgrameNumber;
            pPatInfo->u16PmtPid = u16Pid;
            pPatTb->u16ProgNum++;
#if 1//def SRCH_DEBUG
            printf(" parser PAT get PmtPid %d(0x%x) ServiceID %d(0x%x)  index %d\n",
                   pPatInfo->u16PmtPid, pPatInfo->u16PmtPid, pPatInfo->u16ServiceID, pPatInfo->u16ServiceID, pPatTb->u16ProgNum);
#endif
        }

        pu8SectionData += 4;
        s32Length -= 4;
    }

    return (0);
}

int IsSubtitleStream( unsigned char *pu8Data, int s32Length)
{
    int bRet = 0;

    int nLen = s32Length;
    unsigned char *pData = (unsigned char*)pu8Data;
    unsigned char u8Tag = 0;
    unsigned char u8Length = 0;

    while (nLen > 0)
    {
        u8Tag = *pData++;
        u8Length = *pData++;

        if (u8Tag == SUBTITLING_DESCRIPTOR)
        {
            bRet = 1;
            break;
        }

        pData += u8Length;

        nLen -= (u8Length+2);
    }

    return bRet;
}

int SRH_ParsePMT ( unsigned char *pu8SectionData, int s32Length, unsigned char *pSectionStruct)
{
    unsigned char u8StreamType = 0;

    unsigned short u16DesLen = 0;
    unsigned short u16Pid  = 0x1FFF;
    unsigned char AudioTag = 0;
    PMT_TB *pPmtTb = (PMT_TB *)pSectionStruct;

    if ((NULL == pu8SectionData) || (NULL == pSectionStruct))
    {
        return -1;
    }

    if (PMT_TABLE_ID != pu8SectionData[0])
    {
        return -1;
    }

   // printf("enter length : %d\n", s32Length);

    s32Length = (int)((pu8SectionData[1] << 8) | pu8SectionData[2]) & 0x0fff;

   // printf("s32Length = %d, %#x:%#x\n", s32Length, pu8SectionData[1], pu8SectionData[2]);

    pPmtTb->u16ServiceID = (unsigned short)((pu8SectionData[3] << 8) | pu8SectionData[4]);

    pu8SectionData += 8; /* skip 8-byte to PCR_PID*/

    u16Pid = (unsigned short)(((pu8SectionData[0] & 0x1F) << 8) | pu8SectionData[1]);
    u16DesLen = (unsigned short)(((pu8SectionData[2] & 0x0F) << 8) | pu8SectionData[3]);

    pPmtTb->u16PcrPid = u16Pid;

    pu8SectionData += 4;

    s32Length -= 9;

   // printf("pcr pid = %#x, deslength:%d, s32Length = %d\n", u16Pid, u16DesLen, s32Length);

    if (u16DesLen > 0)
    {
        ParsePMTDesc(pu8SectionData, u16DesLen, pPmtTb);
        AudioTag = *pu8SectionData;
        pu8SectionData += u16DesLen;
        s32Length -= u16DesLen;
    }

    while (s32Length > 4)
    {
        u8StreamType = pu8SectionData[0];
        AudioTag = *pu8SectionData;

        u16Pid = (((pu8SectionData[1] & 0x1F) << 8) | pu8SectionData[2]);
        u16DesLen = (((pu8SectionData[3] & 0x0F) << 8) | pu8SectionData[4]);

        pu8SectionData += 5;
        s32Length -= 5;

        //printf("stream type : %#x, pid:%#x, length:%d, u16DesLen = %d\n", u8StreamType, u16Pid, s32Length, u16DesLen);

        switch (u8StreamType)
        {
            /*video stream*/
        case STREAM_TYPE_14496_10_VIDEO:
        case STREAM_TYPE_14496_2_VIDEO:
        case STREAM_TYPE_11172_VIDEO:
        case STREAM_TYPE_13818_VIDEO:
        case STREAM_TYPE_AVS_VIDEO:
        case 0x24:
        {
           // printf("video stream type is %#x, pid=%#x\n", u8StreamType, u16Pid);
			if (pPmtTb->u16VideoNum < PROG_MAX_VIDEO)
			{
	            pPmtTb->Videoinfo[pPmtTb->u16VideoNum].u16VideoPid = u16Pid;

	            if (u8StreamType == STREAM_TYPE_14496_10_VIDEO)
	            {
	                pPmtTb->Videoinfo[pPmtTb->u16VideoNum].u32VideoEncType = VCODEC_TYPE_H264;//HI_UNF_VCODEC_TYPE_H264;
	            }
	            else if (u8StreamType == STREAM_TYPE_14496_2_VIDEO)
	            {
	                pPmtTb->Videoinfo[pPmtTb->u16VideoNum].u32VideoEncType = VCODEC_TYPE_MPEG4;//HI_UNF_VCODEC_TYPE_MPEG4;
	            }
                else if (u8StreamType == STREAM_TYPE_AVS_VIDEO)
                {
	                pPmtTb->Videoinfo[pPmtTb->u16VideoNum].u32VideoEncType = VCODEC_TYPE_AVS;//HI_UNF_VCODEC_TYPE_AVS;
                }
                else if (0x24==u8StreamType)
                    pPmtTb->Videoinfo[pPmtTb->u16VideoNum].u32VideoEncType = VCODEC_TYPE_H265;
	            else
	            {
	                pPmtTb->Videoinfo[pPmtTb->u16VideoNum].u32VideoEncType = VCODEC_TYPE_MPEG2;//HI_UNF_VCODEC_TYPE_MPEG2;
	            }

                if (u16DesLen > 0)
                {
                    ParsePMTDesc(pu8SectionData, u16DesLen, pPmtTb);
                    pu8SectionData += u16DesLen;
                    s32Length -= u16DesLen;
                }

	            pPmtTb->u16VideoNum++;
			}

            break;
        }

            /*audio stream*/
        case STREAM_TYPE_11172_AUDIO:
        case STREAM_TYPE_13818_AUDIO:
        case STREAM_TYPE_14496_3_AUDIO:
        case STREAM_TYPE_13818_7_AUDIO:
        case STREAM_TYPE_AC3_AUDIO:
        {
           // printf("audio stream type is %#x, pid = %#x\n", u8StreamType, u16Pid);
            if(pPmtTb->u16AudoNum < PROG_MAX_AUDIO)
            {
                pPmtTb->Audioinfo[pPmtTb->u16AudoNum].u16AudioPid = u16Pid;
#ifdef SRCH_DEBUG
                printf(" audio type = 0x%x \n", u8StreamType);
#endif
                if (u8StreamType == STREAM_TYPE_13818_7_AUDIO)
                {
                    pPmtTb->Audioinfo[pPmtTb->u16AudoNum].u32AudioEncType = ACODEC_TYPE_AAC;//HA_AUDIO_ID_AAC;
                }
                else if (u8StreamType == STREAM_TYPE_AC3_AUDIO)
                {
                    pPmtTb->Audioinfo[pPmtTb->u16AudoNum].u32AudioEncType = ACODEC_TYPE_AC3;//HA_AUDIO_ID_DOLBY_PLUS;
                }
                else
                {
                    pPmtTb->Audioinfo[pPmtTb->u16AudoNum].u32AudioEncType = ACODEC_TYPE_MP3;//HA_AUDIO_ID_MP3;
                }

                if (u16DesLen > 0)
                {
                    ParsePMTDesc(pu8SectionData, u16DesLen, pPmtTb);
                    pu8SectionData += u16DesLen;
                    s32Length -= u16DesLen;
                }

                pPmtTb->u16AudoNum++;
            }

            break;
        }
#if 1
        case STREAM_TYPE_PRIVATE:
        {
           // printf("private stream...........pid=%#x\n", u16Pid);
            if (u16DesLen > 0 )/* subtitling stream info */
            {
                if  (IsSubtitleStream(pu8SectionData, u16DesLen))
                {
                    if (SUBTITLING_MAX > pPmtTb->u16SubtitlingNum)
                    {
                        pPmtTb->SubtitingInfo[pPmtTb->u16SubtitlingNum].u16SubtitlingPID = u16Pid;

                        //printf("using pPmtTb->u16SubtitlingNum = %d to parse description\n", pPmtTb->u16SubtitlingNum);

                        ParsePMTDesc(pu8SectionData, u16DesLen, pPmtTb);

                        pPmtTb->u16SubtitlingNum++;
                    }
                    else
                    {
                        printf("Subtitle language is over than the max number:%d\n", SUBTITLING_MAX);
                    }
                }
                else
                {
                	if (pPmtTb->u16AudoNum < PROG_MAX_AUDIO)
					{
						/*dolby parse*/
						pPmtTb->Audioinfo[pPmtTb->u16AudoNum].u16AudioPid = u16Pid;
	                    ParsePMTDesc(pu8SectionData, u16DesLen, pPmtTb);
						AudioTag = *pu8SectionData;


	            		/*if the audio type is AC3 OR AC3_PLUS , use HA_AUDIO_ID_DOLBY_PLUS to decode it*/
						if((AudioTag == AC3_DESCRIPTOR)||(AudioTag == AC3_PLUS_DESCRIPTOR) || (AC3_EXT_DESCRIPTOR == AudioTag))
	            		{
			                pPmtTb->Audioinfo[pPmtTb->u16AudoNum].u32AudioEncType = ACODEC_TYPE_AC3;//HA_AUDIO_ID_DOLBY_PLUS;
			                //printf("> %s: [%d] AudioTag=%#X\n", __FUNCTION__, __LINE__, AudioTag);
			            }
			            else
			            {
			                pPmtTb->Audioinfo[pPmtTb->u16AudoNum].u32AudioEncType = ACODEC_TYPE_MP3;//HA_AUDIO_ID_MP3;
			            }

			            pPmtTb->u16AudoNum++;
                	}
                }

                pu8SectionData += u16DesLen;
		        s32Length -= u16DesLen;

       		}

        }
        break;
#endif
        default:
        {
            if (u16DesLen > 0)
            {
                ParsePMTDesc(pu8SectionData, u16DesLen, pPmtTb);
                pu8SectionData += u16DesLen;
                s32Length -= u16DesLen;
            }
        }
        break;
        }
    }

    return 0;
}

int SRH_ParseSDT( unsigned char *pu8SectionData, int s32Length, unsigned char *pSectionStruct)
{
    unsigned char u8EitFlag = 0;
    unsigned char u8EitFlag_PF = 0;
    unsigned char u8RunStatus = 0;
    unsigned char u8FreeCA = 0;
    unsigned short u16NetworkId = 0x1FFF;
    unsigned short u16ProgramNumber = 0;
    unsigned short u16TsId   = 0;
    int s32DesLen = 0;

    SDT_TB *pSdtTb = (SDT_TB *)pSectionStruct;
    SDT_INFO *pSdtInfo;

    if ((NULL == pu8SectionData) || (NULL == pSectionStruct))
    {
        return -1;
    }

    if (SDT_TABLE_ID_ACTUAL != pu8SectionData[0])
    {
        return -1;
    }

    s32Length = (int)((pu8SectionData[1] << 8) | pu8SectionData[2]) & 0x0fff;

    u16TsId = (unsigned short)((pu8SectionData[3] << 8) | pu8SectionData[4]);
    u16NetworkId = (unsigned short)((pu8SectionData[8] << 8) | pu8SectionData[9]);

    pSdtTb->u16TsId  = u16TsId;
    pSdtTb->u16NetID = u16NetworkId;

    pu8SectionData += 11;
    s32Length -= 11;

    while (s32Length > 4)
    {
        pSdtInfo = &(pSdtTb->SdtInfo[pSdtTb->u32ProgNum]);

        u16ProgramNumber = (unsigned short)((pu8SectionData[0] << 8) | pu8SectionData[1]);

        u8EitFlag = (unsigned char)((pu8SectionData[2] & 0x02) >> 1);
        u8EitFlag_PF = (unsigned char)(pu8SectionData[2] & 0x01);

        u8RunStatus = (unsigned char)((pu8SectionData[3] & 0xE0) >> 5);

        u8FreeCA = (unsigned char)((pu8SectionData[3] & 0x10) >> 4);

        pSdtInfo->u16ServiceID = u16ProgramNumber;
        pSdtInfo->u8EitFlag = u8EitFlag;
        pSdtInfo->u8EitFlag_PF = u8EitFlag_PF;
        pSdtInfo->RunState = (RUN_STATE_E)u8RunStatus;
        pSdtInfo->CAMode = (CA_MODE_E)u8FreeCA;

        s32DesLen = (int)(((pu8SectionData[3] & 0x0F) << 8) | pu8SectionData[4]);

        pu8SectionData += 5;
        s32Length -= 5;

        ParseSDTDesc( pu8SectionData, s32DesLen, pSdtInfo);

        pu8SectionData += s32DesLen;
        s32Length -= s32DesLen;
        pSdtTb->u32ProgNum++;
    }

    return 0;
}

int  DB_AddDVBProg(DB_PROGRAM_S *proginfo_s , DB_PROGRAM_S *proginfo_d, int *count)
{
    int i = 0;

    if ((NULL == proginfo_s) || (NULL == proginfo_d))
    {
        return -1;
    }
    for (i = 0; i < *count; i++)
    {
        if (proginfo_d[i].u16ServiceID == proginfo_s->u16ServiceID)
        {
            return i;
        }
    }

    memcpy((void *)&proginfo_d[*count], (void *)proginfo_s, sizeof(DB_PROGRAM_S));
    (*count)++;
    return *count;
}

int media_info(char *file, DB_PROGRAM_S **ppProgList, int *ProgCount)
{
    DB_PROGRAM_S stProgInfo;
    DB_PROGRAM_S *pProgList=NULL;
    PAT_TB pat_tb;
    PMT_TB pmt_tb;
    unsigned char buf[256], *payload=NULL, *p, u8SectionData[2048];
    int i, j, tspid, flag, step=PAT_TABLE_ID, payloadlen, seclen, seclen_t;
    char payload_unit_start_indicator, adaptation_field_control;

    FILE * fp = NULL;
    fp = fopen(file, "rb");
    if ( NULL==fp )
        return !(*ProgCount = 0);

    p = buf;
    flag = 0;
    do{
        if ( 0 >= fread(buf, 1, 188, fp)){
            break;
        }

        if ( p[0]!=0x47 ){
            continue;
        }
        tspid = (p[1]&0x1f)<<8|p[2];
        payload_unit_start_indicator=p[1]&0x40;
        adaptation_field_control=p[3]&0x30;
        if (adaptation_field_control!=0x20){
            payload = &p[4];
            if (payload_unit_start_indicator==0x40)
            {
                if (adaptation_field_control==0x30 || adaptation_field_control==0x10)
                    payload = &p[p[4]+5];
                if (0==flag)
                {
                    seclen_t = 0;
                    seclen = ((payload[1]&0xf)<<8) | payload[2];
                }
            }
            payloadlen = 188-(payload-p);
        }
        else
            continue;

        if ( tspid == PAT_TSPID ){
            flag = 1;
            memcpy(u8SectionData+seclen_t, payload, payloadlen);
            seclen_t += payloadlen;
            if (seclen_t>=seclen)
            {
                memset(&pat_tb, 0, sizeof(PAT_TB));
                SRH_ParsePAT (u8SectionData, seclen, (unsigned char*)&pat_tb);
                step = PMT_TABLE_ID;
                break;
            }
        }
    }while (1);

    if (step == PMT_TABLE_ID)
    {
        if ( pat_tb.u16ProgNum>0 ){
            pProgList = (DB_PROGRAM_S*)calloc(1, pat_tb.u16ProgNum*sizeof(DB_PROGRAM_S));
        }
        for (i = 0; i < pat_tb.u16ProgNum; i++)
        {
            flag = 0;
            memset(&stProgInfo, 0, sizeof(DB_PROGRAM_S));
            stProgInfo.u16FrontendID = SEARCHING_FRONTEND_ID;
            stProgInfo.u16TsID   = pat_tb.u16TsID;
            stProgInfo.u16PmtPid = pat_tb.PatInfo[i].u16PmtPid;
            stProgInfo.u16ServiceID = pat_tb.PatInfo[i].u16ServiceID;

            memset(&pmt_tb, 0, sizeof(PMT_TB));
            rewind(fp);
            do{
                if ( 0 >= fread(buf, 1, 188, fp)){
                    printf("failed to get pmt, pid=%#x\n", stProgInfo.u16PmtPid);
                    break;
                }

                if ( p[0]!=0x47 ){
                    continue;
                }
                tspid = (p[1]&0x1f)<<8|p[2];
                payload_unit_start_indicator=p[1]&0x40;
                adaptation_field_control=p[3]&0x30;
                if (adaptation_field_control!=0x20){
                    payload = &p[4];
                    if (payload_unit_start_indicator==0x40)
                    {
                        if (adaptation_field_control==0x30 || adaptation_field_control==0x10)
                            payload = &p[p[4]+5];
                        if (0==flag)
                        {
                            seclen_t = 0;
                            seclen = ((payload[1]&0xf)<<8) | payload[2];
                        }
                    }
                    payloadlen = 188-(payload-p);
                }
                else
                    continue;

                if ( tspid == stProgInfo.u16PmtPid ){
                    flag = 1;
                    memcpy(u8SectionData+seclen_t, payload, payloadlen);
                    seclen_t += payloadlen;
                    if (seclen_t>=seclen)
                    {
                        if (0 == SRH_ParsePMT (u8SectionData, seclen, (unsigned char*)&pmt_tb) )
                        {
                            stProgInfo.u16PcrPid = pmt_tb.u16PcrPid;
                            stProgInfo.u8AudioChannel = pmt_tb.u16AudoNum;
                            for (j = 0; j <= pmt_tb.u16AudoNum; j++)
                            {
                                stProgInfo.AudioEX[j].u16audiopid = pmt_tb.Audioinfo[j].u16AudioPid;
                                stProgInfo.AudioEX[j].u32AudioEncType = pmt_tb.Audioinfo[j].u32AudioEncType;
                            }

                            stProgInfo.u8VideoChannel = pmt_tb.u16VideoNum;
                            stProgInfo.VideoEX.u16VideoPid = pmt_tb.Videoinfo[0].u16VideoPid;
                            stProgInfo.VideoEX.u32VideoEncType = pmt_tb.Videoinfo[0].u32VideoEncType;
                        }
                        DB_AddDVBProg(&stProgInfo, pProgList, ProgCount);
                        break;
                    }
                }
            }while (1);
        }
        *ppProgList = pProgList;
    }
    fclose(fp);
    if (step == PMT_TABLE_ID)
        return 0;
    return !(*ProgCount = 0);
}

struct {
    int num;
    struct tuner_module_t* module;
    struct tuner_device_t* device;
    struct tuner_t const* list;
}tuner;

struct {
    int num;
    struct demux_module_t* module;
    struct demux_device_t* device;
    struct demux_t const* list;
}demux;

struct {
    int num;
    struct avdec_module_t* module;
    struct avdec_device_t* device;
    struct avdec_t const* list;
}avdec;
struct {
    int num;
    struct avout_module_t* module;
    struct avout_device_t* device;
    struct avout_t const* list;
}avout;

static unsigned int crc_table[256] = { 0x00000000, 0x04c11db7, 0x09823b6e,
		0x0d4326d9, 0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005, 0x2608edb8,
		0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a,
		0x384fbdbd, 0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac,
		0x5bd4b01b, 0x569796c2, 0x52568b75, 0x6a1936c8, 0x6ed82b7f, 0x639b0da6,
		0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd, 0x9823b6e0,
		0x9ce2ab57, 0x91a18d8e, 0x95609039, 0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52,
		0x8664e6e5, 0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84,
		0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d, 0xd4326d90, 0xd0f37027, 0xddb056fe,
		0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95, 0xf23a8028,
		0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a,
		0xec7dd02d, 0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae, 0x278206ab,
		0x23431b1c, 0x2e003dc5, 0x2ac12072, 0x128e9dcf, 0x164f8078, 0x1b0ca6a1,
		0x1fcdbb16, 0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca, 0x7897ab07,
		0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c, 0x6211e6b5,
		0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066, 0x4d9b3063,
		0x495a2dd4, 0x44190b0d, 0x40d816ba, 0xaca5c697, 0xa864db20, 0xa527fdf9,
		0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692, 0x8aad2b2f,
		0x8e6c3698, 0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d,
		0x94ea7b2a, 0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e, 0xf3b06b3b,
		0xf771768c, 0xfa325055, 0xfef34de2, 0xc6bcf05f, 0xc27dede8, 0xcf3ecb31,
		0xcbffd686, 0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a, 0x690ce0ee,
		0x6dcdfd59, 0x608edb80, 0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c,
		0x774bb0eb, 0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a,
		0x58c1663d, 0x558240e4, 0x51435d53, 0x251d3b9e, 0x21dc2629, 0x2c9f00f0,
		0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b, 0x0315d626,
		0x07d4cb91, 0x0a97ed48, 0x0e56f0ff, 0x1011a0fa, 0x14d0bd4d, 0x19939b94,
		0x1d528623, 0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2,
		0xe6ea3d65, 0xeba91bbc, 0xef68060b, 0xd727bbb6, 0xd3e6a601, 0xdea580d8,
		0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3, 0xbd3e8d7e,
		0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2, 0xaafbe615, 0xa7b8c0cc,
		0xa379dd7b, 0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f, 0x8832161a,
		0x8cf30bad, 0x81b02d74, 0x857130c3, 0x5d8a9099, 0x594b8d2e, 0x5408abf7,
		0x50c9b640, 0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c, 0x7b827d21,
		0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a, 0x61043093,
		0x65c52d24, 0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30, 0x029f3d35,
		0x065e2082, 0x0b1d065b, 0x0fdc1bec, 0x3793a651, 0x3352bbe6, 0x3e119d3f,
		0x3ad08088, 0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654, 0xc5a92679,
		0xc1683bce, 0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb,
		0xdbee767c, 0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18, 0xf0a5bd1d,
		0xf464a0aa, 0xf9278673, 0xfde69bc4, 0x89b8fd09, 0x8d79e0be, 0x803ac667,
		0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c, 0xafb010b1,
		0xab710d06, 0xa6322bdf, 0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03,
		0xb1f740b4 };

static unsigned int dvb_crc32(const unsigned char *data, int len) {
	register int i;
	unsigned int crc = 0xffffffff;

	for (i = 0; i < len; i++) {
		crc = (crc << 8) ^ crc_table[((crc >> 24) ^ *data++) & 0xff];
	}

	return crc;
}

extern char *optarg;
extern int optind, opterr, optopt;
static int testrunning=0, testout=1;
static pthread_t testthread;

int tuner_prepare(int freq, int sym, int mod)
{
    int ret, timeout=100, lock, fd;
    char buffer[128];
    ret = hw_get_module(TUNER_HARDWARE_MODULE_ID, (const hw_module_t**) &tuner.module);
    if (0!=ret)
        return -1;
    tuner.num = tuner.module->get_tuner_list(tuner.module, &tuner.list);
    ret = tuner_open (&tuner.module->common, &tuner.device);
    if (0!=ret)
        return -1;
    tuner.device->set_singal_mode(tuner.device, tuner.list->handle, TUNER_SIGNAL_MODE_DIGITAL);
    snprintf(buffer, sizeof(buffer), "frequency://%d?delivery=cable&symbol_rate=%d000&modulation=qam%d", freq, sym, mod);
    const char *par = buffer;//"frequency://299000000?delivery=cable&symbol_rate=6875000&modulation=qam64";
    ret = tuner.device->lock_delivery(tuner.device, tuner.list->handle, freq, par);
    if (0!=ret)
        return -1;
    while (timeout-->0)
    {
        if (0==tuner.device->is_locked(tuner.device, tuner.list->handle, &lock, &fd) && 1==lock)
            return 0;
        usleep(100000);
    }
    return -1;
}

int avdec_prepare(void)
{
    int rc;
    rc = hw_get_module(AVDEC_HARDWARE_MODULE_ID, (const hw_module_t**) &avdec.module);
    if (0!=rc)
        return -1;
    avdec.num = avdec.module->get_avdec_list(avdec.module, &avdec.list);
    rc = avdec_open (&avdec.module->common, &avdec.device);
    if (0!=rc)
        return -1;
    return 0;
}

int demux_prepare(void)
{
    int rc;
    rc = hw_get_module(DEMUX_HARDWARE_MODULE_ID, (const hw_module_t**) &demux.module);
    if (0!=rc)
        return -1;
   demux.num = demux.module->get_demux_list(demux.module, &demux.list);
    rc = demux_open (&demux.module->common, &demux.device);
    if (0!=rc)
        return -1;
    return 0;
}

int avout_prepare(void)
{
    int rc;
    rc = hw_get_module(AVOUT_HARDWARE_MODULE_ID, (const hw_module_t**) &avout.module);
    if (0!=rc)
        return -1;
    avout.num = avout.module->get_avout_list(avout.module, &avout.list);
    rc = avout_open (&avout.module->common, &avout.device);
    if (0!=rc)
        return -1;
    return 0;
}

static void* test_filter(void*args)
{
    testout = 0;
    int pid=0, coe=0, mask=0xff, exc=0;
    int i,j,k, rc, idemux;
    int psich, psiflt, data_size, timeout;
    demux_filter_param_t fparam;
    unsigned char *p, *ps;
    int sectionlen, crc0, crc1;
    for (i=0;i<demux.num;i++)
    {
        if (demux.list[i].support_tuner_src||demux.list[i].support_push_src){
            idemux = i;
            break;
        }
    }

    if ( args ){
        int * p = (int *)args;
        pid = *p++;
        coe = *p++;
        mask = *p++;
        exc = *p;
    }

    demux.device->bind_tuner_src(demux.device, demux.list[idemux].handle, tuner.device, tuner.list->handle);
    psich = demux.device->create_channel(demux.device, demux.list[idemux].handle, 8*1024*1024, DEMUX_CH_TYPE_SECTION, 0);
    demux.device->start_channel(demux.device, demux.list[idemux].handle, psich, pid);
    psiflt= demux.device->add_filter(demux.device, demux.list[idemux].handle, psich);
    memset(&fparam, 0, sizeof(fparam));
    fparam.depth = 1;
    fparam.coef[0] = coe;
    fparam.mask[0] = mask;
    fparam.excl[0] = exc;
    demux.device->start_filter(demux.device, demux.list[idemux].handle, psich, psiflt, &fparam);
    timeout = 100;
    while ( testrunning ){
        if ( demux.device->poll_channels(demux.device, demux.list[idemux].handle, (unsigned char*)&psich, 1)){
            data_size = demux.device->acquire_channel_data(demux.device, demux.list[idemux].handle, psich, &p, &data_size);
            if ( data_size ) {
                ps = p;
                while (p-ps<data_size){
                    sectionlen = (((p[1]<<8)|p[2])&0x0fff)+3;
                    LOGD("tid=%02x len=%d\n", p[0], sectionlen);
                    crc0 = (p[sectionlen-4]<<24)|(p[sectionlen-3]<<16)|(p[sectionlen-2]<<8)|p[sectionlen-1];
                    crc1 = dvb_crc32 (p, sectionlen-4);
                    if (crc0!=crc1){
                        LOGE("crc0=%08x, crc1=%08x", crc0, crc1);
                    }
                    p+=sectionlen;
                }
                demux.device->release_channel_data(demux.device, demux.list[idemux].handle, psich, data_size);
            }
        }else
            usleep(10000);
    }
    demux.device->stop_filter(demux.device, demux.list[idemux].handle, psich, psiflt);
    demux.device->remove_filter(demux.device, demux.list[idemux].handle, psich, psiflt);
    demux.device->stop_channel(demux.device, demux.list[idemux].handle, psich);
    demux.device->destroy_channel(demux.device, demux.list[idemux].handle, psich);
    testout = 1;
    return NULL;
}

static void *test_play(void * args)
{
    testout = 0;
    int i,j,k, rc, iadec, ivdec, idemux, iavout, ilayer, iach, ivch, ipch;
    int hframer, psich, psiflt, data_size, timeout, hsounder;
    demux_filter_param_t fparam;
    unsigned char *p =NULL;
    int ProgTotalCount = 0;
    DB_PROGRAM_S *pProgList=NULL;
    DB_PROGRAM_S stProgInfo;
    PAT_TB pat_tb;
    PMT_TB pmt_tb;
    SDT_TB sdt_tb;
    char bl[128];
    char * file = (char*)args;

    iadec = ivdec = idemux = iavout = ilayer = -1;
    for (i=0;i<avdec.num;i++)
    {
        if ( avdec.list[i].type==MEDIA_TYPE_AUDIO&&avdec.list[i].support_demux_src&&iadec==-1)
            iadec=i;
        else if (avdec.list[i].type==MEDIA_TYPE_VIDEO&&avdec.list[i].support_demux_src&&ivdec==-1)
            ivdec=i;
    }
    for (i=0;i<demux.num;i++)
    {
        if (demux.list[i].support_tuner_src||demux.list[i].support_push_src){
            idemux = i;
            break;
        }
    }
    for (i=0;i<avout.num;i++)
    {
        iavout = i;
        break;
    }
    for (i=0;i<AVOUT_LAYER_IDMAXSIZE;i++)
    {
        if (avout.list[iavout].layer[i].is_valid_layer && (
            avout.list[iavout].layer[i].layer_type==AVOUT_LAYER_ID_UNDERLAY2
            ||avout.list[iavout].layer[i].layer_type==AVOUT_LAYER_ID_UNDERLAY)){
            ilayer = i;
            break;
        }
    }

    if (!file){
        demux.device->bind_tuner_src(demux.device, demux.list[idemux].handle, tuner.device, tuner.list->handle);
        psich = demux.device->create_channel(demux.device, demux.list[idemux].handle, 0, DEMUX_CH_TYPE_SECTION, 0);
        demux.device->start_channel(demux.device, demux.list[idemux].handle, psich, PAT_TSPID);
        psiflt= demux.device->add_filter(demux.device, demux.list[idemux].handle, psich);
        memset(&fparam, 0, sizeof(fparam));
        fparam.depth = 1;
        fparam.coef[0] = PAT_TABLE_ID;
        fparam.mask[0] = 0xff;
        demux.device->start_filter(demux.device, demux.list[idemux].handle, psich, psiflt, &fparam);
        timeout = 100;
        while ( timeout-->0 ){
            if ( demux.device->poll_channels(demux.device, demux.list[idemux].handle, (unsigned char*)&psich, 1)){
                break;
            }
            usleep(10000);
        }
        if (timeout>0){
            data_size = demux.device->acquire_channel_data(demux.device, demux.list[idemux].handle, psich, &p, &data_size);
            if ( data_size>0 ){
                memset(&pat_tb, 0, sizeof(PAT_TB));
                SRH_ParsePAT(p, data_size, (unsigned char*)&pat_tb);
                demux.device->release_channel_data(demux.device, demux.list[idemux].handle, psich, data_size);
            }
        }
        demux.device->stop_filter(demux.device, demux.list[idemux].handle, psich, psiflt);
        demux.device->stop_channel(demux.device, demux.list[idemux].handle, psich);
        if ( pat_tb.u16ProgNum > 0 )
            pProgList = (DB_PROGRAM_S*)calloc(1, pat_tb.u16ProgNum*sizeof(DB_PROGRAM_S));
        for (i = 0; i < pat_tb.u16ProgNum; i++)
        {
            memset(&stProgInfo, 0, sizeof(DB_PROGRAM_S));
            stProgInfo.u16FrontendID = SEARCHING_FRONTEND_ID;
            stProgInfo.u16TsID   = pat_tb.u16TsID;
            stProgInfo.u16PmtPid = pat_tb.PatInfo[i].u16PmtPid;
            stProgInfo.u16ServiceID = pat_tb.PatInfo[i].u16ServiceID;
            demux.device->start_channel(demux.device, demux.list[idemux].handle, psich, stProgInfo.u16PmtPid);
            memset(&fparam, 0, sizeof(fparam));
            fparam.depth = 1;
            fparam.coef[0] = PMT_TABLE_ID;
            fparam.mask[0] = 0xff;
            demux.device->start_filter(demux.device, demux.list[idemux].handle, psich, psiflt, &fparam);
            timeout = 100;
            while ( timeout-->0 ){
                if ( demux.device->poll_channels(demux.device, demux.list[idemux].handle, (unsigned char*)&psich, 1)){
                    break;
                }
                usleep(10000);
            }

            if (timeout>0){
                data_size = demux.device->acquire_channel_data(demux.device, demux.list[idemux].handle, psich, &p, &data_size);
                if (data_size>0){
                    memset(&pmt_tb, 0, sizeof(PMT_TB));
                    if (0 == SRH_ParsePMT (p, data_size, (unsigned char*)&pmt_tb) )
                    {
                        stProgInfo.u16PcrPid = pmt_tb.u16PcrPid;
                        stProgInfo.u8AudioChannel = pmt_tb.u16AudoNum;
                        for (j = 0; j <= pmt_tb.u16AudoNum; j++)
                        {
                            stProgInfo.AudioEX[j].u16audiopid = pmt_tb.Audioinfo[j].u16AudioPid;
                            stProgInfo.AudioEX[j].u32AudioEncType = pmt_tb.Audioinfo[j].u32AudioEncType;
                        }

                        stProgInfo.u8VideoChannel = pmt_tb.u16VideoNum;
                        stProgInfo.VideoEX.u16VideoPid = pmt_tb.Videoinfo[0].u16VideoPid;
                        stProgInfo.VideoEX.u32VideoEncType = pmt_tb.Videoinfo[0].u32VideoEncType;
                        DB_AddDVBProg(&stProgInfo, pProgList, &ProgTotalCount);
                    }
                    demux.device->release_channel_data(demux.device, demux.list[idemux].handle, psich, data_size);
                }
            }
            demux.device->stop_filter(demux.device, demux.list[idemux].handle, psich, psiflt);
            demux.device->stop_channel(demux.device, demux.list[idemux].handle, psich);
        }
        demux.device->remove_filter(demux.device, demux.list[idemux].handle, psich, psiflt);
        demux.device->destroy_channel(demux.device, demux.list[idemux].handle, psich);
    }
    else{
        demux.device->bind_push_src(demux.device, demux.list[idemux].handle);
        media_info(file, &pProgList, &ProgTotalCount);
    }
    if (ProgTotalCount<=0){
        testout = 1;
        testrunning = 0;
        printf("no services found\n");
    	return NULL;
    }

    hframer = avout.device->framer_create(avout.device, avout.list[iavout].handle, ilayer, 1);
    avout.device->framer_bind_vdec_src(avout.device, avout.list[iavout].handle, hframer, 0, avdec.device, avdec.list[ivdec].handle);
    hsounder = avout.device->sounder_create(avout.device, avout.list[iavout].handle);
    avout.device->sounder_bind_adec_src(avout.device, avout.list[iavout].handle, hsounder, avdec.device, avdec.list[iadec].handle);

    ivch = demux.device->create_channel(demux.device, demux.list[idemux].handle, 0, DEMUX_CH_TYPE_VIDEO, 0);
    iach = demux.device->create_channel(demux.device, demux.list[idemux].handle, 0, DEMUX_CH_TYPE_AUDIO, 0);
    ipch = demux.device->create_channel(demux.device, demux.list[idemux].handle, 0, DEMUX_CH_TYPE_PCR, 0);
    avdec.device->bind_demux_src(avdec.device, avdec.list[iadec].handle, demux.device, demux.list[idemux].handle, iach);
    avdec.device->bind_demux_src(avdec.device, avdec.list[ivdec].handle, demux.device, demux.list[idemux].handle, ivch);
    //avdec.device->bind_demux_src(avdec.device, avdec.list[iadec].handle, demux.device, demux.list[idemux].handle, ipch);
    //avdec.device->bind_demux_src(avdec.device, avdec.list[ivdec].handle, demux.device, demux.list[idemux].handle, ipch);

    char value[PROPERTY_VALUE_MAX];
    property_get("dtvtest.avdec.sync", value, "1");
    avdec.device->set_sync(avdec.device, avdec.list[iadec].handle, avdec.list[ivdec].handle, ipch);

    i=0;
play_program:
    i %= ProgTotalCount;
    printf("program info:%d\n\tServiceID:%d\tProgramName:%s\n",
        i, pProgList[i].u16ServiceID, NULL/*ProgList[i].ProgramName*/);
    printf("\tvpid:%#x\tvfmt:%#x\tppid:%#x\n",
        pProgList[i].VideoEX.u16VideoPid, pProgList[i].VideoEX.u32VideoEncType, pProgList[i].u16PcrPid);
    for (k=0;k<pProgList[i].u8AudioChannel;k++)
        printf("\tapid:%#x\tafmt:%#x\n", pProgList[i].AudioEX[k].u16audiopid, pProgList[i].AudioEX[k].u32AudioEncType);

    demux.device->start_channel(demux.device, demux.list[idemux].handle, iach, pProgList[i].AudioEX[0].u16audiopid);
    demux.device->start_channel(demux.device, demux.list[idemux].handle, ivch, pProgList[i].VideoEX.u16VideoPid);
    demux.device->start_channel(demux.device, demux.list[idemux].handle, ipch, pProgList[i].u16PcrPid);
    avdec.device->set_format(avdec.device, avdec.list[ivdec].handle, pProgList[i].VideoEX.u32VideoEncType, VDEC_DECODER_FLAG_HD);
    avdec.device->set_format(avdec.device, avdec.list[iadec].handle, pProgList[i].AudioEX[0].u32AudioEncType, VDEC_DECODER_FLAG_HD);
    avdec.device->start(avdec.device, avdec.list[iadec].handle);
    avdec.device->start(avdec.device, avdec.list[ivdec].handle);

    if (file){
        FILE * fp;
        unsigned char buf[188*100];
        int len, len0=0;
        fp = fopen(file, "rb");
        if (!fp) {
            LOGD("%s not found", file);
            goto stop;
        }
        while (testrunning)
        {
            if ( 0==len0 ){
                len = fread(buf, 1, 188*100, fp);
                if (len<=0){
                    rewind(fp);
                    continue;
                }
            }
            else
                len = len0;
            len0 = demux.device->push_data(demux.device, demux.list[idemux].handle, buf, len);
            if (len0==0){
                usleep(10000);
            }
            else if (len!=len0){
                memmove(buf, buf+len0, len-len0);
                usleep(10000);
            }
            len0 = len-len0;
        }
    }
    else{
        printf("totalprograms:%d, now play program:%d\n", ProgTotalCount, i);
        fgets(bl, sizeof(bl), stdin);
    }
stop:
    avdec.device->stop(avdec.device, avdec.list[iadec].handle, 0);
    avdec.device->stop(avdec.device, avdec.list[ivdec].handle, 0);
    demux.device->stop_channel(demux.device, demux.list[idemux].handle, iach);
    demux.device->stop_channel(demux.device, demux.list[idemux].handle, ivch);
    demux.device->stop_channel(demux.device, demux.list[idemux].handle, ipch);
    if (!file){
        if ( bl[0]!='q' )
        {
            if (bl[0]>='0' && bl[0]<ProgTotalCount+'0')
                i = bl[0]-'0';
            goto play_program;
        }
        testrunning = 0;
    }
    demux.device->destroy_channel(demux.device, demux.list[idemux].handle, iach);
    demux.device->destroy_channel(demux.device, demux.list[idemux].handle, ivch);
    demux.device->destroy_channel(demux.device, demux.list[idemux].handle, ipch);
    testout = 1;
    return NULL;
}


int main(int argc, char**argv) {
    int test, freq, qam, sym, pid, coe, mask, exc;
    int ch;
    char * file;
    int args[10];
    char cmd[128];

    test = 1;
    freq = 259;
    qam = 64;
    sym = 6875;
    pid = 0;
    coe = 0;
    mask = 0xff;
    exc = 0;
    file = NULL;

    opterr = 0;
    while((ch = getopt(argc, argv, "t:f:q:s:p:c:m:e:i:"))!= -1)
    {
        switch(ch)
        {
        case 't':
            test = atoi(optarg);
            printf("test:%d\n", test);
            break;
        case 'f':
            freq = atoi(optarg);
            printf("frequency=%d\n", freq);
            break;
        case 'q':
            qam = atoi(optarg);
            printf("qam=%d\n", qam);
            break;
        case 's':
            sym= atoi(optarg);
            printf("symblerate=%d\n", sym);
            break;
        case 'p':
            pid= atoi(optarg);
            printf("pid=%d\n", pid);
            break;
        case 'c':
            coe= atoi(optarg);
            printf("coefficient=%d\n", coe);
            break;
        case 'm':
            mask= atoi(optarg);
            printf("mask=%d\n", mask);
            break;
        case 'e':
            exc= atoi(optarg);
            printf("exclusion=%d\n", exc);
            break;
        case 'i':
            file = optarg;
            printf("input file=%s\n", file);
            break;

        default:
            printf("unknow option:%c\n", ch);
            break;
        }
    }

    LOGD("test start\n");

    if (test==1 || test==2){ //live play or message filter test
        LOGD("tuner_prepare");
        if ( tuner_prepare(freq*1000000, sym, qam) ){
            LOGD("lock failed");
            return 0;
        }
    }

    LOGD("demux_prepare");
    demux_prepare();
    testrunning = 1;
    if (test==2){
        args[0]=pid;
        args[1]=coe;
        args[2]=mask;
        args[3]=exc;
        pthread_create(&testthread, NULL, test_filter, (void*)args);
    }else{
        LOGD("avout_prepare");
        avout_prepare();
        LOGD("avdec_prepare");
        avdec_prepare();
        if (test==1) file=NULL;
        pthread_create(&testthread, NULL, test_play, (void*)file);
    }

    while ( test!=1 ){
        fgets(cmd, sizeof(cmd), stdin);
        if ( cmd[0]=='q' )
        {
            testrunning = 0;
            break;
        }
    }

    while ( testrunning || 0==testout ){
        sleep(1);
    }

    LOGE("test out\n");
    return 0;
}

