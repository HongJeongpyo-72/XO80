#include <assert.h>

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"


static bool m_bSU300_AUI1_Rcv_Error = false;//chpark.24.02.14

//chpark.12.19
#define SU300_DEL_AUTO_OFF  0
#define SU300_DEL_AUTO_START 1
#define SU300_DEL_AUTO_ON 2

static uint32_t SU300_FastTimer;
static uint8_t SU300_FastFlag = SU300_DEL_AUTO_OFF;
static uint8_t SU300_1SecCNT  = 0;
bool SU300_Delete_Char(int nIndex);
//


//chpark.11.20
//char *g_SU300_MSG_TEXT_ERROR_EXIST[5];//"The name you entered is already registered."
//char *g_SU300_MSG_TEXT_ERROR_NOSEAT[5];//"The number of registered users has reached its limit.\nNo more users can be registered.

//static ITUWidget* SU300_ERROR_TEXT;//chpark.11.20
//

static uint32_t SU300_CursorCNT = 0;
static bool m_bSU300_CursorShow = true;
static ITUIcon* SU300_Cursor[2];  //chpark.11.02 (0:blue 1:black)

static int m_nSU300_MaxKeyCnt[5][2];

#define MAX_KEY_CNT  44

#define SU300_SUCCESS			0
#define SU300_FAIL				1


#define SU300_KEY_SMALL  0  
#define SU300_KEY_CAP    1  

static bool     m_bSU300ReadyToJOG = false;

static int      m_nSU300_ButtonIndex;

static bool m_bSU300_GoTo_LayerMode = false;
static int  m_nSU300_GoTo_LayerModeCnt=0;
static int  m_nSU300_FailMsg_Cnt=0;
static int  m_nSU300_Mode=0;

static uint32_t SU300_SecTimer;
static uint32_t SU300_ExitCNT = 0;
static uint32_t SU300_SendCNT = 0;


static ITUIcon* SU300_Key_Sel[50];
static ITUButton* SU300_Key_Bt[50];
//static ITUIcon* UserName_Data[MAX_NAME_CNT];
static ITUWidget* SU300_NAME_RESULT_ICON[5]; //
static bool SU300_b_CAP = false;

static ITUIcon* SU300_UserName_Bt_Gray;

static char SU300_TxtUserName[50];


bool SU300_Button_Select(int nIndex,bool bDownImage);
void SU300_InitControl();

void SU300_InitKeyMaping();
void SU300_InitKeyMaping2();

void SU300_InitKeyMaping_ENG();
void SU300_InitKeyMaping_GER();
void SU300_InitKeyMaping_FR();
void SU300_InitKeyMaping_IT();
void SU300_InitKeyMaping_SP();

void SU300_SetLang(uint8_t nIndex);
void SU300_Display_UserName();
void SU300_SetUserTitle(uint8_t tUserIdx);


void SU300_Chk_Result();
void SU300_Check_SP_Char();

//=================================
void SU300_MenuTabPrev();
void SU300_MenuTabNext();
void SU300_MenuTabUp();
void SU300_MenuTabDown();
void SU300_MenuTabLeft();
void SU300_MenuTabRight();
void SU300_MenuTabESC();
void SU300_MenuTabD7();
//=================================

#define MB300_START_KEY_INDEX_ENG	1
#define MB300_END_KEY_INDEX_ENG	    45   //[1~44: key] [45th:D7 KeyValue:45]
#define MB300_START_KEY_INDEX_ETC	1
#define MB300_END_KEY_INDEX_ETC	    46   //[1~45: key] [46th:D7 KeyValue:45]

#define SU300_LANG_ENG 0
#define SU300_LANG_GER 1
#define SU300_LANG_FR 2
#define SU300_LANG_IT 3
#define SU300_LANG_SP 4

#define SU300_KEY_S   0 //small
#define SU300_KEY_C   1 //capital

#define SU300_SUCCESS	1
#define SU300_FAIL		0

#define SU300_MODE_START 0
#define SU300_MODE_RESULT_SUCCESS 1
#define SU300_MODE_RESULT_FAIL 2

#define SU300_RCV_INIT       0
#define SU300_RCV_USER_IDX   1
#define SU300_RCV_USER_CHK   2

static ITUWidget* SU300_TOP_ICON[2]; //
static ITUWidget* SU300_TOP_NAME; //

static ITUWidget* SU300_CONT_KEY_BACK[5]; //
static ITUWidget* SU300_CONT_KEY_TYPE[10][2]; //

static ITUWidget* SU300_CONT_ERROR_TXT; //
static ITUWidget* SU300_ERROR_TXT[5]; //

static ITUWidget* SU300_CONT_ERROR_1_TXT; //chpark.24.02.07
static ITUWidget* SU300_ERROR_1_TXT[5];   //chpark.24.02.07

static ITUWidget* SU300_CONT_RESULT; //
static ITUWidget* SU300_RESULT_ICON[2]; //

static ITUWidget* SU300_TEXT_NAME; //

static ITUWidget* SU300_ICON_SEL_A[5];
static ITUWidget* SU300_ICON_SEL_B[5];
static ITUWidget* SU300_ICON_SEL_C[5];

static ITUWidget* SU300_ICON_DOWN_A[5];
static ITUWidget* SU300_ICON_DOWN_B[5];
static ITUWidget* SU300_ICON_DOWN_C[5];

static ITUWidget* SU300_ICON_D7_SEL;
static ITUWidget* SU300_ICON_D7_DOWN;

static int ENG_KEY[2][55];//chpark.11.02
static int GER_KEY[2][55];
static int FR_KEY[2][55];
static int IT_KEY[2][55];
static int SP_KEY[2][55];

static int SU300_CAP_KEY[5][2];
static int SU300_DEL_KEY[5][2];
static int SU300_D7_KEY[5][2];

bool SU300_KeyDn(ITUWidget* widget, char* param);
bool SU300_KeyUp(ITUWidget* widget, char* param);
bool SU300_Send_TX(int nType);

uint8_t  SU300_UM5_2Byte_07bit;		//chpark.08.22 valid user number
uint8_t  SU300_UM5_1Byte_45bit;		//chpark.08.22 1:Success 2:Fail

static uint8_t SU300_UM5_Rcv_Type=SU300_RCV_INIT;
static bool    m_bSU300_ReadyTo_Input = false;

static bool SU300_b_VMode = false;//chpark.01.15

bool SU300_Enter(ITUWidget* widget, char* param)
{
    if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
	{
        SU300_b_VMode = false;
    }
    else
    {   
        SU300_b_VMode = true;
    }

    int i=0,j;
    m_nSU300_ButtonIndex = 1;//chpark.11.02
    m_bSU300_GoTo_LayerMode = false;
    SU300_SecTimer  = gMainTimerCnt_10m;
    m_nSU300_Mode   = SU300_MODE_START;
    SU300_UM5_Rcv_Type=SU300_RCV_INIT;
    m_bSU300_ReadyTo_Input = false;

    //printf("SU300_Enter==\n");
    memset(SU300_TxtUserName, '\0', sizeof(SU300_TxtUserName));
    if(g_nSU100_Mode==SU100_Add_Mode)
    {
       printf("SU300_Enter==SU100_Add_Mode: g_nSU100_AddUserIdx=%d\n",g_nSU100_AddUserIdx);
       //chpark.24.02.16
       //memcpy(SU300_TxtUserName,&User_RegInfo[g_nSU100_AddUserIdx].UserName[0],MAX_SU100_NAME_CNT);  //chpark.11.27
       User_RegInfo[g_nSU100_AddUserIdx].bAddProcessing = true;
    }
    else if(g_nSU100_Mode==SU100_Edit_Mode)
    {
        printf("SU300_Enter==SU100_Edit_Mode: g_nSU100_EditUserIdx=%d\n",g_nSU100_EditUserIdx);
        //chpark.24.02.16
        //memcpy(SU300_TxtUserName,&User_RegInfo[g_nSU100_EditUserIdx].UserName[0],MAX_SU100_NAME_CNT);  
    }

    SU300_b_CAP = false;//small

    SU300_InitControl();

    SU300_SetLang(Lang_Flag[M7_UserIndex]);

    //chpark.11.02
    m_nSU300_MaxKeyCnt[SU300_LANG_ENG][SU300_KEY_SMALL]= 44;
    m_nSU300_MaxKeyCnt[SU300_LANG_ENG][SU300_KEY_CAP]  = 44;

    m_nSU300_MaxKeyCnt[SU300_LANG_GER][SU300_KEY_SMALL]= 49;
    m_nSU300_MaxKeyCnt[SU300_LANG_GER][SU300_KEY_CAP]  = 49;

    m_nSU300_MaxKeyCnt[SU300_LANG_FR][SU300_KEY_SMALL]= 47;
    m_nSU300_MaxKeyCnt[SU300_LANG_FR][SU300_KEY_CAP]  = 46;

    m_nSU300_MaxKeyCnt[SU300_LANG_IT][SU300_KEY_SMALL]= 48;
    m_nSU300_MaxKeyCnt[SU300_LANG_IT][SU300_KEY_CAP]  = 47;

    m_nSU300_MaxKeyCnt[SU300_LANG_SP][SU300_KEY_SMALL]= 47;
    m_nSU300_MaxKeyCnt[SU300_LANG_SP][SU300_KEY_CAP]  = 47;

    SU300_InitKeyMaping(); //key widget setting
    SU300_InitKeyMaping2();//key byte arrary setting

    SU300_SetUserTitle(g_nSU100_CurUserIdx);

    //init name.
    SU300_Display_UserName();
    //init key point
    SU300_Button_Select(1,false);

    m_bSU300ReadyToJOG = true;

    if(g_nSU100_Mode==SU100_Add_Mode)
    {
        g_UM5_ChkTime = 0;
        SU300_UM5_Rcv_Type=SU300_RCV_USER_IDX;
        SU300_Send_TX(0);
    }

    //chpark.12.21
    SU300_1SecCNT = 100;
	SU300_FastFlag = SU300_DEL_AUTO_OFF;

    m_bSU300_AUI1_Rcv_Error = false;        //chpark.24.02.14
    UserPW_Time_AUI1 = gMainTimerCnt_10m;	//chpark.24.02.14

    if(!gUI_Server_Mode)
    {
        m_bSU300_ReadyTo_Input = true;//chpark.24.03.18
    }

    if(g_nSU100_Mode==SU100_Edit_Mode)
    {
         m_bSU300_ReadyTo_Input = true;//chpark.24.03.22
    }
    
    return true;
}

void SU300_InitControl()
{
    int tIndex=0;
//Text setting    
    //g_SU300_MSG_TEXT_ERROR_EXIST[SU300_LANG_ENG] = "The name you entered is already registered.";
    //g_SU300_MSG_TEXT_ERROR_EXIST[SU300_LANG_GER] = "The name you entered is already registered.";
    //g_SU300_MSG_TEXT_ERROR_EXIST[SU300_LANG_FR]  = "The name you entered is already registered.";
    //g_SU300_MSG_TEXT_ERROR_EXIST[SU300_LANG_IT]  = "The name you entered is already registered.";
    //g_SU300_MSG_TEXT_ERROR_EXIST[SU300_LANG_SP]  = "The name you entered is already registered.";

    //g_SU300_MSG_TEXT_ERROR_NOSEAT[SU300_LANG_ENG] = "The number of registered users has reached its limit.\nNo more users can be registered.";
    //g_SU300_MSG_TEXT_ERROR_NOSEAT[SU300_LANG_GER] = "The number of registered users has reached its limit.\nNo more users can be registered.";
    //g_SU300_MSG_TEXT_ERROR_NOSEAT[SU300_LANG_FR]  = "The number of registered users has reached its limit.\nNo more users can be registered.";
    //g_SU300_MSG_TEXT_ERROR_NOSEAT[SU300_LANG_IT]  = "The number of registered users has reached its limit.\nNo more users can be registered.";
    //g_SU300_MSG_TEXT_ERROR_NOSEAT[SU300_LANG_SP]  = "The number of registered users has reached its limit.\nNo more users can be registered.";
//printf("SU300===========11111\n");
    SetWidget(SU300_TOP_ICON[SU100_ICON_AD],"SU300_T_Icon_Admin");
	SetWidget(SU300_TOP_ICON[SU100_ICON_ST],"SU300_T_Icon_Std");
	SetWidget(SU300_TOP_NAME,"SU300_T_Name");
//printf("SU300===========22222\n");
    SetWidget(SU300_CONT_KEY_BACK[SU300_LANG_ENG],"SU300_ENG");
    SetWidget(SU300_CONT_KEY_BACK[SU300_LANG_GER],"SU300_DEU");
    SetWidget(SU300_CONT_KEY_BACK[SU300_LANG_FR], "SU300_FRA");
    SetWidget(SU300_CONT_KEY_BACK[SU300_LANG_IT], "SU300_ITA");
    SetWidget(SU300_CONT_KEY_BACK[SU300_LANG_SP], "SU300_ESP");
//printf("SU300===========33333\n");
    SetWidget(SU300_CONT_KEY_TYPE[SU300_LANG_ENG][SU300_KEY_S],"SU300_Ns_ENG");
    SetWidget(SU300_CONT_KEY_TYPE[SU300_LANG_ENG][SU300_KEY_C],"SU300_Nc_ENG");
    SetWidget(SU300_CONT_KEY_TYPE[SU300_LANG_GER][SU300_KEY_S],"SU300_Ns_DEU");
    SetWidget(SU300_CONT_KEY_TYPE[SU300_LANG_GER][SU300_KEY_C],"SU300_Nc_DEU");
    SetWidget(SU300_CONT_KEY_TYPE[SU300_LANG_FR][SU300_KEY_S],"SU300_Ns_FRA");
    SetWidget(SU300_CONT_KEY_TYPE[SU300_LANG_FR][SU300_KEY_C],"SU300_Nc_FRA");
    SetWidget(SU300_CONT_KEY_TYPE[SU300_LANG_IT][SU300_KEY_S],"SU300_Ns_ITA");
    SetWidget(SU300_CONT_KEY_TYPE[SU300_LANG_IT][SU300_KEY_C],"SU300_Nc_ITA");
    SetWidget(SU300_CONT_KEY_TYPE[SU300_LANG_SP][SU300_KEY_S],"SU300_Ns_ESP");
    SetWidget(SU300_CONT_KEY_TYPE[SU300_LANG_SP][SU300_KEY_C],"SU300_Nc_ESP");
//printf("SU300===========4444\n");
    SetWidget(SU300_CONT_ERROR_TXT, "SU300_CONT_ERROR_TXT");
    SetWidget(SU300_ERROR_TXT[SU300_LANG_ENG], "SU300_ERROR_ENG");
    SetWidget(SU300_ERROR_TXT[SU300_LANG_GER], "SU300_ERROR_GER");
    SetWidget(SU300_ERROR_TXT[SU300_LANG_FR],  "SU300_ERROR_FR");
    SetWidget(SU300_ERROR_TXT[SU300_LANG_IT],  "SU300_ERROR_IT");
    SetWidget(SU300_ERROR_TXT[SU300_LANG_SP],  "SU300_ERROR_SP");
    //chpark.24.02.07
    SetWidget(SU300_CONT_ERROR_1_TXT, "SU300_CONT_ERROR_1_TXT"); 
    SetWidget(SU300_ERROR_1_TXT[SU300_LANG_ENG], "SU300_ERROR_1_ENG");
    SetWidget(SU300_ERROR_1_TXT[SU300_LANG_GER], "SU300_ERROR_1_GER");
    SetWidget(SU300_ERROR_1_TXT[SU300_LANG_FR],  "SU300_ERROR_1_FR");
    SetWidget(SU300_ERROR_1_TXT[SU300_LANG_IT],  "SU300_ERROR_1_IT");
    SetWidget(SU300_ERROR_1_TXT[SU300_LANG_SP],  "SU300_ERROR_1_SP");
//printf("SU300===========5555\n");
    SetWidget(SU300_CONT_RESULT, "SU300_CONT_RESULT");
    SetWidget(SU300_RESULT_ICON[SU300_SUCCESS],"SU300_OK");
    SetWidget(SU300_RESULT_ICON[SU300_FAIL],   "SU300_NO");

    SetWidget(SU300_TEXT_NAME,"SU300_Text_User_Name");
//=======================
    SetWidget(SU300_ICON_SEL_A[SU300_LANG_ENG], "SU300_ENG_SEL_A");//2
    SetWidget(SU300_ICON_SEL_A[SU300_LANG_GER], "SU300_DEU_SEL_A");//2
    SetWidget(SU300_ICON_SEL_A[SU300_LANG_FR],  "SU300_FRA_SEL_A");//3
    SetWidget(SU300_ICON_SEL_A[SU300_LANG_IT],  "SU300_ITA_SEL_A");//3
    SetWidget(SU300_ICON_SEL_A[SU300_LANG_SP],  "SU300_ESP_SEL_A");//3

    SetWidget(SU300_ICON_SEL_B[SU300_LANG_ENG], "SU300_ENG_SEL_B");//not use
    SetWidget(SU300_ICON_SEL_B[SU300_LANG_GER], "SU300_DEU_SEL_B");//not use
    SetWidget(SU300_ICON_SEL_B[SU300_LANG_FR],  "SU300_FRA_SEL_B");
    SetWidget(SU300_ICON_SEL_B[SU300_LANG_IT],  "SU300_ITA_SEL_B");
    SetWidget(SU300_ICON_SEL_B[SU300_LANG_SP],  "SU300_ESP_SEL_B");

    SetWidget(SU300_ICON_SEL_C[SU300_LANG_ENG], "SU300_ENG_SEL_C");
    SetWidget(SU300_ICON_SEL_C[SU300_LANG_GER], "SU300_DEU_SEL_C");
    SetWidget(SU300_ICON_SEL_C[SU300_LANG_FR],  "SU300_FRA_SEL_C");
    SetWidget(SU300_ICON_SEL_C[SU300_LANG_IT],  "SU300_ITA_SEL_C");
    SetWidget(SU300_ICON_SEL_C[SU300_LANG_SP],  "SU300_ESP_SEL_C");

    SetWidget(SU300_ICON_DOWN_A[SU300_LANG_ENG], "SU300_ENG_DOWN_A");
    SetWidget(SU300_ICON_DOWN_A[SU300_LANG_GER], "SU300_DEU_DOWN_A");
    SetWidget(SU300_ICON_DOWN_A[SU300_LANG_FR],  "SU300_FRA_DOWN_A");
    SetWidget(SU300_ICON_DOWN_A[SU300_LANG_IT],  "SU300_ITA_DOWN_A");
    SetWidget(SU300_ICON_DOWN_A[SU300_LANG_SP],  "SU300_ESP_DOWN_A");

    SetWidget(SU300_ICON_DOWN_B[SU300_LANG_ENG], "SU300_ENG_DOWN_B");//not use
    SetWidget(SU300_ICON_DOWN_B[SU300_LANG_GER], "SU300_DEU_DOWN_B");//not use
    SetWidget(SU300_ICON_DOWN_B[SU300_LANG_FR],  "SU300_FRA_DOWN_B");
    SetWidget(SU300_ICON_DOWN_B[SU300_LANG_IT],  "SU300_ITA_DOWN_B");
    SetWidget(SU300_ICON_DOWN_B[SU300_LANG_SP],  "SU300_ESP_DOWN_B");

    SetWidget(SU300_ICON_DOWN_C[SU300_LANG_ENG], "SU300_ENG_DOWN_C");
    SetWidget(SU300_ICON_DOWN_C[SU300_LANG_GER], "SU300_DEU_DOWN_C");
    SetWidget(SU300_ICON_DOWN_C[SU300_LANG_FR],  "SU300_FRA_DOWN_C");
    SetWidget(SU300_ICON_DOWN_C[SU300_LANG_IT],  "SU300_ITA_DOWN_C");
    SetWidget(SU300_ICON_DOWN_C[SU300_LANG_SP],  "SU300_ESP_DOWN_C");
//================================
//printf("SU300===========66666\n"); 
    SetWidget(SU300_ICON_D7_SEL, "SU300_ICON_D7_SEL");
    SetWidget(SU300_ICON_D7_DOWN,"SU300_ICON_D7_DOWN");
//printf("SU300===========77777\n");
    //result
    ituWidgetSetVisible(SU300_CONT_ERROR_TXT,  false);
    ituWidgetSetVisible(SU300_CONT_ERROR_1_TXT,  false);//chpark.24.02.07
    ituWidgetSetVisible(SU300_CONT_RESULT,     false);
    //
//printf("SU300===========88888\n");
    //chpark.11.02
    SetWidget(SU300_Cursor[0],   "SU300_Cursor_blue");
    ituWidgetSetVisible(SU300_Cursor[0],  true);

    //chpark.11.20
    //SetWidget(SU300_ERROR_TEXT,   "SU300_ERROR_TEXT");
}

