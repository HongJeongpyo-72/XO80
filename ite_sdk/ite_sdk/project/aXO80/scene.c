#include <sys/ioctl.h>
#include <sys/time.h>
#include <assert.h>
#include <math.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "SDL/SDL.h"
#include "ite/itp.h"
#include "ite/itv.h"
#include "scene.h"
#include "ctrlboard.h"
#ifndef _WIN32
#include "flower/flower.h"
#endif

#include "HT_DispDdefine.h"


#ifdef _WIN32
    #include <crtdbg.h>

    #ifndef CFG_VIDEO_ENABLE
        #define DISABLE_SWITCH_VIDEO_STATE
    #endif
#endif // _WIN32

#ifndef CFG_POWER_WAKEUP_DOUBLE_CLICK_INTERVAL
    #define DOUBLE_KEY_INTERVAL 200
#endif

#define FPS_ENABLE
//#define DOUBLE_KEY_ENABLE
bool DOUBLE_KEY_ENABLE_FLAG=1;

#define GESTURE_THRESHOLD           40
#define MAX_COMMAND_QUEUE_SIZE      8
#define MOUSEDOWN_LONGPRESS_DELAY   1000

extern ITUActionFunction actionFunctions[];
extern void resetScene(void);

// status
static QuitValue    quitValue;
static bool         inVideoState;

// command
typedef enum
{
    CMD_NONE			=0,
    CMD_LOAD_SCENE		=0x81,
    CMD_CALL_CONNECTED	=0x82,
    CMD_GOTO_MAINMENU	=0x83,
    CMD_CHANGE_LANG		=0x84,
    CMD_PREDRAW			=0x85,
    CMD_GOTO_MAIN		=0x86,
    CMD_CHANGE_SCENE	=0x87
} CommandID;

#define MAX_STRARG_LEN 32

typedef struct
{
    CommandID   id;
    int         arg1;
    int         arg2;
    char        strarg1[MAX_STRARG_LEN];
	ITURotation arg3;
} Command;

static mqd_t        commandQueue = -1;

// scene
ITUScene            theScene;
static SDL_Window   *window;
static ITUSurface   *screenSurf;
static int          screenWidth;
static int          screenHeight;
static float        screenDistance;
static bool         isReady;
static int          periodPerFrame;

#if defined(CFG_USB_MOUSE) || defined(_WIN32)
static ITUIcon      *cursorIcon;
#endif

extern void ScreenSetDoubleClick(void);


// Test for 2 itu file rotation
static ExternalOrientation orgOrientation = EXTERNAL_VERTICAL;
static ExternalOrientation extOrientation = EXTERNAL_VERTICAL;
void ExternalSetOrientation(ExternalOrientation val)
{
	extOrientation = val;
}
// Test for 2 itu file rotation



extern void G_Sensor_Init(void);				// extern 
extern uint8_t Get_PL_Flag(void);			// extern

