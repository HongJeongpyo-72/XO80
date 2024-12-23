/*
 * Copyright (c) 2015 ITE technology Corp. All Rights Reserved.
 */
/** @file itv.c
 * Used to do H/W video overlay
 *
 * @author I-Chun Lai
 * @version 0.1
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include "ite/ith.h"
#include "ite/itp.h"
#include "ite/itv.h"
#include "isp/mmp_isp.h"
#include "fc_sync.h"
#include "ith/ith_video.h"
#include "capture/capture_970/mmp_capture.h"
#if (CFG_CHIP_FAMILY != 970 && CFG_CHIP_FAMILY != 960)
    #include "ite/ite_m2d.h"
    #include "m2d/m2d_graphics.h"
#endif

//=============================================================================
//                              Constant Definition
//=============================================================================
#if (defined(WIN32) && defined(_DEBUG))
    #define DEBUG_FLIP
#endif

//#define FRAME_FUNCTION_ENABLE

#define ITV_FF_NSET             2
#define ITV_FF_NBUF             3

#define MAX_ROTATE_BUFFER_COUNT 2

#define ITV_MAX_VSBUF           3

#ifdef CFG_ITV_VP_HIGH_QUALITY
#define timer_ff_x              128
#define timer_ff_y              656
#define timer_ff_w              1024//1088//1024//1280
#define timer_ff_h              64

#define thumbnail_w             168
#define thumbnail_h             100
#endif

//=============================================================================
//                              Macro Definition
//=============================================================================
#define WAIT_UNTIL(expr)          while (!(expr)) { usleep(20000); }
#define MIDDLE_WAIT_UNTIL(expr)   while (!(expr)) { usleep(3000);  }
#define SHORT_WAIT_UNTIL(expr)    while (!(expr)) { usleep(1000);  }

#define QUEUE_IS_FULL(w, r, size) ((((w) + 1) % (size)) == (r))

#define IS_VERTICAL_DISPLAY()     ((gRotation) == ITU_ROT_90 || (gRotation) == ITU_ROT_270)
#define IS_ROTATE_DISPLAY()       ((gRotation) != ITU_ROT_0)

//=============================================================================
//                              Structure Definition
//=============================================================================
typedef struct
{
    uint8_t         *uibuf[ITV_FF_NBUF];
    ITV_UI_PROPERTY uiprop[ITV_FF_NBUF];
    volatile int    ui_ridx;
    volatile int    ui_widx;
} ITV_STRC_FF;

typedef struct
{
    /* DECODE VIDEO BUFFER */
    bool              bVideo;
    bool              video_update;
    uint8_t           *dbuf[ITV_MAX_NDBUF];             // decoded video buffer address
    ITV_DBUF_PROPERTY dbufprop[ITV_MAX_NDBUF];          // property of the decoded video buffer
    ITV_DBUF_PROPERTY blank_yuv_frame;
    ITV_DBUF_PROPERTY *curr_video;
    volatile int      disp_buf_ridx;
    volatile int      disp_buf_widx;

    /* VIDEO SURFACE INDEX */
    bool              check_video_surf;
    bool              check_capvideo_surf;
    volatile int      vid_surf_cur_idx;
    volatile int      vid_surf_ridx;
    volatile int      vid_surf_widx;

    /* PLAYER STC SYNC */
    //FC_STRC_SYNC      itv_sync;

    /* FLUSH COMMAND */
    volatile int      pcmd_flush_dbuf;
    volatile int      pcmd_flush_uibuf[ITV_FF_NSET];    // frame function
#ifdef CFG_ITV_VP_HIGH_QUALITY
    volatile int      pcmd_flush_osdbuf[ITV_FF_NSET];
#endif

    /* MTAL */
    int               mtal_pb_mode_isr;                 // indicate whether the player currently is in playback mode or not.
    volatile int      mtal_pb_mode_u;

    /* FRAME FUNCTION */
    bool              ui_update;
    int               ff_mode_isr[ITV_FF_NSET];
    volatile int      ff_mode_u[ITV_FF_NSET];
    ITV_STRC_FF       ff[ITV_FF_NSET];
    volatile int      ff_setbase;
    volatile int      ff_setbase_id;
    volatile int      ff_setbase_bid;
    volatile uint8_t  *ff_setbase_base;
#ifdef CFG_ITV_VP_HIGH_QUALITY
    bool              osd_update;
    int               osd_mode_isr[ITV_FF_NSET];
    volatile int      osd_mode_u[ITV_FF_NSET];
    ITV_STRC_FF       osd[ITV_FF_NSET];
    volatile int      osd_setbase;
    volatile int      osd_setbase_id;
    volatile int      osd_setbase_bid;
    volatile uint8_t  *osd_setbase_base;
    bool              osd_updateFFREG[ITV_FF_NSET];
#endif
} ITV_STRC_RESOURCE;

typedef struct LCD_FLIP_TAG
{
    sem_t     tScanlineEvent;
    uint32_t  bInited;
    pthread_t hw_overlay_tid;
    pthread_t lcd_isr_tid;
} LCD_FLIP;

//=============================================================================
//                              Extern Reference
//=============================================================================

//=============================================================================
//                              Global Data Definition
//=============================================================================
static ITV_STRC_RESOURCE itv_rcs;    // a blank 16 x 16 frame will be defalut input
static ITV_DBUF_PROPERTY gCapPROP;
static ISP_DEVICE        gIspDev;
static LCD_FLIP          gtFlipCtrl;
static ITURotation       gRotation = ITU_ROT_0;
#ifdef CFG_ITV_VP_HIGH_QUALITY
static uint8_t           *g_thumbnail_buf = NULL;
QueueHandle_t            gITVENCQueue = NULL;
static QueueHandle_t     gISPCMDQMSGQueue = NULL;
#endif
static ITV_DISPLAY_WND   vid_window;

static bool              g_stop_itv_hw_overlay_task;
static uint32_t          g_blank_buf;
sem_t                    gtISPEvent;
static int               g_lcd_index;
static int               g_lcd_index_counter;

#if (CFG_CHIP_FAMILY != 970 && CFG_CHIP_FAMILY != 960)
static uint32_t          gRotateBuffer[MAX_ROTATE_BUFFER_COUNT];
static MMP_M2D_SURFACE   gRoateSurf[MAX_ROTATE_BUFFER_COUNT];
static MMP_M2D_SURFACE   *_gRoateSurf;
static MMP_M2D_SURFACE   _gLcdSurf[ITV_MAX_VSBUF];
#endif

#ifdef CFG_ITV_VP_HIGH_QUALITY
static pthread_mutex_t OSD_PREVIEW_MUTEX = PTHREAD_MUTEX_INITIALIZER;
static ITV_ISP_CMDQ_INDEX gITVCMDQ_ENCIndexInput = ITV_2D_INDEX_2;
static ITV_ISP_CMDQ_INDEX gITVCMDQ_ENCIndex = ITV_2D_INDEX_2;
static ITV_ISP_CMDQ_INDEX gITVCMDQ_JPEGIndexInput = ITV_2D_INDEX_2;
static ITV_ISP_CMDQ_INDEX gITVCMDQ_JPEGIndex = ITV_2D_INDEX_2;
static ITV_ISP_CMDQ_INDEX gITVCMDQ_2DIndexInput = ITV_ENC_OSD_INDEX_2;
static ITV_ISP_CMDQ_INDEX gITVCMDQ_2DIndex = ITV_ENC_OSD_INDEX_2;
#endif

//=============================================================================
//                              Private Function Declaration
//=============================================================================
static void itv_lcdISR(void *arg);
static void itv_isp0ISR(void *arg);
static void itv_isp1ISR(void *arg);
static void itv_change_display_format();
int itv_check_vidSurf_anchor(void);

static int next_lcd_index()
{
    g_lcd_index_counter = (g_lcd_index_counter + 1) % ITV_MAX_DISP_BUF;

    return g_lcd_index_counter;
}

