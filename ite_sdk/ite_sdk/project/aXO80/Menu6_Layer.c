#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

/* widgets:
Menu6_Layer
Menu_DateBG
M6_TopUserName
M6_TopStandard
M6_TopAdmin
M6_5_Value
M6_4_Value
M6_3b_Value
M6_2b_Value
M6_1b_Value
M6_3a_Value
M6_2a_Value
M6_1a_Value
M6_5_BTN
M6_5_SEL
M6_4_BTN
M6_4_SEL
M6_7_BTN
M6_6_BTN
M6_10_ZoomBTN
M6_10_ANI
M6_10_Up1
M6_10_Up0
M6_10_BTN
M6_9_ZoomBTN
M6_9_ANI
M6_9_Up1
M6_9_Up0
M6_9_BTN
M6_8_ZoomBTN
M6_8_ANI
M6_8_Up1
M6_8_Up0
M6_8_BTN
M6_1b_BTN
M6_1b_Sel
M6_SP2b
M6_3b_BTN
M6_3b_Sel
M6_SP1b
M6_2b_BTN
M6_2b_Sel
M6_3a_BTN
M6_3a_Sel
M6_SP2a
M6_2a_BTN
M6_2a_Sel
M6_SP1a
M6_1a_BTN
M6_1a_Sel
M6_12_ZoomBTN
M6_12_ANI
M6_12_Up1
M6_12_Up0
M6_12_BTN
M6_11_ZoomBTN
M6_11_ANI
M6_11_Up1
M6_11_Up0
M6_11_BTN
M6_D7_BTN
M6_D6_BTN
M6_D5_BTN
*/

static ITUText* M6_1a_Value;		// type 1 [YYYY/MM/DD] , YYYY
static ITUText* M6_2a_Value;		// type 1 [YYYY/MM/DD] , MM
static ITUText* M6_3a_Value;		// type 1 [YYYY/MM/DD] , DD

static ITUText* M6_1b_Value;		// type 2 [MM/DD/YYYY] , YYYY
static ITUText* M6_2b_Value;		// type 2 [MM/DD/YYYY] , MM or type 3 [DD/MM/YYYY] , DD
static ITUText* M6_3b_Value;		// type 2 [MM/DD/YYYY] , DD or type 3 [DD/MM/YYYY] , MM

static ITUText* M6_4_Value;			// Hour
static ITUText* M6_5_Value;			// Min

static uint8_t M6_8_Scaleflag = 0;		// type yyyymmdd 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M6_9_Scaleflag = 0;		// type mmddyyyy  0; None ,1;Scale up  ,2;Scale Down
static uint8_t M6_10_Scaleflag = 0;		// type ddmmyyyy  0; None ,1;Scale up  ,2;Scale Down

static uint8_t M6_11_Scaleflag = 0;		// mDate_ValueSet 0; None ,1;Scale up  ,2;Scale Down
static uint8_t M6_12_Scaleflag = 0;		// mDate_TypeSet  0; None ,1;Scale up  ,2;Scale Down


static ITUButton* M6_10_ZoomBTN;
static ITUAnimation* M6_10_ANI;
static ITUIcon* M6_10_Up1;
static ITUIcon* M6_10_Up0;
static ITUButton* M6_10_BTN;
static ITUButton* M6_9_ZoomBTN;
static ITUAnimation* M6_9_ANI;
static ITUIcon* M6_9_Up1;
static ITUIcon* M6_9_Up0;
static ITUButton* M6_9_BTN;
static ITUButton* M6_8_ZoomBTN;
static ITUAnimation* M6_8_ANI;
static ITUIcon* M6_8_Up1;
static ITUIcon* M6_8_Up0;
static ITUButton* M6_8_BTN;
static ITUButton* M6_12_ZoomBTN;
static ITUAnimation* M6_12_ANI;
static ITUIcon* M6_12_Up1;
static ITUIcon* M6_12_Up0;
static ITUButton* M6_12_BTN;
static ITUButton* M6_11_ZoomBTN;
static ITUAnimation* M6_11_ANI;
static ITUIcon* M6_11_Up1;
static ITUIcon* M6_11_Up0;
static ITUButton* M6_11_BTN;



#define YEAR_MAX	2099
#define YEAR_MIN	2022

uint16_t gYear  = 2023;				// system time sync
uint8_t  gMonth = 1;
uint8_t  gDay   = 1;
uint8_t  gHour  = 12;
uint8_t  gMin   = 0;
uint8_t  gSec   = 0;		// JPHong 2023-04-18
bool M6_Jog_enterFlag = false;		// 2024-05-22 add whlim

static uint16_t M6_Year = 2023;				// 
static uint8_t M6_Month = 1;
static uint8_t M6_Day = 1;
static uint8_t M6_Hour = 12;
static uint8_t M6_Min = 0;
static uint8_t M6_Select_Date = 0;

static uint32_t M6_SecTimer;
static uint8_t M6_1SecCNT;
static uint8_t M6_ExitCNT = 0;
static uint32_t M6_BlinkTimer;
static uint8_t M6_BlinkFlag = 0;
static uint32_t M6_FastTimer;
static uint8_t M6_FastFlag = 0;
static uint8_t M6_FastSource = 0;

static uint8_t M6_SkipFlag = 0;
static uint8_t M6_TypeChangeDir = 0;		// 0:down , 1:up

void M6_SubMenuClear(void);
void M6_TypeClear(void);
void M6_SelectValue(uint8_t tSelectValue);
void M6_SelectValue_Clear(void);
void M6_ValueUI_Base(uint8_t tType);

void M6_T1YYYY_Update(void);
void M6_T1MM_Update(void);
void M6_T1DD_Update(void);

void M6_T23YYYY_Update(void);
void M6_T2MM_Update(void);
void M6_T2DD_Update(void);

void M6_T3MM_Update(void);
void M6_T3DD_Update(void);

void M6_Hour_Update(void);
void M6_Min_Update(void);

void M6_Currnt_status(void);
void M6_AniMenuStatus(void);

bool M6_UserDisplay(void);

extern void HT_gettimeofday(uint16_t *year,uint8_t *mon,uint8_t *mday,uint8_t *hour,uint8_t *min,uint8_t *sec);		// add 230516 jhlee
extern void HT_settimeofday(uint16_t year,uint8_t mon,uint8_t mday,uint8_t hour,uint8_t min,uint8_t sec);		// add 230516 jhlee

extern void GotoMenuTop(void);

void M6_JogMultiKeyDnChk_Update(void); // 2024-05-22 add whlim

void GotoMenu6(void)	// JPHong 2023-04-18
{
    ITULayer* Menu6_Layer = ituSceneFindWidget(&theScene, "Menu6_Layer");
    assert(Menu6_Layer);
    ituLayerGoto(Menu6_Layer);
}

bool M6_TouchUP_Restet(void)
{
	M6_FastFlag = 0;
	M6_1SecCNT = 100;
	M6_FastSource = 0;
}
bool M6_Enter(ITUWidget* widget, char* param)
{
	M6_Jog_enterFlag = false;		// 2024-05-22 add whlim
	M6_FastFlag = 0;  // 2024-05-17 add whlim, default setting

	Main_Disp_Mode = MAIN_MODE_MENU;	// JPHong 2023-04-18

	HT_gettimeofday(&gYear,&gMonth,&gDay,&gHour,&gMin,&gSec);	// JPHong 2023-04-18

	M6_1a_Value = ituSceneFindWidget(&theScene, "M6_1a_Value");
	assert(M6_1a_Value);
	M6_2a_Value = ituSceneFindWidget(&theScene, "M6_2a_Value");
	assert(M6_2a_Value);
	M6_3a_Value = ituSceneFindWidget(&theScene, "M6_3a_Value");
	assert(M6_3a_Value);

	M6_1b_Value = ituSceneFindWidget(&theScene, "M6_1b_Value");
	assert(M6_1b_Value);
	M6_2b_Value = ituSceneFindWidget(&theScene, "M6_2b_Value");
	assert(M6_2b_Value);
	M6_3b_Value = ituSceneFindWidget(&theScene, "M6_3b_Value");
	assert(M6_3b_Value);

	M6_4_Value = ituSceneFindWidget(&theScene, "M6_4_Value");
	assert(M6_4_Value);
	M6_5_Value = ituSceneFindWidget(&theScene, "M6_5_Value");
	assert(M6_5_Value);


	SetWidget(M6_10_ZoomBTN,"M6_10_ZoomBTN");
	SetWidget(M6_10_ANI,"M6_10_ANI");
	SetWidget(M6_10_Up1,"M6_10_Up1");
	SetWidget(M6_10_Up0,"M6_10_Up0");
	SetWidget(M6_10_BTN,"M6_10_BTN");
	SetWidget(M6_9_ZoomBTN,"M6_9_ZoomBTN");
	SetWidget(M6_9_ANI,"M6_9_ANI");
	SetWidget(M6_9_Up1,"M6_9_Up1");
	SetWidget(M6_9_Up0,"M6_9_Up0");
	SetWidget(M6_9_BTN,"M6_9_BTN");
	SetWidget(M6_8_ZoomBTN,"M6_8_ZoomBTN");
	SetWidget(M6_8_ANI,"M6_8_ANI");
	SetWidget(M6_8_Up1,"M6_8_Up1");
	SetWidget(M6_8_Up0,"M6_8_Up0");
	SetWidget(M6_8_BTN,"M6_8_BTN");
	SetWidget(M6_12_ZoomBTN,"M6_12_ZoomBTN");
	SetWidget(M6_12_ANI,"M6_12_ANI");
	SetWidget(M6_12_Up1,"M6_12_Up1");
	SetWidget(M6_12_Up0,"M6_12_Up0");
	SetWidget(M6_12_BTN,"M6_12_BTN");
	SetWidget(M6_11_ZoomBTN,"M6_11_ZoomBTN");
	SetWidget(M6_11_ANI,"M6_11_ANI");
	SetWidget(M6_11_Up1,"M6_11_Up1");
	SetWidget(M6_11_Up0,"M6_11_Up0");
	SetWidget(M6_11_BTN,"M6_11_BTN");
	
	M6_SubMenuClear();

	MenuDate_Choice_Index = mDate_ValueSet;//chpark.24.11.07

	switch(MenuDate_Choice_Index)
	{
		case mDate_ValueSet:
		{
			ituWidgetSetVisible(M6_11_BTN, false);
			ituWidgetSetVisible(M6_11_ANI, true);

			M6_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M6_11_ANI,0);
		}
		break;
		
		case mDate_TypeSet:
		{
			ituWidgetSetVisible(M6_12_BTN, false);
			ituWidgetSetVisible(M6_12_ANI, true);

			M6_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M6_12_ANI,0);
		}
		break;
	}

	M6_ValueUI_Base(DTM_DateTime_Mode);
	M6_TypeClear();
	
	switch(DTM_DateTime_Mode)
	{
		case Type1_YYYYMMDD:
		{
			HT_gettimeofday(&gYear,&gMonth,&gDay,&gHour,&gMin,&gSec);	// JPHong 2023-05-06

			M6_Year = gYear;				// 
			M6_Month = gMonth;
			M6_Day = gDay;
			M6_Hour = gHour;
			M6_Min = gMin;
			
			
			M6_T1YYYY_Update();
			ituWidgetSetVisible(M6_1a_Value, true);
			
			M6_T1MM_Update();
			ituWidgetSetVisible(M6_2a_Value, true);
			
			M6_T1DD_Update();
			ituWidgetSetVisible(M6_3a_Value, true);
			
			M6_Hour_Update();
			ituWidgetSetVisible(M6_4_Value, true);
			
			M6_Min_Update();
			ituWidgetSetVisible(M6_5_Value, true);
			
			ituWidgetSetVisible(M6_8_BTN, false);
			ituWidgetSetVisible(M6_8_ANI, true);

			M6_8_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M6_8_ANI,0);
		}
		break;

		case Type2_MMDDYYYY:
		{
			HT_gettimeofday(&gYear,&gMonth,&gDay,&gHour,&gMin,&gSec);	// JPHong 2023-05-06

			M6_Year = gYear;				// 
			M6_Month = gMonth;
			M6_Day = gDay;
			M6_Hour = gHour;
			M6_Min = gMin;


			M6_T2MM_Update();
			ituWidgetSetVisible(M6_2b_Value, true);
			
			M6_T2DD_Update();
			ituWidgetSetVisible(M6_3b_Value, true);
			
			M6_T23YYYY_Update();
			ituWidgetSetVisible(M6_1b_Value, true);
			
			M6_Hour_Update();
			ituWidgetSetVisible(M6_4_Value, true);
			
			M6_Min_Update();
			ituWidgetSetVisible(M6_5_Value, true);

			ituWidgetSetVisible(M6_9_BTN, false);
			ituWidgetSetVisible(M6_9_ANI, true);

			M6_9_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M6_9_ANI,0);
		}
		break;
		
		case Type3_DDMMYYYY:
		{
			HT_gettimeofday(&gYear,&gMonth,&gDay,&gHour,&gMin,&gSec);	// JPHong 2023-05-06

			M6_Year = gYear;				// 
			M6_Month = gMonth;
			M6_Day = gDay;
			M6_Hour = gHour;
			M6_Min = gMin;

			M6_T3DD_Update();
			ituWidgetSetVisible(M6_2b_Value, true);

			M6_T3MM_Update();
			ituWidgetSetVisible(M6_3b_Value, true);
			
			M6_T23YYYY_Update();
			ituWidgetSetVisible(M6_1b_Value, true);
			
			M6_Hour_Update();
			ituWidgetSetVisible(M6_4_Value, true);
			
			M6_Min_Update();
			ituWidgetSetVisible(M6_5_Value, true);
			
			ituWidgetSetVisible(M6_10_BTN, false);
			ituWidgetSetVisible(M6_10_ANI, true);

			M6_10_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M6_10_ANI,0);
		}
		break;
	}

	M6_UserDisplay();
	
	M6_SecTimer = gMainTimerCnt_10m;
	M6_ExitCNT = 0;
	M6_1SecCNT = 100;
	
	M6_BlinkFlag = 0;
	M6_Select_Date = 0;//chpark.24.04.19
    return true;
}

