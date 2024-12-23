#include <assert.h>

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"



void PW100_Save_CLI();//24.11.11
bool PW100_Delete_Char();

static bool     m_bPW100_ReadyToJOG_OffBtn = false;
static bool     m_bPW100_ReadyToJOG = false;
static int m_nPW100_ButtonIndex=0;//current selected key index. 0:skip 1-10:key 11:del 12:D7 

static bool m_bPW100_GoTo_LayerMode = false;


static uint32_t PW100_SecTimer;
static uint32_t PW100_ExitCNT = 0;


//button state
#define PW100_STATE_NONE   -1
#define PW100_STATE_BTN     0
#define PW100_STATE_SEL     1
#define PW100_STATE_DOWN    2

//password mode
#define PW100_NEW_PASSMODE  0
#define PW100_CF_PASSMODE   1
#define PW100_INPUT_PASSMODE   2

static int m_nPW100_PW_Mode     = 0;    //0:PW100_NEW_PASSMODE(Set) 1:PW100_CF_PASSMODE(set)  2:PW100_INPUT_PASSMODE(input)

#define PW100_MAX_PW_CNT	4

//key mapping
#define PW100_DEL_Key       11
#define PW100_D7_Key        12
#define PW100_PASS_ON_BTN    20
#define PW100_PASS_OFF_BTN   21

//language
#define PW100_LANG_ENG 0
#define PW100_LANG_GER 1
#define PW100_LANG_FR 2
#define PW100_LANG_IT 3
#define PW100_LANG_SP 4


#define PW100_SUCCESS		1
#define PW100_FAIL			0


static int PASSWORD_KEY[13];

uint32_t PW100_Passcode_InNumber = 0;

static char PW100_Txt_NewPassWord[MAX_PW100_PWD+2];
static char PW100_Txt_ConfirmPassWord[MAX_PW100_PWD+2];
static char PW100_Txt_InputPassWord[MAX_PW100_PWD+2];
static char PW100_Txt_CurPassWord[MAX_PW100_PWD+2];
//=====================================
static ITUWidget* PW100_CONT_RESULT; //
static ITUWidget* PW100_RESULT_ICON[2]; //
//=====================================


static ITUWidget* PW100_BG_TYPE[2];  //0:PW100_MODE_SET   1:PW100_MODE_INPUT
//=====================================
static ITUWidget* PW100_TXT[5]; //
static ITUWidget* PW100_BTN[2][3];   //[type:on/off][stats:BTN/SEL/DOWN]

static ITUWidget* PW100_TXTP[5]; //
//=====================================

//=====================================
static ITUWidget* PW100_CONT_UserPW_Data;
static ITUWidget* PW100_CONT_UserPW_Bt;
static ITUWidget* PW100_CONT_UserPW_Sel;

static ITUWidget* PW100_CONT_New_Pass_Icon;
static ITUWidget* PW100_CONT_CF_Pass_Icon;
static ITUWidget* PW100_CONT_Input_Pass_Icon;

static ITUIcon* PW100_Icon_New_Data[PW100_MAX_PW_CNT];
static ITUIcon* PW100_Icon_CF_Data[PW100_MAX_PW_CNT];
static ITUIcon* PW100_Icon_Input_Data[PW100_MAX_PW_CNT];
       


static ITUIcon* PW100_UserPW_Sel[15];
static ITUIcon* PW100_UserPW_Bt[15];

static ITUWidget* PW100_ICON_DOWN;
//=====================================
static ITUWidget* PW100_ICON_D7_SEL;
static ITUWidget* PW100_ICON_D7_DOWN;

//=====================================
int PW100_Name_Char_Index=0;
uint8_t PW100_Key_SelIndex=0xFF;
//=====================================

void PW100_MenuGoToLayer(int nLayerType);
//=====================================
void PW100_MenuTabPrev();
void PW100_MenuTabNext();
void PW100_MenuTabUp();
void PW100_MenuTabDown();
void PW100_MenuTabLeft();
void PW100_MenuTabRight();
void PW100_MenuTabESC(bool bD5_Back);
void PW100_MenuTabEnter();
//=================================


void PW100_InitControl();
void PW100_InitKeyMaping();

void PW100_Display_PassCode();

bool PW100_Button_Select(int nIndex,bool bDownImage);


void PW100_Show_Result_UI(int nRes);

void PW100_Set_Layer_Mode(int nMode);
void PW100_Set_PassMode_On(int nMode);

void PW100_Passcode_Clear(int nMode);
int  PW100_Passcode_Check();



bool    m_bPW100_ResultUI_Show = false;//chpark.11.02
int     PW100_ResultUI_Cnt = 0;//chpark.11.02
int     PW100_ResultUI_Status; //-1:none  0:X  1:O  2:X+text  3:O+text 

static bool PW100_b_VMode = false;//chpark.01.15

static int PW100_input_toggle_cnt;

bool PW100_Enter(ITUWidget* widget, char* param)
{
    if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
	{
        PW100_b_VMode = false;
    }
    else
    {   
        PW100_b_VMode = true;
    }
    gMB_Language = Lang_Flag[M7_UserIndex];//chpark.24.02.19
    m_bPW100_GoTo_LayerMode = false;

    PW100_input_toggle_cnt = 0;

    //PW100_CursorCNT=0;
    PW100_ResultUI_Cnt=0;
    m_bPW100_ResultUI_Show = false;

    PW100_SecTimer = gMainTimerCnt_10m;
	PW100_ExitCNT = 0;

    PW100_ResultUI_Status = -1;//none

    int i=0,j,tIndex=0;
    m_nPW100_ButtonIndex = 0;

    memset(PW100_Txt_NewPassWord, '\0', sizeof(PW100_Txt_NewPassWord));
    memset(PW100_Txt_ConfirmPassWord, '\0', sizeof(PW100_Txt_ConfirmPassWord));
    memset(PW100_Txt_InputPassWord, '\0', sizeof(PW100_Txt_InputPassWord));

    memset(PW100_Txt_CurPassWord, '\0', sizeof(PW100_Txt_CurPassWord));

    PW100_InitControl();
    
    PW100_InitKeyMaping();

    
    memset(PW100_Txt_CurPassWord, '\0', sizeof(PW100_Txt_CurPassWord));
    memcpy(PW100_Txt_CurPassWord,PW100_PWD,MAX_PW100_PWD);
    int len_cur;
    len_cur = strlen(PW100_Txt_CurPassWord);

    //=====================================================================
    int nErrNum=0;
    for(int i=0;i<MAX_PW100_PWD;i++)
    {
        if(PW100_Txt_CurPassWord[i]>=0x30 && PW100_Txt_CurPassWord[i]<=0x39)
        {
               //0~9 num 
        }
        else
        {
            nErrNum++;
            //printf("Error PW[%d]:%s\n",i,PW100_Txt_CurPassWord[i]);
        }
    }
    
    if(len_cur<4 || nErrNum>0)
    {
        memset(PW100_Txt_CurPassWord, '\0', sizeof(PW100_Txt_CurPassWord));
        memset(PW100_PWD, '\0', sizeof(PW100_PWD));
        len_cur = strlen(PW100_Txt_CurPassWord);
        printf("[PW100_Enter] Reset Old Pass.: nErrNum:%d\n",nErrNum);
        
    }
    //=====================================================================

    if(len_cur<4 || nErrNum>0)
    {
        printf("[PW100_Enter]  CurPass:%s Cur Password Length Error:%d\n PW SET Mode change.\n",PW100_Txt_CurPassWord,len_cur);
        //main
        g_nPW100_LayerType = PW100_MODE_SET;
        PW100_Set_Layer_Mode(g_nPW100_LayerType);
        //sub
        g_bPW100_PW_OnOff = PW100_PASS_MODE_OFF;
        PW100_Set_PassMode_On(g_bPW100_PW_OnOff);
        PW100_Button_Select(PW100_PASS_OFF_BTN, true);//focus off btn
    }
    else
    {
        printf("[PW100_Enter]  CurPass:%s Cur Password Length:%d\n ",PW100_Txt_CurPassWord,len_cur);
        //main
        PW100_Set_Layer_Mode(g_nPW100_LayerType);
        //sub

        //set mode off when it start.
        //Enter can only be entered when mode is off.
        //Mode on starts with the timer.
        //Enter에 들어올 경우는 오직 mode off일때 밖에 없다.
        //Mode on으로 시작은 timer에서 한다.
        if(g_nPW100_LayerType == PW100_MODE_SET)
        {
            g_bPW100_PW_OnOff = PW100_PASS_MODE_OFF;
            PW100_Set_PassMode_On(g_bPW100_PW_OnOff);
            PW100_Button_Select(PW100_PASS_OFF_BTN, true);//focus off btn
        }
        else
        {
            //do nothing.
        }
    }

    PW100_Show_Result_UI(-1);
    
    //In set mode, always regardless of passmode on/off
    //Focus should be passmode_on set.
    //set모드에서는 passmode on/off와 상관없이 항상
    //set에 포커스를 맞춰야 한다. 
    //chpark.24.11.11
    if(g_nPW100_LayerType==PW100_MODE_SET)
    {
        printf("[PW100_Enter]  PW Setup Mode\n");
        PW100_Button_Select(PW100_PASS_ON_BTN, true);//focus on btn by default.
    }
    else if(g_nPW100_LayerType==PW100_MODE_INPUT)
    {
        printf("[PW100_Enter]  PW Input Mode:\n");
        PW100_Button_Select(1, false);//focus 1
    }
    else
    {
        printf("[PW100_Enter] PW Input Mode Error:%d\n",g_nPW100_LayerType);
    }

    PW100_input_toggle_cnt = 0;//reset btn touch count

    m_bPW100_ReadyToJOG = true;
    m_bPW100_ReadyToJOG_OffBtn = false; //chpark.24.11.04

    return true;
}


