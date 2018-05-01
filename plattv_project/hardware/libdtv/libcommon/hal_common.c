/* ----------------------------------------------------------------------------
 * @module 
 *			HAL module alternately interface
 * @routines 
 *			 alternately device (use tuner,demux,av dec/out common call)
 * @date
 *			2011.4
 */

#define LOG_TAG "[plat]common"

#include "hal_common.h"
#include <cutils/log.h>
#define HI_CHANNEL_MAX_NUMS    	16
#define HI_AVPLAYER_MAX_NUMS    	16
#define HI_WINDOW_MAX_NUMS		16
#define HI_AVOUT_MAX_NUMS		2 // 1
#define HAL_AUDIO_TYPE       0x00
#define HAL_VIDEO_TYPE      0x01
#define HAL_PCR_TYPE        0x02
#define AV_CHANNEL_ID_BASE            100

#define HAL_CHECK_CHID(chid) (100 <= chid&&chid <148)
/* save all global data */
typedef struct hal_demux_channel_info_t {
	int flag;//0x04 0x02 0x01 0x000 值分别表示3种通道使用情况
	struct {
		int chid;
		int pid;
		int type;
		int status;
	} ch[3];
	void* av_play;
} hdc_info;

typedef struct hal_avplay_info_t {
	struct hal_decode_t {
		int type;
		int id;
		HI_HANDLE dec_handle;
		int status;
	} dec[3];
	HI_HANDLE play_handle;
	void* ch_info;
	HI_HANDLE win_handle;
	HI_HANDLE track_handle;
} hav_info;

typedef struct hal_avplay_window_t {
	int out_handle;
	HI_HANDLE win_handle[16];//一共16个 保证0下标表示base,目前只有一层可用所以定义为一维数组
	HI_HANDLE track_handle[16];
} hav_avout;

static hdc_info ch_info[HI_CHANNEL_MAX_NUMS];
static hav_info av_play_info[HI_AVPLAYER_MAX_NUMS];//下标0表示base播放器其他表示马赛克播放器
static hav_avout av_out[HI_AVOUT_MAX_NUMS];//目前只有一个avout
static int visable_avhandle=0;

static int is_share_data_init = 0;
static int m_trackid = 0;
static HI_HANDLE dmx_tsbuf = (HI_HANDLE) NULL;
static int start_sta = 0;
static int play_freeze[HI_AVPLAYER_MAX_NUMS];

static void hal_share_init() {
	if (is_share_data_init == 1)
		return;
	int i = 0, j = 0, k = 0;
	memset((void*) ch_info, 0x00, sizeof(ch_info));
	memset((void*) av_play_info, 0x00, sizeof(av_play_info));
	memset((void*) av_out, 0x00, sizeof(av_out));
    for(i=0;i<HI_AVOUT_MAX_NUMS;i++)
        av_out[i].out_handle=i;
	for (i = 0, j = 0, k = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		av_play_info[i].dec[HAL_AUDIO_TYPE].type = HAL_AUDIO_TYPE;
		av_play_info[i].dec[HAL_VIDEO_TYPE].type = HAL_VIDEO_TYPE;
		av_play_info[i].dec[HAL_PCR_TYPE].type = HAL_PCR_TYPE;
		av_play_info[i].dec[HAL_AUDIO_TYPE].dec_handle = j++;
		av_play_info[i].dec[HAL_VIDEO_TYPE].dec_handle = j++;
		ch_info[i].ch[HAL_AUDIO_TYPE].type = HAL_AUDIO_TYPE;
		ch_info[i].ch[HAL_VIDEO_TYPE].type = HAL_VIDEO_TYPE;
		ch_info[i].ch[HAL_PCR_TYPE].type = HAL_PCR_TYPE;
		ch_info[i].ch[HAL_AUDIO_TYPE].chid = AV_CHANNEL_ID_BASE + k++;
		ch_info[i].ch[HAL_VIDEO_TYPE].chid = AV_CHANNEL_ID_BASE + k++;
		ch_info[i].ch[HAL_PCR_TYPE].chid = AV_CHANNEL_ID_BASE + k++;
	}
	memset(play_freeze,0,sizeof(play_freeze));
	is_share_data_init = 1;
	return;
}
MFUN_EXTERN int MFUN_DECL
demux_get_idle_virch(int flag) {
	hal_share_init();
	int i = 0;
	int ret = -1;
	switch (flag) {
	case HAL_AUDIO_TYPE: {
		for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
			if (!(ch_info[i].flag & 0x04)) {
				ret = ch_info[i].ch[HAL_AUDIO_TYPE].chid;
				ch_info[i].flag |= 0x04;
				break;
			}
		}
		break;
	}
	case HAL_VIDEO_TYPE: {
		for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
			if (!(ch_info[i].flag & 0x02)) {
				ret = ch_info[i].ch[HAL_VIDEO_TYPE].chid;
				ch_info[i].flag |= 0x02;
				break;
			}
		}
		break;
	}
	case HAL_PCR_TYPE: {
		for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
			if (!(ch_info[i].flag & 0x01)) {
				ret = ch_info[i].ch[HAL_PCR_TYPE].chid;
				ch_info[i].flag |= 0x01;
				break;
			}
		}
		break;
	}
	default: {
		LOGE("demux_get_idle_virch > flag error!");
		break;
	}
	};
	LOGD("demux_get_idle_virch > ret = 0x%x", ret);
	return ret;
}