void SU300_SetLang(uint8_t nIndex)
{
    for(int i=0;i<5;i++)
    {
        ituWidgetSetVisible(SU300_CONT_KEY_BACK[i],false);
        ituWidgetSetVisible(SU300_CONT_KEY_TYPE[i][0],false);
        ituWidgetSetVisible(SU300_CONT_KEY_TYPE[i][1],false);

        ituWidgetSetVisible(SU300_ICON_SEL_A[i],false);
        ituWidgetSetVisible(SU300_ICON_SEL_B[i],false);
        ituWidgetSetVisible(SU300_ICON_SEL_C[i],false);
    
        ituWidgetSetVisible(SU300_ICON_DOWN_A[i],false);
        ituWidgetSetVisible(SU300_ICON_DOWN_B[i],false);
        ituWidgetSetVisible(SU300_ICON_DOWN_C[i],false);
    }

    ituWidgetSetVisible(SU300_CONT_KEY_BACK[nIndex],true);//keyboard language
    ituWidgetSetVisible(SU300_CONT_KEY_TYPE[nIndex][SU300_b_CAP],true);// small, capital
}

void SU300_InitKeyMaping()
{
    int tIndex=0;
    int i;
    char tmp_WidgetName[100];
    
    int nMaxKeyCntS;
    int nMaxKeyCntC;

    nMaxKeyCntS = m_nSU300_MaxKeyCnt[Lang_Flag[M7_UserIndex]][SU300_KEY_SMALL];
    nMaxKeyCntC = m_nSU300_MaxKeyCnt[Lang_Flag[M7_UserIndex]][SU300_KEY_CAP];

    printf("nMaxKeyCntS=%d  nMaxKeyCntC%s \n",nMaxKeyCntS,nMaxKeyCntC);

    if(Lang_Flag[M7_UserIndex]==SU300_LANG_ENG)
    {
        if(!SU300_b_CAP)
        {
            for ( i=0;i<nMaxKeyCntS;i++)
            {
                memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
                sprintf(tmp_WidgetName, "SU300_ENGs_%02d",i+1);
                SetWidget(SU300_Key_Bt[i],tmp_WidgetName);
                //printf("tmp_WidgetName=%s\n",tmp_WidgetName);
            }
        }
        else
        {
            for ( i=0;i<nMaxKeyCntC;i++)
            {
                memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
                sprintf(tmp_WidgetName, "SU300_ENGc_%02d",i+1);
                SetWidget(SU300_Key_Bt[i],tmp_WidgetName);
                //printf("tmp_WidgetName=%s\n",tmp_WidgetName);
            }
        }
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_GER)
    {
        if(!SU300_b_CAP)
        {
            for ( i=0;i<nMaxKeyCntS;i++)
            {
                memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
                sprintf(tmp_WidgetName, "SU300_DEUs_%02d",i+1);
                SetWidget(SU300_Key_Bt[i],tmp_WidgetName);
                //printf("tmp_WidgetName=%s\n",tmp_WidgetName);
            }
        }
        else
        {
            for ( i=0;i<nMaxKeyCntC;i++)
            {
                memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
                sprintf(tmp_WidgetName, "SU300_DEUc_%02d",i+1);
                SetWidget(SU300_Key_Bt[i],tmp_WidgetName);
                //printf("tmp_WidgetName=%s\n",tmp_WidgetName);
            }
        }
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_FR)
    {
        if(!SU300_b_CAP)
        {
            for ( i=0;i<nMaxKeyCntS;i++)
            {
                memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
                sprintf(tmp_WidgetName, "SU300_FRAs_%02d",i+1);
                SetWidget(SU300_Key_Bt[i],tmp_WidgetName);
                //printf("tmp_WidgetName=%s\n",tmp_WidgetName);
            }
        }
        else
        {
            for ( i=0;i<nMaxKeyCntC;i++)
            {
                memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
                sprintf(tmp_WidgetName, "SU300_FRAc_%02d",i+1);
                SetWidget(SU300_Key_Bt[i],tmp_WidgetName);
                //printf("tmp_WidgetName=%s\n",tmp_WidgetName);
            }
        }
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_IT)
    {
        if(!SU300_b_CAP)
        {
            for ( i=0;i<nMaxKeyCntS;i++)
            {
                memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
                sprintf(tmp_WidgetName, "SU300_ITAs_%02d",i+1);
                SetWidget(SU300_Key_Bt[i],tmp_WidgetName);
                //printf("tmp_WidgetName=%s\n",tmp_WidgetName);
            }
        }
        else
        {
            for ( i=0;i<nMaxKeyCntC;i++)
            {
                memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
                sprintf(tmp_WidgetName, "SU300_ITAc_%02d",i+1);
                SetWidget(SU300_Key_Bt[i],tmp_WidgetName);
                //printf("tmp_WidgetName=%s\n",tmp_WidgetName);
            }

        }
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_SP)
    {
        if(!SU300_b_CAP)
        {
            for ( i=0;i<nMaxKeyCntS;i++)
            {
                memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
                sprintf(tmp_WidgetName, "SU300_ESPs_%02d",i+1);
                SetWidget(SU300_Key_Bt[i],tmp_WidgetName);
                //printf("tmp_WidgetName=%s\n",tmp_WidgetName);
            }
        }
        else
        {
            for ( i=0;i<nMaxKeyCntC;i++)
            {
                memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
                sprintf(tmp_WidgetName, "SU300_ESPc_%02d",i+1);
                SetWidget(SU300_Key_Bt[i],tmp_WidgetName);
                //printf("tmp_WidgetName=%s\n",tmp_WidgetName);
            }
        }
    }
    else
    {
        if(!SU300_b_CAP)
        {
            for (i=0;i<MAX_KEY_CNT;i++)
            {
                memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
                sprintf(tmp_WidgetName, "SU300_ENGs_%02d",i+1);
                SetWidget(SU300_Key_Bt[i],tmp_WidgetName);
            }
        }
        else
        {
            for (i=0;i<MAX_KEY_CNT;i++)
            {
                memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
                sprintf(tmp_WidgetName, "SU300_ENGc_%02d",i+1);
                SetWidget(SU300_Key_Bt[i],tmp_WidgetName);
            }
        }
    }
}

void SU300_InitKeyMaping2()
{
    if(Lang_Flag[M7_UserIndex]==SU300_LANG_ENG)
    {
        SU300_InitKeyMaping_ENG();
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_GER)
    {
        SU300_InitKeyMaping_GER();
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_FR)
    {
        SU300_InitKeyMaping_FR();
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_IT)
    {
        SU300_InitKeyMaping_IT();
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_SP)
    {
        SU300_InitKeyMaping_SP();
    }
    else
    {
        SU300_InitKeyMaping_ENG();
    }
}

void SU300_InitKeyMaping_ENG()
{
    ENG_KEY[SU300_KEY_S][0]=0;//none
    ENG_KEY[SU300_KEY_S][1]=4901;
    ENG_KEY[SU300_KEY_S][2]=5002;
    ENG_KEY[SU300_KEY_S][3]=5103;
    ENG_KEY[SU300_KEY_S][4]=5204;
    ENG_KEY[SU300_KEY_S][5]=5305;
    ENG_KEY[SU300_KEY_S][6]=5406;
    ENG_KEY[SU300_KEY_S][7]=5507;
    ENG_KEY[SU300_KEY_S][8]=5608;
    ENG_KEY[SU300_KEY_S][9]=5709;
    ENG_KEY[SU300_KEY_S][10]=4810;
    ENG_KEY[SU300_KEY_S][11]=11311;
    ENG_KEY[SU300_KEY_S][12]=11912;
    ENG_KEY[SU300_KEY_S][13]=10113;
    ENG_KEY[SU300_KEY_S][14]=11414;
    ENG_KEY[SU300_KEY_S][15]=11615;
    ENG_KEY[SU300_KEY_S][16]=12116;
    ENG_KEY[SU300_KEY_S][17]=11717;
    ENG_KEY[SU300_KEY_S][18]=10518;
    ENG_KEY[SU300_KEY_S][19]=11119;
    ENG_KEY[SU300_KEY_S][20]=11220;
    ENG_KEY[SU300_KEY_S][21]=9721;
    ENG_KEY[SU300_KEY_S][22]=11522;
    ENG_KEY[SU300_KEY_S][23]=10023;
    ENG_KEY[SU300_KEY_S][24]=10224;
    ENG_KEY[SU300_KEY_S][25]=10325;
    ENG_KEY[SU300_KEY_S][26]=10426;
    ENG_KEY[SU300_KEY_S][27]=10627;
    ENG_KEY[SU300_KEY_S][28]=10728;
    ENG_KEY[SU300_KEY_S][29]=10829;
    ENG_KEY[SU300_KEY_S][30]=12230;
    ENG_KEY[SU300_KEY_S][31]=12031;
    ENG_KEY[SU300_KEY_S][32]=9932;
    ENG_KEY[SU300_KEY_S][33]=11833;
    ENG_KEY[SU300_KEY_S][34]=9834;
    ENG_KEY[SU300_KEY_S][35]=11035;
    ENG_KEY[SU300_KEY_S][36]=10936;
    ENG_KEY[SU300_KEY_S][37]=100037;
    ENG_KEY[SU300_KEY_S][38]=4438;
    ENG_KEY[SU300_KEY_S][39]=4639;
    ENG_KEY[SU300_KEY_S][40]=3240;
    ENG_KEY[SU300_KEY_S][41]=9541;
    ENG_KEY[SU300_KEY_S][42]=4542;
    ENG_KEY[SU300_KEY_S][43]=6143;
    ENG_KEY[SU300_KEY_S][44]=12744;
    ENG_KEY[SU300_KEY_S][45]=10045;//D7

    ENG_KEY[SU300_KEY_C][0]=0;//none
    ENG_KEY[SU300_KEY_C][1]=4901;
    ENG_KEY[SU300_KEY_C][2]=5002;
    ENG_KEY[SU300_KEY_C][3]=5103;
    ENG_KEY[SU300_KEY_C][4]=5204;
    ENG_KEY[SU300_KEY_C][5]=5305;
    ENG_KEY[SU300_KEY_C][6]=5406;
    ENG_KEY[SU300_KEY_C][7]=5507;
    ENG_KEY[SU300_KEY_C][8]=5608;
    ENG_KEY[SU300_KEY_C][9]=5709;
    ENG_KEY[SU300_KEY_C][10]=4810;
    ENG_KEY[SU300_KEY_C][11]=8111;
    ENG_KEY[SU300_KEY_C][12]=8712;
    ENG_KEY[SU300_KEY_C][13]=6913;
    ENG_KEY[SU300_KEY_C][14]=8214;
    ENG_KEY[SU300_KEY_C][15]=84515;
    ENG_KEY[SU300_KEY_C][16]=8916;
    ENG_KEY[SU300_KEY_C][17]=8517;
    ENG_KEY[SU300_KEY_C][18]=7318;
    ENG_KEY[SU300_KEY_C][19]=7919;
    ENG_KEY[SU300_KEY_C][20]=8020;
    ENG_KEY[SU300_KEY_C][21]=6521;
    ENG_KEY[SU300_KEY_C][22]=8322;
    ENG_KEY[SU300_KEY_C][23]=6823;
    ENG_KEY[SU300_KEY_C][24]=7024;
    ENG_KEY[SU300_KEY_C][25]=7125;
    ENG_KEY[SU300_KEY_C][26]=7226;
    ENG_KEY[SU300_KEY_C][27]=7427;
    ENG_KEY[SU300_KEY_C][28]=7528;
    ENG_KEY[SU300_KEY_C][29]=7629;
    ENG_KEY[SU300_KEY_C][30]=9030;
    ENG_KEY[SU300_KEY_C][31]=8831;
    ENG_KEY[SU300_KEY_C][32]=6732;
    ENG_KEY[SU300_KEY_C][33]=8633;
    ENG_KEY[SU300_KEY_C][34]=6634;
    ENG_KEY[SU300_KEY_C][35]=7835;
    ENG_KEY[SU300_KEY_C][36]=7736;
    ENG_KEY[SU300_KEY_C][37]=200037;
    ENG_KEY[SU300_KEY_C][38]=4438;
    ENG_KEY[SU300_KEY_C][39]=4639;
    ENG_KEY[SU300_KEY_C][40]=3240;
    ENG_KEY[SU300_KEY_C][41]=9541;
    ENG_KEY[SU300_KEY_C][42]=4542;
    ENG_KEY[SU300_KEY_C][43]=6143;
    ENG_KEY[SU300_KEY_C][44]=12744;
    ENG_KEY[SU300_KEY_C][45]=10045;//D7

    SU300_CAP_KEY[SU300_LANG_ENG][SU300_KEY_S] = 37;
    SU300_CAP_KEY[SU300_LANG_ENG][SU300_KEY_C] = 37;

    SU300_DEL_KEY[SU300_LANG_ENG][SU300_KEY_S] = 44;
    SU300_DEL_KEY[SU300_LANG_ENG][SU300_KEY_C] = 44;

    SU300_D7_KEY[SU300_LANG_ENG][SU300_KEY_S] = 45;
    SU300_D7_KEY[SU300_LANG_ENG][SU300_KEY_C] = 45;
}


void SU300_InitKeyMaping_GER()
{
    GER_KEY[SU300_KEY_S][0]=0;//none
    GER_KEY[SU300_KEY_S][1]=4901;
    GER_KEY[SU300_KEY_S][2]=5002;
    GER_KEY[SU300_KEY_S][3]=5103;
    GER_KEY[SU300_KEY_S][4]=5204;
    GER_KEY[SU300_KEY_S][5]=5305;
    GER_KEY[SU300_KEY_S][6]=5406;
    GER_KEY[SU300_KEY_S][7]=5507;
    GER_KEY[SU300_KEY_S][8]=5608;
    GER_KEY[SU300_KEY_S][9]=5709;
    GER_KEY[SU300_KEY_S][10]=4810;
    GER_KEY[SU300_KEY_S][11]=22311;
    GER_KEY[SU300_KEY_S][12]=11312;
    GER_KEY[SU300_KEY_S][13]=11913;
    GER_KEY[SU300_KEY_S][14]=10114;
    GER_KEY[SU300_KEY_S][15]=11415;
    GER_KEY[SU300_KEY_S][16]=11616;
    GER_KEY[SU300_KEY_S][17]=12217;
    GER_KEY[SU300_KEY_S][18]=11718;
    GER_KEY[SU300_KEY_S][19]=10519;
    GER_KEY[SU300_KEY_S][20]=11120;
    GER_KEY[SU300_KEY_S][21]=11221;
    GER_KEY[SU300_KEY_S][22]=25222;
    GER_KEY[SU300_KEY_S][23]=9723;
    GER_KEY[SU300_KEY_S][24]=11524;
    GER_KEY[SU300_KEY_S][25]=10025;
    GER_KEY[SU300_KEY_S][26]=10226;
    GER_KEY[SU300_KEY_S][27]=10327;
    GER_KEY[SU300_KEY_S][28]=10428;
    GER_KEY[SU300_KEY_S][29]=10629;
    GER_KEY[SU300_KEY_S][30]=10730;
    GER_KEY[SU300_KEY_S][31]=10831;
    GER_KEY[SU300_KEY_S][32]=24632;
    GER_KEY[SU300_KEY_S][33]=22833;
    GER_KEY[SU300_KEY_S][34]=12134;
    GER_KEY[SU300_KEY_S][35]=12035;
    GER_KEY[SU300_KEY_S][36]=9936;
    GER_KEY[SU300_KEY_S][37]=11837;
    GER_KEY[SU300_KEY_S][38]=9838;
    GER_KEY[SU300_KEY_S][39]=11039;
    GER_KEY[SU300_KEY_S][40]=10940;

    GER_KEY[SU300_KEY_S][41]=18041;//chpark.11.02
    GER_KEY[SU300_KEY_S][42]=100042;//CAP //chpark.11.02
    GER_KEY[SU300_KEY_S][43]=4443;//chpark.11.02
    GER_KEY[SU300_KEY_S][44]=4644;//chpark.11.02
    GER_KEY[SU300_KEY_S][45]=3245;//chpark.11.02
    GER_KEY[SU300_KEY_S][46]=9546;//chpark.11.02
    GER_KEY[SU300_KEY_S][47]=4547;//chpark.11.02
    GER_KEY[SU300_KEY_S][48]=6148;//chpark.11.02
    GER_KEY[SU300_KEY_S][49]=12749;//DEL //chpark.11.02
    GER_KEY[SU300_KEY_S][50]=10050;//D7 //chpark.11.02

    GER_KEY[SU300_KEY_C][0]=0;//none
    GER_KEY[SU300_KEY_C][1]=4901;
    GER_KEY[SU300_KEY_C][2]=5002;
    GER_KEY[SU300_KEY_C][3]=5103;
    GER_KEY[SU300_KEY_C][4]=5204;
    GER_KEY[SU300_KEY_C][5]=5305;
    GER_KEY[SU300_KEY_C][6]=5406;
    GER_KEY[SU300_KEY_C][7]=5507;
    GER_KEY[SU300_KEY_C][8]=5608;
    GER_KEY[SU300_KEY_C][9]=5709;
    GER_KEY[SU300_KEY_C][10]=4810;
    GER_KEY[SU300_KEY_C][11]=22311;
    GER_KEY[SU300_KEY_C][12]=8112;
    GER_KEY[SU300_KEY_C][13]=8713;
    GER_KEY[SU300_KEY_C][14]=6914;
    GER_KEY[SU300_KEY_C][15]=8215;
    GER_KEY[SU300_KEY_C][16]=8416;
    GER_KEY[SU300_KEY_C][17]=9017;
    GER_KEY[SU300_KEY_C][18]=8518;
    GER_KEY[SU300_KEY_C][19]=7319;
    GER_KEY[SU300_KEY_C][20]=7920;
    GER_KEY[SU300_KEY_C][21]=8021;
    GER_KEY[SU300_KEY_C][22]=22022;
    GER_KEY[SU300_KEY_C][23]=6523;
    GER_KEY[SU300_KEY_C][24]=8324;
    GER_KEY[SU300_KEY_C][25]=6825;
    GER_KEY[SU300_KEY_C][26]=7026;
    GER_KEY[SU300_KEY_C][27]=7127;
    GER_KEY[SU300_KEY_C][28]=7228;
    GER_KEY[SU300_KEY_C][29]=7429;
    GER_KEY[SU300_KEY_C][30]=7530;
    GER_KEY[SU300_KEY_C][31]=7631;
    GER_KEY[SU300_KEY_C][32]=21432;
    GER_KEY[SU300_KEY_C][33]=19633;
    GER_KEY[SU300_KEY_C][34]=8934;
    GER_KEY[SU300_KEY_C][35]=8835;
    GER_KEY[SU300_KEY_C][36]=6736;
    GER_KEY[SU300_KEY_C][37]=8637;
    GER_KEY[SU300_KEY_C][38]=6638;
    GER_KEY[SU300_KEY_C][39]=7839;
    GER_KEY[SU300_KEY_C][40]=7740;

    GER_KEY[SU300_KEY_C][41]=9641;//chpark.11.02
    GER_KEY[SU300_KEY_C][42]=100042;//chpark.11.02//CAP
    GER_KEY[SU300_KEY_C][43]=4443;//chpark.11.02
    GER_KEY[SU300_KEY_C][44]=4644;//chpark.11.02
    GER_KEY[SU300_KEY_C][45]=3245;//chpark.11.02
    GER_KEY[SU300_KEY_C][46]=9546;//chpark.11.02
    GER_KEY[SU300_KEY_C][47]=4547;//chpark.11.02
    GER_KEY[SU300_KEY_C][48]=6148;//chpark.11.02
    GER_KEY[SU300_KEY_C][49]=12749;//DEL //chpark.11.02
    GER_KEY[SU300_KEY_C][50]=10050;//D7  //chpark.11.02

    SU300_CAP_KEY[SU300_LANG_GER][SU300_KEY_S] = 42;//chpark.11.02
    SU300_CAP_KEY[SU300_LANG_GER][SU300_KEY_C] = 42;//chpark.11.02

    SU300_DEL_KEY[SU300_LANG_GER][SU300_KEY_S] = 49;//chpark.11.02
    SU300_DEL_KEY[SU300_LANG_GER][SU300_KEY_C] = 49;//chpark.11.02

    SU300_D7_KEY[SU300_LANG_GER][SU300_KEY_S] = 50;//chpark.11.02
    SU300_D7_KEY[SU300_LANG_GER][SU300_KEY_C] = 50;//chpark.11.02
}


