#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"
#include "ite/itu.h"
#include "leaf/leaf_mediastream.h"
#include "castor3player.h"


#include "HT_DispDdefine.h"
#include "layer_main_FW.h"

#define CAMERA_NUM 		2

//  현상태 Rear  완벽  (OFF)  E:\Project\IT978\SW\ite_sdk\ite_sdk\sdk\include\sensor\TP2825.h  수정해 한다 
//  #define USED_SWAP	
// USED_SWAP:OFF, #define USED_SVM_REAR : ON Rear : DEMO_978_DUALCAP_XO-23-06-04.txt
					
					
// DEMO_978_DUALCAP_XO-23-06-04.txt  사용  





	// 	: Rear camera(gCameraStatus0)	,CameraOnEnter : 4 ,1,4 , 1,3  Main_Disp_Mode:4   완벽  
	// DEMO_978_DUALCAP_XO-23-06-04.txt		 	: Side camera(gCameraStatus1)   ,CameraOnEnter : 4 ,1,4 , 0,2  Main_Disp_Mode:4
	
	// DEMO_978_DUALCAP_XO-23-06-04_SWAP.txt 	==> Rear:gCameraStatus0, 	Side:gCameraStatus1
	// DEMO_978_DUALCAP_XO-23-06-04.txt			==> Rear:gCameraStatus0, 	Side:gCameraStatus1
					
//  0.73	934.4	525.6
//  0.725	928		522

#define AVM_REAL_SIZE	660			//  620
#define AVM_REAL_SIZE2	412			// 1280:800=620:x => 800x620/1280 = 387
									// 1280:800=660:x => 800x660/1280 = 412

static ITUIcon* Cam5_BCam;
static ITUIcon* Cam5_RCam;
static ITUIcon* Cam5_LCam;

static ITUBackground* Cam5_2B_Black = NULL;

static ITUContainer* backupCamContainer = NULL;
//static ITUBackground* backupCamBackground = NULL;

#ifdef TEST_CAM_COLOR
	static ITUBackground*  Cam_Color_Con = NULL;
	static ITUButton* Cam_Color_Bt[13];
	static ITUIcon* Cam_Color_Icon[7];
	static ITUText* Cam_Color_Text = NULL;
	static ITUIcon* Cam_Color_TxIcon = NULL;
	
	static uint8_t CamColorMode = 1;			// 0:Off , 1:On
	static uint8_t CamColorDayNight = 0;		// 0:Day , 1:Night
	static uint8_t CamColorIndex = 0;			// 0:Brightness , 1:Contrast , 2:Saturation , 3:Hue , 4:Sharpness;
	
	
	int8_t 	CC1_Brightness;
	uint8_t CC1_Contrast;
	uint8_t CC1_Saturation;
	int8_t 	CC1_Hue;
	uint8_t CC1_Sharpness;
	int8_t 	CC2_Brightness;
	uint8_t CC2_Contrast;
	uint8_t CC2_Saturation;
	int8_t 	CC2_Hue;
	uint8_t CC2_Sharpness;

	uint8_t CamColorLongKeyMode = 0;	// 1:up,2:down
	uint32_t CamColorLongKeyCnt = 0;
	
	bool CamColorDown(ITUWidget* widget, char* param);
	bool CamColorUp(ITUWidget* widget, char* param);
	
	bool Cam_Color_Icon_Disp(uint8_t SIndex,uint8_t SDayNight,DISP_MODE DispMode);
	bool Cam_Color_Value_Disp(uint8_t SIndex,uint8_t ItemValueD,uint8_t ItemValueN,DISP_MODE DispMode);
	bool Cam_Color_Con_Disp(DISP_MODE DispMode);
	bool CamColorNight(ITUWidget* widget, char* param);
	bool CamColorDay(ITUWidget* widget, char* param);
#endif

int camera1playing = 0;	// 0:off , 1:error , 2:init
uint8_t CamChannel = 0;
uint8_t CamMode = 5;

static bool LayerFlag = 0;
static char widgetName[30];
static uint8_t KeyDepth = 0;

static int ZoomMode =6;
uint8_t sendstr1[9] = {0x02, 0x10, 0xff, 0x80, 0x00, 0x00, 0x00, 0x06, 0x03};
static int CamZoom =0;
static int32_t Cam_tSymbol=0xFF;
static int32_t Cam_TempRpm = LONG_KEY_TIME;
static uint32_t Cam_EI_34B = 0xFFFF;
static uint32_t Cam_DD_1B = 0xFFFF;

bool CameraOnEnter(void);
extern bool MainLayer_ButtonSel(uint8_t SelIndex,uint8_t OnOff);
void CameraScreenDisp(uint8_t tCamMode);

uint8_t StartCameraStatus=0;
uint32_t StartCameraStatusCnt=0;


static int dst_x, dst_y, dst_width, dst_height = 0;
static int src_width, src_height = 0;

static MTAL_SPEC mtal_spec1 = { 0 };

static char SourcePath0[5] = { 'C', 'H', '0' };
static char SourcePath1[5] = { 'C', 'H', '1' };
static bool CaptureError = false;



bool CameraSetColor(uint8_t DayNight,bool ReTry);
bool InitCamera(void);

