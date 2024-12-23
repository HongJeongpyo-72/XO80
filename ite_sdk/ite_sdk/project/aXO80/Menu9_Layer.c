#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

/* widgets:
Menu9_Layer
Menu9_BG
M9_C_MSG_ESP
M9_C_MSG_ITA
M9_C_MSG_FRA
M9_C_MSG_DEU
M9_C_MSG_ENG
M9_No_BTN
M9_No_SEL
M9_Yes_BTN
M9_Yes_SEL
M9_END_L8
M9_END_L7
M9_END_L6
M9_END_L5
M9_END_L4
M9_END_L3
M9_END_L2
M9_END_L1
M9_STR_L8
M9_STR_L7
M9_STR_L6
M9_STR_L5
M9_STR_L4
M9_STR_L3
M9_STR_L2
M9_STR_L1
M9_Data8
M9_Data7
M9_Data6
M9_Data5
M9_Data4
M9_Data3
M9_Data2
M9_Data1
M9_UserName8
M9_UserName7
M9_UserName6
M9_UserName5
M9_UserName4
M9_UserName3
M9_UserName2
M9_UserName1
M9_UserIcon_Standard8
M9_UserIcon_Standard7
M9_UserIcon_Standard6
M9_UserIcon_Standard5
M9_UserIcon_Standard4
M9_UserIcon_Standard3
M9_UserIcon_Standard2
M9_UserIcon_Standard1
M9_UserIcon_Admin8
M9_UserIcon_Admin7
M9_UserIcon_Admin6
M9_UserIcon_Admin5
M9_UserIcon_Admin4
M9_UserIcon_Admin3
M9_UserIcon_Admin2
M9_UserIcon_Admin1
M9_D7_BTN
M9_D6_BTN
M9_D5_BTN
M9_1_BTN
M9_1_SEL
*/


//Yes NO

#define M9_MODE_RESET 0
#define M9_MODE_YESNO 1

static ITUWidget* M9_CONT_MODE[2]; //0:reset  1:yes/no
static ITUWidget* M9_RESET_ICON[5];//
static ITUWidget* M9_YES_ICON[5];  //
static ITUWidget* M9_NO_ICON[5];   //

	

static uint32_t M9_SecTimer;
static uint8_t M9_ExitCNT = 0;

static uint8_t M9_ResetFlag = 0;
static uint8_t M9_ResetSel = 0;

UserWorkTime UserMeter[10];

static ITUWidget *M9_UserIcon_Admin[8];
static ITUWidget *M9_UserIcon_Standard[8];

static ITUText *M9_UserName[8];
static ITUText *M9_STR_L[8];
static ITUText *M9_END_L[8];

void M9_ResetScreen(void);

void User_IconDisplay(uint8_t tLine,uint8_t tUserNum,bool Disp_Flag);
void User_NameDisplay(uint8_t tLine,uint8_t tUserNum,bool Disp_Flag);
void User_MeterDisplay(uint8_t tLine,uint8_t tUserNum,bool Disp_Flag);

void User_MeterDisplay2(uint8_t tLine,uint8_t tUserIdx);

bool M9_UserNo(ITUWidget* widget, char* param);
bool M9_UserYes(ITUWidget* widget, char* param);

bool M9_UserReset(ITUWidget* widget, char* param);
void M9_ResetSelDisplay(uint8_t tFlag);

bool M9D5_BTNMouseUp(ITUWidget* widget, char* param);

bool M9_UserDisplay(void);

extern void GotoMenuTop(void);


bool Menu9_Enter(ITUWidget* widget, char* param)
{
	uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};		// 231214 jhlee

	char tmp_iconwidget[100];
	uint8_t i,tUserIdx,tUserLevel;


//printf(" UserMeter[0].tUserNum:[%d],[%d]\n", UserMeter[0].tUserNum, UserMeter[0].tUserNum);


	for(i=0;i<8;i++)
	{
		memset(tmp_iconwidget,0,100);
		sprintf(tmp_iconwidget,"M9_UserIcon_Admin%d",i+1);
		M9_UserIcon_Admin[i] = ituSceneFindWidget(&theScene, tmp_iconwidget);
		assert(M9_UserIcon_Admin[i]);

		memset(tmp_iconwidget,0,100);
		sprintf(tmp_iconwidget,"M9_UserIcon_Standard%d",i+1);
		M9_UserIcon_Standard[i] = ituSceneFindWidget(&theScene, tmp_iconwidget);
		assert(M9_UserIcon_Standard[i]);
		
		memset(tmp_iconwidget,0,100);
		sprintf(tmp_iconwidget,"M9_UserName%d",i+1);
		M9_UserName[i] = ituSceneFindWidget(&theScene, tmp_iconwidget);
		assert(M9_UserName[i]);

		memset(tmp_iconwidget,0,100);
		sprintf(tmp_iconwidget,"M9_STR_L%d",i+1);
		M9_STR_L[i] = ituSceneFindWidget(&theScene, tmp_iconwidget);
		assert(M9_STR_L[i]);

		memset(tmp_iconwidget,0,100);
		sprintf(tmp_iconwidget,"M9_END_L%d",i+1);
		M9_END_L[i] = ituSceneFindWidget(&theScene, tmp_iconwidget);
		assert(M9_END_L[i]);
	}

	M9_ResetFlag = 0;		// reset btn

