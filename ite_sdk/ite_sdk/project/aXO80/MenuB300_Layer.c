#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#define MB300_ICON_OFF		0
#define MB300_ICON_ON		1
#define MB300_ICON_ON_HOLD	2
#define MB300_ICON_NONE		3

#define MB300_ICON_SEL		2

#define MenuB300_UI_VMode			0
#define MenuB300_UI_HMode			1

bool MB300_Send_TX(uint8_t nIndex,bool bOn);//chpark.06.08

static uint32_t MB300_gMT_Timer;
static uint8_t  MB300_gMT_ExitTimer = 0;			// Sec Count

//static uint8_t MB300_MAX_BUTTON_NUM=13;
//static uint8_t MB300_MAX_LEFT_BUTTON_NUM=7;
//static uint8_t MB300_MAX_RIGHT_BUTTON_NUM=6;
static uint8_t MB300_MAX_BUTTON_NUM=10;
static uint8_t MB300_MAX_LEFT_BUTTON_NUM=5;
static uint8_t MB300_MAX_RIGHT_BUTTON_NUM=5;


typedef struct _B300ButtonState
{
	uint8_t nIndex;	
	uint8_t nState;			
	uint8_t nSend;	
	uint8_t nSelect;  //0:not selected 1:selected
}B300ButtonState;	


int8_t MB300_Choice_Index;

static ITUIcon*     MB300_C1_ICON[13][4];       //[13button][0:0ff 1:On 2:Sel 3:Del]
//static ITUButton*   MB300_C1_BTN[13];         //0,1[0]  2,3 [1]
static ITUIcon*     MB300_C1_LANG[5];       

static B300ButtonState     gOption_Button[13];  //13button

void MenuB300_GetMenuData(uint8_t nIndex);

void MenuB300_InitControl();
void MenuB300_SetLang(uint8_t nIndex);

bool   MB300_Button_Select(uint8_t nIndex);
int8_t MB300_Button_GetFirstIndex();
int8_t MB300_Button_GetNextIndex(uint8_t nIndex);
int8_t MB300_Button_GetPrevIndex(uint8_t nIndex);

int8_t MB300_Button_Left_GetPrevIndex(uint8_t nIndex);
int8_t MB300_Button_Right_GetPrevIndex(uint8_t nIndex);
int8_t MB300_Button_Right_GetNextIndex(uint8_t nIndex);
int8_t MB300_Button_Left_GetNextIndex(uint8_t nIndex);
int8_t MB300_Button_Left_GetNextIndex(uint8_t nIndex);

void MB300_MenuTabNext();
void MB300_MenuTabPrev();

//
void MB300_MenuTabPrev_InUIMode(uint8_t nVmode);
void MB300_MenuTabNext_InUIMode(uint8_t nVmode);
void MB300_MenuTabLeftRight_InUIMode(uint8_t nVmode);
int8_t MB300_Button_GetPrevIndex_InUIMode(uint8_t nVmode);
int8_t MB300_Button_GetNextIndex_InUIMode(uint8_t nVmode);
int8_t MB300_Button_GetLeftRightIndex_InUIMode(uint8_t nVmode);
//

bool MB300_Button_Toggle(uint8_t nIndex);
void MB300_Button_Display(uint8_t nIndex);

//
void MB300_MenuTabToggle();



bool MenuB300_Enter(ITUWidget* widget, char* param)
{
    //ithPrintf("MenuB300 Enter..===")
	MB300_gMT_ExitTimer = 0;
	MB300_gMT_Timer = gMainTimerCnt_10m;
    //printf("\r\n MenuB300_Enter--111---\r\n");

    MenuB300_InitControl();
    MenuB300_SetLang(gMB_Language);     
//printf("\r\n MenuB300_Enter--222---\r\n");
 
    for(int i=0;i<MB300_MAX_BUTTON_NUM;i++)
    {
        //printf("\r\n MenuB300_Enter--333--- %d\r\n",i);
        gOption_Button[i].nState = 0;
        gOption_Button[i].nSelect = 0;
        MenuB300_GetMenuData(i); 
        MB300_Button_Display(i); 
    }
    /*
     //============================
     //for test.
     gOption_Button[0].nState = 2;
     gOption_Button[1].nState = 3;
     gOption_Button[2].nState = 1;
     gOption_Button[3].nState = 1;

     gOption_Button[7].nState = 1;
     gOption_Button[10].nState = 1;
     //============================
    */
    for(int i=0;i<MB300_MAX_BUTTON_NUM;i++)
    {
        MB300_Button_Display(i); //Draw Button
    }
     
    MB300_Choice_Index = MB300_Button_GetFirstIndex();

//printf("\r\n MenuB300_Enter--555---%d\r\n",MB300_Choice_Index);
    
    if(MB300_Choice_Index>=0)
    {
        //printf("\r\n MenuB300_Enter--MB300_Button_Select---%d\r\n",MB300_Choice_Index);
        MB300_Button_Select(MB300_Choice_Index);
    }
    else
    {
        MB300_Choice_Index = 0;
        //printf("\r\n MenuB300_Enter--Error---%d\r\n",MB300_Choice_Index);
    }

    return true;
}
    