void ResetCamera(void)
{
	LayerFlag=0;
}
void GotoCamera(uint8_t CCdata)		// 0:off , 1:BS , 2:B , 3:S , 4: AB , 5:AR , 6:AL , 7:BR , 8:3c
{

printf("GotoCamera : CCdata:%d  Main_Disp_Mode:%d  %d\n\n",CCdata,Main_Disp_Mode,gCameraModeDisp);

	if (Main_Disp_Mode_Flag==1)
	{
		printf("\n[GotoCamera]Main_Disp_Mode_Flag===> : %d %d %d\n",Main_Disp_Mode,MainBtFlag,RotationMode);
		printf("[GotoCamera]Main_Disp_Mode_Flag===> : %d %d %d\n",Main_Disp_Mode,MainBtFlag,RotationMode);
		printf("[GotoCamera]Main_Disp_Mode_Flag===> : %d %d %d\n",Main_Disp_Mode,MainBtFlag,RotationMode);
		printf("[GotoCamera]Main_Disp_Mode_Flag===> : %d %d %d\n",Main_Disp_Mode,MainBtFlag,RotationMode);
		printf("[GotoCamera]Main_Disp_Mode_Flag===> : %d %d %d\n\n",Main_Disp_Mode,MainBtFlag,RotationMode);
	}
	Main_Disp_Mode_Flag=1;

    CamMode = CCdata;
    
	CamChannel = 3;
	
	Cam_tSymbol = 0xFF;
	Cam_TempRpm = LONG_KEY_TIME;
	Cam_EI_34B = 0xFFFF;
	Cam_DD_1B = 0xFFFF;
	TempCPI_0Byte = 0xFF;

	{
		/*
		Brightness 	: 0 	: -64 ~63 	(Reset : 0x00)
		Contrast 	: 100 	: 0-127 	(Reset : 0x40)
		Saturation 	: 64 	: 0-127 	(Reset : 0x40)
		Hue 		: -10 	: -90~90 	(Reset : 0x00)
		Sharpness 	: 0 	: 0~31 		(Reset : 0x00)
		*/
		#if 0
		CC1.Brightness	= DEFAULT_CAM_BR;
		CC1.Contrast	= DEFAULT_CAM_CR;
		CC1.Saturation	= DEFAULT_CAM_SR;
		CC1.Hue			= DEFAULT_CAM_HUE;
		CC1.Sharpness	= DEFAULT_CAM_SP;
			
		CC2.Brightness	= DEFAULT_CAM_BR_N;
		CC2.Contrast	= DEFAULT_CAM_CR_N;
		CC2.Saturation	= DEFAULT_CAM_SR_N;
		CC2.Hue			= DEFAULT_CAM_HUE_N;
		CC2.Sharpness	= DEFAULT_CAM_SP_N;
		#endif
		#ifdef TEST_CAM_COLOR
			CC1_Brightness 	= CC1.Brightness;
			CC1_Contrast 	= CC1.Contrast;
			CC1_Saturation 	= CC1.Saturation;
			CC1_Hue 		= CC1.Hue;
			CC1_Sharpness 	= CC1.Sharpness;
			CC2_Brightness 	= CC2.Brightness;
			CC2_Contrast 	= CC2.Contrast;
			CC2_Saturation 	= CC2.Saturation;
			CC2_Hue 		= CC2.Hue;
			CC2_Sharpness 	= CC2.Sharpness;
			CamColorDayNight = Main_DayNight;
		#endif
		
		CameraSetColor(Main_DayNight,1);
	}
	
	StartCameraStatus = gCameraStatus0;
	StartCameraStatusCnt = 0;

	InitCamera( );
}

#if (CFG_VIDEO_ENABLE)
static void PreviewVideoBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
	int destx, desty;
	ITURectangle* rect = (ITURectangle*)&widget->rect;
    //printf("===== PreviewVideoBackgroundDraw\n");
	destx = rect->x + x;
	desty = rect->y + y;

#if 0
	ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);                   
#else
	if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
	{
		ituDrawVideoBackground(widget, dest, destx, desty, src_width, src_height, ITU_ROT_270);
	}
	else
	{
		ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
	}
#endif	
}

static void EventHandler(PLAYER_EVENT nEventID, void *arg)
{
	switch (nEventID)
	{
	case PLAYER_EVENT_EOF:
		printf("File EOF ...\n");
		break;
	case PLAYER_EVENT_CAPTURE_DEV:
		printf("CAPTURE ERROR ...\n");
		CaptureError = true;
		break;
	default:

		break;
	}
}
#endif


uint8_t CameraCheckCAN(void)
{
	if (TempCPI_0Byte!=CPI_0Byte)
	{
		printf("CameraCheckCAN Start==> gCameraModeDisp:[%d],CPI_0Byte:[%d]\n",gCameraModeDisp,CPI_0Byte);	
		switch(CPI_0Byte)//	CPI_0Byte			//0:off , 1,5: AB , 2,6:AR , 3,7,:AL , 14,15,:BR , 16,17:3c
		{	
			case 0:
			{
				gCameraModeDisp = 0; 	// 0:off , 1:BS , 2:B , 3:S , 4: AB , 5:AR , 6:AL , 7:BR , 8:3c			
			}
			break;	
			case 1:
			case 5:
			{
				gCameraModeDisp = 4; 	// 0:off , 1:BS , 2:B , 3:S , 4: AB , 5:AR , 6:AL , 7:BR , 8:3c
			}
			break;
			case 14:
			case 15:
			{
				gCameraModeDisp = 7; 	// 0:off , 1:BS , 2:B , 3:S , 4: AB , 5:AR , 6:AL , 7:BR , 8:3c
			}
			break;
			default: 
			{			
				gCameraModeDisp = 4;
			}
			break;
		}
		TempCPI_0Byte=CPI_0Byte;
		CamMode = gCameraModeDisp;
		printf("CameraCheckCAN End==> gCameraModeDisp:[%d],CPI_0Byte:[%d]\n",gCameraModeDisp,CPI_0Byte);
	}
	return true;
}



#define CAM_ICON_1_V_X		609
#define CAM_ICON_1_V_Y		160
#define CAM_ICON_2_V_X		CAM_ICON_1_V_X
#define CAM_ICON_2_V_Y		674

#define CAM_ICON_1_H_X		403
#define CAM_ICON_1_H_Y		163
#define CAM_ICON_2_H_X		864
#define CAM_ICON_2_H_Y		CAM_ICON_1_H_Y


bool CameraIconDisp(uint8_t DMode,DISP_MODE DispMode,bool CallCam)
{
	static uint8_t tDMode = 0xFF;
	char JD_Buf[10] = {0,};

	//if ((DispMode==DISP_ON)&&(DMode!=tDMode))
	if (DispMode==DISP_ON)
	{
		ituWidgetSetVisible(Cam5_LCam, DISP_OFF);
		ituWidgetSetVisible(Cam5_RCam, DISP_OFF);
		ituWidgetSetVisible(Cam5_BCam, DISP_OFF);
			
		printf("CameraIconDisp ==>  %d , %d , %d\n",DMode,tDMode,gCameraModeDisp);
		if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
		{
			switch(DMode)		//4: AB , 5:AR , 6:AL , 7:BR , 8:3c
			{
				case 4:
				{
					ituWidgetSetPosition(Cam5_BCam, CAM_ICON_2_H_X, CAM_ICON_2_H_Y);
					ituWidgetSetVisible(Cam5_BCam, DISP_ON); 	// 0:off , 1:BS , 2:B , 3:S , 4: AB , 5:AR , 6:AL , 7:BR , 8:3c
				}
				break;
				case 7:
				{
					ituWidgetSetPosition(Cam5_RCam, CAM_ICON_2_H_X, CAM_ICON_2_H_Y);
					ituWidgetSetPosition(Cam5_BCam, CAM_ICON_1_H_X, CAM_ICON_1_H_Y);
					ituWidgetSetVisible(Cam5_RCam, DISP_ON); 	// 0:off , 1:BS , 2:B , 3:S , 4: AB , 5:AR , 6:AL , 7:BR , 8:3c
					ituWidgetSetVisible(Cam5_BCam, DISP_ON);
				}
				break;
			}
		}
		else
		{
			switch(DMode)		//4: AB , 5:AR , 6:AL , 7:BR , 8:3c
			{
				case 4:
				{
					ituWidgetSetPosition(Cam5_BCam, CAM_ICON_2_V_X, CAM_ICON_2_V_Y);
					ituWidgetSetVisible(Cam5_BCam, DISP_ON); 	// 0:off , 1:BS , 2:B , 3:S , 4: AB , 5:AR , 6:AL , 7:BR , 8:3c
				}
				break;
				case 7:
				{
					ituWidgetSetPosition(Cam5_RCam, CAM_ICON_1_V_X,CAM_ICON_1_V_Y);
					ituWidgetSetPosition(Cam5_BCam, CAM_ICON_2_V_X,CAM_ICON_2_V_Y);
					ituWidgetSetVisible(Cam5_RCam, DISP_ON); 	// 0:off , 1:BS , 2:B , 3:S , 4: AB , 5:AR , 6:AL , 7:BR , 8:3c
					ituWidgetSetVisible(Cam5_BCam, DISP_ON);
				}
				break;
			}
		}
		JD_Buf[0]=1;	
	}
	tDMode=DMode;
	return true;
}

