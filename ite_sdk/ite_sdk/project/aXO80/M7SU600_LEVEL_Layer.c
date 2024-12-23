#include <assert.h>

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

static bool m_bSU600_AUI1_Rcv_Error = false;//chpark.24.02.14
static ITUWidget* SU600_CONT_ERROR_TXT; //
static ITUWidget* SU600_ERROR_TXT[5]; //

//chpark.11.27
//char *g_SU600_MSG_TEXT_ERROR_NOSELECT[5];//"Select either one."
static bool m_bSU600_ErrorMsg_Show = false;
int     SU600_ErrorMsg_Cnt = 0;//chpark.11.27

static bool     m_bSU600ReadyToJOG = false;
static int      m_nSU600_ButtonIndex=0;//current selected key index. 0:ST 1:AD 2:YES 3:NO 4:D7 
static uint32_t SU600_SecTimer;
static uint32_t SU600_ExitCNT = 0;
static uint32_t SU600_SendCNT = 0;

static uint8_t  SU600_A_Level;//0:not registered user 5:admin 9:standard
static uint8_t  SU600_A_AUX;  //0:SU600_AUX_TYPE_NONE 1:yes 2:no
static bool     m_bSU600_GoTo_LayerMode   = false;
static int      m_nSU600_GoTo_LayerModeCnt= 0;

#define SU600_ICON_ST   0
#define SU600_ICON_AD   1



#define SU600_LANG_ENG 0
#define SU600_LANG_GER 1
#define SU600_LANG_FR 2
#define SU600_LANG_IT 3
#define SU600_LANG_SP 4

#define SU600_FAIL		    0
#define SU600_SUCCESS	    1
#define SU600_FAIL_NOCHECK	2
#define SU600_RESET	        10

#define SU600_OFF	0
#define SU600_ON	1

#define SU600_KEY_ST	0
#define SU600_KEY_AD	1
#define SU600_KEY_YES	2
#define SU600_KEY_NO	3
#define SU600_KEY_D7	4

#define SU600_RCV_INIT        0
#define SU600_RCV_LEVEL_CHK   1


static char SU600_TxtUserName[50];
//=====================================
static ITUWidget* SU600_TITLE[5]; //
//=====================================
static ITUWidget* SU600_TOP_ICON[2]; //
static ITUWidget* SU600_TOP_NAME; //
//=====================================
static ITUWidget* SU600_CONT_RESULT; //
static ITUWidget* SU600_RESULT_ICON[2]; //
//=====================================

static ITUWidget* SU600_TXT_ACCESS_LEVEL[5]; //

static ITUWidget* SU600_CONT_ON_ST;//chpark.01.15
static ITUWidget* SU600_CONT_OFF_ST;//chpark.01.15
static ITUWidget* SU600_ON_ST[5]; //chpark.01.15
static ITUWidget* SU600_OFF_ST[5]; //chpark.01.15
static ITUWidget* SU600_SEL_ST;

static ITUWidget* SU600_CONT_ON_AD;//chpark.01.15
static ITUWidget* SU600_CONT_OFF_AD;//chpark.01.15
static ITUWidget* SU600_ON_AD[5]; //chpark.01.15
static ITUWidget* SU600_OFF_AD[5]; //chpark.01.15
static ITUWidget* SU600_SEL_AD;

//====================================
static ITUWidget* SU600_CONT_ACCESS_TXT; //
static ITUWidget* SU600_TXT_ACCESS[5]; //
//=================================================
static ITUWidget* SU600_CONT_BTN_YES; //
static ITUWidget* SU600_CONT_ON_YES; //
static ITUWidget* SU600_ON_YES[5]; //
//=====================================
static ITUWidget* SU600_CONT_OFF_YES; //
static ITUWidget* SU600_OFF_YES[5]; //
//=====================================
static ITUWidget* SU600_SEL_YES;
//===================================================
static ITUWidget* SU600_CONT_BTN_NO; //
static ITUWidget* SU600_CONT_ON_NO; //
static ITUWidget* SU600_ON_NO[5]; //
//=====================================
static ITUWidget* SU600_CONT_OFF_NO; //
static ITUWidget* SU600_OFF_NO[5]; //
//=====================================
static ITUWidget* SU600_SEL_NO;
//=====================================
static ITUWidget* SU600_ICON_D7_SEL;
static ITUWidget* SU600_ICON_D7_DOWN;
//=====================================

static ITUWidget* SU600_SEL[10];



void SU600_InitControl();
void SU600_Show_Result_UI(int nResult);
void SU600_SetUserTitle(uint8_t tUserIdx);

void SU600_Select_ST_AD(int nUserType);
void SU600_Select_YES_NO(int  nType);

bool SU600_Button_Select(int nIndex,bool bDownImage);

void SU600_MenuTabNext();
void SU600_MenuTabPrev();
void SU600_MenuTabUp();
void SU600_MenuTabDown();
void SU600_MenuTabLeftRight();
void SU600_MenuTabESC();

bool SU600_BTN_MouseUp(ITUWidget* widget, char* param);
bool SU600_D7_MouseUp(ITUWidget* widget, char* param);//chpark.10.23

bool SU600_Send_TX(int nType);

uint8_t  SU600_UM5_1Byte_03bit;		//chpark.08.22

static uint8_t SU600_UM5_Rcv_Type=SU600_RCV_INIT;
static bool    m_bSU600_Chk_Success = true;


