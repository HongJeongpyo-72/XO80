#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ctrlboard.h"

#include "HT_DispDdefine.h"

#include "Menu_Layer.h"

#include <math.h>

/* widgets:
Menu5_Layer
Menu_DataBG
M5_9_NA
M5_8_NA
M5_7_NA
M5_6_NA
M5_5_NA
M5_4b_NA
M5_4a_NA
M5_3b_NA
M5_3a_NA
M5_2b_NA
M5_2a_NA
M5_1_NA
M5_TopUserName
M5_TopStandard
M5_TopAdmin
M5_9_Value
M5_8_Value
M5_7_Value
M5_6_Value
M5_5_Value
M5_4b_Value
M5_4a_Value
M5_3b_Value
M5_3a_Value
M5_2b_Value
M5_2a_Value
M5_1_Value
M5_D7_BTN
M5_D6_BTN
M5_D5_BTN
*/

static ITUText *M5_1_Value;
static ITUText *M5_2a_Value;
static ITUText *M5_2b_Value;
static ITUText *M5_3a_Value;
static ITUText *M5_3b_Value;
static ITUText *M5_4a_Value;
static ITUText *M5_4b_Value;
static ITUText *M5_5_Value;
static ITUText *M5_6_Value;
static ITUText *M5_7_Value;
static ITUText *M5_8_Value;
static ITUText *M5_9_Value;

static ITUText *M5_1_NA;
static ITUText *M5_2a_NA;
static ITUText *M5_2b_NA;
static ITUText *M5_3a_NA;
static ITUText *M5_3b_NA;
static ITUText *M5_4a_NA;
static ITUText *M5_4b_NA;
static ITUText *M5_5_NA;
static ITUText *M5_6_NA;
static ITUText *M5_7_NA;
static ITUText *M5_8_NA;
static ITUText *M5_9_NA;

uint16_t M5_RPM = 0xFFFF;				// CAN[EEC1] 3-4byte 0~8031.875 (0~0xFAFF)(0xFExx:Error , 0xFFxx:N/A) rpm (0.125/bit)
uint8_t M5_WTemp = 0xFF;				// CAN[ET1] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
uint8_t M5_OTemp = 0xFF;				// CAN[SI] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
uint8_t M5_UTemp = 0xFF;				// CAN[AT1T1I] 1byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
uint16_t M5_Fuel = 0xFFFF;	// JPHong 0823		// CAN[DD] 1byte (0~250;0~0xFA) (0xFF:N/A) %(0.4/bit)
uint16_t M5_Urea = 0xFFFF;	// JPHong 0823			// CAN[AT1T1I] 0byte (0~250;0~0xFA) (0xFF:N/A) %(0.4/bit)
uint32_t M5_Batt = 0xFFFFFFFF;	// JPHong 0823				// CAN[VEP1] 4~5byte (0~3212.75;0~0xFAFF) (0xFFFF:N/A) V(0.05/bit)
uint8_t M5_PM = 0xFF;					// CAN[AT1S] 2byte (0~250;0~0xFA) (0xFF:N/A) %(1/bit)
uint8_t M5_ASH = 0xFF;					// CAN[AT1S] 1byte (0~250;0~0xFA) (0xFF:N/A) %(1/bit)

uint32_t M5_RPM_Timer=0;
uint32_t M5_WTemp_Timer=0;
uint32_t M5_OTemp_Timer=0;
uint32_t M5_UTemp_Timer=0;
uint32_t M5_Fuel_Timer=0;
uint32_t M5_Urea_Timer=0;
uint32_t M5_Batt_Timer=0;
uint32_t M5_PM_Timer=0;
uint32_t M5_ASH_Timer=0;

static uint8_t M5_NA_Flag[9];

static uint32_t M5_SecTimer=0;
static uint8_t M5_ExitCNT = 0;
static double tUL_Value = 0;

bool M5_UserDisplay(void);

extern void GotoMenuTop(void);

