#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"
#include "ite/itp.h"

#include "HT_DispDdefine.h"
#include "Menu_Layer.h"



static char widgetName[30];
static uint8_t KeyDepth = 0;



uint8_t gMicomFirmVer[8] = {0,};


uint8_t RotationMode = 0;	// 0:off , 1:ing , 2:run
uint32_t RotationModeTime=0;

uint8_t MainCursorCnt = 0;
uint8_t Main_Select_Index = MAIN_SEL_MENU;	// JPHong 2024-09-21 Spec V0.19 Page 40  0:off , 1:Warring(x)  , 2:ATT  , 3:AirCon  , 4:Radio  , 5:HourMeter  , 6:Bar  , 7:TimeDate(x)
uint8_t Main_Select_Index_OnOff = DISP_ON;	// JPHong 2024-09-21 Spec V0.19 Page 40


uint8_t Main_Disp_Mode = MAIN_MODE_PW;		// 0:Normal , 1:AirCon  , 2:Radio  , 3:DAB1  , 4:DAB2  , 5:Bluetooth  , 6:Phone  , 7:Camera
uint8_t Main_Disp_Mode_Flag = 0;	// 0:Normal , 1:Changing



static uint8_t ZoomFlag_W_01d1 = 0;
static uint8_t ZoomFlag_W_01d2 = 0;
static uint8_t ZoomFlag_W_02d1 = 0;
uint8_t MainBtFlag = 0;


extern bool MainMode_AC_Sel(uint8_t IncDec,uint8_t SelIndex,uint8_t OnOff);
extern bool MainMode_RD_Sel(uint8_t ReDisp,uint8_t SelIndex,uint8_t OnOff);
extern bool MainLayer_ButtonSel(uint8_t SelIndex,uint8_t OnOff);

//===
extern void DAB3_Timer();//chpark.07.18
//===

static ITULayer* mainLayer=NULL;
 
bool MainCamera(ITUWidget* widget, char* param);
bool Main_USB_Inserted(ITUWidget* widget, char* param);



	


void GotoMainReal(uint8_t SubMode) 
{

	printf("\nGotoMainReal===> :[%d][%d][%d]\n",MainLayerFlag,mainLayer,RotationMode);
	char JD_Buf[10] = {0,};
	if (mainLayer==NULL)
	{
		mainLayer = ituSceneFindWidget(&theScene, "mainLayer");
		assert(mainLayer);
	}

	
	
	printf("GotoMainReal===> : [%d][%d][%d][%d]\n",Main_Disp_Mode,SubMode,MainBtFlag,Main_Disp_Mode_Flag);
	if (((Main_Disp_Mode!=SubMode)&&(MainBtFlag==0))||(RotationMode!=0))
	{
		printf("Micom Ver : [%s]\n\n",gMicomFirmVer);
		switch (SubMode)
		{
			case MAIN_MODE_NORMAL:	// MainNormal
				Main_Disp_Mode = MAIN_MODE_NORMAL;
				MainBtFlag = MAIN_MODE_NORMAL;
			break;
			case MAIN_MODE_USERINFO:
				Main_Disp_Mode = MAIN_MODE_USERINFO;
				MainBtFlag = MAIN_MODE_USERINFO;
			break;
			case MAIN_MODE_RADIO:	// Radio
				Main_Disp_Mode = MAIN_MODE_RADIO;
				MainBtFlag = MAIN_MODE_RADIO;
			break;
			case MAIN_MODE_AIRCON:	// Aircon
				Main_Disp_Mode = MAIN_MODE_AIRCON;
				MainBtFlag = MAIN_MODE_AIRCON;
			break;
			case MAIN_MODE_DAB1:
				Main_Disp_Mode = MAIN_MODE_DAB1;
				MainBtFlag = MAIN_MODE_RADIO;
			break;		
			case MAIN_MODE_DAB2:
				Main_Disp_Mode = MAIN_MODE_DAB2;
				MainBtFlag = MAIN_MODE_RADIO;
			break;
			case MAIN_MODE_DAB3:
				Main_Disp_Mode = MAIN_MODE_DAB3;
				MainBtFlag = MAIN_MODE_RADIO;
			break;
			case MAIN_MODE_BLUETOOTH:	// Aircon
				Main_Disp_Mode = MAIN_MODE_BLUETOOTH;
				MainBtFlag = MAIN_MODE_RADIO;
			break;
			case MAIN_MODE_PHONE:	// Phone
				Main_Disp_Mode = MAIN_MODE_PHONE;
				MainBtFlag = MAIN_MODE_RADIO;
			break;
			case MAIN_FIRMWARE_UP:	// FirmUp
				Main_USB_Inserted(NULL, NULL);
			break;
			default:
				Main_Disp_Mode = MAIN_MODE_NORMAL;
				MainBtFlag = MAIN_MODE_NORMAL;
			break;
		}
		ituLayerGoto(mainLayer);
	}
}

void InitJogIncDecData_AC(void)
{
	AC_SEL_DIR_LR_FLAG = 0;
	AC_SEL_TEMP_LR_FLAG = 0;
	AC_SEL_LEVEL_LR_FLAG = 0;
	AC_SEL_DIR_R_CNT = LONG_KEY_TIME;
	AC_SEL_TEMP_R_CNT = LONG_KEY_TIME;
	AC_SEL_LEVEL_R_CNT = LONG_KEY_TIME;
	AC_SEL_DIR_L_CNT = LONG_KEY_TIME;
	AC_SEL_TEMP_L_CNT = LONG_KEY_TIME;
	AC_SEL_LEVEL_L_CNT = LONG_KEY_TIME;
	Main_CLI2[2] = 0;
}
void InitJogIncDecData_RD(void)
{
//	printf("InitJogIncDecData_RD=%d,%d,%d,%d,%d\n",RD_SEL_TUNE_LR_FLAG,RD_SEL_TONE_DU_FLAG,RD_SEL_VOL_DU_FLAG);
	RD_SEL_TUNE_LR_FLAG = 0;
	RD_SEL_TONE_DU_FLAG = 0;
	RD_SEL_VOL_DU_FLAG = 0;
	Main_CLI2[3] = 0;
	Main_CLI2[4] = 0;
	RD_SEL_TUNE_L_CNT = LONG_KEY_TIME;
	RD_SEL_TONE_D_CNT = LONG_KEY_TIME;
	RD_SEL_VOL_D_CNT = LONG_KEY_TIME;
	RD_SEL_TUNE_R_CNT = LONG_KEY_TIME;
	RD_SEL_TONE_U_CNT = LONG_KEY_TIME;
	RD_SEL_VOL_U_CNT = LONG_KEY_TIME;
}
void InitVariableData(void)
{
	Main_WarningInit();
}

void InitVariable(uint8_t UserIndex, bool bNameReset)//chpark.24.11.19
{
	int j;
	Lang_Flag[UserIndex] = Lang_ENG;
	Option_Function.AutoStop[UserIndex] = 1;
	Option_Function.AutoStopKey = 1;
	//printf("[SU200_Timer][1]UserName[%d]==>[%s] bNameReset=%d\n",g_nSU100_AddUserIdx,User_RegInfo[g_nSU100_AddUserIdx].UserName ,bNameReset);
	if(bNameReset)
	{
		for(j=0;j<MAX_SU100_USER_NAME;j++)
		{
			User_RegInfo[UserIndex].UserName[j] = NULL;				// username Max. 10digit
			//memset(User_RegInfo[UserIndex].UserName, 0, sizeof(User_RegInfo[UserIndex].UserName));
		}
		for(j=0;j<MAX_SU100_USER_PW;j++)//chpark.12.12
		{
			User_RegInfo[UserIndex].UserPassCode[j] = NULL;		// user passcode 10digit(Max 10digit,Min 4digit)
			//memset(User_RegInfo[UserIndex].UserPassCode, 0, sizeof(User_RegInfo[UserIndex].UserPassCode));
		}
			//printf("[SU200_Timer][2]UserName[%d]==>[%s] bNameReset=%d\n",g_nSU100_AddUserIdx,User_RegInfo[g_nSU100_AddUserIdx].UserName ,bNameReset);

	}

	if(UserIndex==0)
		strncpy(&User_RegInfo[UserIndex].UserName[0],"Super+ Admin",MAX_SU100_NAME_CNT);
	else if(UserIndex==1)
		strncpy(&User_RegInfo[UserIndex].UserName[0],"Super Admin",MAX_SU100_NAME_CNT);
	else
	{
		if(bNameReset)
		{
			char ibuffer[14];
			sprintf(ibuffer,"default user%d",UserIndex-1);
			strncpy(&User_RegInfo[UserIndex].UserName[0],ibuffer,MAX_SU100_NAME_CNT);
			User_RegInfo[UserIndex].RegKeyNum = UserIndex;	
						//printf("[SU200_Timer][3]UserName[%d]==>[%s] bNameReset=%d\n",g_nSU100_AddUserIdx,User_RegInfo[g_nSU100_AddUserIdx].UserName ,bNameReset);


		}
	}
	//printf("[SU200_Timer][4]UserName[%d]==>[%s]\n",g_nSU100_AddUserIdx,User_RegInfo[g_nSU100_AddUserIdx].UserName);
	//printf("[SU200_Timer][4]UserIndex[%d==>[%s] RegKeyNum[%d]\n",g_nSU100_AddUserIdx,User_RegInfo[g_nSU100_AddUserIdx].UserName,User_RegInfo[g_nSU100_AddUserIdx].RegKeyNum);	

}
	
	
void SettingInit(void)
{
	int i;
	
	AUX1CurrentOutMode = 0;	// 0~2
	DTM_DateTime_Mode = Type1_YYYYMMDD;	// 	Type1_YYYYMMDD:1 , Type2_MMDDYYYY:2 , Type3_DDMMYYYY:3
	BG_Color = BG_BLACK;	// BG_BLUE:1 , BG_BLACK:2
	LCD_BL_Day = 100; 
	LCD_BL_Night = 10; 	// JPHong 2023-10-04 menu spec 18page.
	SIUS_SEL_Flag = 0;	// 0:SI , 1:US
	HMV.HourMeter_Trip = 0;
	Main_DayNight = 0;		// 0:Day , 1:Night
	


	/*
	Brightness 	: 0 	: -64 ~63
	Contrast 	: 100 	: 0-127
	Saturation 	: 64 	: 0-127
	Hue 		: -10 	: -90~90
	Sharpness 	: 0 	: 0~31
	*/
	
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

	S7_SelectRegion = 1;
	Radio_Region = 0;
	cRadio_PowerCondition = 1;
	
	Option_Function.AutoDecel 		= 1;
	Option_Function.AutoCamera		= 1;
	Option_Function.Rotate_Flag	= 1;
	
	M7_Group = 0;
	
	for(i=0;i<MAX_SU100_USER_NUM;i++)
	{
		InitVariable(i,true); //chpark.24.11.19
	}
	
	M7_UserSet[0].A_Value = 50;
	M7_UserSet[1].A_Value = 37;
	M7_UserSet[2].A_Value = 25;
	M7_UserSet[0].B_Value = 50;
	M7_UserSet[1].B_Value = 37;
	M7_UserSet[2].B_Value = 25;
	M7_UserSet[0].P1_Value = 68;
	M7_UserSet[1].P1_Value = 68;
	M7_UserSet[2].P1_Value = 68;
	M7_UserSet[0].ATT_State = 1;
	M7_UserSet[1].ATT_State = 2;
	M7_UserSet[2].ATT_State = 3;
	M7_UserSet[0].Way_Opt = 0;
	M7_UserSet[1].Way_Opt = 0;
	M7_UserSet[2].Way_Opt = 0;
	M7_UserAUX24Set.C_Value = 100;
	M7_UserAUX24Set.D_Value = 100;
	M7_UserAUX24Set.P2_Value = 68;
	M7_UserAUX24Set.G_Value = 100;
	M7_UserAUX24Set.H_Value = 100;
	M7_UserAUX24Set.P4_Value = 68;

	g_bPW100_PW_OnOff = PW100_PASS_MODE_OFF; //chpark.24.09.19
	for(int i=0;i<3;i++)
	{
		g_nM7_Lock_Data[i].Lock = 1;
		g_nM7_Lock_Data[i].Show = 0;
	}
	memset(PW100_PWD,'\0', sizeof(PW100_PWD));
}


