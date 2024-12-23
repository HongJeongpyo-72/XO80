#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "HT_DispDdefine.h"
#include "layer_main_FW.h"
#include "Menu_Layer.h"


static char widgetName[30];

static ITUIcon* AC_BG;
static ITUIcon* AC_Black_BG;
static ITUIcon* AC_Blue_BG;
	static ITUIcon* mAC_WindDirD[6];
	static ITUIcon* mAC_Wind_Off;

	static ITUIcon* mAC_Auto_Off;
	static ITUIcon* mAC_Auto_On;
	static ITUIcon* mAC_Off;
	static ITUIcon* mAC_On;
	static ITUIcon* mAC_InAir;
	static ITUIcon* mAC_OutAir;
	static ITUIcon* mAC_WindLvD[7];
	static ITUText* mAC_TempTx;
	static ITUIcon* mAC_MODE_SEL[10];
	static ITUButton* mAC_Bt[10];


//#define mRD_BT_CNT_ALL	33
#define mRD_BT_Radio	19
#define mRD_BT_DAB1		20
#define mRD_BT_DAB2		19

static ITUIcon* RD_BG;
static ITUIcon* DAB2_BG;
static ITUIcon* BT_BG;
static ITUIcon* PH_BG;
//	static ITUIcon* mRadio_Mute;
//	static ITUIcon* mRadio_SpkOn;
	static ITUIcon* mRadio_LOUD;
	
	static ITUIcon* mRadio_PowerOff;
	static ITUIcon* mRadio_PowerOn;
	static ITUIcon* mRD_MODE_SEL[mRD_BT_CNT_ALL];
	static ITUButton* mRadio_Bt[mRD_BT_CNT_ALL];
	static ITUText* mRadio_ModeTx;
	static ITUIcon* mRadio_Stereo;
	static ITUText* mRadio_FreqTx;

	
	
	static ITUText* mRD_DAB2_MODE; //chpark.06.03
    static ITUText* mRD_DAB2_CAT;  //chpark.06.03
	
		static ITUText* mRD_BCSTx;
		static ITUText* mRD_PrgTx;
	static ITUIcon* mRD_Vol_ValD[10];
	static ITUIcon* mRD_Tone_ValD[10];
		static ITUIcon* mRD_FreqCh[8];
		
//		static ITUText* mRD_DAB2_PTY[6];
		static ITUText* mRD_DAB2_OND[6];
		
		
		static ITUIcon* mRD_DAB1;
		
		
		
		static ITUIcon* mRD_DAB2;


static ITUIcon* mRadio_AUTO;
static ITUIcon* mRadio_TUNE;
static ITUIcon* mRadio_SCAN;

static ITUIcon* mRD_DAB2_BG;
static ITUIcon* mRD_Bluetooth_BG;
static ITUIcon* mRD_Phone_BG;
static ITUIcon* mRD_PHONE1_HD;
static ITUIcon* mRD_PHONE2_HD;
static ITUIcon* mRD_PHONE3_HD;
static ITUIcon* mRD_BT_Pairing[4];
static ITUText* mRD_PhoneNum;//chpark.24.10.15

static ITUIcon* Radio_Black_BG;
static ITUIcon* Radio_Blue_BG;
static ITUIcon* mRD_DAB2_Blue_BG;
static ITUIcon* mRD_Bluetooth_Blue_BG;
static ITUIcon* mRD_Phone_Blue_BG;


uint8_t MainMode_AC_Select_Index = AC_SEL_AIRMODE;	// 0-6
uint8_t MainMode_RD_Select_Index = RD_SEL_MODE;
uint8_t MainMode_DAB_Select_Index = RD_SEL_MODE;
uint8_t MainMode_DAB2_Select_Index = RD_SEL_AUTO;//RD_SEL_MODE;   //chpark.24.10.07
uint8_t MainMode_BT_Select_Index = RD_SEL_MODE;
uint8_t MainMode_Phone_Select_Index = PH_SEL_R31;

uint8_t DAB2_PTY_Index = 0;	// 0-18


uint32_t Media_RunTime_Cnt = 0;
uint32_t PH_RunTime_Cnt = 0;

uint32_t Main_AC_Time_Cnt = 0;
uint32_t Main_RD_Time_Cnt = 0;

extern bool MainLayer_ButtonSel(uint8_t SelIndex,uint8_t OnOff);
//=====================================================
extern bool JogDialKey_Scroll_DAB2(char* param);	//chpark.06.21
extern bool JogDialRot_Scroll_DAB2(char* param);	//chpark.06.21
extern bool JogDialKey_Scroll_DAB2_H(char* param);	//chpark.06.21
extern bool JogDialRot_Scroll_DAB2_H(char* param);	//chpark.06.21
extern bool DAB2ListBox_SetPTY(int nIndex);			//chpark.06.21
extern bool DAB2ListBoxH_SetPTY(int nIndex);		//chpark.06.21

void DAB2_SetCategory(char* strTitle);//chpark.06.03
//chpark.06.03
void DAB2_SetCategory(char* strTitle)
{
	ituWidgetSetVisible(mRD_DAB2_CAT,strTitle);//chpark.11.15
	ituTextSetString(mRD_DAB2_CAT,strTitle);
}
void DAB2_Set_JD_Button(uint8_t btnIdx);//chpark.09.09

void DAB2_Set_JD_Button(uint8_t btnIdx)//chpark.09.09
{
	printf("====================DAB2_Set_JD_Button==%d \n",g_bDAB2_DataMode);//chpar.11.13
	g_bDAB2_DataMode  =true;      //chpark.11.15
	
	char JD_Buf[10] = {0,};
	uint8_t Select_Index = btnIdx;
	JD_Buf[0] = (Select_Index/10);
	JD_Buf[1] = (Select_Index%10);
	RD_MAIN_UpBt(NULL, JD_Buf);
	//MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
}

bool MainMode_DAB2_PTY_ON(uint8_t ReDis,DISP_MODE DispMode);//chpark.06.08
void MainMode_DAB2_Reflesh();//chpark.10.17
//=====================================================

//=====================================================
extern bool DAB3ListBox_Reflesh();//chpark.07.18

bool RD_MAIN_DnBt(ITUWidget* widget, char* param);
bool RD_MAIN_DnBt2(ITUWidget* widget, char* param);
bool AC_MAIN_UpBt2(ITUWidget* widget, char* param);
bool AC_MAIN_DnBt2(ITUWidget* widget, char* param);
bool AC_MAIN_UpBt(ITUWidget* widget, char* param);


void DAB3_Timer();//chpark.07.18

void DAB3_Timer()
{
	#ifdef USED_ROTATION //chpark.10.19
    if (RotationMode!=0)
    {
      return 0;
    }
    #endif
	
	
	if(Main_Disp_Mode == MAIN_MODE_DAB2 || Main_Disp_Mode == MAIN_MODE_DAB3)
	{
		if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
		{
			DAB3ListBox_Reflesh();
		}
		else
		{
			DAB3ListBoxH_Reflesh();
		}
		//
	}
	else
	{
		//printf("DAB3_Timer==\n");
	}
	

}

void MainMode_DAB2_Reflesh()//chpark.10.17
{
	/*
	if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
	{
		DAB3ListBox_Reflesh();
	}
	else
	{
		DAB3ListBoxH_Reflesh();
	}
	*/
}
//=====================================================




const int DAB_Freq[38] = {
					174928,		//0
					176640,
					178352,
					180064,
					181936,
					183648,		//5
					185360,
					187072,
					188928,
					190640,
					192352,		//10
					194064,
					195936,
					197648,
					199360,
					201072,		//15
					202928,
					204640,
					206352,
					208064,
					209936,		//20
					211648,
					213360,
					215072,
					216928,
					218640,		//25
					220352,
					222064,
					223936,
					225648,
					227360,		//30
					229072,
					230784,
					232496,
					234208,
					235776,		//35
					237488,
					239200		//37
				};
				
				
				
				
bool MainMode_AC_Sel(uint8_t ReDisp,uint8_t SelIndex,uint8_t OnOff)	// 0-6
{
	static uint8_t tSelIndex=0xFF;
	static uint8_t tOnOff=0xFF;
	int i;

//	printf("=================> MainMode_AC_Sel=%d %d %d\n",SelIndex,OnOff,Main_Disp_Mode);
	
//	printf("=================> MainMode_AC_Sel Start=%d %d %d\n",Main_Select_Index,MainMode_AC_Select_Index,MainMode_RD_Select_Index);
	
		
//	if ((tSelIndex!=SelIndex)||(tOnOff!=OnOff)||(ReDisp==1))
	{
		for (i=0;i<10;i++)
		{
			ituWidgetSetVisible(mAC_MODE_SEL[i], DISP_OFF);
		}
		if (OnOff==DISP_ON)
		{
			tSelIndex = SelIndex;
			switch (tSelIndex)
			{
			    case AC_SEL_AIRMODE:	//1
			    {
			    	if ((ACV.AirMode==0)||(ACV.AirMode==1))
			   			ituWidgetSetVisible(mAC_MODE_SEL[0], DISP_ON);
			    }
			    break;
			    case AC_SEL_ONOFF:		//2
			    {
			   		ituWidgetSetVisible(mAC_MODE_SEL[1], DISP_ON);
			    }
			    break;
			    case AC_SEL_DIR_L:		//3
			    case AC_SEL_DIR_R:		//4
			    {
			   		ituWidgetSetVisible(mAC_MODE_SEL[5], DISP_ON);
			    }
			    break;
			    case AC_SEL_TEMP_L:		//5
			    case AC_SEL_TEMP_R:		//6
			    {
			   		ituWidgetSetVisible(mAC_MODE_SEL[7], DISP_ON);
			    }
			    break;
			    case AC_SEL_LEVEL_L:	//7
			    case AC_SEL_LEVEL_R:	//8
			    {
			   		ituWidgetSetVisible(mAC_MODE_SEL[9], DISP_ON);
			    }
			    break;
			    case AC_SEL_AUTO:		//9
			    {
			   		ituWidgetSetVisible(mAC_MODE_SEL[2], DISP_ON);
			    }
			    break;
			    case AC_SEL_PWR:		//10
			    {
			   		ituWidgetSetVisible(mAC_MODE_SEL[3], DISP_ON);
			    }
			    break;
			}
			MainLayer_ButtonSel(Main_Select_Index,DISP_OFF);
		}
		tOnOff=OnOff;
		if (OnOff==DISP_ON)
		{
			MainMode_AC_Select_Index = SelIndex;
		}
//		printf("=================> MainMode_AC_Sel End=%d %d %d\n",Main_Select_Index,MainMode_AC_Select_Index,MainMode_RD_Select_Index);
	}
	return true;
}
bool MainMode_ACDisp(bool ReDisp,DISP_MODE DispMode)
{
	static uint8_t tInOutAir=99;
	static uint8_t tACTemp=99;
	static uint8_t tACOn=99;
	static uint8_t tACAuto=99;
	static uint8_t tACWindLv=0;
	static uint8_t tACWindDir=0;
	char tmp[30] = {0x20,};
	static uint8_t FirstFlag = 0;
	
 	int i;
// 	printf("MainMode_ACDisp=%d %d %d\n",MainMode_AC_Select_Index,ReDisp,DispMode);
//	if (MainLayerFlag)
	{
		if ((ReDisp==1)||(DispMode==DISP_OFF))
		{
			for (i=0;i<10;i++)
			{
				ituWidgetSetVisible(mAC_MODE_SEL[i], DISP_OFF);
				ituWidgetSetVisible(mAC_Bt[i], DISP_OFF);
			}
			printf("MainMode_ACDisp DISP_OFF\n");
			ituWidgetSetVisible(AC_BG, DISP_OFF);
			ituWidgetSetVisible(AC_Black_BG, DISP_OFF);
			ituWidgetSetVisible(AC_Blue_BG, DISP_OFF);
			tInOutAir=99;
			tACTemp=99;
			tACOn=99;
			tACAuto=99;
			tACWindLv=0;
			tACWindDir=0;
			memset(tmp,0x20,30);


		}

		if (((tInOutAir != ACV.AirMode)||(tACTemp != ACV.ACTemp)||(tACOn != ACV.ACOnOff)||(tACAuto != ACV.ACAuto)||(tACWindLv != ACV.ACLevel)||(tACWindDir != ACV.ACDir))&&(DispMode!=DISP_OFF))
		{
			for (i=0;i<6;i++)
			{
				ituWidgetSetVisible(mAC_WindDirD[i], DISP_OFF);
			}
			ituWidgetSetVisible(mAC_Wind_Off, DISP_OFF);

			ituWidgetSetVisible(mAC_Auto_Off, DISP_OFF);
			ituWidgetSetVisible(mAC_Auto_On, DISP_OFF);
			ituWidgetSetVisible(mAC_Off, DISP_OFF);
			ituWidgetSetVisible(mAC_On, DISP_OFF);
			ituWidgetSetVisible(mAC_InAir, DISP_OFF);
			ituWidgetSetVisible(mAC_OutAir, DISP_OFF);
				
			
			for (i=0;i<7;i++)
			{
				ituWidgetSetVisible(mAC_WindLvD[i], DISP_OFF);
			}
			ituWidgetSetVisible(mAC_TempTx, DISP_OFF);

			if (FirstFlag==0)
			{
				MainMode_AC_Select_Index = AC_SEL_AIRMODE;
				FirstFlag = 1;
			}

//printf("MainMode_ACDisp=%d\n",DispMode);
#if 1	

						
//			if (ACV.ACOnOff==1)
			{
				if (ACV.ACTemp==0xFF)		// #10075
				{
					sprintf(tmp, "     " );
				}
				else
				{
					if (SIUS_SEL_Flag==0)
						sprintf(tmp, "%d.%d", ((ACV.ACTemp*5)/10),((ACV.ACTemp*5)%10));
					else
					{
						int TempData;
						TempData = ((ACV.ACTemp*18*5)+3200)/10;	//(ACV.ACTemp*1.8*0.5)+32
						sprintf(tmp, "%d.%d", ((TempData)/10),((TempData)%10));
					}
				}
				ituTextSetString(mAC_TempTx, tmp);
				ituWidgetSetVisible(mAC_TempTx, DispMode);
				if (ACV.ACLevel!=0xF)
					ituWidgetSetVisible(mAC_WindLvD[ACV.ACLevel], DispMode);
				if (ACV.ACDir<=4)
					ituWidgetSetVisible(mAC_WindDirD[ACV.ACDir+1], DispMode);
				else
					ituWidgetSetVisible(mAC_WindDirD[0], DispMode);
			}
			if (ACV.ACOnOff==0)
			{
				ituWidgetSetVisible(mAC_Off, DispMode);
			}
			else
			{
				ituWidgetSetVisible(mAC_On, DispMode);
			}

			ituWidgetSetVisible(mAC_Wind_Off, DispMode);
			if (ACV.AirMode==0)
			{
				printf("ACV.AirMode==%d  :DISP_ON \n",ACV.AirMode);
				ituWidgetSetVisible(mAC_OutAir, DispMode);
				ituWidgetSetVisible(mAC_Bt[0], DISP_ON);
			}
			else if (ACV.AirMode==1)
			{
				printf("ACV.AirMode==%d  :DISP_ON \n",ACV.AirMode);
				ituWidgetSetVisible(mAC_InAir, DispMode);
				ituWidgetSetVisible(mAC_Bt[0], DISP_ON);
			}
			else
			{
				printf("ACV.AirMode==%d  :DISP_OFF \n",ACV.AirMode);
				ituWidgetSetVisible(mAC_Bt[0], DISP_OFF);
				if (MainMode_AC_Select_Index==AC_SEL_AIRMODE)
					MainMode_AC_Select_Index = AC_SEL_ONOFF;
			}
			if (ACV.ACAuto==1)
			{
				ituWidgetSetVisible(mAC_Auto_On, DispMode);
			}
			else
			{
				ituWidgetSetVisible(mAC_Auto_Off, DispMode);
			}
#endif
			tInOutAir = ACV.AirMode;
			tACTemp = ACV.ACTemp;
			tACOn = ACV.ACOnOff;
			tACAuto = ACV.ACAuto;
			tACWindLv = ACV.ACLevel;
			tACWindDir = ACV.ACDir;
			
			if (Main_Disp_Mode==MAIN_MODE_AIRCON)
			{
				switch (MainMode_AC_Select_Index)
				{
					case AC_SEL_ATT:
					case AC_SEL_AIRCON:
					case AC_SEL_RADIO:
					case AC_SEL_SPK:
					case AC_SEL_HOURMETER:
					case AC_SEL_BAR:
					case AC_SEL_TIMEDATE:
					case AC_SEL_POWER:
					case AC_SEL_CAMERA:
					case AC_SEL_MENU:
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
					break;
					default:
						MainMode_AC_Sel(1,MainMode_AC_Select_Index,DISP_ON);
					break;
				}
			}			
		}
	}
	return true;
}


#define VOL_UNIT	15
bool MainMode_RD_BarDisp(bool ReDisp ,uint8_t RDV_RDVol ,uint8_t RDV_RDTone ,DISP_MODE DispMode)
{
	static uint8_t TempVol = 0xFF;
	static uint8_t TempTone = 0xFF;
	static uint8_t RealVol = 0xFF;
	static uint8_t RealTone = 0xFF;
	static uint8_t tOnOff=0xFF;
	uint8_t tData = 0xFF,i;
	
	if ((TempVol!=RDV_RDVol)||(tOnOff!=DispMode))
	{
		TempVol = RDV_RDVol;
		RealVol = TempVol;
		if (RealVol>24)
		{
			RealVol = 24;
		}
		
		
		for (i=0;i<10;i++)
		{
			ituWidgetSetVisible(mRD_Vol_ValD[i], DISP_OFF);
		}		
		if (RealVol==0)
		{
			tData = 0;
		}
		else 
		{
			if (RealVol<4)
			{
				tData = 0;
			}
			else if (RealVol<6)
			{
				tData = 1;
			}
			else if (RealVol<9)
			{
				tData = 2;
			}
			else if (RealVol<11)
			{
				tData = 3;
			}
			else if (RealVol<14)
			{
				tData = 4;
			}
			else if (RealVol<16)
			{
				tData = 5;
			}
			else if (RealVol<19)
			{
				tData = 6;
			}
			else if (RealVol<21)
			{
				tData = 7;
			}
			else if (RealVol<24)
			{
				tData = 8;
			}
			else
			{
				tData = 9;
			}
			ituWidgetSetVisible(mRD_Vol_ValD[tData], DispMode);
		}
	}
	if ((TempTone!=RDV_RDTone)||(tOnOff!=DispMode))
	{
		TempTone = RDV_RDTone;
		RealTone = TempTone;
		if (RealTone>14)
		{
			RealTone = 14;
		}
		
		for (i=0;i<10;i++)
		{
			ituWidgetSetVisible(mRD_Tone_ValD[i], DISP_OFF);
		}		
		if (RealTone==0)
		{
			tData = 0;
		}
		else 
		{
			if (RealTone<3)
			{
				tData = 0;
			}
			else if (RealTone<4)
			{
				tData = 1;
			}
			else if (RealTone<6)
			{
				tData = 2;
			}
			else if (RealTone<7)
			{
				tData = 3;
			}
			else if (RealTone<9)
			{
				tData = 4;
			}
			else if (RealTone<10)
			{
				tData = 5;
			}
			else if (RealTone<12)
			{
				tData = 6;
			}
			else if (RealTone<13)
			{
				tData = 7;
			}
			else if (RealTone<14)
			{
				tData = 8;
			}
			else
			{
				tData = 9;
			}
			ituWidgetSetVisible(mRD_Tone_ValD[tData], DispMode);
		}
	}
	tOnOff=DispMode;
	return true;
}
bool MainMode_RD_Sel(uint8_t ReDisp,uint8_t SelIndex,uint8_t OnOff)	// 0-18
{
	static uint8_t tSelIndex=0xFF;
	static uint8_t tOnOff=0xFF;
	int i;
	
//	printf("=================> MainMode_RD_Sel=%d %d %d\n",SelIndex,OnOff,Main_Disp_Mode);
	
//	printf("=================> MainMode_RD_Sel Start=%d %d %d\n",Main_Select_Index,MainMode_AC_Select_Index,MainMode_RD_Select_Index);
	
	if ((tSelIndex!=SelIndex)||(tOnOff!=OnOff)||(ReDisp==1))
	{
		for (i=0;i<mRD_BT_CNT_ALL;i++)
		{
			ituWidgetSetVisible(mRD_MODE_SEL[i], DISP_OFF);
		}
		if (OnOff==DISP_ON)
		{
			tSelIndex = SelIndex;

			switch (tSelIndex)
			{
			    case RD_SEL_OFF:
			    {
			    }
			    break;
				case RD_SEL_1C:
				case RD_SEL_2C:
				case RD_SEL_3C:
				case RD_SEL_4C:
				case RD_SEL_5C:
				case RD_SEL_6C:
				case RD_SEL_7C:
				case RD_SEL_8C:
			    {
					ituWidgetSetVisible(mRD_MODE_SEL[SelIndex-1], DISP_ON);			    	
			    }
			    break;
				case RD_SEL_TUNE_L:
				case RD_SEL_TUNE_R:
			    {
					ituWidgetSetVisible(mRD_MODE_SEL[RD_SEL_TUNE_L-1], DISP_ON);
					ituWidgetSetVisible(mRD_MODE_SEL[RD_SEL_TUNE_R-1], DISP_ON);
			    }
			    break;
				case RD_SEL_AUTO:
			    {
			    	ituWidgetSetVisible(mRD_MODE_SEL[RD_SEL_AUTO-1], DISP_ON);
			    }
			    break;
				case RD_SEL_MODE:
			    {
			    	ituWidgetSetVisible(mRD_MODE_SEL[RD_SEL_MODE-1], DISP_ON);
			    }
			    break;
				case RD_SEL_TONE_D:
				case RD_SEL_TONE_U:
			    {
					ituWidgetSetVisible(mRD_MODE_SEL[RD_SEL_TONE_D-1], DISP_ON);
					ituWidgetSetVisible(mRD_MODE_SEL[RD_SEL_TONE_U-1], DISP_ON);
			    }
			    break;
				case RD_SEL_VOL_D:
				case RD_SEL_VOL_U:
			    {
					ituWidgetSetVisible(mRD_MODE_SEL[RD_SEL_VOL_D-1], DISP_ON);
					ituWidgetSetVisible(mRD_MODE_SEL[RD_SEL_VOL_U-1], DISP_ON);
			    }
			    break;
				case RD_SEL_PWR:
			    {
			    	ituWidgetSetVisible(mRD_MODE_SEL[RD_SEL_PWR-1], DISP_ON);
			    }
			    break;
				case RD_SEL_LOUD:
			    {
			    	ituWidgetSetVisible(mRD_MODE_SEL[RD_SEL_LOUD-1], DISP_ON);
			    }
			    break;
				case RD_SEL_SCAN:
				{
					ituWidgetSetVisible(mRD_MODE_SEL[RD_SEL_SCAN-1], DISP_ON);
				}
				break;
				case DAB2_SEL_1C:
				case DAB2_SEL_2C:
				case DAB2_SEL_3C:
				case DAB2_SEL_4C:
				case DAB2_SEL_5C:
				case DAB2_SEL_6C:
				case DAB2_SEL_7C:
				case DAB2_SEL_8C:
				{
					//chpark.06.21
					//All items of Index=>ON
					ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_1C-1], DISP_ON);
					ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_2C-1], DISP_ON);
					ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_3C-1], DISP_ON);
					ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_4C-1], DISP_ON);
					ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_5C-1], DISP_ON);
					ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_6C-1], DISP_ON);
					ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_7C-1], DISP_ON);
					if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
					{
						ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_8C-1], DISP_ON);//chpark.24.10.02
					}
					else
					{
						ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_8C-1], DISP_ON);
					}
					//
				}
				break;

				case BT_SEL_R27:
				case BT_SEL_R28:
				case BT_SEL_R29:
				case BT_SEL_R30:
				case PH_SEL_R34:
				case PH_SEL_R31:
				case PH_SEL_R32:
			    {
			    	ituWidgetSetVisible(mRD_MODE_SEL[SelIndex-1], DISP_ON);
			    }
			    break;
			}
			MainLayer_ButtonSel(Main_Select_Index,DISP_OFF);
		}
		else
		{
			//chpark.06.21
			for (i=0;i<mRD_BT_CNT_ALL;i++)
			{
				ituWidgetSetVisible(mRD_MODE_SEL[i], DISP_OFF);
			}
			
			tSelIndex = SelIndex;

			switch (tSelIndex)
			{
				case DAB2_SEL_1C:
				case DAB2_SEL_2C:
				case DAB2_SEL_3C:
				case DAB2_SEL_4C:
				case DAB2_SEL_5C:
				case DAB2_SEL_6C:
				case DAB2_SEL_7C:
				case DAB2_SEL_8C: //chpark.06.08
				{
					//chpark.06.21
					//All items of Index=>OFF
					ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_1C-1], DISP_OFF);
					ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_2C-1], DISP_OFF);
					ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_3C-1], DISP_OFF);
					ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_4C-1], DISP_OFF);
					ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_5C-1], DISP_OFF);
					ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_6C-1], DISP_OFF);
					ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_7C-1], DISP_OFF);
					ituWidgetSetVisible(mRD_MODE_SEL[DAB2_SEL_8C-1], DISP_OFF);
					
				}
				break;
			}

		}
		//tSelIndex=SelIndex;
		tOnOff=OnOff;
		if (OnOff==DISP_ON)
		{
			switch (Main_Disp_Mode)
			{
				case MAIN_MODE_RADIO:
				{
					MainMode_RD_Select_Index = SelIndex;
				}
				break;
				case MAIN_MODE_DAB1:
				{
					MainMode_DAB_Select_Index = SelIndex;
				}
				break;
				case MAIN_MODE_DAB2:
				case MAIN_MODE_DAB3:
				{
					MainMode_DAB2_Select_Index = SelIndex;
				}
				break;
				case MAIN_MODE_BLUETOOTH:
				{
					MainMode_BT_Select_Index = SelIndex;
				}
				break;
				case MAIN_MODE_PHONE:
				{
					MainMode_Phone_Select_Index = SelIndex;
				}
				break;
			}
		}
	}
//	printf("=================> MainMode_RD_Sel End=%d %d %d\n",Main_Select_Index,MainMode_AC_Select_Index,MainMode_RD_Select_Index);
	return true;
}


uint8_t AC_SEL_DIR_LR_FLAG = 0;
uint8_t AC_SEL_TEMP_LR_FLAG = 0;
uint8_t AC_SEL_LEVEL_LR_FLAG = 0;
uint32_t AC_SEL_DIR_R_CNT = LONG_KEY_TIME;
uint32_t AC_SEL_TEMP_R_CNT = LONG_KEY_TIME;
uint32_t AC_SEL_LEVEL_R_CNT = LONG_KEY_TIME;
uint32_t AC_SEL_DIR_L_CNT = LONG_KEY_TIME;
uint32_t AC_SEL_TEMP_L_CNT = LONG_KEY_TIME;
uint32_t AC_SEL_LEVEL_L_CNT = LONG_KEY_TIME;

bool MainMode_Bluetooth_Pairing_Flag = 0;


uint8_t RD_SEL_TUNE_LR_FLAG = 0;
uint8_t RD_SEL_TONE_DU_FLAG = 0;
uint8_t RD_SEL_VOL_DU_FLAG = 0;

uint32_t RD_SEL_TUNE_L_CNT = LONG_KEY_TIME;
uint32_t RD_SEL_TONE_D_CNT = LONG_KEY_TIME;
uint32_t RD_SEL_VOL_D_CNT = LONG_KEY_TIME;
uint32_t RD_SEL_TUNE_R_CNT = LONG_KEY_TIME;
uint32_t RD_SEL_TONE_U_CNT = LONG_KEY_TIME;
uint32_t RD_SEL_VOL_U_CNT = LONG_KEY_TIME;
extern void InitJogIncDecData_AC(void);
extern void InitJogIncDecData_RD(void);


void Clear_ACRD_TimeCnt(void)
{
	RD_SEL_TUNE_L_CNT = LONG_KEY_TIME;
	RD_SEL_TONE_D_CNT = LONG_KEY_TIME;
	RD_SEL_VOL_D_CNT = LONG_KEY_TIME;
	RD_SEL_TUNE_R_CNT = LONG_KEY_TIME;
	RD_SEL_TONE_U_CNT = LONG_KEY_TIME;
	RD_SEL_VOL_U_CNT = LONG_KEY_TIME;
	AC_SEL_DIR_R_CNT = LONG_KEY_TIME;
	AC_SEL_TEMP_R_CNT = LONG_KEY_TIME;
	AC_SEL_LEVEL_R_CNT = LONG_KEY_TIME;
	AC_SEL_DIR_L_CNT = LONG_KEY_TIME;
	AC_SEL_TEMP_L_CNT = LONG_KEY_TIME;
	AC_SEL_LEVEL_L_CNT = LONG_KEY_TIME;
						Main_CLI2[3] = 0;
						Main_CLI2[4] = 0;
							Main_CLI2[2] = 0;
}
bool AC_Toggle_Init(void)
{
	if (AC_SEL_DIR_LR_FLAG)
	{
		AC_SEL_DIR_LR_FLAG=0;
		MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
	}
	if(AC_SEL_TEMP_LR_FLAG)
	{
		AC_SEL_TEMP_LR_FLAG=0;
		MainMode_AC_Sel(0,AC_SEL_TEMP_L,DISP_ON);
	}
	if(AC_SEL_LEVEL_LR_FLAG)
	{
		AC_SEL_LEVEL_LR_FLAG=0;
		MainMode_AC_Sel(0,AC_SEL_LEVEL_L,DISP_ON);
	}
	InitJogIncDecData_AC();
	return true;
}
bool RD_Toggle_Init(void)
{
	
	printf("RD_Toggle_Init=%d,%d,%d,%d,%d\n",RD_SEL_TUNE_LR_FLAG,RD_SEL_TONE_DU_FLAG,RD_SEL_VOL_DU_FLAG);
	if (RD_SEL_TUNE_LR_FLAG)
	{
		RD_SEL_TUNE_LR_FLAG=0;
		MainMode_RD_Sel(0,RD_SEL_TUNE_L,DISP_ON);
	}
	if(RD_SEL_TONE_DU_FLAG)
	{
		RD_SEL_TONE_DU_FLAG=0;
		MainMode_RD_Sel(0,RD_SEL_TONE_D,DISP_ON);
	}
	if(RD_SEL_VOL_DU_FLAG)
	{
		RD_SEL_VOL_DU_FLAG=0;
		MainMode_RD_Sel(0,RD_SEL_VOL_D,DISP_ON);
	}
	InitJogIncDecData_RD();
	//chpark.06.21 
	//JOG_DIAL_SW_ESC_ON in scroll mode of(DAB2,DAB3)
	if(MainMode_DAB2_Select_Index == RD_SEL_SCROLL)
	{
		MainMode_DAB2_Select_Index = DAB2_SEL_1C;
		MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
		//chpark.06.21
		//3333=>set Focus on PTY position.(Set to 0 if there is no  PTY.)
		//-1=>set No Focus in Scroll
        //
		if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
		{
			DAB2ListBox_Select(-1);
		}
		else
		{
			DAB2ListBoxH_Select(-1);
		}
		//

	}
	//
	return true;
}

