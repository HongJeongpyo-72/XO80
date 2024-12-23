#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

bool MB400_D7_MouseDn(ITUWidget* widget, char* param);

#define MB400_UNLOCK		0
#define MB400_LOCK		    1

static uint32_t MB400_gMT_Timer;
static bool     MB400_bClick_Down = false;
static bool     MB400_bClick_Up   = false;

bool MB400_Send_TX(uint8_t nIndex);//chpark.06.08
void MB400_Button_ClickUI(uint8_t nIndex, bool bDown);//chpark.24.01.26

static uint8_t MB400_MAX_BUTTON_NUM=6;
static uint8_t MB400_MAX_PEDAL_BUTTON_NUM=3;
static uint8_t MB400_MAX_BLADE_BUTTON_NUM=3;
//static uint8_t MB400_MAX_LEFT_BUTTON_NUM=3;
//static uint8_t MB400_MAX_RIGHT_BUTTON_NUM=3;

typedef struct _B400ButtonState
{
	uint8_t nIndex;	
	uint8_t nLock;	  //0:UnLock 	1: Lock
	uint8_t nSend;	  //0: none  1:sent step1  2:sent step2
	uint8_t nRecv;	  //0: none  1:recv step1  2:recv step2 
	uint8_t nSelect;  //0:not selected 1:selected
}B400ButtonState;	

int8_t MB400_Choice_Index;

static B400ButtonState     	gCal_Button[6];  		
static ITUIcon*     		MB400_ICON_LOCK[6];		
static ITUIcon*     		MB400_ICON_SEL[6];		

static ITUIcon*     		MB400_LANG[5];       	
static ITUButton* 			MB400_BUTTON[6];		

void MenuB400_InitControl();
void MenuB400_SetLang(uint8_t nIndex);
void MB400_Button_Display(uint8_t nIndex);
bool MB400_Button_Select(uint8_t nIndex);

void MB400_MenuTabNext();
void MB400_MenuTabPrev();
int8_t MB400_Button_GetNextIndex(uint8_t nIndex);
int8_t MB400_Button_GetPrevIndex(uint8_t nIndex);

void MB400_MenuTabPrev_InPedal();
void MB400_MenuTabPrev_InPedal();
int8_t MB400_Button_GetNextIndex_InPedal(uint8_t nIndex);
int8_t MB400_Button_GetPrevIndex_InPedal(uint8_t nIndex);

void MB400_MenuTabPrev_InBlade();
void MB400_MenuTabPrev_InBlade();
int8_t MB400_Button_GetNextIndex_InBlade(uint8_t nIndex);
int8_t MB400_Button_GetPrevIndex_InBlade(uint8_t nIndex);

void MB400_MenuTabUpDown();

bool MenuB400_Enter(ITUWidget* widget, char* param)
{
	//printf("MenuB400_Enter=== %d\n",gMB_Language);
    MB400_bClick_Down = false;
    MB400_bClick_Up   = false;

    MB400_gMT_Timer = gMainTimerCnt_10m;
   
    MenuB400_InitControl();

    MenuB400_SetLang(gMB_Language);	
 
    for(int i=0;i<6;i++)
    {
        //printf("\r\n MenuB400_Enter--333--- %d\r\n",i);
        gCal_Button[i].nLock = MB400_UNLOCK;
        gCal_Button[i].nSelect = 0;
    }
	   
    //===================================
    gCal_Button[0].nLock = MB400_UNLOCK;
    gCal_Button[1].nLock = MB400_LOCK;
    gCal_Button[2].nLock = MB400_LOCK;
    gCal_Button[3].nLock = MB400_UNLOCK;
    gCal_Button[4].nLock = MB400_LOCK;
    gCal_Button[5].nLock = MB400_LOCK;
    //===================================
    
    gCal_Button[0].nSelect = true;
    MB400_Choice_Index = 0;//chpark.24.03.29-

    for(int i=0;i<6;i++)
    {
        MB400_Button_Display(i); 
    }
	//printf("\r\n MenuB400_Enter--444---\r\n");
    return true;
}

