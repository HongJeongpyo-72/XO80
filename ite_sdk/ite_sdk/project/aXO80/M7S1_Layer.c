#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

/* widgets:
M7_S1
S1_D7_BTN
S1_D6_BTN
S1_D5_BTN
M7S1_BG
S1_C_NA
M7S1_TopUserName
M7S1_TopStandard
M7S1_TopAdmin
S1_9_D
S1_8_D
S1_17_ZoomBTN
S1_17_ANI
S1_17_Up1
S1_17_Up0
S1_17_BTN
S1_16_ZoomBTN
S1_16_ANI
S1_16_Up1
S1_16_Up0
S1_16_BTN
S1_15_ZoomBTN
S1_15_ANI
S1_15_Up1
S1_15_Up0
S1_15_BTN
S1_14_Sel
S1_2bWay_Value
S1_2aWay_Value
S1_1Way_Value
S1_9
S1_8
S1_7
S1_6
S1_5
S1_4
S1_C_Sel
S1_C_Value
S1_10_ATT10
S1_10_ATT9
S1_10_ATT8
S1_10_ATT7
S1_10_ATT6
S1_10_ATT5
S1_10_ATT4
S1_10_ATT3
S1_10_ATT2
S1_10_ATT1
S1_10_BTN
S1_10_AttSel
S1_B_Sel
S1_B_Value
S1_A_Sel
S1_A_Value
S1_3_ZoomBTN
S1_3_ANI
S1_3_Up1
S1_3_Up0
S1_3_BTN
S1_2_ZoomBTN
S1_2_ANI
S1_2_Up1
S1_2_Up0
S1_2_BTN
S1_1_ZoomBTN
S1_1_ANI
S1_1_Up1
S1_1_Up0
S1_1_BTN
M7S1_usBG
M7S1_siBG
*/

static ITUText* S1_A_Value;
static ITUText* S1_B_Value;
static ITUText* S1_C_Value;
static ITUText *S1_C_NA;
	
static ITUText* S1_1Way_Value;
static ITUText* S1_2aWay_Value;
static ITUText* S1_2bWay_Value;


static ITUText* M7S1_TopUserName;
static ITUIcon* M7S1_TopStandard;
static ITUIcon* M7S1_TopAdmin;
static ITUButton* S1_17_ZoomBTN;
static ITUAnimation* S1_17_ANI;
static ITUIcon* S1_17_Up1;
static ITUIcon* S1_17_Up0;
static ITUButton* S1_17_BTN;
static ITUIcon* S1_16_ZoomBTN;
static ITUAnimation* S1_16_ANI;
static ITUIcon* S1_16_Up1;
static ITUIcon* S1_16_Up0;
static ITUButton* S1_16_BTN;
static ITUIcon* S1_15_ZoomBTN;
static ITUAnimation* S1_15_ANI;
static ITUIcon* S1_15_Up1;
static ITUIcon* S1_15_Up0;
static ITUButton* S1_15_BTN;

static ITUButton* S1_3_ZoomBTN;
static ITUAnimation* S1_3_ANI;
static ITUIcon* S1_3_Up1;
static ITUIcon* S1_3_Up0;
static ITUButton* S1_3_BTN;
static ITUButton* S1_2_ZoomBTN;
static ITUAnimation* S1_2_ANI;
static ITUIcon* S1_2_Up1;
static ITUIcon* S1_2_Up0;
static ITUButton* S1_2_BTN;
static ITUButton* S1_1_ZoomBTN;
static ITUAnimation* S1_1_ANI;
static ITUIcon* S1_1_Up1;
static ITUIcon* S1_1_Up0;
static ITUButton* S1_1_BTN;

static ITUButton* S1_4;
static ITUButton* S1_5;
static ITUButton* S1_6;
static ITUButton* S1_7;
static ITUButton* S1_8;
static ITUButton* S1_9;
static ITUButton* S1_8_D;
static ITUButton* S1_9_D;
static ITUButton* S1_10_BTN;

static ITUIcon* S1_A_Sel;
static ITUIcon* S1_B_Sel;
static ITUIcon* S1_10_AttSel;
static ITUIcon* S1_C_Sel;
static ITUIcon* S1_14_Sel;

static ITUIcon* S1_A_Sel;
static ITUIcon* S1_B_Sel;
static ITUIcon* S1_C_Sel;
static ITUIcon* S1_10_AttSel;
static ITUIcon* S1_14_Sel;


static uint8_t S1_1_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t S1_2_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t S1_3_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down

static uint8_t S1_15_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t S1_16_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down
static uint8_t S1_17_Scaleflag = 0; 	// 0; None ,1;Scale up  ,2;Scale Down

static uint32_t S1_SecTimer;
static uint8_t S1_ExitCNT = 0;
static uint8_t S1_1SecCNT = 0;

static uint32_t S1_BlinkTimer;
static uint8_t S1_BlinkFlag = 0;
static uint8_t S1_SkipFlag = 0;

static uint32_t S1_FastTimer;
static uint8_t S1_FastFlag = 0;
static uint8_t S1_FastSource = 0;

uint8_t M7_UserIndex = 0;		// 0 ~ 9 ; 10user , CAN[UI1] 0byte 0~3bit
uint8_t M7_UserIndexEx = 0;

uint8_t M7_Group;				// 0 ~ 2 ; 3set
UserPresetData M7_UserSet[3];	// JPHong Menu V13 2024-04-17
UserAUX24Data M7_UserAUX24Set;	//

uint8_t M7_P1_OPI_Flag = 0;					// CAN[OPI] 6byte 0~1bit; 0:OPT None , 1:OPT Ok , 2:OPT hold , 3:NULL
uint8_t M7_Way_OPI_Flag = 0;				// CAN[OPI] 0byte 2~3bit; 0:OPT None , 1:OPT Ok , 2:OPT hold , 3:NULL

uint32_t M7_Way_Timer = 0;
uint32_t M7_P1_Timer = 0;

uint8_t gGroup=0;
uint8_t tgGroup=0xFF;
UserPresetData gUserAUX1Data[3];
//UserAUX24Data gUserAUX24Data;
static uint8_t S1_SelectValue = 0;
static char S1_PARA[2];

	//dev.opt
//uint8_t S1_1WAY_X[6]={10,30,50,70,90,100};		// CAN[AUX_U/L5] 1 ~ 6byte
//uint8_t S1_1WAY_Y[6]={10,15,30,60,80,90};			// CAN[AUX_U/L4] 1 ~ 6byte

uint8_t S1_1WAY_1st_X[6]={0,0,0,0,0,0};			// CAN[AUX_UL5] 0byte 0 , 1 ~ 6byte
uint8_t S1_1WAY_1st_Y[6]={0,0,0,0,0,0};			// CAN[AUX_UL4] 0byte 1 , 1 ~ 6byte
uint8_t S1_1WAY_2nd_X[6]={0,0,0,0,0,0};			// CAN[AUX_UL5] 0byte 2 , 1 ~ 6byte
uint8_t S1_1WAY_2nd_Y[6]={0,0,0,0,0,0};			// CAN[AUX_UL4] 0byte 3 , 1 ~ 6byte

uint8_t S1_AUX1_FlowRate_Max = 250;			// CAN[AUX_U/L] 0byte
uint8_t S1_AUX1_FlowRate_Min = 0;				// CAN[AUX_U/L] 1byte
uint8_t S1_AUX1_Pressure_Max = 250;			// CAN[AUX_PU/L] 0byte
uint8_t S1_AUX1_Pressure_Min = 0;				// CAN[AUX_PU/L] 1byte

uint8_t S1_UserOption = 0;
uint8_t AUX_ConfigOptionData1 = 0xFF;
extern UserAUX24Data cUserAUX24Data;

bool M7S1_EnterFlag = 0;

void AUX1_Init(void);

void S1_GroupClear(uint8_t MenuIndex);
void S1_SelectClear(void);
void S1_WayOptClear(void);
void S1_WayOpt_Enable(void);
void S1_WayOpt_Disable(void);
void S1_WayOpt_Display(uint8_t tWay_Opt);
void S1_ATT_Clear(void);
void S1_ATT_Display(uint8_t tATT_State);
void S1_Value_Display(uint8_t tSelectValue);

void S1_1Way_FlowRateUpdate(uint8_t tA_Value);
void S1_1Way_ValueUpdate(uint8_t tA_Value);

void S1_2aWay_FlowRateUpdate(uint8_t tA_Value);
void S1_2aWay_ValueUpdate(uint8_t tA_Value);

void S1_2bWay_FlowRateUpdate(uint8_t tB_Value);
void S1_2bWay_ValueUpdate(uint8_t tB_Value);

void S1_Pressure_Update(uint8_t tMPa_Value);
void S1_P1_Value_Update(uint8_t tMPa_Value);

uint8_t Cal_1Way1st_FlowRate(uint8_t t1way_X);
uint8_t Cal_1Way2nd_FlowRate(uint8_t t1way_X);

void S1_Edit_Update(uint8_t tUpdateFlag);

bool S1_10_MouseDn(ITUWidget* widget, char* param);
bool S1_10_MouseUp(ITUWidget* widget, char* param);

bool S1_WayOpt_MouseUp(ITUWidget* widget, char* param);

bool S1_P_MouseDn(ITUWidget* widget, char* param);
bool S1_P_MouseUp(ITUWidget* widget, char* param);

bool S1D5_MouseDn(ITUWidget* widget, char* param);
bool S1D5_MouseUp(ITUWidget* widget, char* param);

bool M7S1_UserDisplay(void);
uint8_t AUX_ConfigOption(void);

void S1_UpDn_Enable(void);
void S1_UpDn_Disable(void);

extern uint16_t Change_kpa2psi(uint16_t DateVal);
extern uint16_t Change_psi2kpa(uint16_t DataVal);

extern uint16_t Change_Lm2gpm(uint16_t DateVal);
extern uint16_t Change_gpm2Lm(uint16_t DateVal);

extern void GotoMenuTop(void);
extern void GotoMenu7(void);
void GotoM7S1(uint8_t AUX_NO);
void S1_GroupMenuAniClear(void);

uint8_t M7S1_UserDisplay_Data = 0xFF;
uint8_t M7_14_SelFlag = 0;		// JPHong 2024-03-21



void GotoM7S1(uint8_t AUX_NO)	// JPHong 2023-04-18
{
	//chpark.24.10.15
	ITULayer* M7_S1 = ituSceneFindWidget(&theScene, "M7_S1");
	assert(M7_S1);
	tgGroup = AUX_NO;
	//	M7_Group = gGroup;		// JPHong 2024-04-15

	if(g_nM7_StartType == M7_MODE_PUSH)
	{
		printf("[GotoM7S1][M7_MODE_PUSH]\n"); 	
	
		ituLayerGoto(M7_S1);
	}
	else if(g_nM7_StartType == M7_MODE_KEY)
	{
		if(g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF || g_nM7_Lock_Data[0].Show == M7_LSTATE_HIDE || g_nM7_Lock_Data[0].Lock == M7_LSTATE_UNLOCK)
		{
			printf("[GotoM7S1][M7_MODE_KEY] tgGroup=%d g_nM7_Lock_Data[tgGroup].Show=%d g_nM7_Lock_Data[tgGroup].Lock=%d\n",tgGroup,g_nM7_Lock_Data[tgGroup].Show,g_nM7_Lock_Data[tgGroup].Lock);
			ituLayerGoto(M7_S1);
		}
		else if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON && g_nM7_Lock_Data[0].Lock == M7_LSTATE_LOCK)
		{
			g_nPW100_PreLayer  = PW100_M7_LAYER;

			g_nPW100_NextLayer = PW100_1st_LAYER;
		
			g_nPW100_LayerType = PW100_MODE_INPUT;

			ITULayer* tPass_Layer = ituSceneFindWidget(&theScene, "PW100_PASS_Layer");
			ituLayerGoto(tPass_Layer);
		}
	}
	else
	{
		printf("[GotoM7S1][Error]g_nM7_StartType=%d\n",g_nM7_StartType);
	}
}


bool M7S1_TouchUP_Restet(void)
{
	S1_1SecCNT = 100;
	S1_FastFlag = 0;
	S1_FastSource = 0;

	S1_SecTimer = gMainTimerCnt_10m;
}

void AUX1_Init(void)
{

}

uint8_t AUX_ConfigOption(void)
{
	uint8_t tOpt_Check = 0;
	uint8_t tAccessAUX = 0,tRegFlag = 0;

	if (gMenu_UI_Time>0)	// JPHong 2024-03-25
	{
		tRegFlag = User_RegInfo[M7_UserIndex].RegFlag;
		tAccessAUX = User_RegInfo[M7_UserIndex].AccessAUX;
	
		//tRegFlag = 9;					// dev_opt
		//tAccessAUX = 0;				// dev_opt
		
		// case 1 ; User_RegInfo[M7_UserIndex].RegFlag >> 2,3,5
		if((tRegFlag == 2)||(tRegFlag == 3)||(tRegFlag == 5))
		{
			tOpt_Check = 1;
		}
	
		// case 2 ; User_RegInfo[M7_UserIndex].RegFlag == 9 && User_RegInfo[M7_UserIndex].AccessAUX == 1;Yes (0;None define , 2;No)
		else if(tRegFlag == 9)
		{
			if(tAccessAUX == SU600_AUX_TYPE_YES)
			{
				tOpt_Check = 2;
			}
		}
		
	//	printf("AUX_ConfigOption = %d\r\n",tOpt_Check);
	//	printf("User_RegInfo[%d].RegFlag = %d\r\n",M7_UserIndex,tRegFlag);
	//	printf("User_RegInfo[%d].AccessAUX = %d\r\n",M7_UserIndex,tAccessAUX);
	}

	//chpark.24.10.23
	if(g_nM7_StartType != M7_MODE_PUSH)
	{
		tOpt_Check = 0;
	}

	return tOpt_Check;
}