bool MainMode_RD_Init(void)
{
	int i;

	printf("============MainMode_RD_Init Start:%d\n",g_bDAB2_DataMode);//chpark.11.15
	
	//chpark.24.10.15
	/*
	if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
	{
		//chpark.24.09.06
		//ituWidgetSetPosition(mRadio_FreqTx, 146, 663);
		ituWidgetSetPosition(mRadio_FreqTx, 198, 345);
	}
	*/
	InitJogIncDecData_AC();
	InitJogIncDecData_RD();
//ok return true;	
	ituWidgetSetVisible(RD_BG, DISP_OFF);
	MainMode_Bluetooth_Pairing_Flag = 0;
	ituWidgetSetVisible(mRadio_AUTO, DISP_OFF);
	ituWidgetSetVisible(mRadio_TUNE, DISP_OFF);
	ituWidgetSetVisible(mRadio_SCAN, DISP_OFF);
	for (i=0;i<8;i++)
	{
		ituWidgetSetVisible(mRD_FreqCh[i], DISP_OFF);
	}
	ituWidgetSetVisible(mRD_BCSTx, DISP_OFF);
	ituWidgetSetVisible(mRD_PrgTx, DISP_OFF);
	for (i=0;i<6;i++)
	{
//		ituWidgetSetVisible(mRD_DAB2_PTY[i], DISP_OFF);
		ituWidgetSetVisible(mRD_DAB2_OND[i], DISP_OFF);
	}
	
	ituWidgetSetVisible(mRD_DAB1, DISP_OFF);
	ituWidgetSetVisible(mRD_DAB2, DISP_OFF);
	
	for (i=0;i<mRD_BT_CNT_ALL;i++)
	{
		ituWidgetSetVisible(mRD_MODE_SEL[i], DISP_OFF);
		ituWidgetSetVisible(mRadio_Bt[i], DISP_OFF);
	}
//ok return true;


	ituWidgetSetVisible(Radio_Black_BG, DISP_OFF);
	ituWidgetSetVisible(mRD_DAB2_BG, DISP_OFF);
	ituWidgetSetVisible(mRD_Bluetooth_BG, DISP_OFF);
	ituWidgetSetVisible(mRD_Phone_BG, DISP_OFF);

//	ituWidgetSetVisible(mRadio_Mute, DISP_OFF);
//	ituWidgetSetVisible(mRadio_SpkOn, DISP_OFF);
	ituWidgetSetVisible(mRadio_LOUD, DISP_OFF);
	
	ituWidgetSetVisible(mRadio_Stereo, DISP_OFF);
	ituWidgetSetVisible(mRadio_PowerOff, DISP_OFF);
	ituWidgetSetVisible(mRadio_PowerOn, DISP_OFF);
	ituWidgetSetVisible(mRadio_ModeTx, DISP_OFF);
	ituWidgetSetVisible(mRadio_FreqTx, DISP_OFF);
	ituWidgetSetVisible(mRD_PhoneNum, DISP_OFF);//chpark.24.10.18
	ituWidgetSetVisible(mRD_DAB2_MODE, DISP_OFF);
	ituWidgetSetVisible(mRD_DAB2_CAT, DISP_OFF);
//NG return true;	
	for (i=0;i<4;i++)
	{
		ituWidgetSetVisible(mRD_BT_Pairing[i], DISP_OFF);
	}

	ituWidgetSetVisible(Radio_Blue_BG, DISP_OFF);
	ituWidgetSetVisible(mRD_DAB2_Blue_BG, DISP_OFF);
	ituWidgetSetVisible(mRD_Bluetooth_Blue_BG, DISP_OFF);
	ituWidgetSetVisible(mRD_Phone_Blue_BG, DISP_OFF);
	
	ituWidgetSetVisible(mRD_PHONE1_HD, DISP_OFF);
	ituWidgetSetVisible(mRD_PHONE2_HD, DISP_OFF);
	ituWidgetSetVisible(mRD_PHONE3_HD, DISP_OFF);

//NG return true;	
#ifdef NEW_GUI	
	if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
	{
		//chpark.24.10.02
		/*
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_AUTO-1], 310, 1008);
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_MODE-1], 450, 1008);
		
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_AUTO-1], 310, 1008);
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_MODE-1], 450, 1008);
		*/
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_AUTO-1], 429, 783);
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_MODE-1], 602, 783);
		ituWidgetSetVisible(mRadio_Bt[RD_SEL_MODE-1], DISP_ON);
		
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_AUTO-1], 429, 783);
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_MODE-1], 602, 783);
		ituWidgetSetVisible(mRD_MODE_SEL[RD_SEL_MODE-1], DISP_ON);
	}
	else
	{
		//chpark.24.10.02
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_AUTO-1], 374, 621);
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_MODE-1], 548, 621);
		ituWidgetSetVisible(mRadio_Bt[RD_SEL_MODE-1], DISP_ON);
		
		
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_AUTO-1], 374, 621);
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_MODE-1], 548, 621);
		ituWidgetSetVisible(mRD_MODE_SEL[RD_SEL_MODE-1], DISP_ON);
	}
#else
	if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
	{
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_AUTO-1], 310, 1008);
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_MODE-1], 451, 1008);
		
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_AUTO-1], 305, 1003);
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_MODE-1], 445, 1003);
	}
	else
	{
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_AUTO-1], 275, 621);
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_MODE-1], 399, 621);
		
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_AUTO-1], 275, 621);
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_MODE-1], 399, 621);
	}
#endif
//NG return true;	
	//
	//chpark.11.15
	
	if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
	{
		MainOnDAB2_OnOff(DISP_OFF);
		MainOnDAB2_H_OnOff(DISP_OFF);//chpark.05.30
	}
	else
	{
		MainOnDAB2_OnOff(DISP_OFF);
		MainOnDAB2_H_OnOff(DISP_OFF);//chpark.05.30
	}
	
	//	
//	ituWidgetSetVisible(Radio_Off, DISP_OFF);
//	ituWidgetSetVisible(SPK_ON, DISP_OFF);
//	ituWidgetSetVisible(SPK_OFF, DISP_OFF);
//NG return true;		
	printf("MainMode_RD_Init End\n");			
	return true;
}

bool MainMode_DAB2_PTY_ON(uint8_t ReDis,DISP_MODE DispMode)
{
	uint8_t i,Stsel;
	static int tPTYNo=-1;
	static uint8_t tPTY_Index=0xFF;
	static uint8_t tDispMode=0xFF;
	char tmp[30] = {0x20,};
	
	
	if ((MainLayerFlag))
	{
		//if ((tPTYNo!=RDV.PTYNo)||(tPTY_Index!=DAB2_PTY_Index)||(tDispMode!=DispMode)||(ReDis==1))
		//if ((tPTY_Index!=DAB2_PTY_Index)||(tDispMode!=DispMode)||(ReDis==1))
		if ((tPTY_Index!=RDV.PTYNo)||(DispMode==DISP_ON)||(ReDis==1))//chpark.09.11
		{
			uint8_t JD_Buf[2] = {0xBB,0x00};
			JD_Buf[0]=0xBB;

			//chpark.05.30
			if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
			{  
				//DAB2ListBoxOnSelection(NULL, JD_Buf);
				printf("\n\n DAB2ListBox_SetPTY: [RDV.PTYNo=%d]\n",RDV.PTYNo);
		
				DAB2ListBox_SetPTY(RDV.PTYNo);//chpark.06.21
			}
			else
			{
				printf("\n\n DAB2ListBoxH_SetPTY: [RDV.=PTYNo=%d]\n",RDV.PTYNo);
				
				DAB2ListBoxH_SetPTY(RDV.PTYNo);//chpark.06.21
			}

			tPTYNo=RDV.PTYNo;
			tPTY_Index=DAB2_PTY_Index;
			tDispMode=DispMode;
		}
		else
		{
			printf("The Same PTY====%d\n",RDV.PTYNo);
		}
	}
	
	return true;
}

bool MainMode_DAB2_PTY_Disp(uint8_t IncDec,uint8_t SelIndex,DISP_MODE DispMode)
{
	//chpark.06.21
	/*
	int tSelIndex=0xFF,i,Stsel;
	char tmp[30] = {0,};

	tSelIndex = SelIndex;
	if (IncDec==1)	//0:NOP , 1: INC , 2:DEC
	{
		tSelIndex = tSelIndex+6;
		if (tSelIndex>=29)
		{
			tSelIndex = tSelIndex%6;
		}
	}
	else if (IncDec==2)
	{
		if (tSelIndex<6)
		{
			tSelIndex = tSelIndex+24;
		}
		else
			tSelIndex = tSelIndex-6;
	}
	DAB2_PTY_Index = tSelIndex;
	Stsel = DAB2_PTY_Index/6;

	printf("MainMode_DAB2_PTY_Disp:%d %d %d\n",SelIndex,Stsel,(DAB2_PTY_Index%6));
	for (i=0;i<6;i++)
	{
		memset (tmp,0x00,30);
//		sprintf(tmp, " %d %s",(Stsel*6+i),&DAB2_PTY_No[Stsel*6+i][0]);
		printf("MainMode_DAB2_PTY_Disp:[%s] \n",tmp);
//		ituTextSetString(mRD_DAB2_PTY[i], tmp);
	}
	MainMode_DAB2_PTY_ON(1,DispMode);
	*/
	return true;
}

bool MainMode_AMFM_Init(DISP_MODE DispMode)
{
	int i;
	static uint8_t FirstFlag = 0;
	printf("MainMode_AMFM_Init %d\n",DispMode);
	ituWidgetSetVisible(RD_BG, DispMode);
	
	//chpark.24.10.15
	ituWidgetSetVisible(mRadio_FreqTx, DispMode);
	ituWidgetSetVisible(mRD_PhoneNum, !DispMode); //chpark.24.10.18
	/*
	if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
	{
		//chpark.24.09.06
		//ituWidgetSetPosition(mRadio_FreqTx, 146, 663);
		ituWidgetSetPosition(mRadio_FreqTx, 198, 345);		
	}
	*/
	if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
	{
		ituWidgetSetVisible(Radio_Blue_BG, DispMode);
	}
	else
	{
		ituWidgetSetVisible(Radio_Black_BG, DispMode);
	}
	ituWidgetSetVisible(mRadio_AUTO, DispMode);
	ituWidgetSetVisible(mRadio_TUNE, DispMode);
	for (i=0;i<mRD_BT_Radio;i++)	// SCAN off
	{
		ituWidgetSetVisible(mRadio_Bt[i], DispMode);
	}
	if (MainMode_RD_Select_Index>RD_SEL_LOUD)
	{
//		MainMode_RD_Select_Index = RD_SEL_LOUD; 
	}
	if (FirstFlag==0)
	{
		MainMode_RD_Select_Index = RD_SEL_MODE;
		FirstFlag = 1;
	}
	return true;
}
bool MainMode_DAB1_Init(DISP_MODE DispMode)
{
	int i;
	static uint8_t FirstFlag = 0;
	printf("MainMode_DAB1_Init %d\n",DispMode);

	ituWidgetSetVisible(RD_BG, DispMode);
	
	//chpark.24.10.15
	ituWidgetSetVisible(mRadio_FreqTx, DispMode);
	ituWidgetSetVisible(mRD_PhoneNum, !DispMode); //chpark.24.10.18
	/*
	if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
	{
		//chpark.24.09.06
		//ituWidgetSetPosition(mRadio_FreqTx, 146, 663);
		ituWidgetSetPosition(mRadio_FreqTx, 198, 345);
	}
	*/
	if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
	{
		ituWidgetSetVisible(Radio_Blue_BG, DispMode);
	}
	else
	{
		ituWidgetSetVisible(Radio_Black_BG, DispMode);
	}
	ituWidgetSetVisible(mRD_DAB1, DispMode);
				
	ituWidgetSetVisible(mRadio_SCAN, DispMode);
	ituWidgetSetVisible(mRD_BCSTx, DispMode);
	ituWidgetSetVisible(mRD_PrgTx, DispMode);
	for (i=0;i<mRD_BT_DAB1;i++)
	{
		ituWidgetSetVisible(mRadio_Bt[i], DispMode);
	}
	
	if (FirstFlag==0)
	{
		MainMode_DAB_Select_Index = RD_SEL_MODE;
		FirstFlag = 1;
	}

	return true;
}

bool DAB2ListOnEnter();		//chpark.11.15
bool DAB2ListH_OnEnter();	//chpark.11.15

bool MainMode_DAB2_Init(DISP_MODE DispMode)
{
	int i;
	static uint8_t FirstFlag = 0;
	
	printf("MainMode_DAB2_Init %d\n",DispMode);
	
	//chpark.11.15
	if(DispMode == DISP_OFF)
	{
		MainOnDAB2_OnOff(DISP_OFF);
		MainOnDAB2_H_OnOff(DISP_OFF);//chpark.05.30
	}
	else
	{ 
		if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
		{
			//chpark.24.09.06
			//ituWidgetSetPosition(mRadio_FreqTx, 146, 663);
			ituWidgetSetPosition(mRadio_FreqTx, 198, 345);
			
			DAB2ListOnEnter();
		}
		else
		{
			DAB2ListH_OnEnter();
		}
	}
	
	
	ituWidgetSetVisible(RD_BG, DispMode);
	ituWidgetSetVisible(DAB2_BG, DispMode);

	if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
	{
		ituWidgetSetVisible(mRD_DAB2_Blue_BG, DispMode);
	}
	else
	{
		ituWidgetSetVisible(mRD_DAB2_BG, DispMode);
	}

	ituWidgetSetVisible(mRD_DAB2, DispMode);

#ifdef NEW_GUI
	if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
	{
		//chpark.24.10.02
		/*
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_AUTO-1], 670, 721);
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_MODE-1], 670, 801);
		
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_AUTO-1], 670, 721);
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_MODE-1], 670, 801);
		*/
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_AUTO-1], 610, 170);
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_AUTO-1], 610, 170);
	}
	else
	{
		//chpark.24.10.02
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_AUTO-1], 802, 199);
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_AUTO-1], 802, 199);

	}
#else
	if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
	{
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_AUTO-1], 671, 721);
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_MODE-1], 671, 801);
		
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_AUTO-1], 666, 716);
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_MODE-1], 666, 796);
	}
	else
	{
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_AUTO-1], 895, 460);
		ituWidgetSetPosition(mRadio_Bt[RD_SEL_MODE-1], 895, 535);
		
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_AUTO-1], 895, 460);
		ituWidgetSetPosition(mRD_MODE_SEL[RD_SEL_MODE-1], 895, 535);
	}
#endif
//    RD_SEL_AUTO		= 11,
//    RD_SEL_MODE 	= 12,


    
	ituWidgetSetVisible(mRadio_Bt[RD_SEL_AUTO-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[RD_SEL_MODE-1], DISP_OFF);//chpark.24.09.05
	ituWidgetSetVisible(mRadio_Bt[RD_SEL_PWR-1], DispMode);
	
/*	
	ituWidgetSetVisible(mRadio_Bt[DAB2_SEL_1C-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[DAB2_SEL_2C-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[DAB2_SEL_3C-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[DAB2_SEL_4C-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[DAB2_SEL_5C-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[DAB2_SEL_6C-1], DispMode);
	*/
	//DAB2_PTY_Index = 0;
	//MainMode_DAB2_PTY_Disp(0,DAB2_PTY_Index,DispMode);//chpark.11.15
	
	for (i=0;i<6;i++)
	{
//		ituWidgetSetVisible(mRD_DAB2_PTY[i], DispMode);
	}
	if (FirstFlag==0)
	{
		//MainMode_DAB2_Select_Index = RD_SEL_AUTO;//chpark.24.11.11
		MainMode_DAB2_Select_Index = DAB2_SEL_1C;//chpark.24.11.11
		FirstFlag = 1;
	}

	return true;
}

bool MainMode_Bluetooth_PairDisp(bool ToggleFlag , DISP_MODE DispMode)
{
	int i;
	static uint8_t FirstFlag = 0;
	
//	printf("MainMode_Bluetooth_Init %d\n",DispMode);

	for (i=0;i<4;i++)
	{
		ituWidgetSetVisible(mRD_BT_Pairing[i], DISP_OFF);
	}
	
	if (DispMode!=DISP_OFF)
	{
		switch (RDV.BTPair)
		{
			case 1:
				if (ToggleFlag)
					ituWidgetSetVisible(mRD_BT_Pairing[1], DispMode);
				else
					ituWidgetSetVisible(mRD_BT_Pairing[2], DispMode);
			break;
			case 2:
				ituWidgetSetVisible(mRD_BT_Pairing[3], DispMode);
			break;
			default :
				ituWidgetSetVisible(mRD_BT_Pairing[0], DispMode);
			break;
		}
	}
	else
	{
		ituWidgetSetVisible(mRD_BT_Pairing[0], DISP_ON);
	}
	return true;
}

