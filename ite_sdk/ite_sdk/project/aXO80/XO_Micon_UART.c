#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "ite/ith.h"
#include "ite/itp.h"
#include "openrtos/FreeRTOS.h"
#include "openrtos/queue.h"
#include "openrtos/FreeRTOS.h"
#include "openrtos/task.h"
#include "scene.h"
#include "HT_DispDdefine.h"
#include "Menu_Layer.h"

#include <stdlib.h>
#include <math.h>

#define DM14_DBG_MSG


#define MICOM_RX_DATA_CNT 	2000
#define MICOM_RX_DATA_OVF	300
#define MICOM_TX_DATA_CNT 	MICOM_RX_DATA_CNT

#define MICOM_STX_AP		0x3E	//from AP
#define MICOM_STX_MICOM		0x3C	//to AP
#define MICOM_ETX			0x2F
#define MICOM_DATA_LENTH	16



typedef struct{
	uint8_t RxData[MICOM_RX_DATA_CNT][MICOM_DATA_LENTH];
	uint8_t RxFlag[MICOM_RX_DATA_CNT];
	uint16_t Rx_Read;
	uint16_t Rx_Write;
}UART_RX_DATA;

typedef struct{
	uint8_t TxData[MICOM_TX_DATA_CNT][MICOM_DATA_LENTH];
	uint8_t TxFlag[MICOM_TX_DATA_CNT];
	uint16_t Tx_Read;
	uint16_t Tx_Write;
}UART_TX_DATA;

volatile UART_RX_DATA 	MICOM_RX;
volatile UART_TX_DATA 	MICOM_TX;


#define MICOM_STACK_SIZE 1024000

#ifdef TEST_ERR_602
	volatile uint8_t ConnectCan=0;
#else
	volatile uint8_t ConnectCan=3;//0; 	//8.0 3   , 9.0 : 0
#endif
DM14_DATA 	DM14_Data;
DM15_DATA 	DM15_Data;
TPCM_RTS	TPCM_Rts;
TPCM_CTS 	TPCM_Cts;
TPCM_ACK	TPCM_Ack;
DM14_TPDT	DM14_Tpdt;



#ifdef CFG_DBG_UART0
	#undef CFG_UART0_INTR
	#undef CFG_UART0_DMA
	#undef CFG_UART0_FIFO
#endif
#ifdef CFG_DBG_UART1
	#undef CFG_UART1_INTR
	#undef CFG_UART1_DMA
	#undef CFG_UART1_FIFO
#endif

#if defined(CFG_UART0_INTR) || defined(CFG_UART1_INTR)\
	|| defined(CFG_UART2_INTR) || defined(CFG_UART3_INTR)\
	|| defined(CFG_UART4_INTR) || defined(CFG_UART5_INTR)
#define VERIFY_INTR_MODE
#endif

#if defined(CFG_UART0_DMA) || defined(CFG_UART1_DMA)\
	|| defined(CFG_UART2_DMA) || defined(CFG_UART3_DMA)\
	|| defined(CFG_UART4_DMA) || defined(CFG_UART5_DMA)
#define VERIFY_DMA_MODE
#endif

#if defined(CFG_UART0_FIFO) || defined(CFG_UART1_FIFO)\
	|| defined(CFG_UART2_FIFO) || defined(CFG_UART3_FIFO)\
	|| defined(CFG_UART4_FIFO) || defined(CFG_UART5_FIFO)
#define VERIFY_FIFO_MODE
#endif

#if defined(VERIFY_INTR_MODE)
	extern void* XO_MICOM_FuncUseINTR(void* arg);
#elif defined(VERIFY_DMA_MODE)
	extern void* XO_MICOM_FuncUseDMA(void* arg);
#elif defined(VERIFY_FIFO_MODE)
	extern void* XO_MICOM_FuncUseFIFO(void* arg);
#endif



#define UartCommandLen 9

#ifdef CFG_DBG_UART0
	#undef CFG_UART0_INTR
	#undef CFG_UART0_DMA
	#undef CFG_UART0_FIFO
	#undef CFG_UART0_ENABLE
#endif
#ifdef CFG_DBG_UART1
	#undef CFG_UART1_INTR
	#undef CFG_UART1_DMA
	#undef CFG_UART1_FIFO
	#undef CFG_UART1_ENABLE
#endif

#ifdef TEST_ERR_602
extern uint32_t ERR_602_RX;
extern uint32_t ERR_602_RXe;
extern uint32_t ERR_602_TX;
extern uint32_t ERR_602_TXe;
#endif

#define MICOM_INTR_MODE
//#define MICOM_DMA_MODE
//#define MICOM_FIFO_MODE
#if 1
	#define MICOM_PORT       ITP_DEVICE_UART0
	#define MICOM_DEVICE     itpDeviceUart0
	#define MICOM_BAUDRATE   CFG_UART0_BAUDRATE
	#define MICOM_GPIO_RX    CFG_GPIO_UART0_RX
#else
	#define MICOM_PORT       ITP_DEVICE_UART2
	#define MICOM_DEVICE     itpDeviceUart2
	#define MICOM_BAUDRATE   CFG_UART2_BAUDRATE
	#define MICOM_GPIO_RX    CFG_GPIO_UART2_RX
#endif

#if defined(MICOM_INTR_MODE)
	static sem_t UartSemIntr;
#elif defined(MICOM_DMA_MODE)
	static sem_t UartSemDma;
#elif defined(MICOM_FIFO_MODE)
	static sem_t UartSemFifo;
#endif

static pthread_mutex_t push_tx;
uint8_t MICOM_CheckSum(uint8_t *RxData);
void MICOM_TxData(uint8_t *RxData);
uint8_t TempgBuzzerMode=0xFE;


static bool MICOMInited = 0;
static bool MICOMQuit = 0;
static pthread_t MICOMTask;
bool MICOM_RxParsing(void);

extern uint32_t UserPW_Reply_time;	// JPHong 2024-02-23 Main_V13_P21
extern bool M7S1_EnterFlag;
extern uint8_t gGroup;

#if defined(MICOM_INTR_MODE)
static void MICOM_UartCallback(void* arg1, uint32_t arg2)
{
	#if 1  // Turn on  ITP_IOCTL_REG_UART_CB  flag , should using these codes.
		sem_post(&UartSemIntr);
	#else  // Turn on ITP_IOCTL_REG_UART_DEFER_CB flag , should using these codes.
		uint8_t getstr1[256]= { 0x00 };
		uint8_t sendstr1[8] = { 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30 };
		int len = 0;
		static int totalcount = 0;
		len = read(MICOM_PORT,getstr1+totalcount,256);
		totalcount += len;
		printf("MICOM_UartCallback %s\n"getstr1);
		if(totalcount >= UartCommandLen)
		{	  
			write(MICOM_PORT,sendstr1, 8);
			totalcount =0;
			memset(getstr1, 0, 256);
		}
	#endif
}
#endif

#if defined(MICOM_DMA_MODE) || defined(MICOM_FIFO_MODE)
void timer_isr(void* data)
{
    uint32_t timer = (uint32_t)data;

    ithTimerClearIntr(timer);
	#if defined(MICOM_DMA_MODE)
		sem_post(&UartSemDma);
	#elif defined(MICOM_FIFO_MODE)
		sem_post(&UartSemFifo);
	#endif
}

static void UartIntrHandler(unsigned int pin, void *arg)
{
	ITHTimer timer = (ITHTimer)arg;
	//ithPrintf("timer=%d\n", timer);
	ithGpioClearIntr(pin);
	ithTimerCtrlDisable(timer, ITH_TIMER_EN);	
	ithTimerSetTimeout(timer, 1000);//us
	ithTimerCtrlEnable(timer, ITH_TIMER_EN);	
}

/*
static void UartIntrHandler2(void * arg)
{
	ITHUartPort port = (ITHUartPort)arg;
	ithUartClearIntr(port);
	ithPrintf("-- uart handle2 --\n");
}*/

void InitUartIntr(ITHUartPort port)
{
	ithEnterCritical();
#if 1
    ithGpioClearIntr(MICOM_GPIO_RX);
    ithGpioSetIn(MICOM_GPIO_RX);
    ithGpioRegisterIntrHandler(MICOM_GPIO_RX, UartIntrHandler, ITH_TIMER5);
	
    ithGpioCtrlDisable(MICOM_GPIO_RX, ITH_GPIO_INTR_LEVELTRIGGER);   /* use edge trigger mode */
    ithGpioCtrlEnable(MICOM_GPIO_RX, ITH_GPIO_INTR_BOTHEDGE); /* both edge */    
    ithIntrEnableIrq(ITH_INTR_GPIO);
    ithGpioEnableIntr(MICOM_GPIO_RX);
#else
	//printf("port = 0x%x\n", port);
	ithIntrDisableIrq(ITH_INTR_UART2);
    ithUartClearIntr(ITH_UART2);
    ithIntrClearIrq(ITH_INTR_UART2);

    ithIntrSetTriggerModeIrq(ITH_INTR_UART2, ITH_INTR_LEVEL);
    ithIntrRegisterHandlerIrq(ITH_INTR_UART2, UartIntrHandler2, (void *)ITH_UART2);
    ithUartEnableIntr(ITH_UART2, ITH_UART_RX_READY);

    /* Enable the interrupts. */
    ithIntrEnableIrq(ITH_INTR_UART2);
#endif
    ithExitCritical();
}

void InitTimer(ITHTimer timer, ITHIntr intr)
{
	ithTimerReset(timer);
	
	// Initialize Timer IRQ
	ithIntrDisableIrq(intr);
	ithIntrClearIrq(intr);

	// register Timer Handler to IRQ
	ithIntrRegisterHandlerIrq(intr, timer_isr, (void*)timer);

	// set Timer IRQ to edge trigger
	ithIntrSetTriggerModeIrq(intr, ITH_INTR_EDGE);

	// set Timer IRQ to detect rising edge
	ithIntrSetTriggerLevelIrq(intr, ITH_INTR_HIGH_RISING);

	// Enable Timer IRQ
	ithIntrEnableIrq(intr);
}
#endif

bool PushTxData (uint32_t IDenti, uint8_t *TxData,uint8_t Length);

uint32_t CheckTxFlagCnt=0;
void CheckTxFlag(void)		// JPHong 2024-02-26
{
	int i;
	if ((CheckTxFlagCnt+TIME_CNT_500MS)<gMainTimerCnt_10m)
	{
		for (i=0;i<MICOM_TX_DATA_CNT;i++)
		{
			if (MICOM_TX.TxFlag[i]==1)
			{
				printf("\n\nCheckTxFlag : [%d][%d][%d] \n\n",MICOM_TX.Tx_Read,MICOM_TX.Tx_Write,i);
				
				MICOM_TX.Tx_Read = i;
				#ifdef TEST_ERR_602
				ERR_602_TXe++;
				#endif
				break;
			}
		}
		CheckTxFlagCnt=gMainTimerCnt_10m;
	}
}

static void Can0TaskTx()
{
	int i;
	if (ConnectCan>2)
	{

		if (MICOM_TX.TxFlag[MICOM_TX.Tx_Read]==1)
		{
			#ifdef TEST_ERR_602
			{
				// if (AUX_FC1 == CanTxData[CanTxData_Read].IDentifier)
//					 ERR_602_TX++;
			}
			#endif
			
			#if 0
				uint8_t getstr[MICOM_DATA_LENTH+1] = {0x3E,0x18,0x01,0x02,0x03,0x08,0x01,0x02,0x03,0x04,0xF1,0xF2,0xF3,0xF4,0x2F,0x67,0x0d};
				write(MICOM_PORT, &(getstr[0]), MICOM_DATA_LENTH);			
			#else
				write(MICOM_PORT, &(MICOM_TX.TxData[MICOM_TX.Tx_Read][0]), MICOM_DATA_LENTH);
			#endif
			MICOM_TX.TxFlag[MICOM_TX.Tx_Read]=0;

			MICOM_TX.Tx_Read++;
			if (MICOM_TX.Tx_Read >= MICOM_TX_DATA_CNT)
			{
				MICOM_TX.Tx_Read = 0;
			}
			CheckTxFlagCnt=gMainTimerCnt_10m;		// JPHong 2024-02-26
		}
		else
		{
			CheckTxFlag();		// JPHong 2024-02-26
		}
	}
	else
	{
		if (MICOM_TX.Tx_Read!=MICOM_TX.Tx_Write)
		{
			MICOM_TX.TxFlag[MICOM_TX.Tx_Read]=0;
			MICOM_TX.Tx_Read++;
			if (MICOM_TX.Tx_Read >= MICOM_TX_DATA_CNT)
			{
				MICOM_TX.Tx_Read = 0;
			}
		}
	}
//		return NULL;
}

#ifdef TEST_ERR_602
	static pthread_mutex_t TestCan0_Tx;
	uint32_t TestCan0TaskTxCnt = 0;
	uint32_t TestCan0TaskTxFlag = 0;
	uint8_t Txgetstr[MICOM_DATA_LENTH+1] = {0x3E,0x18,0x01,0x02,0x03,0x08,0x01,0x02,0x03,0x04,0xF1,0xF2,0xF3,0xF4,0x2F,0x67,0x0d};
	void TestCan0TaskTx(void)
	{
		int i;
		if (TestCan0TaskTxFlag%2)
		{
			pthread_mutex_lock(&TestCan0_Tx);
			TestCan0TaskTxCnt++;
			ERR_602_TX++;
			Txgetstr[0] = MICOM_STX_AP;
			Txgetstr[1] = 0x10;
			Txgetstr[2] = 0x32;
			Txgetstr[3] = 0x54;
			Txgetstr[4] = 0x76;
			Txgetstr[5] = 8;
			Txgetstr[6] = (TestCan0TaskTxCnt>>0&0xFF);
			Txgetstr[7] = (TestCan0TaskTxCnt>>8&0xFF);
			Txgetstr[8] = (TestCan0TaskTxCnt>>16&0xFF);
			Txgetstr[9] = (TestCan0TaskTxCnt>>24&0xFF);
			Txgetstr[10] = (TestCan0TaskTxCnt>>32&0xFF);
			Txgetstr[11] = 0x7F;
			Txgetstr[12] = 0x8F;
			Txgetstr[13] = 0x9F;
			Txgetstr[14] = MICOM_ETX;
			Txgetstr[15] = MICOM_CheckSum(&(Txgetstr[0]));	
			write(MICOM_PORT, &(Txgetstr[0]), MICOM_DATA_LENTH);
			pthread_mutex_unlock(&TestCan0_Tx);
		}
	}
#endif


bool CanbusTxDataManagement_mutex()
{
	#ifdef TEST_ERR_602
		pthread_mutex_init(&TestCan0_Tx, NULL);
	#endif
	pthread_mutex_init(&push_tx, NULL);
	return 0;
}

