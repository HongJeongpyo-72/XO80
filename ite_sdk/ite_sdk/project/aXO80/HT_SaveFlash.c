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


//void HT_Save_Config(bool SaveFlag);
void HT_gettimeofday(uint16_t *year,uint8_t *mon,uint8_t *mday,uint8_t *hour,uint8_t *min,uint8_t *sec);		// add 230516 jhlee
void HT_settimeofday(uint16_t year,uint8_t mon,uint8_t mday,uint8_t hour,uint8_t min,uint8_t sec);		// add 230516 jhlee

// #define DGB_MSG_SF

#define CheckValidData(tSrc,tData,MinD,MaxD)	{										\
													if ((tSrc<MinD)||(tSrc>=(MaxD)))	\
													{									\
														tSrc = tData;					\
													}									\
												}

	

int sf_BlockSize=0;
int sf_GapSize=0;
int sf_AlignSize=0;
uint8_t *ConfigBuff;


uint32_t sf_CNT=0;
uint32_t XSC_FlashAddr;

// #define TEST_FLASH_DATA_FF
#define MODEL_INDEXs		0x42	// 570:0x41 , T800:0x42 , C10-5:0x43
#define MODEL_INDEXe		0x62	// 570:0x61 , T800:0x62 , C10-5:0x63


bool SaveConfigData (XoSaveConfig* XSC,bool FlashInit)
{
	int i,sAddr;
	int StartUser,EndUser;
	char TempData[10] = {0,};
#ifdef DGB_MSG_SF
	printf("SaveConfigData ==> DataBuff:0x%08X ,FlashAddr:0x%08X ,UserNum:%d  ,SaveFlag:%d   ,ForceSave:%d \n",XSC->DataBuff,XSC->FlashAddr,XSC->UserNum,XSC->SaveFlag,XSC->ForceSave);
#endif	
#ifndef TEST_CAM_COLOR	
	if ((UserPW_PassFlag!=1)&&(FlashInit==0)&&(XSC->ForceSave==0))
	{
		printf("SaveConfigData ==> Can't save:[%d][%d][%d]\n",UserPW_PassFlag,FlashInit,XSC->ForceSave);
		return 0;
	}
#endif
#ifdef TEST_FLASH_DATA_FF
	memset(&XSC->DataBuff[0], 0xFF, sf_BlockSize);
	Flash_Write(XSC->DataBuff,XSC->FlashAddr);
	return 1;
#endif
	sf_CNT++;

	printf("SaveConfigData Start:%d ,BG_Color:%d ,  XSC->UserNum:%d , sf_CNT:%d\n",DTM_DateTime_Mode,BG_Color, XSC->UserNum,sf_CNT);
	XSC->DataBuff[sf_Addr_sSTART+0] = 0x73;	//	s
	XSC->DataBuff[sf_Addr_sSTART+1] = 0x74;	//	t
	XSC->DataBuff[sf_Addr_sSTART+2] = 0x58;	//	X
	XSC->DataBuff[sf_Addr_sSTART+3] = MODEL_INDEXs;	//	O
	
	XSC->DataBuff[sf_Addr_Count+0] = (sf_CNT>>24)&0xFF;
	XSC->DataBuff[sf_Addr_Count+1] = (sf_CNT>>16)&0xFF;
	XSC->DataBuff[sf_Addr_Count+2] = (sf_CNT>>8)&0xFF;
	XSC->DataBuff[sf_Addr_Count+3] = (sf_CNT>>0)&0xFF;
	
	XSC->DataBuff[sf_Addr_M7_UserIndex] = XSC->UserNum;
	
	XSC->DataBuff[sf_Addr_sEND+0] = 0x65;	//	e
	XSC->DataBuff[sf_Addr_sEND+1] = 0x64;	//	d
	XSC->DataBuff[sf_Addr_sEND+2] = 0x58;	//	X
	XSC->DataBuff[sf_Addr_sEND+3] = MODEL_INDEXe;	//	O
	
	
	
	sAddr = 0;
#ifdef TEST_FLASH_DATA_FF
	Flash_Write(XSC->DataBuff,XSC->FlashAddr);
	return 1;
#endif
	XSC->DataBuff[sAddr+sf_Addr_AUX1CurrentOutMode] = AUX1CurrentOutMode;
	XSC->DataBuff[sAddr+sf_Addr_DTM_DateTime_Mode] = DTM_DateTime_Mode;
	XSC->DataBuff[sAddr+sf_Addr_BG_Color] = BG_Color;
	XSC->DataBuff[sAddr+sf_Addr_LCD_BL_Day] = LCD_BL_Day;
	XSC->DataBuff[sAddr+sf_Addr_LCD_BL_Night] = LCD_BL_Night;
	XSC->DataBuff[sAddr+sf_Addr_SIUS_SEL_Flag] = SIUS_SEL_Flag;
	XSC->DataBuff[sAddr+sf_Addr_HMV_HourMeter_Trip] = HMV.HourMeter_Trip;
	XSC->DataBuff[sAddr+sf_Addr_Option_Function_Rotate_Flag] = Option_Function.Rotate_Flag;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserSet0_A_Value] = M7_UserSet[0].A_Value;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserSet1_A_Value] = M7_UserSet[1].A_Value;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserSet2_A_Value] = M7_UserSet[2].A_Value;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserSet0_B_Value] = M7_UserSet[0].B_Value;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserSet1_B_Value] = M7_UserSet[1].B_Value;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserSet2_B_Value] = M7_UserSet[2].B_Value;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserSet0_P1_Value] = M7_UserSet[0].P1_Value;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserSet1_P1_Value] = M7_UserSet[1].P1_Value;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserSet2_P1_Value] = M7_UserSet[2].P1_Value;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserSet0_ATT_State] = M7_UserSet[0].ATT_State;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserSet1_ATT_State] = M7_UserSet[1].ATT_State;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserSet2_ATT_State] = M7_UserSet[2].ATT_State;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserSet0_Way_Opt] = M7_UserSet[0].Way_Opt;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserSet1_Way_Opt] = M7_UserSet[1].Way_Opt;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserSet2_Way_Opt] = M7_UserSet[2].Way_Opt;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserAUX24Set_C_Value] = M7_UserAUX24Set.C_Value;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserAUX24Set_D_Value] = M7_UserAUX24Set.D_Value;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserAUX24Set_P2_Value] = M7_UserAUX24Set.P2_Value;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserAUX24Set_G_Value] = M7_UserAUX24Set.G_Value;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserAUX24Set_H_Value] = M7_UserAUX24Set.H_Value;
	XSC->DataBuff[sAddr+sf_Addr_M7_UserAUX24Set_P4_Value] = M7_UserAUX24Set.P4_Value;
	XSC->DataBuff[sAddr+sf_Addr_Option_Function_AutoStopKey] = Option_Function.AutoStopKey;
	
	XSC->DataBuff[sAddr+sf_Addr_CC1_Brightness] = CC1.Brightness;
	XSC->DataBuff[sAddr+sf_Addr_CC1_Contrast] = CC1.Contrast;
	XSC->DataBuff[sAddr+sf_Addr_CC1_Saturation] = CC1.Saturation;
	XSC->DataBuff[sAddr+sf_Addr_CC1_Hue] = CC1.Hue;
	XSC->DataBuff[sAddr+sf_Addr_CC1_Sharpness] = CC1.Sharpness;
	XSC->DataBuff[sAddr+sf_Addr_CC2_Brightness] = CC2.Brightness;
	XSC->DataBuff[sAddr+sf_Addr_CC2_Contrast] = CC2.Contrast;
	XSC->DataBuff[sAddr+sf_Addr_CC2_Saturation] = CC2.Saturation;
	XSC->DataBuff[sAddr+sf_Addr_CC2_Hue] = CC2.Hue;
	XSC->DataBuff[sAddr+sf_Addr_CC2_Sharpness] = CC2.Sharpness;
	
	
	for (i=0;i<MAX_SU100_USER_NUM;i++)//chpark.12.12
	{
	
		XSC->DataBuff[sAddr+sf_Addr_Lang_Flag+i] = Lang_Flag[i];
		User_RegInfo[i].Language = Lang_Flag[i];	
		XSC->DataBuff[sAddr+sf_Addr_Option_Function_AutoStop+i] = Option_Function.AutoStop[i];
		
		XSC->DataBuff[sAddr+sf_Addr_User_RegInfo_RegKeyNum+i] = User_RegInfo[i].RegKeyNum;
		XSC->DataBuff[sAddr+sf_Addr_User_RegInfo_RegFlag+i] = User_RegInfo[i].RegFlag;//chpark.12.12
		XSC->DataBuff[sAddr+sf_Addr_User_RegInfo_AccessAUX+i] = User_RegInfo[i].AccessAUX;//chpark.12.12
		memcpy (&XSC->DataBuff[sAddr+sf_Addr_User_RegInfo_UserPassCode+i*sf_Size_User_RegInfo_UserPassCode],User_RegInfo[i].UserPassCode,MAX_SU100_USER_PW);
		memcpy (&XSC->DataBuff[sAddr+sf_Addr_User_RegInfo_UserName+i*sf_Size_User_RegInfo_UserName],User_RegInfo[i].UserName,MAX_SU100_USER_NAME);
	}
	
		
	memcpy (&XSC->DataBuff[sAddr+sf_Addr_Device_UID],Device_UID,MAX_DEVICE_UID);//chpark.24.05.31
	memcpy (&XSC->DataBuff[sAddr+sf_Addr_PW100_PWD],PW100_PWD,MAX_PW100_PWD);   //chpark.24.09.09
	XSC->DataBuff[sAddr+sf_Addr_PW100_PW_OnOff] = g_bPW100_PW_OnOff;	//chpark.24.09.09
	XSC->DataBuff[sAddr+sf_Addr_M7_Lock_Data_Value0_Lock] = g_nM7_Lock_Data[0].Lock;	//chpark.24.09.19
	XSC->DataBuff[sAddr+sf_Addr_M7_Lock_Data_Value1_Lock] = g_nM7_Lock_Data[1].Lock;	//chpark.24.09.19
	XSC->DataBuff[sAddr+sf_Addr_M7_Lock_Data_Value2_Lock] = g_nM7_Lock_Data[2].Lock;	//chpark.24.09.19
	XSC->DataBuff[sAddr+sf_Addr_M7_Lock_Data_Value0_Show] = g_nM7_Lock_Data[0].Show;	//chpark.24.09.19
	XSC->DataBuff[sAddr+sf_Addr_M7_Lock_Data_Value1_Show] = g_nM7_Lock_Data[1].Show;	//chpark.24.09.19
	XSC->DataBuff[sAddr+sf_Addr_M7_Lock_Data_Value2_Show] = g_nM7_Lock_Data[2].Show;	//chpark.24.09.19
	
	if (XSC->SaveFlag)
	{
		Flash_Write(XSC->DataBuff,XSC->FlashAddr);
	}
