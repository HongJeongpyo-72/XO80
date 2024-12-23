#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

/* widgets:
Menu8_Layer
M8_TopUserName
M8_TopStandard
M8_TopAdmin
M8_D7_BTN
M8_D6_BTN
M8_D5_BTN
M8_EngPastCode_BG
PastEng_PS2_BTN4
PastEng_PS2_BTN3
PastEng_PS2_BTN2
PastEng_PS2_BTN1
PastEng_PS1_BTN4
PastEng_PS1_BTN3
PastEng_PS1_BTN2
PastEng_PS1_BTN1
PastEng_Title
PastEng_Name4
PastEng_Name3
PastEng_Name2
PastEng_Name1
PastEng_Standard4
PastEng_Standard3
PastEng_Standard2
PastEng_Standard1
PastEng_Admin4
PastEng_Admin3
PastEng_Admin2
PastEng_Admin1
PastEngMeter4
PastEngMeter3
PastEngMeter2
PastEngMeter1
PastEngErrCodeDetail
PastEngErrCode
PastEngFMI4_Value
PastEngFMI3_Value
PastEngFMI2_Value
PastEngFMI1_Value
PastEngSPN4_Value
PastEngSPN3_Value
PastEngSPN2_Value
PastEngSPN1_Value
PastEngErr4_BTN
PastEngErr3_BTN
PastEngErr2_BTN
PastEngErr1_BTN
M8_CarPastCode_BG
PastCar_PS2_BTN5
PastCar_PS2_BTN4
PastCar_PS2_BTN3
PastCar_PS2_BTN2
PastCar_PS2_BTN1
PastCar_PS1_BTN5
PastCar_PS1_BTN4
PastCar_PS1_BTN3
PastCar_PS1_BTN2
PastCar_PS1_BTN1
PastCar_Name5
PastCar_Name4
PastCar_Name3
PastCar_Name2
PastCar_Name1
PastCar_Title
PastCarStandard5
PastCarStandard4
PastCarStandard3
PastCarStandard2
PastCarStandard1
PastCarAdmin5
PastCarAdmin4
PastCarAdmin3
PastCarAdmin2
PastCarAdmin1
PastCarErrCodeDetail
PastCarErrCode
PastCarTrim5_Value
PastCarTrim4_Value
PastCarTrim3_Value
PastCarTrim2_Value
PastCarTrim1_Value
PastCarErr5_Value
PastCarErr4_Value
PastCarErr3_Value
PastCarErr2_Value
PastCarErr1_Value
PastCarErr5_BTN
PastCarErr4_BTN
PastCarErr3_BTN
PastCarErr2_BTN
PastCarErr1_BTN
M8_AirCode_BG
ACErrCodeDetail
ACErrCode
ACErr8_Value
ACErr7_Value
ACErr6_Value
ACErr5_Value
ACErr4_Value
ACErr3_Value
ACErr2_Value
ACErr1_Value
ACErr8_BTN
ACErr7_BTN
ACErr6_BTN
ACErr5_BTN
ACErr4_BTN
ACErr3_BTN
ACErr2_BTN
ACErr1_BTN
M8_EngCode_BG
EngErrCodeDetail
EngErrCode
EngFMI3_Value
EngFMI2_Value
EngFMI1_Value
EngSPN3_Value
EngSPN2_Value
EngSPN1_Value
EngErr3_BTN
EngErr2_BTN
EngErr1_BTN
CarErr4_Value
CarErr3_Value
CarErr2_Value
CarErr1_Value
CarErr4_BTN
CarErr3_BTN
CarErr2_BTN
CarErr1_BTN
*/



#include "Error_Code.h"

#include "ACErrorMSG_V.h"
#include "ACErrorMSG_H.h"

#include "CarErrorMSG_ENG_V.h"
#include "CarErrorMSG_DEU_V.h"
#include "CarErrorMSG_FRA_V.h"
#include "CarErrorMSG_ITA_V.h"
#include "CarErrorMSG_ESP_V.h"

#include "CarErrorMSG_ENG_H.h"
#include "CarErrorMSG_DEU_H.h"
#include "CarErrorMSG_FRA_H.h"
#include "CarErrorMSG_ITA_H.h"
#include "CarErrorMSG_ESP_H.h"

#include "EngErrorMSG_ENG_V.h"
#include "EngErrorMSG_DEU_V.h"
#include "EngErrorMSG_FRA_V.h"
#include "EngErrorMSG_ITA_V.h"
#include "EngErrorMSG_ESP_V.h"

#include "EngErrorMSG_ENG_H.h"
#include "EngErrorMSG_DEU_H.h"
#include "EngErrorMSG_FRA_H.h"
#include "EngErrorMSG_ITA_H.h"
#include "EngErrorMSG_ESP_H.h"



static ITUWidget* PastCar_TITLE_CONT; //chpark.24.04.11
static ITUWidget* PastCar_TITLE[5];   //chpark.24.04.11


// EngErr
static ITUText* CarErr1_Value;
static ITUText* CarErr2_Value;
static ITUText* CarErr3_Value;
static ITUText* CarErr4_Value;

static ITUText* EngSPN1_Value;
static ITUText* EngSPN2_Value;
static ITUText* EngSPN3_Value;

static ITUText* EngFMI1_Value;
static ITUText* EngFMI2_Value;
static ITUText* EngFMI3_Value;

static ITUText* EngErrCode;
static ITUTextBox* EngErrCodeDetail;

// ACErr
static ITUText* ACErr1_Value;
static ITUText* ACErr2_Value;
static ITUText* ACErr3_Value;
static ITUText* ACErr4_Value;
static ITUText* ACErr5_Value;
static ITUText* ACErr6_Value;
static ITUText* ACErr7_Value;
static ITUText* ACErr8_Value;

static ITUText* ACErrCode;
static ITUTextBox* ACErrCodeDetail;

// PastCar
static ITUText* PastCarErr1_Value;
static ITUText* PastCarErr2_Value;
static ITUText* PastCarErr3_Value;
static ITUText* PastCarErr4_Value;
static ITUText* PastCarErr5_Value;

static ITUText* PastCarTrim1_Value;
static ITUText* PastCarTrim2_Value;
static ITUText* PastCarTrim3_Value;
static ITUText* PastCarTrim4_Value;
static ITUText* PastCarTrim5_Value;

static ITUText *PastCar_Name1;
static ITUText *PastCar_Name2;
static ITUText *PastCar_Name3;
static ITUText *PastCar_Name4;
static ITUText *PastCar_Name5;

static ITUText* PastCarErrCode;
static ITUTextBox* PastCarErrCodeDetail;

// PastEng
static ITUText* PastEngSPN1_Value;
static ITUText* PastEngSPN2_Value;
static ITUText* PastEngSPN3_Value;
static ITUText* PastEngSPN4_Value;

static ITUText* PastEngFMI1_Value;
static ITUText* PastEngFMI2_Value;
static ITUText* PastEngFMI3_Value;
static ITUText* PastEngFMI4_Value;

static ITUText *PastEngMeter1;
static ITUText *PastEngMeter2;
static ITUText *PastEngMeter3;
static ITUText *PastEngMeter4;

static ITUText *PastEng_Name1;
static ITUText *PastEng_Name2;
static ITUText *PastEng_Name3;
static ITUText *PastEng_Name4;

static ITUText* PastEngErrCode;
static ITUTextBox* PastEngErrCodeDetail;

#define EngineErrorCode		0
#define AirConErrorCode		1
#define PastCarCode				2
#define PastEngineCode		3

#define SumString_Size		300

static uint8_t cPage_Index = 0;
static uint8_t cError_Index = 0;

// car error
uint16_t gEMR[4]={0,0,0,0};			// for Error Display , DMM

// engine error
uint32_t gSPN_sECU = 0;					// single ECU ErrorCode
uint8_t gFMI_sECU = 0;
uint32_t gSPN_sDCU = 0;					// single DCU ErrorCode
uint8_t gFMI_sDCU = 0;
uint32_t gSPN_ECU[3]={0,0,0};		// Multi ECU ErrorCode
uint8_t gFMI_ECU[3]={0,0,0};
uint32_t gSPN_DCU[3]={0,0,0};		// Multi DCU ErrorCode	
uint8_t gFMI_DCU[3]={0,0,0};

uint32_t gSPN[3]={0,0,0};				// for Error Display
uint8_t gFMI[3]={0,0,0};				// for Error Display

// AC error
uint8_t gACERR_Code[8]={0,0,0,0,0,0,0,0};

// past car error
uint32_t gEMR_Time[5]={0,0,0,0,0};	// 0.0 ~ 99999.9hr (0.1hr/bit)CAN[P_DTCU] 0x18503FE4
uint16_t gEMR_Past[5]={0,0,0,0,0};	//
uint8_t gPastCar_UserIdx[5]={0,0,0,0,0};

// past engine error
uint32_t gSPN_sPast = 0;					// single Past Eng ErrorCode
uint8_t gFMI_sPast = 0;
uint32_t gSPN_Past[4]={0,0,0,0};		// Multi Past Eng ErrorCode
uint8_t gFMI_Past[4]={0,0,0,0};

uint32_t gPastEng_Time[4]={0,0,0,0};			// for Past ENG Error Meter Display
uint8_t gPastEng_UserIdx[4]={0,0,0,0};

uint8_t EngErr_CNT = 0;
	
#define ASCIILINESZ         (1024)

static char *ErrorMSG;

static uint8_t gM8_UserFlag = 0;
static uint32_t M8_SecTimer;
static uint8_t M8_ExitCNT = 0;

void M8_EngSelectFree(void);
void M8_ACSelectFree(void);
void M8_PastCarSelectFree(void);
void M8_PastEngSelectFree(void);
void M8_Change_Page(uint8_t tOldPageIdx,uint8_t tNewPageIdx);

bool M8_UserDisplay(void);

void ACErrorMSG_Display(uint8_t AC_ErrorCode);
void CarErrorMSG_Display(uint16_t Car_ErrorCode);
void EngErrorMSG_Display(uint32_t Eng_SPNCode,uint8_t Eng_FMICode);
void PastCarErrorMSG_Display(uint16_t Car_ErrorCode);
void PastEngErrorMSG_Display(uint32_t Eng_SPNCode,uint8_t Eng_FMICode);

void CarErrCode_TB(uint16_t tCarCode);
void CarErr_ValueUpdate(ITUText *tCarErrValue,uint16_t tCarCode);

void PastCarErrCode_TB(uint16_t tCarCode);
void PastCarErr_ValueUpdate(ITUText* tCarErrValue,uint16_t tCarCode);
void PastCarTrim_ValueUpdate(ITUText* tCarTrimValue,uint32_t tCarTrim);
void PastCarBTN_PressDisp(uint8_t tBTN_Num,bool Disp);
void PastCar_UserDisplay(uint8_t tLine,bool tDisp);
void PastCarBTN_PressDispClear();

void EngError_MakeDisplayData(void);

void EngErrCode_TB(uint32_t mSPN,uint8_t mFMI);
void EngErr_SPNValueUpdate(ITUText *tEngSPNValue,uint32_t tSPN);
void EngErr_FMIValueUpdate(ITUText *tEngFMIValue,uint8_t tFMI);


void PastEng_UserDisplay(uint8_t tLine,bool tDisp);
void PastEngBTN_PressDisp(uint8_t tBTN_Num,bool Disp);
void PastEngErrCode_TB(uint32_t mSPN,uint8_t mFMI);
void PastEngErr_SPNValueUpdate(ITUText *tEngSPNValue,uint32_t tSPN);
void PastEngErr_FMIValueUpdate(ITUText *tEngFMIValue,uint8_t tFMI);
void PastEngTrim_ValueUpdate(ITUText* tEngTrimValue,uint32_t tEngTrim);
void PastEngBTN_PressDispClear();

void ACErrCode_TB(uint8_t tAC_Code);
void ACErr_ValueUpdate(ITUText* tACErrValue,uint8_t tAC_Code);

void Get_ACErrorMSG2(char *AC_MSG,uint8_t tAC_Code);
void Get_CarErrorMSG2(char *Car_MSG,uint16_t tCar_Code);
void Get_EngErrorMSG2(char *Eng_MSG,uint32_t kSPN,uint8_t kFMI);

void Get_ACErrorMSG3(char *AC_MSG,uint8_t tAC_Code);
void Get_CarErrorMSG3(char *Car_MSG,uint16_t tCar_Code);
void Get_EngErrorMSG3(char *Eng_MSG,uint32_t kSPN,uint8_t kFMI);

bool M8D7_MouseUp(ITUWidget* widget, char* param);

void Reset_M8HeepMemory(void);

extern void GotoMenuTop(void);
extern void GotoMenu7(void);

// extern bool PushTxData (uint32_t IDenti, uint8_t *TxData,uint8_t Length);  JPHong 06-11

extern bool DOUBLE_KEY_ENABLE_FLAG;


bool M8_CheckUserIcon(void)	// JPHong 2024-03-08
{   
	//chpark.24.11.14
	if(User_RegInfo[M7_UserIndex].RegFlag == SU100_USER_AD && g_nM7_StartType == M7_MODE_PUSH)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	/*
	switch (User_RegInfo[M7_UserIndex].RegFlag)
	{
		case 0:
		case 2:
		case 3:
		case 5:
		case 9:
		case 15:
			return 1;
		break;
	}
   	return 0;
	*/
}


void GotoMenu8_Error(void)	// JPHong 2023-05-18
{
    ITULayer *Menu8_Layer = ituSceneFindWidget(&theScene, "Menu8_Layer");
    assert(Menu8_Layer);
    ituLayerGoto(Menu8_Layer);
}

void Reset_M8HeepMemory(void)
{
	
}

