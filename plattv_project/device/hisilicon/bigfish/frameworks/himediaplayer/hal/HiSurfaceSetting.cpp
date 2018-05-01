#define LOG_NDEBUG 0
#define LOG_TAG "HisiSurfaceSetting"

#include <cutils/properties.h>
#if defined (PRODUCT_STB)
#include "DisplayClient.h"
#endif
#include "hidisplay.h"
#include "HiSurfaceSetting.h"
#include "HiDrvAdpter.h"

namespace android
{
    // surfaceSetting Class Impl

    surfaceSetting::surfaceSetting( HI_HANDLE window, HI_HANDLE AvPlayer)
        : mWindow(window),
          mAvPlayer(AvPlayer)
//    surfaceSetting::surfaceSetting(HI_HANDLE window, HI_HANDLE AvPlayer)
    {
    		 LOGV("gongcm surfaceSetting::surfaceSetting \n");
        init();
    }

    void surfaceSetting::init()
    {
        LOGV("HiMediaPlayer::surfaceSetting::init");
#if defined (PRODUCT_STB)
        DisplayClient dispClient;
#endif
        char buffer_orientation[PROP_VALUE_MAX];
        memset(buffer_orientation, 0, sizeof(buffer_orientation));
        property_get("persist.sys.screenorientation", buffer_orientation, "landscape");
        int cmpRet = strcmp("portrait", buffer_orientation);
        if (cmpRet == 0)
        {
            screen_orientation = 1;
        }
        else
        {
            screen_orientation = 0;
        }

        mSurfaceX = 0;
        mSurfaceY = 0;
#if defined (PRODUCT_DPT)
        mSurfaceWidth = HIMEDIA_DEFAULT_OUTPUT_W;
        mSurfaceHeight = HIMEDIA_DEFAULT_OUTPUT_H;
        mCvrs = VOUT_AR_CONVERSION_CUSTOM;
#else
        char buffer[PROP_VALUE_MAX];
        int resolution = 720;
        memset(buffer, 0, PROP_VALUE_MAX);
        property_get("persist.sys.resolution", buffer, "720");
        resolution = atoi(buffer);
        LOGV("surfaceSetting::init, resolution = %d", resolution);
        if (resolution == 1080)
        {
            mSurfaceWidth = 1920;
            mSurfaceHeight = 1080;
        }
        else
        {
            mSurfaceWidth = 1280;
            mSurfaceHeight = 720;
        }
        mCvrs = dispClient.GetAspectCvrs(); //HIMEDIAPLAYER_CURVE_DEFAUL;
#endif
        mRatio.width = -1;
        mRatio.height = -1;

        LOGV("init mCvrs is %d", mCvrs);

        setVideoWindowAttr(mSurfaceX, mSurfaceY, mSurfaceWidth, mSurfaceHeight, mRatio, mCvrs);
    }

    status_t surfaceSetting::updateSurfacePosition()
    {
        return setVideoWindowAttr(mSurfaceX, mSurfaceY, mSurfaceWidth, mSurfaceHeight, mRatio, mCvrs);
    }

    int surfaceSetting::setWindowMute(HI_BOOL bWindowMute)
    {
        HI_S32 s32Ret = HI_SUCCESS;
        s32Ret = HiDrvAdpter::VOSetAttr(mWindow, VO_ATTR_ID_MUTE, &bWindowMute);
        return s32Ret;
    }

    status_t surfaceSetting::setVideoWindowAttr(int x, int y,
            int w, int h, HI_ASPECT_RATIO_S ratio, int cvrs)
    {
        HI_S32 s32Ret = NO_ERROR;
        HI_CHAR value[PROPERTY_VALUE_MAX] = {0};
        HI_BOOL bUpdateRect = HI_TRUE;

        if (property_get("service.media.hiplayer.output", value, "sideband") && !strcasecmp("sideband", value))
        {
            bUpdateRect = HI_FALSE;
        }
        if (screen_orientation == 0)
        {
            // landscape
            mSurfaceX      = x;
            mSurfaceY      = y;
            mSurfaceWidth  = w;
            mSurfaceHeight = h;
        }
        else if (x != mSurfaceX || y != mSurfaceY || w != mSurfaceWidth || h != mSurfaceHeight)
        {
            // portrait
            mSurfaceX      = y;
            mSurfaceY      = x;
            mSurfaceWidth  = h;
            mSurfaceHeight = w;
        }

        mRatio.width = ratio.width;
        mRatio.height = ratio.height;

        if (HI_SVR_PLAYER_INVALID_HDL == mWindow)
        {
            LOGV("setVideoWindowAttr mWindow error");
            return UNKNOWN_ERROR;
        }

        if (screen_orientation == 1 &&  HI_SVR_PLAYER_INVALID_HDL != mAvPlayer)
        {
            AV_VID_ROTATION_E eRotation = AV_VID_ROTATION_90;
            HiDrvAdpter::AVSetAttr(mAvPlayer, AV_ATTR_ID_ROTATION, &eRotation);
        }

        VO_ASPECT_S stAspect;
        mCvrs = cvrs;
        stAspect.u32AspectRation = cvrs;
        stAspect.u32UserAspectW = mRatio.height;
        stAspect.u32UserAspectH = mRatio.width;

        if (-1 != mRatio.width && -1 != mRatio.height)
        {
            stAspect.bUserDefAspect = HI_TRUE;
        }
        else
        {
            stAspect.bUserDefAspect = HI_FALSE;
        }

        s32Ret = HiDrvAdpter::VOSetAttr(mWindow, VO_ATTR_ID_ASPECT_CVRS, &stAspect);

        VO_RECT_S stRect = {.x = (AV_COORD_T)mSurfaceX, .y = (AV_COORD_T)mSurfaceY, .w = (AV_COORD_T)mSurfaceWidth, .h = (AV_COORD_T)mSurfaceHeight};
        if (bUpdateRect)
        {
            s32Ret |= HiDrvAdpter::VOSetAttr(mWindow, VO_ATTR_ID_OUTPUT_RECT, &stRect);
        }
        LOGV("Set Window Attr x[%d] y[%d] w[%d] h[%d] mCvrs[%d] ratio [w %d] [h %d] bUserDefAspectRatio is %d",
             stRect.x, stRect.y, stRect.w, stRect.h, mCvrs, stAspect.u32UserAspectW, stAspect.u32UserAspectH, stAspect.bUserDefAspect);

        return ((HI_SUCCESS == s32Ret) ? NO_ERROR : UNKNOWN_ERROR);
    }