void MenuB400_InitControl()
{
	MB400_LANG[0] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_LANG1");
    MB400_LANG[1] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_LANG2");
    MB400_LANG[2] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_LANG3");
    MB400_LANG[3] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_LANG4");
    MB400_LANG[4] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_LANG5");
//printf("\r\n MenuB400_InitControl--111---\r\n");
    MB400_ICON_LOCK[0] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_C1_OFF");
	MB400_ICON_LOCK[1] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_C2_OFF");
	MB400_ICON_LOCK[2] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_C3_OFF");
	MB400_ICON_LOCK[3] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_C4_OFF");
	MB400_ICON_LOCK[4] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_C5_OFF");
	MB400_ICON_LOCK[5] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_C6_OFF");
//printf("\r\n MenuB400_InitControl--222---\r\n");
    MB400_ICON_SEL[0] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_C1_SEL");
	MB400_ICON_SEL[1] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_C2_SEL");
	MB400_ICON_SEL[2] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_C3_SEL");
	MB400_ICON_SEL[3] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_C4_SEL");
	MB400_ICON_SEL[4] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_C5_SEL");
	MB400_ICON_SEL[5] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB400_C6_SEL");    
//printf("\r\n MenuB400_InitControl--3333---\r\n");
	MB400_BUTTON[0] =  (ITUButton* )ituSceneFindWidget(&theScene, "MenuB400_C1_BTN");
    MB400_BUTTON[1] =  (ITUButton* )ituSceneFindWidget(&theScene, "MenuB400_C2_BTN");
    MB400_BUTTON[2] =  (ITUButton* )ituSceneFindWidget(&theScene, "MenuB400_C3_BTN");
    MB400_BUTTON[3] =  (ITUButton* )ituSceneFindWidget(&theScene, "MenuB400_C4_BTN");
    MB400_BUTTON[4] =  (ITUButton* )ituSceneFindWidget(&theScene, "MenuB400_C5_BTN");
	MB400_BUTTON[5] =  (ITUButton* )ituSceneFindWidget(&theScene, "MenuB400_C6_BTN");
//printf("\r\n MenuB400_InitControl--4444---\r\n");
}

void MenuB400_SetLang(uint8_t nIndex)
{	
	for(int i=0;i<5;i++)
    {
		//printf("\r\n MenuB400_SetLang--1111---%d\r\n",i);
        ituWidgetSetVisible(MB400_LANG[i],false);       
    }
	//printf("\r\n MenuB400_SetLang--nIndex---%d\r\n",nIndex);

    ituWidgetSetVisible(MB400_LANG[nIndex],true);
}

void MB400_Button_ClickUI(uint8_t nIndex,bool bDown)
{
    //printf("MB400_Button_ClickUI:%d  ====\n",nIndex);  
    if(gCal_Button[nIndex].nLock==MB400_UNLOCK )
    {
        if(bDown)
        {  
            //printf("MB400_Button_ClickUI Show:%d Down:%d>>>>>====\n",MB400_Choice_Index,bDown  );
            ituWidgetSetVisible(MB400_ICON_LOCK[MB400_Choice_Index],true);
            //usleep(1000*1000);
        }
        else 
        {   
            //printf("MB400_Button_ClickUI Hide:%d Down:%d>>>>====\n",MB400_Choice_Index,bDown  );
            ituWidgetSetVisible(MB400_ICON_LOCK[MB400_Choice_Index],false);
        }
    }
}

void MB400_Button_Display(uint8_t nIndex)
{
    //printf("MB400_Button_Display:%d  ====\n",nIndex  );    
    if(gCal_Button[nIndex].nLock==MB400_LOCK)
    {
        //printf("MB400_Button_Display:%d  MB400_LOCK====\n",nIndex  );
		ituWidgetSetVisible(MB400_BUTTON[nIndex],false);
        ituWidgetSetVisible(MB400_ICON_LOCK[nIndex],true);
        ituWidgetSetVisible(MB400_ICON_SEL[nIndex],false);
	
        if(gCal_Button[nIndex].nSelect==1)
        {
           
        }
        else
        {
           
        }
    }
    else if(gCal_Button[nIndex].nLock==MB400_UNLOCK )
    {
        //printf("MB400_Button_Display:%d  MB400_ICON_UNLOCK====\n",nIndex);
		ituWidgetSetVisible(MB400_BUTTON[nIndex],true);
        ituWidgetSetVisible(MB400_ICON_LOCK[nIndex],false);
        
        if(gCal_Button[nIndex].nSelect==1)
        {
            ituWidgetSetVisible(MB400_ICON_SEL[nIndex],true);
        }
        else
        {
            ituWidgetSetVisible(MB400_ICON_SEL[nIndex],false);
        }
    }   
}


