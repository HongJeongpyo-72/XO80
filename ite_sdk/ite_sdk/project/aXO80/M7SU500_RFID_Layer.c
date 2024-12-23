#include <assert.h>

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

static bool m_bSU500_ErrorMsg_Show  = false;//chpark.24.02.20
int SU500_ErrorMsg_Cnt = 0;//chpark.24.02.20

static bool m_bSU500_AUI1_Rcv_Error = false;//chpark.24.02.14

//char *g_SU500_MSG_TEXT_ERROR_EXIST[5]; //"The RFID key is already registered."
//char *g_SU500_MSG_TEXT_ERROR_RETURN[5];//"Register passcode or RFID key."

static bool     m_bSU500ReadyToJOG = false;
static int      m_nSU500_ButtonIndex=0;//current selected key index. (0:SKip  1:D7) 
static uint32_t SU500_SecTimer;
static uint32_t SU500_ExitCNT = 0;
static uint32_t SU500_SendCNT = 0;

static bool     m_bSU500_GoTo_LayerMode = false;
static int      m_nSU500_GoTo_LayerModeCnt = 0;

static int      m_nSU500_Mode=0;
int  SU500_Result_UI_Status; //-1:none  0:X  1:O  2:X+text  3:O+text 


static char SU500_TxtUserName[50];
//=====================================
//normal mode
static ITUWidget* SU500_BG_KEY; //
//=====================================
static ITUWidget* SU500_TITLE[5]; //
//=====================================
static ITUWidget* SU500_TOP_ICON[2]; //
static ITUWidget* SU500_TOP_NAME; //
//=====================================
static ITUWidget* SU500_CONT_SKIP_BTN; //
static ITUWidget* SU500_SKIP_BTN[5]; //
static ITUWidget* SU500_SKIP_SEL; //
static ITUWidget* SU500_SKIP_DOWN; //chpark.01.15
//=====================================
static ITUWidget* SU500_CONT_ERROR_TXT; //
static ITUWidget* SU500_ERROR_TXT[5]; //
//=====================================
static ITUWidget* SU500_CONT_RESULT; //
static ITUWidget* SU500_RESULT_ICON[2]; //
//=====================================

//Error Mode
static ITUWidget* SU500_BG; //
static ITUWidget* SU500_CONT_RET;

static ITUWidget* SU500_TXT_RET[5]; //register msg.
static ITUWidget* SU500_BTN_RET[5]; //register button.
static ITUWidget* SU500_BTN_RET_SEL; //
static ITUWidget* SU500_BTN_RET_DOWN; //chpark.01.15
//=====================================

static ITUWidget* SU500_BTN_D7_SEL;   //D7 select
static ITUWidget* SU500_ICON_D7_DOWN; //D7 down


#define SU500_LANG_ENG  0
#define SU500_LANG_GER  1
#define SU500_LANG_FR   2
#define SU500_LANG_IT   3
#define SU500_LANG_SP   4

#define SU500_FAIL	        0
#define SU500_SUCCESS       1
#define SU500_FAIL_EXIST    2
#define SU500_FAIL_TIMEOUT  3
#define SU500_RESET	        10

#define SU500_NORMAL_MODE        0
#define SU500_REG_ERROR_MODE     1

#define SU500_NONE_BUTTON          50
#define SU500_SKIP_BUTTON          0
#define SU500_RETURN_BUTTON        1
#define SU500_D7_BUTTON            2

#define SU500_RCV_INIT      0
#define SU500_RCV_RFID_CHK  1

void SU500_InitControl();
void SU500_SetMode(int nType);

void SU500_Chk_RFID_result_FromCT();//Get RFID result info from controller
void SU500_SetUserTitle(uint8_t tUserIdx);
void SU500_Show_Result_UI(int nRes);
void SU500_MenuTabESC();
void SU500_MenuTabNext();

bool SU500_Button_Select(int nIndex,bool bDownImage);

bool SU500_BTN_MouseUp(ITUWidget* widget, char* param);

uint8_t SU500_UM5_0Byte_23bit;		//chpark.08.22
static uint8_t SU500_UM5_Rcv_Type=SU500_RCV_INIT;
static bool    m_bSU500_Chk_Success = true;
bool SU500_Send_TX(int nType);


bool SU500_Enter(ITUWidget* widget, char* param)
{
    m_nSU500_ButtonIndex = SU500_NONE_BUTTON;
    SU500_SecTimer = gMainTimerCnt_10m;
    m_nSU500_Mode  = SU500_NORMAL_MODE;
	SU500_ExitCNT  = 0;
    m_bSU500_GoTo_LayerMode = false;
    m_nSU500_GoTo_LayerModeCnt = 0;
    m_bSU500_ErrorMsg_Show  = false;
    
    SU500_UM5_Rcv_Type=SU500_RCV_INIT;

    memset(SU500_TxtUserName, '\0', sizeof(SU500_TxtUserName));
    SU500_InitControl();

    SU500_SetUserTitle(g_nSU100_CurUserIdx);

    SU500_SetMode(SU500_NORMAL_MODE);

    m_bSU500ReadyToJOG = true;

    SU500_Button_Select(m_nSU500_ButtonIndex,false);

    g_UM5_ChkTime = 0;
    SU500_UM5_Rcv_Type = SU500_RCV_RFID_CHK;

    m_bSU500_AUI1_Rcv_Error = false;        //chpark.24.02.14
    UserPW_Time_AUI1 = gMainTimerCnt_10m;	//chpark.24.02.14

    //chpark.24.03.08   
    SU500_Send_TX(0);

    return true;
}


