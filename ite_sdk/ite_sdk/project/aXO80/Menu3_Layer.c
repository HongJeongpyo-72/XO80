#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

/* widgets:
Menu3_Layer
Menu_AuxBG
M3_H_NA
M3_G_NA
M3_D_NA
M3_C_NA
M3_B_NA
M3_A_NA
M3_TopUserName
M3_TopStandard
M3_TopAdmin
M3_D7_BTN
M3_D6_BTN
M3_D5_BTN
M3_H_Value
M3_G_Value
M3_D_Value
M3_C_Value
M3_B_Value
M3_A_Value
*/

static ITUText *M3_A_Value;
static ITUText *M3_B_Value;
static ITUText *M3_C_Value;
static ITUText *M3_D_Value;
static ITUText *M3_G_Value;
static ITUText *M3_H_Value;

static ITUText *M3_A_NA;
static ITUText *M3_B_NA;
static ITUText *M3_C_NA;
static ITUText *M3_D_NA;
static ITUText *M3_G_NA;
static ITUText *M3_H_NA;

uint8_t AUX_A_Value = 0xFF,AUX_B_Value = 0xFF;
uint8_t AUX_C_Value = 0xFF,AUX_D_Value = 0xFF;
uint8_t AUX_G_Value = 0xFF,AUX_H_Value = 0xFF;

uint32_t AUX_Value_CNT;
static uint8_t M3_NAFlag[6];

static uint32_t M3_SecTimer;
static uint8_t M3_ExitCNT = 0;
 
uint32_t tExit3_timer = 0;		// jhlee 231006

bool M3_UserDisplay(void);
 
extern void GotoMenuTop(void);


