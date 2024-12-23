#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

/* widgets:
Menu4_Layer
Menu_LcdBG
M4_TopUserName
M4_TopStandard
M4_TopAdmin
M4_12_ZoomBTN
M4_11_ZoomBTN
M4_10_ZoomBTN
M4_9_ZoomBTN
M4_8_ZoomBTN
M4_7_ZoomBTN
M4_6_ZoomBTN
M4_5_ZoomBTN
M4_12_BTN
M4_11_BTN
M4_10_BTN
M4_9_BTN
M4_8_BTN
M4_7_BTN
M4_6_BTN
M4_5_BTN
M4_12_ANI
M4_12_Up1
M4_12_Up0
M4_11_ANI
M4_11_Up1
M4_11_Up0
M4_10_ANI
M4_10_Up1
M4_10_Up0
M4_9_ANI
M4_9_Up1
M4_9_Up0
M4_8_ANI
M4_8_Up1
M4_8_Up0
M4_7_ANI
M4_7_Up1
M4_7_Up0
M4_6_ANI
M4_6_Up1
M4_6_Up0
M4_5_ANI
M4_5_Up1
M4_5_Up0
M4_4_BTN
M4_3_BTN
M4_2_BTN
M4_1_BTN
M4_Night_Value
M4_Day_Value
M4_D7_BTN
M4_D6_BTN
M4_D5_BTN
*/

static ITUText *M4_Day_Value;
static ITUText *M4_Night_Value;

static ITUButton* M4_12_ZoomBTN;
static ITUButton* M4_11_ZoomBTN;
static ITUButton* M4_10_ZoomBTN;
static ITUButton* M4_9_ZoomBTN;
static ITUButton* M4_8_ZoomBTN;
static ITUButton* M4_7_ZoomBTN;
static ITUButton* M4_6_ZoomBTN;
static ITUButton* M4_5_ZoomBTN;
static ITUButton* M4_12_BTN;
static ITUButton* M4_11_BTN;
static ITUButton* M4_10_BTN;
static ITUButton* M4_9_BTN;
static ITUButton* M4_8_BTN;
static ITUButton* M4_7_BTN;
static ITUButton* M4_6_BTN;
static ITUButton* M4_5_BTN;
static ITUAnimation* M4_12_ANI;
static ITUIcon* M4_12_Up1;
static ITUIcon* M4_12_Up0;
static ITUAnimation* M4_11_ANI;
static ITUIcon* M4_11_Up1;
static ITUIcon* M4_11_Up0;
static ITUAnimation* M4_10_ANI;
static ITUIcon* M4_10_Up1;
static ITUIcon* M4_10_Up0;
static ITUAnimation* M4_9_ANI;
static ITUIcon* M4_9_Up1;
static ITUIcon* M4_9_Up0;
static ITUAnimation* M4_8_ANI;
static ITUIcon* M4_8_Up1;
static ITUIcon* M4_8_Up0;
static ITUAnimation* M4_7_ANI;
static ITUIcon* M4_7_Up1;
static ITUIcon* M4_7_Up0;
static ITUAnimation* M4_6_ANI;
static ITUIcon* M4_6_Up1;
static ITUIcon* M4_6_Up0;
static ITUAnimation* M4_5_ANI;
static ITUIcon* M4_5_Up1;
static ITUIcon* M4_5_Up0;
static ITUButton* M4_4_BTN;
static ITUButton* M4_3_BTN;
static ITUButton* M4_2_BTN;
static ITUButton* M4_1_BTN;
static ITUButton* M4_D7_BTN;
static ITUButton* M4_D6_BTN;
static ITUButton* M4_D5_BTN;
  
static ITUText* M4_TopUserName;
static ITUIcon* M4_TopStandard;
static ITUIcon* M4_TopAdmin;


static uint8_t M4_DayBright = 100;		// LCD_BL_Day
static uint8_t M4_NightBright = 10;		// LCD_BL_Night
uint8_t M4_BGMode = BG_BLACK;					// BG_Color 				// BG_BLUE:1 , BG_BLACK:2
uint8_t M4_UnitFlag = 0;				// SIUS_SEL_Flag		// 0:SI , 1:US

static uint8_t M4_5_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M4_6_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down

static uint8_t M4_7_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M4_8_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down

static uint8_t M4_9_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M4_10_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M4_11_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M4_12_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down

static uint32_t M4_SecTimer;
static uint8_t M4_1SecCNT;
static uint8_t M4_ExitCNT = 0;
static uint32_t M4_BlinkTimer;
static uint8_t M4_BlinkFlag = 0;

static uint8_t M4_SkipMenuTop=0;

//static uint8_t M4_EditMenu = 0;
static char M4_PARA[2];
static uint8_t M4_1TimeFlag = 1;
static uint32_t M4_JogTimer = 0;

static uint32_t M4_FastTimer;
static uint8_t M4_FastFlag = 0;
static uint8_t M4_FastSource = 0;

uint32_t test4_timer = 0;

void M4_SubMenuClear(uint8_t MenuIndex);
void M4_BGModeClear(void);
void M4_UnitClear(void);

bool M4_AniMenuStatus(void);

bool M4_BTN_MouseDn(ITUWidget* widget, char* param);
bool M4_BTN_MouseUp(ITUWidget* widget, char* param);

bool M4D5_MouseDn(ITUWidget* widget, char* param);
bool M4D5_MouseUp(ITUWidget* widget, char* param);
bool M4D7_MouseUp(ITUWidget* widget, char* param);

bool M4_UserDisplay(void);

extern void GotoMenuTop(void);
extern bool LCDLayer_BackLight(int8_t BL_Data);


bool M4_TouchUP_Restet(void)
{
	M4_1SecCNT = 100;
	M4_FastFlag = 0;
	M4_FastSource = 0;

	M4_SecTimer = gMainTimerCnt_10m;
}
bool M4_Enter(ITUWidget* widget, char* param)
{
	char ptBrightValue[4];
	
	SetWidget(M4_Day_Value,"M4_Day_Value");
	SetWidget(M4_Night_Value,"M4_Night_Value");

	SetWidget(M4_12_ZoomBTN,"M4_12_ZoomBTN");
	SetWidget(M4_11_ZoomBTN,"M4_11_ZoomBTN");
	SetWidget(M4_10_ZoomBTN,"M4_10_ZoomBTN");
	SetWidget(M4_9_ZoomBTN,"M4_9_ZoomBTN");
	SetWidget(M4_8_ZoomBTN,"M4_8_ZoomBTN");
	SetWidget(M4_7_ZoomBTN,"M4_7_ZoomBTN");
	SetWidget(M4_6_ZoomBTN,"M4_6_ZoomBTN");
	SetWidget(M4_5_ZoomBTN,"M4_5_ZoomBTN");
	SetWidget(M4_12_BTN,"M4_12_BTN");
	SetWidget(M4_11_BTN,"M4_11_BTN");
	SetWidget(M4_10_BTN,"M4_10_BTN");
	SetWidget(M4_9_BTN,"M4_9_BTN");
	SetWidget(M4_8_BTN,"M4_8_BTN");
	SetWidget(M4_7_BTN,"M4_7_BTN");
	SetWidget(M4_6_BTN,"M4_6_BTN");
	SetWidget(M4_5_BTN,"M4_5_BTN");
	SetWidget(M4_12_ANI,"M4_12_ANI");
	SetWidget(M4_12_Up1,"M4_12_Up1");
	SetWidget(M4_12_Up0,"M4_12_Up0");
	SetWidget(M4_11_ANI,"M4_11_ANI");
	SetWidget(M4_11_Up1,"M4_11_Up1");
	SetWidget(M4_11_Up0,"M4_11_Up0");
	SetWidget(M4_10_ANI,"M4_10_ANI");
	SetWidget(M4_10_Up1,"M4_10_Up1");
	SetWidget(M4_10_Up0,"M4_10_Up0");
	SetWidget(M4_9_ANI,"M4_9_ANI");
	SetWidget(M4_9_Up1,"M4_9_Up1");
	SetWidget(M4_9_Up0,"M4_9_Up0");
	SetWidget(M4_8_ANI,"M4_8_ANI");
	SetWidget(M4_8_Up1,"M4_8_Up1");
	SetWidget(M4_8_Up0,"M4_8_Up0");
	SetWidget(M4_7_ANI,"M4_7_ANI");
	SetWidget(M4_7_Up1,"M4_7_Up1");
	SetWidget(M4_7_Up0,"M4_7_Up0");
	SetWidget(M4_6_ANI,"M4_6_ANI");
	SetWidget(M4_6_Up1,"M4_6_Up1");
	SetWidget(M4_6_Up0,"M4_6_Up0");
	SetWidget(M4_5_ANI,"M4_5_ANI");
	SetWidget(M4_5_Up1,"M4_5_Up1");
	SetWidget(M4_5_Up0,"M4_5_Up0");
	SetWidget(M4_4_BTN,"M4_4_BTN");
	SetWidget(M4_3_BTN,"M4_3_BTN");
	SetWidget(M4_2_BTN,"M4_2_BTN");
	SetWidget(M4_1_BTN,"M4_1_BTN");
	SetWidget(M4_D7_BTN,"M4_D7_BTN");
	SetWidget(M4_D6_BTN,"M4_D6_BTN");
	SetWidget(M4_D5_BTN,"M4_D5_BTN");

	SetWidget(M4_TopUserName,"M4_TopUserName");
	SetWidget(M4_TopStandard,"M4_TopStandard");
	SetWidget(M4_TopAdmin,"M4_TopAdmin");
	
ituWidgetSetVisible(M4_9_ZoomBTN,false);
ituWidgetSetVisible(M4_10_ZoomBTN,false);
ituWidgetSetVisible(M4_11_ZoomBTN,false);
ituWidgetSetVisible(M4_12_ZoomBTN,false);
	
/*
	printf("\r\n>>> M4 Day Bright : %d\r\n",LCD_BL_Day);
	printf(">>> M4 Night Bright : %d\r\n",LCD_BL_Night);
	printf(">>> M4 BG Color : %d\r\n",BG_Color);
	printf(">>> M4 Unit : %d\r\n",SIUS_SEL_Flag);
*/

	M4_DayBright = LCD_BL_Day;
	M4_NightBright = LCD_BL_Night;

	memset(ptBrightValue,0,4);
	sprintf(ptBrightValue,"%3d",M4_DayBright);
	ituTextSetString(M4_Day_Value, ptBrightValue);
	ituWidgetSetVisible(M4_Day_Value, true);

	memset(ptBrightValue,0,4);
	sprintf(ptBrightValue,"%3d",M4_NightBright);
	ituTextSetString(M4_Night_Value, ptBrightValue);
	ituWidgetSetVisible(M4_Night_Value, true);

	M4_SubMenuClear(MenuLCD_Choice_Index);//chpark.24.11.07
	MenuLCD_Choice_Index = mLCD_DayBright;//chpark.24.11.07

	switch(MenuLCD_Choice_Index)
	{
		case mLCD_DayBright:
		{

			ituWidgetSetVisible(M4_9_BTN,false);
			ituWidgetSetVisible(M4_9_ANI,true);

			// M4_9 scale up
			M4_9_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_9_ANI, 1);
		}	
		break;
		
		case mLCD_NightBright:
		{


			ituWidgetSetVisible(M4_10_BTN,false);
			ituWidgetSetVisible(M4_10_ANI,true);

			// M4_10 scale up
			M4_10_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_10_ANI, 1);
		}	
		break;

		case mLCD_BGMode:
		{
			ituWidgetSetVisible(M4_11_BTN,false);
			ituWidgetSetVisible(M4_11_ANI,true);
			// M4_11 scale up
			M4_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_11_ANI, 1);
		}	
		break;

		case mLCD_UnitMode:
		{
			ituWidgetSetVisible(M4_12_BTN,false);
			ituWidgetSetVisible(M4_12_ANI,true);

			// M4_12 scale up
			M4_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_12_ANI, 1);
		}	
		break;
	}

	M4_BGModeClear();
	
	M4_BGMode = BG_Color;
	
	switch(M4_BGMode)
	{
		case BG_BLACK:
		{
			ituWidgetSetVisible(M4_5_BTN,false);
			ituWidgetSetVisible(M4_5_ANI,true);

			// M4_5 scale up
			M4_5_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_5_ANI, 0);
		}	
		break;
		
		case BG_BLUE:
		{


			ituWidgetSetVisible(M4_6_BTN,false);
			ituWidgetSetVisible(M4_6_ANI,true);

			// M4_6 scale up
			M4_6_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_6_ANI, 0);
		}
		break;
	}

	M4_UnitClear();
	
	M4_UnitFlag = SIUS_SEL_Flag;
	
	switch(M4_UnitFlag)
	{
		case 0:		// 0:SI
		{


			ituWidgetSetVisible(M4_7_BTN,false);
			ituWidgetSetVisible(M4_7_ANI,true);

			// M4_7 scale up
			M4_7_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_7_ANI, 0);
		}	
		break;
		
		case 1:		// 1:US
		{


			ituWidgetSetVisible(M4_8_BTN,false);
			ituWidgetSetVisible(M4_8_ANI,true);

			// M4_8 scale up
			M4_8_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_8_ANI, 0);
		}
		break;
	}

	M4_UserDisplay();

	M4_SecTimer = gMainTimerCnt_10m;
	M4_ExitCNT = 0;

	M4_BlinkFlag = 0;

	M4_1SecCNT = 100;
	M4_FastFlag = 0;
	M4_FastSource = 0;

    return true;
}

