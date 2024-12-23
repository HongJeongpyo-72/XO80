#include <assert.h>

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

static bool m_bSU200_AUI1_Rcv_Error = false;//chpark.24.02.14

//0:Name 1:Passcode 2:RFID 3:Access Level 4:Access AUX 5:Register 
static int      m_nSU200_ButtonIndex=0;//current selected key index.
static bool     m_bSU200ReadyToJOG = false;

static uint32_t SU200_SecTimer;
static uint32_t SU200_ExitCNT = 0;
static uint32_t SU200_SendCNT = 0;
static uint8_t  SU200_A_Level;//1:admin 2:standard
static uint8_t  SU200_A_AUX;  //1:yes 0:no
static bool     m_bSU200_GoTo_LayerMode   = false;
static int      m_nSU200_GoTo_LayerModeCnt= 0;



#define SU200_LANG_ENG 0
#define SU200_LANG_GER 1
#define SU200_LANG_FR 2
#define SU200_LANG_IT 3
#define SU200_LANG_SP 4

#define SU200_SUCCESS	1
#define SU200_FAIL		0

#define SU200_NO	0
#define SU200_YES	1

#define SU200_MAX_PW_CNT    9

#define SU200_SEL_NAME          0
#define SU200_SEL_PASSCODE      1
#define SU200_SEL_RFID          2
#define SU200_SEL_LEVEL         3
#define SU200_SEL_AUX           4
#define SU200_SEL_RG            5       

#define SU200_RCV_INIT           0
#define SU200_RCV_REGISTER_CHK   1


//=====================================
static char SU200_TxtUserName[50];
//=====================================
static char SU200_Txt_PassCode[15];
//=====================================
//static ITUWidget* SU200_TITLE[5]; //not use.
//=====================================
static ITUWidget* SU200_TOP_ICON[2]; //
static ITUWidget* SU200_TOP_NAME;    //
//=====================================
static ITUWidget* SU200_CONT_TXT;   //
static ITUWidget* SU200_TXT[5];     //      //Background Text
//=====================================

static ITUWidget* SU200_TEXT_NAME;          //(0)Name
//--------------------------------------    
static ITUWidget* SU200_UserPW_Data[SU200_MAX_PW_CNT];     //(1)passcode dot
static ITUWidget* SU200_UserPX_Data[SU200_MAX_PW_CNT];
//=====================================
static ITUWidget* SU200_CHK_RFID;           //(2)RFID
//=====================================
//static ITUWidget* SU200_ACCESS_LEVEL[2];    //(3)Access Level
static ITUWidget* SU200_CONT_LEVEL[2];// 0:admin 1:standard //24.01.24
static ITUWidget* SU200_LEVEL_ICON_AD[5];//24.01.24
static ITUWidget* SU200_LEVEL_ICON_ST[5];//24.01.24
//=====================================
static ITUWidget* SU200_CONT_AUX;           //(4)Access AUX

static ITUWidget* SU200_CONT_AUX_TXT;
static ITUWidget* SU200_AUX_TXT[5];

//static ITUWidget* SU200_AUX_ICON[2];      //yes no
static ITUWidget* SU200_CONT_AUX_YESNO[2];// 0:no 1:yes //24.01.24
static ITUWidget* SU200_AUX_ICON_NO[5];//24.01.24
static ITUWidget* SU200_AUX_ICON_YES[5];//24.01.24
//=====================================
static ITUWidget* SU200_CONT_RG;            //(5)Register Button  
static ITUWidget* SU200_BTN_RG[5];          //(5)Register Button  
static ITUWidget* SU200_RG_DOWN; //chpark.01.15
//=====================================
static ITUWidget* SU200_SEL[10];            //(0)Name (1)Passcode (2)RFID (3)Access Level (4)Access AUX
//=====================================

void SU200_InitControl();
void SU200_SetUserTitle(uint8_t tUserIdx);

bool SU200_Button_Select(int nIndex,bool bDownImage);

void SU200_MenuTabNext();
void SU200_MenuTabPrev();
void SU200_MenuTabUp();
void SU200_MenuTabDown();
void SU200_MenuTabRight();
void SU200_MenuTabLeft();
void SU200_MenuTabESC();

void SU200_MenuTabGoto();

void SU200_SetUserName();
void SU200_Display_PassCode();
void SU200_Show_InCenter(int nLength);

void SU200_Chk_Result(int nSuc);

uint8_t  SU200_Chk_User_RFID(int nUserIdx);
uint8_t  SU200_Chk_User_Level(int nUserIdx);

uint8_t  SU200_UM5_1Byte_67bit;		//Register ACK:[0:N/A 1:success 2:fail 3:N/A]
bool SU200_Send_TX(int nType);
static uint8_t SU200_UM5_Rcv_Type=SU200_RCV_INIT;
static bool    m_bSU200_Chk_Success = true;

bool SU200_D7_MouseUp(ITUWidget* widget, char* param);//chpark.10.23

static bool SU200_b_VMode = false;//chpark.24.04.16

bool SU200_Enter(ITUWidget* widget, char* param)
{
    if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
	{
        SU200_b_VMode = false;
    }
    else
    {   
        SU200_b_VMode = true;
    }

    SU200_SecTimer = gMainTimerCnt_10m;
	SU200_ExitCNT = 0;
    m_bSU200_GoTo_LayerMode = false;
    m_nSU200_GoTo_LayerModeCnt= 0;
    SU200_UM5_Rcv_Type = SU200_RCV_INIT;

//PassCode
    memset(SU200_Txt_PassCode, '\0', sizeof(SU200_Txt_PassCode));

    m_nSU200_ButtonIndex = g_nSU200_InitButtonIndex;//Pre Focus auto setting.

    SU200_InitControl();
    SU200_Button_Select(m_nSU200_ButtonIndex,false);
    SU200_SetUserTitle(g_nSU100_CurUserIdx);

    m_bSU200ReadyToJOG = true;

    m_bSU200_AUI1_Rcv_Error = false;        //chpark.24.02.14
    UserPW_Time_AUI1 = gMainTimerCnt_10m;	//chpark.24.02.14

    //chpark.24.03.08
    if(g_nSU100_Mode==SU100_Add_Mode)
    {
        SU200_Send_TX(0);
    }
    else
    {
        printf("[SU200_Enter]SU200_Send_TX :dont send.");
    }

    return true;
}