void SU500_InitControl()
{
    int tIndex=0;
    char tmp_WidgetName[100];
//Text setting    
    //g_SU500_MSG_TEXT_ERROR_EXIST[SU500_LANG_ENG] = "The RFID key is already registered.";
    //g_SU500_MSG_TEXT_ERROR_EXIST[SU500_LANG_GER] = "The RFID key is already registered.";
    //g_SU500_MSG_TEXT_ERROR_EXIST[SU500_LANG_FR]  = "The RFID key is already registered.";
    //g_SU500_MSG_TEXT_ERROR_EXIST[SU500_LANG_IT]  = "The RFID key is already registered.";
    //g_SU500_MSG_TEXT_ERROR_EXIST[SU500_LANG_SP]  = "The RFID key is already registered.";

    //g_SU500_MSG_TEXT_ERROR_RETURN[SU500_LANG_ENG] = "Register passcode or RFID key.";
    //g_SU500_MSG_TEXT_ERROR_RETURN[SU500_LANG_GER] = "Register passcode or RFID key.";
    //g_SU500_MSG_TEXT_ERROR_RETURN[SU500_LANG_FR]  = "Register passcode or RFID key.";
    //g_SU500_MSG_TEXT_ERROR_RETURN[SU500_LANG_IT]  = "Register passcode or RFID key.";
    //g_SU500_MSG_TEXT_ERROR_RETURN[SU500_LANG_SP]  = "Register passcode or RFID key.";

//Control setting
    SetWidget(SU500_BG_KEY,"SU500_BG_KEY");
    
    SetWidget(SU500_TOP_ICON[SU100_ICON_AD],"SU500_T_Icon_Admin");
	SetWidget(SU500_TOP_ICON[SU100_ICON_ST],"SU500_T_Icon_Std");
	SetWidget(SU500_TOP_NAME,"SU500_T_Name");

    SetWidget(SU500_TITLE[SU500_LANG_ENG],"SU500_TITLE_ENG");
    SetWidget(SU500_TITLE[SU500_LANG_GER],"SU500_TITLE_GER");
    SetWidget(SU500_TITLE[SU500_LANG_FR], "SU500_TITLE_FR");
    SetWidget(SU500_TITLE[SU500_LANG_IT], "SU500_TITLE_IT");
    SetWidget(SU500_TITLE[SU500_LANG_SP], "SU500_TITLE_SP");
    
    SetWidget(SU500_CONT_SKIP_BTN,"SU500_CONT_SKIP_BTN");
    SetWidget(SU500_SKIP_BTN[SU500_LANG_ENG],"SU500_SKIP_BTN_ENG");
    SetWidget(SU500_SKIP_BTN[SU500_LANG_GER],"SU500_SKIP_BTN_GER");
    SetWidget(SU500_SKIP_BTN[SU500_LANG_FR], "SU500_SKIP_BTN_FR");
    SetWidget(SU500_SKIP_BTN[SU500_LANG_IT], "SU500_SKIP_BTN_IT");
    SetWidget(SU500_SKIP_BTN[SU500_LANG_SP], "SU500_SKIP_BTN_SP");
    
    SetWidget(SU500_SKIP_SEL,"SU500_SKIP_SEL");
    SetWidget(SU500_SKIP_DOWN,"SU500_SKIP_DOWN");

    SetWidget(SU500_CONT_ERROR_TXT, "SU500_CONT_ERROR_TXT");
    SetWidget(SU500_ERROR_TXT[SU500_LANG_ENG], "SU500_ERROR_ENG");
    SetWidget(SU500_ERROR_TXT[SU500_LANG_GER], "SU500_ERROR_GER");
    SetWidget(SU500_ERROR_TXT[SU500_LANG_FR],  "SU500_ERROR_FR");
    SetWidget(SU500_ERROR_TXT[SU500_LANG_IT],  "SU500_ERROR_IT");
    SetWidget(SU500_ERROR_TXT[SU500_LANG_SP],  "SU500_ERROR_SP");

    SetWidget(SU500_CONT_RESULT, "SU500_CONT_RESULT");
    SetWidget(SU500_RESULT_ICON[SU500_SUCCESS],"SU500_OK");
    SetWidget(SU500_RESULT_ICON[SU500_FAIL],   "SU500_NO");    

    //==============================================================
    //mode2
    SetWidget(SU500_BG,"SU500_BG");
    SetWidget(SU500_CONT_RET,"SU500_CONT_RET");
   
    SetWidget(SU500_TXT_RET[SU500_LANG_ENG], "SU500_TXT_RET_ENG");
    SetWidget(SU500_TXT_RET[SU500_LANG_GER], "SU500_TXT_RET_GER");
    SetWidget(SU500_TXT_RET[SU500_LANG_FR],  "SU500_TXT_RET_FR");
    SetWidget(SU500_TXT_RET[SU500_LANG_IT],  "SU500_TXT_RET_IT");
    SetWidget(SU500_TXT_RET[SU500_LANG_SP],  "SU500_TXT_RET_SP");

    SetWidget(SU500_BTN_RET[SU500_LANG_ENG], "SU500_BTN_RET_ENG");
    SetWidget(SU500_BTN_RET[SU500_LANG_GER], "SU500_BTN_RET_GER");
    SetWidget(SU500_BTN_RET[SU500_LANG_FR],  "SU500_BTN_RET_FR");
    SetWidget(SU500_BTN_RET[SU500_LANG_IT],  "SU500_BTN_RET_IT");
    SetWidget(SU500_BTN_RET[SU500_LANG_SP],  "SU500_BTN_RET_SP");
    
    SetWidget(SU500_BTN_RET_SEL,   "SU500_BTN_RET_SEL");
    SetWidget(SU500_BTN_RET_DOWN,  "SU500_BTN_RET_DOWN");//chpark.01.15
    //================================================================ 
// UI setting
    SetWidget(SU500_BTN_D7_SEL,     "SU500_ICON_D7_SEL");
    SetWidget(SU500_ICON_D7_DOWN,   "SU500_ICON_D7_DOWN");

    ituWidgetSetVisible(SU500_ICON_D7_DOWN, false);
    //===================================================================
}