extern uint8_t  SandTPDT_Data[8];
	bool CanbusTxDataManagement( uint32_t TimerCnt_10ms, uint32_t ID,uint8_t *TxData)
	{
	
		uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
		
// return 0;		

//		txbuffer[7] = TimerCnt_10ms;
		pthread_mutex_lock(&push_tx);

		Can0TaskTx();
		if (ID!=0)
		{
			if (MicomFirm_UP_Flag!=0)
			{
				if (ID == XO_MICOM_FIRM_UP)
				{
					memcpy(txbuffer, TxData, sizeof(txbuffer));
					PushTxData (ID , txbuffer,8);
				}
				else if (ID == XO_MICOM_FIRM_DATA)
				{
					memcpy(txbuffer, TxData, sizeof(txbuffer));
					PushTxData (ID , txbuffer,8);
				}
			}
			else if (ID == UA1)				// UA1			0x1850E6CE
			{
				txbuffer[0] = (LogoPW_Index<<1)+1;
				txbuffer[1] = (LogoPW_Data>>0)&0xFF;
				txbuffer[2] = (LogoPW_Data>>8)&0xFF;
				txbuffer[3] = (LogoPW_Data>>16)&0xFF;
				txbuffer[4] = (LogoPW_Data>>24)&0xFF;
				txbuffer[5] = 0;
				txbuffer[6] = 0;
				txbuffer[7] = 0;

				PushTxData (ID , txbuffer,8);
			}
			else if (ID == DM15D)	//	0x18D825CE
			{
#ifdef DM14_DBG_MSG				
				printf("DM15_Data.Length= '%d''0x%X''0x%X'\n",DM15_Data.Length,DM15_Data.Length,DM15_Data.Seed);
#endif
				txbuffer[0] = (DM15_Data.Length);
				txbuffer[1] = (((DM15_Data.Length)&0x700)>>3)|((DM15_Data.Status)<<1)|0x11;
				txbuffer[2] = (DM15_Data.EDCP>>0)&0xFF;
				txbuffer[3] = (DM15_Data.EDCP>>8)&0xFF;
				txbuffer[4] = (DM15_Data.EDCP>>16)&0xFF;
				txbuffer[5] = DM15_Data.EDCPEx;
				txbuffer[6] = (DM15_Data.Seed>>0)&0xFF;
				txbuffer[7] = (DM15_Data.Seed>>8)&0xFF;
#ifdef DM14_DBG_MSG				
				printf("[DM15D:0x%X]= 0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\n",DM15D,txbuffer[0],txbuffer[1],txbuffer[2],txbuffer[3],txbuffer[4],txbuffer[5],txbuffer[6],txbuffer[7]);
#endif
//				SDL_Delay(20);
				PushTxData (ID , txbuffer,8);
			}
			else if (ID == TP_CM_D)	//	1CEC25CE  TP_DT_D
			{
#ifdef DM14_DBG_MSG			
//	printf("TP_CM_D = '0x%X''0x%X''0x%X'\n",DM14_Data.Command,TPCM_Ack.Control,TPCM_Ack.Size);
#endif	
				if (DM14_Data.Command==DM14_RX_WRITE)
				{
					if (TPCM_Ack.Control!=0)
					{
						txbuffer[0] = (TPCM_Ack.Control);
						txbuffer[1] = (TPCM_Ack.Size>>0)&0xFF;
						txbuffer[2] = (TPCM_Ack.Size>>8)&0xFF;
						txbuffer[3] = (TPCM_Ack.Packet)&0xFF;
						txbuffer[4] = (TPCM_Cts.SAE)&0xFF;
						txbuffer[5] = (TPCM_Cts.PGN>>0)&0xFF;
						txbuffer[6] = (TPCM_Cts.PGN>>8)&0xFF;
						txbuffer[7] = (TPCM_Cts.PGN>>16)&0xFF;
#ifdef DM14_DBG_MSG				
				printf("[TP_CM_D1:0x%X]= 0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\n",TP_CM_D,txbuffer[0],txbuffer[1],txbuffer[2],txbuffer[3],txbuffer[4],txbuffer[5],txbuffer[6],txbuffer[7]);
#endif						
						PushTxData (ID , txbuffer,8);
					}
					else if (TPCM_Cts.Control!=0)
					{
						txbuffer[0] = (TPCM_Cts.Control);
						txbuffer[1] = (TPCM_Cts.NPS);
						txbuffer[2] = (TPCM_Cts.NPN);
						txbuffer[3] = (TPCM_Cts.SAE>>0)&0xFF;
						txbuffer[4] = (TPCM_Cts.SAE>>8)&0xFF;
						txbuffer[5] = (TPCM_Cts.PGN>>0)&0xFF;
						txbuffer[6] = (TPCM_Cts.PGN>>8)&0xFF;
						txbuffer[7] = (TPCM_Cts.PGN>>16)&0xFF;
#ifdef DM14_DBG_MSG				
				printf("[TP_CM_D2:0x%X]= 0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\n",TP_CM_D,txbuffer[0],txbuffer[1],txbuffer[2],txbuffer[3],txbuffer[4],txbuffer[5],txbuffer[6],txbuffer[7]);
#endif						
						PushTxData (ID , txbuffer,8);
					}
				}
				else if (DM14_Data.Command==DM14_RX_READ)
				{	
					txbuffer[0] = (TPCM_Ack.Control);
					txbuffer[1] = (TPCM_Ack.Size>>0)&0xFF;
					txbuffer[2] = (TPCM_Ack.Size>>8)&0xFF;
					txbuffer[3] = (TPCM_Ack.Packet)&0xFF;
					txbuffer[4] = (TPCM_Ack.SAE)&0xFF;
					txbuffer[5] = (TPCM_Ack.PGN>>0)&0xFF;
					txbuffer[6] = (TPCM_Ack.PGN>>8)&0xFF;
					txbuffer[7] = (TPCM_Ack.PGN>>16)&0xFF;
#ifdef DM14_DBG_MSG
//				printf("[TP_CM_D3:0x%X]= 0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\n",TP_CM_D,txbuffer[0],txbuffer[1],txbuffer[2],txbuffer[3],txbuffer[4],txbuffer[5],txbuffer[6],txbuffer[7]);
#endif					
					PushTxData (ID , txbuffer,8);
				}
			}
			else if (ID == TP_DT_D)	//	1CEB25CE
			{
				memcpy(txbuffer, SandTPDT_Data, sizeof(txbuffer));
#ifdef DM14_DBG_MSG				
				printf("[TP_DT_D:0x%X]= 0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\n",TP_DT_D,txbuffer[0],txbuffer[1],txbuffer[2],txbuffer[3],txbuffer[4],txbuffer[5],txbuffer[6],txbuffer[7]);
#endif				
				PushTxData (ID , txbuffer,8);
			}
			else if (ID == PGN_REQ1)	//	18EAE4CE
			{
				memcpy(txbuffer, TxData, sizeof(txbuffer));
				PushTxData (ID , txbuffer,3);
			}	
			/*
			else if (ID == IMM_I)	//	18502BCE //chpark.24.10.29
			{
				memcpy(txbuffer, TxData, sizeof(txbuffer));
				#ifndef RELEASE_VER				
				printf("[IMM_I:0x%X]= 0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\n",IMM_I,txbuffer[0],txbuffer[1],txbuffer[2],txbuffer[3],txbuffer[4],txbuffer[5],txbuffer[6],txbuffer[7]);
				#endif	
				PushTxData (ID , txbuffer,8);
			}
			*/				
			else	// SOFT_ID
			{
				memcpy(txbuffer, TxData, sizeof(txbuffer));
				PushTxData (ID , txbuffer,8);
			}
		}
		else
		{
			if ((TimerCnt_10ms%50)==0)	// 500ms
			{
				txbuffer[1] = HMV.HourMeter_Trip_Clear1;
				HMV.HourMeter_Trip_Clear1 = 0;
				
				#if 0
					txbuffer[2] = HMV.HourMeter_Trip_Clear2| ((Main_FnWAY&0x01)<<7);
					HMV.HourMeter_Trip_Clear2 = 0;
					txbuffer[3] = (gRead_Gpio_Value&0xC0) | (Main_FnECO&0x01);
					txbuffer[4] = (gRead_Gpio_Value&0x3F) | (Func_APC_Flag<<6);
				#else
					txbuffer[2] = HMV.HourMeter_Trip_Clear2;
					HMV.HourMeter_Trip_Clear2 = 0;

					txbuffer[2] &= 0xBF;
					if (g_nM7_StartType==M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
						txbuffer[2] |= (uint8_t)((Option_Function.AutoStop[M7_UserIndex]&0x01)<<6);
					else
						txbuffer[2] |= (uint8_t)((Option_Function.AutoStopKey&0x01)<<6);
					
					txbuffer[3] = (gRead_Gpio_Value&0xC0);
					txbuffer[4] = (gRead_Gpio_Value&0x3F);

					//chpark.24.11.11
					if(g_bPW100_PW_OnOff == PW100_PASS_MODE_ON)
					{
						txbuffer[5] = g_PW100_Password_5Byte;
						txbuffer[6] = g_PW100_Password_6Byte;
					}
					else
					{
						txbuffer[5] = 0x00;
						txbuffer[6] = 0x00;
					}
				#endif
			
				PushTxData (CLI , txbuffer,8);
			}

			if ((TimerCnt_10ms%6)==0)	// 50ms
			{
				if ((Main_Disp_Mode==MAIN_MODE_DAB2)||(Main_Disp_Mode==MAIN_MODE_DAB3))
				{
					txbuffer[0] = MAIN_MODE_DAB1;
				}
				else
				{
					txbuffer[0] = Main_Disp_Mode;
				}
				if ((RotationDir()==LANDSCAPE_1)||(RotationDir()==LANDSCAPE_2))
				{
					txbuffer[0] = txbuffer[0]|0x10;
				}
				else
				{
					txbuffer[0] = txbuffer[0]&0xEF;
				}
			
				txbuffer[1] = Main_CLI2[1];
				txbuffer[2] = Main_CLI2[2];
				txbuffer[3] = Main_CLI2[3];
				txbuffer[4] = Main_CLI2[4];
				txbuffer[5] = Main_CLI2[5];
				txbuffer[6] = Main_CLI2[6];
				txbuffer[7] = Main_CLI2[7];
				
				// add 0608 jhlee
				uint8_t tD7 = 0;

				if(S7_ChoiceRegion == 1)
					tD7 = 0x02;			// japan on
				else if(S7_ChoiceRegion == 2)
					tD7 = 0x04;			// north america on
				else if(S7_ChoiceRegion == 3)
					tD7 = 0x08;			// europe on
				else if(S7_ChoiceRegion == 4)
					tD7 = 0x10;			// latin america on
				else if(S7_ChoiceRegion == 0)
					tD7 = 0x00;			// off

				txbuffer[7] = (txbuffer[7]&0xE1)|tD7;
				// add end
					
				PushTxData (CLI2 , txbuffer,8);		//0x185051CE
#if 1				
				if ((AC_SEL_DIR_LR_FLAG==2)||(AC_SEL_TEMP_LR_FLAG==2)||(AC_SEL_LEVEL_LR_FLAG==2))
				{
					Main_CLI2[2] = 0;
					if (AC_SEL_DIR_LR_FLAG==2)
						AC_SEL_DIR_LR_FLAG=1;
					if (AC_SEL_TEMP_LR_FLAG==2)
						AC_SEL_TEMP_LR_FLAG=1;
					if (AC_SEL_LEVEL_LR_FLAG==2)
						AC_SEL_LEVEL_LR_FLAG=1;
				}
				else
#endif				
				{
					Main_CLI2[2] = Main_CLI2[2] & 0x3C;	// 0x3F JPHong 2024-01-17
				}
#if 1			
//					printf("JOG_DIAL_KEY_ENTER_OFF1= '%d' '%d' '%d' '0x%02X%d'\n",RD_SEL_TUNE_LR_FLAG,RD_SEL_TONE_DU_FLAG,RD_SEL_VOL_DU_FLAG,Main_CLI2[3]);
	
				if ((RD_SEL_TUNE_LR_FLAG==2)||(RD_SEL_TONE_DU_FLAG==2)||(RD_SEL_VOL_DU_FLAG==2))
				{
//					printf("JOG_DIAL_KEY_ENTER_OFF= '%d' '%d' '%d' '0x%02X%d'\n",RD_SEL_TUNE_LR_FLAG,RD_SEL_TONE_DU_FLAG,RD_SEL_VOL_DU_FLAG,Main_CLI2[3]);
					Main_CLI2[3] = 0;
					Main_CLI2[4] = 0;
					if (RD_SEL_TUNE_LR_FLAG==2)
						RD_SEL_TUNE_LR_FLAG=1;
					if (RD_SEL_TONE_DU_FLAG==2)
						RD_SEL_TONE_DU_FLAG=1;
					if (RD_SEL_VOL_DU_FLAG==2)
						RD_SEL_VOL_DU_FLAG=1;
				}
				else
#endif				
				{
					if (Main_Disp_Mode == MAIN_MODE_DAB1)
					{
						Main_CLI2[3] = Main_CLI2[3] & 0xCC;
					}
					else if ((Main_Disp_Mode == MAIN_MODE_DAB2)||(Main_Disp_Mode == MAIN_MODE_DAB3))
					{
						Main_CLI2[3] = Main_CLI2[3] & 0xC0;
					}
					else if (Main_Disp_Mode == MAIN_MODE_BLUETOOTH)
					{
						Main_CLI2[3] = Main_CLI2[3] & 0xD0;
					}
					else
					{
						Main_CLI2[3] = Main_CLI2[3] & 0xDC;
					}
					Main_CLI2[4] = Main_CLI2[4] & 0xE3;
				}
//					printf("JOG_DIAL_KEY_ENTER_OFF2= '%d' '%d' '%d' '0x%02X%d'\n",RD_SEL_TUNE_LR_FLAG,RD_SEL_TONE_DU_FLAG,RD_SEL_VOL_DU_FLAG,Main_CLI2[3]);
	
				Main_CLI2[5] = Main_CLI2[5] & 0x1F;
				Main_CLI2[6] = Main_CLI2[6] & 0x04;
				Main_CLI2[7] = Main_CLI2[7] & 0xFE;
				
			}

			if (((TimerCnt_10ms+3)%6)==0)	// 50ms
			{
				txbuffer[0] = Main_CLI3[0];
				txbuffer[1] = Main_CLI3[1];
				txbuffer[2] = Main_CLI3[2];
				txbuffer[3] = Main_CLI3[3];
				txbuffer[4] = Main_CLI3[4];
				txbuffer[5] = Main_CLI3[5];
				txbuffer[6] = Main_CLI3[6];
				txbuffer[7] = Main_CLI3[7];
		
				PushTxData (CLI3 , txbuffer,8);		//0x185053CE
//				Main_CLI3[0] = 0;
				Main_CLI3[3] = Main_CLI3[3] & 0xF7;
				Main_CLI3[4] = Main_CLI3[4] & 0xFC;
				Main_CLI3[5] = 0;
				Main_CLI3[6] = 0;
				Main_CLI3[7] = Main_CLI3[7] & 0x01;	// 2024-02-24 JPHong Main V13P31
			}
			
			if(((TimerCnt_10ms+4)%6)==0)	// 50ms
			{
				txbuffer[5] &= 0xFE;  
				txbuffer[5] |= (Option_Function.Rotate_Flag&0x01);
				
				PushTxData (CLI4 , txbuffer,8);		//0x185057CE
			}
			
			if (((TimerCnt_10ms+15)%100)==0)	// 1000ms	//0x18500ECE
			{		
				uint8_t  txbuf_FC1[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

				txbuf_FC1[0] = M7_UserSet[0].A_Value;    // AUX1Val.Pre[M7_UserIndex].Percent1st[0];	// JPHong Menu V13
				txbuf_FC1[1] = M7_UserSet[0].B_Value;    // AUX1Val.Pre[M7_UserIndex].Percent1st[1];
				txbuf_FC1[2] = M7_UserSet[1].A_Value;    // AUX1Val.Pre[M7_UserIndex].Percent1st[2];
				txbuf_FC1[3] = M7_UserSet[1].B_Value;    // AUX1Val.Pre[M7_UserIndex].Percent1st[3];
				txbuf_FC1[4] = M7_UserSet[2].A_Value;    // AUX1Val.Pre[M7_UserIndex].Percent1st[4];
				txbuf_FC1[5] = M7_UserSet[2].B_Value;    // AUX1Val.Pre[M7_UserIndex].Percent1st[5];
				
				if((M7_Way_OPI_Flag == 1)||(M7_Way_OPI_Flag == 2))
				{
					//chpark.24.09.19
					//
					/*
					if (M7S1_EnterFlag)
						txbuf_FC1[6] = (M7_UserSet[gGroup].Way_Opt&0x03)<<6;
					else
						txbuf_FC1[6] = (M7_UserSet[AUX1CurrentOutMode].Way_Opt&0x03)<<6; 
					*/
					txbuf_FC1[6] = (M7_UserSet[AUX1CurrentOutMode].Way_Opt&0x03)<<6; 
				}
				else if((M7_Way_OPI_Flag == 0)||(M7_Way_OPI_Flag == 3))
				{
					txbuf_FC1[6] = 0;
				}
				txbuf_FC1[7] = (AUX1CurrentOutMode+1)&0x0F;
				PushTxData (AUX_FC1 , txbuf_FC1,8);
			}

			// add jhlee 230530
			if (((TimerCnt_10ms+16)%100)==0)	// 1000ms	//CAN Tx[AUX_PC1] 185016CE 
			{		
				uint8_t  txbuf_PC1[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
				
				txbuf_PC1[0] = M7_UserSet[0].P1_Value;    // Users No1 AUX1 P1 value
				txbuf_PC1[1] = 0;
				txbuf_PC1[2] = M7_UserSet[1].P1_Value;    // Users No2 AUX1 P1 value
				txbuf_PC1[3] = 0;
				txbuf_PC1[4] = M7_UserSet[2].P1_Value;    // Users No3 AUX1 P1 value
				txbuf_PC1[5] = 0;

				txbuf_PC1[6] = 0;
				txbuf_PC1[7] = 0;

				PushTxData (AUX_PC1,txbuf_PC1,8);
			}

			// add jhlee 230531
			if (((TimerCnt_10ms+17)%100)==0)	// 1000ms	//CAN Tx[AUX_FC2] 18500FCE
			{		
				uint8_t  txbuf_FC2[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

				txbuf_FC2[0] = M7_UserAUX24Set.C_Value;    // Users AUX2 C value
				txbuf_FC2[1] = M7_UserAUX24Set.D_Value;    // Users AUX2 D value
				txbuf_FC2[2] = M7_UserAUX24Set.G_Value;    // Users AUX4 G value
				txbuf_FC2[3] = M7_UserAUX24Set.H_Value;    // Users AUX4 H value
				txbuf_FC2[4] = 0;
				txbuf_FC2[5] = 0;
				txbuf_FC2[6] = 0;
				txbuf_FC2[7] = 0;

				PushTxData (AUX_FC2 , txbuf_FC2,8);
			}

			// add jhlee 230531
			if(((TimerCnt_10ms+18)%100)==0)	// 1000ms	//CAN Tx[AUX_PC2] 185017CE 
			{
				uint8_t  txbuf_PC2[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

				txbuf_PC2[0] = M7_UserAUX24Set.P2_Value;    // Users AUX2 P2 value
				txbuf_PC2[1] = 0;
				txbuf_PC2[2] = M7_UserAUX24Set.P4_Value;    // Users AUX4 P4 value
				txbuf_PC2[3] = 0;
				txbuf_PC2[4] = 0;
				txbuf_PC2[5] = 0;
				txbuf_PC2[6] = 0;
				txbuf_PC2[7] = 0;

				PushTxData (AUX_PC2 , txbuf_PC2,8);
			}

			if(((TimerCnt_10ms+19)%100)==0)			//	1C504BCE , add 230705 jhlee
			{
				uint8_t  txbuf_AG_S[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
				
				txbuf_AG_S[2] = AG_Time;

				PushTxData(AG_S,txbuf_AG_S,8);
			}

			if(((TimerCnt_10ms+1)%50)==0)				//	1C504CCE , add 230705 jhlee
			{
				uint8_t  txbuf_AG_ST[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

				txbuf_AG_ST[0] = (uint8_t)(AG_Start | (AG_Stop<<1));
				AG_Start = 0;
				AG_Stop = 0;

				PushTxData (AG_ST,txbuf_AG_ST,8);	
			}


		}
#if 1		
		Can0TaskTx();
//		Can0TaskTx();
#endif
		pthread_mutex_unlock(&push_tx);
		return 1;
	}


bool UART_FULL_Flag =  0;
extern bool Finish_LogoBoot;

#define UartDataBuff	2000


uint8_t ReturnMode4CAN(uint32_t CAN_ID)
{
	switch (Main_Disp_Mode)
	{
		
		case MAIN_MODE_USERINFO:
		case MAIN_MODE_BACK:
		case MAIN_MODE_MENU:
		{
			#ifdef RELEASE_VER
				if (CAN_ID != JOG2ECU)	//18FF603B
					return 1;
			#else
				return 1;
			#endif
		}
		break;
		case MAIN_MODE_PW:			// JPHong 2023-01-27
			switch (CAN_ID)
			{
				#ifndef RELEASE_VER
				case JOG2ECU:	//18FF603B
				#endif
				case CO:		// 185050E4
				case MI:		// 185000E4
				case OPI2:		// 185038E4
				case OPI3:		// 185039E4
				case AUI1:		// 18510E8C	JPHong 0606
				case UI1:		// 18511D8C	
				case UA2:		// 1850E78C				
				case DM14T:		// 18D9CE25
				case DM16T:		// 18D7CE25
				case TP_CM_T:	// 1CECCE25
				case TP_DT_T:	// 1CEBCE25
				case GFI:		// 1850EEE4
				
				case TCI:		// 1850F1E4
				
				case XO_MICOM_SOFT_REQ:
				case XO_MICOM_FIRM_UP:
				case XO_MICOM_FIRM_DATA:

				case XO_TMODE_REQ: //121200AC //chpark.24.05.27
				case XO_TMODE_ACK: //131300AC //chpark.24.05.27

				case IMM_A: //chpark.24.08.29
				case IMM_R: //chpark.24.08.29
					return 1;
				break;
			}		
		break;
		default:	//Main
			switch (CAN_ID)
			{
				case CLM1:		// 14FF00E4
				case CLM2:		// 14500EE4	// LCD ����ȭ�鿡�� �ְ� �����?�������� ���� ���� , LCD �ְ� ���?
				case CLM3:		// 14500CE4	
				case CLM4:		// 14500AE4
				case AUX_IO2:
				#ifndef RELEASE_VER
				case JOG2ECU:	//18FF603B
				#endif
				case CO:		// 185050E4
				case EI:		// 185009E4
				case AT1T1I:	// 18FE563D
				case DD:		// 18FEFCE4
				case SI:		// 18500DE4
				case EEC1:		// 0CF00400
				case ET1:		// 18FEEE00
				case ACAS:		// 18FFA019
				case HM:		// 185001E4
				case TM1:		// 185002E4
				case TM2:		// 185003E4
				case TM3:		// 185008E4
				case RI1:		// 18FF704C
				case RI2:		// 18FF714C
				case RI3:		// 18FF724C
				case RI4:		// 18FF734C
				case RI5:		// 18FF744C

				case DM1a:		// 18FECA00		// add 230601 jhlee
				case DM1b:		// 18FECA3D
				case TP_DTa:	// 1CEBFF00
				case TP_DTb:	// 1CEBFF3D
				case TP_CMa:	// 1CECFF00
				case TP_CMb:	// 1CECFF3D

				case DM1aY:		// 1CFECA00
				case DM1bY:		// 1CFECA3D
				case TP_DTaY:	// 18EBFF00
				case TP_DTbY:	// 18EBFF3D
				case TP_CMaY:	// 18ECFF00
				case TP_CMbY:	// 18ECFF3D

				case TP_CM_PGN:	// 18ECFFE4
				case TP_DT_PGN:	// 18EBFFE4

				case DMM:		// 18FF01E4
				case P_DTC:		// 18FF04E4
				case ACDM:		// 18FFA119		// add end jhlee

				case DM2a:		// 1CFECB00		// add 230626 jhlee
				case DM2b:		// 1CFECB3D
				case DM2aY:		// 18FECB00
				case DM2bY:		// 18FECB3D		// add end 230626 jhlee

				case AUX_UL:	// 1C501FE4		// add 230607 jhlee
				case AUX_PUL:	// 1C5025E4
				case AUX_UL5:	// 1C5019E4
				case AUX_UL4:	// 1C5018E4		// add end jhlee 230607

				case MI:		// 185000E4
				case OPI2:		// 185038E4
				case OPI:		// 185012E4	JPHong 0619
				case CPI:		// 18FF811C	JPHong 0528
				case AUI1:		// 18510E8C	JPHong 0606

				case AG_I:		// 1C504AE4 	//	add jhlee 230706
				case UI1:		// 18511D8C
				case UA2:		// 1850E78C
				case UI2:		// 18511E8C//chpark.08.22
				case UI3:		// 18511F8C//chpark.08.22
				case UI4:		// 1851208C//chpark.08.22
				case UI5:		// 1851218C//chpark.08.22
				
				case UM1:		// 18511ACE//chpark.08.22
				case UM2:		// 18511BCE//chpark.08.22
				case UM5:		// 18511C8C//chpark.08.22

				case AT1S:		// 18FD7B00		// add jhlee 231027
				case UWH:		// 1850EDE4		// add jhlee 231207
				case FNI1:		// 1850E8E4
				
				case DM14T:		// 18D9CE25
				case DM16T:		// 18D7CE25
				case TP_CM_T:	// 1CECCE25
				case TP_DT_T:	// 1CEBCE25
				case GFI:		// 1850EEE4
				case XO_MICOM_SOFT_REQ:
				case XO_MICOM_FIRM_UP:
				case XO_MICOM_FIRM_DATA:

				case XO_TMODE_REQ: //121200AC //chpark.24.05.27
				case XO_TMODE_ACK: //131300AC //chpark.24.05.27

				case IMM_A: //chpark.24.08.29
				case IMM_R: //chpark.24.08.29
				case TCI:		// 1850F1E4

					return 1;
				break;
				case 0x00000000:	//00000000
//					printf("CNT_RI1:%d , CNT_RI2:%d , CNT_RI3:%d , CNT_RI4:%d , CNT_RI5:%d , SUM:%d , UART_CNT:%d\n",CNT_RI1,CNT_RI2,CNT_RI3,CNT_RI4,CNT_RI5,(MICOM_RX.Rx_Write),UART_CNT);
				break;
				default:
					if ((CAN_ID&0xFFFFFF00)==SOFT_REQ)
						return 1;
				break;
			}
		break;
	}
	return 0;
}

				
uint8_t ReturnMode4CAN_Rot(uint32_t CAN_ID)
{
	switch (CAN_ID)
	{
		case RI1:	// 18FF704C
		case RI2:	// 18FF714C
		case RI3:	// 18FF724C
		case RI4:	// 18FF734C
		case RI5:	// 18FF744C

		case DM1a:		// 18FECA00		// add 230601 jhlee
		case DM1b:		// 18FECA3D
		case TP_DTa:	// 1CEBFF00
		case TP_DTb:	// 1CEBFF3D
		case TP_CMa:	// 1CECFF00
		case TP_CMb:	// 1CECFF3D

		case DM1aY:		// 1CFECA00
		case DM1bY:		// 1CFECA3D
		case TP_DTaY:	// 18EBFF00
		case TP_DTbY:	// 18EBFF3D
		case TP_CMaY:	// 18ECFF00
		case TP_CMbY:	// 18ECFF3D

		case TP_CM_PGN:		// 18ECFFE4
		case TP_DT_PGN:		// 18EBFFE4

		case DMM:			// 18FF01E4
		case P_DTC:		// 18FF04E4
		case ACDM:		// 18FFA119		// add end jhlee

		case DM2a:		// 1CFECB00		// add 230626 jhlee
		case DM2b:		// 1CFECB3D
		case DM2aY:		// 18FECB00
		case DM2bY:		// 18FECB3D		// add end 230626 jhlee
			return 1;
		break;
	}
	return 0;
}

#if defined(MICOM_INTR_MODE)
void* XO_MICOM_FuncUseINTR(void* arg)
{
	uint8_t getstr[UartDataBuff] = {0,};
	int len = 0, count = 0, i=0,j;
	static uint8_t StartByteFlag =  0;
	static uint16_t StartByteCnt =  0;
	uint8_t CS_DATA=0;
	uint32_t CAN_ID=0;
	
	printf("Start uart XO_MICOM\n");

	itpRegisterDevice(MICOM_PORT, &MICOM_DEVICE);
	ioctl(MICOM_PORT, ITP_IOCTL_RESET, (void*)MICOM_BAUDRATE);
	ioctl(MICOM_PORT, ITP_IOCTL_REG_UART_CB, (void*)MICOM_UartCallback);
	//ioctl(MICOM_PORT, ITP_IOCTL_REG_UART_DEFER_CB , (void*)UartCallback);

	sem_init(&UartSemIntr, 0, 0);
	printf("Start XO_MICOM_FuncUseINTR\n");	
	while (1)
	{
		sem_wait(&UartSemIntr);
        if (MICOMQuit)
            break;
		len = read(MICOM_PORT, getstr + count, (UartDataBuff-count));
		
		#ifdef TEST_ERR_602
		{
			static uint32_t TTT =  0;
			TTT=(len+TTT);
			
		//	ERR_602_TX=TTT/MICOM_DATA_LENTH;
		}
		#endif		

		if (Finish_LogoBoot==1)
		{
	//		printf("len = %d, getstr = %s\n", len, getstr);
			count += len;
			do
			{
				#if 0
					for (j=0;j<(count-StartByteCnt);j++)
					{
						printf("0x%02X ", getstr[StartByteCnt+j]);
					}
					printf("\n");
					printf("%d,%d,%d\n", count,StartByteCnt,len);
				#endif	
				if (StartByteFlag==0)
				{
					for (i=0;i<(count-StartByteCnt);i++)
					{
						if (getstr[StartByteCnt+i]==MICOM_STX_MICOM)
						{
							StartByteFlag = 1;
							StartByteCnt = StartByteCnt+i;
							break;
						}
						else
						{
							#ifdef TEST_ERR_602
							ERR_602_RXe++;
							#endif
							#if 0
								for (j=0;j<(count-StartByteCnt);j++)
								{
									printf("0x%02X ", getstr[StartByteCnt+j]);
								}
								printf("\n");
								printf("%d,%d\n", count,StartByteCnt);
							#endif						
						}
					}
				}
				if (StartByteFlag==1)
				{
					//printf("count:%d,StartByteCnt:%d\n", count,StartByteCnt);
					if (count >= (MICOM_DATA_LENTH+StartByteCnt))
					{
						//printf("MICOM_ETX 0x%X 0x%X ,%d , 0x%X\n", getstr[StartByteCnt],getstr[StartByteCnt+MICOM_DATA_LENTH-2],StartByteCnt+MICOM_DATA_LENTH-1, MICOM_ETX);
						if (getstr[StartByteCnt+MICOM_DATA_LENTH-2]==MICOM_ETX)
						{
							#ifdef TEST_ERR_602
							ERR_602_RX++;
							#endif					
							if (MICOM_RX.RxFlag[MICOM_RX.Rx_Write]==1)
							{
								int i=0;
								printf("MICOM : Buffer Full: %d,%d\n",MICOM_RX.Rx_Write,MICOM_RX.Rx_Read);
								#ifdef TEST_ERR_602
								ERR_602_RXe++;
								#endif								
	
								if (MainLayerFlag==0)
								{
									XO_MICOM_Mem_Reset();
								}
								else
								{
									for (i=0;i<MICOM_RX_DATA_CNT/2;i++)
										MICOM_RxParsing();
								}
								count = 0;
								StartByteCnt = 0;
								StartByteFlag=0;
								memset(getstr, 0, UartDataBuff);									
							}
							else
							{
								memcpy(&(MICOM_RX.RxData[MICOM_RX.Rx_Write][0]), &getstr[StartByteCnt], MICOM_DATA_LENTH);
								//printf("uart read: %s,count=%d %d\n", &(MICOM_RX.RxData[MICOM_RX.Rx_Write][0]), count,StartByteCnt+MICOM_DATA_LENTH);
								CS_DATA = MICOM_CheckSum(&(MICOM_RX.RxData[MICOM_RX.Rx_Write][0]));
	//							printf("uart read:[0x%08X],[%d],0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\n",CS_DATA,MICOM_RX.RxData[MICOM_RX.Rx_Write][5],MICOM_RX.RxData[MICOM_RX.Rx_Write][6],MICOM_RX.RxData[MICOM_RX.Rx_Write][7],MICOM_RX.RxData[MICOM_RX.Rx_Write][8],MICOM_RX.RxData[MICOM_RX.Rx_Write][9],MICOM_RX.RxData[MICOM_RX.Rx_Write][10]);

								if (CS_DATA==MICOM_RX.RxData[MICOM_RX.Rx_Write][MICOM_DATA_LENTH-1])
								{
									CAN_ID = (MICOM_RX.RxData[MICOM_RX.Rx_Write][1]<<24)|(MICOM_RX.RxData[MICOM_RX.Rx_Write][2]<<16)|(MICOM_RX.RxData[MICOM_RX.Rx_Write][3]<<8)|(MICOM_RX.RxData[MICOM_RX.Rx_Write][4]<<0);
									if (ReturnMode4CAN(CAN_ID))
									{
										MICOM_RX.RxFlag[MICOM_RX.Rx_Write] = 1;
										MICOM_RX.Rx_Write++;
										if (MICOM_RX.Rx_Write >= MICOM_RX_DATA_CNT)
										{
											MICOM_RX.Rx_Write = 0;
										}
										if (MICOM_RX.Rx_Read>MICOM_RX.Rx_Write)
										{
											if ((MICOM_RX.Rx_Read-MICOM_RX.Rx_Write)<(MICOM_RX_DATA_OVF))
											{
												#ifdef TEST_ERR_602
												ERR_602_RXe++;
												#endif										
												UART_FULL_Flag =  1;
												printf("MICOM : Buffer Full 1: %d,%d\n",MICOM_RX.Rx_Write,MICOM_RX.Rx_Read);
											}
										}
										else
										{
											if ((MICOM_RX.Rx_Write-MICOM_RX.Rx_Read)>(MICOM_RX_DATA_CNT-MICOM_RX_DATA_OVF))
											{
												#ifdef TEST_ERR_602
												ERR_602_RXe++;
												#endif	
												UART_FULL_Flag =  1;
												printf("MICOM : Buffer Full 2: %d,%d\n",MICOM_RX.Rx_Write,MICOM_RX.Rx_Read);
											}
										}
									}
									StartByteFlag = 0;
									if (count>=(UartDataBuff-MICOM_DATA_LENTH))
									{
										#ifdef TEST_ERR_602
										ERR_602_RXe++;
										#endif
										printf("MICOM : Overflow: %d,%d\n",count,StartByteCnt);
										count = 0;
										StartByteCnt = 0;
										StartByteFlag=0;
										memset(getstr, 0, UartDataBuff);
									}
									else if (count==(StartByteCnt+MICOM_DATA_LENTH))
									{
										count = 0;
										StartByteCnt = 0;
//										memset(getstr, 0, UartDataBuff);
									}
									else
									{
										StartByteCnt = StartByteCnt+MICOM_DATA_LENTH;						
									}
								}
								else
								{
									#ifdef TEST_ERR_602
									ERR_602_RXe++;
									#endif
									printf("MICOM : CheckSum Error : 0x%X , 0x%X\n",CS_DATA,MICOM_RX.RxData[MICOM_RX.Rx_Write][MICOM_DATA_LENTH-1]);
									count = 0;
									StartByteCnt = 0;
									StartByteFlag=0;
									memset(getstr, 0, UartDataBuff);
								}
							}
						}
						else
						{
							#ifdef TEST_ERR_602
							ERR_602_RXe++;
							#endif
							printf("MICOM : Not Found ETX Data:%d 0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X \n",StartByteCnt,getstr[StartByteCnt+8],getstr[StartByteCnt+9],getstr[StartByteCnt+10],getstr[StartByteCnt+11],getstr[StartByteCnt+12],getstr[StartByteCnt+13],getstr[StartByteCnt+14],getstr[StartByteCnt+15]);
							count = 0;
							StartByteCnt = 0;
							StartByteFlag=0;
							memset(getstr, 0, UartDataBuff);
						}
					}	//if (count >= (MICOM_DATA_LENTH+StartByteCnt))
				}	//if (StartByteFlag==1)
			}while (count >= (MICOM_DATA_LENTH+StartByteCnt));
		}	//if (Finish_LogoBoot==1)
        if (MICOMQuit)
            break;
	}
}
#elif defined(MICOM_DMA_MODE)
void* XO_MICOM_FuncUseDMA(void* arg)
{
	int len = 0, count = 0;
	char getstr[256] = "", sendstr[256] = "";
	
	itpRegisterDevice(MICOM_PORT, &MICOM_DEVICE);
	ioctl(MICOM_PORT, ITP_IOCTL_RESET, (void*)MICOM_BAUDRATE);
	
	InitTimer(ITH_TIMER5, ITH_INTR_TIMER5);
	InitUartIntr(MICOM_PORT);

	printf("Start uart DMA mode test!\n");

	sem_init(&UartSemDma, 0, 0);

	while(1)
	{
		sem_wait(&UartSemDma);
		
		len = read(MICOM_PORT, getstr + count, 256);
		printf("len = %d, getstr = %s\n", len, getstr);
	    count += len;

	    if(count >= UartCommandLen)
	    {
			printf("uart read: %s,count=%d\n", getstr, count);
			memcpy(sendstr, getstr, count);
			sendstr[count] = '\n';
			write(MICOM_PORT, sendstr, count + 1);

			memset(getstr, 0, count + 1);
			memset(sendstr, 0, count + 1);
			count = 0;
	    }
	}
}
#elif defined(MICOM_FIFO_MODE)
void* XO_MICOM_FuncUseFIFO(void* arg)
{
	int len = 0, count = 0;
	char getstr[256] = "", sendstr[256] = "";

	itpRegisterDevice(MICOM_PORT, &MICOM_DEVICE);
	ioctl(MICOM_PORT, ITP_IOCTL_RESET, (void*)MICOM_BAUDRATE);

	InitTimer(ITH_TIMER5, ITH_INTR_TIMER5);
	InitUartIntr(MICOM_PORT);

	printf("Start uart FIFO mode test!\n");

	sem_init(&UartSemFifo, 0, 0);

	while (1)
	{
		sem_wait(&UartSemFifo);

		// 9860 FIFO buffer maximum depth : 128
		len = read(MICOM_PORT, getstr + count, 128);
		printf("len = %d, getstr = %s\n", len, getstr);
		count += len;

		if (count >= UartCommandLen)
		{
			printf("uart read: %s,count=%d\n", getstr, count);
			memcpy(sendstr, getstr, count);
			sendstr[count] = '\n';
			write(MICOM_PORT, sendstr, count+1);

			memset(getstr, 0, count + 1);
			memset(sendstr, 0, count + 1);
			count = 0;
		}
	}
}
#endif


bool PushTxData_1 (uint32_t IDenti, uint8_t *TxData,uint8_t Length)
{
	static int i=0;

	if (ConnectCan<=2)
		return 0;
	if (MICOM_TX.TxFlag[MICOM_TX.Tx_Write]==1)
	{
		
		{
			i++;
			printf("Tx CanTxData_Write:%d , CanTxData_Read:%d , i:%d\n",MICOM_TX.Tx_Write,MICOM_TX.Tx_Read,i);
			if (i>10)
			{
				if (ConnectCan>2)
				{
		
				}
				i=0;
			}
		} 
		if (MICOM_TX.Tx_Write == 0)
		{
			MICOM_TX.Tx_Write = MICOM_TX_DATA_CNT;
		}
		MICOM_TX.Tx_Write--;		
	}
	MICOM_TX.TxData[MICOM_TX.Tx_Write][0] = MICOM_STX_AP;
	
	MICOM_TX.TxData[MICOM_TX.Tx_Write][1] = (IDenti>>24)&0xFF;
	MICOM_TX.TxData[MICOM_TX.Tx_Write][2] = (IDenti>>16)&0xFF;
	MICOM_TX.TxData[MICOM_TX.Tx_Write][3] = (IDenti>>8)&0xFF;
	MICOM_TX.TxData[MICOM_TX.Tx_Write][4] = (IDenti>>0)&0xFF;
	MICOM_TX.TxData[MICOM_TX.Tx_Write][5] = Length;
	memcpy (&(MICOM_TX.TxData[MICOM_TX.Tx_Write][6]),&(TxData[0]),0x08);
	MICOM_TX.TxData[MICOM_TX.Tx_Write][14] = MICOM_ETX;
	MICOM_TX.TxData[MICOM_TX.Tx_Write][15] = MICOM_CheckSum(&(MICOM_TX.TxData[MICOM_TX.Tx_Write][0]));

	MICOM_TX.TxFlag[MICOM_TX.Tx_Write]=1;

	
//	printf("PushTxData  0x%X 0x%X\n",IDenti,CanTxData[CanTxData_Write].IDentifier);
	
	#if 0
		CanTxData_Write = ++ CanTxData_Write % CAN_TX_DATA_CNT;
	#else
		MICOM_TX.Tx_Write++;
		if (MICOM_TX.Tx_Write >= MICOM_TX_DATA_CNT)
		{
			MICOM_TX.Tx_Write = 0;
		}
	#endif
	

	return 1;
}

bool PushTxData (uint32_t IDenti, uint8_t *TxData,uint8_t Length)
{
	PushTxData_1 ( IDenti, TxData, Length);
	#ifdef TEST_ERR_602
	PushTxData_1 ( IDenti, TxData, Length);
	#endif	
	return 1;
}


void XO_MICOM_EXIT(void)
{
    if (!MICOMInited)
        return;

    MICOMQuit = true;

#if defined(VERIFY_INTR_MODE)
	sem_post(&UartSemIntr);
	pthread_join(MICOMTask, NULL);
#elif defined(VERIFY_DMA_MODE)
	sem_post(&UartSemDma);
	pthread_join(MICOMTask, NULL);
#elif defined(VERIFY_FIFO_MODE)
	sem_post(&UartSemFifo);
	pthread_join(MICOMTask, NULL);
#endif
}

int XO_MICOM_Mem_Reset(void)
{

	
	int i;
	for (i=0;i<MICOM_RX_DATA_CNT;i++)
	{
		memset(&(MICOM_RX.RxData[i][0]),0,MICOM_DATA_LENTH);
		MICOM_RX.RxFlag[i] = 0;
	}
	MICOM_RX.Rx_Read = 0;
	MICOM_RX.Rx_Write = 0;
	
	for (i=0;i<MICOM_TX_DATA_CNT;i++)
	{
		memset(&(MICOM_TX.TxData[i][0]),0,MICOM_DATA_LENTH);
		MICOM_TX.TxFlag[i] = 0;
	}
	MICOM_TX.Tx_Read = 0;
	MICOM_TX.Tx_Write = 0;

    return true;
}
int XO_MICOM_INT(void)
{
    pthread_attr_t attr;
	struct sched_param param;

	MICOMInited = 0;
	MICOMQuit = 0;
    pthread_attr_init(&attr);
    attr.stacksize = MICOM_STACK_SIZE;
printf("Start XO_MICOM_INT\n");	
	XO_MICOM_Mem_Reset();
	
#if 0  // Micom uart Test
	itpRegisterDevice(MICOM_PORT, &MICOM_DEVICE);
	ioctl(MICOM_PORT, ITP_IOCTL_INIT, NULL);
	ioctl(MICOM_PORT, ITP_IOCTL_RESET, MICOM_BAUDRATE); 
	
	itpRegisterDevice(MICOM_PORT, &MICOM_DEVICE);
	ioctl(MICOM_PORT, ITP_IOCTL_INIT, NULL);
	ioctl(MICOM_PORT, ITP_IOCTL_RESET, MICOM_BAUDRATE); 
	itpRegisterDevice(MICOM_PORT, &MICOM_DEVICE);
	ioctl(MICOM_PORT, ITP_IOCTL_INIT, NULL);
	ioctl(MICOM_PORT, ITP_IOCTL_RESET, MICOM_BAUDRATE); 
	itpRegisterDevice(MICOM_PORT, &MICOM_DEVICE);
	ioctl(MICOM_PORT, ITP_IOCTL_INIT, NULL);
	ioctl(MICOM_PORT, ITP_IOCTL_RESET, MICOM_BAUDRATE); 
	itpRegisterDevice(MICOM_PORT, &MICOM_DEVICE);
	ioctl(MICOM_PORT, ITP_IOCTL_INIT, NULL);
	ioctl(MICOM_PORT, ITP_IOCTL_RESET, MICOM_BAUDRATE); 
	itpRegisterDevice(MICOM_PORT, &MICOM_DEVICE);
	ioctl(MICOM_PORT, ITP_IOCTL_INIT, NULL);
	ioctl(MICOM_PORT, ITP_IOCTL_RESET, MICOM_BAUDRATE); 
#endif

	
		
#if defined(VERIFY_INTR_MODE)
	param.sched_priority = sched_get_priority_max(2);//sched_get_priority_min(1) + 2;//sched_get_priority_max(1);//sched_get_priority_min(1) + 30;// = sched_get_priority_max(1);
	pthread_attr_setschedparam(&attr, &param);	
	pthread_create(&MICOMTask, &attr, XO_MICOM_FuncUseINTR, NULL);
#elif defined(VERIFY_DMA_MODE)
	pthread_create(&MICOMTask, &attr, XO_MICOM_FuncUseDMA, NULL);
#elif defined(VERIFY_FIFO_MODE)
	pthread_create(&MICOMTask, &attr, XO_MICOM_FuncUseFIFO, NULL);
#endif

	MICOMInited = true;
    return true;
}


uint8_t MICOM_CheckSum(uint8_t *RxData)
{
	uint8_t i,CS_DATA=0;
	
	CS_DATA=0;
	for (i=0;i<MICOM_DATA_LENTH-1;i++)
	{
		CS_DATA = CS_DATA+RxData[i];
		//printf("MICOM_CheckSum:0x%X %d\n", RxData[i],i);
	}
	return CS_DATA;
}
void MICOM_TxData(uint8_t *RxData)
{
	uint8_t TX_DATA[MICOM_DATA_LENTH],CS_DATA=0,i;
	
	CS_DATA=0;
	
	memcpy(&TX_DATA[0], RxData, MICOM_DATA_LENTH);
	
	TX_DATA[1] = TX_DATA[1]&0x7F;
	for (i=0;i<MICOM_DATA_LENTH-1;i++)
	{
		CS_DATA = CS_DATA+TX_DATA[i];
		//printf("MICOM_TxData:0x%X %d\n", TX_DATA[i],i);
	}
	TX_DATA[MICOM_DATA_LENTH-1] = CS_DATA;
	
	
	
	write(MICOM_PORT, &TX_DATA[0], MICOM_DATA_LENTH);
}



#define ReturnTrueData(Data,Mask,cData,TrueData,FailData,ZoomFlag)              ((Data&Mask)==cData) ? (TrueData|0x80):(FailData|ZoomFlag)


// // off�� �����? 0x80:Ȯ�� ���� , 40:Ȯ�� �Ϸ� , 20:Ȯ���� , 10:Ȯ�� �ʿ�
#define GetRxD2bit(tData,MainFunc,zoom) 	{ 													\
												if (tData==0)									\
												{ 												\
													MainFunc = 0;								\
												} 												\
												else if (((MainFunc&0x03)!=0x0)&&((tData)!=0))	\
												{    											\
													MainFunc = tData|(MainFunc&0xF0);	 		\
												} 												\
												else if (zoom==0)								\
												{ 												\
													MainFunc = (tData|0x10);					\
												} 												\
												else  											\
												{ 												\
													MainFunc = (tData|zoom);					\
												} 												\
											}

#define GetRxD2bitF(tData,MainFunc,zoom) 	{ 													\
												if (tData==0)									\
												{ 												\
													MainFunc = 0;								\
												} 												\
												else if (((MainFunc&0x03)!=0x0)&&((tData)!=0))	\
												{    											\
													MainFunc = tData|(MainFunc&0xF0); 			\
												} 												\
												else if (zoom==0)								\
												{ 												\
													MainFunc = (tData|0x10);					\
												} 												\
												else  											\
												{ 												\
													MainFunc = (tData|zoom);					\
												} 												\
											}


// // off�� �����? 0x80:Ȯ�� ���� , 40:Ȯ�� �Ϸ� , 20:Ȯ���� , 10:Ȯ�� �ʿ�

// JPHong 2023-10-12
//uint8_t WarningPriority[WARNING_ICON_CNT] = { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27};
  uint8_t WarningPriority[WARNING_ICON_CNT] = { 1, 2, 3,14, 6,15, 4,16, 5,19,18,17, 7, 8, 9,10,13,11,12,25,27,26,24,23,22,21,20};


//uint8_t FunctionPriority[FUNCTION_ICON_CNT] = { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19};
  uint8_t FunctionPriority[FUNCTION_ICON_CNT] = { 1, 2, 3,13, 4, 5, 6, 7, 8, 9,12,10,11,14,15,16,17,18,19,20,21,22};
  
  
//#define ChkZoombit(Index) 	if (((MainWarnigData[Index]&0x03)!=0)&&((MainWarnigData[Index]&0xF0)==0x10))



#if 0
#define ChkZoombit(Index) 	if ((((MainWarnigData[Index]&0x03)!=0)&&((MainWarnigData[Index]&0xF0)==0x10))&&(Main_wZoomIndex_Time+TIME_CNT_300MS < gMainTimerCnt_10m)&&(WarningPriority[Index] < Main_wZoomIndex))	\
							{												\
								Main_wZoomIndex = Index;					\
								Main_wZoomIndex_Time = gMainTimerCnt_10m;	\
							}
#else
/*
#define ChkZoombit(Index) 	if ((((MainWarnigData[Index]&0x03)!=0)&&((MainWarnigData[Index]&0xF0)==0x10))&&(Main_wZoomIndex_Time+TIME_CNT_300MS < gMainTimerCnt_10m)&&(WarningPriority[Index] < Main_wZoomIndex))	\
							{																					\
								Main_wZoomIndex = Index;														\
								Main_wZoomIndex_Time = gMainTimerCnt_10m;										\
							}																					\
							else if ((((MainWarnigData[Index]&0x03)!=0)&&((MainWarnigData[Index]&0xF0)==0x10)))	\
							{																					\
								Main_wZoomIndex_Time_Disp = 1;													\
							}
*/

#define ChkZoombit(Index) 	if ((Main_wZoomIndex_Time+TIME_CNT_300MS > gMainTimerCnt_10m)&&(((MainWarnigData[Index]&0x03)!=0)&&((MainWarnigData[Index]&0xF0)==0x10))&&(WarningPriority[Index] < WarningPriority[Main_wZoomIndex]))	\
							{													\
								{												\
									Main_wZoomIndex_Time_Disp = 1;				\
									Main_wZoomIndex_Time = gMainTimerCnt_10m;	\
									Main_wZoomIndex = Index;					\
								}												\
							}													\
							else if ((Main_wZoomIndex_Time+TIME_CNT_300MS > gMainTimerCnt_10m)&&(((MainWarnigData[Index]&0x03)!=0)&&((MainWarnigData[Index]&0xF0)==0x10)))	\
							{													\
								{												\
									Main_ZoomSetDataW (0x10 , 0x20);			\
									Main_wZoomIndex_Time = gMainTimerCnt_10m;	\
								}												\
							}													\
							else if (((MainWarnigData[Index]&0x03)!=0)&&((MainWarnigData[Index]&0xF0)==0x10))	\
							{													\
								{												\
									Main_wZoomIndex_Time_Disp = 0;				\
									Main_wZoomIndex_Time = gMainTimerCnt_10m;	\
									Main_wZoomIndex = Index;					\
								}												\
							}

#endif
//#define ChkZoombitF(Index) 	if (((MainFunctionData[Index-cF_01_B]&0x03)!=0)&&((MainFunctionData[Index-cF_01_B]&0xF0)==0x10))

#if 0
#define ChkZoombitF(Index) 	if ((((MainFunctionData[Index-cF_01_B]&0x03)!=0)&&((MainFunctionData[Index-cF_01_B]&0xF0)==0x10))&&(Main_fZoomIndex_Time+TIME_CNT_300MS < gMainTimerCnt_10m)&&(FunctionPriority[Index-cF_01_B] < (Main_fZoomIndex-cF_01_B)))	\
							{												\
								Main_fZoomIndex = Index;					\
								Main_fZoomIndex_Time = gMainTimerCnt_10m;	\
							}

#endif


#define ChkZoombitF(Index) 	if ((Main_fZoomIndex_Time+TIME_CNT_300MS > gMainTimerCnt_10m)&&(((MainFunctionData[Index-cF_01_B]&0x03)!=0)&&((MainFunctionData[Index-cF_01_B]&0xF0)==0x10))&&(FunctionPriority[Index-cF_01_B] < FunctionPriority[Main_fZoomIndex]))			\
							{														\
								{													\
									Main_fZoomIndex_Time_Disp = 1;					\
									Main_fZoomIndex_Time = gMainTimerCnt_10m;		\
									Main_fZoomIndex = Index;						\
								}													\
							}														\
							else if ((Main_fZoomIndex_Time+TIME_CNT_300MS > gMainTimerCnt_10m)&&(((MainFunctionData[Index-cF_01_B]&0x03)!=0)&&((MainFunctionData[Index-cF_01_B]&0xF0)==0x10)))	\
							{														\
								{													\
									Main_ZoomSetDataF (0x10 , 0x20);				\
									Main_fZoomIndex_Time = gMainTimerCnt_10m;		\
								}													\
							}														\
							else if (((MainFunctionData[Index-cF_01_B]&0x03)!=0)&&((MainFunctionData[Index-cF_01_B]&0xF0)==0x10))	\
							{														\
								{													\
									Main_fZoomIndex_Time_Disp = 0;					\
									Main_fZoomIndex_Time = gMainTimerCnt_10m;		\
									Main_fZoomIndex = Index;						\
								}													\
							}
							
							
							
							

bool MainLayer_Power_Icon_Check(bool OffFlag);
extern void Clear_ACRD_TimeCnt(void);


extern uint8_t gLogoPW_D7;

bool MICOM_RxParsing(void)
{
	uint32_t CAN_ID =0 ;
	static uint8_t TempRI2=0;
	static uint8_t TempRI3_7ByteH=0;
	static uint8_t TempRI3_7ByteL=0;
	static uint8_t tTempRI3_7ByteL=99;
	static uint8_t tbTempRI3_7ByteL=99;	
	static uint8_t TempRI4_0Byte=0;
	static uint8_t TempRI4_Cnt=0;
	static uint8_t bTempRI4_0Byte=0;
	static uint8_t bTempRI4_Cnt=0;
	
	static uint8_t tLenght = 0;				// add 230615 jhlee
	static uint8_t tPacket_CNT = 0;		// add 230615 jhlee
	static uint8_t PktData[128];			// add 230615 jhlee

	static uint8_t tLenght_ECU = 0;				// add 230615 jhlee
	static uint8_t tPacket_ECU_CNT = 0;		// add 230615 jhlee
	static uint8_t PktData_ECU[128];			// add 230615 jhlee

	static uint8_t tLenght_DCU = 0;				// add 230615 jhlee
	static uint8_t tPacket_DCU_CNT = 0;		// add 230615 jhlee
	static uint8_t PktData_DCU[128];			// add 230615 jhlee
	
	static uint16_t tPGN = 0;			// add 230626 jhlee
		
	if (MICOM_RX.RxFlag[MICOM_RX.Rx_Read]==1)
	{
		if (ConnectCan<=2)
		{
			//if (ConnectCan==2)
			//	pthread_mutex_unlock(&tx0_mutex);
			ConnectCan++;
		}
	
		CAN_ID = (MICOM_RX.RxData[MICOM_RX.Rx_Read][1]<<24)|(MICOM_RX.RxData[MICOM_RX.Rx_Read][2]<<16)|(MICOM_RX.RxData[MICOM_RX.Rx_Read][3]<<8)|(MICOM_RX.RxData[MICOM_RX.Rx_Read][4]<<0);
		
//		printf("CAN_ID:0x%08X \n", CAN_ID);


#ifdef USED_ROTATION
		if (RotationMode!=0)
		{
			if (ReturnMode4CAN_Rot(CAN_ID)==0)
			{
				MICOM_RX.RxFlag[MICOM_RX.Rx_Read]=0;
				MICOM_RX.Rx_Read++;
				if (MICOM_RX.Rx_Read >= MICOM_RX_DATA_CNT)
				{
					MICOM_RX.Rx_Read = 0;
				}
				return 0;
			}
		}
#endif	
		switch(CAN_ID)	// cmd
		{
			case CLM1:	// 14FF00E4
			{
				if (MainLayerFlag == 1)
				{
					uint8_t ZoomFlag = 0x80;  // 0:zoom
					uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
					uint8_t tData;
					static bool tMeterLiftLdFlag= 0;
					
	//				printf("[CLM1] [%x]\n", sData[0]);
	//				printf("[CLM1] Main_WSafetyStart : [%x]\n", Main_WSafetyStart);
									
					if (((sData[5]>>6)&0x01) == 0x1)
					{
						Main_WEngStopSw = 0x03;
						if (Main_WSafetyStart != 0x0)
							Main_WSafetyStart = 0x01;
						else
							Main_WSafetyStart = 0;
					}
					else if (((sData[5]>>6)&0x01) == 0x0)
					{
						if (Main_WEngStopSw != 0x0)
							Main_WEngStopSw = 0x01;
						else
							Main_WEngStopSw = 0;
					}
					if (((sData[5]>>6)&0x01) != 0x1)
					{	
						if (((sData[5]>>7)&0x01) == 0x1)
							Main_WSafetyStart = 0x03;
						else if (((sData[5]>>7)&0x01) == 0x0)
						{
							if (Main_WSafetyStart != 0x0)
								Main_WSafetyStart = 0x01;
							else
								Main_WSafetyStart = 0;
						}
					}
	//				printf("[CLM1] Main_WEngStopSw : [%x]\n", Main_WEngStopSw);
	//				printf("[CLM1] Main_WSafetyStart : [%x]\n\n", Main_WSafetyStart);
					
									
					if ((Main_wZoomIndex>WARNING_ICON_CNT)&&((Main_WEngStopSw == 0x0)&&(Main_WSafetyStart == 0x0)))
					{
						ZoomFlag = (sData[7]&0x80);
					}
					//printf("sData[6]:0x%08X \n", sData[6]);
					GetRxD2bitF(((sData[6]>>0)&0x01),MainFunctionData[cF_04_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[6]>>4)&0x01),MainFunctionData[cF_05_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[6]>>3)&0x01),MainFunctionData[cF_07_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[6]>>1)&0x01),MainFunctionData[cF_08_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[6]>>5)&0x01),MainFunctionData[cF_09_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[0]>>0)&0x01),MainFunctionData[cF_10_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[0]>>1)&0x01),MainFunctionData[cF_11_B-cF_01_B],0x80);			//auto dcel
					GetRxD2bitF(((sData[1]>>6)&0x01),MainFunctionData[cF_12_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[0]>>3)&0x01),MainFunctionData[cF_13_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[1]>>0)&0x01),MainFunctionData[cF_18_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[2]>>4)&0x01),MainFunctionData[cF_14_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[2]>>6)&0x01),MainFunctionData[cF_15_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[2]>>1)&0x01),MainFunctionData[cF_16_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[2]>>3)&0x01),MainFunctionData[cF_17_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[1]>>3)&0x01),MainFunctionData[cF_19_B-cF_01_B],ZoomFlag);
					
					if (tMeterLiftLdFlag!=((sData[1]>>6)&0x01))
					{
						tMeterLiftLdFlag=((sData[1]>>6)&0x01);
						
						MeterLiftLdFlag = tMeterLiftLdFlag;
					}
					
					
	
	#if 0
					printf("\n[CLM1] cF_04_B : [%x]\n", MainFunctionData[cF_04_B-cF_01_B]);
					printf("[CLM1] cF_05_B : [%x]\n", MainFunctionData[cF_05_B-cF_01_B]);
					printf("[CLM1] cF_07_B : [%x]\n", MainFunctionData[cF_07_B-cF_01_B]);
					printf("[CLM1] cF_08_B : [%x]\n", MainFunctionData[cF_08_B-cF_01_B]);
					printf("[CLM1] cF_09_B : [%x]\n", MainFunctionData[cF_09_B-cF_01_B]);
					printf("[CLM1] cF_10_B : [%x]\n", MainFunctionData[cF_10_B-cF_01_B]);
					printf("[CLM1] cF_11_B : [%x]\n", MainFunctionData[cF_11_B-cF_01_B]);
					printf("[CLM1] cF_12_B : [%x]\n", MainFunctionData[cF_12_B-cF_01_B]);
					printf("[CLM1] cF_13_B : [%x]\n", MainFunctionData[cF_13_B-cF_01_B]);
					printf("[CLM1] cF_18_B : [%x]\n", MainFunctionData[cF_18_B-cF_01_B]);
					printf("[CLM1] cF_14_B : [%x]\n", MainFunctionData[cF_14_B-cF_01_B]);
					printf("[CLM1] cF_15_B : [%x]\n", MainFunctionData[cF_15_B-cF_01_B]);
					printf("[CLM1] cF_16_B : [%x]\n", MainFunctionData[cF_16_B-cF_01_B]);
					printf("[CLM1] cF_17_B : [%x]\n", MainFunctionData[cF_17_B-cF_01_B]);
					printf("[CLM1] cF_19_B : [%x]\n", MainFunctionData[cF_19_B-cF_01_B]);
	
	//				printf("Main_wZoomIndex : [%d]\n", Main_wZoomIndex);
	//				printf("Main_fZoomIndex : [%d]\n", Main_fZoomIndex);
	#endif
					if ((ZoomFlag==0x00)&&((cF_01_B<=Main_wZoomIndex)||(Main_wZoomIndex==0xFF)))
					{					
						ChkZoombitF(cF_13_B)
						else ChkZoombitF(cF_04_B)
						else ChkZoombitF(cF_05_B)
						else ChkZoombitF(cF_07_B)
						else ChkZoombitF(cF_08_B)
						else ChkZoombitF(cF_09_B)
						else ChkZoombitF(cF_12_B)
						else ChkZoombitF(cF_10_B)
	//					else ChkZoombitF(cF_11_B)	//auto dcel
						else ChkZoombitF(cF_14_B)				
						else ChkZoombitF(cF_15_B)
						else ChkZoombitF(cF_16_B)
						else ChkZoombitF(cF_17_B)
						else ChkZoombitF(cF_18_B)
						else ChkZoombitF(cF_19_B)
					}
					CLM1_Time = gMainTimerCnt_10m;
	//				printf("Main_fZoomIndex : [%d]\n\n", Main_fZoomIndex);
	#if 0
					printf("\n     [CLM1] cF_05_B : [%x]\n", MainFunctionData[cF_05_B-cF_01_B]);
				
					if ((Main_fZoomIndex!=0xFF)&&(Main_fZoomIndex!=0xFE))
					{
						printf("Main_wZoomIndex : [%d]\n", Main_wZoomIndex);
						printf("Main_fZoomIndex : [%d]\n", Main_fZoomIndex);
					}
	#endif	
	#if 0
					printf("[CLM1] cF_04_B : [%x]\n", MainFunctionData[cF_04_B-cF_01_B]);
					printf("[CLM1] cF_05_B : [%x]\n", MainFunctionData[cF_05_B-cF_01_B]);
					printf("[CLM1] cF_07_B : [%x]\n", MainFunctionData[cF_07_B-cF_01_B]);
					printf("[CLM1] cF_08_B : [%x]\n", MainFunctionData[cF_08_B-cF_01_B]);
					printf("[CLM1] cF_09_B : [%x]\n", MainFunctionData[cF_09_B-cF_01_B]);
					printf("[CLM1] cF_10_B : [%x]\n", MainFunctionData[cF_10_B-cF_01_B]);
					printf("[CLM1] cF_11_B : [%x]\n", MainFunctionData[cF_11_B-cF_01_B]);
					printf("[CLM1] cF_12_B : [%x]\n", MainFunctionData[cF_12_B-cF_01_B]);
					printf("[CLM1] cF_13_B : [%x]\n", MainFunctionData[cF_13_B-cF_01_B]);
					printf("[CLM1] cF_18_B : [%x]\n", MainFunctionData[cF_18_B-cF_01_B]);
					printf("[CLM1] cF_14_B : [%x]\n", MainFunctionData[cF_14_B-cF_01_B]);
					printf("[CLM1] cF_15_B : [%x]\n", MainFunctionData[cF_15_B-cF_01_B]);
					printf("[CLM1] cF_16_B : [%x]\n", MainFunctionData[cF_16_B-cF_01_B]);
					printf("[CLM1] cF_17_B : [%x]\n", MainFunctionData[cF_17_B-cF_01_B]);
					printf("[CLM1] cF_19_B : [%x]\n", MainFunctionData[cF_19_B-cF_01_B]);
	
					printf("Main_wZoomIndex : [%d]\n", Main_wZoomIndex);
					printf("Main_fZoomIndex : [%d]\n\n", Main_fZoomIndex);
	#endif
				}
			}
			break;

			case CLM2:	// 14500EE4	// LCD ����ȭ�鿡�� �ְ� �����?�������� ���� ���� , LCD �ְ� ���?
			{
				if (MainLayerFlag == 1)
				{
					uint8_t ZoomFlag = 0x80;  // 0:zoom
					uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
					uint8_t tData;
					static uint8_t tCLM2_7Byte_5bit = 0x0FF;	//0:No change , 1: 1->0(off) ,  2: 0->1(on) 
					
					
					if ((Main_wZoomIndex>WARNING_ICON_CNT)&&((Main_WEngStopSw == 0x0)&&(Main_WSafetyStart == 0x0)))
					{
						ZoomFlag = (sData[7]&0x80);
					}
	
					GetRxD2bitF(((sData[2]>>7)&0x01),MainFunctionData[cF_01_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[6]>>6)&0x01),MainFunctionData[cF_02_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[6]>>2)&0x01),MainFunctionData[cF_03_B-cF_01_B],ZoomFlag);
	
					if ((ZoomFlag==0x00)&&((cF_01_B<=Main_wZoomIndex)||(Main_wZoomIndex==0xFF)))
					{
						ChkZoombitF(cF_01_B)
						else ChkZoombitF(cF_02_B)
						else ChkZoombitF(cF_03_B)
					}

					Main_DayNight = (sData[6]>>5)&0x01;

	//				printf("[CLM2]1 CLM2_7Byte_5bit : [%d] [%d] [%d] [%d]\n", CLM2_7Byte_5bit,tCLM2_7Byte_5bit,Option_Function.AutoCamera,Main_Disp_Mode);
					
