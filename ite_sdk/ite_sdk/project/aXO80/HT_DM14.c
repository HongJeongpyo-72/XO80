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

extern DM14_DATA	DM14_Data;
extern DM15_DATA 	DM15_Data;
extern TPCM_RTS		TPCM_Rts;
extern TPCM_CTS		TPCM_Cts;
extern TPCM_ACK		TPCM_Ack;
extern DM14_TPDT	DM14_Tpdt;

uint16_t UeseSeed = 0;
bool Sand_TPCM(uint8_t cmd,uint32_t tData);


#define DM14_DATA_PACKET	7
#define DM14_DATA_LENGHT	35
#define DM14_DATA_LENGHT1	30


#define DM14_TX_DELAY  	SDL_Delay(5);
#define DM14_TX_DELAY2  SDL_Delay(5);


void ClearDM14(void)
{
	memset(&User_RegInfoDM14,0,sizeof(User_RegInfoDM14));
	
	memset(&DM14_Data,0,sizeof(DM14_Data));
	memset(&DM15_Data,0,sizeof(DM15_Data));
	memset(&TPCM_Rts,0,sizeof(TPCM_Rts));
	memset(&TPCM_Cts,0,sizeof(TPCM_Cts));
	memset(&TPCM_Ack,0,sizeof(TPCM_Ack));
	memset(&DM14_Tpdt,0,sizeof(DM14_Tpdt));
	DM14_Data.User_Level = 0;	// TTMS 15226
}


uint16_t CalSeed (uint16_t tSeed)	// 2024-01-18
{
	unsigned int delta = 0x8168;		// 33128
	unsigned int key, sum, i;
	unsigned char v0, v1;				
	v0 = ((tSeed + delta) >> 8) & 0xFF;	//62509	F42D      62509+33128 = 95637(17595)>>8    75
	v1 = (tSeed + delta) & 0xFF;		//62509	F42D      62509+33128 = 95637(17595)     7595
	for (i = 0; i < 32; i++)
	{
		sum += delta;
		v0 += (v1 >> 2) ^ (v1 + sum) ^ (v1 << 3);
		v1 += (v0 >> 2) ^ (v0 + sum) ^ (v0 << 3);
	}
	key = (v0 << 8) + v1;
	
	printf("[CalSeed]SEED=0x%04X ,key=0x%04X\n",tSeed,key);
	
	return key;
}