void PW100_InitControl()
{
    int tIndex=0;
    char tmp_WidgetName[100];
//Control setting
printf("PW100===========Setup\n");
    SetWidget(PW100_BG_TYPE[PW100_MODE_SET],"PW100_BG");

        SetWidget(PW100_TXT[PW100_LANG_ENG],"PW100_TXT_ENG");
        SetWidget(PW100_TXT[PW100_LANG_GER],"PW100_TXT_GER");
        SetWidget(PW100_TXT[PW100_LANG_FR], "PW100_TXT_FR");
        SetWidget(PW100_TXT[PW100_LANG_IT], "PW100_TXT_IT");
        SetWidget(PW100_TXT[PW100_LANG_SP], "PW100_TXT_SP");

        SetWidget(PW100_BTN[PW100_PASS_MODE_ON][PW100_STATE_BTN],  "PW100_BTN_ON");  //button itself
        SetWidget(PW100_BTN[PW100_PASS_MODE_OFF][PW100_STATE_BTN], "PW100_BTN_OFF"); //button itself
        SetWidget(PW100_BTN[PW100_PASS_MODE_ON][PW100_STATE_SEL],  "PW100_BTN_ON_SEL");  //button select
        SetWidget(PW100_BTN[PW100_PASS_MODE_OFF][PW100_STATE_SEL], "PW100_BTN_OFF_SEL"); //button select
        SetWidget(PW100_BTN[PW100_PASS_MODE_ON][PW100_STATE_DOWN],  "PW100_BTN_ON_DOWN"); //button down
        SetWidget(PW100_BTN[PW100_PASS_MODE_OFF][PW100_STATE_DOWN], "PW100_BTN_OFF_DOWN"); //button down
   

        SetWidget(PW100_CONT_New_Pass_Icon, "PW100_New_PW_Cont"); 
        SetWidget(PW100_CONT_CF_Pass_Icon, "PW100_Confirm_PW_Cont"); 

        ituWidgetSetVisible(PW100_CONT_New_Pass_Icon, true);
        ituWidgetSetVisible(PW100_CONT_CF_Pass_Icon,  true);

        for (int i=0;i<PW100_MAX_PW_CNT;i++)
        {
            //new passcode
            memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
            sprintf(tmp_WidgetName, "PW100_New_Data%d",i+1);
            SetWidget(PW100_Icon_New_Data[i],tmp_WidgetName);
            ituWidgetSetVisible(PW100_Icon_New_Data[i], false);
            //confirm passcode
            memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
            sprintf(tmp_WidgetName, "PW100_CF_Data%d",i+1);
            SetWidget(PW100_Icon_CF_Data[i],tmp_WidgetName);
            ituWidgetSetVisible(PW100_Icon_CF_Data[i], false);
        }

printf("PW100===========Input\n");     
    SetWidget(PW100_BG_TYPE[PW100_MODE_INPUT],"PW100_BG_INPUT");
      
        SetWidget(PW100_TXTP[PW100_LANG_ENG],"PW100_TXTP_ENG");
        SetWidget(PW100_TXTP[PW100_LANG_GER],"PW100_TXTP_GER");
        SetWidget(PW100_TXTP[PW100_LANG_FR], "PW100_TXTP_FR");
        SetWidget(PW100_TXTP[PW100_LANG_IT], "PW100_TXTP_IT");
        SetWidget(PW100_TXTP[PW100_LANG_SP], "PW100_TXTP_SP");

        SetWidget(PW100_CONT_Input_Pass_Icon, "PW100_PW_Cont"); 
        ituWidgetSetVisible(PW100_CONT_Input_Pass_Icon,  true);

        for (int i=0;i<PW100_MAX_PW_CNT;i++)
        {
            //input passcode
            memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
            sprintf(tmp_WidgetName, "PW100_PW_Data%d",i+1);
            SetWidget(PW100_Icon_Input_Data[i],tmp_WidgetName);
            ituWidgetSetVisible(PW100_Icon_Input_Data[i], false);
        }

//printf("PW100===========33333 BTN SEL\n");
    for (int i=0;i<11;i++)
	{
        memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
		sprintf(tmp_WidgetName, "PW100_UserPW_Sel%02d",i+1);
		SetWidget(PW100_UserPW_Sel[i],tmp_WidgetName);
		sprintf(tmp_WidgetName, "PW100_UserPW_Bt%02d",i+1);
		SetWidget(PW100_UserPW_Bt[i],tmp_WidgetName);

        ituWidgetSetVisible(PW100_UserPW_Sel[i], false);
		ituWidgetSetVisible(PW100_UserPW_Bt[i],  true);
        //printf("PW100_UserPW_Bt==%d\n",i);
	}

//printf("PW100===========4444 Result\n");
    SetWidget(PW100_CONT_RESULT, "PW100_CONT_RESULT");
        SetWidget(PW100_RESULT_ICON[PW100_SUCCESS],"PW100_OK");
        SetWidget(PW100_RESULT_ICON[PW100_FAIL],   "PW100_NO");

    //================================================================  
    SetWidget(PW100_ICON_DOWN,   "PW100_ICON_DOWN");
     
    SetWidget(PW100_ICON_D7_SEL, "PW100_ICON_D7_SEL");
    SetWidget(PW100_ICON_D7_DOWN,"PW100_ICON_D7_DOWN");
    //================================================================ 

    

// UI setting
    for(int i=0; i<5; i++)
    {  
        ituWidgetSetVisible(PW100_TXT[i], false);
        ituWidgetSetVisible(PW100_TXTP[i], false);
    }
    ituWidgetSetVisible(PW100_TXT[gMB_Language],  true);
    ituWidgetSetVisible(PW100_TXTP[gMB_Language], true);
    
//result image (o,X)
    ituWidgetSetVisible(PW100_CONT_RESULT, false);

//ETC
    ituWidgetSetVisible(PW100_ICON_DOWN,     false);
    ituWidgetSetVisible(PW100_ICON_D7_SEL,   false);
    ituWidgetSetVisible(PW100_ICON_D7_DOWN,  false);  
}

void PW100_InitKeyMaping()
{
    //current selected key index. 0:skip 1-10:key 11:del 12:D7 
    PASSWORD_KEY[0] =   0;
    PASSWORD_KEY[1] =   4901;
    PASSWORD_KEY[2] =   5002;
    PASSWORD_KEY[3] =   5103;
    PASSWORD_KEY[4] =   5204;
    PASSWORD_KEY[5] =   5305;
    PASSWORD_KEY[6] =   5406;
    PASSWORD_KEY[7] =   5507;
    PASSWORD_KEY[8] =   5608;
    PASSWORD_KEY[9] =   5709;
    PASSWORD_KEY[10] =  4810;
    PASSWORD_KEY[11] =  12711;//del
    PASSWORD_KEY[12] =  100012;//D7
}