bool M5_Enter(ITUWidget* widget, char* param)
{
	char ptString[10];

	signed int M5_WTemp_C=0,M5_WTemp_F=0;
	signed int M5_OTemp_C=0,M5_OTemp_F=0;
	signed int M5_UTemp_C=0,M5_UTemp_F=0;
	
	M5_1_Value = ituSceneFindWidget(&theScene, "M5_1_Value");
	assert(M5_1_Value);
	M5_2a_Value = ituSceneFindWidget(&theScene, "M5_2a_Value");
	assert(M5_2a_Value);
	M5_2b_Value = ituSceneFindWidget(&theScene, "M5_2b_Value");
	assert(M5_2b_Value);
	M5_3a_Value = ituSceneFindWidget(&theScene, "M5_3a_Value");
	assert(M5_3a_Value);
	M5_3b_Value = ituSceneFindWidget(&theScene, "M5_3b_Value");
	assert(M5_3b_Value);
	M5_4a_Value = ituSceneFindWidget(&theScene, "M5_4a_Value");
	assert(M5_4a_Value);
	M5_4b_Value = ituSceneFindWidget(&theScene, "M5_4b_Value");
	assert(M5_4b_Value);
	M5_5_Value = ituSceneFindWidget(&theScene, "M5_5_Value");
	assert(M5_5_Value);
	M5_6_Value = ituSceneFindWidget(&theScene, "M5_6_Value");
	assert(M5_6_Value);
	M5_7_Value = ituSceneFindWidget(&theScene, "M5_7_Value");
	assert(M5_7_Value);
	M5_8_Value = ituSceneFindWidget(&theScene, "M5_8_Value");
	assert(M5_8_Value);
	M5_9_Value = ituSceneFindWidget(&theScene, "M5_9_Value");
	assert(M5_9_Value);

	M5_1_NA = ituSceneFindWidget(&theScene, "M5_1_NA");
	assert(M5_1_NA);
	M5_2a_NA = ituSceneFindWidget(&theScene, "M5_2a_NA");
	assert(M5_2a_NA);
	M5_2b_NA = ituSceneFindWidget(&theScene, "M5_2b_NA");
	assert(M5_2b_NA);
	M5_3a_NA = ituSceneFindWidget(&theScene, "M5_3a_NA");
	assert(M5_3a_NA);
	M5_3b_NA = ituSceneFindWidget(&theScene, "M5_3b_NA");
	assert(M5_3b_NA);
	M5_4a_NA = ituSceneFindWidget(&theScene, "M5_4a_NA");
	assert(M5_4a_NA);
	M5_4b_NA = ituSceneFindWidget(&theScene, "M5_4b_NA");
	assert(M5_4b_NA);
	M5_5_NA = ituSceneFindWidget(&theScene, "M5_5_NA");
	assert(M5_5_NA);
	M5_6_NA = ituSceneFindWidget(&theScene, "M5_6_NA");
	assert(M5_6_NA);
	M5_7_NA = ituSceneFindWidget(&theScene, "M5_7_NA");
	assert(M5_7_NA);
	M5_8_NA = ituSceneFindWidget(&theScene, "M5_8_NA");
	assert(M5_8_NA);
	M5_9_NA = ituSceneFindWidget(&theScene, "M5_9_NA");
	assert(M5_9_NA);

#if 0
	//M5_RPM = 2500;	// dev.opt
	if(M5_RPM == 0xFFFF)				// CAN[EEC1] 3-4byte 0~8031.875 (0~0xFAFF)(0xFExx:Error , 0xFFxx:N/A) rpm (0.125/bit)
	{
		memset(ptString,0,10);
		sprintf(ptString,"N/A   ");
		ituTextSetString(M5_1_Value, ptString);
		ituWidgetSetVisible(M5_1_Value, true);
	}
	else
	{
		memset(ptString,0,10);
		
		if(M5_RPM > 8031)			// add 230621 jhlee
			M5_RPM = 8031;
			
		sprintf(ptString,"%4d",M5_RPM);
		ituTextSetString(M5_1_Value, ptString);
		ituWidgetSetVisible(M5_1_Value, true);
	}
#else
{       
	uint32_t tData = 0;

	memset(ptString,0,10);
	if ((DTVal.EEC1_34Bytes==0xFFFFFFFF)||(DTVal.EEC1_34Cnt==0))
	{
		DTVal.EEC1_34Cnt = 0;
		DTVal.EEC1_34Bytes=0;

		sprintf(ptString,"N/A");
		ituTextSetString(M5_1_NA, ptString);
		
		M5_NA_Flag[0] = 1;
	}
	else
	{
		tData = DTVal.EEC1_34Bytes/DTVal.EEC1_34Cnt;
		DTVal.EEC1_34Bytes = 0;
		DTVal.EEC1_34Cnt = 0;

		sprintf(ptString, "%d", tData);
		ituTextSetString(M5_1_Value, ptString);

		M5_NA_Flag[0] = 0;
	}

	if(M5_NA_Flag[0] == 0)
	{
		ituWidgetSetVisible(M5_1_NA,false);
		ituWidgetSetVisible(M5_1_Value,true);
	}
	else if(M5_NA_Flag[0] == 1)
	{
		ituWidgetSetVisible(M5_1_Value,false);
		ituWidgetSetVisible(M5_1_NA,true);
	}
}
#endif
	//M5_WTemp = 10;
	if(M5_WTemp == 0xFF)				// CAN[ET1] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
	{
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_2a_NA, ptString);
		ituTextSetString(M5_2b_NA, ptString);
		
		M5_NA_Flag[1] = 1;

		ituWidgetSetVisible(M5_2a_Value,false);
		ituWidgetSetVisible(M5_2a_NA, true);

		ituWidgetSetVisible(M5_2b_Value,false);
		ituWidgetSetVisible(M5_2b_NA, true);
	}
	else
	{
		if(M5_WTemp > 250)
			M5_WTemp = 250;
			
		M5_WTemp_C = M5_WTemp-40;
		tUL_Value = round((((double)M5_WTemp_C*9)/5))+32;
		M5_WTemp_F = (signed int)tUL_Value;
		//M5_WTemp_F = ((M5_WTemp_C*9)/5)+32;

		memset(ptString,0,10);
		sprintf(ptString,"%4d",M5_WTemp_C);
		ituTextSetString(M5_2a_Value, ptString);

		memset(ptString,0,10);
		sprintf(ptString,"%4d",M5_WTemp_F);
		ituTextSetString(M5_2b_Value, ptString);

		M5_NA_Flag[1] = 0;

		ituWidgetSetVisible(M5_2a_NA, false);
		ituWidgetSetVisible(M5_2a_Value, true);

		ituWidgetSetVisible(M5_2b_NA, false);
		ituWidgetSetVisible(M5_2b_Value, true);
	}

	//M5_OTemp = 0xFF;
	if(M5_OTemp == 0xFF)				// CAN[SI] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
	{
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_3a_NA, ptString);
		ituTextSetString(M5_3b_NA, ptString);

		M5_NA_Flag[2] = 1;

		ituWidgetSetVisible(M5_3a_Value, false);
		ituWidgetSetVisible(M5_3a_NA, true);

		ituWidgetSetVisible(M5_3b_Value, false);
		ituWidgetSetVisible(M5_3b_NA, true);
	}
	else
	{
		if(M5_OTemp > 250)
			M5_OTemp = 250;

		M5_OTemp_C = M5_OTemp-40;
		tUL_Value = round((((double)M5_OTemp_C*9)/5))+32;
		M5_OTemp_F = (signed int)tUL_Value;

		//M5_OTemp_F = ((M5_OTemp_C*9)/5)+32;

		memset(ptString,0,10);
		sprintf(ptString,"%4d",M5_OTemp_C);
		ituTextSetString(M5_3a_Value, ptString);

		memset(ptString,0,10);
		sprintf(ptString,"%4d",M5_OTemp_F);
		ituTextSetString(M5_3b_Value, ptString);

		M5_NA_Flag[2] = 0;

		ituWidgetSetVisible(M5_3a_NA, false);
		ituWidgetSetVisible(M5_3a_Value, true);

		ituWidgetSetVisible(M5_3b_NA, false);
		ituWidgetSetVisible(M5_3b_Value, true);
	}

	//M5_UTemp = 180;
	if(M5_UTemp == 0xFF)				// CAN[SI] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
	{
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_4a_NA, ptString);
		ituTextSetString(M5_4b_NA, ptString);
		
		M5_NA_Flag[3] = 1;

		ituWidgetSetVisible(M5_4a_Value, false);
		ituWidgetSetVisible(M5_4a_NA, true);

		ituWidgetSetVisible(M5_4b_Value, false);
		ituWidgetSetVisible(M5_4b_NA, true);
	}
	else
	{
		if(M5_UTemp > 250)
			M5_UTemp = 250;

		M5_UTemp_C = M5_UTemp-40;
		tUL_Value = round((((double)M5_UTemp_C*9)/5))+32;
		M5_UTemp_F = (signed int)tUL_Value;

		//M5_UTemp_F = ((M5_UTemp_C*9)/5)+32;

		memset(ptString,0,10);
		sprintf(ptString,"%4d",M5_UTemp_C);
		ituTextSetString(M5_4a_Value, ptString);

		memset(ptString,0,10);
		sprintf(ptString,"%4d",M5_UTemp_F);
		ituTextSetString(M5_4b_Value, ptString);

		M5_NA_Flag[3] = 0;

		ituWidgetSetVisible(M5_4a_NA, false);
		ituWidgetSetVisible(M5_4a_Value, true);

		ituWidgetSetVisible(M5_4b_NA, false);
		ituWidgetSetVisible(M5_4b_Value, true);
	}

	//M5_Fuel = 750;	// dev.opt
	if(M5_Fuel == 0xFFFF)					// CAN[DD] 1byte (0~250;0~0xFA) (0xFF:N/A) %(0.4/bit)
	{
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_5_NA, ptString);
		
		M5_NA_Flag[4] = 1;

		ituWidgetSetVisible(M5_5_Value, false);
		ituWidgetSetVisible(M5_5_NA, true);
	}
	else
	{
		if(M5_Fuel > 100)
			M5_Fuel = 100;
		
		memset(ptString,0,10);
		sprintf(ptString,"%3d",M5_Fuel);  // JPHong 0823
		ituTextSetString(M5_5_Value, ptString);

		M5_NA_Flag[4] = 0;

		ituWidgetSetVisible(M5_5_NA, false);
		ituWidgetSetVisible(M5_5_Value, true);
	}

	//M5_Urea = 750;	// dev.opt
	if(M5_Urea == 0xFFFF)					// CAN[AT1T1I] 0byte (0~250;0~0xFA) (0xFF:N/A) %(0.4/bit)
	{
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_6_NA, ptString);

		M5_NA_Flag[5] = 1;

		ituWidgetSetVisible(M5_6_Value, false);
		ituWidgetSetVisible(M5_6_NA, true);
	}
	else
	{
		if(M5_Urea > 100)
			M5_Urea = 100;

		memset(ptString,0,10);
		sprintf(ptString,"%3d",M5_Urea);  // JPHong 0823
		ituTextSetString(M5_6_Value, ptString);

		M5_NA_Flag[5] = 0;

		ituWidgetSetVisible(M5_6_NA, false);
		ituWidgetSetVisible(M5_6_Value, true);
	}

	//M5_Batt = 127;	// dev.opt
	if(M5_Batt == 0xFFFFFFFF)					// CAN[VEP1] 4~5byte (0~3212.75;0~0xFAFF) (0xFFFF:N/A) V(0.05/bit)
	{
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_7_NA, ptString);

		M5_NA_Flag[6] = 1;

		ituWidgetSetVisible(M5_7_Value, false);
		ituWidgetSetVisible(M5_7_NA, true);
	}
	else
	{
		if(M5_Batt > 32128)	// JPHong 0823
			M5_Batt = 32128;
		
		memset(ptString,0,10);
		sprintf(ptString,"%4d.%1d",M5_Batt/10,M5_Batt%10); // JPHong 0823
		ituTextSetString(M5_7_Value, ptString);
		
		M5_NA_Flag[6] = 0;
		
		ituWidgetSetVisible(M5_7_NA, false);
		ituWidgetSetVisible(M5_7_Value, true);
	}

	//M5_PM = 4;	// dev.opt
	if(M5_PM == 0xFF)					// CAN[EI] 2byte (0~250;0~0xFA) (0xFF:N/A) %(1/bit)
	{
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_8_NA, ptString);

		M5_NA_Flag[7] = 1;

		ituWidgetSetVisible(M5_8_Value, false);
		ituWidgetSetVisible(M5_8_NA, true);
	}
	else
	{
		if(M5_PM > 250)
			M5_PM = 250;
		
		memset(ptString,0,10);
		sprintf(ptString,"%4d",M5_PM);
		ituTextSetString(M5_8_Value, ptString);
		
		M5_NA_Flag[7] = 0;
		
		ituWidgetSetVisible(M5_8_NA, false);
		ituWidgetSetVisible(M5_8_Value, true);
	}

	//M5_ASH = 7;	// dev.opt
	if(M5_ASH == 0xFF)					// CAN[AT1S] 1byte (0~250;0~0xFA) (0xFF:N/A) %(1/bit)
	{
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_9_NA, ptString);
		
		M5_NA_Flag[8] = 1;
		
		ituWidgetSetVisible(M5_9_Value, false);
		ituWidgetSetVisible(M5_9_NA, true);
	}
	else
	{
		if(M5_ASH > 250)
			M5_ASH = 250;

		memset(ptString,0,10);
		sprintf(ptString,"%4d",M5_ASH);
		ituTextSetString(M5_9_Value, ptString);

		M5_NA_Flag[8] = 0;

		ituWidgetSetVisible(M5_9_NA, false);
		ituWidgetSetVisible(M5_9_Value, true);
	}

	M5_UserDisplay();

	M5_SecTimer = gMainTimerCnt_10m;
	M5_ExitCNT = 0;

    return true;
}