bool M7S1_Enter(ITUWidget* widget, char* param)
{

	static uint8_t Initial_Flag=0;

	uint8_t i,j;
	
	printf("M7S1_Layer Enter\r\n");
	Main_Disp_Mode = MAIN_MODE_MENU;	// JPHong 2023-04-18
	M7S1_UserDisplay_Data = 0xFF;
	AUX_ConfigOptionData1 = 0xFF;
	M7_14_SelFlag = 0;		// JPHong 2024-03-21
	
	
	SetWidget(S1_A_Value,"S1_A_Value");
	SetWidget(S1_B_Value,"S1_B_Value");
	SetWidget(S1_C_Value,"S1_C_Value");
	SetWidget(S1_C_NA,"S1_C_NA");
	SetWidget(S1_1Way_Value,"S1_1Way_Value");
	SetWidget(S1_2aWay_Value,"S1_2aWay_Value");
	SetWidget(S1_2bWay_Value,"S1_2bWay_Value");


	SetWidget(M7S1_TopUserName,"M7S1_TopUserName");
	SetWidget(M7S1_TopStandard,"M7S1_TopStandard");
	SetWidget(M7S1_TopAdmin,"M7S1_TopAdmin");
	SetWidget(S1_17_ZoomBTN,"S1_17_ZoomBTN");
	SetWidget(S1_17_ANI,"S1_17_ANI");
	SetWidget(S1_17_Up1,"S1_17_Up1");
	SetWidget(S1_17_Up0,"S1_17_Up0");
	SetWidget(S1_17_BTN,"S1_17_BTN");
	SetWidget(S1_16_ZoomBTN,"S1_16_ZoomBTN");
	SetWidget(S1_16_ANI,"S1_16_ANI");
	SetWidget(S1_16_Up1,"S1_16_Up1");
	SetWidget(S1_16_Up0,"S1_16_Up0");
	SetWidget(S1_16_BTN,"S1_16_BTN");
	SetWidget(S1_15_ZoomBTN,"S1_15_ZoomBTN");
	SetWidget(S1_15_ANI,"S1_15_ANI");
	SetWidget(S1_15_Up1,"S1_15_Up1");
	SetWidget(S1_15_Up0,"S1_15_Up0");
	SetWidget(S1_15_BTN,"S1_15_BTN");


	SetWidget(S1_3_ZoomBTN,"S1_3_ZoomBTN");
	SetWidget(S1_3_ANI,"S1_3_ANI");
	SetWidget(S1_3_Up1,"S1_3_Up1");
	SetWidget(S1_3_Up0,"S1_3_Up0");
	SetWidget(S1_3_BTN,"S1_3_BTN");
	SetWidget(S1_2_ZoomBTN,"S1_2_ZoomBTN");
	SetWidget(S1_2_ANI,"S1_2_ANI");
	SetWidget(S1_2_Up1,"S1_2_Up1");
	SetWidget(S1_2_Up0,"S1_2_Up0");
	SetWidget(S1_2_BTN,"S1_2_BTN");
	SetWidget(S1_1_ZoomBTN,"S1_1_ZoomBTN");
	SetWidget(S1_1_ANI,"S1_1_ANI");
	SetWidget(S1_1_Up1,"S1_1_Up1");
	SetWidget(S1_1_Up0,"S1_1_Up0");
	SetWidget(S1_1_BTN,"S1_1_BTN");
	
	SetWidget(S1_4,"S1_4");
	SetWidget(S1_5,"S1_5");
	SetWidget(S1_6,"S1_6");
	SetWidget(S1_7,"S1_7");
	SetWidget(S1_8,"S1_8");
	SetWidget(S1_9,"S1_9");
	SetWidget(S1_8_D,"S1_8_D");
	SetWidget(S1_9_D,"S1_9_D");
	SetWidget(S1_10_BTN,"S1_10_BTN");

	SetWidget(S1_A_Sel,"S1_A_Sel");
	SetWidget(S1_B_Sel,"S1_B_Sel");
	SetWidget(S1_10_AttSel,"S1_10_AttSel");
	SetWidget(S1_C_Sel,"S1_C_Sel");
	SetWidget(S1_14_Sel,"S1_14_Sel");
	SetWidget(S1_10_BTN,"S1_10_BTN");
	
	SetWidget(S1_A_Sel,"S1_A_Sel");
	SetWidget(S1_B_Sel,"S1_B_Sel");
	SetWidget(S1_C_Sel,"S1_C_Sel");
	SetWidget(S1_10_AttSel,"S1_10_AttSel");
	SetWidget(S1_14_Sel,"S1_14_Sel");
	
	
	//M7_UserIndex = 0;			//dev.opt
	if (tgGroup!=0xFF)			// JPHong 2024-04-15
		gGroup = tgGroup;
	else
		gGroup = 0;
		
	tgGroup=0xFF;
	
	M7_Group = gGroup;
	
	
	
	for(j=0;j<3;j++)
		gUserAUX1Data[j] = M7_UserSet[j];

	cUserAUX24Data = M7_UserAUX24Set;

	AUX1_LimitCheck();		// ?��??�증??메인?�작???�용 ?�요
	for(j=0;j<3;j++)
		M7_UserSet[j] = gUserAUX1Data[j];
	
	AUX2_4_LimitCheck();	// ?��??�증??메인?�작???�용 ?�요
	M7_UserAUX24Set = cUserAUX24Data;


	ITUBackground* M7S1_siBG = ituSceneFindWidget(&theScene, "M7S1_siBG");
	assert(M7S1_siBG);
	ITUBackground* M7S1_usBG = ituSceneFindWidget(&theScene, "M7S1_usBG");
	assert(M7S1_usBG);

	if(SIUS_SEL_Flag == 0)	//si
	{
		ituWidgetSetVisible(M7S1_usBG, false);
		ituWidgetSetVisible(M7S1_siBG, true);
	}
	else if(SIUS_SEL_Flag == 1)		// us
	{
		ituWidgetSetVisible(M7S1_siBG, false);
		ituWidgetSetVisible(M7S1_usBG, true);
	}

//	S1_GroupClear();
	switch(gGroup)
	{
		case 0:
		{
			S1_GroupClear(1);
			S1_GroupClear(2);
			ituWidgetSetVisible(S1_1_BTN, false);
			ituWidgetSetVisible(S1_1_ANI, true);

			S1_1_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(S1_1_ANI,0);
		}
		break;
		
		case 1:
		{
			S1_GroupClear(0);
			S1_GroupClear(2);
			ituWidgetSetVisible(S1_2_BTN, false);
			ituWidgetSetVisible(S1_2_ANI, true);

			S1_2_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(S1_2_ANI,0);
		}
		break;
		
		case 2:
		{
			S1_GroupClear(0);
			S1_GroupClear(1);
			ituWidgetSetVisible(S1_3_BTN, false);
			ituWidgetSetVisible(S1_3_ANI, true);

			S1_3_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(S1_3_ANI,0);
		}
		break;
	}

	S1_SelectValue = 0;
	S1_SelectClear();

	//=======================
	S1_UserOption = AUX_ConfigOption();
	
	if(S1_UserOption == 0)
	{
		S1_UpDn_Disable();
	}
	else
	{
		S1_UpDn_Enable();
	}
	AUX_ConfigOptionData1 = S1_UserOption;

	//=======================
	S1_ATT_Display(gUserAUX1Data[gGroup].ATT_State);

	S1_1Way_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
	S1_2aWay_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
	S1_2bWay_FlowRateUpdate(gUserAUX1Data[gGroup].B_Value);
	S1_Pressure_Update(gUserAUX1Data[gGroup].P1_Value);

	if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
	{
		S1_WayOpt_Enable();
		S1_WayOpt_Display(gUserAUX1Data[gGroup].Way_Opt);
	}
	else if((M7_Way_OPI_Flag == 0)||(M7_Way_OPI_Flag == 3))
	{
		ituWidgetSetVisible(S1_14_Sel, false);
		S1_WayOpt_Disable();
	}

	M7S1_UserDisplay();
	
	S1_SecTimer = gMainTimerCnt_10m;
	S1_ExitCNT = 0;

	S1_BlinkFlag = 0;
	
	S1_1SecCNT = 100;
	S1_FastFlag = 0;
	S1_FastSource = 0;
	M7S1_EnterFlag = 1;		// JPHong 2024-04-15
    return true;
}

bool M7S1_Timer(ITUWidget* widget, char* param)
{
	static uint8_t B_Way_OPI_Flag = 0;

#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	
	if((gMainTimerCnt_10m - S1_SecTimer) >= 100)
	{
		if(S1_ExitCNT < 30)
			S1_ExitCNT++;
		if(S1_ExitCNT == 30)
		{
			S1_ExitCNT = 100;
			printf("\r\nM7S1 Menu 30Sec Exit > goto Main\r\n");
			GotoMain(MAIN_MODE_NORMAL); 
		}

		// long arrow btn check
		if(S1_1SecCNT < 1)
			S1_1SecCNT++;
		if(S1_1SecCNT == 1)
		{
			S1_1SecCNT = 100;
			printf("\r\nS1 Long key\r\n");

			if(S1_FastFlag == 1)
			{
				S1_FastFlag = 2;
				S1_FastTimer = gMainTimerCnt_10m;
			}
		}
			
		S1_SecTimer = gMainTimerCnt_10m;
	}

	//if((S1_BlinkFlag == 0) || (S1_BlinkFlag == 1)) 
	{
		AUX1_LimitCheck();
		AUX2_4_LimitCheck();

		S1_1Way_ValueUpdate(gUserAUX1Data[gGroup].A_Value);
		S1_2aWay_ValueUpdate(gUserAUX1Data[gGroup].A_Value);
		S1_2bWay_ValueUpdate(gUserAUX1Data[gGroup].B_Value);
		S1_P1_Value_Update(gUserAUX1Data[gGroup].P1_Value);
		
		if (AUX_ConfigOptionData1 != AUX_ConfigOption())
		{
			AUX_ConfigOptionData1 = AUX_ConfigOption();
			if(AUX_ConfigOptionData1 == 0)
			{
				S1_UpDn_Disable();
			}
			else
			{
				S1_UpDn_Enable();
			}
		}
	}


//printf("S1_BlinkFlag : [%d][%d][%d][%d]\r\n",S1_BlinkFlag,S1_SelectValue,S1_BlinkTimer,gMainTimerCnt_10m);
	if(S1_BlinkFlag == 1)		// on
	{
		if((gMainTimerCnt_10m - S1_BlinkTimer) >= 30) // 300msec
		{
			S1_Value_Display(S1_SelectValue);
			S1_BlinkFlag = 2;
			S1_BlinkTimer = gMainTimerCnt_10m;
		}
	}
	else if(S1_BlinkFlag == 2)	// off
	{
		if((gMainTimerCnt_10m - S1_BlinkTimer) >= 30) // 300msec
		{
	//		printf("S1_BlinkFlag : [%d][%d]\r\n",S1_BlinkFlag,S1_SelectValue);
			switch(S1_SelectValue)
			{
				case 1:			// A_Value
					ituWidgetSetVisible(S1_A_Value, false);
				break;
				
				case 2:			// B_Value
					ituWidgetSetVisible(S1_B_Value, false);
				break;

				case 3:			// C_Value
					ituWidgetSetVisible(S1_C_Value, false);
				break;
			}

			S1_BlinkFlag = 1;
			S1_BlinkTimer = gMainTimerCnt_10m;
		}
	}
	
	if(S1_FastFlag == 2)
	{
		// fast up/down count
		switch(S1_FastSource)
		{
			case 4:			// A_Value down count
				if((gMainTimerCnt_10m - S1_FastTimer) >= 10)
				{
					if(gUserAUX1Data[gGroup].A_Value > S1_AUX1_FlowRate_Min)
						gUserAUX1Data[gGroup].A_Value--;
					
					S1_1Way_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
					S1_2aWay_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);

					S1_FastTimer = gMainTimerCnt_10m;
				}
			break;
			
			case 5:			// A Value up count
				if((gMainTimerCnt_10m - S1_FastTimer) >= 10)
				{
					if(gUserAUX1Data[gGroup].A_Value < S1_AUX1_FlowRate_Max)
						gUserAUX1Data[gGroup].A_Value++;
					
					S1_1Way_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
					S1_2aWay_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);

					S1_FastTimer = gMainTimerCnt_10m;
				}
			break;
			
			case 6:			// B value down count
				if((gMainTimerCnt_10m - S1_FastTimer) >= 10)
				{
					if(gUserAUX1Data[gGroup].B_Value > S1_AUX1_FlowRate_Min)
						gUserAUX1Data[gGroup].B_Value--;
					
					S1_2bWay_FlowRateUpdate(gUserAUX1Data[gGroup].B_Value);

					S1_FastTimer = gMainTimerCnt_10m;
				}
			break;
			
			case 7:			// B value up count
				if((gMainTimerCnt_10m - S1_FastTimer) >= 10)
				{
					if(gUserAUX1Data[gGroup].B_Value < S1_AUX1_FlowRate_Max)
						gUserAUX1Data[gGroup].B_Value++;
					
					S1_2bWay_FlowRateUpdate(gUserAUX1Data[gGroup].B_Value);

					S1_FastTimer = gMainTimerCnt_10m;
				}
			break;
			
			case 8:			// C value down count
				if((gMainTimerCnt_10m - S1_FastTimer) >= 10)
				{
					if(gUserAUX1Data[gGroup].P1_Value == S1_AUX1_Pressure_Max)
					{
						if((S1_AUX1_Pressure_Max%2)==1)
							gUserAUX1Data[gGroup].P1_Value -= 1;
						else
							gUserAUX1Data[gGroup].P1_Value -= 2;
					}
					else if(gUserAUX1Data[gGroup].P1_Value >= (S1_AUX1_Pressure_Min+2))
						gUserAUX1Data[gGroup].P1_Value -= 2;
					else if(gUserAUX1Data[gGroup].P1_Value < (S1_AUX1_Pressure_Min+2))
						gUserAUX1Data[gGroup].P1_Value = S1_AUX1_Pressure_Min;

					S1_Pressure_Update(gUserAUX1Data[gGroup].P1_Value);

					S1_FastTimer = gMainTimerCnt_10m;
				}
			break;
			
			case 9:			// C value up count
				if((gMainTimerCnt_10m - S1_FastTimer) >= 10)
				{
					if(gUserAUX1Data[gGroup].P1_Value == S1_AUX1_Pressure_Min)
					{
						if((S1_AUX1_Pressure_Min%2) == 1)
							gUserAUX1Data[gGroup].P1_Value += 1;
						else 
							gUserAUX1Data[gGroup].P1_Value += 2;
					}
					else
					{
						gUserAUX1Data[gGroup].P1_Value += 2;
					}
					
					if(gUserAUX1Data[gGroup].P1_Value > S1_AUX1_Pressure_Max)
						gUserAUX1Data[gGroup].P1_Value = S1_AUX1_Pressure_Max;
						
					S1_Pressure_Update(gUserAUX1Data[gGroup].P1_Value);

					S1_FastTimer = gMainTimerCnt_10m;
				}
			break;
		}
	}

	if(B_Way_OPI_Flag != M7_Way_OPI_Flag)
	{
		B_Way_OPI_Flag = M7_Way_OPI_Flag;
		
		// CAN Polling -> AUX , OPI
		if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
		{
			S1_WayOpt_Enable();
			S1_WayOpt_Display(gUserAUX1Data[gGroup].Way_Opt);
		}
		else if((M7_Way_OPI_Flag == 0)||(M7_Way_OPI_Flag == 3))
		{
//			if((gMainTimerCnt_10m - M7_Way_Timer) >= 500)
			{
				ituWidgetSetVisible(S1_14_Sel, false);
				S1_WayOpt_Disable();
			}
		}
	}

	//S1_GroupMenuAniClear();

	M7S1_UserDisplay();

    return true;
}