//printf(" UserMeter[1].tUserNum:[%d],[%d]\n", UserMeter[0].tUserNum, UserMeter[0].tUserNum);
	for(i=0;i<8;i++)
	{
//		User_MeterDisplay2(i,0);
	}
		
		
	txbuffer[0] = 0xED;		// PGN ; 50ED(UWH)
	txbuffer[1] = 0x50;
	txbuffer[2] = 0x00;
				
	CanbusTxDataManagement( gMainTimerCnt_10m,PGN_REQ1,txbuffer);			// 231214 jhlee

	M9_ResetScreen();
//printf(" UserMeter[2].tUserNum:[%d],[%d]\n", UserMeter[0].tUserNum, UserMeter[0].tUserNum);
	for(i=0;i<8;i++)
	{
		tUserIdx = UserMeter[i].tUserNum;
			
		User_MeterDisplay2(i,tUserIdx);
	}

	M9_UserDisplay();


	SetWidget(M9_CONT_MODE[0],"M9_CONT_RESET_ICON");
	SetWidget(M9_CONT_MODE[1],"M9_CONT_YESNO_ICON");

	SetWidget(M9_RESET_ICON[0],"M9_ICON_RESET_ENG");
	SetWidget(M9_RESET_ICON[1],"M9_ICON_RESET_GER");
	SetWidget(M9_RESET_ICON[2],"M9_ICON_RESET_FR");
	SetWidget(M9_RESET_ICON[3],"M9_ICON_RESET_IT");
	SetWidget(M9_RESET_ICON[4],"M9_ICON_RESET_SP");

	SetWidget(M9_YES_ICON[0],"M9_ICON_YES_ENG");
	SetWidget(M9_YES_ICON[1],"M9_ICON_YES_GER");
	SetWidget(M9_YES_ICON[2],"M9_ICON_YES_FR");
	SetWidget(M9_YES_ICON[3],"M9_ICON_YES_IT");
	SetWidget(M9_YES_ICON[4],"M9_ICON_YES_SP");

	SetWidget(M9_NO_ICON[0],"M9_ICON_NO_ENG");
	SetWidget(M9_NO_ICON[1],"M9_ICON_NO_GER");
	SetWidget(M9_NO_ICON[2],"M9_ICON_NO_FR");
	SetWidget(M9_NO_ICON[3],"M9_ICON_NO_IT");
	SetWidget(M9_NO_ICON[4],"M9_ICON_NO_SP");

	Menu9_SetMode(M9_MODE_RESET);//chpark.24.04.11

    return true;
}

void Menu9_SetMode(int nMode)
{
	if(nMode==0) 
	{
		//0: reset 
		ituWidgetSetVisible(M9_CONT_MODE[0],true);
		ituWidgetSetVisible(M9_CONT_MODE[1],false);

		for(int i=0;i<5;i++)
		{
			ituWidgetSetVisible(M9_RESET_ICON[i],false);
		}
		ituWidgetSetVisible(M9_RESET_ICON[Lang_Flag[M7_UserIndex]], true);
	}
	else  
	{
		//1: yes/no
		ituWidgetSetVisible(M9_CONT_MODE[0],false);
		ituWidgetSetVisible(M9_CONT_MODE[1],true);

		for(int i=0;i<5;i++)
		{
			ituWidgetSetVisible(M9_YES_ICON[i],false);
			ituWidgetSetVisible(M9_NO_ICON[i],false);
		}
		ituWidgetSetVisible(M9_YES_ICON[Lang_Flag[M7_UserIndex]], true);
		ituWidgetSetVisible(M9_NO_ICON[Lang_Flag[M7_UserIndex]], true);
	}

}

bool Menu9_Timer(ITUWidget* widget, char* param)
{
	uint8_t i,k,tUserIdx,tUserLevel;
	static UserWorkTime bUserMeter[10]={0xFF,};
	static uint8_t M9_UpdataFlag = 0;
	
/*
	if((gMainTimerCnt_10m - M9_SecTimer) >= 3000)
	{
		//printf("\r\n M9 Menu Exit > 30 sec test\r\n");
		GotoMain(MAIN_MODE_NORMAL); 
	}
*/
	if(((gMainTimerCnt_10m - M9HM_Time) >= 500) &&(M9HM_Time))// check 5sec
	{
		// HM ; N/A
		UserMeter[0].tEndTime = 0xFFFFFFFF;
		M9HM_Time = 0;
	}

	if(bUserMeter[0].tEndTime != UserMeter[0].tEndTime)
	{
		bUserMeter[0].tEndTime = UserMeter[0].tEndTime;
		
		M9_UpdataFlag = 1;
	}
	
	for(i=0;i<8;i++)
	{
		if(bUserMeter[i].tSrtTime != UserMeter[i].tSrtTime)
		{
			for(k=0;k<8;k++)
			{
				bUserMeter[k].tSrtTime = UserMeter[k].tSrtTime;
				bUserMeter[k].tEndTime = UserMeter[k].tEndTime;
				bUserMeter[k].tUserNum = UserMeter[k].tUserNum;
			}

			M9_UpdataFlag = 1;
			break;
		}
	}

	if(M9_UpdataFlag == 1)
	{
		M9_UpdataFlag = 0;
		
		for(i=0;i<8;i++)
		{
			tUserIdx = UserMeter[i].tUserNum;
				
			User_MeterDisplay2(i,tUserIdx);
		}
	}

	M9_UserDisplay();

    return true;
}