void SceneInit(void)
{
    struct mq_attr  attr;
    ITURotation     rot;

#ifdef CFG_LCD_ENABLE
    screenWidth     = ithLcdGetWidth();
    screenHeight    = ithLcdGetHeight();

    window          = SDL_CreateWindow("Display Control Board", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, 0);
    if (!window)
    {
        printf("Couldn't create window: %s\n", SDL_GetError());
        return;
    }

    // init itu
    ituLcdInit();

    #ifdef CFG_M2D_ENABLE
    	ituM2dInit();
    #else
    	ituSWInit();
    #endif // CFG_M2D_ENABLE

    ituSceneInit(&theScene, NULL);

    //#ifdef CFG_ENABLE_ROTATE
    //ituSceneSetRotation(&theScene, ITU_ROT_90, CFG_LCD_WIDTH, CFG_LCD_HEIGHT);
    //#endif

    //#ifdef CFG_VIDEO_ENABLE
    //ituFrameFuncInit();
    //#endif // CFG_VIDEO_ENABLE

	#ifdef CFG_PLAY_VIDEO_ON_BOOTING
		#ifndef CFG_BOOT_VIDEO_ENABLE_WINDOW_MODE
			rot = itv_get_rotation();
			
			if (rot == ITU_ROT_90 || rot == ITU_ROT_270)
				PlayVideo(0, 0, ithLcdGetHeight(), ithLcdGetWidth(), CFG_BOOT_VIDEO_BGCOLOR, CFG_BOOT_VIDEO_VOLUME);
			else
				PlayVideo(0, 0, ithLcdGetWidth(), ithLcdGetHeight(), CFG_BOOT_VIDEO_BGCOLOR, CFG_BOOT_VIDEO_VOLUME);
		#else
			PlayVideo(CFG_VIDEO_WINDOW_X_POS, CFG_VIDEO_WINDOW_Y_POS, CFG_VIDEO_WINDOW_WIDTH, CFG_VIDEO_WINDOW_HEIGHT, CFG_BOOT_VIDEO_BGCOLOR, CFG_BOOT_VIDEO_VOLUME);
		#endif
	#endif

	#ifdef CFG_PLAY_MJPEG_ON_BOOTING
		#ifndef CFG_BOOT_VIDEO_ENABLE_WINDOW_MODE
			rot = itv_get_rotation();
			
			if (rot == ITU_ROT_90 || rot == ITU_ROT_270)
				PlayMjpeg(0, 0, ithLcdGetHeight(), ithLcdGetWidth(), CFG_BOOT_VIDEO_BGCOLOR, 0);
			else
				PlayMjpeg(0, 0, ithLcdGetWidth(), ithLcdGetHeight(), CFG_BOOT_VIDEO_BGCOLOR, 0);
		#else
			PlayMjpeg(CFG_VIDEO_WINDOW_X_POS, CFG_VIDEO_WINDOW_Y_POS, CFG_VIDEO_WINDOW_WIDTH, CFG_VIDEO_WINDOW_HEIGHT, CFG_BOOT_VIDEO_BGCOLOR, 0);
		#endif
	#endif

    screenSurf = ituGetDisplaySurface();

    ituFtInit();
    

printf("\r\nStart Load Font\r\n");	    
    #ifdef USED_NOTOSANS_FONT
		ituFtLoadFont(0, CFG_PRIVATE_DRIVE ":/font/" "NotoSans-Regular.ttf", ITU_GLYPH_8BPP);
		ituFtLoadFont(1, CFG_PRIVATE_DRIVE ":/font/" "NotoSans-Bold.ttf", ITU_GLYPH_8BPP);    
    #else
		ituFtLoadFont(0, CFG_PRIVATE_DRIVE ":/font/" CFG_FONT_FILENAME, ITU_GLYPH_8BPP);				//Carlito-Regular.ttf
		//ituFtLoadFont(0, CFG_PRIVATE_DRIVE ":/font/" "Carlito-Regular.ttf", ITU_GLYPH_8BPP);
		
		ituFtLoadFont(1, CFG_PRIVATE_DRIVE ":/font/" "Carlito-Bold.ttf", ITU_GLYPH_8BPP);   
    #endif
printf("End Load Font\r\n\r\n");	    
    //ituSceneInit(&theScene, NULL);
    ituSceneSetFunctionTable(&theScene, actionFunctions);

    attr.mq_flags   = 0;
    attr.mq_maxmsg  = MAX_COMMAND_QUEUE_SIZE;
    attr.mq_msgsize = sizeof(Command);

    commandQueue    = mq_open("scene", O_CREAT | O_NONBLOCK, 0644, &attr);
    assert(commandQueue != -1);

    screenDistance  = sqrtf(screenWidth * screenWidth + screenHeight * screenHeight);

    isReady         = false;
    periodPerFrame  = MS_PER_FRAME;
#endif

#ifndef _WIN32
//	ite_flower_init();
#endif
}

void SceneExit(void)
{
#ifdef CFG_LCD_ENABLE
    mq_close(commandQueue);
    commandQueue = -1;

    resetScene();

    if (theScene.root)
    {
        ituSceneExit(&theScene);
    }
    ituFtExit();

    #ifdef CFG_M2D_ENABLE
    ituM2dExit();
        #ifdef CFG_VIDEO_ENABLE
    ituFrameFuncExit();
        #endif // CFG_VIDEO_ENABLE
    #else
    ituSWExit();
    #endif // CFG_M2D_ENABLE

    SDL_DestroyWindow(window);
#endif

#ifndef _WIN32
	ite_flower_deinit();
#endif
}

void SceneLoad(void)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    isReady = false;

    cmd.id  = CMD_LOAD_SCENE;

    mq_send(commandQueue, (const char *)&cmd, sizeof(Command), 0);
}

void SceneGotoMainMenu(void)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    cmd.id = CMD_GOTO_MAINMENU;
    mq_send(commandQueue, (const char *)&cmd, sizeof(Command), 0);
}

void SceneChangeLanguage(void)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    cmd.id = CMD_CHANGE_LANG;
    mq_send(commandQueue, (const char *)&cmd, sizeof(Command), 0);
}

void ScenePredraw(int arg)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    cmd.id = CMD_PREDRAW;
    mq_send(commandQueue, (const char *)&cmd, sizeof(Command), 0);
}

void SceneSetReady(bool ready)
{
    isReady = ready;
}




void GotoMain(uint8_t SubMode) 
{

    Command cmd;

    if (commandQueue == -1)
        return;
	cmd.id = SubMode;
	mq_send(commandQueue, (const char *)&cmd, sizeof(Command), 0);
}


#ifdef TEST_ITU_USB
	#define V_ITU 	"E:/aXO80_V.itu"
	#define H_ITU 	"E:/aXO80_H.itu"
#else
	#define V_ITU 	CFG_PRIVATE_DRIVE ":/aXO80_V.itu"
	#define H_ITU 	CFG_PRIVATE_DRIVE ":/aXO80_H.itu"
