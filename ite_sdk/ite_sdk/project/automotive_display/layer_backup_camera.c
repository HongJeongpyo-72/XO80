#include <assert.h>
#include <sys/time.h>
#include <string.h>
#include "scene.h"
#include "SDL/SDL.h"
#include "castor3player.h"
#ifdef __OPENRTOS__
#include "flower/flower.h"
#endif
#include "ite/itv.h"
#include "ctrlboard.h"


extern ITUAnimation* menuBackupCamAnimation;

static ITULayer* topLayer;
static ITUContainer* backupCamContainer = 0;

static ITUAnimation* backupCamReverseLAnimation;
static ITUAnimation* backupCamReverseRAnimation;
static ITUAnimation* backupCamReverseCAnimation;
static ITUIcon* backupCamReverseIcon;
static ITUSprite* backupCamReverseSprite;
static ITUSprite* backupCamReverseSprite1;
static ITUBackground* backupCamReversingBackground;

static ITUAnimation* backupCamBtAnimation;
static ITUAnimation* backupCamTextAnimation;
static ITUAnimation* backupCamBackBtAnimation;
static ITUSprite* backupCamTextSprite[2];
static ITUSprite* backupCamBtSprite[2];

static ITURadioBox*		backupCamLoopRadioBox[2][3];
static ITUIcon*			backupCamPlaybackVideoIcon[8];
static ITUButton*		backupCamPlaybackVideoButton[8];
static ITUText*			backupCamPlaybackVideoText[8];
static ITUText*			backupCamPlaybackFileNameText;
//static ITUScrollMediaFileListBox* backupCamScrollMediaFileListBox;
static ITUButton*		backupCamMenuButton;
static ITUButton*		backupCamRecordStopButton;
static ITUContainer*	backupCamMenuContainer;
static ITUContainer*	backupCamPlaybackMenuContainer;
static ITUContainer*	backupCamPlaybackVideoContainer;
static ITUAnimation*	backupCamLoopAnimation;

static int x, y, width, height = 0;
static MTAL_SPEC backup_mtal_spec = { 0 };
static LayerControl backup = {0};
#ifdef __OPENRTOS__
static IteFlower *flower_ptr = NULL;
#endif
static VideoProcessHandle* vprocess_ptr = NULL;
static char Tfile_reversing[] = "B:/media/reverse.mkv";
extern const char VideoSubdirectory[6];
extern const char JpegSubdirectory[4];
static uint32_t backupLastTick;
static bool     isShowReversing = false;

static bool toCreateCameraflow = false;

static void JpgLoadCallback(uint8_t* data, int size)
{
    if (data && size > 0)
    {
        if (backup.ImageDataSize > 0)
        {
            free(backup.ImageData);
            backup.ImageData = NULL;
            backup.ImageDataSize = 0;
        }
        backup.ImageData = data;
        backup.ImageDataSize = size;
		backup.ImageLoaded = true;
    }
	else
	{
		//load fail
		backup.ImageLoadedCount++;
	}
}

static void VdieoFileLoadCallback(VideoProcessHandle* handle)
{
	vprocess_ptr = handle;
	backup.LoadText = true;
	if(vprocess_ptr->NOSDcard == 0)
		backup.LoadImage= true;
}

