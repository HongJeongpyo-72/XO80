#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

/* widgets:
M7_S7
M7S7_BG
S7_D7_BTN
S7_D6_BTN
S7_D5_BTN
S7_4_G
S7_3_G
S7_2_G
S7_1_G
S7_4_SEL
S7_3_SEL
S7_2_SEL
S7_1_SEL
S7_Name_ESP
S7_Name_ITA
S7_Name_FRA
S7_Name_DEU
S7_Name_ENG
S7_4_BTN
S7_3_BTN
S7_2_BTN
S7_1_BTN
*/


uint8_t S7_SelectRegion=0;
uint8_t Radio_Region=0;			// system data resume
uint8_t cRadio_PowerCondition=1;	//JPHong 0607


static uint8_t cRadioRegion = 0;
uint8_t S7_ChoiceRegion = 0;

static uint32_t S7_SecTimer;
static uint8_t S7_ExitCNT = 0;

void S7_SelectClear(void);
void S7_Select_On(uint8_t tRadioRegion);

void S7_RegionClear(void);
void S7_Region_On(uint8_t tRadioRegion);

void S7_Lang_Tile(uint8_t tLang);

bool M7S7_UserDisplay(void);

extern void GotoMenuTop(void);
extern void GotoMenu7(void);

bool M7S7_Enter(ITUWidget* widget, char* param)
{
	cRadio_PowerCondition = RDV.RDOnOff;			// system data sync, 0;off 1;on

	cRadioRegion = Radio_Region;		// system data resume
	S7_SelectRegion = cRadioRegion + 1;
	S7_ChoiceRegion = 0;
	
	S7_Lang_Tile(Lang_Flag[M7_UserIndex]);

	S7_SelectClear();
	S7_RegionClear();
	
	S7_Select_On(S7_SelectRegion);
	S7_Region_On(S7_SelectRegion);

	M7S7_UserDisplay();

	S7_SecTimer = gMainTimerCnt_10m;
	S7_ExitCNT = 0;

 	M7S7_UserDisplay();
 
   return true;
}

bool M7S7_Timer(ITUWidget* widget, char* param)
{
	static uint8_t tSel_Region = 0;

#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	
	if((gMainTimerCnt_10m - S7_SecTimer) >= 100)
	{
		if(S7_ExitCNT < 30)
			S7_ExitCNT++;
		if(S7_ExitCNT == 30)
		{
			S7_ExitCNT = 100;
			printf("\r\nM7S7 Menu 30Sec Exit > goto Main\r\n");
			GotoMain(MAIN_MODE_NORMAL); 
		}
			
		S7_SecTimer = gMainTimerCnt_10m;
	}

	cRadio_PowerCondition = RDV.RDOnOff;			// system data sync, 0;off 1;on

	if(tSel_Region != Radio_Region)
	{
		tSel_Region = Radio_Region;
		
		S7_RegionClear();
		S7_Region_On(tSel_Region+1);
	}
	
    return true;
}

bool M7S7_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t tJogWheel = 0;

	S7_ExitCNT = 0;
	tJogWheel = param[0];
	
	printf("S7 JogWheel = %d\r\n",tJogWheel);

	if(tJogWheel == JOG_DIAL_ROT_LEFT)
	{
		S7_SelectClear();

		S7_SelectRegion--;
		if(S7_SelectRegion == 0)
			S7_SelectRegion = 4;

		S7_Select_On(S7_SelectRegion);
	}
	else if(tJogWheel == JOG_DIAL_ROT_RIGHT)
	{
		S7_SelectClear();

		S7_SelectRegion++;
		if(S7_SelectRegion == 5)
			S7_SelectRegion = 1;

		S7_Select_On(S7_SelectRegion);
	}

    return true;
}