bool M5_Timer(ITUWidget* widget, char* param)
{
	char ptString[10];

	static uint16_t 	b_RPM=0;
	static uint8_t 		b_WTemp=0;
	static uint8_t 		b_OTemp=0;
	static uint8_t 		b_UTemp=0;
	static uint16_t 	b_Fuel=0;
	static uint16_t 	b_Urea=0;
	static uint32_t 	b_Batt=0;			//
	static uint8_t 		b_PM=0;
	static uint8_t 		b_ASH=0;

	signed int M5_WTemp_C=0,M5_WTemp_F=0;
	signed int M5_OTemp_C=0,M5_OTemp_F=0;
	signed int M5_UTemp_C=0,M5_UTemp_F=0;

	static uint32_t tRPM_SUM = 0;
	static uint8_t tRPM_CNT = 0;
	static uint32_t tRPM_TIME = 0;

#ifdef USED_ROTATION				//jhlee 231020
	if(RotationMode!=0)
	{
		return 0;
	}
#endif
	
	if((gMainTimerCnt_10m - M5_SecTimer) >= 100)
	{
		if(M5_ExitCNT < 30)
			M5_ExitCNT++;

		if(M5_ExitCNT == 30)
		{
			M5_ExitCNT = 100;
			printf("\r\nM5 Menu 30Sec Exit > goto Main\r\n");
			//GotoMain(MAIN_MODE_NORMAL); 
		}

		M5_SecTimer = gMainTimerCnt_10m;

	}
/*
	if(b_RPM != M5_RPM)
	{
		b_RPM = M5_RPM;

		if(M5_RPM == 0xFFFF)				// CAN[EEC1] 3-4byte 0~8031.875 (0~0xFAFF)(0xFExx:Error , 0xFFxx:N/A) rpm (0.125/bit)
		{
			memset(ptString,0,10);
			sprintf(ptString,"N/A   ");
			ituTextSetString(M5_1_Value, ptString);
			ituWidgetSetVisible(M5_1_Value, true);
		}
		else
		{
			memset(ptString,0,10);
			sprintf(ptString,"%4d",M5_RPM);
			ituTextSetString(M5_1_Value, ptString);
			ituWidgetSetVisible(M5_1_Value, true);
		}
	}
*/
#if 0
	if(M5_RPM == 0xFFFF)
	{
		tRPM_SUM = 0;
		tRPM_CNT = 0;
		tRPM_TIME = gMainTimerCnt_10m;
		
		memset(ptString,0,10);
		sprintf(ptString,"N/A   ");
		ituTextSetString(M5_1_Value, ptString);
		ituWidgetSetVisible(M5_1_Value, true);
	}
	else
	{
		if((gMainTimerCnt_10m - tRPM_TIME) < 50)			// 500msec
		{
			tRPM_SUM += M5_RPM;
			tRPM_CNT++;
		}
		else if (tRPM_CNT==0)	//JPHong 08-28
		{
			tRPM_SUM = M5_RPM;
			tRPM_CNT = 1;
			tRPM_TIME = gMainTimerCnt_10m;
		}
		else if((gMainTimerCnt_10m - tRPM_TIME) >= 50)
		{
			M5_RPM = (uint16_t)(tRPM_SUM / tRPM_CNT);
//			printf("RPM SUM = %d,RPM_CNT = %d,RPM = %d\r\n",tRPM_SUM,tRPM_CNT,M5_RPM);

			memset(ptString,0,10);
			sprintf(ptString,"%4d",M5_RPM);
			ituTextSetString(M5_1_Value, ptString);
			ituWidgetSetVisible(M5_1_Value, true);
		
			tRPM_SUM = 0;
			tRPM_CNT = 0;
			tRPM_TIME = gMainTimerCnt_10m;
		}
	}

	if((gMainTimerCnt_10m - M5_RPM_Timer) >= 500)
	{
		M5_RPM = 0xFFFF;
		
		memset(ptString,0,10);
		sprintf(ptString,"N/A   ");
		ituTextSetString(M5_1_Value, ptString);
		ituWidgetSetVisible(M5_1_Value, true);
	}
#else
{       
	uint32_t tData = 0;
	memset(ptString,0,10);
	if ((DTVal.EEC1_34Bytes==0xFFFFFFFF)||((DTVal.EEC1_34Time+TIME_CNT_5SEC)<gMainTimerCnt_10m))
	{
		DTVal.EEC1_34Cnt = 0;
		DTVal.EEC1_34Bytes=0;
		tRPM_TIME = gMainTimerCnt_10m;
		sprintf(ptString, "N/A");
		ituTextSetString(M5_1_NA, ptString);
		
		M5_NA_Flag[0] = 1;
		
		ituWidgetSetVisible(M5_1_Value, false);
		ituWidgetSetVisible(M5_1_NA, true);
	}
	else //if (tTime != DTVal.EEC1_34Time)
	{
		if (DTVal.EEC1_34Cnt==0)
		{
			tRPM_TIME = gMainTimerCnt_10m;
			DTVal.EEC1_34Cnt = 0;
			DTVal.EEC1_34Bytes=0;
		}
		else if ((tRPM_TIME+TIME_CNT_500MS)<gMainTimerCnt_10m)
		{
			tData = DTVal.EEC1_34Bytes/DTVal.EEC1_34Cnt;
//			printf("M5_Timer=%d %d %d %d\n",DTVal.EEC1_34Bytes,DTVal.EEC1_34Cnt,tData);
			DTVal.EEC1_34Bytes = 0;
			DTVal.EEC1_34Cnt = 0;	// JPHong0830	
			sprintf(ptString, "%d", tData);

			tRPM_TIME = gMainTimerCnt_10m;
			ituTextSetString(M5_1_Value, ptString);

			M5_NA_Flag[0] = 0;
			
			ituWidgetSetVisible(M5_1_NA, false);
			ituWidgetSetVisible(M5_1_Value, true);
		}
	}
}
#endif
	if(b_WTemp != M5_WTemp)
	{
		b_WTemp = M5_WTemp;
		
		M5_WTemp_C = M5_WTemp-40;
		tUL_Value = round((((double)M5_WTemp_C*9)/5))+32;
		M5_WTemp_F = (signed int)tUL_Value;

		//M5_WTemp_F = ((M5_WTemp_C*9)/5)+32;

		if(M5_WTemp == 0xFF)				// CAN[ET1] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
		{
			memset(ptString,0,10);
			sprintf(ptString,"N/A");
			ituTextSetString(M5_2a_NA, ptString);
			ituTextSetString(M5_2b_NA, ptString);

			M5_NA_Flag[1] = 1;

			ituWidgetSetVisible(M5_2a_Value, false);
			ituWidgetSetVisible(M5_2a_NA, true);
    
			ituWidgetSetVisible(M5_2b_Value, false);
			ituWidgetSetVisible(M5_2b_NA, true);
		}
		else
		{
			memset(ptString,0,10);
			sprintf(ptString,"%4d",M5_WTemp_C);
			ituTextSetString(M5_2a_Value, ptString);

			memset(ptString,0,10);
			sprintf(ptString,"%4d",M5_WTemp_F);
			ituTextSetString(M5_2b_Value, ptString);

			M5_NA_Flag[1] = 0;

			ituWidgetSetVisible(M5_2a_NA, false);
			ituWidgetSetVisible(M5_2a_Value, true);
    
			ituWidgetSetVisible(M5_2b_NA, false);
			ituWidgetSetVisible(M5_2b_Value, true);
		}
	}

	if((gMainTimerCnt_10m - M5_WTemp_Timer) >= 500)
	{
		M5_WTemp = 0xFF;
		
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_2a_NA, ptString);
		ituTextSetString(M5_2b_NA, ptString);
		
		M5_NA_Flag[1] = 1;

		ituWidgetSetVisible(M5_2a_Value, false);
		ituWidgetSetVisible(M5_2a_NA, true);
    
		ituWidgetSetVisible(M5_2b_Value, false);
		ituWidgetSetVisible(M5_2b_NA, true);
	}

	if(b_OTemp != M5_OTemp)
	{
		b_OTemp = M5_OTemp;
		
		M5_OTemp_C = M5_OTemp-40;
		tUL_Value = round((((double)M5_OTemp_C*9)/5))+32;
		M5_OTemp_F = (signed int)tUL_Value;

		//M5_OTemp_F = ((M5_OTemp_C*9)/5)+32;

		if(M5_OTemp == 0xFF)				// CAN[SI] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
		{
			memset(ptString,0,10);
			sprintf(ptString,"N/A");
			ituTextSetString(M5_3a_NA, ptString);
			ituTextSetString(M5_3b_NA, ptString);

			M5_NA_Flag[2] = 1;

			ituWidgetSetVisible(M5_3a_Value, false);
			ituWidgetSetVisible(M5_3a_NA, true);
    
			ituWidgetSetVisible(M5_3b_Value, false);
			ituWidgetSetVisible(M5_3b_NA, true);
		}
		else
		{
			memset(ptString,0,10);
			sprintf(ptString,"%4d",M5_OTemp_C);
			ituTextSetString(M5_3a_Value, ptString);

			memset(ptString,0,10);
			sprintf(ptString,"%4d",M5_OTemp_F);
			ituTextSetString(M5_3b_Value, ptString);

			M5_NA_Flag[2] = 0;

			ituWidgetSetVisible(M5_3a_NA, false);
			ituWidgetSetVisible(M5_3a_Value, true);
    
			ituWidgetSetVisible(M5_3b_NA, false);
			ituWidgetSetVisible(M5_3b_Value, true);
		}
	}

	if((gMainTimerCnt_10m - M5_OTemp_Timer) >= 500)
	{
		M5_OTemp = 0xFF;
		
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_3a_NA, ptString);
		ituTextSetString(M5_3b_NA, ptString);

		M5_NA_Flag[2] = 1;

		ituWidgetSetVisible(M5_3a_Value, false);
		ituWidgetSetVisible(M5_3a_NA, true);
    
		ituWidgetSetVisible(M5_3b_Value, false);
		ituWidgetSetVisible(M5_3b_NA, true);
	}

	if(b_UTemp != M5_UTemp)
	{
		b_UTemp = M5_UTemp;
		
		M5_UTemp_C = M5_UTemp-40;
		tUL_Value = round((((double)M5_UTemp_C*9)/5))+32;
		M5_UTemp_F = (signed int)tUL_Value;

		//M5_UTemp_F = ((M5_UTemp_C*9)/5)+32;

		if(M5_UTemp == 0xFF)				// CAN[SI] 0byte (0~250;0~0xFA)(0xFE:Error , 0xFF:N/A) c(1/bit,offset;-40c)
		{
			memset(ptString,0,10);
			sprintf(ptString,"N/A");
			ituTextSetString(M5_4a_NA, ptString);
			ituTextSetString(M5_4b_NA, ptString);

			M5_NA_Flag[3] = 1;

			ituWidgetSetVisible(M5_4a_Value, false);
			ituWidgetSetVisible(M5_4a_NA, true);
    
			ituWidgetSetVisible(M5_4b_Value, false);
			ituWidgetSetVisible(M5_4b_NA, true);
		}
		else
		{
			memset(ptString,0,10);
			sprintf(ptString,"%4d",M5_UTemp_C);
			ituTextSetString(M5_4a_Value, ptString);

			memset(ptString,0,10);
			sprintf(ptString,"%4d",M5_UTemp_F);
			ituTextSetString(M5_4b_Value, ptString);

			M5_NA_Flag[3] = 0;

			ituWidgetSetVisible(M5_4a_NA, false);
			ituWidgetSetVisible(M5_4a_Value, true);
    
			ituWidgetSetVisible(M5_4b_NA, false);
			ituWidgetSetVisible(M5_4b_Value, true);
		}
	}

	if((gMainTimerCnt_10m - M5_UTemp_Timer) >= 500)
	{
		M5_UTemp = 0xFF;
		
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_4a_NA, ptString);
		ituTextSetString(M5_4b_NA, ptString);

		M5_NA_Flag[3] = 1;

		ituWidgetSetVisible(M5_4a_Value, false);
		ituWidgetSetVisible(M5_4a_NA, true);
    
		ituWidgetSetVisible(M5_4b_Value, false);
		ituWidgetSetVisible(M5_4b_NA, true);
	}

	if(b_Fuel != M5_Fuel)
	{
		b_Fuel = M5_Fuel;
		
		if(M5_Fuel == 0xFFFF)					// CAN[DD] 1byte (0~250;0~0xFA) (0xFF:N/A) %(0.4/bit)
		{
			memset(ptString,0,10);
			sprintf(ptString,"N/A");
			ituTextSetString(M5_5_NA, ptString);

			M5_NA_Flag[4] = 1;

			ituWidgetSetVisible(M5_5_Value, false);
			ituWidgetSetVisible(M5_5_NA, true);
		}
		else
		{
			memset(ptString,0,10);
			sprintf(ptString,"%3d",M5_Fuel);  // JPHong 0829
			ituTextSetString(M5_5_Value, ptString);

			M5_NA_Flag[4] = 0;

			ituWidgetSetVisible(M5_5_NA, false);
			ituWidgetSetVisible(M5_5_Value, true);
		}
	}

	if((gMainTimerCnt_10m - M5_Fuel_Timer) >= 500)
	{
		M5_Fuel = 0xFFFF;
		
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_5_NA, ptString);

		M5_NA_Flag[4] = 1;

		ituWidgetSetVisible(M5_5_Value, false);
		ituWidgetSetVisible(M5_5_NA, true);
	}

	if(b_Urea != M5_Urea)
	{
		b_Urea = M5_Urea;
		
		if(M5_Urea == 0xFFFF)					// CAN[AT1T1I] 0byte (0~250;0~0xFA) (0xFF:N/A) %(0.4/bit)
		{
			memset(ptString,0,10);
			sprintf(ptString,"N/A");
			ituTextSetString(M5_6_NA, ptString);

			M5_NA_Flag[5] = 1;

			ituWidgetSetVisible(M5_6_Value, false);
			ituWidgetSetVisible(M5_6_NA, true);
		}
		else
		{
			memset(ptString,0,10);
			sprintf(ptString,"%3d",M5_Urea);  // JPHong 0823
			ituTextSetString(M5_6_Value, ptString);
			
			M5_NA_Flag[5] = 0;

			ituWidgetSetVisible(M5_6_NA, false);
			ituWidgetSetVisible(M5_6_Value, true);
		}
	}

	if((gMainTimerCnt_10m - M5_Urea_Timer) >= 500)
	{
		M5_Urea = 0xFFFF;	// JPHong 0823
		
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_6_NA, ptString);

		M5_NA_Flag[5] = 1;

		ituWidgetSetVisible(M5_6_Value, false);
		ituWidgetSetVisible(M5_6_NA, true);
	}

	if(b_Batt != M5_Batt)
	{
		b_Batt = M5_Batt;

		if(M5_Batt == 0xFFFFFFFF)					// CAN[VEP1] 4~5byte (0~3212.75;0~0xFAFF) (0xFFFF:N/A) V(0.05/bit)
		{
			memset(ptString,0,10);
			sprintf(ptString,"N/A");
			ituTextSetString(M5_7_NA, ptString);

			M5_NA_Flag[6] = 1;

			ituWidgetSetVisible(M5_7_Value, false);
			ituWidgetSetVisible(M5_7_NA, true);
		}
		else
		{
			memset(ptString,0,10);
			sprintf(ptString,"%4d.%1d",M5_Batt/10,M5_Batt%10);  // JPHong 0823
			ituTextSetString(M5_7_Value, ptString);

			M5_NA_Flag[6] = 0;

			ituWidgetSetVisible(M5_7_NA, false);
			ituWidgetSetVisible(M5_7_Value, true);
		}
	}

	if((gMainTimerCnt_10m - M5_Batt_Timer) >= 500)
	{
		M5_Batt = 0xFFFFFFFF;
		
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_7_NA, ptString);

		M5_NA_Flag[6] = 1;

		ituWidgetSetVisible(M5_7_Value, false);
		ituWidgetSetVisible(M5_7_NA, true);
	}

	if(b_PM != M5_PM)
	{
		b_PM = M5_PM;

		if(M5_PM == 0xFF)					// CAN[EI] 2byte (0~250;0~0xFA) (0xFF:N/A) %(1/bit)
		{
			memset(ptString,0,10);
			sprintf(ptString,"N/A");
			ituTextSetString(M5_8_NA, ptString);

			M5_NA_Flag[7] = 1;
			
			ituWidgetSetVisible(M5_8_Value, false);
			ituWidgetSetVisible(M5_8_NA, true);
		}
		else
		{
			memset(ptString,0,10);
			sprintf(ptString,"%4d",M5_PM);
			ituTextSetString(M5_8_Value, ptString);

			M5_NA_Flag[7] = 0;

			ituWidgetSetVisible(M5_8_NA, false);
			ituWidgetSetVisible(M5_8_Value, true);
		}
	}

	if((gMainTimerCnt_10m - M5_PM_Timer) >= 12000)
	{
		M5_PM = 0xFF;
		
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_8_NA, ptString);

		M5_NA_Flag[7] = 1;
			
		ituWidgetSetVisible(M5_8_Value, false);
		ituWidgetSetVisible(M5_8_NA, true);
	}

	if(b_ASH != M5_ASH)
	{
		b_ASH = M5_ASH;
		
		if(M5_ASH == 0xFF)					// CAN[AT1S] 1byte (0~250;0~0xFA) (0xFF:N/A) %(1/bit)
		{
			memset(ptString,0,10);
			sprintf(ptString,"N/A");
			ituTextSetString(M5_9_NA, ptString);
			
			M5_NA_Flag[8] = 1;

			ituWidgetSetVisible(M5_9_Value, false);
			ituWidgetSetVisible(M5_9_NA, true);
		}
		else
		{
			memset(ptString,0,10);
			sprintf(ptString,"%4d",M5_ASH);
			ituTextSetString(M5_9_Value, ptString);

			M5_NA_Flag[8] = 0;

			ituWidgetSetVisible(M5_9_NA, false);
			ituWidgetSetVisible(M5_9_Value, true);
		}
	}

	if((gMainTimerCnt_10m - M5_ASH_Timer) >= 12000)		// //2min
	{
		M5_ASH = 0xFF;
		
		memset(ptString,0,10);
		sprintf(ptString,"N/A");
		ituTextSetString(M5_9_NA, ptString);

		M5_NA_Flag[8] = 1;

		ituWidgetSetVisible(M5_9_Value, false);
		ituWidgetSetVisible(M5_9_NA, true);
	}

	M5_UserDisplay();
	
    return true;
}