bool M7S1_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t tJogWheel = 0;
	uint8_t tGroup = 0,tValue = 0;
	
	S1_ExitCNT = 0;	
	tJogWheel = param[0];
	
	printf("M7S1 JogWheel [%d][%d][%d][%d]\r\n",tJogWheel,S1_BlinkFlag,S1_SelectValue,AUX_ConfigOption());

	if(tJogWheel == JOG_DIAL_ROT_LEFT)
	{
		if(S1_BlinkFlag == 0)
		{
			if(S1_SelectValue == 0)
			{
				// group change 3>2>1>3
				tGroup = gGroup + 1;
					
				tGroup--;
				if(tGroup == 0)
					tGroup = 3;
				S1_GroupClear(gGroup);
				memset(S1_PARA,0,2);
				sprintf(S1_PARA,"%d",tGroup);
				S1_P_MouseDn(NULL,S1_PARA);
				//usleep(10*1000);
				usleep(1000);
				S1_P_MouseUp(NULL,S1_PARA);
				
				S1_GroupMenuAniClear();
			}
			else if((S1_SelectValue != 0)&&(AUX_ConfigOption() != 0))
			{
				// select change 5>3>4>2>1>5
				S1_SelectClear();

				if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				{
					// CCW
					// 1>5or3,2>1,3>4,4>2,5>3or4
					uint8_t tDestValue1[6] = {0,5,1,4,2,3};
					
					tValue = tDestValue1[S1_SelectValue];
					
					if(tValue == 5)
					{
						if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
							S1_SelectValue = 5;
						else if((M7_Way_OPI_Flag == 0)||(M7_Way_OPI_Flag == 3))
						{
							if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
								S1_SelectValue = 3;
							else if((M7_P1_OPI_Flag == 0)||(M7_P1_OPI_Flag == 3))
								S1_SelectValue = 4;
						}
					}
					else if(tValue == 3)
					{
						if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
							S1_SelectValue = 3;
						else if((M7_P1_OPI_Flag == 0)||(M7_P1_OPI_Flag == 3))
							S1_SelectValue = 4;
					}
					else
						S1_SelectValue = tValue;
				}
				else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				{
					// CCW
					// 1>5or4,2>1,3>2,4>3or2,5>4
					uint8_t tDestValue2[6] = {0,5,1,2,3,4};
					
					tValue = tDestValue2[S1_SelectValue];
					
					if(tValue == 5)
					{
						if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
							S1_SelectValue = 5;
						else if((M7_Way_OPI_Flag == 0)||(M7_Way_OPI_Flag == 3))
							S1_SelectValue = 4;
					}
					else if(tValue == 3)
					{
						if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
							S1_SelectValue = 3;
						else if((M7_P1_OPI_Flag == 0)||(M7_P1_OPI_Flag == 3))
							S1_SelectValue = 2;
					}
					else
						S1_SelectValue = tValue;
				}
				
				switch(S1_SelectValue)
				{
					case 1:			// AUX1 1way
						ituWidgetSetVisible(S1_A_Sel, true);
					break;
        
					case 2:			// AUX1 2way
						ituWidgetSetVisible(S1_B_Sel, true);
					break;
        
					case 3:			// AUX1 MPa
						ituWidgetSetVisible(S1_C_Sel, true);
					break;
        
					case 4:			// AUX1 ATT
						ituWidgetSetVisible(S1_10_AttSel, true);
					break;
					
					case 5:			// AUX1 WayOpt
						ituWidgetSetVisible(S1_14_Sel, true);
					break;		
				}
			}
		}
		else if((S1_BlinkFlag != 0)&&(AUX_ConfigOption() != 0))
		{
			if(S1_SelectValue == 1)
			{
				// A value down count
				if(gUserAUX1Data[gGroup].A_Value > S1_AUX1_FlowRate_Min)
					gUserAUX1Data[gGroup].A_Value--;
			
				S1_1Way_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
				S1_2aWay_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
			}
			else if(S1_SelectValue == 2)
			{
				// B value down count
				if(gUserAUX1Data[gGroup].B_Value > S1_AUX1_FlowRate_Min)
					gUserAUX1Data[gGroup].B_Value--;
			
				S1_2bWay_FlowRateUpdate(gUserAUX1Data[gGroup].B_Value);
			}
			else if(S1_SelectValue == 3)	// JPHong 2024-03-20
			{
				// C value down count
				if(gUserAUX1Data[gGroup].P1_Value == S1_AUX1_Pressure_Max)
				{
					if((S1_AUX1_Pressure_Max%2)==1)
						gUserAUX1Data[gGroup].P1_Value -= 1;
					else
						gUserAUX1Data[gGroup].P1_Value -= 2;
				}
				else if(gUserAUX1Data[gGroup].P1_Value >= (S1_AUX1_Pressure_Min+2))
					gUserAUX1Data[gGroup].P1_Value -= 2;
				else if(gUserAUX1Data[gGroup].P1_Value < (S1_AUX1_Pressure_Min+2))
					gUserAUX1Data[gGroup].P1_Value = S1_AUX1_Pressure_Min;
  
				S1_Pressure_Update(gUserAUX1Data[gGroup].P1_Value);
			}
		}
	}
	else if(tJogWheel == JOG_DIAL_ROT_RIGHT)
	{
		if(S1_BlinkFlag == 0)
		{
			if(S1_SelectValue == 0)
			{
				// group change 1>2>3>1
				tGroup = gGroup + 1;
					
				tGroup++;
				if(tGroup > 3)
					tGroup = 1;
				S1_GroupClear(gGroup);
				memset(S1_PARA,0,2);
				sprintf(S1_PARA,"%d",tGroup);
				S1_P_MouseDn(NULL,S1_PARA);
				//usleep(10*1000);
				usleep(1000);
				S1_P_MouseUp(NULL,S1_PARA);
				
				S1_GroupMenuAniClear();
			}
			else if((S1_SelectValue != 0)&&(AUX_ConfigOption() != 0))
			{
				// select change 1>2>4>3>5>1
				S1_SelectClear();

				if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				{
					// CW
					// 1>2,2>4,3>5or1,4>3or5or1,5>1
					uint8_t tDestValue3[6] = {0,2,4,5,3,1};
					
					tValue = tDestValue3[S1_SelectValue];
					
					if(tValue == 5)
					{
						if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
							S1_SelectValue = 5;
						else if((M7_Way_OPI_Flag == 0)||(M7_Way_OPI_Flag == 3))
							S1_SelectValue = 1;
					}
					else if(tValue == 3)
					{
						if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
							S1_SelectValue = 3;
						else if((M7_P1_OPI_Flag == 0)||(M7_P1_OPI_Flag == 3))
						{
							if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
								S1_SelectValue = 5;
							else if((M7_Way_OPI_Flag == 0)||(M7_Way_OPI_Flag == 3))
								S1_SelectValue = 1;
						}
					}
					else
						S1_SelectValue = tValue;
				}
				else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				{
					// CW
					// 1>2,2>3or4,3>4,4>5or1,5>1
					uint8_t tDestValue4[6] = {0,2,3,4,5,1};
					
					tValue = tDestValue4[S1_SelectValue];
					
					if(tValue == 5)
					{
						if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
							S1_SelectValue = 5;
						else if((M7_Way_OPI_Flag == 0)||(M7_Way_OPI_Flag == 3))
							S1_SelectValue = 1;
					}
					else if(tValue == 3)
					{
						if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
							S1_SelectValue = 3;
						else if((M7_P1_OPI_Flag == 0)||(M7_P1_OPI_Flag == 3))
							S1_SelectValue = 4;
					}
					else
						S1_SelectValue = tValue;
				}				

				switch(S1_SelectValue)
				{
					case 1:			// AUX1 1way
						ituWidgetSetVisible(S1_A_Sel, true);
					break;
        
					case 2:			// AUX1 2way
						ituWidgetSetVisible(S1_B_Sel, true);
					break;
        
					case 3:			// AUX1 MPa
						ituWidgetSetVisible(S1_C_Sel, true);
					break;
        
					case 4:			// AUX1 ATT
						ituWidgetSetVisible(S1_10_AttSel, true);
					break;
					
					case 5:			// AUX1 WayOpt
						ituWidgetSetVisible(S1_14_Sel, true);
					break;		
				}
			}
		}
		else if((S1_BlinkFlag != 0)&&(AUX_ConfigOption() != 0))
		{
			if(S1_SelectValue == 1)
			{
				// A value up count
				if(gUserAUX1Data[gGroup].A_Value < S1_AUX1_FlowRate_Max)
					gUserAUX1Data[gGroup].A_Value++;
			
				S1_1Way_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
				S1_2aWay_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
			}
			else if(S1_SelectValue == 2)
			{
				// B value up count
				if(gUserAUX1Data[gGroup].B_Value < S1_AUX1_FlowRate_Max)
					gUserAUX1Data[gGroup].B_Value++;
			
				S1_2bWay_FlowRateUpdate(gUserAUX1Data[gGroup].B_Value);
			}
			else if(S1_SelectValue == 3)	// JPHong 2024-03-20
			{
				// C value up count
				if(gUserAUX1Data[gGroup].P1_Value == S1_AUX1_Pressure_Min)
				{
					if((S1_AUX1_Pressure_Min%2) == 1)
						gUserAUX1Data[gGroup].P1_Value += 1;
					else 
						gUserAUX1Data[gGroup].P1_Value += 2;
				}
				else
				{
					gUserAUX1Data[gGroup].P1_Value += 2;
				}
				
				if(gUserAUX1Data[gGroup].P1_Value > S1_AUX1_Pressure_Max)
					gUserAUX1Data[gGroup].P1_Value = S1_AUX1_Pressure_Max;
					
				S1_Pressure_Update(gUserAUX1Data[gGroup].P1_Value);
			}
		}
	}
 
    return true;
}