void MenuB300_GetMenuData(uint8_t nIndex)
{
    switch(nIndex)
    {
        //1st AUX
        case 0:
        {
            if( (gMB_IO_Value.OPI_AllByte_Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m )
            {
                gOption_Button[nIndex].nState = MB300_ICON_NONE;
            }
            else
            {
                gOption_Button[nIndex].nState = OPI3Byte23bit;
            }
        }
        break;

        //2nd AUX
        case 1:
        {
            if( (gMB_IO_Value.OPI_AllByte_Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m )
            {
                gOption_Button[nIndex].nState = MB300_ICON_NONE;
            }
            else
            {
                gOption_Button[nIndex].nState = OPI1Byte67bit;
            }
        }
        break;

        //4th AUX
        case 2:
        {
            if( (gMB_IO_Value.OPI_AllByte_Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m )
            {
                gOption_Button[nIndex].nState = MB300_ICON_NONE;
            }
            else
            {
                gOption_Button[nIndex].nState = OPI0Byte67bit;
            }
        }
        break;
        /*
        //High flow
        case 3:
        {
            if( (gMB_IO_Value.OPI_AllByte_Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m )
            {
                gOption_Button[nIndex].nState = MB300_ICON_NONE;
            }
            else
            {
                gOption_Button[nIndex].nState = OPI3Byte01bit;
            }
        }
        break;
        */

        //4th AUX independent
        case 3:
        {
            if( (gMB_IO_Value.OPI_AllByte_Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m )
            {
                gOption_Button[nIndex].nState = MB300_ICON_NONE;
            }
            else
            {
                gOption_Button[nIndex].nState = OPI7Byte67bit;
            }
        }
        break;
        /*
        //Lift alarm
        case 4:
        {
            if( (gMB_IO_Value.OPI_AllByte_Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m )
            {
                gOption_Button[nIndex].nState = MB300_ICON_NONE;
            }
            else
            {
                gOption_Button[nIndex].nState = OPI2Byte67bit;
            }
        }
        break;
        */
        
        //2piece boom
        case 4:
        {
            if( (gMB_IO_Value.OPI_AllByte_Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m )
            {
                gOption_Button[nIndex].nState = MB300_ICON_NONE;
            }
            else
            {
                gOption_Button[nIndex].nState = OPI0Byte45bit;
            }
        }
        break;
        
        //-------------
        /*
        //270 camera
        case 7:
        {
           if( (gMB_IO_Value.OPI2_AllByte_Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m )
            {
                gOption_Button[nIndex].nState = MB300_ICON_NONE;
            }
            else
            {
                gOption_Button[nIndex].nState = OPI2_0Byte_01bit;
            }
        }
        break;
        */

        //Auto tank
        case 5:
        {
            if( (gMB_IO_Value.OPI_AllByte_Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m )
            {
                gOption_Button[nIndex].nState = MB300_ICON_NONE;
            }
            else
            {
                gOption_Button[nIndex].nState = OPI0Byte23bit;
            }
        }
    
        break;

        //Auto grease
        case 6:
        {
            if( (gMB_IO_Value.OPI_AllByte_Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m )
            {
                gOption_Button[nIndex].nState = MB300_ICON_NONE;
            }
            else
            {
                gOption_Button[nIndex].nState = OPI7Byte23bit;
            }
        }
        break;

        //LED package2
        case 7:
        {
            if( (gMB_IO_Value.OPI2_AllByte_Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m )
            {
                gOption_Button[nIndex].nState = MB300_ICON_NONE;
            }
            else
            {
                gOption_Button[nIndex].nState = OPI2_0Byte_45bit;
            }
        }
        break;

        //AUX1 Pressure fix
        case 8:
        {
            if( (gMB_IO_Value.OPI_AllByte_Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m )
            {
                gOption_Button[nIndex].nState = MB300_ICON_NONE;
            }
            else
            {
                gOption_Button[nIndex].nState = OPI6Byte01bit;
            }
        }
        
        break;

        //Aux2 Pressure fix
        case 9:
        {
            if( (gMB_IO_Value.OPI_AllByte_Time+TIME_CNT_5SEC_CHPark)<gMainTimerCnt_10m )
            {
                gOption_Button[nIndex].nState = MB300_ICON_NONE;
            }
            else
            {
                gOption_Button[nIndex].nState = OPI6Byte23bit;
            }

        }
        break;
    }
}

void MenuB300_InitControl()
{
    MB300_C1_LANG[0] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C1_LANG1");
    MB300_C1_LANG[1] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C1_LANG2");
    MB300_C1_LANG[2] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C1_LANG3");
    MB300_C1_LANG[3] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C1_LANG4");
    MB300_C1_LANG[4] =  (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C1_LANG5");

  //printf("\r\n MenuB300_InitControl--111---\r\n");
    MB300_C1_ICON[0][0] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C1_OFF");
    MB300_C1_ICON[0][1] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C1_ON");
    MB300_C1_ICON[0][2] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C1_SEL");
    MB300_C1_ICON[0][3] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C1_DEL");

    MB300_C1_ICON[1][0] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C2_OFF");
    MB300_C1_ICON[1][1] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C2_ON");
    MB300_C1_ICON[1][2] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C2_SEL");
    MB300_C1_ICON[1][3] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C2_DEL");

 //printf("\r\n MenuB300_InitControl--111---\r\n");
    MB300_C1_ICON[2][0] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C3_OFF");
    MB300_C1_ICON[2][1] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C3_ON");
    MB300_C1_ICON[2][2] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C3_SEL");
    MB300_C1_ICON[2][3] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C3_DEL");

    MB300_C1_ICON[3][0] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C4_OFF");
    MB300_C1_ICON[3][1] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C4_ON");
    MB300_C1_ICON[3][2] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C4_SEL");
    MB300_C1_ICON[3][3] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C4_DEL");


    MB300_C1_ICON[4][0] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C5_OFF");
    MB300_C1_ICON[4][1] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C5_ON");
    MB300_C1_ICON[4][2] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C5_SEL");
    MB300_C1_ICON[4][3] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C5_DEL");

//printf("\r\n MenuB300_InitControl--111---\r\n");
    MB300_C1_ICON[5][0] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C6_OFF");
    MB300_C1_ICON[5][1] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C6_ON");
    MB300_C1_ICON[5][2] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C6_SEL");
    MB300_C1_ICON[5][3] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C6_DEL");

    MB300_C1_ICON[6][0] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C7_OFF");
    MB300_C1_ICON[6][1] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C7_ON");
    MB300_C1_ICON[6][2] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C7_SEL");
    MB300_C1_ICON[6][3] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C7_DEL");

    MB300_C1_ICON[7][0] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C8_OFF");
    MB300_C1_ICON[7][1] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C8_ON");
    MB300_C1_ICON[7][2] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C8_SEL");
    MB300_C1_ICON[7][3] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C8_DEL");

    MB300_C1_ICON[8][0] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C9_OFF");
    MB300_C1_ICON[8][1] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C9_ON");
    MB300_C1_ICON[8][2] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C9_SEL");
    MB300_C1_ICON[8][3] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C9_DEL");

    MB300_C1_ICON[9][0] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C10_OFF");
    MB300_C1_ICON[9][1] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C10_ON");
    MB300_C1_ICON[9][2] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C10_SEL");
    MB300_C1_ICON[9][3] = (ITUIcon* )ituSceneFindWidget(&theScene, "MenuB300_C10_DEL");
//printf("\r\n MenuB300_InitControl--111---\r\n");
}

void MenuB300_SetLang(uint8_t nIndex)
{
    for(int i=0;i<5;i++)
    {
        ituWidgetSetVisible(MB300_C1_LANG[i],false);
    }

    ituWidgetSetVisible(MB300_C1_LANG[nIndex],true);
}



bool MB300_Button_Select(uint8_t nIndex)
{
    bool bRet = false;
    if(gOption_Button[nIndex].nState==MB300_ICON_OFF || gOption_Button[nIndex].nState==MB300_ICON_ON)
    {
        for(int i=0; i<MB300_MAX_BUTTON_NUM ;i++)
        {
             //printf("\r\n MB300_Button_Select--555---\r\n");
             ituWidgetSetVisible(MB300_C1_ICON[i][MB300_ICON_SEL],false);
        }
        //===============================
        gOption_Button[nIndex].nSelect = true;
        MB300_Choice_Index = nIndex;//
        //===============================
        //printf("\r\n MB300_Button_Select--666---\r\n");
        ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_SEL],true);
        //printf("MB300_Button_Select %d===\r\n\n\n",nIndex);
        bRet = true;
    }
    else
    {
        
    }

    return bRet;
}


int8_t MB300_Button_GetFirstIndex()
{
    int8_t nRet=-1;
      
    for(int i=0;i<MB300_MAX_BUTTON_NUM;i++)
    {
        if(gOption_Button[i].nState==MB300_ICON_OFF || gOption_Button[i].nState==MB300_ICON_ON)
        {
            nRet = i;
            break;
        }
    }

    //printf("MB300_Button_GetFirstIndex %d===\n",nRet);
    return nRet;
}


int8_t MB300_Button_GetNextIndex(uint8_t nIndex)
{
    int8_t nRet=-1;
    
    for(int i=nIndex+1; i<MB300_MAX_BUTTON_NUM ; i++)
    {
        if(gOption_Button[i].nState==MB300_ICON_OFF || gOption_Button[i].nState==MB300_ICON_ON)
        {
            nRet = i;
            break;
        }
    }
    
    if(nRet<0)
    {
        for(int i=0;i<=nIndex;i++)
        {
            if(gOption_Button[i].nState==MB300_ICON_OFF || gOption_Button[i].nState==MB300_ICON_ON)
            {
                nRet = i;
                break;
            }
        }
    }

    //printf("MB300_Button_GetNextIndex %d===\n",nRet);
    return nRet;
}


int8_t MB300_Button_GetPrevIndex(uint8_t nIndex)
{
    int8_t nRet=-1;
    
    for(int i=nIndex-1; i>=0 ; i--)
    {
        if(gOption_Button[i].nState==MB300_ICON_OFF || gOption_Button[i].nState==MB300_ICON_ON)
        {
            nRet = i;
            break;
        }
    }
    
    if(nRet<0)
    {
        for(int i=MB300_MAX_BUTTON_NUM-1;i>=nIndex;i--)
        {
            if(gOption_Button[i].nState==MB300_ICON_OFF || gOption_Button[i].nState==MB300_ICON_ON)
            {
                nRet = i;
                break;
            }
        }
    }

    //printf("MB300_Button_GetPrevIndex %d===\n",nRet);
    return nRet;
}

int8_t MB300_Button_GetNextIndex_InUIMode(uint8_t nVmode)
{
    int8_t nRet=-1;
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = MB300_Choice_Index;

    if(nVmode==MenuB300_UI_VMode)
    {
        //MB300_MAX_LEFT_BUTTON_NUM   =7;
        //MB300_MAX_RIGHT_BUTTON_NUM  =6;
        MB300_MAX_LEFT_BUTTON_NUM   =5;
        MB300_MAX_RIGHT_BUTTON_NUM  =5;
    }
    else if(nVmode==MenuB300_UI_HMode)
    {
        //MB300_MAX_LEFT_BUTTON_NUM   =6;
        //MB300_MAX_RIGHT_BUTTON_NUM  =7;
        MB300_MAX_LEFT_BUTTON_NUM   =6;
        MB300_MAX_RIGHT_BUTTON_NUM  =4;
    }

    if(nCurBtnIndex<=MB300_MAX_LEFT_BUTTON_NUM-1)
    {
        nRet = MB300_Button_Left_GetNextIndex(nCurBtnIndex);
    }
    else
    {
        nRet = MB300_Button_Right_GetNextIndex(nCurBtnIndex);
    }
              
    //printf("MB300_Button_GetNextIndex_InDetail %d===\n",nRet);

    return nRet;
}


int8_t MB300_Button_GetPrevIndex_InUIMode(uint8_t nVmode)
{
    int8_t nRet=-1;
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = MB300_Choice_Index;

    
    if(nVmode==MenuB300_UI_VMode)
    {
        //MB300_MAX_LEFT_BUTTON_NUM   =7;
        //MB300_MAX_RIGHT_BUTTON_NUM  =6;
        MB300_MAX_LEFT_BUTTON_NUM   =5;
        MB300_MAX_RIGHT_BUTTON_NUM  =5;
    }
    else if(nVmode==MenuB300_UI_HMode)
    {
        //MB300_MAX_LEFT_BUTTON_NUM   =6;
        //MB300_MAX_RIGHT_BUTTON_NUM  =7;
        MB300_MAX_LEFT_BUTTON_NUM   =6;
        MB300_MAX_RIGHT_BUTTON_NUM  =4;
    }

    if(nCurBtnIndex<=MB300_MAX_LEFT_BUTTON_NUM-1)
    {
        nRet = MB300_Button_Left_GetPrevIndex(nCurBtnIndex);
    }
    else
    {
        nRet = MB300_Button_Right_GetPrevIndex(nCurBtnIndex);
    }
              
    printf("MB300_Button_GetPrevIndex_InDetail %d===\n",nRet);

    return nRet;
}


int8_t MB300_Button_GetLeftRightIndex_InUIMode(uint8_t nVmode)
{
    int8_t nRet=-1;
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = MB300_Choice_Index;
   
    if(nVmode==MenuB300_UI_VMode)
    {
        if(nCurBtnIndex>=0 && nCurBtnIndex<5)
        {
            nTmpBtnIndex = nCurBtnIndex + 5;
        }
        else if(nCurBtnIndex>=5 && nCurBtnIndex<MB300_MAX_BUTTON_NUM)
        {
            nTmpBtnIndex = nCurBtnIndex - 5;
        }
    }
    else if(nVmode==MenuB300_UI_HMode)
    {
        if((nCurBtnIndex >= 0) && (nCurBtnIndex <= 3))
        {
            nTmpBtnIndex = nCurBtnIndex+6;
        }
        else if(nCurBtnIndex == 4 || nCurBtnIndex == 5)
        {
            nTmpBtnIndex = 9;
        }
        else if((nCurBtnIndex >= 7) && (nCurBtnIndex <= 12))
        {
            nTmpBtnIndex = nCurBtnIndex-6;
        }
    }

    if(gOption_Button[nTmpBtnIndex].nState==MB300_ICON_OFF || gOption_Button[nTmpBtnIndex].nState==MB300_ICON_ON)
    {
        nRet = nTmpBtnIndex;
    }
   
    //printf("MB300_Button_GetLeftRightIndex_InUIMode %d===\n",nRet);
    return nRet;
}


int8_t MB300_Button_Left_GetNextIndex(uint8_t nIndex)
{
    int8_t nRet=-1;
  
    for(int i=nIndex+1; i<MB300_MAX_LEFT_BUTTON_NUM ; i++)
    {
        if(gOption_Button[i].nState==MB300_ICON_OFF || gOption_Button[i].nState==MB300_ICON_ON)
        {
            nRet = i;
            break;
        }
    }
  
    if(nRet<0)
    {
        for(int i=0;i<=nIndex;i++)
        {
            if(gOption_Button[i].nState==MB300_ICON_OFF || gOption_Button[i].nState==MB300_ICON_ON)
            {
                nRet = i;
                break;
            }

        }
    }
    //printf("MB300_Button_GetNextIndex %d===\n",nRet);

    return nRet;
}


int8_t MB300_Button_Left_GetPrevIndex(uint8_t nIndex)
{
    int8_t nRet=-1;
   
    for(int i=nIndex-1; i>=0 ; i--)
    {
        if(gOption_Button[i].nState==MB300_ICON_OFF || gOption_Button[i].nState==MB300_ICON_ON)
        {
            nRet = i;
            break;
        }
    }
 
    if(nRet<0)
    {
        for(int i=MB300_MAX_LEFT_BUTTON_NUM-1;i>=nIndex;i--)
        {
            if(gOption_Button[i].nState==MB300_ICON_OFF || gOption_Button[i].nState==MB300_ICON_ON)
            {
                nRet = i;
                break;
            }
        }
    }

    //printf("MB300_Button_Left_GetPrevIndex %d===\n",nRet);
    return nRet;
}


int8_t MB300_Button_Right_GetNextIndex(uint8_t nIndex)
{
    int8_t nRet=-1;
  
    for(int i=nIndex+1; i<MB300_MAX_BUTTON_NUM ; i++)
    {
        if(gOption_Button[i].nState==MB300_ICON_OFF || gOption_Button[i].nState==MB300_ICON_ON)
        {
            nRet = i;
            break;
        }
    }
  
    if(nRet<0)
    {
        for(int i=MB300_MAX_LEFT_BUTTON_NUM;i<=nIndex;i++)
        {
            if(gOption_Button[i].nState==MB300_ICON_OFF || gOption_Button[i].nState==MB300_ICON_ON)
            {
                nRet = i;
                break;
            }

        }
    }

    //printf("MB300_Button_Right_GetNextIndex %d===\n",nRet);
    return nRet;
}


int8_t MB300_Button_Right_GetPrevIndex(uint8_t nIndex)
{
    int8_t nRet=-1;
   
    for(int i=nIndex-1; i>=MB300_MAX_LEFT_BUTTON_NUM ; i--)
    {
        if(gOption_Button[i].nState==MB300_ICON_OFF || gOption_Button[i].nState==MB300_ICON_ON)
        {
            nRet = i;
            break;
        }
    }
 
    if(nRet<0)
    {
        for(int i=MB300_MAX_BUTTON_NUM-1;i>=nIndex;i--)
        {
            if(gOption_Button[i].nState==MB300_ICON_OFF || gOption_Button[i].nState==MB300_ICON_ON)
            {
                nRet = i;
                break;
            }
        }
    }

    //printf("MB300_Button_Right_GetPrevIndex %d===\n",nRet);
    return nRet;
}


bool MB300_Button_Toggle(uint8_t nIndex)
{
    bool bRet = false;

    if(gOption_Button[nIndex].nState==MB300_ICON_OFF)
    {
        //gOption_Button[nIndex].nState=MB300_ICON_ON;//chpark.07.31
        MB300_Send_TX(nIndex,MB300_ICON_ON);//chpark.06.08
        bRet = true;
    }
    else if(gOption_Button[nIndex].nState==MB300_ICON_ON)
    {
        //gOption_Button[nIndex].nState=MB300_ICON_OFF;//chpark.07.31
        MB300_Send_TX(nIndex,MB300_ICON_OFF);//chpark.06.08
        bRet = true;
    }

    //printf("MB300_Button_Toggle:%d Change=%d ====\n",nIndex, bRet );
    return bRet;
}

bool MB300_Send_TX(uint8_t nIndex,bool bOn)
{
    uint8_t  txbuffer[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    if(bOn)
    {
        txbuffer[4] = 0x01;
    }
    else
    {
        txbuffer[4] = 0x00;
    }
    //5~6byte:1 //fix
    txbuffer[5] = 0x01;
    txbuffer[6] = 0x00;
    
    //01 00 00 00 01 01 00 00 00-->1st aux on
    //01 00 00 00 00 01 00 00 00-->1st aux off
    switch(nIndex)
    {
        case 0://1st AUX(1)
			txbuffer[0] = 0x01;//0~3byte : Index        
            break;
        case 1://2nd AUX(2)
			txbuffer[0] = 0x02;//0~3byte : Index        //02 00 00 00    
            break;
        case 2://4th AUX(4)
			txbuffer[0] = 0x04;//0~3byte : Index            
            break;
        //case 3://High flow(5)
		//	txbuffer[0] = 0x05;//0~3byte : Index            
        //    break;    
        case 3://4th AUX independent(25)
			txbuffer[0] = 0x19;//0~3byte : Index            
            break;
        //case 5://1st Lift Alarm(7)
		//	txbuffer[0] = 0x07;//0~3byte : Index            
        //    break;
        case 4://2piece boom(9)
			txbuffer[0] = 0x09;//0~3byte : Index            
            break;
        //case 7://270 camera(26)
		//	txbuffer[0] = 0x1A;//0~3byte : Index            
        //    break;
        case 5://Auto tank(28)
			txbuffer[0] = 0x1C;//0~3byte : Index            
            break;
        case 6://Auto grease(23)
			txbuffer[0] = 0x17;//0~3byte : Index            
            break;
        case 7://LED package2(27)
			txbuffer[0] = 0x1B;//0~3byte : Index            
            break;
        case 8://AUX1 Pressure fix(19)
			txbuffer[0] = 0x13;//0~3byte : Index            
            break;
        case 9://AUX2 Pressure fix(20)
			txbuffer[0] = 0x14;//0~3byte : Index            
            break;    
        default:
            printf("MB300_Send_TX:[Index:%d]  ERROR  ====\n",nIndex  );
            break;

    }

    CanbusTxDataManagement( gMainTimerCnt_10m,OPS1,txbuffer);//chpark.06.21

    return true;
}
void MB300_Button_Display(uint8_t nIndex)
{
    //printf("MB300_Button_Display:%d  ====\n",nIndex  );
    if(gOption_Button[nIndex].nState==MB300_ICON_OFF)
    {
        //printf("MB300_Button_Display:%d  MB300_ICON_OFF====\n",nIndex  );
        ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_OFF],true);
        ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_ON],false);

        if(gOption_Button[nIndex].nSelect==1)
        {
            ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_SEL],true);
        }
        else
        {
            ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_SEL],false);
        }

        ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_NONE],false);
    }
    else if(gOption_Button[nIndex].nState==MB300_ICON_ON )
    {
        //printf("MB300_Button_Display:%d  MB300_ICON_ON====\n",nIndex  );
        ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_OFF],false);
        ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_ON],true);

        if(gOption_Button[nIndex].nSelect==1)
        {
            ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_SEL],true);
        }
        else
        {
            ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_SEL],false);
        }

        ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_NONE],false);
    }
    else if(gOption_Button[nIndex].nState==MB300_ICON_ON_HOLD)
    {
        //printf("MB300_Button_Display:%d  MB300_ICON_ON_HOLD====\n",nIndex  );
        ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_OFF],false);
        ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_ON],true);
        ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_SEL],false);

        ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_NONE],false);
    }
    else if(gOption_Button[nIndex].nState==MB300_ICON_NONE )
    {
        // printf("MB300_Button_Display:%d  MB300_ICON_NONE====\n",nIndex  );
        ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_OFF],false);
        ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_ON],false);
        ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_SEL],false);

        ituWidgetSetVisible(MB300_C1_ICON[nIndex][MB300_ICON_NONE],true);
    }
}


