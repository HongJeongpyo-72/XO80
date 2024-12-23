#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "HT_DispDdefine.h"
#include "layer_main_1.h"
#include "layer_main_FW.h"
#include "Menu_Layer.h"


static char widgetName[30];

bool MainLayerFlag = 0;
bool MainLayerFlag4ZommIcon = 0;
static char widgetName[30];
static uint8_t KeyDepth = 0;


/*
ITULayer
ITUIcon
ITUBackground
ITUStringSet
ITUText
ITUTextBox
ITUButton

ITUAnimation
ITUMeter
ITUContainer
*/

static ITUContainer* EI_5B;
static ITUContainer* DD_1B;
static ITUMeter* SI_67B;
static ITUMeter* EI_34B;

static ITUContainer* Function_BG;
static ITUContainer* Meter_BG;
static ITUContainer* Warning_BG;

static ITUIcon* MainWarnigIcon[WARNING_ICON_CNT][3]={0,};
static ITUIcon* MainFinctionIcon[FUNCTION_ICON_CNT][3]={0,};


static ITUButton* Bar_EngRPM;
static ITUButton* Bar_EngLd;
static ITUButton* Bar_LiftLd;
static ITUButton* Bar_PM;
static ITUButton* Bar_ASH;
static ITUIcon* Bar_Gauge[14];
static ITUIcon* DD_Gauge[10];
static ITUIcon* EI_Gauge[10];
static ITUIcon* ATT_B[3][10]={0,};
static ITUIcon* ATT_S[3][10]={0,};

 
ITUText* EngValue;

static ITUContainer* ATT_1_B;
static ITUContainer* ATT_2_B;
static ITUContainer* ATT_3_B;

static ITUIcon* EngStopSw_H;
static ITUIcon* SafetyStart_H;


static ITUButton* MainCameraBt;
static ITUButton* MainHomeBt;
static ITUButton* MainMenuBt;

static ITUButton* Main_ATT3_Bt;
static ITUButton* Main_ATT2_Bt;
static ITUButton* Main_ATT1_Bt;
static ITUButton* Main_BarBt;
static ITUButton* Main_HMeterBt;
static ITUButton* Main_TimeDateBt;
static ITUButton* Main_RadioBt;
static ITUButton* Main_ACBt;
static ITUButton* Main_ATTBt;
static ITUButton* Main_WarringBt;
static ITUButton* Main_SPK_Bt;
static ITUButton* Main_PwrBt;


static ITUIcon* Radio_Sel;
static ITUIcon* Bar_Sel;
static ITUIcon* ATT_Sel;
static ITUIcon* AC_WindSel;
static ITUIcon* HourMtSel;
static ITUIcon* Power_Sel;
static ITUIcon* TimeDataSel;
static ITUIcon* MainCameraSel;
static ITUIcon* Main_SPK_Sel;
static ITUIcon* Main_RadioSel;
static ITUIcon* Main_ACSel;
static ITUIcon* MainMenuSel;


static ITUText* TimeDataTx;
static ITUText* HourMtTx;
static ITUText* HourMt[4];
static ITUIcon* BarGaugeBar;
static ITUIcon* EI_5B_Graduation;
static ITUIcon* SI_67B_Graduation;

static ITUIcon* MainNoCamera;

static ITUBackground* Main_Black_BG;
static ITUBackground* Main_Blue_BG;

static ITUContainer* MainModeBG;

static ITUContainer* MainButton=NULL;

static ITUIcon* SPK_OFF;
static ITUIcon* SPK_ON;
static ITUIcon* SPK_Sel;
static ITUIcon* Power_ON;
static ITUIcon* Power_OFF;

static ITUText* MainTestMsg;
static ITUText* MainTestMsg2;
static ITUText* MainTestMsg3;

extern void HT_gettimeofday(uint16_t *year,uint8_t *mon,uint8_t *mday,uint8_t *hour,uint8_t *min,uint8_t *sec);		// add 230516 jhlee
extern bool MainMode_AC_Sel(uint8_t IncDec,uint8_t SelIndex,uint8_t OnOff);
extern bool MainMode_RD_Sel(uint8_t ReDisp,uint8_t SelIndex,uint8_t OnOff);
extern bool LCDLayer_BackLight(int8_t BL_Data);

static int32_t Main_TempRpm = 0x01;
static int8_t Main_tSymbol=0xFF;
static uint32_t Main_EI_34B = 0xFFFF;
static uint32_t Main_DD_1B = 0xFFFF;
static uint32_t Main_EI_5B = 0xFFFF;
static uint32_t Main_SI_67B = 0xFFFF;


bool LCD_BackLight(int8_t DayNightFlag)	// 0:Day , 1:Night
{ 
	if (DayNightFlag==0)
		LCDLayer_BackLight(LCD_BL_Day);
	else
		LCDLayer_BackLight(LCD_BL_Night);	
}




#define MeterERpm_25P		77
#define MeterERpm_50P		154
#define MeterERpm_75P		231
#define MeterERpm_100P		308
/*
25% : 77
50% : 154
75% : 231
100% : 308
*/

	static ITUText* TestMSG1;

#ifdef TEST_ERR_602
	uint32_t ERR_602_RX = 0;
	uint32_t ERR_602_RXe = 0;
	uint32_t ERR_602_TX = 0;
	uint32_t ERR_602_TXe = 0;
	void TestErr602(void)
	{
		char tmp[100]={0,};
		sprintf(tmp, "R:%d:%d , T:%d:%d",ERR_602_RX,ERR_602_RXe,ERR_602_TX,ERR_602_TXe);
//		sprintf(tmp, "FC Cnt:%d",ERR_602_TX);
//		sprintf(tmp, "%d,%d",ERR_602_RX,ERR_602_TX);
		ituTextSetString(TestMSG1, tmp);
	}
#endif



//==========================================
void MC100_InitControl();
void Goto_MC100_Mode(int nStep);
void MC100_PregressUpdate(int nVal);
bool MC100_D6_MouseUp(ITUWidget* widget, char* param);

static ITUWidget* MC100_Cont; 
static ITUWidget* MC100_Text[2];//0:status 1:percent
	#define MC100_TEXT_STATUS		0
	#define MC100_TEXT_PERCENT		1

static ITUWidget* MC100_Pregress; 

static ITUWidget* MC100_Result_Cont[3]; //0:cont 1:success 2:fail
	#define MC100_RESULT_CONT		0
	#define MC100_RESULT_OK			1
	#define MC100_RESULT_FAIL		2

static ITUWidget* MC100_Btn_Cont[3]; //0:cont 1:button 2:select
	#define MC100_BTN_CONT		0
	#define MC100_BTN			1
	#define MC100_BTN_SEL		2

static ITUWidget* MC100_D6_BTN;
static ITUWidget* MC100_ICON_D6_SEL;	
static ITUWidget* MC100_MSG;	// JPHong 2024-09-19

void MC100_InitControl()
{
	SetWidget(MC100_Cont,	"MicomUpdate");
	
	SetWidget(MC100_Text[MC100_TEXT_STATUS],		"MC100_Text");
	SetWidget(MC100_Text[MC100_TEXT_PERCENT],		"MC100_Per");

	SetWidget(MC100_Pregress,		"MC100_Progress");
	
	SetWidget(MC100_Result_Cont[MC100_RESULT_CONT],		"MC100_CONT_RESULT");
	SetWidget(MC100_Result_Cont[MC100_RESULT_OK],		"MC100_OK");
	SetWidget(MC100_Result_Cont[MC100_RESULT_FAIL],		"MC100_NO");

	SetWidget(MC100_D6_BTN,				"MC100_D6_BTN");
	SetWidget(MC100_MSG,				"MC100_MSG");	// JPHong 2024-09-19
	SetWidget(MC100_ICON_D6_SEL,		"MC100_ICON_D6_SEL");
	/*
	//for test
	Goto_MC100_Mode(0);
	Goto_MC100_Mode(10);
	Goto_MC100_Mode(11);
	Goto_MC100_Mode(12);
	
	Goto_MC100_Mode(20);
	Goto_MC100_Mode(21);
	Goto_MC100_Mode(22);

	Goto_MC100_Mode(30);
	Goto_MC100_Mode(31);
	Goto_MC100_Mode(32);

	Goto_MC100_Mode(100);
	Goto_MC100_Mode(101);
	Goto_MC100_Mode(200);
	*/

	ituWidgetSetVisible(MC100_Cont, DISP_OFF);
}

bool MC100_D6_MouseUp(ITUWidget* widget, char* param)
{
	Goto_MC100_Mode(200);
	return true;
}

void MC100_PregressUpdate(int nVal)
{
	if(nVal<=0)nVal=0;
	if(nVal>=100)nVal=100;

	ituProgressBarSetValue(MC100_Pregress, nVal);
	 
	char chPer[5];
	if(nVal==100)
	{
		sprintf(chPer, "%02d/100%%", nVal); 
	}
	else
	{
		sprintf(chPer, "%02d/100%%", nVal); 
	}
	 
	ituTextSetString(MC100_Text[MC100_TEXT_PERCENT], chPer);
}

void Goto_MC100_Mode(int nStep)
{
	char chMicomVersion[50];

	switch(nStep)
	{
		case 0: //Micom update start
			printf("Micom Ver Before: [%s]\n\n",gMicomFirmVer);
			
			ituWidgetSetVisible(MC100_Cont, DISP_ON);

			for(int i=0;i<2;i++)ituWidgetSetVisible(MC100_Text[i], DISP_ON);

			ituWidgetSetVisible(MC100_Pregress, DISP_ON);

			ituWidgetSetVisible(MC100_Result_Cont[MC100_RESULT_CONT], 	DISP_OFF);
			
			ituWidgetSetVisible(MC100_D6_BTN, 			DISP_OFF);
			ituWidgetSetVisible(MC100_MSG, 				DISP_OFF);		// JPHong 2024-09-19
			ituWidgetSetVisible(MC100_ICON_D6_SEL, 		DISP_OFF);

			ituTextSetString(MC100_Text[MC100_TEXT_STATUS], "Micom Update. Start");
			MC100_PregressUpdate(0);//reset

			break;
		
		case 10: //ready. start
			ituTextSetString(MC100_Text[MC100_TEXT_STATUS], "Micom Ready. Start");
			break;
		
			case 11: //ready. end suc
			ituTextSetString(MC100_Text[MC100_TEXT_STATUS], "Micom Ready. OK");
				break;
			
			case 12: //ready. end fail
			ituTextSetString(MC100_Text[MC100_TEXT_STATUS], "Micom Ready. Fail");
				break;

		case 20: //Send. start
			ituTextSetString(MC100_Text[MC100_TEXT_STATUS], "Micom Send. Start");
			break;
		
			case 21: //Send. end suc
			ituTextSetString(MC100_Text[MC100_TEXT_STATUS], "Micom Send. OK");
				break;
			
			case 22: //Send. end fail
			ituTextSetString(MC100_Text[MC100_TEXT_STATUS], "Micom Send. Fail");
				break;

		case 30: //Write. start

			ituTextSetString(MC100_Text[MC100_TEXT_STATUS], "Micom Write. Start");
			break;
			
			case 31: //Write. end suc
			ituTextSetString(MC100_Text[MC100_TEXT_STATUS], "Micom Write. OK");
				break;
		
			case 32: //Write. end fail
			ituTextSetString(MC100_Text[MC100_TEXT_STATUS], "Micom Write. Fail");
			break;

		case 101: //Micom update end. success
			DeleteMicomFirm();	// JPHong 2024-09-19
			ituWidgetSetVisible(MC100_Cont, DISP_ON);

			for(int i=0;i<2;i++)ituWidgetSetVisible(MC100_Text[i], DISP_ON);

			ituWidgetSetVisible(MC100_Pregress, DISP_ON);

			ituWidgetSetVisible(MC100_Result_Cont[MC100_RESULT_CONT], 	DISP_ON);
				ituWidgetSetVisible(MC100_Result_Cont[MC100_RESULT_OK], 	DISP_ON);
				ituWidgetSetVisible(MC100_Result_Cont[MC100_RESULT_FAIL], 	DISP_OFF);
		
			ituWidgetSetVisible(MC100_D6_BTN, 			DISP_ON);
			ituWidgetSetVisible(MC100_MSG, 				DISP_ON);		// JPHong 2024-09-19
			ituWidgetSetVisible(MC100_ICON_D6_SEL, 		DISP_ON);

			memset(chMicomVersion,0,sizeof(chMicomVersion));
			sprintf(chMicomVersion, "Micom Updated. OK.   Version:%s", gMicomFirmVer); 
			ituTextSetString(MC100_Text[MC100_TEXT_STATUS], chMicomVersion);

			ituTextSetString(MC100_Text[MC100_TEXT_PERCENT], "100");

			MC100_PregressUpdate(100);

			MicomFirm_UP_Flag = 0;

			break;
		case 102: //Micom update end. fail
			DeleteMicomFirm();	// JPHong 2024-09-19
			ituWidgetSetVisible(MC100_Cont, DISP_ON);

			for(int i=0;i<2;i++)ituWidgetSetVisible(MC100_Text[i], DISP_ON);

			ituWidgetSetVisible(MC100_Pregress, DISP_ON);

			ituWidgetSetVisible(MC100_Result_Cont[MC100_RESULT_CONT], 	DISP_ON);
				ituWidgetSetVisible(MC100_Result_Cont[MC100_RESULT_OK], 	DISP_OFF);
				ituWidgetSetVisible(MC100_Result_Cont[MC100_RESULT_FAIL], 	DISP_ON);
			
			ituWidgetSetVisible(MC100_D6_BTN, 			DISP_ON);
			ituWidgetSetVisible(MC100_ICON_D6_SEL, 		DISP_ON);

			memset(chMicomVersion,0,sizeof(chMicomVersion));
			sprintf(chMicomVersion, "Micom Updated. OK.   Version:%s", gMicomFirmVer);   
			ituTextSetString(MC100_Text[MC100_TEXT_STATUS], chMicomVersion);

			ituTextSetString(MC100_Text[MC100_TEXT_PERCENT], "100");

			MC100_PregressUpdate(100);

			MicomFirm_UP_Flag = 0;

			break;

		case 200: //go to main
			MicomFirm_UP_Flag = 0;
			MC100_PregressUpdate(0);
			
			ituWidgetSetVisible(MC100_Cont, DISP_OFF);
			GotoMain(MAIN_MODE_NORMAL);
			break;

		default:
			break;
	}
}



//===========================
//chpark.24.04.24
bool T400_D5_MouseUp(ITUWidget* widget, char* param);//chpark.24.07.02
bool T400_D7_MouseUp(ITUWidget* widget, char* param);//chpark.24.07.02
bool T_BTN_Press(ITUWidget* widget, char* param);
bool BuzzerButton(ITUWidget* widget, char* param);

static ITUWidget* TestMode_CONT; //

static ITUWidget* T_STEP_Cont[6]; //
#define T_STEP_LCD		0
#define T_STEP_CAM      1 //chpark.24.06.03
#define T_STEP_INFO		2 //chpark.24.06.03
#define T_STEP_ROTATE   3 //chpark.24.06.03
#define T_STEP_END		4 //chpark.24.06.03