void SU300_InitKeyMaping_FR()
{
    FR_KEY[SU300_KEY_S][0]=0;//none
    FR_KEY[SU300_KEY_S][1]=0;//not
    FR_KEY[SU300_KEY_S][2]=23302;
    FR_KEY[SU300_KEY_S][3]=0;//not
    FR_KEY[SU300_KEY_S][4]=0;//not
    FR_KEY[SU300_KEY_S][5]=0;//not
    FR_KEY[SU300_KEY_S][6]=0;//not
    FR_KEY[SU300_KEY_S][7]=23207;
    FR_KEY[SU300_KEY_S][8]=0;//not
    FR_KEY[SU300_KEY_S][9]=19909;
    FR_KEY[SU300_KEY_S][10]=22410;
    FR_KEY[SU300_KEY_S][11]=9711;
    FR_KEY[SU300_KEY_S][12]=12212;
    FR_KEY[SU300_KEY_S][13]=10113;
    FR_KEY[SU300_KEY_S][14]=11414;
    FR_KEY[SU300_KEY_S][15]=11615;
    FR_KEY[SU300_KEY_S][16]=12116;
    FR_KEY[SU300_KEY_S][17]=11717;
    FR_KEY[SU300_KEY_S][18]=10518;
    FR_KEY[SU300_KEY_S][19]=11119;
    FR_KEY[SU300_KEY_S][20]=11220;
    FR_KEY[SU300_KEY_S][21]=9421;
    FR_KEY[SU300_KEY_S][22]=11322;
    FR_KEY[SU300_KEY_S][23]=11523;
    FR_KEY[SU300_KEY_S][24]=10024;
    FR_KEY[SU300_KEY_S][25]=10225;
    FR_KEY[SU300_KEY_S][26]=10326;
    FR_KEY[SU300_KEY_S][27]=10427;
    FR_KEY[SU300_KEY_S][28]=10628;
    FR_KEY[SU300_KEY_S][29]=10729;
    FR_KEY[SU300_KEY_S][30]=10830;
    FR_KEY[SU300_KEY_S][31]=10931;
    FR_KEY[SU300_KEY_S][32]=24932;
    FR_KEY[SU300_KEY_S][33]=11933;
    FR_KEY[SU300_KEY_S][34]=12034;
    FR_KEY[SU300_KEY_S][35]=9935;
    FR_KEY[SU300_KEY_S][36]=11836;
    FR_KEY[SU300_KEY_S][37]=9837;
    FR_KEY[SU300_KEY_S][38]=11038;
    FR_KEY[SU300_KEY_S][39]=18039;
    FR_KEY[SU300_KEY_S][40]=100040;
    FR_KEY[SU300_KEY_S][41]=4441;
    FR_KEY[SU300_KEY_S][42]=4642;
    FR_KEY[SU300_KEY_S][43]=3243;
    FR_KEY[SU300_KEY_S][44]=9544;
    FR_KEY[SU300_KEY_S][45]=4545;
    FR_KEY[SU300_KEY_S][46]=6146;
    FR_KEY[SU300_KEY_S][47]=12747;
    FR_KEY[SU300_KEY_S][48]=10048;//D7
    FR_KEY[SU300_KEY_S][49]=0;//not

    FR_KEY[SU300_KEY_C][0]=0;//none
    FR_KEY[SU300_KEY_C][1]=4901;
    FR_KEY[SU300_KEY_C][2]=5002;
    FR_KEY[SU300_KEY_C][3]=5103;
    FR_KEY[SU300_KEY_C][4]=5204;
    FR_KEY[SU300_KEY_C][5]=5305;
    FR_KEY[SU300_KEY_C][6]=5406;
    FR_KEY[SU300_KEY_C][7]=5507;
    FR_KEY[SU300_KEY_C][8]=5608;
    FR_KEY[SU300_KEY_C][9]=5709;
    FR_KEY[SU300_KEY_C][10]=4810;
    FR_KEY[SU300_KEY_C][11]=6511;
    FR_KEY[SU300_KEY_C][12]=9012;
    FR_KEY[SU300_KEY_C][13]=6913;
    FR_KEY[SU300_KEY_C][14]=8214;
    FR_KEY[SU300_KEY_C][15]=8415;
    FR_KEY[SU300_KEY_C][16]=8916;
    FR_KEY[SU300_KEY_C][17]=8517;
    FR_KEY[SU300_KEY_C][18]=7318;
    FR_KEY[SU300_KEY_C][19]=7919;
    FR_KEY[SU300_KEY_C][20]=8020;
    FR_KEY[SU300_KEY_C][21]=16821;
    FR_KEY[SU300_KEY_C][22]=8122;
    FR_KEY[SU300_KEY_C][23]=8323;
    FR_KEY[SU300_KEY_C][24]=6824;
    FR_KEY[SU300_KEY_C][25]=7025;
    FR_KEY[SU300_KEY_C][26]=7126;
    FR_KEY[SU300_KEY_C][27]=7227;
    FR_KEY[SU300_KEY_C][28]=7428;
    FR_KEY[SU300_KEY_C][29]=7529;
    FR_KEY[SU300_KEY_C][30]=7630;
    FR_KEY[SU300_KEY_C][31]=7731;
    FR_KEY[SU300_KEY_C][32]=8732;
    FR_KEY[SU300_KEY_C][33]=8833;
    FR_KEY[SU300_KEY_C][34]=6734;
    FR_KEY[SU300_KEY_C][35]=8635;
    FR_KEY[SU300_KEY_C][36]=6636;
    FR_KEY[SU300_KEY_C][37]=7837;
    FR_KEY[SU300_KEY_C][38]=9638;//chpark.11.02
    FR_KEY[SU300_KEY_C][39]=100039;
    FR_KEY[SU300_KEY_C][40]=4440;
    FR_KEY[SU300_KEY_C][41]=4641;
    FR_KEY[SU300_KEY_C][42]=3242;
    FR_KEY[SU300_KEY_C][43]=9543;
    FR_KEY[SU300_KEY_C][44]=4544;
    FR_KEY[SU300_KEY_C][45]=6145;
    FR_KEY[SU300_KEY_C][46]=12746;//DEL
    FR_KEY[SU300_KEY_C][47]=10047;//D7
    FR_KEY[SU300_KEY_C][48]=0;//not
    FR_KEY[SU300_KEY_C][49]=0;//not

    SU300_CAP_KEY[SU300_LANG_FR][SU300_KEY_S] = 40;
    SU300_CAP_KEY[SU300_LANG_FR][SU300_KEY_C] = 39;

    SU300_DEL_KEY[SU300_LANG_FR][SU300_KEY_S] = 47;
    SU300_DEL_KEY[SU300_LANG_FR][SU300_KEY_C] = 46;

    SU300_D7_KEY[SU300_LANG_FR][SU300_KEY_S] = 48;
    SU300_D7_KEY[SU300_LANG_FR][SU300_KEY_C] = 47;
}


void SU300_InitKeyMaping_IT()
{
    IT_KEY[SU300_KEY_S][0]=0;//none
    IT_KEY[SU300_KEY_S][1]=4901;
    IT_KEY[SU300_KEY_S][2]=5002;
    IT_KEY[SU300_KEY_S][3]=5103;
    IT_KEY[SU300_KEY_S][4]=5204;
    IT_KEY[SU300_KEY_S][5]=5305;
    IT_KEY[SU300_KEY_S][6]=5406;
    IT_KEY[SU300_KEY_S][7]=5507;
    IT_KEY[SU300_KEY_S][8]=5608;
    IT_KEY[SU300_KEY_S][9]=5709;
    IT_KEY[SU300_KEY_S][10]=23610;//chpark.24.02.20
    IT_KEY[SU300_KEY_S][11]=11311;
    IT_KEY[SU300_KEY_S][12]=11912;
    IT_KEY[SU300_KEY_S][13]=10113;
    IT_KEY[SU300_KEY_S][14]=11414;
    IT_KEY[SU300_KEY_S][15]=11615;
    IT_KEY[SU300_KEY_S][16]=12116;
    IT_KEY[SU300_KEY_S][17]=11717;
    IT_KEY[SU300_KEY_S][18]=10518;
    IT_KEY[SU300_KEY_S][19]=11119;
    IT_KEY[SU300_KEY_S][20]=11220;
    IT_KEY[SU300_KEY_S][21]=23221;
    IT_KEY[SU300_KEY_S][22]=9722;
    IT_KEY[SU300_KEY_S][23]=11523;
    IT_KEY[SU300_KEY_S][24]=10024;
    IT_KEY[SU300_KEY_S][25]=10225;
    IT_KEY[SU300_KEY_S][26]=10326;
    IT_KEY[SU300_KEY_S][27]=10427;
    IT_KEY[SU300_KEY_S][28]=10628;
    IT_KEY[SU300_KEY_S][29]=10729;
    IT_KEY[SU300_KEY_S][30]=10830;
    IT_KEY[SU300_KEY_S][31]=24231;//chpark.11.02
    IT_KEY[SU300_KEY_S][32]=22432;//chpark.11.02
    IT_KEY[SU300_KEY_S][33]=12233;
    IT_KEY[SU300_KEY_S][34]=12034;
    IT_KEY[SU300_KEY_S][35]=9935;
    IT_KEY[SU300_KEY_S][36]=11836;
    IT_KEY[SU300_KEY_S][37]=9837;
    IT_KEY[SU300_KEY_S][38]=11038;
    IT_KEY[SU300_KEY_S][39]=10939;
    IT_KEY[SU300_KEY_S][40]=24940;//chpark.11.02
    IT_KEY[SU300_KEY_S][41]=100041;//CAPS //chpark.11.02
    IT_KEY[SU300_KEY_S][42]=4442;//chpark.11.02
    IT_KEY[SU300_KEY_S][43]=4643;//chpark.11.02
    IT_KEY[SU300_KEY_S][44]=3244;//chpark.11.02
    IT_KEY[SU300_KEY_S][45]=9545;//chpark.11.02
    IT_KEY[SU300_KEY_S][46]=4546;//chpark.11.02
    IT_KEY[SU300_KEY_S][47]=6147;//chpark.11.02
    IT_KEY[SU300_KEY_S][48]=12748;//DEL //chpark.11.02
    IT_KEY[SU300_KEY_S][49]=10049;//D7 //chpark.11.02
    IT_KEY[SU300_KEY_S][50]=0;//not

    IT_KEY[SU300_KEY_C][0]=0;//none
    IT_KEY[SU300_KEY_C][1]=4901;
    IT_KEY[SU300_KEY_C][2]=5002;
    IT_KEY[SU300_KEY_C][3]=5103;
    IT_KEY[SU300_KEY_C][4]=5204;
    IT_KEY[SU300_KEY_C][5]=5305;
    IT_KEY[SU300_KEY_C][6]=5406;
    IT_KEY[SU300_KEY_C][7]=5507;
    IT_KEY[SU300_KEY_C][8]=5608;
    IT_KEY[SU300_KEY_C][9]=5709;
    IT_KEY[SU300_KEY_C][10]=4810;
    IT_KEY[SU300_KEY_C][11]=8111;
    IT_KEY[SU300_KEY_C][12]=8712;
    IT_KEY[SU300_KEY_C][13]=6913;
    IT_KEY[SU300_KEY_C][14]=8214;
    IT_KEY[SU300_KEY_C][15]=84515;
    IT_KEY[SU300_KEY_C][16]=8916;
    IT_KEY[SU300_KEY_C][17]=8517;
    IT_KEY[SU300_KEY_C][18]=7318;
    IT_KEY[SU300_KEY_C][19]=7919;
    IT_KEY[SU300_KEY_C][20]=8020;
    IT_KEY[SU300_KEY_C][21]=23321;
    IT_KEY[SU300_KEY_C][22]=6522;
    IT_KEY[SU300_KEY_C][23]=8323;
    IT_KEY[SU300_KEY_C][24]=6824;
    IT_KEY[SU300_KEY_C][25]=7025;
    IT_KEY[SU300_KEY_C][26]=7126;
    IT_KEY[SU300_KEY_C][27]=7227;
    IT_KEY[SU300_KEY_C][28]=7428;
    IT_KEY[SU300_KEY_C][29]=7529;
    IT_KEY[SU300_KEY_C][30]=7630;
    IT_KEY[SU300_KEY_C][31]=9631;
    IT_KEY[SU300_KEY_C][32]=9032;
    IT_KEY[SU300_KEY_C][33]=8833;
    IT_KEY[SU300_KEY_C][34]=6734;
    IT_KEY[SU300_KEY_C][35]=8635;
    IT_KEY[SU300_KEY_C][36]=6636;
    IT_KEY[SU300_KEY_C][37]=7837;
    IT_KEY[SU300_KEY_C][38]=7738;
    IT_KEY[SU300_KEY_C][39]=18039;//chpark.11.02
    IT_KEY[SU300_KEY_C][40]=100040;//CAPS //chpark.11.02
    IT_KEY[SU300_KEY_C][41]=4441;//chpark.11.02
    IT_KEY[SU300_KEY_C][42]=4642;//chpark.11.02
    IT_KEY[SU300_KEY_C][43]=3243;//chpark.11.02
    IT_KEY[SU300_KEY_C][44]=9544;//chpark.11.02
    IT_KEY[SU300_KEY_C][45]=4545;//chpark.11.02
    IT_KEY[SU300_KEY_C][46]=6146;//chpark.11.02
    IT_KEY[SU300_KEY_C][47]=12747;//DEL //chpark.11.02
    IT_KEY[SU300_KEY_C][48]=10048;//D7
    IT_KEY[SU300_KEY_C][49]=0;//not
    IT_KEY[SU300_KEY_C][50]=0;//not

    SU300_CAP_KEY[SU300_LANG_IT][SU300_KEY_S] = 41;//chpark.11.02
    SU300_CAP_KEY[SU300_LANG_IT][SU300_KEY_C] = 40;//chpark.11.02

    SU300_DEL_KEY[SU300_LANG_IT][SU300_KEY_S] = 48;//chpark.11.02
    SU300_DEL_KEY[SU300_LANG_IT][SU300_KEY_C] = 47;//chpark.11.02

    SU300_D7_KEY[SU300_LANG_IT][SU300_KEY_S] = 49;//chpark.11.02
    SU300_D7_KEY[SU300_LANG_IT][SU300_KEY_C] = 48;//chpark.11.02
}


