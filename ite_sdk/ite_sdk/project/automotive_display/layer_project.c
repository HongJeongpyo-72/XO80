#include <assert.h>
#include <sys/time.h>
#include "scene.h"
#include "audio_mgr.h"
#include "SDL/SDL.h"
#include "leaf/leaf_mediastream.h"
#include "qrencode.h"
#include "Qrenc.h"
#include "lwip/ip.h"
#include "castor3player.h"

#ifdef CFG_NET_WIFI_SDIO_NGPL
#include "wifi_conf.h"
extern struct netif xnetif[NET_IF_NUM];
extern uint8_t* LwIP_GetMAC(struct netif *pnetif);
extern uint8_t* LwIP_GetIP(struct netif *pnetif);
#endif

#if defined(CFG_NET_WIFI_SDIO_NGPL_AP6256) || defined(CFG_NET_WIFI_SDIO_NGPL_AP6236) || defined(CFG_NET_WIFI_SDIO_NGPL_AP6181)
#include "mhd_api.h"

#define FORMAT_IPADDR(x) ((unsigned char *)&x)[3], ((unsigned char *)&x)[2], ((unsigned char *)&x)[1], ((unsigned char *)&x)[0]

#endif

static ITUBackground* openingBg;

static ITUBackground* projectCentrolBackground;
static ITUBackground* projectButtonBackground;
static ITUButton* projectPlayButton;
static ITUButton* projectChangeUIButton;
static ITUButton* projectShowReverseButton;
static ITUIcon* projectREARIcon;

static ITUAnimation*  projectDirectionAnimation[2];
static ITUAnimation*  projectBottomAnimation;
static ITUAnimation* projectFuelAnimation;
static ITUAnimation* projectTempAnimation;
static ITUAnimation* projectSpeedAnimation;
static ITUAnimation* projectModeAnimation;

static ITUIcon* projectTempPointIcon[2];
static ITUIcon* projectFuelPointIcon[2];
static ITUText* projectFuelText[2];
static ITUText* projectTempText[2];

static ITUSprite* projectModeSprite[2];
static ITUSprite* projectGearLeverSprite;
static ITUSprite* projectDirectionSprite[2][2];

static ITUProgressBar* projectFuelProgressBar[2];
static ITUProgressBar* projectTempProgressBar[2];
static ITUBackground* projectSpeedBackground1;
static ITUCircleProgressBar* projectSpeedCircleProgressBar[2];//260~0
static ITUMeter* projectSpeedMeter[2];
static ITUText* projectSpeedText[2]; //0~260
static ITUText* projectODOText[2]; //00000000000~9999999999

static ITUContainer* projectPhoneContainer = 0;
static ITUBackground* projectPhoneBackground = 0;
static ITUContainer* projectQrcodeContainer = 0;
static ITUIcon*	projectQrcodeIcon = 0;
static ITUTextBox* projectConnectTextBox = 0;

//static ITUVideo* projectReverseVideo;
static ITUAnimation* projectReverseLAnimation;
static ITUAnimation* projectReverseRAnimation;
static ITUAnimation* projectReverseCAnimation;
static ITUIcon* projectReverseIcon;
static ITUSprite* projectReverseSprite;

static ITUContainer* projectReversingViewContainer;

static ITUButton* projectWiFiButton = 0;
//#ifdef CFG_ENABLE_ROTATE
static ITUBackground* projectHorizontalBackground;
static ITUContainer* projectHorizontalPhoneContainer = 0;
static ITUBackground* projectHorizontalPhoneBackground = 0;
static ITUText* projectHSpeedText = 0;
static ITUText* projectHODOText = 0;
static ITUText* projectHFuelText = 0;
static ITUText* projectHTempText = 0;
static ITUIcon* projectHFuelPointIcon = 0;
static ITUIcon* projectHTempPointIcon = 0;

static bool showingHorizontalBackground = false;
//#endif
static bool showingPhoneContainer = false;
static  uint32_t lastTick = 0;
static  bool is_playing = false;
static  bool temp_is_add = false;
static  bool fuel_is_add = true;
static  bool speed_is_add = true;
static  int direction_frame = 0;
static  int temperature = 110;
static  int fuel = 0;
static  int speed = 0;
static  int odo = 0;
static  int is_central_pos = true;
static  int demo_loop = 0;
static int x, y, width, height = 0;

struct timeval tv;
struct tm* tm;

int pTemperaturePointX[86] = {
	37, 35, 34, 32, 30, 29, 27, 26, 24, 23, 21, 20, 19, 17, 16, 15, 14, 13, 12, 11,
	10, 9, 8, 8, 7, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5,
	6, 7, 8, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 20, 21, 23, 24, 26, 27,
	29, 30, 32, 34, 35, 37
};//temperature range: 25~110
int pTemperaturePointY[86] = {
	315, 311, 308, 304, 300, 296, 293, 289, 285, 282, 278, 274, 271, 267, 263, 259, 256, 252, 248, 245,
	241, 237, 233, 230, 226, 222, 219, 215, 211, 208, 204, 200, 196, 193, 189, 185, 182, 178, 174, 170,
	167, 163, 159, 156, 152, 148, 145, 141, 137, 133, 130, 126, 122, 119, 115, 111, 107, 104, 100, 96,
	93, 89, 85, 82, 78, 74, 70, 67, 63, 59, 56, 52, 48, 44, 41, 37, 33, 30, 26, 22,
	19, 15, 11, 7, 4, 0
};//temperature range: 25~110

int pFuelPointX[101] = {
	18, 20, 21, 23, 24, 26, 27, 28, 30, 31, 32, 33, 34, 36, 37, 38, 39, 40, 41, 42,
	43, 43, 44, 45, 46, 47, 47, 48, 49, 49, 50, 50, 51, 51, 52, 52, 53, 53, 53, 54,
	54, 54, 55, 55, 55, 55, 55, 55, 55, 55, 56, 55, 55, 55, 55, 55, 55, 55, 55, 54,
	54, 54, 53, 53, 53, 52, 52, 51, 51, 50, 50, 49, 49, 48, 47, 47, 46, 45, 44, 43,
	43, 42, 41, 40, 39, 38, 37, 36, 34, 33, 32, 31, 30, 28, 27, 26, 24, 23, 21, 20, 18 };