void SU500_SetUserTitle(uint8_t tUserIdx)
{
	uint8_t i;
	uint8_t tDestName[100];
	
	//Set Icon
	if(User_RegInfo[tUserIdx].RegFlag==SU100_USER_AD || User_RegInfo[tUserIdx].RegFlag==SU100_USER_DEFAULT)
	{
		ituWidgetSetVisible(SU500_TOP_ICON[SU100_ICON_AD],true);
		ituWidgetSetVisible(SU500_TOP_ICON[SU100_ICON_ST],false);
	}
	else if(User_RegInfo[tUserIdx].RegFlag==SU100_USER_ST)
	{
		ituWidgetSetVisible(SU500_TOP_ICON[SU100_ICON_AD],false);
		ituWidgetSetVisible(SU500_TOP_ICON[SU100_ICON_ST],true);
    }
	else
    {
        ituWidgetSetVisible(SU500_TOP_ICON[SU100_ICON_AD],false);
		ituWidgetSetVisible(SU500_TOP_ICON[SU100_ICON_ST],false);
    }

    if(g_UI1_ChkTime>0)
	{
        memset(tDestName,0,100);

		ituWidgetSetVisible(SU500_TOP_NAME,true);
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[tUserIdx].UserName);
		ituTextSetString(SU500_TOP_NAME, tDestName);
	}
	else if((g_UI1_ChkTime == 0)||(M7_UserIndex > (MAX_SU100_USER_NUM-1)))
	{
		ituWidgetSetVisible(SU500_TOP_NAME,false);
	}
}

void SU500_SetMode(int nType)
{
    //mode setting
    m_nSU500_Mode = nType;
   
    for(int i=0; i<5; i++)
    {
        ituWidgetSetVisible(SU500_TITLE[i],         false);
        ituWidgetSetVisible(SU500_SKIP_BTN[i],      false);
    }   
    
    if(nType==SU500_NORMAL_MODE)
    {
        //
        ituWidgetSetVisible(SU500_BG_KEY,true);  
        ituWidgetSetVisible(SU500_BG,    false);  
        //
        ituWidgetSetVisible(SU500_TITLE[Lang_Flag[M7_UserIndex]],true);  //title
        //
        ituWidgetSetVisible(SU500_CONT_SKIP_BTN,    true);  //skip button
        ituWidgetSetVisible(SU500_CONT_ERROR_TXT,   false); //error msg

        ituWidgetSetVisible(SU500_SKIP_BTN[Lang_Flag[M7_UserIndex]],true);
        //register button. off.
        ituWidgetSetVisible(SU500_CONT_RET,false); 
        //
        ituWidgetSetVisible(SU500_CONT_RESULT,false);//result image (o,X)
        //
        ituWidgetSetVisible(SU500_SKIP_SEL,     true);//result image (o,X)
        ituWidgetSetVisible(SU500_BTN_RET_SEL,  false); //focus
        ituWidgetSetVisible(SU500_BTN_RET_DOWN,  false); //chpark.01.15
        ituWidgetSetVisible(SU500_ICON_D7_DOWN, false);
        ituWidgetSetVisible(SU500_BTN_D7_SEL,   false);
    }
    else if(nType==SU500_REG_ERROR_MODE)
    {
        //
        ituWidgetSetVisible(SU500_BG_KEY,false);  
        
        ituWidgetSetVisible(SU500_CONT_SKIP_BTN,    false); 
        ituWidgetSetVisible(SU500_CONT_ERROR_TXT,   false); 
        ituWidgetSetVisible(SU500_CONT_RESULT,      false); 

        //no title txt

        //register button. on.
        ituWidgetSetVisible(SU500_BG,true);  
        
        ituWidgetSetVisible(SU500_CONT_RET,true); 

        for(int i=0; i<5; i++)
        {
            ituWidgetSetVisible(SU500_TXT_RET[i],      false);
            ituWidgetSetVisible(SU500_BTN_RET[i],      false);
        }   
        
        ituWidgetSetVisible(SU500_TXT_RET[Lang_Flag[M7_UserIndex]],  true);//chpark.24.01.31
        
        ituWidgetSetVisible(SU500_BTN_RET[Lang_Flag[M7_UserIndex]],  true);//return button

        ituWidgetSetVisible(SU500_SKIP_SEL,     false);//result image (o,X)
        ituWidgetSetVisible(SU500_BTN_RET_SEL,  true); //focus
        ituWidgetSetVisible(SU500_ICON_D7_DOWN, false);
        ituWidgetSetVisible(SU500_BTN_D7_SEL,   false);
    }
}



