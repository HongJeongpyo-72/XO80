#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "MenuTop_Layer.h"

/* widgets:
MenuTop_Layer
MenuTop_BG
MT_TopUserName
MT_TopStandard
MT_TopAdmin
MTD7_BTN
MTD6_BTN
MTD5_BTN
M1_8_ZoomBTN
M1_8_ANI
M1_8_Up1
M1_8_Up0
M1_8_BTN
M1_7_ZoomBTN
M1_7_ANI
M1_7_Up1
M1_7_Up0
M1_7_BTN
M1_6_ZoomBTN
M1_6_ANI
M1_6_Up1
M1_6_Up0
M1_6_BTN
M1_5_ZoomBTN
M1_5_ANI
M1_5_Up1
M1_5_Up0
M1_5_BTN
M1_4_ZoomBTN
M1_4_ANI
M1_4_Up1
M1_4_Up0
M1_4_BTN
M1_3_ZoomBTN
M1_3_ANI
M1_3_Up1
M1_3_Up0
M1_3_BTN
M1_2_ZoomBTN
M1_2_ANI
M1_2_Up1
M1_2_Up0
M1_2_BTN
M1_1b_ZoomBTN
M1_1b_ANI
M1_1b_Up1
M1_1b_Up0
M1_1b_BTN
M1_1a_ZoomBTN
M1_1a_ANI
M1_1a_Up1
M1_1a_Up0
M1_1a_BTN
*/

#define Menu_Top					0
#define Menu_Trip					1
#define Menu_AUX					2
#define Menu_LCD					3
#define Menu_Data					4
#define Menu_Time					5
#define Menu_Preset				6
#define Menu_ErCode				7
#define Menu_UserDefine		8

//static uint8_t gMenuIdx;
//static uint8_t gMT_ChoiceMenu = Menu_Trip;

//static uint32_t gMT_Timer;					// jhlee 231005
static uint32_t gMT_ExitTimer = 0;		// jhlee 231005
static uint32_t gMT_DeepTimer = 0;		// jhlee 231005

static uint8_t MT_1a_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t MT_1b_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t MT_2_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t MT_3_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t MT_4_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t MT_5_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t MT_6_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t MT_7_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t MT_8_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down

//static uint8_t MT_DeepCHK = 0;		// jhlee 231005
static uint8_t MT_DeepCHK_JOG = 0;	// JPHong 2023-07-08
uint8_t MT_D5_Flag = 0;
uint8_t MT_D7_Flag = 0;

uint32_t gMenu_UI_Time = 0;

static uint8_t gMT_UserFlag = 0;

static uint8_t JW_1TimeFlag = 1;
static uint32_t tJogTimer = 0;
char PARA[2];

void GotoMenuTop(void);
void GotoDeepMenu(void);
void MT_ChoiceMenuClear(uint8_t MenuIndex);
void MT_8_Display(uint8_t tDis_Flag);

bool M1_BTN_MouseDn(ITUWidget* widget, char* param);
bool M1_BTN_MouseUp(ITUWidget* widget, char* param);

extern uint8_t Get_PL_Flag(void);			// extern

#define RegMode_Dev 1

//uint32_t ANI_TImer = 0;		// jhlee 231005

extern bool DOUBLE_KEY_ENABLE_FLAG;	// JPHong 0727 #10956


bool MT_AniMenuStatus(void);

bool M1_1a_ANIStopped(ITUWidget* widget, char* param);
bool M1_1b_ANIStopped(ITUWidget* widget, char* param);
bool M1_2_ANIStopped(ITUWidget* widget, char* param);
bool M1_3_ANIStopped(ITUWidget* widget, char* param);
bool M1_4_ANIStopped(ITUWidget* widget, char* param);
bool M1_5_ANIStopped(ITUWidget* widget, char* param);
bool M1_6_ANIStopped(ITUWidget* widget, char* param);
bool M1_7_ANIStopped(ITUWidget* widget, char* param);
bool M1_8_ANIStopped(ITUWidget* widget, char* param);

bool MTD6_MouseUp(ITUWidget* widget, char* param);
uint8_t tM1_1a_ANIStopped=0xFF;		// JPHong 2024-03-06
uint8_t tM1_1b_ANIStopped=0xFF;
uint8_t tM1_2_ANIStopped=0xFF;
uint8_t tM1_3_ANIStopped=0xFF;
uint8_t tM1_4_ANIStopped=0xFF;
uint8_t tM1_5_ANIStopped=0xFF;
uint8_t tM1_6_ANIStopped=0xFF;
uint8_t tM1_7_ANIStopped=0xFF;
uint8_t tM1_8_ANIStopped=0xFF;


static ITUButton* MTD7_BTN;
static ITUButton* MTD6_BTN;
static ITUButton* MTD5_BTN;
static ITUButton* M1_8_ZoomBTN;
static ITUAnimation* M1_8_ANI;
static ITUIcon* M1_8_Up1;
static ITUIcon* M1_8_Up0;
static ITUButton* M1_8_BTN;
static ITUButton* M1_7_ZoomBTN;
static ITUAnimation* M1_7_ANI;
static ITUIcon* M1_7_Up1;
static ITUIcon* M1_7_Up0;
static ITUButton* M1_7_BTN;
static ITUButton* M1_6_ZoomBTN;
static ITUAnimation* M1_6_ANI;
static ITUIcon* M1_6_Up1;
static ITUIcon* M1_6_Up0;
static ITUButton* M1_6_BTN;
static ITUButton* M1_5_ZoomBTN;
static ITUAnimation* M1_5_ANI;
static ITUIcon* M1_5_Up1;
static ITUIcon* M1_5_Up0;
static ITUButton* M1_5_BTN;
static ITUButton* M1_4_ZoomBTN;
static ITUAnimation* M1_4_ANI;
static ITUIcon* M1_4_Up1;
static ITUIcon* M1_4_Up0;
static ITUButton* M1_4_BTN;
static ITUButton* M1_3_ZoomBTN;
static ITUAnimation* M1_3_ANI;
static ITUIcon* M1_3_Up1;
static ITUIcon* M1_3_Up0;
static ITUButton* M1_3_BTN;
static ITUButton* M1_2_ZoomBTN;
static ITUAnimation* M1_2_ANI;
static ITUIcon* M1_2_Up1;
static ITUIcon* M1_2_Up0;
static ITUButton* M1_2_BTN;
static ITUButton* M1_1b_ZoomBTN;
static ITUAnimation* M1_1b_ANI;
static ITUIcon* M1_1b_Up1;
static ITUIcon* M1_1b_Up0;
static ITUButton* M1_1b_BTN;
static ITUButton* M1_1a_ZoomBTN;
static ITUAnimation* M1_1a_ANI;
static ITUIcon* M1_1a_Up1;
static ITUIcon* M1_1a_Up0;
static ITUButton* M1_1a_BTN;


uint8_t MT_8_OnOff_Flag = 0xFF;	// JPHong 2024-03-06

bool MenuLayerFlag = 0;

bool MT_UserDisplay(void);
extern bool Menu7LayerFlag;
bool MT_BTN_MouseUpFlag = 0;


void Check_Null_M7_UserName();//chpark.24.04.05


void ResetMenu(void)
{
	MenuLayerFlag = 0;
	Menu7LayerFlag = 0;
}