void SU200_InitControl()
{
    int tIndex=0;
    char tmp_WidgetName[100];
//Control setting
//printf("SU200===========1111\n");
    SetWidget(SU200_TOP_ICON[SU100_ICON_AD],"SU200_T_Icon_Admin");
	SetWidget(SU200_TOP_ICON[SU100_ICON_ST],"SU200_T_Icon_Std");
	SetWidget(SU200_TOP_NAME,"SU200_T_Name");

//printf("SU200===========22222\n");
//Background Text
    SetWidget(SU200_TXT[SU200_LANG_ENG],"SU200_TXT_ENG");
    SetWidget(SU200_TXT[SU200_LANG_GER],"SU200_TXT_GER");
    SetWidget(SU200_TXT[SU200_LANG_FR], "SU200_TXT_FR");
    SetWidget(SU200_TXT[SU200_LANG_IT], "SU200_TXT_IT");
    SetWidget(SU200_TXT[SU200_LANG_SP], "SU200_TXT_SP");

//Name
    SetWidget(SU200_TEXT_NAME,"SU200_Text_User_Name");
	
//Passcode
    for (int i=0;i<SU200_MAX_PW_CNT;i++)
	{
        memset(tmp_WidgetName, '\0', sizeof(tmp_WidgetName));
		sprintf(tmp_WidgetName, "SU200_Dot%d",i+1);
		SetWidget(SU200_UserPW_Data[i],tmp_WidgetName);
        ituWidgetSetVisible(SU200_UserPW_Data[i], false);
	}

    for (int i=0;i<SU200_MAX_PW_CNT-1;i++)
	{
        memset(tmp_WidgetName, 0, sizeof(tmp_WidgetName));
		sprintf(tmp_WidgetName, "SU200_Dot_X%d",i+1);
		SetWidget(SU200_UserPX_Data[i],tmp_WidgetName);
        ituWidgetSetVisible(SU200_UserPX_Data[i], false);
	}

//RFID
    SetWidget(SU200_CHK_RFID,"SU200_CHK_RFID");
//Access Level: (standard/admin)
    //admin
    SetWidget(SU200_CONT_LEVEL[SU100_ICON_AD],"SU200_CONT_AD");
        SetWidget(SU200_LEVEL_ICON_AD[SU200_LANG_ENG],"SU200_AD_ENG");
        SetWidget(SU200_LEVEL_ICON_AD[SU200_LANG_GER],"SU200_AD_GER");
        SetWidget(SU200_LEVEL_ICON_AD[SU200_LANG_FR],"SU200_AD_FR");
        SetWidget(SU200_LEVEL_ICON_AD[SU200_LANG_IT],"SU200_AD_IT");
        SetWidget(SU200_LEVEL_ICON_AD[SU200_LANG_SP],"SU200_AD_SP");
    //standard
    SetWidget(SU200_CONT_LEVEL[SU100_ICON_ST],"SU200_CONT_ST");
        SetWidget(SU200_LEVEL_ICON_ST[SU200_LANG_ENG],"SU200_ST_ENG");
        SetWidget(SU200_LEVEL_ICON_ST[SU200_LANG_GER],"SU200_ST_GER");
        SetWidget(SU200_LEVEL_ICON_ST[SU200_LANG_FR],"SU200_ST_FR");
        SetWidget(SU200_LEVEL_ICON_ST[SU200_LANG_IT],"SU200_ST_IT");
        SetWidget(SU200_LEVEL_ICON_ST[SU200_LANG_SP],"SU200_ST_SP");

//Access AUX:
    SetWidget(SU200_CONT_AUX,"SU200_CONT_AUX");
        SetWidget(SU200_AUX_TXT[SU200_LANG_ENG],"SU200_AUX_TXT_ENG");
        SetWidget(SU200_AUX_TXT[SU200_LANG_GER],"SU200_AUX_TXT_GER");
        SetWidget(SU200_AUX_TXT[SU200_LANG_FR],"SU200_AUX_TXT_FR");
        SetWidget(SU200_AUX_TXT[SU200_LANG_IT],"SU200_AUX_TXT_IT");
        SetWidget(SU200_AUX_TXT[SU200_LANG_SP],"SU200_AUX_TXT_SP");
    
    //YES
    SetWidget(SU200_CONT_AUX_YESNO[SU200_YES],"SU200_CONT_YES");
        SetWidget(SU200_AUX_ICON_YES[SU200_LANG_ENG],"SU200_YES_ENG");
        SetWidget(SU200_AUX_ICON_YES[SU200_LANG_GER],"SU200_YES_GER");
        SetWidget(SU200_AUX_ICON_YES[SU200_LANG_FR],"SU200_YES_FR");
        SetWidget(SU200_AUX_ICON_YES[SU200_LANG_IT],"SU200_YES_IT");
        SetWidget(SU200_AUX_ICON_YES[SU200_LANG_SP],"SU200_YES_SP");
    //NO
    SetWidget(SU200_CONT_AUX_YESNO[SU200_NO],"SU200_CONT_NO");
        SetWidget(SU200_AUX_ICON_NO[SU200_LANG_ENG],"SU200_NO_ENG");
        SetWidget(SU200_AUX_ICON_NO[SU200_LANG_GER],"SU200_NO_GER");
        SetWidget(SU200_AUX_ICON_NO[SU200_LANG_FR],"SU200_NO_FR");
        SetWidget(SU200_AUX_ICON_NO[SU200_LANG_IT],"SU200_NO_IT");
        SetWidget(SU200_AUX_ICON_NO[SU200_LANG_SP],"SU200_NO_SP");

//Register:
    SetWidget(SU200_CONT_RG,  "SU200_CONT_RG");
        SetWidget(SU200_BTN_RG[SU200_LANG_ENG],"SU200_BTN_RG_ENG");
        SetWidget(SU200_BTN_RG[SU200_LANG_GER],"SU200_BTN_RG_GER");
        SetWidget(SU200_BTN_RG[SU200_LANG_FR],"SU200_BTN_RG_FR");
        SetWidget(SU200_BTN_RG[SU200_LANG_IT],"SU200_BTN_RG_IT");
        SetWidget(SU200_BTN_RG[SU200_LANG_SP],"SU200_BTN_RG_SP");    
    

    //================================================================ 
    SetWidget(SU200_SEL[SU200_SEL_NAME],    "SU200_SEL_NAME");
    SetWidget(SU200_SEL[SU200_SEL_PASSCODE],"SU200_SEL_PASS");
    SetWidget(SU200_SEL[SU200_SEL_RFID],    "SU200_SEL_RFID");
    SetWidget(SU200_SEL[SU200_SEL_LEVEL],   "SU200_SEL_LEVEL");
    SetWidget(SU200_SEL[SU200_SEL_AUX],     "SU200_SEL_AUX");    
    SetWidget(SU200_SEL[SU200_SEL_RG],      "SU200_SEL_RG");  

    SetWidget(SU200_RG_DOWN,"SU200_RG_DOWN");   //chpark.01.15

    ituWidgetSetVisible(SU200_CONT_RG,  false); //hide Register Button.

    for(int i=0; i<5; i++)
    {
        ituWidgetSetVisible(SU200_TXT[i],       false);    
        ituWidgetSetVisible(SU200_AUX_TXT[i],   false);    
    }   

    ituWidgetSetVisible(SU200_SEL[SU200_SEL_NAME],       true); 
    ituWidgetSetVisible(SU200_SEL[SU200_SEL_PASSCODE],   false); 
    ituWidgetSetVisible(SU200_SEL[SU200_SEL_RFID],       false); 
    ituWidgetSetVisible(SU200_SEL[SU200_SEL_LEVEL],      false); 
    ituWidgetSetVisible(SU200_SEL[SU200_SEL_AUX],        false); 
    ituWidgetSetVisible(SU200_SEL[SU200_SEL_RG],         false); 

    ituWidgetSetVisible(SU200_TXT[Lang_Flag[M7_UserIndex]],  true);   
    ituWidgetSetVisible(SU200_TEXT_NAME,       true); 
    //==================================================================

    //Name
    SU200_SetUserName();

    //PassCode
    SU200_Display_PassCode();

    //
    if(g_nSU100_Mode == SU100_Add_Mode)
    {
        //register button
        ituWidgetSetVisible(SU200_CONT_RG,  true);
        for(int i=0;i<5;i++)
        {
            ituWidgetSetVisible(SU200_BTN_RG[i],  false);
        }
        ituWidgetSetVisible(SU200_BTN_RG[Lang_Flag[M7_UserIndex]],  true);

        //(1)RFID
        int nRegFlag;
        #ifdef RELEASE_VER
        //nRegFlag =  SU200_Chk_User_RFID(g_nSU100_AddUserIdx);
        //User_RegInfo[g_nSU100_AddUserIdx].RFStatus = nRegFlag;
        #endif

        if(User_RegInfo[g_nSU100_AddUserIdx].RFStatus==SU200_YES)
        {
            ituWidgetSetVisible(SU200_CHK_RFID, true); 
        }
        else
        {
            ituWidgetSetVisible(SU200_CHK_RFID, false); 
        }

        //(2)Level
        #ifdef RELEASE_VER
        //nRegFlag =  SU200_Chk_User_Level(g_nSU100_AddUserIdx);
        //User_RegInfo[g_nSU100_AddUserIdx].nTmp_RegFlag = nRegFlag;
        #endif

        if(User_RegInfo[g_nSU100_AddUserIdx].nTmp_RegFlag==SU100_USER_AD)//chpark.10.26
        {
            //admin

            //standard
            ituWidgetSetVisible(SU200_CONT_AUX, false); 
           
            //(2)level  //chpark.01.24
            for(int i=0; i<5 ;i++)
            {
                ituWidgetSetVisible(SU200_LEVEL_ICON_AD[i], false); 
                ituWidgetSetVisible(SU200_LEVEL_ICON_ST[i], false); 
            }

            ituWidgetSetVisible(SU200_LEVEL_ICON_AD[Lang_Flag[M7_UserIndex]], true); 
        }
        else if(User_RegInfo[g_nSU100_AddUserIdx].nTmp_RegFlag==SU100_USER_ST)
        {
            //standard
            ituWidgetSetVisible(SU200_CONT_AUX, true);
            ituWidgetSetVisible(SU200_AUX_TXT[Lang_Flag[M7_UserIndex]],  true);   
 
            for(int i=0; i<5 ;i++)
            {
                ituWidgetSetVisible(SU200_LEVEL_ICON_AD[i], false); 
                ituWidgetSetVisible(SU200_LEVEL_ICON_ST[i], false); 
            }

            ituWidgetSetVisible(SU200_LEVEL_ICON_ST[Lang_Flag[M7_UserIndex]], true); 
            
            if(User_RegInfo[g_nSU100_AddUserIdx].nTmp_AccessAUX==SU600_AUX_TYPE_YES)
            {
                //yes
                ituWidgetSetVisible(SU200_CONT_AUX_YESNO[SU200_YES], true);
                ituWidgetSetVisible(SU200_CONT_AUX_YESNO[SU200_NO], false);

                for(int i=0; i<5 ;i++)
                {
                    ituWidgetSetVisible(SU200_AUX_ICON_YES[i], false); 
                    ituWidgetSetVisible(SU200_AUX_ICON_NO[i], false); 
                }
                //printf("[SU200_AUX_ICON_YES]:%d========= \n",Lang_Flag[M7_UserIndex]);
                ituWidgetSetVisible(SU200_AUX_ICON_YES[Lang_Flag[M7_UserIndex]], true);
                
            }
            else
            {
                //no
                ituWidgetSetVisible(SU200_CONT_AUX_YESNO[SU200_YES], false);
                ituWidgetSetVisible(SU200_CONT_AUX_YESNO[SU200_NO], true);

                for(int i=0; i<5 ;i++)
                {
                    ituWidgetSetVisible(SU200_AUX_ICON_YES[i], false); 
                    ituWidgetSetVisible(SU200_AUX_ICON_NO[i], false); 
                }
                //printf("[SU200_AUX_ICON_NO]:%d========= \n",Lang_Flag[M7_UserIndex]);
                ituWidgetSetVisible(SU200_AUX_ICON_NO[Lang_Flag[M7_UserIndex]], true);                
            }
        }
        else
        {
            //Error 
            printf("[Add][Error]:%d========= \n",User_RegInfo[g_nSU100_AddUserIdx].nTmp_RegFlag);
        }       
    }
    else if(g_nSU100_Mode == SU100_Edit_Mode)
    {
        ituWidgetSetVisible(SU200_CONT_RG,  false); 

        //(1)RFID
        int nRegFlag;

        if(User_RegInfo[g_nSU100_EditUserIdx].RFStatus==SU200_YES)
        {
            ituWidgetSetVisible(SU200_CHK_RFID, true); 
        }
        else
        {
            ituWidgetSetVisible(SU200_CHK_RFID, false); 
        }

        //Level
        if(User_RegInfo[g_nSU100_EditUserIdx].nTmp_RegFlag==SU100_USER_AD)//chpark.10.26
        {
            //admin

            //standard
            ituWidgetSetVisible(SU200_CONT_AUX, false);  

            //(2)level  //chpark.01.24
            for(int i=0; i<5 ;i++)
            {
                ituWidgetSetVisible(SU200_LEVEL_ICON_AD[i], false); 
                ituWidgetSetVisible(SU200_LEVEL_ICON_ST[i], false); 
            }

            ituWidgetSetVisible(SU200_LEVEL_ICON_AD[Lang_Flag[M7_UserIndex]], true); 
        }
        else if(User_RegInfo[g_nSU100_EditUserIdx].nTmp_RegFlag==SU100_USER_ST || User_RegInfo[g_nSU100_EditUserIdx].nTmp_RegFlag==SU100_USER_SPAD ||User_RegInfo[g_nSU100_EditUserIdx].nTmp_RegFlag==SU100_USER_SAD)
        {
            //standard
            ituWidgetSetVisible(SU200_CONT_AUX, true);
            ituWidgetSetVisible(SU200_AUX_TXT[Lang_Flag[M7_UserIndex]],  true);   

            for(int i=0; i<5 ;i++)
            {
                ituWidgetSetVisible(SU200_LEVEL_ICON_AD[i], false); 
                ituWidgetSetVisible(SU200_LEVEL_ICON_ST[i], false); 
            }

            ituWidgetSetVisible(SU200_LEVEL_ICON_ST[Lang_Flag[M7_UserIndex]], true); 

            if(User_RegInfo[g_nSU100_EditUserIdx].nTmp_AccessAUX==SU600_AUX_TYPE_YES)
            {
                //yes
                ituWidgetSetVisible(SU200_CONT_AUX_YESNO[SU200_YES], true);
                ituWidgetSetVisible(SU200_CONT_AUX_YESNO[SU200_NO], false);

                for(int i=0; i<5 ;i++)
                {
                    ituWidgetSetVisible(SU200_AUX_ICON_YES[i], false); 
                    ituWidgetSetVisible(SU200_AUX_ICON_NO[i], false); 
                }

                ituWidgetSetVisible(SU200_AUX_ICON_YES[Lang_Flag[M7_UserIndex]], true);

                printf("[SU200_AUX_ICON_YES][Show][ID:%d][YESNO Value:%d]========= \n",g_nSU100_EditUserIdx,User_RegInfo[g_nSU100_EditUserIdx].nTmp_AccessAUX);
            }
            else
            {
                //no
                ituWidgetSetVisible(SU200_CONT_AUX_YESNO[SU200_YES], false);
                ituWidgetSetVisible(SU200_CONT_AUX_YESNO[SU200_NO], true);

                for(int i=0; i<5 ;i++)
                {
                    ituWidgetSetVisible(SU200_AUX_ICON_YES[i], false); 
                    ituWidgetSetVisible(SU200_AUX_ICON_NO[i], false); 
                }

                ituWidgetSetVisible(SU200_AUX_ICON_NO[Lang_Flag[M7_UserIndex]], true);

                printf("[SU200_AUX_ICON_NO][Show][ID:%d][YESNO Value:%d]========= \n",g_nSU100_EditUserIdx,User_RegInfo[g_nSU100_EditUserIdx].nTmp_AccessAUX);
            }
        }
        else if(User_RegInfo[g_nSU100_EditUserIdx].nTmp_RegFlag==SU100_USER_N_R || User_RegInfo[g_nSU100_EditUserIdx].nTmp_RegFlag==SU100_USER_N_A)//chpark.24.02.14
        {
            //printf("[Edit][NR or NA User]:g_nSU100_EditUserIdx[%d]:%d========= \n",g_nSU100_EditUserIdx,User_RegInfo[g_nSU100_EditUserIdx].nTmp_RegFlag);
            ituWidgetSetVisible(SU200_CONT_LEVEL[SU100_ICON_AD], false);
            ituWidgetSetVisible(SU200_CONT_LEVEL[SU100_ICON_ST], false);

            ituWidgetSetVisible(SU200_CONT_AUX, false);
        }
        else
        {
            //Error 
            printf("[Edit][Error]:%d========= \n",User_RegInfo[g_nSU100_EditUserIdx].nTmp_RegFlag);
        }
    } 
    
}