static ITUWidget* T400_STEP_LCD_Cont[6]; //
#define T_STEP_LCD_RED		0
#define T_STEP_LCD_YELLOW	1
#define T_STEP_LCD_BLUE		2
#define T_STEP_LCD_BLACK	3
#define T_STEP_LCD_WHITE	4
#define T_STEP_LCD_END		5

static ITUWidget* T400_STEP_INFO_Cont[10]; //
#define T_STEP_INFO_VERSION		0
#define T_STEP_INFO_RTC			1
#define T_STEP_INFO_BUZZER		2
#define T_STEP_INFO_VERSION_SEL	3
#define T_STEP_INFO_RTC_SEL		4
#define T_STEP_INFO_BUZZER_SEL	5
#define T_STEP_INFO_CAM	6			//chpark.24.06.03
#define T_STEP_INFO_CAM_SEL	7		//chpark.24.06.03
#define T_STEP_INFO_ROTATE	8 		//chpark.24.06.03
#define T_STEP_INFO_ROTATE_SEL	9	//chpark.24.06.03


static ITUWidget* T400_Next_Btn_Cont; //
	static ITUWidget* T400_Pass_Cont[4]; //0:container 1:icon 2:Button
	static ITUWidget* T400_Fail_Cont[4]; //0:container 1:icon 2:Button
static ITUWidget* T400_Next_Btn_Blink[2]; //

#define T_STEP_INFO_CONT	0
#define T_STEP_INFO_ICON	1
#define T_STEP_INFO_BTN		2

#define T_NEXT_ICON_PUSH	0
#define T_NEXT_ICON_SELECT	1

#define T_PKT_UID			0 //chpark.24.05.27
#define T_PKT_MODEL			1 //chpark.24.05.27
#define T_PKT_VERSION		2 //chpark.24.05.27
#define T_PKT_TEST_STEP		3 //chpark.24.05.27

#define T_MODEL_430T			0 //chpark.24.05.27
#define T_MODEL_570T			1 //chpark.24.05.27
#define T_MODEL_800T			2 //chpark.24.05.27


static bool T400_bShowIcon = true;	

void TestMode_InitControl();
void TestMode_MakeSerial();

void GotoFacMode(int nStep);
void SetNextBtnPosition(int nStep);

void TestMode_ShowTime(uint16_t Year,uint8_t Mon,uint8_t Day,uint8_t Hour,uint8_t Min,uint8_t Sec);
bool TestMode_Send_TX(uint8_t nPacketMode, uint8_t nPacketType, uint8_t nStepVal, uint8_t nResult);



const char TestMode_charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
const int  TestMode_charset_size = sizeof(TestMode_charset) - 1;

void TestMode_InitControl()
{
	SetWidget(TestMode_CONT,	"TestMode");
	
	SetWidget(T_STEP_Cont[T_STEP_LCD],	"T400_LCD_Cont");
	SetWidget(T400_STEP_LCD_Cont[T_STEP_LCD_RED],		"T400_Red");
	SetWidget(T400_STEP_LCD_Cont[T_STEP_LCD_YELLOW],	"T400_Yellow");
	SetWidget(T400_STEP_LCD_Cont[T_STEP_LCD_BLUE],		"T400_Blue");
	SetWidget(T400_STEP_LCD_Cont[T_STEP_LCD_BLACK],		"T400_Black");
	SetWidget(T400_STEP_LCD_Cont[T_STEP_LCD_WHITE],		"T400_White");
	
	SetWidget(T_STEP_Cont[T_STEP_INFO],	"T400_INFO_Cont");
	SetWidget(T400_STEP_INFO_Cont[T_STEP_INFO_VERSION],		"T400_Version");
	ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_VERSION], DISP_ON);
	char tmp_charVersion[10];
	memset(tmp_charVersion,0,sizeof(tmp_charVersion));
	sprintf(tmp_charVersion," V%d.%d%d%c%c ", CFG_VERSION_MAJOR,CFG_VERSION_MINOR,CFG_VERSION_PATCH,gMicomFirmVer[5],gMicomFirmVer[6]);		// JPHong 2024-05-20
	//sprintf(tmp_charVersion," V%d.%d%d ", CFG_VERSION_MAJOR,CFG_VERSION_MINOR,CFG_VERSION_PATCH);
	ituTextSetString(T400_STEP_INFO_Cont[T_STEP_INFO_VERSION], tmp_charVersion);
	
	
	SetWidget(T400_STEP_INFO_Cont[T_STEP_INFO_RTC],		"T400_Time");
	ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_RTC], DISP_ON);
	
	SetWidget(T400_STEP_INFO_Cont[T_STEP_INFO_BUZZER],		"T400_Btn_Buzzer");
	ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_BUZZER], DISP_ON);
	
	SetWidget(T400_STEP_INFO_Cont[T_STEP_INFO_VERSION_SEL],		"T400_Sel_Ver");
	SetWidget(T400_STEP_INFO_Cont[T_STEP_INFO_RTC_SEL],			"T400_Sel_Time");
	SetWidget(T400_STEP_INFO_Cont[T_STEP_INFO_BUZZER_SEL],		"T400_Sel_Buzzer");
	SetWidget(T400_STEP_INFO_Cont[T_STEP_INFO_CAM_SEL],			"T400_Sel_Cam");
	SetWidget(T400_STEP_INFO_Cont[T_STEP_INFO_ROTATE_SEL],		"T400_Sel_Rotate");
	
	
	SetWidget(T400_Next_Btn_Cont,	"T400_Next_Btn_Cont");
	SetWidget(T400_Pass_Cont[T_STEP_INFO_CONT],		"T400_Pass_Cont");
	SetWidget(T400_Pass_Cont[T_STEP_INFO_ICON],	"T400_Pass_Icon");
	SetWidget(T400_Pass_Cont[T_STEP_INFO_BTN],	"T400_Pass_Btn");
	
	SetWidget(T400_Fail_Cont[T_STEP_INFO_CONT],		"T400_Fail_Cont");
	SetWidget(T400_Fail_Cont[T_STEP_INFO_ICON],	"T400_Fail_Icon");
	SetWidget(T400_Fail_Cont[T_STEP_INFO_BTN],	"T400_Fail_Btn");
	
	SetWidget(T400_Next_Btn_Blink[T_NEXT_ICON_PUSH],		"T400_Push_Icon");
	SetWidget(T400_Next_Btn_Blink[T_NEXT_ICON_SELECT],		"T400_Select_Icon");

	ituWidgetSetVisible(T400_Next_Btn_Blink[T_NEXT_ICON_PUSH], DISP_ON);
	ituWidgetSetVisible(T400_Next_Btn_Blink[T_NEXT_ICON_SELECT], DISP_ON);

	ituWidgetSetVisible(T400_Next_Btn_Cont, DISP_ON);
		
	for(int i=0;i<=T_STEP_INFO_BTN;i++)
	{
		ituWidgetSetVisible(T400_Pass_Cont[i], DISP_ON);
		ituWidgetSetVisible(T400_Fail_Cont[i], DISP_ON);
	}

	ituWidgetSetVisible(TestMode_CONT, DISP_OFF);

	TestMode_MakeSerial();//chpark.24.05.31
}

// 8ë°”ì´???œìˆ˜ ?œë¦¬?¼ì„ ?ì„±?˜ëŠ” ?¨ìˆ˜
void TestMode_generate_random_serial(char *serial, size_t length) {
    for (size_t i = 0; i < length; i++) {
        int key = rand() % TestMode_charset_size;
        serial[i] = TestMode_charset[key];
    }
    serial[length] = '\0'; // null-terminate the string
}

void TestMode_MakeSerial()
{
	int nSizeOfName = strlen(Device_UID);
	if(nSizeOfName>4 &&  (Device_UID[0] == ('0' +  T_MODEL_800T)) )
	{
		printf("[TestMode_MakeSerial] UID exist:%s\n",Device_UID);
		return;
	}

    srand(time(NULL));
    char serial_1[12]; //10 characters + 1 for null terminator
	TestMode_generate_random_serial(serial_1, 9);

 	sprintf(Device_UID, "%d%s",T_MODEL_800T,serial_1);

	XO_Save_Config(M7_UserIndex,true,0);
  
    printf("Device_UID: %s\n", Device_UID);
}

bool BuzzerButton(ITUWidget* widget, char* param)
{
	uint8_t nTouch = 0;
	nTouch = (uint8_t)atoi(param);

	if(nTouch==0)
	{
		printf("[BuzzerButton]nTouch=%d\n",nTouch);
		MI_Time =  gMainTimerCnt_10m; //buzzer time start //chpark.24.06.03
	}
	else
	{
		printf("[BuzzerButton]nTouch=%d\n",nTouch);
	}

	return true;
}

bool T400_D5_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t nTouch = 0;
	nTouch = (uint8_t)atoi(param);
	printf("[T400_D5_Button]nTouch=%d\n",nTouch);

	// Right Touch
		switch(g_nTest_Step)
		{
			case T_STEP_LCD_RED:
				g_nTest_Step = T_STEP_LCD_RED;
				break;

			case T_STEP_LCD_YELLOW:
				g_nTest_Step = T_STEP_LCD_RED;	
				break;
			
			case T_STEP_LCD_BLUE:
				g_nTest_Step = T_STEP_LCD_YELLOW;
				break;

			case T_STEP_LCD_BLACK:
				g_nTest_Step = T_STEP_LCD_BLUE;
				break;

			case T_STEP_LCD_WHITE:
				g_nTest_Step = T_STEP_LCD_BLACK; //(T_STEP_CAM*10);
				break;

			case (T_STEP_CAM*10)://camera 
				g_nTest_Step = T_STEP_LCD_RED; //LCD_RED
				break;

			case (T_STEP_INFO*10+T_STEP_INFO_CAM)://Info-cam btn 
				g_nTest_Step = (T_STEP_CAM*10);//camera 
				break;
			
			case (T_STEP_INFO*10)://version
				g_nTest_Step = (T_STEP_INFO*10+T_STEP_INFO_CAM);//Info-cam btn 
				break;

			case (T_STEP_INFO*10+1)://RTC
				g_nTest_Step = (T_STEP_INFO*10);//version
				break;

			case (T_STEP_INFO*10+2)://Buzzer
				g_nTest_Step = (T_STEP_INFO*10+1);//RTC
				break;

			case (T_STEP_ROTATE*10)://Rotate
				g_nTest_Step = (T_STEP_INFO*10+2);//Buzzer
				break;
			
			default:
				g_nTest_Step = T_STEP_END;
				break;	
		}	

	GotoFacMode(g_nTest_Step);

	return true;
}

bool T400_D7_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t nTouch = 0;
	nTouch = (uint8_t)atoi(param);
	printf("[T400_D7_Button]nTouch=%d\n",nTouch);

	return true;
}

bool T_BTN_Press(ITUWidget* widget, char* param)
{
	uint8_t nCur_StepResult = 0;
	uint8_t nCur_StepVal = 0;
	
	nCur_StepResult = (uint8_t)atoi(param);

		// Right Touch
		switch(g_nTest_Step)
		{
			case T_STEP_LCD_RED:
			    nCur_StepVal = T_STEP_LCD_RED;
				g_nTest_Step = T_STEP_LCD_YELLOW;
				break;

			case T_STEP_LCD_YELLOW:
				nCur_StepVal = T_STEP_LCD_YELLOW;
				g_nTest_Step = T_STEP_LCD_BLUE;	
				break;
			
			case T_STEP_LCD_BLUE:
				nCur_StepVal = T_STEP_LCD_BLUE;
				g_nTest_Step = T_STEP_LCD_BLACK;
				break;

			case T_STEP_LCD_BLACK:
				nCur_StepVal = T_STEP_LCD_BLACK;
				g_nTest_Step = T_STEP_LCD_WHITE;
				break;

			case T_STEP_LCD_WHITE:
				nCur_StepVal = T_STEP_LCD_WHITE;
				g_nTest_Step = (T_STEP_CAM*10);
				break;

			case (T_STEP_CAM*10)://camera
				nCur_StepVal = T_STEP_CAM*10;
				g_nTest_Step = (nCur_StepVal*10);//Version
				break;

			case (T_STEP_INFO*10+T_STEP_INFO_CAM)://Info-cam btn
				nCur_StepVal = T_STEP_INFO*10+T_STEP_INFO_CAM; //cam-info
				g_nTest_Step = (T_STEP_INFO*10);//version
				break;
			
			case (T_STEP_INFO*10)://version
				nCur_StepVal = T_STEP_INFO*10;
				g_nTest_Step = (T_STEP_INFO*10+1);//RTC
				break;

			case (T_STEP_INFO*10+1)://RTC
				nCur_StepVal = T_STEP_INFO*10+1;
				g_nTest_Step = (T_STEP_INFO*10+2);//Buzzer
				break;

			case (T_STEP_INFO*10+2)://Buzzer
				nCur_StepVal = T_STEP_INFO*10+2;
				g_nTest_Step = (T_STEP_ROTATE*10);//Rotate
				break;

			case (T_STEP_ROTATE*10)://Rotate
				nCur_StepVal = T_STEP_ROTATE*10;
				g_nTest_Step = (T_STEP_END*10);//END
				break;
			
			default:
				nCur_StepVal = T_STEP_END*10;
				g_nTest_Step = T_STEP_END;
				break;	
		}	
		printf("[T_BTN_Press] nCur_StepVal:%d  nCur_StepResult:%d\n",nCur_StepVal,nCur_StepResult);

		TestMode_Send_TX(1,T_PKT_TEST_STEP,nCur_StepVal,nCur_StepResult);//chpark.24.05.27

		GotoFacMode(g_nTest_Step);

	return true;
}

