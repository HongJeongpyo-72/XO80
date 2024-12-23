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
MenuIM_Layer
*/

static bool IM_b_GUI_Ready = false;

//chpark.24.10.22
static bool IM_b_100m_DataSend;
static bool IM_b_100m_ResetSend;
static uint8_t IM_n_100m_SendByte;
static uint8_t IM_n_100m_SendData;
static int IM_n_100m_NextStep;

uint8_t g_IM_Main_Step;

#define IM_CONT_MAIN    5

#define Menu_IM_REGISTER			0
#define Menu_IM_DELETE				1
#define Menu_IM_YES			        2
#define Menu_IM_NO	    		    3

#define IM_CONT_MODE    0 //예외. main 1개만 있다.
#define IM_CONT_REG1    1 //main1개+각각 언어5개
#define IM_CONT_REG2    2
#define IM_CONT_REG3    3
#define IM_CONT_REG4    4
#define IM_CONT_REG5    5
#define IM_CONT_REG6    6 //
#define IM_CONT_REG7    7 //
#define IM_CONT_REG8    8 //
#define IM_CONT_REG_FAIL    9
#define IM_CONT_AU_FAIL     10
#define IM_CONT_DEL             11
#define IM_CONT_DEL_SUC         12
#define IM_CONT_DEL_FAIL1       13
#define IM_CONT_DEL_FAIL2       14 
#define IM_CONT_YN_MODE     15  //예외. main 1개만 있다.

#define IM_CONT_BTN_REG     16 
#define IM_CONT_BTN_DEL     17 
#define IM_CONT_BTN_YES     18 
#define IM_CONT_BTN_NO      19 


#define IM_BTN_REG     0 
#define IM_BTN_DEL     1 
#define IM_BTN_YES     2 
#define IM_BTN_NO      3 

#define IM_SELECT_REG     1 
#define IM_SELECT_DEL     2 
#define IM_SELECT_YES     3 
#define IM_SELECT_NO      4 



#define IM_ANI_NONE     0 //0:None  1:Scale up  2:Scale down
#define IM_ANI_DOWN     1 //0:None  1:down  2:up
#define IM_ANI_UP       2 

#define IM_BTN_STATUS_NONE     0 
#define IM_BTN_STATUS_DOWN     1 
#define IM_BTN_STATUS_UP       2 

static uint8_t nIM_Scaleflag[4];   //0:None  1:Scale up  2:Scale down
static uint8_t nIM_BTN_Status[4];  //0:None  1:down  2:up

static ITUWidget* IM_CONT[21][7]; //

static ITUWidget*    IM_BTN_S[4][5];
static ITUAnimation* IM_BTN_ANI[4][5];
static ITUWidget*    IM_BTN_L[4][5];

static ITUWidget*    IM_D7_BTN;//chpark.24.10.30

static uint32_t IM_SecTimer;
static uint32_t IM_ExitCNT = 0;
static uint32_t IM_SendCNT = 0;
static bool IM_b_VMode = false;

bool g_IM_b_ExistLayer;

uint8_t g_IM_Main_Step;
uint8_t g_IM_RD_Select; //1:register 2: delete
uint8_t g_IM_YN_Select; //1:yes 2: no

void MenuIM_Send_100m_TX(uint8_t nByte, uint8_t nData, int nNextStep);

void MenuIM_InitControl();
void MenuIM_InitControl_ANI();

void MenuIM_Send_TX(uint8_t nByte, uint8_t nData);
void MenuIM_RcvData(int nData); //chpark.24.08.29
void MenuIM_SetMode(int nMode);

bool IM_Button_Select(uint8_t nIndex);
void IM_MenuTabEnter();
void IM_MenuTabESC();
void IM_MenuTabNext();


bool IM_D7_BTNMouseUp(ITUWidget* widget, char* param);
bool IM_D6_BTNMouseUp(ITUWidget* widget, char* param);
bool IM_D5_BTNMouseUp(ITUWidget* widget, char* param);
bool IM_BTN_MouseUp(ITUWidget* widget, char* param);
bool IM_BTN_MouseDown(ITUWidget* widget, char* param);

void IM_ANIStopped_Set_Button_State(int nIndex);

/*
bool IM_ANIStopped_N(ITUWidget* widget, char* param);
bool IM_ANIStopped_Y(ITUWidget* widget, char* param);
bool IM_ANIStopped_D(ITUWidget* widget, char* param);
bool IM_ANIStopped_R(ITUWidget* widget, char* param);
*/

void IM_ChoiceBtnClear();

//chpark.24.10.30
void GotoBSImmobilizer(void)
{
//	if(CameraOnLeaveCallSrc(MAIN_MODE_MENU)==1)
	{
	    ITULayer *MenuIM_Layer = ituSceneFindWidget(&theScene, "MenuIM_Layer");
		ituLayerGoto(MenuIM_Layer);
		//MenuIM_RcvData(1);
        g_IM_b_ExistLayer = true;
	    Main_Disp_Mode = MAIN_MODE_IMMOBI;
	}
}

bool MenuIM_Enter(ITUWidget* widget, char* param)
{
    IM_b_GUI_Ready = false;

    printf("MenuIM_Enter==START\n");
    g_IM_b_ExistLayer = true;

    gMB_Language = Lang_Flag[M7_UserIndex];//chpark.24.02.19
    IM_SecTimer  = gMainTimerCnt_10m;
	IM_ExitCNT 	 = 0;
	IM_SendCNT	 = 0;//1sec

    if((RotationDir() == LANDSCAPE_1)||(RotationDir() == LANDSCAPE_2))
	{
        IM_b_VMode = false;
    }
    else
    {   
        IM_b_VMode = true;
    }
   
    MenuIM_InitControl();
    MenuIM_InitControl_ANI();

    IM_ChoiceBtnClear();

    uint8_t nStep =  g_IM_Main_Step;

    IM_b_GUI_Ready = true;

    if(nStep<100)
    {
        printf("[MenuIM_Enter]MenuIM_SetMode\n");
        MenuIM_SetMode(nStep);
    }
    else
    {
        MenuIM_RcvData(nStep);
        printf("[MenuIM_Enter]NOT MenuIM_SetMode\n");
    }

	ReadyRotation();	// JPHong 2024-10-31
	ScreenSetOnOff(1);	// JPHong 2024-10-31

    printf("MenuIM_Enter==END\n");
    
    return true;
}


