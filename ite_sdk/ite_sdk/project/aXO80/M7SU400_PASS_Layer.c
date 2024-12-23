#include <assert.h>

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

static bool m_bSU400_AUI1_Rcv_Error = false;//chpark.24.02.14

//chpark.12.19
#define SU400_DEL_AUTO_OFF  0
#define SU400_DEL_AUTO_START 1
#define SU400_DEL_AUTO_ON 2

static uint32_t SU400_FastTimer;
static uint8_t SU400_FastFlag = SU400_DEL_AUTO_OFF;
static uint8_t SU400_1SecCNT  = 0;
bool SU400_Delete_Char(int nMode);

static bool SU400_bLongPress_Start = false;
//chpark.11.20
//char *g_SU400_MSG_TEXT_ERROR_EXIST[5];//"The passcode is already used."
//char *g_SU400_MSG_TEXT_ERROR_MATCH[5];//"The passcode confirmation does not match."
//char *g_SU400_MSG_TEXT_ERROR_DIGIT[5];//"The passcode must be between 4 and 9 characters."

static ITUWidget* SU400_ERROR_TEXT;//chpark.11.20

uint8_t SU400_UM5_0Byte_01bit;		//chpark.10.30

static bool     m_bSU400ReadyToJOG = false;
static int m_nSU400_ButtonIndex=0;//current selected key index. 0:skip 1-10:key 11:del 12:D7 

static bool m_bSU400_GoTo_LayerMode = false;
static int  m_nSU400_GoTo_LayerModeCnt=0;

static uint32_t SU400_SecTimer;
static uint32_t SU400_ExitCNT = 0;
static uint32_t SU400_SendCNT = 0;

uint32_t SU400_CursorCNT = 0;
bool m_bSU400_CursorShow = true;

static int SU400_PC_MODE = 0;//passcode mode. 0:new passcode  1: confirm passcode.

#define SU400_MAX_PW_CNT	9

#define SU400_SKIP_KEY      0
#define SU400_DEL_Key       11
#define SU400_D7_Key        12
#define SU400_NEW_PASS_BTN  20
#define SU400_CF_PASS_BTN   21

#define SU400_LANG_ENG 0
#define SU400_LANG_GER 1
#define SU400_LANG_FR 2
#define SU400_LANG_IT 3
#define SU400_LANG_SP 4
#define SU400_LANG_TXT 5

#define SU400_SUCCESS		1
#define SU400_FAIL			0

#define SU400_NEW_PASSMODE  0
#define SU400_CF_PASSMODE   1

#define SU400_RCV_INIT          0
#define SU400_RCV_PASSCODE_CHK  1

int PASSCODE_KEY[13];

uint32_t SU400_Passcode_InNumber = 0;
static char SU400_TxtUserName[50];
static char SU400_Txt_NewPassCode[MAX_SU100_USER_PW];
static char SU400_Txt_ConfirmPassCode[MAX_SU100_USER_PW];
//=====================================
static ITUWidget* SU400_TITLE[5]; //
//=====================================
static ITUWidget* SU400_TOP_ICON[2]; //
static ITUWidget* SU400_TOP_NAME; //
//=====================================
static ITUWidget* SU400_CONT_ERROR_TXT; //The passcode Confirmation does not match
static ITUWidget* SU400_ERROR_TXT[6];   //
//=====================================
static ITUWidget* SU400_CONT_ERROR_1_TXT; //The passcode must be between 4 and 9 Characters
static ITUWidget* SU400_ERROR_1_TXT[6];   //
//=====================================
static ITUWidget* SU400_CONT_ERROR_2_TXT; //The passcode is already used.
static ITUWidget* SU400_ERROR_2_TXT[5];   //
//=====================================
static ITUWidget* SU400_CONT_RESULT; //
static ITUWidget* SU400_RESULT_ICON[2]; //
//=====================================
static ITUWidget* SU400_CONT_SKIP_BTN; //
static ITUWidget* SU400_SKIP_BTN[5]; //
static ITUWidget* SU400_SKIP_SEL; //
static ITUWidget* SU400_SKIP_DOWN; //chpark.01.15
//=====================================
static ITUWidget* SU400_CONT_TXT_PWD; //
static ITUWidget* SU400_NEW_PWD_TXT[6]; //
//=====================================
static ITUWidget* SU400_CONT_TXT_CFPWD; //
static ITUWidget* SU400_CF_PWD_TXT[6]; //
//=====================================
static ITUWidget* SU400_CONT_UserPW_Data;
static ITUWidget* SU400_CONT_UserPW_Bt;
static ITUWidget* SU400_CONT_UserPW_Sel;

static ITUIcon* SU400_UserPW_Data[SU400_MAX_PW_CNT];
static ITUIcon* SU400_UserPX_Data[SU400_MAX_PW_CNT];

static ITUIcon* SU400_UserPW_CF_Data[SU400_MAX_PW_CNT];//chpark.11.02
static ITUIcon* SU400_UserPX_CF_Data[SU400_MAX_PW_CNT];//chpark.11.02
static ITUIcon* SU400_Cursor[2];            //chpark.11.02 (0:blue 1:black)
static ITUWidget* SU400_New_PW_BTN;         //chpark.11.02 
static ITUWidget* SU400_Confirm_PW_BTN;     //chpark.11.02 


static ITUIcon* SU400_UserPW_Sel[15];
static ITUIcon* SU400_UserPW_Bt[15];

static ITUWidget* SU400_ICON_DOWN;
//=====================================
static ITUWidget* SU400_ICON_D7_SEL;
static ITUWidget* SU400_ICON_D7_DOWN;
//=====================================
int SU400_Name_Char_Index=0;
uint8_t SU400_Key_SelIndex=0xFF;
//=====================================

//=====================================
void SU400_MenuTabPrev();
void SU400_MenuTabNext();
void SU400_MenuTabUp();
void SU400_MenuTabDown();
void SU400_MenuTabLeft();
void SU400_MenuTabRight();
void SU400_MenuTabESC();
void SU400_MenuTabEnter();
//=================================

void SU400_SetUserTitle(uint8_t tUserIdx);
void SU400_InitControl();
void SU400_InitKeyMaping();

void SU400_Display_PassCode(int nMode);
void SU400_Show_InCenter(int nMode,int nLength);
void SU400_Show_InLeft(int nMode,uint8_t nLength);
bool SU400_Button_Select(int nIndex,bool bDownImage);

void SU400_Chk_Result_InServer(int nSuc);
void SU400_Show_Result_UI(int nRes);

void SU400_Change_Passcode_Mode(int nMode);
void SU400_Passcode_Clear(int nMode);
int  SU400_Passcode_Check();

bool SU400_Send_TX(int nType);
static uint8_t SU400_UM5_Rcv_Type=SU400_RCV_INIT;

bool    m_bSU400_ErrorMsg_Show = false;//chpark.11.02
int     SU400_ErrorMsg_Cnt = 0;//chpark.11.02
int     SU400_Result_UI_Status; //-1:none  0:X  1:O  2:X+text  3:O+text 

static bool SU400_b_VMode = false;//chpark.01.15

bool SU400_Enter(ITUWidget* widget, char* param)
{
    if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
	{
        SU400_b_VMode = false;
    }
    else
    {   
        SU400_b_VMode = true;
    }

    m_bSU400_GoTo_LayerMode = false;
    m_nSU400_GoTo_LayerModeCnt = 0;

    SU400_CursorCNT=0;
    SU400_ErrorMsg_Cnt=0;
    m_bSU400_ErrorMsg_Show = false;

    SU400_SecTimer = gMainTimerCnt_10m;
	SU400_ExitCNT = 0;

    SU400_PC_MODE = SU400_NEW_PASSMODE;

    SU400_Result_UI_Status = -1;//none

    int i=0,j,tIndex=0;
    m_nSU400_ButtonIndex = 0;
    SU400_UM5_Rcv_Type=SU400_RCV_INIT;

    memset(SU400_TxtUserName, '\0', sizeof(SU400_TxtUserName));
    memset(SU400_Txt_NewPassCode, '\0', sizeof(SU400_Txt_NewPassCode));
    memset(SU400_Txt_ConfirmPassCode, '\0', sizeof(SU400_Txt_ConfirmPassCode));
    
    if(g_nSU100_Mode==SU100_Add_Mode)
    {
       printf("SU400_Enter==SU100_Add_Mode: g_nSU100_AddUserIdx=%d  CurPassCode=%s\n",g_nSU100_AddUserIdx,User_RegInfo[g_nSU100_AddUserIdx].UserPassCode);
    }
    else if(g_nSU100_Mode==SU100_Edit_Mode)
    {
        printf("SU400_Enter==SU100_Edit_Mode: g_nSU100_EditUserIdx=%d\n",g_nSU100_EditUserIdx);
    }

    SU400_InitControl();
    
    SU400_InitKeyMaping();
    SU400_SetUserTitle(g_nSU100_CurUserIdx);

    //init passcode mode. (new passcode mode)
    //init passcode.
    SU400_Display_PassCode(SU400_NEW_PASSMODE);

    SU400_Change_Passcode_Mode(SU400_NEW_PASSMODE);

    //init key point
    SU400_Button_Select(1,false);

    m_bSU400ReadyToJOG = true;

    //chpark.24.03.08
    if(g_nSU100_Mode==SU100_Add_Mode)
    {
        SU400_Send_TX(0);
    }
    //

    //chpark.12.19
    SU400_1SecCNT = 100;
	SU400_FastFlag = SU400_DEL_AUTO_OFF;

    m_bSU400_AUI1_Rcv_Error = false;        //chpark.24.02.14
    UserPW_Time_AUI1 = gMainTimerCnt_10m;	//chpark.24.02.14

    return true;
}

