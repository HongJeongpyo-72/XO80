#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#define MB200_IO1				0
#define MB200_IO2				1
#define MB200_IO3				2
#define MB200_IO3_1				3
#define MB200_IO4				4
#define MB200_IO5				5
#define MB200_IO6				6
#define MB200_END				7

static uint32_t MB200_gMT_Timer;
static uint8_t  MB200_gMT_ExitTimer = 0;			// Sec Count

bool MB200_MenuTabNext();
bool MB200_MenuTabPrev();

static ITUContainer* MB200_CT[MB200_END];
/*
static ITUText* MB200_C1_Text[10]; //chpark.09.14//input1
static ITUText* MB200_C2_Text[7];  //chpark.09.14//input2
static ITUText* MB200_C3_Text[2];  //chpark.09.14//input3//0
static ITUText* MB200_C4_Text[7];  //chpark.09.14//out1
static ITUText* MB200_C5_Text[10];  //chpark.09.14//out2
static ITUText* MB200_C6_Text[2];  

static ITUCheckBox* MB200_C1_CHK[5];  //chpark.24.04.08//input1
static ITUCheckBox* MB200_C2_CHK[5];  //chpark.24.04.08//input2
static ITUCheckBox* MB200_C3_CHK[14]; //chpark.24.04.08//input3
static ITUCheckBox* MB200_C4_CHK[10]; //chpark.09.14//out1
static ITUCheckBox* MB200_C5_CHK[8];  //chpark.09.14//out2
static ITUCheckBox* MB200_C6_CHK[3];  //chpark.01.25//out3
*/
static ITUText* MB200_C1_Text[7];   //chpark.24.10.18//input1
static ITUText* MB200_C2_Text[10];  //chpark.24.10.18//input2
static ITUText* MB200_C3_Text[2];   //chpark.24.10.18//input3//0
static ITUText* MB200_C3_1_Text[1];  //chpark.24.10.18//input4//0

static ITUText* MB200_C4_Text[7];   //chpark.24.10.18//out1
static ITUText* MB200_C5_Text[10];  //chpark.24.10.18//out2
static ITUText* MB200_C6_Text[4];   //chpark.24.10.18



static ITUCheckBox* MB200_C1_CHK[5];  //chpark.24.10.18//input1
static ITUCheckBox* MB200_C2_CHK[6];  //chpark.24.10.18//input2
static ITUCheckBox* MB200_C3_CHK[12]; //chpark.24.10.18//input3
static ITUCheckBox* MB200_C3_1_CHK[2]; //chpark.24.10.18//input4

static ITUCheckBox* MB200_C4_CHK[11]; //chpark.24.10.18//out1
static ITUCheckBox* MB200_C5_CHK[8];  //chpark.24.10.18//out2
static ITUCheckBox* MB200_C6_CHK[6];  //chpark.24.10.18//out3


static ITUIcon* MB200_C1_LANG[5]; //
static ITUIcon* MB200_C2_LANG[5]; //
static ITUIcon* MB200_C3_LANG[5]; //
static ITUIcon* MB200_C3_1_LANG[5]; //chpark.24.10.18//input4
static ITUIcon* MB200_C4_LANG[5]; //
static ITUIcon* MB200_C5_LANG[5]; //
static ITUIcon* MB200_C6_LANG[5]; //


//----------------------------------------------------------
//MB200 Input1
uint8_t MB200_C1_ACC; //(C1P06) ACC
uint8_t MB200_C1_MGL_SW4;//(C1P07) Multifunctional grip L SW4
//(C1P10)-
float MB200_C1_FS; //(C1P11) Fuel sender
float MB200_C1_LAS;//(C1P12) Lift alarm sensor
uint8_t MB200_C1_ST;//(C1P15) START position
float MB200_C1_HOT;//(C1P16) Hydraulic oil temp.
uint8_t MB200_C1_CO_SW;//(C1P17) Coolant level SW
uint8_t MB200_C1_PA_SW;//(C1P18) PA pressure SW
//(C1P19)-
float MB200_C1_AP;//(C1P20) Accelerator position
float MB200_C1_P4;//(C1P21) P4 pressure sensor 
float MB200_C1_SGR_P1;//(C1P26)(1) Standard grip R prop. 1
float MB200_C1_MGR_P1;//(C1P26)(2) Multifunctional grip R prop. 1

float MB200_C2_SGL_P1;//(C1P27)(1) Standard grip L prop. 1
float MB200_C2_MGL_P1;//(C1P27)(2) Multifunctional grip L prop. 1

uint8_t MB200_C2_GPS_ACC;//(C1P28) GPS power monitoring
float MB200_C2_GL_P3;    //(C1P29) Multifunctional grip L prop. 3
//------------------------------------------------------------
//MB200 Input2
float MB200_C2_MGR_P3;//(C2P01) Multifunctional grip R prop. 3
float MB200_C2_MGR_P2;//(C2P02) Multifunctional grip R prop. 2
float MB200_C2_MGL_P2;//(C2P03) Multifunctional grip L prop. 2
float MB200_C2_BL1; //(C2P04) Blade lever 1
float MB200_C2_BL2; //(C2P05) Blade lever 2
float MB200_C2_P1;  //(C2P06) Pedal 1
float MB200_C2_P2;  //(C2P07) Pedal 2
//(C2P08)-
uint8_t MB200_C2_LL_SW;  //(C2P11) Lever lock SW
uint8_t MB200_C2_AUX3_F; //(C2P12) AUX3 foot
uint8_t MB200_C2_SGL_SW2;//(C2P13)(1) Standard grip L SW2
uint8_t MB200_C2_MGL_SW3;//(C2P13)(2) Multifunctional grip L SW3
uint8_t MB200_C2_SB;//(C2P14) Seat belt
//------------------------------------------------------------
//MB200 Input3
uint8_t MB200_C3_F_SW;//(C2P20) Float SW
uint8_t MB200_C3_SGR_SW4;//(C2P21)(1) Standard grip R SW4
uint8_t MB200_C3_MGR_SW4;//(C2P21)(2) Multifunctional grip R SW4

uint8_t MB200_C3_SGR_SW2;//(C2P22)(1) Standard grip R SW2
uint8_t MB200_C3_MGR_SW3;//(C2P22)(2) Multifunctional grip R SW3

uint8_t MB200_C3_SGL_SW3;//(C2P23)(1) Standard grip L SW3
uint8_t MB200_C3_MGL_SW1;//(C2P23)(2) Multifunctional grip L SW1

uint8_t MB200_C3_SGL_SW1;//(C2P24)(1) Standard grip L SW1
uint8_t MB200_C3_MGL_SW2;//(C2P24)(2) Multifunctional grip L SW2

uint8_t MB200_C3_SGR_SW1;//(C2P25)(1) Standard grip R SW1
uint8_t MB200_C3_MGR_SW2;//(C2P25)(2) Multifunctional grip R SW2

uint8_t MB200_C3_S_SW;   //(C2P26) Starter SW
//(C2P27)-
uint8_t MB200_C3_1_GE;     //(C2P28) Grease empty
//(C2P29)-
float   MB200_C3_1_AUX3_P_SW;//(C2P30) AUX3 pressure SW
//------------------------------------------------------------
//MB200 Out1
uint8_t MB200_C4_AR;//(C1P33) ACC relay //chpark.24.10.18
float   MB200_C4_HP;//(C1P32) Hydraulic pump
uint8_t MB200_C4_BR;//(C1P33) Backup relay
uint8_t MB200_C4_LD;//(C1P34) Load dump
uint8_t MB200_C4_WL;//(C1P35) Work light 

uint8_t MB200_C4_1_SOL;     //(C1P36) 1WAY SOL
float   MB200_C4_AUX1_PA;   //(C1P37) AUX1 press. adjust.
float   MB200_C4_AUX2_PA;   //(C1P38) AUX2 press. adjust.
uint8_t MB200_C4_AUX3_S;    //(C1P39) AUX3 SOL
uint8_t MB200_C4_L_SOL;     //(C1P40) Lever lock SOL

float MB200_C4_AUX4_G_SOL;  //(C1P41) AUX4 G SOL
float MB200_C4_AUX4_H_SOL;  //(C1P42) AUX4 H SOL
uint8_t MB200_C4_F_SOL;     //(C1P43) Float SOL
uint8_t MB200_C4_AUX24_SOL; //(C1P44) AUX2/4 select. SOL
uint8_t MB200_C4_HR;        //(C1P45) Horn relay

uint8_t MB200_C4_T_SOL;     //(C1P46) Travel shift SOL
float MB200_C4_AUX1_A_SOL;  //(C1P47) AUX1 A SOL
float MB200_C4_AUX1_B_SOL;  //(C1P48) AUX1 B SOL
//------------------------------------------------------------
//MB200 Out2
float MB200_C5_AUX2_C_SOL; //(C1P49) AUX2 C SOL
float MB200_C5_AUX2_D_SOL; //(C1P50) AUX2 D SOL
float MB200_C5_AD_SOL;//(C2P31)(1) Adjuster retraction SOL
float MB200_C5_SL_SOL;//(C2P31)(2) Swing L SOL
float MB200_C5_AC_SOL;//(C2P32)(1) Adjuster extension SOL
float MB200_C5_SR_SOL;//(C2P32)(2) Swing R SOL

uint8_t MB200_C5_SL; //(C2P33) Side light
uint8_t MB200_C5_WR; //(C2P34) Washer relay
uint8_t MB200_C5_SR; //(C2P35) Starter relay
//(7)-
uint8_t MB200_C5_GP; //(C2P36) Grease pump
uint8_t MB200_C5_REL;//(C2P37) Rear light 
uint8_t MB200_C5_ROL;//(C2P38) Room lamp

float MB200_C5_RF_SOL;//(11) Travel RF SOL
float MB200_C5_RB_SOL;//(12) Travel RB SOL
float MB200_C5_LF_SOL;//(13) Travel LF SOL
float MB200_C5_LB_SOL;//(14) Travel LB SOL
uint8_t MB200_C5_BR;//(15) Beacon relay

uint8_t MB200_C5_WI;//(16) Wiper 