static void update_ui()
{
    int         i;
    ITV_STRC_FF *ff;
    int         widx, ridx, ridx_last;

    for (i = 0; i < ITV_FF_NSET; i++)
    {
        ff   = &(itv_rcs.ff[i]);
        widx = ff->ui_widx;
        ridx = ff->ui_ridx;

        if (widx != ridx)
        {
            if (itv_rcs.ff_mode_isr[i] == 1)
            {
                if (widx) ridx_last = widx - 1;
                else ridx_last = ITV_FF_NBUF - 1;

                if (itv_check_vidSurf_anchor() == 1)
                    itv_rcs.ui_update = true;

                switch (i)
                {
                case 0:
                    mmpIspEnable(gIspDev, MMP_ISP_FRAME_FUNCTION_0);
                    mmpIspSetFrameFunction(
                            gIspDev,
                            MMP_ISP_FRAME_FUNCTION_0,
                            ff->uibuf[ridx_last],
                            ff->uiprop[ridx_last].startX,
                            ff->uiprop[ridx_last].startY,
                            ff->uiprop[ridx_last].width,
                            ff->uiprop[ridx_last].height,
                            ff->uiprop[ridx_last].pitch,
                            ff->uiprop[ridx_last].colorKeyR,
                            ff->uiprop[ridx_last].colorKeyG,
                            ff->uiprop[ridx_last].colorKeyB,
                            ff->uiprop[ridx_last].EnAlphaBlend,
                            ff->uiprop[ridx_last].constAlpha,
                            MMP_PIXEL_FORMAT_RGB565,
                            ridx_last);
                    break;

                case 1:
                    mmpIspEnable(gIspDev, MMP_ISP_FRAME_FUNCTION_1);
                    mmpIspSetFrameFunction(
                            gIspDev,
                            MMP_ISP_FRAME_FUNCTION_1,
                            ff->uibuf[ridx_last],
                            ff->uiprop[ridx_last].startX,
                            ff->uiprop[ridx_last].startY,
                            ff->uiprop[ridx_last].width,
                            ff->uiprop[ridx_last].height,
                            ff->uiprop[ridx_last].pitch,
                            ff->uiprop[ridx_last].colorKeyR,
                            ff->uiprop[ridx_last].colorKeyG,
                            ff->uiprop[ridx_last].colorKeyB,
                            ff->uiprop[ridx_last].EnAlphaBlend,
                            ff->uiprop[ridx_last].constAlpha,
                            MMP_PIXEL_FORMAT_RGB565,
                            ridx_last);
                    break;

                default:
                    break;
                }
            }
            else
            {
                switch (i)
                {
                case 0:
                    mmpIspDisable(gIspDev, MMP_ISP_FRAME_FUNCTION_0);
                    break;

                case 1:
                    mmpIspDisable(gIspDev, MMP_ISP_FRAME_FUNCTION_1);
                    break;

                default:
                    break;
                }
            }
        }
        else
        {
            itv_rcs.pcmd_flush_uibuf[i] = 0;

            if (itv_rcs.ff_mode_isr[i] == 0)
            {
                switch (i)
                {
                case 0:
                    mmpIspDisable(gIspDev, MMP_ISP_FRAME_FUNCTION_0);
                    break;

                case 1:
                    mmpIspDisable(gIspDev, MMP_ISP_FRAME_FUNCTION_1);
                    break;

                default:
                    break;
                }
            }
        }
    }
}

#ifdef CFG_ITV_VP_HIGH_QUALITY
static void update_osd()
{
    int         i;
    ITV_STRC_FF *ff;
    int         widx, ridx;

    for (i = 0; i < ITV_FF_NSET; i++)
    {
        ff   = &(itv_rcs.osd[i]);
        widx = ff->ui_widx;
        ridx = ff->ui_ridx;

        if (widx != ridx)
        {
            if (itv_rcs.osd_mode_isr[i] == 1)
            {
                itv_rcs.osd_update = true;

                switch (i)
                {
                case 0:
                    itv_rcs.osd_updateFFREG[i] = true;
                    break;

                case 1:
                    itv_rcs.osd_updateFFREG[i] = true;
                    break;

                default:
                    break;
                }
            }
            else
            {
                switch (i)
                {
                case 0:
                    itv_rcs.osd_updateFFREG[i] = false;
                    break;

                case 1:
                    itv_rcs.osd_updateFFREG[i] = false;
                    break;

                default:
                    break;
                }
            }
        }
        else
        {
            itv_rcs.pcmd_flush_osdbuf[i] = 0;

            if (itv_rcs.osd_mode_isr[i] == 0)
            {
                switch (i)
                {
                case 0:
                    itv_rcs.osd_updateFFREG[i] = false;
                    break;

                case 1:
                    itv_rcs.osd_updateFFREG[i] = false;
                    break;

                default:
                    break;
                }
            }
        }
    }
}

static void flip_jpec_osd(ITV_DBUF_PROPERTY *encDispProp)
{
    MMP_ISP_OUTPUT_INFO out_info       = {0};
    MMP_ISP_SHARE       isp_share      = {0};

    isp_share.addrY   = encDispProp->ya;
    isp_share.addrU   = encDispProp->ua;
    isp_share.width   = encDispProp->src_w;
    isp_share.height  = encDispProp->src_h;
    isp_share.pitchY  = encDispProp->pitch_y;
    isp_share.pitchUv = encDispProp->pitch_uv;
    isp_share.format  = encDispProp->format;
    out_info.width    = thumbnail_w;
    out_info.height   = thumbnail_h;
    out_info.pitchY   = thumbnail_w;
    out_info.pitchUv  = thumbnail_w;
    out_info.format   = MMP_ISP_OUT_NV12;
    out_info.addrY    = g_thumbnail_buf;
    out_info.addrU    = g_thumbnail_buf + thumbnail_w * thumbnail_h;

    mmpIspDisableLCDOnFlyMode(gIspDev);
    mmpIspSetMode(gIspDev, MMP_ISP_MODE_PLAY_VIDEO);
    mmpIspSetOutputWindow(gIspDev, &out_info);
    mmpIspSetVideoWindow(gIspDev, 0, 0, out_info.width, out_info.height);
    mmpIspDisable(gIspDev, MMP_ISP_FRAME_FUNCTION_0);
    mmpIspDisable(gIspDev, MMP_ISP_FRAME_FUNCTION_1);

    if (xQueueSend(gISPCMDQMSGQueue, &gITVCMDQ_JPEGIndexInput, 0) != pdPASS)
    {
        printf("[ITV] ISP CMDQ MSG is overflowing\n");
    }
    mmpIspPlayImageProcess(gIspDev, &isp_share);
    SHORT_WAIT_UNTIL(gITVCMDQ_JPEGIndexInput == gITVCMDQ_JPEGIndex);
}

static void flip_enc_osd(ITV_DBUF_PROPERTY *encDispProp)
{
    MMP_ISP_OUTPUT_INFO out_info       = {0};
    MMP_ISP_SHARE       isp_share      = {0};
    int                 i              = 0;
    ITV_STRC_FF         *ff            = NULL;
    int                 widx           = 0;
    int                 ridx_last      = 0;

    flip_jpec_osd(encDispProp);

    isp_share.addrY   = encDispProp->ya + timer_ff_x + timer_ff_y * encDispProp->pitch_y;
    isp_share.addrU   = encDispProp->ua + timer_ff_x + timer_ff_y * encDispProp->pitch_uv / 2;
    isp_share.width   = timer_ff_w;
    isp_share.height  = timer_ff_h;
    isp_share.pitchY  = encDispProp->pitch_y;
    isp_share.pitchUv = encDispProp->pitch_uv;
    isp_share.format  = encDispProp->format;
    out_info.width    = timer_ff_w;
    out_info.height   = timer_ff_h;
    out_info.pitchY   = encDispProp->pitch_y;
    out_info.pitchUv  = encDispProp->pitch_uv;
    out_info.format   = MMP_ISP_OUT_NV12;
    out_info.addrY    = encDispProp->ya + timer_ff_x + timer_ff_y * encDispProp->pitch_y;
    out_info.addrU    = encDispProp->ua + timer_ff_x + timer_ff_y * encDispProp->pitch_uv / 2;

    mmpIspDisableLCDOnFlyMode(gIspDev);
    mmpIspSetMode(gIspDev, MMP_ISP_MODE_PLAY_VIDEO_BYPASS);
    mmpIspSetOutputWindow(gIspDev, &out_info);
    mmpIspSetVideoWindow(gIspDev, 0, 0, out_info.width, out_info.height);

    for (i = 0; i < ITV_FF_NSET; i++)
    {
        ff   = &(itv_rcs.osd[i]);
        widx = ff->ui_widx;

        if (widx) ridx_last = widx - 1;
        else ridx_last = ITV_FF_NBUF - 1;

        switch (i)
        {
        case 0:
            if (itv_rcs.osd_updateFFREG[i])
            {
                mmpIspEnable(gIspDev, MMP_ISP_FRAME_FUNCTION_0);
                mmpIspSetFrameFunction(
                        gIspDev,
                        MMP_ISP_FRAME_FUNCTION_0,
                        ff->uibuf[ridx_last],
                        ff->uiprop[ridx_last].startX,
                        ff->uiprop[ridx_last].startY,
                        ff->uiprop[ridx_last].width,
                        ff->uiprop[ridx_last].height,
                        ff->uiprop[ridx_last].pitch,
                        ff->uiprop[ridx_last].colorKeyR,
                        ff->uiprop[ridx_last].colorKeyG,
                        ff->uiprop[ridx_last].colorKeyB,
                        ff->uiprop[ridx_last].EnAlphaBlend,
                        ff->uiprop[ridx_last].constAlpha,
                        MMP_PIXEL_FORMAT_ARGB8888,
                        ridx_last);
            }
            else
                mmpIspDisable(gIspDev, MMP_ISP_FRAME_FUNCTION_0);
            break;

        case 1:
            if (itv_rcs.osd_updateFFREG[i])
            {
                mmpIspEnable(gIspDev, MMP_ISP_FRAME_FUNCTION_1);
                mmpIspSetFrameFunction(
                        gIspDev,
                        MMP_ISP_FRAME_FUNCTION_1,
                        ff->uibuf[ridx_last],
                        ff->uiprop[ridx_last].startX,
                        ff->uiprop[ridx_last].startY,
                        ff->uiprop[ridx_last].width,
                        ff->uiprop[ridx_last].height,
                        ff->uiprop[ridx_last].pitch,
                        ff->uiprop[ridx_last].colorKeyR,
                        ff->uiprop[ridx_last].colorKeyG,
                        ff->uiprop[ridx_last].colorKeyB,
                        ff->uiprop[ridx_last].EnAlphaBlend,
                        ff->uiprop[ridx_last].constAlpha,
                        MMP_PIXEL_FORMAT_ARGB8888,
                        ridx_last);
            }
            else
                mmpIspDisable(gIspDev, MMP_ISP_FRAME_FUNCTION_1);

        default:
            break;
        }
    }

    gITVCMDQ_ENCIndexInput = encDispProp->bidx;
    if (xQueueSend(gISPCMDQMSGQueue, &encDispProp->bidx, 0) != pdPASS)
    {
        printf("[ITV] ISP CMDQ MSG is overflowing\n");
    }
    mmpIspPlayImageProcess(gIspDev, &isp_share);
    SHORT_WAIT_UNTIL(gITVCMDQ_ENCIndexInput == gITVCMDQ_ENCIndex);
}
#endif