void SU400_InitControl()
{
    int tIndex=0;
    char tmp_WidgetName[100];
//Text setting    
    //g_SU400_MSG_TEXT_ERROR_EXIST[SU400_LANG_ENG] = "The passcode is already used.";
    //g_SU400_MSG_TEXT_ERROR_EXIST[SU400_LANG_GER] = "Der Passcode wird bereits verwendet.";
    //g_SU400_MSG_TEXT_ERROR_EXIST[SU400_LANG_FR]  = "Le code est déjà utilisé.";
    //g_SU400_MSG_TEXT_ERROR_EXIST[SU400_LANG_IT]  = "Il passcode è già utilizzato.";
    //g_SU400_MSG_TEXT_ERROR_EXIST[SU400_LANG_SP]  = "El código de acceso ya se ha utilizado.";

    //g_SU400_MSG_TEXT_ERROR_MATCH[SU400_LANG_ENG] = "The passcode confirmation does not match.";
    //g_SU400_MSG_TEXT_ERROR_MATCH[SU400_LANG_GER] = "Die beiden Passcodes stimmen nicht überein.";
    //g_SU400_MSG_TEXT_ERROR_MATCH[SU400_LANG_FR]  = "La confirmation du code ne correspond pas.";
    //g_SU400_MSG_TEXT_ERROR_MATCH[SU400_LANG_IT]  = "Il passcode confermato non corrisponde.";
    //g_SU400_MSG_TEXT_ERROR_MATCH[SU400_LANG_SP]  = "El código de acceso debe tener entre 4 y 9 caracteres.";

    //g_SU400_MSG_TEXT_ERROR_DIGIT[SU400_LANG_ENG] = "The passcode must be between 4 and 9 characters.";
    //g_SU400_MSG_TEXT_ERROR_DIGIT[SU400_LANG_GER] = "Der Passcode muss zwischen 4 und 9 Zeichen haben.";
    //g_SU400_MSG_TEXT_ERROR_DIGIT[SU400_LANG_FR]  = "Le code doit compter entre 4 et 9 caractères.";
    //g_SU400_MSG_TEXT_ERROR_DIGIT[SU400_LANG_IT]  = "Il passcode deve contenere da 4 a 9 caratteri.";
    //g_SU400_MSG_TEXT_ERROR_DIGIT[SU400_LANG_SP]  = "El código de acceso debe tener entre 4 y 9 caracteres.";
//Control setting
//printf("SU400===========11111\n");
    SetWidget(SU400_TOP_ICON[SU100_ICON_AD],"SU400_T_Icon_Admin");
	SetWidget(SU400_TOP_ICON[SU100_ICON_ST],"SU400_T_Icon_Std");
	SetWidget(SU400_TOP_NAME,"SU400_T_Name");

    SetWidget(SU400_TITLE[SU400_LANG_ENG],"SU400_TITLE_ENG");
    SetWidget(SU400_TITLE[SU400_LANG_GER],"SU400_TITLE_GER");
    SetWidget(SU400_TITLE[SU400_LANG_FR], "SU400_TITLE_FR");
    SetWidget(SU400_TITLE[SU400_LANG_IT], "SU400_TITLE_IT");
    SetWidget(SU400_TITLE[SU400_LANG_SP], "SU400_TITLE_SP");
//printf("SU400===========22222\n");
    SetWidget(SU400_CONT_SKIP_BTN,"SU400_CONT_SKIP_BTN");
    SetWidget(SU400_SKIP_BTN[SU400_LANG_ENG],"SU400_SKIP_BTN_ENG");
    SetWidget(SU400_SKIP_BTN[SU400_LANG_GER],"SU400_SKIP_BTN_GER");
    SetWidget(SU400_SKIP_BTN[SU400_LANG_FR],"SU400_SKIP_BTN_FR");
    SetWidget(SU400_SKIP_BTN[SU400_LANG_IT],"SU400_SKIP_BTN_IT");
    SetWidget(SU400_SKIP_BTN[SU400_LANG_SP],"SU400_SKIP_BTN_SP");
   
    SetWidget(SU400_SKIP_SEL,"SU400_SKIP_SEL");
    SetWidget(SU400_SKIP_DOWN,"SU400_SKIP_DOWN");
//printf("SU400===========22222\n");
    SetWidget(SU400_CONT_ERROR_TXT, "SU400_CONT_ERROR_TXT");
    SetWidget(SU400_ERROR_TXT[SU400_LANG_ENG], "SU400_ERROR_ENG");
    SetWidget(SU400_ERROR_TXT[SU400_LANG_GER], "SU400_ERROR_GER");
    SetWidget(SU400_ERROR_TXT[SU400_LANG_FR],  "SU400_ERROR_FR");
    SetWidget(SU400_ERROR_TXT[SU400_LANG_IT],  "SU400_ERROR_IT");
    SetWidget(SU400_ERROR_TXT[SU400_LANG_SP],  "SU400_ERROR_SP");
    SetWidget(SU400_ERROR_TXT[SU400_LANG_TXT],  "SU400_ERROR_TXT");

    SetWidget(SU400_CONT_ERROR_1_TXT, "SU400_CONT_ERROR_1_TXT");
    SetWidget(SU400_ERROR_1_TXT[SU400_LANG_ENG], "SU400_ERROR_1_ENG");
    SetWidget(SU400_ERROR_1_TXT[SU400_LANG_GER], "SU400_ERROR_1_GER");
    SetWidget(SU400_ERROR_1_TXT[SU400_LANG_FR],  "SU400_ERROR_1_FR");
    SetWidget(SU400_ERROR_1_TXT[SU400_LANG_IT],  "SU400_ERROR_1_IT");
    SetWidget(SU400_ERROR_1_TXT[SU400_LANG_SP],  "SU400_ERROR_1_SP");
    SetWidget(SU400_ERROR_1_TXT[SU400_LANG_TXT],  "SU400_ERROR_1_TXT");

    SetWidget(SU400_CONT_ERROR_2_TXT, "SU400_CONT_ERROR_2_TXT");
    SetWidget(SU400_ERROR_2_TXT[SU400_LANG_ENG], "SU400_ERROR_2_ENG");
    SetWidget(SU400_ERROR_2_TXT[SU400_LANG_GER], "SU400_ERROR_2_GER");
    SetWidget(SU400_ERROR_2_TXT[SU400_LANG_FR],  "SU400_ERROR_2_FR");
    SetWidget(SU400_ERROR_2_TXT[SU400_LANG_IT],  "SU400_ERROR_2_IT");
    SetWidget(SU400_ERROR_2_TXT[SU400_LANG_SP],  "SU400_ERROR_2_SP");
//printf("SU400===========33333\n");
    SetWidget(SU400_CONT_RESULT, "SU400_CONT_RESULT");
    SetWidget(SU400_RESULT_ICON[SU400_SUCCESS],"SU400_OK");
    SetWidget(SU400_RESULT_ICON[SU400_FAIL],   "SU400_NO");
//printf("SU400===========44444\n");
    SetWidget(SU400_CONT_TXT_PWD,"SU400_CONT_TXT_PWD");
    SetWidget(SU400_NEW_PWD_TXT[SU400_LANG_ENG],"SU400_NEW_PWD_ENG");
    SetWidget(SU400_NEW_PWD_TXT[SU400_LANG_GER],"SU400_NEW_PWD_GER");
    SetWidget(SU400_NEW_PWD_TXT[SU400_LANG_FR], "SU400_NEW_PWD_FR");
    SetWidget(SU400_NEW_PWD_TXT[SU400_LANG_IT], "SU400_NEW_PWD_IT");
    SetWidget(SU400_NEW_PWD_TXT[SU400_LANG_SP], "SU400_NEW_PWD_SP");
    SetWidget(SU400_NEW_PWD_TXT[SU400_LANG_TXT], "SU400_NEW_PWD_TXT");//chpark.11.20
//printf("SU400===========44444\n");
    SetWidget(SU400_CONT_TXT_CFPWD,"SU400_CONT_TXT_CFPWD");
    SetWidget(SU400_CF_PWD_TXT[SU400_LANG_ENG],"SU400_CF_PWD_ENG");
    SetWidget(SU400_CF_PWD_TXT[SU400_LANG_GER],"SU400_CF_PWD_GER");
    SetWidget(SU400_CF_PWD_TXT[SU400_LANG_FR], "SU400_CF_PWD_FR");
    SetWidget(SU400_CF_PWD_TXT[SU400_LANG_IT], "SU400_CF_PWD_IT");
    SetWidget(SU400_CF_PWD_TXT[SU400_LANG_SP], "SU400_CF_PWD_SP");
    SetWidget(SU400_CF_PWD_TXT[SU400_LANG_TXT], "SU400_CF_PWD_TXT");//chpark.11.20
//printf("SU400=========== StarMark\n");
    //ODD//9 unit//chpark.11.02
    for (int i=0;i<SU400_MAX_PW_CNT;i++)
	{
        //new passcode
        memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
		sprintf(tmp_WidgetName, "SU400_UserPW_Data%d",i+1);
		SetWidget(SU400_UserPW_Data[i],tmp_WidgetName);
        ituWidgetSetVisible(SU400_UserPW_Data[i], false);
        //confirm passcode
        memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
		sprintf(tmp_WidgetName, "SU400_UserPW_CF_Data%d",i+1);
		SetWidget(SU400_UserPW_CF_Data[i],tmp_WidgetName);
        ituWidgetSetVisible(SU400_UserPW_CF_Data[i], false);
    }
    //EVEN//8 unit
    for (int i=0;i<SU400_MAX_PW_CNT-1;i++)
	{
        //new passcode
        memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
		sprintf(tmp_WidgetName, "SU400_UserPX_Data%d",i+1);
		SetWidget(SU400_UserPX_Data[i],tmp_WidgetName);
        ituWidgetSetVisible(SU400_UserPX_Data[i], false);
        //confirm passcode
        memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
		sprintf(tmp_WidgetName, "SU400_UserPX_CF_Data%d",i+1);
		SetWidget(SU400_UserPX_CF_Data[i],tmp_WidgetName);
        ituWidgetSetVisible(SU400_UserPX_CF_Data[i], false);
	}
    //chpark.11.02
    SetWidget(SU400_Cursor[0],   "SU400_Cursor_blue");
    SetWidget(SU400_Cursor[1],   "SU400_Cursor_black");
    SetWidget(SU400_New_PW_BTN,   "SU400_New_PW_BTN");
    SetWidget(SU400_Confirm_PW_BTN,   "SU400_Confirm_PW_BTN");
    //================================
    
    for (int i=0;i<11;i++)
	{
        memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
		sprintf(tmp_WidgetName, "SU400_UserPW_Sel%02d",i+1);
		SetWidget(SU400_UserPW_Sel[i],tmp_WidgetName);
		sprintf(tmp_WidgetName, "SU400_UserPW_Bt%02d",i+1);
		SetWidget(SU400_UserPW_Bt[i],tmp_WidgetName);

        ituWidgetSetVisible(SU400_UserPW_Sel[i], false);
		ituWidgetSetVisible(SU400_UserPW_Bt[i],  true);
        
        //printf("SU400_UserPW_Bt==%d\n",i);
	}

    SetWidget(SU400_ICON_DOWN,   "SU400_ICON_DOWN");
     
    SetWidget(SU400_ICON_D7_SEL, "SU400_ICON_D7_SEL");
    SetWidget(SU400_ICON_D7_DOWN,"SU400_ICON_D7_DOWN");

    SetWidget(SU400_ERROR_TEXT,"SU400_ERROR_TEXT");//chpark.11.20
    //================================================================ 

// UI setting
    ituWidgetSetVisible(SU400_CONT_SKIP_BTN,    true);  //skip button
    ituWidgetSetVisible(SU400_CONT_ERROR_TXT,   false);   //error msg
    ituWidgetSetVisible(SU400_CONT_ERROR_1_TXT,   false); //error msg
    ituWidgetSetVisible(SU400_CONT_ERROR_2_TXT,   false); //error msg//chpark.24.01.31
    
    ituWidgetSetVisible(SU400_CONT_TXT_PWD,       true); //new passcode
    ituWidgetSetVisible(SU400_CONT_TXT_CFPWD,     true);//confirm passcode

    for(int i=0; i<5; i++)
    {
        ituWidgetSetVisible(SU400_TITLE[i],       false);
        ituWidgetSetVisible(SU400_SKIP_BTN[i],    false);
        ituWidgetSetVisible(SU400_NEW_PWD_TXT[i], false);
        ituWidgetSetVisible(SU400_CF_PWD_TXT[i],  false);
    }

    ituWidgetSetVisible(SU400_TITLE[Lang_Flag[M7_UserIndex]], true);  //title
    ituWidgetSetVisible(SU400_SKIP_BTN[Lang_Flag[M7_UserIndex]] ,    true);  
    ituWidgetSetVisible(SU400_CONT_RESULT, false);//result image (o,X)
    //
    ituWidgetSetVisible(SU400_ICON_DOWN,     false);
    ituWidgetSetVisible(SU400_ICON_D7_SEL,   false);
    ituWidgetSetVisible(SU400_ICON_D7_DOWN,  false);  

    ituWidgetSetVisible(SU400_Cursor[0], m_bSU400_CursorShow); 
    //
    ituWidgetSetVisible(SU400_ERROR_TEXT,  false);  //chpark.11.20
    //
}

