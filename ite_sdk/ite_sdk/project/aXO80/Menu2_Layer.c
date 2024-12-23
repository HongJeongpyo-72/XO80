#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

/* widgets:
Menu2_Layer
Menu_TripBG
M2_6T_NA
M2_5T_NA
M2_4T_NA
M2_3T_NA
M2_2T_NA
M2_1T_NA
M2_TopUserName
M2_TopStandard
M2_TopAdmin
M2_D7_BTN
M2_D6_BTN
M2_D5_BTN
M2_6T_Value
M2_5T_Value
M2_4T_Value
M2_3T_Value
M2_2T_Value
M2_1T_Value
M2_6_BTN
M2_5_BTN
M2_4_BTN
M2_3_BTN
M2_2_BTN
M2_1_BTN
M2_6_P
M2_5_P
M2_4_P
M2_3_P
M2_2_P
M2_1_P
M2_6_SEL
M2_5_SEL
M2_4_SEL
M2_3_SEL
M2_2_SEL
M2_1_SEL
*/

static ITUText *M2_1T_Value;
static ITUText *M2_2T_Value;
static ITUText *M2_3T_Value;
static ITUText *M2_4T_Value;
static ITUText *M2_5T_Value;
static ITUText *M2_6T_Value;

static ITUText *M2_1T_NA;
static ITUText *M2_2T_NA;
static ITUText *M2_3T_NA;
static ITUText *M2_4T_NA;
static ITUText *M2_5T_NA;
static ITUText *M2_6T_NA;

ITUWidget *M2_1_SEL;
ITUWidget *M2_2_SEL;
ITUWidget *M2_3_SEL;
ITUWidget *M2_4_SEL;
ITUWidget *M2_5_SEL;
ITUWidget *M2_6_SEL;

ITUWidget *M2_TopAdmin = NULL; 
ITUWidget *M2_TopStandard = NULL;
ITUText *M2_TopUserName = NULL;


uint32_t Trip1_Value = 0xFFFFFFFF;		// CAN[TM1] 0-3byte
uint32_t Trip2_Value = 0xFFFFFFFF;		// CAN[TM1] 4-7byte
uint32_t Trip3_Value = 0xFFFFFFFF;		// CAN[TM2] 0-3byte
uint32_t Trip4_Value = 0xFFFFFFFF;		// CAN[TM2] 4-7byte
uint32_t Trip5_Value = 0xFFFFFFFF;		// CAN[TM3] 0-3byte
uint32_t Trip6_Value = 0xFFFFFFFF;		// CAN[TM3] 4-7byte

static uint8_t Trip_NAFlag[6];

static uint32_t M2_SecTimer;
static uint8_t M2_3SecCNT;
static uint8_t M2_ExitCNT = 0;
static uint32_t M2_BlinkTimer;
static uint8_t M2_BlinkFlag = 0;

char M2_PARA[2];
static uint8_t M2_1TimeFlag = 1;
static uint32_t M2_JogTimer = 0;

uint32_t tExit2_Timer = 0;

void M2_ChoiceTripClear(void);
void M2_ValueUpdate(void);

bool M2_MouseDn(ITUWidget* widget, char* param);
bool M2_MouseUp(ITUWidget* widget, char* param);

bool M2D7_MouseDn(ITUWidget* widget, char* param);
bool M2D7_MouseUp(ITUWidget* widget, char* param);

bool M2_UserDisplay(void);

extern void GotoMenuTop(void);

bool M2_Enter(ITUWidget* widget, char* param)
{
	M2_TopAdmin = ituSceneFindWidget(&theScene, "M2_TopAdmin"); // whLim 2024-04-09
 	M2_TopStandard = ituSceneFindWidget(&theScene, "M2_TopStandard"); // whLim 2024-04-09
 	M2_TopUserName = ituSceneFindWidget(&theScene, "M2_TopUserName"); // whLim 2024-04-09

	M2_1T_Value = ituSceneFindWidget(&theScene, "M2_1T_Value");
	assert(M2_1T_Value);
	M2_2T_Value = ituSceneFindWidget(&theScene, "M2_2T_Value");
	assert(M2_2T_Value);
	M2_3T_Value = ituSceneFindWidget(&theScene, "M2_3T_Value");
	assert(M2_3T_Value);
	M2_4T_Value = ituSceneFindWidget(&theScene, "M2_4T_Value");
	assert(M2_4T_Value);
	M2_5T_Value = ituSceneFindWidget(&theScene, "M2_5T_Value");
	assert(M2_5T_Value);
	M2_6T_Value = ituSceneFindWidget(&theScene, "M2_6T_Value");
	assert(M2_6T_Value);

	M2_1T_NA = ituSceneFindWidget(&theScene, "M2_1T_NA");
	assert(M2_1T_NA);
	M2_2T_NA = ituSceneFindWidget(&theScene, "M2_2T_NA");
	assert(M2_2T_NA);
	M2_3T_NA = ituSceneFindWidget(&theScene, "M2_3T_NA");
	assert(M2_3T_NA);
	M2_4T_NA = ituSceneFindWidget(&theScene, "M2_4T_NA");
	assert(M2_4T_NA);
	M2_5T_NA = ituSceneFindWidget(&theScene, "M2_5T_NA");
	assert(M2_5T_NA);
	M2_6T_NA = ituSceneFindWidget(&theScene, "M2_6T_NA");
	assert(M2_6T_NA);

	M2_1_SEL = ituSceneFindWidget(&theScene, "M2_1_SEL");
	M2_2_SEL = ituSceneFindWidget(&theScene, "M2_2_SEL");
	M2_3_SEL = ituSceneFindWidget(&theScene, "M2_3_SEL");
	M2_4_SEL = ituSceneFindWidget(&theScene, "M2_4_SEL");
	M2_5_SEL = ituSceneFindWidget(&theScene, "M2_5_SEL");
	M2_6_SEL = ituSceneFindWidget(&theScene, "M2_6_SEL");

	M2_ChoiceTripClear();
	M2_ChoiceDnTripClear();
	M2_ValueUpdate();

	MenuTrip_Choice_Index = 1;

	switch(MenuTrip_Choice_Index)
	{
		case 1:			// trip 1
		{
			ituWidgetSetVisible(M2_1_SEL,true);
		}
		break;
		
		case 2:			// trip 2
		{
			ituWidgetSetVisible(M2_2_SEL,true);
		}
		break;
		
		case 3:			// trip 3
		{
			ituWidgetSetVisible(M2_3_SEL,true);
		}
		break;

		case 4:			// trip 4
		{
			ituWidgetSetVisible(M2_4_SEL,true);
		}
		break;

		case 5:			// trip 5
		{
			ituWidgetSetVisible(M2_5_SEL,true);
		}
		break;

		case 6:			// trip 6
		{
			ituWidgetSetVisible(M2_6_SEL,true);
		}
		break;
	}

	if(Trip_NAFlag[0] == 0)
	{
		ituWidgetSetVisible(M2_1T_NA,false);
		ituWidgetSetVisible(M2_1T_Value,true);
	}
	else if(Trip_NAFlag[0] == 1)
	{
		ituWidgetSetVisible(M2_1T_Value,false);
		ituWidgetSetVisible(M2_1T_NA,true);
	}

	if(Trip_NAFlag[1] == 0)
	{	
		ituWidgetSetVisible(M2_2T_NA,false);
		ituWidgetSetVisible(M2_2T_Value,true);
	}
	else if(Trip_NAFlag[1] == 1)
	{
		ituWidgetSetVisible(M2_2T_Value,false);
		ituWidgetSetVisible(M2_2T_NA,true);
	}
	
	if(Trip_NAFlag[2] == 0)
	{	
		ituWidgetSetVisible(M2_3T_NA,false);
		ituWidgetSetVisible(M2_3T_Value,true);
	}
	else if(Trip_NAFlag[2] == 1)
	{
		ituWidgetSetVisible(M2_3T_Value,false);
		ituWidgetSetVisible(M2_3T_NA,true);
	}

	if(Trip_NAFlag[3] == 0)
	{	
		ituWidgetSetVisible(M2_4T_NA,false);
		ituWidgetSetVisible(M2_4T_Value,true);
	}
	else if(Trip_NAFlag[3] == 1)
	{
		ituWidgetSetVisible(M2_4T_Value,false);
		ituWidgetSetVisible(M2_4T_NA,true);
	}

	if(Trip_NAFlag[4] == 0)
	{	
		ituWidgetSetVisible(M2_5T_NA,false);
		ituWidgetSetVisible(M2_5T_Value,true);
	}
	else if(Trip_NAFlag[4] == 1)
	{
		ituWidgetSetVisible(M2_5T_Value,false);
		ituWidgetSetVisible(M2_5T_NA,true);
	}

	if(Trip_NAFlag[5] == 0)
	{	
		ituWidgetSetVisible(M2_6T_NA,false);
		ituWidgetSetVisible(M2_6T_Value,true);
	}
	else if(Trip_NAFlag[5] == 1)
	{
		ituWidgetSetVisible(M2_6T_Value,false);
		ituWidgetSetVisible(M2_6T_NA,true);
	}

	M2_UserDisplay();

	M2_SecTimer = gMainTimerCnt_10m;
	M2_ExitCNT = 0;
	M2_3SecCNT = 100;

	M2_BlinkFlag = 0;
	
	tExit2_Timer = gMainTimerCnt_10m;
	
    return true;
}

