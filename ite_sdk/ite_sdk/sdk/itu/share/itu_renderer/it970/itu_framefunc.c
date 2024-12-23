#include <assert.h>
#include <string.h>
#include <unistd.h>
#include "itu_cfg.h"
#include "ite/ith.h"
#include "ite/itu.h"
#include "ite/itp.h"

#if defined(CFG_M2D_ENABLE)
    #include "gfx/gfx.h"
#endif

#if defined(CFG_VIDEO_ENABLE) && !defined(CFG_FFMPEG_H264_SW)
    #include "ite/itv.h"
    #include "ith/ith_video.h"
#endif
#include "itu_private.h"

//=============================================================================
//                              Constant Definition
//=============================================================================
#define MAX_UI_BUFFER_COUNT    2
//#if defined(CFG_VIDEO_ENABLE) && !defined(CFG_FFMPEG_H264_SW)
//    #define FRAME_BUFFER_COUNT 4
//#endif
#define VIDEO_SURFACE_COUNT    3
//=============================================================================
//                              Macro Definition
//=============================================================================

//=============================================================================
//                              Structure Definition
//=============================================================================

//=============================================================================
//                              Static Data Definition
//=============================================================================
static bool     g_inited;
static uint32_t g_vid_buff_addr[VIDEO_SURFACE_COUNT];
ITUSurface      *VideoSurf[VIDEO_SURFACE_COUNT];

//=============================================================================
//                              Private Function Declaration
//=============================================================================
#if defined(CFG_VIDEO_ENABLE) && !defined(CFG_FFMPEG_H264_SW)
static void VideoInit(void);
static void VideoExit(void);
#endif // CFG_VIDEO_ENABLE

//=============================================================================
//                              Public Function Definition
//=============================================================================
void ituFrameFuncInit(void)
{
#ifndef CFG_FFMPEG_H264_SW
    int        width;
    int        height;
    int        pitch;
    int        i;
	ITURotation     rot;
#endif
    
    if (g_inited)
        return;
        
#ifndef CFG_FFMPEG_H264_SW

    rot = itv_get_rotation();

printf("\n");
printf("===== ituFrameFuncInit [%d]\n",rot);
printf("===== ituFrameFuncInit [%d]\n",rot);
printf("===== ituFrameFuncInit [%d]\n",rot);
printf("\n");


    
    switch (rot)
    {
    case ITU_ROT_90:
    case ITU_ROT_270:
        width  = ithLcdGetHeight();
        height = ithLcdGetWidth();
        pitch  = ithLcdGetPitch() * ithLcdGetHeight() / ithLcdGetWidth();
        break;
        
    default:
	    #ifdef CFG_ITV_DYNAMIC_ROTATE //JPHong 2024-09-25
	        if(ithLcdGetWidth() >= ithLcdGetHeight())
	        {
	            width  = ithLcdGetWidth();
	            height = ithLcdGetWidth();
				pitch  = ithLcdGetWidth() * CFG_LCD_BPP;
	        }
	        else
	        {
	            width  = ithLcdGetHeight();
	            height = ithLcdGetHeight();
				pitch  = ithLcdGetHeight() * CFG_LCD_BPP;
	        }
	    #else
	        width  = ithLcdGetWidth();
	        height = ithLcdGetHeight();
	        pitch  = ithLcdGetPitch();
	    #endif
        break;
    }

    if (!g_vid_buff_addr[0])
    {        
        int size = pitch * height * VIDEO_SURFACE_COUNT;
        g_vid_buff_addr[0] = itpVmemAlloc(size);

        assert(g_vid_buff_addr[0]);

        for (i = 1; i < VIDEO_SURFACE_COUNT; ++i)
        {
            g_vid_buff_addr[i] = g_vid_buff_addr[i - 1] + pitch * height;
        }
    #ifdef _DEBUG
        for (i = 0; i < VIDEO_SURFACE_COUNT; ++i)
            printf("video buffer(%d, %X)\n", i, g_vid_buff_addr[i]);
    #endif
        //memset(g_vid_buff_addr[0], 0, size);
    }
    
#endif
  
#ifndef CFG_FFMPEG_H264_SW
    VideoInit();

    for (i = 0; i < VIDEO_SURFACE_COUNT; i++)
    {
        VideoSurf[i] = ituCreateSurface(width, height, pitch, ITU_RGB565, g_vid_buff_addr[i], ITU_STATIC);
        itv_set_vidSurf_buf(g_vid_buff_addr[i] , i);
    }

#endif

    g_inited = true;  
}