extern bool IconUpdate(ITUWidget* widget, ITUEvent ev, int arg1, int arg2, int arg3);
bool InitCamera(void)
{
	int j,i=0;

	printf("MainOnCAM LayerFlag:%d\n",LayerFlag );
	if (!LayerFlag)
	{
		SetWidget(backupCamContainer,"backupCamContainer");
		
		SetWidget(Cam5_BCam,"Cam5_BCam");
		SetWidget(Cam5_RCam,"Cam5_RCam");
		SetWidget(Cam5_LCam,"Cam5_LCam");

// ituWidgetSetUpdate(backupCamContainer, IconUpdate);


		ituWidgetSetVisible(Cam5_BCam, DISP_OFF);
		ituWidgetSetVisible(Cam5_RCam, DISP_OFF);
		ituWidgetSetVisible(Cam5_LCam, DISP_OFF);

		SetWidget(Cam5_2B_Black,"Cam5_2B_Black");
		ituWidgetSetVisible(Cam5_2B_Black, DISP_ON);
		#ifdef TEST_CAM_COLOR
			if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
			{
				SetWidget(Cam_Color_Con,"Cam_Color_Con");
				SetWidget(Cam_Color_Bt[0],"CamBrButton");
				SetWidget(Cam_Color_Bt[1],"CamConButton");
				SetWidget(Cam_Color_Bt[2],"CamSatButton");
				SetWidget(Cam_Color_Bt[3],"CamHueButton");
				SetWidget(Cam_Color_Bt[4],"CamShaButton");
				SetWidget(Cam_Color_Bt[5],"CamDayButton");
				SetWidget(Cam_Color_Bt[6],"CamNightButton");
				SetWidget(Cam_Color_Bt[7],"CamUpButton");
				SetWidget(Cam_Color_Bt[8],"CamDownButton");
				SetWidget(Cam_Color_Bt[9],"CamColor_Save");		// 1,2
				SetWidget(Cam_Color_Bt[10],"CamColor_Cancel");	// 3,4
				SetWidget(Cam_Color_Bt[11],"CamColor_Reset");	// 5,6
				SetWidget(Cam_Color_Bt[12],"CamColor_Cam");	// 5,6
				
			
				SetWidget(Cam_Color_Icon[0],"CamBrIcon");
				SetWidget(Cam_Color_Icon[1],"CamConIcon");
				SetWidget(Cam_Color_Icon[2],"CamSatIcon");
				SetWidget(Cam_Color_Icon[3],"CamHueIcon");
				SetWidget(Cam_Color_Icon[4],"CamShaIcon");
				SetWidget(Cam_Color_Icon[5],"CamDayIcon");
				SetWidget(Cam_Color_Icon[6],"CamNightIcon");
			
				SetWidget(Cam_Color_Text,"Cam_Color_Text");
				SetWidget(Cam_Color_TxIcon,"Cam_Color_TxIcon");
			}
		#endif
		ituWidgetSetVisible(Cam5_2B_Black, DISP_ON);
		LayerFlag=1;
    }
 

}
bool CameraSetColor(uint8_t DayNight,bool ReTry)
{
	static uint8_t tDayNight = 0xFF;
	

	static int8_t 	Temp_Brightness = 0xFF;
	static uint8_t Temp_Contrast = 0xFF;
	static uint8_t Temp_Saturation = 0xFF;
	static int8_t 	Temp_Hue = 0xFF;
	static uint8_t Temp_Sharpness = 0xFF;

	if ((tDayNight==DayNight)&&(ReTry==0))
	{
		return 0;
	}
	
	if (ReTry==1)
	{
		Temp_Brightness = 0xFF;
		Temp_Contrast = 0xFF;
		Temp_Saturation = 0xFF;
		Temp_Hue = 0xFF;
		Temp_Sharpness = 0xFF;	
	}
		
	if (DayNight == 0) // 0:Day , 1:Night
	{
		if (Temp_Brightness!=CC1.Brightness)
		{
			_tp2825_set_Brightness(CC1.Brightness);
			Temp_Brightness=CC1.Brightness;
		}
		if (Temp_Contrast!=CC1.Contrast)
		{
			_tp2825_set_Contrast(CC1.Contrast);
			Temp_Contrast=CC1.Contrast;
		}
		if (Temp_Saturation!=CC1.Saturation)
		{
			_tp2825_set_Uvgain(CC1.Saturation);
			Temp_Saturation=CC1.Saturation;
		}
		if (Temp_Hue!=CC1.Hue)
		{
			_tp2825_set_Hue(CC1.Hue);
			Temp_Hue=CC1.Hue;
		}
		if (Temp_Sharpness!=CC1.Sharpness)
		{
			_tp2825_set_Sharpness(CC1.Sharpness);
			Temp_Sharpness=CC1.Sharpness;
		}
	}
	else
	{
		if (Temp_Brightness!=CC2.Brightness)
		{
			_tp2825_set_Brightness(CC2.Brightness);
			Temp_Brightness=CC2.Brightness;
		}
		if (Temp_Contrast!=CC2.Contrast)
		{
			_tp2825_set_Contrast(CC2.Contrast);
			Temp_Contrast=CC2.Contrast;
		}
		if (Temp_Saturation!=CC2.Saturation)
		{
			_tp2825_set_Uvgain(CC2.Saturation);
			Temp_Saturation=CC2.Saturation;
		}
		if (Temp_Hue!=CC2.Hue)
		{
			_tp2825_set_Hue(CC2.Hue);
			Temp_Hue=CC2.Hue;
		}
		if (Temp_Sharpness!=CC2.Sharpness)
		{
			_tp2825_set_Sharpness(CC2.Sharpness);
			Temp_Sharpness=CC2.Sharpness;
		}
	}
	tDayNight=DayNight;
}
bool CameraOnEnter(void)
{
	unsigned int* addr = NULL;

	int j,i=0;
	
	if ((camera1playing!=0)||(gCameraStatus0!=4))	//gCameraLock0:1,gCameraStatus0:4
	{
//		printf("CameraOnEnter:Error  ==> CamMode:%d , Main_Disp_Mode:%d\n",CamMode,Main_Disp_Mode);
		return 0;
	}


	printf("\n Start : CameraOnEnter ==> CamMode:%d , Main_Disp_Mode:%d\n",CamMode,Main_Disp_Mode);
	printf("CameraOnEnter ==> gCameraLock0:%d,gCameraStatus0:%d , camera1playing:%d,gCameraStatus1:%d\n\n",gCameraLock0,gCameraStatus0,camera1playing,gCameraStatus1);
	GotoCamera(5);
	StartCameraStatus = gCameraStatus0;
	StartCameraStatusCnt = 0;
	
	
	CameraScreenDisp(CamMode);
	
	
	CaptureError = false;

	char tmp[32];
	
	{
		sprintf(tmp,"%s","Close");
		
		SceneEnterVideoState(33);
		
		#ifdef CFG_VIDEO_ENABLE

			if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
			{
	       		src_height = dst_width;
	        	src_width  = dst_height;
	        	itv_set_video_window(0, 0, dst_width, dst_height);
			}
			else
			{
	       		src_height = dst_height;
	        	src_width  = dst_width;
	        	itv_set_video_window(dst_x, dst_y, dst_width, dst_height);
			}
		        
			//Set video output size
	//		itv_set_video_window(dst_x, dst_y, dst_width, dst_height);
			//play
			mtal_pb_init(EventHandler);
			#ifdef CFG_SENSOR_ENABLE
				mtal_spec1.camera_in = CAPTURE_IN;//select capdev_player
				strcpy(mtal_spec1.srcname, SourcePath0);//select input ch 0
			#endif
			mtal_pb_select_file(&mtal_spec1);
			mtal_pb_play();
		#endif
	}
	SceneEnterVideoState(33);
	ScreenSetOnOff(1);
	
	
	printf("\n End CameraOnEnter : dst_x[%d] dst_y[%d] dst_width[%d] dst_height[%d] src_width[%d] src_height[%d]\n",dst_x, dst_y, dst_width, dst_height,src_width, src_height);
	#ifdef TEST_CAM_COLOR
		if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
		{
			Cam_Color_Con_Disp(DISP_ON);
			ituWidgetSetVisible(Cam_Color_Con, DISP_ON);
		}
		else
		{
//			ituWidgetSetVisible(Cam_Color_Con, DISP_OFF);
		}
		CamColorMode = 1;			// 0:Off , 1:On
		CamColorDayNight = 0;		// 0:Day , 1:Night
		CamColorIndex = 0;			// 0:Brightness , 1:Contrast , 2:Saturation , 3:Hue , 4:Sharpness;
	#endif
	
	
	ituWidgetSetVisible(Cam5_2B_Black, DISP_ON);
	camera1playing = 1;
    return true;
}