int pFuelPointY[101] = {
	315, 312, 309, 306, 302, 299, 296, 293, 290, 287, 284, 280, 277, 274, 271, 268, 265, 261, 258, 255,
	252, 249, 246, 243, 239, 236, 233, 230, 227, 224, 221, 217, 214, 211, 208, 205, 202, 198, 195, 192,
	189, 186, 183, 180, 176, 173, 170, 167, 164, 161, 158, 154, 151, 148, 145, 142, 139, 135, 132, 129,
	126, 123, 120, 117, 113, 110, 107, 104, 101, 98, 95, 91, 88, 85, 82, 79, 76, 72, 69, 66,
	63, 60, 57, 54, 50, 47, 44, 41, 38, 35, 32, 28, 25, 22, 19, 16, 13, 9, 6, 3, 0 };

int TemperatureHPointX[86] = {
	0,3,6,10,13,16,19,22,25,29,32,35,38,41,44,48,51,54,57,60,64,67,70,73,76,79,
	83,86,89,92,95,98,102,105,108,111,114,118,121,124,127,130,133,137,140,143,146,
	149,152,156,159,162,165,168,172,175,178,181,184,187,191,194,197,200,203,206,210,213,216,219,222,226,229,
	232,235,238,241,245,248,251,254,257,260,264,267,270
};//temperature range: 25~110

int TemperatureHPointY[86] = {
	32,30,29,27,26,25,23,22,21,19,18,17,16,15,14,13,12,11,10,9,9,8,7,6,6,5,5,4,4,3,3,2,2,2,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,3,3,4,4,5,5,6,6,7,8,9,9,10,11,12,13,14,15,16,17,18,19,21,22,23,25,26,27,29,30,32
};//temperature range: 25~110

int FuelHPointX[101] = {
	0,3,5,8,11,14,16,19,22,24,27,30,32,35,38,41,43,46,49,51,54,57,59,62,65,68,70,73,76,78,81,84,86,89,92,95,97,
	100,103,105,108,111,113,116,119,122,124,127,130,132,135,138,140,143,146,149,151,154,157,159,162,165,167,170,
	173,176,178,181,184,186,189,192,194,197,200,203,205,208,211,213,216,219,221,224,227,230,232,235,238,240,243,
	246,248,251,254,257,259,262,265,267,270
 };

int FuelHPointY[101] = {
	20,21,23,24,25,26,28,29,30,31,32,33,34,35,36,37,38,39,39,40,41,42,42,43,44,44,45,46,46,47,47,48,48,48,49,49,
	50,50,50,51,51,51,51,51,52,52,52,52,52,52,52,52,52,52,52,52,52,51,51,51,51,51,50,50,50,49,49,48,48,48,47,47,46,46,45,
	44,44,43,42,42,41,40,39,39,38,37,36,35,34,33,32,31,30,29,28,26,25,24,23,21,20
 };


bool pModeSpriteDemo(int time);
bool pGearSpriteDemo(int time);
bool pDirectionSpriteDemo(int time);
bool pTempDemo(int loops);
bool pFuelDemo(int loop);
bool pSpeedDemo(int loop);
bool pODODemo(int loop);

#define QRCODE_TEXT                 "SSID:iteAP\nPASSWORD:12345678\nMAC:"       //Text to encode into QRCode
#define QRCODE_SIZE                 6                                       //Show Output size 

typedef struct Rect
{
	int x;
	int y;
	int size; //size*size rect
} Rect;
static void Fill_Rect(uint16_t* dest, int w, Rect rect, bool color);
static void Fill_Rect_32(uint32_t* dest, int w, Rect rect, bool color);
static void Draw_QRcode(QRcode * content, ITUIcon* icon, ITUPixelFormat format);

int gShowTextMode = 0;
int gTextMode = 0;
int showText(int nMode);
static MTAL_SPEC mtal_spec2 = { 0 };
static char Tfile[] = "B:/media/reverse.mkv";

#ifdef CFG_VIDEO_ENABLE
static void PreviewVideoBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
	int destx, desty;
	ITURectangle* rect = (ITURectangle*) &widget->rect;

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
		//is_file_eof = true;
		break;

	default:
		//is_file_eof = true;
		break;
	}
}
#endif
bool pModeSpriteDemo(int time)// change in each 600ms 
{
	static int frame = 0;
	static int pre_frame = 0;
	int ret = false;

	frame = (time / 600) % 4;
	if (pre_frame != frame)
	{
		if (is_central_pos == true)
		{
			ituSpriteGoto(projectModeSprite[0], frame);
		}
		else
		{
			ituSpriteGoto(projectModeSprite[1], frame);
		}
		
		pre_frame = frame;
		ret = true;
	}
	return ret;
}

bool pGearSpriteDemo(int time)// change in each 600ms 
{
	static int frame = 0;
	static int pre_frame = 0;
	bool ret = false;

	frame = (time / 600) % 6;
	if (pre_frame != frame)
	{
		ituSpriteGoto(projectGearLeverSprite, frame);
		pre_frame = frame;
		ret = true;
	}
	return ret;
}

bool pDirectionSpriteDemo(int time)//change in each 800ms
{
	static int pre_frame = 0;
	bool ret = false;

	direction_frame = (time / 800) % 2;
	if (pre_frame != direction_frame)
	{
		if (is_central_pos == true)
		{
			ituSpriteGoto(projectDirectionSprite[0][0], direction_frame);
			ituSpriteGoto(projectDirectionSprite[1][0], direction_frame);
		}
		else
		{
			ituSpriteGoto(projectDirectionSprite[0][1], direction_frame);
			ituSpriteGoto(projectDirectionSprite[1][1], direction_frame);
		}

		pre_frame = direction_frame;
		ret = true;
	}
	return ret;
}

bool pTempDemo(int loop)
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
	if (is_central_pos == true)
	{
		//ituProgressBarSetValue(projectTempProgressBar[0], temperature);
		ituTextSetString(projectTempText[0], tmp);
		ituWidgetSetPosition(projectTempPointIcon[0], pTemperaturePointX[temperature - 25], pTemperaturePointY[temperature - 25]);
	}
