﻿#include <pthread.h>
#include "openrtos/FreeRTOS.h"
#include "openrtos/task.h"
#include "ite/itp.h"

#define TEST_STACK_SIZE 500000

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
#define VERIFY_INTR_MODE
#endif

#if defined(CFG_RS485_0_DMA) || defined(CFG_RS485_1_DMA)\
	|| defined(CFG_RS485_2_DMA) || defined(CFG_RS485_3_DMA)\
	|| defined(CFG_RS485_4_DMA) || defined(CFG_RS485_5_DMA)
#define VERIFY_DMA_MODE
#endif

#if defined(CFG_RS485_0_FIFO) || defined(CFG_RS485_1_FIFO)\
	|| defined(CFG_RS485_2_FIFO) || defined(CFG_RS485_3_FIFO)\
	|| defined(CFG_RS485_4_FIFO) || defined(CFG_RS485_5_FIFO)
#define VERIFY_FIFO_MODE
#endif

#if defined(VERIFY_INTR_MODE)
extern void* TestFuncUseINTR(void* arg);
#elif defined(VERIFY_DMA_MODE)
extern void* TestFuncUseDMA(void* arg);
#elif defined(VERIFY_FIFO_MODE)
extern void* TestFuncUseFIFO(void* arg);
#endif

int main(void)
{
    pthread_t task;
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    attr.stacksize = TEST_STACK_SIZE;

#if defined(VERIFY_INTR_MODE)
	pthread_create(&task, &attr, TestFuncUseINTR, NULL);
#elif defined(VERIFY_DMA_MODE)
	pthread_create(&task, &attr, TestFuncUseDMA, NULL);
#elif defined(VERIFY_FIFO_MODE)
	pthread_create(&task, &attr, TestFuncUseFIFO, NULL);
#endif

    /* Now all the tasks have been started - start the scheduler. */
    vTaskStartScheduler();

    /* Should never reach here! */
    return 0;
}