bool Menu8_Enter(ITUWidget* widget, char* param)
{
	printf("menu8 Layer DOUBLE_KEY_ENABLE_FLAG: %d\n", DOUBLE_KEY_ENABLE_FLAG);
	Main_Disp_Mode = MAIN_MODE_MENU;	// JPHong 2023-04-18
	
	{	// JPHong TTMS 15418
		uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
		txbuffer[0] = 0x3F;
		txbuffer[1] = 0x50;
		txbuffer[2] = 0x00;
		
		#if 0
			PushTxData (0x18503FE4,txbuffer,8);
		#else
			CanbusTxDataManagement( gMainTimerCnt_10m,PGN_REQ1,txbuffer);
		#endif
		
			txbuffer[0] = 0xEA;
			txbuffer[1] = 0x50;
			txbuffer[2] = 0x00;

		#if 0
			PushTxData (0x1850EAE4,txbuffer,8);
		#else
			CanbusTxDataManagement( gMainTimerCnt_10m,PGN_REQ1,txbuffer);
		#endif
	}
				
	//chpark.24.04.11
	SetWidget(PastCar_TITLE_CONT,"PastCar_CONT_TITLE");

	SetWidget(PastCar_TITLE[Lang_ENG],"PastCar_TITLE_ENG");
    SetWidget(PastCar_TITLE[Lang_DEU],"PastCar_TITLE_GER");
    SetWidget(PastCar_TITLE[Lang_FRA], "PastCar_TITLE_FR");
    SetWidget(PastCar_TITLE[Lang_ITA], "PastCar_TITLE_IT");
    SetWidget(PastCar_TITLE[Lang_ESP], "PastCar_TITLE_SP");

	CarErr1_Value = ituSceneFindWidget(&theScene, "CarErr1_Value");
	assert(CarErr1_Value);
	CarErr2_Value = ituSceneFindWidget(&theScene, "CarErr2_Value");
	assert(CarErr2_Value);
	CarErr3_Value = ituSceneFindWidget(&theScene, "CarErr3_Value");
	assert(CarErr3_Value);
	CarErr4_Value = ituSceneFindWidget(&theScene, "CarErr4_Value");
	assert(CarErr4_Value);

	EngSPN1_Value = ituSceneFindWidget(&theScene, "EngSPN1_Value");
	assert(EngSPN1_Value);
	EngSPN2_Value = ituSceneFindWidget(&theScene, "EngSPN2_Value");
	assert(EngSPN2_Value);
	EngSPN3_Value = ituSceneFindWidget(&theScene, "EngSPN3_Value");
	assert(EngSPN3_Value);

	EngFMI1_Value = ituSceneFindWidget(&theScene, "EngFMI1_Value");
	assert(EngFMI1_Value);
	EngFMI2_Value = ituSceneFindWidget(&theScene, "EngFMI2_Value");
	assert(EngFMI2_Value);
	EngFMI3_Value = ituSceneFindWidget(&theScene, "EngFMI3_Value");
	assert(EngFMI3_Value);

	EngErrCode = ituSceneFindWidget(&theScene, "EngErrCode");
	assert(EngErrCode);
	EngErrCodeDetail = ituSceneFindWidget(&theScene, "EngErrCodeDetail");
	assert(EngErrCodeDetail);

	ACErr1_Value = ituSceneFindWidget(&theScene, "ACErr1_Value");
	assert(ACErr1_Value);
	ACErr2_Value = ituSceneFindWidget(&theScene, "ACErr2_Value");
	assert(ACErr2_Value);
	ACErr3_Value = ituSceneFindWidget(&theScene, "ACErr3_Value");
	assert(ACErr3_Value);
	ACErr4_Value = ituSceneFindWidget(&theScene, "ACErr4_Value");
	assert(ACErr4_Value);
	ACErr5_Value = ituSceneFindWidget(&theScene, "ACErr5_Value");
	assert(ACErr5_Value);
	ACErr6_Value = ituSceneFindWidget(&theScene, "ACErr6_Value");
	assert(ACErr6_Value);
	ACErr7_Value = ituSceneFindWidget(&theScene, "ACErr7_Value");
	assert(ACErr7_Value);
	ACErr8_Value = ituSceneFindWidget(&theScene, "ACErr8_Value");
	assert(ACErr8_Value);

	ACErrCode = ituSceneFindWidget(&theScene, "ACErrCode");
	assert(ACErrCode);
	ACErrCodeDetail = ituSceneFindWidget(&theScene, "ACErrCodeDetail");
	assert(ACErrCodeDetail);

	PastCarErr1_Value = ituSceneFindWidget(&theScene, "PastCarErr1_Value");
	assert(PastCarErr1_Value);
	PastCarErr2_Value = ituSceneFindWidget(&theScene, "PastCarErr2_Value");
	assert(PastCarErr2_Value);
	PastCarErr3_Value = ituSceneFindWidget(&theScene, "PastCarErr3_Value");
	assert(PastCarErr3_Value);
	PastCarErr4_Value = ituSceneFindWidget(&theScene, "PastCarErr4_Value");
	assert(PastCarErr4_Value);
	PastCarErr5_Value = ituSceneFindWidget(&theScene, "PastCarErr5_Value");
	assert(PastCarErr5_Value);

	PastCarTrim1_Value = ituSceneFindWidget(&theScene, "PastCarTrim1_Value");
	assert(PastCarTrim1_Value);
	PastCarTrim2_Value = ituSceneFindWidget(&theScene, "PastCarTrim2_Value");
	assert(PastCarTrim2_Value);
	PastCarTrim3_Value = ituSceneFindWidget(&theScene, "PastCarTrim3_Value");
	assert(PastCarTrim3_Value);
	PastCarTrim4_Value = ituSceneFindWidget(&theScene, "PastCarTrim4_Value");
	assert(PastCarTrim4_Value);
	PastCarTrim5_Value = ituSceneFindWidget(&theScene, "PastCarTrim5_Value");
	assert(PastCarTrim5_Value);

	PastCar_Name1 = ituSceneFindWidget(&theScene, "PastCar_Name1");
	assert(PastCar_Name1);
	PastCar_Name2 = ituSceneFindWidget(&theScene, "PastCar_Name2");
	assert(PastCar_Name2);
	PastCar_Name3 = ituSceneFindWidget(&theScene, "PastCar_Name3");
	assert(PastCar_Name3);
	PastCar_Name4 = ituSceneFindWidget(&theScene, "PastCar_Name4");
	assert(PastCar_Name4);
	PastCar_Name5 = ituSceneFindWidget(&theScene, "PastCar_Name5");
	assert(PastCar_Name5);

	PastCarErrCode = ituSceneFindWidget(&theScene, "PastCarErrCode");
	assert(PastCarErrCode);
	PastCarErrCodeDetail = ituSceneFindWidget(&theScene, "PastCarErrCodeDetail");
	assert(PastCarErrCodeDetail);

	PastEngSPN1_Value = ituSceneFindWidget(&theScene, "PastEngSPN1_Value");
	assert(PastEngSPN1_Value);
	PastEngSPN2_Value = ituSceneFindWidget(&theScene, "PastEngSPN2_Value");
	assert(PastEngSPN2_Value);
	PastEngSPN3_Value = ituSceneFindWidget(&theScene, "PastEngSPN3_Value");
	assert(PastEngSPN3_Value);
	PastEngSPN4_Value = ituSceneFindWidget(&theScene, "PastEngSPN4_Value");
	assert(PastEngSPN4_Value);

	PastEngFMI1_Value = ituSceneFindWidget(&theScene, "PastEngFMI1_Value");
	assert(PastEngFMI1_Value);
	PastEngFMI2_Value = ituSceneFindWidget(&theScene, "PastEngFMI2_Value");
	assert(PastEngFMI2_Value);
	PastEngFMI3_Value = ituSceneFindWidget(&theScene, "PastEngFMI3_Value");
	assert(PastEngFMI3_Value);
	PastEngFMI4_Value = ituSceneFindWidget(&theScene, "PastEngFMI4_Value");
	assert(PastEngFMI4_Value);

	PastEngMeter1 = ituSceneFindWidget(&theScene, "PastEngMeter1");
	assert(PastEngMeter1);
	PastEngMeter2 = ituSceneFindWidget(&theScene, "PastEngMeter2");
	assert(PastEngMeter2);
	PastEngMeter3 = ituSceneFindWidget(&theScene, "PastEngMeter3");
	assert(PastEngMeter3);
	PastEngMeter4 = ituSceneFindWidget(&theScene, "PastEngMeter4");
	assert(PastEngMeter4);

	PastEng_Name1 = ituSceneFindWidget(&theScene, "PastEng_Name1");
	assert(PastEng_Name1);
	PastEng_Name2 = ituSceneFindWidget(&theScene, "PastEng_Name2");
	assert(PastEng_Name2);
	PastEng_Name3 = ituSceneFindWidget(&theScene, "PastEng_Name3");
	assert(PastEng_Name3);
	PastEng_Name4 = ituSceneFindWidget(&theScene, "PastEng_Name4");
	assert(PastEng_Name4);

	PastEngErrCode = ituSceneFindWidget(&theScene, "PastEngErrCode");
	assert(PastEngErrCode);
	PastEngErrCodeDetail = ituSceneFindWidget(&theScene, "PastEngErrCodeDetail");
	assert(PastEngErrCodeDetail);

	char str[SumString_Size];
	uint8_t i;
	
	uint16_t ErrorMSGSize;
	uint16_t tReadMSGSize;

	gM8_UserFlag = User_RegInfo[M7_UserIndex].RegFlag;

/*
	gM8_UserFlag = 5;		// dev.opt 240118 5;admin

	// dev.opt
	User_RegInfo[0].RegFlag = 5;
	User_RegInfo[1].RegFlag = 2;
	User_RegInfo[2].RegFlag = 3;
	User_RegInfo[3].RegFlag = 9;

	User_RegInfo[4].RegFlag = 5;
	User_RegInfo[5].RegFlag = 2;
	User_RegInfo[6].RegFlag = 3;
	User_RegInfo[7].RegFlag = 9;

	sprintf(User_RegInfo[0].UserName,"Big james");
	sprintf(User_RegInfo[1].UserName,"Jhon");
	sprintf(User_RegInfo[2].UserName,"TAKEUCHI");
	sprintf(User_RegInfo[3].UserName,"SPK_XO");

	sprintf(User_RegInfo[4].UserName,"BatMan");
	sprintf(User_RegInfo[5].UserName,"SuperMan");
	sprintf(User_RegInfo[6].UserName,"IronMan");
	sprintf(User_RegInfo[7].UserName,"XO800");
*/

	M8_Change_Page(cPage_Index,0);
	cPage_Index = 0;
	cError_Index = 0;

	ituWidgetSetVisible(EngErrCode, false);
	ituWidgetSetVisible(EngErrCodeDetail, false);

	memset(str,0,SumString_Size);

	EngError_MakeDisplayData();

	PastEngBTN_PressDispClear(); // 2024-04-30 add whlim, reset pressDisplay
	PastCarBTN_PressDispClear(); // 2024-04-30 add whlim, reset pressDisplay
//================================================================================

	if(gEMR[0] > 0)
	{
		cError_Index = 1;
		CarErrorMSG_Display(gEMR[0]);
	}
	else if(gEMR[1] > 0)
	{
		cError_Index = 2;
		CarErrorMSG_Display(gEMR[1]);
	}
	else if(gEMR[2] > 0)
	{
		cError_Index = 3;
		CarErrorMSG_Display(gEMR[2]);
	}
	else if(gEMR[3] > 0)
	{
		cError_Index = 4;
		CarErrorMSG_Display(gEMR[3]);
	}
	else if(gSPN[0] > 0)
	{
		cError_Index = 5;
		EngErrorMSG_Display(gSPN[0],gFMI[0]);
	}
	else if(gSPN[1] > 0)
	{
		cError_Index = 6;
		EngErrorMSG_Display(gSPN[1],gFMI[1]);
	}
	else if(gSPN[2] > 0)
	{
		cError_Index = 7;
		EngErrorMSG_Display(gSPN[2],gFMI[2]);
	}

//==================================================================================

	M8_SecTimer = gMainTimerCnt_10m;
	M8_ExitCNT = 0;

    return true;
}