bool Menu9_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t tJogWheel = 0;

	ITUWidget *M9_Yes_Sel,*M9_Yes_BTN;
	ITUWidget *M9_No_Sel,*M9_No_BTN;

	M9_Yes_Sel = ituSceneFindWidget(&theScene, "M9_Yes_SEL");
	assert(M9_Yes_Sel);
	M9_Yes_BTN = ituSceneFindWidget(&theScene, "M9_Yes_BTN");
	assert(M9_Yes_BTN);

	M9_No_Sel = ituSceneFindWidget(&theScene, "M9_No_SEL");
	assert(M9_No_Sel);
	M9_No_BTN = ituSceneFindWidget(&theScene, "M9_No_BTN");
	assert(M9_No_BTN);

	tJogWheel = param[0];
	
	if(M9_ResetFlag == 1)		// yes/no
	{
		if(ituWidgetIsVisible(M9_Yes_Sel) == true)
		{
			if(ituWidgetIsVisible(M9_No_Sel) == false)
			{
				M9_UserNo(NULL,"0");
			}
		}
		else if(ituWidgetIsVisible(M9_Yes_Sel) == false)
		{
			if(ituWidgetIsVisible(M9_No_Sel) == true)
			{
				M9_UserYes(NULL,"0");
			}
			else if(ituWidgetIsVisible(M9_No_Sel) == false)
			{
				M9_UserYes(NULL,"0");
			}
		}
	}
	else if(M9_ResetFlag == 0)		// reset
	{
		if(M9_ResetSel == 1)
			M9_ResetSelDisplay(0);
		else if(M9_ResetSel == 0)
			M9_ResetSelDisplay(1);
	}
	
    return true;
}

bool Menu9_JogButton(ITUWidget* widget, char* param)
{
	uint8_t tJogButton = 0;

	ITUWidget *M9_Yes_Sel,*M9_Yes_BTN;
	ITUWidget *M9_No_Sel,*M9_No_BTN;

	M9_Yes_Sel = ituSceneFindWidget(&theScene, "M9_Yes_SEL");
	assert(M9_Yes_Sel);
	M9_Yes_BTN = ituSceneFindWidget(&theScene, "M9_Yes_BTN");
	assert(M9_Yes_BTN);

	M9_No_Sel = ituSceneFindWidget(&theScene, "M9_No_SEL");
	assert(M9_No_Sel);
	M9_No_BTN = ituSceneFindWidget(&theScene, "M9_No_BTN");
	assert(M9_No_BTN);

	tJogButton = param[0];

	if((tJogButton == JOG_DIAL_KEY_LEFT) || (tJogButton ==JOG_DIAL_KEY_RIGHT))
	{
		if(M9_ResetFlag == 1)		// yes/no
		{
			if(ituWidgetIsVisible(M9_Yes_Sel) == true)
			{
				if(ituWidgetIsVisible(M9_No_Sel) == false)
				{
					M9_UserNo(NULL,"0");
				}
			}
			else if(ituWidgetIsVisible(M9_Yes_Sel) == false)
			{
				if(ituWidgetIsVisible(M9_No_Sel) == true)
				{
					M9_UserYes(NULL,"0");
				}
				else if(ituWidgetIsVisible(M9_No_Sel) == false)
				{
					M9_UserYes(NULL,"0");
				}
			}
		}
		else if(M9_ResetFlag == 0)
		{
			if(M9_ResetSel == 1)
				M9_ResetSelDisplay(0);
			else if(M9_ResetSel == 0)
				M9_ResetSelDisplay(1);
		}
	}
	else if((tJogButton == JOG_DIAL_KEY_UP) || (tJogButton ==JOG_DIAL_KEY_DOWN)) 
	{
		if(M9_ResetFlag == 0)
		{
			if(M9_ResetSel == 1)
				M9_ResetSelDisplay(0);
			else if(M9_ResetSel == 0)
				M9_ResetSelDisplay(1);
		}
	}
	else if(tJogButton == JOG_DIAL_KEY_ENTER_OFF)
	{
		if(M9_ResetFlag == 1)		// yes/no
		{
			if(ituWidgetIsVisible(M9_Yes_Sel) == true)
			{
				M9_UserYes(NULL,"1");
			}
			else if(ituWidgetIsVisible(M9_No_Sel) == true)
			{
				M9_UserNo(NULL,"1");
			}
		}
		else if(M9_ResetFlag == 0)		// reset
		{
			if(M9_ResetSel == 1)
				M9_UserReset(NULL,"1");
		}
	}

    return true;
}

bool Menu9_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t tJogSwitch = 0;
	
	tJogSwitch = param[0];

	if(tJogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
		M9D5_BTNMouseUp(NULL, NULL);
	}
	else if(tJogSwitch == JOG_DIAL_SW_HOME_OFF)
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}

    return true;
}