void SU500_Show_Result_UI(int nRes)
{
    //int  SU400_Result_UI_Status; //-1:none  0:X  1:O  2:X+text  3:O+text 
    //Show O,X UI
    SU500_Result_UI_Status = nRes;
    m_bSU500_ErrorMsg_Show = true;
    SU500_ErrorMsg_Cnt=0;

    switch(SU500_Result_UI_Status)
    {
        case SU500_RESET: //Reset
            m_bSU500_ErrorMsg_Show = false;
            ituWidgetSetVisible(SU500_CONT_ERROR_TXT, false);       
            ituWidgetSetVisible(SU500_CONT_RESULT, false);
            break;

        case SU500_FAIL://Fail . Only X
        case SU500_FAIL_TIMEOUT:
            //Error Message
            ituWidgetSetVisible(SU500_CONT_ERROR_TXT, false);
           
            //X Icon.    
            ituWidgetSetVisible(SU500_CONT_RESULT, true);
            ituWidgetSetVisible(SU500_RESULT_ICON[SU500_SUCCESS], false);
            ituWidgetSetVisible(SU500_RESULT_ICON[SU500_FAIL], true);
            break;

        case SU500_SUCCESS://Success
            //Error Message
            ituWidgetSetVisible(SU500_CONT_ERROR_TXT, false);
            
            //V Icon.    
            ituWidgetSetVisible(SU500_CONT_RESULT, true);
            ituWidgetSetVisible(SU500_RESULT_ICON[SU500_SUCCESS], true);
            ituWidgetSetVisible(SU500_RESULT_ICON[SU500_FAIL], false);
            break;

        case SU500_FAIL_EXIST://Fail : the RFID key is already registered.
            //Error Message
            ituWidgetSetVisible(SU500_CONT_ERROR_TXT, true);//chpark.11.20
            for(int i=0;i<5;i++)
            {
                ituWidgetSetVisible(SU500_ERROR_TXT[i], false);    
            }
            ituWidgetSetVisible(SU500_ERROR_TXT[Lang_Flag[M7_UserIndex]], true);

            //X Icon.    
            ituWidgetSetVisible(SU500_CONT_RESULT, true);
            ituWidgetSetVisible(SU500_RESULT_ICON[SU500_SUCCESS], false);
            ituWidgetSetVisible(SU500_RESULT_ICON[SU500_FAIL], true);
            break;
       

        default:
            m_bSU500_ErrorMsg_Show = false;
            ituWidgetSetVisible(SU500_CONT_ERROR_TXT, false);  
             
            ituWidgetSetVisible(SU500_CONT_RESULT, false);
            break;
    }
}

void SU500_MenuTabESC()
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
            ITULayer *M7_SU400_PASS = ituSceneFindWidget(&theScene, "M7_SU400_PASS");
            assert(M7_SU400_PASS);
            ituLayerGoto(M7_SU400_PASS);
        }
    }
    else if(g_nSU100_Mode == SU100_Edit_Mode)
    {
         ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
        assert(M7_SU200_EDIT);
        ituLayerGoto(M7_SU200_EDIT);
    }
}


bool SU500_JogWheel(ITUWidget* widget, char* param)
{
    if(!m_bSU500ReadyToJOG || m_bSU500_GoTo_LayerMode)return false;

    SU500_ExitCNT = 0;
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
                //printf("SU500 JogWheel[R] %d\r\n",JogDir);
                SU500_MenuTabNext();
			}
			else if(JogDir == JOG_DIAL_ROT_LEFT)
			{
                //printf("SU500 JogWheel[L] %d\r\n",JogDir);
                SU500_MenuTabNext();
			}

		break;
	}

    return true;
}

bool SU500_JogButton(ITUWidget* widget, char* param)
{
    if(!m_bSU500ReadyToJOG || m_bSU500_GoTo_LayerMode)return false;

    SU500_ExitCNT = 0;
    uint8_t JogButton = 0;
	JogButton = param[0];

	//printf("SU500_JogButton %d\r\n",JogButton);

	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
        case LANDSCAPE_1:
		case LANDSCAPE_2:
			
			switch(JogButton)
			{
				case JOG_DIAL_KEY_UP:
				case JOG_DIAL_KEY_DOWN:
                    if(m_nSU500_Mode==SU500_NORMAL_MODE)
                    {
                        if(m_nSU500_ButtonIndex==SU500_SKIP_BUTTON)
                        {
                            m_nSU500_ButtonIndex = SU500_NONE_BUTTON;
                        }
                        else
                        {
                            m_nSU500_ButtonIndex = SU500_SKIP_BUTTON;
                        }
                    }
                    else if(m_nSU500_Mode==SU500_REG_ERROR_MODE)     
                    {
                        if(m_nSU500_ButtonIndex==SU500_RETURN_BUTTON)
                        {
                            m_nSU500_ButtonIndex = SU500_NONE_BUTTON;
                        }
                        else
                        {
                            m_nSU500_ButtonIndex = SU500_RETURN_BUTTON;
                        }
                    }

                    SU500_Button_Select(m_nSU500_ButtonIndex, false);
                
                    break;
				case JOG_DIAL_KEY_LEFT:
				case JOG_DIAL_KEY_RIGHT:
                    if(m_nSU500_Mode==SU500_NORMAL_MODE)//chpark.11.02
                    {
                        if(m_nSU500_ButtonIndex==SU500_SKIP_BUTTON)
                        {
                            m_nSU500_ButtonIndex = SU500_NONE_BUTTON;
                        }
                        else
                        {
                            m_nSU500_ButtonIndex = SU500_SKIP_BUTTON;
                        }  
                    }
                    else if(m_nSU500_Mode==SU500_REG_ERROR_MODE)     
                    {
                        if(m_nSU500_ButtonIndex==SU500_RETURN_BUTTON)
                        {
                            m_nSU500_ButtonIndex = SU500_NONE_BUTTON;
                        }
                        else
                        {
                            m_nSU500_ButtonIndex = SU500_RETURN_BUTTON;
                        }
                    }

                    SU500_Button_Select(m_nSU500_ButtonIndex, false);

                    //printf("m_nSU500_Mode=%d  m_nSU500_ButtonIndex=%d ===\n",m_nSU500_Mode,m_nSU500_ButtonIndex);

                    break;
				
                case JOG_DIAL_KEY_ENTER_ON:
                {
                    SU500_Button_Select(m_nSU500_ButtonIndex, true);
                }
				break;

				case JOG_DIAL_KEY_ENTER_OFF:				
        		case JOG_DIAL_KEY_NONE:
				{
                    ituWidgetSetVisible(SU500_ICON_D7_DOWN, false);

                    int nTmpKeyValue=0;
                    if(m_nSU500_Mode==SU500_NORMAL_MODE)
                    {
                        nTmpKeyValue = m_nSU500_ButtonIndex;
                    }
                    else if(m_nSU500_Mode==SU500_REG_ERROR_MODE)     
                    {
                        nTmpKeyValue = m_nSU500_ButtonIndex;
                    }
                    
                    char TMP_PARA[10];
                    
                    memset(TMP_PARA,0,10);
                    sprintf(TMP_PARA,"%d",nTmpKeyValue);
                    
                    SU500_BTN_MouseUp(NULL, TMP_PARA);
        		}
				break;
			}
		break;
	}

    return true;
}

