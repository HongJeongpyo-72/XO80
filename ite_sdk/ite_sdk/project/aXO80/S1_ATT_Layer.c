#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

/* widgets:
S1_ATT_Layer
S1ATT_BG
ATT10_ANI
ATT10_1
ATT10_0
ATT10_BTN
ATT9_ANI
ATT9_1
ATT9_0
ATT9_BTN
ATT8_ANI
ATT8_1
ATT8_0
ATT8_BTN
ATT7_ANI
ATT7_1
ATT7_0
ATT7_BTN
ATT6_ANI
ATT6_1
ATT6_0
ATT6_BTN
ATT5_ANI
ATT5_1
ATT5_0
ATT5_BTN
ATT4_ANI
ATT4_1
ATT4_0
ATT4_BTN
ATT3_ANI
ATT3_1
ATT3_0
ATT3_BTN
ATT2_ANI
ATT2_1
ATT2_0
ATT2_BTN
ATT1_ANI
ATT1_1
ATT1_0
ATT1_BTN
S1ATT_D7_BTN
S1ATT_D6_BTN
S1ATT_D5_BTN
*/



static ITUAnimation* ATT10_ANI;
static ITUIcon* ATT10_1;
static ITUIcon* ATT10_0;
static ITUButton* ATT10_BTN;
static ITUAnimation* ATT9_ANI;
static ITUIcon* ATT9_1;
static ITUIcon* ATT9_0;
static ITUButton* ATT9_BTN;
static ITUAnimation* ATT8_ANI;
static ITUIcon* ATT8_1;
static ITUIcon* ATT8_0;
static ITUButton* ATT8_BTN;
static ITUAnimation* ATT7_ANI;
static ITUIcon* ATT7_1;
static ITUIcon* ATT7_0;
static ITUButton* ATT7_BTN;
static ITUAnimation* ATT6_ANI;
static ITUIcon* ATT6_1;
static ITUIcon* ATT6_0;
static ITUButton* ATT6_BTN;
static ITUAnimation* ATT5_ANI;
static ITUIcon* ATT5_1;
static ITUIcon* ATT5_0;
static ITUButton* ATT5_BTN;
static ITUAnimation* ATT4_ANI;
static ITUIcon* ATT4_1;
static ITUIcon* ATT4_0;
static ITUButton* ATT4_BTN;
static ITUAnimation* ATT3_ANI;
static ITUIcon* ATT3_1;
static ITUIcon* ATT3_0;
static ITUButton* ATT3_BTN;
static ITUAnimation* ATT2_ANI;
static ITUIcon* ATT2_1;
static ITUIcon* ATT2_0;
static ITUButton* ATT2_BTN;
static ITUAnimation* ATT1_ANI;
static ITUIcon* ATT1_1;
static ITUIcon* ATT1_0;
static ITUButton* ATT1_BTN;

static ITUIcon* S1ATT_TopAdmin;
static ITUIcon* S1ATT_TopStandard;
static ITUText* S1ATT_TopUserName;

static uint8_t ATT1_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t ATT2_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t ATT3_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t ATT4_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t ATT5_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t ATT6_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t ATT7_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t ATT8_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t ATT9_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t ATT10_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down

static uint32_t S1ATT_SecTimer;
static uint8_t S1ATT_ExitCNT = 0;
static uint8_t S1ATT_Key_Name_Old = 0,S1ATT_Key_Name_New = 0;

uint8_t S1ATT_UserDisplay_Data = 0xFF;


extern uint8_t gGroup;

void ATT_Clear(void);
void ATT_Display(uint8_t tOldATT,uint8_t tNewATT);

bool S1ATT_UserDisplay(void);

extern void GotoMenuTop(void);
extern void GotoMenu7(void);
extern void GotoM7S1(uint8_t AUX_NO);