bool M2_Timer(ITUWidget* widget, char* param)
{
	char ptTripValue[15];

#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		return 0;
	}
#endif

	if((gMainTimerCnt_10m - tExit2_Timer) >= 3000)
	{
		//printf("\r\n M2 Menu Exit > 30 sec test\r\n");
		
		GotoMain(MAIN_MODE_NORMAL); 
	}
	
	if((gMainTimerCnt_10m - M2_SecTimer) >= 100)
	{
		if(M2_3SecCNT < 3)
			M2_3SecCNT++;

		if(M2_3SecCNT == 3)
		{
			M2_3SecCNT = 100;

			// trip data reset
			switch(MenuTrip_Choice_Index)
			{
				case 1:
					//printf("\r\nTrip1 Reset\r\n");
					Trip1_Value = 0;		// CAN[TM1] 0-3byte

					// CLI 1byte 0~1bit clear CAN Tx
					HMV.HourMeter_Trip_Clear1 = HMV.HourMeter_Trip_Clear1|0x01;
					
				break;

				case 2:
					//printf("\r\nTrip2 Reset\r\n");
					Trip2_Value = 0;			// CAN[TM1] 4-7byte

					// CLI 1byte 2~3bit clear CAN Tx
					HMV.HourMeter_Trip_Clear1 = HMV.HourMeter_Trip_Clear1|0x04;

				break;

				case 3:
					//printf("\r\nTrip3 Reset\r\n");
					Trip3_Value = 0;			// CAN[TM2] 0-3byte

					// CLI 1byte 4~5bit clear CAN Tx
					HMV.HourMeter_Trip_Clear1 = HMV.HourMeter_Trip_Clear1|0x10;
					
				break;

				case 4:
					//printf("\r\nTrip4 Reset\r\n");
					Trip4_Value = 0;		// CAN[TM2] 4-7byte

					// CLI 1byte 6~7bit clear CAN Tx
					HMV.HourMeter_Trip_Clear1 = HMV.HourMeter_Trip_Clear1|0x40;
				break;
				
				case 5:
					//printf("\r\nTrip5 Reset\r\n");
					Trip5_Value = 0;			// CAN[TM3] 0-3byte

					// CLI 2byte 0~1bit clear CAN Tx
					HMV.HourMeter_Trip_Clear2 = HMV.HourMeter_Trip_Clear2|0x01;
					
				break;
				
				case 6:
					//printf("\r\nTrip6 Reset\r\n");
					Trip6_Value = 0;		// CAN[TM4] 4-7byte

					// CLI 2byte 2~3bit clear CAN Tx
					HMV.HourMeter_Trip_Clear2 = HMV.HourMeter_Trip_Clear2|0x04;

				break;
			}
		}

		M2_SecTimer = gMainTimerCnt_10m;
	}

	M2_ValueUpdate();

	if((gMainTimerCnt_10m - TM1_Time) >= 500)
	{
		// trip 1&2 N/A display
		Trip_NAFlag[0] = 1;
		Trip_NAFlag[1] = 1;

		memset(ptTripValue,0,15);
		sprintf(ptTripValue,"N/A");
		ituTextSetString(M2_1T_NA, ptTripValue);
		ituTextSetString(M2_2T_NA, ptTripValue);

		if(M2_BlinkFlag > 0)// whLim 2024-04-09 err modi
		{
			if(MenuTrip_Choice_Index == 1)
			{
				ituWidgetSetVisible(M2_2T_NA,true);
				ituWidgetSetVisible(M2_2T_Value,false);	
			}
			else if(MenuTrip_Choice_Index == 2)
			{
				ituWidgetSetVisible(M2_1T_NA,true);
				ituWidgetSetVisible(M2_1T_Value,false);
			}
			else
			{
				ituWidgetSetVisible(M2_1T_NA,true);
				ituWidgetSetVisible(M2_1T_Value,false);
				ituWidgetSetVisible(M2_2T_NA,true);
				ituWidgetSetVisible(M2_2T_Value,false);
			}
		}
	}
	else // whLim 2024-04-09 err modi
	{
		if(M2_BlinkFlag > 0)
		{
			if(MenuTrip_Choice_Index == 1)
			{
				ituWidgetSetVisible(M2_2T_NA,false);
				ituWidgetSetVisible(M2_2T_Value,true);	
			}
			else if(MenuTrip_Choice_Index == 2)
			{
				ituWidgetSetVisible(M2_1T_NA,false);
				ituWidgetSetVisible(M2_1T_Value,true);
			}
			else
			{
				ituWidgetSetVisible(M2_1T_NA,false);
				ituWidgetSetVisible(M2_1T_Value,true);
				ituWidgetSetVisible(M2_2T_NA,false);
				ituWidgetSetVisible(M2_2T_Value,true);
			}
		}
	}

	if((gMainTimerCnt_10m - TM2_Time) >= 500)
	{
		// trip 3&4 N/A display
		Trip_NAFlag[2] = 1;
		Trip_NAFlag[3] = 1;

		memset(ptTripValue,0,15);
		sprintf(ptTripValue,"N/A");
		ituTextSetString(M2_3T_NA, ptTripValue);
		ituTextSetString(M2_4T_NA, ptTripValue);

		if(M2_BlinkFlag > 0)
		{
			if(MenuTrip_Choice_Index == 3)
			{
				ituWidgetSetVisible(M2_4T_NA,true);
				ituWidgetSetVisible(M2_4T_Value,false);	
			}
			else if(MenuTrip_Choice_Index == 4)
			{
				ituWidgetSetVisible(M2_3T_NA,true);
				ituWidgetSetVisible(M2_3T_Value,false);
			}
			else
			{
				ituWidgetSetVisible(M2_3T_NA,true);
				ituWidgetSetVisible(M2_3T_Value,false);
				ituWidgetSetVisible(M2_4T_NA,true);
				ituWidgetSetVisible(M2_4T_Value,false);
			}
		}
	}
	else
	{
		if(M2_BlinkFlag > 0)
		{
			if(MenuTrip_Choice_Index == 3)
			{
				ituWidgetSetVisible(M2_4T_NA,false);
				ituWidgetSetVisible(M2_4T_Value,true);	
			}
			else if(MenuTrip_Choice_Index == 4)
			{
				ituWidgetSetVisible(M2_3T_NA,false);
				ituWidgetSetVisible(M2_3T_Value,true);
			}
			else
			{
				ituWidgetSetVisible(M2_3T_NA,false);
				ituWidgetSetVisible(M2_3T_Value,true);
				ituWidgetSetVisible(M2_4T_NA,false);
				ituWidgetSetVisible(M2_4T_Value,true);
			}
		}
	}

	if((gMainTimerCnt_10m - TM3_Time) >= 500)
	{
		// trip 5&6 N/A display
		Trip_NAFlag[4] = 1;
		Trip_NAFlag[5] = 1;

		memset(ptTripValue,0,15);
		sprintf(ptTripValue,"N/A");
		ituTextSetString(M2_5T_NA, ptTripValue);
		ituTextSetString(M2_6T_NA, ptTripValue);

		if(M2_BlinkFlag > 0)
		{
			if(MenuTrip_Choice_Index == 5)
			{
				ituWidgetSetVisible(M2_6T_NA,true);
				ituWidgetSetVisible(M2_6T_Value,false);	
			}
			else if(MenuTrip_Choice_Index == 6)
			{
				ituWidgetSetVisible(M2_5T_NA,true);
				ituWidgetSetVisible(M2_5T_Value,false);
			}
			else
			{
				ituWidgetSetVisible(M2_5T_NA,true);
				ituWidgetSetVisible(M2_5T_Value,false);
				ituWidgetSetVisible(M2_6T_NA,true);
				ituWidgetSetVisible(M2_6T_Value,false);
			}
		}
	}
	else
	{
		if(M2_BlinkFlag > 0)
		{
			if(MenuTrip_Choice_Index == 5)
			{
				ituWidgetSetVisible(M2_6T_NA,false);
				ituWidgetSetVisible(M2_6T_Value,true);	
			}
			else if(MenuTrip_Choice_Index == 6)
			{
				ituWidgetSetVisible(M2_5T_NA,false);
				ituWidgetSetVisible(M2_5T_Value,true);
			}
			else
			{
				ituWidgetSetVisible(M2_5T_NA,false);
				ituWidgetSetVisible(M2_5T_Value,true);
				ituWidgetSetVisible(M2_6T_NA,false);
				ituWidgetSetVisible(M2_6T_Value,true);
			}
		}
	}
	
	if(M2_BlinkFlag == 1)		// on
	{
		if((gMainTimerCnt_10m - M2_BlinkTimer) >= 30) // 300msec
		{
			switch(MenuTrip_Choice_Index)
			{
				case 1:
				{
					if(Trip_NAFlag[0] == 0)
					{
						ituWidgetSetVisible(M2_1T_NA,false);
						ituWidgetSetVisible(M2_1T_Value,true);
					}
					else if(Trip_NAFlag[0] == 1)
					{
						ituWidgetSetVisible(M2_1T_Value,false);
						ituWidgetSetVisible(M2_1T_NA,true);
					}
				}
				break;

				case 2:
				{
					if(Trip_NAFlag[1] == 0)
					{
						ituWidgetSetVisible(M2_2T_NA,false);
						ituWidgetSetVisible(M2_2T_Value,true);
					}
					else if(Trip_NAFlag[1] == 1)
					{
						ituWidgetSetVisible(M2_2T_Value,false);
						ituWidgetSetVisible(M2_2T_NA,true);
					}
				}
				break;

				case 3:
				{
					if(Trip_NAFlag[2] == 0)
					{
						ituWidgetSetVisible(M2_3T_NA,false);
						ituWidgetSetVisible(M2_3T_Value,true);
					}
					else if(Trip_NAFlag[2] == 1)
					{
						ituWidgetSetVisible(M2_3T_Value,false);
						ituWidgetSetVisible(M2_3T_NA,true);
					}
				}
				break;

				case 4:
				{
					if(Trip_NAFlag[3] == 0)
					{
						ituWidgetSetVisible(M2_4T_NA,false);
						ituWidgetSetVisible(M2_4T_Value,true);
					}
					else if(Trip_NAFlag[3] == 1)
					{
						ituWidgetSetVisible(M2_4T_Value,false);
						ituWidgetSetVisible(M2_4T_NA,true);
					}
				}
				break;

				case 5:
				{
					if(Trip_NAFlag[4] == 0)
					{
						ituWidgetSetVisible(M2_5T_NA,false);
						ituWidgetSetVisible(M2_5T_Value,true);
					}
					else if(Trip_NAFlag[4] == 1)
					{
						ituWidgetSetVisible(M2_5T_Value,false);
						ituWidgetSetVisible(M2_5T_NA,true);
					}
				}
				break;

				case 6:
				{
					if(Trip_NAFlag[5] == 0)
					{
						ituWidgetSetVisible(M2_6T_NA,false);
						ituWidgetSetVisible(M2_6T_Value,true);
					}
					else if(Trip_NAFlag[5] == 1)
					{
						ituWidgetSetVisible(M2_6T_Value,false);
						ituWidgetSetVisible(M2_6T_NA,true);
					}
				}
				break;
			}

			M2_BlinkFlag = 2;
			M2_BlinkTimer = gMainTimerCnt_10m;
		}
	}
	else if(M2_BlinkFlag == 2)	// off
	{
		if((gMainTimerCnt_10m - M2_BlinkTimer) >= 30) // 300msec
		{
			switch(MenuTrip_Choice_Index)
			{
				case 1:
				{
					ituWidgetSetVisible(M2_1T_Value,false);
					ituWidgetSetVisible(M2_1T_NA,false);
				}
				break;

				case 2:
				{
					ituWidgetSetVisible(M2_2T_Value,false);
					ituWidgetSetVisible(M2_2T_NA,false);
				}
				break;

				case 3:
				{
					ituWidgetSetVisible(M2_3T_Value,false);
					ituWidgetSetVisible(M2_3T_NA,false);
				}
				break;

				case 4:
				{
					ituWidgetSetVisible(M2_4T_Value,false);
					ituWidgetSetVisible(M2_4T_NA,false);
				}
				break;

				case 5:
				{
					ituWidgetSetVisible(M2_5T_Value,false);
					ituWidgetSetVisible(M2_5T_NA,false);
				}
				break;

				case 6:
				{
					ituWidgetSetVisible(M2_6T_Value,false);
					ituWidgetSetVisible(M2_6T_NA,false);
				}
				break;
			}

			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
		}
	}
	else if(M2_BlinkFlag == 0)
	{
		if(Trip_NAFlag[0] == 0)
		{
			ituWidgetSetVisible(M2_1T_NA,false);
			ituWidgetSetVisible(M2_1T_Value,true);
		}
		else if(Trip_NAFlag[0] == 1)
		{
			ituWidgetSetVisible(M2_1T_Value,false);
			ituWidgetSetVisible(M2_1T_NA,true);
		}
    
		if(Trip_NAFlag[1] == 0)
		{	
			ituWidgetSetVisible(M2_2T_NA,false);
			ituWidgetSetVisible(M2_2T_Value,true);
		}
		else if(Trip_NAFlag[1] == 1)
		{
			ituWidgetSetVisible(M2_2T_Value,false);
			ituWidgetSetVisible(M2_2T_NA,true);
		}
		
		if(Trip_NAFlag[2] == 0)
		{	
			ituWidgetSetVisible(M2_3T_NA,false);
			ituWidgetSetVisible(M2_3T_Value,true);
		}
		else if(Trip_NAFlag[2] == 1)
		{
			ituWidgetSetVisible(M2_3T_Value,false);
			ituWidgetSetVisible(M2_3T_NA,true);
		}
    
		if(Trip_NAFlag[3] == 0)
		{	
			ituWidgetSetVisible(M2_4T_NA,false);
			ituWidgetSetVisible(M2_4T_Value,true);
		}
		else if(Trip_NAFlag[3] == 1)
		{
			ituWidgetSetVisible(M2_4T_Value,false);
			ituWidgetSetVisible(M2_4T_NA,true);
		}
    
		if(Trip_NAFlag[4] == 0)
		{	
			ituWidgetSetVisible(M2_5T_NA,false);
			ituWidgetSetVisible(M2_5T_Value,true);
		}
		else if(Trip_NAFlag[4] == 1)
		{
			ituWidgetSetVisible(M2_5T_Value,false);
			ituWidgetSetVisible(M2_5T_NA,true);
		}
    
		if(Trip_NAFlag[5] == 0)
		{	
			ituWidgetSetVisible(M2_6T_NA,false);
			ituWidgetSetVisible(M2_6T_Value,true);
		}
		else if(Trip_NAFlag[5] == 1)
		{
			ituWidgetSetVisible(M2_6T_Value,false);
			ituWidgetSetVisible(M2_6T_NA,true);
		}
	}


	// jog wheel action
	if(((gMainTimerCnt_10m - M2_JogTimer) >= 10) && (M2_1TimeFlag == 0))
	{
		M2_1TimeFlag = 1;
		M2_MouseDn(NULL, M2_PARA);
		usleep(10*1000);
		M2_MouseUp(NULL, M2_PARA);
	}

	M2_UserDisplay();	
	
    return true;
}