bool MenuTop_Enter(ITUWidget* widget, char* param)
{
	XO_ituButtonSetMode_Bt(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
	XO_ituButtonSetMode_TP(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
printf("\n[MenuTop_Enter Start] : %d %d %d\n",Main_Disp_Mode,MainBtFlag,Main_Disp_Mode_Flag);
	//gMenuIdx = Menu_Index;
	//gMT_ChoiceMenu = MenuTop_Choice_Index;
	Main_Disp_Mode = MAIN_MODE_MENU;	// JPHong 2023-04-18
//AUX_Value_CNT = gMainTimerCnt_10m;	// JPHong 2023-05-30
//M5_UTemp_Timer = gMainTimerCnt_10m;
//M5_Urea_Timer = gMainTimerCnt_10m;
//M5_Batt_Timer = gMainTimerCnt_10m;
//M5_Fuel_Timer = gMainTimerCnt_10m;
//M5_OTemp_Timer = gMainTimerCnt_10m;
//M5_RPM_Timer = gMainTimerCnt_10m;
//M5_WTemp_Timer = gMainTimerCnt_10m;
//HM_Time = gMainTimerCnt_10m;  // JPHong 0824
//M5_PM_Timer = gMainTimerCnt_10m;
//M5_ASH_Timer = gMainTimerCnt_10m;
//TM1_Time = gMainTimerCnt_10m;
//TM2_Time = gMainTimerCnt_10m;
//TM3_Time = gMainTimerCnt_10m;

	MT_BTN_MouseUpFlag = 0;
	if (MenuLayerFlag==0)
	{
		SetWidget(MTD7_BTN,"MTD7_BTN");
		SetWidget(MTD6_BTN,"MTD6_BTN");
		SetWidget(MTD5_BTN,"MTD5_BTN");
		SetWidget(M1_8_ZoomBTN,"M1_8_ZoomBTN");
		SetWidget(M1_8_ANI,"M1_8_ANI");
		SetWidget(M1_8_Up1,"M1_8_Up1");
		SetWidget(M1_8_Up0,"M1_8_Up0");
		SetWidget(M1_8_BTN,"M1_8_BTN");
		SetWidget(M1_7_ZoomBTN,"M1_7_ZoomBTN");
		SetWidget(M1_7_ANI,"M1_7_ANI");
		SetWidget(M1_7_Up1,"M1_7_Up1");
		SetWidget(M1_7_Up0,"M1_7_Up0");
		SetWidget(M1_7_BTN,"M1_7_BTN");
		SetWidget(M1_6_ZoomBTN,"M1_6_ZoomBTN");
		SetWidget(M1_6_ANI,"M1_6_ANI");
		SetWidget(M1_6_Up1,"M1_6_Up1");
		SetWidget(M1_6_Up0,"M1_6_Up0");
		SetWidget(M1_6_BTN,"M1_6_BTN");
		SetWidget(M1_5_ZoomBTN,"M1_5_ZoomBTN");
		SetWidget(M1_5_ANI,"M1_5_ANI");
		SetWidget(M1_5_Up1,"M1_5_Up1");
		SetWidget(M1_5_Up0,"M1_5_Up0");
		SetWidget(M1_5_BTN,"M1_5_BTN");
		SetWidget(M1_4_ZoomBTN,"M1_4_ZoomBTN");
		SetWidget(M1_4_ANI,"M1_4_ANI");
		SetWidget(M1_4_Up1,"M1_4_Up1");
		SetWidget(M1_4_Up0,"M1_4_Up0");
		SetWidget(M1_4_BTN,"M1_4_BTN");
		SetWidget(M1_3_ZoomBTN,"M1_3_ZoomBTN");
		SetWidget(M1_3_ANI,"M1_3_ANI");
		SetWidget(M1_3_Up1,"M1_3_Up1");
		SetWidget(M1_3_Up0,"M1_3_Up0");
		SetWidget(M1_3_BTN,"M1_3_BTN");
		SetWidget(M1_2_ZoomBTN,"M1_2_ZoomBTN");
		SetWidget(M1_2_ANI,"M1_2_ANI");
		SetWidget(M1_2_Up1,"M1_2_Up1");
		SetWidget(M1_2_Up0,"M1_2_Up0");
		SetWidget(M1_2_BTN,"M1_2_BTN");
		SetWidget(M1_1b_ZoomBTN,"M1_1b_ZoomBTN");
		SetWidget(M1_1b_ANI,"M1_1b_ANI");
		SetWidget(M1_1b_Up1,"M1_1b_Up1");
		SetWidget(M1_1b_Up0,"M1_1b_Up0");
		SetWidget(M1_1b_BTN,"M1_1b_BTN");
		SetWidget(M1_1a_ZoomBTN,"M1_1a_ZoomBTN");
		SetWidget(M1_1a_ANI,"M1_1a_ANI");
		SetWidget(M1_1a_Up1,"M1_1a_Up1");
		SetWidget(M1_1a_Up0,"M1_1a_Up0");
		SetWidget(M1_1a_BTN,"M1_1a_BTN");
		
	tM1_1a_ANIStopped=0xFF;		// JPHong 2024-03-06
	tM1_1b_ANIStopped=0xFF;
	tM1_2_ANIStopped=0xFF;
	tM1_3_ANIStopped=0xFF;
	tM1_4_ANIStopped=0xFF;
	tM1_5_ANIStopped=0xFF;
	tM1_6_ANIStopped=0xFF;
	tM1_7_ANIStopped=0xFF;
	tM1_8_ANIStopped=0xFF;
	
	
		switch(MenuTop_Choice_Index)
		{
			case Menu_Trip:
			{
				//chpark.24.10.02
				/*
				ituWidgetSetVisible(M1_1a_BTN,false);
				ituWidgetSetVisible(M1_1b_BTN,false);
	
				ituWidgetSetVisible(M1_1a_ANI, true);
				ituWidgetSetVisible(M1_1b_ANI, true);
	
				// MT_1a,1b scale up
				MT_1a_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M1_1a_ANI, 0);
			
				MT_1b_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M1_1b_ANI, 0);
				*/
				
				// MT_1a,1b scale up
				MT_1a_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
				MT_1b_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
				ituWidgetSetVisible(M1_1a_BTN,false);
				ituWidgetSetVisible(M1_1b_BTN,false);

				ituWidgetSetVisible(M1_1a_ANI,false);
				ituWidgetSetVisible(M1_1b_ANI,false);

				ituWidgetSetVisible(M1_1a_ZoomBTN,true);
				ituWidgetSetVisible(M1_1b_ZoomBTN,true);
			}	
			break;
			
			case Menu_AUX:
			{
				//chpark.24.10.02
				/*
				ituWidgetSetVisible(M1_2_BTN,false);
				ituWidgetSetVisible(M1_2_ANI, true);
	
				// MT_2 scale up
				MT_2_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M1_2_ANI, 0);
				*/

				// MT_2 scale up
				MT_2_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
				ituWidgetSetVisible(M1_2_BTN, false);
				ituWidgetSetVisible(M1_2_ANI, false);
				ituWidgetSetVisible(M1_2_ZoomBTN, true);
			}
			break;
			
			case Menu_LCD:
			{
				/*
				ituWidgetSetVisible(M1_3_BTN,false);
				ituWidgetSetVisible(M1_3_ANI, true);
	
				// MT_3 scale up
				MT_3_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M1_3_ANI, 0);
				*/
				// MT_3 scale up
				MT_3_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
				ituWidgetSetVisible(M1_3_BTN,false);
				ituWidgetSetVisible(M1_3_ANI,false);
				ituWidgetSetVisible(M1_3_ZoomBTN,true);
			}
			break;
	
			case Menu_Data:
			{
				/*
				ituWidgetSetVisible(M1_4_BTN,false);
				ituWidgetSetVisible(M1_4_ANI, true);
	
				// MT_4 scale up
				MT_4_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M1_4_ANI, 0);
				*/
				// MT_4 scale up
				MT_4_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
				ituWidgetSetVisible(M1_4_BTN,false);
				ituWidgetSetVisible(M1_4_ANI,false);
				ituWidgetSetVisible(M1_4_ZoomBTN,true);
			}
			break;
	
			case Menu_Time:
			{
				/*
				ituWidgetSetVisible(M1_5_BTN,false);
				ituWidgetSetVisible(M1_5_ANI, true);
	
				// MT_5 scale up
				MT_5_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M1_5_ANI, 0);
				*/
				// MT_5 scale up
				MT_5_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
				ituWidgetSetVisible(M1_5_BTN,false);
				ituWidgetSetVisible(M1_5_ANI,false);
				ituWidgetSetVisible(M1_5_ZoomBTN,true);
			}
			break;
	
			case Menu_Preset:
			{
				/*
				ituWidgetSetVisible(M1_6_BTN,false);
				ituWidgetSetVisible(M1_6_ANI, true);
	
				// MT_6 scale up
				MT_6_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M1_6_ANI, 0);
				*/
				// MT_6 scale up
				MT_6_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
				ituWidgetSetVisible(M1_6_BTN,false);
				ituWidgetSetVisible(M1_6_ANI,false);
				ituWidgetSetVisible(M1_6_ZoomBTN,true);
			}
			break;
	
			case Menu_ErCode:
			{
				/*
				ituWidgetSetVisible(M1_7_BTN,false);
				ituWidgetSetVisible(M1_7_ANI, true);
	
				// MT_7 scale up
				MT_7_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M1_7_ANI, 0);
				*/
				// MT_7 scale up
				MT_7_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
				ituWidgetSetVisible(M1_7_BTN,false);
				ituWidgetSetVisible(M1_7_ANI,false);
				ituWidgetSetVisible(M1_7_ZoomBTN,true);
			}
			break;
	
			case Menu_UserDefine:
			{
				if(MT_8_OnOff_Flag == 0)			// V010
				{
				}
				else
				{
					/*
					ituWidgetSetVisible(M1_8_BTN,false);
					ituWidgetSetVisible(M1_8_ANI, true);
	
					// MT_8 scale up
					MT_8_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
					ituAnimationPlay(M1_8_ANI, 0);
					*/
					
					// MT_8 scale up
					MT_8_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
					ituWidgetSetVisible(M1_8_BTN,false);
					ituWidgetSetVisible(M1_8_ANI,false);
					ituWidgetSetVisible(M1_8_ZoomBTN,true);
				}
			}
			break;
		}
	
			
		MenuLayerFlag=1;
	}	
 
 	MT_8_OnOff_Flag = 0xFF;	// JPHong 2024-03-06
 	
	gMT_UserFlag = User_RegInfo[M7_UserIndex].RegFlag;

	//gMT_UserFlag = 5;		// dev.opt

	//if((gMT_UserFlag == 2)||(gMT_UserFlag == 3)||(gMT_UserFlag == 9))			// V010
	if((gMT_UserFlag == 5)&&(gMenu_UI_Time != 0) && g_nM7_StartType==M7_MODE_PUSH) //chpark.24.11.18
	{
		MT_8_Display(1);	// on   ������ menu 2 ��	JPHong 2024-03-20
	}
	else
	{
		MT_8_Display(0);	// off
	}

//	MT_ChoiceMenuClear(0xFF);  // 0xFF all 

	//MT_DeepCHK = 0;
	MT_DeepCHK_JOG = 0;	// JPHong 2023-07-08
	//gMT_Timer = gMainTimerCnt_10m;

	gMT_ExitTimer = gMainTimerCnt_10m;		// jhlee 231005
	gMT_DeepTimer = gMainTimerCnt_10m;		// jhlee 231005


	//=======================
	//chpark.24.04.05
	for(int i=0;i<MAX_SU100_USER_NUM;i++)
	{
		User_RegInfo[i].RegKeyNum = i;
	}

	Check_Null_M7_UserName();
	//=======================

	MT_UserDisplay();

	DOUBLE_KEY_ENABLE_FLAG = 0;	// JPHong 0727 #10956
	ReadyRotation();	// JPHong 2023-10-24
	ScreenSetOnOff(1);	// JPHong 2023-12-04
	
	
	printf("\n[MenuTop_Enter End] : %d %d %d\n",Main_Disp_Mode,MainBtFlag,Main_Disp_Mode_Flag);
	
    return true;
}


//chpark.24.04.05
void Check_Null_M7_UserName()
{
	/*
	// for test
	//
	M7_UserIndex = 0;

	for(int i=0;i<50;i++)
	{
		User_RegInfo[0].UserName[i] = NULL;
	}
	User_RegInfo[0].RegFlag = SU100_USER_SPAD;
	//
	*/

	//chpark.24.04.05
	int i = M7_UserIndex;

	if((gMenu_UI_Time > 0)&&(0<=M7_UserIndex)&&(M7_UserIndex<=MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		char chTmpUserName[50];
		memset(chTmpUserName, 0, sizeof(chTmpUserName));
		memcpy(chTmpUserName, &User_RegInfo[i].UserName[0], sizeof(chTmpUserName));
		int nSizeOfName = strlen(chTmpUserName);

		if(SU100_Is_Empty_InFlash(User_RegInfo[i].UserName[0]) || nSizeOfName==0)
		{
			char chTmpUserName[50];
			memset(chTmpUserName, 0, sizeof(chTmpUserName));
			memset (&User_RegInfo[i].UserName[0],0,sizeof(User_RegInfo[i].UserName));

			//if(i==0 && User_RegInfo[i].RegFlag==SU100_USER_SPAD)//chpark.24.04.11
			if(i==0)//chpark.24.04.11
			{
				strncpy(&User_RegInfo[i].UserName[0],"Super+ Admin",MAX_SU100_NAME_CNT);
			}
			//else if(i==1 && User_RegInfo[i].RegFlag==SU100_USER_SAD)
			else if(i==1)
			{
				strncpy(&User_RegInfo[i].UserName[0],"Super Admin",MAX_SU100_NAME_CNT);
			}
			else 
			{
				sprintf(chTmpUserName, "default user%d",User_RegInfo[i].RegKeyNum-1);//user idx:2->default user1//chpark.12.19
				strncpy(&User_RegInfo[i].UserName[0],chTmpUserName,MAX_SU100_NAME_CNT);
			}
			
			if(User_RegInfo[i].AccessAUX == SU600_AUX_TYPE_NONE)//chpark.24.03.27
			{
				User_RegInfo[i].AccessAUX 		= SU600_AUX_TYPE_NO;
				User_RegInfo[i].nTmp_AccessAUX	= SU600_AUX_TYPE_NO;
			}

			printf("[Check_Null_M7_UserName](1)M7_UserIndex=%d nSizeOfName=%d chTmpUserName=%s\n",M7_UserIndex,nSizeOfName,chTmpUserName);
			printf("[Check_Null_M7_UserName](2)%d user name = %s\n",i,&User_RegInfo[i].UserName[0]);
		}
		else
		{
			printf("[Check_Null_M7_UserName](4)M7_UserIndex=%d nSizeOfName=%d\n",M7_UserIndexEx,nSizeOfName);
		}

		printf("[Check_Null_M7_UserName](5)%d user name = %s\n",i,&User_RegInfo[i].UserName[0]);
	}
	else
	{

		printf("[Check_Null_M7_UserName][Error]%d user name = %s\n",i,&User_RegInfo[i].UserName[0]);
		
	}
}

bool MenuTop_Timer(ITUWidget* widget, char* param)
{

	static uint32_t OnTimerCnt_33ms = 0;	// JPHong 2024-03-06
	static uint32_t OnTimerCnt_100ms = 0;
	static uint32_t OnTimerCnt_300ms = 0;
	static uint32_t OnTimerCnt_500ms = 0;
	static uint32_t OnTimerCnt_600ms = 0;
	static uint32_t OnTimerCnt_1S = 0;
	
	OnTimerCnt_33ms++;
	
#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		//printf("[MenuTop_Timer][Timer]return============================RotationMode:%d\n",RotationMode);
		return 0;
	}
#endif
		//printf("[MenuTop_Timer][Timer]RotationMode:%d\n",RotationMode);

// jhlee 231005 for 30sec exit timer
	if((gMainTimerCnt_10m - gMT_ExitTimer) >= 3000)
	{
		printf("\r\n Menu Exit > 30 sec\r\n");
		
		MTD6_MouseUp("MenuTop_Timer",NULL);	//JPHong 2024-03-13
		//GotoMain(MAIN_MODE_NORMAL);
	}

	if(((MT_D5_Flag==1)&&(MT_D7_Flag==1))||(MT_DeepCHK_JOG==1))
	{
		if((gMainTimerCnt_10m - gMT_DeepTimer) >= 500)
		{
			printf("\r\n Menu Exit > goto DeepMEnu\r\n");
			GotoBackScreenT(false);	// JPHong 2023-05-18
		}
	}
//=========================================

	// CAN[UI1] polling
	gMT_UserFlag = User_RegInfo[M7_UserIndex].RegFlag;

	//gMT_UserFlag = 5;		// dev.opt master fix   JPHong
#if 1
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_100MS)==0)	// 100ms
	{
		OnTimerCnt_100ms++;
		if((gMT_UserFlag == 5)&&(gMenu_UI_Time != 0) && g_nM7_StartType==M7_MODE_PUSH) //chpark.24.11.18
		{
			MT_8_Display(1);	// on   ������ menu 2 ��	JPHong 2024-03-20
		}
		else
		{
			MT_8_Display(0);	// off
		}
		MT_UserDisplay();
	}
#endif	
//	MT_AniMenuStatus();
	
    return true;
}