bool SU500_JogSwitch(ITUWidget* widget, char* param)
{
    if(!m_bSU500ReadyToJOG || m_bSU500_GoTo_LayerMode)return false;

    SU500_ExitCNT = 0;
	uint8_t JogSwitch = 0;
	JogSwitch = param[0];
	
	if(JogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
        SU500_MenuTabESC();
	}
	else if(JogSwitch == JOG_DIAL_SW_HOME_OFF)
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}

    return true;
}


void SU500_MenuTabNext()
{
    if(m_nSU500_Mode==SU500_NORMAL_MODE)
    {
        if(m_nSU500_ButtonIndex==SU500_SKIP_BUTTON)
        {
            m_nSU500_ButtonIndex = SU500_NONE_BUTTON;
        }
        else
        {
            m_nSU500_ButtonIndex = SU500_SKIP_BUTTON;
        }
    }
    else if(m_nSU500_Mode==SU500_REG_ERROR_MODE)     
    {
        if(m_nSU500_ButtonIndex==SU500_RETURN_BUTTON)
        {
            m_nSU500_ButtonIndex = SU500_NONE_BUTTON;
        }
        else
        {
            m_nSU500_ButtonIndex = SU500_RETURN_BUTTON;
        }
    }

    SU500_Button_Select(m_nSU500_ButtonIndex, false);
}

//======================================================
bool SU500_BTN_MouseDn(ITUWidget* widget, char* param)
{
    uint8_t i,tBTN_Index = 0;
	
	SU500_ExitCNT = 0;
	tBTN_Index = (uint8_t)atoi(param);

    //chpark.24.02.20
    if(m_bSU500_GoTo_LayerMode) return false;
    if(m_bSU500_ErrorMsg_Show)
    {
         return false;
    }

    //down image
    if(tBTN_Index==SU500_SKIP_BUTTON)
    {
        ituWidgetSetVisible(SU500_SKIP_SEL, true);
        ituWidgetSetVisible(SU500_SKIP_DOWN, true);//chpark.01.15
    }
    else if(tBTN_Index==SU500_RETURN_BUTTON)
    {
        ituWidgetSetVisible(SU500_BTN_RET_SEL,true); 
        ituWidgetSetVisible(SU500_BTN_RET_DOWN,  true); //chpark.01.15
    }
        
    return true;
}