bool M2_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t JogDir = 0;
	
	M2_ExitCNT = 0;
	tExit2_Timer = gMainTimerCnt_10m;

	JogDir = param[0];

	printf("MenuTrip JogWheel %d\r\n",JogDir);

	if((JogDir == JOG_DIAL_ROT_RIGHT) && (M2_1TimeFlag == 1))
	//if(JogDir == JOG_DIAL_ROT_RIGHT)
	{
		MenuTrip_Choice_Index++;
		if(MenuTrip_Choice_Index > 6)
			MenuTrip_Choice_Index = 1;
		
		memset(M2_PARA,0,2);
		itoa(MenuTrip_Choice_Index,M2_PARA,10);
		
		printf("wheel CW Branch Trip : %s\r\n",M2_PARA);
		
		param[0] = 0;
		M2_JogTimer = gMainTimerCnt_10m;
		M2_1TimeFlag = 0;
	}
	else if((JogDir == JOG_DIAL_ROT_LEFT) && (M2_1TimeFlag == 1))
	//else if(JogDir == JOG_DIAL_ROT_LEFT)
	{
		if(MenuTrip_Choice_Index > 1)
			MenuTrip_Choice_Index--;
		else if(MenuTrip_Choice_Index == 1)
			MenuTrip_Choice_Index = 6;
		
		memset(M2_PARA,0,2);
		itoa(MenuTrip_Choice_Index,M2_PARA,10);
		
		printf("wheel ccw Branch Trip : %s\r\n",M2_PARA);
		
		param[0] = 0;
		M2_JogTimer = gMainTimerCnt_10m;
		M2_1TimeFlag = 0;
	}

    return true;
}