//#ifdef CFG_ENABLE_ROTATE	
	else if (showingHorizontalBackground)
	{
		ituTextSetString(projectHTempText, tmp);
		ituWidgetSetPosition(projectHTempPointIcon, TemperatureHPointX[temperature - 25], TemperatureHPointY[temperature - 25]);
	}
//#endif	
	else
	{
		//ituProgressBarSetValue(projectTempProgressBar[1], temperature);
		ituTextSetString(projectTempText[1], tmp);
		ituWidgetSetPosition(projectTempPointIcon[1], pTemperaturePointX[temperature - 25], pTemperaturePointY[temperature - 25]);
	}

	ret = true;
	

	return ret;
}

bool pFuelDemo(int loop)
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

	if (is_central_pos == true)
	{
		//ituProgressBarSetValue(projectFuelProgressBar[0], fuel);
		ituTextSetString(projectFuelText[0], tmp);
		ituWidgetSetPosition(projectFuelPointIcon[0], pFuelPointX[fuel], pFuelPointY[fuel]);
	}
//#ifdef CFG_ENABLE_ROTATE	
	else if (showingHorizontalBackground)
	{
		ituTextSetString(projectHFuelText, tmp);
		ituWidgetSetPosition(projectHFuelPointIcon, FuelHPointX[fuel], FuelHPointY[fuel]);
	}
//#endif	
	else
	{
		//ituProgressBarSetValue(projectFuelProgressBar[1], fuel);
		ituTextSetString(projectFuelText[1], tmp);
		ituWidgetSetPosition(projectFuelPointIcon[1], pFuelPointX[fuel], pFuelPointY[fuel]);
	}
	ret = true;
	
	return ret;
}


bool pSpeedDemo(int loop)
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
	if (is_central_pos == true)
	{
		//ituCircleProgressBarSetValue(projectSpeedCircleProgressBar[0], 260 - speed);
		ituMeterSetValue(projectSpeedMeter[0], 260 - speed);
		ituTextSetString(projectSpeedText[0], tmp);
	}
//#ifdef CFG_ENABLE_ROTATE	
	else if (showingHorizontalBackground)
	{
		ituTextSetString(projectHSpeedText, tmp);
	}
//#endif	
	else
	{
		//ituCircleProgressBarSetValue(projectSpeedCircleProgressBar[1], 260 - speed);
		ituMeterSetValue(projectSpeedMeter[1], 260 - speed);
		ituTextSetString(projectSpeedText[1], tmp);
	}
	ret = true;
	
	return ret;
}

bool pODODemo(int loop)
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

		if (is_central_pos == true)
			ituTextSetString(projectODOText[0], tmp);		
//#ifdef CFG_ENABLE_ROTATE			
		else if (showingHorizontalBackground)
			ituTextSetString(projectHODOText, tmp);
//#endif			
		else
			ituTextSetString(projectODOText[1], tmp);
		ret = true;
	}


	return ret;
}

void pPlayAnimation()
{
	demo_loop = 0;
	is_playing = true;
}

void pShowReversing()
{
	ituWidgetSetVisible(projectReverseSprite, true);
	ituSpritePlay(projectReverseSprite, 0);
	ituWidgetSetVisible(projectReverseLAnimation, true);
	ituAnimationPlay(projectReverseLAnimation, 0);
}