#if 0										
					if (tCLM2_7Byte_5bit == 0x0FF)
						tCLM2_7Byte_5bit=((sData[7]>>5)&0x01);
					if (Option_Function.AutoCamera!=0)
					{
						if (tCLM2_7Byte_5bit!=((sData[7]>>5)&0x01))
						{
							tCLM2_7Byte_5bit=((sData[7]>>5)&0x01);
							if (tCLM2_7Byte_5bit==0)
								CLM2_7Byte_5bit = 1;	////0:No change , 1: 1->0(off) ,  2: 0->1(on)
							else
							{
								CLM2_7Byte_5bit = 2;	////0:No change , 1: 1->0(off) ,  2: 0->1(on)
								if ((CLM2_7Byte_5bit==2)&&(Option_Function.AutoCamera!=0))	//0:No change , 1: 1->0(off) ,  2: 0->1(on)
								{
									CLM2_7Byte_5bit = 0;
									if (Main_Disp_Mode != MAIN_MODE_CAMERA)	// #14532  JPHong 2024-03-15
									{
										switch (Main_Disp_Mode)
										{
											case MAIN_MODE_NORMAL:
											case MAIN_MODE_AIRCON:
											case MAIN_MODE_RADIO:
											case MAIN_MODE_BLUETOOTH:
											case MAIN_MODE_PHONE:
											case MAIN_MODE_DAB1:
											case MAIN_MODE_DAB2:
											case MAIN_MODE_DAB3:
											{
												if((Main_Disp_Mode==MAIN_MODE_MENU)||(Main_Disp_Mode==MAIN_MODE_BACK)||(Main_Disp_Mode==MAIN_MODE_PW)||(Main_Disp_Mode==MAIN_MODE_USERINFO))
												{
													GotoMain(MAIN_MODE_CAMERA); 
												}
												else
												{
													char JD_Buf[10] = {0,};	// JPHong 2024-01-18
													JD_Buf[0] = 0;
													MainCamera(NULL, JD_Buf);
												}
											}
										}
									}
								}							
							}
						}
					}
					else
					{
						tCLM2_7Byte_5bit=((sData[7]>>5)&0x01);
						CLM2_7Byte_5bit = 0;
					}
#endif					
	//				printf("[CLM2]2 CLM2_7Byte_5bit : [%d] [%d] [%d] [%d]\n", CLM2_7Byte_5bit,tCLM2_7Byte_5bit,Option_Function.AutoCamera,Main_Disp_Mode);
	#if 0
					printf("[CLM2] cF_01_B : [%x] %d\n", MainFunctionData[cF_01_B-cF_01_B],ZoomFlag);
					printf("[CLM2] cF_02_B : [%x] %d\n", MainFunctionData[cF_02_B-cF_01_B],ZoomFlag);
					printf("[CLM2] cF_03_B : [%x] %d\n", MainFunctionData[cF_03_B-cF_01_B],ZoomFlag);
	#endif
					CLM2_Time = gMainTimerCnt_10m;
				}
			}
			break;

			case CLM3:	// 14500CE4
			{
				if (MainLayerFlag == 1)
				{
					uint8_t ZoomFlag = 0x80;  // 0:zoom
					uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
					uint8_t tData;
					ZoomFlag = (sData[7]&0x80);

	
					GetRxD2bit(((sData[5]>>2)&0x03),MainWarnigData[cW_01_B],ZoomFlag);
					GetRxD2bit(((sData[1]>>4)&0x03),MainWarnigData[cW_02_B],ZoomFlag);
					GetRxD2bit(((sData[2]>>0)&0x03),MainWarnigData[cW_03_B],ZoomFlag);
					GetRxD2bit(((sData[0]>>0)&0x03),MainWarnigData[cW_04_B],ZoomFlag);
					GetRxD2bit(((sData[0]>>4)&0x03),MainWarnigData[cW_05_B],ZoomFlag);
					GetRxD2bit(((sData[0]>>6)&0x03),MainWarnigData[cW_06_B],ZoomFlag);
					GetRxD2bit(((sData[0]>>2)&0x03),MainWarnigData[cW_07_B],ZoomFlag);
					GetRxD2bit(((sData[1]>>0)&0x03),MainWarnigData[cW_08_B],ZoomFlag);
					GetRxD2bit(((sData[5]>>4)&0x03),MainWarnigData[cW_09_B],ZoomFlag);
					GetRxD2bit(((sData[2]>>2)&0x03),MainWarnigData[cW_10_B],ZoomFlag);
					GetRxD2bit(((sData[4]>>2)&0x03),MainWarnigData[cW_11_B],ZoomFlag);
					GetRxD2bit(((sData[4]>>4)&0x03),MainWarnigData[cW_12_B],ZoomFlag);
					GetRxD2bit(((sData[6]>>6)&0x03),MainWarnigData[cW_17_B],ZoomFlag);
					GetRxD2bit(((sData[5]>>6)&0x03),MainWarnigData[cW_20_B],ZoomFlag);
	
	
	#if 0
					printf("Main_wZoomIndex : [%d]\n", Main_wZoomIndex);
					printf("[CLM3] cW_01_B : [%x] \n", MainWarnigData[cW_01_B]);
					printf("[CLM3] cW_02_B : [%x] \n", MainWarnigData[cW_02_B]);
					printf("[CLM3] cW_03_B : [%x] \n", MainWarnigData[cW_03_B]);
					printf("[CLM3] cW_04_B : [%x] \n", MainWarnigData[cW_04_B]);
					printf("[CLM3] cW_05_B : [%x] \n", MainWarnigData[cW_05_B]);
					printf("[CLM3] cW_06_B : [%x] \n", MainWarnigData[cW_06_B]);
					printf("[CLM3] cW_07_B : [%x] \n", MainWarnigData[cW_07_B]);
					printf("[CLM3] cW_08_B : [%x] \n", MainWarnigData[cW_08_B]);
					printf("[CLM3] cW_09_B : [%x] \n", MainWarnigData[cW_09_B]);
					printf("[CLM3] cW_10_B : [%x] \n", MainWarnigData[cW_10_B]);
					printf("[CLM3] cW_11_B : [%x] \n", MainWarnigData[cW_11_B]);
					printf("[CLM3] cW_12_B : [%x] \n", MainWarnigData[cW_12_B]);
					printf("[CLM3] cW_17_B : [%x] \n", MainWarnigData[cW_17_B]);
					printf("[CLM3] cW_20_B : [%x] \n", MainWarnigData[cW_20_B]);
					printf("Main_wZoomIndex : [%d]\n", Main_wZoomIndex);
					printf("Main_fZoomIndex : [%d]\n\n", Main_fZoomIndex);
					
					printf("[CLM3] cF_06_B : [%x] \n", MainFunctionData[cF_06_B]);
	#endif
//printf("\n[CLM3] cW_01_B : [%x] \n", MainWarnigData[cW_01_B]);	
#if 0	
					if ((Main_wZoomIndex!=0xFF)&&(Main_wZoomIndex!=0xFE))
					{
						printf("\n[CLM3] cW_01_B1: [%x] \n", MainWarnigData[cW_01_B]);
						printf("Main_wZoomIndex : [%d]\n", Main_wZoomIndex);
						printf("Main_fZoomIndex : [%d]\n\n", Main_fZoomIndex);
						
					}
#endif	
					if (ZoomFlag==0x00)
					{
						ChkZoombit(cW_01_B)
						else ChkZoombit(cW_02_B)
						else ChkZoombit(cW_03_B)
						else ChkZoombit(cW_07_B)
						else ChkZoombit(cW_09_B)
						else ChkZoombit(cW_05_B)
						else ChkZoombit(cW_17_B)
						else ChkZoombit(cW_04_B)
						else ChkZoombit(cW_06_B)
						else ChkZoombit(cW_08_B)
						else ChkZoombit(cW_12_B)
						else ChkZoombit(cW_11_B)					
						else ChkZoombit(cW_10_B)
						else ChkZoombit(cW_20_B)
					}
					ZoomFlag = 0x80;
					if ((Main_wZoomIndex>WARNING_ICON_CNT)&&((Main_WEngStopSw == 0x0)&&(Main_WSafetyStart == 0x0)))
					{
						ZoomFlag = (sData[7]&0x80);
					}
//					printf("[CLM3] cF_06_B 1: [%d][%d] \n",ZoomFlag, MainFunctionData[cF_06_B-cF_01_B]);
//					printf("Main_fZoomIndex 1: [%d]\n\n", Main_fZoomIndex);
					GetRxD2bitF(((sData[1]>>6)&0x03),MainFunctionData[cF_06_B-cF_01_B],ZoomFlag);
					
//					printf("[CLM3] cF_06_B 2: [%d] \n", MainFunctionData[cF_06_B-cF_01_B]);
//					printf("Main_fZoomIndex 2: [%d] [%d] [%d]\n\n", Main_fZoomIndex,ZoomFlag,Main_wZoomIndex);				
					if ((ZoomFlag==0x00)&&((cF_01_B<=Main_wZoomIndex)||(Main_wZoomIndex==0xFF)))
					{
						ChkZoombitF(cF_06_B)
					}
//					printf("[CLM3] cF_06_B 3: [%d] \n", MainFunctionData[cF_06_B-cF_01_B]);
//					printf("Main_fZoomIndex 3: [%d]\n\n", Main_fZoomIndex);
	
	
//	printf("\n[CLM3] cW_01_B : [%x] [%x] [%x] \n", MainWarnigData[cW_01_B],Main_wZoomIndex,Main_fZoomIndex);
//printf("\n[CLM3] cW_01_B : [%x] \n", MainWarnigData[cW_01_B]);	
//printf("Main_wZoomIndex : [%d]\n", Main_wZoomIndex);	
	#if 0
					printf("Main_wZoomIndex : [%d]\n", Main_wZoomIndex);
					printf("[CLM3] cW_01_B : [%x] \n", MainWarnigData[cW_01_B]);
					printf("[CLM3] cW_02_B : [%x] \n", MainWarnigData[cW_02_B]);
					printf("[CLM3] cW_03_B : [%x] \n", MainWarnigData[cW_03_B]);
					printf("[CLM3] cW_04_B : [%x] \n", MainWarnigData[cW_04_B]);
					printf("[CLM3] cW_05_B : [%x] \n", MainWarnigData[cW_05_B]);
					printf("[CLM3] cW_06_B : [%x] \n", MainWarnigData[cW_06_B]);
					printf("[CLM3] cW_07_B : [%x] \n", MainWarnigData[cW_07_B]);
					printf("[CLM3] cW_08_B : [%x] \n", MainWarnigData[cW_08_B]);
					printf("[CLM3] cW_09_B : [%x] \n", MainWarnigData[cW_09_B]);
					printf("[CLM3] cW_10_B : [%x] \n", MainWarnigData[cW_10_B]);
					printf("[CLM3] cW_11_B : [%x] \n", MainWarnigData[cW_11_B]);
					printf("[CLM3] cW_12_B : [%x] \n", MainWarnigData[cW_12_B]);
					printf("[CLM3] cW_17_B : [%x] \n", MainWarnigData[cW_17_B]);
					printf("[CLM3] cW_20_B : [%x] \n", MainWarnigData[cW_20_B]);
					printf("Main_wZoomIndex : [%d]\n", Main_wZoomIndex);
					printf("Main_fZoomIndex : [%d]\n\n", Main_fZoomIndex);
					
					printf("[CLM3] cF_06_B : [%x] \n", MainFunctionData[cF_06_B]);
	#endif
					CLM3_Time = gMainTimerCnt_10m;
				}
			}
			break;
			case CLM4:	// 14500AE4
			{
				if (MainLayerFlag == 1)
				{
					uint8_t ZoomFlag = 0x80;  // 0:zoom
					uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
					uint8_t tData;
					ZoomFlag = (sData[7]&0x80);
	
#if 0
					printf("\n\n[CLM4] cW_13_B : [%x] \n", MainWarnigData[cW_13_B]);
					printf("[CLM4] cW_14_B : [%x] \n", MainWarnigData[cW_14_B]);
					printf("[CLM4] cW_15_B : [%x] \n", MainWarnigData[cW_15_B]);
					printf("[CLM4] cW_16_B : [%x] \n", MainWarnigData[cW_16_B]);
					printf("[CLM4] cW_18_B : [%x] \n", MainWarnigData[cW_18_B]);
					printf("[CLM4] cW_19_B : [%x] \n", MainWarnigData[cW_19_B]);
	//				printf("[CLM4] cW_20_B : [%x] \n", MainWarnigData[cW_20_B]);
					printf("[CLM4]1 Main_wZoomIndex : [%d] \n\n", Main_wZoomIndex);
#endif
//printf("[CLM4] cW_14_B : [%x] \n", MainWarnigData[cW_14_B]);	
					GetRxD2bit(((sData[0]>>6)&0x03),MainWarnigData[cW_13_B],ZoomFlag);
					GetRxD2bit(((sData[0]>>4)&0x03),MainWarnigData[cW_14_B],ZoomFlag);
					GetRxD2bit(((sData[1]>>2)&0x03),MainWarnigData[cW_15_B],ZoomFlag);
					GetRxD2bit(((sData[1]>>0)&0x03),MainWarnigData[cW_16_B],ZoomFlag);
					GetRxD2bit(((sData[0]>>2)&0x03),MainWarnigData[cW_18_B],ZoomFlag);
					GetRxD2bit(((sData[0]>>0)&0x03),MainWarnigData[cW_19_B],ZoomFlag);
	//				GetRxD2bit(((sData[3]>>6)&0x03),MainWarnigData[cW_20_B],ZoomFlag);
//printf("[CLM4] cW_14_B : [%x] \n", MainWarnigData[cW_14_B]);
//printf("[CLM4]1 Main_wZoomIndex : [%d] \n\n", Main_wZoomIndex);
	#if 0
					printf("[CLM4] cW_13_B : [%x] \n", MainWarnigData[cW_13_B]);
					printf("[CLM4] cW_14_B : [%x] \n", MainWarnigData[cW_14_B]);
					printf("[CLM4] cW_15_B : [%x] \n", MainWarnigData[cW_15_B]);
					printf("[CLM4] cW_16_B : [%x] \n", MainWarnigData[cW_16_B]);
					printf("[CLM4] cW_18_B : [%x] \n", MainWarnigData[cW_18_B]);
					printf("[CLM4] cW_19_B : [%x] \n", MainWarnigData[cW_19_B]);
	//				printf("[CLM4] cW_20_B : [%x] \n", MainWarnigData[cW_20_B]);
					printf("[CLM4]1 Main_wZoomIndex : [%d] \n\n", Main_wZoomIndex);
#endif
					if (ZoomFlag==0x00)
					{
						ChkZoombit(cW_13_B)
						else ChkZoombit(cW_14_B)
						else ChkZoombit(cW_15_B)
						else ChkZoombit(cW_16_B)
						else ChkZoombit(cW_18_B)
						else ChkZoombit(cW_19_B)
						else ChkZoombit(cW_17_B)
	//					else ChkZoombit(cW_20_B)
					}
	#if 0
					printf("[CLM4] cW_13_B : [%x] \n", MainWarnigData[cW_13_B]);
					printf("[CLM4] cW_14_B : [%x] \n", MainWarnigData[cW_14_B]);
					printf("[CLM4] cW_15_B : [%x] \n", MainWarnigData[cW_15_B]);
					printf("[CLM4] cW_16_B : [%x] \n", MainWarnigData[cW_16_B]);
					printf("[CLM4] cW_18_B : [%x] \n", MainWarnigData[cW_18_B]);
					printf("[CLM4] cW_19_B : [%x] \n", MainWarnigData[cW_19_B]);
	//				printf("[CLM4] cW_20_B : [%x] \n", MainWarnigData[cW_20_B]);
					printf("[CLM4]1 Main_wZoomIndex : [%d] \n\n", Main_wZoomIndex);
	#endif
					CLM4_Time = gMainTimerCnt_10m;
				}
			}
			break;
			

			case AUX_IO2:		// 18FF32CF
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				AUX_A_Value = sData[1];
				AUX_B_Value = sData[2];

				AUX_C_Value = sData[3];
				AUX_D_Value = sData[4];
				
				AUX_G_Value = sData[5];
				AUX_H_Value = sData[6];
				
				AUX_Value_CNT = gMainTimerCnt_10m;
			}
			break;

#if 1
			case JOG2ECU:	//18FF603B
			{
				char JD_Buf[10] = {0,};	// JPHong 2024-01-18
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				static uint8_t tJogWheel=0,tJogBTN=0,tJogSW=0,SW_Off_Flag=0;
				static uint8_t tJogStep = 0;
				static uint8_t tJogChatCNT = 0;
				
				static uint32_t JogWheelCnt = 0;

//				printf(">>JOG data %02d;[%d][%d][%d]\r\n",MICOM_RX.Rx_Read,sData[0],sData[1],sData[2]);

				// D2 0 > FF ;CW , D2 FF > 0 ; CCW
				if(tJogWheel != sData[2])
				{
					if ((abs(tJogWheel-sData[2])>=230))
					{
						if(tJogWheel>230)
						{
							JD_Buf[0] = JOG_DIAL_ROT_RIGHT;
						}
						else
						{
							JD_Buf[0] = JOG_DIAL_ROT_LEFT;
						}
					}
					else
					{
						if(tJogWheel > sData[2])
						{
							JD_Buf[0] = JOG_DIAL_ROT_LEFT;
						}
						else if(tJogWheel < sData[2])
						{
							JD_Buf[0] = JOG_DIAL_ROT_RIGHT;
						}
					}
					if ((JogWheelCnt+TIME_CNT_50MS)<gMainTimerCnt_10m)
					{
						JogWheelCnt=gMainTimerCnt_10m;
						ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY0, JD_Buf);
					}
				}
				tJogWheel = sData[2];

				// D1b7 ; AirCon Menu , D1b6 ; AirCon Auto , D1b5 ; AirCon Off
				// D1b4 ; Exit , D1b3 ; Home , D1b2 ; Ring
				if((tJogSW != sData[1])&&(Main_Disp_Mode!=MAIN_MODE_PW)&&(Main_Disp_Mode!=MAIN_MODE_USERINFO))		// JPHong 2024-02-08  ���� main 1
				{
					switch (sData[1])
					{
						case 0x80: // AirCon Menu
							if (Main_Disp_Mode!=MAIN_MODE_AIRCON)
							{
								if((Main_Disp_Mode==MAIN_MODE_MENU)||(Main_Disp_Mode==MAIN_MODE_BACK)||(Main_Disp_Mode==MAIN_MODE_PW)||(Main_Disp_Mode==MAIN_MODE_USERINFO))
								{
									SW_Off_Flag = 0;
									GotoMain(MAIN_MODE_AIRCON); 
								}
								else
								{
									JD_Buf[0] = JOG_DIAL_SW_ARICON;
									SW_Off_Flag = JOG_DIAL_SW_ARICON;
									ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY2, JD_Buf);
								}
							}
						break;
						case 0x40: // AirCon Auto
							if((Main_Disp_Mode==MAIN_MODE_MENU)||(Main_Disp_Mode==MAIN_MODE_BACK))
							{
								Main_CLI2[2] = Main_CLI2[2] |0x01;
								SW_Off_Flag = 0;
							}
							else
							{
								JD_Buf[0] = JOG_DIAL_SW_AC_AUTO;
								SW_Off_Flag = JOG_DIAL_SW_AC_AUTO;
								ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY2, JD_Buf);
							}
						break;
						case 0x20: // AirCon Off
							if((Main_Disp_Mode==MAIN_MODE_MENU)||(Main_Disp_Mode==MAIN_MODE_BACK))
							{
								Main_CLI2[2] = Main_CLI2[2] |0x80;
								SW_Off_Flag = 0;
							}
							else
							{
								JD_Buf[0] = JOG_DIAL_SW_AC_OFF;
								SW_Off_Flag = JOG_DIAL_SW_AC_OFF;
								ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY2, JD_Buf);
							}
						break;
						case 0x10: // Exit
							if(sData[0] == 0x08)		// Exit + Jog Enter
							{
								JD_Buf[0] = JOG_DIAL_SW_DEEP;
							}
							else
							{
								JD_Buf[0] = JOG_DIAL_SW_ESC_ON;
							}
							SW_Off_Flag = JOG_DIAL_SW_ESC_ON;
							ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY2, JD_Buf);
						break;
						case 0x08: // Home
							JD_Buf[0] = JOG_DIAL_SW_HOME;
							SW_Off_Flag = JOG_DIAL_SW_HOME;
							ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY2, JD_Buf);
						break;
						case 0x04: // Ring
							if ((Main_Disp_Mode!=MAIN_MODE_RADIO))	// Ȯ��  �ʿ�
							{
								if((Main_Disp_Mode==MAIN_MODE_MENU)||(Main_Disp_Mode==MAIN_MODE_BACK)||(Main_Disp_Mode==MAIN_MODE_PW)||(Main_Disp_Mode==MAIN_MODE_USERINFO))
								{
									SW_Off_Flag = 0;
									GotoMain(MAIN_MODE_RADIO); 
								}
								else
								{
									JD_Buf[0] = JOG_DIAL_SW_AUDIO;
									SW_Off_Flag = JOG_DIAL_SW_AUDIO;
									ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY2, JD_Buf);
								}
							}
						break;
						case 0x00: // SW NONE
							switch (SW_Off_Flag)
							{
								case JOG_DIAL_SW_ARICON: // AirCon Menu
									JD_Buf[0] = JOG_DIAL_SW_ARICON_OFF;
									SW_Off_Flag = 0;
								break;
								case JOG_DIAL_SW_AC_AUTO: // AirCon Auto
									JD_Buf[0] = JOG_DIAL_SW_AC_AUTO_OFF;
									SW_Off_Flag = 0;
								break;
								case JOG_DIAL_SW_AC_OFF: // AirCon Off
									JD_Buf[0] = JOG_DIAL_SW_AC_OFF_OFF;
									SW_Off_Flag = 0;
								break;
								case JOG_DIAL_SW_ESC_ON: // Exit
									JD_Buf[0] = JOG_DIAL_SW_ESC_ON_OFF;
									SW_Off_Flag = 0;
								break;
								case JOG_DIAL_SW_HOME: // Home
									JD_Buf[0] = JOG_DIAL_SW_HOME_OFF;
									SW_Off_Flag = 0;
								break;
								case JOG_DIAL_SW_AUDIO: // Ring
									JD_Buf[0] = JOG_DIAL_SW_AUDIO_OFF;
									SW_Off_Flag = 0;
								break;
								case 0x00: // SW NONE
									JD_Buf[0] = JOG_DIAL_KEY_NONE;
									SW_Off_Flag = 0;
								break;
							}
							InitJogIncDecData_AC();
							InitJogIncDecData_RD();
							ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY2, JD_Buf);
						break;
					}
					tJogSW = sData[1];
				}

				// D0b7 ; JOG UP, D0b6 ; JOG Down , D0b5 ; JOG Left , D0b4 ; JOG Right , D0b3 ; JOG Enter 
				if(tJogBTN != sData[0])
				{
					if(sData[0] == 0x80)
					{
						JD_Buf[0] = JOG_DIAL_KEY_UP;
						SW_Off_Flag = JOG_DIAL_KEY_UP;
						
					}
					else if(sData[0] == 0x40)
					{
						JD_Buf[0] = JOG_DIAL_KEY_DOWN;
						SW_Off_Flag = JOG_DIAL_KEY_DOWN;
					}
					else if(sData[0] == 0x20)
					{
						JD_Buf[0] = JOG_DIAL_KEY_LEFT;
						SW_Off_Flag = JOG_DIAL_KEY_LEFT;
					}
					else if(sData[0] == 0x10)
					{
						JD_Buf[0] = JOG_DIAL_KEY_RIGHT;
						SW_Off_Flag = JOG_DIAL_KEY_RIGHT;
					}
					else if(sData[0] == 0x08)		// jog enter
					{
						if(sData[1] == 0x10)		// sw exit
						{
							JD_Buf[0] = JOG_DIAL_KEY_DEEP;
						}
						else
						{
							JD_Buf[0] = JOG_DIAL_KEY_ENTER_ON;
						}
						SW_Off_Flag = JOG_DIAL_KEY_ENTER_ON;
					}
					else if(sData[0] == 0x00)
					{
						switch (SW_Off_Flag)
						{
							case JOG_DIAL_KEY_UP:
								JD_Buf[0] = JOG_DIAL_KEY_UP_OFF;
							break;
							case JOG_DIAL_KEY_DOWN:
								JD_Buf[0] = JOG_DIAL_KEY_DOWN_OFF;
							break;
							case JOG_DIAL_KEY_LEFT:
								JD_Buf[0] = JOG_DIAL_KEY_LEFT_OFF;
								break;
							case JOG_DIAL_KEY_RIGHT:
								JD_Buf[0] = JOG_DIAL_KEY_RIGHT_OFF;
							break;
							case JOG_DIAL_KEY_ENTER_ON:
								JD_Buf[0] = JOG_DIAL_KEY_ENTER_OFF;
							break;
							default: 
								JD_Buf[0] = JOG_DIAL_KEY_NONE;
							break;
						}
						SW_Off_Flag = 0;
						Clear_ACRD_TimeCnt();
					}
					else
					{
						JD_Buf[0] = 0;
						Clear_ACRD_TimeCnt();
					}

					tJogBTN = sData[0];
					
				//	printf("EVENT_CUSTOM_KEY1=[0x%X][0x%X]\n",JD_Buf[0],JD_Buf[1]);
					ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY1, JD_Buf);
				}

				memset(&MICOM_RX.RxData[MICOM_RX.Rx_Read][0],0,MICOM_DATA_LENTH);
			}
			break;