int8_t MB400_Button_GetNextIndex(uint8_t nIndex)
{
	int8_t nRet=-1;
    
    for(int i=nIndex+1; i<MB400_MAX_BUTTON_NUM ; i++)
    {
        if(gCal_Button[i].nLock==MB400_UNLOCK)
        {
            nRet = i;
            break;
        }
    }
    
    if(nRet<0)
    {
        for(int i=0;i<=nIndex;i++)
        {
            if(gCal_Button[i].nLock==MB400_UNLOCK)
            {
                nRet = i;
                break;
            }
        }
    }

    //printf("MB400_Button_GetNextIndex %d===\n",nRet);
    return nRet;
}

int8_t MB400_Button_GetPrevIndex(uint8_t nIndex)
{
	int8_t nRet=-1;
    
    for(int i=nIndex-1; i>=0 ; i--)
    {
        if(gCal_Button[i].nLock==MB400_UNLOCK )
        {
            nRet = i;
            break;
        }
    }
    
    if(nRet<0)
    {
        for(int i=MB400_MAX_BUTTON_NUM-1;i>=nIndex;i--)
        {
            if(gCal_Button[i].nLock==MB400_UNLOCK)
            {
                nRet = i;
                break;
            }

        }
    }
    //printf("MB400_Button_GetPrevIndex %d===\n",nRet);
    return nRet;
}

int8_t MB400_Button_GetNextIndex_InPedal(uint8_t nIndex)
{
	int8_t nRet=-1;
    
    for(int i=nIndex+1; i<MB400_MAX_PEDAL_BUTTON_NUM ; i++)
    {
        if(gCal_Button[i].nLock==MB400_UNLOCK)
        {
            nRet = i;
            break;
        }
    }

    if(nRet<0)
    {
        for(int i=0;i<=nIndex;i++)
        {
            if(gCal_Button[i].nLock==MB400_UNLOCK)
            {
                nRet = i;
                break;
            }
        }
    }

    //printf("MB400_Button_GetNextIndex_InPedal %d===\n",nRet);
    return nRet;
}

int8_t MB400_Button_GetPrevIndex_InPedal(uint8_t nIndex)
{
	int8_t nRet=-1;

    for(int i=nIndex-1; i>=0 ; i--)
    {
        if(gCal_Button[i].nLock==MB400_UNLOCK )
        {
            nRet = i;
            break;
        }
    }
 
    if(nRet<0)
    {
        for(int i=MB400_MAX_PEDAL_BUTTON_NUM-1;i>=nIndex;i--)
        {
            if(gCal_Button[i].nLock==MB400_UNLOCK)
            {
                nRet = i;
                break;
            }
        }
    }

    //printf("MB400_Button_GetPrevIndex_InPedal %d===\n",nRet);
    return nRet;
}

int8_t MB400_Button_GetNextIndex_InBlade(uint8_t nIndex)
{
	int8_t nRet=-1;
   
    for(int i=nIndex+1; i<(MB400_MAX_PEDAL_BUTTON_NUM+MB400_MAX_BLADE_BUTTON_NUM) ; i++)
    {
        if(gCal_Button[i].nLock==MB400_UNLOCK)
        {
            nRet = i;
            break;
        }
    }
    
    if(nRet<0)
    {
        for(int i=MB400_MAX_PEDAL_BUTTON_NUM;i<=nIndex;i++)
        {
            if(gCal_Button[i].nLock==MB400_UNLOCK)
            {
                nRet = i;
                break;
            }
        }
    }

    //printf("MB400_Button_GetNextIndex_InPedal %d===\n",nRet);
    return nRet;
}

int8_t MB400_Button_GetPrevIndex_InBlade(uint8_t nIndex)
{
	int8_t nRet=-1;
    
    for(int i=nIndex-1; i>=MB400_MAX_PEDAL_BUTTON_NUM ; i--)
    {
        if(gCal_Button[i].nLock==MB400_UNLOCK )
        {
            nRet = i;
            break;
        }
    }
    
    if(nRet<0)
    {
        for(int i=(MB400_MAX_PEDAL_BUTTON_NUM+MB400_MAX_BLADE_BUTTON_NUM-1); i>=nIndex ;i--)
        {
            if(gCal_Button[i].nLock==MB400_UNLOCK)
            {
                nRet = i;
                break;
            }
        }
    }

    //printf("MB400_Button_GetPrevIndex_InPedal %d===\n",nRet);
    return nRet;
}