#endif		

ITUScene theSceneV;
ITUScene theSceneH;
//#define THESCENE_READ


uint8_t tPL_Flag_Old = 0;
static void LoadScene(void)
{
#ifdef CFG_LCD_ENABLE
    uint32_t tick1, tick2;
	G_Sensor_XYZ();
    resetScene();
    if (theScene.root)
    {
        ituSceneExit(&theScene);
    }


	G_Sensor_XYZ();
    #ifdef CFG_LCD_MULTIPLE
    {
        char filepath[PATH_MAX];

        sprintf(filepath, CFG_PRIVATE_DRIVE ":/itu/%ux%u/aXO80.itu", ithLcdGetWidth(), ithLcdGetHeight());
        ituSceneLoadFileCore(&theScene, filepath);
    }
    #else
//    ituSceneLoadFileCore(&theScene, CFG_PRIVATE_DRIVE ":/A27C_P.itu");

	G_Sensor_XYZ();
#if 0
    ituSceneLoadFileCore(&theScene, CFG_PRIVATE_DRIVE ":/aXO80_V.itu");
#else    

		
	#ifdef THESCENE_READ
		ituSceneLoadFileCore(&theSceneV, V_ITU);
		ituSceneLoadFileCore(&theSceneH, H_ITU);
	#endif
	printf("\r\nStart G_Sensor_XYZ\r\n");
	while(G_Sensor_XYZ()!=2)
	{
		usleep(1000);
		Cal_XYZ2(0);
	}

	Cal_XYZ2(0);
	printf("End G_Sensor_XYZ\r\n");
    // load itu file
    tick1 = SDL_GetTicks();	
	PL_Flag_New_Value = Get_PL_Flag();
	if (PL_Flag_New_Value==NO_CHECK)
		PL_Flag_New_Value = PORTRAIT_2;
			
	switch(PL_Flag_New_Value)
	{
		case PORTRAIT_1:
			printf("sPORTRATE_1\r\n");
			#ifdef THESCENE_READ
				memcpy (&theScene,&theSceneV,sizeof(ITUScene));
			#else
				ituSceneLoadFileCore(&theScene, V_ITU);
			#endif
			printf("sPORTRATE_11\r\n");
			ituSceneSetRotation(&theScene, ITU_ROT_180, CFG_LCD_WIDTH, CFG_LCD_HEIGHT);
			printf("sPORTRATE_12\r\n");
			orgOrientation = EXTERNAL_VERTICAL;
		break;
			
		case PORTRAIT_2:
			printf("sPORTRATE_2\r\n");
			#ifdef THESCENE_READ
				memcpy (&theScene,&theSceneV,sizeof(ITUScene));
			#else
				ituSceneLoadFileCore(&theScene, V_ITU);
			#endif
			printf("sPORTRATE_21\r\n");
			ituSceneSetRotation(&theScene, ITU_ROT_0, CFG_LCD_WIDTH, CFG_LCD_HEIGHT);
			printf("sPORTRATE_22\r\n");
			orgOrientation = EXTERNAL_VERTICAL;
		break;
			
		case LANDSCAPE_1:
			printf("sLANDSCAPE_1\r\n");
			#ifdef THESCENE_READ
				memcpy (&theScene,&theSceneH,sizeof(ITUScene));
			#else
				ituSceneLoadFileCore(&theScene, H_ITU);
			#endif
			printf("sLANDSCAPE_11\r\n");
			ituSceneSetRotation(&theScene, ITU_ROT_90, CFG_LCD_WIDTH, CFG_LCD_HEIGHT);
			printf("sLANDSCAPE_12\r\n");
			orgOrientation = EXTERNAL_HORIZONTAL;
		break;
			
		case LANDSCAPE_2:
			printf("sLANDSCAPE_2\r\n");
			#ifdef THESCENE_READ
				memcpy (&theScene,&theSceneH,sizeof(ITUScene));
			#else
				ituSceneLoadFileCore(&theScene, H_ITU);
			#endif
			printf("sLANDSCAPE_21\r\n");
			ituSceneSetRotation(&theScene, ITU_ROT_270, CFG_LCD_WIDTH, CFG_LCD_HEIGHT);
			printf("sLANDSCAPE_22\r\n");
			orgOrientation = EXTERNAL_HORIZONTAL;
		break;
			
		default:
			printf("Default Portrate2\r\n");
			PL_Flag_New_Value = PORTRAIT_2;
			#ifdef THESCENE_READ
				memcpy (&theScene,&theSceneV,sizeof(ITUScene));
			#else
				ituSceneLoadFileCore(&theScene, V_ITU);
			#endif
			printf("Default Portrate21\r\n");
			ituSceneSetRotation(&theScene, ITU_ROT_0, CFG_LCD_WIDTH, CFG_LCD_HEIGHT);
			printf("Default Portrate22\r\n");
			orgOrientation = EXTERNAL_VERTICAL;
		break;
	}

#endif	
	
#endif // CFG_LCD_MULTIPLE
		PL_Flag_Manual = PL_Flag_New_Value;
		extOrientation = orgOrientation;


printf("End LoadFileCore, ITU\r\n\r\n");	

    tick2 = SDL_GetTicks();
    printf("itu loading time: %dms\n", tick2 - tick1);

//    if (theConfig.lang != LANG_ENG)
 //       ituSceneUpdate(&theScene, ITU_EVENT_LANGUAGE, theConfig.lang, 0, 0);

    //ituSceneSetRotation(&theScene, ITU_ROT_90, ithLcdGetWidth(), ithLcdGetHeight());

    tick1       = tick2;

    #if defined(CFG_USB_MOUSE) || defined(_WIN32)
    cursorIcon  = ituSceneFindWidget(&theScene, "cursorIcon");
    if (cursorIcon)
    {
        ituWidgetSetVisible(cursorIcon, true);
    }
    #endif // defined(CFG_USB_MOUSE) || defined(_WIN32)

    tick2 = SDL_GetTicks();
    printf("itu init time: %dms\n", tick2 - tick1);

//    ExternalProcessInit();
#endif
}
// Test for 2 itu file rotation
void SceneChange(ITURotation rot)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    isReady = false;

    cmd.id  = CMD_CHANGE_SCENE;
		cmd.arg3 = rot;
		
		mq_send(commandQueue, (const char *)&cmd, sizeof(Command), 0);
}