bool MB300_C_BTN_MouseDown(ITUWidget* widget, char* param)
{
    uint8_t nNewChoiceIndex = (uint8_t)atoi(param);
    MB300_gMT_ExitTimer = 0;
	//printf("MB300_C_BTN_MouseDown:%d %d ====\n",MB300_Choice_Index,nNewChoiceIndex);
    int8_t nTmpBtnIndex = nNewChoiceIndex-1;
    if(nTmpBtnIndex<0)nTmpBtnIndex=0;

    MB300_Button_Select(nTmpBtnIndex);//chpark.07.31

    return true;
}

bool MB300_C_BTN_MouseUp(ITUWidget* widget, char* param)
{
    MB300_gMT_ExitTimer = 0;
    uint8_t nNewChoiceIndex = (uint8_t)atoi(param);

	//printf("MB300_C_BTN_MouseUp:%d %d ====\n",MB300_Choice_Index,nNewChoiceIndex);

    int8_t nTmpBtnIndex = nNewChoiceIndex-1;
    if(nTmpBtnIndex<0)nTmpBtnIndex=0;

    //=========================================================
    int8_t nCurBtnIndex = MB300_Choice_Index;

    if(nCurBtnIndex>=0 && nCurBtnIndex<MB300_MAX_BUTTON_NUM)
    {
        MB300_Button_Toggle(nCurBtnIndex);
        MB300_Button_Display(nCurBtnIndex);
    }
    //=======================================================

    return true;
}


