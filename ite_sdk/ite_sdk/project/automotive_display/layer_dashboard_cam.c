#include <assert.h>
#include <sys/time.h>
#include "scene.h"
#include "SDL/SDL.h"
#include "castor3player.h"
#ifdef __OPENRTOS__
#include "flower/flower.h"
#endif
#include "ite/itv.h"
#include "ctrlboard.h"


static ITUText* dashboardCamSpeedText;
static ITUText* dashboardCamODOText;
static ITUText* dashboardCamFuelText;
static ITUText* dashboardCamTempText;
static ITUIcon* dashboardCamFuelPointIcon;
static ITUIcon* dashboardCamTempPointIcon;

static ITUContainer*	dashboardCamViewContainer;
static ITURadioBox*		dashboardCamLoopRadioBox[2][3];
static ITUIcon*			dashboardCamPlaybackVideoIcon[8];
static ITUButton*		dashboardCamPlaybackVideoButton[8];
static ITUText*			dashboardCamPlaybackVideoText[8];
static ITUContainer*	dashboardCamPlaybackViewContainer;
static ITUText*			dashboardCamPlaybackFileNameText;
//static ITUScrollMediaFileListBox* dashboardCamScrollMediaFileListBox;
static ITUDigitalClock*	dashboardCamDigitalClock;

static  uint32_t lastTick = 0;
static  bool is_playing = false;
static  bool temp_is_add = false;
static  bool fuel_is_add = true;
static  bool speed_is_add = true;
static  int temperature = 110;
static  int fuel = 0;
static  int speed = 0;
static  int odo = 0;
static  int demo_loop = 0;
static	int x, y, width, height = 0;

static MTAL_SPEC dashcam_mtal_spec = { 0 };
static LayerControl dashcam = {0};
#ifdef __OPENRTOS__
static IteFlower *flower_ptr = NULL;
#endif
static VideoProcessHandle* vprocess_ptr = NULL;
extern const char VideoSubdirectory[6];
extern const char JpegSubdirectory[4];
static uint32_t dashcamLastTick;


static int dTemperaturePointX[86] = {
	0,3,6,10,13,16,19,22,25,29,32,35,38,41,44,48,51,54,57,60,64,67,70,73,76,79,
	83,86,89,92,95,98,102,105,108,111,114,118,121,124,127,130,133,137,140,143,146,
	149,152,156,159,162,165,168,172,175,178,181,184,187,191,194,197,200,203,206,210,213,216,219,222,226,229,
	232,235,238,241,245,248,251,254,257,260,264,267,270
};//temperature range: 25~110

static int dTemperaturePointY[86] = {
	32,30,29,27,26,25,23,22,21,19,18,17,16,15,14,13,12,11,10,9,9,8,7,6,6,5,5,4,4,3,3,2,2,2,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,3,3,4,4,5,5,6,6,7,8,9,9,10,11,12,13,14,15,16,17,18,19,21,22,23,25,26,27,29,30,32
};//temperature range: 25~110

static int dFuelPointX[101] = {
	0,3,5,8,11,14,16,19,22,24,27,30,32,35,38,41,43,46,49,51,54,57,59,62,65,68,70,73,76,78,81,84,86,89,92,95,97,
	100,103,105,108,111,113,116,119,122,124,127,130,132,135,138,140,143,146,149,151,154,157,159,162,165,167,170,
	173,176,178,181,184,186,189,192,194,197,200,203,205,208,211,213,216,219,221,224,227,230,232,235,238,240,243,
	246,248,251,254,257,259,262,265,267,270
 };

static int dFuelPointY[101] = {
	20,21,23,24,25,26,28,29,30,31,32,33,34,35,36,37,38,39,39,40,41,42,42,43,44,44,45,46,46,47,47,48,48,48,49,49,
	50,50,50,51,51,51,51,51,52,52,52,52,52,52,52,52,52,52,52,52,52,51,51,51,51,51,50,50,50,49,49,48,48,48,47,47,46,46,45,
	44,44,43,42,42,41,40,39,39,38,37,36,35,34,33,32,31,30,29,28,26,25,24,23,21,20
 };

bool _dTempDemo(int loops);
bool _dFuelDemo(int loop);
bool _dSpeedDemo(int loop);
bool _dODODemo(int loop);