bool M7S7_JogButton(ITUWidget* widget, char* param)
{
	uint8_t tJogButton = 0;
	
	S7_ExitCNT = 0;
	tJogButton = param[0];
	
	printf("S7 JogButton = %d\r\n",tJogButton);

	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
		{
			if(tJogButton == JOG_DIAL_KEY_UP)
			{
				S7_SelectClear();

				S7_SelectRegion--;
				if(S7_SelectRegion == 0)
					S7_SelectRegion = 4;

				S7_Select_On(S7_SelectRegion);
			}
			else if(tJogButton == JOG_DIAL_KEY_DOWN)
			{
				S7_SelectClear();

				S7_SelectRegion++;
				if(S7_SelectRegion == 5)
					S7_SelectRegion = 1;

				S7_Select_On(S7_SelectRegion);
			}
			else if(tJogButton == JOG_DIAL_KEY_ENTER_ON)
			{
				//S7_RegionClear();
				//cRadioRegion = S7_SelectRegion - 1;
				//S7_Region_On(S7_SelectRegion);

				if(cRadio_PowerCondition == 0)		// 0; off , 1;on
				{
					S7_ChoiceRegion = S7_SelectRegion;
				}
			}
			else if(tJogButton == JOG_DIAL_KEY_ENTER_OFF)
			{
				//S7_RegionClear();
				//cRadioRegion = S7_SelectRegion - 1;
				//S7_Region_On(S7_SelectRegion);
				
				if(cRadio_PowerCondition == 0)		// 0; off , 1;on
				{
					S7_ChoiceRegion = 0;
				}
			}
		}
		break;

		case LANDSCAPE_1:
		case LANDSCAPE_2:
		{
			switch(tJogButton)
			{
				case JOG_DIAL_KEY_UP:
				case JOG_DIAL_KEY_DOWN:
				{	
					uint8_t tDestRegion_Up[5] = {0,2,1,4,3};
					
					S7_SelectClear();
					S7_SelectRegion = tDestRegion_Up[S7_SelectRegion];
					S7_Select_On(S7_SelectRegion);
				}
				break;
				
				case JOG_DIAL_KEY_LEFT:
				case JOG_DIAL_KEY_RIGHT:
				{	
					uint8_t tDestRegion_Left[5] = {0,3,4,1,2};
					
					S7_SelectClear();
					S7_SelectRegion = tDestRegion_Left[S7_SelectRegion];
					S7_Select_On(S7_SelectRegion);
				}
				break;
		
				case JOG_DIAL_KEY_ENTER_ON:
				{
					//S7_RegionClear();
					//cRadioRegion = S7_SelectRegion - 1;
					//S7_Region_On(S7_SelectRegion);

					if(cRadio_PowerCondition == 0)		// 0; off , 1;on
					{
						S7_ChoiceRegion = S7_SelectRegion;
					}
				}
				break;
				
				case JOG_DIAL_KEY_ENTER_OFF:
				{
					//S7_RegionClear();
					//cRadioRegion = S7_SelectRegion - 1;
					//S7_Region_On(S7_SelectRegion);
				
					if(cRadio_PowerCondition == 0)		// 0; off , 1;on
					{
						S7_ChoiceRegion = 0;
					}
				}
				break;
			}
		}
		break;
	}

    return true;
}

bool M7S7_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t tJogSwitch = 0;
	
	S7_ExitCNT = 0;
	tJogSwitch = param[0];
	
	printf("M7S7_JogSwitch JogSwitch = %d\r\n",tJogSwitch);

	if(tJogSwitch == JOG_DIAL_SW_ESC_ON_OFF) //JPHong 2024-03-22
		GotoMenu7();
	else if(tJogSwitch == JOG_DIAL_SW_HOME)
		GotoMain(MAIN_MODE_NORMAL); 

    return true;
}

bool S7D7_BTNMouseUp(ITUWidget* widget, char* param)
{
	if(Radio_Region != cRadioRegion)	// JPHong 06-11
	{
		Radio_Region = cRadioRegion;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
		//Radio_Region
		XO_Save_Config(M7_UserIndex,true,0);
#else
		HT_Save_Config(1);
#endif
	}
	GotoMenu7();

    return true;
}

bool S7D6_BTNMouseUp(ITUWidget* widget, char* param)
{
	if(Radio_Region != cRadioRegion)	// JPHong 06-11
	{
		Radio_Region = cRadioRegion;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
		//Radio_Region
		XO_Save_Config(M7_UserIndex,true,0);
#else
		HT_Save_Config(1);
#endif
	}

	GotoMain(MAIN_MODE_NORMAL); 

    return true;
}

bool S7D5_BTNMouseUp(ITUWidget* widget, char* param)
{
	if(Radio_Region != cRadioRegion)	// JPHong 06-11
	{
		Radio_Region = cRadioRegion;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
		//Radio_Region
		XO_Save_Config(M7_UserIndex,true,0);
#else
		HT_Save_Config(1);
#endif
	}
	
	GotoMenu7();
	
    return true;
}

bool S7_BTN_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tRadio = 0;
	
	S7_ExitCNT = 0;

	tRadio = (uint8_t)atoi(param);

	//printf("S7 BTN %d DN\r\n",tRadio);

	//printf("Radio Power = %d\r\n",cRadio_PowerCondition);

	S7_SelectClear();
	S7_SelectRegion = tRadio;
	S7_Select_On(S7_SelectRegion);

	if(cRadio_PowerCondition == 0)		// 0; off , 1;on
	{
		S7_ChoiceRegion = S7_SelectRegion;
	}

    return true;
}