bool M4_Timer(ITUWidget* widget, char* param)
{
	char ptBrightValue[4];

#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	
	if((gMainTimerCnt_10m - M4_SecTimer) >= 100)
	{
		if(M4_ExitCNT < 30)
			M4_ExitCNT++;

		if(M4_ExitCNT == 30)
		{
			M4_ExitCNT = 100;
			printf("\r\nM4 Menu 30Sec Exit > goto Main\r\n");
			GotoMain(MAIN_MODE_NORMAL); 
		}

		// long arrow btn check
		if(M4_1SecCNT < 1)
			M4_1SecCNT++;

		if(M4_1SecCNT == 1)
		{
			M4_1SecCNT == 100;
			printf("\r\nM4 Long key\r\n");

			if(M4_FastFlag == 1)
			{
				//printf(" longkey time1 = %d\r\n",gMainTimerCnt_10m-test4_timer);

				M4_FastFlag = 2;
				M4_FastTimer = gMainTimerCnt_10m;
			}
		}
			
		M4_SecTimer = gMainTimerCnt_10m;
	}

	if(M4_BlinkFlag == 1)		// on
	{
		if((gMainTimerCnt_10m - M4_BlinkTimer) >= 30) // 300msec
		{
			switch(MenuLCD_Choice_Index)
			{
				case mLCD_DayBright:
					ituWidgetSetVisible(M4_Day_Value, true);
				break;
				
				case mLCD_NightBright:
					ituWidgetSetVisible(M4_Night_Value, true);
				break;
			}

			M4_BlinkFlag = 2;
			M4_BlinkTimer = gMainTimerCnt_10m;
		}
	}
	else if(M4_BlinkFlag == 2)	// off
	{
		if((gMainTimerCnt_10m - M4_BlinkTimer) >= 30) // 300msec
		{
			switch(MenuLCD_Choice_Index)
			{
				case mLCD_DayBright:
					ituWidgetSetVisible(M4_Day_Value, false);
				break;
				
				case mLCD_NightBright:
					ituWidgetSetVisible(M4_Night_Value, false);
				break;
			}

			M4_BlinkFlag = 1;
			M4_BlinkTimer = gMainTimerCnt_10m;
		}
	}

	if(M4_FastFlag == 2)
	{
		switch(M4_FastSource)
		{
			case 1:				// day , down
				if((gMainTimerCnt_10m - M4_FastTimer) >= 10)
				{
					//printf(" longkey time2 = %d\r\n",gMainTimerCnt_10m-test4_timer);

					if(M4_DayBright > 1)		//LCD_BL_Day
						M4_DayBright --;

					memset(ptBrightValue,0,4);
					sprintf(ptBrightValue,"%3d",M4_DayBright);
					ituTextSetString(M4_Day_Value, ptBrightValue);

					if(Main_DayNight == 0)			// Day Mode
						LCDLayer_BackLight(M4_DayBright);
					
					M4_FastTimer = gMainTimerCnt_10m;
				}
			break;

			case 2:				// day , up
				if((gMainTimerCnt_10m - M4_FastTimer) >= 10)
				{
					if(M4_DayBright < 100)		//LCD_BL_Day
						M4_DayBright ++;

					memset(ptBrightValue,0,4);
					sprintf(ptBrightValue,"%3d",M4_DayBright);
					ituTextSetString(M4_Day_Value, ptBrightValue);
					
					if(Main_DayNight == 0)			// Day Mode
						LCDLayer_BackLight(M4_DayBright);

					M4_FastTimer = gMainTimerCnt_10m;
				}
			break;
			
			case 3:				// night , down
				if((gMainTimerCnt_10m - M4_FastTimer) >= 10)
				{

					//printf(" longkey time2 = %d\r\n",gMainTimerCnt_10m-test4_timer);

					if(M4_NightBright > 1)		//LCD_BL_Day
						M4_NightBright --;

					memset(ptBrightValue,0,4);
					sprintf(ptBrightValue,"%3d",M4_NightBright);
					ituTextSetString(M4_Night_Value, ptBrightValue);
					
					if(Main_DayNight == 1)			// Night Mode
						LCDLayer_BackLight(M4_NightBright);

					M4_FastTimer = gMainTimerCnt_10m;
				}
			break;

			case 4:				// night , up
				if((gMainTimerCnt_10m - M4_FastTimer) >= 10)
				{
					if(M4_NightBright < 100)		//LCD_BL_Day
						M4_NightBright ++;

					memset(ptBrightValue,0,4);
					sprintf(ptBrightValue,"%3d",M4_NightBright);
					ituTextSetString(M4_Night_Value, ptBrightValue);

					if(Main_DayNight == 1)			// Night Mode
						LCDLayer_BackLight(M4_NightBright);
					
					M4_FastTimer = gMainTimerCnt_10m;
				}
			break;
		}
	}

/*
	// jog wheel action
	if(((gMainTimerCnt_10m - M4_JogTimer) >= 20) && (M4_1TimeFlag == 0))
	{
		M4_1TimeFlag = 1;
		M4_BTN_MouseDn(NULL, M4_PARA);
		usleep(10*1000);
		M4_BTN_MouseUp(NULL, M4_PARA);
	}
*/

	M4_AniMenuStatus();

	M4_UserDisplay();
	
    return true;
}

