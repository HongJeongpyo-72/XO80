/**
 * @file example.c
 * @author I-Chun Lai (ichun.lai@ite.com.tw)
 * @brief A UVC example which demos how to use [libuvc](https://github.com/ktossell/libuvc)
 *        to get a JPEG/YUYV video frame from uvc camera device.
 * @version 0.1
 * @date 2019-02-18
 * @remark ctrlboard device is needed to run this example.
 *
 * @copyright Copyright (c) 2019 ITE Tech. Inc. All Rights Reserved.
 */
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include "jpg/ite_jpg.h"
#include "isp/mmp_isp.h"
#include "libuvc/libuvc.h"
#include "packet_queue.h"
#include "jpg.h"
#include "color_conv.h"

//=============================================================================
//                  Constant Definition
//=============================================================================
#define TOTAL_TEST_SECONDS 120
#define DISPLAY_WND_X      0
#define DISPLAY_WND_Y      0
#define DISPLAY_WND_WIDTH  ithLcdGetWidth()
#define DISPLAY_WND_HEIGHT ithLcdGetHeight()

//=============================================================================
//                  Macro Definition
//=============================================================================

//=============================================================================
//                  Structure Definition
//=============================================================================

typedef void (*pfPktReleaseCb) (void *pkt);

typedef struct YuyvPkt {
    void     *data;
    size_t   data_bytes;
    uint32_t width;
    uint32_t height;
} YuyvPkt;

//=============================================================================
//                  Global Data Definition
//=============================================================================
PktQueue   gtJpegInputQueue = { 0 };
PktQueue   gtJpegOutputQueue = { 0 };
PktQueue   gtYuyvQueue = { 0 };
int        gbAbortFlag   = 0;
int        gChipId       = 0;
int        gChipRevision = 0;

//=============================================================================
//                  Private Function Definition
//=============================================================================


static void
_jpegInputPktRelease(
    void *pkt)
{
    JpegInputPkt *ptJpegInputPkt = (JpegInputPkt *)pkt;
    if (ptJpegInputPkt && ptJpegInputPkt->pInputBuffer && ptJpegInputPkt->bufferSize)
    {
        free(ptJpegInputPkt->pInputBuffer);
    }
    free(pkt);
}

static void
_jpegOutputPktRelease(
    void *pkt)
{
    JpegOutputPkt *ptJpegOutputPkt = (JpegOutputPkt *)pkt;
    if (ptJpegOutputPkt && ptJpegOutputPkt->pYBuffer)
    {
        free(ptJpegOutputPkt->pYBuffer);
    }
    free(pkt);
}

static void
_yuyvPktRelease(
    void *pkt)
{
    YuyvPkt *ptPkt = (YuyvPkt *)pkt;
    if (ptPkt && ptPkt->data && ptPkt->data_bytes)
    {
        free(ptPkt->data);
    }
    free(pkt);
}