bool MenuTop_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t JogDir = 0;

	//gMT_Timer = gMainTimerCnt_10m;
	//gMT_ExitTimer = 0;

	gMT_ExitTimer = gMainTimerCnt_10m;		// jhlee 231005

	JogDir = param[0];

	printf("\r\nMenuTop JogWheel %d\r\n",JogDir);

	//if((JogDir == JOG_DIAL_ROT_RIGHT) && (JW_1TimeFlag == 1))
	if(JogDir == JOG_DIAL_ROT_RIGHT)
	{
		MT_ChoiceMenuClear(MenuTop_Choice_Index);
		MenuTop_Choice_Index++;
		if(MenuTop_Choice_Index > Menu_UserDefine)
		{
			MenuTop_Choice_Index = Menu_Trip;
		}
		if((MT_8_OnOff_Flag == 0)&&(MenuTop_Choice_Index == Menu_UserDefine))			// V010
		{
			MenuTop_Choice_Index = Menu_Trip;
		}


		memset(PARA,0,2);
		itoa(MenuTop_Choice_Index,PARA,10);
		
		printf("wheel CW Branch Menu : %s\r\n",PARA);
		
		param[0] = 0;
		tJogTimer = gMainTimerCnt_10m;
		//JW_1TimeFlag = 0;
		
		M1_BTN_MouseDn(NULL, PARA);
	}
	//else if((JogDir == JOG_DIAL_ROT_LEFT) && (JW_1TimeFlag == 1))
	else if(JogDir == JOG_DIAL_ROT_LEFT)
	{
		MT_ChoiceMenuClear(MenuTop_Choice_Index);
		if(MenuTop_Choice_Index > Menu_Trip)
			MenuTop_Choice_Index--;
		else if(MenuTop_Choice_Index == Menu_Trip)
		{
			MenuTop_Choice_Index = Menu_UserDefine;
		}

		if((MT_8_OnOff_Flag == 0)&&(MenuTop_Choice_Index == Menu_UserDefine))		// operater
		{
			MenuTop_Choice_Index = Menu_ErCode;
		}
		
		memset(PARA,0,2);
		itoa(MenuTop_Choice_Index,PARA,10);
		
		printf("wheel ccw Branch Menu : %s\r\n",PARA);
		
		param[0] = 0;
		tJogTimer = gMainTimerCnt_10m;
		//JW_1TimeFlag = 0;
		
		M1_BTN_MouseDn(NULL, PARA);
	}

    return true;
}