bool Menu8_Timer(ITUWidget* widget, char* param)
{
	char str[SumString_Size];

	static uint16_t b_gEMR[4];
	static uint32_t b_gSPN[3];
	static uint8_t b_gFMI[3];

	static uint8_t b_gACERR_Code[8];

	static uint8_t b_gPastCar_UserIdx[5];
	static uint16_t b_gEMR_Past[5];
	static uint32_t b_gEMR_Time[5];

	static uint8_t b_gPastEng_UserIdx[4];
	static uint32_t b_gSPN_Past[4];
	static uint8_t b_gFMI_Past[4];
	static uint32_t b_gPastEng_Time[4];


#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	
	if((gMainTimerCnt_10m - M8_SecTimer) >= 100)
	{
		if(M8_ExitCNT < 30)
			M8_ExitCNT++;
		if(M8_ExitCNT == 30)
		{
			M8_ExitCNT = 100;
			printf("\r\nM8 Menu 30Sec Exit > goto Main\r\n");
			//GotoMain(MAIN_MODE_NORMAL); 
		}

		M8_SecTimer = gMainTimerCnt_10m;
	}

	EngError_MakeDisplayData();
//	PastEngError_MakeDisplayData();

	gM8_UserFlag = User_RegInfo[M7_UserIndex].RegFlag;
			
	switch(cPage_Index)
	{
		case EngineErrorCode:
		{
			if(b_gEMR[0] != gEMR[0])
			{
				b_gEMR[0] = gEMR[0];
				CarErr_ValueUpdate(CarErr1_Value, gEMR[0]);
				
				if(cError_Index == 1)
				{
					if(gEMR[0] > 0)
					{
						CarErrorMSG_Display(gEMR[0]);
					}
					else if(gEMR[0] == 0)
					{
						ituWidgetSetVisible(EngErrCode, false);
						ituWidgetSetVisible(EngErrCodeDetail, false);
					}
				}
			}
			if(b_gEMR[1] != gEMR[1])
			{
				b_gEMR[1] = gEMR[1];
				CarErr_ValueUpdate(CarErr2_Value, gEMR[1]);
				
				if(cError_Index == 2)
				{
					if(gEMR[1] > 0)
					{
						CarErrorMSG_Display(gEMR[1]);
					}
					else if(gEMR[1] == 0)
					{
						ituWidgetSetVisible(EngErrCode, false);
						ituWidgetSetVisible(EngErrCodeDetail, false);
					}
				}
			}
			if(b_gEMR[2] != gEMR[2])
			{
				b_gEMR[2] = gEMR[2];
				CarErr_ValueUpdate(CarErr3_Value, gEMR[2]);

				if(cError_Index == 3)
				{
					if(gEMR[2] > 0)
					{
						CarErrorMSG_Display(gEMR[2]);
					}
					else if(gEMR[2] == 0)
					{
						ituWidgetSetVisible(EngErrCode, false);
						ituWidgetSetVisible(EngErrCodeDetail, false);
					}
				}
			}
			if(b_gEMR[3] != gEMR[3])
			{
				b_gEMR[3] = gEMR[3];
				CarErr_ValueUpdate(CarErr4_Value, gEMR[3]);

				if(cError_Index == 4)
				{
					if(gEMR[3] > 0)
					{
						CarErrorMSG_Display(gEMR[3]);
					}
					else if(gEMR[3] == 0)
					{
						ituWidgetSetVisible(EngErrCode, false);
						ituWidgetSetVisible(EngErrCodeDetail, false);
					}
				}
			}
			if((b_gSPN[0] != gSPN[0]) || (b_gFMI[0] != gFMI[0]))
			{
				b_gSPN[0] = gSPN[0];
				b_gFMI[0] = gFMI[0];
					
				EngErr_SPNValueUpdate(EngSPN1_Value,gSPN[0]);
				EngErr_FMIValueUpdate(EngFMI1_Value,gFMI[0]);

				if(cError_Index == 5)
				{
					if(gSPN[0] > 0)
					{
						EngErrorMSG_Display(gSPN[0],gFMI[0]);
					}
					else if(gSPN[0] == 0)
					{
						ituWidgetSetVisible(EngErrCode, false);
						ituWidgetSetVisible(EngErrCodeDetail, false);
					}
				}
			}			
			if((b_gSPN[1] != gSPN[1]) || (b_gFMI[1] != gFMI[1]))
			{
				b_gSPN[1] = gSPN[1];
				b_gFMI[1] = gFMI[1];
			
				EngErr_SPNValueUpdate(EngSPN2_Value,gSPN[1]);
				EngErr_FMIValueUpdate(EngFMI2_Value,gFMI[1]);

				if(cError_Index == 6)
				{
					if(gSPN[1] > 0)
					{
						EngErrorMSG_Display(gSPN[1],gFMI[1]);
					}
					else if(gSPN[1] == 0)
					{
						ituWidgetSetVisible(EngErrCode, false);
						ituWidgetSetVisible(EngErrCodeDetail, false);
					}
				}
			}
			if((b_gSPN[2] != gSPN[2]) || (b_gFMI[2] != gFMI[2]))
			{
				b_gSPN[2] = gSPN[2];
				b_gFMI[2] = gFMI[2];
				
				EngErr_SPNValueUpdate(EngSPN3_Value,gSPN[2]);
				EngErr_FMIValueUpdate(EngFMI3_Value,gFMI[2]);
				
				if(cError_Index == 7)
				{
					if(gSPN[2] > 0)
					{
						EngErrorMSG_Display(gSPN[2],gFMI[2]);
					}
					else if(gSPN[2] == 0)
					{
						ituWidgetSetVisible(EngErrCode, false);
						ituWidgetSetVisible(EngErrCodeDetail, false);
					}
				}
			}
		}
		break;

		case AirConErrorCode:
		{
			if(b_gACERR_Code[0] != gACERR_Code[0])
			{
				b_gACERR_Code[0] = gACERR_Code[0];
				ACErr_ValueUpdate(ACErr1_Value,gACERR_Code[0]);
				
				if(cError_Index == 1)
				{
					if(gACERR_Code[0] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[0]);
					}
					else if(gACERR_Code[0] == 0)
					{
						ituWidgetSetVisible(ACErrCode, false);
						ituWidgetSetVisible(ACErrCodeDetail, false);
					}
				}
			}
			if(b_gACERR_Code[1] != gACERR_Code[1])
			{
				b_gACERR_Code[1] = gACERR_Code[1];
				ACErr_ValueUpdate(ACErr2_Value,gACERR_Code[1]);

				if(cError_Index == 2)
				{
					if(gACERR_Code[1] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[1]);
					}
					else if(gACERR_Code[1] == 0)
					{
						ituWidgetSetVisible(ACErrCode, false);
						ituWidgetSetVisible(ACErrCodeDetail, false);
					}
				}
			}
			if(b_gACERR_Code[2] != gACERR_Code[2])
			{
				b_gACERR_Code[2] = gACERR_Code[2];
				ACErr_ValueUpdate(ACErr3_Value,gACERR_Code[2]);
				
				if(cError_Index == 3)
				{
					if(gACERR_Code[2] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[2]);
					}
					else if(gACERR_Code[2] == 0)
					{
						ituWidgetSetVisible(ACErrCode, false);
						ituWidgetSetVisible(ACErrCodeDetail, false);
					}
				}				
			}
			if(b_gACERR_Code[3] != gACERR_Code[3])
			{
				b_gACERR_Code[3] = gACERR_Code[3];
				ACErr_ValueUpdate(ACErr4_Value,gACERR_Code[3]);
				
				if(cError_Index == 4)
				{
					if(gACERR_Code[3] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[3]);
					}
					else if(gACERR_Code[3] == 0)
					{
						ituWidgetSetVisible(ACErrCode, false);
						ituWidgetSetVisible(ACErrCodeDetail, false);
					}
				}				
			}
			if(b_gACERR_Code[4] != gACERR_Code[4])
			{
				b_gACERR_Code[4] = gACERR_Code[4];
				ACErr_ValueUpdate(ACErr5_Value,gACERR_Code[4]);
				
				if(cError_Index == 5)
				{
					if(gACERR_Code[4] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[5]);
					}
					else if(gACERR_Code[4] == 0)
					{
						ituWidgetSetVisible(ACErrCode, false);
						ituWidgetSetVisible(ACErrCodeDetail, false);
					}
				}
			}
			if(b_gACERR_Code[5] != gACERR_Code[5])
			{
				b_gACERR_Code[5] = gACERR_Code[5];
				ACErr_ValueUpdate(ACErr6_Value,gACERR_Code[5]);

				if(cError_Index == 6)
				{
					if(gACERR_Code[5] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[5]);
					}
					else if(gACERR_Code[5] == 0)
					{
						ituWidgetSetVisible(ACErrCode, false);
						ituWidgetSetVisible(ACErrCodeDetail, false);
					}
				}
			}
			if(b_gACERR_Code[6] != gACERR_Code[6])
			{
				b_gACERR_Code[6] = gACERR_Code[6];
				ACErr_ValueUpdate(ACErr7_Value,gACERR_Code[6]);
				
				if(cError_Index == 7)
				{
					if(gACERR_Code[6] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[6]);
					}
					else if(gACERR_Code[6] == 0)
					{
						ituWidgetSetVisible(ACErrCode, false);
						ituWidgetSetVisible(ACErrCodeDetail, false);
					}
				}
			}
			if(b_gACERR_Code[7] != gACERR_Code[7])
			{
				b_gACERR_Code[7] = gACERR_Code[7];
				ACErr_ValueUpdate(ACErr8_Value,gACERR_Code[7]);

				if(cError_Index == 8)
				{
					if(gACERR_Code[7] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[7]);
					}
					else if(gACERR_Code[7] == 0)
					{
						ituWidgetSetVisible(ACErrCode, false);
						ituWidgetSetVisible(ACErrCodeDetail, false);
					}	
				}
			}
		}
		break;

		case PastCarCode:
		{
			if((b_gEMR_Past[0] != gEMR_Past[0])||(b_gEMR_Time[0] != gEMR_Time[0])||(b_gPastCar_UserIdx[0] != gPastCar_UserIdx[0]))
			{
				b_gEMR_Past[0] = gEMR_Past[0];
				b_gEMR_Time[0] = gEMR_Time[0];
				b_gPastCar_UserIdx[0] = gPastCar_UserIdx[0];

				if(M8_CheckUserIcon() == 1)
					PastCar_UserDisplay(0,true);
				else
					PastCar_UserDisplay(0,false);

				PastCarErr_ValueUpdate(PastCarErr1_Value,gEMR_Past[0]);
				PastCarTrim_ValueUpdate(PastCarTrim1_Value,gEMR_Time[0]);
				
				if(cError_Index == 1)
				{
					if(gEMR_Past[0] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[0]);
					}
					else if(gEMR_Past[0] == 0)
					{
						ituWidgetSetVisible(PastCarErrCode, false);
						ituWidgetSetVisible(PastCarErrCodeDetail, false);
					}
				}
			}

			if((b_gEMR_Past[1] != gEMR_Past[1])||(b_gEMR_Time[1] != gEMR_Time[1])||(b_gPastCar_UserIdx[1] != gPastCar_UserIdx[1]))
			{
				b_gEMR_Past[1] = gEMR_Past[1];
				b_gEMR_Time[1] = gEMR_Time[1];
				b_gPastCar_UserIdx[1] = gPastCar_UserIdx[1];

				PastCarErr_ValueUpdate(PastCarErr2_Value,gEMR_Past[1]);
				PastCarTrim_ValueUpdate(PastCarTrim2_Value,gEMR_Time[1]);

				if(M8_CheckUserIcon() == 1)
					PastCar_UserDisplay(1,true);
				else
					PastCar_UserDisplay(1,false);
				
				if(cError_Index == 2)
				{
					if(gEMR_Past[1] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[1]);
					}
					else if(gEMR_Past[1] == 0)
					{
						ituWidgetSetVisible(PastCarErrCode, false);
						ituWidgetSetVisible(PastCarErrCodeDetail, false);
					}
				}
			}

			if((b_gEMR_Past[2] != gEMR_Past[2])||(b_gEMR_Time[2] != gEMR_Time[2])||(b_gPastCar_UserIdx[2] != gPastCar_UserIdx[2]))
			{
				b_gEMR_Past[2] = gEMR_Past[2];
				b_gEMR_Time[2] = gEMR_Time[2];
				b_gPastCar_UserIdx[2] = gPastCar_UserIdx[2];

				PastCarErr_ValueUpdate(PastCarErr3_Value,gEMR_Past[2]);
				PastCarTrim_ValueUpdate(PastCarTrim3_Value,gEMR_Time[2]);

				if(M8_CheckUserIcon() == 1)
					PastCar_UserDisplay(2,true);
				else
					PastCar_UserDisplay(2,false);
			
				if(cError_Index == 3)
				{
					if(gEMR_Past[2] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[2]);
					}
					else if(gEMR_Past[2] == 0)
					{
						ituWidgetSetVisible(PastCarErrCode, false);
						ituWidgetSetVisible(PastCarErrCodeDetail, false);
					}
				}			
			}	

			if((b_gEMR_Past[3] != gEMR_Past[3])||(b_gEMR_Time[3] != gEMR_Time[3])||(b_gPastCar_UserIdx[3] != gPastCar_UserIdx[3]))
			{
				b_gEMR_Past[3] = gEMR_Past[3];
				b_gEMR_Time[3] = gEMR_Time[3];
				b_gPastCar_UserIdx[3] = gPastCar_UserIdx[3];

				PastCarErr_ValueUpdate(PastCarErr4_Value,gEMR_Past[3]);
				PastCarTrim_ValueUpdate(PastCarTrim4_Value,gEMR_Time[3]);

				if(M8_CheckUserIcon() == 1)
					PastCar_UserDisplay(3,true);
				else
					PastCar_UserDisplay(3,false);
			
				if(cError_Index == 4)
				{
					if(gEMR_Past[3] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[3]);
					}
					else if(gEMR_Past[3] == 0)
					{
						ituWidgetSetVisible(PastCarErrCode, false);
						ituWidgetSetVisible(PastCarErrCodeDetail, false);
					}
				}		
			}	

			if((b_gEMR_Past[4] != gEMR_Past[4])||(b_gEMR_Time[4] != gEMR_Time[4])||(b_gPastCar_UserIdx[4] != gPastCar_UserIdx[4]))
			{
				b_gEMR_Past[4] = gEMR_Past[4];
				b_gEMR_Time[4] = gEMR_Time[4];
				b_gPastCar_UserIdx[4] = gPastCar_UserIdx[4];
				
				PastCarErr_ValueUpdate(PastCarErr5_Value,gEMR_Past[4]);
				PastCarTrim_ValueUpdate(PastCarTrim5_Value,gEMR_Time[4]);
			
				if(M8_CheckUserIcon() == 1)
					PastCar_UserDisplay(4,true);
				else
					PastCar_UserDisplay(4,false);

				if(cError_Index == 5)
				{
					if(gEMR_Past[4] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[4]);
					}
					else if(gEMR_Past[4] == 0)
					{
						ituWidgetSetVisible(PastCarErrCode, false);
						ituWidgetSetVisible(PastCarErrCodeDetail, false);
					}
				}
			}
		}
		break;

		case PastEngineCode:
		{
			if((b_gSPN_Past[0] != gSPN_Past[0]) || (b_gFMI_Past[0] != gFMI_Past[0]) || (b_gPastEng_Time[0] != gPastEng_Time[0])||(b_gPastEng_UserIdx[0] != gPastEng_UserIdx[0]))
			{
				b_gSPN_Past[0] = gSPN_Past[0];
				b_gFMI_Past[0] = gFMI_Past[0];
				b_gPastEng_Time[0] = gPastEng_Time[0];
				b_gPastEng_UserIdx[0] = gPastEng_UserIdx[0];
				
				PastEngErr_SPNValueUpdate(PastEngSPN1_Value,gSPN_Past[0]);
				PastEngErr_FMIValueUpdate(PastEngFMI1_Value,gFMI_Past[0]);
				PastEngTrim_ValueUpdate(PastEngMeter1,gPastEng_Time[0]);

				if(M8_CheckUserIcon() == 1)
					PastEng_UserDisplay(0,true);
				else
					PastEng_UserDisplay(0,false);

				if(cError_Index == 1)
				{
					if(gSPN_Past[0] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[0],gFMI_Past[0]);
					}
					else if(gSPN_Past[0] == 0)
					{
						ituWidgetSetVisible(PastEngErrCode, false);
						ituWidgetSetVisible(PastEngErrCodeDetail, false);
					}
				}
			}			
			if((b_gSPN_Past[1] != gSPN_Past[1])||(b_gFMI_Past[1] != gFMI_Past[1])||(b_gPastEng_Time[1] != gPastEng_Time[1])||(b_gPastEng_UserIdx[1] != gPastEng_UserIdx[1]))
			{
				b_gSPN_Past[1] = gSPN_Past[1];
				b_gFMI_Past[1] = gFMI_Past[1];
				b_gPastEng_Time[1] = gPastEng_Time[1];
				b_gPastEng_UserIdx[1] = gPastEng_UserIdx[1];
				
				PastEngErr_SPNValueUpdate(PastEngSPN2_Value,gSPN_Past[1]);
				PastEngErr_FMIValueUpdate(PastEngFMI2_Value,gFMI_Past[1]);
				PastEngTrim_ValueUpdate(PastEngMeter2,gPastEng_Time[1]);

				if(M8_CheckUserIcon() == 1)
					PastEng_UserDisplay(1,true);
				else
					PastEng_UserDisplay(1,false);

				if(cError_Index == 2)
				{
					if(gSPN_Past[1] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[1],gFMI_Past[1]);
					}
					else if(gSPN_Past[1] == 0)
					{
						ituWidgetSetVisible(PastEngErrCode, false);
						ituWidgetSetVisible(PastEngErrCodeDetail, false);
					}
				}
			}	
			if((b_gSPN_Past[2] != gSPN_Past[2])||(b_gFMI_Past[2] != gFMI_Past[2])||(b_gPastEng_Time[2] != gPastEng_Time[2])||(b_gPastEng_UserIdx[2] != gPastEng_UserIdx[2]))
			{
				b_gSPN_Past[2] = gSPN_Past[2];
				b_gFMI_Past[2] = gFMI_Past[2];
				b_gPastEng_Time[2] = gPastEng_Time[2];
				b_gPastEng_UserIdx[2] = gPastEng_UserIdx[2];

				PastEngErr_SPNValueUpdate(PastEngSPN3_Value,gSPN_Past[2]);
				PastEngErr_FMIValueUpdate(PastEngFMI3_Value,gFMI_Past[2]);
				PastEngTrim_ValueUpdate(PastEngMeter3,gPastEng_Time[2]);

				if(M8_CheckUserIcon() == 1)
					PastEng_UserDisplay(2,true);
				else
					PastEng_UserDisplay(2,false);

				if(cError_Index == 3)
				{
					if(gSPN_Past[2] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[2],gFMI_Past[2]);
					}
					else if(gSPN_Past[2] == 0)
					{
						ituWidgetSetVisible(PastEngErrCode, false);
						ituWidgetSetVisible(PastEngErrCodeDetail, false);
					}
				}
			}
			if((b_gSPN_Past[3] != gSPN_Past[3])||(b_gFMI_Past[3] != gFMI_Past[3])||(b_gPastEng_Time[3] != gPastEng_Time[3])||(b_gPastEng_UserIdx[3] != gPastEng_UserIdx[3]))
			{
				b_gSPN_Past[3] = gSPN_Past[3];
				b_gFMI_Past[3] = gFMI_Past[3];
				b_gPastEng_Time[3] = gPastEng_Time[3];
				b_gPastEng_UserIdx[3] = gPastEng_UserIdx[3];
				
				PastEngErr_SPNValueUpdate(PastEngSPN4_Value,gSPN_Past[3]);
				PastEngErr_FMIValueUpdate(PastEngFMI4_Value,gFMI_Past[3]);
				PastEngTrim_ValueUpdate(PastEngMeter4,gPastEng_Time[3]);

				if(M8_CheckUserIcon() == 1)
					PastEng_UserDisplay(3,true);
				else
					PastEng_UserDisplay(3,false);

				if(cError_Index == 4)
				{
					if(gSPN_Past[3] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[3],gFMI_Past[3]);
					}
					else if(gSPN_Past[3] == 0)
					{
						ituWidgetSetVisible(PastEngErrCode, false);
						ituWidgetSetVisible(PastEngErrCodeDetail, false);
					}
				}
			}
		}
		break;
	}

	M8_UserDisplay();
	
    return true;
}
bool Menu8_Leave(ITUWidget* widget, char* param)
{
	printf("M8 Menu8_Leave \r\n");
    return true;
}

bool Menu8_JogWheel(ITUWidget* widget, char* param)
{
	char str[SumString_Size];

	uint8_t tJogWheel = 0;
	
	M8_ExitCNT = 0;
	tJogWheel = param[0];
	
	printf("M8 JogWheel = %d\r\n",tJogWheel);

	if(tJogWheel == JOG_DIAL_ROT_RIGHT)
	{
		switch(cPage_Index)
		{
			case EngineErrorCode:
			{
				//M8_EngSelectFree();
				ituWidgetSetVisible(EngErrCode, false);
				ituWidgetSetVisible(EngErrCodeDetail, false);

_EngRetry_CCW:
				if(cError_Index > 0)
				{
					cError_Index--;

					if(cError_Index == 0)
						cError_Index = 7;
				}
					
				if(cError_Index == 1)
				{
					if(gEMR[0] > 0)
					{
						CarErrorMSG_Display(gEMR[0]);
					}
					else
						goto _EngRetry_CCW;
				}
				else if(cError_Index == 2)
				{
					if(gEMR[1] > 0)
					{
						CarErrorMSG_Display(gEMR[1]);
					}
					else
						goto _EngRetry_CCW;
				}
				else if(cError_Index == 3)
				{
					if(gEMR[2] > 0)
					{
						CarErrorMSG_Display(gEMR[2]);
					}
					else
						goto _EngRetry_CCW;
				}
				else if(cError_Index == 4)
				{
					if(gEMR[3] > 0)
					{
						CarErrorMSG_Display(gEMR[3]);
					}
					else
						goto _EngRetry_CCW;
				}
				else if(cError_Index == 5)
				{
					if(gSPN[0] > 0)
					{
						EngErrorMSG_Display(gSPN[0],gFMI[0]);
					}
					else
						goto _EngRetry_CCW;
				}
				else if(cError_Index == 6)
				{
					if(gSPN[1] > 0)
					{
						EngErrorMSG_Display(gSPN[1],gFMI[1]);
					}
					else
						goto _EngRetry_CCW;
				}
				else if(cError_Index == 7)
				{
					if(gSPN[2] > 0)
					{
						EngErrorMSG_Display(gSPN[2],gFMI[2]);
					}
					else
						goto _EngRetry_CCW;
				}
			}
			break;

			case AirConErrorCode:
			{
				//M8_ACSelectFree();
				ituWidgetSetVisible(ACErrCode, false);
				ituWidgetSetVisible(ACErrCodeDetail, false);

_ACRetry_CCW:
				if(cError_Index > 0)
				{
					cError_Index--;

					if(cError_Index == 0)
						cError_Index = 8;
				}
					
				if(cError_Index == 1)
				{
					if(gACERR_Code[0] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[0]);
					}
					else
						goto _ACRetry_CCW;
				}
				else if(cError_Index == 2)
				{
					if(gACERR_Code[1] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[1]);
					}
					else
						goto _ACRetry_CCW;
				}
				else if(cError_Index == 3)
				{
					if(gACERR_Code[2] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[2]);
					}
					else
						goto _ACRetry_CCW;
				}
				else if(cError_Index == 4)
				{
					if(gACERR_Code[3] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[3]);
					}
					else
						goto _ACRetry_CCW;
				}
				else if(cError_Index == 5)
				{
					if(gACERR_Code[4] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[4]);
					}
					else
						goto _ACRetry_CCW;
				}
				else if(cError_Index == 6)
				{
					if(gACERR_Code[5] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[5]);
					}
					else
						goto _ACRetry_CCW;
				}
				else if(cError_Index == 7)
				{
					if(gACERR_Code[6] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[6]);
					}
					else
						goto _ACRetry_CCW;
				}
				else if(cError_Index == 8)
				{
					if(gACERR_Code[7] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[7]);
					}
					else
						goto _ACRetry_CCW;
				}
			}
			break;
			
			case PastCarCode:
			{
				//M8_PastCarSelectFree();
				ituWidgetSetVisible(PastCarErrCode, false);
				ituWidgetSetVisible(PastCarErrCodeDetail, false);

_PastCarRetry_CCW:
				if(cError_Index > 0)
				{
					cError_Index--;

					if(cError_Index == 0)
						cError_Index = 5;
				}

				if(cError_Index == 1)
				{
					if(gEMR_Past[0] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[0]);
					}
					else
						goto _PastCarRetry_CCW;
				}
				else if(cError_Index == 2)
				{
					if(gEMR_Past[1] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[1]);
					}
					else
						goto _PastCarRetry_CCW;
				}
				else if(cError_Index == 3)
				{
					if(gEMR_Past[2] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[2]);
					}
					else
						goto _PastCarRetry_CCW;
				}
				else if(cError_Index == 4)
				{
					if(gEMR_Past[3] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[3]);
					}
					else
						goto _PastCarRetry_CCW;
				}
				else if(cError_Index == 5)
				{
					if(gEMR_Past[4] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[4]);
					}
					else
						goto _PastCarRetry_CCW;
				}
			}
			break;
			
			case PastEngineCode:
			{
				//M8_PastEngSelectFree();
				ituWidgetSetVisible(PastEngErrCode, false);
				ituWidgetSetVisible(PastEngErrCodeDetail, false);

_PastEngRetry_CCW:
				if(cError_Index > 0)
				{
					cError_Index--;

					if(cError_Index == 0)
						cError_Index = 4;
				}

				if(cError_Index == 1)
				{
					if(gSPN_Past[0] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[0],gFMI_Past[0]);
					}
					else
						goto _PastEngRetry_CCW;
				}
				else if(cError_Index == 2)
				{
					if(gSPN_Past[1] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[1],gFMI_Past[1]);
					}
					else
						goto _PastEngRetry_CCW;
				}
				else if(cError_Index == 3)
				{
					if(gSPN_Past[2] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[2],gFMI_Past[2]);
					}
					else
						goto _PastEngRetry_CCW;
				}
				else if(cError_Index == 4)
				{
					if(gSPN_Past[3] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[3],gFMI_Past[3]);
					}
					else
						goto _PastEngRetry_CCW;
				}
			}
			break;
		}
	}
	else if(tJogWheel == JOG_DIAL_ROT_LEFT)
	{
		switch(cPage_Index)
		{
			case EngineErrorCode:
			{
				//M8_EngSelectFree();
				ituWidgetSetVisible(EngErrCode, false);
				ituWidgetSetVisible(EngErrCodeDetail, false);

_EngRetry_CW:
				if((cError_Index > 0) && (cError_Index < 8))
				{
					cError_Index++;

					if(cError_Index == 8)
						cError_Index = 1;
				}
					
				if(cError_Index == 1)
				{
					if(gEMR[0] > 0)
					{
						CarErrorMSG_Display(gEMR[0]);
					}
					else
						goto _EngRetry_CW;
				}
				else if(cError_Index == 2)
				{
					if(gEMR[1] > 0)
					{
						CarErrorMSG_Display(gEMR[1]);
					}
					else
						goto _EngRetry_CW;
				}
				else if(cError_Index == 3)
				{
					if(gEMR[2] > 0)
					{
						CarErrorMSG_Display(gEMR[2]);
					}
					else
						goto _EngRetry_CW;
				}
				else if(cError_Index == 4)
				{
					if(gEMR[3] > 0)
					{
						CarErrorMSG_Display(gEMR[3]);
					}
					else
						goto _EngRetry_CW;
				}
				else if(cError_Index == 5)
				{
					if(gSPN[0] > 0)
					{
						EngErrorMSG_Display(gSPN[0],gFMI[0]);
					}
					else
						goto _EngRetry_CW;
				}
				else if(cError_Index == 6)
				{
					if(gSPN[1] > 0)
					{
						EngErrorMSG_Display(gSPN[1],gFMI[1]);
					}
					else
						goto _EngRetry_CW;
				}
				else if(cError_Index == 7)
				{
					if(gSPN[2] > 0)
					{
						EngErrorMSG_Display(gSPN[2],gFMI[2]);
					}
					else
						goto _EngRetry_CW;
				}
			}
			break;

			case AirConErrorCode:
			{
				//M8_ACSelectFree();
				ituWidgetSetVisible(ACErrCode, false);
				ituWidgetSetVisible(ACErrCodeDetail, false);

_ACRetry_CW:
				if((cError_Index > 0) && (cError_Index < 9))
				{
					cError_Index++;
				
					if(cError_Index == 9)
						cError_Index = 1;
				}
					
				if(cError_Index == 1)
				{
					if(gACERR_Code[0] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[0]);
					}
					else
						goto _ACRetry_CW;
				}
				else if(cError_Index == 2)
				{
					if(gACERR_Code[1] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[1]);
					}
					else
						goto _ACRetry_CW;
				}
				else if(cError_Index == 3)
				{
					if(gACERR_Code[2] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[2]);
					}
					else
						goto _ACRetry_CW;
				}
				else if(cError_Index == 4)
				{
					if(gACERR_Code[3] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[3]);
					}
					else
						goto _ACRetry_CW;
				}
				else if(cError_Index == 5)
				{
					if(gACERR_Code[4] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[4]);
					}
					else
						goto _ACRetry_CW;
				}
				else if(cError_Index == 6)
				{
					if(gACERR_Code[5] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[5]);
					}
					else
						goto _ACRetry_CW;
				}
				else if(cError_Index == 7)
				{
					if(gACERR_Code[6] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[6]);
					}
					else
						goto _ACRetry_CW;
				}
				else if(cError_Index == 8)
				{
					if(gACERR_Code[7] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[7]);
					}
					else
						goto _ACRetry_CW;
				}
			}
			break;

			case PastCarCode:
			{
				//M8_PastCarSelectFree();
				ituWidgetSetVisible(PastCarErrCode, false);
				ituWidgetSetVisible(PastCarErrCodeDetail, false);

_PastCarRetry_CW:
				if((cError_Index > 0) && (cError_Index < 6))
				{
					cError_Index++;

					if(cError_Index == 6)
						cError_Index = 1;
				}

				if(cError_Index == 1)
				{
					if(gEMR_Past[0] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[0]);
					}
					else
						goto _PastCarRetry_CW;
				}
				else if(cError_Index == 2)
				{
					if(gEMR_Past[1] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[1]);
					}
					else
						goto _PastCarRetry_CW;
				}
				else if(cError_Index == 3)
				{
					if(gEMR_Past[2] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[2]);
					}
					else
						goto _PastCarRetry_CW;
				}
				else if(cError_Index == 4)
				{
					if(gEMR_Past[3] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[3]);
					}
					else
						goto _PastCarRetry_CW;
				}
				else if(cError_Index == 5)
				{
					if(gEMR_Past[4] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[4]);
					}
					else
						goto _PastCarRetry_CW;
				}
			}
			break;

			case PastEngineCode:
			{
				//M8_PastEngSelectFree();
				ituWidgetSetVisible(PastEngErrCode, false);
				ituWidgetSetVisible(PastEngErrCodeDetail, false);

_PastEngRetry_CW:
				if((cError_Index > 0) && (cError_Index < 5))
				{
					cError_Index++;

					if(cError_Index == 5)
						cError_Index = 1;
				}

				if(cError_Index == 1)
				{
					if(gSPN_Past[0] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[0],gFMI_Past[0]);
					}
					else
						goto _PastEngRetry_CW;
				}
				else if(cError_Index == 2)
				{
					if(gSPN_Past[1] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[1],gFMI_Past[1]);
					}
					else
						goto _PastEngRetry_CW;
				}
				else if(cError_Index == 3)
				{
					if(gSPN_Past[2] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[2],gFMI_Past[2]);
					}
					else
						goto _PastEngRetry_CW;
				}
				else if(cError_Index == 4)
				{
					if(gSPN_Past[3] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[3],gFMI_Past[3]);
					}
					else
						goto _PastEngRetry_CW;
				}
			}
			break;
		}
	}

    return true;
}