void gInitVariableData(void)
{
	int i,j,k;

	//	MainIMM_R_4B03b=0;
	
	printf("\nStart gInitVariableData\n");
	
	
	
	M7_UserIndex = 0;	// 0~3
	M7_UserIndexEx = 0;

	SIDEAUX_Machine = 0;	// 0~8
	
	
	
	//	Main_ZoomData = 0xFF;	//0x80(Function)
	//	End_Main_Zoom = 0xFF;
	
	//	Main_FuelMeterValue		= 0x00;		//0~1200%
	//	Main_CoolantMeterValue	= 0x00;		//0~100%
	//	Main_CoolantOrange = 0;
	//	Main_HydTempMeterValue=0;
	//	Main_AdBLUEMeterValue=0xFF;
	
	
	
	HMV.HourMeter_Toggle = 0;
	HMV.HourMeter_Trip_Clear1 = 0;
	HMV.HourMeter_Trip_Clear2 = 0;
	HMV.HourMete_rHour[0] = 0;
	HMV.HourMete_rHour[1] = 0;
	HMV.HourMete_rHour[2] = 0;
	HMV.HourMete_rHour[3] = 0;
	HMV.HourMete_rHour[4] = 0;
	HMV.HourMete_rHour[5] = 0;
	HMV.HourMete_rHour[6] = 0;
	HMV.Min_V = 0;
	HMV.Hour_V = 0;
	HMV.Day_V = 0;
	HMV.Mon_V = 0;
	HMV.Year_V = 0;
	
	
	
	MRV.Symbol = m13_MeterSymbol_EngRPM; // m11_MeterSymbol_PM=11,m12_MeterSymbol_LiftLd=12,m13_MeterSymbol_EngRPM=13,m14_MeterSymbol_EngLd=14,m15_MeterSymbol_Ash=15,
	MRV.PM = 0xFF;	// 0 ~ 100  0%~100%
	MRV.PMTime = 0;
	MRV.LiftLd = 0xFF;	// 0 ~ 250  0%~100%
	MRV.LiftLdTime = 0;
	MRV.EngRPM = 0xFF;	// 0 ~ 100  0%~100%
	MRV.EngRPMTime = 0;
	MRV.EngLd = 0xFF;	// 0 ~ 250  0%~100%
	MRV.EngLdTime = 0;
	MRV.PM = 0xFF;	// 0 ~ 250  0%~100%
	MRV.PMTime = 0;
	
	//	MRV.MeterRevoFlag=0;
	//	MRV.MeterLiftAlarmFlag=0;
	
	ACV.AirMode=3; 	// In Out
	ACV.ACTemp=0xFF;		//
	ACV.ACOnOff=0;
	ACV.ACLevel=0;	// 0-5
	ACV.ACAuto=0;
	ACV.ACDir=0;		// 0-4
	
	
	RDV.RDOnOff=0; 		// oNoFF
	RDV.RDStereo=0;		//
	RDV.RDScan=0;
	RDV.RDMode=1;		// AM,FM,BT,DAB1,DAB2
	RDV.RDFreq=0;	//
	RDV.RDStep=0;
	RDV.RDTone=0;
	RDV.RDVol=0;
	RDV.RDMute=0;
	RDV.BTPair=0;
	RDV.RDPhone=0;		// 0-4
	RDV.PTYNo = -1;
	RDV.DAB2_PTYNo = -1; //chpark.05.30
	RDV.DAB3_PTYNo = -1; //chpark.05.30
	RDV.PTYNo = -1;
	RDV.RDSelCh = 0;
	RDV.DABSelCh = 0;
	RDV.DABFreq = 0;

	//printf("==========RESET ServiceCnt====\n");//chpark.11.15
	memset (&RDV.DABServiceCnt[0],0,sizeof(RDV.DABServiceCnt));
	memset (&RDV.RDPhoneNum[0],0,sizeof(RDV.RDPhoneNum));
	
	gPW_NextMode=0;			// 	0:NG ,11:1st AUX  ,12:2nd /4th   ,13:LOCK ,14:PASSWORD
	gAutoGreaseFlag=0;		// 0:off , 1:on
	SIDEAUX_Machine=0;		// 0~8
	AutoTankDispFlag=0;
	PressureDispFlag=0;
	
	
	SettingInit();
	
	Fn_Cursor = 0;
	Fn_ModeDepth = 0;
	gCameraModeDisp = 0;	// 0:off , 1:BS , 2:B , 3:S , 4: AB , 5:AR , 6:AL , 7:BR , 8:3c
	gCameraStatus0 = 0xFF;
	gCameraLock0 = 0;	// 0:off , 1:lock
	gCameraStatus1 = 0xFF;
	gCameraLock1 = 0;	// 0:off , 1:lock
	MenuCameraMode = 0;
	gCreepMode = 0;			// 0:off , 1:on   7bit : OPI2  , 6bit : CLM2
	gCreepValue = 5;
	
	gUserPW[0] = 0;
	gUserPW[1] = 0;
	gUserPW[2] = 0;
	gUserPW[3] = 0;
	
	gRead_Gpio_Value=0;
	
	// AUX1st_LockMode=0;
	// AUX2nd_LockMode=0;
	
	// AG_ForecedSuppy = 0;
	// AG_Time = 0;
	// AG_Max = 0;
	// AG_Min = 0;
	// AG_Start = 0;
	// AG_Stop = 0;
	
	
	DTVal.EEC1_34Bytes = 0xFFFFFFFF;	// [?????? ??? ???? 0 ~ 65,278] [????? 0.125/bit)] [??? ??? ???? 0 ~ 8,159]
	DTVal.EEC1_34Cnt = 0;	// J	// JPHong0830 DTVal.EEC1_34Time = gMainTimerCnt_10m;	// [?????? ??? ???? 0 ~ 65,278] [????? 0.125/bit)] [??? ??? ???? 0 ~ 8,159]
	DTVal.ET1_0Byte = 0xFF;		// [?????? ??? ???? 0 ~ 250] [OFFSET -40] [??? ??? ???? -40 ~ 210]
	DTVal.ET1_0Time = gMainTimerCnt_10m;		// [?????? ??? ???? 0 ~ 250] [OFFSET -40] [??? ??? ???? -40 ~ 210]
	DTVal.SI_0Byte = 0xFF;		// [?????? ??? ???? 0 ~ 250] [OFFSET -40] [??? ??? ???? -40 ~ 210]
	DTVal.SI_0Time = gMainTimerCnt_10m;		// [?????? ??? ???? 0 ~ 250] [OFFSET -40] [??? ??? ???? -40 ~ 210]
	DTVal.AT1T1I_1Byte = 0xFF;	// [?????? ??? ???? 0 ~ 250] [OFFSET -40] [??? ??? ???? -40 ~ 210]
	DTVal.AT1T1I_1Time = gMainTimerCnt_10m;	// [?????? ??? ???? 0 ~ 250] [OFFSET -40] [??? ??? ???? -40 ~ 210]
	DTVal.DD_1Byte = 0xFF;		// [?????? ??? ???? 0 ~ 250] [????? 0.4/bit)] [??? ??? ???? 0 ~ 100]
	DTVal.DD_1Time = gMainTimerCnt_10m;		// [?????? ??? ???? 0 ~ 250] [????? 0.4/bit)] [??? ??? ???? 0 ~ 100]
	DTVal.AT1T1I_0Byte = 0xFF;	// [?????? ??? ???? 0 ~ 250] ] [????? 0.4/bit)] [??? ??? ???? 0 ~ 100]
	DTVal.AT1T1I_0Time = gMainTimerCnt_10m;	// [?????? ??? ???? 0 ~ 250] ] [????? 0.4/bit)] [??? ??? ???? 0 ~ 100]
	DTVal.VEP_45Bytes = 0xFFFF;	// [?????? ??? ???? 0 ~ 1,790] [????? 0.05/bit)] [??? ??? ???? 0 ~ 89.5]
	DTVal.VEP_45Time = gMainTimerCnt_10m;	// [?????? ??? ???? 0 ~ 1,790] [????? 0.05/bit)] [??? ??? ???? 0 ~ 89.5]
	DTVal.EI_2Byte = 0xFF;		// [?????? ??? ???? 0 ~ 250] ] [????? 0.4/bit)] [??? ??? ???? 0 ~ 250]
	DTVal.EI_2Time = gMainTimerCnt_10m;		// [?????? ??? ???? 0 ~ 250] ] [????? 0.4/bit)] [??? ??? ???? 0 ~ 250]
	DTVal.AT1S_1Byte = 0xFF;		// [?????? ??? ???? 0 ~ 250] ] [????? 0.4/bit)] [??? ??? ???? 0 ~ 250]
	DTVal.AT1S_1Time = gMainTimerCnt_10m;		// [?????? ??? ???? 0 ~ 250] ] [????? 0.4/bit)] [??? ??? ???? 0 ~ 250]
	DTVal.ET1_1Byte = 0xFF;		// [?????? ??? ???? 0 ~ 250] [OFFSET -40] [??? ??? ???? -40 ~ 210]
	DTVal.ET1_1Time = gMainTimerCnt_10m;		// [?????? ??? ???? 0 ~ 250] [OFFSET -40] [??? ??? ???? -40 ~ 210]
	DTVal.AT1S_2Byte = 0xFF;		// [?????? ??? ???? 0 ~ 250] ] [????? 0.4/bit)] [??? ??? ???? 0 ~ 250]
	DTVal.AT1S_2Time = gMainTimerCnt_10m;		// [?????? ??? ???? 0 ~ 250] ] [????? 0.4/bit)] [??? ??? ???? 0 ~ 250]
	
	memset(&gMB_IO_Value, 0xFF, sizeof(MB_IO_Value)); //chpark.07.05
	CLM1_Time=0;
	CLM2_Time=0;
	CLM3_Time=0;
	CLM4_Time=0;
	AG_I_Time=0;
	EI_Time=0;
	DD_Time=0;
	SI_Time=0;
	//	MI_Time=0;
	HM_Time=0;
	M9HM_Time=0;
	TM1_Time=0;
	TM2_Time=0;
	TM3_Time=0;
	AUX_IO2_Time=0;
	
	IN1_Time = 0;
	IN2_Time = 0;
	OUT1_Time = 0;
	OUT6_Time = 0;
	IN17_Time = 0;
	IN3_Time = 0;
	IN4_Time = 0;
	IN5_Time = 0;
	IN6_Time = 0;
	IN7_Time = 0;
	IN9_Time = 0;
	IN13_Time = 0;
	IN23_Time = 0;
	OUT2_Time = 0;
	OUT3_Time = 0;
	OUT4_Time = 0;
	OUT8_Time = 0;
	AUX1_Time = 0;
	AUX2_Time = 0;
	MTSPD_Time = 0;
	OUT5_Time = 0;
	
	for (i=0;i<4;i++)
	{
		ErVal.EMR[i] = 0;
	}
	for (i=0;i<3;i++)
	{
		ErVal.ECM_S[i] = 0;
		ErVal.ECM_F[i] = 0;
	}
	ErVal.ECM_CNT = 0;
	Main_DriveMode = 1;		// 0:????  , 1:Drive
	
	
	MainCursorCnt = 0;
	Main_Select_Index = MAIN_SEL_MENU;	// 0:off , 1:Warring(x)  , 2:ATT  , 3:AirCon  , 4:Radio  , 5:HourMeter  , 6:Bar  , 7:TimeDate(x)
	Main_Disp_Mode = MAIN_MODE_PW;		

	
	for (i=0;i<WARNING_ICON_CNT;i++)
	{
		MainWarnigData[i] = 0;
	}
	for (i=0;i<FUNCTION_ICON_CNT;i++)
	{
		MainFunctionData[i] = 0;
	}
	
	RotationMode = 0;	// 0:off , 1:ing , 2:run
	RotationModeTime = 0;
	
	sprintf(RDV.RDPrg, " No Information ");
	sprintf(RDV.RDBCS, " No Information ");
	

	g_TCI_0B = 0;
	g_TCI_1B = 0xFFFF;
	g_TCI_Time = 0;

	for(i=0;i<MAX_SU100_USER_NUM;i++)
	{
	
		User_RegInfo[i].RegFlag = 0;			// 0;Not Register ,1;Master ,2;Operater
		User_RegInfo[i].PassDigit = 0;		// PassCode Digit
		User_RegInfo[i].RFStatus = 0;			// RFID Register Status
		User_RegInfo[i].MacStatus = 0;		// BT MAC ID Register status

		for(j=0;j<MAX_SU100_USER_NAME;j++)
		{
			User_RegInfo[i].UserName[j] = 0;				// username Max. 10digit
		}
		for(j=0;j<MAX_SU100_USER_PW;j++)//chpark.12.12
		{
			User_RegInfo[i].UserPassCode[j] = 0;		// user passcode 10digit(Max 10digit,Min 4digit)
		}
		for(k=0;k<MAX_SU100_USER_MID;k++)
			User_RegInfo[i].UserMacID[k] = 0;				// user BT MascID 12digit( XX:XX:XX:XX:XX:XX ,X:0~9 and A~F)
	}
		
	AUX1_Init();
	AUX24_Init();			// add 230607 jhlee
	
	memset (&gMB_IO_Value.MAIN_CON_1234567Bytes[0],0,sizeof(gMB_IO_Value.MAIN_CON_1234567Bytes));
	ClearDM14();
	
	OPI2_1Byte_23bit = 1;	//Rotation  JPHong 2024-10-14
	g_IM_b_ExistLayer = false; //chpark.24.10.24
	
	printf("End gInitVariableData\n\n");
	//	TempMain_CanCamOn =0x0;
	
	printf("\n");
	printf("\n");
	printf("AAA M7_UserIndex=%d , ModelType=%d\n",M7_UserIndex,ModelType);
}

