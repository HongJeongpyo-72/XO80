#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

/* widgets:
M7_S5
M7S5_BG
S5_D7_BTN
S5_D6_BTN
S5_D5_BTN
S5_5_SEL
S5_4_SEL
S5_3_SEL
S5_2_SEL
S5_1_SEL
S5_5_Toggle
S5_4_Toggle
S5_3_Toggle
S5_2_Toggle
S5_1_Toggle
S5_5_BTN
S5_4_BTN
S5_3_BTN
S5_2_BTN
S5_1_BTN
*/

uint8_t Lang_Flag[MAX_SU100_USER_NUM]={Lang_ENG,};

static uint32_t S5_SecTimer;
static uint8_t S5_ExitCNT = 0;

static uint8_t cLangFlag = 0;
static uint8_t gLangFlag = 0;

void S5_SelectClear(void);
void S5_SelectDisplay(uint8_t tLangFlag);

void S5_ToggleClear(void);
void S5_SelectToggle(uint8_t tLangFlag);

bool M7S5_UserDisplay(void);

extern void GotoMenuTop(void);
extern void GotoMenu7(void);

bool M7S5_Enter(ITUWidget* widget, char* param)
{
	cLangFlag = Lang_Flag[M7_UserIndex];
	gLangFlag = Lang_Flag[M7_UserIndex];
	
	S5_SelectClear();
	S5_ToggleClear();
	
	S5_SelectDisplay(cLangFlag);
	S5_SelectToggle(cLangFlag);
	
	S5_SecTimer = gMainTimerCnt_10m;
	S5_ExitCNT = 0;

	M7S5_UserDisplay();
	
    return true;
}

bool M7S5_Timer(ITUWidget* widget, char* param)
{

#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		return 0;
	}
#endif

	if((gMainTimerCnt_10m - S5_SecTimer) >= 100)
	{
		if(S5_ExitCNT < 30)
			S5_ExitCNT++;
		if(S5_ExitCNT == 30)
		{
			S5_ExitCNT = 100;
			printf("\r\nM7S5 Menu 30Sec Exit > goto Main\r\n");
			GotoMain(MAIN_MODE_NORMAL); 
		}
			
		S5_SecTimer = gMainTimerCnt_10m;
	}

	M7S5_UserDisplay();
	
    return true;
}

bool M7S5_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t tJogWheel = 0;
	
	S5_ExitCNT = 0;
	tJogWheel = param[0];
	
	printf("S5 JogWheel = %d\r\n",tJogWheel);
	
	if(tJogWheel == JOG_DIAL_ROT_LEFT)
	{
		S5_SelectClear();
			
		if(cLangFlag > Lang_ENG)
			cLangFlag--;
		else if(cLangFlag == Lang_ENG)
			cLangFlag = Lang_ESP;

		S5_SelectDisplay(cLangFlag);
	}
	else if(tJogWheel == JOG_DIAL_ROT_RIGHT)
	{
		S5_SelectClear();
			
		if(cLangFlag < Lang_ESP)
			cLangFlag++;
		else if(cLangFlag == Lang_ESP)
			cLangFlag = Lang_ENG;

		S5_SelectDisplay(cLangFlag);
	}

    return true;
}

