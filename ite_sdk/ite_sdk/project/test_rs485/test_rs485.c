/*
 * Copyright (c) 2015 ITE Tech. Inc. All Rights Reserved.
 *
 * @file (%project)/project/test_rs485/test_rs485.c
 *
 * @author Benson Lin
 * @version 1.0.0
 *
 * example code for how to using rs485.
 */
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "ite/itp.h"	//for all ith driver

#define RS485CommandLen 4

/*
UART0: FIFO 32 Bytes , It means if we choice UART0 , we can transter/recevier 32 bytes once a time.
UART1: FIFO 16 Bytes , It means if we choice UART1 , we can transter/recevier 16 bytes once a time.
UART2: FIFO   8 Bytes,  It means if we choice UART2 , we can transter/recevier  8 bytes once a time.
UART3: FIFO   8 Bytes,  It means if we choice UART3 , we can transter/recevier  8 bytes once a time.
*/

#ifdef CFG_DBG_UART0
#undef CFG_RS485_0_INTR
#undef CFG_RS485_0_DMA
#undef CFG_RS485_0_FIFO
#endif
#ifdef CFG_DBG_UART1
#undef CFG_RS485_1_INTR
#undef CFG_RS485_1_DMA
#undef CFG_RS485_1_FIFO
#endif

#if defined(CFG_RS485_0_INTR) || defined(CFG_RS485_1_INTR)\
	|| defined(CFG_RS485_2_INTR) || defined(CFG_RS485_3_INTR)\
	|| defined(CFG_RS485_4_INTR) || defined(CFG_RS485_5_INTR)
#define TEST_INTR_MODE
#endif

#if defined(CFG_RS485_0_DMA) || defined(CFG_RS485_1_DMA)\
	|| defined(CFG_RS485_2_DMA) || defined(CFG_RS485_3_DMA)\
	|| defined(CFG_RS485_4_DMA) || defined(CFG_RS485_5_DMA)
#define TEST_DMA_MODE
#endif

#if defined(CFG_RS485_0_FIFO) || defined(CFG_RS485_1_FIFO)\
	|| defined(CFG_RS485_2_FIFO) || defined(CFG_RS485_3_FIFO)\
	|| defined(CFG_RS485_4_FIFO) || defined(CFG_RS485_5_FIFO)
#define TEST_FIFO_MODE
#endif

#if defined(CFG_RS485_0_ENABLE)
#define TEST_PORT       ITP_DEVICE_RS485_0
#define TEST_DEVICE     itpDeviceRS485_0
#define TEST_BAUDRATE   CFG_RS485_0_BAUDRATE
#define TEST_GPIO_RX    CFG_GPIO_RS485_0_RX
#elif defined(CFG_RS485_1_ENABLE)
#define TEST_PORT       ITP_DEVICE_RS485_1
#define TEST_DEVICE     itpDeviceRS485_1
#define TEST_BAUDRATE   CFG_RS485_1_BAUDRATE
#define TEST_GPIO_RX    CFG_GPIO_RS485_1_RX
#elif defined(CFG_RS485_2_ENABLE)
#define TEST_PORT       ITP_DEVICE_RS485_2
#define TEST_DEVICE     itpDeviceRS485_2
#define TEST_BAUDRATE   CFG_RS485_2_BAUDRATE
#define TEST_GPIO_RX    CFG_GPIO_RS485_2_RX
#elif defined(CFG_RS485_3_ENABLE)
#define TEST_PORT       ITP_DEVICE_RS485_3
#define TEST_DEVICE     itpDeviceRS485_3
#define TEST_BAUDRATE   CFG_RS485_3_BAUDRATE
#define TEST_GPIO_RX    CFG_GPIO_RS485_3_RX
#elif defined(CFG_RS485_4_ENABLE)
#define TEST_PORT       ITP_DEVICE_RS485_4
#define TEST_DEVICE     itpDeviceRS485_4
#define TEST_BAUDRATE   CFG_RS485_4_BAUDRATE
#define TEST_GPIO_RX    CFG_GPIO_RS485_4_RX
#elif defined(CFG_RS485_5_ENABLE)
#define TEST_PORT       ITP_DEVICE_RS485_5
#define TEST_DEVICE     itpDeviceRS485_5
#define TEST_BAUDRATE   CFG_RS485_5_BAUDRATE
#define TEST_GPIO_RX    CFG_GPIO_RS485_5_RX
#endif

#if defined(TEST_INTR_MODE)
static sem_t RS485SemIntr;
#elif defined(TEST_DMA_MODE)
static sem_t RS485SemDma;
#elif defined(TEST_FIFO_MODE)
static sem_t RS485SemFifo;
#endif

