/*
 * Copyright (c) 2018 ITE Tech. Inc. All Rights Reserved.
 *
 * @brief Can ctrl test code.
 * @author
 * @version 1.0.0
 *
 */
#include <stdio.h>
#include <malloc.h>
#include "ite/itp.h"    //for all ith driver
#include "ite/ith.h"
//#include "can_bus/it9860/can_api.h"
#include <pthread.h>
#include <sys/time.h>

#include "openrtos/FreeRTOS.h"
#include "openrtos/task.h"
#include "scene.h"
#include "HT_DispDdefine.h"
#include "Menu_Layer.h"

uint16_t Change_kpa2psi(uint16_t DateVal);
uint16_t Change_psi2kpa(uint16_t DataVal);


void HT_gettimeofday(uint16_t *year,uint8_t *mon,uint8_t *mday,uint8_t *hour,uint8_t *min,uint8_t *sec);		// add 230516 jhlee
void HT_settimeofday(uint16_t year,uint8_t mon,uint8_t mday,uint8_t hour,uint8_t min,uint8_t sec);		// add 230516 jhlee

extern void Reset_M8HeepMemory(void);



void HT_Init_GPIO(void)
{

	printf("HT_Init_GPIO\n");
	ithGpioSetMode(HT_MICOM_RST, ITH_GPIO_MODE0);
	ithGpioSet(HT_MICOM_RST);
	ithGpioSetOut(HT_MICOM_RST);
	ithGpioSet(HT_MICOM_RST);

	ithGpioSetMode(HT_GPIO_BUZZER, ITH_GPIO_MODE0);
	ithGpioClear(HT_GPIO_BUZZER);
	ithGpioSetOut(HT_GPIO_BUZZER);
	ithGpioClear(HT_GPIO_BUZZER);
	ithGpioSetMode(HT_GPIO_FLASH_WP, ITH_GPIO_MODE0);
	ithGpioClear(HT_GPIO_FLASH_WP);
	ithGpioSetOut(HT_GPIO_FLASH_WP);
	ithGpioClear(HT_GPIO_FLASH_WP);
}
void HT_Set_FLASH_WP(bool WP_OnOff)
{
	#if 0
	if (WP_OnOff)
	{
		ithGpioClear(HT_GPIO_FLASH_WP);
	}
	else
	{
		ithGpioSet(HT_GPIO_FLASH_WP);
	}
	#endif
}
void HT_Read_GPIO(void)
{

	#if 0 
		if (ithGpioGet(HT_GPIO_IN6)==0)
		{
			gRead_Gpio_Value = (gRead_Gpio_Value&0x3F)| (1<<6);	//6,7bit
		}
		else
		{
			gRead_Gpio_Value = (gRead_Gpio_Value&0x3F)| (0<<6);	//6,7bit
		}
		if (ithGpioGet(HT_GPIO_IN7)==0)
		{
			gRead_Gpio_Value = (gRead_Gpio_Value&0x3F)| (1<<0);	//0,1bit
		}
		else
		{
			gRead_Gpio_Value = (gRead_Gpio_Value&0x3F)| (0<<0);	//0,1bit
		}
		if (ithGpioGet(HT_GPIO_IN8)==0)
		{
			gRead_Gpio_Value = (gRead_Gpio_Value&0x3F)| (1<<2);	//2,3bit
		}
		else
		{
			gRead_Gpio_Value = (gRead_Gpio_Value&0x3F)| (0<<2);	//2,3bit
		}
		if (ithGpioGet(HT_GPIO_IN9)==0)
		{
			gRead_Gpio_Value = (gRead_Gpio_Value&0x3F)| (1<<4);	//4,5bit
		}
		else
		{
			gRead_Gpio_Value = (gRead_Gpio_Value&0x3F)| (0<<4);	//4,5bit
		}				
		
		
	#endif
}
static pthread_mutex_t            HTtimeofdaymutex;


bool HT_timeofday_mutex(void)
{
	pthread_mutex_init(&HTtimeofdaymutex, NULL);
}

void HT_gettimeofday(uint16_t *year,uint8_t *mon,uint8_t *mday,uint8_t *hour,uint8_t *min,uint8_t *sec)
{
	struct timeval tv, tv2;
	unsigned long long start_utime, end_utime;
	struct tm *tm;
	
	pthread_mutex_lock(&HTtimeofdaymutex);
	gettimeofday(&tv,NULL);
	tm = localtime(&tv.tv_sec);
	pthread_mutex_unlock(&HTtimeofdaymutex);
	//printf("  %d %d %d  %d %d %d   %d %d\n", tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec,tm->tm_yday,tm->tm_wday);
	
	*year = tm->tm_year+1900;
	*mon = tm->tm_mon+1;
	*mday = tm->tm_mday;
	*hour = tm->tm_hour;
	*min = tm->tm_min;
	*sec = tm->tm_sec;
	
	
//	printf("The time is %s\n", ctime(&tv.tv_sec));                                     
}