void MenuIM_InitControl()
{
    char tmpWidgetName[40];
    //Main container setting
    SetWidget(IM_CONT[IM_CONT_MODE][IM_CONT_MAIN],"IM_C_Mode");
    
    SetWidget(IM_CONT[IM_CONT_REG1][IM_CONT_MAIN],"IM_C_Reg1");
    SetWidget(IM_CONT[IM_CONT_REG2][IM_CONT_MAIN],"IM_C_Reg2");
    SetWidget(IM_CONT[IM_CONT_REG3][IM_CONT_MAIN],"IM_C_Reg3");
    SetWidget(IM_CONT[IM_CONT_REG4][IM_CONT_MAIN],"IM_C_Reg4");
    SetWidget(IM_CONT[IM_CONT_REG5][IM_CONT_MAIN],"IM_C_Reg5");
    SetWidget(IM_CONT[IM_CONT_REG6][IM_CONT_MAIN],"IM_C_Reg6");
    SetWidget(IM_CONT[IM_CONT_REG7][IM_CONT_MAIN],"IM_C_Reg7");
    SetWidget(IM_CONT[IM_CONT_REG8][IM_CONT_MAIN],"IM_C_Reg8");

    SetWidget(IM_CONT[IM_CONT_REG_FAIL][IM_CONT_MAIN],"IM_C_Reg_Fail");
    SetWidget(IM_CONT[IM_CONT_AU_FAIL][IM_CONT_MAIN],"IM_C_AU_Fail");

    SetWidget(IM_CONT[IM_CONT_DEL][IM_CONT_MAIN],"IM_C_Del");
    SetWidget(IM_CONT[IM_CONT_DEL_SUC][IM_CONT_MAIN],"IM_C_Del_Suc");
    SetWidget(IM_CONT[IM_CONT_DEL_FAIL1][IM_CONT_MAIN],"IM_C_Del_Fail1");
    SetWidget(IM_CONT[IM_CONT_DEL_FAIL2][IM_CONT_MAIN],"IM_C_Del_Fail2");

    SetWidget(IM_CONT[IM_CONT_YN_MODE][IM_CONT_MAIN],"IM_YN_Mode");

    SetWidget(IM_CONT[IM_CONT_BTN_REG][IM_CONT_MAIN],"IM_C_Mode");
    SetWidget(IM_CONT[IM_CONT_BTN_DEL][IM_CONT_MAIN],"IM_C_Mode");
    SetWidget(IM_CONT[IM_CONT_BTN_YES][IM_CONT_MAIN],"IM_YN_Mode");
    SetWidget(IM_CONT[IM_CONT_BTN_NO][IM_CONT_MAIN], "IM_YN_Mode");

    SetWidget(IM_D7_BTN, "IM_D7_BTN");
    
    //printf("[MenuIM_InitControl] #1 OK.\n");
    //Sub Container setting
    for(int j=0; j<5 ;j++)//16,17
    {
        memset(tmpWidgetName,0,sizeof(tmpWidgetName)); //only 1
        sprintf(tmpWidgetName, "IM_C_Mode");
        SetWidget(IM_CONT[IM_CONT_MODE][j],tmpWidgetName);

        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_C_Mode_Reg_LANG%d",j+1);
        SetWidget(IM_CONT[IM_CONT_BTN_REG][j],tmpWidgetName);
        
        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_C_Mode_Del_LANG%d",j+1);
        SetWidget(IM_CONT[IM_CONT_BTN_DEL][j],tmpWidgetName);
    }

    //printf("[MenuIM_InitControl] #2 OK.\n");
    for(int j=0; j<5 ;j++)//10
    {
        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_C_AU_Fail_LANG%d",j+1);
        SetWidget(IM_CONT[IM_CONT_AU_FAIL][j],tmpWidgetName);
    }

    //printf("[MenuIM_InitControl] #3 OK.\n");
    for(int i=IM_CONT_REG1;i<=IM_CONT_REG8;i++) //1~8
    {
        for(int j=0; j<5 ;j++)
        {
            memset(tmpWidgetName,0,sizeof(tmpWidgetName));
            sprintf(tmpWidgetName, "IM_C_Reg%d_LANG%d",i,j+1);
            SetWidget(IM_CONT[i][j],tmpWidgetName);
        }
    }

    //printf("[MenuIM_InitControl] #4 OK.\n");
    for(int j=0; j<5 ;j++)//9
    {
        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_C_Reg_Fail_LANG%d",j+1);
        SetWidget(IM_CONT[IM_CONT_REG_FAIL][j],tmpWidgetName);
    }

    //printf("[MenuIM_InitControl] #5 OK.\n");
    for(int j=0; j<5 ;j++)
    {
        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_C_Del_LANG%d",j+1);
        SetWidget(IM_CONT[IM_CONT_DEL][j],tmpWidgetName);

        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_C_Del_Suc_LANG%d",j+1);
        SetWidget(IM_CONT[IM_CONT_DEL_SUC][j],tmpWidgetName);

        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_C_Del_Fail1_LANG%d",j+1);
        SetWidget(IM_CONT[IM_CONT_DEL_FAIL1][j],tmpWidgetName);

        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_C_Del_Fail2_LANG%d",j+1);
        SetWidget(IM_CONT[IM_CONT_DEL_FAIL2][j],tmpWidgetName);
    }

    //printf("[MenuIM_InitControl] #6 OK.\n");
    for(int j=0; j<5 ;j++)
    {
        memset(tmpWidgetName,0,sizeof(tmpWidgetName)); //only 1
        sprintf(tmpWidgetName, "IM_YN_Mode");
        SetWidget(IM_CONT[IM_CONT_YN_MODE][j],tmpWidgetName);

        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_YN_Mode_Yes_LANG%d",j+1);
        SetWidget(IM_CONT[IM_CONT_BTN_YES][j],tmpWidgetName);
        
        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_YN_Mode_No_LANG%d",j+1);
        SetWidget(IM_CONT[IM_CONT_BTN_NO][j],tmpWidgetName);
    }

    //printf("[MenuIM_InitControl] #7 OK.\n");
    for(int i=IM_CONT_MODE; i<=IM_CONT_BTN_NO;i++)
    {
        ituWidgetSetVisible(IM_CONT[i][IM_CONT_MAIN],false);
        for(int j=0;j<6;j++) //Lang_ENG~
        {
            ituWidgetSetVisible(IM_CONT[i][j],false);
        }
    }

    ituWidgetSetVisible(IM_D7_BTN,false);
    
    printf("[MenuIM_InitControl] OK.\n");
}

