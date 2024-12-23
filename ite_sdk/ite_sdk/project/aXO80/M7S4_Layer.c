#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

/* widgets:
M7_S4
M7S4_BG
M7S4_D7_BTN
M7S4_D6_BTN
M7S4_D5_BTN
M7S4_G_Value
M7S4_G_BTN
M7S4_G_SEL
M7S4_2_BTN
M7S4_1_BTN
S4_FSerr_ESP
S4_FSx_ESP
S4_FSs_ESP
S4_FSerr_ITA
S4_FSx_ITA
S4_FSs_ITA
S4_FSerr_FRA
S4_FSx_FRA
S4_FSs_FRA
S4_FSerr_DEU
S4_FSx_DEU
S4_FSs_DEU
S4_FSerr_ENG
S4_FSx_ENG
S4_FSs_ENG
S4_FS_BTN
S4_FS_SEL
*/

uint8_t	AG_OnTime = 0;				// CAN[AG_I] 4byte
uint8_t AG_ActiveStatus = 0;	// CAN[AG_I] 0byte,0bit 0:Not Active , 1:Active On
uint8_t AG_SupplyFlag = 0;		// CAN[AG_I] 1byte,0bit 0:No Supply(STOP) , 1:Force Supply(WORK)
uint8_t AG_ErrorStatus = 0;		// CAN[AG_I] 1byte,6bit 0:Normal , 1:Error
uint8_t AG_MaxLimit = 250;		// CAN[AG_I] 2byte
uint8_t AG_MinLimit = 0;			// CAN[AG_I] 3byte

uint8_t AG_Time = 0;			// modify uint32_t -> uint8_t (min) for AG_S
bool AG_Start = 0;
bool AG_Stop = 0;
uint8_t S4_2SecFlag = 0;


static uint8_t cAG_OnTime = 0;				// use CAN_Tx[AG_S] 2byte
static uint8_t cAGST_StartFlag = 0;		// use CAN_Tx[AG_ST] 0byte,0bit
static uint8_t cAGST_StopFlag = 0;		// use CAN_Tx[AG_ST] 0byte,1bit
uint32_t cAG_SetTimer = 0;						// 2Sec timer AG_S

static uint8_t cAG_SelIdx = 0;
static uint8_t cAG_FSIdx = 0;			// FS index flag ; 0:on , 1:off , 2:Error

static ITUText *M7S4_G_Value;

static uint32_t S4_SecTimer;
static uint8_t S4_ExitCNT = 0;

static uint32_t S4_BlinkTimer;
static uint8_t S4_1SecCNT = 0;
static uint8_t S4_2SecCNT = 0;
static uint8_t S4_BlinkFlag = 0;

static uint32_t S4_FastTimer;
static uint8_t S4_FastFlag = 0;
static uint8_t S4_FastSource = 0;
static uint8_t S4_SkipFlag = 0;


void AG_OnTIme_Update(uint8_t tOnTime,bool tDisp_Opt);
void AG_FS_Display(uint8_t tFSIdx);
void AG_FS_Clear(void);

bool M7S4_UserDisplay(void);

extern void GotoMenuTop(void);
extern void GotoMenu7(void);


bool M7S4_TouchUP_Restet(void)
{
	S4_1SecCNT = 100;
	S4_FastFlag = 0;
	S4_FastSource = 0;

	S4_SecTimer = gMainTimerCnt_10m;
}


bool M7S4_Enter(ITUWidget* widget, char* param)
{
	ITUWidget *M7S4_G_SEL = ituSceneFindWidget(&theScene, "M7S4_G_SEL");
	assert(M7S4_G_SEL);
	ITUWidget *S4_FS_SEL = ituSceneFindWidget(&theScene, "S4_FS_SEL");
	assert(S4_FS_SEL);

	M7S4_G_Value = ituSceneFindWidget(&theScene, "M7S4_G_Value");
	assert(M7S4_G_Value);

/*
	// dev.opt
	AG_OnTime = 10;				// CAN[AG_I] 4byte
	AG_ActiveStatus = 1;	// CAN[AG_I] 0byte,0bit 0:Off , 1:On
	AG_SupplyFlag = 0;		// CAN[AG_I] 1byte,0bit 0:No Supply(STOP) , 1:Force Supply(WORK)
	AG_ErrorStatus = 0;		// CAN[AG_I] 1byte,6bit 0:Normal , 1:Error
	AG_MaxLimit = 30;			// CAN[AG_I] 2byte
	AG_MinLimit = 1;			// CAN[AG_I] 3byte
*/

	cAG_SelIdx = 1;				// select AG_Value fix
	ituWidgetSetVisible(S4_FS_SEL, false);
	ituWidgetSetVisible(M7S4_G_SEL, true);
	S4_BlinkFlag = 0;

	cAG_OnTime = AG_OnTime;

	AG_Time =	AG_OnTime;

	if(cAG_OnTime > AG_MaxLimit)
		cAG_OnTime = AG_MaxLimit;
	else if(cAG_OnTime < AG_MinLimit)
		cAG_OnTime = AG_MinLimit;
	
	AG_OnTIme_Update(cAG_OnTime,true);
	
	printf("Language = %d\r\n",Lang_Flag[M7_UserIndex]);

	//printf("AG_ErrorStatus = %d\r\n",AG_ErrorStatus);
	//printf("AG_SupplyFlag = %d\r\n",AG_SupplyFlag);
	//printf("AG_ActiveStatus = %d\r\n",AG_ActiveStatus);
	
	if(AG_ErrorStatus == 1)
		cAG_FSIdx = 2;							// Force supply Error , red
	else if(AG_ErrorStatus == 0)
	{
		if(AG_SupplyFlag == 1)
		{
			cAG_FSIdx = 0;		// green
		}
		else if(AG_SupplyFlag == 0)
		{
			if(AG_ActiveStatus == 1)
				cAG_FSIdx = 1;	// gray
			else if(AG_ActiveStatus == 0)
				cAG_FSIdx = 1;	// gray
		}
	}

	AG_FS_Clear();
	AG_FS_Display(cAG_FSIdx);

	M7S4_UserDisplay();

	S4_SecTimer = gMainTimerCnt_10m;
	S4_ExitCNT = 0;

	//fast / long key stop
	S4_2SecCNT = 100;
	S4_2SecFlag = 0;
	S4_1SecCNT = 100;
	S4_FastFlag = 0;
	S4_FastSource = 0;
		
    return true;
}