void Save_DM14(void)
{
	int i;
	
	if ((TPCM_Rts.Length<=DM14_Tpdt.Count)||(TPCM_Rts.TNoP<=DM14_Tpdt.PSN))
	{
		int NumCnt = DM14_Tpdt.Count/DM14_DATA_LENGHT;
		
		for (i=0;i<NumCnt;i++)
		{
			User_RegInfoDM14.RegKeyNum = ((DM14_Data.Pointer&0xFFFF)/DM14_DATA_LENGHT)+i;
			
			User_RegInfo[User_RegInfoDM14.RegKeyNum].RegFlag = DM14_Data.User_Level;
			memcpy (User_RegInfo[User_RegInfoDM14.RegKeyNum].UserName,&DM14_Tpdt.Data[i*DM14_DATA_LENGHT],DM14_DATA_LENGHT1);
			memset(User_RegInfo[User_RegInfoDM14.RegKeyNum].UserPassCode,0x30,5);
			User_RegInfo[User_RegInfoDM14.RegKeyNum].Language = DM14_Tpdt.Data[i*DM14_DATA_LENGHT+DM14_DATA_LENGHT1]&0x0F;
			if (((DM14_Tpdt.Data[i*DM14_DATA_LENGHT+DM14_DATA_LENGHT1]&0x30)>>4)==1)
			{
	            User_RegInfo[User_RegInfoDM14.RegKeyNum].AccessAUX        = SU600_AUX_TYPE_NO;
	            User_RegInfo[User_RegInfoDM14.RegKeyNum].nTmp_AccessAUX   = SU600_AUX_TYPE_NO;
			}
			else if (((DM14_Tpdt.Data[i*DM14_DATA_LENGHT+DM14_DATA_LENGHT1]&0x30)>>4)==2)
			{
	            User_RegInfo[User_RegInfoDM14.RegKeyNum].AccessAUX        = SU600_AUX_TYPE_YES;
	            User_RegInfo[User_RegInfoDM14.RegKeyNum].nTmp_AccessAUX   = SU600_AUX_TYPE_YES;
			}
			else
			{
	            User_RegInfo[User_RegInfoDM14.RegKeyNum].AccessAUX        = SU600_AUX_TYPE_NONE;
	            User_RegInfo[User_RegInfoDM14.RegKeyNum].nTmp_AccessAUX   = SU600_AUX_TYPE_NONE;
			}
			Lang_Flag[User_RegInfoDM14.RegKeyNum] = User_RegInfo[User_RegInfoDM14.RegKeyNum].Language;
			User_RegInfo[User_RegInfoDM14.RegKeyNum].bRegister = true;
					
			printf("\n[TP_DT_T]RegKeyNum=%d\n",User_RegInfoDM14.RegKeyNum);
			printf("[TP_DT_T]RegFlag=[%d]\n",User_RegInfo[User_RegInfoDM14.RegKeyNum].RegFlag);
			printf("[TP_DT_T]UserName=[%s]\n",User_RegInfo[User_RegInfoDM14.RegKeyNum].UserName);
			printf("[TP_DT_T]UserPassCode=[%s]\n",User_RegInfo[User_RegInfoDM14.RegKeyNum].UserPassCode);
			printf("[TP_DT_T]Language=[%d]\n\n",User_RegInfo[User_RegInfoDM14.RegKeyNum].Language);
			printf("[TP_DT_T]AccessAUX=[%d]\n\n",User_RegInfo[User_RegInfoDM14.RegKeyNum].AccessAUX);
//			XO_Save_Config(User_RegInfoDM14.RegKeyNum,true,0);
//			SDL_Delay(1000);
		}
		if (NumCnt>1)
			XO_Save_Config(((DM14_Data.Pointer&0xFFFF)/DM14_DATA_LENGHT),true,0);
		else
			XO_Save_Config(User_RegInfoDM14.RegKeyNum,true,0);
	}
}