void SU400_InitKeyMaping()
{
    //current selected key index. 0:skip 1-10:key 11:del 12:D7 
    PASSCODE_KEY[0] =   0;
    PASSCODE_KEY[1] =   4901;
    PASSCODE_KEY[2] =   5002;
    PASSCODE_KEY[3] =   5103;
    PASSCODE_KEY[4] =   5204;
    PASSCODE_KEY[5] =   5305;
    PASSCODE_KEY[6] =   5406;
    PASSCODE_KEY[7] =   5507;
    PASSCODE_KEY[8] =   5608;
    PASSCODE_KEY[9] =   5709;
    PASSCODE_KEY[10] =  4810;
    PASSCODE_KEY[11] =  12711;//del
    PASSCODE_KEY[12] =  100012;//D7
}

void SU400_SetUserTitle(uint8_t tUserIdx)
{
	uint8_t i;
	uint8_t tDestName[100];

	//Set Icon
	if(User_RegInfo[tUserIdx].RegFlag==SU100_USER_AD )
	{
		ituWidgetSetVisible(SU400_TOP_ICON[SU100_ICON_AD],true);
		ituWidgetSetVisible(SU400_TOP_ICON[SU100_ICON_ST],false);
	}
	else if(User_RegInfo[tUserIdx].RegFlag==SU100_USER_ST)
	{
		ituWidgetSetVisible(SU400_TOP_ICON[SU100_ICON_AD],false);
		ituWidgetSetVisible(SU400_TOP_ICON[SU100_ICON_ST],true);
	}
    else
    {
        ituWidgetSetVisible(SU400_TOP_ICON[SU100_ICON_AD],false);
		ituWidgetSetVisible(SU400_TOP_ICON[SU100_ICON_ST],false);
    }

    if(g_UI1_ChkTime>0)
	{
        memset(tDestName,0,100);

		ituWidgetSetVisible(SU400_TOP_NAME,true);
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[tUserIdx].UserName);
		ituTextSetString(SU400_TOP_NAME, tDestName);
	}
	else if((g_UI1_ChkTime == 0)||(M7_UserIndex > (MAX_SU100_USER_NUM-1)))
	{
		ituWidgetSetVisible(SU400_TOP_NAME,false);
	}

}

bool SU400_Button_Select(int nIndex,bool bDownImage)
{
    //printf("[SU400_Button_Select] ========\n");
    int nTmpPrevIndex = m_nSU400_ButtonIndex;
   //save new key index
    m_nSU400_ButtonIndex = nIndex;

    //===================
    //clear
    for(int i=0;i<11;i++)
    {
        ituWidgetSetVisible(SU400_UserPW_Sel[i], false);
    }
    ituWidgetSetVisible(SU400_SKIP_SEL,     false);
    ituWidgetSetVisible(SU400_SKIP_DOWN,    false);//chpark.01.31
    ituWidgetSetVisible(SU400_ICON_D7_SEL,  false);
    ituWidgetSetVisible(SU400_ICON_DOWN, false);
    //===================

    //select button
    int x, y;
    if(nIndex==SU400_SKIP_KEY)
    {
        //skip button
        ituWidgetSetVisible(SU400_SKIP_SEL,     true);
        ituWidgetSetVisible(SU400_SKIP_DOWN,    true);

        if(bDownImage)
        {
            ituWidgetSetVisible(SU400_SKIP_DOWN, true);
        }
        else
        {
            ituWidgetSetVisible(SU400_SKIP_DOWN, false);
        }
    }
    else if(nIndex>=1 && nIndex<=11)
    {
        //printf("[%d]Button POS[1] : x=%d y=%d========\n",nIndex,x,y);
        ituWidgetSetVisible(SU400_UserPW_Sel[nIndex-1], true);

        ituWidgetGetGlobalPosition(SU400_UserPW_Bt[nIndex-1], &x, &y);
            
        if(bDownImage)
        {
            ituWidgetSetVisible(SU400_ICON_DOWN, true);
            //ituWidgetSetPosition(SU400_ICON_DOWN,x-6, y-6);
            ituWidgetSetPosition(SU400_ICON_DOWN,x, y);
        }
        else
        {
            ituWidgetSetVisible(SU400_ICON_DOWN, false);
        }

        ituWidgetSetVisible(SU400_ICON_D7_SEL, false);
        ituWidgetSetVisible(SU400_ICON_D7_DOWN, false);
            
        //printf("[%d]Button POS[2] : x=%d y=%d========\n",nIndex,x,y);
    }
    else if(nIndex==SU400_D7_Key)
    {
        //D7 Button
        ituWidgetSetVisible(SU400_ICON_D7_SEL,  true);
        if(bDownImage)
        {
            ituWidgetSetVisible(SU400_ICON_D7_DOWN, true);
        }
        else
        {
            ituWidgetSetVisible(SU400_ICON_D7_DOWN, false);
        }
    }
    else
    {
        printf("[SU400_Button_Select] Select Error=%d \n",SU400_Button_Select);
    }

    return true;
}

//======================================================
void SU400_Display_PassCode(int nMode)
{
    int len;
    
    if(SU400_PC_MODE==SU400_NEW_PASSMODE)
    {
        //clear
        for (int i=0;i<SU400_MAX_PW_CNT;i++)
	    {
		    ituWidgetSetVisible(SU400_UserPW_Data[i], false);
	    }
        for (int i=0;i<SU400_MAX_PW_CNT-1;i++)
	    {
		    ituWidgetSetVisible(SU400_UserPX_Data[i], false);
	    }
        //calculate num of passcode character
        len = strlen(SU400_Txt_NewPassCode);
        //SU400_Show_InCenter(SU400_NEW_PASSMODE,len);
        SU400_Show_InLeft(SU400_NEW_PASSMODE,len);
        
        //printf("\n[SU400_Display][New][PassCode]: %s ========\n",SU400_Txt_NewPassCode);
    }
    else if(SU400_PC_MODE==SU400_CF_PASSMODE)
    {
        //printf("\n[SU400_Display][CF][PassCode][1]: %s ========\n",SU400_Txt_ConfirmPassCode);
        //clear
        for (int i=0;i<SU400_MAX_PW_CNT;i++)
	    {
		    ituWidgetSetVisible(SU400_UserPW_CF_Data[i], false);
	    }
        for (int i=0;i<SU400_MAX_PW_CNT-1;i++)
	    {
		    ituWidgetSetVisible(SU400_UserPX_CF_Data[i], false);
	    }
        //calculate num of passcode character
        len = strlen(SU400_Txt_ConfirmPassCode);
        //SU400_Show_InCenter(SU400_CF_PASSMODE,len);
        SU400_Show_InLeft(SU400_CF_PASSMODE,len);

        //printf("\n[SU400_Display][CF][PassCode][2]: %s ========\n",SU400_Txt_ConfirmPassCode);
    }
}

void SU400_Show_InLeft(int nMode, uint8_t nLength)
{
    int nPos_X,nPos_Y;

    if(nMode==SU400_NEW_PASSMODE)
    {
        for(int i=0;i<nLength;i++)
        {
            ituWidgetSetVisible(SU400_UserPW_Data[i], true);  
        }
        //chpark.12.21
        if(nLength>=SU400_MAX_PW_CNT)
        {
            ituWidgetGetGlobalPosition(SU400_UserPW_Data[SU400_MAX_PW_CNT-1], &nPos_X, &nPos_Y); 
            if(SU400_b_VMode)//chpark.01.15
            {
                nPos_X = nPos_X + 62;
            }
            else
            {
                nPos_X = nPos_X + 45;//chpark.24.02.05
            }
        }
        else
        {
            ituWidgetGetGlobalPosition(SU400_UserPW_Data[nLength], &nPos_X, &nPos_Y); 
        }
    }
    else
    {          
        for(int i=0;i<nLength;i++)
        {
            ituWidgetSetVisible(SU400_UserPW_CF_Data[i], true);   
        }
        //chpark.12.21
        if(nLength>=SU400_MAX_PW_CNT)
        {
            ituWidgetGetGlobalPosition(SU400_UserPW_CF_Data[SU400_MAX_PW_CNT-1], &nPos_X, &nPos_Y); 
            if(SU400_b_VMode)//chpark.01.15
            {
                nPos_X = nPos_X + 62;
            }
            else
            {
                nPos_X = nPos_X + 45;//chpark.24.02.05
            }
        }
        else
        {
            ituWidgetGetGlobalPosition(SU400_UserPW_CF_Data[nLength], &nPos_X, &nPos_Y); 
        }
    }
    
    ituWidgetSetPosition(SU400_Cursor[0],nPos_X,nPos_Y);
}


