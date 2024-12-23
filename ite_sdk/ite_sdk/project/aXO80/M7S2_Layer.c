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
M7_S2
S2_D7_BTN
S2_D6_BTN
S2_D5_BTN
M7S2_BG
S2_F4_Value
S2_F3_Value
S2_P4_Value
S2_H_Value
S2_G_Value
S2_F2_Value
S2_F1_Value
S2_P2_Value
S2_D_Value
S2_C_Value
S2_12
S2_11
S2_10
S2_9
S2_8
S2_7
S2_6
S2_5
S2_4
S2_3
S2_2
S2_1
S2_P4_Sel
S2_H_Sel
S2_G_Sel
S2_P2_Sel
S2_D_Sel
S2_C_Sel
M7S2_usBG
M7S2_siBG
*/

static ITUText *S2_C_Value;
static ITUText *S2_D_Value;
static ITUText *S2_P2_Value;
static ITUText *S2_F1_Value;
static ITUText *S2_F2_Value;
static ITUText *S2_G_Value;
static ITUText *S2_H_Value;
static ITUText *S2_P4_Value;
static ITUText *S2_F3_Value;
static ITUText *S2_F4_Value;

static ITUText *S2_P2_NA;
static ITUText *S2_P4_NA;


static ITUIcon *M7S2_TopAdmin;
static ITUIcon *M7S2_TopStandard;
static ITUText *M7S2_TopUserName;;
		
static ITUIcon *M7S2_usBG;
static ITUIcon *M7S2_siBG;

static ITUButton* S2_1;
static ITUButton* S2_2;
static ITUButton* S2_3;
static ITUButton* S2_4;
static ITUButton* S2_5;
static ITUButton* S2_6;
static ITUButton* S2_7;
static ITUButton* S2_8;
static ITUButton* S2_9;
static ITUButton* S2_10;
static ITUButton* S2_11;
static ITUButton* S2_12;

static ITUIcon* S2_5_D;
static ITUIcon* S2_6_D;
static ITUIcon* S2_11_D;
static ITUIcon* S2_12_D;

static ITUIcon* S2_C_Sel;
static ITUIcon* S2_D_Sel;
static ITUIcon* S2_P2_Sel;
static ITUIcon* S2_G_Sel;
static ITUIcon* S2_H_Sel;
static ITUIcon* S2_P4_Sel;



static uint32_t S2_SecTimer;
static uint8_t S2_ExitCNT = 0;
static uint8_t S2_1SecCNT = 0;

static uint32_t S2_BlinkTimer;
static uint8_t S2_BlinkFlag = 0;
static uint8_t S2_SkipFlag = 0;

static uint32_t S2_FastTimer;
static uint8_t S2_FastFlag = 0;
static uint8_t S2_FastSource = 0;

static uint8_t S2_Select_Value = 0;

UserAUX24Data cUserAUX24Data;

uint8_t S1_2WAY_X[6]={0,0,0,0,0,0};			// CAN[AUX_UL5] 0byte 2 , 1 ~ 6byte
uint8_t S1_2WAY_Y[6]={0,0,0,0,0,0};			// CAN[AUX_UL4] 0byte 2 , 1 ~ 6byte
uint8_t S1_4WAY_X[6]={0,0,0,0,0,0};			// CAN[AUX_UL5] 0byte 3 , 1 ~ 6byte
uint8_t S1_4WAY_Y[6]={0,0,0,0,0,0};			// CAN[AUX_UL4] 0byte 3 , 1 ~ 6byte

uint8_t S2_AUX2_FlowRate_Max = 250;		// CAN[AUX_U/L] 2byte
uint8_t S2_AUX2_FlowRate_Min = 0;			// CAN[AUX_U/L] 3byte
uint8_t S2_AUX2_Pressure_Max = 250;		// CAN[AUX_PU/L] 2byte
uint8_t S2_AUX2_Pressure_Min = 0;			// CAN[AUX_PU/L] 3byte

uint8_t S2_AUX4_FlowRate_Max = 250;		// CAN[AUX_U/L] 4byte
uint8_t S2_AUX4_FlowRate_Min = 0;			// CAN[AUX_U/L] 5byte
uint8_t S2_AUX4_Pressure_Max = 250;		// CAN[AUX_PU/L] 4byte
uint8_t S2_AUX4_Pressure_Min = 0;			// CAN[AUX_PU/L] 5byte

uint8_t M7_P2_OPI_Flag = 0;					// CAN[OPI] 6byte 2~3bit (AUX2 Pressure Control); 0:OPT None , 1:OPT Ok , 2:OPT hold , 3:NULL
uint8_t M7_P4_OPI_Flag = 0;					// CAN[OPI] 0byte 6~7bit (AUX4); 0:OPT None , 1:OPT Ok , 2:OPT hold , 3:NULL
uint8_t M7_P24_OPI_Flag = 0;				// CAN[OPI] 7byte 6~7bit (AUX2/4); 0:OPT None , 1:OPT Ok , 2:OPT hold , 3:NULL

static uint8_t S2_AUXFlag = 1;			// access for AUX setting flag 0;no , 1;yes
static uint8_t S2_UserLv = 9;				// User level

static uint8_t b_tOpt = 0;
uint8_t M7S2_UserDisplay_Data = 0xFF;
uint8_t AUX_ConfigOptionData2 = 0xFF;

extern UserPresetData gUserAUX1Data[3];
void S2_ValueVisable(void);

void S2_SelectClear(void);
void S2_Select_Display(uint8_t tSelectValue);

void S2_AUX2a_FlowRate_Update(uint8_t tC_Value);
void S2_AUX2a_Value_Update(uint8_t tC_Value);

void S2_AUX2b_FlowRate_Update(uint8_t tD_Value);
void S2_AUX2b_Value_Update(uint8_t tD_Value);

void S2_AUX2_Pressure_Update(uint8_t tP_Value);
void S2_AUX2_P2_Value_Update(uint8_t tP_Value);

void S2_AUX4a_FlowRate_Update(uint8_t tG_Value);
void S2_AUX4a_Value_Update(uint8_t tG_Value);

void S2_AUX4b_FlowRate_Update(uint8_t tH_Value);
void S2_AUX4b_Value_Update(uint8_t tH_Value);

void S2_AUX4_Pressure_Update(uint8_t tP_Value);
void S2_AUX4_P4_Value_Update(uint8_t tP_Value);

uint8_t Cal_2Way_FlowRate(uint8_t t1way_X);
uint8_t Cal_4Way_FlowRate(uint8_t t1way_X);

void AUX24_Init(void);
void AUX2_4_LimitCheck(void);

bool M7S2_UserDisplay(void);

void S2_UpDn_Enable(void);
void S2_UpDn_Disable(void);

extern uint8_t AUX_ConfigOption(void);

extern uint16_t Change_kpa2psi(uint16_t DateVal);
extern uint16_t Change_psi2kpa(uint16_t DataVal);

extern uint16_t Change_Lm2gpm(uint16_t DateVal);
extern uint16_t Change_gpm2Lm(uint16_t DateVal);

extern void GotoMenuTop(void);
extern void GotoMenu7(void);
void S2_Value_Display(uint8_t tSelect_Value);

bool M7S2_TouchUP_Restet(void)
{
	S2_1SecCNT = 100;
	S2_FastFlag = 0;
	S2_FastSource = 0;

	S2_SecTimer = gMainTimerCnt_10m;
}

void AUX24_Init(void)
{
	uint8_t i=0;

	S2_UserLv = User_RegInfo[M7_UserIndex].RegFlag;
//	S2_UserLv = 9;			// dev.opt
	
	S2_AUXFlag = 1;			// dev.opt access for AUX flag 0;no , 1;yes
}

bool M7S2_Enter(ITUWidget* widget, char* param)
{

	uint8_t cUserIdx;
	
	uint8_t i;
	M7S2_UserDisplay_Data = 0xFF;
	AUX_ConfigOptionData2 = 0xFF;
	
	SetWidget(S2_C_Value,"S2_C_Value");
	SetWidget(S2_D_Value,"S2_D_Value");
	SetWidget(S2_P2_Value,"S2_P2_Value");
	SetWidget(S2_F1_Value,"S2_F1_Value");
	SetWidget(S2_F2_Value,"S2_F2_Value");
	SetWidget(S2_G_Value,"S2_G_Value");
	SetWidget(S2_H_Value,"S2_H_Value");
	SetWidget(S2_P4_Value,"S2_P4_Value");
	SetWidget(S2_F3_Value,"S2_F3_Value");
	SetWidget(S2_F4_Value,"S2_F4_Value");
	SetWidget(S2_P2_NA,"S2_P2_NA");
	SetWidget(S2_P4_NA,"S2_P4_NA");
	SetWidget(M7S2_siBG,"M7S2_siBG");
	SetWidget(M7S2_usBG,"M7S2_usBG");

	SetWidget(M7S2_TopAdmin,"M7S2_TopAdmin");
	SetWidget(M7S2_TopStandard,"M7S2_TopStandard");
	SetWidget(M7S2_TopUserName,"M7S2_TopUserName");

	SetWidget(S2_1,"S2_1");
	SetWidget(S2_2,"S2_2");
	SetWidget(S2_3,"S2_3");
	SetWidget(S2_4,"S2_4");
	SetWidget(S2_5,"S2_5");
	SetWidget(S2_6,"S2_6");
	SetWidget(S2_7,"S2_7");
	SetWidget(S2_8,"S2_8");
	SetWidget(S2_9,"S2_9");
	SetWidget(S2_10,"S2_10");
	SetWidget(S2_11,"S2_11");
	SetWidget(S2_12,"S2_12");

	SetWidget(S2_5_D,"S2_5_D");
	SetWidget(S2_6_D,"S2_6_D");
	SetWidget(S2_11_D,"S2_11_D");
	SetWidget(S2_12_D,"S2_12_D");

	SetWidget(S2_C_Sel,"S2_C_Sel");
	SetWidget(S2_D_Sel,"S2_D_Sel");
	SetWidget(S2_P2_Sel,"S2_P2_Sel");
	SetWidget(S2_G_Sel,"S2_G_Sel");
	SetWidget(S2_H_Sel,"S2_H_Sel");
	SetWidget(S2_P4_Sel,"S2_P4_Sel");
	
	if(SIUS_SEL_Flag == 0)	//si
	{
		ituWidgetSetVisible(M7S2_usBG, false);
		ituWidgetSetVisible(M7S2_siBG, true);
	}
	else if(SIUS_SEL_Flag == 1)		// us
	{
		ituWidgetSetVisible(M7S2_siBG, false);
		ituWidgetSetVisible(M7S2_usBG, true);
	}

	for(i=0;i<3;i++)
		gUserAUX1Data[i] = M7_UserSet[i];

	cUserAUX24Data = M7_UserAUX24Set;

	AUX1_LimitCheck();
	for(i=0;i<3;i++)
		M7_UserSet[i] = gUserAUX1Data[i];

	AUX2_4_LimitCheck();
	M7_UserAUX24Set = cUserAUX24Data;

	S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);
	S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);
	S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);
	S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);
	S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);
	S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);

	S2_SelectClear();

	//=======================

	b_tOpt = AUX_ConfigOption();
	if(b_tOpt == 0)
	{
		S2_Select_Value = 0;
		S2_UpDn_Disable();
	}
	else
	{
		S2_Select_Value = 1;		// C value
		S2_Select_Display(S2_Select_Value);
		S2_UpDn_Enable();
	}

	//=======================

	M7S2_UserDisplay();

	S2_SecTimer = gMainTimerCnt_10m;
	S2_ExitCNT = 0;

	S2_BlinkFlag = 0;
	
	S2_1SecCNT = 100;	
	S2_FastFlag = 0;
	S2_FastSource = 0;
	
    return true;
}