void MB400_MenuTabNext()
{
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = MB400_Choice_Index;
    nTmpBtnIndex = MB400_Button_GetNextIndex(MB400_Choice_Index);
    if(nTmpBtnIndex>=0 && nTmpBtnIndex<MB400_MAX_BUTTON_NUM)
    {
        MB400_Button_Select(nTmpBtnIndex);
    }
    else
    {
       
    }
}


void MB400_MenuTabPrev()
{
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = MB400_Choice_Index;
    nTmpBtnIndex = MB400_Button_GetPrevIndex(MB400_Choice_Index);
    if(nTmpBtnIndex>=0 && nTmpBtnIndex<MB400_MAX_BUTTON_NUM)
    {
        MB400_Button_Select(nTmpBtnIndex);
    }
    else
    {
        
    }
}


void MB400_MenuTabNext_InPedal()
{
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = MB400_Choice_Index;
    nTmpBtnIndex = MB400_Button_GetNextIndex_InPedal(MB400_Choice_Index);
    if(nTmpBtnIndex>=0 && nTmpBtnIndex<MB400_MAX_BUTTON_NUM)
    {
        MB400_Button_Select(nTmpBtnIndex);
    }
    else
    {
        
    }
}


void MB400_MenuTabPrev_InPedal()
{
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = MB400_Choice_Index;
    nTmpBtnIndex = MB400_Button_GetPrevIndex_InPedal(MB400_Choice_Index);
    if(nTmpBtnIndex>=0 && nTmpBtnIndex<MB400_MAX_BUTTON_NUM)
    {
        MB400_Button_Select(nTmpBtnIndex);
    }
    else
    {

    }
}


void MB400_MenuTabNext_InBlade()
{
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = MB400_Choice_Index;
    nTmpBtnIndex = MB400_Button_GetNextIndex_InBlade(MB400_Choice_Index);
    if(nTmpBtnIndex>=0 && nTmpBtnIndex<MB400_MAX_BUTTON_NUM)
    {
        MB400_Button_Select(nTmpBtnIndex);
    }
    else
    {
       
    }
}


void MB400_MenuTabPrev_InBlade()
{
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = MB400_Choice_Index;
    nTmpBtnIndex = MB400_Button_GetPrevIndex_InBlade(MB400_Choice_Index);
    if(nTmpBtnIndex>=0 && nTmpBtnIndex<MB400_MAX_BUTTON_NUM)
    {
        MB400_Button_Select(nTmpBtnIndex);
    }
    else
    {
        
    }
}


void MB400_MenuTabUpDown()
{
	int8_t nTmpBtnIndex = MB400_Choice_Index;
	
	switch(nTmpBtnIndex)
	{
		case 0:
			nTmpBtnIndex = 3;
			break;
		case 1:
			nTmpBtnIndex = 4;
			break;
		case 2:
			nTmpBtnIndex = 5;
			break;
		case 3:
			nTmpBtnIndex = 0;
			break;
		case 4:
			nTmpBtnIndex = 1;
			break;
		case 5:
			nTmpBtnIndex = 2;
			break;
		default:
			break;
	}

	MB400_Button_Select(nTmpBtnIndex);
}


bool MB400_Button_Select(uint8_t nIndex)
{
    bool bRet = false;
	
	if(nIndex>=0 && nIndex<3)
	{
		//(Pedal) Calibration
    	if(gCal_Button[nIndex].nLock==MB400_UNLOCK )
		{
			for(int i=0; i<6 ;i++)
			{
				ituWidgetSetVisible(MB400_ICON_SEL[i],false);
                gCal_Button[i].nSelect = false;
			}
			//===============================
			gCal_Button[nIndex].nSelect = true;
			MB400_Choice_Index = nIndex;//
			//===============================
			//printf("\r\n MB400_Button_Select--666---\r\n");
			ituWidgetSetVisible(MB400_ICON_SEL[nIndex],true);
			//printf("MB400_Button_Select %d===\r\n\n\n",nIndex);
			bRet = true;
		}
		else
		{
			
		}
	}
	else 
	{
		//(Blade) Calibration
		if(gCal_Button[nIndex].nLock==MB400_UNLOCK )
		{
			for(int i=0; i<6 ;i++)
			{
				//printf("\r\n MB400_Button_Select--777---\r\n");
				ituWidgetSetVisible(MB400_ICON_SEL[i],false);
                gCal_Button[i].nSelect = false;
			}
			//===============================
			gCal_Button[nIndex].nSelect = true;
			MB400_Choice_Index = nIndex;//
			//===============================
			//printf("\r\n MB400_Button_Select--888---\r\n");
			ituWidgetSetVisible(MB400_ICON_SEL[nIndex],true);
			//printf("MB400_Button_Select %d===\r\n\n\n",nIndex);
			bRet = true;
		}
		else
		{

		}
	}

    return bRet;
}