void SU200_SetUserTitle(uint8_t tUserIdx)
{
	uint8_t i;
	uint8_t tDestName[100];
		
	//Set Icon
	if(User_RegInfo[tUserIdx].RegFlag==SU100_USER_AD)
	{
		ituWidgetSetVisible(SU200_TOP_ICON[SU100_ICON_AD],true);
		ituWidgetSetVisible(SU200_TOP_ICON[SU100_ICON_ST],false);
	}
	else if(User_RegInfo[tUserIdx].RegFlag==SU100_USER_ST || User_RegInfo[tUserIdx].RegFlag==SU100_USER_SPAD||User_RegInfo[tUserIdx].RegFlag==SU100_USER_SAD)
	{
		ituWidgetSetVisible(SU200_TOP_ICON[SU100_ICON_AD],false);
		ituWidgetSetVisible(SU200_TOP_ICON[SU100_ICON_ST],true);
	}
    else
    {
		ituWidgetSetVisible(SU200_TOP_ICON[SU100_ICON_AD],false);
		ituWidgetSetVisible(SU200_TOP_ICON[SU100_ICON_ST],false);
    }

    if(g_UI1_ChkTime>0)
	{
        memset(tDestName,0,100);

		ituWidgetSetVisible(SU200_TOP_NAME,true);
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[tUserIdx].UserName);
		ituTextSetString(SU200_TOP_NAME, tDestName);
	}
	else if((g_UI1_ChkTime == 0)||(M7_UserIndex > (MAX_SU100_USER_NUM-1)))
	{
		ituWidgetSetVisible(SU200_TOP_NAME,false);
	}

}