bool M3_Enter(ITUWidget* widget, char* param)
{
	char ptAUXValue[10];
	//signed int tbyte;
	
	M3_A_Value = ituSceneFindWidget(&theScene, "M3_A_Value");
	assert(M3_A_Value);
	M3_B_Value = ituSceneFindWidget(&theScene, "M3_B_Value");
	assert(M3_B_Value);
	M3_C_Value = ituSceneFindWidget(&theScene, "M3_C_Value");
	assert(M3_C_Value);
	M3_D_Value = ituSceneFindWidget(&theScene, "M3_D_Value");
	assert(M3_D_Value);
	M3_G_Value = ituSceneFindWidget(&theScene, "M3_G_Value");
	assert(M3_G_Value);
	M3_H_Value = ituSceneFindWidget(&theScene, "M3_H_Value");
	assert(M3_H_Value);

	M3_A_NA = ituSceneFindWidget(&theScene, "M3_A_NA");
	assert(M3_A_NA);
	M3_B_NA = ituSceneFindWidget(&theScene, "M3_B_NA");
	assert(M3_B_NA);
	M3_C_NA = ituSceneFindWidget(&theScene, "M3_C_NA");
	assert(M3_C_NA);
	M3_D_NA = ituSceneFindWidget(&theScene, "M3_D_NA");
	assert(M3_D_NA);
	M3_G_NA = ituSceneFindWidget(&theScene, "M3_G_NA");
	assert(M3_G_NA);
	M3_H_NA = ituSceneFindWidget(&theScene, "M3_H_NA");
	assert(M3_H_NA);

	memset(ptAUXValue,0,10);

	if(AUX_A_Value == 0xFF)
	{
		AUX_A_Value = 0xFF;
		sprintf(ptAUXValue,"N/A");
		
		M3_NAFlag[0] = 1;
	}
	else if(AUX_A_Value > 250)
	{
		AUX_A_Value = 250;
		sprintf(ptAUXValue,"%3d",AUX_A_Value);

		M3_NAFlag[0] = 0;
	}
	else if((AUX_A_Value >= 0) && (AUX_A_Value <= 250))
	{
		sprintf(ptAUXValue,"%3d",AUX_A_Value);

		M3_NAFlag[0] = 0;
	}

	if(M3_NAFlag[0] == 0)
	{
		ituTextSetString(M3_A_Value, ptAUXValue);

		ituWidgetSetVisible(M3_A_NA, false);
		ituWidgetSetVisible(M3_A_Value, true);
	}
	else if(M3_NAFlag[0] == 1)
	{
		ituTextSetString(M3_A_NA, ptAUXValue);

		ituWidgetSetVisible(M3_A_Value, false);
		ituWidgetSetVisible(M3_A_NA, true);
	}

	memset(ptAUXValue,0,10);

	if(AUX_B_Value == 0xFF)
	{
		AUX_B_Value = 0xFF;
		sprintf(ptAUXValue,"N/A");
		
		M3_NAFlag[1] = 1;
	}
	else if(AUX_B_Value > 250)
	{
		AUX_B_Value = 250;
		sprintf(ptAUXValue,"%3d",AUX_B_Value);

		M3_NAFlag[1] = 0;
	}
	else if((AUX_B_Value >= 0) && (AUX_B_Value <= 250))
	{
		sprintf(ptAUXValue,"%3d",AUX_B_Value);
		
		M3_NAFlag[1] = 0;
	}

	if(M3_NAFlag[1] == 0)
	{
		ituTextSetString(M3_B_Value, ptAUXValue);

		ituWidgetSetVisible(M3_B_NA, false);
		ituWidgetSetVisible(M3_B_Value, true);
	}
	else if(M3_NAFlag[1] == 1)
	{
		ituTextSetString(M3_B_NA, ptAUXValue);

		ituWidgetSetVisible(M3_B_Value, false);
		ituWidgetSetVisible(M3_B_NA, true);
	}

	memset(ptAUXValue,0,10);

	if(AUX_C_Value == 0xFF)
	{
		AUX_C_Value = 0xFF;
		sprintf(ptAUXValue,"N/A");
		
		M3_NAFlag[2] = 1;
	}
	else if(AUX_C_Value > 250)
	{
		AUX_C_Value = 250;
		sprintf(ptAUXValue,"%3d",AUX_C_Value);

		M3_NAFlag[2] = 0;
	}
	else if((AUX_C_Value >= 0) && (AUX_C_Value <= 250))
	{
		sprintf(ptAUXValue,"%3d",AUX_C_Value);

		M3_NAFlag[2] = 0;
	}
	
	if(M3_NAFlag[2] == 0)
	{
		ituTextSetString(M3_C_Value, ptAUXValue);

		ituWidgetSetVisible(M3_C_NA, false);
		ituWidgetSetVisible(M3_C_Value, true);
	}
	else if(M3_NAFlag[2] == 1)
	{
		ituTextSetString(M3_C_NA, ptAUXValue);

		ituWidgetSetVisible(M3_C_Value, false);
		ituWidgetSetVisible(M3_C_NA, true);
	}

	memset(ptAUXValue,0,10);

	if(AUX_D_Value == 0xFF)
	{
		AUX_D_Value = 0xFF;
		sprintf(ptAUXValue,"N/A");
		
		M3_NAFlag[3] = 1;
	}
	else if(AUX_D_Value > 250)
	{
		AUX_D_Value = 250;
		sprintf(ptAUXValue,"%3d",AUX_D_Value);

		M3_NAFlag[3] = 0;
	}
	else if((AUX_D_Value >= 0) && (AUX_D_Value <= 250))
	{
		sprintf(ptAUXValue,"%3d",AUX_D_Value);

		M3_NAFlag[3] = 0;
	}

	if(M3_NAFlag[3] == 0)
	{
		ituTextSetString(M3_D_Value, ptAUXValue);

		ituWidgetSetVisible(M3_D_NA, false);
		ituWidgetSetVisible(M3_D_Value, true);
	}
	else if(M3_NAFlag[3] == 1)
	{
		ituTextSetString(M3_D_NA, ptAUXValue);

		ituWidgetSetVisible(M3_D_Value, false);
		ituWidgetSetVisible(M3_D_NA, true);
	}
	
	memset(ptAUXValue,0,10);

	if(AUX_G_Value == 0xFF)
	{
		AUX_G_Value = 0xFF;
		sprintf(ptAUXValue,"N/A");

		M3_NAFlag[4] = 1;
	}
	else if(AUX_G_Value > 250)
	{
		AUX_G_Value = 250;
		sprintf(ptAUXValue,"%3d",AUX_G_Value);

		M3_NAFlag[4] = 0;
	}
	else if((AUX_G_Value >= 0) && (AUX_G_Value <= 250))
	{
		sprintf(ptAUXValue,"%3d",AUX_G_Value);

		M3_NAFlag[4] = 0;
	}

	if(M3_NAFlag[4] == 0)
	{
		ituTextSetString(M3_G_Value, ptAUXValue);

		ituWidgetSetVisible(M3_G_NA, false);
		ituWidgetSetVisible(M3_G_Value, true);
	}
	else if(M3_NAFlag[4] == 1)
	{
		ituTextSetString(M3_G_NA, ptAUXValue);

		ituWidgetSetVisible(M3_G_Value, false);
		ituWidgetSetVisible(M3_G_NA, true);
	}

	memset(ptAUXValue,0,10);

	if(AUX_H_Value == 0xFF)
	{
		AUX_H_Value = 0xFF;
		sprintf(ptAUXValue,"N/A");
		
		M3_NAFlag[5] = 1;
	}
	else if(AUX_H_Value > 250)
	{
		AUX_H_Value = 250;
		sprintf(ptAUXValue,"%3d",AUX_H_Value);

		M3_NAFlag[5] = 0;
	}
	else if((AUX_H_Value >= 0) && (AUX_H_Value <= 250))
	{
		sprintf(ptAUXValue,"%3d",AUX_H_Value);

		M3_NAFlag[5] = 0;
	}

	if(M3_NAFlag[5] == 0)
	{
		ituTextSetString(M3_H_Value, ptAUXValue);

		ituWidgetSetVisible(M3_H_NA, false);
		ituWidgetSetVisible(M3_H_Value, true);
	}
	else if(M3_NAFlag[5] == 1)
	{
		ituTextSetString(M3_H_NA, ptAUXValue);

		ituWidgetSetVisible(M3_H_Value, false);
		ituWidgetSetVisible(M3_H_NA, true);
	}

	M3_UserDisplay();

	M3_ExitCNT = 0;
	M3_SecTimer = gMainTimerCnt_10m;
	tExit3_timer = gMainTimerCnt_10m;
    return true;
}