bool M7S2_Timer(ITUWidget* widget, char* param)
{

#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	
	if((gMainTimerCnt_10m - S2_SecTimer) >= 100)
	{
		if(S2_ExitCNT < 30)
			S2_ExitCNT++;

		if(S2_ExitCNT == 30)
		{
			S2_ExitCNT = 100;
			printf("\r\nM7S2 Menu 30Sec Exit > goto Main\r\n");
			GotoMain(MAIN_MODE_NORMAL); 
		}

		// long arrow btn check
		if(S2_1SecCNT < 1)
			S2_1SecCNT++;

		if(S2_1SecCNT == 1)
		{
			S2_1SecCNT == 100;
			printf("\r\nS2 Long key\r\n");

			if(S2_FastFlag == 1)
			{
				S2_FastFlag = 2;
				S2_FastTimer = gMainTimerCnt_10m;
			}
		}
			
		S2_SecTimer = gMainTimerCnt_10m;
	}

	//if((S2_BlinkFlag == 0)||(S2_BlinkFlag == 1))
	{
		AUX1_LimitCheck();
		AUX2_4_LimitCheck();

		S2_AUX2a_Value_Update(cUserAUX24Data.C_Value);
		S2_AUX2b_Value_Update(cUserAUX24Data.D_Value);
		S2_AUX2_P2_Value_Update(cUserAUX24Data.P2_Value);
		S2_AUX4a_Value_Update(cUserAUX24Data.G_Value);
		S2_AUX4b_Value_Update(cUserAUX24Data.H_Value);
		S2_AUX4_P4_Value_Update(cUserAUX24Data.P4_Value);

		//=======================
		
		if (AUX_ConfigOptionData2!=AUX_ConfigOption())
		{
			AUX_ConfigOptionData2=AUX_ConfigOption();
			if(AUX_ConfigOptionData2 == 0)
			{
	//			S2_Select_Value = 0;   // JPHong 2024-03-07  �̰� Ȯ�� �ʿ�
				S2_UpDn_Disable();
				S2_SelectClear();
				if(S2_BlinkFlag != 0)		// on
				{
		//		printf("AUX_ConfigOptionData2 = [%d][%d][%d]\n",AUX_ConfigOptionData2,S2_Select_Value,S2_BlinkFlag);
					S2_Value_Display(S2_Select_Value );
				}
				S2_BlinkFlag = 0;
			}
			else if((AUX_ConfigOption() == 1)||(AUX_ConfigOption() == 2))
			{
	//			S2_Select_Value = 1;
				S2_UpDn_Enable();
			}
		}
	}

	if(S2_BlinkFlag == 1)		// on
	{
		if((gMainTimerCnt_10m - S2_BlinkTimer) >= 30) // 300msec
		{
	//		printf("S2_BlinkFlag = [%d][%d][%d]\n",S2_BlinkTimer,S2_Select_Value,S2_BlinkFlag);
			S2_Value_Display(S2_Select_Value );

			S2_BlinkFlag = 2;
			S2_BlinkTimer = gMainTimerCnt_10m;
		}
	}
	else if(S2_BlinkFlag == 2)	// off
	{
		if((gMainTimerCnt_10m - S2_BlinkTimer) >= 30) // 300msec
		{
			switch(S2_Select_Value)
			{
				case 1:
					ituWidgetSetVisible(S2_C_Value, false);
				break;

				case 2:
					ituWidgetSetVisible(S2_D_Value, false);
				break;

				case 3:
					ituWidgetSetVisible(S2_P2_Value, false);
				break;

				case 4:
					ituWidgetSetVisible(S2_G_Value, false);
				break;

				case 5:
					ituWidgetSetVisible(S2_H_Value, false);
				break;

				case 6:
					ituWidgetSetVisible(S2_P4_Value, false);
				break;
			}

			S2_BlinkFlag = 1;
			S2_BlinkTimer = gMainTimerCnt_10m;
		}
	}

	if(S2_FastFlag == 2)
	{
		// fast up/down count
		switch(S2_FastSource)
		{
			case 1:				// C value down count
				if((gMainTimerCnt_10m - S2_FastTimer) >= 10)
				{
					if(cUserAUX24Data.C_Value > S2_AUX2_FlowRate_Min)
						cUserAUX24Data.C_Value--;
					
					S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);

					S2_FastTimer = gMainTimerCnt_10m;
				}
			break;

			case 2:				// C Value up count
				if((gMainTimerCnt_10m - S2_FastTimer) >= 10)
				{
					if(cUserAUX24Data.C_Value < S2_AUX2_FlowRate_Max)
						cUserAUX24Data.C_Value++;
					
					S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);

					S2_FastTimer = gMainTimerCnt_10m;
				}
			break;
			
			case 3:				// D Value down count
				if((gMainTimerCnt_10m - S2_FastTimer) >= 10)
				{
					if(cUserAUX24Data.D_Value > S2_AUX2_FlowRate_Min)
						cUserAUX24Data.D_Value--;
					
					S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);

					S2_FastTimer = gMainTimerCnt_10m;
				}
			break;
			
			case 4:				// D Value up count
				if((gMainTimerCnt_10m - S2_FastTimer) >= 10)
				{
					if(cUserAUX24Data.D_Value < S2_AUX2_FlowRate_Max)
						cUserAUX24Data.D_Value++;
					
					S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);

					S2_FastTimer = gMainTimerCnt_10m;
				}
			break;
			
			case 5:				// P2 Value down count
				if((gMainTimerCnt_10m - S2_FastTimer) >= 10)
				{
					if(cUserAUX24Data.P2_Value == S2_AUX2_Pressure_Max)
					{
						if((S2_AUX2_Pressure_Max%2)==1)
							cUserAUX24Data.P2_Value -= 1;
						else
							cUserAUX24Data.P2_Value -= 2;
					}
					else if(cUserAUX24Data.P2_Value >= (S2_AUX2_Pressure_Min+2))
						cUserAUX24Data.P2_Value -= 2;
					else if(cUserAUX24Data.P2_Value < (S2_AUX2_Pressure_Min+2))
						cUserAUX24Data.P2_Value = S2_AUX2_Pressure_Min;

					S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);

					S2_FastTimer = gMainTimerCnt_10m;
				}
			break;
			
			case 6:				// P2 Value up count
				if((gMainTimerCnt_10m - S2_FastTimer) >= 10)
				{
					if(cUserAUX24Data.P2_Value == S2_AUX2_Pressure_Min)
					{
						if((S2_AUX2_Pressure_Min%2) == 1)
							cUserAUX24Data.P2_Value += 1;
						else 
							cUserAUX24Data.P2_Value += 2;
					}
					else
					{
						cUserAUX24Data.P2_Value += 2;
					}
	
					if(cUserAUX24Data.P2_Value > S2_AUX2_Pressure_Max)
						cUserAUX24Data.P2_Value = S2_AUX2_Pressure_Max;
					
					S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);

					S2_FastTimer = gMainTimerCnt_10m;
				}
			break;

			case 7:				// G Value down count
				if((gMainTimerCnt_10m - S2_FastTimer) >= 10)
				{
					if(cUserAUX24Data.G_Value > S2_AUX4_FlowRate_Min)
						cUserAUX24Data.G_Value--;
					
					S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);

					S2_FastTimer = gMainTimerCnt_10m;
				}
			break;
			
			case 8:				// G Value up count
				if((gMainTimerCnt_10m - S2_FastTimer) >= 10)
				{
					if(cUserAUX24Data.G_Value < S2_AUX4_FlowRate_Max)
						cUserAUX24Data.G_Value++;
					
					S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);

					S2_FastTimer = gMainTimerCnt_10m;
				}
			break;

			case 9:				// H Value down count
				if((gMainTimerCnt_10m - S2_FastTimer) >= 10)
				{
					if(cUserAUX24Data.H_Value > S2_AUX4_FlowRate_Min)
						cUserAUX24Data.H_Value--;
					
					S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);

					S2_FastTimer = gMainTimerCnt_10m;
				}
			break;
			
			case 10:				// H Value up count
				if((gMainTimerCnt_10m - S2_FastTimer) >= 10)
				{
					if(cUserAUX24Data.H_Value < S2_AUX4_FlowRate_Max)
						cUserAUX24Data.H_Value++;
					
					S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);

					S2_FastTimer = gMainTimerCnt_10m;
				}
			break;

			case 11:				// P4 Value down count
				if((gMainTimerCnt_10m - S2_FastTimer) >= 10)
				{
					if(cUserAUX24Data.P4_Value == S2_AUX4_Pressure_Max)
					{
						if((S2_AUX4_Pressure_Max%2)==1)
							cUserAUX24Data.P4_Value -= 1;
						else
							cUserAUX24Data.P4_Value -= 2;
					}
					else if(cUserAUX24Data.P4_Value >= (S2_AUX4_Pressure_Min+2))
						cUserAUX24Data.P4_Value -= 2;
					else if(cUserAUX24Data.P4_Value < (S2_AUX4_Pressure_Min+2))
						cUserAUX24Data.P4_Value = S2_AUX4_Pressure_Min;

					S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);

					S2_FastTimer = gMainTimerCnt_10m;
				}
			break;
			
			case 12:				// P4 Value up count
				if((gMainTimerCnt_10m - S2_FastTimer) >= 10)
				{
					if(cUserAUX24Data.P4_Value == S2_AUX4_Pressure_Min)
					{
						if((S2_AUX4_Pressure_Min%2) == 1)
							cUserAUX24Data.P4_Value += 1;
						else 
							cUserAUX24Data.P4_Value += 2;
					}
					else
					{
						cUserAUX24Data.P4_Value += 2;
					}
	
					if(cUserAUX24Data.P4_Value > S2_AUX4_Pressure_Max)
						cUserAUX24Data.P4_Value = S2_AUX4_Pressure_Max;
					
					S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);

					S2_FastTimer = gMainTimerCnt_10m;
				}
			break;
		}
	}

	M7S2_UserDisplay();

    return true;
}

bool M7S2_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t tJogWheel = 0;
	
	S2_ExitCNT = 0;
	tJogWheel = param[0];
	
	printf("Menu AUX2 Preset JogWheel [%d][%d][%d]\r\n",tJogWheel,S2_BlinkFlag,S2_Select_Value);
	
	if(tJogWheel == JOG_DIAL_ROT_LEFT)
	{
		if(S2_BlinkFlag == 0)
		{
			// no blink > select change , 1>6or5 ,2>1 ,3>2 ,4>3or2 ,5>4, 6>5
			if(S2_Select_Value == 1)
			{
				if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
					S2_Select_Value = 6;
				else
					S2_Select_Value = 5;
			}
			else if(S2_Select_Value == 2)
				S2_Select_Value = 1;
			else if(S2_Select_Value == 3)
				S2_Select_Value = 2;
			else if(S2_Select_Value == 4)
			{
				if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
					S2_Select_Value = 3;
				else
					S2_Select_Value = 2;
			}
			else if(S2_Select_Value == 5)
				S2_Select_Value = 4;
			else if(S2_Select_Value == 6)
				S2_Select_Value = 5;

			S2_SelectClear();
			S2_Select_Display(S2_Select_Value);
		}
		else if(S2_BlinkFlag != 0)
		{
			// blink > edit value down count
			switch(S2_Select_Value)
			{
				case 1:				// C value
				{
					if(cUserAUX24Data.C_Value > S2_AUX2_FlowRate_Min)
						cUserAUX24Data.C_Value--;
			
					S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);
				}
				break;
				
				case 2:				// D value
				{
					if(cUserAUX24Data.D_Value > S2_AUX2_FlowRate_Min)
						cUserAUX24Data.D_Value--;
			
					S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);
				}
				break;

				case 3:				// P2 value
				{
					if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
					{
						if(cUserAUX24Data.P2_Value == S2_AUX2_Pressure_Max)
						{
							if((S2_AUX2_Pressure_Max%2)==1)
								cUserAUX24Data.P2_Value -= 1;
							else
								cUserAUX24Data.P2_Value -= 2;
						}
						else if(cUserAUX24Data.P2_Value >= (S2_AUX2_Pressure_Min+2))
							cUserAUX24Data.P2_Value -= 2;
						else if(cUserAUX24Data.P2_Value < (S2_AUX2_Pressure_Min+2))
							cUserAUX24Data.P2_Value = S2_AUX2_Pressure_Min;
    
						S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);
					}
				}
				break;

				case 4:				// G value
				{
					if(cUserAUX24Data.G_Value > S2_AUX4_FlowRate_Min)
						cUserAUX24Data.G_Value--;
			
					S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);
				}
				break;

				case 5:				// H value
				{
					if(cUserAUX24Data.H_Value > S2_AUX4_FlowRate_Min)
						cUserAUX24Data.H_Value--;
			
					S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);
				}
				break;

				case 6:				// P4 value
				{
					if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
					{
						if(cUserAUX24Data.P4_Value == S2_AUX4_Pressure_Max)
						{
							if((S2_AUX4_Pressure_Max%2)==1)
								cUserAUX24Data.P4_Value -= 1;
							else
								cUserAUX24Data.P4_Value -= 2;
						}
						else if(cUserAUX24Data.P4_Value >= (S2_AUX4_Pressure_Min+2))
							cUserAUX24Data.P4_Value -= 2;
						else if(cUserAUX24Data.P4_Value < (S2_AUX4_Pressure_Min+2))
							cUserAUX24Data.P4_Value = S2_AUX4_Pressure_Min;
    
						S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);
					}
				}
				break;
			}
		}
	}
	else if(tJogWheel == JOG_DIAL_ROT_RIGHT)
	{
		if(S2_BlinkFlag == 0)
		{
			// no blink > select change , 1>2 ,2>3or4 ,3>4 ,4>5 ,5>6or1 ,6>1
			if(S2_Select_Value == 1)
				S2_Select_Value = 2;
			else if(S2_Select_Value == 2)
			{
				if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
					S2_Select_Value = 3;
				else
					S2_Select_Value = 4;
			}
			else if(S2_Select_Value == 3)
				S2_Select_Value = 4;
			else if(S2_Select_Value == 4)
				S2_Select_Value = 5;
			else if(S2_Select_Value == 5)
			{
				if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
					S2_Select_Value = 6;		// value
				else
					S2_Select_Value = 1;		// N/A
			}
			else if(S2_Select_Value == 6)
				S2_Select_Value = 1;

			S2_SelectClear();
			S2_Select_Display(S2_Select_Value);
		}
		else if(S2_BlinkFlag != 0)
		{
			// blink > edit value up count
			switch(S2_Select_Value)
			{
				case 1:				// C value
				{
					if(cUserAUX24Data.C_Value < S2_AUX2_FlowRate_Max)
						cUserAUX24Data.C_Value++;
		
					S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);
				}
				break;
				
				case 2:				// D value
				{
					if(cUserAUX24Data.D_Value < S2_AUX2_FlowRate_Max)
						cUserAUX24Data.D_Value++;

					S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);
				}
				break;

				case 3:				// P2 value
				{
					if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
					{
						if(cUserAUX24Data.P2_Value == S2_AUX2_Pressure_Min)
						{
							if((S2_AUX2_Pressure_Min%2) == 1)
								cUserAUX24Data.P2_Value += 1;
							else 
								cUserAUX24Data.P2_Value += 2;
						}
						else
						{
							cUserAUX24Data.P2_Value += 2;
						}

						if(cUserAUX24Data.P2_Value > S2_AUX2_Pressure_Max)
							cUserAUX24Data.P2_Value = S2_AUX2_Pressure_Max;

						S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);
					}
				}
				break;

				case 4:				// G value
				{
					if(cUserAUX24Data.G_Value < S2_AUX4_FlowRate_Max)
						cUserAUX24Data.G_Value++;

					S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);
				}
				break;

				case 5:				// H value
				{
					if(cUserAUX24Data.H_Value < S2_AUX4_FlowRate_Max)
						cUserAUX24Data.H_Value++;

					S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);
				}
				break;

				case 6:				// P4 value
				{
					if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
					{
						if(cUserAUX24Data.P4_Value == S2_AUX4_Pressure_Min)
						{
							if((S2_AUX4_Pressure_Min%2) == 1)
								cUserAUX24Data.P4_Value += 1;
							else 
								cUserAUX24Data.P4_Value += 2;
						}
						else
						{
							cUserAUX24Data.P4_Value += 2;
						}

						if(cUserAUX24Data.P4_Value > S2_AUX4_Pressure_Max)
							cUserAUX24Data.P4_Value = S2_AUX4_Pressure_Max;

						S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);
					}
				}
				break;
			}
		}
	}

    return true;
}

