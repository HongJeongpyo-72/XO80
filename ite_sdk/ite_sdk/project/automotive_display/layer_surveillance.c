#include <assert.h>
#include <sys/time.h>
#include "scene.h"
#include "SDL/SDL.h"
#include "castor3player.h"
#ifdef __OPENRTOS__
#include "flower/flower.h"
#endif
#include "ite/itv.h"
#include "ite/itu.h"


static ITULayer* topLayer;
static ITUContainer* surveillanceContainer = 0;

static ITUAnimation* surveillanceReverseLAnimation;
static ITUAnimation* surveillanceReverseRAnimation;
static ITUAnimation* surveillanceReverseCAnimation;
static ITUIcon* surveillanceReverseIcon;
static ITUSprite* surveillanceReverseSprite;
static ITUSprite* surveillanceReverseSprite1;
static ITUBackground* surveillanceReversingBackground;

static ITUAnimation* surveillanceBtAnimation;
static ITUAnimation* surveillanceTextAnimation;
static ITUAnimation* surveillanceBackBtAnimation;
static ITUSprite* surveillanceTextSprite[2];
static ITUSprite* surveillanceBtSprite[2];

static int x, y, width, height = 0;
static MTAL_SPEC surveillance_mtal_spec = { 0 };
static char Tfile_quad[] = "B:/media/Quad split.mp4";
static char Tfile_reversing[] = "B:/media/reverse.mkv";

static bool isVideoLoop = false; // video loop play status
static int  camera_id = 0;  //0: file_Quad, 1:rear, 2: revresing
static bool isPlaying = false; // camera 1 playing or not
#ifdef __OPENRTOS__
static IteFlower *flower_ptr = NULL;
#endif

static bool toShowCamera = false;

static void _ShowReversing(int arg)
{
	ituWidgetSetVisible(surveillanceReversingBackground, true);
	ituWidgetSetVisible(surveillanceReverseSprite, true);
	ituSpritePlay(surveillanceReverseSprite, 0);
	ituWidgetSetVisible(surveillanceReverseLAnimation, true);
	ituAnimationPlay(surveillanceReverseLAnimation, 0);
}

static void _ShowCamera(int arg)
{
#ifdef CFG_VIDEO_ENABLE
	itv_set_video_window(x, y, width, height);
	itv_set_pb_mode(1);
#endif
#ifdef __OPENRTOS__
    flow_start_camera_show(flower_ptr, arg);
#endif
	isPlaying = true; 
}


#ifdef CFG_VIDEO_ENABLE
static void PreviewVideoBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
	int destx, desty;
	ITURectangle* rect = (ITURectangle*)&widget->rect;

	destx = rect->x + x;
	desty = rect->y + y;

	ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
}

static void EventHandler(PLAYER_EVENT nEventID, void *arg)
{
	switch (nEventID)
	{
	case PLAYER_EVENT_EOF:
		printf("File EOF ...\n");
		if(isVideoLoop)
			mtal_pb_seekto(0);
		break;
	default:
		break;
	}
}

#endif