bool M3_Timer(ITUWidget* widget, char* param)
{
	char ptAUXValue[4];

	static uint8_t b_A_Value,b_B_Value;
	static uint8_t b_C_Value,b_D_Value;
	static uint8_t b_G_Value,b_H_Value;

#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		return 0;
	}
#endif

/*	// jhlee 231006
	if((gMainTimerCnt_10m - tExit3_timer) >= 3000)
	{
		M3_ExitCNT = 100;
		printf("\r\nM3 Menu 30Sec Exit > goto Main\r\n");
		//GotoMain(MAIN_MODE_NORMAL); 
	}

	if((gMainTimerCnt_10m - M3_SecTimer) >= 100)
	{
		if(M3_ExitCNT < 30)
			M3_ExitCNT++;
		else if(M3_ExitCNT == 30)
		{
			M3_ExitCNT = 100;
			printf("\r\nM3 Menu 30Sec Exit > goto Main\r\n");
			//GotoMain(MAIN_MODE_NORMAL); 
		}

		M3_SecTimer = gMainTimerCnt_10m;
	}
*/

	if((gMainTimerCnt_10m - AUX_Value_CNT) >= 500)
	{
		memset(ptAUXValue,0,4);
		sprintf(ptAUXValue,"N/A");

		ituTextSetString(M3_A_NA, ptAUXValue);

		ituWidgetSetVisible(M3_A_Value, false);
		ituWidgetSetVisible(M3_A_NA, true);

		ituTextSetString(M3_B_NA, ptAUXValue);

		ituWidgetSetVisible(M3_B_Value, false);
		ituWidgetSetVisible(M3_B_NA, true);

		ituTextSetString(M3_C_NA, ptAUXValue);

		ituWidgetSetVisible(M3_C_Value, false);
		ituWidgetSetVisible(M3_C_NA, true);

		ituTextSetString(M3_D_NA, ptAUXValue);

		ituWidgetSetVisible(M3_D_Value, false);
		ituWidgetSetVisible(M3_D_NA, true);

		ituTextSetString(M3_G_NA, ptAUXValue);

		ituWidgetSetVisible(M3_G_Value, false);
		ituWidgetSetVisible(M3_G_NA, true);

		ituTextSetString(M3_H_NA, ptAUXValue);

		ituWidgetSetVisible(M3_H_Value, false);
		ituWidgetSetVisible(M3_H_NA, true);

		AUX_A_Value = 0xFF;
		b_A_Value = 0xFF;
		M3_NAFlag[0] = 1;
		
		AUX_B_Value = 0xFF;
		b_B_Value = 0xFF;
		M3_NAFlag[1] = 1;

		AUX_C_Value = 0xFF;
		b_C_Value = 0xFF;
		M3_NAFlag[2] = 1;
		
		AUX_D_Value = 0xFF;
		b_D_Value = 0xFF;
		M3_NAFlag[3] = 1;

		AUX_G_Value = 0xFF;
		b_G_Value = 0xFF;
		M3_NAFlag[4] = 1;

		AUX_H_Value = 0xFF;
		b_H_Value = 0xFF;
		M3_NAFlag[5] = 1;
		
		AUX_Value_CNT = gMainTimerCnt_10m;
	}
	else
	{
		if(b_A_Value != AUX_A_Value)
		{
			memset(ptAUXValue,0,4);

			if(AUX_A_Value == 0xFF)		
			{
				sprintf(ptAUXValue,"N/A");

				M3_NAFlag[0] = 1;
			}
			else if((AUX_A_Value > 250) && (AUX_A_Value < 255))
			{
				AUX_A_Value = 250;
				sprintf(ptAUXValue,"%3d",AUX_A_Value);

				M3_NAFlag[0] = 0;
			}
			else if((AUX_A_Value >= 0) && (AUX_A_Value <= 250))
			{
				sprintf(ptAUXValue,"%3d",AUX_A_Value);

				M3_NAFlag[0] = 0;
			}

			b_A_Value = AUX_A_Value;

			if(M3_NAFlag[0] == 0)
			{
				ituTextSetString(M3_A_Value, ptAUXValue);

				ituWidgetSetVisible(M3_A_NA, false);
				ituWidgetSetVisible(M3_A_Value, true);
			}
			else if(M3_NAFlag[0] == 1)
			{
				ituTextSetString(M3_A_NA, ptAUXValue);

				ituWidgetSetVisible(M3_A_Value, false);
				ituWidgetSetVisible(M3_A_NA, true);
			}
		}

		if(b_B_Value != AUX_B_Value)
		{
			memset(ptAUXValue,0,4);

			if(AUX_B_Value == 0xFF)		
			{
				sprintf(ptAUXValue,"N/A");

				M3_NAFlag[1] = 1;
			}
			else if((AUX_B_Value > 250) && (AUX_B_Value < 255))
			{
				AUX_B_Value = 250;
				sprintf(ptAUXValue,"%3d",AUX_B_Value);

				M3_NAFlag[1] = 0;
			}
			else if((AUX_B_Value >= 0) && (AUX_B_Value <= 250))
			{
				sprintf(ptAUXValue,"%3d",AUX_B_Value);

				M3_NAFlag[1] = 0;
			}

			b_B_Value = AUX_B_Value;

			if(M3_NAFlag[1] == 0)
			{
				ituTextSetString(M3_B_Value, ptAUXValue);

				ituWidgetSetVisible(M3_B_NA, false);
				ituWidgetSetVisible(M3_B_Value, true);
			}
			else if(M3_NAFlag[1] == 1)
			{
				ituTextSetString(M3_B_NA, ptAUXValue);

				ituWidgetSetVisible(M3_B_Value, false);
				ituWidgetSetVisible(M3_B_NA, true);
			}
		}

		if(b_C_Value != AUX_C_Value)
		{
			memset(ptAUXValue,0,4);

			if(AUX_C_Value == 0xFF)		
			{
				sprintf(ptAUXValue,"N/A");

				M3_NAFlag[2] = 1;
			}
			else if((AUX_C_Value > 250) && (AUX_C_Value < 255))
			{
				AUX_C_Value = 250;
				sprintf(ptAUXValue,"%3d",AUX_C_Value);

				M3_NAFlag[2] = 0;
			}
			else if((AUX_C_Value >= 0) && (AUX_C_Value <= 250))
			{
				sprintf(ptAUXValue,"%3d",AUX_C_Value);

				M3_NAFlag[2] = 0;
			}

			b_C_Value = AUX_C_Value;

			if(M3_NAFlag[2] == 0)
			{
				ituTextSetString(M3_C_Value, ptAUXValue);

				ituWidgetSetVisible(M3_C_NA, false);
				ituWidgetSetVisible(M3_C_Value, true);
			}
			else if(M3_NAFlag[2] == 1)
			{
				ituTextSetString(M3_C_NA, ptAUXValue);

				ituWidgetSetVisible(M3_C_Value, false);
				ituWidgetSetVisible(M3_C_NA, true);
			}
		}

		if(b_D_Value != AUX_D_Value)
		{
			memset(ptAUXValue,0,4);

			if(AUX_D_Value == 0xFF)		
			{
				sprintf(ptAUXValue,"N/A");

				M3_NAFlag[3] = 1;
			}
			else if((AUX_D_Value > 250) && (AUX_D_Value < 255))
			{
				AUX_D_Value = 250;
				sprintf(ptAUXValue,"%3d",AUX_D_Value);

				M3_NAFlag[3] = 0;
			}
			else if((AUX_D_Value >= 0) && (AUX_D_Value <= 250))
			{
				sprintf(ptAUXValue,"%3d",AUX_D_Value);

				M3_NAFlag[3] = 0;
			}

			b_D_Value = AUX_D_Value;

			if(M3_NAFlag[3] == 0)
			{
				ituTextSetString(M3_D_Value, ptAUXValue);

				ituWidgetSetVisible(M3_D_NA, false);
				ituWidgetSetVisible(M3_D_Value, true);
			}
			else if(M3_NAFlag[3] == 1)
			{
				ituTextSetString(M3_D_NA, ptAUXValue);

				ituWidgetSetVisible(M3_D_Value, false);
				ituWidgetSetVisible(M3_D_NA, true);
			}
		}

		if(b_G_Value != AUX_G_Value)
		{
			memset(ptAUXValue,0,4);

			if(AUX_G_Value == 0xFF)		
			{
				sprintf(ptAUXValue,"N/A");

				M3_NAFlag[4] = 1;
			}
			else if((AUX_G_Value > 250) && (AUX_G_Value < 255))
			{
				AUX_G_Value = 250;
				sprintf(ptAUXValue,"%3d",AUX_G_Value);

				M3_NAFlag[4] = 0;
			}
			else if((AUX_G_Value >= 0) && (AUX_G_Value <= 250))
			{
				sprintf(ptAUXValue,"%3d",AUX_G_Value);

				M3_NAFlag[4] = 0;
			}

			b_G_Value = AUX_G_Value;

			if(M3_NAFlag[4] == 0)
			{
				ituTextSetString(M3_G_Value, ptAUXValue);

				ituWidgetSetVisible(M3_G_NA, false);
				ituWidgetSetVisible(M3_G_Value, true);
			}
			else if(M3_NAFlag[4] == 1)
			{
				ituTextSetString(M3_G_NA, ptAUXValue);

				ituWidgetSetVisible(M3_G_Value, false);
				ituWidgetSetVisible(M3_G_NA, true);
			}
		}
	
		if(b_H_Value != AUX_H_Value)
		{
			memset(ptAUXValue,0,4);

			if(AUX_H_Value == 0xFF)		
			{
				sprintf(ptAUXValue,"N/A");

				M3_NAFlag[5] = 1;
			}
			else if((AUX_H_Value > 250) && (AUX_H_Value < 255))
			{
				AUX_H_Value = 250;
				sprintf(ptAUXValue,"%3d",AUX_H_Value);

				M3_NAFlag[5] = 0;
			}
			else if((AUX_H_Value >= 0) && (AUX_H_Value <= 250))
			{
				sprintf(ptAUXValue,"%3d",AUX_H_Value);

				M3_NAFlag[5] = 0;
			}

			b_H_Value = AUX_H_Value;

			if(M3_NAFlag[5] == 0)
			{
				ituTextSetString(M3_H_Value, ptAUXValue);

				ituWidgetSetVisible(M3_H_NA, false);
				ituWidgetSetVisible(M3_H_Value, true);
			}
			else if(M3_NAFlag[5] == 1)
			{
				ituTextSetString(M3_H_NA, ptAUXValue);

				ituWidgetSetVisible(M3_H_Value, false);
				ituWidgetSetVisible(M3_H_NA, true);
			}
		}
	}

	M3_UserDisplay();

    return true;
}