bool M4_JogWheel(ITUWidget* widget, char* param)
{
	char ptBrightValue[4];
	uint8_t tJogDir = 0;
	
	M4_ExitCNT = 0;
	
	tJogDir = param[0];
	printf("MenuLCD JogWheel [%d][%d][%d]\r\n",tJogDir,M4_BlinkFlag,MenuLCD_Choice_Index);

	//if((tJogDir == JOG_DIAL_ROT_RIGHT) && (M4_1TimeFlag == 1))
	if(tJogDir == JOG_DIAL_ROT_RIGHT)
	{
		if(M4_BlinkFlag == 0)
		{
			M4_SubMenuClear(MenuLCD_Choice_Index);
			MenuLCD_Choice_Index++;
			if(MenuLCD_Choice_Index > mLCD_UnitMode)
				MenuLCD_Choice_Index = mLCD_DayBright;
			
			memset(M4_PARA,0,2);
			itoa((MenuLCD_Choice_Index+8),M4_PARA,10);
			
//			printf("wheel CW Branch sub Lcd Value : %s\r\n",M4_PARA);
			
			param[0] = 0;
			//M4_JogTimer = gMainTimerCnt_10m;
			//M4_1TimeFlag = 0;
      
			M4_BTN_MouseDn(NULL, M4_PARA);
			usleep(10*1000);
			M4_BTN_MouseUp(NULL, M4_PARA);
		}
		else if(M4_BlinkFlag != 0)
		{
			switch(MenuLCD_Choice_Index)
			{
				case mLCD_DayBright:
				{
					if(M4_FastFlag != 2)
					{
						if((M4_DayBright < 100)&&(M4_SkipMenuTop == 0))		//LCD_BL_Day
							M4_DayBright++;
						else if(M4_SkipMenuTop == 1)
							M4_SkipMenuTop = 0;
			
						memset(ptBrightValue,0,4);
						sprintf(ptBrightValue,"%3d",M4_DayBright);
						ituTextSetString(M4_Day_Value, ptBrightValue);

						if(Main_DayNight == 0)
							LCDLayer_BackLight(M4_DayBright);
					}
				}
				break;
				
				case mLCD_NightBright:
				{
					if(M4_FastFlag != 2)
					{
						if((M4_NightBright < 100)&&(M4_SkipMenuTop == 0))		//LCD_BL_Night
							M4_NightBright++;
						else if(M4_SkipMenuTop == 1)
							M4_SkipMenuTop = 0;
         
						memset(ptBrightValue,0,4);
						sprintf(ptBrightValue,"%3d",M4_NightBright);
						ituTextSetString(M4_Night_Value, ptBrightValue);

						if(Main_DayNight == 1)			// 0:Day , 1:Night
							LCDLayer_BackLight(M4_NightBright);
					}
				}
				break;
			}
		}
	}
	//else if((tJogDir == JOG_DIAL_ROT_LEFT) && (M4_1TimeFlag == 1))
	else if(tJogDir == JOG_DIAL_ROT_LEFT)
	{
		if(M4_BlinkFlag == 0)
		{
			M4_SubMenuClear(MenuLCD_Choice_Index);
			if(MenuLCD_Choice_Index > mLCD_DayBright)
				MenuLCD_Choice_Index--;
			else if(MenuLCD_Choice_Index == mLCD_DayBright)
				MenuLCD_Choice_Index = mLCD_UnitMode;
			
			memset(M4_PARA,0,2);
			itoa((MenuLCD_Choice_Index+8),M4_PARA,10);
			
//			printf("wheel ccw Branch sub Lcd Value : %s\r\n",M4_PARA);
			
			param[0] = 0;
			//M4_JogTimer = gMainTimerCnt_10m;
			//M4_1TimeFlag = 0;
    
			M4_BTN_MouseDn(NULL, M4_PARA);
			usleep(10*1000);
			M4_BTN_MouseUp(NULL, M4_PARA);
		}
		else if(M4_BlinkFlag != 0)
		{
			switch(MenuLCD_Choice_Index)
			{
				case mLCD_DayBright:
				{
					if(M4_FastFlag != 2)
					{
						if((M4_DayBright > 1)&&(M4_SkipMenuTop == 0))		//LCD_BL_Day
							M4_DayBright --;
						else if(M4_SkipMenuTop == 1)
							M4_SkipMenuTop = 0;
			
						memset(ptBrightValue,0,4);
						sprintf(ptBrightValue,"%3d",M4_DayBright);
						ituTextSetString(M4_Day_Value, ptBrightValue);

						if(Main_DayNight == 0)		// Day Mode
							LCDLayer_BackLight(M4_DayBright);
					}
				}
				break;
				
				case mLCD_NightBright:
				{
					if(M4_FastFlag != 2)
					{
						if((M4_NightBright > 1)&&(M4_SkipMenuTop == 0))		//LCD_BL_Night
							M4_NightBright --;
						else if(M4_SkipMenuTop == 1)
							M4_SkipMenuTop = 0;
      
						memset(ptBrightValue,0,4);
						sprintf(ptBrightValue,"%3d",M4_NightBright);
						ituTextSetString(M4_Night_Value, ptBrightValue);

						if(Main_DayNight == 1)			// 1:Night Mode
							LCDLayer_BackLight(M4_NightBright);
					}
				}
				break;
			}
		}
	}

    return true;
}