void SU400_Show_InCenter(int nMode,int nLength)
{
    int nPos_X,nPos_Y;

    if(nMode==SU400_NEW_PASSMODE)
    {
        switch(nLength)
        {
            case 0:
            ituWidgetGetGlobalPosition(SU400_UserPW_Data[4], &nPos_X, &nPos_Y);
            break;
            case 1:
            ituWidgetSetVisible(SU400_UserPW_Data[4], true);
            ituWidgetGetGlobalPosition(SU400_UserPW_Data[4], &nPos_X, &nPos_Y);
            break;
            case 2:
            ituWidgetSetVisible(SU400_UserPX_Data[3], true);
            ituWidgetSetVisible(SU400_UserPX_Data[4], true);
            ituWidgetGetGlobalPosition(SU400_UserPX_Data[4], &nPos_X, &nPos_Y);
            break;
            case 3:
            ituWidgetSetVisible(SU400_UserPW_Data[3], true);
            ituWidgetSetVisible(SU400_UserPW_Data[4], true);
            ituWidgetSetVisible(SU400_UserPW_Data[5], true);
            ituWidgetGetGlobalPosition(SU400_UserPW_Data[5], &nPos_X, &nPos_Y);
            break;
            case 4:
            ituWidgetSetVisible(SU400_UserPX_Data[2], true);
            ituWidgetSetVisible(SU400_UserPX_Data[3], true);
            ituWidgetSetVisible(SU400_UserPX_Data[4], true);
            ituWidgetSetVisible(SU400_UserPX_Data[5], true);
            ituWidgetGetGlobalPosition(SU400_UserPX_Data[5], &nPos_X, &nPos_Y);
            break;
            case 5:
            ituWidgetSetVisible(SU400_UserPW_Data[2], true);
            ituWidgetSetVisible(SU400_UserPW_Data[3], true);
            ituWidgetSetVisible(SU400_UserPW_Data[4], true);
            ituWidgetSetVisible(SU400_UserPW_Data[5], true);
            ituWidgetSetVisible(SU400_UserPW_Data[6], true);
            ituWidgetGetGlobalPosition(SU400_UserPW_Data[6], &nPos_X, &nPos_Y);
            break;
            case 6:
                for(int i=1;i<=6;i++)
                {
                    ituWidgetSetVisible(SU400_UserPX_Data[i], true);   
                }
                ituWidgetGetGlobalPosition(SU400_UserPX_Data[6], &nPos_X, &nPos_Y);
                break;
            case 7:
                for(int i=1;i<=7;i++)
                {
                    ituWidgetSetVisible(SU400_UserPW_Data[i], true);   
                }
                ituWidgetGetGlobalPosition(SU400_UserPW_Data[7], &nPos_X, &nPos_Y);
                break;
            case 8:
                for(int i=0;i<=7;i++)
                {
                    ituWidgetSetVisible(SU400_UserPX_Data[i], true);   
                }
                ituWidgetGetGlobalPosition(SU400_UserPX_Data[7], &nPos_X, &nPos_Y);
                break;
            case 9:
                for(int i=0;i<=8;i++)
                {
                    ituWidgetSetVisible(SU400_UserPW_Data[i], true);   
                }
                ituWidgetGetGlobalPosition(SU400_UserPW_Data[8], &nPos_X, &nPos_Y);
                break;

            default:
                break;    

        }
    }
    else
    {
        switch(nLength)
        {
            case 0:
            ituWidgetGetGlobalPosition(SU400_UserPW_CF_Data[4], &nPos_X, &nPos_Y);
            break;
            case 1:
            ituWidgetSetVisible(SU400_UserPW_CF_Data[4], true);
            ituWidgetGetGlobalPosition(SU400_UserPW_CF_Data[4], &nPos_X, &nPos_Y);
            break;
            case 2:
            ituWidgetSetVisible(SU400_UserPX_CF_Data[3], true);
            ituWidgetSetVisible(SU400_UserPX_CF_Data[4], true);
            ituWidgetGetGlobalPosition(SU400_UserPX_CF_Data[4], &nPos_X, &nPos_Y);
            break;
            case 3:
            ituWidgetSetVisible(SU400_UserPW_CF_Data[3], true);
            ituWidgetSetVisible(SU400_UserPW_CF_Data[4], true);
            ituWidgetSetVisible(SU400_UserPW_CF_Data[5], true);
            ituWidgetGetGlobalPosition(SU400_UserPW_CF_Data[5], &nPos_X, &nPos_Y);
            break;
            case 4:
            ituWidgetSetVisible(SU400_UserPX_CF_Data[2], true);
            ituWidgetSetVisible(SU400_UserPX_CF_Data[3], true);
            ituWidgetSetVisible(SU400_UserPX_CF_Data[4], true);
            ituWidgetSetVisible(SU400_UserPX_CF_Data[5], true);
            ituWidgetGetGlobalPosition(SU400_UserPX_CF_Data[5], &nPos_X, &nPos_Y);
            break;
            case 5:
            ituWidgetSetVisible(SU400_UserPW_CF_Data[2], true);
            ituWidgetSetVisible(SU400_UserPW_CF_Data[3], true);
            ituWidgetSetVisible(SU400_UserPW_CF_Data[4], true);
            ituWidgetSetVisible(SU400_UserPW_CF_Data[5], true);
            ituWidgetSetVisible(SU400_UserPW_CF_Data[6], true);
            ituWidgetGetGlobalPosition(SU400_UserPW_CF_Data[6], &nPos_X, &nPos_Y);
            break;
            case 6:
                for(int i=1;i<=6;i++)
                {
                    ituWidgetSetVisible(SU400_UserPX_CF_Data[i], true);   
                }
                ituWidgetGetGlobalPosition(SU400_UserPX_CF_Data[6], &nPos_X, &nPos_Y);
                break;
            case 7:
                for(int i=1;i<=7;i++)
                {
                    ituWidgetSetVisible(SU400_UserPW_CF_Data[i], true);   
                }
                ituWidgetGetGlobalPosition(SU400_UserPW_CF_Data[7], &nPos_X, &nPos_Y);
                break;
            case 8:
                for(int i=0;i<=7;i++)
                {
                    ituWidgetSetVisible(SU400_UserPX_CF_Data[i], true);   
                }
                ituWidgetGetGlobalPosition(SU400_UserPX_CF_Data[7], &nPos_X, &nPos_Y);
                break;
            case 9:
                for(int i=0;i<=8;i++)
                {
                    ituWidgetSetVisible(SU400_UserPW_CF_Data[i], true);   
                }
                ituWidgetGetGlobalPosition(SU400_UserPW_CF_Data[8], &nPos_X, &nPos_Y);
                break;

            default:
                break;    

        }

    }

    if(nLength==0)
    {

    }
    else
    {
        nPos_X = nPos_X+60;
    }

    ituWidgetSetPosition(SU400_Cursor[0],nPos_X,nPos_Y);
}



//======================================================
bool SU400_BTN_MouseDn(ITUWidget* widget, char* param)
{
    if(m_bSU400_GoTo_LayerMode) return false;
    if(m_bSU400_ErrorMsg_Show) return false;

    SU400_ExitCNT = 0;
    int nKeyParam,nKeyIndex;
    uint8_t nASC;
    nKeyParam = atoi(param);
    //printf("SU400_BTN_MouseDn nKeyParam=%d\n",nKeyParam);

    nASC = nKeyParam/100;
    nKeyIndex = nKeyParam%100;
    
    //chpark.24.03.21
    if(nKeyIndex==SU400_NEW_PASS_BTN)
    {
        return false;
    }
    else if(nKeyIndex==SU400_CF_PASS_BTN)
    {
        return false;
    }

    //key select ui
    SU400_Button_Select(nKeyIndex,true);

    //chpark.12.19
    if(nKeyIndex==SU400_DEL_Key)
    {
        SU400_1SecCNT  = 0;
	    SU400_FastFlag = SU400_DEL_AUTO_START;
        //printf("[SU400]Start auto delete\n");
    }

    return true;
}

bool SU400_BTN_MouseUp(ITUWidget* widget, char* param)
{
    if(m_bSU400_ErrorMsg_Show) return false;
    if(m_bSU400_GoTo_LayerMode) return false;

    //key select UI
    SU400_ExitCNT = 0;
    int nKeyParam,nKeyIndex;
    uint8_t nASC;
    nKeyParam = atoi(param);

    nASC = (uint8_t)(nKeyParam/100);
    nKeyIndex = nKeyParam%100;

    //chpark.24.03.21
    if(nKeyIndex==SU400_NEW_PASS_BTN)
    {
        return false;
    }
    else if(nKeyIndex==SU400_CF_PASS_BTN)
    {
        return false;
    }

    SU400_1SecCNT  = 100;
    SU400_FastFlag = SU400_DEL_AUTO_OFF;

    //printf("SU400_BTN_MouseUp nKeyParam=%d nASC=%d nKeyIndex=%d\n",nKeyParam,nASC,nKeyIndex);
    
    //key up UI
    ituWidgetSetVisible(SU400_ICON_DOWN, false);
    ituWidgetSetVisible(SU400_ICON_D7_DOWN, false);
/*
    //chpark.24.03.21
    //touch check of Input Window
    if(nKeyIndex==SU400_NEW_PASS_BTN)
    {
        //touched->new passcode area->set (new pascode mode)
        SU400_Change_Passcode_Mode(SU400_NEW_PASSMODE);
        SU400_Display_PassCode(SU400_NEW_PASSMODE);
        return false;
    }
    else if(nKeyIndex==SU400_CF_PASS_BTN)
    {
        //touched->confirm passcode area->set (confirm pascode mode)
        SU400_Change_Passcode_Mode(SU400_CF_PASSMODE);
        SU400_Display_PassCode(SU400_CF_PASSMODE);
        return false;
    }
 */   
    //==========================================
    //key input
    if(SU400_PC_MODE==SU400_NEW_PASSMODE)
    {
        if(nKeyIndex==SU400_SKIP_KEY)//skip button
        {
            if(g_nSU100_Mode == SU100_Add_Mode)
            {
                //chpark.11.27
                if(g_nSU100_PreLayer==M7_SU200_EDIT_LAYER)
                {
                    //do not save skip info.
                    ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
                    assert(M7_SU200_EDIT);
                    ituLayerGoto(M7_SU200_EDIT);
                }
                else
                {
                    //save skip info.
                    User_RegInfo[g_nSU100_AddUserIdx].nTmp_PassCode_Skip=1;

                    ITULayer *M7_SU500_RFID = ituSceneFindWidget(&theScene, "M7_SU500_RFID");
                    assert(M7_SU500_RFID);
                    ituLayerGoto(M7_SU500_RFID);
                }
            }
            else if(g_nSU100_Mode == SU100_Edit_Mode)
            {
                ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
                assert(M7_SU200_EDIT);
                ituLayerGoto(M7_SU200_EDIT);
            }
        }
        else if(nKeyIndex==SU400_DEL_Key)//Del Key
        {
            //SU400_Show_Result_UI(-1);
            SU400_Delete_Char(0);   //delete char//chpark.12.19

            SU400_Display_PassCode(SU400_NEW_PASSMODE);
        }
        else if(nKeyIndex>=1 && nKeyIndex<=10)
        {
            //SU400_Show_Result_UI(-1);

            int len;
            len = strlen(SU400_Txt_NewPassCode);

            if(len>=SU400_MAX_PW_CNT)
            {
                //printf("\n[PassCode:Len Over]len=%d input=%s \n\n",len,SU400_Txt_NewPassCode);
                SU400_Display_PassCode(SU400_NEW_PASSMODE);//chpark.12.19
                return false;
            }

            SU400_Txt_NewPassCode[len] = (char)nASC;//ascii char input
            SU400_Txt_NewPassCode[len+1] = '\0';//ascii char input
            
            //printf("\n[PassCode:NUM] len=%d input=%s \n\n",len,SU400_Txt_NewPassCode);

            SU400_Display_PassCode(SU400_NEW_PASSMODE);
        }
        else if(nKeyIndex==SU400_D7_Key)//D7 Key
        {
            SU400_MenuTabEnter();    
        }

    }
    else if(SU400_PC_MODE==SU400_CF_PASSMODE)
    {
        if(nKeyIndex==SU400_SKIP_KEY)//skip button
        {
            if(g_nSU100_Mode == SU100_Add_Mode)
            {
                //save skip info.
                User_RegInfo[g_nSU100_AddUserIdx].nTmp_PassCode_Skip=1;
                
                ITULayer *M7_SU500_RFID = ituSceneFindWidget(&theScene, "M7_SU500_RFID");
                assert(M7_SU500_RFID);
                ituLayerGoto(M7_SU500_RFID);
            }
            else if(g_nSU100_Mode == SU100_Edit_Mode)
            {
                ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
                assert(M7_SU200_EDIT);
                ituLayerGoto(M7_SU200_EDIT);
            }
        }
        else if(nKeyIndex==SU400_DEL_Key)//Del Key
        {
            SU400_Delete_Char(0);   //delete char//chpark.12.19

            SU400_Display_PassCode(SU400_CF_PASSMODE);
        }
        else if(nKeyIndex>=1 && nKeyIndex<=10)
        {
            //SU400_Show_Result_UI(-1);  
            int len;
            len = strlen(SU400_Txt_ConfirmPassCode);

            if(len>=SU400_MAX_PW_CNT)
            {
                //printf("\n[PassCode:Len Over]len=%d input=%s \n\n",len,SU400_Txt_ConfirmPassCode);
                SU400_Display_PassCode(SU400_CF_PASSMODE);//chpark.12.19
                return false;
            }
            else
            {
                //printf("\n[PassCode:NUM] len=%d input=%s \n\n",len,SU400_Txt_ConfirmPassCode);
            }

            SU400_Txt_ConfirmPassCode[len] = (char)nASC;//ascii char input
            SU400_Txt_ConfirmPassCode[len+1] = '\0';//ascii char input
            
            SU400_Display_PassCode(SU400_CF_PASSMODE);//chpark.12.19
            
        }
        else if(nKeyIndex==SU400_D7_Key)//D7 Key
        {
            SU400_MenuTabEnter(); 
        }
    }
    //=========================================
    return true;
}