bool MB400_Send_TX(uint8_t nIndex)
{
    bool bRet = false;
    uint8_t  txbuffer[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	
	if(nIndex>=0 && nIndex<3)
	{
		//(Pedal) Calibration
    	if(gCal_Button[nIndex].nLock==MB400_UNLOCK )
		{
			//printf("MB400_Send_TX: %d�?? Button===\r\n\n\n",nIndex);
            //start??��??�� : 0A 00 E4 00 01 00 00 00
            //neutral   : 0A 00 E4 00 02 00 00 00
            //write     : 0A 00 E4 00 04 00 00 00
            if(nIndex==0)
            {
                //0~1byte :10
                //2byte : 0xE4
                //4byte : 1
                txbuffer[0] = 0x0A;
                txbuffer[1] = 0x00;  
                txbuffer[2] = 0xE4;
				txbuffer[4] = 0x01;
				
                CanbusTxDataManagement( gMainTimerCnt_10m,TCMD1,txbuffer);//chpark.09.13
            }
            else if(nIndex==1)
            {
                //0~1byte :10
                //2byte : 0xE4
                //4byte : 2
                txbuffer[0] = 0x0A;
                txbuffer[1] = 0x00;  
                txbuffer[2] = 0xE4;
				txbuffer[4] = 0x02;
				
                CanbusTxDataManagement( gMainTimerCnt_10m,TCMD1,txbuffer);//chpark.09.13
            }
            else if(nIndex==2)
            {
                //0~1byte :10
                //2byte : 0xE4
                //4byte : 4
                txbuffer[0] = 0x0A;
                txbuffer[1] = 0x00;  
                txbuffer[2] = 0xE4;
				txbuffer[4] = 0x04;
				
                CanbusTxDataManagement( gMainTimerCnt_10m,TCMD1,txbuffer);//chpark.09.13
            }

			bRet = true;
		}
		else
		{
		
		}
	}
	else 
	{
        //start??��??�� : 0B 00 E4 00 01 00 00 00
        //neutral   : 0B 00 E4 00 02 00 00 00
        //write     : 0B 00 E4 00 04 00 00 00
		//(Blade) Calibration
		if(gCal_Button[nIndex].nLock==MB400_UNLOCK )
		{
			//printf("MB400_Send_TX: %d�?? Button===\r\n\n\n",nIndex);
            if(nIndex==3)
            {
                //0~1byte :11
                //2byte : 0xE4
                //4byte : 1
                txbuffer[0] = 0x0B;
                txbuffer[1] = 0x00;  
                txbuffer[2] = 0xE4;
				txbuffer[4] = 0x01;
				
                CanbusTxDataManagement( gMainTimerCnt_10m,TCMD1,txbuffer);//chpark.09.13
            }
            else if(nIndex==4)
            {
                //0~1byte :11
                //2byte : 0xE4
                //4byte : 2
                txbuffer[0] = 0x0B;
                txbuffer[1] = 0x00;  
                txbuffer[2] = 0xE4;
				txbuffer[4] = 0x02;
				
                CanbusTxDataManagement( gMainTimerCnt_10m,TCMD1,txbuffer);//chpark.09.13
            }
            else if(nIndex==5)
            {
                //0~1byte :11
                //2byte : 0xE4
                //4byte : 4
                txbuffer[0] = 0x0B;
                txbuffer[1] = 0x00;  
                txbuffer[2] = 0xE4;
				txbuffer[4] = 0x04;
				
                CanbusTxDataManagement( gMainTimerCnt_10m,TCMD1,txbuffer);//chpark.09.13
            }

			bRet = true;
		}
		else
		{
		
		}
	}

    return bRet;
}


bool MenuB400_Timer(ITUWidget* widget, char* param)
{
    #ifdef USED_ROTATION //chpark.10.19
    if (RotationMode!=0)
    {
      return 0;
    }
    #endif

    return true;
}

bool MenuB400_JogWheel(ITUWidget* widget, char* param)
{
    uint8_t JogDir = 0;
	JogDir = param[0];
		
	//printf("\nMenuB400_JogWheel %d============\r\n",JogDir);
	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
        case LANDSCAPE_1:
		case LANDSCAPE_2:
			//printf("PORTRATE\r\n");
			if(JogDir == JOG_DIAL_ROT_RIGHT)
			{
				MB400_MenuTabNext();
			}
			else if(JogDir == JOG_DIAL_ROT_LEFT)
			{
				MB400_MenuTabPrev();
			}

			break;
	}

    return true;
}