void MenuIM_InitControl_ANI()
{    
    char tmpWidgetName[40];

    for(int j=0; j<5 ;j++)
    {
        int index = j+1;
        //register button
        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_Reg_BTN_LANG%d",index);
        SetWidget(IM_BTN_S[IM_BTN_REG][j],tmpWidgetName);

        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_Reg_ANI_LANG%d",index);
        SetWidget(IM_BTN_ANI[IM_BTN_REG][j],tmpWidgetName);

        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_Reg_ZBTN_LANG%d",index);
        SetWidget(IM_BTN_L[IM_BTN_REG][j],tmpWidgetName);

        //delete button
        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_Del_BTN_LANG%d",index);
        SetWidget(IM_BTN_S[IM_BTN_DEL][j],tmpWidgetName);

        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_Del_ANI_LANG%d",index);
        SetWidget(IM_BTN_ANI[IM_BTN_DEL][j],tmpWidgetName);

        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_Del_ZBTN_LANG%d",index);
        SetWidget(IM_BTN_L[IM_BTN_DEL][j],tmpWidgetName);

        //yes button
        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_Yes_BTN_LANG%d",index);
        SetWidget(IM_BTN_S[IM_BTN_YES][j],tmpWidgetName);

        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_Yes_ANI_LANG%d",index);
        SetWidget(IM_BTN_ANI[IM_BTN_YES][j],tmpWidgetName);

        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_Yes_ZBTN_LANG%d",index);
        SetWidget(IM_BTN_L[IM_BTN_YES][j],tmpWidgetName);

        //no button
        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_No_BTN_LANG%d",index);
        SetWidget(IM_BTN_S[IM_BTN_NO][j],tmpWidgetName);

        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_No_ANI_LANG%d",index);
        SetWidget(IM_BTN_ANI[IM_BTN_NO][j],tmpWidgetName);

        memset(tmpWidgetName,0,sizeof(tmpWidgetName));
        sprintf(tmpWidgetName, "IM_No_ZBTN_LANG%d",index);
        SetWidget(IM_BTN_L[IM_BTN_NO][j],tmpWidgetName);
    }

    for(int i=IM_BTN_REG; i<=IM_BTN_NO;i++)
    {
        for(int j=0;j<5;j++) //Lang_ENG~
        {
            ituWidgetSetVisible(IM_BTN_S[i][j],false);
            ituWidgetSetVisible(IM_BTN_ANI[i][j],false);
            ituWidgetSetVisible(IM_BTN_L[i][j],false);
        }
    }

    printf("[MenuIM_InitControl_ANI] OK.\n");
}


void MenuIM_Send_TX(uint8_t nByte, uint8_t nData)
{
    uint8_t  txbuffer[8] = {0xFF,};//chpark.24.10.22
   
    txbuffer[nByte] = nData;
    CanbusTxDataManagement( gMainTimerCnt_10m,IMM_I,txbuffer);//chpark.10.30  
    printf("[MenuIM_Send_TX] nByte:%d  nData:%d \n",nByte,nData);
}

void MenuIM_Send_100m_TX(uint8_t nByte, uint8_t nData, int nNextStep)
{
    if(IM_b_100m_DataSend || IM_b_100m_ResetSend)
    {
        printf("[MenuIM_Send_100m_TX][Error] IM_b_100m_DataSend=%d IM_b_100m_ResetSend=%d\n",IM_b_100m_DataSend,IM_b_100m_ResetSend);
        return;
    }

    IM_b_100m_ResetSend = false;

    IM_n_100m_SendByte  = nByte;
    IM_n_100m_SendData  = nData;
    IM_b_100m_DataSend  = true;
    IM_n_100m_NextStep  = nNextStep;
}