bool S1ATT_Enter(ITUWidget* widget, char* param)
{
	uint8_t cATT_State;
	
	cATT_State = M7_UserSet[M7_Group].ATT_State;

	S1ATT_UserDisplay_Data = 0xFF;
	SetWidget(ATT10_ANI,"ATT10_ANI");
	SetWidget(ATT10_1,"ATT10_1");
	SetWidget(ATT10_0,"ATT10_0");
	SetWidget(ATT10_BTN,"ATT10_BTN");
	SetWidget(ATT9_ANI,"ATT9_ANI");
	SetWidget(ATT9_1,"ATT9_1");
	SetWidget(ATT9_0,"ATT9_0");
	SetWidget(ATT9_BTN,"ATT9_BTN");
	SetWidget(ATT8_ANI,"ATT8_ANI");
	SetWidget(ATT8_1,"ATT8_1");
	SetWidget(ATT8_0,"ATT8_0");
	SetWidget(ATT8_BTN,"ATT8_BTN");
	SetWidget(ATT7_ANI,"ATT7_ANI");
	SetWidget(ATT7_1,"ATT7_1");
	SetWidget(ATT7_0,"ATT7_0");
	SetWidget(ATT7_BTN,"ATT7_BTN");
	SetWidget(ATT6_ANI,"ATT6_ANI");
	SetWidget(ATT6_1,"ATT6_1");
	SetWidget(ATT6_0,"ATT6_0");
	SetWidget(ATT6_BTN,"ATT6_BTN");
	SetWidget(ATT5_ANI,"ATT5_ANI");
	SetWidget(ATT5_1,"ATT5_1");
	SetWidget(ATT5_0,"ATT5_0");
	SetWidget(ATT5_BTN,"ATT5_BTN");
	SetWidget(ATT4_ANI,"ATT4_ANI");
	SetWidget(ATT4_1,"ATT4_1");
	SetWidget(ATT4_0,"ATT4_0");
	SetWidget(ATT4_BTN,"ATT4_BTN");
	SetWidget(ATT3_ANI,"ATT3_ANI");
	SetWidget(ATT3_1,"ATT3_1");
	SetWidget(ATT3_0,"ATT3_0");
	SetWidget(ATT3_BTN,"ATT3_BTN");
	SetWidget(ATT2_ANI,"ATT2_ANI");
	SetWidget(ATT2_1,"ATT2_1");
	SetWidget(ATT2_0,"ATT2_0");
	SetWidget(ATT2_BTN,"ATT2_BTN");
	SetWidget(ATT1_ANI,"ATT1_ANI");
	SetWidget(ATT1_1,"ATT1_1");
	SetWidget(ATT1_0,"ATT1_0");
	SetWidget(ATT1_BTN,"ATT1_BTN");
	SetWidget(S1ATT_TopAdmin,"S1ATT_TopAdmin");
	SetWidget(S1ATT_TopStandard,"S1ATT_TopStandard");
	SetWidget(S1ATT_TopUserName,"S1ATT_TopUserName");


	
	ATT_Clear();
	ATT_Display(0xff,cATT_State);

	S1ATT_UserDisplay();

	S1ATT_SecTimer = gMainTimerCnt_10m;
	S1ATT_ExitCNT = 0;

    return true;
}

bool S1ATT_Timer(ITUWidget* widget, char* param)
{
#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	
	if((gMainTimerCnt_10m - S1ATT_SecTimer) >= 100)
	{
		if(S1ATT_ExitCNT < 30)
			S1ATT_ExitCNT++;
		else if(S1ATT_ExitCNT == 30)
		{
			S1ATT_ExitCNT = 100;
			printf("\r\nS1ATT Menu 30Sec Exit > goto Main\r\n");
			GotoMain(MAIN_MODE_NORMAL); 
		}
			
		S1ATT_SecTimer = gMainTimerCnt_10m;
	}

	S1ATT_UserDisplay();
		
    return true;
}

bool S1ATT_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t tJogWheel = 0;
	uint8_t tATTNum = 0;
	static uint8_t wATTNum = 0;

	S1ATT_ExitCNT = 0;
	tJogWheel = param[0];

	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
		case LANDSCAPE_1:
		case LANDSCAPE_2:
		{
			if(tJogWheel == JOG_DIAL_ROT_LEFT)
			{
				uint8_t dest_left[10] = {10,1,2,3,4,5,6,7,8,9};
				// att change 1>10,2>1,3>2,
				//            4>3,5>4,6>5,
				//            7>6,8>7,9>8,
				//            10>9
				tATTNum = M7_UserSet[M7_Group].ATT_State;
				wATTNum = dest_left[tATTNum-1];

				ATT_Display(tATTNum,wATTNum);
				M7_UserSet[M7_Group].ATT_State = wATTNum;
			}
			else if(tJogWheel == JOG_DIAL_ROT_RIGHT)
			{
				uint8_t dest_right[10] = {2,3,4,5,6,7,8,9,10,1};
				// att change 1>2,2>3,3>4,
				//            4>5,5>6,6>7,
				//            7>8,8>9,9>10,
				//            10>1
				tATTNum = M7_UserSet[M7_Group].ATT_State;
				wATTNum = dest_right[tATTNum-1];

				ATT_Display(tATTNum,wATTNum);
				M7_UserSet[M7_Group].ATT_State = wATTNum;
			}
		}
		break;	
/*
		case LANDSCAPE_1:
		case LANDSCAPE_2:
		{
			
		}
		break;
*/
	}

    return true;
}