bool M9_UserNo(ITUWidget* widget, char* param)
{
	uint8_t tBTN_Status;
	ITUWidget *M9_No_Sel,*M9_No_BTN;
	ITUWidget *M9_Yes_Sel,*M9_Yes_BTN;

	uint8_t tLANG;
	ITUWidget *M9_Reset_Sel,*M9_Reset_BTN;
	ITUWidget *M9_MSG_ENG,*M9_MSG_DEU,*M9_MSG_FRA,*M9_MSG_ITA,*M9_MSG_ESP;

	M9_No_Sel = ituSceneFindWidget(&theScene, "M9_No_SEL");
	assert(M9_No_Sel);
	M9_No_BTN = ituSceneFindWidget(&theScene, "M9_No_BTN");
	assert(M9_No_BTN);

	M9_Yes_Sel = ituSceneFindWidget(&theScene, "M9_Yes_SEL");
	assert(M9_Yes_Sel);
	M9_Yes_BTN = ituSceneFindWidget(&theScene, "M9_Yes_BTN");
	assert(M9_Yes_BTN);
	
	M9_Reset_Sel = ituSceneFindWidget(&theScene, "M9_1_SEL");
	assert(M9_Reset_Sel);
	M9_Reset_BTN = ituSceneFindWidget(&theScene, "M9_1_BTN");
	assert(M9_Reset_BTN);

	M9_MSG_ENG = ituSceneFindWidget(&theScene, "M9_C_MSG_ENG");
	assert(M9_MSG_ENG);
	M9_MSG_DEU = ituSceneFindWidget(&theScene, "M9_C_MSG_DEU");
	assert(M9_MSG_DEU);
	M9_MSG_FRA = ituSceneFindWidget(&theScene, "M9_C_MSG_FRA");
	assert(M9_MSG_FRA);
	M9_MSG_ITA = ituSceneFindWidget(&theScene, "M9_C_MSG_ITA");
	assert(M9_MSG_ITA);
	M9_MSG_ESP = ituSceneFindWidget(&theScene, "M9_C_MSG_ESP");
	assert(M9_MSG_ESP);

	
	tBTN_Status = (uint8_t)atoi(param);	// 0; down , 1;up
	
	if(tBTN_Status == 0)		// down
	{
		ituWidgetSetVisible(M9_No_Sel,true);
		ituWidgetSetVisible(M9_Yes_Sel,false);
	}
	else if(tBTN_Status == 1)		// up
	{
		M9_ResetFlag = 0;		// yes/no btn
		
		ituWidgetSetVisible(M9_No_Sel,false);
		ituWidgetSetVisible(M9_Yes_Sel,false);
		ituWidgetSetVisible(M9_Yes_BTN,false);
		ituWidgetSetVisible(M9_No_BTN,false);

		ituWidgetSetVisible(M9_Reset_Sel,false);

		// confirm msg display
		ituWidgetSetVisible(M9_MSG_DEU,false);
		ituWidgetSetVisible(M9_MSG_FRA,false);
		ituWidgetSetVisible(M9_MSG_ITA,false);
		ituWidgetSetVisible(M9_MSG_ESP,false);
		ituWidgetSetVisible(M9_MSG_ENG,false);
		
		ituWidgetSetVisible(M9_Reset_BTN,true);

		Menu9_SetMode(M9_MODE_RESET);//chpark.24.04.11
	}
    return true;
}

bool M9_UserYes(ITUWidget* widget, char* param)
{
	uint8_t tBTN_Status;
	ITUWidget *M9_Yes_Sel,*M9_Yes_BTN;
	ITUWidget *M9_No_Sel,*M9_No_BTN;

	uint8_t tLANG;
	ITUWidget *M9_Reset_Sel,*M9_Reset_BTN;
	ITUWidget *M9_MSG_ENG,*M9_MSG_DEU,*M9_MSG_FRA,*M9_MSG_ITA,*M9_MSG_ESP;

	M9_Yes_Sel = ituSceneFindWidget(&theScene, "M9_Yes_SEL");
	assert(M9_Yes_Sel);
	M9_Yes_BTN = ituSceneFindWidget(&theScene, "M9_Yes_BTN");
	assert(M9_Yes_BTN);

	M9_No_Sel = ituSceneFindWidget(&theScene, "M9_No_SEL");
	assert(M9_No_Sel);
	M9_No_BTN = ituSceneFindWidget(&theScene, "M9_No_BTN");
	assert(M9_No_BTN);

	M9_Reset_Sel = ituSceneFindWidget(&theScene, "M9_1_SEL");
	assert(M9_Reset_Sel);
	M9_Reset_BTN = ituSceneFindWidget(&theScene, "M9_1_BTN");
	assert(M9_Reset_BTN);

	M9_MSG_ENG = ituSceneFindWidget(&theScene, "M9_C_MSG_ENG");
	assert(M9_MSG_ENG);
	M9_MSG_DEU = ituSceneFindWidget(&theScene, "M9_C_MSG_DEU");
	assert(M9_MSG_DEU);
	M9_MSG_FRA = ituSceneFindWidget(&theScene, "M9_C_MSG_FRA");
	assert(M9_MSG_FRA);
	M9_MSG_ITA = ituSceneFindWidget(&theScene, "M9_C_MSG_ITA");
	assert(M9_MSG_ITA);
	M9_MSG_ESP = ituSceneFindWidget(&theScene, "M9_C_MSG_ESP");
	assert(M9_MSG_ESP);
	
	tBTN_Status = (uint8_t)atoi(param);	// 0; down , 1;up
	
	if(tBTN_Status == 0)		// down
	{
		ituWidgetSetVisible(M9_Yes_Sel,true);
		ituWidgetSetVisible(M9_No_Sel,false);
	}
	else if(tBTN_Status == 1)		// up
	{
		M9_ResetFlag = 0;		// yes/no btn
		
		HMV.HourMeter_Trip_Clear2 = HMV.HourMeter_Trip_Clear2|0x10;		// CLI 2byte,4-5bit // 01;on , 00;off

		ituWidgetSetVisible(M9_No_Sel,false);
		ituWidgetSetVisible(M9_Yes_Sel,false);
		ituWidgetSetVisible(M9_Yes_BTN,false);
		ituWidgetSetVisible(M9_No_BTN,false);

		ituWidgetSetVisible(M9_Reset_Sel,false);

		// confirm msg display
		ituWidgetSetVisible(M9_MSG_DEU,false);
		ituWidgetSetVisible(M9_MSG_FRA,false);
		ituWidgetSetVisible(M9_MSG_ITA,false);
		ituWidgetSetVisible(M9_MSG_ESP,false);
		ituWidgetSetVisible(M9_MSG_ENG,false);
		
		ituWidgetSetVisible(M9_Reset_BTN,true);

		Menu9_SetMode(M9_MODE_RESET);//chpark.24.04.11
	}

    return true;
}