bool SU600_Enter(ITUWidget* widget, char* param)
{
   
    SU600_SecTimer = gMainTimerCnt_10m;
	SU600_ExitCNT = 0;
    m_bSU600_GoTo_LayerMode = false;
    m_nSU600_GoTo_LayerModeCnt= 0;

    g_UM5_ChkTime = 0;
    SU600_UM5_Rcv_Type = SU600_RCV_INIT;
    m_bSU600_ErrorMsg_Show = false;//chpark.11.27
   
    memset(SU600_TxtUserName, '\0', sizeof(SU600_TxtUserName));
    /*
    if(g_nSU100_Mode==SU100_Add_Mode)
    {
       printf("SU600_Enter==SU100_Add_Mode: g_nSU100_AddUserIdx=%d\n",g_nSU100_AddUserIdx);

    }
    else if(g_nSU100_Mode==SU100_Edit_Mode)
    {
        printf("SU600_Enter==SU100_Edit_Mode: g_nSU100_EditUserIdx=%d\n",g_nSU100_EditUserIdx);
        //memcpy(SU600_TxtUserName,&User_RegInfo[g_nSU100_EditUserIdx].UserName[0],MAX_NAME_CNT);  
    }
    */
   
    SU600_InitControl();

    SU600_SetUserTitle(g_nSU100_CurUserIdx);

    //(1) set first focus.24.04.16
    if(SU600_A_Level == SU100_USER_AD)
    {
         m_nSU600_ButtonIndex = SU600_KEY_AD;
    }
    else
    {
         m_nSU600_ButtonIndex = SU600_KEY_ST;
    }

    SU600_Button_Select(m_nSU600_ButtonIndex,false);

    if(g_nSU100_Mode == SU100_Add_Mode)
    {
        printf("[SU600_Enter]SU100_Add_Mode: g_nSU100_AddUserIdx=%d AUX_TYPE=%d\n",g_nSU100_AddUserIdx,User_RegInfo[g_nSU100_AddUserIdx].nTmp_AccessAUX);
        
        if(User_RegInfo[g_nSU100_AddUserIdx].nTmp_AccessAUX==SU600_AUX_TYPE_NONE)
        {
            SU600_Select_YES_NO(SU600_AUX_TYPE_NONE);
        }
        else if(User_RegInfo[g_nSU100_AddUserIdx].nTmp_AccessAUX==SU600_AUX_TYPE_YES)
        {
            SU600_Select_YES_NO(SU600_AUX_TYPE_YES);
        }
        else if(User_RegInfo[g_nSU100_AddUserIdx].nTmp_AccessAUX==SU600_AUX_TYPE_NO)
        {
            SU600_Select_YES_NO(SU600_AUX_TYPE_NO);
        }
    }
    else
    {
        printf("[SU600_Enter]SU100_Edit_Mode: g_nSU100_EditUserIdx=%d AUX_TYPE=%d\n",g_nSU100_EditUserIdx,User_RegInfo[g_nSU100_EditUserIdx].nTmp_AccessAUX);

        if(User_RegInfo[g_nSU100_EditUserIdx].nTmp_AccessAUX==SU600_AUX_TYPE_NONE)
        {
            //SU600_Select_YES_NO(SU600_AUX_TYPE_NONE);
            //chpark.24.04.03
            User_RegInfo[g_nSU100_EditUserIdx].AccessAUX = SU600_AUX_TYPE_NO;
            User_RegInfo[g_nSU100_EditUserIdx].nTmp_AccessAUX = SU600_AUX_TYPE_NO;
            SU600_Select_YES_NO(SU600_AUX_TYPE_NO);
        }
        else if(User_RegInfo[g_nSU100_EditUserIdx].nTmp_AccessAUX==SU600_AUX_TYPE_YES)
        {
            SU600_Select_YES_NO(SU600_AUX_TYPE_YES);
        }
        else if(User_RegInfo[g_nSU100_EditUserIdx].nTmp_AccessAUX==SU600_AUX_TYPE_NO)
        {
            SU600_Select_YES_NO(SU600_AUX_TYPE_NO);
        }
    }
    
    m_bSU600ReadyToJOG = true;

    m_bSU600_AUI1_Rcv_Error = false;        //chpark.24.02.14
    UserPW_Time_AUI1 = gMainTimerCnt_10m;	//chpark.24.02.14

    //chpark.24.03.08
    if(g_nSU100_Mode==SU100_Add_Mode)
    {
        SU600_Send_TX(0);
    }
    else
    {
        printf("[SU600_Enter]SU600_Send_TX :dont send.");
    }

    return true;
}


void SU600_SetUserTitle(uint8_t tUserIdx)
{
	uint8_t i;
	uint8_t tDestName[100];
	
	//Set Icon
	if(User_RegInfo[tUserIdx].RegFlag==SU100_USER_AD)
	{
		ituWidgetSetVisible(SU600_TOP_ICON[SU600_ICON_AD],true);
		ituWidgetSetVisible(SU600_TOP_ICON[SU600_ICON_ST],false);

		ituWidgetSetVisible(SU600_TOP_NAME,true);
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[tUserIdx].UserName);
		ituTextSetString(SU600_TOP_NAME, tDestName);
	}
	else if(User_RegInfo[tUserIdx].RegFlag==SU100_USER_ST)
	{
		ituWidgetSetVisible(SU600_TOP_ICON[SU100_ICON_AD],false);
		ituWidgetSetVisible(SU600_TOP_ICON[SU100_ICON_ST],true);

		ituWidgetSetVisible(SU600_TOP_NAME,true);
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[tUserIdx].UserName);
		ituTextSetString(SU600_TOP_NAME, tDestName);
	}
    else
    {
        ituWidgetSetVisible(SU600_TOP_ICON[SU100_ICON_AD],false);
		ituWidgetSetVisible(SU600_TOP_ICON[SU100_ICON_ST],false);
    }

    if(g_UI1_ChkTime>0)
	{
        memset(tDestName,0,100);

		ituWidgetSetVisible(SU600_TOP_NAME,true);
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[tUserIdx].UserName);
		ituTextSetString(SU600_TOP_NAME, tDestName);
	}
	else if((g_UI1_ChkTime == 0)||(M7_UserIndex > (MAX_SU100_USER_NUM-1)))
	{
		ituWidgetSetVisible(SU600_TOP_NAME,false);
	}
}



void SU600_MenuTabNext()
{
    if(SU600_A_Level == SU100_USER_ST)
    {
        //Standard mode
        m_nSU600_ButtonIndex++;
        if(m_nSU600_ButtonIndex>SU600_KEY_D7)
            m_nSU600_ButtonIndex = SU600_KEY_ST;
    }
    else
    {
        //Admin mode
        if(m_nSU600_ButtonIndex==SU600_KEY_ST)
        {
            m_nSU600_ButtonIndex = SU600_KEY_AD;
        }
        else if(m_nSU600_ButtonIndex==SU600_KEY_AD)
        {
            m_nSU600_ButtonIndex = SU600_KEY_D7;
        }
        else if(m_nSU600_ButtonIndex==SU600_KEY_D7)
        {
            m_nSU600_ButtonIndex = SU600_KEY_ST;
        }
    }

    SU600_Button_Select(m_nSU600_ButtonIndex,false);
}

void SU600_MenuTabPrev()
{
    if(SU600_A_Level == SU100_USER_ST)
    {
        //Standard mode
        m_nSU600_ButtonIndex--;
        if(m_nSU600_ButtonIndex<SU600_KEY_ST)
            m_nSU600_ButtonIndex = SU600_KEY_D7;
    }
    else
    {
        //Admin mode
        if(m_nSU600_ButtonIndex==SU600_KEY_ST)
        {
            m_nSU600_ButtonIndex = SU600_KEY_D7;
        }
        else if(m_nSU600_ButtonIndex==SU600_KEY_AD)
        {
            m_nSU600_ButtonIndex = SU600_KEY_ST;
        }
        else if(m_nSU600_ButtonIndex==SU600_KEY_D7)
        {
            m_nSU600_ButtonIndex = SU600_KEY_AD;
        }
    }
    
    SU600_Button_Select(m_nSU600_ButtonIndex,false);
}


void SU600_MenuTabUp()
{
    if(SU600_A_Level == SU100_USER_ST)
    {
        //Standard mode
        switch(m_nSU600_ButtonIndex)
        {
            case SU600_KEY_ST: 
                m_nSU600_ButtonIndex=SU600_KEY_YES; 
                break;
            case SU600_KEY_YES: 
                m_nSU600_ButtonIndex=SU600_KEY_ST; 
                break;
            case SU600_KEY_AD: 
                m_nSU600_ButtonIndex=SU600_KEY_D7; 
                break;
            case SU600_KEY_D7: 
                m_nSU600_ButtonIndex=SU600_KEY_NO; 
                break;
            case SU600_KEY_NO: 
                m_nSU600_ButtonIndex=SU600_KEY_AD; 
                break;
            default:
                break;           
        }
    }
    else
    {
        //Admin mode
        if(m_nSU600_ButtonIndex==SU600_KEY_D7)
        {
            m_nSU600_ButtonIndex = SU600_KEY_AD;
        }
        else if(m_nSU600_ButtonIndex==SU600_KEY_AD)
        {
            m_nSU600_ButtonIndex = SU600_KEY_D7;
        }
    }

    SU600_Button_Select(m_nSU600_ButtonIndex,false);
}