bool M7S5_JogButton(ITUWidget* widget, char* param)
{
	uint8_t tJogButton = 0;
	uint8_t dLangFlag = 0;
	
	S5_ExitCNT = 0;
	tJogButton = param[0];

	printf("S5 JogButton = %d\r\n",tJogButton);
	
	switch(tJogButton)
	{
		case JOG_DIAL_KEY_UP:
		{
			S5_SelectClear();

			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
				// ENG[0],DEU[1],FRA[2],ITA[3],ESP[4]
				uint8_t tDest_up1[5] = {4,0,1,2,3};
				
				dLangFlag = tDest_up1[cLangFlag];
				cLangFlag = dLangFlag;
			}
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				// ENG[0],FRA[2],ESP[4]
				// DEU[1],ITA[3]
				uint8_t tDest_up2[5] = {1,0,3,2,3};
				
				dLangFlag = tDest_up2[cLangFlag];
				cLangFlag = dLangFlag;
			}

			S5_SelectDisplay(cLangFlag);
		}
		break;
		
		case JOG_DIAL_KEY_DOWN:
		{
			S5_SelectClear();
			
			if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
			{
				// ENG[0],DEU[1],FRA[2],ITA[3],ESP[4]
				uint8_t tDest_down1[5] = {1,2,3,4,0};
				
				dLangFlag = tDest_down1[cLangFlag];
				cLangFlag = dLangFlag;
			}
			else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				// ENG[0],FRA[2],ESP[4]
				// DEU[1],ITA[3]
				uint8_t tDest_down2[5] = {1,0,3,2,3};
				
				dLangFlag = tDest_down2[cLangFlag];
				cLangFlag = dLangFlag;
			}

			S5_SelectDisplay(cLangFlag);
		}
		break;

		case JOG_DIAL_KEY_LEFT:
		{	
			if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				// ENG[0],FRA[2],ESP[4]
				// DEU[1],ITA[3]
				uint8_t tDest_left1[5] = {4,3,0,1,2};
				
				dLangFlag = tDest_left1[cLangFlag];
				cLangFlag = dLangFlag;
				
				S5_SelectClear();
				S5_SelectDisplay(cLangFlag);
			}
		}
		break;
		
		case JOG_DIAL_KEY_RIGHT:
		{
			if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
			{
				// ENG[0],FRA[2],ESP[4]
				// DEU[1],ITA[3]
				uint8_t tDest_right1[5] = {2,3,4,1,0};
				
				dLangFlag = tDest_right1[cLangFlag];
				cLangFlag = dLangFlag;
				
				S5_SelectClear();
				S5_SelectDisplay(cLangFlag);
			}
		}
		break;
		
		case JOG_DIAL_KEY_ENTER_ON:
		break;
		
		case JOG_DIAL_KEY_ENTER_OFF:
		{
			S5_ToggleClear();
			gLangFlag = cLangFlag;
			S5_SelectToggle(cLangFlag);
			
#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
			if(gLangFlag != Lang_Flag[M7_UserIndex])
			{
				Lang_Flag[M7_UserIndex] = gLangFlag;

				//Lang_Flag[M7_UserIndex]
				XO_Save_Config(M7_UserIndex,true,0);
			}
#else
			if(gLangFlag != Lang_Flag[M7_UserIndex])
			{
				Lang_Flag[M7_UserIndex] = gLangFlag;

				HT_Save_Config(1);
			}
#endif

		}
		break;
	}
		
    return true;
}

bool M7S5_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t tJogSwitch = 0;
	
	S5_ExitCNT = 0;
	tJogSwitch = param[0];

	printf("S5 JogSwitch = %d\r\n",tJogSwitch);

	if(tJogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
		if(gLangFlag != Lang_Flag[M7_UserIndex])
		{
			Lang_Flag[M7_UserIndex] = gLangFlag;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
			//Lang_Flag[M7_UserIndex]
			XO_Save_Config(M7_UserIndex,true,0);
#else
			HT_Save_Config(1);
#endif
		}

		GotoMenu7();
	}
	else if(tJogSwitch == JOG_DIAL_SW_HOME_OFF)
	{
		if(gLangFlag != Lang_Flag[M7_UserIndex])
		{
			Lang_Flag[M7_UserIndex] = gLangFlag;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
			//Lang_Flag[M7_UserIndex]
			XO_Save_Config(M7_UserIndex,true,0);
#else
			HT_Save_Config(1);
#endif
		}

		GotoMain(MAIN_MODE_NORMAL); 
	}
		
    return true;
}

bool S5_L_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tLangFlag = 0;
	
	S5_ExitCNT = 0;
	tLangFlag = (uint8_t)atoi(param);
	
	S5_SelectClear();
	cLangFlag = tLangFlag - 1;
	S5_SelectDisplay(cLangFlag);