void SU200_SetUserName()
{
	uint8_t i;

	uint8_t tDestName[50];

	memset(tDestName,0,50);
	
	//Set Icon
	if(g_nSU100_Mode==SU100_Add_Mode)
	{
		ituWidgetSetVisible(SU200_TOP_NAME,true);
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[g_nSU100_AddUserIdx].UserName);
		ituTextSetString(SU200_TEXT_NAME, tDestName);
        //printf("[SU200_Add_Mode]: g_nSU100_AddUserIdx=%d\n",g_nSU100_AddUserIdx);
	}
	else if(g_nSU100_Mode==SU100_Edit_Mode)
	{
		ituWidgetSetVisible(SU200_TOP_NAME,true);
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[g_nSU100_EditUserIdx].UserName);
		ituTextSetString(SU200_TEXT_NAME, tDestName);
        //printf("[SU100_Edit_Mode]: g_nSU100_EditUserIdx=%d  Name:%s\n",g_nSU100_EditUserIdx,tDestName);
	}
}

void SU200_Display_PassCode()
{
    int len,len2;//chpark.12.19
    int nPasscode_NOD=0;
    //PassCode
    if(g_nSU100_Mode==SU100_Add_Mode)
    {
        //printf("[SU200_Add_Mode]: g_nSU100_AddUserIdx=%d\n",g_nSU100_AddUserIdx);
        memcpy(SU200_Txt_PassCode,User_RegInfo[g_nSU100_AddUserIdx].UserPassCode,SU200_MAX_PW_CNT);
        len = strlen(SU200_Txt_PassCode);//chpark.12.19
        User_RegInfo[g_nSU100_AddUserIdx].PassNOD = len;//chpark.12.19
        nPasscode_NOD = User_RegInfo[g_nSU100_AddUserIdx].PassNOD;
        //printf("[Passcode][Add_Mode]: g_nSU100_AddUserIdx[%d]:%s nPasscode_NOD=%d\n",g_nSU100_AddUserIdx,SU200_Txt_PassCode,nPasscode_NOD);
    }
    else if(g_nSU100_Mode==SU100_Edit_Mode)
    {
        //printf("[SU200_Edit_Mode]: g_nSU100_EditUserIdx=%d\n",g_nSU100_EditUserIdx);
        if(g_UI1_ChkTime>0)
        {
            nPasscode_NOD = User_RegInfo[g_nSU100_EditUserIdx].PassNOD;
        }
        else
        {
            memcpy(SU200_Txt_PassCode,User_RegInfo[g_nSU100_EditUserIdx].UserPassCode,SU200_MAX_PW_CNT);
            len = strlen(SU200_Txt_PassCode);//chpark.12.19
            User_RegInfo[g_nSU100_EditUserIdx].PassNOD = len;//chpark.12.19
        }
        
        //printf("[Passcode][Edit_Mode]: g_nSU100_EditUserIdx[%d]:%s nPasscode_NOD=%d\n",g_nSU100_EditUserIdx,SU200_Txt_PassCode,nPasscode_NOD);  
    }

    for (int i=0;i<SU200_MAX_PW_CNT;i++)
	{
		ituWidgetSetVisible(SU200_UserPW_Data[i], false);
	}
    for (int i=0;i<SU200_MAX_PW_CNT-1;i++)
	{
		ituWidgetSetVisible(SU200_UserPX_Data[i], false);
	}
   
    //SU200_Show_InCenter(len);
    SU200_Show_InCenter(nPasscode_NOD);//chpark.12.19

    //printf("len = %d \n",len);
    //printf("[SU200_Display_PassCode]: %d  len2=%d  %s\n",len,len2,User_RegInfo[g_nSU100_EditUserIdx].UserPassCode);
    //printf("\n[SU200_Display_PassCode]: %d   %s   %S========\n",len,User_RegInfo[g_nSU100_EditUserIdx].UserPassCode,SU200_Txt_PassCode);
}


void SU200_Show_InCenter(int nLength)
{
    /*
    for (int i=0;i<nLength;i++)
    {
        ituWidgetSetVisible(SU200_UserPW_Data[i], true);
    }
    */
    switch(nLength)
    {
        case 1:
         ituWidgetSetVisible(SU200_UserPW_Data[4], true);
         break;
        case 2:
         ituWidgetSetVisible(SU200_UserPX_Data[3], true);
         ituWidgetSetVisible(SU200_UserPX_Data[4], true);
         break;
        case 3:
         ituWidgetSetVisible(SU200_UserPW_Data[3], true);
         ituWidgetSetVisible(SU200_UserPW_Data[4], true);
         ituWidgetSetVisible(SU200_UserPW_Data[5], true);
         break;
        case 4:
         ituWidgetSetVisible(SU200_UserPX_Data[2], true);
         ituWidgetSetVisible(SU200_UserPX_Data[3], true);
         ituWidgetSetVisible(SU200_UserPX_Data[4], true);
         ituWidgetSetVisible(SU200_UserPX_Data[5], true);
         break;
        case 5:
         ituWidgetSetVisible(SU200_UserPW_Data[2], true);
         ituWidgetSetVisible(SU200_UserPW_Data[3], true);
         ituWidgetSetVisible(SU200_UserPW_Data[4], true);
         ituWidgetSetVisible(SU200_UserPW_Data[5], true);
         ituWidgetSetVisible(SU200_UserPW_Data[6], true);
         break;
        case 6:
            for(int i=1;i<=6;i++)
            {
                ituWidgetSetVisible(SU200_UserPX_Data[i], true);   
            }
            break;
        case 7:
            for(int i=1;i<=7;i++)
            {
                ituWidgetSetVisible(SU200_UserPW_Data[i], true);   
            }
            break;
        case 8:
            for(int i=0;i<=7;i++)
            {
                ituWidgetSetVisible(SU200_UserPX_Data[i], true);   
            }
            break;
        case 9:
            for(int i=0;i<=8;i++)
            {
                ituWidgetSetVisible(SU200_UserPW_Data[i], true);   
            }
            break;

        default:
            break;    
    }
}