void SU300_InitKeyMaping_SP()
{
    SP_KEY[SU300_KEY_S][0]=0;//none
    SP_KEY[SU300_KEY_S][1]=4901;
    SP_KEY[SU300_KEY_S][2]=5002;
    SP_KEY[SU300_KEY_S][3]=5103;
    SP_KEY[SU300_KEY_S][4]=5204;
    SP_KEY[SU300_KEY_S][5]=5305;
    SP_KEY[SU300_KEY_S][6]=5406;
    SP_KEY[SU300_KEY_S][7]=5507;
    SP_KEY[SU300_KEY_S][8]=5608;
    SP_KEY[SU300_KEY_S][9]=5709;
    SP_KEY[SU300_KEY_S][10]=4810;
    SP_KEY[SU300_KEY_S][11]=11311;
    SP_KEY[SU300_KEY_S][12]=11912;
    SP_KEY[SU300_KEY_S][13]=10113;
    SP_KEY[SU300_KEY_S][14]=11414;
    SP_KEY[SU300_KEY_S][15]=11615;
    SP_KEY[SU300_KEY_S][16]=12116;
    SP_KEY[SU300_KEY_S][17]=11717;
    SP_KEY[SU300_KEY_S][18]=10518;
    SP_KEY[SU300_KEY_S][19]=11119;
    SP_KEY[SU300_KEY_S][20]=11220;
    SP_KEY[SU300_KEY_S][21]=9721;
    SP_KEY[SU300_KEY_S][22]=11522;
    SP_KEY[SU300_KEY_S][23]=10023;
    SP_KEY[SU300_KEY_S][24]=10224;
    SP_KEY[SU300_KEY_S][25]=10325;
    SP_KEY[SU300_KEY_S][26]=10426;
    SP_KEY[SU300_KEY_S][27]=10627;
    SP_KEY[SU300_KEY_S][28]=10728;
    SP_KEY[SU300_KEY_S][29]=10829;
    SP_KEY[SU300_KEY_S][30]=24130;
    SP_KEY[SU300_KEY_S][31]=9631;//chpark.11.02
    SP_KEY[SU300_KEY_S][32]=12232;
    SP_KEY[SU300_KEY_S][33]=12033;
    SP_KEY[SU300_KEY_S][34]=9934;
    SP_KEY[SU300_KEY_S][35]=11835;
    SP_KEY[SU300_KEY_S][36]=9836;
    SP_KEY[SU300_KEY_S][37]=11037;
    SP_KEY[SU300_KEY_S][38]=10938;
    SP_KEY[SU300_KEY_S][39]=18039;//chpark.11.02
    
    SP_KEY[SU300_KEY_S][40]=100040;//chpark.11.02
    SP_KEY[SU300_KEY_S][41]=4441;//chpark.11.02
    SP_KEY[SU300_KEY_S][42]=4642;//chpark.11.02
    SP_KEY[SU300_KEY_S][43]=3243;//chpark.11.02
    SP_KEY[SU300_KEY_S][44]=9544;//chpark.11.02
    SP_KEY[SU300_KEY_S][45]=4545;//chpark.11.02
    SP_KEY[SU300_KEY_S][46]=6146;//chpark.11.02
    SP_KEY[SU300_KEY_S][47]=12747;//chpark.11.02
    SP_KEY[SU300_KEY_S][48]=10048;//D7
    SP_KEY[SU300_KEY_S][49]=0;//not//chpark.11.02
    SP_KEY[SU300_KEY_S][50]=0;//not

    SP_KEY[SU300_KEY_C][0]=0;//none
    SP_KEY[SU300_KEY_C][1]=4901;
    SP_KEY[SU300_KEY_C][2]=5002;
    SP_KEY[SU300_KEY_C][3]=5103;
    SP_KEY[SU300_KEY_C][4]=5204;
    SP_KEY[SU300_KEY_C][5]=5305;
    SP_KEY[SU300_KEY_C][6]=5406;
    SP_KEY[SU300_KEY_C][7]=5507;
    SP_KEY[SU300_KEY_C][8]=5608;
    SP_KEY[SU300_KEY_C][9]=5709;
    SP_KEY[SU300_KEY_C][10]=4810;
    SP_KEY[SU300_KEY_C][11]=8111;
    SP_KEY[SU300_KEY_C][12]=8712;
    SP_KEY[SU300_KEY_C][13]=6913;
    SP_KEY[SU300_KEY_C][14]=8214;
    SP_KEY[SU300_KEY_C][15]=8415;
    SP_KEY[SU300_KEY_C][16]=8916;
    SP_KEY[SU300_KEY_C][17]=8517;
    SP_KEY[SU300_KEY_C][18]=7318;
    SP_KEY[SU300_KEY_C][19]=7919;
    SP_KEY[SU300_KEY_C][20]=8020;
    SP_KEY[SU300_KEY_C][21]=6521;
    SP_KEY[SU300_KEY_C][22]=8322;
    SP_KEY[SU300_KEY_C][23]=6823;
    SP_KEY[SU300_KEY_C][24]=7024;
    SP_KEY[SU300_KEY_C][25]=7125;
    SP_KEY[SU300_KEY_C][26]=7226;
    SP_KEY[SU300_KEY_C][27]=7427;
    SP_KEY[SU300_KEY_C][28]=7528;
    SP_KEY[SU300_KEY_C][29]=7629;
    SP_KEY[SU300_KEY_C][30]=20930;
    SP_KEY[SU300_KEY_C][31]=9431;//chpark.11.02
    SP_KEY[SU300_KEY_C][32]=9032;
    SP_KEY[SU300_KEY_C][33]=8833;
    SP_KEY[SU300_KEY_C][34]=6734;
    SP_KEY[SU300_KEY_C][35]=8635;
    SP_KEY[SU300_KEY_C][36]=6636;
    SP_KEY[SU300_KEY_C][37]=7837;
    SP_KEY[SU300_KEY_C][38]=7738;
    SP_KEY[SU300_KEY_C][39]=16839;//chpark.11.02
    SP_KEY[SU300_KEY_C][40]=100040;//chpark.11.02
    SP_KEY[SU300_KEY_C][41]=4441;//chpark.11.02
    SP_KEY[SU300_KEY_C][42]=4642;//chpark.11.02
    SP_KEY[SU300_KEY_C][43]=3243;//chpark.11.02
    SP_KEY[SU300_KEY_C][44]=9544;//chpark.11.02
    SP_KEY[SU300_KEY_C][45]=4545;//chpark.11.02
    SP_KEY[SU300_KEY_C][46]=6146;//chpark.11.02
    SP_KEY[SU300_KEY_C][47]=12747;//chpark.11.02
    SP_KEY[SU300_KEY_C][48]=10048;//D7
    SP_KEY[SU300_KEY_C][49]=0;//not
    SP_KEY[SU300_KEY_C][50]=0;//not

    SU300_CAP_KEY[SU300_LANG_SP][SU300_KEY_S] = 40;//chpark.11.02
    SU300_CAP_KEY[SU300_LANG_SP][SU300_KEY_C] = 40;//chpark.11.02

    SU300_DEL_KEY[SU300_LANG_SP][SU300_KEY_S] = 47;//chpark.11.02
    SU300_DEL_KEY[SU300_LANG_SP][SU300_KEY_C] = 47;//chpark.11.02

    SU300_D7_KEY[SU300_LANG_SP][SU300_KEY_S] = 48;//chpark.11.02
    SU300_D7_KEY[SU300_LANG_SP][SU300_KEY_C] = 48;//chpark.11.02
}


//1: Focus index
//2: Down image
bool SU300_Button_Select(int nIndex,bool bDownImage)
{
    char i;
	
    int nTmpPrevIndex = m_nSU300_ButtonIndex;
    //save new key index
    m_nSU300_ButtonIndex = nIndex;

    int x, y;
    int nKeyTypeA_END,nKeyTypeB_END,nKeyTypeC_END;

    if(Lang_Flag[M7_UserIndex]==SU300_LANG_ENG)
    {
        nKeyTypeA_END = 36;
        nKeyTypeC_END = 44;

        if(nIndex>=1 && nIndex<=nKeyTypeA_END)
        {
            //A Type Show (C Hide)(B NONE)
            ituWidgetGetGlobalPosition(SU300_Key_Bt[nIndex-1], &x, &y);
            ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);

            ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);
              
            ituWidgetSetVisible (SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]], true);
            ituWidgetSetPosition(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],x ,y);
            if(bDownImage)
            {
                ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], true);
                ituWidgetSetPosition(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]],x, y);
            }
            else
            {
                ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
            }

            ituWidgetSetVisible(SU300_ICON_D7_SEL, false);
            ituWidgetSetVisible(SU300_ICON_D7_DOWN, false);
            
            //printf("[%d]Button POS : x=%d y=%d========\n",nIndex,x,y);
        }
        else if(nIndex>nKeyTypeA_END && nIndex<=nKeyTypeC_END)
        {
            //C Type Show (A Hide)(B NONE)
            ituWidgetGetGlobalPosition(SU300_Key_Bt[nIndex-1], &x, &y);
            ituWidgetSetVisible(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);

            ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);

            ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]], true);
            ituWidgetSetPosition(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],x, y);
            if(bDownImage)
            {
                ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], true);
                ituWidgetSetPosition(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]],x, y);
            }
            else
            {
                ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
            }

            ituWidgetSetVisible(SU300_ICON_D7_SEL, false);
            ituWidgetSetVisible(SU300_ICON_D7_DOWN, false);

            //printf("[%d]Button POS : x=%d y=%d========\n",nIndex,x,y);        
        }
        else if(nIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            //D7 Button
            
            //key disable
            ituWidgetSetVisible(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
            ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);
            ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
            //D7 Push
            ituWidgetSetVisible(SU300_ICON_D7_SEL, true);
            if(bDownImage)
            {
                ituWidgetSetVisible(SU300_ICON_D7_DOWN, true);

            }
            else
            {
                ituWidgetSetVisible(SU300_ICON_D7_DOWN, false);
            }

        }
        else
        {
            m_nSU300_ButtonIndex = nTmpPrevIndex;
            printf("[%d]Button POS ERROR: x=%d y=%d========\n",nIndex,x,y);
        }
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_GER)
    {
        nKeyTypeB_END = 41;//chpark.11.02
        nKeyTypeC_END = 49;

        if(nIndex>=1 && nIndex<=nKeyTypeB_END)
        {
             //B Type Show (A,C Hide)
            ituWidgetGetGlobalPosition(SU300_Key_Bt[nIndex-1], &x, &y);
            ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);

            ituWidgetSetVisible(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
              
            ituWidgetSetVisible (SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]], true);
            ituWidgetSetPosition(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],x ,y);
            if(bDownImage)
            {
                ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], true);
                ituWidgetSetPosition(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]],x, y);
            }
            else
            {
                ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);
            }

            ituWidgetSetVisible(SU300_ICON_D7_SEL, false);
            ituWidgetSetVisible(SU300_ICON_D7_DOWN, false);
            
            //printf("[%d]Button POS : x=%d y=%d========\n",nIndex,x,y);
        }
        else if(nIndex>nKeyTypeB_END && nIndex<=nKeyTypeC_END)
        {
             //C Type Show (A,B Hide)
            ituWidgetGetGlobalPosition(SU300_Key_Bt[nIndex-1], &x, &y);
            ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);

            ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]], true);
            ituWidgetSetPosition(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],x, y);
            if(bDownImage)
            {
                ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], true);
                ituWidgetSetPosition(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]],x, y);
            }
            else
            {
                ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
            }

            ituWidgetSetVisible(SU300_ICON_D7_SEL, false);
            ituWidgetSetVisible(SU300_ICON_D7_DOWN, false);

            //printf("[%d]Button POS : x=%d y=%d========\n",nIndex,x,y);        
        }
        else if(nIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            //D7 Button
            
            //key disable
            ituWidgetSetVisible(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
            ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);
            ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
            //D7 Push
            ituWidgetSetVisible(SU300_ICON_D7_SEL, true);
            if(bDownImage)
            {
                ituWidgetSetVisible(SU300_ICON_D7_DOWN, true);

            }
            else
            {
                ituWidgetSetVisible(SU300_ICON_D7_DOWN, false);
            }

        }
        else
        {
            m_nSU300_ButtonIndex = nTmpPrevIndex;
            printf("[%d]Button POS ERROR: x=%d y=%d========\n",nIndex,x,y);
        }

    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_FR)
    {
        //chpark.11.02
        if(!SU300_b_CAP)
        {
            nKeyTypeA_END = 39;
            
            nKeyTypeC_END = 47;
        }
        else
        {
            nKeyTypeA_END = 38;
            
            nKeyTypeC_END = 46;
        }

        if(nIndex>=1 && nIndex<=nKeyTypeA_END)
        {
            ituWidgetGetGlobalPosition(SU300_Key_Bt[nIndex-1], &x, &y);
            if(!SU300_b_CAP)
            {
                if(nIndex>=11 && nIndex<=32)
                {
                    //B Type Show (A,C Hide)
                    ituWidgetSetVisible(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);

                    ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
                    
                    ituWidgetSetVisible (SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]], true);
                    ituWidgetSetPosition(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],x ,y);
                    if(bDownImage)
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], true);
                        ituWidgetSetPosition(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]],x, y);
                    }
                    else
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);
                    }
                }
                else
                {
                    //A Type Show (B,C Hide)
                    ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);

                    ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
                    
                    ituWidgetSetVisible (SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]], true);
                    ituWidgetSetPosition(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],x ,y);
                    if(bDownImage)
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], true);
                        ituWidgetSetPosition(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]],x, y);
                    }
                    else
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
                    }
                }
            }
            else
            {
                if(nIndex>=11 && nIndex<=21)
                {
                    //B Type Show (A,C Hide)
                    ituWidgetSetVisible(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);

                    ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
                    
                    ituWidgetSetVisible (SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]], true);
                    ituWidgetSetPosition(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],x ,y);
                    if(bDownImage)
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], true);
                        ituWidgetSetPosition(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]],x, y);
                    }
                    else
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);
                    }
                }
                else
                {
                    //A Type Show (B,C Hide)
                    ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);

                    ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
                    
                    ituWidgetSetVisible (SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]], true);
                    ituWidgetSetPosition(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],x ,y);
                    if(bDownImage)
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], true);
                        ituWidgetSetPosition(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]],x, y);
                    }
                    else
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
                    }
                }
            }
            
            ituWidgetSetVisible(SU300_ICON_D7_SEL, false);
            ituWidgetSetVisible(SU300_ICON_D7_DOWN, false);
            
            //printf("[%d]Button POS : x=%d y=%d========\n",nIndex,x,y);
        }
        else if(nIndex>nKeyTypeA_END && nIndex<=nKeyTypeC_END)
        {
            //C Type Show (A,B Hide)
            ituWidgetGetGlobalPosition(SU300_Key_Bt[nIndex-1], &x, &y);
            ituWidgetSetVisible(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
            ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);

            ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]], true);
            ituWidgetSetPosition(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],x, y);
            if(bDownImage)
            {
                ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], true);
                ituWidgetSetPosition(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]],x, y);
            }
            else
            {
                ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
            }

            ituWidgetSetVisible(SU300_ICON_D7_SEL, false);
            ituWidgetSetVisible(SU300_ICON_D7_DOWN, false);

            //printf("[%d]Button POS : x=%d y=%d========\n",nIndex,x,y);        
        }
        else if(nIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            //D7 Button
            
            //key disable
            ituWidgetSetVisible(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
            ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);
            ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
            //D7 Push
            ituWidgetSetVisible(SU300_ICON_D7_SEL, true);
            if(bDownImage)
            {
                ituWidgetSetVisible(SU300_ICON_D7_DOWN, true);

            }
            else
            {
                ituWidgetSetVisible(SU300_ICON_D7_DOWN, false);
            }

        }
        else
        {
            m_nSU300_ButtonIndex = nTmpPrevIndex;
            printf("[%d]Button POS ERROR: x=%d y=%d========\n",nIndex,x,y);
        }
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_IT)
    {
        //chpark.11.02
        if(!SU300_b_CAP)
        {
            nKeyTypeA_END = 40;
            
            nKeyTypeC_END = 48;
        }
        else
        {
            nKeyTypeA_END = 39;
            
            nKeyTypeC_END = 47;
        }

        if(nIndex>=1 && nIndex<=nKeyTypeA_END)
        {
            ituWidgetGetGlobalPosition(SU300_Key_Bt[nIndex-1], &x, &y);
            if(!SU300_b_CAP)
            {
                if(nIndex>=11 && nIndex<=40)
                {
                    //B Type Show (A,C Hide)
                    ituWidgetSetVisible(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);

                    ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
                    
                    ituWidgetSetVisible (SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]], true);
                    ituWidgetSetPosition(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],x ,y);
                    if(bDownImage)
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], true);
                        ituWidgetSetPosition(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]],x, y);
                    }
                    else
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);
                    }
                }
                else
                {
                    //A Type Show (B,C Hide)
                    ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);

                    ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
                    
                    ituWidgetSetVisible (SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]], true);
                    ituWidgetSetPosition(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],x ,y);
                    if(bDownImage)
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], true);
                        ituWidgetSetPosition(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]],x, y);
                    }
                    else
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
                    }
                }
            }
            else
            {
                if( (nIndex>=11 && nIndex<=21) || (nIndex>=32 && nIndex<=39) )//chpark.11.02
                {
                    //B Type Show (A,C Hide)
                    ituWidgetSetVisible(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);

                    ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
                    
                    ituWidgetSetVisible (SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]], true);
                    ituWidgetSetPosition(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],x ,y);
                    if(bDownImage)
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], true);
                        ituWidgetSetPosition(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]],x, y);
                    }
                    else
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);
                    }
                }
                else
                {
                    //A Type Show (B,C Hide)
                    ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);

                    ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
                    ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
                    
                    ituWidgetSetVisible (SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]], true);
                    ituWidgetSetPosition(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],x ,y);
                    if(bDownImage)
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], true);
                        ituWidgetSetPosition(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]],x, y);
                    }
                    else
                    {
                        ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
                    }
                }

            }
            
            ituWidgetSetVisible(SU300_ICON_D7_SEL, false);
            ituWidgetSetVisible(SU300_ICON_D7_DOWN, false);
            
            //printf("[%d]Button POS : x=%d y=%d========\n",nIndex,x,y);
        }
        else if(nIndex>nKeyTypeA_END && nIndex<=nKeyTypeC_END)
        {
            //C Type Show (A,B Hide)
            ituWidgetGetGlobalPosition(SU300_Key_Bt[nIndex-1], &x, &y);
            ituWidgetSetVisible(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
            ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);

            ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]], true);
            ituWidgetSetPosition(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],x, y);
            if(bDownImage)
            {
                ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], true);
                ituWidgetSetPosition(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]],x, y);
            }
            else
            {
                ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
            }

            ituWidgetSetVisible(SU300_ICON_D7_SEL, false);
            ituWidgetSetVisible(SU300_ICON_D7_DOWN, false);

            //printf("[%d]Button POS : x=%d y=%d========\n",nIndex,x,y);        
        }
        else if(nIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            //D7 Button
            
            //key disable
            ituWidgetSetVisible(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
            ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);
            ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
            //D7 Push
            ituWidgetSetVisible(SU300_ICON_D7_SEL, true);
            if(bDownImage)
            {
                ituWidgetSetVisible(SU300_ICON_D7_DOWN, true);

            }
            else
            {
                ituWidgetSetVisible(SU300_ICON_D7_DOWN, false);
            }

        }
        else
        {
            m_nSU300_ButtonIndex = nTmpPrevIndex;
            printf("[%d]Button POS ERROR: x=%d y=%d========\n",nIndex,x,y);
        }
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_SP)
    {
        //chpark.11.02
        if(!SU300_b_CAP)
        {
            nKeyTypeA_END = 39;
            
            nKeyTypeC_END = 47;
        }
        else
        {
            nKeyTypeA_END = 39;
            
            nKeyTypeC_END = 47;
        }

        if(nIndex>=1 && nIndex<=nKeyTypeA_END)
        {
            ituWidgetGetGlobalPosition(SU300_Key_Bt[nIndex-1], &x, &y);
            if(nIndex>=21 && nIndex<=39)//chpark.01.15
            {
                //B Type Show (A,C Hide)
                ituWidgetSetVisible(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],  false);
                ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);

                ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
                ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
                    
                ituWidgetSetVisible (SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]], true);
                ituWidgetSetPosition(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],x ,y);
                if(bDownImage)
                {
                    ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], true);
                    ituWidgetSetPosition(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]],x, y);
                }
                else
                {
                    ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);
                }
            }
            else
            {
                    //A Type Show (B,C Hide)
                ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
                ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);

                ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
                ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
                    
                ituWidgetSetVisible (SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]], true);
                ituWidgetSetPosition(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],x ,y);
                if(bDownImage)
                {
                    ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], true);
                    ituWidgetSetPosition(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]],x, y);
                }
                else
                {
                    ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
                }
            }
            
            ituWidgetSetVisible(SU300_ICON_D7_SEL, false);
            ituWidgetSetVisible(SU300_ICON_D7_DOWN, false);
            
            //printf("[%d]Button POS : x=%d y=%d========\n",nIndex,x,y);
        }
        else if(nIndex>nKeyTypeA_END && nIndex<=nKeyTypeC_END)
        {
            //C Type Show (A,B Hide)
            ituWidgetGetGlobalPosition(SU300_Key_Bt[nIndex-1], &x, &y);
            ituWidgetSetVisible(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
            ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);

            ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]], true);
            ituWidgetSetPosition(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],x, y);
            if(bDownImage)
            {
                ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], true);
                ituWidgetSetPosition(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]],x, y);
            }
            else
            {
                ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
            }

            ituWidgetSetVisible(SU300_ICON_D7_SEL, false);
            ituWidgetSetVisible(SU300_ICON_D7_DOWN, false);

            //printf("[%d]Button POS : x=%d y=%d========\n",nIndex,x,y);        
        }
        else if(nIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            //D7 Button
            
            //key disable
            ituWidgetSetVisible(SU300_ICON_SEL_A[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
            ituWidgetSetVisible(SU300_ICON_SEL_B[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);
            ituWidgetSetVisible(SU300_ICON_SEL_C[Lang_Flag[M7_UserIndex]],  false);
            ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
            //D7 Push
            ituWidgetSetVisible(SU300_ICON_D7_SEL, true);
            if(bDownImage)
            {
                ituWidgetSetVisible(SU300_ICON_D7_DOWN, true);

            }
            else
            {
                ituWidgetSetVisible(SU300_ICON_D7_DOWN, false);
            }

        }
        else
        {
            m_nSU300_ButtonIndex = nTmpPrevIndex;
            printf("[%d]Button POS ERROR: x=%d y=%d========\n",nIndex,x,y);
        }
    }
}

void SU300_Display_UserName()
{
    ituWidgetSetVisible(SU300_TEXT_NAME, true);
	//ituTextSetString(SU300_TEXT_NAME, SU300_TxtUserName);	
    
    //SU300_TxtUserName[0] = 'a';
    //SU300_TxtUserName[1] = 'b';
    //SU300_TxtUserName[2] = (char)144;
    //SU300_TxtUserName[3] = (char)144;
    //SU300_TxtUserName[4] = '\0';
    uint8_t tDestName[50];
	memset(tDestName,0,50);
    SU100_ISO8859_to_UTF8(tDestName,SU300_TxtUserName);

    ituTextSetFontWidth(SU300_TEXT_NAME,30);//chpark.24.02.06
    ituTextSetString(SU300_TEXT_NAME,tDestName);	

    //===============================
    //Show cursor
    int len = strlen(SU300_TxtUserName);//char number. utf-8 shoud not use.
    int nPos_X,nPos_Y;
    int n_Char,nChar_width; 
    
        //nPos_X=400;
        nPos_Y=470;
       
        char* pch = tDestName;
        
        int xx = 0;
        int w=0;
        int letterSpacing=0;//no useful
        int cusorSpacing=3;
            
        int nCharNum=0;
        int nTextBox_StartX = 107;
        int nTextBox_SizeX  = 670;
        int nNameBox_X,nNameBox_Y;
        
        if(SU300_b_VMode)
        {
            nPos_Y = 470;
            nTextBox_StartX = 107;//chpark.24.02.06
            nTextBox_SizeX  = 670;
        }
        else
        {
            nPos_Y = 270;
            nTextBox_StartX = 102;//44 //chpark.24.02.06
            nTextBox_SizeX  = 274;
        }
        
       //chpark.01.15
       
        //ituWidgetGetGlobalPosition(SU300_TEXT_NAME, &nNameBox_X, &nNameBox_Y);
        //nTextBox_StartX = nNameBox_X;
        
        while (*pch != '\0')
        {
            int size = ituFtGetCharWidth(pch, &w);
            if (size == 0)
                break;

            //ituFtDrawChar(dest, destx + xx, desty, pch);
            xx += w + letterSpacing;
            pch += size;

            nCharNum++;
            //printf("[New==1]Num=%d  nPos_X=%d w=%d====\n",nCharNum,xx,w);
        }
        //center position
        //nPos_X = nTextBox_StartX + nTextBox_SizeX/2 + xx/2;
        //Left Position
        nPos_X = nTextBox_StartX  + xx + cusorSpacing;
        //printf("[New==final]Num=%d  nPos_X=%d nPos_Y=%d====\n",nCharNum,nPos_X,nPos_Y);
    
    ituWidgetSetPosition(SU300_Cursor[0],nPos_X,nPos_Y);
    //===================================================
}


void SU300_SetUserTitle(uint8_t tUserIdx)
{
	uint8_t i;
	uint8_t tDestName[100];
	
	//Set Icon
	if(User_RegInfo[tUserIdx].RegFlag==SU100_USER_AD)
	{
		ituWidgetSetVisible(SU300_TOP_ICON[SU100_ICON_AD],true);
		ituWidgetSetVisible(SU300_TOP_ICON[SU100_ICON_ST],false);
	}
	else if(User_RegInfo[tUserIdx].RegFlag==SU100_USER_ST || User_RegInfo[tUserIdx].RegFlag==SU100_USER_SPAD||User_RegInfo[tUserIdx].RegFlag==SU100_USER_SAD)
	{
		ituWidgetSetVisible(SU300_TOP_ICON[SU100_ICON_AD],false);
		ituWidgetSetVisible(SU300_TOP_ICON[SU100_ICON_ST],true);
	}
    else
    {
        ituWidgetSetVisible(SU300_TOP_ICON[SU100_ICON_AD],false);
		ituWidgetSetVisible(SU300_TOP_ICON[SU100_ICON_ST],false);
    }

    if(g_UI1_ChkTime>0)
	{
        memset(tDestName,0,100);

		ituWidgetSetVisible(SU300_TOP_NAME,true);
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[tUserIdx].UserName);
		ituTextSetString(SU300_TOP_NAME, tDestName);

        //printf("\n[SU300_TOP_NAME] Show:%s==g_UI1_ChkTime=%d\n",tDestName,g_UI1_ChkTime);
	}
	else if((g_UI1_ChkTime == 0)||(M7_UserIndex > (MAX_SU100_USER_NUM-1)))
	{
		ituWidgetSetVisible(SU300_TOP_NAME,false);
        //printf("\n[SU300_TOP_NAME] Hide:%s==g_UI1_ChkTime=%d\n",tDestName,g_UI1_ChkTime);
	}
}