bool M4_JogButton(ITUWidget* widget, char* param)
{
	char ptBrightValue[4];
	static uint8_t JogButton = 0,JogBTN_Old=0;

	
	M4_ExitCNT = 0;

	JogButton = param[0];
	printf("MenuLCD JogButton [%d][%d]\r\n",JogButton,MenuLCD_Choice_Index);

	switch(JogButton)
	{
		case JOG_DIAL_KEY_RIGHT:
			// no blink
			if(M4_BlinkFlag == 0)
			{
// JPHong 2024-03-30 Page26
				if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				{
				}
				else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				{
					M4_SubMenuClear(MenuLCD_Choice_Index);
					// 1>2,2>1,3>4,4>3
					if(MenuLCD_Choice_Index == mLCD_DayBright)
						MenuLCD_Choice_Index = mLCD_BGMode;
					else if(MenuLCD_Choice_Index == mLCD_NightBright)
						MenuLCD_Choice_Index = mLCD_UnitMode;
					else if(MenuLCD_Choice_Index == mLCD_BGMode)
						MenuLCD_Choice_Index = mLCD_DayBright;
					else if(MenuLCD_Choice_Index == mLCD_UnitMode)
						MenuLCD_Choice_Index = mLCD_NightBright;
					memset(M4_PARA,0,2);
					itoa((MenuLCD_Choice_Index+8),M4_PARA,10);
				
	//				printf("button down Branch sub Lcd Value : %s\r\n",M4_PARA);
				
					M4_BTN_MouseDn(NULL, M4_PARA);
					usleep(10*1000);
					M4_BTN_MouseUp(NULL, M4_PARA);
				}
			}
			else if(M4_BlinkFlag != 0)
			{
				switch(MenuLCD_Choice_Index)
				{
					case mLCD_DayBright:

						printf("button up  - Day Bright M DN\r\n");
						// day value long key check start
						M4_SecTimer = gMainTimerCnt_10m;
						M4_1SecCNT = 0;
						M4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

						M4_FastSource = 2;

						JogBTN_Old = JogButton;
					break;
					
					case mLCD_NightBright:

						printf("button up - Night Bright M DN\r\n");
						
						M4_SecTimer = gMainTimerCnt_10m;
						M4_1SecCNT = 0;
						M4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

						M4_FastSource = 4;

						JogBTN_Old = JogButton;
					break;
				}				
			}
		break;	
		case JOG_DIAL_KEY_UP:

			// no blink
			if(M4_BlinkFlag == 0)
			{
				M4_SubMenuClear(MenuLCD_Choice_Index);
				if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				{
					if(MenuLCD_Choice_Index > mLCD_DayBright)
						MenuLCD_Choice_Index--;
					else if(MenuLCD_Choice_Index == mLCD_DayBright)
						MenuLCD_Choice_Index = mLCD_UnitMode;
				}
				else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				{
					// 1>2,2>1,3>4,4>3
					if(MenuLCD_Choice_Index == mLCD_DayBright)
						MenuLCD_Choice_Index = mLCD_NightBright;
					else if(MenuLCD_Choice_Index == mLCD_NightBright)
						MenuLCD_Choice_Index = mLCD_DayBright;
					else if(MenuLCD_Choice_Index == mLCD_BGMode)
						MenuLCD_Choice_Index = mLCD_UnitMode;
					else if(MenuLCD_Choice_Index == mLCD_UnitMode)
						MenuLCD_Choice_Index = mLCD_BGMode;
				}
				
				memset(M4_PARA,0,2);
				itoa((MenuLCD_Choice_Index+8),M4_PARA,10);
			
				printf("button down Branch sub Lcd Value : %s\r\n",M4_PARA);
			
				M4_BTN_MouseDn(NULL, M4_PARA);
				usleep(10*1000);
				M4_BTN_MouseUp(NULL, M4_PARA);
			}
			else
			{
				switch(MenuLCD_Choice_Index)
				{
					case mLCD_DayBright:

						printf("button up  - Day Bright M DN\r\n");
						// day value long key check start
						M4_SecTimer = gMainTimerCnt_10m;
						M4_1SecCNT = 0;
						M4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

						M4_FastSource = 2;

						JogBTN_Old = JogButton;
					break;
					
					case mLCD_NightBright:

						printf("button up - Night Bright M DN\r\n");
						
						M4_SecTimer = gMainTimerCnt_10m;
						M4_1SecCNT = 0;
						M4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

						M4_FastSource = 4;

						JogBTN_Old = JogButton;
					break;
				}				
			}

			param[0] = 0;
		break;

		case JOG_DIAL_KEY_RIGHT_OFF://(JOG_DIAL_KEY_RIGHT&0x3f):
		case JOG_DIAL_KEY_UP_OFF://(JOG_DIAL_KEY_UP&0x3f):

			if(M4_BlinkFlag != 0)
			{
				switch(MenuLCD_Choice_Index)
				{
					case mLCD_DayBright:
      
						if(M4_FastFlag != 2)
						{
							if((M4_DayBright < 100)&&(M4_SkipMenuTop == 0))		//LCD_BL_Day
								M4_DayBright++;
							else if(M4_SkipMenuTop == 1)
								M4_SkipMenuTop = 0;
				
							memset(ptBrightValue,0,4);
							sprintf(ptBrightValue,"%3d",M4_DayBright);
							ituTextSetString(M4_Day_Value, ptBrightValue);

							if(Main_DayNight==0)		// Day Mode
								LCDLayer_BackLight(M4_DayBright);
						}
					break;
					
					case mLCD_NightBright:
      
						if(M4_FastFlag != 2)
						{
							if((M4_NightBright < 100)&&(M4_SkipMenuTop == 0))		//LCD_BL_Night
								M4_NightBright++;
							else if(M4_SkipMenuTop == 1)
								M4_SkipMenuTop = 0;
           
							memset(ptBrightValue,0,4);
							sprintf(ptBrightValue,"%3d",M4_NightBright);
							ituTextSetString(M4_Night_Value, ptBrightValue);

							if(Main_DayNight == 1)			// 1:Night Mode
								LCDLayer_BackLight(M4_NightBright);
						}
					break;
				}
			}
			// No scale up > day value down , long key check reset
			M4_TouchUP_Restet();
		break;

		case JOG_DIAL_KEY_LEFT:

			// no blink
			if(M4_BlinkFlag == 0)
			{
// JPHong 2024-03-30 Page26 
				if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				{
				}
				else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				{
					M4_SubMenuClear(MenuLCD_Choice_Index);
					//1>3,2>4.3>1,4>2
					if(MenuLCD_Choice_Index == mLCD_DayBright)
						MenuLCD_Choice_Index = mLCD_BGMode;
					else if(MenuLCD_Choice_Index == mLCD_NightBright)
						MenuLCD_Choice_Index = mLCD_UnitMode;
					else if(MenuLCD_Choice_Index == mLCD_BGMode)
						MenuLCD_Choice_Index = mLCD_DayBright;
					else if(MenuLCD_Choice_Index == mLCD_UnitMode)
						MenuLCD_Choice_Index = mLCD_NightBright;
					memset(M4_PARA,0,2);
					itoa((MenuLCD_Choice_Index+8),M4_PARA,10);
				
					printf("button down Branch sub Lcd Value : %s\r\n",M4_PARA);
	      
					M4_BTN_MouseDn(NULL, M4_PARA);
					usleep(10*1000);
					M4_BTN_MouseUp(NULL, M4_PARA);						
				}
			

			}
			else if(M4_BlinkFlag != 0)
			{
				switch(MenuLCD_Choice_Index)
				{
					case mLCD_DayBright:

						printf("button down - Day Bright M DN\r\n");
						
						// day value long key check start
						M4_SecTimer = gMainTimerCnt_10m;
						M4_1SecCNT = 0;
						M4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

						M4_FastSource = 1;
						JogBTN_Old = JogButton;

					break;
					
					case mLCD_NightBright:

						printf("button down - Night Bright M DN\r\n");
						
						M4_SecTimer = gMainTimerCnt_10m;
						M4_1SecCNT = 0;
						M4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

						M4_FastSource = 3;
						JogBTN_Old = JogButton;

					break;
				}				
			}
		break;
		case JOG_DIAL_KEY_DOWN:

			// no blink
			if(M4_BlinkFlag == 0)
			{
				M4_SubMenuClear(MenuLCD_Choice_Index);
				if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				{
					MenuLCD_Choice_Index++;
					if(MenuLCD_Choice_Index > mLCD_UnitMode)
						MenuLCD_Choice_Index = mLCD_DayBright;
				}
				else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				{
					//1>2,2>1,3>4,4>3
					if(MenuLCD_Choice_Index == mLCD_DayBright)
						MenuLCD_Choice_Index = mLCD_NightBright;
					else if(MenuLCD_Choice_Index == mLCD_NightBright)
						MenuLCD_Choice_Index = mLCD_DayBright;
					else if(MenuLCD_Choice_Index == mLCD_BGMode)
						MenuLCD_Choice_Index = mLCD_UnitMode;
					else if(MenuLCD_Choice_Index == mLCD_UnitMode)
						MenuLCD_Choice_Index = mLCD_BGMode;
				}
			
				memset(M4_PARA,0,2);
				itoa((MenuLCD_Choice_Index+8),M4_PARA,10);
			
				printf("button down Branch sub Lcd Value : %s\r\n",M4_PARA);
      
				M4_BTN_MouseDn(NULL, M4_PARA);
				usleep(10*1000);
				M4_BTN_MouseUp(NULL, M4_PARA);
			}
			else
			{
				switch(MenuLCD_Choice_Index)
				{
					case mLCD_DayBright:

						printf("button down - Day Bright M DN\r\n");
						
						// day value long key check start
						M4_SecTimer = gMainTimerCnt_10m;
						M4_1SecCNT = 0;
						M4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

						M4_FastSource = 1;
						JogBTN_Old = JogButton;

					break;
					
					case mLCD_NightBright:

						printf("button down - Night Bright M DN\r\n");
						
						M4_SecTimer = gMainTimerCnt_10m;
						M4_1SecCNT = 0;
						M4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

						M4_FastSource = 3;
						JogBTN_Old = JogButton;

					break;
				}				
			}

			param[0] = 0;
		break;

		case JOG_DIAL_KEY_LEFT_OFF://(JOG_DIAL_KEY_LEFT&0x3f):
		case JOG_DIAL_KEY_DOWN_OFF://(JOG_DIAL_KEY_DOWN&0x3f):

			if(M4_BlinkFlag != 0)
			{
				switch(MenuLCD_Choice_Index)
				{
					case mLCD_DayBright:
  
						if(M4_FastFlag != 2)
						{
							if((M4_DayBright > 1)&&(M4_SkipMenuTop == 0))		//LCD_BL_Day
								M4_DayBright --;
							else if(M4_SkipMenuTop == 1)
								M4_SkipMenuTop = 0;
				
							memset(ptBrightValue,0,4);
							sprintf(ptBrightValue,"%3d",M4_DayBright);
							ituTextSetString(M4_Day_Value, ptBrightValue);

							if(Main_DayNight == 0)		// Day mode
								LCDLayer_BackLight(M4_DayBright);
						}
					break;
					
					case mLCD_NightBright:
  
						if(M4_FastFlag != 2)
						{
							if((M4_NightBright > 1)&&(M4_SkipMenuTop == 0))		//LCD_BL_Night
								M4_NightBright --;
							else if(M4_SkipMenuTop == 1)
								M4_SkipMenuTop = 0;
        
							memset(ptBrightValue,0,4);
							sprintf(ptBrightValue,"%3d",M4_NightBright);
							ituTextSetString(M4_Night_Value, ptBrightValue);

							if(Main_DayNight == 1)			// 1:Night Mode
								LCDLayer_BackLight(M4_NightBright);
						}
					break;
				}
			}

			// No scale up > day value down , long key check reset
			M4_TouchUP_Restet();

		break;
		
		case JOG_DIAL_KEY_ENTER_ON:

			switch(MenuLCD_Choice_Index)
			{
				case mLCD_DayBright:
					// day bright value blink

					if(ituWidgetIsVisible(M4_9_ZoomBTN) == true)
					{
						if(M4_BlinkFlag == 0)
						{
							M4_BlinkTimer = gMainTimerCnt_10m;
							M4_BlinkFlag = 1;
						}
						else
						{
							M4_BlinkFlag = 0;
							printf("button enter - day Bright fix , blink stop\r\n");
							LCD_BL_Day = M4_DayBright;

							if(Main_DayNight==0)
								LCDLayer_BackLight(M4_DayBright);
		
							memset(ptBrightValue,0,4);
							sprintf(ptBrightValue,"%3d",M4_DayBright);
							ituTextSetString(M4_Day_Value, ptBrightValue);
							ituWidgetSetVisible(M4_Day_Value, true);
						}
					}
				break;
				
				case mLCD_NightBright:

					//M4_EditMenu = mLCD_NightBright;
					if(ituWidgetIsVisible(M4_10_ZoomBTN) == true)
					{
						if(M4_BlinkFlag == 0)
						{
							M4_BlinkTimer = gMainTimerCnt_10m;
							M4_BlinkFlag = 1;
						}
						else
						{
							M4_BlinkFlag = 0;
							printf("button enter - day Bright fix , blink stop\r\n");

							LCD_BL_Night = M4_NightBright;

							if(Main_DayNight==1)
								LCDLayer_BackLight(M4_NightBright);
		
							memset(ptBrightValue,0,4);
							sprintf(ptBrightValue,"%3d",M4_NightBright);
							ituTextSetString(M4_Night_Value, ptBrightValue);
							ituWidgetSetVisible(M4_Night_Value, true);
						}
					}

				break;
				
				case mLCD_BGMode:

					printf("\r\njog button enter bg mode\r\n");
					//M4_EditMenu = mLCD_BGMode;
					if(ituWidgetIsVisible(M4_11_ZoomBTN) == true)
					{
						if(M4_BGMode == BG_BLUE)
						{
							memset(M4_PARA,0,2);
							sprintf(M4_PARA,"%d",5);
							printf("\r\njog button enter bg blue -> bg black[%s]\r\n",M4_PARA);
						}
						else if(M4_BGMode == BG_BLACK)
						{
							memset(M4_PARA,0,2);
							sprintf(M4_PARA,"%d",6);
							printf("\r\njog button enter bg black -> bg blue[%s]\r\n",M4_PARA);
						}

						M4_BTN_MouseDn(NULL, M4_PARA);
						usleep(10*1000);
						M4_BTN_MouseUp(NULL, M4_PARA);
					}

				break;
				
				case mLCD_UnitMode:

					//M4_EditMenu = mLCD_UnitMode;
					if(ituWidgetIsVisible(M4_12_ZoomBTN) == true)
					{
						if(M4_UnitFlag == 0)
						{
							memset(M4_PARA,0,2);
							sprintf(M4_PARA,"%d",8);
						}
						else if(M4_UnitFlag == 1)
						{
							memset(M4_PARA,0,2);
							sprintf(M4_PARA,"%d",7);
						}

						M4_BTN_MouseDn(NULL, M4_PARA);
						usleep(10*1000);
						M4_BTN_MouseUp(NULL, M4_PARA);
					}

				break;
			}

			JogBTN_Old = JogButton;
			param[0] = 0;
		break;
		
		case JOG_DIAL_KEY_ENTER_OFF:

			param[0] = 0;

#ifdef USED_SAVE_CONFIG

			//M4_BGMode
			//LCD_BL_Day
			//LCD_BL_Night
			//SIUS_SEL_Flag
			XO_Save_Config(M7_UserIndex,true,0);

#else
			HT_Save_Config(1);
#endif

		break;
	}
	
    return true;
}

bool M4_JpgSwitch(ITUWidget* widget, char* param)
{
	uint8_t JogSwitch = 0;

	JogSwitch = param[0];
	printf("M4_JpgSwitch JogSwitch %d\r\n",JogSwitch);

	M4_ExitCNT = 0;

	if(JogSwitch == JOG_DIAL_SW_ESC_ON)
	{
		M4_TouchUP_Restet(); // 2024-05-17 add whlim,
		M4D5_MouseDn(NULL,NULL);
	}
	//else if(JogSwitch == (JOG_DIAL_SW_ESC_ON&0x3f))
	else if(JogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
		M4D5_MouseUp(NULL,NULL);
	}
	else if(JogSwitch == JOG_DIAL_SW_HOME)
	{
/*
#ifdef USED_SAVE_CONFIG

		//M4_BGMode
		//LCD_BL_Day
		//LCD_BL_Night
		//SIUS_SEL_Flag
	XO_Save_Config(M7_UserIndex,true,0);

#else
	HT_Save_Config(1);
#endif
*/
		GotoMain(MAIN_MODE_NORMAL); 
	}

    return true;
}