#if defined(TEST_INTR_MODE)
static void RS485Callback(void* arg1, uint32_t arg2)
{
	uint8_t getstr[256];
	uint8_t sendstr1[8] = {0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
	int len = 0;
	static int totalcount =0;

#if 1  // Turn on  ITP_IOCTL_REG_RS485_CB  flag , should using these codes.
	sem_post(&RS485SemIntr);
#else  // Turn on ITP_IOCTL_REG_RS485_DEFER_CB flag , should using these codes.
	
	len = read(TEST_PORT,getstr+totalcount,256);
	totalcount += len;
	
	if(totalcount >= RS485CommandLen)
	{	  
		write(TEST_PORT,sendstr1,8);
		totalcount =0;
		memset(getstr, 0, 256);
	}
#endif
}
#endif
#if defined(TEST_DMA_MODE) || defined(TEST_FIFO_MODE)
static void gpio_isr(unsigned int pin, void *arg)
{
#if defined(TEST_DMA_MODE)
	sem_post(&RS485SemDma);
#elif defined(TEST_FIFO_MODE)
	sem_post(&RS485SemFifo);
#endif
}

void InitRS485Intr(ITHUartPort port)
{
	ithEnterCritical();

	ithGpioClearIntr(TEST_GPIO_RX);
	ithGpioSetIn(TEST_GPIO_RX);
	ithGpioRegisterIntrHandler(TEST_GPIO_RX, gpio_isr, NULL);
	
	ithGpioCtrlDisable(TEST_GPIO_RX, ITH_GPIO_INTR_LEVELTRIGGER);   /* use edge trigger mode */
	ithGpioCtrlEnable(TEST_GPIO_RX, ITH_GPIO_INTR_BOTHEDGE); /* both edge */
	ithIntrEnableIrq(ITH_INTR_GPIO);
	ithGpioEnableIntr(TEST_GPIO_RX);

	ithExitCritical();
}
#endif

#if defined(TEST_INTR_MODE)
void* TestFuncUseINTR(void* arg)
{
	char getstr[256] = "", sendstr[256] = "";
	int len = 0, count =0;

   	printf("Start RS485 Interrupt mode test !\n");

	itpRegisterDevice(TEST_PORT, &TEST_DEVICE);
	ioctl(TEST_PORT, ITP_IOCTL_RESET, (void*)TEST_BAUDRATE);
	ioctl(TEST_PORT, ITP_IOCTL_REG_RS485_CB, (void*)RS485Callback);
	//ioctl(TEST_PORT, ITP_IOCTL_REG_RS485_DEFER_CB , (void*)RS485Callback);

	sem_init(&RS485SemIntr, 0, 0);

	while(1)
	{	
		sem_wait(&RS485SemIntr);

		len = read(TEST_PORT, getstr + count, 256);
		printf("len = %d, getstr = %s\n", len, getstr);
		count += len;
	
		if(count >= RS485CommandLen)
		{
			printf("rs485 read: %s,count=%d\n", getstr, count);
			memcpy(sendstr, getstr, count);
			sendstr[count] = '\n';
			write(TEST_PORT, sendstr, count + 1);

			memset(getstr, 0, count + 1);
			memset(sendstr, 0, count + 1);
			count = 0;
		}
	}
	return NULL;
}
#elif defined(TEST_DMA_MODE)
void* TestFuncUseDMA(void* arg)
{
	int len = 0, count = 0;
	char getstr[256] = "", sendstr[256] = "";

	itpRegisterDevice(TEST_PORT, &TEST_DEVICE);
	ioctl(TEST_PORT, ITP_IOCTL_RESET, (void*)TEST_BAUDRATE);

	InitRS485Intr(TEST_PORT);

	printf("Start RS485 DMA mode test!\n");

	sem_init(&RS485SemDma, 0, 0);

	while (1)
	{
		sem_wait(&RS485SemDma);
		len = read(TEST_PORT, getstr + count, 256);
		printf("len = %d, getstr = %s\n", len, getstr);
		count += len;

		if (count >= RS485CommandLen)
		{
			printf("uart read: %s,count=%d\n", getstr, count);
			memcpy(sendstr, getstr, count);
			sendstr[count] = '\n';
			write(TEST_PORT, sendstr, count + 1);

			memset(getstr, 0, count + 1);
			memset(sendstr, 0, count + 1);
			count = 0;
		}
	}
}
#elif defined(TEST_FIFO_MODE)
void* TestFuncUseFIFO(void* arg)
{
	int len = 0, count = 0;
	char getstr[256] = "", sendstr[256] = "";

	itpRegisterDevice(TEST_PORT, &TEST_DEVICE);
	ioctl(TEST_PORT, ITP_IOCTL_RESET, (void*)TEST_BAUDRATE);

	InitRS485Intr(TEST_PORT);

	printf("Start RS485 FIFO mode test!\n");

	sem_init(&RS485SemFifo, 0, 0);

	while (1)
	{
		sem_wait(&RS485SemFifo);
		len = read(TEST_PORT, getstr + count, 256);
		printf("len = %d, getstr = %s\n", len, getstr);
		count += len;
		if (count >= RS485CommandLen)
		{
			printf("uart read: %s,count=%d\n", getstr, count);
			memcpy(sendstr, getstr, count);
			sendstr[count] = '\n';
			write(TEST_PORT, sendstr, count+1);

			memset(getstr, 0, count + 1);
			memset(sendstr, 0, count + 1);
			count = 0;
		}
	}
}
#endif
