
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"
#include "Menu_Layer.h"

#define MenuB_Engine				0
#define MenuB_IO					1
#define MenuB_Option				2
#define MenuB_Cal				    3
#define MenuB_UserDefine            4

static uint8_t gMBTop_Choice_Index = 0;			// default 0 : Engine menu.
static bool g_bReadyToMouseDown = true;

uint8_t gMB_Unit = 0; //0:ST 1: US
uint8_t gMB_Language = 0; //0:US  1:German 2:French 3:Italian 4:Spain

static uint32_t MB_gMT_Timer_100m;	//100 milli sec timer.//go layer button check.
static uint32_t MB_gMT_Timer_1000m; //1 sec timer.

static uint8_t  MB_gMT_Timer_Exit = 0;			// Sec Count
static uint32_t  MB_gMT_DeepTimer = 0;

static uint8_t MB_MT_1a_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t MB_MT_1b_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t MB_MT_2_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t MB_MT_3_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t MB_MT_4_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down

static uint8_t MB_MT_DeepCHK = 0;
static uint8_t MB_MT_DeepCHK_JOG = 0;	// 2024-04-11 add whlim
uint8_t MB_MT_D5_Flag = 0; 			// 2024-04-11 add whlim
uint8_t MB_MT_D7_Flag = 0; 			// 2024-04-11 add whlim

static uint32_t MB_gMT_Reset_Timer_1000m; //1 sec timer. 2024-04-22 add whlim
static bool MB_MT_ResetIcon_Flag = false; 			// 2024-04-22 add whlim
static bool MB_MT_Reset_start_Flag = false; 			// 2024-04-22 add whlim

static uint32_t MB_Timer_tJog = 0;
char PARA[2];

extern uint8_t Get_PL_Flag(void);

void MBTop_InitControl();
void MBTop_ChoiceMenuClear(void);
bool MB1_BTN_MouseDown(ITUWidget* widget, char* param);
bool MB1_BTN_MouseUp(ITUWidget* widget, char* param);

void MBTop_MoveToSelectedMenu(int8_t nIndex);

int8_t MBTop_Button_GetNextIndex(uint8_t nIndex);
int8_t MBTop_Button_GetPrevIndex(uint8_t nIndex);
int8_t MBTop_Button_GetLeftRightIndex(uint8_t nIndex);

void MBTop_MenuTabNext();
void MBTop_MenuTabPrev();
void MBTop_MenuTabLeftRight();

//chpark.24.02.16
void MBTop_MenuTabUp();
void MBTop_MenuTabDown();

bool   MBTop_Button_Select(uint8_t nIndex);
//====

static ITUWidget* MB1_1a_BTN;
static ITUWidget* MB1_1b_BTN;

static ITUAnimation* MB1_1a_ANI;
static ITUAnimation* MB1_1b_ANI;

static ITUWidget* MB1_1a_ZoomBTN;
static ITUWidget* MB1_1b_ZoomBTN;
//====

static ITUWidget* 	 MB1_2_BTN;
static ITUAnimation* MB1_2_ANI;
static ITUWidget* 	 MB1_2_ZoomBTN;
//====

static ITUWidget* 	 MB1_3_BTN;
static ITUAnimation* MB1_3_ANI;
static ITUWidget* 	 MB1_3_ZoomBTN;

static ITUWidget* 	 MB1_4_BTN;
static ITUAnimation* MB1_4_ANI;
static ITUWidget* 	 MB1_4_ZoomBTN;
//====
static ITUWidget* 	 MBT_Lock;

// reset icon 2024-04-22 add whlim
static ITUWidget* 	 MB1_ResetIcon;

static bool MBTop_ReadyToMoveLayer[4];
static bool MBTop_GoToMoveLayer[4];

extern bool DOUBLE_KEY_ENABLE_FLAG;	// JPHong 0727 #10956

bool MB_ReadyToJog = true;//chpark.24.04.26
bool MB_Chk_SwitchBtn[2];

void GotoBackScreenT(bool bReadyToJog)	// JPHong 2023-05-18. //chpark.24.04.26
{
    ITULayer *MenuBTop_Layer = ituSceneFindWidget(&theScene, "MenuBTop_Layer");
    assert(MenuBTop_Layer);
    ituLayerGoto(MenuBTop_Layer);

	MB_ReadyToJog = bReadyToJog; //chpark.24.04.26
	MB_Chk_SwitchBtn[0] = true; //chpark.24.09.19
	MB_Chk_SwitchBtn[1] = true; //chpark.24.09.19
	
}

