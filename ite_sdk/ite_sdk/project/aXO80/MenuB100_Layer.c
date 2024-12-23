#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h" 

#define MB100_ENG1				0
#define MB100_ENG2				1
#define MB100_ENG3				2
#define MB100_ENG4				3
#define MB100_END				4

static uint32_t MB100_gMT_Timer;
static uint8_t  MB100_gMT_ExitTimer = 0;			// Sec Count

bool MB100_MenuTabNext();
bool MB100_MenuTabPrev();

static ITUContainer* MB100_CT[4];
static int8_t nMenuB100_MenuIndex=0;
//=========================================

void MenuB100_UpdateMenuUI(uint8_t nIndex);
void MenuB100_GetMenuData(uint8_t nIndex);
void MenuB100_SetMenuTab(uint8_t nIndex);

void MenuB100_InitControl();

void MenuB100_SetLang(uint8_t nIndex);
void MenuB100_SetUnit(uint8_t nIndex);

bool MenuB100_34BMeterDisp(void);//chpark.08.28
//=======================================
static char sMB100_Name[30];

static ITUText* MB100_C1_Text[17]; //
static ITUText* MB100_C2_Text[12]; //
static ITUText* MB100_C3_Text[9];  //
static ITUText* MB100_C4_Text[5];  //

static ITUWidget* MB100_C1_U[2]; //
static ITUWidget* MB100_C2_U[2]; //
static ITUWidget* MB100_C3_U[2]; //
static ITUWidget* MB100_C4_U[2]; //

static ITUWidget* MB100_C1_LANG[5]; //
static ITUWidget* MB100_C2_LANG[5]; //
static ITUWidget* MB100_C3_LANG[5]; //
static ITUWidget* MB100_C4_LANG[5]; //
//===================================================================
//MB100 Engine Info1
uint8_t     MB100_C1_AP;        //(1)Accel position
uint16_t    MB100_C1_OS;        //(2)Operating speed
uint8_t     MB100_C1_TO;        //(3)Torque
int32_t     MB100_C1_OP[2];     //(4)Oil pressure
int32_t     MB100_C1_HOT[2];    //(5)Hydraulic oil temp //int32_t     MB100_C1_OT[2];     //(5)Oil temp

int32_t     MB100_C1_CT[2];     //(6)Coolant temp
float       MB100_C1_FR[2];     //(7)Fuel rate
int32_t     MB100_C1_FT[2];     //(8)Fuel temp
int32_t     MB100_C1_BP[2];     //(9)Barometric pressure
int32_t     MB100_C1_AAT[2];    //(10)Ambient air temp

int32_t     MB100_C1_EAIP[2];   //(11)Engine air inlet pressure //int32_t     MB100_C1_BOP[2];  //(12)Boost pressure
int32_t     MB100_C1_BOT[2];    //(12)Boost temp
float       MB100_C1_CRP[2];    //(13)Common-rail pressure
float       MB100_C1_ETA;       //(14)Engine throttle actuator
float       MB100_C1_DEF_DP[2]; //(15)EGR differential pressure

int32_t     MB100_C1_DEF_T[2];  //(16)EGR temp    

//MB100 Engine Info2
int32_t     MB100_C2_DPF_EDP[2];    //(1)DPF exhaust diff. pressure
int32_t     MB100_C2_DPF_IGT[2];    //(2)DPF inlet gas temp
int32_t     MB100_C2_DPF_IT[2];     //(3)DPF intermediate temp
int32_t     MB100_C2_DPF_OGT[2];    //(4)DPF outlet gas temp
float       MB100_C2_DPF_TSL;       //(5)DPF time since last(sec)

int32_t     MB100_C2_SCR_IGT[2];    //(6)SCR inlet gas temp
uint16_t    MB100_C2_DPF_SLP;       //(7)DPF soot load percent
uint16_t    MB100_C2_DPF_ALP;       //(8)DPF ash load percent
uint16_t    MB100_C2_DEF_TL;        //(9)DEF tank level
int32_t     MB100_C2_DEF_TT[2];     //(10)DEF tank temp

int32_t     MB100_C2_DEF_AQ[2];     //(11)DEF actual quantity
int32_t     MB100_C2_DEF_AP[2];     //(12)DEF absolute pressure

//MB100 Engine Info3
int16_t     MB100_C3_AUX1;  //(1)AUX1 prop.
int16_t     MB100_C3_AUX2;  //(2)AUX2 prop.
int16_t     MB100_C3_AUX4;  //(3)AUX4 prop.
int16_t     MB100_C3_BL;    //(4)Blade lever
int16_t     MB100_C3_SW;    //(5)Swing

int16_t     MB100_C3_AD;    //(6)Adjust
float       MB100_C3_LAS[2];//(7)Lift alarm sensor
//float       MB100_C3_AC[2]; //(8)Adjust crowd
//float       MB100_C3_ADD[2];//(9)Adjust dump

//MB100 Engine Info4
char        MB100_C4_MC_Version[10] = {'\0'};//(1)Main controller
uint16_t    MB100_C4_DS;    //(2)Display soft

float       MB100_C4_SPT[2];//(3)Set point temp
float       MB100_C4_CPT[2];//(4)Cabin point temp
float       MB100_C4_ET[2]; //(5)Evaporator temp
//===================================================================

bool MB100_m_b_ReadyToStart = false;

bool MenuB100_Enter(ITUWidget* widget, char* param)
{
    //printf("\r\nMenuB100_Enter--Unit::%d Lang:%d\r\n",gMB_Unit,gMB_Language);
    MB100_m_b_ReadyToStart = false;

	MB100_gMT_ExitTimer = 0;
    
	MB100_gMT_Timer = gMainTimerCnt_10m;

    MenuB100_InitControl();
    MenuB100_SetMenuTab(0);

    MB100_m_b_ReadyToStart = true;

    return true;
}