MFUN_EXTERN int MFUN_DECL
demux_release_virch(int chid) {
	LOGD("demux_release_virch chid = 0x%x",chid);
	int ret = -1, i = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		if (ch_info[i].ch[HAL_AUDIO_TYPE].chid == chid) {
			ch_info[i].flag &= 0x03;
			ret = 0;
			break;
		}
		if (ch_info[i].ch[HAL_VIDEO_TYPE].chid == chid) {
			ch_info[i].flag &= 0x05;
			ret = 0;
			break;
		}
		if (ch_info[i].ch[HAL_PCR_TYPE].chid == chid) {
			ch_info[i].flag &= 0x06;
			ret = 0;
			break;
		}
	}
	LOGD("demux_release_virch > ret = 0x%x", ret);
	return ret;
}

MFUN_EXTERN int MFUN_DECL
demux_set_pid(int chid, int pid) {
	LOGD("demux_set_pid > chid xxxxxx= 0x%x,pid = 0x%x", chid, pid);
	hal_share_init();
	int i = 0, j = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		for (j = 0; j < HAL_PCR_TYPE + 1; j++) {
			if (ch_info[i].ch[j].chid == chid) {
				ch_info[i].ch[j].pid = pid;
				return 0;
			}
		}
	}
	LOGE("demux_set_pid fail!");
	return -1;
}

MFUN_EXTERN int MFUN_DECL
demux_get_pid(int chid, int *pid) {
	hal_share_init();
	LOGD("demux_get_pid > chid xxxxxxx= 0x%x", chid);
	int i = 0, j = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		for (j = 0; j < HAL_PCR_TYPE + 1; j++) {
			if (ch_info[i].ch[j].chid == chid) {
				*pid = ch_info[i].ch[j].pid;
				return 0;
			}
		}
	}
	LOGE("demux_get_pid fail!");
	return -1;
}

MFUN_EXTERN int MFUN_DECL
avdec_set_pid(HI_HANDLE dec_haldle,int pid){
	hal_share_init();
	LOGD("avdec_set_pid > dec_haldle = 0x%x,pid = 0x%x", dec_haldle,pid);
	int i = 0, j = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		for (j = 0; j < HAL_PCR_TYPE + 1; j++) {
			LOGD("avdec_set_pid > 0");
			if (av_play_info[i].dec[j].dec_handle == dec_haldle) {
				LOGD("avdec_set_pid succ");
				((hdc_info*) (av_play_info[i].ch_info))->ch[av_play_info[i].dec[j].type].pid = pid;
				return 0;
			}
		}
	}
	return -1;
}