extern bool XO_ituButtonSetMode_Bt(bool SetMode);
extern bool XO_ituButtonSetMode_TP(bool SetMode);
extern bool M7S1_EnterFlag;
void ReStartMain(void);
bool MainOnEnter(ITUWidget* widget, char* param)
{
	int i;
	uint8_t tIndex = 0;
	if (1)
	{
		if (gMicomFirmVer==NULL)
		{
			printf("XO_MICOM_SOFT_REQ  XO_MICOM_SOFT_REQ\n");	
			CanbusTxDataManagement( gMainTimerCnt_10m,XO_MICOM_SOFT_REQ,NULL);			
		}
	}
	printf("MainOnEnter=%s,%d,%d\n",param,Main_Disp_Mode,M7_UserIndex);
	printf("MainOnEnter=%d,%d,d,%d\n",AUX1CurrentOutMode,M7_UserSet[0].ATT_State-1,M7_UserSet[1].ATT_State-1,M7_UserSet[2].ATT_State-1);
	XO_ituButtonSetMode_Bt(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
	XO_ituButtonSetMode_TP(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
	
	
#ifndef TEST_CAM_COLOR
	if (!MainLayerFlag)
	{
	//	CameraOnEnter();
	}
#endif		
	printf("MainOnEnter End=%s,%d,%d\n",param,Main_Disp_Mode,M7_UserIndex);
		
	if (Main_Disp_Mode_Flag==1)
	{
//		CameraOnLeaveCallSrc(MAIN_MODE_NORMAL);
		printf("\n[MainOnEnter]Main_Disp_Mode_Flag===> : %d %d %d\n",Main_Disp_Mode,MainBtFlag,Main_Disp_Mode_Flag);
		printf("[MainOnEnter]Main_Disp_Mode_Flag===> : %d %d %d\n",Main_Disp_Mode,MainBtFlag,Main_Disp_Mode_Flag);
		printf("[MainOnEnter]Main_Disp_Mode_Flag===> : %d %d %d\n",Main_Disp_Mode,MainBtFlag,Main_Disp_Mode_Flag);
		printf("[MainOnEnter]Main_Disp_Mode_Flag===> : %d %d %d\n",Main_Disp_Mode,MainBtFlag,RotationMode);
		printf("[MainOnEnter]Main_Disp_Mode_Flag===> : %d %d %d\n\n",Main_Disp_Mode,MainBtFlag,Main_Disp_Mode_Flag);
	}

	
	
#if 1	
	CLM1_Time = gMainTimerCnt_10m;
	CLM2_Time = gMainTimerCnt_10m;
	CLM3_Time = gMainTimerCnt_10m;
	CLM4_Time = gMainTimerCnt_10m;
	AG_I_Time = gMainTimerCnt_10m;
	Main_EI_5BTime = gMainTimerCnt_10m;
	MRV.PMTime = gMainTimerCnt_10m;
	MRV.EngRPMTime = gMainTimerCnt_10m;
	MRV.EngLdTime = gMainTimerCnt_10m;
	MRV.ASHTime = gMainTimerCnt_10m;
	Main_SI_67BTime = gMainTimerCnt_10m;
	MRV.LiftLdTime = gMainTimerCnt_10m;
//	DTVal.EEC1_34Time = gMainTimerCnt_10m;
//	TM1_Time = gMainTimerCnt_10m;
//	TM2_Time = gMainTimerCnt_10m;
//	TM3_Time = gMainTimerCnt_10m;
#endif

	gCameraModeDisp = 0;	// 0:off , 1:BS , 2:B , 3:S , 4: AB , 5:AR , 6:AL , 7:BR , 8:3c	
	M7S1_EnterFlag=0;
	MainOnEnter_1(widget, param);


#ifdef TEST_CAM_COLOR
	{
		MainLayer_Button(DISP_OFF);
		CameraOnEnter();
	}
#endif


	if (Main_Disp_Mode==MAIN_MODE_USERINFO)
	{
		UserInfoOnEnter();
		UserInformation_OnOff (DISP_ON);
	}
	else
	{
		UserInformation_OnOff (DISP_OFF);
	}
	
	
	
		
	if (mainLayer==NULL)
		SetWidget(mainLayer,"mainLayer");
	ituWidgetSetVisible(mainLayer, DISP_ON);
	
	MainLayer_BGDisp(Main_DayNight,BG_Color,Main_Disp_Mode);	// BG_BLUE:1 , BG_BLACK:2

	ZoomFlag_W_01d1 = 0;
	ZoomFlag_W_01d2 = 0;
	ZoomFlag_W_02d1 = 0;
	
	
//	if (Main_Select_Index_OnOff == DISP_OFF)
//		MainLayer_ButtonSel(MAIN_SEL_OFF,DISP_OFF);
//	else
		MainLayer_ButtonSel(Main_Select_Index,DISP_ON);
	

	MainLayer_FuncATTDisp(AUX1CurrentOutMode,M7_UserSet[0].ATT_State-1,M7_UserSet[1].ATT_State-1,M7_UserSet[2].ATT_State-1,1,DISP_ON);
	MainDispDateFirst();
	MainLayer_Power_Icon_Check(0);
	
	ReStartMain();
	
//	gCameraModeDisp = 0;	// 0:off , 1:BS , 2:B , 3:S , 4: AB , 5:AR , 6:AL , 7:BR , 8:3c

	if ((MainBtFlag == MAIN_MODE_NORMAL)||(Main_Disp_Mode==MAIN_MODE_USERINFO))
	{
		ScreenSetOnOff(1);
	}
	ReadyRotation();

	printf("MainOnEnter End=%s,%d,%d\n",param,Main_Disp_Mode,M7_UserIndex);
    return true;
}


#ifdef TEST_ERR_602
extern uint32_t TestCan0TaskTxCnt;
extern uint32_t TestCan0TaskTxFlag;
extern void TestCan0TaskTx(void);
#endif


bool RD_MAIN_DnBt(ITUWidget* widget, char* param);
bool RD_MAIN_DnBt2(ITUWidget* widget, char* param);
bool AC_MAIN_UpBt2(ITUWidget* widget, char* param);
bool AC_MAIN_DnBt2(ITUWidget* widget, char* param);
bool AC_MAIN_UpBt(ITUWidget* widget, char* param);
bool MainOnTimer(ITUWidget* widget, char* param)
{
	static uint32_t OnTimerCnt_33ms = 0;
	static uint32_t OnTimerCnt_100ms = 0;
	static uint32_t OnTimerCnt_300ms = 0;
	static uint32_t OnTimerCnt_500ms = 0;
	static uint32_t OnTimerCnt_600ms = 0;
	static uint32_t OnTimerCnt_1S = 0;
	static uint32_t OnTimerCnt_5S = 0,iCON_CNT=0,i;

	OnTimerCnt_33ms++;

	#ifdef TEST_ERR_602
		TestErr602();
		TestCan0TaskTx();
		TestCan0TaskTx();
	#endif
	#ifdef USED_ROTATION
	if (RotationMode!=0)
	{
		return 0;
	}
	#endif
	
	if (MicomFirm_UP_Flag!=0)
	{
		static uint8_t progressCnt=0;

		switch (MicomFirm_UP_Flag)
		{
			case 1:	// Check Ready
			{
				CheckMicomReady();
			}
			break;
			case 2:	// Send Firm
			{
				SendMicomFirm();
			}
			break;
			case 3:	// Write Firm
			{
				WriteMicomFirm();
			}
			break;
		}

		//chpark.24.05.17
		if ((OnTimerCnt_33ms%TIME_CNT_33ms_500MS)==0)	// 500ms
		{
			if(MicomFirm_UP_Flag>=1 && MicomFirm_UP_Flag<=2)
			{
				//the progress bar progresses to 40 in step 1.
				progressCnt+=20;
				if(progressCnt>=40)
				{
					progressCnt=40;
				}
				printf("[MC100_PregressUpdate]progressCnt=%d\n",progressCnt);
				MC100_PregressUpdate(progressCnt);
			}
			else
			{
				progressCnt = 0;
			}
		}
	}
	
	if (Main_Disp_Mode == MAIN_MODE_USERINFO)
	{
		if ((OnTimerCnt_33ms%TIME_CNT_33ms_100MS)==0)	// 100ms
		{
			static uint8_t tM7_UserIndex = 0xFE;
			static uint8_t tLanguage = 0xFE;
			static uint8_t tMUI_TPDT = 0xFE;
			static bool tgUI_Server_Mode = 0;
			uint8_t ReDisplay = 0;
			
			if ((M7_UserIndex!=tM7_UserIndex)||(User_RegInfo[M7_UserIndex].Language!=tLanguage)||(MUI.TPDT[0]!=tMUI_TPDT)||(tgUI_Server_Mode!=gUI_Server_Mode))
			{
				ReDisplay = 1;
				tM7_UserIndex=M7_UserIndex;
				tLanguage=User_RegInfo[M7_UserIndex].Language;
				tMUI_TPDT=MUI.TPDT[0];
				tgUI_Server_Mode=gUI_Server_Mode;
			}
			
			UI_BG(MUI.TPDT[0],M7_UserIndex,DISP_ON,ReDisplay);
			UI_UserName(M7_UserIndex,DISP_ON,ReDisplay);
			UI_UserIcon (M7_UserIndex,DISP_ON,ReDisplay);
			UI_UserEngineAuto(M7_UserIndex,DISP_ON,ReDisplay);
			UI_GripIconDisp(MUI.TPDT[0],ReDisplay);
		}	
		return 0;
	}
	 
#ifdef CAMERA_IN_MAIN	
//	if (Main_Disp_Mode == MAIN_MODE_CAMERA)
		CameraOnTimer();
#endif	

	if ((OnTimerCnt_33ms%TIME_CNT_33ms_100MS)==0)	// 100ms
	{
		char JD_Buf[10] = {0,};
		static int Cs_Cnt = 0;
		OnTimerCnt_100ms++;
		JogDialKey_LongEnter();
		if (RD_SEL_TUNE_LR_FLAG==1)
		{
			if ((RD_SEL_TUNE_L_CNT+TIME_CNT_500MS)<gMainTimerCnt_10m)
			{
				JD_Buf[0] = (RD_SEL_TUNE_L/10);
				JD_Buf[1] = (RD_SEL_TUNE_L%10);
				RD_MAIN_DnBt2(NULL, JD_Buf);
			}
			else if ((RD_SEL_TUNE_R_CNT+TIME_CNT_500MS)<gMainTimerCnt_10m)
			{
				JD_Buf[0] = (RD_SEL_TUNE_R/10);
				JD_Buf[1] = (RD_SEL_TUNE_R%10);
				RD_MAIN_DnBt2(NULL, JD_Buf);
			}
		}
		else if (RD_SEL_TONE_DU_FLAG==1)
		{
			if ((RD_SEL_TONE_D_CNT+TIME_CNT_500MS)<gMainTimerCnt_10m)
			{
				JD_Buf[0] = (RD_SEL_TONE_D/10);
				JD_Buf[1] = (RD_SEL_TONE_D%10);
				RD_MAIN_DnBt2(NULL, JD_Buf);
			}
			else if ((RD_SEL_TONE_U_CNT+TIME_CNT_500MS)<gMainTimerCnt_10m)
			{
				JD_Buf[0] = (RD_SEL_TONE_U/10);
				JD_Buf[1] = (RD_SEL_TONE_U%10);
				RD_MAIN_DnBt2(NULL, JD_Buf);
			}
		}
		else if (RD_SEL_VOL_DU_FLAG==1)
		{
			if ((RD_SEL_VOL_D_CNT+TIME_CNT_500MS)<gMainTimerCnt_10m)
			{
				JD_Buf[0] = (RD_SEL_VOL_D/10);
				JD_Buf[1] = (RD_SEL_VOL_D%10);
				RD_MAIN_DnBt2(NULL, JD_Buf);
			}
			else if ((RD_SEL_VOL_U_CNT+TIME_CNT_500MS)<gMainTimerCnt_10m)
			{
				JD_Buf[0] = (RD_SEL_VOL_U/10);
				JD_Buf[1] = (RD_SEL_VOL_U%10);
				RD_MAIN_DnBt2(NULL, JD_Buf);
			}
		}
		if (AC_SEL_DIR_LR_FLAG==1)
		{
			if ((AC_SEL_DIR_R_CNT+TIME_CNT_500MS)<gMainTimerCnt_10m)
			{
				JD_Buf[0] = AC_SEL_DIR_R;
//				AC_MAIN_UpBt2(NULL, JD_Buf);	// 2024-01-22 Spec V12
			}
			else if ((AC_SEL_DIR_L_CNT+TIME_CNT_500MS)<gMainTimerCnt_10m)
			{
				JD_Buf[0] = AC_SEL_DIR_L;
//				AC_MAIN_UpBt2(NULL, JD_Buf);	// 2024-01-22 Spec V12
			}
		}
		else if (AC_SEL_TEMP_LR_FLAG==1)
		{
			if ((AC_SEL_TEMP_R_CNT+TIME_CNT_500MS)<gMainTimerCnt_10m)
			{
				JD_Buf[0] = AC_SEL_TEMP_R;
				AC_MAIN_DnBt2(NULL, JD_Buf);
			}
			else if ((AC_SEL_TEMP_L_CNT+TIME_CNT_500MS)<gMainTimerCnt_10m)
			{
				JD_Buf[0] = AC_SEL_TEMP_L;
				AC_MAIN_DnBt2(NULL, JD_Buf);
			}
		}
		else if (AC_SEL_LEVEL_LR_FLAG==1)
		{
			if ((AC_SEL_LEVEL_R_CNT+TIME_CNT_500MS)<gMainTimerCnt_10m)
			{
				JD_Buf[0] = AC_SEL_LEVEL_R;
				AC_MAIN_DnBt2(NULL, JD_Buf);
			}
			else if ((AC_SEL_LEVEL_L_CNT+TIME_CNT_500MS)<gMainTimerCnt_10m)
			{
				JD_Buf[0] = AC_SEL_LEVEL_L;
				AC_MAIN_DnBt2(NULL, JD_Buf);
			}
		}		
	}
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_300MS)==0)	// 300ms
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_300ms++;

		IncrementData(Cs_Cnt,2,0)

		DAB3_Timer();//chpark.09.26
	}
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_500MS)==0)	// 500ms
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_500ms++;

		IncrementData(Cs_Cnt,2,0)
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
		MainDispDate();		// 2024-04-11 JPHong
		
		
		#ifdef USED_SAVE_CONFIG		// Test
//			XO_Save_Config(NULL,sf_Addr_ALL,NULL,M7_UserIndex,true,0);
		#endif
	}
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_5000MS)==0)
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_5S++;
	}
	#ifdef TEST_ERR_602
		TestCan0TaskTx();
	#endif	
    return true;
}
void ResetMain(void);
bool MainOnLeave(ITUWidget* widget, char* param)
{
	printf("MainOnLeave=[%s][%s][%d][%d][%d][%d]\n",widget,param,Main_Disp_Mode_Flag,Main_Disp_Mode,MainBtFlag,mainLayer);
	CameraOnLeaveCallSrc(36);	// JPHong 2024-05-10
	Main_Disp_Mode_Flag=0;
	ituWidgetSetVisible(mainLayer, DISP_OFF);
	MainLayer_ButtonSel(Main_Select_Index,DISP_OFF);
	if (Main_Disp_Mode == MAIN_MODE_PW)  //  UART
		ResetMain();
    return true;
}
bool JogDialRotation(ITUWidget* widget, char* param) 
{
//	printf("JogDialRotation= '0x%x'\n",param[0]);

	if (MainLayerFlag==0)
		return 0;
	if (Main_Disp_Mode==MAIN_MODE_USERINFO)
	{
		UserInfoJDR(NULL, param);
	}
	else
	{
		switch (Main_Disp_Mode)
		{
			case MAIN_MODE_NORMAL:
			{
				JogDialRot_MAIN_NORMAL(param);
			}
			break;
			case MAIN_MODE_AIRCON:
			{
				JogDialRot_MAIN_AC(param);
				Media_RunTime_Cnt = gMainTimerCnt_10m;
			}
			break;
			case MAIN_MODE_RADIO:
			case MAIN_MODE_DAB1:
			{
				JogDialRot_MAIN_RD(param);
				Media_RunTime_Cnt = gMainTimerCnt_10m;
			}
			break;
			case MAIN_MODE_DAB2:
			case MAIN_MODE_DAB3:
			{
				JogDialRot_MAIN_DAB2(param);
				Media_RunTime_Cnt = gMainTimerCnt_10m;
			}
			break;
			case MAIN_MODE_BLUETOOTH:
			{
				JogDialRot_MAIN_Bluetooth(param);
				Media_RunTime_Cnt = gMainTimerCnt_10m;
			}
			break;
			case MAIN_MODE_PHONE:
			{
				JogDialRot_MAIN_Phone(param);
			}
			break;
		
		}
	}
    return true;
}


bool Main_ATTBt_Func(ITUWidget* widget, char* param);  // 0BYTE:??? 0~3 , 1BYTE:LONG
bool Main_HMeterBt_Func(ITUWidget* widget, char* param);
bool Main_RadioBt_Func(ITUWidget* widget, char* param);
bool Main_ACBt_Func(ITUWidget* widget, char* param);