bool PW100_Button_Select(int nIndex,bool bDownImage)
{
    //chpark.24.10.15
    int nTmpPrevIndex = m_nPW100_ButtonIndex;

    if(nIndex==PW100_D7_Key)
    {
        //do nothing
        return false;
    }

    if(nTmpPrevIndex==PW100_PASS_ON_BTN && nIndex == PW100_PASS_OFF_BTN)
        PW100_input_toggle_cnt++;
    if(nTmpPrevIndex==PW100_PASS_OFF_BTN && nIndex == PW100_PASS_ON_BTN)
        PW100_input_toggle_cnt++;

    //save new key index
    m_nPW100_ButtonIndex = nIndex;

    //===================
    //clear
    ituWidgetSetVisible(PW100_BTN[PW100_PASS_MODE_ON][PW100_STATE_SEL],  false);
    ituWidgetSetVisible(PW100_BTN[PW100_PASS_MODE_OFF][PW100_STATE_SEL], false);

    for(int i=0;i<11;i++)
    {
        ituWidgetSetVisible(PW100_UserPW_Sel[i], false);
    }
 
    ituWidgetSetVisible(PW100_ICON_D7_SEL,  false);
    ituWidgetSetVisible(PW100_ICON_DOWN, false);

    if(nIndex<0)
    {
        //remove all selection
        return true;
    }
    //===================

    //select button
    int x, y;

    if(nIndex == PW100_PASS_ON_BTN)
    {     
        ituWidgetSetVisible(PW100_BTN[PW100_PASS_MODE_ON][PW100_STATE_SEL], true);
    }
    else if(nIndex == PW100_PASS_OFF_BTN)
    {
        ituWidgetSetVisible(PW100_BTN[PW100_PASS_MODE_OFF][PW100_STATE_SEL], true);
    }
    else if(nIndex>=1 && nIndex<=11)
    {
        //printf("[%d]Button POS[1] : x=%d y=%d========\n",nIndex,x,y);
        ituWidgetSetVisible(PW100_UserPW_Sel[nIndex-1], true);

        ituWidgetGetGlobalPosition(PW100_UserPW_Bt[nIndex-1], &x, &y);
            
        if(bDownImage)
        {
            ituWidgetSetVisible(PW100_ICON_DOWN, true);
            //ituWidgetSetPosition(PW100_ICON_DOWN,x-6, y-6);
            ituWidgetSetPosition(PW100_ICON_DOWN,x, y);
        }
        else
        {
            ituWidgetSetVisible(PW100_ICON_DOWN, false);
        }

        ituWidgetSetVisible(PW100_ICON_D7_SEL, false);
        ituWidgetSetVisible(PW100_ICON_D7_DOWN, false);
            
        //printf("[%d]Button POS[2] : x=%d y=%d========\n",nIndex,x,y);
    }
    else if(nIndex==PW100_D7_Key)
    {
        /*
        //D7 Button
        ituWidgetSetVisible(PW100_ICON_D7_SEL,  true);
        if(bDownImage)
        {
            ituWidgetSetVisible(PW100_ICON_D7_DOWN, true);
        }
        else
        {
            ituWidgetSetVisible(PW100_ICON_D7_DOWN, false);
        }
        */
    }
    else
    {
        printf("[PW100_Button_Select] Select Error=%d \n",PW100_Button_Select);
    }

    return true;
}

//======================================================
void PW100_Display_PassCode()
{
    if(g_nPW100_LayerType == PW100_MODE_SET)
    {
        int len_new,len_cf,len_total;
        len_new = strlen(PW100_Txt_NewPassWord);
        len_cf  = strlen(PW100_Txt_ConfirmPassWord);

        len_total = len_new + len_cf;

        //clear
        for (int i=0;i<PW100_MAX_PW_CNT;i++)
        {
            ituWidgetSetVisible(PW100_Icon_New_Data[i], false);
            ituWidgetSetVisible(PW100_Icon_CF_Data[i], false);
        }
            
        //calculate num of passcode character
        for(int i=0;i<len_new;i++)
        {
            ituWidgetSetVisible(PW100_Icon_New_Data[i], true);  
            printf("[PW100_Display_PassCode] [New]ituWidgetIsVisible=%d\n",ituWidgetIsVisible(PW100_Icon_New_Data[i]));
        }
        for(int i=0;i<len_cf;i++)
        {
            ituWidgetSetVisible(PW100_Icon_CF_Data[i], true);  
            int nX  = ituWidgetGetX(PW100_Icon_CF_Data[i]);
            int nY  = ituWidgetGetY(PW100_Icon_CF_Data[i]);
            printf("[PW100_Display_PassCode] [CF]nX=%d nY=%d ituWidgetIsVisible=%d\n",nX,nY,ituWidgetIsVisible(PW100_Icon_CF_Data[i]));
        }
        printf("\n[PW100_Display_PassCode][set data] PW100_Txt_NewPassWord=%s      len_new=%d\n",PW100_Txt_NewPassWord,len_new);
        printf("[PW100_Display_PassCode][set data] PW100_Txt_ConfirmPassWord=%s  len=%d\n",PW100_Txt_ConfirmPassWord,len_cf);
    }
    else if(g_nPW100_LayerType == PW100_MODE_INPUT)
    {
        int len_input;
        len_input = strlen(PW100_Txt_InputPassWord);
        printf("[PW100_Display_PassCode][input data] PW100_Txt_InputPassWord=%s  len=%d\n",PW100_Txt_InputPassWord,len_input);
        //clear
        for (int i=0;i<PW100_MAX_PW_CNT;i++)
        {
            ituWidgetSetVisible(PW100_Icon_Input_Data[i], false);
        }

        //calculate num of passcode character
        for(int i=0;i<len_input;i++)
        {
            ituWidgetSetVisible(PW100_Icon_Input_Data[i], true);  
            printf("[PW100_Display_PassCode][input data]show [*][%d]\n",i);
        }
    }
}



//======================================================
bool PW100_BTN_MouseDn(ITUWidget* widget, char* param)
{
    if(m_bPW100_GoTo_LayerMode) return false;
    if(m_bPW100_ResultUI_Show) return false;

    PW100_ExitCNT = 0;
    int nKeyParam,nKeyIndex;
    uint8_t nASC;
    nKeyParam = atoi(param);
    //printf("PW100_BTN_MouseDn nKeyParam=%d\n",nKeyParam);

    nASC = nKeyParam/100;
    nKeyIndex = nKeyParam%100;

    //password off가 되어있을 때엔, set password , no password, 버튼이외에는 키가 작동하면 안된다.
    if(g_nPW100_LayerType == PW100_MODE_SET && g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF)
    {
        if(nKeyIndex==PW100_PASS_ON_BTN || nKeyIndex==PW100_PASS_OFF_BTN)
        {
            //go on next step.
        }
        else
        {
            printf("[PW100_BTN_MouseDn][g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF] Key doesn't work. \n");
            return false;
        }
    }

    //key select ui
    PW100_Button_Select(nKeyIndex,true);

    return true;
}