MFUN_EXTERN int MFUN_DECL
avdec_get_pid(HI_HANDLE dec_haldle, int *pid) {
	hal_share_init();
	LOGD("avdec_get_pid > dec_haldle = 0x%x", dec_haldle);
	int i = 0, j = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		for (j = 0; j < HAL_PCR_TYPE + 1; j++) {
			LOGD("avdec_get_pid > 0");
			if (av_play_info[i].dec[j].dec_handle == dec_haldle) {
				LOGD("avdec_get_pid > 1");
				LOGD("avdec_get_pid >av_play_info[i].dec[j].type = %d", av_play_info[i].dec[j].type);
				LOGD("avdec_get_pid >av_play_info[i].ch_info = %p", av_play_info[i].ch_info);
				*pid = ((hdc_info*) (av_play_info[i].ch_info))->ch[av_play_info[i].dec[j].type].pid;
				LOGD("avdec_get_pid pid =  0x%x",*pid);
				return 0;
			}
		}
	}
	return -1;
}

MFUN_EXTERN int MFUN_DECL
demux_get_type_by_chid(int chid) {
	hal_share_init();
	int i = 0, j = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		for (j = 0; j < HAL_PCR_TYPE + 1; j++) {
			if (ch_info[i].ch[j].chid == chid) {
				return ch_info[i].ch[j].type;
			}
		}
	}
	return -1;
}

MFUN_EXTERN int MFUN_DECL
avdec_set_avplay_handle(HI_HANDLE dec_haldle, HI_HANDLE play_handle) {
	hal_share_init();
	int i = 0, j = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		for (j = 0; j < HAL_PCR_TYPE + 1; j++) {
			if (av_play_info[i].dec[j].dec_handle == dec_haldle) {
				av_play_info[i].play_handle = play_handle;
				return 0;
			}
		}
	}
	return -1;
}
MFUN_EXTERN HI_HANDLE MFUN_DECL
avdec_get_avplay_handle(HI_HANDLE dec_haldle) {
	hal_share_init();
	int ret = -1, i = 0, j = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		for (j = 0; j < HAL_PCR_TYPE + 1; j++) {
			if (av_play_info[i].dec[j].dec_handle == dec_haldle) {
				return av_play_info[i].play_handle;
			}
		}
	}
	return (HI_HANDLE) NULL;
}
MFUN_EXTERN int MFUN_DECL
avdec_remove_avplay_handle(HI_HANDLE handle) {
	hal_share_init();
	int ret = -1, i = 0, j = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		if (av_play_info[i].play_handle == handle) {
			((hdc_info*) (av_play_info[i].ch_info))->av_play = NULL;
			av_play_info[i].play_handle = 0;
			av_play_info[i].win_handle = (int) NULL;
			av_play_info[i].track_handle = (int) NULL;
			av_play_info[i].ch_info = NULL;
			//TODO
			break;
		}
	}
	return -1;
}

MFUN_EXTERN int MFUN_DECL
avdec_clear_avplay_handle() {
	int i = 0, j = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		if (av_play_info[i].play_handle != 0) {
			av_play_info[i].play_handle == 0;
		}
	}
	return 0;
}

MFUN_EXTERN HI_HANDLE MFUN_DECL
avout_get_avplay_handle_bytk(HI_HANDLE tk_handle) {
	hal_share_init();
	LOGD("avout_get_avplay_handle_bytk > tk_handle = 0x%x", tk_handle);
	int i = 0, j = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		if (av_play_info[i].track_handle == tk_handle) {
			return av_play_info[i].play_handle;
		}
	}
	return -1;
}

MFUN_EXTERN HI_HANDLE MFUN_DECL
avout_get_avplay_handle_bywin(HI_HANDLE win_handle) {
	hal_share_init();
	LOGD("avout_get_avplay_handle_bywin > win_handle = 0x%x", win_handle);
	int i = 0, j = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		LOGD("[avout_get_avplay_handle_bywin] av_play_info[%d].win_handle = 0x%x",i,av_play_info[i].win_handle);
		if (av_play_info[i].win_handle == win_handle) {
			return av_play_info[i].play_handle;
		}
	}
	return -1;
}