bool M7S4_Timer(ITUWidget* widget, char* param)
{
	static uint8_t tAG_FSIdx = 0;
	char ptAGOnTime[4]={0,0,0,0};

#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		return 0;
	}
#endif

	if((gMainTimerCnt_10m - S4_SecTimer) >= 100)
	{
		if(S4_ExitCNT < 30)
			S4_ExitCNT++;
		if(S4_ExitCNT == 30)
		{
			S4_ExitCNT = 100;
			printf("\r\nM7S4 Menu 30Sec Exit > goto Main\r\n");
			GotoMain(MAIN_MODE_NORMAL); 
		}

		if(S4_1SecCNT < 1)
			S4_1SecCNT++;
		if(S4_1SecCNT == 1)
		{
			S4_1SecCNT == 100;
			printf("\r\nS4 Long key\r\n");

			if(S4_FastFlag == 1)
			{
				S4_FastFlag = 2;
				S4_FastTimer = gMainTimerCnt_10m;
			}
		}
		
		if(S4_2SecCNT < 2)
			S4_2SecCNT++;
		else if(S4_2SecCNT == 2)
		{
			S4_2SecCNT = 100;
			S4_2SecFlag = 0;
			//printf("\r\nS4 AG_OnTime Value Resume\r\n");
			
			AG_Time = AG_OnTime;					// AG_OnTime == AG_I_4byte 
			cAG_OnTime = AG_OnTime;
			
			AG_OnTIme_Update(cAG_OnTime,true);
		}
			
		S4_SecTimer = gMainTimerCnt_10m;
	}

	if(S4_BlinkFlag == 1)		// on
	{
		if((gMainTimerCnt_10m - S4_BlinkTimer) >= 30) // 300msec
		{
			ituWidgetSetVisible(M7S4_G_Value, true);
				
			S4_BlinkFlag = 2;
			S4_BlinkTimer = gMainTimerCnt_10m;
		}
	}
	else if(S4_BlinkFlag == 2)	// off
	{
		if((gMainTimerCnt_10m - S4_BlinkTimer) >= 30) // 300msec
		{
			ituWidgetSetVisible(M7S4_G_Value, false);

			S4_BlinkFlag = 1;
			S4_BlinkTimer = gMainTimerCnt_10m;
		}
	}

	if(S4_FastFlag == 2)
	{
		// fast up/down count
		if((gMainTimerCnt_10m - S4_FastTimer) >= 10)
		{
			if(S4_FastSource == 1)		// down
			{
				if(cAG_OnTime > AG_MinLimit)
					cAG_OnTime--;
				else if(cAG_OnTime == AG_MinLimit)
					cAG_OnTime = AG_MinLimit;

				sprintf(ptAGOnTime,"%3d",cAG_OnTime);
				ituTextSetString(M7S4_G_Value, ptAGOnTime);
			}
			else if(S4_FastSource == 2)		// up
			{
				if(cAG_OnTime < AG_MaxLimit)
					cAG_OnTime++;
				else if(cAG_OnTime == AG_MaxLimit)
					cAG_OnTime = AG_MaxLimit;

				sprintf(ptAGOnTime,"%3d",cAG_OnTime);
				ituTextSetString(M7S4_G_Value, ptAGOnTime);
			}

			S4_FastTimer = gMainTimerCnt_10m;
		}
	}

	// AG_OnTime Limit Check
	if(cAG_OnTime > AG_MaxLimit)
	{
		cAG_OnTime = AG_MaxLimit;
		
		sprintf(ptAGOnTime,"%3d",cAG_OnTime);
		ituTextSetString(M7S4_G_Value, ptAGOnTime);
	}
	else if(cAG_OnTime < AG_MinLimit)
	{
		cAG_OnTime = AG_MinLimit;

		sprintf(ptAGOnTime,"%3d",cAG_OnTime);
		ituTextSetString(M7S4_G_Value, ptAGOnTime);
	}

	// FS Display
	if(AG_ErrorStatus == 1)
		cAG_FSIdx = 2;							// Force supply Error , red
	else if(AG_ErrorStatus == 0)
	{
		if(AG_SupplyFlag == 1)
		{
			cAG_FSIdx = 0;		// green
		}
		else if(AG_SupplyFlag == 0)
		{
			if(AG_ActiveStatus == 1)
				cAG_FSIdx = 1;	// gray
			else if(AG_ActiveStatus == 0)
				cAG_FSIdx = 1;	// gray
		}
	}

	if(tAG_FSIdx != cAG_FSIdx)
	{
		tAG_FSIdx = cAG_FSIdx;
		
		AG_FS_Clear();
		AG_FS_Display(cAG_FSIdx);
	}

	M7S4_UserDisplay();
		
    return true;
}