bool MenuTop_JogButton(ITUWidget* widget, char* param)
{
	uint8_t JogButton = 0;
	uint8_t tMenuIndex = 0;

	//gMT_Timer = gMainTimerCnt_10m;	
	//gMT_ExitTimer = 0;

	gMT_ExitTimer = gMainTimerCnt_10m;		// jhlee 231005

	JogButton = param[0];

	printf("MenuTop JogButton %d\r\n",JogButton);

	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
		{
			switch(JogButton)
			{
				case JOG_DIAL_KEY_UP:
				{
					// 1>6,2>1,3>2,4>3,5>4,6>5,7>6,8>7
					//uint8_t tDest1[10] = {0,6,1,2,3,4,5,6,7,0};
					// 1>6,2>1,3>2,4>3,5>4,6>5,7>8,8>7 //chpark.24.04.22
					uint8_t tDest1[10] = {0,6,1,2,3,4,5,8,7,0};
					
					tMenuIndex = tDest1[MenuTop_Choice_Index];

					if((MT_8_OnOff_Flag == 0)&&(tMenuIndex == 8))
					{		// operater//chpark.24.04.22
						tMenuIndex = 7; //chpark.24.04.22
						return false;//chpark.24.04.22
					}
					memset(PARA,0,2);
					itoa(tMenuIndex,PARA,10);
					printf("button up Branch Menu : %s\r\n",PARA);
					param[0] = 0;
					M1_BTN_MouseDn(NULL, PARA);
				}
				break;

				case JOG_DIAL_KEY_DOWN:
				{
					// 1>2,2>3,3>4,4>5,5>6,6>1,7>8,8>1
					//uint8_t tDest2[10] = {0,2,3,4,5,6,1,8,1,0};
					// 1>2,2>3,3>4,4>5,5>6,6>1,7>8,8>7
					uint8_t tDest2[10] = {0,2,3,4,5,6,1,8,7,0};//chpark.24.04.22
					
					tMenuIndex = tDest2[MenuTop_Choice_Index];

					if((MT_8_OnOff_Flag == 0)&&(tMenuIndex == 8))
					{		// operater
						tMenuIndex = 7; //chpark.24.04.22
						return false;
					}
					memset(PARA,0,2);
					itoa(tMenuIndex,PARA,10);
					printf("button down Branch Menu : %s\r\n",PARA);
					param[0] = 0;
					M1_BTN_MouseDn(NULL, PARA);
				}
				break;
				
				case JOG_DIAL_KEY_LEFT:
				case JOG_DIAL_KEY_RIGHT:
				{
					//1>7,2>8,3>8,4>8,5>8,6>8,7>1,8>2
					uint8_t tDest3[10] = {0,7,8,8,8,8,8,1,2,0}; 

					tMenuIndex = tDest3[MenuTop_Choice_Index];
	
					if((MT_8_OnOff_Flag == 0)&&(tMenuIndex == 8))		// operater
						tMenuIndex = 7;
	
					memset(PARA,0,2);
					itoa(tMenuIndex,PARA,10);
					printf("button Left/Right Branch Menu : %s\r\n",PARA);
					param[0] = 0;
					M1_BTN_MouseDn(NULL, PARA);
				}
				break;

				case JOG_DIAL_KEY_ENTER_OFF:
				{
					memset(PARA,0,2);
					itoa(MenuTop_Choice_Index,PARA,10);
					M1_BTN_MouseUp(NULL, PARA);
				}
				break;
				case JOG_DIAL_KEY_DEEP:
					printf("JOG DEEP MENU\r\n");
					MT_DeepCHK_JOG=1;	//JPHong 2023-07-08
					gMT_DeepTimer = gMainTimerCnt_10m;		// jhlee 231005 deeptimer init
				break;
				
				case JOG_DIAL_KEY_NONE:
					printf("JOG BUTTON OFF\r\n");
					MT_DeepCHK_JOG=0;	//JPHong 2023-07-08
				break;				
			}
		}
		break;
			
		case LANDSCAPE_1:
		case LANDSCAPE_2:
		{
			switch(JogButton)
			{
				case JOG_DIAL_KEY_UP:
				{
					// 1>3,2>1,3>2,4>6,5>4,6>5,7>8,8>7
					uint8_t tDest4[10] = {0,3,1,2,6,4,5,8,7,0}; 
					
					tMenuIndex = tDest4[MenuTop_Choice_Index];
				
					if((MT_8_OnOff_Flag == 0)&&(tMenuIndex == 8))
					{		// operater
						tMenuIndex = 7;
						return false;
					}	
				
					memset(PARA,0,2);
					itoa(tMenuIndex,PARA,10);
					printf("button up Branch Menu : %s\r\n",PARA);
					param[0] = 0;
					M1_BTN_MouseDn(NULL, PARA);
				}
				break;
				
				case JOG_DIAL_KEY_DOWN:
				{
					// 1>2,2>3,3>1,4>5,5>6,6>4,7>8,8>7
					uint8_t tDest5[10] = {0,2,3,1,5,6,4,8,7,0};
					
					tMenuIndex = tDest5[MenuTop_Choice_Index];

					if((MT_8_OnOff_Flag == 0)&&(tMenuIndex == 8))		// operater
					{
						tMenuIndex = 7;
						return false;
					}
				
					memset(PARA,0,2);
					itoa(tMenuIndex,PARA,10);
					printf("button down Branch Menu : %s\r\n",PARA);
					param[0] = 0;
					M1_BTN_MouseDn(NULL, PARA);
				}
				break;

				case JOG_DIAL_KEY_LEFT:
				{
					// 1>7,2>8,3>6,4>1,5>2,6>3,7>4,8>5
					uint8_t tDest6[10] = {0,7,8,6,1,2,3,4,5,0};

					tMenuIndex = tDest6[MenuTop_Choice_Index];

					if((MT_8_OnOff_Flag == 0)&&(tMenuIndex == 8))		// operater
						tMenuIndex = 5;
				
					memset(PARA,0,2);
					itoa(tMenuIndex,PARA,10);
					printf("button left Branch Menu : %s\r\n",PARA);
					param[0] = 0;
					M1_BTN_MouseDn(NULL, PARA);
				}
				break;

				case JOG_DIAL_KEY_RIGHT:
				{
					// 1>4,2>5,3>6,4>7,5>8,6>3,7>1,8>2
					uint8_t tDest7[10] = {0,4,5,6,7,8,3,1,2,0};

					tMenuIndex = tDest7[MenuTop_Choice_Index];

					if((MT_8_OnOff_Flag == 0)&&(tMenuIndex == 8))		// operater
						tMenuIndex = 2;
				
					memset(PARA,0,2);
					itoa(tMenuIndex,PARA,10);
					printf("button right Branch Menu : %s\r\n",PARA);
					param[0] = 0;
					M1_BTN_MouseDn(NULL, PARA);
				}
				break;

				case JOG_DIAL_KEY_ENTER_OFF:
				{
					memset(PARA,0,2);
					itoa(MenuTop_Choice_Index,PARA,10);
					M1_BTN_MouseUp(NULL, PARA);
				}
				break;
				case JOG_DIAL_KEY_DEEP:
					printf("JOG DEEP MENU\r\n");
					MT_DeepCHK_JOG=1;	//JPHong 2023-07-08
					gMT_DeepTimer = gMainTimerCnt_10m;		// jhlee 231005 deeptimer init
				break;
				
				case JOG_DIAL_KEY_NONE:
					printf("JOG BUTTON OFF\r\n");
					MT_DeepCHK_JOG=0;	//JPHong 2023-07-08
				break;				
			}
		}
		break;
	}
	
    return true;
}

bool MenuTop_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t JogSwitch = 0;

	JogSwitch = param[0];

	//gMT_Timer = gMainTimerCnt_10m;
	//gMT_ExitTimer = 0;

	gMT_ExitTimer = gMainTimerCnt_10m;

	if(JogSwitch == JOG_DIAL_SW_ESC_ON)
	{
		//Menu_Index = Menu_Top;
		//MTD6_MouseUp(NULL,NULL);	//JPHong 2024-03-22
		//GotoMain(MAIN_MODE_NORMAL);
	}
	else if(JogSwitch == JOG_DIAL_SW_HOME)
	{
		MTD6_MouseUp("JOG_DIAL_SW_HOME",NULL);	//JPHong 2024-03-13
		//GotoMain(MAIN_MODE_NORMAL);
	}
	else if(JogSwitch == JOG_DIAL_KEY_DEEP)
	{
		printf("JOG DEEP MENU\r\n");
		MT_DeepCHK_JOG=1;	//JPHong 2023-07-08
		gMT_DeepTimer = gMainTimerCnt_10m;		// jhlee 231005 
	}
	else if(JogSwitch == JOG_DIAL_SW_HOME)
	{
		printf("JOG_DIAL_SW_HOME\r\n");
		MT_DeepCHK_JOG=0;	//JPHong 2023-07-08
		//gMT_DeepTimer = 0;
	}
	else if(JogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
		printf("JOG_DIAL_SW_ESC_ON_OFF\r\n");
		MTD6_MouseUp("JOG_DIAL_SW_ESC_ON_OFF",NULL);	//JPHong 2024-03-22
		MT_DeepCHK_JOG=0;			//JPHong 2023-07-08
		//gMT_DeepTimer = 0;
	}
    return true;
}