bool M6_Timer(ITUWidget* widget, char* param)
{
	uint8_t tDayMax[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	
	if((gMainTimerCnt_10m - M6_SecTimer) >= 100)
	{
		if(M6_ExitCNT<30)
			M6_ExitCNT++;

		if(M6_ExitCNT == 30)
		{
			M6_ExitCNT = 100;
			printf("\r\nM6 Menu 30Sec Exit > goto Main\r\n");
			//GotoMain(MAIN_MODE_NORMAL); 
		}

		// 1sec long fast up/dn check
		if(M6_1SecCNT < 1)
			M6_1SecCNT++;

		if(M6_1SecCNT == 1)
		{
			M6_1SecCNT == 100;
			printf("\r\nM6 Long key\r\n");

			if(M6_FastFlag == 1)
			{
				M6_FastFlag = 2;
				M6_FastTimer = gMainTimerCnt_10m;
			}
		}
		
		M6_SecTimer = gMainTimerCnt_10m;
	}

	if(M6_BlinkFlag == 1)		// on
	{
		if((gMainTimerCnt_10m - M6_BlinkTimer) >= 30) // 300msec
		{
			//printf("Blink on %d\r\n",M6_Select_Date);
			
			switch(M6_Select_Date)
			{
				case 1:
					ituWidgetSetVisible(M6_1a_Value, true);
				break;
				
				case 2:
					ituWidgetSetVisible(M6_2a_Value, true);
				break;

				case 3:
					ituWidgetSetVisible(M6_3a_Value, true);
				break;

				case 4:
					ituWidgetSetVisible(M6_4_Value, true);
				break;

				case 5:
					ituWidgetSetVisible(M6_5_Value, true);
				break;

				case 11:
					ituWidgetSetVisible(M6_1b_Value, true);
				break;
				
				case 12:
					ituWidgetSetVisible(M6_2b_Value, true);
				break;

				case 13:
					ituWidgetSetVisible(M6_3b_Value, true);
				break;
			}

			M6_BlinkFlag = 2;
			M6_BlinkTimer = gMainTimerCnt_10m;
		}
	}
	else if(M6_BlinkFlag == 2)	// off
	{
		if((gMainTimerCnt_10m - M6_BlinkTimer) >= 30) // 300msec
		{
			//printf("Blink off %d\r\n",M6_Select_Date);
			
			switch(M6_Select_Date)
			{
				case 1:
					ituWidgetSetVisible(M6_1a_Value, false);
				break;
				
				case 2:
					ituWidgetSetVisible(M6_2a_Value, false);
				break;

				case 3:
					ituWidgetSetVisible(M6_3a_Value, false);
				break;

				case 4:
					ituWidgetSetVisible(M6_4_Value, false);
				break;

				case 5:
					ituWidgetSetVisible(M6_5_Value, false);
				break;

				case 11:
					ituWidgetSetVisible(M6_1b_Value, false);
				break;
				
				case 12:
					ituWidgetSetVisible(M6_2b_Value, false);
				break;

				case 13:
					ituWidgetSetVisible(M6_3b_Value, false);
				break;
			}

			M6_BlinkFlag = 1;
			M6_BlinkTimer = gMainTimerCnt_10m;
		}
	}

	if(M6_FastFlag == 2)
	{
		// fast up/down count
		switch(M6_FastSource)
		{
			case 7:		// up count
			{
				switch(M6_Select_Date)
				{
					case 1:		// type1 year
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if(M6_Year < YEAR_MAX)
								M6_Year++;
							M6_T1YYYY_Update();
					
							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;

					case 2:		// type1 month
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if(M6_Month < 12)
								M6_Month++;
							M6_T1MM_Update();
					
							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;

					case 3:		// type1 day
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if((M6_Year%4)==0)
								tDayMax[1] = 29;
							else
								tDayMax[1] = 28;

							if(M6_Day < tDayMax[M6_Month-1])			// max limit check
								M6_Day++;
							M6_T1DD_Update();

							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;

					case 4:		// hour
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if(M6_Hour < 23)
								M6_Hour++;
							M6_Hour_Update();

							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;

					case 5:		// min
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if(M6_Min < 59)
								M6_Min++;
							M6_Min_Update();

							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;

					case 11:	// type23 year
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if(M6_Year < YEAR_MAX)
								M6_Year++;
							M6_T23YYYY_Update();

							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;

					case 12:	// type2 month , type3 day 
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if(DTM_DateTime_Mode == Type2_MMDDYYYY)
							{
								if(M6_Month < 12)
									M6_Month++;
								M6_T2MM_Update();
							}
							else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
							{
								if((M6_Year%4)==0)
									tDayMax[1] = 29;
								else
									tDayMax[1] = 28;

								if(M6_Day < tDayMax[M6_Month-1])			// max limit check
									M6_Day++;
								M6_T3DD_Update();
							}

							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;

					case 13:	// type2 day , type3 month 
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if(DTM_DateTime_Mode == Type2_MMDDYYYY)
							{
								if((M6_Year%4)==0)
									tDayMax[1] = 29;
								else
									tDayMax[1] = 28;

								if(M6_Day < tDayMax[M6_Month-1])			// max limit check
									M6_Day++;
								M6_T2DD_Update();
							}
							else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
							{
								if(M6_Month < 12)
									M6_Month++;
								M6_T3MM_Update();
							}

							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;
				}
			}
			break;
			
			case 6:		// down count
			{
				switch(M6_Select_Date)
				{
					case 1:		// type1 year
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if(M6_Year > YEAR_MIN)
								M6_Year--;
							M6_T1YYYY_Update();
					
							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;
					
					case 2:		// type1 month
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if(M6_Month > 1)
								M6_Month--;
							M6_T1MM_Update();

							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;

					case 3:		// type1 day
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if(M6_Day > 1)
								M6_Day--;
							M6_T1DD_Update();

							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;

					case 4:		// hour
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if(M6_Hour > 0)
								M6_Hour--;
							M6_Hour_Update();

							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;

					case 5:		// min
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if(M6_Min > 0)
								M6_Min--;
							M6_Min_Update();

							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;

					case 11:	// type23 year
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if(M6_Year > YEAR_MIN)
								M6_Year--;
							M6_T23YYYY_Update();

							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;

					case 12:	// type2 month , type3 day 
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if(DTM_DateTime_Mode == Type2_MMDDYYYY)
							{
								if(M6_Month > 1)
									M6_Month--;
								M6_T2MM_Update();
							}
							else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
							{
								if(M6_Day > 1)
									M6_Day--;
								M6_T3DD_Update();
							}

							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;

					case 13:	// type2 day , type3 month 
						if((gMainTimerCnt_10m - M6_FastTimer) >= 10)
						{
							if(DTM_DateTime_Mode == Type2_MMDDYYYY)
							{
								if(M6_Day > 1)
									M6_Day--;
								M6_T2DD_Update();
							}
							else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
							{
								if(M6_Month > 1)
									M6_Month--;
								M6_T3MM_Update();
							}

							M6_FastTimer = gMainTimerCnt_10m;
						}
					break;
				}
			}
			break;
		}
	}	
	
	M6_AniMenuStatus();

    return true;
}

bool M6_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t tJogWheel = 0;
	uint8_t tDayMax[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	
	M6_ExitCNT = 0;

	tJogWheel = param[0];
	printf("Menu Date JogWheel %d\r\n",tJogWheel);

	if(tJogWheel == JOG_DIAL_ROT_LEFT)
	{
		// down count
		// no blink , select value none > sub menu change
		// no blink , (enter button)selected value > select window change
		if(M6_BlinkFlag == 0)
		{
			if(M6_Select_Date == 0)
			{
				//sub menu change
				if(MenuDate_Choice_Index == mDate_ValueSet)
				{
					M6_SubMenuClear();
					
					ituWidgetSetVisible(M6_12_BTN, false);
					ituWidgetSetVisible(M6_12_ANI, true);
    
					MenuDate_Choice_Index = mDate_TypeSet;
					M6_TypeChangeDir = 0;
    
					M6_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
					ituAnimationPlay(M6_12_ANI,0);
				}
				else if(MenuDate_Choice_Index == mDate_TypeSet)
				{
					M6_SubMenuClear();
    
					ituWidgetSetVisible(M6_11_BTN, false);
					ituWidgetSetVisible(M6_11_ANI, true);
    
					MenuDate_Choice_Index = mDate_ValueSet;
    
					M6_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
					ituAnimationPlay(M6_11_ANI,0);
				}
			}
			else if(M6_Select_Date != 0)
			{
				// CCW 
				// type1
				// 1>5 , 2>1 , 3>2 , 4>3 , 5>4
				// type2,3
				// 12>5 , 13>12 , 11>13 , 4>11 , 5>4
				if(DTM_DateTime_Mode == Type1_YYYYMMDD)
				{
					if(M6_Select_Date == 1)
						M6_Select_Date = 5;
					else if(M6_Select_Date == 2)
						M6_Select_Date = 1;
					else if(M6_Select_Date == 3)
						M6_Select_Date = 2;
					else if(M6_Select_Date == 4)
						M6_Select_Date = 3;
					else if(M6_Select_Date == 5)
						M6_Select_Date = 4;
				}
				else if((DTM_DateTime_Mode == Type2_MMDDYYYY)||(DTM_DateTime_Mode == Type3_DDMMYYYY))
				{
					if(M6_Select_Date == 12)
						M6_Select_Date = 5;
					else if(M6_Select_Date == 13)
						M6_Select_Date = 12;
					else if(M6_Select_Date == 11)
						M6_Select_Date = 13;
					else if(M6_Select_Date == 4)
						M6_Select_Date = 11;
					else if(M6_Select_Date == 5)
						M6_Select_Date = 4;
				}
				
				M6_SelectValue_Clear();
				M6_SelectValue(M6_Select_Date);
			}
		}
		else if(M6_BlinkFlag != 0)
		{
			//select value down count
			switch(M6_Select_Date)
			{
				case 1:
					
					if(M6_Year > YEAR_MIN)
						M6_Year--;
					M6_T1YYYY_Update();
					
				break;
				
				case 2:
      
					if(M6_Month > 1)
						M6_Month--;
					M6_T1MM_Update();
					
				break;
				
				case 3:
					
					if(M6_Day > 1)
						M6_Day--;
					M6_T1DD_Update();
      
				break;
				
				case 4:
					
					if(M6_Hour > 0)
						M6_Hour--;
					M6_Hour_Update();
      
				break;
				
				case 5:
				
					if(M6_Min > 0)
						M6_Min--;
					M6_Min_Update();
      
				break;
				
				case 11:
      
					if(M6_Year > YEAR_MIN)
						M6_Year--;
					M6_T23YYYY_Update();
      
				break;
				
				case 12:
      
					if(DTM_DateTime_Mode == Type2_MMDDYYYY)
					{
						if(M6_Month > 1)
							M6_Month--;
						M6_T2MM_Update();
					}
					else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
					{
						if(M6_Day > 1)
							M6_Day--;
						M6_T3DD_Update();
					}
					
				break;
				
				case 13:
      
					if(DTM_DateTime_Mode == Type2_MMDDYYYY)
					{
						if(M6_Day > 1)
							M6_Day--;
						M6_T2DD_Update();
					}
					else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
					{
						if(M6_Month > 1)
							M6_Month--;
						M6_T3MM_Update();
					}
      
				break;
			}
		}
	}
	else if(tJogWheel == JOG_DIAL_ROT_RIGHT)
	{
		// up count
		// no blink , select value none > sub menu change
		// no blink , (enter button)selected value > select window change
		if(M6_BlinkFlag == 0)
		{
			if(M6_Select_Date == 0)
			{
				//sub menu change
				if(MenuDate_Choice_Index == mDate_ValueSet)
				{
					M6_SubMenuClear();
					
					ituWidgetSetVisible(M6_12_BTN, false);
					ituWidgetSetVisible(M6_12_ANI, true);
    
					MenuDate_Choice_Index = mDate_TypeSet;
					M6_TypeChangeDir = 0;
    
					M6_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
					ituAnimationPlay(M6_12_ANI,0);
				}
				else if(MenuDate_Choice_Index == mDate_TypeSet)
				{
					M6_SubMenuClear();
    
					ituWidgetSetVisible(M6_11_BTN, false);
					ituWidgetSetVisible(M6_11_ANI, true);
    
					MenuDate_Choice_Index = mDate_ValueSet;
    
					M6_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
					ituAnimationPlay(M6_11_ANI,0);
				}
			}
			else if(M6_Select_Date != 0)
			{
				// CW 
				// type1
				// 1>2 , 2>3 , 3>4 , 4>5 , 5>1
				// type2,3
				// 12>13 , 13>11 , 11>4 , 4>5 , 5>12
				if(DTM_DateTime_Mode == Type1_YYYYMMDD)
				{
					if(M6_Select_Date == 1)
						M6_Select_Date = 2;
					else if(M6_Select_Date == 2)
						M6_Select_Date = 3;
					else if(M6_Select_Date == 3)
						M6_Select_Date = 4;
					else if(M6_Select_Date == 4)
						M6_Select_Date = 5;
					else if(M6_Select_Date == 5)
						M6_Select_Date = 1;
				}
				else if((DTM_DateTime_Mode == Type2_MMDDYYYY)||(DTM_DateTime_Mode == Type3_DDMMYYYY))
				{
					if(M6_Select_Date == 12)
						M6_Select_Date = 13;
					else if(M6_Select_Date == 13)
						M6_Select_Date = 11;
					else if(M6_Select_Date == 11)
						M6_Select_Date = 4;
					else if(M6_Select_Date == 4)
						M6_Select_Date = 5;
					else if(M6_Select_Date == 5)
						M6_Select_Date = 12;
				}
				
				M6_SelectValue_Clear();
				M6_SelectValue(M6_Select_Date);
			}
		}
		else if(M6_BlinkFlag != 0)
		{
			//(enter button)blink > select value up count
			switch(M6_Select_Date)
			{
				case 1:
					
					if(M6_Year < YEAR_MAX)
						M6_Year++;
					M6_T1YYYY_Update();
      
				break;
				
				case 2:
      
					if(M6_Month < 12)
						M6_Month++;
					M6_T1MM_Update();
      
				break;
				
				case 3:
      
					if((M6_Year%4)==0)
						tDayMax[1] = 29;
					else
						tDayMax[1] = 28;
      
					if(M6_Day < tDayMax[M6_Month-1])			// max limit check
						M6_Day++;
					M6_T1DD_Update();
      
				break;
				
				case 4:
      
					if(M6_Hour < 23)
						M6_Hour++;
					M6_Hour_Update();
      
				break;
				
				case 5:
      
					if(M6_Min < 59)
						M6_Min++;
					M6_Min_Update();
      
				break;
				
				case 11:
      
					if(M6_Year < YEAR_MAX)
						M6_Year++;
					M6_T23YYYY_Update();
      
				break;
				
				case 12:
      
					if(DTM_DateTime_Mode == Type2_MMDDYYYY)
					{
						if(M6_Month < 12)
							M6_Month++;
						M6_T2MM_Update();
					}
					else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
					{
						if((M6_Year%4)==0)
							tDayMax[1] = 29;
						else
							tDayMax[1] = 28;
      
						if(M6_Day < tDayMax[M6_Month-1])			// max limit check
							M6_Day++;
						M6_T3DD_Update();
					}
      
				break;
				
				case 13:
      
					if(DTM_DateTime_Mode == Type2_MMDDYYYY)
					{
						if((M6_Year%4)==0)
							tDayMax[1] = 29;
						else
							tDayMax[1] = 28;
      
						if(M6_Day < tDayMax[M6_Month-1])			// max limit check
							M6_Day++;
						M6_T2DD_Update();
					}
					else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
					{
						if(M6_Month < 12)
							M6_Month++;
						M6_T3MM_Update();
					}
      
				break;
			}
		}
	}

    return true;
}

bool M6_JogButton(ITUWidget* widget, char* param)
{
	uint8_t tDayMax[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	uint8_t tJogButton = 0;

	M6_ExitCNT = 0;

	M6_Currnt_status();
	
	tJogButton = param[0];
	printf("Menu6 Date JogButton %d\r\n",tJogButton);

	M6_JogMultiKeyDnChk_Update(); // 2024-05-22 add whlim

			switch(tJogButton)
			{
				case JOG_DIAL_KEY_UP:
					if(MenuDate_Choice_Index == mDate_ValueSet)
					{
						if(M6_Select_Date == 0)
						{
							if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
							{
								// sub menu switching
								printf("sub menu switching\r\n");
								
								M6_Select_Date = 0;
								M6_SelectValue_Clear();
              
								M6_SubMenuClear();
								
								ituWidgetSetVisible(M6_12_BTN, false);
								ituWidgetSetVisible(M6_12_ANI, true);
              
								MenuDate_Choice_Index = mDate_TypeSet;
								M6_TypeChangeDir = 0;
              
								M6_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
								ituAnimationPlay(M6_12_ANI,0);
							}
						}
						else if(M6_Select_Date != 0)
						{
							if(M6_BlinkFlag == 0)
							{
								// select date change
								printf("select date change\r\n");

									// up button 
								if(DTM_DateTime_Mode == Type1_YYYYMMDD)
								{
									// Type1_YYYYMMDD
									// 1>4 , 2>4 , 3>5
									//       4>2 , 5>3
									if(M6_Select_Date == 1)
										M6_Select_Date = 4;
									else if(M6_Select_Date == 2)
										M6_Select_Date = 4;
									else if(M6_Select_Date == 4)
										M6_Select_Date = 2;
									else if(M6_Select_Date == 3)
										M6_Select_Date = 5;
									else if(M6_Select_Date == 5)
										M6_Select_Date = 3;
								}
								else if((DTM_DateTime_Mode == Type2_MMDDYYYY)||(DTM_DateTime_Mode == Type3_DDMMYYYY))
								{
									// Type2_MMDDYYYY,Type3_DDMMYYYY
									// 12>4 , 13>4 , 11>5
									//        4>13 , 5>11 
									if(M6_Select_Date == 12)
										M6_Select_Date = 4;
									else if(M6_Select_Date == 13)
										M6_Select_Date = 4;
									else if(M6_Select_Date == 4)
										M6_Select_Date = 13;
									else if(M6_Select_Date == 11)
										M6_Select_Date = 5;
									else if(M6_Select_Date == 5)
										M6_Select_Date = 11;
								}
								
								M6_SelectValue_Clear();
								M6_SelectValue(M6_Select_Date);
							}
							else if(M6_BlinkFlag != 0)
							{
								// blink > long key check start
								printf("long key check start\r\n");
								M6_FastTimer = gMainTimerCnt_10m;
								M6_FastFlag = 1;
								M6_1SecCNT = 0;

								M6_FastSource = 7;	//up btn
							}
						}
					}
					else if(MenuDate_Choice_Index == mDate_TypeSet)
					{
						if(M6_Select_Date == 0)
						{
							if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
							{
								// sub menu switching
								printf("sub menu switching\r\n");
								M6_Select_Date = 0;
								M6_SelectValue_Clear();
                
								M6_SubMenuClear();
                
								ituWidgetSetVisible(M6_11_BTN, false);
								ituWidgetSetVisible(M6_11_ANI, true);
                
								MenuDate_Choice_Index = mDate_ValueSet;
                
								M6_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
								ituAnimationPlay(M6_11_ANI,0);
							}
						}
					}
					
				break;
				
				case JOG_DIAL_KEY_UP_OFF: //JOG_DIAL_KEY_UP&0x3f:			// up off
					
					if(MenuDate_Choice_Index == mDate_ValueSet)
					{
						if(M6_Select_Date != 0)
						{
							// fast up count stop
							printf("fast up count stop\r\n");
							M6_TouchUP_Restet();

							// blink value up count
							if(M6_BlinkFlag != 0)
							{
								printf("blink value up count\r\n");
								switch(M6_Select_Date)
								{
									case 1:
										
										if(M6_Year < YEAR_MAX)
											M6_Year++;
										M6_T1YYYY_Update();
        
									break;
									
									case 2:
        
										if(M6_Month < 12)
											M6_Month++;
										M6_T1MM_Update();
        
									break;
									
									case 3:
        
										if((M6_Year%4)==0)
											tDayMax[1] = 29;
										else
											tDayMax[1] = 28;
        
										if(M6_Day < tDayMax[M6_Month-1])			// max limit check
											M6_Day++;
										M6_T1DD_Update();
        
									break;
									
									case 4:
        
										if(M6_Hour < 23)
											M6_Hour++;
										M6_Hour_Update();
        
									break;
									
									case 5:
        
										if(M6_Min < 59)
											M6_Min++;
										M6_Min_Update();
        
									break;
									
									case 11:
        
										if(M6_Year < YEAR_MAX)
											M6_Year++;
										M6_T23YYYY_Update();
        
									break;
									
									case 12:
        
										if(DTM_DateTime_Mode == Type2_MMDDYYYY)
										{
											if(M6_Month < 12)
												M6_Month++;
											M6_T2MM_Update();
										}
										else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
										{
											if((M6_Year%4)==0)
												tDayMax[1] = 29;
											else
												tDayMax[1] = 28;
        
											if(M6_Day < tDayMax[M6_Month-1])			// max limit check
												M6_Day++;
											M6_T3DD_Update();
										}
        
									break;
									
									case 13:
        
										if(DTM_DateTime_Mode == Type2_MMDDYYYY)
										{
											if((M6_Year%4)==0)
												tDayMax[1] = 29;
											else
												tDayMax[1] = 28;
        
											if(M6_Day < tDayMax[M6_Month-1])			// max limit check
												M6_Day++;
											M6_T2DD_Update();
										}
										else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
										{
											if(M6_Month < 12)
												M6_Month++;
											M6_T3MM_Update();
										}
        
									break;
								}
							}
						}
					}

				break;
				
				case JOG_DIAL_KEY_DOWN:

					if(MenuDate_Choice_Index == mDate_ValueSet)
					{
						if(M6_Select_Date == 0)
						{
							if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
							{
								// sub menu switching
								M6_Select_Date = 0;
								M6_SelectValue_Clear();
                
								M6_SubMenuClear();
								
								ituWidgetSetVisible(M6_12_BTN, false);
								ituWidgetSetVisible(M6_12_ANI, true);
                
								MenuDate_Choice_Index = mDate_TypeSet;
								M6_TypeChangeDir = 0;
                
								M6_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
								ituAnimationPlay(M6_12_ANI,0);
							}
						}
						else if(M6_Select_Date != 0)
						{
							if(M6_BlinkFlag == 0)
							{
								// select date change
								// down button 
								if(DTM_DateTime_Mode == Type1_YYYYMMDD)
								{
									// Type1_YYYYMMDD
									// 1>4 , 2>4 , 3>5
									//       4>2 , 5>3
									if(M6_Select_Date == 1)
										M6_Select_Date = 4;
									else if(M6_Select_Date == 2)
										M6_Select_Date = 4;
									else if(M6_Select_Date == 4)
										M6_Select_Date = 2;
									else if(M6_Select_Date == 3)
										M6_Select_Date = 5;
									else if(M6_Select_Date == 5)
										M6_Select_Date = 3;
								}
								else if((DTM_DateTime_Mode == Type2_MMDDYYYY)||(DTM_DateTime_Mode == Type3_DDMMYYYY))
								{
									// Type2_MMDDYYYY,Type3_DDMMYYYY
									// 12>4 , 13>4 , 11>5
									//        4>13 , 5>11 
									if(M6_Select_Date == 12)
										M6_Select_Date = 4;
									else if(M6_Select_Date == 13)
										M6_Select_Date = 4;
									else if(M6_Select_Date == 4)
										M6_Select_Date = 13;
									else if(M6_Select_Date == 11)
										M6_Select_Date = 5;
									else if(M6_Select_Date == 5)
										M6_Select_Date = 11;
								}

								M6_SelectValue_Clear();
								M6_SelectValue(M6_Select_Date);
							}
							else if(M6_BlinkFlag != 0)
							{
								// blink > long key check start
								printf("long key check start\r\n");
								M6_FastTimer = gMainTimerCnt_10m;
								M6_FastFlag = 1;
								M6_1SecCNT = 0;

								M6_FastSource = 6;	//down btn
							}
						}
					}
					else if(MenuDate_Choice_Index == mDate_TypeSet)
					{
						if(M6_Select_Date == 0)
						{
							if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
							{
								// sub menu switching
								M6_Select_Date = 0;
								M6_SelectValue_Clear();
                
								M6_SubMenuClear();
                
								ituWidgetSetVisible(M6_11_BTN, false);
								ituWidgetSetVisible(M6_11_ANI, true);
                
								MenuDate_Choice_Index = mDate_ValueSet;
                
								M6_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
								ituAnimationPlay(M6_11_ANI,0);
							}
						}
					}

				break;

				case JOG_DIAL_KEY_DOWN_OFF: //JOG_DIAL_KEY_DOWN&0x3f:			// down off
					
					if(MenuDate_Choice_Index == mDate_ValueSet)
					{
						if(M6_Select_Date != 0)
						{
							// fast up count stop
							printf("fast up count stop\r\n");
							M6_TouchUP_Restet();

							// blink value down count
							if(M6_BlinkFlag != 0)
							{
								printf("blink value down count\r\n");
								switch(M6_Select_Date)
								{
									case 1:
										
										if(M6_Year > YEAR_MIN)
											M6_Year--;
										M6_T1YYYY_Update();
										
									break;
									
									case 2:
          
										if(M6_Month > 1)
											M6_Month--;
										M6_T1MM_Update();
										
									break;
									
									case 3:
										
										if(M6_Day > 1)
											M6_Day--;
										M6_T1DD_Update();
          
									break;
									
									case 4:
										
										if(M6_Hour > 0)
											M6_Hour--;
										M6_Hour_Update();
          
									break;
									
									case 5:
									
										if(M6_Min > 0)
											M6_Min--;
										M6_Min_Update();
          
									break;
									
									case 11:
          
										if(M6_Year > YEAR_MIN)
											M6_Year--;
										M6_T23YYYY_Update();
          
									break;
									
									case 12:
          
										if(DTM_DateTime_Mode == Type2_MMDDYYYY)
										{
											if(M6_Month > 1)
												M6_Month--;
											M6_T2MM_Update();
										}
										else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
										{
											if(M6_Day > 1)
												M6_Day--;
											M6_T3DD_Update();
										}
										
									break;
									
									case 13:
          
										if(DTM_DateTime_Mode == Type2_MMDDYYYY)
										{
											if(M6_Day > 1)
												M6_Day--;
											M6_T2DD_Update();
										}
										else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
										{
											if(M6_Month > 1)
												M6_Month--;
											M6_T3MM_Update();
										}
          
									break;
								}
							}
						}
					}

				break;

				case JOG_DIAL_KEY_LEFT:

					if(MenuDate_Choice_Index == mDate_ValueSet)
					{
						if(M6_Select_Date == 0)
						{
							if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
							{
								// sub menu switching
								M6_Select_Date = 0;
								M6_SelectValue_Clear();
                
								M6_SubMenuClear();
								
								ituWidgetSetVisible(M6_12_BTN, false);
								ituWidgetSetVisible(M6_12_ANI, true);
                
								MenuDate_Choice_Index = mDate_TypeSet;
								M6_TypeChangeDir = 0;
                
								M6_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
								ituAnimationPlay(M6_12_ANI,0);
							}
						}
						else if(M6_Select_Date != 0)
						{
							if(M6_BlinkFlag == 0)
							{
								// select date change	
								// left button 
								if(DTM_DateTime_Mode == Type1_YYYYMMDD)
								{
									// Type1_YYYYMMDD
									// 1>3 , 2>1 , 3>2
									//       4>5 , 5>4
									if(M6_Select_Date == 1)
										M6_Select_Date = 3;
									else if(M6_Select_Date == 2)
										M6_Select_Date = 1;
									else if(M6_Select_Date == 3)
										M6_Select_Date = 2;
									else if(M6_Select_Date == 4)
										M6_Select_Date = 5;
									else if(M6_Select_Date == 5)
										M6_Select_Date = 4;
								}
								else if((DTM_DateTime_Mode == Type2_MMDDYYYY)||(DTM_DateTime_Mode == Type3_DDMMYYYY))
								{
									// Type2_MMDDYYYY,Type3_DDMMYYYY
									// 12>11 , 13>12 , 11>13
									//        4>5 , 5>4 
									if(M6_Select_Date == 12)
										M6_Select_Date = 11;
									else if(M6_Select_Date == 13)
										M6_Select_Date = 12;
									else if(M6_Select_Date == 11)
										M6_Select_Date = 13;
									else if(M6_Select_Date == 4)
										M6_Select_Date = 5;
									else if(M6_Select_Date == 5)
										M6_Select_Date = 4;
								}

								M6_SelectValue_Clear();
								M6_SelectValue(M6_Select_Date);
							}
							else if(M6_BlinkFlag != 0)
							{
								// long key check start
								printf("long key check start\r\n");
								M6_FastTimer = gMainTimerCnt_10m;
								M6_FastFlag = 1;
								M6_1SecCNT = 0;

								M6_FastSource = 6;	//down btn
							}
						}
					}
					else if(MenuDate_Choice_Index == mDate_TypeSet)
					{
						if(M6_Select_Date == 0)
						{
							if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
							{
								// sub menu switching
								M6_Select_Date = 0;
								M6_SelectValue_Clear();
                
								M6_SubMenuClear();
                
								ituWidgetSetVisible(M6_11_BTN, false);
								ituWidgetSetVisible(M6_11_ANI, true);
                
								MenuDate_Choice_Index = mDate_ValueSet;
                
								M6_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
								ituAnimationPlay(M6_11_ANI,0);
							}
						}
					}

				break;
				
				case JOG_DIAL_KEY_LEFT_OFF: //JOG_DIAL_KEY_LEFT&0x3f:

					if(MenuDate_Choice_Index == mDate_ValueSet)
					{
						if(M6_Select_Date != 0)
						{
							// fast up count stop
							printf("fast up count stop\r\n");
							M6_TouchUP_Restet();

							// blink value down count
							if(M6_BlinkFlag != 0)
							{
								printf("blink value down count\r\n");
								switch(M6_Select_Date)
								{
									case 1:
										
										if(M6_Year > YEAR_MIN)
											M6_Year--;
										M6_T1YYYY_Update();
										
									break;
									
									case 2:
          
										if(M6_Month > 1)
											M6_Month--;
										M6_T1MM_Update();
										
									break;
									
									case 3:
										
										if(M6_Day > 1)
											M6_Day--;
										M6_T1DD_Update();
          
									break;
									
									case 4:
										
										if(M6_Hour > 0)
											M6_Hour--;
										M6_Hour_Update();
          
									break;
									
									case 5:
									
										if(M6_Min > 0)
											M6_Min--;
										M6_Min_Update();
          
									break;
									
									case 11:
          
										if(M6_Year > YEAR_MIN)
											M6_Year--;
										M6_T23YYYY_Update();
          
									break;
									
									case 12:
          
										if(DTM_DateTime_Mode == Type2_MMDDYYYY)
										{
											if(M6_Month > 1)
												M6_Month--;
											M6_T2MM_Update();
										}
										else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
										{
											if(M6_Day > 1)
												M6_Day--;
											M6_T3DD_Update();
										}
										
									break;
									
									case 13:
          
										if(DTM_DateTime_Mode == Type2_MMDDYYYY)
										{
											if(M6_Day > 1)
												M6_Day--;
											M6_T2DD_Update();
										}
										else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
										{
											if(M6_Month > 1)
												M6_Month--;
											M6_T3MM_Update();
										}
          
									break;
								}
							}
						}
					}

				break;
				
				case JOG_DIAL_KEY_RIGHT:

					if(MenuDate_Choice_Index == mDate_ValueSet)
					{
						if(M6_Select_Date == 0)
						{
							if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
							{
								// sub menu switching
								M6_Select_Date = 0;
								M6_SelectValue_Clear();
                
								M6_SubMenuClear();
								
								ituWidgetSetVisible(M6_12_BTN, false);
								ituWidgetSetVisible(M6_12_ANI, true);
                
								MenuDate_Choice_Index = mDate_TypeSet;
								M6_TypeChangeDir = 0;
                
								M6_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
								ituAnimationPlay(M6_12_ANI,0);
							}
						}
						else if(M6_Select_Date != 0)
						{
							if(M6_BlinkFlag == 0)
							{
								// select date change	
								// right button 
								if(DTM_DateTime_Mode == Type1_YYYYMMDD)
								{
									// Type1_YYYYMMDD
									// 1>2 , 2>3 , 3>1
									//       4>5 , 5>4
									if(M6_Select_Date == 1)
										M6_Select_Date = 2;
									else if(M6_Select_Date == 2)
										M6_Select_Date = 3;
									else if(M6_Select_Date == 3)
										M6_Select_Date = 1;
									else if(M6_Select_Date == 4)
										M6_Select_Date = 5;
									else if(M6_Select_Date == 5)
										M6_Select_Date = 4;
								}
								else if((DTM_DateTime_Mode == Type2_MMDDYYYY)||(DTM_DateTime_Mode == Type3_DDMMYYYY))
								{
									// Type2_MMDDYYYY,Type3_DDMMYYYY
									// 12>13 , 13>11 , 11>12
									//          4>5  ,  5>4 
									if(M6_Select_Date == 12)
										M6_Select_Date = 13;
									else if(M6_Select_Date == 13)
										M6_Select_Date = 11;
									else if(M6_Select_Date == 11)
										M6_Select_Date = 12;
									else if(M6_Select_Date == 4)
										M6_Select_Date = 5;
									else if(M6_Select_Date == 5)
										M6_Select_Date = 4;
								}

								M6_SelectValue_Clear();
								M6_SelectValue(M6_Select_Date);
							}
							else if(M6_BlinkFlag != 0)
							{
								// long key check start
								printf("long key check start\r\n");
								M6_FastTimer = gMainTimerCnt_10m;
								M6_FastFlag = 1;
								M6_1SecCNT = 0;

								M6_FastSource = 7;	//up btn
							}
						}
					}
					else if(MenuDate_Choice_Index == mDate_TypeSet)
					{
						if(M6_Select_Date == 0)
						{
							if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
							{
								// sub menu switching
								M6_Select_Date = 0;
								M6_SelectValue_Clear();
                
								M6_SubMenuClear();
                
								ituWidgetSetVisible(M6_11_BTN, false);
								ituWidgetSetVisible(M6_11_ANI, true);
                
								MenuDate_Choice_Index = mDate_ValueSet;
                
								M6_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
								ituAnimationPlay(M6_11_ANI,0);
							}
						}
					}

				break;
				
				case JOG_DIAL_KEY_RIGHT_OFF: //JOG_DIAL_KEY_RIGHT&0x3f:
					
					if(MenuDate_Choice_Index == mDate_ValueSet)
					{
						if(M6_Select_Date != 0)
						{
							// fast up count stop
							printf("fast up count stop\r\n");
							M6_TouchUP_Restet();

							// blink value up count
							if(M6_BlinkFlag != 0)
							{
								printf("blink value up count\r\n");
								switch(M6_Select_Date)
								{
									case 1:
										
										if(M6_Year < YEAR_MAX)
											M6_Year++;
										M6_T1YYYY_Update();
        
									break;
									
									case 2:
        
										if(M6_Month < 12)
											M6_Month++;
										M6_T1MM_Update();
        
									break;
									
									case 3:
        
										if((M6_Year%4)==0)
											tDayMax[1] = 29;
										else
											tDayMax[1] = 28;
        
										if(M6_Day < tDayMax[M6_Month-1])			// max limit check
											M6_Day++;
										M6_T1DD_Update();
        
									break;
									
									case 4:
        
										if(M6_Hour < 23)
											M6_Hour++;
										M6_Hour_Update();
        
									break;
									
									case 5:
        
										if(M6_Min < 59)
											M6_Min++;
										M6_Min_Update();
        
									break;
									
									case 11:
        
										if(M6_Year < YEAR_MAX)
											M6_Year++;
										M6_T23YYYY_Update();
        
									break;
									
									case 12:
        
										if(DTM_DateTime_Mode == Type2_MMDDYYYY)
										{
											if(M6_Month < 12)
												M6_Month++;
											M6_T2MM_Update();
										}
										else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
										{
											if((M6_Year%4)==0)
												tDayMax[1] = 29;
											else
												tDayMax[1] = 28;
        
											if(M6_Day < tDayMax[M6_Month-1])			// max limit check
												M6_Day++;
											M6_T3DD_Update();
										}
        
									break;
									
									case 13:
        
										if(DTM_DateTime_Mode == Type2_MMDDYYYY)
										{
											if((M6_Year%4)==0)
												tDayMax[1] = 29;
											else
												tDayMax[1] = 28;
        
											if(M6_Day < tDayMax[M6_Month-1])			// max limit check
												M6_Day++;
											M6_T2DD_Update();
										}
										else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
										{
											if(M6_Month < 12)
												M6_Month++;
											M6_T3MM_Update();
										}
        
									break;
								}
							}
						}
					}

				break;

				case JOG_DIAL_KEY_ENTER_ON:

					if(MenuDate_Choice_Index == mDate_ValueSet)
					{
						if(M6_Select_Date == 0)
						{
							// type`s 1st value select
							if(DTM_DateTime_Mode == Type1_YYYYMMDD)
								M6_Select_Date = 1;
							else if((DTM_DateTime_Mode == Type2_MMDDYYYY)||(DTM_DateTime_Mode == Type3_DDMMYYYY))
								M6_Select_Date = 12;
    
							M6_SelectValue(M6_Select_Date);
						}
						else if(M6_Select_Date != 0)
						{
							if(M6_BlinkFlag == 0)
							{
								// no blink > blink	
								M6_BlinkFlag = 1;
								M6_BlinkTimer = gMainTimerCnt_10m;
							}
							else if(M6_BlinkFlag != 0)
							{
								// blink > blink stop , edit value system update
								M6_BlinkFlag = 0;

								switch(M6_Select_Date)
								{
									case 1:
										gYear = M6_Year;			// system year update
										M6_T1YYYY_Update();
										ituWidgetSetVisible(M6_1a_Value, true);
									break;
            
									case 2:
										gMonth = M6_Month;			// system month update
										M6_T1MM_Update();
										ituWidgetSetVisible(M6_2a_Value, true);
									break;
            
									case 3:
										gDay = M6_Day;				// system day update
										M6_T1DD_Update();
										ituWidgetSetVisible(M6_3a_Value, true);
									break;
            
									case 4:
										gHour = M6_Hour;			// system hour update
										M6_Hour_Update();
										ituWidgetSetVisible(M6_4_Value, true);
									break;
            
									case 5:
										gMin = M6_Min;				// system min update
										M6_Min_Update();
										ituWidgetSetVisible(M6_5_Value, true);
									break;
									
									case 11:
										gYear = M6_Year;			 // system year update
										M6_T23YYYY_Update();
										ituWidgetSetVisible(M6_1b_Value, true);
									break;
            
									case 12:
										if(DTM_DateTime_Mode == Type2_MMDDYYYY)
										{
											gMonth = M6_Month;			// system month update
											M6_T2MM_Update();
											ituWidgetSetVisible(M6_2b_Value, true);
										}
										else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
										{
											gDay = M6_Day;				// system day update
											M6_T3DD_Update();
											ituWidgetSetVisible(M6_2b_Value, true);
										}
									break;
            
									case 13:
										if(DTM_DateTime_Mode == Type2_MMDDYYYY)
										{
											gDay = M6_Day;				// system day update
											M6_T2DD_Update();
											ituWidgetSetVisible(M6_3b_Value, true);
										}
										else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
										{
											gMonth = M6_Month;			// system month update
											M6_T3MM_Update();
											ituWidgetSetVisible(M6_3b_Value, true);
										}
									break;
								}
								
								HT_settimeofday(gYear,gMonth,gDay,gHour,gMin,0);	// JPHong 2024-05-03
							}
						}
					}
					else if(MenuDate_Choice_Index == mDate_TypeSet)
					{
						M6_Jog_enterFlag = true;  // 2024-05-22 add whlim

						// date display type change
						if(M6_TypeChangeDir == 0)	// ui down
						{
							DTM_DateTime_Mode++;
							if(DTM_DateTime_Mode > Type3_DDMMYYYY)
							{
								M6_TypeChangeDir = 1;
								DTM_DateTime_Mode = Type2_MMDDYYYY;
							}
						}
						else if(M6_TypeChangeDir == 1)
						{
							DTM_DateTime_Mode--;
							if(DTM_DateTime_Mode < Type1_YYYYMMDD)
							{
								M6_TypeChangeDir = 0;
								DTM_DateTime_Mode = Type2_MMDDYYYY;
							}	
						}
						
						switch(DTM_DateTime_Mode)
						{
							case Type1_YYYYMMDD:

								HT_gettimeofday(&gYear,&gMonth,&gDay,&gHour,&gMin,&gSec);	// JPHong 2023-05-06
            
								M6_Year = gYear;				// 
								M6_Month = gMonth;
								M6_Day = gDay;
								M6_Hour = gHour;
								M6_Min = gMin;
								
								M6_Select_Date = 0;
								M6_SelectValue_Clear();
            
								M6_TypeClear();
            
								ituWidgetSetVisible(M6_8_BTN, false);
								ituWidgetSetVisible(M6_8_ANI, true);
            
								M6_8_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
								ituAnimationPlay(M6_8_ANI,0);
          
							break;
          
							case Type2_MMDDYYYY:
          
								HT_gettimeofday(&gYear,&gMonth,&gDay,&gHour,&gMin,&gSec);	// JPHong 2023-05-06
            
								M6_Year = gYear;				// 
								M6_Month = gMonth;
								M6_Day = gDay;
								M6_Hour = gHour;
								M6_Min = gMin;
            
								M6_Select_Date = 0;
								M6_SelectValue_Clear();
            
								M6_TypeClear();
            
								ituWidgetSetVisible(M6_9_BTN, false);
								ituWidgetSetVisible(M6_9_ANI, true);
            
								M6_9_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
								ituAnimationPlay(M6_9_ANI,0);
          
							break;
          
							case Type3_DDMMYYYY:
          
								HT_gettimeofday(&gYear,&gMonth,&gDay,&gHour,&gMin,&gSec);	// JPHong 2023-05-06
            
								M6_Year = gYear;				// 
								M6_Month = gMonth;
								M6_Day = gDay;
								M6_Hour = gHour;
								M6_Min = gMin;
            
								M6_Select_Date = 0;
								M6_SelectValue_Clear();
								
								M6_TypeClear();
            
								ituWidgetSetVisible(M6_10_BTN, false);
								ituWidgetSetVisible(M6_10_ANI, true);
            
								M6_10_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
								ituAnimationPlay(M6_10_ANI,0);
          
							break;
						}
						
#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
						//DTM_DateTime_Mode
						XO_Save_Config(M7_UserIndex,true,0);
#else
						HT_Save_Config(1);
#endif
					}
				break;

				case JOG_DIAL_KEY_ENTER_OFF:

					if(MenuDate_Choice_Index == mDate_TypeSet)
					{
						M6_Jog_enterFlag = false;  // 2024-05-22 add whlim

						switch(DTM_DateTime_Mode)
						{
							case Type1_YYYYMMDD:
								M6_ValueUI_Base(Type1_YYYYMMDD);
            
								M6_T1YYYY_Update();
								M6_T1MM_Update();
								M6_T1DD_Update();
								M6_Hour_Update();
								M6_Min_Update();
								
								ituWidgetSetVisible(M6_1a_Value, true);
								ituWidgetSetVisible(M6_2a_Value, true);
								ituWidgetSetVisible(M6_3a_Value, true);
            
								ituWidgetSetVisible(M6_4_Value, true);
								ituWidgetSetVisible(M6_5_Value, true);
							break;
							
							case Type2_MMDDYYYY:
								M6_ValueUI_Base(Type2_MMDDYYYY);
            
								M6_T23YYYY_Update();
								M6_T2MM_Update();
								M6_T2DD_Update();
								M6_Hour_Update();
								M6_Min_Update();
								
								ituWidgetSetVisible(M6_1b_Value, true);
								ituWidgetSetVisible(M6_2b_Value, true);
								ituWidgetSetVisible(M6_3b_Value, true);
            
								ituWidgetSetVisible(M6_4_Value, true);
								ituWidgetSetVisible(M6_5_Value, true);
							break;

							case Type3_DDMMYYYY:
								M6_ValueUI_Base(Type3_DDMMYYYY);
            
								M6_T23YYYY_Update();
								M6_T3MM_Update();
								M6_T3DD_Update();
								M6_Hour_Update();
								M6_Min_Update();
								
								ituWidgetSetVisible(M6_1b_Value, true);
								ituWidgetSetVisible(M6_2b_Value, true);
								ituWidgetSetVisible(M6_3b_Value, true);
            
								ituWidgetSetVisible(M6_4_Value, true);
								ituWidgetSetVisible(M6_5_Value, true);
							break;
						}
					}
				break;
			}

    return true;
}

bool M6_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t tJogSwitch = 0;
	
	M6_ExitCNT = 0;

	tJogSwitch = param[0];
	printf("M6_JogSwitch JogSwitch %d\r\n",tJogSwitch);

	if(tJogSwitch == JOG_DIAL_SW_ESC_ON)
	{
		if(M6_BlinkFlag != 0)
		{
			M6_BlinkFlag = 0;			// value blink stop
		
			M6_SkipFlag = 1;			// skip goto menutop

			M6_TouchUP_Restet(); // 2024-05-17 add whlim,
		}
		else if(M6_BlinkFlag == 0)
		{
			if(M6_Select_Date != 0)
			{
				M6_Select_Date = 0;
				M6_SelectValue_Clear();

				M6_SkipFlag = 1;			// skip goto menutop
			}
		}
	}
	//else if(tJogSwitch == (JOG_DIAL_SW_ESC_ON&0x3f))
	else if(tJogSwitch == JOG_DIAL_SW_ESC_ON_OFF)	// ESC OFF
	{
		if(M6_SkipFlag == 1)
		{
			M6_SkipFlag = 0;
			
			// edit value resume
			switch(M6_Select_Date)
			{
				case 1:
					
					M6_Year = gYear;		// year value resume
					
					M6_T1YYYY_Update();
					ituWidgetSetVisible(M6_1a_Value, true);
				break;
				
				case 2:
					
					M6_Month = gMonth;		// month value resume
					
					M6_T1MM_Update();
					ituWidgetSetVisible(M6_2a_Value, true);
    
				break;
				
				case 3:
					
					M6_Day = gDay;		// Day value resume
					
					M6_T1DD_Update();
					ituWidgetSetVisible(M6_3a_Value, true);
    
				break;
				
				case 4:
					
					M6_Hour = gHour;		// Hour value resume
					
					M6_Hour_Update();
					ituWidgetSetVisible(M6_4_Value, true);
    
				break;
				
				case 5:
				
					M6_Min = gMin;		// Min value resume
					
					M6_Min_Update();
					ituWidgetSetVisible(M6_5_Value, true);
    
				break;
				
				case 11:
    
					M6_Year = gYear;		// year value resume
					
					M6_T23YYYY_Update();
					ituWidgetSetVisible(M6_1b_Value, true);
    
				break;
				
				case 12:
    
					if(DTM_DateTime_Mode == Type2_MMDDYYYY)
					{
						M6_Month = gMonth;		// month value resume
					
						M6_T2MM_Update();
						ituWidgetSetVisible(M6_2b_Value, true);
					}
					else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
					{
						M6_Day = gDay;		// Day value resume
					
						M6_T3DD_Update();
						ituWidgetSetVisible(M6_2b_Value, true);
					}
    
				break;
				
				case 13:
    
					if(DTM_DateTime_Mode == Type2_MMDDYYYY)
					{
						M6_Day = gDay;		// Day value resume
					
						M6_T2DD_Update();
						ituWidgetSetVisible(M6_3b_Value, true);
					}
					else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
					{
						M6_Month = gMonth;		// month value resume
					
						M6_T3MM_Update();
						ituWidgetSetVisible(M6_3b_Value, true);
					}
				break;
			}
			
			M6_Currnt_status();
		}
		else if(M6_SkipFlag == 0)			// skip goto menutop
		{
//			HT_settimeofday(gYear,gMonth,gDay,gHour,gMin,0);	// JPHong 2024-05-03
/*
#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
			//DTM_DateTime_Mode
			XO_Save_Config(M7_UserIndex,true,0);
#else
			HT_Save_Config(1);
#endif
*/			
			GotoMenuTop();
		}
	}
	else if(tJogSwitch == JOG_DIAL_SW_HOME)
	{
//		HT_settimeofday(gYear,gMonth,gDay,gHour,gMin,0);	// JPHong 2024-05-03
/*
#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
		//DTM_DateTime_Mode
		XO_Save_Config(M7_UserIndex,true,0);
#else
		HT_Save_Config(1);
#endif
*/
		GotoMain(MAIN_MODE_NORMAL); 
	}

    return true;
}

bool M6_Date_MouseDn(ITUWidget* widget, char* param)
{
	M6_ExitCNT = 0;

	// if blink  > value modify done system update > blink stop
	if(M6_BlinkFlag != 0)
	{
		M6_BlinkFlag = 0;
		
		switch(M6_Select_Date)
		{
			case 1:
				gYear = M6_Year;			// system year update
				M6_T1YYYY_Update();
				ituWidgetSetVisible(M6_1a_Value, true);
			break;

			case 2:
				gMonth = M6_Month;			// system month update
				M6_T1MM_Update();
				ituWidgetSetVisible(M6_2a_Value, true);
			break;

			case 3:
				gDay = M6_Day;				// system day update
				M6_T1DD_Update();
				ituWidgetSetVisible(M6_3a_Value, true);
			break;

			case 4:
				gHour = M6_Hour;			// system hour update
				M6_Hour_Update();
				ituWidgetSetVisible(M6_4_Value, true);
			break;

			case 5:
				gMin = M6_Min;				// system min update
				M6_Min_Update();
				ituWidgetSetVisible(M6_5_Value, true);
			break;
			
			case 11:
				gYear = M6_Year;			 // system year update
				M6_T23YYYY_Update();
				ituWidgetSetVisible(M6_1b_Value, true);
			break;

			case 12:
				if(DTM_DateTime_Mode == Type2_MMDDYYYY)
				{
					gMonth = M6_Month;			// system month update
					M6_T2MM_Update();
					ituWidgetSetVisible(M6_2b_Value, true);
				}
				else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
				{
					gDay = M6_Day;				// system day update
					M6_T3DD_Update();
					ituWidgetSetVisible(M6_2b_Value, true);
				}
			break;

			case 13:
				if(DTM_DateTime_Mode == Type2_MMDDYYYY)
				{
					gDay = M6_Day;				// system day update
					M6_T2DD_Update();
					ituWidgetSetVisible(M6_3b_Value, true);
				}
				else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
				{
					gMonth = M6_Month;			// system month update
					M6_T3MM_Update();
					ituWidgetSetVisible(M6_3b_Value, true);
				}
			break;
		}
		
		M6_Select_Date = 0;
		HT_settimeofday(gYear,gMonth,gDay,gHour,gMin,0);	// JPHong 2024-05-03
	}

	

	M6_Select_Date = (uint8_t)atoi(param);

	//printf(" Date Value Select %d\r\n",M6_Select_Date);

	if(ituWidgetIsVisible(M6_11_ZoomBTN) == false)
	{
		M6_SubMenuClear();

		ituWidgetSetVisible(M6_11_BTN, false);
		ituWidgetSetVisible(M6_11_ANI, true);

		M6_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationPlay(M6_11_ANI,0);
	}

	MenuDate_Choice_Index = mDate_ValueSet;

	M6_SelectValue_Clear();
	M6_SelectValue(M6_Select_Date);

    return true;
}

bool M6_Date_MouseUp(ITUWidget* widget, char* param)
{
	M6_ExitCNT = 0;

	// select value blink start
	M6_BlinkTimer = gMainTimerCnt_10m;
	M6_BlinkFlag = 1;

	M6_Currnt_status();

    return true;
}

bool M6_DnUp_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tUpDn_BTN;
	
	M6_ExitCNT = 0;

	tUpDn_BTN = (uint8_t)atoi(param);
	printf("M6_DnUp_MouseDn=tUpDn_BTN:%d,M6_BlinkFlag:%d \n",tUpDn_BTN,M6_BlinkFlag);
	if(M6_BlinkFlag != 0)
	{
		// 1sec long key check start
		M6_FastTimer = gMainTimerCnt_10m;
		M6_FastFlag = 1;
		M6_1SecCNT = 0;

		M6_FastSource = tUpDn_BTN;
	}

    return true;
}