void MenuB100_InitControl()
{
    MB100_CT[0] = (ITUContainer*)ituSceneFindWidget(&theScene, "MenuB100_C1");
    MB100_CT[1] = (ITUContainer*)ituSceneFindWidget(&theScene, "MenuB100_C2");
    MB100_CT[2] = (ITUContainer*)ituSceneFindWidget(&theScene, "MenuB100_C3");
    MB100_CT[3] = (ITUContainer*)ituSceneFindWidget(&theScene, "MenuB100_C4");
   
    MB100_C1_Text[0] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T1");
    MB100_C1_Text[1] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T2");
    MB100_C1_Text[2] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T3");
    MB100_C1_Text[3] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T4");
    MB100_C1_Text[4] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T5");
    MB100_C1_Text[5] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T6");
    MB100_C1_Text[6] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T7");
    MB100_C1_Text[7] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T8");
    MB100_C1_Text[8] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T9");
    MB100_C1_Text[9] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T10");
    MB100_C1_Text[10] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T11");
    MB100_C1_Text[11] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T12");
    MB100_C1_Text[12] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T13");
    MB100_C1_Text[13] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T14");
    MB100_C1_Text[14] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T15");
    MB100_C1_Text[15] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T16");
    //MB100_C1_Text[16] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C1_T17");

    for(int i=0;i<16;i++)
    {
        ituWidgetSetVisible(MB100_C1_Text[i], true);
    }
         
    MB100_C2_Text[0] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C2_T1");
    MB100_C2_Text[1] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C2_T2");
    MB100_C2_Text[2] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C2_T3");
    MB100_C2_Text[3] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C2_T4");
    MB100_C2_Text[4] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C2_T5");
    MB100_C2_Text[5] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C2_T6");
    MB100_C2_Text[6] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C2_T7");
    MB100_C2_Text[7] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C2_T8");
    MB100_C2_Text[8] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C2_T9");
    MB100_C2_Text[9] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C2_T10");
    MB100_C2_Text[10] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C2_T11");
    MB100_C2_Text[11] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C2_T12");

    for(int i=0;i<12;i++)
    {
        ituWidgetSetVisible(MB100_C2_Text[i], true);
    }

    MB100_C3_Text[0] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C3_T1");
    MB100_C3_Text[1] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C3_T2");
    MB100_C3_Text[2] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C3_T3");
    MB100_C3_Text[3] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C3_T4");
    MB100_C3_Text[4] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C3_T5");
    MB100_C3_Text[5] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C3_T6");
    MB100_C3_Text[6] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C3_T7");
    //MB100_C3_Text[7] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C3_T8");
    //MB100_C3_Text[8] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C3_T9");

    for(int i=0;i<7;i++)
    {
        ituWidgetSetVisible(MB100_C3_Text[i], true);
    }

    MB100_C4_Text[0] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C4_T1");
    MB100_C4_Text[1] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C4_T2");
    MB100_C4_Text[2] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C4_T3");
    MB100_C4_Text[3] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C4_T4");
    MB100_C4_Text[4] = (ITUText*)ituSceneFindWidget(&theScene, "MenuB100_C4_T5");

    for(int i=0;i<5;i++)
    {
        ituWidgetSetVisible(MB100_C4_Text[i], true);
    }

    MB100_C1_U[0] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C1_U1");
    MB100_C1_U[1] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C1_U2");

    MB100_C2_U[0] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C2_U1");
    MB100_C2_U[1] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C2_U2");

    MB100_C3_U[0] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C3_U1");
    MB100_C3_U[1] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C3_U2");

    MB100_C4_U[0] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C4_U1");
    MB100_C4_U[1] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C4_U2");

    MB100_C1_LANG[0] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C1_LANG1");
    MB100_C1_LANG[1] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C1_LANG2");
    MB100_C1_LANG[2] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C1_LANG3");
    MB100_C1_LANG[3] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C1_LANG4");
    MB100_C1_LANG[4] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C1_LANG5");

    MB100_C2_LANG[0] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C2_LANG1");
    MB100_C2_LANG[1] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C2_LANG2");
    MB100_C2_LANG[2] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C2_LANG3");
    MB100_C2_LANG[3] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C2_LANG4");
    MB100_C2_LANG[4] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C2_LANG5");

    MB100_C3_LANG[0] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C3_LANG1");
    MB100_C3_LANG[1] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C3_LANG2");
    MB100_C3_LANG[2] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C3_LANG3");
    MB100_C3_LANG[3] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C3_LANG4");
    MB100_C3_LANG[4] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C3_LANG5");

    MB100_C4_LANG[0] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C4_LANG1");
    MB100_C4_LANG[1] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C4_LANG2");
    MB100_C4_LANG[2] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C4_LANG3");
    MB100_C4_LANG[3] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C4_LANG4");
    MB100_C4_LANG[4] = (ITUIcon*)ituSceneFindWidget(&theScene, "MenuB100_C4_LANG5");
}

void MenuB100_SetLang(uint8_t nIndex)
{
    for(int i=0;i<5;i++)
    {
        ituWidgetSetVisible(MB100_C1_LANG[i],false);
        ituWidgetSetVisible(MB100_C2_LANG[i],false);
        ituWidgetSetVisible(MB100_C3_LANG[i],false);
        ituWidgetSetVisible(MB100_C4_LANG[i],false);
    }

    if(nMenuB100_MenuIndex==0)
    {
        ituWidgetSetVisible(MB100_C1_LANG[nIndex],true);
    }
    else if(nMenuB100_MenuIndex==1)
    {
        ituWidgetSetVisible(MB100_C2_LANG[nIndex],true);
    }
    else if(nMenuB100_MenuIndex==2)
    {
        ituWidgetSetVisible(MB100_C3_LANG[nIndex],true);
    }
    else if(nMenuB100_MenuIndex==3)
    {
        ituWidgetSetVisible(MB100_C4_LANG[nIndex],true);
    }
}

void MenuB100_SetUnit(uint8_t nIndex)
{
    for(int i=0;i<2;i++)
    {
        ituWidgetSetVisible(MB100_C1_U[i],false);
        ituWidgetSetVisible(MB100_C2_U[i],false);
        ituWidgetSetVisible(MB100_C3_U[i],false);
        ituWidgetSetVisible(MB100_C4_U[i],false);
    }
    
    if(nMenuB100_MenuIndex==0)
    {
        ituWidgetSetVisible(MB100_C1_U[nIndex],true);
    }
    else if(nMenuB100_MenuIndex==1)
    {
        ituWidgetSetVisible(MB100_C2_U[nIndex],true);
    }
    else if(nMenuB100_MenuIndex==2)
    {
        ituWidgetSetVisible(MB100_C3_U[nIndex],true);
    }
    else if(nMenuB100_MenuIndex==3)
    {
        ituWidgetSetVisible(MB100_C4_U[nIndex],true);
    }
}


