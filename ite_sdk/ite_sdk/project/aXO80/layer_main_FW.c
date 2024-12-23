#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "HT_DispDdefine.h"
#include "layer_main_FW.h"



static char widgetName[30];


/* widgets:
*/
///////////////////  Source 추가 START ////////////////////////

bool Main_ZoomSetDataF (uint8_t Src , uint8_t Dest)
{
	
	int i;
	for (i=0;i<FUNCTION_ICON_CNT;i++)
	{	
		if ((MainFunctionData[i]&Src)==Src)
		//if ((MainFunctionData[i]&&0xF0)==Src)
		{
			MainFunctionData[i] = MainFunctionData[i]&0x03;
			MainFunctionData[i] = MainFunctionData[i] | Dest;
		}
	}
}
bool Main_ZoomSetDataW (uint8_t Src , uint8_t Dest)
{
	
	int i;
	for (i=0;i<WARNING_ICON_CNT;i++)
	{	
		if ((MainWarnigData[i]&Src)==Src)
		//if ((MainWarnigData[i]&0xF0)==Src)
		{
			MainWarnigData[i] = MainWarnigData[i]&0x03;
			MainWarnigData[i] = MainWarnigData[i] | Dest;
		}
	}
}

bool Main_ZoomDispW(uint32_t TimeCnt,bool *ZoomFlag)
{
	static uint8_t tW_ZoomIndex = 0xFF;
	static uint32_t tTimeCnt = 0;
	static uint16_t tTimeUnit = 0;
#if 1
//	printf("Main_ZoomDisp S :%d %d \n",(*Main_Zoom),*ZoomFlag);

	//if ((*ZoomFlag==0)&&((*Main_Zoom)!=0xFF))
 	if (*ZoomFlag==1)
	{
		if (tW_ZoomIndex == Main_wZoomIndex)
		{
//			printf("tW_ZoomIndex 0:%d 0x%X %d \n\n\n",Main_wZoomIndex,MainWarnigData[Main_wZoomIndex]&0x0FC,(tTimeCnt+tTimeUnit<TimeCnt));
			if ((MainWarnigData[Main_wZoomIndex]&0x0FC)==0x0)//#10252		// off로 변경됨  0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요 
			{
				MainLayer_ZoomDispW(tW_ZoomIndex,DISP_OFF);
				*ZoomFlag=0;
				Main_ZoomSetDataW (0x20 , 0x80);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
				Main_wZoomIndex = 0xFE;
			}
			else if (tTimeCnt+tTimeUnit<TimeCnt)
			{
				MainLayer_ZoomDispW(tW_ZoomIndex,DISP_ANIMATION);
				*ZoomFlag=0;
				Main_ZoomSetDataW (0x20 , 0x40);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
				Main_ZoomSetDataW (0x10 , 0x40);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
				Main_wZoomIndex = 0xFE;
			}
		}
		else
		{
	//		printf("tW_ZoomIndex 1:%d %d \n\n\n",Main_wZoomIndex,tW_ZoomIndex);
			
			
			if ((Main_wZoomIndex!=0xFF)&&(Main_wZoomIndex!=0xFE))
			{
#if 1				
//				if ((Main_wZoomIndex_Time_Disp+TIME_CNT_200MS >= gMainTimerCnt_10m))
				if (Main_wZoomIndex_Time_Disp==1)
				{
					Main_wZoomIndex_Time_Disp = 0;
//					printf("  tW_ZoomIndex 2:%d %d \n\n\n",Main_wZoomIndex,tW_ZoomIndex);
					Main_ZoomSetDataW (0x40 , 0x20);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
					MainLayer_ZoomDispW(tW_ZoomIndex,DISP_OFF);
				}
				else
#endif
				{
//					printf("  tW_ZoomIndex 3:%d %d \n\n\n",Main_wZoomIndex,tW_ZoomIndex);
					Main_ZoomSetDataW (0x20 , 0x80);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
					Main_ZoomSetDataW (0x40 , 0x80);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
					MainLayer_ZoomDispW(tW_ZoomIndex,DISP_NORMAL_ON);
				}
	//			printf("tW_ZoomIndex 4:%d %d \n\n\n",Main_wZoomIndex,tW_ZoomIndex);
				MainLayer_ZoomDispW(Main_wZoomIndex,DISP_ON);
				tTimeUnit = ((Main_wZoomIndex<=WARNING_ICON_CNT)) ? TIME_CNT_3SEC:TIME_CNT_1SEC;  //  func와 wing의 디스플레이 시간이 다르다   
				tW_ZoomIndex = Main_wZoomIndex;
				tTimeCnt = TimeCnt;
				*ZoomFlag = 1;
				Main_ZoomSetDataW (0x10 , 0x20);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
			}
			else
			{
				MainLayer_ZoomDispW(tW_ZoomIndex,DISP_NORMAL_ON);
				*ZoomFlag=0;
				Main_ZoomSetDataW (0x20 , 0x80);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
				Main_wZoomIndex = 0xFE;
			}
		}
	}	
	else if ((Main_wZoomIndex!=0xFF)&&(Main_wZoomIndex!=0xFE))
	{

		{
	//		printf("Main_ZoomDispW :%d 0x%X \n\n\n",Main_wZoomIndex,MainWarnigData[Main_wZoomIndex]);
			MainLayer_ZoomDispW(Main_wZoomIndex,DISP_ON);
			tTimeUnit = ((Main_wZoomIndex<=WARNING_ICON_CNT)) ? TIME_CNT_3SEC:TIME_CNT_1SEC;  //  func와 wing의 디스플레이 시간이 다르다   
			tW_ZoomIndex = Main_wZoomIndex;
//			(*Main_Zoom) = 0xFF;
			tTimeCnt = TimeCnt;
			*ZoomFlag = 1;
			Main_ZoomSetDataW (0x10 , 0x20);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
			
//			printf("\n\nMain_ZoomDispW :%d 0x%X \n\n\n",Main_wZoomIndex,MainWarnigData[Main_wZoomIndex]);
		}
	}
	
//	printf("Main_ZoomDisp E :%d %d \n\n",(*Main_Zoom),*ZoomFlag);
#endif	
	return 0;
}