bool M6_DnUp_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tUpDn_BTN;
	uint8_t tDayMax[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

	M6_ExitCNT = 0;

	M6_TouchUP_Restet();
	
	tUpDn_BTN = (uint8_t)atoi(param);
	printf("M6_DnUp_MouseUp=tUpDn_BTN:%d,M6_BlinkFlag:%d \n",tUpDn_BTN,M6_BlinkFlag);
	if(M6_BlinkFlag==0)
	{
		M6_FastTimer = 0;//gMainTimerCnt_10m;
		M6_BlinkFlag = 1;
	}
	else if(M6_BlinkFlag != 0)
	{
		switch(tUpDn_BTN)
		{
			case 6:					// select value down count
			{
				switch(M6_Select_Date)
				{
					case 1:
						
						if(M6_Year > YEAR_MIN)
							M6_Year--;
						M6_T1YYYY_Update();
						
					break;
					
					case 2:

						if(M6_Month > 1)
							M6_Month--;
						M6_T1MM_Update();
						
					break;
					
					case 3:
						
						if(M6_Day > 1)
							M6_Day--;
						M6_T1DD_Update();

					break;
					
					case 4:
						
						if(M6_Hour > 0)
							M6_Hour--;
						M6_Hour_Update();

					break;
					
					case 5:
					
						if(M6_Min > 0)
							M6_Min--;
						M6_Min_Update();

					break;
					
					case 11:

						if(M6_Year > YEAR_MIN)
							M6_Year--;
						M6_T23YYYY_Update();

					break;
					
					case 12:

						if(DTM_DateTime_Mode == Type2_MMDDYYYY)
						{
							if(M6_Month > 1)
								M6_Month--;
							M6_T2MM_Update();
						}
						else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
						{
							if(M6_Day > 1)
								M6_Day--;
							M6_T3DD_Update();
						}
						
					break;
					
					case 13:

						if(DTM_DateTime_Mode == Type2_MMDDYYYY)
						{
							if(M6_Day > 1)
								M6_Day--;
							M6_T2DD_Update();
						}
						else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
						{
							if(M6_Month > 1)
								M6_Month--;
							M6_T3MM_Update();
						}

					break;
				}
			}
			break;		

			case 7:					// select value up count
			{
				switch(M6_Select_Date)
				{
					case 1:
						
						if(M6_Year < YEAR_MAX)
							M6_Year++;
						M6_T1YYYY_Update();

					break;
					
					case 2:

						if(M6_Month < 12)
							M6_Month++;
						M6_T1MM_Update();

					break;
					
					case 3:

						if((M6_Year%4)==0)
							tDayMax[1] = 29;
						else
							tDayMax[1] = 28;

						if(M6_Day < tDayMax[M6_Month-1])			// max limit check
							M6_Day++;
						M6_T1DD_Update();

					break;
					
					case 4:

						if(M6_Hour < 23)
							M6_Hour++;
						M6_Hour_Update();

					break;
					
					case 5:

						if(M6_Min < 59)
							M6_Min++;
						M6_Min_Update();

					break;
					
					case 11:

						if(M6_Year < YEAR_MAX)
							M6_Year++;
						M6_T23YYYY_Update();

					break;
					
					case 12:

						if(DTM_DateTime_Mode == Type2_MMDDYYYY)
						{
							if(M6_Month < 12)
								M6_Month++;
							M6_T2MM_Update();
						}
						else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
						{
							if((M6_Year%4)==0)
								tDayMax[1] = 29;
							else
								tDayMax[1] = 28;

							if(M6_Day < tDayMax[M6_Month-1])			// max limit check
								M6_Day++;
							M6_T3DD_Update();
						}

					break;
					
					case 13:

						if(DTM_DateTime_Mode == Type2_MMDDYYYY)
						{
							if((M6_Year%4)==0)
								tDayMax[1] = 29;
							else
								tDayMax[1] = 28;

							if(M6_Day < tDayMax[M6_Month-1])			// max limit check
								M6_Day++;
							M6_T2DD_Update();
						}
						else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
						{
							if(M6_Month < 12)
								M6_Month++;
							M6_T3MM_Update();
						}

					break;
				}
			}
			break;		
		}
	}
	
    return true;
}