bool S1ATT_JogButton(ITUWidget* widget, char* param)
{
	uint8_t tJogButton = 0;
	uint8_t tATTNum = 0;
	static uint8_t cATTNum = 0;


	S1ATT_ExitCNT = 0;
	
	tJogButton = param[0];
	
	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
		{
			switch(tJogButton)
			{
				case JOG_DIAL_KEY_UP:
				{
					uint8_t dest_up[10] = {10,8,9,1,2,3,4,5,6,7};
					// att change 1>10,2>8,3>9,
					//            4>1,5>2,6>3,
					//            7>4,8>5,9>6,
					//            10>7
					tATTNum = M7_UserSet[M7_Group].ATT_State;
					cATTNum = dest_up[tATTNum-1];

					ATT_Display(tATTNum,cATTNum);
					M7_UserSet[M7_Group].ATT_State = cATTNum;
				}					
				break;
				
				case JOG_DIAL_KEY_DOWN:
				{
					uint8_t dest_down[10] = {4,5,6,7,8,9,10,2,3,1};
					// att change 1>4,2>5,3>6,
					//            4>7,5>8,6>9,
					//            7>10,8>2,9>3,
					//            10>1

					tATTNum = M7_UserSet[M7_Group].ATT_State;
					cATTNum = dest_down[tATTNum-1];

					ATT_Display(tATTNum,cATTNum);
					M7_UserSet[M7_Group].ATT_State = cATTNum;
				}
				break;
				
				case JOG_DIAL_KEY_LEFT:
				{
					uint8_t dest_left[10] = {3,1,2,6,4,5,9,7,8,9};
					// att change 1>3,2>1,3>2,
					//            4>6,5>4,6>5,
					//            7>9,8>7,9>8,
					//            10>9
					tATTNum = M7_UserSet[M7_Group].ATT_State;
					cATTNum = dest_left[tATTNum-1];

					ATT_Display(tATTNum,cATTNum);
					M7_UserSet[M7_Group].ATT_State = cATTNum;
				}
				break;
				
				case JOG_DIAL_KEY_RIGHT:
				{
					uint8_t dest_right[10] = {2,3,1,5,6,4,8,9,7,8};
					// att change 1>2,2>3,3>1,
					//            4>5,5>6,6>4,
					//            7>8,8>9,9>7,
					//            10>8
					tATTNum = M7_UserSet[M7_Group].ATT_State;
					cATTNum = dest_right[tATTNum-1];

					ATT_Display(tATTNum,cATTNum);
					M7_UserSet[M7_Group].ATT_State = cATTNum;
				}
				break;
				
				case JOG_DIAL_KEY_ENTER_ON:

					S1ATT_Key_Name_Old = S1ATT_Key_Name_New;
					S1ATT_Key_Name_New = JOG_DIAL_KEY_ENTER_ON;

					//M7_UserSet[M7_Group].ATT_State = cATTNum;
					
#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif

				break;
				
				case JOG_DIAL_KEY_ENTER_OFF:
				{
					if((S1ATT_Key_Name_Old != S1ATT_Key_Name_New)&&(S1ATT_Key_Name_New == JOG_DIAL_KEY_ENTER_ON))
					{
						S1ATT_Key_Name_Old = S1ATT_Key_Name_New;
						S1ATT_Key_Name_New = JOG_DIAL_KEY_ENTER_OFF;

						ITULayer *tBranchLayer = ituSceneFindWidget(&theScene, "M7_S1");
						assert(tBranchLayer);
						ituLayerGoto(tBranchLayer);
					}
				}	
				break;
			}
		}
		break;

		case LANDSCAPE_1:
		case LANDSCAPE_2:
		{
			switch(tJogButton)
			{
				case JOG_DIAL_KEY_UP:
				{
					uint8_t dest_up[10] = {6,7,8,9,10,1,2,3,4,5};
					// att change 1>6,2>7,3>8,4>9,5>10
					//            6>1,7>2,8>3,9>4,10>5

					tATTNum = M7_UserSet[M7_Group].ATT_State;
					cATTNum = dest_up[tATTNum-1];

					ATT_Display(tATTNum,cATTNum);
					M7_UserSet[M7_Group].ATT_State = cATTNum;
				}					
				break;
				
				case JOG_DIAL_KEY_DOWN:
				{
					uint8_t dest_down[10] = {6,7,8,9,10,1,2,3,4,5};
					// att change 1>6,2>7,3>8,4>9,5>10
					//            6>1,7>2,8>3,9>4,10>5

					tATTNum = M7_UserSet[M7_Group].ATT_State;
					cATTNum = dest_down[tATTNum-1];

					ATT_Display(tATTNum,cATTNum);
					M7_UserSet[M7_Group].ATT_State = cATTNum;
				}
				break;
				
				case JOG_DIAL_KEY_LEFT:
				{
					uint8_t dest_left[10] = {5,1,2,3,4,10,6,7,8,9};
					// att change 1>5 ,2>1,3>2,4>3,5>4
					//            6>10,7>6,8>7,9>8,10>9

					tATTNum = M7_UserSet[M7_Group].ATT_State;
					cATTNum = dest_left[tATTNum-1];

					ATT_Display(tATTNum,cATTNum);
					M7_UserSet[M7_Group].ATT_State = cATTNum;
				}
				break;
				
				case JOG_DIAL_KEY_RIGHT:
				{
					uint8_t dest_right[10] = {2,3,4,5,1,7,8,9,10,6};
					// att change 1>2,2>3,3>4,4>5 ,5>1
					//            6>7,7>8,8>9,9>10,10>6

					tATTNum = M7_UserSet[M7_Group].ATT_State;
					cATTNum = dest_right[tATTNum-1];

					ATT_Display(tATTNum,cATTNum);
					M7_UserSet[M7_Group].ATT_State = cATTNum;
				}
				break;
				
				case JOG_DIAL_KEY_ENTER_ON:

					S1ATT_Key_Name_Old = S1ATT_Key_Name_New;
					S1ATT_Key_Name_New = JOG_DIAL_KEY_ENTER_ON;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
				break;
				
				case JOG_DIAL_KEY_ENTER_OFF:
				{
					if((S1ATT_Key_Name_Old != S1ATT_Key_Name_New)&&(S1ATT_Key_Name_New == JOG_DIAL_KEY_ENTER_ON))
					{
						S1ATT_Key_Name_Old = S1ATT_Key_Name_New;
						S1ATT_Key_Name_New = JOG_DIAL_KEY_ENTER_OFF;

						ITULayer *tBranchLayer = ituSceneFindWidget(&theScene, "M7_S1");
						assert(tBranchLayer);
						ituLayerGoto(tBranchLayer);
					}
				}	
				break;
			}

		}
		break;
	}

    return true;
}