bool SU400_Delete_Char(int nMode)
{
    bool bRet = false;

    if(SU400_PC_MODE==SU400_NEW_PASSMODE)
    {
        int len;
            len = strlen(SU400_Txt_NewPassCode);

            if( (len-1)<0)
            {
                //printf("Cannot delete char.[Error====] len:%d\n",len-1);
                return bRet;
            }

            SU400_Txt_NewPassCode[len-1] = '\0';//ascii char input
            bRet = true;

    }
    else if(SU400_PC_MODE==SU400_CF_PASSMODE)
    {
        int len;
            len = strlen(SU400_Txt_ConfirmPassCode);

            if( (len-1)<0)
            {
                //printf("Cannot delete char.[Error====] len:%d\n",len-1);
                return bRet;
            }

            SU400_Txt_ConfirmPassCode[len-1] = '\0';//ascii char input
            bRet = true;

            //printf("\n[PassCode:DEL]len=%d input=%s \n\n",len,SU400_Txt_ConfirmPassCode);
    }

    return bRet;
}

//======================================================


void SU400_Change_Passcode_Mode(int nMode)
{
    if(nMode==SU400_NEW_PASSMODE)
    {
        //new passcode.
        ituWidgetSetVisible(SU400_CONT_SKIP_BTN,    true);  //skip button

        SU400_PC_MODE = SU400_NEW_PASSMODE;

        ituWidgetSetVisible(SU400_CONT_TXT_PWD, true);
        ituWidgetSetVisible(SU400_CONT_TXT_CFPWD, true);
        ituWidgetSetVisible(SU400_NEW_PWD_TXT[Lang_Flag[M7_UserIndex]], true);//chpark.24.02.05
        ituWidgetSetVisible(SU400_CF_PWD_TXT[Lang_Flag[M7_UserIndex]],  true);//chpark.24.02.05

        ituWidgetSetVisible(SU400_NEW_PWD_TXT[SU400_LANG_TXT], false);//chpark.24.02.05
        ituWidgetSetVisible(SU400_CF_PWD_TXT[SU400_LANG_TXT],  false);//chpark.24.02.05
    }
    else
    {
        //confirm passcode.
        ituWidgetSetVisible(SU400_CONT_SKIP_BTN,    true);  //skip button

        SU400_PC_MODE = SU400_CF_PASSMODE;

        ituWidgetSetVisible(SU400_CONT_TXT_PWD, true);
        ituWidgetSetVisible(SU400_CONT_TXT_CFPWD, true);
        ituWidgetSetVisible(SU400_NEW_PWD_TXT[Lang_Flag[M7_UserIndex]], true);//chpark.24.02.05
        ituWidgetSetVisible(SU400_CF_PWD_TXT[Lang_Flag[M7_UserIndex]],  true);//chpark.24.02.05

        ituWidgetSetVisible(SU400_NEW_PWD_TXT[SU400_LANG_TXT], false);//chpark.24.02.05
        ituWidgetSetVisible(SU400_CF_PWD_TXT[SU400_LANG_TXT],  false);//chpark.24.02.05
    }

    //if change mode, set focus at #1.
    SU400_Button_Select(1,false);
}

void SU400_Passcode_Clear(int nMode)
{
    if(nMode==SU400_NEW_PASSMODE)
    {
        //new passcode.
        for (int i=0;i<SU400_MAX_PW_CNT;i++)
	    {
		    ituWidgetSetVisible(SU400_UserPW_Data[i], false);
	    }
        for (int i=0;i<SU400_MAX_PW_CNT-1;i++)
	    {
		    ituWidgetSetVisible(SU400_UserPX_Data[i], false);
	    }

        memset(SU400_Txt_NewPassCode, '\0', sizeof(SU400_Txt_NewPassCode));

        SU400_Display_PassCode(SU400_NEW_PASSMODE);
    }
    else
    {
        //confirm passcode.
         for (int i=0;i<SU400_MAX_PW_CNT;i++)
	    {
		    ituWidgetSetVisible(SU400_UserPW_CF_Data[i], false);
	    }
        for (int i=0;i<SU400_MAX_PW_CNT-1;i++)
	    {
		    ituWidgetSetVisible(SU400_UserPX_CF_Data[i], false);
	    }

        memset(SU400_Txt_ConfirmPassCode, '\0', sizeof(SU400_Txt_ConfirmPassCode));

        SU400_Display_PassCode(SU400_CF_PASSMODE);
    }
}
//======================================================

int SU400_Passcode_Check()
{
    //check new passcode & confirm passcode.
    int nResult = 0;
    nResult = strcmp(SU400_Txt_NewPassCode,SU400_Txt_ConfirmPassCode);

    //printf("[SU400_Passcode_Check] = %d \n", nResult);

    return nResult;
}

void SU400_Chk_Result_InServer(int nSuc)
{  
    //======================================================================
    //convering
    //printf("SU400_Txt_ConfirmPassCode:[%s] \n",SU400_Txt_ConfirmPassCode);
    //=======================================================================            
    if(nSuc)
    {
        //success msg
        SU400_Show_Result_UI(10);

        //save data
        if(g_nSU100_Mode == SU100_Add_Mode)
        {
            //move to passcode setting
            memset(User_RegInfo[g_nSU100_AddUserIdx].UserPassCode,'\0', SU400_MAX_PW_CNT);
            memcpy(User_RegInfo[g_nSU100_AddUserIdx].UserPassCode, SU400_Txt_ConfirmPassCode ,SU400_MAX_PW_CNT);
            
            //printf("SU400_Chk_Result:data save==\n");
        }
        else
        {
            memset(User_RegInfo[g_nSU100_EditUserIdx].UserPassCode,'\0', SU400_MAX_PW_CNT);
            memcpy(User_RegInfo[g_nSU100_EditUserIdx].UserPassCode, SU400_Txt_ConfirmPassCode ,SU400_MAX_PW_CNT);
            //move to passcode setting
           
            //printf("SU400_Chk_Result:data save==[#%d user][new passocde]:%s\n",g_nSU100_EditUserIdx,User_RegInfo[g_nSU100_EditUserIdx].UserPassCode);
        }

        //move to passcode setting
        m_bSU400_GoTo_LayerMode = true;
    }
    else
    {
        //Error Message
        SU400_Show_Result_UI(3);
    }
}