bool M6_ZoomMouseDn(ITUWidget* widget, char* param)
{
	M6_ExitCNT = 0;

	// if blink  > value modify done system update > blink stop
	if(M6_BlinkFlag != 0)
	{
		M6_BlinkFlag = 0;
		
		switch(M6_Select_Date)
		{
			case 1:
				gYear = M6_Year;			// system year update
				M6_T1YYYY_Update();
				ituWidgetSetVisible(M6_1a_Value, true);
			break;

			case 2:
				gMonth = M6_Month;			// system month update
				M6_T1MM_Update();
				ituWidgetSetVisible(M6_2a_Value, true);
			break;

			case 3:
				gDay = M6_Day;				// system day update
				M6_T1DD_Update();
				ituWidgetSetVisible(M6_3a_Value, true);
			break;

			case 4:
				gHour = M6_Hour;			// system hour update
				M6_Hour_Update();
				ituWidgetSetVisible(M6_4_Value, true);
			break;

			case 5:
				gMin = M6_Min;				// system min update
				M6_Min_Update();
				ituWidgetSetVisible(M6_5_Value, true);
			break;

			case 11:
				gYear = M6_Year;			 // system year update
				M6_T23YYYY_Update();
				ituWidgetSetVisible(M6_1b_Value, true);
			break;

			case 12:
				if(DTM_DateTime_Mode == Type2_MMDDYYYY)
				{
					gMonth = M6_Month;			// system month update
					M6_T2MM_Update();
					ituWidgetSetVisible(M6_2b_Value, true);
				}
				else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
				{
					gDay = M6_Day;				// system day update
					M6_T3DD_Update();
					ituWidgetSetVisible(M6_2b_Value, true);
				}
			break;

			case 13:
				if(DTM_DateTime_Mode == Type2_MMDDYYYY)
				{
					gDay = M6_Day;				// system day update
					M6_T2DD_Update();
					ituWidgetSetVisible(M6_3b_Value, true);
				}
				else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
				{
					gMonth = M6_Month;			// system month update
					M6_T3MM_Update();
					ituWidgetSetVisible(M6_3b_Value, true);
				}
			break;
		}
		
		M6_Select_Date = 0;
		M6_SelectValue_Clear();
		HT_settimeofday(gYear,gMonth,gDay,gHour,gMin,0);	// JPHong 2024-05-03
	}

	

	if(M6_FastFlag == 2)
	{
		M6_TouchUP_Restet();
	}

    return true;
}