void HT_settimeofday(uint16_t year,uint8_t mon,uint8_t mday,uint8_t hour,uint8_t min,uint8_t sec)
{
	struct timeval tv;
	struct timezone tz;
	struct tm *tm, mytime;
	pthread_mutex_lock(&HTtimeofdaymutex);
	gettimeofday(&tv, &tz);
	tm = localtime(&tv.tv_sec);
	pthread_mutex_unlock(&HTtimeofdaymutex);
	memcpy(&mytime, tm, sizeof (struct tm));
	
	mytime.tm_year = year - 1900;	// 122;//2000 - 1900;   
    mytime.tm_mon = mon-1;	//2;// March  3 - 1;
    mytime.tm_mday = mday;
    
    
    mytime.tm_hour = hour;
    mytime.tm_min = min;
    mytime.tm_sec = sec;
	tv.tv_sec = mktime ( &mytime );
	printf("\nSettime:[%02d]Min [%02d]Sec\n\n", mytime.tm_min,mytime.tm_sec);
	if(settimeofday(&tv,&tz) == -1)
	{
		printf("HT_settimeofday Error.\n");
	}
}

bool BuzzerFlag=0;	//0:OFF , 1:ON(100)OFF(1900) , 2:ON(200)OFF(800) , 3:ON(500)OFF(500) , 4:ON(300)OFF(300)  , 5:ON , 6:ON(100)OFF(9900)  , 7:ON(120)OFF(120) 3Time   , 10:ON(500)OFF

void BuzzerRun( uint32_t TimerCnt_10ms)
{
	static uint32_t tmerCnt_10ms=0;
	static uint8_t sBuzzerMode=0;
	static uint8_t sBuzzer7Cnt=0;
	static uint8_t sBuzzer12Cnt=0;

	if (sBuzzerMode!=gBuzzerMode)
	{
		sBuzzerMode=gBuzzerMode;
		if (sBuzzerMode==0)
			BuzzerFlag=0;
		else
			BuzzerFlag=1;
		tmerCnt_10ms = TimerCnt_10ms;
		sBuzzer7Cnt=0;
		sBuzzer12Cnt=0;
	}
	else
	{
		switch (gBuzzerMode)
		{
			case 0:	//OFF
			{
				BuzzerFlag=0;
				tmerCnt_10ms = TimerCnt_10ms;
			}
			break;
			case 1:	//ON(100)OFF(1900)
			{
				if (BuzzerFlag==1)
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=10)
					{
						BuzzerFlag=0;
						tmerCnt_10ms = TimerCnt_10ms;
					}
				}
				else
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=190)
					{
						BuzzerFlag=1;
						tmerCnt_10ms = TimerCnt_10ms;
					}
				}
			}
			break;
			case 2:	//ON(200)OFF(800)
				if (BuzzerFlag==1)
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=20)
					{
						BuzzerFlag=0;
						tmerCnt_10ms = TimerCnt_10ms;
					}
				}
				else
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=80)
					{
						BuzzerFlag=1;
						tmerCnt_10ms = TimerCnt_10ms;
					}
				}			
			break;
			case 3:	//(500)OFF(500)
				if (BuzzerFlag==1)
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=50)
					{
						BuzzerFlag=0;
						tmerCnt_10ms = TimerCnt_10ms;
					}
				}
				else
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=50)
					{
						BuzzerFlag=1;
						tmerCnt_10ms = TimerCnt_10ms;
					}
				}			
			break;
			case 4:	//(300)OFF(300)
			case 8:	//(300)OFF(300)
				if (BuzzerFlag==1)
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=30)
					{
						BuzzerFlag=0;
						tmerCnt_10ms = TimerCnt_10ms;
					}
				}
				else
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=30)
					{
						BuzzerFlag=1;
						tmerCnt_10ms = TimerCnt_10ms;
					}
				}			
			break;
			case 5:	//ON
			{
				BuzzerFlag=1;
				tmerCnt_10ms = TimerCnt_10ms;
			}
			break;
			case 6:	//ON(100)OFF(9900)
				if (BuzzerFlag==1)
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=10)
					{
						BuzzerFlag=0;
						tmerCnt_10ms = TimerCnt_10ms;
					}
				}
				else
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=990)
					{
						BuzzerFlag=1;
						tmerCnt_10ms = TimerCnt_10ms;
					}
				}			
			break;
			case 7:	//ON(120)OFF(120) 3Time
				if (BuzzerFlag==1)
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=12)
					{
						BuzzerFlag=0;
						tmerCnt_10ms = TimerCnt_10ms;
					}
				}
				else
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=12)
					{
						BuzzerFlag=1;
						tmerCnt_10ms = TimerCnt_10ms;
						sBuzzer7Cnt++;
						if (sBuzzer7Cnt>=3)
						{
							BuzzerFlag=0;
							gBuzzerMode = 0;
							tmerCnt_10ms = TimerCnt_10ms;
						}
					}
				}
			break;
			case 10:	//(500)OFF
				if (BuzzerFlag==1)
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=50)
					{
						BuzzerFlag=0;
						gBuzzerMode = 0;
						tmerCnt_10ms = TimerCnt_10ms;
					}
				}
				else
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=50)
					{
						BuzzerFlag=1;
						tmerCnt_10ms = TimerCnt_10ms;
					}
				}			
			break;
			case 11:	//ON(300) 1Time
				if (BuzzerFlag==1)
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=30)
					{
						BuzzerFlag=0;
						gBuzzerMode = 0;
						tmerCnt_10ms = TimerCnt_10ms;
					}
				}
			break;
			case 12:	//ON(300)OFF(300) 3Time
				if (BuzzerFlag==1)
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=30)
					{
						BuzzerFlag=0;
						tmerCnt_10ms = TimerCnt_10ms;
					}
				}
				else
				{
					if ((TimerCnt_10ms-tmerCnt_10ms)>=30)
					{
						BuzzerFlag=1;
						tmerCnt_10ms = TimerCnt_10ms;
						sBuzzer12Cnt++;
						if (sBuzzer12Cnt>=3)
						{
							BuzzerFlag=0;
							gBuzzerMode = 0;
							tmerCnt_10ms = TimerCnt_10ms;
						}
					}
				}
			break;
		}
	}
	if (BuzzerFlag==1)	// On
	{
	//	printf("BuzzerRun On : [%d] %d %d\n", gBuzzerMode,gBuzzerTimerCnt_10m,gMainTimerCnt_10m);
		ithGpioSet(HT_GPIO_BUZZER); 
	}
	else
	{
	//	printf("BuzzerRun Off : [%d] %d %d\n", gBuzzerMode,gBuzzerTimerCnt_10m,gMainTimerCnt_10m);
		ithGpioClear(HT_GPIO_BUZZER); 
	}
}

