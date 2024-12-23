#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "HT_DispDdefine.h"





///////////////////  Source Ãß°¡ START ////////////////////////

	 uint32_t tTimePM = 0x00;
	 uint32_t tTimeLiftLd = 0x00;
	 uint32_t tTimeEngRPM = 0x00;
	 uint32_t tTimeEngLd = 0x00;
	 uint32_t tTimeASH = 0x00;
	 
	 extern ITUText* EngValue;
	 
	 
bool Main_MeterDisp(uint32_t TimeCnt17ms)
{
	static uint8_t tSymbol=0xF0; // m11_MeterSymbol_PM=11,m12_MeterSymbol_LiftLd=12,m13_MeterSymbol_EngRPM=13,m14_MeterSymbol_EngLd=14
    static uint32_t tTimeCnt = 0;
	static uint8_t tPM = 0xF0;
	static uint8_t tLiftLd = 0xF0;
	static uint8_t tEngRPM = 0xF0;
	static uint8_t tEngLd = 0xF0;
	static uint8_t tASH = 0xF0;
	static uint32_t tTimeCnt17ms = 0;
	
	static uint8_t tMain_FLiftAlarmSwOn = 0;
	static bool tFirstFlag = 0;
	static bool tRPMFlag = 0;
	
	
	

	
		
 //   printf("MRV.MeterRevoFlag:%d,tSymbol:%d,MRV.Symbol:%d\n",MRV.MeterRevoFlag,tSymbol,MRV.Symbol);


	if (tFirstFlag==0)
	{
		tPM = MRV.PM;
		tLiftLd = MRV.LiftLd;
		tEngRPM = MRV.EngRPM;
		tEngLd = MRV.EngLd;
		tASH = MRV.ASH;
		tFirstFlag = 1;
	}
	
	
	if ((MRV.PMTime+TIME_CNT_5SEC)<gMainTimerCnt_10m)
	{
		MRV.PM = 0xFF;
	}
	if ((MRV.LiftLdTime+TIME_CNT_5SEC)<gMainTimerCnt_10m)
	{
		MRV.LiftLd = 0xFF;
	}
	if ((MRV.EngRPMTime+TIME_CNT_5SEC)<gMainTimerCnt_10m)
	{
		MRV.EngRPM = 0xFF;
	}
	if ((MRV.EngLdTime+TIME_CNT_5SEC)<gMainTimerCnt_10m)
	{
		MRV.EngLd = 0xFF;
	}	
	if ((MRV.ASHTime+TIME_CNT_5SEC)<gMainTimerCnt_10m)
	{
		MRV.ASH = 0xFF;
	}

	if (MeterLiftLdFlag==1)
	{
    	MRV.Symbol = m12_MeterSymbol_LiftLd;
    	tTimeLiftLd = TimeCnt17ms;
    	MeterLiftLdFlag = 2;
	}
	else
	{
		switch (MRV.Symbol)
		{
			case m11_MeterSymbol_PM:
			{
				if ((abs(MRV.EngRPM-tEngRPM)>=5))
			    {
			    	MRV.Symbol = m13_MeterSymbol_EngRPM;
			    	tTimeEngRPM = TimeCnt17ms;
			    }
			    else
			    {
						if (MRV.PM!=0xFF)
						{
							tTimePM = TimeCnt17ms;
						}
						else if (MRV.ASH!=0xFF)
						{
							MRV.Symbol = m11_MeterSymbol_AHS;
							tTimeASH = gMainTimerCnt_10m;
						}
						else if (MRV.EngRPM!=0xFF) 
						{
							MRV.Symbol = m13_MeterSymbol_EngRPM;
							tTimeEngRPM = gMainTimerCnt_10m;
						}
			    }
			}
			break;
			case m12_MeterSymbol_LiftLd:
			{
				if ((abs(MRV.EngRPM-tEngRPM)>=5))
			    {
			    	MRV.Symbol = m13_MeterSymbol_EngRPM;
			    	tTimeEngRPM = TimeCnt17ms;
			    }
			    else
			    {
			    	if (MeterLiftLdFlag!=0)
					{
						if (MRV.LiftLd!=0xFF)
						{
							tTimeLiftLd = TimeCnt17ms;
						}
						else if (MRV.PM!=0xFF)
						{
							MRV.Symbol = m11_MeterSymbol_PM;
							tTimePM = gMainTimerCnt_10m;
						}
						else if (MRV.ASH!=0xFF)
						{
							MRV.Symbol = m11_MeterSymbol_AHS;
							tTimeASH = gMainTimerCnt_10m;
						}
						else if (MRV.EngRPM!=0xFF) 
						{
							MRV.Symbol = m13_MeterSymbol_EngRPM;
							tTimeEngRPM = gMainTimerCnt_10m;
						}
					}
					else
					{
						if ((abs(MRV.LiftLd-tLiftLd)<5)&&((tTimeLiftLd+TIME_CNT_5SEC)<TimeCnt17ms))
						{
						    {
						    	MRV.Symbol = m13_MeterSymbol_EngRPM;
						    	tTimeEngRPM = TimeCnt17ms;
						    }
						}
						else //if ((abs(MRV.LiftLd-tLiftLd)>5))
						{
							tTimeLiftLd = TimeCnt17ms;
						}
					}
			    }
			}
			break;
			case m13_MeterSymbol_EngRPM:
			{
				if ((abs(MRV.EngRPM-tEngRPM)<5)&&((tTimeEngRPM+TIME_CNT_5SEC)<TimeCnt17ms)&&(MeterLiftLdFlag!=0))
				{
					{
						MRV.Symbol = m12_MeterSymbol_LiftLd;
						tTimeLiftLd = TimeCnt17ms;
					}
				}
				else
				{
					if (MeterLiftLdFlag!=0)
					{
						if ((abs(MRV.EngRPM-tEngRPM)<5)&&((tTimeEngRPM+TIME_CNT_5SEC)<TimeCnt17ms))
						{
							if (MRV.LiftLd!=0xFF)
							{
								MRV.Symbol = m12_MeterSymbol_LiftLd;
								tTimeLiftLd = gMainTimerCnt_10m;
							}
							else if (MRV.PM!=0xFF)
							{
								MRV.Symbol = m11_MeterSymbol_PM;
								tTimePM = gMainTimerCnt_10m;
							}
							else if (MRV.ASH!=0xFF)
							{
								MRV.Symbol = m11_MeterSymbol_AHS;
								tTimeASH = gMainTimerCnt_10m;
							}
							else if (MRV.EngRPM!=0xFF)
							{
								MRV.Symbol = m13_MeterSymbol_EngRPM;
								tTimeEngRPM = gMainTimerCnt_10m;
							}
						}
						else if ((abs(MRV.EngRPM-tEngRPM)>5))
						{
							tTimeEngRPM = TimeCnt17ms;
						}
					}
					else if ((abs(MRV.EngRPM-tEngRPM)>5))
					{
						tTimeEngRPM = TimeCnt17ms;
					}
				}
			}
			break;
			case m14_MeterSymbol_EngLd:
			{
				if ((abs(MRV.EngRPM-tEngRPM)>=5)&&((MRV.EngLd != 0xFF)||(MeterLiftLdFlag==0)))
				{
					MRV.Symbol = m13_MeterSymbol_EngRPM;
					tTimeEngRPM = TimeCnt17ms;
				}
				else
				{
					if (MeterLiftLdFlag!=0)
					{
						if (((abs(MRV.EngRPM-tEngRPM)<5)&&((tTimeEngRPM+TIME_CNT_5SEC)<TimeCnt17ms))||(MRV.EngLd == 0xFF))
						{
							if (MRV.LiftLd!=0xFF)
							{
								MRV.Symbol = m12_MeterSymbol_LiftLd;
								tTimeLiftLd = gMainTimerCnt_10m;
							}
							else if (MRV.PM!=0xFF)
							{
								MRV.Symbol = m11_MeterSymbol_PM;
								tTimePM = gMainTimerCnt_10m;
							}
							else if (MRV.ASH!=0xFF)
							{
								MRV.Symbol = m11_MeterSymbol_AHS;
								tTimeASH = gMainTimerCnt_10m;
							}
							else if (MRV.EngRPM!=0xFF) 
							{
								MRV.Symbol = m13_MeterSymbol_EngRPM;
								tTimeEngRPM = gMainTimerCnt_10m;
							}
						}
						else if ((abs(MRV.EngRPM-tEngRPM)>5))
						{
							tTimeEngRPM = TimeCnt17ms;
						}
					}
					else if ((abs(MRV.EngLd-tEngLd)>5))
					{
						tTimeEngLd = TimeCnt17ms;
					}
				}
			}
			break;
			case m11_MeterSymbol_AHS:
			{
				if ((abs(MRV.EngRPM-tEngRPM)>=5))
				{
					MRV.Symbol = m13_MeterSymbol_EngRPM;
					tTimeEngRPM = TimeCnt17ms;
				}
				else
				{
					if (MeterLiftLdFlag!=0)
					{
						if (MRV.ASH==0xFF)
						{
							MRV.Symbol = m13_MeterSymbol_EngRPM;
							tTimeEngRPM = TimeCnt17ms;
						}
					}
					else if ((abs(MRV.ASH-tASH)>5))
					{
						tTimeASH = TimeCnt17ms;
					}
				}
			}
			break;
		}
	}
//	tMain_FLiftAlarmSwOn = Main_FLiftAlarmSwOn;

 
    if (tSymbol != MRV.Symbol)
    {
    	tSymbol = MRV.Symbol;
    }

//printf("Main_MeterDisp:%d,%d ,%d\n",gCameraModeDisp,RotationDir(),MRV.Symbol);
	{
		switch (MRV.Symbol)
		{
			case m11_MeterSymbol_PM:
			{
				MainLayer_MeterERpmDisp(DISP_ON,MRV.Symbol,MRV.PM);
			}
			break;
			case m12_MeterSymbol_LiftLd:
			{
				MainLayer_MeterERpmDisp(DISP_ON,MRV.Symbol,MRV.LiftLd);
			}
			break;
			case m13_MeterSymbol_EngRPM:
			{
				MainLayer_MeterERpmDisp(DISP_ON,MRV.Symbol,MRV.EngRPM);
			}
			break;
			case m14_MeterSymbol_EngLd:
			{
				MainLayer_MeterERpmDisp(DISP_ON,MRV.Symbol,MRV.EngLd);
			}
			break;
			case m11_MeterSymbol_AHS:
			{
				MainLayer_MeterERpmDisp(DISP_ON,MRV.Symbol,MRV.ASH);
			}
			break;
		}
		
	}

	tPM = MRV.PM;
	tLiftLd = MRV.LiftLd;
	tEngRPM = MRV.EngRPM;
	tEngLd = MRV.EngLd;
	tASH = MRV.ASH;
	
	if ((Main_Disp_Mode == MAIN_MODE_MENU)||(Main_Disp_Mode == MAIN_MODE_BACK)||(Main_Disp_Mode == MAIN_MODE_IMMOBI) ) //chpark.24.10.30
		return true;
	Main_EEC1_34BMeterDisp(EngValue);
	return true;
}