bool PW100_BTN_MouseUp(ITUWidget* widget, char* param)
{
    //printf("[PW100_BTN_MouseUp] m_bPW100_ResultUI_Show=%d m_bPW100_GoTo_LayerMode=%d\n",m_bPW100_ResultUI_Show,m_bPW100_GoTo_LayerMode);
    if(m_bPW100_ResultUI_Show) return false;
    if(m_bPW100_GoTo_LayerMode) return false;

    //key select UI
    PW100_ExitCNT = 0;
    int nKeyParam,nKeyIndex;
    uint8_t nASC;
    nKeyParam = atoi(param);

    nASC = (uint8_t)(nKeyParam/100);
    nKeyIndex = nKeyParam%100;
    printf("[PW100_BTN_MouseUp] g_nPW100_LayerType=%d nKeyParam=%d nASC=%d nKeyIndex=%d\n",g_nPW100_LayerType,nKeyParam,nASC,nKeyIndex);
    
    //key up UI
    ituWidgetSetVisible(PW100_ICON_DOWN, false);
    ituWidgetSetVisible(PW100_ICON_D7_DOWN, false);

    //password off가 되어있을 때엔, set password , no password, 버튼이외에는 키가 작동하면 안된다.
    if(g_nPW100_LayerType == PW100_MODE_SET && g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF)
    {
        if(nKeyIndex==PW100_PASS_ON_BTN || nKeyIndex==PW100_PASS_OFF_BTN)
        {
            //go on next step.
        }
        else
        {
            printf("[PW100_BTN_MouseUp][g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF] Key doesn't work. \n");
            return false;
        }
    }
    
    //============================================
    //key input
    if(nKeyIndex==PW100_PASS_ON_BTN)
    {
        if(g_nPW100_LayerType == PW100_MODE_SET)
        {
            PW100_Set_PassMode_On(PW100_PASS_MODE_ON);   
            PW100_Button_Select(1, false);//focus 1
        }
        else
        {
            printf("[PW100_BTN_MouseUp][PW100_PASS_ON_BTN] Error. Mode Set\n");
        }
    }
    else if(nKeyIndex==PW100_PASS_OFF_BTN)
    {   
        if(g_nPW100_LayerType == PW100_MODE_SET)
        {
            memset(PW100_PWD, '\0', sizeof(PW100_PWD));
            PW100_Save_CLI();

            PW100_Set_PassMode_On(PW100_PASS_MODE_OFF); 
            PW100_Button_Select(PW100_PASS_OFF_BTN, true);//focus off btn

            PW100_Passcode_Clear(PW100_NEW_PASSMODE);	
            PW100_Passcode_Clear(PW100_CF_PASSMODE);
            PW100_Display_PassCode();	
           
        }
        else
        {
            printf("[PW100_BTN_MouseUp][PW100_PASS_OFF_BTN] Error. Mode Set\n");
        }
    }
    else if(nKeyIndex==PW100_DEL_Key)//Del Key
    {
        PW100_Delete_Char();   //delete char

        PW100_Display_PassCode();
    }
    else if(nKeyIndex>=1 && nKeyIndex<=10)
    {
        if(g_nPW100_LayerType == PW100_MODE_SET)
        {
            int len_new,len_cf,len_total;
            len_new = strlen(PW100_Txt_NewPassWord);
            len_cf  = strlen(PW100_Txt_ConfirmPassWord);

            len_total = len_new + len_cf;

            if(len_new<PW100_MAX_PW_CNT)
            {
                PW100_Txt_NewPassWord[len_new] = (char)nASC;//ascii char input
                PW100_Txt_NewPassWord[len_new+1] = '\0';//ascii char input

                PW100_Display_PassCode();
            }
            else if(len_new==PW100_MAX_PW_CNT && len_cf<PW100_MAX_PW_CNT)
            {
                //printf("\n[PassCode:NUM] len=%d input=%s \n\n",len,PW100_Txt_NewPassWord);
                if(len_cf>=PW100_MAX_PW_CNT)
                {
                    printf("\n[PW100:Len Over]error len=%d input=%s \n\n",len_cf,PW100_Txt_ConfirmPassWord);
                    PW100_Display_PassCode();
                    return false;
                }

                PW100_Txt_ConfirmPassWord[len_cf] = (char)nASC;//ascii char input
                PW100_Txt_ConfirmPassWord[len_cf+1] = '\0';//ascii char input

                len_cf  = strlen(PW100_Txt_ConfirmPassWord);

                PW100_Display_PassCode();

                if(len_cf>=PW100_MAX_PW_CNT)
                {
                    PW100_MenuTabEnter();
                    return true;
                }

            }
            else
            {
                printf("\n[PW100:Len Over]error new_pw=%s cf_pw=%s \n\n",PW100_Txt_NewPassWord,PW100_Txt_ConfirmPassWord);
                //Error.
            }
        }
        else if(g_nPW100_LayerType == PW100_MODE_INPUT)
        {
            int len_input;
            len_input = strlen(PW100_Txt_InputPassWord);

            printf("[PW100_BTN_MouseUp][PW100_MODE_INPUT] PW100_Txt_InputPassWord:%s\n",PW100_Txt_InputPassWord);

            if(len_input<PW100_MAX_PW_CNT)
            {
                PW100_Txt_InputPassWord[len_input] = (char)nASC;//ascii char input
                PW100_Txt_InputPassWord[len_input+1] = '\0';//ascii char input
            }

            PW100_Display_PassCode();

            len_input = strlen(PW100_Txt_InputPassWord);

            if(len_input>=PW100_MAX_PW_CNT)
            {
                PW100_MenuTabEnter();
                return true;
            }

           
        }
    }
    else if(nKeyIndex==PW100_D7_Key)//D7 Key
    {
        //noting to do  
    }
   
    //=========================================
    return true;
}

bool PW100_Delete_Char()
{
    bool bRet = false;

    if(g_nPW100_LayerType==PW100_MODE_SET)
    {
        int len_new,len_cf,len_total;
        len_new = strlen(PW100_Txt_NewPassWord);
        len_cf  = strlen(PW100_Txt_ConfirmPassWord);

        len_total = len_new + len_cf;
        
        if(len_total<=PW100_MAX_PW_CNT)
        {
            if((len_new-1)<0)
            {
                //printf("Cannot delete char.[Error====] len:%d\n",len-1);
                return bRet;
            }

            PW100_Txt_NewPassWord[len_new-1] = '\0';//ascii char input
            bRet = true;
            printf("[PW100_Delete_Char] PW100_Txt_NewPassWord=%s\n",PW100_Txt_NewPassWord);
        }
        else
        {
            if((len_cf-1)<0)
            {
                //printf("Cannot delete char.[Error====] len:%d\n",len-1);
                return bRet;
            }

            PW100_Txt_ConfirmPassWord[len_cf-1] = '\0';//ascii char input
            bRet = true;
            printf("[PW100_Delete_Char] PW100_Txt_ConfirmPassWord=%s\n",PW100_Txt_ConfirmPassWord);
        }
    }
    else if(g_nPW100_LayerType==PW100_MODE_INPUT)
    {
        int len_input;
        len_input = strlen(PW100_Txt_InputPassWord);

        if((len_input-1)<0)
        {
            //printf("Cannot delete char.[Error====] len:%d\n",len-1);
            return bRet;
        }

        PW100_Txt_InputPassWord[len_input-1] = '\0';//ascii char input
        bRet = true;
        
        printf("\n[PW100_Delete_Char]PW100_Txt_InputPassWord=%s \n",PW100_Txt_InputPassWord);
    }
    else
    {
        printf("[PW100_Delete_Char] m_nPW100_PW_Mode=%d\n",m_nPW100_PW_Mode);
    }

    return bRet;
}

//======================================================
void PW100_Set_Layer_Mode(int nMode)
{
    if(nMode==PW100_MODE_SET)
    {
        //Password Setup Mode
        g_nPW100_LayerType = nMode;

        ituWidgetSetVisible(PW100_BG_TYPE[PW100_MODE_SET],   true);
        ituWidgetSetVisible(PW100_BG_TYPE[PW100_MODE_INPUT], false);
        
        if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
        {
            ituWidgetSetPosition(PW100_CONT_RESULT,157,392);
        }
        else
        {
             ituWidgetSetPosition(PW100_CONT_RESULT,92,495);
        }

        PW100_Passcode_Clear(PW100_NEW_PASSMODE);	
        PW100_Passcode_Clear(PW100_CF_PASSMODE);	
    }
    else if(nMode==PW100_MODE_INPUT)
    {
        //Password Input Mode
        g_nPW100_LayerType = nMode;

        ituWidgetSetVisible(PW100_BG_TYPE[PW100_MODE_SET],   false);
        ituWidgetSetVisible(PW100_BG_TYPE[PW100_MODE_INPUT], true);

        if((RotationDir() == PORTRAIT_1)||(RotationDir() == PORTRAIT_2))
        {
            ituWidgetSetPosition(PW100_CONT_RESULT,157,675);
        }
        else
        {
            ituWidgetSetPosition(PW100_CONT_RESULT,92,495);
        }	

        PW100_Passcode_Clear(PW100_INPUT_PASSMODE);	
    }
    else
    {
        printf("[PW100_Set_Layer_Mode]Error:nMode=%d\n",nMode);
    }
}

void PW100_Set_PassMode_On(int nMode)
{
    if(g_nPW100_LayerType == PW100_MODE_SET)
    {
        if(nMode==PW100_PASS_MODE_ON)
        {   
            printf("[PW100_Set_PassMode_On]PW100_PASS_MODE_ON=%d\n",nMode);
            g_bPW100_PW_OnOff = nMode;
            
            ituWidgetSetVisible(PW100_BTN[PW100_PASS_MODE_ON][PW100_STATE_DOWN],  false);
            ituWidgetSetVisible(PW100_BTN[PW100_PASS_MODE_OFF][PW100_STATE_DOWN], true);
        }
        else if(nMode==PW100_PASS_MODE_OFF)
        {
            printf("[PW100_Set_PassMode_On]PW100_PASS_MODE_OFF=%d\n",nMode);
            g_bPW100_PW_OnOff = nMode;
        
            ituWidgetSetVisible(PW100_BTN[PW100_PASS_MODE_ON][PW100_STATE_DOWN],  true);
            ituWidgetSetVisible(PW100_BTN[PW100_PASS_MODE_OFF][PW100_STATE_DOWN], false);

            XO_Save_Config(M7_UserIndex,true,0); //always save
        }
        else
        {
            printf("[PW100_Set_PassMode_On]Error:nMode=%d\n",nMode);
        }
    }
}