bool M7S4_JogWheel(ITUWidget* widget, char* param)
{
	ITUWidget *M7S4_G_SEL = ituSceneFindWidget(&theScene, "M7S4_G_SEL");
	assert(M7S4_G_SEL);
	ITUWidget *S4_FS_SEL = ituSceneFindWidget(&theScene, "S4_FS_SEL");
	assert(S4_FS_SEL);
	
	uint8_t tJogWheel = 0;

	S4_ExitCNT = 0;
	tJogWheel = param[0];

	printf("S4 JogWheel = %d\r\n",tJogWheel);

	if(tJogWheel == JOG_DIAL_ROT_LEFT)
	{
		if(S4_BlinkFlag == 0)
		{
			// no blinking
			if(cAG_SelIdx == 1)
			{
				cAG_SelIdx = 2;				// select FS button
				ituWidgetSetVisible(M7S4_G_SEL, false);
				ituWidgetSetVisible(S4_FS_SEL, true);
			}
			else if(cAG_SelIdx == 2)
			{
				cAG_SelIdx = 1;				// select AG Value
				ituWidgetSetVisible(S4_FS_SEL, false);
				ituWidgetSetVisible(M7S4_G_SEL, true);
			}
		}
		else if(S4_BlinkFlag != 0)
		{
			// blinking , AG OnTime down count
			if(cAG_OnTime > AG_MinLimit)
				cAG_OnTime--;
			else if(cAG_OnTime == AG_MinLimit)
				cAG_OnTime = AG_MinLimit;

			AG_OnTIme_Update(cAG_OnTime,true);
					
			// fast count stop
			S4_1SecCNT = 100;
			S4_FastFlag = 0;
			S4_FastSource = 0;
  
			S4_SecTimer = gMainTimerCnt_10m;	
		}
	}
	else if(tJogWheel == JOG_DIAL_ROT_RIGHT)
	{
		if(S4_BlinkFlag == 0)
		{
			// no blinking
			if(cAG_SelIdx == 1)
			{
				cAG_SelIdx = 2;				// select FS button
				ituWidgetSetVisible(M7S4_G_SEL, false);
				ituWidgetSetVisible(S4_FS_SEL, true);
			}
			else if(cAG_SelIdx == 2)
			{
				cAG_SelIdx = 1;				// select AG Value
				ituWidgetSetVisible(S4_FS_SEL, false);
				ituWidgetSetVisible(M7S4_G_SEL, true);
			}
		}
		else if(S4_BlinkFlag != 0)
		{
			// blinking , AG OnTime up count
			if(cAG_OnTime < AG_MaxLimit)
				cAG_OnTime++;
			else if(cAG_OnTime == AG_MaxLimit)
				cAG_OnTime = AG_MaxLimit;

			AG_OnTIme_Update(cAG_OnTime,true);

			// fast count stop
			S4_1SecCNT = 100;
			S4_FastFlag = 0;
			S4_FastSource = 0;
  
			S4_SecTimer = gMainTimerCnt_10m;	
		}
	}

    return true;
}