bool M7S2_JogButton(ITUWidget* widget, char* param)
{
	uint8_t tJogButton = 0,tValue = 0;;

	S2_ExitCNT = 0;
	tJogButton = param[0];
	
	printf("Menu AUX2 Preset JogButton %d\r\n",tJogButton);

	switch(tJogButton)
	{
		case JOG_DIAL_KEY_UP:
		{
			if(S2_BlinkFlag == 0)
			{
				if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				{
					// no blink > select change
					// up
					// 1>6or4, 2>5
					// 3>1
					// 4>3or1, 5>2
					// 6>4
					uint8_t tDest_up1[7] = {0,6,5,1,3,2,4};

					tValue = tDest_up1[S2_Select_Value];

					if(tValue == 3)
					{
						if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
							S2_Select_Value = tValue;
						//else if((M7_P2_OPI_Flag == 0)||(M7_P2_OPI_Flag == 3))
						else
							S2_Select_Value = 1;
					}
					else if(tValue == 6)
					{
						//if(((M7_P2_OPI_Flag == 0)||(M7_P2_OPI_Flag == 3))&&((M7_P4_OPI_Flag == 0)||(M7_P4_OPI_Flag == 3))&&((M7_P24_OPI_Flag == 1)||(M7_P24_OPI_Flag == 2)))
						//	S2_Select_Value = 4;
						if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
							S2_Select_Value = 6;
						else
							S2_Select_Value = 4;
					}
					else
						S2_Select_Value = tValue;
				}
				else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				{
					// no blink > select change
					// up
					// 1>4, 2>5, 3>6or3
					// 4>1, 5>2, 6>3or6
					uint8_t tDest_up2[7] = {0,4,5,6,1,2,3};

					tValue = tDest_up2[S2_Select_Value];

					if(tValue == 3)
					{
						if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
							S2_Select_Value = tValue;
						//else if((M7_P2_OPI_Flag == 0)||(M7_P2_OPI_Flag == 3))
						else
							S2_Select_Value = 6;
					}
					else if(tValue == 6)
					{
						//if(((M7_P2_OPI_Flag == 0)||(M7_P2_OPI_Flag == 3))&&((M7_P4_OPI_Flag == 0)||(M7_P4_OPI_Flag == 3))&&((M7_P24_OPI_Flag == 1)||(M7_P24_OPI_Flag == 2)))
						//	S2_Select_Value = 3;
						if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
							S2_Select_Value = 6;
						else
							S2_Select_Value = 3;
					}
					else					
						S2_Select_Value = tValue;
				}
				
				S2_SelectClear();
				S2_Select_Display(S2_Select_Value);
			}
			else if(S2_BlinkFlag != 0)
			{
				// blink > long key check start
				switch(S2_Select_Value)
				{
					case 1:				// C value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 2;
					}
					break;
					
					case 2:				// D value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 4;
					}
					break;
					
					case 3:				// P2 value
					{
						if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
						{
							S2_SecTimer = gMainTimerCnt_10m;
							S2_1SecCNT = 0;
							S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
							S2_FastSource = 6;
						}
					}
					break;
					
					case 4:				// G value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 8;
					}
					break;
					
					case 5:				// H value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 10;
					}
					break;

					case 6:				// P4 value
					{
						if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
						{	
							S2_SecTimer = gMainTimerCnt_10m;
							S2_1SecCNT = 0;
							S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
							S2_FastSource = 12;
						}
					}
					break;
				}			
			}
		}
		break;
		
		case JOG_DIAL_KEY_UP_OFF:
		{
			if(S2_BlinkFlag != 0)
			{
				// blink > edit value up count , fast up count stop
				switch(S2_Select_Value)
				{
					case 1:				// C value
					{
						if(cUserAUX24Data.C_Value < S2_AUX2_FlowRate_Max)
							cUserAUX24Data.C_Value++;
			
						S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);

						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;

						S2_SecTimer = gMainTimerCnt_10m;
					}
					break;
					
					case 2:				// D value
					{
						if(cUserAUX24Data.D_Value < S2_AUX2_FlowRate_Max)
							cUserAUX24Data.D_Value++;

						S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);

						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;

						S2_SecTimer = gMainTimerCnt_10m;
					}
					break;

					case 3:				// P2 value
					{
						if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
						{
							if(cUserAUX24Data.P2_Value == S2_AUX2_Pressure_Min)
							{
								if((S2_AUX2_Pressure_Min%2) == 1)
									cUserAUX24Data.P2_Value += 1;
								else 
									cUserAUX24Data.P2_Value += 2;
							}
							else
							{
								cUserAUX24Data.P2_Value += 2;
							}

							if(cUserAUX24Data.P2_Value > S2_AUX2_Pressure_Max)
								cUserAUX24Data.P2_Value = S2_AUX2_Pressure_Max;

							S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);
    
							// long key check stop
							S2_1SecCNT = 100;
							S2_FastFlag = 0;
							S2_FastSource = 0;

							S2_SecTimer = gMainTimerCnt_10m;	
						}
					}
					break;

					case 4:				// G value
					{
						if(cUserAUX24Data.G_Value < S2_AUX4_FlowRate_Max)
							cUserAUX24Data.G_Value++;

						S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);

						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;

						S2_SecTimer = gMainTimerCnt_10m;	
					}
					break;

					case 5:				// H value
					{
						if(cUserAUX24Data.H_Value < S2_AUX4_FlowRate_Max)
							cUserAUX24Data.H_Value++;

						S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);

						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;

						S2_SecTimer = gMainTimerCnt_10m;	
					}
					break;

					case 6:				// P4 value
					{
						if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
						{
							if(cUserAUX24Data.P4_Value == S2_AUX4_Pressure_Min)
							{
								if((S2_AUX4_Pressure_Min%2) == 1)
									cUserAUX24Data.P4_Value += 1;
								else 
									cUserAUX24Data.P4_Value += 2;
							}
							else
							{
								cUserAUX24Data.P4_Value += 2;
							}

							if(cUserAUX24Data.P4_Value > S2_AUX4_Pressure_Max)
								cUserAUX24Data.P4_Value = S2_AUX4_Pressure_Max;

							S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);
    
							// long key check stop
							S2_1SecCNT = 100;
							S2_FastFlag = 0;
							S2_FastSource = 0;

							S2_SecTimer = gMainTimerCnt_10m;	
						}
					}
					break;
					
				}						
			}
		}
		break;
		
		case JOG_DIAL_KEY_DOWN:
		{
			if(S2_BlinkFlag == 0)
			{
				if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				{
					// no blink > select change
					// down
					// 1>3or4, 2>5
					// 3>4
					// 4>6or1, 5>2
					// 6>1
					uint8_t tDest_down1[7] = {0,3,5,4,6,2,1};

					tValue = tDest_down1[S2_Select_Value];

					if(tValue == 3)
					{
						if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
							S2_Select_Value = tValue;
						//else if((M7_P2_OPI_Flag == 0)||(M7_P2_OPI_Flag == 3))
						else
							S2_Select_Value = 4;
					}
					else if(tValue == 6)
					{
						//if(((M7_P2_OPI_Flag == 0)||(M7_P2_OPI_Flag == 3))&&((M7_P4_OPI_Flag == 0)||(M7_P4_OPI_Flag == 3))&&((M7_P24_OPI_Flag == 1)||(M7_P24_OPI_Flag == 2)))
						//	S2_Select_Value = 1;
						if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
							S2_Select_Value = 6;
						else
							S2_Select_Value = 1;
					}
					else
						S2_Select_Value = tValue;
				}
				else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				{
					// no blink > select change
					// down
					// 1>4, 2>5, 3>6or3
					// 4>1, 5>2, 6>3or6
					uint8_t tDest_down2[7] = {0,4,5,6,1,2,3};

					tValue = tDest_down2[S2_Select_Value];

					if(tValue == 3)
					{
						if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
							S2_Select_Value = tValue;
						else if((M7_P2_OPI_Flag == 0)||(M7_P2_OPI_Flag == 3))
							S2_Select_Value = 6;
					}
					else if(tValue == 6)
					{
						//if(((M7_P2_OPI_Flag == 0)||(M7_P2_OPI_Flag == 3))&&((M7_P4_OPI_Flag == 0)||(M7_P4_OPI_Flag == 3))&&((M7_P24_OPI_Flag == 1)||(M7_P24_OPI_Flag == 2)))
						//	S2_Select_Value = 3;
						if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
							S2_Select_Value = 6;
						else
							S2_Select_Value = 3;
					}
					else
						S2_Select_Value = tValue;
				}

				S2_SelectClear();
				S2_Select_Display(S2_Select_Value);
			}
			else if(S2_BlinkFlag != 0)
			{
				// blink > long key check start
				switch(S2_Select_Value)
				{
					case 1:				// C value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 1;
					}
					break;
					
					case 2:				// D value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 3;
					}
					break;

					case 3:				// P2 value
					{
						if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
						{
							S2_SecTimer = gMainTimerCnt_10m;
							S2_1SecCNT = 0;
							S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
							S2_FastSource = 5;
						}
					}
					break;

					case 4:				// G value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 7;
					}
					break;
					
					case 5:				// H value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 9;
					}
					break;

					case 6:				// P4 value
					{
						if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
						{
							S2_SecTimer = gMainTimerCnt_10m;
							S2_1SecCNT = 0;
							S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
							S2_FastSource = 11;
						}
					}
					break;
				}
			}
		}
		break;
		
		case JOG_DIAL_KEY_DOWN_OFF:
		{
			if(S2_BlinkFlag != 0)
			{
				// blink > edit value down count , fast up count stop
				switch(S2_Select_Value)
				{
					case 1:				// C value
					{
						if(cUserAUX24Data.C_Value > S2_AUX2_FlowRate_Min)
							cUserAUX24Data.C_Value--;
						
						S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);
  
						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;
  
						S2_SecTimer = gMainTimerCnt_10m;	
					}
					break;

					case 2:				// D value
					{
						if(cUserAUX24Data.D_Value > S2_AUX2_FlowRate_Min)
							cUserAUX24Data.D_Value--;
						
						S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);
    
						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;
    
						S2_SecTimer = gMainTimerCnt_10m;	
					}
					break;

					case 3:				// P2 value
					{
						if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
						{
							if(cUserAUX24Data.P2_Value == S2_AUX2_Pressure_Max)
							{
								if((S2_AUX2_Pressure_Max%2)==1)
									cUserAUX24Data.P2_Value -= 1;
								else
									cUserAUX24Data.P2_Value -= 2;
							}
							else if(cUserAUX24Data.P2_Value >= (S2_AUX2_Pressure_Min+2))
								cUserAUX24Data.P2_Value -= 2;
							else if(cUserAUX24Data.P2_Value < (S2_AUX2_Pressure_Min+2))
								cUserAUX24Data.P2_Value = S2_AUX2_Pressure_Min;
      
							S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);
      
							// long key check stop
							S2_1SecCNT = 100;
							S2_FastFlag = 0;
							S2_FastSource = 0;
      
							S2_SecTimer = gMainTimerCnt_10m;	
						}
					}
					break;

					case 4:				// G value
					{
						if(cUserAUX24Data.G_Value > S2_AUX4_FlowRate_Min)
							cUserAUX24Data.G_Value--;
						
						S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);
  
						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;
  
						S2_SecTimer = gMainTimerCnt_10m;	
					}
					break;

					case 5:				// H value
					{
						if(cUserAUX24Data.H_Value > S2_AUX4_FlowRate_Min)
							cUserAUX24Data.H_Value--;
						
						S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);
    
						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;
    
						S2_SecTimer = gMainTimerCnt_10m;	
					}
					break;

					case 6:				// P4 value
					{
						if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
						{
							if(cUserAUX24Data.P4_Value == S2_AUX4_Pressure_Max)
							{
								if((S2_AUX4_Pressure_Max%2)==1)
									cUserAUX24Data.P4_Value -= 1;
								else
									cUserAUX24Data.P4_Value -= 2;
							}
							else if(cUserAUX24Data.P4_Value >= (S2_AUX4_Pressure_Min+2))
								cUserAUX24Data.P4_Value -= 2;
							else if(cUserAUX24Data.P4_Value < (S2_AUX4_Pressure_Min+2))
								cUserAUX24Data.P4_Value = S2_AUX4_Pressure_Min;
      
							S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);
      
							// long key check stop
							S2_1SecCNT = 100;
							S2_FastFlag = 0;
							S2_FastSource = 0;
      
							S2_SecTimer = gMainTimerCnt_10m;	
						}
					}
					break;
				}
			}
		}
		break;
		
		case JOG_DIAL_KEY_LEFT:
		{
			if(S2_BlinkFlag == 0)
			{
				if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				{
					// no blink > select change
					// left
					// 1>2, 2>1
					// 3>2
					// 4>5, 5>4
					// 6>5
					uint8_t tDest_left1[7] = {0,2,1,2,5,4,5};

					tValue = tDest_left1[S2_Select_Value];

					S2_Select_Value = tValue;
				}
				else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				{
					// no blink > select change
					// left
					// 1>3or2, 2>1, 3>2
					// 4>6or5, 5>4 ,6>5
					uint8_t tDest_left2[7] = {0,3,1,2,6,4,5};

					tValue = tDest_left2[S2_Select_Value];

					if(tValue == 3)
					{
						if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
							S2_Select_Value = tValue;
						else if((M7_P2_OPI_Flag == 0)||(M7_P2_OPI_Flag == 3))
							S2_Select_Value = 2;
					}
					else if(tValue == 6)
					{
						//if(((M7_P2_OPI_Flag == 0)||(M7_P2_OPI_Flag == 3))&&((M7_P4_OPI_Flag == 0)||(M7_P4_OPI_Flag == 3))&&((M7_P24_OPI_Flag == 1)||(M7_P24_OPI_Flag == 2)))
						//	S2_Select_Value = 5;
						if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
							S2_Select_Value = 6;
						else
							S2_Select_Value = 5;
					}
					else
						S2_Select_Value = tValue;
				}

				S2_SelectClear();
				S2_Select_Display(S2_Select_Value);
			}
			else if(S2_BlinkFlag != 0)
			{
				// blink > long key check start
				switch(S2_Select_Value)
				{
					case 1:				// C value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 1;
					}
					break;
					
					case 2:				// D value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 3;
					}
					break;

					case 3:				// P2 value
					{
						if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
						{
							S2_SecTimer = gMainTimerCnt_10m;
							S2_1SecCNT = 0;
							S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
							S2_FastSource = 5;
						}
					}
					break;

					case 4:				// G value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 7;
					}
					break;
					
					case 5:				// H value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 9;
					}
					break;

					case 6:				// P4 value
					{
						if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
						{
							S2_SecTimer = gMainTimerCnt_10m;
							S2_1SecCNT = 0;
							S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
							S2_FastSource = 11;
						}
					}
					break;
				}
			}
		}
		break;
		
		case JOG_DIAL_KEY_LEFT_OFF:
		{
			if(S2_BlinkFlag != 0)
			{
				// blink > edit value down count , fast up count stop
				switch(S2_Select_Value)
				{
					case 1:				// C value
					{
						if(cUserAUX24Data.C_Value > S2_AUX2_FlowRate_Min)
							cUserAUX24Data.C_Value--;
						
						S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);
  
						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;
  
						S2_SecTimer = gMainTimerCnt_10m;	
					}
					break;

					case 2:				// D value
					{
						if(cUserAUX24Data.D_Value > S2_AUX2_FlowRate_Min)
							cUserAUX24Data.D_Value--;
						
						S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);
    
						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;
    
						S2_SecTimer = gMainTimerCnt_10m;	
					}
					break;

					case 3:				// P2 value
					{
						if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
						{
							if(cUserAUX24Data.P2_Value == S2_AUX2_Pressure_Max)
							{
								if((S2_AUX2_Pressure_Max%2)==1)
									cUserAUX24Data.P2_Value -= 1;
								else
									cUserAUX24Data.P2_Value -= 2;
							}
							else if(cUserAUX24Data.P2_Value >= (S2_AUX2_Pressure_Min+2))
								cUserAUX24Data.P2_Value -= 2;
							else if(cUserAUX24Data.P2_Value < (S2_AUX2_Pressure_Min+2))
								cUserAUX24Data.P2_Value = S2_AUX2_Pressure_Min;
      
							S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);
      
							// long key check stop
							S2_1SecCNT = 100;
							S2_FastFlag = 0;
							S2_FastSource = 0;
      
							S2_SecTimer = gMainTimerCnt_10m;	
						}
					}
					break;

					case 4:				// G value
					{
						if(cUserAUX24Data.G_Value > S2_AUX4_FlowRate_Min)
							cUserAUX24Data.G_Value--;
						
						S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);
  
						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;
  
						S2_SecTimer = gMainTimerCnt_10m;	
					}
					break;

					case 5:				// H value
					{
						if(cUserAUX24Data.H_Value > S2_AUX4_FlowRate_Min)
							cUserAUX24Data.H_Value--;
						
						S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);
    
						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;
    
						S2_SecTimer = gMainTimerCnt_10m;	
					}
					break;

					case 6:				// P4 value
					{
						if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
						{
							if(cUserAUX24Data.P4_Value == S2_AUX4_Pressure_Max)
							{
								if((S2_AUX4_Pressure_Max%2)==1)
									cUserAUX24Data.P4_Value -= 1;
								else
									cUserAUX24Data.P4_Value -= 2;
							}
							else if(cUserAUX24Data.P4_Value >= (S2_AUX4_Pressure_Min+2))
								cUserAUX24Data.P4_Value -= 2;
							else if(cUserAUX24Data.P4_Value < (S2_AUX4_Pressure_Min+2))
								cUserAUX24Data.P4_Value = S2_AUX4_Pressure_Min;
      
							S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);
      
							// long key check stop
							S2_1SecCNT = 100;
							S2_FastFlag = 0;
							S2_FastSource = 0;
      
							S2_SecTimer = gMainTimerCnt_10m;	
						}
					}
					break;
				}
			}
		}
		break;
		
		case JOG_DIAL_KEY_RIGHT:
		{
			if(S2_BlinkFlag == 0)
			{
				if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
				{
					// no blink > select change
					// right
					// 1>2, 2>1
					// 3>2
					// 4>5, 5>4
					// 6>5
					uint8_t tDest_right1[7] = {0,2,1,2,5,4,5};

					tValue = tDest_right1[S2_Select_Value];

					S2_Select_Value = tValue;
				}
				else if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
				{
					// no blink > select change
					// right
					// 1>2, 2>3or1, 3>1
					// 4>5, 5>6or4, 6>4
					uint8_t tDest_right2[7] = {0,2,3,1,5,6,4};

					tValue = tDest_right2[S2_Select_Value];

					if(tValue == 3)
					{
						if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
							S2_Select_Value = tValue;
						//else if((M7_P2_OPI_Flag == 0)||(M7_P2_OPI_Flag == 3))
						else
							S2_Select_Value = 1;
					}
					else if(tValue == 6)
					{
						//if(((M7_P2_OPI_Flag == 0)||(M7_P2_OPI_Flag == 3))&&((M7_P4_OPI_Flag == 0)||(M7_P4_OPI_Flag == 3))&&((M7_P24_OPI_Flag == 1)||(M7_P24_OPI_Flag == 2)))
						//	S2_Select_Value = 4;
						if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
							S2_Select_Value = 6;
						else
							S2_Select_Value = 4;
					}
					else
						S2_Select_Value = tValue;
				}

				S2_SelectClear();
				S2_Select_Display(S2_Select_Value);
			}
			else if(S2_BlinkFlag != 0)
			{
				// blink > long key check start
				switch(S2_Select_Value)
				{
					case 1:				// C value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 2;
					}
					break;
					
					case 2:				// D value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 4;
					}
					break;

					case 3:				// P2 value
					{
						if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
						{
							S2_SecTimer = gMainTimerCnt_10m;
							S2_1SecCNT = 0;
							S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
							S2_FastSource = 6;
						}
					}
					break;

					case 4:				// G value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 8;
					}
					break;
					
					case 5:				// H value
					{
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
						S2_FastSource = 10;
					}
					break;

					case 6:				// P4 value
					{
						if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
						{
							S2_SecTimer = gMainTimerCnt_10m;
							S2_1SecCNT = 0;
							S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
							S2_FastSource = 12;
						}
					}
					break;
				}
			}
		}
		break;
		
		case JOG_DIAL_KEY_RIGHT_OFF:
		{
			if(S2_BlinkFlag != 0)
			{
				// blink > edit value up count , fast up count stop
				switch(S2_Select_Value)
				{
					case 1:				// C value
					{
						if(cUserAUX24Data.C_Value < S2_AUX2_FlowRate_Max)
							cUserAUX24Data.C_Value++;
			
						S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);

						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;

						S2_SecTimer = gMainTimerCnt_10m;
					}
					break;
					
					case 2:				// D value
					{
						if(cUserAUX24Data.D_Value < S2_AUX2_FlowRate_Max)
							cUserAUX24Data.D_Value++;

						S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);

						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;

						S2_SecTimer = gMainTimerCnt_10m;
					}
					break;

					case 3:				// P2 value
					{
						if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
						{
							if(cUserAUX24Data.P2_Value == S2_AUX2_Pressure_Min)
							{
								if((S2_AUX2_Pressure_Min%2) == 1)
									cUserAUX24Data.P2_Value += 1;
								else 
									cUserAUX24Data.P2_Value += 2;
							}
							else
							{
								cUserAUX24Data.P2_Value += 2;
							}

							if(cUserAUX24Data.P2_Value > S2_AUX2_Pressure_Max)
								cUserAUX24Data.P2_Value = S2_AUX2_Pressure_Max;

							S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);
    
							// long key check stop
							S2_1SecCNT = 100;
							S2_FastFlag = 0;
							S2_FastSource = 0;

							S2_SecTimer = gMainTimerCnt_10m;	
						}
					}
					break;

					case 4:				// G value
					{
						if(cUserAUX24Data.G_Value < S2_AUX4_FlowRate_Max)
							cUserAUX24Data.G_Value++;

						S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);

						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;

						S2_SecTimer = gMainTimerCnt_10m;	
					}
					break;

					case 5:				// H value
					{
						if(cUserAUX24Data.H_Value < S2_AUX4_FlowRate_Max)
							cUserAUX24Data.H_Value++;

						S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);

						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;

						S2_SecTimer = gMainTimerCnt_10m;	
					}
					break;

					case 6:				// P4 value
					{
						if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
						{
							if(cUserAUX24Data.P4_Value == S2_AUX4_Pressure_Min)
							{
								if((S2_AUX4_Pressure_Min%2) == 1)
									cUserAUX24Data.P4_Value += 1;
								else 
									cUserAUX24Data.P4_Value += 2;
							}
							else
							{
								cUserAUX24Data.P4_Value += 2;
							}

							if(cUserAUX24Data.P4_Value > S2_AUX4_Pressure_Max)
								cUserAUX24Data.P4_Value = S2_AUX4_Pressure_Max;

							S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);
    
							// long key check stop
							S2_1SecCNT = 100;
							S2_FastFlag = 0;
							S2_FastSource = 0;

							S2_SecTimer = gMainTimerCnt_10m;	
						}
					}
					break;
				}						
			}
		}
		break;

		case JOG_DIAL_KEY_ENTER_ON:
		{
			if(S2_BlinkFlag == 0)
			{
				// no blink > blink , edit value passable
				S2_BlinkTimer = gMainTimerCnt_10m;
				S2_BlinkFlag = 1;
			}
			else if(S2_BlinkFlag != 0)
			{
				// blink > blink stop
				S2_BlinkFlag = 0;

				S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);
				S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);
				S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);
				S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);
				S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);
				S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);

				M7_UserAUX24Set = cUserAUX24Data;
				S2_SkipFlag = 2;
			}
		}
		break;
		
		case JOG_DIAL_KEY_ENTER_OFF:
		{
			if(S2_SkipFlag == 2)
			{
				// blink > blink stop > edit value system update
				S2_SkipFlag = 0;
	
				S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);
				S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);
				S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);
				S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);
				S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);
				S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
				//M7_UserAUX24Set.C_Value
				//M7_UserAUX24Set.D_Value
				//M7_UserAUX24Set.P2_Value
				//M7_UserAUX24Set.G_Value
				//M7_UserAUX24Set.H_Value
				//M7_UserAUX24Set.P4_Value
				XO_Save_Config(M7_UserIndex,true,0);