#define MICOM_RX_PRE_CNT 300


static void ChangeScene(ITURotation rotation)
{
	int i;
//	if(orgOrientation != extOrientation)
	{
		orgOrientation = extOrientation;

	    if (theScene.root)
	    {
//	    	ituSceneLoadFileCancel(&theScene);
	        ituSceneExit(&theScene);
	    }
	    
		ituM2dExit(); 
		ituM2dInit();
				
		printf(" --> ChangeScene 2\n");		
	    if (extOrientation == EXTERNAL_VERTICAL)
	    {
			printf(" --> Change v.itu : '%d'\n",rotation);
			ScreenSetOnOff(0);
			printf(" --> ChangeScene 3\n");
			#ifdef THESCENE_READ
				memcpy (&theScene,&theSceneV,sizeof(ITUScene));
			#else
				for (i=0;i<MICOM_RX_PRE_CNT;i++)
					MICOM_RxParsing();
			ituSceneLoadFileCancel(&theScene);
				ituSceneLoadFileCore(&theScene, V_ITU);
			#endif
	    }
	    else
	    {
			printf(" --> Change h.itu : '%d'\n",rotation);
			printf(" --> ChangeScene 4\n");	
			ScreenSetOnOff(0);
			#ifdef THESCENE_READ
				memcpy (&theScene,&theSceneH,sizeof(ITUScene));
			#else
				for (i=0;i<MICOM_RX_PRE_CNT;i++)
					MICOM_RxParsing();
			ituSceneLoadFileCancel(&theScene);
				ituSceneLoadFileCore(&theScene, H_ITU);
			#endif
	    }
		printf(" --> ChangeScene 5\n");
		if(theScene.rotation != rotation)
		{
	  		ituSceneSetRotation(&theScene, rotation, CFG_LCD_WIDTH, CFG_LCD_HEIGHT);
		}
		
		CameraOnLeave4Rot(22);
		printf(" --> ChangeScene 6\n");
		ituSceneStart(&theScene);
		printf(" --> Change itu END\n");
	}
	#if 0
	else
	{
		if(theScene.rotation != rotation)
		{
	  		ituSceneSetRotation(&theScene, rotation, CFG_LCD_WIDTH, CFG_LCD_HEIGHT);
		}
	}
	#endif
}


void SceneEnterVideoState(int timePerFrm)
{
    if (inVideoState)
    {
        return;
    }

#ifndef DISABLE_SWITCH_VIDEO_STATE
    #ifdef CFG_VIDEO_ENABLE
    ituFrameFuncInit();
    #endif
    screenSurf      = ituGetDisplaySurface();
    inVideoState    = true;
    if (timePerFrm != 0)
        periodPerFrame = timePerFrm;
#endif
}

void SceneLeaveVideoState(void)
{
    if (!inVideoState)
    {
        return;
    }

#ifndef DISABLE_SWITCH_VIDEO_STATE
    #ifdef CFG_VIDEO_ENABLE
    ituFrameFuncExit();
    #endif
    #ifdef CFG_LCD_ENABLE
    ituLcdInit();
    #endif
    #ifdef CFG_M2D_ENABLE
    ituM2dInit();
    #else
    ituSWInit();
    #endif

    screenSurf      = ituGetDisplaySurface();
    periodPerFrame  = MS_PER_FRAME;
#endif
    inVideoState    = false;
}