#endif

case CO:	// 185050E4	   100ms
			{
			/*
				ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY0, JD_Buf);	Function,Custom5,JogDialRotation,
				���� ȸ�� : 1Byte 0-7bit�� 0��255�� ��ȭ, �ݺ�
				���� ȸ�� : 1Byte 0-7bit�� 255��0���� ��ȭ, �ݺ�
				
				ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY1, JD_Buf);	Function,Custom6,JogDialKey,
				���� ���� : 3Byte 0bit�� 1�� ��ȭ
				���� ���� : 3Byte 1bit�� 1�� ��ȭ
				���� ���� : 3Byte 2bit�� 1�� ��ȭ
				���� ���� : 3Byte 3bit�� 1�� ��ȭ
				���� ���� : 2Byte 6bit�� 1�� ��ȭ
				
				ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY2, JD_Buf);	Function,Custom7,JogDialSwitch,
				SW1 : 0Byte 0-3bit�� 2�� ��ȭ
				SW2 : 
				SW3 : 
				SW4 : 2Byte 3bit�� 1�� ��ȭ
				SW5 : 0Byte 0-3bit�� 1�� ��ȭ
				SW6 : 0Byte 0-3bit�� 3���� ��ȭ
				
				
				�̸����������?ȭ���� ������ ��ü ȭ���� CAN[CO 185050E4 ]0Byte 0-3bit�� 8�� �ٲ����?�� ��ȭ ȭ������ ��ȯ�Ѵ�.				
				CAN[CO 185050E4 ]0Byte 4-7bit�� 0(�׼� ���? �� ��, CAN[CO 185050E4 ]1Byte�� ��ȭ�ص� �����Ѵ�.
		


			*/
				char JD_Buf[10] = {0,};	// JPHong 2024-01-18
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				static uint8_t c1Byte = 0x00;
				static uint8_t c3Byte0bit = 0x00;
				static uint8_t c3Byte1bit = 0x00;
				static uint8_t c3Byte2bit = 0x00;
				static uint8_t c3Byte3bit = 0x00;
				static uint8_t c3Byte4bit = 0x01;
				static uint8_t c2Byte6bit = 0x00;
				
				static uint8_t c0Byte3210bit = 0x00;
				static uint8_t c2Byte3bit = 0x00;
				static uint32_t JogWheelCnt = 0;
				static uint8_t SW_Off_Flag=0;
				
				
				JD_Buf[0]=0;
				JD_Buf[1]=0;
				
			//	printf("[CO] [%x] ,%d\n",sData[0],MainBtFlag);
				if (MainBtFlag==0)
				{
					if ((sData[0]&0xF0)!=0)
					{
						//	FF==>0 , 0==>�϶�  ó������  
						if(c1Byte != sData[1])
						{
							if ((abs(c1Byte-sData[1])>=230))
							{
								if(c1Byte>230)
								{
									JD_Buf[0] = JOG_DIAL_ROT_RIGHT;
								}
								else
								{
									JD_Buf[0] = JOG_DIAL_ROT_LEFT;
								}
							}
							else
							{
								if(c1Byte > sData[1])
								{
									JD_Buf[0] = JOG_DIAL_ROT_LEFT;
								}
								else if(c1Byte < sData[1])
								{
									JD_Buf[0] = JOG_DIAL_ROT_RIGHT;
								}
							}
	//						printf("[CO] [%d][%d][%d]\r\n",c1Byte,sData[1],JD_Buf[0]);
							if ((JogWheelCnt+TIME_CNT_50MS)<gMainTimerCnt_10m)
							{
								JogWheelCnt=gMainTimerCnt_10m;
								ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY0, JD_Buf);
							}
						}
						c1Byte = sData[1];
					}
	
					if (c0Byte3210bit!=((sData[0]>>0)&0x0F))
					{
						c0Byte3210bit=((sData[0]>>0)&0x0F);
						if (c0Byte3210bit==0x01)
						{
							JD_Buf[0] = JOG_DIAL_SW_HOME;
							SW_Off_Flag = JOG_DIAL_SW_HOME;
							ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY2, JD_Buf);
						}
						else if (c0Byte3210bit==0x02)
						{
							if ((Main_Disp_Mode!=MAIN_MODE_AIRCON)&&(Main_Disp_Mode!=MAIN_MODE_PW)&&(Main_Disp_Mode!=MAIN_MODE_USERINFO))	// JPHong 2024-03-06
							{
								if((Main_Disp_Mode==MAIN_MODE_MENU)||(Main_Disp_Mode==MAIN_MODE_BACK))
								{
									SW_Off_Flag = 0;
									GotoMain(MAIN_MODE_AIRCON); 
								}
								else
								{
									JD_Buf[0] = JOG_DIAL_SW_ARICON;
									SW_Off_Flag = JOG_DIAL_SW_ARICON;
									ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY2, JD_Buf);
								}
							}
						}
						else if (c0Byte3210bit==0x03)
						{
							if ((Main_Disp_Mode!=MAIN_MODE_RADIO)&&(Main_Disp_Mode!=MAIN_MODE_PW)&&(Main_Disp_Mode!=MAIN_MODE_USERINFO))	// JPHong 2024-03-06
							{
								if((Main_Disp_Mode==MAIN_MODE_MENU)||(Main_Disp_Mode==MAIN_MODE_BACK))
								{
									SW_Off_Flag = 0;
									GotoMain(MAIN_MODE_RADIO);
								}
								else
								{
									JD_Buf[0] = JOG_DIAL_SW_AUDIO;
									SW_Off_Flag = JOG_DIAL_SW_AUDIO;
									ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY2, JD_Buf);
								}
							}
						}
/*						
						else if (c0Byte3210bit==0x04)
						{
							if ((Main_Disp_Mode!=MAIN_MODE_CAMERA)&&(Main_Disp_Mode!=MAIN_MODE_PW)&&(Main_Disp_Mode!=MAIN_MODE_USERINFO))	// JPHong 2024-03-06
							{
								if((Main_Disp_Mode==MAIN_MODE_MENU)||(Main_Disp_Mode==MAIN_MODE_BACK))
								{
									GotoMain(MAIN_MODE_CAMERA); 
								}
								else
								{
									char JD_Buf[10] = {0,};	// JPHong 2024-01-18
									JD_Buf[0] = 0;
									MainCamera("CO", JD_Buf);
								}
							}					
							SW_Off_Flag = 0;
						}
*/						
						else
						{
							switch (SW_Off_Flag)
							{
								case JOG_DIAL_SW_HOME:
									JD_Buf[0] = JOG_DIAL_SW_HOME_OFF;
									SW_Off_Flag = 0;
								break;
								case JOG_DIAL_SW_ARICON:
									JD_Buf[0] = JOG_DIAL_SW_ARICON_OFF;
									SW_Off_Flag = 0;
								break;
								case JOG_DIAL_SW_AUDIO:
									JD_Buf[0] = JOG_DIAL_SW_AUDIO_OFF;
									SW_Off_Flag = 0;
								break;
								case 0x00: // SW NONE
									JD_Buf[0] = JOG_DIAL_KEY_NONE;
									SW_Off_Flag = 0;
								break;
							}
							InitJogIncDecData_AC();
							InitJogIncDecData_RD();
							ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY2, JD_Buf);
						}
					}
	
					if (c2Byte3bit!=((sData[2]>>3)&0x01))
					{
						c2Byte3bit=((sData[2]>>3)&0x01);
						if (c2Byte3bit==0x01)
						{
							c2Byte6bit=((sData[2]>>6)&0x01);
							if (c2Byte6bit==0x01)
							{
								JD_Buf[0] = JOG_DIAL_KEY_DEEP;
								SW_Off_Flag = JOG_DIAL_KEY_DEEP;
								ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY1, JD_Buf);
							}
							else
							{
								JD_Buf[0] = JOG_DIAL_SW_ESC_ON;
								SW_Off_Flag = JOG_DIAL_SW_ESC_ON;
								ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY2, JD_Buf);
							}
						}
						else
						{
							JD_Buf[0] = JOG_DIAL_SW_ESC_ON_OFF;
							SW_Off_Flag = 0;
	//						InitJogIncDecData_AC();
	//						InitJogIncDecData_RD();
							ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY2, JD_Buf);
						}
					}
	
					if (c2Byte6bit!=((sData[2]>>6)&0x01))
					{
	//					printf("[CO] [%x][%x][%x][%x] %d,%d\n",sData[0],sData[1],sData[2],c2Byte6bit,MICOM_RX.Rx_Write,MICOM_RX.Rx_Read);
						c2Byte6bit=((sData[2]>>6)&0x01);
						if (c2Byte6bit==0x01)
						{
							c2Byte3bit=((sData[2]>>3)&0x01);
							if (c2Byte3bit==0x01)
							{
								JD_Buf[0] = JOG_DIAL_KEY_DEEP;
								SW_Off_Flag = JOG_DIAL_KEY_DEEP;
								ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY1, JD_Buf);
							}
							else
							{
								JD_Buf[0] = JOG_DIAL_KEY_ENTER_ON;
								SW_Off_Flag = JOG_DIAL_KEY_ENTER_ON;
								ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY1, JD_Buf);
							}
						}
						else
						{
							SW_Off_Flag = 0;
							Clear_ACRD_TimeCnt();
							JD_Buf[0] = JOG_DIAL_KEY_ENTER_OFF;
							ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY1, JD_Buf);
						}
					}
	
					if (c3Byte3bit!=((sData[3]>>3)&0x01))
					{
						c3Byte3bit=((sData[3]>>3)&0x01);
						if (c3Byte3bit==0x01)
						{
							JD_Buf[0] = JOG_DIAL_KEY_DOWN;
							SW_Off_Flag = JOG_DIAL_KEY_DOWN;
						}
						else
						{
							SW_Off_Flag = 0;
							Clear_ACRD_TimeCnt();
							JD_Buf[0] = JOG_DIAL_KEY_DOWN_OFF;	//JPHong 0704
						}
						ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY1, JD_Buf);
					}
	
					if (c3Byte0bit!=((sData[3]>>0)&0x01))
					{
						c3Byte0bit=((sData[3]>>0)&0x01);
						if (c3Byte0bit==0x01)
						{
							JD_Buf[0] = JOG_DIAL_KEY_LEFT;
							SW_Off_Flag = JOG_DIAL_KEY_LEFT;
						}
						else
						{
							SW_Off_Flag = 0;
							Clear_ACRD_TimeCnt();
							JD_Buf[0] = JOG_DIAL_KEY_LEFT_OFF;	//JPHong 0704
						}
						ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY1, JD_Buf);
					}
					if (c3Byte1bit!=((sData[3]>>1)&0x01))
					{
						c3Byte1bit=((sData[3]>>1)&0x01);
						if (c3Byte1bit==0x01)
						{
							JD_Buf[0] = JOG_DIAL_KEY_RIGHT;
							SW_Off_Flag = JOG_DIAL_KEY_RIGHT;
						}
						else
						{
							SW_Off_Flag = 0;
							Clear_ACRD_TimeCnt();
							JD_Buf[0] = JOG_DIAL_KEY_RIGHT_OFF;	//JPHong 0704 > jhlee 231010
						}
						ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY1, JD_Buf);
					}
					if (c3Byte2bit!=((sData[3]>>2)&0x01))
					{
						c3Byte2bit=((sData[3]>>2)&0x01);
						if (c3Byte2bit==0x01)
						{
							JD_Buf[0] = JOG_DIAL_KEY_UP;
							SW_Off_Flag = JOG_DIAL_KEY_UP;
						}
						else
						{
							SW_Off_Flag = 0;
							Clear_ACRD_TimeCnt();
							JD_Buf[0] = JOG_DIAL_KEY_UP_OFF;	//JPHong 0704
						}
						ituSceneSendEvent(&theScene, EVENT_CUSTOM_KEY1, JD_Buf);
					}
	
					if ((Main_Disp_Mode != MAIN_MODE_PW)&&(Main_Disp_Mode != MAIN_MODE_USERINFO)&&(Main_Disp_Mode != MAIN_MODE_NORMAL)&&(Main_Disp_Mode != MAIN_MODE_ENGN_COOL)&&(Main_Disp_Mode != MAIN_MODE_IMMOBI))
					{
						if (Phone0Byte!=((sData[0]>>0)&0x0F))
						{
							char JD_Buf[10] = {0,};
							Phone0Byte=((sData[0]>>0)&0x0F);
							if ((Phone0Byte==8)&&(RDV.RDPhone!=5))
							{							
								if ((Main_Disp_Mode!=MAIN_MODE_PHONE))
								{
									if (RDV.RDPhone==0)
										RDV.RDPhone=4;
									Phone_Time=PHONE_TIME;								
									if((Main_Disp_Mode==MAIN_MODE_MENU)||(Main_Disp_Mode==MAIN_MODE_BACK))
									{
										GotoMain(MAIN_MODE_PHONE); 
									}
									else
									{
										//Main_Disp_Mode = MAIN_MODE_PHONE;
										
										JD_Buf[0] = 0;
										JD_Buf[1] = 0;
										Main_RadioBt_Func("CO", JD_Buf);
									}
								}
							}
						}					
						else if (c3Byte4bit!=((sData[3]>>4)&0x01))
						{
							c3Byte4bit=((sData[3]>>4)&0x01);
							if (c3Byte4bit==0x01)
							{
								GotoMain(MAIN_MODE_NORMAL);
							}
						}
					}
				}	//if (MainBtFlag==0)
			}
			break;
			
			
			case EI:	// 185009E4	
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				if (((sData[4]<<8) | sData[3])==0xFFFF)
					Main_EI_34BValue = 0xFFFF;
				else
				{
					Main_EI_34BValue = ((sData[4]<<8) | sData[3]);//100;  //0~120  JPHong 0823 
				}
//				printf("[EI][%d][%d]\n",MICOM_RX.Rx_Read,Main_EI_34BValue);
				
				if (sData[5]==0xFF)
					Main_EI_5BValue = 0xFFFF;
				else if (sData[5]>100)
					Main_EI_5BValue = 100;
				else
					Main_EI_5BValue = sData[5];
				Main_EI_5BTime = gMainTimerCnt_10m;
				MRV.PM = sData[2];
				MRV.PMTime = gMainTimerCnt_10m;
				MRV.EngRPM = sData[0];
				MRV.EngRPMTime = gMainTimerCnt_10m;
				MRV.EngLd = sData[7];
				MRV.EngLdTime = gMainTimerCnt_10m;
				MRV.ASH = sData[6];
				MRV.ASHTime = gMainTimerCnt_10m;
				
				//M5_PM = MRV.PM;		// modify 230517 jhlee
				//M5_ASH = MRV.ASH;
			}
			break;
			
			case AT1T1I:	// 18FE563D
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if(sData[0] == 0xFF)
					M5_Urea = 0xFFFF;
				else if(sData[0] > 0xFA)
					M5_Urea = 100;
				else 
					M5_Urea = ((sData[0]*4)+5)/10;// JPHong 0829

				if(sData[1] == 0xFF)
					DTVal.AT1T1I_1Byte = 0xFF;
				else if(sData[1] > 0xFA)
					DTVal.AT1T1I_1Byte = 0xFA;
				else 
					DTVal.AT1T1I_1Byte = sData[1];	// JPHong 0823
			
				M5_UTemp = DTVal.AT1T1I_1Byte;
				M5_UTemp_Timer = gMainTimerCnt_10m;
				M5_Urea_Timer = gMainTimerCnt_10m;
#if 1	// JPHong 0829
	gMB_IO_Value.AT1T1I_0Byte = M5_Urea;
	gMB_IO_Value.AT1T1I_1Byte = M5_UTemp;
#else
				if (sData[0]==0xFF) 
					gMB_IO_Value.AT1T1I_0Byte = 0xFF;
				else if(sData[0] > 0xFA)
					gMB_IO_Value.AT1T1I_0Byte = 0xFA;// JPHong 0823
				else
					gMB_IO_Value.AT1T1I_0Byte = sData[0];//chpark.

				if (sData[1]==0xFF) 
					gMB_IO_Value.AT1T1I_1Byte = 0xFF;
				else if(sData[1] > 0xFA)
					gMB_IO_Value.AT1T1I_1Byte = 0xFA;// JPHong 0823
				else
					gMB_IO_Value.AT1T1I_1Byte = sData[1];//chpark.
#endif					
			}
			break;
			
			case VEP1:		// 18FEF7E4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint32_t tBatt = 0;
				
				if(((sData[5]<<8)|sData[4]) == 0xFFFF)
					tBatt = 0xFFFFFFFF;
				else if(((sData[5]<<8)|sData[4]) > 0xFAFF)
					tBatt = 32128;
				else
				{
					tBatt = (sData[5]<<8)|sData[4];
					tBatt = ((tBatt * 5)+5)/10;  // JPHong  0823  tBatt = (tBatt * 5) / 10;
				}
				
				M5_Batt = tBatt;			// 
				M5_Batt_Timer = gMainTimerCnt_10m;
			}
			break;
			
			case DD:	// 18FEFCE4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (sData[1]==0xFF)
					Main_DD_1BValue = 0xFFFF;
				else
				{
					Main_DD_1BValue = ((sData[1])*4);	//Main_DD_1BValue = (sData[1]*40+90)/100;//		((sData[1])*4+5)/10;
				}				

				if(sData[1] == 0xFF)
					M5_Fuel = 0xFFFF;
				else if(sData[1] > 0xFA)
					M5_Fuel = 100;
				else 
					M5_Fuel = ((sData[1]*4)+5)/10;// JPHong 0829

				M5_Fuel_Timer = gMainTimerCnt_10m;
			}
			break;
			
			case SI:	// 18500DE4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);


				if(sData[0] == 0xFF)
					DTVal.SI_0Byte = 0xFF;
				else if(sData[0] > 0xFA)
					DTVal.SI_0Byte = 0xFA;
				else 
					DTVal.SI_0Byte = sData[0];		// JPHong 0823
					
									
				M5_OTemp = DTVal.SI_0Byte;		//menu5 �� �� �� �� �� 
				M5_OTemp_Timer = gMainTimerCnt_10m;
				
				if (((sData[7]<<8) | sData[6])==0xFFFF)
					Main_SI_67BValue = 0xFFFF;
				else
				{
					Main_SI_67BValue = ((sData[7]<<8) | sData[6]);  //0~12000
				}
				Main_SI_67BTime = gMainTimerCnt_10m;
				MRV.LiftLd = sData[1];
				MRV.LiftLdTime = gMainTimerCnt_10m;

			
				if (sData[3]==0xFF) 
					gMB_IO_Value.SI_3Byte = 0xFF;
				else
					gMB_IO_Value.SI_3Byte = sData[3];//chpark.
#if 1
				gMB_IO_Value.SI_0Byte = DTVal.SI_0Byte;//chpark.
#else
				if (sData[0]==0xFF) 
					gMB_IO_Value.SI_0Byte = 0xFF;
				else
					gMB_IO_Value.SI_0Byte = sData[0];//chpark.
#endif	

				if (((sData[5]<<8) | sData[4])==0xFFFF)
					gMB_IO_Value.SI_45Bytes = 0xFFFF;
				else
				{
					gMB_IO_Value.SI_45Bytes = ((sData[5]<<8) | sData[4]);  //chpark.
				}

				gMB_IO_Value.SI_3Time = gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.SI_0Time = gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.SI_45Time= gMainTimerCnt_10m;//chpark.07.05

//				printf("[UART]SI=%d %d %d\n", gMB_IO_Value.SI_3Byte,gMB_IO_Value.SI_0Byte, gMB_IO_Value.SI_45Bytes);

				
				
			}
			break;
			case MI:	// 185000E4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				
				if ((Main_Disp_Mode!=MAIN_MODE_PW)&&(Main_Disp_Mode!=MAIN_MODE_USERINFO))	//#10066  New....JPHong 2023-12-23 Page21 ��26   V12 2024-02-06
				{
					if (gBuzzerMode==7)
					{
						TempgBuzzerMode = sData[7];// 0 ~ 7
					}
					else if ((TempgBuzzerMode==7)&&(sData[7]==7))
					{
					}
					else
					{
						gBuzzerMode = sData[7];// 0 ~ 7
						TempgBuzzerMode = gBuzzerMode;
					}
										
					if (gBuzzerMode<=7)
					{
						gBuzzerTimerCnt_10m = gMainTimerCnt_10m;
					}
			//		printf("[MI] gBuzzerMode : [%d] [%d]\n", gBuzzerMode,sData[7]);
					
				}

				
				MI_Time = gMainTimerCnt_10m;
#if (DBG_MSG==1)				
				printf("[MI] gBuzzerMode : [%d] \n", gBuzzerMode);
				printf("[MI] ModelType : [%d] \n", ModelType);
#endif				
			}
			break;			
			case EEC1:	//0CF00400
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				
				if (((sData[4]<<8) | sData[3])==0xFFFF)
				{
					DTVal.EEC1_34Bytes = 0xFFFFFFFF;
				}
				else
				{
					uint32_t EEC34Bytes;
					EEC34Bytes = (sData[4]<<8 | sData[3]) * 125;  // JPHong 08-29
					EEC34Bytes = (EEC34Bytes+500)/1000;  // JPHong 08-29
					DTVal.EEC1_34Bytes += EEC34Bytes; // JPHong 08-29
					DTVal.EEC1_34Cnt++;
				}
				M5_RPM = DTVal.EEC1_34Bytes;		//JPHong 08-27			
				DTVal.EEC1_34Time = gMainTimerCnt_10m;
				M5_RPM_Timer = gMainTimerCnt_10m;
#if 1		// JPHong 08-29
				gMB_IO_Value.EEC1_34Bytes = M5_RPM;
#else				
				//chpark.======
				if (((sData[4]<<8) | sData[3])==0xFFFF)
					gMB_IO_Value.EEC1_34Bytes = 0xFFFF;
				else
				{
					gMB_IO_Value.EEC1_34Bytes = ((sData[4]<<8) | sData[3]);  
				}
#endif
				gMB_IO_Value.EEC1_34Time = gMainTimerCnt_10m;//chpark.07.05
				//=============
			}
			break;

			case ET1:		// 18FEEE00
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				

				if(sData[0] == 0xFF)
					DTVal.ET1_0Byte = 0xFF;
				else if(sData[0] > 0xFA)
					DTVal.ET1_0Byte = 0xFA;
				else 
					DTVal.ET1_0Byte = sData[0];		// JPHong 0823
					
				if(sData[1] == 0xFF)
					DTVal.ET1_1Byte = 0xFF;
				else if(sData[1] > 0xFA)
					DTVal.ET1_1Byte = 0xFA;
				else 
					DTVal.ET1_1Byte = sData[1];		// JPHong 0823
					
				
				M5_WTemp = DTVal.ET1_0Byte;
				M5_WTemp_Timer = gMainTimerCnt_10m;
				
				/*
				//chpark.===
				if (((sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.ET1_23Bytes = 0xFFFF;
				else if(sData[3]==0xFF)//chpark.10.19
					gMB_IO_Value.ET1_23Bytes = 0xFFFF;
				else
				{
					gMB_IO_Value.ET1_23Bytes = (sData[3]<<8 | sData[2]);
				}
				*/
#if 1		// JPHong 08-29
				gMB_IO_Value.ET1_0Byte = DTVal.ET1_0Byte;
				gMB_IO_Value.ET1_1Byte = DTVal.ET1_1Byte;//chpark
#else

				if (sData[0]==0xFF)// eng coolant(water) temp.
				{
					gMB_IO_Value.ET1_0Byte = 0xFF;
				}
				else if(sData[0] > 0xFA)
					gMB_IO_Value.ET1_0Byte = 0xFA;
				else
				{
					gMB_IO_Value.ET1_0Byte = sData[0];//chpark
				}

				if (sData[1]==0xFF)// fuel temp.
				{
					gMB_IO_Value.ET1_1Byte = 0xFF;
				}
				else if(sData[1] > 0xFA)
					gMB_IO_Value.ET1_1Byte = 0xFA;
				else 
				{
					gMB_IO_Value.ET1_1Byte = sData[1];//chpark
				}
#endif				
				//=====
				gMB_IO_Value.ET1_0Time 	= gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.ET1_1Time 	= gMainTimerCnt_10m;//chpark.07.05
				//gMB_IO_Value.ET1_23Time = gMainTimerCnt_10m;//chpark.07.05

			}	
			break;
			
			case ACAS:	// 18FFA019
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				ACV.AirMode=sData[3]&0x03; 		// In Out
    			ACV.ACTemp=sData[0];			//
    			ACV.ACOnOff=(sData[3]>>3)&0x01;
    			
   	 			ACV.ACLevel=sData[1]&0x0F;		// 0-5
   	 			if (ACV.ACLevel>=0xF)
   	 				ACV.ACLevel = 0xF;
   	 			else if (ACV.ACLevel>6)
   	 				ACV.ACLevel = 0;
   	 				
    			ACV.ACAuto=(sData[3]>>2)&0x01;
    			ACV.ACDir=(sData[1]>>4)&0x0F;	// 0-4
//   	 			if (ACV.ACDir>4)
//   	 				ACV.ACDir = 4;

				//===========
				if (sData[0]==0xFF)//
				{
					gMB_IO_Value.ACAS_0Byte = 0xFF;
				}
				else
				{
					gMB_IO_Value.ACAS_0Byte = sData[0];//chpark
				}

				if (sData[2]==0xFF)// 
				{
					gMB_IO_Value.ACAS_2Byte = 0xFF;
				}
				else
				{
					gMB_IO_Value.ACAS_2Byte = sData[2];//chpark
				}

				if (sData[4]==0xFF)//
				{
					gMB_IO_Value.ACAS_4Byte = 0xFF;
				}
				else
				{
					gMB_IO_Value.ACAS_4Byte = sData[4];//chpark
				}
				//
				gMB_IO_Value.ACAS_0Time 	= gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.ACAS_2Time 	= gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.ACAS_4Time 	= gMainTimerCnt_10m;//chpark.07.05
				//===========		
			}
			break;
			case RI1:	// 18FF704C
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				RDV.RDOnOff=sData[0]&0x01;
				RDV.RDMute=(sData[0]>>1)&0x01;
				RDV.RDStereo=(sData[0]>>5)&0x01;
				RDV.RDSelCh=(sData[1]>>4)&0x0F;
				RDV.RDFreq = sData[2];
				RDV.RDStep = (sData[0]>>2)&0x01;
//				printf("RDV.RDStep:%d \n",RDV.RDStep);
				switch ((sData[1]>>4)&0x0F)
				{
					case 0:
					{
						RDV.RDMode = 0;	//AM
					}
					break;
					case 1:
					{
						RDV.RDMode = 1;	//FM
					}
					break;
					case 3:
					{
						if ((TempRI2>>0)&0x01)
							RDV.RDMode = 1;	//FM
						else
							RDV.RDMode = 0;	//AM
					}
					break;
					case 4:
					{
						if ((TempRI2>>1)&0x01)
							RDV.RDMode = 1;	//FM
						else
							RDV.RDMode = 0;	//AM
					}
					break;
					case 5:
					{
						if ((TempRI2>>2)&0x01)
							RDV.RDMode = 1;	//FM
						else
							RDV.RDMode = 0;	//AM
					}
					break;
					case 6:
					{
						if ((TempRI2>>3)&0x01)
							RDV.RDMode = 1;	//FM
						else
							RDV.RDMode = 0;	//AM
					}
					break;
					case 7:
					{
						if ((TempRI2>>4)&0x01)
							RDV.RDMode = 1;	//FM
						else
							RDV.RDMode = 0;	//AM
					}
					break;
					case 8:
					{
						if ((TempRI2>>5)&0x01)
							RDV.RDMode = 1;	//FM
						else
							RDV.RDMode = 0;	//AM
					}
					break;
					case 9:
					{
						if ((TempRI2>>6)&0x01)
							RDV.RDMode = 1;	//FM
						else
							RDV.RDMode = 0;	//AM
					}
					break;
					case 10:
					{
						if ((TempRI2>>7)&0x01)
							RDV.RDMode = 1;	//FM
						else
							RDV.RDMode = 0;	//AM
					}
					break;
					case 11:
					{
						RDV.RDMode = 3;	//DAB
					}
					break;
					case 13:
					{
						RDV.RDMode = 2;	//BT
					}
					break;
				}
			    RDV.RDTone=sData[4];
			    RDV.RDVol=sData[3];
	//			printf("[RI1] RDV.RDOnOff : [%x] \n", RDV.RDOnOff);
	//			printf("[RI1] RDV.RDStereo : [%x] \n", RDV.RDStereo);
	//			printf("[RI1] RDV.RDFreq : [%x] \n", RDV.RDFreq);
	//			printf("[RI1] RDV.RDMode : [%x] \n", RDV.RDMode);
	//			printf("[RI1] TempRI2 : [%x] \n", TempRI2);
	
				Radio_Region = (sData[1]&0x0F);		// add 230608 jhlee
				if (Radio_Region>3)		//JPHong 2023-078-18
					Radio_Region = 0;
			}
			break;	
			case RI2:	// 18FF714C
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				TempRI2=sData[0];
			}
			break;
			case RI3:	// 18FF724C
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				TempRI3_7ByteH=((sData[7]>>4)&0x0F);
				TempRI3_7ByteL=((sData[7]>>0)&0x0F);
				RDV.RDLOUD=(sData[0]>>7)&0x01;
				RDV.DABSelCh=(sData[1]>>4)&0x0F;
				
				if (((sData[1]>>2)&0x03)==0x01)
					RDV.RDScan=1;
				else
					RDV.RDScan=0;
				
				//chpark.09.11
				//In the case of DAB2 screen, the selected pty is immediately displayed.
				//If it is not a DAB2 screen, only the pty value is saved.

				//If there is no service channel, do not select pty.
				// �ؼ��� ���� ��0���� PTY�� [RI3 18FF724C ] 2Byte 2-7bit�� ���� ��ġ�ص� ���û��·� ���� ���� ��
				int tmpPTYNo = (sData[2]>>2)&0x3F;
				//if(RDV.DABServiceCnt[tmpPTYNo]>0)//JPHong.06.21//chpark.10.16
				//{
				//chpark.10.16
				if(RDV.DABServiceCnt[tmpPTYNo]>0)
				{
					
				}
				else
				{
					//if no service channel is selected, remove Current PTY.
					tmpPTYNo=-2;
				}

				if (Main_Disp_Mode==MAIN_MODE_DAB2)
				{	
						//In DAB2 mode
					if(tmpPTYNo!=RDV.PTYNo)
					{
						RDV.PTYNo=tmpPTYNo;
						//Show PTY Num in DAB2 List.
						MainMode_DAB2_PTY_ON(1,DISP_ON);
						//printf("[RI3] [In DAB2Mode]RDV.PTYNo : [%d] [%d] [%d] \n", RDV.PTYNo,TempRI3_7ByteH,TempRI3_7ByteL);
						DAB2_DataChkTime = gMainTimerCnt_10m;//chpark.10.11
					}
					else
					{

					}
				}
				else
				{
					//Not In DAB2 mode
					//No Show PTY Num in DAB2 List.
					if(tmpPTYNo!=RDV.PTYNo)
					{
						RDV.PTYNo=tmpPTYNo;
						DAB2_DataChkTime = gMainTimerCnt_10m;//chpark.10.11
					}
						//printf("[RI3] [Not in DAB2Mode] RDV.PTYNo : %d \n", RDV.PTYNo);
				}

					
				//}
				//else
				//{
					
				//}
				
				//DAB2_DataChkTime = gMainTimerCnt_10m;//chpark.09.26