void MenuIM_RcvData(int nData)
{
    if(!IM_b_GUI_Ready)
    {
        printf("[MenuIM_RcvData] NOT GUI Ready. nData=%d g_IM_RecvTime=%d\n",nData,g_IM_RecvTime);
        return;
    }

    if(nData>100)
    {
        nData = nData-100;
    }
    g_IM_RecvTime = 0;
    printf("[MenuIM_RcvData] nData=%d g_IM_RecvTime=%d\n",nData,g_IM_RecvTime);
    int nShowMode = IM_CONT_MODE; 

    switch(nData)
    {
        case 1: 
            nShowMode = IM_CONT_MODE;
            break;
        case 2://reg1
            nShowMode = IM_CONT_REG1;
            break;
        case 3://reg2
            nShowMode = IM_CONT_REG2;
            break;
        case 4://reg3
            nShowMode = IM_CONT_REG3;
            break;
        case 5://reg4
            nShowMode = IM_CONT_REG4;
            break;
        case 6://reg5
            nShowMode = IM_CONT_REG5;
            break;
        case 7://reg6
            nShowMode = IM_CONT_REG6;
            break;
        case 8://del
            nShowMode = IM_CONT_REG_FAIL;
            break;
        case 9://del
            nShowMode = IM_CONT_DEL;
            break;
        case 10://del
            nShowMode = IM_CONT_DEL_SUC;
            break;
        case 11://delete fail1
            nShowMode = IM_CONT_DEL_FAIL1;
            break;
        case 12://delete fail2
            nShowMode = IM_CONT_DEL_FAIL2;
            break;
        case 13://au fail
            nShowMode = IM_CONT_AU_FAIL;
            break;
        
        default://default
            nShowMode = IM_CONT_MODE;
            break;
    }

    if(g_IM_Main_Step == nShowMode)
    {
       printf("[MenuIM_RcvData]Already Show this mode=%d\n",nShowMode); 
    }
    else
    {
        MenuIM_SetMode(nShowMode);
    }

    MenuIM_Send_TX(4,nData); //chpark.24.10.31
}

void MenuIM_SetMode(int nMode)
{
    printf("[MenuIM_SetMode]nMode=%d\n",nMode);

    for(int i=IM_CONT_MODE; i<=IM_CONT_BTN_NO;i++)
    {
        ituWidgetSetVisible(IM_CONT[i][IM_CONT_MAIN],false);
    }

    bool bStep_Changed = false;
    if(g_IM_Main_Step != nMode)
    {
        bStep_Changed = true;
    }
    g_IM_Main_Step = nMode;

    switch(nMode)
    {
        case IM_CONT_MODE: //
            ituWidgetSetVisible(IM_CONT[nMode][IM_CONT_MAIN],true);
            
            ituWidgetSetVisible(IM_CONT[IM_CONT_BTN_REG][gMB_Language],true);
            ituWidgetSetVisible(IM_CONT[IM_CONT_BTN_DEL][gMB_Language],true);

            ituWidgetSetVisible(IM_D7_BTN,true);
            //register/delete button
            if(bStep_Changed)
            {
                g_IM_RD_Select = IM_BTN_REG;
                IM_Button_Select(IM_BTN_REG);
            }
            else
            {
                IM_Button_Select(g_IM_RD_Select);
            }

            break;

        case IM_CONT_REG1:
        case IM_CONT_REG2:
        case IM_CONT_REG3:
        case IM_CONT_REG4:
        case IM_CONT_REG5:
        case IM_CONT_REG6:
            ituWidgetSetVisible(IM_CONT[nMode][IM_CONT_MAIN],true);
            ituWidgetSetVisible(IM_CONT[nMode][gMB_Language],true);
            ituWidgetSetVisible(IM_D7_BTN,false);
            break;

        case IM_CONT_REG7: //yes no
            ituWidgetSetVisible(IM_CONT[nMode][IM_CONT_MAIN],true);
            ituWidgetSetVisible(IM_CONT[IM_CONT_REG7][gMB_Language],true);

            ituWidgetSetVisible(IM_CONT[IM_CONT_YN_MODE][IM_CONT_MAIN],true);
            ituWidgetSetVisible(IM_CONT[IM_CONT_BTN_YES][gMB_Language],true);
            ituWidgetSetVisible(IM_CONT[IM_CONT_BTN_NO][gMB_Language],true);

            ituWidgetSetVisible(IM_D7_BTN,true);
            //yes/no button
            if(bStep_Changed)
            {
                g_IM_YN_Select = IM_BTN_YES;
                IM_Button_Select(IM_BTN_YES);
            }
            else
            {
                IM_Button_Select(g_IM_RD_Select);
            }
            break;

        case IM_CONT_REG8://yes no
            ituWidgetSetVisible(IM_CONT[nMode][IM_CONT_MAIN],true);
            ituWidgetSetVisible(IM_CONT[IM_CONT_REG8][gMB_Language],true);

            ituWidgetSetVisible(IM_CONT[IM_CONT_YN_MODE][IM_CONT_MAIN],true);
            ituWidgetSetVisible(IM_CONT[IM_CONT_BTN_YES][gMB_Language],true);
            ituWidgetSetVisible(IM_CONT[IM_CONT_BTN_NO][gMB_Language],true);

            ituWidgetSetVisible(IM_D7_BTN,true);

            //yes/no button
            if(bStep_Changed)
            {
                g_IM_YN_Select = IM_BTN_YES;
                IM_Button_Select(IM_BTN_YES);
            }
            else
            {
                IM_Button_Select(g_IM_RD_Select);
            }
            break;

        case IM_CONT_REG_FAIL:
        case IM_CONT_AU_FAIL:
        case IM_CONT_DEL:
        case IM_CONT_DEL_SUC:
        case IM_CONT_DEL_FAIL1:
        case IM_CONT_DEL_FAIL2:
            ituWidgetSetVisible(IM_CONT[nMode][IM_CONT_MAIN],true);
            ituWidgetSetVisible(IM_CONT[nMode][gMB_Language],true);
            ituWidgetSetVisible(IM_D7_BTN,false);
            break;
    
        default:
            ituWidgetSetVisible(IM_CONT[nMode][IM_CONT_MAIN],true);
            
            ituWidgetSetVisible(IM_CONT[IM_CONT_BTN_REG][gMB_Language],true);
            ituWidgetSetVisible(IM_CONT[IM_CONT_BTN_DEL][gMB_Language],true);

            ituWidgetSetVisible(IM_D7_BTN,true);
            //register/delete button
            if(bStep_Changed)
            {
                g_IM_RD_Select = IM_BTN_REG;
                IM_Button_Select(IM_BTN_REG);
            }
            else
            {
                IM_Button_Select(g_IM_RD_Select);
            }
            printf("[MenuIM_SetMode][default UI]ETC error\n");
            break;    
    }
}