bool SU500_BTN_MouseUp(ITUWidget* widget, char* param)
{
    uint8_t i,tBTN_Index = 0;
	
	SU500_ExitCNT = 0;
	tBTN_Index = (uint8_t)atoi(param);
    //printf("[SU500_BTN_MouseUp]tBTN_Index=%d\n",tBTN_Index);

    //chpark.24.02.20
    if(m_bSU500_GoTo_LayerMode) return false;
    if(m_bSU500_ErrorMsg_Show)
    {
        if(tBTN_Index==SU500_SKIP_BUTTON)
        {
            ituWidgetSetVisible(SU500_SKIP_SEL, false);
            ituWidgetSetVisible(SU500_SKIP_DOWN, false);
            //printf("[SU500_BTN_MouseUp]SU500_SKIP_BUTTON\n");
        }
        else if(tBTN_Index==SU500_RETURN_BUTTON)//return button
        {
            ituWidgetSetVisible(SU500_BTN_RET_SEL,false); 
            ituWidgetSetVisible(SU500_BTN_RET_DOWN,  false); 
           // printf("[SU500_BTN_MouseUp]SU500_RETURN_BUTTON\n");
        }
        return false;
    }
    //

    //skip button
    if(tBTN_Index==SU500_SKIP_BUTTON)
    {
        ituWidgetSetVisible(SU500_SKIP_SEL, false);
        ituWidgetSetVisible(SU500_SKIP_DOWN, false);//chpark.01.15

        if(g_nSU100_Mode == SU100_Add_Mode)
        {
            //chpark.11.27
            if(g_nSU100_PreLayer==M7_SU200_EDIT_LAYER)
            {
                ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
                assert(M7_SU200_EDIT);
                ituLayerGoto(M7_SU200_EDIT);
            }
            else
            {
                //already skip passcode.
                if(User_RegInfo[g_nSU100_AddUserIdx].nTmp_PassCode_Skip==1)
                {
                    SU500_SetMode(SU500_REG_ERROR_MODE);
                    SU500_Send_TX(0);//chpark.24.03.18
                }
                else if(User_RegInfo[g_nSU100_AddUserIdx].PassNOD==0 && User_RegInfo[g_nSU100_AddUserIdx].RFStatus==0)
                {
                    //chpark.24.03.13
                    #ifdef TEST_USER_SET_ICON		// 	JPHong2024-02-07	Menu7_Layer	M7_9_ZoomBTN
		                ITULayer *M7_SU600_LEVEL = ituSceneFindWidget(&theScene, "M7_SU600_LEVEL");
                        assert(M7_SU600_LEVEL);
                        ituLayerGoto(M7_SU600_LEVEL);

                        return true;
	                #endif

                    SU500_SetMode(SU500_REG_ERROR_MODE);
                    SU500_Send_TX(0);//chpark.24.03.18
                }
                else if(User_RegInfo[g_nSU100_AddUserIdx].nTmp_PassCode_Skip==0)//already passcode registered &  RFID Skip
                {
                    ITULayer *M7_SU600_LEVEL = ituSceneFindWidget(&theScene, "M7_SU600_LEVEL");
                    assert(M7_SU600_LEVEL);
                    ituLayerGoto(M7_SU600_LEVEL);
                }
                else if(m_bSU500_GoTo_LayerMode)
                {
                    //move level setting.
                }
                else
                {
                    //fail & skip button
                }
            }
        }
        else if(g_nSU100_Mode == SU100_Edit_Mode)
        {
            ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
            assert(M7_SU200_EDIT);
            ituLayerGoto(M7_SU200_EDIT);
        }

    }
    else if(tBTN_Index==SU500_RETURN_BUTTON)//return button
    {
        ituWidgetSetVisible(SU500_BTN_RET_SEL,false); 
        ituWidgetSetVisible(SU500_BTN_RET_DOWN,  false); //chpark.01.15

        ITULayer *M7_SU400_PASS = ituSceneFindWidget(&theScene, "M7_SU400_PASS");
        assert(M7_SU400_PASS);
        ituLayerGoto(M7_SU400_PASS);
    }
    
    return true;
}

bool SU500_D7_MouseUp(ITUWidget* widget, char* param)
{
    //chpark.24.02.20
    if(m_bSU500_GoTo_LayerMode) return false;
    if(m_bSU500_ErrorMsg_Show) return false;

    if(m_nSU500_Mode==SU500_NORMAL_MODE)
    {
        ituWidgetSetVisible(SU500_SKIP_SEL, false);
        ituWidgetSetVisible(SU500_SKIP_DOWN, false);//chpark.01.05
        SU500_ExitCNT = 0;

        //chpark.10.23
        if(!gUI_Server_Mode)
        {
            //Success
            //V only
            SU500_Show_Result_UI(SU500_SUCCESS);
            //save data
            if(g_nSU100_Mode == SU100_Add_Mode)
            {
                User_RegInfo[g_nSU100_AddUserIdx].RFStatus=1;
            }
            else if(g_nSU100_Mode == SU100_Edit_Mode)
            {
                User_RegInfo[g_nSU100_EditUserIdx].RFStatus=1;
            }
            
            //move to level setting
            m_bSU500_GoTo_LayerMode = true;
        }
    }
    else if(m_nSU500_Mode==SU500_REG_ERROR_MODE)
    {
        ituWidgetSetVisible(SU500_BTN_RET_SEL,false); 
        ituWidgetSetVisible(SU500_BTN_RET_DOWN,  false); //chpark.01.15
        SU500_ExitCNT = 0;
        ITULayer *M7_SU400_PASS = ituSceneFindWidget(&theScene, "M7_SU400_PASS");
        assert(M7_SU400_PASS);
        ituLayerGoto(M7_SU400_PASS);
    }

    return true;
}

bool SU500_D6_MouseUp(ITUWidget* widget, char* param)
{
    //chpark.24.02.20
    if(m_bSU500_GoTo_LayerMode) return false;
    if(m_bSU500_ErrorMsg_Show)
    { 
        return false;
    }

    SU500_ExitCNT = 0;
    ituWidgetSetVisible(SU500_SKIP_SEL, false);
    ituWidgetSetVisible(SU500_SKIP_DOWN, false);//chpark.01.15
    ituWidgetSetVisible(SU500_BTN_RET_SEL,false);
    ituWidgetSetVisible(SU500_BTN_RET_DOWN,  false); //chpark.01.15

    SU100_Send_Add_Cancel();//chpark.10.30
	GotoMain(MAIN_MODE_NORMAL); //

    return true;
}

bool SU500_D5_MouseUp(ITUWidget* widget, char* param)
{
    //chpark.24.02.20
    if(m_bSU500_ErrorMsg_Show) return false;
    if(m_bSU500_GoTo_LayerMode) return false;

    SU500_ExitCNT = 0;
    ituWidgetSetVisible(SU500_SKIP_SEL, false);
    ituWidgetSetVisible(SU500_SKIP_DOWN, false);//chpark.01.15
    ituWidgetSetVisible(SU500_BTN_RET_SEL,false);
    ituWidgetSetVisible(SU500_BTN_RET_DOWN,  false); //chpark.01.15

    SU500_MenuTabESC();

    return true;
}