bool MenuB400_JogButton(ITUWidget* widget, char* param)
{
    uint8_t JogButton 	= 0;
    JogButton = param[0];

	uint8_t tMenuIndex 	= 0;
	//printf("MenuB400 JogButton %d\r\n",JogButton);
	
	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
			//printf("PORTRATE\r\n");
			switch(JogButton)
			{
				case JOG_DIAL_KEY_UP:
					MB400_MenuTabUpDown();
					break;

				case JOG_DIAL_KEY_DOWN:	
					MB400_MenuTabUpDown();
					break;
				
				case JOG_DIAL_KEY_LEFT:
					if(MB400_Choice_Index>=0 && MB400_Choice_Index<MB400_MAX_PEDAL_BUTTON_NUM)
					{
						MB400_MenuTabPrev_InPedal();
					}
					else
					{
						MB400_MenuTabPrev_InBlade();
					}
					break;

				case JOG_DIAL_KEY_RIGHT:
					if(MB400_Choice_Index>=0 && MB400_Choice_Index<MB400_MAX_PEDAL_BUTTON_NUM)
					{
						MB400_MenuTabNext_InPedal();
					}
					else
					{
						MB400_MenuTabNext_InBlade();
					}
					break;

				case JOG_DIAL_KEY_ENTER_ON:
                    //printf("JOG_DIAL_KEY_ENTER_ON===\n Command Send To MC\n");
                    MB400_Button_ClickUI(MB400_Choice_Index,true);//chpark.24.01.25
					break;

                case JOG_DIAL_KEY_ENTER_OFF:
                    MB400_Send_TX(MB400_Choice_Index);//chpark.24.01.25
                    MB400_Button_ClickUI(MB400_Choice_Index,false);
                    break;
			}
			break;
			
		case LANDSCAPE_1:
		case LANDSCAPE_2:
			switch(JogButton)
			{
				case JOG_DIAL_KEY_UP:
				case JOG_DIAL_KEY_DOWN:
					break;
				
				case JOG_DIAL_KEY_LEFT:
					MB400_MenuTabPrev();
					break;

				case JOG_DIAL_KEY_RIGHT:
					MB400_MenuTabNext();
					break;

				case JOG_DIAL_KEY_ENTER_ON:
                    //printf("JOG_DIAL_KEY_ENTER_ON===\n Command Send To MC\n");
                    MB400_Button_ClickUI(MB400_Choice_Index,true);//chpark.24.01.25

					break;
                case JOG_DIAL_KEY_ENTER_OFF:
                    MB400_Send_TX(MB400_Choice_Index);//chpark.24.01.25
                    MB400_Button_ClickUI(MB400_Choice_Index,false);
                    break;
			}
			break;
	}

    return true;
}