bool M4_ZoomBTN_MouseDn(ITUWidget* widget, char* param)
{
	char ptBrightValue[4];
	
	uint8_t tZoomBTN = 0;


	M4_ExitCNT = 0;
 	tZoomBTN = (uint8_t)atoi(param);

	if(M4_BlinkFlag != 0)
	{
		M4_BlinkFlag = 0;		

		if(MenuLCD_Choice_Index == mLCD_DayBright)
		{
			LCD_BL_Day = M4_DayBright;

			if (Main_DayNight==0)
				LCDLayer_BackLight(M4_DayBright);
				
			memset(ptBrightValue,0,4);
			sprintf(ptBrightValue,"%3d",M4_DayBright);
			ituTextSetString(M4_Day_Value, ptBrightValue);
			ituWidgetSetVisible(M4_Day_Value, true);
		}
		else if(MenuLCD_Choice_Index == mLCD_NightBright)
		{
			LCD_BL_Night = M4_NightBright;

			if (Main_DayNight==1)
				LCDLayer_BackLight(M4_NightBright);
				
			memset(ptBrightValue,0,4);
			sprintf(ptBrightValue,"%3d",M4_NightBright);
			ituTextSetString(M4_Night_Value, ptBrightValue);
			ituWidgetSetVisible(M4_Night_Value, true);
		}
		
#ifdef USED_SAVE_CONFIG
		//M4_BGMode
		//LCD_BL_Day
		//LCD_BL_Night
		//SIUS_SEL_Flag
		XO_Save_Config(M7_UserIndex,true,0);
#else
		HT_Save_Config(1);
#endif
	}
		
	switch(tZoomBTN)
	{
		case 5:
		case 6:
			if(ituWidgetIsVisible(M4_11_ZoomBTN) == false)
			{
				M4_SubMenuClear(MenuLCD_Choice_Index);
				
				MenuLCD_Choice_Index = mLCD_BGMode;
    
				ituWidgetSetVisible(M4_11_BTN,false);
				ituWidgetSetVisible(M4_11_ANI,true);
    
				// M4_11 scale up
				M4_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M4_11_ANI, 1);
			}
		break;
		
		case 7:
		case 8:
			if(ituWidgetIsVisible(M4_12_ZoomBTN) == false)
			{
				M4_SubMenuClear(MenuLCD_Choice_Index);
				
				MenuLCD_Choice_Index = mLCD_UnitMode;
      
				ituWidgetSetVisible(M4_12_BTN,false);
				ituWidgetSetVisible(M4_12_ANI,true);
      
				// M4_12 scale up
				M4_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M4_12_ANI, 1);
			}
		break;
	}
	
    return true;
}

bool M4_BTN_MouseDn(ITUWidget* widget, char* param)
{

	uint8_t tM4_SubMenu = 0;
	
 	tM4_SubMenu = (uint8_t)atoi(param);
	printf("M4_BTN_MouseDn : [%d][%d]\r\n",tM4_SubMenu,M4_BlinkFlag);
	M4_ExitCNT = 0;

	M4_1SecCNT = 100;
	M4_FastFlag = 0;
	M4_FastSource = 0;
	
//	M4D7_MouseUp(NULL,NULL);

	switch(tM4_SubMenu)
	{
		case 5:
			if(ituWidgetIsVisible(M4_11_ZoomBTN) == false)
			{
				if (MenuLCD_Choice_Index != mLCD_BGMode)
					M4_SubMenuClear(MenuLCD_Choice_Index);
				
				MenuLCD_Choice_Index = mLCD_BGMode;
    
				ituWidgetSetVisible(M4_11_BTN,false);
				ituWidgetSetVisible(M4_11_ANI,true);
    
				// M4_11 scale up
				M4_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M4_11_ANI, 0);	// 2024-05-07 modi whlim, add animation
			}
		break;
		case 6:
			if(ituWidgetIsVisible(M4_11_ZoomBTN) == false)
			{
				if (MenuLCD_Choice_Index != mLCD_BGMode)
					M4_SubMenuClear(MenuLCD_Choice_Index);
				
				MenuLCD_Choice_Index = mLCD_BGMode;
    
				ituWidgetSetVisible(M4_11_BTN,false);
				ituWidgetSetVisible(M4_11_ANI,true);
    
				// M4_11 scale up
				M4_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M4_11_ANI, 0); // 2024-05-07 modi whlim, add animation
			}
		break;
		case 7:
			if(ituWidgetIsVisible(M4_12_ZoomBTN) == false)
			{
				if (MenuLCD_Choice_Index != mLCD_UnitMode)
					M4_SubMenuClear(MenuLCD_Choice_Index);
				
				MenuLCD_Choice_Index = mLCD_UnitMode;
      
				ituWidgetSetVisible(M4_12_BTN,false);
				ituWidgetSetVisible(M4_12_ANI,true);
      
				// M4_12 scale up
				M4_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M4_12_ANI, 0); // 2024-05-07 modi whlim, add animation
			}
		break;
		case 8:
			if(ituWidgetIsVisible(M4_12_ZoomBTN) == false)
			{
				if (MenuLCD_Choice_Index != mLCD_UnitMode)
					M4_SubMenuClear(MenuLCD_Choice_Index);
				
				MenuLCD_Choice_Index = mLCD_UnitMode;
      
				ituWidgetSetVisible(M4_12_BTN,false);
				ituWidgetSetVisible(M4_12_ANI,true);
      
				// M4_12 scale up
				M4_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M4_12_ANI, 0); // 2024-05-07 modi whlim, add animation
			}
		break;

	}
    return true;
}

bool M4_BTN_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tM4_SubMenu = 0;
	char ptBrightValue[4];
 	tM4_SubMenu = (uint8_t)atoi(param);
	
	M4_ExitCNT = 0;
	printf("M4_BTN_MouseUp : [%d][%d]\r\n",tM4_SubMenu,M4_BlinkFlag);
	
	
	if(M4_BlinkFlag != 0)
	{
		M4_BlinkFlag = 0;		

		printf("MenuLCD_Choice_Index = %d\r\n",MenuLCD_Choice_Index);

		//if(MenuLCD_Choice_Index == mLCD_DayBright)
		{
			LCD_BL_Day = M4_DayBright;

			if(Main_DayNight==0)
				LCDLayer_BackLight(M4_DayBright);
				
			memset(ptBrightValue,0,4);
			sprintf(ptBrightValue,"%3d",M4_DayBright);
			ituTextSetString(M4_Day_Value, ptBrightValue);
			ituWidgetSetVisible(M4_Day_Value, true);
		}
		//else if(MenuLCD_Choice_Index == mLCD_NightBright)
		{
			LCD_BL_Night = M4_NightBright;

			if(Main_DayNight==1)
				LCDLayer_BackLight(M4_NightBright);
				
			memset(ptBrightValue,0,4);
			sprintf(ptBrightValue,"%3d",M4_NightBright);
			ituTextSetString(M4_Night_Value, ptBrightValue);
			ituWidgetSetVisible(M4_Night_Value, true);
		}
	}
	
	switch(tM4_SubMenu)
	{
		case 5:
			M4_BGModeClear();

			M4_BGMode = BG_BLACK;			//BG_BLUE
			
			ituWidgetSetVisible(M4_5_BTN,false);
			ituWidgetSetVisible(M4_5_ANI,true);

			// M4_5 scale up
			M4_5_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_5_ANI, 0);
			
		break;

		case 6:
			M4_BGModeClear();

			M4_BGMode = BG_BLUE;			//BG_BLACK
			
			ituWidgetSetVisible(M4_6_BTN,false);
			ituWidgetSetVisible(M4_6_ANI,true);

			// M4_6 scale up
			M4_6_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_6_ANI, 0);

		break;
		
		case 7:
			M4_UnitClear();

			M4_UnitFlag = 0;	//SIUS_SEL_Flag;
			
			ituWidgetSetVisible(M4_7_BTN,false);
			ituWidgetSetVisible(M4_7_ANI,true);

			// M4_7 scale up
			M4_7_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_7_ANI, 0);
			
		break;

		case 8:
			M4_UnitClear();

			M4_UnitFlag = 1;	//SIUS_SEL_Flag;
			
			ituWidgetSetVisible(M4_8_BTN,false);
			ituWidgetSetVisible(M4_8_ANI,true);

			// M4_8 scale up
			M4_8_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_8_ANI, 0); 
			
		break;
		
		case 9:

			if (MenuLCD_Choice_Index != mLCD_DayBright)
				M4_SubMenuClear(MenuLCD_Choice_Index);
			
			MenuLCD_Choice_Index = mLCD_DayBright;

			ituWidgetSetVisible(M4_9_BTN,false);
			ituWidgetSetVisible(M4_9_ANI,true);

			// M4_9 scale up
			M4_9_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_9_ANI, 0); // 2024-05-07 modi whlim, add animation

		break;

		case 10:

			if (MenuLCD_Choice_Index != mLCD_NightBright)
				M4_SubMenuClear(MenuLCD_Choice_Index);
			
			MenuLCD_Choice_Index = mLCD_NightBright;

			ituWidgetSetVisible(M4_10_BTN,false);
			ituWidgetSetVisible(M4_10_ANI,true);

			// M4_10 scale up
			M4_10_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_10_ANI, 0); // 2024-05-07 modi whlim, add animation

		break;

		case 11:

			if (MenuLCD_Choice_Index != mLCD_BGMode)
				M4_SubMenuClear(MenuLCD_Choice_Index);
			
			MenuLCD_Choice_Index = mLCD_BGMode;

			ituWidgetSetVisible(M4_11_BTN,false);
			ituWidgetSetVisible(M4_11_ANI,true);

			// M4_11 scale up
			M4_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_11_ANI, 0); // 2024-05-07 modi whlim, add animation

		break;

		case 12:
			if (MenuLCD_Choice_Index != mLCD_UnitMode)
				M4_SubMenuClear(MenuLCD_Choice_Index);
			
			MenuLCD_Choice_Index = mLCD_UnitMode;

			ituWidgetSetVisible(M4_12_BTN,false);
			ituWidgetSetVisible(M4_12_ANI,true);

			// M4_12 scale up
			M4_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_12_ANI, 0); // 2024-05-07 modi whlim, add animation

		break;
	}
	
		
	switch(tM4_SubMenu)
	{
		case 5:
		case 6:
			BG_Color = M4_BGMode;

#ifdef USED_SAVE_CONFIG
			//M4_BGMode
			//LCD_BL_Day
			//LCD_BL_Night
			//SIUS_SEL_Flag
			XO_Save_Config(M7_UserIndex,true,0);
#else
			HT_Save_Config(1);
#endif
		
			printf("\r\n BG Color SET [%d]\r\n",BG_Color);
		break;

		case 7:
		case 8:
			SIUS_SEL_Flag = M4_UnitFlag;	//SIUS_SEL_Flag;

#ifdef USED_SAVE_CONFIG
			//M4_BGMode
			//LCD_BL_Day
			//LCD_BL_Night
			//SIUS_SEL_Flag
			XO_Save_Config(M7_UserIndex,true,0);
#else
			HT_Save_Config(1);
#endif

			printf("\r\n Unit Flag SET [%d]\r\n",SIUS_SEL_Flag);
		break;
	}
	
    return true;
}