bool Menu8_JogButton(ITUWidget* widget, char* param)
{
	char str[SumString_Size];

	uint8_t tJogButton = 0;
	
	M8_ExitCNT = 0;
	tJogButton = param[0];
	
	printf("M8 JogButton = %d\r\n",tJogButton);

	if((tJogButton == JOG_DIAL_KEY_UP)||(tJogButton == JOG_DIAL_KEY_RIGHT))
	{
		switch(cPage_Index)
		{
			case EngineErrorCode:
			{
				//M8_EngSelectFree();
				ituWidgetSetVisible(EngErrCode, false);
				ituWidgetSetVisible(EngErrCodeDetail, false);

_EngRetry_CCW1:
				if(cError_Index > 0)
				{
					cError_Index--;

					if(cError_Index == 0)
						cError_Index = 7;
				}
					
				if(cError_Index == 1)
				{
					if(gEMR[0] > 0)
					{
						CarErrorMSG_Display(gEMR[0]);
					}
					else
						goto _EngRetry_CCW1;
				}
				else if(cError_Index == 2)
				{
					if(gEMR[1] > 0)
					{
						CarErrorMSG_Display(gEMR[1]);
					}
					else
						goto _EngRetry_CCW1;
				}
				else if(cError_Index == 3)
				{
					if(gEMR[2] > 0)
					{
						CarErrorMSG_Display(gEMR[2]);
					}
					else
						goto _EngRetry_CCW1;
				}
				else if(cError_Index == 4)
				{
					if(gEMR[3] > 0)
					{
						CarErrorMSG_Display(gEMR[3]);
					}
					else
						goto _EngRetry_CCW1;
				}
				else if(cError_Index == 5)
				{
					if(gSPN[0] > 0)
					{
						EngErrorMSG_Display(gSPN[0],gFMI[0]);
					}
					else
						goto _EngRetry_CCW1;
				}
				else if(cError_Index == 6)
				{
					if(gSPN[1] > 0)
					{
						EngErrorMSG_Display(gSPN[1],gFMI[1]);
					}
					else
						goto _EngRetry_CCW1;
				}
				else if(cError_Index == 7)
				{
					if(gSPN[2] > 0)
					{
						EngErrorMSG_Display(gSPN[2],gFMI[2]);
					}
					else
						goto _EngRetry_CCW1;
				}
			}
			break;

			case AirConErrorCode:
			{
				//M8_ACSelectFree();
				ituWidgetSetVisible(ACErrCode, false);
				ituWidgetSetVisible(ACErrCodeDetail, false);

_ACRetry_CCW1:
				if(cError_Index > 0)
				{
					cError_Index--;

					if(cError_Index == 0)
						cError_Index = 8;
				}
					
				if(cError_Index == 1)
				{
					if(gACERR_Code[0] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[0]);
					}
					else
						goto _ACRetry_CCW1;
				}
				else if(cError_Index == 2)
				{
					if(gACERR_Code[1] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[1]);
					}
					else
						goto _ACRetry_CCW1;
				}
				else if(cError_Index == 3)
				{
					if(gACERR_Code[2] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[2]);
					}
					else
						goto _ACRetry_CCW1;
				}
				else if(cError_Index == 4)
				{
					if(gACERR_Code[3] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[3]);
					}
					else
						goto _ACRetry_CCW1;
				}
				else if(cError_Index == 5)
				{
					if(gACERR_Code[4] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[4]);
					}
					else
						goto _ACRetry_CCW1;
				}
				else if(cError_Index == 6)
				{
					if(gACERR_Code[5] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[5]);
					}
					else
						goto _ACRetry_CCW1;
				}
				else if(cError_Index == 7)
				{
					if(gACERR_Code[6] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[6]);
					}
					else
						goto _ACRetry_CCW1;
				}
				else if(cError_Index == 8)
				{
					if(gACERR_Code[7] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[7]);
					}
					else
						goto _ACRetry_CCW1;
				}
			}
			break;
			
			case PastCarCode:
			{
				//M8_PastCarSelectFree();
				ituWidgetSetVisible(PastCarErrCode, false);
				ituWidgetSetVisible(PastCarErrCodeDetail, false);

_PastCarRetry_CCW1:
				if(cError_Index > 0)
				{
					cError_Index--;

					if(cError_Index == 0)
						cError_Index = 5;
				}

				if(cError_Index == 1)
				{
					if(gEMR_Past[0] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[0]);
					}
					else
						goto _PastCarRetry_CCW1;
				}
				else if(cError_Index == 2)
				{
					if(gEMR_Past[1] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[1]);
					}
					else
						goto _PastCarRetry_CCW1;
				}
				else if(cError_Index == 3)
				{
					if(gEMR_Past[2] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[2]);
					}
					else
						goto _PastCarRetry_CCW1;
				}
				else if(cError_Index == 4)
				{
					if(gEMR_Past[3] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[3]);
					}
					else
						goto _PastCarRetry_CCW1;
				}
				else if(cError_Index == 5)
				{
					if(gEMR_Past[4] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[4]);
					}
					else
						goto _PastCarRetry_CCW1;
				}
			}
			break;
			
			case PastEngineCode:
			{
				//M8_PastEngSelectFree();
				ituWidgetSetVisible(PastEngErrCode, false);
				ituWidgetSetVisible(PastEngErrCodeDetail, false);

_PastEngRetry_CCW1:
				if(cError_Index > 0)
				{
					cError_Index--;

					if(cError_Index == 0)
						cError_Index = 4;
				}

				if(cError_Index == 1)
				{
					if(gSPN_Past[0] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[0],gFMI_Past[0]);
					}
					else
						goto _PastEngRetry_CCW1;
				}
				else if(cError_Index == 2)
				{
					if(gSPN_Past[1] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[1],gFMI_Past[1]);
					}
					else
						goto _PastEngRetry_CCW1;
				}
				else if(cError_Index == 3)
				{
					if(gSPN_Past[2] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[2],gFMI_Past[2]);
					}
					else
						goto _PastEngRetry_CCW1;
				}
				else if(cError_Index == 4)
				{
					if(gSPN_Past[3] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[3],gFMI_Past[3]);
					}
					else
						goto _PastEngRetry_CCW1;
				}
			}
			break;
		}
	}
	else if((tJogButton == JOG_DIAL_KEY_DOWN)||(tJogButton == JOG_DIAL_KEY_LEFT))
	{
		switch(cPage_Index)
		{
			case EngineErrorCode:
			{
				//M8_EngSelectFree();
				ituWidgetSetVisible(EngErrCode, false);
				ituWidgetSetVisible(EngErrCodeDetail, false);

_EngRetry_CW1:
				if((cError_Index > 0) && (cError_Index < 8))
				{
					cError_Index++;

					if(cError_Index == 8)
						cError_Index = 1;
				}
					
				if(cError_Index == 1)
				{
					if(gEMR[0] > 0)
					{
						CarErrorMSG_Display(gEMR[0]);
					}
					else
						goto _EngRetry_CW1;
				}
				else if(cError_Index == 2)
				{
					if(gEMR[1] > 0)
					{
						CarErrorMSG_Display(gEMR[1]);
					}
					else
						goto _EngRetry_CW1;
				}
				else if(cError_Index == 3)
				{
					if(gEMR[2] > 0)
					{
						CarErrorMSG_Display(gEMR[2]);
					}
					else
						goto _EngRetry_CW1;
				}
				else if(cError_Index == 4)
				{
					if(gEMR[3] > 0)
					{
						CarErrorMSG_Display(gEMR[3]);
					}
					else
						goto _EngRetry_CW1;
				}
				else if(cError_Index == 5)
				{
					if(gSPN[0] > 0)
					{
						EngErrorMSG_Display(gSPN[0],gFMI[0]);
					}
					else
						goto _EngRetry_CW1;
				}
				else if(cError_Index == 6)
				{
					if(gSPN[1] > 0)
					{
						EngErrorMSG_Display(gSPN[1],gFMI[1]);
					}
					else
						goto _EngRetry_CW1;
				}
				else if(cError_Index == 7)
				{
					if(gSPN[2] > 0)
					{
						EngErrorMSG_Display(gSPN[2],gFMI[2]);
					}
					else
						goto _EngRetry_CW1;
				}
			}
			break;

			case AirConErrorCode:
			{
				//M8_ACSelectFree();
				ituWidgetSetVisible(ACErrCode, false);
				ituWidgetSetVisible(ACErrCodeDetail, false);

_ACRetry_CW1:
				if((cError_Index > 0) && (cError_Index < 9))
				{
					cError_Index++;
				
					if(cError_Index == 9)
						cError_Index = 1;
				}
					
				if(cError_Index == 1)
				{
					if(gACERR_Code[0] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[0]);
					}
					else
						goto _ACRetry_CW1;
				}
				else if(cError_Index == 2)
				{
					if(gACERR_Code[1] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[1]);
					}
					else
						goto _ACRetry_CW1;
				}
				else if(cError_Index == 3)
				{
					if(gACERR_Code[2] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[2]);
					}
					else
						goto _ACRetry_CW1;
				}
				else if(cError_Index == 4)
				{
					if(gACERR_Code[3] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[3]);
					}
					else
						goto _ACRetry_CW1;
				}
				else if(cError_Index == 5)
				{
					if(gACERR_Code[4] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[4]);
					}
					else
						goto _ACRetry_CW1;
				}
				else if(cError_Index == 6)
				{
					if(gACERR_Code[5] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[5]);
					}
					else
						goto _ACRetry_CW1;
				}
				else if(cError_Index == 7)
				{
					if(gACERR_Code[6] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[6]);
					}
					else
						goto _ACRetry_CW1;
				}
				else if(cError_Index == 8)
				{
					if(gACERR_Code[7] > 0)
					{
						ACErrorMSG_Display(gACERR_Code[7]);
					}
					else
						goto _ACRetry_CW1;
				}
			}
			break;

			case PastCarCode:
			{
				//M8_PastCarSelectFree();
				ituWidgetSetVisible(PastCarErrCode, false);
				ituWidgetSetVisible(PastCarErrCodeDetail, false);

_PastCarRetry_CW1:
				if((cError_Index > 0) && (cError_Index < 6))
				{
					cError_Index++;

					if(cError_Index == 6)
						cError_Index = 1;
				}

				if(cError_Index == 1)
				{
					if(gEMR_Past[0] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[0]);
					}
					else
						goto _PastCarRetry_CW1;
				}
				else if(cError_Index == 2)
				{
					if(gEMR_Past[1] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[1]);
					}
					else
						goto _PastCarRetry_CW1;
				}
				else if(cError_Index == 3)
				{
					if(gEMR_Past[2] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[2]);
					}
					else
						goto _PastCarRetry_CW1;
				}
				else if(cError_Index == 4)
				{
					if(gEMR_Past[3] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[3]);
					}
					else
						goto _PastCarRetry_CW1;
				}
				else if(cError_Index == 5)
				{
					if(gEMR_Past[4] > 0)
					{
						PastCarErrorMSG_Display(gEMR_Past[4]);
					}
					else
						goto _PastCarRetry_CW1;
				}
			}
			break;

			case PastEngineCode:
			{
				//M8_PastEngSelectFree();
				ituWidgetSetVisible(PastEngErrCode, false);
				ituWidgetSetVisible(PastEngErrCodeDetail, false);

_PastEngRetry_CW1:
				if((cError_Index > 0) && (cError_Index < 5))
				{
					cError_Index++;

					if(cError_Index == 5)
						cError_Index = 1;
				}

				if(cError_Index == 1)
				{
					if(gSPN_Past[0] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[0],gFMI_Past[0]);
					}
					else
						goto _PastEngRetry_CW1;
				}
				else if(cError_Index == 2)
				{
					if(gSPN_Past[1] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[1],gFMI_Past[1]);
					}
					else
						goto _PastEngRetry_CW1;
				}
				else if(cError_Index == 3)
				{
					if(gSPN_Past[2] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[2],gFMI_Past[2]); //lim.24.04.15
					}
					else
						goto _PastEngRetry_CW1;
				}
				else if(cError_Index == 4)
				{
					if(gSPN_Past[3] > 0)
					{
						PastEngErrorMSG_Display(gSPN_Past[3],gFMI_Past[3]);
					}
					else
						goto _PastEngRetry_CW1;
				}
			}
			break;
		}
	}
	else if(tJogButton == JOG_DIAL_KEY_ENTER_ON)
	{
		M8D7_MouseUp(NULL, NULL);
	}

    return true;
}