//------------------------------------------------------------
//MB200 Out3
uint8_t MB200_C6_HF_SOL;    //(1) High flow SOL
uint8_t MB200_C6_PSR;       //(2) Push switch relay
uint8_t MB200_C6_TL;        //(3) Tail lamp
uint8_t MB200_C6_BB;        //(4) Back buzzer
uint16_t MB200_C6_BU_SOL;   //(5) Blade up SOL
uint16_t MB200_C6_BD_SOL;   //(6) Blade down SOL
//OX Module
uint16_t MB200_C6_EO15A;      //(7) Extension out 1/5A
uint16_t MB200_C6_EO25B;      //(8) Extension out 2/5B
uint8_t  MB200_C6_EO3;        //(9) Extension out 3
uint8_t  MB200_C6_EO4;        //(10) Extension out 4
//------------------------------------------------------------

static int8_t nMenuB200_MenuIndex=0;

void MenuB200_UpdateMenuUI(uint8_t nIndex);
void MenuB200_UpdateMenuUI_Input(uint8_t nIndex);
void MenuB200_UpdateMenuUI_Output(uint8_t nIndex);

void MenuB200_SetMenuTab(uint8_t nIndex);
void MenuB200_GetMenuData(uint8_t nIndex);
void MenuB200_InitControl();

void MenuB200_SetLang(uint8_t nIndex);

bool MenuB200_Enter(ITUWidget* widget, char* param)
{
    //printf("\n[MenuB200_Enter]--1111---\n");
	MB200_gMT_ExitTimer = 0;
	MB200_gMT_Timer = gMainTimerCnt_10m;

    MenuB200_InitControl();

    //printf("\n[MenuB200_Enter]--3333---\n");
    for(int i=0;i<MB200_END;i++)
    {
        MenuB200_GetMenuData(i);
    }

    MenuB200_SetMenuTab(0);

    return true;
}

void MenuB200_InitControl()
{
    //printf("\r\n MenuB200_InitControl2--111---\r\n");
   MB200_CT[0] = (ITUContainer*)ituSceneFindWidget(&theScene, "MenuB200_C1");
   MB200_CT[1] = (ITUContainer*)ituSceneFindWidget(&theScene, "MenuB200_C2");
   MB200_CT[2] = (ITUContainer*)ituSceneFindWidget(&theScene, "MenuB200_C3");
   MB200_CT[3] = (ITUContainer*)ituSceneFindWidget(&theScene, "MenuB200_C3_1");
   MB200_CT[4] = (ITUContainer*)ituSceneFindWidget(&theScene, "MenuB200_C4");
   MB200_CT[5] = (ITUContainer*)ituSceneFindWidget(&theScene, "MenuB200_C5");
   MB200_CT[6] = (ITUContainer*)ituSceneFindWidget(&theScene, "MenuB200_C6");

 //-------------------
//Input1  
//printf("\r\n MenuB200_InitControl2--111---\r\n");
    MB200_C1_CHK[0] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C1_R1");
    MB200_C1_CHK[1] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C1_R2");
    MB200_C1_CHK[2] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C1_R3");
    MB200_C1_CHK[3] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C1_R4");
    MB200_C1_CHK[4] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C1_R5");
    //MB200_C1_CHK[5] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C1_R6");
   
    for(int i=0;i<5;i++)
    {
        ituWidgetSetVisible(MB200_C1_CHK[i], true);
    }

    //printf(" sizeof(MB200_C1_CHK) = %d \n",sizeof(MB200_C1_CHK));
    
    MB200_C1_Text[0] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C1_T1");
    MB200_C1_Text[1] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C1_T2");
    MB200_C1_Text[2] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C1_T3");
    MB200_C1_Text[3] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C1_T4");
    MB200_C1_Text[4] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C1_T5");
    MB200_C1_Text[5] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C1_T6");
    MB200_C1_Text[6] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C1_T7");
    //MB200_C1_Text[7] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C1_T8");
    //MB200_C1_Text[8] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C1_T9");
    //MB200_C1_Text[9] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C1_T10");

    for(int i=0;i<7;i++)
    {
        ituWidgetSetVisible(MB200_C1_Text[i], true);
    }

    MB200_C1_LANG[0] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C1_LANG1");
    MB200_C1_LANG[1] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C1_LANG2");
    MB200_C1_LANG[2] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C1_LANG3");
    MB200_C1_LANG[3] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C1_LANG4");
    MB200_C1_LANG[4] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C1_LANG5");

   //-------------------
//Input2
    MB200_C2_CHK[0] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C2_R1");
    MB200_C2_CHK[1] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C2_R2");
    MB200_C2_CHK[2] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C2_R3");
    MB200_C2_CHK[3] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C2_R4");
    MB200_C2_CHK[4] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C2_R5");
    MB200_C2_CHK[5] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C2_R6");
   
    for(int i=0;i<6;i++)
    {
        ituWidgetSetVisible(MB200_C2_CHK[i], true);
    }

    MB200_C2_Text[0] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C2_T1");
    MB200_C2_Text[1] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C2_T2");
    MB200_C2_Text[2] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C2_T3");
    MB200_C2_Text[3] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C2_T4");
    MB200_C2_Text[4] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C2_T5");
    MB200_C2_Text[5] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C2_T6");
    MB200_C2_Text[6] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C2_T7");
    MB200_C2_Text[7] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C2_T8");
    MB200_C2_Text[8] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C2_T9");
    MB200_C2_Text[9] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C2_T10");


    for(int i=0;i<10;i++)
    {
        ituWidgetSetVisible(MB200_C2_Text[i], true);
    }
  
    MB200_C2_LANG[0] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C2_LANG1");
    MB200_C2_LANG[1] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C2_LANG2");
    MB200_C2_LANG[2] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C2_LANG3");
    MB200_C2_LANG[3] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C2_LANG4");
    MB200_C2_LANG[4] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C2_LANG5");

//printf("\r\n MenuB200_InitControl2--444---\r\n");
    //-------------------
//Input3
    MB200_C3_CHK[0] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_R1");
    MB200_C3_CHK[1] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_R2");
    MB200_C3_CHK[2] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_R3");
    MB200_C3_CHK[3] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_R4");
    MB200_C3_CHK[4] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_R5");
    MB200_C3_CHK[5] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_R6");
    MB200_C3_CHK[6] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_R7");

    MB200_C3_CHK[7]  =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_R8");
    MB200_C3_CHK[8]  =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_R9");
    MB200_C3_CHK[9]  =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_R10");
    MB200_C3_CHK[10] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_R11");
    MB200_C3_CHK[11] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_R12");
    //MB200_C3_CHK[12] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_R13");
    //MB200_C3_CHK[13] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_R14");
    
    for(int i=0;i<12;i++)
    {
        ituWidgetSetVisible(MB200_C3_CHK[i], true);
    }
   
    MB200_C3_LANG[0] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C3_LANG1");
    MB200_C3_LANG[1] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C3_LANG2");
    MB200_C3_LANG[2] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C3_LANG3");
    MB200_C3_LANG[3] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C3_LANG4");
    MB200_C3_LANG[4] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C3_LANG5");
    //-------------------
//Input4
    MB200_C3_1_CHK[0] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_1_R1");
    MB200_C3_1_CHK[1] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C3_1_R2");

    for(int i=0;i<2;i++)
    {
        ituWidgetSetVisible(MB200_C3_1_CHK[i], true);
    }

    MB200_C3_1_LANG[0] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C3_1_LANG1");
    MB200_C3_1_LANG[1] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C3_1_LANG2");
    MB200_C3_1_LANG[2] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C3_1_LANG3");
    MB200_C3_1_LANG[3] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C3_1_LANG4");
    MB200_C3_1_LANG[4] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C3_1_LANG5");

//printf("\r\n MenuB200_InitControl2--555---\r\n");
//-------------------
//Out1  
    MB200_C4_CHK[0] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C4_R1");
    MB200_C4_CHK[1] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C4_R2");
    MB200_C4_CHK[2] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C4_R3");
    MB200_C4_CHK[3] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C4_R4");
    MB200_C4_CHK[4] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C4_R5");
    MB200_C4_CHK[5] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C4_R6");
    MB200_C4_CHK[6] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C4_R7");
    MB200_C4_CHK[7] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C4_R8");
    MB200_C4_CHK[8] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C4_R9");
    MB200_C4_CHK[9] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C4_R10");
    MB200_C4_CHK[10] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C4_R11");

    for(int i=0;i<11;i++)
    {
        ituWidgetSetVisible(MB200_C4_CHK[i], true);
    }
   
    MB200_C4_Text[0] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C4_T1");
    MB200_C4_Text[1] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C4_T2");
    MB200_C4_Text[2] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C4_T3");
    MB200_C4_Text[3] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C4_T4");
    MB200_C4_Text[4] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C4_T5");
    MB200_C4_Text[5] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C4_T6");
    MB200_C4_Text[6] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C4_T7");
   
    for(int i=0;i<7;i++)
    {
        ituWidgetSetVisible(MB200_C4_Text[i], true);
    }

    MB200_C4_LANG[0] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C4_LANG1");
    MB200_C4_LANG[1] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C4_LANG2");
    MB200_C4_LANG[2] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C4_LANG3");
    MB200_C4_LANG[3] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C4_LANG4");
    MB200_C4_LANG[4] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C4_LANG5");

//printf("\r\n MenuB200_InitControl2--666---\r\n");
   //-------------------
//Out2  
    MB200_C5_CHK[0] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C5_R1");
    MB200_C5_CHK[1] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C5_R2");
    MB200_C5_CHK[2] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C5_R3");
    MB200_C5_CHK[3] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C5_R4");
    MB200_C5_CHK[4] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C5_R5");
    MB200_C5_CHK[5] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C5_R6");
    MB200_C5_CHK[6] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C5_R7");
    MB200_C5_CHK[7] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C5_R8");

    for(int i=0;i<8;i++)
    {
        ituWidgetSetVisible(MB200_C5_CHK[i], true);
    }

    MB200_C5_Text[0] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C5_T1");
    MB200_C5_Text[1] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C5_T2");
    MB200_C5_Text[2] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C5_T3");
    MB200_C5_Text[3] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C5_T4");
    MB200_C5_Text[4] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C5_T5");
    MB200_C5_Text[5] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C5_T6");
    MB200_C5_Text[6] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C5_T7");
    MB200_C5_Text[7] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C5_T8");
    MB200_C5_Text[8] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C5_T9");
    MB200_C5_Text[9] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C5_T10");

    for(int i=0;i<10;i++)
    {
        ituWidgetSetVisible(MB200_C5_Text[i], true);
    }

    MB200_C5_LANG[0] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C5_LANG1");
    MB200_C5_LANG[1] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C5_LANG2");
    MB200_C5_LANG[2] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C5_LANG3");
    MB200_C5_LANG[3] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C5_LANG4");
    MB200_C5_LANG[4] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C5_LANG5");

//printf("\r\n MenuB200_InitControl2--777---\r\n");
   //-------------------
//Out3  
    MB200_C6_CHK[0] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C6_R1");
    MB200_C6_CHK[1] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C6_R2");
    MB200_C6_CHK[2] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C6_R3");
    MB200_C6_CHK[3] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C6_R4");
    MB200_C6_CHK[4] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C6_R5");
    MB200_C6_CHK[5] =  (ITUCheckBox* )ituSceneFindWidget(&theScene, "MenuB200_C6_R6");

    for(int i=0;i<4;i++)
    {
        ituWidgetSetVisible(MB200_C6_CHK[i], true);//chpark.24.10.18
    }

    MB200_C6_Text[0] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C6_T1");
    MB200_C6_Text[1] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C6_T2");
    MB200_C6_Text[2] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C6_T3");//chpark.24.10.18
    MB200_C6_Text[3] =  (ITUText* )ituSceneFindWidget(&theScene, "MenuB200_C6_T4");//chpark.24.10.18

    for(int i=0;i<4;i++)
    {
        ituWidgetSetVisible(MB200_C6_Text[i], true);
    }

    MB200_C6_LANG[0] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C6_LANG1");
    MB200_C6_LANG[1] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C6_LANG2");
    MB200_C6_LANG[2] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C6_LANG3");
    MB200_C6_LANG[3] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C6_LANG4");
    MB200_C6_LANG[4] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB200_C6_LANG5");

//printf("\r\n MenuB200_InitControl2--111---\r\n");
    for(int i=0;i<5;i++) ituWidgetDisable(MB200_C1_CHK[i]);
    for(int i=0;i<6;i++) ituWidgetDisable(MB200_C2_CHK[i]);
    for(int i=0;i<12;i++) ituWidgetDisable(MB200_C3_CHK[i]);
    for(int i=0;i<2;i++) ituWidgetDisable(MB200_C3_1_CHK[i]);

    //for(int i=0;i<5;i++) ituWidgetDisable(MB200_C1_CHK[i]);
    //for(int i=0;i<5;i++) ituWidgetDisable(MB200_C2_CHK[i]);
    //for(int i=0;i<14;i++) ituWidgetDisable(MB200_C3_CHK[i]);


    for(int i=0;i<11;i++) ituWidgetDisable(MB200_C4_CHK[i]);
    for(int i=0;i<8;i++) ituWidgetDisable(MB200_C5_CHK[i]);
    for(int i=0;i<6;i++) ituWidgetDisable(MB200_C6_CHK[i]);//chpark.24.10.18
}