bool S1ATT_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t tJogSwitch = 0;
	
 	S1ATT_ExitCNT = 0;

	tJogSwitch = param[0];

	printf("S1ATT_JogSwitch %d\r\n",tJogSwitch);

	if(tJogSwitch == JOG_DIAL_SW_ESC_ON)
	{

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
		XO_Save_Config(M7_UserIndex,true,0);
#else
		HT_Save_Config(1);
#endif
	}
	else if(tJogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
		GotoM7S1(gGroup);
	}
	else if(tJogSwitch == JOG_DIAL_SW_HOME)
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}

    return true;
}

bool S1ATT_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t cATT_State;
	uint8_t tATT_Sel = 0;
	
	S1ATT_ExitCNT = 0;

	tATT_Sel = (uint8_t)atoi(param);
	cATT_State = M7_UserSet[M7_Group].ATT_State;
	
	ATT_Display(cATT_State,tATT_Sel);
	M7_UserSet[M7_Group].ATT_State = tATT_Sel;
	
    return true;
}

bool S1ATT_MouseUp(ITUWidget* widget, char* param)
{

	S1ATT_ExitCNT = 0;
	
    return true;
}

bool ATT10_ANIStop(ITUWidget* widget, char* param)
{
	if(ATT10_Scaleflag == 1)
	{
		ATT10_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down

		ituAnimationGoto(ATT10_ANI, 1);
		ituWidgetSetVisible(ATT10_BTN, false);
	}
	else if(ATT10_Scaleflag == 2)
	{
		ATT10_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		
		ituAnimationGoto(ATT10_ANI, 0);
		ituWidgetSetVisible(ATT10_BTN, true);
	}

    return true;
}

bool ATT9_ANIStop(ITUWidget* widget, char* param)
{
	if(ATT9_Scaleflag == 1)
	{
		ATT9_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down

		ituAnimationGoto(ATT9_ANI, 1);
		ituWidgetSetVisible(ATT9_BTN, false);
	}
	else if(ATT9_Scaleflag == 2)
	{
		ATT9_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		
		ituAnimationGoto(ATT9_ANI, 0);
		ituWidgetSetVisible(ATT9_BTN, true);
	}

    return true;
}

