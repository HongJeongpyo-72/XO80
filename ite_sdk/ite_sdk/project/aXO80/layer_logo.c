#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"
#include "HT_DispDdefine.h"
#include "ite/itp.h"


#define MAX_PW_CNT	9
#ifdef RELEASE_VER
	#ifdef TEST_ITU_USB
		#define LOGO_FIRM_VER	"Ver 01-25_ITU"
	#else
		#define LOGO_FIRM_VER	"Ver 08.8_Boot2(LCD)"
	#endif
#else
	#define LOGO_FIRM_VER	"Ver 01-15=>Test_1 "
#endif
static ITUIcon* UserPW_Sel[12];
static ITUIcon* UserPW_Bt[12];
static ITUIcon* UserPW_Data[MAX_PW_CNT];
static ITUIcon* UserPWNG;
static ITUIcon* UserPWPass;
static ITUIcon* UserPW_Bt_Toggle;
//static ITUIcon* UserPW_Back_Off;
static ITUBackground* logo_Black;

static ITUText* UserPW_Firm;

static ITULayer* logoLayer=NULL;

static uint8_t tCamSetFlag = 0;

uint8_t ChangeScene_finished=0;


static bool LayerFlag = 0;
static char widgetName[30];
uint8_t LogoPW[30]={0,};
uint8_t LogoPW_Index=0;
uint32_t LogoPW_Data = 0;
bool Finish_LogoBoot=1;
uint8_t LogoPWSelIndex=0xFF;

uint32_t UserPW_Back_Off_time=0; 
uint32_t UserPW_Reply_time = 0; 
uint8_t UserPW_PosIndex=0xFF; 
uint8_t CamSetFlag = 0;

uint8_t LogoPW_ResultFlag=0;	// 1:pass , 2:NG

uint16_t UserPW_Pos[10][2] = { 
						{ 	 45,  726 }, 
						{ 	285,  726 }, 
						{   525,  726 }, 
						{    45,  862 }, 
						{   285,  862 }, 
						{   525,  862 }, 
						{ 	 45,  996 },
						{   285,  996 }, 
						{   525,  996 }, 
						{   285, 1132 }, 
};
 
uint16_t UserPW_Pos_H[10][2] = {
					{   590, 140 },
					{   820, 140 },
					{  1050, 140 },
					{   590, 275 },
					{   820, 275 },
					{  1050, 275 },
					{   590, 410 },
					{   820, 410 },
					{  1050, 410 },
					{   820, 545 },
};


bool LogoPW_Disp(void);
bool LogoLayerOneTime = 0;

void GotoLogo(void)
{
    logoLayer = ituSceneFindWidget(&theScene, "logoLayer");
    assert(logoLayer);
	printf("Go Logo : 0x%X,%d\n",logoLayer,LayerFlag);
	UserPW_Time_UA2 = gMainTimerCnt_10m;
	UserPW_Time_AUI1 = gMainTimerCnt_10m;	
	ChangeScene_finished=0;
	LayerFlag = 0;
	memset (LogoPW,0,sizeof(LogoPW));
	LogoPW_Index=0;
	LogoPW_Data = 0;
	LogoPWSelIndex=0xFF;
	LogoPWSelIndex=0;
	UserPW_Back_Off_time=0; 
	UserPW_Reply_time = 0; 
	UserPW_PosIndex=0xFF; 
	MainBtFlag=0;
    ituLayerGoto(logoLayer);
}

bool LogoPW_DataDispClear(void)
{
	printf("LogoPW_DataDispClear : %d,%d\n",LogoPW_Index,UserPW_PassFlag);
	memset (LogoPW,0,sizeof(LogoPW));
	LogoPW_Index=0;
	LogoPW_Disp();
//	UserPW_PassFlag=0;
}