static void flip_lcd(
    int lcd_index)
{
    MMP_ISP_OUTPUT_INFO out_info  = {0};
    MMP_ISP_SHARE       isp_share = {0};
    ITURotation         rot;
#ifdef CFG_ITV_VP_HIGH_QUALITY
    ITV_ISP_CMDQ_INDEX  CMDQ_Queue_index = ITV_ENC_OSD_INDEX_0;
#endif

#ifdef FRAME_FUNCTION_ENABLE
    if (itv_rcs.curr_video != &itv_rcs.blank_yuv_frame)
    {
        if ((NULL == itv_rcs.curr_video) ||
                (NULL == itv_rcs.curr_video->ya) ||
                (NULL == itv_rcs.curr_video->ua) ||
                (NULL == itv_rcs.curr_video->va) ||
                (0 == itv_rcs.curr_video->src_w) ||
                (0 == itv_rcs.curr_video->src_h) ||
                (0 == itv_rcs.curr_video->pitch_y) ||
                (0 == itv_rcs.curr_video->pitch_uv))
        {
            itv_rcs.curr_video = &itv_rcs.blank_yuv_frame;
            break;
        }

        if (itv_rcs.curr_video->format == 0)
            itv_rcs.curr_video->format = MMP_ISP_IN_YUV420;
    }
#endif

    isp_share.addrY   = (uint32_t)itv_rcs.curr_video->ya;
    isp_share.addrU   = (uint32_t)itv_rcs.curr_video->ua;
    isp_share.addrV   = (uint32_t)itv_rcs.curr_video->va;
    isp_share.width   = itv_rcs.curr_video->src_w;
    isp_share.height  = itv_rcs.curr_video->src_h;
    isp_share.pitchY  = itv_rcs.curr_video->pitch_y;
    isp_share.pitchUv = itv_rcs.curr_video->pitch_uv;
    isp_share.format  = itv_rcs.curr_video->format;

#if (CFG_CHIP_FAMILY != 970 && CFG_CHIP_FAMILY != 960)
    if (IS_ROTATE_DISPLAY())
    {
        _gRoateSurf      = (_gRoateSurf == &gRoateSurf[0]) ? &gRoateSurf[1] : &gRoateSurf[0];
        out_info.addrRGB = (MMP_UINT32)((M2D_SURFACE *)(*_gRoateSurf))->baseScanPtr;
    }
    else
#endif
    {
        lcd_index        = next_lcd_index();
        out_info.addrRGB = (uint32_t)itv_rcs.dbuf[lcd_index];
    }

#if (CFG_CHIP_FAMILY != 970 && CFG_CHIP_FAMILY != 960)
    if (IS_VERTICAL_DISPLAY())
    {
        out_info.width    = ithLcdGetHeight();
        out_info.height   = ithLcdGetWidth();
        out_info.pitchRGB = out_info.width * (ithLcdGetPitch() / ithLcdGetWidth());
    }
    else
#endif
    {
        rot = itv_get_rotation();
printf("\n");
printf("===== flip_lcd1 [%d]\n",rot);
printf("===== flip_lcd1 [%d]\n",rot);
printf("===== flip_lcd1 [%d]\n",rot);
printf("\n");
        switch (rot)
        {
        case ITU_ROT_90:
        case ITU_ROT_270:
            out_info.width  = ithLcdGetHeight();
            out_info.height = ithLcdGetWidth();
            out_info.pitchRGB = ithLcdGetPitch() * ithLcdGetHeight() / ithLcdGetWidth();
            break;

        default:
        #ifdef CFG_ITV_DYNAMIC_ROTATE //JPHong 2024-09-25
            if(ithLcdGetWidth() >= ithLcdGetHeight())
            {
                out_info.width    = ithLcdGetWidth();
                out_info.height   = ithLcdGetWidth();
				out_info.pitchRGB = ithLcdGetWidth() * CFG_LCD_BPP;
            }
            else
            {
                out_info.width    = ithLcdGetHeight();
                out_info.height   = ithLcdGetHeight();   
				out_info.pitchRGB = ithLcdGetHeight() * CFG_LCD_BPP;
            }
            
        #else
            out_info.width    = ithLcdGetWidth();
            out_info.height   = ithLcdGetHeight();
            out_info.pitchRGB = ithLcdGetPitch();        
        #endif
            break;
        }
    }
    out_info.format = MMP_ISP_OUT_DITHER565A;

    if (isp_share.format != MMP_ISP_IN_RGB565)
    {
		if (mmpIspQuery(gIspDev, MMP_ISP_CAP_ONFLY_MODE))
		 {
			 out_info.width  = vid_window.width;
			 out_info.height = vid_window.height;
		
			 rot			 = itv_get_rotation();
printf("\n");
printf("===== flip_lcd2 [%d]\n",rot);
printf("===== flip_lcd2 [%d]\n",rot);
printf("===== flip_lcd2 [%d]\n",rot);
printf("\n");		
			 switch (rot)
			 {
				 case ITU_ROT_90:
				 case ITU_ROT_270:
					 out_info.pitchRGB = ithLcdGetPitch() * ithLcdGetHeight() / ithLcdGetWidth();
					 break;
		
				 default:
				 	#ifdef CFG_ITV_DYNAMIC_ROTATE //JPHong 2024-09-25
			            if(ithLcdGetWidth() >= ithLcdGetHeight())
			            {
							out_info.pitchRGB = ithLcdGetWidth() * CFG_LCD_BPP;
			            }
			            else
			            { 
							out_info.pitchRGB = ithLcdGetHeight() * CFG_LCD_BPP;
			            }
       				 #else
					 	out_info.pitchRGB = ithLcdGetPitch();
					 #endif
					 break;
			 }
		
			 out_info.addrRGB	 = (uint32_t)itv_rcs.dbuf[0];
			 out_info.startX	 = vid_window.startX;
			 out_info.startY	 = vid_window.startY;
		
			 if (mmpIspQuery(gIspDev, MMP_ISP_OUTPUT_FIELD_MODE))
			 {
				 vid_window.height	 /= 2;
				 out_info.height			 /= 2;
				 out_info.startY			 /= 2;
			 }
		
			 mmpIspSetOutputWindow(gIspDev, &out_info);
			 mmpIspSetVideoWindow(gIspDev,
					 0,
					 0,
					 vid_window.width,
					 vid_window.height);
		 }
		 else
		 {
	#ifdef CFG_ITV_VP_HIGH_QUALITY
			 mmpIspDisableLCDOnFlyMode(gIspDev);
			 mmpIspSetMode(gIspDev, MMP_ISP_MODE_PLAY_VIDEO);
	#endif
			 mmpIspSetOutputWindow(gIspDev, &out_info);
			 mmpIspSetVideoWindow(gIspDev,
					 vid_window.startX,
					 vid_window.startY,
					 vid_window.width,
					 vid_window.height);
		 }


    #ifdef CFG_ITV_VP_HIGH_QUALITY
        mmpIspDisable(gIspDev, MMP_ISP_FRAME_FUNCTION_0);
        mmpIspDisable(gIspDev, MMP_ISP_FRAME_FUNCTION_1);

        CMDQ_Queue_index = itv_rcs.disp_buf_ridx + ITV_2D_INDEX_0;
        gITVCMDQ_2DIndexInput = CMDQ_Queue_index;
        if (xQueueSend(gISPCMDQMSGQueue, &CMDQ_Queue_index, 0) != pdPASS)
        {
            printf("[ITV] ISP CMDQ MSG is overflowing\n");
        }
    #endif
        mmpIspPlayImageProcess(gIspDev, &isp_share);
    #ifdef CFG_ITV_VP_HIGH_QUALITY
        SHORT_WAIT_UNTIL(gITVCMDQ_2DIndexInput == gITVCMDQ_2DIndex);
    #endif

        if (mmpIspQuery(gIspDev, MMP_ISP_CAP_ONFLY_MODE)) return;

    #ifdef _WIN32
        SHORT_WAIT_UNTIL(mmpIspIsEngineIdle(gIspDev));
    #else
        #ifndef CFG_ITV_VP_HIGH_QUALITY
            sem_wait(&gtISPEvent);
        #endif
    #endif
    }
    
#if (CFG_CHIP_FAMILY != 970 && CFG_CHIP_FAMILY != 960)
    if (IS_ROTATE_DISPLAY())
    {
        MMP_M2D_SURFACE *m2dtempSurf;
        float           angle;
        MMP_INT         dX;
        MMP_INT         dY;
        MMP_INT         cX;
        MMP_INT         cY;

        lcd_index = next_lcd_index();

        switch (gRotation)
        {
        case ITU_ROT_90:
            angle = 90.0f;
            dX    = ithLcdGetWidth() - 1;
            dY    = 0;
            cX    = 0;
            cY    = 0;
            break;

        case ITU_ROT_270:
            angle = 270.0f;
            dX    = 0;
            dY    = 0;
            cX    = ithLcdGetHeight();
            cY    = 0;
            break;

        default:
            angle = 0;
            break;
        }

        m2dtempSurf = &_gLcdSurf[lcd_index];
    #ifdef CFG_M2D_ENABLE
        mmpM2dTransformations(
                *m2dtempSurf,
                dX,
                dY,
                *_gRoateSurf,
                cX,
                cY,
                angle,
                1.0f);

        mmpM2dWaitIdle();
    #endif
    }
#endif

    g_lcd_index = lcd_index;
}