bool SurveillanceOnEnter(ITUWidget* widget, char* param)
{
	int i;
	char tmp[32];

	if (!surveillanceContainer)
	{
		surveillanceContainer = ituSceneFindWidget(&theScene, "surveillanceContainer");
		assert(surveillanceContainer);

		surveillanceReverseLAnimation = ituSceneFindWidget(&theScene, "surveillanceReverseLAnimation");
		assert(surveillanceReverseLAnimation);

		surveillanceReverseRAnimation = ituSceneFindWidget(&theScene, "surveillanceReverseRAnimation");
		assert(surveillanceReverseRAnimation);

		surveillanceReverseCAnimation = ituSceneFindWidget(&theScene, "surveillanceReverseCAnimation");
		assert(surveillanceReverseCAnimation);

		surveillanceReverseIcon = ituSceneFindWidget(&theScene, "surveillanceReverseIcon");
		assert(surveillanceReverseIcon);

		surveillanceReverseSprite = ituSceneFindWidget(&theScene, "surveillanceReverseSprite");
		assert(surveillanceReverseSprite);

		surveillanceReverseSprite1 = ituSceneFindWidget(&theScene, "surveillanceReverseSprite1");
		assert(surveillanceReverseSprite1);

		surveillanceReversingBackground = ituSceneFindWidget(&theScene, "surveillanceReversingBackground");
		assert(surveillanceReversingBackground);

		topLayer = ituSceneFindWidget(&theScene, "topLayer");
		assert(topLayer);

		surveillanceBtAnimation = ituSceneFindWidget(&theScene, "surveillanceBtAnimation");
		assert(surveillanceBtAnimation);

		surveillanceTextAnimation = ituSceneFindWidget(&theScene, "surveillanceTextAnimation");
		assert(surveillanceTextAnimation);

		surveillanceBackBtAnimation = ituSceneFindWidget(&theScene, "surveillanceBackBtAnimation");
		assert(surveillanceBackBtAnimation);

		for (i = 0; i < 2; i++)
		{
			sprintf(tmp, "surveillanceTextSprite%d", i);
			surveillanceTextSprite[i] = ituSceneFindWidget(&theScene, tmp);
			assert(surveillanceTextSprite[i]);

			sprintf(tmp, "surveillanceBtSprite%d", i);
			surveillanceBtSprite[i] = ituSceneFindWidget(&theScene, tmp);
			assert(surveillanceBtSprite[i]);
		}

		


	}
	//theConfig.screensaver_type = SCREENSAVER_NONE;

	ituAnimationPlay(surveillanceBtAnimation, 0);
	ituAnimationPlay(surveillanceTextAnimation, 0);
	ituAnimationPlay(surveillanceBackBtAnimation, 0);
	ituWidgetSetVisible(surveillanceContainer, true);
	ituSpriteGoto(surveillanceTextSprite[0], 0);
	ituSpriteGoto(surveillanceTextSprite[1], 0);
	ituSpriteGoto(surveillanceBtSprite[0], 0);
	ituSpriteGoto(surveillanceBtSprite[1], 0);

	SceneEnterVideoState(33);

    ituWidgetGetGlobalPosition(surveillanceContainer, &x, &y);
    width = ituWidgetGetWidth(surveillanceContainer);
    height = ituWidgetGetHeight(surveillanceContainer);
#ifdef CFG_VIDEO_ENABLE
	itv_set_video_window(x, y, width, height);


	memset(&surveillance_mtal_spec, 0, sizeof(MTAL_SPEC));
	ituWidgetSetDraw(surveillanceContainer, PreviewVideoBackgroundDraw);
    
	mtal_pb_init(EventHandler);
	strcpy(surveillance_mtal_spec.srcname, Tfile_quad);
	printf("mtal_pb_select_file(%s)\n", Tfile_quad);
	mtal_pb_select_file(&surveillance_mtal_spec);
	mtal_pb_play_videoloop();
#endif
#ifdef __OPENRTOS__
    flower_ptr = (IteFlower*)ite_new0(IteFlower,1);
#endif
	camera_id = 0;//Quad split
	isVideoLoop = true;
	isPlaying = false;
	ituWidgetSetVisible(topLayer, true);
    return true;
}

bool SurveillanceOnLeave(ITUWidget* widget, char* param)
{
	//theConfig.screensaver_type = screenSaverType;

	ituWidgetSetVisible(topLayer, false);
	if (camera_id == 2)
	{
		ituSpriteStop(surveillanceReverseSprite);
		ituSpriteStop(surveillanceReverseSprite1);
		ituWidgetSetVisible(surveillanceReverseSprite1, false);
		ituWidgetSetVisible(surveillanceReverseLAnimation, false);
		ituWidgetSetVisible(surveillanceReverseRAnimation, false);
		ituWidgetSetVisible(surveillanceReverseCAnimation, false);
		ituWidgetSetVisible(surveillanceReverseIcon, false);
		ituWidgetSetVisible(surveillanceReversingBackground, false);
	}

	ituWidgetSetVisible(surveillanceContainer, false);
	ituWidgetHide(surveillanceReversingBackground, ITU_EFFECT_FADE, 0);
	//ituAnimationGotoFrame(surveillanceBtAnimation, 1);
	//ituAnimationGotoFrame(surveillanceTextAnimation, 1);
	//ituAnimationGotoFrame(surveillanceBackBtAnimation, 1);
	ituAnimationReversePlay(surveillanceBtAnimation, 1);
	ituAnimationReversePlay(surveillanceTextAnimation, 1);
	ituAnimationReversePlay(surveillanceBackBtAnimation, 1);

#ifdef CFG_VIDEO_ENABLE
    if(camera_id == 1)
    {
        flow_stop_camera_show(flower_ptr);
        itv_set_pb_mode(0); 	
    }
    else
    {
    	mtal_pb_stop();
    	mtal_pb_exit();
		if(isVideoLoop)
        	isVideoLoop = false;

    }

#endif
#ifdef __OPENRTOS__
	if(flower_ptr)
		free(flower_ptr);
#endif
	SceneLeaveVideoState();

	return true;
}

