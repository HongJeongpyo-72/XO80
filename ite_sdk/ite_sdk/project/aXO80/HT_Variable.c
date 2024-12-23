#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "HT_DispDdefine.h"

uint8_t ModelType=0;			// 0:A14FW , 1:A12A�� , 2:A12BŸ�� , 3:A14H , 4:A25E , 5:L9EV , 6:C9-3Ÿ�� , 9:L5E , 11:L6F , 12:F10E
								// 0��A14FW , 1��A12A , 2��A12B , 3��A14H , 4��A25E 
								// 5��L9EV , 9��L5E , 11��L6F
								// 12��F10E
bool Main_CoolantOrange=0;
uint8_t gCreepValue = 5;
bool EEPROM_Lock=0;	// 0:off , 1:on


uint8_t Main_DriveMode=1;		// 0:����  , 1:Drive

uint8_t SubModelType=0;
uint8_t gUserPW[4];

uint8_t MainWarnigData[WARNING_ICON_CNT]={0,};
uint8_t MainFunctionData[FUNCTION_ICON_CNT]={0,};	// 0x80:Ȯ�� ���� , 40:Ȯ�� �Ϸ� , 20:Ȯ���� , 10:Ȯ�� �ʿ� 

uint8_t Main_WEngStopSw = 0;
uint8_t Main_WSafetyStart = 0;
uint8_t Phone0Byte = 0x00;
uint8_t PowerBtFlag = 0x00;

uint8_t Main_wZoomIndex=0xFF;
uint8_t Main_fZoomIndex=0xFF;

uint32_t Main_wZoomIndex_Time=0;		// JPHong 2023-10-12
uint32_t Main_wZoomIndex_Time_Disp=0;		// JPHong 2023-10-24
uint32_t Main_fZoomIndex_Time=0;		// JPHong 2023-10-12
uint32_t Main_fZoomIndex_Time_Disp=0;		// JPHong 2023-10-24


uint16_t Main_EI_5BValue=0;//0xFF;
uint32_t Main_EI_5BTime=0;
uint16_t Main_DD_1BValue=0;
uint16_t Main_SI_67BValue=0;//0xFFFF;
uint32_t Main_SI_67BTime=0;
uint16_t Main_EI_34BValue=0;

uint8_t Main_CLI2[8]={0,};
uint8_t Main_CLI3[8]={0,};

HourMeterValue HMV;
MeterRateValue MRV;
AirConValue ACV;
RadioValue RDV;

uint8_t AUI1_0B_3210b=0;
uint8_t UserPW_PassFlag=0;
uint32_t UserPW_Time_UA2=0;
uint32_t UserPW_Time_AUI1=0;
uint32_t Phone_Time=PHONE_TIME;

uint8_t  gBuzzerMode=0;	//10;	//0:OFF , 1:ON(100)OFF(1900) , 2:ON(200)OFF(800) , 3:ON(500)OFF(500) , 4:ON(300)OFF(300)  , 5:ON , 6:ON(100)OFF(9900)  , 7:ON(100)OFF(100) 3Time   , 10:ON(500)OFF
uint32_t gBuzzerTimerCnt_10m=0;
uint32_t gMainTimerCnt_10m=0;
uint32_t UpdateDateCnt=0;
uint32_t CLM1_Time=0;
uint32_t CLM2_Time=0;
uint32_t CLM3_Time=0;
uint32_t CLM4_Time=0;
uint32_t AG_I_Time=0;

uint32_t EI_Time=0;
uint32_t DD_Time=0;
uint32_t SI_Time=0;
uint32_t MI_Time=0;

uint32_t HM_Time=0;
uint32_t TM1_Time=0;
uint32_t TM2_Time=0;
uint32_t TM3_Time=0;
uint32_t AUX_IO2_Time=0;
uint32_t M9HM_Time=0;