bool Sand_DM14(uint8_t cmd,uint8_t index,uint16_t tData)
{
	switch (cmd)
	{
		case DM14_TX_PROCEED:
		{
			printf("\n[Sand_DM14] DM14_TX_PROCEED[%d] \n",index);
			if (index==0)
			{
				memset(&User_RegInfoDM14,0,sizeof(User_RegInfoDM14));
				memset(&DM15_Data,0,sizeof(DM15_Data));
				memset(&TPCM_Rts,0,sizeof(TPCM_Rts));
				memset(&TPCM_Cts,0,sizeof(TPCM_Cts));
				memset(&TPCM_Ack,0,sizeof(TPCM_Ack));
				memset(&DM14_Tpdt,0,sizeof(DM14_Tpdt));
				
				DM15_Data.Length = DM14_Data.Length;// 0Byte,1Byte 5-7Bit
				DM15_Data.Status = DM14_TX_PROCEED;	// 1Byte 1-3Bit
				DM15_Data.EDCP = 0;					// 2-4Byte
				DM15_Data.EDCPEx = 0xFF;			// 5Byte
				UeseSeed	= gMainTimerCnt_10m&0xFFFF;
				UeseSeed	= 0xF42D;	//gMainTimerCnt_10m;
				
				DM15_Data.Seed	 = UeseSeed;		// 6-7Byte
				DM14_TX_DELAY
				CanbusTxDataManagement( gMainTimerCnt_10m,DM15D,NULL);
			}
			else if (index==1)
			{
				DM15_Data.Length = DM14_Data.Length;	// 0Byte,1Byte 5-7Bit
				DM15_Data.Status = DM14_TX_PROCEED;		// 1Byte 1-3Bit
				DM15_Data.EDCP = 0;						// 2-4Byte
				DM15_Data.EDCPEx = 0xFF;				// 5Byte
				DM15_Data.Seed	 = 0xFFFF;	// UeseSeed;			// 6-7Byte
				DM14_TX_DELAY
				CanbusTxDataManagement( gMainTimerCnt_10m,DM15D,NULL);
				DM14_TX_DELAY
				if (DM14_Data.Command==DM14_RX_WRITE)
					Sand_TPCM(TPCM_RX_CTS,DM16_PGN);
				else if (DM14_Data.Command==DM14_RX_READ)
					Sand_TPCM(TPCM_RX_RTS,DM16_PGN);
			}
			else if (index==2)
			{

			}
		}
		break;
		case DM14_TX_BUSY:
		break;
		case DM14_TX_COMPLETED:
		{
			printf("\n[Sand_DM14] DM14_TX_COMPLETED[%d][%d] \n",index,DM14_Data.Length);
			DM15_Data.Length = DM14_Data.Length;	// 0Byte,1Byte 5-7Bit
			DM15_Data.Status = DM14_TX_COMPLETED;	// 1Byte 1-3Bit
			DM15_Data.EDCP = 0;						// 2-4Byte
			DM15_Data.EDCPEx = 255;					// 5Byte
			DM15_Data.Seed	 = UeseSeed;			// 6-7Byte
			DM14_TX_DELAY
			CanbusTxDataManagement( gMainTimerCnt_10m,DM15D,NULL);

			if (DM14_Data.Command==DM14_RX_WRITE)
			{
				Save_DM14();
			}
		}
		break;
		case DM14_TX_FAILED:
		{
			printf("\n[Sand_DM14] DM14_TX_FAILED[%d] \n",index);
		}
		break;
	}
	return true;
}

bool Sand_DM14_Error(uint32_t cmd,uint8_t index,uint16_t tData)
{
	switch (cmd)
	{
		case DM14_ERROR_LENGTH:		// 0x00102
		{
			printf("\n[Sand_DM14_Error] DM14_ERROR_LENGTH[%d] \n",DM14_Data.Length);
			DM15_Data.Length = DM14_Data.Length;
			DM15_Data.Status = DM14_TX_BUSY;			// 1Byte 1-3Bit
			DM15_Data.EDCP = DM14_ERROR_LENGTH;			// 2-4Byte
			DM15_Data.EDCPEx = 6;						// 5Byte
			DM15_Data.Seed	 = 0xFF;					// 6-7Byte
			DM14_TX_DELAY
			CanbusTxDataManagement( gMainTimerCnt_10m,DM15D,NULL);
			ClearDM14();
			DM14_Data.DM14_ErrorFlag = DM14_ERROR_LENGTH;
		}
		break;
		case DM14_ERROR_POINTER:	// 0x00100
		{
			printf("\n[Sand_DM14_Error] DM14_ERROR_POINTER[%d] \n",DM14_Data.Pointer);
			DM15_Data.Length = DM14_Data.Length;
			DM15_Data.Status = DM14_TX_BUSY;			// 1Byte 1-3Bit
			DM15_Data.EDCP = DM14_ERROR_POINTER;		// 2-4Byte
			DM15_Data.EDCPEx = 6;						// 5Byte
			DM15_Data.Seed	 = UeseSeed;				// 6-7Byte
			DM14_TX_DELAY
			CanbusTxDataManagement( gMainTimerCnt_10m,DM15D,NULL);
			ClearDM14();
			DM14_Data.DM14_ErrorFlag = DM14_ERROR_POINTER;
		}
		break;
		case DM14_ERROR_SEED:		// 0x01003
		{
			printf("\n[Sand_DM14_Error] DM14_ERROR_SEED[%d] \n",DM14_Data.Key);
			DM15_Data.Length = DM14_Data.Length;
			DM15_Data.Status = DM14_TX_BUSY;			// 1Byte 1-3Bit
			DM15_Data.EDCP = DM14_ERROR_SEED;			// 2-4Byte
			DM15_Data.EDCPEx = 6;						// 5Byte
			DM15_Data.Seed	 = UeseSeed;				// 6-7Byte
			DM14_TX_DELAY
			CanbusTxDataManagement( gMainTimerCnt_10m,DM15D,NULL);
			ClearDM14();
			DM14_Data.DM14_ErrorFlag = DM14_ERROR_SEED;
		}
		break;
		case DM14_ERROR_TIME:		// 0x10002
		{
			printf("\n[Sand_DM14_Error] DM14_ERROR_TIME[%d][%d] \n",DM14_Data.Time,gMainTimerCnt_10m);
			DM15_Data.Length = DM14_Data.Length;
			DM15_Data.Status = DM14_TX_BUSY;			// 1Byte 1-3Bit
			DM15_Data.EDCP = DM14_ERROR_TIME;			// 2-4Byte
			DM15_Data.EDCPEx = 6;						// 5Byte
			DM15_Data.Seed	 = UeseSeed;				// 6-7Byte
			DM14_TX_DELAY
			CanbusTxDataManagement( gMainTimerCnt_10m,DM15D,NULL);
			ClearDM14();
			DM14_Data.DM14_ErrorFlag = DM14_ERROR_TIME;
		}
		break;
	}
}