//=====================================================

void SU300_MenuTabNext()
{
    m_nSU300_ButtonIndex++;
    
    int nTmpMaxKeyIndex = SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP]; //D7 key

    if(m_nSU300_ButtonIndex>nTmpMaxKeyIndex)
    {
        m_nSU300_ButtonIndex = 1;
    }

    //chpark.11.20
    //blank button index in French.
    if(Lang_Flag[M7_UserIndex]==SU300_LANG_FR && !SU300_b_CAP)
    {
        if(m_nSU300_ButtonIndex==1)m_nSU300_ButtonIndex=2;
        else if(m_nSU300_ButtonIndex>=3 && m_nSU300_ButtonIndex<=6)m_nSU300_ButtonIndex=7;
        else if(m_nSU300_ButtonIndex==8)m_nSU300_ButtonIndex=9;
    }

    SU300_Button_Select(m_nSU300_ButtonIndex, false);
}

void SU300_MenuTabPrev()
{
    m_nSU300_ButtonIndex--;
    int nTmpMaxKeyIndex = SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP]; //D7 key

    if(m_nSU300_ButtonIndex<1)
    {
        m_nSU300_ButtonIndex = nTmpMaxKeyIndex;
    }

    //chpark.11.20
    //blank button index in French.
    if(Lang_Flag[M7_UserIndex]==SU300_LANG_FR && !SU300_b_CAP)
    {
        if(m_nSU300_ButtonIndex<=1)m_nSU300_ButtonIndex=nTmpMaxKeyIndex;
        else if(m_nSU300_ButtonIndex>=3 && m_nSU300_ButtonIndex<=6)m_nSU300_ButtonIndex=2;
        else if(m_nSU300_ButtonIndex==8)m_nSU300_ButtonIndex=7;
    }

    SU300_Button_Select(m_nSU300_ButtonIndex, false);
}

void SU300_MenuTabUp()
{
    if(Lang_Flag[M7_UserIndex]==SU300_LANG_ENG)
    {
        if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
        {
            m_nSU300_ButtonIndex=SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];
        }
        else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=20)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-10;
        }
        else if(m_nSU300_ButtonIndex>=21 && m_nSU300_ButtonIndex<=29)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-10;
        }
        else if(m_nSU300_ButtonIndex>=30 && m_nSU300_ButtonIndex<=36)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-8;
        }
        else if(m_nSU300_ButtonIndex==37)
        {
            m_nSU300_ButtonIndex = 30;
        }
        else if(m_nSU300_ButtonIndex>=38 && m_nSU300_ButtonIndex<=44)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-8;
        }
        else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            //m_nSU300_ButtonIndex=44;
            m_nSU300_ButtonIndex = SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];//chpark.24.04.22
        } 

    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_GER)
    {
        if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=11)
        {
            m_nSU300_ButtonIndex = SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];
        }        
        else if(m_nSU300_ButtonIndex>=12 && m_nSU300_ButtonIndex<=33)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-11;
        }
        else if(m_nSU300_ButtonIndex>=34 && m_nSU300_ButtonIndex<=40)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-10;
        }
        else if(m_nSU300_ButtonIndex==41)
        {
            m_nSU300_ButtonIndex = 32;
        }
        else if(m_nSU300_ButtonIndex==42)
        {
            m_nSU300_ButtonIndex = 34;
        }
        else if(m_nSU300_ButtonIndex>=43 && m_nSU300_ButtonIndex<=45)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-9;
        }
        else if(m_nSU300_ButtonIndex>=46 && m_nSU300_ButtonIndex<=49)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-8;
        }
        else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            //m_nSU300_ButtonIndex=49;
            m_nSU300_ButtonIndex = SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];//chpark.24.04.22
        } 

    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_FR)
    {
        if(!SU300_b_CAP)
        {
            if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=11)
            {
                m_nSU300_ButtonIndex=SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];
            }
            else if(m_nSU300_ButtonIndex>=12 && m_nSU300_ButtonIndex<=13)
            {
                m_nSU300_ButtonIndex=2;
            }
            else if(m_nSU300_ButtonIndex>=14 && m_nSU300_ButtonIndex<=16)
            {
                m_nSU300_ButtonIndex=SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];
            }
            else if(m_nSU300_ButtonIndex>=17 && m_nSU300_ButtonIndex<=18)
            {
                m_nSU300_ButtonIndex=7;
            }
            else if(m_nSU300_ButtonIndex>=19 && m_nSU300_ButtonIndex<=20)
            {
                m_nSU300_ButtonIndex=9;
            }
            else if(m_nSU300_ButtonIndex==21)
            {
                m_nSU300_ButtonIndex=10;
            }
            else if(m_nSU300_ButtonIndex>=22 && m_nSU300_ButtonIndex<=32)
            {
                m_nSU300_ButtonIndex=m_nSU300_ButtonIndex-11;
            }
            else if(m_nSU300_ButtonIndex>=33 && m_nSU300_ButtonIndex<=39)
            {
                m_nSU300_ButtonIndex=m_nSU300_ButtonIndex-9;
            }
            else if(m_nSU300_ButtonIndex==40)
            {
                m_nSU300_ButtonIndex=33;
            }
            else if(m_nSU300_ButtonIndex>=41 && m_nSU300_ButtonIndex<=47)
            {
                m_nSU300_ButtonIndex=m_nSU300_ButtonIndex-8;
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                //m_nSU300_ButtonIndex = 47;
                m_nSU300_ButtonIndex = SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];//chpark.24.04.22
            }        
            
        }
        else
        {
            if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
            {
                m_nSU300_ButtonIndex = SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];
            }        
            else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=15)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-10;
            }
            else if(m_nSU300_ButtonIndex>=16 && m_nSU300_ButtonIndex<=21)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-11;
            }
            else if(m_nSU300_ButtonIndex>=22 && m_nSU300_ButtonIndex<=26)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-11;
            }
            else if(m_nSU300_ButtonIndex>=27 && m_nSU300_ButtonIndex<=31)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-10;
            }
            else if(m_nSU300_ButtonIndex>=32 && m_nSU300_ButtonIndex<=35)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-9;
            }
            else if(m_nSU300_ButtonIndex>=36 && m_nSU300_ButtonIndex<=38)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-8;
            }
            else if(m_nSU300_ButtonIndex>=39 && m_nSU300_ButtonIndex<=42)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-7;
            }
            else if(m_nSU300_ButtonIndex>=43 && m_nSU300_ButtonIndex<=46)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-8;
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                //m_nSU300_ButtonIndex=46;
                m_nSU300_ButtonIndex = SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];//chpark.24.04.22
            } 
        }
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_IT)
    {
        if(!SU300_b_CAP)
        {
            if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
            {
                m_nSU300_ButtonIndex=SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];
            }
            else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=16)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-10;
            }
            else if(m_nSU300_ButtonIndex>=17 && m_nSU300_ButtonIndex<=21)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-11;
            }
            else if(m_nSU300_ButtonIndex>=22 && m_nSU300_ButtonIndex<=32)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-11;
            }
            else if(m_nSU300_ButtonIndex>=33 && m_nSU300_ButtonIndex<=39)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-10;
            }
            else if(m_nSU300_ButtonIndex==40)
            {
                m_nSU300_ButtonIndex = 31;
            }
            else if(m_nSU300_ButtonIndex==41)
            {
                m_nSU300_ButtonIndex = 33;
            }
            else if(m_nSU300_ButtonIndex>=42 && m_nSU300_ButtonIndex<=43)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-9;
            }
            else if(m_nSU300_ButtonIndex>=44 && m_nSU300_ButtonIndex<=48)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-8;
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                //m_nSU300_ButtonIndex=48;
                m_nSU300_ButtonIndex = SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];//chpark.24.04.22
            } 
        }
        else
        {
            if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
            {
                m_nSU300_ButtonIndex=SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];
            }
            else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=16)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-10;
            }
            else if(m_nSU300_ButtonIndex>=17 && m_nSU300_ButtonIndex<=21)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-11;
            }
            else if(m_nSU300_ButtonIndex>=22 && m_nSU300_ButtonIndex<=26)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-11;
            }
            else if(m_nSU300_ButtonIndex>=27 && m_nSU300_ButtonIndex<=31)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-10;
            }
            else if(m_nSU300_ButtonIndex>=32 && m_nSU300_ButtonIndex<=39)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-9;
            }
            else if(m_nSU300_ButtonIndex==40)
            {
                m_nSU300_ButtonIndex = 32;
            }
            else if(m_nSU300_ButtonIndex>=41 && m_nSU300_ButtonIndex<=42)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-9;
            }
            else if(m_nSU300_ButtonIndex>=43 && m_nSU300_ButtonIndex<=47)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-8;
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                //m_nSU300_ButtonIndex=47;
                m_nSU300_ButtonIndex = SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];//chpark.24.04.22
            } 
        }

    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_SP)
    {
        if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
        {
            m_nSU300_ButtonIndex=SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];
        }
        else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=20)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-10;
        }
        else if(m_nSU300_ButtonIndex>=21 && m_nSU300_ButtonIndex<=25)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-10;
        }
        else if(m_nSU300_ButtonIndex>=26 && m_nSU300_ButtonIndex<=31)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-11;
        }
        else if(m_nSU300_ButtonIndex>=32 && m_nSU300_ButtonIndex<=38)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-10;
        }
        else if(m_nSU300_ButtonIndex==39)
        {
            m_nSU300_ButtonIndex=30;
        }
        else if(m_nSU300_ButtonIndex==40)
        {
            m_nSU300_ButtonIndex=32;
        }
        else if(m_nSU300_ButtonIndex>=41 && m_nSU300_ButtonIndex<=42)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-9;
        }
        else if(m_nSU300_ButtonIndex>=43 && m_nSU300_ButtonIndex<=47)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex-8;
        }
        else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            //m_nSU300_ButtonIndex=47;
            m_nSU300_ButtonIndex = SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];//chpark.24.04.22
        } 
    }
    else
    {
        if(m_nSU300_ButtonIndex>MB300_END_KEY_INDEX_ETC)
        {
            m_nSU300_ButtonIndex = MB300_START_KEY_INDEX_ETC;
        }
    }

    SU300_Button_Select(m_nSU300_ButtonIndex, false);
}