bool MainMode_Bluetooth_Init(DISP_MODE DispMode)
{
	int i;
	static uint8_t FirstFlag = 0;
	
//	printf("MainMode_Bluetooth_Init %d\n",DispMode);
	ituWidgetSetVisible(RD_BG, DispMode);
	ituWidgetSetVisible(BT_BG, DispMode);
	if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
	{
		//chpark.24.09.06
		//ituWidgetSetPosition(mRadio_FreqTx, 146, 663);
		ituWidgetSetPosition(mRadio_FreqTx, 198, 345);
	}
	
	if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
	{
		ituWidgetSetVisible(mRD_Bluetooth_Blue_BG, DispMode);
	}
	else
	{
		ituWidgetSetVisible(mRD_Bluetooth_BG, DispMode);
	}

	ituWidgetSetVisible(mRadio_Bt[RD_SEL_MODE-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[RD_SEL_TONE_D-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[RD_SEL_TONE_U-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[RD_SEL_VOL_D-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[RD_SEL_VOL_U-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[RD_SEL_PWR-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[RD_SEL_LOUD-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[BT_SEL_R27-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[BT_SEL_R28-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[BT_SEL_R29-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[BT_SEL_R30-1], DispMode);

	for (i=0;i<4;i++)
	{
		ituWidgetSetVisible(mRD_BT_Pairing[i], DISP_OFF);
	}
	MainMode_Bluetooth_PairDisp(0 , DispMode);
	if (FirstFlag==0)
	{
		MainMode_BT_Select_Index = RD_SEL_MODE;
		FirstFlag = 1;
	}
	MainMode_Bluetooth_Pairing_Flag = 1;
	return true;
}

bool MainMode_Phone_Init(DISP_MODE DispMode)
{
	int i;
	static uint8_t FirstFlag = 0;
	
	printf("MainMode_Phone_Init [%d][%d]\n",DispMode,RDV.RDPhone);
	ituWidgetSetVisible(RD_BG, DispMode);
	ituWidgetSetVisible(PH_BG, DispMode);
	
	//chpark.24.10.15
	ituWidgetSetVisible(mRadio_FreqTx, !DispMode);
	ituWidgetSetVisible(mRD_PhoneNum, DispMode); //chpark.24.10.18
	/*
	if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
	{
		//chpark.24.09.06
		//ituWidgetSetPosition(mRadio_FreqTx, 224, 663);
		ituWidgetSetPosition(mRadio_FreqTx, 157, 331);
	}
	*/
	if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
	{
		ituWidgetSetVisible(mRD_Phone_Blue_BG, DispMode);
	}
	else
	{
		ituWidgetSetVisible(mRD_Phone_BG, DispMode);
	}
	ituWidgetSetVisible(mRadio_Bt[RD_SEL_TONE_D-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[RD_SEL_TONE_U-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[RD_SEL_VOL_D-1], DispMode);
	ituWidgetSetVisible(mRadio_Bt[RD_SEL_VOL_U-1], DispMode);
//	ituWidgetSetVisible(mRadio_Bt[RD_SEL_PWR-1], DispMode); // JPHong0728   10966
#if 0	
	if ((Phone_Time+TIME_CNT_15SEC)>gMainTimerCnt_10m)
	{
		ituWidgetSetVisible(mRadio_Bt[PH_SEL_R32-1], DispMode);
		ituWidgetSetVisible(mRD_PHONE1_HD, DispMode);
		ituWidgetSetVisible(mRD_PHONE2_HD, DispMode);
	}
	else 
#endif	
	{

		if ((RDV.RDPhone==2)||(RDV.RDPhone==3))
		{
			ituWidgetSetVisible(mRadio_Bt[PH_SEL_R34-1], DispMode);
			ituWidgetSetVisible(mRD_PHONE2_HD, DispMode);
			ituWidgetSetVisible(mRD_PHONE3_HD, DispMode);
			Phone_Time=PHONE_TIME;
			if ((MainMode_Phone_Select_Index == PH_SEL_R31)||(MainMode_Phone_Select_Index == PH_SEL_R32))
				MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
		}
		else if (RDV.RDPhone==1)
		{
			ituWidgetSetVisible(mRadio_Bt[PH_SEL_R34-1], DispMode);
			ituWidgetSetVisible(mRadio_Bt[PH_SEL_R31-1], DispMode);
			ituWidgetSetVisible(mRD_PHONE3_HD, DispMode);
			Phone_Time=PHONE_TIME;
			if ((MainMode_Phone_Select_Index == PH_SEL_R32)||(MainMode_Phone_Select_Index == PH_SEL_R34))
				MainMode_RD_Sel(1,PH_SEL_R31,DISP_ON);
		}
		else if (RDV.RDPhone==4)
		{
			ituWidgetSetVisible(mRadio_Bt[PH_SEL_R32-1], DispMode);
			ituWidgetSetVisible(mRD_PHONE1_HD, DispMode);
			ituWidgetSetVisible(mRD_PHONE2_HD, DispMode);
			Phone_Time=PHONE_TIME;
			if ((MainMode_Phone_Select_Index == PH_SEL_R31)||(MainMode_Phone_Select_Index == PH_SEL_R34))
				MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
		}
		else if (RDV.RDPhone==5)
		{
			if ((Phone_Time+TIME_CNT_15SEC)>gMainTimerCnt_10m)
			{
				ituWidgetSetVisible(mRadio_Bt[PH_SEL_R32-1], DispMode);
				ituWidgetSetVisible(mRD_PHONE1_HD, DispMode);
				ituWidgetSetVisible(mRD_PHONE2_HD, DispMode);
				if ((MainMode_Phone_Select_Index == PH_SEL_R31)||(MainMode_Phone_Select_Index == PH_SEL_R34))
					MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
			}
			else
			{
				char JD_Buf[10] = {0,};
				RDV.RDPhone=0;
				Phone_Time=PHONE_TIME;
				JD_Buf[0] = 0;
				MainHome(NULL, JD_Buf);
			}
		}
		else
		{
			ituWidgetSetVisible(mRadio_Bt[PH_SEL_R34-1], DispMode);
			ituWidgetSetVisible(mRadio_Bt[PH_SEL_R31-1], DispMode);
			ituWidgetSetVisible(mRadio_Bt[PH_SEL_R32-1], DispMode);
			Phone_Time=PHONE_TIME;
			MainMode_RD_Sel(1,PH_SEL_R31,DISP_ON);
		}
	}
	//ituWidgetSetVisible(mRadio_Bt[PH_SEL_R32-1], DispMode);
	
	if (FirstFlag==0)
	{
//		MainMode_Phone_Select_Index = RD_SEL_OFF;
		FirstFlag = 1;
	}

	return true;
}

bool MainMode_RD_SEL_CH_DISP(bool RD_DAB,DISP_MODE DispMode)
{
	int i;
	for (i=0;i<8;i++)
	{
		ituWidgetSetVisible(mRD_FreqCh[i], DISP_OFF);
	}
	if (RD_DAB==0)//rd
	{
		if ((RDV.RDSelCh<3)||(RDV.RDSelCh>10))
			return true;
	}
	else
	{
		if ((RDV.DABSelCh<1)||(RDV.DABSelCh>8))
			return true;
	}		
	if (RD_DAB==0)//rd
	{
		ituWidgetSetVisible(mRD_FreqCh[RDV.RDSelCh-3], DispMode);
	}
	else
	{
		ituWidgetSetVisible(mRD_FreqCh[RDV.DABSelCh-1], DispMode);
	}
	return true;
}

bool MainMode_RDDisp(bool ReDisp,DISP_MODE DispMode)
{
	static uint8_t tRDOnOff=99;
	static uint8_t tRDStereo=99;
	static uint8_t tRDMode=99;
	static uint16_t tRDFreq=0xFFFF;
	static uint8_t tRDStep=99;
	static uint8_t tRDTone=99;
	static uint8_t tRDVol=99;
	static uint8_t tMute=99;
	static uint8_t tLOUD=99;
	static uint8_t tMDM=99;
	static uint8_t tRDPhone=99;
	static uint8_t tRDPrg[120]={0,};
	static uint8_t tRDBCS[120]={0,};
	static uint8_t tPhoneNum[10]={0,};
	static uint8_t tRDSelCh = 99;
	static uint8_t tDABSelCh = 99;
	static uint8_t tDABFreq = 99;
	static uint32_t tPhone_Time = 0;
	static uint8_t tRadio_Region = 99;
	char tmp[120] = {0x20,};
 	int i;


	if (Main_Disp_Mode_Flag==1)
	{
		printf("Return MainMode_RDDisp=%d\n",Main_Disp_Mode_Flag);
//		return 0;
	}


// 	printf("MainMode_RDDisp=%d %d %d\n",ReDisp,Main_Disp_Mode,DispMode);
//	if (MainLayerFlag)
	{
		
		
		if ((ReDisp==1)||(DispMode==DISP_OFF)||(tRDMode!=RDV.RDMode)||(tRDPhone!=RDV.RDPhone)||(tMDM!=Main_Disp_Mode)||(tMDM!=Main_Disp_Mode))
		{
			printf("\n\n====MainMode_RDDisp 0 ='%d''%d''%d''%d''%d'\n",(ReDisp==1),(DispMode==DISP_OFF),(tRDMode!=RDV.RDMode),(tRDPhone!=RDV.RDPhone),(tMDM!=Main_Disp_Mode));
		
			MainMode_RD_Init();
			

			if (DispMode==DISP_OFF)
			{
			//	if (RDV.RDPhone==4)
			//	{
			//		RDV.RDPhone=0;
			//		Phone_Time=PHONE_TIME;
			//	}
			}
			if (((tRDPhone!=RDV.RDPhone)||(tRDMode!=RDV.RDMode))&&(DispMode!=DISP_OFF)&&((Main_Disp_Mode == MAIN_MODE_PHONE)||(Main_Disp_Mode == MAIN_MODE_RADIO)||(Main_Disp_Mode == MAIN_MODE_DAB1)||(Main_Disp_Mode == MAIN_MODE_DAB2)||(Main_Disp_Mode == MAIN_MODE_DAB3)||(Main_Disp_Mode == MAIN_MODE_BLUETOOTH)||(Main_Disp_Mode == MAIN_MODE_PHONE)))
			{
//				printf("[RI5] RDV.RDPhone MMD 1: [%d] [%x] [%d]\n", RDV.RDPhone,Phone_Time,Main_Disp_Mode);
				if (RDV.RDPhone==5)
				{
					if ((Phone_Time+TIME_CNT_15SEC)>gMainTimerCnt_10m)
					{
						Main_Disp_Mode = MAIN_MODE_PHONE;
					}
					else
					{
						char JD_Buf[10] = {0,};
						RDV.RDPhone=0;
						Phone_Time=PHONE_TIME;
						JD_Buf[0] = 0;
						MainHome(NULL, JD_Buf);
					}
				}
				else if ((RDV.RDPhone==1)||(RDV.RDPhone==2)||(RDV.RDPhone==3)||(RDV.RDPhone==4))
				{
					Main_Disp_Mode = MAIN_MODE_PHONE;
				}
				else
				{
					switch (RDV.RDMode)
					{
						case 0://AM
						{
							Main_Disp_Mode = MAIN_MODE_RADIO;
						}
						break;
						case 1://FM
						{
							Main_Disp_Mode = MAIN_MODE_RADIO;
						}
						break;
						case 2://BT
						{
							Main_Disp_Mode = MAIN_MODE_BLUETOOTH;
						}
						break;
						case 3://DAB1
						{
							if ((Main_Disp_Mode != MAIN_MODE_DAB2)&&(Main_Disp_Mode != MAIN_MODE_DAB3))
								Main_Disp_Mode = MAIN_MODE_DAB1;
						}
						break;
					}
				}
			}
			
			tRDOnOff=99;
			tRDStereo=99;
			tRDMode=99;
			tRDFreq=0xFFFF;
			tRDStep=99;
			tRDTone=99;
			tRDVol=99;
			tMute=99;
			tLOUD=99;
			tRDSelCh = 99;
			tDABSelCh = 99;
			tDABFreq = 99;
			tPhone_Time=0;
			tRadio_Region = 99;
			tRDPhone=RDV.RDPhone;
			
			memset (tRDPrg,0,sizeof(tRDPrg));
			memset (tRDBCS,0,sizeof(tRDPrg));
			memset (tPhoneNum,0,sizeof(tPhoneNum));
			memset(tmp,0x20,sizeof(tmp));
//			printf("[RI5] RDV.RDPhone MMD 2: [%d] [%x] [%d]\n", RDV.RDPhone,Phone_Time,Main_Disp_Mode);
			if (DispMode!=DISP_OFF)
			{
				uint8_t Temp_Select_Index=0xFF;
				switch (Main_Disp_Mode)
				{
					case MAIN_MODE_PHONE:
					{
						MainMode_Phone_Init(DispMode);
						Temp_Select_Index = MainMode_Phone_Select_Index;
					}
					break;
					case MAIN_MODE_RADIO://AM FM
					{
						MainMode_AMFM_Init(DispMode);
						Temp_Select_Index = MainMode_RD_Select_Index;
					}
					break;
					case MAIN_MODE_BLUETOOTH://BT
					{
						MainMode_Bluetooth_Init(DispMode);
						Temp_Select_Index = MainMode_BT_Select_Index;
					}
					break;
					case MAIN_MODE_DAB1://DAB1
					{
						MainMode_DAB1_Init(DispMode);
						Temp_Select_Index = MainMode_DAB_Select_Index;
					}
					break;
					case MAIN_MODE_DAB2://DAB2
					case MAIN_MODE_DAB3://DAB2
					{
						MainMode_DAB2_Init(DispMode);
						Temp_Select_Index = MainMode_DAB2_Select_Index;
					}
					break;
				}
				switch (Temp_Select_Index)
				{
					case RD_SEL_ATT:
					case RD_SEL_AIRCON:
					case RD_SEL_RADIO:
					case RD_SEL_SPK:
					case RD_SEL_HOURMETER:
					case RD_SEL_BAR:
					case RD_SEL_TIMEDATE:
					case RD_SEL_POWER:
					case RD_SEL_CAMERA:
					case RD_SEL_MENU:
						MainLayer_ButtonSel(Temp_Select_Index,DISP_ON);
					break;
					default:
						MainMode_RD_Sel(1,Temp_Select_Index,DISP_ON);
					break;
				}
			}
			tMDM=Main_Disp_Mode;
		}
	
		if (((tRadio_Region != Radio_Region)||(tRDSelCh != RDV.RDSelCh)||(strcmp(tPhoneNum,RDV.RDPhoneNum) != 0)||(tDABFreq != RDV.DABFreq)||(tRDStep != RDV.RDStep)||(tDABSelCh != RDV.DABSelCh)||(tLOUD != RDV.RDLOUD)||(tMute != RDV.RDMute)||(tRDOnOff != RDV.RDOnOff)||(tRDStereo != RDV.RDStereo)||(tRDMode != RDV.RDMode)||(tRDFreq != RDV.RDFreq)||(tRDTone != RDV.RDTone)||(tRDVol != RDV.RDVol)||(strcmp(tRDPrg,RDV.RDPrg) != 0)||(strcmp(tRDBCS,RDV.RDBCS) != 0))&&(DispMode!=DISP_OFF))
		{


			printf("MainMode_RDDisp 1 ='%d''%d''%d''%d''%d''%d''%d''%d'\n\n",(tLOUD != RDV.RDLOUD),(tMute != RDV.RDMute),(tRDOnOff != RDV.RDOnOff),(tRDStereo != RDV.RDStereo),(tRDMode != RDV.RDMode),(tRDFreq != RDV.RDFreq),(tRDTone != RDV.RDTone),(tRDVol != RDV.RDVol));
			
			//printf("MainMode_RDPrg='%s' '%s' '%d'\n",tRDPrg,RDV.RDPrg,(strcmp(tRDPrg,RDV.RDPrg) != 0));
			//printf("MainMode_RDBCS='%s' '%s' '%d'\n",tRDBCS,RDV.RDBCS,(strcmp(tRDBCS,RDV.RDBCS) != 0));	
			
//			ituWidgetSetVisible(mRadio_Mute, DISP_OFF);
//			ituWidgetSetVisible(mRadio_SpkOn, DISP_OFF);
			ituWidgetSetVisible(mRadio_LOUD, DISP_OFF);
			ituWidgetSetVisible(mRadio_PowerOff, DISP_OFF);
			ituWidgetSetVisible(mRadio_PowerOn, DISP_OFF);
			ituWidgetSetVisible(mRadio_Stereo, DISP_OFF);

			
			if ((RDV.RDPhone==1)||(RDV.RDPhone==2)||(RDV.RDPhone==3)||(RDV.RDPhone==4)||(RDV.RDPhone==5))
			{
//				printf("RDPhone!='%d''%d''%d''%d'\n",(RDV.RDPhoneNum[0]>>4)*10,RDV.RDPhoneNum[0]&0xF,(RDV.RDPhoneNum[1]>>4)*10,RDV.RDPhoneNum[1]&0xF);

				memset(tmp,0x0,sizeof(tmp));
				for (int i=0;i<6;i++)
				{
					if ((RDV.RDPhoneNum[i]>>4)>9)
						break;
					else
						sprintf(tmp,"%s%d",tmp,(RDV.RDPhoneNum[i]>>4));
					if ((RDV.RDPhoneNum[i]&0xF)>9)
						break;
					else
						sprintf(tmp,"%s%d",tmp,(RDV.RDPhoneNum[i]&0xF));
				}

//			sprintf(tmp,"%d%d%d%d%d%d%d%d%d%d%d%d",(RDV.RDPhoneNum[0]>>4),RDV.RDPhoneNum[0]&0xF,(RDV.RDPhoneNum[1]>>4),RDV.RDPhoneNum[1]&0xF,(RDV.RDPhoneNum[2]>>4),RDV.RDPhoneNum[2]&0xF,(RDV.RDPhoneNum[3]>>4),RDV.RDPhoneNum[3]&0xF,(RDV.RDPhoneNum[4]>>4),RDV.RDPhoneNum[4]&0xF,(RDV.RDPhoneNum[5]>>4),RDV.RDPhoneNum[5]&0xF);
				//ituTextSetString(mRadio_FreqTx, tmp);//?�화번호//chpark.24.09.06
				//ituWidgetSetVisible(mRadio_FreqTx, DispMode);
				ituTextSetString(mRD_PhoneNum, tmp);//?�화번호//chpark.24.10.15
				ituWidgetSetVisible(mRD_PhoneNum, DispMode);//chpark.24.10.15
				
				memcpy (tPhoneNum,RDV.RDPhoneNum,sizeof(RDV.RDPhoneNum));
				if (RDV.RDMute==1)
				{
//					ituWidgetSetVisible(mRadio_Mute, DispMode);
				}
				else
				{
//					ituWidgetSetVisible(mRadio_SpkOn, DispMode);
				}
			}
			else
			{
				if (RDV.RDOnOff==0)
				{
					ituWidgetSetVisible(mRadio_PowerOff, DispMode);
				}
				else
				{
					ituWidgetSetVisible(mRadio_PowerOn, DispMode);
				}
				switch (RDV.RDMode)
				{
					case 0://AM
					{
						ituTextSetString(mRadio_ModeTx, " AM ");
						CalRadioFreq(tmp,RDV.RDMode,RDV.RDFreq);
						ituTextSetString(mRadio_FreqTx, tmp);
						if (RDV.RDStereo==1)
						{
							ituWidgetSetVisible(mRadio_Stereo, DispMode);
						}
						if (tRDSelCh != RDV.RDSelCh)
						{
							MainMode_RD_SEL_CH_DISP(0,DispMode);
						}
						ituWidgetSetVisible(mRadio_FreqTx, DispMode);
						ituWidgetSetVisible(mRadio_ModeTx, DispMode);
						ituWidgetSetVisible(mRD_PhoneNum, !DispMode); //chpark.24.10.18
					}
					break;
					case 1://FM
					{
						ituTextSetString(mRadio_ModeTx, " FM ");
						CalRadioFreq(tmp,RDV.RDMode,RDV.RDFreq);
						
						ituTextSetString(mRadio_FreqTx, tmp);
						if (RDV.RDStereo==1)
						{
							ituWidgetSetVisible(mRadio_Stereo, DispMode);
						}
						if (tRDSelCh != RDV.RDSelCh)
						{
							MainMode_RD_SEL_CH_DISP(0,DispMode);
						}
						ituWidgetSetVisible(mRadio_FreqTx, DispMode);
						ituWidgetSetVisible(mRadio_ModeTx, DispMode);
						ituWidgetSetVisible(mRD_PhoneNum, !DispMode); //chpark.24.10.18
					}
					break;
					case 2://BT
					{
						ituWidgetSetVisible(mRadio_FreqTx, DISP_OFF);
						ituWidgetSetVisible(mRadio_ModeTx, DISP_OFF);
						ituWidgetSetVisible(mRD_PhoneNum, !DispMode); //chpark.24.10.18
					}
					break;
					case 3://DAB1
					{
						if (Main_Disp_Mode==MAIN_MODE_DAB1)
						{
							ituTextSetString(mRadio_ModeTx, " DAB ");
							sprintf(tmp, " %d.%03d MHz ", DAB_Freq[RDV.DABFreq]/1000,DAB_Freq[RDV.DABFreq]%1000);
							ituTextSetString(mRadio_FreqTx, tmp);
							//ituWidgetSetVisible(mRD_DAB2, DispMode);
							ituWidgetSetVisible(mRD_DAB2, DISP_OFF);//chpark.24.10.02
							ituTextSetString(mRD_PrgTx, RDV.RDPrg);
							ituTextSetString(mRD_BCSTx, RDV.RDBCS);
							if (tDABSelCh != RDV.DABSelCh)
							{
								MainMode_RD_SEL_CH_DISP(1,DispMode);
							}
							ituWidgetSetVisible(mRadio_FreqTx, DispMode); //chpark.06.03
						    ituWidgetSetVisible(mRadio_ModeTx, DispMode); //chpark.06.03
							ituWidgetSetVisible(mRD_DAB2_MODE, !DispMode);//chpark.06.03
						    ituWidgetSetVisible(mRD_DAB2_CAT, !DispMode);//chpark.06.03
							ituWidgetSetVisible(mRD_PhoneNum, !DispMode); //chpark.24.10.18
						}
						else if ((Main_Disp_Mode==MAIN_MODE_DAB2)||(Main_Disp_Mode==MAIN_MODE_DAB3))
						{
							//ituTextSetString(mRadio_ModeTx, " DAB "); //chpark.06.03
//							sprintf(tmp, "%s",&DAB2_PTY_No[RDV.PTYNo-1][0]);
							//ituTextSetString(mRadio_FreqTx, tmp);		//chpark.06.03
							ituWidgetSetVisible(mRadio_FreqTx, !DispMode); //chpark.06.03
						    ituWidgetSetVisible(mRadio_ModeTx, !DispMode); //chpark.06.03
							ituWidgetSetVisible(mRD_DAB2_MODE, DispMode);  //chpark.06.03
							ituWidgetSetVisible(mRadio_PowerOn, DISP_OFF); //chpark.24.09.06
							ituWidgetSetVisible(mRadio_PowerOff, DISP_OFF);//chpark.24.09.06
							ituWidgetSetVisible(mRD_DAB2, DispMode);//chpark.24.10.02
							ituWidgetSetVisible(mRD_PhoneNum, !DispMode); //chpark.24.10.18
							
							if(g_bDAB2_DataMode)//�ʱ�ȭ//chpark.09.09
							{
								printf("g_bDAB2_DataMode=%d\n\n",g_bDAB2_DataMode);
						    	ituWidgetSetVisible(mRD_DAB2_CAT, DispMode);//chpark.06.03
								DAB2_SetCategory("CATEGORY");
							}


						}
						if (RDV.RDStereo==1)
						{
							ituWidgetSetVisible(mRadio_Stereo, DispMode);
						}
					
					}
					break;
				}
				if ((Main_Disp_Mode!=MAIN_MODE_DAB2)&&(Main_Disp_Mode!=MAIN_MODE_DAB3)&&(Main_Disp_Mode!=MAIN_MODE_PHONE))
				{
					if (RDV.RDMute==1)
					{
//						ituWidgetSetVisible(mRadio_Mute, DispMode);
					}
					else
					{
//						ituWidgetSetVisible(mRadio_SpkOn, DispMode);
					}
					if (RDV.RDLOUD==1)
						ituWidgetSetVisible(mRadio_LOUD, DispMode);
				}
			}


		

			tLOUD = RDV.RDLOUD;
			tMute = RDV.RDMute;
			tRDOnOff = RDV.RDOnOff;
			tRDStereo = RDV.RDStereo;
			tRDMode = RDV.RDMode;
			tRDFreq = RDV.RDFreq;
			tRDStep = RDV.RDStep;
			tRDTone = RDV.RDTone;
			tRDVol = RDV.RDVol;
			tRDPhone=RDV.RDPhone;
			tRDSelCh = RDV.RDSelCh;
			tDABSelCh = RDV.DABSelCh;
			tDABFreq = RDV.DABFreq;
			tRadio_Region = Radio_Region;
			memset (tRDPrg,0,sizeof(tRDPrg));
			memset (tRDBCS,0,sizeof(tRDBCS));
			memset (tPhoneNum,0,sizeof(tPhoneNum));
			memcpy (tRDPrg,RDV.RDPrg,sizeof(RDV.RDPrg));
			memcpy (tRDBCS,RDV.RDBCS,sizeof(RDV.RDBCS));
			memcpy (tPhoneNum,RDV.RDPhoneNum,sizeof(RDV.RDPhoneNum));
	//		tPhone_Time=Phone_Time;
			if ((Main_Disp_Mode==MAIN_MODE_DAB2)||(Main_Disp_Mode==MAIN_MODE_DAB3))
			{
				MainMode_RD_BarDisp( ReDisp,RDV.RDVol,RDV.RDTone, DISP_OFF);
			}
			else
			{
				MainMode_RD_BarDisp( ReDisp,RDV.RDVol,RDV.RDTone, DispMode);
			}
		}
	}
	return true;
}


bool MainLayer_BG_BL_SUB(uint8_t MainMode)
{
	int i;
	
	printf("==================   MainLayer_BG_BL_SUB:%d\n",MainMode);
	switch (MainMode)
	{
	    case MAIN_MODE_AIRCON:
		{
			if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
			{
				printf("==================   AC_Blue_BG BG_BL_SUB\n");
				ituWidgetSetVisible(AC_Black_BG, DISP_OFF);
				ituWidgetSetVisible(AC_Blue_BG, DISP_ON);
			}
			else
			{
				printf("==================   AC_Black_BG BG_BL_SUB\n");
				ituWidgetSetVisible(AC_Blue_BG, DISP_OFF);
				ituWidgetSetVisible(AC_Black_BG, DISP_ON);
			}
		}
		break;
	    case MAIN_MODE_RADIO:
	    case MAIN_MODE_DAB1:
		{
			if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
			{
				ituWidgetSetVisible(Radio_Black_BG, DISP_OFF);
				ituWidgetSetVisible(Radio_Blue_BG, DISP_ON);
			}
			else
			{
				ituWidgetSetVisible(Radio_Blue_BG, DISP_OFF);
				ituWidgetSetVisible(Radio_Black_BG, DISP_ON);
			}
		}
		break;
	    case MAIN_MODE_DAB2:
	    case MAIN_MODE_DAB3:
		{
			if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
			{
				ituWidgetSetVisible(mRD_DAB2_BG, DISP_OFF);
				ituWidgetSetVisible(mRD_DAB2_Blue_BG, DISP_ON);
			}
			else
			{
				ituWidgetSetVisible(mRD_DAB2_Blue_BG, DISP_OFF);
				ituWidgetSetVisible(mRD_DAB2_BG, DISP_ON);
			}
		}
		break;
	    case MAIN_MODE_BLUETOOTH:
		{
			if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
			{
				ituWidgetSetVisible(mRD_Bluetooth_BG, DISP_OFF);
				ituWidgetSetVisible(mRD_Bluetooth_Blue_BG, DISP_ON);
			}
			else
			{
				ituWidgetSetVisible(mRD_Bluetooth_Blue_BG, DISP_OFF);
				ituWidgetSetVisible(mRD_Bluetooth_BG, DISP_ON);
			}
		}
		break;
	    case MAIN_MODE_PHONE:
		{
			if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
			{
				ituWidgetSetVisible(mRD_Phone_BG, DISP_OFF);
				ituWidgetSetVisible(mRD_Phone_Blue_BG, DISP_ON);
			}
			else
			{
				ituWidgetSetVisible(mRD_Phone_Blue_BG, DISP_OFF);
				ituWidgetSetVisible(mRD_Phone_BG, DISP_ON);
			}
		}
		break;
    }
    return true;
}

bool EnterMainMode(uint8_t MainMode)
{
	int i;
	
	//printf("==================   EnterMainMode:%d==========================================\n",MainMode);
	switch (MainMode)
	{
		case MAIN_MODE_NORMAL:
		{
			MainMode_ACDisp(0,DISP_OFF);
			MainMode_RDDisp(0,DISP_OFF);
			Main_Select_Index = MAIN_SEL_MENU;
		}
		break;
	    case MAIN_MODE_AIRCON:
		{
			//MainMode_AC_Select_Index = 0xFF;
			//MainMode_AC_Select_Index = AC_SEL_AIRMODE;
			MainMode_ACDisp(0,DISP_OFF);
			if ((ACV.AirMode==0)||(ACV.AirMode==1))
				ituWidgetSetVisible(mAC_Bt[0], DISP_ON);
			
			for (i=1;i<10;i++)
			{
				ituWidgetSetVisible(mAC_Bt[i], DISP_ON);
			}
			ituWidgetSetVisible(AC_BG, DISP_ON);
			
			if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
			{
				printf("==================   AC_Blue_BG DISP_ON\n");
				ituWidgetSetVisible(AC_Blue_BG, DISP_ON);
			}
			else
			{
				printf("==================   AC_Black_BG DISP_ON\n");
				ituWidgetSetVisible(AC_Black_BG, DISP_ON);
			}
			
			MainMode_RDDisp(0,DISP_OFF);
			switch (MainMode_AC_Select_Index)
			{
				case RD_SEL_AIRCON:
				case RD_SEL_RADIO:
				case RD_SEL_SPK:
				case RD_SEL_HOURMETER:
				case RD_SEL_BAR:
				case RD_SEL_TIMEDATE:
				case RD_SEL_POWER:
				case RD_SEL_CAMERA:
				case RD_SEL_MENU:
					MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				break;		
				default:
					MainMode_AC_Sel(0,MainMode_AC_Select_Index,DISP_ON);
				break;
			}
		}
		break;
	    case MAIN_MODE_RADIO:
	    case MAIN_MODE_DAB1:
	    case MAIN_MODE_DAB2:
	    case MAIN_MODE_DAB3:
		{
			//MainMode_RD_Select_Index = RD_SEL_MODE;  //chpark.24.10.07
			//MainMode_DAB_Select_Index = RD_SEL_MODE; //chpark.24.10.07
			//MainMode_DAB2_Select_Index = RD_SEL_AUTO;//chpark.24.10.07
			//================================
			//chpark.11.15
			//printf("==================  (1) EnterMainMode:%d\n",g_bDAB2_DataMode);
			if(!g_bDAB2_DataMode)
			{
				g_bDAB2_DataMode = false;
			}
			//================================
			//printf("================== (2) EnterMainMode:%d\n",g_bDAB2_DataMode);
			MainMode_RDDisp(0,DISP_OFF);
			//printf("================== (3) EnterMainMode:%d\n",g_bDAB2_DataMode);
			if (MainMode==MAIN_MODE_RADIO)
			{
				for (i=0;i<mRD_BT_Radio;i++)	// SCAN off
				{
					ituWidgetSetVisible(mRadio_Bt[i], DISP_ON);
				}
			}
			else if (MainMode==MAIN_MODE_DAB1)
			{
				for (i=0;i<mRD_BT_DAB1;i++)
				{
					ituWidgetSetVisible(mRadio_Bt[i], DISP_ON);
				}
			}
			else if ((MainMode==MAIN_MODE_DAB2)||(MainMode==MAIN_MODE_DAB3))
			{
				if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
				{
					for (i=DAB2_SEL_1C;i<DAB2_SEL_8C;i++)
					{
						ituWidgetSetVisible(mRadio_Bt[i], DISP_ON);
					}
				}
				else
				{
					for (i=DAB2_SEL_1C;i<DAB2_SEL_7C;i++)
					{
						ituWidgetSetVisible(mRadio_Bt[i], DISP_ON);
					}
				}
				ituWidgetSetVisible(mRadio_Bt[RD_SEL_AUTO], DISP_ON);
				ituWidgetSetVisible(mRadio_Bt[RD_SEL_MODE], DISP_ON);
				ituWidgetSetVisible(mRadio_Bt[RD_SEL_PWR], DISP_ON);
			}
			ituWidgetSetVisible(RD_BG, DISP_ON);
			if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
			{
				ituWidgetSetVisible(Radio_Blue_BG, DISP_ON);
			}
			MainMode_ACDisp(0,DISP_OFF);	
		}
		break;
	    case MAIN_MODE_BLUETOOTH:
		{
			MainMode_BT_Select_Index = RD_SEL_MODE;
			MainMode_RDDisp(0,DISP_OFF);
			ituWidgetSetVisible(RD_BG, DISP_ON);
			if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
			{
				ituWidgetSetVisible(Radio_Blue_BG, DISP_ON);
			}
			MainMode_ACDisp(0,DISP_OFF);
			
			
			printf("MainMode_BT_Select_Index= '0x%X' '%d'\n",MainMode_BT_Select_Index,MainMode_BT_Select_Index);
			
			
			switch (MainMode_BT_Select_Index)
			{
				case RD_SEL_AIRCON:
				case RD_SEL_RADIO:
				case RD_SEL_SPK:
				case RD_SEL_HOURMETER:
				case RD_SEL_BAR:
				case RD_SEL_TIMEDATE:
				case RD_SEL_POWER:
				case RD_SEL_CAMERA:
				case RD_SEL_MENU:
					MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				break;
				default:
					MainMode_RD_Sel(1,MainMode_BT_Select_Index,DISP_ON);
				break;
			}
		}
		break;
	    case MAIN_MODE_PHONE:
		{
			MainMode_Phone_Select_Index = PH_SEL_R31;
			MainMode_RDDisp(0,DISP_OFF);
			ituWidgetSetVisible(RD_BG, DISP_ON);
			if ((BG_Color==BG_BLUE)&&(Main_DayNight==0))
			{
				ituWidgetSetVisible(Radio_Blue_BG, DISP_ON);
			}
			MainMode_ACDisp(0,DISP_OFF);
			switch (MainMode_Phone_Select_Index)
			{
				case RD_SEL_AIRCON:
				case RD_SEL_RADIO:
				case RD_SEL_SPK:
				case RD_SEL_HOURMETER:
				case RD_SEL_BAR:
				case RD_SEL_TIMEDATE:
				case RD_SEL_POWER:
				case RD_SEL_CAMERA:
				case RD_SEL_MENU:
					MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				break;
				default:
					MainMode_RD_Sel(1,MainMode_Phone_Select_Index,DISP_ON);
				break;
			}
		}
		break;
#if 0		
	    case MAIN_MODE_CAMERA:
		{
		}
#endif		
		break;
    }
    return true;
}



bool MainLayer_SelBar(uint8_t IncDec,DISP_MODE DispMode)
{
	if (Main_Disp_Mode == MAIN_MODE_NORMAL)
	{
		if (DispMode==DISP_ON)
		{
			if (IncDec==1)	//0:NOP , 1: INC , 2:DEC
			{
				IncrementData(Main_Select_Index,MAIN_SEL_TIMEDATE,MAIN_SEL_ATT)
			}
			else if (IncDec==2)
			{
				DecrementData(Main_Select_Index,MAIN_SEL_ATT,MAIN_SEL_TIMEDATE)
			}
		}
		MainLayer_ButtonSel(Main_Select_Index,DispMode);
	}
	return true;
}
bool MainOnJD(ITUWidget* widget, char* param)
{
	int j,i=0,tIndex=0;

	printf("MainOnJD Start=%s\n",param);
	
	
	SetWidget(AC_BG,"AC_BG");
	SetWidget(AC_Black_BG,"AC_Black_BG");
	SetWidget(AC_Blue_BG,"AC_Blue_BG");

	SetWidget(mRadio_AUTO,"mRadio_AUTO");
	SetWidget(mRadio_TUNE,"mRadio_TUNE");
	SetWidget(mRadio_SCAN,"mRadio_SCAN");
	
	
	for (i=0;i<6;i++)
	{
		sprintf(widgetName, "mAC_WindDir%d",i);
		SetWidget(mAC_WindDirD[i],widgetName);
	}
	SetWidget(mAC_Wind_Off,"mAC_Wind_Off");

	SetWidget(mAC_Auto_Off,"mAC_Auto_Off");
	SetWidget(mAC_Auto_On,"mAC_Auto_On");
	SetWidget(mAC_Off,"mAC_Off");
	SetWidget(mAC_On,"mAC_On");
	SetWidget(mAC_InAir,"mAC_InAir");
	SetWidget(mAC_OutAir,"mAC_OutAir");
	
	
	for (i=0;i<7;i++)
	{
		sprintf(widgetName, "mAC_WindLv%d",i);
		SetWidget(mAC_WindLvD[i],widgetName);
	}
	SetWidget(mAC_TempTx,"mAC_TempTx");
	for (i=0;i<10;i++)
	{
		sprintf(widgetName, "mAC_MODE%d_SEL",i);
		SetWidget(mAC_MODE_SEL[i],widgetName);
		sprintf(widgetName, "mAC_Bt%d",i);
		SetWidget(mAC_Bt[i],widgetName);
	}

	SetWidget(RD_BG,"RD_BG");
	SetWidget(DAB2_BG,"DAB2_BG");
	SetWidget(BT_BG,"BT_BG");
	SetWidget(PH_BG,"PH_BG");
	

	SetWidget(Radio_Black_BG,"Radio_Black_BG");
//	SetWidget(mRadio_Mute,"mRadio_Mute");
	
//	SetWidget(mRadio_SpkOn,"mRadio_SpkOn");
	SetWidget(mRadio_LOUD,"mRadio_LOUD");
	SetWidget(mRadio_PowerOff,"mRadio_PowerOff");
	SetWidget(mRadio_PowerOn,"mRadio_PowerOn");
	SetWidget(mRadio_ModeTx,"mRadio_ModeTx");
	SetWidget(mRadio_Stereo,"mRadio_Stereo");
	SetWidget(mRadio_FreqTx,"mRadio_FreqTx");

	SetWidget(mRadio_ModeTx,"mRadio_ModeTx");
	SetWidget(mRadio_ModeTx,"mRadio_ModeTx");
	
	SetWidget(mRD_DAB2_MODE,"mRD_DAB2_MODE");   //chpark.06.03
	SetWidget(mRD_DAB2_CAT,"mRD_DAB2_CAT");//chpark.06.03

	for (i=0;i<mRD_BT_CNT_ALL;i++)
	{
		sprintf(widgetName, "mRD_MODE%02d_SEL",i+1);
		SetWidget(mRD_MODE_SEL[i],widgetName);
		sprintf(widgetName, "mRadio_Bt%02d",i+1);
		SetWidget(mRadio_Bt[i],widgetName);	
	}
	
	
	for (i=0;i<10;i++)
	{
		sprintf(widgetName, "mRD_Vol_Val%d",i);
		SetWidget(mRD_Vol_ValD[i],widgetName);
		sprintf(widgetName, "mRD_Tone_Val%d",i);
		SetWidget(mRD_Tone_ValD[i],widgetName);	
	}
	for (i=0;i<8;i++)
	{
		sprintf(widgetName, "mRD_FreqCh%d",i+1);
		SetWidget(mRD_FreqCh[i],widgetName);
	}
	SetWidget(mRD_BCSTx,"mRD_BCSTx");
	SetWidget(mRD_PrgTx,"mRD_PrgTx");
	
	SetWidget(mRD_DAB2_BG,"mRD_DAB2_BG");
	SetWidget(mRD_Bluetooth_BG,"mRD_Bluetooth_BG");
	SetWidget(mRD_Phone_BG,"mRD_Phone_BG");

	for (i=0;i<6;i++)
	{
//		sprintf(widgetName, "mRD_DAB2_PTY%d",i+1);
//		SetWidget(mRD_DAB2_PTY[i],widgetName);
		sprintf(widgetName, "mRD_DAB2_ON%d",i+1);
		SetWidget(mRD_DAB2_OND[i],widgetName);
	}
	SetWidget(mRD_DAB1,"mRD_DAB1");
	SetWidget(mRD_DAB2,"mRD_DAB2");
	
	for (i=0;i<4;i++)
	{
		sprintf(widgetName, "mRD_BT_Pairing%d",i+1);
		SetWidget(mRD_BT_Pairing[i],widgetName);
	}


	SetWidget(Radio_Blue_BG,"Radio_Blue_BG");
	SetWidget(mRD_DAB2_Blue_BG,"mRD_DAB2_Blue_BG");
	SetWidget(mRD_Bluetooth_Blue_BG,"mRD_Bluetooth_Blue_BG");
	SetWidget(mRD_Phone_Blue_BG,"mRD_Phone_Blue_BG");
	
	SetWidget(mRD_PHONE1_HD,"mRD_PHONE1_HD");
	SetWidget(mRD_PHONE2_HD,"mRD_PHONE2_HD");
	SetWidget(mRD_PHONE3_HD,"mRD_PHONE3_HD");

	SetWidget(mRD_PhoneNum,"mRD_PhoneNum");//chpark.24.10.15

	if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
	{
    	//chpark.12.05
    	MainOnDAB2(true);
		MainOnDAB2_H(false);
	}
	else
	{
		//chpark.12.05
		MainOnDAB2(false);
		MainOnDAB2_H(true);
	}
    		
	printf("MainOnJD End=%s\n",param);
	
	return true;
}
bool JogDialRot_MAIN_NORMAL(char* param)
{
	switch (param[0])
	{
		case JOG_DIAL_ROT_LEFT:
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(Main_Select_Index)
				{
					case MAIN_SEL_ATT:
					{
						MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
					}
					break;
					case MAIN_SEL_AIRCON:
					{
						MainLayer_ButtonSel(MAIN_SEL_MENU, DISP_ON);
					}
					break;
					case MAIN_SEL_RADIO:
					{
						MainLayer_ButtonSel(MAIN_SEL_AIRCON, DISP_ON);
					}
					break;
					case MAIN_SEL_SPK:
					{
						MainLayer_ButtonSel(MAIN_SEL_RADIO, DISP_ON);
					}
					break;
					case MAIN_SEL_HOURMETER:
					{
						if(AUI1_0B_3210b==3) // Off
							MainLayer_ButtonSel(MAIN_SEL_ATT, DISP_ON);
						else
							MainLayer_ButtonSel(MAIN_SEL_POWER, DISP_ON);
					}
					break;
					case MAIN_SEL_BAR:
					{
						MainLayer_ButtonSel(MAIN_SEL_CAMERA, DISP_ON);
					}
					break;
					case MAIN_SEL_TIMEDATE:
					{
						MainLayer_ButtonSel(MAIN_SEL_TIMEDATE, DISP_ON);
					}
					break;
					case MAIN_SEL_POWER:
					{
						MainLayer_ButtonSel(MAIN_SEL_ATT, DISP_ON);
					}
					break;
					case MAIN_SEL_CAMERA:
					{
						MainLayer_ButtonSel(MAIN_SEL_SPK, DISP_ON);
					}
					break;
					case MAIN_SEL_MENU:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
				}
			}
			else
			{
				switch(Main_Select_Index)
				{
					case MAIN_SEL_ATT:
					{
						MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
					}
					break;
					case MAIN_SEL_AIRCON:
					{
						MainLayer_ButtonSel(MAIN_SEL_MENU, DISP_ON);
					}
					break;
					case MAIN_SEL_RADIO:
					{
						MainLayer_ButtonSel(MAIN_SEL_AIRCON, DISP_ON);
					}
					break;
					case MAIN_SEL_SPK:
					{
						MainLayer_ButtonSel(MAIN_SEL_RADIO, DISP_ON);
					}
					break;
					case MAIN_SEL_HOURMETER:
					{
						MainLayer_ButtonSel(MAIN_SEL_ATT, DISP_ON);
					}
					break;
					case MAIN_SEL_BAR:
					{
						if(AUI1_0B_3210b==3) // Off
							MainLayer_ButtonSel(MAIN_SEL_CAMERA, DISP_ON);
						else
							MainLayer_ButtonSel(MAIN_SEL_POWER, DISP_ON);
					}
					break;
					case MAIN_SEL_TIMEDATE:
					{
						MainLayer_ButtonSel(MAIN_SEL_TIMEDATE, DISP_ON);
					}
					break;
					case MAIN_SEL_POWER:
					{
						MainLayer_ButtonSel(MAIN_SEL_CAMERA, DISP_ON);
					}
					break;
					case MAIN_SEL_CAMERA:
					{
						MainLayer_ButtonSel(MAIN_SEL_SPK, DISP_ON);
					}
					break;
					case MAIN_SEL_MENU:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
				}
			}
		}
		break;
		case JOG_DIAL_ROT_RIGHT:
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(Main_Select_Index)
				{
					case MAIN_SEL_ATT:
					{
						if(AUI1_0B_3210b==3) // Off
							MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
						else
							MainLayer_ButtonSel(MAIN_SEL_POWER, DISP_ON);
					}
					break;
					case MAIN_SEL_AIRCON:
					{
						MainLayer_ButtonSel(MAIN_SEL_RADIO, DISP_ON);
					}
					break;
					case MAIN_SEL_RADIO:
					{
						MainLayer_ButtonSel(MAIN_SEL_SPK, DISP_ON);
					}
					break;
					case MAIN_SEL_SPK:
					{
						MainLayer_ButtonSel(MAIN_SEL_CAMERA, DISP_ON);
					}
					break;
					case MAIN_SEL_HOURMETER:
					{
						MainLayer_ButtonSel(MAIN_SEL_MENU, DISP_ON);
					}
					break;
					case MAIN_SEL_BAR:
					{
						MainLayer_ButtonSel(MAIN_SEL_ATT, DISP_ON);
					}
					break;
					case MAIN_SEL_TIMEDATE:
					{
						MainLayer_ButtonSel(MAIN_SEL_TIMEDATE, DISP_ON);
					}
					break;
					case MAIN_SEL_POWER:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
					case MAIN_SEL_CAMERA:
					{
						MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
					}
					break;
					case MAIN_SEL_MENU:
					{
						MainLayer_ButtonSel(MAIN_SEL_AIRCON, DISP_ON);
					}
					break;
				}
			}
			else
			{
				switch(Main_Select_Index)
				{
					case MAIN_SEL_ATT:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
					case MAIN_SEL_AIRCON:
					{
						MainLayer_ButtonSel(MAIN_SEL_RADIO, DISP_ON);
					}
					break;
					case MAIN_SEL_RADIO:
					{
						MainLayer_ButtonSel(MAIN_SEL_SPK, DISP_ON);
					}
					break;
					case MAIN_SEL_SPK:
					{
						MainLayer_ButtonSel(MAIN_SEL_CAMERA, DISP_ON);
					}
					break;
					case MAIN_SEL_HOURMETER:
					{
						MainLayer_ButtonSel(MAIN_SEL_MENU, DISP_ON);
					}
					break;
					case MAIN_SEL_BAR:
					{
						MainLayer_ButtonSel(MAIN_SEL_ATT, DISP_ON);
					}
					break;
					case MAIN_SEL_TIMEDATE:
					{
						MainLayer_ButtonSel(MAIN_SEL_TIMEDATE, DISP_ON);
					}
					break;
					case MAIN_SEL_POWER:
					{
						MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
					}
					break;
					case MAIN_SEL_CAMERA:
					{
						if(AUI1_0B_3210b==3) // Off
							MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
						else
							MainLayer_ButtonSel(MAIN_SEL_POWER, DISP_ON);
					}
					break;
					case MAIN_SEL_MENU:
					{
						MainLayer_ButtonSel(MAIN_SEL_AIRCON, DISP_ON);
					}
					break;
				}
			}
		}
		break;
	}	
	return true;
}



				
				
uint32_t KEY_ENTER_HM_CNT = 0;
static uint32_t KEY_ENTER_ATT_CNT = 0;

bool JogDialKey_LongEnter(void)
{
	char JD_Buf[10] = {0,};	
	
//	printf("JogDialKey_LongEnter '%d' '%d' '%d' '%d'\n",Main_Disp_Mode,RD_SEL_TUNE_LR_FLAG,RD_SEL_TONE_DU_FLAG,RD_SEL_VOL_DU_FLAG);
	if ((Main_Select_Index==MAIN_SEL_HOURMETER)&&(KEY_ENTER_HM_CNT!=0))
	{
		if ((KEY_ENTER_HM_CNT+TIME_CNT_3SEC)<gMainTimerCnt_10m)
		{
			printf("KEY_ENTER_HM_CNT=%d\n",KEY_ENTER_HM_CNT);
			JD_Buf[0] = 1;
			JD_Buf[1] = 1;
			Main_HMeterBt_Func(NULL, JD_Buf);
			KEY_ENTER_HM_CNT = 0xFFFFF000;
		}
	}
	if ((Main_Select_Index==MAIN_SEL_ATT)&&(KEY_ENTER_ATT_CNT!=0))
	{
		if ((KEY_ENTER_ATT_CNT+TIME_CNT_1SEC)<gMainTimerCnt_10m)
		{
			//Goto Aux1
			printf("Goto Aux1 Set=%d\n",KEY_ENTER_ATT_CNT);
			JD_Buf[0] = 1;
			JD_Buf[1] = 1;
			Main_ATTBt_Func(NULL, JD_Buf);
			KEY_ENTER_ATT_CNT = 0xFFFFF000;
		}
	}
	
	switch (Main_Disp_Mode)
	{
		case MAIN_MODE_NORMAL:
		{
		}
		break;
		case MAIN_MODE_AIRCON:
		{
			if (AC_SEL_DIR_LR_FLAG)
			{
				MainMode_AC_Sel(0,AC_SEL_DIR_L,gOnFlag_500ms);
			}
			else if(AC_SEL_TEMP_LR_FLAG)
			{
				MainMode_AC_Sel(0,AC_SEL_TEMP_L,gOnFlag_500ms);
			}
			else if(AC_SEL_LEVEL_LR_FLAG)
			{
				MainMode_AC_Sel(0,AC_SEL_LEVEL_L,gOnFlag_500ms);
			}
		}
		break;
		case MAIN_MODE_RADIO:
		case MAIN_MODE_DAB2:
		case MAIN_MODE_DAB3:
		case MAIN_MODE_BLUETOOTH:
		case MAIN_MODE_PHONE:
		case MAIN_MODE_DAB1:
		{
			if (RD_SEL_TUNE_LR_FLAG)
			{
				MainMode_RD_Sel(0,RD_SEL_TUNE_L,gOnFlag_500ms);
			}
			else if(RD_SEL_TONE_DU_FLAG)
			{
				MainMode_RD_Sel(0,RD_SEL_TONE_D,gOnFlag_500ms);
			}
			else if(RD_SEL_VOL_DU_FLAG)
			{
				MainMode_RD_Sel(0,RD_SEL_VOL_D,gOnFlag_500ms);
			}
		}
		break;
	}
	return true;
}

bool JogDialKey_MAIN_NORMAL(char* param)
{
	char JD_Buf[10] = {0,};
	uint8_t KeyData = 0;
	KeyData = param[0];
	
	printf("JogDialKey_MAIN_NORMAL '%d' '%d' \n",KeyData,Main_Select_Index);
	switch (KeyData)
	{
		case JOG_DIAL_KEY_LEFT:				//c3Byte0bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(Main_Select_Index)
				{
					case MAIN_SEL_ATT:
					break;
					case MAIN_SEL_AIRCON:
					{
						MainLayer_ButtonSel(MAIN_SEL_MENU, DISP_ON);
					}
					break;
					case MAIN_SEL_RADIO:
					{
						MainLayer_ButtonSel(MAIN_SEL_AIRCON, DISP_ON);
					}
					break;
					case MAIN_SEL_SPK:
					{
						MainLayer_ButtonSel(MAIN_SEL_RADIO, DISP_ON);
					}
					break;
					case MAIN_SEL_HOURMETER:
					{
						if(AUI1_0B_3210b==3) // Off
							MainLayer_ButtonSel(MAIN_SEL_CAMERA, DISP_ON);
						else
							MainLayer_ButtonSel(MAIN_SEL_POWER, DISP_ON);
					}
					break;
					case MAIN_SEL_BAR:
					break;
					case MAIN_SEL_TIMEDATE:
					break;
					case MAIN_SEL_POWER:
					{
						MainLayer_ButtonSel(MAIN_SEL_CAMERA, DISP_ON);
					}
					break;
					case MAIN_SEL_CAMERA:
					{
						MainLayer_ButtonSel(MAIN_SEL_SPK, DISP_ON);
					}
					break;
					case MAIN_SEL_MENU:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
				}
			}
			else
			{
				switch(Main_Select_Index)
				{
					case MAIN_SEL_ATT:
					{
						MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
					}
					break;
					case MAIN_SEL_AIRCON:
					{
						MainLayer_ButtonSel(MAIN_SEL_MENU, DISP_ON);
					}
					break;
					case MAIN_SEL_RADIO:
					{
						MainLayer_ButtonSel(MAIN_SEL_AIRCON, DISP_ON);
					}
					break;
					case MAIN_SEL_SPK:
					{
						MainLayer_ButtonSel(MAIN_SEL_RADIO, DISP_ON);
					}
					break;
					case MAIN_SEL_HOURMETER:
					break;
					case MAIN_SEL_BAR:
					{
						if(AUI1_0B_3210b==3) // Off
							MainLayer_ButtonSel(MAIN_SEL_ATT, DISP_ON);
						else
							MainLayer_ButtonSel(MAIN_SEL_POWER, DISP_ON);
					}
					break;
					case MAIN_SEL_TIMEDATE:
					break;
					case MAIN_SEL_POWER:
					{
						MainLayer_ButtonSel(MAIN_SEL_ATT, DISP_ON);
					}
					break;
					case MAIN_SEL_CAMERA:
					{
						MainLayer_ButtonSel(MAIN_SEL_SPK, DISP_ON);
					}
					break;
					case MAIN_SEL_MENU:
					{
						MainLayer_ButtonSel(MAIN_SEL_CAMERA, DISP_ON);
					}
					break;
				}
			}
		}
		break;
		case JOG_DIAL_KEY_RIGHT:			//c3Byte1bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(Main_Select_Index)
				{
					case MAIN_SEL_ATT:
					break;
					case MAIN_SEL_AIRCON:
					{
						MainLayer_ButtonSel(MAIN_SEL_RADIO, DISP_ON);
					}
					break;
					case MAIN_SEL_RADIO:
					{
						MainLayer_ButtonSel(MAIN_SEL_SPK, DISP_ON);
					}
					break;
					case MAIN_SEL_SPK:
					{
						MainLayer_ButtonSel(MAIN_SEL_CAMERA, DISP_ON);
					}
					break;
					case MAIN_SEL_HOURMETER:
					{
						MainLayer_ButtonSel(MAIN_SEL_MENU, DISP_ON);
					}
					break;
					case MAIN_SEL_BAR:
					break;
					case MAIN_SEL_TIMEDATE:
					break;
					case MAIN_SEL_POWER:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
					case MAIN_SEL_CAMERA:
					{
						if(AUI1_0B_3210b==3) // Off
							MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
						else
							MainLayer_ButtonSel(MAIN_SEL_POWER, DISP_ON);
					}
					break;
					case MAIN_SEL_MENU:
					{
						MainLayer_ButtonSel(MAIN_SEL_AIRCON, DISP_ON);
					}
					break;
				}
			}
			else
			{
				switch(Main_Select_Index)
				{
					case MAIN_SEL_ATT:
					{
						if(AUI1_0B_3210b==3) // Off
							MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
						else
							MainLayer_ButtonSel(MAIN_SEL_POWER, DISP_ON);						
					}
					break;
					case MAIN_SEL_AIRCON:
					{
						MainLayer_ButtonSel(MAIN_SEL_RADIO, DISP_ON);
					}
					break;
					case MAIN_SEL_RADIO:
					{
						MainLayer_ButtonSel(MAIN_SEL_SPK, DISP_ON);
					}
					break;
					case MAIN_SEL_SPK:
					{
						MainLayer_ButtonSel(MAIN_SEL_CAMERA, DISP_ON);
					}
					break;
					case MAIN_SEL_HOURMETER:
					break;
					case MAIN_SEL_BAR:
					{
						MainLayer_ButtonSel(MAIN_SEL_ATT, DISP_ON);
					}
					break;
					case MAIN_SEL_TIMEDATE:
					break;
					case MAIN_SEL_POWER:
					{
						MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
					}
					break;
					case MAIN_SEL_CAMERA:
					{
						MainLayer_ButtonSel(MAIN_SEL_MENU, DISP_ON);
					}
					break;
					case MAIN_SEL_MENU:
					{
						MainLayer_ButtonSel(MAIN_SEL_AIRCON, DISP_ON);
					}
					break;
				}
			}
		}
		break;
		case JOG_DIAL_KEY_UP:				//c3Byte2bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(Main_Select_Index)
				{
					case MAIN_SEL_ATT:
					{
						MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
					}
					break;
					case MAIN_SEL_AIRCON:
					break;
					case MAIN_SEL_RADIO:
					break;
					case MAIN_SEL_SPK:
					break;
					case MAIN_SEL_HOURMETER:
					{
						MainLayer_ButtonSel(MAIN_SEL_ATT, DISP_ON);
					}
					break;
					case MAIN_SEL_BAR:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
					case MAIN_SEL_TIMEDATE:
					break;
					case MAIN_SEL_POWER:
					{
						MainLayer_ButtonSel(MAIN_SEL_ATT, DISP_ON);
					}
					break;
					case MAIN_SEL_CAMERA:
					break;
					case MAIN_SEL_MENU:
					break;
				}
			}
			else
			{
				switch(Main_Select_Index)
				{
					case MAIN_SEL_ATT:
					{
						MainLayer_ButtonSel(MAIN_SEL_MENU, DISP_ON);
					}
					break;
					case MAIN_SEL_AIRCON:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
					case MAIN_SEL_RADIO:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
					case MAIN_SEL_SPK:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
					case MAIN_SEL_HOURMETER:
					{
						MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
					}
					break;
					case MAIN_SEL_BAR:
					{
						MainLayer_ButtonSel(MAIN_SEL_MENU, DISP_ON);
					}
					break;
					case MAIN_SEL_TIMEDATE:
					break;
					case MAIN_SEL_POWER:
					{
						MainLayer_ButtonSel(MAIN_SEL_MENU, DISP_ON);
					}
					break;
					case MAIN_SEL_CAMERA:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
					case MAIN_SEL_MENU:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
				}
			}
		}
		break;
		case JOG_DIAL_KEY_DOWN:				//c3Byte3bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(Main_Select_Index)
				{
					case MAIN_SEL_ATT:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
					case MAIN_SEL_AIRCON:
					break;
					case MAIN_SEL_RADIO:
					break;
					case MAIN_SEL_SPK:
					break;
					case MAIN_SEL_HOURMETER:
					{
						MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
					}
					break;
					case MAIN_SEL_BAR:
					{
						MainLayer_ButtonSel(MAIN_SEL_ATT, DISP_ON);
					}
					break;
					case MAIN_SEL_TIMEDATE:
					break;
					case MAIN_SEL_POWER:
					{
						MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
					}
					break;
					case MAIN_SEL_CAMERA:
					break;
					case MAIN_SEL_MENU:
					break;
				}
			}
			else
			{
				switch(Main_Select_Index)
				{
					case MAIN_SEL_ATT:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
					case MAIN_SEL_AIRCON:
					{
						MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
					}
					break;
					case MAIN_SEL_RADIO:
					{
						MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
					}
					break;
					case MAIN_SEL_SPK:
					{
						MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
					}
					break;
					case MAIN_SEL_HOURMETER:
					{
						MainLayer_ButtonSel(MAIN_SEL_RADIO, DISP_ON);
					}
					break;
					case MAIN_SEL_BAR:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
					case MAIN_SEL_TIMEDATE:
					break;
					case MAIN_SEL_POWER:
					{
						MainLayer_ButtonSel(MAIN_SEL_HOURMETER, DISP_ON);
					}
					break;
					case MAIN_SEL_CAMERA:
					{
						MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
					}
					break;
					case MAIN_SEL_MENU:
					{
						MainLayer_ButtonSel(MAIN_SEL_BAR, DISP_ON);
					}
					break;
				}
			}
		}
		break;
		case JOG_DIAL_KEY_ENTER_ON:
		{			
			switch(Main_Select_Index)
			{
				case MAIN_SEL_HOURMETER:
				{
					printf("JOG_DIAL_KEY_ENTER_ON=MAIN_SEL_HOURMETER '%d' \n",KEY_ENTER_HM_CNT);
					if (KEY_ENTER_HM_CNT == 0)
						KEY_ENTER_HM_CNT = gMainTimerCnt_10m;
				}
				break;
				case MAIN_SEL_ATT:
				{
					printf("JOG_DIAL_KEY_ENTER_ON=KEY_ENTER_ATT_CNT '%d' \n",KEY_ENTER_ATT_CNT);
					if (KEY_ENTER_ATT_CNT == 0)
						KEY_ENTER_ATT_CNT = gMainTimerCnt_10m;
				}
				break;
			}
		}
		break;
		case JOG_DIAL_KEY_ENTER_OFF:
//		case JOG_DIAL_KEY_NONE:
		{
			printf("JOG_DIAL_KEY_ENTER_OFF= '%d' 0x%x\n",Main_Select_Index,KEY_ENTER_HM_CNT);
			switch(Main_Select_Index)
			{

				case MAIN_SEL_ATT:
				{
					if (KEY_ENTER_ATT_CNT != 0xFFFFF000)
					{
						JD_Buf[1] = 0;
						if (AUX1CurrentOutMode==0)
							JD_Buf[0] = 2;
						else if (AUX1CurrentOutMode==1)
							JD_Buf[0] = 3;
						else if (AUX1CurrentOutMode==2)
							JD_Buf[0] = 1;
						Main_ATTBt_Func(NULL, JD_Buf);
					}
					KEY_ENTER_ATT_CNT = 0;
				}
				break;
				case MAIN_SEL_AIRCON:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_ACBt_Func(NULL, JD_Buf);
				}
				break;
				case MAIN_SEL_RADIO:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_RadioBt_Func("MAIN_SEL_RADIO", JD_Buf);
				}
				break;
				case MAIN_SEL_SPK:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_SPK_Bt_Func(NULL, JD_Buf);
				}
				break;
				case MAIN_SEL_HOURMETER:
				{
					if (KEY_ENTER_HM_CNT != 0xFFFFF000)
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_HMeterBt_Func(NULL, JD_Buf);
					}
					else
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_HMeterBt_Func(NULL, JD_Buf);
					}
					KEY_ENTER_HM_CNT = 0;
				}
				break;
				case MAIN_SEL_BAR:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_BarBt_Func(NULL, JD_Buf);
				}
				break;
				case MAIN_SEL_POWER:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_PwrBt_Func(NULL, JD_Buf);
				}
				break;
				case MAIN_SEL_CAMERA:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					MainCamera(NULL, JD_Buf);
				}
				break;
				case MAIN_SEL_MENU:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					MainMenu(NULL, JD_Buf);
				}
				break;
			}
		}
		break;
	}
	return true;
}