static void itv_lcdISR(void *arg __attribute__((__unused__)))
{
    if (gtFlipCtrl.bInited)
        sem_post(&gtFlipCtrl.tScanlineEvent);

    ithLcdIntrClear();
}

static void itv_isp0ISR(void *arg __attribute__((__unused__)))
{
    sem_post(&gtISPEvent);
    mmpIspClearInterrupt(gIspDev);

    if (mmpIspQuery(gIspDev, MMP_ISP_CAP_ONFLY_MODE) && !itv_rcs.check_capvideo_surf)
        itv_rcs.check_capvideo_surf = true;
}

#ifdef CFG_ITV_VP_HIGH_QUALITY
static void itv_isp1ISR(void *arg __attribute__((__unused__)))
{
    ITV_ISP_CMDQ_INDEX index = ITV_ENC_OSD_INDEX_0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (gIspDev)
        mmpIspClearInterrupt(gIspDev);

    if (gISPCMDQMSGQueue)
    {
        if (xQueueReceiveFromISR(gISPCMDQMSGQueue, &index, &xHigherPriorityTaskWoken) == pdPASS)
        {
            //ithPrintf("(%lu)ITV index %d\n", xTaskGetTickCountFromISR(), index);
            if (index == ITV_ENC_OSD_INDEX_0 || index == ITV_ENC_OSD_INDEX_1 || index == ITV_ENC_OSD_INDEX_2)
            {
                gITVCMDQ_ENCIndex = index;
                VideoEncoder_SetEncodeIdx(index);
            }
            else if (index == ITV_2D_INDEX_0 || index == ITV_2D_INDEX_1 || index == ITV_2D_INDEX_2)
            {
                gITVCMDQ_2DIndex = index;
            }
            else if (index == ITV_JPEG_OSD_INDEX_0)
            {
                gITVCMDQ_JPEGIndex = index;
            }
        }
    }
}
#else
static void itv_isp1ISR(void *arg __attribute__((__unused__)))
{
    sem_post(&gtISPEvent);
    mmpIspClearInterrupt(gIspDev);

    if (mmpIspQuery(gIspDev, MMP_ISP_CAP_ONFLY_MODE) && !itv_rcs.check_capvideo_surf)
        itv_rcs.check_capvideo_surf = true;
}
#endif