static void ProcessCommand(void)
{
    Command cmd;

    while (mq_receive(commandQueue, (char *)&cmd, sizeof(Command), 0) > 0)
    {
        switch (cmd.id)
        {
			case CMD_LOAD_SCENE:
				LoadScene();
				#if defined(CFG_PLAY_VIDEO_ON_BOOTING)
				ituScenePreDraw(&theScene, screenSurf);
				WaitPlayVideoFinish();
				#elif defined(CFG_PLAY_MJPEG_ON_BOOTING)
				ituScenePreDraw(&theScene, screenSurf);
				WaitPlayMjpegFinish();
				#endif
				printf("====>Start ituSceneStart\n");
				ituSceneStart(&theScene);
				printf("====>End ituSceneStart\n");
			break;
			
			case CMD_GOTO_MAINMENU:
			//            GotoMainMenu();
			break;
			
			case CMD_CHANGE_LANG:
				ituSceneUpdate( &theScene,  ITU_EVENT_LANGUAGE, theConfig.lang, 0,  0);
				ituSceneUpdate( &theScene,  ITU_EVENT_LAYOUT,   0,              0,  0);
			break;
			
			#if !defined(CFG_PLAY_VIDEO_ON_BOOTING) && !defined(CFG_PLAY_MJPEG_ON_BOOTING)
			case CMD_PREDRAW:
				ituScenePreDraw(&theScene, screenSurf);
			break;
			#endif
			// Test for 2 itu file rotation
			case CMD_CHANGE_SCENE:
				ChangeScene(cmd.arg3);
			break;
			
			case MAIN_MODE_NORMAL:
			case MAIN_MODE_USERINFO:
//			case MAIN_MODE_CAMERA:
			case MAIN_MODE_RADIO:
			case MAIN_MODE_AIRCON:
			case MAIN_MODE_DAB1:
			case MAIN_MODE_DAB2:
			case MAIN_MODE_DAB3:
			case MAIN_MODE_BLUETOOTH:
			case MAIN_MODE_PHONE:
			case MAIN_FIRMWARE_UP:
			{
				GotoMainReal(cmd.id);
			}
			break;
			case MAIN_MODE_MENU:
			{
				GotoMenuTop();
			}
			break;
			case MAIN_MODE_ENGN_COOL:
			{
				GotoEngnCool();
			}
			break;
			case MAIN_MODE_BACK:
			{
	//			GotoMainReal(cmd.id);
			}
			break;
        }
    }
}

static bool CheckQuitValue(void)
{
    if (quitValue)
    {
        if (ScreenSaverIsScreenSaving() && theConfig.screensaver_type == SCREENSAVER_BLANK)
            ScreenSaverRefresh();

        return true;
    }
    return false;
}

static void CheckStorage(void)
{
    StorageAction action = StorageCheck();

    switch (action)
    {
    case STORAGE_SD_INSERTED:
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_SD_INSERTED, NULL);
        break;

    case STORAGE_SD_REMOVED:
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_SD_REMOVED, NULL);
        break;

    case STORAGE_USB_INSERTED:
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_USB_INSERTED, NULL);
        break;

    case STORAGE_USB_REMOVED:
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_USB_REMOVED, NULL);
        break;

    case STORAGE_USB_DEVICE_INSERTED:
        {
            ITULayer *usbDeviceModeLayer = ituSceneFindWidget(&theScene, "usbDeviceModeLayer");
            assert(usbDeviceModeLayer);

            ituLayerGoto(usbDeviceModeLayer);
        }
        break;

    case STORAGE_USB_DEVICE_REMOVED:
        {
            ITULayer *mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
            assert(mainMenuLayer);

            ituLayerGoto(mainMenuLayer);
        }
        break;
    }
}

static void CheckExternal(void)
{
    ExternalEvent   ev;
    int             ret = ExternalReceive(&ev);

    if (ret)
    {
        ScreenSaverRefresh();
        ExternalProcessEvent(&ev);
    }
}

#if defined(CFG_USB_MOUSE) || defined(_WIN32)

static void CheckMouse(void)
{
    if (ioctl(ITP_DEVICE_USBMOUSE, ITP_IOCTL_IS_AVAIL, NULL))
    {
        if (!ituWidgetIsVisible(cursorIcon))
            ituWidgetSetVisible(cursorIcon, true);
    }
    else
    {
        if (ituWidgetIsVisible(cursorIcon))
            ituWidgetSetVisible(cursorIcon, false);
    }
}

#endif // defined(CFG_USB_MOUSE) || defined(_WIN32)
extern bool UART_FULL_Flag;

bool MICOM_RxParsingRun(void)
{
	#ifndef TEST_ROTATION
		#ifndef CAN_PARSING_TIMER
			MICOM_RxParsing();
			MICOM_RxParsing();
			MICOM_RxParsing();
			MICOM_RxParsing();
			MICOM_RxParsing();
			MainTimerIcon();
		#endif
	#endif
}

#ifdef TEST_CAM_COLOR
	extern uint8_t CamColorLongKeyMode;
	extern uint32_t CamColorLongKeyCnt;
#endif