#else
				HT_Save_Config(1);
#endif
				
				S2_1SecCNT = 100;	
				S2_FastFlag = 0;
				S2_FastSource = 0;
				
				S2_ValueVisable();
			}
		}
		break;
	}

    return true;
}

bool M7S2_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t tJogSwitch = 0;
	
	S2_ExitCNT = 0;
	tJogSwitch = param[0];
	
	printf("Menu AUX2 Preset JogSwitch %d\r\n",tJogSwitch);

	if(tJogSwitch == JOG_DIAL_SW_ESC_ON)
	{
		if(S2_BlinkFlag != 0)
		{
			M7S2_TouchUP_Restet(); // 2024-05-21 add whlim
			S2_BlinkFlag = 0;
		
			cUserAUX24Data = M7_UserAUX24Set;
			S2_SkipFlag = 1;
		}
	}
	else if(tJogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
		if(S2_SkipFlag == 1)
		{
			S2_SkipFlag = 0;
			
			S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);
			S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);
			S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);
			S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);
			S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);
			S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);
		}
		else if(S2_SkipFlag == 0)
		{
			GotoMenu7();
		}
	}
	else if(tJogSwitch == JOG_DIAL_SW_HOME)
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}

    return true;
}

bool S2D7_MouseDn(ITUWidget* widget, char* param)
{
	S2_ExitCNT = 0;
		
	if(S2_BlinkFlag == 0)
	{
		// no blink > blink value
		S2_BlinkTimer = gMainTimerCnt_10m;
		S2_BlinkFlag = 1;
	}
	else if(S2_BlinkFlag != 0)
	{
		// blink > no blink , edit value system update
		S2_BlinkFlag = 0;

		S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);
		S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);
		S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);
		S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);
		S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);
		S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);
		
		M7_UserAUX24Set = cUserAUX24Data;
		S2_SkipFlag = 2;
	}

    return true;
}