#ifdef TEST_ERR_602
extern uint32_t TestCan0TaskTxCnt;
extern uint32_t TestCan0TaskTxFlag;
extern void TestCan0TaskTx(void);
#endif


#ifdef CAN_PARSING_TIMER
	void gMainTimer_handler( timer_t timerid,int myarg )
	{
		static uint32_t OnTimerCnt_10ms = 0;
	
		OnTimerCnt_10ms++;
		gMainTimerCnt_10m++;
		
	
		if ((gMainTimerCnt_10m%(TIME_CNT_1SEC*10))==0)
		{
			gOnFlag_1S ^= 1;
		}
		if ((gMainTimerCnt_10m%(TIME_CNT_500MS*10))==0)
		{
			gOnFlag_500ms ^= 1;
		}
		if ((gMainTimerCnt_10m%(TIME_CNT_300MS*10))==0)
		{
			gOnFlag_300ms ^= 1;
		}
		if ((gMainTimerCnt_10m%(10))==0)
		{
			CanbusTxDataManagement( gMainTimerCnt_10m,0,NULL);
		}	
		if ((gMainTimerCnt_10m%(TIME_CNT_50MS))==0)
		{
			MICOM_RxParsing();
		}
		
		DM14_Timer();
		#ifdef DEMO_MODE_PLAY
		{
			void CAN_Data_Save(void);
			CAN_Data_Save();
		}
		#endif
		#ifdef TEST_FLASH_MEM
		{
			void Test_Flash_Save(void);
			Test_Flash_Save();
		}
		#endif
	}