void SU600_MenuTabDown()
{
    //Standard mode
    if(SU600_A_Level == SU100_USER_ST)
    {
        switch(m_nSU600_ButtonIndex)
        {
            case SU600_KEY_ST: 
                m_nSU600_ButtonIndex=SU600_KEY_YES; 
                break;
            case SU600_KEY_YES: 
                m_nSU600_ButtonIndex=SU600_KEY_ST; 
                break;
            case SU600_KEY_AD: 
                m_nSU600_ButtonIndex=SU600_KEY_NO; 
                break;
            case SU600_KEY_NO: 
                m_nSU600_ButtonIndex=SU600_KEY_D7; 
                break;
            case SU600_KEY_D7: 
                m_nSU600_ButtonIndex=SU600_KEY_AD; 
                break;
            default:
                break;           
        }
    }
    else
    {
        //Admin mode
        if(m_nSU600_ButtonIndex==SU600_KEY_D7)
        {
            m_nSU600_ButtonIndex = SU600_KEY_AD;
        }
        else if(m_nSU600_ButtonIndex==SU600_KEY_AD)
        {
            m_nSU600_ButtonIndex = SU600_KEY_D7;
        }
    }

    SU600_Button_Select(m_nSU600_ButtonIndex,false);
}

void SU600_MenuTabLeftRight()
{
    if(SU600_A_Level == SU100_USER_ST)
    {
        //Standard mode
        switch(m_nSU600_ButtonIndex)
        {
            case SU600_KEY_ST: 
                m_nSU600_ButtonIndex=SU600_KEY_AD; 
                break;
            case SU600_KEY_AD: 
                m_nSU600_ButtonIndex=SU600_KEY_ST; 
                break;
            case SU600_KEY_YES: 
                m_nSU600_ButtonIndex=SU600_KEY_NO; 
                break;
            case SU600_KEY_NO: 
                m_nSU600_ButtonIndex=SU600_KEY_YES; 
                break;           
            default:
                break;           
        }
    }
    else
    {
        //Admin mode
        switch(m_nSU600_ButtonIndex)
        {
            case SU600_KEY_ST: 
                m_nSU600_ButtonIndex=SU600_KEY_AD; 
                break;
            case SU600_KEY_AD: 
                m_nSU600_ButtonIndex=SU600_KEY_ST; 
                break;
            default:
                break;
        }    

    }

    SU600_Button_Select(m_nSU600_ButtonIndex,false);
}


void SU600_MenuTabESC()
{
    SU600_ExitCNT = 0;
                    
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


bool SU600_JogWheel(ITUWidget* widget, char* param)
{
    if(!m_bSU600ReadyToJOG || m_bSU600_GoTo_LayerMode)return false;
    
    SU600_ExitCNT = 0;                
    uint8_t JogDir = 0;
    JogDir = param[0];
	//printf("\n SU600_JogWheel %d============\r\n",JogDir);
	
	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
        case LANDSCAPE_1:
		case LANDSCAPE_2:
			if((JogDir == JOG_DIAL_ROT_RIGHT) )
			{
				SU600_MenuTabNext();
			}
			else if((JogDir == JOG_DIAL_ROT_LEFT) )
			{
				SU600_MenuTabPrev();
			}
			break;
	}

    return true;
}

bool SU600_JogButton(ITUWidget* widget, char* param)
{
    if(!m_bSU600ReadyToJOG || m_bSU600_GoTo_LayerMode)return false;

    SU600_ExitCNT = 0;
    uint8_t JogButton = 0;
	JogButton = param[0];
	//printf("SU600_JogButton %d\r\n",JogButton);

	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
		case LANDSCAPE_1:
		case LANDSCAPE_2:
			switch(JogButton)
			{
				case JOG_DIAL_KEY_UP:
                    SU600_MenuTabUp();
					break;

				case JOG_DIAL_KEY_DOWN:
                    SU600_MenuTabDown();
					break;
				
				case JOG_DIAL_KEY_LEFT:
                    SU600_MenuTabLeftRight();
                    break;

				case JOG_DIAL_KEY_RIGHT:
					SU600_MenuTabLeftRight();
					break;

				case JOG_DIAL_KEY_ENTER_ON:
                    SU600_Button_Select(m_nSU600_ButtonIndex,true);
					break;

				case JOG_DIAL_KEY_ENTER_OFF:				
        		case JOG_DIAL_KEY_NONE:
				{
                    //==========================
                     int nTmpKeyValue = m_nSU600_ButtonIndex;
                    char TMP_PARA[10];
                    
                    memset(TMP_PARA,0,10);
                    sprintf(TMP_PARA,"%d",nTmpKeyValue);
                    
                    SU600_BTN_MouseUp(NULL, TMP_PARA);
                    //==========================
        		}
					break;
			}
		break;
	}

    return true;
}

bool SU600_JogSwitch(ITUWidget* widget, char* param)
{
    SU600_ExitCNT = 0;
	uint8_t JogSwitch = 0;
	JogSwitch = param[0];

	if(JogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
        SU600_MenuTabESC();
	}
	else if(JogSwitch == JOG_DIAL_SW_HOME_OFF)
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}

    return true;
}