bool S7_BTN_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tRadio = 0;
	
	S7_ExitCNT = 0;
	
	tRadio = (uint8_t)atoi(param);

	//printf("S7 BTN %d UP\r\n",tRadio);
	
	if(cRadio_PowerCondition == 0)			// 0; off , 1;on
	{
		//S7_RegionClear();
		//S7_SelectRegion = tRadio;
		//cRadioRegion = tRadio - 1;
		//S7_Region_On(S7_SelectRegion);

		S7_ChoiceRegion = 0;
	}
	
    return true;
}

bool M7S7_UserDisplay(void)
{
	uint8_t tDestName[100];

	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
		return 0;

	ITUWidget *M7S7_TopAdmin = ituSceneFindWidget(&theScene, "M7S7_TopAdmin");
	assert(M7S7_TopAdmin);
	ITUWidget *M7S7_TopStandard = ituSceneFindWidget(&theScene, "M7S7_TopStandard");
	assert(M7S7_TopStandard);
	
	ITUText *M7S7_TopUserName = ituSceneFindWidget(&theScene, "M7S7_TopUserName");
	assert(M7S7_TopUserName);
	
	if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
	{
		ituWidgetSetVisible(M7S7_TopStandard,false);
		ituWidgetSetVisible(M7S7_TopAdmin,true);
	}
	else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
	{
		ituWidgetSetVisible(M7S7_TopAdmin,false);
		ituWidgetSetVisible(M7S7_TopStandard,true);
	}
	else
	{
		ituWidgetSetVisible(M7S7_TopAdmin,false);
		ituWidgetSetVisible(M7S7_TopStandard,false);
	}

	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M7S7_TopUserName, tDestName);

		ituWidgetSetVisible(M7S7_TopUserName,true);
	}
	else
	{
		ituWidgetSetVisible(M7S7_TopUserName,false);
	}
}

void S7_SelectClear(void)
{
	ITUWidget *S7_1_SEL = ituSceneFindWidget(&theScene, "S7_1_SEL");
	assert(S7_1_SEL);
	ITUWidget *S7_2_SEL = ituSceneFindWidget(&theScene, "S7_2_SEL");
	assert(S7_2_SEL);
	ITUWidget *S7_3_SEL = ituSceneFindWidget(&theScene, "S7_3_SEL");
	assert(S7_3_SEL);
	ITUWidget *S7_4_SEL = ituSceneFindWidget(&theScene, "S7_4_SEL");
	assert(S7_4_SEL);

	if(ituWidgetIsVisible(S7_1_SEL) == true)
		ituWidgetSetVisible(S7_1_SEL, false);
	
	if(ituWidgetIsVisible(S7_2_SEL) == true)
		ituWidgetSetVisible(S7_2_SEL, false);
	
	if(ituWidgetIsVisible(S7_3_SEL) == true)
		ituWidgetSetVisible(S7_3_SEL, false);

	if(ituWidgetIsVisible(S7_4_SEL) == true)
		ituWidgetSetVisible(S7_4_SEL, false);
}

void S7_Select_On(uint8_t tRadioRegion)
{
	ITUWidget *S7_1_SEL = ituSceneFindWidget(&theScene, "S7_1_SEL");
	assert(S7_1_SEL);
	ITUWidget *S7_2_SEL = ituSceneFindWidget(&theScene, "S7_2_SEL");
	assert(S7_2_SEL);
	ITUWidget *S7_3_SEL = ituSceneFindWidget(&theScene, "S7_3_SEL");
	assert(S7_3_SEL);
	ITUWidget *S7_4_SEL = ituSceneFindWidget(&theScene, "S7_4_SEL");
	assert(S7_4_SEL);
	
	switch(tRadioRegion)
	{
		case 1:
			ituWidgetSetVisible(S7_1_SEL, true);
		break;

		case 2:
			ituWidgetSetVisible(S7_2_SEL, true);
		break;

		case 3:
			ituWidgetSetVisible(S7_3_SEL, true);
		break;

		case 4:
			ituWidgetSetVisible(S7_4_SEL, true);
		break;
	}
}