bool gInDemoLayer = false;
bool ProjectOnEnter(ITUWidget* widget, char* param)
{
	printf("[project]%s\n", __FUNCTION__);

	int i = 0;
	int j = 0;
	char tmp[64];
	
    gInDemoLayer = true;

	gettimeofday(&tv, NULL);
	tm = localtime(&tv.tv_sec);

	if (!projectCentrolBackground)
	{
		projectCentrolBackground = ituSceneFindWidget(&theScene, "projectCentrolBackground");
		assert(projectCentrolBackground);

		projectButtonBackground = ituSceneFindWidget(&theScene, "projectButtonBackground");
		assert(projectButtonBackground);

		projectPlayButton = ituSceneFindWidget(&theScene, "projectPlayButton");
		assert(projectPlayButton);

		projectChangeUIButton = ituSceneFindWidget(&theScene, "projectChangeUIButton");
		assert(projectChangeUIButton);

		projectShowReverseButton = ituSceneFindWidget(&theScene, "projectShowReverseButton");
		assert(projectShowReverseButton);

		projectREARIcon = ituSceneFindWidget(&theScene, "projectREARIcon");
		assert(projectREARIcon);

		projectGearLeverSprite = ituSceneFindWidget(&theScene, "projectGearLeverSprite");
		assert(projectGearLeverSprite);

		projectBottomAnimation = ituSceneFindWidget(&theScene, "projectBottomAnimation");
		assert(projectBottomAnimation);

		projectFuelAnimation = ituSceneFindWidget(&theScene, "projectFuelAnimation");
		assert(projectFuelAnimation);

		projectModeAnimation = ituSceneFindWidget(&theScene, "projectModeAnimation");
		assert(projectModeAnimation);

		projectTempAnimation = ituSceneFindWidget(&theScene, "projectTempAnimation");
		assert(projectTempAnimation);

		projectSpeedAnimation = ituSceneFindWidget(&theScene, "projectSpeedAnimation");
		assert(projectSpeedAnimation);

		projectSpeedBackground1 = ituSceneFindWidget(&theScene, "projectSpeedBackground1");
		assert(projectSpeedBackground1);

		projectPhoneContainer = ituSceneFindWidget(&theScene, "projectPhoneContainer");
		assert(projectPhoneContainer);

		projectPhoneBackground = ituSceneFindWidget(&theScene, "projectPhoneBackground");
		assert(projectPhoneBackground);

		projectQrcodeContainer = ituSceneFindWidget(&theScene, "projectQrcodeContainer");
		assert(projectQrcodeContainer);

		projectQrcodeIcon = ituSceneFindWidget(&theScene, "projectQrcodeIcon");
		assert(projectQrcodeIcon);

		projectConnectTextBox = ituSceneFindWidget(&theScene, "projectConnectTextBox");
		assert(projectConnectTextBox);

		//projectReverseVideo = ituSceneFindWidget(&theScene, "projectReverseVideo");
		//assert(projectReverseVideo);
		projectReversingViewContainer = ituSceneFindWidget(&theScene, "projectReversingViewContainer");
		assert(projectReversingViewContainer);

		projectReverseLAnimation = ituSceneFindWidget(&theScene, "projectReverseLAnimation");
		assert(projectReverseLAnimation);

		projectReverseRAnimation = ituSceneFindWidget(&theScene, "projectReverseRAnimation");
		assert(projectReverseRAnimation);

		projectReverseCAnimation = ituSceneFindWidget(&theScene, "projectReverseCAnimation");
		assert(projectReverseCAnimation);

		projectReverseIcon = ituSceneFindWidget(&theScene, "projectReverseIcon");
		assert(projectReverseIcon);

		projectReverseSprite = ituSceneFindWidget(&theScene, "projectReverseSprite");
		assert(projectReverseSprite);

//#ifdef CFG_ENABLE_ROTATE
		projectHorizontalBackground = ituSceneFindWidget(&theScene, "projectHorizontalBackground");
		assert(projectHorizontalBackground);

		projectHorizontalPhoneContainer = ituSceneFindWidget(&theScene, "projectHorizontalPhoneContainer");
		assert(projectHorizontalPhoneContainer);

		projectHorizontalPhoneBackground = ituSceneFindWidget(&theScene, "projectHorizontalPhoneBackground");
		assert(projectHorizontalPhoneBackground);

		projectHSpeedText = ituSceneFindWidget(&theScene, "projectHSpeedText");
		assert(projectHSpeedText);
		
		projectHODOText = ituSceneFindWidget(&theScene, "projectHODOText");
		assert(projectHODOText);
		
		projectHFuelText = ituSceneFindWidget(&theScene, "projectHFuelText");
		assert(projectHFuelText);
		
		projectHTempText = ituSceneFindWidget(&theScene, "projectHTempText");
		assert(projectHTempText);
		
		projectHFuelPointIcon = ituSceneFindWidget(&theScene, "projectHFuelPointIcon");
		assert(projectHFuelPointIcon);
		
		projectHTempPointIcon = ituSceneFindWidget(&theScene, "projectHTempPointIcon");
		assert(projectHTempPointIcon);
	
//#endif		

		projectWiFiButton = ituSceneFindWidget(&theScene, "projectWiFiButton");
		assert(projectWiFiButton);

		for (i = 0; i < 2; i++)
		{
			sprintf(tmp, "projectDirectionAnimation%d", i);
			projectDirectionAnimation[i] = ituSceneFindWidget(&theScene, tmp);
			assert(projectDirectionAnimation[i]);

			sprintf(tmp, "projectTempPointIcon%d", i);
			projectTempPointIcon[i] = ituSceneFindWidget(&theScene, tmp);
			assert(projectTempPointIcon[i]);

			sprintf(tmp, "projectFuelPointIcon%d", i);
			projectFuelPointIcon[i] = ituSceneFindWidget(&theScene, tmp);
			assert(projectFuelPointIcon[i]);

			for (j = 0; j < 2; j++)
			{
				sprintf(tmp, "projectDirectionSprite%d%d", i,j);
				projectDirectionSprite[i][j] = ituSceneFindWidget(&theScene, tmp);
				assert(projectDirectionSprite[i][j]);
				
			}
			

			sprintf(tmp, "projectFuelProgressBar%d", i);
			projectFuelProgressBar[i] = ituSceneFindWidget(&theScene, tmp);
			assert(projectFuelProgressBar[i]);

			sprintf(tmp, "projectTempProgressBar%d", i);
			projectTempProgressBar[i] = ituSceneFindWidget(&theScene, tmp);
			assert(projectTempProgressBar[i]);

			sprintf(tmp, "projectSpeedCircleProgressBar%d", i);
			projectSpeedCircleProgressBar[i] = ituSceneFindWidget(&theScene, tmp);
			assert(projectSpeedCircleProgressBar[i]);

			sprintf(tmp, "projectSpeedText%d", i);
			projectSpeedText[i] = ituSceneFindWidget(&theScene, tmp);
			assert(projectSpeedText[i]);

			sprintf(tmp, "projectODOText%d", i);
			projectODOText[i] = ituSceneFindWidget(&theScene, tmp);
			assert(projectODOText[i]);

			sprintf(tmp, "projectModeSprite%d", i);
			projectModeSprite[i] = ituSceneFindWidget(&theScene, tmp);
			assert(projectModeSprite[i]);

			sprintf(tmp, "projectSpeedMeter%d", i);
			projectSpeedMeter[i] = ituSceneFindWidget(&theScene, tmp);
			assert(projectSpeedMeter[i]);

			sprintf(tmp, "projectFuelText%d", i);
			projectFuelText[i] = ituSceneFindWidget(&theScene, tmp);
			assert(projectFuelText[i]);

			sprintf(tmp, "projectTempText%d", i);
			projectTempText[i] = ituSceneFindWidget(&theScene, tmp);
			assert(projectTempText[i]);
			
			
		}
		
	}
	ituWidgetSetVisible(projectWiFiButton, false);

	is_central_pos = true;
	ituWidgetEnable(projectButtonBackground);

	ituWidgetSetVisible(projectREARIcon, true);
	

	ituSpriteGoto(projectModeSprite[0], 0);
	ituSpriteGoto(projectGearLeverSprite, 0);
	ituSpriteGoto(projectDirectionSprite[0][0], 0);
	ituSpriteGoto(projectDirectionSprite[0][1], 0);
	ituSpriteGoto(projectDirectionSprite[1][0], 0);
	ituSpriteGoto(projectDirectionSprite[1][1], 0);

	temp_is_add = false;
	temperature = 110;
	fuel_is_add = true;
	fuel = 0;
	speed_is_add = true;
	speed = 0;
	odo = 0;

	demo_loop = 0;
	direction_frame = 0;


	for (i = 0; i < 2; i++)
	{
		//ituProgressBarSetValue(projectTempProgressBar[i], 25);
		ituTextSetString(projectTempText[i], "25");
		ituWidgetSetPosition(projectTempPointIcon[i], pTemperaturePointX[0], pTemperaturePointY[0]);

		//ituProgressBarSetValue(projectFuelProgressBar[i], 0);
		ituTextSetString(projectFuelText[i], "0");
		ituWidgetSetPosition(projectFuelPointIcon[i], pFuelPointX[0], pFuelPointY[0]);

		//ituCircleProgressBarSetValue(projectSpeedCircleProgressBar[i], 260 - speed);
		ituMeterSetValue(projectSpeedMeter[i], 260 - speed);



	}
//#ifdef CFG_ENABLE_ROTATE	
	ituTextSetString(projectHTempText, "25");
	ituWidgetSetPosition(projectHTempPointIcon, TemperatureHPointX[0], TemperatureHPointY[0]);

	ituTextSetString(projectHFuelText, "0");
	ituWidgetSetPosition(projectHFuelPointIcon, FuelHPointX[0] + 10, FuelHPointY[0]);
//#endif	
	ituTextSetString(projectSpeedText[0], "0");
	ituTextSetString(projectSpeedText[1], "0");
//#ifdef CFG_ENABLE_ROTATE	
	ituTextSetString(projectHSpeedText, "0");
//#endif	
	ituTextSetString(projectODOText[0], "0000000000");
	ituTextSetString(projectODOText[1], "0000000000");
//#ifdef CFG_ENABLE_ROTATE	
	ituTextSetString(projectHODOText, "0000000000");
//#endif

	ituAnimationGoto(projectDirectionAnimation[0], 0);
	ituAnimationGoto(projectDirectionAnimation[1], 0);
	ituAnimationGoto(projectBottomAnimation, 0);
	ituAnimationGoto(projectFuelAnimation, 0);
	ituAnimationGoto(projectModeAnimation, 0);
	ituAnimationGoto(projectTempAnimation, 0);
	ituAnimationGoto(projectSpeedAnimation, 0);


	ituWidgetSetVisible(projectDirectionSprite[0][1], false);
	ituWidgetSetVisible(projectDirectionSprite[1][1], false);
	ituWidgetSetVisible(projectSpeedBackground1, false);
	ituWidgetSetVisible(projectQrcodeContainer, false);
#ifndef CFG_BTA_ENABLE
	ituSceneExecuteCommand(&theScene, 5, pPlayAnimation, NULL);
#endif
#ifdef CFG_VIDEO_ENABLE
	memset(&mtal_spec2, 0, sizeof(MTAL_SPEC));
#endif
    SceneEnterVideoState(33);
//#ifdef CFG_NET_WIFI_SDIO_NGPL        
//    int bIsAvail;
//    WifiMgr_Sta_Is_Available(&bIsAvail);
//    if(bIsAvail)
//    {
//#ifndef _WIN32
#if defined CFG_NET_ENABLE
        network_mirror_init();
#endif
//    }
//#endif     
    return true;
}