void PW100_Passcode_Clear(int nMode)
{
    printf("[PW100_Passcode_Clear]nMode=%d\n",nMode);
    if(nMode==PW100_NEW_PASSMODE)
    {
        for (int i=0;i<PW100_MAX_PW_CNT;i++)
	    {
		    ituWidgetSetVisible(PW100_Icon_New_Data[i], false);
	    }
        memset(PW100_Txt_NewPassWord, '\0', sizeof(PW100_Txt_NewPassWord));
    }
    else if(nMode==PW100_CF_PASSMODE)
    {
        for (int i=0;i<PW100_MAX_PW_CNT;i++)
	    {
		    ituWidgetSetVisible(PW100_Icon_CF_Data[i], false);
	    }
        memset(PW100_Txt_ConfirmPassWord, '\0', sizeof(PW100_Txt_ConfirmPassWord));
    }
    else if(nMode==PW100_INPUT_PASSMODE)
    {
        for (int i=0;i<PW100_MAX_PW_CNT;i++)
	    {
		    ituWidgetSetVisible(PW100_Icon_Input_Data[i], false);
	    }
        memset(PW100_Txt_InputPassWord, '\0', sizeof(PW100_Txt_InputPassWord));
    }
    else
    {
        printf("[PW100_Passcode_Clear]Error:nMode=%d\n",nMode);
    }
}
//======================================================

int PW100_Passcode_Check()
{
    //check new passcode & confirm passcode.
    int nResult = 0;
    
    if(g_nPW100_LayerType==PW100_MODE_SET)
    {
        nResult = strcmp(PW100_Txt_NewPassWord,PW100_Txt_ConfirmPassWord);
    }
    else
    {
        memset(PW100_Txt_CurPassWord, '\0', sizeof(PW100_Txt_CurPassWord));
        memcpy(PW100_Txt_CurPassWord,PW100_PWD,MAX_PW100_PWD);
        nResult = strcmp(PW100_Txt_InputPassWord, PW100_Txt_CurPassWord);
    }

    printf("[PW100_Passcode_Check] = %d \n", nResult);

    return nResult;
}


void PW100_Show_Result_UI(int nRes)
{
    //int  PW100_ResultUI_Status; //-1:none  0:X  1:O  2:X clear 
    //Show O,X UI
    PW100_ResultUI_Status = nRes;

    if(PW100_ResultUI_Status<0)//chpark.24.01.31
    {
        ituWidgetSetVisible(PW100_ICON_DOWN,     false);
        ituWidgetSetVisible(PW100_ICON_D7_SEL,   false);
        ituWidgetSetVisible(PW100_ICON_D7_DOWN,  false);  
    }

    switch(PW100_ResultUI_Status)
    {
        case -1: //Reset all
            {
                ituWidgetSetVisible(PW100_CONT_RESULT, false);

                //delete all passcode & set focus in new passcode
                if(g_nPW100_LayerType==PW100_MODE_SET)
                {
                    PW100_Passcode_Clear(PW100_CF_PASSMODE);
                    PW100_Passcode_Clear(PW100_NEW_PASSMODE);
                }
                else
                {
                    PW100_Passcode_Clear(PW100_INPUT_PASSMODE);
                }                

                printf("[PW100_Show_Result_UI]Clear All====\n");
                m_bPW100_ResultUI_Show = false;
            }
            break;

        case 0://Fail . Only X (new pass!= cf pass)
            //The passcode confirmation does not match
            //Error Message

            //X Icon.    
            ituWidgetSetVisible(PW100_CONT_RESULT, true);
            ituWidgetSetVisible(PW100_RESULT_ICON[PW100_SUCCESS], false);
            ituWidgetSetVisible(PW100_RESULT_ICON[PW100_FAIL], true);
            PW100_ResultUI_Cnt = 0;
            m_bPW100_ResultUI_Show = true;

            break;
    
       
        case 1://Success
         
            //V Icon.    
            ituWidgetSetVisible(PW100_CONT_RESULT, true);
            ituWidgetSetVisible(PW100_RESULT_ICON[PW100_SUCCESS], true);
            ituWidgetSetVisible(PW100_RESULT_ICON[PW100_FAIL], false);
            PW100_ResultUI_Cnt = 0;
            m_bPW100_ResultUI_Show = true;
            break;

        case 2: //X + Clear
            {
                ituWidgetSetVisible(PW100_CONT_RESULT, false);

                //delete all passcode & set focus in new passcode
                if(g_nPW100_LayerType==PW100_MODE_SET)
                {
                    PW100_Passcode_Clear(PW100_CF_PASSMODE);
                }
                else
                {
                    PW100_Passcode_Clear(PW100_INPUT_PASSMODE);
                }                

                //printf("Clear All====\n");
                m_bPW100_ResultUI_Show = false;
            }
            break;

        default:
   
            ituWidgetSetVisible(PW100_CONT_RESULT, false);
            break;
    }

}


//static int m_nPW100_ButtonIndex=0;//current selected key index. 0:skip 1-10:key 11:del 12:D7 
void PW100_MenuTabPrev()
{
    int nTmpIndex = m_nPW100_ButtonIndex;

    if(PW100_b_VMode)
    {        
        if(nTmpIndex==1)
        {
            nTmpIndex = 10;
        }
        else if(nTmpIndex==10)
        {
            nTmpIndex = PW100_DEL_Key;
        }
        else if(nTmpIndex==PW100_DEL_Key)
        {
            nTmpIndex = 9;
        }
        else if(nTmpIndex>=2 && nTmpIndex<=9)
        {
            nTmpIndex--;
        }
        else
        {
            printf("[PW100_MenuTabPrev]Error. nTmpIndex=%d",nTmpIndex);
        }
    }
    else
    {
        if(nTmpIndex==1)
        {
            nTmpIndex = PW100_DEL_Key;
        }
        else if(nTmpIndex>=2 && nTmpIndex<=PW100_DEL_Key)
        {
            nTmpIndex--;
        }
        else
        {
            printf("[PW100_MenuTabPrev]Error. nTmpIndex=%d",nTmpIndex);
        }
    }

    if(g_nPW100_LayerType == PW100_MODE_SET)
    {
        if(nTmpIndex==PW100_PASS_ON_BTN)
        {
            nTmpIndex = PW100_PASS_OFF_BTN;
        }
        else if(nTmpIndex==PW100_PASS_OFF_BTN)
        {
            nTmpIndex = PW100_PASS_ON_BTN;
        }
    }
   
    PW100_Button_Select(nTmpIndex, false);//chpark.11.02
}


void PW100_MenuTabNext()
{
     int nTmpIndex = m_nPW100_ButtonIndex;

    if(PW100_b_VMode)
    {        
        if(nTmpIndex>=1 && nTmpIndex<=8)
        {
            nTmpIndex++;
        }
        else if(nTmpIndex==9)
        {
            nTmpIndex = PW100_DEL_Key;
        }
		else if(nTmpIndex==PW100_DEL_Key)
        {
            nTmpIndex = 10;
        }
        else if(nTmpIndex==10)
        {
            nTmpIndex = 1;
        }
    }
    else
    {
        if(nTmpIndex>=1 && nTmpIndex<=10)
        {
            nTmpIndex++;
        }
		else if(nTmpIndex==PW100_DEL_Key)
        {
            nTmpIndex = 1;
        }
    }

    if(g_nPW100_LayerType == PW100_MODE_SET)
    {
        if(nTmpIndex==PW100_PASS_ON_BTN)
        {
            nTmpIndex = PW100_PASS_OFF_BTN;
        }
        else if(nTmpIndex==PW100_PASS_OFF_BTN)
        {
            nTmpIndex = PW100_PASS_ON_BTN;
        }
    }

    PW100_Button_Select(nTmpIndex, false);//chpark.11.02
}