uint32_t IN1_Time = 0;
uint32_t IN2_Time = 0;
uint32_t OUT1_Time = 0;
uint32_t OUT6_Time = 0;
uint32_t IN17_Time = 0;
uint32_t IN3_Time = 0;
uint32_t IN4_Time = 0;
uint32_t IN5_Time = 0;
uint32_t IN6_Time = 0;
uint32_t IN7_Time = 0;
uint32_t IN9_Time = 0;
uint32_t IN13_Time = 0;
uint32_t IN23_Time = 0;
uint32_t OUT2_Time = 0;
uint32_t OUT3_Time = 0;
uint32_t OUT4_Time = 0;
uint32_t OUT8_Time = 0;
uint32_t AUX1_Time = 0;
uint32_t AUX2_Time = 0;
uint32_t MTSPD_Time = 0;
uint32_t OUT5_Time = 0;


uint8_t	KeyPressCnt=0; 


DATAValue DTVal;


uint8_t gPW_NextMode=0;	// 	0:NG ,11:1st AUX  ,12:2nd /4th   ,13:LOCK ,14:PASSWORD
bool gAutoGreaseFlag=0;	// 0:off , 1:on


AUX1Value AUX1Val;
uint8_t SIDEAUX_Machine=0;		// 0~8
uint8_t AutoTankDispFlag=0;
uint8_t PressureDispFlag=0;

uint8_t AUX1CurrentOutMode = 0;				// 0~2
uint8_t DTM_DateTime_Mode = Type1_YYYYMMDD;	// 	Type1_YYYYMMDD:1 , Type2_MMDDYYYY:2 , Type3_DDMMYYYY:3
uint8_t BG_Color = BG_BLACK;	// BG_BLUE:1 , BG_BLACK:2
uint8_t LCD_BL_Day = 0;
uint8_t LCD_BL_Night = 0;
uint8_t SIUS_SEL_Flag = 0;	// 0:SI , 1:US
uint8_t Main_DayNight = 0;		// 0:Day , 1:Night
CamColor CC1;
CamColor CC2;




uint8_t Fn_Cursor = 0;
uint8_t Fn_ModeDepth = 0;

uint8_t gCameraModeDisp = 0;	// 0:off , 1:BS , 2:B , 3:S , 4: AB , 5:AR , 6:AL , 7:BR , 8:3c
 

uint8_t gCameraStatus0 = 0xFF;
uint8_t gCameraLock0 = 0;	// 0:off , 1:lock
uint8_t gCameraStatus1 = 0xFF;
uint8_t gCameraLock1 = 0;	// 0:off , 1:lock

uint8_t MenuCameraMode = 0;	// 0:off , 1:on

uint8_t gCreepMode = 0;	// 0:off , 1:on   7bit : OPI2  , 6bit : CLM2
uint8_t MeterLiftLdFlag = 0;	// 0:off , 1:on

ErrorValue	ErVal; 


uint8_t OPI_5Byte_45bit	= 0;	//Active power control 0off  
uint8_t tOPI2_0Byte_01bit = 0xFF;	//Pre 270 camera //JPHong
uint8_t OPI2_0Byte_45bit = 0;	//LED  chpark  
uint8_t OPI2_1Byte_45bit= 0;	//Two-Speed Travel with Automatic Shift  0 off 
uint8_t CLM2_7Byte_5bit	= 0;	//0:No change , 1: 1->0(off) ,  2: 0->1(on) 



#ifdef CAMERA_DEFAULT_SVM
	uint8_t OPI2_0Byte_01bit = 1;	//270 camera //chpark
	uint8_t CPI_0Byte = 1;
#else
	uint8_t OPI2_0Byte_01bit = 0;	//Rear Camera
	uint8_t CPI_0Byte = 0;
#endif

uint8_t OPI2_1Byte_23bit = 1;	//Rotation  JPHong 2024-10-14

uint8_t TempCPI_0Byte = 0;

