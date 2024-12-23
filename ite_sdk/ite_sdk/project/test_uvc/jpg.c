#include <stdio.h>
#include <malloc.h>
#include "jpg/ite_jpg.h"
#include "ite/ith.h"
#include "jpg.h"

int32_t
jpg_decode(
    uint8_t       *jpegStream,
    uint32_t      streamLength,
    JpegOutputPkt *ptOutputPkt)
{
    JPG_ERR         jRst = JPG_ERR_OK;
    HJPG            *pHJpeg = 0;
    JPG_INIT_PARAM  initParam = { 0 };
    JPG_STREAM_INFO inStreamInfo = { 0 };
    JPG_STREAM_INFO outStreamInfo = { 0 };
    JPG_BUF_INFO    entropyBufInfo = { 0 };
    JPG_USER_INFO   jpgUserInfo = { 0 };
    JPG_RECT        destRect = { 0 };

    uint8_t         *pY = 0, *pU = 0, *pV = 0;
    uint32_t        real_width = 0, real_height = 0, pitch_width = 0;

    // decode
    initParam.codecType     = JPG_CODEC_DEC_JPG_CMD;
    initParam.decType       = JPG_DEC_PRIMARY;

    initParam.outColorSpace = JPG_COLOR_SPACE_YUV420;
    initParam.width         = ithLcdGetWidth();
    initParam.height        = ithLcdGetHeight();
    initParam.dispMode      = JPG_DISP_CENTER;

    //intf("initParam.width= %d ,initParam.height=%d ,ithLcdGetWidth()=%d\n",initParam.width,initParam.height,ithLcdGetWidth());
    iteJpg_CreateHandle(&pHJpeg, &initParam, 0);

    inStreamInfo.streamIOType          = JPG_STREAM_IO_READ;
    inStreamInfo.streamType            = JPG_STREAM_MEM;
    inStreamInfo.jstream.mem[0].pAddr  = jpegStream;
    inStreamInfo.jstream.mem[0].length = streamLength;
    inStreamInfo.validCompCnt          = 1;

    if ((jRst = iteJpg_SetStreamInfo(pHJpeg, &inStreamInfo, 0, 0)))
    {
        printf(" err 0x%x !! %s[%u]\n", jRst, __FUNCTION__, __LINE__);
        goto error_leave;
    }

    //if( (jRst = iteJpg_Parsing(pHJpeg, &entropyBufInfo, 0)) )
    if ((jRst = iteJpg_Parsing(pHJpeg, &entropyBufInfo, (void *)&destRect)))
    {
        printf(" err 0x%x !! %s[%u]\n", jRst, __FUNCTION__, __LINE__);
        goto error_leave;
    }

    iteJpg_GetStatus(pHJpeg, &jpgUserInfo, 0);
    /*
       printf("  disp(%ux%u), dispMode=%d, real(%ux%u), img(%ux%u), slice=%u, pitch(%u, %u)\r\n",
       jpgUserInfo.jpgRect.w, jpgUserInfo.jpgRect.h,
       initParam.dispMode,
       jpgUserInfo.real_width, jpgUserInfo.real_height,
       jpgUserInfo.imgWidth, jpgUserInfo.imgHeight,
       jpgUserInfo.slice_num,
       jpgUserInfo.comp1Pitch, jpgUserInfo.comp23Pitch);
     */
    real_width                          = jpgUserInfo.real_width;
    real_height                         = jpgUserInfo.real_height;
    pitch_width                         = real_width;
    outStreamInfo.streamIOType          = JPG_STREAM_IO_WRITE;
    outStreamInfo.streamType            = JPG_STREAM_MEM;
    // Y
    pY                                  = malloc(pitch_width * real_height * 3);
    //memset(pY, 0x0, real_width * real_height * 3);

    outStreamInfo.jstream.mem[0].pAddr  = (uint8_t *)pY; // get output buf;
    outStreamInfo.jstream.mem[0].pitch  = jpgUserInfo.comp1Pitch;
    outStreamInfo.jstream.mem[0].length = pitch_width * real_height;
    // U
    outStreamInfo.jstream.mem[1].pAddr  = (uint8_t *)(outStreamInfo.jstream.mem[0].pAddr + outStreamInfo.jstream.mem[0].length);
    outStreamInfo.jstream.mem[1].pitch  = jpgUserInfo.comp23Pitch;
    outStreamInfo.jstream.mem[1].length = outStreamInfo.jstream.mem[1].pitch * real_height;
    // V
    outStreamInfo.jstream.mem[2].pAddr  = (uint8_t *)(outStreamInfo.jstream.mem[1].pAddr + outStreamInfo.jstream.mem[1].length);
    outStreamInfo.jstream.mem[2].pitch  = jpgUserInfo.comp23Pitch;
    outStreamInfo.jstream.mem[2].length = outStreamInfo.jstream.mem[2].pitch * real_height;
    outStreamInfo.validCompCnt          = 3;
    /*
       printf("\n\tY=0x%x, u=0x%x, v=0x%x\n",
       outStreamInfo.jstream.mem[0].pAddr,
       outStreamInfo.jstream.mem[1].pAddr,
       outStreamInfo.jstream.mem[2].pAddr);
     */
    if ((jRst = iteJpg_SetStreamInfo(pHJpeg, 0, &outStreamInfo, 0)))
    {
        printf(" err 0x%x !! %s[%u]\n", jRst, __FUNCTION__, __LINE__);
        goto error_free_resource;
    }
    // printf("  LCD A=0x%x, LCD B=0x%x\n", _Get_Lcd_Addr_A(), _Get_Lcd_Addr_B());

    if ((jRst = iteJpg_Setup(pHJpeg, 0)))
    {
        printf(" err 0x%x !! %s[%u]\n", jRst, __FUNCTION__, __LINE__);
        goto error_free_resource;
    }

    if ((jRst = iteJpg_Process(pHJpeg, &entropyBufInfo, 0, 0)))
    {
        printf(" err 0x%x !! %s[%u]\n", jRst, __FUNCTION__, __LINE__);
        goto error_free_resource;
    }

    iteJpg_GetStatus(pHJpeg, &jpgUserInfo, 0);
    //printf("\n\tresult = %d\n", jpgUserInfo.status);

    iteJpg_DestroyHandle(&pHJpeg, 0);

    ptOutputPkt->pYBuffer = outStreamInfo.jstream.mem[0].pAddr;
    ptOutputPkt->pUBuffer = outStreamInfo.jstream.mem[1].pAddr;
    ptOutputPkt->pVBuffer = outStreamInfo.jstream.mem[2].pAddr;
    //ptOutputPkt->ySize    = outStreamInfo.jstream.mem[0].length;
    //ptOutputPkt->uvSize   = outStreamInfo.jstream.mem[1].length;
    ptOutputPkt->yPitch   = pitch_width;
    ptOutputPkt->width    = real_width;
    ptOutputPkt->height   = real_height;
    switch (jpgUserInfo.colorFormate)
    {
    case JPG_COLOR_SPACE_YUV444:
        ptOutputPkt->colorType = DATA_COLOR_YUV444;
        break;
    case JPG_COLOR_SPACE_YUV422:
        ptOutputPkt->colorType = DATA_COLOR_YUV422;
        break;
    case JPG_COLOR_SPACE_YUV420:
        ptOutputPkt->colorType = DATA_COLOR_YUV420;
        break;
    case JPG_COLOR_SPACE_YUV422R:
        ptOutputPkt->colorType = DATA_COLOR_YUV422R;
        break;
    }
    return 0;

error_free_resource:
    if (pY)
    {
        free(pY);
    }
error_leave:
    iteJpg_DestroyHandle(&pHJpeg, 0);
    return -1;
}