bool MB300_D7_BTNMouseUp(ITUWidget* widget, char* param)
{
    uint8_t nNewChoiceIndex = (uint8_t)atoi(param);
    
    //==================================================
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = MB300_Choice_Index;

    if(nCurBtnIndex>=0 && nCurBtnIndex<MB300_MAX_BUTTON_NUM)
    {
        MB300_Button_Toggle(nCurBtnIndex);
        MB300_Button_Display(nCurBtnIndex);
    }

    return true;
}

bool MB300_D6_BTNMouseUp(ITUWidget* widget, char* param)
{
    uint8_t nNewChoiceIndex = (uint8_t)atoi(param);
    //printf("MB300_D6_BTNMouseUp:%d  -> %d ====\n",MB300_Choice_Index,nNewChoiceIndex);

    GotoMain(MAIN_MODE_NORMAL); 

    return true;
}

bool MB300_D5_BTNMouseUp(ITUWidget* widget, char* param)
{
    //printf("MB300_D5_BTNMouseUp..BackMenu===");
    ITULayer *MenuBTop_Layer = ituSceneFindWidget(&theScene, "MenuBTop_Layer");
    assert(MenuBTop_Layer);
    ituLayerGoto(MenuBTop_Layer);
    return true;
}


bool MenuB300_Timer(ITUWidget* widget, char* param)
{
    #ifdef USED_ROTATION //chpark.10.19
    if (RotationMode!=0)
    {
      return 0;
    }
    #endif

	if((gMainTimerCnt_10m - MB300_gMT_Timer) >= 30)		// 0.3Sec
	{
		MB300_gMT_Timer = gMainTimerCnt_10m;

		MB300_gMT_ExitTimer++;
		
		if(MB300_gMT_ExitTimer >= 100)
		{
			//printf("\r\n Menu Exit > goto Main\r\n");
            #ifdef RELEASE_VER
	            GotoMain(MAIN_MODE_NORMAL); 
            #else
	            printf("\r\n [Debug Mode]Menu Exit > goto Main\r\n");
            #endif
		}

        for(int i=0;i<MB300_MAX_BUTTON_NUM;i++)
        {
            //printf("\r\n MenuB300_Enter--333--- %d\r\n",i);
            gOption_Button[i].nState = 0;

            gOption_Button[i].nSelect = 0;
            MenuB300_GetMenuData(i); 
            MB300_Button_Display(i); 
        }
        MB300_Button_Select(MB300_Choice_Index);
	}

    return true;
}