bool JogDialKey(ITUWidget* widget, char* param)
{

	if (MainLayerFlag==0)
		return 0;
	
	printf("\n\nJogDialKey= '0x%X' '%d'\n",param[0],Main_Disp_Mode);
	if (Main_Disp_Mode==MAIN_MODE_USERINFO)
	{
		UserInfoJDK(NULL, param);
	}
	else
	{
		switch (Main_Disp_Mode)
		{
			case MAIN_MODE_NORMAL:
			{
				JogDialKey_MAIN_NORMAL(param);
			}
			break;
			case MAIN_MODE_AIRCON:
			{
				JogDialKey_MAIN_AC(param);
				Media_RunTime_Cnt = gMainTimerCnt_10m;
			}
			break;
			case MAIN_MODE_RADIO:
			case MAIN_MODE_DAB1:
			{
				JogDialKey_MAIN_RD(param);
				Media_RunTime_Cnt = gMainTimerCnt_10m;
			}
			break;
			case MAIN_MODE_DAB2:
			case MAIN_MODE_DAB3:
			{
				JogDialKey_MAIN_DAB2(param);
				Media_RunTime_Cnt = gMainTimerCnt_10m;
			}
			break;
			case MAIN_MODE_BLUETOOTH:
			{
				JogDialKey_MAIN_Bluetooth(param);
				Media_RunTime_Cnt = gMainTimerCnt_10m;
			}
			break;
			case MAIN_MODE_PHONE:
			{
				JogDialKey_MAIN_Phone(param);
			}
			break;		
		}
	}
    return true;
}
bool MainHome(ITUWidget* widget, char* param);
bool MainMenu(ITUWidget* widget, char* param);
bool JogDialSwitch(ITUWidget* widget, char* param)
{
	char JD_Buf[10] = {0,};

	if (MainLayerFlag==0)
		return 0;
//	printf("\n\nJogDialSwitch= '%d'\n",param[0]);
	if (Main_Disp_Mode==MAIN_MODE_USERINFO)
	{
		UserInfoJDS(NULL, param);
	}
	else
	{	
		switch (param[0])
		{
			case JOG_DIAL_SW_ARICON:
			{
				JD_Buf[0] = 0;
				Main_ACBt_Func(NULL, JD_Buf);
			}
			break;
			case JOG_DIAL_SW_AC_AUTO:
			{
				JD_Buf[0] = AC_SEL_AUTO;
				AC_MAIN_UpBt(NULL, JD_Buf);
			}
			break;
			case JOG_DIAL_SW_AC_OFF:
			{
				JD_Buf[0] = AC_SEL_ONOFF;
				AC_MAIN_UpBt(NULL, JD_Buf);
			}
			break;
			//case JOG_DIAL_SW_ESC_ON:	// JPHong 2024-03-25
			case JOG_DIAL_SW_ESC_ON_OFF:
			{
				switch (Main_Disp_Mode)
				{
//					case MAIN_MODE_CAMERA:	//JPHong 2024-03-22
					case MAIN_MODE_NORMAL:
					{
						JD_Buf[0] = 0;
						MainMenu(NULL, JD_Buf);
					}
					break;
					case MAIN_MODE_AIRCON:
					{
						if ((AC_SEL_DIR_LR_FLAG)||(AC_SEL_TEMP_LR_FLAG)||(AC_SEL_LEVEL_LR_FLAG))
							AC_Toggle_Init();
						else
						{
							JD_Buf[0] = 0;
							MainMenu(NULL, JD_Buf);
						}
					}
					break;
					case MAIN_MODE_RADIO:
					case MAIN_MODE_BLUETOOTH:
					case MAIN_MODE_PHONE:
					{
						if ((RD_SEL_TUNE_LR_FLAG)||(RD_SEL_TONE_DU_FLAG)||(RD_SEL_VOL_DU_FLAG))
							RD_Toggle_Init();
						else
						{
							JD_Buf[0] = 0;
							MainMenu(NULL, JD_Buf);
						}
					}
					break;
					case MAIN_MODE_DAB1:			//  �� �κ� ���� ���� �ؾ� �Ѵ�. . 
					{
						if ((RD_SEL_TUNE_LR_FLAG)||(RD_SEL_TONE_DU_FLAG)||(RD_SEL_VOL_DU_FLAG))
							RD_Toggle_Init();
						else
						{
							JD_Buf[0] = 0;
							MainMenu(NULL, JD_Buf);
						}
					}
					break;
					case MAIN_MODE_DAB2:
					{
						//chpark.06.21
						if ((RD_SEL_TUNE_LR_FLAG)||(RD_SEL_TONE_DU_FLAG)||(RD_SEL_VOL_DU_FLAG)||(MainMode_DAB2_Select_Index == RD_SEL_SCROLL))
							RD_Toggle_Init();
						else
						{
							//chpark.24.10.10
							//JD_Buf[0] = 0;
							//MainMenu(NULL, JD_Buf);
							printf("[Main_Switch][JOG_DIAL_SW_ESC_ON_OFF][DAB2->DAB1]==%d\n",g_bDAB2_DataMode);
							Main_Disp_Mode = MAIN_MODE_DAB1;
						}
					}
					break;
					case MAIN_MODE_DAB3:
					{	
						//chpark.06.21
						if ((RD_SEL_TUNE_LR_FLAG)||(RD_SEL_TONE_DU_FLAG)||(RD_SEL_VOL_DU_FLAG)||(MainMode_DAB2_Select_Index == RD_SEL_SCROLL))
							RD_Toggle_Init();
						else
						{
							//chpark.24.10.10
							//JD_Buf[0] = 0;
							//MainMenu(NULL, JD_Buf);
							printf("[Main_Switch][JOG_DIAL_SW_ESC_ON_OFF][DAB3->DAB1]==%d\n",g_bDAB2_DataMode);
							Main_Disp_Mode = MAIN_MODE_DAB1;
						}
					}
					break;
				}
			}
			break;
			case JOG_DIAL_SW_HOME:
			{
				JD_Buf[0] = 0;
				MainHome(NULL, JD_Buf);
			}
			break;
			case JOG_DIAL_SW_AUDIO:
			{
				JD_Buf[0] = 0;
				Main_RadioBt_Func("JogDialSwitch", JD_Buf);
			}
			break;
		}
	}
    return true;
}









#define BUF_SIZE    		100*1024
#define FAC_FILE			"FAC_DATA.TXT"
#define SD_TEST_FILE		"SD_TEST.TXT"
#define CFG_UPGRADE_FOLDER 	"XO_UP.TXT"	//XO_FACUP

ITPDriveStatus* driveStatusTable;
ITPDriveStatus* driveStatus = NULL;


#if (LCD_MODULE_SEL == ADS_LCD)
	#define CFG_UPGRADE_FILENAME_LIST_ORG "XO80_17301-43843.PKG"
#elif (LCD_MODULE_SEL == ITS_LCD)
	#define CFG_UPGRADE_FILENAME_LIST_ORG "XO80_17301-43843_ITS.PKG"
#elif (LCD_MODULE_SEL == EMB_LCD)
	#define CFG_UPGRADE_FILENAME_LIST_ORG "XO80_17301-43843_EMB.PKG"
#else
	"Error!!!  No LCD module selected"
#endif

//A27C 17301-43843
//L85G 17301-43845
//8.0inchCluster　17301-43843(표준)　17301-43844(270deg사양)

#define CFG_UPGRADE_MICOM_FILE "Uart2CAN_App.hex"

bool CheckFirm_File(void)
{
	

    int i;
	uint8_t  tfilePath[256] = {0};
	static char pkgFilePath[PATH_MAX];
	FILE* file = NULL;
	
	
    // try to find the package drive
    ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_GET_TABLE, &driveStatusTable);

    for (i = ITP_MAX_DRIVE - 1; i >= 0; i--)
    {
        driveStatus = &driveStatusTable[i];
        if (driveStatus->avail && driveStatus->removable)
        {
            char buf[PATH_MAX], *ptr;

			printf("drive[%d]:disk=%d\n \n\n", i, driveStatus->disk);
			
#if 0			
            // get file path from list
            strcpy(buf, CFG_UPGRADE_FILENAME_LIST);
            ptr = strtok(buf, " ");
            do
            {
                strcpy(pkgFilePath, driveStatus->name);
                strcat(pkgFilePath, ptr);
				file = fopen(pkgFilePath, "r");
                if(file)
                {
			    	fclose(file);	
                    return 1;
                }
                else
                {
                    printf("try to fopen(%s) fail \n", pkgFilePath);
                }
            }
            while ((ptr = strtok(NULL, " ")) != NULL);
#endif            
            
            sprintf(tfilePath, "%s%s", driveStatus->name, CFG_UPGRADE_FILENAME_LIST_ORG);
  
		 	file = fopen(tfilePath, "r");
		    if(!file)
		    {
		        printf("Can not File : %s \n\n", tfilePath);
		        return 0;
		    }
		    fclose(file);
		    printf(" file name0: %s \n", CFG_UPGRADE_FILENAME_LIST_ORG);
		    return 1;
        }
    }
}

uint8_t* MicomFileBuff = NULL;
int MicomFileSize = 0;
bool CheckFAC_File(void)
{
    int i;
	uint8_t  tfilePath[256] = {0};
	static char pkgFilePath[PATH_MAX];
	FILE* file = NULL;
	
    // try to find the package drive
    ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_GET_TABLE, &driveStatusTable);

    for (i = ITP_MAX_DRIVE - 1; i >= 0; i--)
    {
        driveStatus = &driveStatusTable[i];
        if (driveStatus->avail && driveStatus->removable)
        {
            char buf[PATH_MAX], *ptr;

			printf("drive[%d]:disk=%d\n \n\n", i, driveStatus->disk);
            // get file path from list
            
            sprintf(tfilePath, "%s%s", driveStatus->name, FAC_FILE);
  
		 	file = fopen(tfilePath, "r");
		    if(!file)
		    {
		        printf("Can not File : %s \n\n", tfilePath);
		        return 0;
		    }
		    fclose(file);	
		    return 1;
        }
    }
}

bool CheckFirm_Micom(void)
{
    int i;
	uint8_t  tfilePath[256] = {0};
	static char pkgFilePath[PATH_MAX];
	FILE* file = NULL;
	struct stat sb;
	
    // try to find the package drive
    ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_GET_TABLE, &driveStatusTable);

    for (i = ITP_MAX_DRIVE - 1; i >= 0; i--)
    {
        driveStatus = &driveStatusTable[i];
        if (driveStatus->avail && driveStatus->removable)
        {
            char buf[PATH_MAX], *ptr;

			printf("drive[%d]:disk=%d\n \n\n", i, driveStatus->disk);
            // get file path from list
            strcpy(buf, CFG_UPGRADE_MICOM_FILE);
            ptr = strtok(buf, " ");
            do
            {
                strcpy(pkgFilePath, driveStatus->name);
                strcat(pkgFilePath, ptr);
				file = fopen(pkgFilePath, "r");
                if(file)
                {
				    if (fstat(fileno(file), &sb) == -1)
				    {
				        printf("get file size fail\n");
				        fclose(file);
				        return 0;
				    }
		
				    MicomFileBuff = malloc(sb.st_size);
				    if (!MicomFileBuff)
				    {
				    	printf("MicomFileBuff fail\n");
				    	fclose(file);
				        return 0;
					}
				    MicomFileSize = fread(MicomFileBuff, 1, sb.st_size, file);
		                    
					fclose(file);
					for (i=0;i<16;i++)
						printf("0x%02X,",MicomFileBuff[i]);
					printf("\r\n");					
					return 1;
                }
                else
                {
                    printf("try to fopen(%s) fail \n", pkgFilePath);
                }
            }
            while ((ptr = strtok(NULL, " ")) != NULL);
            
		    return 0;
        }
    }
}

bool Main_SD_Inserted(ITUWidget* widget, char* param)
{
	
	printf("[%s]\n", __FUNCTION__);
    return true;
}

static uint8_t Firm_UP_Flag = 0;
static uint8_t Fac_TestModeFlag = 0;	//0:Off , 1:On , 2:Camera , 3:End  //chpark.24.04.24



void ReBootFunc(void)
{
	printf("\n[%s]\n", __FUNCTION__);
	printf("[%s]\n", __FUNCTION__);
	printf("[%s]\n", __FUNCTION__);
	printf("[%s]\n\n", __FUNCTION__);
	#if 1
    sleep(2);
    itp_codec_standby();
    exit(0);
    
    
    Reboot();
    #endif
}

extern void XO_MICOM_EXIT(void);


bool DeleteUpgradeCheck(void)
{
    int i;
	uint8_t  tfilePath[256] = {0};
	static char pkgFilePath[PATH_MAX];
	FILE* file = NULL;
	
    // try to find the package drive
    ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_GET_TABLE, &driveStatusTable);

    for (i = ITP_MAX_DRIVE - 1; i >= 0; i--)
    {
        driveStatus = &driveStatusTable[i];
        if (driveStatus->avail && driveStatus->removable)
        {
            char buf[PATH_MAX], *ptr;

			printf("drive[%d]:disk=%d\n \n\n", i, driveStatus->disk);
            // get file path from list
            
            sprintf(tfilePath, "%s%s", driveStatus->name, CFG_UPGRADE_FOLDER);
  
		 	file = fopen(tfilePath, "r");
		    if(!file)
		    {
		        printf("Can not File : %s \n\n", tfilePath);
		        return 0;
		    }
		    fclose(file);	
		    return 1;
        }
    }
}



bool DeleteMicomFirm(void)
{
    int i;
	uint8_t  tfilePath[256] = {0};
	static char pkgFilePath[PATH_MAX];
	FILE* file = NULL;
	
	
	if (DeleteUpgradeCheck()==0)
	{	
	    // try to find the package drive
	    ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_GET_TABLE, &driveStatusTable);
	
	    for (i = ITP_MAX_DRIVE - 1; i >= 0; i--)
	    {
	        driveStatus = &driveStatusTable[i];
	        if (driveStatus->avail && driveStatus->removable)
	        {
	            char buf[PATH_MAX], *ptr;
	
				printf("drive[%d]:disk=%d\n \n\n", i, driveStatus->disk);
	            // get file path from list
	            
	            sprintf(tfilePath, "%s%s", driveStatus->name, CFG_UPGRADE_MICOM_FILE);
			    if (remove(tfilePath))
			    {
					printf("DeleteMicomFirm : Delete %s fail \n", tfilePath);
					return 0;
				}
				else
				{
					printf("DeleteMicomFirm : Delete %s success \n", tfilePath);
					return 1;
				}
	        }
	    }
    }
    return 0;
}

bool UpdateMainFirm(void)
{
	if (CheckFirm_File()==1)
    {
        printf(" file name1: %s \n", CFG_UPGRADE_FILENAME_LIST);

		if (Firm_UP_Flag==0)
		{
			Firm_UP_Flag = 1;
			XO_MICOM_EXIT();
			ReBootFunc();
		}
    }
    else
    	printf(" Not found Firmware Filee : [%s]\n",CFG_UPGRADE_FILENAME_LIST);
}