bool M2_JogButton(ITUWidget* widget, char* param)
{
	uint8_t JogButton = 0;
	uint8_t tTripIndex = 0;
	
	M2_ExitCNT = 0;
	tExit2_Timer = gMainTimerCnt_10m;

	JogButton = param[0];
	printf("MenuTrip JogButton %d\r\n",JogButton);

	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
		{
			switch(JogButton)
			{
				case JOG_DIAL_KEY_UP:
				{
					uint8_t tDest1[8]={0,6,1,2,3,4,5,0};

					tTripIndex = tDest1[MenuTrip_Choice_Index];
/*					
					if(MenuTrip_Choice_Index > 1)
						MenuTrip_Choice_Index--;
					else if(MenuTrip_Choice_Index == 1)
						MenuTrip_Choice_Index = 6;
*/				
					memset(M2_PARA,0,2);
					itoa(tTripIndex,M2_PARA,10);
				
					printf("buton up Branch Trip : %s\r\n",M2_PARA);
					M2_MouseDn(NULL, M2_PARA);
					usleep(10*1000);
					M2_MouseUp(NULL, M2_PARA);
					param[0] = 0;
				}
				break;

				case JOG_DIAL_KEY_DOWN:
				{
					uint8_t tDest2[8]={0,2,3,4,5,6,1,0};

					tTripIndex = tDest2[MenuTrip_Choice_Index];
/*
					MenuTrip_Choice_Index++;
					if(MenuTrip_Choice_Index > 6)
						MenuTrip_Choice_Index = 1;
*/				
					memset(M2_PARA,0,2);
					itoa(tTripIndex,M2_PARA,10);
				
					printf("buton down Branch Trip : %s\r\n",M2_PARA);
					M2_MouseDn(NULL, M2_PARA);
					usleep(10*1000);
					M2_MouseUp(NULL, M2_PARA);
					param[0] = 0;
				}
				break;
/*				
				case JOG_DIAL_KEY_LEFT:
				case JOG_DIAL_KEY_RIGHT:

				break;
*/
				case JOG_DIAL_KEY_ENTER_ON:

					memset(M2_PARA,0,2);
					itoa(MenuTrip_Choice_Index,M2_PARA,10);
				
					printf("buton enter 3Sec Check : %s\r\n",M2_PARA);

					M2D7_MouseDn(NULL, M2_PARA);

				break;
				
				case JOG_DIAL_KEY_ENTER_OFF:

					memset(M2_PARA,0,2);
					itoa(MenuTrip_Choice_Index,M2_PARA,10);
				
					printf("buton enter Exit : %s\r\n",M2_PARA);

					M2D7_MouseUp(NULL, M2_PARA);

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
					// 1>3,2>1,3>2,4>6,5>4,6>5
					uint8_t tDest3[8]={0,3,1,2,6,4,5,0};

					tTripIndex = tDest3[MenuTrip_Choice_Index];

					memset(M2_PARA,0,2);
					itoa(tTripIndex,M2_PARA,10);
				
					printf("buton up Branch Trip : %s\r\n",M2_PARA);
					M2_MouseDn(NULL, M2_PARA);
					usleep(10*1000);
					M2_MouseUp(NULL, M2_PARA);
					param[0] = 0;
				}
				break;

				case JOG_DIAL_KEY_DOWN:
				{
					// 1>2,2>3,3>1,4>5,5>6,6>4
					uint8_t tDest4[8]={0,2,3,1,5,6,4,0};

					tTripIndex = tDest4[MenuTrip_Choice_Index];

					memset(M2_PARA,0,2);
					itoa(tTripIndex,M2_PARA,10);
				
					printf("buton down Branch Trip : %s\r\n",M2_PARA);
					M2_MouseDn(NULL, M2_PARA);
					usleep(10*1000);
					M2_MouseUp(NULL, M2_PARA);
					param[0] = 0;
				}
				break;

				case JOG_DIAL_KEY_LEFT:
				case JOG_DIAL_KEY_RIGHT:
				{
					// 1>4,2>5,3>6,4>1,5>2,6>3
					uint8_t tDest5[8]={0,4,5,6,1,2,3,0};

					tTripIndex = tDest5[MenuTrip_Choice_Index];

					memset(M2_PARA,0,2);
					itoa(tTripIndex,M2_PARA,10);
				
					printf("buton down Branch Trip : %s\r\n",M2_PARA);
					M2_MouseDn(NULL, M2_PARA);
					usleep(10*1000);
					M2_MouseUp(NULL, M2_PARA);
					param[0] = 0;
				}
				break;

				case JOG_DIAL_KEY_ENTER_ON:

					memset(M2_PARA,0,2);
					itoa(MenuTrip_Choice_Index,M2_PARA,10);
				
					printf("buton enter 3Sec Check : %s\r\n",M2_PARA);

					M2D7_MouseDn(NULL, M2_PARA);

				break;
				
				case JOG_DIAL_KEY_ENTER_OFF:

					memset(M2_PARA,0,2);
					itoa(MenuTrip_Choice_Index,M2_PARA,10);
				
					printf("buton enter Exit : %s\r\n",M2_PARA);

					M2D7_MouseUp(NULL, M2_PARA);

				break;
			}
		}
		break;
	}

    return true;
}