bool Main_ZoomDispF(uint32_t TimeCnt,bool *ZoomFlag)
{
	static uint8_t tF_ZoomIndex = 0xFF;
	static uint32_t tTimeCnt = 0;
	static uint16_t tTimeUnit = 0;


 	if (*ZoomFlag==1)
	{
		if ((Main_wZoomIndex!=0xFF)&&(Main_wZoomIndex!=0xFE))
		{
			MainLayer_ZoomDispF(tF_ZoomIndex,DISP_NORMAL_ON);
			Main_ZoomSetDataF (0x20 , 0x80);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
			Main_ZoomSetDataF (0x40 , 0x80);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
			*ZoomFlag=0;
			Main_fZoomIndex = 0xFE;
		}
		else if (tF_ZoomIndex == Main_fZoomIndex)
		{
			if ((MainFunctionData[Main_fZoomIndex-cF_01_B]&0x0FC)==0x0)//#10252		// off로 변경됨  0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요 
			{
				MainLayer_ZoomDispF(tF_ZoomIndex,DISP_OFF);
				*ZoomFlag=0;
				Main_ZoomSetDataF (0x20 , 0x80);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
				Main_fZoomIndex = 0xFE;
			}
			else
			{
				if (tTimeCnt+tTimeUnit<TimeCnt)
				{
	//				printf("1  Main_fZoomIndex:0x%X\n",Main_fZoomIndex);
					if (Main_Disp_Mode == MAIN_MODE_NORMAL)
					{
//					printf("44 TimeCnt:[%d][%d] , Main_fZoomIndex:0x%X\n",TimeCnt,gMainTimerCnt_10m,Main_fZoomIndex);
						MainLayer_ZoomDispF(tF_ZoomIndex,DISP_ANIMATION);
						*ZoomFlag=0;
						Main_ZoomSetDataF (0x20 , 0x40);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
						Main_ZoomSetDataF (0x10 , 0x40);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
					}
					else
					{
						MainLayer_ZoomDispF(tF_ZoomIndex,DISP_OFF);
						*ZoomFlag=0;
						Main_ZoomSetDataF (0x20 , 0x80);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
						Main_ZoomSetDataF (0x10 , 0x80);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
					}
					Main_fZoomIndex = 0xFE;
				}
			}
		}
		else
		{
			if (Main_Disp_Mode == MAIN_MODE_NORMAL)
			{
				if ((Main_fZoomIndex!=0xFF)&&(Main_fZoomIndex!=0xFE))
				{
					if (Main_fZoomIndex_Time_Disp==1)
					{
						Main_fZoomIndex_Time_Disp = 0;
						Main_ZoomSetDataF (0x40 , 0x20);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
						MainLayer_ZoomDispF(tF_ZoomIndex,DISP_OFF);
					}
					else
					{
	//					printf("2  Main_fZoomIndex:0x%X\n",Main_fZoomIndex);
						Main_ZoomSetDataF (0x20 , 0x80);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
						Main_ZoomSetDataF (0x40 , 0x80);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
						MainLayer_ZoomDispF(tF_ZoomIndex,DISP_NORMAL_ON);
						
					}
//					printf("6 TimeCnt:%d , Main_fZoomIndex:0x%X\n",TimeCnt,Main_fZoomIndex);
					MainLayer_ZoomDispF(Main_fZoomIndex,DISP_ON);
					tTimeUnit = TIME_CNT_1SEC;
					tF_ZoomIndex = Main_fZoomIndex;
					tTimeCnt = TimeCnt;
					*ZoomFlag = 1;
					Main_ZoomSetDataF (0x10 , 0x20);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
				}
				else
				{
					MainLayer_ZoomDispF(tF_ZoomIndex,DISP_NORMAL_ON);
					*ZoomFlag=0;
					Main_ZoomSetDataF (0x20 , 0x80);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
					Main_fZoomIndex = 0xFE;
				}
			}
			else
			{
				MainLayer_ZoomDispF(tF_ZoomIndex,DISP_OFF);
				if ((Main_fZoomIndex!=0xFF)&&(Main_fZoomIndex!=0xFE))
				{
					if (Main_fZoomIndex_Time_Disp==1)
					{
						Main_fZoomIndex_Time_Disp = 0;
						Main_ZoomSetDataF (0x40 , 0x20);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
					}
					else
					{
	//					printf("2  Main_fZoomIndex:0x%X\n",Main_fZoomIndex);
						Main_ZoomSetDataF (0x20 , 0x80);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
						Main_ZoomSetDataF (0x40 , 0x80);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
						
					}
//				printf("5 TimeCnt:%d , Main_fZoomIndex:0x%X\n",TimeCnt,Main_fZoomIndex);
					MainLayer_ZoomDispF(Main_fZoomIndex,DISP_ON);
					tTimeUnit = TIME_CNT_1SEC;
					tF_ZoomIndex = Main_fZoomIndex;
					tTimeCnt = TimeCnt;
					*ZoomFlag = 1;
					Main_ZoomSetDataF (0x10 , 0x20);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
				}
				else
				{
					*ZoomFlag=0;
					Main_ZoomSetDataF (0x20 , 0x80);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
					Main_fZoomIndex = 0xFE;
				}
			}
		}
	}	
	else if ((Main_fZoomIndex!=0xFF)&&(Main_fZoomIndex!=0xFE))
	{
		
//		printf("4  Main_fZoomIndex:0x%X\n",Main_fZoomIndex);
		{
//		printf("4 TimeCnt:[%d][%d] , Main_fZoomIndex:0x%X\n",TimeCnt,gMainTimerCnt_10m,Main_fZoomIndex);
			MainLayer_ZoomDispF(Main_fZoomIndex,DISP_ON);
			tTimeUnit = TIME_CNT_1SEC;
			tF_ZoomIndex = Main_fZoomIndex;
			tTimeCnt = TimeCnt;
			*ZoomFlag = 1;
			Main_ZoomSetDataF (0x10 , 0x20);// 0x80:확대 금지 , 40:확대 완료 , 20:확대중 , 10:확대 필요
		}
	}
	return 0;
}
uint8_t Main_Warning_1Disp(uint8_t tMain_Func , uint8_t Main_Func , uint8_t IconIndex ,bool OnFlag)
{
	static uint8_t tIconIndex=0xFF;
	if ((Main_Func&0x03)>0x01)
	{
		if (OnFlag==1)
			MainLayer_WarningDisp(IconIndex,DISP_NORMAL_ON);
		else
			MainLayer_WarningDisp(IconIndex,DISP_OFF);
//		printf("Main_Warning_1Disp ==>%d , (OnFlag&0x01):%d t_Func:%d\n",gMainTimerCnt_10m,OnFlag,tIconIndex);
		return Main_Func;
	}
	else if ((tIconIndex!=IconIndex)||(tMain_Func!=Main_Func))
	{
		MainLayer_WarningDisp(IconIndex,(Main_Func&0x01));
		tIconIndex = IconIndex;		
	//	printf("Main_Warning_1Disp ==>%d , (Main_Func&0x01):%d  t_Func:%d\n",gMainTimerCnt_10m,(Main_Func),tIconIndex);
		return Main_Func;
	}
}


		
bool Main_WarningDisp(uint32_t TimeCnt,bool RT_Flag)
{


	
//00(0x0) : 소등
//01(0x1) : 점등
//10(0x2) : 0.5s 점등, 0.5s 소등 반복
//11(0x3) : 0.3s 점등, 0.3s 소등 반복	

	static uint8_t tModelType = 0;
	static uint8_t ZoomFlag = 0;
	bool OnFlag = 0;
	uint8_t i = 0;
	
	
	static uint8_t stMainWarnigData[WARNING_ICON_CNT]={0,};

	static uint8_t tMain_wZoomIndex = 0;
	

#if 0
//	printf("Main_wZoomIndex ==>  %d , %d\n",tMain_wZoomIndex,Main_wZoomIndex);
	if ((tMain_wZoomIndex != Main_wZoomIndex)&&(Main_wZoomIndex!=0xFE))
	{
//		printf("Main_wZoomIndex ==>  %d , %d , %d\n",tMain_wZoomIndex,Main_wZoomIndex,MainWarnigData[Main_wZoomIndex]);
		for (i=0;i<WARNING_ICON_CNT;i++)
		{
//			printf("MainWarnigData ==>  %d , 0x%02X 0x%02X\n",i,stMainWarnigData[i],MainWarnigData[i]);
			if (((stMainWarnigData[i]&0x80)==0x00)&&((stMainWarnigData[i]&0x03)!=0x00))
			{
				MainWarnigData[i] = MainWarnigData[i]&0x0F;
				MainWarnigData[i] = MainWarnigData[i]|0x80;
				printf("MainWarnigData 2==>  %d , 0x%02X 0x%02X\n",i,stMainWarnigData[i],MainWarnigData[i]);
			}
		}
	}
#endif

	#ifdef USED_ROTATION
	if (RT_Flag==1)
	{
		uint8_t MW_Data[WARNING_ICON_CNT]={0,};
		memcpy (MW_Data,MainWarnigData,sizeof(MainWarnigData));
		memset (MainWarnigData,0,sizeof(MainWarnigData));
		
		
		Main_Warn_2Disp(cW_01_B,cW_02_B)
		Main_WD(cW_03_B);
		Main_WD(cW_04_B);
		Main_WD(cW_05_B);
		Main_WD(cW_06_B);
		Main_WD(cW_07_B);
		Main_WD(cW_08_B);
		Main_WD(cW_09_B);
		Main_WD(cW_10_B);
		Main_WD(cW_11_B);
		Main_WD(cW_12_B);
		Main_Warn_2Disp(cW_13_B,cW_14_B)
		Main_Warn_3Disp(cW_15_B,cW_16_B,cW_17_B)
		Main_Warn_2Disp(cW_18_B,cW_19_B)
		Main_WD(cW_20_B);
		
		memcpy (MainWarnigData,MW_Data,sizeof(MainWarnigData));
		
	}
	else
	#endif
	{
		Main_Warn_2Disp(cW_01_B,cW_02_B)
		Main_WD(cW_03_B);
		Main_WD(cW_04_B);
		Main_WD(cW_05_B);
		Main_WD(cW_06_B);
		Main_WD(cW_07_B);
		Main_WD(cW_08_B);
		Main_WD(cW_09_B);
		Main_WD(cW_10_B);
		Main_WD(cW_11_B);
		Main_WD(cW_12_B);
		Main_Warn_2Disp(cW_13_B,cW_14_B)
		Main_Warn_3Disp(cW_15_B,cW_16_B,cW_17_B)
		Main_Warn_2Disp(cW_18_B,cW_19_B)
		Main_WD(cW_20_B);
	}
	tMain_wZoomIndex = Main_wZoomIndex;


	//if ((Main_wZoomIndex==0xFF)||(Main_wZoomIndex==0xFE)||(Main_fZoomIndex==0xFF)||(Main_fZoomIndex==0xFE))
	
	
//	printf("MainWarnigData ==>  0x%02X  , 0x%02X 0x%02X\n",Main_wZoomIndex,Main_WEngStopSw,Main_WSafetyStart);
	
	
	//if (((Main_wZoomIndex==0xFF)||(Main_wZoomIndex==0xFE))&&((Main_fZoomIndex==0xFF)||(Main_fZoomIndex==0xFE)))
	if ((Main_wZoomIndex==0xFF)||(Main_wZoomIndex==0xFE))
	{
		if ((Main_WSafetyStart&0x03)!=0x00)
		{
			static bool OnFlag = 0;
			static bool FirstFlag = 0;
			static uint32_t tTimeCnt = 0;
			uint8_t TimeUnit;
			{
				if ((Main_WSafetyStart&0x03)==0x03)
				{
					TimeUnit = TIME_CNT_300MS;
					MainLayer_WarningDisp2(cW_EngStopSw,DISP_OFF);
					if (OnFlag != gOnFlag_300ms)
					{
						if (gOnFlag_300ms==0)
							MainLayer_WarningDisp2(cW_SafetyStart,DISP_ON);
						else
						{
							MainLayer_WarningDisp2(cW_SafetyStart,DISP_OFF);
						}
						OnFlag = gOnFlag_300ms;
					}
					FirstFlag=0;
					tTimeCnt = TimeCnt;
				}
				else if ((Main_WSafetyStart&0x03)==0x01)
				{
					if ((Main_WEngStopSw&0x03)==0x03)
					{
						tTimeCnt=(TimeCnt-TIME_CNT_300MS*10)-1;
					}
					if (TimeCnt>(tTimeCnt+TIME_CNT_300MS*10))
					{
						FirstFlag=0;
						MainLayer_WarningDisp2(cW_SafetyStart,DISP_OFF);
						Main_WSafetyStart = 0;
					}
					else
					{
						TimeUnit = TIME_CNT_300MS;
						MainLayer_WarningDisp2(cW_EngStopSw,DISP_OFF);
						if (OnFlag != gOnFlag_300ms)
						{
							if (gOnFlag_300ms==0)
								MainLayer_WarningDisp2(cW_SafetyStart,DISP_ON);
							else
							{
								MainLayer_WarningDisp2(cW_SafetyStart,DISP_OFF);
							}
							OnFlag = gOnFlag_300ms;
						}
						FirstFlag=0;
					}
				}
			}
		}
		else if ((Main_WEngStopSw&0x03)!=0x00)
		{
			static bool OnFlag = 0;
			static bool FirstFlag = 0;
			static uint32_t tTimeCnt = 0;
			uint8_t TimeUnit;
			{
				if ((Main_WEngStopSw&0x03)==0x03)
				{
					MainLayer_WarningDisp2(cW_SafetyStart,DISP_OFF);
					TimeUnit = TIME_CNT_300MS;
					if (OnFlag != gOnFlag_300ms)
					{
						if (gOnFlag_300ms==0)
							MainLayer_WarningDisp2(cW_EngStopSw,DISP_ON);
						else
						{
							MainLayer_WarningDisp2(cW_EngStopSw,DISP_OFF);
						}
						OnFlag = gOnFlag_300ms;
					}
					FirstFlag=0;
					tTimeCnt = TimeCnt;
				}
				else if ((Main_WEngStopSw&0x03)==0x01)
				{
					if (TimeCnt>(tTimeCnt+TIME_CNT_300MS*10))
					{
						FirstFlag=0;
						MainLayer_WarningDisp2(cW_EngStopSw,DISP_OFF);
						Main_WEngStopSw = 0;
					}
					else
					{
						TimeUnit = TIME_CNT_300MS;
						if (OnFlag != gOnFlag_300ms)
						{
							if (gOnFlag_300ms==0)
								MainLayer_WarningDisp2(cW_EngStopSw,DISP_ON);
							else
							{
								MainLayer_WarningDisp2(cW_EngStopSw,DISP_OFF);
							}
							OnFlag = gOnFlag_300ms;
						}
					}
				}
			}
		}	
	}
	else
	{
		MainLayer_WarningDisp2(cW_EngStopSw,DISP_OFF);
		MainLayer_WarningDisp2(cW_SafetyStart,DISP_OFF);
	}
	
	return true;
}