bool ProjectOnTimer(ITUWidget* widget, char* param)
{


	bool ret = true;
	uint32_t diff, tick = SDL_GetTicks();
	static int loop_cnt = 0;
	static uint32_t pre_diff = 0;

	int i = 0;


	if (tick >= lastTick)
		diff = tick - lastTick;
	else
		diff = 0xFFFFFFFF - lastTick + tick;

	// Text
	if (gShowTextMode){
		printf("Test_main gShowTextMode %d %d \n", gShowTextMode, gTextMode);
		showText(gTextMode);
		gShowTextMode = 0;
	}

	if (is_playing == false)
		pre_diff = diff;
	else
	{
		
		ret = ret | pModeSpriteDemo(diff);
		ret = ret | pGearSpriteDemo(diff);
		ret = ret | pDirectionSpriteDemo(diff);
		ret = ret | pSpeedDemo(loop_cnt);

		if ((diff - pre_diff) > 50) //50ms
		{
			int add = (diff - pre_diff) / 50;
			ret = ret | pTempDemo(add);
			ret = ret | pFuelDemo(add);
			pre_diff = diff;
		}

		ret = ret | pODODemo(loop_cnt);
	
	}

	loop_cnt++;
	if (loop_cnt > 1000)
		loop_cnt = 0;

	return ret;
}

bool ProjectOnLeave(ITUWidget* widget, char* param)
{
	printf("[project]%s\n", __FUNCTION__);
//#ifndef _WIN32
#if defined CFG_NET_ENABLE
    network_mirror_stop();
#endif    
	lastTick = 0;
	is_playing = false;

	showingPhoneContainer = false;
	ituWidgetSetVisible(projectPhoneContainer, false);
//#ifdef CFG_ENABLE_ROTATE
	showingHorizontalBackground = false;
	ituWidgetSetVisible(projectHorizontalBackground, false);
//#endif
	if (!openingBg)
	{
		openingBg = ituSceneFindWidget(&theScene, "openingBackground");
		assert(openingBg);
	}
	ituWidgetSetVisible(openingBg, true);

	//ituAnimationGoto(projectDirectionAnimation[0], 0);
	//ituAnimationGoto(projectDirectionAnimation[1], 0);
	//ituAnimationGoto(projectBottomAnimation, 0);
	//ituAnimationGoto(projectFuelAnimation, 0);
	//ituAnimationGoto(projectModeAnimation, 0);
	//ituAnimationGoto(projectTempAnimation, 0);
	//ituAnimationGoto(projectSpeedAnimation, 0);


	//ituWidgetSetVisible(projectREARIcon, true);


	//ituWidgetSetVisible(projectDirectionSprite[0][1], false);
	//ituWidgetSetVisible(projectDirectionSprite[1][1], false);
	//ituWidgetSetVisible(projectSpeedBackground1, false);

	//is_central_pos = true;
	//ituWidgetSetVisible(projectShowReverseButton, false);
	ituWidgetEnable(projectButtonBackground);

	ituWidgetSetVisible(projectReverseLAnimation, false);
	ituWidgetSetVisible(projectReverseRAnimation, false);
	ituWidgetSetVisible(projectReverseCAnimation, false);
	ituWidgetSetVisible(projectReverseIcon, false);
#ifndef _WIN32
	mtal_pb_stop();
	printf("1111\n");
	mtal_pb_exit();
	printf("2222\n");
#endif
	printf("3333\n");
    SceneLeaveVideoState();
	printf("4444\n");
	return true;
}