bool M7S1_JogButton(ITUWidget* widget, char* param)
{
	uint8_t tJogButton = 0;
	uint8_t tGroup = 0,tWayOpt = 0,tValue = 0;
	
	
	S1_ExitCNT = 0;	
	tJogButton = param[0];
	
	printf("M7S1 AUX Preset JogButton [%d][%d]\r\n",tJogButton,S1_BlinkFlag);

	switch(tJogButton)
	{
		case JOG_DIAL_KEY_UP:
		{
			M7_14_SelFlag  = 0;
			if(S1_BlinkFlag == 0)
			{
				if(S1_SelectValue == 0)
				{
					// no blink > no select, group change 1>2>3>1
					tGroup = gGroup + 1;
					
					tGroup--;
					if(tGroup == 0)
						tGroup = 3;
					S1_GroupClear(gGroup);
					memset(S1_PARA,0,2);
					sprintf(S1_PARA,"%d",tGroup);
					S1_P_MouseDn(NULL,S1_PARA);
					usleep(10*1000);
					S1_P_MouseUp(NULL,S1_PARA);
				}
				else if((S1_SelectValue != 0)&&(AUX_ConfigOption() != 0))
				{
					if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
					{
						// up
						// 1>5or3or1, 2>5or3or1, 3>1, 4>5or3or1, 5>3or1
						uint8_t tDestIdx1[6]={0,5,5,1,5,3};
						
						tValue = tDestIdx1[S1_SelectValue];

						S1_SelectClear();

						if(tValue == 5)
						{
							if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
								S1_SelectValue = 5;
							else if((M7_Way_OPI_Flag == 0)||(M7_Way_OPI_Flag == 3))
							{
								if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
									S1_SelectValue = 3;
								else if((M7_P1_OPI_Flag == 0)||(M7_P1_OPI_Flag == 3))
									S1_SelectValue = 1;
							}	
						}
						else if(tValue == 3)
						{
							if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
								S1_SelectValue = 3;
							else if((M7_P1_OPI_Flag == 0)||(M7_P1_OPI_Flag == 3))
								S1_SelectValue = 1;
						}
						else
							S1_SelectValue = 1;
						
						if(S1_SelectValue == 1)
							ituWidgetSetVisible(S1_A_Sel, true);
						else if(S1_SelectValue == 3)
							ituWidgetSetVisible(S1_C_Sel, true);
						else if(S1_SelectValue == 5)
							ituWidgetSetVisible(S1_14_Sel, true);
					}
					else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
					{
						// up
						// 1>5or1, 2>5or1, 3>5or1, 4>5or1, 5>1
						uint8_t tDestIdx2[6]={0,5,5,5,5,1};
						
						tValue = tDestIdx2[S1_SelectValue];

						S1_SelectClear();

						if(tValue == 5)
						{
							if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
								S1_SelectValue = 5;
							else if((M7_Way_OPI_Flag == 0)||(M7_Way_OPI_Flag == 3))
								S1_SelectValue = 1;
						}
						else
							S1_SelectValue = 1;

						if(S1_SelectValue == 1)
							ituWidgetSetVisible(S1_A_Sel, true);
						else if(S1_SelectValue == 5)
							ituWidgetSetVisible(S1_14_Sel, true);
					}
				}
			}
			else if((S1_BlinkFlag != 0)&&(AUX_ConfigOption() != 0))
			{
				// blink > select 1,2,3 > fast up count start
				if(S1_SelectValue == 1)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 5;		// A value up count
				}
				else if(S1_SelectValue == 2)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 7;		// B value up count
				}
				else if(S1_SelectValue == 3)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 9;		// C value up count
				}
			}
		}
		break;

		case JOG_DIAL_KEY_UP_OFF:
		{
			M7_14_SelFlag  = 0;
			if((S1_BlinkFlag != 0)&&(AUX_ConfigOption() != 0))
			{
				// blink > select 1,2,3,> edit value up count , fast up count stop
				if(S1_SelectValue == 1)
				{
					if(gUserAUX1Data[gGroup].A_Value < S1_AUX1_FlowRate_Max)
						gUserAUX1Data[gGroup].A_Value++;
			
					S1_1Way_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
					S1_2aWay_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
					
					S1_1SecCNT = 100;
					S1_FastFlag = 0;
					S1_FastSource = 0;

					S1_SecTimer = gMainTimerCnt_10m;	
				}
				else if(S1_SelectValue == 2)
				{
					if(gUserAUX1Data[gGroup].B_Value < S1_AUX1_FlowRate_Max)
						gUserAUX1Data[gGroup].B_Value++;
			
					S1_2bWay_FlowRateUpdate(gUserAUX1Data[gGroup].B_Value);

					S1_1SecCNT = 100;
					S1_FastFlag = 0;
					S1_FastSource = 0;

					S1_SecTimer = gMainTimerCnt_10m;	
				}
				else if(S1_SelectValue == 3)
				{
					if(gUserAUX1Data[gGroup].P1_Value == S1_AUX1_Pressure_Min)
					{
						if((S1_AUX1_Pressure_Min%2) == 1)
							gUserAUX1Data[gGroup].P1_Value += 1;
						else 
							gUserAUX1Data[gGroup].P1_Value += 2;
					}
					else
					{
						gUserAUX1Data[gGroup].P1_Value += 2;
					}
					
					if(gUserAUX1Data[gGroup].P1_Value > S1_AUX1_Pressure_Max)
						gUserAUX1Data[gGroup].P1_Value = S1_AUX1_Pressure_Max;
						
					S1_Pressure_Update(gUserAUX1Data[gGroup].P1_Value);

					S1_1SecCNT = 100;
					S1_FastFlag = 0;
					S1_FastSource = 0;

					S1_SecTimer = gMainTimerCnt_10m;	
				}
			}
		}
		break;
		
		case JOG_DIAL_KEY_DOWN:
		{
			M7_14_SelFlag  = 0;
			// no blink > no select, group change 3>2>1>3
			if(S1_BlinkFlag == 0)
			{
				if(S1_SelectValue == 0)
				{
					tGroup = gGroup + 1;
					
					tGroup++;
					if(tGroup > 3)
						tGroup = 1;
					S1_GroupClear(gGroup);
					memset(S1_PARA,0,2);
					sprintf(S1_PARA,"%d",tGroup);
					S1_P_MouseDn(NULL,S1_PARA);
					usleep(10*1000);
					S1_P_MouseUp(NULL,S1_PARA);
				}
				else if((S1_SelectValue != 0)&&(AUX_ConfigOption() != 0))
				{
					if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
					{
						// down
						// 1>3or5or1, 2>3or5or1, 3>5or1, 4>3or5or1, 5>1
						uint8_t tDestIdx3[6]={0,3,3,5,3,1};
						
						tValue = tDestIdx3[S1_SelectValue];

						S1_SelectClear();
						
						if(tValue == 3)
						{
							if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
								S1_SelectValue = 3;
							else if((M7_P1_OPI_Flag == 0)||(M7_P1_OPI_Flag == 3))
							{
								if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
									S1_SelectValue = 5;
								else if((M7_Way_OPI_Flag == 0)||(M7_Way_OPI_Flag == 3))
									S1_SelectValue = 1;
							}
						}
						else if(tValue == 5)
						{
							if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
								S1_SelectValue = 5;
							else if((M7_Way_OPI_Flag == 0)||(M7_Way_OPI_Flag == 3))
								S1_SelectValue = 1;
						}
						else
							S1_SelectValue = 1;

						if(S1_SelectValue == 1)
							ituWidgetSetVisible(S1_A_Sel, true);
						else if(S1_SelectValue == 3)
							ituWidgetSetVisible(S1_C_Sel, true);
						else if(S1_SelectValue == 5)
							ituWidgetSetVisible(S1_14_Sel, true);
					}
					else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
					{
						// down
						// 1>5or1, 2>5or1, 3>5or1, 4>5or1, 5>1
						uint8_t tDestIdx4[6]={0,5,5,5,5,1};
						
						tValue = tDestIdx4[S1_SelectValue];

						S1_SelectClear();
						
						if(tValue == 5)
						{
							if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
								S1_SelectValue = 5;
							else if((M7_Way_OPI_Flag == 0)||(M7_Way_OPI_Flag == 3))
								S1_SelectValue = 1;
						}
						else if(tValue == 1)
							S1_SelectValue = 1;

						if(S1_SelectValue == 1)
							ituWidgetSetVisible(S1_A_Sel, true);
						else if(S1_SelectValue == 5)
							ituWidgetSetVisible(S1_14_Sel, true);
					}
				}
			}
			else if((S1_BlinkFlag != 0)&&(AUX_ConfigOption() != 0))
			{
				// blink > select 1,2,3 > fast down count start
				if(S1_SelectValue == 1)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 4;		// A value down count
				}
				else if(S1_SelectValue == 2)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 6;		// B value down count
				}
				else if(S1_SelectValue == 3)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 8;		// C value down count
				}
			}
		}	
		break;

		case JOG_DIAL_KEY_DOWN_OFF:
		{
			M7_14_SelFlag  = 0;
			if((S1_BlinkFlag != 0)&&(AUX_ConfigOption() != 0))
			{
				// blink > select 1,2,3,> edit value down count , fast down count stop
				if(S1_SelectValue == 1)
				{
					if(gUserAUX1Data[gGroup].A_Value > S1_AUX1_FlowRate_Min)
						gUserAUX1Data[gGroup].A_Value--;
			
					S1_1Way_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
					S1_2aWay_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
					
					S1_1SecCNT = 100;
					S1_FastFlag = 0;
					S1_FastSource = 0;

					S1_SecTimer = gMainTimerCnt_10m;	
				}
				else if(S1_SelectValue == 2)
				{
					if(gUserAUX1Data[gGroup].B_Value > S1_AUX1_FlowRate_Min)
						gUserAUX1Data[gGroup].B_Value--;
			
					S1_2bWay_FlowRateUpdate(gUserAUX1Data[gGroup].B_Value);

					S1_1SecCNT = 100;
					S1_FastFlag = 0;
					S1_FastSource = 0;

					S1_SecTimer = gMainTimerCnt_10m;	
				}
				else if(S1_SelectValue == 3)
				{
					if(gUserAUX1Data[gGroup].P1_Value == S1_AUX1_Pressure_Max)
					{
						if((S1_AUX1_Pressure_Max%2)==1)
							gUserAUX1Data[gGroup].P1_Value -= 1;
						else
							gUserAUX1Data[gGroup].P1_Value -= 2;
					}
					else if(gUserAUX1Data[gGroup].P1_Value >= (S1_AUX1_Pressure_Min+2))
						gUserAUX1Data[gGroup].P1_Value -= 2;
					else if(gUserAUX1Data[gGroup].P1_Value < (S1_AUX1_Pressure_Min+2))
						gUserAUX1Data[gGroup].P1_Value = S1_AUX1_Pressure_Min;
    
					S1_Pressure_Update(gUserAUX1Data[gGroup].P1_Value);

					S1_1SecCNT = 100;
					S1_FastFlag = 0;
					S1_FastSource = 0;

					S1_SecTimer = gMainTimerCnt_10m;	
				}
			}
		}
		break;
		
		case JOG_DIAL_KEY_LEFT:
		{
			M7_14_SelFlag  = 0;
			// no blink > no select, group change 3>2>1>3
			if(S1_BlinkFlag == 0)
			{
				if(S1_SelectValue == 0)
				{
					tGroup = gGroup + 1;
					
					tGroup--;
					if(tGroup == 0)
						tGroup = 3;
					S1_GroupClear(gGroup);
					memset(S1_PARA,0,2);
					sprintf(S1_PARA,"%d",tGroup);
					S1_P_MouseDn(NULL,S1_PARA);
					usleep(10*1000);
					S1_P_MouseUp(NULL,S1_PARA);
				}
				else if((S1_SelectValue != 0)&&(AUX_ConfigOption() != 0))
				{
					if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
					{
						// left
						// 1>4, 2>1, 3>4, 4>2, 5>none
						uint8_t tDestIdx5[6]={0,4,1,4,2,0};
						
						tValue = tDestIdx5[S1_SelectValue];

						if(tValue > 0)
						{
							S1_SelectClear();

							if(tValue == 4)
							{
								S1_SelectValue = 4;
								ituWidgetSetVisible(S1_10_AttSel, true);
							}
							else if(tValue == 1)
							{
								S1_SelectValue = 1;
								ituWidgetSetVisible(S1_A_Sel, true);
							}
							else if(tValue == 2)
							{
								S1_SelectValue = 2;
								ituWidgetSetVisible(S1_B_Sel, true);
							}
						}
					}
					else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
					{
						// left
						// 1>4, 2>1, 3>2, 4>3or2, 5>4
						uint8_t tDestIdx6[6]={0,4,1,2,3,4};
						
						tValue = tDestIdx6[S1_SelectValue];

						S1_SelectClear();
											
						if(tValue == 3)
						{
							if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
								S1_SelectValue = 3;
							else if((M7_P1_OPI_Flag == 0)||(M7_P1_OPI_Flag == 3))
								S1_SelectValue = 2;
						}
						else if(tValue > 0)
							S1_SelectValue = tValue;
						
						if(S1_SelectValue == 1)
							ituWidgetSetVisible(S1_A_Sel, true);
						else if(S1_SelectValue == 2)
							ituWidgetSetVisible(S1_B_Sel, true);
						else if(S1_SelectValue == 3)
							ituWidgetSetVisible(S1_C_Sel, true);
						else if(S1_SelectValue == 4)
							ituWidgetSetVisible(S1_10_AttSel, true);
					}					
				}
			}
			else if((S1_BlinkFlag != 0)&&(AUX_ConfigOption() != 0))
			{
				// blink > select 1,2,3 > fast down count start
				if(S1_SelectValue == 1)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 4;		// A value down count
				}
				else if(S1_SelectValue == 2)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 6;		// B value down count
				}
				else if(S1_SelectValue == 3)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 8;		// C value down count
				}
			}
		}
		break;

		case JOG_DIAL_KEY_LEFT_OFF:
		{
			M7_14_SelFlag  = 0;
			if((S1_BlinkFlag != 0)&&(AUX_ConfigOption() != 0))
			{
				// blink > select 1,2,3,> edit value down count , fast down count stop
				if(S1_SelectValue == 1)
				{
					if(gUserAUX1Data[gGroup].A_Value > S1_AUX1_FlowRate_Min)
						gUserAUX1Data[gGroup].A_Value--;
			
					S1_1Way_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
					S1_2aWay_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
					
					S1_1SecCNT = 100;
					S1_FastFlag = 0;
					S1_FastSource = 0;

					S1_SecTimer = gMainTimerCnt_10m;	
				}
				else if(S1_SelectValue == 2)
				{
					if(gUserAUX1Data[gGroup].B_Value > S1_AUX1_FlowRate_Min)
						gUserAUX1Data[gGroup].B_Value--;
			
					S1_2bWay_FlowRateUpdate(gUserAUX1Data[gGroup].B_Value);

					S1_1SecCNT = 100;
					S1_FastFlag = 0;
					S1_FastSource = 0;

					S1_SecTimer = gMainTimerCnt_10m;	
				}
				else if(S1_SelectValue == 3)
				{
					if(gUserAUX1Data[gGroup].P1_Value == S1_AUX1_Pressure_Max)
					{
						if((S1_AUX1_Pressure_Max%2)==1)
							gUserAUX1Data[gGroup].P1_Value -= 1;
						else
							gUserAUX1Data[gGroup].P1_Value -= 2;
					}
					else if(gUserAUX1Data[gGroup].P1_Value >= (S1_AUX1_Pressure_Min+2))
						gUserAUX1Data[gGroup].P1_Value -= 2;
					else if(gUserAUX1Data[gGroup].P1_Value < (S1_AUX1_Pressure_Min+2))
						gUserAUX1Data[gGroup].P1_Value = S1_AUX1_Pressure_Min;
    
					S1_Pressure_Update(gUserAUX1Data[gGroup].P1_Value);

					S1_1SecCNT = 100;
					S1_FastFlag = 0;
					S1_FastSource = 0;

					S1_SecTimer = gMainTimerCnt_10m;	
				}
			}
		}
		break;
		
		case JOG_DIAL_KEY_RIGHT:
		{
			M7_14_SelFlag  = 0;
			// no blink > no select, group change 1>2>3>1
			if(S1_BlinkFlag == 0)
			{
				if(S1_SelectValue == 0)
				{
					tGroup = gGroup + 1;
					
					tGroup++;
					if(tGroup > 3)
						tGroup = 1;
					S1_GroupClear(gGroup);
					memset(S1_PARA,0,2);
					sprintf(S1_PARA,"%d",tGroup);
					S1_P_MouseDn(NULL,S1_PARA);
					usleep(10*1000);
					S1_P_MouseUp(NULL,S1_PARA);
				}
				else if((S1_SelectValue != 0)&&(AUX_ConfigOption() != 0))
				{
					if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
					{
						// right
						// 1>2, 2>4, 3>2, 4>1, 5>none
						uint8_t tDestIdx7[6]={0,2,4,2,1,0};
						
						tValue = tDestIdx7[S1_SelectValue];

						if(tValue > 0)
						{
							S1_SelectClear();

							if(tValue == 1)
							{
								S1_SelectValue = 1;
								ituWidgetSetVisible(S1_A_Sel, true);
							}
							else if(tValue == 2)
							{
								S1_SelectValue = 2;
								ituWidgetSetVisible(S1_B_Sel, true);
							}
							else if(tValue == 4)
							{
								S1_SelectValue = 4;
								ituWidgetSetVisible(S1_10_AttSel, true);
							}
						}
					}				
					else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
					{
						// right
						// 1>2, 2>3or4, 3>4, 4>1, 5>2
						uint8_t tDestIdx7[6]={0,2,3,4,1,2};
						
						tValue = tDestIdx7[S1_SelectValue];

						S1_SelectClear();
						
						if(tValue == 3)
						{
							if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
								S1_SelectValue = 3;
							else if((M7_P1_OPI_Flag == 0)||(M7_P1_OPI_Flag == 3))
								S1_SelectValue = 4;
						}
						else if((tValue != 3) && (tValue > 0))
							S1_SelectValue = tValue;

						if(S1_SelectValue == 1)
							ituWidgetSetVisible(S1_A_Sel, true);
						else if(S1_SelectValue == 2)
							ituWidgetSetVisible(S1_B_Sel, true);
						else if(S1_SelectValue == 3)
							ituWidgetSetVisible(S1_C_Sel, true);
						else if(S1_SelectValue == 4)
							ituWidgetSetVisible(S1_10_AttSel, true);
					}
				}
			}
			else if((S1_BlinkFlag != 0)&&(AUX_ConfigOption() != 0))
			{
				// blink > select 1,2,3 > fast up count start
				if(S1_SelectValue == 1)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 5;		// A value up count
				}
				else if(S1_SelectValue == 2)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 7;		// B value up count
				}
				else if(S1_SelectValue == 3)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 9;		// C value up count
				}
			}
		}	
		break;

		case JOG_DIAL_KEY_RIGHT_OFF:
		{
			M7_14_SelFlag  = 0;
			if((S1_BlinkFlag != 0)&&(AUX_ConfigOption() != 0))
			{
				// blink > select 1,2,3,> edit value up count , fast up count stop
				if(S1_SelectValue == 1)
				{
					if(gUserAUX1Data[gGroup].A_Value < S1_AUX1_FlowRate_Max)
						gUserAUX1Data[gGroup].A_Value++;
			
					S1_1Way_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
					S1_2aWay_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
					
					S1_1SecCNT = 100;
					S1_FastFlag = 0;
					S1_FastSource = 0;

					S1_SecTimer = gMainTimerCnt_10m;	
				}
				else if(S1_SelectValue == 2)
				{
					if(gUserAUX1Data[gGroup].B_Value < S1_AUX1_FlowRate_Max)
						gUserAUX1Data[gGroup].B_Value++;
			
					S1_2bWay_FlowRateUpdate(gUserAUX1Data[gGroup].B_Value);

					S1_1SecCNT = 100;
					S1_FastFlag = 0;
					S1_FastSource = 0;

					S1_SecTimer = gMainTimerCnt_10m;	
				}
				else if(S1_SelectValue == 3)
				{
					if(gUserAUX1Data[gGroup].P1_Value == S1_AUX1_Pressure_Min)
					{
						if((S1_AUX1_Pressure_Min%2) == 1)
							gUserAUX1Data[gGroup].P1_Value += 1;
						else 
							gUserAUX1Data[gGroup].P1_Value += 2;
					}
					else
					{
						gUserAUX1Data[gGroup].P1_Value += 2;
					}
					
					if(gUserAUX1Data[gGroup].P1_Value > S1_AUX1_Pressure_Max)
						gUserAUX1Data[gGroup].P1_Value = S1_AUX1_Pressure_Max;
						
					S1_Pressure_Update(gUserAUX1Data[gGroup].P1_Value);

					S1_1SecCNT = 100;
					S1_FastFlag = 0;
					S1_FastSource = 0;

					S1_SecTimer = gMainTimerCnt_10m;	
				}
			}
		}
		break;
		
		case JOG_DIAL_KEY_ENTER_ON:	//70
		{	
			printf("JOG_DIAL_KEY_ENTER_ON [%d][%d]\r\n",S1_BlinkFlag,S1_SelectValue);
			M7_14_SelFlag  = 0;
			if(S1_BlinkFlag == 0)
			{
				if(S1_SelectValue == 0)
				{
					// no blink > select 0 > select 1 , edit A value start
					if(AUX_ConfigOption() != 0)		// enable state
					{
						S1_SelectClear();
						S1_SelectValue = 1;			// A_Value
						ituWidgetSetVisible(S1_A_Sel, true);
					}
					else
					{
						if(ituWidgetIsVisible(S1_14_Sel) == false)
							M7_14_SelFlag  = 1;
						S1_SelectClear();
						if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
						{
							S1_SelectValue = 5;
							ituWidgetSetVisible(S1_14_Sel, true);
						}
					}
				}
				else if(S1_SelectValue == 4)
				{
					// no blink > select 4 > ATT set layer
					S1_10_MouseDn(NULL,NULL);
					usleep(10*1000);
					S1_10_MouseUp(NULL,NULL);
				}
				else if((S1_SelectValue == 1)||(S1_SelectValue == 2)||(S1_SelectValue == 3))
				{
					// no blink > select 1,2,3 > blink
					S1_BlinkTimer = 0;//gMainTimerCnt_10m;
					S1_BlinkFlag = 1;
				}
				else if(S1_SelectValue == 5)
				{
					// no blink > select 5 > wayopt change
					S1_1SecCNT = 100;
					S1_FastFlag = 0;
					S1_FastSource = 0;
      
					S1_SecTimer = gMainTimerCnt_10m;	
				}
			}
			else if(S1_BlinkFlag != 0)
			{
				printf("JOG_DIAL_KEY_ENTER_ON [%d][%d]\r\n",S1_SelectValue,S1_BlinkFlag);
				if((S1_SelectValue == 1)||(S1_SelectValue == 2)||(S1_SelectValue == 3))
				{
					// blink > select 1,2,3 > no blink,
					S1_BlinkFlag = 0;
					S1_SkipFlag = 2;
				}
			}
		}
		break;
		
		case JOG_DIAL_KEY_ENTER_OFF:	//86
		{
			// no blink > select 5 > wayopt change
			printf("JOG_DIAL_KEY_ENTER_OFF [%d][%d]\r\n",S1_BlinkFlag,S1_SelectValue);
			if(S1_BlinkFlag == 0)
			{
				if((S1_SelectValue == 5)&&(M7_14_SelFlag  == 0))
				{
/*
					tWayOpt = gUserAUX1Data[gGroup].Way_Opt + 1;
					
					tWayOpt++;
					if(tWayOpt > 3)
						tWayOpt = 1;
*/
//					tWayOpt = gUserAUX1Data[gGroup].Way_Opt;

					if(gUserAUX1Data[gGroup].Way_Opt == 0)		// 2Way -> 1Way
						tWayOpt = 1;			
					else if(gUserAUX1Data[gGroup].Way_Opt == 1)		// 1Way -> 1Way Auto
						tWayOpt = 2;
					else if(gUserAUX1Data[gGroup].Way_Opt == 2)		// 1Way Auto -> 2Way
						tWayOpt = 3;

					memset(S1_PARA,0,2);
					sprintf(S1_PARA,"%d",tWayOpt);
				
					S1_WayOpt_MouseUp(NULL, S1_PARA);
				}
				M7_14_SelFlag  = 0;
			}
			// blink > select 1,2,3 > no blink > edit value system update
			if(S1_SkipFlag == 2)
			{
				S1_SkipFlag = 0;

				ituWidgetSetVisible(S1_A_Value, true);
				ituWidgetSetVisible(S1_B_Value, true);
				ituWidgetSetVisible(S1_C_Value, true);

				//edit value system update
				S1_Edit_Update(1);
				
#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
				XO_Save_Config(M7_UserIndex,true,0);
#else
	HT_Save_Config(1);
#endif
			}
		}
		break;
	}
	
    return true;
}

bool M7S1_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t tJogSwitch;
	
	S1_ExitCNT = 0;	

	tJogSwitch = param[0];
	printf("M7S1_JogSwitch JogSwitch %d\r\n",tJogSwitch);

	if(tJogSwitch == JOG_DIAL_SW_ESC_ON)
	{
		M7S1_TouchUP_Restet(); // 2024-05-20 add whlim
		S1D5_MouseDn(NULL,NULL);
	}
	else if(tJogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
		S1D5_MouseUp(NULL,NULL);
	}
	else if(tJogSwitch == JOG_DIAL_SW_HOME)
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}
    return true;
}