#else
	void gMainTimer_handler( timer_t timerid,int myarg )
	{
		static uint32_t OnTimerCnt_10ms = 0;
	
		OnTimerCnt_10ms++;
		gMainTimerCnt_10m++;
		
		#ifdef TEST_ERR_602
		TestCan0TaskTx();
		TestCan0TaskTx();
		#endif
		if ((gMainTimerCnt_10m%(TIME_CNT_1SEC))==0)
		{
			gOnFlag_1S ^= 1;
		}
		if ((gMainTimerCnt_10m%(TIME_CNT_500MS))==0)
		{
			gOnFlag_500ms ^= 1;
		}
		if ((gMainTimerCnt_10m%(TIME_CNT_300MS))==0)
		{
			gOnFlag_300ms ^= 1;
		}
//		if ((gMainTimerCnt_10m%(10))==0)
		{
			CanbusTxDataManagement( gMainTimerCnt_10m,0,NULL);
		}
		#ifdef USED_ROTATION
		if (((RotationModeTime+TIME_CNT_300MS)<gMainTimerCnt_10m)&&(RotationMode == 1))
		{
			RotationMode = 2;
			ithGpioClear(HT_GPIO_BUZZER);
			if (RotationButtonOnPress()==0)
			{
				ReadyRotation();
			}
		}
		#endif
		
		DM14_Timer();
		BuzzerRun( gMainTimerCnt_10m );
		#ifdef DEMO_MODE_PLAY
		{
			void CAN_Data_Save(void);
			CAN_Data_Save();
		}
		#endif
		#ifdef TEST_FLASH_MEM
		{
			void Test_Flash_Save(void);
			Test_Flash_Save();
		}
		#endif
	}
#endif


void gMain_timer(void)
{
	timer_t timerid;
	struct itimerspec	value;
	
	value.it_value.tv_sec = 1;
	value.it_value.tv_nsec = 0;
	
	value.it_interval.tv_sec = 0;
	#ifdef CAN_PARSING_TIMER
		value.it_interval.tv_nsec = 1*1000000;
	#else
		value.it_interval.tv_nsec = 10*1000000;
	#endif
	timer_create (CLOCK_REALTIME, NULL, &timerid);
	timer_connect (timerid, gMainTimer_handler,0);
	timer_settime (timerid, 0, &value, NULL);
	
	//sleep(5);
	//timer_delete(timerid);
	
}

uint16_t Change_Lm2gpm(uint16_t DateVal)
{
	uint32_t OutData=0;
	if (DateVal==0)
		return 0;
 	OutData = (DateVal*1585)/6+500;
 	OutData = OutData/1000;
 	return OutData;
}
uint16_t Change_gpm2Lm(uint16_t DateVal)
{
	uint32_t OutData=0;
	if (DateVal==0)
		return 0;
 	OutData = (DateVal*10000)+(2642/2);
 	OutData = OutData/2642;
 	return OutData;
}
uint16_t Change_kpa2psi(uint16_t DateVal)
{
	uint32_t OutData=0;
	if (DateVal==0)
		return 0;
 	OutData = (DateVal*145)+500;
 	OutData = OutData/1000;
 	return (OutData);
}

uint16_t Change_psi2kpa(uint16_t DataVal)
{
	uint32_t OutData=0;
	if (DataVal==0)
		return 0;
 	OutData = (DataVal*6895)+500;
 	OutData = OutData/1000;
 	return (OutData);
}

uint16_t Change_mpa2psi(uint16_t DateVal)
{
	uint32_t OutData=0;
	if (DateVal==0)
		return 0;
 	OutData = (DateVal*145)+500;
 	OutData = OutData/1000;
 	return (OutData);
}
uint16_t Change_psi2mpa(uint16_t DateVal)
{
	uint32_t OutData=0;
	if (DateVal==0)
		return 0;
 	OutData = (DateVal*6895)+(1000000/2);
 	OutData = OutData/1000000;
 	return OutData;
}




bool LCDLayer_BackLight(int8_t BL_Data)
{ 
	static uint32_t tBrightness = 0x5A5A5A5AFF;
	//theConfig.brightness = (BL_Data-1)/5;
	theConfig.brightness = BL_Data;
	
	printf("LCDLayer_BackLight=%d %d %d\n",tBrightness,BL_Data,theConfig.brightness);
	if (tBrightness != theConfig.brightness)
	{
		ScreenSetBrightness(theConfig.brightness);
		tBrightness = theConfig.brightness;
	}
}

void SendSoftID(void)
{
	uint8_t  txbuffer[10] = {0x00,};

#if 1
	txbuffer[0] = 7;
	txbuffer[1] = 'V';
	txbuffer[2] = CFG_VERSION_MAJOR|0x30;
	txbuffer[3] = '.';
	txbuffer[4] = CFG_VERSION_MINOR|0x30;
	txbuffer[5] = CFG_VERSION_PATCH|0x30;
	txbuffer[6] = gMicomFirmVer[5];
	txbuffer[7] = gMicomFirmVer[6];
#else
	txbuffer[0] = 5;
	txbuffer[1] = 'V';
	txbuffer[2] = CFG_VERSION_MAJOR|0x30;
	txbuffer[3] = '.';
	txbuffer[4] = CFG_VERSION_MINOR|0x30;
	txbuffer[5] = CFG_VERSION_PATCH|0x30;
#endif
	printf("SendSoftID=[%s]\n",txbuffer);
	CanbusTxDataManagement( gMainTimerCnt_10m,SOFT_ID,txbuffer);
}