void PW100_MenuTabUp()
{
    int nTmpIndex = m_nPW100_ButtonIndex;
    if(PW100_b_VMode)
    {    
        switch(nTmpIndex)
        {     
            case 1:
                nTmpIndex=PW100_DEL_Key;break;
            case 2:
                nTmpIndex=10;break;
            case 3:
                nTmpIndex=9;break;
            case 4:
                nTmpIndex=1;break;
            case 5:
                nTmpIndex=2;break;
            case 6:
                nTmpIndex=3;break;
            case 7:
                nTmpIndex=4;break;
            case 8:
                nTmpIndex=5;break;
            case 9:
                nTmpIndex=6;break;
            case PW100_DEL_Key:
                nTmpIndex=7;break;
            case 10:
                nTmpIndex=8;break;
            
            default:
                break;
        }

        if(g_nPW100_LayerType == PW100_MODE_SET)
        {
            if(nTmpIndex==PW100_PASS_ON_BTN)
            {
                nTmpIndex = PW100_PASS_OFF_BTN;
            }
            else if(nTmpIndex==PW100_PASS_OFF_BTN)
            {
                nTmpIndex = PW100_PASS_ON_BTN;
            }
        }
    }
    else
    {
       //nothing to do.
      
    }

    PW100_Button_Select(nTmpIndex, false);
}

void PW100_MenuTabDown()
{
    int nTmpIndex = m_nPW100_ButtonIndex;
   
    if(PW100_b_VMode)
    {   
        switch(nTmpIndex)
        {     
            case 1:
                nTmpIndex=4;break;
            case 2:
                nTmpIndex=5;break;
            case 3:
                nTmpIndex=6;break;
            case 4:
                nTmpIndex=7;break;
            case 5:
                nTmpIndex=8;break;
            case 6:
                nTmpIndex=9;break;
            case 7:
                nTmpIndex=PW100_DEL_Key;break;
            case 8:
                nTmpIndex=10;break;
            case 9:
                nTmpIndex=3;break;
            case PW100_DEL_Key:
                nTmpIndex=1;break;
            case 10:
                nTmpIndex=2;break;
            default:
                break;
        }

        if(g_nPW100_LayerType == PW100_MODE_SET)
        {
            if(nTmpIndex==PW100_PASS_ON_BTN)
            {
                nTmpIndex = PW100_PASS_OFF_BTN;
            }
            else if(nTmpIndex==PW100_PASS_OFF_BTN)
            {
                nTmpIndex = PW100_PASS_ON_BTN;
            }
        }
       
    }
    else
    {
        //nothing to do.
        
    }
    
    PW100_Button_Select(nTmpIndex, false);
}

void PW100_MenuTabLeft()
{
    int nTmpIndex = m_nPW100_ButtonIndex;

    if(PW100_b_VMode)
    {
        switch(nTmpIndex)
        {
            case 1:
                nTmpIndex=3;break;
            case 2:
                nTmpIndex=1;break;
            case 3:
                nTmpIndex=2;break;
            case 4:
                nTmpIndex=6;break;
            case 5:
                nTmpIndex=4;break;
            case 6:
                nTmpIndex=5;break;
            case 7:
                nTmpIndex=9;break;
            case 8:
                nTmpIndex=7;break;
            case 9:
                nTmpIndex=8;break;
            case PW100_DEL_Key:
                nTmpIndex=10;break;
            case 10:
                nTmpIndex=PW100_DEL_Key;break;
            default:
                break;
        }

        PW100_Button_Select(nTmpIndex, false);
    }
    else
    {
        if(nTmpIndex==1)
        {
            nTmpIndex = PW100_DEL_Key;
        }
        else if(nTmpIndex>=2 && nTmpIndex<=10)
        {
            nTmpIndex--;
        }
        else if(nTmpIndex==PW100_DEL_Key)
        {
            nTmpIndex = 10;
        }


        if(g_nPW100_LayerType == PW100_MODE_SET)
        {
            if(nTmpIndex==PW100_PASS_ON_BTN)
            {
                nTmpIndex = PW100_PASS_OFF_BTN;
            }
            else if(nTmpIndex==PW100_PASS_OFF_BTN)
            {
                nTmpIndex = PW100_PASS_ON_BTN;
            }
        }

        PW100_Button_Select(nTmpIndex, false);
    }
}

void PW100_MenuTabRight()
{
    int nTmpIndex = m_nPW100_ButtonIndex;

    if(PW100_b_VMode)
    {
        switch(nTmpIndex)
        {
            case 1:
                nTmpIndex=2;break;
            case 2:
                nTmpIndex=3;break;
            case 3:
                nTmpIndex=1;break;
            case 4:
                nTmpIndex=5;break;
            case 5:
                nTmpIndex=6;break;
            case 6:
                nTmpIndex=4;break;
            case 7:
                nTmpIndex=8;break;
            case 8:
                nTmpIndex=9;break;
            case 9:
                nTmpIndex=7;break;
            case PW100_DEL_Key:
                nTmpIndex=10;break;
            case 10:
                nTmpIndex=PW100_DEL_Key;break;
            default:
                break;
        }
        
        PW100_Button_Select(nTmpIndex, false);
    }
    else
    {
        
        if(nTmpIndex>=1 && nTmpIndex<=9)
        {
            nTmpIndex++;
        }
        else if(nTmpIndex==10)
        {
            nTmpIndex = PW100_DEL_Key;
        }
        else if(nTmpIndex==PW100_DEL_Key)
        {
            nTmpIndex = 1;
        }

        if(g_nPW100_LayerType == PW100_MODE_SET)
        {
            if(nTmpIndex==PW100_PASS_ON_BTN)
            {
                nTmpIndex = PW100_PASS_OFF_BTN;
            }
            else if(nTmpIndex==PW100_PASS_OFF_BTN)
            {
                nTmpIndex = PW100_PASS_ON_BTN;
            }
        }

        PW100_Button_Select(nTmpIndex, false);
        
    }
}

void PW100_Save_CLI()
{
    //char c_hex[] = "9876";
	int nTmpPassword = atoi(PW100_PWD);
	printf("[PW100_Save_CLI]nTmpPassword= %d\n", nTmpPassword);

    if(nTmpPassword==0 || strlen(PW100_PWD)==0)
    {
        g_PW100_Password_5Byte = 0x00;
        g_PW100_Password_6Byte = 0x00;
    }
    else
    {
        uint8_t bytes[4];
        memcpy(bytes, &nTmpPassword, sizeof(int));
        printf("Final[5]:%x  Final[6]:%x\n", bytes[0],bytes[1]);

        g_PW100_Password_5Byte = bytes[0];
        g_PW100_Password_6Byte = bytes[1];
    }

	printf("[PW100_Save_CLI][5]:%d\n", g_PW100_Password_5Byte);
	printf("[PW100_Save_CLI][6]:%d\n", g_PW100_Password_6Byte);
}