uint8_t Main_Func_1Disp(uint8_t tMain_Func , uint8_t Main_Func , uint8_t IconIndex ,bool OnFlag)
{
	static uint8_t tIconIndex=0xFF;
	if ((Main_Func&0x03)>0x01)
	{
		if (OnFlag==1)
			MainLayer_FunctionDisp(IconIndex,DISP_NORMAL_ON);
		else
			MainLayer_FunctionDisp(IconIndex,DISP_OFF);
//		printf("Main_Warning_1Disp ==>%d , (OnFlag&0x01):%d t_Func:%d\n",gMainTimerCnt_10m,OnFlag,tIconIndex);
		return Main_Func;
	}
	else //if ((tIconIndex!=IconIndex)||(tMain_Func!=Main_Func))
	{
		MainLayer_FunctionDisp(IconIndex,(Main_Func&0x01));
		tIconIndex = IconIndex;		
	//	printf("Main_Warning_1Disp ==>%d , (Main_Func&0x01):%d  t_Func:%d\n",gMainTimerCnt_10m,(Main_Func),tIconIndex);
		return Main_Func;
	}
}
bool Main_FunctionDisp(uint32_t TimeCnt,bool RT_Flag)
{

	static uint8_t tModelType = 0;
	static uint8_t ZoomFlag = 0;
	bool OnFlag = 0;
	uint8_t i = 0;

	
	static uint8_t stMainFunctionData[FUNCTION_ICON_CNT]={0,};

	static uint8_t tMain_fZoomIndex = 0xFF;
	
#if 1		
	if ((tModelType!=ModelType)&&(RotationMode==0))
	{
		uint8_t tData = 0;
		
		printf("Main_FunctionDisp ==>  %d , %d , %d\n",tModelType,ModelType,gCameraModeDisp);
		{
			for (i=0;i<FUNCTION_ICON_CNT;i++)
			{
				{
					stMainFunctionData[i] = 0;
					MainLayer_FunctionIcon(i+cF_01_B,DISP_OFF);
				}
			}
		}
		tMain_fZoomIndex = 0;
		tModelType=ModelType;
	}
#endif	
	
	
#if 0	
//	printf("Main_fZoomIndex ==>  %d , %d\n",tMain_fZoomIndex,Main_fZoomIndex);
	if (((tMain_fZoomIndex != Main_fZoomIndex)&&(Main_fZoomIndex!=0xFE))||(Main_wZoomIndex<WARNING_ICON_CNT))
	{
//		printf("Main_fZoomIndex ==>  %d , %d\n",stMainFunctionData-cF_01_B,Main_fZoomIndex-cF_01_B);
		for (i=0;i<FUNCTION_ICON_CNT;i++)
		{
//			printf("MainFunctionData ==>  %d , 0x%02X 0x%02X\n",i,stMainFunctionData[i],MainFunctionData[i]);
			if (((stMainFunctionData[i]&0x80)==0x00)&&((stMainFunctionData[i]&0x03)!=0x00))
			{
				MainFunctionData[i] = MainFunctionData[i]&0x0F;
				MainFunctionData[i] = MainFunctionData[i]|0x80;
//				printf("MainFunctionData 2==>  %d , 0x%02X 0x%02X\n",i,stMainFunctionData[i],MainFunctionData[i]);
			}
		}
	}
#endif
	tMain_fZoomIndex = Main_fZoomIndex;

	#ifdef USED_ROTATION
	if (RT_Flag==1)
	{
		uint8_t MF_Data[FUNCTION_ICON_CNT]={0,};
		memcpy (MF_Data,MainFunctionData,sizeof(MainFunctionData));
		memset (MainFunctionData,0,sizeof(MainFunctionData));
		
		
		if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
		{
			Main_FD(cF_01_B)
			Main_Func_2Disp(cF_02_B,cF_03_B)
			Main_FD(cF_13_B)
			Main_Func_2Disp(cF_04_B,cF_05_B)
			Main_FD(cF_06_B)
			Main_Func_3Disp(cF_07_B,cF_08_B,cF_09_B)
			Main_FD(cF_12_B)
			Main_Func_2Disp(cF_10_B,cF_11_B)
			Main_Func_2Disp(cF_14_B,cF_15_B)
			Main_Func_2Disp(cF_16_B,cF_17_B)
			Main_FD(cF_18_B)
			Main_FD(cF_19_B)
			Main_Func_3Disp(cF_20_B,cF_21_B,cF_22_B)
		}
		else
		{
			Main_FD(cF_01_B)
			Main_Func_2Disp(cF_02_B,cF_03_B)
			Main_Func_2Disp(cF_04_B,cF_05_B)
			Main_FD(cF_06_B)
			Main_Func_3Disp(cF_07_B,cF_08_B,cF_09_B)
			Main_Func_2Disp(cF_10_B,cF_11_B)
			Main_FD(cF_12_B)
			Main_FD(cF_13_B)
			Main_FD(cF_18_B)
			Main_Func_2Disp(cF_14_B,cF_15_B)
			Main_Func_2Disp(cF_16_B,cF_17_B)
			Main_FD(cF_19_B)
			Main_Func_3Disp(cF_20_B,cF_21_B,cF_22_B)
		}
		
		memcpy (MainFunctionData,MF_Data,sizeof(MainFunctionData));
		
	}
	else
	#endif
	{
		{
			if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
			{
				Main_FD(cF_01_B)
				Main_Func_2Disp(cF_02_B,cF_03_B)
				Main_FD(cF_13_B)
				Main_Func_2Disp(cF_04_B,cF_05_B)
				Main_FD(cF_06_B)
				Main_Func_3Disp(cF_07_B,cF_08_B,cF_09_B)
				Main_FD(cF_12_B)
				Main_Func_2Disp(cF_10_B,cF_11_B)
				Main_Func_2Disp(cF_14_B,cF_15_B)
				Main_Func_2Disp(cF_16_B,cF_17_B)
				Main_FD(cF_18_B)
				Main_FD(cF_19_B)
				Main_Func_3Disp(cF_20_B,cF_21_B,cF_22_B)
			}
			else
			{
				Main_FD(cF_01_B)
				Main_Func_2Disp(cF_02_B,cF_03_B)
				Main_Func_2Disp(cF_04_B,cF_05_B)
				Main_FD(cF_06_B)
				Main_Func_3Disp(cF_07_B,cF_08_B,cF_09_B)
				Main_Func_2Disp(cF_10_B,cF_11_B)
				Main_FD(cF_12_B)
				Main_FD(cF_13_B)
				Main_FD(cF_18_B)
				Main_Func_2Disp(cF_14_B,cF_15_B)
				Main_Func_2Disp(cF_16_B,cF_17_B)
				Main_FD(cF_19_B)
				Main_Func_3Disp(cF_20_B,cF_21_B,cF_22_B)
			}
		}
	}
	
	return true;
}