bool Main_USB_Inserted(ITUWidget* widget, char* param)
{
	printf("[%s]\n", __FUNCTION__);

    if (CheckFAC_File()==1)
    {
       // sprintf(filePath, "%s%s", driveStatus.name, FAC_FILE);
       // printf(" file name: %s \n", filePath);

		//if (g_nFac_TestModeFlag != 1) //for test
		if (1)
		{
			g_nFac_TestModeFlag = 1;
			GotoFacMode(0);
			return true;
		}
		else
		{
			GotoFacMode(-1);
			printf("[Main_USB_Inserted][Not FAC MODE]:Fac_TestModeFlag=%d\n", Fac_TestModeFlag);
		}	           
    }
	else
	{
		GotoFacMode(-1);
	}



	if (CheckFirm_Micom()==1)
    {
    	uint8_t RetVal = 0;
        printf(" file name1: %s \n", CFG_UPGRADE_MICOM_FILE);

		if (MicomFirm_UP_Flag==0)
		{
			MicomFirm_UP_Flag = 1;
			MicomFirm_ReciveData = 0;
			RetVal = CheckMicomFirm();
			if (RetVal==1)
			{
				printf("MicomFirm : [%s] Start Update [0x%X]!!!\n",CFG_UPGRADE_MICOM_FILE,RetVal);
				Goto_MC100_Mode(0);//chpark.24.05.14
			}
			else
			{
				MicomFirm_UP_Flag=0;
				printf("MicomFirm : [%s]File Error [0x%X]!!!\n",CFG_UPGRADE_MICOM_FILE,RetVal);
			}
		}
    }
    else
    {
    	printf(" Not found Micom File : [%s]\n",CFG_UPGRADE_MICOM_FILE);
    	    
   		UpdateMainFirm();
    }
    return true;
}
bool TestBt1(ITUWidget* widget, char* param)
{
	if (MainLayerFlag==0)
		return 0;
	printf("TestBt1= '%s'\n",param);
	
	if ((param[0]&0x0F)==0)
	{
		ReBootFunc();
	}
    return true;
}

bool MainCamera(ITUWidget* widget, char* param)
{
	static int CamMode=4;
	if (MainLayerFlag==0)
		return 0;
	printf("MainCamera= [%d] [%s] [%d][%d]\n",(param[0]&0x0F),widget,MainBtFlag,Main_Disp_Mode);
	if ((param[0]&0x0F)==1)
	{		
		switch (Main_Disp_Mode)
		{
			case MAIN_MODE_RADIO:	// Radio
				MainMode_RD_Select_Index = MAIN_SEL_CAMERA;
				MainLayer_ButtonSel(MainMode_RD_Select_Index,DISP_ON);
			break;
			case MAIN_MODE_AIRCON:	// Aircon
				MainMode_AC_Select_Index = MAIN_SEL_CAMERA;
				MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
			break;	
			case MAIN_MODE_DAB1:
				MainMode_DAB_Select_Index = MAIN_SEL_CAMERA;
				MainLayer_ButtonSel(MainMode_DAB_Select_Index,DISP_ON);
			break;		
			case MAIN_MODE_DAB2:
			case MAIN_MODE_DAB3:
				MainMode_DAB2_Select_Index = MAIN_SEL_CAMERA;
				MainLayer_ButtonSel(MainMode_DAB2_Select_Index,DISP_ON);
			break;		
			case MAIN_MODE_BLUETOOTH:	// 
				MainMode_BT_Select_Index = MAIN_SEL_CAMERA;
				MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
			break;
			case MAIN_MODE_PHONE:	// Phone
				MainMode_Phone_Select_Index = MAIN_SEL_CAMERA;
				MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
			break;
			default:
				Main_Select_Index = MAIN_SEL_CAMERA;
				MainLayer_ButtonSel(Main_Select_Index,DISP_ON);
			break;
		}		
	}
	else if ((param[0]&0x0F)==0)
	{
		Main_CLI2[6] = Main_CLI2[6] | 0x80;
	}	
    return true;
}

// Test for 2 itu file rotation
bool RotationButtonOnPress(void)
{
	uint8_t tPL_Flag_Manual = Get_PL_Flag();
	
//	printf("RotationButtonOnPress= '%d' '%d'\n",Get_PL_Flag(),RotationDir());
	
	if (tPL_Flag_Manual!=PL_Flag_Manual)
	{
		switch(tPL_Flag_Manual)
		{
			case PORTRAIT_2:
				PL_Flag_Manual = PORTRAIT_2;
				ExternalSetOrientation(EXTERNAL_VERTICAL);
				SceneChange(ITU_ROT_0);
				break;
				
			case LANDSCAPE_1:
				PL_Flag_Manual = LANDSCAPE_1;
				ExternalSetOrientation(EXTERNAL_HORIZONTAL);
				SceneChange(ITU_ROT_90);
				break;
	
			case PORTRAIT_1:
				PL_Flag_Manual = PORTRAIT_1;
				ExternalSetOrientation(EXTERNAL_VERTICAL);
				SceneChange(ITU_ROT_180);
				break;
	
			case LANDSCAPE_2:
				PL_Flag_Manual = LANDSCAPE_2;
				ExternalSetOrientation(EXTERNAL_HORIZONTAL);
				SceneChange(ITU_ROT_270);
				break;
		}
		return true;
	}
	return 0;
}


bool MainHome(ITUWidget* widget, char* param)
{
	if (MainLayerFlag==0)
		return 0;
	printf("MainHomeBt= [0x%02X]\n",param[0]&0x0F);
	
// TestSetUserData();		// Test.. JPHong 2024-03-20

	if ((param[0]&0x0F)==1)
	{
		switch (Main_Disp_Mode)
		{
			case MAIN_MODE_RADIO:	// Radio
				MainMode_RD_Select_Index = MAIN_SEL_MENU;
				MainLayer_ButtonSel(MainMode_RD_Select_Index,DISP_ON);
			break;
			case MAIN_MODE_AIRCON:	// Aircon
				MainMode_AC_Select_Index = MAIN_SEL_MENU;
				MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
			break;	
			case MAIN_MODE_DAB1:
				MainMode_DAB_Select_Index = MAIN_SEL_MENU;
				MainLayer_ButtonSel(MainMode_DAB_Select_Index,DISP_ON);
			break;		
			case MAIN_MODE_DAB2:
			case MAIN_MODE_DAB3:
				MainMode_DAB2_Select_Index = MAIN_SEL_MENU;
				MainLayer_ButtonSel(MainMode_DAB2_Select_Index,DISP_ON);
			break;		
			case MAIN_MODE_BLUETOOTH:	// 
				MainMode_BT_Select_Index = MAIN_SEL_MENU;
				MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
			break;		
			case MAIN_MODE_PHONE:	// Phone
				MainMode_Phone_Select_Index = MAIN_SEL_MENU;
				MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
			break;
		}
	}
	else if ((param[0]&0x0F)==0)
	{
		#ifdef TEST_ERR_602
		TestCan0TaskTxFlag++;
		#endif
		Phone0Byte=0;
		MainBtFlag = MAIN_MODE_NORMAL;
	}
  return true;
}

bool MainMenu(ITUWidget* widget, char* param)
{
	if (MainLayerFlag==0)
		return 0;
	printf("MainMenu= '%s'\n",param);
	//chpark.24.07.02
	if(g_nFac_TestModeFlag==1)
	{
		if(Main_Disp_Mode == MAIN_MODE_CAMERA)
		{
//			printf("[g_nFac_TestModeFlag][MAIN_MODE_CAMERA][D5]==%d\n",g_nFac_TestModeFlag);//chpar.11.13
			GotoFacMode(4);//LCD test->End(T_STEP_LCD_WHITE)
			return false;
		}
	}
	//

	if ((param[0]&0x0F)==1)
	{
		MainLayer_ButtonSel(MAIN_SEL_MENU,DISP_ON);
	}
	else if ((param[0]&0x0F)==0)
	{
		printf("====================MainMenu==GotoMenuTop==%d\n",g_bDAB2_DataMode);//chpar.11.13
		Phone0Byte=0;
		g_bDAB2_DataMode  = true;    // JPHong 2023-11-13
//		GotoMenuTop();
		GotoMain(MAIN_MODE_MENU);
	}
    return true;
}

bool Main_SPK_Bt_Func(ITUWidget* widget, char* param)
{
	printf("Main_SPK_Bt_Func= '%s'\n",param[0]&0x0F);
	Phone0Byte=0;
	if ((param[0]&0x0F)==1)
	{
		MainLayer_ButtonSel(MAIN_SEL_SPK,DISP_ON);
	}
	else if ((param[0]&0x0F)==0)
	{
		MainLayer_ButtonSel(MAIN_SEL_SPK,DISP_ON);
		Main_CLI2[4] = Main_CLI2[4] | 0x04;
	}
	
	switch (Main_Disp_Mode)
	{
		case MAIN_MODE_AIRCON:
		{
			MainMode_AC_Select_Index = AC_SEL_SPK;
		}
		break;
		case MAIN_MODE_RADIO:
		{
			MainMode_RD_Select_Index = RD_SEL_SPK;
		}
		break;
		case MAIN_MODE_DAB1:
		{
			MainMode_DAB_Select_Index = RD_SEL_SPK;
		}
		break;
		case MAIN_MODE_DAB2:
		case MAIN_MODE_DAB3:
		{
			MainMode_DAB2_Select_Index = RD_SEL_SPK;
		}
		break;
		case MAIN_MODE_BLUETOOTH:
		{
			MainMode_BT_Select_Index = RD_SEL_SPK;
		}
		break;
		case MAIN_MODE_PHONE:
		{
			MainMode_Phone_Select_Index = RD_SEL_SPK;
		}
		break;
	}
			
    return true;
}

bool Main_ATTBt_Func(ITUWidget* widget, char* param)  // 0BYTE:??? 0~3 , 1BYTE:LONG
{
	printf("Start : Main_ATTBt_Func= [0x%0X]\n",param[0]&0x0F);
	if ((param[1]&0x0F)!=0)	//Long
	{
		MainBtFlag = MAIN_MODE_MENU_ATT;
	}
	else	
	{
		if (Main_Select_Index!=MAIN_SEL_ATT)
		{
			MainLayer_ButtonSel(MAIN_SEL_ATT,DISP_ON);
		}
		else
		{
			switch((param[0]&0x0F))
			{
				case 0:
				break;
				case 1:
					AUX1CurrentOutMode = 0;
					MainLayer_FuncATTDisp(AUX1CurrentOutMode,M7_UserSet[0].ATT_State-1,M7_UserSet[1].ATT_State-1,M7_UserSet[2].ATT_State-1,1,DISP_ON);
					#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
						// AUX1CurrentOutMode
						XO_Save_Config(M7_UserIndex,true,0);
					#else
						HT_Save_Config(1);
					#endif
				break;
				case 2:
					AUX1CurrentOutMode = 1;
					MainLayer_FuncATTDisp(AUX1CurrentOutMode,M7_UserSet[0].ATT_State-1,M7_UserSet[1].ATT_State-1,M7_UserSet[2].ATT_State-1,1,DISP_ON);
					#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
						// AUX1CurrentOutMode
						XO_Save_Config(M7_UserIndex,true,0);
					#else
						HT_Save_Config(1);
					#endif
				break;
				case 3:
					AUX1CurrentOutMode = 2;
					MainLayer_FuncATTDisp(AUX1CurrentOutMode,M7_UserSet[0].ATT_State-1,M7_UserSet[1].ATT_State-1,M7_UserSet[2].ATT_State-1,1,DISP_ON);
					#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
						// AUX1CurrentOutMode
						XO_Save_Config(M7_UserIndex,true,0);
					#else
						HT_Save_Config(1);
					#endif
				break;
			}
		}
	}
	printf("End : Main_ATTBt_Func= [0x%0X]\n",param[0]&0x0F);
    return true;
}

extern uint32_t tTimePM;
extern uint32_t tTimeLiftLd;
extern uint32_t tTimeEngRPM;
extern uint32_t tTimeEngLd;
extern uint32_t tTimeASH;
	 