bool M2_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t JogSwitch = 0;

	JogSwitch = param[0];

	M2_ExitCNT = 0;
	tExit2_Timer = gMainTimerCnt_10m;

	if(JogSwitch == JOG_DIAL_SW_ESC_ON_OFF)			// jhlee 231006
	{
		//Menu_Index = Menu_Top;
		GotoMenuTop();
	}
	else if(JogSwitch == JOG_DIAL_SW_HOME_OFF)		// jhlee 231006
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}

    return true;
}

bool M2D7_MouseUp(ITUWidget* widget, char* param)
{
	M2_ExitCNT = 0;
	tExit2_Timer = gMainTimerCnt_10m;

	switch(MenuTrip_Choice_Index)
	{
		case 1:
		{
			ITUWidget *M2_1_P = ituSceneFindWidget(&theScene, "M2_1_P");
			assert(M2_1_P);

			ituWidgetSetVisible(M2_1_P,false);
			
			//MenuTrip_Choice_Index = 1;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 100;
		}
		break;

		case 2:
		{
			ITUWidget *M2_2_P = ituSceneFindWidget(&theScene, "M2_2_P");
			assert(M2_2_P);

			ituWidgetSetVisible(M2_2_P,false);
			
			//MenuTrip_Choice_Index = 2;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 100;
		}
		break;

		case 3:
		{
			ITUWidget *M2_3_P = ituSceneFindWidget(&theScene, "M2_3_P");
			assert(M2_3_P);

			ituWidgetSetVisible(M2_3_P,false);
			
			//MenuTrip_Choice_Index = 3;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 100;
		}
		break;

		case 4:
		{
			ITUWidget *M2_4_P = ituSceneFindWidget(&theScene, "M2_4_P");
			assert(M2_4_P);

			ituWidgetSetVisible(M2_4_P,false);
			
			//MenuTrip_Choice_Index = 4;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 100;
		}
		break;

		case 5:
		{
			ITUWidget *M2_5_P = ituSceneFindWidget(&theScene, "M2_5_P");
			assert(M2_5_P);

			ituWidgetSetVisible(M2_5_P,false);
			
			//MenuTrip_Choice_Index = 5;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 100;
		}
		break;

		case 6:
		{
			ITUWidget *M2_6_P = ituSceneFindWidget(&theScene, "M2_6_P");
			assert(M2_6_P);

			ituWidgetSetVisible(M2_6_P,false);
			
			//MenuTrip_Choice_Index = 6;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 100;
		}
		break;
	}

    return true;
}