bool SU200_Button_Select(int nIndex,bool bDownImage)
{
    for(int i=0;i<=SU200_SEL_RG;i++)
    {
        ituWidgetSetVisible(SU200_SEL[i], false);
    }

    ituWidgetSetVisible(SU200_SEL[nIndex], true);

    //chpark.01.15
    if(nIndex==SU200_SEL_RG)
    {
        if(bDownImage)
        {
            ituWidgetSetVisible(SU200_RG_DOWN, true);
        }
        else
        {
            ituWidgetSetVisible(SU200_RG_DOWN, false);
        }
    }

    m_nSU200_ButtonIndex = nIndex;

    return true;
}


void SU200_MenuTabNext()
{
    if(g_nSU100_Mode==SU100_Add_Mode)
	{  
        //(1) add : admin  (aux X, register O)
        if(User_RegInfo[g_nSU100_AddUserIdx].RegFlag==SU100_USER_AD )
        {   
            //admin -> No aux
            switch(m_nSU200_ButtonIndex)
            {
                case SU200_SEL_NAME: 
                    m_nSU200_ButtonIndex = SU200_SEL_PASSCODE;
                    break;
                case SU200_SEL_PASSCODE: 
                    m_nSU200_ButtonIndex = SU200_SEL_RFID;
                    break;    
                case SU200_SEL_RFID: 
                    m_nSU200_ButtonIndex = SU200_SEL_LEVEL;
                    break;    
                case SU200_SEL_LEVEL: 
                    m_nSU200_ButtonIndex = SU200_SEL_RG;
                    break;      
                case SU200_SEL_RG: 
                    m_nSU200_ButtonIndex = SU200_SEL_NAME;
                    break;     
                default:
                    break;
            }
        }
        else
        {
        //(2) add : standard (aux O, register O)
            switch(m_nSU200_ButtonIndex)
            {
                case SU200_SEL_NAME: 
                    m_nSU200_ButtonIndex = SU200_SEL_PASSCODE;
                    break;
                case SU200_SEL_PASSCODE: 
                    m_nSU200_ButtonIndex = SU200_SEL_RFID;
                    break;    
                case SU200_SEL_RFID: 
                    m_nSU200_ButtonIndex = SU200_SEL_LEVEL;
                    break;    
                case SU200_SEL_LEVEL: 
                    m_nSU200_ButtonIndex = SU200_SEL_AUX;
                    break;      
                case SU200_SEL_AUX: 
                    m_nSU200_ButtonIndex = SU200_SEL_RG;
                    break;     
                case SU200_SEL_RG: 
                    m_nSU200_ButtonIndex = SU200_SEL_NAME;
                    break;     
                default:
                    break;
            }
        }
    }
    else if(g_nSU100_Mode==SU100_Edit_Mode)
    {
        //(3) edit : admin (aux X, register X)
        if(User_RegInfo[g_nSU100_EditUserIdx].RegFlag==SU100_USER_AD)
        {   
            //admin -> No aux
            switch(m_nSU200_ButtonIndex)
            {
                case SU200_SEL_NAME: 
                    m_nSU200_ButtonIndex = SU200_SEL_PASSCODE;
                    break;
                case SU200_SEL_PASSCODE: 
                    m_nSU200_ButtonIndex = SU200_SEL_RFID;
                    break;    
                case SU200_SEL_RFID: 
                    m_nSU200_ButtonIndex = SU200_SEL_LEVEL;
                    break;    
                case SU200_SEL_LEVEL: 
                    m_nSU200_ButtonIndex = SU200_SEL_NAME;
                    break;         
                default:
                    break;
            }
        }
        else if(User_RegInfo[g_nSU100_EditUserIdx].RegFlag==SU100_USER_N_R || User_RegInfo[g_nSU100_EditUserIdx].RegFlag==SU100_USER_N_A)
        {   
            //NA NR User -> No aux
            switch(m_nSU200_ButtonIndex)
            {
                case SU200_SEL_NAME: 
                    m_nSU200_ButtonIndex = SU200_SEL_PASSCODE;
                    break;
                case SU200_SEL_PASSCODE: 
                    m_nSU200_ButtonIndex = SU200_SEL_RFID;
                    break;    
                case SU200_SEL_RFID: 
                    m_nSU200_ButtonIndex = SU200_SEL_NAME;
                    break;    
                default:
                    m_nSU200_ButtonIndex = SU200_SEL_NAME;
                    break;
            }
        }
        else
        {
        //(4) edit : standard (aux O, register X)
            switch(m_nSU200_ButtonIndex)
            {
                case SU200_SEL_NAME: 
                    m_nSU200_ButtonIndex = SU200_SEL_PASSCODE;
                    break;
                case SU200_SEL_PASSCODE: 
                    m_nSU200_ButtonIndex = SU200_SEL_RFID;
                    break;    
                case SU200_SEL_RFID: 
                    m_nSU200_ButtonIndex = SU200_SEL_LEVEL;
                    break;    
                case SU200_SEL_LEVEL: 
                    m_nSU200_ButtonIndex = SU200_SEL_AUX;
                    break;      
                case SU200_SEL_AUX: 
                    m_nSU200_ButtonIndex = SU200_SEL_NAME;
                    break;        
                default:
                    break;
            }
        }
    }

    SU200_Button_Select(m_nSU200_ButtonIndex,false);//chpark.01.15
}

void SU200_MenuTabPrev()
{
    if(g_nSU100_Mode==SU100_Add_Mode)
	{  
        //(1) add : admin  (aux X, register O)
        if(User_RegInfo[g_nSU100_AddUserIdx].RegFlag==SU100_USER_AD)
        {   
            //admin -> No aux
            switch(m_nSU200_ButtonIndex)
            {
                case SU200_SEL_NAME: 
                    m_nSU200_ButtonIndex = SU200_SEL_RG;
                    break;
                case SU200_SEL_PASSCODE: 
                    m_nSU200_ButtonIndex = SU200_SEL_NAME;
                    break;    
                case SU200_SEL_RFID: 
                    m_nSU200_ButtonIndex = SU200_SEL_PASSCODE;
                    break;    
                case SU200_SEL_LEVEL: 
                    m_nSU200_ButtonIndex = SU200_SEL_RFID;
                    break;      
                case SU200_SEL_RG: 
                    m_nSU200_ButtonIndex = SU200_SEL_LEVEL;
                    break;     
                default:
                    break;
            }
        }
        else
        {
        //(2) add : standard (aux O, register O)
            switch(m_nSU200_ButtonIndex)
            {
                case SU200_SEL_NAME: 
                    m_nSU200_ButtonIndex = SU200_SEL_RG;
                    break;
                case SU200_SEL_PASSCODE: 
                    m_nSU200_ButtonIndex = SU200_SEL_NAME;
                    break;    
                case SU200_SEL_RFID: 
                    m_nSU200_ButtonIndex = SU200_SEL_PASSCODE;
                    break;    
                case SU200_SEL_LEVEL: 
                    m_nSU200_ButtonIndex = SU200_SEL_RFID;
                    break;      
                case SU200_SEL_AUX: 
                    m_nSU200_ButtonIndex = SU200_SEL_LEVEL;
                    break;     
                case SU200_SEL_RG: 
                    m_nSU200_ButtonIndex = SU200_SEL_AUX;
                    break;     
                default:
                    break;
            }
        }
    }
    else if(g_nSU100_Mode==SU100_Edit_Mode)
    {
        //(3) edit : admin (aux X, register X)
        if(User_RegInfo[g_nSU100_EditUserIdx].RegFlag==SU100_USER_AD )
        {   
            //admin -> No aux , No RG
            switch(m_nSU200_ButtonIndex)
            {
                case SU200_SEL_NAME: 
                    m_nSU200_ButtonIndex = SU200_SEL_LEVEL;
                    break;
                case SU200_SEL_PASSCODE: 
                    m_nSU200_ButtonIndex = SU200_SEL_NAME;
                    break;    
                case SU200_SEL_RFID: 
                    m_nSU200_ButtonIndex = SU200_SEL_PASSCODE;
                    break;    
                case SU200_SEL_LEVEL: 
                    m_nSU200_ButtonIndex = SU200_SEL_RFID;
                    break;         
                default:
                    break;
            }
        }
        else if(User_RegInfo[g_nSU100_EditUserIdx].RegFlag==SU100_USER_N_R || User_RegInfo[g_nSU100_EditUserIdx].RegFlag==SU100_USER_N_A)
        {
            //NA NR User -> No aux , No RG
            switch(m_nSU200_ButtonIndex)
            {
                case SU200_SEL_NAME: 
                    m_nSU200_ButtonIndex = SU200_SEL_RFID;
                    break;
                case SU200_SEL_PASSCODE: 
                    m_nSU200_ButtonIndex = SU200_SEL_NAME;
                    break;    
                case SU200_SEL_RFID: 
                    m_nSU200_ButtonIndex = SU200_SEL_PASSCODE;
                    break;    
                default:
                    break;
            }
        }
        else
        {
        //(4) edit : standard (aux O, register X)
            switch(m_nSU200_ButtonIndex)
            {
                case SU200_SEL_NAME: 
                    m_nSU200_ButtonIndex = SU200_SEL_AUX;
                    break;
                case SU200_SEL_PASSCODE: 
                    m_nSU200_ButtonIndex = SU200_SEL_NAME;
                    break;    
                case SU200_SEL_RFID: 
                    m_nSU200_ButtonIndex = SU200_SEL_PASSCODE;
                    break;    
                case SU200_SEL_LEVEL: 
                    m_nSU200_ButtonIndex = SU200_SEL_RFID;
                    break;      
                case SU200_SEL_AUX: 
                    m_nSU200_ButtonIndex = SU200_SEL_LEVEL;
                    break;        
                default:
                    break;
            }
        }

    }

    SU200_Button_Select(m_nSU200_ButtonIndex,false);
}

