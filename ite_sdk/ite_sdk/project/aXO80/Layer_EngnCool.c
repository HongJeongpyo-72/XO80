#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"
#include "HT_DispDdefine.h"
#include "ite/itp.h"

#include "Menu_Layer.h"

/* widgets:
EngnCoolLayer
Engn_Value
Engn_Off
Engn_On
EngnCoolBG
*/


static bool LayerFlag = 0;
static char widgetName[30];
static ITUIcon* Engn_Value;
static ITUIcon* Engn_Off[5];
static ITUIcon* Engn_On[5];
static ITUIcon* Engn_Title[5];
static ITUText* EngnValueTx;
static ITUBackground* EngnCoolBG;

static ITULayer* EngnCoolLayer=NULL;

static uint16_t TempEngMode = 0xFFFFFF;

void GotoEngnCool(void)
{
    EngnCoolLayer = ituSceneFindWidget(&theScene, "EngnCoolLayer");
    assert(EngnCoolLayer);
    ituLayerGoto(EngnCoolLayer);
}
void ResetEngnCool(void)
{
	LayerFlag = 0;
	TempEngMode = 0xFFFFFF;
	g_TCI_Time=gMainTimerCnt_10m;
}


bool MainLayer_EngnCoolValueDisp(DISP_MODE DispMode,uint32_t tValue,bool ReDisp)
{
	static uint32_t TempValue = 0xFFFFFF;
	uint32_t RealValue = 0;
	static DISP_MODE tDispMode = 0xFF;
	char tmp[32] = {0,};
	
	if (ReDisp)
	{
		TempValue = 0xFFFFFF;
	 	tDispMode = 0xFF;
	}
	
	if ((TempValue!=tValue)||(tDispMode!=DispMode))
	{
		if ((0<tValue)&&(tValue<=100))
		{
			RealValue = tValue;
			if (RealValue==0xFFFF)
			{
				RealValue = 0;
			}
			RealValue = (646*RealValue)/100;
			ituWidgetSetWidth(Engn_Value, RealValue);
			
			sprintf(tmp, "%d%c", tValue,'%');
			ituTextSetString(EngnValueTx, tmp);
		}
		else
		{
			ituWidgetSetWidth(Engn_Value, 0);
			if (tValue==0)
			{
				sprintf(tmp, "0%c",'%');
			}
			else
			{
				sprintf(tmp, "N/A");
			}
			ituTextSetString(EngnValueTx, tmp);
		}

		ituWidgetSetVisible(Engn_Value, DispMode);
		ituWidgetSetVisible(EngnValueTx, DispMode);
		
		TempValue=tValue;
		tDispMode=DispMode;
	}
}