void ituFrameFuncExit(void)
{
    uint8_t i;
    
    if (!g_inited)
        return;

#ifndef CFG_FFMPEG_H264_SW
    
    VideoExit();

    for (i = 0; i < VIDEO_SURFACE_COUNT; i++)
    {
        ituDestroySurface(VideoSurf[i]);        
    }

    if (g_vid_buff_addr[0])
    {
        itpVmemFree(g_vid_buff_addr[0]);
        memset(g_vid_buff_addr, 0, sizeof(g_vid_buff_addr));
    }
#endif
    g_inited = false;
}

void ituDrawVideoSurface(ITUSurface* dest, int startX, int startY, int width, int height)
{
#ifndef CFG_FFMPEG_H264_SW
    int index;
    ITUColor color;
    
    color.alpha = 0;
    color.red = 0;
    color.green = 0;
    color.blue = 0;
#endif    
    if (!g_inited)
        return;

#ifndef CFG_FFMPEG_H264_SW
    index = itv_get_vidSurf_index();

    if (index == -1 || index == -2)
    	ituColorFill(dest, startX, startY, width, height, &color);
    else
        ituBitBlt(dest, startX, startY, width, height, VideoSurf[index], startX, startY);    
#endif
}

int ituDrawVideoSurface_ex(ITUSurface* dest, int startX, int startY, int width, int height)
{
#ifndef CFG_FFMPEG_H264_SW
    int index;
    ITUColor color;

    color.alpha = 0;
    color.red = 0;
    color.green = 0;
    color.blue = 0;
#endif    
    if (!g_inited)
        return;

#ifndef CFG_FFMPEG_H264_SW
    index = itv_get_vidSurf_index();

    if (index != -1 && index != -2)
        ituBitBlt(dest, startX, startY, width, height, VideoSurf[index], startX, startY);
    else
        return -1;
#endif
    return 0;
}

void ituDrawVideoBackground(ITUWidget* widget, ITUSurface* dest, int startX, int startY, int src_width, int src_height, ITURotation angle)
{

#ifndef CFG_FFMPEG_H264_SW
	ITURectangle prevClip;
	ITURectangle* rect = (ITURectangle*)&widget->rect;
	int x = 0;
	int y = 0;
    int index = 0;
	int src_index = 0;
    ITUColor color;
    
    color.alpha = 0;
    color.red = 0;
    color.green = 0;
    color.blue = 0;
#endif    
    if (!g_inited)
    {
        return;
    }
	
	if((angle != ITU_ROT_90) && (angle != ITU_ROT_270))
	{
		return;
	}
#ifndef CFG_FFMPEG_H264_SW

#if 0
    ituWidgetSetClipping(widget, dest, x, y, &prevClip);
#endif
    index = itv_get_vidSurf_index();

	if(index == -1 || index == -2)
	{
		src_index = 0;
	}
	else
	{
		src_index = index;
	}

	switch(angle)
	{
		case ITU_ROT_90:

			ituRotate(
				dest, (startX + src_width), startY,
				VideoSurf[src_index], 0, 0,
				(float)90.0f,
				1.0f,
				1.0f);

		break;
		case ITU_ROT_270:

			ituRotate(
				dest, startX, (startY + src_height),
				VideoSurf[src_index], 0, 0,
				(float)270.0f,
				1.0f,
				1.0f);
			
		break;
		case ITU_ROT_0:

			ituRotate(
				dest, startX, startY,
				VideoSurf[src_index], 0, 0,
				(float)0.0f,
				1.0f,
				1.0f);

		break;
		case ITU_ROT_180:

			ituRotate(
				dest, startX, startY,
				VideoSurf[src_index], 0, 0,
				(float)180.0f,
				1.0f,
				1.0f);
			
		break;
		default:
			
		break;
		
	}

    if (index == -1 || index == -2)
    {
    	ituColorFill(dest, startX, startY, rect->width, rect->height, &color);		
    }
#if 0
    ituSurfaceSetClipping(dest, prevClip.x, prevClip.y, prevClip.width, prevClip.height);
#endif

#endif
}

//=============================================================================
//                              Private Function Definition
//=============================================================================
#if defined(CFG_VIDEO_ENABLE) && !defined(CFG_FFMPEG_H264_SW)
static void
VideoInit(
    void)
{
#if (CFG_CHIP_FAMILY == 960)	
    ithVideoInit(NULL);
#endif    
    itv_init();
}

static void
VideoExit(
    void)
{
    /* release dbuf & itv */
    itv_stop_vidSurf_anchor();
    itv_flush_dbuf();
    itv_deinit();

#if (CFG_CHIP_FAMILY == 960)	
    /* release decoder stuff */
    printf("%s(%d)\n", __FUNCTION__, __LINE__);
    ithVideoExit();
    printf("%s(%d)\n", __FUNCTION__, __LINE__);
#endif    
}
#endif // CFG_VIDEO_ENABLE