void PW100_MenuTabEnter()
{
    if(m_bPW100_GoTo_LayerMode)return;
        
    if(g_nPW100_LayerType == PW100_MODE_SET)
    {
        int len_new,len_cf;
        len_new     = strlen(PW100_Txt_NewPassWord);
        len_cf      = strlen(PW100_Txt_ConfirmPassWord);
        if(len_new==4 && len_cf==4)
        {
            int nValid_Passcode = PW100_Passcode_Check();
            if(nValid_Passcode==0)
            {
                //save pass
                memset(PW100_Txt_CurPassWord,'\0', sizeof(PW100_Txt_CurPassWord));
                memset(PW100_PWD,'\0', sizeof(PW100_PWD));

                memcpy(PW100_Txt_CurPassWord, PW100_Txt_ConfirmPassWord ,MAX_PW100_PWD);
                memcpy(PW100_PWD, PW100_Txt_ConfirmPassWord ,MAX_PW100_PWD);

                //reset lock date
                //chpark.24.11.11 
                for(int i=0;i<3;i++)
                {
                    g_nM7_Lock_Data[i].Lock = M7_LSTATE_LOCK; 
                    g_nM7_Lock_Data[i].Show = M7_LSTATE_HIDE; 
                }

                //save CLI Data
	            PW100_Save_CLI();

                //===================================
                //save password & other setting

                g_bPW100_PW_OnOff = PW100_PASS_MODE_ON;

                XO_Save_Config(M7_UserIndex,true,0);
                //===================================

                //new pass == cf pass =>success
                PW100_Show_Result_UI(1);
            }
            else
            {
                //new pass != cf pass =>fail
                PW100_Show_Result_UI(0);  
                printf("[PW100_MenuTabEnter][set] not confirm pass error : %d\n",len_cf);
            }
        }
        else
        {
             printf("[PW100_MenuTabEnter] length error: len_new:%d len_cf:%d\n",len_new,len_cf);
        }
    }
    else if(g_nPW100_LayerType == PW100_MODE_INPUT)
    {
        int len_input;
        len_input     = strlen(PW100_Txt_InputPassWord);

        if(len_input==4)
        {
            int nValid_Passcode = PW100_Passcode_Check();
            if(nValid_Passcode==0)
            {
                printf("[PW100_MenuTabEnter]PASS : nValid_Passcode:%d g_nPW100_NextLayer=%d Unlock\n",nValid_Passcode,g_nPW100_NextLayer);
                //pass == exist pass  =>success
                if(g_nPW100_NextLayer == PW100_1st_LAYER)
                {
                    printf("[PW100_1st_LAYER] M7_LSTATE_UNLOCK\n");
                    g_nM7_Lock_Data[0].Lock = M7_LSTATE_UNLOCK;
                }
                else if(g_nPW100_NextLayer == PW100_2nd_LAYER)
                {
                    printf("[PW100_2nd_LAYER] M7_LSTATE_UNLOCK\n");
                    g_nM7_Lock_Data[1].Lock = M7_LSTATE_UNLOCK;
                }
                else if(g_nPW100_NextLayer == PW100_AutoPressure_LAYER)
                {
                    printf("[PW100_AutoPressure_LAYER] M7_LSTATE_UNLOCK\n");
                    g_nM7_Lock_Data[2].Lock = M7_LSTATE_UNLOCK;
                }
                printf("[PW100_MenuTabEnter]PW100_Show_Result_UI:1\n");
                PW100_Show_Result_UI(1);
            }
            else
            {
                //pass != exist pass =>fail
                PW100_Show_Result_UI(0);  
                printf("[PW100_MenuTabEnter][input] not input pass error:%d\n",len_input);
            }
        }
        else
        {
            //nothing to do
            printf("[PW100_MenuTabEnter][input] input pass error:%d\n",len_input);
        }
    }
    else
    {
        //error
    }
}

void PW100_MenuTabESC(bool bD5_Back)
{
    if(m_bPW100_GoTo_LayerMode)return;

        bool bExistInput = false;
        if(g_nPW100_LayerType == PW100_MODE_SET)
        {
            int len_new,len_cf,len_total;
            len_new = strlen(PW100_Txt_NewPassWord);
            len_cf  = strlen(PW100_Txt_ConfirmPassWord);

            len_total = len_new + len_cf;
            if(len_total>0)
            {
                bExistInput = true;
            }

            if(bExistInput)
            {
                if(bD5_Back)
                {
                    //if there is a input in password, delete all
                    PW100_Passcode_Clear(PW100_NEW_PASSMODE);	
                    PW100_Passcode_Clear(PW100_CF_PASSMODE);	
                    PW100_Display_PassCode();  

                    PW100_Button_Select(-1, false);//no focus
                    PW100_Button_Select(PW100_PASS_ON_BTN, true);//on
                         
                    printf("[PW100_MenuTabESC][D5 back][setup][on][remove focus] PW100_Button_Select=%d\n",m_nPW100_ButtonIndex); 
                    return;
                }
                else
                {
                    //if there is a input in password, delete it
                    PW100_Delete_Char();   //delete char
                    PW100_Display_PassCode();  
                }
            }
            else
            {
                if( (len_total == 0) && (g_bPW100_PW_OnOff == PW100_PASS_MODE_ON) )
                {
                    //On이 되어있더라도, 패스워드가 하나도 입력되지 않은 상태이며
                    //패스워드 ON,OFF를 여러번 작동한 상태에서 뒤로 간다면 모두 off를 시킨다.
                    //
                    if(PW100_input_toggle_cnt>0)
                    {
                        g_bPW100_PW_OnOff = PW100_PASS_MODE_OFF;
                        XO_Save_Config(M7_UserIndex,true,0); //always save
                    }

                    //if there is no input, remove focus //chpark.24.10.15
                    if(m_nPW100_ButtonIndex>=1 && m_nPW100_ButtonIndex<=11) 
                    {
                         PW100_Button_Select(-1, false);//no focus
                         PW100_Button_Select(PW100_PASS_ON_BTN, true);//on
                         
                         printf("[PW100_MenuTabESC][setup][on][SW4_Back][remove focus] PW100_Button_Select=%d\n",m_nPW100_ButtonIndex); 
                         return;
                    }
                }
                //setup mode->m7 
                printf("[PW100_MenuTabESC][setup] m_nPW100_ButtonIndex=%d g_nPW100_PreLayer=%d MenuPreset_Choice_Index=%d \n",m_nPW100_ButtonIndex, g_nPW100_PreLayer,MenuPreset_Choice_Index);

                if(g_nPW100_PreLayer == PW100_M7_LAYER)
                {
                    g_nM7_StartType = M7_MODE_KEY;
                }

                PW100_MenuGoToLayer(g_nPW100_PreLayer);
            }
        }
        else if(g_nPW100_LayerType==PW100_MODE_INPUT)
        {
            int len_input;
            len_input = strlen(PW100_Txt_InputPassWord);
            if(len_input>0)
            {
                bExistInput = true;
            }

            if(bExistInput)
            {
                //if there is a input in password, delete it
                PW100_Delete_Char();   //delete char
                PW100_Display_PassCode();
            }
            else
            {
                //setup mode->m7 
                printf("[PW100_MenuTabESC][input] g_nPW100_PreLayer=%d MenuPreset_Choice_Index=%d \n",g_nPW100_PreLayer,MenuPreset_Choice_Index);
                 
                if(g_nPW100_PreLayer == PW100_M7_LAYER)
                {
                    g_nM7_StartType = M7_MODE_KEY;
                }

                PW100_MenuGoToLayer(g_nPW100_PreLayer);
            }
        }

    
}


bool PW100_JogWheel(ITUWidget* widget, char* param)
{
    if(!m_bPW100_ReadyToJOG ||  m_bPW100_GoTo_LayerMode) return false;
    if(m_bPW100_ResultUI_Show) return false;

    PW100_ExitCNT = 0;
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
                //printf("PW100 JogWheel[R] %d\r\n",JogDir);
                PW100_MenuTabNext();
			}
			else if(JogDir == JOG_DIAL_ROT_LEFT)
			{
                //printf("PW100 JogWheel[L] %d\r\n",JogDir);
                PW100_MenuTabPrev();
			}

		break;
	}

    return true;
}

bool PW100_JogButton(ITUWidget* widget, char* param)
{
    if(!m_bPW100_ReadyToJOG || m_bPW100_GoTo_LayerMode) return false;
    if(m_bPW100_ResultUI_Show) return false;
    
    PW100_ExitCNT = 0;
    uint8_t JogButton = 0;
	JogButton = param[0];
	//printf("PW100_JogButton %d\r\n",JogButton);

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
                    PW100_MenuTabUp();
					break;

				case JOG_DIAL_KEY_DOWN:
                    PW100_MenuTabDown();
					break;
				
				case JOG_DIAL_KEY_LEFT:
                    PW100_MenuTabLeft();
                    break;

				case JOG_DIAL_KEY_RIGHT:
					PW100_MenuTabRight();
					break;

				case JOG_DIAL_KEY_ENTER_ON:
                    PW100_Button_Select(m_nPW100_ButtonIndex,true);
                    m_bPW100_ReadyToJOG_OffBtn = true;
					break;

				case JOG_DIAL_KEY_ENTER_OFF:				//c3Byte0bit
        		case JOG_DIAL_KEY_NONE:
				{
                    if(m_bPW100_ReadyToJOG_OffBtn)
                    {
                        PW100_Button_Select(m_nPW100_ButtonIndex,false); 
                        printf("[400]JOG_DIAL_KEY_ENTER_OFF ready: index:%d m_bPW100_ReadyToJOG_OffBtn:%d\n",m_nPW100_ButtonIndex,m_bPW100_ReadyToJOG_OffBtn);
                        //========================== 
                        char TMP_PARA[10];
                        memset(TMP_PARA,0,10);
                        int nTmpKeyValue;

                        if(m_nPW100_ButtonIndex>=0 && m_nPW100_ButtonIndex<13)
                        {
                            nTmpKeyValue = PASSWORD_KEY[m_nPW100_ButtonIndex];
                            sprintf(TMP_PARA,"%d",nTmpKeyValue);
                            PW100_BTN_MouseUp(NULL, TMP_PARA);
                        }
                        else if(m_nPW100_ButtonIndex == PW100_PASS_ON_BTN)
                        {
                            nTmpKeyValue = 50020;
                            sprintf(TMP_PARA,"%d",nTmpKeyValue);
                            PW100_BTN_MouseUp(NULL, TMP_PARA);
                        }
                        else if(m_nPW100_ButtonIndex == PW100_PASS_OFF_BTN)
                        {
                            nTmpKeyValue = 50021;
                            sprintf(TMP_PARA,"%d",nTmpKeyValue);
                            PW100_BTN_MouseUp(NULL, TMP_PARA);        
                        }
                        else
                        {
                            printf("[JOG_DIAL_KEY_ENTER_OFF]error\n"); 
                        }
                    }
                    else
                    {
                        printf("[JOG_DIAL_KEY_ENTER_OFF]not ready:%d\n",m_bPW100_ReadyToJOG_OffBtn); 
                        //m_bPW100_ReadyToJOG_OffBtn = true;
                    }
                    //==========================

        		}
					break;
			}
		break;
	}

    return true;
}