static void _LayerBackupShowReversing(int arg)
{
	ituWidgetSetVisible(backupCamReversingBackground, true);
	ituWidgetSetVisible(backupCamReverseSprite, true);
	ituSpritePlay(backupCamReverseSprite, 0);
	ituWidgetSetVisible(backupCamReverseLAnimation, true);
	ituAnimationPlay(backupCamReverseLAnimation, 0);
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
	backup.Satus  = VStatus_CAMERA;
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
                
                backupLastTick = SDL_GetTicks();
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
			backup.Satus  = VStatus_NONE;
			_LoopRecordControl(false, false);
			_DestroyCameraflow();
			break;	
		case VStatus_CAMERA:
			backup.Satus  = VStatus_NONE;
			_DestroyCameraflow();
			break;
#ifdef CFG_VIDEO_ENABLE
		case VStatus_VIDEO:
			backup.Satus  = VStatus_NONE;
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

bool BackupCamOnEnter(ITUWidget* widget, char* param)
{
	int i,j;
	char tmp[32] = {0};  
    
	if (!backupCamContainer)
	{
		backupCamContainer = ituSceneFindWidget(&theScene, "backupCamContainer");
		assert(backupCamContainer);

		backupCamReverseLAnimation = ituSceneFindWidget(&theScene, "backupCamReverseLAnimation");
		assert(backupCamReverseLAnimation);

		backupCamReverseRAnimation = ituSceneFindWidget(&theScene, "backupCamReverseRAnimation");
		assert(backupCamReverseRAnimation);

		backupCamReverseCAnimation = ituSceneFindWidget(&theScene, "backupCamReverseCAnimation");
		assert(backupCamReverseCAnimation);

		backupCamReverseIcon = ituSceneFindWidget(&theScene, "backupCamReverseIcon");
		assert(backupCamReverseIcon);

		backupCamReverseSprite = ituSceneFindWidget(&theScene, "backupCamReverseSprite");
		assert(backupCamReverseSprite);

		backupCamReverseSprite1 = ituSceneFindWidget(&theScene, "backupCamReverseSprite1");
		assert(backupCamReverseSprite1);

		backupCamReversingBackground = ituSceneFindWidget(&theScene,"backupCamReversingBackground");
		assert(backupCamReversingBackground);

		topLayer = ituSceneFindWidget(&theScene, "topLayer");
		assert(topLayer);

		backupCamBtAnimation = ituSceneFindWidget(&theScene, "backupCamBtAnimation");
		assert(backupCamBtAnimation);

		backupCamTextAnimation = ituSceneFindWidget(&theScene, "backupCamTextAnimation");
		assert(backupCamTextAnimation);

		backupCamBackBtAnimation = ituSceneFindWidget(&theScene, "backupCamBackBtAnimation");
		assert(backupCamBackBtAnimation);

		backupCamPlaybackFileNameText = ituSceneFindWidget(&theScene, "backupCamPlaybackFileNameText");
		assert(backupCamPlaybackFileNameText);

		backupCamMenuButton = ituSceneFindWidget(&theScene, "backupCamMenuButton");
		assert(backupCamMenuButton);

		backupCamRecordStopButton = ituSceneFindWidget(&theScene, "backupCamRecordStopButton");
		assert(backupCamRecordStopButton);

		backupCamMenuContainer = ituSceneFindWidget(&theScene, "backupCamMenuContainer");
		assert(backupCamMenuContainer);

		backupCamPlaybackMenuContainer = ituSceneFindWidget(&theScene, "backupCamPlaybackMenuContainer");
		assert(backupCamPlaybackMenuContainer);

		backupCamPlaybackVideoContainer = ituSceneFindWidget(&theScene, "backupCamPlaybackVideoContainer");
		assert(backupCamPlaybackVideoContainer);

		backupCamLoopAnimation = ituSceneFindWidget(&theScene, "backupCamLoopAnimation");
		assert(backupCamLoopAnimation);

		for (i = 0; i < 2; i++)
		{
			sprintf(tmp, "backupCamTextSprite%d", i);
			backupCamTextSprite[i] = ituSceneFindWidget(&theScene, tmp);
			assert(backupCamTextSprite[i]);

			sprintf(tmp, "backupCamBtSprite%d", i);
			backupCamBtSprite[i] = ituSceneFindWidget(&theScene, tmp);
			assert(backupCamBtSprite[i]);

			for (j = 0; j < 3; j++)
			{
				sprintf(tmp, "backupCamLoopRadioBox%d%d", i, j);
				backupCamLoopRadioBox[i][j] = ituSceneFindWidget(&theScene, tmp);
				assert(backupCamLoopRadioBox[i][j]);
			}
		}

		for (i = 0; i < 8; i++)
		{
			sprintf(tmp, "backupCamPlaybackVideoIcon%d", i);
			backupCamPlaybackVideoIcon[i] = ituSceneFindWidget(&theScene, tmp);
			assert(backupCamPlaybackVideoIcon[i]);

			sprintf(tmp, "backupCamPlaybackVideoButton%d", i);
			backupCamPlaybackVideoButton[i] = ituSceneFindWidget(&theScene, tmp);
			assert(backupCamPlaybackVideoButton[i]);

			sprintf(tmp, "backupCamPlaybackVideoText%d", i);
			backupCamPlaybackVideoText[i] = ituSceneFindWidget(&theScene, tmp);
			assert(backupCamPlaybackVideoText[i]);

		}
        
        //backupCamScrollMediaFileListBox = ituSceneFindWidget(&theScene, "backupCamScrollMediaFileListBox");
        //assert(backupCamScrollMediaFileListBox);


	}

	//theConfig.screensaver_type = SCREENSAVER_NONE;

	ituAnimationPlay(backupCamBtAnimation, 0);
	ituAnimationPlay(backupCamTextAnimation, 0);
	ituAnimationPlay(backupCamBackBtAnimation, 0);
	ituWidgetSetVisible(backupCamContainer, true);
	ituSpriteGoto(backupCamTextSprite[0], 0);
	ituSpriteGoto(backupCamTextSprite[1], 0);
	ituSpriteGoto(backupCamBtSprite[0], 0);
	ituSpriteGoto(backupCamBtSprite[1], 0);

	SceneEnterVideoState(33);
#ifdef CFG_VIDEO_ENABLE
	ituWidgetSetDraw(backupCamContainer, VideoBackgroundDrawCallBack);
    ituWidgetGetGlobalPosition(backupCamContainer, &x, &y);
    width = ituWidgetGetWidth(backupCamContainer);
    height = ituWidgetGetHeight(backupCamContainer);
	itv_set_video_window(x, y, width, height);
#endif
	ituWidgetSetVisible(topLayer, true);

#ifdef __OPENRTOS__
    flower_ptr = (IteFlower*)ite_new0(IteFlower,1);
#endif	
	backup.CamID      = 0;
	backup.LoadText           = false;
	backup.LoadImage          = false;
	backup.ImageLoadedCount   = 0;
	backup.Satus              = VStatus_NONE;
	backup.PageIndex          = 0;
	
	VideoFileInit();
	VideoFileMemSpaceCheckStart();
	//ituSceneExecuteCommand(&theScene, 5, _CreateCameraflow, backup.CamID); 
	toCreateCameraflow = true;
	
    return true;
}

bool BackupCamOnLeave(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);
	//theConfig.screensaver_type = screenSaverType;

	ituWidgetSetVisible(topLayer, false);

	if(isShowReversing)
	{
		ituSpriteStop(backupCamReverseSprite);
		ituSpriteStop(backupCamReverseSprite1);
		ituWidgetSetVisible(backupCamReverseSprite1, false);
		ituWidgetSetVisible(backupCamReverseLAnimation, false);
		ituWidgetSetVisible(backupCamReverseRAnimation, false);
		ituWidgetSetVisible(backupCamReverseCAnimation, false);
		ituWidgetSetVisible(backupCamReverseIcon, false);
		ituWidgetSetVisible(backupCamReversingBackground, false);
        isShowReversing = false;
	}

	ituWidgetSetVisible(backupCamContainer, false);
	ituWidgetHide(backupCamReversingBackground, ITU_EFFECT_FADE, 0);
	//ituAnimationGotoFrame(backupCamBtAnimation, 1);
	//ituAnimationGotoFrame(backupCamTextAnimation, 1);
	//ituAnimationGotoFrame(backupCamBackBtAnimation, 1);
	ituAnimationReversePlay(backupCamBtAnimation, 1);
	ituAnimationReversePlay(backupCamTextAnimation, 1);
	ituAnimationReversePlay(backupCamBackBtAnimation, 1);

	_CloseVideo(backup.Satus);
#ifdef __OPENRTOS__
	if(flower_ptr)
		free(flower_ptr);
#endif
	VideoFileMemSpaceCheckStop();
	SceneLeaveVideoState();

	return true;
}