void SU400_Show_Result_UI(int nRes)
{
    //int  SU400_Result_UI_Status; //-1:none  0:X  1:O  2:X+text  3:O+text 
    //Show O,X UI
    SU400_Result_UI_Status = nRes;

    if(SU400_Result_UI_Status<0)//chpark.24.01.31
    {
        ituWidgetSetVisible(SU400_ICON_DOWN,     false);
        ituWidgetSetVisible(SU400_ICON_D7_SEL,   false);
        ituWidgetSetVisible(SU400_ICON_D7_DOWN,  false);  
    }

    switch(SU400_Result_UI_Status)
    {
        case -10: //Reset all
            {
                ituWidgetSetVisible(SU400_CONT_ERROR_TXT, false);  
                ituWidgetSetVisible(SU400_CONT_ERROR_1_TXT, false);      
                ituWidgetSetVisible(SU400_CONT_ERROR_2_TXT, false);  //chpark.24.01.31
                ituWidgetSetVisible(SU400_CONT_RESULT, false);
                 //chpark.11.20
                ituWidgetSetVisible(SU400_ERROR_TEXT, false);

                //delete all passcode & set focus in new passcode
                SU400_Passcode_Clear(SU400_CF_PASSMODE);
                SU400_Change_Passcode_Mode(SU400_NEW_PASSMODE);                
                SU400_Passcode_Clear(SU400_NEW_PASSMODE);

                //printf("Clear All====\n");
               
                m_bSU400_ErrorMsg_Show = false;

                SU400_Button_Select(1,false);//chpark.24.02.05
            }
            break;

        case -1: //Reset new pass
            {
                ituWidgetSetVisible(SU400_CONT_ERROR_TXT, false);   
                ituWidgetSetVisible(SU400_CONT_ERROR_1_TXT, false); 
                ituWidgetSetVisible(SU400_CONT_ERROR_2_TXT, false);  //chpark.24.01.31     
                ituWidgetSetVisible(SU400_CONT_RESULT, false);
                 //chpark.11.20
                ituWidgetSetVisible(SU400_ERROR_TEXT, false);

                SU400_Passcode_Clear(SU400_NEW_PASSMODE);
                m_bSU400_ErrorMsg_Show = false;
            }
            break;

        case -2: //Reset cf pass
            {
                ituWidgetSetVisible(SU400_CONT_ERROR_TXT, false); 
                ituWidgetSetVisible(SU400_CONT_ERROR_1_TXT, false);   
                ituWidgetSetVisible(SU400_CONT_ERROR_2_TXT, false);  //chpark.24.01.31     
                ituWidgetSetVisible(SU400_CONT_RESULT, false);
                //chpark.11.20
                ituWidgetSetVisible(SU400_ERROR_TEXT, false);

                SU400_Passcode_Clear(SU400_CF_PASSMODE);
                m_bSU400_ErrorMsg_Show = false;
            }
            break;

        case 0://Fail . Only X (new pass!= cf pass)
            //The passcode confirmation does not match
            //Error Message
            ituWidgetSetVisible(SU400_CONT_ERROR_TXT, false);
            ituWidgetSetVisible(SU400_CONT_ERROR_1_TXT, false);
            ituWidgetSetVisible(SU400_CONT_ERROR_2_TXT, false);  //chpark.24.01.31

            ituWidgetSetVisible(SU400_CONT_ERROR_TXT, true);
            for(int i=0;i<=SU400_LANG_TXT;i++)
            {
                ituWidgetSetVisible(SU400_ERROR_TXT[i], false); 
            }
            ituWidgetSetVisible(SU400_ERROR_TXT[Lang_Flag[M7_UserIndex]], true); //chpark.24.01.31
            
            //chpark.11.20
            ituWidgetSetVisible(SU400_ERROR_TEXT, false);//chpark.24.01.31

            //X Icon.    
            ituWidgetSetVisible(SU400_CONT_RESULT, true);
            ituWidgetSetVisible(SU400_RESULT_ICON[SU400_SUCCESS], false);
            ituWidgetSetVisible(SU400_RESULT_ICON[SU400_FAIL], true);
            SU400_ErrorMsg_Cnt = 0;//chpark.11.02
            m_bSU400_ErrorMsg_Show = true;

            
            break;
        case 1://Fail . Only X new pass <4
            //The passcode must be between 4 and 9 characters
            //Error Message
            ituWidgetSetVisible(SU400_CONT_ERROR_TXT, false);
            ituWidgetSetVisible(SU400_CONT_ERROR_1_TXT, false);
            ituWidgetSetVisible(SU400_CONT_ERROR_2_TXT, false);  //chpark.24.01.31
            
            ituWidgetSetVisible(SU400_CONT_ERROR_1_TXT, true);
            for(int i=0;i<=SU400_LANG_TXT;i++)
            {
                ituWidgetSetVisible(SU400_ERROR_1_TXT[i], false); 
            }
            ituWidgetSetVisible(SU400_ERROR_1_TXT[Lang_Flag[M7_UserIndex]], true); 
            
            //chpark.11.20
            ituWidgetSetVisible(SU400_ERROR_TEXT, false);//chpark.24.01.31

            //X Icon.    
            ituWidgetSetVisible(SU400_CONT_RESULT, true);
            ituWidgetSetVisible(SU400_RESULT_ICON[SU400_SUCCESS], false);
            ituWidgetSetVisible(SU400_RESULT_ICON[SU400_FAIL], true);
            
            SU400_ErrorMsg_Cnt = 0;//chpark.11.02
            m_bSU400_ErrorMsg_Show = true;
            break;

        case 2://Fail . Only X cf pass <4
            //Error Message
            //The passcode must be between 4 and 9 characters
            ituWidgetSetVisible(SU400_CONT_ERROR_TXT, false);
            ituWidgetSetVisible(SU400_CONT_ERROR_1_TXT, false);
            ituWidgetSetVisible(SU400_CONT_ERROR_2_TXT, false);  //chpark.24.01.31
            
            ituWidgetSetVisible(SU400_CONT_ERROR_1_TXT, true);
            for(int i=0;i<=SU400_LANG_TXT;i++)
            {
                ituWidgetSetVisible(SU400_ERROR_1_TXT[i], false); 
            }
            ituWidgetSetVisible(SU400_ERROR_1_TXT[Lang_Flag[M7_UserIndex]], true);
            
            //chpark.11.20
            ituWidgetSetVisible(SU400_ERROR_TEXT, false);//chpark.24.01.31

            //X Icon.    
            ituWidgetSetVisible(SU400_CONT_RESULT, true);
            ituWidgetSetVisible(SU400_RESULT_ICON[SU400_SUCCESS], false);
            ituWidgetSetVisible(SU400_RESULT_ICON[SU400_FAIL], true);
            SU400_ErrorMsg_Cnt = 0;//chpark.11.02
            m_bSU400_ErrorMsg_Show = true;
            break;
       
        case 3://Fail : 
            //the passcode is already used.
            //Reset all passcode & show error msg

            //chpark.12.13
            //delete all passcode & set focus in new passcode
            SU400_Passcode_Clear(SU400_CF_PASSMODE);
            SU400_Change_Passcode_Mode(SU400_NEW_PASSMODE);                
            SU400_Passcode_Clear(SU400_NEW_PASSMODE);

            //Error Message
            ituWidgetSetVisible(SU400_CONT_ERROR_TXT,   false);
            ituWidgetSetVisible(SU400_CONT_ERROR_1_TXT, false);
            ituWidgetSetVisible(SU400_CONT_ERROR_2_TXT, false);  //chpark.24.01.31
           
            //chpark.11.20
            ituWidgetSetVisible(SU400_ERROR_TEXT, false);//chpark.24.01.31

            ituWidgetSetVisible(SU400_CONT_ERROR_2_TXT, true);
            for(int i=0;i<=SU400_LANG_SP;i++)
            {
                ituWidgetSetVisible(SU400_ERROR_2_TXT[i], false); 
            }
            ituWidgetSetVisible(SU400_ERROR_2_TXT[Lang_Flag[M7_UserIndex]], true);
           	
            //X Icon.    
            ituWidgetSetVisible(SU400_CONT_RESULT, true);
            ituWidgetSetVisible(SU400_RESULT_ICON[SU400_SUCCESS], false);
            ituWidgetSetVisible(SU400_RESULT_ICON[SU400_FAIL], true);
            SU400_ErrorMsg_Cnt = 0;//chpark.11.02
            m_bSU400_ErrorMsg_Show = true;
            break;
        
        case 4://Success
            ituWidgetSetVisible(SU400_CONT_ERROR_TXT, false);
            ituWidgetSetVisible(SU400_CONT_ERROR_1_TXT, false);
            ituWidgetSetVisible(SU400_CONT_ERROR_2_TXT, false);//chpark.24.01.31
            ituWidgetSetVisible(SU400_ERROR_TEXT, false);
            //V Icon.    
            ituWidgetSetVisible(SU400_CONT_RESULT, true);
            ituWidgetSetVisible(SU400_RESULT_ICON[SU400_SUCCESS], true);
            ituWidgetSetVisible(SU400_RESULT_ICON[SU400_FAIL], false);
            break;

        case 10://Success
            //Delete Error Message 
            ituWidgetSetVisible(SU400_CONT_ERROR_TXT, false);
            ituWidgetSetVisible(SU400_CONT_ERROR_1_TXT, false);
            ituWidgetSetVisible(SU400_CONT_ERROR_2_TXT, false);//chpark.24.01.31
            ituWidgetSetVisible(SU400_ERROR_TEXT, false);
            //V Icon.    
            ituWidgetSetVisible(SU400_CONT_RESULT, true);
            ituWidgetSetVisible(SU400_RESULT_ICON[SU400_SUCCESS], true);
            ituWidgetSetVisible(SU400_RESULT_ICON[SU400_FAIL], false);
            break;

        default:
            ituWidgetSetVisible(SU400_CONT_ERROR_TXT, false);       
            ituWidgetSetVisible(SU400_CONT_ERROR_1_TXT, false);
            ituWidgetSetVisible(SU400_CONT_ERROR_2_TXT, false);//chpark.24.01.31
            ituWidgetSetVisible(SU400_CONT_RESULT, false);
            ituWidgetSetVisible(SU400_ERROR_TEXT, false);//chpark.11.20
            break;
    }

}


//static int m_nSU400_ButtonIndex=0;//current selected key index. 0:skip 1-10:key 11:del 12:D7 
void SU400_MenuTabPrev()
{
    if(SU400_b_VMode)
    {        
        if(m_nSU400_ButtonIndex==1)
        {
            m_nSU400_ButtonIndex = SU400_SKIP_KEY;
        }
        else if(m_nSU400_ButtonIndex>=2 && m_nSU400_ButtonIndex<=9)
        {
            m_nSU400_ButtonIndex--;
        }
        else if(m_nSU400_ButtonIndex==SU400_DEL_Key)
        {
            m_nSU400_ButtonIndex = 9;
        }
		else if(m_nSU400_ButtonIndex==10)
        {
            m_nSU400_ButtonIndex = SU400_DEL_Key;
        }
        else if(m_nSU400_ButtonIndex==SU400_D7_Key)
        {
            m_nSU400_ButtonIndex = 10;
        }
        else if(m_nSU400_ButtonIndex==SU400_SKIP_KEY)
        {
            m_nSU400_ButtonIndex = SU400_D7_Key;
        }
    }
    else
    {
        if(m_nSU400_ButtonIndex==1)
        {
            m_nSU400_ButtonIndex = SU400_SKIP_KEY;
        }
        else if(m_nSU400_ButtonIndex>=2 && m_nSU400_ButtonIndex<=10)
        {
            m_nSU400_ButtonIndex--;
        }
        else if(m_nSU400_ButtonIndex==SU400_DEL_Key)
        {
            m_nSU400_ButtonIndex = 10;
        }
        else if(m_nSU400_ButtonIndex==SU400_D7_Key)
        {
            m_nSU400_ButtonIndex = SU400_DEL_Key;
        }
        else if(m_nSU400_ButtonIndex==SU400_SKIP_KEY)
        {
            m_nSU400_ButtonIndex = SU400_D7_Key;
        }
    }

    SU400_Button_Select(m_nSU400_ButtonIndex, false);//chpark.11.02
}



void SU400_MenuTabNext()
{
    if(SU400_b_VMode)
    {        
        if(m_nSU400_ButtonIndex==SU400_SKIP_KEY)
        {
            m_nSU400_ButtonIndex = 1;
        }
        else if(m_nSU400_ButtonIndex>=1 && m_nSU400_ButtonIndex<=8)
        {
            m_nSU400_ButtonIndex++;
        }
        else if(m_nSU400_ButtonIndex==9)
        {
            m_nSU400_ButtonIndex = SU400_DEL_Key;
        }
		else if(m_nSU400_ButtonIndex==SU400_DEL_Key)
        {
            m_nSU400_ButtonIndex = 10;
        }
        else if(m_nSU400_ButtonIndex==10)
        {
            m_nSU400_ButtonIndex = SU400_D7_Key;
        }
        else if(m_nSU400_ButtonIndex==SU400_D7_Key)
        {
            m_nSU400_ButtonIndex = SU400_SKIP_KEY;
        }
    }
    else
    {
        if(m_nSU400_ButtonIndex==SU400_SKIP_KEY)
        {
            m_nSU400_ButtonIndex = 1;
        }
        else if(m_nSU400_ButtonIndex>=1 && m_nSU400_ButtonIndex<=9)
        {
            m_nSU400_ButtonIndex++;
        }
        else if(m_nSU400_ButtonIndex==10)
        {
            m_nSU400_ButtonIndex = SU400_DEL_Key;
        }
        else if(m_nSU400_ButtonIndex==SU400_DEL_Key)
        {
            m_nSU400_ButtonIndex = SU400_D7_Key;
        }
        else if(m_nSU400_ButtonIndex==SU400_D7_Key)
        {
            m_nSU400_ButtonIndex = SU400_SKIP_KEY;
        }
    }

    SU400_Button_Select(m_nSU400_ButtonIndex, false);//chpark.11.02
}

void SU400_MenuTabUp()
{
    if(SU400_b_VMode)
    {        
        if(m_nSU400_ButtonIndex==1)
        {
            m_nSU400_ButtonIndex = SU400_SKIP_KEY;
        }
        else if(m_nSU400_ButtonIndex==2)
        {
            m_nSU400_ButtonIndex = SU400_SKIP_KEY;
        }
        else if(m_nSU400_ButtonIndex==3)
        {
            m_nSU400_ButtonIndex = SU400_SKIP_KEY;
        }
        else if(m_nSU400_ButtonIndex>=4 && m_nSU400_ButtonIndex<=9)
        {
            m_nSU400_ButtonIndex = m_nSU400_ButtonIndex-3;
        }
        else if(m_nSU400_ButtonIndex==SU400_DEL_Key)
        {
            m_nSU400_ButtonIndex = 7;
        }
        else if(m_nSU400_ButtonIndex==10)
        {
            m_nSU400_ButtonIndex = 8;
        }
        else if(m_nSU400_ButtonIndex==SU400_D7_Key)
        {
            m_nSU400_ButtonIndex = 9;
        }
        else if(m_nSU400_ButtonIndex==SU400_SKIP_KEY)
        {
            m_nSU400_ButtonIndex = SU400_D7_Key;
        }

        SU400_Button_Select(m_nSU400_ButtonIndex, false);//chpark.11.02
    }
    else
    {
        if(m_nSU400_ButtonIndex>=1 && m_nSU400_ButtonIndex<=10)
        {
            m_nSU400_ButtonIndex = SU400_SKIP_KEY;
        }
        else if(m_nSU400_ButtonIndex==SU400_DEL_Key)
        {
            m_nSU400_ButtonIndex = SU400_SKIP_KEY;
        }
        else if(m_nSU400_ButtonIndex==SU400_D7_Key)
        {
            m_nSU400_ButtonIndex = SU400_DEL_Key;
        }
        else if(m_nSU400_ButtonIndex==SU400_SKIP_KEY)
        {
            m_nSU400_ButtonIndex = SU400_D7_Key;
        }
        SU400_Button_Select(m_nSU400_ButtonIndex, false);//chpark.11.02
    }

}