static void JpgLoadCallback(uint8_t* data, int size)
{
    if (data && size > 0)
    {
        if (dashcam.ImageDataSize > 0)
        {
            free(dashcam.ImageData);
            dashcam.ImageData = NULL;
            dashcam.ImageDataSize = 0;
        }
        dashcam.ImageData = data;
        dashcam.ImageDataSize = size;
		dashcam.ImageLoaded = true;
    }
	else
	{
		//load fail
		dashcam.ImageLoadedCount++;
	}
}

static void VdieoFileLoadCallback(VideoProcessHandle* handle)
{
	vprocess_ptr = handle;
	dashcam.LoadText = true;
	if(vprocess_ptr->NOSDcard == 0)
		dashcam.LoadImage= true;
}

static void _CreateCameraflow(int arg)
{
#ifdef CFG_VIDEO_ENABLE
	itv_set_video_window(x, y, width, height);
	itv_set_pb_mode(1);
	timer_ff_init();
#endif
#ifdef __OPENRTOS__
	if(flower_ptr)
		flow_create_camera_record(flower_ptr, arg);
#endif
	dashcam.Satus  = VStatus_CAMERA;

}

static void _DestroyCameraflow(void)
{
#ifdef CFG_VIDEO_ENABLE
	itv_set_pb_mode(0);
	itv_stop_vidSurf_anchor();
	timer_ff_deinit();
#endif
#ifdef __OPENRTOS__
	if(flower_ptr)
		flow_destroy_camera_record(flower_ptr);
#endif
}

static int _LoopRecordControl(bool On, bool splitfile)
{
	char vbuf[32] = {0};
	char jbuf[32] = {0};
	char vfile_path[PATH_MAX] = {0};  
	char jfile_path[PATH_MAX] = {0}; 
#ifdef __OPENRTOS__
    if(On)
    {
     	if(flow_polling_camera_status(flower_ptr))
    	{
    		struct timeval tv;
    		struct tm* timeinfo;	
    		// time
    		gettimeofday(&tv, NULL);
    		timeinfo = localtime((const time_t*)&tv.tv_sec);
    		strftime(vbuf, sizeof(vbuf), "/%m%d_%H%M%S.avi", timeinfo);
    	    strftime(jbuf, sizeof(jbuf), "/%m%d_%H%M%S.jpg", timeinfo);
    		if(StorageGetCurrType() == STORAGE_SD)
    		{
    			strncpy(jfile_path, StorageGetDrivePath(STORAGE_SD), sizeof(jfile_path));
			    strcat(jfile_path, JpegSubdirectory);
				strlcat(jfile_path, jbuf, sizeof(jbuf));
				printf("jpeg file_path = %s \n", jfile_path);
				if(flower_ptr->videostream->Fjpegwriter)
					ite_filter_call_method(flower_ptr->videostream->Fjpegwriter, ITE_FILTER_JPEG_SNAPSHOT, (void*)jfile_path);
				
    	        strncpy(vfile_path, StorageGetDrivePath(STORAGE_SD), sizeof(vfile_path));
    	        strcat(vfile_path, VideoSubdirectory);		
    			strlcat(vfile_path, vbuf, sizeof(vbuf));
    			printf("video file_path = %s \n", vfile_path);
				
    			if(splitfile)
                    flow_start_camera_record_split(flower_ptr, vfile_path);
                else
    			    flow_start_camera_record(flower_ptr, vfile_path);
                
                dashcamLastTick = SDL_GetTicks();
                return 0;
    		}
    		else
    		{
    		    return -1;
    			printf("[%s]####No SD Storage, DO Nothing####\n", __FUNCTION__);
    		}
    	}
    	else
    	{
    	    return -1;
    		printf("[%s]####No Signal, DO Nothing####\n", __FUNCTION__);
    	}   
    }
    else
    {
        if(flower_ptr)
            flow_stop_camera_record(flower_ptr);

		return 0;
    }
#endif
}