bool ProjectChangeUIBtOnMouseUp(ITUWidget* widget, char* param)
{
	char tmp[32];
	static char msg[256];
	if (is_central_pos == true)
	{
//#ifdef CFG_ENABLE_ROTATE		
		showingHorizontalBackground = false;
//#endif		
		is_playing = false;
		demo_loop = 0;

		ituWidgetSetVisible(projectDirectionSprite[0][1], true);
		ituWidgetSetVisible(projectDirectionSprite[1][1], true);
		ituWidgetSetVisible(projectSpeedBackground1, true);
		ituSpriteGoto(projectDirectionSprite[0][1], direction_frame);
		ituSpriteGoto(projectDirectionSprite[1][1], direction_frame);

		sprintf(tmp, "%d", temperature);
		//ituProgressBarSetValue(projectTempProgressBar[1], temperature);
		ituTextSetString(projectTempText[1], tmp);
		ituWidgetSetPosition(projectTempPointIcon[1], pTemperaturePointX[temperature - 25], pTemperaturePointY[temperature - 25]);

		sprintf(tmp, "%d", fuel);
		//ituProgressBarSetValue(projectFuelProgressBar[1], fuel);
		ituTextSetString(projectFuelText[1], tmp);
		ituWidgetSetPosition(projectFuelPointIcon[1], pFuelPointX[fuel], pFuelPointY[fuel]);


		sprintf(tmp, "%d", speed);
		//ituCircleProgressBarSetValue(projectSpeedCircleProgressBar[1], 260 - speed);
		ituMeterSetValue(projectSpeedMeter[1], 260 - speed);

		ituTextSetString(projectSpeedText[1], tmp);

		sprintf(tmp, "%d%d%d%d%d%d%d%d%d%d", odo, odo, odo, odo, odo, odo, odo, odo, odo, odo);
		ituTextSetString(projectODOText[1], tmp);

		ituAnimationPlay(projectFuelAnimation, 0);

		ituWidgetSetVisible(projectREARIcon, false);

		ituSceneExecuteCommand(&theScene, 5, pPlayAnimation, NULL);

		is_central_pos = false;

		//ituWidgetSetVisible(projectPhoneContainer, true);
		showingPhoneContainer = true;
		ituWidgetSetVisible(projectShowReverseButton, false);
		if (ituWidgetIsVisible(projectQrcodeContainer))
			ituWidgetSetVisible(projectQrcodeContainer, false);
#ifdef CFG_VIDEO_ENABLE
		ituWidgetSetDraw(projectPhoneContainer, PreviewVideoBackgroundDraw);
#endif
		ituWidgetGetGlobalPosition(projectPhoneContainer, &x, &y);
		width = ituWidgetGetWidth(projectPhoneContainer);
		height = ituWidgetGetHeight(projectPhoneContainer);
        ////ituWidgetSetVisible(projectHorizontalPhoneContainer, false);
        ////ituWidgetSetVisible(projectPhoneContainer, true);
#ifndef _WIN32
       itv_set_video_window(x, y, width, height);
#endif
	}
//#ifdef CFG_ENABLE_ROTATE	
	else if (!showingHorizontalBackground)
	{
		is_playing = false;

		ituWidgetSetVisible(projectHorizontalBackground, true);

		sprintf(tmp, "%d", temperature);
		ituTextSetString(projectHTempText, tmp);
		ituWidgetSetPosition(projectHTempPointIcon, TemperatureHPointX[temperature - 25] - 10, TemperatureHPointY[temperature - 25]);

		sprintf(tmp, "%d", fuel);
		ituTextSetString(projectHFuelText, tmp);
		ituWidgetSetPosition(projectHFuelPointIcon, FuelHPointX[fuel] + 10, FuelHPointY[fuel]);

		sprintf(tmp, "%d", speed);
		ituTextSetString(projectHSpeedText, tmp);

		sprintf(tmp, "%d%d%d%d%d%d%d%d%d%d", odo, odo, odo, odo, odo, odo, odo, odo, odo, odo);
		ituTextSetString(projectHODOText, tmp);

		ituSceneExecuteCommand(&theScene, 5, pPlayAnimation, NULL);

		showingPhoneContainer = false;
		showingHorizontalBackground = true;

		//210322 ituWidgetSetVisible(projectHorizontalPhoneContainer, true);
		ituWidgetSetVisible(projectShowReverseButton, true);
		
#ifdef CFG_VIDEO_ENABLE
		ituWidgetSetDraw(projectHorizontalPhoneContainer, PreviewVideoBackgroundDraw);
#endif
		ituWidgetGetGlobalPosition(projectHorizontalPhoneContainer, &x, &y);
		width = ituWidgetGetWidth(projectHorizontalPhoneContainer);
		height = ituWidgetGetHeight(projectHorizontalPhoneContainer);
        ////ituWidgetSetVisible(projectPhoneContainer, false);
        ////ituWidgetSetVisible(projectHorizontalPhoneContainer, true);
#ifndef _WIN32
        itv_set_video_window(x, y, width, height);
#endif
	}
//#endif	
	else
	{
		ituWidgetSetVisible(projectShowReverseButton, false);
//#ifdef CFG_ENABLE_ROTATE		
		showingHorizontalBackground = false;
		ituWidgetSetVisible(projectHorizontalBackground, false);
//#endif
		is_playing = false;

		ituSpriteGoto(projectDirectionSprite[0][0], direction_frame);
		ituSpriteGoto(projectDirectionSprite[1][0], direction_frame);

		sprintf(tmp, "%d", temperature);
		//ituProgressBarSetValue(projectTempProgressBar[0], temperature);
		ituTextSetString(projectTempText[0], tmp);
		ituWidgetSetPosition(projectTempPointIcon[0], pTemperaturePointX[temperature - 25], pTemperaturePointY[temperature - 25]);

		sprintf(tmp, "%d", fuel);
		//ituProgressBarSetValue(projectFuelProgressBar[0], fuel);
		ituTextSetString(projectFuelText[0], tmp);
		ituWidgetSetPosition(projectFuelPointIcon[0], pFuelPointX[fuel], pFuelPointY[fuel]);

		sprintf(tmp, "%d", speed);
		//ituCircleProgressBarSetValue(projectSpeedCircleProgressBar[0], 260 - speed);
		ituMeterSetValue(projectSpeedMeter[0], 260 - speed);

		ituTextSetString(projectSpeedText[0], tmp);

		sprintf(tmp, "%d%d%d%d%d%d%d%d%d%d", odo, odo, odo, odo, odo, odo, odo, odo, odo, odo);
		ituTextSetString(projectODOText[0], tmp);

		ituAnimationGoto(projectDirectionAnimation[0], 0);
		ituAnimationGoto(projectDirectionAnimation[1], 0);
		ituAnimationGoto(projectBottomAnimation, 0);
		ituAnimationGoto(projectFuelAnimation, 0);
		ituAnimationGoto(projectModeAnimation, 0);
		ituAnimationGoto(projectTempAnimation, 0);
		ituAnimationGoto(projectSpeedAnimation, 0);

		ituSceneExecuteCommand(&theScene, 5, pPlayAnimation, NULL);

	
		ituWidgetSetVisible(projectREARIcon, true);


		ituWidgetSetVisible(projectDirectionSprite[0][1], false);
		ituWidgetSetVisible(projectDirectionSprite[1][1], false);
		ituWidgetSetVisible(projectSpeedBackground1, false);

		is_central_pos = true;


		ituWidgetSetVisible(projectPhoneContainer, false);

		showingPhoneContainer = false;

	}
	

	return true;
}