void SU400_MenuTabDown()
{
    if(SU400_b_VMode)
    {        
        if(m_nSU400_ButtonIndex>=1 && m_nSU400_ButtonIndex<=6)
        {
            m_nSU400_ButtonIndex = m_nSU400_ButtonIndex+3;
        }
        else if(m_nSU400_ButtonIndex==7)
        {
            m_nSU400_ButtonIndex = SU400_DEL_Key;
        }
        else if(m_nSU400_ButtonIndex==8)
        {
            m_nSU400_ButtonIndex = 10;
        }
        else if(m_nSU400_ButtonIndex==9)
        {
            m_nSU400_ButtonIndex = SU400_D7_Key;
        }
        else if(m_nSU400_ButtonIndex==SU400_D7_Key)
        {
            m_nSU400_ButtonIndex = SU400_SKIP_KEY;
        }
        else if(m_nSU400_ButtonIndex==SU400_SKIP_KEY)
        {
            m_nSU400_ButtonIndex = 3;
        }
        else if(m_nSU400_ButtonIndex==10)
        {
            m_nSU400_ButtonIndex = SU400_D7_Key;
        }
        else if(m_nSU400_ButtonIndex==SU400_DEL_Key)
        {
            m_nSU400_ButtonIndex = SU400_D7_Key;
        }
        SU400_Button_Select(m_nSU400_ButtonIndex, false);//chpark.11.02
    }
    else
    {
        if(m_nSU400_ButtonIndex>=1 && m_nSU400_ButtonIndex<=10)
        {
            m_nSU400_ButtonIndex = SU400_D7_Key;
        }
        else if(m_nSU400_ButtonIndex==SU400_DEL_Key)
        {
            m_nSU400_ButtonIndex = SU400_D7_Key;
        }
        else if(m_nSU400_ButtonIndex==SU400_D7_Key)
        {
            m_nSU400_ButtonIndex = SU400_SKIP_KEY;
        }
        else if(m_nSU400_ButtonIndex==SU400_SKIP_KEY)
        {
            m_nSU400_ButtonIndex = 1;//SU400_DEL_Key; //chpark.24.04.19
        }
        SU400_Button_Select(m_nSU400_ButtonIndex, false);//chpark.11.02
    }

}

void SU400_MenuTabLeft()
{
    if(SU400_b_VMode)
    {
        switch(m_nSU400_ButtonIndex)
        {
                case 1:
                    m_nSU400_ButtonIndex=3;break;//chpark.24.04.16
                case 2:
                    m_nSU400_ButtonIndex=1;break;
                case 3:
                    m_nSU400_ButtonIndex=2;break;
                case 4:
                    m_nSU400_ButtonIndex=6;break;
                case 5:
                    m_nSU400_ButtonIndex=4;break;
                case 6:
                    m_nSU400_ButtonIndex=5;break;
                case 7:
                    m_nSU400_ButtonIndex=9;break;
                case 8:
                    m_nSU400_ButtonIndex=7;break;
                case 9:
                    m_nSU400_ButtonIndex=8;break;
                case SU400_DEL_Key:
                    m_nSU400_ButtonIndex=10;break;
                case 10:
                    m_nSU400_ButtonIndex=SU400_DEL_Key;break;
                case SU400_D7_Key:
                    m_nSU400_ButtonIndex=10;break;
                case SU400_SKIP_KEY:
                    m_nSU400_ButtonIndex=SU400_D7_Key;break;
                default:
                    break;
        }
    }
    else
    {
        if(m_nSU400_ButtonIndex==1)
        {
            m_nSU400_ButtonIndex = SU400_DEL_Key;
        }
        else if(m_nSU400_ButtonIndex>=2 && m_nSU400_ButtonIndex<=10)
        {
            m_nSU400_ButtonIndex--;
        }
        else if(m_nSU400_ButtonIndex==SU400_DEL_Key)
        {
            m_nSU400_ButtonIndex = 10;
        }
    }

    SU400_Button_Select(m_nSU400_ButtonIndex, false);//chpark.11.02
}

void SU400_MenuTabRight()
{
    if(SU400_b_VMode)
    {
        switch(m_nSU400_ButtonIndex)
        {
                case 1:
                    m_nSU400_ButtonIndex=2;break;
                case 2:
                    m_nSU400_ButtonIndex=3;break;
                case 3:
                    m_nSU400_ButtonIndex=1;break;
                case 4:
                    m_nSU400_ButtonIndex=5;break;
                case 5:
                    m_nSU400_ButtonIndex=6;break;
                case 6:
                    m_nSU400_ButtonIndex=4;break;
                case 7:
                    m_nSU400_ButtonIndex=8;break;
                case 8:
                    m_nSU400_ButtonIndex=9;break;
                case 9:
                    m_nSU400_ButtonIndex=7;break;
                case SU400_DEL_Key:
                    m_nSU400_ButtonIndex=10;break;
                case 10:
                    m_nSU400_ButtonIndex=SU400_D7_Key;break;
                case SU400_D7_Key:
                    m_nSU400_ButtonIndex=SU400_SKIP_KEY;break;
                case SU400_SKIP_KEY:
                    m_nSU400_ButtonIndex=1;break;
                default:
                    break;
        }
    }
    else
    {
        if(m_nSU400_ButtonIndex>=1 && m_nSU400_ButtonIndex<=9)
        {
            m_nSU400_ButtonIndex++;
        }
        else if(m_nSU400_ButtonIndex==10)
        {
            m_nSU400_ButtonIndex = SU400_DEL_Key;
        }
        else if(m_nSU400_ButtonIndex==SU400_DEL_Key)
        {
            m_nSU400_ButtonIndex = 1;
        }
    }

    SU400_Button_Select(m_nSU400_ButtonIndex, false);//chpark.11.02
}

void SU400_MenuTabEnter()
{
    int len=0;

    if(SU400_PC_MODE==SU400_NEW_PASSMODE)
    {
        len = strlen(SU400_Txt_NewPassCode);
        if(len>=4 && len<=9)
        {
            SU400_Change_Passcode_Mode(SU400_CF_PASSMODE);
            //SU400_Passcode_Clear(SU400_CF_PASSMODE);
            SU400_Display_PassCode(SU400_CF_PASSMODE);
        }
        else
        {
            SU400_Show_Result_UI(1);    
            printf("[UP][New] Len Error : %d\n",len);
        }
        
    }
    else if(SU400_PC_MODE==SU400_CF_PASSMODE)
    {
        if(m_bSU400_GoTo_LayerMode)return;
        
        len = strlen(SU400_Txt_ConfirmPassCode);
        if(len>=4 && len<=9)
        {
            int nValid_Passcode = SU400_Passcode_Check();
            if(nValid_Passcode==0)
            {
                //new pass == cf pass
                g_UM5_ChkTime = 0;
                SU400_UM5_Rcv_Type = SU400_RCV_PASSCODE_CHK;
                
                if(!gUI_Server_Mode)
				{
                    SU400_Chk_Result_InServer(1);
                }
                else
                {
                    SU400_Send_TX(1);
                }
               
            }
            else
            {
                //new pass xx cf pass
                SU400_Show_Result_UI(0);  
                printf("[UP][CF] not confirm pass error : %d\n",len);
            }
        }
        else
        {
            SU400_Show_Result_UI(2);  
            printf("[UP][CF] Len Error : %d\n",len);
        }
    }
}

void SU400_MenuTabESC()
{
    if(g_nSU100_Mode == SU100_Add_Mode)
    {
        //if(SU400_PC_MODE==SU400_NEW_PASSMODE)
        //{
            if(g_nSU100_PreLayer==M7_SU200_EDIT_LAYER)
            {
                ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
                assert(M7_SU200_EDIT);
                ituLayerGoto(M7_SU200_EDIT);
            }
            else
            {
                ITULayer *M7_SU300_NAME = ituSceneFindWidget(&theScene, "M7_SU300_NAME");
                assert(M7_SU300_NAME);
                ituLayerGoto(M7_SU300_NAME);
            }
        //}
        //else
        //{
            //go new passmode
            //SU400_Change_Passcode_Mode(SU400_NEW_PASSMODE);
            //SU400_Passcode_Clear(SU400_CF_PASSMODE);
            //SU400_Display_PassCode(SU400_NEW_PASSMODE);
        //}
    }
    else if(g_nSU100_Mode == SU100_Edit_Mode)
    {
        //if(SU400_PC_MODE==SU400_NEW_PASSMODE)
        //{
            ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
            assert(M7_SU200_EDIT);
            ituLayerGoto(M7_SU200_EDIT);
        //}
        //else
        //{
            //go new passmode
            //SU400_Change_Passcode_Mode(SU400_NEW_PASSMODE);
            //SU400_Passcode_Clear(SU400_CF_PASSMODE);
            //SU400_Display_PassCode(SU400_NEW_PASSMODE);
        //}
    }
}


bool SU400_JogWheel(ITUWidget* widget, char* param)
{
    if(!m_bSU400ReadyToJOG ||  m_bSU400_GoTo_LayerMode) return false;
    if(m_bSU400_ErrorMsg_Show) return false;

    SU400_ExitCNT = 0;
	uint8_t JogDir = 0;
	JogDir = param[0];
	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
        case LANDSCAPE_1:
		case LANDSCAPE_2:
			//printf("PORTRATE\r\n");

            if(JogDir == JOG_DIAL_ROT_RIGHT)
			{
                //printf("SU400 JogWheel[R] %d\r\n",JogDir);
                SU400_MenuTabNext();
			}
			else if(JogDir == JOG_DIAL_ROT_LEFT)
			{
                //printf("SU400 JogWheel[L] %d\r\n",JogDir);
                SU400_MenuTabPrev();
			}

		break;
	}

    return true;
}

bool SU400_JogButton(ITUWidget* widget, char* param)
{
    if(!m_bSU400ReadyToJOG || m_bSU400_GoTo_LayerMode) return false;
    if(m_bSU400_ErrorMsg_Show) return false;
    
    SU400_ExitCNT = 0;
    uint8_t JogButton = 0;
	JogButton = param[0];
	//printf("SU400_JogButton %d\r\n",JogButton);

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
                    SU400_MenuTabUp();
					break;

				case JOG_DIAL_KEY_DOWN:
                    SU400_MenuTabDown();
					break;
				
				case JOG_DIAL_KEY_LEFT:
                    SU400_MenuTabLeft();
                    break;

				case JOG_DIAL_KEY_RIGHT:
					SU400_MenuTabRight();
					break;

				case JOG_DIAL_KEY_ENTER_ON:
                    SU400_Button_Select(m_nSU400_ButtonIndex,true);
                     //chpark.12.19
                    if(m_nSU400_ButtonIndex==SU400_DEL_Key)
                    {
                        SU400_1SecCNT  = 0;
                        SU400_FastFlag = SU400_DEL_AUTO_START;
                        //printf("[SU400]Start auto delete\n");
                    }
                    
					break;

				case JOG_DIAL_KEY_ENTER_OFF:				//c3Byte0bit
        		case JOG_DIAL_KEY_NONE:
				{
             		SU400_Button_Select(m_nSU400_ButtonIndex,false); 
                    //printf("[400]JOG_DIAL_KEY_ENTER_OFF:%d\n",m_nSU400_ButtonIndex);
                    //========================== 
                    if(m_nSU400_ButtonIndex>=0 && m_nSU400_ButtonIndex<13)
                    {
                        int nTmpKeyValue = PASSCODE_KEY[m_nSU400_ButtonIndex];
                        char TMP_PARA[10];
                    
                        memset(TMP_PARA,0,10);
                        sprintf(TMP_PARA,"%d",nTmpKeyValue);

                        SU400_BTN_MouseUp(NULL, TMP_PARA);
                    }
                    //==========================

        		}
					break;
			}
		break;
	}

    return true;
}