void M9_ResetScreen(void)
{
	ITUWidget *M9_Reset_Sel,*M9_Reset_BTN;
	ITUWidget *M9_Yes_Sel,*M9_No_Sel;
	ITUWidget *M9_Yes_BTN,*M9_No_BTN;
	ITUWidget *M9_MSG_ENG,*M9_MSG_DEU,*M9_MSG_FRA,*M9_MSG_ITA,*M9_MSG_ESP;
	
	M9_Reset_Sel = ituSceneFindWidget(&theScene, "M9_1_SEL");
	assert(M9_Reset_Sel);
	M9_Reset_BTN = ituSceneFindWidget(&theScene, "M9_1_BTN");
	assert(M9_Reset_BTN);

	M9_Yes_Sel = ituSceneFindWidget(&theScene, "M9_Yes_SEL");
	assert(M9_Yes_Sel);
	M9_Yes_BTN = ituSceneFindWidget(&theScene, "M9_Yes_BTN");
	assert(M9_Yes_BTN);

	M9_No_Sel = ituSceneFindWidget(&theScene, "M9_No_SEL");
	assert(M9_No_Sel);
	M9_No_BTN = ituSceneFindWidget(&theScene, "M9_No_BTN");
	assert(M9_No_BTN);

	M9_MSG_ENG = ituSceneFindWidget(&theScene, "M9_C_MSG_ENG");
	assert(M9_MSG_ENG);
	M9_MSG_DEU = ituSceneFindWidget(&theScene, "M9_C_MSG_DEU");
	assert(M9_MSG_DEU);
	M9_MSG_FRA = ituSceneFindWidget(&theScene, "M9_C_MSG_FRA");
	assert(M9_MSG_FRA);
	M9_MSG_ITA = ituSceneFindWidget(&theScene, "M9_C_MSG_ITA");
	assert(M9_MSG_ITA);
	M9_MSG_ESP = ituSceneFindWidget(&theScene, "M9_C_MSG_ESP");
	assert(M9_MSG_ESP);

	ituWidgetSetVisible(M9_Reset_Sel,false);

	ituWidgetSetVisible(M9_Yes_Sel,false);
	ituWidgetSetVisible(M9_No_Sel,false);
	ituWidgetSetVisible(M9_Yes_BTN,false);
	ituWidgetSetVisible(M9_No_BTN,false);
	
	ituWidgetSetVisible(M9_MSG_ENG,false);
	ituWidgetSetVisible(M9_MSG_DEU,false);
	ituWidgetSetVisible(M9_MSG_FRA,false);
	ituWidgetSetVisible(M9_MSG_ITA,false);
	ituWidgetSetVisible(M9_MSG_ESP,false);

	ituWidgetSetVisible(M9_Reset_BTN,true);
}

void M9_ResetSelDisplay(uint8_t tFlag)
{
	ITUWidget *M9_Reset_Sel = ituSceneFindWidget(&theScene, "M9_1_SEL");
	assert(M9_Reset_Sel);
	
	if(tFlag == 1)
	{
		M9_ResetSel = 1;
		ituWidgetSetVisible(M9_Reset_Sel,true);
	}
	else
	{
		M9_ResetSel = 0;
		ituWidgetSetVisible(M9_Reset_Sel,false);
	}
}