void SU200_MenuTabUp()
{
    SU200_MenuTabPrev();
}

void SU200_MenuTabDown()
{
    SU200_MenuTabNext();
}

void SU200_MenuTabRight()
{
   if(SU200_b_VMode)
   {
     //not move
   }
   else
   {
        if(g_nSU100_Mode==SU100_Add_Mode)
        {  
            //(1) add : admin  (aux X, register O)
            if(User_RegInfo[g_nSU100_AddUserIdx].RegFlag==SU100_USER_AD )
            {   
                //admin -> No aux
                switch(m_nSU200_ButtonIndex)
                {
                    case SU200_SEL_NAME: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;
                    case SU200_SEL_PASSCODE: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;    
                    case SU200_SEL_RFID: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;    
                    case SU200_SEL_LEVEL: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;      
                    case SU200_SEL_RG: 
                        m_nSU200_ButtonIndex = SU200_SEL_LEVEL;
                        break;     
                    default:
                        break;
                }
            }
            else
            {
            //(2) add : standard (aux O, register O)
                switch(m_nSU200_ButtonIndex)
                {
                    case SU200_SEL_NAME: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;
                    case SU200_SEL_PASSCODE: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;    
                    case SU200_SEL_RFID: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;    
                    case SU200_SEL_LEVEL: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;      
                    case SU200_SEL_AUX: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;     
                    case SU200_SEL_RG: 
                        m_nSU200_ButtonIndex = SU200_SEL_AUX;
                        break;     
                    default:
                        break;
                }
            }
        }
        else if(g_nSU100_Mode==SU100_Edit_Mode)
        {
           //not move
        }

        SU200_Button_Select(m_nSU200_ButtonIndex,false);//chpark.01.15
   }
}

void SU200_MenuTabLeft()
{

   if(SU200_b_VMode)
   {
     //not move
   }
   else
   {
        if(g_nSU100_Mode==SU100_Add_Mode)
        {  
            //(1) add : admin  (aux X, register O)
            if(User_RegInfo[g_nSU100_AddUserIdx].RegFlag==SU100_USER_AD )
            {   
                //admin -> No aux
                switch(m_nSU200_ButtonIndex)
                {
                    case SU200_SEL_NAME: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;
                    case SU200_SEL_PASSCODE: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;    
                    case SU200_SEL_RFID: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;    
                    case SU200_SEL_LEVEL: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;      
                    case SU200_SEL_RG: 
                        m_nSU200_ButtonIndex = SU200_SEL_LEVEL;
                        break;     
                    default:
                        break;
                }
            }
            else
            {
            //(2) add : standard (aux O, register O)
                switch(m_nSU200_ButtonIndex)
                {
                    case SU200_SEL_NAME: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;
                    case SU200_SEL_PASSCODE: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;    
                    case SU200_SEL_RFID: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;    
                    case SU200_SEL_LEVEL: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;      
                    case SU200_SEL_AUX: 
                        m_nSU200_ButtonIndex = SU200_SEL_RG;
                        break;     
                    case SU200_SEL_RG: 
                        m_nSU200_ButtonIndex = SU200_SEL_AUX;
                        break;     
                    default:
                        break;
                }
            }
        }
        else if(g_nSU100_Mode==SU100_Edit_Mode)
        {
           //not move
        }

        SU200_Button_Select(m_nSU200_ButtonIndex,false);//chpark.01.15
   }
}

void SU200_MenuTabESC()
{
    //chpark.11.27
    if(g_nSU100_Mode == SU100_Add_Mode)
    {
        g_nSU100_PreLayer = M7_SU500_RFID_LAYER;//chpark.24.04.03
        
        //24.04.16
        User_RegInfo[g_nSU100_AddUserIdx].RegFlag          = SU600_AUX_TYPE_NONE;
        User_RegInfo[g_nSU100_AddUserIdx].nTmp_RegFlag     = SU600_AUX_TYPE_NONE;
        //
        
        ITULayer *M7_SU600_LEVEL = ituSceneFindWidget(&theScene, "M7_SU600_LEVEL");
        assert(M7_SU600_LEVEL);
        ituLayerGoto(M7_SU600_LEVEL);
    }
    else if(g_nSU100_Mode == SU100_Edit_Mode)
    {
        ITULayer *M7_SU100 = ituSceneFindWidget(&theScene, "M7_SU100");
        assert(M7_SU100);
        ituLayerGoto(M7_SU100);
    }
}


void SU200_MenuTabGoto()
{
    //printf("\n [SU200_MenuTabGoto] == %d\n\n",m_nSU200_ButtonIndex);
    ITULayer *M7_SU_MOVELAYER;
    switch(m_nSU200_ButtonIndex)
    {
        case SU200_SEL_NAME:
            g_nSU200_InitButtonIndex = SU200_SEL_NAME;
            g_nSU100_PreLayer = M7_SU200_EDIT_LAYER;

            M7_SU_MOVELAYER = ituSceneFindWidget(&theScene, "M7_SU300_NAME");
            assert(M7_SU_MOVELAYER);
            ituLayerGoto(M7_SU_MOVELAYER);
            break;
        
        case SU200_SEL_PASSCODE:
            g_nSU200_InitButtonIndex = SU200_SEL_PASSCODE;
            g_nSU100_PreLayer = M7_SU200_EDIT_LAYER;

            M7_SU_MOVELAYER = ituSceneFindWidget(&theScene, "M7_SU400_PASS");
            assert(M7_SU_MOVELAYER);
            ituLayerGoto(M7_SU_MOVELAYER);
            break;
        
        case SU200_SEL_RFID:
            g_nSU200_InitButtonIndex = SU200_SEL_RFID;
            g_nSU100_PreLayer = M7_SU200_EDIT_LAYER;

            M7_SU_MOVELAYER = ituSceneFindWidget(&theScene, "M7_SU500_RFID");
            assert(M7_SU_MOVELAYER);
            ituLayerGoto(M7_SU_MOVELAYER);            
            break;
        
        case SU200_SEL_LEVEL:
        case SU200_SEL_AUX:
            g_nSU200_InitButtonIndex = SU200_SEL_LEVEL;
            g_nSU100_PreLayer = M7_SU200_EDIT_LAYER;

            M7_SU_MOVELAYER = ituSceneFindWidget(&theScene, "M7_SU600_LEVEL");
            assert(M7_SU_MOVELAYER);
            ituLayerGoto(M7_SU_MOVELAYER);           
            break;
        
        case SU200_SEL_RG:            
            break;
        
        default:
            break;    
    }
}