bool M6_BTN_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tBTN_Num;
	
	M6_ExitCNT = 0;

	tBTN_Num = (uint8_t)atoi(param);
	printf("Menu6 BTN Dn = %02d",tBTN_Num);

	// if blink  > value modify done system update > blink stop
	if(M6_BlinkFlag != 0)
	{
		M6_BlinkFlag = 0;
		
		switch(M6_Select_Date)
		{
			case 1:
				gYear = M6_Year;			// system year update
				M6_T1YYYY_Update();
				ituWidgetSetVisible(M6_1a_Value, true);
			break;

			case 2:
				gMonth = M6_Month;			// system month update
				M6_T1MM_Update();
				ituWidgetSetVisible(M6_2a_Value, true);
			break;

			case 3:
				gDay = M6_Day;				// system day update
				M6_T1DD_Update();
				ituWidgetSetVisible(M6_3a_Value, true);
			break;

			case 4:
				gHour = M6_Hour;			// system hour update
				M6_Hour_Update();
				ituWidgetSetVisible(M6_4_Value, true);
			break;

			case 5:
				gMin = M6_Min;				// system min update
				M6_Min_Update();
				ituWidgetSetVisible(M6_5_Value, true);
			break;

			case 11:
				gYear = M6_Year;			 // system year update
				M6_T23YYYY_Update();
				ituWidgetSetVisible(M6_1b_Value, true);
			break;

			case 12:
				if(DTM_DateTime_Mode == Type2_MMDDYYYY)
				{
					gMonth = M6_Month;			// system month update
					M6_T2MM_Update();
					ituWidgetSetVisible(M6_2b_Value, true);
				}
				else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
				{
					gDay = M6_Day;				// system day update
					M6_T3DD_Update();
					ituWidgetSetVisible(M6_2b_Value, true);
				}
			break;

			case 13:
				if(DTM_DateTime_Mode == Type2_MMDDYYYY)
				{
					gDay = M6_Day;				// system day update
					M6_T2DD_Update();
					ituWidgetSetVisible(M6_3b_Value, true);
				}
				else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
				{
					gMonth = M6_Month;			// system month update
					M6_T3MM_Update();
					ituWidgetSetVisible(M6_3b_Value, true);
				}
			break;
		}
		
		M6_Select_Date = 0;
		M6_SelectValue_Clear();
		HT_settimeofday(gYear,gMonth,gDay,gHour,gMin,0);	// JPHong 2024-05-03
	}