bool SU500_Button_Select(int nIndex,bool bDownImage)
{
    SU500_ExitCNT = 0;

    ituWidgetSetVisible(SU500_SKIP_SEL,     false);
    ituWidgetSetVisible(SU500_SKIP_DOWN,     false);//chpark.01.15
    ituWidgetSetVisible(SU500_BTN_RET_SEL,  false);
    ituWidgetSetVisible(SU500_BTN_RET_DOWN,  false); //chpark.01.15
    ituWidgetSetVisible(SU500_BTN_D7_SEL,   false);
    ituWidgetSetVisible(SU500_ICON_D7_DOWN,   false);
    
    switch(nIndex)
    {
        case SU500_SKIP_BUTTON:
            ituWidgetSetVisible(SU500_SKIP_SEL, true);
            if(bDownImage)//chpark.01.15
                ituWidgetSetVisible(SU500_SKIP_DOWN, true);
            else
                ituWidgetSetVisible(SU500_SKIP_DOWN, false);
            break;

        case SU500_RETURN_BUTTON:
            ituWidgetSetVisible(SU500_BTN_RET_SEL, true);
            if(bDownImage)//chpark.01.15
                ituWidgetSetVisible(SU500_BTN_RET_DOWN,  true); 
            else
                ituWidgetSetVisible(SU500_BTN_RET_DOWN,  false); 

            break; 

        case SU500_D7_BUTTON:
            if(bDownImage)
                ituWidgetSetVisible(SU500_ICON_D7_DOWN, true);
            else
                ituWidgetSetVisible(SU500_ICON_D7_DOWN, false);

            ituWidgetSetVisible(SU500_BTN_D7_SEL, true);
            break;

        case SU500_NONE_BUTTON:
            ituWidgetSetVisible(SU500_SKIP_SEL, false);
            ituWidgetSetVisible(SU500_SKIP_DOWN, false);//chpark.01.15
            ituWidgetSetVisible(SU500_BTN_RET_SEL, false);
            ituWidgetSetVisible(SU500_BTN_RET_DOWN, false);//chpark.01.15
            ituWidgetSetVisible(SU500_BTN_D7_SEL, false);
            break;

        default:
            break;
    }

    return true;
}
//========================================================================

bool SU500_Timer(ITUWidget* widget, char* param)
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
	    if(!m_bSU500_AUI1_Rcv_Error && !m_bSU500_GoTo_LayerMode && gUI_Server_Mode)
		{
            if(m_nSU500_Mode!=SU500_REG_ERROR_MODE)//chpark.24.02.29    
            {
                //printf("\r\n AUI1 Protocol 5Sec Exit >>>>>>> goto Main\r\n");

                ituWidgetSetVisible(SU500_CONT_RESULT, true);
                ituWidgetSetVisible(SU500_RESULT_ICON[SU500_SUCCESS], false);
                ituWidgetSetVisible(SU500_RESULT_ICON[SU500_FAIL], true);

                m_bSU500_AUI1_Rcv_Error = true;
                m_bSU500_GoTo_LayerMode = true;
            }
        }
	}
    #endif

    
    if((gMainTimerCnt_10m - SU500_SecTimer) >= 20)//100)//200msec
	{
		if(SU500_ExitCNT < 30*5)//30sec
			SU500_ExitCNT++;
		else if(SU500_ExitCNT == 30*5)//30sec
		{
			SU500_ExitCNT = 300;
            #ifdef RELEASE_VER
                SU100_Send_Add_Cancel();//chpark.10.30
                //printf("\r\n SU500 Menu 25Sec Exit > goto Main\r\n");
                GotoMain(MAIN_MODE_NORMAL); 
                return true;
	        #endif
		}

        //check RFID Key status
        if(gUI_Server_Mode)
		{
            SU500_Chk_RFID_result_FromCT();//chpark.10.30 
        }
        //

        //if(SU500_ExitCNT>25*5)//25sec
        //{
            //chaprk.24.02.20
            //register time out.-> X Message
            //if(m_nSU500_Mode==SU500_NORMAL_MODE)
            //{
                //SU500_Show_Result_UI(SU500_FAIL_TIMEOUT);
            //}   
        //}

        if(m_bSU500_GoTo_LayerMode)
		{
			m_nSU500_GoTo_LayerModeCnt++;

			if(m_nSU500_GoTo_LayerModeCnt>=2*5)//2sec
			{
				m_bSU500_GoTo_LayerMode = false;
				m_nSU500_GoTo_LayerModeCnt=0;

                if(m_bSU500_AUI1_Rcv_Error)
                {
                    ituWidgetSetVisible(SU500_CONT_RESULT, false);//chpark.24.02.14
                    m_bSU500_AUI1_Rcv_Error = false;
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
                        g_nSU100_PreLayer = M7_SU500_RFID_LAYER; 
                        //data save.

                        //move 
                        ITULayer *M7_SU600_LEVEL = ituSceneFindWidget(&theScene, "M7_SU600_LEVEL");
                        assert(M7_SU600_LEVEL);
                        ituLayerGoto(M7_SU600_LEVEL);
                    }
                }
                else if(g_nSU100_Mode==SU100_Edit_Mode)
                {
                    //data save
                                        
                    //move
                    ITULayer *M7_SU200_EDIT = ituSceneFindWidget(&theScene, "M7_SU200_EDIT");
                    assert(M7_SU200_EDIT);
                    ituLayerGoto(M7_SU200_EDIT);
                }
			}			
		}

        //======
        //data received chpark.08.22
        SU500_SendCNT++;
		
		if(SU500_SendCNT>=5)//1sec
		{
			SU500_SendCNT=0;
		}
        //=====

        //================================
        //24.02.20
        if(m_bSU500_ErrorMsg_Show)
        {
            //printf("SU500_ErrorMsg_Cnt==>%d\n",SU500_ErrorMsg_Cnt);
            SU500_ErrorMsg_Cnt++;
            
            if(SU500_ErrorMsg_Cnt>=2*5)
            {
                m_bSU500_ErrorMsg_Show = false;
                SU500_ErrorMsg_Cnt=0;
                SU500_Show_Result_UI(SU500_RESET);
            }
        }
        //================================

		SU500_SecTimer = gMainTimerCnt_10m;
    }

    return true;
}