bool MenuIM_Timer(ITUWidget* widget, char* param)
{
    #ifdef USED_ROTATION 
    if (RotationMode!=0)
    {
      return 0;
    }
    #endif

    if((gMainTimerCnt_10m - IM_SecTimer) >= 20)//100)//200 msec
	{
		if(IM_ExitCNT < 30*5)//30 sec
			IM_ExitCNT++;
		else if(IM_ExitCNT == 30*5)//30 sec
		{
			IM_ExitCNT = 300;
			//printf("\r\n SU100 Menu 30Sec Exit > goto Main\r\n");
			#ifdef RELEASE_VER
			GotoMain(MAIN_MODE_NORMAL); 
			#endif
		}

        IM_SecTimer = gMainTimerCnt_10m;
    }

    if((gMainTimerCnt_10m - IM_SecTimer) >= 10)//100)//100 msec
    {
        if(IM_b_100m_DataSend)
        {
            IM_b_100m_DataSend  = false;
            IM_b_100m_ResetSend = false;

            MenuIM_Send_TX(IM_n_100m_SendByte,IM_n_100m_SendData);
            IM_b_100m_ResetSend = true;
        }
        else if(IM_b_100m_ResetSend)
        {
            IM_b_100m_ResetSend = false;
            MenuIM_Send_TX(IM_n_100m_SendByte,0x00);
            MenuIM_SetMode(IM_n_100m_NextStep);
            printf("[MenuIM_Timer]Go Next Step:%d\n",IM_n_100m_NextStep);
        }

        IM_SecTimer = gMainTimerCnt_10m;//chpark.24.10.29
    }

    return true;
}
//=====================================================

bool IM_Button_Select(uint8_t nIndex)
{
    //버튼의 선택 까지만 나타낸다.
    //animation까지만 나타낸다.
    IM_ExitCNT = 0;
    printf("[IM_Button_Select]nIndex=%d\n",nIndex);
    switch(nIndex)
    {
        case IM_BTN_REG:
            if(g_IM_RD_Select == nIndex)
            {
                //same->Show Large Icon
                ituWidgetSetVisible(IM_BTN_S[IM_BTN_REG][gMB_Language],     false);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_REG][gMB_Language],   false);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_REG][gMB_Language],     true);

                ituWidgetSetVisible(IM_BTN_S[IM_BTN_DEL][gMB_Language],     true);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_DEL][gMB_Language],   false);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_DEL][gMB_Language],     false);

                nIM_BTN_Status[nIndex] = IM_BTN_STATUS_DOWN;
            }
            else
            {
                 //different->Animation
                g_IM_RD_Select = nIndex;
                
                nIM_BTN_Status[IM_BTN_REG] = IM_BTN_STATUS_NONE;
                ituWidgetSetVisible(IM_BTN_S[IM_BTN_REG][gMB_Language],     false);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_REG][gMB_Language],   true);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_REG][gMB_Language],     false);
                // 0; None ,1;Scale up  ,2;Scale Down
                nIM_Scaleflag[IM_BTN_REG] = IM_ANI_UP;
                printf("ituAnimationPlay(IM_BTN_ANI[IM_BTN_REG] start\n");
                ituAnimationPlay(IM_BTN_ANI[IM_BTN_REG][gMB_Language], 0);
                
                nIM_BTN_Status[IM_BTN_DEL] = IM_BTN_STATUS_NONE;
                ituWidgetSetVisible(IM_BTN_S[IM_BTN_DEL][gMB_Language],     false);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_DEL][gMB_Language],   true);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_DEL][gMB_Language],     false);
                // 0; None ,1;Scale up  ,2;Scale Down
                nIM_Scaleflag[IM_BTN_DEL] = IM_ANI_DOWN;
                printf("ituAnimationReversePlay(IM_BTN_ANI[IM_BTN_DEL] start\n");
                ituAnimationReversePlay(IM_BTN_ANI[IM_BTN_DEL][gMB_Language], 1);
                
            }
            break;
        
        case IM_BTN_DEL:
            if(g_IM_RD_Select == nIndex)
            {
                //same->Show Large Icon
                ituWidgetSetVisible(IM_BTN_S[IM_BTN_REG][gMB_Language],     true);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_REG][gMB_Language],   false);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_REG][gMB_Language],     false);

                ituWidgetSetVisible(IM_BTN_S[IM_BTN_DEL][gMB_Language],     false);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_DEL][gMB_Language],   false);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_DEL][gMB_Language],     true);

                nIM_BTN_Status[nIndex] = IM_BTN_STATUS_DOWN;
            }
            else
            {
                g_IM_RD_Select = nIndex;
                
                
                nIM_BTN_Status[IM_BTN_REG] = IM_BTN_STATUS_NONE;
                ituWidgetSetVisible(IM_BTN_S[IM_BTN_REG][gMB_Language],     false);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_REG][gMB_Language],   true);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_REG][gMB_Language],     false);
                // 0; None ,1;Scale up  ,2;Scale Down
                nIM_Scaleflag[IM_BTN_REG] = IM_ANI_DOWN;
                printf("ituAnimationReversePlay(IM_BTN_ANI[IM_BTN_REG] start\n");
                ituAnimationReversePlay(IM_BTN_ANI[IM_BTN_REG][gMB_Language], 1);
                
                nIM_BTN_Status[IM_BTN_DEL] = IM_BTN_STATUS_NONE;
                ituWidgetSetVisible(IM_BTN_S[IM_BTN_DEL][gMB_Language],     false);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_DEL][gMB_Language],   true);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_DEL][gMB_Language],     false);
                // 0; None ,1;Scale up  ,2;Scale Down
                nIM_Scaleflag[IM_BTN_DEL] = IM_ANI_UP;
                printf("ituAnimationPlay(IM_BTN_ANI[IM_BTN_DEL] start\n");
                ituAnimationPlay(IM_BTN_ANI[IM_BTN_DEL][gMB_Language], 0);
                
            }
            break;
        
        case IM_BTN_YES:
            if(g_IM_YN_Select == nIndex)
            {
                ituWidgetSetVisible(IM_BTN_S[IM_BTN_YES][gMB_Language],     false);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_YES][gMB_Language],   false);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_YES][gMB_Language],     true);

                ituWidgetSetVisible(IM_BTN_S[IM_BTN_NO][gMB_Language],      true);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_NO][gMB_Language],    false);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_NO][gMB_Language],      false);

                nIM_BTN_Status[nIndex] = IM_BTN_STATUS_DOWN;
            }
            else
            {
                g_IM_YN_Select = nIndex;
                
                nIM_BTN_Status[IM_BTN_YES] = IM_BTN_STATUS_NONE;
                ituWidgetSetVisible(IM_BTN_S[IM_BTN_YES][gMB_Language],     false);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_YES][gMB_Language],   true);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_YES][gMB_Language],     false);
                // 0; None ,1;Scale up  ,2;Scale Down
                nIM_Scaleflag[IM_BTN_YES] = IM_ANI_UP;
                ituAnimationPlay(IM_BTN_ANI[IM_BTN_YES][gMB_Language], 0);
                
                nIM_BTN_Status[IM_BTN_NO] = IM_BTN_STATUS_NONE;
                ituWidgetSetVisible(IM_BTN_S[IM_BTN_NO][gMB_Language],      false);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_NO][gMB_Language],    true);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_NO][gMB_Language],      false);
                // 0; None ,1;Scale up  ,2;Scale Down
                nIM_Scaleflag[IM_BTN_NO] = IM_ANI_DOWN;
                ituAnimationReversePlay(IM_BTN_ANI[IM_BTN_NO][gMB_Language], 1);
                
            }
            break;
        
        case IM_BTN_NO:
            if(g_IM_YN_Select == nIndex)
            {
                ituWidgetSetVisible(IM_BTN_S[IM_BTN_YES][gMB_Language],     true);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_YES][gMB_Language],   false);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_YES][gMB_Language],     false);
                
                ituWidgetSetVisible(IM_BTN_S[IM_BTN_NO][gMB_Language],      false);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_NO][gMB_Language],    false);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_NO][gMB_Language],      true);

                nIM_BTN_Status[nIndex] = IM_BTN_STATUS_DOWN;
            }
            else
            {
                g_IM_YN_Select = nIndex;

                nIM_BTN_Status[IM_BTN_YES] = IM_BTN_STATUS_NONE;
                ituWidgetSetVisible(IM_BTN_S[IM_BTN_YES][gMB_Language],     false);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_YES][gMB_Language],   true);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_YES][gMB_Language],     false);
                // 0; None ,1;Scale up  ,2;Scale Down
                nIM_Scaleflag[IM_BTN_YES] = IM_ANI_DOWN;
                ituAnimationReversePlay(IM_BTN_ANI[IM_BTN_YES][gMB_Language], 1);

                nIM_BTN_Status[IM_BTN_NO] = IM_BTN_STATUS_NONE;
                ituWidgetSetVisible(IM_BTN_S[IM_BTN_NO][gMB_Language],      false);
                ituWidgetSetVisible(IM_BTN_ANI[IM_BTN_NO][gMB_Language],    true);
                ituWidgetSetVisible(IM_BTN_L[IM_BTN_NO][gMB_Language],      false);
                // 0; None ,1;Scale up  ,2;Scale Down
                nIM_Scaleflag[IM_BTN_NO] = IM_ANI_UP;
                ituAnimationPlay(IM_BTN_ANI[IM_BTN_NO][gMB_Language], 0);
            }
            break;
        
        default:
            break;            
    }
    
    return true;
}