#if 0
	printf("\n");
	for (i=0;i<sf_FLASH_SECTOR_SIZE;i+=8)//chpark.12.12
	{
		printf("[0x%04X,%04d] : [0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X]\n",i,i,XSC->DataBuff[i+0],XSC->DataBuff[i+1],XSC->DataBuff[i+2],XSC->DataBuff[i+3],XSC->DataBuff[i+4],XSC->DataBuff[i+5],XSC->DataBuff[i+6],XSC->DataBuff[i+7]);
	}
	printf("\n");
#endif
#if 0
	printf("\n\n");
	
	memset (&TempData[0],0,sizeof(TempData));
	memcpy (&TempData[0],&XSC->DataBuff[sf_Addr_sSTART],sf_Size_sSTART);

	printf(" Save sSTART : [%4s]\n",&TempData[0]);
	printf("Count : [%d]\n",(XSC->DataBuff[sf_Addr_Count+0]<<24)|(XSC->DataBuff[sf_Addr_Count+1]<<16)|(XSC->DataBuff[sf_Addr_Count+2]<<8)|(XSC->DataBuff[sf_Addr_Count+3]<<0));
	printf("M7_UserIndex : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserIndex]);
	printf("AUX1CurrentOutMode : [%d]\n",XSC->DataBuff[sf_Addr_AUX1CurrentOutMode]);
	printf("DTM_DateTime_Mode : [%d]\n",XSC->DataBuff[sf_Addr_DTM_DateTime_Mode]);
	printf("BG_Color : [%d]\n",XSC->DataBuff[sf_Addr_BG_Color]);
	printf("LCD_BL_Day : [%d]\n",XSC->DataBuff[sf_Addr_LCD_BL_Day]);
	printf("LCD_BL_Night : [%d]\n",XSC->DataBuff[sf_Addr_LCD_BL_Night]);
	printf("SIUS_SEL_Flag : [%d]\n",XSC->DataBuff[sf_Addr_SIUS_SEL_Flag]);
	printf("HMV_HourMeter_Trip : [%d]\n",XSC->DataBuff[sf_Addr_HMV_HourMeter_Trip]);
	printf("Option_Function_Rotate_Flag : [%d]\n",XSC->DataBuff[sf_Addr_Option_Function_Rotate_Flag]);
	printf("M7_UserSet0_A_Value : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserSet0_A_Value]);
	printf("M7_UserSet1_A_Value : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserSet1_A_Value]);
	printf("M7_UserSet2_A_Value : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserSet2_A_Value]);
	printf("M7_UserSet0_B_Value : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserSet0_B_Value]);
	printf("M7_UserSet1_B_Value : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserSet1_B_Value]);
	printf("M7_UserSet2_B_Value : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserSet2_B_Value]);
	printf("M7_UserSet0_P1_Value : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserSet0_P1_Value]);
	printf("M7_UserSet1_P1_Value : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserSet1_P1_Value]);
	printf("M7_UserSet2_P1_Value : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserSet2_P1_Value]);
	printf("M7_UserSet0_ATT_State : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserSet0_ATT_State]);
	printf("M7_UserSet1_ATT_State : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserSet1_ATT_State]);
	printf("M7_UserSet2_ATT_State : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserSet2_ATT_State]);
	printf("M7_UserSet0_Way_Opt : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserSet0_Way_Opt]);
	printf("M7_UserSet1_Way_Opt : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserSet1_Way_Opt]);
	printf("M7_UserSet2_Way_Opt : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserSet2_Way_Opt]);
	printf("M7_UserAUX24Set_C_Value : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserAUX24Set_C_Value]);
	printf("M7_UserAUX24Set_D_Value : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserAUX24Set_D_Value]);
	printf("M7_UserAUX24Set_P2_Value : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserAUX24Set_P2_Value]);
	printf("M7_UserAUX24Set_G_Value : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserAUX24Set_G_Value]);
	printf("M7_UserAUX24Set_H_Value : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserAUX24Set_H_Value]);
	printf("M7_UserAUX24Set_P4_Value : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserAUX24Set_P4_Value]);
	printf("Option_Function_AutoStopKey : [%d]\n",XSC->DataBuff[sf_Addr_Option_Function_AutoStopKey]);
	
	printf("CC1_Brightness : [%d]\n",XSC->DataBuff[sf_Addr_CC1_Brightness]);
	printf("CC1_Contrast : [%d]\n",XSC->DataBuff[sf_Addr_CC1_Contrast]);
	printf("CC1_Saturation : [%d]\n",XSC->DataBuff[sf_Addr_CC1_Saturation]);
	printf("CC1_Hue	: [%d]\n",XSC->DataBuff[sf_Addr_CC1_Hue]);
	printf("CC1_Sharpness : [%d]\n",XSC->DataBuff[sf_Addr_CC1_Sharpness]);
	printf("CC2_Brightness : [%d]\n",XSC->DataBuff[sf_Addr_CC2_Brightness]);
	printf("CC2_Contrast : [%d]\n",XSC->DataBuff[sf_Addr_CC2_Contrast]);
	printf("CC2_Saturation : [%d]\n",XSC->DataBuff[sf_Addr_CC2_Saturation]);
	printf("CC2_Hue : [%d]\n",XSC->DataBuff[sf_Addr_CC2_Hue]);
	printf("CC2_Sharpness : [%d]\n",XSC->DataBuff[sf_Addr_CC2_Sharpness]);
	printf("\n");
	printf("Device_UID : [%20s]\n",&XSC->DataBuff[sf_Addr_Device_UID]);
	printf("PW100_PWD : [%10s]\n",&XSC->DataBuff[sf_Addr_PW100_PWD]);
	printf("PW100_PW_OnOff : [%d]\n",XSC->DataBuff[sf_Addr_PW100_PW_OnOff]);
	printf("M7_Lock_Data_Value0_Lock : [%d]\n",XSC->DataBuff[sf_Addr_M7_Lock_Data_Value0_Lock]);
	printf("M7_Lock_Data_Value1_Lock : [%d]\n",XSC->DataBuff[sf_Addr_M7_Lock_Data_Value1_Lock]);
	printf("M7_Lock_Data_Value2_Lock : [%d]\n",XSC->DataBuff[sf_Addr_M7_Lock_Data_Value2_Lock]);
	printf("M7_Lock_Data_Value0_Show : [%d]\n",XSC->DataBuff[sf_Addr_M7_Lock_Data_Value0_Show]);
	printf("M7_Lock_Data_Value1_Show : [%d]\n",XSC->DataBuff[sf_Addr_M7_Lock_Data_Value1_Show]);
	printf("M7_Lock_Data_Value2_Show : [%d]\n",XSC->DataBuff[sf_Addr_M7_Lock_Data_Value2_Show]);
	printf("\n");
	for (i=0;i<MAX_SU100_USER_NUM;i++)//chpark.12.12
	{
		printf("Lang_Flag[%02d] : [%d]\n",i,XSC->DataBuff[sf_Addr_Lang_Flag+i]);
		printf("Option_Function_AutoStop[%02d] : [%d]\n",i,XSC->DataBuff[sf_Addr_Option_Function_AutoStop+i]);
		printf("User_RegInfo_RegKeyNum[%02d] : [%d]\n",i,XSC->DataBuff[sf_Addr_User_RegInfo_RegKeyNum+i]);
		printf("User_RegInfo_RegFlag[%02d] : [%d]\n",i,XSC->DataBuff[sf_Addr_User_RegInfo_RegFlag+i]);
		printf("User_RegInfo_AccessAUX[%02d] : [%d]\n",i,XSC->DataBuff[sf_Addr_User_RegInfo_AccessAUX+i]);
		printf("User_RegInfo_UserPassCode[%02d] : [%20s]\n",i,&XSC->DataBuff[sf_Addr_User_RegInfo_UserPassCode+i*sf_Size_User_RegInfo_UserPassCode]);
		printf("User_RegInfo_UserName[%02d] : [%20s]\n",i,&XSC->DataBuff[sf_Addr_User_RegInfo_UserName+i*sf_Size_User_RegInfo_UserName]);
	}	
	memset (&TempData[0],0,sizeof(TempData));
	memcpy (&TempData[0],&XSC->DataBuff[sf_Addr_sEND],sf_Size_sSTART);
	
	printf("sEND : [%4s]\n",&TempData[0]);
	printf("\n\n");
	
#endif
	return 1;
}


bool LoadConfigData (XoSaveConfig* XSC,bool FlashInit)
{
	int i,j,sAddr;
	char TempData[10] = {0,};
//	printf("LoadConfigData ==> DataBuff:0x%08X ,FlashAddr:0x%08X ,UserNum:%d  ,SaveFlag:%d \n",XSC->DataBuff,XSC->FlashAddr,XSC->UserNum,XSC->SaveFlag);

	sf_CNT= (XSC->DataBuff[sf_Addr_Count+0]<<24) | (XSC->DataBuff[sf_Addr_Count+1]<<16) | (XSC->DataBuff[sf_Addr_Count+2]<<8) | (XSC->DataBuff[sf_Addr_Count+3]<<0);
	
//	M7_UserIndex = XSC->DataBuff[sf_Addr_M7_UserIndex];
	CheckValidData(M7_UserIndex,0,0,MAX_SU100_USER_NUM)
	
	XSC->UserNum = M7_UserIndex;

	sAddr = 0;

	printf("LoadConfigData ==> DataBuff:0x%08X ,FlashAddr:0x%08X ,SaveFlag:%d  \n",XSC->DataBuff,XSC->FlashAddr,XSC->SaveFlag);
	printf("LoadConfigData ==> sf_CNT::%d:0x%08X ,sAddr:0x%08X \n",sf_CNT,sf_CNT,sAddr);
		
	sAddr = 0;



	
	
			
	sAddr = 0;
	AUX1CurrentOutMode = XSC->DataBuff[sAddr+sf_Addr_AUX1CurrentOutMode];
	CheckValidData(AUX1CurrentOutMode ,0,0,2+1)
	DTM_DateTime_Mode = XSC->DataBuff[sAddr+sf_Addr_DTM_DateTime_Mode];
	CheckValidData(DTM_DateTime_Mode,Type1_YYYYMMDD,Type1_YYYYMMDD,Type3_DDMMYYYY+1)
	BG_Color = XSC->DataBuff[sAddr+sf_Addr_BG_Color];
	CheckValidData(BG_Color,BG_BLACK,BG_BLUE,BG_BLACK+1)
	LCD_BL_Day = XSC->DataBuff[sAddr+sf_Addr_LCD_BL_Day];
	CheckValidData(LCD_BL_Day,100,0,101)
	LCD_BL_Night = XSC->DataBuff[sAddr+sf_Addr_LCD_BL_Night];
	CheckValidData(LCD_BL_Night,10,0,101)
	SIUS_SEL_Flag = XSC->DataBuff[sAddr+sf_Addr_SIUS_SEL_Flag];
	CheckValidData(SIUS_SEL_Flag,0,0,2)
	HMV.HourMeter_Trip = XSC->DataBuff[sAddr+sf_Addr_HMV_HourMeter_Trip];
	CheckValidData(HMV.HourMeter_Trip,0,0,3+1)
	Option_Function.Rotate_Flag = XSC->DataBuff[sAddr+sf_Addr_Option_Function_Rotate_Flag];
	CheckValidData(Option_Function.Rotate_Flag,1,0,2)
	M7_UserSet[0].A_Value = XSC->DataBuff[sAddr+sf_Addr_M7_UserSet0_A_Value];
	CheckValidData(M7_UserSet[0].A_Value,50,0,101)
	M7_UserSet[1].A_Value = XSC->DataBuff[sAddr+sf_Addr_M7_UserSet1_A_Value];
	CheckValidData(M7_UserSet[1].A_Value,37,0,101)
	M7_UserSet[2].A_Value = XSC->DataBuff[sAddr+sf_Addr_M7_UserSet2_A_Value];
	CheckValidData(M7_UserSet[2].A_Value,25,0,101)
	M7_UserSet[0].B_Value = XSC->DataBuff[sAddr+sf_Addr_M7_UserSet0_B_Value];
	CheckValidData(M7_UserSet[0].B_Value,50,0,101)
	M7_UserSet[1].B_Value = XSC->DataBuff[sAddr+sf_Addr_M7_UserSet1_B_Value];
	CheckValidData(M7_UserSet[1].B_Value,37,0,101)
	M7_UserSet[2].B_Value = XSC->DataBuff[sAddr+sf_Addr_M7_UserSet2_B_Value];
	CheckValidData(M7_UserSet[2].B_Value,25,0,101)
	M7_UserSet[0].P1_Value = XSC->DataBuff[sAddr+sf_Addr_M7_UserSet0_P1_Value];
	CheckValidData(M7_UserSet[0].P1_Value,68,0,101)
	M7_UserSet[1].P1_Value = XSC->DataBuff[sAddr+sf_Addr_M7_UserSet1_P1_Value];
	CheckValidData(M7_UserSet[1].P1_Value,68,0,101)
	M7_UserSet[2].P1_Value = XSC->DataBuff[sAddr+sf_Addr_M7_UserSet2_P1_Value];
	CheckValidData(M7_UserSet[2].P1_Value,68,0,101)
	M7_UserSet[0].ATT_State = XSC->DataBuff[sAddr+sf_Addr_M7_UserSet0_ATT_State];
	CheckValidData(M7_UserSet[0].ATT_State,1,1,11)
	M7_UserSet[1].ATT_State = XSC->DataBuff[sAddr+sf_Addr_M7_UserSet1_ATT_State];
	CheckValidData(M7_UserSet[1].ATT_State,2,1,11)
	M7_UserSet[2].ATT_State = XSC->DataBuff[sAddr+sf_Addr_M7_UserSet2_ATT_State];
	CheckValidData(M7_UserSet[2].ATT_State,3,1,11)
	M7_UserSet[0].Way_Opt = XSC->DataBuff[sAddr+sf_Addr_M7_UserSet0_Way_Opt];
	CheckValidData(M7_UserSet[0].Way_Opt,0,0,10)
	M7_UserSet[1].Way_Opt = XSC->DataBuff[sAddr+sf_Addr_M7_UserSet1_Way_Opt];
	CheckValidData(M7_UserSet[1].Way_Opt,0,0,10)
	M7_UserSet[2].Way_Opt = XSC->DataBuff[sAddr+sf_Addr_M7_UserSet2_Way_Opt];
	CheckValidData(M7_UserSet[2].Way_Opt,0,0,10)
	M7_UserAUX24Set.C_Value = XSC->DataBuff[sAddr+sf_Addr_M7_UserAUX24Set_C_Value];
	CheckValidData(M7_UserAUX24Set.C_Value,100,0,250)
	M7_UserAUX24Set.D_Value = XSC->DataBuff[sAddr+sf_Addr_M7_UserAUX24Set_D_Value];
	CheckValidData(M7_UserAUX24Set.D_Value,100,0,250)
	M7_UserAUX24Set.P2_Value = XSC->DataBuff[sAddr+sf_Addr_M7_UserAUX24Set_P2_Value];
	CheckValidData(M7_UserAUX24Set.P2_Value,68,0,250)
	M7_UserAUX24Set.G_Value = XSC->DataBuff[sAddr+sf_Addr_M7_UserAUX24Set_G_Value];
	CheckValidData(M7_UserAUX24Set.G_Value,100,0,250)
	M7_UserAUX24Set.H_Value = XSC->DataBuff[sAddr+sf_Addr_M7_UserAUX24Set_H_Value];
	CheckValidData(M7_UserAUX24Set.H_Value,100,0,250)
	M7_UserAUX24Set.P4_Value = XSC->DataBuff[sAddr+sf_Addr_M7_UserAUX24Set_P4_Value];
	CheckValidData(M7_UserAUX24Set.P4_Value,68,0,250)
	Option_Function.AutoStopKey = XSC->DataBuff[sAddr+sf_Addr_Option_Function_AutoStopKey];
	CheckValidData(Option_Function.AutoStopKey,1,0,2)
	
	CC1.Brightness = XSC->DataBuff[sAddr+sf_Addr_CC1_Brightness];
	if ((CC1.Brightness<-64)||(CC1.Brightness>63))
		CC1.Brightness = DEFAULT_CAM_BR;
	CC1.Contrast = XSC->DataBuff[sAddr+sf_Addr_CC1_Contrast];
	if ((CC1.Contrast<0)||(CC1.Contrast>127))
		CC1.Contrast = DEFAULT_CAM_CR;
	CC1.Saturation = XSC->DataBuff[sAddr+sf_Addr_CC1_Saturation];
	if ((CC1.Saturation<0)||(CC1.Saturation>127))
		CC1.Saturation = DEFAULT_CAM_SR;
	CC1.Hue = XSC->DataBuff[sAddr+sf_Addr_CC1_Hue];
	if ((CC1.Hue<-90)||(CC1.Hue>90))
		CC1.Hue = DEFAULT_CAM_HUE;
	CC1.Sharpness = XSC->DataBuff[sAddr+sf_Addr_CC1_Sharpness];
	if ((CC1.Sharpness<0)||(CC1.Sharpness>31))
		CC1.Sharpness = DEFAULT_CAM_SP;
	CC2.Brightness = XSC->DataBuff[sAddr+sf_Addr_CC2_Brightness];
	if ((CC2.Brightness<-64)||(CC2.Brightness>63))
		CC2.Brightness = DEFAULT_CAM_BR_N;
	CC2.Contrast = XSC->DataBuff[sAddr+sf_Addr_CC2_Contrast];
	if ((CC2.Contrast<0)||(CC2.Contrast>127))
		CC2.Contrast = DEFAULT_CAM_CR_N;
	CC2.Saturation = XSC->DataBuff[sAddr+sf_Addr_CC2_Saturation];
	if ((CC2.Saturation<0)||(CC2.Saturation>127))
		CC2.Saturation = DEFAULT_CAM_SR_N;
	CC2.Hue = XSC->DataBuff[sAddr+sf_Addr_CC2_Hue];
	if ((CC2.Hue<-90)||(CC2.Hue>90))
		CC2.Hue = DEFAULT_CAM_HUE_N;
	CC2.Sharpness = XSC->DataBuff[sAddr+sf_Addr_CC2_Sharpness];
	if ((CC2.Sharpness<0)||(CC2.Sharpness>31))
		CC2.Sharpness = DEFAULT_CAM_SP_N;
	
	for (i=0;i<MAX_SU100_USER_NUM;i++)
	{
		Lang_Flag[i] = XSC->DataBuff[sAddr+sf_Addr_Lang_Flag+i];
		CheckValidData(Lang_Flag[i],Lang_ENG,Lang_ENG,(Lang_ESP+1))
		User_RegInfo[i].Language = Lang_Flag[i];	
		Option_Function.AutoStop[i] = XSC->DataBuff[sAddr+sf_Addr_Option_Function_AutoStop+i];
		CheckValidData(Option_Function.AutoStop[i],1,0,2)
		
		User_RegInfo[i].RegKeyNum = i;
		User_RegInfo[i].RegFlag = XSC->DataBuff[sAddr+sf_Addr_User_RegInfo_RegFlag+i];
		CheckValidData(User_RegInfo[i].RegFlag,SU100_USER_ST,SU100_USER_N_R,SU100_USER_INIT+1)
		User_RegInfo[i].AccessAUX = XSC->DataBuff[sAddr+sf_Addr_User_RegInfo_AccessAUX+i];
		CheckValidData(User_RegInfo[i].AccessAUX,SU600_AUX_TYPE_NONE,SU600_AUX_TYPE_NONE,SU600_AUX_TYPE_NO+1)
		memcpy (User_RegInfo[i].UserPassCode,&XSC->DataBuff[sAddr+sf_Addr_User_RegInfo_UserPassCode+i*sf_Size_User_RegInfo_UserPassCode],MAX_SU100_USER_PW);
		for (j=0;j<MAX_SU100_USER_PW;j++)
		{
			if ((User_RegInfo[i].UserPassCode[j]<0x30)||(User_RegInfo[i].UserPassCode[j]>0x39))
			{
				User_RegInfo[i].UserPassCode[j] = 0;
				XSC->DataBuff[sAddr+sf_Addr_User_RegInfo_UserPassCode+j] = 0;
			}
		}	
		memcpy (User_RegInfo[i].UserName,&XSC->DataBuff[sAddr+sf_Addr_User_RegInfo_UserName+i*sf_Size_User_RegInfo_UserName],MAX_SU100_USER_NAME);
		for (j=0;j<MAX_SU100_USER_NAME;j++)
		{
			if (User_RegInfo[i].UserName[j]==0xFF)
			{
				User_RegInfo[i].UserName[j] = 0;
				XSC->DataBuff[sAddr+sf_Addr_User_RegInfo_UserName+j] = 0;
			}
		}	
	}
	memcpy (Device_UID,&XSC->DataBuff[sAddr+sf_Addr_Device_UID],MAX_DEVICE_UID);
	memcpy (PW100_PWD,&XSC->DataBuff[sAddr+sf_Addr_PW100_PWD],MAX_PW100_PWD);
	g_bPW100_PW_OnOff = XSC->DataBuff[sAddr+sf_Addr_PW100_PW_OnOff];
	g_nM7_Lock_Data[0].Lock = XSC->DataBuff[sAddr+sf_Addr_M7_Lock_Data_Value0_Lock];
	g_nM7_Lock_Data[1].Lock = XSC->DataBuff[sAddr+sf_Addr_M7_Lock_Data_Value1_Lock];
	g_nM7_Lock_Data[2].Lock = XSC->DataBuff[sAddr+sf_Addr_M7_Lock_Data_Value2_Lock];
	g_nM7_Lock_Data[0].Show = XSC->DataBuff[sAddr+sf_Addr_M7_Lock_Data_Value0_Show];
	g_nM7_Lock_Data[1].Show = XSC->DataBuff[sAddr+sf_Addr_M7_Lock_Data_Value1_Show];
	g_nM7_Lock_Data[2].Show = XSC->DataBuff[sAddr+sf_Addr_M7_Lock_Data_Value2_Show];


	//chpark.24.09.19
	if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON)
	{
		for(int i=0;i<3;i++)
		{
			if( g_nM7_Lock_Data[i].Lock == M7_LSTATE_UNLOCK)
			{
				g_nM7_Lock_Data[i].Lock = M7_LSTATE_LOCK;
			}
		}
	}
	//chpark.24.11.11
	if(g_bPW100_PW_OnOff)
	{
		PW100_Save_CLI();
	}

	#if 1
		printf("\n\n");
		memset (&TempData[0],0,sizeof(TempData));
		memcpy (&TempData[0],&XSC->DataBuff[sf_Addr_sSTART],sf_Size_sSTART);
	
		printf("Load sSTART : [%4s]\n",&TempData[0]);
		printf("Count : [%d]\n",(XSC->DataBuff[sf_Addr_Count+0]<<24)|(XSC->DataBuff[sf_Addr_Count+1]<<16)|(XSC->DataBuff[sf_Addr_Count+2]<<8)|(XSC->DataBuff[sf_Addr_Count+3]<<0));
		printf("M7_UserIndex : [%d]\n",XSC->DataBuff[sf_Addr_M7_UserIndex]);
		printf("AUX1CurrentOutMode : [%d]\n",AUX1CurrentOutMode);
		printf("DTM_DateTime_Mode : [%d]\n",DTM_DateTime_Mode);
		printf("BG_Color : [%d]\n",BG_Color);
		printf("LCD_BL_Day : [%d]\n",LCD_BL_Day);
		printf("LCD_BL_Night : [%d]\n",LCD_BL_Night);
		printf("SIUS_SEL_Flag : [%d]\n",SIUS_SEL_Flag);
		printf("HMV_HourMeter_Trip : [%d]\n",HMV.HourMeter_Trip);
		printf("Option_Function_Rotate_Flag : [%d]\n",Option_Function.Rotate_Flag);
		printf("M7_UserSet0_A_Value : [%d]\n",M7_UserSet[0].A_Value);
		printf("M7_UserSet1_A_Value : [%d]\n",M7_UserSet[1].A_Value);
		printf("M7_UserSet2_A_Value : [%d]\n",M7_UserSet[2].A_Value);
		printf("M7_UserSet0_B_Value : [%d]\n",M7_UserSet[0].B_Value);
		printf("M7_UserSet1_B_Value : [%d]\n",M7_UserSet[1].B_Value);
		printf("M7_UserSet2_B_Value : [%d]\n",M7_UserSet[2].B_Value);
		printf("M7_UserSet0_P1_Value : [%d]\n",M7_UserSet[0].P1_Value);
		printf("M7_UserSet1_P1_Value : [%d]\n",M7_UserSet[1].P1_Value);
		printf("M7_UserSet2_P1_Value : [%d]\n",M7_UserSet[2].P1_Value);
		printf("M7_UserSet0_ATT_State : [%d]\n",M7_UserSet[0].ATT_State);
		printf("M7_UserSet1_ATT_State : [%d]\n",M7_UserSet[1].ATT_State);
		printf("M7_UserSet2_ATT_State : [%d]\n",M7_UserSet[2].ATT_State);
		printf("M7_UserSet0_Way_Opt : [%d]\n",M7_UserSet[0].Way_Opt);
		printf("M7_UserSet1_Way_Opt : [%d]\n",M7_UserSet[1].Way_Opt);
		printf("M7_UserSet2_Way_Opt : [%d]\n",M7_UserSet[2].Way_Opt);
		printf("M7_UserAUX24Set_C_Value : [%d]\n",M7_UserAUX24Set.C_Value);
		printf("M7_UserAUX24Set_D_Value : [%d]\n",M7_UserAUX24Set.D_Value);
		printf("M7_UserAUX24Set_P2_Value : [%d]\n",M7_UserAUX24Set.P2_Value);
		printf("M7_UserAUX24Set_G_Value : [%d]\n",M7_UserAUX24Set.G_Value);
		printf("M7_UserAUX24Set_H_Value : [%d]\n",M7_UserAUX24Set.H_Value);
		printf("M7_UserAUX24Set_P4_Value : [%d]\n",M7_UserAUX24Set.P4_Value);
		printf("Option_Function_AutoStopKey : [%d]\n",Option_Function.AutoStopKey);
		
		printf("CC1_Brightness : [%d]\n",CC1.Brightness);
		printf("CC1_Contrast : [%d]\n",CC1.Contrast);
		printf("CC1_Saturation : [%d]\n",CC1.Saturation);
		printf("CC1_Hue	: [%d]\n",CC1.Hue);
		printf("CC1_Sharpness : [%d]\n",CC1.Sharpness);
		printf("CC2_Brightness : [%d]\n",CC2.Brightness);
		printf("CC2_Contrast : [%d]\n",CC2.Contrast);
		printf("CC2_Saturation : [%d]\n",CC2.Saturation);
		printf("CC2_Hue : [%d]\n",CC2.Hue);
		printf("CC2_Sharpness : [%d]\n",CC2.Sharpness);
		printf("\n");
		printf("Device_UID : [%20s]\n",&Device_UID[0]);
		printf("PW100_PWD : [%10s]\n",&PW100_PWD[0]);
		printf("PW100_PW_OnOff : [%d]\n",g_bPW100_PW_OnOff);
		printf("M7_Lock_Data_Value0_Lock : [%d]\n",g_nM7_Lock_Data[0].Lock);
		printf("M7_Lock_Data_Value1_Lock : [%d]\n",g_nM7_Lock_Data[1].Lock);
		printf("M7_Lock_Data_Value2_Lock : [%d]\n",g_nM7_Lock_Data[2].Lock);
		printf("M7_Lock_Data_Value0_Show : [%d]\n",g_nM7_Lock_Data[0].Show);
		printf("M7_Lock_Data_Value1_Show : [%d]\n",g_nM7_Lock_Data[1].Show);
		printf("M7_Lock_Data_Value2_Show : [%d]\n",g_nM7_Lock_Data[2].Show);
		printf("\n");
		for (i=0;i<MAX_SU100_USER_NUM;i++)//chpark.12.12
		{
			printf("Lang_Flag[%02d] : [%d]\n",i,Lang_Flag[i]);
			printf("Option_Function_AutoStop[%02d] : [%d]\n",i,Option_Function.AutoStop[i]);
			printf("User_RegInfo_RegKeyNum[%02d] : [%d]\n",i,User_RegInfo[i].RegKeyNum);
			printf("User_RegInfo_RegFlag[%02d] : [%d]\n",i,User_RegInfo[i].RegFlag);
			printf("User_RegInfo_AccessAUX[%02d] : [%d]\n",i,User_RegInfo[i].AccessAUX);
			printf("User_RegInfo_UserPassCode[%02d] : [%20s]\n",i,&User_RegInfo[i].UserPassCode[0]);
			printf("User_RegInfo_UserName[%02d] : [%20s]\n",i,&User_RegInfo[i].UserName[0]);
		}	
	memset (&TempData[0],0,sizeof(TempData));
	memcpy (&TempData[0],&XSC->DataBuff[sf_Addr_sEND],sf_Size_sSTART);
	
	printf("sEND : [%4s]\n",&TempData[0]);
		printf("\n\n");
		
	#endif

}

#ifdef TEST_FLASH_MEM
	extern uint32_t Flash_Save_Cnt;
	extern uint32_t Flash_Save_Start1;
	extern uint32_t Flash_Save_End1;
	extern uint32_t Flash_Save_sf_CNT1;
	extern uint32_t Flash_Save_Start2;
	extern uint32_t Flash_Save_End2;
	extern uint32_t Flash_Save_sf_CNT2;
	extern uint16_t Flash_Status;
#endif
void Flash_Write(uint8_t *DataBuff,uint32_t DataAddr) 
{
	int fd=0, ret;
	uint32_t pos, pos2;
#ifdef DGB_MSG_SF
	printf("Start Flash_Write:0x%02X , 0x%02X, 0x%02X\n",DataBuff[0],DataBuff[sf_BlockSize-1],sf_CNT);
#endif	
	ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
	fd = open(":nor", O_WRONLY, 0);

//sf_CNT = 0x789ABCDE;
	pos = DataAddr;
	pos = pos / (sf_BlockSize + sf_GapSize); // normally, blocksize is 65536, gapsize is 0
	pos2 = lseek(fd, pos, SEEK_SET);
//memset (DataBuff,0xFF,sf_BlockSize);

	printf("\nStart Flash_Write:[0x%08X],[%d]\n",pos,sf_CNT);
	ret = write(fd, DataBuff, sf_AlignSize);
	printf("End Flash_Write:[0x%08X],[0x%08X]\n",pos,sf_CNT);
	if(ret != 1)
	{
		#ifdef TEST_FLASH_MEM
			Flash_Status = 6;
		#endif
		printf("write error %d\n", ret);
	}

	if (fd)
	{
		close(fd);
	}
#ifdef TEST_FLASH_MEM
	Flash_Save_Cnt++;
#endif

#if 1	//def DGB_MSG_SF
	printf("End Flash_Write:0x%08X , sf_CNT:%d:0x%08X\n\n",DataAddr,sf_CNT,sf_CNT);
#endif		

}

void Flash_Read(uint8_t *DataBuff,uint32_t DataAddr)
{
	int fd=0, ret;
	uint32_t pos, pos2;
	uint32_t FlashEnd = 0;
	uint32_t FlashStart = 0;
	
	printf("\nStart Flash_Read:0x%08X\n",DataAddr);
	ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
	fd = open(":nor", O_RDONLY, 0);

	pos = DataAddr;
	pos = pos / (sf_BlockSize + sf_GapSize);
	pos2 = lseek(fd, pos, SEEK_SET);

	ret = read(fd, DataBuff, sf_AlignSize);
	if(ret != 1)
	{
		#ifdef TEST_FLASH_MEM
			Flash_Status = 7;
		#endif
		printf("write error %d\n\n", ret);
	}

	if (fd)
	{
		close(fd);
	}
	
	sf_CNT = (DataBuff[sf_Addr_Count+0]<<24) | (DataBuff[sf_Addr_Count+1]<<16) | (DataBuff[sf_Addr_Count+2]<<8) | (DataBuff[sf_Addr_Count+3]<<0);


	FlashStart = (DataBuff[sf_Addr_sSTART+0]<<24)|(DataBuff[sf_Addr_sSTART+1]<<16)|(DataBuff[sf_Addr_sSTART+2]<<8)|(DataBuff[sf_Addr_sSTART+3]<<0);
	FlashEnd = (DataBuff[sf_Addr_sEND+0]<<24)|(DataBuff[sf_Addr_sEND+1]<<16)|(DataBuff[sf_Addr_sEND+2]<<8)|(DataBuff[sf_Addr_sEND+3]<<0);
	
	printf("Start:0x%X , End:0x%X , sf_CNT:%d\n",FlashStart,FlashEnd,sf_CNT);
	
	#ifdef TEST_FLASH_MEM
	{
		static bool FirstFlag = 0;
		if (sf_CNT<=0)
		{
			Flash_Status+=10;
		}
		if (FirstFlag==0)
		{
			Flash_Save_Start1 = FlashStart;
			Flash_Save_End1 = FlashEnd;
			Flash_Save_sf_CNT1 = sf_CNT;
			FirstFlag = 1;
		}
		else
		{
			Flash_Save_Start2 = FlashStart;
			Flash_Save_End2 = FlashEnd;
			Flash_Save_sf_CNT2 = sf_CNT;
			FirstFlag = 0;
		}
	}	
	#endif
		
	if ((FlashStart!=(0x73745800|MODEL_INDEXs))||(FlashEnd!=(0x65645800|MODEL_INDEXe)))
	{
		DataBuff[sf_Addr_Count+0] = 0xFF;
		DataBuff[sf_Addr_Count+1] = 0xFF;
		DataBuff[sf_Addr_Count+2] = 0xFF;
		DataBuff[sf_Addr_Count+3] = 0xFF;		
	}
	printf("End Flash_Read\n");
}


void Flash_Init(void)
{
	int fd=0, ret,i,j;
	uint8_t *TempConfigBuff;
	uint32_t FlashCnt1 = 0;
	uint32_t FlashCnt2 = 0;
	uint32_t UserNum1 = 0;
	uint32_t UserNum2 = 0;
	uint32_t FlashEnd1 = 0;
	uint32_t FlashEnd2 = 0;
	
	
	XoSaveConfig XSC;


	printf("\n\nStart Flash_Init\n");

	// flush if FAT data is pending
	ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);

	fd = open(":nor", O_RDONLY, 0);

	if (ioctl(fd, ITP_IOCTL_GET_BLOCK_SIZE, &sf_BlockSize))
	{
		#ifdef TEST_FLASH_MEM
			Flash_Status = 1;
		#endif
		ret = __LINE__;
		printf("Get block size error\n");
	}

	if (ioctl(fd, ITP_IOCTL_GET_GAP_SIZE, &sf_GapSize))
	{
		#ifdef TEST_FLASH_MEM
			Flash_Status = 2;
		#endif
		ret = __LINE__;
		printf("get gap size error\n");
	}

	TempConfigBuff = malloc(sf_BlockSize);
	ConfigBuff = malloc(sf_BlockSize);
	if(ConfigBuff == NULL)
	{
		#ifdef TEST_FLASH_MEM
			Flash_Status = 3;
		#endif
		printf("malloc error\n");
		return -1;
	}
	memset(TempConfigBuff, 0, sf_BlockSize);
	memset(ConfigBuff, 0, sf_BlockSize);

	sf_AlignSize = ITH_ALIGN_UP(sf_BlockSize, sf_BlockSize) / sf_BlockSize; // alignsize must be 1, 2, ...

	printf("sf_AlignSize=%d, sf_BlockSize=%d, sf_GapSize=%d \n", sf_AlignSize, sf_BlockSize, sf_GapSize);

	if (fd)
	{
		close(fd);
	}


#if 0	
	Flash_Read(ConfigBuff,0);		// Read Bootloader for Bootloader Ver.
	i=0;
	j=0;
	
	printf("\n");
	for (i=0;i<10;i++)
	{
		printf("0x%04X : ",i*8);
		printf("[0x%02X][0x%02X][0x%02X][0x%02X] [0x%02X][0x%02X][0x%02X][0x%02X]\n",ConfigBuff[i*8+0],ConfigBuff[i*8+1],ConfigBuff[i*8+2],ConfigBuff[i*8+3],ConfigBuff[i*8+4],ConfigBuff[i*8+5],ConfigBuff[i*8+6],ConfigBuff[i*8+7]);
	}
	
	printf("\n");
#endif	
	
	Flash_Read(TempConfigBuff	,sf_FLASH_BLOCK1_ADDR);
	Flash_Read(ConfigBuff		,sf_FLASH_BLOCK2_ADDR);


	FlashCnt1 = (TempConfigBuff[sf_Addr_Count]<<24)|(TempConfigBuff[sf_Addr_Count+1]<<16)|(TempConfigBuff[sf_Addr_Count+2]<<8)|(TempConfigBuff[sf_Addr_Count+3]<<0);
	FlashCnt2 = (ConfigBuff[sf_Addr_Count]<<24)|(ConfigBuff[sf_Addr_Count+1]<<16)|(ConfigBuff[sf_Addr_Count+2]<<8)|(ConfigBuff[sf_Addr_Count+3]<<0);
	
	UserNum1 = TempConfigBuff[sf_Addr_M7_UserIndex];
	UserNum2 = ConfigBuff[sf_Addr_M7_UserIndex];
	
	FlashEnd1 = (TempConfigBuff[sf_BlockSize-3]<<16)|(TempConfigBuff[sf_BlockSize-2]<<8)|(TempConfigBuff[sf_BlockSize-1]<<0);
	FlashEnd2 = (ConfigBuff[sf_BlockSize-3]<<16)|(ConfigBuff[sf_BlockSize-2]<<8)|(ConfigBuff[sf_BlockSize-1]<<0);
		
	printf("\nFlashCnt1=0x%08X:0x%08X:%d, FlashCnt2=0x%08X:0x%08X:%d \n", FlashCnt1,UserNum1,FlashEnd1, FlashCnt2,UserNum2,FlashEnd2);
//	if (((FlashCnt1==0xFFFFFFFF)&&(FlashCnt2==0xFFFFFFFF))||((FlashCnt1==0)&&(FlashCnt2==0))||((UserNum1==0xFF)&&(UserNum2==0xFF)))
	if (((FlashCnt1==0xFFFFFFFF)&&(FlashCnt2==0xFFFFFFFF))||((FlashCnt1==0)&&(FlashCnt2==0)))
	{
		// Write Default
		printf("Write Default 1 \n");
		memset(ConfigBuff, 0, sf_BlockSize);
		XSC.DataBuff=ConfigBuff;
		XSC.FlashAddr=sf_FLASH_BLOCK2_ADDR;
		XSC.UserNum=M7_UserIndex;
		XSC.SaveFlag=1;
		XSC.ForceSave=1;
		sf_CNT=1;
		SaveConfigData (&XSC,1);
		
		XSC.FlashAddr=sf_FLASH_BLOCK1_ADDR;
		SaveConfigData (&XSC,1);

		memset(ConfigBuff, 0, sf_BlockSize);
		Flash_Read(ConfigBuff,sf_FLASH_BLOCK2_ADDR);

		XSC_FlashAddr=sf_FLASH_BLOCK1_ADDR;
		#ifdef TEST_FLASH_MEM
			Flash_Status = 4;
		#endif
	}
	else
	{
		if ((FlashCnt1!=0xFFFFFFFF)&&(FlashCnt1!=0)&&(FlashCnt2!=0xFFFFFFFF)&&(FlashCnt2!=0))
		{
			if (FlashCnt1>FlashCnt2)
			{
				//Check Start and End
				printf("==>FlashCnt1 is High==> used sf_FLASH_BLOCK2_ADDR:0x%08X\n",sf_FLASH_BLOCK2_ADDR);
				memcpy(ConfigBuff, TempConfigBuff, sf_BlockSize);
				XSC_FlashAddr=sf_FLASH_BLOCK2_ADDR;
			}
			else
			{
				printf("==>FlashCnt2 is High==> used sf_FLASH_BLOCK1_ADDR:0x%08X\n",sf_FLASH_BLOCK1_ADDR);
				XSC_FlashAddr=sf_FLASH_BLOCK1_ADDR;
			}
		}
		else if ((FlashCnt1!=0xFFFFFFFF)&&(FlashCnt1!=0))
		{
			//Check 1
			printf("==>FlashCnt2 is 0xFFFFFFFF==> used sf_FLASH_BLOCK2_ADDR:0x%08X\n",sf_FLASH_BLOCK2_ADDR);
			memcpy(ConfigBuff, TempConfigBuff, sf_BlockSize);
			XSC_FlashAddr=sf_FLASH_BLOCK2_ADDR;
		}
		else if ((FlashCnt2!=0xFFFFFFFF)&&(FlashCnt2!=0))
		{
			//Check 2
			printf("==>FlashCnt1 is 0xFFFFFFFF==> used sf_FLASH_BLOCK1_ADDR:0x%08X\n",sf_FLASH_BLOCK1_ADDR);
			XSC_FlashAddr=sf_FLASH_BLOCK1_ADDR;
		}
		else
		{
			// Write Default
			printf("Write Default 2 \n");
			XSC.DataBuff=ConfigBuff;
			XSC.FlashAddr=sf_FLASH_BLOCK2_ADDR;
			XSC.UserNum=M7_UserIndex;
			sf_CNT=1;
			SaveConfigData (&XSC,1);
			
			XSC.FlashAddr=sf_FLASH_BLOCK1_ADDR;
			SaveConfigData (&XSC,1);
	
			memset(ConfigBuff, 0, sf_BlockSize);
			Flash_Read(ConfigBuff,sf_FLASH_BLOCK2_ADDR);
	
			XSC_FlashAddr=sf_FLASH_BLOCK1_ADDR;
			#ifdef TEST_FLASH_MEM
				Flash_Status = 5;
			#endif
		}
	}
	{
		XSC.DataBuff=ConfigBuff;
		XSC.FlashAddr=NULL;
		XSC.UserNum=M7_UserIndex;
		XSC.SaveFlag=1;
		XSC.ForceSave=1;
		LoadConfigData (&XSC,0);
	}	
	free(TempConfigBuff);
	printf("End Flash_Init\n\n");
}



XoSaveConfig gXSC;


void XO_Save_Config(uint8_t tUserNum,bool tSaveFlag,bool tForceSave)
{
#if 1	//def DGB_MSG_SF
	printf("\nStart XO_Save_Config:0x%08X , sf_CNT:%d , tSaveFlag:%d , tForceSave:%d\n",XSC_FlashAddr,sf_CNT,tSaveFlag,tForceSave);
#endif
//	if (SaveFlag)
	{
		gXSC.DataBuff=ConfigBuff;
		gXSC.FlashAddr=XSC_FlashAddr;
		gXSC.UserNum=tUserNum;
		gXSC.SaveFlag=tSaveFlag;
		gXSC.ForceSave=tForceSave;
		XO_ConfigSave();
	}
#ifdef DGB_MSG_SF
	printf("End XO_Save_Config:0x%08X , sf_CNT:%d:0x%08X , DataCmd:%d\n\n",XSC_FlashAddr,sf_CNT,sf_CNT,tDataCmd);
#endif
}


void XO_ConfigSavePublic(void)
{
	SaveConfigData (&gXSC,0);
	if (XSC_FlashAddr==sf_FLASH_BLOCK1_ADDR)
		XSC_FlashAddr=sf_FLASH_BLOCK2_ADDR;
	else
		XSC_FlashAddr=sf_FLASH_BLOCK1_ADDR;
}


void XO_Mem_Reset()
{
	int i,j;
	
	SettingInit();

	if(Main_DayNight==0)	//add whlim, day night brightness reflection
		LCDLayer_BackLight(LCD_BL_Day);
	else if(Main_DayNight==1)
		LCDLayer_BackLight(LCD_BL_Night);

	XO_Save_Config(0,true,true);
}