void MenuB200_SetLang(uint8_t nIndex)
{
    for(int i=0;i<5;i++)
    {
        ituWidgetSetVisible(MB200_C1_LANG[i],false);
        ituWidgetSetVisible(MB200_C2_LANG[i],false);
        ituWidgetSetVisible(MB200_C3_LANG[i],false);
        ituWidgetSetVisible(MB200_C3_1_LANG[i],false);
        ituWidgetSetVisible(MB200_C4_LANG[i],false);
        ituWidgetSetVisible(MB200_C5_LANG[i],false);
        ituWidgetSetVisible(MB200_C6_LANG[i],false);
    }

    if(nMenuB200_MenuIndex==0)
    {
        ituWidgetSetVisible(MB200_C1_LANG[nIndex],true);
    }
    else if(nMenuB200_MenuIndex==1)
    {
        ituWidgetSetVisible(MB200_C2_LANG[nIndex],true);
    }
    else if(nMenuB200_MenuIndex==2)
    {
        ituWidgetSetVisible(MB200_C3_LANG[nIndex],true);
    }
    else if(nMenuB200_MenuIndex==3)
    {
        ituWidgetSetVisible(MB200_C3_1_LANG[nIndex],true);
    }
    else if(nMenuB200_MenuIndex==4)
    {
        ituWidgetSetVisible(MB200_C4_LANG[nIndex],true);
    }
    else if(nMenuB200_MenuIndex==5)
    {
        ituWidgetSetVisible(MB200_C5_LANG[nIndex],true);
    }
    else if(nMenuB200_MenuIndex==6)
    {
        ituWidgetSetVisible(MB200_C6_LANG[nIndex],true);
    }

}