void MenuB100_GetMenuData(uint8_t nIndex)
{
    //printf("\r\n MenuB100_GetData---12--%d\r\n",nIndex);
    switch( nIndex)
    {
        case 0:
        {
            MB100_C1_AP = gMB_IO_Value.SI_3Byte;        //(1)Accel position
            MB100_C1_OS = gMB_IO_Value.EEC1_34Bytes;    //(2)Operating speed
            MB100_C1_TO = gMB_IO_Value.EEC2_2Byte;      //(3)Torque
            MB100_C1_OP[0] = gMB_IO_Value.EFL_P1_3Byte; //(4)Oil pressure
            MB100_C1_HOT[0] = gMB_IO_Value.SI_0Byte;    //(5)Hydraulic oil temp//MB100_C1_OT[0] = gMB_IO_Value.ET1_23Bytes;  //(5)Oil temp //chpark.24.10.28
         
            MB100_C1_CT[0] = gMB_IO_Value.ET1_0Byte;    //(6)Coolant temp
            MB100_C1_FR[0] = gMB_IO_Value.LFE_01Bytes;  //(7)Fuel rate
            MB100_C1_FT[0] = gMB_IO_Value.ET1_1Byte;    //(8)Fuel temp
            MB100_C1_BP[0] = gMB_IO_Value.AMB_0Byte;    //(9)Barometric pressure
            MB100_C1_AAT[0] = gMB_IO_Value.AMB_34Bytes;     //(10)Ambient air temp
                        
            MB100_C1_EAIP[0] = gMB_IO_Value.IC1_3Byte;      //(11)Engine air inlet pressure //MB100_C1_BOP[0] = gMB_IO_Value.IC1_1Byte;     //(11)Boost pressure
            MB100_C1_BOT[0]  = gMB_IO_Value.IC1_2Byte;      //(12)Boost temp
            MB100_C1_CRP[0] = gMB_IO_Value.EFL_P2_23Bytes;  //(13)Common-rail pressure
            MB100_C1_ETA    = gMB_IO_Value.TFAC_01Bytes;    //(14)Engine throttle actuator
            MB100_C1_DEF_DP[0]  = gMB_IO_Value.ET2_45Bytes; //(15)EGR differential pressure

            MB100_C1_DEF_T[0]   = gMB_IO_Value.ET2_67Bytes; //(16)EGR temp        
        }
        break;

        case 1:
        {
            MB100_C2_DPF_EDP[0] = gMB_IO_Value.AT1IMG_45Bytes; //(1)DPF exhaust diff. pressure
            MB100_C2_DPF_IGT[0] = gMB_IO_Value.AT1IG2_23Bytes; //(2)DPF inlet gas temp
            MB100_C2_DPF_IT[0]  = gMB_IO_Value.AT1IMG_23Bytes; //(3)DPF intermediate temp
            MB100_C2_DPF_OGT[0] = gMB_IO_Value.AT1IG2_01Bytes; //(4)DPF outlet gas temp
            MB100_C2_DPF_TSL    = gMB_IO_Value.AT1S_2345Bytes; //(5)DPF time since last(sec)

            MB100_C2_SCR_IGT[0] = gMB_IO_Value.A1SCREGT1_01Bytes;//(6)SCR inlet gas temp
            MB100_C2_DPF_SLP    = gMB_IO_Value.AT1S_0Byte;       //(7)DPF soot load percent
            MB100_C2_DPF_ALP    = gMB_IO_Value.AT1S_1Byte;       //(8)DPF ash load percent
            MB100_C2_DEF_TL     = gMB_IO_Value.AT1T1I_0Byte;     //(9)DEF tank level
            MB100_C2_DEF_TT[0]  = gMB_IO_Value.AT1T1I_1Byte;     //(10)DEF tank temp
    
            MB100_C2_DEF_AQ[0] = gMB_IO_Value.A1SCRDSI1_01Bytes; //(11)DEF actual quantity
            MB100_C2_DEF_AP[0] = gMB_IO_Value.A1SCRDSI1_5Byte;   //(12)DEF absolute pressure
        }
        break;
        
        case 2:
        {
            MB100_C3_AUX1   = gMB_IO_Value.AUX1_IO_7Byte;   //(1)AUX1 prop.
            MB100_C3_AUX2   = gMB_IO_Value.AUX2_IO_7Byte;   //(2)AUX2 prop.
            MB100_C3_AUX4   = gMB_IO_Value.AUX4_IO_7Byte;   //(3)AUX4 prop.
            MB100_C3_BL     = gMB_IO_Value.IN10_7Byte;      //(4)Blade lever
            MB100_C3_SW     = gMB_IO_Value.IN10_6Byte;      //(5)Swing

            MB100_C3_AD     = gMB_IO_Value.IN17_5Byte;      //(6)Adjust
            MB100_C3_LAS[0] = gMB_IO_Value.SI_45Bytes;      //(7)Lift alarm sensor
            //MB100_C3_AC[0]  = gMB_IO_Value.IN23_45Bytes;    //(8)Adjust crowd
            //MB100_C3_ADD[0] = gMB_IO_Value.IN23_67Bytes;    //(9)Adjust dump
        }
        break;
        
        case 3:
        {
            //MB100_C4_MC = gMB_IO_Value.MAIN_CON_1234567Bytes;
            memcpy(MB100_C4_MC_Version,&gMB_IO_Value.MAIN_CON_1234567Bytes,10);//(1)Main controller
            MB100_C4_DS = 0;    //(2)Display soft

            MB100_C4_SPT[0] = gMB_IO_Value.ACAS_0Byte;  //(3)Set point temp
            MB100_C4_CPT[0] = gMB_IO_Value.ACAS_2Byte;  //(4)Cabin point temp
            MB100_C4_ET[0]  = gMB_IO_Value.ACAS_4Byte;  //(5)Evaporator temp
        }
        break;

        default:
        {

        }
        break;
    }
}