static int
_jpegDecToYuv(
    unsigned char *jpegStream,
    unsigned int  streamLength,
    JpegOutputPkt *ptOutputPkt)
{
    JPG_ERR         jRst = JPG_ERR_OK;
    HJPG            *pHJpeg = 0;
    JPG_INIT_PARAM  initParam = {0};
    JPG_STREAM_INFO inStreamInfo = {0};
    JPG_STREAM_INFO outStreamInfo = {0};
    JPG_BUF_INFO    entropyBufInfo = {0};
    JPG_USER_INFO   jpgUserInfo = {0};
    JPG_RECT        destRect = {0};     //Benson add.

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
    if ((jRst = iteJpg_Parsing(pHJpeg, &entropyBufInfo,  (void *)&destRect)))
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
    real_width  = jpgUserInfo.real_width;
    real_height = jpgUserInfo.real_height;
    if (gChipId == 0x9850 && gChipRevision == 0)
    {
        pitch_width = 2048;
    }
    else
    {
        pitch_width = real_width;
    }
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
    ptOutputPkt->ySize    = outStreamInfo.jstream.mem[0].length;
    ptOutputPkt->uvSize   = outStreamInfo.jstream.mem[1].length;
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

static void *
_jpegThread(
    void *arg)
{
    for (;;)
    {
        JpegInputPkt *ptInputPkt = 0;

        if (gbAbortFlag)
        {
            break;
        }

        if (packetQueueGet(&gtJpegInputQueue, (void **) &ptInputPkt, 0) > 0)
        {
            JpegOutputPkt tOutputPkt = { 0 };
            if (_jpegDecToYuv(ptInputPkt->pInputBuffer, ptInputPkt->bufferSize, &tOutputPkt) >= 0)
            {
                packetQueuePut(&gtJpegOutputQueue, &tOutputPkt);
            }
            _jpegInputPktRelease(ptInputPkt);
        }
        else
        {
            usleep(3 * 1000);
        }
    }
}

static void *
_displayThread(
    void *arg)
{
    BASE_RECT      videoWnd = { 0 };
    int            pktCount = 0;
    struct timeval startT, curT;
    int            timeDiff = 0;
    videoWnd.x = DISPLAY_WND_X;
    videoWnd.y = DISPLAY_WND_Y;
    videoWnd.w = DISPLAY_WND_WIDTH;
    videoWnd.h = DISPLAY_WND_HEIGHT;

    gettimeofday(&startT, NULL);
    for (;;)
    {
        JpegOutputPkt *ptOutputPkt = 0;

        if (gbAbortFlag)
        {
            break;
        }

        if (packetQueueGet(&gtJpegOutputQueue, (void **) &ptOutputPkt, 0) > 0)
        {
            BASE_RECT       srcRect   = {0};
            BASE_RECT       realRect  = {0};
            BASE_RECT       videoRect = {0};
            DATA_COLOR_TYPE colorType = 0;

            pktCount++;

            colorType  = ptOutputPkt->colorType;
            srcRect.w  = ptOutputPkt->yPitch;
            srcRect.h  = ptOutputPkt->height;

            realRect.w = ptOutputPkt->width;
            realRect.h = ptOutputPkt->height;

            _ispColorTrans(
                ptOutputPkt->pYBuffer,
                ptOutputPkt->pUBuffer,
                ptOutputPkt->pVBuffer,
                colorType,
                &videoWnd,
                &srcRect,
                &realRect);
            _jpegOutputPktRelease(ptOutputPkt);
        }
        else
        {
            usleep(3 * 1000);
        }

        gettimeofday(&curT, NULL);
        if ((timeDiff = itpTimevalDiff(&startT, &curT)) >= 2000)
        {
            printf("fps: %.2f\n", ((float)pktCount * 1000 / timeDiff));
            pktCount = 0;
            gettimeofday(&startT, NULL);
        }
    }
}

/* This callback function runs once per frame. Use it to perform any
 * quick processing you need, or have it put the frame into your application's
 * input queue. If this function takes too long, you'll start losing frames. */
static void
_uvcFrameCb(
    uvc_frame_t *frame,
    void        *ptr)
{
    if (frame->data_bytes && frame->data)
    {
        switch (frame->frame_format)
        {
        case UVC_FRAME_FORMAT_MJPEG:
            {
                JpegInputPkt tJpegPkt = { 0 };
                tJpegPkt.pInputBuffer = frame->data;
                tJpegPkt.bufferSize   = frame->data_bytes;
                packetQueuePut(&gtJpegInputQueue, &tJpegPkt);
                break;
            }

        case UVC_FRAME_FORMAT_YUYV:
            {
                int expected_size = frame->width * frame->height * 2;
                // The received data may be wrong, so the amount of data received
                // needed to be checked to confirm whether  the complete video frame
                // have been received.
                if (expected_size == frame->data_bytes)
                {
                    BASE_RECT videoWnd = { 0 };
                    videoWnd.x = DISPLAY_WND_X;
                    videoWnd.y = DISPLAY_WND_Y;
                    videoWnd.w = DISPLAY_WND_WIDTH;
                    videoWnd.h = DISPLAY_WND_HEIGHT;

                    BASE_RECT       srcRect   = { 0 };
                    BASE_RECT       realRect  = { 0 };
                    DATA_COLOR_TYPE colorType = 0;

                    colorType  = DATA_COLOR_YUYV;
                    srcRect.w  = frame->width;
                    srcRect.h  = frame->height;

                    realRect.w = frame->width;
                    realRect.h = frame->height;

                    // When cpu write back mode is enabled, cache must be flushed
                    // before the h/w ISP engine starting to do color conversion.
                    // Otherwise, the h/w ISP engine may read old memory data
                    // and there will be some noise seen on the screen.
                    ithFlushDCacheRange(frame->data, frame->data_bytes);

                    _ispColorTrans(
                        frame->data,
                        frame->data,
                        frame->data,
                        colorType,
                        &videoWnd,
                        &srcRect,
                        &realRect);

                    static bool snapshot = false;
                    static int32_t frame_cnt = 0;
                    if (frame_cnt++ == 10)
                        snapshot = true;
                    if (snapshot)
                    {
                        snapshot = false;
                        // do color conversion
                        uint8_t* p_out_y = (uint8_t*)malloc(frame->width * frame->height * 3 / 2);
                        if (p_out_y != NULL)
                        {
                            RawImage out_img = { 0 };
                            out_img.width = frame->width;
                            out_img.height = frame->height;
                            out_img.pYBuffer = p_out_y;
                            out_img.pUBuffer = p_out_y + out_img.width * out_img.height;
                            out_img.pVBuffer = out_img.pUBuffer + out_img.width * out_img.height / 4;
                            out_img.yPitch = out_img.width;
                            out_img.uvPitch = out_img.width / 2;
                            out_img.colorType = DATA_COLOR_YUV420;
                            color_conv_to_yuv420(
                                frame->data,
                                frame->data,
                                frame->data,
                                &srcRect,
                                colorType,
                                &out_img);
                            if (0)
                            {
                                FILE* f = fopen("A:\\snapshot.yuv", "wb");
                                if (f != NULL)
                                {
                                    fwrite(out_img.pYBuffer, frame->width * frame->height * 3 / 2, 1, f);
                                    fclose(f);
                                }
                            }

                            uint32_t jpg_size = 256 * 1024;
                            uint8_t* p_out_jpg = (uint8_t*)malloc(jpg_size);
                            if (p_out_jpg != NULL)
                            {
                                jpg_encode(
                                    &out_img,
                                    p_out_jpg,
                                    &jpg_size);
                                printf("jpg_size(%u)\n", jpg_size);

                                // This is an example of how to make a snapshot.
                                // It had better to put the part of the file operation
                                // on another thread, because the file operation is time
                                // consuming and can cause the video to get stuck.
                                FILE* f = fopen("A:\\snapshot.jpg", "wb");
                                if (f != NULL)
                                {
                                    fwrite(p_out_jpg, jpg_size, 1, f);
                                    fclose(f);
                                }
                                free(p_out_jpg);
                            }

                            free(p_out_y);
                        }
                    }
                }
                break;
            }
        }
    }
}

//=============================================================================
//                  Public Function Definition
//=============================================================================
int uvc_main()
{
    uvc_context_t       *ctx;
    uvc_device_t        *dev;
    uvc_device_handle_t *devh;
    uvc_stream_ctrl_t   ctrl;
    uvc_error_t         res;
    pthread_t           pJpegDecodeThread, pDisplayThread;

    gChipId       = ithGetDeviceId();
    gChipRevision = ithGetRevisionId();
    printf("Chip: 0x%X, gChipRevision: 0x%X\n", gChipId, gChipRevision);
    /* Initialize a UVC service context. Libuvc will set up its own libusb
     * context. Replace NULL with a libusb_context pointer to run libuvc
     * from an existing libusb context. */
    printf("Init memory status-----------------------\n");
    malloc_stats();
    printf("-----------------------------------------\n");

    res = uvc_init(&ctx);

    if (res < 0)
    {
        uvc_perror(res, "uvc_init");
        return res;
    }

    puts("UVC initialized");

    /* Locates the first attached UVC device, stores in dev */
    res = uvc_find_device(ctx, &dev, 0, 0, NULL); /* filter devices: vendor_id, product_id, "serial_num" */

    if (res < 0)
    {
        uvc_perror(res, "uvc_find_device"); /* no devices found */
    }
    else
    {
        puts("Device found");

        /* Try to open the device: requires exclusive access */
        res = uvc_open(dev, &devh);
        if (res < 0)
        {
            uvc_perror(res, "uvc_open"); /* unable to open device */
        }
        else
        {
            puts("Device opened");
            /* Print out a message containing all the information that libuvc
             * knows about the device */
            uvc_print_diag(devh, stderr);

            const uvc_format_desc_t *fmt_desc = uvc_get_format_descs(devh);
            switch (fmt_desc->bDescriptorSubtype)
            {
            case UVC_VS_FORMAT_UNCOMPRESSED:
                // fmt_desc->guidFormat
                break;

            case UVC_VS_FORMAT_MJPEG:
                break;
            }

            /* Try to negotiate a 640x480 30 fps YUYV stream profile */
            //res = uvc_get_stream_ctrl_format_size(
            //    devh, &ctrl,            /* result stored in ctrl */
            //    UVC_FRAME_FORMAT_MJPEG, /* YUV 422, aka YUV 4:2:2. try _COMPRESSED */
            //    640, 480, 30);          /* width, height, fps */

            res = uvc_get_stream_ctrl_format_size(
                devh, &ctrl,           /* result stored in ctrl */
                UVC_FRAME_FORMAT_YUYV, /* YUV 422, aka YUV 4:2:2. try _COMPRESSED */
                640, 480, 25);         /* width, height, fps */

            /* Print out the result */
            uvc_print_stream_ctrl(&ctrl, stderr);

            if (res < 0)
            {
                uvc_perror(res, "get_mode"); /* device doesn't provide a matching stream */
            }
            else
            {
                //create jpeg decode thread for jpeg decode.
                //packetQueueInit(&gtJpegInputQueue,  _jpegInputPktRelease,  4, sizeof(JpegInputPkt));
                //packetQueueInit(&gtJpegOutputQueue, _jpegOutputPktRelease, 4, sizeof(JpegOutputPkt));
                packetQueueInit(&gtYuyvQueue, _yuyvPktRelease, 4, sizeof(YuyvPkt));
                //pthread_create(&pJpegDecodeThread, NULL, _jpegThread,    0);
                //pthread_create(&pDisplayThread,    NULL, _displayThread, 0);

                /* Start the video stream. The library will call user function cb:
                 *   cb(frame, (void*) 12345)
                 */
                res = uvc_start_streaming(devh, &ctrl, _uvcFrameCb, NULL, 0);

                if (res < 0)
                {
                    uvc_perror(res, "start_streaming"); /* unable to start stream */
                }
                else
                {
                    puts("Streaming...");

                    uvc_set_ae_mode(devh, 1);  /* e.g., turn on auto exposure */

                    sleep(TOTAL_TEST_SECONDS); /* stream for (TOTAL_TEST_SECONDS) seconds */

                    puts("Done streaming.");
                    /* End the stream. Blocks until last callback is serviced */
                    uvc_stop_streaming(devh);
                }
            }
            /* Release our handle on the device */
            uvc_close(devh);
            puts("Device closed");

            gbAbortFlag = 1;
            puts("Wait Jpeg decode thread done");
            //pthread_join(pJpegDecodeThread, NULL);
            puts("Jpeg decode thread is done");

            puts("Wait Display flip thread done");
            //pthread_join(pDisplayThread, NULL);
            puts("Display flip thread is done");

            puts("Wait free queue allocated resource done");
            packetQueueEnd(&gtJpegInputQueue);
            packetQueueEnd(&gtJpegOutputQueue);
            packetQueueEnd(&gtYuyvQueue);
            puts("Free queue allocated resource is done");
        }
    }

    /* Close the UVC context. This closes and cleans up any existing device handles,
     * and it closes the libusb context if one was not provided. */
    uvc_exit(ctx);
    puts("UVC exited");

    printf("Leave memory status-----------------------\n");
    malloc_stats();
    printf("-----------------------------------------\n");

    return 0;
}