bool ATT8_ANIStop(ITUWidget* widget, char* param)
{
	if(ATT8_Scaleflag == 1)
	{
		ATT8_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down

		ituAnimationGoto(ATT8_ANI, 1);
		ituWidgetSetVisible(ATT8_BTN, false);
	}
	else if(ATT8_Scaleflag == 2)
	{
		ATT8_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		
		ituAnimationGoto(ATT8_ANI, 0);
		ituWidgetSetVisible(ATT8_BTN, true);
	}

    return true;
}

bool ATT7_ANIStop(ITUWidget* widget, char* param)
{
	if(ATT7_Scaleflag == 1)
	{
		ATT7_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down

		ituAnimationGoto(ATT7_ANI, 1);
		ituWidgetSetVisible(ATT7_BTN, false);
	}
	else if(ATT7_Scaleflag == 2)
	{
		ATT7_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		
		ituAnimationGoto(ATT7_ANI, 0);
		ituWidgetSetVisible(ATT7_BTN, true);
	}

    return true;
}

bool ATT6_ANIStop(ITUWidget* widget, char* param)
{
	if(ATT6_Scaleflag == 1)
	{
		ATT6_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down

		ituAnimationGoto(ATT6_ANI, 1);
		ituWidgetSetVisible(ATT6_BTN, false);
	}
	else if(ATT6_Scaleflag == 2)
	{
		ATT6_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		
		ituAnimationGoto(ATT6_ANI, 0);
		ituWidgetSetVisible(ATT6_BTN, true);
	}

    return true;
}

bool ATT5_ANIStop(ITUWidget* widget, char* param)
{
	if(ATT5_Scaleflag == 1)
	{
		ATT5_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down

		ituAnimationGoto(ATT5_ANI, 1);
		ituWidgetSetVisible(ATT5_BTN, false);
	}
	else if(ATT5_Scaleflag == 2)
	{
		ATT5_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		
		ituAnimationGoto(ATT5_ANI, 0);
		ituWidgetSetVisible(ATT5_BTN, true);
	}

    return true;
}

bool ATT4_ANIStop(ITUWidget* widget, char* param)
{
	if(ATT4_Scaleflag == 1)
	{
		ATT4_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down

		ituAnimationGoto(ATT4_ANI, 1);
		ituWidgetSetVisible(ATT4_BTN, false);
	}
	else if(ATT4_Scaleflag == 2)
	{
		ATT4_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		
		ituAnimationGoto(ATT4_ANI, 0);
		ituWidgetSetVisible(ATT4_BTN, true);
	}

    return true;
}

bool ATT3_ANIStop(ITUWidget* widget, char* param)
{
	if(ATT3_Scaleflag == 1)
	{
		ATT3_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down

		ituAnimationGoto(ATT3_ANI, 1);
		ituWidgetSetVisible(ATT3_BTN, false);
	}
	else if(ATT3_Scaleflag == 2)
	{
		ATT3_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		
		ituAnimationGoto(ATT3_ANI, 0);
		ituWidgetSetVisible(ATT3_BTN, true);
	}

    return true;
}

bool ATT2_ANIStop(ITUWidget* widget, char* param)
{
	if(ATT2_Scaleflag == 1)
	{
		ATT2_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down

		ituAnimationGoto(ATT2_ANI, 1);
		ituWidgetSetVisible(ATT2_BTN, false);
	}
	else if(ATT2_Scaleflag == 2)
	{
		ATT2_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		
		ituAnimationGoto(ATT2_ANI, 0);
		ituWidgetSetVisible(ATT2_BTN, true);
	}

    return true;
}

bool ATT1_ANIStop(ITUWidget* widget, char* param)
{
	if(ATT1_Scaleflag == 1)
	{
		ATT1_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down

		ituAnimationGoto(ATT1_ANI, 1);
		ituWidgetSetVisible(ATT1_BTN, false);
	}
	else if(ATT1_Scaleflag == 2)
	{
		ATT1_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		
		ituAnimationGoto(ATT1_ANI, 0);
		ituWidgetSetVisible(ATT1_BTN, true);
	}

    return true;
}

bool S1ATTD7_MouseDn(ITUWidget* widget, char* param)
{
#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
	XO_Save_Config(M7_UserIndex,true,0);
#else
	HT_Save_Config(1);
#endif
		
    return true;
}

bool S1ATTD7_MouseUp(ITUWidget* widget, char* param)
{

	GotoM7S1(gGroup);

/*
	ITULayer *tBranchLayer = ituSceneFindWidget(&theScene, "M7_S1");
	assert(tBranchLayer);
	ituLayerGoto(tBranchLayer);
*/
    return true;
}

bool S1ATTD6_MouseUp(ITUWidget* widget, char* param)
{
#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
	XO_Save_Config(M7_UserIndex,true,0);
#else
	HT_Save_Config(1);
#endif

	GotoMain(MAIN_MODE_NORMAL); 

    return true;
}