static void _CloseVideo(int status)
{
	switch(status)
	{
		case VStatus_RECOEDING:
			dashcam.Satus  = VStatus_NONE;
			_LoopRecordControl(false, false);
			_DestroyCameraflow();
			break;
		case VStatus_CAMERA:
			dashcam.Satus = VStatus_NONE;
			_DestroyCameraflow();
			break;
#ifdef CFG_VIDEO_ENABLE
		case VStatus_VIDEO:
			dashcam.Satus = VStatus_NONE;
			mtal_pb_stop();
			mtal_pb_exit();
			break;
#endif
	}
}

#ifdef CFG_VIDEO_ENABLE
static void VideoBackgroundDrawCallBack(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
	int destx, desty;
	ITURectangle* rect = (ITURectangle*)&widget->rect;

	destx = rect->x + x;
	desty = rect->y + y;
	timer_ff_drawing();
	
	ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
}

static void EventHandler(PLAYER_EVENT nEventID, void *arg)
{
	switch (nEventID)
	{
	case PLAYER_EVENT_EOF:
		printf("File EOF ...\n");
		break;
	default:

		break;
	}
}

#endif

bool _dTempDemo(int loop)
{
	bool ret = false;
	char tmp[32];

	if (temp_is_add == true)
	{
		temperature = temperature + loop;
		if (temperature > 110)
			temperature = 110;

		if (temperature == 110)
		{
			temp_is_add = false;
		}
	}
	else
	{
		temperature = temperature - loop;
		if (temperature < 25)
			temperature = 25;

		if (temperature == 25)
		{
			temp_is_add = true;
		}
	}
	sprintf(tmp, "%d", temperature);

	ituTextSetString(dashboardCamTempText, tmp);
	ituWidgetSetPosition(dashboardCamTempPointIcon, dTemperaturePointX[temperature - 25], dTemperaturePointY[temperature - 25]);

	ret = true;
	
	return ret;
}

bool _dFuelDemo(int loop)
{
	bool ret = false;
	char tmp[32];

	if (fuel_is_add == true)
	{
		fuel = fuel + loop;
		if (fuel > 100)
			fuel = 100;

		if (fuel == 100)
		{
			fuel_is_add = false;
		}
	}
	else
	{
		fuel = fuel - loop;
		if (fuel < 0)
			fuel = 0;

		if (fuel == 0)
		{
			fuel_is_add = true;
		}
	}
	sprintf(tmp, "%d", fuel);

	ituTextSetString(dashboardCamFuelText, tmp);
	ituWidgetSetPosition(dashboardCamFuelPointIcon, dFuelPointX[fuel], dFuelPointY[fuel]);

	ret = true;
	
	return ret;
}


bool _dSpeedDemo(int loop)
{
	bool ret = false;
	char tmp[32];

	if (speed_is_add == true)
	{
		speed++;
		if (speed >= 260)
		{
			speed = 260;
			speed_is_add = false;
		}
	}
	else
	{
		speed--;
		if (speed <= 0)
		{
			speed = 0;
			speed_is_add = true;
			demo_loop++;
			if (demo_loop == 2)
			{
				is_playing = false;
				demo_loop = 0;
			}
		}
	}

	sprintf(tmp, "%d", speed);

	ituTextSetString(dashboardCamSpeedText, tmp);

	ret = true;
	
	return ret;
}

bool _dODODemo(int loop)
{
	bool ret = false;
	char tmp[16];

	if (loop % 5 == 0)
	{
		odo++;
		if (odo == 10)
		{
			odo = 0;
		}
		
		sprintf(tmp, "%d%d%d%d%d%d%d%d%d%d", odo, odo, odo, odo, odo, odo, odo, odo, odo, odo);

		ituTextSetString(dashboardCamODOText, tmp);

		ret = true;
	}


	return ret;
}

void dPlayAnimation()
{
	demo_loop = 0;
	is_playing = true;
}