/*	
	switch(tLangFlag)
	{
		case 1:				// ENG
			S5_SelectClear();
			cLangFlag = Lang_ENG;
			S5_SelectDisplay(cLangFlag);
		break;

		case 2:				// DEU
			S5_SelectClear();
			cLangFlag = Lang_DEU;
			S5_SelectDisplay(cLangFlag);
		break;

		case 3:				// FRA
			S5_SelectClear();
			cLangFlag = Lang_FRA;
			S5_SelectDisplay(cLangFlag);
		break;

		case 4:				// ITA
			S5_SelectClear();
			cLangFlag = Lang_ITA;
			S5_SelectDisplay(cLangFlag);
		break;

		case 5:				// ESP
			S5_SelectClear();
			cLangFlag = Lang_ESP;
			S5_SelectDisplay(cLangFlag);
		break;
	}
*/
    return true;
}

bool S5_L_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tLangFlag = 0;
	
	S5_ExitCNT = 0;
	tLangFlag = (uint8_t)atoi(param);

	S5_ToggleClear();
	cLangFlag = tLangFlag - 1;
	gLangFlag = cLangFlag;
	S5_SelectToggle(cLangFlag);

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
	if(gLangFlag != Lang_Flag[M7_UserIndex])
	{
		Lang_Flag[M7_UserIndex] = gLangFlag;

		//Lang_Flag[M7_UserIndex]
		XO_Save_Config(M7_UserIndex,true,0);
	}
#else
	if(gLangFlag != Lang_Flag[M7_UserIndex])
	{
		Lang_Flag[M7_UserIndex] = gLangFlag;

		HT_Save_Config(1);
	}
#endif

/*	
	switch(tLangFlag)
	{
		case 1:				// ENG
			S5_ToggleClear();
			cLangFlag = Lang_ENG;
			Lang_Flag[M7_UserIndex] = cLangFlag;
			S5_SelectToggle(cLangFlag);
		break;

		case 2:				// DEU
			S5_ToggleClear();
			cLangFlag = Lang_DEU;
			Lang_Flag[M7_UserIndex] = cLangFlag;
			S5_SelectToggle(cLangFlag);
		break;

		case 3:				// FRA
			S5_ToggleClear();
			cLangFlag = Lang_FRA;
			Lang_Flag[M7_UserIndex] = cLangFlag;
			S5_SelectToggle(cLangFlag);
		break;

		case 4:				// ITA
			S5_ToggleClear();
			cLangFlag = Lang_ITA;
			Lang_Flag[M7_UserIndex] = cLangFlag;
			S5_SelectToggle(cLangFlag);
		break;

		case 5:				// ESP
			S5_ToggleClear();
			cLangFlag = Lang_ESP;
			Lang_Flag[M7_UserIndex] = cLangFlag;
			S5_SelectToggle(cLangFlag);
		break;
	}
*/

    return true;
}

bool S5D7_MouseDn(ITUWidget* widget, char* param)
{
	S5_ExitCNT = 0;

    return true;
}

bool S5D7_MouseUp(ITUWidget* widget, char* param)
{
	S5_ExitCNT = 0;
	char JD_Buf[10] = {0,};
	JD_Buf[0] = JOG_DIAL_KEY_ENTER_OFF;

	M7S5_JogButton(NULL, JD_Buf);

//	GotoMenu7();//  JPHong 2024-04-01   13841-Menu-4
	
    return true;
}

bool S5D6_MouseUp(ITUWidget* widget, char* param)
{
	S5_ExitCNT = 0;

	if(gLangFlag != Lang_Flag[M7_UserIndex])
	{
		Lang_Flag[M7_UserIndex] = gLangFlag;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
		//Lang_Flag[M7_UserIndex]
		XO_Save_Config(M7_UserIndex,true,0);
#else
		HT_Save_Config(1);
#endif
	}

	GotoMain(MAIN_MODE_NORMAL); 
	
    return true;
}

