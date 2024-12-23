#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "ite/ith.h"
#include "ite/itp.h"
#include "ite/itv.h"
#include "ith/ith_video.h"
#include "ite/itu.h"
#include "castor3player.h"

#define               FrameRate_30    33
#define               FrameRate_60    16
#define               MAX_FPS         FrameRate_30

pthread_t             event_task;
pthread_t             display_task;
MTAL_SPEC 			  mtal_spec = {0};
bool                  capture_error = false;
bool                  stopTest = false;
char                  filepath0[5] = {'C','H','0'}; //ch0 {'C','H','0'} ch1 {'C','H','1'} ch2 {'C','H','2'}


//=====================================

void EventHandler(PLAYER_EVENT nEventID, void *arg)
{
    switch (nEventID)
    {
    case PLAYER_EVENT_CAPTURE_DEV:
        printf("Capture Error\n");
		capture_error = true;
        break;

    default:
        break;
    }
}

static void* DrawVideoSurface(void* arg)
{
	ITUSurface* lcdSurf;
    lcdSurf = ituGetDisplaySurface();

	while(!stopTest)
    {
		ituDrawVideoSurface(lcdSurf, 0, 0, ithLcdGetWidth(), ithLcdGetHeight());
        ituFlip(lcdSurf);
        usleep(MAX_FPS*1000);
    }
}

void PlayVideo()
{
	/*call sdk/share/ffmpeg/capdev_player.c*/
#ifdef CFG_LCD_ENABLE	
    mtal_pb_init(EventHandler);
    itv_set_video_window(0,0, ithLcdGetWidth(), ithLcdGetHeight());
	mtal_spec.camera_in = CAPTURE_IN;//select capdev_player
	strcpy(mtal_spec.srcname, filepath0);//select input ch 0
    mtal_pb_select_file(&mtal_spec);
    mtal_pb_play();
	stopTest = false;
	pthread_create(&display_task, NULL, DrawVideoSurface, NULL);
#endif
}

void PlayExit()
{
#ifdef CFG_LCD_ENABLE	
	stopTest = true;
	mtal_pb_stop();
	mtal_pb_exit();
	pthread_join(display_task, NULL);
#endif
}

static void *_EventThread()
{
	while(1){
		
		printf("*********************************\n");
		printf("*  iTE MediaPlayer				*\n");
		printf("*********************************\n\n");
		
#ifdef CFG_VIDEO_ENABLE
		ituFrameFuncInit();
#endif // CFG_VIDEO_ENABLE

		PlayVideo();
		for(;;){
			if(capture_error)break;
			sleep(1);
		}
		PlayExit();
		
#ifdef CFG_VIDEO_ENABLE
		ituFrameFuncExit();
#endif // CFG_VIDEO_ENABLE	
		capture_error = false;

		printf("*********************************\n");
		printf("*  iTE MediaPlayer  End         *\n");
		printf("*********************************\n\n");	
		sleep(1);
	}
}

//Modules test Run(Sensor) peripheral->sensor enable.
void *TestFunc(void *arg)
{
    printf("modules test cap start\n");   
    itpInit();
	
#ifdef CFG_LCD_ENABLE
	/*lcd panel int*/
	ioctl(ITP_DEVICE_SCREEN,	ITP_IOCTL_POST_RESET, NULL);
#ifdef CFG_BACKLIGHT_ENABLE
	ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_RESET,	  NULL);
#endif

	ituLcdInit();

#ifdef CFG_M2D_ENABLE
	ituM2dInit();
#else
	ituSWInit();
#endif // CFG_M2D_ENABLE
#endif

    pthread_create(&event_task, NULL, _EventThread, NULL);

}