    status_t surfaceSetting::updateSurfacePosition(int x, int y, int w, int h)
    {
        if ( w < 0 || h < 0 || x < 0 || y < 0)
        { return UNKNOWN_ERROR; }

        if (x == mSurfaceX && y == mSurfaceY && w == mSurfaceWidth && h == mSurfaceHeight)
        { return NO_ERROR; }

        return setVideoWindowAttr(x, y, w, h, mRatio, mCvrs);
    }

    status_t surfaceSetting::setVideoRatio(HI_ASPECT_RATIO_S Ratio)
    {
        if (Ratio.width == mRatio.width && Ratio.height == mRatio.height)
        {
            LOGV("Ratio setting w %d h %d is same with current setting", Ratio.width, Ratio.height);
            return NO_ERROR;
        }

        if (Ratio.width < 0 || Ratio.width > HIMEDIA_ASPECTRATIO_WIDTH_MAX || Ratio.height < 0
            || Ratio.height > HIMEDIA_ASPECTRATIO_HEIGHT_MAX)
        {
            LOGV("Ratio setting is illegal,w %d h %d,width range is 0--%d height range is 0--%d", Ratio.width, Ratio.height, HIMEDIA_ASPECTRATIO_WIDTH_MAX,
                 HIMEDIA_ASPECTRATIO_HEIGHT_MAX);
        }
        return setVideoWindowAttr(mSurfaceX, mSurfaceY, mSurfaceWidth,
                                  mSurfaceHeight, Ratio, mCvrs);
    }

    status_t surfaceSetting::setVideoCvrs(int cvrs)
    {
        if (cvrs == mCvrs || cvrs < 0)
        { return NO_ERROR; }

        mCvrs = cvrs;

        if (HI_SVR_PLAYER_INVALID_HDL == mWindow)
        {
            return NO_ERROR;
        }

        return setVideoWindowAttr(mSurfaceX, mSurfaceY, mSurfaceWidth,
                                  mSurfaceHeight, mRatio, cvrs);
    }

    status_t surfaceSetting::setVideoZOrder(int ZOrder)
    {
        HI_S32 s32Ret = HI_FAILURE;
        LOGV("Call surfaceSetting::setVideoZOrder IN");

        if (HI_SVR_PLAYER_INVALID_HDL == mWindow)
        {
            LOGE("setVideoZOrder window is null");
            return UNKNOWN_ERROR;
        }

        if (ZOrder < HI_MEDIAPLAYER_ZORDER_MOVETOP || ZOrder > HI_MEDIAPLAYER_ZORDER_MOVEDOWN)
        {
            LOGE("setVideoZOrder,Invalid args, the input arg is %d", ZOrder);
            return UNKNOWN_ERROR;
        }
        else
        {
            LOGI("setVideoZOrder, the input arg is %d", ZOrder);
        }

        s32Ret = HiDrvAdpter::VOSetAttr(mWindow, VO_ATTR_ID_ZORDER, &ZOrder);
        return ((HI_SUCCESS == s32Ret) ? NO_ERROR : UNKNOWN_ERROR);
    }

    status_t  surfaceSetting::setWindowFreezeStatus(int flag)
    {
        HI_S32 s32Ret = HI_FAILURE;
        LOGV("Call surfaceSetting::setWindowFreezeStatus IN");

        if (HI_SVR_PLAYER_INVALID_HDL == mWindow)
        {
            LOGE("setWindowFreeze window is null");
            return UNKNOWN_ERROR;
        }

        HI_BOOL bFlag = (0 == flag) ? HI_FALSE : HI_TRUE;

        LOGV("Call surfaceSetting::setWindowFreezeStatus flag = %d, bFlag = %d\n", flag, bFlag);
        s32Ret = HiDrvAdpter::VOSetAttr(mWindow, VO_ATTR_ID_FREEZE_WINDOW, &bFlag);

        return ((HI_SUCCESS == s32Ret) ? NO_ERROR : UNKNOWN_ERROR);
    }

} // end namespace android