bool IconUpdate(ITUWidget* widget, ITUEvent ev, int arg1, int arg2, int arg3)
{
	return 1;
	
	
    if (ev == ITU_EVENT_TOUCHPINCH)
    {
        int x = ituWidgetGetX(widget) - arg1 / 2;
        int y = ituWidgetGetY(widget) - arg1 / 2;
        int width = ituWidgetGetWidth(widget) + arg1;
        int height = ituWidgetGetHeight(widget) + arg1;
		printf("IconUpdate:arg[%d][%d][%d],x[%d],y[%d],width[%d],height[%d],Swidth[%d],Sheight[%d]\n", arg1,arg2,arg3,x,y,width,height,screenWidth,screenHeight);
		if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
		{
	        if (width > 0 && width <= screenWidth && height > 0 && height <= screenHeight)
	        {
	            ituWidgetSetPosition(widget, x, y);
	            ituWidgetSetDimension(widget, width, height);
	        }
		}
		else
		{
	        if (width > 0 && width <= screenHeight && height > 0 && height <= screenWidth)
	        {
	            ituWidgetSetPosition(widget, y, x);
	            ituWidgetSetDimension(widget, width, height);
	        }
		}
    }
    return ituIconUpdate(widget, ev, arg1, arg2, arg3);
}



extern uint8_t MT_D5_Flag;
extern uint8_t MT_D7_Flag;
int SceneRun(void)
{
    SDL_Event   ev;
	int					delay, frames, lastx, lasty;
	uint32_t		tick, dblclk, lasttick, mouseDownTick;
	static bool	first_screenSurf = true, sleepModeDoubleClick = false;
#if defined(CFG_POWER_WAKEUP_IR)
	static bool	sleepModeIR = false;
#endif


	/* Watch keystrokes */
	dblclk = frames = lasttick = lastx = lasty = mouseDownTick = 0;
	ithGpioClear(HT_GPIO_BUZZER);
	gBuzzerMode=0;	//0:OFF , 1:ON(100)OFF(1900) , 2:ON(200)OFF(800) , 3:ON(500)OFF(500) , 4:ON(300)OFF(300)  , 5:ON , 6:ON(100)OFF(9900)  , 7:ON(100)OFF(100) 3Time   , 8:ON(300)OFF(300 MI) , 10:ON(500)OFF
	
	
	printf("\n ===>  Start SceneRun\n\n");
	
	
	for(;;)
	{
		bool result = false;
        if (CheckQuitValue())
            break;
#ifdef CFG_LCD_ENABLE
        ProcessCommand();
#endif
//        CheckExternal();
        CheckStorage();

        tick = SDL_GetTicks();


#ifdef FPS_ENABLE
        frames++;
        if (tick - lasttick >= 1000)
        {
            printf("fps: %d\n", frames);
            frames      = 0;
            lasttick    = tick;
        }
#endif     // FPS_ENABLE
		MICOM_RxParsingRun();
		
		gCameraStatus0 = _tp2825_get_control_state();
		gCameraLock0 = _tp2825_get_lock_state();
#ifdef CFG_SECOND_SENSOR_ENABLE
		gCameraStatus1 = _tp2825_2_get_control_state();
		gCameraLock1 = _tp2825_2_get_lock_state();
#endif		
		
		//printf("gCameraStatus1:%d , gCameraLock1:%d , gCameraStatus2:%d , gCameraLock2:%d\n", gCameraStatus1,gCameraLock1,gCameraStatus2,gCameraLock2);
		
#ifdef CFG_LCD_ENABLE
        while (SDL_PollEvent(&ev))
        {
			MICOM_RxParsingRun();
            switch (ev.type)
            {
            case SDL_KEYDOWN:
                result = ituSceneUpdate(&theScene, ITU_EVENT_KEYDOWN, ev.key.keysym.sym, 0, 0);
//                MICOM_RxParsingRun();
                switch (ev.key.keysym.sym)
                {
                case SDLK_UP:
                    ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY0, NULL);
                    break;

                case SDLK_DOWN:
                    ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY1, NULL);
                    break;

                case SDLK_LEFT:
                    ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY2, NULL);
                    break;

                case SDLK_RIGHT:
                    ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY3, NULL);
                    break;

                case SDLK_INSERT:
                    break;

    #ifdef _WIN32
                case SDLK_e:
                    result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHPINCH, 20, 30, 40);
                    break;

                case SDLK_f:
                    {
                        ITULayer *usbDeviceModeLayer = ituSceneFindWidget(&theScene, "usbDeviceModeLayer");
                        assert(usbDeviceModeLayer);

                        ituLayerGoto(usbDeviceModeLayer);
                    }
                    break;

                case SDLK_g:
                    {
                        ExternalEvent ev;

                        ev.type = EXTERNAL_SHOW_MSG;
                        strcpy(ev.buf1, "test");

                        ScreenSaverRefresh();
                        ExternalProcessEvent(&ev);
                    }
                    break;

    #endif          // _WIN32
                }
                break;

            case SDL_KEYUP:
                result = ituSceneUpdate(&theScene, ITU_EVENT_KEYUP, ev.key.keysym.sym, 0, 0);
                break;

            case SDL_MOUSEMOTION:
    #if defined(CFG_USB_MOUSE) || defined(_WIN32)
                if (cursorIcon)
                {
                    ituWidgetSetX(cursorIcon, ev.button.x);
                    ituWidgetSetY(cursorIcon, ev.button.y);
                    ituWidgetSetDirty(cursorIcon, true);
                    //printf("mouse: move %d, %d\n", ev.button.x, ev.button.y);
                }
    #endif             // defined(CFG_USB_MOUSE) || defined(_WIN32)
                result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEMOVE, ev.button.button, ev.button.x, ev.button.y);
                break;

            case SDL_MOUSEBUTTONDOWN:
                printf("mouse: down %d, %d\n", ev.button.x, ev.button.y);
                {
                    mouseDownTick = SDL_GetTicks();
                    if (DOUBLE_KEY_ENABLE_FLAG==1)
                    {
	                    if (mouseDownTick - dblclk <= 200)
	                    {
	                        result  = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOUBLECLICK, ev.button.button, ev.button.x, ev.button.y);
	                        dblclk  = 0;
	                    }
	                    else
	                    {
	                        result  = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOWN, ev.button.button, ev.button.x, ev.button.y);
	                        dblclk  = mouseDownTick;
	                        lastx   = ev.button.x;
	                        lasty   = ev.button.y;
	                    }
                    }
                    else
                    {
                        result  = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOWN, ev.button.button, ev.button.x, ev.button.y);
                        dblclk  = mouseDownTick;
                        lastx   = ev.button.x;
                        lasty   = ev.button.y;
                    }
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (SDL_GetTicks() - dblclk <= 200)
                {
                    int xdiff   = abs(ev.button.x - lastx);
                    int ydiff   = abs(ev.button.y - lasty);

                    if (xdiff >= GESTURE_THRESHOLD && xdiff > ydiff)
                    {
                        if (ev.button.x > lastx)
                        {
           //                 printf("mouse: slide to right\n");
                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDERIGHT, xdiff, ev.button.x, ev.button.y);
                        }
                        else
                        {
           //                 printf("mouse: slide to left\n");
                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDELEFT, xdiff, ev.button.x, ev.button.y);
                        }
                    }
                    else if (ydiff >= GESTURE_THRESHOLD)
                    {
                        if (ev.button.y > lasty)
                        {
           //                 printf("mouse: slide to down\n");
                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEDOWN, ydiff, ev.button.x, ev.button.y);
                        }
                        else
                        {
         //                   printf("mouse: slide to up\n");
                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEUP, ydiff, ev.button.x, ev.button.y);
                        }
                    }
                }
                result          |= ituSceneUpdate(&theScene, ITU_EVENT_MOUSEUP, ev.button.button, ev.button.x, ev.button.y);
                mouseDownTick   = 0;
                break;

            case SDL_FINGERMOTION:
//                printf("touch: move %d, %d\n", ev.tfinger.x, ev.tfinger.y);
                result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEMOVE, 1, ev.tfinger.x, ev.tfinger.y);
                break;

            case SDL_FINGERDOWN:		           
            	printf("touch: down %d, %d\n", ev.tfinger.x, ev.tfinger.y);
				#if 1	//2024-06-19 add whlim
				if(RotationMode != 0)
				{
					break;
				}
				#endif
				{
					mouseDownTick = SDL_GetTicks();
					if (DOUBLE_KEY_ENABLE_FLAG==1)
					{
						#ifdef CFG_POWER_WAKEUP_DOUBLE_CLICK_INTERVAL
							if (mouseDownTick - dblclk <= CFG_POWER_WAKEUP_DOUBLE_CLICK_INTERVAL)
						#else
							if (mouseDownTick - dblclk <= 200)
						#endif
							{
								printf("double touch!\n");
								if (sleepModeDoubleClick)
								{
									ScreenSetDoubleClick();
									sleepModeDoubleClick = false;
								}
								result  = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOUBLECLICK, 1, ev.tfinger.x, ev.tfinger.y);
								dblclk  = mouseDownTick = 0;
							}
							else
							{
								result  = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOWN, 1, ev.tfinger.x, ev.tfinger.y);
								dblclk  = mouseDownTick;
								lastx   = ev.tfinger.x;
								lasty   = ev.tfinger.y;
							}						
					}
					else
					{
						result  = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOWN, 1, ev.tfinger.x, ev.tfinger.y);
						dblclk  = mouseDownTick;
						lastx   = ev.tfinger.x;
						lasty   = ev.tfinger.y;
					}
				}
                break;

            case SDL_FINGERUP:
                printf("touch: up %d, %d\n", ev.tfinger.x, ev.tfinger.y);
	if ((MT_D5_Flag==1)&&(MT_D7_Flag==1))
	{
		MT_D5_Flag = 0;
		MT_D7_Flag = 0;
	}
				#if 1	//2024-06-19 add whlim
				if(RotationMode != 0)
				{
					mouseDownTick   = 0;
					break;
				}
				#endif