bool JogDialRot_MAIN_AC(char* param)
{
	char JD_Buf[10] = {0,};
	printf("JogDialRot_MAIN_AC= '0x%X' '%d' '%d'\n",param[0],MainMode_AC_Select_Index,AC_SEL_LEVEL_LR_FLAG);
	Media_RunTime_Cnt=gMainTimerCnt_10m;
	switch (param[0])
	{
		case JOG_DIAL_ROT_LEFT:
		{
	    	if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
		    {
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_OFF:
//				    	MainMode_AC_Sel(0,AC_SEL_TEMP_L,DISP_ON);
					break;
				    case AC_SEL_AIRMODE:
				   		MainMode_AC_Select_Index = AC_SEL_HOURMETER;//chpark.24.10.31
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);

				    break;
				    case AC_SEL_ONOFF:
				    	if ((ACV.AirMode==0)||(ACV.AirMode==1))
							MainMode_AC_Sel(0,AC_SEL_AIRMODE,DISP_ON);
						else
						{
				   			//MainMode_AC_Select_Index = AC_SEL_CAMERA;
							MainMode_AC_Select_Index = AC_SEL_HOURMETER;//chpark.24.10.31
							MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
						}
				    break;
				    case AC_SEL_DIR_L:
				    case AC_SEL_DIR_R:
				    	if (AC_SEL_DIR_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_DIR_L;
							AC_MAIN_UpBt2(NULL, JD_Buf);
							AC_SEL_DIR_LR_FLAG = 2;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_ONOFF,DISP_ON);
				    break;
				    case AC_SEL_TEMP_L:
				    case AC_SEL_TEMP_R:
				    	if (AC_SEL_TEMP_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_TEMP_L;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_TEMP_LR_FLAG = 2;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_LEVEL_L:
				    case AC_SEL_LEVEL_R:
				    	if (AC_SEL_LEVEL_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_LEVEL_L;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_LEVEL_LR_FLAG = 2;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_TEMP_L,DISP_ON);
				    break;
				    case AC_SEL_AUTO:
				   		MainMode_AC_Sel(0,AC_SEL_LEVEL_L,DISP_ON);
				    break;
				    case AC_SEL_PWR:
				   		MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
				    break;
				    
				    
				   	case AC_SEL_ATT:			// 72
				    	MainMode_AC_Select_Index = AC_SEL_BAR;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_AIRCON:		// 73,
				    	MainMode_AC_Select_Index = AC_SEL_MENU;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_RADIO:		// 4,
				    	MainMode_AC_Select_Index = AC_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_SPK:			// 75,
				    	MainMode_AC_Select_Index = AC_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_HOURMETER:	// 76,
				    	if(AUI1_0B_3210b==3) // Off
							MainMode_AC_Select_Index = AC_SEL_ATT;
						else
							MainMode_AC_Select_Index = AC_SEL_POWER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_BAR:			// 77,
				    	/*
						if(AUI1_0B_3210b==3) // Off
							MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
						else
						{
							MainMode_AC_Select_Index = AC_SEL_POWER;
							MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
						}
						*/
						MainMode_AC_Select_Index = AC_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_POWER:		// 79,
				    	MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
				    break;
				    case AC_SEL_CAMERA:		// 80,
				    	MainMode_AC_Select_Index = AC_SEL_SPK;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_MENU:		// 91
				    	//MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						//MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
						MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
				    break;
				}
		    }
		    else
		    {
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_OFF:
//				    	MainMode_AC_Sel(0,AC_SEL_TEMP_L,DISP_ON);
					break;
				    case AC_SEL_AIRMODE:
				   		MainMode_AC_Select_Index = AC_SEL_CAMERA;
						MainLayer_ButtonSel(AC_SEL_CAMERA,DISP_ON);
				    break;
				    case AC_SEL_ONOFF:
				    	if ((ACV.AirMode==0)||(ACV.AirMode==1))
							MainMode_AC_Sel(0,AC_SEL_AIRMODE,DISP_ON);
						else
						{
				   			MainMode_AC_Select_Index = AC_SEL_CAMERA;
							MainLayer_ButtonSel(AC_SEL_CAMERA,DISP_ON);
						}
				    break;
				    case AC_SEL_DIR_L:
				    case AC_SEL_DIR_R:
				    	if (AC_SEL_DIR_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_DIR_L;
							AC_MAIN_UpBt2(NULL, JD_Buf);
							AC_SEL_DIR_LR_FLAG = 2;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_ONOFF,DISP_ON);
				    break;
				    case AC_SEL_TEMP_L:
				    case AC_SEL_TEMP_R:
				    	if (AC_SEL_TEMP_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_TEMP_L;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_TEMP_LR_FLAG = 2;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_LEVEL_L:
				    case AC_SEL_LEVEL_R:
				    	if (AC_SEL_LEVEL_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_LEVEL_L;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_LEVEL_LR_FLAG = 2;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_TEMP_L,DISP_ON);
				    break;
				    case AC_SEL_AUTO:
				   		MainMode_AC_Sel(0,AC_SEL_LEVEL_L,DISP_ON);
				    break;
				    case AC_SEL_PWR:
				   		MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
				    break;
				    
				    
				   	case AC_SEL_ATT:			// 72
				    	MainMode_AC_Select_Index = AC_SEL_BAR;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_AIRCON:		// 73,
				    	MainMode_AC_Select_Index = AC_SEL_MENU;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_RADIO:		// 4,
				    	MainMode_AC_Select_Index = AC_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_SPK:			// 75,
				    	MainMode_AC_Select_Index = AC_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_HOURMETER:	// 76,
				    	MainMode_AC_Select_Index = AC_SEL_ATT;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_BAR:			// 77,
				    	if(AUI1_0B_3210b==3) // Off
							MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
						else
						{
							MainMode_AC_Select_Index = AC_SEL_POWER;
							MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
						}
				    break;
				    case AC_SEL_POWER:		// 79,
						MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
				    break;
				    case AC_SEL_CAMERA:		// 80,
				    	MainMode_AC_Select_Index = AC_SEL_SPK;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_MENU:		// 91
				    	MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				}
			}
		}
		break;

		case JOG_DIAL_ROT_RIGHT:
		{
	    	if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
		    {
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_OFF:
//				    	MainMode_AC_Sel(0,AC_SEL_TEMP_L,DISP_ON);
					break;
				    case AC_SEL_AIRMODE:
				    	MainMode_AC_Sel(0,AC_SEL_ONOFF,DISP_ON);
				    break;
				    case AC_SEL_ONOFF:
				    	MainMode_AC_Sel(0,AC_SEL_DIR_R,DISP_ON);
				    break;
				    case AC_SEL_DIR_L:
				    case AC_SEL_DIR_R:
				    	if (AC_SEL_DIR_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_DIR_R;
							AC_MAIN_UpBt2(NULL, JD_Buf);
							AC_SEL_DIR_LR_FLAG = 2;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_TEMP_R,DISP_ON);
				    break;
				    case AC_SEL_TEMP_L:
				    case AC_SEL_TEMP_R:
				    	if (AC_SEL_TEMP_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_TEMP_R;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_TEMP_LR_FLAG = 2;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_LEVEL_R,DISP_ON);
				    break;
				    case AC_SEL_LEVEL_L:
				    case AC_SEL_LEVEL_R:
				    	if (AC_SEL_LEVEL_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_LEVEL_R;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_LEVEL_LR_FLAG = 2;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
				    break;
				    case AC_SEL_AUTO:
				   		MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
				    break;
				    case AC_SEL_PWR:
				   		MainMode_AC_Select_Index = AC_SEL_MENU;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    
				    
				    case AC_SEL_ATT:
				    	if(AUI1_0B_3210b==3) // Off
							MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						else
							MainMode_AC_Select_Index = AC_SEL_POWER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_AIRCON:
				   		MainMode_AC_Select_Index = AC_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_RADIO:
				   		MainMode_AC_Select_Index = AC_SEL_SPK;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_SPK:
				   		MainMode_AC_Select_Index = AC_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_HOURMETER:
				    	if ((ACV.AirMode==0)||(ACV.AirMode==1))
							MainMode_AC_Sel(0,AC_SEL_AIRMODE,DISP_ON);
						else
						{
				   			MainMode_AC_Sel(0,AC_SEL_ONOFF,DISP_ON);
						}
				    break;
				    case AC_SEL_BAR:
				   		MainMode_AC_Select_Index = AC_SEL_ATT;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_TIMEDATE:
				    break;
				    case AC_SEL_POWER:
				   		MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_CAMERA:
				    	MainMode_AC_Select_Index = AC_SEL_BAR;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_MENU:
				   		MainMode_AC_Select_Index = AC_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				}
		    }
		    else
		    {
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_OFF:
//				    	MainMode_AC_Sel(0,AC_SEL_TEMP_L,DISP_ON);
					break;
				    case AC_SEL_AIRMODE:
				    	MainMode_AC_Sel(0,AC_SEL_ONOFF,DISP_ON);
				    break;
				    case AC_SEL_ONOFF:
				    	MainMode_AC_Sel(0,AC_SEL_DIR_R,DISP_ON);
				    break;
				    case AC_SEL_DIR_L:
				    case AC_SEL_DIR_R:
				    	if (AC_SEL_DIR_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_DIR_R;
							AC_MAIN_UpBt2(NULL, JD_Buf);
							AC_SEL_DIR_LR_FLAG = 2;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_TEMP_R,DISP_ON);
				    break;
				    case AC_SEL_TEMP_L:
				    case AC_SEL_TEMP_R:
				    	if (AC_SEL_TEMP_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_TEMP_R;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_TEMP_LR_FLAG = 2;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_LEVEL_R,DISP_ON);
				    break;
				    case AC_SEL_LEVEL_L:
				    case AC_SEL_LEVEL_R:
				    	if (AC_SEL_LEVEL_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_LEVEL_R;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_LEVEL_LR_FLAG = 2;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
				    break;
				    case AC_SEL_AUTO:
				   		MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
				    break;
				    case AC_SEL_PWR:
				    	if(AUI1_0B_3210b==3) // Off
							MainMode_AC_Select_Index = AC_SEL_BAR;
						else
							MainMode_AC_Select_Index = AC_SEL_POWER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    
				   	case AC_SEL_ATT:			// 72
				    	MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_AIRCON:		// 73,
				    	MainMode_AC_Select_Index = AC_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_RADIO:		// 4,
				    	MainMode_AC_Select_Index = AC_SEL_SPK;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_SPK:			// 75,
				    	MainMode_AC_Select_Index = AC_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_HOURMETER:	// 76,
				    	MainMode_AC_Select_Index = AC_SEL_MENU;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_BAR:			// 77,
				    	MainMode_AC_Select_Index = AC_SEL_ATT;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_POWER:		// 79,
						MainMode_AC_Select_Index = AC_SEL_BAR;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_CAMERA:		// 80,
				    	if ((ACV.AirMode==0)||(ACV.AirMode==1))
							MainMode_AC_Sel(0,AC_SEL_AIRMODE,DISP_ON);
						else
						{
				   			MainMode_AC_Sel(0,AC_SEL_ONOFF,DISP_ON);
						}
				    break;
				    case AC_SEL_MENU:		// 91
				    	MainMode_AC_Select_Index = AC_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				}
			}
		}
		break;
	}
	return true;
}
bool JogDialKey_MAIN_AC(char* param)
{
	char JD_Buf[10] = {0,};
	printf("JogDialKey_MAIN_AC= '0x%X' '%d'\n",param[0],MainMode_AC_Select_Index);
	Media_RunTime_Cnt=gMainTimerCnt_10m;
	if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
    {
		switch (param[0])
		{
			case JOG_DIAL_KEY_LEFT:				//c3Byte0bit
			{
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_OFF:
					break;
				    case AC_SEL_AIRMODE:
				    	MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
				    break;
				    case AC_SEL_ONOFF:
				    	MainMode_AC_Select_Index = AC_SEL_BAR;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_DIR_L:
				    case AC_SEL_DIR_R:
				    	if (AC_SEL_DIR_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_DIR_L;
							AC_MAIN_UpBt2(NULL, JD_Buf);
							AC_SEL_DIR_L_CNT = gMainTimerCnt_10m;
						}
						else
						{
					    	if ((ACV.AirMode==0)||(ACV.AirMode==1))
								MainMode_AC_Sel(0,AC_SEL_AIRMODE,DISP_ON);
							else
								MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
						}
				    break;
				    case AC_SEL_TEMP_L:
				    case AC_SEL_TEMP_R:
				    	if (AC_SEL_TEMP_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_TEMP_L;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_TEMP_L_CNT = gMainTimerCnt_10m;
						}
						else
						{
					    	if ((ACV.AirMode==0)||(ACV.AirMode==1))
								MainMode_AC_Sel(0,AC_SEL_AIRMODE,DISP_ON);
							else
								MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
						}
				    break;
				    case AC_SEL_LEVEL_L:
				    case AC_SEL_LEVEL_R:
				    	if (AC_SEL_LEVEL_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_LEVEL_L;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_LEVEL_L_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_ONOFF,DISP_ON);
				    break;
				    case AC_SEL_AUTO:
				   		MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_PWR:
				    	MainMode_AC_Sel(0,AC_SEL_LEVEL_L,DISP_ON);
				    break;
				    
				   	case AC_SEL_ATT:			// 72
				    	MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
				    break;
				    case AC_SEL_AIRCON:		// 73,
				    	MainMode_AC_Select_Index = AC_SEL_MENU;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_RADIO:		// 4,
				    	MainMode_AC_Select_Index = AC_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_SPK:			// 75,
				    	MainMode_AC_Select_Index = AC_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_HOURMETER:	// 76,
				    	if(AUI1_0B_3210b==3) // Off
							MainMode_AC_Select_Index = AC_SEL_CAMERA;
						else
							MainMode_AC_Select_Index = AC_SEL_POWER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_BAR:			// 77,
				    	MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
				    break;
				    case AC_SEL_POWER:		// 79,
						MainMode_AC_Select_Index = AC_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_CAMERA:		// 80,
						MainMode_AC_Select_Index = AC_SEL_SPK;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_MENU:		// 91
				    	MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				}
			}
			break;
			case JOG_DIAL_KEY_RIGHT:			//c3Byte1bit
			{
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_OFF:
					break;
				    case AC_SEL_AIRMODE:
				    	MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_ONOFF:
						MainMode_AC_Sel(0,AC_SEL_LEVEL_L,DISP_ON);
				    break;
				    case AC_SEL_DIR_L:
				    case AC_SEL_DIR_R:
				    	if (AC_SEL_DIR_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_DIR_R;
							AC_MAIN_UpBt2(NULL, JD_Buf);
							AC_SEL_DIR_R_CNT = gMainTimerCnt_10m;
						}
						else
						{
					    	MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
						}
				    break;
				    case AC_SEL_TEMP_L:
				    case AC_SEL_TEMP_R:
				    	if (AC_SEL_TEMP_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_TEMP_R;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_TEMP_R_CNT = gMainTimerCnt_10m;
						}
						else
						{
					    	MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
						}
				    break;
				    case AC_SEL_LEVEL_L:
				    case AC_SEL_LEVEL_R:
				    	if (AC_SEL_LEVEL_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_LEVEL_R;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_LEVEL_R_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
				    break;
				    case AC_SEL_AUTO:
				    	if ((ACV.AirMode==0)||(ACV.AirMode==1))
							MainMode_AC_Sel(0,AC_SEL_AIRMODE,DISP_ON);
						else
							MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_PWR:
				    	MainMode_AC_Select_Index = AC_SEL_BAR;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    
				   	case AC_SEL_ATT:			// 72
				    	MainMode_AC_Sel(0,AC_SEL_ONOFF,DISP_ON);
				    break;
				    case AC_SEL_AIRCON:		// 73,
				    	MainMode_AC_Select_Index = AC_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_RADIO:		// 4,
				    	MainMode_AC_Select_Index = AC_SEL_SPK;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_SPK:			// 75,
				    	MainMode_AC_Select_Index = AC_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_HOURMETER:	// 76,
				    	MainMode_AC_Select_Index = AC_SEL_MENU;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_BAR:			// 77,
				    	MainMode_AC_Sel(0,AC_SEL_ONOFF,DISP_ON);
				    break;
				    case AC_SEL_POWER:		// 79,
						MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_CAMERA:		// 80,
						if(AUI1_0B_3210b==3) // Off
							MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						else
							MainMode_AC_Select_Index = AC_SEL_POWER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_MENU:		// 91
				    	MainMode_AC_Select_Index = AC_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				}
			}
			break;
			case JOG_DIAL_KEY_UP:				//c3Byte2bit
			{
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_OFF:
					break;
				    case AC_SEL_AIRMODE:
				    	MainMode_AC_Select_Index = AC_SEL_MENU;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_ONOFF:
				    	if ((ACV.AirMode==0)||(ACV.AirMode==1))
							MainMode_AC_Sel(0,AC_SEL_AIRMODE,DISP_ON);
						else
						{
				    		MainMode_AC_Select_Index = AC_SEL_MENU;
							MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
						}
				    break;
				    case AC_SEL_DIR_L:
				    case AC_SEL_DIR_R:
				    	if (AC_SEL_DIR_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_DIR_L;
							AC_MAIN_UpBt2(NULL, JD_Buf);
							AC_SEL_DIR_L_CNT = gMainTimerCnt_10m;
						}
						else
						{
				    		MainMode_AC_Select_Index = AC_SEL_MENU;
							MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
						}
				    break;
				    case AC_SEL_TEMP_L:
				    case AC_SEL_TEMP_R:
				    	if (AC_SEL_TEMP_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_TEMP_L;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_TEMP_L_CNT = gMainTimerCnt_10m;
						}
						else
						{
					    	MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
						}
				    break;
				    case AC_SEL_LEVEL_L:
				    case AC_SEL_LEVEL_R:
				    	if (AC_SEL_LEVEL_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_LEVEL_L;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_LEVEL_L_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_TEMP_L,DISP_ON);
				    break;
				    case AC_SEL_AUTO:
				    	MainMode_AC_Select_Index = AC_SEL_MENU;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_PWR:
				    	MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
				    break;
				    
				   	case AC_SEL_ATT:			// 72
				    	MainMode_AC_Select_Index = AC_SEL_BAR;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_AIRCON:		// 73,
				    	MainMode_AC_Sel(0,AC_SEL_LEVEL_L,DISP_ON);
				    break;
				    case AC_SEL_RADIO:		// 4,
				    	MainMode_AC_Sel(0,AC_SEL_LEVEL_L,DISP_ON);
				    break;
				    case AC_SEL_SPK:			// 75,
				    	MainMode_AC_Sel(0,AC_SEL_LEVEL_L,DISP_ON);
				    break;
				    case AC_SEL_HOURMETER:	// 76,
				    	MainMode_AC_Select_Index = AC_SEL_ATT;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_BAR:			// 77,
				    	MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_POWER:		// 79,
						MainMode_AC_Select_Index = AC_SEL_ATT;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_CAMERA:		// 80,
						MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
				    break;
				    case AC_SEL_MENU:		// 91
				    	MainMode_AC_Sel(0,AC_SEL_ONOFF,DISP_ON);
				    break;
				}
			}
			break;
			case JOG_DIAL_KEY_DOWN:				//c3Byte3bit
			{
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_OFF:
					break;
				    case AC_SEL_AIRMODE:
				    	MainMode_AC_Sel(0,AC_SEL_ONOFF,DISP_ON);
				    break;
				    case AC_SEL_ONOFF:
				    	MainMode_AC_Select_Index = AC_SEL_MENU;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_DIR_L:
				    case AC_SEL_DIR_R:
				    	if (AC_SEL_DIR_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_DIR_R;
							AC_MAIN_UpBt2(NULL, JD_Buf);
							AC_SEL_DIR_R_CNT = gMainTimerCnt_10m;
						}
						else
						{
					    	MainMode_AC_Sel(0,AC_SEL_TEMP_L,DISP_ON);
						}
				    break;
				    case AC_SEL_TEMP_L:
				    case AC_SEL_TEMP_R:
				    	if (AC_SEL_TEMP_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_TEMP_R;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_TEMP_R_CNT = gMainTimerCnt_10m;
						}
						else
						{
					    	MainMode_AC_Sel(0,AC_SEL_LEVEL_L,DISP_ON);
						}
				    break;
				    case AC_SEL_LEVEL_L:
				    case AC_SEL_LEVEL_R:
				    	if (AC_SEL_LEVEL_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_LEVEL_R;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_LEVEL_R_CNT = gMainTimerCnt_10m;
						}
						else
						{
				    		MainMode_AC_Select_Index = AC_SEL_RADIO;
							MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
						}
				    break;
				    case AC_SEL_AUTO:
				   		MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
				    break;
				    case AC_SEL_PWR:
				    	MainMode_AC_Select_Index = AC_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    
				   	case AC_SEL_ATT:			// 72
				    	MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_AIRCON:		// 73,
				    	MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_RADIO:		// 4,
				    	MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_SPK:			// 75,
				    	MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_HOURMETER:	// 76,
				    	MainMode_AC_Select_Index = AC_SEL_BAR;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_BAR:			// 77,
				    	MainMode_AC_Select_Index = AC_SEL_ATT;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_POWER:		// 79,
						MainMode_AC_Select_Index = AC_SEL_BAR;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_CAMERA:		// 80,
						MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_MENU:		// 91
				    	MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				}
			}
			break;
			case JOG_DIAL_KEY_LEFT_OFF:
			case JOG_DIAL_KEY_RIGHT_OFF:
			case JOG_DIAL_KEY_UP_OFF:
			case JOG_DIAL_KEY_DOWN_OFF:
			{
				printf("JOG_DIAL_KEY_ENTER_ON1='%d' '%d' '%d' '0x%02X' \n",AC_SEL_DIR_LR_FLAG,AC_SEL_TEMP_LR_FLAG,AC_SEL_LEVEL_LR_FLAG);
				if (AC_SEL_DIR_LR_FLAG)
				{
					Main_CLI2[2] = Main_CLI2[2] & 0xC0;// 0xFC;
				}
				if (AC_SEL_TEMP_LR_FLAG)
				{
					Main_CLI2[2] = Main_CLI2[2] & 0xC0;// 0xF3;
				}
				if (AC_SEL_LEVEL_LR_FLAG)
				{
					Main_CLI2[2] = Main_CLI2[2] & 0xC0;//0xCF;
				}
				printf("JOG_DIAL_KEY_ENTER_ON2='%d' '%d' '%d' '0x%02X' \n",AC_SEL_DIR_LR_FLAG,AC_SEL_TEMP_LR_FLAG,AC_SEL_LEVEL_LR_FLAG);
			}
			break;
			case JOG_DIAL_KEY_ENTER_ON:
			{	
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_HOURMETER:
						printf("JOG_DIAL_KEY_ENTER_ON=AC_SEL_HOURMETER '%d' \n",KEY_ENTER_HM_CNT);
						if (KEY_ENTER_HM_CNT == 0)
							KEY_ENTER_HM_CNT = gMainTimerCnt_10m;
					break;
					case AC_SEL_ATT:
					{
						if (KEY_ENTER_ATT_CNT == 0)
							KEY_ENTER_ATT_CNT = gMainTimerCnt_10m;
					}
					break;
				}
			}
			break;
			case JOG_DIAL_KEY_ENTER_OFF:
//			case JOG_DIAL_KEY_NONE:
			{
	//			printf("JOG_DIAL_KEY_ENTER_OFF= '%d' 0x%x\n",MainMode_AC_Select_Index,KEY_ENTER_HM_CNT);
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_AIRMODE:
					case AC_SEL_ONOFF:
					case AC_SEL_AUTO:
					case AC_SEL_PWR:
						JD_Buf[0] = MainMode_AC_Select_Index;
						AC_MAIN_UpBt(NULL, JD_Buf);
					break;
					case AC_SEL_DIR_L:
					case AC_SEL_DIR_R:
						if (AC_SEL_DIR_LR_FLAG==0)
						{
							AC_SEL_DIR_LR_FLAG = 1;
						
						}
						else
						{
							AC_SEL_DIR_LR_FLAG = 0;
							Main_CLI2[2] = 0;
							MainMode_AC_Sel(0,AC_SEL_DIR_R,DISP_ON);
							AC_SEL_DIR_R_CNT = LONG_KEY_TIME;
							AC_SEL_DIR_L_CNT = LONG_KEY_TIME;
						}
					break;
					case AC_SEL_TEMP_L:
					case AC_SEL_TEMP_R:
						if (AC_SEL_TEMP_LR_FLAG==0)
						{
							AC_SEL_TEMP_LR_FLAG = 1;
						
						}
						else
						{
							AC_SEL_TEMP_LR_FLAG = 0;
							Main_CLI2[2] = 0;
							MainMode_AC_Sel(0,AC_SEL_TEMP_R,DISP_ON);
							AC_SEL_TEMP_R_CNT = LONG_KEY_TIME;
							AC_SEL_TEMP_L_CNT = LONG_KEY_TIME;
						}
					break;
					case AC_SEL_LEVEL_L:
					case AC_SEL_LEVEL_R:
						if (AC_SEL_LEVEL_LR_FLAG==0)
						{
							AC_SEL_LEVEL_LR_FLAG = 1;
						
						}
						else
						{
							AC_SEL_LEVEL_LR_FLAG = 0;
							Main_CLI2[2] = 0;
							MainMode_AC_Sel(0,AC_SEL_LEVEL_R,DISP_ON);
							AC_SEL_LEVEL_R_CNT = LONG_KEY_TIME;
							AC_SEL_LEVEL_L_CNT = LONG_KEY_TIME;
						}
					break;					case AC_SEL_ATT:
					{
						if (KEY_ENTER_ATT_CNT != 0xFFFFF000)
						{
							JD_Buf[1] = 0;
							if (AUX1CurrentOutMode==0)
								JD_Buf[0] = 2;
							else if (AUX1CurrentOutMode==1)
								JD_Buf[0] = 3;
							else if (AUX1CurrentOutMode==2)
								JD_Buf[0] = 1;
							Main_ATTBt_Func(NULL, JD_Buf);
						}
						KEY_ENTER_ATT_CNT = 0;
					}
					break;
					case AC_SEL_AIRCON:
					{
					}
					break;
					case AC_SEL_RADIO:
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_RadioBt_Func("AC_SEL_RADIO", JD_Buf);
					}
					break;
					case AC_SEL_SPK:
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_SPK_Bt_Func(NULL, JD_Buf);
					}
					break;
					case AC_SEL_HOURMETER:
					{
						if (KEY_ENTER_HM_CNT != 0xFFFFF000)
						{
							JD_Buf[0] = 0;
							JD_Buf[1] = 0;
							Main_HMeterBt_Func(NULL, JD_Buf);
						}
						else
						{
							JD_Buf[0] = 0;
							JD_Buf[1] = 0;
							Main_HMeterBt_Func(NULL, JD_Buf);
						}
						KEY_ENTER_HM_CNT = 0;
					}
					break;
					case AC_SEL_BAR:
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_BarBt_Func(NULL, JD_Buf);
					}
					break;
					case AC_SEL_POWER:
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_PwrBt_Func(NULL, JD_Buf);
					}
					break;
					case AC_SEL_CAMERA:
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						MainCamera(NULL, JD_Buf);
					}
					break;
					case AC_SEL_MENU:
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						MainHome(NULL, JD_Buf);
					}
					break;
				}
			}
		}    	
	}
	else
	{
		switch (param[0])
		{
			case JOG_DIAL_KEY_LEFT:				//c3Byte0bit
			{
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_OFF:
					break;
				    case AC_SEL_AIRMODE:
				    	MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
				    break;
				    case AC_SEL_ONOFF:
						MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
				    break;
				    case AC_SEL_DIR_L:
				    case AC_SEL_DIR_R:
				    	if (AC_SEL_DIR_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_DIR_L;
							AC_MAIN_UpBt2(NULL, JD_Buf);
							AC_SEL_DIR_L_CNT = gMainTimerCnt_10m;
						}
						else
						{
					    	if ((ACV.AirMode==0)||(ACV.AirMode==1))
								MainMode_AC_Sel(0,AC_SEL_AIRMODE,DISP_ON);
							else
								MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
						}
				    break;
				    case AC_SEL_TEMP_L:
				    case AC_SEL_TEMP_R:
				    	if (AC_SEL_TEMP_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_TEMP_L;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_TEMP_L_CNT = gMainTimerCnt_10m;
						}
						else
						{
					    	if ((ACV.AirMode==0)||(ACV.AirMode==1))
								MainMode_AC_Sel(0,AC_SEL_AIRMODE,DISP_ON);
							else
								MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
						}
				    break;
				    case AC_SEL_LEVEL_L:
				    case AC_SEL_LEVEL_R:
				    	if (AC_SEL_LEVEL_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_LEVEL_L;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_LEVEL_L_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_ONOFF,DISP_ON);
				    break;
				    case AC_SEL_AUTO:
				   		MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_PWR:
				    	MainMode_AC_Sel(0,AC_SEL_LEVEL_L,DISP_ON);
				    break;
				    
				   	case AC_SEL_ATT:			// 72
				    	MainMode_AC_Select_Index = AC_SEL_BAR;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_AIRCON:		// 73,
				    	MainMode_AC_Select_Index = AC_SEL_MENU;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_RADIO:		// 4,
				    	MainMode_AC_Select_Index = AC_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_SPK:			// 75,
				    	MainMode_AC_Select_Index = AC_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_HOURMETER:	// 76,
				    break;
				    case AC_SEL_BAR:			// 77,
				    	if(AUI1_0B_3210b==3) // Off
							MainMode_AC_Select_Index = AC_SEL_ATT;
						else
							MainMode_AC_Select_Index = AC_SEL_POWER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_POWER:		// 79,
						MainMode_AC_Select_Index = AC_SEL_ATT;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_CAMERA:		// 80,
						MainMode_AC_Select_Index = AC_SEL_SPK;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_MENU:		// 91
				    	MainMode_AC_Select_Index = AC_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				}
			}
			break;
			case JOG_DIAL_KEY_RIGHT:			//c3Byte1bit
			{
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_OFF:
					break;
				    case AC_SEL_AIRMODE:
				    	MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_ONOFF:
						MainMode_AC_Sel(0,AC_SEL_LEVEL_L,DISP_ON);
				    break;
				    case AC_SEL_DIR_L:
				    case AC_SEL_DIR_R:
				    	if (AC_SEL_DIR_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_DIR_R;
							AC_MAIN_UpBt2(NULL, JD_Buf);
							AC_SEL_DIR_R_CNT = gMainTimerCnt_10m;
						}
						else
						{
					    	MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
						}
				    break;
				    case AC_SEL_TEMP_L:
				    case AC_SEL_TEMP_R:
				    	if (AC_SEL_TEMP_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_TEMP_R;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_TEMP_R_CNT = gMainTimerCnt_10m;
						}
						else
						{
					    	MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
						}
				    break;
				    case AC_SEL_LEVEL_L:
				    case AC_SEL_LEVEL_R:
				    	if (AC_SEL_LEVEL_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_LEVEL_R;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_LEVEL_R_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
				    break;
				    case AC_SEL_AUTO:
				    	if ((ACV.AirMode==0)||(ACV.AirMode==1))
							MainMode_AC_Sel(0,AC_SEL_AIRMODE,DISP_ON);
						else
							MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_PWR:
				    	MainMode_AC_Sel(0,AC_SEL_ONOFF,DISP_ON);
				    break;
				    
				   	case AC_SEL_ATT:			// 72
				    	if(AUI1_0B_3210b==3) // Off
							MainMode_AC_Select_Index = AC_SEL_BAR;
						else
							MainMode_AC_Select_Index = AC_SEL_POWER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_AIRCON:		// 73,
				    	MainMode_AC_Select_Index = AC_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_RADIO:		// 4,
				    	MainMode_AC_Select_Index = AC_SEL_SPK;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_SPK:			// 75,
				    	MainMode_AC_Select_Index = AC_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_HOURMETER:	// 76,
				    break;
				    case AC_SEL_BAR:			// 77,
				    	MainMode_AC_Select_Index = AC_SEL_ATT;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_POWER:		// 79,
						MainMode_AC_Select_Index = AC_SEL_BAR;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_CAMERA:		// 80,
						MainMode_AC_Select_Index = AC_SEL_MENU;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_MENU:		// 91
				    	MainMode_AC_Select_Index = AC_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				}
			}
			break;
			case JOG_DIAL_KEY_UP:				//c3Byte2bit
			{
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_OFF:
					break;
				    case AC_SEL_AIRMODE:
				    	MainMode_AC_Select_Index = AC_SEL_MENU;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_ONOFF:
			    		if ((ACV.AirMode==0)||(ACV.AirMode==1))
							MainMode_AC_Sel(0,AC_SEL_AIRMODE,DISP_ON);
						else
						{
				    		MainMode_AC_Select_Index = AC_SEL_MENU;
							MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
						}
				    break;
				    case AC_SEL_DIR_L:
				    case AC_SEL_DIR_R:
				    	if (AC_SEL_DIR_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_DIR_L;
							AC_MAIN_UpBt2(NULL, JD_Buf);
							AC_SEL_DIR_L_CNT = gMainTimerCnt_10m;
						}
						else
						{
				    		MainMode_AC_Select_Index = AC_SEL_MENU;
							MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
						}
				    break;
				    case AC_SEL_TEMP_L:
				    case AC_SEL_TEMP_R:
				    	if (AC_SEL_TEMP_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_TEMP_L;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_TEMP_L_CNT = gMainTimerCnt_10m;
						}
						else
						{
					    	MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
						}
				    break;
				    case AC_SEL_LEVEL_L:
				    case AC_SEL_LEVEL_R:
				    	if (AC_SEL_LEVEL_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_LEVEL_L;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_LEVEL_L_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_AC_Sel(0,AC_SEL_TEMP_L,DISP_ON);
				    break;
				    case AC_SEL_AUTO:
				    	MainMode_AC_Select_Index = AC_SEL_MENU;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_PWR:
				    	MainMode_AC_Sel(0,AC_SEL_AUTO,DISP_ON);
				    break;
				    
				   	case AC_SEL_ATT:			// 72
				    	MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
				    break;
				    case AC_SEL_AIRCON:		// 73,
				    	MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_RADIO:		// 4,
				    	MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_SPK:			// 75,
				    	MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_HOURMETER:	// 76,
				    	MainMode_AC_Select_Index = AC_SEL_BAR;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_BAR:			// 77,
				    	MainMode_AC_Sel(0,AC_SEL_LEVEL_L,DISP_ON);
				    break;
				    case AC_SEL_POWER:		// 79,
						MainMode_AC_Sel(0,AC_SEL_ONOFF,DISP_ON);
				    break;
				    case AC_SEL_CAMERA:		// 80,
						MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_MENU:		// 91
						MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				}
			}
			break;
			case JOG_DIAL_KEY_DOWN:				//c3Byte3bit
			{
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_OFF:
					break;
				    case AC_SEL_AIRMODE:
				    	MainMode_AC_Sel(0,AC_SEL_ONOFF,DISP_ON);
				    break;
				    case AC_SEL_ONOFF:
				    	MainMode_AC_Select_Index = AC_SEL_BAR;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_DIR_L:
				    case AC_SEL_DIR_R:
				    	if (AC_SEL_DIR_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_DIR_R;
							AC_MAIN_UpBt2(NULL, JD_Buf);
							AC_SEL_DIR_R_CNT = gMainTimerCnt_10m;
						}
						else
						{
					    	MainMode_AC_Sel(0,AC_SEL_TEMP_L,DISP_ON);
						}
				    break;
				    case AC_SEL_TEMP_L:
				    case AC_SEL_TEMP_R:
				    	if (AC_SEL_TEMP_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_TEMP_R;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_TEMP_R_CNT = gMainTimerCnt_10m;
						}
						else
						{
					    	MainMode_AC_Sel(0,AC_SEL_LEVEL_L,DISP_ON);
						}
				    break;
				    case AC_SEL_LEVEL_L:
				    case AC_SEL_LEVEL_R:
				    	if (AC_SEL_LEVEL_LR_FLAG)
						{
							JD_Buf[0] = AC_SEL_LEVEL_R;
							AC_MAIN_DnBt2(NULL, JD_Buf);
							AC_SEL_LEVEL_R_CNT = gMainTimerCnt_10m;
						}
						else
						{
				    		MainMode_AC_Select_Index = AC_SEL_BAR;
							MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
						}
				    break;
				    case AC_SEL_AUTO:
				   		MainMode_AC_Sel(0,AC_SEL_PWR,DISP_ON);
				    break;
				    case AC_SEL_PWR:
				    	MainMode_AC_Select_Index = AC_SEL_ATT;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    
				   	case AC_SEL_ATT:			// 72
				    	MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_AIRCON:		// 73,
				    	MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_RADIO:		// 4,
				    	MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_SPK:			// 75,
				    	MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_HOURMETER:	// 76,
				    	MainMode_AC_Select_Index = AC_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_BAR:			// 77,
				    	MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_POWER:		// 79,
						MainMode_AC_Select_Index = AC_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_AC_Select_Index,DISP_ON);
				    break;
				    case AC_SEL_CAMERA:		// 80,
						MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				    case AC_SEL_MENU:		// 91
				    	MainMode_AC_Sel(0,AC_SEL_DIR_L,DISP_ON);
				    break;
				}
			}
			break;
			case JOG_DIAL_KEY_LEFT_OFF:
			case JOG_DIAL_KEY_RIGHT_OFF:
			case JOG_DIAL_KEY_UP_OFF:
			case JOG_DIAL_KEY_DOWN_OFF:
			{
				printf("JOG_DIAL_KEY_ENTER_ON1='%d' '%d' '%d' '0x%02X' \n",AC_SEL_DIR_LR_FLAG,AC_SEL_TEMP_LR_FLAG,AC_SEL_LEVEL_LR_FLAG);
				if (AC_SEL_DIR_LR_FLAG)
				{
					Main_CLI2[2] = Main_CLI2[2] & 0xC0;//0xFC;
				}
				if (AC_SEL_TEMP_LR_FLAG)
				{
					Main_CLI2[2] = Main_CLI2[2] & 0xC0;//0xF3;
				}
				if (AC_SEL_LEVEL_LR_FLAG)
				{
					Main_CLI2[2] = Main_CLI2[2] & 0xC0;//0xCF;
				}
				printf("JOG_DIAL_KEY_ENTER_ON2='%d' '%d' '%d' '0x%02X' \n",AC_SEL_DIR_LR_FLAG,AC_SEL_TEMP_LR_FLAG,AC_SEL_LEVEL_LR_FLAG);
			}
			break;
			case JOG_DIAL_KEY_ENTER_ON:
			{	
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_HOURMETER:
						printf("JOG_DIAL_KEY_ENTER_ON=AC_SEL_HOURMETER '%d' \n",KEY_ENTER_HM_CNT);
						if (KEY_ENTER_HM_CNT == 0)
							KEY_ENTER_HM_CNT = gMainTimerCnt_10m;
					break;
					case AC_SEL_ATT:
					{
						if (KEY_ENTER_ATT_CNT == 0)
							KEY_ENTER_ATT_CNT = gMainTimerCnt_10m;
					}
					break;
				}
			}
			break;
			case JOG_DIAL_KEY_ENTER_OFF:
//			case JOG_DIAL_KEY_NONE:
			{
	//			printf("JOG_DIAL_KEY_ENTER_OFF= '%d' 0x%x\n",MainMode_AC_Select_Index,KEY_ENTER_HM_CNT);
				switch(MainMode_AC_Select_Index)
				{
					case AC_SEL_AIRMODE:
					case AC_SEL_ONOFF:
					case AC_SEL_AUTO:
					case AC_SEL_PWR:
						JD_Buf[0] = MainMode_AC_Select_Index;
						AC_MAIN_UpBt(NULL, JD_Buf);
					break;
					case AC_SEL_DIR_L:
					case AC_SEL_DIR_R:
						if (AC_SEL_DIR_LR_FLAG==0)
						{
							AC_SEL_DIR_LR_FLAG = 1;

						}
						else
						{
							AC_SEL_DIR_LR_FLAG = 0;
							Main_CLI2[2] = 0;
							MainMode_AC_Sel(0,AC_SEL_DIR_R,DISP_ON);
							AC_SEL_DIR_R_CNT = LONG_KEY_TIME;
							AC_SEL_DIR_L_CNT = LONG_KEY_TIME;
						}
					break;
					case AC_SEL_TEMP_L:
					case AC_SEL_TEMP_R:
						if (AC_SEL_TEMP_LR_FLAG==0)
						{
							AC_SEL_TEMP_LR_FLAG = 1;
						
						}
						else
						{
							AC_SEL_TEMP_LR_FLAG = 0;
							Main_CLI2[2] = 0;
							MainMode_AC_Sel(0,AC_SEL_TEMP_R,DISP_ON);
							AC_SEL_TEMP_R_CNT = LONG_KEY_TIME;
							AC_SEL_TEMP_L_CNT = LONG_KEY_TIME;
						}
					break;
					case AC_SEL_LEVEL_L:
					case AC_SEL_LEVEL_R:
						if (AC_SEL_LEVEL_LR_FLAG==0)
						{
							AC_SEL_LEVEL_LR_FLAG = 1;
							
						}
						else
						{
							AC_SEL_LEVEL_LR_FLAG = 0;
							Main_CLI2[2] = 0;
							MainMode_AC_Sel(0,AC_SEL_LEVEL_R,DISP_ON);
							AC_SEL_LEVEL_R_CNT = LONG_KEY_TIME;
							AC_SEL_LEVEL_L_CNT = LONG_KEY_TIME;
						}
					break;
					case AC_SEL_ATT:
					{
						if (KEY_ENTER_ATT_CNT != 0xFFFFF000)
						{
							JD_Buf[1] = 0;
							if (AUX1CurrentOutMode==0)
								JD_Buf[0] = 2;
							else if (AUX1CurrentOutMode==1)
								JD_Buf[0] = 3;
							else if (AUX1CurrentOutMode==2)
								JD_Buf[0] = 1;
							Main_ATTBt_Func(NULL, JD_Buf);
						}
						KEY_ENTER_ATT_CNT = 0;
					}
					break;
					case AC_SEL_AIRCON:
					{
					}
					break;
					case AC_SEL_RADIO:
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_RadioBt_Func("AC_SEL_RADIO", JD_Buf);
					}
					break;
					case AC_SEL_SPK:
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_SPK_Bt_Func(NULL, JD_Buf);
					}
					break;
					case AC_SEL_HOURMETER:
					{
						if (KEY_ENTER_HM_CNT != 0xFFFFF000)
						{
							JD_Buf[0] = 0;
							JD_Buf[1] = 0;
							Main_HMeterBt_Func(NULL, JD_Buf);
						}
						else
						{
							JD_Buf[0] = 0;
							JD_Buf[1] = 0;
							Main_HMeterBt_Func(NULL, JD_Buf);
						}
						KEY_ENTER_HM_CNT = 0;
					}
					break;
					case AC_SEL_BAR:
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_BarBt_Func(NULL, JD_Buf);
					}
					break;
					case AC_SEL_POWER:
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_PwrBt_Func(NULL, JD_Buf);
					}
					break;
					case AC_SEL_CAMERA:
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						MainCamera(NULL, JD_Buf);
					}
					break;
					case AC_SEL_MENU:
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						MainHome(NULL, JD_Buf);
					}
					break;
				}
			}
		}
	}
	return true;
}


#define RADIO_JOG_KEY_SEL(SelIndex) {																\
										if (Main_Disp_Mode==MAIN_MODE_DAB1)							\
										{															\
									   		MainMode_DAB_Select_Index = SelIndex;					\
											MainLayer_ButtonSel(MainMode_DAB_Select_Index,DISP_ON);	\
										}															\
										else														\
										{															\
									   		MainMode_RD_Select_Index = SelIndex;					\
											MainLayer_ButtonSel(MainMode_RD_Select_Index,DISP_ON);	\
										}															\
									}
												
bool JogDialRot_MAIN_RD(char* param)
{
	char JD_Buf[10] = {0,};
	uint8_t Select_Index;
	RD_SEL_IDX SelIndex;
	if (Main_Disp_Mode==MAIN_MODE_DAB1)
		Select_Index = MainMode_DAB_Select_Index;
	else
		Select_Index = MainMode_RD_Select_Index;
	printf("JogDialRot_MAIN_RD= '0x%X' '%d'\n",param[0],Select_Index);
	Media_RunTime_Cnt=gMainTimerCnt_10m;
	switch (param[0])
	{
		case JOG_DIAL_ROT_LEFT:				//c3Byte0bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_1C:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_2C:
						MainMode_RD_Sel(1,RD_SEL_1C,DISP_ON);
					break;
					case RD_SEL_3C:
						MainMode_RD_Sel(1,RD_SEL_2C,DISP_ON);
					break;
					case RD_SEL_4C:
						MainMode_RD_Sel(1,RD_SEL_3C,DISP_ON);
					break;
					case RD_SEL_5C:
						MainMode_RD_Sel(1,RD_SEL_4C,DISP_ON);
					break;
					case RD_SEL_6C:
						MainMode_RD_Sel(1,RD_SEL_5C,DISP_ON);
					break;
					case RD_SEL_7C:
						MainMode_RD_Sel(1,RD_SEL_6C,DISP_ON);
					break;
					case RD_SEL_8C:
						MainMode_RD_Sel(1,RD_SEL_7C,DISP_ON);
					break;
					case RD_SEL_TUNE_L:
					case RD_SEL_TUNE_R:
				   		if (RD_SEL_TUNE_LR_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TUNE_L/10);
							JD_Buf[1] = (RD_SEL_TUNE_L%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TUNE_LR_FLAG = 2;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_8C,DISP_ON);
					break;
					case RD_SEL_AUTO:
						if (Main_Disp_Mode==MAIN_MODE_RADIO)
							MainMode_RD_Sel(1,RD_SEL_TUNE_R,DISP_ON);
						else
							MainMode_RD_Sel(1,RD_SEL_SCAN,DISP_ON);	
					break;
					case RD_SEL_MODE:
				   		MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_DU_FLAG = 2;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_DU_FLAG = 2;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case RD_SEL_PWR:
						SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
					case RD_SEL_SCAN:
				   		MainMode_RD_Sel(1,RD_SEL_TUNE_R,DISP_ON);
					break;
				    case RD_SEL_ATT:
						SelIndex = RD_SEL_BAR;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_AIRCON:
						SelIndex = RD_SEL_MENU;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_RADIO:
						SelIndex = RD_SEL_AIRCON;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_SPK:
						SelIndex = RD_SEL_RADIO;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_HOURMETER:
						if(AUI1_0B_3210b==3) // Off
							SelIndex = RD_SEL_ATT;
						else
							SelIndex = RD_SEL_POWER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_BAR:
						SelIndex = RD_SEL_CAMERA;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						SelIndex = RD_SEL_ATT;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_CAMERA:
						SelIndex = RD_SEL_SPK;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
				    case RD_SEL_MENU:
				   		MainMode_RD_Sel(1,RD_SEL_VOL_U,DISP_ON);
				    break;
				}
			}
			else
			{
				switch(Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_1C:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_2C:
						MainMode_RD_Sel(1,RD_SEL_1C,DISP_ON);
					break;
					case RD_SEL_3C:
						MainMode_RD_Sel(1,RD_SEL_2C,DISP_ON);
					break;
					case RD_SEL_4C:
						MainMode_RD_Sel(1,RD_SEL_3C,DISP_ON);
					break;
					case RD_SEL_5C:
						MainMode_RD_Sel(1,RD_SEL_4C,DISP_ON);
					break;
					case RD_SEL_6C:
						MainMode_RD_Sel(1,RD_SEL_5C,DISP_ON);
					break;
					case RD_SEL_7C:
						MainMode_RD_Sel(1,RD_SEL_6C,DISP_ON);
					break;
					case RD_SEL_8C:
						MainMode_RD_Sel(1,RD_SEL_7C,DISP_ON);
					break;
					case RD_SEL_TUNE_L:
					case RD_SEL_TUNE_R:
				   		if (RD_SEL_TUNE_LR_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TUNE_L/10);
							JD_Buf[1] = (RD_SEL_TUNE_L%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TUNE_LR_FLAG = 2;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_8C,DISP_ON);
					break;
					case RD_SEL_AUTO:
						if (Main_Disp_Mode==MAIN_MODE_RADIO)
							MainMode_RD_Sel(1,RD_SEL_TUNE_R,DISP_ON);
						else
							MainMode_RD_Sel(1,RD_SEL_SCAN,DISP_ON);	
					break;
					case RD_SEL_MODE:
				   		MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_DU_FLAG = 2;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_DU_FLAG = 2;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case RD_SEL_PWR:
						SelIndex = RD_SEL_CAMERA;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
					case RD_SEL_SCAN:
				   		MainMode_RD_Sel(1,RD_SEL_TUNE_R,DISP_ON);
					break;
				    case RD_SEL_ATT:
						SelIndex = RD_SEL_BAR;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_AIRCON:
						SelIndex = RD_SEL_MENU;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_RADIO:
						SelIndex = RD_SEL_AIRCON;;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_SPK:
						SelIndex = RD_SEL_RADIO;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_HOURMETER:
						SelIndex = RD_SEL_ATT;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_BAR:
						if(AUI1_0B_3210b==3) // Off
							MainMode_RD_Sel(1,RD_SEL_VOL_U,DISP_ON);
						else
						{
							SelIndex = RD_SEL_POWER;
							RADIO_JOG_KEY_SEL (SelIndex);
						}
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_RD_Sel(1,RD_SEL_VOL_U,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
						SelIndex = RD_SEL_SPK;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_MENU:
						SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				}
			}
		}
		break;

		case JOG_DIAL_ROT_RIGHT:			//c3Byte1bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(Select_Index)
				{
					case RD_SEL_OFF:
//						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_1C:
						MainMode_RD_Sel(1,RD_SEL_2C,DISP_ON);
					break;
					case RD_SEL_2C:
						MainMode_RD_Sel(1,RD_SEL_3C,DISP_ON);
					break;
					case RD_SEL_3C:
						MainMode_RD_Sel(1,RD_SEL_4C,DISP_ON);
					break;
					case RD_SEL_4C:
						MainMode_RD_Sel(1,RD_SEL_5C,DISP_ON);
					break;
					case RD_SEL_5C:
						MainMode_RD_Sel(1,RD_SEL_6C,DISP_ON);
					break;
					case RD_SEL_6C:
						MainMode_RD_Sel(1,RD_SEL_7C,DISP_ON);
					break;
					case RD_SEL_7C:
						MainMode_RD_Sel(1,RD_SEL_8C,DISP_ON);
					break;
					case RD_SEL_8C:
						MainMode_RD_Sel(1,RD_SEL_TUNE_L,DISP_ON);
					break;
					case RD_SEL_TUNE_L:
					case RD_SEL_TUNE_R:
						if (RD_SEL_TUNE_LR_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TUNE_R/10);
							JD_Buf[1] = (RD_SEL_TUNE_R%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TUNE_LR_FLAG = 2;
						}
						else
						{
							if (Main_Disp_Mode==MAIN_MODE_RADIO)
								MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
							else
								MainMode_RD_Sel(1,RD_SEL_SCAN,DISP_ON);
						}
					break;
					case RD_SEL_AUTO:
				   		MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_MODE:
				   		MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_U/10);
							JD_Buf[1] = (RD_SEL_TONE_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_DU_FLAG = 2;
						}
						else
						{
							MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
						}
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_U/10);
							JD_Buf[1] = (RD_SEL_VOL_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_DU_FLAG = 2;
						}
						else
						{
							SelIndex = RD_SEL_MENU;
							RADIO_JOG_KEY_SEL (SelIndex);
						}
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_1C,DISP_ON);
					break;
					case RD_SEL_PWR:
				   		MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_SCAN:
				   		MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
				    case RD_SEL_ATT:
						if(AUI1_0B_3210b==3) // Off
							SelIndex = RD_SEL_HOURMETER;
						else
							SelIndex = RD_SEL_POWER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_AIRCON:
						SelIndex = RD_SEL_RADIO;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_RADIO:
						SelIndex = RD_SEL_SPK;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_SPK:
						SelIndex = RD_SEL_CAMERA;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_HOURMETER:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_BAR:
						SelIndex = RD_SEL_ATT;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_CAMERA:
						SelIndex = RD_SEL_BAR;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_MENU:
						SelIndex = RD_SEL_AIRCON;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				}
			}
			else
			{
				switch(Select_Index)
				{
					case RD_SEL_OFF:
//						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_1C:
						MainMode_RD_Sel(1,RD_SEL_2C,DISP_ON);
					break;
					case RD_SEL_2C:
						MainMode_RD_Sel(1,RD_SEL_3C,DISP_ON);
					break;
					case RD_SEL_3C:
						MainMode_RD_Sel(1,RD_SEL_4C,DISP_ON);
					break;
					case RD_SEL_4C:
						MainMode_RD_Sel(1,RD_SEL_5C,DISP_ON);
					break;
					case RD_SEL_5C:
						MainMode_RD_Sel(1,RD_SEL_6C,DISP_ON);
					break;
					case RD_SEL_6C:
						MainMode_RD_Sel(1,RD_SEL_7C,DISP_ON);
					break;
					case RD_SEL_7C:
						MainMode_RD_Sel(1,RD_SEL_8C,DISP_ON);
					break;
					case RD_SEL_8C:
						MainMode_RD_Sel(1,RD_SEL_TUNE_L,DISP_ON);
					break;
					case RD_SEL_TUNE_L:
					case RD_SEL_TUNE_R:
						if (RD_SEL_TUNE_LR_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TUNE_R/10);
							JD_Buf[1] = (RD_SEL_TUNE_R%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TUNE_LR_FLAG = 2;
						}
						else
						{
							if (Main_Disp_Mode==MAIN_MODE_RADIO)
								MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
							else
								MainMode_RD_Sel(1,RD_SEL_SCAN,DISP_ON);
						}
					break;
					case RD_SEL_AUTO:
				   		MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_MODE:
				   		MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_U/10);
							JD_Buf[1] = (RD_SEL_TONE_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_DU_FLAG = 2;
						}
						else
						{
							MainMode_RD_Sel(1,RD_SEL_VOL_U,DISP_ON);
						}
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_U/10);
							JD_Buf[1] = (RD_SEL_VOL_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_DU_FLAG = 2;
						}
						else
						{
							if(AUI1_0B_3210b==3) // Off
								SelIndex = RD_SEL_BAR;
							else
								SelIndex = RD_SEL_POWER;
							RADIO_JOG_KEY_SEL (SelIndex);
						}
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_1C,DISP_ON);
					break;
					case RD_SEL_PWR:
				   		MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_SCAN:
				   		MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
				    case RD_SEL_ATT:
						SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_AIRCON:
						SelIndex = RD_SEL_RADIO;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_RADIO:
						SelIndex = RD_SEL_SPK;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_SPK:
						SelIndex = RD_SEL_CAMERA;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_HOURMETER:
						SelIndex = RD_SEL_MENU;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_BAR:
						SelIndex = RD_SEL_ATT;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						SelIndex = RD_SEL_BAR;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_CAMERA:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_MENU:
						SelIndex = RD_SEL_AIRCON;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				}
			}
		}
		break;
	}
	return true;
}