void MB300_MenuTabToggle()
{
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = MB300_Choice_Index;

    if(nCurBtnIndex>=0 && nCurBtnIndex<MB300_MAX_BUTTON_NUM)
    {
        MB300_Button_Toggle(nCurBtnIndex);
        MB300_Button_Display(nCurBtnIndex);
    }
}


void MB300_MenuTabNext()
{
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = MB300_Choice_Index;
    nTmpBtnIndex = MB300_Button_GetNextIndex(MB300_Choice_Index);
    if(nTmpBtnIndex>=0 && nTmpBtnIndex<MB300_MAX_BUTTON_NUM)
    {
        MB300_Button_Select(nTmpBtnIndex);
    }
    else
    {
        
    }
}


void MB300_MenuTabPrev()
{
    int8_t nTmpBtnIndex=0;
    int8_t nCurBtnIndex = MB300_Choice_Index;
    nTmpBtnIndex = MB300_Button_GetPrevIndex(MB300_Choice_Index);
    if(nTmpBtnIndex>=0 && nTmpBtnIndex<MB300_MAX_BUTTON_NUM)
    {
        MB300_Button_Select(nTmpBtnIndex);
    }
    else
    {
        
    }
}


void MB300_MenuTabNext_InUIMode(uint8_t nVmode)
{
    int8_t nTmpBtnIndex=0;
               
    nTmpBtnIndex = MB300_Button_GetNextIndex_InUIMode(nVmode);
    if(nTmpBtnIndex>=0 && nTmpBtnIndex<MB300_MAX_BUTTON_NUM)
    {
        MB300_Button_Select(nTmpBtnIndex);
    }
    else
    {
        
    }
}