bool M7S4_JogButton(ITUWidget* widget, char* param)
{
	ITUWidget *M7S4_G_SEL = ituSceneFindWidget(&theScene, "M7S4_G_SEL");
	assert(M7S4_G_SEL);
	ITUWidget *S4_FS_SEL = ituSceneFindWidget(&theScene, "S4_FS_SEL");
	assert(S4_FS_SEL);

	uint8_t tJogButton = 0;
	
	S4_ExitCNT = 0;

	tJogButton = param[0];

	printf("S4 JogButton = %d\r\n",tJogButton);

	switch(tJogButton)
	{
		case JOG_DIAL_KEY_LEFT:
		{
			if(S4_BlinkFlag != 0)
			{
				// fast down count start
				S4_SecTimer = gMainTimerCnt_10m;
				S4_1SecCNT = 0;
				S4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

				S4_FastSource = 1;	// down
			}
		}
		break;

		case JOG_DIAL_KEY_DOWN:
		{
			if(S4_BlinkFlag == 0)
			{
				// select change
				if(cAG_SelIdx == 1)
				{
					cAG_SelIdx = 2;				// select FS button
					ituWidgetSetVisible(M7S4_G_SEL, false);
					ituWidgetSetVisible(S4_FS_SEL, true);
				}
				else if(cAG_SelIdx == 2)
				{
					cAG_SelIdx = 1;				// select AG Value
					ituWidgetSetVisible(S4_FS_SEL, false);
					ituWidgetSetVisible(M7S4_G_SEL, true);
				}
			}
			else if(S4_BlinkFlag != 0)
			{
				// fast down count start
				S4_SecTimer = gMainTimerCnt_10m;
				S4_1SecCNT = 0;
				S4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

				S4_FastSource = 1;	// down
			}
		}
		break;

		case JOG_DIAL_KEY_LEFT_OFF:
		case JOG_DIAL_KEY_DOWN_OFF:
		{
			if(S4_BlinkFlag != 0)
			{
				// AG OnTime Value down count
				if(cAG_OnTime > AG_MinLimit)
					cAG_OnTime--;
				else if(cAG_OnTime == AG_MinLimit)
					cAG_OnTime = AG_MinLimit;

				AG_OnTIme_Update(cAG_OnTime,true);

				// fast count stop
				S4_1SecCNT = 100;
				S4_FastFlag = 0;
				S4_FastSource = 0;
  
				S4_SecTimer = gMainTimerCnt_10m;	
			}
		}
		break;

		case JOG_DIAL_KEY_RIGHT:
		{
			if(S4_BlinkFlag != 0)
			{
				// fast up count start
				S4_SecTimer = gMainTimerCnt_10m;
				S4_1SecCNT = 0;
				S4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

				S4_FastSource = 2;	// up
			}			
		}
		break;

		case JOG_DIAL_KEY_UP:
		{
			if(S4_BlinkFlag == 0)
			{
				// select change
				if(cAG_SelIdx == 1)
				{
					cAG_SelIdx = 2;				// select FS button
					ituWidgetSetVisible(M7S4_G_SEL, false);
					ituWidgetSetVisible(S4_FS_SEL, true);
				}
				else if(cAG_SelIdx == 2)
				{
					cAG_SelIdx = 1;				// select AG Value
					ituWidgetSetVisible(S4_FS_SEL, false);
					ituWidgetSetVisible(M7S4_G_SEL, true);
				}
			}
			else if(S4_BlinkFlag != 0)
			{
				// fast up count start
				S4_SecTimer = gMainTimerCnt_10m;
				S4_1SecCNT = 0;
				S4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

				S4_FastSource = 2;	// up
			}
		}
		break;

		case JOG_DIAL_KEY_RIGHT_OFF:
		case JOG_DIAL_KEY_UP_OFF:
		{
			if(S4_BlinkFlag != 0)
			{
				// AG Value up count
				if(cAG_OnTime < AG_MaxLimit)
					cAG_OnTime++;
				else if(cAG_OnTime == AG_MaxLimit)
					cAG_OnTime = AG_MaxLimit;

				AG_OnTIme_Update(cAG_OnTime,true);

				// fast count stop
				S4_1SecCNT = 100;
				S4_FastFlag = 0;
				S4_FastSource = 0;
  
				S4_SecTimer = gMainTimerCnt_10m;	
			}			
		}
		break;

		case JOG_DIAL_KEY_ENTER_ON:
		{
			if(S4_BlinkFlag == 0 )
			{
				if(cAG_SelIdx == 1)
				{
					// edit AG_Ontime Value
					S4_BlinkFlag = 1;
					S4_BlinkTimer = gMainTimerCnt_10m;
				}
				else if(cAG_SelIdx == 2)
				{
					// FS button reaction
					if(AG_ErrorStatus == 0)
					{
						if(AG_SupplyFlag == 0)
						{
							//gray CAN Tx[AG_ST] 0byte,0bit
							printf("gray CAN Tx[AG_ST] 0byte,0bit\r\n");
							AG_Start = 1;
							AG_Stop = 0;
						}
						else if(AG_SupplyFlag == 1)
						{
							//green CAN Tx[AG_ST] 0byte,1bit
							printf("green CAN Tx[AG_ST] 0byte,1bit\r\n");
							AG_Start = 0;
							AG_Stop = 1;
						}
					}
				}
			}
			else if(S4_BlinkFlag != 0)
			{
				S4_BlinkFlag = 0;
				AG_OnTIme_Update(cAG_OnTime,true);
				
				//printf("S4 JOG Enter AG_S send\r\n");
				// CAN TX AG_S cAG_OnTime
				AG_Time = cAG_OnTime;
				// 2Sec timer start
				S4_2SecCNT = 0;
				S4_2SecFlag = 1;
			}
		}	
		break;
	}
    return true;
}

bool M7S4_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t tJogSwitch = 0;
	
	S4_ExitCNT = 0;

	tJogSwitch = param[0];

	printf("S4 JogSwitch = %d\r\n",tJogSwitch);

	if(tJogSwitch == JOG_DIAL_SW_ESC_ON)
	{
		if(S4_BlinkFlag != 0)
		{
			M7S4_TouchUP_Restet(); // add whlim

			// blink stop
			S4_BlinkFlag = 0;
		
			// edit value restore
			cAG_OnTime = AG_OnTime;

			if(cAG_OnTime > AG_MaxLimit)
				cAG_OnTime = AG_MaxLimit;
			else if(cAG_OnTime < AG_MinLimit)
				cAG_OnTime = AG_MinLimit;
	
			AG_OnTIme_Update(cAG_OnTime,true);
	
			S4_SkipFlag = 1;
		}
	}
	else if(tJogSwitch == JOG_DIAL_SW_ESC_ON_OFF)	
	{
		if(S4_SkipFlag == 1)
			S4_SkipFlag = 0;
		else if(S4_SkipFlag == 0)
			GotoMenu7();
	}
	else if(tJogSwitch == JOG_DIAL_SW_HOME)
	{
		GotoMain(MAIN_MODE_NORMAL);
	}
	
    return true;
}