//==============================================================
void SU600_InitControl()
{
    int tIndex=0;
    char tmp_WidgetName[100];
//Control setting
//printf("SU600===========1111\n");
    SetWidget(SU600_TOP_ICON[SU600_ICON_ST],"SU600_T_Icon_Std");
    SetWidget(SU600_TOP_ICON[SU600_ICON_AD],"SU600_T_Icon_Admin");
	
	SetWidget(SU600_TOP_NAME,"SU600_T_Name");

    SetWidget(SU600_TITLE[SU600_LANG_ENG],"SU600_TITLE_ENG");
    SetWidget(SU600_TITLE[SU600_LANG_GER],"SU600_TITLE_GER");
    SetWidget(SU600_TITLE[SU600_LANG_FR], "SU600_TITLE_FR");
    SetWidget(SU600_TITLE[SU600_LANG_IT], "SU600_TITLE_IT");
    SetWidget(SU600_TITLE[SU600_LANG_SP], "SU600_TITLE_SP");
//printf("SU600===========22222\n");
//result
    SetWidget(SU600_CONT_RESULT, "SU600_CONT_RESULT");
    SetWidget(SU600_RESULT_ICON[SU600_SUCCESS],"SU600_OK");
    SetWidget(SU600_RESULT_ICON[SU600_FAIL],   "SU600_NO");
//Access Level
    //txt
    SetWidget(SU600_TXT_ACCESS_LEVEL[SU600_LANG_ENG],"SU600_TXT_ACCESS_LEV_ENG");
    SetWidget(SU600_TXT_ACCESS_LEVEL[SU600_LANG_GER],"SU600_TXT_ACCESS_LEV_GER");
    SetWidget(SU600_TXT_ACCESS_LEVEL[SU600_LANG_FR], "SU600_TXT_ACCESS_LEV_FR");
    SetWidget(SU600_TXT_ACCESS_LEVEL[SU600_LANG_IT], "SU600_TXT_ACCESS_LEV_IT");
    SetWidget(SU600_TXT_ACCESS_LEVEL[SU600_LANG_SP], "SU600_TXT_ACCESS_LEV_SP");
    //Standard
    SetWidget(SU600_SEL[SU600_KEY_ST],"SU600_SEL_ST");
    
    SetWidget(SU600_CONT_ON_ST, "SU600_CONT_ON_ST");//chpark.01.15
    SetWidget(SU600_ON_ST[SU600_LANG_ENG],"SU600_ON_ST_ENG");
    SetWidget(SU600_ON_ST[SU600_LANG_GER],"SU600_ON_ST_GER");
    SetWidget(SU600_ON_ST[SU600_LANG_FR], "SU600_ON_ST_FR");
    SetWidget(SU600_ON_ST[SU600_LANG_IT], "SU600_ON_ST_IT");
    SetWidget(SU600_ON_ST[SU600_LANG_SP], "SU600_ON_ST_SP");

    SetWidget(SU600_CONT_OFF_ST, "SU600_CONT_OFF_ST");
    SetWidget(SU600_OFF_ST[SU600_LANG_ENG],"SU600_OFF_ST_ENG");
    SetWidget(SU600_OFF_ST[SU600_LANG_GER],"SU600_OFF_ST_GER");
    SetWidget(SU600_OFF_ST[SU600_LANG_FR], "SU600_OFF_ST_FR");
    SetWidget(SU600_OFF_ST[SU600_LANG_IT], "SU600_OFF_ST_IT");
    SetWidget(SU600_OFF_ST[SU600_LANG_SP], "SU600_OFF_ST_SP");
    
    //Admin
    SetWidget(SU600_SEL[SU600_KEY_AD],"SU600_SEL_AD");

    SetWidget(SU600_CONT_ON_AD, "SU600_CONT_ON_AD");//chpark.01.15
    SetWidget(SU600_ON_AD[SU600_LANG_ENG],"SU600_ON_AD_ENG");
    SetWidget(SU600_ON_AD[SU600_LANG_GER],"SU600_ON_AD_GER");
    SetWidget(SU600_ON_AD[SU600_LANG_FR], "SU600_ON_AD_FR");
    SetWidget(SU600_ON_AD[SU600_LANG_IT], "SU600_ON_AD_IT");
    SetWidget(SU600_ON_AD[SU600_LANG_SP], "SU600_ON_AD_SP");

    SetWidget(SU600_CONT_OFF_AD, "SU600_CONT_OFF_AD");
    SetWidget(SU600_OFF_AD[SU600_LANG_ENG],"SU600_OFF_AD_ENG");
    SetWidget(SU600_OFF_AD[SU600_LANG_GER],"SU600_OFF_AD_GER");
    SetWidget(SU600_OFF_AD[SU600_LANG_FR], "SU600_OFF_AD_FR");
    SetWidget(SU600_OFF_AD[SU600_LANG_IT], "SU600_OFF_AD_IT");
    SetWidget(SU600_OFF_AD[SU600_LANG_SP], "SU600_OFF_AD_SP");

//Access for AUXsetting
    //Text
    SetWidget(SU600_CONT_ACCESS_TXT, "SU600_CONT_ACCESS_TXT");
    SetWidget(SU600_TXT_ACCESS[SU600_LANG_ENG],"SU600_TXT_ACCESS_ENG");
    SetWidget(SU600_TXT_ACCESS[SU600_LANG_GER],"SU600_TXT_ACCESS_GER");
    SetWidget(SU600_TXT_ACCESS[SU600_LANG_FR], "SU600_TXT_ACCESS_FR");
    SetWidget(SU600_TXT_ACCESS[SU600_LANG_IT], "SU600_TXT_ACCESS_IT");
    SetWidget(SU600_TXT_ACCESS[SU600_LANG_SP], "SU600_TXT_ACCESS_SP");

    //Yes
    SetWidget(SU600_CONT_BTN_YES, "SU600_CONT_BTN_YES");
    SetWidget(SU600_CONT_ON_YES, "SU600_CONT_ON_YES");
    SetWidget(SU600_ON_YES[SU600_LANG_ENG],"SU600_ON_YES_ENG");
    SetWidget(SU600_ON_YES[SU600_LANG_GER],"SU600_ON_YES_GER");
    SetWidget(SU600_ON_YES[SU600_LANG_FR], "SU600_ON_YES_FR");
    SetWidget(SU600_ON_YES[SU600_LANG_IT], "SU600_ON_YES_IT");
    SetWidget(SU600_ON_YES[SU600_LANG_SP], "SU600_ON_YES_SP");

    SetWidget(SU600_CONT_OFF_YES, "SU600_CONT_OFF_YES");
    SetWidget(SU600_OFF_YES[SU600_LANG_ENG],"SU600_OFF_YES_ENG");
    SetWidget(SU600_OFF_YES[SU600_LANG_GER],"SU600_OFF_YES_GER");
    SetWidget(SU600_OFF_YES[SU600_LANG_FR], "SU600_OFF_YES_FR");
    SetWidget(SU600_OFF_YES[SU600_LANG_IT], "SU600_OFF_YES_IT");
    SetWidget(SU600_OFF_YES[SU600_LANG_SP], "SU600_OFF_YES_SP");

    SetWidget(SU600_SEL[SU600_KEY_YES], "SU600_SEL_YES");

    //No
    SetWidget(SU600_CONT_BTN_NO, "SU600_CONT_BTN_NO");
    SetWidget(SU600_CONT_ON_NO, "SU600_CONT_ON_NO");
    SetWidget(SU600_ON_NO[SU600_LANG_ENG],"SU600_ON_NO_ENG");
    SetWidget(SU600_ON_NO[SU600_LANG_GER],"SU600_ON_NO_GER");
    SetWidget(SU600_ON_NO[SU600_LANG_FR], "SU600_ON_NO_FR");
    SetWidget(SU600_ON_NO[SU600_LANG_IT], "SU600_ON_NO_IT");
    SetWidget(SU600_ON_NO[SU600_LANG_SP], "SU600_ON_NO_SP");

    SetWidget(SU600_CONT_OFF_NO, "SU600_CONT_OFF_NO");
    SetWidget(SU600_OFF_NO[SU600_LANG_ENG],"SU600_OFF_NO_ENG");
    SetWidget(SU600_OFF_NO[SU600_LANG_GER],"SU600_OFF_NO_GER");
    SetWidget(SU600_OFF_NO[SU600_LANG_FR], "SU600_OFF_NO_FR");
    SetWidget(SU600_OFF_NO[SU600_LANG_IT], "SU600_OFF_NO_IT");
    SetWidget(SU600_OFF_NO[SU600_LANG_SP], "SU600_OFF_NO_SP");

    SetWidget(SU600_SEL[SU600_KEY_NO], "SU600_SEL_NO");   

    SetWidget(SU600_SEL[SU600_KEY_D7],  "SU600_ICON_D7_SEL");   
    SetWidget(SU600_ICON_D7_DOWN, "SU600_ICON_D7_DOWN");   

     //chpark.11.27
    //g_SU600_MSG_TEXT_ERROR_NOSELECT[SU600_LANG_ENG] = "Select either one.";
    //g_SU600_MSG_TEXT_ERROR_NOSELECT[SU600_LANG_GER] = "Select either one.";
    //g_SU600_MSG_TEXT_ERROR_NOSELECT[SU600_LANG_FR]  = "Select either one.";
    //g_SU600_MSG_TEXT_ERROR_NOSELECT[SU600_LANG_IT]  = "Select either one.";
    //g_SU600_MSG_TEXT_ERROR_NOSELECT[SU600_LANG_SP]  = "Select either one.";

    SetWidget(SU600_CONT_ERROR_TXT, "SU600_CONT_ERROR_TXT");
    SetWidget(SU600_ERROR_TXT[SU600_LANG_ENG], "SU600_ERROR_ENG");
    SetWidget(SU600_ERROR_TXT[SU600_LANG_GER], "SU600_ERROR_GER");
    SetWidget(SU600_ERROR_TXT[SU600_LANG_FR],  "SU600_ERROR_FR");
    SetWidget(SU600_ERROR_TXT[SU600_LANG_IT],  "SU600_ERROR_IT");
    SetWidget(SU600_ERROR_TXT[SU600_LANG_SP],  "SU600_ERROR_SP");
     
    //================================================================ 
    ituWidgetSetVisible(SU600_CONT_ERROR_TXT, false);//error msg text

    for(int i=0; i<5; i++)
    {
        ituWidgetSetVisible(SU600_TITLE[i], false);
        ituWidgetSetVisible(SU600_TXT_ACCESS_LEVEL[i], false);
        ituWidgetSetVisible(SU600_TXT_ACCESS[i], false);        
    }   

    ituWidgetSetVisible(SU600_TITLE[Lang_Flag[M7_UserIndex]], true);              //title
    ituWidgetSetVisible(SU600_TXT_ACCESS_LEVEL[Lang_Flag[M7_UserIndex]], true);   //Access Level
    ituWidgetSetVisible(SU600_TXT_ACCESS[Lang_Flag[M7_UserIndex]], true);         //Access for AUX setting
   
    ituWidgetSetVisible(SU600_CONT_RESULT, false);//result image (o,X)
  
    ituWidgetSetVisible(SU600_SEL[SU600_KEY_D7],   false);
    ituWidgetSetVisible(SU600_ICON_D7_DOWN,  false); 

    if(g_nSU100_Mode == SU100_Add_Mode)
    {
        if(User_RegInfo[g_nSU100_AddUserIdx].RegFlag==SU100_USER_AD)
        {
            //admin
            SU600_Select_ST_AD(SU100_USER_AD); 
        }
        else if(User_RegInfo[g_nSU100_AddUserIdx].RegFlag==SU100_USER_ST)
        {
            //standard
            SU600_Select_ST_AD(SU100_USER_ST); 
        }
        else
        {
            //Error 
            printf("[Add][Level]Error in ST_AD Level Index=%d:%d \n",g_nSU100_AddUserIdx,User_RegInfo[g_nSU100_AddUserIdx].RegFlag);
            SU600_Select_ST_AD(SU100_USER_N_R); 
        }
    }
    else if(g_nSU100_Mode == SU100_Edit_Mode)
    {
        if(User_RegInfo[g_nSU100_EditUserIdx].RegFlag==SU100_USER_AD)
        {
            //admin
            SU600_Select_ST_AD(SU100_USER_AD); 
        }
        else if(User_RegInfo[g_nSU100_EditUserIdx].RegFlag==SU100_USER_ST)
        {
            //standard
            SU600_Select_ST_AD(SU100_USER_ST); 
        }
        else if(User_RegInfo[g_nSU100_EditUserIdx].RegFlag==SU100_USER_N_R || User_RegInfo[g_nSU100_EditUserIdx].RegFlag==SU100_USER_N_A)
        {
            //NR NA
            SU600_Select_ST_AD(SU100_USER_N_R); 
        }
        else
        {
            //Error 
            printf("[Edit][Level]Error in ST_AD Level Index=%d:%d \n",g_nSU100_EditUserIdx,User_RegInfo[g_nSU100_EditUserIdx].RegFlag);
            SU600_Select_ST_AD(SU100_USER_N_R); 
        }
    } 
}