bool S1D7_MouseDn(ITUWidget* widget, char* param)
{
	S1_ExitCNT = 0;	

	// no blink > no select, current group A_value select
	//          > select1,2,3 > select value blink
	//          > select 4 > branch ATT layer
	//          > select 5 > WayOpt change 1way>1way auto>2way>1way
	
	printf("S1D7_MouseDn:[%d][%d]\r\n",S1_BlinkFlag,S1_SelectValue);
	
	
	if(S1_BlinkFlag == 0)
	{
		switch(S1_SelectValue)
		{
			case 0:
			{
				if(AUX_ConfigOption() != 0)		// enable state
				{
					S1_SelectClear();
					S1_SelectValue = 1;			// A_Value
					ituWidgetSetVisible(S1_A_Sel, true);
				}
				else if(AUX_ConfigOption() == 0) // disable state
				{
					S1_SelectClear();
					if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
					{
						S1_SelectValue = 5;
						ituWidgetSetVisible(S1_14_Sel, true);
					}
				}
			}
			break;
  
			case 1:			// A_Value update
			case 2:			// B_Value update
			case 3:			// C_Value update
				
				S1_BlinkTimer = 0;//gMainTimerCnt_10m;
				S1_BlinkFlag = 1;
			
			break;
			
			case 4:			// goto ATT Layer
				S1_10_MouseDn(NULL,NULL);
				usleep(10*1000);
				S1_10_MouseUp(NULL,NULL);
  
			break;
			
			case 5:			// WayOpt change
			{
				gUserAUX1Data[gGroup].Way_Opt++;
				if(gUserAUX1Data[gGroup].Way_Opt > 2)
					gUserAUX1Data[gGroup].Way_Opt = 0;
  
				S1_Edit_Update(1);
				S1_WayOpt_Display(gUserAUX1Data[gGroup].Way_Opt);
			}
		}
	}
	else if(S1_BlinkFlag != 0)
	{
		printf("S1D7_MouseDn [%d]\r\n",S1_BlinkFlag);
		// no blink
		S1_BlinkFlag = 0;
		
		S1_SkipFlag = 2;
	}
    return true;
}

bool S1D7_MouseUp(ITUWidget* widget, char* param)
{
	S1_ExitCNT = 0;
	M7_14_SelFlag  = 0;
	printf("S1D7_MouseUp:[%d][%d][%d]\r\n",S1_BlinkFlag,S1_SelectValue,S1_SkipFlag);
	if(S1_SkipFlag == 2)
	{
		S1_SkipFlag = 0;

		//edit value system update
		ituWidgetSetVisible(S1_A_Value, true);
		ituWidgetSetVisible(S1_B_Value, true);
		ituWidgetSetVisible(S1_C_Value, true);
		
		S1_Edit_Update(1);
		
#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
		XO_Save_Config(M7_UserIndex,true,0);
#else
		HT_Save_Config(1);
#endif
	}

    return true;
}

bool S1D6_MouseUp(ITUWidget* widget, char* param)
{
/*
static uint8_t t_step = 0;

	switch(t_step)
	{
		case 0:
		//User_RegInfo[M7_UserIndex].RegFlag = 5;					// dev_opt
		//User_RegInfo[M7_UserIndex].AccessAUX = 0;				// dev_opt

		break;
		
		case 1:
		break;
		
		case 2:
		break;
	}
*/
	
	GotoMain(MAIN_MODE_NORMAL); 
	
    return true;
}

bool S1D5_MouseDn(ITUWidget* widget, char* param)
{
	S1_ExitCNT = 0;	
	printf("S1D5_MouseDn:[%d][%d][%d]\r\n",S1_BlinkFlag,S1_SelectValue,S1_SkipFlag);
	// blink > edit value resume > skip goto menu7
	if(S1_BlinkFlag != 0)
	{
		S1_BlinkFlag = 0;				// blink stop
		
		S1_Edit_Update(0);			// edit value resume
		S1_SkipFlag = 1;
	}
	else if(S1_BlinkFlag == 0)
	{
		if(S1_SelectValue != 0)
		{
			S1_SelectValue = 0;
			S1_SelectClear();
			
			S1_SkipFlag = 1;
		}
		else
			S1_SkipFlag = 0;
	}

    return true;
}

bool S1D5_MouseUp(ITUWidget* widget, char* param)
{
	printf("S1D5_MouseUp:[%d][%d][%d]\r\n",S1_BlinkFlag,S1_SelectValue,S1_SkipFlag);
	S1_ExitCNT = 0;	

	// skip check
	if(S1_SkipFlag == 1)
	{
		S1_SkipFlag = 0;
		
		S1_1Way_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
		S1_2aWay_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
		S1_2bWay_FlowRateUpdate(gUserAUX1Data[gGroup].B_Value);
		S1_Pressure_Update(gUserAUX1Data[gGroup].P1_Value);

		S1_ATT_Display(gUserAUX1Data[gGroup].ATT_State);

		//S1_WayOpt_Display(gUserAUX1Data[gGroup].Way_Opt);
		
		if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
		{
			S1_WayOpt_Enable();
			S1_WayOpt_Display(gUserAUX1Data[gGroup].Way_Opt);
		}
		else if((M7_Way_OPI_Flag == 0)||(M7_Way_OPI_Flag == 3))
		{
			ituWidgetSetVisible(S1_14_Sel, false);
			S1_WayOpt_Disable();
		}

	}
	else if(S1_SkipFlag == 0)
	{
		M7S1_EnterFlag=0;
		GotoMenu7();
	}

    return true;
}

bool M7S1_UserDisplay(void)
{
	uint8_t tDestName[100];

	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
		return 0;

	if (M7S1_UserDisplay_Data!=User_RegInfo[M7_UserIndex].RegFlag)
	{
		if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
		{
			ituWidgetSetVisible(M7S1_TopStandard,false);
			ituWidgetSetVisible(M7S1_TopAdmin,true);
		}
		else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
		{
			ituWidgetSetVisible(M7S1_TopAdmin,false);
			ituWidgetSetVisible(M7S1_TopStandard,true);
		}
		else
		{
			ituWidgetSetVisible(M7S1_TopAdmin,false);
			ituWidgetSetVisible(M7S1_TopStandard,false);
		}
		M7S1_UserDisplay_Data=User_RegInfo[M7_UserIndex].RegFlag;
	}
	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M7S1_TopUserName, tDestName);

		ituWidgetSetVisible(M7S1_TopUserName,true);
	}
	else
	{
		ituWidgetSetVisible(M7S1_TopUserName,false);
	}
}

bool S1_WayZoom_Dn(ITUWidget* widget, char* param)
{

	S1_ExitCNT = 0;	
	printf("S1_WayZoom_Dn:[%d][%d][%d]\r\n",S1_BlinkFlag,S1_SelectValue,S1_SkipFlag);
	if(S1_BlinkFlag != 0)		// blink > blink stop > value update
	{
		S1_BlinkFlag = 0;				
	
		S1_1SecCNT = 100;
		S1_FastFlag = 0;
		S1_FastSource = 0;

		S1_SecTimer = gMainTimerCnt_10m;	

		S1_Edit_Update(1);

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
		XO_Save_Config(M7_UserIndex,true,0);
#else
		HT_Save_Config(1);
#endif

		ituWidgetSetVisible(S1_A_Value, true);
		ituWidgetSetVisible(S1_B_Value, true);
		ituWidgetSetVisible(S1_C_Value, true);
	}

	S1_SelectClear();
	S1_SelectValue = 5;
	ituWidgetSetVisible(S1_14_Sel, true);

    return true;
}

bool S1_WayOpt_MouseDn(ITUWidget* widget, char* param)
{
	printf("S1_WayOpt_MouseDn:[%d][%d][%d]\r\n",S1_BlinkFlag,S1_SelectValue,S1_SkipFlag);
	S1_ExitCNT = 0;	

	if(S1_BlinkFlag != 0)		// blink > blink stop > value update
	{
		S1_BlinkFlag = 0;				
	
		S1_1SecCNT = 100;
		S1_FastFlag = 0;
		S1_FastSource = 0;

		S1_SecTimer = gMainTimerCnt_10m;	

		S1_Edit_Update(1);

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
		XO_Save_Config(M7_UserIndex,true,0);
#else
		HT_Save_Config(1);
#endif

		ituWidgetSetVisible(S1_A_Value, true);
		ituWidgetSetVisible(S1_B_Value, true);
		ituWidgetSetVisible(S1_C_Value, true);
	}

	S1_SelectClear();
	S1_SelectValue = 5;
	ituWidgetSetVisible(S1_14_Sel, true);

	uint8_t tWayOpt = 0;			// 2024-05-29 add whlim
	tWayOpt = (uint8_t)atoi(param);	// 1~3
	switch(tWayOpt)
	{
		case 1:			// 1way
			S1_WayOpt_Display(1);
		break;
		
		case 2:			// 1way auto
			S1_WayOpt_Display(2);
		break;
		
		case 3:			// 2way
			S1_WayOpt_Display(0);
		break;
	}

    return true;
}

bool S1_WayOpt_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tWayOpt = 0;
	
	S1_ExitCNT = 0;
	M7_14_SelFlag  = 0;
	printf("S1_WayOpt_MouseUp:[%d][%d][%d]\r\n",S1_BlinkFlag,S1_SelectValue,S1_SkipFlag);
	tWayOpt = (uint8_t)atoi(param);	// 1~3
	switch(tWayOpt)
	{
		case 1:			// 1way
		
			gUserAUX1Data[gGroup].Way_Opt = 1;
			S1_Edit_Update(1);
			S1_WayOpt_Display(1);

		break;
		
		case 2:			// 1way auto

			gUserAUX1Data[gGroup].Way_Opt = 2;
			S1_Edit_Update(1);
			S1_WayOpt_Display(2);

		break;
		
		case 3:			// 2way

			gUserAUX1Data[gGroup].Way_Opt = 0;
			S1_Edit_Update(1);
			S1_WayOpt_Display(0);

		break;
	}

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
		XO_Save_Config(M7_UserIndex,true,0);
#else
		HT_Save_Config(1);
#endif

    return true;
}

bool S1_17_ANIStopped(ITUWidget* widget, char* param)
{
	if(S1_17_Scaleflag == 1)
	{
		S1_17_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(S1_17_ANI, false);
		ituWidgetSetVisible(S1_17_ZoomBTN, true);
	}
	else if(S1_17_Scaleflag == 2)
	{
		S1_17_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(S1_17_ANI, false);
		ituWidgetSetVisible(S1_17_BTN, true);
	}

    return true;
}

bool S1_16_ANIStopped(ITUWidget* widget, char* param)
{
	if(S1_16_Scaleflag == 1)
	{
		S1_16_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(S1_16_ANI, false);
		ituWidgetSetVisible(S1_16_ZoomBTN, true);
	}
	else if(S1_16_Scaleflag == 2)
	{
		S1_16_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(S1_16_ANI, false);
		ituWidgetSetVisible(S1_16_BTN, true);
	}

    return true;
}

bool S1_15_ANIStopped(ITUWidget* widget, char* param)
{
	if(S1_15_Scaleflag == 1)
	{
		S1_15_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(S1_15_ANI, false);
		ituWidgetSetVisible(S1_15_ZoomBTN, true);
	}
	else if(S1_15_Scaleflag == 2)
	{
		S1_15_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(S1_15_ANI, false);
		ituWidgetSetVisible(S1_15_BTN, true);
	}

    return true;
}

bool S1_UP_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tUp_BTN;

	S1_ExitCNT = 0;	
	tUp_BTN = (uint8_t)atoi(param);

	switch(tUp_BTN)
	{
		case 5:
		{
			if(S1_SelectValue != 1)
			{
				S1_SelectClear();
				S1_Value_Display(S1_SelectValue);

				switch(S1_SelectValue)
				{
					case 1:
						M7_UserSet[gGroup].A_Value = gUserAUX1Data[gGroup].A_Value;
					break;
					
					case 2:
						M7_UserSet[gGroup].B_Value = gUserAUX1Data[gGroup].B_Value;
					break;
					
					case 3:
						M7_UserSet[gGroup].P1_Value = gUserAUX1Data[gGroup].P1_Value;
					break;
				}

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
				XO_Save_Config(M7_UserIndex,true,0);
#else
				HT_Save_Config(1);
#endif
				S1_BlinkFlag = 0;
				S1_SelectValue = 1;			// A_Value
				ituWidgetSetVisible(S1_A_Sel, true);
			}
			else if(S1_SelectValue == 1)
			{
				if(S1_BlinkFlag != 0)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 5;
				}
			}
		}
		break;
		
		case 7:
		{
			if(S1_SelectValue != 2)
			{
				S1_SelectClear();
				S1_Value_Display(S1_SelectValue);

				switch(S1_SelectValue)
				{
					case 1:
						M7_UserSet[gGroup].A_Value = gUserAUX1Data[gGroup].A_Value;
					break;
					
					case 2:
						M7_UserSet[gGroup].B_Value = gUserAUX1Data[gGroup].B_Value;
					break;
					
					case 3:
						M7_UserSet[gGroup].P1_Value = gUserAUX1Data[gGroup].P1_Value;
					break;
				}

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
				XO_Save_Config(M7_UserIndex,true,0);
#else
				HT_Save_Config(1);
#endif
				S1_BlinkFlag = 0;
				S1_SelectValue = 2;			// B_Value
				ituWidgetSetVisible(S1_B_Sel, true);
			}
			else if(S1_SelectValue == 2)
			{
				if(S1_BlinkFlag != 0)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 7;
				}
			}
		}
		break;
		
		case 9:
		{
			if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
			{
				if(S1_SelectValue != 3)
				{
					S1_SelectClear();
					S1_Value_Display(S1_SelectValue);

					switch(S1_SelectValue)
					{
						case 1:
							M7_UserSet[gGroup].A_Value = gUserAUX1Data[gGroup].A_Value;
						break;
					
						case 2:
							M7_UserSet[gGroup].B_Value = gUserAUX1Data[gGroup].B_Value;
						break;
					
						case 3:
							M7_UserSet[gGroup].P1_Value = gUserAUX1Data[gGroup].P1_Value;
						break;
					}

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
				XO_Save_Config(M7_UserIndex,true,0);
#else
				HT_Save_Config(1);
#endif
					S1_BlinkFlag = 0;
					S1_SelectValue = 3;			// C_Value
					ituWidgetSetVisible(S1_C_Sel, true);
				}
				else if(S1_SelectValue == 3)
				{
					if(S1_BlinkFlag != 0)
					{
						S1_SecTimer = gMainTimerCnt_10m;
						S1_1SecCNT = 0;
						S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
    
						S1_FastSource = 9;
					}
				}
			}
		}
		break;
	}

    return true;
}