bool Main_BarBt_Func(ITUWidget* widget, char* param)
{
	printf("Start : Main_BarBt_Func= 0x%X,%d\n",param[0],MRV.Symbol);
	if ((param[0]&0x0F)==1)
	{
		MainLayer_ButtonSel(MAIN_SEL_BAR,DISP_ON);
	}
	else if ((param[0]&0x0F)==0)
	{
		MainLayer_ButtonSel(MAIN_SEL_BAR,DISP_ON);
		switch(MRV.Symbol)
		{
			case m11_MeterSymbol_PM:
			{
				if (MRV.ASH!=0xFF)
				{
					MRV.Symbol = m11_MeterSymbol_AHS;
					tTimeASH = gMainTimerCnt_10m;
				}
				else
				{
					MRV.Symbol = m13_MeterSymbol_EngRPM;
					tTimeEngRPM = gMainTimerCnt_10m;
				}
			}
			break;
			case m12_MeterSymbol_LiftLd:
			{
				if (MRV.PM!=0xFF)
				{
					MRV.Symbol = m11_MeterSymbol_PM;
					tTimePM = gMainTimerCnt_10m;
				}
				else if (MRV.ASH!=0xFF)
				{
					MRV.Symbol = m11_MeterSymbol_AHS;
					tTimeASH = gMainTimerCnt_10m;
				}
				else 
				{
					MRV.Symbol = m13_MeterSymbol_EngRPM;
					tTimeEngRPM = gMainTimerCnt_10m;
				}
			}
			break;
			case m13_MeterSymbol_EngRPM:
			{
				if (MRV.EngLd!=0xFF)
				{
					MRV.Symbol = m14_MeterSymbol_EngLd;
					tTimeEngLd = gMainTimerCnt_10m;
					if (MeterLiftLdFlag!=0)
						tTimeEngRPM = gMainTimerCnt_10m;
				}
				else if ((MRV.LiftLd!=0xFF)||(MeterLiftLdFlag!=0))
				{
					MRV.Symbol = m12_MeterSymbol_LiftLd;
					tTimeLiftLd = gMainTimerCnt_10m;
				}
				else if (MRV.PM!=0xFF)
				{
					MRV.Symbol = m11_MeterSymbol_PM;
					tTimePM = gMainTimerCnt_10m;
				}
				else if (MRV.ASH!=0xFF)
				{
					MRV.Symbol = m11_MeterSymbol_AHS;
					tTimeASH = gMainTimerCnt_10m;
				}
				else 
				{
					MRV.Symbol = m13_MeterSymbol_EngRPM;
					tTimeEngRPM = gMainTimerCnt_10m;
				}
			}
			break;
			case m14_MeterSymbol_EngLd:
			{
				if ((MRV.LiftLd!=0xFF)||(MeterLiftLdFlag!=0))
				{
					MRV.Symbol = m12_MeterSymbol_LiftLd;
					tTimeLiftLd = gMainTimerCnt_10m;
				}
				else if (MRV.PM!=0xFF)
				{
					MRV.Symbol = m11_MeterSymbol_PM;
					tTimePM = gMainTimerCnt_10m;
				}
				else if (MRV.ASH!=0xFF)
				{
					MRV.Symbol = m11_MeterSymbol_AHS;
					tTimeASH = gMainTimerCnt_10m;
				}
				else 
				{
					MRV.Symbol = m13_MeterSymbol_EngRPM;
					tTimeEngRPM = gMainTimerCnt_10m;
				}
			}
			break;
			case m11_MeterSymbol_AHS:
			{
				MRV.Symbol = m13_MeterSymbol_EngRPM;
				tTimeEngRPM = gMainTimerCnt_10m;
			}
			break;
		}
		switch (Main_Disp_Mode)
		{
			case MAIN_MODE_AIRCON:
			{
				MainMode_AC_Select_Index = AC_SEL_BAR;
				Media_RunTime_Cnt = gMainTimerCnt_10m;
			}
			break;
			case MAIN_MODE_RADIO:
			{
				MainMode_RD_Select_Index = RD_SEL_BAR;
				Media_RunTime_Cnt = gMainTimerCnt_10m;
			}
			break;
			case MAIN_MODE_DAB1:
			{
				MainMode_DAB_Select_Index = RD_SEL_BAR;
				Media_RunTime_Cnt = gMainTimerCnt_10m;
			}
			break;
			case MAIN_MODE_DAB2:
			case MAIN_MODE_DAB3:
			{
				MainMode_DAB2_Select_Index = RD_SEL_BAR;
				Media_RunTime_Cnt = gMainTimerCnt_10m;
			}
			break;
			case MAIN_MODE_BLUETOOTH:
			{
				MainMode_BT_Select_Index = RD_SEL_BAR;
				Media_RunTime_Cnt = gMainTimerCnt_10m;
			}
			break;
			case MAIN_MODE_PHONE:
			{
				MainMode_Phone_Select_Index = RD_SEL_BAR;
			}
			break;
		}
	}
	printf("End : Main_BarBt_Func= 0x%X,%d\n",param[0],MRV.Symbol);
    return true;
}
bool Main_PwrBt_Func(ITUWidget* widget, char* param)
{
	uint8_t tData;
	printf("Main_PwrBt_Func= 0x%X 0x%X\n",param[0],param[1]);
	
	if ((param[0]&0x0F)==1)
	{
		MainLayer_ButtonSel(MAIN_SEL_POWER,DISP_ON);
		switch (Main_Disp_Mode)
		{
			case MAIN_MODE_AIRCON:
			{
				MainMode_AC_Select_Index = AC_SEL_POWER;
			}
			break;
			case MAIN_MODE_RADIO:
			{
				MainMode_RD_Select_Index = RD_SEL_POWER;
			}
			case MAIN_MODE_DAB1:
			{
				MainMode_DAB_Select_Index = RD_SEL_POWER;
			}
			break;
			case MAIN_MODE_DAB2:
			case MAIN_MODE_DAB3:
			{
				MainMode_DAB2_Select_Index = RD_SEL_POWER;
			}
			break;
			case MAIN_MODE_BLUETOOTH:
			{
				MainMode_BT_Select_Index = RD_SEL_POWER;
			}
			break;
			case MAIN_MODE_PHONE:
			{
				MainMode_Phone_Select_Index = RD_SEL_POWER;
			}
			break;
		}
	}
	else if ((param[0]&0x0F)==0)
	{
		if ((AUI1_0B_3210b==1)||(AUI1_0B_3210b==2))
		{
			Main_CLI3[3] = Main_CLI3[3]|0x08;
		}
	}
    return true;
}

bool Main_HMeterBt_Func(ITUWidget* widget, char* param)
{
	static uint8_t LognFlag = 0;
	printf("Main_HMeterBt_Func= 0x%X 0x%X\n",param[0],param[1]);
	if (Main_Select_Index!=MAIN_SEL_HOURMETER)
	{
		if ((Main_Disp_Mode == MAIN_MODE_DAB2)||(Main_Disp_Mode == MAIN_MODE_DAB3))
		{
			if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
			{
				DAB2ListBox_Select(-1);
			}
			else
			{
				DAB2ListBoxH_Select(-1);
			}
		}
		MainLayer_ButtonSel(MAIN_SEL_HOURMETER,DISP_ON);
		LognFlag = 0;
	}
	else
	{
		MainLayer_ButtonSel(MAIN_SEL_HOURMETER,DISP_ON);
		if ((param[1]&0x0F)!=0)	//Long
		{
			LognFlag = 1;
			switch (HMV.HourMeter_Trip)
			{
				case 1:
					HMV.HourMeter_Trip_Clear1 = HMV.HourMeter_Trip_Clear1|0x01;
				break;
				case 2:
					HMV.HourMeter_Trip_Clear1 = HMV.HourMeter_Trip_Clear1|0x04;
				break;
				case 3:
					HMV.HourMeter_Trip_Clear1 = HMV.HourMeter_Trip_Clear1|0x10;
				break;
			}
		}
		else
		{
			if (((param[0]&0x0F)==0)&&(LognFlag == 0))
			{
				HMV.HourMeter_Trip++;
				if (HMV.HourMeter_Trip>=4)
					HMV.HourMeter_Trip = 0;
					
				#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
					//HMV.HourMeter_Trip
					XO_Save_Config(M7_UserIndex,true,0);
				#else
					HT_Save_Config(1);
				#endif
			}
			LognFlag = 0;
		}
		switch (Main_Disp_Mode)
		{
			case MAIN_MODE_AIRCON:
			{
				MainMode_AC_Select_Index = AC_SEL_HOURMETER;
			}
			break;
			case MAIN_MODE_RADIO:
			{
				MainMode_RD_Select_Index = RD_SEL_HOURMETER;
			}
			case MAIN_MODE_DAB1:
			{
				MainMode_DAB_Select_Index = RD_SEL_HOURMETER;
			}
			break;
			case MAIN_MODE_DAB2:
			case MAIN_MODE_DAB3:
			{
				MainMode_DAB2_Select_Index = RD_SEL_HOURMETER;
			}
			break;
			case MAIN_MODE_BLUETOOTH:
			{
				MainMode_BT_Select_Index = RD_SEL_HOURMETER;
			}
			break;
			case MAIN_MODE_PHONE:
			{
				MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
			}
			break;
		}
	}
    return true;
}

bool Main_TimeDateBt_Func(ITUWidget* widget, char* param)
{
	printf("Main_TimeDateBt_Func= '%s'\n",param);
	
	if ((param[1]&0x0F)!=0)	//Long
	{
		MainBtFlag = MAIN_MODE_MENU_TIME;
	}
	if (Main_Select_Index!=MAIN_SEL_TIMEDATE)
	{
//		MainLayer_ButtonSel(MAIN_SEL_TIMEDATE,DISP_ON);
	}
    return true;
}

bool Main_RadioBt_Func(ITUWidget* widget, char* param)
{
	uint8_t Disp_Mode=0;
	printf("Main_RadioBt_Func Start= [%s]%d %d %d,[%d][%d]\n",widget,(param[0]&0x0F),RDV.RDPhone,RDV.RDMode,MainBtFlag,Main_Disp_Mode);
	
		
	if ((param[0]&0x0F)==1)
	{
		//if (Main_Select_Index!=MAIN_SEL_RADIO)
		{
			MainLayer_ButtonSel(MAIN_SEL_RADIO,DISP_ON);
		}
	}
	else
	{
		if (Main_Disp_Mode!=MAIN_MODE_RADIO)
			MainBtFlag = MAIN_MODE_RADIO;
	}

	switch (Main_Disp_Mode)
	{
		case MAIN_MODE_AIRCON:
		{
			MainMode_AC_Select_Index = RD_SEL_RADIO;
		}
		break;
		case MAIN_MODE_RADIO:
		{
			MainMode_RD_Select_Index = RD_SEL_RADIO;
		}
		break;
		case MAIN_MODE_DAB1:
		{
			MainMode_DAB_Select_Index = RD_SEL_RADIO;
		}
		break;
		case MAIN_MODE_DAB2:
		case MAIN_MODE_DAB3:
		{
			MainMode_DAB2_Select_Index = RD_SEL_RADIO;
		}
		break;
		case MAIN_MODE_BLUETOOTH:
		{
			MainMode_BT_Select_Index = RD_SEL_RADIO;
		}
		break;
		case MAIN_MODE_PHONE:
		{
			MainMode_Phone_Select_Index = RD_SEL_RADIO;
		}
		break;
	}
	printf("Main_RadioBt_Func End= [%s]%d %d %d,[%d][%d]\n",widget,(param[0]&0x0F),RDV.RDPhone,RDV.RDMode,MainBtFlag,Main_Disp_Mode);
    return true;
}

bool Main_ACBt_Func(ITUWidget* widget, char* param)
{
	uint8_t Disp_Mode=0;
	printf("Main_ACBt_Func= '0x%02X''0x%02X'\n",param[0],param[1]);
	
	
	if ((param[0]&0x0F)==1)
	{
		//if (Main_Select_Index!=MAIN_SEL_AIRCON)
		{
			MainLayer_ButtonSel(MAIN_SEL_AIRCON,DISP_ON);
		}
	}
	else
	{
		Phone0Byte=0;
		if (Main_Disp_Mode!=MAIN_MODE_AIRCON)
			MainBtFlag = MAIN_MODE_AIRCON;
	}

	switch (Main_Disp_Mode)
	{
		case MAIN_MODE_AIRCON:
		{
			MainMode_AC_Select_Index = AC_SEL_AIRCON;
		}
		break;
		case MAIN_MODE_RADIO:
		{
			MainMode_RD_Select_Index = AC_SEL_AIRCON;
		}
		break;
		case MAIN_MODE_DAB1:
		{
			MainMode_DAB_Select_Index = AC_SEL_AIRCON;
		}
		break;
		case MAIN_MODE_DAB2:
		case MAIN_MODE_DAB3:
		{
			MainMode_DAB2_Select_Index = AC_SEL_AIRCON;
		}
		break;
		case MAIN_MODE_BLUETOOTH:
		{
			MainMode_BT_Select_Index = AC_SEL_AIRCON;
		}
		break;
		case MAIN_MODE_PHONE:
		{
			MainMode_Phone_Select_Index = AC_SEL_AIRCON;
		}
		break;
	}
    return true;
}

bool Main_WarringBt_Func(ITUWidget* widget, char* param)
{
	printf("Main_WarringBt_Func= '%s'\n",param);
	if (((MainWarnigData[cW_01_B]&0x80)!=0)||((MainWarnigData[cW_02_B]&0x80)!=0)||((MainWarnigData[cW_03_B]&0x80)!=0))
	{
		MainBtFlag = MAIN_MODE_MENU_WARRING;
		Phone0Byte=0;
	}
	if (Main_Select_Index!=MAIN_SEL_WARRING)
	{
		MainLayer_ButtonSel(MAIN_SEL_WARRING,DISP_ON);
	}
    return true;
}




bool W_01_B_Stop(ITUWidget* widget, char* param)
{
	printf("W_01_B_Stop=%d,0x%x , 0x%x\n",widget,widget,param);
	Main_wZoomIndex = 0xFF;
	Main_ZoomSetDataW (0x40 , 0x80);
	return true;
}
bool F_01_B_Stop(ITUWidget* widget, char* param)
{
//	printf("F_01_B_Stop=[%d],0x%x , 0x%x\n",gMainTimerCnt_10m,widget,param);
	Main_fZoomIndex = 0xFF;
	Main_ZoomSetDataF (0x40 , 0x80);
	return true;
}


uint8_t RD_MAIN_DnBt_Long=0;
extern uint8_t DAB2_PTY_Index;