bool JogDialKey_MAIN_RD(char* param)
{
	char JD_Buf[10] = {0,};
	uint8_t Select_Index;
	RD_SEL_IDX SelIndex;
	if (Main_Disp_Mode==MAIN_MODE_DAB1)
		Select_Index = MainMode_DAB_Select_Index;
	else
		Select_Index = MainMode_RD_Select_Index;
	printf("JogDialKey_MAIN_RD= '0x%X' '%d' '%d'\n",param[0],Select_Index,RD_SEL_TUNE_LR_FLAG);
	Media_RunTime_Cnt=gMainTimerCnt_10m;
	switch (param[0])
	{
		case JOG_DIAL_KEY_LEFT:				//c3Byte0bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_1C:
						SelIndex = RD_SEL_BAR;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
					case RD_SEL_2C:
						MainMode_RD_Sel(1,RD_SEL_1C,DISP_ON);
					break;
					case RD_SEL_3C:
						MainMode_RD_Sel(1,RD_SEL_2C,DISP_ON);
					break;
					case RD_SEL_4C:
						MainMode_RD_Sel(1,RD_SEL_3C,DISP_ON);
					break;
					case RD_SEL_5C:
						SelIndex = RD_SEL_ATT;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
					case RD_SEL_6C:
						MainMode_RD_Sel(1,RD_SEL_5C,DISP_ON);
					break;
					case RD_SEL_7C:
						MainMode_RD_Sel(1,RD_SEL_6C,DISP_ON);
					break;
					case RD_SEL_8C:
						MainMode_RD_Sel(1,RD_SEL_7C,DISP_ON);
					break;
					case RD_SEL_TUNE_L:
					case RD_SEL_TUNE_R:
				   		if (RD_SEL_TUNE_LR_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TUNE_L/10);
							JD_Buf[1] = (RD_SEL_TUNE_L%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TUNE_L_CNT=gMainTimerCnt_10m;
						}
						else
						{
				    		SelIndex = RD_SEL_ATT;
							RADIO_JOG_KEY_SEL (SelIndex);
						}
					break;
					case RD_SEL_AUTO:
						if (Main_Disp_Mode==MAIN_MODE_RADIO)
							MainMode_RD_Sel(1,RD_SEL_TUNE_R,DISP_ON);
						else
							MainMode_RD_Sel(1,RD_SEL_SCAN,DISP_ON);	
					break;
					case RD_SEL_MODE:
				   		MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_4C,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case RD_SEL_PWR:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_SCAN:
				   		MainMode_RD_Sel(1,RD_SEL_TUNE_R,DISP_ON);
					break;
				    case RD_SEL_ATT:
						MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
						SelIndex = RD_SEL_MENU;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_RADIO:
						SelIndex = RD_SEL_AIRCON;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_SPK:
						SelIndex = RD_SEL_RADIO;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_HOURMETER:
						if(AUI1_0B_3210b==3) // Off
							SelIndex = RD_SEL_CAMERA;
						else
							SelIndex = RD_SEL_POWER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_BAR:
						MainMode_RD_Sel(1,RD_SEL_VOL_U,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						SelIndex = RD_SEL_CAMERA;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_CAMERA:
						SelIndex = RD_SEL_SPK;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
				    case RD_SEL_MENU:
				   		SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				}
			}
			else
			{
				switch(Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_1C:
						MainMode_RD_Sel(1,RD_SEL_4C,DISP_ON);
					break;
					case RD_SEL_2C:
						MainMode_RD_Sel(1,RD_SEL_1C,DISP_ON);
					break;
					case RD_SEL_3C:
						MainMode_RD_Sel(1,RD_SEL_2C,DISP_ON);
					break;
					case RD_SEL_4C:
						MainMode_RD_Sel(1,RD_SEL_3C,DISP_ON);
					break;
					case RD_SEL_5C:
						MainMode_RD_Sel(1,RD_SEL_8C,DISP_ON);
					break;
					case RD_SEL_6C:
						MainMode_RD_Sel(1,RD_SEL_5C,DISP_ON);
					break;
					case RD_SEL_7C:
						MainMode_RD_Sel(1,RD_SEL_6C,DISP_ON);
					break;
					case RD_SEL_8C:
						MainMode_RD_Sel(1,RD_SEL_7C,DISP_ON);
					break;
					case RD_SEL_TUNE_L:
					case RD_SEL_TUNE_R:
				   		if (RD_SEL_TUNE_LR_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TUNE_L/10);
							JD_Buf[1] = (RD_SEL_TUNE_L%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TUNE_L_CNT=gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_AUTO:
						if (Main_Disp_Mode==MAIN_MODE_RADIO)
							MainMode_RD_Sel(1,RD_SEL_TUNE_R,DISP_ON);
						else
							MainMode_RD_Sel(1,RD_SEL_SCAN,DISP_ON);
					break;
					case RD_SEL_MODE:
				   		MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case RD_SEL_PWR:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_SCAN:
				   		MainMode_RD_Sel(1,RD_SEL_TUNE_R,DISP_ON);
					break;
				    case RD_SEL_ATT:
						SelIndex = RD_SEL_BAR;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_AIRCON:
						SelIndex = RD_SEL_MENU;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_RADIO:
						SelIndex = RD_SEL_AIRCON;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_SPK:
						SelIndex = RD_SEL_RADIO;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_HOURMETER:
				    break;
				    case RD_SEL_BAR:
						if(AUI1_0B_3210b==3) // Off
							SelIndex = RD_SEL_ATT;
						else
							SelIndex = RD_SEL_POWER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						SelIndex = RD_SEL_ATT;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_CAMERA:
						SelIndex = RD_SEL_SPK;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
				    case RD_SEL_MENU:
				   		SelIndex = RD_SEL_CAMERA;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				}
			}
		}
		break;

		case JOG_DIAL_KEY_RIGHT:			//c3Byte1bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_1C:
						MainMode_RD_Sel(1,RD_SEL_2C,DISP_ON);
					break;
					case RD_SEL_2C:
						MainMode_RD_Sel(1,RD_SEL_3C,DISP_ON);
					break;
					case RD_SEL_3C:
						MainMode_RD_Sel(1,RD_SEL_4C,DISP_ON);
					break;
					case RD_SEL_4C:
						MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;
					case RD_SEL_5C:
						MainMode_RD_Sel(1,RD_SEL_6C,DISP_ON);
					break;
					case RD_SEL_6C:
						MainMode_RD_Sel(1,RD_SEL_7C,DISP_ON);
					break;
					case RD_SEL_7C:
						MainMode_RD_Sel(1,RD_SEL_8C,DISP_ON);
					break;
					case RD_SEL_8C:
						MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;
					case RD_SEL_TUNE_L:
					case RD_SEL_TUNE_R:
				   		if (RD_SEL_TUNE_LR_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TUNE_L/10);
							JD_Buf[1] = (RD_SEL_TUNE_L%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TUNE_L_CNT=gMainTimerCnt_10m;
						}
						else
						{
							if (Main_Disp_Mode==MAIN_MODE_RADIO)
								MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
							else
								MainMode_RD_Sel(1,RD_SEL_SCAN,DISP_ON);
						}
					break;
					case RD_SEL_AUTO:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_MODE:
				   		MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
							SelIndex = RD_SEL_BAR;
							RADIO_JOG_KEY_SEL (SelIndex);
						}
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case RD_SEL_PWR:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_SCAN:
				   		MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;

				    case RD_SEL_ATT:
						MainMode_RD_Sel(1,RD_SEL_5C,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
						SelIndex = RD_SEL_RADIO;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_RADIO:
						SelIndex = RD_SEL_SPK;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_SPK:
						SelIndex = RD_SEL_CAMERA;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_HOURMETER:
						SelIndex = RD_SEL_MENU;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_BAR:
						MainMode_RD_Sel(1,RD_SEL_1C,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_CAMERA:
						if(AUI1_0B_3210b==3) // Off
							SelIndex = RD_SEL_HOURMETER;
						else
							SelIndex = RD_SEL_POWER;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
				    case RD_SEL_MENU:
				   		SelIndex = RD_SEL_AIRCON;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				}
			}
			else
			{
				switch(Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_1C:
						MainMode_RD_Sel(1,RD_SEL_2C,DISP_ON);
					break;
					case RD_SEL_2C:
						MainMode_RD_Sel(1,RD_SEL_3C,DISP_ON);
					break;
					case RD_SEL_3C:
						MainMode_RD_Sel(1,RD_SEL_4C,DISP_ON);
					break;
					case RD_SEL_4C:
						MainMode_RD_Sel(1,RD_SEL_1C,DISP_ON);
					break;
					case RD_SEL_5C:
						MainMode_RD_Sel(1,RD_SEL_6C,DISP_ON);
					break;
					case RD_SEL_6C:
						MainMode_RD_Sel(1,RD_SEL_7C,DISP_ON);
					break;
					case RD_SEL_7C:
						MainMode_RD_Sel(1,RD_SEL_8C,DISP_ON);
					break;
					case RD_SEL_8C:
						MainMode_RD_Sel(1,RD_SEL_5C,DISP_ON);
					break;
					case RD_SEL_TUNE_L:
					case RD_SEL_TUNE_R:
				   		if (RD_SEL_TUNE_LR_FLAG)
						{
							JD_Buf[1] = (RD_SEL_TUNE_L%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TUNE_L_CNT=gMainTimerCnt_10m;
						}
						else
						{
							if (Main_Disp_Mode==MAIN_MODE_RADIO)
				    			MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
							else
								MainMode_RD_Sel(1,RD_SEL_SCAN,DISP_ON);	
						}
					break;
					case RD_SEL_AUTO:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_MODE:
				   		MainMode_RD_Sel(1,RD_SEL_TUNE_L,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case RD_SEL_PWR:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_SCAN:
				   		MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;

				    case RD_SEL_ATT:
						if(AUI1_0B_3210b==3) // Off
							SelIndex = RD_SEL_BAR;
						else
							SelIndex = RD_SEL_POWER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    break;
				    case RD_SEL_AIRCON:
						SelIndex = RD_SEL_RADIO;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_RADIO:
						SelIndex = RD_SEL_SPK;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_SPK:
						SelIndex = RD_SEL_CAMERA;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_HOURMETER:
				    break;
				    case RD_SEL_BAR:
						SelIndex = RD_SEL_ATT;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						SelIndex = RD_SEL_BAR;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_CAMERA:
						SelIndex = RD_SEL_MENU;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
				    case RD_SEL_MENU:
				   		SelIndex = RD_SEL_AIRCON;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				}
			}
		}
		break;
		case JOG_DIAL_KEY_UP:				//c3Byte2bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_1C:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case RD_SEL_2C:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case RD_SEL_3C:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_4C:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_5C:
						MainMode_RD_Sel(1,RD_SEL_1C,DISP_ON);
					break;
					case RD_SEL_6C:
						MainMode_RD_Sel(1,RD_SEL_2C,DISP_ON);
					break;
					case RD_SEL_7C:
						MainMode_RD_Sel(1,RD_SEL_3C,DISP_ON);
					break;
					case RD_SEL_8C:
						MainMode_RD_Sel(1,RD_SEL_4C,DISP_ON);
					break;
					case RD_SEL_TUNE_L:
					case RD_SEL_TUNE_R:
				   		if (RD_SEL_TUNE_LR_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TUNE_L/10);
							JD_Buf[1] = (RD_SEL_TUNE_L%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TUNE_L_CNT=gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_5C,DISP_ON);
					break;
					case RD_SEL_AUTO:
						MainMode_RD_Sel(1,RD_SEL_7C,DISP_ON);
					break;
					case RD_SEL_MODE:
				   		MainMode_RD_Sel(1,RD_SEL_8C,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_LOUD:
						SelIndex = RD_SEL_MENU;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
					case RD_SEL_PWR:
						SelIndex = RD_SEL_MENU;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
					case RD_SEL_SCAN:
				   		MainMode_RD_Sel(1,RD_SEL_6C,DISP_ON);
					break;

				    case RD_SEL_ATT:
						SelIndex = RD_SEL_BAR;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_AIRCON:
						if (Main_Disp_Mode==MAIN_MODE_RADIO)
			    			MainMode_RD_Sel(1,RD_SEL_TUNE_L,DISP_ON);
						else
							MainMode_RD_Sel(1,RD_SEL_SCAN,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
				    break;
				    case RD_SEL_SPK:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
						SelIndex = RD_SEL_ATT;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_BAR:
						SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						SelIndex = RD_SEL_ATT;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_CAMERA:
						MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
				    case RD_SEL_MENU:
				   		MainMode_RD_Sel(1,RD_SEL_TUNE_L,DISP_ON);
				    break;
				}
			}
			else
			{
				switch(Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_1C:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case RD_SEL_2C:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case RD_SEL_3C:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_4C:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_5C:
						MainMode_RD_Sel(1,RD_SEL_1C,DISP_ON);
					break;
					case RD_SEL_6C:
						MainMode_RD_Sel(1,RD_SEL_2C,DISP_ON);
					break;
					case RD_SEL_7C:
						MainMode_RD_Sel(1,RD_SEL_3C,DISP_ON);
					break;
					case RD_SEL_8C:
						MainMode_RD_Sel(1,RD_SEL_4C,DISP_ON);
					break;
					case RD_SEL_TUNE_L:
					case RD_SEL_TUNE_R:
				   		if (RD_SEL_TUNE_LR_FLAG)
						{
							JD_Buf[1] = (RD_SEL_TUNE_L%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TUNE_L_CNT=gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_5C,DISP_ON);
					break;
					case RD_SEL_AUTO:
						MainMode_RD_Sel(1,RD_SEL_7C,DISP_ON);
					break;
					case RD_SEL_MODE:
				   		MainMode_RD_Sel(1,RD_SEL_8C,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_TUNE_L,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_LOUD:
						SelIndex = RD_SEL_MENU;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
					case RD_SEL_PWR:
						SelIndex = RD_SEL_MENU;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
					case RD_SEL_SCAN:
				   		MainMode_RD_Sel(1,RD_SEL_6C,DISP_ON);
					break;

				    case RD_SEL_ATT:
						MainMode_RD_Sel(1,RD_SEL_VOL_U,DISP_ON);
				    break;
				    break;
				    case RD_SEL_AIRCON:
						SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_RADIO:
						SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_SPK:
						SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_HOURMETER:
				    	SelIndex = RD_SEL_BAR;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_BAR:
						MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
						SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
				    case RD_SEL_MENU:
				   		SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				}
			}
		}
		break;
		case JOG_DIAL_KEY_DOWN:				//c3Byte3bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_1C:
						MainMode_RD_Sel(1,RD_SEL_5C,DISP_ON);
					break;
					case RD_SEL_2C:
						MainMode_RD_Sel(1,RD_SEL_6C,DISP_ON);
					break;
					case RD_SEL_3C:
						MainMode_RD_Sel(1,RD_SEL_7C,DISP_ON);
					break;
					case RD_SEL_4C:
						MainMode_RD_Sel(1,RD_SEL_8C,DISP_ON);
					break;
					case RD_SEL_5C:
						MainMode_RD_Sel(1,RD_SEL_TUNE_L,DISP_ON);
					break;
					case RD_SEL_6C:
						if (Main_Disp_Mode==MAIN_MODE_RADIO)
			    			MainMode_RD_Sel(1,RD_SEL_TUNE_L,DISP_ON);
						else
							MainMode_RD_Sel(1,RD_SEL_SCAN,DISP_ON);
					break;
					case RD_SEL_7C:
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_8C:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_TUNE_L:
					case RD_SEL_TUNE_R:
				   		if (RD_SEL_TUNE_LR_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TUNE_L/10);
							JD_Buf[1] = (RD_SEL_TUNE_L%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TUNE_L_CNT=gMainTimerCnt_10m;
						}
						else
						{
				    		SelIndex = RD_SEL_MENU;
							RADIO_JOG_KEY_SEL (SelIndex);
						}
					break;
					case RD_SEL_AUTO:
						SelIndex = RD_SEL_RADIO;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
					case RD_SEL_MODE:
				   		SelIndex = RD_SEL_SPK;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
				    		SelIndex = RD_SEL_CAMERA;
							RADIO_JOG_KEY_SEL (SelIndex);
						}
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
							SelIndex = RD_SEL_CAMERA;
							RADIO_JOG_KEY_SEL (SelIndex);
						}
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_4C,DISP_ON);
					break;
					case RD_SEL_PWR:
						MainMode_RD_Sel(1,RD_SEL_1C,DISP_ON);
					break;
					case RD_SEL_SCAN:
						SelIndex = RD_SEL_AIRCON;
						RADIO_JOG_KEY_SEL (SelIndex);
					break;

				    case RD_SEL_ATT:
						SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_AIRCON:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_SPK:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
						SelIndex = RD_SEL_BAR;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_BAR:
						SelIndex = RD_SEL_ATT;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						SelIndex = RD_SEL_BAR;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_CAMERA:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
				    case RD_SEL_MENU:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				}
			}
			else
			{
				switch(Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_1C:
						MainMode_RD_Sel(1,RD_SEL_5C,DISP_ON);
					break;
					case RD_SEL_2C:
						MainMode_RD_Sel(1,RD_SEL_6C,DISP_ON);
					break;
					case RD_SEL_3C:
						MainMode_RD_Sel(1,RD_SEL_7C,DISP_ON);
					break;
					case RD_SEL_4C:
						MainMode_RD_Sel(1,RD_SEL_8C,DISP_ON);
					break;
					case RD_SEL_5C:
						MainMode_RD_Sel(1,RD_SEL_TUNE_L,DISP_ON);
					break;
					case RD_SEL_6C:
						if (Main_Disp_Mode==MAIN_MODE_RADIO)
			    			MainMode_RD_Sel(1,RD_SEL_TUNE_L,DISP_ON);
						else
							MainMode_RD_Sel(1,RD_SEL_SCAN,DISP_ON);
					break;
					case RD_SEL_7C:
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_8C:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_TUNE_L:
					case RD_SEL_TUNE_R:
				   		if (RD_SEL_TUNE_LR_FLAG)
						{
							JD_Buf[1] = (RD_SEL_TUNE_L%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TUNE_L_CNT=gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;
					case RD_SEL_AUTO:
						MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
					break;
					case RD_SEL_MODE:
				   		MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
							SelIndex = RD_SEL_BAR;
							RADIO_JOG_KEY_SEL (SelIndex);
						}
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
							SelIndex = RD_SEL_ATT;
							RADIO_JOG_KEY_SEL (SelIndex);
						}
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_4C,DISP_ON);
					break;
					case RD_SEL_PWR:
						MainMode_RD_Sel(1,RD_SEL_1C,DISP_ON);
					break;
					case RD_SEL_SCAN:
				   		MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;

				    case RD_SEL_ATT:
						SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    break;
				    case RD_SEL_AIRCON:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_SPK:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				    	SelIndex = RD_SEL_RADIO;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_BAR:
						SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				    	SelIndex = RD_SEL_HOURMETER;
						RADIO_JOG_KEY_SEL (SelIndex);
				    break;
				    case RD_SEL_CAMERA:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
				    case RD_SEL_MENU:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				}
			}
		}
		break;
		case JOG_DIAL_KEY_LEFT_OFF:
		case JOG_DIAL_KEY_RIGHT_OFF:
		case JOG_DIAL_KEY_UP_OFF:
		case JOG_DIAL_KEY_DOWN_OFF:
		{
	//	printf("JOG_DIAL_KEY_OFF= '%d''%d''%d' 0x%x 0x%x\n",RD_SEL_TUNE_LR_FLAG,RD_SEL_TONE_DU_FLAG,RD_SEL_VOL_DU_FLAG,Main_CLI2[3],Main_CLI2[4]);
#if 1		
			if (RD_SEL_TUNE_LR_FLAG)
			{
				Main_CLI2[3] = Main_CLI2[3] & 0x3F;// 0xF3;
			}
			if (RD_SEL_TONE_DU_FLAG)
			{
				Main_CLI2[3] = Main_CLI2[3] & 0xF3;// 0x3F;
			}
			if (RD_SEL_VOL_DU_FLAG)
			{
				Main_CLI2[4] = Main_CLI2[4] & 0xFC;
			}
#endif 	// 2024-01-18  ��Ű �Ҷ� ª�� ������ �����ϱ����� ���µȴ�.	
		}
		break;
		case JOG_DIAL_KEY_ENTER_ON:
		{
			switch(Select_Index)
			{
				case RD_SEL_HOURMETER:
				{
					if (KEY_ENTER_HM_CNT == 0)
						KEY_ENTER_HM_CNT = gMainTimerCnt_10m;
				}
				break;
				case RD_SEL_ATT:
				{
					if (KEY_ENTER_ATT_CNT == 0)
						KEY_ENTER_ATT_CNT = gMainTimerCnt_10m;
				}
				break;
				case RD_SEL_1C:
				case RD_SEL_2C:
				case RD_SEL_3C:
				case RD_SEL_4C:
				case RD_SEL_5C:
				case RD_SEL_6C:
				case RD_SEL_7C:
				case RD_SEL_8C:
				case RD_SEL_AUTO:
				case RD_SEL_SCAN:
				{
					//chpark.11.15
					JD_Buf[0] = (Select_Index/10);
					JD_Buf[1] = (Select_Index%10);
					printf("==========>RD_MAIN_DnBt 2\n");
					RD_MAIN_DnBt(NULL, JD_Buf);
				}
				break;
			}
		}
		break;
		case JOG_DIAL_KEY_ENTER_OFF:
//		case JOG_DIAL_KEY_NONE:
		{
	//	printf("JOG_DIAL_KEY_ENTER_OFF = '%d''%d''%d' 0x%x 0x%x\n",RD_SEL_TUNE_LR_FLAG,RD_SEL_TONE_DU_FLAG,RD_SEL_VOL_DU_FLAG,Main_CLI2[3],Main_CLI2[4]);
//			printf("JOG_DIAL_KEY_ENTER_OFF= '%d' 0x%x\n",MainMode_RD_Select_Index,KEY_ENTER_HM_CNT);
			switch(Select_Index)
			{
				case RD_SEL_TUNE_L:
				case RD_SEL_TUNE_R:
				{
					if (RD_SEL_TUNE_LR_FLAG==0)
					{
						RD_SEL_TUNE_LR_FLAG = 1;
					}
					else
					{
						RD_SEL_TUNE_LR_FLAG = 0;
						Main_CLI2[3] = 0;
						Main_CLI2[4] = 0;
						RD_SEL_TUNE_R_CNT = LONG_KEY_TIME;
						RD_SEL_TUNE_L_CNT = LONG_KEY_TIME;
						MainMode_RD_Sel(0,RD_SEL_TUNE_R,DISP_ON);
					}
				}
				break;

				case RD_SEL_TONE_D:
				case RD_SEL_TONE_U:
				{
					if (RD_SEL_TONE_DU_FLAG==0)
					{
						RD_SEL_TONE_DU_FLAG = 1;
					}
					else
					{
						RD_SEL_TONE_DU_FLAG = 0;
						Main_CLI2[3] = 0;
						Main_CLI2[4] = 0;
						RD_SEL_TONE_D_CNT = LONG_KEY_TIME;
						RD_SEL_TONE_U_CNT = LONG_KEY_TIME;
						MainMode_RD_Sel(0,RD_SEL_TONE_U,DISP_ON);
					}
				}
				break;
				case RD_SEL_VOL_D:
				case RD_SEL_VOL_U:
				{
					if (RD_SEL_VOL_DU_FLAG==0)
					{
						RD_SEL_VOL_DU_FLAG = 1;
					}
					else
					{
						RD_SEL_VOL_DU_FLAG = 0;
						Main_CLI2[3] = 0;
						Main_CLI2[4] = 0;
						RD_SEL_VOL_U_CNT = LONG_KEY_TIME;
						RD_SEL_VOL_D_CNT = LONG_KEY_TIME;
						MainMode_RD_Sel(0,RD_SEL_VOL_U,DISP_ON);
					}
				}
				break;
				case RD_SEL_1C:
				case RD_SEL_2C:
				case RD_SEL_3C:
				case RD_SEL_4C:
				case RD_SEL_5C:
				case RD_SEL_6C:
				case RD_SEL_7C:
				case RD_SEL_8C:
				case RD_SEL_AUTO:
				case RD_SEL_SCAN:
				case RD_SEL_PWR:
				case RD_SEL_LOUD:
				case RD_SEL_MODE:
				{
					JD_Buf[0] = (Select_Index/10);
					JD_Buf[1] = (Select_Index%10);
					RD_MAIN_UpBt(NULL, JD_Buf);
				}
				break;
				case RD_SEL_ATT:
				{
					if (KEY_ENTER_ATT_CNT != 0xFFFFF000)
					{
						JD_Buf[1] = 0;
						if (AUX1CurrentOutMode==0)
							JD_Buf[0] = 2;
						else if (AUX1CurrentOutMode==1)
							JD_Buf[0] = 3;
						else if (AUX1CurrentOutMode==2)
							JD_Buf[0] = 1;
						Main_ATTBt_Func(NULL, JD_Buf);
					}
					KEY_ENTER_ATT_CNT = 0;
				}
				break;
				case RD_SEL_AIRCON:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_ACBt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_RADIO:
				{
				}
				break;
				case RD_SEL_SPK:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_SPK_Bt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_HOURMETER:
				{
					if (KEY_ENTER_HM_CNT != 0xFFFFF000)
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_HMeterBt_Func(NULL, JD_Buf);
					}
					else
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_HMeterBt_Func(NULL, JD_Buf);
					}
					KEY_ENTER_HM_CNT = 0;
				}
				break;
				case RD_SEL_BAR:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_BarBt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_POWER:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_PwrBt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_CAMERA:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					MainCamera(NULL, JD_Buf);
				}
				break;
				case RD_SEL_MENU:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					MainHome(NULL, JD_Buf);
				}
				break;
			}
		}
	}
	return true;
}

//chpark.06.21
bool JogDialSwitch_MAIN_DAB2(char* param)
{
	char JD_Buf[10] = {0,};
	printf("JogDialSwitch_MAIN_DAB2= '0x%X' '%d'\n",param[0],MainMode_DAB2_Select_Index);

	return true;
}
//