bool MenuTop_Leave(ITUWidget* widget, char* param)
{
	DOUBLE_KEY_ENABLE_FLAG = 1;	// JPHong 0727 #10956

	XO_ituButtonSetMode_Bt(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
	XO_ituButtonSetMode_TP(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28	
	
    return true;
}

bool MT_DeepCheck(ITUWidget* widget, char* param)
{
	uint8_t tBTNChk = 0;
	
	tBTNChk = (uint8_t)atoi(param);

	if(tBTNChk == 5)
	{
		XO_ituButtonSetMode_Bt(0);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
		XO_ituButtonSetMode_TP(0);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
		MT_D5_Flag = 1;
	}
	else if(tBTNChk == 7)
	{
		XO_ituButtonSetMode_Bt(0);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
		XO_ituButtonSetMode_TP(0);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
		MT_D7_Flag = 1;
	}
	
	//MT_DeepCHK += tBTNChk;
	//printf("D5&D7 Press Check = %d %d %d\r\n",MT_DeepCHK,MT_D5_Flag,MT_D7_Flag);

	//if(MT_DeepCHK == 12)	// JPHong 07-17
	if ((MT_D5_Flag==1)&&(MT_D7_Flag==1))
	{
		printf("D5&D7 Press 5Sec Count Start\r\n");
		gMT_DeepTimer = gMainTimerCnt_10m;		// jhlee 231005
	}

	//gMT_Timer = gMainTimerCnt_10m;
	//gMT_ExitTimer = 0;
	
	gMT_ExitTimer = gMainTimerCnt_10m;		// jhlee 231005
	
    return true;
}

bool MTD7_MouseUp(ITUWidget* widget, char* param)
{
	XO_ituButtonSetMode_Bt(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
	XO_ituButtonSetMode_TP(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
	if((MT_D7_Flag == 1)&&(MT_D5_Flag == 0))		// jhlee 231005
	{
		printf("Choice Menu Branch\r\n");

		memset(PARA,0,2);
		itoa(MenuTop_Choice_Index,PARA,10);
		M1_BTN_MouseUp(NULL, PARA);
	}

	MT_D7_Flag = 0;
	//MT_DeepCHK = 0;		// jhlee 231005

    return true;
}
bool MTD7_LongPress(ITUWidget* widget, char* param)
{
	printf("MTD7_LongPress\r\n");
    return true;
}

bool MTD6_MouseUp(ITUWidget* widget, char* param)
{
	XO_ituButtonSetMode_Bt(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
	XO_ituButtonSetMode_TP(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
	MT_D5_Flag = 0;
	MT_D7_Flag = 0;
	//MT_DeepCHK = 0;		// jhlee 231005
	printf("MTD6_MouseUp[%s][%d]\r\n",widget,param[0]);
	#if 0 //def USED_ROTATION	//TEST_ROTATION
	{
		if (RotationMode==0)
		{
			RotationModeTime = gMainTimerCnt_10m;
			RotationMode = 1;
		}
	}
	#else
		GotoMain(MAIN_MODE_NORMAL); 
	#endif		
	
	
    return true;
}

bool MTD5_MouseUp(ITUWidget* widget, char* param)
{
	//GotoMainMenu();
	if((MT_D5_Flag == 1)&&(MT_D7_Flag == 0))
	{
		//		GotoMain(MAIN_MODE_NORMAL); 
		MTD6_MouseUp("MTD5_MouseUp",NULL);	//JPHong 2024-03-13
	}	
	
	MT_D5_Flag = 0;
	MT_D7_Flag = 0;
	//MT_DeepCHK = 0;
	
	return true;
}

bool MTD5_LongPress(ITUWidget* widget, char* param)
{
	printf("MTD5_LongPress\r\n");
    return true;
}
bool M1_BTN_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tMT_ChoiceMenu;

	ITULayer *tBranchLayer;
	
	tMT_ChoiceMenu = (uint8_t)atoi(param);
	
	printf("M1_BTN_MouseUp[%d]\r\n",tMT_ChoiceMenu);
	
	MT_D5_Flag = 0;
	MT_D7_Flag = 0;
	//MT_DeepCHK = 0;		// jhlee 231005

	if(tMT_ChoiceMenu == MenuTop_Choice_Index)
	{
		switch(MenuTop_Choice_Index)
		{
			case Menu_Trip:
    		Menu_Index = MenuTop_Choice_Index;

				tBranchLayer = ituSceneFindWidget(&theScene, "Menu2_Layer");
				assert(tBranchLayer);
				ituLayerGoto(tBranchLayer);

			break;

			case Menu_AUX:
    		Menu_Index = MenuTop_Choice_Index;

				tBranchLayer = ituSceneFindWidget(&theScene, "Menu3_Layer");
				assert(tBranchLayer);
				ituLayerGoto(tBranchLayer);

			break;

			case Menu_LCD:
    		Menu_Index = MenuTop_Choice_Index;

				tBranchLayer = ituSceneFindWidget(&theScene, "Menu4_Layer");
				assert(tBranchLayer);
				ituLayerGoto(tBranchLayer);

			break;

			case Menu_Data:
    		Menu_Index = MenuTop_Choice_Index;

				tBranchLayer = ituSceneFindWidget(&theScene, "Menu5_Layer");
				assert(tBranchLayer);
				ituLayerGoto(tBranchLayer);

			break;

			case Menu_Time:
    		Menu_Index = MenuTop_Choice_Index;

				tBranchLayer = ituSceneFindWidget(&theScene, "Menu6_Layer");
				assert(tBranchLayer);
				ituLayerGoto(tBranchLayer);

			break;

			case Menu_Preset:
    		Menu_Index = MenuTop_Choice_Index;

				tBranchLayer = ituSceneFindWidget(&theScene, "Menu7_Layer");
				assert(tBranchLayer);
				ituLayerGoto(tBranchLayer);

			break;

			case Menu_ErCode:
    		Menu_Index = MenuTop_Choice_Index;

				tBranchLayer = ituSceneFindWidget(&theScene, "Menu8_Layer");
				assert(tBranchLayer);
				ituLayerGoto(tBranchLayer);

			break;

			case Menu_UserDefine:
    			Menu_Index = MenuTop_Choice_Index;
				if (MT_8_Scaleflag == 1)
				{
					MT_BTN_MouseUpFlag = 1;
				}
				else
				{
					MT_BTN_MouseUpFlag = 0;
					tBranchLayer = ituSceneFindWidget(&theScene, "Menu9_Layer");
					assert(tBranchLayer);
					ituLayerGoto(tBranchLayer);
				}
			break;
		}
	}
	
    return true;
}

bool M1_BTN_MouseDn(ITUWidget* widget, char* param)
{
	MT_ChoiceMenuClear(MenuTop_Choice_Index);
 	MenuTop_Choice_Index = (uint8_t)atoi(param);

	//gMT_Timer = gMainTimerCnt_10m;		// jhlee 231005

	//ANI_TImer = gMainTimerCnt_10m;
	
//	MT_ChoiceMenuClear();

	switch(MenuTop_Choice_Index)
	{
		ituWidgetSetVisible(M1_1a_ZoomBTN, false); //chpark.24.04.22
		ituWidgetSetVisible(M1_1b_ZoomBTN, false); //chpark.24.04.22

		ituWidgetSetVisible(M1_2_ZoomBTN, false); //chpark.24.04.22
		ituWidgetSetVisible(M1_3_ZoomBTN, false); //chpark.24.04.22
		ituWidgetSetVisible(M1_4_ZoomBTN, false); //chpark.24.04.22
		ituWidgetSetVisible(M1_5_ZoomBTN, false); //chpark.24.04.22
		ituWidgetSetVisible(M1_6_ZoomBTN, false); //chpark.24.04.22
		ituWidgetSetVisible(M1_7_ZoomBTN, false); //chpark.24.04.22
		ituWidgetSetVisible(M1_8_ZoomBTN, false); //chpark.24.04.22

		/*
		ituWidgetSetVisible(M1_1a_BTN, true); //chpark.24.04.22
		ituWidgetSetVisible(M1_1b_BTN, true); //chpark.24.04.22

		ituWidgetSetVisible(M1_2_BTN, true);//chpark.24.04.22
		ituWidgetSetVisible(M1_3_BTN, true);//chpark.24.04.22
		ituWidgetSetVisible(M1_4_BTN, true);//chpark.24.04.22
		ituWidgetSetVisible(M1_5_BTN, true);//chpark.24.04.22
		ituWidgetSetVisible(M1_6_BTN, true);//chpark.24.04.22
		ituWidgetSetVisible(M1_7_BTN, true);//chpark.24.04.22
		ituWidgetSetVisible(M1_8_BTN, true);//chpark.24.04.22
		*/

		case Menu_Trip:
		{
			MT_AniMenuStatus();

			ituWidgetSetVisible(M1_1a_BTN,false);
			ituWidgetSetVisible(M1_1b_BTN,false);

			ituWidgetSetVisible(M1_1a_ANI, true);
			ituWidgetSetVisible(M1_1b_ANI, true);

			// MT_1a,1b scale up
			MT_1a_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M1_1a_ANI, 0);
		
			MT_1b_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M1_1b_ANI, 0);
		}	
		break;
		
		case Menu_AUX:
		{
			MT_AniMenuStatus();

			ituWidgetSetVisible(M1_2_BTN,false);
			ituWidgetSetVisible(M1_2_ANI, true);

			// MT_2 scale up
			MT_2_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M1_2_ANI, 0);
		}
		break;
		
		case Menu_LCD:
		{
			MT_AniMenuStatus();

			ituWidgetSetVisible(M1_3_BTN,false);
			ituWidgetSetVisible(M1_3_ANI, true);

			// MT_3 scale up
			MT_3_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M1_3_ANI, 0);
		}
		break;

		case Menu_Data:
		{
			MT_AniMenuStatus();

			ituWidgetSetVisible(M1_4_BTN,false);
			ituWidgetSetVisible(M1_4_ANI, true);

			// MT_4 scale up
			MT_4_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M1_4_ANI, 0);
		}
		break;

		case Menu_Time:
		{
			MT_AniMenuStatus();

			ituWidgetSetVisible(M1_5_BTN,false);
			ituWidgetSetVisible(M1_5_ANI, true);

			// MT_5 scale up
			MT_5_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M1_5_ANI, 0);
		}
		break;

		case Menu_Preset:
		{
			MT_AniMenuStatus();

			ituWidgetSetVisible(M1_6_BTN,false);
			ituWidgetSetVisible(M1_6_ANI, true);

			// MT_6 scale up
			MT_6_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M1_6_ANI, 0);
		}
		break;

		case Menu_ErCode:
		{


			ituWidgetSetVisible(M1_7_BTN,false);
			ituWidgetSetVisible(M1_7_ANI, true);

			// MT_7 scale up
			MT_7_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M1_7_ANI, 0);
		}
		break;

		case Menu_UserDefine:
		{
			MT_AniMenuStatus();

			if(MT_8_OnOff_Flag)			// Not Op user
			{
				ituWidgetSetVisible(M1_8_BTN,false);
				ituWidgetSetVisible(M1_8_ANI, true);

				// MT_8 scale up
				MT_8_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M1_8_ANI, 0);
			}
			else	//  if(MT_8_OnOff_Flag == 0)		// operater user
			{
				MenuTop_Choice_Index = Menu_Trip;
				
				ituWidgetSetVisible(M1_1a_BTN,false);
				ituWidgetSetVisible(M1_1b_BTN,false);

				ituWidgetSetVisible(M1_1a_ANI, true);
				ituWidgetSetVisible(M1_1b_ANI, true);

				// MT_1a,1b scale up
				MT_1a_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M1_1a_ANI, 0);
		
				MT_1b_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M1_1b_ANI, 0);
			}

		}
		break;
	}
	
    return true;
}