bool M3_JogWheel(ITUWidget* widget, char* param)
{
	M3_ExitCNT = 0;
	tExit3_timer = gMainTimerCnt_10m;		// jhlee 231006
	
    return true;
}

bool M3_JogButton(ITUWidget* widget, char* param)
{
	M3_ExitCNT = 0;
	tExit3_timer = gMainTimerCnt_10m;		// jhlee 231006
	
    return true;
}

bool M3_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t JogSwitch = 0;

	M3_ExitCNT = 0;
	tExit3_timer = gMainTimerCnt_10m;		// jhlee 231006
	
	JogSwitch = param[0];

//	printf("M3_JogSwitch JogSwitch %d\r\n",JogSwitch);

	if(JogSwitch == JOG_DIAL_SW_ESC_ON_OFF)		// jhlee 231006
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

bool M3D7_MouseUp(ITUWidget* widget, char* param)
{
	M3_ExitCNT = 0;

    return true;
}

bool M3D6_MouseUp(ITUWidget* widget, char* param)
{

	GotoMain(MAIN_MODE_NORMAL); 
	
    return true;
}

bool M3D5_MouseUp(ITUWidget* widget, char* param)
{

	GotoMenuTop();
	
    return true;
}

bool M3_UserDisplay(void)
{
	uint8_t tDestName[100];

	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
		return 0;

	ITUWidget *M3_TopAdmin = ituSceneFindWidget(&theScene, "M3_TopAdmin");
	assert(M3_TopAdmin);
	ITUWidget *M3_TopStandard = ituSceneFindWidget(&theScene, "M3_TopStandard");
	assert(M3_TopStandard);
	
	ITUText *M3_TopUserName = ituSceneFindWidget(&theScene, "M3_TopUserName");
	assert(M3_TopUserName);
	
	if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
	{
		ituWidgetSetVisible(M3_TopStandard,false);
		ituWidgetSetVisible(M3_TopAdmin,true);
	}
	else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
	{
		ituWidgetSetVisible(M3_TopAdmin,false);
		ituWidgetSetVisible(M3_TopStandard,true);
	}
	else
	{
		ituWidgetSetVisible(M3_TopAdmin,false);
		ituWidgetSetVisible(M3_TopStandard,false);
	}

	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M3_TopUserName, tDestName);

		ituWidgetSetVisible(M3_TopUserName,true);
	}
	else
	{
		ituWidgetSetVisible(M3_TopUserName,false);
	}
/*
	if((gMainTimerCnt_10m - gMenu_UI_Time) < 500)		// 5sec	
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M3_TopUserName, tDestName);

		ituWidgetSetVisible(M3_TopUserName,true);
	}
	else if(((gMainTimerCnt_10m - gMenu_UI_Time) >= 500)||(M7_UserIndex > (MAX_SU100_USER_NUM-1)))
	{
		ituWidgetSetVisible(M3_TopUserName,false);
	}
*/
}