bool M4_12_ANIStopped(ITUWidget* widget, char* param)
{
	printf("M4_12_ANIStopped [%d]\r\n",M4_12_Scaleflag);
	if(M4_12_Scaleflag == 1)
	{
		M4_12_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_12_ANI, false);
		ituWidgetSetVisible(M4_12_ZoomBTN, true);
	}
	else if(M4_12_Scaleflag == 2)
	{
		M4_12_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_12_ANI, false);
		ituWidgetSetVisible(M4_12_BTN, true);
	}
	
    return true;
}

bool M4_11_ANIStopped(ITUWidget* widget, char* param)
{
	printf("M4_11_ANIStopped [%d]\r\n",M4_11_Scaleflag);
	if(M4_11_Scaleflag == 1)
	{
		M4_11_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_11_ANI, false);
		ituWidgetSetVisible(M4_11_ZoomBTN, true);
	}
	else if(M4_11_Scaleflag == 2)
	{
		M4_11_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_11_ANI, false);
		ituWidgetSetVisible(M4_11_BTN, true);
	}

    return true;
}

bool M4_10_ANIStopped(ITUWidget* widget, char* param)
{
	printf("M4_10_ANIStopped [%d]\r\n",M4_10_Scaleflag);
	if(M4_10_Scaleflag == 1)
	{
		M4_10_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_10_ANI, false);
		ituWidgetSetVisible(M4_10_ZoomBTN, true);
	}
	else if(M4_10_Scaleflag == 2)
	{
		M4_10_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_10_ANI, false);
		ituWidgetSetVisible(M4_10_BTN, true);
	}

    return true;
}

bool M4_9_ANIStopped(ITUWidget* widget, char* param)
{
	printf("M4_9_ANIStopped [%d]\r\n",M4_9_Scaleflag);
	if(M4_9_Scaleflag == 1)
	{
		M4_9_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_9_ANI, false);
		ituWidgetSetVisible(M4_9_ZoomBTN, true);
	}
	else if(M4_9_Scaleflag == 2)
	{
		M4_9_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_9_ANI, false);
		ituWidgetSetVisible(M4_9_BTN, true);
	}

    return true;
}

bool M4_8_ANIStopped(ITUWidget* widget, char* param)
{

	printf("M4_8_ANIStopped [%d]\r\n",M4_9_Scaleflag);
	if(M4_8_Scaleflag == 1)
	{
		M4_8_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_8_ANI, false);
		ituWidgetSetVisible(M4_8_ZoomBTN, true);
	}
	else if(M4_8_Scaleflag == 2)
	{
		M4_8_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_8_ANI, false);
		ituWidgetSetVisible(M4_8_BTN, true);
	}

    return true;
}

bool M4_7_ANIStopped(ITUWidget* widget, char* param)
{

	if(M4_7_Scaleflag == 1)
	{
		M4_7_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_7_ANI, false);
		ituWidgetSetVisible(M4_7_ZoomBTN, true);
	}
	else if(M4_7_Scaleflag == 2)
	{
		M4_7_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_7_ANI, false);
		ituWidgetSetVisible(M4_7_BTN, true);
	}

    return true;
}

bool M4_6_ANIStopped(ITUWidget* widget, char* param)
{


	if(M4_6_Scaleflag == 1)
	{
		M4_6_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_6_ANI, false);
		ituWidgetSetVisible(M4_6_ZoomBTN, true);
	}
	else if(M4_6_Scaleflag == 2)
	{
		M4_6_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_6_ANI, false);
		ituWidgetSetVisible(M4_6_BTN, true);
	}

    return true;
}

bool M4_5_ANIStopped(ITUWidget* widget, char* param)
{

	if(M4_5_Scaleflag == 1)
	{
		M4_5_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_5_ANI, false);
		ituWidgetSetVisible(M4_5_ZoomBTN, true);
	}
	else if(M4_5_Scaleflag == 2)
	{
		M4_5_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M4_5_ANI, false);
		ituWidgetSetVisible(M4_5_BTN, true);
	}

    return true;
}

bool M4_Up_MouseDn(ITUWidget* widget, char* param)
{
	char ptBrightValue[4];
	uint8_t tUpBTN = 0;

	tUpBTN = (uint8_t)atoi(param);

	M4_ExitCNT = 0;
	
	switch(tUpBTN)
	{
		case 2:				// day

			if(MenuLCD_Choice_Index == mLCD_NightBright)
			{
				if(M4_BlinkFlag != 0)
				{
					M4_BlinkFlag = 0;		

					LCD_BL_Night = M4_NightBright;

					if(Main_DayNight==1)
						LCDLayer_BackLight(M4_NightBright);

#ifdef USED_SAVE_CONFIG
					//M4_BGMode
					//LCD_BL_Day
					//LCD_BL_Night
					//SIUS_SEL_Flag
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
				
					memset(ptBrightValue,0,4);
					sprintf(ptBrightValue,"%3d",M4_NightBright);
					ituTextSetString(M4_Night_Value, ptBrightValue);
					ituWidgetSetVisible(M4_Night_Value, true);

					if(M4_FastFlag == 2)
					{
						M4_1SecCNT = 100;
						M4_FastFlag = 0;
						M4_FastSource = 0;

						//M4_SecTimer = gMainTimerCnt_10m;	
					}
				}
			}

			if(ituWidgetIsVisible(M4_9_ZoomBTN) == false)
			{
				M4_SubMenuClear(MenuLCD_Choice_Index);
			
				MenuLCD_Choice_Index = mLCD_DayBright;

				ituWidgetSetVisible(M4_9_BTN,false);
				ituWidgetSetVisible(M4_9_ANI,true);

				// M4_9 scale up
				M4_9_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M4_9_ANI, 1);
				//M4_SkipMenuTop = 1;
			}
			else
			{
				if((M4_BlinkFlag == 1)||(M4_BlinkFlag == 2))
				{ 
					MenuLCD_Choice_Index = mLCD_DayBright;

					// day value long key check start
					M4_SecTimer = gMainTimerCnt_10m;
					M4_1SecCNT = 0;
					M4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					M4_FastSource = 2;
				}
			}
			
			ituWidgetSetVisible(M4_Day_Value, true);
			ituWidgetSetVisible(M4_Night_Value, true);
		break;
		
		case 4:			// night

			if(MenuLCD_Choice_Index == mLCD_DayBright)
			{
				if(M4_BlinkFlag != 0)
				{
					M4_BlinkFlag = 0;		

					LCD_BL_Day = M4_DayBright;

					if(Main_DayNight==0)
						LCDLayer_BackLight(M4_DayBright);

#ifdef USED_SAVE_CONFIG
					//M4_BGMode
					//LCD_BL_Day
					//LCD_BL_Night
					//SIUS_SEL_Flag
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
				
					memset(ptBrightValue,0,4);
					sprintf(ptBrightValue,"%3d",M4_DayBright);
					ituTextSetString(M4_Day_Value, ptBrightValue);
					ituWidgetSetVisible(M4_Day_Value, true);

					if(M4_FastFlag == 2)
					{
						M4_1SecCNT = 100;
						M4_FastFlag = 0;
						M4_FastSource = 0;

						//M4_SecTimer = gMainTimerCnt_10m;	
					}
				}
			}

			if(ituWidgetIsVisible(M4_10_ZoomBTN) == false)
			{
				M4_SubMenuClear(MenuLCD_Choice_Index);
			
				MenuLCD_Choice_Index = mLCD_NightBright;

				ituWidgetSetVisible(M4_10_BTN,false);
				ituWidgetSetVisible(M4_10_ANI,true);

				// M4_10 scale up
				M4_10_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M4_10_ANI, 1);
				//M4_SkipMenuTop = 1;
			}
			else
			{
				if((M4_BlinkFlag == 1)||(M4_BlinkFlag == 2))
				{ 
					MenuLCD_Choice_Index = mLCD_NightBright;

					// day value long key check start
					M4_SecTimer = gMainTimerCnt_10m;
					M4_1SecCNT = 0;
					M4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					M4_FastSource = 4;
				}
			}

			ituWidgetSetVisible(M4_Day_Value, true);
			ituWidgetSetVisible(M4_Night_Value, true);
		break;
	}

    return true;
}

bool M4_Up_MouseUp(ITUWidget* widget, char* param)
{
	char ptBrightValue[4];
	uint8_t tUpBTN = 0;
	
	tUpBTN = (uint8_t)atoi(param);

	M4_ExitCNT = 0;
	printf("M4_Up_MouseUp=[0x%X]\n",tUpBTN);
	switch(tUpBTN)
	{
		case 2:
			// Day Value Blink start
			if(M4_BlinkFlag == 0)
			{
				M4_BlinkTimer = gMainTimerCnt_10m;
				M4_BlinkFlag = 1;
			}
			else if((M4_BlinkFlag == 1)||(M4_BlinkFlag == 2))
			{
				//if(M4_FastFlag != 2)
				{
					if((M4_DayBright < 100)&&(M4_SkipMenuTop == 0))		//LCD_BL_Day
						M4_DayBright++;
					else if(M4_SkipMenuTop == 1)
						M4_SkipMenuTop = 0;
					
					memset(ptBrightValue,0,4);
					sprintf(ptBrightValue,"%3d",M4_DayBright);
					ituTextSetString(M4_Day_Value, ptBrightValue);

					if(Main_DayNight == 0)		// Day Mode
						LCDLayer_BackLight(M4_DayBright);
				}
			}

			// No scale up > day value down , long key check reset
			M4_TouchUP_Restet();
		break;
		
		case 4:
			// Night Value Blink start
			if(M4_BlinkFlag == 0)
			{
				M4_BlinkTimer = gMainTimerCnt_10m;
				M4_BlinkFlag = 1;
			}
			else if((M4_BlinkFlag == 1)||(M4_BlinkFlag == 2))
			{
				//if(M4_FastFlag != 2)
				{
					if((M4_NightBright < 100)&&(M4_SkipMenuTop == 0))		//LCD_BL_Night
						M4_NightBright++;
					else if(M4_SkipMenuTop == 1)
						M4_SkipMenuTop = 0;

					memset(ptBrightValue,0,4);
					sprintf(ptBrightValue,"%3d",M4_NightBright);
					ituTextSetString(M4_Night_Value, ptBrightValue);

					if(Main_DayNight == 1)			// 1:Night Mode
						LCDLayer_BackLight(M4_NightBright);
				}
			}
			
			// No scale up > day value down , long key check reset
			M4_TouchUP_Restet();
		break;
	}

    return true;
}