bool M5_JogWheel(ITUWidget* widget, char* param)
{
	M5_ExitCNT = 0;
	
    return true;
}

bool M5_JogButton(ITUWidget* widget, char* param)
{
	M5_ExitCNT = 0;

    return true;
}

bool M5_JogSwitch(ITUWidget* widget, char* param)
{
	uint8_t tJogSwitch = 0;

	M5_ExitCNT = 0;
	tJogSwitch = param[0];
	printf("M5_JogButton JogSwitch %d\r\n",tJogSwitch);
	if(tJogSwitch == JOG_DIAL_SW_ESC_ON)
	{
		//Menu_Index = Menu_Top;
		//GotoMenuTop();	//JPHong 2024-03-22
	}
	else if(tJogSwitch == JOG_DIAL_SW_HOME)
	{
		GotoMain(MAIN_MODE_NORMAL); 
	}
	else if(tJogSwitch == JOG_DIAL_SW_ESC_ON_OFF)
	{
		//Menu_Index = Menu_Top;
		GotoMenuTop();	//JPHong 2024-03-22
	}
    return true;
}

bool M5D7_MouseUp(ITUWidget* widget, char* param)
{
    return true;
}

bool M5D6_MouseUp(ITUWidget* widget, char* param)
{
	
	GotoMain(MAIN_MODE_NORMAL); 
	
    return true;
}