//	tBTN_Num = (uint8_t)atoi(param);
	
	switch(tBTN_Num)
	{
		case 8:
		{
			if(ituWidgetIsVisible(M6_12_ZoomBTN) == false)
			{
				M6_SubMenuClear();
				
				ituWidgetSetVisible(M6_12_BTN, false);
				ituWidgetSetVisible(M6_12_ANI, true);

				M6_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M6_12_ANI,0);
				
				if(M6_FastFlag == 2)
				{
					M6_TouchUP_Restet();
				}
			}

			MenuDate_Choice_Index = mDate_TypeSet;
			M6_TypeChangeDir = 0;
			
			HT_gettimeofday(&gYear,&gMonth,&gDay,&gHour,&gMin,&gSec);	// JPHong 2023-05-06

			M6_Year = gYear;				// 
			M6_Month = gMonth;
			M6_Day = gDay;
			M6_Hour = gHour;
			M6_Min = gMin;
			
			M6_Select_Date = 0;
			M6_SelectValue_Clear();

			M6_TypeClear();

			ituWidgetSetVisible(M6_8_BTN, false);
			ituWidgetSetVisible(M6_8_ANI, true);

			M6_8_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M6_8_ANI,0);
		}
		break;

		case 9:
		{
			if(ituWidgetIsVisible(M6_12_ZoomBTN) == false)
			{
				M6_SubMenuClear();
				
				ituWidgetSetVisible(M6_12_BTN, false);
				ituWidgetSetVisible(M6_12_ANI, true);

				M6_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M6_12_ANI,0);

				if(M6_FastFlag == 2)
				{
					M6_TouchUP_Restet();
				}
			}

			MenuDate_Choice_Index = mDate_TypeSet;
			M6_TypeChangeDir = 0;
			
			HT_gettimeofday(&gYear,&gMonth,&gDay,&gHour,&gMin,&gSec);	// JPHong 2023-05-06

			M6_Year = gYear;				// 
			M6_Month = gMonth;
			M6_Day = gDay;
			M6_Hour = gHour;
			M6_Min = gMin;

			M6_Select_Date = 0;
			M6_SelectValue_Clear();

			M6_TypeClear();

			ituWidgetSetVisible(M6_9_BTN, false);
			ituWidgetSetVisible(M6_9_ANI, true);

			M6_9_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M6_9_ANI,0);
		}
		break;

		case 10:
		{
			if(ituWidgetIsVisible(M6_12_ZoomBTN) == false)
			{
				M6_SubMenuClear();
				
				ituWidgetSetVisible(M6_12_BTN, false);
				ituWidgetSetVisible(M6_12_ANI, true);

				M6_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M6_12_ANI,0);

				if(M6_FastFlag == 2)
				{
					M6_TouchUP_Restet();
				}
			}

			MenuDate_Choice_Index = mDate_TypeSet;
			M6_TypeChangeDir = 0;

			HT_gettimeofday(&gYear,&gMonth,&gDay,&gHour,&gMin,&gSec);	// JPHong 2023-05-06

			M6_Year = gYear;				// 
			M6_Month = gMonth;
			M6_Day = gDay;
			M6_Hour = gHour;
			M6_Min = gMin;

			M6_Select_Date = 0;
			M6_SelectValue_Clear();
			
			M6_TypeClear();

			ituWidgetSetVisible(M6_10_BTN, false);
			ituWidgetSetVisible(M6_10_ANI, true);

			M6_10_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M6_10_ANI,0);
		}
		break;
		
		case 11:

			M6_Select_Date = 0;
			M6_SelectValue_Clear();

			M6_SubMenuClear();

			ituWidgetSetVisible(M6_11_BTN, false);
			ituWidgetSetVisible(M6_11_ANI, true);

			MenuDate_Choice_Index = mDate_ValueSet;

			M6_11_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M6_11_ANI,0);
			
			if(M6_FastFlag == 2)
			{
				M6_TouchUP_Restet();
			}
		break;
		
		case 12:

			M6_Select_Date = 0;
			M6_SelectValue_Clear();

			M6_SubMenuClear();
			
			ituWidgetSetVisible(M6_12_BTN, false);
			ituWidgetSetVisible(M6_12_ANI, true);

			MenuDate_Choice_Index = mDate_TypeSet;
			M6_TypeChangeDir = 0;

			M6_12_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(M6_12_ANI,0);

			if(M6_FastFlag == 2)
			{
				M6_TouchUP_Restet();
			}
		break;
	}

    return true;
}

bool M6_BTN_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tBTN_Num;
	
	M6_ExitCNT = 0;

	tBTN_Num = (uint8_t)atoi(param);
	
	switch(tBTN_Num)
	{
		case 8:
			DTM_DateTime_Mode = Type1_YYYYMMDD;
			M6_ValueUI_Base(Type1_YYYYMMDD);

			M6_T1YYYY_Update();
			M6_T1MM_Update();
			M6_T1DD_Update();
			M6_Hour_Update();
			M6_Min_Update();
			
			ituWidgetSetVisible(M6_1a_Value, true);
			ituWidgetSetVisible(M6_2a_Value, true);
			ituWidgetSetVisible(M6_3a_Value, true);

			ituWidgetSetVisible(M6_4_Value, true);
			ituWidgetSetVisible(M6_5_Value, true);
		break;
		
		case 9:
			DTM_DateTime_Mode = Type2_MMDDYYYY;
			M6_ValueUI_Base(Type2_MMDDYYYY);

			M6_T23YYYY_Update();
			M6_T2MM_Update();
			M6_T2DD_Update();
			M6_Hour_Update();
			M6_Min_Update();
			
			ituWidgetSetVisible(M6_1b_Value, true);
			ituWidgetSetVisible(M6_2b_Value, true);
			ituWidgetSetVisible(M6_3b_Value, true);

			ituWidgetSetVisible(M6_4_Value, true);
			ituWidgetSetVisible(M6_5_Value, true);
		break;
		
		case 10:
			DTM_DateTime_Mode = Type3_DDMMYYYY;
			M6_ValueUI_Base(Type3_DDMMYYYY);

			M6_T23YYYY_Update();
			M6_T3MM_Update();
			M6_T3DD_Update();
			M6_Hour_Update();
			M6_Min_Update();
			
			ituWidgetSetVisible(M6_1b_Value, true);
			ituWidgetSetVisible(M6_2b_Value, true);
			ituWidgetSetVisible(M6_3b_Value, true);

			ituWidgetSetVisible(M6_4_Value, true);
			ituWidgetSetVisible(M6_5_Value, true);
		break;
	}

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
	//DTM_DateTime_Mode
	XO_Save_Config(M7_UserIndex,true,0);
#else
	HT_Save_Config(1);
#endif

	M6_Currnt_status();

    return true;
}

bool M6_12_ANIStopped(ITUWidget* widget, char* param)
{
	if(M6_12_Scaleflag == 1)
	{
		M6_12_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M6_12_ANI, false);
		ituWidgetSetVisible(M6_12_ZoomBTN, true);
	}
	else if(M6_12_Scaleflag == 2)
	{
		M6_12_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M6_12_ANI, false);
		ituWidgetSetVisible(M6_12_BTN, true);
	}

    return true;
}

bool M6_11_ANIStopped(ITUWidget* widget, char* param)
{
	if(M6_11_Scaleflag == 1)
	{
		M6_11_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M6_11_ANI, false);
		ituWidgetSetVisible(M6_11_ZoomBTN, true);
	}
	else if(M6_11_Scaleflag == 2)
	{
		M6_11_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M6_11_ANI, false);
		ituWidgetSetVisible(M6_11_BTN, true);
	}

    return true;
}

bool M6_10_ANIStopped(ITUWidget* widget, char* param)
{
	if(M6_10_Scaleflag == 1)
	{
		M6_10_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M6_10_ANI, false);
		ituWidgetSetVisible(M6_10_ZoomBTN, true);
	}
	else if(M6_10_Scaleflag == 2)
	{
		M6_10_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M6_10_ANI, false);
		ituWidgetSetVisible(M6_10_BTN, true);
	}

    return true;
}

bool M6_9_ANIStopped(ITUWidget* widget, char* param)
{
	if(M6_9_Scaleflag == 1)
	{
		M6_9_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M6_9_ANI, false);
		ituWidgetSetVisible(M6_9_ZoomBTN, true);
	}
	else if(M6_9_Scaleflag == 2)
	{
		M6_9_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M6_9_ANI, false);
		ituWidgetSetVisible(M6_9_BTN, true);
	}

    return true;
}

bool M6_8_ANIStopped(ITUWidget* widget, char* param)
{
	if(M6_8_Scaleflag == 1)
	{
		M6_8_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M6_8_ANI, false);
		ituWidgetSetVisible(M6_8_ZoomBTN, true);
	}
	else if(M6_8_Scaleflag == 2)
	{
		M6_8_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(M6_8_ANI, false);
		ituWidgetSetVisible(M6_8_BTN, true);
	}

    return true;
}

bool M6D7_MouseDn(ITUWidget* widget, char* param)
{
	M6_ExitCNT = 0;

	if(MenuDate_Choice_Index == mDate_ValueSet)
	{
		if((M6_BlinkFlag == 0) && (M6_Select_Date > 0))
		{
			M6_BlinkTimer = gMainTimerCnt_10m;
			M6_BlinkFlag = 1;				// select value blink start(edit mode)
		}
		else if((M6_BlinkFlag == 0) && (M6_Select_Date == 0))
		{
			if(DTM_DateTime_Mode == Type1_YYYYMMDD)
				M6_Select_Date = 1;
			else if((DTM_DateTime_Mode == Type2_MMDDYYYY)||(DTM_DateTime_Mode == Type3_DDMMYYYY))
				M6_Select_Date = 12;
    
			M6_SelectValue(M6_Select_Date);
		}
		else if((M6_BlinkFlag != 0))
		{
			M6_BlinkFlag = 0;;				// select value edit mode exit
			
			M6_SkipFlag = 2;
		} 
	}
	else if(MenuDate_Choice_Index == mDate_TypeSet)
	{
		if(M6_TypeChangeDir == 0)	// ui down
		{
			DTM_DateTime_Mode++;
			if(DTM_DateTime_Mode > Type3_DDMMYYYY)
			{
				M6_TypeChangeDir = 1;
				DTM_DateTime_Mode = Type2_MMDDYYYY;
			}
		}
		else if(M6_TypeChangeDir == 1)
		{
			DTM_DateTime_Mode--;
			if(DTM_DateTime_Mode < Type1_YYYYMMDD)
			{
				M6_TypeChangeDir = 0;
				DTM_DateTime_Mode = Type2_MMDDYYYY;
			}	
		}
		
		switch(DTM_DateTime_Mode)
		{
			case Type1_YYYYMMDD:

				HT_gettimeofday(&gYear,&gMonth,&gDay,&gHour,&gMin,&gSec);	// JPHong 2023-05-06
    
				M6_Year = gYear;				// 
				M6_Month = gMonth;
				M6_Day = gDay;
				M6_Hour = gHour;
				M6_Min = gMin;
				
				M6_Select_Date = 0;
				M6_SelectValue_Clear();
    
				M6_TypeClear();
    
				ituWidgetSetVisible(M6_8_BTN, false);
				ituWidgetSetVisible(M6_8_ANI, true);
    
				M6_8_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M6_8_ANI,0);

			break;

			case Type2_MMDDYYYY:

				HT_gettimeofday(&gYear,&gMonth,&gDay,&gHour,&gMin,&gSec);	// JPHong 2023-05-06
    
				M6_Year = gYear;				// 
				M6_Month = gMonth;
				M6_Day = gDay;
				M6_Hour = gHour;
				M6_Min = gMin;
    
				M6_Select_Date = 0;
				M6_SelectValue_Clear();
    
				M6_TypeClear();
    
				ituWidgetSetVisible(M6_9_BTN, false);
				ituWidgetSetVisible(M6_9_ANI, true);
    
				M6_9_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M6_9_ANI,0);

			break;

			case Type3_DDMMYYYY:

				HT_gettimeofday(&gYear,&gMonth,&gDay,&gHour,&gMin,&gSec);	// JPHong 2023-05-06
    
				M6_Year = gYear;				// 
				M6_Month = gMonth;
				M6_Day = gDay;
				M6_Hour = gHour;
				M6_Min = gMin;
    
				M6_Select_Date = 0;
				M6_SelectValue_Clear();
				
				M6_TypeClear();
    
				ituWidgetSetVisible(M6_10_BTN, false);
				ituWidgetSetVisible(M6_10_ANI, true);
    
				M6_10_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(M6_10_ANI,0);

			break;
		}
	}
	
    return true;
}