bool S2D7_MouseUp(ITUWidget* widget, char* param)
{
	// blink > no blink , edit value system update
	S2_ExitCNT = 0;

	if(S2_SkipFlag == 2)
	{
		S2_SkipFlag = 0;
		
		S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);
		S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);
		S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);
		S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);
		S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);
		S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);

		M7_UserAUX24Set = cUserAUX24Data;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
		//M7_UserAUX24Set.C_Value
		//M7_UserAUX24Set.D_Value
		//M7_UserAUX24Set.P2_Value
		//M7_UserAUX24Set.G_Value
		//M7_UserAUX24Set.H_Value
		//M7_UserAUX24Set.P4_Value
		XO_Save_Config(M7_UserIndex,true,0);
#else
		HT_Save_Config(1);
#endif
		
		//S2_SelectClear();
		//S2_Select_Value = 1;		// C value
		//S2_Select_Display(S2_Select_Value);

		S2_SecTimer = gMainTimerCnt_10m;
		S2_BlinkFlag = 0;
	
		S2_1SecCNT = 100;	
		S2_FastFlag = 0;
		S2_FastSource = 0;
	}

    return true;
}

bool S2D6_MouseUp(ITUWidget* widget, char* param)
{
	
	GotoMain(MAIN_MODE_NORMAL); 
	
    return true;
}

bool S2D5_MouseDn(ITUWidget* widget, char* param)
{
	S2_ExitCNT = 0;

	// blink > no blink , edit value resume , skip goto menu7
	if(S2_BlinkFlag != 0)
	{
		S2_BlinkFlag = 0;
		
		cUserAUX24Data = M7_UserAUX24Set;
		S2_SkipFlag = 1;
	}

    return true;
}

bool S2D5_MouseUp(ITUWidget* widget, char* param)
{

	S2_ExitCNT = 0;

	if(S2_SkipFlag == 1)
	{
		S2_SkipFlag = 0;
		
		S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);
		S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);
		S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);
		S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);
		S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);
		S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);
	}
	else if(S2_SkipFlag == 0)
	{
		GotoMenu7();
	}
	
    return true;
}

bool S2_UP_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tUP_button = 0;
	
	S2_ExitCNT = 0;
	tUP_button = (uint8_t)atoi(param);
	printf("S2_UP_MouseDn : [%d][%d][%d]\r\n",tUP_button,S2_Select_Value,S2_BlinkFlag);
	switch(tUP_button)
	{
		case 2:					// C value up btn
			if(S2_Select_Value != 1)
			{
				if(S2_BlinkFlag != 0)
				{
					// not select 1 > blink > edit value system update
					M7_UserAUX24Set = cUserAUX24Data;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
					//M7_UserAUX24Set.C_Value
					//M7_UserAUX24Set.D_Value
					//M7_UserAUX24Set.P2_Value
					//M7_UserAUX24Set.G_Value
					//M7_UserAUX24Set.H_Value
					//M7_UserAUX24Set.P4_Value
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
				}

				// not select 1 > select 1 change
				S2_SelectClear();
				S2_Select_Value = 1;		// C value
				S2_BlinkFlag = 0;
				S2_Select_Display(S2_Select_Value);
				S2_ValueVisable();
			}
			else if(S2_Select_Value == 1)
			{
				if(S2_BlinkFlag != 0)
				{
					// blink > long key check start
					S2_SecTimer = gMainTimerCnt_10m;
					S2_1SecCNT = 0;
					S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S2_FastSource = 2;
				}
			}
		break;
		
		case 4:					// D value up btn
			if(S2_Select_Value != 2)
			{
				if(S2_BlinkFlag != 0)
				{
					// not select 1 > blink > edit value system update
					M7_UserAUX24Set = cUserAUX24Data;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
					//M7_UserAUX24Set.C_Value
					//M7_UserAUX24Set.D_Value
					//M7_UserAUX24Set.P2_Value
					//M7_UserAUX24Set.G_Value
					//M7_UserAUX24Set.H_Value
					//M7_UserAUX24Set.P4_Value
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
				}

				// not select 2 > select 2 change
				S2_SelectClear();
				S2_Select_Value = 2;		// D value
				S2_BlinkFlag = 0;
				S2_Select_Display(S2_Select_Value);
				S2_ValueVisable();
			}
			else if(S2_Select_Value == 2)
			{
				if(S2_BlinkFlag != 0)
				{
					// blink > long key check start
					S2_SecTimer = gMainTimerCnt_10m;
					S2_1SecCNT = 0;
					S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S2_FastSource = 4;
				}
			}
		break;
		
		case 6:					// P value up btn
			if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
			{
				if(S2_Select_Value != 3)
				{
					if(S2_BlinkFlag != 0)
					{
						// not select 1 > blink > edit value system update
						M7_UserAUX24Set = cUserAUX24Data;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
					//M7_UserAUX24Set.C_Value
					//M7_UserAUX24Set.D_Value
					//M7_UserAUX24Set.P2_Value
					//M7_UserAUX24Set.G_Value
					//M7_UserAUX24Set.H_Value
					//M7_UserAUX24Set.P4_Value
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
					}

					// not select 3 > select 3 change
					S2_SelectClear();
					S2_Select_Value = 3;		// P2 value
					S2_BlinkFlag = 0;
					S2_Select_Display(S2_Select_Value);
					S2_ValueVisable();
				}
				else if(S2_Select_Value == 3)
				{
					if(S2_BlinkFlag != 0)
					{
						// blink > long key check start
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
    
						S2_FastSource = 6;
					}
				}
			}
		break;
		
		case 8:					// G value up btn
			if(S2_Select_Value != 4)
			{
				if(S2_BlinkFlag != 0)
				{
					// not select 1 > blink > edit value system update
					M7_UserAUX24Set = cUserAUX24Data;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
					//M7_UserAUX24Set.C_Value
					//M7_UserAUX24Set.D_Value
					//M7_UserAUX24Set.P2_Value
					//M7_UserAUX24Set.G_Value
					//M7_UserAUX24Set.H_Value
					//M7_UserAUX24Set.P4_Value
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
				}

				// not select 4 > select 4 change
				S2_SelectClear();
				S2_Select_Value = 4;		// G value
				S2_BlinkFlag = 0;
				S2_Select_Display(S2_Select_Value);
				S2_ValueVisable();
			}
			else if(S2_Select_Value == 4)
			{
				if(S2_BlinkFlag != 0)
				{
					// blink > long key check start
					S2_SecTimer = gMainTimerCnt_10m;
					S2_1SecCNT = 0;
					S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S2_FastSource = 8;
				}
			}
		break;

		case 10:				// H value up btn
			if(S2_Select_Value != 5)
			{
				if(S2_BlinkFlag != 0)
				{
					// not select 1 > blink > edit value system update
					M7_UserAUX24Set = cUserAUX24Data;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
					//M7_UserAUX24Set.C_Value
					//M7_UserAUX24Set.D_Value
					//M7_UserAUX24Set.P2_Value
					//M7_UserAUX24Set.G_Value
					//M7_UserAUX24Set.H_Value
					//M7_UserAUX24Set.P4_Value
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
				}

				// not select 5 > select 5 change
				S2_SelectClear();
				S2_Select_Value = 5;		// H value
				S2_BlinkFlag = 0;
				S2_Select_Display(S2_Select_Value);
				S2_ValueVisable();
			}
			else if(S2_Select_Value == 5)
			{
				if(S2_BlinkFlag != 0)
				{
					// blink > long key check start
					S2_SecTimer = gMainTimerCnt_10m;
					S2_1SecCNT = 0;
					S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S2_FastSource = 10;
				}
			}
		break;
		
		case 12:
			if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
			{
				if(S2_Select_Value != 6)
				{
					if(S2_BlinkFlag != 0)
					{
						// not select 1 > blink > edit value system update
						M7_UserAUX24Set = cUserAUX24Data;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
					//M7_UserAUX24Set.C_Value
					//M7_UserAUX24Set.D_Value
					//M7_UserAUX24Set.P2_Value
					//M7_UserAUX24Set.G_Value
					//M7_UserAUX24Set.H_Value
					//M7_UserAUX24Set.P4_Value
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
					}

					// not select 3 > select 3 change
					S2_SelectClear();
					S2_Select_Value = 6;		// P4 value
					S2_BlinkFlag = 0;
					S2_Select_Display(S2_Select_Value);
					S2_ValueVisable();
				}
				else if(S2_Select_Value == 6)
				{
					if(S2_BlinkFlag != 0)
					{
						// blink > long key check start
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
    
						S2_FastSource = 12;
					}
				}
			}
		break;
	}

    return true;
}

bool S2_UP_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tUP_button = 0;
	
	S2_ExitCNT = 0;
	tUP_button = (uint8_t)atoi(param);
	printf("S2_UP_MouseUp : [%d][%d][%d]\r\n",tUP_button,S2_Select_Value,S2_BlinkFlag);
	switch(tUP_button)
	{
		case 2:					// C value up btn
			if(S2_Select_Value == 1)
			{
				if(S2_BlinkFlag == 0)
				{
					// select 1 > no blink > blink
					S2_BlinkTimer = gMainTimerCnt_10m;
					S2_BlinkFlag = 1;
				}
				else if(S2_BlinkFlag != 0)
				{
					// blink > edit c value up count , fast down count stop ,longkey check stop
					if(cUserAUX24Data.C_Value < S2_AUX2_FlowRate_Max)
						cUserAUX24Data.C_Value++;
					
					S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);

					// long key check stop
					S2_1SecCNT = 100;
					S2_FastFlag = 0;
					S2_FastSource = 0;

					S2_SecTimer = gMainTimerCnt_10m;	
				}
			}
		break;
		
		case 4:					// D value up btn
			if(S2_Select_Value == 2)
			{
				if(S2_BlinkFlag == 0)
				{
					// select 2 > no blink > blink
					S2_BlinkTimer = gMainTimerCnt_10m;
					S2_BlinkFlag = 1;
				}
				else if(S2_BlinkFlag != 0)
				{
					// blink > edit d value up count , fast down count stop ,longkey check stop
					if(cUserAUX24Data.D_Value < S2_AUX2_FlowRate_Max)
						cUserAUX24Data.D_Value++;
					
					S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);

					// long key check stop
					S2_1SecCNT = 100;
					S2_FastFlag = 0;
					S2_FastSource = 0;

					S2_SecTimer = gMainTimerCnt_10m;	
				}
			}
		break;
		
		case 6:					// P2 value up btn
			if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
			{
				if(S2_Select_Value == 3)
				{
					if(S2_BlinkFlag == 0)
					{
						// select 3 > no blink > blink
						S2_BlinkTimer = gMainTimerCnt_10m;
						S2_BlinkFlag = 1;
					}
					else if(S2_BlinkFlag != 0)
					{
						// blink > edit P value up count , fast down count stop ,longkey check stop
						if(cUserAUX24Data.P2_Value == S2_AUX2_Pressure_Min)
						{
							if((S2_AUX2_Pressure_Min%2) == 1)
								cUserAUX24Data.P2_Value += 1;
							else 
								cUserAUX24Data.P2_Value += 2;
						}
						else
						{
							cUserAUX24Data.P2_Value += 2;
						}
		
						if(cUserAUX24Data.P2_Value > S2_AUX2_Pressure_Max)
							cUserAUX24Data.P2_Value = S2_AUX2_Pressure_Max;
						
						S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);
  
						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;
  
						S2_SecTimer = gMainTimerCnt_10m;	
					}
				}
			}
		break;
		
		case 8:					// G value up btn
			if(S2_Select_Value == 4)
			{
				if(S2_BlinkFlag == 0)
				{
					// select 4 > no blink > blink
					S2_BlinkTimer = gMainTimerCnt_10m;
					S2_BlinkFlag = 1;
				}
				else if(S2_BlinkFlag != 0)
				{
					// blink > edit G value up count , fast down count stop ,longkey check stop
					if(cUserAUX24Data.G_Value < S2_AUX4_FlowRate_Max)
						cUserAUX24Data.G_Value++;
					
					S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);

					// long key check stop
					S2_1SecCNT = 100;
					S2_FastFlag = 0;
					S2_FastSource = 0;

					S2_SecTimer = gMainTimerCnt_10m;	
				}
			}
		break;

		case 10:				// H value up btn
			if(S2_Select_Value == 5)
			{
				if(S2_BlinkFlag == 0)
				{
					// select 5 > no blink > blink
					S2_BlinkTimer = gMainTimerCnt_10m;
					S2_BlinkFlag = 1;
				}
				else if(S2_BlinkFlag != 0)
				{
					// blink > edit H value up count , fast down count stop ,longkey check stop
					if(cUserAUX24Data.H_Value < S2_AUX4_FlowRate_Max)
						cUserAUX24Data.H_Value++;
					
					S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);

					// long key check stop
					S2_1SecCNT = 100;
					S2_FastFlag = 0;
					S2_FastSource = 0;

					S2_SecTimer = gMainTimerCnt_10m;	
				}
			}
		break;
		
		case 12:
			if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
			{
				if(S2_Select_Value == 6)
				{
					if(S2_BlinkFlag == 0)
					{
						// select 6 > no blink > blink
						S2_BlinkTimer = gMainTimerCnt_10m;
						S2_BlinkFlag = 1;
					}
					else if(S2_BlinkFlag != 0)
					{
						// blink > edit P4 value up count , fast down count stop ,longkey check stop
						if(cUserAUX24Data.P4_Value == S2_AUX4_Pressure_Min)
						{
							if((S2_AUX4_Pressure_Min%2) == 1)
								cUserAUX24Data.P4_Value += 1;
							else 
								cUserAUX24Data.P4_Value += 2;
						}
						else
						{
							cUserAUX24Data.P4_Value += 2;
						}
		
						if(cUserAUX24Data.P4_Value > S2_AUX4_Pressure_Max)
							cUserAUX24Data.P4_Value = S2_AUX4_Pressure_Max;
						
						S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);
  
						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;
  
						S2_SecTimer = gMainTimerCnt_10m;	
					}
				}
			}
		break;
	}

    return true;
}