void MenuB100_UpdateMenuUI(uint8_t nIndex)
{
    //printf("\r\n MenuB100_UpdateMenuUI---12--%d\r\n",nIndex);
    char ptString[10];

    int32_t  n_FinalVal[2];
    float    f_FinalVal[2];

    switch( nIndex)
    {
        case 0:
        {
            //(1)Accel position
            if(MB100_C1_AP == 0xFF||((gMB_IO_Value.SI_3Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				// CAN[EEC1] 3-4byte 0~8031.875 (0~0xFAFF)(0xFExx:Error , 0xFFxx:N/A) rpm (0.125/bit)
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[0], ptString);
            }
            else
            {
                n_FinalVal[0] = MB100_C1_AP;
                n_FinalVal[1] = MB100_C1_AP;

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C1_Text[0], ptString);
            }

            //(2)Operating speed
            MenuB100_34BMeterDisp();//chpark.08.28

            //(3)Torque
            //MB100_C1_TO = 40;	// dev.opt
            if(MB100_C1_TO == 0xFF||((gMB_IO_Value.EEC2_2Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				// CAN[EEC1] 3-4byte 0~8031.875 (0~0xFAFF)(0xFExx:Error , 0xFFxx:N/A) rpm (0.125/bit)
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[2], ptString);
            }
            else
            {
                n_FinalVal[0] = MB100_C1_TO;
                n_FinalVal[1] = MB100_C1_TO;

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C1_Text[2], ptString);
            }

            //(4)Oil pressure
            if(MB100_C1_OP[0] == 0xFF||((gMB_IO_Value.EFL_P1_3Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				// CAN[ET1] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[3], ptString);
            }
            else
            {
                n_FinalVal[0] = (MB100_C1_OP[0]*4);//kPa
                n_FinalVal[1] = (n_FinalVal[0]*0.145);//n_FinalVal[0]*0.145+0.5; //PSI //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C1_Text[3], ptString);   
            }
            /*
            //(5)Oil Temp.
            if(MB100_C1_OT[0] == 0xFFFF||((gMB_IO_Value.ET1_23Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m)  )				// CAN[ET1] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[4], ptString);
            }
            else
            {
                n_FinalVal[0] = round(MB100_C1_OT[0]*0.03125)-273;//C    //chpark.10.17
                n_FinalVal[1] = round((n_FinalVal[0]*9)/5)+32;//F        //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C1_Text[4], ptString);
            }
            */

            //(5)Hydraulic oil temp.
            if(MB100_C1_HOT[0] == 0xFF||((gMB_IO_Value.SI_0Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				// CAN[ET1] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[4], ptString);
            }
            else
            {
                n_FinalVal[0] = MB100_C1_HOT[0]-40;   //C
                n_FinalVal[1] = round((n_FinalVal[0]*9)/5)+32;//F  //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C1_Text[4], ptString);
            }

            //(6)Coolant temp.
            if(MB100_C1_CT[0] == 0xFF||((gMB_IO_Value.ET1_0Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				// CAN[ET1] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[5], ptString);
            }
            else
            {
                n_FinalVal[0] = MB100_C1_CT[0]-40;//C
                n_FinalVal[1] = round((n_FinalVal[0]*9)/5)+32;//F //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C1_Text[5], ptString);
            }

            //(7)Fuel rate
            if(MB100_C1_FR[0] == 0xFFFF||((gMB_IO_Value.LFE_01Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m)  )				// CAN[ET1] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[6], ptString);
            }
            else
            {
                f_FinalVal[0] = MB100_C1_FR[0]*0.05;// L/h //chpark.10.17
                f_FinalVal[1] = f_FinalVal[0]*0.264;// gph //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%6.1f",f_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C1_Text[6], ptString);
            }

            //(8)Fuel temp.
            if(MB100_C1_FT[0] == 0xFF||((gMB_IO_Value.ET1_1Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				// CAN[ET1] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[7], ptString);
            }
            else
            {
                n_FinalVal[0] = MB100_C1_FT[0]-40;//C
                n_FinalVal[1] = round((n_FinalVal[0]*9)/5)+32;//F  //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C1_Text[7], ptString);
            }

            //(9)Barometric pressure
            if(MB100_C1_BP[0] == 0xFF||((gMB_IO_Value.AMB_0Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[8], ptString);
            }
            else
            {
                n_FinalVal[0] = round(MB100_C1_BP[0]*0.5);//kPa //chpark.10.17
                n_FinalVal[1] = round(n_FinalVal[0]*0.145);//psi//chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C1_Text[8], ptString);
            }

            //(10)Ambiant air temp.
            if(MB100_C1_AAT[0] == 0xFFFF||((gMB_IO_Value.AMB_34Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m)  )				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[9], ptString);
            }
            else
            {
                n_FinalVal[0] = round(MB100_C1_AAT[0]*0.03125)-273;//C  //chpark.10.17
                n_FinalVal[1] = round((n_FinalVal[0]*9)/5)+32;//F       //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C1_Text[9], ptString);
            }
            /*
            //(11)Boost pressure
            if(MB100_C1_BOP[0] == 0xFF||((gMB_IO_Value.IC1_1Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[11], ptString);
            }
            else
            {
                n_FinalVal[0] = MB100_C1_BOP[0]*2;//kPa
                n_FinalVal[1] = round(n_FinalVal[0]*0.145);//psi //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C1_Text[11], ptString);
            }
            */
            //(11)Engine air inlet pressure
            if(MB100_C1_EAIP[0] == 0xFF||((gMB_IO_Value.IC1_3Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[10], ptString);
            }
            else
            {
                n_FinalVal[0] = MB100_C1_EAIP[0]*2;//kPa
                n_FinalVal[1] = round(n_FinalVal[0]*0.145);//psi 

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C1_Text[10], ptString);
            }
            
            //(12)Boost temp.
            if(MB100_C1_BOT[0] == 0xFF||((gMB_IO_Value.IC1_2Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))			
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[11], ptString);
            }
            else
            {
                n_FinalVal[0] = MB100_C1_BOT[0]-40;//C
                n_FinalVal[1] = round((n_FinalVal[0]*9)/5)+32;//F //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C1_Text[11], ptString);   
            }

            //(13)Common-rail pressure
            if(MB100_C1_CRP[0] == 0xFFFF||((gMB_IO_Value.EFL_P2_23Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m) )				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[12], ptString);
            }
            else
            {
            
                n_FinalVal[0] = round(MB100_C1_CRP[0]/256);//Mpa
                n_FinalVal[1] = round( (n_FinalVal[0]*1000)*0.145 );//Psi //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C1_Text[12], ptString);   
            }

            //(14)Engine throttle actuator
            if(MB100_C1_ETA == 0xFFFF||((gMB_IO_Value.TFAC_01Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[13], ptString);
            }
            else
            {
                f_FinalVal[0] = MB100_C1_ETA*0.0025;
                f_FinalVal[1] = MB100_C1_ETA*0.0025;

                memset(ptString,0,10);
                sprintf(ptString,"%6.1f",f_FinalVal[gMB_Unit] ); //chpark.10.17
                ituTextSetString(MB100_C1_Text[13], ptString);// 
            }


            //(15)DEF differential pressure
            if(MB100_C1_DEF_DP[0] == 0xFFFF||((gMB_IO_Value.ET2_45Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m) )				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[14], ptString);
            }
            else
            {
                f_FinalVal[0] = MB100_C1_DEF_DP[0]*0.007813;//kpa   //chpark.10.17
                f_FinalVal[1] = f_FinalVal[0]*0.145;//psi           //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%6.1f",f_FinalVal[gMB_Unit] ); //chpark.10.17

                ituTextSetString(MB100_C1_Text[14], ptString);   
            }

            //(16)DEF temp.
            if(MB100_C1_DEF_T[0] == 0xFFFF||((gMB_IO_Value.ET2_67Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m) )				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C1_Text[15], ptString);
            }
            else
            {
                n_FinalVal[0] = round(MB100_C1_DEF_T[0]*0.03125)-273;//C    //chpark.10.17
                n_FinalVal[1] = round((n_FinalVal[0]*9)/5)+32;//F           //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C1_Text[15], ptString);   
            }
        }

        break;
        
        case 1:
        {
            //(2-1)DPF exhaust diff.pressure
            if(MB100_C2_DPF_EDP[0] == 0xFFFF||((gMB_IO_Value.AT1IMG_45Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m) )				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C2_Text[0], ptString);
            }
            else
            {
                n_FinalVal[0] = round(MB100_C2_DPF_EDP[0]*0.1);//kPa   //chpark.10.17
                n_FinalVal[1] = round(n_FinalVal[0]*0.145);//psi       //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C2_Text[0], ptString);                
            }

            //(2-2)DPF inlet gas temp.
            if(MB100_C2_DPF_IGT[0] == 0xFFFF||((gMB_IO_Value.AT1IG2_23Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m) )				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C2_Text[1], ptString);
            }
            else
            {
                n_FinalVal[0] = round(MB100_C2_DPF_IGT[0]*0.03125)-273;//C  //chpark.10.17
                n_FinalVal[1] = round((n_FinalVal[0]*9)/5)+32;//F           //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C2_Text[1], ptString);   
            }

            //(2-3)DPF intermediate temp.
            if(MB100_C2_DPF_IT[0] == 0xFFFF||((gMB_IO_Value.AT1IMG_23Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				// CAN[ET1] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C2_Text[2], ptString);
            }
            else
            {
                n_FinalVal[0] = round(MB100_C2_DPF_IT[0]*0.03125)-273; //C  //chpark.10.17
                n_FinalVal[1] = round((n_FinalVal[0]*9)/5)+32;//F           //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C2_Text[2], ptString);   
            }

            //(2-4)DPF outlet gas temp.
            if(MB100_C2_DPF_OGT[0] == 0xFFFF||((gMB_IO_Value.AT1IG2_01Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m) )				// CAN[ET1] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C2_Text[3], ptString);
            }
            else
            {
                n_FinalVal[0] = round(MB100_C2_DPF_OGT[0]*0.03125)-273;//C  //chpark.10.17
                n_FinalVal[1] = round((n_FinalVal[0]*9)/5)+32;//F           //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C2_Text[3], ptString);   
            }

            //(2-5)DPF time since last(sec)//chpark.10.17
            if(MB100_C2_DPF_TSL == 0xFFFFFFFF||((gMB_IO_Value.AT1S_2345Time+TIME_CNT_2MIN)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C2_Text[4], ptString);
            }
            else
            {
                f_FinalVal[0] = MB100_C2_DPF_TSL/3600;//MB100_C2_DPF_TSL/3600+0.05; //chpark.08.01//chpark.10.17
                //f_FinalVal[1] = MB100_C2_DPF_TSL/3600;//MB100_C2_DPF_TSL/3600+0.05; //chpark.08.01//chpark.10.17
               
                //because of time
                if(f_FinalVal[0]>9999.9)
                {
                    f_FinalVal[0]=9999.9;// MAX:9999.9
                    ituTextSetString(MB100_C2_Text[4], "9999.9");
                }
                else
                {
                    //printf("\nAT1S_2345Bytes:0x%X  AT1S_2345Bytes:%6.1f  Raw:%6.1f   Round:%6.1f \n\n",gMB_IO_Value.AT1S_2345Bytes,gMB_IO_Value.AT1S_2345Bytes,f_FinalVal[0],round(f_FinalVal[0]) );
                    memset(ptString,0,10);
                    sprintf(ptString,"%6.1f",f_FinalVal[0] );//hr //chpark.10.17
                    ituTextSetString(MB100_C2_Text[4], ptString);
                }
            }

            //(2-6)SCR inlet gas temp.
            if(MB100_C2_SCR_IGT[0] == 0xFFFF||((gMB_IO_Value.A1SCREGT1_01Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))					
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C2_Text[5], ptString);
            }
            else
            {
                n_FinalVal[0] = round(MB100_C2_SCR_IGT[0]*0.03125)-273;//C  //chpark.10.17
                n_FinalVal[1] = round((n_FinalVal[0]*9)/5)+32;//F           //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C2_Text[5], ptString);   
            }

            //(2-7)DPF soot load percent
            if(MB100_C2_DPF_SLP == 0xFF||((gMB_IO_Value.AT1S_0Time+TIME_CNT_2MIN)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C2_Text[6], ptString);
            }
            else
            {
                n_FinalVal[0] = MB100_C2_DPF_SLP;
                n_FinalVal[1] = MB100_C2_DPF_SLP;
                
                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C2_Text[6], ptString);
            }


            //(2-8)DPF ash load percent
            if(MB100_C2_DPF_ALP == 0xFF||((gMB_IO_Value.AT1S_1Time+TIME_CNT_2MIN)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C2_Text[7], ptString);
            }
            else
            {       
                n_FinalVal[0] = MB100_C2_DPF_ALP;
                n_FinalVal[1] = MB100_C2_DPF_ALP;

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C2_Text[7], ptString); 
            }


            //(2-9)DEF tank level
            //if(MB100_C2_DEF_TL == 0xFF||((gMB_IO_Value.AT1T1I_0Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))		
            if(MB100_C2_DEF_TL == 0xFF||((M5_UTemp_Timer+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))//chpark.24.02.19		
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C2_Text[8], ptString);
            }
            else
            {
                n_FinalVal[0] = MB100_C2_DEF_TL;//round(MB100_C2_DEF_TL*0.4); //chpark.10.17
                n_FinalVal[1] = MB100_C2_DEF_TL;//round(MB100_C2_DEF_TL*0.4);    //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);//chpark.24.01.29 // calculate in UART.
                ituTextSetString(MB100_C2_Text[8], ptString);
            }

            //(2-10)DEF tank temp
            //if(MB100_C2_DEF_TT[0] == 0xFF||((gMB_IO_Value.AT1T1I_1Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))		
            if(MB100_C2_DEF_TT[0] == 0xFF||((M5_Urea_Timer+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))//chpark.24.02.19			
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C2_Text[9], ptString);
            }
            else
            {
                n_FinalVal[0] = MB100_C2_DEF_TT[0]-40;
                n_FinalVal[1] = round((n_FinalVal[0]*9)/5)+32; //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C2_Text[9], ptString);
            }

            //(2-11)DEF actual quantity
            if(MB100_C2_DEF_AQ[0] == 0xFFFF||((gMB_IO_Value.A1SCRDSI1_01Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m)  )					
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C2_Text[10], ptString);
            }
            else
            {
                n_FinalVal[0] = round(MB100_C2_DEF_AQ[0]*0.3);// g/h //chpark.10.17
                n_FinalVal[1] = round(n_FinalVal[0]*0.035);//ozh     //chpark.10.17

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C2_Text[10], ptString);
            }

            //(2-12)DEF absolute pressure
            if(MB100_C2_DEF_AP[0] == 0xFF||((gMB_IO_Value.A1SCRDSI1_5Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C2_Text[11], ptString);
            }
            else
            {
                f_FinalVal[0] = MB100_C2_DEF_AP[0]*8;//kPa      
                f_FinalVal[1] = f_FinalVal[0]*0.145;//psi  

                memset(ptString,0,10);
                sprintf(ptString,"%6.1f",round(f_FinalVal[gMB_Unit]*10)/10);//chpark.24.01.29
                ituTextSetString(MB100_C2_Text[11], ptString);
            }
        }
        break;


        case 2:
        {
            //(3-1)AUX1 prop.
            if(MB100_C3_AUX1 == 0xFF||((gMB_IO_Value.AUX1_IO_7Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C3_Text[0], ptString);
            }
            else
            {
                n_FinalVal[0] = MB100_C3_AUX1 -125;
                n_FinalVal[1] = MB100_C3_AUX1 -125;
                
                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C3_Text[0], ptString);
            }

            //(3-2)AUX2 prop.
            if(MB100_C3_AUX2 == 0xFF||((gMB_IO_Value.AUX2_IO_7Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C3_Text[1], ptString);
            }
            else
            {                 
                n_FinalVal[0] = MB100_C3_AUX2 -125;
                n_FinalVal[1] = MB100_C3_AUX2 -125;

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C3_Text[1], ptString);
            }

            //(3-3)AUX4 prop.
            if(MB100_C3_AUX4 == 0xFF||((gMB_IO_Value.AUX4_IO_7Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C3_Text[2], ptString);
            }
            else
            {
               n_FinalVal[0] = MB100_C3_AUX4 -125;
               n_FinalVal[1] = MB100_C3_AUX4 -125;

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C3_Text[2], ptString);   
            }

            //(3-4)Blade prop.
            if(((gMB_IO_Value.IN10_7Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C3_Text[3], ptString);
            }
            else
            {
                //chpark.12.05
                //n_FinalVal[0] = MB100_C3_BL -128;
                //n_FinalVal[1] = MB100_C3_BL -128;
                //
                if(MB100_C3_BL>=0x80)//0x80->128
                {
                    n_FinalVal[0] = MB100_C3_BL-256;
                    n_FinalVal[1] = MB100_C3_BL-256;
                }
                else
                {
                    n_FinalVal[0] = MB100_C3_BL;
                    n_FinalVal[1] = MB100_C3_BL;
                }

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C3_Text[3], ptString);
            }

            //(3-5)Swing prop.
            if(((gMB_IO_Value.IN10_6Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))					
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C3_Text[4], ptString);
            }
            else
            {
                //chpark.12.05
                if(MB100_C3_SW>=0x80)
                {
                    n_FinalVal[0] = MB100_C3_SW-256;
                    n_FinalVal[1] = MB100_C3_SW-256;
                }
                else
                {
                    n_FinalVal[0] = MB100_C3_SW;
                    n_FinalVal[1] = MB100_C3_SW;
                }

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C3_Text[4], ptString);
            }

            //(3-6)Adjust prop.
            if(((gMB_IO_Value.IN17_5Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))					
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C3_Text[5], ptString);
            }
            else
            {
                //printf("[MenuB100]IN17_5Time:%d   MB100_C3_AD:%x\n",gMB_IO_Value.IN17_5Time,MB100_C3_AD);//chpark.
                //chpark.12.05
                if(MB100_C3_AD>=0x80)
                {
                    n_FinalVal[0] = MB100_C3_AD-256;
                    n_FinalVal[1] = MB100_C3_AD-256;
                }
                else
                {
                    n_FinalVal[0] = MB100_C3_AD;
                    n_FinalVal[1] = MB100_C3_AD;
                }

                memset(ptString,0,10);
                sprintf(ptString,"%5d",n_FinalVal[gMB_Unit]);
                ituTextSetString(MB100_C3_Text[5], ptString);
            }

            //(3-7)Lift alarm sensor
            if(MB100_C3_LAS[0] == 0xFFFF ||((gMB_IO_Value.SI_45Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m)  )	
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C3_Text[6], ptString);
            }
            else
            {
                f_FinalVal[0] = MB100_C3_LAS[0]*0.001;//MPa     
                f_FinalVal[1] = f_FinalVal[0]*1000*0.145;//psi  

                memset(ptString,0,10);
                sprintf(ptString,"%6.1f",round(f_FinalVal[gMB_Unit]*10)/10);//chpark.24.01.29
                ituTextSetString(MB100_C3_Text[6], ptString);
            }
            /*
            //(3-8)Adjust crowd
            if(MB100_C3_AC[0] == 0xFFFF||((gMB_IO_Value.IN23_45Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m)  )	
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C3_Text[7], ptString);
            }
            else
            {
                f_FinalVal[0] = MB100_C3_AC[0]*0.001;//MPa       
                f_FinalVal[1] = f_FinalVal[0]*1000*0.145;//psi   
             
                memset(ptString,0,10);
                sprintf(ptString,"%6.1f",round(f_FinalVal[gMB_Unit]*10)/10);//chpark.24.01.29
                ituTextSetString(MB100_C3_Text[7], ptString);
            }


            //(3-9)Adjust dump
            if(MB100_C3_ADD[0] == 0xFFFF||((gMB_IO_Value.IN23_67Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m)  )	
            {
                memset(ptString,0,10);
                
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C3_Text[8], ptString);
            }
            else
            {
                f_FinalVal[0] = MB100_C3_ADD[0]*0.001;//MPa
                f_FinalVal[1] = f_FinalVal[0]*1000*0.145;//psi

                memset(ptString,0,10);
                sprintf(ptString,"%6.1f",round(f_FinalVal[gMB_Unit]*10)/10);//chpark.24.01.29
                ituTextSetString(MB100_C3_Text[8], ptString);               
            }
            */
        }
        break;

        case 3:
        {
			//printf("gMB_IO_Value.MAIN_CON_1234567Bytes[%s],MB100_C4_MC_Version[%s]\n",gMB_IO_Value.MAIN_CON_1234567Bytes,MB100_C4_MC_Version);
            //(4-1)Main controller 
            int nSize = strlen(MB100_C4_MC_Version);
            //printf("MAIN_CON_1234567Bytes [nSize=%d]\n",nSize);
            if(nSize<=1)				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A     ");
                ituTextSetString(MB100_C4_Text[0], ptString);
            }
            else
            {
                //memcpy(MB100_C4_MC_Version,&gMB_IO_Value.MAIN_CON_1234567Bytes,10);
                //memset(ptString,0,10);
                sprintf(ptString,"%s",MB100_C4_MC_Version); 
                ituTextSetString(MB100_C4_Text[0], ptString);
            }
           
            // JPHong 2023-05-18
            {
                n_FinalVal[0] = MB100_C4_DS;
                n_FinalVal[1] = MB100_C4_DS;

                memset(ptString,0,10);
                sprintf(ptString," V%d.%d%d%c%c ", CFG_VERSION_MAJOR,CFG_VERSION_MINOR,CFG_VERSION_PATCH,gMicomFirmVer[5],gMicomFirmVer[6]);		// JPHong 2024-05-20
//				sprintf(ptString," V%d.%d%d ", CFG_VERSION_MAJOR,CFG_VERSION_MINOR,CFG_VERSION_PATCH);
                ituTextSetString(MB100_C4_Text[1], ptString);
            }

            //(4-3)Set point temp.
           if(MB100_C4_SPT[0] == 0xFF ||((gMB_IO_Value.ACAS_0Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))					
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C4_Text[2], ptString);
            }
            else
            {
                f_FinalVal[0] = MB100_C4_SPT[0]*0.5; //C     
                f_FinalVal[1] = (f_FinalVal[0]*9)/5+32;//F   

                memset(ptString,0,10);
                sprintf(ptString,"%6.1f",round(f_FinalVal[gMB_Unit]*10)/10 );//chpark.24.01.29
                ituTextSetString(MB100_C4_Text[2], ptString);
            }

            //(4-4)Cabin point temp.
            if(MB100_C4_CPT[0] == 0xFF||((gMB_IO_Value.ACAS_2Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))					
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C4_Text[3], ptString);
            }
            else
            {
                f_FinalVal[0] = MB100_C4_CPT[0]*0.5-20;//C       
                f_FinalVal[1] = (f_FinalVal[0]*9)/5+32;//F       

                memset(ptString,0,10);
                sprintf(ptString,"%6.1f",round(f_FinalVal[gMB_Unit]*10)/10);//chpark.24.01.29
                ituTextSetString(MB100_C4_Text[3], ptString);   
            }


            //(4-5)Evaporator temp.
            if(MB100_C4_ET[0] == 0xFF||((gMB_IO_Value.ACAS_4Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))						
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB100_C4_Text[4], ptString);
            }
            else
            {                 
                f_FinalVal[0] = MB100_C4_ET[0]*0.5-20;//C    
                f_FinalVal[1] = (f_FinalVal[0]*9)/5+32;//F   

                memset(ptString,0,10);
                sprintf(ptString,"%6.1f",round(f_FinalVal[gMB_Unit]*10)/10 );//chpark.24.01.29
                ituTextSetString(MB100_C4_Text[4], ptString);
            }

        }

        break;
    }
}