MFUN_EXTERN int MFUN_DECL
avout_set_video_window(int out_handle, int index, HI_HANDLE win_handle) {
	hal_share_init();
	int i = 0;
	if (win_handle == (HI_HANDLE) NULL)
		return -1;
	for (i = 0; i < HI_AVOUT_MAX_NUMS; i++) {
		if (av_out[i].out_handle == out_handle) {
			av_out[i].win_handle[index] = win_handle;
			return 0;
		}
	}
	return -1;
}

MFUN_EXTERN int MFUN_DECL
avout_set_video_track(int out_handle, int index, HI_HANDLE tk_handle) {
	hal_share_init();
	int i = 0;
	if (tk_handle == (HI_HANDLE) NULL)
		return -1;
    LOGI("[%s] ... in handle=0x%0x index %d track handle=0x%0x\n", __FUNCTION__, out_handle,index,tk_handle);
    for (i = 0; i < HI_AVOUT_MAX_NUMS; i++)
    {
 	    LOGI("[%s] ... out handle=0x%0x  track handle=0x%0x\n", __FUNCTION__, av_out[i].out_handle,av_out[i].track_handle[0]);
    }	
	
	for (i = 0; i < HI_AVOUT_MAX_NUMS; i++) {
		if (av_out[i].out_handle == out_handle) {
			av_out[i].track_handle[index] = tk_handle;
			return 0;
		}
	}
	return -1;
}

MFUN_EXTERN int MFUN_DECL
avout_remove_video_window(int out_handle, int index, HI_HANDLE sWindow) {
	hal_share_init();
	int i = 0,j = 0;
	for (i = 0; i < HI_AVOUT_MAX_NUMS; i++) {
		if (av_out[i].out_handle == out_handle) {
			for (j = 0; j < HI_AVPLAYER_MAX_NUMS; j++) {
		      if (av_play_info[j].win_handle == av_out[i].win_handle[index]) {
		      	  LOGD("[avout_remove_video_window] av_play_info[j].win_handle = 0x%x",av_play_info[j].win_handle);
			        av_play_info[j].win_handle = NULL;
		      }
	    }
	    av_out[i].win_handle[index] = (int) NULL;
	    break;
		}
	}
	return 0;
}

MFUN_EXTERN int MFUN_DECL
avout_remove_video_track(int out_handle, int index, HI_HANDLE tk_handle) {
	hal_share_init();
	int i = 0;
    LOGI("[%s] ... in handle=0x%0x index %d track handle=0x%0x\n", __FUNCTION__, out_handle,index,tk_handle);
    for (i = 0; i < HI_AVOUT_MAX_NUMS; i++)
    {
 	    LOGI("[%s] ... out handle=0x%0x  track handle=0x%0x\n", __FUNCTION__, av_out[i].out_handle,av_out[i].track_handle[0]);
    }    
	for (i = 0; i < HI_AVOUT_MAX_NUMS; i++) {
		if (av_out[i].out_handle == out_handle) {
			av_out[i].track_handle[index] = (int) NULL;
			return 0;
		}
	}
	return -1;
}

MFUN_EXTERN HI_HANDLE MFUN_DECL
avout_get_video_window(int out_handle, int index) {
	hal_share_init();
	int i = 0;
	for (i = 0; i < HI_AVOUT_MAX_NUMS; i++) {
		if (av_out[i].out_handle == out_handle) {
			return av_out[i].win_handle[index];
		}
	}
	return -1;
}

MFUN_EXTERN HI_HANDLE MFUN_DECL
avout_get_video_track(int out_handle, int index) {
	hal_share_init();
	int i = 0;
    LOGI("[%s] ... in handle=0x%0x index %d \n", __FUNCTION__, out_handle,index);
    for (i = 0; i < HI_AVOUT_MAX_NUMS; i++)
    {
 	    LOGI("[%s] ... out handle=0x%0x  track handle=0x%0x\n", __FUNCTION__, av_out[i].out_handle,av_out[i].track_handle[0]);
    }    
	for (i = 0; i < HI_AVOUT_MAX_NUMS; i++) {
		if (av_out[i].out_handle == out_handle) {
			return av_out[i].track_handle[index];
		}
	}
	return -1;
}