static void _itv_hw_overlay_task(void)
{
    int         i;
    ITV_STRC_FF *ff;
    int         widx, ridx;
#ifdef CFG_ITV_VP_HIGH_QUALITY
    ITV_DBUF_PROPERTY encDispProp;
#endif

    if (itv_rcs.mtal_pb_mode_u != -1)
    {
    #ifndef FRAME_FUNCTION_ENABLE
        if (itv_rcs.mtal_pb_mode_isr == 0 && itv_rcs.mtal_pb_mode_u == 1)
            itv_rcs.mtal_pb_mode_u = 2;
    #endif

        itv_rcs.mtal_pb_mode_isr = itv_rcs.mtal_pb_mode_u;
        itv_rcs.mtal_pb_mode_u   = -1;
    }

#ifndef FRAME_FUNCTION_ENABLE
    if (itv_rcs.mtal_pb_mode_isr == 2)
    {
        itv_rcs.check_video_surf = true;
        itv_rcs.mtal_pb_mode_isr = 1;
        itv_rcs.disp_buf_ridx = 0;
        itv_rcs.disp_buf_widx = 1;
        itv_rcs.vid_surf_ridx = itv_rcs.vid_surf_widx = 0;

        for (i = 0; i < ITV_FF_NSET; i++)
        {
            ff          = &(itv_rcs.ff[i]);
            ff->ui_widx = ff->ui_ridx = 0;
        }

        if (mmpIspQuery(gIspDev, MMP_ISP_CAP_ONFLY_MODE))
            memcpy((void *)&itv_rcs.dbufprop[itv_rcs.disp_buf_ridx], (void *)&gCapPROP, sizeof(ITV_DBUF_PROPERTY));
        else
            memcpy((void *)&itv_rcs.dbufprop[itv_rcs.disp_buf_ridx], (void *)&itv_rcs.blank_yuv_frame, sizeof(ITV_DBUF_PROPERTY));
    }
#endif

    if (itv_rcs.mtal_pb_mode_isr == 0)
        memcpy((void *)&itv_rcs.dbufprop[itv_rcs.disp_buf_ridx], (void *)&itv_rcs.blank_yuv_frame, sizeof(ITV_DBUF_PROPERTY));

#ifdef FRAME_FUNCTION_ENABLE
    #if defined(CFG_LCD_ENABLE)
        if (g_lcd_index >= 0)
        {
            ithLcdSwFlip(g_lcd_index);

            if (gtFlipCtrl.bInited)
                sem_wait(&gtFlipCtrl.tScanlineEvent);
            g_lcd_index = -1;
        }
    #endif
#endif

    for (i = 0; i < ITV_FF_NSET; i++)
    {
        if (itv_rcs.ff_mode_u[i] != -1)
        {
            itv_rcs.ff_mode_isr[i] = itv_rcs.ff_mode_u[i];
            itv_rcs.ff_mode_u[i]   = -1;
        }
    }

#ifdef CFG_ITV_VP_HIGH_QUALITY
    for (i = 0; i < ITV_FF_NSET; i++)
    {
        if (itv_rcs.osd_mode_u[i] != -1)
        {
            itv_rcs.osd_mode_isr[i] = itv_rcs.osd_mode_u[i];
            itv_rcs.osd_mode_u[i]   = -1;
        }
    }
#endif

    if (itv_rcs.ff_setbase == 1)
    {
        const int   id  = itv_rcs.ff_setbase_id;
        const int   bid = itv_rcs.ff_setbase_bid;
        ITV_STRC_FF *ff = &(itv_rcs.ff[id]);

        ff->uibuf[bid]     = itv_rcs.ff_setbase_base;
        itv_rcs.ff_setbase = 0;
    }

#ifdef CFG_ITV_VP_HIGH_QUALITY
    if (itv_rcs.osd_setbase == 1)
    {
        const int   id  = itv_rcs.osd_setbase_id;
        const int   bid = itv_rcs.osd_setbase_bid;
        ITV_STRC_FF *ff = &(itv_rcs.osd[id]);

        ff->uibuf[bid]      = itv_rcs.osd_setbase_base;
        itv_rcs.osd_setbase = 0;
    }

    update_osd();

    pthread_mutex_lock(&OSD_PREVIEW_MUTEX);

    if (itv_rcs.disp_buf_ridx == itv_rcs.disp_buf_widx)
    {
        if (xQueueReceive(gITVENCQueue, &encDispProp, 0))
        {
            flip_enc_osd(&encDispProp);
            //VideoEncoder_SetEncodeIdx(0);
        }
    }

    if (itv_rcs.osd_update)
    {
        for (i = 0; i < ITV_FF_NSET; i++)
        {
            ff   = &(itv_rcs.osd[i]);
            widx = ff->ui_widx;
            ridx = ff->ui_ridx;

            if (itv_rcs.osd_mode_isr[i] == 1)
                ff->ui_ridx = (ridx + 1) % ITV_FF_NBUF;
        }

        itv_rcs.osd_update = false;
    }
#endif

    update_ui();

    if ((itv_rcs.disp_buf_ridx != itv_rcs.disp_buf_widx) && (itv_check_vidSurf_anchor() == 1))
    {
        itv_rcs.curr_video   = &itv_rcs.dbufprop[itv_rcs.disp_buf_ridx];
        itv_rcs.video_update = true;
    }
    else
    {
        itv_rcs.pcmd_flush_dbuf = 0;    // set this flag to 0 means all queued display buffers have been flushed.
    }

#ifdef CFG_ITV_VP_HIGH_QUALITY
    pthread_mutex_unlock(&OSD_PREVIEW_MUTEX);
#endif

    if (itv_rcs.video_update || itv_rcs.ui_update)
    {
        flip_lcd(itv_rcs.disp_buf_ridx);

        if (itv_rcs.video_update)
            itv_rcs.disp_buf_ridx = (itv_rcs.disp_buf_ridx + 1) % ITV_MAX_DISP_BUF;

        if (itv_rcs.ui_update)
        {
            for (i = 0; i < ITV_FF_NSET; i++)
            {
                ff   = &(itv_rcs.ff[i]);
                widx = ff->ui_widx;
                ridx = ff->ui_ridx;

                if (itv_rcs.ff_mode_isr[i] == 1)
                    ff->ui_ridx = (ridx + 1) % ITV_FF_NBUF;
            }
        }

    #ifndef FRAME_FUNCTION_ENABLE
        itv_rcs.vid_surf_widx = (itv_rcs.vid_surf_widx + 1) % ITV_MAX_VSBUF;
    #endif

        itv_rcs.video_update = false;
        itv_rcs.ui_update    = false;

        if ((itv_rcs.curr_video)->src_w != 16)
            itv_rcs.bVideo = true;
        else
            itv_rcs.bVideo = false;
    }
}

#ifdef _WIN32
static g_stop_lcd_isr_task;

static void *lcd_isr_task(void *arg)
{
#ifdef DEBUG_FLIP
    printf("[LCD ISR] Enter\n");
#endif

    g_stop_lcd_isr_task = false;
    while (!g_stop_lcd_isr_task)
    {
        itv_lcdISR(arg);
        usleep(16000);
    }

#ifdef DEBUG_FLIP
    printf("[LCD ISR] Leave\n");
#endif

    pthread_exit(NULL);

    return (void *)0;
}
#endif

static void *itv_hw_overlay_task(void *arg)
{
    g_stop_itv_hw_overlay_task = false;

    while (!g_stop_itv_hw_overlay_task || itv_rcs.mtal_pb_mode_isr != 0)
    {
        _itv_hw_overlay_task();
        usleep(1000);
    }

    pthread_exit(NULL);

    return (void *)0;
}

static void itv_change_display_format()
{
    MMP_ISP_CORE_INFO   ISPCOREINFO = {0};
    MMP_ISP_OUTPUT_INFO outInfo     = {0};
    int                 width       = ithLcdGetWidth();
    int                 height      = ithLcdGetHeight();
    ITURotation         rot;
#ifdef CFG_ITV_VP_HIGH_QUALITY
    pthread_attr_t attr = {0};
    struct sched_param param = {0};

    pthread_attr_init(&attr);
    param.sched_priority = 5;
    pthread_attr_setschedparam(&attr, &param);
#endif

    rot = itv_get_rotation();

    switch (rot)
    {
    case ITU_ROT_90:
    case ITU_ROT_270:
        width  = ithLcdGetHeight();
        height = ithLcdGetWidth();
        break;

    default:
        width  = ithLcdGetWidth();
        height = ithLcdGetHeight();
        break;
    }

#ifdef CFG_ITV_VP_NORMAL
    mmpIspInitialize(&gIspDev, MMP_ISP_CORE_0);
#elif defined(CFG_LCD_PQ_TUNING)
    mmpIspInitialize(&gIspDev, MMP_ISP_CORE_1);
#elif defined(CFG_ITV_VP_HIGH_QUALITY)
    mmpIspInitialize(&gIspDev, MMP_ISP_CORE_1);
#endif

    ISPCOREINFO.EnPreview = false;
    ISPCOREINFO.PreScaleSel = MMP_ISP_PRESCALE_NORMAL;
    mmpIspSetCore(gIspDev, &ISPCOREINFO);
    mmpIspSetMode(gIspDev, MMP_ISP_MODE_PLAY_VIDEO);
    mmpIspSetDisplayWindow(gIspDev, 0, 0, width, height);
    mmpIspSetOutputFormat(gIspDev, MMP_ISP_OUT_DITHER565A);

#ifdef _WIN32
    mmpIspDisableInterrupt(gIspDev);
#else
    mmpIspEnableInterrupt(gIspDev);
#endif

#ifdef FRAME_FUNCTION_ENABLE
    ithLcdSwFlip(ithLcdGetFlip());
    ithLcdDisableHwFlip();
#endif

    if (!gtFlipCtrl.bInited)
        gtFlipCtrl.bInited = !sem_init(&gtFlipCtrl.tScanlineEvent, 0, 1);

    sem_init(&gtISPEvent, 0, 1);
    sem_wait(&gtISPEvent);

    if (gtFlipCtrl.bInited)
    {
        g_lcd_index = -1;
    #ifdef CFG_ITV_VP_HIGH_QUALITY
        pthread_create(&gtFlipCtrl.hw_overlay_tid, &attr, itv_hw_overlay_task, NULL);
    #else
        pthread_create(&gtFlipCtrl.hw_overlay_tid, NULL, itv_hw_overlay_task, NULL);
    #endif

    #ifndef WIN32
        #ifdef FRAME_FUNCTION_ENABLE
            uint32_t lcd_height = ithLcdGetHeight();
            ithIntrDisableIrq(ITH_INTR_LCD);
            ithLcdIntrClear();
            ithIntrClearIrq(ITH_INTR_LCD);
            ithIntrRegisterHandlerIrq(ITH_INTR_LCD, itv_lcdISR, NULL);
            ithLcdIntrSetScanLine1(lcd_height);
            ithLcdIntrSetScanLine2(lcd_height);
            ithLcdIntrCtrlDisable(ITH_LCD_INTR_OUTPUT2);
            ithLcdIntrCtrlDisable(ITH_LCD_INTR_FIELDMODE2);
            ithLcdIntrCtrlDisable(ITH_LCD_INTR_OUTPUT1);
            ithLcdIntrCtrlDisable(ITH_LCD_INTR_FIELDMODE1);
            ithLcdIntrCtrlEnable(ITH_LCD_INTR_OUTPUT2);
            ithIntrEnableIrq(ITH_INTR_LCD);
            ithLcdIntrEnable();
        #endif

        #ifdef CFG_ITV_VP_NORMAL
            mmpISPRegisterIRQ(gIspDev, itv_isp0ISR);
        #elif defined(CFG_LCD_PQ_TUNING)
            mmpISPRegisterIRQ(gIspDev, itv_isp1ISR);
        #elif defined(CFG_ITV_VP_HIGH_QUALITY)
            mmpISPRegisterIRQ(gIspDev, itv_isp1ISR);
            mmpIspEnableCMDQ(gIspDev);
        #endif
    #else
        pthread_create(&gtFlipCtrl.lcd_isr_tid, NULL, lcd_isr_task, NULL);
    #endif
    }
}