//chpark.01.15
void SU600_Select_ST_AD(int nUserType)
{
    if(nUserType==SU100_USER_ST)
    {
        //standard mode
       
        //standard on
        ituWidgetSetVisible(SU600_CONT_ON_ST,   true);
        ituWidgetSetVisible(SU600_CONT_OFF_ST, false);
        for(int i=0; i<5; i++)
        {
            ituWidgetSetVisible(SU600_ON_ST[i], false);
        }
        ituWidgetSetVisible(SU600_ON_ST[Lang_Flag[M7_UserIndex]], true);
        //admin off
        ituWidgetSetVisible(SU600_CONT_ON_AD,   false);
        ituWidgetSetVisible(SU600_CONT_OFF_AD,  true);
        for(int i=0; i<5; i++)
        {
            ituWidgetSetVisible(SU600_OFF_AD[i], false);
        }
        ituWidgetSetVisible(SU600_OFF_AD[Lang_Flag[M7_UserIndex]], true);

        //yes no button show
        ituWidgetSetVisible(SU600_CONT_ACCESS_TXT,   true);
        ituWidgetSetVisible(SU600_CONT_BTN_YES,  true);
        ituWidgetSetVisible(SU600_CONT_BTN_NO,   true);

        SU600_A_Level = SU100_USER_ST;
    }
    else if(nUserType==SU100_USER_AD)
    {
        //admin mode
        
        //standard off
        ituWidgetSetVisible(SU600_CONT_ON_ST,   false);
        ituWidgetSetVisible(SU600_CONT_OFF_ST,  true);
        for(int i=0; i<5; i++)
        {
            ituWidgetSetVisible(SU600_OFF_ST[i], false);
        }
        ituWidgetSetVisible(SU600_OFF_ST[Lang_Flag[M7_UserIndex]], true);
        //admin on
        ituWidgetSetVisible(SU600_CONT_ON_AD,   true);
        ituWidgetSetVisible(SU600_CONT_OFF_AD,  false);
        for(int i=0; i<5; i++)
        {
            ituWidgetSetVisible(SU600_ON_AD[i], false);
        }
        ituWidgetSetVisible(SU600_ON_AD[Lang_Flag[M7_UserIndex]], true);

        //yes no button hide
        ituWidgetSetVisible(SU600_CONT_ACCESS_TXT,    false);
        ituWidgetSetVisible(SU600_CONT_BTN_YES,   false);
        ituWidgetSetVisible(SU600_CONT_BTN_NO,    false);

        if(g_nSU100_Mode == SU100_Add_Mode)
        {
            User_RegInfo[g_nSU100_AddUserIdx].nTmp_AccessAUX=SU600_AUX_TYPE_NONE;
        }

        SU600_A_Level = SU100_USER_AD;
    }
    else if(nUserType==SU100_USER_N_R || nUserType==SU100_USER_N_A)
    {
        if(g_nSU100_Mode == SU100_Edit_Mode)
        {
            //standard off
            ituWidgetSetVisible(SU600_CONT_ON_ST,   false);
            ituWidgetSetVisible(SU600_CONT_OFF_ST,  false);
            //for(int i=0; i<5; i++)
            //{
            //    ituWidgetSetVisible(SU600_OFF_ST[i], false);
            //}
            //ituWidgetSetVisible(SU600_OFF_ST[Lang_Flag[M7_UserIndex]], true);

            //admin off
            ituWidgetSetVisible(SU600_CONT_ON_AD,   false);
            ituWidgetSetVisible(SU600_CONT_OFF_AD,  false);
            //for(int i=0; i<5; i++)
            //{
            //    ituWidgetSetVisible(SU600_OFF_AD[i], false);
            //}
            //ituWidgetSetVisible(SU600_OFF_AD[Lang_Flag[M7_UserIndex]], true);

            //yes no button hide
            ituWidgetSetVisible(SU600_CONT_ACCESS_TXT,    false);
            ituWidgetSetVisible(SU600_CONT_BTN_YES,   false);
            ituWidgetSetVisible(SU600_CONT_BTN_NO,    false);

            SU600_A_Level = nUserType; //chpark.11.27
        }
        else
        {
            //standard off
            ituWidgetSetVisible(SU600_CONT_ON_ST,   false);
            ituWidgetSetVisible(SU600_CONT_OFF_ST,  true);
            for(int i=0; i<5; i++)
            {
                ituWidgetSetVisible(SU600_OFF_ST[i], false);
            }
            ituWidgetSetVisible(SU600_OFF_ST[Lang_Flag[M7_UserIndex]], true);

            //admin off
            ituWidgetSetVisible(SU600_CONT_ON_AD,   false);
            ituWidgetSetVisible(SU600_CONT_OFF_AD,  true);
            for(int i=0; i<5; i++)
            {
                ituWidgetSetVisible(SU600_OFF_AD[i], false);
            }
            ituWidgetSetVisible(SU600_OFF_AD[Lang_Flag[M7_UserIndex]], true);

            //yes no button hide
            ituWidgetSetVisible(SU600_CONT_ACCESS_TXT,    false);
            ituWidgetSetVisible(SU600_CONT_BTN_YES,   false);
            ituWidgetSetVisible(SU600_CONT_BTN_NO,    false);

            SU600_A_Level = nUserType; //chpark.11.27
        }

        printf("SU600_A_Level Type :N_R or N_A User: %d\n",SU600_A_Level);
    }
    else
    {
        //standard off
        ituWidgetSetVisible(SU600_CONT_ON_ST,   false);
        ituWidgetSetVisible(SU600_CONT_OFF_ST,  false);

         //admin off
        ituWidgetSetVisible(SU600_CONT_ON_AD,   false);
        ituWidgetSetVisible(SU600_CONT_OFF_AD,  false);

        SU600_A_Level = nUserType; //chpark.11.27

        printf("SU600_A_Level Type Error: %d\n",SU600_A_Level);
    }
}