bool S2_DN_MouseDn(ITUWidget* widget, char* param)
{
	uint8_t tDN_button = 0;
	
	S2_ExitCNT = 0;
	tDN_button = (uint8_t)atoi(param);
	printf("S2_DN_MouseDn : [%d][%d][%d]\r\n",tDN_button,S2_Select_Value,S2_BlinkFlag);
	switch(tDN_button)
	{
		case 1:					// C value down btn
			if(S2_Select_Value != 1)
			{
				if(S2_BlinkFlag != 0)
				{
					// not select 1 > blink > edit value system update
					M7_UserAUX24Set = cUserAUX24Data;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
					//M7_UserAUX24Set.C_Value
					//M7_UserAUX24Set.D_Value
					//M7_UserAUX24Set.P2_Value
					//M7_UserAUX24Set.G_Value
					//M7_UserAUX24Set.H_Value
					//M7_UserAUX24Set.P4_Value
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
				}

				// not select 1 > select 1 change
				S2_SelectClear();
				S2_Select_Value = 1;		// C value
				S2_BlinkFlag = 0;
				S2_Select_Display(S2_Select_Value);
				S2_ValueVisable();
			}
			else if(S2_Select_Value == 1)
			{
				if(S2_BlinkFlag != 0)
				{
					// blink > long key check start
					S2_SecTimer = gMainTimerCnt_10m;
					S2_1SecCNT = 0;
					S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S2_FastSource = 1;
				}
			}
		break;
		
		case 3:					// D value down btn
		printf("S2_DN_MouseDn 3: [%d][%d][%d]\r\n",tDN_button,S2_Select_Value,S2_BlinkFlag);
			if(S2_Select_Value != 2)
			{
			printf("S2_DN_MouseDn 33: [%d][%d][%d]\r\n",tDN_button,S2_Select_Value,S2_BlinkFlag);
				if(S2_BlinkFlag != 0)
				{
					// not select 1 > blink > edit value system update
					M7_UserAUX24Set = cUserAUX24Data;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
					//M7_UserAUX24Set.C_Value
					//M7_UserAUX24Set.D_Value
					//M7_UserAUX24Set.P2_Value
					//M7_UserAUX24Set.G_Value
					//M7_UserAUX24Set.H_Value
					//M7_UserAUX24Set.P4_Value
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
				}
				// not select 2 > select 2 change
				S2_SelectClear();
				S2_Select_Value = 2;		// D value  
				S2_BlinkFlag = 0;
				S2_Select_Display(S2_Select_Value);
				S2_ValueVisable();
			}
			else if(S2_Select_Value == 2)
			{
				if(S2_BlinkFlag != 0)
				{
					// blink > long key check start
					S2_SecTimer = gMainTimerCnt_10m;
					S2_1SecCNT = 0;
					S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S2_FastSource = 3;
				}
			}
			printf("S2_DN_MouseDn 333: [%d][%d][%d]\r\n",tDN_button,S2_Select_Value,S2_BlinkFlag);
		break;
		
		case 5:					// P2 value down btn
			if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
			{
				if(S2_Select_Value != 3)
				{
					if(S2_BlinkFlag != 0)
					{
						// not select 1 > blink > edit value system update
						M7_UserAUX24Set = cUserAUX24Data;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
					//M7_UserAUX24Set.C_Value
					//M7_UserAUX24Set.D_Value
					//M7_UserAUX24Set.P2_Value
					//M7_UserAUX24Set.G_Value
					//M7_UserAUX24Set.H_Value
					//M7_UserAUX24Set.P4_Value
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
					}

					// not select 3 > select 3 change
					S2_SelectClear();
					S2_Select_Value = 3;		// P value
					S2_BlinkFlag = 0;
					S2_Select_Display(S2_Select_Value);
					S2_ValueVisable();
				}
				else if(S2_Select_Value == 3)
				{
					if(S2_BlinkFlag != 0)
					{
						// blink > long key check start
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
    
						S2_FastSource = 5;
					}
				}
			}
		break;
		
		case 7:					// G value down btn
			if(S2_Select_Value != 4)
			{
				if(S2_BlinkFlag != 0)
				{
					// not select 1 > blink > edit value system update
					M7_UserAUX24Set = cUserAUX24Data;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
					//M7_UserAUX24Set.C_Value
					//M7_UserAUX24Set.D_Value
					//M7_UserAUX24Set.P2_Value
					//M7_UserAUX24Set.G_Value
					//M7_UserAUX24Set.H_Value
					//M7_UserAUX24Set.P4_Value
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
				}

				// not select 4 > select 4 change
				S2_SelectClear();
				S2_Select_Value = 4;		// G value
				S2_BlinkFlag = 0;
				S2_Select_Display(S2_Select_Value);
				S2_ValueVisable();
			}
			else if(S2_Select_Value == 4)
			{
				if(S2_BlinkFlag != 0)
				{
					// blink > long key check start
					S2_SecTimer = gMainTimerCnt_10m;
					S2_1SecCNT = 0;
					S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S2_FastSource = 7;
				}
			}
		break;

		case 9:					// H value down btn
			if(S2_Select_Value != 5)
			{
				if(S2_BlinkFlag != 0)
				{
					// not select 1 > blink > edit value system update
					M7_UserAUX24Set = cUserAUX24Data;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
					//M7_UserAUX24Set.C_Value
					//M7_UserAUX24Set.D_Value
					//M7_UserAUX24Set.P2_Value
					//M7_UserAUX24Set.G_Value
					//M7_UserAUX24Set.H_Value
					//M7_UserAUX24Set.P4_Value
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
				}

				// not select 5 > select 5 change
				S2_SelectClear();
				S2_Select_Value = 5;		// H value
				S2_BlinkFlag = 0;
				S2_Select_Display(S2_Select_Value);
				S2_ValueVisable();
			}
			else if(S2_Select_Value == 5)
			{
				if(S2_BlinkFlag != 0)
				{
					// blink > long key check start
					S2_SecTimer = gMainTimerCnt_10m;
					S2_1SecCNT = 0;
					S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

					S2_FastSource = 9;
				}
			}
		break;
		
		case 11:
			if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
			{
				if(S2_Select_Value != 6)
				{
					if(S2_BlinkFlag != 0)
					{
						// not select 1 > blink > edit value system update
						M7_UserAUX24Set = cUserAUX24Data;

#ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
					//M7_UserAUX24Set.C_Value
					//M7_UserAUX24Set.D_Value
					//M7_UserAUX24Set.P2_Value
					//M7_UserAUX24Set.G_Value
					//M7_UserAUX24Set.H_Value
					//M7_UserAUX24Set.P4_Value
					XO_Save_Config(M7_UserIndex,true,0);
#else
					HT_Save_Config(1);
#endif
					}

					// not select 6 > select 6 change
					S2_SelectClear();
					S2_Select_Value = 6;		// P4 value
					S2_BlinkFlag = 0;
					S2_Select_Display(S2_Select_Value);
					S2_ValueVisable();
				}
				else if(S2_Select_Value == 6)
				{
					if(S2_BlinkFlag != 0)
					{
						// blink > long key check start
						S2_SecTimer = gMainTimerCnt_10m;
						S2_1SecCNT = 0;
						S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
    
						S2_FastSource = 11;
					}
				}
			}
		break;
	}

    return true;
}

bool S2_DN_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t tDN_button = 0;

	S2_ExitCNT = 0;
	tDN_button = (uint8_t)atoi(param);
	printf("S2_DN_MouseUp : [%d][%d][%d]\r\n",tDN_button,S2_Select_Value,S2_BlinkFlag);
	switch(tDN_button)
	{
		case 1:					// C value down btn
			if(S2_Select_Value == 1)
			{
				if(S2_BlinkFlag == 0)
				{
					// select 1 > no blink > blink
					S2_BlinkTimer = 0;
					S2_BlinkFlag = 1;
				}
				else if(S2_BlinkFlag != 0)
				{
					// blink > edit c value down count , fast down count stop ,longkey check stop
					if(cUserAUX24Data.C_Value > S2_AUX2_FlowRate_Min)
						cUserAUX24Data.C_Value--;
					
					S2_AUX2a_FlowRate_Update(cUserAUX24Data.C_Value);

					// long key check stop
					S2_1SecCNT = 100;
					S2_FastFlag = 0;
					S2_FastSource = 0;

					S2_SecTimer = gMainTimerCnt_10m;	
				}
			}
		break;
		
		case 3:					// D value down btn
			if(S2_Select_Value == 2)
			{
				if(S2_BlinkFlag == 0)
				{
					// select 1 > no blink > blink
					S2_BlinkTimer = 0;
					S2_BlinkFlag = 1;
				}
				else if(S2_BlinkFlag != 0)
				{
					// blink > edit c value down count , fast down count stop ,longkey check stop
					if(cUserAUX24Data.D_Value > S2_AUX2_FlowRate_Min)
						cUserAUX24Data.D_Value--;
					
					S2_AUX2b_FlowRate_Update(cUserAUX24Data.D_Value);

					// long key check stop
					S2_1SecCNT = 100;
					S2_FastFlag = 0;
					S2_FastSource = 0;

					S2_SecTimer = gMainTimerCnt_10m;	
				}
			}
		break;
		
		case 5:					// P2 value down btn
			if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
			{
				if(S2_Select_Value == 3)
				{
					if(S2_BlinkFlag == 0)
					{
						// select 3 > no blink > blink
						S2_BlinkTimer = 0;
						S2_BlinkFlag = 1;
					}
					else if(S2_BlinkFlag != 0)
					{
						// blink > edit P value down count , fast down count stop ,longkey check stop
						if(cUserAUX24Data.P2_Value == S2_AUX2_Pressure_Max)
						{
							if((S2_AUX2_Pressure_Max%2)==1)
								cUserAUX24Data.P2_Value -= 1;
							else
								cUserAUX24Data.P2_Value -= 2;
						}
						else if(cUserAUX24Data.P2_Value >= (S2_AUX2_Pressure_Min+2))
							cUserAUX24Data.P2_Value -= 2;
						else if(cUserAUX24Data.P2_Value < (S2_AUX2_Pressure_Min+2))
							cUserAUX24Data.P2_Value = S2_AUX2_Pressure_Min;
    
						S2_AUX2_Pressure_Update(cUserAUX24Data.P2_Value);
    
						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;
    
						S2_SecTimer = gMainTimerCnt_10m;	
					}
				}
			}
		break;
		
		case 7:					// G value down btn
			if(S2_Select_Value == 4)
			{
				if(S2_BlinkFlag == 0)
				{
					// select 4 > no blink > blink
					S2_BlinkTimer = 0;
					S2_BlinkFlag = 1;
				}
				else if(S2_BlinkFlag != 0)
				{
					// blink > edit G value down count , fast down count stop ,longkey check stop
					if(cUserAUX24Data.G_Value > S2_AUX4_FlowRate_Min)
						cUserAUX24Data.G_Value--;
					
					S2_AUX4a_FlowRate_Update(cUserAUX24Data.G_Value);

					// long key check stop
					S2_1SecCNT = 100;
					S2_FastFlag = 0;
					S2_FastSource = 0;

					S2_SecTimer = gMainTimerCnt_10m;	
				}
			}
		break;

		case 9:					// H value down btn
			if(S2_Select_Value == 5)
			{
				if(S2_BlinkFlag == 0)
				{
					// select 5 > no blink > blink
					S2_BlinkTimer = 0;
					S2_BlinkFlag = 1;
				}
				else if(S2_BlinkFlag != 0)
				{
					// blink > edit H value down count , fast down count stop ,longkey check stop
					if(cUserAUX24Data.H_Value > S2_AUX4_FlowRate_Min)
						cUserAUX24Data.H_Value--;
					
					S2_AUX4b_FlowRate_Update(cUserAUX24Data.H_Value);

					// long key check stop
					S2_1SecCNT = 100;
					S2_FastFlag = 0;
					S2_FastSource = 0;

					S2_SecTimer = gMainTimerCnt_10m;	
				}
			}
		break;
		
		case 11:
			if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
			{
				if(S2_Select_Value == 6)
				{
					if(S2_BlinkFlag == 0)
					{
						// select 6 > no blink > blink
						S2_BlinkTimer = 0;
						S2_BlinkFlag = 1;
					}
					else if(S2_BlinkFlag != 0)
					{
						// blink > edit P4 value down count , fast down count stop ,longkey check stop
						if(cUserAUX24Data.P4_Value == S2_AUX4_Pressure_Max)
						{
							if((S2_AUX4_Pressure_Max%2)==1)
								cUserAUX24Data.P4_Value -= 1;
							else
								cUserAUX24Data.P4_Value -= 2;
						}
						else if(cUserAUX24Data.P4_Value >= (S2_AUX4_Pressure_Min+2))
							cUserAUX24Data.P4_Value -= 2;
						else if(cUserAUX24Data.P4_Value < (S2_AUX4_Pressure_Min+2))
							cUserAUX24Data.P4_Value = S2_AUX4_Pressure_Min;
    
						S2_AUX4_Pressure_Update(cUserAUX24Data.P4_Value);
    
						// long key check stop
						S2_1SecCNT = 100;
						S2_FastFlag = 0;
						S2_FastSource = 0;
    
						S2_SecTimer = gMainTimerCnt_10m;	
					}
				}
			}
		break;
	}

    return true;
}