bool MenuB400_JogSwitch(ITUWidget* widget, char* param)
{
    uint8_t JogSwitch = 0;
	JogSwitch = param[0];

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


bool MenuB400_Event_Cal(ITUWidget* widget, char* param)
{
	//printf("MenuB400_Event_Cal===\n");
	uint16_t nTcmd_Msg = 0;
	nTcmd_Msg = param[0];
	
	switch (nTcmd_Msg)
	{
		case PEDAL_INIT:
		 	gCal_Button[0].nLock = MB400_UNLOCK;
			gCal_Button[1].nLock = MB400_LOCK;
			gCal_Button[2].nLock = MB400_LOCK;

			MB400_Button_Display(1);
			MB400_Button_Display(2);
			//printf("PEDAL_INIT===\n");
			break;
		
		case PEDAL_STEP2_OPEN:
			gCal_Button[0].nLock = MB400_UNLOCK;
			gCal_Button[1].nLock = MB400_UNLOCK;
			gCal_Button[2].nLock = MB400_LOCK;
			
			MB400_Button_Display(1);
			MB400_Button_Display(2);

			//printf("PEDAL_STEP2_OPEN===\n");
			break;
	
		case PEDAL_STEP3_OPEN:
			gCal_Button[0].nLock = MB400_UNLOCK;
			gCal_Button[1].nLock = MB400_UNLOCK;
			gCal_Button[2].nLock = MB400_UNLOCK;

			MB400_Button_Display(1);
			MB400_Button_Display(2);
			
            //printf("PEDAL_STEP3_OPEN===\n");
			break;

		case BLADE_INIT:
			gCal_Button[3].nLock = MB400_UNLOCK;
			gCal_Button[4].nLock = MB400_LOCK;
			gCal_Button[5].nLock = MB400_LOCK;

			MB400_Button_Display(4);
			MB400_Button_Display(5);
			//printf("BLADE_INIT===\n");
			break;

		case BLADE_STEP2_OPEN:
			gCal_Button[3].nLock = MB400_UNLOCK;
			gCal_Button[4].nLock = MB400_UNLOCK;
			gCal_Button[5].nLock = MB400_LOCK;

			MB400_Button_Display(4);
			MB400_Button_Display(5);

			//printf("BLADE_STEP2_OPEN===\n");
			break;

		case BLADE_STEP3_OPEN:
			gCal_Button[3].nLock = MB400_UNLOCK;
			gCal_Button[4].nLock = MB400_UNLOCK;
			gCal_Button[5].nLock = MB400_UNLOCK;

			MB400_Button_Display(4);
			MB400_Button_Display(5);
			//printf("BLADE_STEP3_OPEN===\n");
			break;

		default:
			printf("MenuB400_Event_Cal Error:%d===\n",nTcmd_Msg);

			break;
	}


	return true;
}


bool MB400_C_BTN_MouseDown(ITUWidget* widget, char* param)
{
	uint8_t nNewChoiceIndex = (uint8_t)atoi(param);
    int8_t nTmpBtnIndex = nNewChoiceIndex-1;
    
    if(nTmpBtnIndex<0)nTmpBtnIndex=0;
	//printf("MB400_C_BTN_MouseDown:%d %d ====\n",MB400_Choice_Index,nNewChoiceIndex);
    MB400_Button_Select(nTmpBtnIndex);//chpark.01.25

    return true;
}

bool MB400_C_BTN_MouseUp(ITUWidget* widget, char* param)
{
	uint8_t nNewChoiceIndex = (uint8_t)atoi(param);
	//printf("MB300_C_BTN_MouseUp:%d %d ====\n",MB400_Choice_Index,nNewChoiceIndex);

    int8_t nTmpBtnIndex = nNewChoiceIndex-1;
    
    if(nTmpBtnIndex<0)nTmpBtnIndex=0;

    //MB400_Button_Select(nTmpBtnIndex);//chpark.01.25
    MB400_Send_TX(nTmpBtnIndex);//chpark.06.08

    return true;
}

bool MB400_D7_MouseDn(ITUWidget* widget, char* param)
{
    //printf("MB400_D7_MouseDn===\n");
    MB400_Button_ClickUI(MB400_Choice_Index,true);

    return true;
}

bool MB400_D7_MouseUp(ITUWidget* widget, char* param)
{
    //printf("MB400_D7_MouseUp===\n Command Send To MC\n");
    MB400_Send_TX(MB400_Choice_Index);//chpark.06.08

    MB400_Button_ClickUI(MB400_Choice_Index,false);
  
    return true;
}

bool MB400_D6_MouseUp(ITUWidget* widget, char* param)
{
    //printf("MB400_D6_MouseUp===\n");
    GotoMain(MAIN_MODE_NORMAL); 

    return true;
}

bool MB400_D5_MouseUp(ITUWidget* widget, char* param)
{
    //printf("MB400_D5_MouseUp===\n");
    ITULayer *MenuBTop_Layer = ituSceneFindWidget(&theScene, "MenuBTop_Layer");
    assert(MenuBTop_Layer);
    ituLayerGoto(MenuBTop_Layer);

    return true;
}