bool M6D7_MouseUp(ITUWidget* widget, char* param)
{
	M6_ExitCNT = 0;

	if(MenuDate_Choice_Index == mDate_ValueSet)
	{
		// select value > blink > no blink > system update
		if(M6_SkipFlag == 2)
		{
			M6_SkipFlag = 0;
			
			switch(M6_Select_Date)
			{
				case 1:
					gYear = M6_Year;			// system year update
					M6_T1YYYY_Update();
					ituWidgetSetVisible(M6_1a_Value, true);
				break;
    
				case 2:
					gMonth = M6_Month;			// system month update
					M6_T1MM_Update();
					ituWidgetSetVisible(M6_2a_Value, true);
				break;
    
				case 3:
					gDay = M6_Day;				// system day update
					M6_T1DD_Update();
					ituWidgetSetVisible(M6_3a_Value, true);
				break;
    
				case 4:
					gHour = M6_Hour;			// system hour update
					M6_Hour_Update();
					ituWidgetSetVisible(M6_4_Value, true);
				break;
    
				case 5:
					gMin = M6_Min;				// system min update
					M6_Min_Update();
					ituWidgetSetVisible(M6_5_Value, true);
				break;
				
				case 11:
					gYear = M6_Year;			 // system year update
					M6_T23YYYY_Update();
					ituWidgetSetVisible(M6_1b_Value, true);
				break;
    
				case 12:
					if(DTM_DateTime_Mode == Type2_MMDDYYYY)
					{
						gMonth = M6_Month;			// system month update
						M6_T2MM_Update();
						ituWidgetSetVisible(M6_2b_Value, true);
					}
					else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
					{
						gDay = M6_Day;				// system day update
						M6_T3DD_Update();
						ituWidgetSetVisible(M6_2b_Value, true);
					}
				break;
    
				case 13:
					if(DTM_DateTime_Mode == Type2_MMDDYYYY)
					{
						gDay = M6_Day;				// system day update
						M6_T2DD_Update();
						ituWidgetSetVisible(M6_3b_Value, true);
					}
					else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
					{
						gMonth = M6_Month;			// system month update
						M6_T3MM_Update();
						ituWidgetSetVisible(M6_3b_Value, true);
					}
				break;
			}
		}
	}
	else if(MenuDate_Choice_Index == mDate_TypeSet)
	{
		switch(DTM_DateTime_Mode)
		{
			case Type1_YYYYMMDD:

				M6_ValueUI_Base(Type1_YYYYMMDD);
    
				M6_T1YYYY_Update();
				M6_T1MM_Update();
				M6_T1DD_Update();
				M6_Hour_Update();
				M6_Min_Update();
				
				ituWidgetSetVisible(M6_1a_Value, true);
				ituWidgetSetVisible(M6_2a_Value, true);
				ituWidgetSetVisible(M6_3a_Value, true);
    
				ituWidgetSetVisible(M6_4_Value, true);
				ituWidgetSetVisible(M6_5_Value, true);

			break;

			case Type2_MMDDYYYY:

				M6_ValueUI_Base(Type2_MMDDYYYY);
    
				M6_T23YYYY_Update();
				M6_T2MM_Update();
				M6_T2DD_Update();
				M6_Hour_Update();
				M6_Min_Update();
				
				ituWidgetSetVisible(M6_1b_Value, true);
				ituWidgetSetVisible(M6_2b_Value, true);
				ituWidgetSetVisible(M6_3b_Value, true);
    
				ituWidgetSetVisible(M6_4_Value, true);
				ituWidgetSetVisible(M6_5_Value, true);

			break;

			case Type3_DDMMYYYY:

				M6_ValueUI_Base(Type3_DDMMYYYY);
    
				M6_T23YYYY_Update();
				M6_T3MM_Update();
				M6_T3DD_Update();
				M6_Hour_Update();
				M6_Min_Update();
				
				ituWidgetSetVisible(M6_1b_Value, true);
				ituWidgetSetVisible(M6_2b_Value, true);
				ituWidgetSetVisible(M6_3b_Value, true);
    
				ituWidgetSetVisible(M6_4_Value, true);
				ituWidgetSetVisible(M6_5_Value, true);

			break;
		}
	}
	
	M6_Currnt_status();
	
	HT_settimeofday(gYear,gMonth,gDay,gHour,gMin,0);	// JPHong 2024-05-03
	
#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
	//DTM_DateTime_Mode
	XO_Save_Config(M7_UserIndex,true,0);
#else
	HT_Save_Config(1);
#endif
	
    return true;
}

bool M6D6_MouseUp(ITUWidget* widget, char* param)
{
/*
#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
	//DTM_DateTime_Mode
	XO_Save_Config(M7_UserIndex,true,0);
#else
	HT_Save_Config(1);
#endif
*/
	GotoMain(MAIN_MODE_NORMAL); 
	
    return true;
}

bool M6D5_MouseDn(ITUWidget* widget, char* param)
{
	M6_ExitCNT = 0;

	if(M6_BlinkFlag != 0)
	{
		M6_BlinkFlag = 0;			// value blink stop
		
		M6_SkipFlag = 1;			// skip goto menutop
	}
	else
		M6_SkipFlag = 0;

    return true;
}

bool M6D5_MouseUp(ITUWidget* widget, char* param)
{
	M6_ExitCNT = 0;

	if(M6_SkipFlag == 1)
	{
		M6_SkipFlag = 0;
		
		// edit value resume
		switch(M6_Select_Date)
		{
			case 1:
				
				M6_Year = gYear;		// year value resume
				
				M6_T1YYYY_Update();
				ituWidgetSetVisible(M6_1a_Value, true);
			break;
			
			case 2:
				
				M6_Month = gMonth;		// month value resume
				
				M6_T1MM_Update();
				ituWidgetSetVisible(M6_2a_Value, true);

			break;
			
			case 3:
				
				M6_Day = gDay;		// Day value resume
				
				M6_T1DD_Update();
				ituWidgetSetVisible(M6_3a_Value, true);

			break;
			
			case 4:
				
				M6_Hour = gHour;		// Hour value resume
				
				M6_Hour_Update();
				ituWidgetSetVisible(M6_4_Value, true);

			break;
			
			case 5:
			
				M6_Min = gMin;		// Min value resume
				
				M6_Min_Update();
				ituWidgetSetVisible(M6_5_Value, true);

			break;
			
			case 11:

				M6_Year = gYear;		// year value resume
				
				M6_T23YYYY_Update();
				ituWidgetSetVisible(M6_1b_Value, true);

			break;
			
			case 12:

				if(DTM_DateTime_Mode == Type2_MMDDYYYY)
				{
					M6_Month = gMonth;		// month value resume
				
					M6_T2MM_Update();
					ituWidgetSetVisible(M6_2b_Value, true);
				}
				else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
				{
					M6_Day = gDay;		// Day value resume
				
					M6_T3DD_Update();
					ituWidgetSetVisible(M6_2b_Value, true);
				}

			break;
			
			case 13:

				if(DTM_DateTime_Mode == Type2_MMDDYYYY)
				{
					M6_Day = gDay;		// Day value resume
				
					M6_T2DD_Update();
					ituWidgetSetVisible(M6_3b_Value, true);
				}
				else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
				{
					M6_Month = gMonth;		// month value resume
				
					M6_T3MM_Update();
					ituWidgetSetVisible(M6_3b_Value, true);
				}
			break;
		}
		
		M6_Currnt_status();
	}
	else if(M6_SkipFlag == 0)			// skip goto menutop
	{
/*
#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
	//DTM_DateTime_Mode
	XO_Save_Config(M7_UserIndex,true,0);
#else
	HT_Save_Config(1);
#endif
*/
		GotoMenuTop();
	}

    return true;
}

bool M6_UserDisplay(void)
{
	uint8_t tDestName[100];

	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
		return 0;

	ITUWidget *M6_TopAdmin = ituSceneFindWidget(&theScene, "M6_TopAdmin");
	assert(M6_TopAdmin);
	ITUWidget *M6_TopStandard = ituSceneFindWidget(&theScene, "M6_TopStandard");
	assert(M6_TopStandard);
	
	ITUText *M6_TopUserName = ituSceneFindWidget(&theScene, "M6_TopUserName");
	assert(M6_TopUserName);
	
	if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
	{
		ituWidgetSetVisible(M6_TopStandard,false);
		ituWidgetSetVisible(M6_TopAdmin,true);
	}
	else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
	{
		ituWidgetSetVisible(M6_TopAdmin,false);
		ituWidgetSetVisible(M6_TopStandard,true);
	}
	else
	{
		ituWidgetSetVisible(M6_TopAdmin,false);
		ituWidgetSetVisible(M6_TopStandard,false);
	}

	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M6_TopUserName, tDestName);

		ituWidgetSetVisible(M6_TopUserName,true);
	}
	else
	{
		ituWidgetSetVisible(M6_TopUserName,false);
	}
/*
	if((gMainTimerCnt_10m - gMenu_UI_Time) < 500)		// 5sec	
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M6_TopUserName, tDestName);

		ituWidgetSetVisible(M6_TopUserName,true);
	}
	else if(((gMainTimerCnt_10m - gMenu_UI_Time) >= 500)||(M7_UserIndex > (MAX_SU100_USER_NUM-1)))
	{
		ituWidgetSetVisible(M6_TopUserName,false);
	}
*/
}

void M6_SubMenuClear(void)
{
	if(ituWidgetIsVisible(M6_11_ZoomBTN) == true)
	{
		ituWidgetSetVisible(M6_11_ZoomBTN,false);
		ituWidgetSetVisible(M6_11_ANI,true);
		
		// M6_11 scale down
		M6_11_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(M6_11_ANI, 1);
	}
	
	if(ituWidgetIsVisible(M6_12_ZoomBTN) == true)
	{
		ituWidgetSetVisible(M6_12_ZoomBTN,false);
		ituWidgetSetVisible(M6_12_ANI,true);
		
		// M6_12 scale down
		M6_12_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(M6_12_ANI, 1);
	}
}

void M6_TypeClear(void)
{
	if(ituWidgetIsVisible(M6_8_ZoomBTN) == true || ituWidgetIsVisible(M6_8_ANI) == true ) // 2024-05-02 modi whlim, add (|| ituWidgetIsVisible(M6_8_ANI) == true )
	{
		ituWidgetSetVisible(M6_8_ZoomBTN,false);
		ituWidgetSetVisible(M6_8_ANI,true);
		
		// M6_8 scale down
		M6_8_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(M6_8_ANI, 1);
	}
	
	if(ituWidgetIsVisible(M6_9_ZoomBTN) == true || ituWidgetIsVisible(M6_9_ANI) == true)
	{
		ituWidgetSetVisible(M6_9_ZoomBTN,false);
		ituWidgetSetVisible(M6_9_ANI,true);
		
		// M6_9 scale down
		M6_9_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(M6_9_ANI, 1);
	}
	
	if(ituWidgetIsVisible(M6_10_ZoomBTN) == true  || ituWidgetIsVisible(M6_10_ANI) == true)
	{
		ituWidgetSetVisible(M6_10_ZoomBTN,false);
		ituWidgetSetVisible(M6_10_ANI,true);
		
		// M6_10 scale down
		M6_10_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(M6_10_ANI, 1);
	}
}

void M6_SelectValue(uint8_t tSelectValue)
{
	ITUWidget *M6_1a_Sel = ituSceneFindWidget(&theScene, "M6_1a_Sel");
	assert(M6_1a_Sel);
	ITUWidget *M6_2a_Sel = ituSceneFindWidget(&theScene, "M6_2a_Sel");
	assert(M6_2a_Sel);
	ITUWidget *M6_3a_Sel = ituSceneFindWidget(&theScene, "M6_3a_Sel");
	assert(M6_3a_Sel);

	ITUWidget *M6_1b_Sel = ituSceneFindWidget(&theScene, "M6_1b_Sel");
	assert(M6_1b_Sel);
	ITUWidget *M6_2b_Sel = ituSceneFindWidget(&theScene, "M6_2b_Sel");
	assert(M6_2b_Sel);
	ITUWidget *M6_3b_Sel = ituSceneFindWidget(&theScene, "M6_3b_Sel");
	assert(M6_3b_Sel);

	ITUWidget *M6_4_SEL = ituSceneFindWidget(&theScene, "M6_4_SEL");
	assert(M6_4_SEL);
	ITUWidget *M6_5_SEL = ituSceneFindWidget(&theScene, "M6_5_SEL");
	assert(M6_5_SEL);

	switch(tSelectValue)
	{
		case 1:			// type1 year value window
			ituWidgetSetVisible(M6_1a_Sel,true);
		break;
		
		case 2:			// type1 month value window
			ituWidgetSetVisible(M6_2a_Sel,true);
		break;
		
		case 3:			// type1 day value window
			ituWidgetSetVisible(M6_3a_Sel,true);
		break;

		case 4:			// hour value window
			ituWidgetSetVisible(M6_4_SEL,true);
		break;

		case 5:			// min value window
			ituWidgetSetVisible(M6_5_SEL,true);
		break;

		case 11:			// type23 year value window
			ituWidgetSetVisible(M6_1b_Sel,true);
		break;

		case 12:			// type2 month , type3 day value window
			ituWidgetSetVisible(M6_2b_Sel,true);
		break;

		case 13:			// type2 day , type3 month value window
			ituWidgetSetVisible(M6_3b_Sel,true);
		break;
	}
}