void SU300_MenuTabDown()
{
    if(Lang_Flag[M7_UserIndex]==SU300_LANG_ENG)
    {
        if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=19)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+10;
        }
        else if(m_nSU300_ButtonIndex==20)
        {
            m_nSU300_ButtonIndex=29;
        } 
        else if(m_nSU300_ButtonIndex==21)
        {
            m_nSU300_ButtonIndex=30;
        }
        else if(m_nSU300_ButtonIndex>=22 && m_nSU300_ButtonIndex<=28)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+8;
        }
        else if(m_nSU300_ButtonIndex==29)
        {
            m_nSU300_ButtonIndex=36;
        }
        else if( m_nSU300_ButtonIndex>=30 && m_nSU300_ButtonIndex<=35)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+8;
        }
        else if(m_nSU300_ButtonIndex==36)
        {
            m_nSU300_ButtonIndex=43;
        }
        else if(m_nSU300_ButtonIndex>=37 && m_nSU300_ButtonIndex<=44)
        {
            m_nSU300_ButtonIndex=SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];
        }
        else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            m_nSU300_ButtonIndex=10;
        }
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_GER)
    {
        if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=22)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+11;
        }
        else if(m_nSU300_ButtonIndex==23)
        {
            m_nSU300_ButtonIndex=34;
        }
        else if(m_nSU300_ButtonIndex>=24 && m_nSU300_ButtonIndex<=31)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+10;
        }
        else if( m_nSU300_ButtonIndex>=32 && m_nSU300_ButtonIndex<=33)
        {
            m_nSU300_ButtonIndex = 41;
        }
        else if(m_nSU300_ButtonIndex>=34 && m_nSU300_ButtonIndex<=35)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+9;
        }
        else if(m_nSU300_ButtonIndex>=36 && m_nSU300_ButtonIndex<=40)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+8;
        } 
        else if(m_nSU300_ButtonIndex==41)
        {
            m_nSU300_ButtonIndex=48;
        } 
        else if(m_nSU300_ButtonIndex>=42 && m_nSU300_ButtonIndex<=49)
        {
            m_nSU300_ButtonIndex=SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];
        }
        else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            m_nSU300_ButtonIndex=11;
        }
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_FR)
    {
        if(!SU300_b_CAP)
        {
            if(m_nSU300_ButtonIndex==2 || m_nSU300_ButtonIndex==7 ||m_nSU300_ButtonIndex==9||m_nSU300_ButtonIndex==10)
            {
                m_nSU300_ButtonIndex=m_nSU300_ButtonIndex+11;
            }
            else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=21)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+11;
            }
            else if(m_nSU300_ButtonIndex>=22 && m_nSU300_ButtonIndex<=23)
            {
                m_nSU300_ButtonIndex=33;
            }
            else if(m_nSU300_ButtonIndex>=24 && m_nSU300_ButtonIndex<=30)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+9;
            }
            else if( m_nSU300_ButtonIndex>=31 && m_nSU300_ButtonIndex<=32)
            {
                m_nSU300_ButtonIndex = 39;
            }
            else if(m_nSU300_ButtonIndex>=33 && m_nSU300_ButtonIndex<=35)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+8;
            }
            else if(m_nSU300_ButtonIndex==36)
            {
                m_nSU300_ButtonIndex = 43;
            }
            else if(m_nSU300_ButtonIndex>=37 && m_nSU300_ButtonIndex<=38)
            {
                m_nSU300_ButtonIndex=m_nSU300_ButtonIndex+8;
            } 
            else if(m_nSU300_ButtonIndex==39)
            {
                m_nSU300_ButtonIndex = 46;
            }
            else if(m_nSU300_ButtonIndex>=40 && m_nSU300_ButtonIndex<=47)
            {
                m_nSU300_ButtonIndex=SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                m_nSU300_ButtonIndex=10;
            }
        }
        else
        {
            if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+10;
            }
            else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=15)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+11;
            }
            else if(m_nSU300_ButtonIndex>=16 && m_nSU300_ButtonIndex<=21)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+10;
            }
            else if(m_nSU300_ButtonIndex==22)
            {
                m_nSU300_ButtonIndex=32;
            }
            else if(m_nSU300_ButtonIndex>=23 && m_nSU300_ButtonIndex<=26)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+9;
            }
            else if(m_nSU300_ButtonIndex>=27 && m_nSU300_ButtonIndex<=30)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+8;
            }
            else if(m_nSU300_ButtonIndex==31)
            {
                m_nSU300_ButtonIndex = 38;
            }
            else if(m_nSU300_ButtonIndex>=32 && m_nSU300_ButtonIndex<=38)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+8;
            }
            else if(m_nSU300_ButtonIndex>=39 && m_nSU300_ButtonIndex<=46)
            {
                m_nSU300_ButtonIndex=SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                m_nSU300_ButtonIndex=10;
            }
        }
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_IT)
    {
        if(!SU300_b_CAP)
        {
            if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=5)
            {
                m_nSU300_ButtonIndex=m_nSU300_ButtonIndex+10;
            }
            else if(m_nSU300_ButtonIndex>=6 && m_nSU300_ButtonIndex<=10)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+11;
            }
            else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=21)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+11;
            }
            else if( m_nSU300_ButtonIndex==22)
            {
                m_nSU300_ButtonIndex = 33;
            }
            else if(m_nSU300_ButtonIndex>=23 && m_nSU300_ButtonIndex<=30)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+10;
            }
            else if( m_nSU300_ButtonIndex>=31 && m_nSU300_ButtonIndex<=32)
            {
                m_nSU300_ButtonIndex = 40;
            }
            else if( m_nSU300_ButtonIndex==33)
            {
                m_nSU300_ButtonIndex = 41;
            }
            else if(m_nSU300_ButtonIndex>=34 && m_nSU300_ButtonIndex<=35)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+9;
            }
            else if(m_nSU300_ButtonIndex>=36 && m_nSU300_ButtonIndex<=40)
            {
                m_nSU300_ButtonIndex=m_nSU300_ButtonIndex+8;
            } 
            else if(m_nSU300_ButtonIndex>=41 && m_nSU300_ButtonIndex<=48)
            {
                m_nSU300_ButtonIndex=SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                m_nSU300_ButtonIndex=10;
            }
        }
        else
        {
            if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=5)
            {
                m_nSU300_ButtonIndex=m_nSU300_ButtonIndex+10;
            }
            else if(m_nSU300_ButtonIndex>=6 && m_nSU300_ButtonIndex<=10)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+11;
            }
            else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=16)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+11;
            }
            else if(m_nSU300_ButtonIndex>=17 && m_nSU300_ButtonIndex<=21)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+10;
            }
            else if(m_nSU300_ButtonIndex==22)
            {
                m_nSU300_ButtonIndex=32;
            }
            else if(m_nSU300_ButtonIndex>=23 && m_nSU300_ButtonIndex<=30)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+9;
            }
            else if(m_nSU300_ButtonIndex==31)
            {
                m_nSU300_ButtonIndex = 39;
            }
            else if(m_nSU300_ButtonIndex==32)
            {
                m_nSU300_ButtonIndex = 40;
            }
            else if( m_nSU300_ButtonIndex>=33 && m_nSU300_ButtonIndex<=34)
            {
                 m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+9;
            }
            else if(m_nSU300_ButtonIndex>=35 && m_nSU300_ButtonIndex<=39)
            {
                m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+8;
            }
            else if(m_nSU300_ButtonIndex>=40 && m_nSU300_ButtonIndex<=47)
            {
                m_nSU300_ButtonIndex=SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                m_nSU300_ButtonIndex=10;
            }
        }
    
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_SP)
    {
        if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=15)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+10;
        }
        else if(m_nSU300_ButtonIndex>=16 && m_nSU300_ButtonIndex<=21)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+11;
        } 
        else if(m_nSU300_ButtonIndex>=22 && m_nSU300_ButtonIndex<=29)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+10;
        } 
        else if( m_nSU300_ButtonIndex>=30 && m_nSU300_ButtonIndex<=31)
        {
            m_nSU300_ButtonIndex = 39;
        }
        else if( m_nSU300_ButtonIndex==32)
        {
            m_nSU300_ButtonIndex = 40;
        }
        else if(m_nSU300_ButtonIndex>=33 && m_nSU300_ButtonIndex<=34)
        {
            m_nSU300_ButtonIndex = m_nSU300_ButtonIndex+9;
        }
        else if(m_nSU300_ButtonIndex>=35 && m_nSU300_ButtonIndex<=39)
        {
            m_nSU300_ButtonIndex= m_nSU300_ButtonIndex+8;
        }  
        else if(m_nSU300_ButtonIndex>=40 && m_nSU300_ButtonIndex<=47)
        {
            m_nSU300_ButtonIndex=SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];
        }
        else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            m_nSU300_ButtonIndex=10;
        }
    }
    else
    {
        if(m_nSU300_ButtonIndex>MB300_END_KEY_INDEX_ETC)
        {
            m_nSU300_ButtonIndex = MB300_START_KEY_INDEX_ETC;
        }
    }

    SU300_Button_Select(m_nSU300_ButtonIndex, false);
}

void SU300_MenuTabLeft()
{
    if(Lang_Flag[M7_UserIndex]==SU300_LANG_ENG)
    {
        if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
        {
            m_nSU300_ButtonIndex--;
            if(m_nSU300_ButtonIndex<1)m_nSU300_ButtonIndex=10;
        }
        else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=20)
        {
            m_nSU300_ButtonIndex--;
            if(m_nSU300_ButtonIndex<11)m_nSU300_ButtonIndex=20;
        } 
        else if(m_nSU300_ButtonIndex>=21 && m_nSU300_ButtonIndex<=29)
        {
            m_nSU300_ButtonIndex--;
            if(m_nSU300_ButtonIndex<21)m_nSU300_ButtonIndex=29;
        }
        else if(m_nSU300_ButtonIndex>=30 && m_nSU300_ButtonIndex<=36)
        {
            m_nSU300_ButtonIndex--;
            if(m_nSU300_ButtonIndex<30)m_nSU300_ButtonIndex=36;
        }
        else if( m_nSU300_ButtonIndex>=37 && m_nSU300_ButtonIndex<=44)
        {
            m_nSU300_ButtonIndex--;
            if(m_nSU300_ButtonIndex<37)m_nSU300_ButtonIndex=44;
        }
        else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            //m_nSU300_ButtonIndex=44;
            m_nSU300_ButtonIndex = SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];//chpark.24.04.22
        }

    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_GER)
    {
        if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=11)
        {
            m_nSU300_ButtonIndex--;
            if(m_nSU300_ButtonIndex<1)m_nSU300_ButtonIndex=11;
        }
        else if(m_nSU300_ButtonIndex>=12 && m_nSU300_ButtonIndex<=22)
        {
            m_nSU300_ButtonIndex--;
            if(m_nSU300_ButtonIndex<12)m_nSU300_ButtonIndex=22;
        } 
        else if(m_nSU300_ButtonIndex>=23 && m_nSU300_ButtonIndex<=33)
        {
            m_nSU300_ButtonIndex--;
            if(m_nSU300_ButtonIndex<23)m_nSU300_ButtonIndex=33;
        }
        else if(m_nSU300_ButtonIndex>=34 && m_nSU300_ButtonIndex<=41)
        {
            m_nSU300_ButtonIndex--;
            if(m_nSU300_ButtonIndex<34)m_nSU300_ButtonIndex=41;
        }
        else if( m_nSU300_ButtonIndex>=42 && m_nSU300_ButtonIndex<=49)
        {
            m_nSU300_ButtonIndex--;
            if(m_nSU300_ButtonIndex<42)m_nSU300_ButtonIndex=49;
        }
        else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            //m_nSU300_ButtonIndex=47;
            m_nSU300_ButtonIndex = SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];//chpark.24.04.22
        }

    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_FR)
    {
        if(!SU300_b_CAP)
        {
            if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
            {
                m_nSU300_ButtonIndex--;
                //chpark.11.20
                if(m_nSU300_ButtonIndex<=1)m_nSU300_ButtonIndex=10;
                else if(m_nSU300_ButtonIndex>=3 && m_nSU300_ButtonIndex<=6)m_nSU300_ButtonIndex=2;
                else if(m_nSU300_ButtonIndex==8)m_nSU300_ButtonIndex=7;
            }
            else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=21)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<11)m_nSU300_ButtonIndex=21;
            } 
            else if(m_nSU300_ButtonIndex>=22 && m_nSU300_ButtonIndex<=32)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<22)m_nSU300_ButtonIndex=32;
            }
            else if(m_nSU300_ButtonIndex>=33 && m_nSU300_ButtonIndex<=39)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<33)m_nSU300_ButtonIndex=39;
            }
            else if( m_nSU300_ButtonIndex>=40 && m_nSU300_ButtonIndex<=47)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<40)m_nSU300_ButtonIndex=47;
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                //m_nSU300_ButtonIndex=47;
                m_nSU300_ButtonIndex = SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];//chpark.24.04.22
            }
        }
        else
        {
            if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<1)m_nSU300_ButtonIndex=10;
            }
            else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=21)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<11)m_nSU300_ButtonIndex=21;
            } 
            else if(m_nSU300_ButtonIndex>=22 && m_nSU300_ButtonIndex<=31)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<22)m_nSU300_ButtonIndex=31;
            }
            else if(m_nSU300_ButtonIndex>=32 && m_nSU300_ButtonIndex<=38)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<32)m_nSU300_ButtonIndex=38;
            }
            else if( m_nSU300_ButtonIndex>=39 && m_nSU300_ButtonIndex<=46)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<39)m_nSU300_ButtonIndex=46;
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                //m_nSU300_ButtonIndex=46;
                m_nSU300_ButtonIndex = SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];//chpark.24.04.22
            }
        }

    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_IT)
    {
        if(!SU300_b_CAP)
        {
            if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<1)m_nSU300_ButtonIndex=10;
            }
            else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=21)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<11)m_nSU300_ButtonIndex=21;
            } 
            else if(m_nSU300_ButtonIndex>=22 && m_nSU300_ButtonIndex<=32)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<22)m_nSU300_ButtonIndex=32;
            }
            else if(m_nSU300_ButtonIndex>=33 && m_nSU300_ButtonIndex<=40)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<33)m_nSU300_ButtonIndex=40;
            }
            else if( m_nSU300_ButtonIndex>=41 && m_nSU300_ButtonIndex<=48)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<41)m_nSU300_ButtonIndex=48;
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                //m_nSU300_ButtonIndex=48;
                m_nSU300_ButtonIndex = SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];//chpark.24.04.22
            }
        }
        else
        {
            if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<1)m_nSU300_ButtonIndex=10;
            }
            else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=21)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<11)m_nSU300_ButtonIndex=21;
            } 
            else if(m_nSU300_ButtonIndex>=22 && m_nSU300_ButtonIndex<=31)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<22)m_nSU300_ButtonIndex=31;
            }
            else if(m_nSU300_ButtonIndex>=32 && m_nSU300_ButtonIndex<=39)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<32)m_nSU300_ButtonIndex=39;
            }
            else if( m_nSU300_ButtonIndex>=40 && m_nSU300_ButtonIndex<=47)
            {
                m_nSU300_ButtonIndex--;
                if(m_nSU300_ButtonIndex<40)m_nSU300_ButtonIndex=47;
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                //m_nSU300_ButtonIndex=47;
                m_nSU300_ButtonIndex = SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];//chpark.24.04.22
            }
        }

    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_SP)
    {
        if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
        {
            m_nSU300_ButtonIndex--;
            if(m_nSU300_ButtonIndex<1)m_nSU300_ButtonIndex=10;
        }
        else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=20)
        {
            m_nSU300_ButtonIndex--;
            if(m_nSU300_ButtonIndex<11)m_nSU300_ButtonIndex=20;
        } 
        else if(m_nSU300_ButtonIndex>=21 && m_nSU300_ButtonIndex<=31)
        {
            m_nSU300_ButtonIndex--;
            if(m_nSU300_ButtonIndex<21)m_nSU300_ButtonIndex=31;
        }
        else if(m_nSU300_ButtonIndex>=32 && m_nSU300_ButtonIndex<=39)
        {
            m_nSU300_ButtonIndex--;
            if(m_nSU300_ButtonIndex<32)m_nSU300_ButtonIndex=39;
        }
        else if( m_nSU300_ButtonIndex>=40 && m_nSU300_ButtonIndex<=47)
        {
            m_nSU300_ButtonIndex--;
            if(m_nSU300_ButtonIndex<40)m_nSU300_ButtonIndex=47;
        }
        else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            //m_nSU300_ButtonIndex=47;
            m_nSU300_ButtonIndex = SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP];//chpark.24.04.22
        }

    }
    else
    {
        if(m_nSU300_ButtonIndex>MB300_END_KEY_INDEX_ETC)
        {
            m_nSU300_ButtonIndex = MB300_START_KEY_INDEX_ETC;
        }
    }

    SU300_Button_Select(m_nSU300_ButtonIndex, false);
}