bool M1_8_ANIStopped(ITUWidget* widget, char* param)
{
	// if (tM1_8_ANIStopped==MT_8_Scaleflag) // 2024-05-07 modi whlim, jog error modify
	// {
	// 	return true;
	// }
	printf("M1_8_ANIStopped=[%d][%d][%d]\n",tM1_8_ANIStopped,MT_8_Scaleflag,MT_BTN_MouseUpFlag);
	tM1_8_ANIStopped=MT_8_Scaleflag;


	if(MT_8_Scaleflag == 1)
	{
		MT_8_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_8_ANI, false);
		ituWidgetSetVisible(M1_8_ZoomBTN, true);

 		//printf("Icon S8 UP Time = %d\r\n",(gMainTimerCnt_10m - ANI_TImer));
	}
	else if(MT_8_Scaleflag == 2)
	{
		MT_8_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_8_ANI, false);
		ituWidgetSetVisible(M1_8_BTN, true);
	}
	if (MT_BTN_MouseUpFlag)
	{
		ITULayer *tBranchLayer;
		MT_BTN_MouseUpFlag = 0;
		tBranchLayer = ituSceneFindWidget(&theScene, "Menu9_Layer");
		assert(tBranchLayer);
		ituLayerGoto(tBranchLayer);
	}
    return true;
}

bool M1_7_ANIStopped(ITUWidget* widget, char* param)
{
	// if (tM1_7_ANIStopped==MT_7_Scaleflag) // 2024-05-07 modi whlim, jog error modify
	// {
	// 	return true;
	// }
	printf("M1_7_ANIStopped=[%d][%d]\n",tM1_7_ANIStopped,MT_7_Scaleflag);
	tM1_7_ANIStopped=MT_7_Scaleflag;


	if(MT_7_Scaleflag == 1)
	{
		MT_7_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_7_ANI, false);
		ituWidgetSetVisible(M1_7_ZoomBTN, true);

 		//printf("Icon S7 UP Time = %d\r\n",(gMainTimerCnt_10m - ANI_TImer));
	}
	else if(MT_7_Scaleflag == 2)
	{
		MT_7_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_7_ANI, false);
		ituWidgetSetVisible(M1_7_BTN, true);
	}

    return true;
}

bool M1_6_ANIStopped(ITUWidget* widget, char* param)
{
	// if (tM1_6_ANIStopped==MT_6_Scaleflag) // 2024-05-07 modi whlim, jog error modify
	// {
	// 	return true;
	// }
	printf("M1_6_ANIStopped=[%d][%d]\n",tM1_6_ANIStopped,MT_6_Scaleflag);
	tM1_6_ANIStopped=MT_6_Scaleflag;


	if(MT_6_Scaleflag == 1)
	{
		MT_6_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_6_ANI, false);
		ituWidgetSetVisible(M1_6_ZoomBTN, true);

 		//printf("Icon S6 UP Time = %d\r\n",(gMainTimerCnt_10m - ANI_TImer));
	}
	else if(MT_6_Scaleflag == 2)
	{
		MT_6_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_6_ANI, false);
		ituWidgetSetVisible(M1_6_BTN, true);
	}

    return true;
}

bool M1_5_ANIStopped(ITUWidget* widget, char* param)
{
	// if (tM1_5_ANIStopped==MT_5_Scaleflag) // 2024-05-07 modi whlim, jog error modify
	// {
	// 	return true;
	// }
	printf("M1_5_ANIStopped=[%d][%d]\n",tM1_5_ANIStopped,MT_5_Scaleflag);
	tM1_5_ANIStopped=MT_5_Scaleflag;


	if(MT_5_Scaleflag == 1)
	{
		MT_5_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_5_ANI, false);
		ituWidgetSetVisible(M1_5_ZoomBTN, true);

 		//printf("Icon S5 UP Time = %d\r\n",(gMainTimerCnt_10m - ANI_TImer));
	}
	else if(MT_5_Scaleflag == 2)
	{
		MT_5_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_5_ANI, false);
		ituWidgetSetVisible(M1_5_BTN, true);
	}

    return true;
}

bool M1_4_ANIStopped(ITUWidget* widget, char* param)
{
	// if (tM1_4_ANIStopped==MT_4_Scaleflag) // 2024-05-07 modi whlim, jog error modify
	// {
	// 	return true;
	// }
	printf("M1_4_ANIStopped=[%d][%d]\n",tM1_4_ANIStopped,MT_4_Scaleflag);
	tM1_4_ANIStopped=MT_4_Scaleflag;


	if(MT_4_Scaleflag == 1)
	{
		MT_4_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_4_ANI, false);
		ituWidgetSetVisible(M1_4_ZoomBTN, true);

 		//printf("Icon S4 UP Time = %d\r\n",(gMainTimerCnt_10m - ANI_TImer));
	}
	else if(MT_4_Scaleflag == 2)
	{
		MT_4_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_4_ANI, false);
		ituWidgetSetVisible(M1_4_BTN, true);
	}

    return true;
}

bool M1_3_ANIStopped(ITUWidget* widget, char* param)
{
	// if (tM1_3_ANIStopped==MT_3_Scaleflag) // 2024-05-07 modi whlim, jog error modify
	// {
	// 	return true;
	// }
	printf("M1_3_ANIStopped=[%d][%d]\n",tM1_3_ANIStopped,MT_3_Scaleflag);
	tM1_3_ANIStopped=MT_3_Scaleflag;


	if(MT_3_Scaleflag == 1)
	{
		MT_3_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_3_ANI, false);
		ituWidgetSetVisible(M1_3_ZoomBTN, true);

 		//printf("Icon S3 UP Time = %d\r\n",(gMainTimerCnt_10m - ANI_TImer));
	}
	else if(MT_3_Scaleflag == 2)
	{
		MT_3_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_3_ANI, false);
		ituWidgetSetVisible(M1_3_BTN, true);
	}

    return true;
}