bool BackupCamOnTimer(ITUWidget* widget, char* param)
{
	bool ret = false;
	int i =0;
	int fileindex = 0;
    uint32_t difftick, tick = SDL_GetTicks();
	char tbuf[32] = {0};
	char file_path[PATH_MAX] = {0};

	ScreenSaverRefresh();

	//signal lose error handle.
	if(backup.Satus == VStatus_RECOEDING || backup.Satus == VStatus_CAMERA)
	{
#ifdef __OPENRTOS__	
		if(flow_polling_capture_status(flower_ptr))
		{
			//close
			if(backup.Satus == VStatus_RECOEDING)
				_LoopRecordControl(false, false);
			
			_DestroyCameraflow();
			
			//restart
			_CreateCameraflow(backup.CamID);	

			if(backup.Satus == VStatus_RECOEDING)
				_LoopRecordControl(true, false);
		}
#endif
	}
	//loop reocording timer
    if(backup.Satus == VStatus_RECOEDING)
    {
        if (tick >= backupLastTick)
        {
            difftick = tick - backupLastTick;
        }
        else
        {
            difftick = 0xFFFFFFFF - backupLastTick + tick;
        }

        if(difftick > (backup.LoopRecordMode[backup.CamID] * 2 +1) * (60 * 1000) )
		{
            _LoopRecordControl(true, true);
        }
    }
    
	if(backup.LoadText)
	{
		fileindex = (backup.PageIndex * 8);
		for(i = 0; i < 8; i++)
		{
			if((i + fileindex) < vprocess_ptr->FileCount)
			{
				ituTextSetString(backupCamPlaybackVideoText[i], vprocess_ptr->VideoFileNameArray[i+fileindex]);
				ituWidgetSetVisible(backupCamPlaybackVideoText[i], true);
				ituWidgetSetVisible(backupCamPlaybackVideoIcon[i], false);
                ituWidgetSetVisible(backupCamPlaybackVideoButton[i], true);
			}
			else
			{
				ituWidgetSetVisible(backupCamPlaybackVideoText[i], false);
				ituWidgetSetVisible(backupCamPlaybackVideoIcon[i], false);
                ituWidgetSetVisible(backupCamPlaybackVideoButton[i], false);
			}
		}
		
		backup.LoadText = false;
		ret = true;
	}
    
	if(backup.LoadImage)
	{
		fileindex = (backup.PageIndex * 8);
		//load jpg  
		if(backup.ImageLoaded == false)
		{
			//printf("ImageLoadedCount = %d\n",backup.ImageLoadedCount);
			//printf("fileindex = %d\n",fileindex);
			PhotoLoad(vprocess_ptr->ThumbnailPathArray[backup.ImageLoadedCount + fileindex], JpgLoadCallback);
		}
		else
		{
			//show thumbnail
			ituIconLoadJpegData(backupCamPlaybackVideoIcon[backup.ImageLoadedCount] , backup.ImageData, backup.ImageDataSize);
			ituWidgetSetVisible(backupCamPlaybackVideoIcon[backup.ImageLoadedCount], true);
			backup.ImageLoaded = false;
			backup.ImageLoadedCount++;
		}
		//check this page finished 
		if(backup.ImageLoadedCount >= 8 || (backup.ImageLoadedCount + fileindex) >= vprocess_ptr->FileCount)
		{
			backup.ImageLoaded = false;
			backup.ImageLoadedCount = 0;			
			backup.LoadImage = false;
		}
		ret = true;
	}

	if (toCreateCameraflow && !backupCamBtAnimation->playing)
	{
		toCreateCameraflow = false;
		ituSceneExecuteCommand(&theScene, 5, _CreateCameraflow, backup.CamID);
	}


	return ret;

}
bool BackupCamReverseBtOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);

    isShowReversing = true;
	_CloseVideo(backup.Satus);

	ituWidgetSetVisible(backupCamReverseRAnimation, false);
	ituWidgetSetVisible(backupCamReverseCAnimation, false);
	ituWidgetSetVisible(backupCamReverseIcon, false);
	ituSpriteStop(backupCamReverseSprite);
	ituSpriteStop(backupCamReverseSprite1);