bool SU200_BTN_MouseDn(ITUWidget* widget, char* param)
{
    uint8_t i,tBTN_Index = 0;
	
	SU200_ExitCNT = 0;
	tBTN_Index = (uint8_t)atoi(param);
    
    //chpark.24.02.14
    if(g_nSU100_Mode == SU100_Edit_Mode)
    {
        if(tBTN_Index==SU200_SEL_LEVEL ||  tBTN_Index==SU200_SEL_AUX)        
        {
            if(User_RegInfo[g_nSU100_EditUserIdx].nTmp_RegFlag==SU100_USER_N_R || User_RegInfo[g_nSU100_EditUserIdx].nTmp_RegFlag==SU100_USER_N_A)
            {
                return false;
            }
        }
    }

    SU200_Button_Select(tBTN_Index, true);  
   
    return true;
}

bool SU200_BTN_MouseUp(ITUWidget* widget, char* param)
{
    uint8_t i,tBTN_Index = 0;
	
	SU200_ExitCNT = 0;
	tBTN_Index = (uint8_t)atoi(param);

    //chpark.24.02.14
    if(g_nSU100_Mode == SU100_Edit_Mode)
    {
        if(tBTN_Index==SU200_SEL_LEVEL ||  tBTN_Index==SU200_SEL_AUX)        
        {
            if(User_RegInfo[g_nSU100_EditUserIdx].nTmp_RegFlag==SU100_USER_N_R || User_RegInfo[g_nSU100_EditUserIdx].nTmp_RegFlag==SU100_USER_N_A)
            {
                return false;
            }
        }
    }

    SU200_Button_Select(tBTN_Index, false);  
    
    SU200_D7_MouseUp(NULL,NULL);
 
    return true;
}

void SU200_Chk_Result(int nSuc)
{
    //register button 
        
    //send svr info

    //register user info
    if(g_nSU100_Mode==SU100_Add_Mode)
    {
        User_RegInfo[g_nSU100_AddUserIdx].bRegister      = true;//register success
        User_RegInfo[g_nSU100_AddUserIdx].bAddProcessing = false; //complete->false. incomplete->true.
        //User_RegInfo[g_nSU100_AddUserIdx].Language       = Lang_Flag[M7_UserIndex]; //save user language.
        User_RegInfo[g_nSU100_AddUserIdx].bDelSelect     = false; //save user language.

        printf("[SU200_Chk_Result][%d][Lang:%d]===\n",g_nSU100_AddUserIdx, User_RegInfo[g_nSU100_AddUserIdx].Language);
    }

    g_nSU100_PreLayer = M7_SU100_LAYER;
    ITULayer *M7_SU100 = ituSceneFindWidget(&theScene, "M7_SU100");
    assert(M7_SU100);
    ituLayerGoto(M7_SU100);
}


uint8_t SU200_Chk_User_RFID(int nUserIdx)
{
    //4bit check
    uint8_t nUserRFID = 0;

    switch(nUserIdx)
    {
        case 0: nUserRFID=(SU100_UI2_All_Byte[0] & 0x10); break; //#1 user
        case 1: nUserRFID=(SU100_UI2_All_Byte[2] & 0x10); break; //#2 user
        case 2: nUserRFID=(SU100_UI2_All_Byte[4] & 0x10); break; //#3 user
        case 3: nUserRFID=(SU100_UI2_All_Byte[6] & 0x10); break; //#4 user

        case 4: nUserRFID=(SU100_UI3_All_Byte[0] & 0x10); break; //#5 user
        case 5: nUserRFID=(SU100_UI3_All_Byte[2] & 0x10); break; //#6 user
        case 6: nUserRFID=(SU100_UI3_All_Byte[4] & 0x10); break; //#7 user
        case 7: nUserRFID=(SU100_UI3_All_Byte[6] & 0x10); break; //#8 user

        case 8: nUserRFID =(SU100_UI4_All_Byte[0] & 0x10); break; //#9 user
        case 9: nUserRFID =(SU100_UI4_All_Byte[2] & 0x10); break; //#10 user
        case 10: nUserRFID=(SU100_UI4_All_Byte[4] & 0x10); break; //#11 user
        case 11: nUserRFID=(SU100_UI4_All_Byte[6] & 0x10); break; //#12 user

        case 12: nUserRFID =(SU100_UI5_All_Byte[0] & 0x10); break; //#13 user
        case 13: nUserRFID =(SU100_UI5_All_Byte[2] & 0x10); break; //#14 user
        case 14: nUserRFID =(SU100_UI5_All_Byte[4] & 0x10); break; //#15 user
    }

    printf("%d user level=%d\n",nUserIdx,nUserRFID);

    return nUserRFID; 
}

uint8_t SU200_Chk_User_Level(int nUserIdx)
{
    uint8_t nUserLevel = 0;

    switch(nUserIdx)
    {
        case 0: nUserLevel=(SU100_UI2_All_Byte[0] & 0x0F); break; //#1 user
        case 1: nUserLevel=(SU100_UI2_All_Byte[2] & 0x0F); break; //#2 user
        case 2: nUserLevel=(SU100_UI2_All_Byte[4] & 0x0F); break; //#3 user
        case 3: nUserLevel=(SU100_UI2_All_Byte[6] & 0x0F); break; //#4 user

        case 4: nUserLevel=(SU100_UI3_All_Byte[0] & 0x0F); break; //#5 user
        case 5: nUserLevel=(SU100_UI3_All_Byte[2] & 0x0F); break; //#6 user
        case 6: nUserLevel=(SU100_UI3_All_Byte[4] & 0x0F); break; //#7 user
        case 7: nUserLevel=(SU100_UI3_All_Byte[6] & 0x0F); break; //#8 user

        case 8: nUserLevel =(SU100_UI4_All_Byte[0] & 0x0F); break; //#9 user
        case 9: nUserLevel =(SU100_UI4_All_Byte[2] & 0x0F); break; //#10 user
        case 10: nUserLevel=(SU100_UI4_All_Byte[4] & 0x0F); break; //#11 user
        case 11: nUserLevel=(SU100_UI4_All_Byte[6] & 0x0F); break; //#12 user

        case 12: nUserLevel =(SU100_UI5_All_Byte[0] & 0x0F); break; //#13 user
        case 13: nUserLevel =(SU100_UI5_All_Byte[2] & 0x0F); break; //#14 user
        case 14: nUserLevel =(SU100_UI5_All_Byte[4] & 0x0F); break; //#15 user
    }

    printf("%d user level=%d\n",nUserIdx,nUserLevel);
    if(nUserLevel==0)
    {
        //not registered
        printf("[Error]Not registered user\n");
    }
    else if(nUserLevel>=2 && nUserLevel<=5)
    {
        //2: super plus admin
        //3: super admin
        //5: admin
        nUserLevel = 1;
    }
    else if(nUserLevel==9)
    {
        //9:standard
        nUserLevel = 2;
    }
    else
    {
        //15:N/A
        printf("[Error]N/A user\n");
    }
    
    return nUserLevel; 
}

//=========================================================
bool SU200_JogWheel(ITUWidget* widget, char* param)
{
    if(!m_bSU200ReadyToJOG || m_bSU200_GoTo_LayerMode)return false;
    SU200_ExitCNT = 0;
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
                //printf("SU200 JogWheel[R] %d\r\n",JogDir);
                SU200_MenuTabNext();
			}
			else if(JogDir == JOG_DIAL_ROT_LEFT)
			{
                //printf("SU200 JogWheel[L] %d\r\n",JogDir);
                SU200_MenuTabPrev();
			}

		break;
	}

    return true;
}