bool RD_MAIN_DnBt(ITUWidget* widget, char* param)
{
	static uint8_t sMM_RD_SI = 0xFF;
	uint8_t tsMM_RD_SI = 0xFF;
	
	tsMM_RD_SI = (param[0]&0xF)*10+(param[1]&0xF);
	
	printf("RD_MAIN_DnBt=%d,%d,%d,%d,%d\n",tsMM_RD_SI,param[0]&0x0F,param[1]&0x0F,param[2]&0x1F,MainMode_RD_Select_Index);
	
	Media_RunTime_Cnt = gMainTimerCnt_10m;
	if (((RD_SEL_TUNE_LR_FLAG==0)&&(RD_SEL_TONE_DU_FLAG==0)&&(RD_SEL_VOL_DU_FLAG==0))||(sMM_RD_SI!=tsMM_RD_SI))
	{
		if ((RD_SEL_TUNE_LR_FLAG)||(RD_SEL_TONE_DU_FLAG)||(RD_SEL_VOL_DU_FLAG))
			InitJogIncDecData_RD();
		sMM_RD_SI=tsMM_RD_SI;
		MainMode_RD_Sel(1,sMM_RD_SI,DISP_ON);
	}
	sMM_RD_SI=tsMM_RD_SI;
	if ((param[2]&0x1F)==17)	//long
	{
		if (RD_MAIN_DnBt_Long==(param[2]&0x1F))
		{
			RD_MAIN_DnBt_Long=(param[2]&0x1F);
			
			if ((Main_Disp_Mode == MAIN_MODE_DAB2)||(Main_Disp_Mode == MAIN_MODE_DAB3))
			{
				//DAB List?? ????

			}
			else
			{		
			}
		}
		else
		{
			RD_MAIN_DnBt_Long=(param[2]&0x1F);
		}
	}
	else
	{
		RD_MAIN_DnBt_Long=0;
		switch (sMM_RD_SI)
		{
			case RD_SEL_1C:
				Main_CLI2[4] = Main_CLI2[4] | 0x20;
			break;
			case RD_SEL_2C:
				Main_CLI2[4] = Main_CLI2[4] | 0x40;
			break;
			case RD_SEL_3C:
				Main_CLI2[4] = Main_CLI2[4] | 0x80;
			break;
			case RD_SEL_4C:
				Main_CLI2[5] = Main_CLI2[5] | 0x01;
			break;
			case RD_SEL_5C:
				Main_CLI2[5] = Main_CLI2[5] | 0x02;
			break;
			case RD_SEL_6C:
				Main_CLI2[5] = Main_CLI2[5] | 0x04;
			break;
			case RD_SEL_7C:
				Main_CLI2[5] = Main_CLI2[5] | 0x08;
			break;
			case RD_SEL_8C:
				Main_CLI2[5] = Main_CLI2[5] | 0x10;
			break;
			
			
			case RD_SEL_TUNE_L:
				if ((Main_Disp_Mode!=MAIN_MODE_DAB2)&&(Main_Disp_Mode!=MAIN_MODE_DAB3))
				{
					Main_CLI2[3] = Main_CLI2[3] & 0x33;
					Main_CLI2[3] = Main_CLI2[3] | 0x04;
				}
			break;
			case RD_SEL_TUNE_R:
				if ((Main_Disp_Mode!=MAIN_MODE_DAB2)&&(Main_Disp_Mode!=MAIN_MODE_DAB3))
				{
					Main_CLI2[3] = Main_CLI2[3] & 0x33;
					Main_CLI2[3] = Main_CLI2[3] | 0x08;
				}
			break;
			case RD_SEL_AUTO:
				printf("[Touch_UP] RD_MAIN_DnBt:RD_SEL_AUTO= Touch %d %d\n",Main_Disp_Mode,g_bDAB2_DataMode);//chpark.11.15

				if (Main_Disp_Mode == MAIN_MODE_DAB1)
				{
				}
				else if ((Main_Disp_Mode == MAIN_MODE_DAB2)||(Main_Disp_Mode == MAIN_MODE_DAB3))
				{
				}
				else
				{
					Main_CLI2[3] = Main_CLI2[3] | 0x10;
				}
				printf("RD_MAIN_DnBt:RD_SEL_AUTO=%d \n",Main_Disp_Mode);
			break;
			case RD_SEL_MODE:
				if ((Main_Disp_Mode == MAIN_MODE_DAB2)||(Main_Disp_Mode == MAIN_MODE_DAB3))
				{
					if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
					{
						DAB2ListBox_Select(-1);
					}
					else
					{
						DAB2ListBoxH_Select(-1);
					}
				}
			break;
			case RD_SEL_TONE_D:
				Main_CLI2[3] = Main_CLI2[3] & 0x33;
				Main_CLI2[3] = Main_CLI2[3] | 0x40;
			break;
			case RD_SEL_TONE_U:
				Main_CLI2[3] = Main_CLI2[3] & 0x33;
				Main_CLI2[3] = Main_CLI2[3] | 0x80;
			break;
			case RD_SEL_VOL_D:
				Main_CLI2[4] = Main_CLI2[4] & 0xFC;
				Main_CLI2[4] = Main_CLI2[4] | 0x01;
			break;
			case RD_SEL_VOL_U:
				Main_CLI2[4] = Main_CLI2[4] & 0xFC;
				Main_CLI2[4] = Main_CLI2[4] | 0x02;
			break;
			case RD_SEL_PWR:
				if ((Main_Disp_Mode == MAIN_MODE_DAB2)||(Main_Disp_Mode == MAIN_MODE_DAB3))
				{
					if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
					{
						DAB2ListBox_Select(-1);
					}
					else
					{
						DAB2ListBoxH_Select(-1);
					}
				}
			break;
			case RD_SEL_LOUD:
			break;
			case RD_SEL_SCAN:
				Main_CLI2[6] = Main_CLI2[6] | 0x04;
			break;
		}
	}
	return true;
}
bool RD_MAIN_DnBt2(ITUWidget* widget, char* param)
{
	static uint8_t sMM_RD_SI = 0xFF;
	uint8_t tsMM_RD_SI = 0xFF;
	
	tsMM_RD_SI = (param[0]&0xF)*10+(param[1]&0xF);
	
//	printf("RD_MAIN_DnBt2=%d,%d,%d,%d,%d\n",tsMM_RD_SI,param[0]&0x0F,param[1]&0x0F,param[2]&0x1F,MainMode_RD_Select_Index);
	
	Media_RunTime_Cnt = gMainTimerCnt_10m;
//	if (sMM_RD_SI!=tsMM_RD_SI)
	{
		sMM_RD_SI=tsMM_RD_SI;
		InitJogIncDecData_AC();
	}

	if ((param[2]&0x1F)==17)	//long
	{
		if (RD_MAIN_DnBt_Long==(param[2]&0x1F))
		{
			RD_MAIN_DnBt_Long=(param[2]&0x1F);
			
			if ((Main_Disp_Mode == MAIN_MODE_DAB2)||(Main_Disp_Mode == MAIN_MODE_DAB3))
			{
				//DAB List?? ????

			}
			else
			{		
			}
		}
		else
		{
			RD_MAIN_DnBt_Long=(param[2]&0x1F);
		}
	}
	else
	{
		RD_MAIN_DnBt_Long=0;
		switch (sMM_RD_SI)
		{
			case RD_SEL_1C:
				Main_CLI2[4] = Main_CLI2[4] | 0x20;
			break;
			case RD_SEL_2C:
				Main_CLI2[4] = Main_CLI2[4] | 0x40;
			break;
			case RD_SEL_3C:
				Main_CLI2[4] = Main_CLI2[4] | 0x80;
			break;
			case RD_SEL_4C:
				Main_CLI2[5] = Main_CLI2[5] | 0x01;
			break;
			case RD_SEL_5C:
				Main_CLI2[5] = Main_CLI2[5] | 0x02;
			break;
			case RD_SEL_6C:
				Main_CLI2[5] = Main_CLI2[5] | 0x04;
			break;
			case RD_SEL_7C:
				Main_CLI2[5] = Main_CLI2[5] | 0x08;
			break;
			case RD_SEL_8C:
				Main_CLI2[5] = Main_CLI2[5] | 0x10;
			break;
			
			
			case RD_SEL_TUNE_L:
				if ((Main_Disp_Mode!=MAIN_MODE_DAB2)&&(Main_Disp_Mode!=MAIN_MODE_DAB3))
				{
					Main_CLI2[3] = Main_CLI2[3] & 0x33;
					Main_CLI2[3] = Main_CLI2[3] | 0x04;
				}
			break;
			case RD_SEL_TUNE_R:
				if ((Main_Disp_Mode!=MAIN_MODE_DAB2)&&(Main_Disp_Mode!=MAIN_MODE_DAB3))
				{
					Main_CLI2[3] = Main_CLI2[3] & 0x33;
					Main_CLI2[3] = Main_CLI2[3] | 0x08;
				}	
			break;
			case RD_SEL_AUTO:
				printf("RD_MAIN_DnBt2:RD_SEL_AUTO=%d \n",Main_Disp_Mode);
				if (Main_Disp_Mode == MAIN_MODE_DAB1)
				{

				}
				else if ((Main_Disp_Mode == MAIN_MODE_DAB2)||(Main_Disp_Mode == MAIN_MODE_DAB3))
				{
				}
				else
				{
					Main_CLI2[3] = Main_CLI2[3] | 0x10;
				}
				printf("RD_MAIN_DnBt2:RD_SEL_AUTO=%d \n",Main_Disp_Mode);
			break;
			case RD_SEL_MODE:
				if ((Main_Disp_Mode == MAIN_MODE_DAB2)||(Main_Disp_Mode == MAIN_MODE_DAB3))
				{
					if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
					{
						DAB2ListBox_Select(-1);
					}
					else
					{
						DAB2ListBoxH_Select(-1);
					}
				}			
			break;
			case RD_SEL_TONE_D:
				Main_CLI2[3] = Main_CLI2[3] & 0x33;
				Main_CLI2[3] = Main_CLI2[3] | 0x40;
			break;
			case RD_SEL_TONE_U:
				Main_CLI2[3] = Main_CLI2[3] & 0x33;
				Main_CLI2[3] = Main_CLI2[3] | 0x80;
			break;
			case RD_SEL_VOL_D:
				Main_CLI2[4] = Main_CLI2[4] & 0xFC;
				Main_CLI2[4] = Main_CLI2[4] | 0x01;
			break;
			case RD_SEL_VOL_U:
				Main_CLI2[4] = Main_CLI2[4] & 0xFC;
				Main_CLI2[4] = Main_CLI2[4] | 0x02;
			break;
			case RD_SEL_PWR:
				if ((Main_Disp_Mode == MAIN_MODE_DAB2)||(Main_Disp_Mode == MAIN_MODE_DAB3))
				{
					if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
					{
						DAB2ListBox_Select(-1);
					}
					else
					{
						DAB2ListBoxH_Select(-1);
					}
				}
			break;
			case RD_SEL_LOUD:
			break;
			case RD_SEL_SCAN:
				Main_CLI2[6] = Main_CLI2[6] | 0x04;
			break;
		}
	}
	return true;
}