bool M9_UserReset(ITUWidget* widget, char* param)
{
	uint8_t tBTN_Status,tLANG;
	ITUWidget *M9_Reset_Sel,*M9_Reset_BTN;
	ITUWidget *M9_Yes_BTN,*M9_No_BTN;
	ITUWidget *M9_MSG_ENG,*M9_MSG_DEU,*M9_MSG_FRA,*M9_MSG_ITA,*M9_MSG_ESP;
	
	M9_Reset_Sel = ituSceneFindWidget(&theScene, "M9_1_SEL");
	assert(M9_Reset_Sel);
	M9_Reset_BTN = ituSceneFindWidget(&theScene, "M9_1_BTN");
	assert(M9_Reset_BTN);

	M9_Yes_BTN = ituSceneFindWidget(&theScene, "M9_Yes_BTN");
	assert(M9_Yes_BTN);
	M9_No_BTN = ituSceneFindWidget(&theScene, "M9_No_BTN");
	assert(M9_No_BTN);

	M9_MSG_ENG = ituSceneFindWidget(&theScene, "M9_C_MSG_ENG");
	assert(M9_MSG_ENG);
	M9_MSG_DEU = ituSceneFindWidget(&theScene, "M9_C_MSG_DEU");
	assert(M9_MSG_DEU);
	M9_MSG_FRA = ituSceneFindWidget(&theScene, "M9_C_MSG_FRA");
	assert(M9_MSG_FRA);
	M9_MSG_ITA = ituSceneFindWidget(&theScene, "M9_C_MSG_ITA");
	assert(M9_MSG_ITA);
	M9_MSG_ESP = ituSceneFindWidget(&theScene, "M9_C_MSG_ESP");
	assert(M9_MSG_ESP);

	tBTN_Status = (uint8_t)atoi(param);	// 0; down , 1;up
	
	if(tBTN_Status == 0)		// down
	{
		// 
		M9_ResetSel = 1;
		ituWidgetSetVisible(M9_Reset_Sel,true);
	}
	else if(tBTN_Status == 1)		// up
	{
		M9_ResetSel = 0;
		M9_ResetFlag = 1;		// yes/no btn
		
		ituWidgetSetVisible(M9_Reset_Sel,false);
		ituWidgetSetVisible(M9_Reset_BTN,false);

		// confirm msg display
		tLANG = Lang_Flag[M7_UserIndex];		// S5 set
		
		switch(tLANG)
		{
			case Lang_ENG:
			{
				ituWidgetSetVisible(M9_MSG_DEU,false);
				ituWidgetSetVisible(M9_MSG_FRA,false);
				ituWidgetSetVisible(M9_MSG_ITA,false);
				ituWidgetSetVisible(M9_MSG_ESP,false);
				ituWidgetSetVisible(M9_MSG_ENG,true);
			}
			break;
			case Lang_DEU:
			{
				ituWidgetSetVisible(M9_MSG_ENG,false);
				ituWidgetSetVisible(M9_MSG_FRA,false);
				ituWidgetSetVisible(M9_MSG_ITA,false);
				ituWidgetSetVisible(M9_MSG_ESP,false);
				ituWidgetSetVisible(M9_MSG_DEU,true);
			}
			break;
			case Lang_FRA:
			{
				ituWidgetSetVisible(M9_MSG_ENG,false);
				ituWidgetSetVisible(M9_MSG_DEU,false);
				ituWidgetSetVisible(M9_MSG_ITA,false);
				ituWidgetSetVisible(M9_MSG_ESP,false);
				ituWidgetSetVisible(M9_MSG_FRA,true);
			}
			break;
			case Lang_ITA:
			{
				ituWidgetSetVisible(M9_MSG_ENG,false);
				ituWidgetSetVisible(M9_MSG_DEU,false);
				ituWidgetSetVisible(M9_MSG_FRA,false);
				ituWidgetSetVisible(M9_MSG_ESP,false);
				ituWidgetSetVisible(M9_MSG_ITA,true);
			}
			break;
			case Lang_ESP:
			{
				ituWidgetSetVisible(M9_MSG_ENG,false);
				ituWidgetSetVisible(M9_MSG_DEU,false);
				ituWidgetSetVisible(M9_MSG_FRA,false);
				ituWidgetSetVisible(M9_MSG_ITA,false);
				ituWidgetSetVisible(M9_MSG_ESP,true);
			}
			break;
		}
	
		ituWidgetSetVisible(M9_Yes_BTN,true);
		ituWidgetSetVisible(M9_No_BTN,true);

		Menu9_SetMode(M9_MODE_YESNO);//chpark.24.04.11
	} 

    return true;
}

bool M9D7_BTNMouseUp(ITUWidget* widget, char* param)
{
    return true;
}

bool M9D6_BTNMouseUp(ITUWidget* widget, char* param)
{
	GotoMain(MAIN_MODE_NORMAL); 

    return true;
}

bool M9D5_BTNMouseUp(ITUWidget* widget, char* param)
{
	if(M9_ResetFlag == 1)		// yes/no btn
	{
		M9_ResetFlag = 0;
		M9_UserNo(NULL,"1");
	}
	else
	{
		GotoMenuTop();
	}
	
    return true;
}

bool M9_UserDisplay(void)
{
	uint8_t tDestName[100];

	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
		return 0;

	ITUWidget *M9_TopAdmin = ituSceneFindWidget(&theScene, "M9_TopAdmin");
	assert(M9_TopAdmin);
	ITUWidget *M9_TopStandard = ituSceneFindWidget(&theScene, "M9_TopStandard");
	assert(M9_TopStandard);
	
	ITUText *M9_TopUserName = ituSceneFindWidget(&theScene, "M9_TopUserName");
	assert(M9_TopUserName);
	
	if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
	{
		ituWidgetSetVisible(M9_TopStandard,false);
		ituWidgetSetVisible(M9_TopAdmin,true);
	}
	else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
	{
		ituWidgetSetVisible(M9_TopAdmin,false);
		ituWidgetSetVisible(M9_TopStandard,true);
	}
	else
	{
		ituWidgetSetVisible(M9_TopAdmin,false);
		ituWidgetSetVisible(M9_TopStandard,false);
	}

	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M9_TopUserName, tDestName);

		ituWidgetSetVisible(M9_TopUserName,true);
	}
	else
	{
		ituWidgetSetVisible(M9_TopUserName,false);
	}