bool Sand_TPCM(uint8_t cmd,uint32_t tData)
{
//printf("\n[Sand_TPCM] cmd[%d]\n",cmd);
	DM14_TX_DELAY
	switch (cmd)
	{
		case TPCM_RX_RTS:							// 16, Destination Specific Request_To_Send (RTS)
		{
			TPCM_Cts.Control = 0;					// 0Byte
			TPCM_Ack.Control = cmd;					// 0Byte
			TPCM_Ack.Size = DM14_Data.Length;		// 1,2Byte	Total message size, number of bytes
			TPCM_Ack.Packet = TPCM_Ack.Size/DM14_DATA_PACKET;	// 3Byte	Total number of packets
			if (TPCM_Ack.Size%DM14_DATA_PACKET)
				TPCM_Ack.Packet++;
			TPCM_Ack.SAE = 0xFF;									// 4Byte	Reserved for assignment by SAE, these bytes should be filled with FF
			TPCM_Ack.PGN = tData;									// 5-7Byte	Parameter Group Number of the packeted message(PGN) DM16(0xD700)
			DM14_TX_DELAY
			CanbusTxDataManagement( gMainTimerCnt_10m,TP_CM_D,NULL);		
		}
		break;
		case TPCM_RX_CTS:		// 17, Destination Specific Clear_To_Send (CTS)
		{
			TPCM_Ack.Control = 0;
			TPCM_Cts.Control = cmd;		// 0Byte
			TPCM_Cts.NPS = 0xFF;		// 1Byte	Number of packets that can be sent
										// Number of packets that can be sent. This value shall be no larger than the smaller of the 
										// two values in byte 4 and byte 5 of the RTS message. 
			TPCM_Cts.NPN = 0xFF;		// 2Byte	Next packet number to be sent // zero not allowed
			TPCM_Cts.SAE = 0xFFFF;		// 3,4Byte	Reserved for assignment by SAE, these bytes should be filled with FF
			TPCM_Cts.PGN = tData;		// 5-7Byte	Parameter Group Number of the packeted message(PGN) DM16(0xD700)
			DM14_TX_DELAY
			CanbusTxDataManagement( gMainTimerCnt_10m,TP_CM_D,NULL);
		}
		break;
		case TPCM_RX_EOMA:		// 19, End_of_Message Acknowledge
		{
			TPCM_Cts.Control = 0;				// 0Byte
			TPCM_Ack.Control = cmd;				// 0Byte
			TPCM_Ack.Size = TPCM_Rts.Length;	// 1,2Byte	Total message size, number of bytes
			TPCM_Ack.Packet = TPCM_Rts.TNoP;	// 3Byte	Total number of packets
			TPCM_Ack.SAE = 0xFF;				// 4Byte	Reserved for assignment by SAE, these bytes should be filled with FF
			TPCM_Ack.PGN = tData;				// 5-7Byte	Parameter Group Number of the packeted message(PGN) DM16(0xD700)
			DM14_TX_DELAY
			CanbusTxDataManagement( gMainTimerCnt_10m,TP_CM_D,NULL);
		}
		break;
		case TPCM_RX_BAM:		// 32, Broadcast Announce Message
		{
		}
		break;
	}
	return true;
}