bool M2D7_MouseDn(ITUWidget* widget, char* param)
{
	//M2_3SecCNT = 0;
	M2_ExitCNT = 0;
	tExit2_Timer = gMainTimerCnt_10m;

	switch(MenuTrip_Choice_Index)
	{
		case 1:
		{
			ITUWidget *M2_1_P = ituSceneFindWidget(&theScene, "M2_1_P");
			assert(M2_1_P);

			ituWidgetSetVisible(M2_1_P,true);
			
			//MenuTrip_Choice_Index = 1;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 0;
		}
		break;

		case 2:
		{
			ITUWidget *M2_2_P = ituSceneFindWidget(&theScene, "M2_2_P");
			assert(M2_2_P);

			ituWidgetSetVisible(M2_2_P,true);
			
			//MenuTrip_Choice_Index = 2;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 0;
		}
		break;

		case 3:
		{
			ITUWidget *M2_3_P = ituSceneFindWidget(&theScene, "M2_3_P");
			assert(M2_3_P);

			ituWidgetSetVisible(M2_3_P,true);
			
			//MenuTrip_Choice_Index = 3;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 0;
		}
		break;

		case 4:
		{
			ITUWidget *M2_4_P = ituSceneFindWidget(&theScene, "M2_4_P");
			assert(M2_4_P);

			ituWidgetSetVisible(M2_4_P,true);
			
			//MenuTrip_Choice_Index = 4;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 0;
		}
		break;

		case 5:
		{
			ITUWidget *M2_5_P = ituSceneFindWidget(&theScene, "M2_5_P");
			assert(M2_5_P);

			ituWidgetSetVisible(M2_5_P,true);
			
			//MenuTrip_Choice_Index = 5;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 0;
		}
		break;

		case 6:
		{
			ITUWidget *M2_6_P = ituSceneFindWidget(&theScene, "M2_6_P");
			assert(M2_6_P);

			ituWidgetSetVisible(M2_6_P,true);
			
			//MenuTrip_Choice_Index = 6;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 0;
		}
		break;
	}

    return true;
}

bool M2D6_MouseUp(ITUWidget* widget, char* param)
{
	GotoMain(MAIN_MODE_NORMAL); 
	
    return true;
}

bool M2D5_MouseUp(ITUWidget* widget, char* param)
{
	GotoMenuTop();
	
    return true;
}

bool M2_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tTripNum = 0;

	M2_ExitCNT = 0;
	tExit2_Timer = gMainTimerCnt_10m;
	
	tTripNum = (uint8_t)atoi(param);

	M2_ChoiceTripClear();
	M2_ChoiceDnTripClear();
	switch(tTripNum)
	{
		case 1:
		{
			ITUWidget *M2_1_P = ituSceneFindWidget(&theScene, "M2_1_P");

			ituWidgetSetVisible(M2_1_SEL,true);
			ituWidgetSetVisible(M2_1_P,true);
			
			MenuTrip_Choice_Index = 1;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 0;
		}
		break;

		case 2:
		{
			ITUWidget *M2_2_P = ituSceneFindWidget(&theScene, "M2_2_P");

			ituWidgetSetVisible(M2_2_SEL,true);
			ituWidgetSetVisible(M2_2_P,true);
			
			MenuTrip_Choice_Index = 2;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 0;
		}
		break;

		case 3:
		{
			ITUWidget *M2_3_P = ituSceneFindWidget(&theScene, "M2_3_P");

			ituWidgetSetVisible(M2_3_SEL,true);
			ituWidgetSetVisible(M2_3_P,true);
			
			MenuTrip_Choice_Index = 3;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 0;
		}
		break;

		case 4:
		{
			ITUWidget *M2_4_P = ituSceneFindWidget(&theScene, "M2_4_P");

			ituWidgetSetVisible(M2_4_SEL,true);
			ituWidgetSetVisible(M2_4_P,true);
			
			MenuTrip_Choice_Index = 4;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 0;
		}
		break;

		case 5:
		{
			ITUWidget *M2_5_P = ituSceneFindWidget(&theScene, "M2_5_P");

			ituWidgetSetVisible(M2_5_SEL,true);
			ituWidgetSetVisible(M2_5_P,true);
			
			MenuTrip_Choice_Index = 5;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 0;
		}
		break;

		case 6:
		{
			ITUWidget *M2_6_P = ituSceneFindWidget(&theScene, "M2_6_P");

			ituWidgetSetVisible(M2_6_SEL,true);
			ituWidgetSetVisible(M2_6_P,true);
			
			MenuTrip_Choice_Index = 6;
			M2_BlinkFlag = 1;
			M2_BlinkTimer = gMainTimerCnt_10m;
			M2_3SecCNT = 0;
		}
		break;
	}
	
    return true;
}

bool M2_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tTripNum = 0;

	M2_ExitCNT = 0;
	tExit2_Timer = gMainTimerCnt_10m;
	
	tTripNum = (uint8_t)atoi(param);
	
	switch(tTripNum)
	{
		case 1:
		{
			ITUWidget *M2_1_P = ituSceneFindWidget(&theScene, "M2_1_P");
			assert(M2_1_P);

			ituWidgetSetVisible(M2_1_P,false);
			
			if(ituWidgetIsVisible(M2_1_SEL))
			{
				MenuTrip_Choice_Index = 1;
				M2_BlinkFlag = 1;
				M2_BlinkTimer = gMainTimerCnt_10m;
				M2_3SecCNT = 100;
			}
		}
		break;

		case 2:
		{
			ITUWidget *M2_2_P = ituSceneFindWidget(&theScene, "M2_2_P");
			assert(M2_2_P);

			ituWidgetSetVisible(M2_2_P,false);

			if(ituWidgetIsVisible(M2_2_SEL))
			{
				MenuTrip_Choice_Index = 2;
				M2_BlinkFlag = 1;
				M2_BlinkTimer = gMainTimerCnt_10m;
				M2_3SecCNT = 100;
			}
		}
		break;

		case 3:
		{
			ITUWidget *M2_3_P = ituSceneFindWidget(&theScene, "M2_3_P");
			assert(M2_3_P);

			ituWidgetSetVisible(M2_3_P,false);
			
			if(ituWidgetIsVisible(M2_3_SEL))
			{
				MenuTrip_Choice_Index = 3;
				M2_BlinkFlag = 1;
				M2_BlinkTimer = gMainTimerCnt_10m;
				M2_3SecCNT = 100;
			}
		}
		break;

		case 4:
		{
			ITUWidget *M2_4_P = ituSceneFindWidget(&theScene, "M2_4_P");
			assert(M2_4_P);

			ituWidgetSetVisible(M2_4_P,false);
			
			if(ituWidgetIsVisible(M2_4_SEL))
			{
				MenuTrip_Choice_Index = 4;
				M2_BlinkFlag = 1;
				M2_BlinkTimer = gMainTimerCnt_10m;
				M2_3SecCNT = 100;
			}
		}
		break;

		case 5:
		{
			ITUWidget *M2_5_P = ituSceneFindWidget(&theScene, "M2_5_P");
			assert(M2_5_P);

			ituWidgetSetVisible(M2_5_P,false);
			
			if(ituWidgetIsVisible(M2_5_SEL))
			{
				MenuTrip_Choice_Index = 5;
				M2_BlinkFlag = 1;
				M2_BlinkTimer = gMainTimerCnt_10m;
				M2_3SecCNT = 100;
			}
		}
		break;

		case 6:
		{
			ITUWidget *M2_6_P = ituSceneFindWidget(&theScene, "M2_6_P");
			assert(M2_6_P);

			ituWidgetSetVisible(M2_6_P,false);
			
			if(ituWidgetIsVisible(M2_6_SEL))
			{
				MenuTrip_Choice_Index = 6;
				M2_BlinkFlag = 1;
				M2_BlinkTimer = gMainTimerCnt_10m;
				M2_3SecCNT = 100;
			}
		}
		break;
	}

    return true;
}