/*
	if((gMainTimerCnt_10m - gMenu_UI_Time) < 500)		// 5sec	
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M9_TopUserName, tDestName);

		ituWidgetSetVisible(M9_TopUserName,true);
	}
	else if(((gMainTimerCnt_10m - gMenu_UI_Time) >= 500)||(M7_UserIndex > (MAX_SU100_USER_NUM-1)))
	{
		ituWidgetSetVisible(M9_TopUserName,false);
	}
*/
}

void User_MeterDisplay2(uint8_t tLine,uint8_t tUserIdx)
{
	uint8_t tDestName[100],tUserLv;

	char tmp_STR[20],tmp_END[20];
	uint32_t tSTR_Time,tEND_Time;
//	printf("User_MeterDisplay2 [%d] [%d]  [0x%X] [0x%X]\n",tLine,tUserIdx,UserMeter[tLine].tSrtTime,UserMeter[tLine].tEndTime);
	if((tUserIdx >= 1)&&(tUserIdx <= 253))
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[tUserIdx-1].UserName);
		ituTextSetString(M9_UserName[tLine], tDestName);

		tUserLv = User_RegInfo[tUserIdx-1].RegFlag;

		memset(tmp_STR,0,20);
		memset(tmp_END,0,20);
	
		tSTR_Time = UserMeter[tLine].tSrtTime;
		tEND_Time = UserMeter[tLine].tEndTime;

		if(tSTR_Time == 0x00FFFFFF)
		{
			sprintf(tmp_STR,"N/A    ");
		}
		else
		{
			if(tSTR_Time > 999999)
				tSTR_Time = 999999;
				
			sprintf(tmp_STR,"%d.%d",tSTR_Time/10,tSTR_Time%10);
		}
		ituTextSetString(M9_STR_L[tLine], tmp_STR);

		if(tLine == 0)
		{
			if(UserMeter[tLine].tEndTime == 0xFFFFFFFF)
			{
				sprintf(tmp_END,"N/A    ");
			}
			else
			{
				if(tEND_Time > 999999)
					tEND_Time = 999999;
				
				sprintf(tmp_END,"%d.%d",tEND_Time/10,tEND_Time%10);
			}
		}
		else
		{
			if(tEND_Time == 0x00FFFFFF)
			{
				sprintf(tmp_END,"N/A    ");
			}
			else
			{
				if(tEND_Time > 999999)
					tEND_Time = 999999;

				sprintf(tmp_END,"%d.%d",tEND_Time/10,tEND_Time%10);
			}
		}

		ituTextSetString(M9_END_L[tLine], tmp_END);

		if(tUserLv == 5)	// admin
		{
//			printf("Line %d > User[%d] admin(%d) ",tLine+1,tUserIdx-1,tUserLv);
//			printf("true\r\n");
			
			// icon
			ituWidgetSetVisible(M9_UserIcon_Admin[tLine],true);
			ituWidgetSetVisible(M9_UserIcon_Standard[tLine],false);
			
			// name
			ituWidgetSetVisible(M9_UserName[tLine],true);
			
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
				ituTextSetFontWidth(M9_STR_L[tLine],60);
				ituTextSetFontHeight(M9_STR_L[tLine],60);

				ituTextSetFontWidth(M9_END_L[tLine],60);
				ituTextSetFontHeight(M9_END_L[tLine],60);
			}
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				ituTextSetFontWidth(M9_STR_L[tLine],48);
				ituTextSetFontHeight(M9_STR_L[tLine],50);

				ituTextSetFontWidth(M9_END_L[tLine],48);
				ituTextSetFontHeight(M9_END_L[tLine],50);
			}
			
			ituWidgetSetVisible(M9_STR_L[tLine],true);
			ituWidgetSetVisible(M9_END_L[tLine],true);
		}
		else if((tUserLv == 2)||(tUserLv == 3)||(tUserLv == 9))		// standard
		{
//			printf("Line %d > User[%d] Standard(%d) ",tLine+1,tUserIdx-1,tUserLv);
//			printf("true\r\n");
			
			// icon
			ituWidgetSetVisible(M9_UserIcon_Admin[tLine],false);
			ituWidgetSetVisible(M9_UserIcon_Standard[tLine],true);

			// name
			ituWidgetSetVisible(M9_UserName[tLine],true);
			


			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
				ituTextSetFontWidth(M9_STR_L[tLine],60);
				ituTextSetFontHeight(M9_STR_L[tLine],60);

				ituTextSetFontWidth(M9_END_L[tLine],60);
				ituTextSetFontHeight(M9_END_L[tLine],60);
			}
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				ituTextSetFontWidth(M9_STR_L[tLine],48);
				ituTextSetFontHeight(M9_STR_L[tLine],50);

				ituTextSetFontWidth(M9_END_L[tLine],48);
				ituTextSetFontHeight(M9_END_L[tLine],50);
			}

			ituWidgetSetVisible(M9_STR_L[tLine],true);
			ituWidgetSetVisible(M9_END_L[tLine],true);
		}
		else // if((tUserLv == 0)||(tUserLv == 15))	// ������ menu 3��	JPHong 2024-03-20
		{
//			printf("Line %d > User[%d] Standard(%d) ",tLine+1,tUserIdx-1,tUserLv);
//			printf("false\r\n");

			// icon
			ituWidgetSetVisible(M9_UserIcon_Admin[tLine],false);
			ituWidgetSetVisible(M9_UserIcon_Standard[tLine],false);

			// name
			ituWidgetSetVisible(M9_UserName[tLine],true);

			// meter data
			ituWidgetSetVisible(M9_STR_L[tLine],false);
			ituWidgetSetVisible(M9_END_L[tLine],false);
		}
	}
	else if((tUserIdx == 0)||(tUserIdx == 0xFE)||(tUserIdx == 0xFF))
	{
//		printf("Line %d , Not Define UserIndex ",tLine+1);
//		printf("None\r\n");

		// icon
		ituWidgetSetVisible(M9_UserIcon_Admin[tLine],false);
		ituWidgetSetVisible(M9_UserIcon_Standard[tLine],false);

		// name
		ituWidgetSetVisible(M9_UserName[tLine],false);

		// meter data		
		ituWidgetSetVisible(M9_STR_L[tLine],false);
		ituWidgetSetVisible(M9_END_L[tLine],false);
	}
	