//				printf("[RI3] RDV.PTYNo : [%d] [%d] [%d] \n", RDV.PTYNo,TempRI3_7ByteH,TempRI3_7ByteL);
			}
			break;
			case RI4:	// 18FF734C
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				static uint16_t tPacketNum = 0;	// ����ä���� ��Ŷ �ѹ� 
				static uint16_t tListNum = 0;		// ä���� ���� ä��    strlen(const char* str)
				static uint16_t tPTY_No = 0;		// PTY ���?ä��
				uint16_t tSize = 0;
				uint8_t tmp_CharacterSet = 0; //chpark.09.09//0BBYTE:6-7bit
				
				if (TempRI3_7ByteH==8)		// DAB3  
				{
					tmp_CharacterSet = (sData[0]>>6)&0x03;//chpark.09.11 //00:Complete 01:UCS-2 10:UTF-8 11:NOSHOW
//					printf("[RI4]tmp_CharacterSet = %d\n",tmp_CharacterSet);
					if(tmp_CharacterSet!=3)
					{
						//printf("[RI4][RAW]%x %x %x %x %x %x %x %x\n",sData[0],sData[1],sData[2],sData[3],sData[4],sData[5],sData[6],sData[7]);
						if ((sData[0]&0x0F)==0)
						{
							TempRI4_0Byte=0;
							TempRI4_Cnt=0;
							
							tPacketNum = (sData[0]&0x0F);
							tListNum = ((sData[1]>>7&0x01)<<8)|sData[2];
							tPTY_No = sData[1]&0x7F;
							memset (DAB3PTYNo[tListNum],0,sizeof(DAB3PTYNo[tListNum]));
							
							memcpy (DAB3PTYNo[tListNum],&sData[3],5);
							
							//printf("[RI4]1 tPacketNum[%d] , tListNum[%d] , tPTY_No[%d] , [%d]\n", tPacketNum,tListNum,tPTY_No);
							//printf("[RI4]2 Count[%d] , [%s]\n",strlen(DAB3PTYNo[tListNum]),DAB3PTYNo[tListNum]);
							
							//printf("[RI4][1]%d_%d Length=[%d] , [%s]\n",tListNum,tPacketNum,strlen(DAB3PTYNo[tListNum]),DAB3PTYNo[tListNum]);
						}
						else if(((sData[0]&0x0F)==(tPacketNum+1)) && ((((sData[1]>>7&0x01)<<8)|sData[2])==tListNum) && ((sData[1]&0x7F)==tPTY_No))
						{
							tPacketNum = (sData[0]&0x0F);
							memcpy (DAB3PTYNo[tListNum]+strlen(DAB3PTYNo[tListNum]),&sData[3],5);
							
							//printf("[RI4]3 tPacketNum[%d] , tListNum[%d] , tPTY_No[%d] , [%d]\n", tPacketNum,tListNum,tPTY_No);
							//printf("[RI4]4 Count[%d] , [%s]\n",strlen(DAB3PTYNo[tListNum]),DAB3PTYNo[tListNum]);
							//printf("[RI4][2]%d_%d Length=[%d] , [%s]\n",tListNum,tPacketNum,strlen(DAB3PTYNo[tListNum]),DAB3PTYNo[tListNum]);
						}
						else if( tPacketNum==(sData[0]&0x0F) )
						{
							//printf("[RI4]%d_%d : The Same Packet==",tListNum,tPacketNum);
						}

						/*
						//chpark.12.04 chpark
						//if same packet receive, below code will cause an error.
						else
						{
							tPacketNum = 0;		// ����ä���� ��Ŷ �ѹ� 
							tListNum = 0;		// ä���� ���� ä��    strlen(const char* str)
							tPTY_No = 0;		// PTY ���?ä��						
						}
						*/

						DAB3_DataChkTime = gMainTimerCnt_10m;//chpark.07.18
						//printf("Call MainMode_DAB2_Reflesh==%d \n",DAB3_DataChkTime)
						//MainMode_DAB2_Reflesh();//chpark.10.17
					}
					else
					{
//						printf("No Show Character Set : tmp_CharacterSet=%d\n",tmp_CharacterSet);
					}

				}
				else if (TempRI3_7ByteH==7)		// ��۸�? 
				{
//					printf("\n\n[RI4] RDV.RDPrg :'%s' [%d] [%d] [%d] [%d] [%d]\n\n\n", RDV.RDPrg,TempRI3_7ByteL,tTempRI3_7ByteL,TempRI4_0Byte,bTempRI4_0Byte,TempRI4_Cnt);
					//if((TempRI3_7ByteL==0)&&(TempRI3_7ByteL!=tTempRI3_7ByteL))
					if ((((sData[0]&0x0F)==0)&&((sData[1]==0)))||(((sData[0]>>6)&0x03)==0x03))
					{
						TempRI4_0Byte=0;
						TempRI4_Cnt=0;
						memset (&RDV.RDPrg[0],0,sizeof(RDV.RDPrg));
						sprintf(RDV.RDPrg, " No Information ");
					}
					else if ((TempRI3_7ByteL==0)||((sData[0]&0x0F)==0))
					{
						TempRI4_0Byte=0;
						TempRI4_Cnt=0;
						memset (&RDV.RDPrg[0],0,sizeof(RDV.RDPrg));
						TempRI4_0Byte=TempRI3_7ByteL+1;
						if (TempRI4_Cnt<112)	// JPHong 2024-01-16
						{
							memcpy (&RDV.RDPrg[TempRI4_Cnt],&sData[1],7);
							TempRI4_Cnt = TempRI4_Cnt+7;
						}						
						tTempRI3_7ByteL=TempRI3_7ByteL;
					}
					else if (((TempRI3_7ByteL)==(sData[0]&0x0F))&&(TempRI4_0Byte==TempRI3_7ByteL))
					{
						if (TempRI3_7ByteL==0)
						{
							memset (&RDV.RDPrg[0],0,sizeof(RDV.RDPrg));
							TempRI4_Cnt = 0;
						}
						TempRI4_0Byte=TempRI3_7ByteL+1;
						if (TempRI4_Cnt<112)	// JPHong 2024-01-16
						{
							memcpy (&RDV.RDPrg[TempRI4_Cnt],&sData[1],7);
							TempRI4_Cnt = TempRI4_Cnt+7;
						}
					}
#if 0					
					else
					{
						TempRI4_0Byte=0;
						TempRI4_Cnt=0;
						memset (&RDV.RDPrg[0],0,sizeof(RDV.RDPrg));
					}
#endif
//					printf("\n\n[RI4] RDV.RDPrg :'%s' [%d] [%d] [%d]\n\n\n", RDV.RDPrg,TempRI3_7ByteL,bTempRI4_0Byte,TempRI4_Cnt);
				}
				else if (TempRI3_7ByteH==6)		// ��۱�?
				{				
					if ((((sData[0]&0x0F)==0)&&((sData[1]==0)))||(((sData[0]>>6)&0x03)==0x03))
					{
						bTempRI4_0Byte=0;
						bTempRI4_Cnt=0;
						memset (&RDV.RDBCS[0],0,sizeof(RDV.RDBCS));
						sprintf(RDV.RDBCS, " No Information ");
					}
					else if((TempRI3_7ByteL==0)||((sData[0]&0x0F)==0))
					{
						bTempRI4_0Byte=0;
						bTempRI4_Cnt=0;
						memset (&RDV.RDBCS[0],0,sizeof(RDV.RDBCS));
						bTempRI4_0Byte=TempRI3_7ByteL+1;
						memcpy (&RDV.RDBCS[bTempRI4_Cnt],&sData[1],7);
						bTempRI4_Cnt = bTempRI4_Cnt+7;
						tbTempRI3_7ByteL=TempRI3_7ByteL;
					}
					else if (((TempRI3_7ByteL)==(sData[0]&0x0F))&&(bTempRI4_0Byte==TempRI3_7ByteL))
					{
						if (TempRI3_7ByteL==0)
						{
							memset (&RDV.RDBCS[0],0,sizeof(RDV.RDBCS));
							bTempRI4_Cnt = 0;
						}
						bTempRI4_0Byte=TempRI3_7ByteL+1;
						memcpy (&RDV.RDBCS[bTempRI4_Cnt],&sData[1],7);
						bTempRI4_Cnt = bTempRI4_Cnt+7;
					}
#if 0					
					else
					{
						bTempRI4_0Byte=0;
						bTempRI4_Cnt=0;
						memset (&RDV.RDBCS[0],0,sizeof(RDV.RDBCS));
					}
#endif
	//				printf("\n\n[RI4] RDV.RDBCS :'%s' [%d] [%d] [%d]\n\n\n", RDV.RDBCS,TempRI3_7ByteL,bTempRI4_0Byte,TempRI4_Cnt);
				}
				else if ((TempRI3_7ByteH==0x0A)&&(TempRI3_7ByteL==0x03))	//DAB Freq
				{
					RDV.DABFreq = (((sData[0]>>4)&0x0F)-5)*4+((sData[0]>>1)&0x07);
					if (RDV.DABFreq>37)
					{
						RDV.DABFreq = 0;
					}
	//				printf("\n\n[RI4] RDV.DABFreq = 0; :[%d] [0x%X] [0x%X] [%d] [%d]\n\n\n", RDV.DABFreq,(((sData[0]>>4)&0x0F)-5)*4,((sData[0]>>1)&0x03),TempRI3_7ByteH,TempRI3_7ByteL);
				}
				else if (TempRI3_7ByteH==0x0C)	//DAB2	18FF724C  18FF734C
				{
					static uint16_t tDABServiceCnt[DAB_SVC_CNT]={0,};
					uint8_t RI4_0Byte_06bit = 0;
					uint16_t RI4_1Byte = 0;
					
					RI4_0Byte_06bit = sData[0]&0x7F;
					RI4_1Byte = (sData[0]&0x80);
					RI4_1Byte = (RI4_1Byte<<1)|sData[1];
					if (RI4_0Byte_06bit<=MAX_DAB2LIST_COUNT)
					{
						tDABServiceCnt[RI4_0Byte_06bit] = RI4_1Byte;
						if (RDV.DABServiceCnt[RI4_0Byte_06bit]!=tDABServiceCnt[RI4_0Byte_06bit])
						{
							RDV.DABServiceCnt[RI4_0Byte_06bit] = RI4_1Byte;
							DAB2_DataChkTime = gMainTimerCnt_10m;//chpark.10.11
						}
						else
						{
							//printf("\n[RI4] SAME DATA=PYT:[%d],CNT[%d]\n", RI4_0Byte_06bit,RDV.DABServiceCnt[RI4_0Byte_06bit]);		
						}
//						printf("\n[RI4] RDV.DABServiceCnt = PYT:[%d] , CNT[%d]\n\n\n", RI4_0Byte_06bit,RDV.DABServiceCnt[RI4_0Byte_06bit]);						
					}
					//DAB2_DataChkTime = gMainTimerCnt_10m;//chpark.09.25
				}
				else if ((TempRI3_7ByteH==0x01)&&(TempRI3_7ByteL==0x0F))	//phongnum
				{
					RDV.RDPhoneNum[0] = sData[0];
					RDV.RDPhoneNum[1] = sData[1];
					RDV.RDPhoneNum[2] = sData[2];
					RDV.RDPhoneNum[3] = sData[3];
					RDV.RDPhoneNum[4] = sData[4];
					RDV.RDPhoneNum[5] = sData[5];

//					printf("\n\n[RI4] RDV.RDPhoneNum = [0x%X][0x%X][0x%X][0x%X]\n\n\n",RDV.RDPhoneNum[1],RDV.RDPhoneNum[2],RDV.RDPhoneNum[3],RDV.RDPhoneNum[4]);
				}
			}
			break;
			case RI5:	// 18FF744C
			{
				uint8_t tRDPhone=0xFF;
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
//				printf("\n[RI5] RDV.RDPhone 1: [%d] [%x]\n", RDV.RDPhone,Phone_Time);
				
				tRDPhone = ((sData[0]>>4)&0x0F);
				
				if ((tRDPhone==0)||(tRDPhone==1)||(tRDPhone==2)||(tRDPhone==3))
				{
					if ((RDV.RDPhone==5)&&(tRDPhone==0))
					{
					}
					else if (RDV.RDPhone!=0)
					{
						if ((tRDPhone==0)&&(Phone_Time==PHONE_TIME))
						{
							Phone_Time = gMainTimerCnt_10m;
							RDV.RDPhone=5;
						}
						else
						{
							RDV.RDPhone = tRDPhone;
							Phone_Time=PHONE_TIME;
						}
					}
					else
					{
						RDV.RDPhone = tRDPhone;
						Phone_Time=PHONE_TIME;
					}
				}
//				printf("[RI5] RDV.RDPhone 2: [%d] [%x]\n\n", RDV.RDPhone,Phone_Time);
				switch ((sData[0]>>1)&0x03)
				{
					case 1:
					case 2:
					case 3:
					{
						RDV.BTPair=2;	// ���?�Ϸ�
					}
					break;
					default :
					{
						if (((sData[1]>>0)&0x0F)==1)
							RDV.BTPair=1;	// ���?��
						else if (((sData[1]>>0)&0x0F)==3)
							RDV.BTPair=3;	// Ÿ�� �ƿ�
						else
							RDV.BTPair=0;
					}
					break;
				}
//				printf("[RI5] RDV.RDPhone : [%x] \n", RDV.RDPhone);
//				printf("[RI5] RDV.BTPair : [%x] \n", RDV.BTPair);
			}
			break;
			
			case CPI:		// 18FF811C	JPHong 0528   //18FF80E4(  CPR   SVM)
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				CPI_0Byte = (sData[0]);		//0:off , 1,5: AB , 2,6:AR , 3,7,:AL , 14,15,:BR , 16,17:3c
			}
			break;
			
			case UWH:		// 1850EDE4 jhlee 231207
			{
				uint8_t tmp;
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				UserMeter[0].tUserNum = sData[3];		// User Index Num 1~253 , 0;No item , 0xFE;Error , 0xFF; N/A
				UserMeter[0].tSrtTime	= (uint32_t)((sData[2]<<16)|(sData[1]<<8)|sData[0]);
				
				// 2~8 user meter reset ?
				for(tmp=1;tmp<8;tmp++)
				{
					UserMeter[tmp].tUserNum = 0;
					UserMeter[tmp].tSrtTime = 0;
					UserMeter[tmp].tEndTime = 0;
				}
			}
			break;
			
			case HM:	// 185001E4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				HMV.HourMete_rHour[0] = (sData[3]<<24 | sData[2]<<16 | sData[1]<<8 | sData[0]);
				HMV.HourMeter_Toggle = (sData[4]&0x01);
				HM_Time = gMainTimerCnt_10m;
				M9HM_Time = gMainTimerCnt_10m;
				
				UserMeter[0].tEndTime = HMV.HourMete_rHour[0];
				
				//printf("UserMeter HM L%d [%d] [%d.%d]>",1,UserMeter[0].tUserNum,UserMeter[0].tSrtTime/10,UserMeter[0].tSrtTime%10);
				//printf("[%d.%d]hr\r\n",UserMeter[0].tEndTime/10,UserMeter[0].tEndTime%10);

			}
			break;
			case TM1:	// 185002E4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				HMV.HourMete_rHour[1] = (uint32_t)(sData[3]<<24 | sData[2]<<16 | sData[1]<<8 | sData[0]);
				HMV.HourMete_rHour[2] = (uint32_t)(sData[7]<<24 | sData[6]<<16 | sData[5]<<8 | sData[4]);

				Trip1_Value = HMV.HourMete_rHour[1];		// add 230517 jhlee
				Trip2_Value = HMV.HourMete_rHour[2];		// add 230517 jhlee
				TM1_Time = gMainTimerCnt_10m;
			}
			break;
			case TM2:	// 185003E4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				HMV.HourMete_rHour[3] = (uint32_t)(sData[3]<<24 | sData[2]<<16 | sData[1]<<8 | sData[0]);
				HMV.HourMete_rHour[4] = (uint32_t)(sData[7]<<24 | sData[6]<<16 | sData[5]<<8 | sData[4]);

				Trip3_Value = HMV.HourMete_rHour[3];		// add 230517 jhlee
				Trip4_Value = HMV.HourMete_rHour[4];		// add 230517 jhlee
				TM2_Time = gMainTimerCnt_10m;
			}
			break;
			case TM3:	// 185008E4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				HMV.HourMete_rHour[5] = (uint32_t)(sData[3]<<24 | sData[2]<<16 | sData[1]<<8 | sData[0]);
				HMV.HourMete_rHour[6] = (uint32_t)(sData[7]<<24 | sData[6]<<16 | sData[5]<<8 | sData[4]);

				Trip5_Value = HMV.HourMete_rHour[5];		// add 230517 jhlee
				Trip6_Value = HMV.HourMete_rHour[6];		// add 230517 jhlee
				TM3_Time = gMainTimerCnt_10m;
			}
			break;
			case UA2:	// 1850E78C
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint8_t TempData;
				TempData = ((sData[1]>>0)&0x03);
#if 0	// JPHong 2023-12-23 ��11
				if (TempData==1)
				{
					UserPW_PassFlag = 1;
				}
				else
#endif
				if (((TempData==2)||(TempData==3))&&(gLogoPW_D7!=TempData))
				{
					UserPW_PassFlag = 4;	// NG
					gLogoPW_D7=TempData;
					gBuzzerMode = 12; //ON(300)OFF(300) 3Time
				}
				UserPW_Time_UA2 = gMainTimerCnt_10m;
				UserPW_Reply_time = 0;	// JPHong 2024-02-23 Main_V13_P21
			}
			break;
			case AUI1:	// 18510E8C
			{
				static uint8_t tAUI1_0B_3210b=0xFF;
				static uint8_t tAUI1_0B_3210b_PW=0xFF;
				
				
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint8_t TempData;
				TempData = ((sData[0]>>0)&0x0F);
				if ((TempData==2)||(TempData==3))
				{
					UserPW_PassFlag = 1;
					gBuzzerMode = 11;	//ON(300) 1Time
				}
				AUI1_0B_3210b = TempData;
				if (tAUI1_0B_3210b_PW!=AUI1_0B_3210b)	// JPHong 2023-12-23
				{
					if (g_nM7_StartType==M7_MODE_PUSH)		// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
					{
						if (((AUI1_0B_3210b==0)||(AUI1_0B_3210b==1)||(AUI1_0B_3210b==5))&&(Main_Disp_Mode != MAIN_MODE_PW))
						{
	//						CameraOnLeaveCallSrc(21);	// JPHong 2024-05-10
							
							Main_Disp_Mode = MAIN_MODE_PW;  // MainOnLeave
							GotoLogo();
						}
								
						tAUI1_0B_3210b_PW=AUI1_0B_3210b;
					}
				}
				UserPW_Time_AUI1 = gMainTimerCnt_10m;
			}
			break;

			case TCI:		// 1850F1E4
			{
				static uint8_t tg_TCI_0B=0xFF;
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				g_TCI_1B = sData[1];
				tg_TCI_0B = (sData[0]&0x3);
				g_TCI_Time = gMainTimerCnt_10m;
				if (g_TCI_0B != tg_TCI_0B)
				{
					if (((tg_TCI_0B==1)||(tg_TCI_0B==2))&&(Main_Disp_Mode!=MAIN_MODE_ENGN_COOL))
						GotoMain(MAIN_MODE_ENGN_COOL);
					g_TCI_0B = tg_TCI_0B;
				}
			}	
			break;
			//==================================================================
			//jhlee.
			case DMM:		// 0x18FF01E4 , ECU Error Code(ACC OnTime)
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				gEMR[0] = (uint16_t)((sData[1]<<8)|sData[0]);
				if(gEMR[0] == 0xFFFF)
				{
					gEMR[0] = 0;
				}

				gEMR[1] = (uint16_t)((sData[3]<<8)|sData[2]);
				if(gEMR[1] == 0xFFFF)
				{
					gEMR[1] = 0;
				}

				gEMR[2] = (uint16_t)((sData[5]<<8)|sData[4]);
				if(gEMR[2] == 0xFFFF)
				{
					gEMR[2] = 0;
				}

				gEMR[3] = (uint16_t)((sData[7]<<8)|sData[6]);
				if(gEMR[3] == 0xFFFF)
				{
					gEMR[3] = 0;
				}
			}
			break;

			case ACDM:	// 0x18FFA119 , AC Error Code
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				gACERR_Code[0] = sData[0];
				if(gACERR_Code[0] == 0xFF)
					gACERR_Code[0] = 0;
					
				gACERR_Code[1] = sData[1];
				if(gACERR_Code[1] == 0xFF)
					gACERR_Code[1] = 0;

				gACERR_Code[2] = sData[2];
				if(gACERR_Code[2] == 0xFF)
					gACERR_Code[2] = 0;

				gACERR_Code[3] = sData[3];
				if(gACERR_Code[3] == 0xFF)
					gACERR_Code[3] = 0;

				gACERR_Code[4] = sData[4];
				if(gACERR_Code[4] == 0xFF)
					gACERR_Code[4] = 0;

				gACERR_Code[5] = sData[5];
				if(gACERR_Code[5] == 0xFF)
					gACERR_Code[5] = 0;

				gACERR_Code[6] = sData[6];
				if(gACERR_Code[6] == 0xFF)
					gACERR_Code[6] = 0;

				gACERR_Code[7] = sData[7];
				if(gACERR_Code[7] == 0xFF)
					gACERR_Code[7] = 0;
			}
			break;

			case P_DTC:		// 18FF04E4
			{
				uint16_t tEMR_Past;
				uint32_t tEMR_Time;
				uint8_t i;
				
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				tEMR_Past = (uint16_t)((sData[1]<<8)|sData[0]);										// Past EMR Error code
				tEMR_Time = (uint32_t)((sData[5]<<16)|(sData[4]<<8)|sData[3]);		// Past EMR Error time

				if((sData[1]!=0xFF)&&(sData[0]!=0xFF))
				//if(tEMR_Past > 0)
				{
					gEMR_Past[0] = tEMR_Past;
					gEMR_Time[0] = tEMR_Time;
					
					for(i=1;i<6;i++)
					{
						gEMR_Past[i] = 0;
						gEMR_Time[i] = 0;
					}
				}
			}	
			break;

			case P_DTCU:		// 18503FE4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				uint16_t tEMR_Past;
				uint32_t tEMR_Time;
				uint8_t tPastCar_UserIdx;
				uint8_t i;

				tEMR_Past = (uint16_t)((sData[1]<<8)|sData[0]);										// Past CAR Error code
				tEMR_Time = (uint32_t)((sData[5]<<16)|(sData[4]<<8)|sData[3]);		// Past CAR Error time

				tPastCar_UserIdx = sData[6];		// Past CAR Error User Index

//				printf("Past Car Error User[%d] , Er : %d , %d.%d hr\r\n",tPastCar_UserIdx,tEMR_Past,tEMR_Time/10,tEMR_Time%10);

				if((sData[1]!=0xFF)&&(sData[0]!=0xFF))
				{
					gEMR_Past[0] = tEMR_Past;
					gEMR_Time[0] = tEMR_Time;
					gPastCar_UserIdx[0] = tPastCar_UserIdx;
					
					for(i=1;i<5;i++)
					{
						gEMR_Past[i] = 0;
						gEMR_Time[i] = 0;
						gPastCar_UserIdx[i] = 0;
					}
				}
			}
			break;
			
			case PEEI:		// 1850EAE4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				uint8_t tPastEng_UserIdx;
				uint32_t tPastEng_SPN;
				uint8_t tPastEng_FMI;
				uint32_t tPastEng_Time;

				uint8_t i;

				tPastEng_SPN = (uint32_t)((sData[2]<<16)|(sData[1]<<8)|sData[0]);				// Past ENGINE Error code
				tPastEng_FMI = sData[3];
				tPastEng_Time = (uint32_t)((sData[6]<<16)|(sData[5]<<8)|sData[4]);				// Past Engine Error Time
				tPastEng_UserIdx = sData[7];

				if(tPastEng_SPN == 0x00FFFFFF)
				{
					gSPN_Past[0] = 0;
					gFMI_Past[0] = 0;
					gPastEng_Time[0] = 0;
					gPastEng_UserIdx[0] = 0;
				}
				else
				{
					if(tPastEng_FMI == 0xFF)
					{
						gSPN_Past[0] = 0;
						gFMI_Past[0] = 0;
						gPastEng_Time[0] = 0;
						gPastEng_UserIdx[0] = 0;
					}
					else
					{
						gSPN_Past[0] = tPastEng_SPN;
						gFMI_Past[0] = tPastEng_FMI;
						gPastEng_Time[0] = tPastEng_Time;
						gPastEng_UserIdx[0] = tPastEng_UserIdx;
					}
				}

				for(i=1;i<4;i++)
				{
					gSPN_Past[i] = 0;
					gFMI_Past[i] = 0;
					gPastEng_Time[i] = 0;
					gPastEng_UserIdx[i] = 0;
				}
			
//				printf("Past Eng Error User[%d] , SPN : %d , FMI : %d , %d.%d hr\r\n",tPastEng_UserIdx,tPastEng_SPN,tPastEng_FMI,tPastEng_Time/10,tPastEng_Time%10);
				
			}	
			break;

			case GFI:	// 1850EEE4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				memset(&MUI,0,sizeof(MUI));
				memcpy(MUI.TPDT,(uint8_t *)(sData),8);
			}	
			break;
			
			case TP_CM_PGN:		// 18ECFFE4
			{
				uint8_t i;
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				if(sData[0] == 0x20)
				{
					tPGN = (sData[7]<<16)|(sData[6]<<8)|sData[5];
					if(tPGN == PGN50EE)		// PGN50EE(GFI)	//2024-01-27 JPHong 
					{
						memset(MUI.TPDT,0,sizeof(MUI.TPDT));
						MUI.PacketCnt = sData[3];
						MUI.DataLenght = (sData[2]<<8)|sData[1];
						MUI.Status = 1;
					}
					else
					{
						tLenght = (sData[2]<<8)|sData[1];
						tPacket_CNT = sData[3];
						//printf("TPCM_PGN = %04X\r\n",tPGN);					// PGN ; 50ED(UWH) // PGN ; 503F(P_DTCU) // PGN ; 50EA(PEEI)
						//printf("TPDT_PKT = %d\r\n",tPacket_CNT);
						//printf("TPDT_Length = %d\r\n",tLenght);
						
						if(tPGN == 0x50ED)		// PGN ; 50ED(UWH)
						{
							M9HM_Time = 0;
						}						
/*
						if(tPGN == 0x50ED)		// PGN ; 50ED(UWH)
						{
							//printf("UWH Data CNT = %d\r\n",tLenght/4);
							for(i=0;i<8;i++)
							{
								UserMeter[i].tUserNum = 0;		// User Index Num 1~253 , 0;No item , 0xFE;Error , 0xFF; N/A
								UserMeter[i].tSrtTime	= 0;
								UserMeter[i].tEndTime	= 0;
							}
							M9HM_Time = 0;
						}
						else if(tPGN == 0x503F)		// PGN ; 503F(P_DTCU)
						{
							for(i=0;i<5;i++)
							{
								gEMR_Past[i] = 0;
								gEMR_Time[i] = 0;
								gPastCar_UserIdx[i] = 0;
							}
						}
						else if(tPGN == 0x50EA)		// PGN ; 50EA(PEEI)
						{
							for(i=0;i<4;i++)
							{
								gSPN_Past[i] = 0;				// Past ENGINE Error code
								gFMI_Past[i] = 0;
								gPastEng_Time[i] = 0;	// Past Engine Error Time
								gPastEng_UserIdx[i] = 0;
							}
						}
*/						
						if(tLenght >= 128)
						{
							tLenght = 126;
							tPacket_CNT = 21;
						}
						memset(PktData,0,128);
					}
				}
			}
			break;
			
			case TP_DT_PGN:		// 18EBFFE4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				uint8_t PktIdx = 0,i,tPkt_Cnt=0;
				
				PktIdx = sData[0];
				if(tPGN == PGN50EE)		// PGN50EE(GFI)	//2024-01-27 JPHong 
				{
					if(MUI.PacketCnt == PktIdx)		// last packet
					{
						if((MUI.DataLenght%7) == 0)
						{
							memcpy((uint8_t *)(MUI.TPDT+((PktIdx-1)*7)),(uint8_t *)(sData+1),7);
						}
						else
						{
							memcpy((uint8_t *)(MUI.TPDT+((PktIdx-1)*7)),(uint8_t *)(sData+1),(MUI.DataLenght%7));
						}
						MUI.Status = 2;
					}
					else
					{
						memcpy((uint8_t *)(MUI.TPDT+((PktIdx-1)*7)),(uint8_t *)(sData+1),7);
						if (PktIdx==0x01)
						{
							// SPEC ==> 0:ǥ�� , 1:�ٱ�ɱ׸�?KPM��)
							// XO ==> 0:OFF , 1:�ٱ�ɱ׸�?KPM��) , 2:ǥ��
							if(MUI.TPDT[0] == 0)
							{
								MUI.TPDT[0] = 2;
							}
						}
					}
#if 0					
					printf("PGN50EE DATA : %d %d\r\n",PktIdx,MUI.Status);
					for(i=0;i<MUI.DataLenght;i++)
						printf("[%02X] ",MUI.TPDT[i]);
					printf("\r\n");					
#endif
				}
				else
				{
					if(tPacket_CNT == PktIdx)		// last packet
					{
						if((tLenght%7) == 0)
						{
							memcpy((uint8_t *)(PktData+((PktIdx-1)*7)),(uint8_t *)(sData+1),7);
						}
						else
						{
							memcpy((uint8_t *)(PktData+((PktIdx-1)*7)),(uint8_t *)(sData+1),(tLenght%7));
						}
#if 0						
						printf("0x50ED DATA : %d \r\n",PktIdx);
						for(i=0;i<tLenght;i++)
							printf("[%02X] ",PktData[i]);
						printf("\r\n");
#endif
										
						if(tPGN == 0x503F)		// PGN ; 503F(P_DTCU) Past Car Error
						{
							tPkt_Cnt = tLenght / 7;
							
							for(i=0;i<5;i++)
							{
								gEMR_Past[i] = (uint16_t)((PktData[i*7+2]<<16)|(PktData[i*7+1]<<8)|PktData[i*7]);
								gEMR_Time[i] = (uint32_t)((PktData[i*7+5]<<16)|(PktData[i*7+4]<<8)|PktData[i*7+3]);
								gPastCar_UserIdx[i] = (uint8_t)PktData[i*7+6];
							
								if(gEMR_Past[i] == 0xFFFF)
								{
									gEMR_Past[i] = 0;
									gEMR_Time[i] = 0;
									gPastCar_UserIdx[i] = 0;
								}
								
								if(i>=tPkt_Cnt)
								{
									gEMR_Past[i] = 0;
									gEMR_Time[i] = 0;
									gPastCar_UserIdx[i] = 0;
								}
								
//								printf("Past Car ErrorCode_%d [%d][%d.%dh] > User Index[%d]\r\n",i+1,gEMR_Past[i],gEMR_Time[i]/10,gEMR_Time[i]%10,gPastCar_UserIdx[i]);
							}
						}
						else if(tPGN == 0x50EA)		// PGN ; 50EA(PEEI)
						{
							tPkt_Cnt = tLenght / 8;
							
							for(i=0;i<4;i++)
							{
								gSPN_Past[i] = (uint32_t)((PktData[i*8+2]<<16)|(PktData[i*8+1]<<8)|PktData[i*8]);				// Past ENGINE Error code
								gFMI_Past[i] = PktData[i*8+3];
								gPastEng_Time[i] = (uint32_t)((PktData[i*8+6]<<16)|(PktData[i*8+5]<<8)|PktData[i*8+4]);	// Past Engine Error Time
								gPastEng_UserIdx[i] = PktData[i*8+7];
	
								if(i >= tPkt_Cnt)
								{
									gSPN_Past[i] = 0;				// Past ENGINE Error code
									gFMI_Past[i] = 0;
									gPastEng_Time[i] = 0;	// Past Engine Error Time
									gPastEng_UserIdx[i] = 0;
								}
	
//								printf("Past Eng Error User[%d] , SPN : %d , FMI : %d , %d.%d hr\r\n",gPastEng_UserIdx[i],gSPN_Past[i],gFMI_Past[i],gPastEng_Time[i]/10,gPastEng_Time[i]%10);
							}
						}
						else if(tPGN == 0x50ED)		// PGN ; 50ED(UWH)
						{
							tPkt_Cnt = tLenght / 4;

							for(i=0;i<8;i++)
							{
								UserMeter[i].tUserNum = PktData[i*4+3];		// User Index Num 1~253 , 0;No item , 0xFE;Error , 0xFF; N/A
								UserMeter[i].tSrtTime	= (uint32_t)((PktData[i*4+2]<<16)|(PktData[i*4+1]<<8)|PktData[i*4]);
								
//								printf("STR[%08X] [%02X][%02X][%02X][%02X]",UserMeter[i].tSrtTime,PktData[i*4+2],PktData[i*4+1],PktData[i*4],PktData[i*4+3]);
								
								if(i > 0)
								{
									UserMeter[i].tEndTime = UserMeter[i-1].tSrtTime;
								}
#if 1
								if(i >= tPkt_Cnt)
								{
									UserMeter[i].tUserNum = 0;		// User Index Num 1~253 , 0;No item , 0xFE;Error , 0xFF; N/A
									UserMeter[i].tSrtTime = 0;
									UserMeter[i].tEndTime = 0;
								}
#endif
//								printf("  UserMeter L%d [%d] [%d.%d]>",i+1,UserMeter[i].tUserNum,UserMeter[i].tSrtTime/10,UserMeter[i].tSrtTime%10);
//								printf("[%d.%d]hr\r\n",UserMeter[i].tEndTime/10,UserMeter[i].tEndTime%10);
							}
						}
					}
					else
					{
						memcpy((uint8_t *)(PktData+((PktIdx-1)*7)),(uint8_t *)(sData+1),7);
#if 0						
						printf("0x50ED DATA : %d \r\n",PktIdx);
						for(i=0;i<tLenght;i++)
							printf("[%02X] ",PktData[i]);
						printf("\r\n");
#endif						
					}
				}
			}
			break;

			// engine error single
			case DM1a:		// 1CFECA00 , ECU
			case DM1aY:		// 18FECA00 , ECU
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint8_t i;
				
				uint32_t tmp_Data = 0;
				
				tmp_Data = (uint32_t)((sData[4]<<16)|(sData[3]<<8)|sData[2]);
				
				//if((sData[4]!=0xFF)&&(sData[3]!=0xFF)&&(sData[2]!=0xFF))
				if(tmp_Data != 0x00FFFFFF)
				{
					gSPN_sECU = (uint32_t)(((sData[4]&0xE0)<<11)|(sData[3]<<8)|sData[2]);
					gFMI_sECU = (uint8_t)(sData[4]&0x1F);
					
					for(i=0;i<3;i++)
					{
						gSPN_ECU[i] = 0;
						gFMI_ECU[i] = 0;
					}
				}
				else if((sData[4]==0xFF)&&(sData[3]==0xFF)&&(sData[2]==0xFF))
				{
					gSPN_sECU = 0;
					gFMI_sECU = 0;
				}
			}
			break;

			case DM1b:		// 1CFECA3D	, DCU
			case DM1bY:		// 18FECA3D	, DCU
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint8_t i;

				uint32_t tmp_Data2 = 0;
				
				tmp_Data2 = (uint32_t)((sData[4]<<16)|(sData[3]<<8)|sData[2]);
				
				//if((sData[4]!=0xFF)&&(sData[3]!=0xFF)&&(sData[2]!=0xFF))
				if(tmp_Data2 != 0x00FFFFFF)
				{
					gSPN_sDCU = (uint32_t)(((sData[4]&0xE0)<<11)|(sData[3]<<8)|sData[2]);
					gFMI_sDCU = (sData[4]&0x1F);

					for(i=0;i<3;i++)
					{
						gSPN_DCU[i] = 0;
						gFMI_DCU[i] = 0;
					}
				}
				else if((sData[4]==0xFF)&&(sData[3]==0xFF)&&(sData[2]==0xFF))
				{
					gSPN_sDCU = 0;
					gFMI_sDCU = 0;
				}
			}
			break;