void MenuB200_GetMenuData(uint8_t nIndex)
{
    //printf("\r\n MenuB200_SetData---12--%d\r\n",nIndex);
    switch( nIndex)
    {
        case 0:
        {
            //(1)//-
            MB200_C1_ACC        = gMB_IO_Value.IN1_4Byte_4bit;  //(2)//C1P06
            MB200_C1_MGL_SW4     = gMB_IO_Value.IN29_0Byte_3bit;//(3)//C1P07
            //(4)//-//C1P10
            MB200_C1_FS         = gMB_IO_Value.IN3_45Bytes; //(5)//C1P11
            
            MB200_C1_LAS        = gMB_IO_Value.OUT2_23Bytes;//(6)//C1P12
            MB200_C1_ST         = gMB_IO_Value.IN1_4Byte_5bit;//(6)//C1P15
            MB200_C1_HOT        = gMB_IO_Value.IN3_67Bytes; //(8)//C1P16
            MB200_C1_CO_SW      = gMB_IO_Value.IN2_4Byte_7bit;//(9)//C1P17
            MB200_C1_PA_SW      = gMB_IO_Value.IN1_2Byte_4bit;//(10)//C1P18
            
            //(11)//-//C1P19
            MB200_C1_AP         = gMB_IO_Value.OUT2_01Bytes;//(12)//C1P20
            MB200_C1_P4         = gMB_IO_Value.IN3_23Bytes;//(13)//C1P21
            //(14)//-//C1P22
            //(15)//-//C1P25

            MB200_C1_SGR_P1     = gMB_IO_Value.IN28_45Bytes;//(16)//C1P26
            MB200_C1_MGR_P1     = gMB_IO_Value.IN30_12Bytes;//(17)
            //(18)//-//-
        }
        break;
        
        case 1:
        {
            MB200_C2_SGL_P1     = gMB_IO_Value.IN28_23Bytes;//(1)//C1P27
            MB200_C2_MGL_P1     = gMB_IO_Value.IN29_12Bytes;//(2)
            MB200_C2_GPS_ACC    = gMB_IO_Value.IN26_0Byte_4bit;//(3)//C1P28
            MB200_C2_GL_P3      = gMB_IO_Value.IN29_56Bytes;//(4)//C1P29
            //(5)//-//C1P30

            MB200_C2_MGR_P3        = gMB_IO_Value.IN30_56Bytes;//(6)//C2P01
            MB200_C2_MGR_P2        = gMB_IO_Value.IN30_34Bytes;//(7)//C2P02
            MB200_C2_MGL_P2        = gMB_IO_Value.IN29_34Bytes;//(8)//C2P03
            MB200_C2_BL1           = gMB_IO_Value.IN19_45Bytes;//(9)//C2P04
            MB200_C2_BL2           = gMB_IO_Value.IN19_67Bytes;//(10)//C2P05

            MB200_C2_P1            = gMB_IO_Value.IN19_23Bytes; //(11)//C2P06
            MB200_C2_P2            = gMB_IO_Value.IN21_01Bytes; //(12)//C2P07
            //(13)C2P08//-
            MB200_C2_LL_SW         = gMB_IO_Value.IN1_4Byte_0bit; //(14)//C2P11
            MB200_C2_AUX3_F        = gMB_IO_Value.IN1_2Byte_3bit; //(15)//C2P12

            MB200_C2_SGL_SW2       = gMB_IO_Value.IN28_0Byte_1bit; //(16)//C2P13
            MB200_C2_MGL_SW3       = gMB_IO_Value.IN29_0Byte_2bit; //(17)//C2P13
            MB200_C2_SB       = gMB_IO_Value.IN2_1Byte_2bit; //(18)//C2P14
        }
        break;
        
        case 2:
        {
            //(1)//C2P15 
            //(2)//C2P16 
            //(3)//C2P17 
            //(4)//C2P18 
            //(5)//C2P19 

            MB200_C3_F_SW          = gMB_IO_Value.IN2_2Byte_6bit;  //(6)//C2P20            
            MB200_C3_SGR_SW4       = gMB_IO_Value.IN28_1Byte_3bit; //(7)//C2P21
            MB200_C3_MGR_SW4       = gMB_IO_Value.IN30_0Byte_3bit; //(8)
            MB200_C3_SGR_SW2       = gMB_IO_Value.IN28_1Byte_1bit ;//(9)//C2P22
            MB200_C3_MGR_SW3        = gMB_IO_Value.IN30_0Byte_2bit;//(10)//C2P29

            MB200_C3_SGL_SW3       = gMB_IO_Value.IN28_0Byte_2bit;  //(11)//C2P23
            MB200_C3_MGL_SW1       = gMB_IO_Value.IN29_0Byte_0bit;  //(12)
            MB200_C3_SGL_SW1       = gMB_IO_Value.IN28_0Byte_0bit;  //(13)//C2P24
            MB200_C3_MGL_SW2       = gMB_IO_Value.IN29_0Byte_1bit;  //(14)
            MB200_C3_SGR_SW1        = gMB_IO_Value.IN28_1Byte_0bit; //(15)//C2P25

            MB200_C3_MGR_SW2        = gMB_IO_Value.IN30_0Byte_1bit;//(16)
            MB200_C3_S_SW           = gMB_IO_Value.IN26_1Byte_0bit;//(17)//C2P26
            //(18)//C2P27//- 
        }
        break;

        case 3://in4
        {
            MB200_C3_1_GE             = gMB_IO_Value.IN2_4Byte_6bit;//(1)//C2P28
            //(2)//C2P29//- 
            MB200_C3_1_AUX3_P_SW      = gMB_IO_Value.IN1_2Byte_2bit;//(2)//C2P30  
        }
        break;
        
        case 4://out1
        {
            printf("B200_GetMenuData==\n");
            MB200_C4_AR        = gMB_IO_Value.OUT6_7Byte_3bit;  //(1)C1P31
            MB200_C4_HP        = gMB_IO_Value.OUT3_01Bytes;     //(2)C1P32
            MB200_C4_BR        = gMB_IO_Value.OUT1_3Byte_3bit;  //(3)C1P33
            MB200_C4_LD        = gMB_IO_Value.OUT1_0Byte_0bit;  //(4)C1P34
            MB200_C4_WL        = gMB_IO_Value.OUT1_4Byte_7bit;  //(5)C1P35

            MB200_C4_1_SOL     = gMB_IO_Value.OUT1_3Byte_2bit;//(6)C1P36
            MB200_C4_AUX1_PA   = gMB_IO_Value.AUX_PIO_01Bytes;//(7)C1P37
            MB200_C4_AUX2_PA   = gMB_IO_Value.AUX_PIO_23Bytes;//(8)C1P38
            MB200_C4_AUX3_S    = gMB_IO_Value.OUT1_2Byte_5bit;//(9)C1P39
            MB200_C4_L_SOL     = gMB_IO_Value.OUT1_2Byte_0bit;//(10)C1P40

            MB200_C4_AUX4_G_SOL= gMB_IO_Value.AUX4_IO_23Bytes;//(11)C1P41
            MB200_C4_AUX4_H_SOL= gMB_IO_Value.AUX4_IO_45Bytes;//(12)C1P42
            MB200_C4_F_SOL     = gMB_IO_Value.OUT1_1Byte_4bit;//(13)C1P43
            MB200_C4_AUX24_SOL = gMB_IO_Value.OUT1_2Byte_6bit;//(14)C1P44
            MB200_C4_HR        = gMB_IO_Value.OUT1_0Byte_5bit;//(15)C1P45

            MB200_C4_T_SOL     = gMB_IO_Value.OUT1_2Byte_4bit;//(16)C1P46
            MB200_C4_AUX1_A_SOL= gMB_IO_Value.AUX1_IO_23Bytes;//(17)C1P47
            MB200_C4_AUX1_B_SOL= gMB_IO_Value.AUX1_IO_45Bytes;//(18)C1P48
        }
        break;

        case 5://out2
        {
            MB200_C5_AUX2_C_SOL    = gMB_IO_Value.AUX2_IO_23Bytes; //(1)C1P49
            MB200_C5_AUX2_D_SOL    = gMB_IO_Value.AUX2_IO_45Bytes; //(2)C1P50

            MB200_C5_AD_SOL         = gMB_IO_Value.OUT14_01Bytes;//(3-1)C2P31
            MB200_C5_SL_SOL         = gMB_IO_Value.OUT10_67Bytes;//(3-2)
            MB200_C5_AC_SOL        = gMB_IO_Value.OUT14_23Bytes;//(4-1)C2P32
            MB200_C5_SR_SOL        = gMB_IO_Value.OUT10_45Bytes; //(4-2)
            MB200_C5_SL            = gMB_IO_Value.OUT1_3Byte_4bit;//(5)C2P33

            MB200_C5_WR            = gMB_IO_Value.OUT1_5Byte_7bit;//(6)C2P34
            MB200_C5_SR            = gMB_IO_Value.OUT6_5Byte_3bit;//(7)C2P35//chpark.24.01.250
            //(7)-
            MB200_C5_GP            = gMB_IO_Value.OUT6_0Byte_0bit;//(8)C2P36
            MB200_C5_REL           = gMB_IO_Value.OUT6_0Byte_3bit;//(9)C2P37
            MB200_C5_ROL           = gMB_IO_Value.OUT1_0Byte_3bit;//(10)C2P38

            MB200_C5_RF_SOL        = gMB_IO_Value.OUT13_45Bytes; //(11)C2P39
            MB200_C5_RB_SOL        = gMB_IO_Value.OUT13_67Bytes; //(12)C2P40
            MB200_C5_LF_SOL        = gMB_IO_Value.OUT13_01Bytes; //(13)C2P41
            MB200_C5_LB_SOL        = gMB_IO_Value.OUT13_23Bytes; //(14)C2P42

            MB200_C5_BR            = gMB_IO_Value.OUT6_0Byte_4bit;//(15)C2P43
            MB200_C5_WI            = gMB_IO_Value.OUT1_0Byte_4bit;//(16)C2P44
        }
        break;

        case 6://out3
        {
            MB200_C6_HF_SOL    = gMB_IO_Value.OUT1_3Byte_7bit;//(1)C2P45
            MB200_C6_PSR       = gMB_IO_Value.OUT6_7Byte_4bit;//(2)C2P46
            MB200_C6_TL        = gMB_IO_Value.OUT6_0Byte_6bit;//(3)C2P47 
            MB200_C6_BB        = gMB_IO_Value.OUT1_1Byte_7bit;//(4)C2P48
            MB200_C6_BU_SOL    = gMB_IO_Value.OUT10_01Bytes;  //(5)C2P49
            MB200_C6_BD_SOL    = gMB_IO_Value.OUT10_23Bytes;  //(6)C2P50
            //OX module
            MB200_C6_EO15A      = gMB_IO_Value.OUT20_01Bytes;  //(7)C1P06
            MB200_C6_EO25B      = gMB_IO_Value.OUT20_23Bytes;  //(8)C1P07
            MB200_C6_EO3        = gMB_IO_Value.OUT6_5Byte_6bit;//(9)C1P08 
            MB200_C6_EO4        = gMB_IO_Value.OUT6_5Byte_7bit;//(10)C1P09
        }
        break;

        default:
        {
            printf("\r\n MenuB200_SetData---12--Error: %d\r\n",nIndex);
        }
        break;
    }
}

void MenuB200_UpdateMenuUI(uint8_t nIndex)
{
    if(nIndex>=0 && nIndex<=3)
    {
        //Input:0,1,2,3
        MenuB200_UpdateMenuUI_Input(nIndex);
    }
    else
    {
        //Output:0,1,2
        nIndex = nIndex-4;
        printf("[MenuB200_UpdateMenuUI][out]nIndex=%d\n",nIndex);
        MenuB200_UpdateMenuUI_Output(nIndex);
    }
}