bool M7S4_D7_MouseDn(ITUWidget* widget, char* param)
{
	S4_ExitCNT = 0;

    return true;
}

bool M7S4_D7_MouseUp(ITUWidget* widget, char* param)
{
	S4_ExitCNT = 0;

	if(S4_BlinkFlag != 0)
	{
		S4_BlinkFlag = 0;
		AG_OnTIme_Update(cAG_OnTime,true);
				
		//printf("S4 D7 up AG_S send\r\n");
		// CAN TX AG_S cAG_OnTime
		AG_Time = cAG_OnTime;
		// 2Sec timer start
		S4_2SecCNT = 0;
		S4_2SecFlag = 1;
	}
	else if(S4_BlinkFlag == 0)
	{
		if(cAG_SelIdx == 1)
		{
			// blink start > value edit mode
			//printf("S4 D7 up AG OnTIme Value Edit\r\n");
			S4_BlinkFlag = 1;
			S4_BlinkTimer = gMainTimerCnt_10m;
		}
		else if(cAG_SelIdx == 2)
		{
			printf("S4 D7 up FS Button\r\n");

			// FS button reaction
			if(AG_ErrorStatus == 0)
			{
				if(AG_SupplyFlag == 0)
				{
					//gray CAN Tx[AG_ST] 0byte,0bit
					printf("gray CAN Tx[AG_ST] 0byte,0bit\r\n");
					AG_Start = 1;
					AG_Stop = 0;
				}
				else if(AG_SupplyFlag == 1)
				{
					//green CAN Tx[AG_ST] 0byte,1bit
					printf("green CAN Tx[AG_ST] 0byte,1bit\r\n");
					AG_Start = 0;
					AG_Stop = 1;
				}
			}
		}
	}
	
    return true;
}

bool M7S4_D6_MouseUp(ITUWidget* widget, char* param)
{
	GotoMain(MAIN_MODE_NORMAL); 
	
    return true;
}

bool M7S4_D5_MouseDn(ITUWidget* widget, char* param)
{
	S4_ExitCNT = 0;

	if(S4_BlinkFlag != 0)
	{
		// blink stop
		S4_BlinkFlag = 0;
		
		// edit value restore
		cAG_OnTime = AG_OnTime;

		if(cAG_OnTime > AG_MaxLimit)
			cAG_OnTime = AG_MaxLimit;
		else if(cAG_OnTime < AG_MinLimit)
			cAG_OnTime = AG_MinLimit;
	
		AG_OnTIme_Update(cAG_OnTime,true);
	
		S4_SkipFlag = 1;
	}

    return true;
}

bool M7S4_D5_MouseUp(ITUWidget* widget, char* param)
{
	S4_ExitCNT = 0;
	
	if(S4_SkipFlag == 1)
		S4_SkipFlag = 0;
	else if(S4_SkipFlag == 0)
		GotoMenu7();
	
    return true;
}

bool M7S4_BTN_MouseDn(ITUWidget* widget, char* param)
{
	ITUWidget *M7S4_G_SEL = ituSceneFindWidget(&theScene, "M7S4_G_SEL");
	assert(M7S4_G_SEL);
	ITUWidget *S4_FS_SEL = ituSceneFindWidget(&theScene, "S4_FS_SEL");
	assert(S4_FS_SEL);

	uint8_t tBTN_Index;
	
	S4_ExitCNT = 0;
	
	tBTN_Index = (uint8_t)atoi(param);
	//printf("M7S4 BTN Down = %d\r\n",tBTN_Index);
	
	switch(tBTN_Index)
	{
		case 1:				// down
		{
			if(S4_BlinkFlag == 0)		// no blink
			{
				cAG_SelIdx = 1;				// select AG Value
				ituWidgetSetVisible(S4_FS_SEL, false);
				ituWidgetSetVisible(M7S4_G_SEL, true);
			}
			else if(S4_BlinkFlag != 0)
			{
				// fast count start
				S4_SecTimer = gMainTimerCnt_10m;
				S4_1SecCNT = 0;
				S4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

				S4_FastSource = 1;	// down
			}
		}
		break;
		
		case 2:				// up
		{
			if(S4_BlinkFlag == 0)		// no blink
			{
				cAG_SelIdx = 1;				// select AG Value

				ituWidgetSetVisible(S4_FS_SEL, false);
				ituWidgetSetVisible(M7S4_G_SEL, true);
			}
			else if(S4_BlinkFlag != 0)
			{
				// fast count start
				S4_SecTimer = gMainTimerCnt_10m;
				S4_1SecCNT = 0;
				S4_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

				S4_FastSource = 2;	// up
			}
		}
		break;

		case 4:				// FS sel
		{
			if(S4_BlinkFlag != 0)
			{
				S4_BlinkFlag = 0;
				AG_OnTIme_Update(cAG_OnTime,true);
				
				// CAN TX AG_S cAG_OnTime
				AG_Time = cAG_OnTime;
				// 2Sec timer start
				S4_2SecCNT = 0;
				S4_2SecFlag = 1;
			}
			
			cAG_SelIdx = 2;				// select FS button
			ituWidgetSetVisible(M7S4_G_SEL, false);
			ituWidgetSetVisible(S4_FS_SEL, true);
		}
		break;
	}

    return true;
}