bool S1_UP_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tUp_BTN;

	char ptFlowValue[5];
	M7_14_SelFlag  = 0;
	S1_ExitCNT = 0;	
	tUp_BTN = (uint8_t)atoi(param);

	switch(tUp_BTN)
	{
		case 5:
		{
			if((S1_BlinkFlag == 0) && (S1_SelectValue == 1))
			{
				S1_BlinkTimer = 0;//gMainTimerCnt_10m;
				S1_BlinkFlag = 1;
			}
			else if((S1_BlinkFlag != 0) && (S1_SelectValue == 1))
			{
				printf("AUX1 A_Value UpCount[%d] %d > ",S1_AUX1_FlowRate_Max,gUserAUX1Data[gGroup].A_Value);
				
				// blink > A Value Up count
				if(gUserAUX1Data[gGroup].A_Value < S1_AUX1_FlowRate_Max)
					gUserAUX1Data[gGroup].A_Value++;

				printf("%d\r\n",gUserAUX1Data[gGroup].A_Value);
					
				S1_1Way_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
				S1_2aWay_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
				
				// long key check stop
				S1_1SecCNT = 100;
				S1_FastFlag = 0;
				S1_FastSource = 0;

				S1_SecTimer = gMainTimerCnt_10m;	
			}
		}
		break;
		
		case 7:
		{
			if((S1_BlinkFlag == 0) && (S1_SelectValue == 2))
			{
				S1_BlinkTimer = 0;//gMainTimerCnt_10m;
				S1_BlinkFlag = 1;
			}
			else if((S1_BlinkFlag != 0) && (S1_SelectValue == 2))
			{
				printf("AUX1 B_Value UpCount[%d] %d > ",S1_AUX1_FlowRate_Max,gUserAUX1Data[gGroup].B_Value);

				// blink > B Value Up count
				if(gUserAUX1Data[gGroup].B_Value < S1_AUX1_FlowRate_Max)
					gUserAUX1Data[gGroup].B_Value++;

				printf("%d\r\n",gUserAUX1Data[gGroup].B_Value);
					
				S1_2bWay_FlowRateUpdate(gUserAUX1Data[gGroup].B_Value);
				
				// long key check stop
				S1_1SecCNT = 100;
				S1_FastFlag = 0;
				S1_FastSource = 0;

				S1_SecTimer = gMainTimerCnt_10m;	
			}
		}
		break;
		
		case 9:
		{
			if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
			{
				if((S1_BlinkFlag == 0) && (S1_SelectValue == 3))
				{
					S1_BlinkTimer = 0;//gMainTimerCnt_10m;
					S1_BlinkFlag = 1;
				}
				else if((S1_BlinkFlag != 0) && (S1_SelectValue == 3))
				{
					// blink > P1 Value Up count
					if(gUserAUX1Data[gGroup].P1_Value == S1_AUX1_Pressure_Min)
					{
						if((S1_AUX1_Pressure_Min%2) == 1)
							gUserAUX1Data[gGroup].P1_Value += 1;
						else 
							gUserAUX1Data[gGroup].P1_Value += 2;
					}
					else
					{
						gUserAUX1Data[gGroup].P1_Value += 2;
					}
					
					if(gUserAUX1Data[gGroup].P1_Value > S1_AUX1_Pressure_Max)
						gUserAUX1Data[gGroup].P1_Value = S1_AUX1_Pressure_Max;
						
					//S1_Pressure_Update(gUserAUX1Data[gGroup].P1_Value);
					
					memset(ptFlowValue,0,5);
					sprintf(ptFlowValue,"%4d",gUserAUX1Data[gGroup].P1_Value);
					ituTextSetString(S1_C_Value, ptFlowValue);
					ituWidgetSetVisible(S1_C_Value, true);
					
					// long key check stop
					S1_1SecCNT = 100;
					S1_FastFlag = 0;
					S1_FastSource = 0;
  
					S1_SecTimer = gMainTimerCnt_10m;	
				}
			}
		}
		break;
	}

    return true;
}

bool S1_DN_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tDown_BTN;
	
	S1_ExitCNT = 0;	
	tDown_BTN = (uint8_t)atoi(param);
	printf("S1_DN_MouseDn = [%d][%d]\n",tDown_BTN,S1_SelectValue);
	switch(tDown_BTN)
	{
		case 4:
		{
			if(S1_SelectValue != 1)
			{
				S1_SelectClear();
				S1_Value_Display(S1_SelectValue);

				switch(S1_SelectValue)
				{
					case 1:
						M7_UserSet[gGroup].A_Value = gUserAUX1Data[gGroup].A_Value;
					break;
					
					case 2:
						M7_UserSet[gGroup].B_Value = gUserAUX1Data[gGroup].B_Value;
					break;
					
					case 3:
						M7_UserSet[gGroup].P1_Value = gUserAUX1Data[gGroup].P1_Value;
					break;
				}

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
				XO_Save_Config(M7_UserIndex,true,0);
#else
				HT_Save_Config(1);
#endif
				S1_BlinkFlag = 0;
				S1_SelectValue = 1;			// A_Value
				ituWidgetSetVisible(S1_A_Sel, true);
			}
			else if(S1_SelectValue == 1)
			{
				if(S1_BlinkFlag != 0)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 4;
				}
			}
		}
		break;
		
		case 6:
		{
			if(S1_SelectValue != 2)
			{
				S1_SelectClear();
				S1_Value_Display(S1_SelectValue);

				switch(S1_SelectValue)
				{
					case 1:
						M7_UserSet[gGroup].A_Value = gUserAUX1Data[gGroup].A_Value;
					break;
					
					case 2:
						M7_UserSet[gGroup].B_Value = gUserAUX1Data[gGroup].B_Value;
					break;
					
					case 3:
						M7_UserSet[gGroup].P1_Value = gUserAUX1Data[gGroup].P1_Value;
					break;
				}

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
				XO_Save_Config(M7_UserIndex,true,0);
#else
				HT_Save_Config(1);
#endif
				S1_BlinkFlag = 0;
				S1_SelectValue = 2;			// B_Value
				ituWidgetSetVisible(S1_B_Sel, true);
			}
			else if(S1_SelectValue == 2)
			{
				if(S1_BlinkFlag != 0)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 6;
				}
			}
		}
		break;
		
		case 8:
		{
			if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
			{
				if(S1_SelectValue != 3)
				{
					S1_SelectClear();
					S1_Value_Display(S1_SelectValue);

					switch(S1_SelectValue)
					{
						case 1:
							M7_UserSet[gGroup].A_Value = gUserAUX1Data[gGroup].A_Value;
						break;
					
						case 2:
							M7_UserSet[gGroup].B_Value = gUserAUX1Data[gGroup].B_Value;
						break;
					
						case 3:
							M7_UserSet[gGroup].P1_Value = gUserAUX1Data[gGroup].P1_Value;
						break;
					}

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
				XO_Save_Config(M7_UserIndex,true,0);
#else
				HT_Save_Config(1);
#endif
					S1_BlinkFlag = 0;
					S1_SelectValue = 3;			// C_Value
					ituWidgetSetVisible(S1_C_Sel, true);
				}
				else if(S1_SelectValue == 3)
				{
					if(S1_BlinkFlag != 0)
					{
						S1_SecTimer = gMainTimerCnt_10m;
						S1_1SecCNT = 0;
						S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
        
						S1_FastSource = 8;
					}
				}
			}
		}
		break;
	}

    return true;
}

bool S1_DN_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tDown_BTN;
	char ptFlowValue[5];
	M7_14_SelFlag  = 0;
	S1_ExitCNT = 0;	
	tDown_BTN = (uint8_t)atoi(param);
	printf("S1_DN_MouseUp = [%d][%d][%d]\n",tDown_BTN,S1_SelectValue,S1_BlinkFlag);
	switch(tDown_BTN)
	{
		case 4:
		{
			if((S1_BlinkFlag == 0) && (S1_SelectValue == 1))
			{
				S1_BlinkTimer = 0;//gMainTimerCnt_10m;
				S1_BlinkFlag = 1;
			}
			else if((S1_BlinkFlag != 0) && (S1_SelectValue == 1))
			{
				printf("AUX1 A_Value DownCount[%d] %d > ",S1_AUX1_FlowRate_Min,gUserAUX1Data[gGroup].A_Value);

				// blink > A Value down count
				if(gUserAUX1Data[gGroup].A_Value > S1_AUX1_FlowRate_Min)
					gUserAUX1Data[gGroup].A_Value--;

				printf("%d\r\n",gUserAUX1Data[gGroup].A_Value);
					
				S1_1Way_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
				S1_2aWay_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
				
				// long key check stop
				S1_1SecCNT = 100;
				S1_FastFlag = 0;
				S1_FastSource = 0;

				S1_SecTimer = gMainTimerCnt_10m;	
			}
		}
		break;
		
		case 6:
		{
			if((S1_BlinkFlag == 0) && (S1_SelectValue == 2))
			{
				S1_BlinkTimer = 0;//gMainTimerCnt_10m;
				S1_BlinkFlag = 1;
			}
			else if((S1_BlinkFlag != 0) && (S1_SelectValue == 2))
			{
				printf("AUX1 B_Value DownCount[%d] %d > ",S1_AUX1_FlowRate_Min,gUserAUX1Data[gGroup].B_Value);

				// blink > B Value down count
				if(gUserAUX1Data[gGroup].B_Value > S1_AUX1_FlowRate_Min)
					gUserAUX1Data[gGroup].B_Value--;

				printf("%d\r\n",gUserAUX1Data[gGroup].A_Value);
					
				S1_2bWay_FlowRateUpdate(gUserAUX1Data[gGroup].B_Value);
				
				// long key check stop
				S1_1SecCNT = 100;
				S1_FastFlag = 0;
				S1_FastSource = 0;

				S1_SecTimer = gMainTimerCnt_10m;	
			}
		}
		break;
		
		case 8:
		{
			if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
			{
				if((S1_BlinkFlag == 0) && (S1_SelectValue == 3))
				{
					S1_BlinkTimer = 0;//gMainTimerCnt_10m;
					S1_BlinkFlag = 1;
				}
				else if((S1_BlinkFlag != 0) && (S1_SelectValue == 3))
				{
					printf("P1_Value[MinLimit %d] %d -> ",S1_AUX1_Pressure_Min,gUserAUX1Data[gGroup].P1_Value);
					
					// blink > P1 Value down count
					if(gUserAUX1Data[gGroup].P1_Value == S1_AUX1_Pressure_Max)
					{
						if((S1_AUX1_Pressure_Max%2)==1)
							gUserAUX1Data[gGroup].P1_Value -= 1;
						else
							gUserAUX1Data[gGroup].P1_Value -= 2;
					}
					else if(gUserAUX1Data[gGroup].P1_Value >= (S1_AUX1_Pressure_Min+2))
						gUserAUX1Data[gGroup].P1_Value -= 2;
					else if(gUserAUX1Data[gGroup].P1_Value < (S1_AUX1_Pressure_Min+2))
						gUserAUX1Data[gGroup].P1_Value = S1_AUX1_Pressure_Min;

					printf("%d\r\n",gUserAUX1Data[gGroup].P1_Value);
    
					//S1_Pressure_Update(gUserAUX1Data[gGroup].P1_Value);

					memset(ptFlowValue,0,5);
					sprintf(ptFlowValue,"%4d",gUserAUX1Data[gGroup].P1_Value);
					ituTextSetString(S1_C_Value, ptFlowValue);
					ituWidgetSetVisible(S1_C_Value, true);
					
					// long key check stop
					S1_1SecCNT = 100;
					S1_FastFlag = 0;
					S1_FastSource = 0;
    
					S1_SecTimer = gMainTimerCnt_10m;	
				}
			}
		}
		break;
	}

    return true;
}

bool S1_Zoom_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tPresetGroup = 0;
	
	S1_ExitCNT = 0;	

	tPresetGroup = (uint8_t)atoi(param);
	
printf("S1_Zoom_MouseDn= [%d][%d][%d]\n",tPresetGroup,S1_BlinkFlag,gGroup);

	if((S1_BlinkFlag != 0)||((tPresetGroup-1)==gGroup))	// blink > blink stop > value update
	{
		S1_SelectValue = 0;	//
		S1_BlinkFlag = 0;				
		S1_1SecCNT = 100;
		S1_FastFlag = 0;
		S1_FastSource = 0;

		S1_SecTimer = gMainTimerCnt_10m;	

		S1_Edit_Update(1);

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
		XO_Save_Config(M7_UserIndex,true,0);
#else
		HT_Save_Config(1);
#endif

		ituWidgetSetVisible(S1_A_Value, true);
		ituWidgetSetVisible(S1_B_Value, true);
		ituWidgetSetVisible(S1_C_Value, true);
	}

	S1_SelectClear();

    return true;
}

bool S1_10_MouseDn(ITUWidget* widget, char* param)
{

	S1_ExitCNT = 0;	

	S1_Edit_Update(1);

	ituWidgetSetVisible(S1_A_Value, true);
	ituWidgetSetVisible(S1_B_Value, true);
	ituWidgetSetVisible(S1_C_Value, true);

	// att select
	S1_SelectClear();
	S1_SelectValue = 4;
	ituWidgetSetVisible(S1_10_AttSel, true);

    return true;
}

bool S1_10_MouseUp(ITUWidget* widget, char* param)
{
	S1_ExitCNT = 0;	
	M7_14_SelFlag  = 0;
	if(S1_BlinkFlag != 0)		// blink > blink stop > value update
	{
		S1_BlinkFlag = 0;				
		S1_1SecCNT = 100;
		S1_FastFlag = 0;
		S1_FastSource = 0;

		S1_SecTimer = gMainTimerCnt_10m;	

		S1_Edit_Update(1);

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
	XO_Save_Config(M7_UserIndex,true,0);
#else
	HT_Save_Config(1);
#endif

		ituWidgetSetVisible(S1_A_Value, true);
		ituWidgetSetVisible(S1_B_Value, true);
		ituWidgetSetVisible(S1_C_Value, true);
	}

	ITULayer *tBranchLayer = ituSceneFindWidget(&theScene, "S1_ATT_Layer");
	assert(tBranchLayer);
	ituLayerGoto(tBranchLayer);

    return true;
}

bool S1_P_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tPresetNum = 0;

	S1_ExitCNT = 0;	
	tPresetNum = (uint8_t)atoi(param);
	
	printf("User %d AUX1 Preset Group %d select[%d]\r\n",M7_UserIndex,gGroup,(tPresetNum - 1));
	
	if (gGroup != (tPresetNum - 1))
		S1_GroupClear(gGroup);
		
	gGroup = tPresetNum - 1;
	M7_Group = gGroup;

		
	if(S1_BlinkFlag != 0)		// blink > blink stop > value update
	{
		S1_BlinkFlag = 0;				
		S1_1SecCNT = 100;
		S1_FastFlag = 0;
		S1_FastSource = 0;

		S1_SecTimer = gMainTimerCnt_10m;	

		S1_Edit_Update(1);

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
	XO_Save_Config(M7_UserIndex,true,0);
#else
	HT_Save_Config(1);
#endif
	}
	
//	S1_GroupClear();
	switch(gGroup)
	{
		case 0:
		{
			ituWidgetSetVisible(S1_1_BTN, false);
			ituWidgetSetVisible(S1_1_ANI, true);

			S1_1_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(S1_1_ANI,0);
		}
		break;
		
		case 1:
		{
			ituWidgetSetVisible(S1_2_BTN, false);
			ituWidgetSetVisible(S1_2_ANI, true);

			S1_2_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(S1_2_ANI,0);
		}
		break;
		
		case 2:
		{
			ituWidgetSetVisible(S1_3_BTN, false);
			ituWidgetSetVisible(S1_3_ANI, true);

			S1_3_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
			ituAnimationPlay(S1_3_ANI,0);
		}
		break;
	}

	S1_SelectClear();
	
    return true;
}

bool S1_P_MouseUp(ITUWidget* widget, char* param)
{

	uint8_t tPresetNum = 0,j=0;
	M7_14_SelFlag  = 0;
	S1_ExitCNT = 0;	
	tPresetNum = (uint8_t)atoi(param);

	gGroup = tPresetNum - 1;

	S1_SelectValue = 0;
	S1_BlinkFlag = 0;
	S1_1SecCNT = 100;
	S1_FastFlag = 0;
	S1_FastSource = 0;

	S1_SecTimer = gMainTimerCnt_10m;	

	for(j=0;j<3;j++)
	{
		M7_UserSet[j] = gUserAUX1Data[j];
	}

	S1_ATT_Display(gUserAUX1Data[gGroup].ATT_State);

	S1_1Way_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
	S1_2aWay_FlowRateUpdate(gUserAUX1Data[gGroup].A_Value);
	S1_2bWay_FlowRateUpdate(gUserAUX1Data[gGroup].B_Value);
	S1_Pressure_Update(gUserAUX1Data[gGroup].P1_Value);

	if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
	{
		S1_WayOpt_Enable();
		S1_WayOpt_Display(gUserAUX1Data[gGroup].Way_Opt);
	}
	else if((M7_Way_OPI_Flag == 0)||(M7_Way_OPI_Flag == 3))
	{
//		if((gMainTimerCnt_10m - M7_Way_Timer) >= 500)
		{
			ituWidgetSetVisible(S1_14_Sel, false);
			S1_WayOpt_Disable();
		}
	}
	
    return true;
}

bool S1_3_ANIStopped(ITUWidget* widget, char* param)
{
	if(S1_3_Scaleflag == 1)
	{
		S1_3_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(S1_3_ANI, false);
		ituWidgetSetVisible(S1_3_ZoomBTN, true);
	}
	else if(S1_3_Scaleflag == 2)
	{
		S1_3_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(S1_3_ANI, false);
		ituWidgetSetVisible(S1_3_BTN, true);
	}

    return true;
}