bool S1ATTD5_MouseDn(ITUWidget* widget, char* param)
{
#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
	XO_Save_Config(M7_UserIndex,true,0);
#else
	HT_Save_Config(1);
#endif
	
    return true;
}

bool S1ATTD5_MouseUp(ITUWidget* widget, char* param)
{
	GotoM7S1(gGroup);
	
/*	
	ITULayer *tBranchLayer = ituSceneFindWidget(&theScene, "M7_S1");
	assert(tBranchLayer);
	ituLayerGoto(tBranchLayer);
*/	
    return true;
}

bool S1ATT_UserDisplay(void)
{
	uint8_t tDestName[100];
	
	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
		return 0;
	
	if (S1ATT_UserDisplay_Data!=User_RegInfo[M7_UserIndex].RegFlag)
	{	
		if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
		{
			ituWidgetSetVisible(S1ATT_TopStandard,false);
			ituWidgetSetVisible(S1ATT_TopAdmin,true);
		}
		else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
		{
			ituWidgetSetVisible(S1ATT_TopAdmin,false);
			ituWidgetSetVisible(S1ATT_TopStandard,true);
		}
		else
		{
			ituWidgetSetVisible(S1ATT_TopAdmin,false);
			ituWidgetSetVisible(S1ATT_TopStandard,false);
		}
		S1ATT_UserDisplay_Data=User_RegInfo[M7_UserIndex].RegFlag;
	}

	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(S1ATT_TopUserName, tDestName);

		ituWidgetSetVisible(S1ATT_TopUserName,true);
	}
	else
	{
		ituWidgetSetVisible(S1ATT_TopUserName,false);
	}
}

void ATT_Clear(void)
{
	if(ituWidgetIsVisible(ATT1_BTN) == false)
	{
		ituWidgetSetVisible(ATT1_ANI, true);

		ATT1_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(ATT1_ANI, 1);
	}

	if(ituWidgetIsVisible(ATT2_BTN) == false)
	{
		ituWidgetSetVisible(ATT2_ANI, true);

		ATT2_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(ATT2_ANI, 1);
	}

	if(ituWidgetIsVisible(ATT3_BTN) == false)
	{
		ituWidgetSetVisible(ATT3_ANI, true);

		ATT3_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(ATT3_ANI, 1);
	}

	if(ituWidgetIsVisible(ATT4_BTN) == false)
	{
		ituWidgetSetVisible(ATT4_ANI, true);

		ATT4_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(ATT4_ANI, 1);
	}

	if(ituWidgetIsVisible(ATT5_BTN) == false)
	{
		ituWidgetSetVisible(ATT5_ANI, true);

		ATT5_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(ATT5_ANI, 1);
	}

	if(ituWidgetIsVisible(ATT6_BTN) == false)
	{
		ituWidgetSetVisible(ATT6_ANI, true);

		ATT6_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(ATT6_ANI, 1);
	}

	if(ituWidgetIsVisible(ATT7_BTN) == false)
	{
		ituWidgetSetVisible(ATT7_ANI, true);

		ATT7_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(ATT7_ANI, 1);
	}

	if(ituWidgetIsVisible(ATT8_BTN) == false)
	{
		ituWidgetSetVisible(ATT8_ANI, true);

		ATT8_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(ATT8_ANI, 1);
	}

	if(ituWidgetIsVisible(ATT9_BTN) == false)
	{
		ituWidgetSetVisible(ATT9_ANI, true);

		ATT9_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(ATT9_ANI, 1);
	}

	if(ituWidgetIsVisible(ATT10_BTN) == false)
	{
		ituWidgetSetVisible(ATT10_ANI, true);

		ATT10_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(ATT10_ANI, 1);
	}
}