#ifdef TEST_CAM_COLOR
				CamColorLongKeyMode = 0;	// 1:up,2:down
				CamColorLongKeyCnt = 0;
#endif				
						InitJogIncDecData_AC();
						InitJogIncDecData_RD();
						M4_TouchUP_Restet();
						M6_TouchUP_Restet();
						M7S1_TouchUP_Restet();
						M7S2_TouchUP_Restet();
						M7S4_TouchUP_Restet();
						
//	MT_DeepCHK = 0;	
//					MICOM_RxParsingRun();	
					if((SDL_GetTicks() - dblclk) <= 300)
					{
						int xdiff = abs(ev.tfinger.x - lastx);
						int ydiff = abs(ev.tfinger.y - lasty);

						if((xdiff >= GESTURE_THRESHOLD) && (xdiff > ydiff))
						{
							if(ev.tfinger.x > lastx)
							{
//								printf("touch: slide to right %d %d\n", ev.tfinger.x, ev.tfinger.y);
								result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDERIGHT, xdiff, ev.tfinger.x, ev.tfinger.y);
							}
							else
							{
//								printf("touch: slide to left %d %d\n", ev.tfinger.x, ev.tfinger.y);
								result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDELEFT, xdiff, ev.tfinger.x, ev.tfinger.y);
							}
						}
						else if(ydiff >= GESTURE_THRESHOLD)
						{	
							if(ev.tfinger.y > lasty)
							{
	//                            printf("touch: slide to down %d %d\n", ev.tfinger.x, ev.tfinger.y);
	                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEDOWN, ydiff, ev.tfinger.x, ev.tfinger.y);
	                        }
	                        else
	                        {
	//                            printf("touch: slide to up %d %d\n", ev.tfinger.x, ev.tfinger.y);
	                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEUP, ydiff, ev.tfinger.x, ev.tfinger.y);
	                        }
	                    }
	                }
                result          |= ituSceneUpdate(&theScene, ITU_EVENT_MOUSEUP, 1, ev.tfinger.x, ev.tfinger.y);
                mouseDownTick   = 0;
                break;

            case SDL_MULTIGESTURE:
                printf("touch: multi [%d],[%d],[%d]\n", ev.mgesture.x, ev.mgesture.y,ev.mgesture.dDist);
                if (ev.mgesture.dDist > 0.002f || ev.mgesture.dDist < -0.002f)
                {
                    int dist    = (int)(screenDistance * ev.mgesture.dDist);
                    int x       = (int)(screenWidth * ev.mgesture.x);
                    int y       = (int)(screenHeight * ev.mgesture.y);
                    result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHPINCH, dist, x, y);
                }
                break;
			}
		}
		
		{
			MICOM_RxParsingRun();
			MICOM_RxParsingRun();
		}
			result |= ituSceneUpdate(&theScene, ITU_EVENT_TIMER, 0, 0, 0);
			if(result)
			{
				ituSceneDraw(&theScene, screenSurf);
				ituFlip(screenSurf);

				if(first_screenSurf)
				{
					ScreenSetBrightness(theConfig.brightness);
					first_screenSurf = false;
				}
			}
#endif //#ifdef CFG_LCD_ENABLE
#if 1			
	#ifdef USED_ROTATION
	{
		if (RotationMode==0)
		{
//			printf("Get_PL_Flag:%d , RotationDir:%d\r\n",Get_PL_Flag(),RotationDir());
			if ((Get_PL_Flag()!=RotationDir())&&(Option_Function.Rotate_Flag==1))
			{
				HT_RunRotation();
			}
		}
	}
	#endif
		do		// 33MS�� �� ���?�ִ� 3ms(��Ʈ ���� ok)  , 33MS�� �� ���?�ִ� 8ms(��Ʈ �� ��), 17ms �� ���?�ִ� 4ms(��Ʈ ���� ok)
		{
			#ifndef CAN_PARSING_TIMER
			{
				int i;
				if (UART_FULL_Flag == 1)
				{
					for (i=0;i<30;i++)
						MICOM_RxParsing();
					UART_FULL_Flag = 0;
				}
				MICOM_RxParsingRun();
			}
			#endif
			delay = periodPerFrame - (SDL_GetTicks() - tick);
		}while (delay > 0);
//		sched_yield(); 	
#else
	delay = periodPerFrame - (SDL_GetTicks() - tick);
	//printf("scene loop delay=%d\n", delay);
	if (delay > 0)
	{
	    SDL_Delay(delay);
	}
	else
	    sched_yield();
#endif		

    }

    return quitValue;
}

void SceneQuit(QuitValue value)
{
    quitValue = value;
}

QuitValue SceneGetQuitValue(void)
{
    return quitValue;
}