uint8_t  SandTPDT_Data[8] = {0,};

bool Sand_TPDT(uint8_t cmd,uint32_t tData)
{
	int i=0,j=0;
	int NumCnt = DM14_Data.Length/DM14_DATA_LENGHT;

	DM14_Tpdt.PSN = 1;
	
//	printf("\n[Sand_TPDT] User_RegInfoDM14.RegKeyNum[%d] DM14_Data.Pointer[%d] NumCnt[%d] \n",User_RegInfoDM14.RegKeyNum,DM14_Data.Pointer, NumCnt);	
	for (i=0;i<NumCnt;i++)
	{
		User_RegInfoDM14.RegKeyNum = (DM14_Data.Pointer&0xFFFF)/DM14_DATA_LENGHT+i;
		
//		printf("\n[Sand_TPDT] User_RegInfo[%d].UserName[%s]\n",User_RegInfoDM14.RegKeyNum,User_RegInfo[User_RegInfoDM14.RegKeyNum].UserName);
//		printf("[Sand_TPDT] User_RegInfo[%d].UserPassCode[%s]\n",User_RegInfoDM14.RegKeyNum,User_RegInfo[User_RegInfoDM14.RegKeyNum].UserPassCode);
		DM14_Tpdt.Count = 0;
	
		for (j=0;j<5;j++)
		{	
			SandTPDT_Data[0] = DM14_Tpdt.PSN++;
			memcpy (&SandTPDT_Data[1],&User_RegInfo[User_RegInfoDM14.RegKeyNum].UserName[j*DM14_DATA_PACKET],DM14_DATA_PACKET);
			if (j==4)
			{
				if ((User_RegInfo[User_RegInfoDM14.RegKeyNum].AccessAUX)==SU600_AUX_TYPE_NO)
				{
		            SandTPDT_Data[3] = 0x10|(User_RegInfo[User_RegInfoDM14.RegKeyNum].Language&0x0F);
				}
				else if ((User_RegInfo[User_RegInfoDM14.RegKeyNum].AccessAUX)==SU600_AUX_TYPE_YES)
				{
		            SandTPDT_Data[3] = 0x20|(User_RegInfo[User_RegInfoDM14.RegKeyNum].Language&0x0F);
				}
				else
				{
		            SandTPDT_Data[3] = 0x00|(User_RegInfo[User_RegInfoDM14.RegKeyNum].Language&0x0F);
				}
				SandTPDT_Data[4] = 0xFF;
				SandTPDT_Data[5] = 0xFF;
				SandTPDT_Data[6] = 0xFF;
				SandTPDT_Data[7] = 0xFF;
			}
//			printf("SandTPDT_Data[0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X]\n",SandTPDT_Data[0],SandTPDT_Data[1],SandTPDT_Data[2],SandTPDT_Data[3],SandTPDT_Data[4],SandTPDT_Data[5],SandTPDT_Data[6],SandTPDT_Data[7]);
			DM14_TX_DELAY
			CanbusTxDataManagement( gMainTimerCnt_10m,TP_DT_D,NULL);
			
		}		
	}
	return true;
}