bool M4_Dn_MouseDn(ITUWidget* widget, char* param)
{
	char ptBrightValue[4];
	uint8_t tDownBTN = 0;


	
	tDownBTN = (uint8_t)atoi(param);

	M4_ExitCNT = 0;
		
	switch(tDownBTN)
	{
		case 1:

			if(MenuLCD_Choice_Index == mLCD_NightBright)
			{
				if(M4_BlinkFlag != 0)
				{
					M4_BlinkFlag = 0;		

					LCD_BL_Night = M4_NightBright;

					if(Main_DayNight==1)
						LCDLayer_BackLight(M4_NightBright);

#ifdef USED_SAVE_CONFIG
					//M4_BGMode
					//LCD_BL_Day
					//LCD_BL_Night
					//SIUS_SEL_Flag
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif

					memset(ptBrightValue,0,4);
					sprintf(ptBrightValue,"%3d",M4_NightBright);
					ituTextSetString(M4_Night_Value, ptBrightValue);
					ituWidgetSetVisible(M4_Night_Value, true);
					
					if(M4_FastFlag == 2)
					{
						M4_1SecCNT = 100;
						M4_FastFlag = 0;
						M4_FastSource = 0;

						//M4_SecTimer = gMainTimerCnt_10m;	
					}
				}
			}

			if(ituWidgetIsVisible(M4_9_ZoomBTN) == false)
			{
				M4_SubMenuClear(MenuLCD_Choice_Index);
			
				MenuLCD_Choice_Index = mLCD_DayBright;

				ituWidgetSetVisible(M4_9_BTN,false);
				ituWidgetSetVisible(M4_9_ANI,true);

				// M4_9 scale up
				M4_9_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M4_9_ANI, 1);
				//M4_SkipMenuTop = 1;
			}
			else// if(ituWidgetIsVisible(M4_9_ZoomBTN) == true)
			{
				if((M4_BlinkFlag == 1)||(M4_BlinkFlag == 2))
				{ 
					MenuLCD_Choice_Index = mLCD_DayBright;

					// day value long key check start
					M4_SecTimer = gMainTimerCnt_10m;
					M4_1SecCNT = 0;
					M4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					M4_FastSource = 1;
				
					//test4_timer = gMainTimerCnt_10m;
				}
			}
			
			ituWidgetSetVisible(M4_Day_Value, true);
			ituWidgetSetVisible(M4_Night_Value, true);
		break;
		
		case 3:

			if(MenuLCD_Choice_Index == mLCD_DayBright)
			{
				if(M4_BlinkFlag != 0)
				{
					M4_BlinkFlag = 0;		

					LCD_BL_Day = M4_DayBright;

					if(Main_DayNight==0)
						LCDLayer_BackLight(M4_DayBright);

#ifdef USED_SAVE_CONFIG
					//M4_BGMode
					//LCD_BL_Day
					//LCD_BL_Night
					//SIUS_SEL_Flag
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
				
					memset(ptBrightValue,0,4);
					sprintf(ptBrightValue,"%3d",M4_DayBright);
					ituTextSetString(M4_Day_Value, ptBrightValue);
					ituWidgetSetVisible(M4_Day_Value, true);

					if(M4_FastFlag == 2)
					{
						M4_1SecCNT = 100;
						M4_FastFlag = 0;
						M4_FastSource = 0;

						//M4_SecTimer = gMainTimerCnt_10m;	
					}
				}
			}

			if(ituWidgetIsVisible(M4_10_ZoomBTN) == false)
			{
				M4_SubMenuClear(MenuLCD_Choice_Index);
			
				MenuLCD_Choice_Index = mLCD_NightBright;

				ituWidgetSetVisible(M4_10_BTN,false);
				ituWidgetSetVisible(M4_10_ANI,true);

				// M4_10 scale up
				M4_10_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M4_10_ANI, 1);
				//M4_SkipMenuTop = 1;
			}
			else// if(ituWidgetIsVisible(M4_10_ZoomBTN) == true)
			{
				if((M4_BlinkFlag == 1)||(M4_BlinkFlag == 2))
				{ 
					MenuLCD_Choice_Index = mLCD_NightBright;

					// day value long key check start
					M4_SecTimer = gMainTimerCnt_10m;
					M4_1SecCNT = 0;
					M4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					M4_FastSource = 3;
				
					//test4_timer = gMainTimerCnt_10m;
				}
			}

			ituWidgetSetVisible(M4_Day_Value, true);
			ituWidgetSetVisible(M4_Night_Value, true);
		break;
	}
	
    return true;
}

bool M4_Dn_MouseUp(ITUWidget* widget, char* param)
{
	char ptBrightValue[4];
	uint8_t tDownBTN = 0;
	
	tDownBTN = (uint8_t)atoi(param);

	M4_ExitCNT = 0;
	printf("M4_Dn_MouseUp=[0x%X]\n",tDownBTN);
	switch(tDownBTN)
	{
		case 1:
			// Day Value Blink start
			if(M4_BlinkFlag == 0)
			{
				M4_BlinkTimer = gMainTimerCnt_10m;
				M4_BlinkFlag = 1;
			}
			else if((M4_BlinkFlag == 1)||(M4_BlinkFlag == 2))
			{
				//if(M4_FastFlag != 2)
				{
					if((M4_DayBright > 1)&&(M4_SkipMenuTop == 0))		//LCD_BL_Day
						M4_DayBright --;
					else if(M4_SkipMenuTop == 1)
						M4_SkipMenuTop = 0;
					
					memset(ptBrightValue,0,4);
					sprintf(ptBrightValue,"%3d",M4_DayBright);
					ituTextSetString(M4_Day_Value, ptBrightValue);

					if(Main_DayNight == 0)			// Day Mode
						LCDLayer_BackLight(M4_DayBright);
				}
			}
			
			// No scale up > day value down , long key check reset
			M4_TouchUP_Restet();
		break;
		
		case 3:
			// Night Value Blink start
			if(M4_BlinkFlag == 0)
			{
				M4_BlinkTimer = gMainTimerCnt_10m;
				M4_BlinkFlag = 1;
			}
			else if((M4_BlinkFlag == 1)||(M4_BlinkFlag == 2))
			{
				//if(M4_FastFlag != 2)
				{
					if((M4_NightBright > 1)&&(M4_SkipMenuTop == 0))		//LCD_BL_Night
						M4_NightBright --;
					else if(M4_SkipMenuTop == 1)
						M4_SkipMenuTop = 0;

					memset(ptBrightValue,0,4);
					sprintf(ptBrightValue,"%3d",M4_NightBright);
					ituTextSetString(M4_Night_Value, ptBrightValue);

					if(Main_DayNight == 1)		// 1:Night Mode
						LCDLayer_BackLight(M4_NightBright);
				}
			}
			
			// No scale up > day value down , long key check reset
			M4_TouchUP_Restet();
		break;
	}

    return true;
}

bool M4D7_MouseDn(ITUWidget* widget, char* param)
{

	M4_ExitCNT = 0;
	//M4_EditMenu = 0;
	
    return true;
}

bool M4D7_MouseUp(ITUWidget* widget, char* param)
{

	char ptBrightValue[4];

	M4_ExitCNT = 0;
	
	if(M4_BlinkFlag != 0)
	{
		M4_BlinkFlag = 0;		

		if(MenuLCD_Choice_Index == mLCD_DayBright)
		{
			LCD_BL_Day = M4_DayBright;

			if (Main_DayNight==0)
				LCDLayer_BackLight(M4_DayBright);
				
			memset(ptBrightValue,0,4);
			sprintf(ptBrightValue,"%3d",M4_DayBright);
			ituTextSetString(M4_Day_Value, ptBrightValue);
			ituWidgetSetVisible(M4_Day_Value, true);
		}
		else if(MenuLCD_Choice_Index == mLCD_NightBright)
		{
			LCD_BL_Night = M4_NightBright;

			if (Main_DayNight==1)
				LCDLayer_BackLight(M4_NightBright);
				
			memset(ptBrightValue,0,4);
			sprintf(ptBrightValue,"%3d",M4_NightBright);
			ituTextSetString(M4_Night_Value, ptBrightValue);
			ituWidgetSetVisible(M4_Night_Value, true);
		}
		
		//M4_SkipMenuTop = 1;
	}
	else if(MenuLCD_Choice_Index == mLCD_BGMode)
	{
		// BG toggle
		if(M4_BGMode == BG_BLACK)
		{
			M4_BGModeClear();

			M4_BGMode = BG_BLUE;			//BG_BLACK
			
			ituWidgetSetVisible(M4_6_BTN,false);
			ituWidgetSetVisible(M4_6_ANI,true);

			// M4_6 scale up
			M4_6_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_6_ANI, 0);
		}
		else if(M4_BGMode == BG_BLUE)
		{
			M4_BGModeClear();

			M4_BGMode = BG_BLACK;			//BG_BLUE
			
			ituWidgetSetVisible(M4_5_BTN,false);
			ituWidgetSetVisible(M4_5_ANI,true);

			// M4_5 scale up
			M4_5_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_5_ANI, 0);
		}

		BG_Color = M4_BGMode;
		printf("\r\n BG Color SET [%d]\r\n",BG_Color);
	}
	else if(MenuLCD_Choice_Index == mLCD_UnitMode)
	{
		// Unit toggle
		if(M4_UnitFlag == 1)
		{
			M4_UnitClear();

			M4_UnitFlag = 0;	//SIUS_SEL_Flag;
			
			ituWidgetSetVisible(M4_7_BTN,false);
			ituWidgetSetVisible(M4_7_ANI,true);

			// M4_7 scale up
			M4_7_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_7_ANI, 0);
		}
		else if(M4_UnitFlag == 0)
		{
			M4_UnitClear();

			M4_UnitFlag = 1;	//SIUS_SEL_Flag;
			
			ituWidgetSetVisible(M4_8_BTN,false);
			ituWidgetSetVisible(M4_8_ANI,true);

			// M4_8 scale up
			M4_8_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M4_8_ANI, 0);
		}

		SIUS_SEL_Flag = M4_UnitFlag;	//SIUS_SEL_Flag;
		printf("\r\n Unit Flag SET [%d]\r\n",SIUS_SEL_Flag);
	}

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
	BG_Color = M4_BGMode;

	//BG_Color
	//LCD_BL_Day
	//LCD_BL_Night
	//SIUS_SEL_Flag
	XO_Save_Config(M7_UserIndex,true,0);