void CameraScreenDisp(uint8_t tCamMode)
{
	CamMode=tCamMode;
	printf("CameraScreenDisp ==>CamMode:%d , gCameraModeDisp:%d %d\n",CamMode,gCameraModeDisp,CPI_0Byte);


	ituWidgetGetGlobalPosition(backupCamContainer, &dst_x, &dst_y);
	dst_width = ituWidgetGetWidth(backupCamContainer);
	dst_height = ituWidgetGetHeight(backupCamContainer);

	ituWidgetSetDraw(backupCamContainer, PreviewVideoBackgroundDraw);

	printf("\nCameraScreenDisp:destx[%d], desty[%d], src_width[%d], src_height[%d]\n\n",dst_x, dst_y, src_width, src_height);
}


uint8_t CheckCameraModeDisp = 0xFF;
bool CameraCheckCPI(void)
{
	if (!LayerFlag)
		return true;
	CameraCheckCAN();
	if (camera1playing!=2)
	{
		ituWidgetSetVisible(Cam5_2B_Black, DISP_ON);
		CheckCameraModeDisp = 0xFF;
	}
	else if (CheckCameraModeDisp != CPI_0Byte)
	{
		if (CPI_0Byte==0)
		{	
			CameraIconDisp(gCameraModeDisp,DISP_ON,1);
			ituWidgetSetVisible(Cam5_2B_Black, DISP_ON);
		}
		else if ((CPI_0Byte==1)||(CPI_0Byte==5)||(CPI_0Byte==14)||(CPI_0Byte==15))	//0:off , 1,5: AB , 2,6:AR , 3,7,:AL , 14,15,:BR , 16,17:3c
		{
			ituWidgetSetVisible(Cam5_2B_Black, DISP_OFF);
			CameraIconDisp(CamMode,DISP_ON,0);
		}
		CheckCameraModeDisp = CPI_0Byte;
	}
	return true;
}

bool CameraOnLeave(ITUWidget* widget, char* param)
{
	
	printf("CameraOnLeave= '%s''%d''%d'\n",param,param,camera1playing);

	if (camera1playing!=0)
	{
		camera1playing = 0;
		gCameraModeDisp = 0;	// 0:off , 1:BS , 2:B , 3:S , 4: AB , 5:AR , 6:AL , 7:BR , 8:3c		
		#ifdef CFG_VIDEO_ENABLE
		mtal_pb_stop();
		mtal_pb_exit();
		#endif
		
		SceneLeaveVideoState();
	}
    return true;
}
bool CameraOnLeave4Rot(uint8_t CallSrc)
{
#if 1
	if (Main_Disp_Mode_Flag==1)
	{
		printf("\n[CameraOnLeave4Rot]Main_Disp_Mode_Flag===> : %d %d %d\n",Main_Disp_Mode,MainBtFlag,RotationMode);
		printf("[CameraOnLeave4Rot]Main_Disp_Mode_Flag===> : %d %d %d\n",Main_Disp_Mode,MainBtFlag,RotationMode);
		printf("[CameraOnLeave4Rot]Main_Disp_Mode_Flag===> : %d %d %d\n",Main_Disp_Mode,MainBtFlag,RotationMode);
		printf("[CameraOnLeave4Rot]Main_Disp_Mode_Flag===> : %d %d %d\n",Main_Disp_Mode,MainBtFlag,RotationMode);
		printf("[CameraOnLeave4Rot]Main_Disp_Mode_Flag===> : %d %d %d\n\n",Main_Disp_Mode,MainBtFlag,CallSrc);
	}		
//	if (Main_Disp_Mode_Flag==0)
	{
		CameraOnLeave(NULL,CallSrc);
		printf("CameraOnLeave4Rot A: [%d]\n",CallSrc);
	}
	printf("CameraOnLeave4Rot B:[%d][%d][%d]\n",CallSrc,Main_Disp_Mode,Main_Disp_Mode_Flag);
//	if (Main_Disp_Mode_Flag==1)
//		return 0;
	Main_Disp_Mode_Flag=0;
#endif	
	return true;
}
bool CameraOnLeaveCallSrc(uint8_t CallSrc)
{
	return true;
}		