bool MenuBTop_Leave(ITUWidget* widget, char* param)
{
	XO_ituButtonSetMode_Bt(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
	XO_ituButtonSetMode_TP(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
	DOUBLE_KEY_ENABLE_FLAG = 1;	// JPHong 0727 #10956
    return true;
}


bool MenuBTop_Enter(ITUWidget* widget, char* param)
{
	XO_ituButtonSetMode_Bt(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
	XO_ituButtonSetMode_TP(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
	MB_MT_D5_Flag = 0; 			// 2024-04-11 add whlim
	MB_MT_D7_Flag = 0; 			// 2024-04-11 add whlim
	MB_MT_DeepCHK_JOG = 0;		// 2024-04-11 add whlim
	MB_MT_ResetIcon_Flag = false;	// 2024-04-22 add whlim
	MB_MT_Reset_start_Flag = false; // 2024-04-22 add whlim

	Main_Disp_Mode = MAIN_MODE_BACK;	// JPHong 2023-05-18

	gMB_Language = Lang_Flag[M7_UserIndex];
	
	gMB_Unit = SIUS_SEL_Flag;

	MB_gMT_Timer_Exit = 0;
	MB_gMT_DeepTimer = gMainTimerCnt_10m;

	MB_MT_DeepCHK = 0;
	MB_gMT_Timer_1000m 	= gMainTimerCnt_10m;
	MB_gMT_Timer_100m 	= gMainTimerCnt_10m;

	MBTop_InitControl();

	for(int i=0;i<3;i++)
	{
		MBTop_ReadyToMoveLayer[i]=	false;
		MBTop_GoToMoveLayer[i]	 =	false;
	}

	switch(gMBTop_Choice_Index)
	{
		case MenuB_Engine:
		{
			//printf("\r\nMenuBTop_Enter->MenuB_Engine-00----\r\n");
			ituWidgetSetVisible(MB1_1a_BTN,false);
			ituWidgetSetVisible(MB1_1b_BTN,false);

			ituWidgetSetVisible(MB1_1a_ANI, false);
			ituWidgetSetVisible(MB1_1b_ANI, false);

			ituWidgetSetVisible(MB1_1a_ZoomBTN, true);
			ituWidgetSetVisible(MB1_1b_ZoomBTN, true);

			ituWidgetSetVisible(MB1_2_BTN,true);
			ituWidgetSetVisible(MB1_2_ANI, false);
			ituWidgetSetVisible(MB1_2_ZoomBTN, false);

			ituWidgetSetVisible(MB1_3_BTN,true);
			ituWidgetSetVisible(MB1_3_ANI, false);
			ituWidgetSetVisible(MB1_3_ZoomBTN, false);

			
			ituWidgetSetVisible(MB1_4_BTN,true);
			ituWidgetSetVisible(MB1_4_ANI, false);
			ituWidgetSetVisible(MB1_4_ZoomBTN, false);

			MBTop_ReadyToMoveLayer[0] = true;
			gMBTop_Choice_Index = 0;
		}	
		break;
		
		case MenuB_IO:
		{
			//printf("\r\nMenuBTop_Enter->MenuB_IO---11--\r\n");
			ituWidgetSetVisible(MB1_1a_BTN,true);
			ituWidgetSetVisible(MB1_1b_BTN,true);

			ituWidgetSetVisible(MB1_1a_ANI, false);
			ituWidgetSetVisible(MB1_1b_ANI, false);

			ituWidgetSetVisible(MB1_1a_ZoomBTN, false);
			ituWidgetSetVisible(MB1_1b_ZoomBTN, false);

			ituWidgetSetVisible(MB1_2_BTN,false);
			ituWidgetSetVisible(MB1_2_ANI, false);
			ituWidgetSetVisible(MB1_2_ZoomBTN, true);

			ituWidgetSetVisible(MB1_3_BTN,true);
			ituWidgetSetVisible(MB1_3_ANI, false);
			ituWidgetSetVisible(MB1_3_ZoomBTN, false);

			ituWidgetSetVisible(MB1_4_BTN,true);
			ituWidgetSetVisible(MB1_4_ANI, false);
			ituWidgetSetVisible(MB1_4_ZoomBTN, false);

			MBTop_ReadyToMoveLayer[1] = true;
			gMBTop_Choice_Index = 1;
		}
		break;
		
		case MenuB_Option:
		{
			//printf("\r\nMenuBTop_Enter->MenuB_Option---22--\r\n");
			ituWidgetSetVisible(MB1_1a_BTN,true);
			ituWidgetSetVisible(MB1_1b_BTN,true);

			ituWidgetSetVisible(MB1_1a_ANI, false);
			ituWidgetSetVisible(MB1_1b_ANI, false);

			ituWidgetSetVisible(MB1_1a_ZoomBTN, false);
			ituWidgetSetVisible(MB1_1b_ZoomBTN, false);

			ituWidgetSetVisible(MB1_2_BTN,true);
			ituWidgetSetVisible(MB1_2_ANI, false);
			ituWidgetSetVisible(MB1_2_ZoomBTN, false);

			ituWidgetSetVisible(MB1_3_BTN,false);
			ituWidgetSetVisible(MB1_3_ANI, false);
			ituWidgetSetVisible(MB1_3_ZoomBTN, true);

			ituWidgetSetVisible(MB1_4_BTN,true);
			ituWidgetSetVisible(MB1_4_ANI, false);
			ituWidgetSetVisible(MB1_4_ZoomBTN, false);

			MBTop_ReadyToMoveLayer[2] = true;
			gMBTop_Choice_Index = 2;
		}
		break;

		case MenuB_Cal:
		{
			//printf("\r\nMenuBTop_Enter->MenuB_Option---33--\r\n");
			ituWidgetSetVisible(MB1_1a_BTN,true);
			ituWidgetSetVisible(MB1_1b_BTN,true);

			ituWidgetSetVisible(MB1_1a_ANI, false);
			ituWidgetSetVisible(MB1_1b_ANI, false);

			ituWidgetSetVisible(MB1_1a_ZoomBTN, false);
			ituWidgetSetVisible(MB1_1b_ZoomBTN, false);

			ituWidgetSetVisible(MB1_2_BTN,true);
			ituWidgetSetVisible(MB1_2_ANI, false);
			ituWidgetSetVisible(MB1_2_ZoomBTN, false);

			ituWidgetSetVisible(MB1_3_BTN,true);
			ituWidgetSetVisible(MB1_3_ANI, false);
			ituWidgetSetVisible(MB1_3_ZoomBTN, false);

			ituWidgetSetVisible(MB1_4_BTN,false);
			ituWidgetSetVisible(MB1_4_ANI, false);
			ituWidgetSetVisible(MB1_4_ZoomBTN, true);

			MBTop_ReadyToMoveLayer[3] = true;
			gMBTop_Choice_Index = 3;
		}
		break;
	}
	
	DOUBLE_KEY_ENABLE_FLAG = 0;	// JPHong 0727 #10956
	ReadyRotation();	// JPHong 2023-10-24
	ScreenSetOnOff(1);	// JPHong 2023-12-04

	//chpark.24.10.22 
	if( (g_nM7_StartType == M7_MODE_KEY) && (g_bPW100_PW_OnOff == PW100_PASS_MODE_ON) )
	{
		ituWidgetSetVisible(MBT_Lock,true);
	}
	else
	{
		ituWidgetSetVisible(MBT_Lock,false);
	}
	

    return true;
}

void MBTop_InitControl()
{
	SetWidget(MB1_1a_ANI,"MB1_1a_ANI");
	SetWidget(MB1_1b_ANI,"MB1_1b_ANI");

	SetWidget(MB1_1a_BTN,"MB1_1a_BTN");
	SetWidget(MB1_1b_BTN,"MB1_1b_BTN");

	SetWidget(MB1_1a_ZoomBTN,"MB1_1a_ZoomBTN");
	SetWidget(MB1_1b_ZoomBTN,"MB1_1b_ZoomBTN");

	SetWidget(MB1_2_ANI,"MB1_2_ANI");
	SetWidget(MB1_2_BTN,"MB1_2_BTN");
	SetWidget(MB1_2_ZoomBTN,"MB1_2_ZoomBTN");

	SetWidget(MB1_3_ANI,"MB1_3_ANI");
	SetWidget(MB1_3_BTN,"MB1_3_BTN");
	SetWidget(MB1_3_ZoomBTN,"MB1_3_ZoomBTN");

	SetWidget(MB1_4_ANI,"MB1_4_ANI");
	SetWidget(MB1_4_BTN,"MB1_4_BTN");
	SetWidget(MB1_4_ZoomBTN,"MB1_4_ZoomBTN");

	SetWidget(MB1_ResetIcon,"MB1_Reset_Success");

	SetWidget(MBT_Lock,"MBT_Lock"); //chpark.24.10.22
}

bool MenuBTop_Timer(ITUWidget* widget, char* param)
{
	#ifdef USED_ROTATION //chpark.10.19
    if (RotationMode!=0)
    {
      return 0;
    }
    #endif

	if((gMainTimerCnt_10m - MB_gMT_Timer_100m) >= 10)		// 0.1 Sec
	{
		MB_gMT_Timer_100m = gMainTimerCnt_10m;

		if(MBTop_GoToMoveLayer[gMBTop_Choice_Index])
		{
			//printf("\r\n timer MBTop_GoToMoveLayer======= %d %d\r\n",gMBTop_Choice_Index, MBTop_GoToMoveLayer[gMBTop_Choice_Index]);
			MBTop_GoToMoveLayer[gMBTop_Choice_Index] = false;
			MBTop_MoveToSelectedMenu(gMBTop_Choice_Index);
		}
	}

	if(((MB_MT_D5_Flag==1)&&(MB_MT_D7_Flag==1))||(MB_MT_DeepCHK_JOG==1))
	{
		if((gMainTimerCnt_10m - MB_gMT_DeepTimer) >= 500)
		{
			MB_MT_D5_Flag = 0;
			MB_MT_D7_Flag = 0;
			bool bMB_MT_DeepCHK_JOG = false;
			if(MB_MT_DeepCHK_JOG)
				bMB_MT_DeepCHK_JOG = true;
			MB_MT_DeepCHK_JOG = 0;


			if(!MB_MT_Reset_start_Flag)
			{
				MB_MT_Reset_start_Flag = true;
				XO_Mem_Reset();
				MB_gMT_Reset_Timer_1000m = gMainTimerCnt_10m;
				MB_MT_ResetIcon_Flag = true;
				if(bMB_MT_DeepCHK_JOG) // 2024-05-15 add whlim
				{
					MB_ReadyToJog = false;
					printf("##### MB_ReadyToJog set false #####\n");
				}
			}
		}
	}

	if(MB_MT_ResetIcon_Flag)	// add reset success icon. 2024-04-22 add whlim
	{
		if((gMainTimerCnt_10m - MB_gMT_Reset_Timer_1000m) <= 50)   // 0.5sec. time on visible reset success icon 
		{
			 if (!ituWidgetIsVisible(MB1_ResetIcon))
				ituWidgetSetVisible(MB1_ResetIcon, true);
		}
		else
		{
			ituWidgetSetVisible(MB1_ResetIcon, false);
			MB_MT_Reset_start_Flag = false;
			MB_MT_ResetIcon_Flag = false;
		}
	}

	if((gMainTimerCnt_10m - MB_gMT_Timer_1000m) >= 100)		// 1Sec
	{
		MB_gMT_Timer_1000m = gMainTimerCnt_10m;

		MB_gMT_Timer_Exit++;
		
		if(MB_gMT_Timer_Exit >= 30)
		{
			MB_gMT_Timer_Exit = 0;
			//printf("\r\n Menu Exit > goto Main\r\n");
				
            #ifdef RELEASE_VER
	            GotoMain(MAIN_MODE_NORMAL); 
            #else
	            printf("\r\n [Debug Mode]Menu Exit > goto Main\r\n");
            #endif
		}
	}
	
    return true;
}

void MBTop_MoveToSelectedMenu(int8_t nIndex)
{
	ITULayer *tBranchLayer;
	
	switch(nIndex)
	{
		case MenuB_Engine:
			tBranchLayer = ituSceneFindWidget(&theScene, "MenuB100_Layer");
			ituLayerGoto(tBranchLayer);
			break;

		case MenuB_IO:
			tBranchLayer = ituSceneFindWidget(&theScene, "MenuB200_Layer");
			ituLayerGoto(tBranchLayer);
			break;

		case MenuB_Option:
			//chpark.24.10.22
			if( (g_nM7_StartType == M7_MODE_KEY) && (g_bPW100_PW_OnOff == PW100_PASS_MODE_ON))
			{
				g_nPW100_LayerType = PW100_MODE_INPUT;
				g_nPW100_PreLayer  = PW100_BTOP_LAYER;
				g_nPW100_NextLayer = PW100_BTOP_OPT_LAYER;
				tBranchLayer = ituSceneFindWidget(&theScene, "PW100_PASS_Layer");
			}
			else
			{
				tBranchLayer = ituSceneFindWidget(&theScene, "MenuB300_Layer");
			}
			ituLayerGoto(tBranchLayer);
			break;

		case MenuB_Cal:
			tBranchLayer = ituSceneFindWidget(&theScene, "MenuB400_Layer");
			ituLayerGoto(tBranchLayer);
			break;
	}
}


int8_t MBTop_Button_GetNextIndex(uint8_t nIndex)
{
	int8_t nTmpChoice_Index=0;
	nTmpChoice_Index = nIndex;
	nTmpChoice_Index++;

	if(nTmpChoice_Index >= MenuB_UserDefine)
		nTmpChoice_Index = MenuB_Engine;
	else if(nTmpChoice_Index<MenuB_Engine)
		nTmpChoice_Index = MenuB_Cal;
    
	return nTmpChoice_Index;
}


int8_t MBTop_Button_GetPrevIndex(uint8_t nIndex)
{
	int8_t nTmpChoice_Index=0;
	nTmpChoice_Index = nIndex;
	nTmpChoice_Index--;

	if(nTmpChoice_Index >= MenuB_UserDefine)
		nTmpChoice_Index = MenuB_Engine;
	else if(nTmpChoice_Index<MenuB_Engine)
		nTmpChoice_Index = MenuB_Cal;
    
	return nTmpChoice_Index;
}


int8_t MBTop_Button_GetLeftRightIndex(uint8_t nIndex)
{
	int8_t nTmpChoice_Index=0;
	nTmpChoice_Index = nIndex;

	if(nTmpChoice_Index==MenuB_Engine)
	{
		nTmpChoice_Index = MenuB_Cal;
	}
	else if(nTmpChoice_Index==MenuB_Cal)
	{
		nTmpChoice_Index = MenuB_Engine;
	}
	else
	{
		nTmpChoice_Index = MenuB_UserDefine;
	}

	return nTmpChoice_Index;
}


void MBTop_MenuTabNext()
{
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = gMBTop_Choice_Index;
    nTmpBtnIndex = MBTop_Button_GetNextIndex(nCurBtnIndex);
    if(nTmpBtnIndex>=0 && nTmpBtnIndex<MenuB_UserDefine)
    {
		if(!g_bReadyToMouseDown){
			//printf("\n [%d button] not ready to mouse down = %d ====\n",nTmpBtnIndex,g_bReadyToMouseDown);
			return;
		}
        MBTop_Button_Select(nTmpBtnIndex);
    }
    else
    {
        
    }
}


void MBTop_MenuTabPrev()
{
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = gMBTop_Choice_Index;
    nTmpBtnIndex = MBTop_Button_GetPrevIndex(nCurBtnIndex);
    if(nTmpBtnIndex>=0 && nTmpBtnIndex<MenuB_UserDefine)
    {
		if(!g_bReadyToMouseDown){
			//printf("\n [%d button] not ready to mouse down = %d ====\n",nTmpBtnIndex,g_bReadyToMouseDown);
			return;
		}
		
        MBTop_Button_Select(nTmpBtnIndex);
    }
    else
    {
        
    }
}


void MBTop_MenuTabLeftRight()
{
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = gMBTop_Choice_Index;

	switch(nCurBtnIndex)
	{		
		case MenuB_Engine:
		{
			MBTop_Button_Select(MenuB_Cal);
		}	
		break;
		
		case MenuB_IO:
		{
			MBTop_Button_Select(MenuB_Cal);
		}	
		break;
		
		case MenuB_Option:
		{
			MBTop_Button_Select(MenuB_Cal);
		}	
		break;

		case MenuB_Cal:
		{
			MBTop_Button_Select(MenuB_Engine);
		}	
		break;

		default :
		{

		}
		break;
	}
}

void MBTop_MenuTabUp()
{
	int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = gMBTop_Choice_Index;

	switch(nCurBtnIndex)
	{		
		case MenuB_Engine:
		{
			MBTop_Button_Select(MenuB_Option);
		}	
		break;
		
		case MenuB_IO:
		{
			MBTop_Button_Select(MenuB_Engine);
		}	
		break;
		
		case MenuB_Option:
		{
			MBTop_Button_Select(MenuB_IO);
		}	
		break;

		case MenuB_Cal:
		{
			//none
		}	
		break;

		default :
		{

		}
		break;
	}
}

void MBTop_MenuTabDown()
{
	int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = gMBTop_Choice_Index;
	
	switch(nCurBtnIndex)
	{		
		case MenuB_Engine:
		{
			MBTop_Button_Select(MenuB_IO);
		}	
		break;
		
		case MenuB_IO:
		{
			MBTop_Button_Select(MenuB_Option);
		}	
		break;
		
		case MenuB_Option:
		{
			MBTop_Button_Select(MenuB_Engine);
		}	
		break;

		case MenuB_Cal:
		{
			//none
		}	
		break;

		default :
		{

		}
		break;
	}
}


bool   MBTop_Button_Select(uint8_t nIndex)
{
	bool bRet = false;
	uint8_t nNewChoiceIndex = nIndex;

	if(gMBTop_Choice_Index == nIndex)
	{
		//printf("MBTop_Button_Select: Same button:%d %d ====\n",gMBTop_Choice_Index,nNewChoiceIndex);
		g_bReadyToMouseDown = true;
		return bRet;
	}
	//==================================	
	g_bReadyToMouseDown = false;
	
	gMBTop_Choice_Index = nNewChoiceIndex;
	
	MBTop_ChoiceMenuClear();

	//=======================================================
	ituWidgetSetVisible(MB1_1a_ZoomBTN, false); //chpark.24.04.26
	ituWidgetSetVisible(MB1_1b_ZoomBTN, false); //chpark.24.04.26

	ituWidgetSetVisible(MB1_2_ZoomBTN, false); //chpark.24.04.26
	ituWidgetSetVisible(MB1_3_ZoomBTN, false); //chpark.24.04.26
	ituWidgetSetVisible(MB1_4_ZoomBTN, false); //chpark.24.04.26
	//=======================================================
	
	switch(nNewChoiceIndex)
	{		
		case MenuB_Engine:
		{
			MBTop_ReadyToMoveLayer[0] = false;

			ituWidgetSetVisible(MB1_1a_BTN,false);
			ituWidgetSetVisible(MB1_1b_BTN,false);

			ituWidgetSetVisible(MB1_1a_ANI, true);
			ituWidgetSetVisible(MB1_1b_ANI, true);

			// MT_1a,1b scale up
			MB_MT_1a_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(MB1_1a_ANI, 0);
		
			MB_MT_1b_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(MB1_1b_ANI, 0);
			//printf("ituAnimationPlay:MB1_1b_ANI\n");
		}	
		break;
		
		case MenuB_IO:
		{
			
			MBTop_ReadyToMoveLayer[1] = false;
			ituWidgetSetVisible(MB1_2_BTN,false);
			ituWidgetSetVisible(MB1_2_ANI, true);

			// MT_2 scale up
			MB_MT_2_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(MB1_2_ANI, 0);
			//printf("ituAnimationPlay:MB1_2_ANI\n");
		}
		break;
		
		case MenuB_Option:
		{
			
			MBTop_ReadyToMoveLayer[2] = false;
			ituWidgetSetVisible(MB1_3_BTN,false);
			ituWidgetSetVisible(MB1_3_ANI, true);

			// MT_3 scale up
			MB_MT_3_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(MB1_3_ANI, 0);
			//printf("ituAnimationPlay:MB1_3_ANI====2222\n");
		}
		break;

		case MenuB_Cal:
		{
			MBTop_ReadyToMoveLayer[3] = false;

			ituWidgetSetVisible(MB1_4_BTN,false);
			ituWidgetSetVisible(MB1_4_ANI, true);

			// MT_3 scale up
			MB_MT_4_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(MB1_4_ANI, 0);
			//printf("ituAnimationPlay:MB1_4_ANI==========33333\n");
		}
		break;
	}

	bRet = true;

	return bRet;
}


bool MenuBTop_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t JogDir = 0;
	MB_gMT_Timer_Exit = 0;
	JogDir = param[0];
	
	for(int i=MenuB_Engine;i<MenuB_UserDefine;i++)
	{
		if(MBTop_GoToMoveLayer[i])
		{
			//printf("MenuTop JogWheel %d \n Don't jog wheel. (layer moving)%d\r\n",JogDir,MBTop_GoToMoveLayer[i]);
			return true;
		}
	}

	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
		case LANDSCAPE_1:
		case LANDSCAPE_2:
			//printf("PORTRATE\r\n");
			if(JogDir == JOG_DIAL_ROT_RIGHT)
			{
				//printf("MBTop_MenuTabNext[JOG_DIAL_ROT_RIGHT]=======\r\n");
				MBTop_MenuTabNext();
			}
			else if(JogDir == JOG_DIAL_ROT_LEFT)
			{
				//printf("MBTop_MenuTabNext[JOG_DIAL_ROT_LEFT]=======\r\n");
				MBTop_MenuTabPrev();
			}

		break;
	}

    return true;
}


bool MenuBTop_JogButton(ITUWidget* widget, char* param)
{
	uint8_t JogButton 	= 0;
	JogButton = param[0];

	//chpark.24.09.19
	//printf("[MenuBTop_JogButton]%d\n");
	if(MB_Chk_SwitchBtn[1]==true && JogButton==JOG_DIAL_KEY_ENTER_OFF)
	{
		printf("[MenuBTop_JogButton]=JOG_DIAL_SW_HOME_OFF. Not Action\n");
		MB_Chk_SwitchBtn[1] = false; 
		if(!MB_Chk_SwitchBtn[0] && !MB_Chk_SwitchBtn[1])
		{			
			MB_ReadyToJog = true;
		}
		return false;
	}	
	//===============
	

	uint8_t tMenuIndex 	= 0;
	
	uint8_t nTmpChoice_Index = gMBTop_Choice_Index;

	if(MBTop_GoToMoveLayer[nTmpChoice_Index])
	{
		return true;
	}
	
	MB_gMT_Timer_Exit = 0;

	//printf("MenuBTop JogButton %d\r\n",JogButton);
	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
			
			switch(JogButton)
			{
				case JOG_DIAL_KEY_UP:
					MBTop_MenuTabPrev();
					break;

				case JOG_DIAL_KEY_DOWN:
					MBTop_MenuTabNext();
					break;
				
				case JOG_DIAL_KEY_LEFT:
				case JOG_DIAL_KEY_RIGHT:
					break;

				case JOG_DIAL_KEY_ENTER_OFF:
					if(MBTop_ReadyToMoveLayer[gMBTop_Choice_Index])
					{
						//printf("JOG_DIAL_KEY_ENTER_OFF: %d %d 0000====\n", gMBTop_Choice_Index, MBTop_ReadyToMoveLayer[gMBTop_Choice_Index]);
						MBTop_ReadyToMoveLayer[gMBTop_Choice_Index] = false;
						MBTop_GoToMoveLayer[gMBTop_Choice_Index] 	= true;
					}
					break;
				case JOG_DIAL_KEY_DEEP:
					//printf("JOG DEEP MENU\r\n");
					MB_MT_DeepCHK_JOG=1;	// 2024-04-11 add whlim
					MB_gMT_DeepTimer = gMainTimerCnt_10m;		// 2024-04-11 add whlim, deeptimer init
				break;
				
				case JOG_DIAL_KEY_NONE:
					//printf("JOG BUTTON OFF\r\n");
					MB_MT_DeepCHK_JOG=0;	//2024-04-11 add whlim
				break;
			}
			break;
			
		case LANDSCAPE_1:
		case LANDSCAPE_2:
			switch(JogButton)
			{
				case JOG_DIAL_KEY_UP:
					MBTop_MenuTabUp();
					break;

				case JOG_DIAL_KEY_DOWN:
					MBTop_MenuTabDown();
					break;
				
				case JOG_DIAL_KEY_LEFT:
				case JOG_DIAL_KEY_RIGHT:
					MBTop_MenuTabLeftRight();
					break;

				case JOG_DIAL_KEY_ENTER_OFF:
					if(MBTop_ReadyToMoveLayer[gMBTop_Choice_Index])
					{
						//printf("JOG_DIAL_KEY_ENTER_OFF: %d %d 0000====\n", gMBTop_Choice_Index, MBTop_ReadyToMoveLayer[gMBTop_Choice_Index]);
						MBTop_ReadyToMoveLayer[gMBTop_Choice_Index] = false;
						MBTop_GoToMoveLayer[gMBTop_Choice_Index] 	= true;
					}
					break;
				case JOG_DIAL_KEY_DEEP:
					//printf("JOG DEEP MENU\r\n");
					MB_MT_DeepCHK_JOG=1;	// 2024-04-11 add whlim
					MB_gMT_DeepTimer = gMainTimerCnt_10m;		// 2024-04-11 add whlim
				break;
				
				case JOG_DIAL_KEY_NONE:
					//printf("JOG BUTTON OFF\r\n");
					MB_MT_DeepCHK_JOG=0;	// 2024-04-11 add whlim
				break;	
			}
			break;
	}

    return true;
}

bool MenuBTop_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t JogSwitch = 0;
	JogSwitch = param[0];
	//printf("[MenuBTop_JogSwitch]=%d\n",JogSwitch);
	if(MBTop_GoToMoveLayer[gMBTop_Choice_Index])
	{
		return true;
	}

	#ifndef RELEASE_VER
		MB_Chk_SwitchBtn[0]==false;
	#endif
	
	MB_gMT_Timer_Exit = 0;
	
	if(JogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
		//chpark.24.09.19
		if(MB_Chk_SwitchBtn[0]==true)
		{
			printf("[MenuBTop_JogSwitch]=JOG_DIAL_SW_ESC_ON_OFF: Not Action. MB_Chk_SwitchBtn[0]:%d\n",MB_Chk_SwitchBtn[0]);
			MB_Chk_SwitchBtn[0] = false; 
			if(!MB_Chk_SwitchBtn[0] && !MB_Chk_SwitchBtn[1])
			{			
				MB_ReadyToJog = true;
				//printf("[MenuBTop_JogSwitch]ReadyToJog===TRUE\n");
			}
			return;
		}
		//===============	
		
		ITULayer *MenuTop_Layer = ituSceneFindWidget(&theScene, "MenuTop_Layer");
		assert(MenuTop_Layer);
		MB_MT_DeepCHK_JOG = 0;
		ituLayerGoto(MenuTop_Layer);
	}
	else if(JogSwitch == JOG_DIAL_SW_HOME_OFF)
	{
		//printf("[MenuBTop_JogSwitch]=JOG_DIAL_SW_HOME_OFF\n");
		MB_MT_DeepCHK_JOG = 0;
		GotoMain(MAIN_MODE_NORMAL); 
	}
	else if(JogSwitch == JOG_DIAL_KEY_DEEP)
	{
		//printf("[MenuBTop_JogSwitch]=JOG_DIAL_KEY_DEEP\n");
		//printf("JOG DEEP MENU\r\n");
		MB_MT_DeepCHK_JOG=1;	// 2024-04-11 whlim
		MB_gMT_DeepTimer = gMainTimerCnt_10m;	// 2024-04-11 whlim
	}

    return true;
}