bool S5D5_MouseDn(ITUWidget* widget, char* param)
{
	S5_ExitCNT = 0;

    return true;
}

bool S5D5_MouseUp(ITUWidget* widget, char* param)
{
	S5_ExitCNT = 0;

	if(gLangFlag != Lang_Flag[M7_UserIndex])
	{
		Lang_Flag[M7_UserIndex] = gLangFlag;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
		//Lang_Flag[M7_UserIndex]
		XO_Save_Config(M7_UserIndex,true,0);
#else
		HT_Save_Config(1);
#endif
	}
	
	GotoMenu7();
	
    return true;
}

bool M7S5_UserDisplay(void)
{
	uint8_t tDestName[100];

	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
		return 0;

	ITUWidget *M7S5_TopAdmin = ituSceneFindWidget(&theScene, "M7S5_TopAdmin");
	assert(M7S5_TopAdmin);
	ITUWidget *M7S5_TopStandard = ituSceneFindWidget(&theScene, "M7S5_TopStandard");
	assert(M7S5_TopStandard);
	
	ITUText *M7S5_TopUserName = ituSceneFindWidget(&theScene, "M7S5_TopUserName");
	assert(M7S5_TopUserName);
	
	if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
	{
		ituWidgetSetVisible(M7S5_TopStandard,false);
		ituWidgetSetVisible(M7S5_TopAdmin,true);
	}
	else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
	{
		ituWidgetSetVisible(M7S5_TopAdmin,false);
		ituWidgetSetVisible(M7S5_TopStandard,true);
	}
	else
	{
		ituWidgetSetVisible(M7S5_TopAdmin,false);
		ituWidgetSetVisible(M7S5_TopStandard,false);
	}

	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M7S5_TopUserName, tDestName);

		ituWidgetSetVisible(M7S5_TopUserName,true);
	}
	else
	{
		ituWidgetSetVisible(M7S5_TopUserName,false);
	}
}

void S5_SelectToggle(uint8_t tLangFlag)
{
	ITUWidget *S5_1_Toggle = ituSceneFindWidget(&theScene, "S5_1_Toggle");
	assert(S5_1_Toggle);
	ITUWidget *S5_2_Toggle = ituSceneFindWidget(&theScene, "S5_2_Toggle");
	assert(S5_2_Toggle);
	ITUWidget *S5_3_Toggle = ituSceneFindWidget(&theScene, "S5_3_Toggle");
	assert(S5_3_Toggle);
	ITUWidget *S5_4_Toggle = ituSceneFindWidget(&theScene, "S5_4_Toggle");
	assert(S5_4_Toggle);
	ITUWidget *S5_5_Toggle = ituSceneFindWidget(&theScene, "S5_5_Toggle");
	assert(S5_5_Toggle);
	
	switch(tLangFlag)
	{
		case Lang_ENG:				// ENG
			ituWidgetSetVisible(S5_1_Toggle, true);
		break;

		case Lang_DEU:				// DEU
			ituWidgetSetVisible(S5_2_Toggle, true);
		break;

		case Lang_FRA:				// FRA
			ituWidgetSetVisible(S5_3_Toggle, true);
		break;

		case Lang_ITA:				// ITA
			ituWidgetSetVisible(S5_4_Toggle, true);
		break;

		case Lang_ESP:				// ESP
			ituWidgetSetVisible(S5_5_Toggle, true);
		break;
	}	
}