void SU600_Select_YES_NO(int  nType)
{
    if(g_nSU100_Mode == SU100_Edit_Mode)
    {
        if(SU600_A_Level == SU100_USER_N_R || SU600_A_Level == SU100_USER_N_A)
        {
            //printf("[SU600_Select_YES_NO]SU600_A_Level = %d\n",SU600_A_Level);
            //yes no button hide
            ituWidgetSetVisible(SU600_CONT_ACCESS_TXT,    false);
            ituWidgetSetVisible(SU600_CONT_BTN_YES,   false);
            ituWidgetSetVisible(SU600_CONT_BTN_NO,    false);

            return;
        }
    }

    if(nType==SU600_AUX_TYPE_YES)
    {
        //YES selected

        //YES on
        ituWidgetSetVisible(SU600_CONT_ON_YES,   true);
        ituWidgetSetVisible(SU600_CONT_OFF_YES, false);
        for(int i=0; i<5; i++)
        {
            ituWidgetSetVisible(SU600_ON_YES[i], false);
        }
        ituWidgetSetVisible(SU600_ON_YES[Lang_Flag[M7_UserIndex]], true);
        //No off
        ituWidgetSetVisible(SU600_CONT_ON_NO,   false);
        ituWidgetSetVisible(SU600_CONT_OFF_NO,  true);
        for(int i=0; i<5; i++)
        {
            ituWidgetSetVisible(SU600_OFF_NO[i], false);
        }
        ituWidgetSetVisible(SU600_OFF_NO[Lang_Flag[M7_UserIndex]], true);

        SU600_A_AUX = SU600_AUX_TYPE_YES;

        //printf("[SU600_Select_YES_NO] [YES button]=====%d\n",SU600_A_AUX);
    }       
    else if(nType==SU600_AUX_TYPE_NO)
    {
        //No selected

        //YES off
        ituWidgetSetVisible(SU600_CONT_ON_YES,   false);
        ituWidgetSetVisible(SU600_CONT_OFF_YES,  true);
        for(int i=0; i<5; i++)
        {
            ituWidgetSetVisible(SU600_OFF_YES[i], false);
        }
        ituWidgetSetVisible(SU600_OFF_YES[Lang_Flag[M7_UserIndex]], true);
        //No on
        ituWidgetSetVisible(SU600_CONT_ON_NO,   true);
        ituWidgetSetVisible(SU600_CONT_OFF_NO,  false);
        for(int i=0; i<5; i++)
        {
            ituWidgetSetVisible(SU600_ON_NO[i], false);
        }
        ituWidgetSetVisible(SU600_ON_NO[Lang_Flag[M7_UserIndex]], true);

        SU600_A_AUX = SU600_AUX_TYPE_NO;

        //printf("[SU600_Select_YES_NO] [NO button]=====%d \n",SU600_A_AUX);
    }
    else
    {
        //NONE selected
        //YES off
        ituWidgetSetVisible(SU600_CONT_ON_YES,   false);
        ituWidgetSetVisible(SU600_CONT_OFF_YES,  true);
        for(int i=0; i<5; i++)
        {
            ituWidgetSetVisible(SU600_OFF_YES[i], false);
        }
        ituWidgetSetVisible(SU600_OFF_YES[Lang_Flag[M7_UserIndex]], true);
        //No off
        ituWidgetSetVisible(SU600_CONT_ON_NO,   false);
        ituWidgetSetVisible(SU600_CONT_OFF_NO,  true);
        for(int i=0; i<5; i++)
        {
            ituWidgetSetVisible(SU600_OFF_NO[i], false);
        }
        ituWidgetSetVisible(SU600_OFF_NO[Lang_Flag[M7_UserIndex]], true);

        //printf("SU600_A_Level=====%d\n",SU600_A_Level);
       
        SU600_A_AUX = SU600_AUX_TYPE_NONE;
    } 
}



bool SU600_Button_Select(int nIndex,bool bDownImage)
{
    int nTmpPrevIndex = m_nSU600_ButtonIndex;
    
    if(nIndex>=SU600_KEY_ST && nIndex<=SU600_KEY_D7)
    {

    }
    else
    {
        return false;
    }
    
    //save new key index
    m_nSU600_ButtonIndex = nIndex;

    //clear
    for(int i=SU600_KEY_ST;i<=SU600_KEY_D7;i++)
    {
       ituWidgetSetVisible(SU600_SEL[i], false);
    }
    ituWidgetSetVisible(SU600_ICON_D7_DOWN,   false);
    //=======
    
    //show select
    ituWidgetSetVisible(SU600_SEL[nIndex], true);

    //draw down image
    if(bDownImage)
    {
        if(nIndex==SU600_KEY_D7)
        {
            ituWidgetSetVisible(SU600_ICON_D7_DOWN,   true);
        }
    }
    else
    {
        
    }
  
    return true;
}