void M6_SelectValue_Clear(void)
{
	ITUWidget *M6_1a_Sel = ituSceneFindWidget(&theScene, "M6_1a_Sel");
	assert(M6_1a_Sel);
	ITUWidget *M6_2a_Sel = ituSceneFindWidget(&theScene, "M6_2a_Sel");
	assert(M6_2a_Sel);
	ITUWidget *M6_3a_Sel = ituSceneFindWidget(&theScene, "M6_3a_Sel");
	assert(M6_3a_Sel);

	ITUWidget *M6_1b_Sel = ituSceneFindWidget(&theScene, "M6_1b_Sel");
	assert(M6_1b_Sel);
	ITUWidget *M6_2b_Sel = ituSceneFindWidget(&theScene, "M6_2b_Sel");
	assert(M6_2b_Sel);
	ITUWidget *M6_3b_Sel = ituSceneFindWidget(&theScene, "M6_3b_Sel");
	assert(M6_3b_Sel);

	ITUWidget *M6_4_SEL = ituSceneFindWidget(&theScene, "M6_4_SEL");
	assert(M6_4_SEL);
	ITUWidget *M6_5_SEL = ituSceneFindWidget(&theScene, "M6_5_SEL");
	assert(M6_5_SEL);

	if(ituWidgetIsVisible(M6_1a_Sel) == true)
	{
		ituWidgetSetVisible(M6_1a_Sel,false);
		ituWidgetSetVisible(M6_1a_Value, true);
	}

	if(ituWidgetIsVisible(M6_2a_Sel) == true)
	{
		ituWidgetSetVisible(M6_2a_Sel,false);
		ituWidgetSetVisible(M6_2a_Value, true);
	}

	if(ituWidgetIsVisible(M6_3a_Sel) == true)
	{
		ituWidgetSetVisible(M6_3a_Sel,false);
		ituWidgetSetVisible(M6_3a_Value, true);
	}

	if(ituWidgetIsVisible(M6_1b_Sel) == true)
	{
		ituWidgetSetVisible(M6_1b_Sel,false);
		ituWidgetSetVisible(M6_1b_Value, true);
	}

	if(ituWidgetIsVisible(M6_2b_Sel) == true)
	{
		ituWidgetSetVisible(M6_2b_Sel,false);
		ituWidgetSetVisible(M6_2b_Value, true);
	}
	
	if(ituWidgetIsVisible(M6_3b_Sel) == true)
	{
		ituWidgetSetVisible(M6_3b_Sel,false);
		ituWidgetSetVisible(M6_3b_Value, true);
	}

	if(ituWidgetIsVisible(M6_4_SEL) == true)
	{
		ituWidgetSetVisible(M6_4_SEL,false);
		ituWidgetSetVisible(M6_4_Value, true);
	}
	
	if(ituWidgetIsVisible(M6_5_SEL) == true)
	{
		ituWidgetSetVisible(M6_5_SEL,false);
		ituWidgetSetVisible(M6_5_Value, true);
	}
}

void M6_ValueUI_Base(uint8_t tType)
{
	ITUWidget *M6_SP1a = ituSceneFindWidget(&theScene, "M6_SP1a");
	assert(M6_SP1a);
	ITUWidget *M6_SP2a = ituSceneFindWidget(&theScene, "M6_SP2a");
	assert(M6_SP2a);

	ITUWidget *M6_SP1b = ituSceneFindWidget(&theScene, "M6_SP1b");
	assert(M6_SP1b);
	ITUWidget *M6_SP2b = ituSceneFindWidget(&theScene, "M6_SP2b");
	assert(M6_SP2b);

	ITUWidget *M6_1a_BTN = ituSceneFindWidget(&theScene, "M6_1a_BTN");
	assert(M6_1a_BTN);
	ITUWidget *M6_2a_BTN = ituSceneFindWidget(&theScene, "M6_2a_BTN");
	assert(M6_2a_BTN);
	ITUWidget *M6_3a_BTN = ituSceneFindWidget(&theScene, "M6_3a_BTN");
	assert(M6_3a_BTN);

	ITUWidget *M6_1b_BTN = ituSceneFindWidget(&theScene, "M6_1b_BTN");
	assert(M6_1b_BTN);
	ITUWidget *M6_2b_BTN = ituSceneFindWidget(&theScene, "M6_2b_BTN");
	assert(M6_2b_BTN);
	ITUWidget *M6_3b_BTN = ituSceneFindWidget(&theScene, "M6_3b_BTN");
	assert(M6_3b_BTN);

	ITUWidget *M6_4_BTN = ituSceneFindWidget(&theScene, "M6_4_BTN");
	assert(M6_4_BTN);
	ITUWidget *M6_5_BTN = ituSceneFindWidget(&theScene, "M6_5_BTN");
	assert(M6_5_BTN);

	M6_SelectValue_Clear();

	ituWidgetSetVisible(M6_4_BTN,true);
	ituWidgetSetVisible(M6_5_BTN,true);
	
	switch(tType)
	{
		case Type1_YYYYMMDD:
		{
			ituWidgetSetVisible(M6_SP1b,false);
			ituWidgetSetVisible(M6_SP2b,false);

			ituWidgetSetVisible(M6_1b_BTN,false);
			ituWidgetSetVisible(M6_2b_BTN,false);
			ituWidgetSetVisible(M6_3b_BTN,false);

			ituWidgetSetVisible(M6_1b_Value, false);
			ituWidgetSetVisible(M6_2b_Value, false);
			ituWidgetSetVisible(M6_3b_Value, false);

			ituWidgetSetVisible(M6_SP1a,true);
			ituWidgetSetVisible(M6_SP2a,true);

			ituWidgetSetVisible(M6_1a_BTN,true);
			ituWidgetSetVisible(M6_2a_BTN,true);
			ituWidgetSetVisible(M6_3a_BTN,true);
		}	
		break;

		case Type2_MMDDYYYY:
		case Type3_DDMMYYYY:
		{
			ituWidgetSetVisible(M6_SP1a,false);
			ituWidgetSetVisible(M6_SP2a,false);

			ituWidgetSetVisible(M6_1a_BTN,false);
			ituWidgetSetVisible(M6_2a_BTN,false);
			ituWidgetSetVisible(M6_3a_BTN,false);

			ituWidgetSetVisible(M6_1a_Value, false);
			ituWidgetSetVisible(M6_2a_Value, false);
			ituWidgetSetVisible(M6_3a_Value, false);

			ituWidgetSetVisible(M6_SP1b,true);
			ituWidgetSetVisible(M6_SP2b,true);

			ituWidgetSetVisible(M6_1b_BTN,true);
			ituWidgetSetVisible(M6_2b_BTN,true);
			ituWidgetSetVisible(M6_3b_BTN,true);
		}	
		break;
	}
}

void M6_T1YYYY_Update(void)
{
	char ptM6Value[5];

	memset(ptM6Value,0,5);
	sprintf(ptM6Value,"%4d",M6_Year);
	ituTextSetString(M6_1a_Value, ptM6Value);		// type1 YYYYMMDD;YYYY
	//ituWidgetSetVisible(M6_1a_Value, true);
}

void M6_T1MM_Update(void)
{
	char ptM6Value[5];

	memset(ptM6Value,0,5);
	sprintf(ptM6Value,"%02d",M6_Month);
	ituTextSetString(M6_2a_Value, ptM6Value);		// type1 YYYYMMDD;MM
	//ituWidgetSetVisible(M6_2a_Value, true);
}

void M6_T1DD_Update(void)
{
	char ptM6Value[5];

	memset(ptM6Value,0,5);
	sprintf(ptM6Value,"%02d",M6_Day);
	ituTextSetString(M6_3a_Value, ptM6Value);		// type1 YYYYMMDD;DD
	//ituWidgetSetVisible(M6_3a_Value, true);
}

void M6_T23YYYY_Update(void)
{
	char ptM6Value[5];

	memset(ptM6Value,0,5);
	sprintf(ptM6Value,"%4d",M6_Year);
	ituTextSetString(M6_1b_Value, ptM6Value);		// type2 MMDDYYYY;YYYY
	//ituWidgetSetVisible(M6_1b_Value, true);
}

void M6_T2MM_Update(void)
{
	char ptM6Value[5];

	memset(ptM6Value,0,5);
	sprintf(ptM6Value,"%02d",M6_Month);
	ituTextSetString(M6_2b_Value, ptM6Value);		// type2 MMDDYYYY;MM
	//ituWidgetSetVisible(M6_2b_Value, true);
}

void M6_T2DD_Update(void)
{
	char ptM6Value[5];

	memset(ptM6Value,0,5);
	sprintf(ptM6Value,"%02d",M6_Day);
	ituTextSetString(M6_3b_Value, ptM6Value);		// type2 MMDDYYYY;DD
	//ituWidgetSetVisible(M6_3b_Value, true);
}

void M6_T3MM_Update(void)
{
	char ptM6Value[5];

	memset(ptM6Value,0,5);
	sprintf(ptM6Value,"%02d",M6_Month);
	ituTextSetString(M6_3b_Value, ptM6Value);		// type3 DDMMYYYY;MM
	//ituWidgetSetVisible(M6_3b_Value, true);
}

void M6_T3DD_Update(void)
{
	char ptM6Value[5];

	memset(ptM6Value,0,5);
	sprintf(ptM6Value,"%02d",M6_Day);
	ituTextSetString(M6_2b_Value, ptM6Value);		// type3 DDMMYYYY;DD
	//ituWidgetSetVisible(M6_2b_Value, true);
}

void M6_Hour_Update(void)
{
	char ptM6Value[3];

	memset(ptM6Value,0,3);
	sprintf(ptM6Value,"%02d",M6_Hour);
	ituTextSetString(M6_4_Value, ptM6Value);		// 
	//ituWidgetSetVisible(M6_4_Value, true);
}

void M6_Min_Update(void)
{
	char ptM6Value[3];

	memset(ptM6Value,0,3);
	sprintf(ptM6Value,"%02d",M6_Min);
	ituTextSetString(M6_5_Value, ptM6Value);		// 
	//ituWidgetSetVisible(M6_5_Value, true);
}

void M6_Currnt_status(void)
{
	if(MenuDate_Choice_Index == mDate_ValueSet)
		printf(" _ValueSet,");
	else if(MenuDate_Choice_Index == mDate_TypeSet)
		printf(" _TypeSet,");
	else
		printf(" _NULL,");

	if(DTM_DateTime_Mode == Type1_YYYYMMDD)
	{
		printf("_TYPE1,");
		
		if(M6_Select_Date == 1)
			printf("_Year,");
		else if(M6_Select_Date == 2)
			printf("_Month,");
		else if(M6_Select_Date == 3)
			printf("_Day,");
		else if(M6_Select_Date == 0)
			printf("_None,");
		else if(M6_Select_Date == 4)
			printf("_Hour,");
		else if(M6_Select_Date == 5)
			printf("_Min,");
		else
			printf("_Error,");
	}
	else if(DTM_DateTime_Mode == Type2_MMDDYYYY)
	{
		printf("_TYPE2,");

		if(M6_Select_Date == 11)
			printf("_Year,");
		else if(M6_Select_Date == 12)
			printf("_Month,");
		else if(M6_Select_Date == 13)
			printf("_Day,");
		else if(M6_Select_Date == 0)
			printf("_None,");
		else if(M6_Select_Date == 4)
			printf("_Hour,");
		else if(M6_Select_Date == 5)
			printf("_Min,");
		else
			printf("_Error,");
	}
	else if(DTM_DateTime_Mode == Type3_DDMMYYYY)
	{
		printf("_TYPE3,");

		if(M6_Select_Date == 11)
			printf("_Year,");
		else if(M6_Select_Date == 12)
			printf("_Day,");
		else if(M6_Select_Date == 13)
			printf("_Month,");
		else if(M6_Select_Date == 0)
			printf("_None,");
		else if(M6_Select_Date == 4)
			printf("_Hour,");
		else if(M6_Select_Date == 5)
			printf("_Min,");
		else
			printf("_Error,");
	}
	else
	{
		printf("_ERROR,");

		printf("_Error,");
	}		

	if(M6_BlinkFlag == 0)
		printf("_Blink_OFF\r\n");
	else if((M6_BlinkFlag == 1) || (M6_BlinkFlag == 2))
		printf("_Blink_ON\r\n");
	else
		printf("_Blink_ERR\r\n");
}

void M6_AniMenuStatus(void)
{
	if(ituWidgetIsVisible(M6_11_ZoomBTN) == true)
	{
		if(MenuDate_Choice_Index != mDate_ValueSet)
		{
			M6_11_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M6_11_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M6_11_ANI) == true)
	{
		if(MenuDate_Choice_Index != mDate_ValueSet)
		{
			M6_11_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M6_11_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M6_11_Up1) == true)
	{
		if(MenuDate_Choice_Index != mDate_ValueSet)
		{
			M6_11_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M6_11_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M6_11_Up0) == true)
	{
		if(MenuDate_Choice_Index != mDate_ValueSet)
		{
			M6_11_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M6_11_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(M6_12_ZoomBTN) == true)
	{
		if(MenuDate_Choice_Index != mDate_TypeSet)
		{
			M6_12_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M6_12_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M6_12_ANI) == true)
	{
		if(MenuDate_Choice_Index != mDate_TypeSet)
		{
			M6_12_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M6_12_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M6_12_Up1) == true)
	{
		if(MenuDate_Choice_Index != mDate_TypeSet)
		{
			M6_12_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M6_12_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(M6_12_Up0) == true)
	{
		if(MenuDate_Choice_Index != mDate_TypeSet)
		{
			M6_12_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			M6_12_ANIStopped(NULL,NULL);
		}
	}
	
}

void M6_JogMultiKeyDnChk_Update(void) // 2024-05-22 add whlim
{
	if(M6_Jog_enterFlag) 
	{
		M6_Jog_enterFlag = false;

		switch(DTM_DateTime_Mode)
		{
			case Type1_YYYYMMDD:
				M6_ValueUI_Base(Type1_YYYYMMDD);

				M6_T1YYYY_Update();
				M6_T1MM_Update();
				M6_T1DD_Update();
				M6_Hour_Update();
				M6_Min_Update();
				
				ituWidgetSetVisible(M6_1a_Value, true);
				ituWidgetSetVisible(M6_2a_Value, true);
				ituWidgetSetVisible(M6_3a_Value, true);

				ituWidgetSetVisible(M6_4_Value, true);
				ituWidgetSetVisible(M6_5_Value, true);
			break;
			
			case Type2_MMDDYYYY:
				M6_ValueUI_Base(Type2_MMDDYYYY);

				M6_T23YYYY_Update();
				M6_T2MM_Update();
				M6_T2DD_Update();
				M6_Hour_Update();
				M6_Min_Update();
				
				ituWidgetSetVisible(M6_1b_Value, true);
				ituWidgetSetVisible(M6_2b_Value, true);
				ituWidgetSetVisible(M6_3b_Value, true);

				ituWidgetSetVisible(M6_4_Value, true);
				ituWidgetSetVisible(M6_5_Value, true);
			break;

			case Type3_DDMMYYYY:
				M6_ValueUI_Base(Type3_DDMMYYYY);

				M6_T23YYYY_Update();
				M6_T3MM_Update();
				M6_T3DD_Update();
				M6_Hour_Update();
				M6_Min_Update();
				
				ituWidgetSetVisible(M6_1b_Value, true);
				ituWidgetSetVisible(M6_2b_Value, true);
				ituWidgetSetVisible(M6_3b_Value, true);

				ituWidgetSetVisible(M6_4_Value, true);
				ituWidgetSetVisible(M6_5_Value, true);
			break;
		}
	}
}