void ReadyRotation(void)
{
	RotationMode = 0;	// 0:off , 1:ing , 2:run
	RotationModeTime = 0;
	//ScreenSetOnOff(1);
}
bool ResetRotation(void)
{
	ResetMain();
	ResetEngnCool();
	ResetCamera();
	ResetUI();
	Reset_M8HeepMemory();   //  �켱 ���� ���߿�Ȯ�� JPHong 2024-03-08 
	
	ResetMenu();
	return true;
}

uint8_t RotationDir(void)
{
	#ifdef USED_ROTATION
		return PL_Flag_Manual;
	#else
		return PL_Flag_New_Value;
	#endif
}


void HT_RunRotation (void)
{
#if 0
	if (Main_Disp_Mode == MAIN_MODE_CAMERA)
#else
	if (1)
#endif
	{
		if (RotationMode==0)		// 0:off , 1:ing , 2:run
		{
			RotationModeTime = gMainTimerCnt_10m;
			RotationMode = 1;
			//CameraOnLeaveCallSrc(22);	// JPHong 2024-05-10
			//CameraOnLeave4Rot(22);
			RotationModeTime = gMainTimerCnt_10m;
			RotationMode = 1;		// 0:off , 1:ing , 2:run
		}
	}
	else

	{
		if (RotationMode==0)
		{
			RotationModeTime = gMainTimerCnt_10m;
			RotationMode = 1;
		}
	}
}


// #define USER_01_ONLY

void TestSetUserData(void)
{
	int i=0;

	#ifdef USER_01_ONLY
		i = 0;
		User_RegInfo[i++].RegFlag = 5;
		User_RegInfo[i++].RegFlag = 9;
		
		i = 0;
		sprintf(User_RegInfo[i++].UserName,"Super Hong");
		sprintf(User_RegInfo[i++].UserName,"Hong Jeong Pyo");
		
		i = 0;
		User_RegInfo[i++].bRegister = 1;
		User_RegInfo[i++].bRegister = 1;
		
		i = 0;
		Lang_Flag[i++] = 0;
		Lang_Flag[i++] = 1;
	#else
		i = 0;
		User_RegInfo[i++].RegFlag = 0;
		User_RegInfo[i++].RegFlag = 1;
		User_RegInfo[i++].RegFlag = 2;
		User_RegInfo[i++].RegFlag = 3;
		User_RegInfo[i++].RegFlag = 4;
		User_RegInfo[i++].RegFlag = 5;
		User_RegInfo[i++].RegFlag = 6;
		User_RegInfo[i++].RegFlag = 7;
		User_RegInfo[i++].RegFlag = 8;
		User_RegInfo[i++].RegFlag = 9;
		User_RegInfo[i++].RegFlag = 10;
		User_RegInfo[i++].RegFlag = 11;
		User_RegInfo[i++].RegFlag = 12;
		User_RegInfo[i++].RegFlag = 13;
		User_RegInfo[i++].RegFlag = 14;
		
		i = 0;
		sprintf(User_RegInfo[i++].UserName,"Super Hong_0");
		sprintf(User_RegInfo[i++].UserName,"Hong Jeong Pyo_1");
		sprintf(User_RegInfo[i++].UserName,"TAKEUCHI_2");
		sprintf(User_RegInfo[i++].UserName,"SPK_XO_3");
		sprintf(User_RegInfo[i++].UserName,"BatMan_4");
		sprintf(User_RegInfo[i++].UserName,"SuperMan_5");
		sprintf(User_RegInfo[i++].UserName,"IronMan_6");
		sprintf(User_RegInfo[i++].UserName,"XO800_7");
		sprintf(User_RegInfo[i++].UserName,"JPHong_8");
		sprintf(User_RegInfo[i++].UserName,"NewJeans_9");
		sprintf(User_RegInfo[i++].UserName,"NewJeans_10");
		sprintf(User_RegInfo[i++].UserName,"NewJeans_11");
		sprintf(User_RegInfo[i++].UserName,"NewJeans_12");
		sprintf(User_RegInfo[i++].UserName,"NewJeans_13");
		sprintf(User_RegInfo[i++].UserName,"NewJeans_14");
		
		i = 0;
		User_RegInfo[i++].bRegister = 1;
		User_RegInfo[i++].bRegister = 1;
		User_RegInfo[i++].bRegister = 1;
		User_RegInfo[i++].bRegister = 1;
		User_RegInfo[i++].bRegister = 1;
		User_RegInfo[i++].bRegister = 1;
		User_RegInfo[i++].bRegister = 1;
		User_RegInfo[i++].bRegister = 1;
		User_RegInfo[i++].bRegister = 1;
		User_RegInfo[i++].bRegister = 1;
		User_RegInfo[i++].bRegister = 1;
		User_RegInfo[i++].bRegister = 1;
		User_RegInfo[i++].bRegister = 1;
		User_RegInfo[i++].bRegister = 1;
		User_RegInfo[i++].bRegister = 1;
		
		i = 0;
		Lang_Flag[i++] = 0;
		Lang_Flag[i++] = 1;
		Lang_Flag[i++] = 2;
		Lang_Flag[i++] = 3;
		Lang_Flag[i++] = 0;
		Lang_Flag[i++] = 1;
		Lang_Flag[i++] = 2;
		Lang_Flag[i++] = 3;
		Lang_Flag[i++] = 0;
		Lang_Flag[i++] = 1;
		Lang_Flag[i++] = 2;
		Lang_Flag[i++] = 3;
		Lang_Flag[i++] = 4;
		Lang_Flag[i++] = 5;
		Lang_Flag[i++] = 6;
	#endif	

	
	
	XO_Save_Config(g_nSU100_EditUserIdx,true,true);
}