uint8_t OPI3Byte23bit = 0;
uint8_t OPI1Byte67bit = 0;
uint8_t OPI2Byte45bit = 0;
uint8_t OPI0Byte67bit = 0;
uint8_t OPI3Byte01bit = 0;
uint8_t OPI3Byte45bit = 0;
uint8_t OPI2Byte67bit = 0;
uint8_t OPI1Byte45bit = 0;
uint8_t OPI0Byte45bit = 0;
uint8_t OPI1Byte01bit = 0;
uint8_t OPI2Byte23bit = 0;
uint8_t OPI0Byte01bit = 0;
uint8_t OPI0Byte23bit = 0;//chpark
uint8_t OPI4Byte01bit = 0;
uint8_t OPI4Byte45bit = 0;
uint8_t OPI2Byte01bit = 0;
uint8_t OPI4Byte67bit = 0;
uint8_t OPI6Byte01bit = 0;//chpark
uint8_t OPI6Byte23bit = 0;//chpark
uint8_t OPI6Byte45bit = 0;
uint8_t OPI7Byte23bit = 0;//chpark
uint8_t OPI7Byte67bit = 0;//chpark
uint8_t OPI5Byte23bit = 0;
uint8_t OPI21Byte45bit = 0;
uint8_t OPI24Byte01bit = 0;


uint8_t Lock_1st_Flag	= 0;	// 0:off , 1:on
uint8_t Lock_2nd_Flag = 0;		// 0:off , 1:on


bool gMainScreenMode=0;
uint8_t gRead_Gpio_Value=0;

bool Main_CanCamOn=0;

bool gOnFlag_1S = 0;
bool gOnFlag_500ms = 0;
bool gOnFlag_300ms = 0;

uint8_t PL_Flag_New_Value = 0;
uint8_t PL_Flag_Manual = 0;

MB_IO_Value gMB_IO_Value;   //chpark.

uint32_t DAB2_DataChkTime = 0; //chpark.09.25
uint32_t DAB3_DataChkTime = 0; //chpark.07.18

uint32_t g_UM5_ChkTime = 0;	//chpark.08.22

uint32_t g_UI1_ChkTime = 0;	//chpark.10.12
uint32_t g_UI2_ChkTime = 0;	//chpark.10.12
uint32_t g_UI3_ChkTime = 0;	//chpark.10.12
uint32_t g_UI4_ChkTime = 0;	//chpark.10.12
uint32_t g_UI5_ChkTime = 0;	//chpark.10.12

bool gUI_Server_Mode = 0; //chpark.10.12

bool g_bDAB2_DataMode  = true;//chpark.11.15
uint8_t g_TestMode_Req_Time = 0; //chpark.24.05.27

uint32_t g_IM_RecvTime = 0;	//chpark.24.08.29

int g_nPW100_LayerType;//chpark.24.09.09 //0:PW100_MODE_SET   1:PW100_MODE_INPUT
int g_nPW100_PreLayer; //chpark.24.09.09
int g_nPW100_NextLayer;//chpark.24.09.09


bool g_bPW100_PW_OnOff;       //0:PW100_PASS_MODE_OFF   1:PW100_PASS_MODE_ON

uint8_t PW100_PWD[5];  //chpark.24.09.09

#ifdef TEST_CAM_COLOR
	uint8_t g_nM7_StartType = M7_MODE_PUSH; //chpark.24.09.12 0:M7_MODE_PUSH  1:M7_MODE_KEY(Default)  2:M7_MODE_LOCK
#else
	uint8_t g_nM7_StartType = M7_MODE_PUSH; //chpark.24.09.12 0:M7_MODE_PUSH(UserPW)  1:M7_MODE_KEY(Default)  2:M7_MODE_LOCK
#endif

uint8_t g_TCI_0B = 0;
uint16_t g_TCI_1B = 0xFFFF;
uint32_t g_TCI_Time = 0;


M7_Lock_Data g_nM7_Lock_Data[3];

uint8_t g_PW100_Password_5Byte = 0; //chpark.24.11.11
uint8_t g_PW100_Password_6Byte = 0; //chpark.24.11.11