/*
			case DM2a: 		// 0x1CFECB00		// single Past ECU ErrorCode
			case DM2aY: 	// 0x18FECB00
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint8_t i;
				
				if((sData[4]!=0xFF)&&(sData[3]!=0xFF)&&(sData[2]!=0xFF))
				{
					gSPN_sPastECU = (uint32_t)(((sData[4]&0xE0)<<11)|(sData[3]<<8)|sData[2]);
					gFMI_sPastECU = (uint8_t)(sData[4]&0x1F);
					
					for(i=0;i<6;i++)
					{
						gSPN_PastECU[i] = 0;
						gFMI_PastECU[i] = 0;
					}
				}
				else if((sData[4]==0xFF)&&(sData[3]==0xFF)&&(sData[2]==0xFF))
				{
					gSPN_sPastECU = 0;
					gFMI_sPastECU = 0;
				}
			}
			break;
			
			case DM2b: 		// 0x1CFECB3D		// single Past DCU ErrorCode
			case DM2bY: 	// 0x18FECB3D
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint8_t i;
				
				if((sData[4]!=0xFF)&&(sData[3]!=0xFF)&&(sData[2]!=0xFF))
				{
					gSPN_sPastDCU = (uint32_t)(((sData[4]&0xE0)<<11)|(sData[3]<<8)|sData[2]);
					gFMI_sPastDCU = (sData[4]&0x1F);

					for(i=0;i<6;i++)
					{
						gSPN_PastDCU[i] = 0;
						gFMI_PastDCU[i] = 0;
					}
				}
				else if((sData[4]==0xFF)&&(sData[3]==0xFF)&&(sData[2]==0xFF))
				{
					gSPN_sPastDCU = 0;
					gFMI_sPastDCU = 0;
				}
			}
			break;
*/			
			// multi engine error
			case TP_CMa:		// 1CECFF00 , ECU
			case TP_CMaY:		// 18ECFF00 , ECU
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				//uint8_t i;
				
				if(sData[0] == 0x20)
				{
					tLenght_ECU = (sData[2]<<8)|sData[1];
					
					tPacket_ECU_CNT = sData[3];

					tPGN = (uint16_t)((sData[6]<<8)|sData[5]);
//					printf("Multi ECU PGN = 0x%8X\r\n",tPGN);					//FECA(DM1) , FECB(DM2)
					
					if(tLenght >= 128)
					{
						tLenght_ECU = 126;
						tPacket_ECU_CNT = 31;
					}

					memset(PktData_ECU,0,128);
				}
			}
			break;
			
			case TP_DTa:		// 1CEBFF00 , ECU
			case TP_DTaY:		// 18EBFF00 , ECU
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				uint8_t PktIdx = 0,i;
				uint16_t kPGN = 0;
				
				PktIdx = sData[0];

				if(tPacket_ECU_CNT == PktIdx)		// last packet
				{
					if((tLenght_ECU%7) == 0)
					{
						memcpy((uint8_t *)(PktData_ECU+((PktIdx-1)*7)),(uint8_t *)(sData+1),7);
					}
					else
					{
						memcpy((uint8_t *)(PktData_ECU+((PktIdx-1)*7)),(uint8_t *)(sData+1),(tLenght_ECU%7));
					}

					kPGN = (uint16_t)((PktData_ECU[1]<<8)|PktData_ECU[0]);		//FECA(DM1) , FECB(DM2)

//					printf("TP_DT ECU PGN = 0x%04X , 0x%04X\r\n",kPGN,tPGN);

					if(tPGN == 0xFECA)
					{
						for(i=0;i<3;i++)
						{
							if((PktData_ECU[i*4+4] == 0xFF)&&(PktData_ECU[i*4+3] == 0xFF)&&(PktData_ECU[i*4+2] ==0xFF))
							{
								gSPN_ECU[i] = 0;
								gFMI_ECU[i] = 0;
							}	
							else
							{
								gSPN_ECU[i] = (uint32_t)(((PktData_ECU[i*4+4]&0xE0)<<11)|(PktData_ECU[i*4+3]<<8)|PktData_ECU[i*4+2]);
								gFMI_ECU[i] = (uint8_t)(PktData_ECU[i*4+4]&0x1F);
							}

//							printf("TP_DT ECU Error = SPN[%6d] FMI[%2d]\r\n",gSPN_ECU[i],gFMI_ECU[i]);
						}

						if(gSPN_ECU[0] > 0)
						{
							gSPN_sECU = 0;
							gFMI_sECU = 0;
						}
					}
/*					
					else if(tPGN == 0xFECB)
					{
						for(i=0;i<6;i++)
						{
							if((PktData_ECU[i*4+4] == 0xFF)&&(PktData_ECU[i*4+3] == 0xFF)&&(PktData_ECU[i*4+2] ==0xFF))
							{
								gSPN_PastECU[i] = 0;
								gFMI_PastECU[i] = 0;
							}	
							else
							{
								gSPN_PastECU[i] = (uint32_t)(((PktData_ECU[i*4+4]&0xE0)<<11)|(PktData_ECU[i*4+3]<<8)|PktData_ECU[i*4+2]);
								gFMI_PastECU[i] = (uint8_t)(PktData_ECU[i*4+4]&0x1F);
							}	

							printf("TP_DT Past ECU Error = SPN[%6d] FMI[%2d]\r\n",gSPN_PastECU[i],gFMI_PastECU[i]);
						}

						if(gSPN_PastECU[0] > 0)
						{
							gSPN_sPastECU = 0;
							gFMI_sPastECU = 0;
						}
					}
*/					
				}
				else
				{
					memcpy((uint8_t *)(PktData_ECU+((PktIdx-1)*7)),(uint8_t *)(sData+1),7);
				}
			}
			break;

			case TP_CMb:		// 1CECFF3D , DCU
			case TP_CMbY:		// 18ECFF3D , DCU
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				if(sData[0] == 0x20)
				{
					tLenght_DCU = (sData[2]<<8)|sData[1];
					
					tPacket_DCU_CNT = sData[3];

					tPGN = (uint16_t)((sData[6]<<8)|sData[5]);
//					printf("Mutil DCU PGN = %8X\r\n",tPGN);

					if(tLenght_DCU >= 128)
					{
						tLenght_DCU = 126;
						tPacket_DCU_CNT = 31;
					}

					memset(PktData_DCU,0,128);
				}
			}
			break;

			case TP_DTb:		// 1CEBFF3D , DCU
			case TP_DTbY:		// 18EBFF3D , DCU
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				uint8_t PktIdx = 0,i;
				uint16_t kPGN = 0;
				
				PktIdx = sData[0];

				if(tPacket_DCU_CNT == PktIdx)		// last packet
				{
					if((tLenght_DCU%7) == 0)
					{
						memcpy((uint8_t *)(PktData_DCU+((PktIdx-1)*7)),(uint8_t *)(sData+1),7);
					}
					else
					{
						memcpy((uint8_t *)(PktData_DCU+((PktIdx-1)*7)),(uint8_t *)(sData+1),(tLenght_DCU%7));
					}

					kPGN = (uint16_t)((PktData_DCU[1]<<8)|PktData_DCU[0]);
//					printf("TP_DT ECU PGN = 0x%04X , 0x%04X\r\n",kPGN,tPGN);		//FECA(DM1) , FECB(DM2)

					if(tPGN == 0xFECA)			// DM1
					{
						for(i=0;i<3;i++)
						{
							if((PktData_DCU[i*4+4] == 0xFF)&&(PktData_DCU[i*4+3] == 0xFF)&&(PktData_DCU[i*4+2] ==0xFF))
							{
								gSPN_DCU[i] = 0;
								gFMI_DCU[i] = 0;
							}
							else 
							{
								gSPN_DCU[i] = (uint32_t)(((PktData_DCU[i*4+4]&0xE0)<<11)|(PktData_DCU[i*4+3]<<8)|PktData_DCU[i*4+2]);
								gFMI_DCU[i] = (uint8_t)(PktData_DCU[i*4+4]&0x1F);
							}

//							printf("TP_DT DCU Error = SPN[%6d] FMI[%2d]\r\n",gSPN_DCU[i],gFMI_DCU[i]);
						}

						if(gSPN_DCU[0] > 0)
						{
							gSPN_sDCU = 0;
							gFMI_sDCU = 0;
						}
					}
/*
					else if(tPGN == 0xFECB)		// DM2
					{
						for(i=0;i<6;i++)
						{
							if((PktData_DCU[i*4+4] == 0xFF)&&(PktData_DCU[i*4+3] == 0xFF)&&(PktData_DCU[i*4+2] ==0xFF))
							{
								gSPN_PastDCU[i] = 0;
								gFMI_PastDCU[i] = 0;
							}
							else
							{
								gSPN_PastDCU[i] = (uint32_t)(((PktData_DCU[i*4+4]&0xE0)<<11)|(PktData_DCU[i*4+3]<<8)|PktData_DCU[i*4+2]);
								gFMI_PastDCU[i] = (uint8_t)(PktData_DCU[i*4+4]&0x1F);
							}

							printf("TP_DT Past DCU Error = SPN[%6d] FMI[%2d]\r\n",gSPN_PastDCU[i],gFMI_PastDCU[i]);
						}

						if(gSPN_PastDCU[0] > 0)
						{
							gSPN_sPastDCU = 0;
							gFMI_sPastDCU = 0;
						}
					}
*/					
				}
				else
				{
					memcpy((uint8_t *)(PktData_DCU+((PktIdx-1)*7)),(uint8_t *)(sData+1),7);
				}
			}
			break;
			case DM14T: 		// 0x18D9CE25
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint8_t i;
#ifdef DM14_DBG_MSG
				printf("[DM14T:0x%X]= 0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\n",DM14T,sData[0],sData[1],sData[2],sData[3],sData[4],sData[5],sData[6],sData[7]);
#endif				
				DM14_Data.Length = ((sData[1]&0xE0)<<3)|(sData[0])	;			// 0Byte,1Byte 5-7Bit
				DM14_Data.Command = (sData[1]&0xE)>>1;							// 1Byte 1-3Bit
				DM14_Data.Pointer = (sData[4]<<16)|(sData[3]<<8)|(sData[2]<<0);	// 2-4Byte
				DM14_Data.PointerEx = sData[5];									// 5Byte
				if (DM14_Data.Count==0)
				{
					DM14_Data.User_Level = (sData[7]<<8)|(sData[6]<<0);				// 6-7Byte
					DM14_Data.User_Level = 0;	// TTMS 15226
					DM14_Data.DM14_ErrorFlag = 0;
				}
				else
					DM14_Data.Key = (sData[7]<<8)|(sData[6]<<0);				// 6-7Byte
#if 0 //def DM14_DBG_MSG	
				printf("\n[DM14T]Length=[%d][0x%X]\n",DM14_Data.Length,DM14_Data.Length);
				printf("[DM14T]Command=0x%X\n",DM14_Data.Command);
				printf("[DM14T]Pointer=0x%X\n",DM14_Data.Pointer);
				printf("[DM14T]PointerEx=0x%X\n",DM14_Data.PointerEx);
				printf("[DM14T]User_Level=0x%X\n",DM14_Data.User_Level);
				printf("[DM14T]Key=0x%X\n",DM14_Data.Key);
				printf("[DM14T]Count=0x%X\n",DM14_Data.Count);
#endif			
								
				Parsing_DM14();

				
			}
			break;
			case TP_CM_T:	// 1CECCE25
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint8_t i;
#ifdef DM14_DBG_MSG
				printf("[TP_CM_T MM:0x%X]= 0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\n",TP_CM_T,sData[0],sData[1],sData[2],sData[3],sData[4],sData[5],sData[6],sData[7]);
#endif				
				if (DM14_Data.DM14_ErrorFlag == 0)
				{
					DM14_Data.Time=gMainTimerCnt_10m;
					if ((DM14_Data.Command==DM14_RX_WRITE)&&(DM14_Data.Count==2))
					{
						TPCM_Rts.Control = sData[0];								// 0Byte
						TPCM_Rts.Length = (sData[2]<<8)|(sData[1]<<0);				// 1Byte,2Byte
						TPCM_Rts.TNoP = sData[3];									// 3Byte	Total number of packets
						TPCM_Rts.MNoP = sData[4];									// 4Byte	Maximum number of packets
						TPCM_Rts.PGN = (sData[7]<<16)|(sData[6]<<8)|(sData[5]<<0);	// 5-7Byte	Parameter Group Number of the packeted message(PGN) DM16(0xD700)
						Parsing_TPCM();
#ifdef DM14_DBG_MSG							
						printf("\n[TP_CM_T]Control=0x%X\n",TPCM_Rts.Control);
						printf("[TP_CM_T]Length=0x%X\n",TPCM_Rts.Length);
						printf("[TP_CM_T]TNoP=0x%X\n",TPCM_Rts.TNoP);
						printf("[TP_CM_T]MNoP=0x%X\n",TPCM_Rts.MNoP);
						printf("[TP_CM_T]PGN=0x%X\n",TPCM_Rts.PGN);
#endif						
					}
					else if ((DM14_Data.Command==DM14_RX_READ)&&(DM14_Data.Count==2))
					{
						if (TPCM_Ack.Count == 0)
						{
							TPCM_Ack.Control = 0;
							TPCM_Cts.Control = sData[0];								// 0Byte
							TPCM_Cts.NPS = sData[1];									// 1Byte	Number of packets that can be sent
																						// Number of packets that can be sent. This value shall be no larger than the smaller of the 
																						// two values in byte 4 and byte 5 of the RTS message. 
							TPCM_Cts.NPN = sData[2];									// 2Byte	Next packet number to be sent // zero not allowed
							TPCM_Cts.SAE = (sData[4]<<8)|(sData[3]<<0);					// 3,4Byte	Reserved for assignment by SAE, these bytes should be filled with FF
							TPCM_Cts.PGN = (sData[7]<<16)|(sData[6]<<8)|(sData[5]<<0);	// 5-7Byte	Parameter Group Number of the packeted message(PGN) DM16(0xD700)
							
							if (TPCM_Cts.Control==TPCM_RX_CTS)
							{
								Sand_TPDT(DM14_RX_READ,0);
							}
							TPCM_Ack.Count = 1;
						}
						else if (TPCM_Ack.Count == 1)
						{
							TPCM_Rts.Control = sData[0];								// 0Byte
							TPCM_Rts.Length = (sData[2]<<8)|(sData[1]<<0);				// 1Byte,2Byte
							TPCM_Rts.TNoP = sData[3];									// 3Byte	Total number of packets
							TPCM_Rts.MNoP = sData[4];									// 4Byte	Maximum number of packets
							TPCM_Rts.PGN = (sData[7]<<16)|(sData[6]<<8)|(sData[5]<<0);	// 5-7Byte	Parameter Group Number of the packeted message(PGN) DM16(0xD700)
							
							if (TPCM_Rts.Control == TPCM_RX_EOMA)
							{
								Sand_DM14(DM14_TX_COMPLETED,DM14_Data.Count,0);
							}
						}
					}
				}
			}
			break;
			case TP_DT_T:	// 1CEBCE25
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint8_t i;
#ifdef DM14_DBG_MSG
				printf("[TP_DT_T:0x%X]= 0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X\n",TP_DT_T,sData[0],sData[1],sData[2],sData[3],sData[4],sData[5],sData[6],sData[7]);
#endif				
				if ((DM14_Data.DM14_ErrorFlag == 0)&&(DM14_Data.Count==2))
				{
					DM14_Data.Time=gMainTimerCnt_10m;
					if ((TPCM_Rts.Length>=DM14_Tpdt.Count)&&(TPCM_Rts.TNoP>=DM14_Tpdt.PSN))
					{
						if ((DM14_Tpdt.PSN+1)==sData[0])
						{
							memcpy (&DM14_Tpdt.Data[DM14_Tpdt.Count],&sData[1],7);
#ifdef DM14_DBG_MSG								
	printf("[TP_DT_T %d]Data=[0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X][0x%02X]\n",DM14_Tpdt.Count,sData[1],sData[2],sData[3],sData[4],sData[5],sData[6],sData[7]);
#endif
							DM14_Tpdt.PSN = sData[0];
							DM14_Tpdt.Count = DM14_Tpdt.Count+7;
						}
						else	//error
						{
							printf("TP_DT_T Error PSN[%d][%d]\n",(DM14_Tpdt.PSN+1),sData[0]);
						}
					}
					else	// Error
					{
						printf("TP_DT_T Error Length[%d][%d][%d][%d]\n",TPCM_Rts.Length,DM14_Tpdt.Count,TPCM_Rts.TNoP,DM14_Tpdt.PSN);
					}
					if ((TPCM_Rts.Length<=DM14_Tpdt.Count)||(TPCM_Rts.TNoP<=DM14_Tpdt.PSN))
					{
						DM14_Tpdt.Count = TPCM_Rts.Length;
						Sand_TPCM(TPCM_RX_EOMA,TPCM_Rts.PGN);
						Sand_DM14(DM14_TX_COMPLETED,DM14_Data.Count,0);
					}
#ifdef DM14_DBG_MSG
					{
						printf("\n[TP_DT_T]Data=[%s]\n",DM14_Tpdt.Data);
						printf("[TP_DT_T]PSN=[%d]\n",DM14_Tpdt.PSN);
						printf("[TP_DT_T]Count=[%d][%d][%d][%d]\n",TPCM_Rts.Length,DM14_Tpdt.Count,TPCM_Rts.TNoP,DM14_Tpdt.PSN);
					}
#endif
				}
			}
			break;
			case UI1:		// 18511D8C //chpark.08.22
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				
				// Spec : "0: ���� ���� 1~253: �� �����?��ȣ, 0xFD���� ���� 0xFE��Err 0xFF��N/A"
				// XO Spec : "0~252: �� �����?��ȣ, 0xFC���� ���� 0xFD��Err, 0xFE��N/A, 0xFF: ���� ���� "
				// 2023-11-13 JPHong 
				

				M7_UserIndexEx = sData[0];
				
				if (M7_UserIndexEx==0)
				{
					M7_UserIndex = 0xFF;
				}
				else
					M7_UserIndex = M7_UserIndexEx-1;	//chpark.12.18//current user idx

				#if 1	
					if (M7_UserIndex>=MAX_SU100_USER_NUM)
					{
						M7_UserIndex = 0;
					}
				#endif

				User_RegInfo[M7_UserIndex].PassNOD = sData[1]&0x0F; //current user passcode num of digit
				bool bRegPassCode = (sData[1]>>4) & 0x01;//current user passcode exist
				if(!bRegPassCode)
				{
					User_RegInfo[M7_UserIndex].PassNOD = 0;
				}
				User_RegInfo[M7_UserIndex].RFStatus = (sData[1]>>5) & 0x01;//current user RFID registered.
				User_RegInfo[M7_UserIndex].RegFlag = sData[2]&0x0F; //current user access level : 2byte 0-3bit
				
				SU100_UI1_3Byte =  sData[3];//chpark.08.22
				SU100_UI1_4Byte =  sData[4];//chpark.08.22	
				//======================
				//chpark.24.02.08
				for(int i=0;i<8;i++)//8
				{
					User_RegInfo[i].bRegister = (SU100_UI1_3Byte>>i)&0x01;
					//printf("User_RegInfo[%d].bRegister = %d\n",i,User_RegInfo[i].bRegister);
				}

				for(int i=0;i<7;i++)//7
				{
					User_RegInfo[i+8].bRegister = (SU100_UI1_4Byte>>i)&0x01;
					//printf("User_RegInfo[%d].bRegister = %d\n",i+8,User_RegInfo[i+8].bRegister);
				}	
				//======================

				//printf("[UI1]CurUser=%d level=%d  3Byte:%d 4Byte:%d\n",M7_UserIndex,User_RegInfo[M7_UserIndex].RegFlag ,SU100_UI1_3Byte,SU100_UI1_4Byte);

				g_UI1_ChkTime = gMainTimerCnt_10m;	//chpark.10.12
				gMenu_UI_Time = gMainTimerCnt_10m;	// jhlee 240202

				gUI_Server_Mode = true;	

			}
			break;

			case UI2:		// 18511E8C //chpark.08.22
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				//memcpy(&SU100_UI2_All_Byte[0],&sData[0],8);
				for(int i=0;i<8;i++)
				{
					SU100_UI2_All_Byte[i] = sData[i];
					//printf("[UI2][%d] = %d \r\n",i,SU100_UI2_All_Byte[i]);
				}
				//chpark.24.02.08
				User_RegInfo[0].PassNOD = (SU100_UI2_All_Byte[0] & 0x0F);
				User_RegInfo[1].PassNOD = (SU100_UI2_All_Byte[2] & 0x0F);
				User_RegInfo[2].PassNOD = (SU100_UI2_All_Byte[4] & 0x0F);
				User_RegInfo[3].PassNOD = (SU100_UI2_All_Byte[6] & 0x0F);
				//
				User_RegInfo[0].RFStatus = ( (SU100_UI2_All_Byte[0]>>4) & 0x01);
				User_RegInfo[1].RFStatus = ( (SU100_UI2_All_Byte[2]>>4) & 0x01);
				User_RegInfo[2].RFStatus = ( (SU100_UI2_All_Byte[4]>>4) & 0x01);
				User_RegInfo[3].RFStatus = ( (SU100_UI2_All_Byte[6]>>4) & 0x01);
				//
				User_RegInfo[0].RegFlag = (SU100_UI2_All_Byte[1] & 0x0F);
				User_RegInfo[1].RegFlag = (SU100_UI2_All_Byte[3] & 0x0F);
				User_RegInfo[2].RegFlag = (SU100_UI2_All_Byte[5] & 0x0F);
				User_RegInfo[3].RegFlag = (SU100_UI2_All_Byte[7] & 0x0F);


				g_UI2_ChkTime = gMainTimerCnt_10m;	//chpark.10.12

			}
			break;

			case UI3:		// 18511F8C //chpark.08.22
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				//memcpy(&SU100_UI3_All_Byte[0],&sData[0],8);
				for(int i=0;i<8;i++)
				{
					SU100_UI3_All_Byte[i] = sData[i];
					//printf("[UI3][%d] = %d \r\n",i,SU100_UI3_All_Byte[i]);
				}

				//chpark.24.02.08
				User_RegInfo[4].PassNOD = (SU100_UI3_All_Byte[0] & 0x0F);
				User_RegInfo[5].PassNOD = (SU100_UI3_All_Byte[2] & 0x0F);
				User_RegInfo[6].PassNOD = (SU100_UI3_All_Byte[4] & 0x0F);
				User_RegInfo[7].PassNOD = (SU100_UI3_All_Byte[6] & 0x0F);
				//
				User_RegInfo[4].RFStatus = ( (SU100_UI3_All_Byte[0]>>4) & 0x01);
				User_RegInfo[5].RFStatus = ( (SU100_UI3_All_Byte[2]>>4) & 0x01);
				User_RegInfo[6].RFStatus = ( (SU100_UI3_All_Byte[4]>>4) & 0x01);
				User_RegInfo[7].RFStatus = ( (SU100_UI3_All_Byte[6]>>4) & 0x01);
				//
				User_RegInfo[4].RegFlag = (SU100_UI3_All_Byte[1] & 0x0F);
				User_RegInfo[5].RegFlag = (SU100_UI3_All_Byte[3] & 0x0F);
				User_RegInfo[6].RegFlag = (SU100_UI3_All_Byte[5] & 0x0F);
				User_RegInfo[7].RegFlag = (SU100_UI3_All_Byte[7] & 0x0F);
				//

				g_UI3_ChkTime = gMainTimerCnt_10m;	//chpark.10.12

			}
			break;

			case UI4:		// 1851208C //chpark.08.22
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				//memcpy(&SU100_UI4_All_Byte[0],&sData[0],8);
				for(int i=0;i<8;i++)
				{
					SU100_UI4_All_Byte[i] = sData[i];
					//printf("[UI4][%d] = %d \r\n",i,SU100_UI4_All_Byte[i]);
				}
				//chpark.24.02.08
				User_RegInfo[8].PassNOD  = (SU100_UI4_All_Byte[0] & 0x0F);
				User_RegInfo[9].PassNOD  = (SU100_UI4_All_Byte[2] & 0x0F);
				User_RegInfo[10].PassNOD = (SU100_UI4_All_Byte[4] & 0x0F);
				User_RegInfo[11].PassNOD = (SU100_UI4_All_Byte[6] & 0x0F);
				//
				User_RegInfo[8].RFStatus  = ( (SU100_UI4_All_Byte[0]>>4) & 0x01);
				User_RegInfo[9].RFStatus  = ( (SU100_UI4_All_Byte[2]>>4) & 0x01);
				User_RegInfo[10].RFStatus = ( (SU100_UI4_All_Byte[4]>>4) & 0x01);
				User_RegInfo[11].RFStatus = ( (SU100_UI4_All_Byte[6]>>4) & 0x01);
				//
				User_RegInfo[8].RegFlag  = (SU100_UI4_All_Byte[1] & 0x0F);
				User_RegInfo[9].RegFlag  = (SU100_UI4_All_Byte[3] & 0x0F);
				User_RegInfo[10].RegFlag = (SU100_UI4_All_Byte[5] & 0x0F);
				User_RegInfo[11].RegFlag = (SU100_UI4_All_Byte[7] & 0x0F);

				g_UI4_ChkTime = gMainTimerCnt_10m;	//chpark.10.12

			}
			break;

			case UI5:		// 1851218C //chpark.08.22
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				//memcpy(&SU100_UI5_All_Byte[0],&sData[0],8);
				for(int i=0;i<8;i++)
				{
					SU100_UI5_All_Byte[i] = sData[i];
					//printf("[UI5][%d] = %d \r\n",i,SU100_UI5_All_Byte[i]);
				}
				//chpark.24.02.08
				User_RegInfo[12].PassNOD  = (SU100_UI5_All_Byte[0] & 0x0F);
				User_RegInfo[13].PassNOD  = (SU100_UI5_All_Byte[2] & 0x0F);
				User_RegInfo[14].PassNOD  = (SU100_UI5_All_Byte[4] & 0x0F);
				//
				User_RegInfo[12].RFStatus  = ( (SU100_UI5_All_Byte[0]>>4) & 0x01);
				User_RegInfo[13].RFStatus  = ( (SU100_UI5_All_Byte[2]>>4) & 0x01);
				User_RegInfo[14].RFStatus  = ( (SU100_UI5_All_Byte[4]>>4) & 0x01);
				//
				User_RegInfo[12].RegFlag  = (SU100_UI5_All_Byte[1] & 0x0F);
				User_RegInfo[13].RegFlag  = (SU100_UI5_All_Byte[3] & 0x0F);
				User_RegInfo[14].RegFlag  = (SU100_UI5_All_Byte[5] & 0x0F);

				g_UI5_ChkTime = gMainTimerCnt_10m;	//chpark.10.12
	
			}
			break;

			case UM5:		// 18511C8C
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				SU400_UM5_0Byte_01bit = (sData[0])&0x03;	//chpark.10.30
				SU500_UM5_0Byte_23bit = (sData[0]>>2)&0x03;	//chpark.08.22
				SU600_UM5_1Byte_03bit = sData[1]&0x0F;		//chpark.08.22
				SU300_UM5_1Byte_45bit = (sData[1]>>4)&0x03;	//chpark.08.22
				SU200_UM5_1Byte_67bit = (sData[1]>>6)&0x03;	//chpark.08.22
				SU300_UM5_2Byte_07bit = sData[2];			//chpark.08.22

				for(int i=0;i<8;i++)
				{
					SU100_UM5_All_Byte[i] = sData[i];
//					printf("[UM5][%d] = %d \r\n",i,SU100_UM5_All_Byte[i]);
				}
//				printf("[SU300_UM5_2Byte_07bit] = %d \r\n",SU300_UM5_2Byte_07bit);				
//				printf("[SU400_UM5_0Byte_01bit][RASSCODE RET]=[%d] \r\n",SU400_UM5_0Byte_01bit);
//				printf("[SU500_UM5_0Byte_23bit][RFID RET]=[%d] \r\n",SU500_UM5_0Byte_23bit);

				g_UM5_ChkTime = gMainTimerCnt_10m;	//chpark.08.22			
			}
			break;

			case AUX_UL:	// 1C501FE4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint8_t i=0;
				
				static uint8_t Aux1_FL_Max,Aux1_FL_Min;
				static uint8_t Aux2_FL_Max,Aux2_FL_Min;
				static uint8_t Aux4_FL_Max,Aux4_FL_Min;
				
				S1_AUX1_FlowRate_Max = sData[0];
				S1_AUX1_FlowRate_Min = sData[1];
				S2_AUX2_FlowRate_Max = sData[2];
				S2_AUX2_FlowRate_Min = sData[3];
				S2_AUX4_FlowRate_Max = sData[4];
				S2_AUX4_FlowRate_Min = sData[5];

				if((Aux1_FL_Max != S1_AUX1_FlowRate_Max) || (Aux1_FL_Min != S1_AUX1_FlowRate_Min))
				{
					Aux1_FL_Max = S1_AUX1_FlowRate_Max;
					Aux1_FL_Min = S1_AUX1_FlowRate_Min;

					for(i=0;i<3;i++)
					{
						if(M7_UserSet[i].A_Value > S1_AUX1_FlowRate_Max)
							M7_UserSet[i].A_Value = S1_AUX1_FlowRate_Max;
						else if(M7_UserSet[i].A_Value < S1_AUX1_FlowRate_Min)
							M7_UserSet[i].A_Value = S1_AUX1_FlowRate_Min;

						if(M7_UserSet[i].B_Value > S1_AUX1_FlowRate_Max)
							M7_UserSet[i].B_Value = S1_AUX1_FlowRate_Max;
						else if(M7_UserSet[i].B_Value < S1_AUX1_FlowRate_Min)
							M7_UserSet[i].B_Value = S1_AUX1_FlowRate_Min;
					}
				}

				if((Aux2_FL_Max != S2_AUX2_FlowRate_Max) || (Aux2_FL_Min != S2_AUX2_FlowRate_Min))
				{
					Aux2_FL_Max = S2_AUX2_FlowRate_Max;
					Aux2_FL_Min = S2_AUX2_FlowRate_Min;

					if(M7_UserAUX24Set.C_Value > S2_AUX2_FlowRate_Max)
						M7_UserAUX24Set.C_Value = S2_AUX2_FlowRate_Max;
					else if(M7_UserAUX24Set.C_Value < S2_AUX2_FlowRate_Min)
						M7_UserAUX24Set.C_Value = S2_AUX2_FlowRate_Min;

					if(M7_UserAUX24Set.D_Value > S2_AUX2_FlowRate_Max)
						M7_UserAUX24Set.D_Value = S2_AUX2_FlowRate_Max;
					else if(M7_UserAUX24Set.D_Value < S2_AUX2_FlowRate_Min)
						M7_UserAUX24Set.D_Value = S2_AUX2_FlowRate_Min;
				}

				if((Aux4_FL_Max != S2_AUX4_FlowRate_Max) || (Aux4_FL_Min != S2_AUX4_FlowRate_Min))
				{
					Aux4_FL_Max = S2_AUX4_FlowRate_Max;
					Aux4_FL_Min = S2_AUX4_FlowRate_Min;

					if(M7_UserAUX24Set.G_Value > S2_AUX4_FlowRate_Max)
						M7_UserAUX24Set.G_Value = S2_AUX4_FlowRate_Max;
					else if(M7_UserAUX24Set.G_Value < S2_AUX4_FlowRate_Min)
						M7_UserAUX24Set.G_Value = S2_AUX4_FlowRate_Min;

					if(M7_UserAUX24Set.H_Value > S2_AUX4_FlowRate_Max)
						M7_UserAUX24Set.H_Value = S2_AUX4_FlowRate_Max;
					else if(M7_UserAUX24Set.H_Value < S2_AUX4_FlowRate_Min)
						M7_UserAUX24Set.H_Value = S2_AUX4_FlowRate_Min;
				}

			}	
			break;

			case AUX_PUL:	// 1C5025E4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint8_t i=0;

				static uint8_t Aux1_P_Max,Aux1_P_Min;
				static uint8_t Aux2_P_Max,Aux2_P_Min;
				static uint8_t Aux4_P_Max,Aux4_P_Min;
				
				S1_AUX1_Pressure_Max = sData[0];
				S1_AUX1_Pressure_Min = sData[1];
				S2_AUX2_Pressure_Max = sData[2];
				S2_AUX2_Pressure_Min = sData[3];
				S2_AUX4_Pressure_Max = sData[4];
				S2_AUX4_Pressure_Min = sData[5];

				if((Aux1_P_Max != S1_AUX1_Pressure_Max)||(Aux1_P_Min != S1_AUX1_Pressure_Min))
				{
					Aux1_P_Max = S1_AUX1_Pressure_Max;
					Aux1_P_Min = S1_AUX1_Pressure_Min;

					for(i=0;i<3;i++)
					{
						if(M7_UserSet[i].P1_Value > S1_AUX1_Pressure_Max)
							M7_UserSet[i].P1_Value = S1_AUX1_Pressure_Max;
						else if(M7_UserSet[i].P1_Value < S1_AUX1_Pressure_Min)
							M7_UserSet[i].P1_Value = S1_AUX1_Pressure_Min;
					}
				}

				if((Aux2_P_Max != S2_AUX2_Pressure_Max)||(Aux2_P_Min != S2_AUX2_Pressure_Min))
				{
					Aux2_P_Max = S2_AUX2_Pressure_Max;
					Aux2_P_Min = S2_AUX2_Pressure_Min;

					if(M7_UserAUX24Set.P2_Value > S2_AUX2_Pressure_Max)
						M7_UserAUX24Set.P2_Value = S2_AUX2_Pressure_Max;
					else if(M7_UserAUX24Set.P2_Value < S2_AUX2_Pressure_Min)
						M7_UserAUX24Set.P2_Value = S2_AUX2_Pressure_Min;
				}

				if((Aux4_P_Max != S2_AUX4_Pressure_Max)||(Aux4_P_Min != S2_AUX4_Pressure_Min))
				{
					Aux4_P_Max = S2_AUX4_Pressure_Max;
					Aux4_P_Min = S2_AUX4_Pressure_Min;

					if(M7_UserAUX24Set.P4_Value > S2_AUX4_Pressure_Max)
						M7_UserAUX24Set.P4_Value = S2_AUX4_Pressure_Max;
					else if(M7_UserAUX24Set.P4_Value < S2_AUX4_Pressure_Min)
						M7_UserAUX24Set.P4_Value = S2_AUX4_Pressure_Min;
				}

			}
			break;
			case AUX_UL4:			// 1C5018E4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if(sData[0] == 0)
				{
					S1_1WAY_1st_Y[0] = sData[1];
					S1_1WAY_1st_Y[1] = sData[2];
					S1_1WAY_1st_Y[2] = sData[3];
					S1_1WAY_1st_Y[3] = sData[4];
					S1_1WAY_1st_Y[4] = sData[5];
					S1_1WAY_1st_Y[5] = sData[6];
				}
				else if(sData[0] == 1)
				{
					S1_1WAY_2nd_Y[0] = sData[1];
					S1_1WAY_2nd_Y[1] = sData[2];
					S1_1WAY_2nd_Y[2] = sData[3];
					S1_1WAY_2nd_Y[3] = sData[4];
					S1_1WAY_2nd_Y[4] = sData[5];
					S1_1WAY_2nd_Y[5] = sData[6];
				}
				else if(sData[0] == 2)
				{
					S1_2WAY_Y[0] = sData[1];
					S1_2WAY_Y[1] = sData[2];
					S1_2WAY_Y[2] = sData[3];
					S1_2WAY_Y[3] = sData[4];
					S1_2WAY_Y[4] = sData[5];
					S1_2WAY_Y[5] = sData[6];
				}
				else if(sData[0] == 3)
				{
					S1_4WAY_Y[0] = sData[1];
					S1_4WAY_Y[1] = sData[2];
					S1_4WAY_Y[2] = sData[3];
					S1_4WAY_Y[3] = sData[4];
					S1_4WAY_Y[4] = sData[5];
					S1_4WAY_Y[5] = sData[6];
				}
			}	
			break;
			
			case AUX_UL5:			// 1C5019E4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if(sData[0] == 0)
				{
					S1_1WAY_1st_X[0] = sData[1];
					S1_1WAY_1st_X[1] = sData[2];
					S1_1WAY_1st_X[2] = sData[3];
					S1_1WAY_1st_X[3] = sData[4];
					S1_1WAY_1st_X[4] = sData[5];
					S1_1WAY_1st_X[5] = sData[6];
				}
				else if(sData[0] == 1)
				{
					S1_1WAY_2nd_X[0] = sData[1];
					S1_1WAY_2nd_X[1] = sData[2];
					S1_1WAY_2nd_X[2] = sData[3];
					S1_1WAY_2nd_X[3] = sData[4];
					S1_1WAY_2nd_X[4] = sData[5];
					S1_1WAY_2nd_X[5] = sData[6];
				}
				else if(sData[0] == 2)
				{
					S1_2WAY_X[0] = sData[1];
					S1_2WAY_X[1] = sData[2];
					S1_2WAY_X[2] = sData[3];
					S1_2WAY_X[3] = sData[4];
					S1_2WAY_X[4] = sData[5];
					S1_2WAY_X[5] = sData[6];
				}
				else if(sData[0] == 3)
				{
					S1_4WAY_X[0] = sData[1];
					S1_4WAY_X[1] = sData[2];
					S1_4WAY_X[2] = sData[3];
					S1_4WAY_X[3] = sData[4];
					S1_4WAY_X[4] = sData[5];
					S1_4WAY_X[5] = sData[6];
				}
			}	
			break;

			case AG_I:		// 1C504AE4
			{
				if (MainLayerFlag == 1)
				{
					uint8_t ZoomFlag = 0x80;  // 0:zoom
					uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
					uint8_t tData;
	
					if ((Main_wZoomIndex>WARNING_ICON_CNT)&&((Main_WEngStopSw == 0x0)&&(Main_WSafetyStart == 0x0)))
					{
						ZoomFlag = 0;
					}
					GetRxD2bitF(((sData[1]>>6)&0x01),MainFunctionData[cF_20_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[1]>>0)&0x01),MainFunctionData[cF_21_B-cF_01_B],ZoomFlag);
					GetRxD2bitF(((sData[0]>>0)&0x01),MainFunctionData[cF_22_B-cF_01_B],0x80);	
					if ((ZoomFlag==0x00)&&((cF_01_B<=Main_wZoomIndex)||(Main_wZoomIndex==0xFF)))
					{					
						ChkZoombitF(cF_20_B)
						else ChkZoombitF(cF_21_B)
						
						
//						else ChkZoombitF(cF_22_B)
					}
					AG_I_Time = gMainTimerCnt_10m;
	#if 0
					printf("[AG_I] cF_20_B : [%x]\n", MainFunctionData[cF_20_B-cF_01_B]);
					printf("[AG_I] cF_21_B : [%x]\n", MainFunctionData[cF_21_B-cF_01_B]);
					printf("[AG_I] cF_22_B : [%x]\n", MainFunctionData[cF_22_B-cF_01_B]);
	
					printf("Main_wZoomIndex : [%d]\n", Main_wZoomIndex);
					printf("Main_fZoomIndex : [%d]\n\n", Main_fZoomIndex);
	#endif
				
	
					AG_ActiveStatus = (sData[0]&0x01);				// CAN[AG_I] 0byte,0bit 0:Not Active , 1:Active On
					AG_SupplyFlag = (sData[1]&0x01);					// CAN[AG_I] 1byte,0bit 0:No Supply(STOP) , 1:Force Supply(WORK)
					AG_ErrorStatus = ((sData[1]>>6)&0x01);		// CAN[AG_I] 1byte,6bit 0:Normal , 1:Error
					AG_MaxLimit = sData[2];				// CAN[AG_I] 2byte
					AG_MinLimit = sData[3];				// CAN[AG_I] 3byte
					AG_OnTime = sData[4];					// CAN[AG_I] 4byte
	
					if(S4_2SecFlag == 0)
						AG_Time = AG_OnTime;
				}
			}
			break;


			//==================================================================
			//chpark.
			case TCMD:
			{
				char TC_Buf[1];
				uint16_t nTmpCID,nTmpStatus;
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[1]<<8) | sData[0])==0xFFFF) //커맨???�이??					
				{
					gMB_IO_Value.TCMD_01Bytes = 0xFFFF;
				}
				else
				{
					gMB_IO_Value.TCMD_01Bytes = (sData[1]<<8 | sData[0]);	
				}

				if (sData[2]==0xFF) //?�신�??�드?�스
					gMB_IO_Value.TCMD_2Byte = 0xFF;
				else
					gMB_IO_Value.TCMD_2Byte = sData[2];//chpark.

				if (sData[3]==0xFF) //?�태(1:?�료 2:�?비중 3:�?비완�?4:?�행�?
					gMB_IO_Value.TCMD_3Byte_47bit = 0xFF;
				else
					gMB_IO_Value.TCMD_3Byte_47bit = (sData[3]>>4)&0x0F;//chpark.

				if (sData[4]==0xFF) //컨트롤러 �?					
					gMB_IO_Value.TCMD_4Byte = 0xFF;
				else
					gMB_IO_Value.TCMD_4Byte = sData[4];//chpark.