bool M7S4_BTN_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tBTN_Index;

	S4_ExitCNT = 0;

	tBTN_Index = (uint8_t)atoi(param);
	//printf("M7S4 BTN Up = %d\r\n",tBTN_Index);
	
	switch(tBTN_Index)
	{
		case 1:				// down
			if(S4_BlinkFlag == 0)		// no blink
			{
				// blink start
				S4_BlinkTimer = gMainTimerCnt_10m;
				S4_BlinkFlag = 1;
				
			}
			else if(S4_BlinkFlag != 0)
			{
				// AG Value down count
				if(cAG_OnTime > AG_MinLimit)
					cAG_OnTime--;
				else if(cAG_OnTime == AG_MinLimit)
					cAG_OnTime = AG_MinLimit;

				AG_OnTIme_Update(cAG_OnTime,true);
					
				// fast count stop
				S4_1SecCNT = 100;
				S4_FastFlag = 0;
				S4_FastSource = 0;
  
				S4_SecTimer = gMainTimerCnt_10m;	
			}
		break;
		
		case 2:				// up
			if(S4_BlinkFlag == 0)		// no blink
			{
				// blink start
				S4_BlinkTimer = gMainTimerCnt_10m;
				S4_BlinkFlag = 1;
			}
			else if(S4_BlinkFlag != 0)
			{
				// AG Value up count
				if(cAG_OnTime < AG_MaxLimit)
					cAG_OnTime++;
				else if(cAG_OnTime == AG_MaxLimit)
					cAG_OnTime = AG_MaxLimit;

				AG_OnTIme_Update(cAG_OnTime,true);

				// fast count stop
				S4_1SecCNT = 100;
				S4_FastFlag = 0;
				S4_FastSource = 0;
  
				S4_SecTimer = gMainTimerCnt_10m;	
			}
		break;

		case 4:				// FS sel
		{
			if(AG_ErrorStatus == 0)
			{
				if(AG_SupplyFlag == 0)
				{
					//CAN Tx[AG_ST] 0byte,0bit
					AG_Start = 1;
					AG_Stop = 0;
				}
				else if(AG_SupplyFlag == 1)
				{
					//CAN Tx[AG_ST] 0byte,1bit
					AG_Start = 0;
					AG_Stop = 1;
				}
			}
		}
		
		break;
	}

    return true;
}

bool M7S4_UserDisplay(void)
{
	uint8_t tDestName[100];

	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
		return 0;

	ITUWidget *M7S4_TopAdmin = ituSceneFindWidget(&theScene, "M7S4_TopAdmin");
	assert(M7S4_TopAdmin);
	ITUWidget *M7S4_TopStandard = ituSceneFindWidget(&theScene, "M7S4_TopStandard");
	assert(M7S4_TopStandard);
	
	ITUText *M7S4_TopUserName = ituSceneFindWidget(&theScene, "M7S4_TopUserName");
	assert(M7S4_TopUserName);
	
	if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
	{
		ituWidgetSetVisible(M7S4_TopStandard,false);
		ituWidgetSetVisible(M7S4_TopAdmin,true);
	}
	else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
	{
		ituWidgetSetVisible(M7S4_TopAdmin,false);
		ituWidgetSetVisible(M7S4_TopStandard,true);
	}
	else
	{
		ituWidgetSetVisible(M7S4_TopAdmin,false);
		ituWidgetSetVisible(M7S4_TopStandard,false);
	}

	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M7S4_TopUserName, tDestName);

		ituWidgetSetVisible(M7S4_TopUserName,true);
	}
	else
	{
		ituWidgetSetVisible(M7S4_TopUserName,false);
	}
}

void AG_OnTIme_Update(uint8_t tOnTime,bool tDisp_Opt)
{
	char ptAGOnTime[4]={0,0,0,0};

	sprintf(ptAGOnTime,"%3d",tOnTime);
	ituTextSetString(M7S4_G_Value, ptAGOnTime);
	ituWidgetSetVisible(M7S4_G_Value, tDisp_Opt);
}