bool DashboardCamOnEnter(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);
	int i = 0;
	int j = 0;
	char tmp[64];
	

	if (!dashboardCamSpeedText)
	{
		dashboardCamSpeedText = ituSceneFindWidget(&theScene, "dashboardCamSpeedText");
		assert(dashboardCamSpeedText);
		
		dashboardCamODOText = ituSceneFindWidget(&theScene, "dashboardCamODOText");
		assert(dashboardCamODOText);
		
		dashboardCamFuelText = ituSceneFindWidget(&theScene, "dashboardCamFuelText");
		assert(dashboardCamFuelText);
		
		dashboardCamTempText = ituSceneFindWidget(&theScene, "dashboardCamTempText");
		assert(dashboardCamTempText);
		
		dashboardCamFuelPointIcon = ituSceneFindWidget(&theScene, "dashboardCamFuelPointIcon");
		assert(dashboardCamFuelPointIcon);
		
		dashboardCamTempPointIcon = ituSceneFindWidget(&theScene, "dashboardCamTempPointIcon");
		assert(dashboardCamTempPointIcon);

		dashboardCamViewContainer = ituSceneFindWidget(&theScene, "dashboardCamViewContainer");
		assert(dashboardCamViewContainer);

		dashboardCamPlaybackViewContainer = ituSceneFindWidget(&theScene, "dashboardCamPlaybackViewContainer");
		assert(dashboardCamPlaybackViewContainer);

		dashboardCamPlaybackFileNameText = ituSceneFindWidget(&theScene, "dashboardCamPlaybackFileNameText");
		assert(dashboardCamPlaybackFileNameText);

		//dashboardCamScrollMediaFileListBox = ituSceneFindWidget(&theScene, "dashboardCamScrollMediaFileListBox");
		//assert(dashboardCamScrollMediaFileListBox);

		dashboardCamDigitalClock = ituSceneFindWidget(&theScene, "dashboardCamDigitalClock");
		assert(dashboardCamDigitalClock);

		for (i = 0; i < 2; i++)
		{
			for (j = 0; j < 3; j++)
			{
				sprintf(tmp, "dashboardCamLoopRadioBox%d%d", i,j);
				dashboardCamLoopRadioBox[i][j] = ituSceneFindWidget(&theScene, tmp);
				assert(dashboardCamLoopRadioBox[i][j]);
			}
		}
		for (i = 0; i < 8; i++)
		{
			sprintf(tmp, "dashboardCamPlaybackVideoIcon%d", i);
			dashboardCamPlaybackVideoIcon[i] = ituSceneFindWidget(&theScene, tmp);
			assert(dashboardCamPlaybackVideoIcon[i]);

			sprintf(tmp, "dashboardCamPlaybackVideoButton%d", i);
			dashboardCamPlaybackVideoButton[i] = ituSceneFindWidget(&theScene, tmp);
			assert(dashboardCamPlaybackVideoButton[i]);

			sprintf(tmp, "dashboardCamPlaybackVideoText%d", i);
			dashboardCamPlaybackVideoText[i] = ituSceneFindWidget(&theScene, tmp);
			assert(dashboardCamPlaybackVideoText[i]);

		}
		
	}

	ituWidgetSetVisible(dashboardCamDigitalClock, true);

	temp_is_add = false;
	temperature = 25;
	fuel_is_add = true;
	fuel = 0;
	speed_is_add = true;
	speed = 0;
	odo = 0;
	demo_loop = 0;

	ituSceneExecuteCommand(&theScene, 15, dPlayAnimation, NULL);

	SceneEnterVideoState(33);
#ifdef CFG_VIDEO_ENABLE
	ituWidgetSetDraw(dashboardCamViewContainer, VideoBackgroundDrawCallBack);
	ituWidgetGetGlobalPosition(dashboardCamViewContainer, &x, &y);
	width = ituWidgetGetWidth(dashboardCamViewContainer);
	height = ituWidgetGetHeight(dashboardCamViewContainer);
    itv_set_video_window(x, y, width, height);
#endif
#ifdef __OPENRTOS__
    flower_ptr = (IteFlower*)ite_new0(IteFlower,1);
#endif
	
	dashcam.CamID      = 0;
	dashcam.LoadText           = false;
	dashcam.LoadImage          = false;
	dashcam.ImageLoadedCount   = 0;
	dashcam.Satus              = VStatus_NONE;
	dashcam.PageIndex          = 0;
	
	VideoFileInit();
	VideoFileMemSpaceCheckStart();
	ituSceneExecuteCommand(&theScene, 5, _CreateCameraflow, dashcam.CamID); 

    return true;
}

