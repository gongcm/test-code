/* ----------------------------------------------------------------------------
 * @module 
 *			HAL module alternately interface
 * @routines 
 *			 alternately device (use tuner,demux,av dec/out common call)
 * @date
 *			2011.4
 */

#ifndef JOINTV_COMMON_HAL_INTERFACE_H
#define JOINTV_COMMON_HAL_INTERFACE_H

#include <stdlib.h>

#include <hi_type.h>

#include <hardware/tuner.h>
#include <hardware/demux.h>
#include <hardware/avout.h>
#include <hardware/avdec.h>
#include <hardware/smartcard.h>
#include <hardware/hidev.h>
#include <hardware/hardware.h>


__BEGIN_DECLS


#define MFUN_EXTERN extern
#define MFUN_DECL  

/**Definition of the DEMUX ID */
/**CNcomment:DEMUX ID定义*/
/*hisi接口最新代码已经废弃自己定义下为了阅读方便
#define HI_UNF_DMX_ID_0_PLAY            0
#define HI_UNF_DMX_ID_1_REC             1
#define HI_UNF_DMX_ID_2_REC             2
#define HI_UNF_DMX_ID_3_REC             3
#define HI_UNF_DMX_ID_4_PLAY            4
*/

MFUN_EXTERN  int MFUN_DECL
mce_setflag(int flag);

MFUN_EXTERN  int MFUN_DECL
mce_getflag(void);

#define TUNER_MAX_NUM (1)
#define TUNER_MIN_FREQUENCY ( 47000000LL)
#define TUNER_MAX_FREQUENCY (862000000LL)

typedef struct mtuner_t{
    int id;
    int isopen;
    int demux;
}mtuner_t;

typedef struct tuner_context_t {
    struct tuner_device_t device;
    tuner_t *list;
    mtuner_t tuner[TUNER_MAX_NUM];
} tuner_context_t;

#define DEMUX_MAX_NUM       (2)
#define DEMUX_MAX_CHANNEL   (32)
#define DEMUX_MAX_SECTION   (256)
#define DEMUX_POOLSIZE_MIN  (32*1024)
#define DEMUX_POOLSIZE_MAX  (128*1024)
#define DEMUX_TS_BUFSIZE    (4*1024*1024)

typedef struct filter_t{
    int id;
    HI_HANDLE handle;
}filter_t;

typedef struct channel_t{
    int id;
    int type;
    int pid;
    HI_HANDLE handle;
    int buflen;
    unsigned char * pbuf;
    int dsm;
}channel_t;

typedef struct descrambler_t{
    int id;
    HI_HANDLE handle;
    int chi;
    int attach;
}descrambler_t;

typedef struct mdemux_t{
    int id;
    int dmxid;
    int port;
    int mode;
    HI_HANDLE ts_buf;
    channel_t channel[DEMUX_MAX_CHANNEL];
    filter_t filter[DEMUX_MAX_CHANNEL];
    descrambler_t desc[DEMUX_MAX_CHANNEL];
}mdemux_t;

typedef struct demux_context_t {
	struct demux_device_t device;
	struct demux_t *list;
	mdemux_t demux[DEMUX_MAX_NUM];
} demux_context_t;

#define AVDEC_MAX_NUM (4)

typedef struct avplayer_t{
    int id;
    HI_HANDLE handle;
    int stat;
    int format;
    int source;
    int pcrchn;
    union {
        struct {
            struct avout_device_t *dev;
            int handle;
            int sounder;
            int id;
            int attach;
        }snd;
        struct {
            struct avout_device_t *dev;
            int handle;
            int framer;
            int id;
            int attach;
        }win;
    }out;
    struct {
        struct demux_device_t *dev;
        int handle;
        int chnid;
    }dmx;
    struct avplayer_t *community;
}avplayer_t;

/* video decoder context struct */
typedef struct avdec_context_t {
	struct avdec_device_t device;
    avdec_t *list;
    avplayer_t avdec[AVDEC_MAX_NUM];
} avdec_context_t;

#define AVOUT_MAX_NUM (2)

typedef struct snd_win_t {
    int id;
    int dispwind;//@duanzh
    struct {
        int id;
        HI_HANDLE handle;
        int volume;
        avplayer_t * adec;
    }snd[AVOUT_MAX_NUM];
    struct {
        int id;
        HI_HANDLE handle;
        struct int16_rect_t r;
        int freeze;
        avplayer_t * vdec;
    }win[AVOUT_MAX_NUM];
}snd_win_t;

typedef struct avout_context_t {
    struct avout_device_t device;
    avout_t *list;
    snd_win_t avout[AVOUT_MAX_NUM];
} avout_context_t;

__END_DECLS

#endif /* JOINTV_COMMON_HAL_INTERFACE_H */