bool SU200_JogButton(ITUWidget* widget, char* param)
{
    if(!m_bSU200ReadyToJOG || m_bSU200_GoTo_LayerMode)return false;
    
    SU200_ExitCNT = 0;
    uint8_t JogButton = 0;
	JogButton = param[0];
	//printf("SU200_JogButton %d\r\n",JogButton);

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
                    SU200_MenuTabUp();
					break;

				case JOG_DIAL_KEY_DOWN:
                    SU200_MenuTabDown();
					break;
				
				case JOG_DIAL_KEY_LEFT:
                    
                    SU200_MenuTabLeft();
                    break;

				case JOG_DIAL_KEY_RIGHT:
                    SU200_MenuTabRight();
					break;

				case JOG_DIAL_KEY_ENTER_ON:
                    SU200_Button_Select(m_nSU200_ButtonIndex,true);  
					break;

				case JOG_DIAL_KEY_ENTER_OFF:				//c3Byte0bit
        		case JOG_DIAL_KEY_NONE:
				{
             		SU200_Button_Select(m_nSU200_ButtonIndex,false);  

                    SU200_D7_MouseUp(NULL,NULL);
        		}
					break;
			}
		break;
	}

    return true;
}

bool SU200_JogSwitch(ITUWidget* widget, char* param)
{
    if(!m_bSU200ReadyToJOG || m_bSU200_GoTo_LayerMode )return false;
    SU200_ExitCNT = 0;
	uint8_t JogSwitch = 0;
	JogSwitch = param[0];

	if(JogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
        SU200_MenuTabESC();
	}
	else if(JogSwitch == JOG_DIAL_SW_HOME_OFF)
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}

    return true;
}

//=========================================================
bool SU200_D7_MouseUp(ITUWidget* widget, char* param)
{
    if(m_nSU200_ButtonIndex==SU200_SEL_RG)
    {   
        g_UM5_ChkTime = 0;
        SU200_UM5_Rcv_Type = SU200_RCV_REGISTER_CHK;
        //chpark.10.23
        if(!gUI_Server_Mode)
        {
            printf("SU200_D7_MouseUp==Local Mode\n");
            //=========================
            //chpark.24.02.08
            #ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
                InitVariable(g_nSU100_AddUserIdx,false);//chpark.24.11.19
                Lang_Flag[g_nSU100_AddUserIdx] = User_RegInfo[g_nSU100_AddUserIdx].Language;;//chpark.24.03.21
				XO_Save_Config(g_nSU100_AddUserIdx,true,0);//chpark.12.12  JPHong 2024-12-18 
			#else
				HT_Save_Config(1);
			#endif
            //=========================

            SU200_Chk_Result(1);
        }
        else
        {
            SU200_Send_TX(1);
        }
    }
    else
    {
        SU200_MenuTabGoto();
    }

    return true;
}

bool SU200_D6_MouseUp(ITUWidget* widget, char* param)
{
    SU200_ExitCNT = 0;
    
    SU100_Send_Add_Cancel();//chpark.10.30

    GotoMain(MAIN_MODE_NORMAL); 

    return true;
}

bool SU200_D5_MouseUp(ITUWidget* widget, char* param)
{
    SU200_MenuTabESC();
 
    return true;
}
//=========================================================

//======================================================
bool SU200_Timer(ITUWidget* widget, char* param)
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
		if(!m_bSU200_AUI1_Rcv_Error && !m_bSU200_GoTo_LayerMode && gUI_Server_Mode)
		{
			printf("\r\n AUI1 Protocol 5Sec Exit >>>>>>> goto Main\r\n");

            m_bSU200_AUI1_Rcv_Error = true;
            m_bSU200_GoTo_LayerMode = true;
        }
	}
    #endif

    if((gMainTimerCnt_10m - SU200_SecTimer) >= 20)//100) //200msec 
	{
		if(SU200_ExitCNT < 30*5)
			SU200_ExitCNT++;
		else if(SU200_ExitCNT == 30*5)
		{
			SU200_ExitCNT = 300;
			printf("\r\n SU200 Menu 30Sec Exit > goto Main\r\n");
            #ifdef RELEASE_VER
                SU100_Send_Add_Cancel();//chpark.10.30
                GotoMain(MAIN_MODE_NORMAL); 
                return true;
	        #endif
		}

		if(m_bSU200_GoTo_LayerMode)
		{
            m_nSU200_GoTo_LayerModeCnt++;//chpark.24.02.14
            if(m_nSU200_GoTo_LayerModeCnt>=2*5)//2 sec
			{
                m_bSU200_GoTo_LayerMode = false;
                m_nSU200_GoTo_LayerModeCnt = 0;

                if(m_bSU200_AUI1_Rcv_Error)
                {
                    #ifdef RELEASE_VER
                        SU100_Send_Add_Cancel();//chpark.10.30
                        //chpark.24.02.29
                        ITULayer *Menu7_Layer = ituSceneFindWidget(&theScene, "Menu7_Layer");
                        assert(Menu7_Layer);
                        ituLayerGoto(Menu7_Layer);
                        return true;
	                #endif
                }

                SU200_MenuTabGoto();
            }
		}

        //======
        //data received chpark.08.22
        SU200_SendCNT++;
		
		if(SU200_SendCNT>=5)//1sec
		{
			SU200_SendCNT=0;
		}

        if(g_UM5_ChkTime>0 && !m_bSU200_GoTo_LayerMode)
        {
            g_UM5_ChkTime = 0;
           
            if(SU200_UM5_Rcv_Type == SU200_RCV_REGISTER_CHK)
            {
                SU200_UM5_Rcv_Type = SU200_RCV_INIT;
                if(SU200_UM5_1Byte_67bit == 1)//Register ACK:[0:N/A 1:success 2:fail 3:N/A]
                { 
                    //=========================
                    //chpark.24.02.08
                    #ifdef USED_SAVE_CONFIG		// JPHong 2023-11-07
                        InitVariable(g_nSU100_AddUserIdx,false);//chpark.24.11.19
                        Lang_Flag[g_nSU100_AddUserIdx] = User_RegInfo[g_nSU100_AddUserIdx].Language;;//chpark.24.03.21
				        XO_Save_Config(g_nSU100_AddUserIdx,true,0);//chpark.12.12  JPHong 2024-12-18 
			        #else
				        HT_Save_Config(1);
			        #endif
                    //=========================

                    //true;
                    m_bSU200_GoTo_LayerMode = true;
                
                    SU200_Send_TX(2);
                    SU200_Chk_Result(1);
                }
                else
                {
                    printf("[RET Error] in Register : %d \n",SU200_UM5_1Byte_67bit);
                }
            } 
        }
        //=====

		SU200_SecTimer = gMainTimerCnt_10m;
	}

    return true;
}

bool SU200_Send_TX(int nType)
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

    printf("[SU200_Send_TX][%d] usernum=%d\n",nType,curUserIdx);

    switch(nType)
    {
        case 0:
                //UM1
                //0byte: [0bit=0 1bit=1] 0x0000 0010
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
        case 1:
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
                    txbuffer[0] = 0x04; //
                }

                txbuffer[2] = curUserIdx; //1~15//chpark.12.19
				
                CanbusTxDataManagement( gMainTimerCnt_10m,UM1,txbuffer);//chpark.08.22
            break;
        
        case 2:
                //End Register
                //UM1
                //0byte:[1bit] 0   0x00000000=>0
				//send [No longer addmode]
                txbuffer[0] = 0; //chpark.12.19
                txbuffer[2] = curUserIdx; //1~15//chpark.12.19
				
                CanbusTxDataManagement( gMainTimerCnt_10m,UM1,txbuffer);//chpark.08.22
            break;

        default:
            break;
    }
}
//======================================================