bool Menu8_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t tJogSwitch = 0;
	
	M8_ExitCNT = 0;
	tJogSwitch = param[0];
	
	printf("M8 JogSwitch = %d\r\n",tJogSwitch);

	if(tJogSwitch == JOG_DIAL_SW_ESC_ON)	//JPHong 2024-03-22
	{
		//GotoMenuTop();
	}
	else if(tJogSwitch == JOG_DIAL_SW_HOME)
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}
	else if(tJogSwitch == JOG_DIAL_SW_ESC_ON_OFF)	//JPHong 2024-03-22
	{
		GotoMenuTop();
	}
    return true;
}

bool M8D7_MouseUp(ITUWidget* widget, char* param)
{
	char str[SumString_Size];

	uint8_t tPageIdx;

	M8_ExitCNT = 0;	
	tPageIdx = cPage_Index;

	// change bg
	cPage_Index++;
	if(cPage_Index > PastEngineCode)
		cPage_Index = EngineErrorCode;

	EngError_MakeDisplayData();
//	PastEngError_MakeDisplayData();
	
	M8_Change_Page(tPageIdx,cPage_Index);

	switch(cPage_Index)
	{
		case EngineErrorCode:
		{
			cError_Index = 0;

			ituWidgetSetVisible(EngErrCode, false);
			ituWidgetSetVisible(EngErrCodeDetail, false);

			if(gEMR[0] > 0)
			{
				cError_Index = 1;
				CarErrorMSG_Display(gEMR[0]);
			}
			else if(gEMR[1] > 0)
			{
				cError_Index = 2;
				CarErrorMSG_Display(gEMR[1]);
			}
			else if(gEMR[2] > 0)
			{
				cError_Index = 3;
				CarErrorMSG_Display(gEMR[2]);
			}
			else if(gEMR[3] > 0)
			{
				cError_Index = 4;
				CarErrorMSG_Display(gEMR[3]);
			}
			else if(gSPN[0] > 0)
			{
				cError_Index = 5;
				EngErrorMSG_Display(gSPN[0],gFMI[0]);
			}
			else if(gSPN[1] > 0)
			{
				cError_Index = 6;
				EngErrorMSG_Display(gSPN[1],gFMI[1]);
			}
			else if(gSPN[2] > 0)
			{
				cError_Index = 7;
				EngErrorMSG_Display(gSPN[2],gFMI[2]);
			}
		}
		break;
		
		case AirConErrorCode:
		{
			cError_Index = 0;

			ituWidgetSetVisible(ACErrCode, false);
			ituWidgetSetVisible(ACErrCodeDetail, false);
			
			if(gACERR_Code[0] > 0)
			{
				cError_Index = 1;
				ACErrorMSG_Display(gACERR_Code[0]);
			}
			else if(gACERR_Code[1] > 0)
			{
				cError_Index = 2;
				ACErrorMSG_Display(gACERR_Code[1]);
			}
			else if(gACERR_Code[2] > 0)
			{
				cError_Index = 3;
				ACErrorMSG_Display(gACERR_Code[2]);
			}
			else if(gACERR_Code[3] > 0)
			{
				cError_Index = 4;
				ACErrorMSG_Display(gACERR_Code[3]);
			}
			else if(gACERR_Code[4] > 0)
			{
				cError_Index = 5;
				ACErrorMSG_Display(gACERR_Code[4]);
			}
			else if(gACERR_Code[5] > 0)
			{
				cError_Index = 6;
				ACErrorMSG_Display(gACERR_Code[5]);
			}
			else if(gACERR_Code[6] > 0)
			{
				cError_Index = 7;
				ACErrorMSG_Display(gACERR_Code[6]);
			}
			else if(gACERR_Code[7] > 0)
			{
				cError_Index = 8;
				ACErrorMSG_Display(gACERR_Code[7]);
			}
		}
		break;
		
		case PastCarCode:
		{
			cError_Index = 0;

			ituWidgetSetVisible(PastCarErrCode, false);
			ituWidgetSetVisible(PastCarErrCodeDetail, false);
			
			if(gEMR_Past[0] > 0)
			{
				cError_Index = 1;
				PastCarErrorMSG_Display(gEMR_Past[0]);
			}
			else if(gEMR_Past[1] > 0)
			{
				cError_Index = 2;
				PastCarErrorMSG_Display(gEMR_Past[1]);
			}
			else if(gEMR_Past[2] > 0)
			{
				cError_Index = 3;
				PastCarErrorMSG_Display(gEMR_Past[2]);
			}
			else if(gEMR_Past[3] > 0)
			{
				cError_Index = 4;
				PastCarErrorMSG_Display(gEMR_Past[3]);
			}
			else if(gEMR_Past[4] > 0)
			{
				cError_Index = 5;
				PastCarErrorMSG_Display(gEMR_Past[4]);
			}
		}
		break;
		
		case PastEngineCode:
		{
			cError_Index = 0;

			ituWidgetSetVisible(PastEngErrCode, false);
			ituWidgetSetVisible(PastEngErrCodeDetail, false);
			
			if(gSPN_Past[0] > 0)
			{
				cError_Index = 1;
				PastEngErrorMSG_Display(gSPN_Past[0],gFMI_Past[0]);
			}
			else if(gSPN_Past[1] > 0)
			{
				cError_Index = 2;
				PastEngErrorMSG_Display(gSPN_Past[1],gFMI_Past[1]);
			}
			else if(gSPN_Past[2] > 0)
			{
				cError_Index = 3;
				PastEngErrorMSG_Display(gSPN_Past[2],gFMI_Past[2]);
			}
			else if(gSPN_Past[3] > 0)
			{
				cError_Index = 4;
				PastEngErrorMSG_Display(gSPN_Past[3],gFMI_Past[3]);
			}
		}
		break;
	}
	
    return true;
}

static uint16_t ErCode = 0;

bool M8D6_MouseUp(ITUWidget* widget, char* param)
{
#ifdef TEST_ERROR_MSG_VIEW
	char str[SumString_Size];
	
	if(cPage_Index == EngineErrorCode)
	{
		if(ErCode == 0)
		 	ErCode = 307;
		else
			ErCode--;

		if(Eng_SPNCode[ErCode] > 0)
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				Get_EngErrorMSG2(str,Eng_SPNCode[ErCode],Eng_FMICode[ErCode]);
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				Get_EngErrorMSG3(str,Eng_SPNCode[ErCode],Eng_FMICode[ErCode]);
      
      printf("Get_EngErrorMSG:%s\r\n",str);
      
			EngErrCode_TB(Eng_SPNCode[ErCode],Eng_FMICode[ErCode]);
			ituWidgetSetVisible(EngErrCode, true);
      
			ituTextBoxSetString(EngErrCodeDetail, str);
      
			ituWidgetSetVisible(EngErrCodeDetail, true);
		}
	}
	else if(cPage_Index == AirConErrorCode)
	{
		if(ErCode == 0)
		 	ErCode = 9;
		else
			ErCode--;

		if(AC_ErCode[ErCode] > 0)
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				Get_ACErrorMSG2(str,AC_ErCode[ErCode]);
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				Get_ACErrorMSG3(str,AC_ErCode[ErCode]);
      
      printf("Get_ACErrorMSG:%s\r\n",str);
      
			ACErrCode_TB(AC_ErCode[ErCode]);
			ituWidgetSetVisible(ACErrCode, true);
      
			ituTextBoxSetString(ACErrCodeDetail, str);
      
			ituWidgetSetVisible(ACErrCodeDetail, true);
		}
	}
	else if(cPage_Index == PastCarCode)
	{
		if(ErCode == 0)
		 	ErCode = 87;
		else
			ErCode--;

		if(Car_ErCode[ErCode] > 0)
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				Get_CarErrorMSG2(str,Car_ErCode[ErCode]);
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				Get_CarErrorMSG3(str,Car_ErCode[ErCode]);
      
      printf("Get_CarErrorMSG:%s\r\n",str);
      
			PastCarErrCode_TB(Car_ErCode[ErCode]);
			ituWidgetSetVisible(PastCarErrCode, true);
      
			ituTextBoxSetString(PastCarErrCodeDetail, str);
      
			ituWidgetSetVisible(PastCarErrCodeDetail, true);
		}
	}
	else if(cPage_Index == PastEngineCode)
	{
		if(ErCode == 0)
		 	ErCode = 307;
		else
			ErCode--;

		if(Eng_SPNCode[ErCode] > 0)
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				Get_EngErrorMSG2(str,Eng_SPNCode[ErCode],Eng_FMICode[ErCode]);
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				Get_EngErrorMSG3(str,Eng_SPNCode[ErCode],Eng_FMICode[ErCode]);
      
      printf("Get_EngErrorMSG:%s\r\n",str);
      
			PastEngErrCode_TB(Eng_SPNCode[ErCode],Eng_FMICode[ErCode]);
			ituWidgetSetVisible(PastEngErrCode, true);
      
			ituTextBoxSetString(PastEngErrCodeDetail, str);
      
			ituWidgetSetVisible(PastEngErrCodeDetail, true);
		}
	}
	else
#endif	
		printf("[M8D6]Go Main Menu..1\n");
		GotoMain(MAIN_MODE_NORMAL);
    return true;
}

bool M8D5_MouseUp(ITUWidget* widget, char* param)
{
#ifdef TEST_ERROR_MSG_VIEW
	char str[SumString_Size];
	
	if(cPage_Index == EngineErrorCode)
	{
		ErCode++;
		if(ErCode >= 308)
			ErCode = 0;
		
		if(Eng_SPNCode[ErCode] > 0)
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				Get_EngErrorMSG2(str,Eng_SPNCode[ErCode],Eng_FMICode[ErCode]);
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				Get_EngErrorMSG3(str,Eng_SPNCode[ErCode],Eng_FMICode[ErCode]);
      
      printf("Get_EngErrorMSG2:%s\r\n",str);
      
			EngErrCode_TB(Eng_SPNCode[ErCode],Eng_FMICode[ErCode]);
			ituWidgetSetVisible(EngErrCode, true);
      
			ituTextBoxSetString(EngErrCodeDetail, str);
      
			ituWidgetSetVisible(EngErrCodeDetail, true);
		}
	}
	else if(cPage_Index == AirConErrorCode)
	{
		ErCode++;
		if(ErCode >= 10)
			ErCode = 0;
		
		if(AC_ErCode[ErCode] > 0)
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				Get_ACErrorMSG2(str,AC_ErCode[ErCode]);
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				Get_ACErrorMSG3(str,AC_ErCode[ErCode]);
      
      printf("Get_ACErrorMSG:%s\r\n",str);
      
			ACErrCode_TB(AC_ErCode[ErCode]);
			ituWidgetSetVisible(ACErrCode, true);
      
			ituTextBoxSetString(ACErrCodeDetail, str);
      
			ituWidgetSetVisible(ACErrCodeDetail, true);
		}
	}
	else if(cPage_Index == PastCarCode)
	{
		ErCode++;
		if(ErCode >= 88)
			ErCode = 0;
		
		if(Car_ErCode[ErCode] > 0)
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				Get_CarErrorMSG2(str,Car_ErCode[ErCode]);
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				Get_CarErrorMSG3(str,Car_ErCode[ErCode]);
      
      printf("Get_CarErrorMSG:%s\r\n",str);
      
			PastCarErrCode_TB(Car_ErCode[ErCode]);
			ituWidgetSetVisible(PastCarErrCode, true);
      
			ituTextBoxSetString(PastCarErrCodeDetail, str);
      
			ituWidgetSetVisible(PastCarErrCodeDetail, true);
		}
	}
	else if(cPage_Index == PastEngineCode)
	{
		ErCode++;
		if(ErCode >= 308)
			ErCode = 0;
		
		if(Eng_SPNCode[ErCode] > 0)
		{
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				Get_EngErrorMSG2(str,Eng_SPNCode[ErCode],Eng_FMICode[ErCode]);
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				Get_EngErrorMSG3(str,Eng_SPNCode[ErCode],Eng_FMICode[ErCode]);
      
      printf("Get_EngErrorMSG2:%s\r\n",str);
      
			PastEngErrCode_TB(Eng_SPNCode[ErCode],Eng_FMICode[ErCode]);
			ituWidgetSetVisible(PastEngErrCode, true);
      
			ituTextBoxSetString(PastEngErrCodeDetail, str);
      
			ituWidgetSetVisible(PastEngErrCodeDetail, true);
		}
	}
	else
#endif	

		GotoMenuTop();
	
    return true;
}

bool M8_UserDisplay(void)
{
	uint8_t tDestName[100];

	//chpark.24.11.14
	if(gMenu_UI_Time == 0 || g_nM7_StartType != M7_MODE_PUSH)	// JPHong 2024-03-20
		return 0;

	ITUWidget *M8_TopAdmin = ituSceneFindWidget(&theScene, "M8_TopAdmin");
	assert(M8_TopAdmin);
	ITUWidget *M8_TopStandard = ituSceneFindWidget(&theScene, "M8_TopStandard");
	assert(M8_TopStandard);
	
	ITUText *M8_TopUserName = ituSceneFindWidget(&theScene, "M8_TopUserName");
	assert(M8_TopUserName);

	//printf("M8_UserDisplay User=%d , Lv=%d\r\n",M7_UserIndex,User_RegInfo[M7_UserIndex].RegFlag);

	gM8_UserFlag = User_RegInfo[M7_UserIndex].RegFlag;
	
	if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
	{
		ituWidgetSetVisible(M8_TopStandard,false);
		ituWidgetSetVisible(M8_TopAdmin,true);
	}
	else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
	{
		ituWidgetSetVisible(M8_TopAdmin,false);
		ituWidgetSetVisible(M8_TopStandard,true);
	}
	else
	{
		ituWidgetSetVisible(M8_TopAdmin,false);
		ituWidgetSetVisible(M8_TopStandard,false);
	}

	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M8_TopUserName, tDestName);

		ituWidgetSetVisible(M8_TopUserName,true);
	}
	else
	{
		ituWidgetSetVisible(M8_TopUserName,false);
	}
/*
	if((gMainTimerCnt_10m - gMenu_UI_Time) < 500)		// 5sec	
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M8_TopUserName, tDestName);

		ituWidgetSetVisible(M8_TopUserName,true);
	}
	else if(((gMainTimerCnt_10m - gMenu_UI_Time) >= 500)||(M7_UserIndex > (MAX_SU100_USER_NUM-1)))
	{
		ituWidgetSetVisible(M8_TopUserName,false);
	}
*/
}

void ACErrorMSG_Display(uint8_t AC_ErrorCode)
{
//	uint8_t ACErr_Idx=0xff;

	char AC_ErMSG[SumString_Size];
	
	if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
		Get_ACErrorMSG2(AC_ErMSG,AC_ErrorCode);
	else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
		Get_ACErrorMSG3(AC_ErMSG,AC_ErrorCode);
		
	ACErrCode_TB(AC_ErrorCode);
	ituWidgetSetVisible(ACErrCode, true);
      
	ituTextBoxSetString(ACErrCodeDetail, AC_ErMSG);
	ituWidgetSetVisible(ACErrCodeDetail, true);
}

void CarErrorMSG_Display(uint16_t Car_ErrorCode)
{
//	uint8_t CaeErr_Idx=0xff;

	char Car_ErMSG[SumString_Size];
	
	if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
		Get_CarErrorMSG2(Car_ErMSG,Car_ErrorCode);
	else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
		Get_CarErrorMSG3(Car_ErMSG,Car_ErrorCode);
		
	CarErrCode_TB(Car_ErrorCode);
	ituWidgetSetVisible(EngErrCode, true);
      
	ituTextBoxSetString(EngErrCodeDetail, Car_ErMSG);
	ituWidgetSetVisible(EngErrCodeDetail, true);
}

void PastCarErrorMSG_Display(uint16_t Car_ErrorCode)
{
//	uint8_t CaeErr_Idx=0xff;

	char Car_ErMSG[SumString_Size];
	
	if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
		Get_CarErrorMSG2(Car_ErMSG,Car_ErrorCode);
	else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
		Get_CarErrorMSG3(Car_ErMSG,Car_ErrorCode);
		
	PastCarErrCode_TB(Car_ErrorCode);
	ituWidgetSetVisible(PastCarErrCode, true);
      
	ituTextBoxSetString(PastCarErrCodeDetail, Car_ErMSG);
	ituWidgetSetVisible(PastCarErrCodeDetail, true);
}

void EngErrorMSG_Display(uint32_t Eng_SPNCode,uint8_t Eng_FMICode)
{
	char Eng_ErMSG[SumString_Size];

	if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
		Get_EngErrorMSG2(Eng_ErMSG,Eng_SPNCode,Eng_FMICode);
	else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
		Get_EngErrorMSG3(Eng_ErMSG,Eng_SPNCode,Eng_FMICode);
	
	EngErrCode_TB(Eng_SPNCode,Eng_FMICode);
	ituWidgetSetVisible(EngErrCode, true);
      
	ituTextBoxSetString(EngErrCodeDetail, Eng_ErMSG);
	ituWidgetSetVisible(EngErrCodeDetail, true);
}

void PastEngErrorMSG_Display(uint32_t Eng_SPNCode,uint8_t Eng_FMICode)
{
	char Eng_ErMSG[SumString_Size];

	if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
		Get_EngErrorMSG2(Eng_ErMSG,Eng_SPNCode,Eng_FMICode);
	else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
		Get_EngErrorMSG3(Eng_ErMSG,Eng_SPNCode,Eng_FMICode);
	
	PastEngErrCode_TB(Eng_SPNCode,Eng_FMICode);
	ituWidgetSetVisible(PastEngErrCode, true);
      
	ituTextBoxSetString(PastEngErrCodeDetail, Eng_ErMSG);
	ituWidgetSetVisible(PastEngErrCodeDetail, true);
}