MFUN_EXTERN HI_HANDLE MFUN_DECL
avout_set_visable(int h_avplayer)
{
	hal_share_init();
	LOGI("[%s] ... in handle=0x%0x\n", __FUNCTION__, h_avplayer);
	visable_avhandle=h_avplayer;
	return 0;
}

MFUN_EXTERN HI_HANDLE MFUN_DECL
avout_get_visable()
{
	hal_share_init();
	LOGI("[%s] ... in handle=0x%0x\n", __FUNCTION__, visable_avhandle);
	return visable_avhandle;
}



MFUN_EXTERN HI_HANDLE MFUN_DECL
avdec_get_video_window(HI_HANDLE dec_haldle) {
	hal_share_init();
	LOGD("avdec_get_video_window > dec_haldle = 0x%x", dec_haldle);
	int i = 0, j = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		for (j = 0; j < HAL_PCR_TYPE + 1; j++) {
			if (av_play_info[i].dec[j].dec_handle == dec_haldle) {
				return av_play_info[i].win_handle;
			}
		}
	}
	return -1;
}
MFUN_EXTERN HI_HANDLE MFUN_DECL
avdec_get_video_track(HI_HANDLE dec_haldle) {
	hal_share_init();
	LOGD("avdec_get_video_track > dec_haldle = 0x%x", dec_haldle);
	int i = 0, j = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		for (j = 0; j < HAL_PCR_TYPE + 1; j++) {
			if (av_play_info[i].dec[j].dec_handle == dec_haldle) {
				return av_play_info[i].track_handle;
			}
		}
	}
	return -1;
}
MFUN_EXTERN int MFUN_DECL
avdec_set_avplay_channel_status(HI_HANDLE dec_haldle, int status) {
	hal_share_init();
	int i = 0, j = 0;
	if (status < 0 || status > 3)// 0 1 2 3
		return -1;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		for (j = 0; j < HAL_PCR_TYPE + 1; j++) {
			if (av_play_info[i].dec[j].dec_handle == dec_haldle) {
				av_play_info[i].dec[j].status = status;
				return 0;
			}
		}

	}
	return -1;
}

MFUN_EXTERN int MFUN_DECL
avdec_get_avplay_channel_status(HI_HANDLE dec_haldle) {
	hal_share_init();
	int i = 0, j = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		for (j = 0; j < HAL_PCR_TYPE + 1; j++) {
			if (av_play_info[i].dec[j].dec_handle == dec_haldle) {
				return av_play_info[i].dec[j].status;
			}
		}

	}
	return -1;
}

MFUN_EXTERN int MFUN_DECL
avout_framer_bind_decode(int out_handle, int index, HI_HANDLE dec_haldle) {
	hal_share_init();
	LOGD("avout_framer_bind_decode > out_handle = 0x%x,index = 0x%x,dec_handle = 0x%x", out_handle,
			index, dec_haldle);
	int i = 0, j = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		if (av_play_info[i].dec[HAL_VIDEO_TYPE].dec_handle == dec_haldle) {
			for (j = 0; j < HI_AVOUT_MAX_NUMS; j++) {
				if (av_out[j].out_handle == out_handle) {
					av_play_info[i].win_handle = av_out[j].win_handle[index];
					return 0;
				}
			}
		}
	}
	return -1;
}
MFUN_EXTERN int MFUN_DECL
avout_track_bind_decode(int out_handle, int index, HI_HANDLE dec_haldle) {
	hal_share_init();
	LOGD("avout_framer_bind_decode > out_handle = 0x%x,index = 0x%x,dec_handle = 0x%x", dec_haldle);
	int i = 0, j = 0;
	HI_HANDLE track_handle = -1;
	if (dec_haldle == -1) {
		for (i = 0; i < HI_AVOUT_MAX_NUMS; i++) {
			if (av_out[i].out_handle == out_handle) {
				for (j = 0; j < HI_AVPLAYER_MAX_NUMS; j++) {
					if (av_play_info[j].track_handle == av_out[i].track_handle[index])
						av_play_info[j].track_handle = 0;
				}
			}
		}
	}
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		if (av_play_info[i].dec[HAL_AUDIO_TYPE].dec_handle == dec_haldle) {
			for (j = 0; j < HI_AVOUT_MAX_NUMS; j++) {
				if (av_out[j].out_handle == out_handle) {
					av_play_info[i].track_handle = av_out[j].track_handle[index];
					return 0;
				}
			}
		}
	}
	return -1;
}