void MenuB100_SetMenuTab(uint8_t nIndex)
{
    //printf("\r\n MenuB100_SetMenu---12--%d\r\n",nIndex);
    nMenuB100_MenuIndex  = nIndex;
    for(int i=0;i<MB100_END;i++)
    {
        ituWidgetSetVisible(MB100_CT[i],false);
    }

    MenuB100_GetMenuData(nIndex);
    MenuB100_UpdateMenuUI(nIndex);
    ituWidgetSetVisible(MB100_CT[nIndex],true);

    MenuB100_SetLang(gMB_Language);//chpark.09.26
    MenuB100_SetUnit(gMB_Unit);//chpark.09.26
}


bool MB100_MenuTabNext()
{
    nMenuB100_MenuIndex ++;
    if(nMenuB100_MenuIndex>=MB100_END)nMenuB100_MenuIndex=MB100_ENG1;
    MenuB100_SetMenuTab(nMenuB100_MenuIndex);
}

bool MB100_MenuTabPrev()
{
    nMenuB100_MenuIndex --;
    if(nMenuB100_MenuIndex<0)nMenuB100_MenuIndex=MB100_ENG4;
    MenuB100_SetMenuTab(nMenuB100_MenuIndex);
}

bool MB100_D7_BTNMouseUp(ITUWidget* widget, char* param)
{
    //printf("\r\n MB100_D7_BTNMouseUp---1222--%d\r\n",nMenuB100_MenuIndex);
    MB100_MenuTabNext();

    return true;
}