bool SurveillanceOnTimer(ITUWidget* widget, char* param)
{
	bool ret = false;
	ScreenSaverRefresh();
#ifdef __OPENRTOS__
	if(isPlaying)
	{
		if(flow_polling_capture_status(flower_ptr))//error handle
		{
			//stop
			itv_set_pb_mode(0);
			itv_stop_vidSurf_anchor();
		    flow_stop_camera_show(flower_ptr);
			//replay
			itv_set_video_window(x, y, width, height);
			itv_set_pb_mode(1);	
		    flow_start_camera_show(flower_ptr, camera_id);		
		}
	}
#endif

	if (toShowCamera && (!surveillanceBtAnimation->playing))
	{
		toShowCamera = false;
		ituSceneExecuteCommand(&theScene, 5, _ShowCamera, 1);
	}
	return ret;
}

bool SurveillanceReverseBtOnMouseUp(ITUWidget* widget, char* param)
{
	camera_id = 2;//reversing
	
	if(isPlaying)
	{
		isPlaying = false; 
#ifdef CFG_VIDEO_ENABLE
		itv_set_pb_mode(0);
		itv_stop_vidSurf_anchor();
#endif
#ifdef __OPENRTOS__
		flow_stop_camera_show(flower_ptr);
#endif
	}

	ituWidgetSetVisible(surveillanceReverseRAnimation, false);
	ituWidgetSetVisible(surveillanceReverseCAnimation, false);
	ituWidgetSetVisible(surveillanceReverseIcon, false);
	ituSpriteStop(surveillanceReverseSprite);
	ituSpriteStop(surveillanceReverseSprite1);
#ifdef CFG_VIDEO_ENABLE	
    //show reversing video
	mtal_pb_init(EventHandler);
	surveillance_mtal_spec.camera_in = NOT_CAMERA;
	strcpy(surveillance_mtal_spec.srcname, Tfile_reversing);
	mtal_pb_select_file(&surveillance_mtal_spec);
	mtal_pb_play();
#endif
	ituSceneExecuteCommand(&theScene, 10, _ShowReversing, 0);//for sync with video

	return true;
}

bool SurveillanceQuadBtOnMouseUp(ITUWidget* widget, char* param)
{
	if (ituWidgetIsVisible(surveillanceReversingBackground))
	{
		ituWidgetSetVisible(surveillanceReversingBackground, false);
	}

    if(camera_id == 1)
    {
		isPlaying = false;
#ifdef CFG_VIDEO_ENABLE
		itv_set_pb_mode(0);
		itv_stop_vidSurf_anchor();
#endif
#ifdef __OPENRTOS__
		flow_stop_camera_show(flower_ptr); 
#endif
    }
	else if(camera_id == 2)
    {
#ifdef CFG_VIDEO_ENABLE
        mtal_pb_stop();
        mtal_pb_exit();   
#endif
    }
	
#ifdef CFG_VIDEO_ENABLE
    if(isVideoLoop == false)
    {
        camera_id = 0;
    	mtal_pb_init(EventHandler);
    	surveillance_mtal_spec.camera_in = NOT_CAMERA;
    	strcpy(surveillance_mtal_spec.srcname, Tfile_quad);
    	//printf("mtal_pb_select_file(%s)\n", Tfile_quad);
    	mtal_pb_select_file(&surveillance_mtal_spec);
    	mtal_pb_play_videoloop();
        isVideoLoop = true;
    }
#endif
	
	return true;
}

bool SurveillanceRearBtOnMouseUp(ITUWidget* widget, char* param)
{

#ifdef CFG_VIDEO_ENABLE
    if(isVideoLoop)
    {
        mtal_pb_stop();
        mtal_pb_exit();
        isVideoLoop = false;
    }
#endif
    camera_id = 1;
	toShowCamera= true;
    //ituSceneExecuteCommand(&theScene, 5, _ShowCamera, 1);// arg = Set camera source ch 1
	return true;
}