void SU300_MenuTabRight()
{
    if(Lang_Flag[M7_UserIndex]==SU300_LANG_ENG)
    {
        if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
        {
            m_nSU300_ButtonIndex++;
            if(m_nSU300_ButtonIndex>10)m_nSU300_ButtonIndex=1;
        }
        else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=20)
        {
            m_nSU300_ButtonIndex++;
            if(m_nSU300_ButtonIndex>20)m_nSU300_ButtonIndex=11;
        } 
        else if(m_nSU300_ButtonIndex>=21 && m_nSU300_ButtonIndex<=29)
        {
            m_nSU300_ButtonIndex++;
            if(m_nSU300_ButtonIndex>29)m_nSU300_ButtonIndex=29;
        }
        else if(m_nSU300_ButtonIndex>=30 && m_nSU300_ButtonIndex<=36)
        {
            m_nSU300_ButtonIndex++;
            if(m_nSU300_ButtonIndex>36)m_nSU300_ButtonIndex=30;
        }
        else if( m_nSU300_ButtonIndex>=37 && m_nSU300_ButtonIndex<=44)
        {
            m_nSU300_ButtonIndex++;
            if(m_nSU300_ButtonIndex>44)m_nSU300_ButtonIndex=37;
        }
        else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            m_nSU300_ButtonIndex=1;
        }
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_GER)
    {
        if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=11)
        {
            m_nSU300_ButtonIndex++;
            if(m_nSU300_ButtonIndex>11)m_nSU300_ButtonIndex=1;
        }
        else if(m_nSU300_ButtonIndex>=12 && m_nSU300_ButtonIndex<=22)
        {
            m_nSU300_ButtonIndex++;
            if(m_nSU300_ButtonIndex>22)m_nSU300_ButtonIndex=12;
        } 
        else if(m_nSU300_ButtonIndex>=23 && m_nSU300_ButtonIndex<=33)
        {
            m_nSU300_ButtonIndex++;
            if(m_nSU300_ButtonIndex>33)m_nSU300_ButtonIndex=23;
        }
        else if(m_nSU300_ButtonIndex>=34 && m_nSU300_ButtonIndex<=41)
        {
            m_nSU300_ButtonIndex++;
            if(m_nSU300_ButtonIndex>41)m_nSU300_ButtonIndex=34;
        }
        else if( m_nSU300_ButtonIndex>=42 && m_nSU300_ButtonIndex<=49)
        {
            m_nSU300_ButtonIndex++;
            if(m_nSU300_ButtonIndex>49)m_nSU300_ButtonIndex=42;
        }
        else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            m_nSU300_ButtonIndex=1;
        }
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_FR)
    {
        if(!SU300_b_CAP)
        {
            if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
            {
                m_nSU300_ButtonIndex++;
                //chpark.11.20
                if(m_nSU300_ButtonIndex>10)m_nSU300_ButtonIndex=2;
                else if(m_nSU300_ButtonIndex>=3 && m_nSU300_ButtonIndex<=6)m_nSU300_ButtonIndex=7;
                else if(m_nSU300_ButtonIndex==8)m_nSU300_ButtonIndex=9;
            }
            else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=21)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>21)m_nSU300_ButtonIndex=11;
            } 
            else if(m_nSU300_ButtonIndex>=22 && m_nSU300_ButtonIndex<=32)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>32)m_nSU300_ButtonIndex=22;
            }
            else if(m_nSU300_ButtonIndex>=33 && m_nSU300_ButtonIndex<=39)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>39)m_nSU300_ButtonIndex=33;
            }
            else if( m_nSU300_ButtonIndex>=40 && m_nSU300_ButtonIndex<=47)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>47)m_nSU300_ButtonIndex=40;
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                m_nSU300_ButtonIndex=2;
            }
        }
        else
        {
            if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>10)m_nSU300_ButtonIndex=1;
            }
            else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=21)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>21)m_nSU300_ButtonIndex=11;
            } 
            else if(m_nSU300_ButtonIndex>=22 && m_nSU300_ButtonIndex<=31)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>31)m_nSU300_ButtonIndex=22;
            }
            else if(m_nSU300_ButtonIndex>=32 && m_nSU300_ButtonIndex<=38)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>38)m_nSU300_ButtonIndex=32;
            }
            else if( m_nSU300_ButtonIndex>=39 && m_nSU300_ButtonIndex<=46)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>46)m_nSU300_ButtonIndex=39;
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                m_nSU300_ButtonIndex=1;
            }
        }
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_IT)
    {
        if(!SU300_b_CAP)
        {
            if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>10)m_nSU300_ButtonIndex=1;
            }
            else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=21)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>21)m_nSU300_ButtonIndex=11;
            } 
            else if(m_nSU300_ButtonIndex>=22 && m_nSU300_ButtonIndex<=32)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>32)m_nSU300_ButtonIndex=22;
            }
            else if(m_nSU300_ButtonIndex>=33 && m_nSU300_ButtonIndex<=40)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>40)m_nSU300_ButtonIndex=33;
            }
            else if( m_nSU300_ButtonIndex>=41 && m_nSU300_ButtonIndex<=48)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>48)m_nSU300_ButtonIndex=41;
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                m_nSU300_ButtonIndex=1;
            }
        }
        else
        {
            if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>10)m_nSU300_ButtonIndex=1;
            }
            else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=21)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>21)m_nSU300_ButtonIndex=11;
            } 
            else if(m_nSU300_ButtonIndex>=22 && m_nSU300_ButtonIndex<=31)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>31)m_nSU300_ButtonIndex=22;
            }
            else if(m_nSU300_ButtonIndex>=32 && m_nSU300_ButtonIndex<=39)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>39)m_nSU300_ButtonIndex=32;
            }
            else if( m_nSU300_ButtonIndex>=40 && m_nSU300_ButtonIndex<=47)
            {
                m_nSU300_ButtonIndex++;
                if(m_nSU300_ButtonIndex>47)m_nSU300_ButtonIndex=40;
            }
            else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
            {
                m_nSU300_ButtonIndex=1;
            }
        }
    }
    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_SP)
    {
        if(m_nSU300_ButtonIndex>=1 && m_nSU300_ButtonIndex<=10)
        {
            m_nSU300_ButtonIndex++;
            if(m_nSU300_ButtonIndex>10)m_nSU300_ButtonIndex=1;
        }
        else if(m_nSU300_ButtonIndex>=11 && m_nSU300_ButtonIndex<=20)
        {
            m_nSU300_ButtonIndex++;
            if(m_nSU300_ButtonIndex>20)m_nSU300_ButtonIndex=11;
        } 
        else if(m_nSU300_ButtonIndex>=21 && m_nSU300_ButtonIndex<=31)
        {
            m_nSU300_ButtonIndex++;
            if(m_nSU300_ButtonIndex>31)m_nSU300_ButtonIndex=21;
        }
        else if(m_nSU300_ButtonIndex>=32 && m_nSU300_ButtonIndex<=39)
        {
            m_nSU300_ButtonIndex++;
            if(m_nSU300_ButtonIndex>39)m_nSU300_ButtonIndex=32;
        }
        else if( m_nSU300_ButtonIndex>=40 && m_nSU300_ButtonIndex<=47)
        {
            m_nSU300_ButtonIndex++;
            if(m_nSU300_ButtonIndex>47)m_nSU300_ButtonIndex=40;
        }
        else if(m_nSU300_ButtonIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
        {
            m_nSU300_ButtonIndex=1;
        }
    }
    else
    {
        if(m_nSU300_ButtonIndex>MB300_END_KEY_INDEX_ETC)
        {
            m_nSU300_ButtonIndex = MB300_START_KEY_INDEX_ETC;
        }
    }

    SU300_Button_Select(m_nSU300_ButtonIndex, false);
}


void SU300_MenuTabESC()
{
    if(g_nSU100_Mode == SU100_Add_Mode)
    {
        if(g_nSU100_PreLayer==M7_SU200_EDIT_LAYER)
        {
            ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
            assert(M7_SU200_EDIT);
            ituLayerGoto(M7_SU200_EDIT);
        }
        else
        {
            ITULayer *M7_SU100 = ituSceneFindWidget(&theScene, "M7_SU100");
            assert(M7_SU100);
            ituLayerGoto(M7_SU100);
        }
    }
    else if(g_nSU100_Mode == SU100_Edit_Mode)
    {
         ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
        assert(M7_SU200_EDIT);
        ituLayerGoto(M7_SU200_EDIT);
    }
}

void SU300_MenuTabD7()
{
    if(m_nSU300_Mode == SU300_MODE_START)
    {
        //chpark.24.05.02
        //char num check. 1~30
        int len=0;
        len = strlen(SU300_TxtUserName);
        if(len<1 || len>MAX_SU100_NAME_CNT)
        {
            printf("Input char num Error==%d\n",len);
            return;
        }

        SU300_Chk_Result();
    }
    else if(m_nSU300_Mode == SU300_MODE_RESULT_FAIL)
    {
        m_nSU300_Mode = SU300_MODE_START;
        //Error Message
        ituWidgetSetVisible(SU300_CONT_ERROR_TXT, false);
        ituWidgetSetVisible(SU300_CONT_ERROR_1_TXT,  false);//chpark.24.02.07
        //V Icon    
        ituWidgetSetVisible(SU300_CONT_RESULT, false);
    }
}


//=====================================================
bool SU300_JogWheel(ITUWidget* widget, char* param)
{
    if(!m_bSU300ReadyToJOG || m_bSU300_GoTo_LayerMode) return false;

    if(m_nSU300_Mode == SU300_MODE_RESULT_FAIL || m_nSU300_Mode == SU300_MODE_RESULT_SUCCESS)
        return false;

    SU300_ExitCNT = 0;
	uint8_t JogDir = 0;
	JogDir = param[0];
	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
        case LANDSCAPE_1:
		case LANDSCAPE_2:
			printf("PORTRATE\r\n");

            if(JogDir == JOG_DIAL_ROT_RIGHT)
			{
                printf("SU300 JogWheel[R] %d\r\n",JogDir);
                SU300_MenuTabNext();
			}
			else if(JogDir == JOG_DIAL_ROT_LEFT)
			{
                printf("SU300 JogWheel[L] %d\r\n",JogDir);
                SU300_MenuTabPrev();
			}

		break;
	}

    return true;
}

bool SU300_JogButton(ITUWidget* widget, char* param)
{
    SU300_ExitCNT = 0;
    if(!m_bSU300ReadyToJOG|| m_bSU300_GoTo_LayerMode) return false;
    
    if(m_nSU300_Mode == SU300_MODE_RESULT_FAIL || m_nSU300_Mode == SU300_MODE_RESULT_SUCCESS)
        return false;

    uint8_t JogButton = 0;

	JogButton = param[0];

	printf("SU300_JogButton %d\r\n",JogButton);

    int nTmpKeyValue;

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
                    SU300_MenuTabUp();
					break;

				case JOG_DIAL_KEY_DOWN:
                    SU300_MenuTabDown();
					break;
				
				case JOG_DIAL_KEY_LEFT:
                    SU300_MenuTabLeft();
                    break;

				case JOG_DIAL_KEY_RIGHT:
					SU300_MenuTabRight();
					break;

				case JOG_DIAL_KEY_ENTER_ON:
                    //chpark.12.19
        
                    if(Lang_Flag[M7_UserIndex]==SU300_LANG_ENG)
                    { 
                        nTmpKeyValue = ENG_KEY[SU300_b_CAP][m_nSU300_ButtonIndex];
                    }
                    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_GER)
                    { 
                        nTmpKeyValue = GER_KEY[SU300_b_CAP][m_nSU300_ButtonIndex];
                    }
                    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_FR)
                    { 
                        nTmpKeyValue = FR_KEY[SU300_b_CAP][m_nSU300_ButtonIndex];
                    }
                    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_IT)
                    { 
                        nTmpKeyValue = IT_KEY[SU300_b_CAP][m_nSU300_ButtonIndex];
                    }
                    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_SP)
                    { 
                        nTmpKeyValue = SP_KEY[SU300_b_CAP][m_nSU300_ButtonIndex];
                    }

                    char TMP_PARA[10];
                    
                    memset(TMP_PARA,0,10);
                    sprintf(TMP_PARA,"%d",nTmpKeyValue);

                    SU300_KeyDn(NULL, TMP_PARA);

					break;

				case JOG_DIAL_KEY_ENTER_OFF:				//c3Byte0bit
        		case JOG_DIAL_KEY_NONE:
				{
                    //==========================
                    if(Lang_Flag[M7_UserIndex]==SU300_LANG_ENG)
                    { 
                        nTmpKeyValue = ENG_KEY[SU300_b_CAP][m_nSU300_ButtonIndex];
                    }
                    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_GER)
                    { 
                        nTmpKeyValue = GER_KEY[SU300_b_CAP][m_nSU300_ButtonIndex];
                    }
                    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_FR)
                    { 
                        nTmpKeyValue = FR_KEY[SU300_b_CAP][m_nSU300_ButtonIndex];
                    }
                    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_IT)
                    { 
                        nTmpKeyValue = IT_KEY[SU300_b_CAP][m_nSU300_ButtonIndex];
                    }
                    else if(Lang_Flag[M7_UserIndex]==SU300_LANG_SP)
                    { 
                        nTmpKeyValue = SP_KEY[SU300_b_CAP][m_nSU300_ButtonIndex];
                    }

                    char TMP_PARA[10];
                    
                    memset(TMP_PARA,0,10);
                    sprintf(TMP_PARA,"%d",nTmpKeyValue);

                    SU300_KeyUp(NULL, TMP_PARA);
                    //==========================
        		}

				break;
			}
		break;
	}

    return true;
}



bool SU300_JogSwitch(ITUWidget* widget, char* param)
{
    if(!m_bSU300ReadyToJOG || m_bSU300_GoTo_LayerMode) return false;

    SU300_ExitCNT = 0;
	uint8_t JogSwitch = 0;
	JogSwitch = param[0];

	if(JogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
        SU300_MenuTabESC();
	}
	else if(JogSwitch == JOG_DIAL_SW_HOME_OFF)
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}

    return true;
}
//=====================================================
bool SU300_KeyDn(ITUWidget* widget, char* param)
{
    if(m_nSU300_Mode == SU300_MODE_RESULT_FAIL || m_nSU300_Mode == SU300_MODE_RESULT_SUCCESS)
        return false;

    if(!m_bSU300_ReadyTo_Input)
        return false;    

    SU300_ExitCNT = 0;
    int nKeyParam,nASC,nKeyIndex;

	nKeyParam = atoi(param);

    nASC = nKeyParam/100;
    nKeyIndex = nKeyParam%100;

    //chpark.12.19
    if(nASC<=0)
    {
        printf("SU300_KeyDn nKeyParam=%d [not assigned key]\n",nKeyParam);
        return false;
    }
   
    //printf("SU300_KeyDn nKeyParam=%d\n",nKeyParam);

    //key select ui
    SU300_Button_Select(nKeyIndex,true);

    //chpark.12.19
    if(nKeyIndex==SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
    {
        SU300_1SecCNT  = 0;
	    SU300_FastFlag = SU300_DEL_AUTO_START;
    }

    return true;
}


bool SU300_KeyUp(ITUWidget* widget, char* param)
{
    if(m_nSU300_Mode == SU300_MODE_RESULT_FAIL || m_nSU300_Mode == SU300_MODE_RESULT_SUCCESS)
        return false;

    if(!m_bSU300_ReadyTo_Input)
        return false;    

    SU300_1SecCNT  = 100;
    SU300_FastFlag = SU300_DEL_AUTO_OFF;


    if(m_nSU300_Mode == SU300_MODE_RESULT_FAIL)
    { 
        printf("m_nSU300_Mode==SU300_MODE_RESULT_FAIL  m_nSU300_Mode=%d\n",m_nSU300_Mode);
        m_nSU300_Mode = SU300_MODE_START;
        //Error Message
        ituWidgetSetVisible(SU300_CONT_ERROR_TXT, false);
        ituWidgetSetVisible(SU300_CONT_ERROR_1_TXT,  false);//chpark.24.02.07
        //V Icon    
        ituWidgetSetVisible(SU300_CONT_RESULT, false);

        return false;
    }

    SU300_ExitCNT = 0;
    int nKeyParam,nKeyIndex;
    uint8_t nASC;
    nKeyParam = atoi(param);

    nASC = (uint8_t)(nKeyParam/100);
    nKeyIndex = nKeyParam%100;

    //printf("SU300_KeyUp nKeyParam=%d nASC=%d nKeyIndex=%d m_nSU300_Mode=%d\n",nKeyParam,nASC,nKeyIndex,m_nSU300_Mode);
    
    //key up ui
    ituWidgetSetVisible(SU300_ICON_DOWN_A[Lang_Flag[M7_UserIndex]], false);
    ituWidgetSetVisible(SU300_ICON_DOWN_B[Lang_Flag[M7_UserIndex]], false);
    ituWidgetSetVisible(SU300_ICON_DOWN_C[Lang_Flag[M7_UserIndex]], false);
    
    //==========================================
    //key input
    if(nKeyIndex==SU300_CAP_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])//small<->capital
    {
        SU300_b_CAP = !SU300_b_CAP;

        if(!SU300_b_CAP)
        {
            ituWidgetSetVisible(SU300_CONT_KEY_TYPE[Lang_Flag[M7_UserIndex]][SU300_KEY_S], true);
            ituWidgetSetVisible(SU300_CONT_KEY_TYPE[Lang_Flag[M7_UserIndex]][SU300_KEY_C], false);
        }
        else
        {
            ituWidgetSetVisible(SU300_CONT_KEY_TYPE[Lang_Flag[M7_UserIndex]][SU300_KEY_S], false);
            ituWidgetSetVisible(SU300_CONT_KEY_TYPE[Lang_Flag[M7_UserIndex]][SU300_KEY_C], true);
        }
        
        SU300_InitKeyMaping();

        m_nSU300_ButtonIndex = SU300_CAP_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP]; //reset to CAPs key index
        SU300_Button_Select(m_nSU300_ButtonIndex,false);//chpark.11.02
    }
    else if(nKeyIndex==SU300_D7_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])
    {
        SU300_MenuTabD7();
    }
    else if(nKeyIndex==SU300_DEL_KEY[Lang_Flag[M7_UserIndex]][SU300_b_CAP])//Del Key
    {
        SU300_Delete_Char(0);   //delete char
    }
    else
    {
        //check max char of normal key
        int len;
        len = strlen(SU300_TxtUserName);

        if(len>MAX_SU100_NAME_CNT)//chpark.12.19
        {
            printf("Cannot insert char.[Error:Max Char] len:%d\n",len);
            return false;
        }

        SU300_TxtUserName[len] = (char)nASC;//ascii char input
        SU300_TxtUserName[len+1] = '\0';//ascii char input
        
        //printf("\n len=%d input=%s \n\n",len,SU300_TxtUserName);

        SU300_Check_SP_Char();//check special combination of character

        //recheck max char of sp char.
        len = strlen(SU300_TxtUserName);

        if(len>MAX_SU100_NAME_CNT)//chpark.12.19
        {
            printf("Cannot insert char.[Error:Max Char] len:%d\n",len);
        }

        for(int i=MAX_SU100_NAME_CNT;i<MAX_SU100_USER_NAME;i++)
        {
            SU300_TxtUserName[i]  = '\0';//ascii char input
        }

        //printf("\n len=%d input=%s \n\n",len,SU300_TxtUserName);
    }

    SU300_Display_UserName();
    //=========================================

    return true;
}

 bool SU300_Delete_Char(int nIndex)
 {
    bool bRet = false;

        int len;
        len = strlen(SU300_TxtUserName);

        if( (len-1)<0)
        {
            printf("Cannot delete char.[Error====] len:%d\n",len-1);
            return bRet;
        }

        SU300_TxtUserName[len-1] = '\0';//ascii char input
        
        bRet = true;
    
    return bRet;
 }

//=====================================================
void SU300_Chk_Result()
{
    bool bCheck_Suc = true;
    int nSelfIndex;
    if(g_nSU100_Mode==SU100_Add_Mode)
    {
        nSelfIndex = g_nSU100_AddUserIdx;
    }
    else if(g_nSU100_Mode==SU100_Edit_Mode)
    {
        nSelfIndex = g_nSU100_EditUserIdx;
    }
    
    //duplicate name check. 
    //exclude self index.
    int nResult = 0;
    for(int i=0;i<MAX_SU100_USER_NUM;i++)
    {
        //self name check. exclude self index.
        if(i == nSelfIndex)
        {
            printf("[SU300_Chk_Result] Self Name: Index=[%d] Name=%s\n",nSelfIndex,User_RegInfo[i].UserName);
            continue;
        }

        nResult = strcmp(SU300_TxtUserName,User_RegInfo[i].UserName);
        if(nResult==0)
        {
            printf("[SU300_Chk_Result] CurName:%s  is Duplicate Name:[%d User]=%s \n",SU300_TxtUserName,i,User_RegInfo[i].UserName);
            bCheck_Suc = false;
            break;
        }
        
    }

    if(bCheck_Suc)
    {
        memcpy(&User_RegInfo[nSelfIndex].UserName[0],SU300_TxtUserName, MAX_SU100_NAME_CNT);  

        m_nSU300_Mode = SU300_MODE_RESULT_SUCCESS;

        //Error Message
        ituWidgetSetVisible(SU300_CONT_ERROR_TXT, false);
        ituWidgetSetVisible(SU300_CONT_ERROR_1_TXT, false);//chpark.24.02.07
        //V Icon    
        ituWidgetSetVisible(SU300_CONT_RESULT, true);
        ituWidgetSetVisible(SU300_RESULT_ICON[SU300_SUCCESS], true);
        ituWidgetSetVisible(SU300_RESULT_ICON[SU300_FAIL], false);
        //move to passcode setting

        m_bSU300_GoTo_LayerMode = true;

        if(g_nSU100_Mode==SU100_Edit_Mode)
        {
            #ifdef USED_SAVE_CONFIG		 
				XO_Save_Config(nSelfIndex,true,0);//chpark.24.04.03
			#else
				HT_Save_Config(1);
			#endif
        }
    }
    else
    {
        //delete current 
        //after 2second, delete Error Msg.
        m_nSU300_FailMsg_Cnt=0;//chpark.11.02
       
        memset(SU300_TxtUserName, 0 , sizeof(SU300_TxtUserName));
        SU300_Display_UserName();

        m_nSU300_Mode = SU300_MODE_RESULT_FAIL;

        //Error Message
        ituWidgetSetVisible(SU300_CONT_ERROR_TXT, true);
        for(int i=0;i<5;i++)
        {
            ituWidgetSetVisible(SU300_ERROR_TXT[i], false);
        }
        ituWidgetSetVisible(SU300_ERROR_TXT[Lang_Flag[M7_UserIndex]], true);

        ituWidgetSetVisible(SU300_CONT_ERROR_1_TXT, false);//chpark.24.02.07
        //ituWidgetSetVisible(SU300_ERROR_TEXT,      false);//chpark.24.02.07
        //ituTextSetString(SU300_ERROR_TEXT, g_SU300_MSG_TEXT_ERROR_EXIST[Lang_Flag[M7_UserIndex]]);

        //X Icon.    
        ituWidgetSetVisible(SU300_CONT_RESULT, true);
        ituWidgetSetVisible(SU300_RESULT_ICON[SU300_SUCCESS], false);
        ituWidgetSetVisible(SU300_RESULT_ICON[SU300_FAIL], true);
    }
     
}