void ATT_Display(uint8_t tOldATT,uint8_t tNewATT)
{

	if(tOldATT != tNewATT)
	{
		// scale down
		switch(tOldATT)
		{
			case 1:
				ituWidgetSetVisible(ATT1_ANI, true);

				ATT1_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(ATT1_ANI, 1);
			break;
			
			case 2:
				ituWidgetSetVisible(ATT2_ANI, true);

				ATT2_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(ATT2_ANI, 1);
			break;

			case 3:
				ituWidgetSetVisible(ATT3_ANI, true);

				ATT3_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(ATT3_ANI, 1);
			break;

			case 4:
				ituWidgetSetVisible(ATT4_ANI, true);

				ATT4_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(ATT4_ANI, 1);
			break;

			case 5:
				ituWidgetSetVisible(ATT5_ANI, true);

				ATT5_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(ATT5_ANI, 1);
			break;

			case 6:
				ituWidgetSetVisible(ATT6_ANI, true);

				ATT6_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(ATT6_ANI, 1);
			break;

			case 7:
				ituWidgetSetVisible(ATT7_ANI, true);

				ATT7_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(ATT7_ANI, 1);
			break;

			case 8:
				ituWidgetSetVisible(ATT8_ANI, true);

				ATT8_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(ATT8_ANI, 1);
			break;

			case 9:
				ituWidgetSetVisible(ATT9_ANI, true);

				ATT9_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(ATT9_ANI, 1);
			break;

			case 10:
				ituWidgetSetVisible(ATT10_ANI, true);

				ATT10_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationReversePlay(ATT10_ANI, 1);
			break;
			
			default:
			break;
		}
		
		// scale up
		switch(tNewATT)
		{
			case 1:
				ituWidgetSetVisible(ATT1_BTN, false);
				ituWidgetSetVisible(ATT1_ANI, true);

				ATT1_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(ATT1_ANI,0);
				
				//ituWidgetSetVisible(ATT1_BTN, true);
				ituWidgetSetVisible(ATT2_BTN, true);
				ituWidgetSetVisible(ATT3_BTN, true);
				ituWidgetSetVisible(ATT4_BTN, true);
				ituWidgetSetVisible(ATT5_BTN, true);
				ituWidgetSetVisible(ATT6_BTN, true);
				ituWidgetSetVisible(ATT7_BTN, true);
				ituWidgetSetVisible(ATT8_BTN, true);
				ituWidgetSetVisible(ATT9_BTN, true);
				ituWidgetSetVisible(ATT10_BTN, true);
			break;
			
			case 2:
				ituWidgetSetVisible(ATT2_BTN, false);
				ituWidgetSetVisible(ATT2_ANI, true);

				ATT2_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(ATT2_ANI,0);

				ituWidgetSetVisible(ATT1_BTN, true);
				//ituWidgetSetVisible(ATT2_BTN, true);
				ituWidgetSetVisible(ATT3_BTN, true);
				ituWidgetSetVisible(ATT4_BTN, true);
				ituWidgetSetVisible(ATT5_BTN, true);
				ituWidgetSetVisible(ATT6_BTN, true);
				ituWidgetSetVisible(ATT7_BTN, true);
				ituWidgetSetVisible(ATT8_BTN, true);
				ituWidgetSetVisible(ATT9_BTN, true);
				ituWidgetSetVisible(ATT10_BTN, true);
			break;

			case 3:
				ituWidgetSetVisible(ATT3_BTN, false);
				ituWidgetSetVisible(ATT3_ANI, true);

				ATT3_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(ATT3_ANI,0);

				ituWidgetSetVisible(ATT1_BTN, true);
				ituWidgetSetVisible(ATT2_BTN, true);
				//ituWidgetSetVisible(ATT3_BTN, true);
				ituWidgetSetVisible(ATT4_BTN, true);
				ituWidgetSetVisible(ATT5_BTN, true);
				ituWidgetSetVisible(ATT6_BTN, true);
				ituWidgetSetVisible(ATT7_BTN, true);
				ituWidgetSetVisible(ATT8_BTN, true);
				ituWidgetSetVisible(ATT9_BTN, true);
				ituWidgetSetVisible(ATT10_BTN, true);
			break;

			case 4:
				ituWidgetSetVisible(ATT4_BTN, false);
				ituWidgetSetVisible(ATT4_ANI, true);

				ATT4_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(ATT4_ANI,0);

				ituWidgetSetVisible(ATT1_BTN, true);
				ituWidgetSetVisible(ATT2_BTN, true);
				ituWidgetSetVisible(ATT3_BTN, true);
				//ituWidgetSetVisible(ATT4_BTN, true);
				ituWidgetSetVisible(ATT5_BTN, true);
				ituWidgetSetVisible(ATT6_BTN, true);
				ituWidgetSetVisible(ATT7_BTN, true);
				ituWidgetSetVisible(ATT8_BTN, true);
				ituWidgetSetVisible(ATT9_BTN, true);
				ituWidgetSetVisible(ATT10_BTN, true);
			break;

			case 5:
				ituWidgetSetVisible(ATT5_BTN, false);
				ituWidgetSetVisible(ATT5_ANI, true);

				ATT5_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(ATT5_ANI,0);

				ituWidgetSetVisible(ATT1_BTN, true);
				ituWidgetSetVisible(ATT2_BTN, true);
				ituWidgetSetVisible(ATT3_BTN, true);
				ituWidgetSetVisible(ATT4_BTN, true);
				//ituWidgetSetVisible(ATT5_BTN, true);
				ituWidgetSetVisible(ATT6_BTN, true);
				ituWidgetSetVisible(ATT7_BTN, true);
				ituWidgetSetVisible(ATT8_BTN, true);
				ituWidgetSetVisible(ATT9_BTN, true);
				ituWidgetSetVisible(ATT10_BTN, true);
			break;

			case 6:
				ituWidgetSetVisible(ATT6_BTN, false);
				ituWidgetSetVisible(ATT6_ANI, true);

				ATT6_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(ATT6_ANI,0);

				ituWidgetSetVisible(ATT1_BTN, true);
				ituWidgetSetVisible(ATT2_BTN, true);
				ituWidgetSetVisible(ATT3_BTN, true);
				ituWidgetSetVisible(ATT4_BTN, true);
				ituWidgetSetVisible(ATT5_BTN, true);
				//ituWidgetSetVisible(ATT6_BTN, true);
				ituWidgetSetVisible(ATT7_BTN, true);
				ituWidgetSetVisible(ATT8_BTN, true);
				ituWidgetSetVisible(ATT9_BTN, true);
				ituWidgetSetVisible(ATT10_BTN, true);
			break;

			case 7:
				ituWidgetSetVisible(ATT7_BTN, false);
				ituWidgetSetVisible(ATT7_ANI, true);

				ATT7_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(ATT7_ANI,0);

				ituWidgetSetVisible(ATT1_BTN, true);
				ituWidgetSetVisible(ATT2_BTN, true);
				ituWidgetSetVisible(ATT3_BTN, true);
				ituWidgetSetVisible(ATT4_BTN, true);
				ituWidgetSetVisible(ATT5_BTN, true);
				ituWidgetSetVisible(ATT6_BTN, true);
				//ituWidgetSetVisible(ATT7_BTN, true);
				ituWidgetSetVisible(ATT8_BTN, true);
				ituWidgetSetVisible(ATT9_BTN, true);
				ituWidgetSetVisible(ATT10_BTN, true);
			break;

			case 8:
				ituWidgetSetVisible(ATT8_BTN, false);
				ituWidgetSetVisible(ATT8_ANI, true);

				ATT8_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(ATT8_ANI,0);

				ituWidgetSetVisible(ATT1_BTN, true);
				ituWidgetSetVisible(ATT2_BTN, true);
				ituWidgetSetVisible(ATT3_BTN, true);
				ituWidgetSetVisible(ATT4_BTN, true);
				ituWidgetSetVisible(ATT5_BTN, true);
				ituWidgetSetVisible(ATT6_BTN, true);
				ituWidgetSetVisible(ATT7_BTN, true);
				//ituWidgetSetVisible(ATT8_BTN, true);
				ituWidgetSetVisible(ATT9_BTN, true);
				ituWidgetSetVisible(ATT10_BTN, true);
			break;

			case 9:
				ituWidgetSetVisible(ATT9_BTN, false);
				ituWidgetSetVisible(ATT9_ANI, true);

				ATT9_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(ATT9_ANI,0);

				ituWidgetSetVisible(ATT1_BTN, true);
				ituWidgetSetVisible(ATT2_BTN, true);
				ituWidgetSetVisible(ATT3_BTN, true);
				ituWidgetSetVisible(ATT4_BTN, true);
				ituWidgetSetVisible(ATT5_BTN, true);
				ituWidgetSetVisible(ATT6_BTN, true);
				ituWidgetSetVisible(ATT7_BTN, true);
				ituWidgetSetVisible(ATT8_BTN, true);
				//ituWidgetSetVisible(ATT9_BTN, true);
				ituWidgetSetVisible(ATT10_BTN, true);
			break;

			case 10:
				ituWidgetSetVisible(ATT10_BTN, false);
				ituWidgetSetVisible(ATT10_ANI, true);

				ATT10_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(ATT10_ANI,0);

				ituWidgetSetVisible(ATT1_BTN, true);
				ituWidgetSetVisible(ATT2_BTN, true);
				ituWidgetSetVisible(ATT3_BTN, true);
				ituWidgetSetVisible(ATT4_BTN, true);
				ituWidgetSetVisible(ATT5_BTN, true);
				ituWidgetSetVisible(ATT6_BTN, true);
				ituWidgetSetVisible(ATT7_BTN, true);
				ituWidgetSetVisible(ATT8_BTN, true);
				ituWidgetSetVisible(ATT9_BTN, true);
				//ituWidgetSetVisible(ATT10_BTN, true);
			break;
			
			default:
			break;
		}
	}
}