uint8_t parseBytes(char *InData,uint8_t *OutData,uint8_t length) 
{
	char tmp[3]; 
	int i; 
	uint8_t bytes=0;
	
	// ���� ���ڿ� ���̰� 2�� �����?�ƴ϶��?������ ���� ���ڿ��̹Ƿ� NULL ��ȯ
	if ((length % 2) != 0) return NULL;

	
	// hex ���ڿ� 2���ڸ� �ӽ������Ͽ� 16���������� ��ȯ�ϰ� bytes�� ���� 
	for (i = 0; i <length / 2; i++) 
	{ 
		memcpy(tmp, InData + (i * 2), 2);
		tmp[2] = 0; 
		OutData[i] = (unsigned char)strtoul(tmp, NULL, 16);
	}
	return i;
} 

extern uint8_t* MicomFileBuff;
uint8_t* MicomFileSendBuff = NULL;
extern int MicomFileSize;
uint8_t MicomFileMode=0;


#define HEX_START_ADDR 			0
#define HEX_DATA_LENGTH_ADDR	1
#define HEX_FLASH_ADDR_ADDR		3
#define HEX_REC_ADDR			7
#define HEX_START_DATA_ADDR		9

#define HEX_START_DATA	0x3A	//":"

#define HEX_REC_DATA_CMD	0
#define HEX_REC_END_CMD		1
#define HEX_REC_ES_CMD		2
#define HEX_REC_SS_CMD		3
#define HEX_REC_EA_CMD		4
#define HEX_REC_SA_CMD		5

uint8_t g_nFac_TestModeFlag;	//chpark.24.05.08
uint8_t g_nTest_Step=0;//chpark.24.06.03