//				printf("TCMD ID:%d  Addr:%x  Status:%d  ControlID:%d\r\n",gMB_IO_Value.TCMD_01Bytes,gMB_IO_Value.TCMD_2Byte,gMB_IO_Value.TCMD_3Byte_47bit,gMB_IO_Value.TCMD_4Byte);
				
				nTmpCID 	= gMB_IO_Value.TCMD_01Bytes;
				nTmpStatus 	= gMB_IO_Value.TCMD_3Byte_47bit;
//				printf("TCMD ID:%d  Addr:%x  Status:%d  ControlID:%d\r\n",nTmpCID,gMB_IO_Value.TCMD_2Byte,nTmpStatus,gMB_IO_Value.TCMD_4Byte);
				
				if(nTmpCID==10)
				{
					//Pedal Calibration.
					if(nTmpStatus==1)
					{
						TC_Buf[0] = PEDAL_INIT;
					}
					else if(nTmpStatus==2)
					{
						TC_Buf[0] = PEDAL_STEP2_OPEN;
					}
					else if(nTmpStatus==3)
					{
						TC_Buf[0] = PEDAL_STEP3_OPEN;
					}
					else
					{
						printf("nTmpCID==10 nTmpStatus Error:%d\r\n",nTmpStatus);
					}
					
					ituSceneSendEvent(&theScene, EVENT_CUSTOM20_MB400, TC_Buf);
//					printf("ituSceneSendEvent[10]:%d\r\n",nTmpStatus);
				}
				else if(nTmpCID==11)
				{
					//Blade Calibration.
					if(nTmpStatus==1)
					{
						TC_Buf[0] = BLADE_INIT;
					}
					else if(nTmpStatus==2)
					{
						TC_Buf[0] = BLADE_STEP2_OPEN;
					}
					else if(nTmpStatus==3)
					{
						TC_Buf[0] = BLADE_STEP3_OPEN;
					}
					else
					{
						printf("nTmpCID==11 nTmpStatus Error:%d\r\n",nTmpStatus);
					}

					ituSceneSendEvent(&theScene, EVENT_CUSTOM20_MB400, TC_Buf);
//					printf("ituSceneSendEvent[11]:%d\r\n",nTmpStatus);
				}
				else
				{
					//?�러.
					printf("TCMD ID Error:%d\r\n",nTmpCID);

				}


			}
			break;

			case XO_MICOM_SOFT_REQ: 	//78EA01E4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				memcpy(&gMicomFirmVer, &sData[1],sData[0]);
//				printf("XO_MICOM_SOFT_REQ : [%d][%s]\r\n",sData[0],gMicomFirmVer);
			}
			break;
			case XO_MICOM_FIRM_UP: 	//78EA01E4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				MicomFirm_ReciveData = sData[0];
				printf("XO_MICOM_FIRM_UP : [0x%X]\r\n",MicomFirm_ReciveData);
			}
			break;
			case MAIN_CON:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				/*
				if (((sData[5]<<24) | (sData[4]<<16) | (sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.AT1S_2345Bytes = 0xFFFF;
				else
				{
					gMB_IO_Value.AT1S_2345Bytes = (sData[5]<<24 | sData[4]<<16 | sData[3]<<8 | sData[2]);
					
				}
				*/

				//char ptStringTmp1[10];
				//memset(ptStringTmp1,0,10);

				//printf("MAIN_CON %c%c%c%c%c%c%c\r\n",sData[1],sData[2],sData[3],sData[4],sData[5],sData[6],sData[7]);
				//sprintf(ptStringTmp1,"%c%c%c%c%c%c%c", sData[1],sData[2],sData[3],sData[4],sData[5],sData[6],sData[7]);				
				memcpy(&gMB_IO_Value.MAIN_CON_1234567Bytes, &sData[1],7);


				char ptStringTmp2[10];
				memset(ptStringTmp2,0,10);
				memcpy(ptStringTmp2, &sData[1],7);
				
                //MAIN_CON_1234567Bytes
				
//				printf("MAIN_CON111::==========> %s\r\n",ptStringTmp2);
//				printf("MAIN_CON222::==========> %s\r\n",gMB_IO_Value.MAIN_CON_1234567Bytes);


			}
			break;

			case EEC2:	//0x0CF00300 //chpark
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (sData[2]==0xFF)
					gMB_IO_Value.EEC2_2Byte = 0xFF;
				else
					gMB_IO_Value.EEC2_2Byte = sData[2];//chpark.

				gMB_IO_Value.EEC2_2Time = gMainTimerCnt_10m;//chpark.07.05
				
			}
			break;

			
			case EFL_P1:	//0x18FEEF00 //chpark
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (sData[3]==0xFF)
					gMB_IO_Value.EFL_P1_3Byte = 0xFF;
				else
					gMB_IO_Value.EFL_P1_3Byte = sData[3];//chpark.

				gMB_IO_Value.EFL_P1_3Time =	gMainTimerCnt_10m;//chpark.07.05

				
			}
			break;

			case LFE:	//0x18FEF200 //chpark
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[1]<<8) | sData[0])==0xFFFF)
					gMB_IO_Value.LFE_01Bytes = 0xFFFF;
				else
				{
					gMB_IO_Value.LFE_01Bytes = (sData[1]<<8 | sData[0]);
					
				}

				gMB_IO_Value.LFE_01Time =	gMainTimerCnt_10m;//chpark.07.05

			}
			break;
			
			case AMB:	//0x18FEF500 //chpark
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				gMB_IO_Value.AMB_0Byte = sData[0];//chpark.
				
				if (((sData[4]<<8) | sData[3])==0xFFFF)
					gMB_IO_Value.AMB_34Bytes = 0xFFFF;
				else if(sData[4]==0xFF)
					gMB_IO_Value.AMB_34Bytes = 0xFFFF;//chpark.10.19
				else
				{
					gMB_IO_Value.AMB_34Bytes = (sData[4]<<8 | sData[3]);
					
				}
				gMB_IO_Value.AMB_0Time	= 	gMainTimerCnt_10m;//chpark.10.18
				gMB_IO_Value.AMB_34Time =	gMainTimerCnt_10m;//chpark.07.05
			}
			break;

			case IC1:	//0x18FEF600 //chpark
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				/*
				if (sData[1]==0xFF)
					gMB_IO_Value.IC1_1Byte = 0xFF;
				else
					gMB_IO_Value.IC1_1Byte = sData[1];//chpark
				*/

				if (sData[2]==0xFF)
					gMB_IO_Value.IC1_2Byte = 0xFF;
				else
					gMB_IO_Value.IC1_2Byte = sData[2];//chpark

				if (sData[3]==0xFF)
					gMB_IO_Value.IC1_3Byte = 0xFF;
				else
					gMB_IO_Value.IC1_3Byte = sData[3];//chpark

				//gMB_IO_Value.IC1_1Time =	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.IC1_2Time =	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.IC1_3Time =	gMainTimerCnt_10m;

			}
			break;

			case EFL_P2:	//0x18FEDB00 //chpark
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.EFL_P2_23Bytes = 0xFFFF;
				else if(sData[3]==0xFF)
					gMB_IO_Value.EFL_P2_23Bytes = 0xFFFF; //chpark.10.19
				else
				{
					gMB_IO_Value.EFL_P2_23Bytes = (sData[3]<<8 | sData[2]);
				}
				
				gMB_IO_Value.EFL_P2_23Time =	gMainTimerCnt_10m;//chpark.07.05

			}
			break;

			case TFAC:	//0x10F01A00 //chpark
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[1]<<8) | sData[0])==0xFFFF)
					gMB_IO_Value.TFAC_01Bytes = 0xFFFF; 
				else if(sData[1]==0xFF)
					gMB_IO_Value.TFAC_01Bytes = 0xFFFF; //chpark.10.19
				else
				{
					gMB_IO_Value.TFAC_01Bytes = (sData[1]<<8 | sData[0]);
					
				}
				
				gMB_IO_Value.TFAC_01Time =	gMainTimerCnt_10m;//chpark.07.05

				

			}
			break;

			case ET2:	//0x0CFEA400 //chpark
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[5]<<8) | sData[4])==0xFFFF)
					gMB_IO_Value.ET2_45Bytes = 0xFFFF;
				else if(sData[5]==0xFF)
					gMB_IO_Value.ET2_45Bytes = 0xFFFF;//chpark.10.19
				else
				{
					gMB_IO_Value.ET2_45Bytes = (sData[5]<<8 | sData[4]);
					
				}


				if (((sData[7]<<8) | sData[6])==0xFFFF)
					gMB_IO_Value.ET2_67Bytes = 0xFFFF;
				else if(sData[7]==0xFF)
					gMB_IO_Value.ET2_67Bytes = 0xFFFF;//chpark.10.19
				else
				{
					gMB_IO_Value.ET2_67Bytes = (sData[7]<<8 | sData[6]);
					
				}
				
				gMB_IO_Value.ET2_45Time =	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.ET2_67Time =	gMainTimerCnt_10m;//chpark.07.05


			}
			break;
			//==================================================================
			case AT1IMG:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[5]<<8) | sData[4])==0xFFFF)
					gMB_IO_Value.AT1IMG_45Bytes = 0xFFFF;
				else if(sData[5]==0xFF)
					gMB_IO_Value.AT1IMG_45Bytes = 0xFFFF;//chpark.10.19
				else
				{
					gMB_IO_Value.AT1IMG_45Bytes = (sData[5]<<8 | sData[4]);
					
				}

				if (((sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.AT1IMG_23Bytes = 0xFFFF;
				else if(sData[3]==0xFF)
					gMB_IO_Value.AT1IMG_23Bytes = 0xFFFF;//chpark.10.19
				else
				{
					gMB_IO_Value.AT1IMG_23Bytes = (sData[3]<<8 | sData[2]);
					
				}
				
				gMB_IO_Value.AT1IMG_23Time =	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.AT1IMG_45Time =	gMainTimerCnt_10m;//chpark.07.05


			}

			break;

			case AT1IG2:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.AT1IG2_23Bytes = 0xFFFF;
				else if(sData[3]==0xFF)
					gMB_IO_Value.AT1IG2_23Bytes = 0xFFFF;//chpark.10.19
				else
				{
					gMB_IO_Value.AT1IG2_23Bytes = (sData[3]<<8 | sData[2]);
					
				}

				if (((sData[1]<<8) | sData[0])==0xFFFF)
					gMB_IO_Value.AT1IG2_01Bytes = 0xFFFF;
				else if(sData[1]==0xFF)
					gMB_IO_Value.AT1IG2_01Bytes = 0xFFFF;//chpark.10.19
				else
				{
					gMB_IO_Value.AT1IG2_01Bytes = (sData[1]<<8 | sData[0]);
					
				}

				gMB_IO_Value.AT1IG2_23Time =	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.AT1IG2_01Time =	gMainTimerCnt_10m;//chpark.07.05


			}

			break;

			case AT1S:	//  18FD7B00
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[5]<<24) | (sData[4]<<16) | (sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.AT1S_2345Bytes = 0xFFFFFFFF;
				else if(sData[5]==0xFF)
					gMB_IO_Value.AT1S_2345Bytes = 0xFFFFFFFF;//chpark.10.19
				else
				{
					gMB_IO_Value.AT1S_2345Bytes = (sData[5]<<24 | sData[4]<<16 | sData[3]<<8 | sData[2]);
					
				}
			
				if (sData[0]==0xFF)
					gMB_IO_Value.AT1S_0Byte = 0xFF;
				else if (sData[0]>0xFA)	// JPHong 2023-08-23
					gMB_IO_Value.AT1S_0Byte = 0xFA;
				else
					gMB_IO_Value.AT1S_0Byte = sData[0];//chpark

				if (sData[1]==0xFF)
					gMB_IO_Value.AT1S_1Byte = 0xFF;
				else if (sData[1]>0xFA)	// JPHong 2023-08-23
					gMB_IO_Value.AT1S_1Byte = 0xFA; 	/// ���?Ȯ���Ǹ� �ٽ� Ȯ���Ѵ�
				else
					gMB_IO_Value.AT1S_1Byte = sData[1];//chpark


				M5_PM = gMB_IO_Value.AT1S_0Byte;		// JPHong 2023-08-29
					
				M5_ASH = gMB_IO_Value.AT1S_1Byte;  	// JPHong 2023-08-29
				
				M5_PM_Timer = gMainTimerCnt_10m;
				M5_ASH_Timer = gMainTimerCnt_10m;

				gMB_IO_Value.AT1S_0Time 	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.AT1S_1Time 	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.AT1S_2345Time 	=	gMainTimerCnt_10m;//chpark.07.05

			}

			break;

			case A1SCREGT1:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[1]<<8) | sData[0])==0xFFFF)
					gMB_IO_Value.A1SCREGT1_01Bytes = 0xFFFF;
				else if(sData[1]==0xFF)
					gMB_IO_Value.A1SCREGT1_01Bytes = 0xFFFF;//chpark.10.19
				else
				{
					gMB_IO_Value.A1SCREGT1_01Bytes = (sData[1]<<8 | sData[0]);
				}
				gMB_IO_Value.A1SCREGT1_01Time 	=	gMainTimerCnt_10m;//chpark.07.05

			
			}

			break;


			case A1SCRDSI1:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[1]<<8) | sData[0])==0xFFFF)
					gMB_IO_Value.A1SCRDSI1_01Bytes = 0xFFFF;
				else if(sData[1]==0xFF)
					gMB_IO_Value.A1SCRDSI1_01Bytes = 0xFFFF;//chpark.10.19
				else
				{
					gMB_IO_Value.A1SCRDSI1_01Bytes = (sData[1]<<8 | sData[0]);
					
				}

				if (sData[5]==0xFF)
					gMB_IO_Value.A1SCRDSI1_5Byte = 0xFF;
				else
					gMB_IO_Value.A1SCRDSI1_5Byte = sData[5];//chpark

				
				gMB_IO_Value.A1SCRDSI1_01Time 	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.A1SCRDSI1_5Time 	=	gMainTimerCnt_10m;//chpark.07.05

			
			}

			break;
			//==================================================================
			//chpark.
			//ENGINE3
			case AUX1_IO://chpark.09.13
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.AUX1_IO_23Bytes = 0xFFFF;
				else if (sData[3]==0xFF)
					gMB_IO_Value.AUX1_IO_23Bytes = 0xFFFF; //chpark.10.19
				else
				{
					gMB_IO_Value.AUX1_IO_23Bytes = (sData[3]<<8 | sData[2]);
				}

				if (((sData[5]<<8) | sData[4])==0xFFFF)
					gMB_IO_Value.AUX1_IO_45Bytes = 0xFFFF;
				else if (sData[5]==0xFF)
					gMB_IO_Value.AUX1_IO_45Bytes = 0xFFFF; //chpark.10.19
				else
				{
					gMB_IO_Value.AUX1_IO_45Bytes = (sData[5]<<8 | sData[4]);
				}

				if (sData[7]==0xFF)
				{
					gMB_IO_Value.AUX1_IO_7Byte = 0xFF;
				}
				else
				{
					gMB_IO_Value.AUX1_IO_7Byte = sData[7];//chpark
				}

				//
				//gMB_IO_Value.AUX1_IO_01Time 		=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.AUX1_IO_23Time 		=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.AUX1_IO_45Time 		=	gMainTimerCnt_10m;//chpark.07.05
				//gMB_IO_Value.AUX1_IO_6Byte_01Time 	=	gMainTimerCnt_10m;//chpark.07.05
				//gMB_IO_Value.AUX1_IO_6Byte_23Time 	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.AUX1_IO_7Time 			=	gMainTimerCnt_10m;//chpark.07.05
				//

//				printf("AUX1_IO========Reveive%d\n");
			}

			break;

			case AUX2_IO://chpark.09.13
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.AUX2_IO_23Bytes = 0xFFFF;
				else if (sData[3]==0xFF)
					gMB_IO_Value.AUX2_IO_23Bytes = 0xFFFF; //chpark.10.19
				else
				{
					gMB_IO_Value.AUX2_IO_23Bytes = (sData[3]<<8 | sData[2]);
				}

				if (((sData[5]<<8) | sData[4])==0xFFFF)
					gMB_IO_Value.AUX2_IO_45Bytes = 0xFFFF;
				else if (sData[5]==0xFF)
					gMB_IO_Value.AUX2_IO_45Bytes = 0xFFFF; //chpark.10.19
				else
				{
					gMB_IO_Value.AUX2_IO_45Bytes = (sData[5]<<8 | sData[4]);
				}

				if (sData[7]==0xFF)
				{
					gMB_IO_Value.AUX2_IO_7Byte = 0xFF;
				}
				else
				{
					gMB_IO_Value.AUX2_IO_7Byte = sData[7];//chpark
				}

				//
				//gMB_IO_Value.AUX2_IO_01Time 		=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.AUX2_IO_23Time 		=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.AUX2_IO_45Time 		=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.AUX2_IO_7Time 			=	gMainTimerCnt_10m;//chpark.07.05
				//


			}

			break;

			case AUX4_IO:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.AUX4_IO_23Bytes = 0xFFFF;
				else if (sData[3]==0xFF)
					gMB_IO_Value.AUX4_IO_23Bytes = 0xFFFF; //chpark.10.19
				else
				{
					gMB_IO_Value.AUX4_IO_23Bytes = (sData[3]<<8 | sData[2]);
				}

				if (((sData[5]<<8) | sData[4])==0xFFFF)
					gMB_IO_Value.AUX4_IO_45Bytes = 0xFFFF;
				else if (sData[5]==0xFF)
					gMB_IO_Value.AUX4_IO_45Bytes = 0xFFFF; //chpark.10.19
				else
				{
					gMB_IO_Value.AUX4_IO_45Bytes = (sData[5]<<8 | sData[4]);
				}

				if (sData[7]==0xFF)
				{
					gMB_IO_Value.AUX4_IO_7Byte = 0xFF;
				}
				else
				{
					gMB_IO_Value.AUX4_IO_7Byte = sData[7];//chpark
				}
				//
				gMB_IO_Value.AUX4_IO_23Time		=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.AUX4_IO_45Time 	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.AUX4_IO_7Time 		=	gMainTimerCnt_10m;//chpark.07.05
				//

			}

			break;



			case IN10:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (sData[6]==0xFF)
				{
					gMB_IO_Value.IN10_6Byte = 0xFF;
				}
				else
				{
					gMB_IO_Value.IN10_6Byte = sData[6];//chpark
				}
				
				if (sData[7]==0xFF)
				{
					gMB_IO_Value.IN10_7Byte = 0xFF;
				}
				else
				{
					gMB_IO_Value.IN10_7Byte = sData[7];//chpark
				}

				//
				gMB_IO_Value.IN10_6Time		=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.IN10_7Time 	=	gMainTimerCnt_10m;//chpark.07.05
				
			}

			break;

			
			case IN16://chpark.09.13
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				//if (((sData[7]<<8) | sData[6])==0xFFFF)
				//	gMB_IO_Value.IN16_67Bytes = 0xFFFF;
				//else
				//{
				//	gMB_IO_Value.IN16_67Bytes = (sData[7]<<8 | sData[6]);	
				//}

				//if (((sData[3]<<8) | sData[2])==0xFFFF)
				//	gMB_IO_Value.IN16_23Bytes = 0xFFFF;
				//else
				//{
				//	gMB_IO_Value.IN16_23Bytes = (sData[3]<<8 | sData[2]);	
				//}

				//
				//gMB_IO_Value.IN16_67Time	=	gMainTimerCnt_10m;//chpark.07.05
				//gMB_IO_Value.IN16_23Time 	=	gMainTimerCnt_10m;//chpark.07.05
				//

			}

			break;


			case IN17:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (sData[5]==0xFF)
				{
					gMB_IO_Value.IN17_5Byte = 0xFF;
				}
				else
				{
					gMB_IO_Value.IN17_5Byte = sData[5];//chpark
				}

				gMB_IO_Value.IN17_5Time	=	gMainTimerCnt_10m;//chpark.07.05

			}

			break;



			case IN21:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[1]<<8) | sData[0])==0xFFFF)
					gMB_IO_Value.IN21_01Bytes = 0xFFFF;
				else if (sData[1]==0xFF)
					gMB_IO_Value.IN21_01Bytes = 0xFFFF; //chpark.10.19
				else
				{
					gMB_IO_Value.IN21_01Bytes = ( (sData[1]<<8) | sData[0]);
				}

				gMB_IO_Value.IN21_01Time	=	gMainTimerCnt_10m;//chpark.07.05
			}

			break;


			case IN23:
			{
				/*
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				if (((sData[5]<<8) | sData[4])==0xFFFF)
					gMB_IO_Value.IN23_45Bytes = 0xFFFF;
				else if (sData[5]==0xFF)
					gMB_IO_Value.IN23_45Bytes = 0xFFFF; //chpark.10.19
				else
				{
					gMB_IO_Value.IN23_45Bytes = (sData[5]<<8 | sData[4]);
				}
				
				if (((sData[7]<<8) | sData[6])==0xFFFF)
					gMB_IO_Value.IN23_67Bytes = 0xFFFF;
				else if (sData[7]==0xFF)
					gMB_IO_Value.IN23_45Bytes = 0xFFFF; //chpark.10.19
				else
				{
					gMB_IO_Value.IN23_67Bytes = (sData[7]<<8 | sData[6]);
				}
				
				//
				gMB_IO_Value.IN23_45Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.IN23_67Time	=	gMainTimerCnt_10m;//chpark.07.05
				//
				*/
			}

			break;
			