void IM_MenuTabEnter()
{
    switch(g_IM_Main_Step)
    {
        case IM_CONT_MODE:
            {
                if(g_IM_RD_Select == IM_BTN_REG)
                {
                    if(nIM_BTN_Status[IM_BTN_REG]==IM_BTN_STATUS_DOWN)
                    {
                        printf("[IM_MenuTabEnter][IM_BTN_REG]Enter\n");
                        MenuIM_SetMode(IM_CONT_REG7);
                    }
                    else
                    {
                        //wait for ending animation.
                        printf("[IM_MenuTabEnter][IM_BTN_REG]wait for ani ending\n");
                        nIM_BTN_Status[IM_BTN_REG] = IM_BTN_STATUS_UP;
                    }
                }
                else if(g_IM_RD_Select == IM_BTN_DEL)
                {
                    if(nIM_BTN_Status[IM_BTN_DEL]==IM_BTN_STATUS_DOWN)
                    {
                        printf("[IM_MenuTabEnter][IM_BTN_DEL]Enter\n");
                        MenuIM_Send_100m_TX(3,0x04,IM_CONT_DEL);//chpark.24.10.22
                    }
                    else
                    {
                        //wait for ending animation.
                        printf("[IM_MenuTabEnter][IM_BTN_DEL]wait for ani ending\n");
                        nIM_BTN_Status[IM_BTN_DEL] = IM_BTN_STATUS_UP;
                    }
                }
            }
            break;

        case IM_CONT_REG7:
            {
                if(g_IM_YN_Select == IM_BTN_YES)
                {
                    if(nIM_BTN_Status[IM_BTN_YES]==IM_BTN_STATUS_DOWN)
                    {
                        printf("[IM_MenuTabEnter][IM_BTN_YES]Enter\n");
                        MenuIM_SetMode(IM_CONT_REG8);
                    }
                    else
                    {
                        //wait for ending animation.
                        printf("[IM_MenuTabEnter][IM_BTN_YES]wait for ani ending\n");
                        nIM_BTN_Status[IM_BTN_YES] = IM_BTN_STATUS_UP;
                    }
                }
                else if(g_IM_YN_Select == IM_BTN_NO)
                {
                    if(nIM_BTN_Status[IM_BTN_NO]==IM_BTN_STATUS_DOWN)
                    {
                        printf("[IM_MenuTabEnter][IM_BTN_NO]Enter\n");
                        MenuIM_Send_100m_TX(3,0x03,IM_CONT_MODE);//chpark.24.10.22
                    }
                    else
                    {
                        //wait for ending animation.
                        printf("[IM_MenuTabEnter][IM_BTN_NO]wait for ani ending\n");
                        nIM_BTN_Status[IM_BTN_NO] = IM_BTN_STATUS_UP;
                    }
                }
            }
            break;

        case IM_CONT_REG8:
            {
                if(g_IM_YN_Select == IM_BTN_YES)
                {
                    if(nIM_BTN_Status[IM_BTN_YES]==IM_BTN_STATUS_DOWN)
                    {
                        printf("[IM_MenuTabEnter][IM_BTN_YES]Enter\n");
                        MenuIM_Send_100m_TX(3,0x0B,IM_CONT_REG7);//chpark..24.10.22
                    }
                    else
                    {
                        //wait for ending animation.
                        printf("[IM_MenuTabEnter][IM_BTN_YES]wait for ani ending\n");
                        nIM_BTN_Status[IM_BTN_YES] = IM_BTN_STATUS_UP;
                    }
                }
                else if(g_IM_YN_Select == IM_BTN_NO)
                {
                    if(nIM_BTN_Status[IM_BTN_NO]==IM_BTN_STATUS_DOWN)
                    {
                        printf("[IM_MenuTabEnter][IM_BTN_NO]Enter\n");
                        MenuIM_SetMode(IM_CONT_REG7);
                    }
                    else
                    {
                        //wait for ending animation.
                        printf("[IM_MenuTabEnter][IM_BTN_NO]wait for ani ending\n");
                        nIM_BTN_Status[IM_BTN_NO] = IM_BTN_STATUS_UP;
                    }
                }
            }
            break;

        default:
            break;
    }
}