void MB300_MenuTabPrev_InUIMode(uint8_t nVmode)
{
    int8_t nTmpBtnIndex=0;
               
    nTmpBtnIndex = MB300_Button_GetPrevIndex_InUIMode(nVmode);
    if(nTmpBtnIndex>=0 && nTmpBtnIndex<MB300_MAX_BUTTON_NUM)
    {
        MB300_Button_Select(nTmpBtnIndex);
    }
    else
    {
       
    }
}

void MB300_MenuTabLeftRight_InUIMode(uint8_t nVmode)
{
    int8_t nTmpBtnIndex=0;
                
    nTmpBtnIndex =  MB300_Button_GetLeftRightIndex_InUIMode(nVmode);
    if(nTmpBtnIndex>=0 && nTmpBtnIndex<MB300_MAX_BUTTON_NUM)
    {
        MB300_Button_Select(nTmpBtnIndex);
    }
    else
    {
        
    }
}

bool MenuB300_JogWheel(ITUWidget* widget, char* param)
{
	uint8_t JogDir = 0;
    JogDir = param[0];
    MB300_gMT_ExitTimer = 0;

	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
		case LANDSCAPE_1:
		case LANDSCAPE_2:
        {
            if(JogDir == JOG_DIAL_ROT_RIGHT)
			{
                //printf("MenuB300 JogWheel[R] %d\r\n",JogDir);
                MB300_MenuTabNext();
			}
			else if(JogDir == JOG_DIAL_ROT_LEFT)
			{
                //printf("MenuB300 JogWheel[L] %d\r\n",JogDir);
                MB300_MenuTabPrev();
			}
        }
        break;
	}

    return true;
}