void SU600_Show_Result_UI(int nResult)
{
    switch(nResult)
    {
        case SU600_SUCCESS:
        {
            //Error Message
        
            //V Icon    
            ituWidgetSetVisible(SU600_CONT_RESULT, true);
            ituWidgetSetVisible(SU600_RESULT_ICON[SU600_SUCCESS], true);
            ituWidgetSetVisible(SU600_RESULT_ICON[SU600_FAIL], false);
            //save data
            //
            //printf("SU600_Show_Result_UI:data save==1 \n");
            if(g_nSU100_Mode == SU100_Add_Mode)
            {
                User_RegInfo[g_nSU100_AddUserIdx].ShowUserList     = 1;
                User_RegInfo[g_nSU100_AddUserIdx].RegKeyNum        = g_nSU100_AddUserIdx;
                User_RegInfo[g_nSU100_AddUserIdx].RegFlag          = SU600_A_Level;
                User_RegInfo[g_nSU100_AddUserIdx].nTmp_RegFlag     = SU600_A_Level;
                User_RegInfo[g_nSU100_AddUserIdx].AccessAUX        = SU600_A_AUX;
                User_RegInfo[g_nSU100_AddUserIdx].nTmp_AccessAUX   = SU600_A_AUX;
                User_RegInfo[g_nSU100_AddUserIdx].Language         = Lang_Flag[M7_UserIndex]; //save user language.

                //move to passcode setting
                m_bSU600_GoTo_LayerMode = true;
                //printf("[SU600_Show_Result_UI][user:%d][SU600_A_AUX]:%d Lang:%d\n",g_nSU100_AddUserIdx,SU600_A_AUX,User_RegInfo[g_nSU100_AddUserIdx].Language);
            }
            else
            {
                User_RegInfo[g_nSU100_EditUserIdx].RegFlag          = SU600_A_Level;
                User_RegInfo[g_nSU100_EditUserIdx].nTmp_RegFlag     = SU600_A_Level;
                User_RegInfo[g_nSU100_EditUserIdx].AccessAUX        = SU600_A_AUX;
                User_RegInfo[g_nSU100_EditUserIdx].nTmp_AccessAUX   = SU600_A_AUX;
                
                //move to passcode setting
                m_bSU600_GoTo_LayerMode = true;
                //printf("[SU600_Show_Result_UI][user:%d][SU600_A_AUX]:%d Lang:%d\\n",g_nSU100_EditUserIdx,SU600_A_AUX,User_RegInfo[g_nSU100_EditUserIdx].Language);
            }
        }
        break;
    
        case SU600_FAIL:
        {
            //Error Message
            ituWidgetSetVisible(SU600_CONT_ERROR_TXT, false);//error msg text
            //X Icon.    
            ituWidgetSetVisible(SU600_CONT_RESULT, true);
            ituWidgetSetVisible(SU600_RESULT_ICON[SU600_SUCCESS], false);
            ituWidgetSetVisible(SU600_RESULT_ICON[SU600_FAIL], true);
        }
        break;

        case SU600_FAIL_NOCHECK:
        {
            //Message
            ituWidgetSetVisible(SU600_CONT_ERROR_TXT, true);
            for(int i=0; i<5; i++)
            {
                ituWidgetSetVisible(SU600_ERROR_TXT[i], false);
            }
            ituWidgetSetVisible(SU600_ERROR_TXT[Lang_Flag[M7_UserIndex]], true);

            //X Icon.    
            ituWidgetSetVisible(SU600_CONT_RESULT, true);
            ituWidgetSetVisible(SU600_RESULT_ICON[SU600_SUCCESS], false);
            ituWidgetSetVisible(SU600_RESULT_ICON[SU600_FAIL], true);
        }
        break;

        case SU600_RESET:
        {
            ituWidgetSetVisible(SU600_CONT_ERROR_TXT, false);
            ituWidgetSetVisible(SU600_CONT_RESULT, false);
        }
        break;

        default: 
        {
            ituWidgetSetVisible(SU600_CONT_ERROR_TXT, false);
            ituWidgetSetVisible(SU600_CONT_RESULT, false);
        }
        break;
    }

}

//==============================================================
bool SU600_BTN_MouseDn(ITUWidget* widget, char* param)
{
    if(!m_bSU600ReadyToJOG || m_bSU600_GoTo_LayerMode)return false;

    SU600_ExitCNT = 0;
    int nKeyIndex;
    nKeyIndex = atoi(param);
    //printf("SU600_BTN_MouseDn nKeyIndex=%d\n",nKeyIndex);
    
    if(m_bSU600_ErrorMsg_Show)//chpark.12.13
    {
        //printf("m_bSU600_ErrorMsg_Show=%d : return false\n",m_bSU600_ErrorMsg_Show);
        return false;
    }
    //key select ui
    SU600_Button_Select(nKeyIndex,true);

    return true;
}

bool SU600_BTN_MouseUp(ITUWidget* widget, char* param)
{
    if(!m_bSU600ReadyToJOG || m_bSU600_GoTo_LayerMode)return false;

    SU600_ExitCNT = 0;
    int nKeyIndex;
    nKeyIndex = atoi(param);
    //printf("SU600_BTN_MouseUp nKeyIndex=%d\n",nKeyIndex);

    if(m_bSU600_ErrorMsg_Show)//chpark.12.13
    {
        //printf("m_bSU600_ErrorMsg_Show=%d : return false\n",m_bSU600_ErrorMsg_Show);
        return false;
    }
    //key select ui
    SU600_Button_Select(nKeyIndex,false);

    switch(nKeyIndex)
    {
        case SU600_KEY_ST:
            SU600_Select_ST_AD(SU100_USER_ST);
            SU600_Select_YES_NO(SU600_AUX_TYPE_NONE);//chpark.24.02.14
            break;

        case SU600_KEY_AD:
            SU600_Select_ST_AD(SU100_USER_AD);
            break;

        case SU600_KEY_YES:
            SU600_Select_YES_NO(SU600_AUX_TYPE_YES);
            break;

        case SU600_KEY_NO:
            SU600_Select_YES_NO(SU600_AUX_TYPE_NO);
            break;

        case SU600_KEY_D7:
            SU600_D7_MouseUp(NULL, NULL);
            break;

        default:
            break;
    }    

    return true;
}
//==============================================================
bool SU600_D7_MouseUp(ITUWidget* widget, char* param)
{
    SU600_ExitCNT = 0;
    
    g_UM5_ChkTime = 0;
    SU600_UM5_Rcv_Type = SU600_RCV_LEVEL_CHK;

    ituWidgetSetVisible(SU600_ICON_D7_DOWN,   false);
    
    //======================
    //radio button check
    if(g_nSU100_Mode == SU100_Add_Mode)
    {
        if(SU600_A_Level == SU100_USER_N_R || SU600_A_Level == SU100_USER_N_A)
        {
            printf("SU600_A_Level==No check\n");
            m_bSU600_ErrorMsg_Show = true;
            SU600_ErrorMsg_Cnt = 0;
        
            SU600_Show_Result_UI(SU600_FAIL_NOCHECK);

            return false;
        }
    }
    
    if(SU600_A_Level==SU100_USER_ST && SU600_A_AUX==SU600_AUX_TYPE_NONE)
    {
        printf("SU600_A_AUX==No check\n");
        m_bSU600_ErrorMsg_Show = true;
        SU600_ErrorMsg_Cnt = 0;
        
        SU600_Show_Result_UI(SU600_FAIL_NOCHECK);

        return false;
    }
    //======================

    if(!gUI_Server_Mode)
    {
        //printf("SU600_D7_MouseUp==Local Mode\n");
        SU600_Show_Result_UI(SU600_SUCCESS);

        if(g_nSU100_Mode==SU100_Edit_Mode)
        {
            //SU600_Send_TX(3);
            //chpark.24.04.03
            #ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
                XO_Save_Config(g_nSU100_EditUserIdx,true,0);//chpark.12.12
            #else
                HT_Save_Config(1);
            #endif
        }
    }
    else
    {
        //SU600_Send_TX(1);
        SU600_Send_TX(2); //chpark.24.03.118
    }

    return true;
}

bool SU600_D6_MouseUp(ITUWidget* widget, char* param)
{
    SU600_ExitCNT = 0;
    SU100_Send_Add_Cancel();//chpark.10.30
	GotoMain(MAIN_MODE_NORMAL); //

    return true;
}

bool SU600_D5_MouseUp(ITUWidget* widget, char* param)
{
    SU600_ExitCNT = 0;
    
    SU600_MenuTabESC();

    return true;
}
//==============================================================