bool M2_UserDisplay(void)
{
	uint8_t tDestName[100];

	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
		return 0;

	if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
	{
		ituWidgetSetVisible(M2_TopStandard,false);
		ituWidgetSetVisible(M2_TopAdmin,true);
	}
	else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
	{
		ituWidgetSetVisible(M2_TopAdmin,false);
		ituWidgetSetVisible(M2_TopStandard,true);
	}
	else
	{
		ituWidgetSetVisible(M2_TopAdmin,false);
		ituWidgetSetVisible(M2_TopStandard,false);
	}

	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M2_TopUserName, tDestName);

		ituWidgetSetVisible(M2_TopUserName,true);
	}
	else
	{
		ituWidgetSetVisible(M2_TopUserName,false);
	}
/*
	if((gMainTimerCnt_10m - gMenu_UI_Time) < 500)		// 5sec	
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M2_TopUserName, tDestName);

		ituWidgetSetVisible(M2_TopUserName,true);
	}
	else if(((gMainTimerCnt_10m - gMenu_UI_Time) >= 500)||(M7_UserIndex > (MAX_SU100_USER_NUM-1)))
	{
		ituWidgetSetVisible(M2_TopUserName,false);
	}
*/	
}

void M2_ChoiceTripClear(void)
{
	if(ituWidgetIsVisible(M2_1_SEL) == true)
	{
		ituWidgetSetVisible(M2_1_SEL,false);

		if(Trip_NAFlag[0] == 0)
		{
			ituWidgetSetVisible(M2_1T_NA,false);
			ituWidgetSetVisible(M2_1T_Value,true);
		}
		else if(Trip_NAFlag[0] == 1)
		{
			ituWidgetSetVisible(M2_1T_Value,false);
			ituWidgetSetVisible(M2_1T_NA,true);
		}
	}	
	
	if(ituWidgetIsVisible(M2_2_SEL) == true)
	{
		ituWidgetSetVisible(M2_2_SEL,false);

		if(Trip_NAFlag[1] == 0)
		{
			ituWidgetSetVisible(M2_2T_NA,false);
			ituWidgetSetVisible(M2_2T_Value,true);
		}
		else if(Trip_NAFlag[1] == 1)
		{
			ituWidgetSetVisible(M2_2T_Value,false);
			ituWidgetSetVisible(M2_2T_NA,true);
		}
	}	

	if(ituWidgetIsVisible(M2_3_SEL) == true)
	{
		ituWidgetSetVisible(M2_3_SEL,false);

		if(Trip_NAFlag[2] == 0)
		{
			ituWidgetSetVisible(M2_3T_NA,false);
			ituWidgetSetVisible(M2_3T_Value,true);
		}
		else if(Trip_NAFlag[2] == 1)
		{
			ituWidgetSetVisible(M2_3T_Value,false);
			ituWidgetSetVisible(M2_3T_NA,true);
		}
	}	

	if(ituWidgetIsVisible(M2_4_SEL) == true)
	{
		ituWidgetSetVisible(M2_4_SEL,false);

		if(Trip_NAFlag[3] == 0)
		{
			ituWidgetSetVisible(M2_4T_NA,false);
			ituWidgetSetVisible(M2_4T_Value,true);
		}
		else if(Trip_NAFlag[3] == 1)
		{
			ituWidgetSetVisible(M2_4T_Value,false);
			ituWidgetSetVisible(M2_4T_NA,true);
		}
	}	

	if(ituWidgetIsVisible(M2_5_SEL) == true)
	{
		ituWidgetSetVisible(M2_5_SEL,false);

		if(Trip_NAFlag[4] == 0)
		{
			ituWidgetSetVisible(M2_5T_NA,false);
			ituWidgetSetVisible(M2_5T_Value,true);
		}
		else if(Trip_NAFlag[4] == 1)
		{
			ituWidgetSetVisible(M2_5T_Value,false);
			ituWidgetSetVisible(M2_5T_NA,true);
		}
	}	

	if(ituWidgetIsVisible(M2_6_SEL) == true)
	{
		ituWidgetSetVisible(M2_6_SEL,false);

		if(Trip_NAFlag[5] == 0)
		{
			ituWidgetSetVisible(M2_6T_NA,false);
			ituWidgetSetVisible(M2_6T_Value,true);
		}
		else if(Trip_NAFlag[5] == 1)
		{
			ituWidgetSetVisible(M2_6T_Value,false);
			ituWidgetSetVisible(M2_6T_NA,true);
		}
	}	
}