int itv_init(void)
{
    int i;

    printf("ITV# init !\n");

    if (ITV_MAX_NDBUF > 3)
    {
        printf("ERROR# invalid ITV_MAX_NDBUF (%d), %s:%d !\n", ITV_MAX_NDBUF, __FILE__, __LINE__);
        return -1;
    }

    g_blank_buf = itpVmemAlloc(16*16*2);

    memset((void *)&itv_rcs, 0, sizeof(ITV_STRC_RESOURCE));
    memset((void *)&gCapPROP, 0, sizeof(ITV_DBUF_PROPERTY));

#ifdef CFG_ITV_VP_HIGH_QUALITY
    g_thumbnail_buf = itpVmemAlloc(thumbnail_w * thumbnail_h * 2);

    if (gITVENCQueue == NULL) gITVENCQueue = xQueueCreate((UBaseType_t)(ITV_ENC_OSD_INDEX_2 + 1), (UBaseType_t)sizeof(ITV_DBUF_PROPERTY));
    else printf("ERROR# invalid gITVENCQueue (%x), %s:%d !\n", gITVENCQueue, __FILE__, __LINE__);

    if (gISPCMDQMSGQueue == NULL) gISPCMDQMSGQueue = xQueueCreate((UBaseType_t)(ITV_2D_INDEX_2 + 1), (UBaseType_t)sizeof(int));
    else printf("ERROR# invalid gISPCMDQMSGQueue (%x), %s:%d !\n", gISPCMDQMSGQueue, __FILE__, __LINE__);
#endif

    itv_rcs.blank_yuv_frame.ya       = (uint8_t *)g_blank_buf;
    itv_rcs.blank_yuv_frame.ua       = (uint8_t *)g_blank_buf + 256;
    itv_rcs.blank_yuv_frame.va       = (uint8_t *)g_blank_buf + 256 + 64;

    memset((void *) itv_rcs.blank_yuv_frame.ya, 0x10, 256);
    memset((void *) itv_rcs.blank_yuv_frame.ua, 0x80, 64);
    memset((void *) itv_rcs.blank_yuv_frame.va, 0x80, 64);

    itv_rcs.blank_yuv_frame.src_w    = 16;
    itv_rcs.blank_yuv_frame.src_h    = 16;
    itv_rcs.blank_yuv_frame.pitch_y  = 16;
    itv_rcs.blank_yuv_frame.pitch_uv = 16;
    itv_rcs.blank_yuv_frame.format   = MMP_ISP_IN_YUV420;
    itv_rcs.curr_video               = &itv_rcs.blank_yuv_frame;

#ifdef CFG_LCD_ENABLE
    itv_rcs.dbuf[0]                  = (uint8_t *)ithLcdGetBaseAddrA();
    itv_rcs.dbuf[1]                  = (uint8_t *)ithLcdGetBaseAddrB();
    itv_rcs.dbuf[2]                  = (uint8_t *)ithLcdGetBaseAddrC();
    g_lcd_index_counter              = ithLcdGetFlip();
#endif

    itv_rcs.mtal_pb_mode_u           = -1;
    itv_rcs.vid_surf_cur_idx         = -1;

    for (i = 0; i < ITV_FF_NSET; i++)
        itv_rcs.ff_mode_u[i] = -1;

#ifdef CFG_ITV_VP_HIGH_QUALITY
    for (i = 0; i < ITV_FF_NSET; i++)
        itv_rcs.osd_mode_u[i] = -1;

    gITVCMDQ_ENCIndexInput = ITV_2D_INDEX_2;
    gITVCMDQ_ENCIndex = ITV_2D_INDEX_2;
    gITVCMDQ_JPEGIndexInput = ITV_2D_INDEX_2;
    gITVCMDQ_JPEGIndex = ITV_2D_INDEX_2;
    gITVCMDQ_2DIndexInput = ITV_ENC_OSD_INDEX_2;
    gITVCMDQ_2DIndex = ITV_ENC_OSD_INDEX_2;
#endif

    itv_change_display_format();

    //fc_init_sync(&itv_rcs.itv_sync);    // sync module should be done in the player

    return 0;
}

int itv_deinit(void)
{
#ifdef CFG_ITV_VP_HIGH_QUALITY
    int i = 0;
    QueueHandle_t gITVENCQueue_temp = gITVENCQueue;
    QueueHandle_t gISPCMDQMSGQueue_temp = gISPCMDQMSGQueue;
#endif

    printf("ITV# de-init !\n");

    if (gtFlipCtrl.bInited)
    {
        g_stop_itv_hw_overlay_task = true;
        pthread_join(gtFlipCtrl.hw_overlay_tid, NULL);

    #ifdef CFG_ITV_VP_HIGH_QUALITY
        if (gITVCMDQ_ENCIndexInput != ITV_2D_INDEX_2)
            SHORT_WAIT_UNTIL(gITVCMDQ_ENCIndexInput == gITVCMDQ_ENCIndex);
        if (gITVCMDQ_2DIndexInput != ITV_ENC_OSD_INDEX_2)
            SHORT_WAIT_UNTIL(gITVCMDQ_2DIndexInput == gITVCMDQ_2DIndex);
    #endif

    #ifndef WIN32
        #ifdef FRAME_FUNCTION_ENABLE
            ithLcdIntrDisable();
        #endif
    #else
        g_stop_lcd_isr_task = true;
        pthread_join(gtFlipCtrl.lcd_isr_tid, NULL);
    #endif

        sem_destroy(&(gtFlipCtrl.tScanlineEvent));
        memset(&gtFlipCtrl, 0, sizeof(gtFlipCtrl));

        sem_destroy(&gtISPEvent);
        memset(&gtISPEvent, 0, sizeof(gtISPEvent));
    }

#ifdef CFG_ITV_VP_HIGH_QUALITY
    pthread_mutex_lock(&OSD_PREVIEW_MUTEX);

    gITVENCQueue = NULL;
    gISPCMDQMSGQueue = NULL;
#endif

    if (mmpIspQuery(gIspDev, MMP_ISP_CAP_ONFLY_MODE))
    {
        mmpIspResetEngine(gIspDev);
        ithIspResetReg(ITH_ISP_CORE0);
        while (!mmpIspIsEngineIdle(gIspDev));
    }

    mmpIspTerminate(&gIspDev);

#ifdef CFG_ITV_VP_HIGH_QUALITY
    vQueueDelete(gITVENCQueue_temp);
    vQueueDelete(gISPCMDQMSGQueue_temp);

    pthread_mutex_unlock(&OSD_PREVIEW_MUTEX);
#endif

    //fc_deinit_sync(&itv_rcs.itv_sync);

    memset((void *)&itv_rcs, 0, sizeof(ITV_STRC_RESOURCE));
    memset((void *)&gCapPROP, 0, sizeof(ITV_DBUF_PROPERTY));

#ifdef CFG_ITV_VP_HIGH_QUALITY
    itpVmemFree(g_thumbnail_buf);
#endif

    itpVmemFree(g_blank_buf);

    return 0;
}

uint8_t *itv_get_dbuf_anchor(void)
{    
    if (QUEUE_IS_FULL(itv_rcs.disp_buf_widx, itv_rcs.disp_buf_ridx, ITV_MAX_DISP_BUF))
        return NULL;

    return itv_rcs.dbuf[itv_rcs.disp_buf_widx];
}