bool MB100_D6_BTNMouseUp(ITUWidget* widget, char* param)
{
    GotoMain(MAIN_MODE_NORMAL); 
    return true;
}

bool MB100_D5_BTNMouseUp(ITUWidget* widget, char* param)
{
    ITULayer *MenuBTop_Layer = ituSceneFindWidget(&theScene, "MenuBTop_Layer");
    assert(MenuBTop_Layer);
    ituLayerGoto(MenuBTop_Layer);

    return true;
}


bool MenuB100_Timer(ITUWidget* widget, char* param)
{
    #ifdef USED_ROTATION  //chpark.10.19
    if (RotationMode!=0)
    {
      return 0;
    }
    #endif

	if((gMainTimerCnt_10m - MB100_gMT_Timer) >= 50)	//0.5 Sec
	{
        if(!MB100_m_b_ReadyToStart)
            return false;//chpark.09.25

		MB100_gMT_Timer = gMainTimerCnt_10m;

		MB100_gMT_ExitTimer++;
		
		if(MB100_gMT_ExitTimer >= 60)//30sec
		{
            /*
			printf("\r\n Menu Exit > goto Main\r\n");
			
            #ifdef RELEASE_VER
	            GotoMain(MAIN_MODE_NORMAL); 
            #else
	            printf("\r\n [Debug Mode]Menu Exit > goto Main\r\n");
            #endif
			*/
		}

        MenuB100_GetMenuData(nMenuB100_MenuIndex);
        MenuB100_UpdateMenuUI(nMenuB100_MenuIndex);
        //printf("\r\n[MenuB100_Timer]GetData & ShowData\r\n");
	}

    return true;
}