void AG_FS_Clear(void)
{
	ITUWidget *S4_FSs_ENG = ituSceneFindWidget(&theScene, "S4_FSs_ENG");
	assert(S4_FSs_ENG);
	if(ituWidgetIsVisible(S4_FSs_ENG) == true)
		ituWidgetSetVisible(S4_FSs_ENG, false);

	ITUWidget *S4_FSx_ENG = ituSceneFindWidget(&theScene, "S4_FSx_ENG");
	assert(S4_FSx_ENG);
	if(ituWidgetIsVisible(S4_FSx_ENG) == true)
		ituWidgetSetVisible(S4_FSx_ENG, false);

	ITUWidget *S4_FSerr_ENG = ituSceneFindWidget(&theScene, "S4_FSerr_ENG");
	assert(S4_FSerr_ENG);
	if(ituWidgetIsVisible(S4_FSerr_ENG) == true)
		ituWidgetSetVisible(S4_FSerr_ENG, false);

	ITUWidget *S4_FSs_DEU = ituSceneFindWidget(&theScene, "S4_FSs_DEU");
	assert(S4_FSs_DEU);
	if(ituWidgetIsVisible(S4_FSs_DEU) == true)
		ituWidgetSetVisible(S4_FSs_DEU, false);

	ITUWidget *S4_FSx_DEU = ituSceneFindWidget(&theScene, "S4_FSx_DEU");
	assert(S4_FSx_DEU);
	if(ituWidgetIsVisible(S4_FSx_DEU) == true)
		ituWidgetSetVisible(S4_FSx_DEU, false);

	ITUWidget *S4_FSerr_DEU = ituSceneFindWidget(&theScene, "S4_FSerr_DEU");
	assert(S4_FSerr_DEU);
	if(ituWidgetIsVisible(S4_FSerr_DEU) == true)
		ituWidgetSetVisible(S4_FSerr_DEU, false);
	
	ITUWidget *S4_FSs_FRA = ituSceneFindWidget(&theScene, "S4_FSs_FRA");
	assert(S4_FSs_FRA);
	if(ituWidgetIsVisible(S4_FSs_FRA) == true)
		ituWidgetSetVisible(S4_FSs_FRA, false);

	ITUWidget *S4_FSx_FRA = ituSceneFindWidget(&theScene, "S4_FSx_FRA");
	assert(S4_FSx_FRA);
	if(ituWidgetIsVisible(S4_FSx_FRA) == true)
		ituWidgetSetVisible(S4_FSx_FRA, false);

	ITUWidget *S4_FSerr_FRA = ituSceneFindWidget(&theScene, "S4_FSerr_FRA");
	assert(S4_FSerr_FRA);
	if(ituWidgetIsVisible(S4_FSerr_FRA) == true)
		ituWidgetSetVisible(S4_FSerr_FRA, false);

	ITUWidget *S4_FSs_ITA = ituSceneFindWidget(&theScene, "S4_FSs_ITA");
	assert(S4_FSs_ITA);
	if(ituWidgetIsVisible(S4_FSs_ITA) == true)
		ituWidgetSetVisible(S4_FSs_ITA, false);

	ITUWidget *S4_FSx_ITA = ituSceneFindWidget(&theScene, "S4_FSx_ITA");
	assert(S4_FSx_ITA);
	if(ituWidgetIsVisible(S4_FSx_ITA) == true)
		ituWidgetSetVisible(S4_FSx_ITA, false);

	ITUWidget *S4_FSerr_ITA = ituSceneFindWidget(&theScene, "S4_FSerr_ITA");
	assert(S4_FSerr_ITA);
	if(ituWidgetIsVisible(S4_FSerr_ITA) == true)
		ituWidgetSetVisible(S4_FSerr_ITA, false);

	ITUWidget *S4_FSs_ESP = ituSceneFindWidget(&theScene, "S4_FSs_ESP");
	assert(S4_FSs_ESP);
	if(ituWidgetIsVisible(S4_FSs_ESP) == true)
		ituWidgetSetVisible(S4_FSs_ESP, false);

	ITUWidget *S4_FSx_ESP = ituSceneFindWidget(&theScene, "S4_FSx_ESP");
	assert(S4_FSx_ESP);
	if(ituWidgetIsVisible(S4_FSx_ESP) == true)
		ituWidgetSetVisible(S4_FSx_ESP, false);

	ITUWidget *S4_FSerr_ESP = ituSceneFindWidget(&theScene, "S4_FSerr_ESP");
	assert(S4_FSerr_ESP);
	if(ituWidgetIsVisible(S4_FSerr_ESP) == true)
		ituWidgetSetVisible(S4_FSerr_ESP, false);
}