void SetNextBtnPosition(int nStep)
{
	
	bool bVertical = true;
	if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
	{
		bVertical = false;
	}

	int x,y;

	int Btn_width =ituWidgetGetWidth(T400_Next_Btn_Cont);
	int Btn_height=ituWidgetGetHeight(T400_Next_Btn_Cont);
	
	switch(nStep)
	{
		case T_STEP_LCD_RED:
			if(bVertical)
				ituWidgetSetPosition(T400_Next_Btn_Cont, 800-Btn_width,(1280-Btn_height)/2);	
			else
				ituWidgetSetPosition(T400_Next_Btn_Cont, 1280-Btn_width,(800-Btn_height)/2);	
			break;

		case T_STEP_LCD_YELLOW:
			if(bVertical)
				ituWidgetSetPosition(T400_Next_Btn_Cont, (800-Btn_width)/2,50);	
			else
				ituWidgetSetPosition(T400_Next_Btn_Cont, (1280-Btn_width)/2,50);		
			
			break;
		
		case T_STEP_LCD_BLUE:
			if(bVertical)
				ituWidgetSetPosition(T400_Next_Btn_Cont, 0,(1280-Btn_height)/2);
			else
				ituWidgetSetPosition(T400_Next_Btn_Cont, 0,(800-Btn_height)/2);
			break;

		case T_STEP_LCD_BLACK:
			if(bVertical)
				ituWidgetSetPosition(T400_Next_Btn_Cont, (800-Btn_width)/2,(1280-Btn_height-50) );
			else
				ituWidgetSetPosition(T400_Next_Btn_Cont, (1280-Btn_width)/2,(800-Btn_height-50));
			break;

		case T_STEP_LCD_WHITE:
			if(bVertical)
				ituWidgetSetPosition(T400_Next_Btn_Cont, 800-Btn_width,(1280-Btn_height)/2);	
			else
				ituWidgetSetPosition(T400_Next_Btn_Cont, 1280-Btn_width,(800-Btn_height)/2);
			break;

		case (T_STEP_INFO*10+T_STEP_INFO_CAM): //info-cam btn
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_VERSION_SEL], DISP_OFF);
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_RTC_SEL], DISP_OFF);
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_BUZZER_SEL], DISP_OFF);
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_ROTATE_SEL], DISP_OFF);//chpark.24.06.03
			
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_CAM_SEL], DISP_ON);//chpark.24.06.03
			ituWidgetGetGlobalPosition(T400_STEP_INFO_Cont[T_STEP_INFO_CAM_SEL], &x, &y);
			
			if(bVertical)
				ituWidgetSetPosition(T400_Next_Btn_Cont, x-Btn_width,y);	
			else
				ituWidgetSetPosition(T400_Next_Btn_Cont, x-Btn_width,y);	
			break;


		case (T_STEP_INFO*10)://info-version btn
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_RTC_SEL], DISP_OFF);
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_BUZZER_SEL], DISP_OFF);
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_CAM_SEL], DISP_OFF);//chpark.24.06.03
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_ROTATE_SEL], DISP_OFF);//chpark.24.06.03

			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_VERSION_SEL], DISP_ON);
			ituWidgetGetGlobalPosition(T400_STEP_INFO_Cont[T_STEP_INFO_VERSION_SEL], &x, &y);

			if(bVertical)
				ituWidgetSetPosition(T400_Next_Btn_Cont, x-Btn_width,y);	
			else
				ituWidgetSetPosition(T400_Next_Btn_Cont, x-Btn_width,y);	
			break;

		case (T_STEP_INFO*10+1):
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_VERSION_SEL], DISP_OFF);
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_BUZZER_SEL], DISP_OFF);
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_CAM_SEL], DISP_OFF);//chpark.24.06.03
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_ROTATE_SEL], DISP_OFF);//chpark.24.06.03

			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_RTC_SEL], DISP_ON);
			ituWidgetGetGlobalPosition(T400_STEP_INFO_Cont[T_STEP_INFO_RTC_SEL], &x, &y);
			
			if(bVertical)
				ituWidgetSetPosition(T400_Next_Btn_Cont, x-Btn_width,y);	
			else
				ituWidgetSetPosition(T400_Next_Btn_Cont, x-Btn_width,y);	
			break;

		case (T_STEP_INFO*10+2):
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_VERSION_SEL], DISP_OFF);
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_RTC_SEL], DISP_OFF);
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_CAM_SEL], DISP_OFF);//chpark.24.06.03
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_ROTATE_SEL], DISP_OFF);//chpark.24.06.03
			
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_BUZZER_SEL], DISP_ON);
			ituWidgetGetGlobalPosition(T400_STEP_INFO_Cont[T_STEP_INFO_BUZZER_SEL], &x, &y);
			
			if(bVertical)
				ituWidgetSetPosition(T400_Next_Btn_Cont, x-Btn_width,y);	
			else
				ituWidgetSetPosition(T400_Next_Btn_Cont, x-Btn_width,y);	
			break;
		
	
		case (T_STEP_ROTATE*10):
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_VERSION_SEL], DISP_OFF);
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_RTC_SEL], DISP_OFF);
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_BUZZER_SEL], DISP_OFF);
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_CAM_SEL], DISP_OFF);//chpark.24.06.03
			
			ituWidgetSetVisible(T400_STEP_INFO_Cont[T_STEP_INFO_ROTATE_SEL], DISP_ON);//chpark.24.06.03
			ituWidgetGetGlobalPosition(T400_STEP_INFO_Cont[T_STEP_INFO_ROTATE_SEL], &x, &y);
			
			if(bVertical)
				ituWidgetSetPosition(T400_Next_Btn_Cont, x-Btn_width,y);	
			else
				ituWidgetSetPosition(T400_Next_Btn_Cont, x-Btn_width,y);	
			break;

		default:
			if(bVertical)
				ituWidgetSetPosition(T400_Next_Btn_Cont, 800-Btn_width,(1280-Btn_height)/2);	
			else
				ituWidgetSetPosition(T400_Next_Btn_Cont, 1280-Btn_width,(800-Btn_height)/2);	
			break;
	}	
}

void GotoFacMode(int nStep)
{
	//nStepVal = nStep*10; //Majon Step + Minor Step
	if(nStep==0)
	{
		TestMode_MakeSerial();
		TestMode_Send_TX(1,T_PKT_UID,0,0);//chpark.24.05.27
		TestMode_Send_TX(1,T_PKT_UID,1,0);//chpark.24.05.27

		TestMode_Send_TX(1,T_PKT_MODEL,0,0);//chpark.24.05.27
	}

	int nStep_Main = nStep/10;
	int nStep_Sub  = nStep%10;

	if(nStep==-1)
	{
		g_nTest_Step = 0;
		ituWidgetSetVisible(TestMode_CONT, DISP_OFF);
		return;
	}
	else
	{
		ituWidgetSetVisible(TestMode_CONT, DISP_ON);
		
		
		ituWidgetSetVisible(T_STEP_Cont[T_STEP_LCD], DISP_OFF);
		ituWidgetSetVisible(T_STEP_Cont[T_STEP_INFO], DISP_OFF);
	}

	g_nTest_Step = nStep;//chpark.24.06.03

	printf("g_nTest_Step=nStep=%d nStep_Main=%d nStep_Sub=%d\n",nStep,nStep_Main,nStep_Sub);
	switch(nStep_Main)
	{
		case T_STEP_LCD: //0
			{
				SetNextBtnPosition(nStep);

				ituWidgetSetVisible(T_STEP_Cont[T_STEP_LCD], DISP_ON);
				for(int i=0;i<T_STEP_LCD_END;i++)
				{
					ituWidgetSetVisible(T400_STEP_LCD_Cont[i], DISP_OFF);
				}

				if(nStep_Sub>=T_STEP_LCD_RED && nStep_Sub<=T_STEP_LCD_WHITE)
					ituWidgetSetVisible(T400_STEP_LCD_Cont[nStep_Sub], DISP_ON);
			}
			break;

		case T_STEP_CAM: //1
			{
				ituWidgetSetVisible(TestMode_CONT, DISP_OFF);
			}
			break;

		case T_STEP_INFO: //2
			{
				ituWidgetSetVisible(TestMode_CONT, DISP_ON);
				ituWidgetSetVisible(T_STEP_Cont[T_STEP_LCD], DISP_OFF);
				ituWidgetSetVisible(T_STEP_Cont[T_STEP_INFO], DISP_ON);

				SetNextBtnPosition(nStep);
			}
			break;

		case T_STEP_ROTATE: //3
			{
				ituWidgetSetVisible(TestMode_CONT, DISP_ON);
				ituWidgetSetVisible(T_STEP_Cont[T_STEP_LCD], DISP_OFF);
				ituWidgetSetVisible(T_STEP_Cont[T_STEP_INFO], DISP_ON);

				SetNextBtnPosition(nStep);
			}
			break;

		case T_STEP_END: //4
			{
				ituWidgetSetVisible(TestMode_CONT, DISP_OFF);
				
				g_nFac_TestModeFlag = 0;
				g_nTest_Step = 0;
				char JD_Buf[10] = {0,};
				JD_Buf[0] = 0;
				MainHome(NULL, JD_Buf);
			}
			break;

		case 5:
			break;

		default:
			break;
	}
}

void TestMode_ShowTime(uint16_t Year,uint8_t Mon,uint8_t Day,uint8_t Hour,uint8_t Min,uint8_t Sec)
{
	static int nSec_Cnt = -1;
	static int nSec_Cnt_Start = 0;
	static int nSec_Cnt_End = 0;

	int nSec_Diff=0;

	if(g_nFac_TestModeFlag==1)
	{
		//chpark.24.07.02
		if(nSec_Cnt<0)
		{
			nSec_Cnt = 0;
			nSec_Cnt_Start = Sec;
		}
		else
		{
			nSec_Cnt++;

			if(nSec_Cnt>=5)
			{
				
				nSec_Cnt_End = Sec;

				if(nSec_Cnt_Start>nSec_Cnt_End)
				{
					nSec_Diff = (nSec_Cnt_End+60) - nSec_Cnt_Start;
				}
				else if(nSec_Cnt_Start<nSec_Cnt_End)
				{
					nSec_Diff = nSec_Cnt_End - nSec_Cnt_Start;
				}
				else
				{
					nSec_Diff=0;
				}

				if(nSec_Diff==5)
				{
					printf("[RTC is good] nSec_Diff=%d nSec_Cnt=%d nSec_Cnt_Start=%d nSec_Cnt_End=%d\n",nSec_Diff,nSec_Cnt,nSec_Cnt_Start,nSec_Cnt_End );
				}
				else
				{
					printf("[RTC is bad] nSec_Diff=%d nSec_Cnt=%d nSec_Cnt_Start=%d nSec_Cnt_End=%d\n",nSec_Diff,nSec_Cnt,nSec_Cnt_Start,nSec_Cnt_End );
				}

				nSec_Cnt = -1;
				nSec_Cnt_Start = 0;
				nSec_Cnt_End = 0;
			}
		}
		

		char tmp_time[30];
		memset(tmp_time,0,sizeof(tmp_time));
		sprintf(tmp_time, "%02d/%02d/%02d  %02d:%02d:%02d",Year,Mon,Day, Hour,Min,Sec);  
		ituTextSetString(T400_STEP_INFO_Cont[T_STEP_INFO_RTC], tmp_time);
		
		//chpark.24.05.07
		ituWidgetSetVisible(T400_Next_Btn_Blink[T_NEXT_ICON_PUSH], T400_bShowIcon);
		ituWidgetSetVisible(T400_Next_Btn_Blink[T_NEXT_ICON_SELECT], T400_bShowIcon);
		T400_bShowIcon = !T400_bShowIcon;

		//testmode packet received.
		//chpark.24.05.27
		if(g_TestMode_Req_Time>0)
		{
			g_TestMode_Req_Time = 0;
		}
	}
	else
	{
		nSec_Cnt = 0;
		nSec_Cnt_Start = 0;
		nSec_Cnt_End = 0;
	}
}