int32_t CameraDisplayTime = 0;
bool CameraOnTimer(void)
{


	static uint32_t OnTimerCnt_33ms = 0;
	static uint32_t OnTimerCnt_100ms = 0;
	static uint32_t OnTimerCnt_300ms = 0;
	static uint32_t OnTimerCnt_500ms = 0;
	static uint32_t OnTimerCnt_600ms = 0;
	static uint32_t OnTimerCnt_1S = 0;
	static uint32_t OnTimerCnt_5S = 0,iCON_CNT=0,i;

	OnTimerCnt_33ms++;

	#ifdef USED_ROTATION				//jhlee 231020
		if(RotationMode!=0)
		{
			return 0;
		}
	#endif


	if(camera1playing==1)
	{
		printf("[CameraOnTimer1] '%d','%d','%d'\n",Main_Disp_Mode,CLM2_7Byte_5bit,camera1playing);

		camera1playing = 2;
		CameraDisplayTime = gMainTimerCnt_10m;
		Main_Disp_Mode_Flag=0;
	}
	else if(camera1playing==2)
	{

		if(CaptureError)
		{
			CameraOnLeave(NULL,NULL);		
		}
	}
	else
	{
//		printf("[CameraOnTimer_Test] '%d','%d','%d','%d'\n",Main_Disp_Mode,camera1playing,Main_Disp_Mode,RotationMode);
	}

	#ifdef TEST_CAM_COLOR
		if (((CamColorLongKeyCnt+TIME_CNT_500MS)<gMainTimerCnt_10m)&&(CamColorLongKeyCnt!=0))
		{
			char JD_Buf[10] = {0,};
			if (CamColorLongKeyMode==1)		// 1:up,2:down
			{
				JD_Buf[0] = 0x32;
				JD_Buf[1] = 0;
				CamColorUp(NULL, JD_Buf);
			}
			else if (CamColorLongKeyMode==2)		// 1:up,2:down
			{
				JD_Buf[0] = 0x32;
				JD_Buf[1] = 0;
				CamColorDown(NULL, JD_Buf);
			}
		}
	#endif

	if ((OnTimerCnt_33ms%TIME_CNT_33ms_100MS)==0)	// 100ms
	{
		char JD_Buf[10] = {0,};
		static int Cs_Cnt = 0;
		OnTimerCnt_100ms++;
		if (CaptureError)
		{
			if ((gCameraStatus0 == 4))
			{
				StartCameraStatusCnt++;
			}
			else
			{
				StartCameraStatusCnt = 0;
			}
			if (StartCameraStatusCnt>2)
			{
				StartCameraStatus = gCameraStatus0;
				CameraOnEnter();
			}
//			printf("gCameraStatus0:%d , gCameraLock0:%d , gCameraStatus1:%d , gCameraLock1:%d , StartCameraStatusCnt:%d\n", gCameraStatus0,gCameraLock0,gCameraStatus1,gCameraLock1,StartCameraStatusCnt);
		}
		else
		{
			if (camera1playing==0)
				CameraOnEnter();
		}
	}
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_300MS)==0)	// 300ms
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_300ms++;
		#ifndef TEST_CAM_COLOR
			if(camera1playing==2)
				CameraSetColor(Main_DayNight,0);
		#endif
	}
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_500MS)==0)	// 500ms
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_500ms++;
	}
	
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_600MS)==0)	// 600ms
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_600ms++;
	}
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_1000MS)==0)
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_1S++;
	}
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_5000MS)==0)
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_5S++;
	}
    return true;
}