void
jpg_encode(
    RawImage *src_img,
    uint8_t  *out_stream,
    uint32_t *out_stream_size)
{
    JPG_ERR         jRst           = JPG_ERR_OK;
    HJPG            *pHJpeg        = 0;
    JPG_INIT_PARAM  initParam      = { 0 };
    JPG_STREAM_INFO inStreamInfo   = { 0 };
    JPG_STREAM_INFO outStreamInfo  = { 0 };
    JPG_BUF_INFO    entropyBufInfo = { 0 };
    JPG_USER_INFO   jpgUserInfo    = { 0 };

    // ------------------------------------------------------
    // encode
    initParam.codecType                 = JPG_CODEC_ENC_JPG;
    initParam.exColorSpace = JPG_COLOR_SPACE_YUV420;
    initParam.outColorSpace             = JPG_COLOR_SPACE_YUV420;
    initParam.width                     = src_img->width;
    initParam.height                    = src_img->height;
    initParam.encQuality                = 70;//85;

    iteJpg_CreateHandle(&pHJpeg, &initParam, 0);
    inStreamInfo.streamIOType           = JPG_STREAM_IO_READ;
    inStreamInfo.streamType             = JPG_STREAM_MEM;
    // Y
    inStreamInfo.jstream.mem[0].pAddr   = (uint8_t *)src_img->pYBuffer;
    inStreamInfo.jstream.mem[0].pitch   = src_img->yPitch;
    // U
    inStreamInfo.jstream.mem[1].pAddr   = (uint8_t *)src_img->pUBuffer;
    inStreamInfo.jstream.mem[1].pitch   = src_img->uvPitch;
    // V
    inStreamInfo.jstream.mem[2].pAddr   = (uint8_t *)src_img->pVBuffer;
    inStreamInfo.jstream.mem[2].pitch   = src_img->uvPitch;
    inStreamInfo.validCompCnt           = 3;

    outStreamInfo.streamIOType          = JPG_STREAM_IO_WRITE;
    outStreamInfo.streamType            = JPG_STREAM_MEM;
    outStreamInfo.jpg_reset_stream_info = 0;
    outStreamInfo.jstream.mem[0].pAddr  = out_stream;
    outStreamInfo.jstream.mem[0].pitch  = src_img->yPitch;
    outStreamInfo.jstream.mem[0].length = *out_stream_size;
    outStreamInfo.validCompCnt          = 1;

    iteJpg_SetStreamInfo(pHJpeg, &inStreamInfo, &outStreamInfo, 0);
    iteJpg_GetStatus(pHJpeg, &jpgUserInfo, 0);
    iteJpg_Setup(pHJpeg, 0);
    iteJpg_Process(pHJpeg, &entropyBufInfo, out_stream_size, 0);
    iteJpg_GetStatus(pHJpeg, &jpgUserInfo, 0);
    iteJpg_DestroyHandle(&pHJpeg, 0);
}