bool S1_2_ANIStopped(ITUWidget* widget, char* param)
{
	if(S1_2_Scaleflag == 1)
	{
		S1_2_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(S1_2_ANI, false);
		ituWidgetSetVisible(S1_2_ZoomBTN, true);
	}
	else if(S1_2_Scaleflag == 2)
	{
		S1_2_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(S1_2_ANI, false);
		ituWidgetSetVisible(S1_2_BTN, true);
	}

    return true;
}

bool S1_1_ANIStopped(ITUWidget* widget, char* param)
{
	if(S1_1_Scaleflag == 1)
	{
		S1_1_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(S1_1_ANI, false);
		ituWidgetSetVisible(S1_1_ZoomBTN, true);
	}
	else if(S1_1_Scaleflag == 2)
	{
		S1_1_Scaleflag = 0;		// 0; None ,1;Scale up  ,2;Scale Down
		ituWidgetSetVisible(S1_1_ANI, false);
		ituWidgetSetVisible(S1_1_BTN, true);
	}

    return true;
}

void S1_1Way_FlowRateUpdate(uint8_t tA_Value)
{
	S1_1Way_ValueUpdate(tA_Value);
	
	ituWidgetSetVisible(S1_A_Value, true);
	ituWidgetSetVisible(S1_1Way_Value, true);
}

void S1_1Way_ValueUpdate(uint8_t tA_Value)
{
	char ptFlowValue[5];
	uint8_t Cal_FlowRate_1Way=0;
	uint16_t Cal_FlowRate_1WayUS=0;

	memset(ptFlowValue,0,5);
	sprintf(ptFlowValue,"%3d",tA_Value);
	ituTextSetString(S1_A_Value, ptFlowValue);

	memset(ptFlowValue,0,5);
	Cal_FlowRate_1Way = Cal_1Way1st_FlowRate(tA_Value);

	if(SIUS_SEL_Flag == 0)	//si
	{
		sprintf(ptFlowValue,"%4d ",Cal_FlowRate_1Way);
		ituTextSetString(S1_1Way_Value, ptFlowValue);
	}
	else if(SIUS_SEL_Flag == 1)		// us
	{
		Cal_FlowRate_1WayUS = Change_Lm2gpm((uint16_t)Cal_FlowRate_1Way);
		sprintf(ptFlowValue,"%4d ",Cal_FlowRate_1WayUS);
		ituTextSetString(S1_1Way_Value, ptFlowValue);
	}
}

void S1_2aWay_FlowRateUpdate(uint8_t tA_Value)
{
	S1_2aWay_ValueUpdate(tA_Value);
	
	ituWidgetSetVisible(S1_2aWay_Value, true);
}

void S1_2aWay_ValueUpdate(uint8_t tA_Value)
{
	char ptFlowValue[5];
	uint8_t Cal_FlowRate_2aWay=0;
	uint16_t Cal_FlowRate_2aWayUS=0;

	memset(ptFlowValue,0,5);
	Cal_FlowRate_2aWay = Cal_1Way2nd_FlowRate(tA_Value);		// modify 230713 jhlee

	if(SIUS_SEL_Flag == 0)	//si
	{
		sprintf(ptFlowValue,"%4d ",Cal_FlowRate_2aWay);
		ituTextSetString(S1_2aWay_Value, ptFlowValue);
	}
	else if(SIUS_SEL_Flag == 1)		// us
	{
		Cal_FlowRate_2aWayUS = Change_Lm2gpm((uint16_t)Cal_FlowRate_2aWay);

		sprintf(ptFlowValue,"%4d ",Cal_FlowRate_2aWayUS);
		ituTextSetString(S1_2aWay_Value, ptFlowValue);
	}
}

void S1_2bWay_FlowRateUpdate(uint8_t tB_Value)
{
	S1_2bWay_ValueUpdate(tB_Value);

	ituWidgetSetVisible(S1_B_Value, true);
	ituWidgetSetVisible(S1_2bWay_Value, true);
}

void S1_2bWay_ValueUpdate(uint8_t tB_Value)
{
	char ptFlowValue[5];
	uint8_t Cal_FlowRate_2bWay=0;
	uint16_t Cal_FlowRate_2bWayUS=0;

	memset(ptFlowValue,0,5);
	sprintf(ptFlowValue,"%3d",tB_Value);
	ituTextSetString(S1_B_Value, ptFlowValue);

	memset(ptFlowValue,0,5);
	Cal_FlowRate_2bWay = Cal_1Way2nd_FlowRate(tB_Value);

	if(SIUS_SEL_Flag == 0)	//si
	{
		sprintf(ptFlowValue,"%4d ",Cal_FlowRate_2bWay);
		ituTextSetString(S1_2bWay_Value, ptFlowValue);
	}
	else if(SIUS_SEL_Flag == 1)		// us
	{
		Cal_FlowRate_2bWayUS = Change_Lm2gpm((uint16_t)Cal_FlowRate_2bWay);

		sprintf(ptFlowValue,"%4d ",Cal_FlowRate_2bWayUS);
		ituTextSetString(S1_2bWay_Value, ptFlowValue);
	}
}

void S1_Pressure_Update(uint8_t tMPa_Value)
{
	//S1_P1_Value_Update(tMPa_Value);
}

void S1_P1_Value_Update(uint8_t tMPa_Value)
{
	static uint8_t B_P1_OPI_Flag = 0;

	char ptFlowValue[5];

//printf("S1_P1_Value_Update = [%d][%d][%d]\n",M7_P1_Timer,B_P1_OPI_Flag,M7_P1_OPI_Flag);


	if(B_P1_OPI_Flag != M7_P1_OPI_Flag)
	{
		B_P1_OPI_Flag = M7_P1_OPI_Flag;

		memset(ptFlowValue,0,5);

		if((M7_P1_OPI_Flag == 0)||(M7_P1_OPI_Flag == 3))
		{
			sprintf(ptFlowValue,"N/A");
			if (S1_SelectValue==3)
			{
				if(S1_BlinkFlag != 0)
				{
					S1_SecTimer = gMainTimerCnt_10m;
					S1_1SecCNT = 0;
					S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S1_FastSource = 0;
					S1_BlinkFlag = 0;
				}
			}
			ituWidgetSetVisible(S1_C_Value, false);
			ituWidgetSetVisible(S1_C_NA, true);
			ituTextSetString(S1_C_NA, ptFlowValue);
		}
		else if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
		{
			if (((M7_P1_Timer+TIME_CNT_2_5SEC)<gMainTimerCnt_10m)||(M7_P1_Timer==0))	// JPHong  CAN_[OPI] 6Byte 0-1bit�� 0or3 �Ǵ� CAN[OPI]�� �۽����� ���� ��,�з��������� ��N/A���� ǥ��
			{
				sprintf(ptFlowValue,"N/A");
				ituTextSetString(S1_C_NA, ptFlowValue);
				ituWidgetSetVisible(S1_C_Value, false);
				ituWidgetSetVisible(S1_C_NA, true);
				M7_P1_Timer = 0;
				if (S1_SelectValue==3)
				{
					if(S1_BlinkFlag != 0)
					{
						S1_SecTimer = gMainTimerCnt_10m;
						S1_1SecCNT = 0;
						S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
	
						S1_FastSource = 0;
						S1_BlinkFlag = 0;
					}
				}
			}
			else
			{
				sprintf(ptFlowValue,"%4d",tMPa_Value);
				ituTextSetString(S1_C_Value, ptFlowValue);
				ituWidgetSetVisible(S1_C_NA, false);
				if((S1_SelectValue!=3)||(S1_BlinkFlag == 0))
					ituWidgetSetVisible(S1_C_Value, true);
			}
		}
		
		ituTextSetString(S1_C_Value, ptFlowValue);
	}
	else if((B_P1_OPI_Flag == 0)&&(M7_P1_OPI_Flag == 0))
	{
		memset(ptFlowValue,0,5);
		
		sprintf(ptFlowValue,"N/A");
		
		ituTextSetString(S1_C_NA, ptFlowValue);
		if (S1_SelectValue==3)
		{
			if(S1_BlinkFlag != 0)
			{
				S1_SecTimer = gMainTimerCnt_10m;
				S1_1SecCNT = 0;
				S1_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

				S1_FastSource = 0;
				S1_BlinkFlag = 0;
			}
		}
		ituWidgetSetVisible(S1_C_Value, false);
		ituWidgetSetVisible(S1_C_NA, true);
	}
	else if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
	{
		memset(ptFlowValue,0,5);
		if (((M7_P1_Timer+TIME_CNT_2_5SEC)<gMainTimerCnt_10m)||(M7_P1_Timer==0))	// JPHong  CAN_[OPI] 6Byte 0-1bit�� 0or3 �Ǵ� CAN[OPI]�� �۽����� ���� ��,�з��������� ��N/A���� ǥ��
		{
			sprintf(ptFlowValue,"N/A");
			ituTextSetString(S1_C_NA, ptFlowValue);
			ituWidgetSetVisible(S1_C_Value, false);
			ituWidgetSetVisible(S1_C_NA, true);
			M7_P1_Timer = 0;
			if ((S1_SelectValue==3)&&(S1_BlinkFlag))
			{
				S1_Value_Display(S1_SelectValue);
				S1_BlinkFlag = 0;
			}			
		}
		else
		{
			sprintf(ptFlowValue,"%4d",tMPa_Value);
			ituTextSetString(S1_C_Value, ptFlowValue);
			ituWidgetSetVisible(S1_C_NA, false);
			if((S1_SelectValue!=3)||(S1_BlinkFlag == 0))
				ituWidgetSetVisible(S1_C_Value, true);
		}
	}	
}

uint8_t Cal_1Way1st_FlowRate(uint8_t t1way_X)
{
	uint8_t i,t1Way_Y=0;
	double X_Delta=0,Y_Delta=0;
	double Y_Div=0;
	
	for(i=0;i<6;i++)
	{
		if(t1way_X < S1_1WAY_1st_X[0] )
		{
			t1Way_Y = S1_1WAY_1st_Y[0];
		}
		else if(t1way_X >= S1_1WAY_1st_X[5])
		{
			t1Way_Y = S1_1WAY_1st_Y[5];
		}
		else if((t1way_X >= S1_1WAY_1st_X[i]) && (t1way_X < S1_1WAY_1st_X[i+1]))
		{
			X_Delta = (double)(S1_1WAY_1st_X[i+1] - S1_1WAY_1st_X[i]);
			Y_Delta = (double)(S1_1WAY_1st_Y[i+1] - S1_1WAY_1st_Y[i]);
			
			Y_Div = (double)(Y_Delta*(t1way_X - S1_1WAY_1st_X[i]));

			//t1Way_Y = (uint8_t)round(Y_Div/X_Delta)+S1_1WAY_1st_Y[i];
			t1Way_Y = (uint8_t)floor(Y_Div/X_Delta)+S1_1WAY_1st_Y[i];//chpark.24.04.19
			
			break;
		}
	}

	return t1Way_Y;
}

uint8_t Cal_1Way2nd_FlowRate(uint8_t t1way_X)
{
	uint8_t i,t1Way_Y=0;
	double X_Delta=0,Y_Delta=0;
	double Y_Div=0;
	
	for(i=0;i<6;i++)
	{
		if(t1way_X < S1_1WAY_2nd_X[0] )
		{
			t1Way_Y = S1_1WAY_2nd_Y[0];
		}
		else if(t1way_X >= S1_1WAY_2nd_X[5])
		{
			t1Way_Y = S1_1WAY_2nd_Y[5];
		}
		else if((t1way_X >= S1_1WAY_2nd_X[i]) && (t1way_X < S1_1WAY_2nd_X[i+1]))
		{
			X_Delta = (double)(S1_1WAY_2nd_X[i+1] - S1_1WAY_2nd_X[i]);
			Y_Delta = (double)(S1_1WAY_2nd_Y[i+1] - S1_1WAY_2nd_Y[i]);
			
			Y_Div = (double)(Y_Delta*(t1way_X - S1_1WAY_2nd_X[i]));

			//t1Way_Y = (uint8_t)round(Y_Div/X_Delta)+S1_1WAY_2nd_Y[i];
			t1Way_Y = (uint8_t)floor(Y_Div/X_Delta)+S1_1WAY_2nd_Y[i];//chpark.24.04.19

			//uint8_t t1Way_test= (uint8_t)floor(1.82)+22;//chpark.24.04.19

			break;
		}
	}

	return t1Way_Y;
}

void S1_GroupClear(uint8_t MenuIndex)
{
	// switch(MenuIndex)
	// {
	// 	case 0:
	// 	{
	// 		if(ituWidgetIsVisible(S1_1_ZoomBTN) == true)
	// 		{
	// 			ituWidgetSetVisible(S1_1_ZoomBTN, false);
	// 			ituWidgetSetVisible(S1_1_ANI, true);
		
	// 			S1_1_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
	// 			ituAnimationReversePlay(S1_1_ANI, 1);
	// 		}
	// 	}
	// 	break;
		
	// 	case 1:
	// 	{
	// 		if(ituWidgetIsVisible(S1_2_ZoomBTN) == true)
	// 		{
	// 			ituWidgetSetVisible(S1_2_ZoomBTN, false);
	// 			ituWidgetSetVisible(S1_2_ANI, true);
		
	// 			S1_2_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
	// 			ituAnimationReversePlay(S1_2_ANI, 1);
	// 		}
	// 	}
	// 	break;
		
	// 	case 2:
	// 	{
	// 		if(ituWidgetIsVisible(S1_3_ZoomBTN) == true)
	// 		{
	// 			ituWidgetSetVisible(S1_3_ZoomBTN, false);
	// 			ituWidgetSetVisible(S1_3_ANI, true);
		
	// 			S1_3_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
	// 			ituAnimationReversePlay(S1_3_ANI, 1);
	// 		}
	// 	}
	// 	break;
	// }

	if(ituWidgetIsVisible(S1_1_ZoomBTN) == true || ituWidgetIsVisible(S1_1_ANI) == true)  // 2024-05-20 modi whlim  (add || ituWidgetIsVisible(S1_1_ANI) == true)
	{
		ituWidgetSetVisible(S1_1_ZoomBTN, false);
		ituWidgetSetVisible(S1_1_ANI, true);

		S1_1_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(S1_1_ANI, 1);
	}
	if(ituWidgetIsVisible(S1_2_ZoomBTN) == true || ituWidgetIsVisible(S1_2_ANI) == true)
	{
		ituWidgetSetVisible(S1_2_ZoomBTN, false);
		ituWidgetSetVisible(S1_2_ANI, true);

		S1_2_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(S1_2_ANI, 1);
	}
	if(ituWidgetIsVisible(S1_3_ZoomBTN) == true || ituWidgetIsVisible(S1_3_ANI) == true)
	{
		ituWidgetSetVisible(S1_3_ZoomBTN, false);
		ituWidgetSetVisible(S1_3_ANI, true);

		S1_3_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(S1_3_ANI, 1);
	}
}