bool M1_2_ANIStopped(ITUWidget* widget, char* param)
{
	// if (tM1_2_ANIStopped==MT_2_Scaleflag) // 2024-05-07 modi whlim, jog error modify
	// {
	// 	return true;
	// }
	printf("M1_2_ANIStopped=[%d][%d]\n",tM1_2_ANIStopped,MT_2_Scaleflag);
	tM1_2_ANIStopped=MT_2_Scaleflag;


	if(MT_2_Scaleflag == 1)
	{
		MT_2_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_2_ANI, false);
		ituWidgetSetVisible(M1_2_ZoomBTN, true);

 		//printf("Icon S2 UP Time = %d\r\n",(gMainTimerCnt_10m - ANI_TImer));
	}
	else if(MT_2_Scaleflag == 2)
	{
		MT_2_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_2_ANI, false);
		ituWidgetSetVisible(M1_2_BTN, true);
	}

    return true;
}

bool M1_1b_ANIStopped(ITUWidget* widget, char* param)
{
	// if (tM1_1b_ANIStopped==MT_1b_Scaleflag) // 2024-05-07 modi whlim, jog error modify
	// {
	// 	return true;
	// }
	printf("M1_1b_ANIStopped=[%d][%d]\n",tM1_1b_ANIStopped,MT_1b_Scaleflag);
	tM1_1b_ANIStopped=MT_1b_Scaleflag;

	if(MT_1b_Scaleflag == 1)
	{
		MT_1b_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_1b_ANI, false);
		ituWidgetSetVisible(M1_1b_ZoomBTN, true);

 		//printf("Icon S1 UP Time = %d\r\n",(gMainTimerCnt_10m - ANI_TImer));
	}
	else if(MT_1b_Scaleflag == 2)
	{
		MT_1b_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_1b_ANI, false);
		ituWidgetSetVisible(M1_1b_BTN, true);
	}

    return true;
}

bool M1_1a_ANIStopped(ITUWidget* widget, char* param)
{
	// if (tM1_1a_ANIStopped==MT_1a_Scaleflag) // 2024-05-07 modi whlim, jog error modify
	// {
	// 	return true;
	// }
	printf("M1_1a_ANIStopped=[%d][%d]\n",tM1_1a_ANIStopped,MT_1a_Scaleflag);
	tM1_1a_ANIStopped=MT_1a_Scaleflag;


	if(MT_1a_Scaleflag == 1)
	{
		MT_1a_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_1a_ANI, false);
		ituWidgetSetVisible(M1_1a_ZoomBTN, true);
	}
	else if(MT_1a_Scaleflag == 2)
	{
		MT_1a_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M1_1a_ANI, false);
		ituWidgetSetVisible(M1_1a_BTN, true);
	}
    return true;
}

void GotoMenuTop(void)
{

//	if(CameraOnLeaveCallSrc(MAIN_MODE_MENU)==1)
	{
	    ITULayer *MenuTop_Layer = ituSceneFindWidget(&theScene, "MenuTop_Layer");
	    assert(MenuTop_Layer);
	    Main_Disp_Mode = MAIN_MODE_MENU;
	    ituLayerGoto(MenuTop_Layer);
	}
}

void GotoDeepMenu(void)
{
    ITULayer *DeepMenu_Layer = ituSceneFindWidget(&theScene, "DeepMenu_Layer");
    assert(DeepMenu_Layer);
    ituLayerGoto(DeepMenu_Layer);
}

bool MT_UserDisplay(void)
{
	uint8_t tDestName[100];
	
	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
	{
		//printf("[MT_UserDisplay]gMenu_UI_Time==0 return:\n");
		return 0;
	}
		
	ITUWidget *MT_TopAdmin = ituSceneFindWidget(&theScene, "MT_TopAdmin");
	assert(MT_TopAdmin);
	ITUWidget *MT_TopStandard = ituSceneFindWidget(&theScene, "MT_TopStandard");
	assert(MT_TopStandard);
	
	ITUText *MT_TopUserName = ituSceneFindWidget(&theScene, "MT_TopUserName");
	assert(MT_TopUserName);
	
	if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
	{
		ituWidgetSetVisible(MT_TopStandard,false);
		ituWidgetSetVisible(MT_TopAdmin,true);
	}
	else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
	{
		ituWidgetSetVisible(MT_TopAdmin,false);
		ituWidgetSetVisible(MT_TopStandard,true);
	}
	else
	{
		ituWidgetSetVisible(MT_TopAdmin,false);
		ituWidgetSetVisible(MT_TopStandard,false);
	}

	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		//Check_Null_M7_UserName();//chpark.24.04.05

		memset(tDestName,0,100);

		//printf("[MT_UserDisplay]%d user name = %s\n",M7_UserIndex,&User_RegInfo[M7_UserIndex].UserName[0]);

		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(MT_TopUserName, tDestName);

		ituWidgetSetVisible(MT_TopUserName,true);

		//printf("[MT_UserDisplay][show]MT_TopUserName:%s\n",tDestName);
	}
	else
	{
		ituWidgetSetVisible(MT_TopUserName,false);

		//printf("[MT_UserDisplay][hide]tDestName:%s\n",tDestName);
	}

/*
	if((gMainTimerCnt_10m - gMenu_UI_Time) < 500)		// 5sec	
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(MT_TopUserName, tDestName);

		ituWidgetSetVisible(MT_TopUserName,true);
	}
	else if(((gMainTimerCnt_10m - gMenu_UI_Time) >= 500)||(M7_UserIndex > (MAX_SU100_USER_NUM-1)))
	{
		ituWidgetSetVisible(MT_TopUserName,false);
	}
*/
}

void MT_ChoiceMenuClear(uint8_t MenuIndex)
{
	printf("MT_ChoiceMenuClear [%d]\n",MenuIndex);
	switch(MenuIndex)
	{
		case Menu_Trip:
		{
//			if((ituWidgetIsVisible(M1_1a_ZoomBTN) == true)||(ituWidgetIsVisible(M1_1b_ZoomBTN) == true))
			{
				ituWidgetSetVisible(M1_1a_ZoomBTN,false);
				ituWidgetSetVisible(M1_1b_ZoomBTN,false);
		
				ituWidgetSetVisible(M1_1a_ANI, true);
				ituWidgetSetVisible(M1_1b_ANI, true);
		
				// MT_1a,1b scale down
				MT_1a_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M1_1a_ANI, 1);
				
				MT_1b_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M1_1b_ANI, 1);
				
				//ituWidgetSetVisible(M1_1a_BTN, true); //chpark.24.04.22
				//ituWidgetSetVisible(M1_1b_BTN, true); //chpark.24.04.22
			}
		}
		break;

		case Menu_AUX:
		{
//			if(ituWidgetIsVisible(M1_2_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M1_2_ZoomBTN,false);
				ituWidgetSetVisible(M1_2_ANI, true);
		
				// MT_2 scale down
				MT_2_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M1_2_ANI, 1);//chpark.24.04.22
				//ituWidgetSetVisible(M1_2_BTN, true);//chpark.24.04.22
			}
		}
		break;

		case Menu_LCD:
		{
//			if(ituWidgetIsVisible(M1_3_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M1_3_ZoomBTN,false);
				ituWidgetSetVisible(M1_3_ANI, true);
		
				// MT_3 scale down
				MT_3_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M1_3_ANI, 1);
				//ituWidgetSetVisible(M1_3_BTN, true);//chpark.24.04.22
			}
		}
		break;

		case Menu_Data:
		{
//			if(ituWidgetIsVisible(M1_4_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M1_4_ZoomBTN,false);
				ituWidgetSetVisible(M1_4_ANI, true);
		
				// MT_4 scale down
				MT_4_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M1_4_ANI, 1);
				//ituWidgetSetVisible(M1_4_BTN, true);//chpark.24.04.22
			}
		}
		break;

		case Menu_Time:
		{
//			if(ituWidgetIsVisible(M1_5_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M1_5_ZoomBTN,false);
				ituWidgetSetVisible(M1_5_ANI, true);
		
				// MT_5 scale down
				MT_5_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M1_5_ANI, 1);
				//ituWidgetSetVisible(M1_5_BTN, true);//chpark.24.04.22
			}
		}
		break;

		case Menu_Preset:
		{
//	 		if(ituWidgetIsVisible(M1_6_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M1_6_ZoomBTN,false);
		
				ituWidgetSetVisible(M1_6_ANI, true);
		
				// MT_6 scale down
				MT_6_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M1_6_ANI, 1);
				//ituWidgetSetVisible(M1_6_BTN, true);//chpark.24.04.22
			}	
		}
		break;

		case Menu_ErCode:
		{
//			if(ituWidgetIsVisible(M1_7_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M1_7_ZoomBTN,false);
		
				ituWidgetSetVisible(M1_7_ANI, true);
		
				// MT_7 scale down
				MT_7_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M1_7_ANI, 1);
				//ituWidgetSetVisible(M1_7_BTN, true);//chpark.24.04.22
			}
		}
		break;
		case Menu_UserDefine:
		{
//			if((ituWidgetIsVisible(M1_8_ZoomBTN) == true)&&(gMT_UserFlag != 2))
			if((gMT_UserFlag != 2)&&(gMenu_UI_Time != 0))
			{
				ituWidgetSetVisible(M1_8_ZoomBTN,false);
		
				ituWidgetSetVisible(M1_8_ANI, true);
		
				// MT_8 scale down
				MT_8_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M1_8_ANI, 1);
				//ituWidgetSetVisible(M1_8_BTN, true);//chpark.24.04.22
			}
		}
		break;
	}
}