#ifdef CFG_VIDEO_ENABLE	
    //show reversing video
	mtal_pb_init(EventHandler);
	backup_mtal_spec.camera_in = NOT_CAMERA;
	strcpy(backup_mtal_spec.srcname, Tfile_reversing);
	mtal_pb_select_file(&backup_mtal_spec);
	mtal_pb_play();
	backup.Satus = VStatus_VIDEO;	
#endif

	ituSceneExecuteCommand(&theScene, 10, _LayerBackupShowReversing, 0);//for sync with video
	return true;
}

bool BackupCamLeftBtOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);

	if(isShowReversing)
	{
		ituWidgetSetVisible(backupCamReversingBackground, false);
        isShowReversing = false;
	}
	
	_CloseVideo(backup.Satus);

	ituWidgetSetVisible(backupCamMenuButton, true);
	ituWidgetSetVisible(backupCamRecordStopButton, false);
	ituWidgetSetVisible(backupCamMenuContainer, false);
	ituWidgetSetVisible(backupCamPlaybackMenuContainer, false);
	ituWidgetSetVisible(backupCamPlaybackVideoContainer, false);
	ituAnimationGotoFrame(backupCamLoopAnimation, 0);

	backup.CamID = 0;//left
	backup.PageIndex  = 0;
	//ituSceneExecuteCommand(&theScene, 5, _CreateCameraflow, backup.CamID);
	toCreateCameraflow = true;

	
	return true;
}