void IM_MenuTabNext()
{
    int nNextButton = -1;

    switch(g_IM_Main_Step)
    {
        case IM_CONT_MODE: //register, delete
            if(g_IM_RD_Select==IM_BTN_REG)
            {
                nNextButton = IM_BTN_DEL;
            }
            else
            {
                nNextButton = IM_BTN_REG;
            }

            IM_Button_Select(nNextButton);
            break;

        case IM_CONT_REG7:
        case IM_CONT_REG8:
            //yes, no
            if(g_IM_YN_Select==IM_BTN_YES)
            {
                nNextButton = IM_BTN_NO;
            }
            else
            {
                nNextButton = IM_BTN_YES;
            }

            IM_Button_Select(nNextButton);
            break;
    
        default:
            break;  
    }

    if(nNextButton>=0)
    {

    }
    else
    {
        printf("[IM_MenuTabNext]Error==\n");
    }
}

void IM_MenuTabESC()
{
    int nNextStep = -1;

    switch(g_IM_Main_Step)
    {
        case IM_CONT_MODE: //
            g_IM_b_ExistLayer = false;
            GotoMain(MAIN_MODE_NORMAL); 
            break;
        case IM_CONT_REG1:
            nNextStep = IM_CONT_REG7;
            break;
        case IM_CONT_REG2:
            nNextStep = IM_CONT_REG1;
            break;
        case IM_CONT_REG3:
            nNextStep = IM_CONT_REG2;
            break;
        case IM_CONT_REG4:
            nNextStep = IM_CONT_REG3;
            break;
        case IM_CONT_REG5:
        case IM_CONT_REG6:
            nNextStep = IM_CONT_REG4;
            break;
        case IM_CONT_REG7:
            nNextStep = IM_CONT_MODE;
            break;
        case IM_CONT_REG8:
            nNextStep = IM_CONT_REG7;
            break;
        case IM_CONT_DEL:
            nNextStep = IM_CONT_MODE;
            break;
        case IM_CONT_REG_FAIL:
            nNextStep = IM_CONT_REG2;
            break;
        case IM_CONT_AU_FAIL:
            nNextStep = IM_CONT_MODE;
            break;
        case IM_CONT_DEL_SUC:
        case IM_CONT_DEL_FAIL1:
        case IM_CONT_DEL_FAIL2:
            nNextStep = IM_CONT_DEL;
            break;
        //case IM_CONT_YN_MODE:
            //do nothing
            //break;

        default:
            break;
    }

    if(nNextStep>=0)
    {
        MenuIM_SetMode(nNextStep);
    }
    else
    {
        printf("[IM_MenuTabNext]Error==\n");
    }
}

//=====================================================
void IM_ChoiceBtnClear()
{
    for(int i=0; i<4; i++)
    {
        nIM_Scaleflag[i] = IM_ANI_NONE;
        nIM_BTN_Status[i]= IM_BTN_STATUS_NONE;
    }
}