bool MBT_D5_MouseUp(ITUWidget* widget, char* param)
{
	if((MB_MT_D5_Flag == 1)&&(MB_MT_D7_Flag == 0))
	{
		//gMBTop_Choice_Index = MenuB_Engine;
		ITULayer *MenuTop_Layer = ituSceneFindWidget(&theScene, "MenuTop_Layer");
		assert(MenuTop_Layer);
		ituLayerGoto(MenuTop_Layer);
	}

	MB_MT_D5_Flag = 0;
	MB_MT_D7_Flag = 0;

    return true;
}


bool MBT_D6_MouseUp(ITUWidget* widget, char* param)
{
	XO_ituButtonSetMode_Bt(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
	XO_ituButtonSetMode_TP(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
	MB_MT_D5_Flag = 0;
	MB_MT_D7_Flag = 0;
	GotoMain(MAIN_MODE_NORMAL); 
    return true;
}


bool MBT_D7_MouseUp(ITUWidget* widget, char* param)
{
	XO_ituButtonSetMode_Bt(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
	XO_ituButtonSetMode_TP(1);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28	
	if((MB_MT_D7_Flag == 1)&&(MB_MT_D5_Flag == 0))		// jhlee 231005
	{
		if(MBTop_ReadyToMoveLayer[gMBTop_Choice_Index])
		{
			//printf("MBTop_ReadyToMoveLayer[%d Menu]: %d 0000====\n", gMBTop_Choice_Index, MBTop_ReadyToMoveLayer[gMBTop_Choice_Index]);					
			MBTop_ReadyToMoveLayer[gMBTop_Choice_Index] = false;
			MBTop_GoToMoveLayer[gMBTop_Choice_Index] 	= true;
		}
	}

	MB_MT_D5_Flag = 0;
	MB_MT_D7_Flag = 0;
	
    return true;
}


bool MBT_DeepCheck(ITUWidget* widget, char* param)
{
	uint8_t tBTNChk = 0;
	
	tBTNChk = (uint8_t)atoi(param);

	if(tBTNChk == 5)
	{
		XO_ituButtonSetMode_Bt(0);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
		XO_ituButtonSetMode_TP(0);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
		MB_MT_D5_Flag = 1;
	}
	else if(tBTNChk == 7)
	{
		XO_ituButtonSetMode_Bt(0);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
		XO_ituButtonSetMode_TP(0);	// 0:Multi Finger , 1:Single Finger  // JPHong 2024-11-28
		MB_MT_D7_Flag = 1;
	}

	if ((MB_MT_D5_Flag==1)&&(MB_MT_D7_Flag==1))
	{
		printf("D5&D7 Press 5Sec Count Start\r\n");
		MB_gMT_DeepTimer = gMainTimerCnt_10m;
	}

    return true;
}


bool MB1_BTN_MouseUp(ITUWidget* widget, char* param)
{
	MB_gMT_Timer_Exit = 0;	
	uint8_t nTmpChoice_Index;
	ITULayer *tBranchLayer;
	nTmpChoice_Index = (uint8_t)atoi(param);

	if(MBTop_GoToMoveLayer[nTmpChoice_Index])
	{
		return true;
	}

	gMBTop_Choice_Index = nTmpChoice_Index;
	//printf("MB1_BTN_MouseUp:%d %d ====\n",gMBTop_Choice_Index,nTmpChoice_Index);
	//printf("MBTop_ReadyToMoveLayer[%d]: %d ====\n", nTmpChoice_Index, MBTop_ReadyToMoveLayer[nTmpChoice_Index]);
	if(MBTop_ReadyToMoveLayer[gMBTop_Choice_Index])
	{
		//printf("MBTop_ReadyToMoveLayer[%d Menu]: %d 0000====\n", gMBTop_Choice_Index, MBTop_ReadyToMoveLayer[gMBTop_Choice_Index]);			
		MBTop_ReadyToMoveLayer[gMBTop_Choice_Index] = false;
		MBTop_GoToMoveLayer[gMBTop_Choice_Index] 	= true;
	}
	else
	{
		//printf("MB1_BTN_MouseUp[%d Menu] == Don't move 1111\n", gMBTop_Choice_Index);
		MBTop_ReadyToMoveLayer[gMBTop_Choice_Index] = true;
	}
	
    return true;
}


bool MB1_BTN_MouseDown(ITUWidget* widget, char* param)
{
	MB_gMT_Timer_Exit = 0;
	bool bRet = false;
	uint8_t nNewChoiceIndex = (uint8_t)atoi(param);

	if(!g_bReadyToMouseDown)
	{
		//printf("\n not ready to mouse down = %d ====\n",g_bReadyToMouseDown);
		return false;
	}
	g_bReadyToMouseDown = false;
	
	bRet = MBTop_Button_Select(nNewChoiceIndex);
	
	if(bRet)
	{
	}
	else
	{
	}
	 
    return true;
}


bool MB1_4_ANIStopped(ITUWidget* widget, char* param)
{
	if(MB_MT_4_Scaleflag == 1)
	{
		MB_MT_4_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(MB1_4_ANI, false);
		ituWidgetSetVisible(MB1_4_ZoomBTN, true);

		if(MBTop_ReadyToMoveLayer[3])
		{
			//printf("MB1_4_ANIStopped ==> move %d\n", MBTop_ReadyToMoveLayer[3]);
			MBTop_ReadyToMoveLayer[3] = false;				
			MBTop_GoToMoveLayer[3] = true;
		}
		else
		{
			//printf("MB1_4_ANIStopped ==> Don't move %d\n", MBTop_ReadyToMoveLayer[3]);
			MBTop_ReadyToMoveLayer[3] = true;
		}
		g_bReadyToMouseDown = true; 
	}
	else if(MB_MT_4_Scaleflag == 2)
	{
		MB_MT_4_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(MB1_4_ANI, false);
		ituWidgetSetVisible(MB1_4_BTN, true);
	}
	
    return true;
}


bool MB1_3_ANIStopped(ITUWidget* widget, char* param)
{
	if(MB_MT_3_Scaleflag == 1)
	{
		MB_MT_3_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(MB1_3_ANI, false);
		ituWidgetSetVisible(MB1_3_ZoomBTN, true);

		if(MBTop_ReadyToMoveLayer[2])
		{
			//printf("MB1_3_ANIStopped ==> move %d\n", MBTop_ReadyToMoveLayer[2]);
			MBTop_ReadyToMoveLayer[2] = false;
			MBTop_GoToMoveLayer[2] = true;
		}
		else
		{
			//printf("MB1_3_ANIStopped ==> Don't move %d\n", MBTop_ReadyToMoveLayer[2]);
			MBTop_ReadyToMoveLayer[2] = true;
		}
		g_bReadyToMouseDown = true; 
	}
	else if(MB_MT_3_Scaleflag == 2)
	{
		MB_MT_3_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(MB1_3_ANI, false);
		ituWidgetSetVisible(MB1_3_BTN, true);
	}

    return true;
}

bool MB1_2_ANIStopped(ITUWidget* widget, char* param)
{
	if(MB_MT_2_Scaleflag == 1)
	{
		MB_MT_2_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(MB1_2_ANI, false);
		ituWidgetSetVisible(MB1_2_ZoomBTN, true);
		if(MBTop_ReadyToMoveLayer[1])
		{
			//printf("MB1_2_ANIStopped ==> move %d\n", MBTop_ReadyToMoveLayer[1]);
			MBTop_ReadyToMoveLayer[1] = false;				
			MBTop_GoToMoveLayer[1] = true;
		}
		else
		{
			//printf("MB1_2_ANIStopped ==> Don't move %d\n", MBTop_ReadyToMoveLayer[1]);
			MBTop_ReadyToMoveLayer[1] = true;
		}

		g_bReadyToMouseDown = true; 
	}
	else if(MB_MT_2_Scaleflag == 2)
	{
		MB_MT_2_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(MB1_2_ANI, false);
		ituWidgetSetVisible(MB1_2_BTN, true);
	}

    return true;
}

bool MB1_1b_ANIStopped(ITUWidget* widget, char* param)
{	
	if(MB_MT_1b_Scaleflag == 1)
	{
		MB_MT_1b_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(MB1_1b_ANI, false);
		ituWidgetSetVisible(MB1_1b_ZoomBTN, true);
		if(MBTop_ReadyToMoveLayer[0])
		{
			//printf("MB1_1b_ANIStopped ==> move %d\n", MBTop_ReadyToMoveLayer[0]);
			MBTop_ReadyToMoveLayer[0] = 0;
			MBTop_GoToMoveLayer[0] = true;
		}
		else
		{
			//printf("MB1_1b_ANIStopped ==> move %d\n", MBTop_ReadyToMoveLayer[0]);
			MBTop_ReadyToMoveLayer[0] = true;
		}

		g_bReadyToMouseDown = true; 
	}
	else if(MB_MT_1b_Scaleflag == 2)
	{
		MB_MT_1b_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(MB1_1b_ANI, false);
		ituWidgetSetVisible(MB1_1b_BTN, true);
	}

    return true;
}

bool MB1_1a_ANIStopped(ITUWidget* widget, char* param)
{	
	if(MB_MT_1a_Scaleflag == 1)
	{
		MB_MT_1a_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(MB1_1a_ANI, false);
		ituWidgetSetVisible(MB1_1a_ZoomBTN, true);

		g_bReadyToMouseDown = true; 
	}
	else if(MB_MT_1a_Scaleflag == 2)
	{
		MB_MT_1a_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(MB1_1a_ANI, false);
		ituWidgetSetVisible(MB1_1a_BTN, true);
	}
	
    return true;
}


void MBTop_ChoiceMenuClear(void)
{
	//printf("MBTop_ChoiceMenuClear\n");
	for(int i=0;i<4;i++)
	{
		MBTop_ReadyToMoveLayer[i]=false;
	}
	
	if(ituWidgetIsVisible(MB1_1a_ZoomBTN) == true)
	{
		ituWidgetSetVisible(MB1_1a_ZoomBTN,false);
		ituWidgetSetVisible(MB1_1b_ZoomBTN,false);

		ituWidgetSetVisible(MB1_1a_ANI, true);
		ituWidgetSetVisible(MB1_1b_ANI, true);

		// MT_1a,1b scale down
		MB_MT_1a_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(MB1_1a_ANI, 1);
		
		MB_MT_1b_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(MB1_1b_ANI, 1);
	}
	
	if(ituWidgetIsVisible(MB1_1b_ZoomBTN) == true)
	{
		ituWidgetSetVisible(MB1_1a_ZoomBTN,false);
		ituWidgetSetVisible(MB1_1b_ZoomBTN,false);

		ituWidgetSetVisible(MB1_1a_ANI, true);
		ituWidgetSetVisible(MB1_1b_ANI, true);

		// MT_1a,1b scale down
		MB_MT_1a_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(MB1_1a_ANI, 1);
		
		MB_MT_1b_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(MB1_1b_ANI, 1);
	}
	
	if(ituWidgetIsVisible(MB1_2_ZoomBTN) == true)
	{
		ituWidgetSetVisible(MB1_2_ZoomBTN,false);

		ituWidgetSetVisible(MB1_2_ANI, true);

		// MT_2 scale down
		MB_MT_2_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(MB1_2_ANI, 1); //chpark.24.04.26
	}

	if(ituWidgetIsVisible(MB1_3_ZoomBTN) == true)
	{
		ituWidgetSetVisible(MB1_3_ZoomBTN,false);

		ituWidgetSetVisible(MB1_3_ANI, true);

		// MT_3 scale down
		MB_MT_3_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(MB1_3_ANI, 1);
	}

	if(ituWidgetIsVisible(MB1_4_ZoomBTN) == true)
	{
		ituWidgetSetVisible(MB1_4_ZoomBTN,false);

		ituWidgetSetVisible(MB1_4_ANI, true);

		// MT_3 scale down
		MB_MT_4_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(MB1_4_ANI, 1);
	}
}




	

	