MFUN_EXTERN int MFUN_DECL
avdec_decode_bind_demux(HI_HANDLE dec_haldle, int chid) {
	hal_share_init();
	int ret = -1, i = 0, j = 0;
	void* p = NULL;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		for (j = 0; j < HAL_PCR_TYPE + 1; j++) {
			if (av_play_info[i].dec[j].dec_handle == dec_haldle) {
				p = (void*) &av_play_info[i];
				ret = 0;
				break;
			}
		}
		if (ret == 0)
			break;
	}
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		for (j = 0; j < HAL_PCR_TYPE + 1; j++) {
			if (ch_info[i].ch[j].chid == chid) {
				ch_info[i].av_play = p;
				LOGD("avdec_decode_bind_demux > 000 p = %p", p);
				((hav_info*) p)->ch_info = (void*) &ch_info[i];
				LOGD("avdec_decode_bind_demux > 111 (void*) &ch_info[i] = %p", (void*) &ch_info[i]);
				LOGD("avdec_decode_bind_demux > 222 ((hav_info*) p)->ch_info = %p",
						((hav_info*) p)->ch_info);
				return 0;
			}
		}
	}
	return -1;
}
MFUN_EXTERN int MFUN_DECL
avout_set_snd_trackid(int trackid) {
	LOGI("avout_set_snd_trackid trackid = %d", trackid);
	m_trackid = trackid;
	return 0;
}

MFUN_EXTERN int MFUN_DECL
avout_get_snd_trackid() {
	LOGI("avout_get_snd_trackid m_trackid = %d", m_trackid);
	return m_trackid;
}

MFUN_EXTERN int MFUN_DECL
dmx_set_tsbuf_handle(HI_HANDLE h_buf) {
	LOGI("avdec_set_tsbuffer_handle h_buf = %d", h_buf);
	dmx_tsbuf = h_buf;
	return 0;
}

MFUN_EXTERN int MFUN_DECL
dmx_get_tsbuf_handle(void) {
	//LOGI("avdec_get_tsbuffer_handle dmx_ts_buff = %d", dmx_tsbuf);
	return dmx_tsbuf;
}
static long long freq = 0;
MFUN_EXTERN  int MFUN_DECL
dmx_set_freq(long long f){
	freq = f;
	return 0;
}

MFUN_EXTERN  long long  MFUN_DECL
dmx_get_freq(){
	return freq;
}

MFUN_EXTERN  int MFUN_DECL
file_set_count(){
	start_sta++;
	return 0;
}

MFUN_EXTERN  int MFUN_DECL
file_get_count(){
	return start_sta;
}

MFUN_EXTERN  int MFUN_DECL
play_set_freeze(int index,int mode){
	play_freeze[index] = mode;
	return 0;
}

MFUN_EXTERN  int MFUN_DECL
play_get_freeze(int dec_haldle){
	int i = 0, j = 0, k = 0;
	for (i = 0; i < HI_AVPLAYER_MAX_NUMS; i++) {
		if (av_play_info[i].dec[HAL_VIDEO_TYPE].dec_handle == dec_haldle) {
			for (j = 0; j < HI_AVOUT_MAX_NUMS; j++) {
                for (k=0;k<16;k++)
				    if (av_play_info[i].win_handle == av_out[j].win_handle[k]) {
    					return play_freeze[k];;
				    }
			}
		}
	}
	return 0;//
}

static int mce_playing = 0;
MFUN_EXTERN  int MFUN_DECL
mce_setflag(int flag){
	return (mce_playing = flag);
}

MFUN_EXTERN  int MFUN_DECL
mce_getflag(void){
	return mce_playing;
}