//chpark.24.10.08
bool JogDialKey_MAIN_DAB2(char* param)
{
	char JD_Buf[10] = {0,};
	printf("JogDialKey_MAIN_DAB2= '0x%X' '%d'\n",param[0],MainMode_DAB2_Select_Index);
	Media_RunTime_Cnt=gMainTimerCnt_10m;

	//==============================================
	//chpark.06.21
	if(MainMode_DAB2_Select_Index == RD_SEL_SCROLL)
	{		
		if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
		{
			JogDialKey_Scroll_DAB2(param);
		}
		else
		{
			JogDialKey_Scroll_DAB2_H(param);
		}

		return true;					
	}
	//==============================================

	
	uint8_t tDAB_PrgCnt = 0;
	if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
		tDAB_PrgCnt = DAB_PRG_CNT_H;
	else
		tDAB_PrgCnt = DAB_PRG_CNT_V;
	
	switch (param[0])
	{
		case JOG_DIAL_KEY_LEFT:				//c3Byte0bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_DAB2_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_AUTO:
						//MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
					break;
					case RD_SEL_MODE:
						//MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
					break;
					case DAB2_SEL_1C://chpark.24.10.28
					case DAB2_SEL_2C:
					case DAB2_SEL_3C:
					case DAB2_SEL_4C:
					case DAB2_SEL_5C:
					case DAB2_SEL_6C:
					case DAB2_SEL_7C:
					case DAB2_SEL_8C:
						MainMode_DAB2_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MAIN_SEL_BAR,DISP_ON);
					break;
					case RD_SEL_BAR: //chpark.24.10.28
					case RD_SEL_ATT:
						MainMode_DAB2_Select_Index = DAB2_SEL_1C;
						MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
				    break;
				   

					case RD_SEL_HOURMETER://chpark.24.10.28
				   		if(AUI1_0B_3210b==3) // Off
							MainMode_DAB2_Select_Index = RD_SEL_CAMERA;
						else
							MainMode_DAB2_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_DAB2_Select_Index,DISP_ON);
				    break;
					case RD_SEL_POWER:
				   		MainMode_DAB2_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(RD_SEL_CAMERA,DISP_ON);
				    break;
					case RD_SEL_MENU: //chpark.24.10.28
				   		MainMode_DAB2_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(RD_SEL_HOURMETER,DISP_ON);
				    break;
					case RD_SEL_CAMERA:
				   		MainMode_DAB2_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(RD_SEL_SPK,DISP_ON);
				    break;
					case RD_SEL_SPK:
				   		MainMode_DAB2_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(RD_SEL_RADIO,DISP_ON);
				    break;
					case RD_SEL_RADIO:
				   		MainMode_DAB2_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(RD_SEL_AIRCON,DISP_ON);
				    break;
					case RD_SEL_AIRCON:
				   		MainMode_DAB2_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(RD_SEL_MENU,DISP_ON);
				    break;
					
					default:
					break;
				}
			}
			else
			{
				switch(MainMode_DAB2_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_AUTO:
						//MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
					break;
					case RD_SEL_MODE:
						//MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
					break;
					case DAB2_SEL_1C:
					case DAB2_SEL_2C:
					case DAB2_SEL_3C:
					case DAB2_SEL_4C:
					case DAB2_SEL_5C:
					case DAB2_SEL_6C:
					case DAB2_SEL_7C:
					case DAB2_SEL_8C:
						//MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					 case RD_SEL_POWER:
				   		MainMode_DAB2_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(RD_SEL_ATT,DISP_ON);
				    break;
				    case RD_SEL_ATT:
				   		MainMode_DAB2_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MAIN_SEL_BAR,DISP_ON);
				    break;
					case RD_SEL_BAR://chpark.24.10.28
				   		if(AUI1_0B_3210b==3) // Off
							MainMode_DAB2_Select_Index = RD_SEL_ATT;
						else
							MainMode_DAB2_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_DAB2_Select_Index,DISP_ON);
				    break;
					case RD_SEL_MENU:
				   		MainMode_DAB2_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(RD_SEL_CAMERA,DISP_ON);
				    break;
					case RD_SEL_CAMERA:
				   		MainMode_DAB2_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(RD_SEL_SPK,DISP_ON);
				    break;
					case RD_SEL_SPK:
				   		MainMode_DAB2_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(RD_SEL_RADIO,DISP_ON);
				    break;
					case RD_SEL_RADIO:
				   		MainMode_DAB2_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(RD_SEL_AIRCON,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				   		MainMode_DAB2_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(RD_SEL_MENU,DISP_ON);
				    break;
				   
				}
			}
		}
		break;
		case JOG_DIAL_KEY_RIGHT:			//c3Byte1bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_DAB2_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_DAB2_Select_Index = RD_SEL_AUTO;
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					
					case DAB2_SEL_1C: //chpark.24.10.28
					case DAB2_SEL_2C:
					case DAB2_SEL_3C:
					case DAB2_SEL_4C:
					case DAB2_SEL_5C:
					case DAB2_SEL_6C:
					case DAB2_SEL_7C:
					case DAB2_SEL_8C:
						MainMode_DAB2_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MAIN_SEL_BAR,DISP_ON);
					break;
					//chpark.24.10.28
					case RD_SEL_BAR:
					case RD_SEL_ATT:
				   		MainMode_DAB2_Select_Index = DAB2_SEL_1C;
						MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
				    break;

					case RD_SEL_MENU:
				   		MainMode_DAB2_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(RD_SEL_AIRCON,DISP_ON);
				    break;
					case RD_SEL_AIRCON:
				   		MainMode_DAB2_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(RD_SEL_RADIO,DISP_ON);
				    break;
					case RD_SEL_RADIO:
				   		MainMode_DAB2_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(RD_SEL_SPK,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				   		MainMode_DAB2_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(RD_SEL_CAMERA,DISP_ON);
				    break;
					case RD_SEL_CAMERA:
				   		if(AUI1_0B_3210b==3) // Off
							MainMode_DAB2_Select_Index = RD_SEL_HOURMETER;
						else
							MainMode_DAB2_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_DAB2_Select_Index,DISP_ON);
				    break;
					case RD_SEL_POWER:
				   		MainMode_DAB2_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(RD_SEL_HOURMETER,DISP_ON);
				    break;

					case RD_SEL_HOURMETER: //chpark.24.10.28
						MainMode_DAB2_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(RD_SEL_MENU,DISP_ON);
					break;

				}
			}
			else
			{
				switch(MainMode_DAB2_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_DAB2_Select_Index = RD_SEL_AUTO;
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_AUTO:
						//MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
					break;
					case RD_SEL_MODE:
						//MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
					break;
					case DAB2_SEL_1C:
					case DAB2_SEL_2C:
					case DAB2_SEL_3C:
					case DAB2_SEL_4C:
					case DAB2_SEL_5C:
					case DAB2_SEL_6C:
					case DAB2_SEL_7C:
					case DAB2_SEL_8C:
						//MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;

					case RD_SEL_POWER:
				   		MainMode_DAB2_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(RD_SEL_BAR,DISP_ON);
				    break;
					case RD_SEL_BAR:
				   		MainMode_DAB2_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(RD_SEL_ATT,DISP_ON);
				    break;
					case RD_SEL_ATT:
				   		if(AUI1_0B_3210b==3) // Off
							MainMode_DAB2_Select_Index = RD_SEL_BAR;
						else
							MainMode_DAB2_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_DAB2_Select_Index,DISP_ON);
				    break;

					case RD_SEL_MENU:
				   		MainMode_DAB2_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(RD_SEL_AIRCON,DISP_ON);
				    break;
					 case RD_SEL_AIRCON:
				   		MainMode_DAB2_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(RD_SEL_RADIO,DISP_ON);
				    break;
					case RD_SEL_RADIO:
				   		MainMode_DAB2_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(RD_SEL_SPK,DISP_ON);
				    break;
					case RD_SEL_SPK:
				   		MainMode_DAB2_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(RD_SEL_CAMERA,DISP_ON);
				    break;
					case RD_SEL_CAMERA:
				   		MainMode_DAB2_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(RD_SEL_MENU,DISP_ON);
				    break;

				   
				}
			}
		}
		break;
		case JOG_DIAL_KEY_UP:				//c3Byte2bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_DAB2_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_AUTO: //chpark.24.10.28
						MainMode_DAB2_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(RD_SEL_MENU,DISP_ON);
					break;
					case RD_SEL_MODE:
				   		//MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					
					case DAB2_SEL_1C:
					case DAB2_SEL_2C:
					case DAB2_SEL_3C:
					case DAB2_SEL_4C:
					case DAB2_SEL_5C:
					case DAB2_SEL_6C:
					case DAB2_SEL_7C:
					case DAB2_SEL_8C:
						MainMode_DAB2_Select_Index = RD_SEL_AUTO;
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
						
				    case RD_SEL_ATT:
				   		MainMode_DAB2_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MAIN_SEL_BAR,DISP_ON);
				    break;
					case RD_SEL_HOURMETER:
				   		MainMode_DAB2_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(RD_SEL_ATT,DISP_ON);
				    break;
					 case RD_SEL_POWER:
				   		MainMode_DAB2_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(RD_SEL_ATT,DISP_ON);
				    break;

					case RD_SEL_MENU:
				    case RD_SEL_AIRCON:
					case RD_SEL_RADIO:
					case RD_SEL_SPK:
					case RD_SEL_CAMERA:
				   		MainMode_DAB2_Select_Index = DAB2_SEL_1C;
						MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
				    break;
				   	
				    case RD_SEL_TIMEDATE:
				   		MainMode_DAB2_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MAIN_SEL_BAR,DISP_ON);
				    break;

					case RD_SEL_BAR: //chpark.24.10.28
				   		MainMode_DAB2_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(RD_SEL_HOURMETER,DISP_ON);
				    break;

					default:
					//error
					break;
				   
				}
			}
			else
			{
				switch(MainMode_DAB2_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_DAB2_Select_Index = RD_SEL_AUTO;
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;

					case RD_SEL_AUTO: //chpark.24.10.28
						MainMode_DAB2_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(RD_SEL_MENU,DISP_ON);
					break;
				
					case DAB2_SEL_1C:
					case DAB2_SEL_2C:
					case DAB2_SEL_3C:
					case DAB2_SEL_4C:
					case DAB2_SEL_5C:
					case DAB2_SEL_6C:
					case DAB2_SEL_7C:
					case DAB2_SEL_8C:
						MainMode_DAB2_Select_Index = RD_SEL_AUTO;
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;

					case RD_SEL_POWER:
				    case RD_SEL_ATT:
					case RD_SEL_BAR:
				   		MainMode_DAB2_Select_Index = DAB2_SEL_1C;
						MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
				    break;

					case RD_SEL_HOURMETER:
				   		MainMode_DAB2_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(RD_SEL_BAR,DISP_ON);
				    break;

					case RD_SEL_MENU:
				    case RD_SEL_AIRCON:
					case RD_SEL_RADIO:
					case RD_SEL_SPK:
					case RD_SEL_CAMERA:
				   		MainMode_DAB2_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(RD_SEL_HOURMETER,DISP_ON);
				    break;
				    
					default:
					//error
					break;
				   		
				}
			}
		}
		break;
		case JOG_DIAL_KEY_DOWN:				//c3Byte3bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_DAB2_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_AUTO:
						MainMode_DAB2_Select_Index = DAB2_SEL_1C;
				   		MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
					break;
					case RD_SEL_MODE:
				   		MainMode_DAB2_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MAIN_SEL_BAR,DISP_ON);
					break;
					case DAB2_SEL_1C:
					case DAB2_SEL_2C:
					case DAB2_SEL_3C:
					case DAB2_SEL_4C:
					case DAB2_SEL_5C:
					case DAB2_SEL_6C:
					case DAB2_SEL_7C:
					case DAB2_SEL_8C:
						MainMode_DAB2_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(RD_SEL_MENU,DISP_ON);
					break;

					case RD_SEL_BAR:
				   		MainMode_DAB2_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(RD_SEL_ATT,DISP_ON);
				    break;
			
				    case RD_SEL_ATT:
				   		MainMode_DAB2_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(RD_SEL_HOURMETER,DISP_ON);
				    break;

					case RD_SEL_POWER:
				   		MainMode_DAB2_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(RD_SEL_BAR,DISP_ON);
				    break;

					case RD_SEL_HOURMETER: //chpark.24.10.28
				   		MainMode_DAB2_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(RD_SEL_BAR,DISP_ON);
				    break;

					case RD_SEL_MENU: //chpark.24.10.28
				    case RD_SEL_AIRCON:
					case RD_SEL_RADIO:
					case RD_SEL_SPK:
					case RD_SEL_CAMERA:
				   		MainMode_DAB2_Select_Index = RD_SEL_AUTO; 
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
				    break;
				    
				}
			}
			else
			{
				switch(MainMode_DAB2_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_AUTO:
						MainMode_DAB2_Select_Index = DAB2_SEL_1C;
				   		MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
					break;
					case RD_SEL_MODE:
				   		//MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case DAB2_SEL_1C:
					case DAB2_SEL_2C:
					case DAB2_SEL_3C:
					case DAB2_SEL_4C:
					case DAB2_SEL_5C:
					case DAB2_SEL_6C:
					case DAB2_SEL_7C:
					case DAB2_SEL_8C:
						MainMode_DAB2_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(RD_SEL_BAR,DISP_ON);
					break;
					
					case RD_SEL_BAR:
				   		MainMode_DAB2_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(RD_SEL_HOURMETER,DISP_ON);
				    break;
				    case RD_SEL_ATT:
				   		MainMode_DAB2_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(RD_SEL_HOURMETER,DISP_ON);
				    break;
					case RD_SEL_POWER:
				   		MainMode_DAB2_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(RD_SEL_HOURMETER,DISP_ON);
				    break;

					case RD_SEL_HOURMETER:
				   		MainMode_DAB2_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(RD_SEL_RADIO,DISP_ON);
				    break;

					case RD_SEL_MENU: //chpark.24.10.28
				    case RD_SEL_AIRCON:
					case RD_SEL_RADIO:
					case RD_SEL_SPK:
					case RD_SEL_CAMERA:
				   		MainMode_DAB2_Select_Index = RD_SEL_AUTO; 
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
				    break;

					default:
					break;

				}
			}
		}
		break;
		case JOG_DIAL_KEY_ENTER_ON:
		{
			switch(MainMode_DAB2_Select_Index)
			{
				case RD_SEL_SCAN:
				case RD_SEL_AUTO:
				{
					JD_Buf[0] = (MainMode_DAB2_Select_Index/10);
					JD_Buf[1] = (MainMode_DAB2_Select_Index%10);
					printf("==========>RD_MAIN_DnBt 1\n");
					RD_MAIN_DnBt(NULL, JD_Buf);
				}
				break;
				case RD_SEL_HOURMETER:
				{
					if (KEY_ENTER_HM_CNT == 0)
						KEY_ENTER_HM_CNT = gMainTimerCnt_10m;
				}
				break;
				case RD_SEL_ATT:
				{
					if (KEY_ENTER_ATT_CNT == 0)
						KEY_ENTER_ATT_CNT = gMainTimerCnt_10m;
				}
				break;
			}
		}
		break;
		case JOG_DIAL_KEY_ENTER_OFF:
//		case JOG_DIAL_KEY_NONE:
		{
			char JD_Buf[10] = {0,};
//			printf("JOG_DIAL_KEY_ENTER_OFF= '%d' 0x%x\n",MainMode_DAB2_Select_Index,KEY_ENTER_HM_CNT);
			switch(MainMode_DAB2_Select_Index)
			{
				case DAB2_SEL_1C:
				case DAB2_SEL_2C:
				case DAB2_SEL_3C:
				case DAB2_SEL_4C:
				case DAB2_SEL_5C:
				case DAB2_SEL_6C:
				case DAB2_SEL_7C:
				case DAB2_SEL_8C:
				{
					//chpark.11.15
					//If there is a touch on the scroll, the entire selection is canceled.
    				MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_OFF);     //chpark.06.21 

					//Make the jog signal to be passed to the scroll.
					MainMode_DAB2_Select_Index = RD_SEL_SCROLL;//chpark.06.21

					//chpark.06.21
					///3333=>set Focus on PTY position.(Set to 0 if there is no  PTY.)
					//-1=>set No Focus in Scroll
        			//
					if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
					{
						DAB2ListBox_Select(3333);
					}
					else
					{
						DAB2ListBoxH_Select(3333);
					}
					//

				}
				break;	

				case RD_SEL_AUTO:
				case RD_SEL_PWR:
				case RD_SEL_MODE:
				{
					JD_Buf[0] = (MainMode_DAB2_Select_Index/10);
					JD_Buf[1] = (MainMode_DAB2_Select_Index%10);
					RD_MAIN_UpBt(NULL, JD_Buf);
				}
				break;
				case RD_SEL_ATT:
				{
					if (KEY_ENTER_ATT_CNT != 0xFFFFF000)
					{
						JD_Buf[1] = 0;
						if (AUX1CurrentOutMode==0)
							JD_Buf[0] = 2;
						else if (AUX1CurrentOutMode==1)
							JD_Buf[0] = 3;
						else if (AUX1CurrentOutMode==2)
							JD_Buf[0] = 1;
						Main_ATTBt_Func(NULL, JD_Buf);
					}
					KEY_ENTER_ATT_CNT = 0;
				}
				break;
				case RD_SEL_AIRCON:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_ACBt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_RADIO:
				{
				}
				break;
				case RD_SEL_SPK:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_SPK_Bt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_HOURMETER:
				{
					if (KEY_ENTER_HM_CNT != 0xFFFFF000)
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_HMeterBt_Func(NULL, JD_Buf);
					}
					else
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_HMeterBt_Func(NULL, JD_Buf);
					}
					KEY_ENTER_HM_CNT = 0;
				}
				break;
				case RD_SEL_BAR:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_BarBt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_POWER:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_PwrBt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_CAMERA:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					MainCamera(NULL, JD_Buf);
				}
				break;
				case RD_SEL_MENU:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					MainHome(NULL, JD_Buf);
				}
				break;
			}
		}
	}
	return true;
}

bool JogDialRot_MAIN_DAB2(char* param)
{
	char JD_Buf[10] = {0,};
	printf("JogDialRot_MAIN_DAB2= '0x%X' '%d'\n",param[0],MainMode_DAB2_Select_Index);
	Media_RunTime_Cnt=gMainTimerCnt_10m;
	uint8_t tDAB_PrgCnt = 0;
	if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
		tDAB_PrgCnt = DAB_PRG_CNT_H;
	else
		tDAB_PrgCnt = DAB_PRG_CNT_V;

	//==============================================
	//chpark.06.21
	if(MainMode_DAB2_Select_Index == RD_SEL_SCROLL)
	{		
		if ((RotationDir()==PORTRAIT_1)||(RotationDir()==PORTRAIT_2))
		{
			JogDialRot_Scroll_DAB2(param);
		}
		else
		{
			JogDialRot_Scroll_DAB2_H(param);
		}

		return true;					
	}
	//==============================================

	switch (param[0])
	{
		case JOG_DIAL_ROT_LEFT:
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_DAB2_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_DAB2_Select_Index = RD_SEL_AUTO;
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_AUTO:
						MainMode_DAB2_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(RD_SEL_HOURMETER,DISP_ON);
					break;
					 case RD_SEL_HOURMETER://chpark.24.10.28
				   		if(AUI1_0B_3210b==3) // Off
							MainMode_DAB2_Select_Index = RD_SEL_ATT;
						else
							MainMode_DAB2_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_DAB2_Select_Index,DISP_ON);
				    break;
					 case RD_SEL_POWER:
				   		MainMode_DAB2_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(RD_SEL_ATT,DISP_ON);
				    break;
					 case RD_SEL_ATT:
				   		MainMode_DAB2_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(RD_SEL_BAR,DISP_ON);
				    break;
					case RD_SEL_BAR:
				   		MainMode_DAB2_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(RD_SEL_CAMERA,DISP_ON);
				    break;
					 case RD_SEL_CAMERA:
				   		MainMode_DAB2_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(RD_SEL_SPK,DISP_ON);
				    break;
					case RD_SEL_SPK:
				   		MainMode_DAB2_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(RD_SEL_RADIO,DISP_ON);
				    break;
					case RD_SEL_RADIO:
				   		MainMode_DAB2_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(RD_SEL_AIRCON,DISP_ON);
				    break;
					case RD_SEL_AIRCON:
				   		MainMode_DAB2_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(RD_SEL_MENU,DISP_ON);
				    break;
					case RD_SEL_MENU:
						MainMode_DAB2_Select_Index = DAB2_SEL_1C;
				   		MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
				    break;
					case RD_SEL_MODE:
				   		MainMode_DAB2_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MAIN_SEL_AIRCON,DISP_ON);
					break;
					case DAB2_SEL_1C:
						//MainMode_RD_Sel(1,DAB2_SEL_2C,DISP_ON);
						MainMode_DAB2_Select_Index = RD_SEL_AUTO;
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;


					case DAB2_SEL_2C:
						MainMode_RD_Sel(1,DAB2_SEL_3C,DISP_ON);
					break;
					case DAB2_SEL_3C:
						MainMode_RD_Sel(1,DAB2_SEL_4C,DISP_ON);
					break;
					case DAB2_SEL_4C:
						MainMode_RD_Sel(1,DAB2_SEL_5C,DISP_ON);
					break;
					case DAB2_SEL_5C:
						MainMode_RD_Sel(1,DAB2_SEL_6C,DISP_ON);
					break;
					case DAB2_SEL_6C:
						MainMode_RD_Sel(1,DAB2_SEL_7C,DISP_ON);
					break;
					case DAB2_SEL_7C:
						MainMode_RD_Sel(1,DAB2_SEL_8C,DISP_ON);
					break;
					case DAB2_SEL_8C:
						
					break;
				
				    case RD_SEL_TIMEDATE:
				   		MainMode_RD_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MAIN_SEL_BAR,DISP_ON);
				    break;
				    
				}
			}
			else
			{
				switch(MainMode_DAB2_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_DAB2_Select_Index = RD_SEL_AUTO;
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_AUTO:
				   		MainMode_DAB2_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(RD_SEL_CAMERA,DISP_ON);
					break;
					
					case RD_SEL_CAMERA:
				   		MainMode_DAB2_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(RD_SEL_SPK,DISP_ON);
				    break;
					case RD_SEL_SPK:
				   		MainMode_DAB2_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(RD_SEL_RADIO,DISP_ON);
				    break;
					 case RD_SEL_RADIO:
				   		MainMode_DAB2_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(RD_SEL_AIRCON,DISP_ON);
				    break;
					case RD_SEL_AIRCON:
				   		MainMode_DAB2_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(RD_SEL_MENU,DISP_ON);
				    break;
					case RD_SEL_MENU:
				   		MainMode_DAB2_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(RD_SEL_HOURMETER,DISP_ON);
				    break;
					
					case RD_SEL_HOURMETER:
				   		MainMode_DAB2_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(RD_SEL_ATT,DISP_ON);
				    break;
					
					case RD_SEL_ATT:
				   		MainMode_DAB2_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(RD_SEL_BAR,DISP_ON);
				    break;
					
					case RD_SEL_BAR: //chpark.24.10.28
				   		if(AUI1_0B_3210b==3) // Off
						{
							MainMode_DAB2_Select_Index = DAB2_SEL_1C;
							MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
						}
						else
						{
							MainMode_DAB2_Select_Index = RD_SEL_POWER;
							MainLayer_ButtonSel(MainMode_DAB2_Select_Index,DISP_ON);
						}
				    break;

					case RD_SEL_POWER:
						MainMode_DAB2_Select_Index = DAB2_SEL_1C;
				   		MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
				    break;
					case DAB2_SEL_1C:
						//MainMode_RD_Sel(1,DAB2_SEL_2C,DISP_ON);
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);//chpark.06.21
					break;

					//no need
					case RD_SEL_MODE:	
				   		MainMode_DAB2_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MAIN_SEL_AIRCON,DISP_ON);
					break;
					
					case DAB2_SEL_2C:
						MainMode_RD_Sel(1,DAB2_SEL_3C,DISP_ON);
					break;
					case DAB2_SEL_3C:
						MainMode_RD_Sel(1,DAB2_SEL_4C,DISP_ON);
					break;
					case DAB2_SEL_4C:
						MainMode_RD_Sel(1,DAB2_SEL_5C,DISP_ON);
					break;
					case DAB2_SEL_5C:
						MainMode_RD_Sel(1,DAB2_SEL_6C,DISP_ON);
					break;
					case DAB2_SEL_6C:
						MainMode_RD_Sel(1,DAB2_SEL_7C,DISP_ON);
					break;
					case DAB2_SEL_7C:
							//DAB2_PTY_Index = DAB2_PTY_Index+tDAB_PrgCnt;
							//if (DAB2_PTY_Index>=29)
							//	DAB2_PTY_Index = DAB2_PTY_Index%tDAB_PrgCnt;
							//MainMode_DAB2_PTY_Disp(0,DAB2_PTY_Index,DISP_ON);
					break;
					case DAB2_SEL_8C:
						//DAB2_PTY_Index = DAB2_PTY_Index+tDAB_PrgCnt;
						//if (DAB2_PTY_Index>=29)
						//	DAB2_PTY_Index = DAB2_PTY_Index%tDAB_PrgCnt;
						//MainMode_DAB2_PTY_Disp(0,DAB2_PTY_Index,DISP_ON);
					break;
				    case RD_SEL_TIMEDATE:
				   		MainMode_RD_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MAIN_SEL_BAR,DISP_ON);
				    break;
				}
			}
		}
		break;
		case JOG_DIAL_ROT_RIGHT:
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_DAB2_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_DAB2_Select_Index = RD_SEL_AUTO;
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_AUTO:
						MainMode_DAB2_Select_Index = DAB2_SEL_1C;
				   		MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
					break;
					case RD_SEL_MODE:
				   		MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case DAB2_SEL_1C:
						//MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);//chpark.06.21
						MainMode_DAB2_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(RD_SEL_MENU,DISP_ON);
					break;
					case DAB2_SEL_2C:
						MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
					break;
					case DAB2_SEL_3C:
						MainMode_RD_Sel(1,DAB2_SEL_2C,DISP_ON);
					break;
					case DAB2_SEL_4C:
						MainMode_RD_Sel(1,DAB2_SEL_3C,DISP_ON);
					break;
					case DAB2_SEL_5C:
						MainMode_RD_Sel(1,DAB2_SEL_4C,DISP_ON);
					break;
					case DAB2_SEL_6C:
						MainMode_RD_Sel(1,DAB2_SEL_5C,DISP_ON);
					break;
					case DAB2_SEL_7C:
						MainMode_RD_Sel(1,DAB2_SEL_6C,DISP_ON);
					break;
					case DAB2_SEL_8C:
						MainMode_RD_Sel(1,DAB2_SEL_7C,DISP_ON);
					break;

					case RD_SEL_MENU:
				   		MainMode_DAB2_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(RD_SEL_AIRCON,DISP_ON);
				    break;
					 case RD_SEL_AIRCON:
				   		MainMode_DAB2_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(RD_SEL_RADIO,DISP_ON);
				    break;

					case RD_SEL_RADIO:
				   		MainMode_DAB2_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(RD_SEL_SPK,DISP_ON);
				    break;

					case RD_SEL_SPK:
				   		MainMode_DAB2_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(RD_SEL_CAMERA,DISP_ON);
				    break;

					case RD_SEL_CAMERA:
				   		MainMode_DAB2_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MAIN_SEL_BAR,DISP_ON);
				    break;
					
					case RD_SEL_BAR:
				   		MainMode_DAB2_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(RD_SEL_ATT,DISP_ON);
				    break;

				    case RD_SEL_ATT: //chpark.24.10.28
				   		if(AUI1_0B_3210b==3) // Off
							MainMode_DAB2_Select_Index = RD_SEL_HOURMETER;
						else
							MainMode_DAB2_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_DAB2_Select_Index,DISP_ON);
				    break;
					
					case RD_SEL_POWER:
				   		MainMode_DAB2_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(RD_SEL_HOURMETER,DISP_ON);
				    break;
				    
					case RD_SEL_HOURMETER:
						MainMode_DAB2_Select_Index = RD_SEL_AUTO;
				   		MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
				    break;
				    
				    case RD_SEL_TIMEDATE:
						//error
				   		MainMode_RD_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MAIN_SEL_BAR,DISP_ON);
				    break;
				}
			}
			else
			{
				switch(MainMode_DAB2_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_DAB2_Select_Index = RD_SEL_AUTO;
						MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
					case RD_SEL_AUTO:
						MainMode_DAB2_Select_Index = DAB2_SEL_1C;
				   		MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
					break;
					
					case DAB2_SEL_1C: //chpark.24.10.28
						if(AUI1_0B_3210b==3) // Off
							MainMode_DAB2_Select_Index = RD_SEL_BAR;
						else
							MainMode_DAB2_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_DAB2_Select_Index,DISP_ON);
					break;
					case DAB2_SEL_2C:
						MainMode_RD_Sel(1,DAB2_SEL_1C,DISP_ON);
					break;
					case DAB2_SEL_3C:
						MainMode_RD_Sel(1,DAB2_SEL_2C,DISP_ON);
					break;
					case DAB2_SEL_4C:
						MainMode_RD_Sel(1,DAB2_SEL_3C,DISP_ON);
					break;
					case DAB2_SEL_5C:
						MainMode_RD_Sel(1,DAB2_SEL_4C,DISP_ON);
					break;
					case DAB2_SEL_6C:
						MainMode_RD_Sel(1,DAB2_SEL_5C,DISP_ON);
					break;
					case DAB2_SEL_7C:
						MainMode_RD_Sel(1,DAB2_SEL_6C,DISP_ON);
					break;
					case DAB2_SEL_8C:
						MainMode_RD_Sel(1,DAB2_SEL_7C,DISP_ON);
					break;

					case RD_SEL_POWER:
				   		MainMode_DAB2_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(RD_SEL_BAR,DISP_ON);
				    break;
					 case RD_SEL_BAR:
				   		MainMode_DAB2_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(RD_SEL_ATT,DISP_ON);
				    break;
				    case RD_SEL_ATT:
				   		MainMode_DAB2_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(RD_SEL_HOURMETER,DISP_ON);
				    break;
					
					case RD_SEL_HOURMETER:
				   		MainMode_DAB2_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(RD_SEL_MENU,DISP_ON);
				    break;
					
					case RD_SEL_MENU:
				   		MainMode_DAB2_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(RD_SEL_AIRCON,DISP_ON);
				    break;
				    
					case RD_SEL_AIRCON:
				   		MainMode_DAB2_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(RD_SEL_RADIO,DISP_ON);
				    break;
				    
					case RD_SEL_RADIO:
				   		MainMode_DAB2_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(RD_SEL_SPK,DISP_ON);
				    break;
				    
					case RD_SEL_SPK:
				   		MainMode_DAB2_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(RD_SEL_CAMERA,DISP_ON);
				    break;

					case RD_SEL_CAMERA:
						MainMode_DAB2_Select_Index = RD_SEL_AUTO;
				   		MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
				    break;
				   
				    case RD_SEL_TIMEDATE:
						//error
				   		MainMode_RD_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MAIN_SEL_BAR,DISP_ON);
				    break;
				   
				   case RD_SEL_MODE:
				   		MainMode_RD_Sel(1,RD_SEL_AUTO,DISP_ON);
					break;
				}
			}
		}
		break;
	}
	return true;
}

bool JogDialKey_MAIN_Bluetooth(char* param) 
{
	char JD_Buf[10] = {0,};
	printf("JogDialKey_MAIN_Bluetooth= '0x%X' '%d' '%d'\n",param[0],MainMode_BT_Select_Index,RD_SEL_TONE_DU_FLAG);
	Media_RunTime_Cnt=gMainTimerCnt_10m;
	switch (param[0])
	{
		case JOG_DIAL_KEY_LEFT:				//c3Byte0bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_BT_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_MODE:
						MainMode_BT_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,BT_SEL_R30,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;
					case RD_SEL_PWR:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case BT_SEL_R27:
						MainMode_BT_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
					break;
					case BT_SEL_R28:
						MainMode_RD_Sel(1,BT_SEL_R27,DISP_ON);
					break;
					case BT_SEL_R29:
						MainMode_RD_Sel(1,BT_SEL_R28,DISP_ON);
					break;
					case BT_SEL_R30:
						MainMode_RD_Sel(1,BT_SEL_R29,DISP_ON);
					break;

				    case RD_SEL_ATT:
				   		MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				   		MainMode_BT_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
				   		MainMode_BT_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				   		MainMode_BT_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				   		if(AUI1_0B_3210b==3) // Off
							MainMode_BT_Select_Index = RD_SEL_CAMERA;
						else
							MainMode_BT_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
				   		MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_BT_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				   		MainMode_BT_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				   		MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				}
			}
			else
			{
				switch(MainMode_BT_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_MODE:
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;
					case RD_SEL_PWR:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case BT_SEL_R27:
						MainMode_RD_Sel(1,BT_SEL_R30,DISP_ON);
					break;
					case BT_SEL_R28:
						MainMode_RD_Sel(1,BT_SEL_R27,DISP_ON);
					break;
					case BT_SEL_R29:
						MainMode_RD_Sel(1,BT_SEL_R28,DISP_ON);
					break;
					case BT_SEL_R30:
						MainMode_RD_Sel(1,BT_SEL_R29,DISP_ON);
					break;

				    case RD_SEL_ATT:
				   		MainMode_BT_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				   		MainMode_BT_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
				   		MainMode_BT_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				   		MainMode_BT_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				    break;
				    case RD_SEL_BAR:
				   		if(AUI1_0B_3210b==3) // Off
							MainMode_BT_Select_Index = RD_SEL_ATT;
						else
							MainMode_BT_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_BT_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				   		MainMode_BT_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				   		MainMode_BT_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				}
			}
		}
		break;
		case JOG_DIAL_KEY_RIGHT:			//c3Byte1bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_BT_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_MODE:
						MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
							MainMode_BT_Select_Index = RD_SEL_BAR;
							MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
						}
					break;
					case RD_SEL_PWR:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case BT_SEL_R27:
						MainMode_RD_Sel(1,BT_SEL_R28,DISP_ON);
					break;
					case BT_SEL_R28:
						MainMode_RD_Sel(1,BT_SEL_R29,DISP_ON);
					break;
					case BT_SEL_R29:
						MainMode_RD_Sel(1,BT_SEL_R30,DISP_ON);
					break;
					case BT_SEL_R30:
						MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;

				    case RD_SEL_ATT:
				   		MainMode_RD_Sel(1,BT_SEL_R27,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				   		MainMode_BT_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
				   		MainMode_BT_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				   		MainMode_BT_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				   		MainMode_BT_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
				   		MainMode_RD_Sel(1,BT_SEL_R27,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				   		if(AUI1_0B_3210b==3) // Off
							MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						else
							MainMode_BT_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				   		MainMode_BT_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				}
			}
			else
			{
				switch(MainMode_BT_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_MODE:
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;
					case RD_SEL_PWR:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case BT_SEL_R27:
						MainMode_RD_Sel(1,BT_SEL_R28,DISP_ON);
					break;
					case BT_SEL_R28:
						MainMode_RD_Sel(1,BT_SEL_R29,DISP_ON);
					break;
					case BT_SEL_R29:
						MainMode_RD_Sel(1,BT_SEL_R30,DISP_ON);
					break;
					case BT_SEL_R30:
						MainMode_RD_Sel(1,BT_SEL_R27,DISP_ON);
					break;

				    case RD_SEL_ATT:
				   		if(AUI1_0B_3210b==3) // Off
							MainMode_BT_Select_Index = RD_SEL_BAR;
						else
							MainMode_BT_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				   		MainMode_BT_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
				   		MainMode_BT_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				   		MainMode_BT_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				    break;
				    case RD_SEL_BAR:
				   		MainMode_BT_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_BT_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				   		MainMode_BT_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				   		MainMode_BT_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				}
			}
		}
		break;
		case JOG_DIAL_KEY_UP:				//c3Byte2bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_BT_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_MODE:
						MainMode_RD_Sel(1,BT_SEL_R30,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_PWR:
				   		MainMode_BT_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
					break;
					case RD_SEL_LOUD:
				   		MainMode_BT_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
					break;
					case BT_SEL_R27:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case BT_SEL_R28:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case BT_SEL_R29:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case BT_SEL_R30:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;

				    case RD_SEL_ATT:
				   		MainMode_BT_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				   		MainMode_RD_Sel(1,BT_SEL_R28,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
				   		MainMode_RD_Sel(1,BT_SEL_R29,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				   		MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				   		MainMode_BT_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
				   		MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_BT_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				   		MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				   		MainMode_RD_Sel(1,BT_SEL_R27,DISP_ON);
				    break;
				}
			}
			else
			{
				switch(MainMode_BT_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_MODE:
						MainMode_RD_Sel(1,BT_SEL_R30,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
				    		MainMode_RD_Sel(1,BT_SEL_R27,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_PWR:
				   		MainMode_BT_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
					break;
					case RD_SEL_LOUD:
				   		MainMode_BT_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
					break;
					case BT_SEL_R27:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case BT_SEL_R28:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case BT_SEL_R29:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case BT_SEL_R30:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;

				    case RD_SEL_ATT:
				   		MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				   		MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
				   		MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				   		MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				   		MainMode_BT_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
				   		MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				   		MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				   		MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				}
			}
		}
		break;
		case JOG_DIAL_KEY_DOWN:				//c3Byte3bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_BT_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_MODE:
				   		MainMode_BT_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
				   			MainMode_BT_Select_Index = RD_SEL_CAMERA;
							MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    	}
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
				   			MainMode_BT_Select_Index = RD_SEL_CAMERA;
							MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    	}
					break;
					case RD_SEL_PWR:
						MainMode_RD_Sel(1,BT_SEL_R27,DISP_ON);
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,BT_SEL_R30,DISP_ON);
					break;
					case BT_SEL_R27:
						MainMode_BT_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
					break;
					case BT_SEL_R28:
						MainMode_BT_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
					break;
					case BT_SEL_R29:
						MainMode_BT_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
					break;
					case BT_SEL_R30:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;

				    case RD_SEL_ATT:
				   		MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				   		MainMode_BT_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
				   		MainMode_BT_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_BT_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				}
			}
			else
			{
				switch(MainMode_BT_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_MODE:
				   		MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
				   			MainMode_BT_Select_Index = RD_SEL_BAR;
							MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    	}
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
				   			MainMode_BT_Select_Index = RD_SEL_ATT;
							MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    	}
					break;
					case RD_SEL_PWR:
						MainMode_RD_Sel(1,BT_SEL_R27,DISP_ON);
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,BT_SEL_R30,DISP_ON);
					break;
					case BT_SEL_R27:
						MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case BT_SEL_R28:
						MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case BT_SEL_R29:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case BT_SEL_R30:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;

				    case RD_SEL_ATT:
				   		MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				   		MainMode_BT_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
				   		MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				}
			}
		}
		break;
		case JOG_DIAL_KEY_ENTER_ON:
		{
			switch(MainMode_BT_Select_Index)
			{
				case RD_SEL_HOURMETER:
				{
					if (KEY_ENTER_HM_CNT == 0)
						KEY_ENTER_HM_CNT = gMainTimerCnt_10m;
				}
				break;
				case RD_SEL_ATT:
				{
					if (KEY_ENTER_ATT_CNT == 0)
						KEY_ENTER_ATT_CNT = gMainTimerCnt_10m;
				}
				break;
			}
		}
		break;
		case JOG_DIAL_KEY_ENTER_OFF:
//		case JOG_DIAL_KEY_NONE:
		{
	//		printf("JOG_DIAL_KEY_ENTER_OFF= '%d' 0x%x\n",MainMode_BT_Select_Index,KEY_ENTER_HM_CNT);
			switch(MainMode_BT_Select_Index)
			{
				case RD_SEL_TONE_D:
				case RD_SEL_TONE_U:
				{
					if (RD_SEL_TONE_DU_FLAG==0)
					{
						RD_SEL_TONE_DU_FLAG = 1;
					}
					else
					{
						RD_SEL_TONE_DU_FLAG = 0;
						Main_CLI2[3] = 0;
						Main_CLI2[4] = 0;
						RD_SEL_TONE_D_CNT = LONG_KEY_TIME;
						RD_SEL_TONE_U_CNT = LONG_KEY_TIME;
						MainMode_RD_Sel(0,RD_SEL_TONE_U,DISP_ON);
					}
				}
				break;
				case RD_SEL_VOL_D:
				case RD_SEL_VOL_U:
				{
					if (RD_SEL_VOL_DU_FLAG==0)
					{
						RD_SEL_VOL_DU_FLAG = 1;
					}
					else
					{
						RD_SEL_VOL_DU_FLAG = 0;
						Main_CLI2[3] = 0;
						Main_CLI2[4] = 0;
						RD_SEL_VOL_U_CNT = LONG_KEY_TIME;
						RD_SEL_VOL_D_CNT = LONG_KEY_TIME;
						MainMode_RD_Sel(0,RD_SEL_VOL_U,DISP_ON);
					}
				}
				break;
				case BT_SEL_R27:
				case BT_SEL_R28:
				case BT_SEL_R29:
				case BT_SEL_R30:
				case RD_SEL_PWR:
				case RD_SEL_LOUD:
				case RD_SEL_MODE:
				{
					JD_Buf[0] = (MainMode_BT_Select_Index/10);
					JD_Buf[1] = (MainMode_BT_Select_Index%10);
					RD_MAIN_UpBt(NULL, JD_Buf);
				}
				break;
				case RD_SEL_ATT:
				{
					if (KEY_ENTER_ATT_CNT != 0xFFFFF000)
					{
						JD_Buf[1] = 0;
						if (AUX1CurrentOutMode==0)
							JD_Buf[0] = 2;
						else if (AUX1CurrentOutMode==1)
							JD_Buf[0] = 3;
						else if (AUX1CurrentOutMode==2)
							JD_Buf[0] = 1;
						Main_ATTBt_Func(NULL, JD_Buf);
					}
					KEY_ENTER_ATT_CNT = 0;
				}
				break;
				case RD_SEL_AIRCON:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_ACBt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_RADIO:
				{
				}
				break;
				case RD_SEL_SPK:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_SPK_Bt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_HOURMETER:
				{
					if (KEY_ENTER_HM_CNT != 0xFFFFF000)
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_HMeterBt_Func(NULL, JD_Buf);
					}
					else
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_HMeterBt_Func(NULL, JD_Buf);
					}
					KEY_ENTER_HM_CNT = 0;
				}
				break;
				case RD_SEL_BAR:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_BarBt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_POWER:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_PwrBt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_CAMERA:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					MainCamera(NULL, JD_Buf);
				}
				break;
				case RD_SEL_MENU:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					MainHome(NULL, JD_Buf);
				}
				break;
			}
		}
	}
	return true;
}

bool JogDialRot_MAIN_Bluetooth(char* param)
{
	char JD_Buf[10] = {0,};
	printf("JogDialRot_MAIN_Bluetooth= '0x%X' '%d'\n",param[0],MainMode_BT_Select_Index);
	Media_RunTime_Cnt=gMainTimerCnt_10m;

	switch (param[0])
	{
		case JOG_DIAL_ROT_LEFT:
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_BT_Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_MODE:
						MainMode_RD_Sel(1,BT_SEL_R30,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_DU_FLAG = 2;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_DU_FLAG = 2;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case RD_SEL_PWR:
						MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case BT_SEL_R27:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case BT_SEL_R28:
						MainMode_RD_Sel(1,BT_SEL_R27,DISP_ON);
					break;
					case BT_SEL_R29:
						MainMode_RD_Sel(1,BT_SEL_R28,DISP_ON);
					break;
					case BT_SEL_R30:
						MainMode_RD_Sel(1,BT_SEL_R29,DISP_ON);
					break;
				    case RD_SEL_ATT:
						MainMode_BT_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
						MainMode_BT_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
						MainMode_BT_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
						MainMode_BT_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
						if(AUI1_0B_3210b==3) // Off
							MainMode_BT_Select_Index = RD_SEL_ATT;
						else
							MainMode_BT_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
						MainMode_BT_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						MainMode_BT_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
						MainMode_BT_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
					break;
				    case RD_SEL_MENU:
				   		MainMode_RD_Sel(1,RD_SEL_VOL_U,DISP_ON);
				    break;
				}
			}
			else
			{
				switch(MainMode_BT_Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_MODE:
						MainMode_RD_Sel(1,BT_SEL_R30,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_DU_FLAG = 2;
						}
						else
				    		MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_DU_FLAG = 2;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case RD_SEL_PWR:
						MainMode_BT_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
					break;
					case BT_SEL_R27:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case BT_SEL_R28:
						MainMode_RD_Sel(1,BT_SEL_R27,DISP_ON);
					break;
					case BT_SEL_R29:
						MainMode_RD_Sel(1,BT_SEL_R28,DISP_ON);
					break;
					case BT_SEL_R30:
						MainMode_RD_Sel(1,BT_SEL_R29,DISP_ON);
					break;
				    case RD_SEL_ATT:
						MainMode_BT_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
						MainMode_BT_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
						MainMode_BT_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
						MainMode_BT_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
						MainMode_BT_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
						if(AUI1_0B_3210b==3) // Off
							MainMode_RD_Sel(1,RD_SEL_VOL_U,DISP_ON);
						else
						{
							MainMode_BT_Select_Index = RD_SEL_POWER;
							MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
						}
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_RD_Sel(1,RD_SEL_VOL_U,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
						MainMode_BT_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_MENU:
						MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				}
			}
		}
		break;
		case JOG_DIAL_ROT_RIGHT:
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_BT_Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_MODE:
						MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_U/10);
							JD_Buf[1] = (RD_SEL_TONE_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_DU_FLAG = 2;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_U/10);
							JD_Buf[1] = (RD_SEL_VOL_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_DU_FLAG = 2;
						}
						else
						{
							MainMode_BT_Select_Index = RD_SEL_MENU;
							MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
						}
					break;
					case RD_SEL_PWR:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,BT_SEL_R27,DISP_ON);
					break;
					case BT_SEL_R27:
						MainMode_RD_Sel(1,BT_SEL_R28,DISP_ON);
					break;
					case BT_SEL_R28:
						MainMode_RD_Sel(1,BT_SEL_R29,DISP_ON);
					break;
					case BT_SEL_R29:
						MainMode_RD_Sel(1,BT_SEL_R30,DISP_ON);
					break;
					case BT_SEL_R30:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
				    case RD_SEL_ATT:
						if(AUI1_0B_3210b==3) // Off
							MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						else
							MainMode_BT_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
						MainMode_BT_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
						MainMode_BT_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
						MainMode_BT_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_BAR:
						MainMode_BT_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
						MainMode_BT_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_MENU:
						MainMode_BT_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				}
			}
			else
			{
				switch(MainMode_BT_Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_MODE:
						MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_U/10);
							JD_Buf[1] = (RD_SEL_TONE_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_DU_FLAG = 2;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_U/10);
							JD_Buf[1] = (RD_SEL_VOL_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_DU_FLAG = 2;
						}
						else
						{
							if(AUI1_0B_3210b==3) // Off
								MainMode_BT_Select_Index = RD_SEL_BAR;
							else
								MainMode_BT_Select_Index = RD_SEL_POWER;
							MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);						
						}
					break;
					case RD_SEL_PWR:
						MainMode_RD_Sel(1,RD_SEL_LOUD,DISP_ON);
					break;
					case RD_SEL_LOUD:
						MainMode_RD_Sel(1,BT_SEL_R27,DISP_ON);
					break;
					case BT_SEL_R27:
						MainMode_RD_Sel(1,BT_SEL_R28,DISP_ON);
					break;
					case BT_SEL_R28:
						MainMode_RD_Sel(1,BT_SEL_R29,DISP_ON);
					break;
					case BT_SEL_R29:
						MainMode_RD_Sel(1,BT_SEL_R30,DISP_ON);
					break;
					case BT_SEL_R30:
						MainMode_RD_Sel(1,RD_SEL_MODE,DISP_ON);
					break;
				    case RD_SEL_ATT:
						MainMode_BT_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
						MainMode_BT_Select_Index = RD_SEL_RADIO;
				   		MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
						MainMode_BT_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
						MainMode_BT_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
						MainMode_BT_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
						MainMode_BT_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						MainMode_BT_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				   		MainMode_RD_Sel(1,RD_SEL_PWR,DISP_ON);
				    break;
				    case RD_SEL_MENU:
						MainMode_BT_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_BT_Select_Index,DISP_ON);
				    break;
				}
			}
		}
		break;
	}
	return true;
}
bool JogDialKey_MAIN_Phone(char* param)
{
	char JD_Buf[10] = {0,};
	// printf("JogDialKey_MAIN_Phone= '0x%X' '%d'\n",param[0],MainMode_Phone_Select_Index,RD_SEL_TONE_DU_FLAG);
	Media_RunTime_Cnt=gMainTimerCnt_10m;
	switch (param[0])
	{
		case JOG_DIAL_KEY_LEFT:				//c3Byte0bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_Phone_Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
							if (RDV.RDPhone==5)
								MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
							else if (RDV.RDPhone==1)
				    			MainMode_RD_Sel(1,PH_SEL_R31,DISP_ON);
				    		else if ((RDV.RDPhone==2)||(RDV.RDPhone==3))
					    		MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
							else
							{
								MainMode_Phone_Select_Index = RD_SEL_BAR;
								MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
							}
				    	}
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case PH_SEL_R34:
						MainMode_Phone_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;
					case PH_SEL_R31:
						if (RDV.RDPhone!=5)
							MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
						else
						{
							MainMode_Phone_Select_Index = RD_SEL_BAR;
							MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
						}
					break;
					case PH_SEL_R32:
						MainMode_Phone_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;

				    case RD_SEL_ATT:
				   		MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				   		MainMode_Phone_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
				   		MainMode_Phone_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				   		MainMode_Phone_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				   		if(AUI1_0B_3210b==3) // Off
							MainMode_Phone_Select_Index = RD_SEL_CAMERA;
						else
							MainMode_Phone_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
				   		MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_Phone_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				   		MainMode_Phone_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				   		MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				}
			}
			else
			{
				switch(MainMode_Phone_Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
							MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
				    	}
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
							MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
				    	}
					break;
					case PH_SEL_R34:
						if ((RDV.RDPhone==2)||(RDV.RDPhone==3))
						{
						}
						else
				    		MainMode_RD_Sel(1,PH_SEL_R31,DISP_ON);
					break;
					case PH_SEL_R31:
						if ((RDV.RDPhone==2)||(RDV.RDPhone==3))
						{
						}
						else
				    		MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
					break;
					case PH_SEL_R32:
					break;

				    case RD_SEL_ATT:
				   		MainMode_Phone_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				   		MainMode_Phone_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
				   		MainMode_Phone_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				   		MainMode_Phone_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				    break;
				    case RD_SEL_BAR:
				   		if(AUI1_0B_3210b==3) // Off
							MainMode_Phone_Select_Index = RD_SEL_ATT;
						else
							MainMode_Phone_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_Phone_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				   		MainMode_Phone_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				   		MainMode_Phone_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				}
			}
		}
		break;
		case JOG_DIAL_KEY_RIGHT:			//c3Byte1bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_Phone_Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_U/10);
							JD_Buf[1] = (RD_SEL_TONE_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_U_CNT = gMainTimerCnt_10m;
						}
						else
						{
							MainMode_RD_Sel(1,RD_SEL_VOL_U,DISP_ON);
						}
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_U/10);
							JD_Buf[1] = (RD_SEL_VOL_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_U_CNT = gMainTimerCnt_10m;
						}
						else
						{
							MainMode_Phone_Select_Index = RD_SEL_BAR;
							MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
						}
					break;
					case PH_SEL_R34:
						if ((RDV.RDPhone==5)||(RDV.RDPhone==2)||(RDV.RDPhone==3))
							MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
			    		else
			    			MainMode_RD_Sel(1,PH_SEL_R31,DISP_ON);
					break;
					case PH_SEL_R31:
						MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
					break;
					case PH_SEL_R32:
						MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;

				    case RD_SEL_ATT:
				    	if (RDV.RDPhone==1)
			    			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
			    		else if ((RDV.RDPhone==2)||(RDV.RDPhone==3))
			    			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
			    		else
			    			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				   		MainMode_Phone_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
				   		MainMode_Phone_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				   		MainMode_Phone_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				   		MainMode_Phone_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
						if (RDV.RDPhone==1)
			    			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
			    		else if ((RDV.RDPhone==2)||(RDV.RDPhone==3))
			    			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
			    		else
			    			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				   		if(AUI1_0B_3210b==3) // Off
							MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						else
							MainMode_Phone_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				   		MainMode_Phone_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				}
			}
			else
			{				
				switch(MainMode_Phone_Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_U/10);
							JD_Buf[1] = (RD_SEL_TONE_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_U_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_VOL_U,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_U/10);
							JD_Buf[1] = (RD_SEL_VOL_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_U_CNT = gMainTimerCnt_10m;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case PH_SEL_R34:
						if ((RDV.RDPhone!=2)&&(RDV.RDPhone!=3)&&(RDV.RDPhone!=5))
			    			MainMode_RD_Sel(1,PH_SEL_R31,DISP_ON);
					break;
					case PH_SEL_R31:
						MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
					break;
					case PH_SEL_R32:
					break;

				    case RD_SEL_ATT:
				   		if(AUI1_0B_3210b==3) // Off
							MainMode_Phone_Select_Index = RD_SEL_BAR;
						else
							MainMode_Phone_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				   		MainMode_Phone_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
				   		MainMode_Phone_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				   		MainMode_Phone_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				    break;
				    case RD_SEL_BAR:
				   		MainMode_Phone_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_Phone_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				   		MainMode_Phone_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				   		MainMode_Phone_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				}
			}
		}
		break;
		case JOG_DIAL_KEY_UP:				//c3Byte2bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_Phone_Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_U/10);
							JD_Buf[1] = (RD_SEL_TONE_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_U_CNT = gMainTimerCnt_10m;
						}
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_U/10);
							JD_Buf[1] = (RD_SEL_VOL_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_U_CNT = gMainTimerCnt_10m;
						}
					break;
					case PH_SEL_R34:
				   		MainMode_Phone_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;
					case PH_SEL_R31:
				   		MainMode_Phone_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;
					case PH_SEL_R32:
				   		MainMode_Phone_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;

				    case RD_SEL_ATT:
				   		MainMode_Phone_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				    	if (RDV.RDPhone==5)
				   			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				   		else
				   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
						if (RDV.RDPhone==1)
			    			MainMode_RD_Sel(1,PH_SEL_R31,DISP_ON);
			    		else if ((RDV.RDPhone==2)||(RDV.RDPhone==3))
				    		MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    	else
				    		MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				    break;
				    case RD_SEL_SPK:
						if (RDV.RDPhone==1)
			    			MainMode_RD_Sel(1,PH_SEL_R31,DISP_ON);
			    		else if ((RDV.RDPhone==2)||(RDV.RDPhone==3))
				    		MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    	else
				    		MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				   		MainMode_Phone_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
				   		MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_Phone_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				   		MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				    	if (RDV.RDPhone==5)
				   			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				   		else
				   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    break;
				}
			}
			else
			{
				switch(MainMode_Phone_Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_U/10);
							JD_Buf[1] = (RD_SEL_TONE_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_U_CNT = gMainTimerCnt_10m;
						}
						else
						{
					    	if (RDV.RDPhone==5)
					   			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
					   		else
					   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
						}
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_U/10);
							JD_Buf[1] = (RD_SEL_VOL_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_U_CNT = gMainTimerCnt_10m;
						}
						else
						{
							if (RDV.RDPhone==1)
				    			MainMode_RD_Sel(1,PH_SEL_R31,DISP_ON);
				    		else if ((RDV.RDPhone==2)||(RDV.RDPhone==3))
					    		MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
					    	else
					    		MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
						}
					break;
					case PH_SEL_R34:
						MainMode_Phone_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;
					case PH_SEL_R31:
						MainMode_Phone_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;
					case PH_SEL_R32:
						MainMode_Phone_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;

				    case RD_SEL_ATT:
				   		MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				   		MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
				   		MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				   		MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				   		MainMode_Phone_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
				   		MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_RD_Sel(1,RD_SEL_TONE_D,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				   		MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				   		MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				}
			}
		}
		break;
		case JOG_DIAL_KEY_DOWN:				//c3Byte3bit
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_Phone_Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
				   			MainMode_Phone_Select_Index = RD_SEL_CAMERA;
							MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
						}
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
				   			MainMode_Phone_Select_Index = RD_SEL_CAMERA;
							MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
						}
					break;
					case PH_SEL_R34:
				   		MainMode_Phone_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;
					case PH_SEL_R31:
				   		MainMode_Phone_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;
					case PH_SEL_R32:
				   		MainMode_Phone_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;

				    case RD_SEL_ATT:
				   		MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				    	if (RDV.RDPhone==5)
				   			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				   		else
				   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
				    	if (RDV.RDPhone==5)
				   			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				   		else
				   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				    	if (RDV.RDPhone==5)
				   			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				   		else
				   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				   		MainMode_Phone_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
				   		MainMode_Phone_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_Phone_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				    	if (RDV.RDPhone==5)
				   			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				   		else
				   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				    	if (RDV.RDPhone==5)
				   			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				   		else
				   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    break;
				}
			}
			else
			{
				switch(MainMode_Phone_Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
				   			MainMode_Phone_Select_Index = RD_SEL_BAR;
							MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
						}
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_D_CNT = gMainTimerCnt_10m;
						}
						else
						{
				   			MainMode_Phone_Select_Index = RD_SEL_ATT;
							MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
						}
					break;
					case PH_SEL_R34:
						MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case PH_SEL_R31:
						MainMode_RD_Sel(1,RD_SEL_VOL_U,DISP_ON);
					break;
					case PH_SEL_R32:
						MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;

				    case RD_SEL_ATT:
				   		MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
				    	if (RDV.RDPhone==5)
				   			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				   		else
				   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
				    	if (RDV.RDPhone==5)
				   			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				   		else
				   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    break;
				    case RD_SEL_SPK:
				    	if (RDV.RDPhone==5)
				   			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				   		else
				   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
				   		MainMode_Phone_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
				   		MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				    	if (RDV.RDPhone==5)
				   			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				   		else
				   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    break;
				    case RD_SEL_MENU:
				    	if (RDV.RDPhone==5)
				   			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				   		else
				   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    break;
				}
			}
		}
		break;
		case JOG_DIAL_KEY_ENTER_ON:
		{		
			switch(MainMode_Phone_Select_Index)
			{
				case RD_SEL_HOURMETER:
				{
					if (KEY_ENTER_HM_CNT == 0)
						KEY_ENTER_HM_CNT = gMainTimerCnt_10m;
				}
				break;
				case RD_SEL_ATT:
				{
					if (KEY_ENTER_ATT_CNT == 0)
						KEY_ENTER_ATT_CNT = gMainTimerCnt_10m;
				}
				break;
			}
		}
		break;
		case JOG_DIAL_KEY_ENTER_OFF:
//		case JOG_DIAL_KEY_NONE:
		{
//			printf("JOG_DIAL_KEY_ENTER_OFF= '%d' 0x%x\n",MainMode_Phone_Select_Index,KEY_ENTER_HM_CNT);
			switch(MainMode_Phone_Select_Index)
			{
				case RD_SEL_TONE_D:
				case RD_SEL_TONE_U:
				{
					if (RD_SEL_TONE_DU_FLAG==0)
					{
						RD_SEL_TONE_DU_FLAG = 1;
					}
					else
					{
						RD_SEL_TONE_DU_FLAG = 0;
						Main_CLI2[3] = 0;
						Main_CLI2[4] = 0;
						RD_SEL_TONE_D_CNT = LONG_KEY_TIME;
						RD_SEL_TONE_U_CNT = LONG_KEY_TIME;
						MainMode_RD_Sel(0,RD_SEL_TONE_U,DISP_ON);
					}
				}
				break;
				case RD_SEL_VOL_D:
				case RD_SEL_VOL_U:
				{
					if (RD_SEL_VOL_DU_FLAG==0)
					{
						RD_SEL_VOL_DU_FLAG = 1;
					
				
					}
					else
					{
						RD_SEL_VOL_DU_FLAG = 0;
						Main_CLI2[3] = 0;
						Main_CLI2[4] = 0;
						RD_SEL_VOL_U_CNT = LONG_KEY_TIME;
						RD_SEL_VOL_D_CNT = LONG_KEY_TIME;
						MainMode_RD_Sel(0,RD_SEL_VOL_U,DISP_ON);
					}
				}
				break;
				case PH_SEL_R34:
				case PH_SEL_R31:
				case PH_SEL_R32:
				{
					JD_Buf[0] = (MainMode_Phone_Select_Index/10);
					JD_Buf[1] = (MainMode_Phone_Select_Index%10);
					RD_MAIN_UpBt(NULL, JD_Buf);
				}
				break;
				case RD_SEL_ATT:
				{
					if (KEY_ENTER_ATT_CNT != 0xFFFFF000)
					{
						JD_Buf[1] = 0;
						if (AUX1CurrentOutMode==0)
							JD_Buf[0] = 2;
						else if (AUX1CurrentOutMode==1)
							JD_Buf[0] = 3;
						else if (AUX1CurrentOutMode==2)
							JD_Buf[0] = 1;
						Main_ATTBt_Func(NULL, JD_Buf);
					}
					KEY_ENTER_ATT_CNT = 0;
				}
				break;
				case RD_SEL_AIRCON:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_ACBt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_RADIO:
				{
				}
				break;
				case RD_SEL_SPK:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_SPK_Bt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_HOURMETER:
				{
					if (KEY_ENTER_HM_CNT != 0xFFFFF000)
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_HMeterBt_Func(NULL, JD_Buf);
					}
					else
					{
						JD_Buf[0] = 0;
						JD_Buf[1] = 0;
						Main_HMeterBt_Func(NULL, JD_Buf);
					}
					KEY_ENTER_HM_CNT = 0;
				}
				break;
				case RD_SEL_BAR:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_BarBt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_POWER:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					Main_PwrBt_Func(NULL, JD_Buf);
				}
				break;
				case RD_SEL_CAMERA:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					MainCamera(NULL, JD_Buf);
				}
				break;
				case RD_SEL_MENU:
				{
					JD_Buf[0] = 0;
					JD_Buf[1] = 0;
					MainHome(NULL, JD_Buf);
				}
				break;
			}
		}
	}
	return true;
}
bool JogDialRot_MAIN_Phone(char* param)
{
	char JD_Buf[10] = {0,};
	printf("JogDialRot_MAIN_Phone= '0x%X' '%d'\n",param[0],MainMode_Phone_Select_Index);
	Media_RunTime_Cnt=gMainTimerCnt_10m;
	switch (param[0])
	{
		case JOG_DIAL_ROT_LEFT:
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_Phone_Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_DU_FLAG = 2;
						}
						else
						{
							if (RDV.RDPhone==1)
				    			MainMode_RD_Sel(1,PH_SEL_R31,DISP_ON);
				    		else if ((RDV.RDPhone==2)||(RDV.RDPhone==3))
					    		MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
					    	else
					    		MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				    	}
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_DU_FLAG = 2;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case PH_SEL_R34:
						MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;
					case PH_SEL_R31:
				    	if (RDV.RDPhone==5)
					   	{
							MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
							MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
						}
				   		else
				   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
					break;
					case PH_SEL_R32:
						if (RDV.RDPhone==1)
			    			MainMode_RD_Sel(1,PH_SEL_R31,DISP_ON);
			    		else if ((RDV.RDPhone==2)||(RDV.RDPhone==3))
				    		MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    	else
					   	{
							MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
							MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
						}
					break;
				    case RD_SEL_ATT:
						MainMode_Phone_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
						MainMode_Phone_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
						MainMode_Phone_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
						MainMode_Phone_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
						if(AUI1_0B_3210b==3) // Off
							MainMode_Phone_Select_Index = RD_SEL_ATT;
						else
							MainMode_Phone_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
						MainMode_Phone_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						MainMode_Phone_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
						MainMode_Phone_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;
				    case RD_SEL_MENU:
				   		MainMode_RD_Sel(1,RD_SEL_VOL_U,DISP_ON);
				    break;
				}
			}
			else
			{
				switch(MainMode_Phone_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_Phone_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_D/10);
							JD_Buf[1] = (RD_SEL_TONE_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_DU_FLAG = 2;
						}
						else
						{
							if (RDV.RDPhone==1)
				    			MainMode_RD_Sel(1,PH_SEL_R31,DISP_ON);
				    		else if ((RDV.RDPhone==2)||(RDV.RDPhone==3))
					    		MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
					    	else
					    		MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				    	}
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_D/10);
							JD_Buf[1] = (RD_SEL_VOL_D%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_DU_FLAG = 2;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case PH_SEL_R34:
						MainMode_Phone_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;
					case PH_SEL_R31:
				    	if (RDV.RDPhone==5)
					   	{
							MainMode_Phone_Select_Index = RD_SEL_CAMERA;
							MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
						}
				   		else
				   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
					break;
					case PH_SEL_R32:
						if (RDV.RDPhone==1)
			    			MainMode_RD_Sel(1,PH_SEL_R31,DISP_ON);
			    		else if ((RDV.RDPhone==2)||(RDV.RDPhone==3))
				    		MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    	else
					   	{
							MainMode_Phone_Select_Index = RD_SEL_CAMERA;
							MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
						}
					break;
				    case RD_SEL_ATT:
						MainMode_Phone_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
						MainMode_Phone_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
						MainMode_Phone_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
						MainMode_Phone_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
						MainMode_Phone_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
						if(AUI1_0B_3210b==3) // Off
							MainMode_RD_Sel(1,RD_SEL_VOL_U,DISP_ON);
						else
						{
							MainMode_Phone_Select_Index = RD_SEL_POWER;
							MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
						}
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
				   		MainMode_RD_Sel(1,RD_SEL_VOL_U,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
						MainMode_Phone_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_MENU:
						MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				}
			}
		}
		break;
		case JOG_DIAL_ROT_RIGHT:
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				switch(MainMode_Phone_Select_Index)
				{
					case RD_SEL_OFF:
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_U/10);
							JD_Buf[1] = (RD_SEL_TONE_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_DU_FLAG = 2;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_U/10);
							JD_Buf[1] = (RD_SEL_VOL_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_DU_FLAG = 2;
						}
						else
						{
							MainMode_Phone_Select_Index = RD_SEL_MENU;
							MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
						}
					break;
					case PH_SEL_R34:
						if (RDV.RDPhone==1)
				    		MainMode_RD_Sel(1,PH_SEL_R31,DISP_ON);
				    	else if (RDV.RDPhone==5)
				    		MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				    	else
				    		MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case PH_SEL_R31:
						if (RDV.RDPhone==5)
				    		MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				    	else
				    		MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case PH_SEL_R32:
						MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
				    case RD_SEL_ATT:
						if(AUI1_0B_3210b==3) // Off
							MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						else
							MainMode_Phone_Select_Index = RD_SEL_POWER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
						MainMode_Phone_Select_Index = RD_SEL_RADIO;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
						MainMode_Phone_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
						MainMode_Phone_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
						if (RDV.RDPhone==5)
			    			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
			    		else
				    		MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);				   		
				    break;
				    case RD_SEL_BAR:
						MainMode_Phone_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
						MainMode_Phone_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_MENU:
						MainMode_Phone_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				}
			}
			else
			{
				switch(MainMode_Phone_Select_Index)
				{
					case RD_SEL_OFF:
						MainMode_Phone_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
					break;
					case RD_SEL_TONE_D:
					case RD_SEL_TONE_U:
						if (RD_SEL_TONE_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_TONE_U/10);
							JD_Buf[1] = (RD_SEL_TONE_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_TONE_DU_FLAG = 2;
						}
						else
							MainMode_RD_Sel(1,RD_SEL_VOL_D,DISP_ON);
					break;
					case RD_SEL_VOL_D:
					case RD_SEL_VOL_U:
						if (RD_SEL_VOL_DU_FLAG)
						{
							JD_Buf[0] = (RD_SEL_VOL_U/10);
							JD_Buf[1] = (RD_SEL_VOL_U%10);
							RD_MAIN_DnBt2(NULL, JD_Buf);
							RD_SEL_VOL_DU_FLAG = 2;
						}
						else
						{
							if(AUI1_0B_3210b==3) // Off
								MainMode_Phone_Select_Index = RD_SEL_BAR;
							else
								MainMode_Phone_Select_Index = RD_SEL_POWER;
							MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
						}
					break;
					case PH_SEL_R34:
						if (RDV.RDPhone==1)
				    		MainMode_RD_Sel(1,PH_SEL_R31,DISP_ON);
				    	else if (RDV.RDPhone==5)
				    		MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				    	else
				    		MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case PH_SEL_R31:
						if (RDV.RDPhone==5)
				    		MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				    	else
				    		MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
					case PH_SEL_R32:
						MainMode_RD_Sel(1,RD_SEL_TONE_U,DISP_ON);
					break;
				    case RD_SEL_ATT:
						MainMode_Phone_Select_Index = RD_SEL_HOURMETER;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_AIRCON:
						MainMode_Phone_Select_Index = RD_SEL_RADIO;
				   		MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_RADIO:
						MainMode_Phone_Select_Index = RD_SEL_SPK;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_SPK:
						MainMode_Phone_Select_Index = RD_SEL_CAMERA;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_HOURMETER:
						MainMode_Phone_Select_Index = RD_SEL_MENU;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_BAR:
						MainMode_Phone_Select_Index = RD_SEL_ATT;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_TIMEDATE:
				    break;
				    case RD_SEL_POWER:
						MainMode_Phone_Select_Index = RD_SEL_BAR;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				    case RD_SEL_CAMERA:
				    	if (RDV.RDPhone==5)
				   			MainMode_RD_Sel(1,PH_SEL_R32,DISP_ON);
				   		else
				   			MainMode_RD_Sel(1,PH_SEL_R34,DISP_ON);
				    break;
				    case RD_SEL_MENU:
						MainMode_Phone_Select_Index = RD_SEL_AIRCON;
						MainLayer_ButtonSel(MainMode_Phone_Select_Index,DISP_ON);
				    break;
				}
			}
		}
		break;
	}
	return true;
}