void S5_ToggleClear(void)
{
	ITUWidget *S5_1_Toggle = ituSceneFindWidget(&theScene, "S5_1_Toggle");
	assert(S5_1_Toggle);
	ITUWidget *S5_2_Toggle = ituSceneFindWidget(&theScene, "S5_2_Toggle");
	assert(S5_2_Toggle);
	ITUWidget *S5_3_Toggle = ituSceneFindWidget(&theScene, "S5_3_Toggle");
	assert(S5_3_Toggle);
	ITUWidget *S5_4_Toggle = ituSceneFindWidget(&theScene, "S5_4_Toggle");
	assert(S5_4_Toggle);
	ITUWidget *S5_5_Toggle = ituSceneFindWidget(&theScene, "S5_5_Toggle");
	assert(S5_5_Toggle);
	
	if(ituWidgetIsVisible(S5_1_Toggle) == true)
		ituWidgetSetVisible(S5_1_Toggle, false);

	if(ituWidgetIsVisible(S5_2_Toggle) == true)
		ituWidgetSetVisible(S5_2_Toggle, false);

	if(ituWidgetIsVisible(S5_3_Toggle) == true)
		ituWidgetSetVisible(S5_3_Toggle, false);

	if(ituWidgetIsVisible(S5_4_Toggle) == true)
		ituWidgetSetVisible(S5_4_Toggle, false);

	if(ituWidgetIsVisible(S5_5_Toggle) == true)
		ituWidgetSetVisible(S5_5_Toggle, false);
}

void S5_SelectDisplay(uint8_t tLangFlag)
{
	ITUWidget *S5_1_SEL = ituSceneFindWidget(&theScene, "S5_1_SEL");
	assert(S5_1_SEL);
	ITUWidget *S5_2_SEL = ituSceneFindWidget(&theScene, "S5_2_SEL");
	assert(S5_2_SEL);
	ITUWidget *S5_3_SEL = ituSceneFindWidget(&theScene, "S5_3_SEL");
	assert(S5_3_SEL);
	ITUWidget *S5_4_SEL = ituSceneFindWidget(&theScene, "S5_4_SEL");
	assert(S5_4_SEL);
	ITUWidget *S5_5_SEL = ituSceneFindWidget(&theScene, "S5_5_SEL");
	assert(S5_5_SEL);

	switch(tLangFlag)
	{
		case Lang_ENG:				// ENG
			ituWidgetSetVisible(S5_1_SEL, true);
		break;

		case Lang_DEU:				// DEU
			ituWidgetSetVisible(S5_2_SEL, true);
		break;

		case Lang_FRA:				// FRA
			ituWidgetSetVisible(S5_3_SEL, true);
		break;

		case Lang_ITA:				// ITA
			ituWidgetSetVisible(S5_4_SEL, true);
		break;

		case Lang_ESP:				// ESP
			ituWidgetSetVisible(S5_5_SEL, true);
		break;
	}	
}

void S5_SelectClear(void)
{
	ITUWidget *S5_1_SEL = ituSceneFindWidget(&theScene, "S5_1_SEL");
	assert(S5_1_SEL);
	ITUWidget *S5_2_SEL = ituSceneFindWidget(&theScene, "S5_2_SEL");
	assert(S5_2_SEL);
	ITUWidget *S5_3_SEL = ituSceneFindWidget(&theScene, "S5_3_SEL");
	assert(S5_3_SEL);
	ITUWidget *S5_4_SEL = ituSceneFindWidget(&theScene, "S5_4_SEL");
	assert(S5_4_SEL);
	ITUWidget *S5_5_SEL = ituSceneFindWidget(&theScene, "S5_5_SEL");
	assert(S5_5_SEL);
	
	if(ituWidgetIsVisible(S5_1_SEL) == true)
		ituWidgetSetVisible(S5_1_SEL, false);
		
	if(ituWidgetIsVisible(S5_2_SEL) == true)
		ituWidgetSetVisible(S5_2_SEL, false);

	if(ituWidgetIsVisible(S5_3_SEL) == true)
		ituWidgetSetVisible(S5_3_SEL, false);
		
	if(ituWidgetIsVisible(S5_4_SEL) == true)
		ituWidgetSetVisible(S5_4_SEL, false);
		
	if(ituWidgetIsVisible(S5_5_SEL) == true)
		ituWidgetSetVisible(S5_5_SEL, false);
}