//	printf(" UserMeter[4].tUserNum:[%d],[%d]\n", UserMeter[0].tUserNum, UserMeter[0].tUserNum);
}

void User_IconDisplay(uint8_t tLine,uint8_t tUserNum,bool Disp_Flag)
{
	if(tUserNum > 0)
	{
		if(User_RegInfo[tUserNum-1].RegFlag == 5)		// admin
		{
			printf("Line %d , [%d] admin(%d) ",tLine+1,tUserNum-1,User_RegInfo[tUserNum-1].RegFlag);
    
			if(Disp_Flag)			// visuable
			{
				printf("true\r\n");
				ituWidgetSetVisible(M9_UserIcon_Admin[tLine],true);
				ituWidgetSetVisible(M9_UserIcon_Standard[tLine],false);
			}
			else
			{
				printf("false\r\n");
				ituWidgetSetVisible(M9_UserIcon_Admin[tLine],false);
				ituWidgetSetVisible(M9_UserIcon_Standard[tLine],true);
			}
		}
		else if((User_RegInfo[tUserNum-1].RegFlag == 2)||(User_RegInfo[tUserNum-1].RegFlag == 3)||(User_RegInfo[tUserNum-1].RegFlag == 9))		// standard
		{
			printf("Line %d , [%d] standrad(%d) ",tLine+1,tUserNum-1,User_RegInfo[tUserNum-1].RegFlag);
    
			if(Disp_Flag)			// visuable
			{
				printf("true\r\n");
				ituWidgetSetVisible(M9_UserIcon_Admin[tLine],false);
				ituWidgetSetVisible(M9_UserIcon_Standard[tLine],true);
			}
			else
			{
				printf("false\r\n");
				ituWidgetSetVisible(M9_UserIcon_Admin[tLine],true);
				ituWidgetSetVisible(M9_UserIcon_Standard[tLine],false);
			}
		}
		else if((User_RegInfo[tUserNum-1].RegFlag == 0)||(User_RegInfo[tUserNum-1].RegFlag == 15))
		{
			printf("Line %d , [%d] icon(%d) ",tLine+1,tUserNum-1,User_RegInfo[tUserNum-1].RegFlag);
    
			printf("None\r\n");
			ituWidgetSetVisible(M9_UserIcon_Admin[tLine],false);
			ituWidgetSetVisible(M9_UserIcon_Standard[tLine],false);
		}
	}
	else if(tUserNum == 0)
	{
		printf("Line %d , [%d] icon(%d) ",tLine+1,tUserNum,User_RegInfo[tUserNum].RegFlag);
    
		printf("None\r\n");
		ituWidgetSetVisible(M9_UserIcon_Admin[tLine],false);
		ituWidgetSetVisible(M9_UserIcon_Standard[tLine],false);
	}
}

void User_NameDisplay(uint8_t tLine,uint8_t tUserNum,bool Disp_Flag)
{
	uint8_t tDestName[100];

	memset(tDestName,0,100);
	
	SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[tUserNum-1].UserName);
	ituTextSetString(M9_UserName[tLine], tDestName);
	
	printf("Line %d , UserName ; %s ",tLine+1,tDestName);

	if(Disp_Flag)			// visuable
	{
		printf("true\r\n");
		ituWidgetSetVisible(M9_UserName[tLine],true);
	}
	else
	{
		printf("false\r\n");
		ituWidgetSetVisible(M9_UserName[tLine],false);
	}
}

void User_MeterDisplay(uint8_t tLine,uint8_t tUserNum,bool Disp_Flag)
{
	char tmp_STR[20],tmp_END[20];
	uint32_t tSTR_Time,tEND_Time;
	
	memset(tmp_STR,0,20);
	memset(tmp_END,0,20);
	
	tSTR_Time = UserMeter[tLine].tSrtTime;
	tEND_Time = UserMeter[tLine].tEndTime;
	
	sprintf(tmp_STR,"%d.%d",tSTR_Time/10,tSTR_Time%10);
	ituTextSetString(M9_STR_L[tLine], tmp_STR);
	
	sprintf(tmp_END,"%d.%d",tEND_Time/10,tEND_Time%10);
	ituTextSetString(M9_END_L[tLine], tmp_END);
	
	if(Disp_Flag)			// visuable
	{
		ituWidgetSetVisible(M9_STR_L[tLine],true);
		ituWidgetSetVisible(M9_END_L[tLine],true);
	}
	else
	{
		ituWidgetSetVisible(M9_STR_L[tLine],false);
		ituWidgetSetVisible(M9_END_L[tLine],false);
	}
}