//chpark.24.05.27
bool TestMode_Send_TX(uint8_t nPacketMode, uint8_t nPacketType, uint8_t nStepVal, uint8_t nResult)
{
    uint8_t  txbuffer[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	if(nPacketType==T_PKT_UID)
	{
		int nStartByte;
		if(nStepVal==0)
		{
			nStartByte = 0;
		}
		else
		{
			nStartByte = 5;
		}
		//nPacketMode:XO_TMODE_REQ, XO_TMODE_ACK
		txbuffer[0] = nPacketType;  //packet type  //T_PKT_UID=0,T_PKT_MODEL=1,T_PKT_VERSION=2,T_PKT_TEST_STEP=3
		txbuffer[1] = T_MODEL_800T; //model
		txbuffer[2] = nStepVal;  //version high
		txbuffer[3] = Device_UID[nStartByte+0];
		txbuffer[4] = Device_UID[nStartByte+1];
		txbuffer[5] = Device_UID[nStartByte+2];
		txbuffer[6] = Device_UID[nStartByte+3];
		txbuffer[7] = Device_UID[nStartByte+4];

		//printf("[TestMode_Send_TX]ithGetDeviceId=%d ithGetRevisionId=%d\n",ithGetDeviceId(),ithGetRevisionId());
		printf("[TestMode_Send_TX]nPacketMode=%d nPacketType=%d nStepVal=%d  Device_UID=%s\n",nPacketMode,nPacketType,nStepVal,Device_UID);
	}
	else 
	{
		//nPacketMode:XO_TMODE_REQ, XO_TMODE_ACK
		txbuffer[0] = nPacketType;  //packet type  //T_PKT_UID=0,T_PKT_MODEL=1,T_PKT_VERSION=2,T_PKT_TEST_STEP=3
		txbuffer[1] = T_MODEL_800T; //model
		txbuffer[2] = CFG_VERSION_MAJOR|0x30;  //version high
		txbuffer[3] = '.';
		txbuffer[4] = CFG_VERSION_MINOR|0x30;
		txbuffer[5] = CFG_VERSION_PATCH|0x30;
		txbuffer[6] = nStepVal;//test type
		txbuffer[7] = nResult; //test result

		printf("[TestMode_Send_TX]ithGetDeviceId=%d ithGetRevisionId=%d\n",ithGetDeviceId(),ithGetRevisionId());
		printf("[TestMode_Send_TX]nPacketMode=%d nPacketType=%d nStepVal=%d  nResult=%d\n",nPacketMode,nPacketType,nStepVal,nResult);
	}

	if(nPacketMode==0)
		CanbusTxDataManagement(gMainTimerCnt_10m,XO_TMODE_REQ,txbuffer);
	if(nPacketMode==1)
		CanbusTxDataManagement(gMainTimerCnt_10m,XO_TMODE_ACK,txbuffer);
}

//==================================================================================

bool MainVideoIcon(DISP_MODE DispMode)
{
	if (MainLayerFlag)
	{
		{
			if ((gCameraStatus0==4)||(gCameraStatus1==4))
			{
				ituWidgetSetVisible(MainNoCamera, DISP_OFF);
			}
			else
			{
				ituWidgetSetVisible(MainNoCamera, DISP_ON);
			}
		}
	}
	return true;
}
bool MainLayer_ButtonSel(uint8_t SelIndex,uint8_t OnOff)
{
	static uint8_t tSelIndex=0xFF;
	static uint8_t tOnOff=0xFF;
//	printf("=================> MainLayer_ButtonSel=%d %d %d\n",SelIndex,OnOff,Main_Disp_Mode);
	
//	printf("=================> MainLayer_ButtonSel Start=%d %d %d\n",Main_Select_Index,MainMode_AC_Select_Index,MainMode_RD_Select_Index);
//	if ((tSelIndex!=SelIndex)||(tOnOff!=OnOff))
	{
		ituWidgetSetVisible(Bar_Sel, DISP_OFF);
		ituWidgetSetVisible(ATT_Sel, DISP_OFF);
		ituWidgetSetVisible(HourMtSel, DISP_OFF);
		ituWidgetSetVisible(Power_Sel, DISP_OFF);
		ituWidgetSetVisible(TimeDataSel, DISP_OFF);
		ituWidgetSetVisible(MainCameraSel, DISP_OFF);
		ituWidgetSetVisible(Main_SPK_Sel, DISP_OFF);
		ituWidgetSetVisible(Main_RadioSel, DISP_OFF);
		ituWidgetSetVisible(Main_ACSel, DISP_OFF);
		ituWidgetSetVisible(MainMenuSel, DISP_OFF);
		if (OnOff==DISP_ON)
		{
			switch (SelIndex)
			{
				case MAIN_SEL_ATT:
				{
					ituWidgetSetVisible(ATT_Sel, DISP_ON);
				}
				break;
				case MAIN_SEL_AIRCON:
				{
					ituWidgetSetVisible(Main_ACSel, DISP_ON);
				}
				break;
				case MAIN_SEL_RADIO:
				{
					ituWidgetSetVisible(Main_RadioSel, DISP_ON);
				}
				break;
				case MAIN_SEL_SPK:
				{
					ituWidgetSetVisible(Main_SPK_Sel, DISP_ON);
				}
				break;
				case MAIN_SEL_HOURMETER:
				{
					ituWidgetSetVisible(HourMtSel, DISP_ON);
				}
				break;
				case MAIN_SEL_BAR:
				{
					ituWidgetSetVisible(Bar_Sel, DISP_ON);
				}
				break;
				case MAIN_SEL_TIMEDATE:
				{
					ituWidgetSetVisible(TimeDataSel, DISP_ON);
				}
				break;
				case MAIN_SEL_POWER:
				{
					ituWidgetSetVisible(Power_Sel, DISP_ON);
				}
				break;
				case MAIN_SEL_CAMERA:
				{
					ituWidgetSetVisible(MainCameraSel, DISP_ON);
				}
				break;
				case MAIN_SEL_MENU:
				{
					ituWidgetSetVisible(MainMenuSel, DISP_ON);
				}
				break;
			}
			MainMode_AC_Sel(1,MainMode_AC_Select_Index,DISP_OFF);
			MainMode_RD_Sel(1,MainMode_RD_Select_Index,DISP_OFF);
		}
		tSelIndex=SelIndex;
		tOnOff=OnOff;
		Main_Select_Index = SelIndex;
		Main_Select_Index_OnOff = OnOff;
	}
//	printf("=================> MainLayer_ButtonSel End=%d %d %d\n",Main_Select_Index,MainMode_AC_Select_Index,MainMode_RD_Select_Index);
	return true;
}


bool MainLayer_MeterERpmDisp(DISP_MODE DispMode,int8_t Symbol,int32_t rpm)
{
	int32_t RealRpm = 0;
	int32_t tRpm = 1,i;
	static DISP_MODE tDispMode = 0x01;
	

	if (Main_tSymbol!=Symbol)
	{
		Main_TempRpm = LONG_KEY_TIME;

		ituWidgetSetVisible(Bar_PM, DISP_OFF);
		ituWidgetSetVisible(Bar_LiftLd, DISP_OFF);
		ituWidgetSetVisible(Bar_EngRPM, DISP_OFF);
		ituWidgetSetVisible(Bar_EngLd, DISP_OFF);
		ituWidgetSetVisible(Bar_ASH, DISP_OFF);
		ituWidgetSetVisible(BarGaugeBar, DISP_OFF);
    	switch (Symbol)
    	{
    		case m11_MeterSymbol_PM:
    		{
				ituWidgetSetVisible(Bar_PM, DISP_ON);
				ituWidgetSetVisible(BarGaugeBar, DISP_ON);
    		}
    		break;
    		case m12_MeterSymbol_LiftLd:
   			{
				ituWidgetSetVisible(Bar_LiftLd, DISP_ON);
    		}
    		break;
    		case m13_MeterSymbol_EngRPM:
    		{
				ituWidgetSetVisible(Bar_EngRPM, DISP_ON);
    		}
    		break;
    		case m14_MeterSymbol_EngLd:
    		{
 				ituWidgetSetVisible(Bar_EngLd, DISP_ON);
    		}
    		break;
    		case m11_MeterSymbol_AHS:
    		{
 				ituWidgetSetVisible(Bar_ASH, DISP_ON);
    		}
    		break;    		
    	}	
    	Main_tSymbol=Symbol;
	}

	if ((Main_TempRpm!=rpm)||(tDispMode!=DispMode))
	{
		RealRpm = rpm;
		if ((RealRpm==0xFF)||(RealRpm==0))
		{
			tRpm = 0;
			RealRpm=0;
		}
		if (RealRpm>=100)
		{
			RealRpm = 100;
		}
		if (RealRpm>0)
		{
			RealRpm  = (RealRpm /10	);
		}
		
		for (i=0;i<14;i++)
		{
			ituWidgetSetVisible(Bar_Gauge[i], DISP_OFF);
		}
//		printf("MainLayer_MeterERpmDisp=%d %d\n",RealRpm,MRV.Symbol);
		if (RealRpm!=0)
		{
	    	switch (MRV.Symbol)
	    	{
	    		case m11_MeterSymbol_PM:
	    		{
					if (RealRpm>=9)
					{
						ituWidgetSetVisible(Bar_Gauge[RealRpm+3], DISP_ON);
					}
					else if (RealRpm>=7)
					{
						ituWidgetSetVisible(Bar_Gauge[RealRpm+3], DISP_ON);
					}
					else
					{
						ituWidgetSetVisible(Bar_Gauge[RealRpm-1], DISP_ON);
					}
	    		}
	    		break;
	    		case m12_MeterSymbol_LiftLd:
	    		case m13_MeterSymbol_EngRPM:
	    		case m11_MeterSymbol_AHS:
	    		case m14_MeterSymbol_EngLd:
	    		{
					ituWidgetSetVisible(Bar_Gauge[RealRpm-1], DISP_ON);
	    		}
	    		break;
	    	}		
	    }
		Main_TempRpm = rpm;
		tDispMode = DispMode;
	}
	return true;
}



bool Main_EI_5BMeterDisp(uint16_t Value,uint32_t TimeCnt)
{
	int32_t tValue = 0xFFFF;
	int32_t i;
	if (Value == 0xFFFF)
	{
		ituWidgetSetVisible(EI_5B_Graduation, DISP_OFF);
		ituWidgetSetVisible(EI_5B, DISP_OFF);	
		tValue = Value;
	}
	else if ((Main_EI_5BTime+TIME_CNT_3SEC)<gMainTimerCnt_10m)
	{
		for (i=0;i<10;i++)
		{
			ituWidgetSetVisible(EI_Gauge[i], DISP_OFF);
		}
		tValue = 0;
		Main_EI_5BValue = 0;
		Main_EI_5B = 0;
	}
	else if (Main_EI_5B != Value)
	{
//		printf("Main_EI_5BMeterDisp=%d,%d\n",Value,tValue);
		tValue = Value;
		for (i=0;i<10;i++)
		{
			ituWidgetSetVisible(EI_Gauge[i], DISP_OFF);
		}
		if (tValue<1)
		{
		}
		else if(tValue<11)
		{
			ituWidgetSetVisible(EI_Gauge[0], DISP_ON);
		}
		else if(tValue<21)
		{
			ituWidgetSetVisible(EI_Gauge[1], DISP_ON);
		}
		else if(tValue<31)
		{
			ituWidgetSetVisible(EI_Gauge[2], DISP_ON);
		}
		else if(tValue<41)
		{
			ituWidgetSetVisible(EI_Gauge[3], DISP_ON);
		}
		else if(tValue<51)
		{
			ituWidgetSetVisible(EI_Gauge[4], DISP_ON);
		}
		else if(tValue<61)
		{
			ituWidgetSetVisible(EI_Gauge[5], DISP_ON);
		}
		else if(tValue<71)
		{
			ituWidgetSetVisible(EI_Gauge[6], DISP_ON);
		}
		else if(tValue<81)
		{
			ituWidgetSetVisible(EI_Gauge[7], DISP_ON);
		}
		else if(tValue<91)
		{
			ituWidgetSetVisible(EI_Gauge[8], DISP_ON);
		}
		else
		{
			ituWidgetSetVisible(EI_Gauge[9], DISP_ON);
		}
	    ituWidgetSetVisible(EI_5B_Graduation, DISP_ON);
	    ituWidgetSetVisible(EI_5B, DISP_ON);
		Main_EI_5B = Value;
	}
	return true;
}
bool Main_DD_1BMeterDisp(uint16_t Value,uint32_t TimeCnt)
{
	uint32_t tValue = 0xFFFF;
	int32_t i;
	if ((M5_Fuel_Timer+TIME_CNT_3SEC)<gMainTimerCnt_10m)
	{
		for (i=0;i<10;i++)
		{
			ituWidgetSetVisible(DD_Gauge[i], DISP_OFF);
		}
		Main_DD_1B = 0;
		Main_DD_1BValue = 0;
	}
	else if (Value!=Main_DD_1B)
	{
//		printf("Main_DD_1BMeterDisp=%d,%d\n",Value,Main_DD_1B);
		tValue = Value/10;
		for (i=0;i<10;i++)
		{
			ituWidgetSetVisible(DD_Gauge[i], DISP_OFF);
		}
		if (tValue<1)
		{
		}
		else if(tValue<11)
		{
			ituWidgetSetVisible(DD_Gauge[0], DISP_ON);
		}
		else if(tValue<21)
		{
			ituWidgetSetVisible(DD_Gauge[1], DISP_ON);
		}
		else if(tValue<31)
		{
			ituWidgetSetVisible(DD_Gauge[2], DISP_ON);
		}
		else if(tValue<41)
		{
			ituWidgetSetVisible(DD_Gauge[3], DISP_ON);
		}
		else if(tValue<51)
		{
			ituWidgetSetVisible(DD_Gauge[4], DISP_ON);
		}
		else if(tValue<61)
		{
			ituWidgetSetVisible(DD_Gauge[5], DISP_ON);
		}
		else if(tValue<71)
		{
			ituWidgetSetVisible(DD_Gauge[6], DISP_ON);
		}
		else if(tValue<81)
		{
			ituWidgetSetVisible(DD_Gauge[7], DISP_ON);
		}
		else if(tValue<91)
		{
			ituWidgetSetVisible(DD_Gauge[8], DISP_ON);
		}
		else
		{
			ituWidgetSetVisible(DD_Gauge[9], DISP_ON);
		}
		Main_DD_1B = Value;
	}
	return true;
}
bool Main_SI_67BMeterDisp(uint16_t Value,uint32_t TimeCnt)
{
	uint32_t tValue = 0xFFFF;
	
//	printf("Main_SI_67BMeterDisp=%d\n",Value,tValue,Main_SI_67BTime);
	
	if (Value == 0xFFFF)
	{
		ituWidgetSetVisible(SI_67B, DISP_OFF);
		ituWidgetSetVisible(SI_67B_Graduation, DISP_OFF);
		tValue = Value;
	}
	else if ((Main_SI_67B != Value)||((Main_SI_67BTime+TIME_CNT_3SEC)<gMainTimerCnt_10m))
	{
//		printf("Main_SI_67BMeterDisp=%d\n",Value);
		if ((Main_SI_67BTime+TIME_CNT_3SEC)<gMainTimerCnt_10m)
		{
			ituMeterSetValue(SI_67B, 0);
			tValue = 0;
			Main_SI_67BValue = 0;
			Main_SI_67B = 0;
		}
		else
		{
			if (Value>12000)
			{
				tValue = 12000;
			}
			else if (Value<=10000)
			{
				tValue = Value*96/100;
			}
			else
			{
				tValue = 9600+((Value-10000)/100*120);
			}
			ituMeterSetValue(SI_67B, tValue);
			Main_SI_67B = Value;
		}
		ituWidgetSetVisible(SI_67B, DISP_ON);
		ituWidgetSetVisible(SI_67B_Graduation, DISP_ON);
	}
	return true; 
}
bool Main_EI_34BMeterDisp(uint16_t Value,uint32_t TimeCnt)
{
	static uint32_t tValue = 0xFFFF;
	if ((Main_EI_5BTime+TIME_CNT_3SEC)<gMainTimerCnt_10m)
	{
//		printf("[Main_EI_34BMeterDisp 1][%d][%d][%d]\n", Main_EI_34B,Main_EI_34BValue,Value);
		ituMeterSetValue(EI_34B, 0);
		Main_EI_34B = 0;
		Main_EI_34BValue = 0;
	}
	else if (Main_EI_34B != Value)
	{
//		printf("[Main_EI_34BMeterDisp 2][%d][%d][%d]\n", Main_EI_34B,Main_EI_34BValue,Value);
		if (Value>12000)
		{
			tValue = 12000;
		}
		else if (Value<=10000)
		{
			tValue = Value*96/100;
		}
		else
		{
			tValue = 9600+((Value-10000)/100*120);
		}
		ituMeterSetValue(EI_34B, tValue);
		Main_EI_34B = Value;
	}
	return true; 
}


bool MainLayer_HourMeterDisp(DISP_MODE DispMode,uint8_t Trip,uint32_t HourMeterS,uint32_t TimeCnt,uint8_t OnOff)
{
	static uint8_t tTrip = 0xFF,i;
	static uint32_t tHourMeterS = 0x01;
	static DISP_MODE tDispMode = 0x01;
	static bool OnFlag_500ms = 0x01,tToggle=0;
	char tmp[32];
	static uint8_t tBG_Color = 0xFF;
	static uint8_t tOnOff = 0xFF;
	
	
	if ((tOnOff != OnOff))
	{
		printf("MainLayer_HourMeterDisp= %d,%d\n",OnOff,Main_Disp_Mode);
		tOnOff = OnOff;
		if (OnOff==DISP_OFF)
		{
			for (i = 0; i < 4; i++)
			{
				ituWidgetSetVisible(HourMt[i], DISP_OFF);
			}
			ituWidgetSetVisible(HourMtTx, DISP_OFF);
			return true;
		}
		else
		{
//			if (Main_Disp_Mode == MAIN_MODE_CAMERA)
			{
//				return true;
			}
			ituWidgetSetVisible(HourMt[Trip], DISP_ON);
			ituWidgetSetVisible(HourMtTx, DISP_ON);
			
	//		printf("MainLayer_HourMeterDisp DISP_ON1 = %d,%d\n",OnOff,Main_Disp_Mode);
		}
	}
	if ((tBG_Color!=BG_Color)||((tToggle==1)&&(HMV.HourMeter_Toggle==0)))
	{
		tTrip = 0xFF;
		tDispMode = 0xFF;
		tBG_Color=BG_Color;
		tToggle==0;
	}
//	printf("MainLayer_HourMeterDisp= %d,%d\n",tTrip,Trip);
	if ((tTrip!=Trip)||(tDispMode!=DispMode))
	{
		for (i = 0; i < 4; i++)
		{
			ituWidgetSetVisible(HourMt[i], DISP_OFF);
		}
		if (OnOff==DISP_ON)
		{
			if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
			{
				ituWidgetSetVisible(HourMt[Trip], DISP_ON);
			}
			else
			{
				ituWidgetSetVisible(HourMt[Trip], DISP_ON);
			}
		}
	//	printf("MainLayer_HourMeterDisp DISP_ON2 = %d,%d\n",OnOff,Main_Disp_Mode);
		tToggle=0;
	}
	if ((HMV.HourMeter_Toggle==1)&&(Trip==0)&&(OnOff==DISP_ON))
	{
		if (OnFlag_500ms != gOnFlag_500ms)
		{
			OnFlag_500ms = gOnFlag_500ms;
			if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
			{
				ituWidgetSetVisible(HourMt[Trip], OnFlag_500ms);
			}
			else
			{
				ituWidgetSetVisible(HourMt[Trip], OnFlag_500ms);
			}
		}
		tToggle=1;			
	}

	
	//if (((tTrip==Trip)&&((tHourMeterS!=HourMeterS)||(tDispMode!=DispMode)))||(tTrip!=Trip))
	if (((tHourMeterS!=HourMeterS)||(tDispMode!=DispMode)||(tTrip!=Trip))&&((OnOff==DISP_ON)))
	{
		//printf("MainLayer_HourMeterDisp=Trip:%d,HourMeterS:%d\n",Trip,HourMeterS);
		tHourMeterS=HourMeterS;
		
		if (HourMeterS==0xFFFFFFFF)
		{
			sprintf(tmp, "N/A hr");
		}
		else
		{
			if (tHourMeterS>999999)
				tHourMeterS=999999;
			sprintf(tmp, "%d.%d hr", tHourMeterS/10,tHourMeterS%10);
		}  
		ituTextSetString(HourMtTx, tmp);	
	}
	tTrip = Trip;
	tHourMeterS = HourMeterS;
	tDispMode = DispMode;	
	return true;
}


bool Main_HourMtDisp(uint8_t OnOff)
{
	static uint32_t rHour[7]={0xff,0,1,0xff,0xff,9,0xff};
	char i;
	
	for (i=0;i<7;i++)
	{
		rHour[i] = HMV.HourMete_rHour[i];
	}	
	for (i=0;i<7;i++)
	{
		if (rHour[i]>999999)
			rHour[i]=999999;
	}
	for (i=0;i<7;i++)
	{
		if (HMV.HourMete_rHour[i]==0xFFFFFFFF)
		{
			//rHour[i]=0xFFFFFFFF;
			rHour[i]=999999;
		}
	}
	if ((HM_Time+TIME_CNT_5SEC)<gMainTimerCnt_10m)
	{
		rHour[0] = 0xFFFFFFFF;
		HMV.HourMeter_Toggle = 0;
	}
	if ((TM1_Time+TIME_CNT_5SEC)<gMainTimerCnt_10m)
	{
		rHour[1]=0xFFFFFFFF;
		rHour[2]=0xFFFFFFFF;
	}
	if ((TM2_Time+TIME_CNT_5SEC)<gMainTimerCnt_10m)
	{
		rHour[3]=0xFFFFFFFF;
		rHour[4]=0xFFFFFFFF;
	}
	if ((TM3_Time+TIME_CNT_5SEC)<gMainTimerCnt_10m)
	{
		rHour[5]=0xFFFFFFFF;
		rHour[6]=0xFFFFFFFF;
	}
	if (rHour[0] == 0)
	{
//		HMV.HourMeter_Toggle=0;
	}
	MainLayer_HourMeterDisp(DISP_ON,HMV.HourMeter_Trip, rHour[HMV.HourMeter_Trip],gMainTimerCnt_10m,OnOff);
}



bool MainLayer_YearDayDisp(DISP_MODE DispMode,uint16_t Year,uint8_t Mon,uint8_t Day,uint8_t Hour,uint8_t Min,bool ReDisp)
{
	static int32_t TempYear = 0;
	static int32_t TempMon = 0;
	static int32_t TempDay = 0xff;
	static int32_t TempHour = 0xff;
	static int32_t TempMin = 0;	
	static uint8_t tDTM_DateTime_Mode=0;
	char tmp[32];


		
//	printf("MainLayer_YearDayDisp Main_Disp_Mode: %d   MainBtFlag : %d  %d\n\n",Main_Disp_Mode,MainBtFlag,RotationMode);
	
	ituWidgetSetVisible(TimeDataTx, DISP_ON);

	if ((ReDisp)||(TempYear!=Year)||(TempMon!=Mon)||(TempDay!=Day)||(tDTM_DateTime_Mode!=DTM_DateTime_Mode)||(TempHour!=Hour)||(TempMin!=Min))
	{
		if (DTM_DateTime_Mode == Type1_YYYYMMDD)	// Type1_YYYYMMDD:1 , Type2_MMDDYYYY:2 , Type3_DDMMYYYY:3
		{
			sprintf(tmp, "%04d/%02d/%02d %02d:%02d", Year,Mon,Day,Hour,Min);  
			ituTextSetString(TimeDataTx, tmp);
		}
		else if (DTM_DateTime_Mode == Type3_DDMMYYYY)	// Type1_YYYYMMDD:1 , Type2_MMDDYYYY:2 , Type3_DDMMYYYY:3
		{
			sprintf(tmp, "%02d/%02d/%04d %02d:%02d", Day,Mon,Year,Hour,Min);
			ituTextSetString(TimeDataTx, tmp);							
		}
		else if (DTM_DateTime_Mode == Type2_MMDDYYYY)	// Type1_YYYYMMDD:1 , Type2_MMDDYYYY:2 , Type3_DDMMYYYY:3
		{
			sprintf(tmp, "%02d/%02d/%04d %02d:%02d", Mon,Day,Year,Hour,Min);  
			ituTextSetString(TimeDataTx, tmp);
		}
		TempYear=Year;
		TempMon=Mon;
		TempDay=Day;
		TempHour=Hour;
		TempMin=Min;
		tDTM_DateTime_Mode=DTM_DateTime_Mode;
	}
	return true;
}
bool MainDispDate(void)
{
	uint16_t year;
	uint8_t mon;
	uint8_t mday;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	

	if (!MainLayerFlag)
		return 0;	
	{
		HT_gettimeofday(&year,&mon,&mday,&hour,&min,&sec);
		MainLayer_YearDayDisp(DISP_ON,year,mon,mday,hour,min,0);
		TestMode_ShowTime(year,mon,mday,hour,min,sec);//chpark.24.04.24
	}
	return true;
}
bool MainDispDateFirst(void)
{
	uint16_t year;
	uint8_t mon;
	uint8_t mday;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;

	if (!MainLayerFlag)
		return 0;	
	printf("MainDispDateFirst Main_Disp_Mode: %d   MainBtFlag : %d  %d\n\n",Main_Disp_Mode,MainBtFlag,RotationMode);
	{
		HT_gettimeofday(&year,&mon,&mday,&hour,&min,&sec);
		MainLayer_YearDayDisp(DISP_ON,year,mon,mday,hour,min,1);
	}
	return true;
}
bool Main_EEC1_34BMeterDisp(ITUText* TextWidget)
{       
	char tmp[32] = {0,};
	static uint32_t EEC1_34Bytes_TIME = 0;
	uint32_t tData = 0;

	if (!MainLayerFlag)
		return 0;
	if ((DTVal.EEC1_34Bytes==0xFFFFFFFF)||((DTVal.EEC1_34Time+TIME_CNT_5SEC)<gMainTimerCnt_10m))
	{
		DTVal.EEC1_34Cnt = 0;
		DTVal.EEC1_34Bytes=0;
//		if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
//			sprintf(tmp, "N/A ");
//		else
			sprintf(tmp, "N/A  ");
		EEC1_34Bytes_TIME = 0;
		ituTextSetString(TextWidget, tmp);
	}
	else
	{
		if (DTVal.EEC1_34Cnt==0)
		{
			EEC1_34Bytes_TIME = gMainTimerCnt_10m;
			DTVal.EEC1_34Cnt = 0;
			DTVal.EEC1_34Bytes=0;
		}
		else if ((EEC1_34Bytes_TIME+TIME_CNT_500MS)<gMainTimerCnt_10m)
		{
			tData = DTVal.EEC1_34Bytes/DTVal.EEC1_34Cnt;
//			printf("Main_EEC1_34BMeterDisp2=%d %d %d %d\n",DTVal.EEC1_34Bytes,DTVal.EEC1_34Cnt,tData);
			DTVal.EEC1_34Bytes = 0;
			DTVal.EEC1_34Cnt = 0;	// JPHong0830			
			sprintf(tmp, "%d", tData);
			EEC1_34Bytes_TIME = gMainTimerCnt_10m;
			ituTextSetString(TextWidget, tmp);
		}
	}
	return true;
}
bool CalRadioFreq(char *tmp,uint8_t RDMode,uint16_t RDFreq)
{
	uint32_t tRDFreq;
	switch (Radio_Region)
	{
		case 0:	// japan on
		{
			if (RDMode==0) //AM
			{
				tRDFreq = RDFreq*9+522;
				sprintf(tmp, "     %d kHz  ", tRDFreq);
			}
			else //if (RDMode==1)	//FM
			{
				//tRDFreq = RDFreq+760;
				tRDFreq = 7600 + (RDFreq*10);// + (RDV.RDStep*5);
				sprintf(tmp, "    %d.%02d MHz ", tRDFreq/100,tRDFreq%100);
			}
		}
		break;
		case 1:	// north america on
		{
			if (RDMode==0) //AM
			{
				tRDFreq = (RDFreq*10+530);
				sprintf(tmp, "     %d kHz  ", tRDFreq);
			}
			else //if (RDMode==1)	//FM
			{
				tRDFreq = (RDFreq*2+877);
				sprintf(tmp, "    %d.%01d0 MHz ", tRDFreq/10,tRDFreq%10);
			}
		}
		break;
		case 2:	// europe on
		{
			if (RDMode==0) //AM
			{
				tRDFreq = (RDFreq*9+522);
				sprintf(tmp, "     %d kHz  ", tRDFreq);
			}
			else //if (RDMode==1)	//FM
			{
				tRDFreq = 8750 + (RDFreq*10) + (RDV.RDStep*5);//RDFreq+875;
				sprintf(tmp, "    %d.%02d MHz ", tRDFreq/100,tRDFreq%100);
			}
		}
		break;
		case 3:	// latin america on	
		{
			if (RDMode==0) //AM
			{
				tRDFreq = (RDFreq*5+530);
				sprintf(tmp, "     %d kHz  ", tRDFreq);
			}
			else //if (RDMode==1)	//FM
			{
				tRDFreq = RDFreq+875;
				sprintf(tmp, "    %d.%01d0 MHz ", tRDFreq/10,tRDFreq%10);
			}
		}
		break;
		
	}
}

bool MainLayer_FuncATTDisp(uint8_t OutMode,uint8_t Icon1,uint8_t Icon2,uint8_t Icon3,bool ReDisp,DISP_MODE DispMode)
{
	static uint8_t tOutMode=99;
	static uint8_t tIcon1=0;
	static uint8_t tIcon2=0;
	static uint8_t tIcon3=0;
 
	printf("MainLayer_FuncATTDisp=OutMode:%d,Icon1:%d,Icon2:%d,Icon3:%d,ReDisp:%d\n",OutMode,Icon1,Icon2,Icon3,ReDisp);
//	return true;
	if (MainLayerFlag)
	{
		if ((ReDisp==1)||(DispMode==DISP_OFF))
		{
			int i,j;
			for (i=0;i<3;i++)
			{
				for (j=0;j<10;j++)
				{
					ituWidgetSetVisible(ATT_S[i][j], DISP_OFF);
					ituWidgetSetVisible(ATT_B[i][j], DISP_OFF);
				}
			}
			tOutMode=99;
		}
	
		if (((tOutMode != OutMode)||(tIcon1 != Icon1)||(tIcon2 != Icon2)||(tIcon3 != Icon3))&&(DispMode!=DISP_OFF))
		{

		
					
			ituWidgetSetVisible(ATT_B[0][tIcon1], DISP_OFF);
			ituWidgetSetVisible(ATT_B[1][tIcon2], DISP_OFF);
			ituWidgetSetVisible(ATT_B[2][tIcon3], DISP_OFF);
			ituWidgetSetVisible(ATT_S[0][tIcon1], DISP_OFF);
			ituWidgetSetVisible(ATT_S[1][tIcon2], DISP_OFF);
			ituWidgetSetVisible(ATT_S[2][tIcon3], DISP_OFF);
			tOutMode=OutMode;
			tIcon1=Icon1;
			tIcon2=Icon2;
			tIcon3=Icon3;
			if (tOutMode==0)
			{
				ituWidgetSetVisible(ATT_B[0][tIcon1], DISP_ON);
				ituWidgetSetVisible(ATT_S[1][tIcon2], DISP_ON);
				ituWidgetSetVisible(ATT_S[2][tIcon3], DISP_ON);
			}
			else if (tOutMode==1)
			{
				ituWidgetSetVisible(ATT_S[0][tIcon1], DISP_ON);
				ituWidgetSetVisible(ATT_B[1][tIcon2], DISP_ON);
				ituWidgetSetVisible(ATT_S[2][tIcon3], DISP_ON);
			}
			else
			{
				ituWidgetSetVisible(ATT_S[0][tIcon1], DISP_ON);
				ituWidgetSetVisible(ATT_S[1][tIcon2], DISP_ON);
				ituWidgetSetVisible(ATT_B[2][tIcon3], DISP_ON);
			}
		}
	}
	return true;
}


uint8_t MainLayer_WarningDisp2(uint8_t DispData,DISP_MODE DispMode)
{
	static uint8_t tDispData=0;
	static uint8_t tDispMode=99;
	if ((tDispData!=DispData)||(tDispData!=DispData))
	{
//		printf("WarningDisp2=tDispData:%d,DispData:%d,tDispMode:%dDispMode:%d\n",tDispData,DispData,tDispMode,DispMode);
		tDispData=DispData;
		tDispMode=DispMode;
	}
	if (MainLayerFlag)
	{
		if (DispMode==DISP_ON)
		{
			ituWidgetSetVisible(EngStopSw_H, DISP_OFF);
			ituWidgetSetVisible(SafetyStart_H, DISP_OFF);
		}
		if (DispData == cW_EngStopSw)
		{
			ituWidgetSetVisible(EngStopSw_H, DispMode);
		}
		else //if (DispData == cW_SafetyStart)
		{
			ituWidgetSetVisible(SafetyStart_H, DispMode);
		}
	}
    return true;
}
static uint8_t tMainWarnigData[WARNING_ICON_CNT]={0xFF,};
static uint8_t tMainFunctionData[FUNCTION_ICON_CNT]={0xFF,};
bool wZoomFlag = 0;
bool fZoomFlag = 0;



bool Main_WarningInit(void)
{
	uint8_t tModelType = 0xFF,i;

	//if (tModelType!=ModelType)
	{
		for (i=0;i<WARNING_ICON_CNT;i++)
		{
			ituWidgetSetVisible(MainWarnigIcon[i][0], DISP_OFF);
			ituWidgetSetVisible(MainWarnigIcon[i][1], DISP_OFF);
			ituWidgetSetVisible(MainWarnigIcon[i][2], DISP_OFF);
			if(RotationMode==0)
			{
//				MainWarnigData[i] = 0;
			}
			tMainWarnigData[i]=0xFF;
		}
		MainLayer_WarningDisp2(cW_SafetyStart,DISP_OFF);
		MainLayer_WarningDisp2(cW_EngStopSw,DISP_OFF);			
	}

			
	{
		for (i=0;i<FUNCTION_ICON_CNT;i++)
		{
			ituWidgetSetVisible(MainFinctionIcon[i][0], DISP_OFF);
			ituWidgetSetVisible(MainFinctionIcon[i][1], DISP_OFF);
			ituWidgetSetVisible(MainFinctionIcon[i][2], DISP_OFF);
			if(RotationMode==0)
			{
//				MainFunctionData[i] = 0;
			}		
			tMainFunctionData[i]=0xFF;
		}
	}

//	ituWidgetSetVisible(Meter_BG, DISP_ON);
	ituWidgetSetVisible(Function_BG, DISP_ON);
	ituWidgetSetVisible(Warning_BG, DISP_ON);
	return true;
}




bool MainLayer_WarningIcon(int IconIndex,DISP_MODE DispMode)
{
	int i=0;
	static uint8_t tIconIndex=0;
	static uint8_t tDispMode=99;

	if (MainLayerFlag)
	{
		if (tMainWarnigData[IconIndex]!=DispMode)
		{
		//	if (tIconIndex<cTravelMode)
	//			printf("DISP : %d , %d ,%d , %d , %d  \n",tIconIndex,IconIndex,tDispMode,DispMode,IconIndex);
	
//			printf("MainLayer_WarningIcon=IconIndex:%d,DispMode:%d,tIconIndex_DispMode:%d\n",IconIndex,DispMode,tMainWarnigData[IconIndex]);
			if (DispMode==DISP_OFF)
			{
				ituWidgetSetVisible(MainWarnigIcon[IconIndex][0], DISP_OFF);
				ituWidgetSetVisible(MainWarnigIcon[IconIndex][1], DISP_OFF);
				ituWidgetSetVisible(MainWarnigIcon[IconIndex][2], DISP_OFF);
			}		
			else if ((DispMode==DISP_NORMAL_ON) ||(DispMode==DISP_ON))
			{
				ituAnimationGoto(MainWarnigIcon[IconIndex][0], 1);
				ituWidgetSetVisible(MainWarnigIcon[IconIndex][1], DISP_ON);
				ituWidgetSetVisible(MainWarnigIcon[IconIndex][2], DISP_ON);
				ituWidgetSetVisible(MainWarnigIcon[IconIndex][0], DISP_ON);
			}
			else if (DispMode==DISP_ZOOM_ON) 
			{
				ituAnimationGoto(MainWarnigIcon[IconIndex][0], 0);
				ituWidgetSetVisible(MainWarnigIcon[IconIndex][1], DISP_ON);
				ituWidgetSetVisible(MainWarnigIcon[IconIndex][2], DISP_ON);
				ituWidgetSetVisible(MainWarnigIcon[IconIndex][0], DISP_ON);
			}
			else if (DispMode==DISP_ANIMATION) 
			{
				ituAnimationGoto(MainWarnigIcon[IconIndex][0], 1);
				ituWidgetSetVisible(MainWarnigIcon[IconIndex][1], DISP_ON);
				ituWidgetSetVisible(MainWarnigIcon[IconIndex][2], DISP_ON);
				ituWidgetSetVisible(MainWarnigIcon[IconIndex][0], DISP_ON);
				ituAnimationPlay(MainWarnigIcon[IconIndex][0], 0);
			}			
			tMainWarnigData[IconIndex]=DispMode;
		}
	}
//	printf("MainLayer_WarningIcon End=IconIndex:%d,DispMode:%d,tIconIndex:%d\n",IconIndex,DispMode,tIconIndex);
    return true;
}

bool MainLayer_FunctionIcon(int IconIndex,DISP_MODE DispMode)
{
	int i=0;
	static uint8_t tIconIndex=0;
	static uint8_t tDispMode=99;
	uint8_t tDispIndex=0;
	tDispIndex=IconIndex-cF_01_B;
//	printf("MainLayer_FunctionIcon=IconIndex:%d,DispMode:%d,tIconIndex:%d\n",IconIndex,DispMode,tIconIndex);

//	static uint8_t tMainFunctionData[FUNCTION_ICON_CNT]={0xFF,};
	if (MainLayerFlag)
	{
		if (tMainFunctionData[tDispIndex]!=DispMode)
		{
		//	if (tIconIndex<cTravelMode)
//				printf("DISP : %d , %d ,%d \n",IconIndex,DispMode,tDispIndex);

//			printf("MainLayer_FunctionIcon=IconIndex:%d,DispMode:%d,tIconIndex_DispMode:%d\n",tDispIndex,DispMode,tMainFunctionData[tDispIndex]);
			if (DispMode==DISP_OFF)
			{
				ituWidgetSetVisible(MainFinctionIcon[tDispIndex][0], DISP_OFF);
				ituWidgetSetVisible(MainFinctionIcon[tDispIndex][1], DISP_OFF);
				ituWidgetSetVisible(MainFinctionIcon[tDispIndex][2], DISP_OFF);
			}		
			else if ((DispMode==DISP_NORMAL_ON) ||(DispMode==DISP_ON))
			{
				ituAnimationGoto(MainFinctionIcon[tDispIndex][0], 1);
				ituWidgetSetVisible(MainFinctionIcon[tDispIndex][1], DISP_ON);
				ituWidgetSetVisible(MainFinctionIcon[tDispIndex][2], DISP_ON);
				ituWidgetSetVisible(MainFinctionIcon[tDispIndex][0], DISP_ON);
			}
			else if (DispMode==DISP_ZOOM_ON) 
			{
				ituAnimationGoto(MainFinctionIcon[tDispIndex][0], 0);
				ituWidgetSetVisible(MainFinctionIcon[tDispIndex][1], DISP_ON);
				ituWidgetSetVisible(MainFinctionIcon[tDispIndex][2], DISP_ON);
				ituWidgetSetVisible(MainFinctionIcon[tDispIndex][0], DISP_ON);
			}
			else if (DispMode==DISP_ANIMATION) 
			{
				ituAnimationGoto(MainFinctionIcon[tDispIndex][0], 1);
				ituWidgetSetVisible(MainFinctionIcon[tDispIndex][1], DISP_ON);
				ituWidgetSetVisible(MainFinctionIcon[tDispIndex][2], DISP_ON);
				ituWidgetSetVisible(MainFinctionIcon[tDispIndex][0], DISP_ON);
				ituAnimationPlay(MainFinctionIcon[tDispIndex][0], 0);
			}			
			tMainFunctionData[tDispIndex]=DispMode;
		}
	}
//	printf("MainLayer_FunctionIcon End=IconIndex:%d,DispMode:%d,tIconIndex:%d\n",IconIndex,DispMode,tIconIndex);
    return true;
}

uint8_t MainLayer_WarningDisp(int IconIndex,DISP_MODE DispMode)
{
	MainLayer_WarningIcon( IconIndex, DispMode);
    return MainWarnigData[IconIndex];
}

uint8_t MainLayer_FunctionDisp(int IconIndex,DISP_MODE DispMode)
{
//	static uint8_t tMainFunctionData[FUNCTION_ICON_CNT]={0xFF,};
//	if (tMainFunctionData[IconIndex-cF_01_B]!=DispMode)
	{
//		printf("MainLayer_FunctionDisp End=IconIndex:%d,DispMode:%d,tIconIndex:%d\n",IconIndex,DispMode,tMainFunctionData[IconIndex-cF_01_B]);
//		if (MainLayerFlag)
		{
		    MainLayer_FunctionIcon(IconIndex,DispMode);
		}
//		tMainFunctionData[IconIndex-cF_01_B]=DispMode;
	}
    return MainFunctionData[IconIndex-cF_01_B];
}


bool MainLayer_ZoomDispW(int IconIndex,DISP_MODE DispMode)
{
	int i=0;
	static uint8_t tIconIndex=0;
	static uint8_t tDispMode=99;
//	printf("MainLayer_ZoomDisp=IconIndex:%d,DispMode:%d,tIconIndex:%d\n",IconIndex,DispMode,tIconIndex);
//return true;
	if (MainLayerFlag)
	{
		if ((tIconIndex!=IconIndex)||(tDispMode!=DispMode))
		{
//			printf("MainLayer_ZoomDisp=IconIndex:%d,DispMode:%d,tIconIndex:%d\n",IconIndex,DispMode,tIconIndex);
			if (DispMode==DISP_ON)
			{
				MainLayer_WarningIcon(IconIndex,DISP_ZOOM_ON);
			}		
			else if (DispMode==DISP_NORMAL_ON)
			{
				MainLayer_WarningIcon(IconIndex,DISP_NORMAL_ON);
			}
			else if (DispMode==DISP_ANIMATION)
			{
				MainLayer_WarningIcon(IconIndex,DISP_ANIMATION);
			}
			else if (DispMode==DISP_OFF)
			{
				MainLayer_WarningIcon(IconIndex,DISP_OFF);
			}
			tIconIndex=IconIndex;
			tDispMode=DispMode;			
		}
	}
//	printf("MainLayer_ZoomDisp=IconIndex:%d,DispMode:%d,tIconIndex:%d\n",IconIndex,DispMode,tIconIndex);
    return true;
}

bool MainLayer_ZoomDispF(int IconIndex,DISP_MODE DispMode)
{
	int i=0;
	static uint8_t tIconIndex=0;
	static uint8_t tDispMode=99;
//	printf("MainLayer_ZoomDisp=IconIndex:%d,DispMode:%d,tIconIndex:%d\n",IconIndex,DispMode,tIconIndex);
//return true;
	if (MainLayerFlag)
	{
//		if ((tIconIndex!=IconIndex)||(tDispMode!=DispMode))
		{
			if (DispMode==DISP_ON)
			{
//			printf("MainLayer_ZoomDisp=gMainTimerCnt_10m:[%d],DispMode:%d,tIconIndex:%d\n",gMainTimerCnt_10m,DispMode,tIconIndex);
				MainLayer_FunctionIcon(IconIndex,DISP_ZOOM_ON);
			}		
			else if (DispMode==DISP_NORMAL_ON)
			{
				MainLayer_FunctionIcon(IconIndex,DISP_NORMAL_ON);
			}
			else if (DispMode==DISP_ANIMATION)
			{
				MainLayer_FunctionIcon(IconIndex,DISP_ANIMATION);
			}
			else if (DispMode==DISP_OFF)
			{
				MainLayer_FunctionIcon(IconIndex,DISP_OFF);
			}
			tIconIndex=IconIndex;
			tDispMode=DispMode;			
		}
	}
//	printf("MainLayer_ZoomDisp=IconIndex:%d,DispMode:%d,tIconIndex:%d\n",IconIndex,DispMode,tIconIndex);
    return true;
}

bool MainOnEnter_1(ITUWidget* widget, char* param)
{
	int j,i=0,tIndex=0;

	printf("\nMainOnEnter_1 2024-05-16 :%d %d %d\n\n",RotationDir(),RotationMode,MainLayerFlag);
	printf("MainOnEnter Start=[%s][%s][%d][%d][%d]\n",widget,param,Main_Disp_Mode_Flag,Main_Disp_Mode,MainBtFlag);
	if (!MainLayerFlag)
	{
		SetWidget(Meter_BG,"Meter_BG");
		SetWidget(Function_BG,"Function_BG");
		SetWidget(Warning_BG,"Warning_BG");

		for (i=0;i<WARNING_ICON_CNT;i++)
		{
			sprintf(widgetName, "W_%02d_B",i+1);
//			printf("widgetName=%s\n",widgetName);
			SetWidget(MainWarnigIcon[i][0],widgetName);
			sprintf(widgetName, "W_%02d_N",i+1);
			SetWidget(MainWarnigIcon[i][1],widgetName);
			sprintf(widgetName, "W_%02d_Z",i+1);
			SetWidget(MainWarnigIcon[i][2],widgetName);
		}
		for (i=0;i<FUNCTION_ICON_CNT;i++)
		{
			sprintf(widgetName, "F_%02d_B",i+1);
//			printf("widgetName=%s\n",widgetName);
			SetWidget(MainFinctionIcon[i][0],widgetName);
			sprintf(widgetName, "F_%02d_N",i+1);
			SetWidget(MainFinctionIcon[i][1],widgetName);
			sprintf(widgetName, "F_%02d_Z",i+1);
			SetWidget(MainFinctionIcon[i][2],widgetName);
		}	
	
		SetWidget(EI_5B,"EI_5B");
		SetWidget(DD_1B,"DD_1B");
		SetWidget(SI_67B,"SI_67B");
		SetWidget(EI_34B,"EI_34B");


		SetWidget(Bar_EngRPM,"Bar_EngRPM");
		SetWidget(Bar_EngLd,"Bar_EngLd");
		SetWidget(Bar_LiftLd,"Bar_LiftLd");
		SetWidget(Bar_PM,"Bar_PM");
		SetWidget(Bar_ASH,"Bar_ASH");

		for (i=0;i<14;i++)
		{
			sprintf(widgetName, "Bar_Gauge%02d",i+1);
			SetWidget(Bar_Gauge[i],widgetName);
		}

		for (i=0;i<10;i++)
		{
			sprintf(widgetName, "DD_Gauge%02d",i+1);
			SetWidget(DD_Gauge[i],widgetName);
			sprintf(widgetName, "EI_Gauge%02d",i+1);
			SetWidget(EI_Gauge[i],widgetName);
		}
		
					
		for (i=0;i<3;i++)
		{
			for (j=0;j<10;j++)
			{
				sprintf(widgetName, "ATT_%02d_%d_B",j+1,i+1);
//				printf("widgetName=%s\n",widgetName);
				SetWidget(ATT_B[i][j],widgetName);

				sprintf(widgetName, "ATT_%02d_%d_S",j+1,i+1);
				SetWidget(ATT_S[i][j],widgetName);
			}
		}
		SetWidget(EngValue,"EngValue");

		SetWidget(EngStopSw_H,"EngStopSw_H");
		SetWidget(SafetyStart_H,"SafetyStart_H");
		
		SetWidget(MainCameraBt,"MainCameraBt");
		SetWidget(MainHomeBt,"MainHomeBt");
		SetWidget(MainMenuBt,"MainMenuBt");
		
		
		SetWidget(Main_ATT3_Bt,"Main_ATT3_Bt");
		SetWidget(Main_ATT2_Bt,"Main_ATT2_Bt");
		SetWidget(Main_ATT1_Bt,"Main_ATT1_Bt");
		SetWidget(Main_BarBt,"Main_BarBt");
		SetWidget(Main_HMeterBt,"Main_HMeterBt");
		SetWidget(Main_TimeDateBt,"Main_TimeDateBt");
		SetWidget(Main_RadioBt,"Main_RadioBt");
		SetWidget(Main_ACBt,"Main_ACBt");
		SetWidget(Main_SPK_Bt,"Main_SPK_Bt");
		SetWidget(Main_ATTBt,"Main_ATTBt");
		SetWidget(Main_WarringBt,"Main_WarringBt");
		SetWidget(Main_PwrBt,"Main_PwrBt");

		SetWidget(Bar_Sel,"Bar_Sel");
		SetWidget(ATT_Sel,"ATT_Sel");
		SetWidget(HourMtSel,"HourMtSel");
		SetWidget(Power_Sel,"Power_Sel");
		SetWidget(TimeDataSel,"TimeDataSel");
		SetWidget(MainCameraSel,"MainCameraSel");
		SetWidget(Main_SPK_Sel,"Main_SPK_Sel");
		SetWidget(Main_RadioSel,"Main_RadioSel");
		SetWidget(Main_ACSel,"Main_ACSel");
		SetWidget(MainMenuSel,"MainMenuSel");

		SetWidget(TimeDataTx,"TimeDataTx");

		SetWidget(HourMtTx,"HourMtTx");
		for (i=0;i<4;i++)
		{
			sprintf(widgetName, "HourMt%d",i+1);
			SetWidget(HourMt[i],widgetName);
		}
		SetWidget(BarGaugeBar,"BarGaugeBar");
		SetWidget(EI_5B_Graduation,"EI_5B_Graduation");
		SetWidget(SI_67B_Graduation,"SI_67B_Graduation");

		SetWidget(Main_Black_BG,"Main_Black_BG");
		SetWidget(Main_Blue_BG,"Main_Blue_BG");
		SetWidget(MainModeBG,"MainModeBG");
		SetWidget(SPK_OFF,"SPK_OFF");
		SetWidget(SPK_ON,"SPK_ON");
		SetWidget(Power_ON,"Power_ON");
		SetWidget(Power_OFF,"Power_OFF");
		
		SetWidget(MainButton,"MainButton");
				
		SetWidget(ATT_1_B,"ATT_1_B");
		SetWidget(ATT_2_B,"ATT_2_B");
		SetWidget(ATT_3_B,"ATT_3_B");
		
		SetWidget(MainNoCamera,"MainNoCamera");
		
	
		MainOnJD(widget, param);

		Main_tSymbol=0xFF;
		Main_MeterDisp(gMainTimerCnt_10m);
		ituWidgetSetVisible(MainNoCamera, DISP_OFF);		//10081
		ituWidgetSetVisible(MainCameraBt, DISP_ON);			//10081

		SetWidget(MainTestMsg,"MainTestMsg");
		SetWidget(MainTestMsg2,"MainTestMsg2");
		SetWidget(MainTestMsg3,"MainTestMsg3");
		#ifdef TEST_FLASH_MEM
			ituWidgetSetVisible(MainTestMsg, DISP_ON);
			ituWidgetSetVisible(MainTestMsg2, DISP_ON);
			ituWidgetSetVisible(MainTestMsg3, DISP_ON);
		#else
			ituWidgetSetVisible(MainTestMsg, DISP_OFF);
			ituWidgetSetVisible(MainTestMsg2, DISP_OFF);
			ituWidgetSetVisible(MainTestMsg3, DISP_OFF);
		#endif
		
		
		InitVariableData();
		
		MainLayerFlag = 1;
		printf("\nMainOnEnter MainLayerFlag : %d   MainBtFlag : %d  %d\n\n",MainLayerFlag,MainBtFlag,RotationMode);
		MainMode_ACDisp(0,DISP_OFF);
		MainMode_RD_Init();
		MainMode_RDDisp(0,DISP_OFF);
		
		#ifndef TEST_CAM_COLOR
			{
				CameraOnEnter();
			}
		#endif
	}

#ifdef TEST_ERR_602
	SetWidget(TestMSG1,"TestMSG1");
	ituWidgetSetVisible(TestMSG1, DISP_ON);
#else
	SetWidget(TestMSG1,"TestMSG1");
	ituWidgetSetVisible(TestMSG1, DISP_OFF);
#endif

	MainLayerFlag4ZommIcon = 1;

	TestMode_InitControl();//chpark.24.04.24
	MC100_InitControl();//chpark.24.05.13

	//============================
	//chpark.24.07.02
	if(g_nFac_TestModeFlag == 1)
	{
		GotoFacMode(g_nTest_Step);
	}
	//============================

	printf("MainOnEnter End=[%s][%s][%d][%d][%d]\n",widget,param,Main_Disp_Mode_Flag,Main_Disp_Mode,MainBtFlag);
    return true;
}

bool MainButton_OnOff (uint8_t OnOff)
{
	if (OnOff==DISP_ON)
	{
		ituWidgetSetVisible(MainButton, DISP_ON);
	}
	else
	{
		ituWidgetSetVisible(MainButton, DISP_OFF);
	}
}

bool MainLayer_Power_Icon_Check(bool OffFlag)
{
	uint8_t tData;
	if (!MainLayerFlag)
		return true;

	if ((AUI1_0B_3210b==3)||(OffFlag==1))
	{
		ituWidgetSetVisible(Main_PwrBt, DISP_OFF);
		ituWidgetSetVisible(Power_ON, DISP_OFF);
		ituWidgetSetVisible(Power_OFF, DISP_OFF);
		PowerBtFlag = 0x00;
		AUI1_0B_3210b=3;
//		printf("MainLayer_Power_Icon_Check[%d][%d][%d][%d][%d]\r\n",Main_Disp_Mode,Main_Select_Index,MainMode_RD_Select_Index,MainMode_AC_Select_Index,MainMode_DAB_Select_Index);

		if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
		{
			tData = MAIN_SEL_BAR;
		}
		else
		{
			tData = MAIN_SEL_HOURMETER;
		}
	
	
		switch (Main_Disp_Mode)
		{
			case MAIN_MODE_RADIO:	// Radio
				if(MainMode_RD_Select_Index == RD_SEL_POWER)
				{
					MainMode_RD_Select_Index = tData;
					MainLayer_ButtonSel(MainMode_RD_Select_Index,DISP_ON);
				}
			break;
			case MAIN_MODE_AIRCON:	// Aircon
				if(MainMode_AC_Select_Index == AC_SEL_POWER)
				{
					MainMode_AC_Select_Index = tData;
					MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				}
			break;	
			case MAIN_MODE_DAB1:
				if(MainMode_DAB_Select_Index == RD_SEL_POWER)
				{
					MainMode_DAB_Select_Index = tData;
					MainLayer_ButtonSel(MainMode_DAB_Select_Index,DISP_ON);
				}
			break;		
			case MAIN_MODE_DAB2:
			case MAIN_MODE_DAB3:
				if(MainMode_DAB2_Select_Index == RD_SEL_POWER)
				{
					MainMode_DAB2_Select_Index = tData;
					MainLayer_ButtonSel(MainMode_DAB2_Select_Index,DISP_ON);
				}
			break;		
			case MAIN_MODE_BLUETOOTH:	//
				if(MainMode_BT_Select_Index == RD_SEL_POWER)
				{
					MainMode_BT_Select_Index = tData;
					MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				}
			break;		
			case MAIN_MODE_PHONE:	// Phone
				if(MainMode_Phone_Select_Index == RD_SEL_POWER)
				{
					MainMode_Phone_Select_Index = tData;
					MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				}
			break;
			default:
			{
				if(Main_Select_Index == MAIN_SEL_POWER)
				{
					Main_Select_Index = tData;
					MainLayer_ButtonSel(Main_Select_Index,DISP_ON);
				}
			}
			break;
		}		
	}
	else
	{
		ituWidgetSetVisible(Main_PwrBt, DISP_ON);
		ituWidgetSetVisible(Power_ON, DISP_ON);
		ituWidgetSetVisible(Power_OFF, DISP_OFF);
		PowerBtFlag = 0x01;
	}
	return true;
}

bool MainLayer_Button(DISP_MODE DispMode)
{
#ifdef TEST_CAM_COLOR
	ituWidgetSetVisible(MainCameraBt, DISP_OFF);
	ituWidgetSetVisible(MainHomeBt, DISP_OFF);
	ituWidgetSetVisible(MainMenuBt, DISP_OFF);

	ituWidgetSetVisible(Main_ATT3_Bt, DISP_OFF);
	ituWidgetSetVisible(Main_ATT2_Bt, DISP_OFF);
	ituWidgetSetVisible(Main_ATT1_Bt, DISP_OFF);
	ituWidgetSetVisible(Main_BarBt, DISP_OFF);
	ituWidgetSetVisible(Main_HMeterBt, DISP_OFF);
	ituWidgetSetVisible(Main_TimeDateBt, DISP_OFF);
	ituWidgetSetVisible(Main_RadioBt, DISP_OFF);
	ituWidgetSetVisible(Main_ACBt, DISP_OFF);
	ituWidgetSetVisible(Main_ATTBt, DISP_OFF);
	ituWidgetSetVisible(Main_WarringBt, DISP_OFF);
	ituWidgetSetVisible(Main_SPK_Bt, DISP_OFF);
	ituWidgetSetVisible(Main_PwrBt, DISP_OFF);
#endif	
	return true;
}

bool MainLayer_BGDisp(uint8_t BG_DN,uint8_t BGColor,uint8_t MainMode)
{
	static uint8_t tBG_DN=0xFF;
	static uint8_t tBGColor=0xFF;
	static uint8_t tMainMode=0xFF;
	static uint8_t tgCameraModeDisp=0xFF;
	// 0:Gray , 1:BlueGray , 2:Blue , 3 : Black
	
//	if ((tBG_DN!=BG_DN)||(tBGColor!=BGColor)||(tMainMode!=MainMode)||(tgCameraModeDisp!=gCameraModeDisp))
	{
		printf("MainLayer_BGDisp=%d %d %d %d %d %d\n",BG_DN,BGColor,MainMode,Main_Disp_Mode,gCameraModeDisp,RotationDir());
		Main_TempRpm = 0x01;
		Main_tSymbol=0xFF;
		Main_EI_34B = 0xFFFF;
		Main_DD_1B = 0xFFFF;
		Main_EI_5B = 0xFFFF;
		Main_SI_67B = 0xFFFF;
		ituWidgetSetVisible(Main_Black_BG, DISP_OFF);
		
		ituWidgetSetVisible(Main_Blue_BG, DISP_OFF);

		if (BG_DN==0)
			LCD_BackLight(0);	// 0:Day , 1:Night
		else
			LCD_BackLight(1);

		switch (MainMode)
		{
			case MAIN_MODE_USERINFO:
			case MAIN_MODE_NORMAL:
			{
				#ifndef TEST_CAM_COLOR			
				ituWidgetSetVisible(MainHomeBt, DISP_OFF);
				ituWidgetSetVisible(MainMenuBt, DISP_ON);
				#endif
				MainLayer_Power_Icon_Check(((UserPW_Time_AUI1+TIME_CNT_3SEC)<gMainTimerCnt_10m));
				if (RDV.RDMute==1)
				{
					ituWidgetSetVisible(SPK_ON, DISP_OFF);
					ituWidgetSetVisible(SPK_OFF, DISP_ON);
				}
				else
				{
					ituWidgetSetVisible(SPK_ON, DISP_ON);
					ituWidgetSetVisible(SPK_OFF, DISP_OFF);
				}	
				if ((BGColor==BG_BLUE)&&(BG_DN==0))
				{
					printf("MAIN_MODE_NORMAL = Blue_BG 2 \n");
					ituWidgetSetVisible(Main_Blue_BG, DISP_ON);
				}
				else
				{
					printf("MAIN_MODE_NORMAL = Black_BG \n");
					ituWidgetSetVisible(Main_Black_BG, DISP_ON);
				}
				ituWidgetSetVisible(Meter_BG, DISP_ON);
				ituWidgetSetVisible(MainModeBG, DISP_ON);				
			}
			break;
			case MAIN_MODE_AIRCON:
			{
				ituWidgetSetVisible(MainHomeBt, DISP_ON);
				ituWidgetSetVisible(MainMenuBt, DISP_OFF);
				MainLayer_Power_Icon_Check(((UserPW_Time_AUI1+TIME_CNT_3SEC)<gMainTimerCnt_10m));
				if (RDV.RDMute==1)
				{
					ituWidgetSetVisible(SPK_ON, DISP_OFF);
					ituWidgetSetVisible(SPK_OFF, DISP_ON);
				}
				else
				{
					ituWidgetSetVisible(SPK_ON, DISP_ON);
					ituWidgetSetVisible(SPK_OFF, DISP_OFF);
				}
				if ((BGColor==BG_BLUE)&&(BG_DN==0))
				{
					printf("MAIN_MODE_AIRCON = Blue_BG \n");
					ituWidgetSetVisible(Main_Blue_BG, DISP_ON);
				}
				else
				{
					printf("MAIN_MODE_AIRCON = Black_BG \n");
					ituWidgetSetVisible(Main_Black_BG, DISP_ON);
				}
				ituWidgetSetVisible(Meter_BG, DISP_ON);
				ituWidgetSetVisible(MainModeBG, DISP_ON);				
			}
			break;
			case MAIN_MODE_RADIO:
			case MAIN_MODE_DAB3:
			case MAIN_MODE_DAB2:
			case MAIN_MODE_DAB1:
			case MAIN_MODE_BLUETOOTH:
			case MAIN_MODE_PHONE:
			{
				ituWidgetSetVisible(MainHomeBt, DISP_ON);
				ituWidgetSetVisible(MainMenuBt, DISP_OFF);
				MainLayer_Power_Icon_Check(((UserPW_Time_AUI1+TIME_CNT_3SEC)<gMainTimerCnt_10m));
				if (RDV.RDMute==1)
				{
					ituWidgetSetVisible(SPK_ON, DISP_OFF);
					ituWidgetSetVisible(SPK_OFF, DISP_ON);
				}
				else
				{
					ituWidgetSetVisible(SPK_ON, DISP_ON);
					ituWidgetSetVisible(SPK_OFF, DISP_OFF);
				}
				if ((BGColor==BG_BLUE)&&(BG_DN==0))
				{
					printf("MAIN_MODE_RADIO = Blue_BG \n");
					ituWidgetSetVisible(Main_Blue_BG, DISP_ON);
				}
				else
				{
					printf("MAIN_MODE_RADIO = Black_BG \n");
					ituWidgetSetVisible(Main_Black_BG, DISP_ON);
				}
				ituWidgetSetVisible(Meter_BG, DISP_ON);
				ituWidgetSetVisible(MainModeBG, DISP_ON);				
			}
			break;	
		}
		tBG_DN=BG_DN;
		tBGColor=BGColor;
		tMainMode=MainMode;
		tgCameraModeDisp=gCameraModeDisp;
	}
}
bool MainLayer_BG_BL(uint8_t BG_DN,uint8_t BGColor)
{
	static uint8_t tBG_DN=0xFF;
	static uint8_t tBGColor=0xFF;
	
	if ((tBG_DN!=BG_DN)||(tBGColor!=BGColor))
	{
		if (BG_DN==0)
			LCD_BackLight(0);	// 0:Day , 1:Night
		else
			LCD_BackLight(1);

		switch (Main_Disp_Mode)
		{
			case MAIN_MODE_NORMAL:
			{
				if ((BGColor==BG_BLUE)&&(BG_DN==0))
				{
					printf("MainLayer_BG_BL = Blue_BG 2 \n");
					ituWidgetSetVisible(Main_Blue_BG, DISP_ON);
					ituWidgetSetVisible(Main_Black_BG, DISP_OFF);
				}
				else
				{
					printf("MainLayer_BG_BL = Black_BG \n");
					ituWidgetSetVisible(Main_Blue_BG, DISP_OFF);
					ituWidgetSetVisible(Main_Black_BG, DISP_ON);
				}
				ituWidgetSetVisible(Meter_BG, DISP_ON);
				ituWidgetSetVisible(MainModeBG, DISP_ON);				
			}
			break;
			case MAIN_MODE_AIRCON:
			{
				if ((BGColor==BG_BLUE)&&(BG_DN==0))
				{
					printf("BG_BL AIRCON = Blue_BG1 \n");
					ituWidgetSetVisible(Main_Blue_BG, DISP_ON);
					ituWidgetSetVisible(Main_Black_BG, DISP_OFF);
				}
				else
				{
					printf("BG_BL AIRCON = Black_BG2 \n");
					ituWidgetSetVisible(Main_Black_BG, DISP_ON);
					ituWidgetSetVisible(Main_Blue_BG, DISP_OFF);
				}
				MainLayer_BG_BL_SUB(Main_Disp_Mode);
				ituWidgetSetVisible(Meter_BG, DISP_ON);
				ituWidgetSetVisible(MainModeBG, DISP_ON);				
			}
			break;
			case MAIN_MODE_RADIO:
			case MAIN_MODE_DAB3:
			case MAIN_MODE_DAB2:
			case MAIN_MODE_DAB1:
			case MAIN_MODE_BLUETOOTH:
			case MAIN_MODE_PHONE:
			{
				if ((BGColor==BG_BLUE)&&(BG_DN==0))
				{
					printf("MAIN_MODE_RADIO = Blue_BG \n");
					ituWidgetSetVisible(Main_Blue_BG, DISP_ON);
					ituWidgetSetVisible(Main_Black_BG, DISP_OFF);
				}
				else
				{
					printf("MAIN_MODE_RADIO = Black_BG \n");
					ituWidgetSetVisible(Main_Black_BG, DISP_ON);
					ituWidgetSetVisible(Main_Blue_BG, DISP_OFF);
				}
				MainLayer_BG_BL_SUB(Main_Disp_Mode);
				ituWidgetSetVisible(Meter_BG, DISP_ON);
				ituWidgetSetVisible(MainModeBG, DISP_ON);				
			}
			break;		
		}
		tBG_DN=BG_DN;
		tBGColor=BGColor;
	}
}

bool MainBtExe(void)
{
	char JD_Buf[10] = {0,};
	static uint8_t tRDV_RDMute = 0xFF;
	static uint8_t tAUI1_0B_3210b = 0xFF;
//	printf("[MainBtExe] '%d','%d','%d'\n",MainBtFlag,Main_Disp_Mode,CLM2_7Byte_5bit);

	if (MainBtFlag)
	{
		tRDV_RDMute = 0xFF;
		if (Main_Disp_Mode_Flag==1)
		{
//			printf("\n[MainBtExe]Main_Disp_Mode_Flag===> : %d %d %d %d\n\n",Main_Disp_Mode,MainBtFlag,Main_Disp_Mode_Flag,RotationMode);
//			return 0;
		}
	}

	if (RDV.RDMute!=tRDV_RDMute)	 
	{
		if (RDV.RDMute==1)
		{
			ituWidgetSetVisible(SPK_ON, DISP_OFF);
			ituWidgetSetVisible(SPK_OFF, DISP_ON);
		}
		else
		{
			ituWidgetSetVisible(SPK_ON, DISP_ON);
			ituWidgetSetVisible(SPK_OFF, DISP_OFF);
		}
		tRDV_RDMute = RDV.RDMute;
	}
	
	if ((tAUI1_0B_3210b!=AUI1_0B_3210b)||((UserPW_Time_AUI1+TIME_CNT_3SEC)<gMainTimerCnt_10m))
	{
		MainLayer_Power_Icon_Check(((UserPW_Time_AUI1+TIME_CNT_3SEC)<gMainTimerCnt_10m));
		tAUI1_0B_3210b=AUI1_0B_3210b;
	}
				
					
	switch (MainBtFlag)
	{
		case MAIN_MODE_NORMAL:
		{
//			if(CameraOnLeaveCallSrc(MAIN_MODE_NORMAL)==1)
			{
				MainBtFlag = 0;
				Main_Disp_Mode = MAIN_MODE_NORMAL;
				if (RDV.RDPhone==5)
				{
					RDV.RDPhone = 0;
					Phone_Time=PHONE_TIME;
				}
				MainLayer_BGDisp(Main_DayNight,BG_Color,Main_Disp_Mode);	// 0:Gray , 1:BlueGray , 2:Blue , 3 : Black
				EnterMainMode(Main_Disp_Mode);
				ScreenSetOnOff(1);
				EnterCameraBt(DISP_ON);
				ituWidgetSetVisible(MainCameraBt, DISP_ON);
			}
		}
		break; 	
		case MAIN_MODE_AIRCON:
		{
//			if(CameraOnLeaveCallSrc(MAIN_MODE_AIRCON)==1)
			{
				MainBtFlag = 0;
				Main_Disp_Mode = MAIN_MODE_AIRCON;
				if (RDV.RDPhone==5)
				{
					RDV.RDPhone = 0;
					Phone_Time=PHONE_TIME;
				}
				MainLayer_BGDisp(Main_DayNight,BG_Color,Main_Disp_Mode);	// 0:Gray , 1:BlueGray , 2:Blue , 3 : Black
				EnterMainMode(Main_Disp_Mode);
				Media_RunTime_Cnt = gMainTimerCnt_10m;
				ScreenSetOnOff(1);
				EnterCameraBt(DISP_ON);
				ituWidgetSetVisible(MainCameraBt, DISP_ON);
			}
		}
		break;
		case MAIN_MODE_RADIO:
		{
//			if(CameraOnLeaveCallSrc(MAIN_MODE_RADIO)==1)
			{
				printf("[MainBtExe] '%d','%d','%d'\n",MainBtFlag,Main_Disp_Mode,CLM2_7Byte_5bit);
				MainBtFlag = 0;
				if ((RDV.RDPhone==1)||(RDV.RDPhone==2)||(RDV.RDPhone==3)||(RDV.RDPhone==4))
				{
					// Phone
					Main_Disp_Mode = MAIN_MODE_PHONE;
					MainLayer_BGDisp(Main_DayNight,BG_Color,Main_Disp_Mode);	// 0:Gray , 1:BlueGray , 2:Blue , 3 : Black
					EnterMainMode(Main_Disp_Mode);				
				}
				else
				{
					switch (RDV.RDMode)
					{
						case 0:
						case 1:
						{
							//RADIO
							Main_Disp_Mode = MAIN_MODE_RADIO;
							MainLayer_BGDisp(Main_DayNight,BG_Color,Main_Disp_Mode);	// 0:Gray , 1:BlueGray , 2:Blue , 3 : Black
							EnterMainMode(Main_Disp_Mode);
						}
						break;
						case 2:
						{
							//BT
							Main_Disp_Mode = MAIN_MODE_BLUETOOTH;
							MainLayer_BGDisp(Main_DayNight,BG_Color,Main_Disp_Mode);	// 0:Gray , 1:BlueGray , 2:Blue , 3 : Black
							EnterMainMode(Main_Disp_Mode);
						}
						break;
						case 3:
						case 4:
						{
							//DAB
							if ((Main_Disp_Mode != MAIN_MODE_DAB2)&&(Main_Disp_Mode != MAIN_MODE_DAB3))  // JPHong 2023-11-11
								Main_Disp_Mode = MAIN_MODE_DAB1;
							MainLayer_BGDisp(Main_DayNight,BG_Color,Main_Disp_Mode);	// 0:Gray , 1:BlueGray , 2:Blue , 3 : Black
							EnterMainMode(Main_Disp_Mode);
						}
						break;
					}
				}
				Media_RunTime_Cnt = gMainTimerCnt_10m;
				ScreenSetOnOff(1);
				EnterCameraBt(DISP_ON);
				ituWidgetSetVisible(MainCameraBt, DISP_ON);
			}
		}
		break;
		case MAIN_MODE_MENU_ATT:
		{
//			if(CameraOnLeaveCallSrc(33)==1)
			{
				Main_Disp_Mode = MAIN_MODE_MENU;
				MainBtFlag = 0;
				printf("Goto AUX1 Set\n");
				GotoM7S1(AUX1CurrentOutMode);
			}
		}
		break;
		case MAIN_MODE_MENU_TIME:
		{
//			if(CameraOnLeaveCallSrc(34)==1)
			{
				Main_Disp_Mode = MAIN_MODE_MENU;
				MainBtFlag = 0;
				printf("Goto TimeDate\n");
				GotoMenu6();
			}
		}
		break;
		case MAIN_MODE_MENU_WARRING:
		{
//			if(CameraOnLeaveCallSrc(35)==1)
			{
				Main_Disp_Mode = MAIN_MODE_MENU;
				MainBtFlag = 0;
				if (RDV.RDPhone==5)
				{
					RDV.RDPhone = 0;
					Phone_Time=PHONE_TIME;
				}
				GotoMenu8_Error();
				EnterCameraBt(DISP_ON);
			}
		}
		break;
		case MAIN_MODE_USERINFO:
		{
			MainBtFlag = 0;
		}
		break;
		case 0:
		{
		}
		break;
		default :
			printf("MainBtExe Error[%d]\n",MainBtFlag);
			MainBtFlag = 0;
		break;
	}
	return true;
}

bool CheckKeyEvent(void)
{
	char JD_Buf[10] = {0,};
	switch (Main_Disp_Mode)
	{
		case MAIN_MODE_NORMAL:
		{
		}
		break;
		case MAIN_MODE_AIRCON:
		{
			if ((Media_RunTime_Cnt+TIME_CNT_30SEC)<gMainTimerCnt_10m)
			{
				JD_Buf[0] = 0;
				MainHome(NULL, JD_Buf);
			}
		}
		break;
		case MAIN_MODE_RADIO:
		case MAIN_MODE_DAB3:
		case MAIN_MODE_DAB2:
		case MAIN_MODE_DAB1:
		case MAIN_MODE_BLUETOOTH:
//		case MAIN_MODE_PHONE:
		{
			if ((Media_RunTime_Cnt+TIME_CNT_30SEC)<gMainTimerCnt_10m)
			{
				printf("======================30sec exceed\n");
				JD_Buf[0] = 0;
				MainHome(NULL, JD_Buf);
			}
		}
		break;
	}
	return true;
}

bool Main_FunctionCanClear(void);

bool MainTimerIcon(void)
{
	#ifdef USED_ROTATION
	if (RotationMode!=0)
	{	
		return 0;
	}
	#endif
	if (MainLayerFlag)
	{

	//	if (Main_wZoomIndex<WARNING_ICON_CNT)	// Zoomï¿½ï¿½ ï¿½Ö´ï¿½
		{
			Main_ZoomDispW(gMainTimerCnt_10m,&wZoomFlag);
		}
		
		{
			Main_ZoomDispF(gMainTimerCnt_10m,&fZoomFlag);
		}
		
	
		MainBtExe();
		Main_FunctionCanClear();
		Main_WarningDisp(gMainTimerCnt_10m,0);
		Main_FunctionDisp(gMainTimerCnt_10m,0);
		CheckKeyEvent();
		CameraCheckCPI();
		{
			if (Main_Disp_Mode == MAIN_MODE_AIRCON)
				MainMode_ACDisp(0,DISP_ON);
//			printf("           MainTimerIcon= %d\n",Main_Disp_Mode);	
			if ((Main_Disp_Mode == MAIN_MODE_RADIO)||(Main_Disp_Mode == MAIN_MODE_DAB1)||(Main_Disp_Mode == MAIN_MODE_DAB2)||(Main_Disp_Mode == MAIN_MODE_DAB3)||(Main_Disp_Mode == MAIN_MODE_BLUETOOTH)||(Main_Disp_Mode == MAIN_MODE_PHONE))
			{
				//printf("=======MainMode_RDDisp Before(1)=========\n");
				MainMode_RDDisp(0,DISP_ON);
				if (MainMode_Bluetooth_Pairing_Flag)
					MainMode_Bluetooth_PairDisp(gOnFlag_300ms , DISP_ON);
					
				if (RDV.RDPhone==5)
				{
					if ((Phone_Time+TIME_CNT_15SEC)<gMainTimerCnt_10m)
					{
						char JD_Buf[10] = {0,};
						RDV.RDPhone=0;
						Phone_Time=PHONE_TIME;
						JD_Buf[0] = 0;
						MainHome(NULL, JD_Buf);
					}
				}					
			}
		}

		Main_EI_5BMeterDisp(Main_EI_5BValue,gMainTimerCnt_10m);
		Main_DD_1BMeterDisp(Main_DD_1BValue,gMainTimerCnt_10m);
		Main_SI_67BMeterDisp(Main_SI_67BValue,gMainTimerCnt_10m);
		Main_EI_34BMeterDisp(Main_EI_34BValue,gMainTimerCnt_10m);
		Main_MeterDisp(gMainTimerCnt_10m);
		Main_HourMtDisp(DISP_ON);
	}
	return true;
}



bool Main_FunctionCanClear(void)
{
	bool Flag = 0;


	MainLayer_BG_BL(Main_DayNight,BG_Color);
	if ((Main_Disp_Mode!=MAIN_MODE_PW)&&(Main_Disp_Mode!=MAIN_MODE_USERINFO))
	{	
		if ((MI_Time+TIME_CNT_5SEC)<gMainTimerCnt_10m)
		{
			gBuzzerMode = 8;	//0:OFF , 1:ON(100)OFF(1900) , 2:ON(200)OFF(800) , 3:ON(500)OFF(500) , 4:ON(300)OFF(300)  , 5:ON , 6:ON(100)OFF(9900)  , 7:ON(100)OFF(100) 3Time   , 8:ON(300)OFF(300 MI) , 10:ON(500)OFF
		}
		else if (gBuzzerMode == 8)
		{
			gBuzzerMode = 0;
		}
	}
	else
	{
		gBuzzerMode = 0;
	}
	return Flag;
}

void EnterCameraBt(uint8_t OnOff)
{
    MainDispDate();
    ituWidgetSetVisible(MainCameraBt, OnOff);
}


#ifdef TEST_FLASH_MEM
	extern uint32_t sf_CNT;
	extern uint32_t XSC_FlashAddr;
	uint32_t Flash_Save_Cnt = 0;
	uint32_t Flash_Save_Start1 = 0;
	uint32_t Flash_Save_End1 = 0;
	uint32_t Flash_Save_sf_CNT1 = 0;
	uint32_t Flash_Save_Start2 = 0;
	uint32_t Flash_Save_End2 = 0;
	uint32_t Flash_Save_sf_CNT2 = 0;
	uint16_t Flash_Status = 0;
	bool Flash_Save_Cnt_Disp(void)
	{
		char tmp[32]={0,};
		sprintf(tmp, "[%d] [%07X] [%d] [%d]",Flash_Save_Cnt,XSC_FlashAddr,sf_CNT,Flash_Status);
		ituTextSetString(MainTestMsg, tmp);
		sprintf(tmp, "[%06X] [%06X] [%d] ",Flash_Save_Start1,Flash_Save_End1,Flash_Save_sf_CNT1);
		ituTextSetString(MainTestMsg2, tmp);
		sprintf(tmp, "[%06X] [%06X] [%d] ",Flash_Save_Start2,Flash_Save_End2,Flash_Save_sf_CNT2);
		ituTextSetString(MainTestMsg3, tmp);
	}	
	
#endif //TEST_FLASH_MEM