uint8_t MicomFirm_UP_Flag = 0;
uint8_t MicomFirm_ReciveData = 0;
uint32_t MicomFirmDataSize=0;
uint32_t CheckMicomAckTime;
uint32_t CheckMicomAckTimeRetry;
uint8_t CheckMicomAck(void)
{
	uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	txbuffer[0] = MicomFileMode;
	txbuffer[1] = (MicomFirmDataSize>>24)&0xFF;
	txbuffer[2] = (MicomFirmDataSize>>16)&0xFF;
	txbuffer[3] = (MicomFirmDataSize>>8)&0xFF;
	txbuffer[4] = (MicomFirmDataSize>>0)&0xFF;

	CanbusTxDataManagement( gMainTimerCnt_10m,XO_MICOM_FIRM_UP,txbuffer);
}
uint8_t CheckMicomFirm(void)
{
	uint32_t ReadAddr = 0;
	uint32_t WriteAddr = 0;
	uint32_t FlashAddr = 0;
	uint8_t CheckSum = 0;
	uint32_t i = 0;
	uint8_t TempBuff[50];
	uint32_t DataSize = 0;
	uint32_t TempData = 0;
	uint8_t StartFlag=0;
	uint8_t EndFlag=0;
	
	
	MicomFileMode=0;
	MicomFileSendBuff = malloc(MicomFileSize);
	
	WriteAddr = 0;
	MicomFileSendBuff[WriteAddr++] = HEX_START_DATA;
	MicomFileSendBuff[WriteAddr++] = HEX_START_DATA;
	
	
	do
	{
		if (MicomFileBuff[ReadAddr] == HEX_START_DATA)
		{
			memset (TempBuff,0,sizeof(TempBuff));
			
			parseBytes(&MicomFileBuff[ReadAddr+HEX_DATA_LENGTH_ADDR],&DataSize,2);
			
//			printf("addr : [0x%X],[%d]\n",ReadAddr,DataSize);
			
			
			parseBytes(&MicomFileBuff[ReadAddr+HEX_DATA_LENGTH_ADDR],&TempBuff,(DataSize+5)*2);
	
//			for (i=0;i<(DataSize+5);i++)
//				printf("0x%02X,",TempBuff[i]);
//			printf("\r\n");
			
			CheckSum = 0;
			//  Check CheckSum
			{
				for (i=0;i<(DataSize+4);i++)
				{
					CheckSum += TempBuff[i];
				//	printf("Check Sum   : 0x%X,0x%X\n",CheckSum,TempBuff[i]);
				}
				CheckSum = ~CheckSum+1;
				if (CheckSum!=TempBuff[DataSize+4])
				{
					printf("Check Sum error : 0x%X,0x%X\n",CheckSum,TempBuff[DataSize+4]);
					
					return 0;
				}
			}
			
			if (TempBuff[3] == HEX_REC_DATA_CMD)
			{
				memcpy (&MicomFileSendBuff[WriteAddr],&TempBuff[4],DataSize);
				WriteAddr += DataSize;		
//				printf("ReadAddr:[%d],WriteAddr:[%d]\n",ReadAddr,WriteAddr);
			}
			else if (TempBuff[3] == HEX_REC_EA_CMD)
			{
				if ((TempBuff[4] == 0x08)&&(TempBuff[5] == 0x00))	// MICOM_FIRM_MAIN 0x01  addr : 0x08000000
				{
					printf("Start File : MICOM_FIRM_MAIN\n" );
					MicomFileMode=MICOM_FIRM_MAIN_READY;
				}
				else if ((TempBuff[4] == 0x08)&&(TempBuff[5] == 0x01))	// MICOM_FIRM_UPFIRM 0x02  addr : 0x08010000
				{
					printf("Start File : MICOM_FIRM_UPFIRM\n" );
					MicomFileMode=MICOM_FIRM_UPFIRM_READY;
				}
				else
				{
					printf("Start File Error : Start Addres Error [0x%X][0x%X]\n",TempBuff[4],TempBuff[5]);
					return 0;
				}
				StartFlag=1;
			}
			else if (TempBuff[3] == HEX_REC_SA_CMD)
			{
				printf("End File\n" );
				ReadAddr += (DataSize)*2+13;
				if (MicomFileBuff[ReadAddr] == HEX_START_DATA)
				{
					parseBytes(&MicomFileBuff[ReadAddr+1],&TempBuff,(DataSize+5)*2);
					if ((TempBuff[0]==0x00)&&(TempBuff[1]==0x00)&&(TempBuff[2]==0x00)&&(TempBuff[3]==0x01)&&(TempBuff[4]==0xFF))
					{
						EndFlag=1;
					}
					else
						printf("EndFlag Error\n");
				}
				else
				{
					printf("Last Sync error : 0x%X,%c,%d,0x%X\n",MicomFileBuff[ReadAddr],MicomFileBuff[ReadAddr],ReadAddr,HEX_START_DATA);
					return 0;
				}
			}
			ReadAddr += (DataSize)*2+13;
		}
		else
		{
			printf("Start Sync error : 0x%X,%c,%d,0x%X\n",MicomFileBuff[ReadAddr],MicomFileBuff[ReadAddr],ReadAddr,HEX_START_DATA);
			return 0;
		}
	}while (((StartFlag==0)||(EndFlag==0))&&(ReadAddr<MicomFileSize));
	MicomFirmDataSize = WriteAddr+4; // CheckSum 4byte
	printf("End CheckMicomFirm : [%d][%d][%d]\n",ReadAddr,MicomFileSize,MicomFirmDataSize);
	
	CheckMicomAck();
	CheckMicomAckTime = gMainTimerCnt_10m;
	CheckMicomAckTimeRetry = gMainTimerCnt_10m + TIME_CNT_500MS;
	MicomFirm_ReciveData=0;
	return 1;
}