bool MainLayer_EngnCoolDisp(DISP_MODE DispMode,uint8_t EngMode)
{
	int i;
	if ((g_TCI_Time+TIME_CNT_5SEC)<gMainTimerCnt_10m)	// 
	{
		printf("\nEngnCoolOnEnter=GotoMain\n\n");
		GotoMain(MAIN_MODE_NORMAL);
	}
	else if (TempEngMode!=EngMode)
	{	
		printf("\nMainLayer_EngnCoolDisp[%d][%d][%d]\n\n",EngMode,M7_UserIndex,Lang_Flag[M7_UserIndex]);

		if (EngMode == 0)	// Goto Main
		{
			GotoMain(MAIN_MODE_NORMAL);
		}
		else
		{	
			for (i=0;i<5;i++)
			{
				ituWidgetSetVisible(Engn_Off[i], DISP_OFF);
				ituWidgetSetVisible(Engn_On[i], DISP_OFF);
				ituWidgetSetVisible(Engn_Title[i], DISP_OFF);
			}
	
			if ((EngMode&0x01) == 1)	// 1 Eng On
			{
				ituWidgetSetVisible(Engn_On[Lang_Flag[M7_UserIndex]], DISP_ON);
				ituWidgetSetVisible(Engn_Title[Lang_Flag[M7_UserIndex]], DISP_ON);
			}
			else if ((EngMode&0x02) == 2)	// 2 Eng Off
			{
				ituWidgetSetVisible(Engn_Off[Lang_Flag[M7_UserIndex]], DISP_ON);
				ituWidgetSetVisible(Engn_Title[Lang_Flag[M7_UserIndex]], DISP_ON);
			}
			
			MainLayer_EngnCoolValueDisp(DispMode,g_TCI_1B,1);
		}
		TempEngMode=EngMode;
	}
	else
		MainLayer_EngnCoolValueDisp(DispMode,g_TCI_1B,0);
}
bool EngnCoolOnEnter(ITUWidget* widget, char* param)
{
	int i;
	printf("\nEngnCoolOnEnter=[%s],[%d],[%d],[%d]\n\n",param,LayerFlag,Main_Disp_Mode,Lang_Flag[M7_UserIndex]);
	Main_Disp_Mode=MAIN_MODE_ENGN_COOL;
	TempEngMode = 0xFF;
	if ((LayerFlag==0)||(Main_Disp_Mode==MAIN_MODE_PW))
	{
		SetWidget(Engn_Value,"Engn_Value");
		SetWidget(EngnCoolBG,"EngnCoolBG");
		SetWidget(EngnValueTx,"EngnValueTx");
		for (i=0;i<5;i++)
		{
			sprintf(widgetName, "Engn_Off_%d",i);
			SetWidget(Engn_Off[i],widgetName);
			sprintf(widgetName, "Engn_On_%d",i);
			SetWidget(Engn_On[i],widgetName);
			sprintf(widgetName, "Engn_Title_%d",i);
			SetWidget(Engn_Title[i],widgetName);
		}
		LayerFlag = 1;
	}
	
	MainLayer_EngnCoolDisp(DISP_ON,g_TCI_0B);
	
	ReadyRotation();	// JPHong 2023-10-24
	ScreenSetOnOff(1);	// JPHong 2023-12-04	
    return true;
}

bool EngnCoolOnTimer(ITUWidget* widget, char* param)
{
	static uint32_t OnTimerCnt_33ms = 0;
	static uint32_t OnTimerCnt_100ms = 0;
	static uint32_t OnTimerCnt_300ms = 0;
	static uint32_t OnTimerCnt_500ms = 0;
	static uint32_t OnTimerCnt_600ms = 0;
	static uint32_t OnTimerCnt_1S = 0;
	static uint32_t OnTimerCnt_5S = 0,iCON_CNT=0,i;
	static uint8_t tUserPW_PassFlag = 0xFF;
	static uint32_t tUserPW_Time = 0;


    #ifdef USED_ROTATION
    if (RotationMode!=0)
    {
      return 0;
    }
    #endif
    
	OnTimerCnt_33ms++;
	
	
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_100MS)==0)	// 100ms
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_100ms++;	
		
		if (LayerFlag == 1)
		{
			MainLayer_EngnCoolDisp(DISP_ON,g_TCI_0B);
		}
	}
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_300MS)==0)	// 300ms
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_300ms++;
	}
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_500MS)==0)	// 500ms
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_500ms++;
	}
	
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_600MS)==0)	// 600ms
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_600ms++;
	}
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_1000MS)==0)
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_1S++;
	}
	if ((OnTimerCnt_33ms%TIME_CNT_33ms_5000MS)==0)
	{
		static int Cs_Cnt = 0;
		OnTimerCnt_5S++;
	}
    return true;
}

bool EngnCoolOnLeave(ITUWidget* widget, char* param)
{
//	if (EngnCoolLayer!=NULL)
//		ituWidgetSetVisible(EngnCoolLayer, DISP_OFF);
	printf("EngnCoolOnLeave\n");
	g_TCI_0B = 0;
	g_TCI_1B = 0xFFFF;
	g_TCI_Time = 0;
	LayerFlag = 0;
    return true;
}