bool MenuB100_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t JogDir = 0;
	JogDir = param[0];
	//printf("MenuB100 JogWheel %d\r\n",JogDir);
	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
        case LANDSCAPE_1:
		case LANDSCAPE_2:
            if(JogDir == JOG_DIAL_ROT_RIGHT)
			{
                MB100_MenuTabNext();
			}
			else if(JogDir == JOG_DIAL_ROT_LEFT)
			{
                MB100_MenuTabPrev();
            }    
			
		break;
	}

    return true;
}


bool MenuB100_JogButton(ITUWidget* widget, char* param)
{
	uint8_t JogButton = 0;
    JogButton = param[0];
    MB100_gMT_ExitTimer = 0;
	
	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
        case LANDSCAPE_1:
		case LANDSCAPE_2:
			switch(JogButton)
			{
				case JOG_DIAL_KEY_UP:
                case JOG_DIAL_KEY_LEFT:
					MB100_MenuTabPrev();
				    break;

				case JOG_DIAL_KEY_DOWN:
                case JOG_DIAL_KEY_RIGHT:
                    MB100_MenuTabNext();
				    break;
				
				case JOG_DIAL_KEY_ENTER_OFF:
                    MB100_MenuTabNext();
				    break;
			}

		    break;
	}

    return true;
}