bool MenuB300_JogButton(ITUWidget* widget, char* param)
{
	uint8_t JogButton = 0;
    MB300_gMT_ExitTimer = 0;

	JogButton = param[0];

	switch(RotationDir())
	{
		case PORTRAIT_1:
		case PORTRAIT_2:
			
			switch(JogButton)
			{
				case JOG_DIAL_KEY_UP:

                     MB300_MenuTabPrev_InUIMode(MenuB300_UI_VMode);
					
				break;

				case JOG_DIAL_KEY_DOWN:

                     MB300_MenuTabNext_InUIMode(MenuB300_UI_VMode);

				break;
				
				case JOG_DIAL_KEY_LEFT:
				case JOG_DIAL_KEY_RIGHT:
                   
					MB300_MenuTabLeftRight_InUIMode(MenuB300_UI_VMode);

				break;

				case JOG_DIAL_KEY_ENTER_ON:
                    //MB300_MenuTabToggle();
                break;
                case JOG_DIAL_KEY_ENTER_OFF:
                    MB300_MenuTabToggle();//chpark.07.31
                break;
				    

				break;
			}
		break;
			
		case LANDSCAPE_1:
		case LANDSCAPE_2:
            switch(JogButton)
			{
				case JOG_DIAL_KEY_UP:
                    
                     MB300_MenuTabPrev_InUIMode(MenuB300_UI_HMode);
					
				break;

				case JOG_DIAL_KEY_DOWN:

                     MB300_MenuTabNext_InUIMode(MenuB300_UI_HMode);
					
				break;
				
				case JOG_DIAL_KEY_LEFT:
				case JOG_DIAL_KEY_RIGHT:
                   
					MB300_MenuTabLeftRight_InUIMode(MenuB300_UI_HMode);

				break;

				case JOG_DIAL_KEY_ENTER_ON:
				    //MB300_MenuTabToggle();
                break;

                case JOG_DIAL_KEY_ENTER_OFF:
                    MB300_MenuTabToggle();//chpark.07.31

				break;
			}


		break;
	}

    return true;
}

bool MenuB300_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t JogSwitch = 0;
	JogSwitch = param[0];
	MB300_gMT_ExitTimer = 0;

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