bool PastEngErr_MouseUp(ITUWidget* widget, char* param)
{
	char str[SumString_Size];
	
	uint8_t tEngErrCode = 0;
	
	M8_ExitCNT = 0;
	tEngErrCode = (uint8_t)atoi(param);

	PastEngBTN_PressDisp(tEngErrCode,false);

	switch(tEngErrCode)
	{
		case 1:
			if(gSPN_Past[0] > 0)
			{
				PastEngErrorMSG_Display(gSPN_Past[0],gFMI_Past[0]);
			}
		break;

		case 2:
			if(gSPN_Past[1] > 0)
			{
				PastEngErrorMSG_Display(gSPN_Past[1],gFMI_Past[1]);
			}
		break;

		case 3:
			if(gSPN_Past[2] > 0)
			{
				PastEngErrorMSG_Display(gSPN_Past[2],gFMI_Past[2]);
			}
		break;

		case 4:
			if(gSPN_Past[3] > 0)
			{
				PastEngErrorMSG_Display(gSPN_Past[3],gFMI_Past[3]);
			}
		break;

	}
	
    return true;
}

bool PastEngErr_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tEngErrCode = 0;
	
	M8_ExitCNT = 0;
	tEngErrCode = (uint8_t)atoi(param);

	PastEngBTN_PressDisp(tEngErrCode,true);

	switch(tEngErrCode)
	{
		case 1:
			if(gSPN_Past[0] > 0)
			{
				cError_Index = 1;
			}
		break;

		case 2:
			if(gSPN_Past[1] > 0)
			{
				cError_Index = 2;
			}
		break;

		case 3:
			if(gSPN_Past[2] > 0)
			{
				cError_Index = 3;
			}
		break;

		case 4:
			if(gSPN_Past[3] > 0)
			{
				cError_Index = 4;
			}
		break;
	}

    return true;
}

bool PastCarErr_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tCarErrCode = 0;
	
	M8_ExitCNT = 0;
	tCarErrCode = (uint8_t)atoi(param);

	PastCarBTN_PressDisp(tCarErrCode,false);
	printf("@@@@@@ PastCarErr_MouseUp enter @@@@@@\n");
	switch(tCarErrCode)
	{
		case 1:
			if(gEMR_Past[0] > 0)
			{
				PastCarErrorMSG_Display(gEMR_Past[0]);
			}
		break;
		
		case 2:
			if(gEMR_Past[1] > 0)
			{
				PastCarErrorMSG_Display(gEMR_Past[1]);
			}
		break;

		case 3:
			if(gEMR_Past[2] > 0)
			{
				PastCarErrorMSG_Display(gEMR_Past[2]);
			}
		break;

		case 4:
			if(gEMR_Past[3] > 0)
			{
				PastCarErrorMSG_Display(gEMR_Past[3]);
			}
		break;

		case 5:
			if(gEMR_Past[4] > 0)
			{
				PastCarErrorMSG_Display(gEMR_Past[4]);
			}
		break;
	}
    return true;
}

bool PastCarErr_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tCarErrCode = 0;
	
	M8_ExitCNT = 0;
	tCarErrCode = (uint8_t)atoi(param);

	PastCarBTN_PressDisp(tCarErrCode,true);

	switch(tCarErrCode)
	{
		case 1:
			if(gEMR_Past[0] > 0)
			{
				cError_Index = 1;
			}
		break;
		
		case 2:
			if(gEMR_Past[1] > 0)
			{
				cError_Index = 2;
			}
		break;

		case 3:
			if(gEMR_Past[2] > 0)
			{
				cError_Index = 3;
			}
		break;

		case 4:
			if(gEMR_Past[3] > 0)
			{
				cError_Index = 4;
			}
		break;

		case 5:
			if(gEMR_Past[4] > 0)
			{
				cError_Index = 5;
			}
		break;
	}

    return true;
}

bool ACErr_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tACErrIdx = 0;
	
	M8_ExitCNT = 0;	
	tACErrIdx = (uint8_t)atoi(param);

	//printf("ACErr_MouseUp[%d];ErCode[%d]\r\n" ,tACErrIdx,gACERR_Code[tACErrIdx-1]);

	if(gACERR_Code[tACErrIdx-1] > 0)
	{
		ACErrorMSG_Display(gACERR_Code[tACErrIdx-1]);
	}

/*
	switch(tACErrIdx)
	{
		case 1:
			if(gACERR_Code[0] > 0)
			{
				ACErrorMSG_Display(gACERR_Code[0]);
			}
		break;

		case 2:
			if(gACERR_Code[1] > 0)
			{
				ACErrorMSG_Display(gACERR_Code[1]);
			}
		break;

		case 3:
			if(gACERR_Code[2] > 0)
			{
				ACErrorMSG_Display(gACERR_Code[2]);
			}
		break;

		case 4:
			if(gACERR_Code[3] > 0)
			{
				ACErrorMSG_Display(gACERR_Code[3]);
			}
		break;

		case 5:
			if(gACERR_Code[4] > 0)
			{
				ACErrorMSG_Display(gACERR_Code[4]);
			}
		break;

		case 6:
			if(gACERR_Code[5] > 0)
			{
				ACErrorMSG_Display(gACERR_Code[5]);
			}
		break;

		case 7:
			if(gACERR_Code[6] > 0)
			{
				ACErrorMSG_Display(gACERR_Code[6]);
			}
		break;

		case 8:
			if(gACERR_Code[7] > 0)
			{
				ACErrorMSG_Display(gACERR_Code[7]);
			}
		break;
	}
*/	
    return true;
}

bool ACErr_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tACErrIdx = 0;
	
	M8_ExitCNT = 0;	
	tACErrIdx = (uint8_t)atoi(param);

	//printf("ACErr_MouseDn[%d];ErCode[%d]\r\n" ,tACErrIdx,gACERR_Code[tACErrIdx-1]);

	if(gACERR_Code[tACErrIdx-1] > 0)
	{
		cError_Index = tACErrIdx;
	}

/*	
	switch(tACErrIdx)
	{
		case 1:
			if(gACERR_Code[0] > 0)
			{
				cError_Index = 1;
			}
		break;
		
		case 2:
			if(gACERR_Code[1] > 0)
			{
				cError_Index = 2;
			}
		break;

		case 3:
			if(gACERR_Code[2] > 0)
			{
				cError_Index = 3;
			}
		break;

		case 4:
			if(gACERR_Code[3] > 0)
			{
				cError_Index = 4;
			}
		break;

		case 5:
			if(gACERR_Code[4] > 0)
			{
				cError_Index = 5;
			}
		break;

		case 6:
			if(gACERR_Code[5] > 0)
			{
				cError_Index = 6;
			}
		break;

		case 7:
			if(gACERR_Code[6] > 0)
			{
				cError_Index = 7;
			}
		break;

		case 8:
			if(gACERR_Code[7] > 0)
			{
				cError_Index = 8;
			}
		break;
	}
*/	
    return true;
}

bool EngErr_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tBTNIdx;
	
	M8_ExitCNT = 0;	
	tBTNIdx = (uint8_t)atoi(param);
	
	if(gSPN[tBTNIdx-1] > 0)
	{
		EngErrorMSG_Display(gSPN[tBTNIdx-1],gFMI[tBTNIdx-1]);	
	}

/*
	switch(tBTNIdx)
	{
		case 1:
			if(gSPN[0] > 0)
			{
				EngErrorMSG_Display(gSPN[0],gFMI[0]);	
			}
		break;

		case 2:
			if(gSPN[1] > 0)
			{
				EngErrorMSG_Display(gSPN[1],gFMI[1]);	
			}
		break;

		case 3:
			if(gSPN[2] > 0)
			{
				EngErrorMSG_Display(gSPN[2],gFMI[2]);	
			}
		break;
	}
*/	
    return true;
}

bool EngErr_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tBTNIdx;
	
	M8_ExitCNT = 0;	
	tBTNIdx = (uint8_t)atoi(param);

	if(gSPN[tBTNIdx-1] > 0)
	{
		cError_Index = tBTNIdx+4;
	}

/*
	switch(tBTNIdx)
	{
		case 1:
			if(gSPN[0] > 0)
			{
				cError_Index = 5;
			}
		break;
		
		case 2:
			if(gSPN[1] > 0)
			{
				cError_Index = 6;
			}
		break;
		
		case 3:
			if(gSPN[2] > 0)
			{
				cError_Index = 7;
			}
		break;
	}
*/
    return true;
}

bool CarErr_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tBTNIdx;

	M8_ExitCNT = 0;	
	tBTNIdx = (uint8_t)atoi(param);

	if(gEMR[tBTNIdx-1] > 0)
	{
		CarErrorMSG_Display(gEMR[tBTNIdx-1]);
	}

/*	
	switch(tBTNIdx)
	{
		case 1:
			if(gEMR[0] > 0)
			{
				CarErrorMSG_Display(gEMR[0]);
			}
		break;
		
		case 2:
			if(gEMR[1] > 0)
			{
				CarErrorMSG_Display(gEMR[1]);
			}
		break;

		case 3:
			if(gEMR[2] > 0)
			{
				CarErrorMSG_Display(gEMR[2]);
			}
		break;

		case 4:
			if(gEMR[3] > 0)
			{
				CarErrorMSG_Display(gEMR[3]);
			}
		break;
	}	
*/
    return true;
}

bool CarErr_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tBTNIdx;
	
	M8_ExitCNT = 0;	
	tBTNIdx = (uint8_t)atoi(param);

	if(gEMR[tBTNIdx-1] > 0)
	{
		cError_Index = tBTNIdx;
	}

/*
	switch(tBTNIdx)
	{
		case 1:
			if(gEMR[0] > 0)
			{
				cError_Index = 1;
			}
		break;
		
		case 2:
			if(gEMR[1] > 0)
			{
				cError_Index = 2;
			}
		break;

		case 3:
			if(gEMR[2] > 0)
			{
				cError_Index = 3;
			}
		break;

		case 4:
			if(gEMR[3] > 0)
			{
				cError_Index = 4;
			}
		break;
	}
*/	
    return true;
}

void M8_Change_Page(uint8_t tOldPageIdx,uint8_t tNewPageIdx)
{
	uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	uint8_t i;
	
	ITUBackground* M8_EngCode_BG = ituSceneFindWidget(&theScene, "M8_EngCode_BG");
	assert(M8_EngCode_BG);
	ITUBackground* M8_AirCode_BG = ituSceneFindWidget(&theScene, "M8_AirCode_BG");
	assert(M8_AirCode_BG);
	ITUBackground* M8_CarPastCode_BG = ituSceneFindWidget(&theScene, "M8_CarPastCode_BG");
	assert(M8_CarPastCode_BG);
	ITUBackground* M8_EngPastCode_BG = ituSceneFindWidget(&theScene, "M8_EngPastCode_BG");
	assert(M8_EngPastCode_BG);

	switch(tOldPageIdx)
	{
		case EngineErrorCode:
			ituWidgetSetVisible(M8_EngCode_BG, false);
		break;

		case AirConErrorCode:
			ituWidgetSetVisible(M8_AirCode_BG, false);
		break;

		case PastCarCode:
			ituWidgetSetVisible(M8_CarPastCode_BG, false);
		break;

		case PastEngineCode:
			ituWidgetSetVisible(M8_EngPastCode_BG, false);
		break;
	}

	switch(tNewPageIdx)
	{
		case EngineErrorCode:
			ituWidgetSetVisible(M8_EngCode_BG, true);
			
			CarErr_ValueUpdate(CarErr1_Value, gEMR[0]);
			CarErr_ValueUpdate(CarErr2_Value, gEMR[1]);
			CarErr_ValueUpdate(CarErr3_Value, gEMR[2]);
			CarErr_ValueUpdate(CarErr4_Value, gEMR[3]);

			EngErr_SPNValueUpdate(EngSPN1_Value,gSPN[0]);
			EngErr_FMIValueUpdate(EngFMI1_Value,gFMI[0]);

			EngErr_SPNValueUpdate(EngSPN2_Value,gSPN[1]);
			EngErr_FMIValueUpdate(EngFMI2_Value,gFMI[1]);

			EngErr_SPNValueUpdate(EngSPN3_Value,gSPN[2]);
			EngErr_FMIValueUpdate(EngFMI3_Value,gFMI[2]);
			
//			M8_UserDisplay();
		break;

		case AirConErrorCode:
			ituWidgetSetVisible(M8_AirCode_BG, true);
			
			ACErr_ValueUpdate(ACErr1_Value,gACERR_Code[0]);
			ACErr_ValueUpdate(ACErr2_Value,gACERR_Code[1]);
			ACErr_ValueUpdate(ACErr3_Value,gACERR_Code[2]);
			ACErr_ValueUpdate(ACErr4_Value,gACERR_Code[3]);
			ACErr_ValueUpdate(ACErr5_Value,gACERR_Code[4]);
			ACErr_ValueUpdate(ACErr6_Value,gACERR_Code[5]);
			ACErr_ValueUpdate(ACErr7_Value,gACERR_Code[6]);
			ACErr_ValueUpdate(ACErr8_Value,gACERR_Code[7]);

//			M8_UserDisplay();
		break;

		case PastCarCode:

			// CAN TX CAN[REQ_PGN ; P_DTCU:0x18503FE4] : 3F 50 00 FF FF FF FF FF FF (PGN; 503F(P_DTCU))
			txbuffer[0] = 0x3F;
			txbuffer[1] = 0x50;
			txbuffer[2] = 0x00;
			
			#if 0
				PushTxData (0x18503FE4,txbuffer,8);
			#else
				CanbusTxDataManagement( gMainTimerCnt_10m,PGN_REQ1,txbuffer);
			#endif
			
			ituWidgetSetVisible(M8_CarPastCode_BG, true);

			if(M8_CheckUserIcon() == 1)
			{
				for(i=0;i<5;i++)
					PastCar_UserDisplay(i,true);
			}
			else
			{
				for(i=0;i<5;i++)
					PastCar_UserDisplay(i,false);
			}
			
			PastCarErr_ValueUpdate(PastCarErr1_Value,gEMR_Past[0]);
			PastCarTrim_ValueUpdate(PastCarTrim1_Value,gEMR_Time[0]);

			PastCarErr_ValueUpdate(PastCarErr2_Value,gEMR_Past[1]);
			PastCarTrim_ValueUpdate(PastCarTrim2_Value,gEMR_Time[1]);

			PastCarErr_ValueUpdate(PastCarErr3_Value,gEMR_Past[2]);
			PastCarTrim_ValueUpdate(PastCarTrim3_Value,gEMR_Time[2]);

			PastCarErr_ValueUpdate(PastCarErr4_Value,gEMR_Past[3]);
			PastCarTrim_ValueUpdate(PastCarTrim4_Value,gEMR_Time[3]);

			PastCarErr_ValueUpdate(PastCarErr5_Value,gEMR_Past[4]);
			PastCarTrim_ValueUpdate(PastCarTrim5_Value,gEMR_Time[4]);

//			M8_UserDisplay();
		break;

		case PastEngineCode:
			
			// CAN TX CAN[REQ_PGN ; PEEI 0x1850EAE4] : EA 50 00 FF FF FF FF FF FF (PGN ; 50EA(PEEI))
			txbuffer[0] = 0xEA;
			txbuffer[1] = 0x50;
			txbuffer[2] = 0x00;

			#if 0	
				PushTxData (0x1850EAE4,txbuffer,8);
			#else
				CanbusTxDataManagement( gMainTimerCnt_10m,PGN_REQ1,txbuffer);
			#endif	//JPHong 06-11
			
			ituWidgetSetVisible(M8_EngPastCode_BG, true);

			if(M8_CheckUserIcon() == 1)
			{
				for(i=0;i<4;i++)
					PastEng_UserDisplay(i,true);
			}
			else
			{
				for(i=0;i<4;i++)
					PastEng_UserDisplay(i,false);
			}

			PastEngErr_SPNValueUpdate(PastEngSPN1_Value,gSPN_Past[0]);
			PastEngErr_FMIValueUpdate(PastEngFMI1_Value,gFMI_Past[0]);
			PastEngTrim_ValueUpdate(PastEngMeter1,gPastEng_Time[0]);

			PastEngErr_SPNValueUpdate(PastEngSPN2_Value,gSPN_Past[1]);
			PastEngErr_FMIValueUpdate(PastEngFMI2_Value,gFMI_Past[1]);
			PastEngTrim_ValueUpdate(PastEngMeter2,gPastEng_Time[1]);

			PastEngErr_SPNValueUpdate(PastEngSPN3_Value,gSPN_Past[2]);
			PastEngErr_FMIValueUpdate(PastEngFMI3_Value,gFMI_Past[2]);
			PastEngTrim_ValueUpdate(PastEngMeter3,gPastEng_Time[2]);

			PastEngErr_SPNValueUpdate(PastEngSPN4_Value,gSPN_Past[3]);
			PastEngErr_FMIValueUpdate(PastEngFMI4_Value,gFMI_Past[3]);
			PastEngTrim_ValueUpdate(PastEngMeter4,gPastEng_Time[3]);

//			M8_UserDisplay();
		break;
	}


	//============
	if(tNewPageIdx==PastCarCode || tNewPageIdx==PastEngineCode)
	{
		printf("[M8_change] show // tNewPageIdx=%d\n",tNewPageIdx);
		ituWidgetSetVisible(PastCar_TITLE_CONT, true);

		for(int i=0; i<5; i++)
		{
			ituWidgetSetVisible(PastCar_TITLE[i], false);
		}   
	    ituWidgetSetVisible(PastCar_TITLE[Lang_Flag[M7_UserIndex]], true);

	}
	else
	{
		printf("[M8_change] hide // tNewPageIdx=%d\n",tNewPageIdx);
		ituWidgetSetVisible(PastCar_TITLE_CONT, false);	
	}
	
   
	//============
	
	M8_UserDisplay();
}