void IM_ANIStopped_Set_Button_State(int nIndex)
{
    if(nIM_Scaleflag[nIndex] == IM_ANI_UP) 
    {
        printf("[IM_ANIStopped_Set_Button_State]nIndex=%d nIM_Scaleflag[nIndex]=%d \n",nIndex,nIM_Scaleflag[nIndex]);
        //ituAnimationGoto(IM_BTN_ANI[IM_BTN_REG], 1);
        ituWidgetSetVisible(IM_BTN_S[nIndex][gMB_Language],     false);
        ituWidgetSetVisible(IM_BTN_ANI[nIndex][gMB_Language],   false);
        ituWidgetSetVisible(IM_BTN_L[nIndex][gMB_Language],     true);
        nIM_Scaleflag[nIndex]  = IM_ANI_NONE;
        nIM_BTN_Status[nIndex] = IM_BTN_STATUS_DOWN;
    }
    else if(nIM_Scaleflag[nIndex] == IM_ANI_DOWN)
    {
        printf("[IM_ANIStopped_Set_Button_State]nIndex=%d nIM_Scaleflag[nIndex]=%d \n",nIndex,nIM_Scaleflag[nIndex]);
        //ituAnimationGoto(IM_BTN_ANI[IM_BTN_REG], 0);
        ituWidgetSetVisible(IM_BTN_S[nIndex][gMB_Language],     true);
        ituWidgetSetVisible(IM_BTN_ANI[nIndex][gMB_Language],   false);
        ituWidgetSetVisible(IM_BTN_L[nIndex][gMB_Language],     false);

        nIM_Scaleflag[nIndex]  = IM_ANI_NONE;
        nIM_BTN_Status[nIndex] = IM_BTN_STATUS_NONE;
    }
}

bool IM_ANIStopped(ITUWidget* widget, char* param)
{
    int nIndex;   
    nIndex = atoi(param);
    nIndex = nIndex-1;

    IM_ANIStopped_Set_Button_State(nIndex);

    return true;
}

/*
bool IM_ANIStopped_R(ITUWidget* widget, char* param)
{
    int nIndex;   
    nIndex = atoi(param);
    nIndex = nIndex-1;

    IM_ANIStopped_Set_Button_State(nIndex);

    return true;
}

bool IM_ANIStopped_D(ITUWidget* widget, char* param)
{
     int nIndex;   
    nIndex = atoi(param);

    nIndex = nIndex-1;

    IM_ANIStopped_Set_Button_State(nIndex);

    return true;
}

bool IM_ANIStopped_Y(ITUWidget* widget, char* param)
{
    int nIndex;   
    nIndex = atoi(param);

    nIndex = nIndex-1;

    IM_ANIStopped_Set_Button_State(nIndex);

    return true;
}

bool IM_ANIStopped_N(ITUWidget* widget, char* param)
{
    int nIndex;   
    nIndex = atoi(param);

    nIndex = nIndex-1;

    IM_ANIStopped_Set_Button_State(nIndex);

    return true;
}
*/

//=====================================================
bool IM_BTN_MouseUp(ITUWidget* widget, char* param)
{
    IM_ExitCNT = 0;

    int nTouchIndex;   
    nTouchIndex = atoi(param);

    IM_MenuTabEnter();

    return true;
}

bool IM_BTN_MouseDown(ITUWidget* widget, char* param)
{
    IM_ExitCNT = 0;

    int nTouchIndex;   
    nTouchIndex = atoi(param);

    nTouchIndex = nTouchIndex-1;
    IM_Button_Select(nTouchIndex);

    return true;
}
//=====================================================

bool MenuIM_JogWheel(ITUWidget* widget, char* param)
{
    IM_ExitCNT = 0;

	uint8_t JogDir = 0;
	JogDir = param[0];
	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
		case LANDSCAPE_1:
		case LANDSCAPE_2:
            if(JogDir == JOG_DIAL_ROT_RIGHT)
			{
                IM_MenuTabNext();
			}
			else if(JogDir == JOG_DIAL_ROT_LEFT)
			{
                IM_MenuTabNext();
			}
		break;
	}

    return true;
}

bool MenuIM_JogButton(ITUWidget* widget, char* param)
{
    IM_ExitCNT = 0;
    uint8_t JogButton = 0;
	JogButton = param[0];

	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
		case LANDSCAPE_1:
		case LANDSCAPE_2:
			switch(JogButton)
			{
				case JOG_DIAL_KEY_UP:
                    IM_MenuTabNext();
					break;

				case JOG_DIAL_KEY_DOWN:
                    IM_MenuTabNext();
					break;
				
				case JOG_DIAL_KEY_LEFT:
				case JOG_DIAL_KEY_RIGHT:
					//IM_MenuTabNext();
					break;

				case JOG_DIAL_KEY_ENTER_OFF:				//c3Byte0bit
        		case JOG_DIAL_KEY_NONE:
				{
             		IM_MenuTabEnter();
        		}
					break;
			}
		break;
	}

    return true;
}

bool MenuIM_JogSwitch(ITUWidget* widget, char* param)
{
    IM_ExitCNT = 0;
    uint8_t JogSwitch = 0;
	JogSwitch = param[0];

	if(JogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
       //IM_MenuTabESC(); //chpark.24.10.30
	}
	else if(JogSwitch == JOG_DIAL_SW_HOME_OFF)
	{
		//GotoMain(MAIN_MODE_NORMAL); //chpark.24.10.30
	}
    
    return true;
}

bool IM_D7_BTNMouseUp(ITUWidget* widget, char* param)
{
    IM_ExitCNT = 0;
    //for real
	IM_MenuTabEnter();
    /*
    //for test.
    int nTestStep = g_IM_Main_Step;
    if(g_IM_Main_Step>=IM_CONT_MODE && g_IM_Main_Step<=IM_CONT_DEL_FAIL2)
    {
        nTestStep++;
    }
    else
    {
        nTestStep = 0;
    }
    
    printf("[IM_D7_BTNMouseUp] nTestStep=%d g_IM_Main_Step=%d\n",nTestStep,g_IM_Main_Step);

    MenuIM_SetMode(nTestStep);
    */
    return true;
}

bool IM_D6_BTNMouseUp(ITUWidget* widget, char* param)
{
    IM_ExitCNT = 0;

    g_IM_b_ExistLayer = false;

	#ifdef RELEASE_VER

    #else
    GotoMain(MAIN_MODE_NORMAL);
    #endif 

    return true;
}

bool IM_D5_BTNMouseUp(ITUWidget* widget, char* param)
{
    IM_ExitCNT = 0;
    //do nothing
    //IM_MenuTabESC(); //chpark.24.10.30
   
    return true;
}