uint8_t CheckMicomReady(void)
{				
	uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

	if ((MicomFirm_ReciveData!=0)||((CheckMicomAckTime+TIME_CNT_5SEC)<gMainTimerCnt_10m))
	{
		if (MicomFirm_ReciveData!=0)
		{
			switch (MicomFirm_ReciveData)
			{
				case MICOM_FIRM_MAIN_READY:
				{
					MicomFirm_UP_Flag = 2;
					Goto_MC100_Mode(11);//chpark.24.05.14
				}
				break;
				case MICOM_FIRM_UPFIRM_READY:
				{
					printf("MICOM_FIRM_UPFIRM_READY\n");
					MicomFirm_UP_Flag = 2;
					Goto_MC100_Mode(11);//chpark.24.05.14
				}
				break;
				default:
				{
					MicomFirm_ReciveData = MICOM_READY_ERROR;
					Goto_MC100_Mode(12);//chpark.24.05.14
					printf("CheckMicomReady Error!!!!1\n");
				}
			}
		}
		else
		{
			printf("CheckMicomReady Error!!!!2\n");
			MicomFirm_ReciveData = MICOM_READY_ERROR;
		}

		printf("End CheckMicomReady : [0x%X][%d][%d]\n",MicomFirm_ReciveData,CheckMicomAckTime,gMainTimerCnt_10m);
	}
	else
	{
		if ((CheckMicomAckTimeRetry+TIME_CNT_500MS)<gMainTimerCnt_10m)
		{
			CheckMicomAck();
			CheckMicomAckTimeRetry = gMainTimerCnt_10m;
		}
	}

	return 1;
}
uint8_t SendMicomFirm(void)
{
	int i;
	uint32_t CheckSum = 0;
	uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	Goto_MC100_Mode(20);//chpark.24.05.14
	
	for(i=0;i<MicomFirmDataSize-4;i++)
	{
		CheckSum = CheckSum+MicomFileSendBuff[i];
	}
	CheckSum = ~CheckSum+1;
	
	printf("SendMicomFirm CheckSum:0x%08X\n",CheckSum);
	
	MicomFileSendBuff[MicomFirmDataSize-4] = (CheckSum>>24)&0xFF;
	MicomFileSendBuff[MicomFirmDataSize-3] = (CheckSum>>16)&0xFF;
	MicomFileSendBuff[MicomFirmDataSize-2] = (CheckSum>>8)&0xFF;
	MicomFileSendBuff[MicomFirmDataSize-1] = (CheckSum>>0)&0xFF;
	
	for(i=0;i<MicomFirmDataSize;i=i+8)
	{
		memcpy(&txbuffer[0],&MicomFileSendBuff[i],8);
		CanbusTxDataManagement( gMainTimerCnt_10m,XO_MICOM_FIRM_DATA,txbuffer);
		SDL_Delay(5);
		if(i==MicomFirmDataSize/10*1)
		{
			MC100_PregressUpdate(10);//chpark.24.05.14
		}
		else if(i==MicomFirmDataSize/10*2)
		{
			MC100_PregressUpdate(20);//chpark.24.05.14
		}
		if(i==MicomFirmDataSize/10*3)
		{
			MC100_PregressUpdate(30);//chpark.24.05.14
		}
		else if(i==MicomFirmDataSize/3*2)
		{
			MC100_PregressUpdate(75);//chpark.24.05.14
		}
	}
	printf("End SendMicomFirm : [%d][%d]\n",i,MicomFirmDataSize);
	MicomFirm_UP_Flag = 3;
	CheckMicomAckTime = gMainTimerCnt_10m;
	MicomFirm_ReciveData=0;

	Goto_MC100_Mode(21);//chpark.24.05.14
	return 1;
}

uint8_t WriteMicomFirm(void)
{
	uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	Goto_MC100_Mode(30);//chpark.24.05.14

	if ((MicomFirm_ReciveData!=0)||((CheckMicomAckTime+TIME_CNT_10SEC)<gMainTimerCnt_10m))
	{
		if (MicomFirm_ReciveData==MICOM_FIRM_WRITE_END)
		{
			Goto_MC100_Mode(101);//chpark.24.05.14
			printf("End Write : [0x%X][%d][%d]\n",MicomFirm_ReciveData,CheckMicomAckTime,gMainTimerCnt_10m);
		}
		else
		{
			Goto_MC100_Mode(102);//chpark.24.05.14
			printf("WriteMicomFirm Error!!!!: [0x%X][%d][%d]\n",MicomFirm_ReciveData,CheckMicomAckTime,gMainTimerCnt_10m);
		}
		MicomFirm_UP_Flag = 5;
	}

	return 1;
}
						
						
								
						