bool SU300_D7_MouseUp(ITUWidget* widget, char* param)
{
    SU300_ExitCNT = 0;
    SU300_MenuTabD7();

    return true;
}

bool SU300_D6_MouseUp(ITUWidget* widget, char* param)
{
    SU300_ExitCNT = 0;
    SU100_Send_Add_Cancel();//chpark.10.30
	GotoMain(MAIN_MODE_NORMAL); 

    return true;
}


bool SU300_D5_MouseUp(ITUWidget* widget, char* param)
{
    //SU100_Send_Add_Cancel();//chpark.12.19
    SU300_MenuTabESC();

    return true;
}


void SU300_Check_SP_Char()
{
    int len;
    len = strlen(SU300_TxtUserName);

    if(len<2)return;
    
    uint8_t nFisrt_Char     =  SU300_TxtUserName[len-2];
    uint8_t nSecond_Char    =  SU300_TxtUserName[len-1];
    uint8_t nFinal_Char     =  0;

    if(Lang_Flag[M7_UserIndex]!=SU300_LANG_ENG)
    {
        //`:96   ´:180
        //à:224  á:225
        //è:232  é:233
        //ì:236  í:237
        //ò:232  ó:243
        //ù:249  ú:250 
       //¨+a,( ¨+e,¨+i,)¨+o,¨+u => ? 
       if(nFisrt_Char==168 && nSecond_Char==97)nFinal_Char=228;//?
       //else if(nFisrt_Char==168 && nSecond_Char==101)nFinal_Char=235;//?
       //else if(nFisrt_Char==168 && nSecond_Char==105)nFinal_Char=239;//?
       else if(nFisrt_Char==168 && nSecond_Char==111)nFinal_Char=246;//?
       else if(nFisrt_Char==168 && nSecond_Char==117)nFinal_Char=252;//?

       //^+a, ^+e,^+i,^+o,^+u => ?
       if(nFisrt_Char==94 && nSecond_Char==97)nFinal_Char=226;//?
       else if(nFisrt_Char==94 && nSecond_Char==101)nFinal_Char=234;//?
       else if(nFisrt_Char==94 && nSecond_Char==105)nFinal_Char=238;//?
       else if(nFisrt_Char==94 && nSecond_Char==111)nFinal_Char=244;//?
       else if(nFisrt_Char==94 && nSecond_Char==117)nFinal_Char=251;//?

       //´+a,´+e,´+i,´+o,´+u => , '+y?
       if(nFisrt_Char==180 && nSecond_Char==97)nFinal_Char=225;//á
       else if(nFisrt_Char==180 && nSecond_Char==101)nFinal_Char=233;//é
       else if(nFisrt_Char==180 && nSecond_Char==105)nFinal_Char=237;//í
       else if(nFisrt_Char==180 && nSecond_Char==111)nFinal_Char=243;//ó
       else if(nFisrt_Char==180 && nSecond_Char==117)nFinal_Char=250;//ú
       else if(nFisrt_Char==180 && nSecond_Char==121)nFinal_Char=253;//ý

       //`+a,`+e,`+i,`+o,`+u => ?
       if(nFisrt_Char==96 && nSecond_Char==97)nFinal_Char=224;//à
       else if(nFisrt_Char==96 && nSecond_Char==101)nFinal_Char=232;//è
       else if(nFisrt_Char==96 && nSecond_Char==105)nFinal_Char=236;//ì
       else if(nFisrt_Char==96 && nSecond_Char==111)nFinal_Char=242;//ò
       else if(nFisrt_Char==96 && nSecond_Char==117)nFinal_Char=249;//ù

       //¨+A, (¨+E,¨+I,)¨+O,¨+U => ? 
       if(nFisrt_Char==168 && nSecond_Char==65)nFinal_Char=196;//?
       //else if(nFisrt_Char==168 && nSecond_Char==69)nFinal_Char=203;//?
       //else if(nFisrt_Char==168 && nSecond_Char==73)nFinal_Char=207;//?
       else if(nFisrt_Char==168 && nSecond_Char==79)nFinal_Char=214;//?
       else if(nFisrt_Char==168 && nSecond_Char==85)nFinal_Char=220;//?

       //^+A, ^+E,^+I,^+O,^+U => î
       if(nFisrt_Char==94 && nSecond_Char==65)nFinal_Char=194;//?
       else if(nFisrt_Char==94 && nSecond_Char==69)nFinal_Char=202;//?
       else if(nFisrt_Char==94 && nSecond_Char==73)nFinal_Char=206;//?
       else if(nFisrt_Char==94 && nSecond_Char==79)nFinal_Char=212;//?
       else if(nFisrt_Char==94 && nSecond_Char==85)nFinal_Char=219;//?

       //´+A,´+E,´+I,´+O,´+U,´+Y => ?
       if(nFisrt_Char==180 && nSecond_Char==65)nFinal_Char=193;//Á
       else if(nFisrt_Char==180 && nSecond_Char==69)nFinal_Char=201;//É
       else if(nFisrt_Char==180 && nSecond_Char==73)nFinal_Char=205;//Í
       else if(nFisrt_Char==180 && nSecond_Char==79)nFinal_Char=211;//Ó
       else if(nFisrt_Char==180 && nSecond_Char==85)nFinal_Char=218;//Ú
       else if(nFisrt_Char==180 && nSecond_Char==89)nFinal_Char=221;//ý

       //`+A,`+E,`+I,`+O,`+U => ?
       if(nFisrt_Char==96 && nSecond_Char==65)nFinal_Char=192;//??
       else if(nFisrt_Char==96 && nSecond_Char==69)nFinal_Char=200;//È
       else if(nFisrt_Char==96 && nSecond_Char==73)nFinal_Char=204;//Ì
       else if(nFisrt_Char==96 && nSecond_Char==79)nFinal_Char=210;//Ò
       else if(nFisrt_Char==96 && nSecond_Char==85)nFinal_Char=217;//Ù
    }
    
    if(nFinal_Char!=0)
    {
       SU300_TxtUserName[len-2] = nFinal_Char;
       SU300_TxtUserName[len-1] = '\0';//ascii char input
       //printf("\n[SPChar] len=%d input=%s \n\n",len,SU300_TxtUserName);
    }
}



bool SU300_Timer(ITUWidget* widget, char* param)
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
        if(!m_bSU300_AUI1_Rcv_Error && !m_bSU300_GoTo_LayerMode && gUI_Server_Mode)
		{
            printf("\r\n AUI1 Protocol 5Sec Exit >>>>>>> goto Main\r\n");

            ituWidgetSetVisible(SU300_CONT_RESULT, true);
            ituWidgetSetVisible(SU300_RESULT_ICON[SU300_SUCCESS], false);
            ituWidgetSetVisible(SU300_RESULT_ICON[SU300_FAIL], true);

            m_bSU300_AUI1_Rcv_Error = true;
            m_bSU300_GoTo_LayerMode = true;
        }
	}
    #endif

    
    if((gMainTimerCnt_10m - SU300_SecTimer) >= 20)//100)//200msec
	{
		if(SU300_ExitCNT < 30*5)//30sec
			SU300_ExitCNT++;
		else if(SU300_ExitCNT == 30*5)//30sec
		{
			SU300_ExitCNT = 300;
            #ifdef RELEASE_VER
                SU100_Send_Add_Cancel();//chpark.10.30
                printf("\r\n SU300 Menu 30Sec Exit > goto Main\r\n");
			    GotoMain(MAIN_MODE_NORMAL); 
                return true;
	        #endif
			
		}

        //chpark.12.19
        // long delete check
		if(SU300_1SecCNT < 1*5)
			SU300_1SecCNT++;
		if(SU300_1SecCNT == 1*5)
		{
			SU300_1SecCNT = 100;
			printf("\r\n[SU300]Long delete key detected\r\n");

			if(SU300_FastFlag == SU300_DEL_AUTO_START)
			{
				SU300_FastFlag = SU300_DEL_AUTO_ON;
				SU300_FastTimer = gMainTimerCnt_10m;
			}
		}


        if(m_nSU300_Mode == SU300_MODE_RESULT_FAIL)
        { 
            m_nSU300_FailMsg_Cnt++;
            if(m_nSU300_FailMsg_Cnt>=2*5)//2sec
			{
                if(!m_bSU300_GoTo_LayerMode)
                {
                    m_nSU300_FailMsg_Cnt=0;
                    
                    printf("m_nSU300_Mode==SU300_MODE_RESULT_FAIL Autoback m_nSU300_Mode=%d\n",m_nSU300_Mode);
                    m_nSU300_Mode = SU300_MODE_START;
                    //Error Message
                    ituWidgetSetVisible(SU300_CONT_ERROR_TXT, false);
                    ituWidgetSetVisible(SU300_CONT_ERROR_1_TXT, false);//chpark.24.02.07
                    //V Icon    
                    ituWidgetSetVisible(SU300_CONT_RESULT, false);
                }
            }
        }

		if(m_bSU300_GoTo_LayerMode)
		{
			m_nSU300_GoTo_LayerModeCnt++;

			if(m_nSU300_GoTo_LayerModeCnt>=2*5)//2sec
			{
                //chpark.12.19
                SU300_1SecCNT = 100;
                SU300_FastFlag = SU300_DEL_AUTO_OFF;

				m_bSU300_GoTo_LayerMode = false;
				m_nSU300_GoTo_LayerModeCnt=0;

                if(m_bSU300_AUI1_Rcv_Error)
                {
                    ituWidgetSetVisible(SU300_CONT_RESULT, false);//chpark.24.02.14
                    m_bSU300_AUI1_Rcv_Error = false;
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
                    if(!m_bSU300_ReadyTo_Input)
                    {
                        ITULayer *M7_SU100 = ituSceneFindWidget(&theScene, "M7_SU100");
                        assert(M7_SU100);
                        ituLayerGoto(M7_SU100);
                        return false;
                    }

                    memcpy(&User_RegInfo[g_nSU100_AddUserIdx].UserName[0],SU300_TxtUserName, MAX_SU100_NAME_CNT);  
                    if(g_nSU100_PreLayer==M7_SU200_EDIT_LAYER)
                    {
                        ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
                        assert(M7_SU200_EDIT);
                        ituLayerGoto(M7_SU200_EDIT);
                    }
                    else
                    {
                        g_nSU100_PreLayer = M7_SU300_NAME_LAYER;
                       
                        ITULayer *M7_SU400_PASS = ituSceneFindWidget(&theScene, "M7_SU400_PASS");
                        assert(M7_SU400_PASS);
                        ituLayerGoto(M7_SU400_PASS);
                    }
                }
                else if(g_nSU100_Mode==SU100_Edit_Mode)
                {
                    memcpy(&User_RegInfo[g_nSU100_EditUserIdx].UserName[0],SU300_TxtUserName, MAX_SU100_NAME_CNT);  

                    ITULayer *M7_SU100 = ituSceneFindWidget(&theScene, "M7_SU100");
                    assert(M7_SU100);
                    ituLayerGoto(M7_SU100);
                }
			}			
		}

        SU300_CursorCNT++;
        if(SU300_FastFlag == SU300_DEL_AUTO_ON)//chpark.12.19
        {
            m_bSU300_CursorShow = true;
            ituWidgetSetVisible(SU300_Cursor[0], m_bSU300_CursorShow); 
        }
        else
        {
            if(SU300_CursorCNT>=2)
            {
                SU300_CursorCNT=0;
                m_bSU300_CursorShow = !m_bSU300_CursorShow;
                ituWidgetSetVisible(SU300_Cursor[0], m_bSU300_CursorShow); 
            }
        }

        //======
        //data received chpark.08.22
        SU300_SendCNT++;
		
		if(SU300_SendCNT>=5)//1sec
		{
			SU300_SendCNT=0;

		}

        if(g_UM5_ChkTime>0 && !m_bSU300_GoTo_LayerMode)
        {
            g_UM5_ChkTime = 0;
           
            if(SU300_UM5_Rcv_Type==SU300_RCV_USER_IDX)
            {
                //0) empty user number receive
                //0,1 already exist (valid user idx:2-14)
                if(SU300_UM5_2Byte_07bit>=2 && SU300_UM5_2Byte_07bit<=15)//chpark.24.02.13
                {
                    g_nSU100_AddUserIdx = SU300_UM5_2Byte_07bit-1;
                    SU300_UM5_Rcv_Type = SU300_RCV_USER_CHK;//next step
                    printf("[SU300]SU300_RCV_USER_IDX:g_nSU100_AddUserIdx=%d\n",g_nSU100_AddUserIdx);
                    User_RegInfo[g_nSU100_AddUserIdx].bAddProcessing = true;
                    //Send TX
                    g_UM5_ChkTime = 0;
                    SU300_UM5_Rcv_Type=SU300_RCV_USER_CHK;
                    SU300_Send_TX(1);
                }
                else 
                {
                    //chpark.12.13
                    SU300_UM5_Rcv_Type = SU300_RCV_INIT; //reset because of error
                    printf("[SU300]SU300_RCV_USER_IDX:Error:error in user index:%d\n",SU300_UM5_2Byte_07bit);
                    
                    ituWidgetSetVisible(SU300_CONT_ERROR_TXT, false);
                    ituWidgetSetVisible(SU300_CONT_ERROR_1_TXT, true);//chpark.24.02.07
                    for(int i=0;i<5;i++)
                    {
                        ituWidgetSetVisible(SU300_ERROR_1_TXT[i], false);
                    }

                    if(SU300_UM5_2Byte_07bit==0)
                    {
                        printf("Not defined.\n");
                    }
                    else if(SU300_UM5_2Byte_07bit==0xFD)
                    {
                        printf("The number of registered users has reached its limit.\nNo more users can be registered.\n");
                        //Error Message
                        
                        ituWidgetSetVisible(SU300_ERROR_1_TXT[Lang_Flag[M7_UserIndex]], true);
                        //ituWidgetSetVisible(SU300_ERROR_TEXT,      false);//chpark.24.02.07
                        //ituTextSetString(SU300_ERROR_TEXT, g_SU300_MSG_TEXT_ERROR_NOSEAT[Lang_Flag[M7_UserIndex]]);
                    }
                    else if(SU300_UM5_2Byte_07bit==0xFE)
                    {
                        printf("Error.\n");
                    }
                    else if(SU300_UM5_2Byte_07bit==0xFF)
                    {
                        printf("N/A.\n");
                    }
                   
                    //error in add user index(different in server index)
                    //draw x
                    //X Icon.    
                    ituWidgetSetVisible(SU300_CONT_RESULT, true);
                    ituWidgetSetVisible(SU300_RESULT_ICON[SU300_SUCCESS], false);
                    ituWidgetSetVisible(SU300_RESULT_ICON[SU300_FAIL], true);
                    
                    m_bSU300_ReadyTo_Input  = false;
                    m_bSU300_GoTo_LayerMode = true;
                }    
            }
            else if(SU300_UM5_Rcv_Type==SU300_RCV_USER_CHK)
            {
                SU300_UM5_Rcv_Type = SU300_RCV_INIT;

                if(SU300_UM5_1Byte_45bit==1)
                {
                    //true
                    //can write name
                    printf("[SU300]SU300_RCV_USER_CHK:%d\n",SU300_UM5_1Byte_45bit);
                    
                    m_bSU300_ReadyTo_Input = true;
                }
                else if(SU300_UM5_1Byte_45bit==2)
                {
                    //error in add user index(different in server index)
                    //draw x
                    //X Icon.    
                    ituWidgetSetVisible(SU300_CONT_RESULT, true);
                    ituWidgetSetVisible(SU300_RESULT_ICON[SU300_SUCCESS], false);
                    ituWidgetSetVisible(SU300_RESULT_ICON[SU300_FAIL], true);
                    
                    printf("[SU300]SU300_RCV_USER_CHK:%d\n",SU300_UM5_1Byte_45bit);

                    m_bSU300_ReadyTo_Input  = false;
                    m_bSU300_GoTo_LayerMode = true;
                }
            }
            
        }
        //=====

		SU300_SecTimer = gMainTimerCnt_10m;
	}

    //chpark.12.19
    if(SU300_FastFlag == SU300_DEL_AUTO_ON)
	{
        if((gMainTimerCnt_10m - SU300_FastTimer) >= 10)//400ms
		{
            bool bRet = SU300_Delete_Char(0);   //delete char
            SU300_Display_UserName();// show user name

            if(!bRet)
            {
                //printf("[SU300] END Auto delete=====\n");
                SU300_1SecCNT  = 100;
                SU300_FastFlag = SU300_DEL_AUTO_OFF;
            }

			SU300_FastTimer = gMainTimerCnt_10m;
		}
    }

    return true;
}

//======================================================
bool SU300_Send_TX(int nType)
{
    uint8_t  txbuffer[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    uint8_t curUserIdx;

    if(g_nSU100_Mode==SU100_Add_Mode)
    {
       curUserIdx = g_nSU100_AddUserIdx+1;
    }
    else if(g_nSU100_Mode==SU100_Edit_Mode)
    {
       curUserIdx = g_nSU100_EditUserIdx+1;
    }

    //printf("[SU300_Send_TX] Send User Idx=%d\n",curUserIdx);

    switch(nType)
    {
        case 0:
                //UM1
                //0byte: [0bit=1 1bit=1] 0x0000 0011
                if(g_nSU100_Mode==SU100_Add_Mode)
                {
				    txbuffer[0] = 0x03;   
                }
                else
                {
                    txbuffer[0] = 0x01; 
                }

                txbuffer[2] = curUserIdx; //1~15
                CanbusTxDataManagement( gMainTimerCnt_10m,UM1,txbuffer);//chpark.08.22
            break;
        
        case 1:
                //UM1
                //2byte:[0:7bit] :user ID
                
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
				
                CanbusTxDataManagement( gMainTimerCnt_10m,UM1,txbuffer);//chpark.08.22
            break;

        default:
            break;
    }
}
//======================================================