bool M7S2_UserDisplay(void)
{
	uint8_t tDestName[100];

	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
		return 0;
	
	if (M7S2_UserDisplay_Data!=User_RegInfo[M7_UserIndex].RegFlag)
	{	
		if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
		{
			ituWidgetSetVisible(M7S2_TopStandard,false);
			ituWidgetSetVisible(M7S2_TopAdmin,true);
		}
		else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
		{
			ituWidgetSetVisible(M7S2_TopAdmin,false);
			ituWidgetSetVisible(M7S2_TopStandard,true);
		}
		else
		{
			ituWidgetSetVisible(M7S2_TopAdmin,false);
			ituWidgetSetVisible(M7S2_TopStandard,false);
		}
		M7S2_UserDisplay_Data=User_RegInfo[M7_UserIndex].RegFlag;
	}

	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M7S2_TopUserName, tDestName);

		ituWidgetSetVisible(M7S2_TopUserName,true);
	}
	else
	{
		ituWidgetSetVisible(M7S2_TopUserName,false);
	}
}

void S2_Value_Display(uint8_t tSelect_Value)
{
//printf("S2_Value_Display = [%d][%d][%d]\n",M7_P1_Timer,S2_Select_Value,S2_BlinkFlag);
	switch(tSelect_Value)
	{
		case 1:
			ituWidgetSetVisible(S2_C_Value, true);
		break;

		case 2:
			ituWidgetSetVisible(S2_D_Value, true);
		break;

		case 3:
			ituWidgetSetVisible(S2_P2_Value, true);
		break;

		case 4:
			ituWidgetSetVisible(S2_G_Value, true);
		break;

		case 5:
			ituWidgetSetVisible(S2_H_Value, true);
		break;

		case 6:
			ituWidgetSetVisible(S2_P4_Value, true);
		break;
	}
}			
void S2_ValueVisable(void)
{

printf("S2_ValueVisable = [%d][%d][%d]\n",M7_P1_Timer,S2_Select_Value,S2_BlinkFlag);
	if(ituWidgetIsVisible(S2_C_Value) == false)
		ituWidgetSetVisible(S2_C_Value, true);

	if(ituWidgetIsVisible(S2_D_Value) == false)
		ituWidgetSetVisible(S2_D_Value, true);

	ituWidgetSetVisible(S2_P2_Value, true);

	if(ituWidgetIsVisible(S2_G_Value) == false)
		ituWidgetSetVisible(S2_G_Value, true);
	
	if(ituWidgetIsVisible(S2_H_Value) == false)
		ituWidgetSetVisible(S2_H_Value, true);

	ituWidgetSetVisible(S2_P4_Value, true);
}

void S2_SelectClear(void)
{
	if(ituWidgetIsVisible(S2_C_Sel) == true)
		ituWidgetSetVisible(S2_C_Sel, false);

	if(ituWidgetIsVisible(S2_D_Sel) == true)
		ituWidgetSetVisible(S2_D_Sel, false);

	if(ituWidgetIsVisible(S2_P2_Sel) == true)
		ituWidgetSetVisible(S2_P2_Sel, false);

	if(ituWidgetIsVisible(S2_G_Sel) == true)
		ituWidgetSetVisible(S2_G_Sel, false);

	if(ituWidgetIsVisible(S2_H_Sel) == true)
		ituWidgetSetVisible(S2_H_Sel, false);

	if(ituWidgetIsVisible(S2_P4_Sel) == true)
		ituWidgetSetVisible(S2_P4_Sel, false);
}

void S2_Select_Display(uint8_t tSelectValue)
{
	switch(tSelectValue)
	{
		case 1:
			ituWidgetSetVisible(S2_C_Sel, true);
		break;
		
		case 2:
			ituWidgetSetVisible(S2_D_Sel, true);
		break;
		
		case 3:
			ituWidgetSetVisible(S2_P2_Sel, true);
		break;

		case 4:
			ituWidgetSetVisible(S2_G_Sel, true);
		break;

		case 5:
			ituWidgetSetVisible(S2_H_Sel, true);
		break;

		case 6:
			ituWidgetSetVisible(S2_P4_Sel, true);
		break;
	}
}

void S2_UpDn_Enable(void)
{
	// AUX2 C Flowrate
	ituWidgetSetVisible(S2_1, true);
	ituWidgetSetVisible(S2_2, true);

	// AUX2 D Flowrate
	ituWidgetSetVisible(S2_3, true);
	ituWidgetSetVisible(S2_4, true);

	// AUX2 Pressure
	if(((M7_P1_Timer+TIME_CNT_2_5SEC)<gMainTimerCnt_10m)||(M7_P1_Timer==0))
	{
		ituWidgetSetVisible(S2_11, false);
		ituWidgetSetVisible(S2_12, false);
		ituWidgetSetVisible(S2_11_D, true);
		ituWidgetSetVisible(S2_12_D, true);
		M7_P1_Timer=0;
	}
	else if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
	{
		ituWidgetSetVisible(S2_5_D, false);
		ituWidgetSetVisible(S2_6_D, false);
		ituWidgetSetVisible(S2_5, true);
		ituWidgetSetVisible(S2_6, true);
	}
	else if((M7_P2_OPI_Flag == 0)||(M7_P2_OPI_Flag == 3))
	{
		ituWidgetSetVisible(S2_5, false);
		ituWidgetSetVisible(S2_6, false);
		ituWidgetSetVisible(S2_5_D, true);
		ituWidgetSetVisible(S2_6_D, true);
	}
	
	// AUX4 G Flowrate
	ituWidgetSetVisible(S2_7, true);
	ituWidgetSetVisible(S2_8, true);

	// AUX4 H Flowrate
	ituWidgetSetVisible(S2_9, true);
	ituWidgetSetVisible(S2_10, true);

	// AUX4 Pressure
	if(((M7_P1_Timer+TIME_CNT_2_5SEC)<gMainTimerCnt_10m)||(M7_P1_Timer==0))
	{
		ituWidgetSetVisible(S2_11, false);
		ituWidgetSetVisible(S2_12, false);
		ituWidgetSetVisible(S2_11_D, true);
		ituWidgetSetVisible(S2_12_D, true);
		M7_P1_Timer=0;
	}
	else if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
	{
		ituWidgetSetVisible(S2_11_D, false);
		ituWidgetSetVisible(S2_12_D, false);
		ituWidgetSetVisible(S2_11, true);
		ituWidgetSetVisible(S2_12, true);
	}
	else
	{
		ituWidgetSetVisible(S2_11, false);
		ituWidgetSetVisible(S2_12, false);
		ituWidgetSetVisible(S2_11_D, true);
		ituWidgetSetVisible(S2_12_D, true);
	}
	
	if(S2_BlinkFlag != 0)
	{
		S2_ValueVisable();
		S2_BlinkFlag = 0;
	}

}

void S2_UpDn_Disable(void)
{
	// AUX2 C Flowrate
	ituWidgetSetVisible(S2_1, false);
	ituWidgetSetVisible(S2_2, false);

	// AUX2 D Flowrate
	ituWidgetSetVisible(S2_3, false);
	ituWidgetSetVisible(S2_4, false);

	// AUX2 Pressure
	ituWidgetSetVisible(S2_5, false);
	ituWidgetSetVisible(S2_6, false);
	ituWidgetSetVisible(S2_5_D, false);
	ituWidgetSetVisible(S2_6_D, false);
	
	// AUX4 G Flowrate
	ituWidgetSetVisible(S2_7, false);
	ituWidgetSetVisible(S2_8, false);

	// AUX4 H Flowrate
	ituWidgetSetVisible(S2_9, false);
	ituWidgetSetVisible(S2_10, false);

	// AUX4 Pressure
	ituWidgetSetVisible(S2_11, false);
	ituWidgetSetVisible(S2_12, false);
	ituWidgetSetVisible(S2_11_D, false);
	ituWidgetSetVisible(S2_12_D, false);
}

void S2_AUX2a_FlowRate_Update(uint8_t tC_Value)
{
	S2_AUX2a_Value_Update(tC_Value);

	ituWidgetSetVisible(S2_C_Value, true);
	ituWidgetSetVisible(S2_F1_Value, true);
}

void S2_AUX2a_Value_Update(uint8_t tC_Value)
{
	char ptAUX2Value[7];
	uint8_t tCalFlowRate_SI;
	uint16_t tCalFlowRate_US;

	memset(ptAUX2Value,0,7);
	sprintf(ptAUX2Value,"%3d",tC_Value);
	ituTextSetString(S2_C_Value, ptAUX2Value);
//	ituWidgetSetVisible(S2_C_Value, true);

	memset(ptAUX2Value,0,7);
	tCalFlowRate_SI = Cal_2Way_FlowRate(tC_Value);

	if(SIUS_SEL_Flag == 0)	//si
	{
		sprintf(ptAUX2Value,"%4d ",tCalFlowRate_SI);
		ituTextSetString(S2_F1_Value, ptAUX2Value);
//		ituWidgetSetVisible(S2_F1_Value, true);
	}
	else if(SIUS_SEL_Flag == 1)		// us
	{
		tCalFlowRate_US = Change_Lm2gpm((uint16_t)tCalFlowRate_SI);
		sprintf(ptAUX2Value,"%4d ",tCalFlowRate_US);
		ituTextSetString(S2_F1_Value, ptAUX2Value);
//		ituWidgetSetVisible(S2_F1_Value, true);
	}
}

void S2_AUX2b_FlowRate_Update(uint8_t tD_Value)
{
	S2_AUX2b_Value_Update(tD_Value);

	ituWidgetSetVisible(S2_D_Value, true);
	ituWidgetSetVisible(S2_F2_Value, true);
}

void S2_AUX2b_Value_Update(uint8_t tD_Value)
{
	char ptAUX2Value[7];
	uint8_t tCalFlowRate_SI;
	uint16_t tCalFlowRate_US;

	memset(ptAUX2Value,0,7);
	sprintf(ptAUX2Value,"%3d",tD_Value);
	ituTextSetString(S2_D_Value, ptAUX2Value);
//	ituWidgetSetVisible(S2_D_Value, true);

	memset(ptAUX2Value,0,7);
	tCalFlowRate_SI = Cal_2Way_FlowRate(tD_Value);

	if(SIUS_SEL_Flag == 0)	//si
	{
		sprintf(ptAUX2Value,"%4d ",tCalFlowRate_SI);
		ituTextSetString(S2_F2_Value, ptAUX2Value);
//		ituWidgetSetVisible(S2_F2_Value, true);
	}
	else if(SIUS_SEL_Flag == 1)		// us
	{
		tCalFlowRate_US = Change_Lm2gpm((uint16_t)tCalFlowRate_SI);
		sprintf(ptAUX2Value,"%4d ",tCalFlowRate_US);
		ituTextSetString(S2_F2_Value, ptAUX2Value);
//		ituWidgetSetVisible(S2_F2_Value, true);
	}
}