bool RD_MAIN_UpBt(ITUWidget* widget, char* param)
{
	static uint8_t sMM_RD_SI = 0xFF;
	uint8_t tsMM_RD_SI = 0xFF;
	uint8_t tDAB_PrgCnt = 0;
	if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
		tDAB_PrgCnt = DAB_PRG_CNT_H;
	else
		tDAB_PrgCnt = DAB_PRG_CNT_V;
	tsMM_RD_SI = (param[0]&0xF)*10+(param[1]&0xF);
	
	printf("RD_MAIN_UpBt=%d,%d,%d,%d,%d\n",tsMM_RD_SI,param[0]&0x0F,param[1]&0x0F,param[2]&0x1F,MainMode_RD_Select_Index);
	
	if (RD_SEL_TUNE_LR_FLAG)
	{
		MainMode_RD_Sel(0,RD_SEL_TUNE_L,DISP_ON);
	}
	if(RD_SEL_TONE_DU_FLAG)
	{
		MainMode_RD_Sel(0,RD_SEL_TONE_D,DISP_ON);
	}
	if(RD_SEL_VOL_DU_FLAG)
	{
		MainMode_RD_Sel(0,RD_SEL_VOL_D,DISP_ON);
	}
	InitJogIncDecData_AC();
	InitJogIncDecData_RD();
#if 0
	if (Main_Disp_Mode == MAIN_MODE_DAB1)
	{
		Main_CLI2[3] = Main_CLI2[3] & 0x33;
	}
	else if (Main_Disp_Mode == MAIN_MODE_DAB2)
	{
		Main_CLI2[3] = Main_CLI2[3] & 0x3F;
	}
	else
	{
		Main_CLI2[3] = Main_CLI2[3] & 0x23;
	}

	Main_CLI2[4] = Main_CLI2[4] & 0x1C;
	Main_CLI2[5] = Main_CLI2[5] & 0xE0;	
	Main_CLI2[6] = Main_CLI2[6] & 0xFB;
#else
	Main_CLI2[3] = 0;
	Main_CLI2[4] = 0;
	Main_CLI2[5] = 0;
	Main_CLI2[6] = 0;
#endif	
	RD_MAIN_DnBt_Long=0;
//	if (sMM_RD_SI!=tsMM_RD_SI)
	{
		sMM_RD_SI=tsMM_RD_SI;
	}

		
	switch (sMM_RD_SI)
	{
		case RD_SEL_TUNE_L:
			if ((Main_Disp_Mode == MAIN_MODE_DAB2)||(Main_Disp_Mode == MAIN_MODE_DAB3))
			{
				MainMode_DAB2_PTY_Disp(1,DAB2_PTY_Index,DISP_ON);
			}
		break;
		case RD_SEL_TUNE_R:
			if ((Main_Disp_Mode == MAIN_MODE_DAB2)||(Main_Disp_Mode == MAIN_MODE_DAB3))
			{
				MainMode_DAB2_PTY_Disp(2,DAB2_PTY_Index,DISP_ON);
			}
		break;
		case RD_SEL_AUTO:
			if (Main_Disp_Mode == MAIN_MODE_DAB1)
			{
				printf("[Touch_UP]case RD_SEL_AUTO:==%d\n",g_bDAB2_DataMode);	//chpar.11.13
				//MainDAB_Init();//chpark.24.03.07
				g_bDAB2_DataMode=true;     //chpark.11.15		
				g_DAB2_PreSelect_Index=-1; //chpark.11.15
				g_DAB3_PreSelect_Index=-1; //chpark.12.05
				g_DAB2_PreSelect_POS  = 0; //chpark.24.03.04
				g_DAB2_H_PreSelect_POS= 0; //chpark.24.03.04
			
				Main_Disp_Mode = MAIN_MODE_DAB2;
				//MainMode_DAB2_Select_Index = RD_SEL_AUTO; //chpark.24.10.07
				//MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);   //chpark.11.15
				MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON); //chpark.24.11.11
			}
			else if (Main_Disp_Mode == MAIN_MODE_DAB2)//chpark.24.01.29
			{
				printf("[Touch_UP][case RD_SEL_AUTO][DAB2->Back button->DAB1]==%d\n",g_bDAB2_DataMode);
				Main_Disp_Mode = MAIN_MODE_DAB1;
				//MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);     //chpark.11.15
			}
			else if (Main_Disp_Mode == MAIN_MODE_DAB3)//chpark.24.01.29
			{
				g_bDAB2_DataMode=true;     
				printf("[Touch_UP][case RD_SEL_AUTO][DAB3->Back button->DAB2]==%d\n",g_bDAB2_DataMode);	

				Main_Disp_Mode = MAIN_MODE_DAB2;
				MainMode_RD_Sel(1,RD_SEL_OFF,DISP_ON);        
				//Make the jog signal to be passed to the scroll.
				MainMode_DAB2_Select_Index = RD_SEL_SCROLL;     
			}
			/*
			else if ((Main_Disp_Mode == MAIN_MODE_DAB2)||(Main_Disp_Mode == MAIN_MODE_DAB3))
			{
				Main_Disp_Mode = MAIN_MODE_DAB1;
			}
			*/
		break;
		case RD_SEL_MODE:
			Main_CLI2[3] = Main_CLI2[3] | 0x20;
		break;
		case RD_SEL_MUTE:
			Main_CLI2[4] = Main_CLI2[4] | 0x04;
		break;
		case RD_SEL_PWR:
			Main_CLI2[4] = Main_CLI2[4] | 0x08;
		break;
		case RD_SEL_LOUD:
			Main_CLI2[4] = Main_CLI2[4] | 0x10;
		break;
		case DAB2_SEL_1C:
			DAB2_PTY_Index = (DAB2_PTY_Index/tDAB_PrgCnt)*tDAB_PrgCnt+0;
			MainMode_DAB2_PTY_Disp(0,DAB2_PTY_Index,DISP_ON);
			Main_CLI3[0] = DAB2_PTY_Index+1;
		break;
		case DAB2_SEL_2C:
			DAB2_PTY_Index = (DAB2_PTY_Index/tDAB_PrgCnt)*tDAB_PrgCnt+1;
			MainMode_DAB2_PTY_Disp(0,DAB2_PTY_Index,DISP_ON);
			Main_CLI3[0] = DAB2_PTY_Index+1;
		break;
		case DAB2_SEL_3C:
			DAB2_PTY_Index = (DAB2_PTY_Index/tDAB_PrgCnt)*tDAB_PrgCnt+2;
			MainMode_DAB2_PTY_Disp(0,DAB2_PTY_Index,DISP_ON);
			Main_CLI3[0] = DAB2_PTY_Index+1;
		break;
		case DAB2_SEL_4C:
			DAB2_PTY_Index = (DAB2_PTY_Index/tDAB_PrgCnt)*tDAB_PrgCnt+3;
			MainMode_DAB2_PTY_Disp(0,DAB2_PTY_Index,DISP_ON);
			Main_CLI3[0] = DAB2_PTY_Index+1;
		break;
		case DAB2_SEL_5C:
			DAB2_PTY_Index = (DAB2_PTY_Index/tDAB_PrgCnt)*tDAB_PrgCnt+4;
			MainMode_DAB2_PTY_Disp(0,DAB2_PTY_Index,DISP_ON);
			Main_CLI3[0] = DAB2_PTY_Index+1;
		break;
		case DAB2_SEL_6C:
			DAB2_PTY_Index = (DAB2_PTY_Index/tDAB_PrgCnt)*tDAB_PrgCnt+5;
			MainMode_DAB2_PTY_Disp(0,DAB2_PTY_Index,DISP_ON);
			Main_CLI3[0] = DAB2_PTY_Index+1;
		break;
		case DAB2_SEL_7C:
			DAB2_PTY_Index = (DAB2_PTY_Index/tDAB_PrgCnt)*tDAB_PrgCnt+6;
			MainMode_DAB2_PTY_Disp(0,DAB2_PTY_Index,DISP_ON);
			Main_CLI3[0] = DAB2_PTY_Index+1;
		break;
		case DAB2_SEL_8C:
			DAB2_PTY_Index = (DAB2_PTY_Index/tDAB_PrgCnt)*tDAB_PrgCnt+7;
			MainMode_DAB2_PTY_Disp(0,DAB2_PTY_Index,DISP_ON);
			Main_CLI3[0] = DAB2_PTY_Index+1;
		break;
		case BT_SEL_R27:
			Main_CLI2[3] = Main_CLI2[3] | 0x04;
		break;
		case BT_SEL_R28:
			Main_CLI2[5] = Main_CLI2[5] | 0x20;
		break;
		case BT_SEL_R29:
			Main_CLI2[3] = Main_CLI2[3] | 0x08;
		break;
		case BT_SEL_R30:
			Main_CLI2[6] = Main_CLI2[6] | 0x10;
		break;
		case PH_SEL_R34:
			Main_CLI2[6] = Main_CLI2[6] | 0x02;
		break;
		case PH_SEL_R31:
			Main_CLI2[5] = Main_CLI2[5] | 0x40;
		break;
		case PH_SEL_R32:
			Main_CLI2[5] = Main_CLI2[5] | 0x80;
		break;
	}    
	return true;
}
bool AC_MAIN_DnBt(ITUWidget* widget, char* param)
{
	static uint8_t sMM_AC_SI = 0xFF;
	printf("\n===>AC_MAIN_DnBt=0x%02X 0x%02X   , %d\n",param[0],sMM_AC_SI,MainMode_AC_Select_Index);

	Media_RunTime_Cnt = gMainTimerCnt_10m;

	if (((AC_SEL_DIR_LR_FLAG==0)&&(AC_SEL_TEMP_LR_FLAG==0)&&(AC_SEL_LEVEL_LR_FLAG==0))||(sMM_AC_SI!=(param[0]&0x0F)))
	{
		if ((AC_SEL_DIR_LR_FLAG)||(AC_SEL_TEMP_LR_FLAG)||(AC_SEL_LEVEL_LR_FLAG))
			InitJogIncDecData_AC();
		sMM_AC_SI=(param[0]&0x0F);
		MainMode_AC_Sel(1,sMM_AC_SI,DISP_ON);
	}
	sMM_AC_SI=(param[0]&0x0F);
	switch (sMM_AC_SI)
	{
		case AC_SEL_TEMP_L:
			Main_CLI2[2] = Main_CLI2[2] & 0xC0;
			Main_CLI2[2] = Main_CLI2[2] |0x04;
		break;
		case AC_SEL_TEMP_R:
			Main_CLI2[2] = Main_CLI2[2] & 0xC0;
			Main_CLI2[2] = Main_CLI2[2] |0x08;
		break;
		case AC_SEL_LEVEL_L:
			Main_CLI2[2] = Main_CLI2[2] & 0xC0;
			Main_CLI2[2] = Main_CLI2[2] |0x10;
		break;
		case AC_SEL_LEVEL_R:
			Main_CLI2[2] = Main_CLI2[2] & 0xC0;
			Main_CLI2[2] = Main_CLI2[2] |0x20;
		break;
	}
	return true;
}
bool AC_MAIN_DnBt2(ITUWidget* widget, char* param)
{
	static uint8_t sMM_AC_SI = 0xFF;
//	printf("===>AC_MAIN_DnBt2=0x%02X %d %d %d\n\n",param[0],AC_SEL_LEVEL_LR_FLAG,AC_SEL_TEMP_LR_FLAG,MainMode_AC_Select_Index);

	Media_RunTime_Cnt = gMainTimerCnt_10m;
//	AC_SEL_DIR_LR_FLAG = 0;

	InitJogIncDecData_RD();

//	if (sMM_AC_SI!=(param[0]&0x0F))
	{
		sMM_AC_SI=(param[0]&0x0F);
//		MainMode_AC_Sel(0,sMM_AC_SI,DISP_ON);
	}
	switch (sMM_AC_SI)
	{
		case AC_SEL_TEMP_L:
			Main_CLI2[2] = Main_CLI2[2] & 0xC0;
			Main_CLI2[2] = Main_CLI2[2] |0x04;
//			AC_SEL_LEVEL_LR_FLAG = 0;
		break;
		case AC_SEL_TEMP_R:
			Main_CLI2[2] = Main_CLI2[2] & 0xC0;
			Main_CLI2[2] = Main_CLI2[2] |0x08;
//			AC_SEL_LEVEL_LR_FLAG = 0;
		break;
		case AC_SEL_LEVEL_L:
			Main_CLI2[2] = Main_CLI2[2] & 0xC0;
			Main_CLI2[2] = Main_CLI2[2] |0x10;
//			AC_SEL_TEMP_LR_FLAG = 0;
		break;
		case AC_SEL_LEVEL_R:
			Main_CLI2[2] = Main_CLI2[2] & 0xC0;
			Main_CLI2[2] = Main_CLI2[2] |0x20;
//			AC_SEL_TEMP_LR_FLAG = 0;
		break;
	}
	return true;
}

bool AC_MAIN_UpBt(ITUWidget* widget, char* param)
{
	static uint8_t sMM_AC_SI = 0xFF;
	printf("===>AC_MAIN_UpBt=0x%02X 0x%02X   , %d\n\n",param[0],param[1],MainMode_AC_Select_Index);
	

//	if (sMM_AC_SI!=(param[0]&0x0F))
	{
		sMM_AC_SI=(param[0]&0x0F);
//		MainMode_AC_Sel(0,sMM_AC_SI,DISP_ON);
	}
	Main_CLI2[2]=0;
	if (AC_SEL_DIR_LR_FLAG)
	{
		MainMode_AC_Sel(0,AC_SEL_DIR_R,DISP_ON);
	}
	if(AC_SEL_TEMP_LR_FLAG)
	{
		MainMode_AC_Sel(0,AC_SEL_TEMP_R,DISP_ON);
	}
	if(AC_SEL_LEVEL_LR_FLAG)
	{
		MainMode_AC_Sel(0,AC_SEL_LEVEL_R,DISP_ON);
	}	
	InitJogIncDecData_AC();
	InitJogIncDecData_RD();	
	switch (sMM_AC_SI)
	{
		case AC_SEL_DIR_L:
			Main_CLI2[2] = Main_CLI2[2] & 0xC0;
			Main_CLI2[2] = Main_CLI2[2] |0x01;
		break;
		case AC_SEL_DIR_R:
			Main_CLI2[2] = Main_CLI2[2] & 0xC0;
			Main_CLI2[2] = Main_CLI2[2] |0x02;
		break;
		case AC_SEL_AIRMODE:
			Main_CLI2[2] = Main_CLI2[2] |0x40;
		break;
		case AC_SEL_ONOFF:
			Main_CLI2[2] = Main_CLI2[2] |0x80;
		break;
		case AC_SEL_AUTO:
			Main_CLI2[3] = Main_CLI2[3] |0x01;
		break;
		case AC_SEL_PWR:
			Main_CLI2[3] = Main_CLI2[3] |0x02;
		break;
	}
    return true;
}
bool AC_MAIN_UpBt2(ITUWidget* widget, char* param)
{
	static uint8_t sMM_AC_SI = 0xFF;
//	printf("===>AC_MAIN_UpBt2=0x%02X 0x%02X   , %d\n\n",param[0],Main_CLI2[2],MainMode_AC_Select_Index);
	

//	if (sMM_AC_SI!=(param[0]&0x0F))
	{
		sMM_AC_SI=(param[0]&0x0F);
//		MainMode_AC_Sel(0,sMM_AC_SI,DISP_ON);
	}
//	Main_CLI2[2]=0;
	InitJogIncDecData_RD();	
	switch (sMM_AC_SI)
	{
		case AC_SEL_DIR_L:
			Main_CLI2[2] = Main_CLI2[2] & 0xC0;
			Main_CLI2[2] = Main_CLI2[2] |0x01;
		break;
		case AC_SEL_DIR_R:
			Main_CLI2[2] = Main_CLI2[2] & 0xC0;
			Main_CLI2[2] = Main_CLI2[2] |0x02;
		break;
		case AC_SEL_AIRMODE:
			Main_CLI2[2] = Main_CLI2[2] |0x40;
		break;
		case AC_SEL_ONOFF:
			Main_CLI2[2] = Main_CLI2[2] |0x80;
		break;
		case AC_SEL_AUTO:
			Main_CLI2[3] = Main_CLI2[3] |0x01;
		break;
		case AC_SEL_PWR:
			Main_CLI2[3] = Main_CLI2[3] |0x02;
		break;
	}
    return true;
}

bool Main_Button1(ITUWidget* widget, char* param)
{
	GotoCamera(1);
	return true;
}

bool Main_Button2(ITUWidget* widget, char* param)

{
	GotoCamera(2);
	return true;
} 

bool Main_Button3(ITUWidget* widget, char* param)
{
	GotoCamera(3);
	return true;

}

bool Main_Button4(ITUWidget* widget, char* param)
{
	GotoCamera(4);
	return true;
} 	
bool Main_Button5(ITUWidget* widget, char* param)
{
	GotoCamera(5);
    return true;
}


extern bool wZoomFlag;
extern bool fZoomFlag;
extern uint8_t CheckCameraModeDisp;

void ResetMain(void)
{
	int i;
	mainLayer=NULL;
	MainLayerFlag=0;
	MainLayerFlag4ZommIcon = 0;
	CheckCameraModeDisp = 0xF0;
}
void ReStartMain(void)
{
//	if(RotationMode!=0)
	{
		int ti;
		W_01_B_Stop(NULL,NULL);
		F_01_B_Stop(NULL,NULL);
		wZoomFlag = 0;
		fZoomFlag = 0;
		Main_EI_5BMeterDisp(Main_EI_5BValue,gMainTimerCnt_10m);
		Main_SI_67BMeterDisp(Main_SI_67BValue,gMainTimerCnt_10m);
		MainLayer_HourMeterDisp(DISP_OFF,HMV.HourMeter_Trip, HMV.HourMete_rHour[HMV.HourMeter_Trip],gMainTimerCnt_10m,DISP_ON);		
		for (ti=0;ti<WARNING_ICON_CNT;ti++)
		{
			if (MainWarnigData[ti] != 0)
			{
//				printf("MainWarnigData[%d]0:0x%02X \n",ti, MainWarnigData[ti]);
				MainWarnigData[ti] = MainWarnigData[ti]|0x80;
//				printf("MainWarnigData[%d]1:0x%02X \n",ti, MainWarnigData[ti]);
			}
		}
		
		for (ti=0;ti<FUNCTION_ICON_CNT;ti++)
		{
			if (MainFunctionData[ti] != 0)
			{
//				printf("MainFunctionData[%d]0:0x%02X \n",ti, MainFunctionData[ti]);
				MainFunctionData[ti] = MainFunctionData[ti]|0x80;
//				printf("MainFunctionData[%d]1:0x%02X \n",ti, MainFunctionData[ti]);
			}
		}

//		printf("\nReStartMain RotationMode:[%d]\n\n", RotationMode);
		for (ti=0;ti<WARNING_ICON_CNT;ti++)
		{
//			if (MainWarnigData[ti] != 0)
//			printf("MainWarnigData[%d]3:0x%02X \n",ti, MainWarnigData[ti]);
		}
		
		for (ti=0;ti<FUNCTION_ICON_CNT;ti++)
		{
//			if (MainFunctionData[ti] != 0)
//			printf("MainFunctionData[%d]3:0x%02X \n",ti, MainFunctionData[ti]);
		}
		
		
		
		Main_WarningDisp(gMainTimerCnt_10m,1);
		Main_FunctionDisp(gMainTimerCnt_10m,1);
		MainMode_RD_BarDisp( 1,0,0, DISP_ON);
	}	
}


#ifdef TEST_FLASH_MEM
	void Test_Flash_Save(void)
	{
		char JD_Buf[10] = {0,};		
		
		if (MainLayerFlag==0)
			return 0;		
		if ((gMainTimerCnt_10m%TIME_CNT_2SEC)==0)
		{
			static bool Cs_Cnt = 0;
			char tData[2]={0,};
			if (Cs_Cnt==0)
			{
				Cs_Cnt=1;
				if (AUX1CurrentOutMode==0)
					JD_Buf[0] = 2;
				else if (AUX1CurrentOutMode==1)
					JD_Buf[0] = 3;
				else if (AUX1CurrentOutMode==2)
					JD_Buf[0] = 1;
				Main_ATTBt_Func(NULL, JD_Buf);
				Main_ATTBt_Func(NULL, JD_Buf);
			}
			else
			{
				Cs_Cnt=0;
				JD_Buf[0] = 0;
				JD_Buf[1] = 0;
				Main_HMeterBt_Func(NULL, JD_Buf);
				Main_HMeterBt_Func(NULL, JD_Buf);
			}
			Flash_Save_Cnt_Disp();
		}
	}
#endif	//TEST_FLASH_MEM