#else
	HT_Save_Config(1);
#endif

    return true;
}

bool M4D6_MouseUp(ITUWidget* widget, char* param)
{
/*	
#ifdef USED_SAVE_CONFIG

	//M4_BGMode
	//LCD_BL_Day
	//LCD_BL_Night
	//SIUS_SEL_Flag
	XO_Save_Config(M7_UserIndex,true,0);
#else
	HT_Save_Config(1);
#endif
*/
	GotoMain(MAIN_MODE_NORMAL); 

    return true;
}

bool M4D5_MouseUp(ITUWidget* widget, char* param)
{
	M4_ExitCNT = 0;

	if(M4_SkipMenuTop == 1)
	{
		M4_SkipMenuTop = 0;
	}
	else
	{
/*
#ifdef USED_SAVE_CONFIG

		//M4_BGMode
		//LCD_BL_Day
		//LCD_BL_Night
		//SIUS_SEL_Flag
		XO_Save_Config(M7_UserIndex,true,0);

#else
		HT_Save_Config(1);
#endif
*/
		GotoMenuTop();
	}
	
    return true;
}

bool M4D5_MouseDn(ITUWidget* widget, char* param)
{
	char ptBrightValue[4];
	
	M4_ExitCNT = 0;
	
	// blink value resume
	if(M4_BlinkFlag != 0)
	{
		M4_BlinkFlag = 0;		

		if(MenuLCD_Choice_Index == mLCD_DayBright)
		{
			M4_DayBright = LCD_BL_Day;

			if (Main_DayNight==0)
				LCDLayer_BackLight(M4_DayBright);
				
			memset(ptBrightValue,0,4);
			sprintf(ptBrightValue,"%3d",M4_DayBright);
			ituTextSetString(M4_Day_Value, ptBrightValue);
			ituWidgetSetVisible(M4_Day_Value, true);
		}
		else if(MenuLCD_Choice_Index == mLCD_NightBright)
		{
			M4_NightBright = LCD_BL_Night;

			if (Main_DayNight==1)
				LCDLayer_BackLight(M4_NightBright);
				
			memset(ptBrightValue,0,4);
			sprintf(ptBrightValue,"%3d",M4_NightBright);
			ituTextSetString(M4_Night_Value, ptBrightValue);
			ituWidgetSetVisible(M4_Night_Value, true);
		}
		
		//M4_EditMenu = 0;
		M4_SkipMenuTop = 1;
	}

    return true;
}

bool M4_UserDisplay(void)
{
	uint8_t tDestName[100];


	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
		return 0;

	if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
	{
		ituWidgetSetVisible(M4_TopStandard,false);
		ituWidgetSetVisible(M4_TopAdmin,true);
	}
	else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
	{
		ituWidgetSetVisible(M4_TopAdmin,false);
		ituWidgetSetVisible(M4_TopStandard,true);
	}
	else
	{
		ituWidgetSetVisible(M4_TopAdmin,false);
		ituWidgetSetVisible(M4_TopStandard,false);
	}

	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M4_TopUserName, tDestName);

		ituWidgetSetVisible(M4_TopUserName,true);
	}
	else
	{
		ituWidgetSetVisible(M4_TopUserName,false);
	}
/*
	if((gMainTimerCnt_10m - gMenu_UI_Time) < 500)		// 5sec	
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M4_TopUserName, tDestName);

		ituWidgetSetVisible(M4_TopUserName,true);
	}
	else if(((gMainTimerCnt_10m - gMenu_UI_Time) >= 500)||(M7_UserIndex > (MAX_SU100_USER_NUM-1)))
	{
		ituWidgetSetVisible(M4_TopUserName,false);
	}
*/
}

void M4_SubMenuClear(uint8_t MenuIndex)
{
	printf("M4_SubMenuClear [%d]\r\n",MenuIndex);
	switch(MenuIndex)
	{
		case mLCD_DayBright:
		{
//			if(ituWidgetIsVisible(M4_9_ZoomBTN) == true)
			{	
				ituWidgetSetVisible(M4_9_ZoomBTN,false);
				ituWidgetSetVisible(M4_9_ANI,true);
				
				// M4_9 scale down
				M4_9_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M4_9_ANI, 1);
			}
		}
		break;
		case mLCD_NightBright:
		{
//			if(ituWidgetIsVisible(M4_10_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M4_10_ZoomBTN,false);
				ituWidgetSetVisible(M4_10_ANI,true);
				
				// M4_10 scale down
				M4_10_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M4_10_ANI, 1);
			}
		}
		break;
		case mLCD_BGMode:
		{
//			if(ituWidgetIsVisible(M4_11_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M4_11_ZoomBTN,false);
				ituWidgetSetVisible(M4_11_ANI,true);
				
				// M4_11 scale down
				M4_11_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M4_11_ANI, 1);
			}
		}
		break;
		case mLCD_UnitMode:
		{
//			if(ituWidgetIsVisible(M4_12_ZoomBTN) == true)
			{
				ituWidgetSetVisible(M4_12_ZoomBTN,false);
				ituWidgetSetVisible(M4_12_ANI,true);
				
				// M4_12 scale down
				M4_12_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(M4_12_ANI, 1);
			}
		}
		break;
	}
}

void M4_BGModeClear(void)
{
	if(ituWidgetIsVisible(M4_5_ZoomBTN) == true || ituWidgetIsVisible(M4_5_ANI) == true)  // 2024-05-02 modi whlim, add (|| ituWidgetIsVisible(M4_5_ANI) == true)
	{
		ituWidgetSetVisible(M4_5_ZoomBTN,false);
		ituWidgetSetVisible(M4_5_ANI,true);
		
		// M4_5 scale down
		M4_5_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(M4_5_ANI, 1);
	}	

	if(ituWidgetIsVisible(M4_6_ZoomBTN) == true || ituWidgetIsVisible(M4_6_ANI) == true)
	{
		ituWidgetSetVisible(M4_6_ZoomBTN,false);
		ituWidgetSetVisible(M4_6_ANI,true);
		
		// M4_6 scale down
		M4_6_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(M4_6_ANI, 1);
	}	
}

void M4_UnitClear(void)
{
	if(ituWidgetIsVisible(M4_7_ZoomBTN) == true || ituWidgetIsVisible(M4_7_ANI) == true) // 2024-05-02 modi whlim, add (|| ituWidgetIsVisible(M4_7_ANI) == true)
	{
		ituWidgetSetVisible(M4_7_ZoomBTN,false);
		ituWidgetSetVisible(M4_7_ANI,true);
		
		// M4_7 scale down
		M4_7_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(M4_7_ANI, 1);
	}	

	if(ituWidgetIsVisible(M4_8_ZoomBTN) == true || ituWidgetIsVisible(M4_8_ANI) == true)
	{
		ituWidgetSetVisible(M4_8_ZoomBTN,false);
		ituWidgetSetVisible(M4_8_ANI,true);
		
		// M4_8 scale down
		M4_8_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(M4_8_ANI, 1);
	}	
}

bool M4_AniMenuStatus(void)
{
	return 0;
	
	if(ituWidgetIsVisible(M4_9_ZoomBTN) == true)
	{
		if(MenuLCD_Choice_Index != 1)
		{
			M4_9_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_9_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M4_9_ANI) == true)
	{
		if(MenuLCD_Choice_Index != 1)
		{
			M4_9_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_9_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M4_9_Up1) == true)
	{
		if(MenuLCD_Choice_Index != 1)
		{
			M4_9_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_9_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M4_9_Up0) == true)
	{
		if(MenuLCD_Choice_Index != 1)
		{
			M4_9_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_9_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(M4_10_ZoomBTN) == true)
	{
		if(MenuLCD_Choice_Index != 2)
		{
			M4_10_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_10_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M4_10_ANI) == true)
	{
		if(MenuLCD_Choice_Index != 2)
		{
			M4_10_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_10_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M4_10_Up1) == true)
	{
		if(MenuLCD_Choice_Index != 2)
		{
			M4_10_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_10_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M4_10_Up0) == true)
	{
		if(MenuLCD_Choice_Index != 2)
		{
			M4_10_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_10_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(M4_11_ZoomBTN) == true)
	{
		if(MenuLCD_Choice_Index != 3)
		{
			M4_11_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_11_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M4_11_ANI) == true)
	{
		if(MenuLCD_Choice_Index != 3)
		{
			M4_11_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_11_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M4_11_Up1) == true)
	{
		if(MenuLCD_Choice_Index != 3)
		{
			M4_11_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_11_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M4_11_Up0) == true)
	{
		if(MenuLCD_Choice_Index != 3)
		{
			M4_11_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_11_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(M4_12_ZoomBTN) == true)
	{
		if(MenuLCD_Choice_Index != 4)
		{
			M4_12_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_12_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M4_12_ANI) == true)
	{
		if(MenuLCD_Choice_Index != 4)
		{
			M4_12_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_12_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M4_12_Up1) == true)
	{
		if(MenuLCD_Choice_Index != 4)
		{
			M4_12_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_12_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M4_12_Up0) == true)
	{
		if(MenuLCD_Choice_Index != 4)
		{
			M4_12_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M4_12_ANIStopped(NULL,NULL);
		}
	}



}