bool DashboardCamOnTimer(ITUWidget* widget, char* param)
{


	bool ret = true;
	int i =0;
	int fileindex = 0;
	uint32_t diff = 0;
    uint32_t difftick = 0;
    uint32_t tick = SDL_GetTicks();
	static int loop_cnt = 0;
	static uint32_t pre_diff = 0;
	char tbuf[32] = {0};
	char file_path[PATH_MAX] = {0};

    // UI DEMO START
	if (tick >= lastTick)
		diff = tick - lastTick;
	else
		diff = 0xFFFFFFFF - lastTick + tick;

	if (is_playing == false)
		pre_diff = diff;
	else
	{
		
		ret = ret | _dSpeedDemo(loop_cnt);

		if ((diff - pre_diff) > 50) //50ms
		{
			int add = (diff - pre_diff) / 50;
			ret = ret | _dTempDemo(add);
			ret = ret | _dFuelDemo(add);
			pre_diff = diff;
		}

		ret = ret | _dODODemo(loop_cnt);
	}

	loop_cnt++;
	if (loop_cnt > 1000)
		loop_cnt = 0;
    // UI DEMO END

	//signal lose error handle.
	if(dashcam.Satus == VStatus_RECOEDING || dashcam.Satus == VStatus_CAMERA)
	{
#ifdef __OPENRTOS__	
		if(flow_polling_capture_status(flower_ptr))
		{
			//close
			if(dashcam.Satus == VStatus_RECOEDING)
				_LoopRecordControl(false, false);
			
			_DestroyCameraflow();
			
			//restart
			_CreateCameraflow(dashcam.CamID);	

			if(dashcam.Satus == VStatus_RECOEDING)
				_LoopRecordControl(true, false);
		}
#endif
	}

	//loop reocording timer
    if(dashcam.Satus == VStatus_RECOEDING)
    {
        if (tick >= dashcamLastTick)
        {
            difftick = tick - dashcamLastTick;
        }
        else
        {
            difftick = 0xFFFFFFFF - dashcamLastTick + tick;
        }

        if(difftick > (dashcam.LoopRecordMode[dashcam.CamID] * 2 +1) * (60 * 1000) )
		{
        	_LoopRecordControl(true, true);
        }
    }


	if(dashcam.LoadText)
	{
		fileindex = (dashcam.PageIndex * 8);
		for(i = 0; i < 8; i++)
		{
			if((i + fileindex) < vprocess_ptr->FileCount)
			{
				ituTextSetString(dashboardCamPlaybackVideoText[i], vprocess_ptr->VideoFileNameArray[i+fileindex]);
				ituWidgetSetVisible(dashboardCamPlaybackVideoText[i], true);
				ituWidgetSetVisible(dashboardCamPlaybackVideoIcon[i], false);
                ituWidgetSetVisible(dashboardCamPlaybackVideoButton[i], true);
			}
			else
			{
				ituWidgetSetVisible(dashboardCamPlaybackVideoText[i], false);
				ituWidgetSetVisible(dashboardCamPlaybackVideoIcon[i], false);
                ituWidgetSetVisible(dashboardCamPlaybackVideoButton[i], false);
			}
		}
		
		dashcam.LoadText = false;
		ret = true;
	}
    
	if(dashcam.LoadImage)
	{
		fileindex = (dashcam.PageIndex * 8);
		//load jpg  
		if(dashcam.ImageLoaded == false)
		{
			//printf("ImageLoadedCount = %d\n",dashcam.ImageLoadedCount);
			//printf("fileindex = %d\n",fileindex);
			PhotoLoad(vprocess_ptr->ThumbnailPathArray[dashcam.ImageLoadedCount + fileindex], JpgLoadCallback);
		}
		else
		{
			//show thumbnail
			ituIconLoadJpegData(dashboardCamPlaybackVideoIcon[dashcam.ImageLoadedCount] , dashcam.ImageData, dashcam.ImageDataSize);
			ituWidgetSetVisible(dashboardCamPlaybackVideoIcon[dashcam.ImageLoadedCount], true);
			dashcam.ImageLoaded = false;
			dashcam.ImageLoadedCount++;
		}
		//check this page finished 
		if(dashcam.ImageLoadedCount >= 8 || (dashcam.ImageLoadedCount + fileindex) >= vprocess_ptr->FileCount)
		{
			dashcam.ImageLoaded = false;
			dashcam.ImageLoadedCount = 0;			
			dashcam.LoadImage = false;
		}
		ret = true;
	}


	return ret;
}