bool SU400_JogSwitch(ITUWidget* widget, char* param)
{
    if(!m_bSU400ReadyToJOG || m_bSU400_GoTo_LayerMode) return false;
    SU400_ExitCNT = 0;
	uint8_t JogSwitch = 0;
	JogSwitch = param[0];
	
	if(JogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
        SU400_MenuTabESC();
	}
	else if(JogSwitch == JOG_DIAL_SW_HOME_OFF)
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}
    return true;
}
//======================================================

bool SU400_D7_MouseUp(ITUWidget* widget, char* param)
{   
    SU400_ExitCNT = 0;
    SU400_MenuTabEnter();
    return true;
}

bool SU400_D6_MouseUp(ITUWidget* widget, char* param)
{
    SU400_ExitCNT = 0;
    SU100_Send_Add_Cancel();//chpark.10.30
	GotoMain(MAIN_MODE_NORMAL); 

    return true;
}

bool SU400_D5_MouseUp(ITUWidget* widget, char* param)
{
    SU400_ExitCNT = 0;
   
    SU400_MenuTabESC();

    return true;
}

//======================================================
bool SU400_Timer(ITUWidget* widget, char* param)
{
    #ifdef USED_ROTATION //chpark.10.19
    if (RotationMode!=0)
    {
      return 0;
    }
    #endif

    //chpark.24.02.14
    #ifdef RELEASE_VER
	if ((UserPW_Time_AUI1+TIME_CNT_5SEC)<gMainTimerCnt_10m)
	{
		if(!m_bSU400_AUI1_Rcv_Error && !m_bSU400_GoTo_LayerMode && gUI_Server_Mode)
		{
            printf("\r\n AUI1 Protocol 5Sec Exit >>>>>>> goto Main\r\n");

            ituWidgetSetVisible(SU400_CONT_RESULT, true);
            ituWidgetSetVisible(SU400_RESULT_ICON[SU400_SUCCESS], false);
            ituWidgetSetVisible(SU400_RESULT_ICON[SU400_FAIL], true);

            m_bSU400_AUI1_Rcv_Error = true;
            m_bSU400_GoTo_LayerMode = true;
        }
	}
    #endif
    
    if((gMainTimerCnt_10m - SU400_SecTimer) >= 20)//100) //200msec
	{
		if(SU400_ExitCNT < 30*5)//30sec
			SU400_ExitCNT++;
		else if(SU400_ExitCNT == 30*5)//30sec
		{
			SU400_ExitCNT = 300;
            #ifdef RELEASE_VER
                SU100_Send_Add_Cancel();//chpark.10.30
                printf("\r\n SU400 Menu 30Sec Exit > goto Main\r\n");
			    GotoMain(MAIN_MODE_NORMAL); 
                return true;
	        #endif
			
		}
        
        //chpark.12.19
        // long delete check
		if(SU400_1SecCNT < 1*5)
			SU400_1SecCNT++;
		if(SU400_1SecCNT == 1*5)
		{
			SU400_1SecCNT = 100;
			//printf("\r\nLong delete key detected\r\n");

			if(SU400_FastFlag == SU400_DEL_AUTO_START)
			{
				SU400_FastFlag = SU400_DEL_AUTO_ON;
				SU400_FastTimer = gMainTimerCnt_10m;
			}
		}


		if(m_bSU400_GoTo_LayerMode)
		{
			m_nSU400_GoTo_LayerModeCnt++;

			if(m_nSU400_GoTo_LayerModeCnt>=2*5)//2sec
			{
                //chpark.12.19
                SU400_1SecCNT = 100;
                SU400_FastFlag = SU400_DEL_AUTO_OFF;

				m_bSU400_GoTo_LayerMode = false;
				m_nSU400_GoTo_LayerModeCnt=0;

                if(m_bSU400_AUI1_Rcv_Error)
                {
                    ituWidgetSetVisible(SU400_CONT_RESULT, false);//chpark.24.02.14
                    m_bSU400_AUI1_Rcv_Error = false;
                    #ifdef RELEASE_VER
                        //chpark.24.02.29
                        ITULayer *Menu7_Layer = ituSceneFindWidget(&theScene, "Menu7_Layer");
                        assert(Menu7_Layer);
                        ituLayerGoto(Menu7_Layer);
                        return true;
		            #endif
                }

                if(g_nSU100_Mode==SU100_Add_Mode)
                {
                    //success to register passcode.
                    memcpy(&User_RegInfo[g_nSU100_AddUserIdx].UserPassCode[0],SU400_Txt_NewPassCode, SU400_MAX_PW_CNT);
                    User_RegInfo[g_nSU100_AddUserIdx].nTmp_PassCode_Skip=0;
                    
                    if(g_nSU100_PreLayer==M7_SU200_EDIT_LAYER)
                    {
                        ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
                        assert(M7_SU200_EDIT);
                        ituLayerGoto(M7_SU200_EDIT);
                    }
                    else
                    {
                        g_nSU100_PreLayer = M7_SU400_PASS_LAYER; 
                
                        ITULayer *M7_SU500_RFID = ituSceneFindWidget(&theScene, "M7_SU500_RFID");
                        assert(M7_SU500_RFID);
                        ituLayerGoto(M7_SU500_RFID);
                    }
                }
                else if(g_nSU100_Mode==SU100_Edit_Mode)
                {
                    memcpy(&User_RegInfo[g_nSU100_EditUserIdx].UserPassCode[0],SU400_Txt_NewPassCode, SU400_MAX_PW_CNT);  

                    ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
                    assert(M7_SU200_EDIT);
                    ituLayerGoto(M7_SU200_EDIT);
                }
			}
		}

        if(m_bSU400_ErrorMsg_Show)
        {
            SU400_ErrorMsg_Cnt++;
            //printf("SU400_ErrorMsg_Cnt==%d\n",SU400_ErrorMsg_Cnt);
            if(SU400_ErrorMsg_Cnt>=2*5)//chpark.11.02
            {
                m_bSU400_ErrorMsg_Show = false;
                //printf("Error Msg Delete==>%d\n",SU400_Result_UI_Status);
                SU400_ErrorMsg_Cnt=0;

                if(SU400_Result_UI_Status==0)
                {
                    SU400_Show_Result_UI(-10);
                }
                else if(SU400_Result_UI_Status==1)
                {
                    //SU400_Show_Result_UI(-1);
                    SU400_Show_Result_UI(-10);//chpark.24.04.03
                }
                else if(SU400_Result_UI_Status==2)
                {
                    //SU400_Show_Result_UI(-2);
                    SU400_Show_Result_UI(-10);//chpark.24.04.03
                }
                else
                {
                    SU400_Show_Result_UI(-10);
                }
               
            }   
        } 

        SU400_CursorCNT++;
        if(SU400_FastFlag == SU400_DEL_AUTO_ON)//chpark.12.19
        {
            SU400_CursorCNT=0;
            m_bSU400_CursorShow = true;
            ituWidgetSetVisible(SU400_Cursor[0], m_bSU400_CursorShow); 
        }
        else
        {
            if(SU400_CursorCNT>=2)
            {
                SU400_CursorCNT=0;
                m_bSU400_CursorShow = !m_bSU400_CursorShow;
                ituWidgetSetVisible(SU400_Cursor[0], m_bSU400_CursorShow); 
            }
        }

        //======
        //data received chpark.08.22
        SU400_SendCNT++;
		
		if(SU400_SendCNT>=5)//1sec
		{
			SU400_SendCNT=0;
		}

        if(g_UM5_ChkTime>0 && !m_bSU400_GoTo_LayerMode)
        {
            g_UM5_ChkTime = 0;
           
            if(SU400_UM5_Rcv_Type == SU400_RCV_PASSCODE_CHK)
            {
                printf("[SU300]SU400_RCV_PASSCODE_CHK:%d\n",SU400_UM5_0Byte_01bit);

                SU400_UM5_Rcv_Type = SU400_RCV_INIT;
                if(SU400_UM5_0Byte_01bit== 1)//passcode register ok
                {
                    //true;
                    SU400_Chk_Result_InServer(1);
                }
                else if(SU400_UM5_0Byte_01bit== 2)//already registered passcode
                {
                    //chpark.12.13
                    //false;
                    SU400_Chk_Result_InServer(0);
                }
                else
                {
                    printf("[RET Error] in Passcode : %d \n",SU400_UM5_0Byte_01bit);
                }
            } 
        }
        //=====

		SU400_SecTimer = gMainTimerCnt_10m;
	}

     //chpark.12.19
    if(SU400_FastFlag == SU400_DEL_AUTO_ON)
	{
        printf("[SU400] SU400_FastFlag:SU400_DEL_AUTO_ON=====\n");
        if((gMainTimerCnt_10m - SU400_FastTimer) >= 10)//400ms
		{
            bool bRet = SU400_Delete_Char(0);   //delete char
            SU400_Display_PassCode(0);// show user passcode

            if(!bRet)
            {
                //printf("[SU400] END Auto delete=====\n");
                SU400_1SecCNT  = 100;
                SU400_FastFlag = SU400_DEL_AUTO_OFF;
            }

			SU400_FastTimer = gMainTimerCnt_10m;
		}
    }

    return true;
}


bool SU400_Send_TX(int nType)
{
    uint8_t  txbuffer[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    uint8_t  tmpLen = strlen(SU400_Txt_ConfirmPassCode);
    SU400_Passcode_InNumber = atoi(SU400_Txt_ConfirmPassCode);

    //printf("[SU400_Send_TX]%s=>> ToInt => :%d 0x%X\n",SU400_Txt_ConfirmPassCode, SU400_Passcode_InNumber,SU400_Passcode_InNumber);
    
    uint8_t curUserIdx;

    if(g_nSU100_Mode==SU100_Add_Mode)
    {
       curUserIdx = g_nSU100_AddUserIdx+1;//chpark.12.19
    }
    else if(g_nSU100_Mode==SU100_Edit_Mode)
    {
       curUserIdx = g_nSU100_EditUserIdx+1;//chpark.12.19
    }

    switch(nType)
    {
        case 0:
            //When in user addition mode, set to [0byte 1bit]=1
            if(g_nSU100_Mode==SU100_Add_Mode)
            {
                txbuffer[0] = 0x02;
            }
            else
            {
                txbuffer[0] = 0x00;
            }
            
            txbuffer[2] = curUserIdx; //1~15
            CanbusTxDataManagement( gMainTimerCnt_10m,UM1,txbuffer);//chpark.10.30    
            break;

        case 1:
            //UM2
            //0byte:[0:7bit]user number
            //1byte:[0:3bit]length
            //2-6byte:passcode : ex)9999->0x000000270F 
            txbuffer[0] = curUserIdx; //1~15 //chpark.12.19
            txbuffer[1] = tmpLen;
            txbuffer[2] = (SU400_Passcode_InNumber>>0)&0xFF;
            txbuffer[3] = (SU400_Passcode_InNumber>>8)&0xFF;
            txbuffer[4] = (SU400_Passcode_InNumber>>16)&0xFF;
            txbuffer[5] = (SU400_Passcode_InNumber>>24)&0xFF;
            txbuffer[6] = 0;
            txbuffer[7] = 0;
            
            CanbusTxDataManagement( gMainTimerCnt_10m,UM2,txbuffer);//chpark.07.18
            break;

        default:
            break;
    }
}
//======================================================