void S7_RegionClear(void)
{
	ITUWidget *S7_1_G = ituSceneFindWidget(&theScene, "S7_1_G");
	assert(S7_1_G);
	ITUWidget *S7_2_G = ituSceneFindWidget(&theScene, "S7_2_G");
	assert(S7_2_G);
	ITUWidget *S7_3_G = ituSceneFindWidget(&theScene, "S7_3_G");
	assert(S7_3_G);
	ITUWidget *S7_4_G = ituSceneFindWidget(&theScene, "S7_4_G");
	assert(S7_4_G);

	if(ituWidgetIsVisible(S7_1_G) == false)
		ituWidgetSetVisible(S7_1_G, true);

	if(ituWidgetIsVisible(S7_2_G) == false)
		ituWidgetSetVisible(S7_2_G, true);
	
	if(ituWidgetIsVisible(S7_3_G) == false)
		ituWidgetSetVisible(S7_3_G, true);

	if(ituWidgetIsVisible(S7_4_G) == false)
		ituWidgetSetVisible(S7_4_G, true);
}

void S7_Region_On(uint8_t tRadioRegion)
{
	ITUWidget *S7_1_G = ituSceneFindWidget(&theScene, "S7_1_G");
	assert(S7_1_G);
	ITUWidget *S7_2_G = ituSceneFindWidget(&theScene, "S7_2_G");
	assert(S7_2_G);
	ITUWidget *S7_3_G = ituSceneFindWidget(&theScene, "S7_3_G");
	assert(S7_3_G);
	ITUWidget *S7_4_G = ituSceneFindWidget(&theScene, "S7_4_G");
	assert(S7_4_G);

	switch(tRadioRegion)
	{
		case 1:
			ituWidgetSetVisible(S7_1_G, false);
		break;

		case 2:
			ituWidgetSetVisible(S7_2_G, false);
		break;

		case 3:
			ituWidgetSetVisible(S7_3_G, false);
		break;

		case 4:
			ituWidgetSetVisible(S7_4_G, false);
		break;
	}
}

void S7_Lang_Tile(uint8_t tLang)
{
	ITUBackground *S7_Name_ENG = ituSceneFindWidget(&theScene, "S7_Name_ENG");
	assert(S7_Name_ENG);
	ITUBackground *S7_Name_DEU = ituSceneFindWidget(&theScene, "S7_Name_DEU");
	assert(S7_Name_DEU);
	ITUBackground *S7_Name_FRA = ituSceneFindWidget(&theScene, "S7_Name_FRA");
	assert(S7_Name_FRA);
	ITUBackground *S7_Name_ITA = ituSceneFindWidget(&theScene, "S7_Name_ITA");
	assert(S7_Name_ITA);
	ITUBackground *S7_Name_ESP = ituSceneFindWidget(&theScene, "S7_Name_ESP");
	assert(S7_Name_ESP);

	switch(tLang)
	{
		case Lang_ENG:
		{
			ituWidgetSetVisible(S7_Name_DEU, false);
			ituWidgetSetVisible(S7_Name_FRA, false);
			ituWidgetSetVisible(S7_Name_ITA, false);
			ituWidgetSetVisible(S7_Name_ESP, false);
			
			ituWidgetSetVisible(S7_Name_ENG, true);
		}
		break;
		
		case Lang_DEU:
		{
			ituWidgetSetVisible(S7_Name_ENG, false);
			ituWidgetSetVisible(S7_Name_FRA, false);
			ituWidgetSetVisible(S7_Name_ITA, false);
			ituWidgetSetVisible(S7_Name_ESP, false);

			ituWidgetSetVisible(S7_Name_DEU, true);
		}
		break;
		
		case Lang_FRA:
		{
			ituWidgetSetVisible(S7_Name_ENG, false);
			ituWidgetSetVisible(S7_Name_DEU, false);
			ituWidgetSetVisible(S7_Name_ITA, false);
			ituWidgetSetVisible(S7_Name_ESP, false);

			ituWidgetSetVisible(S7_Name_FRA, true);
		}
		break;

		case Lang_ITA:
		{
			ituWidgetSetVisible(S7_Name_ENG, false);
			ituWidgetSetVisible(S7_Name_DEU, false);
			ituWidgetSetVisible(S7_Name_FRA, false);
			ituWidgetSetVisible(S7_Name_ESP, false);

			ituWidgetSetVisible(S7_Name_ITA, true);
		}
		break;

		case Lang_ESP:
		{
			ituWidgetSetVisible(S7_Name_ENG, false);
			ituWidgetSetVisible(S7_Name_DEU, false);
			ituWidgetSetVisible(S7_Name_FRA, false);
			ituWidgetSetVisible(S7_Name_ITA, false);

			ituWidgetSetVisible(S7_Name_ESP, true);
		}
		break;
	}
}