bool DashboardCamOnLeave(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);

	lastTick = 0;
	is_playing = false;
    
	_CloseVideo(dashcam.Satus);
#ifdef __OPENRTOS__
	if(flower_ptr)
		free(flower_ptr);
#endif
	VideoFileMemSpaceCheckStop();
	SceneLeaveVideoState();

	return true;
}

bool DashboardCamPlayBtOnMouseUp(ITUWidget* widget, char* param)
{
	printf("%s\n",__func__);
	if (is_playing == false)
	{
		demo_loop = 0;
		is_playing = true;
	}
	//_CloseVideo(dashcam.Satus);

	//dashcam.CamID = 0;
	//dashcam.PageIndex = 0;
	//ituSceneExecuteCommand(&theScene, 5, _CreateCameraflow, dashcam.CamID);
	

	return true;
}

bool DashboardCamPlaybackViewBackBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);

	ituWidgetSetVisible(dashboardCamDigitalClock, true);
	_CloseVideo(dashcam.Satus);

	VideoFileLoad(VdieoFileLoadCallback);

	return true;
}

bool DashbaordCamPlaybackVideoBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//show the videoIndex file to dashboardCamPlaybackViewContainer
	ituWidgetSetVisible(dashboardCamDigitalClock, false);
    int btn_index = atoi(param)  + dashcam.PageIndex * 8;
#ifdef CFG_VIDEO_ENABLE
    ituTextSetString(dashboardCamPlaybackFileNameText, vprocess_ptr->VideoFileNameArray[btn_index]);
    mtal_pb_init(EventHandler);
    dashcam_mtal_spec.camera_in = NOT_CAMERA;
    strcpy(dashcam_mtal_spec.srcname, vprocess_ptr->VideoFilePathArray[btn_index]);
    mtal_pb_select_file(&dashcam_mtal_spec);
    mtal_pb_play();
#endif
    dashcam.Satus = VStatus_VIDEO;


	return true;
}

bool DashboardCamPlaybackNextBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//show next page videos. 
	if(dashcam.LoadImage || dashcam.LoadText)
		return true;

	if((dashcam.PageIndex < (PAGE_NUM_MAX - 1)) && (dashcam.PageIndex < vprocess_ptr->PageCount))
	{
		dashcam.PageIndex++;
		dashcam.LoadImage = dashcam.LoadText = true;
	}
	else
	{
		printf("the last page\n");
	}	

	return true;
}

bool DashboardCamPlaybackPrevBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//show previous page videos.
	if(dashcam.LoadImage || dashcam.LoadText)
		return true;
	
	if(dashcam.PageIndex > 0)
	{
		dashcam.PageIndex--;
		dashcam.LoadImage = dashcam.LoadText = true;
	}
	else
	{
		printf("the first page\n");
	}

	return true;
}

bool DashboardCamLoopRadioBoxOnMouseUp(ITUWidget* widget, char* param)
{
	//start record
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);
	
    //0-1min,1-3min,2-5min
    dashcam.LoopRecordMode[dashcam.CamID] = atoi(param);
    if(_LoopRecordControl(true, false) == 0)
        dashcam.Satus = VStatus_RECOEDING;

	return true;
}

bool DashboardCamPlaybackMenuBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);

	//show record files .icon.text
	_CloseVideo(dashcam.Satus);

	VideoFileLoad(VdieoFileLoadCallback);

	return true;
}

bool DashboardCamRecordStopBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);

    _LoopRecordControl(false, false);
    dashcam.Satus = VStatus_CAMERA;

	//stop record
	return true;
}

bool DashboardCamMenuBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);
	//show menu
	ituRadioBoxSetChecked(dashboardCamLoopRadioBox[0][dashcam.LoopRecordMode[dashcam.CamID]], true);
	ituRadioBoxSetChecked(dashboardCamLoopRadioBox[1][dashcam.LoopRecordMode[dashcam.CamID]], true);

	return true;
}

bool DashboardCamPlaybackMenuBackBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);
	ituSceneExecuteCommand(&theScene, 5, _CreateCameraflow, dashcam.CamID);	

	return true;
}