bool BackupCamRearBtOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);
	
	if(isShowReversing)
	{
		ituWidgetSetVisible(backupCamReversingBackground, false);
        isShowReversing = false;
	}
	

	_CloseVideo(backup.Satus);

	backup.CamID = 1;//Rear
	backup.PageIndex = 0;
	//ituSceneExecuteCommand(&theScene, 5, _CreateCameraflow, backup.CamID);
	toCreateCameraflow = true;

	return true;
}
bool BackupCamPlaybackViewBackBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);
	
	_CloseVideo(backup.Satus);

	VideoFileLoad(VdieoFileLoadCallback);

	return true;
}

bool BackupCamPlaybackVideoBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//show the videoIndex file 
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);

	int btn_index = atoi(param) + backup.PageIndex * 8;
#ifdef CFG_VIDEO_ENABLE
    ituTextSetString(backupCamPlaybackFileNameText, vprocess_ptr->VideoFileNameArray[btn_index]);
	mtal_pb_init(EventHandler);
	backup_mtal_spec.camera_in = NOT_CAMERA;
	strcpy(backup_mtal_spec.srcname, vprocess_ptr->VideoFilePathArray[btn_index]);
	mtal_pb_select_file(&backup_mtal_spec);
	mtal_pb_play();
#endif
	backup.Satus = VStatus_VIDEO;

	return true;
}

bool BackupCamPlaybackNextBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);
	//show next page videos. 
	if(backup.LoadImage || backup.LoadText)
		return true;
	
	if((backup.PageIndex < (PAGE_NUM_MAX - 1)) && (backup.PageIndex < vprocess_ptr->PageCount))
	{
		backup.PageIndex++;
		backup.LoadImage = backup.LoadText = true;
	}
	else
	{
		printf("the last page\n");
	}	
	
	return true;
}

bool BackupCamPlaybackPrevBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);
	//show previous page videos.
	if(backup.LoadImage || backup.LoadText)
		return true;
	
	if(backup.PageIndex > 0)
	{
		backup.PageIndex--;
		backup.LoadImage = backup.LoadText = true;
	}
	else
	{
		printf("the first page\n");
	}

	
	return true;
}

//start record
bool BackupCamLoopRadioBoxOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);
	
	char tbuf[32] = {0};
	char file_path[PATH_MAX] = {0};
	
    //0-1min,1-3min,2-5min
    backup.LoopRecordMode[backup.CamID] = atoi(param);

    if(_LoopRecordControl(true, false) == 0)
        backup.Satus = VStatus_RECOEDING;
	
	return true;
}

//stop record 
bool BackupCamRecordStopBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);

    _LoopRecordControl(false, false);

    backup.Satus = VStatus_CAMERA;

	return true;
}

bool BackupCamPlaybackMenuBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);
	//show record files .icon.text
	_CloseVideo(backup.Satus);
	
	VideoFileLoad(VdieoFileLoadCallback);

	return true;
}

bool BackupCamMenuBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);
	//show menu
	ituRadioBoxSetChecked(backupCamLoopRadioBox[0][backup.LoopRecordMode[backup.CamID]], true);
	ituRadioBoxSetChecked(backupCamLoopRadioBox[1][backup.LoopRecordMode[backup.CamID]], true);

	return true;
}

bool BackupCamPlaybackMenuBackBtnOnMouseUp(ITUWidget* widget, char* param)
{
	//printf("%s line = %d\n", __FUNCTION__, __LINE__);
	ituSceneExecuteCommand(&theScene, 5, _CreateCameraflow, backup.CamID);	
	
	return true;
}