bool LogoPW_Bt_OnOff(DISP_MODE OnOff,uint8_t BtIndex)
{
	uint8_t i;
	
	if (g_nM7_StartType!=M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
		return true;
	for (i=0;i<12;i++)
	{
		ituWidgetSetVisible(UserPW_Bt[i], OnOff);
	}
	if (BtIndex!=0xFF)
		ituWidgetSetVisible(UserPW_Bt[11], DISP_ON);
	
	return 1;
}

bool LogoPW_Sel(uint8_t SelIndex)
{
	char i;
	#ifdef TEST_ROTATION
		return true;
	#endif
	if (g_nM7_StartType!=M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
		return true;
	if (LayerFlag)
	{
		for (i=0;i<12;i++)
		{
			ituWidgetSetVisible(UserPW_Sel[i], DISP_OFF);
		}
		if ((SelIndex>=0)&&(SelIndex<12))
		{
			ituWidgetSetVisible(UserPW_Sel[SelIndex], DISP_ON);
			LogoPWSelIndex = SelIndex;
		}
	}
}
bool LogoPW_Sel_Run(uint8_t SelIndex)
{
	#ifdef TEST_ROTATION
		return true;
	#endif
	if (g_nM7_StartType!=M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
		return true;
	if (LayerFlag)
	{
		switch (SelIndex)
		{
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 0:
			{
				int x, y;
				
				if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
				{
					x= UserPW_Pos_H[SelIndex][0];
					y= UserPW_Pos_H[SelIndex][1];
				}
				else
				{
					x= UserPW_Pos[SelIndex][0];
					y= UserPW_Pos[SelIndex][1];
				}
		
			
				UserPW_PosIndex = SelIndex;
			}
			break;
			default :
			{
				UserPW_PosIndex = 0xFF;
			}
			break;
		}
		LogoPWSelIndex = SelIndex;
	}
	return 1;
}
bool LogoPW_Disp(void)
{
	char i;
	#ifdef TEST_ROTATION
		return true;
	#endif
	if (g_nM7_StartType!=M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
		return true;
	if (LayerFlag)
	{
		for (i=0;i<MAX_PW_CNT;i++)
		{
			ituWidgetSetVisible(UserPW_Data[i], DISP_OFF);
		}
		for (i=0;i<LogoPW_Index;i++)
		{
			ituWidgetSetVisible(UserPW_Data[i], DISP_ON);
		}
	}
	return 1;
}
uint8_t gLogoPW_D7=0xFF;
extern bool XO_ituButtonSetMode_Bt(bool SetMode);
extern bool XO_ituButtonSetMode_TP(bool SetMode);
extern volatile uint8_t ConnectCan;

bool LogoOnEnter(ITUWidget* widget, char* param)
{
	int j,i=0;
	printf("\n\nStart LogoOnEnter=[%s],[%d],[%d], LogoPWSelIndex=[%d][%d]\n\n",param,LayerFlag,Main_Disp_Mode,LogoPWSelIndex,g_nM7_StartType);
	LogoPWSelIndex = 0; //chpark.24.05.09
	
	gLogoPW_D7=0xFF;
	LogoPW_ResultFlag = 0;
	ResetRotation();
	if (tCamSetFlag==0)
	{
		if (ConnectCan>2)
		{
			static uint8_t SendFlag = 0;
			if (SendFlag==0)
			{
				uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};		// 231214 jhlee
				Finish_LogoBoot=1;
				SendFlag = 1;
				txbuffer[0] = 0xDA;
				txbuffer[1] = 0xFE;
				txbuffer[2] = 0x00;
				
				CanbusTxDataManagement( gMainTimerCnt_10m,PGN_REQ1,txbuffer);			// 231214 jhlee
				
				
				// CAN TX CAN[PGN_REQ1 0x18EAE4CE ; GFI :0x1850EEE4] : EE 50 00 FF FF FF FF FF FF (PGN50EE)
				txbuffer[0] = 0xEE;
				txbuffer[1] = 0x50;
				txbuffer[2] = 0x00;
				
				#if 0
					PushTxData (0x18503FE4,txbuffer,8);
				#else
					CanbusTxDataManagement( gMainTimerCnt_10m,PGN_REQ1,txbuffer);
				#endif
//				CanbusTxDataManagement( gMainTimerCnt_10m,XO_MICOM_SOFT_REQ,NULL);
			}
		}
		SettingInit();		// JPHong 2024-06-20
		#ifdef USED_SAVE_CONFIG
			Flash_Init();
		#endif
		G_Sensor_Thread_Init();
	
	//	GotoCamera(5);
		
		tCamSetFlag = 1;
	}
	printf("Check LogoOnEnter=[%s],[%d],[%d], LogoPWSelIndex=[%d][%d]\n\n",param,LayerFlag,Main_Disp_Mode,LogoPWSelIndex,g_nM7_StartType);
	if (g_nM7_StartType!=M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
	{
		if ((LayerFlag==0)||(Main_Disp_Mode==MAIN_MODE_PW))
		{

			gBuzzerMode = 0;
			if (LogoLayerOneTime ==  0)
			{
				HT_timeofday_mutex();
				LogoLayerOneTime = 1;
			}
			RotationMode=0;
			LayerFlag = 1;
			ScreenSetOnOff(1);
			
			XO_ituButtonSetMode_Bt(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
			XO_ituButtonSetMode_TP(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
			
			if (Main_DayNight==0)
				LCD_BackLight(0);	// 0:Day , 1:Night
			else
				LCD_BackLight(1);
			UserPW_PassFlag=0;
			Finish_LogoBoot=1;
		
			
			ChangeScene_finished=0;
			
			
			UserPW_Time_UA2 = gMainTimerCnt_10m;
			UserPW_Time_AUI1 = gMainTimerCnt_10m;
					
			UserPW_PassFlag=0;
			if (Main_Disp_Mode==MAIN_MODE_ENGN_COOL)
			{
				GotoEngnCool();
			}
			else if (Main_Disp_Mode==MAIN_MODE_IMMOBI) //chpark.24.10.30
			{
				printf("[START][Immobi][1]==\n");
				GotoBSImmobilizer();
			}
			else if (g_TCI_0B==0)
				GotoMain(MAIN_MODE_USERINFO);
			LayerFlag = 1;		
		}
		#ifdef USED_ROTATION
		else
		{
			logoLayer=NULL;
			printf("LogoOnRotation=%d %d\n",Main_Disp_Mode,logoLayer);
			SetWidget(logo_Black,"logo_Black");
			SetWidget(logoLayer,"logoLayer");
			ituWidgetSetVisible(logoLayer,DISP_ON);
			{
				CamSetFlag = 1;
				GotoCamera(5);
			}
					
			if (Main_Disp_Mode==MAIN_MODE_MENU)
			{
				GotoMenuTop();
			}
			else if (Main_Disp_Mode==MAIN_MODE_BACK)
			{
				printf("[START][Back][1]==\n");
				GotoBackScreenT(true);
			}
			else if (Main_Disp_Mode==MAIN_MODE_ENGN_COOL)
			{
				GotoEngnCool();
			}
			else if (Main_Disp_Mode==MAIN_MODE_IMMOBI) //chpark.24.10.30
			{
				printf("[START][Immobi][1]==\n");
				GotoBSImmobilizer();
			}
			else
			{
			
printf("GotoMainReal  1 \n" );			
				GotoMainReal(Main_Disp_Mode);
			}
		}
		#endif
		return true;
	}
	if ((LayerFlag==0)||(Main_Disp_Mode==MAIN_MODE_PW))
	{
		Main_Disp_Mode=MAIN_MODE_PW;
		gBuzzerMode = 0;
		SetWidget(logo_Black,"logo_Black");
		for (i=0;i<12;i++)
		{
			sprintf(widgetName, "UserPW_Sel%02d",i+1);
			SetWidget(UserPW_Sel[i],widgetName);
			sprintf(widgetName, "UserPW_Bt%02d",i+1);
			SetWidget(UserPW_Bt[i],widgetName);
			ituWidgetSetVisible(UserPW_Bt[i], DISP_ON);
		}
		SetWidget(UserPWNG,"UserPWNG");
		SetWidget(UserPWPass,"UserPWPass");
		
		for (i=0;i<MAX_PW_CNT;i++)
		{
			sprintf(widgetName, "UserPW_Data%d",i+1);
			SetWidget(UserPW_Data[i],widgetName);
		}
		
		SetWidget(UserPW_Firm,"UserPW_Firm");
		{
			char tmp[32]={0,};
			sprintf(tmp, LOGO_FIRM_VER);
			ituTextSetString(UserPW_Firm, tmp);
			ituWidgetSetVisible(UserPW_Firm, DISP_OFF);
			//ituWidgetSetVisible(UserPW_Firm, DISP_ON);
		}

		if (LogoLayerOneTime ==  0)
		{
			HT_timeofday_mutex();
			LogoLayerOneTime = 1;
		}
		else
		{
			LogoPW_Disp();
			LogoPW_Sel(LogoPWSelIndex);
			LogoPW_Sel_Run(LogoPWSelIndex);
		}
		RotationMode=0;
		LayerFlag = 1;
		ScreenSetOnOff(1);
		
		XO_ituButtonSetMode_Bt(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
		XO_ituButtonSetMode_TP(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
		SetWidget(logoLayer,"logoLayer");
		ituWidgetSetVisible(logoLayer, DISP_ON);
		
		if (Main_DayNight==0)
			LCD_BackLight(0);	// 0:Day , 1:Night
		else
			LCD_BackLight(1);
		UserPW_PassFlag=0;
		Finish_LogoBoot=1;
	
		
		ChangeScene_finished=0;
		
		
		UserPW_Time_UA2 = gMainTimerCnt_10m;
		UserPW_Time_AUI1 = gMainTimerCnt_10m;
		LogoPW_Disp();
		LogoPW_Bt_OnOff(DISP_ON,0xFF);
		
		//chpark.24.04.17
		ituWidgetSetVisible(UserPWPass, DISP_OFF);
		ituWidgetSetVisible(UserPWNG, DISP_OFF);
		//

		//chpark.24.05.09
		if (LayerFlag ==  1)
		{		
			LogoPW_Disp();
			LogoPW_Sel(LogoPWSelIndex);
			LogoPW_Sel_Run(LogoPWSelIndex);
		}
		ituWidgetSetVisible(logo_Black,DISP_OFF);
	}
	#ifdef USED_ROTATION
	else
	{
		logoLayer=NULL;
		printf("LogoOnRotation=%d %d\n",Main_Disp_Mode,logoLayer);
		SetWidget(logo_Black,"logo_Black");
		SetWidget(logoLayer,"logoLayer");
		ituWidgetSetVisible(logoLayer,DISP_ON);
		
		{
			CamSetFlag = 1;
			GotoCamera(5);
		}
				
		if (Main_Disp_Mode==MAIN_MODE_MENU)
		{
			GotoMenuTop();
		}
		else if (Main_Disp_Mode==MAIN_MODE_BACK)
		{
			printf("[START][Back][2]==\n");
			GotoBackScreenT(true);
		}
		else if (Main_Disp_Mode==MAIN_MODE_ENGN_COOL)
		{
			GotoEngnCool();
		}
		else if (Main_Disp_Mode==MAIN_MODE_IMMOBI) //chpark.24.10.30
		{
			printf("[START][Immobi][2]==\n");
			GotoBSImmobilizer();
		}
		else
		{
printf("GotoMainReal  2 \n" );	
			GotoMainReal(Main_Disp_Mode);
		}
	}
	#endif

	printf("\nEnd LogoOnEnter=[%s],[%d],[%d], LogoPWSelIndex=%d [%d]\n\n",param,LayerFlag,Main_Disp_Mode,LogoPWSelIndex,g_nM7_StartType);
    return true;
}
bool LogoOnTimer(ITUWidget* widget, char* param)
{
	static uint32_t OnTimerCnt_33ms = 0;
	static uint32_t OnTimerCnt_100ms = 0;
	static uint32_t OnTimerCnt_300ms = 0;
	static uint32_t OnTimerCnt_500ms = 0;
	static uint32_t OnTimerCnt_600ms = 0;
	static uint32_t OnTimerCnt_1S = 0;
	static uint32_t OnTimerCnt_5S = 0,iCON_CNT=0,i;
	static uint8_t tUserPW_PassFlag = 0xFF;
	static uint32_t tUserPW_Time = 0;

	OnTimerCnt_33ms++;
	
	if (g_nM7_StartType!=M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
		return true;
	#ifdef TEST_ROTATION
		return true;
	#endif
#ifdef USED_ROTATION				//JPHong 231022
	if(RotationMode!=0)
	{
		return 0;
	}
#endif



	if (((UserPW_Time_AUI1+TIME_CNT_4_5SEC)<gMainTimerCnt_10m)&&(tCamSetFlag==1))
	{
		UserPW_PassFlag=0;
		GotoMain(MAIN_MODE_NORMAL);
	}
	if (LayerFlag)
	{		
//		printf("UserPW_Reply_time:[%d][%d][%d]\n",UserPW_Reply_time,UserPW_PassFlag,gMainTimerCnt_10m);
		
		
		if ((UserPW_Reply_time!=0)&&((UserPW_Reply_time+TIME_CNT_5SEC)<gMainTimerCnt_10m))
		{
			UserPW_PassFlag = 3;
			UserPW_Reply_time = 0;
			UserPW_Time_UA2 = gMainTimerCnt_10m;
			gBuzzerMode = 12; //ON(300)OFF(300) 3Time
		}
		if (UserPW_PassFlag==0)
		{
			tUserPW_PassFlag = UserPW_PassFlag;
			tUserPW_Time = UserPW_Time_UA2;
			LogoPW_ResultFlag=0;	// 1:pass , 2:NG
//			ituWidgetSetVisible(UserPWPass, DISP_OFF);
//			ituWidgetSetVisible(UserPWNG, DISP_OFF);
		}
		else if (tUserPW_PassFlag == UserPW_PassFlag)
		{
			if (UserPW_PassFlag==1)
			{
				if (((tUserPW_Time+TIME_CNT_1_2SEC)<gMainTimerCnt_10m)&&(tCamSetFlag==1))
				{
					LogoPW_ResultFlag=0;	// 1:pass , 2:NG
					LogoPW_Bt_OnOff(DISP_ON,0xFF);
					tUserPW_PassFlag = 0xFF; //chpark.24.04.17
					#ifdef TEST_FLASH_MEM
						GotoMain(MAIN_MODE_NORMAL);
					#else
						GotoMain(MAIN_MODE_USERINFO);// 2024-02-04 JPHong
					#endif
				}			
			}
			else
			{
				if ((tUserPW_Time+TIME_CNT_2SEC)<gMainTimerCnt_10m)
				{
					ituWidgetSetVisible(UserPWPass, DISP_OFF);
					ituWidgetSetVisible(UserPWNG, DISP_OFF);
					LogoPW_ResultFlag=0;	// 1:pass , 2:NG
					LogoPW_Bt_OnOff(DISP_ON,0xFF);
					tUserPW_PassFlag = 0xFF; //chpark.24.04.17
					gLogoPW_D7=0xFF;//chpark.24.04.17
					LogoPW_DataDispClear();
					UserPW_PassFlag=0;					
				}
			}
		}
		else
		{
			printf("UserPW_PassFlag:[%d]\n",UserPW_PassFlag);
			tUserPW_PassFlag = UserPW_PassFlag;
			tUserPW_Time = gMainTimerCnt_10m;
			if (UserPW_PassFlag==1)
			{
				LogoPW_ResultFlag = 1;	// 1:pass , 2:NG
				LogoPW_Bt_OnOff(DISP_OFF,11);
				ituWidgetSetVisible(UserPWPass, DISP_ON);
				
			}
			else
			{	
				LogoPW_DataDispClear();
				LogoPW_ResultFlag = 2;	// 1:pass , 2:NG
				LogoPW_Bt_OnOff(DISP_OFF,0xFF);
				ituWidgetSetVisible(UserPWNG, DISP_ON);
				
			}
		}
//		if ((UserPW_Back_Off_time+TIME_CNT_200MS)<gMainTimerCnt_10m)
//			ituWidgetSetVisible(UserPW_Back_Off, DISP_OFF);
		

	}
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_100MS)==0)	// 100ms
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_100ms++;
		
#if 0		
		if ((tCamSetFlag==0)&&(OnTimerCnt_100ms>3))
		{
			SettingInit();		// JPHong 2024-06-20
			#ifdef USED_SAVE_CONFIG
				Flash_Init();
			#endif
			G_Sensor_Thread_Init();
		
//			GotoCamera(5);
			
			tCamSetFlag = 1;
		}
#endif		
	}
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_300MS)==0)	// 300ms
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_300ms++;
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
		#ifdef TEST_FLASH_MEM
			M7_UserIndex = 0;
			UserPW_PassFlag = 1;
		#endif		
	}
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_5000MS)==0)
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_5S++;
	}
    return true;
}