#ifdef TEST_CAM_COLOR
	bool CamColorMenu(ITUWidget* widget, char* param)
	{
		uint8_t TP_Param = 0;
		char JD_Buf[10] = {0,};
		TP_Param = (param[0]&0x0F);
		printf("[Camera]%s : 0x%X\r\n", __FUNCTION__, param[0]);

		switch (TP_Param)
		{
			case 1:	// Save Down
			{
			}
			break;
			case 2:	// Save Up
			{
				CC1.Brightness 	= CC1_Brightness;
				CC1.Contrast 	= CC1_Contrast;
				CC1.Saturation 	= CC1_Saturation;
				CC1.Hue 		= CC1_Hue;
				CC1.Sharpness 	= CC1_Sharpness;
				CC2.Brightness 	= CC2_Brightness;
				CC2.Contrast	= CC2_Contrast;
				CC2.Saturation 	= CC2_Saturation;
				CC2.Hue 		= CC2_Hue;
				CC2.Sharpness 	= CC2_Sharpness;
				XO_Save_Config(M7_UserIndex,true,0);
			}
			break;
			case 3:	// Cancel Down
			{
			}
			break;
			case 4:	// Cancel Up
			{
				CC1_Brightness 	= CC1.Brightness;
				CC1_Contrast 	= CC1.Contrast;
				CC1_Saturation 	= CC1.Saturation;
				CC1_Hue 		= CC1.Hue;
				CC1_Sharpness 	= CC1.Sharpness;
				CC2_Brightness 	= CC2.Brightness;
				CC2_Contrast 	= CC2.Contrast;
				CC2_Saturation 	= CC2.Saturation;
				CC2_Hue 		= CC2.Hue;
				CC2_Sharpness 	= CC2.Sharpness;		
			}
			break;
			case 5:	// Reset Down
			{
			}
			break;
			case 6:	// Reset Up
			{
				CC1.Brightness	= DEFAULT_CAM_BR;
				CC1.Contrast	= DEFAULT_CAM_CR;
				CC1.Saturation	= DEFAULT_CAM_SR;
				CC1.Hue			= DEFAULT_CAM_HUE;
				CC1.Sharpness	= DEFAULT_CAM_SP;
				CC2.Brightness	= DEFAULT_CAM_BR_N;
				CC2.Contrast	= DEFAULT_CAM_CR_N;
				CC2.Saturation	= DEFAULT_CAM_SR_N;
				CC2.Hue			= DEFAULT_CAM_HUE_N;
				CC2.Sharpness	= DEFAULT_CAM_SP_N;
				
				CC1_Brightness 	= CC1.Brightness;
				CC1_Contrast 	= CC1.Contrast;
				CC1_Saturation 	= CC1.Saturation;
				CC1_Hue 		= CC1.Hue;
				CC1_Sharpness 	= CC1.Sharpness;
				CC2_Brightness 	= CC2.Brightness;
				CC2_Contrast 	= CC2.Contrast;
				CC2_Saturation 	= CC2.Saturation;
				CC2_Hue 		= CC2.Hue;
				CC2_Sharpness 	= CC2.Sharpness;
				XO_Save_Config(M7_UserIndex,true,0);
			}
			break;
		}
		CameraSetColor(CamColorDayNight,1);
		if (CamColorDayNight==1)	// 0:Day , 1:Night
		{
			JD_Buf[0] = 0;
			CamColorNight(NULL,JD_Buf);
		}
		else
		{
			JD_Buf[0] = 0;
			CamColorDay(NULL,JD_Buf);
		}
	    return true;
	}
	bool CamColorDown(ITUWidget* widget, char* param)
	{
		printf("[Camera]%s : 0x%X  [%d][%d]\r\n", __FUNCTION__, (param[0]&0x0F),CamColorDayNight,CamColorIndex);
		if (CamColorMode == 0)
			return 0;
		{
			if (((param[0]&0x0F)==0)||((param[0]&0x0F)==2))
			{
				if((param[0]&0x0F)==0)
				{
					CamColorLongKeyMode = 0;	// 1:up,2:down
					CamColorLongKeyCnt = 0;
				}
				switch(CamColorIndex)	//0:off , 1x:Up , 2x:Down
				{ 		
		    		case 0:
		    		{
		    			if (CamColorDayNight==0)
		    			{
							if (CC1_Brightness>-64)
							{
								CC1_Brightness--;
							}
		    			}
		    			else
		    			{
							if (CC2_Brightness>-64)
							{
								CC2_Brightness--;
							}
		    			}
						Cam_Color_Value_Disp(CamColorIndex,CC1_Brightness,CC2_Brightness,DISP_ON);
		    		}
		    		break;
		    		case 1:
		   			{
		    			if (CamColorDayNight==0)
		    			{
							if (CC1_Contrast>0)	//0-127
							{
								CC1_Contrast--;
							}
		    			}
		    			else
		    			{
							if (CC2_Contrast>0)	//0-127
							{
								CC2_Contrast--;
							}
		    			}
						Cam_Color_Value_Disp(CamColorIndex,CC1_Contrast,CC2_Contrast,DISP_ON);
		    		}
		    		break;
		    		case 2:
		   			{
		    			if (CamColorDayNight==0)
		    			{
							if (CC1_Saturation>0)	//0-127 , 166~255
							{
								CC1_Saturation--;
							}
		    			}
		    			else
		    			{
							if (CC2_Saturation>0)	//0-127 , 166~255
							{
								CC2_Saturation--;
							}
		    			}
						Cam_Color_Value_Disp(CamColorIndex,CC1_Saturation,CC2_Saturation,DISP_ON);
		    		}
		    		break;
		    		case 3:
		   			{
		    			if (CamColorDayNight==0)
		    			{
							if (CC1_Hue>-90)	//0~90
							{
								CC1_Hue--;
							}
		    			}
		    			else
		    			{
							if (CC2_Hue>-90)	//0~90
							{
								CC2_Hue--;
							}
		    			}
						Cam_Color_Value_Disp(CamColorIndex,CC1_Hue,CC2_Hue,DISP_ON);
		    		}
		    		break;
		    		case 4:
		   			{
		    			if (CamColorDayNight==0)
		    			{
							if (CC1_Sharpness>0)
							{
								CC1_Sharpness--;
							}
		    			}
		    			else
		    			{
							if (CC2_Sharpness>0)
							{
								CC2_Sharpness--;
							}
		    			}
						Cam_Color_Value_Disp(CamColorIndex,CC1_Sharpness,CC2_Sharpness,DISP_ON);
		    		}
		    		break;
		    	}
			}
			else if((param[0]&0x0F)==1)
			{
				CamColorLongKeyMode = 2;	// 1:up,2:down
				CamColorLongKeyCnt = gMainTimerCnt_10m;
			}
		}
	    return true;
	}
	
	bool CamColorUp(ITUWidget* widget, char* param)
	{
		printf("[Camera]%s : 0x%X  [%d][%d]\r\n", __FUNCTION__, (param[0]&0x0F),CamColorDayNight,CamColorIndex);
		if (CamColorMode == 0)
			return 0;
		{
			if (((param[0]&0x0F)==0)||((param[0]&0x0F)==2))
			{
				if((param[0]&0x0F)==0)
				{
					CamColorLongKeyMode = 0;	// 1:up,2:down
					CamColorLongKeyCnt = 0;
				}
				switch(CamColorIndex)	//0:off , 1x:Up , 2x:Down
				{ 		
		    		case 0:
		    		{
		    			if (CamColorDayNight==0)
		    			{
							if (CC1_Brightness<63)
							{
								CC1_Brightness++;
							}
		    			}
		    			else
		    			{
							if (CC2_Brightness<63)
							{
								CC2_Brightness++;
							}
		    			}
						Cam_Color_Value_Disp(CamColorIndex,CC1_Brightness,CC2_Brightness,DISP_ON);
		    		}
		    		break;
		    		case 1:
		   			{
		    			if (CamColorDayNight==0)
		    			{
							if (CC1_Contrast<127)//0-127
							{
								CC1_Contrast++;
							}
		    			}
		    			else
		    			{
							if (CC2_Contrast<127)//0-127
							{
								CC2_Contrast++;
							}
		    			}
						Cam_Color_Value_Disp(CamColorIndex,CC1_Contrast,CC2_Contrast,DISP_ON);
		    		}
		    		break;
		    		case 2:
		   			{
		    			if (CamColorDayNight==0)
		    			{
							if (CC1_Saturation<127)//0-127
							{
								CC1_Saturation++;
							}
		    			}
		    			else
		    			{
							if (CC2_Saturation<127)//0-127
							{
								CC2_Saturation++;
							}
		    			}
						Cam_Color_Value_Disp(CamColorIndex,CC1_Saturation,CC2_Saturation,DISP_ON);
		    		}
		    		break;
		    		case 3:
		   			{
		    			if (CamColorDayNight==0)
		    			{
							if (CC1_Hue<90)	//0~90
							{
								CC1_Hue++;
							}
		    			}
		    			else
		    			{
							if (CC2_Hue<90)	//0~90
							{
								CC2_Hue++;
							}
		    			}
						Cam_Color_Value_Disp(CamColorIndex,CC1_Hue,CC2_Hue,DISP_ON);
		    		}
		    		break;
		    		case 4:
		   			{
		    			if (CamColorDayNight==0)
		    			{
							if (CC1_Sharpness<31)	//0~31
							{
								CC1_Sharpness++;
							}
		    			}
		    			else
		    			{
							if (CC2_Sharpness<31)	//0~31
							{
								CC2_Sharpness++;
							}
		    			}
						Cam_Color_Value_Disp(CamColorIndex,CC1_Sharpness,CC2_Sharpness,DISP_ON);
		    		}
		    		break;
				}
			}
			else if((param[0]&0x0F)==1)
			{
				CamColorLongKeyMode = 1;	// 1:up,2:down
				CamColorLongKeyCnt = gMainTimerCnt_10m;
			}
		}
	    return true;
	}
	
	bool CamColorNight(ITUWidget* widget, char* param)
	{
		printf("[Camera]%s : 0x%X  [%d][%d]\r\n", __FUNCTION__, (param[0]&0x0F),CamColorDayNight,CamColorIndex);
		if (CamColorMode == 0)
			return 0;
		if ((param[0]&0x0F)==0)
		{
//			if (CamColorDayNight!=1)	// 0:Day , 1:Night
			{
//				CamColorIndex = 0;
				CamColorDayNight = 1;
				_tp2825_set_Brightness(CC2_Brightness);
				_tp2825_set_Contrast(CC2_Contrast);
				_tp2825_set_Uvgain(CC2_Saturation);
				_tp2825_set_Hue(CC2_Hue);
				_tp2825_set_Sharpness(CC2_Sharpness);		
				Cam_Color_Icon_Disp(CamColorIndex,CamColorDayNight,DISP_ON);
				switch(CamColorIndex)	//0:off , 1x:Up , 2x:Down
				{ 		
		    		case 0:
		    		{
						Cam_Color_Value_Disp(CamColorIndex,CC1_Brightness,CC2_Brightness,DISP_ON);
		    		}
		    		break;
		    		case 1:
		   			{
						Cam_Color_Value_Disp(CamColorIndex,CC1_Contrast,CC2_Contrast,DISP_ON);
		    		}
		    		break;
		    		case 2:
		   			{
						Cam_Color_Value_Disp(CamColorIndex,CC1_Saturation,CC2_Saturation,DISP_ON);
		    		}
		    		break;
		    		case 3:
		   			{
						Cam_Color_Value_Disp(CamColorIndex,CC1_Hue,CC2_Hue,DISP_ON);
		    		}
		    		break;
		    		case 4:
		   			{
						Cam_Color_Value_Disp(CamColorIndex,CC1_Sharpness,CC2_Sharpness,DISP_ON);
		    		}
		    		break;
				}
			}	
		}
	    return true;
	}
	
	bool CamColorDay(ITUWidget* widget, char* param)
	{
		printf("[Camera]%s : 0x%X  [%d][%d]\r\n", __FUNCTION__, (param[0]&0x0F),CamColorDayNight,CamColorIndex);
		if (CamColorMode == 0)
			return 0;
		if ((param[0]&0x0F)==0)
		{
//			if (CamColorDayNight!=0)	// 0:Day , 1:Night
			{
//				CamColorIndex = 0;
				CamColorDayNight=0;
				
				_tp2825_set_Brightness(CC1_Brightness);
				_tp2825_set_Contrast(CC1_Contrast);
				_tp2825_set_Uvgain(CC1_Saturation);
				_tp2825_set_Hue(CC1_Hue);
				_tp2825_set_Sharpness(CC1_Sharpness);
				
				Cam_Color_Icon_Disp(CamColorIndex,CamColorDayNight,DISP_ON);
				switch(CamColorIndex)	//0:off , 1x:Up , 2x:Down
				{ 		
		    		case 0:
		    		{
						Cam_Color_Value_Disp(CamColorIndex,CC1_Brightness,CC2_Brightness,DISP_ON);
		    		}
		    		break;
		    		case 1:
		   			{
						Cam_Color_Value_Disp(CamColorIndex,CC1_Contrast,CC2_Contrast,DISP_ON);
		    		}
		    		break;
		    		case 2:
		   			{
						Cam_Color_Value_Disp(CamColorIndex,CC1_Saturation,CC2_Saturation,DISP_ON);
		    		}
		    		break;
		    		case 3:
		   			{
						Cam_Color_Value_Disp(CamColorIndex,CC1_Hue,CC2_Hue,DISP_ON);
		    		}
		    		break;
		    		case 4:
		   			{
						Cam_Color_Value_Disp(CamColorIndex,CC1_Sharpness,CC2_Sharpness,DISP_ON);
		    		}
		    		break;
				}		
			}
		}
	    return true;
	}
	
	bool CamColorSha(ITUWidget* widget, char* param)
	{
		printf("[Camera]%s : 0x%X  [%d][%d]\r\n", __FUNCTION__, (param[0]&0x0F),CamColorDayNight,CamColorIndex);
		if (CamColorMode == 0)
			return 0;
		if ((param[0]&0x0F)==0)
		{
			if (CamColorIndex!=4)	// 0:Brightness , 1:Contrast , 2:Saturation , 3:Hue , 4:Sharpness;
			{
				CamColorIndex = 4;
				Cam_Color_Icon_Disp(CamColorIndex,CamColorDayNight,DISP_ON);
				Cam_Color_Value_Disp(CamColorIndex,CC1_Sharpness,CC2_Sharpness,DISP_ON);
			}
		}
	    return true;
	}
	
	bool CamColorHue(ITUWidget* widget, char* param)
	{
		printf("[Camera]%s : 0x%X  [%d][%d]\r\n", __FUNCTION__, (param[0]&0x0F),CamColorDayNight,CamColorIndex);
		if (CamColorMode == 0)
			return 0;
		if ((param[0]&0x0F)==0)
		{
			if (CamColorIndex!=3)	// 0:Brightness , 1:Contrast , 2:Saturation , 3:Hue , 4:Sharpness;
			{
				CamColorIndex = 3;
				Cam_Color_Icon_Disp(CamColorIndex,CamColorDayNight,DISP_ON);
				Cam_Color_Value_Disp(CamColorIndex,CC1_Hue,CC2_Hue,DISP_ON);
			}
		}
	    return true;
	}
	
	bool CamColorSat(ITUWidget* widget, char* param)
	{
		printf("[Camera]%s : 0x%X  [%d][%d]\r\n", __FUNCTION__, (param[0]&0x0F),CamColorDayNight,CamColorIndex);
		if (CamColorMode == 0)
			return 0;
		if ((param[0]&0x0F)==0)
		{
			if (CamColorIndex!=2)	// 0:Brightness , 1:Contrast , 2:Saturation , 3:Hue , 4:Sharpness;
			{
				CamColorIndex = 2;
				Cam_Color_Icon_Disp(CamColorIndex,CamColorDayNight,DISP_ON);
				Cam_Color_Value_Disp(CamColorIndex,CC1_Saturation,CC2_Saturation,DISP_ON);
			}
		}
	    return true;
	}
	
	bool CamColorCon(ITUWidget* widget, char* param)
	{
		printf("[Camera]%s : 0x%X  [%d][%d]\r\n", __FUNCTION__, (param[0]&0x0F),CamColorDayNight,CamColorIndex);
		if (CamColorMode == 0)
			return 0;
		if ((param[0]&0x0F)==0)
		{
			if (CamColorIndex!=1)	// 0:Brightness , 1:Contrast , 2:Saturation , 3:Hue , 4:Sharpness;
			{
				CamColorIndex = 1;
				Cam_Color_Icon_Disp(CamColorIndex,CamColorDayNight,DISP_ON);
				Cam_Color_Value_Disp(CamColorIndex,CC1_Contrast,CC2_Contrast,DISP_ON);
			}
		}
	    return true;
	}
	
	bool CamColorBr(ITUWidget* widget, char* param)
	{
		printf("[Camera]%s : 0x%X  [%d][%d]\r\n", __FUNCTION__, (param[0]&0x0F),CamColorDayNight,CamColorIndex);
		if (CamColorMode == 0)
			return 0;
		if ((param[0]&0x0F)==0)
		{
			if (CamColorIndex!=0)	// 0:Brightness , 1:Contrast , 2:Saturation , 3:Hue , 4:Sharpness;
			{
				CamColorIndex = 0;
				Cam_Color_Icon_Disp(CamColorIndex,CamColorDayNight,DISP_ON);
				Cam_Color_Value_Disp(CamColorIndex,CC1_Brightness,CC2_Brightness,DISP_ON);
			}
		}
	    return true;
	}

	bool Cam_Color_Icon_Disp(uint8_t SIndex,uint8_t SDayNight,DISP_MODE DispMode)
	{
		static uint8_t tDMode = 0xFF;
		static uint8_t tSIndex = 0xFF;
		static uint8_t tSDayNight = 0xFF;
		int i;
		if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_1))
			return 0;
		if ((tSDayNight!=SDayNight)||(tSIndex!=SIndex)||(tDMode!=DispMode))
		{
			for (i=0;i<7;i++)
			{
				ituWidgetSetVisible(Cam_Color_Icon[i],DISP_OFF);
			}
			ituWidgetSetVisible(Cam_Color_Icon[SIndex],DispMode);
			if (SDayNight==0)	// 0:Day , 1:Night
			{
				ituWidgetSetVisible(Cam_Color_Icon[5],DispMode);
			}
			else
			{
				ituWidgetSetVisible(Cam_Color_Icon[6],DispMode);
			}
			tDMode=DispMode;
			tSIndex=SIndex;
			tSDayNight=SDayNight;
		}
	}
	bool Cam_Color_Value_Disp(uint8_t SIndex,uint8_t ItemValueD,uint8_t ItemValueN,DISP_MODE DispMode)
	{
		char tmp[32];
		int32_t TempValue;

		if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_1))
			return 0;
		if (DispMode==DISP_ON)
		{
			if (CamColorDayNight==0)	// 0:Day , 1:Night
			{
		    	switch (SIndex)
		    	{
		    		case 0:
		    		{
		    			sprintf(tmp, "  %d  ",CC1_Brightness);
						_tp2825_set_Brightness(CC1_Brightness);
		    		}
		    		break;
		    		case 1:
		   			{
		   				sprintf(tmp, "  %d  ",CC1_Contrast);
						_tp2825_set_Contrast(CC1_Contrast);
		    		}
		    		break;
		    		case 2:
		    		{
		    			sprintf(tmp, "  %d  ",CC1_Saturation);
						_tp2825_set_Uvgain(CC1_Saturation);
		    		}
		    		break;
		    		case 3:
		    		{
		    			sprintf(tmp, "  %d  ",CC1_Hue);
		 				_tp2825_set_Hue(CC1_Hue);
		    		}
		    		break;
		    		case 4:
		    		{
		    			sprintf(tmp, "  %d  ",CC1_Sharpness);
		 				_tp2825_set_Sharpness(CC1_Sharpness);
		    		}
		    		break;	    		
		    	}
			}
			else
			{
		    	switch (SIndex)
		    	{
		    		case 0:
		    		{
		    			sprintf(tmp, "  %d  ",CC2_Brightness);
						_tp2825_set_Brightness(CC2_Brightness);
		    		}
		    		break;
		    		case 1:
		   			{
		   				sprintf(tmp, "  %d  ",CC2_Contrast);
						_tp2825_set_Contrast(CC2_Contrast);
		    		}
		    		break;
		    		case 2:
		    		{
		    			sprintf(tmp, "  %d  ",CC2_Saturation);
						_tp2825_set_Uvgain(CC2_Saturation);
		    		}
		    		break;
		    		case 3:
		    		{
		    			sprintf(tmp, "  %d  ",CC2_Hue);
		 				_tp2825_set_Hue(CC2_Hue);
		    		}
		    		break;
		    		case 4:
		    		{
		    			sprintf(tmp, "  %d  ",CC2_Sharpness);
		 				_tp2825_set_Sharpness(CC2_Sharpness);
		    		}
		    		break;
		    	}
			}
			ituTextSetString(Cam_Color_Text, tmp);
		}
		ituWidgetSetVisible(Cam_Color_Text,DispMode);
	}
	bool Cam_Color_Con_Disp(DISP_MODE DispMode)
	{
		static uint8_t tDMode = 0xFF;
		int i;
		uint8_t CoDataD = 0xFF;
		uint8_t CoDataN = 0xFF;
		
		if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_1))
			return 0;
		if (tDMode!=DispMode)
		{
			for (i=0;i<13;i++)
			{
				ituWidgetSetVisible(Cam_Color_Bt[i],DispMode);
			}
			ituWidgetSetVisible(Cam_Color_TxIcon,DispMode);
			
			Cam_Color_Icon_Disp(CamColorIndex,CamColorDayNight,DispMode);
			switch(CamColorIndex)
			{
	    		case 0:
	    		{
					CoDataD = CC1_Brightness;
					CoDataN = CC2_Brightness;
	    		}
	    		break;
	    		case 1:
	    		{
					CoDataD = CC1_Contrast;
					CoDataN = CC2_Contrast;
	    		}
	    		break;
	    		case 2:
	    		{
					CoDataD = CC1_Saturation;
					CoDataN = CC2_Saturation;
	    		}
	    		break;
	    		case 3:
	    		{
					CoDataD = CC1_Hue;
					CoDataN = CC2_Hue;
	    		}
	    		break;
	    		case 4:
	    		{
					CoDataD = CC1_Sharpness;
					CoDataN = CC2_Sharpness;
	    		}
	    		break;			
			}
			Cam_Color_Value_Disp(CamColorIndex,CoDataD,CoDataN,DispMode);
			tDMode=DispMode;
		}
	}
#else
	bool CamColorMenu(ITUWidget* widget, char* param)
	{
	    return true;
	}
	bool CamColorDown(ITUWidget* widget, char* param)
	{
	    return true;
	}
	
	bool CamColorUp(ITUWidget* widget, char* param)
	{
	    return true;
	}
	
	bool CamColorNight(ITUWidget* widget, char* param)
	{
	    return true;
	}
	
	bool CamColorDay(ITUWidget* widget, char* param)
	{
	    return true;
	}
	
	bool CamColorSha(ITUWidget* widget, char* param)
	{
	    return true;
	}
	
	bool CamColorHue(ITUWidget* widget, char* param)
	{
	    return true;
	}
	
	bool CamColorSat(ITUWidget* widget, char* param)
	{
	    return true;
	}
	
	bool CamColorCon(ITUWidget* widget, char* param)
	{
	    return true;
	}
	
	bool CamColorBr(ITUWidget* widget, char* param)
	{
	    return true;
	}	
#endif