void M2_ChoiceDnTripClear(void)
{
	ITUWidget *M2_1_P = ituSceneFindWidget(&theScene, "M2_1_P");
	ITUWidget *M2_2_P = ituSceneFindWidget(&theScene, "M2_2_P");
	ITUWidget *M2_3_P = ituSceneFindWidget(&theScene, "M2_3_P");
	ITUWidget *M2_4_P = ituSceneFindWidget(&theScene, "M2_4_P");
	ITUWidget *M2_5_P = ituSceneFindWidget(&theScene, "M2_5_P");
	ITUWidget *M2_6_P = ituSceneFindWidget(&theScene, "M2_6_P");

	if(ituWidgetIsVisible(M2_1_P) == true)
	{
		ituWidgetSetVisible(M2_1_P,false);

		if(Trip_NAFlag[0] == 0)
		{
			ituWidgetSetVisible(M2_1T_NA,false);
			ituWidgetSetVisible(M2_1T_Value,true);
		}
		else if(Trip_NAFlag[0] == 1)
		{
			ituWidgetSetVisible(M2_1T_Value,false);
			ituWidgetSetVisible(M2_1T_NA,true);
		}
	}	
	
	if(ituWidgetIsVisible(M2_2_P) == true)
	{
		ituWidgetSetVisible(M2_2_P,false);

		if(Trip_NAFlag[1] == 0)
		{
			ituWidgetSetVisible(M2_2T_NA,false);
			ituWidgetSetVisible(M2_2T_Value,true);
		}
		else if(Trip_NAFlag[1] == 1)
		{
			ituWidgetSetVisible(M2_2T_Value,false);
			ituWidgetSetVisible(M2_2T_NA,true);
		}
	}	

	if(ituWidgetIsVisible(M2_3_P) == true)
	{
		ituWidgetSetVisible(M2_3_P,false);

		if(Trip_NAFlag[2] == 0)
		{
			ituWidgetSetVisible(M2_3T_NA,false);
			ituWidgetSetVisible(M2_3T_Value,true);
		}
		else if(Trip_NAFlag[2] == 1)
		{
			ituWidgetSetVisible(M2_3T_Value,false);
			ituWidgetSetVisible(M2_3T_NA,true);
		}
	}	

	if(ituWidgetIsVisible(M2_4_P) == true)
	{
		ituWidgetSetVisible(M2_4_P,false);

		if(Trip_NAFlag[3] == 0)
		{
			ituWidgetSetVisible(M2_4T_NA,false);
			ituWidgetSetVisible(M2_4T_Value,true);
		}
		else if(Trip_NAFlag[3] == 1)
		{
			ituWidgetSetVisible(M2_4T_Value,false);
			ituWidgetSetVisible(M2_4T_NA,true);
		}
	}	

	if(ituWidgetIsVisible(M2_5_P) == true)
	{
		ituWidgetSetVisible(M2_5_P,false);

		if(Trip_NAFlag[4] == 0)
		{
			ituWidgetSetVisible(M2_5T_NA,false);
			ituWidgetSetVisible(M2_5T_Value,true);
		}
		else if(Trip_NAFlag[4] == 1)
		{
			ituWidgetSetVisible(M2_5T_Value,false);
			ituWidgetSetVisible(M2_5T_NA,true);
		}
	}	

	if(ituWidgetIsVisible(M2_6_P) == true)
	{
		ituWidgetSetVisible(M2_6_P,false);

		if(Trip_NAFlag[5] == 0)
		{
			ituWidgetSetVisible(M2_6T_NA,false);
			ituWidgetSetVisible(M2_6T_Value,true);
		}
		else if(Trip_NAFlag[5] == 1)
		{
			ituWidgetSetVisible(M2_6T_Value,false);
			ituWidgetSetVisible(M2_6T_NA,true);
		}
	}	
}

void M2_ValueUpdate(void)
{
	char ptTripValue[15];
	
	if(Trip1_Value == 0xFFFFFFFF)
		Trip1_Value = 0xFFFFFFFF;
	else if(Trip1_Value > 999999)
		Trip1_Value = 999999;
	
	if(Trip1_Value <= 999999)
	{
		memset(ptTripValue,0,15);
		sprintf(ptTripValue,"%5d.%d",Trip1_Value/10,Trip1_Value%10);
		
		Trip_NAFlag[0] = 0;
		ituTextSetString(M2_1T_Value, ptTripValue);
	}
	else if(Trip1_Value == 0xFFFFFFFF)
	{
		memset(ptTripValue,0,15);
		sprintf(ptTripValue,"N/A");

		Trip_NAFlag[0] = 1;
		ituTextSetString(M2_1T_NA, ptTripValue);
	}

	if(Trip2_Value == 0xFFFFFFFF)
		Trip2_Value = 0xFFFFFFFF;
	else if(Trip2_Value > 999999)
		Trip2_Value = 999999;

	if(Trip2_Value <= 999999)
	{
		memset(ptTripValue,0,15);
		sprintf(ptTripValue,"%5d.%d",Trip2_Value/10,Trip2_Value%10);
		
		Trip_NAFlag[1] = 0;
		ituTextSetString(M2_2T_Value, ptTripValue);
	}
	else if(Trip2_Value == 0xFFFFFFFF)
	{
		memset(ptTripValue,0,15);
		sprintf(ptTripValue,"N/A");

		Trip_NAFlag[1] = 1;
		ituTextSetString(M2_2T_NA, ptTripValue);
	}

	if(Trip3_Value == 0xFFFFFFFF)
		Trip3_Value = 0xFFFFFFFF;
	else if(Trip3_Value > 999999)
		Trip3_Value = 999999;

	if(Trip3_Value <= 999999)
	{
		memset(ptTripValue,0,15);
		sprintf(ptTripValue,"%5d.%d",Trip3_Value/10,Trip3_Value%10);

		Trip_NAFlag[2] = 0;
		ituTextSetString(M2_3T_Value, ptTripValue);
	}
	else if(Trip3_Value == 0xFFFFFFFF)
	{
		memset(ptTripValue,0,15);
		sprintf(ptTripValue,"N/A");

		Trip_NAFlag[2] = 1;
		ituTextSetString(M2_3T_NA, ptTripValue);
	}

	if(Trip4_Value == 0xFFFFFFFF)
		Trip4_Value = 0xFFFFFFFF;
	else if(Trip4_Value > 999999)
		Trip4_Value = 999999;

	if(Trip4_Value <= 999999)
	{
		memset(ptTripValue,0,15);
		sprintf(ptTripValue,"%5d.%d",Trip4_Value/10,Trip4_Value%10);

		Trip_NAFlag[3] = 0;
		ituTextSetString(M2_4T_Value, ptTripValue);
	}
	else if(Trip4_Value == 0xFFFFFFFF)
	{
		memset(ptTripValue,0,15);
		sprintf(ptTripValue,"N/A");

		Trip_NAFlag[3] = 1;
		ituTextSetString(M2_4T_NA, ptTripValue);
	}

	if(Trip5_Value == 0xFFFFFFFF)
		Trip5_Value = 0xFFFFFFFF;
	else if(Trip5_Value > 999999)
		Trip5_Value = 999999;

	if(Trip5_Value <= 999999)
	{
		memset(ptTripValue,0,15);
		sprintf(ptTripValue,"%5d.%d",Trip5_Value/10,Trip5_Value%10);

		Trip_NAFlag[4] = 0;
		ituTextSetString(M2_5T_Value, ptTripValue);
	}
	else if(Trip5_Value == 0xFFFFFFFF)
	{
		memset(ptTripValue,0,15);
		sprintf(ptTripValue,"N/A");

		Trip_NAFlag[4] = 1;
		ituTextSetString(M2_5T_NA, ptTripValue);
	}

	if(Trip6_Value == 0xFFFFFFFF)
		Trip6_Value = 0xFFFFFFFF;
	else if(Trip6_Value > 999999)
		Trip6_Value = 999999;

	if(Trip6_Value <= 999999)
	{
		memset(ptTripValue,0,15);
		sprintf(ptTripValue,"%5d.%d",Trip6_Value/10,Trip6_Value%10);

		Trip_NAFlag[5] = 0;
		ituTextSetString(M2_6T_Value, ptTripValue);
	}
	else if(Trip6_Value == 0xFFFFFFFF)
	{
		memset(ptTripValue,0,15);
		sprintf(ptTripValue,"N/A");

		Trip_NAFlag[5] = 1;
		ituTextSetString(M2_6T_NA, ptTripValue);
	}
}