#ifdef CFG_ITV_VP_HIGH_QUALITY
int itv_update_osd_anchor(ITV_DBUF_PROPERTY *prop, ITV_DBUF_PROPERTY *prop2)
{
    pthread_mutex_lock(&OSD_PREVIEW_MUTEX);

    if (gITVENCQueue)
    {
        if (xQueueSend(gITVENCQueue, prop, 0) != pdPASS)
        {
            ithPrintf("[itv_update_osd_anchor] ITV ENC Queue is overflowing\n");
        }
    }

    gITVCMDQ_JPEGIndex = ITV_2D_INDEX_2;
    gITVCMDQ_JPEGIndexInput = ITV_JPEG_OSD_INDEX_0;

    pthread_mutex_unlock(&OSD_PREVIEW_MUTEX);

    SHORT_WAIT_UNTIL(gITVCMDQ_JPEGIndexInput == gITVCMDQ_JPEGIndex);

    prop2->ya = g_thumbnail_buf;
    prop2->ua = g_thumbnail_buf + thumbnail_w * thumbnail_h;
    prop2->va = g_thumbnail_buf + thumbnail_w * thumbnail_h;
    prop2->src_w = thumbnail_w;
    prop2->src_h = thumbnail_h;
    prop2->pitch_y = thumbnail_w;
    prop2->pitch_uv = thumbnail_w;

    return 0;
}
#endif

int itv_update_dbuf_anchor(ITV_DBUF_PROPERTY *prop)
{
    if (QUEUE_IS_FULL(itv_rcs.disp_buf_widx, itv_rcs.disp_buf_ridx, ITV_MAX_DISP_BUF))
        return -1;

    memcpy((void *)&itv_rcs.dbufprop[itv_rcs.disp_buf_widx], (void *)prop, sizeof(ITV_DBUF_PROPERTY));
    itv_rcs.disp_buf_widx = (itv_rcs.disp_buf_widx + 1) % ITV_MAX_DISP_BUF;

    return 0;
}

#ifdef CFG_ITV_VP_HIGH_QUALITY
int itv_update_osddbuf_anchor(ITV_DBUF_PROPERTY *prop, ITV_DBUF_PROPERTY *prop2)
{
    if (QUEUE_IS_FULL(itv_rcs.disp_buf_widx, itv_rcs.disp_buf_ridx, ITV_MAX_DISP_BUF))
        return -1;

    pthread_mutex_lock(&OSD_PREVIEW_MUTEX);

    if (gITVENCQueue)
    {
        if (xQueueSend(gITVENCQueue, prop, 0) != pdPASS)
        {
            ithPrintf("[itv_update_osddbuf_anchor] ITV ENC Queue is overflowing\n");
        }
    }

    memcpy((void *)&itv_rcs.dbufprop[itv_rcs.disp_buf_widx], (void *)prop, sizeof(ITV_DBUF_PROPERTY));
    itv_rcs.disp_buf_widx = (itv_rcs.disp_buf_widx + 1) % ITV_MAX_DISP_BUF;

    gITVCMDQ_JPEGIndex = ITV_2D_INDEX_2;
    gITVCMDQ_JPEGIndexInput = ITV_JPEG_OSD_INDEX_0;

    pthread_mutex_unlock(&OSD_PREVIEW_MUTEX);

    SHORT_WAIT_UNTIL(gITVCMDQ_JPEGIndexInput == gITVCMDQ_JPEGIndex);

    prop2->ya = g_thumbnail_buf;
    prop2->ua = g_thumbnail_buf + thumbnail_w * thumbnail_h;
    prop2->va = g_thumbnail_buf + thumbnail_w * thumbnail_h;
    prop2->src_w = thumbnail_w;
    prop2->src_h = thumbnail_h;
    prop2->pitch_y = thumbnail_w;
    prop2->pitch_uv = thumbnail_w;

    return 0;
}
#endif

void itv_flush_dbuf(void)
{
    itv_rcs.pcmd_flush_dbuf = 1;

    WAIT_UNTIL(itv_rcs.pcmd_flush_dbuf == 0);
}

void itv_set_vidSurf_buf(uint8_t* addr, uint8_t index)
{
    itv_rcs.dbuf[index] = addr;
}

bool itv_get_new_video(void)
{
    if (itv_rcs.vid_surf_ridx != itv_rcs.vid_surf_widx)
        return true;
    else
        return false;
}

int itv_get_vidSurf_index(void)
{
    unsigned int value = 0;

    if (mmpIspQuery(gIspDev, MMP_ISP_CAP_ONFLY_MODE))
    {
        value = ithReadRegA((0xD0300000 + 0x3FC));    // read last finish index of VP
        value = (value & 0x300) >> 8;
    
        if (itv_rcs.mtal_pb_mode_isr != 1 || !itv_rcs.check_capvideo_surf)
            return -1;
        else if(!itv_rcs.bVideo)
            return -2;

        return value;
    }
    else
    {
        if (itv_rcs.vid_surf_ridx != itv_rcs.vid_surf_widx)
        {
            itv_rcs.vid_surf_ridx = (itv_rcs.vid_surf_ridx + 1) % ITV_MAX_VSBUF;
            itv_rcs.vid_surf_cur_idx = g_lcd_index;
        }

        if (itv_rcs.mtal_pb_mode_isr != 1)
            return -1;
        else if(!itv_rcs.bVideo)
            return -2;

        return itv_rcs.vid_surf_cur_idx;
    }
}

int itv_check_vidSurf_anchor(void)
{    
#ifndef FRAME_FUNCTION_ENABLE
    if (QUEUE_IS_FULL(itv_rcs.vid_surf_widx, itv_rcs.vid_surf_ridx, ITV_MAX_VSBUF) && itv_rcs.check_video_surf)
        return -1;
#endif

    return 1;
}

void itv_stop_vidSurf_anchor(void)
{
#ifndef FRAME_FUNCTION_ENABLE
    itv_rcs.check_video_surf = false;
#endif
}

int itv_ff_setup_base(int id, int bid, uint8_t *base)
{
    itv_rcs.ff_setbase_id   = id;
    itv_rcs.ff_setbase_bid  = bid;
    itv_rcs.ff_setbase_base = base;
    itv_rcs.ff_setbase      = 1;

    WAIT_UNTIL(itv_rcs.ff_setbase == 0);

    return 0;
}

int itv_ff_enable(int id, int enable)
{
    printf("ITV# %s, id(%d):%d +\n", __func__, id, enable);

    itv_rcs.ff_mode_u[id] = enable;
    WAIT_UNTIL(itv_rcs.ff_mode_u[id] == -1);

    printf("ITV# %s, id(%d):%d -\n", __func__, id, enable);

    return 0;
}

uint8_t *itv_get_uibuf_anchor(int id)
{
    ITV_STRC_FF *ff  = &(itv_rcs.ff[id]);
    const int   widx = ff->ui_widx;
    const int   ridx = ff->ui_ridx;

    if (QUEUE_IS_FULL(widx, ridx, ITV_FF_NBUF))
        return NULL;

    return ff->uibuf[widx];
}

int itv_update_uibuf_anchor(int id, ITV_UI_PROPERTY *uiprop)
{
    ITV_STRC_FF *ff  = &(itv_rcs.ff[id]);
    const int   widx = ff->ui_widx;
    const int   ridx = ff->ui_ridx;

    if (QUEUE_IS_FULL(widx, ridx, ITV_FF_NBUF))
        return -1;

    memcpy((void *)&(ff->uiprop[widx]), (void *)uiprop, sizeof(ITV_UI_PROPERTY));
    ff->ui_widx = (widx + 1) % ITV_FF_NBUF;

    return 0;
}

void itv_flush_uibuf(int id)
{
    itv_rcs.pcmd_flush_uibuf[id] = 1;

    WAIT_UNTIL(itv_rcs.pcmd_flush_uibuf[id] == 0);
}

#ifdef CFG_ITV_VP_HIGH_QUALITY
int itv_osd_setup_base(int id, int bid, uint8_t *base)
{
    itv_rcs.osd_setbase_id   = id;
    itv_rcs.osd_setbase_bid  = bid;
    itv_rcs.osd_setbase_base = base;
    itv_rcs.osd_setbase      = 1;

    WAIT_UNTIL(itv_rcs.osd_setbase == 0);

    return 0;
}

int itv_osd_enable(int id, int enable)
{
    printf("ITV# %s, id(%d):%d +\n", __func__, id, enable);

    itv_rcs.osd_mode_u[id] = enable;
    WAIT_UNTIL(itv_rcs.osd_mode_u[id] == -1);

    printf("ITV# %s, id(%d):%d -\n", __func__, id, enable);

    return 0;
}

uint8_t *itv_get_osdbuf_anchor(int id)
{
    ITV_STRC_FF *ff  = &(itv_rcs.osd[id]);
    const int   widx = ff->ui_widx;
    const int   ridx = ff->ui_ridx;

    if (!gITVENCQueue)
        return NULL;

    if (QUEUE_IS_FULL(widx, ridx, ITV_FF_NBUF))
        return NULL;

    return ff->uibuf[widx];
}