bool ProjectTempAnOnStopped(ITUWidget* widget, char* param)
{
	printf("ProjectTempAnOnStopped showingPhoneContainer %d\n", showingPhoneContainer);

	if (showingPhoneContainer)
	{
		ituWidgetSetVisible(projectPhoneContainer, true);
		printf("SHOW SHOW !!\n");
		ituWidgetSetVisible(projectHorizontalPhoneContainer, true);
	}
	
	is_playing = true;
	return true;
}

bool ProjectPlayBtOnMouseUp(ITUWidget* widget, char* param)
{
	if (is_playing == false)
	{
		demo_loop = 0;
		is_playing = true;
	}

	return true;
}
/*===================================
fill color into Rectangle  color = 1 black , color = 0 white
====================================*/
static void Fill_Rect(uint16_t* dest, int w, Rect rect, bool color)
{
	int i, j = 0;

	for (i = rect.x; i < rect.x + rect.size; i++)
	{
		for (j = rect.y; j < rect.y + rect.size; j++)
		{
			if (color)
				dest[i + j * w] = ITH_RGB565(0, 0, 0);//black
			else
				dest[i + j * w] = ITH_RGB565(255, 255, 255);//white        
		}
	}
}
/*===================================
fill color into Rectangle  color = 1 black , color = 0 white
====================================*/
static void Fill_Rect_32(uint32_t* dest, int w, Rect rect, bool color)
{
	int i, j = 0;

	for (i = rect.x; i < rect.x + rect.size; i++)
	{
		for (j = rect.y; j < rect.y + rect.size; j++)
		{
			if (color)
				dest[i + j * w] = ITH_ARGB8888(255, 0, 0, 0);
			else
				dest[i + j * w] = ITH_ARGB8888(255, 255, 255, 255);
		}
	}
}
/*===================================
Direct show QR encdoe result
You can call Draw_QRcode to display the QR Code on icon
format :RGB565 or ARGB8888
====================================*/
static void Draw_QRcode(QRcode * content, ITUIcon* icon, ITUPixelFormat format)
{
	ITUSurface* surf = NULL;
	int size = QRCODE_SIZE; // scale up size
	int x, y, w, h;
	int scale_up_w;
	Rect rect;
	w = h = (content->width);
	scale_up_w = w * size;
	surf = ituCreateSurface(scale_up_w, scale_up_w, 0, format, NULL, 0);

	if (format == ITU_RGB565)
	{
		uint16_t* dest = (uint16_t*)ituLockSurface(surf, 0, 0, scale_up_w, scale_up_w);
		assert(dest);

		for (y = 0; y < h; y++)
		{
			for (x = 0; x < w; x++)
			{
				rect.x = x * size;
				rect.y = y * size;
				rect.size = size;
				Fill_Rect(dest, scale_up_w, rect, (content->data[x + y * w] & 0x1));
			}
		}
	}
	else
	{
		uint32_t* dest = (uint32_t*)ituLockSurface(surf, 0, 0, scale_up_w, scale_up_w);
		assert(dest);

		for (y = 0; y < h; y++)
		{
			for (x = 0; x < w; x++)
			{
				rect.x = x * size;
				rect.y = y * size;
				rect.size = size;
				Fill_Rect_32(dest, scale_up_w, rect, (content->data[x + y * w] & 0x1));
			}
		}
	}
	ituUnlockSurface(surf);

	icon->loadedSurf = surf;
	icon->widget.flags |= ITU_LOADED;
	free(content);
}

int showText(int nMode)
{
	char msg_wifi[256];

	if (nMode == 0){
		strcpy(msg_wifi, "Cell phone is connected, switch to WiFi client mode.Connecting...");
		ituTextSetString(projectConnectTextBox, msg_wifi);
	}
	else if (nMode == 1){
#ifdef CFG_NET_WIFI_SDIO_NGPL
	    unsigned char *ip_r = LwIP_GetIP(&xnetif[0]);
	    unsigned char *mac = LwIP_GetMAC(&xnetif[0]);
        char ip[16] = {0};
        ipaddr_ntoa_r((const ip_addr_t*)ip_r, ip, sizeof(ip));
        sprintf(msg_wifi, "ITE connected with IP:%s, ready to WIFI mirror.", ip);        
#endif

#if defined(CFG_NET_WIFI_SDIO_NGPL_AP6256) || defined(CFG_NET_WIFI_SDIO_NGPL_AP6236) || defined(CFG_NET_WIFI_SDIO_NGPL_AP6181)
                
                unsigned char cMac [6];
                uint32_t nIp;

                WifiMgr_Get_MAC_Address(cMac);                                
                
                nIp = htonl(mhd_sta_ipv4_ipaddr());
                
                sprintf(msg_wifi, "ITE connected with IP:%u.%u.%u.%u, ready to WIFI mirror.",FORMAT_IPADDR(nIp));        
#endif


		ituTextSetString(projectConnectTextBox, msg_wifi);
	}
	else if (nMode == 2){
		strcpy(msg_wifi, "Cell phone is connected, switch to WiFi client mode.");
		ituTextSetString(projectConnectTextBox, msg_wifi);
    }
	ituWidgetSetVisible(projectConnectTextBox, true);
}