bool MT_AniMenuStatus(void)
{

	return 0;
	if(ituWidgetIsVisible(M1_1a_ZoomBTN) == true)
	{
		if(MenuTop_Choice_Index != 1)
		{
			printf("MT_AniMenuStatus 1\n");
			MT_1a_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_1a_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_1a_ANI) == true)
	{
		if(MenuTop_Choice_Index != 1)
		{
			printf("MT_AniMenuStatus 2\n");
			MT_1a_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_1a_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_1a_Up1) == true)
	{
		if(MenuTop_Choice_Index != 1)
		{
			printf("MT_AniMenuStatus 3\n");
			MT_1a_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_1a_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_1a_Up0) == true)
	{
		if(MenuTop_Choice_Index != 1)
		{
			printf("MT_AniMenuStatus 4\n");
			MT_1a_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_1a_ANIStopped(NULL,NULL);
		}
	}
	
	if(ituWidgetIsVisible(M1_1b_ZoomBTN) == true)
	{
		if(MenuTop_Choice_Index != 1)
		{
			printf("MT_AniMenuStatus 5\n");
			MT_1b_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_1b_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_1b_ANI) == true)
	{
		if(MenuTop_Choice_Index != 1)
		{
			printf("MT_AniMenuStatus 6\n");
			MT_1b_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_1b_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_1b_Up1) == true)
	{
		if(MenuTop_Choice_Index != 1)
		{
			printf("MT_AniMenuStatus 7\n");
			MT_1b_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_1b_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_1b_Up0) == true)
	{
		if(MenuTop_Choice_Index != 1)
		{
			printf("MT_AniMenuStatus 8\n");
			MT_1b_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_1b_ANIStopped(NULL,NULL);
		}
	}
	
	if(ituWidgetIsVisible(M1_2_ZoomBTN) == true)
	{
		if(MenuTop_Choice_Index != 2)
		{
			printf("MT_AniMenuStatus 9\n");
			MT_2_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_2_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_2_ANI) == true)
	{
		if(MenuTop_Choice_Index != 2)
		{
			printf("MT_AniMenuStatus 10\n");
			MT_2_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_2_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_2_Up1) == true)
	{
		if(MenuTop_Choice_Index != 2)
		{
			printf("MT_AniMenuStatus 11\n");
			MT_2_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_2_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_2_Up0) == true)
	{
		if(MenuTop_Choice_Index != 2)
		{
			printf("MT_AniMenuStatus 12\n");
			MT_2_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_2_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(M1_3_ZoomBTN) == true)
	{
		if(MenuTop_Choice_Index != 3)
		{
			printf("MT_AniMenuStatus 13\n");
			MT_3_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_3_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_3_ANI) == true)
	{
		if(MenuTop_Choice_Index != 3)
		{
			printf("MT_AniMenuStatus 14\n");
			MT_3_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_3_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_3_Up1) == true)
	{
		if(MenuTop_Choice_Index != 3)
		{
			printf("MT_AniMenuStatus 15\n");
			MT_3_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_3_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_3_Up0) == true)
	{
		if(MenuTop_Choice_Index != 3)
		{
			printf("MT_AniMenuStatus 16\n");
			MT_3_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_3_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(M1_4_ZoomBTN) == true)
	{
		if(MenuTop_Choice_Index != 4)
		{
			printf("MT_AniMenuStatus 17\n");
			MT_4_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_4_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_4_ANI) == true)
	{
		if(MenuTop_Choice_Index != 4)
		{
			printf("MT_AniMenuStatus 18\n");
			MT_4_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_4_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_4_Up1) == true)
	{
		if(MenuTop_Choice_Index != 4)
		{
			printf("MT_AniMenuStatus 19\n");
			MT_4_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_4_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_4_Up0) == true)
	{
		if(MenuTop_Choice_Index != 4)
		{
			printf("MT_AniMenuStatus 20\n");
			MT_4_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_4_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(M1_5_ZoomBTN) == true)
	{
		if(MenuTop_Choice_Index != 5)
		{
			printf("MT_AniMenuStatus 21\n");
			MT_5_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_5_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_5_ANI) == true)
	{
		if(MenuTop_Choice_Index != 5)
		{
			printf("MT_AniMenuStatus 22\n");
			MT_5_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_5_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_5_Up1) == true)
	{
		if(MenuTop_Choice_Index != 5)
		{
			printf("MT_AniMenuStatus 23\n");
			MT_5_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_5_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_5_Up0) == true)
	{
		if(MenuTop_Choice_Index != 5)
		{
			printf("MT_AniMenuStatus 24\n");
			MT_5_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_5_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(M1_6_ZoomBTN) == true)
	{
		if(MenuTop_Choice_Index != 6)
		{
			printf("MT_AniMenuStatus 25\n");
			MT_6_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_6_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_6_ANI) == true)
	{
		if(MenuTop_Choice_Index != 6)
		{
			printf("MT_AniMenuStatus 26\n");
			MT_6_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_6_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_6_Up1) == true)
	{
		if(MenuTop_Choice_Index != 6)
		{
			printf("MT_AniMenuStatus 27\n");
			MT_6_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_6_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_6_Up0) == true)
	{
		if(MenuTop_Choice_Index != 6)
		{
			printf("MT_AniMenuStatus 28\n");
			MT_6_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_6_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(M1_7_ZoomBTN) == true)
	{
		if(MenuTop_Choice_Index != 7)
		{
			printf("MT_AniMenuStatus 29\n");
			MT_7_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_7_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_7_ANI) == true)
	{
		if(MenuTop_Choice_Index != 7)
		{
			printf("MT_AniMenuStatus 30\n");
			MT_7_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_7_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_7_Up1) == true)
	{
		if(MenuTop_Choice_Index != 7)
		{
			printf("MT_AniMenuStatus 31\n");
			MT_7_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_7_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M1_7_Up0) == true)
	{
		if(MenuTop_Choice_Index != 7)
		{
			printf("MT_AniMenuStatus 32\n");
			MT_7_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_7_ANIStopped(NULL,NULL);
		}
	}

	if((ituWidgetIsVisible(M1_8_ZoomBTN) == true)&&(MT_8_OnOff_Flag == 1))
	{
		if(MenuTop_Choice_Index != 8)
		{
			printf("MT_AniMenuStatus 33\n");
			MT_8_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_8_ANIStopped(NULL,NULL);
		}
	}
	if((ituWidgetIsVisible(M1_8_ANI) == true)&&(MT_8_OnOff_Flag == 1))
	{
		if(MenuTop_Choice_Index != 8)
		{
			printf("MT_AniMenuStatus 34\n");
			MT_8_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_8_ANIStopped(NULL,NULL);
		}
	}
	if((ituWidgetIsVisible(M1_8_Up1) == true)&&(MT_8_OnOff_Flag == 1))
	{
		if(MenuTop_Choice_Index != 8)
		{
			printf("MT_AniMenuStatus 35\n");
			MT_8_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_8_ANIStopped(NULL,NULL);
		}
	}
	if((ituWidgetIsVisible(M1_8_Up0) == true)&&(MT_8_OnOff_Flag == 1))
	{
		if(MenuTop_Choice_Index != 8)
		{
			printf("MT_AniMenuStatus 36\n");
			MT_8_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M1_8_ANIStopped(NULL,NULL);
		}
	}

}

void MT_8_Display(uint8_t tDis_Flag)
{
	if (MT_8_OnOff_Flag!=tDis_Flag)
	{
		MT_8_Scaleflag = 0;
		if(tDis_Flag == 1)		// on
		{
			if(ituWidgetIsVisible(M1_8_ZoomBTN) == true)	// JPHong 2024-03-20 14587
			{
				ituWidgetSetVisible(M1_8_ZoomBTN,true);
				ituWidgetSetVisible(M1_8_BTN,false);			
			}
			else
			{
				ituWidgetSetVisible(M1_8_ZoomBTN,false);
				ituWidgetSetVisible(M1_8_BTN,true);
			}
			ituWidgetSetVisible(M1_8_ANI, false);
	
			printf("MT_8_Display=[%d][%d]\n",MT_8_OnOff_Flag,tDis_Flag);
			
			
		}
		else if(tDis_Flag == 0)		// off
		{
			ituWidgetSetVisible(M1_8_ZoomBTN,false);
			ituWidgetSetVisible(M1_8_ANI, false);
			
			ituWidgetSetVisible(M1_8_BTN,false);
		}
		MT_8_OnOff_Flag=tDis_Flag;
	}
}