void S1_GroupMenuAniClear(void)
{
	if(ituWidgetIsVisible(S1_1_ZoomBTN) == true)
	{
		if(gGroup != 0)
		{
			S1_1_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			S1_1_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(S1_1_ANI) == true)
	{
		if(gGroup != 0)
		{
			S1_1_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			S1_1_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(S1_1_Up1) == true)
	{
		if(gGroup != 0)
		{
			S1_1_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			S1_1_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(S1_1_Up0) == true)
	{
		if(gGroup != 0)
		{
			S1_1_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			S1_1_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(S1_2_ZoomBTN) == true)
	{
		if(gGroup != 1)
		{
			S1_2_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			S1_2_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(S1_2_ANI) == true)
	{
		if(gGroup != 1)
		{
			S1_2_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			S1_2_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(S1_2_Up1) == true)
	{
		if(gGroup != 1)
		{
			S1_2_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			S1_2_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(S1_2_Up0) == true)
	{
		if(gGroup != 1)
		{
			S1_2_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			S1_2_ANIStopped(NULL,NULL);
		}
	}

	if(ituWidgetIsVisible(S1_3_ZoomBTN) == true)
	{
		if(gGroup != 2)
		{
			S1_3_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			S1_3_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(S1_3_ANI) == true)
	{
		if(gGroup != 2)
		{
			S1_3_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			S1_3_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(S1_3_Up1) == true)
	{
		if(gGroup != 2)
		{
			S1_3_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			S1_3_ANIStopped(NULL,NULL);
		}
	}
	if(ituWidgetIsVisible(S1_3_Up0) == true)
	{
		if(gGroup != 2)
		{
			S1_3_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
			S1_3_ANIStopped(NULL,NULL);
		}
	}

	if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
	{
		if(ituWidgetIsVisible(S1_15_ZoomBTN) == true)
		{
			//if(gGroup != 0)
			if(gUserAUX1Data[gGroup].Way_Opt != 1)
			{
				S1_15_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				S1_15_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(S1_15_ANI) == true)
		{
			//if(gGroup != 0)
			if(gUserAUX1Data[gGroup].Way_Opt != 1)
			{
				S1_15_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				S1_15_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(S1_15_Up1) == true)
		{
			//if(gGroup != 0)
			if(gUserAUX1Data[gGroup].Way_Opt != 1)
			{
				S1_15_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				S1_15_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(S1_15_Up0) == true)
		{
			//if(gGroup != 0)
			if(gUserAUX1Data[gGroup].Way_Opt != 1)
			{
				S1_15_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				S1_15_ANIStopped(NULL,NULL);
			}
		}

		if(ituWidgetIsVisible(S1_16_ZoomBTN) == true)
		{
			//if(gGroup != 1)
			if(gUserAUX1Data[gGroup].Way_Opt != 2)
			{
				S1_16_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				S1_16_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(S1_16_ANI) == true)
		{
			//if(gGroup != 1)
			if(gUserAUX1Data[gGroup].Way_Opt != 2)
			{
				S1_16_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				S1_16_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(S1_16_Up1) == true)
		{
			//if(gGroup != 1)
			if(gUserAUX1Data[gGroup].Way_Opt != 2)
			{
				S1_16_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				S1_16_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(S1_16_Up0) == true)
		{
			//if(gGroup != 1)
			if(gUserAUX1Data[gGroup].Way_Opt != 2)
			{
				S1_16_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				S1_16_ANIStopped(NULL,NULL);
			}
		}

		if(ituWidgetIsVisible(S1_17_ZoomBTN) == true)
		{
			//if(gGroup != 2)
			if(gUserAUX1Data[gGroup].Way_Opt != 0)
			{
				S1_17_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				S1_17_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(S1_17_ANI) == true)
		{
			//if(gGroup != 2)
			if(gUserAUX1Data[gGroup].Way_Opt != 0)
			{
				S1_17_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				S1_17_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(S1_17_Up1) == true)
		{
			//if(gGroup != 2)
			if(gUserAUX1Data[gGroup].Way_Opt != 0)
			{
				S1_17_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				S1_17_ANIStopped(NULL,NULL);
			}
		}
		if(ituWidgetIsVisible(S1_17_Up0) == true)
		{
			//if(gGroup != 2)
			if(gUserAUX1Data[gGroup].Way_Opt != 0)
			{
				S1_17_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
				S1_17_ANIStopped(NULL,NULL);
			}
		}
	}

}

void S1_Value_Display(uint8_t tSelectValue)
{
	switch(tSelectValue)
	{
		case 1:			// A_Value
			ituWidgetSetVisible(S1_A_Value, true);
		break;
				
		case 2:			// B_Value
			ituWidgetSetVisible(S1_B_Value, true);
		break;

		case 3:			// C_Value
			ituWidgetSetVisible(S1_C_Value, true);
		break;
	}
}

void S1_SelectClear(void)
{
	if(ituWidgetIsVisible(S1_A_Sel) == true)
	{
		ituWidgetSetVisible(S1_A_Sel, false);
	}

	if(ituWidgetIsVisible(S1_B_Sel) == true)
	{
		ituWidgetSetVisible(S1_B_Sel, false);
	}

	if(ituWidgetIsVisible(S1_10_AttSel) == true)
	{
		ituWidgetSetVisible(S1_10_AttSel, false);
	}

	if(ituWidgetIsVisible(S1_C_Sel) == true)
	{
		ituWidgetSetVisible(S1_C_Sel, false);
	}

	if(ituWidgetIsVisible(S1_14_Sel) == true)
	{
		ituWidgetSetVisible(S1_14_Sel, false);
	}
}

void S1_WayOpt_Display(uint8_t tWay_Opt)
{
	static uint8_t cWay_Opt = 2;
	
	if(cWay_Opt != tWay_Opt)
	{
		cWay_Opt = tWay_Opt;  // modi whlim
		S1_WayOptClear();

		switch(tWay_Opt)
		{
			case 1:			// AUX1 1way
			{
				ituWidgetSetVisible(S1_15_BTN, false);
				ituWidgetSetVisible(S1_15_ANI, true);

				S1_15_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(S1_15_ANI,0);
			}
			break;
			
			case 2:			// AUX1 1way auto
			{
				ituWidgetSetVisible(S1_16_BTN, false);
				ituWidgetSetVisible(S1_16_ANI, true);

				S1_16_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(S1_16_ANI,0);
			}
			break;

			case 0:			// AUX1 2way
			{
				ituWidgetSetVisible(S1_17_BTN, false);
				ituWidgetSetVisible(S1_17_ANI, true);

				S1_17_Scaleflag = 1;		// 0; None ,1;Scale up  ,2;Scale Down
				ituAnimationPlay(S1_17_ANI,0);
			}
			break;
		}
	}
}

void S1_UpDn_Enable(void)
{

 printf("S1_UpDn_Enable = [%d][%d]\n",S1_UserOption,AUX_ConfigOptionData1);
	// 1st Way Flowrate
	ituWidgetSetVisible(S1_4, true);
	ituWidgetSetVisible(S1_5, true);

	// 2nd Way Flowrate
	ituWidgetSetVisible(S1_6, true);
	ituWidgetSetVisible(S1_7, true);

	// Pressure
	if(((M7_P1_OPI_Flag == 0)||(M7_P1_OPI_Flag == 3))||(((M7_P1_Timer+TIME_CNT_2_5SEC)<gMainTimerCnt_10m)||(M7_P1_Timer==0)))
	{
		ituWidgetSetVisible(S1_8, false);
		ituWidgetSetVisible(S1_9, false);
		ituWidgetSetVisible(S1_8_D, true);
		ituWidgetSetVisible(S1_9_D, true);
		M7_P1_Timer=0;
	}
	else if((M7_P1_OPI_Flag == 1)||(M7_P1_OPI_Flag == 2))
	{
		ituWidgetSetVisible(S1_8_D, false);
		ituWidgetSetVisible(S1_9_D, false);
		ituWidgetSetVisible(S1_8, true);
		ituWidgetSetVisible(S1_9, true);
	}
	
	// ATT
	ituWidgetSetVisible(S1_10_BTN, true);

}

void S1_UpDn_Disable(void)
{

 printf("S1_UpDn_Disable = [%d][%d]\n",S1_UserOption,AUX_ConfigOptionData1);
	// 1st Way Flowrate
	ituWidgetSetVisible(S1_4, false);
	ituWidgetSetVisible(S1_5, false);

	// 2nd Way Flowrate
	ituWidgetSetVisible(S1_6, false);
	ituWidgetSetVisible(S1_7, false);

	// Pressure
	ituWidgetSetVisible(S1_8, false);
	ituWidgetSetVisible(S1_9, false);
	ituWidgetSetVisible(S1_8_D, false);
	ituWidgetSetVisible(S1_9_D, false);
	
	// ATT
	ituWidgetSetVisible(S1_10_BTN, false);
	
	
	if(S1_BlinkFlag)		// on
	{
		S1_Value_Display(S1_SelectValue);
		S1_BlinkFlag = 0;
	}
}

void S1_WayOpt_Enable(void)
{
	switch(gUserAUX1Data[gGroup].Way_Opt)
	{
		case 1:
			ituWidgetSetVisible(S1_15_BTN, false);
			ituWidgetSetVisible(S1_15_ZoomBTN, true);
			
			ituWidgetSetVisible(S1_16_BTN, true);
			ituWidgetSetVisible(S1_17_BTN, true);
		break;

		case 2:
			ituWidgetSetVisible(S1_16_BTN, false);
			ituWidgetSetVisible(S1_16_ZoomBTN, true);

			ituWidgetSetVisible(S1_15_BTN, true);
			ituWidgetSetVisible(S1_17_BTN, true);
		break;

		case 0:
			ituWidgetSetVisible(S1_17_BTN, false);
			ituWidgetSetVisible(S1_17_ZoomBTN, true);

			ituWidgetSetVisible(S1_15_BTN, true);
			ituWidgetSetVisible(S1_16_BTN, true);
		break;
	}
}

void S1_WayOpt_Disable(void)
{
	ituWidgetSetVisible(S1_15_BTN, false);
	ituWidgetSetVisible(S1_15_ANI, false);
	ituWidgetSetVisible(S1_15_ZoomBTN, false);

	ituWidgetSetVisible(S1_16_BTN, false);
	ituWidgetSetVisible(S1_16_ANI, false);
	ituWidgetSetVisible(S1_16_ZoomBTN, false);

	ituWidgetSetVisible(S1_17_BTN, false);
	ituWidgetSetVisible(S1_17_ANI, false);
	ituWidgetSetVisible(S1_17_ZoomBTN, false);
}

void S1_WayOptClear(void)
{
	if(ituWidgetIsVisible(S1_15_ZoomBTN) == true || ituWidgetIsVisible(S1_15_ANI) == true) // 2024-06-10 add whlim, add (|| ituWidgetIsVisible(S1_15_ANI) == true)
	{
		ituWidgetSetVisible(S1_15_ZoomBTN, false);
		ituWidgetSetVisible(S1_15_ANI, true);

		S1_15_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(S1_15_ANI, 1);
	}
	
	if(ituWidgetIsVisible(S1_16_ZoomBTN) == true || ituWidgetIsVisible(S1_16_ANI) == true)
	{
		ituWidgetSetVisible(S1_16_ZoomBTN, false);
		ituWidgetSetVisible(S1_16_ANI, true);

		S1_16_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(S1_16_ANI, 1);
	}

	if(ituWidgetIsVisible(S1_17_ZoomBTN) == true || ituWidgetIsVisible(S1_17_ANI) == true)
	{
		ituWidgetSetVisible(S1_17_ZoomBTN, false);
		ituWidgetSetVisible(S1_17_ANI, true);

		S1_17_Scaleflag = 2;		// 0; None ,1;Scale up  ,2;Scale Down
		ituAnimationReversePlay(S1_17_ANI, 1);
	}
}

void S1_ATT_Display(uint8_t tATT_State)
{
	ITUWidget *S1_10_ATT[11];
	char tAttWigGet[15];
	
	memset(tAttWigGet,0,15);
	sprintf(tAttWigGet,"S1_10_ATT%1d",tATT_State);

	printf(" Select ATT state = %s\r\n",tAttWigGet);

	S1_10_ATT[tATT_State] = ituSceneFindWidget(&theScene, tAttWigGet);
	assert(S1_10_ATT[tATT_State]);

	S1_ATT_Clear();

	ituWidgetSetVisible(S1_10_ATT[tATT_State], true);
}

void S1_ATT_Clear(void)
{
	ITUWidget *S1_10_ATT1 = ituSceneFindWidget(&theScene, "S1_10_ATT1");
	assert(S1_10_ATT1);
	ITUWidget *S1_10_ATT2 = ituSceneFindWidget(&theScene, "S1_10_ATT2");
	assert(S1_10_ATT2);
	ITUWidget *S1_10_ATT3 = ituSceneFindWidget(&theScene, "S1_10_ATT3");
	assert(S1_10_ATT3);
	ITUWidget *S1_10_ATT4 = ituSceneFindWidget(&theScene, "S1_10_ATT4");
	assert(S1_10_ATT4);
	ITUWidget *S1_10_ATT5 = ituSceneFindWidget(&theScene, "S1_10_ATT5");
	assert(S1_10_ATT5);
	ITUWidget *S1_10_ATT6 = ituSceneFindWidget(&theScene, "S1_10_ATT6");
	assert(S1_10_ATT6);
	ITUWidget *S1_10_ATT7 = ituSceneFindWidget(&theScene, "S1_10_ATT7");
	assert(S1_10_ATT7);
	ITUWidget *S1_10_ATT8 = ituSceneFindWidget(&theScene, "S1_10_ATT8");
	assert(S1_10_ATT8);
	ITUWidget *S1_10_ATT9 = ituSceneFindWidget(&theScene, "S1_10_ATT9");
	assert(S1_10_ATT9);
	ITUWidget *S1_10_ATT10 = ituSceneFindWidget(&theScene, "S1_10_ATT10");
	assert(S1_10_ATT10);

	if(ituWidgetIsVisible(S1_10_ATT1) == true)
	{
		ituWidgetSetVisible(S1_10_ATT1, false);
	}

	if(ituWidgetIsVisible(S1_10_ATT2) == true)
	{
		ituWidgetSetVisible(S1_10_ATT2, false);
	}

	if(ituWidgetIsVisible(S1_10_ATT3) == true)
	{
		ituWidgetSetVisible(S1_10_ATT3, false);
	}

	if(ituWidgetIsVisible(S1_10_ATT4) == true)
	{
		ituWidgetSetVisible(S1_10_ATT4, false);
	}

	if(ituWidgetIsVisible(S1_10_ATT5) == true)
	{
		ituWidgetSetVisible(S1_10_ATT5, false);
	}

	if(ituWidgetIsVisible(S1_10_ATT6) == true)
	{
		ituWidgetSetVisible(S1_10_ATT6, false);
	}

	if(ituWidgetIsVisible(S1_10_ATT7) == true)
	{
		ituWidgetSetVisible(S1_10_ATT7, false);
	}

	if(ituWidgetIsVisible(S1_10_ATT8) == true)
	{
		ituWidgetSetVisible(S1_10_ATT8, false);
	}

	if(ituWidgetIsVisible(S1_10_ATT9) == true)
	{
		ituWidgetSetVisible(S1_10_ATT9, false);
	}

	if(ituWidgetIsVisible(S1_10_ATT10) == true)
	{
		ituWidgetSetVisible(S1_10_ATT10, false);
	}
}

void S1_Edit_Update(uint8_t tUpdateFlag)
{
	uint8_t i=0;
	
	gGroup = M7_Group;

	if(tUpdateFlag == 1)	// system value update -> config.ini save
	{
		for(i=0;i<3;i++)
		{
			M7_UserSet[i] = gUserAUX1Data[i];
			printf("S1_Edit_Update = [%d][%d][%d]\n",gUserAUX1Data[i].Way_Opt,M7_UserSet[AUX1CurrentOutMode].Way_Opt,M7_UserSet[AUX1CurrentOutMode].Way_Opt);
		}
	}
	else if(tUpdateFlag == 0)		// edit value resume
	{
		for(i=0;i<3;i++)
		{
			gUserAUX1Data[i] = M7_UserSet[i];
		}
	}
}

void AUX1_LimitCheck(void)
{
	uint8_t j=0;
	
	for(j=0;j<3;j++)
	{	
		if(gUserAUX1Data[j].A_Value > S1_AUX1_FlowRate_Max)
			gUserAUX1Data[j].A_Value = S1_AUX1_FlowRate_Max;
		else if(gUserAUX1Data[j].A_Value < S1_AUX1_FlowRate_Min)
			gUserAUX1Data[j].A_Value = S1_AUX1_FlowRate_Min;

		if(gUserAUX1Data[j].B_Value > S1_AUX1_FlowRate_Max)
			gUserAUX1Data[j].B_Value = S1_AUX1_FlowRate_Max;
		else if(gUserAUX1Data[j].B_Value < S1_AUX1_FlowRate_Min)
			gUserAUX1Data[j].B_Value = S1_AUX1_FlowRate_Min;

		if(gUserAUX1Data[j].P1_Value > S1_AUX1_Pressure_Max)
			gUserAUX1Data[j].P1_Value = S1_AUX1_Pressure_Max;
		else if(gUserAUX1Data[j].P1_Value < S1_AUX1_Pressure_Min)
			gUserAUX1Data[j].P1_Value = S1_AUX1_Pressure_Min;
	}
}