bool ProjectWiFiButtonOnPress(ITUWidget* widget, char* param)
{
	printf("[project]%s\n", __FUNCTION__);
	static char msg[256];
    unsigned char cMac [6];
    char buf[32];

    buf[0] = '\0';

	//qrcode
	strcpy(msg, QRCODE_TEXT);
//#ifndef _WIN32
#if defined CFG_NET_ENABLE
    WifiMgr_Get_MAC_Address(cMac);
#endif
    sleep(1);

    buf[0] = '\0';
    sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X\n",
        (uint8_t)cMac[0],
        (uint8_t)cMac[1],
        (uint8_t)cMac[2],
        (uint8_t)cMac[3],
        (uint8_t)cMac[4],
        (uint8_t)cMac[5]);
    strcat(msg,buf);
    //printf("[project]%s %d  %s\n",msg,strlen(msg), __FUNCTION__);
    //    strcat(QRCODE_TEXT,cMac);    
	//Draw_QRcode(encode(QRCODE_TEXT, strlen(QRCODE_TEXT)), projectQrcodeIcon, ITU_RGB565);
	Draw_QRcode(encode(msg, strlen(msg)), projectQrcodeIcon, ITU_RGB565);
//#ifndef _WIN32
#if defined CFG_NET_ENABLE
    QRCode_SoftAP();
#endif
	ituWidgetSetVisible(projectQrcodeContainer, true);

	

	return true;
}

void show_mirror_window(int w, int h)
{
    if(w > h)
    {
        //ituWidgetSetDraw(projectHorizontalPhoneContainer, PreviewVideoBackgroundDraw);
        //ituWidgetGetGlobalPosition(projectHorizontalPhoneContainer, &x, &y);
        //width = ituWidgetGetWidth(projectHorizontalPhoneContainer);
        //height = ituWidgetGetHeight(projectHorizontalPhoneContainer);
        is_central_pos = false;
//#ifdef CFG_ENABLE_ROTATE        
        showingHorizontalBackground = false;
//#endif        
		ProjectChangeUIBtOnMouseUp(projectChangeUIButton, NULL);
    }else
    {
        //ituWidgetSetDraw(projectPhoneContainer, PreviewVideoBackgroundDraw);
        //ituWidgetGetGlobalPosition(projectPhoneContainer, &x, &y);
        //width = ituWidgetGetWidth(projectPhoneContainer);
        //height = ituWidgetGetHeight(projectPhoneContainer);
        if(is_central_pos != true)
			ProjectChangeUIBtOnMouseUp(projectChangeUIButton, NULL);
        is_central_pos = true;
        //showingPhoneContainer = true;
		ProjectChangeUIBtOnMouseUp(projectChangeUIButton, NULL);
    }
    //itv_set_video_window(x, y, width, height);
    //itv_set_pb_mode(1);
}

bool ProjectShowReverseBtOnMouseUp(ITUWidget* widget, char* param)
{
	char tmp[32];
#if defined CFG_NET_ENABLE
	network_mirror_stop();
#endif 
	ituWidgetDisable(projectButtonBackground);
	
	ituWidgetSetVisible(projectReverseIcon, false);
	ituWidgetSetVisible(projectReverseCAnimation, false);

	is_playing = false;

	ituWidgetSetVisible(projectHorizontalBackground, true);

	sprintf(tmp, "%d", temperature);
	ituTextSetString(projectHTempText, tmp);
	ituWidgetSetPosition(projectHTempPointIcon, TemperatureHPointX[temperature - 25] - 10, TemperatureHPointY[temperature - 25]);

	sprintf(tmp, "%d", fuel);
	ituTextSetString(projectHFuelText, tmp);
	ituWidgetSetPosition(projectHFuelPointIcon, FuelHPointX[fuel] + 10, FuelHPointY[fuel]);

	sprintf(tmp, "%d", speed);
	ituTextSetString(projectHSpeedText, tmp);

	sprintf(tmp, "%d%d%d%d%d%d%d%d%d%d", odo, odo, odo, odo, odo, odo, odo, odo, odo, odo);
	ituTextSetString(projectHODOText, tmp);

	ituSceneExecuteCommand(&theScene, 5, pPlayAnimation, NULL);

	is_central_pos = false;
	//showingPhoneContainer = false;
	showingHorizontalBackground = true;


//#ifdef CFG_ENABLE_ROTATE		
	ituWidgetSetVisible(projectHorizontalPhoneContainer, false);
//#endif
	ituWidgetSetVisible(projectPhoneContainer, false);
	showingPhoneContainer = false;


#ifdef CFG_VIDEO_ENABLE
	ituWidgetSetDraw(projectReversingViewContainer, PreviewVideoBackgroundDraw);
#endif
	ituWidgetGetGlobalPosition(projectReversingViewContainer, &x, &y);
	width = ituWidgetGetWidth(projectReversingViewContainer);
	height = ituWidgetGetHeight(projectReversingViewContainer);
	////ituWidgetSetVisible(projectHorizontalPhoneContainer, false);
	////ituWidgetSetVisible(projectReversingViewContainer, true);
#ifdef CFG_VIDEO_ENABLE
	itv_set_video_window(x, y, width, height);

	mtal_pb_init(EventHandler);
	//strcpy(mtal_spec.srcname, CFG_PUBLIC_DRIVE ":/" BOOT_VIDEO);

	strcpy(mtal_spec2.srcname, Tfile);
	printf("mtal_pb_select_file(%s)\n", Tfile);
	mtal_pb_select_file(&mtal_spec2);
	mtal_pb_play();
#endif

	ituSceneExecuteCommand(&theScene, 10, pShowReversing, NULL);//for sync with video

	return true;
}

bool projectGoHomeBtOnMouseUp(ITUWidget* widget, char* param)
{
	/*ituWidgetSetVisible(projectReverseLAnimation, false);
	ituWidgetSetVisible(projectReverseRAnimation, false);
	ituWidgetSetVisible(projectReverseCAnimation, false);
	ituWidgetSetVisible(projectReverseIcon, false);
#ifndef _WIN32
	mtal_pb_stop();
	mtal_pb_exit();
#endif*/
	return true;
}