bool Parsing_DM14(void)
{

	uint32_t tSeed = 0;

	//if ((DM14_Data.Length!=DM14_DATA_LENGHT1)&&((DM14_Data.Length%DM14_DATA_LENGHT)!=0))
	if ((DM14_Data.Length%DM14_DATA_LENGHT)!=0)
	{
		Sand_DM14_Error(DM14_ERROR_LENGTH,0,0);
	}
	else if ((((DM14_Data.Pointer&0xFFFF)+DM14_DATA_LENGHT-1)%DM14_DATA_LENGHT)!=0)
	{
		Sand_DM14_Error(DM14_ERROR_POINTER,0,0);
	}
	else
	{
		switch (DM14_Data.Command)
		{
			case DM14_RX_ERASE:
			break;
			case DM14_RX_READ:
			{
				if (DM14_Data.Count==0)
				{
					User_RegInfoDM14.RegKeyNum = (DM14_Data.Pointer&0xFFFF)/DM14_DATA_LENGHT;
					User_RegInfoDM14.Level = DM14_Data.User_Level;
					if (User_RegInfoDM14.RegKeyNum>=MAX_SU100_USER_NUM)
					{
						User_RegInfoDM14.RegKeyNum = 0xFF;
					}
					Sand_DM14(DM14_TX_PROCEED,DM14_Data.Count,0);
					DM14_Data.Count=1;
					DM14_Data.Time=gMainTimerCnt_10m;
				}
				else if (DM14_Data.Count==1)
				{
					tSeed = CalSeed(DM15_Data.Seed);
					DM15_Data.Seed = 0xFFFF;
					if (tSeed == DM14_Data.Key)
					{
						Sand_DM14(DM14_TX_PROCEED,DM14_Data.Count,tSeed);
						DM14_Data.Count=2;
						DM14_Data.Time=gMainTimerCnt_10m;
					}
					else	//Wrong Key
					{
						Sand_DM14_Error(DM14_ERROR_SEED,0,0);
					}
				}
			}
			break;
			case DM14_RX_WRITE:
			{
				if (DM14_Data.Count==0)
				{
					Sand_DM14(DM14_TX_PROCEED,DM14_Data.Count,0);
					DM14_Data.Count=1;
					DM14_Data.Time=gMainTimerCnt_10m;
				}
				else if (DM14_Data.Count==1)
				{
					tSeed = CalSeed(DM15_Data.Seed);
					DM15_Data.Seed = 0xFFFF;
					if (tSeed == DM14_Data.Key)
					{
						Sand_DM14(DM14_TX_PROCEED,DM14_Data.Count,tSeed);
						DM14_Data.Count=2;
						DM14_Data.Time=gMainTimerCnt_10m;
					}
					else	//Wrong Key
					{
						Sand_DM14_Error(DM14_ERROR_SEED,0,0);
					}
				}
			}
			break;
			case DM14_RX_REQUEST:
			break;
			case DM14_RX_COMPLETED:
			{
				ClearDM14();
			}
			break;
			case DM14_RX_FAILED:
			break;
			case DM14_RX_LOAD:
			break;
			case DM14_RX_GENERATION:
			break;
		}
	}
	return true;
}

bool Parsing_TPCM(void)
{

printf("\n[Parsing_TPCM] TPCM_Rts.Control[%d]\n",TPCM_Rts.Control);
	switch (TPCM_Rts.Control)
	{
		case TPCM_RX_RTS:		// 16, Destination Specific Request_To_Send (RTS)
		{
			if ((TPCM_Rts.PGN==DM16_PGN)&&(DM14_Data.Count==1))
			{
				DM14_TX_DELAY
				Sand_TPCM(TPCM_RX_CTS,TPCM_Rts.PGN);
				DM14_Data.Count=2;
			}
		}
		break;
		case TPCM_RX_CTS:		// 17, Destination Specific Clear_To_Send (CTS)
		{
		}
		break;
		case TPCM_RX_EOMA:		// 19, End_of_Message Acknowledge
		{
		}
		break;
		case TPCM_RX_BAM:		// 32, Broadcast Announce Message
		{
		}
		break;
	}
	return true;
}


void DM14_Timer(void)
{
	if (((DM14_Data.Time+TIME_CNT_10SEC)<gMainTimerCnt_10m)&&(DM14_Data.Time!=0))
	{
		Sand_DM14_Error(DM14_ERROR_TIME,0,0);
	}
}