void MenuB200_UpdateMenuUI_Input(uint8_t nIndex)
{
    //printf("\r\n MenuB200_UpdateMenuUI---12--%d\r\n",nIndex);
    char ptString[10];

    int32_t  n_FinalVal[2];
    float    f_FinalVal[2];

    switch(nIndex)
    {
        case 0:
        //---------------------------------------------------------------  
            //(1-1)(C1P05)-
        //---------------------------------------------------------------
            //(1-2)(C1P06)ACC
            if(MB200_C1_ACC==0xFF||((gMB_IO_Value.IN1_4Byte_4Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C1_CHK[0],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C1_CHK[0],true);
                ituCheckBoxSetChecked(MB200_C1_CHK[0],MB200_C1_ACC);
            }
        //---------------------------------------------------      
            //(1-3)(C1P07)Multifunctional grip L SW4
            if(MB200_C1_MGL_SW4==0xFF||((gMB_IO_Value.IN29_0Byte_3Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C1_CHK[1],false);
            }
            else
            {      
                ituWidgetSetVisible(MB200_C1_CHK[1],true); 
                ituCheckBoxSetChecked(MB200_C1_CHK[1],MB200_C1_MGL_SW4);
            }
        //---------------------------------------------------  
            //(1-4)(C1P10)-
        //---------------------------------------------------  
            //(1-5)(C1P11)Fuel sender
            if(MB200_C1_FS == 0xFFFF||((gMB_IO_Value.IN3_45Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C1_Text[0], ptString);
            }
            else
            {
                MB200_C1_FS = MB200_C1_FS/1000.0;
                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C1_FS);
                ituTextSetString(MB200_C1_Text[0], ptString);           
            }
        //---------------------------------------------------  
            //(1-6)(C1P12)Lift alarm sensor
            if(MB200_C1_LAS == 0xFFFF||((gMB_IO_Value.OUT2_23Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C1_Text[1], ptString);
            }
            else
            {
                MB200_C1_LAS = MB200_C1_LAS/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C1_LAS);
                ituTextSetString(MB200_C1_Text[1], ptString);   
            }
        //---------------------------------------------------   
            //(1-7)(C1P15)START Position
            if(MB200_C1_ST==0xFF||((gMB_IO_Value.IN1_4Byte_5Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C1_CHK[2],false);
            }
            else
            {    
                ituWidgetSetVisible(MB200_C1_CHK[2],true);   
                ituCheckBoxSetChecked(MB200_C1_CHK[2],MB200_C1_ST);
            }
        //---------------------------------------------------------
            //(1-8)(C1P16)Hydraulic oil temp.
            if(MB200_C1_HOT == 0xFFFF||((gMB_IO_Value.IN3_67Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C1_Text[2], ptString);
            }
            else
            {
                MB200_C1_HOT = MB200_C1_HOT/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C1_HOT);
                ituTextSetString(MB200_C1_Text[2], ptString);
            }
        //---------------------------------------------------------
            //(1-9)(C1P17)Coolant level SW 
            if(MB200_C1_CO_SW==0xFF||((gMB_IO_Value.IN2_4Byte_7Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C1_CHK[3],false);
            }
            else
            {    
                ituWidgetSetVisible(MB200_C1_CHK[3],true);   
                ituCheckBoxSetChecked(MB200_C1_CHK[3],MB200_C1_CO_SW);
            }
        //---------------------------------------------------------
            //(1-10)(C1P18)PA pressure SW
            if(MB200_C1_PA_SW==0xFF||((gMB_IO_Value.IN1_2Byte_4Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C1_CHK[4],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C1_CHK[4],true);
                ituCheckBoxSetChecked(MB200_C1_CHK[4],MB200_C1_PA_SW);
            }
        //---------------------------------------------------------
            //(1-11)(C1P19)-
        //---------------------------------------------------------    
            //(1-12)(C1P20)Accelerator position
            if(MB200_C1_AP == 0xFFFF||((gMB_IO_Value.OUT2_01Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C1_Text[3], ptString);
            }
            else
            {
                MB200_C1_AP = MB200_C1_AP/1000.0;
                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C1_AP);
                ituTextSetString(MB200_C1_Text[3], ptString);
            }
         //---------------------------------------------------------
            //(1-13)(C1P21)P4 pressure sensor
            if(MB200_C1_P4 == 0xFFFF||((gMB_IO_Value.IN3_23Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C1_Text[4], ptString);
            }
            else
            {
                MB200_C1_P4 = MB200_C1_P4/1000.0;
                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C1_P4);
                ituTextSetString(MB200_C1_Text[4], ptString);
            }
        //---------------------------------------------------  
            //(1-14)(C1P22)-
            //(1-15)(C1P25)-
        //---------------------------------------------------  
            //(1-16) (C1P26)Standard grip R prop. 1
            if(MB200_C1_SGR_P1 == 0xFFFF||((gMB_IO_Value.IN28_45Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C1_Text[5], ptString);
            }
            else
            {
                MB200_C1_SGR_P1 = MB200_C1_SGR_P1/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C1_SGR_P1);
                ituTextSetString(MB200_C1_Text[5], ptString);
            }
        //---------------------------------------------------  
            //(1-17) (-)Multifunctional grip R prop. 1
            if(MB200_C1_MGR_P1 == 0xFFFF||((gMB_IO_Value.IN30_12Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C1_Text[6], ptString);
            }
            else
            {
                MB200_C1_MGR_P1 = MB200_C1_MGR_P1/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C1_MGR_P1);
                ituTextSetString(MB200_C1_Text[6], ptString);
            }
        //---------------------------------------------------   
            //(1-18) (-)-
        //---------------------------------------------------  
        break;
           
        //====================================================================
        case 1:
            //(2-1) (C1P27)Standard grip L prop. 1
            if(MB200_C2_SGL_P1 == 0xFFFF||((gMB_IO_Value.IN28_23Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C2_Text[0], ptString);
            }
            else
            {
                MB200_C2_SGL_P1 = MB200_C2_SGL_P1/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C2_SGL_P1);
                ituTextSetString(MB200_C2_Text[0], ptString);
            }
        //---------------------------------------------------------
            //(2-2)(C1P27)Multifunctional grip L prop. 1
            if(MB200_C2_MGL_P1 == 0xFFFF||((gMB_IO_Value.IN29_12Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C2_Text[1], ptString);
            }
            else
            {
                MB200_C2_MGL_P1 = MB200_C2_MGL_P1/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C2_MGL_P1);
                ituTextSetString(MB200_C2_Text[1], ptString);
            }
         //---------------------------------------------------------
            //(2-3) (C1P28)GPS power monitoring
            if(MB200_C2_GPS_ACC==0xFF||((gMB_IO_Value.IN26_0Byte_4Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C2_CHK[0],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C2_CHK[0],true);
                ituCheckBoxSetChecked(MB200_C2_CHK[0],MB200_C2_GPS_ACC);
            }
         //---------------------------------------------------------
            //(2-4) (C1P29)Multifunctional grip L prop. 3
            if(MB200_C2_GL_P3 == 0xFFFF||((gMB_IO_Value.IN29_56Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C2_Text[2], ptString);
            }
            else
            {
                MB200_C2_GL_P3 = MB200_C2_GL_P3/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C2_GL_P3);
                ituTextSetString(MB200_C2_Text[2], ptString);
            }
            //---------------------------------------------------   
            //(2-5)(C1P30)(-)
            //--------------------------------------------------- 

            //(2-6)(C2P01) Multifunctional grip R prop. 3
            if(MB200_C2_MGR_P3 == 0xFFFF||((gMB_IO_Value.IN30_56Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C2_Text[3], ptString);
            }
            else
            {
                MB200_C2_MGR_P3 = MB200_C2_MGR_P3/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C2_MGR_P3);
                ituTextSetString(MB200_C2_Text[3], ptString);   
            }

            //(2-7)(C2P02) Multifunctional grip R prop. 2
            if(MB200_C2_MGR_P2 == 0xFFFF||((gMB_IO_Value.IN30_34Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C2_Text[4], ptString);
            }
            else
            {
                //printf("R prop.2 = %6.3f\r\n",MB200_C2_R2/1000);
                MB200_C2_MGR_P2 = MB200_C2_MGR_P2/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C2_MGR_P2);
                ituTextSetString(MB200_C2_Text[4], ptString);
            }

            //(2-8)(C2P03)Multifunctional grip L prop. 2
            if(MB200_C2_MGL_P2 == 0xFFFF||((gMB_IO_Value.IN29_34Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C2_Text[5], ptString);
            }
            else
            {
                MB200_C2_MGL_P2 = MB200_C2_MGL_P2/1000.0;
                //printf("L prop.2 = %6.3f\r\n",MB200_C2_L2);
                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C2_MGL_P2);
                ituTextSetString(MB200_C2_Text[5], ptString);
            }

            //(2-9)(C2P04)Blade lever 1
            if(MB200_C2_BL1 == 0xFFFF||((gMB_IO_Value.IN19_45Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C2_Text[6], ptString);
            }
            else
            {
                MB200_C2_BL1 = MB200_C2_BL1/1000.0;
                //printf("L prop.2 = %6.3f\r\n",MB200_C2_BL1);

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C2_BL1);
                ituTextSetString(MB200_C2_Text[6], ptString);
            }

            //(2-10)(C2P05)Blade Lever 2
            if(MB200_C2_BL2 == 0xFFFF||((gMB_IO_Value.IN19_67Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C2_Text[7], ptString);
            }
            else
            {
                MB200_C2_BL2 = MB200_C2_BL2/1000.0;

                //printf("L prop.2 = %6.3f\r\n",MB200_C2_BL2);
                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C2_BL2);
                ituTextSetString(MB200_C2_Text[7], ptString);
            }

            //(2-11)(C2P06)Pedal 1
            if(MB200_C2_P1 == 0xFFFF||((gMB_IO_Value.IN19_23Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C2_Text[8], ptString);
            }
            else
            {
                MB200_C2_P1 = MB200_C2_P1/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C2_P1);
                ituTextSetString(MB200_C2_Text[8], ptString);
            }

            //(2-12)(C2P07)Pedal 2
            if(MB200_C2_P2 == 0xFFFF||((gMB_IO_Value.IN21_01Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C2_Text[9], ptString);
            }
            else
            {
                MB200_C2_P2 = MB200_C2_P2/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C2_P2);
                ituTextSetString(MB200_C2_Text[9], ptString);
            }
            //---------------------------------------------------   
            //(2-13)(C2P08)(-)
            //--------------------------------------------------- 
            //(2-14)(C2P11)Lever lock SW
            if(MB200_C2_LL_SW==0xFF||((gMB_IO_Value.IN1_4Byte_0Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C2_CHK[1],false);
            }
            else
            {   
                ituWidgetSetVisible(MB200_C2_CHK[1],true);    
                ituCheckBoxSetChecked(MB200_C2_CHK[1],MB200_C2_LL_SW);
            }
        //----------------------------------------------------------
            //(2-15)(C2P12)AUX3 foot
            if(MB200_C2_AUX3_F==0xFF||((gMB_IO_Value.IN1_2Byte_3Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C2_CHK[2],false);
            }
            else
            {    
                ituWidgetSetVisible(MB200_C2_CHK[2],true);   
                ituCheckBoxSetChecked(MB200_C2_CHK[2],MB200_C2_AUX3_F);
            }
        //----------------------------------------------------------
            //(2-16)(C2P13)Standard grip L SW2
            if(MB200_C2_SGL_SW2==0xFF||((gMB_IO_Value.IN28_0Byte_1Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C2_CHK[3],false);
            }
            else
            {     
                ituWidgetSetVisible(MB200_C2_CHK[3],true);  
                ituCheckBoxSetChecked(MB200_C2_CHK[3],MB200_C2_SGL_SW2);
            }

            //(2-17)(-)Multifunctional grip L SW3       
            if(MB200_C2_MGL_SW3==0xFF||((gMB_IO_Value.IN29_0Byte_2Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C2_CHK[4],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C2_CHK[4],true);
                ituCheckBoxSetChecked(MB200_C2_CHK[4],MB200_C2_MGL_SW3);
            }

            //(2-18)(C2P14)Seat belt   
            if(MB200_C2_SB==0xFF||((gMB_IO_Value.IN2_1Byte_2Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C2_CHK[5],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C2_CHK[5],true);
                ituCheckBoxSetChecked(MB200_C2_CHK[5],MB200_C2_SB);
            }
        break;

        //=======================================================================
        case 2:
            //---------------------------------------------------   
            //(3-1)(C2P15)(-)
            //(3-2)(C2P16)(-)
            //(3-3)(C2P17)(-)
            //(3-4)(C2P18)(-)
            //(3-5)(C2P19)(-)
            //--------------------------------------------------- 
            //(3-6)(C2P20)Float SW          
            if(MB200_C3_F_SW==0xFF||((gMB_IO_Value.IN2_2Byte_6Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C3_CHK[0],false);
            }
            else
            {      
                ituWidgetSetVisible(MB200_C3_CHK[0],true); 
                ituCheckBoxSetChecked(MB200_C3_CHK[0],MB200_C3_F_SW);
            }
        //-------------------------
            //(3-7)(C2P21)(1)Standard grip R SW4        
            if(MB200_C3_SGR_SW4==0xFF||((gMB_IO_Value.IN28_1Byte_3Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C3_CHK[1],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C3_CHK[1],true);
                ituCheckBoxSetChecked(MB200_C3_CHK[1],MB200_C3_SGR_SW4);
            }

            //(3-8)(-)(2)Multifunctional grip R SW4
            if(MB200_C3_MGR_SW4==0xFF||((gMB_IO_Value.IN30_0Byte_3Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))		
            {
                ituWidgetSetVisible(MB200_C3_CHK[2],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C3_CHK[2],true);
                ituCheckBoxSetChecked(MB200_C3_CHK[2],MB200_C3_MGR_SW4);
            }
        //-------------------------
             //(3-9)(C2P22)(1)Standard grip R SW2
            if(MB200_C3_SGR_SW2==0xFF||((gMB_IO_Value.IN28_1Byte_1Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C3_CHK[3],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C3_CHK[3],true);
                ituCheckBoxSetChecked(MB200_C3_CHK[3],MB200_C3_SGR_SW2);
            }

            //(3-10)(-)(2)Multifunctional grip R SW3
            if(MB200_C3_MGR_SW3==0xFF||((gMB_IO_Value.IN30_0Byte_2Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C3_CHK[4],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C3_CHK[4],true);
                ituCheckBoxSetChecked(MB200_C3_CHK[4], MB200_C3_MGR_SW3);
            }
        //-------------------------
            //(3-11)(C2P23)(1)Standard grip L SW3     
            if(MB200_C3_SGL_SW3==0xFF||((gMB_IO_Value.IN28_0Byte_2Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C3_CHK[5],false);
            }
            else
            {      
                ituWidgetSetVisible(MB200_C3_CHK[5],true); 
                ituCheckBoxSetChecked(MB200_C3_CHK[5],MB200_C3_SGL_SW3);
            }

            //(3-12)(-)(2)Multifunctional grip L SW1  
            if(MB200_C3_MGL_SW1==0xFF||((gMB_IO_Value.IN29_0Byte_0Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C3_CHK[6],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C3_CHK[6],true);
                ituCheckBoxSetChecked(MB200_C3_CHK[6],MB200_C3_MGL_SW1);
            }
        //-------------------------
            //(3-13)(C2P24)(1)Standard grip L SW1    
            if(MB200_C3_SGL_SW1==0xFF||((gMB_IO_Value.IN28_0Byte_0Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C3_CHK[7],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C3_CHK[7],true);
                ituCheckBoxSetChecked(MB200_C3_CHK[7],MB200_C3_SGL_SW1);
            }

            //(3-14)(-)(2)Multifunctional grip L SW2
            if(MB200_C3_MGL_SW2==0xFF||((gMB_IO_Value.IN29_0Byte_1Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C3_CHK[8],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C3_CHK[8],true);
                ituCheckBoxSetChecked(MB200_C3_CHK[8],MB200_C3_MGL_SW2);
            }
        //-------------------------
            //(3-15)(C2P25)Standard grip R SW1          
            if(MB200_C3_SGR_SW1==0xFF||((gMB_IO_Value.IN28_1Byte_0Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C3_CHK[9],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C3_CHK[9],true);
                ituCheckBoxSetChecked(MB200_C3_CHK[9],MB200_C3_SGR_SW1);
            }

            //(3-16)(-)Multifunctional grip R SW2  
            if(MB200_C3_MGR_SW2==0xFF||((gMB_IO_Value.IN30_0Byte_1Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C3_CHK[10],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C3_CHK[10],true);
                ituCheckBoxSetChecked(MB200_C3_CHK[10],MB200_C3_MGR_SW2);
            }
        //-------------------------
             //(3-17)(C2P26)Starter SW
            if(MB200_C3_S_SW==0xFF||((gMB_IO_Value.IN26_1Byte_0Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C3_CHK[11],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C3_CHK[11],true);
                ituCheckBoxSetChecked(MB200_C3_CHK[11],MB200_C3_S_SW);
            }
        //-------------------------    
           //(3-18)(C2P27)-
        //-------------------------
           

        break;

        //=======================================================================
        case 3:
            //(4-1)(C2P28)Grease empty
            if(MB200_C3_1_GE==0xFF||((gMB_IO_Value.IN2_4Byte_6Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C3_1_CHK[0],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C3_1_CHK[0],true);
                ituCheckBoxSetChecked(MB200_C3_1_CHK[0],MB200_C3_1_GE);
            }
        //-------------------------
            //(4-2)(C2P29)-
        //-------------------------
            //(4-3)(C2P30)AUX3 pressure SW
            if(MB200_C3_1_AUX3_P_SW==0xFF||((gMB_IO_Value.IN1_2Byte_2Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C3_1_CHK[1],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C3_1_CHK[1],true);
                ituCheckBoxSetChecked(MB200_C3_1_CHK[1],MB200_C3_1_AUX3_P_SW);
            }
        //-------------------------

        break;
    }
}


void MenuB200_UpdateMenuUI_Output(uint8_t nIndex)
{
    //printf("\r\n MenuB200_UpdateMenuUI_Output--%d\r\n",nIndex);
    char ptString[10];
    int32_t  n_FinalVal[2];
    float    f_FinalVal[2];

    switch(nIndex)
    {
        case 0:
        {
        //-------------------------
            //(4-1) (C1P31)ACC relay
            if(MB200_C4_AR==0xFF||((gMB_IO_Value.OUT6_7Byte_3Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                printf("[MenuB200_UpdateMenuUI_Output][hide]MB200_C4_AR=%x\n",MB200_C4_AR);
                ituWidgetSetVisible(MB200_C4_CHK[0],false);
            }
            else
            {   
                printf("[MenuB200_UpdateMenuUI_Output][show]MB200_C4_AR=%x\n",MB200_C4_AR);    
                ituWidgetSetVisible(MB200_C4_CHK[0],true);
                ituCheckBoxSetChecked(MB200_C4_CHK[0],MB200_C4_AR);
            }
        //-------------------------
            //(4-2) (C1P32)Hydraulic pump
            if(MB200_C4_HP == 0xFFFF||((gMB_IO_Value.OUT3_01Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))					
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C4_Text[0], ptString);
            }
            else
            {
                MB200_C4_HP = MB200_C4_HP/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(MB200_C4_HP*1000)/1000);//chpark.24.01.29
                ituTextSetString(MB200_C4_Text[0], ptString);
            }
        //-------------------------
            //(4-3) (C1P33)Backup relay          
            if(MB200_C4_BR==0xFF||((gMB_IO_Value.OUT1_3Byte_3Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C4_CHK[1],false);
            }
            else
            {   
                ituWidgetSetVisible(MB200_C4_CHK[1],true);    
                ituCheckBoxSetChecked(MB200_C4_CHK[1],MB200_C4_BR);
            }
        //-------------------------
            //(4-4) (C1P34)Load dump
            if(MB200_C4_LD==0xFF||((gMB_IO_Value.OUT1_0Byte_0Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C4_CHK[2],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C4_CHK[2],true);
                ituCheckBoxSetChecked(MB200_C4_CHK[2],MB200_C4_LD);
            }
        //-------------------------
            //(4-5) (C1P35)Work light
            if(MB200_C4_WL==0xFF||((gMB_IO_Value.OUT1_4Byte_7Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C4_CHK[3],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C4_CHK[3],true);
                ituCheckBoxSetChecked(MB200_C4_CHK[3],MB200_C4_WL);
            }
        //-------------------------
            //(4-6) (C1P36)1way SOL
            if(MB200_C4_1_SOL==0xFF||((gMB_IO_Value.OUT1_3Byte_2Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C4_CHK[4],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C4_CHK[4],true);
                ituCheckBoxSetChecked(MB200_C4_CHK[4],MB200_C4_1_SOL);
            }
        //-------------------------
            //(4-7) (C1P37)AUX1 press.adjust
            if(MB200_C4_AUX1_PA == 0xFFFF||((gMB_IO_Value.AUX_PIO_01Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C4_Text[1], ptString);
            }
            else
            {
                MB200_C4_AUX1_PA = MB200_C4_AUX1_PA/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(MB200_C4_AUX1_PA*1000)/1000);//chpark.24.01.29
                ituTextSetString(MB200_C4_Text[1], ptString);
            }
        //-------------------------
            //(4-8) (C1P38)AUX2 press.adjust
            if(MB200_C4_AUX2_PA == 0xFFFF||((gMB_IO_Value.AUX_PIO_23Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))					
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C4_Text[2], ptString);
            }
            else
            {
                MB200_C4_AUX2_PA = MB200_C4_AUX2_PA/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(MB200_C4_AUX2_PA*1000)/1000); //chpark.24.01.29
                ituTextSetString(MB200_C4_Text[2], ptString);
            }
        //-------------------------
            //(4-9) (C1P39)AUX3 SOL
            if(MB200_C4_AUX3_S==0xFF||((gMB_IO_Value.OUT1_2Byte_5Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C4_CHK[5],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C4_CHK[5],true);
                ituCheckBoxSetChecked(MB200_C4_CHK[5],MB200_C4_AUX3_S);
            }
        //-------------------------
            //(4-10) (C1P40)Lvever lock SOL       
            if(MB200_C4_L_SOL==0xFF||((gMB_IO_Value.OUT1_2Byte_0Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C4_CHK[6],false);
            }
            else
            {      
                ituWidgetSetVisible(MB200_C4_CHK[6],true); 
                ituCheckBoxSetChecked(MB200_C4_CHK[6],MB200_C4_L_SOL);
            }
        //-------------------------            
            //(4-11) (C1P41)AUX4 G SOL
            if(MB200_C4_AUX4_G_SOL == 0xFFFF||((gMB_IO_Value.AUX4_IO_23Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))					
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C4_Text[3], ptString);
            }
            else
            {
                MB200_C4_AUX4_G_SOL = MB200_C4_AUX4_G_SOL/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",MB200_C4_AUX4_G_SOL);
                ituTextSetString(MB200_C4_Text[3], ptString);
            }
        //-------------------------      
            //(4-12) (C1P42)AUX4 H SOL
            if(MB200_C4_AUX4_H_SOL == 0xFFFF||((gMB_IO_Value.AUX4_IO_45Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))					
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C4_Text[4], ptString);
            }
            else
            {
                MB200_C4_AUX4_H_SOL = MB200_C4_AUX4_H_SOL/1000.0;
                memset(ptString,0,10);

                sprintf(ptString,"%6.3f",MB200_C4_AUX4_H_SOL);
                ituTextSetString(MB200_C4_Text[4], ptString);    
            }
        //-------------------------  
            //(4-13) (C1P43)Float SOL
            if(MB200_C4_F_SOL==0xFF||((gMB_IO_Value.OUT1_1Byte_4Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C4_CHK[7],false);
            }
            else
            {    
                ituWidgetSetVisible(MB200_C4_CHK[7],true);   
                ituCheckBoxSetChecked(MB200_C4_CHK[7],MB200_C4_F_SOL);
            }
        //-------------------------  
            //(4-14) (C1P44)AUX2/4 select SOL
            if(MB200_C4_AUX24_SOL==0xFF||((gMB_IO_Value.OUT1_2Byte_6Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C4_CHK[8],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C4_CHK[8],true);
                ituCheckBoxSetChecked(MB200_C4_CHK[8],MB200_C4_AUX24_SOL);
            }
        //-------------------------  
            //(4-15) (C1P45)Horn relay
            if(MB200_C4_HR==0xFF||((gMB_IO_Value.OUT1_0Byte_5Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C4_CHK[9],false);
            }
            else
            {     
                ituWidgetSetVisible(MB200_C4_CHK[9],true);  
                ituCheckBoxSetChecked(MB200_C4_CHK[9],MB200_C4_HR);
            }
        //-------------------------  
            //(4-16) (C1P46)Travel shift SOL
            if(MB200_C4_T_SOL==0xFF||((gMB_IO_Value.OUT1_2Byte_4Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C4_CHK[10],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C4_CHK[10],true);
                ituCheckBoxSetChecked(MB200_C4_CHK[10],MB200_C4_T_SOL);
            }
        //-------------------------  
            //(4-17) (C1P47)AUX1 A SOL
            if(MB200_C4_AUX1_A_SOL == 0xFFFF||((gMB_IO_Value.AUX1_IO_23Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C4_Text[5], ptString);
            }
            else
            {
                //printf("MB200_C4_AUX1_A_SOL:%f\n",MB200_C4_AUX1_A_SOL);
                MB200_C4_AUX1_A_SOL = MB200_C4_AUX1_A_SOL/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(MB200_C4_AUX1_A_SOL*1000)/1000); //chpark.24.01.29
                ituTextSetString(MB200_C4_Text[5], ptString);
                //printf("AUX1 A SOL:%s  MB200_C4_AUX1_A_SOL:%f\n",ptString,MB200_C4_AUX1_A_SOL);
            }
        //-------------------------  
            //(4-18) (C1P48)AUX1 B SOL
            if(MB200_C4_AUX1_B_SOL == 0xFFFF||((gMB_IO_Value.AUX1_IO_45Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C4_Text[6], ptString);
            }
            else
            {
                MB200_C4_AUX1_B_SOL = MB200_C4_AUX1_B_SOL/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(MB200_C4_AUX1_B_SOL*1000)/1000); //chpark.24.01.29
                ituTextSetString(MB200_C4_Text[6], ptString);
            }
        }
        
        break;
        //======================================================
        //Output2
        case 1:
        {
        //-------------------------  
            //(5-1) (C1P49)AUX2 C SOL
            if(MB200_C5_AUX2_C_SOL == 0xFFFF||((gMB_IO_Value.AUX2_IO_23Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))			
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C5_Text[0], ptString);
            }
            else
            {
                f_FinalVal[0] = MB200_C5_AUX2_C_SOL/1000.0;
                f_FinalVal[1] = MB200_C5_AUX2_C_SOL/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(f_FinalVal[gMB_Unit]*1000)/1000); //chpark.24.01.29
                ituTextSetString(MB200_C5_Text[0], ptString);
            }
        //-------------------------  
            //(5-2) (C1P50)AUX2 D SOL
            if(MB200_C5_AUX2_D_SOL == 0xFFFF||((gMB_IO_Value.AUX2_IO_45Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))						
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C5_Text[1], ptString);
            }
            else
            {
                f_FinalVal[0] = MB200_C5_AUX2_D_SOL/1000.0;
                f_FinalVal[1] = MB200_C5_AUX2_D_SOL/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(f_FinalVal[gMB_Unit]*1000)/1000);//chpark.24.01.29
                ituTextSetString(MB200_C5_Text[1], ptString);
            }
        //-------------------------  
            //(5-3) (C2P31)Adjust dump SOL
            if(MB200_C5_AD_SOL == 0xFFFF||((gMB_IO_Value.OUT14_01Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))					
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C5_Text[2], ptString);
            }
            else
            {
                f_FinalVal[0] = MB200_C5_AD_SOL/1000.0;
                f_FinalVal[1] = MB200_C5_AD_SOL/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(f_FinalVal[gMB_Unit]*1000)/1000); //chpark.24.01.29
                ituTextSetString(MB200_C5_Text[2], ptString);
            }
       
            //(5-4) (C2P31)Swing L SOL
            if(MB200_C5_SL_SOL == 0xFFFF||((gMB_IO_Value.OUT10_67Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))					
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C5_Text[3], ptString);
            }
            else
            {
                f_FinalVal[0] = MB200_C5_SL_SOL/1000.0;
                f_FinalVal[1] = MB200_C5_SL_SOL/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(f_FinalVal[gMB_Unit]*1000)/1000);//chpark.24.01.29
                ituTextSetString(MB200_C5_Text[3], ptString);
            }
        //-------------------------  
            //(5-5) (C2P32)Adjust crowd SOL
            if(MB200_C5_AC_SOL == 0xFFFF||((gMB_IO_Value.OUT14_23Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))					
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C5_Text[4], ptString);
            }
            else
            {
                f_FinalVal[0] = MB200_C5_AC_SOL/1000.0;
                f_FinalVal[1] = MB200_C5_AC_SOL/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(f_FinalVal[gMB_Unit]*1000)/1000); //chpark.24.01.29
                ituTextSetString(MB200_C5_Text[4], ptString);
            }
          
             //(5-6) (C2P32))Swing R SOL
            if(MB200_C5_SR_SOL == 0xFFFF||((gMB_IO_Value.OUT10_45Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))					
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C5_Text[5], ptString);
            }
            else
            {
                f_FinalVal[0] = MB200_C5_SR_SOL/1000.0;
                f_FinalVal[1] = MB200_C5_SR_SOL/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(f_FinalVal[gMB_Unit]*1000)/1000);//chpark.24.01.29
                ituTextSetString(MB200_C5_Text[5], ptString);
            }
        //-------------------------  
            //(5-7) (C2P33)Slide light
            if(MB200_C5_SL==0xFF||((gMB_IO_Value.OUT1_3Byte_4Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C5_CHK[0],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C5_CHK[0],true);
                ituCheckBoxSetChecked(MB200_C5_CHK[0],MB200_C5_SL);
            }
        //-------------------------  
            //(5-8) (C2P34)Washer relay
            if(MB200_C5_WR==0xFF||((gMB_IO_Value.OUT1_5Byte_7Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C5_CHK[1],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C5_CHK[1],true);
                ituCheckBoxSetChecked(MB200_C5_CHK[1],MB200_C5_WR);
            }
        //-------------------------  
            //(5-9) (C2P35)Starter relay
            if(MB200_C5_SR==0xFF||((gMB_IO_Value.OUT6_5Byte_3Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))	
            {
                ituWidgetSetVisible(MB200_C5_CHK[2],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C5_CHK[2],true);
                ituCheckBoxSetChecked(MB200_C5_CHK[2],MB200_C5_SR);
            }
        //-------------------------  
            //(5-10) (C2P36)Grease pump
            if(MB200_C5_GP==0xFF||((gMB_IO_Value.OUT6_0Byte_0Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C5_CHK[3],false);
            }
            else
            {   
                ituWidgetSetVisible(MB200_C5_CHK[3],true);
                ituCheckBoxSetChecked(MB200_C5_CHK[3],MB200_C5_GP);
            }
        //-------------------------  
            //(5-11) (C2P37)Rear light
            if(MB200_C5_REL==0xFF||((gMB_IO_Value.OUT6_0Byte_3Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C5_CHK[4],false);
            }
            else
            {    
                ituWidgetSetVisible(MB200_C5_CHK[4],true);   
                ituCheckBoxSetChecked(MB200_C5_CHK[4],MB200_C5_REL);
            }
        //-------------------------  
            //(5-12) (C2P38)Room lamp
            if(MB200_C5_ROL==0xFF||((gMB_IO_Value.OUT1_0Byte_3Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C5_CHK[5],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C5_CHK[5],true);
                ituCheckBoxSetChecked(MB200_C5_CHK[5],MB200_C5_ROL);
            }
        //-------------------------  
            //(5-13) (C2P39)Travel RF SOL
            if(MB200_C5_RF_SOL == 0xFFFF||((gMB_IO_Value.OUT13_45Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C5_Text[6], ptString);
            }
            else
            {
                f_FinalVal[0] = MB200_C5_RF_SOL/1000.0;
                f_FinalVal[1] = MB200_C5_RF_SOL/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(f_FinalVal[gMB_Unit]*1000)/1000);//chpark.24.01.29
                ituTextSetString(MB200_C5_Text[6], ptString);
            }
        //-------------------------  
            //(5-14) (C2P40)Travel RB SOL
            if(MB200_C5_RB_SOL == 0xFFFF||((gMB_IO_Value.OUT13_67Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C5_Text[7], ptString);
            }
            else
            {
                f_FinalVal[0] = MB200_C5_RB_SOL/1000.0;
                f_FinalVal[1] = MB200_C5_RB_SOL/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(f_FinalVal[gMB_Unit]*1000)/1000); //chpark.24.01.29
                ituTextSetString(MB200_C5_Text[7], ptString);
            }
        //-------------------------  
            //(5-15) (C2P41)Travel LF SOL
            if(MB200_C5_LF_SOL == 0xFFFF||((gMB_IO_Value.OUT13_01Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C5_Text[8], ptString);
            }
            else
            {
                f_FinalVal[0] = MB200_C5_LF_SOL/1000.0;
                f_FinalVal[1] = MB200_C5_LF_SOL/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(f_FinalVal[gMB_Unit]*1000)/1000); //chpark.24.01.29
                ituTextSetString(MB200_C5_Text[8], ptString); 
            }
        //-------------------------  
            //(5-16) (C2P42)Travel LB SOL
            if(MB200_C5_LB_SOL == 0xFFFF||((gMB_IO_Value.OUT13_23Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C5_Text[9], ptString);
            }
            else
            {
                f_FinalVal[0] = MB200_C5_LB_SOL/1000.0;
                f_FinalVal[1] = MB200_C5_LB_SOL/1000.0;

                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(f_FinalVal[gMB_Unit]*1000)/1000); //chpark.24.01.29
                ituTextSetString(MB200_C5_Text[9], ptString);
            }
        //-------------------------  
            //(5-17) (C2P43)Beacon relay
            if(MB200_C5_BR==0xFF||((gMB_IO_Value.OUT6_0Byte_4Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C5_CHK[6],false);
            }
            else
            {     
                ituWidgetSetVisible(MB200_C5_CHK[6],true);  
                ituCheckBoxSetChecked(MB200_C5_CHK[6],MB200_C5_BR);
            }
        //-------------------------  
            //(5-18) (C2P44)Wiper
            if(MB200_C5_WI==0xFF||((gMB_IO_Value.OUT1_0Byte_4Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C5_CHK[7],false);
            }
            else
            {    
                ituWidgetSetVisible(MB200_C5_CHK[7],true);   
                ituCheckBoxSetChecked(MB200_C5_CHK[7],MB200_C5_WI);
            
            }
        //-------------------------      
            //(5-18) (C2P46) - 
        //-------------------------      

        }
        break;

        //==========================================================
        //Output3
        case 2:
        {
            //(6-1) (C2P45))High flow SOL
            if(MB200_C6_HF_SOL==0xFF||((gMB_IO_Value.OUT1_3Byte_7Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C6_CHK[0],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C6_CHK[0],true);
                ituCheckBoxSetChecked(MB200_C6_CHK[0],MB200_C6_HF_SOL);
            }
            
            //(6-2) (C2P46))Push switch relay
            if(MB200_C6_PSR==0xFF||((gMB_IO_Value.OUT6_7Byte_4Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C6_CHK[1],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C6_CHK[1],true);
                ituCheckBoxSetChecked(MB200_C6_CHK[1],MB200_C6_PSR);
            }

            //(6-3) (C2P47)Tail Lamp
            if(MB200_C6_TL==0xFF||((gMB_IO_Value.OUT6_0Byte_6Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C6_CHK[2],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C6_CHK[2],true);
                ituCheckBoxSetChecked(MB200_C6_CHK[2],MB200_C6_TL);
            }

            //(6-4) (C2P48)Back buzzeer
            if(MB200_C6_BB==0xFF||((gMB_IO_Value.OUT1_1Byte_7Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C6_CHK[3],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C6_CHK[3],true);
                ituCheckBoxSetChecked(MB200_C6_CHK[3],MB200_C6_BB);
            }


            //(6-5) (C2P49)Blade up SOL
            if(MB200_C6_BU_SOL == 0xFFFF||((gMB_IO_Value.OUT10_01Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C6_Text[0], ptString);
            }
            else
            {
                f_FinalVal[0] = MB200_C6_BU_SOL/1000.0;
                f_FinalVal[1] = MB200_C6_BU_SOL/1000.0;
               
                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(f_FinalVal[gMB_Unit]*1000)/1000); //chpark.24.01.29
                ituTextSetString(MB200_C6_Text[0], ptString); 
            }

            //(6-6) (C2P50)Blade down SOL
            if(MB200_C6_BD_SOL == 0xFFFF||((gMB_IO_Value.OUT10_23Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C6_Text[1], ptString);
            }
            else
            {
                f_FinalVal[0]= MB200_C6_BD_SOL/1000.0;
                f_FinalVal[1]= MB200_C6_BD_SOL/1000.0;
               
                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(f_FinalVal[gMB_Unit]*1000)/1000); //chpark.24.01.29
                ituTextSetString(MB200_C6_Text[1], ptString);
            }

            ////////OX Module
            //(6-7) (C1P06)Extension out 1/5A
            if(MB200_C6_EO15A == 0xFFFF||((gMB_IO_Value.OUT20_01Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C6_Text[2], ptString);
            }
            else
            {
                f_FinalVal[0] = MB200_C6_EO15A/1000.0;
                f_FinalVal[1] = MB200_C6_EO15A/1000.0;
               
                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(f_FinalVal[gMB_Unit]*1000)/1000); //chpark.24.10.18
                ituTextSetString(MB200_C6_Text[2], ptString); 
            }

            //(6-8) (C1P07)(C1P06)Extension out 2/5B
            if(MB200_C6_EO25B == 0xFFFF||((gMB_IO_Value.OUT20_23Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))				
            {
                memset(ptString,0,10);
                sprintf(ptString,"N/A  ");
                ituTextSetString(MB200_C6_Text[3], ptString);
            }
            else
            {
                f_FinalVal[0]= MB200_C6_EO25B/1000.0;
                f_FinalVal[1]= MB200_C6_EO25B/1000.0;
               
                memset(ptString,0,10);
                sprintf(ptString,"%6.3f",round(f_FinalVal[gMB_Unit]*1000)/1000); //chpark.24.10.18
                ituTextSetString(MB200_C6_Text[3], ptString);
            }

            //(6-9) (C1P08))Extension out 3
            if(MB200_C6_EO3==0xFF||((gMB_IO_Value.OUT6_5Byte_6Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C6_CHK[4],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C6_CHK[4],true);
                ituCheckBoxSetChecked(MB200_C6_CHK[4],MB200_C6_EO3);//chpark.24.10.18
            }
            
            //(6-10) (C1P09))Extension out 4
            if(MB200_C6_EO4==0xFF||((gMB_IO_Value.OUT6_5Byte_7Time +TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m))
            {
                ituWidgetSetVisible(MB200_C6_CHK[5],false);
            }
            else
            {       
                ituWidgetSetVisible(MB200_C6_CHK[5],true);
                ituCheckBoxSetChecked(MB200_C6_CHK[5],MB200_C6_EO4);//chpark.24.10.18
            }
        }
        break;
    }
}

void MenuB200_SetMenuTab(uint8_t nIndex)
{
    //printf("\r\n MenuB200_SetMenuTab---12--%d\r\n",nIndex);
    nMenuB200_MenuIndex  = nIndex;
    for(int i=0;i<MB200_END;i++)
    {
        ituWidgetSetVisible(MB200_CT[i],false);
    }

    MenuB200_GetMenuData(nIndex);
    ituWidgetSetVisible(MB200_CT[nIndex],true);
    MenuB200_UpdateMenuUI(nIndex);

    MenuB200_SetLang(gMB_Language);//chpark.09.26
}


bool MB200_D7_BTNMouseUp(ITUWidget* widget, char* param)
{
    //printf("\r\n MB200_D7_BTNMouseUp---1222--%d\r\n",nMenuB200_MenuIndex);
    MB200_MenuTabNext();
    
    return true;
}

bool MB200_D6_BTNMouseUp(ITUWidget* widget, char* param)
{
    GotoMain(MAIN_MODE_NORMAL); 
    return true;
}

bool MB200_D5_BTNMouseUp(ITUWidget* widget, char* param)
{
    ITULayer *MenuBTop_Layer = ituSceneFindWidget(&theScene, "MenuBTop_Layer");
    assert(MenuBTop_Layer);
    ituLayerGoto(MenuBTop_Layer);

    return true;
}



bool MenuB200_Timer(ITUWidget* widget, char* param)
{
   #ifdef USED_ROTATION  //chpark.10.19
   if (RotationMode!=0)
   {
      return 0;
   }
   #endif

	if((gMainTimerCnt_10m - MB200_gMT_Timer) >= 50)		// 0.5sec
	{
		MB200_gMT_Timer = gMainTimerCnt_10m;
		MB200_gMT_ExitTimer++;
		
		if(MB200_gMT_ExitTimer >= 60)
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

        MenuB200_GetMenuData(nMenuB200_MenuIndex);
        MenuB200_UpdateMenuUI(nMenuB200_MenuIndex);
	}
	
    return true;
}


bool MB200_MenuTabNext()
{
    nMenuB200_MenuIndex ++;
    if(nMenuB200_MenuIndex>=MB200_END)nMenuB200_MenuIndex=MB200_IO1;
    MenuB200_SetMenuTab(nMenuB200_MenuIndex);
}

bool MB200_MenuTabPrev()
{
    nMenuB200_MenuIndex --;
    if(nMenuB200_MenuIndex<0)nMenuB200_MenuIndex=MB200_IO6;
    MenuB200_SetMenuTab(nMenuB200_MenuIndex);
}


bool MenuB200_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t JogDir = 0;
    JogDir = param[0];
    MB200_gMT_ExitTimer = 0;

	//printf("MenuB200 JogWheel %d\r\n",JogDir);
	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
        case LANDSCAPE_1:
		case LANDSCAPE_2:
            if(JogDir == JOG_DIAL_ROT_RIGHT)
			{
                MB200_MenuTabNext();	
			}
			else if(JogDir == JOG_DIAL_ROT_LEFT)
			{
                MB200_MenuTabPrev();
			}

		break;
	}

    return true;
}


bool MenuB200_JogButton(ITUWidget* widget, char* param)
{
	uint8_t JogButton = 0;
    JogButton = param[0];
    MB200_gMT_ExitTimer = 0;

	//printf("MenuB100_JogButton %d\r\n",JogButton);
	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
        case LANDSCAPE_1:
		case LANDSCAPE_2:
			//printf("PORTRATE\r\n");
			switch(JogButton)
			{
				case JOG_DIAL_KEY_UP:
                case JOG_DIAL_KEY_LEFT:
                    MB200_MenuTabPrev();					
				    break;

				case JOG_DIAL_KEY_DOWN:
                case JOG_DIAL_KEY_RIGHT:
                    MB200_MenuTabNext();						
				    break;
				
				case JOG_DIAL_KEY_ENTER_OFF:
                    MB200_MenuTabNext();
				    break;
			}
		break;
	}

    return true;
}

bool MenuB200_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t JogSwitch = 0;
	JogSwitch = param[0];
	MB200_gMT_ExitTimer = 0;

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