uint8_t Cal_2Way_FlowRate(uint8_t t1way_X)
{
	uint8_t i,t1Way_Y=0;
	double X_Delta=0,Y_Delta=0;
	double Y_Div=0;
	
	for(i=0;i<6;i++)
	{
		if(t1way_X < S1_2WAY_X[0] )
		{
			t1Way_Y = S1_2WAY_Y[0];
		}
		else if(t1way_X >= S1_2WAY_X[5])
		{
			t1Way_Y = S1_2WAY_Y[5];
		}
		else if((t1way_X >= S1_2WAY_X[i]) && (t1way_X < S1_2WAY_X[i+1]))
		{
			X_Delta = (double)(S1_2WAY_X[i+1] - S1_2WAY_X[i]);
			Y_Delta = (double)(S1_2WAY_Y[i+1] - S1_2WAY_Y[i]);
			
			Y_Div = (double)(Y_Delta*(t1way_X - S1_2WAY_X[i]));

			//t1Way_Y = (uint8_t)round(Y_Div/X_Delta)+S1_2WAY_Y[i];
			t1Way_Y = (uint8_t)floor(Y_Div/X_Delta)+S1_2WAY_Y[i];//chpark.24.04.19
			break;
		}
	}

	return t1Way_Y;
}

void S2_AUX2_Pressure_Update(uint8_t tP_Value)
{
	S2_AUX2_P2_Value_Update(tP_Value);
}

void S2_AUX2_P2_Value_Update(uint8_t tP_Value)
{
	char ptAUX2Value[7];
	static uint8_t tP2_OPI_Flag = 0;

	
	memset(ptAUX2Value,0,7);
	
	if((M7_P2_OPI_Flag == 0)||(M7_P2_OPI_Flag == 3))
	{
		sprintf(ptAUX2Value,"N/A");

		ituTextSetString(S2_P2_NA, ptAUX2Value);
		
		if(ituWidgetIsVisible(S2_P2_Sel) == true)
			ituWidgetSetVisible(S2_P2_Sel, false);

		if((S2_BlinkFlag != 0)&&(S2_Select_Value == 3))
		{
			// blink > long key check start
			S2_SecTimer = gMainTimerCnt_10m;
			S2_1SecCNT = 0;
			S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

			S2_FastSource = 0;
//			S2_Select_Value = 0;
			S2_BlinkFlag = 0;
			tP2_OPI_Flag = 1;
		}
		ituWidgetSetVisible(S2_P2_Value, false);
		ituWidgetSetVisible(S2_P2_NA, true);
	}
	else if((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))
	{
		if(((M7_P1_Timer+TIME_CNT_2_5SEC)<gMainTimerCnt_10m)||(M7_P1_Timer==0))
		{
			sprintf(ptAUX2Value,"N/A");
			ituTextSetString(S2_P2_NA, ptAUX2Value);
			ituWidgetSetVisible(S2_P2_Value, false);
			ituWidgetSetVisible(S2_P2_NA, true);
			M7_P1_Timer=0;
			if((S2_BlinkFlag != 0)&&(S2_Select_Value == 3))
			{
				// blink > long key check start
				S2_SecTimer = gMainTimerCnt_10m;
				S2_1SecCNT = 0;
				S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
	
				S2_FastSource = 0;
//				S2_Select_Value = 0;
				S2_BlinkFlag = 0;
				tP2_OPI_Flag = 1;
			}		
		}
		else
		{
			ituWidgetSetVisible(S2_P2_NA, false);
			sprintf(ptAUX2Value,"%4d",tP_Value);
			ituTextSetString(S2_P2_Value, ptAUX2Value);
			if((S2_BlinkFlag == 0)||(S2_Select_Value != 3))
			{
//				printf("S2_AUX2_P2_Value_Update = [%d][%d][%d]\n",S2_Select_Value,S2_Select_Value,S2_BlinkFlag);
				ituWidgetSetVisible(S2_P2_Value, true);
			}
			tP2_OPI_Flag = 0;
		}
	}
}


void S2_AUX4a_FlowRate_Update(uint8_t tG_Value)
{
	S2_AUX4a_Value_Update(tG_Value);
	
	ituWidgetSetVisible(S2_G_Value, true);
	ituWidgetSetVisible(S2_F3_Value, true);
}

void S2_AUX4a_Value_Update(uint8_t tG_Value)
{
	char ptAUX2Value[7];
	uint8_t tCalFlowRate_SI;
	uint16_t tCalFlowRate_US;

	memset(ptAUX2Value,0,7);
	sprintf(ptAUX2Value,"%3d",tG_Value);
	ituTextSetString(S2_G_Value, ptAUX2Value);
//	ituWidgetSetVisible(S2_G_Value, true);

	memset(ptAUX2Value,0,7);
	tCalFlowRate_SI = Cal_4Way_FlowRate(tG_Value);

	if(SIUS_SEL_Flag == 0)	//si
	{
		sprintf(ptAUX2Value,"%4d ",tCalFlowRate_SI);
		ituTextSetString(S2_F3_Value, ptAUX2Value);
//		ituWidgetSetVisible(S2_F3_Value, true);
	}
	else if(SIUS_SEL_Flag == 1)		// us
	{
		tCalFlowRate_US = Change_Lm2gpm((uint16_t)tCalFlowRate_SI);
		sprintf(ptAUX2Value,"%4d ",tCalFlowRate_US);
		ituTextSetString(S2_F3_Value, ptAUX2Value);
//		ituWidgetSetVisible(S2_F3_Value, true);
	}
}

void S2_AUX4b_FlowRate_Update(uint8_t tH_Value)
{
	S2_AUX4b_Value_Update(tH_Value);
	
	ituWidgetSetVisible(S2_H_Value, true);
	ituWidgetSetVisible(S2_F4_Value, true);
}

void S2_AUX4b_Value_Update(uint8_t tH_Value)
{
	char ptAUX2Value[7];
	uint8_t tCalFlowRate_SI;
	uint16_t tCalFlowRate_US;

	memset(ptAUX2Value,0,7);
	sprintf(ptAUX2Value,"%3d",tH_Value);
	ituTextSetString(S2_H_Value, ptAUX2Value);
//	ituWidgetSetVisible(S2_H_Value, true);

	memset(ptAUX2Value,0,7);
	tCalFlowRate_SI = Cal_4Way_FlowRate(tH_Value);

	if(SIUS_SEL_Flag == 0)	//si
	{
		sprintf(ptAUX2Value,"%4d ",tCalFlowRate_SI);
		ituTextSetString(S2_F4_Value, ptAUX2Value);
//		ituWidgetSetVisible(S2_F4_Value, true);
	}
	else if(SIUS_SEL_Flag == 1)		// us
	{
		tCalFlowRate_US = Change_Lm2gpm((uint16_t)tCalFlowRate_SI);
		sprintf(ptAUX2Value,"%4d ",tCalFlowRate_US);
		ituTextSetString(S2_F4_Value, ptAUX2Value);
//		ituWidgetSetVisible(S2_F4_Value, true);
	}
}

uint8_t Cal_4Way_FlowRate(uint8_t t1way_X)
{
	uint8_t i,t1Way_Y=0;
	double X_Delta=0,Y_Delta=0;
	double Y_Div=0;
	
	for(i=0;i<6;i++)
	{
		if(t1way_X < S1_4WAY_X[0] )
		{
			t1Way_Y = S1_4WAY_Y[0];
		}
		else if(t1way_X >= S1_4WAY_X[5])
		{
			t1Way_Y = S1_4WAY_Y[5];
		}
		else if((t1way_X >= S1_4WAY_X[i]) && (t1way_X < S1_4WAY_X[i+1]))
		{
			X_Delta = (double)(S1_4WAY_X[i+1] - S1_4WAY_X[i]);
			Y_Delta = (double)(S1_4WAY_Y[i+1] - S1_4WAY_Y[i]);
			
			Y_Div = (double)(Y_Delta*(t1way_X - S1_4WAY_X[i]));

			//t1Way_Y = (uint8_t)round(Y_Div/X_Delta)+S1_4WAY_Y[i];
			t1Way_Y = (uint8_t)floor(Y_Div/X_Delta)+S1_4WAY_Y[i];//chpark.24.04.19

			break;
		}
	}

	return t1Way_Y;
}

void S2_AUX4_Pressure_Update(uint8_t tP_Value)
{
	S2_AUX4_P4_Value_Update(tP_Value);

}

void S2_AUX4_P4_Value_Update(uint8_t tP_Value)
{

	char ptAUX4Value[7];
	static uint8_t tP4_OPI_Flag = 0;
	
	memset(ptAUX4Value,0,7);

	if(((M7_P2_OPI_Flag == 1)||(M7_P2_OPI_Flag == 2))&&((M7_P4_OPI_Flag == 1)||(M7_P4_OPI_Flag == 2))&&((M7_P24_OPI_Flag == 0)||(M7_P24_OPI_Flag == 3)))
	{
		if(((M7_P1_Timer+TIME_CNT_2_5SEC)<gMainTimerCnt_10m)||(M7_P1_Timer==0))
		{
			sprintf(ptAUX4Value,"N/A");
			ituTextSetString(S2_P4_NA, ptAUX4Value);
			ituWidgetSetVisible(S2_P4_Value, false);
			ituWidgetSetVisible(S2_P4_NA, true);
			M7_P1_Timer=0;
			if((S2_BlinkFlag != 0)&&(S2_Select_Value == 6))
			{
				// blink > long key check start
				S2_SecTimer = gMainTimerCnt_10m;
				S2_1SecCNT = 0;
				S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast
	
				S2_BlinkFlag = 0;
	
				S2_FastSource = 0;
//				S2_Select_Value = 0;
	
				tP4_OPI_Flag = 1;
			}
		}
		else
		{
			sprintf(ptAUX4Value,"%4d",tP_Value);
			ituTextSetString(S2_P4_Value, ptAUX4Value);
			ituWidgetSetVisible(S2_P4_NA, false);
			if(((S2_BlinkFlag == 0)||(S2_Select_Value != 6)))
			{
		//		printf("S2_AUX4_P4_Value_Update = [%d][%d][%d]\n",S2_Select_Value,S2_Select_Value,S2_BlinkFlag);
				ituWidgetSetVisible(S2_P4_Value, true);
			}
			tP4_OPI_Flag = 0;
		}

		//ituTextSetString(S2_P4_Value, ptAUX4Value);
	}
	//else if(((M7_P2_OPI_Flag == 0)||(M7_P2_OPI_Flag == 3))&&((M7_P4_OPI_Flag == 0)||(M7_P4_OPI_Flag == 3))&&((M7_P24_OPI_Flag == 1)||(M7_P24_OPI_Flag == 2)))
	else
	{
		if(ituWidgetIsVisible(S2_P4_Sel) == true)
			ituWidgetSetVisible(S2_P4_Sel, false);

		sprintf(ptAUX4Value,"N/A");

		if((S2_BlinkFlag != 0)&&(S2_Select_Value == 6))
		{
			// blink > long key check start
			S2_SecTimer = gMainTimerCnt_10m;
			S2_1SecCNT = 0;
			S2_FastFlag = 1;		// 0:No Actiion , 1:start condition , 2:Fast

			S2_BlinkFlag = 0;

			S2_FastSource = 0;
//			S2_Select_Value = 0;

			tP4_OPI_Flag = 1;
		}

		ituTextSetString(S2_P4_NA, ptAUX4Value);
		ituWidgetSetVisible(S2_P4_Value, false);
		ituWidgetSetVisible(S2_P4_NA, true);
	}
}

void AUX2_4_LimitCheck(void)
{
//	cUserAUX24Data = M7_UserAUX24Set;

	if(cUserAUX24Data.C_Value > S2_AUX2_FlowRate_Max)
		cUserAUX24Data.C_Value = S2_AUX2_FlowRate_Max;
	else if(cUserAUX24Data.C_Value < S2_AUX2_FlowRate_Min)
		cUserAUX24Data.C_Value = S2_AUX2_FlowRate_Min;

	if(cUserAUX24Data.D_Value > S2_AUX2_FlowRate_Max)
		cUserAUX24Data.D_Value = S2_AUX2_FlowRate_Max;
	else if(cUserAUX24Data.D_Value < S2_AUX2_FlowRate_Min)
		cUserAUX24Data.D_Value = S2_AUX2_FlowRate_Min;

	if(cUserAUX24Data.P2_Value > S2_AUX2_Pressure_Max)
		cUserAUX24Data.P2_Value = S2_AUX2_Pressure_Max;
	else if(cUserAUX24Data.P2_Value < S2_AUX2_Pressure_Min)
		cUserAUX24Data.P2_Value = S2_AUX2_Pressure_Min;

	if(cUserAUX24Data.G_Value > S2_AUX4_FlowRate_Max)
		cUserAUX24Data.G_Value = S2_AUX4_FlowRate_Max;
	else if(cUserAUX24Data.G_Value < S2_AUX4_FlowRate_Min)
		cUserAUX24Data.G_Value = S2_AUX4_FlowRate_Min;

	if(cUserAUX24Data.H_Value > S2_AUX4_FlowRate_Max)
		cUserAUX24Data.H_Value = S2_AUX4_FlowRate_Max;
	else if(cUserAUX24Data.H_Value < S2_AUX4_FlowRate_Min)
		cUserAUX24Data.H_Value = S2_AUX4_FlowRate_Min;

	if(cUserAUX24Data.P4_Value > S2_AUX4_Pressure_Max)
		cUserAUX24Data.P4_Value = S2_AUX4_Pressure_Max;
	else if(cUserAUX24Data.P4_Value < S2_AUX4_Pressure_Min)
		cUserAUX24Data.P4_Value = S2_AUX4_Pressure_Min;

	//M7_UserAUX24Set = cUserAUX24Data;
}