void AG_FS_Display(uint8_t tFSIdx)
{
	switch(Lang_Flag[M7_UserIndex])		// S5 set
	{
		case Lang_ENG:
		{
			ITUWidget *S4_FSs_ENG = ituSceneFindWidget(&theScene, "S4_FSs_ENG");
			assert(S4_FSs_ENG);
			ITUWidget *S4_FSx_ENG = ituSceneFindWidget(&theScene, "S4_FSx_ENG");
			assert(S4_FSx_ENG);
			ITUWidget *S4_FSerr_ENG = ituSceneFindWidget(&theScene, "S4_FSerr_ENG");
			assert(S4_FSerr_ENG);

			if(tFSIdx == 2)		// error 
			{
				ituWidgetSetVisible(S4_FSs_ENG, false);			// gray
				ituWidgetSetVisible(S4_FSx_ENG, false);			// green
				ituWidgetSetVisible(S4_FSerr_ENG, true);		// red
			}
			else if(tFSIdx == 1) // normal off
			{
				ituWidgetSetVisible(S4_FSx_ENG, false);			// green
				ituWidgetSetVisible(S4_FSerr_ENG, false);		// red
				ituWidgetSetVisible(S4_FSs_ENG, true);			// gray
			}
			else if(tFSIdx == 0)		// normal on
			{
				ituWidgetSetVisible(S4_FSs_ENG, false);			// gray
				ituWidgetSetVisible(S4_FSerr_ENG, false);		// red
				ituWidgetSetVisible(S4_FSx_ENG, true);			// green
			}
		}
		break;
		
		case Lang_DEU:
		{
			ITUWidget *S4_FSs_DEU = ituSceneFindWidget(&theScene, "S4_FSs_DEU");
			assert(S4_FSs_DEU);
			ITUWidget *S4_FSx_DEU = ituSceneFindWidget(&theScene, "S4_FSx_DEU");
			assert(S4_FSx_DEU);
			ITUWidget *S4_FSerr_DEU = ituSceneFindWidget(&theScene, "S4_FSerr_DEU");
			assert(S4_FSerr_DEU);

			if(tFSIdx == 2)		// error 
			{
				ituWidgetSetVisible(S4_FSs_DEU, false);
				ituWidgetSetVisible(S4_FSx_DEU, false);
				ituWidgetSetVisible(S4_FSerr_DEU, true);
			}
			else if(tFSIdx == 1) // normal off
			{
				ituWidgetSetVisible(S4_FSx_DEU, false);
				ituWidgetSetVisible(S4_FSerr_DEU, false);
				ituWidgetSetVisible(S4_FSs_DEU, true);
			}
			else if(tFSIdx == 0)		// normal on
			{
				ituWidgetSetVisible(S4_FSs_DEU, false);
				ituWidgetSetVisible(S4_FSerr_DEU, false);
				ituWidgetSetVisible(S4_FSx_DEU, true);
			}
		}
		break;
		
		case Lang_FRA:
		{
			ITUWidget *S4_FSs_FRA = ituSceneFindWidget(&theScene, "S4_FSs_FRA");
			assert(S4_FSs_FRA);
			ITUWidget *S4_FSx_FRA = ituSceneFindWidget(&theScene, "S4_FSx_FRA");
			assert(S4_FSx_FRA);
			ITUWidget *S4_FSerr_FRA = ituSceneFindWidget(&theScene, "S4_FSerr_FRA");
			assert(S4_FSerr_FRA);

			if(tFSIdx == 2)		// error 
			{
				ituWidgetSetVisible(S4_FSs_FRA, false);
				ituWidgetSetVisible(S4_FSx_FRA, false);
				ituWidgetSetVisible(S4_FSerr_FRA, true);
			}
			else if(tFSIdx == 1) // normal off
			{
				ituWidgetSetVisible(S4_FSx_FRA, false);
				ituWidgetSetVisible(S4_FSerr_FRA, false);
				ituWidgetSetVisible(S4_FSs_FRA, true);
			}
			else if(tFSIdx == 0)		// normal on
			{
				ituWidgetSetVisible(S4_FSs_FRA, false);
				ituWidgetSetVisible(S4_FSerr_FRA, false);
				ituWidgetSetVisible(S4_FSx_FRA, true);
			}
		}
		break;
		
		case Lang_ITA:
		{
			ITUWidget *S4_FSs_ITA = ituSceneFindWidget(&theScene, "S4_FSs_ITA");
			assert(S4_FSs_ITA);
			ITUWidget *S4_FSx_ITA = ituSceneFindWidget(&theScene, "S4_FSx_ITA");
			assert(S4_FSx_ITA);
			ITUWidget *S4_FSerr_ITA = ituSceneFindWidget(&theScene, "S4_FSerr_ITA");
			assert(S4_FSerr_ITA);

			if(tFSIdx == 2)		// error 
			{
				ituWidgetSetVisible(S4_FSs_ITA, false);
				ituWidgetSetVisible(S4_FSx_ITA, false);
				ituWidgetSetVisible(S4_FSerr_ITA, true);
			}
			else if(tFSIdx == 1) // normal off
			{
				ituWidgetSetVisible(S4_FSx_ITA, false);
				ituWidgetSetVisible(S4_FSerr_ITA, false);
				ituWidgetSetVisible(S4_FSs_ITA, true);
			}
			else if(tFSIdx == 0)		// normal on
			{
				ituWidgetSetVisible(S4_FSs_ITA, false);
				ituWidgetSetVisible(S4_FSerr_ITA, false);
				ituWidgetSetVisible(S4_FSx_ITA, true);
			}
		}
		break;
		
		case Lang_ESP:
		{
			ITUWidget *S4_FSs_ESP = ituSceneFindWidget(&theScene, "S4_FSs_ESP");
			assert(S4_FSs_ESP);
			ITUWidget *S4_FSx_ESP = ituSceneFindWidget(&theScene, "S4_FSx_ESP");
			assert(S4_FSx_ESP);
			ITUWidget *S4_FSerr_ESP = ituSceneFindWidget(&theScene, "S4_FSerr_ESP");
			assert(S4_FSerr_ESP);

			if(tFSIdx == 2)		// error 
			{
				ituWidgetSetVisible(S4_FSs_ESP, false);
				ituWidgetSetVisible(S4_FSx_ESP, false);
				ituWidgetSetVisible(S4_FSerr_ESP, true);
			}
			else if(tFSIdx == 1) // normal off
			{
				ituWidgetSetVisible(S4_FSx_ESP, false);
				ituWidgetSetVisible(S4_FSerr_ESP, false);
				ituWidgetSetVisible(S4_FSs_ESP, true);
			}
			else if(tFSIdx == 0)		// normal on
			{
				ituWidgetSetVisible(S4_FSs_ESP, false);
				ituWidgetSetVisible(S4_FSerr_ESP, false);
				ituWidgetSetVisible(S4_FSx_ESP, true);
			}
		}
		break;
	}
}