void SU500_Chk_RFID_result_FromCT()//Get RFID result info from controller
{
    if( g_UM5_ChkTime>0 && !m_bSU500_GoTo_LayerMode && !m_bSU500_ErrorMsg_Show)
    {      
        g_UM5_ChkTime = 0;
        
        if(SU500_UM5_Rcv_Type == SU500_RCV_RFID_CHK)
        {
                SU500_UM5_Rcv_Type = SU500_RCV_INIT; 
                printf("[SU500]RFID register result :%d \n",SU500_UM5_0Byte_23bit);
                
                if(SU500_UM5_0Byte_23bit==0)// N/A
                {
                   //printf("[RFID] Error in RFID Return [N/A]: %d \n",SU500_UM5_0Byte_23bit);
                }
                else if(SU500_UM5_0Byte_23bit== 1)//RFID register ok
                {
                    //Success
                    //Buzzer
                    gBuzzerMode = 11; //ON(300) 1Time //chpark.24.10.23
                    //V only
                    SU500_Show_Result_UI(SU500_SUCCESS);
                    //save data
                    if(g_nSU100_Mode == SU100_Add_Mode)
                    {
                        User_RegInfo[g_nSU100_AddUserIdx].RFStatus=1;
                    }
                    else if(g_nSU100_Mode == SU100_Edit_Mode)
                    {
                        User_RegInfo[g_nSU100_EditUserIdx].RFStatus=1;              
                    }
                        
                    //move to level setting
                    m_bSU500_GoTo_LayerMode = true;

                    printf("[SU500]RFID register OK:%d \n",SU500_UM5_0Byte_23bit);
                }
                else if(SU500_UM5_0Byte_23bit== 2)//already registered RFID
                {
                    //Fail : the RFID key is already registered.
                    gBuzzerMode = 12; //ON(300)OFF(300) 3Time //chpark.24.10.23

                    SU500_Show_Result_UI(SU500_FAIL_EXIST);

                    g_UM5_ChkTime = 0;
                    SU500_UM5_Rcv_Type = SU500_RCV_RFID_CHK;
                    SU500_Send_TX(1);//chpark.24.02.20
                }
                else if(SU500_UM5_0Byte_23bit== 3)//time out
                {
                    //Fail : time out
                    SU500_Show_Result_UI(SU500_FAIL_TIMEOUT);//Fail . Only X

                    g_UM5_ChkTime = 0;
                    SU500_UM5_Rcv_Type = SU500_RCV_RFID_CHK;
                    SU500_Send_TX(2);//chpark.24.02.20
                }
                else
                {
                    printf("[RFID] Error in RFID Return : %d \n",SU500_UM5_0Byte_23bit);
                }
        } 
    }

}


bool SU500_Send_TX(int nType)
{
    uint8_t  txbuffer[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
 
    printf("[SU500_Send_TX]\n");
    
    uint8_t curUserIdx;

    if(g_nSU100_Mode==SU100_Add_Mode)
    {
       curUserIdx = g_nSU100_AddUserIdx+1;
    }
    else if(g_nSU100_Mode==SU100_Edit_Mode)
    {
       curUserIdx = g_nSU100_EditUserIdx+1;
    }

    switch(nType)
    {
        case 0:
                //Enter:UM1  
                //(1)set to [0byte 1bit]=1 (in add mode), =0(in edid mode)
                //(2)0byte:[3:4bit] 0x01:  0000 1000        
                //(3)3byte:[0:7bit] user idx
                if(g_nSU100_Mode==SU100_Add_Mode)
                {
                    txbuffer[0] = (0x01<<3)| 0x02; //modify req. //0000 1010 : 0x0A
                }
                else
                {
                    txbuffer[0] = 0x01<<3; //modify req. //0000 1000 : 0x08
                }
                
				txbuffer[3] = curUserIdx; //1~15
				
                CanbusTxDataManagement( gMainTimerCnt_10m,UM1,txbuffer);//chpark.8.22
                
                break;
        case 1:
                //RFID KEY Duplicate
                //UM1  
                //0byte:[3:4bit] 0x01:  0000 1000
                //3byte:[0:7bit] user idx
                if(g_nSU100_Mode==SU100_Add_Mode)
                {
                    txbuffer[0] = (0x01<<3)| 0x02; //modify req. //0000 1010 : 0x0A
                }
                else
                {
                    txbuffer[0] = 0x01<<3; //modify req. //0000 1000 : 0x08
                }
                
				txbuffer[3] = curUserIdx; //1~15
				
                CanbusTxDataManagement( gMainTimerCnt_10m,UM1,txbuffer);//chpark.8.22
            break;

        case 2:
                //RFID Time out
                //UM1  
                //0byte:[3:4bit] 0x01:  0000 1000
                //3byte:[0:7bit] user idx
                if(g_nSU100_Mode==SU100_Add_Mode)
                {
                    txbuffer[0] = (0x00<<3)| 0x02; //modify req. //0000 1010 : 0x0A
                }
                else
                {
                    txbuffer[0] = 0x00<<3; //modify req. //0000 1000 : 0x08
                }
                
				txbuffer[3] = curUserIdx; //1~15
				
                CanbusTxDataManagement( gMainTimerCnt_10m,UM1,txbuffer);//chpark.8.22
            break;

        default:
            break;
    }
}

//========================================================================