bool LogoOnLeave(ITUWidget* widget, char* param)
{
//	XO_ituButtonSetMode_Bt(0);
//	XO_ituButtonSetMode_TP(0);
	
	UserPW_PosIndex=0xff;
			
				
	if (logoLayer!=NULL)
		ituWidgetSetVisible(logoLayer, DISP_OFF);
		
//		LayerFlag=0;
    return true;
}


bool LogoJDR(ITUWidget* widget, char* param)
{
	uint8_t JogData=0,i;
	if (LayerFlag==0)
		return 0;
	if (g_nM7_StartType!=M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
		return true;
#ifdef USED_ROTATION				//JPHong 231022
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	if (LogoPW_ResultFlag)	// 2024-03-09 JPHong V14
		return 0;
	JogData = param[0];
	printf("LogoJDR= '0x%X' %d\n",JogData,LogoPWSelIndex);
	if ((LogoPWSelIndex<0)||(LogoPWSelIndex>11))
	{
		LogoPWSelIndex = 0;
		LogoPW_Sel(LogoPWSelIndex);
		LogoPW_Sel_Run(LogoPWSelIndex);
	}
	else
	{
		switch (JogData)
		{
			case JOG_DIAL_ROT_RIGHT:
			{
				switch (LogoPWSelIndex)
				{
					case 8:
						LogoPWSelIndex = 10;
					break;
					case 9:
						LogoPWSelIndex = 11;
					break;
					case 10:
						LogoPWSelIndex = 9;
					break;
					case 11:
						LogoPWSelIndex = 0;
					break;
					default:
						LogoPWSelIndex = LogoPWSelIndex+1;
					break;
				}
				LogoPW_Sel(LogoPWSelIndex);
				LogoPW_Sel_Run(LogoPWSelIndex);
			}
			break;
			case JOG_DIAL_ROT_LEFT:
			{
				switch (LogoPWSelIndex)
				{
					case 0:
						LogoPWSelIndex = 11;
					break;
					case 9:
						LogoPWSelIndex = 10;
					break;
					case 10:
						LogoPWSelIndex = 8;
					break;
					case 11:
						LogoPWSelIndex = 9;
					break;
					default:
						LogoPWSelIndex = LogoPWSelIndex-1;
					break;
				}
				LogoPW_Sel(LogoPWSelIndex);
				LogoPW_Sel_Run(LogoPWSelIndex);
			}
			break;	
		}
	}
    return true;
}
bool USER_PW_UpBt_Func(ITUWidget* widget, char* param);
bool LogoJDK(ITUWidget* widget, char* param)
{
	uint8_t JogData=0,i;
	
	if (LayerFlag==0)
		return 0;
	if (g_nM7_StartType!=M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
		return true;
#ifdef USED_ROTATION				//JPHong 231022
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	if (LogoPW_ResultFlag)	// 2024-03-09 JPHong V14
		return 0;
	JogData = param[0];
	printf("LogoJDK= '0x%X' %d\n",JogData,LogoPWSelIndex);
	if ((LogoPWSelIndex<0)||(LogoPWSelIndex>11))
	{
		LogoPWSelIndex = 0;
		LogoPW_Sel(LogoPWSelIndex);
		LogoPW_Sel_Run(LogoPWSelIndex);
	}
	else
	{
		if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
		{
			switch (JogData)
			{
				case JOG_DIAL_KEY_LEFT:
				{
					switch (LogoPWSelIndex)
					{
						case 0:
							LogoPWSelIndex = 2;
						break;
						case 1:
						case 2:
						case 4:
						case 5:
						case 7:
						case 8:
							LogoPWSelIndex = LogoPWSelIndex-1;
						break;
						case 9:
							LogoPWSelIndex = 10;
						break;
						case 11:
							LogoPWSelIndex = 9;
						break;
						case 3:
							LogoPWSelIndex = 5;
						break;
						case 6:
							LogoPWSelIndex = 8;
						break;
						case 10:
							LogoPWSelIndex = 11;
						break;
					}
					LogoPW_Sel(LogoPWSelIndex);
					LogoPW_Sel_Run(LogoPWSelIndex);
				}
				break;
				case JOG_DIAL_KEY_RIGHT:
				{
					switch (LogoPWSelIndex)
					{
						case 0:
						case 1:
						case 3:
						case 4:
						case 6:
						case 7:
							LogoPWSelIndex = LogoPWSelIndex+1;
						break;
						case 2:
							LogoPWSelIndex = 0;
						break;
						case 5:
							LogoPWSelIndex = 3;
						break;
						case 8:
							LogoPWSelIndex = 6;
						break;
						case 9:
							LogoPWSelIndex = 11;
						break;
						case 10:
							LogoPWSelIndex = 9;
						break;
						case 11:
							LogoPWSelIndex = 10;
						break;
					}
					LogoPW_Sel(LogoPWSelIndex);
					LogoPW_Sel_Run(LogoPWSelIndex);
				}
				break;
				case JOG_DIAL_KEY_UP:
				{
					switch (LogoPWSelIndex)
					{
						case 0:
							LogoPWSelIndex = 10;
						break;
						case 1:
							LogoPWSelIndex = 9;
						break;
						case 2:
							LogoPWSelIndex = 11;
						break;
						case 3:
						case 4:
						case 5:
						case 6:
						case 7:
						case 8:
							LogoPWSelIndex = LogoPWSelIndex-3;
						break;
						case 9:
							LogoPWSelIndex = 7;
						break;
						case 10:
							LogoPWSelIndex = 6;
						break;
						case 11:
							LogoPWSelIndex = 8;
						break;
					}
					LogoPW_Sel(LogoPWSelIndex);
					LogoPW_Sel_Run(LogoPWSelIndex);
				}
				break;
				case JOG_DIAL_KEY_DOWN:
				{
					switch (LogoPWSelIndex)
					{
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
						case 8:
							LogoPWSelIndex = LogoPWSelIndex+3;
						break;
						case 6:
							LogoPWSelIndex = 10;
						break;
						case 7:
							LogoPWSelIndex = 9;
						break;
						case 9:
							LogoPWSelIndex =1;
						break;
						case 10:
							LogoPWSelIndex = 0;
						break;
						case 11:
							LogoPWSelIndex = 2;
						break;
					}
					LogoPW_Sel(LogoPWSelIndex);
					LogoPW_Sel_Run(LogoPWSelIndex);
				}
				break;
				case JOG_DIAL_KEY_ENTER_OFF:
				{
					char JD_Buf[10] = {0,};
					LogoPW_Sel(LogoPWSelIndex);
					JD_Buf[0] = LogoPWSelIndex+1;
					USER_PW_UpBt_Func(NULL, JD_Buf);
				}
				break;
				
			}
		}
		else	//if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
		{
			switch (JogData)
			{
				case JOG_DIAL_KEY_LEFT:
				{
					switch (LogoPWSelIndex)
					{
						case 0:
							LogoPWSelIndex = 2;
						break;
						case 1:
						case 2:
						case 4:
						case 5:
						case 7:
						case 8:
							LogoPWSelIndex = LogoPWSelIndex-1;
						break;
						case 9:
							LogoPWSelIndex = 10;
						break;
						case 11:
							LogoPWSelIndex = 9;
						break;
						case 3:
							LogoPWSelIndex = 5;
						break;
						case 6:
							LogoPWSelIndex = 8;
						break;
						case 10:
							LogoPWSelIndex = 11;
						break;
					}
					LogoPW_Sel(LogoPWSelIndex);
					LogoPW_Sel_Run(LogoPWSelIndex);
				}
				break;
				case JOG_DIAL_KEY_RIGHT:
				{
					switch (LogoPWSelIndex)
					{
						case 0:
						case 1:
						case 3:
						case 4:
						case 6:
						case 7:
							LogoPWSelIndex = LogoPWSelIndex+1;
						break;
						case 2:
							LogoPWSelIndex = 0;
						break;
						case 5:
							LogoPWSelIndex = 3;
						break;
						case 8:
							LogoPWSelIndex = 6;
						break;
						case 9:
							LogoPWSelIndex = 11;
						break;
						case 10:
							LogoPWSelIndex = 9;
						break;
						case 11:
							LogoPWSelIndex = 10;
						break;
					}
					LogoPW_Sel(LogoPWSelIndex);
					LogoPW_Sel_Run(LogoPWSelIndex);
				}
				break;
				case JOG_DIAL_KEY_UP:
				{
					switch (LogoPWSelIndex)
					{
						case 0:
							LogoPWSelIndex = 10;
						break;
						case 1:
							LogoPWSelIndex = 9;
						break;
						case 2:
							LogoPWSelIndex = 11;
						break;
						case 3:
						case 4:
						case 5:
						case 6:
						case 7:
						case 8:
							LogoPWSelIndex = LogoPWSelIndex-3;
						break;
						case 9:
							LogoPWSelIndex = 7;
						break;
						case 10:
							LogoPWSelIndex = 6;
						break;
						case 11:
							LogoPWSelIndex = 8;
						break;
					}
					LogoPW_Sel(LogoPWSelIndex);
					LogoPW_Sel_Run(LogoPWSelIndex);
				}
				break;
				case JOG_DIAL_KEY_DOWN:
				{
					switch (LogoPWSelIndex)
					{
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
						case 5:
							LogoPWSelIndex = LogoPWSelIndex+3;
						break;
						case 6:
							LogoPWSelIndex =10;
						break;
						case 7:
							LogoPWSelIndex = 9;
						break;
						case 8:
							LogoPWSelIndex = 11;
						break;
						case 9:
							LogoPWSelIndex = 1;
						break;
						case 10:
							LogoPWSelIndex = 0;
						break;
						case 11:
							LogoPWSelIndex = 2;
						break;
					}
					LogoPW_Sel(LogoPWSelIndex);
					LogoPW_Sel_Run(LogoPWSelIndex);
				}
				break;
				case JOG_DIAL_KEY_ENTER_OFF:
				{
					char JD_Buf[10] = {0,};
					LogoPW_Sel(LogoPWSelIndex);
					JD_Buf[0] = LogoPWSelIndex+1;
					USER_PW_UpBt_Func(NULL, JD_Buf);
				}
				break;
				
			}
		}
	}
	
    return true;
}

bool LogoJDS(ITUWidget* widget, char* param)
{
	uint8_t JogData=0,i;
	
	if (LayerFlag==0)
		return 0;
	if (g_nM7_StartType!=M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
		return true;
#ifdef USED_ROTATION				//JPHong 231022
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	if (LogoPW_ResultFlag)	// 2024-03-09 JPHong V14
		return 0;
	JogData = param[0];
	printf("LogoJDS= '%d','%d','%d'\n",JogData,LogoPW_Index,LogoPWSelIndex);
	if (LogoPW_Index==0)
	{
		UserPW_PosIndex=0xff;
	//	LogoPWSelIndex=0xff;
		LogoPW_Sel(0xff);
	}
	else if (JogData==JOG_DIAL_SW_ESC_ON_OFF)
	{
		LogoPW[LogoPW_Index] = 0;
		LogoPW_Index--;
		LogoPW_Disp();
	}

    return true;
}
extern void XO_MICOM_EXIT(void);
static uint8_t Firm_UP_Flag = 0;
bool Logo_USB_Inserted(ITUWidget* widget, char* param)
{
	if (LayerFlag==0)
		return 0;
	if (CheckFirm_File()==1)
    {
    	#if 0
        printf(" file name: %s 0x%X\n", CFG_UPGRADE_FILENAME_LIST,logoLayer);

		if (Firm_UP_Flag==0)
		{
			Firm_UP_Flag = 1;
			XO_MICOM_EXIT();
			if (logoLayer!=NULL)
				ituWidgetSetVisible(logoLayer, DISP_OFF);

			
			printf(" Start Upgrade.... \n" );
			///UpgradeProcess(QUIT_UPGRADE_FIRMWARE);
			SceneQuit(QUIT_UPGRADE_FIRMWARE);
//            ReBootFunc();
		}
		#else
			GotoMain(MAIN_FIRMWARE_UP); 
		#endif
    }
    else
    	printf(" Not found Firmware File \n" );

        
    return true;
}




bool USER_PW_DnBt_Func(ITUWidget* widget, char* param)
{
	if (LayerFlag==0)
		return 0;
	
#ifdef USED_ROTATION				//JPHong 231022
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	if (LogoPW_ResultFlag)	// 2024-03-09 JPHong V14
		return 0;
	if (g_nM7_StartType!=M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
		return true;
	LogoPWSelIndex = (param[0]&0x1F)-1;
	printf("USER_PW_DnBt_Func= 0x%X , '%d'\n",param[0],LogoPWSelIndex);
	

	LogoPW_Sel(LogoPWSelIndex);
	LogoPW_Sel_Run(LogoPWSelIndex);
    return true;
}

bool USER_PW_UpBt_Func(ITUWidget* widget, char* param)
{
	uint8_t LogoPWData=0,i;
	if (LayerFlag==0)
		return 0;
#ifdef USED_ROTATION				//JPHong 231022
	if(RotationMode!=0)
	{
		return 0;
	}
#endif		
	LogoPWData = (param[0]&0x1F);

	if ((LogoPW_ResultFlag==1)&&(LogoPWData!=12)||(LogoPW_ResultFlag==2))	// 2024-03-09 JPHong V14
		return 0;
	printf("USER_PW_UpBt_Func= '%d' '%d'\n",LogoPWData,LogoPW_Index);

	switch (LogoPWData)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		{
			if (LogoPW_Index<(MAX_PW_CNT))
			{
				LogoPW[LogoPW_Index] = LogoPWData;
				LogoPW_Index++;
			}
		}
		break;
		case 10:
		{
			if (LogoPW_Index<(MAX_PW_CNT))
			{
				LogoPW[LogoPW_Index] = 0;
				LogoPW_Index++;
			}
		}
		break;
		case 11:	// esc	
			if (LogoPW_Index>0)
			{
				LogoPW[LogoPW_Index] = 0;
				LogoPW_Index--;
			}
			else
			{
				#ifndef RELEASE_VER
					GotoMain(MAIN_MODE_NORMAL); 
				#endif
				#ifdef TEST_ROTATION
				{
					RotationButtonOnPress();
					//GotoRotation();
				}
				#endif
				#if 0
				{
				
					#define CFG_GPIO_LCD_RESET	38
					/*					
					extern uint32_t __lcd_base_a;
					extern uint32_t __lcd_base_b;
					extern uint32_t __lcd_base_c;					
					
					printf("================== CFG_GPIO_LCD_RESET S\n");
					ithGpioSet(CFG_GPIO_LCD_RESET);
					ithGpioSetMode(CFG_GPIO_LCD_RESET, ITH_GPIO_MODE0);
					ithGpioSetOut(CFG_GPIO_LCD_RESET);
					ithGpioSet(CFG_GPIO_LCD_RESET);
					usleep(6000);
					ithGpioClear(CFG_GPIO_LCD_RESET);
					usleep(11000);
					ithGpioSet(CFG_GPIO_LCD_RESET);
					usleep(120000); 
					
					
					printf("================== MainTask\n");
					printf("================== MainTask\n");
					printf("================== MainTask\n");
					itpRegisterDevice(ITP_DEVICE_SCREEN, &itpDeviceScreen);
					ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_POST_RESET, NULL);
					ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_RESET, NULL);
					
					ithLcdSetBaseAddrA((uint32_t) &__lcd_base_a);
					ithLcdSetBaseAddrB((uint32_t) &__lcd_base_b);					
					*/
					
				//	ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_SUSPEND, NULL);
					ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_RESUME, NULL);
//					ithCmdQReset(0x0);
//					ithCmdQReset(0x1);
				
				}
				#endif
			}		
		break;
		case 12:	//enter
			gLogoPW_D7=0xFF;
			LogoPW_Data = 0;
			if ((UserPW_PassFlag==1)&&(tCamSetFlag==1))
			{
//				Flash_Init();	// JPHong 2024-06-20
				GotoMain(MAIN_MODE_USERINFO);// 2024-02-04 JPHong
			}
			else
			{
				printf("LogoPW_Index:[%d] \n",LogoPW_Index);
				for (i=0;i<LogoPW_Index;i++)
				{
					uint32_t kkk = (int)(pow(10, (LogoPW_Index-i-1)));
					
//					printf("i:[%d] %d %d 0x%X\n",i,LogoPW[i],kkk,LogoPW_Data);
					LogoPW_Data = LogoPW_Data+(LogoPW[i]*kkk);
//					printf("i:[%d] %d %d 0x%X\n",i,LogoPW[i],kkk,LogoPW_Data);
				}
				CanbusTxDataManagement( gMainTimerCnt_10m,UA1,NULL);
				UserPW_Reply_time = gMainTimerCnt_10m;
			}
		break;
	}
	LogoPW_Disp();
    return true;
}