//==============================================================
bool SU600_Timer(ITUWidget* widget, char* param)
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
		if(!m_bSU600_AUI1_Rcv_Error && !m_bSU600_GoTo_LayerMode && gUI_Server_Mode)
		{
            printf("\r\n AUI1 Protocol 5Sec Exit >>>>>>> goto Main\r\n");

            ituWidgetSetVisible(SU600_CONT_RESULT, true);
            ituWidgetSetVisible(SU600_RESULT_ICON[SU600_SUCCESS], false);
            ituWidgetSetVisible(SU600_RESULT_ICON[SU600_FAIL], true);

            m_bSU600_AUI1_Rcv_Error = true;
            m_bSU600_GoTo_LayerMode = true;
        }
	}
    #endif

    
    if((gMainTimerCnt_10m - SU600_SecTimer) >= 20)//100) //200ms
	{
		if(SU600_ExitCNT < 30*5)//30sec
			SU600_ExitCNT++;
		else if(SU600_ExitCNT == 30*5)//30sec
		{
			SU600_ExitCNT = 300;
            #ifdef RELEASE_VER
                SU100_Send_Add_Cancel();//chpark.10.30
			    printf("\r\n SU600 Menu 30Sec Exit > goto Main\r\n");
			    GotoMain(MAIN_MODE_NORMAL); 
                return true;
            #endif
		}

        if(m_bSU600_GoTo_LayerMode)
		{
			m_nSU600_GoTo_LayerModeCnt++;

			if(m_nSU600_GoTo_LayerModeCnt>=2*5)//2sec
			{
				m_bSU600_GoTo_LayerMode = false;
				m_nSU600_GoTo_LayerModeCnt=0;

                if(m_bSU600_AUI1_Rcv_Error)
                {
                    ituWidgetSetVisible(SU600_CONT_RESULT, false);//chpark.24.02.14
                    m_bSU600_AUI1_Rcv_Error = false;
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
                    if(g_nSU100_PreLayer==M7_SU200_EDIT_LAYER)
                    {
                        ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
                        assert(M7_SU200_EDIT);
                        ituLayerGoto(M7_SU200_EDIT);
                    }
                    else
                    {
                        g_nSU100_PreLayer = M7_SU600_LEVEL_LAYER; 
                        //move 
                        ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
                        assert(M7_SU200_EDIT);
                        ituLayerGoto(M7_SU200_EDIT);
                    }
                }
                else if(g_nSU100_Mode==SU100_Edit_Mode)
                {
                    //move
                    ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
                    assert(M7_SU200_EDIT);
                    ituLayerGoto(M7_SU200_EDIT);
                }
			}			
		}
        //======
        //data received chpark.08.22
        SU600_SendCNT++;
		
		if(SU600_SendCNT>=5)//1sec
		{
			SU600_SendCNT=0;
		}

        if(g_UM5_ChkTime>0 && !m_bSU600_GoTo_LayerMode)
        {
            g_UM5_ChkTime = 0;
            if(SU600_UM5_Rcv_Type == SU600_RCV_LEVEL_CHK)
            {
                printf("[SU360]SU600_RCV_LEVEL_CHK:SU600_UM5_1Byte_03bit=%d==\n",SU600_UM5_1Byte_03bit);

                SU600_UM5_Rcv_Type = SU600_RCV_INIT;

                if(SU600_UM5_1Byte_03bit == 13)//chpark.24.02.20
                {
                    //X is displayed when there is no registered admin user.
                    m_bSU600_ErrorMsg_Show = true;
                    SU600_ErrorMsg_Cnt = 0;

                    SU600_Show_Result_UI(SU600_FAIL);
                }
                else if(SU600_A_Level == SU100_USER_ST)
                {
                    if(SU600_UM5_1Byte_03bit == 9)//standard
                    {
                        //true;
                        //V Check Show & move after 2sec
                        SU600_Show_Result_UI(SU600_SUCCESS);

                        //chpark.24.03.18
                        if(g_nSU100_Mode==SU100_Edit_Mode)
                        {
                            //SU600_Send_TX(3);
                            //chpark.24.04.03
                            #ifdef USED_SAVE_CONFIG		
                                XO_Save_Config(g_nSU100_EditUserIdx,true,0);
                            #else
                                HT_Save_Config(1);
                            #endif
                        }
                    }
                    else
                    {
                        printf("[RET Error] in Level : %d \n",SU600_UM5_1Byte_03bit);
                        //SU600_Show_Result_UI(SU600_SUCCESS);//chpark.12.18 for test
                    } 
                }
                else
                {
                    if(SU600_UM5_1Byte_03bit == 5)//admin
                    {
                        //true;
                        //V Check Show & move after 2sec
                        SU600_Show_Result_UI(SU600_SUCCESS);

                        //chpark.24.03.18
                        if(g_nSU100_Mode==SU100_Edit_Mode)
                        {
                            //SU600_Send_TX(3);
                            //chpark.24.04.03
                            #ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
                                XO_Save_Config(g_nSU100_EditUserIdx,true,0);//chpark.12.12
                            #else
                                HT_Save_Config(1);
                            #endif
                        }
                    } 
                    else
                    {
                        printf("[RET Error] in Level : %d \n",SU600_UM5_1Byte_03bit);
                        //SU600_Show_Result_UI(SU600_SUCCESS);//chpark.12.18 for test
                    } 
                }
            }
        }

        //================================
        if(m_bSU600_ErrorMsg_Show)
        {
            //printf("SU600_ErrorMsg_Cnt==>%d\n",SU600_ErrorMsg_Cnt);
            SU600_ErrorMsg_Cnt++;
            
            if(SU600_ErrorMsg_Cnt>=2*5)//chpark.11.27
            {
                m_bSU600_ErrorMsg_Show = false;
                SU600_ErrorMsg_Cnt=0;
                SU600_Show_Result_UI(SU600_RESET);
            }
        }
        //================================

		SU600_SecTimer = gMainTimerCnt_10m;
    }

    return true;
}

bool SU600_Send_TX(int nType)
{
    uint8_t  txbuffer[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

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

                txbuffer[4] = curUserIdx; //1~15
                CanbusTxDataManagement( gMainTimerCnt_10m,UM1,txbuffer);//chpark.10.30
                
                break;
        case 1:
           
            break;
        
        case 2:
                //printf("[SU600_Send_TX]  userlevel=%d ===\n",SU600_A_Level);
                //UM1
                //1byte:[0:3bit]
                if(g_nSU100_Mode==SU100_Add_Mode)
                {
                    txbuffer[0] = 0x02;
                }
                else
                {
                    txbuffer[0] = 0x00;
                }

                if(SU600_A_Level == SU100_USER_AD)
				    txbuffer[1] = 0x05;
                else
                    txbuffer[1] = 0x09;

                txbuffer[4] = curUserIdx; //1~15
				
                CanbusTxDataManagement( gMainTimerCnt_10m,UM1,txbuffer);//chpark.07.18
            break;

        case 3:
                //Start Register
                //UM1
                //When in user addition mode, set to [0byte 1bit]=1
                if(g_nSU100_Mode==SU100_Add_Mode)
                {
                    //0byte:[2bit] 1   0x00000110=>6    
                    txbuffer[0] = 0x06;
                }
                else
                {
                    //0byte:[2bit] 1   0x00000100=>4   
                    txbuffer[0] = 0x04;
                }

                txbuffer[2] = curUserIdx; //1~15//chpark.12.19
				
                CanbusTxDataManagement( gMainTimerCnt_10m,UM1,txbuffer);//chpark.08.22
                break;
        
        default:
            break;
    }
}
//======================================================



