#include "jpg.h"
#include "color_conv.h"
#include "isp/mmp_isp.h"

ISP_DEVICE gIspDev;
int        lcdFlipIndex = 1;

void
color_conv_to_yuv420(
    uint8_t         *srcAddr_rgby,
    uint8_t         *srcAddr_u,
    uint8_t         *srcAddr_v,
    BASE_RECT       *srcRect,
    DATA_COLOR_TYPE src_color_type,
    RawImage        *dstImg)
{
    MMP_ISP_OUTPUT_INFO outInfo = { 0 };
    MMP_ISP_SHARE       ispInput = { 0 };
    int                 result = 0;

    ispInput.width = srcRect->w;
    ispInput.height = srcRect->h;
    ispInput.isAdobe_CMYK = 0;

    switch (src_color_type)
    {
    case DATA_COLOR_YUYV:
        ispInput.addrY = (uint32_t)srcAddr_rgby;
        ispInput.format = MMP_ISP_IN_YUYV;
        ispInput.pitchY = srcRect->w * 2;
        ispInput.pitchUv = srcRect->w * 2;
        break;
    }

    // ---------------------------------------
    // initial isp
    mmpIspTerminate(&gIspDev);
    result = mmpIspInitialize(&gIspDev, MMP_ISP_CORE_0);
    if (result)
        printf("mmpIspInitialize() error (0x%x) !!\n", result);

    mmpIspSetCore(gIspDev, MMP_NULL);

    result = mmpIspSetMode(gIspDev, MMP_ISP_MODE_TRANSFORM);

    if (result)
        printf("mmpIspSetMode() error (0x%x) !! \n", result);

    outInfo.startX = 0;
    outInfo.startY = 0;
    outInfo.width = ispInput.width;
    outInfo.height = ispInput.height;
    outInfo.addrY = (MMP_UINT32)dstImg->pYBuffer;
    outInfo.addrU = (MMP_UINT32)dstImg->pUBuffer;
    outInfo.addrV = (MMP_UINT32)dstImg->pVBuffer;

    outInfo.pitchY = dstImg->yPitch;
    outInfo.pitchUv = dstImg->uvPitch;
    outInfo.format = MMP_ISP_OUT_YUV420;

    mmpIspSetDisplayWindow(gIspDev, 0, 0, outInfo.width, outInfo.height);
    mmpIspSetVideoWindow(gIspDev, 0, 0, outInfo.width, outInfo.height);
    mmpIspSetOutputWindow(gIspDev, &outInfo);
    result = mmpIspPlayImageProcess(gIspDev, &ispInput);

    result = mmpIspWaitEngineIdle(gIspDev);
}

void
_ispColorTrans(
    uint8_t         *srcAddr_rgby,
    uint8_t         *srcAddr_u,
    uint8_t         *srcAddr_v,
    DATA_COLOR_TYPE colorType,
    BASE_RECT       *videoWnd,
    BASE_RECT       *srcRect,
    BASE_RECT       *realRect)
{
    int                 result = 0, OutputWidth = 0, OutputHeight = 0;
    MMP_ISP_OUTPUT_INFO outInfo = { 0 };
    MMP_ISP_SHARE       ispInput = { 0 };

    ispInput.width = realRect->w;
    ispInput.height = realRect->h;
    ispInput.isAdobe_CMYK = 0;

    switch (colorType)
    {
    case DATA_COLOR_YUV444:
    case DATA_COLOR_YUV422:
    case DATA_COLOR_YUV422R:
    case DATA_COLOR_YUV420:
        ispInput.addrY = (uint32_t)srcAddr_rgby;
        ispInput.addrU = (uint32_t)srcAddr_u;
        ispInput.addrV = (uint32_t)srcAddr_v;
        switch (colorType)
        {
        case DATA_COLOR_YUV444:
            ispInput.format = MMP_ISP_IN_YUV444;
            ispInput.pitchY = srcRect->w;
            ispInput.pitchUv = srcRect->w;
            break;

        case DATA_COLOR_YUV422:
            ispInput.format = MMP_ISP_IN_YUV422;
            ispInput.pitchY = srcRect->w;
            ispInput.pitchUv = (srcRect->w >> 1);
            break;

        case DATA_COLOR_YUV422R:
            ispInput.format = MMP_ISP_IN_YUV422R;
            ispInput.pitchY = srcRect->w;
            ispInput.pitchUv = srcRect->w;
            break;

        case DATA_COLOR_YUV420:
            ispInput.format = MMP_ISP_IN_YUV420;
            ispInput.pitchY = srcRect->w;
            ispInput.pitchUv = (srcRect->w >> 1);
            break;
        }
        break;

    case DATA_COLOR_YUYV:
        ispInput.addrY = (uint32_t)srcAddr_rgby;
        ispInput.format = MMP_ISP_IN_YUYV;
        ispInput.pitchY = srcRect->w * 2;
        ispInput.pitchUv = (srcRect->w);
        break;
    }

    // ---------------------------------------
    // initial isp
    mmpIspTerminate(&gIspDev);
    result = mmpIspInitialize(&gIspDev, MMP_ISP_CORE_0);
    if (result)
        printf("mmpIspInitialize() error (0x%x) !!\n", result);

    mmpIspSetCore(gIspDev, MMP_NULL);

    result = mmpIspSetMode(gIspDev, MMP_ISP_MODE_TRANSFORM);

    if (result)
        printf("mmpIspSetMode() error (0x%x) !! \n", result);

    //Benson add
    OutputWidth = ithLcdGetWidth();
    OutputHeight = ithLcdGetHeight();
    mmpIspSetDisplayWindow(gIspDev, 0, 0, OutputWidth, OutputHeight);
    mmpIspSetVideoWindow(gIspDev, videoWnd->x, videoWnd->y, videoWnd->w, videoWnd->h);
    //end of Benson

    // set isp output to LCD Buffer
    outInfo.startX = 0;
    outInfo.startY = 0;
    outInfo.width = OutputWidth;
    outInfo.height = OutputHeight;

    //Double LCD Buffer to prevent display tearing.
    if (lcdFlipIndex)
    {
        outInfo.addrRGB = ithLcdGetBaseAddrA();
        lcdFlipIndex = 0;
    }
    else
    {
        outInfo.addrRGB = ithLcdGetBaseAddrB();
        lcdFlipIndex = 1;
    }

    //outInfo.addrRGB  = ithLcdGetBaseAddrA();
    outInfo.pitchRGB = (uint16_t)ithLcdGetPitch();
    outInfo.format = MMP_ISP_OUT_DITHER565A;
    mmpIspSetOutputWindow(gIspDev, &outInfo);

    //printf("outInfo.width = %d ,outInfo.height=%d ,outInfo.addrRGB=0x%x\n",outInfo.width,outInfo.height ,outInfo.addrRGB);

    // set isp source info and fire
    result = mmpIspPlayImageProcess(gIspDev, &ispInput);
    if (result)
        printf("mmpIspPlayImageProcess() error (0x%x) !!\n", result);

    result = mmpIspWaitEngineIdle(gIspDev); //mmpIspTerminate();
    ithLcdSwFlip(lcdFlipIndex);
    if (result)
        printf("mmpIspTerminate() error (0x%x) !!\n", result);
}