bool M5D5_MouseUp(ITUWidget* widget, char* param)
{
	
	GotoMenuTop();
	
    return true;
}

bool M5_UserDisplay(void)
{
	uint8_t tDestName[100];

	if(gMenu_UI_Time == 0)	// JPHong 2024-03-20
		return 0;

	ITUWidget *M5_TopAdmin = ituSceneFindWidget(&theScene, "M5_TopAdmin");
	assert(M5_TopAdmin);
	ITUWidget *M5_TopStandard = ituSceneFindWidget(&theScene, "M5_TopStandard");
	assert(M5_TopStandard);
	
	ITUText *M5_TopUserName = ituSceneFindWidget(&theScene, "M5_TopUserName");
	assert(M5_TopUserName);
	
	if(User_RegInfo[M7_UserIndex].RegFlag == 5)		// admin
	{
		ituWidgetSetVisible(M5_TopStandard,false);
		ituWidgetSetVisible(M5_TopAdmin,true);
	}
	else if((User_RegInfo[M7_UserIndex].RegFlag == 2)||(User_RegInfo[M7_UserIndex].RegFlag == 3)||(User_RegInfo[M7_UserIndex].RegFlag == 9))
	{
		ituWidgetSetVisible(M5_TopAdmin,false);
		ituWidgetSetVisible(M5_TopStandard,true);
	}
	else
	{
		ituWidgetSetVisible(M5_TopAdmin,false);
		ituWidgetSetVisible(M5_TopStandard,false);
	}

	if((gMenu_UI_Time > 0)&&(0<M7_UserIndexEx)&&(M7_UserIndexEx <= MAX_SU100_USER_NUM))	// JPHong 2024-03-21 Menu Page7
	{
		memset(tDestName,0,100);
	
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M5_TopUserName, tDestName);

		ituWidgetSetVisible(M5_TopUserName,true);
	}
	else
	{
		ituWidgetSetVisible(M5_TopUserName,false);
	}
/*
	if((gMainTimerCnt_10m - gMenu_UI_Time) < 500)		// 5sec	
	{
		memset(tDestName,0,100);
		
		SU100_ISO8859_to_UTF8(tDestName,User_RegInfo[M7_UserIndex].UserName);
		ituTextSetString(M5_TopUserName, tDestName);

		ituWidgetSetVisible(M5_TopUserName,true);
	}
	else if(((gMainTimerCnt_10m - gMenu_UI_Time) >= 500)||(M7_UserIndex > (MAX_SU100_USER_NUM-1)))
	{
		ituWidgetSetVisible(M5_TopUserName,false);
	}
*/
}