int itv_update_osdbuf_anchor(int id, ITV_UI_PROPERTY *osdprop)
{
    ITV_STRC_FF *ff  = &(itv_rcs.osd[id]);
    const int   widx = ff->ui_widx;
    const int   ridx = ff->ui_ridx;

    if (!gITVENCQueue)
        return -1;

    if (QUEUE_IS_FULL(widx, ridx, ITV_FF_NBUF))
        return -1;

    memcpy((void *)&(ff->uiprop[widx]), (void *)osdprop, sizeof(ITV_UI_PROPERTY));
    ff->ui_widx = (widx + 1) % ITV_FF_NBUF;

    return 0;
}

void itv_flush_osdbuf(int id)
{
    itv_rcs.pcmd_flush_osdbuf[id] = 1;

    WAIT_UNTIL(itv_rcs.pcmd_flush_osdbuf[id] == 0);
}
#endif

// MTAL pb mode, 1 means in playback mode, 0 means not in playback mode
int itv_set_pb_mode(int pb_mode)
{
    printf("%s(%d) pb_mode(%d)\n", __FUNCTION__, __LINE__, pb_mode);

    WAIT_UNTIL(itv_rcs.mtal_pb_mode_u == -1);
    itv_rcs.mtal_pb_mode_u = pb_mode;
    SHORT_WAIT_UNTIL(itv_rcs.mtal_pb_mode_u == -1);

    printf("%s(%d)\n", __FUNCTION__, __LINE__);

    return itv_rcs.mtal_pb_mode_u;
}

ITURotation itv_get_rotation(void)
{
    return gRotation;
}

void itv_set_rotation(ITURotation rot)
{
    gRotation = rot;

#if (CFG_CHIP_FAMILY != 970 && CFG_CHIP_FAMILY != 960)
    if (IS_ROTATE_DISPLAY() && (!gRotateBuffer[0]))
    {
        int i;
        int size = ithLcdGetPitch() * ithLcdGetHeight() * 2;

        gRotateBuffer[0] = itpVmemAlloc(size);
        for (i = 1; i < MAX_ROTATE_BUFFER_COUNT; i++)
        {
            gRotateBuffer[i] = gRotateBuffer[i - 1] + ithLcdGetPitch() * ithLcdGetHeight();
        }

    #ifdef CFG_M2D_ENABLE
        for (i = 0; i < MAX_ROTATE_BUFFER_COUNT; i++)
        {
            mmpM2dCreateVirtualSurface(
                    ithLcdGetHeight(),
                    ithLcdGetWidth(),
                    MMP_M2D_IMAGE_FORMAT_RGB565,
                    (MMP_UINT8 *)gRotateBuffer[i],
                    &gRoateSurf[i]);
        }
        for (i = 0; i < ITV_MAX_VSBUF; i++)
        {
            mmpM2dCreateVirtualSurface(
                    ithLcdGetWidth(),
                    ithLcdGetHeight(),
                    MMP_M2D_IMAGE_FORMAT_RGB565,
                    itv_rcs.dbuf[i],
                    &_gLcdSurf[i]);
        }
    #endif
    }
#endif
}

void
itv_set_video_window(
    uint32_t startX,
    uint32_t startY,
    uint32_t width,
    uint32_t height)
{
    vid_window.startX = startX;
    vid_window.startY = startY;
    vid_window.width  = width;
    vid_window.height = height;
}

ISP_RESULT
itv_enable_isp_feature(
    MMP_ISP_CAPS cap)
{
    return mmpIspEnable(gIspDev, cap);
}

void itv_set_color_correction(MMP_ISP_COLOR_CTRL colorCtrl, int value)
{
    mmpIspSetColorCtrl(gIspDev, colorCtrl, value);
}

void itv_set_isp_onfly(MMP_ISP_SHARE isp_share, bool interlaced)
{

	MMP_ISP_WRITE_BUFFER_INFO	 ispWbInfo	 = {0};
	ITURotation				 rot		 = {0};
	unsigned int                pitch       = 0;

	rot = itv_get_rotation();


//    ITU_ROT_0,      ///< No rotate
//    ITU_ROT_90,     ///< 90 degree rotate
//    ITU_ROT_180,    ///< 180 degree rotate
//    ITU_ROT_270     ///< 270 degree rotate
    
    
	#if 1	// JPHong 2024-09-25
		switch (rot)
		{
			case ITU_ROT_0:
			case ITU_ROT_180:
				if(ithLcdGetWidth() >= ithLcdGetHeight())
				{
					pitch = ithLcdGetWidth() * CFG_LCD_BPP;
				}
				else
				{
					pitch = ithLcdGetHeight() * CFG_LCD_BPP;
				}
			break;
			
			default:
				pitch  = ithLcdGetPitch();
			break;
		}
			
	#else		
		#ifdef CFG_ITV_DYNAMIC_ROTATE
			 if(ithLcdGetWidth() >= ithLcdGetHeight())
			 {
			 	pitch = ithLcdGetWidth() * CFG_LCD_BPP;
			 }
			 else
			 {
			 	pitch = ithLcdGetHeight() * CFG_LCD_BPP;
			 }
		#else
			 pitch  = ithLcdGetPitch();
		#endif
	#endif
	if (interlaced)
	{
		isp_share.height /= 2;
		mmpIspEnable(gIspDev, MMP_ISP_OUTPUT_FIELD_MODE);
		(void)printf("ISP_OUTPUT_FIELD_MODE\n");

		switch (rot)
		{
			case ITU_ROT_90:
			case ITU_ROT_270:
			 ispWbInfo.addrYRGB_1	 = (uint32_t)itv_rcs.dbuf[1] + 2 * vid_window.startX +
				 (pitch * ithLcdGetHeight() / ithLcdGetWidth() * vid_window.startY / 2);
			 ispWbInfo.addrYRGB_2	 = (uint32_t)itv_rcs.dbuf[2] + 2 * vid_window.startX +
				 (pitch * ithLcdGetHeight() / ithLcdGetWidth() * vid_window.startY / 2);
			 break;

			default:
			 ispWbInfo.addrYRGB_1	 = (uint32_t)itv_rcs.dbuf[1] + 2 * vid_window.startX +
				 (pitch * vid_window.startY / 2);
			 ispWbInfo.addrYRGB_2	 = (uint32_t)itv_rcs.dbuf[2] + 2 * vid_window.startX +
				 (pitch * vid_window.startY / 2);
			 break;
		}
	}
	else
	{
		mmpIspDisable(gIspDev, MMP_ISP_OUTPUT_FIELD_MODE);
		(void)printf("DISABLE ISP_OUTPUT_FIELD_MODE\n");

		switch (rot)
		{
		 case ITU_ROT_90:
		 case ITU_ROT_270:
			 ispWbInfo.addrYRGB_1	 = (uint32_t)itv_rcs.dbuf[1] + 2 * vid_window.startX +
				 (pitch * ithLcdGetHeight() / ithLcdGetWidth() * vid_window.startY);
			 ispWbInfo.addrYRGB_2	 = (uint32_t)itv_rcs.dbuf[2] + 2 * vid_window.startX +
				 (pitch * ithLcdGetHeight() / ithLcdGetWidth() * vid_window.startY);
			 break;

		 default:
			 ispWbInfo.addrYRGB_1	 = (uint32_t)itv_rcs.dbuf[1] + 2 * vid_window.startX +
				 (pitch * vid_window.startY);
			 ispWbInfo.addrYRGB_2	 = (uint32_t)itv_rcs.dbuf[2] + 2 * vid_window.startX +
				 (pitch * vid_window.startY);
			 break;
		}
	}

	mmpIspSetWriteBufferInfo(gIspDev, &ispWbInfo);
	mmpIspSetWriteBufferNum(gIspDev, ITV_MAX_VSBUF - 1);

	mmpIspEnableCAPOnFlyMode(gIspDev);

	gCapPROP.ya		 = (uint8_t *)isp_share.addrY;
	gCapPROP.ua		 = (uint8_t *)isp_share.addrU;
	gCapPROP.va		 = (uint8_t *)isp_share.addrV;
	gCapPROP.src_w 	 = (uint32_t)isp_share.width;
	gCapPROP.src_h 	 = (uint32_t)isp_share.height;
	gCapPROP.pitch_y	 = (uint32_t)isp_share.pitchY;
	gCapPROP.pitch_uv	 = (uint32_t)isp_share.pitchUv;
	gCapPROP.format	 = (uint32_t)isp_share.format;

}