void CarErrCode_TB(uint16_t tCarCode)
{
	char tCode[10];
	
	//printf("EMR : %d",tCarCode);
	
	memset(tCode,0,10);
	sprintf(tCode,"%d",tCarCode);
	ituTextSetString(EngErrCode, tCode);
}

void CarErr_ValueUpdate(ITUText* tCarErrValue,uint16_t tCarCode)
{
	char tCode[10];
	
	memset(tCode,0,10);
	sprintf(tCode,"%d",tCarCode);
	ituTextSetString(tCarErrValue, tCode);
}

void PastCar_UserDisplay(uint8_t tLine,bool tDisp)
{
	uint8_t i;

	char PastCar_User[100];
	ITUText *PastCar_Name[5];
	ITUWidget *PastCar_Admin[5];
	ITUWidget *PastCar_Standard[5];

	uint8_t tUser_Idx;
	bool tCheckTime = 1;
	uint8_t tDestName[100];

	for(i=0;i<5;i++)
	{
		memset(PastCar_User,0,100);
		sprintf(PastCar_User,"PastCarAdmin%d",i+1);
		PastCar_Admin[i] = ituSceneFindWidget(&theScene, PastCar_User);
		assert(PastCar_Admin[i]);

		memset(PastCar_User,0,100);
		sprintf(PastCar_User,"PastCarStandard%d",i+1);
		PastCar_Standard[i] = ituSceneFindWidget(&theScene, PastCar_User);
		assert(PastCar_Standard[i]);

		memset(PastCar_User,0,100);
		sprintf(PastCar_User,"PastCar_Name%d",i+1);
		PastCar_Name[i] = ituSceneFindWidget(&theScene, PastCar_User);
		assert(PastCar_Name[i]);
	}

	tUser_Idx = gPastCar_UserIdx[tLine]-1;
	if((tUser_Idx >= 0)&&(tUser_Idx <= 3))		// UI2
	{
		if(g_UI2_ChkTime == 0)
			tCheckTime = false;
	}
	else if((tUser_Idx >= 4)&&(tUser_Idx <= 7))		// UI3
	{
		if(g_UI3_ChkTime == 0)
			tCheckTime = false;
	}
	else if((tUser_Idx >= 8)&&(tUser_Idx <= 11))		// UI4
	{
		if(g_UI4_ChkTime == 0)
			tCheckTime = false;
	}
	else if((tUser_Idx >= 12)&&(tUser_Idx <= 14))		// UI5
	{
		if(g_UI5_ChkTime == 0)
			tCheckTime = false;
	}

	if((tDisp==true)&&(tCheckTime==true))
	{
		memset(tDestName,0,100);

		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[tUser_Idx].UserName);
		ituTextSetString(PastCar_Name[tLine], tDestName);
	
		if((gPastCar_UserIdx[tLine] >= 1)&&(gPastCar_UserIdx[tLine] <= 253))
		{
			if(User_RegInfo[tUser_Idx].RegFlag == 5)		// admin
			{
				ituWidgetSetVisible(PastCar_Standard[tLine],false);
				ituWidgetSetVisible(PastCar_Admin[tLine],true);
			}
			else if((User_RegInfo[tUser_Idx].RegFlag == 2) || (User_RegInfo[tUser_Idx].RegFlag == 3) || (User_RegInfo[tUser_Idx].RegFlag == 9))  
			{
				ituWidgetSetVisible(PastCar_Admin[tLine],false);
				ituWidgetSetVisible(PastCar_Standard[tLine],true);
			}
			else if((User_RegInfo[tUser_Idx].RegFlag == 0) || (User_RegInfo[tUser_Idx].RegFlag == 15))
			{
				ituWidgetSetVisible(PastCar_Admin[tLine],false);
				ituWidgetSetVisible(PastCar_Standard[tLine],false);
			}

			ituWidgetSetVisible(PastCar_Name[tLine],true);
		}
		else if((gPastCar_UserIdx[tLine] == 0)||(gPastCar_UserIdx[tLine] == 0xFE)||(gPastCar_UserIdx[tLine] == 0xFF))
		{
			ituWidgetSetVisible(PastCar_Admin[tLine],false);
			ituWidgetSetVisible(PastCar_Standard[tLine],false);

			ituWidgetSetVisible(PastCar_Name[tLine],false);
		}
	}
	else
	{
		ituWidgetSetVisible(PastCar_Admin[tLine],false);
		ituWidgetSetVisible(PastCar_Standard[tLine],false);
		ituWidgetSetVisible(PastCar_Name[tLine],false);
	}
}

void PastCarBTN_PressDispClear()
{
	uint8_t i;

	char PastCar_PS[50];
	ITUWidget *PastCar_PS1_BTN[5];
	ITUWidget *PastCar_PS2_BTN[5];

	for(i=1;i<6;i++)
	{
		memset(PastCar_PS,0,50);
		sprintf(PastCar_PS,"PastCar_PS1_BTN%d",i);
		PastCar_PS1_BTN[i] = ituSceneFindWidget(&theScene, PastCar_PS);
		memset(PastCar_PS,0,50);
		sprintf(PastCar_PS,"PastCar_PS2_BTN%d",i);
		PastCar_PS2_BTN[i] = ituSceneFindWidget(&theScene, PastCar_PS);
		ituWidgetSetVisible(PastCar_PS1_BTN[i],false);
		ituWidgetSetVisible(PastCar_PS2_BTN[i],false);
	}
}

void PastCarBTN_PressDisp(uint8_t tBTN_Num,bool Disp)
{
	PastCarBTN_PressDispClear();

	uint8_t i;

	char PastCar_PS[50];
	ITUWidget *PastCar_PS1_BTN[5];
	ITUWidget *PastCar_PS2_BTN[5];

	for(i=0;i<5;i++)
	{
		memset(PastCar_PS,0,50);
		sprintf(PastCar_PS,"PastCar_PS1_BTN%d",tBTN_Num);
		PastCar_PS1_BTN[i] = ituSceneFindWidget(&theScene, PastCar_PS);
		assert(PastCar_PS1_BTN[i]);

		memset(PastCar_PS,0,50);
		sprintf(PastCar_PS,"PastCar_PS2_BTN%d",tBTN_Num);
		PastCar_PS2_BTN[i] = ituSceneFindWidget(&theScene, PastCar_PS);
		assert(PastCar_PS2_BTN[i]);
	}


	if(Disp == true)	
	{
		ituWidgetSetVisible(PastCar_PS1_BTN[tBTN_Num-1],false);
		ituWidgetSetVisible(PastCar_PS2_BTN[tBTN_Num-1],true);
	}
	else
	{
		ituWidgetSetVisible(PastCar_PS1_BTN[tBTN_Num-1],false);
		ituWidgetSetVisible(PastCar_PS2_BTN[tBTN_Num-1],false);
	}
}

void PastCarErrCode_TB(uint16_t tCarCode)
{
	char tCode[10];
	
	memset(tCode,0,10);
	sprintf(tCode,"%d",tCarCode);
	ituTextSetString(PastCarErrCode, tCode);
}

void PastCarErr_ValueUpdate(ITUText* tCarErrValue,uint16_t tCarCode)
{
	char tCode[10];
	
	memset(tCode,0,10);
	sprintf(tCode,"%d",tCarCode);
	ituTextSetString(tCarErrValue, tCode);
}

void PastCarTrim_ValueUpdate(ITUText* tCarTrimValue,uint32_t tCarTrim)
{
	char tCode[16];
	
	memset(tCode,0,16);
	sprintf(tCode,"%5d.%1d",tCarTrim/10,tCarTrim%10);
	ituTextSetString(tCarTrimValue, tCode);
}