bool PW100_JogSwitch(ITUWidget* widget, char* param)
{
    if(!m_bPW100_ReadyToJOG || m_bPW100_GoTo_LayerMode) return false;
    PW100_ExitCNT = 0;
	uint8_t JogSwitch = 0;
	JogSwitch = param[0];
    bool bD5_Back = false;
	
	if(JogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
        PW100_MenuTabESC(bD5_Back);
	}
	else if(JogSwitch == JOG_DIAL_SW_HOME_OFF)
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}
    return true;
}
//======================================================

bool PW100_D7_MouseUp(ITUWidget* widget, char* param)
{   
    PW100_ExitCNT = 0;
    //nothing to do.

    return true;
}

bool PW100_D6_MouseUp(ITUWidget* widget, char* param)
{
    PW100_ExitCNT = 0;

    if(g_nPW100_LayerType == PW100_MODE_SET)
    {
        if( (PW100_ResultUI_Status != 1) && (g_bPW100_PW_OnOff == PW100_PASS_MODE_ON) )
        {
            //On이 되어있더라도, 패스워드가 하나도 입력되지 않은 상태이며
            //패스워드 ON,OFF를 여러번 작동한 상태에서 뒤로 간다면 모두 off를 시킨다.
            //
            if(PW100_input_toggle_cnt>0)
            {
                g_bPW100_PW_OnOff = PW100_PASS_MODE_OFF;
            }
        }
    }

	GotoMain(MAIN_MODE_NORMAL); 

    return true;
}

bool PW100_D5_MouseUp(ITUWidget* widget, char* param)
{
    PW100_ExitCNT = 0;
    bool bD5_Back = true;

    PW100_MenuTabESC(bD5_Back);

    return true;
}

//======================================================
bool PW100_Timer(ITUWidget* widget, char* param)
{
    #ifdef USED_ROTATION //chpark.10.19
    if (RotationMode!=0)
    {
      return 0;
    }
    #endif

    if((gMainTimerCnt_10m - PW100_SecTimer) >= 20)//100) //200msec
	{
		if(PW100_ExitCNT < 30*5)//30sec
			PW100_ExitCNT++;
		else if(PW100_ExitCNT == 30*5)//30sec
		{
			PW100_ExitCNT = 300;
            #ifdef RELEASE_VER
                printf("\r\n PW100 Menu 30Sec Exit > goto Main\r\n");
			    GotoMain(MAIN_MODE_NORMAL); 
                return true;
	        #endif
		}
        
		if(m_bPW100_GoTo_LayerMode)
		{
			m_bPW100_GoTo_LayerMode = false;
               
            if(g_nPW100_LayerType==PW100_MODE_SET)
            {
                g_nM7_StartType = M7_MODE_KEY;
                PW100_MenuGoToLayer(PW100_M7_LAYER);
            }
            else if(g_nPW100_LayerType==PW100_MODE_INPUT)
            {   
                printf("[PW100_Timer] m_bPW100_GoTo_LayerMode -> g_nPW100_NextLayer:%d\n",g_nPW100_NextLayer);
                if(g_nPW100_NextLayer == PW100_Lock_LAYER)
                {
                    g_nM7_StartType = M7_MODE_LOCK;
                    g_nPW100_NextLayer = PW100_M7_LAYER;
                }

                PW100_MenuGoToLayer(g_nPW100_NextLayer);
            }
		}

        if(m_bPW100_ResultUI_Show)
        {
            PW100_ResultUI_Cnt++;
            //printf("PW100_ResultUI_Cnt==%d\n",PW100_ResultUI_Cnt);
            if(PW100_ResultUI_Cnt>=1*5)//1sec
            {
                m_bPW100_ResultUI_Show = false;
                //printf("Error Msg Delete==>%d\n",PW100_ResultUI_Status);
                PW100_ResultUI_Cnt=0;

                if(PW100_ResultUI_Status==0)//fail
                {
                    PW100_Show_Result_UI(2);//x clear
                }
                else if(PW100_ResultUI_Status==1)//success
                {
                    PW100_Show_Result_UI(-1);
                    m_bPW100_GoTo_LayerMode = true;
                }
            }   
        } 

        
		PW100_SecTimer = gMainTimerCnt_10m;
	}

    return true;
}


void PW100_MenuGoToLayer(int nLayerType)
{
    if(m_bPW100_GoTo_LayerMode || m_bPW100_ResultUI_Show)
    {
        printf("[PW100_MenuGoToLayer] error return. m_bPW100_GoTo_LayerMode=%d g_bPW100_PW_OnOff=%d\n",m_bPW100_GoTo_LayerMode,g_bPW100_PW_OnOff);
        return;
    }

	if (nLayerType == PW100_M7_LAYER)
	{
        printf("[PW100_MenuGoToLayer] g_bPW100_PW_OnOff=%d\n",g_bPW100_PW_OnOff);
        ITULayer *tmp_Lapyer = ituSceneFindWidget(&theScene, "Menu7_Layer");
        ituLayerGoto(tmp_Lapyer);
	}
    else if (nLayerType == PW100_1st_LAYER)
	{
        ITULayer *tmp_Lapyer;
        tmp_Lapyer = ituSceneFindWidget(&theScene, "M7_S1");
		ituLayerGoto(tmp_Lapyer);
	}
	else if (nLayerType == PW100_2nd_LAYER)
	{
        ITULayer *tmp_Lapyer;
		tmp_Lapyer = ituSceneFindWidget(&theScene, "M7_S2");
		ituLayerGoto(tmp_Lapyer);
	}
	else if (nLayerType == PW100_Lock_LAYER)
	{
		ITULayer* tmp_Lapyer = ituSceneFindWidget(&theScene, "PW100_1st_LAYER");
		ituLayerGoto(tmp_Lapyer);
	}
	else if (nLayerType == PW100_PassSet_LAYER)
	{
        PW100_input_toggle_cnt = 0;//reset btn touch count
		g_nPW100_LayerType = PW100_MODE_SET;
        PW100_Set_Layer_Mode(g_nPW100_LayerType);
        
        PW100_Set_PassMode_On(g_bPW100_PW_OnOff);

        if(g_bPW100_PW_OnOff == PW100_PASS_MODE_OFF)
        {
            PW100_Button_Select(PW100_PASS_OFF_BTN, true);//focus off btn
        }
        else
        {
            PW100_Button_Select(PW100_PASS_ON_BTN, true);//focus on btn
        }
	}
	else if (nLayerType == PW100_AutoPressure_LAYER)
	{
		ITULayer* tmp_Lapyer = ituSceneFindWidget(&theScene, "M7_S11");
		ituLayerGoto(tmp_Lapyer);
	} 
    else if (nLayerType == PW100_BTOP_LAYER)
	{
		ITULayer* tmp_Lapyer = ituSceneFindWidget(&theScene, "MenuBTop_Layer");
		ituLayerGoto(tmp_Lapyer);
	}
    else if (nLayerType == PW100_BTOP_OPT_LAYER)//chpark.24.10.22
	{
		ITULayer* tmp_Lapyer = ituSceneFindWidget(&theScene, "MenuB300_Layer");
		ituLayerGoto(tmp_Lapyer);
	}
    else if (nLayerType == PW100_MAIN_LAYER)
    {
		GotoMain(MAIN_MODE_NORMAL); 
	}
}

//======================================================