//-------

			//==================================================================
			//chpark
			//Option
			case OPI:		// 185012E4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				OPI3Byte23bit=(sData[3]>>2)&0x03; 		
				OPI1Byte67bit=(sData[1]>>6)&0x03; 
				
				OPI0Byte67bit=(sData[0]>>6)&0x03;
				OPI3Byte01bit=(sData[3]>>0)&0x03;
				OPI7Byte67bit=(sData[7]>>6)&0x03;	
				OPI2Byte67bit=(sData[2]>>6)&0x03;	
				OPI0Byte45bit=(sData[0]>>4)&0x03;	
				
				OPI0Byte23bit=(sData[0]>>2)&0x03;	
				
				OPI7Byte23bit=(sData[7]>>2)&0x03;

				OPI6Byte01bit=(sData[6])&0x03;
				OPI6Byte23bit=(sData[6]>>2)&0x03;

				// jhlee
				M7_P1_OPI_Flag = OPI6Byte01bit;
				M7_P1_Timer = gMainTimerCnt_10m;

				M7_Way_OPI_Flag = OPI0Byte23bit;
				M7_Way_Timer = gMainTimerCnt_10m;
				
				M7_AG_OptFlag = OPI7Byte23bit;

		
				M7_P2_OPI_Flag = OPI6Byte23bit;
				M7_P4_OPI_Flag = OPI0Byte67bit;
				M7_P24_OPI_Flag = OPI7Byte67bit;
				//
				gMB_IO_Value.OPI_AllByte_Time = gMainTimerCnt_10m;//chpark.07.05
				//

			} 
			break;

			case OPI2:		// 185038E4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				
				OPI2_0Byte_01bit=(sData[0])&0x03;
				
				#ifdef USED_CAM_2CH
				#else
					OPI2_0Byte_01bit = 1;		// JPHong 2023-12-24
				#endif

				OPI2_0Byte_45bit=(sData[0]>>4)&0x03;
				
				OPI2_1Byte_23bit=(sData[1]>>2)&0x03;
				

				gMB_IO_Value.OPI2_AllByte_Time = gMainTimerCnt_10m;//chpark.07.05
			}
			break;
			case OPI3:		// 185039E4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint8_t tData;
				tData = (sData[2]>>0)&0x03;
				
				#ifdef TEST_CAM_COLOR
					g_nM7_StartType = M7_MODE_PUSH;
				#else
					if ((tData==0)||(tData==3))	// 0:M7_MODE_PUSH  1:M7_MODE_KEY  2:M7_MODE_LOCK
					{
						if (g_nM7_StartType!=M7_MODE_LOCK)
							g_nM7_StartType = M7_MODE_KEY;
					}
					else
					{
						g_nM7_StartType = M7_MODE_PUSH;
					}
				#endif
			}
			break;
			case FNI1:			// 1850E8E4
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint8_t tAutoDecel=0xFF;		// JPHong 2023-12-24
				uint8_t tAutoCamera=0xFF;
				
				tAutoDecel = (sData[7]>>6)&0x03;
				tAutoCamera = (sData[0]>>4)&0x03;
				
				if ((Option_Function.AutoDecel != tAutoDecel)||(Option_Function.AutoCamera != tAutoCamera))
				{
												
					Option_Function.AutoDecel = tAutoDecel;
					Option_Function.AutoCamera = tAutoCamera;
					XO_Save_Config(M7_UserIndex,true,0);
				}
				M7_AP_OptFlag = (sData[6]>>4)&0x03;
				
				S11_Auto1_Flag = (uint8_t)((sData[6]>>6)&0x03);			// FNI1 6byte,6~7bit
				S11_Auto2_Flag = (uint8_t)(sData[7]&0x03);					// FNI1 7byte,0~1bit
				S11_Auto12_Flag = (uint8_t)((sData[7]>>2)&0x03);			// FNI1 7byte,2~3bit
				
			}
			break;
			
			//==================================================================
			//chpark. IO
			case IN1://chpark.09.13
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				gMB_IO_Value.IN1_4Byte_4bit = (sData[4]>>4)&0x01; 	
				//gMB_IO_Value.IN1_3Byte_0bit = (sData[3])&0x01; //chpark.09.13

				gMB_IO_Value.IN1_2Byte_4bit = (sData[2]>>4)&0x01; 	

				//gMB_IO_Value.IN1_4Byte_6bit = (sData[4]>>6)&0x01; //chpark.09.13

				gMB_IO_Value.IN1_2Byte_2bit = (sData[2]>>2)&0x01;

				gMB_IO_Value.IN1_4Byte_0bit = (sData[4]>>0)&0x01; 
				gMB_IO_Value.IN1_2Byte_3bit = (sData[2]>>3)&0x01; 

				//gMB_IO_Value.IN1_1Byte_6bit = (sData[1]>>6)&0x01; //chpark.09.13		

				//gMB_IO_Value.IN1_2Byte_7bit = (sData[2]>>7)&0x01; //chpark.09.13
				gMB_IO_Value.IN1_4Byte_5bit = (sData[4]>>5)&0x01; //chpark.24.10.18
							
				//gMB_IO_Value.IN1_0Byte_3bit = (sData[0]>>3)&0x01; //chpark.09.13
				//gMB_IO_Value.IN1_1Byte_0bit = (sData[1]>>0)&0x01; //chpark.09.13

				//gMB_IO_Value.IN1_0Byte_2bit = (sData[0]>>2)&0x01; //chpark.09.13
				//========
				gMB_IO_Value.IN1_4Byte_4Time	=	gMainTimerCnt_10m;//chpark.07.05
				//gMB_IO_Value.IN1_3Byte_0Time	=	gMainTimerCnt_10m;//chpark.07.05

				gMB_IO_Value.IN1_2Byte_4Time	=	gMainTimerCnt_10m;//chpark.07.05
				//gMB_IO_Value.IN1_4Byte_6Time	=	gMainTimerCnt_10m;//chpark.07.05

				gMB_IO_Value.IN1_2Byte_2Time	=	gMainTimerCnt_10m;//chpark.07.05

				gMB_IO_Value.IN1_4Byte_0Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.IN1_2Byte_3Time		=	gMainTimerCnt_10m;//chpark.07.05

				//gMB_IO_Value.IN1_1Byte_6Time		=	gMainTimerCnt_10m;//chpark.07.05

				//gMB_IO_Value.IN1_2Byte_7Time		=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.IN1_4Byte_5Time		=	gMainTimerCnt_10m;//chpark.24.10.18

				//gMB_IO_Value.IN1_0Byte_3Time		=	gMainTimerCnt_10m;//chpark.07.05
				//gMB_IO_Value.IN1_1Byte_0Time		=	gMainTimerCnt_10m;//chpark.07.05

				//gMB_IO_Value.IN1_0Byte_2Time		=	gMainTimerCnt_10m;//chpark.07.05
				//======

			}
			break;
			
			case IN2://chpark.09.13
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				//gMB_IO_Value.IN2_5Byte_4bit  =(sData[5]>>4)&0x01; 	
				//gMB_IO_Value.IN2_5Byte_5bit  =(sData[5]>>5)&0x01; 

				//gMB_IO_Value.IN2_3Byte_0bit  =(sData[3]>>0)&0x01; 
				//gMB_IO_Value.IN2_5Byte_2bit  =(sData[5]>>2)&0x01; 

				//gMB_IO_Value.IN2_5Byte_6bit  =(sData[5]>>6)&0x01; 
				//gMB_IO_Value.IN2_5Byte_7bit  =(sData[5]>>7)&0x01; 
				gMB_IO_Value.IN2_1Byte_2bit  =(sData[1]>>2)&0x01; //chpark.24.04.08
				gMB_IO_Value.IN2_2Byte_6bit = (sData[2]>>6)&0x01; 
				gMB_IO_Value.IN2_4Byte_6bit = (sData[4]>>6)&0x01; 
				gMB_IO_Value.IN2_4Byte_7bit  =(sData[4]>>7)&0x01; 
				
				//gMB_IO_Value.IN2_5Byte_4Time	=	gMainTimerCnt_10m;//chpark.07.05
				//gMB_IO_Value.IN2_5Byte_5Time	=	gMainTimerCnt_10m;//chpark.07.05

				//gMB_IO_Value.IN2_3Byte_0Time	=	gMainTimerCnt_10m;//chpark.07.05
				//gMB_IO_Value.IN2_5Byte_2Time	=	gMainTimerCnt_10m;//chpark.07.05

				//gMB_IO_Value.IN2_5Byte_6Time	=	gMainTimerCnt_10m;//chpark.07.05
				//gMB_IO_Value.IN2_5Byte_7Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.IN2_1Byte_2Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.IN2_2Byte_6Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.IN2_4Byte_6Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.IN2_4Byte_7Time	=	gMainTimerCnt_10m;//chpark.07.05
				//
			}
			break;

			case IN3:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[5]<<8) | sData[4])==0xFFFF)
					gMB_IO_Value.IN3_45Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.IN3_45Bytes  = (sData[5]<<8 | sData[4]);
					
				}

				if (((sData[7]<<8) | sData[6])==0xFFFF)
					gMB_IO_Value.IN3_67Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.IN3_67Bytes  = (sData[7]<<8 | sData[6]);
					
				}

				if (((sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.IN3_23Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.IN3_23Bytes  = (sData[3]<<8 | sData[2]);
					
				}
				//
				gMB_IO_Value.IN3_45Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.IN3_67Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.IN3_23Time	=	gMainTimerCnt_10m;//chpark.07.05
				//


			}
			break;

			case IN19:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				if (((sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.IN19_23Bytes = 0xFFFF;
				else
				{
					gMB_IO_Value.IN19_23Bytes = ( (sData[3]<<8) | sData[2]);
					
				}

				if (((sData[5]<<8) | sData[4])==0xFFFF)
					gMB_IO_Value.IN19_45Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.IN19_45Bytes  = (sData[5]<<8 | sData[4]);
					
				}

				if (((sData[7]<<8) | sData[6])==0xFFFF)
					gMB_IO_Value.IN19_67Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.IN19_67Bytes  = (sData[7]<<8 | sData[6]);
					
				}
				//
				gMB_IO_Value.IN19_23Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.IN19_45Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.IN19_67Time	=	gMainTimerCnt_10m;//chpark.07.05
				//

				
			
			
			}
			break;


			case IN25:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				//if (((sData[5]<<8) | sData[4])==0xFFFF)
				//	gMB_IO_Value.IN25_45Bytes  = 0xFFFF;
				//else
				//{
				//	gMB_IO_Value.IN25_45Bytes  = (sData[5]<<8 | sData[4]);
					
				//}

				//gMB_IO_Value.IN25_45Time	=	gMainTimerCnt_10m;//chpark.07.05


            
			}
			break;

			case IN26:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
			
				gMB_IO_Value.IN26_1Byte_0bit = (sData[1]>>0)&0x01;
				gMB_IO_Value.IN26_0Byte_4bit = (sData[0]>>4)&0x01;//chpark.09.13
				//
				gMB_IO_Value.IN26_1Byte_0Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.IN26_0Byte_4Time	=	gMainTimerCnt_10m;//chpark.09.13
				//
			
			}
			break;
			

			case IN28://chpark.09.13
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				gMB_IO_Value.IN28_0Byte_0bit = (sData[0]>>0)&0x01;
				gMB_IO_Value.IN28_0Byte_1bit = (sData[0]>>1)&0x01;
				gMB_IO_Value.IN28_0Byte_2bit = (sData[0]>>2)&0x01;
				gMB_IO_Value.IN28_1Byte_0bit = (sData[1]>>0)&0x01;
				gMB_IO_Value.IN28_1Byte_1bit = (sData[1]>>1)&0x01;
				gMB_IO_Value.IN28_1Byte_3bit = (sData[1]>>3)&0x01;
				gMB_IO_Value.IN28_23Bytes 	 = (sData[3]<<8 | sData[2]);
				gMB_IO_Value.IN28_45Bytes 	 = (sData[5]<<8 | sData[4]);
				//
				gMB_IO_Value.IN28_0Byte_0Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN28_0Byte_1Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN28_0Byte_2Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN28_1Byte_0Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN28_1Byte_1Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN28_1Byte_3Time	=	gMainTimerCnt_10m;
				//
				gMB_IO_Value.IN28_23Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN28_45Time	=	gMainTimerCnt_10m;
				
			}
			break;

			case IN29://chpark.09.13
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				gMB_IO_Value.IN29_0Byte_0bit = (sData[0]>>0)&0x01;
				gMB_IO_Value.IN29_0Byte_1bit = (sData[0]>>1)&0x01;
				gMB_IO_Value.IN29_0Byte_2bit = (sData[0]>>2)&0x01;
				gMB_IO_Value.IN29_0Byte_3bit = (sData[0]>>3)&0x01;
				gMB_IO_Value.IN29_0Byte_5bit = (sData[0]>>5)&0x01;

				gMB_IO_Value.IN29_12Bytes 	 = (sData[2]<<8 | sData[1]);
				gMB_IO_Value.IN29_34Bytes 	 = (sData[4]<<8 | sData[3]);
				gMB_IO_Value.IN29_56Bytes 	 = (sData[6]<<8 | sData[5]);
				//
				gMB_IO_Value.IN29_0Byte_0Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN29_0Byte_1Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN29_0Byte_2Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN29_0Byte_3Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN29_0Byte_5Time	=	gMainTimerCnt_10m;

				gMB_IO_Value.IN29_12Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN29_34Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN29_56Time	=	gMainTimerCnt_10m;
				//
			}
			break;
			
			case IN30://chpark.09.13
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				gMB_IO_Value.IN30_0Byte_1bit = (sData[0]>>1)&0x01;
				gMB_IO_Value.IN30_0Byte_2bit = (sData[0]>>2)&0x01;
				gMB_IO_Value.IN30_0Byte_3bit = (sData[0]>>3)&0x01;
				gMB_IO_Value.IN30_0Byte_5bit = (sData[0]>>5)&0x01;
				gMB_IO_Value.IN30_0Byte_6bit = (sData[0]>>6)&0x01;
				gMB_IO_Value.IN30_12Bytes 	 = (sData[2]<<8 | sData[1]);
				gMB_IO_Value.IN30_34Bytes 	 = (sData[4]<<8 | sData[3]);
				gMB_IO_Value.IN30_56Bytes 	 = (sData[6]<<8 | sData[5]);
				//
				gMB_IO_Value.IN30_0Byte_1Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN30_0Byte_2Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN30_0Byte_3Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN30_0Byte_5Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN30_0Byte_6Time	=	gMainTimerCnt_10m;

				gMB_IO_Value.IN30_12Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN30_34Time	=	gMainTimerCnt_10m;
				gMB_IO_Value.IN30_56Time 	=   gMainTimerCnt_10m;
				
			}
			break;

			case OUT1:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				

				gMB_IO_Value.OUT1_3Byte_3bit = (sData[3]>>3)&0x01;
				gMB_IO_Value.OUT1_0Byte_0bit = (sData[0]>>0)&0x01;

				gMB_IO_Value.OUT1_4Byte_7bit = (sData[4]>>7)&0x01;
				gMB_IO_Value.OUT1_2Byte_5bit = (sData[2]>>5)&0x01;

				gMB_IO_Value.OUT1_3Byte_2bit = (sData[3]>>2)&0x01;
				gMB_IO_Value.OUT1_2Byte_4bit = (sData[2]>>4)&0x01;

				gMB_IO_Value.OUT1_1Byte_4bit = (sData[1]>>4)&0x01;
				gMB_IO_Value.OUT1_2Byte_6bit = (sData[2]>>6)&0x01;
				
				gMB_IO_Value.OUT1_0Byte_5bit = (sData[0]>>5)&0x01;

				gMB_IO_Value.OUT1_2Byte_0bit = (sData[2]>>0)&0x01;

				gMB_IO_Value.OUT1_3Byte_4bit = (sData[3]>>4)&0x01;
				gMB_IO_Value.OUT1_5Byte_7bit = (sData[5]>>7)&0x01;

				gMB_IO_Value.OUT1_0Byte_3bit = (sData[0]>>3)&0x01;
				gMB_IO_Value.OUT1_0Byte_4bit = (sData[0]>>4)&0x01;

				gMB_IO_Value.OUT1_3Byte_7bit = (sData[3]>>7)&0x01;
				gMB_IO_Value.OUT1_1Byte_7bit = (sData[1]>>7)&0x01;
				
				//
				gMB_IO_Value.OUT1_3Byte_3Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT1_0Byte_0Time	=	gMainTimerCnt_10m;//chpark.07.05

				gMB_IO_Value.OUT1_4Byte_7Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT1_2Byte_5Time	=	gMainTimerCnt_10m;//chpark.07.05

				gMB_IO_Value.OUT1_3Byte_2Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT1_2Byte_4Time	=	gMainTimerCnt_10m;//chpark.07.05

				gMB_IO_Value.OUT1_1Byte_4Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT1_2Byte_6Time	=	gMainTimerCnt_10m;//chpark.07.05

				gMB_IO_Value.OUT1_0Byte_5Time	=	gMainTimerCnt_10m;//chpark.07.05

				gMB_IO_Value.OUT1_2Byte_0Time	=	gMainTimerCnt_10m;//chpark.07.05

				gMB_IO_Value.OUT1_3Byte_4Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT1_5Byte_7Time	=	gMainTimerCnt_10m;//chpark.07.05

				gMB_IO_Value.OUT1_0Byte_3Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT1_0Byte_4Time	=	gMainTimerCnt_10m;//chpark.07.05

				gMB_IO_Value.OUT1_3Byte_7Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT1_1Byte_7Time	=	gMainTimerCnt_10m;//chpark.07.05
				//
			
			}
			break;



			case OUT2:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.OUT2_23Bytes  = 0xFFFF;
				else if (sData[3]==0xFF)
					gMB_IO_Value.OUT2_23Bytes  = 0xFFFF;//chpark.10.19
				else
				{
					gMB_IO_Value.OUT2_23Bytes  = (sData[3]<<8 | sData[2]);
				}

				if (((sData[1]<<8) | sData[0])==0xFFFF)
					gMB_IO_Value.OUT2_01Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.OUT2_01Bytes  = (sData[1]<<8 | sData[0]);
					
				}
				//
				gMB_IO_Value.OUT2_23Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT2_01Time	=	gMainTimerCnt_10m;//chpark.07.05
				//


			}
			break;

			case OUT3:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[1]<<8) | sData[0])==0xFFFF)
					gMB_IO_Value.OUT3_01Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.OUT3_01Bytes  = (sData[1]<<8 | sData[0]);
					
				}
				//
				gMB_IO_Value.OUT3_01Time	=	gMainTimerCnt_10m;//chpark.07.05
			}
			break;

			case OUT4:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				//if (((sData[7]<<8) | sData[6])==0xFFFF)
				//	gMB_IO_Value.OUT4_67Bytes  = 0xFFFF;
				//else
				//{
			//		gMB_IO_Value.OUT4_67Bytes  = (sData[7]<<8 | sData[6]);
					
				//}
				//
				//gMB_IO_Value.OUT4_67Time	=	gMainTimerCnt_10m;//chpark.07.05

			}
			break;

			
			case OUT6:
			{
//				printf("[UART]OUT6 Receive\n");
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				
				gMB_IO_Value.OUT6_0Byte_3bit = (sData[0]>>3)&0x01;
				gMB_IO_Value.OUT6_0Byte_0bit = (sData[0]>>0)&0x01;

				gMB_IO_Value.OUT6_0Byte_4bit = (sData[0]>>4)&0x01;
				gMB_IO_Value.OUT6_0Byte_6bit = (sData[0]>>6)&0x01;
				gMB_IO_Value.OUT6_5Byte_3bit = (sData[5]>>3)&0x01;
				gMB_IO_Value.OUT6_5Byte_6bit = (sData[5]>>6)&0x01;//chpark.24.10.18
				gMB_IO_Value.OUT6_5Byte_7bit = (sData[5]>>7)&0x01;//chpark.24.10.18
				
				gMB_IO_Value.OUT6_7Byte_3bit = (sData[7]>>3)&0x01;
				gMB_IO_Value.OUT6_7Byte_4bit = (sData[7]>>4)&0x01;//chpark.24.10.18
				//

				gMB_IO_Value.OUT6_0Byte_3Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT6_0Byte_0Time	=	gMainTimerCnt_10m;//chpark.07.05

				gMB_IO_Value.OUT6_0Byte_4Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT6_0Byte_6Time	=	gMainTimerCnt_10m;//chpark.07.05

				gMB_IO_Value.OUT6_5Byte_3Time	=	gMainTimerCnt_10m;//chpark.24.01.25
				gMB_IO_Value.OUT6_5Byte_6Time 	=	gMainTimerCnt_10m;//chpark.24.10.18
				gMB_IO_Value.OUT6_5Byte_7Time 	=	gMainTimerCnt_10m;//chpark.24.10.18

				gMB_IO_Value.OUT6_7Byte_3Time 	= 	gMainTimerCnt_10m;
				
				gMB_IO_Value.OUT6_7Byte_4Time   =   gMainTimerCnt_10m;//chpark.24.10.18
			}
			break;

			case OUT10:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[1]<<8) | sData[0])==0xFFFF)
					gMB_IO_Value.OUT10_01Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.OUT10_01Bytes   = (sData[1]<<8 | sData[0]);
					
				}

				if (((sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.OUT10_23Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.OUT10_23Bytes  = (sData[3]<<8 | sData[2]);
					
				}

				if (((sData[5]<<8) | sData[4])==0xFFFF)
					gMB_IO_Value.OUT10_45Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.OUT10_45Bytes  = (sData[5]<<8 | sData[4]);
					
				}

				if (((sData[7]<<8) | sData[6])==0xFFFF)
					gMB_IO_Value.OUT10_67Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.OUT10_67Bytes  = (sData[7]<<8 | sData[6]);
					
				}

				//
				gMB_IO_Value.OUT10_01Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT10_23Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT10_45Time	=	gMainTimerCnt_10m;//chpark.09.13
				gMB_IO_Value.OUT10_67Time	=	gMainTimerCnt_10m;//chpark.09.13
				
			}
			break;


			case OUT13:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[1]<<8) | sData[0])==0xFFFF)
					gMB_IO_Value.OUT13_01Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.OUT13_01Bytes  = (sData[1]<<8 | sData[0]);
					
				}

				if (((sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.OUT13_23Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.OUT13_23Bytes  = (sData[3]<<8 | sData[2]);
					
				}

				if (((sData[5]<<8) | sData[4])==0xFFFF)
					gMB_IO_Value.OUT13_45Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.OUT13_45Bytes  = (sData[5]<<8 | sData[4]);
					
				}

				if (((sData[7]<<8) | sData[6])==0xFFFF)
					gMB_IO_Value.OUT13_67Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.OUT13_67Bytes  = (sData[7]<<8 | sData[6]);
					
				}

				//
				gMB_IO_Value.OUT13_01Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT13_23Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT13_45Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT13_67Time	=	gMainTimerCnt_10m;//chpark.07.05
				//

			}
			break;


			case OUT14:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[1]<<8) | sData[0])==0xFFFF)
					gMB_IO_Value.OUT14_01Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.OUT14_01Bytes  = (sData[1]<<8 | sData[0]);
					
				}

				if (((sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.OUT14_23Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.OUT14_23Bytes  = (sData[3]<<8 | sData[2]);
					
				}
				//
				gMB_IO_Value.OUT14_01Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.OUT14_23Time	=	gMainTimerCnt_10m;//chpark.07.05

			}
			break;

			//chpark.24.10.18
			case OUT20:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[1]<<8) | sData[0])==0xFFFF)
					gMB_IO_Value.OUT20_01Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.OUT20_01Bytes  = (sData[1]<<8 | sData[0]);
					
				}

				if (((sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.OUT20_23Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.OUT20_23Bytes  = (sData[3]<<8 | sData[2]);
					
				}
				//
				gMB_IO_Value.OUT20_01Time	=	gMainTimerCnt_10m;//chpark.24.10.10
				gMB_IO_Value.OUT20_23Time	=	gMainTimerCnt_10m;//chpark.24.10.10

			}
			break;


			case AUX_PIO:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				if (((sData[1]<<8) | sData[0])==0xFFFF)
					gMB_IO_Value.AUX_PIO_01Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.AUX_PIO_01Bytes  = (sData[1]<<8 | sData[0]);
					
				}

				if (((sData[3]<<8) | sData[2])==0xFFFF)
					gMB_IO_Value.AUX_PIO_23Bytes  = 0xFFFF;
				else
				{
					gMB_IO_Value.AUX_PIO_23Bytes  = (sData[3]<<8 | sData[2]);
					
				}
				//
				gMB_IO_Value.AUX_PIO_01Time	=	gMainTimerCnt_10m;//chpark.07.05
				gMB_IO_Value.AUX_PIO_23Time	=	gMainTimerCnt_10m;//chpark.07.05
				//

			}
			break;
			
			//==================================================================
			//test mode
			case XO_TMODE_REQ:
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);

				g_TestMode_Req_Time = gMainTimerCnt_10m;//chpark.24.05.27				
			}
			break;
			//==================================================================
			case IMM_A: //chpark.24.10.30  18502E17
			{
				static uint32_t IMM_A_10m=0x0;
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint8_t IMM_A_0Byte_12bit;   
				uint8_t IMM_A_1Byte_01bit;
				IMM_A_0Byte_12bit = (sData[0]>>1)&0x03;
				IMM_A_1Byte_01bit = (sData[1]>>0)&0x03;
				if (Main_Disp_Mode != MAIN_MODE_ENGN_COOL)		// JPHong 2024-10-30
				{
					if(IMM_A_0Byte_12bit==1 && IMM_A_1Byte_01bit==0)
					{
						if (IMM_A_10m==0)
	                  	{
	                     	IMM_A_10m=gMainTimerCnt_10m;
	                  	}
	                  	else if ((IMM_A_10m+TIME_CNT_1SEC)<gMainTimerCnt_10m)
	                  	{
							printf("[IMM_A]IMM_A_10m=%d IMM_A_3Byte_0bit=%d GotoBSImmobilizer.\n",IMM_A_10m,IMM_A_1Byte_01bit);
							g_IM_Main_Step = 0;
							GotoBSImmobilizer();
							IMM_A_10m=0;
	                  	}
					}
					else
					{
						IMM_A_10m=0;
						printf("[IMM_A]IMM_A_0Byte_12bit=%d IMM_A_3Byte_0bit=%d\n",IMM_A_0Byte_12bit,IMM_A_1Byte_01bit);
					}
				}
				else		// JPHong 2024-10-30
					IMM_A_10m=0x0;
			}	
			break;
			case IMM_R: // 18EACEE4  chpark.24.08.29
			{
				uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
				uint8_t IMM_R_4Byte_03bit;
				if (MICOM_RX.RxData[MICOM_RX.Rx_Read][5]>3)
				{
					IMM_R_4Byte_03bit = (sData[4]>>0)&0x0F;
					g_IM_RecvTime = gMainTimerCnt_10m;//chpark.24.05.27
					if(g_IM_b_ExistLayer)
					{
						printf("[IMM_R]IM Exist. start IMM Layer(mode=%d) g_IM_b_ExistLayer=%d IMM_R_4Byte_03bit=%d",g_IM_Main_Step,g_IM_b_ExistLayer,IMM_R_4Byte_03bit);
						//g_IM_Main_Step = IMM_R_4Byte_03bit;
						//GotoBSImmobilizer();
						MenuIM_RcvData(IMM_R_4Byte_03bit);
					}
					else
					{
						printf("[IMM_R]Error. == IM Not Exist. start IMM Layer(mode=%d) g_IM_b_ExistLayer=%d IMM_R_4Byte_03bit=%d",g_IM_Main_Step,g_IM_b_ExistLayer,IMM_R_4Byte_03bit);
						g_IM_Main_Step  = IMM_R_4Byte_03bit+100;// = 10;//IMM_R_4Byte_03bit;
						GotoBSImmobilizer();
						//MenuIM_RcvData(IMM_R_4Byte_03bit);
					}
				}
			}	
			break;
			//==================================================================
			default:
				if ((CAN_ID&0xFFFFFF00)==SOFT_REQ)
				{
					uint8_t *sData = &(MICOM_RX.RxData[MICOM_RX.Rx_Read][6]);
					uint8_t i;
					
					if ((sData[0]==0xDA)&&(sData[1]==0xFE))
					{
						SendSoftID();
					}
				}
			break;			
		}
		MICOM_RX.RxFlag[MICOM_RX.Rx_Read]=0;
		MICOM_RX.Rx_Read++;
		if (MICOM_RX.Rx_Read >= MICOM_RX_DATA_CNT)
		{
			MICOM_RX.Rx_Read = 0;
		}		
	}
	return 0;
}




#ifdef DEMO_MODE_PLAY

	#define	DM_SI_DATA	120
	#define	DM_EI_DATA	240

	uint16_t dm_EEC1_34Byte=0;
	uint8_t dm_EEC2_2Byte=0;
	
	uint8_t dm_SI_1Byte=0;
	uint16_t dm_SI_67Byte=0;
	
	uint8_t dm_EI_0Byte=0;
	uint8_t dm_EI_2Byte=0;
	uint16_t dm_EI_34Byte=0;
	uint8_t dm_EI_5Byte=0;
	uint8_t dm_EI_6Byte=0;
	uint8_t dm_EI_7Byte=0;
	
	uint8_t dm_DD_1Byte=0;
	bool dm_DD_1_Flag=0;
	
	uint32_t dm_HM_0123Byte=0;
	uint32_t dm_TM1_0123Byte=0;
	uint32_t dm_TM1_4567Byte=0;
	uint32_t dm_TM2_0123Byte=0;
	
	bool dm_SI_67_Flag=0;
	bool dm_EI_34_Flag=0;

	void SaveCanData(uint32_t IDenti, uint8_t *TxData)
	{
		memcpy(&(MICOM_RX.RxData[MICOM_RX.Rx_Write][6]), &(TxData[0]), MICOM_DATA_LENTH);
		MICOM_RX.RxData[MICOM_RX.Rx_Write][1] = (IDenti>>24)&0xFF;
		MICOM_RX.RxData[MICOM_RX.Rx_Write][2] = (IDenti>>16)&0xFF;
		MICOM_RX.RxData[MICOM_RX.Rx_Write][3] = (IDenti>> 8)&0xFF;
		MICOM_RX.RxData[MICOM_RX.Rx_Write][4] = (IDenti>> 0)&0xFF;
//		if (EI == IDenti)
//			printf("[EI][%d][%d]\n",MICOM_RX.Rx_Write,(MICOM_RX.RxData[MICOM_RX.Rx_Write][10]<<8) |( MICOM_RX.RxData[MICOM_RX.Rx_Write][9]));
		MICOM_RX.RxFlag[MICOM_RX.Rx_Write] = 1;
		MICOM_RX.Rx_Write++;
		if (MICOM_RX.Rx_Write >= MICOM_RX_DATA_CNT)
		{
			MICOM_RX.Rx_Write = 0;
		}
	}

	#define MAX_ICON_F_CNT	17
	uint8_t MaxIcon_F_Cnt = 0;
	uint8_t IconF_Data[MAX_ICON_F_CNT][12] = {
		{0x14,0xFF,0x00,0xE4,    0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00},		// CLM1 	0x14FF00E4
		{0x14,0xFF,0x00,0xE4,    0x00,0x00,0x00,0x00,0x00,0x00,0x09,0x00},		// CLM1 	0x14FF00E4
		{0x14,0x50,0x0E,0xE4,    0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00},		// CLM2 	0x14500EE4
		{0x14,0xFF,0x00,0xE4,    0x02,0x00,0x00,0x00,0x00,0x00,0x09,0x00},		// CLM1 	0x14FF00E4
		{0x14,0xFF,0x00,0xE4,    0x0A,0x00,0x00,0x00,0x00,0x00,0x09,0x00},		// CLM1 	0x14FF00E4
		{0x14,0x50,0x0E,0xE4,    0x00,0x00,0x80,0x00,0x00,0x00,0x04,0x00},		// CLM2 	0x14500EE4
		{0x14,0xFF,0x00,0xE4,    0x0A,0x40,0x00,0x00,0x00,0x00,0x09,0x00},		// CLM1 	0x14FF00E4
		{0x14,0xFF,0x00,0xE4,    0x0A,0x41,0x00,0x00,0x00,0x00,0x09,0x00},		// CLM1 	0x14FF00E4
		{0x14,0x50,0x0E,0xE4,    0x00,0x00,0x80,0x00,0x00,0x00,0x04,0x00},		// CLM2 	0x14500EE4
		{0x14,0xFF,0x00,0xE4,    0x0A,0x41,0x10,0x00,0x00,0x00,0x09,0x00},		// CLM1 	0x14FF00E4
		{0x14,0xFF,0x00,0xE4,    0x0A,0x41,0x12,0x00,0x00,0x00,0x09,0x00},		// CLM1 	0x14FF00E4
		{0x14,0x50,0x0E,0xE4,    0x00,0x00,0x80,0x00,0x00,0x00,0x44,0x00},		// CLM2 	0x14500EE4
		{0x14,0xFF,0x00,0xE4,    0x0A,0x49,0x12,0x00,0x00,0x00,0x09,0x00},		// CLM1 	0x14FF00E4
		{0x14,0xFF,0x00,0xE4,    0x0A,0x49,0x12,0x00,0x00,0x00,0x09,0x00},		// CLM1 	0x14FF00E4
		{0x14,0x50,0x0E,0xE4,    0x00,0x00,0x80,0x00,0x00,0x00,0x44,0x00},		// CLM2 	0x14500EE4
		{0x14,0xFF,0x00,0xE4,    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},		// CLM1 	0x14FF00E4
		{0x14,0x50,0x0E,0xE4,    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}		// CLM2 	0x14500EE4
	};

	#define MAX_ICON_W_CNT	18
	uint8_t MaxIcon_W_Cnt = 0;
	uint8_t IconW_Data[MAX_ICON_W_CNT][12] = {
		{0x14,0x50,0x0C,0xE4,    0x00,0xC0,0x00,0x00,0x00,0x00,0x00,0x00},		// CLM3 	0x14500CE4
		{0x14,0x50,0x0A,0xE4,    0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00},		// CLM4 	0x14500AE4
		{0x14,0x50,0x0C,0xE4,    0x00,0xC0,0x03,0x00,0x00,0x0C,0x00,0x00},		// CLM3 	0x14500CE4
		{0x14,0x50,0x0C,0xE4,    0x03,0xC0,0x03,0x00,0x00,0x0C,0x00,0x00},		// CLM3 	0x14500CE4
		{0x14,0x50,0x0C,0xE4,    0x33,0xC0,0x03,0x00,0x00,0x0C,0x00,0x00},		// CLM3 	0x14500CE4
		{0x14,0x50,0x0C,0xE4,    0xF3,0xC0,0x03,0x00,0x00,0x0C,0x00,0x00},		// CLM3 	0x14500CE4
		{0x14,0x50,0x0A,0xE4,    0x0C,0x03,0x00,0x00,0x00,0x00,0x00,0x00},		// CLM4 	0x14500AE4
		{0x14,0x50,0x0C,0xE4,    0xFF,0xC0,0x03,0x00,0x00,0x0C,0x00,0x00},		// CLM3 	0x14500CE4
		{0x14,0x50,0x0C,0xE4,    0xFF,0xC3,0x03,0x00,0x00,0x0C,0x00,0x00},		// CLM3 	0x14500CE4
		{0x14,0x50,0x0C,0xE4,    0xFF,0xC3,0x03,0x00,0x00,0x3C,0x00,0x00},		// CLM3 	0x14500CE4
		{0x14,0x50,0x0C,0xE4,    0xFF,0xC3,0x0F,0x00,0x00,0x3C,0x00,0x00},		// CLM3 	0x14500CE4
		{0x14,0x50,0x0C,0xE4,    0xFF,0xC3,0x0F,0x00,0x0C,0x3C,0x00,0x00},		// CLM3 	0x14500CE4
		{0x14,0x50,0x0A,0xE4,    0xCC,0x03,0x00,0x00,0x00,0x00,0x00,0x00},		// CLM4 	0x14500AE4
		{0x14,0x50,0x0C,0xE4,    0xFF,0xC3,0x0F,0x00,0x0C,0xFC,0x00,0x00},		// CLM3 	0x14500CE4
		{0x14,0x50,0x0C,0xE4,    0xFF,0xC3,0x0F,0x00,0x3C,0xFC,0x00,0x00},		// CLM3 	0x14500CE4
		{0x14,0x50,0x0C,0xE4,    0xFF,0xC3,0x0F,0x00,0x3C,0xFC,0x00,0x00},		// CLM3 	0x14500CE4
		{0x14,0x50,0x0A,0xE4,    0xCC,0x03,0x00,0x00,0x00,0x00,0x00,0x00},		// CLM4 	0x14500AE4
		{0x14,0x50,0x0A,0xE4,    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},		// CLM4 	0x14500AE4
		{0x14,0x50,0x0C,0xE4,    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}		// CLM3 	0x14500CE4
	};

	void CAN_Data_Save(void)
	{
	
	
		uint8_t  txbuffer[8] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

		if ((gMainTimerCnt_10m%(TIME_CNT_5SEC+1))==0)
		{
			switch(MRV.Symbol)
			{
				case m14_MeterSymbol_EngLd:
				{
					MRV.Symbol = m13_MeterSymbol_EngRPM;
				}
				break;
				case m13_MeterSymbol_EngRPM:
				{
					MRV.Symbol = m12_MeterSymbol_LiftLd;
				}
				break;
				case m12_MeterSymbol_LiftLd:
				{
					MRV.Symbol = m11_MeterSymbol_PM;
				}
				break;
				case m11_MeterSymbol_PM:
				{
					MRV.Symbol = m11_MeterSymbol_AHS;
				}
				break;
				case m11_MeterSymbol_AHS:
				{
					MRV.Symbol = m14_MeterSymbol_EngLd;
				}
				break;
			}	
		}
		if ((gMainTimerCnt_10m%(TIME_CNT_7SEC-1))==0)
		{
			#ifdef TEST_FLASH_MEM
			#else
				HMV.HourMeter_Trip++;
				if (HMV.HourMeter_Trip>=4)
					HMV.HourMeter_Trip = 0;
			#endif
		}
		if ((gMainTimerCnt_10m%(TIME_CNT_20SEC-3))==0)
		{
			DTM_DateTime_Mode++;
			if (DTM_DateTime_Mode>=3)
				DTM_DateTime_Mode = 0;	
		}
		if ((gMainTimerCnt_10m%(TIME_CNT_30SEC-4))==0)
		{
			BG_Color++;
			if (BG_Color>=(BG_BLACK+1))
				BG_Color = BG_BLACK;	
		}

		if ((gMainTimerCnt_10m%(TIME_CNT_1_5SEC+1))==0)
		{
			uint32_t tId=0;
			if (MaxIcon_F_Cnt>MAX_ICON_F_CNT)
				MaxIcon_F_Cnt = 0;
			tId = (IconF_Data[MaxIcon_F_Cnt][0]<<24)|(IconF_Data[MaxIcon_F_Cnt][1]<<16)|(IconF_Data[MaxIcon_F_Cnt][2]<<8)|(IconF_Data[MaxIcon_F_Cnt][3]<<0);
			memset (txbuffer,0,8);
			memcpy (txbuffer,&IconF_Data[MaxIcon_F_Cnt][4],8);
			SaveCanData(tId,txbuffer);
			MaxIcon_F_Cnt++;
		}
		if ((gMainTimerCnt_10m%(TIME_CNT_7SEC-3))==0)
		{
			uint32_t tId=0;
			if (MaxIcon_W_Cnt>MAX_ICON_W_CNT)
				MaxIcon_W_Cnt = 0;
			tId = (IconW_Data[MaxIcon_W_Cnt][0]<<24)|(IconW_Data[MaxIcon_W_Cnt][1]<<16)|(IconW_Data[MaxIcon_W_Cnt][2]<<8)|(IconW_Data[MaxIcon_W_Cnt][3]<<0);
			memset (txbuffer,0,8);
			memcpy (txbuffer,&IconW_Data[MaxIcon_W_Cnt][4],8);
			SaveCanData(tId,txbuffer);
			MaxIcon_W_Cnt++;
		}	
		

		if ((gMainTimerCnt_10m%(TIME_CNT_200MS-1))==0)
		{
			if (dm_SI_67_Flag==0)
			{
				dm_SI_67Byte+=DM_SI_DATA;	// 0~12000
				if (dm_SI_67Byte>13000)
				{
					dm_SI_67Byte = 12000;
					dm_SI_67_Flag = 1;
				}
			}
			else
			{
				if (dm_SI_67Byte<=DM_SI_DATA)
				{
					dm_SI_67Byte = 0;
					dm_SI_67_Flag = 0;
				}
				else
					dm_SI_67Byte-=DM_SI_DATA;	// 0~12000
			}
			dm_SI_1Byte+=8;

			if (dm_SI_1Byte>120)
				dm_SI_1Byte=0;			
			memset (txbuffer,0,8);
			txbuffer[1]=dm_SI_1Byte;
			txbuffer[6]=(dm_SI_67Byte&0xFF);
			txbuffer[7]=((dm_SI_67Byte>>8)&0xFF);
			SaveCanData(SI,txbuffer);			
		}

		if ((gMainTimerCnt_10m%(TIME_CNT_100MS+1))==0)
		{	
			if (dm_EI_34_Flag==0)
			{
				dm_EI_34Byte+=DM_EI_DATA;	// 0~12000
				if (dm_EI_34Byte>14000)
				{
					dm_EI_34Byte = 12000;
					dm_EI_34_Flag = 1;
				}
			}
			else
			{
				if (dm_EI_34Byte<=DM_EI_DATA)
				{
					dm_EI_34Byte = 0;
					dm_EI_34_Flag = 0;
				}
				else
					dm_EI_34Byte-=DM_EI_DATA;	// 0~12000
			}
			dm_EI_0Byte+=5;
			dm_EI_2Byte+=10;
			dm_EI_5Byte+=15;
			dm_EI_6Byte+=20;
			dm_EI_7Byte+=25;

			if (dm_EI_0Byte>120)
				dm_EI_0Byte=0;
			if (dm_EI_2Byte>120)
				dm_EI_2Byte=0;
			if (dm_EI_5Byte>120)
				dm_EI_5Byte=0;
			if (dm_EI_6Byte>120)
				dm_EI_6Byte=0;
			if (dm_EI_7Byte>120)
				dm_EI_7Byte=0;
				
							
			memset (txbuffer,0,8);
			txbuffer[0]=dm_EI_0Byte;
			txbuffer[2]=dm_EI_2Byte;
			txbuffer[3]=(dm_EI_34Byte&0xFF);
			txbuffer[4]=((dm_EI_34Byte>>8)&0xFF);
			txbuffer[5]=dm_EI_5Byte;
			txbuffer[6]=dm_EI_6Byte;
			txbuffer[7]=dm_EI_7Byte;
			SaveCanData(EI,txbuffer);			
		}
	
		if ((gMainTimerCnt_10m%(TIME_CNT_300MS-2))==0)
		{
			dm_EEC2_2Byte+=5;
			if (dm_DD_1_Flag==0)
			{
				if (dm_DD_1Byte>230)
				{
					dm_DD_1Byte = 255;
					dm_DD_1_Flag = 1;
				}
				else
					dm_DD_1Byte+=25;
			}
			else
			{
				if (dm_DD_1Byte<=25)
				{
					dm_DD_1Byte = 0;
					dm_DD_1_Flag = 0;
				}
				else
					dm_DD_1Byte-=25;
			}
		
			memset (txbuffer,0,8);
			txbuffer[2]=dm_EEC2_2Byte;
			SaveCanData(EEC2,txbuffer);
	
			memset (txbuffer,0,8);
			txbuffer[1]=dm_DD_1Byte;
			SaveCanData(DD,txbuffer);
		}

		if ((gMainTimerCnt_10m%(TIME_CNT_400MS-3))==0)
		{
			dm_HM_0123Byte+=111;
			dm_TM1_0123Byte+=211;
			dm_TM1_4567Byte+=311;
			dm_TM2_0123Byte+=411;

			memset (txbuffer,0,8);
			txbuffer[0]=((dm_HM_0123Byte&0xFF));
			txbuffer[1]=((dm_HM_0123Byte>>8)&0xFF);
			txbuffer[2]=((dm_HM_0123Byte>>16)&0xFF);
			txbuffer[3]=((dm_HM_0123Byte>>24)&0xFF);
			SaveCanData(HM,txbuffer);

			memset (txbuffer,0,8);
			txbuffer[0]=((dm_TM1_0123Byte&0xFF));
			txbuffer[1]=((dm_TM1_0123Byte>>8)&0xFF);
			txbuffer[2]=((dm_TM1_0123Byte>>16)&0xFF);
			txbuffer[3]=((dm_TM1_0123Byte>>24)&0xFF);
			txbuffer[4]=((dm_TM1_4567Byte&0xFF));
			txbuffer[5]=((dm_TM1_4567Byte>>8)&0xFF);
			txbuffer[6]=((dm_TM1_4567Byte>>16)&0xFF);
			txbuffer[7]=((dm_TM1_4567Byte>>24)&0xFF);
			SaveCanData(TM1,txbuffer);

			memset (txbuffer,0,8);
			txbuffer[0]=((dm_TM2_0123Byte&0xFF));
			txbuffer[1]=((dm_TM2_0123Byte>>8)&0xFF);
			txbuffer[2]=((dm_TM2_0123Byte>>16)&0xFF);
			txbuffer[3]=((dm_TM2_0123Byte>>24)&0xFF);
			SaveCanData(TM2,txbuffer);

			if (dm_HM_0123Byte>999999)
				dm_HM_0123Byte=0;
			if (dm_TM1_0123Byte>999999)
				dm_TM1_0123Byte=0;
			if (dm_TM1_4567Byte>999999)
				dm_TM1_4567Byte=0;
			if (dm_TM2_0123Byte>999999)
				dm_TM2_0123Byte=0;
		}

		if ((gMainTimerCnt_10m%(TIME_CNT_500MS-4))==0)
		{		
			dm_EEC1_34Byte += 12000;
			memset (txbuffer,0,8);
			txbuffer[3]=(dm_EEC1_34Byte&0xFF);
			txbuffer[4]=((dm_EEC1_34Byte>>8)&0xFF);
			SaveCanData(EEC1,txbuffer);
		}

	}
#endif	//DEMO_MODE_PLAY