void Get_CarErrorMSG2(char *Car_MSG,uint16_t tCar_Code)
{
	//uint8_t i,CarErr_Idx = 0xFF;
	//int i;
	int CarErr_Idx = -1;

	for(int i=0;i<95;i++)//chpark.24.10.10
	{
		if(Car_ErCode[i] == tCar_Code)
		{
			CarErr_Idx = i;
			printf("\n[Get_CarErrorMSG2][Find]Car_ErCode[%d]=%d\n",i,Car_ErCode[i]);
			break;
		}
	}

	//if(CarErr_Idx == 0xFF)
	if(CarErr_Idx == -1)
	{
		// not define Error
		printf("Not define ErrorCode %d\r\n",tCar_Code);

		if(Lang_Flag[M7_UserIndex] == Lang_ENG)
		{
			sprintf(Car_MSG,"%s","There is no description for this error\navailable.\nCheck the instruction manual.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_DEU)
		{
			sprintf(Car_MSG,"%s","Es ist keine Beschreibung für diesen\nFehler verfügbar.\nLesen Sie die Bedienungsanleitung.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_FRA)
		{
			sprintf(Car_MSG,"%s","Il n’y a aucune description disponible\npour cette erreur.\nConsultez le manuel d’instructions.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ITA)
		{
			sprintf(Car_MSG,"%s","Non è disponibile una descrizione\nper questo errore.\nControllare il manuale di istruzioni.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ESP)
		{
			sprintf(Car_MSG,"%s","No hay ninguna descripción\ndisponible para este error.\nConsulte el manual de instrucciones.");
		}
	}
	else
	{
		// error msg passin
		printf("Car Error CODE:%d  Index:%d  Msg:%s\r\n",tCar_Code,CarErr_Idx,V_CarErrorMSG_ENG[CarErr_Idx]);
	
		if(Lang_Flag[M7_UserIndex] == Lang_ENG)
		{
			sprintf(Car_MSG,"%s",V_CarErrorMSG_ENG[CarErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_DEU)
		{
			sprintf(Car_MSG,"%s",V_CarErrorMSG_DEU[CarErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_FRA)
		{
			sprintf(Car_MSG,"%s",V_CarErrorMSG_FRA[CarErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ITA)
		{
			sprintf(Car_MSG,"%s",V_CarErrorMSG_ITA[CarErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ESP)
		{
			sprintf(Car_MSG,"%s",V_CarErrorMSG_ESP[CarErr_Idx]);
		}
	}
}

void Get_CarErrorMSG3(char *Car_MSG,uint16_t tCar_Code)
{
	int CarErr_Idx = -1;
	
	//for(i=0;i<88;i++)
	for(int i=0;i<95;i++)//chpark.24.10.10
	{
		if(Car_ErCode[i] == tCar_Code)
		{
			CarErr_Idx = i;
			break;
		}
	}

	if(CarErr_Idx == -1)
	{
		// not define Error
		printf("Not define ErrorCode %d\r\n",tCar_Code);

		if(Lang_Flag[M7_UserIndex] == Lang_ENG)
		{
			sprintf(Car_MSG,"%s","There is no description for this\nerror available.\nCheck the instruction manual.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_DEU)
		{
			sprintf(Car_MSG,"%s","Es ist keine Beschreibung für\ndiesen Fehler verfügbar.\nLesen Sie die\nBedienungsanleitung.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_FRA)
		{
			sprintf(Car_MSG,"%s","Il n’y a aucune description\ndisponible pour cette erreur.\nConsultez le manuel\nd’instructions.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ITA)
		{
			sprintf(Car_MSG,"%s","Non è disponibile una\ndescrizione per questo errore.\nControllare il manuale di\nistruzioni.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ESP)
		{
			sprintf(Car_MSG,"%s","No hay ninguna descripción\ndisponible para este error.\nConsulte el manual de\ninstrucciones.");
		}
	}
	else
	{
		// error msg passing
		printf("Car Error MSG %d\r\n",tCar_Code);
	
		if(Lang_Flag[M7_UserIndex] == Lang_ENG)
		{
			sprintf(Car_MSG,"%s",H_CarErrorMSG_ENG[CarErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_DEU)
		{
			sprintf(Car_MSG,"%s",H_CarErrorMSG_DEU[CarErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_FRA)
		{
			sprintf(Car_MSG,"%s",H_CarErrorMSG_FRA[CarErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ITA)
		{
			sprintf(Car_MSG,"%s",H_CarErrorMSG_ITA[CarErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ESP)
		{
			sprintf(Car_MSG,"%s",H_CarErrorMSG_ESP[CarErr_Idx]);
		}
	}
}

void Get_EngErrorMSG2(char *Eng_MSG,uint32_t kSPN,uint8_t kFMI)
{
	//chpark.24.10.14
	//uint16_t i;
	//uint16_t EngEr_Idx = 0xFFFF;
	int EngEr_Idx = -1;
	for(int i=0;i<308;i++)
	{
		if((Eng_SPNCode[i] == kSPN)&&(Eng_FMICode[i] == kFMI))
		{
			EngEr_Idx = i;
			break;
		}
	}

	//if(EngEr_Idx == 0xFFFF)
	if(EngEr_Idx == -1)
	{
		// not define Error
		printf("Not define ErrorCode %d %d\r\n",kSPN,kFMI);

		if(Lang_Flag[M7_UserIndex] == Lang_ENG)
		{
			sprintf(Eng_MSG,"%s","There is no description for this error\navailable.\nCheck the instruction manual.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_DEU)
		{
			sprintf(Eng_MSG,"%s","Es ist keine Beschreibung für diesen\nFehler verfügbar.\nLesen Sie die Bedienungsanleitung.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_FRA)
		{
			sprintf(Eng_MSG,"%s","Il n’y a aucune description disponible\npour cette erreur.\nConsultez le manuel d’instructions.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ITA)
		{
			sprintf(Eng_MSG,"%s","Non è disponibile una descrizione\nper questo errore.\nControllare il manuale di istruzioni.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ESP)
		{
			sprintf(Eng_MSG,"%s","No hay ninguna descripción\ndisponible para este error.\nConsulte el manual de instrucciones.");
		}
	}
	else
	{
		// error msg passing
		printf("Eng Error MSG %d %d\r\n",kSPN,kFMI);
	
		if(Lang_Flag[M7_UserIndex] == Lang_ENG)
		{
			sprintf(Eng_MSG,"%s",V_EngErrorMSG_ENG[EngEr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_DEU)
		{
			sprintf(Eng_MSG,"%s",V_EngErrorMSG_DEU[EngEr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_FRA)
		{
			sprintf(Eng_MSG,"%s",V_EngErrorMSG_FRA[EngEr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ITA)
		{
			sprintf(Eng_MSG,"%s",V_EngErrorMSG_ITA[EngEr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ESP)
		{
			sprintf(Eng_MSG,"%s",V_EngErrorMSG_ESP[EngEr_Idx]);
		}
	}
}

void Get_EngErrorMSG3(char *Eng_MSG,uint32_t kSPN,uint8_t kFMI)
{
	//chpark.24.10.14
	//uint16_t i;
	//uint16_t EngEr_Idx = 0xFFFF;
	int EngEr_Idx = -1;
	
	for(int i=0;i<308;i++)
	{
		if((Eng_SPNCode[i] == kSPN)&&(Eng_FMICode[i] == kFMI))
		{
			EngEr_Idx = i;
			break;
		}
	}

	//if(EngEr_Idx == 0xFFFF)
	if(EngEr_Idx == -1)//chpark.24.10.14
	{
		// not define Error
		printf("Not define ErrorCode %d %d\r\n",kSPN,kFMI);

		if(Lang_Flag[M7_UserIndex] == Lang_ENG)
		{
			sprintf(Eng_MSG,"%s","There is no description for this\nerror available.\nCheck the instruction manual.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_DEU)
		{
			sprintf(Eng_MSG,"%s","Es ist keine Beschreibung für\ndiesen Fehler verfügbar.\nLesen Sie die\nBedienungsanleitung.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_FRA)
		{
			sprintf(Eng_MSG,"%s","Il n’y a aucune description\ndisponible pour cette erreur.\nConsultez le manuel\nd’instructions.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ITA)
		{
			sprintf(Eng_MSG,"%s","Non è disponibile una\ndescrizione per questo errore.\nControllare il manuale di\nistruzioni.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ESP)
		{
			sprintf(Eng_MSG,"%s","No hay ninguna descripción\ndisponible para este error.\nConsulte el manual de\ninstrucciones.");
		}
	}
	else
	{
		// error msg passing
		printf("Eng Error MSG %d %d\r\n",kSPN,kFMI);
	
		if(Lang_Flag[M7_UserIndex] == Lang_ENG)
		{
			sprintf(Eng_MSG,"%s",H_EngErrorMSG_ENG[EngEr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_DEU)
		{
			sprintf(Eng_MSG,"%s",H_EngErrorMSG_DEU[EngEr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_FRA)
		{
			sprintf(Eng_MSG,"%s",H_EngErrorMSG_FRA[EngEr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ITA)
		{
			sprintf(Eng_MSG,"%s",H_EngErrorMSG_ITA[EngEr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ESP)
		{
			sprintf(Eng_MSG,"%s",H_EngErrorMSG_ESP[EngEr_Idx]);
		}
	}
}

void Get_ACErrorMSG2(char *AC_MSG,uint8_t tAC_Code)
{
	//chpark.24.10.14
	//uint8_t i,ACErr_Idx = 0xFF;
	int ACErr_Idx = -1;

	for(int i=0;i<10;i++)
	{
		if(AC_ErCode[i] == tAC_Code)
		{
			ACErr_Idx = i;
			break;
		}
	}

	if(ACErr_Idx == -1)
	{
		// not define Error
		printf("Not define ErrorCode %d\r\n",tAC_Code);

		if(Lang_Flag[M7_UserIndex] == Lang_ENG)
		{
			sprintf(AC_MSG,"%s","This error does not prepared\nexplanation.\nPlease check the instruction manual.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_DEU)
		{
			sprintf(AC_MSG,"%s","Deiser Fefler nicht vorbereitet\nEräuterung.\nÜberprüfen Sie bitte die\nBedienungsanleitung.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_FRA)
		{
			sprintf(AC_MSG,"%s","Cette erreur n'a pas prèparè\nexplication.\nS'il vous plaît consulter manual\nd'instruction.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ITA)
		{
			sprintf(AC_MSG,"%s","Questo errore non preparato\nspiegayione.\nSi prega di consultare il manuale di\nisrruzioni.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ESP)
		{
			sprintf(AC_MSG,"%s","Este error no prepara una explicación.\nConsulte el manual de instrucciones.");
		}
	}
	else
	{
		// error msg passing
		printf("AC Error MSG %d\r\n",tAC_Code);
	
		if(Lang_Flag[M7_UserIndex] == Lang_ENG)
		{
			sprintf(AC_MSG,"%s",V_ACErrorMSG_ENG[ACErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_DEU)
		{
			sprintf(AC_MSG,"%s",V_ACErrorMSG_DEU[ACErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_FRA)
		{
			sprintf(AC_MSG,"%s",V_ACErrorMSG_FRA[ACErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ITA)
		{
			sprintf(AC_MSG,"%s",V_ACErrorMSG_ITA[ACErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ESP)
		{
			sprintf(AC_MSG,"%s",V_ACErrorMSG_ESP[ACErr_Idx]);
		}
	}
}

void Get_ACErrorMSG3(char *AC_MSG,uint8_t tAC_Code)
{
	//chpark.24.10.14
	//uint8_t i,ACErr_Idx = 0xFF;
	int ACErr_Idx = -1;

	for(int i=0;i<10;i++)
	{
		if(AC_ErCode[i] == tAC_Code)
		{
			ACErr_Idx = i;
			break;
		}
	}

	if(ACErr_Idx == -1)
	{
		// not define Error
		printf("Not define ErrorCode %d\r\n",tAC_Code);

		if(Lang_Flag[M7_UserIndex] == Lang_ENG)
		{
			sprintf(AC_MSG,"%s","This error does not prepared\nexplanation.\nPlease check the instruction\nmanual.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_DEU)
		{
			sprintf(AC_MSG,"%s","Deiser Fefler nicht vorbereitet\nEräuterung.\nÜberprüfen Sie bitte die\nBedienungsanleitung.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_FRA)
		{
			sprintf(AC_MSG,"%s","Cette erreur n'a pas prèparè\nexplication.\nS'il vous plaît consulter manual\nd'instruction.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ITA)
		{
			sprintf(AC_MSG,"%s","Questo errore non preparato\nspiegayione.\nSi prega di consultare il\nmanuale di isrruzioni.");
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ESP)
		{
			sprintf(AC_MSG,"%s","Este error no prepara una\nexplicación.\nConsulte el manual de\ninstrucciones.");
		}
	}
	else
	{
		// error msg passing
		printf("AC Error MSG %d\r\n",tAC_Code);
	
		if(Lang_Flag[M7_UserIndex] == Lang_ENG)
		{
			sprintf(AC_MSG,"%s",H_ACErrorMSG_ENG[ACErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_DEU)
		{
			sprintf(AC_MSG,"%s",H_ACErrorMSG_DEU[ACErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_FRA)
		{
			sprintf(AC_MSG,"%s",H_ACErrorMSG_FRA[ACErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ITA)
		{
			sprintf(AC_MSG,"%s",H_ACErrorMSG_ITA[ACErr_Idx]);
		}
		else if(Lang_Flag[M7_UserIndex] == Lang_ESP)
		{
			sprintf(AC_MSG,"%s",H_ACErrorMSG_ESP[ACErr_Idx]);
		}
	}
}

void EngErrCode_TB(uint32_t mSPN,uint8_t mFMI)
{
	char tCode[40];
	
	printf("SPN %d ; FMI %d\r\n",mSPN,mFMI);
	memset(tCode,0,40);
	sprintf(tCode,"SPN %d     FMI %d",mSPN,mFMI);
	ituTextSetString(EngErrCode, tCode);
}

void EngErr_SPNValueUpdate(ITUText* tEngSPNValue,uint32_t tSPN)
{
	char tCode[16];
	
	memset(tCode,0,16);
	sprintf(tCode,"%d",tSPN);
	ituTextSetString(tEngSPNValue, tCode);
}

void EngErr_FMIValueUpdate(ITUText* tEngFMIValue,uint8_t tFMI)
{
	char tCode[5];
	
	memset(tCode,0,5);
	sprintf(tCode,"%d",tFMI);
	ituTextSetString(tEngFMIValue, tCode);
}

void PastEng_UserDisplay(uint8_t tLine,bool tDisp)
{
	uint8_t i;

	char PastEng_User[100];
	ITUText *PastEng_Name[4];
	ITUWidget *PastEng_Admin[4];
	ITUWidget *PastEng_Standard[4];
	
	uint8_t tDestName[100];
	bool tCheckTime = 1;
	uint8_t tUser_Idx;

	for(i=0;i<4;i++)
	{
		memset(PastEng_User,0,100);
		sprintf(PastEng_User,"PastEng_Admin%d",i+1);
		PastEng_Admin[i] = ituSceneFindWidget(&theScene, PastEng_User);
		assert(PastEng_Admin[i]);

		memset(PastEng_User,0,100);
		sprintf(PastEng_User,"PastEng_Standard%d",i+1);
		PastEng_Standard[i] = ituSceneFindWidget(&theScene, PastEng_User);
		assert(PastEng_Standard[i]);

		memset(PastEng_User,0,100);
		sprintf(PastEng_User,"PastEng_Name%d",i+1);
		PastEng_Name[i] = ituSceneFindWidget(&theScene, PastEng_User);
		assert(PastEng_Name[i]);
	}

	tUser_Idx = gPastEng_UserIdx[tLine]-1;		// 2024-03-14 JPHong		#14119
	if((tUser_Idx >= 0)&&(tUser_Idx <= 3))		// UI2
	{
		if(g_UI2_ChkTime == 0)
			tCheckTime = false;
	}
	else if((tUser_Idx >= 4)&&(tUser_Idx <= 7))		// UI3
	{
		if(g_UI3_ChkTime == 0)
			tCheckTime = false;
	}
	else if((tUser_Idx >= 8)&&(tUser_Idx <= 11))		// UI4
	{
		if(g_UI4_ChkTime == 0)
			tCheckTime = false;
	}
	else if((tUser_Idx >= 12)&&(tUser_Idx <= 14))		// UI5
	{
		if(g_UI5_ChkTime == 0)
			tCheckTime = false;
	}
	
	if((tDisp==true)&&(tCheckTime==true))
	{
		//tUser_Idx = gPastEng_UserIdx[tLine]-1;

		printf("PastENG User L %d, Ur %d, Lv %d[%s]\r\n",tLine,tUser_Idx,User_RegInfo[tUser_Idx].RegFlag,User_RegInfo[tUser_Idx].UserName);
		
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[tUser_Idx].UserName);
		ituTextSetString(PastEng_Name[tLine], tDestName);

		if((gPastEng_UserIdx[tLine] >= 1)&&(gPastEng_UserIdx[tLine] <= 253))
		{
			if(User_RegInfo[tUser_Idx].RegFlag == 5)		// admin
			{
				ituWidgetSetVisible(PastEng_Standard[tLine],false);
				ituWidgetSetVisible(PastEng_Admin[tLine],true);
			}
			else if((User_RegInfo[tUser_Idx].RegFlag == 2) || (User_RegInfo[tUser_Idx].RegFlag == 3) || (User_RegInfo[tUser_Idx].RegFlag == 9))		// standard
			{
				ituWidgetSetVisible(PastEng_Admin[tLine],false);
				ituWidgetSetVisible(PastEng_Standard[tLine],true);
			}
			else if((User_RegInfo[tUser_Idx].RegFlag == 0) || (User_RegInfo[tUser_Idx].RegFlag == 15))
			{
				ituWidgetSetVisible(PastEng_Admin[tLine],false);
				ituWidgetSetVisible(PastEng_Standard[tLine],false);
			}

			ituWidgetSetVisible(PastEng_Name[tLine],true);
		}
		else if((gPastEng_UserIdx[tUser_Idx] == 0)||(gPastEng_UserIdx[tUser_Idx] == 0xFE)||(gPastEng_UserIdx[tUser_Idx] == 0xFF))
		{
			ituWidgetSetVisible(PastEng_Admin[tLine],false);
			ituWidgetSetVisible(PastEng_Standard[tLine],false);

			ituWidgetSetVisible(PastEng_Name[tLine],false);
		}
	}
	else
	{
		printf("PastENG User L %d, false\r\n",tLine);

		ituWidgetSetVisible(PastEng_Admin[tLine],false);
		ituWidgetSetVisible(PastEng_Standard[tLine],false);
		ituWidgetSetVisible(PastEng_Name[tLine],false);
	}
}

void PastEngBTN_PressDispClear()
{
	uint8_t i;

	char PastEng_PS[50];
	ITUWidget *PastEng_PS1_BTN[4];
	ITUWidget *PastEng_PS2_BTN[4];

	for(i=1;i<5;i++)
	{
		memset(PastEng_PS,0,50);
		sprintf(PastEng_PS,"PastEng_PS1_BTN%d",i);
		PastEng_PS1_BTN[i] = ituSceneFindWidget(&theScene, PastEng_PS);

		memset(PastEng_PS,0,50);
		sprintf(PastEng_PS,"PastEng_PS2_BTN%d",i);
		PastEng_PS2_BTN[i] = ituSceneFindWidget(&theScene, PastEng_PS);
		
		ituWidgetSetVisible(PastEng_PS1_BTN[i],false);
		ituWidgetSetVisible(PastEng_PS2_BTN[i],false);
	}
}

void PastEngBTN_PressDisp(uint8_t tBTN_Num,bool Disp)
{
	PastEngBTN_PressDispClear();

	uint8_t i;

	char PastEng_PS[50];
	ITUWidget *PastEng_PS1_BTN[5];
	ITUWidget *PastEng_PS2_BTN[5];

	for(i=0;i<5;i++)
	{
		memset(PastEng_PS,0,50);
		sprintf(PastEng_PS,"PastEng_PS1_BTN%d",tBTN_Num);
		PastEng_PS1_BTN[i] = ituSceneFindWidget(&theScene, PastEng_PS);
		assert(PastEng_PS1_BTN[i]);

		memset(PastEng_PS,0,50);
		sprintf(PastEng_PS,"PastEng_PS2_BTN%d",tBTN_Num);
		PastEng_PS2_BTN[i] = ituSceneFindWidget(&theScene, PastEng_PS);
		assert(PastEng_PS2_BTN[i]);
	}

	if(Disp == true)	
	{
		ituWidgetSetVisible(PastEng_PS1_BTN[tBTN_Num-1],false);
		ituWidgetSetVisible(PastEng_PS2_BTN[tBTN_Num-1],true);
	}
	else
	{
		ituWidgetSetVisible(PastEng_PS1_BTN[tBTN_Num-1],false);
		ituWidgetSetVisible(PastEng_PS2_BTN[tBTN_Num-1],false);
	}
}

void PastEngErrCode_TB(uint32_t mSPN,uint8_t mFMI)
{
	char tCode[40];
	
	memset(tCode,0,40);
	sprintf(tCode,"SPN %d     FMI %d",mSPN,mFMI);
	ituTextSetString(PastEngErrCode, tCode);
}

void PastEngErr_SPNValueUpdate(ITUText* tEngSPNValue,uint32_t tSPN)
{
	char tCode[16];
	
	memset(tCode,0,16);
	sprintf(tCode,"%d",tSPN);
	ituTextSetString(tEngSPNValue, tCode);
}

void PastEngErr_FMIValueUpdate(ITUText* tEngFMIValue,uint8_t tFMI)
{
	char tCode[5];
	
	memset(tCode,0,5);
	sprintf(tCode,"%d",tFMI);
	ituTextSetString(tEngFMIValue, tCode);
}

void PastEngTrim_ValueUpdate(ITUText* tEngTrimValue,uint32_t tEngTrim)
{
	char tCode[16];
	
	memset(tCode,0,16);
	sprintf(tCode,"%5d.%1d",tEngTrim/10,tEngTrim%10);
	ituTextSetString(tEngTrimValue, tCode);
}

void ACErrCode_TB(uint8_t tAC_Code)
{
	char tCode[5];
	
	printf("ACErr ; %d\r\n",tAC_Code);
	
	memset(tCode,0,5);
	sprintf(tCode,"%d",tAC_Code);
	ituTextSetString(ACErrCode, tCode);
}

void ACErr_ValueUpdate(ITUText* tACErrValue,uint8_t tAC_Code)
{
	char tCode[5];
	
	memset(tCode,0,5);
	sprintf(tCode,"%d",tAC_Code);
	ituTextSetString(tACErrValue, tCode);
}

void EngError_MakeDisplayData(void)
{
	uint8_t i=0,tMulti_ECU_CNT=0,tMulti_DCU_CNT=0;
	uint8_t tEngErr_CNT=0,tSub_CNT = 0;
	
	tMulti_ECU_CNT = 0;
	tMulti_DCU_CNT = 0;
	for(i=0;i<3;i++)
	{
		if(gSPN_ECU[i] > 0)
		{
			tMulti_ECU_CNT++;
		}
			
		if(gSPN_DCU[i] > 0)
		{
			tMulti_DCU_CNT++;
		}

		gSPN[i] = 0;
		gFMI[i] = 0;
	}

	tEngErr_CNT = 0;
	if(gSPN_sECU != 0)
	{
		gSPN[tEngErr_CNT] = gSPN_sECU;
		gFMI[tEngErr_CNT] = gFMI_sECU;

		tEngErr_CNT++;
	}

	tSub_CNT = 0;
	if(tMulti_ECU_CNT > 0)
	{
		for(i=tEngErr_CNT;i<(tEngErr_CNT+tMulti_ECU_CNT);i++)
		{
			if(i<3)
			{
				if(gSPN_ECU[i-tEngErr_CNT] != 0)
				{
					gSPN[i] = gSPN_ECU[i-tEngErr_CNT];
					gFMI[i] = gFMI_ECU[i-tEngErr_CNT];

					tSub_CNT++;
				}
			}
			else if(i==3)
				break;
		}
	}

	tEngErr_CNT += tSub_CNT;
	if(gSPN_sDCU != 0)
	{
		gSPN[tEngErr_CNT] = gSPN_sDCU;
		gFMI[tEngErr_CNT] = gFMI_sDCU;

		tEngErr_CNT++;
	}

	tSub_CNT = 0;
	if(tMulti_DCU_CNT > 0)
	{
		for(i=tEngErr_CNT;i<(tEngErr_CNT+tMulti_DCU_CNT);i++)
		{
			if(i<3)
			{
				if(gSPN_DCU[i-tEngErr_CNT] != 0)
				{
					gSPN[i] = gSPN_DCU[i-tEngErr_CNT];
					gFMI[i] = gFMI_DCU[i-tEngErr_CNT];

					tSub_CNT++;
				}
			}
			else if(i==3)
			{
				tEngErr_CNT = 0;
				tSub_CNT = 0;
				break;
			}
		}
	}
}
/*
void PastEngError_MakeDisplayData(void)
{
	uint8_t i=0,tMulti_PastECU_CNT=0,tMulti_PastDCU_CNT=0;
	uint8_t tEngErr_CNT=0,tSub_CNT = 0;
	
	//static uint8_t mECU_ErCNT = 0, mDCU_ErCNT = 0;

	tMulti_PastECU_CNT = 0;
	tMulti_PastDCU_CNT = 0;
	for(i=0;i<6;i++)
	{
		if(gSPN_PastECU[i] > 0)
		{
			tMulti_PastECU_CNT++;
		}
			
		if(gSPN_PastDCU[i] > 0)
		{
			tMulti_PastDCU_CNT++;
		}

		gSPN_Past[i] = 0;
		gFMI_Past[i] = 0;
	}

	tEngErr_CNT = 0;
	if(gSPN_sPastECU != 0)
	{
		gSPN_Past[tEngErr_CNT] = gSPN_sPastECU;
		gFMI_Past[tEngErr_CNT] = gFMI_sPastECU;

		tEngErr_CNT++;
	}

	tSub_CNT = 0;
	if(tMulti_PastECU_CNT > 0)
	{
		for(i=tEngErr_CNT;i<(tEngErr_CNT+tMulti_PastECU_CNT);i++)
		{
			if(i<6)
			{
				if(gSPN_PastECU[i-tEngErr_CNT] != 0)
				{
					gSPN_Past[i] = gSPN_PastECU[i-tEngErr_CNT];
					gFMI_Past[i] = gFMI_PastECU[i-tEngErr_CNT];

					tSub_CNT++;
				}
			}
			else if(i==3)
				break;
		}
	}

	tEngErr_CNT += tSub_CNT;
	if(gSPN_sPastDCU != 0)
	{
		gSPN_Past[tEngErr_CNT] = gSPN_sPastDCU;
		gFMI_Past[tEngErr_CNT] = gFMI_sPastDCU;

		tEngErr_CNT++;
	}

	tSub_CNT = 0;
	if(tMulti_PastDCU_CNT > 0)
	{
		for(i=tEngErr_CNT;i<(tEngErr_CNT+tMulti_PastDCU_CNT);i++)
		{
			if(i<6)
			{
				if(gSPN_PastDCU[i-tEngErr_CNT] != 0)
				{
					gSPN_Past[i] = gSPN_PastDCU[i-tEngErr_CNT];
					gFMI_Past[i] = gFMI_PastDCU[i-tEngErr_CNT];

					tSub_CNT++;
				}
			}
			else if(i==6)
			{
				tEngErr_CNT = 0;
				tSub_CNT = 0;
				break;
			}
		}
	}
}
*/