bool MenuB100_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t JogSwitch = 0;
	JogSwitch = param[0];
	MB100_gMT_ExitTimer = 0;

	if(JogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
		ITULayer *MenuBTop_Layer = ituSceneFindWidget(&theScene, "MenuBTop_Layer");
        assert(MenuBTop_Layer);
        ituLayerGoto(MenuBTop_Layer);
	}
	else if(JogSwitch == JOG_DIAL_SW_HOME_OFF)
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}

    return true;
}

bool MenuB100_34BMeterDisp(void)	//JPHong 0830
{       
	char tmp[32] = {0,};
	static uint32_t EEC1_34Bytes_TIME = 0;
	uint32_t tData = 0;

	if ((DTVal.EEC1_34Bytes==0xFFFFFFFF)||((DTVal.EEC1_34Time+TIME_CNT_5SEC)<gMainTimerCnt_10m))
	{
		DTVal.EEC1_34Cnt = 0;
		DTVal.EEC1_34Bytes=0;
		sprintf(tmp, "N/A  ");
		EEC1_34Bytes_TIME = 0;
		ituTextSetString(MB100_C1_Text[1], tmp);
	}
	else
	{
		if (DTVal.EEC1_34Cnt==0)
		{
			EEC1_34Bytes_TIME = gMainTimerCnt_10m;
			DTVal.EEC1_34Cnt = 0;
			DTVal.EEC1_34Bytes=0;
		}
		else if ((EEC1_34Bytes_TIME+TIME_CNT_500MS)<gMainTimerCnt_10m)
		{
			tData = DTVal.EEC1_34Bytes/DTVal.EEC1_34Cnt;
//			printf("MenuB100_34BMeterDisp2=%d %d 0x%X\n",DTVal.EEC1_34Bytes/DTVal.EEC1_34Cnt,tData);
			DTVal.EEC1_34Bytes = 0;
			DTVal.EEC1_34Cnt = 0;	// JPHong0830			
			sprintf(tmp, "%d", tData);
			EEC1_34Bytes_TIME = gMainTimerCnt_10m;
			ituTextSetString(MB100_C1_Text[1], tmp);
		}
	}
	return true;
}
