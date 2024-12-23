/** @file
* PAL UART functions.
*
* @author Jim Tan
* @version 1.0
* @date 2011-2012
* @copyright ITE Tech. Inc. All Rights Reserved.
*/
#include <errno.h>
#include <sys/socket.h>
#include "openrtos/FreeRTOS.h"
#include "openrtos/queue.h"
#include "ite/ith.h"
#include "ite/itp.h"
#include "uart/uart.h"

/***
* When U're verifying UART, U could dump UART mode msg
* by turning on macro "VERIFY_UART_MODE_MSG"
***/
//#define VERIFY_UART_MODE_MSG

static UART_OBJ Uartobj[6] =
{
	INIT_UART_OBJECT(0),
	INIT_UART_OBJECT(1),
	INIT_UART_OBJECT(2),
	INIT_UART_OBJECT(3),
	INIT_UART_OBJECT(4),
	INIT_UART_OBJECT(5),
};

static void UartDefaultIntrHandler(void* arg1, uint32_t arg2)
{
	// DO NOTHING
}

#if defined(ENABLE_UART_INTR)
static void _UartIntrHandler(void *arg)
{
	ITHUartPort   port = (ITHUartPort)arg;
	uint32_t      status = ithUartClearIntr(port);
	signed char   cChar;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	QueueHandle_t txQueue, rxQueue;
	int           *pRxqueueFull = NULL;
	long          result = 0;

	UART_OBJ *uartobj = &Uartobj[UART_JUDGE_PORT(port)];

	txQueue = uartobj->xCharsForTx;
	rxQueue = uartobj->xRxedChars;
	pRxqueueFull = &uartobj->RxQueueFull;

	if (status & ITH_UART_THR_EMPTY)
	{
		unsigned int i;
		/* The interrupt was caused by the THR becoming empty.  Are there any
		more characters to transmit? */

		for (;;)
		{
			if (xQueueReceiveFromISR(txQueue, &cChar, &xHigherPriorityTaskWoken) == pdTRUE)
			{
				/* A character was retrieved from the queue so can be sent to the
				THR now. */
				ithUartPutChar(port, cChar);

				if (ithUartIsTxFull(port))
					break;
			}
			else
			{
				/* Queue empty, nothing to send so turn off the Tx interrupt. */
				ithUartDisableIntr(port, ITH_UART_TX_READY);
				break;
			}
		}
	}

	if (status & ITH_UART_RECV_READY)
	{
		/* The interrupt was caused by a character being received.  Grab the
		character from the RHR and place it in the queue or received
		characters. */
		while (ithUartIsRxReady(port))
		{
			cChar = ithUartGetChar(port);
			result = xQueueSendFromISR(rxQueue, &cChar, &xHigherPriorityTaskWoken);
			if (result == 0)
				*pRxqueueFull = 1;
			//ithPrintf("intr cb from port: 0x%x  char: %x\n", port, cChar);
			if (uartobj->UartDeferIntrOn)
				itpPendFunctionCallFromISR(uartobj->itpUartDeferIntrHandler, NULL, 0);
			else
				uartobj->itpUartDeferIntrHandler(NULL, 0);
		}
	}

	/* If an event caused a task to unblock then we call "Yield from ISR" to
	ensure that the unblocked task is the task that executes when the interrupt
	completes if the unblocked task has a priority higher than the interrupted
	task. */
	//portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
#endif
//#elif defined(CFG_UART_DMA)
#if defined(ENABLE_UART_DMA)
static void
DummySleep(void)
{
	unsigned int idle = 100;
	unsigned int i = 0;
	unsigned int nothing = 0;

	for (i = 0; i < idle; i++)
	{
		nothing++;
	}
}

#endif // CFG_UART_INTR

static ITHUartPort uartDebugPort;
static UART_OBJ *uartDbgObj;
static unsigned int Uartbaudrate;

static int iteUartDbgPutchar(int c)
{
#if defined(ENABLE_UART_INTR)
	if (uartDbgObj->UartMode == UART_INTR_MODE)
	{
		int i = 0;
		if (ithGetCpuMode() == ITH_CPU_SYS)
		{
			QueueHandle_t txQueue;

			if (!uartDbgObj->xCharsForTx && !uartDbgObj->xRxedChars)
			{
				/* Create the queues used to hold Rx and Tx characters. */
				uartDbgObj->xRxedChars = xQueueCreate(CFG_UART_INTR_BUF_SIZE, (unsigned portBASE_TYPE) sizeof(signed char));
				uartDbgObj->xCharsForTx = xQueueCreate(CFG_UART_INTR_BUF_SIZE + 1, (unsigned portBASE_TYPE) sizeof(signed char));
			}

			txQueue = uartDbgObj->xCharsForTx;

			do
			{
				signed char cc = c; // for big endian case

				/* Place the character in the queue of characters to be transmitted. */
				if (xQueueSend(txQueue, &cc, 0) != pdPASS)
				{
					if (ithUartIsTxEmpty(uartDebugPort))
					{
						signed char cChar;
						if (xQueueReceive(txQueue, &cChar, 0) == pdTRUE)
							ithUartPutChar(uartDebugPort, cChar);
					}
					ithUartEnableIntr(uartDebugPort, ITH_UART_TX_READY);
				}
				else
				{
					i++;
				}
			} while (i < 1);

			if (ithUartIsTxEmpty(uartDebugPort))
			{
				signed char cChar;

				if (xQueueReceive(txQueue, &cChar, 0) == pdTRUE)
				{
					ithUartPutChar(uartDebugPort, cChar);
				}
			}
			ithUartEnableIntr(uartDebugPort, ITH_UART_TX_READY);
		}
		else
		{
			while (ithUartIsTxFull(uartDebugPort));
			ithUartPutChar(uartDebugPort, c);
		}
	}
#endif
	if (uartDbgObj->UartMode != 0)
	{
		// Is transmitter ready?
		while (ithUartIsTxFull(uartDebugPort));
		ithUartPutChar(uartDebugPort, c);
	}
	return c;
}

void iteUartInit(ITHUartPort port)
{
	uartDebugPort = port;
	uartDbgObj = &Uartobj[UART_JUDGE_PORT(port)];
	ithPutcharFunc = iteUartDbgPutchar;
}

void iteUartOpen(ITHUartPort port, ITHUartParity Parity)
{
	ithUartReset(port, Uartbaudrate, Parity, 1, 8);
}

void iteUartReset(ITHUartPort port, unsigned int baud)
{
	UART_OBJ *uartobj = &Uartobj[UART_JUDGE_PORT(port)];

	Uartbaudrate = baud;

	/* Set the required protocol. */
	ithUartReset(uartobj->port, baud, ITH_UART_NONE, 1, 8);

	ithUartSetMode(uartobj->port, ITH_UART_DEFAULT, uartobj->txgpio, uartobj->rxgpio);

#if defined(ENABLE_UART_INTR)
	if (uartobj->UartMode == UART_INTR_MODE)
	{
		if (!uartobj->xCharsForTx && !uartobj->xRxedChars)
		{
			/* Create the queues used to hold Rx and Tx characters. */
			uartobj->xRxedChars = xQueueCreate(CFG_UART_INTR_BUF_SIZE, (unsigned portBASE_TYPE) sizeof(signed char));
			uartobj->xCharsForTx = xQueueCreate(CFG_UART_INTR_BUF_SIZE + 1, (unsigned portBASE_TYPE) sizeof(signed char));
		}

		ithEnterCritical();

		uartobj->itpUartDeferIntrHandler = UartDefaultIntrHandler;
		uartobj->UartDeferIntrOn = 0;

		/* Enable the Rx interrupts.  The Tx interrupts are not enabled
		until there are characters to be transmitted. */
		ithIntrDisableIrq(uartobj->Intr);
		ithUartClearIntr(uartobj->port);
		ithIntrClearIrq(uartobj->Intr);

		ithIntrSetTriggerModeIrq(uartobj->Intr, ITH_INTR_LEVEL);
		ithIntrRegisterHandlerIrq(uartobj->Intr, _UartIntrHandler, (void *)uartobj->port);
		ithUartEnableIntr(uartobj->port, ITH_UART_RX_READY);

		/* Enable the interrupts. */
		ithIntrEnableIrq(uartobj->Intr);
		ithExitCritical();
	}
#endif
#if defined(ENABLE_UART_DMA)
	if (uartobj->UartMode == UART_DMA_MODE)
	{
		LLP_CONTEXT *llpaddr = NULL;

		uartobj->readDmaChannel = ithDmaRequestCh(uartobj->rdch_name, ITH_DMA_CH_PRIO_HIGH_3, NULL, NULL);
		ithDmaReset(uartobj->readDmaChannel);
		uartobj->writeDmaChannel = ithDmaRequestCh(uartobj->wtch_name, ITH_DMA_CH_PRIO_HIGHEST, NULL, NULL);
		ithDmaReset(uartobj->writeDmaChannel);

		uartobj->tempTxBuf = (uint8_t *)itpVmemAlloc(DMA_BUFFER_SIZE);
		uartobj->gpDMABuffer = (uint8_t *)itpVmemAlloc(DMA_BUFFER_SIZE);

		//ithPrintf("uartobj->gpDMABuffer=0x%x\n", uartobj->gpDMABuffer);
		if (uartobj->gpDMABuffer == NULL || uartobj->tempTxBuf == NULL)
		{
			ithPrintf("Alloc DMA buffer fail\n");
		}
		else
		{
			uartobj->g_LLPCtxt = (LLP_CONTEXT *)itpVmemAlloc(sizeof(LLP_CONTEXT)+32);
			//ithPrintf("--- g_LLPCtxt addr = 0x%x , sizeof(LLP_CONTEXT) = 0x%x---\n", uartobj->g_LLPCtxt, sizeof(LLP_CONTEXT));
			llpaddr = (LLP_CONTEXT *)(((uint32_t)uartobj->g_LLPCtxt + 0x1F) & ~(0x1F));
			//ithPrintf("new g_LLPCtxt addr = 0x%x\n", llpaddr);
			llpaddr->SrcAddr = le32_to_cpu(port);
			llpaddr->DstAddr = le32_to_cpu(uartobj->gpDMABuffer);
			llpaddr->LLP = le32_to_cpu(llpaddr);
			llpaddr->TotalSize = le32_to_cpu(DMA_BUFFER_SIZE);
			llpaddr->Control = le32_to_cpu(0x00210000);

			ithDmaSetSrcAddr(uartobj->readDmaChannel, port);
			ithDmaSetDstAddr(uartobj->readDmaChannel, (uint32_t)uartobj->gpDMABuffer);
			ithDmaSetRequest(uartobj->readDmaChannel, ITH_DMA_HW_HANDSHAKE_MODE, uartobj->dma_rx_req, ITH_DMA_NORMAL_MODE, ITH_DMA_MEM);

			ithDmaSetSrcParams(uartobj->readDmaChannel, ITH_DMA_WIDTH_8, ITH_DMA_CTRL_FIX, ITH_DMA_MASTER_1);
			ithDmaSetDstParams(uartobj->readDmaChannel, ITH_DMA_WIDTH_8, ITH_DMA_CTRL_INC, ITH_DMA_MASTER_0);
			ithDmaSetTxSize(uartobj->readDmaChannel, DMA_BUFFER_SIZE);
			ithDmaSetBurst(uartobj->readDmaChannel, ITH_DMA_BURST_1);

			ithDmaSetLLPAddr(uartobj->readDmaChannel, (uint32_t)llpaddr);
			//ithPrintf("llpaddr:0x%x\n", (uint32_t)llpaddr);

			ithUartEnableDmaMode2(port);
			ithDmaStart(uartobj->readDmaChannel);
		}
	}
#endif
}

int iteUartRead(ITHUartPort port, char *ptr, int len)
{
	UART_OBJ *uartobj = &Uartobj[UART_JUDGE_PORT(port)];

#if defined(ENABLE_UART_INTR)
	uint32_t  lasttime = 0;
	if (uartobj->UartMode == UART_INTR_MODE)
	{
		QueueHandle_t rxQueue = uartobj->xRxedChars;
		int *pRxqueueFull = &uartobj->RxQueueFull;
		int count = 0;

		if (uartobj->timeout)
		{
			if (xQueueReceive(rxQueue, ptr, 0))
			{
				count++;
				ptr++;
				lasttime = itpGetTickCount();
				while (count < len)
				{
					if (xQueueReceive(rxQueue, ptr, 0))
					{
						count++;
						ptr++;
					}
					else if (itpGetTickDuration(lasttime) < uartobj->timeout)
					{
						usleep(50);
					}
					else
						break;
				}
			}
		}
		else
		{
			if (xQueueReceive(rxQueue, ptr, 0))
			{
				count++;
				ptr++;
				while (count < len)
				{
					if (xQueueReceive(rxQueue, ptr, 0))
					{
						count++;
						ptr++;
					}
					else
						break;
				}
			}
		}
		if (*pRxqueueFull)
			printf("rxqueue is Full\n");
#ifdef VERIFY_UART_MODE_MSG
		printf("UartRead through Interrupt at port: 0x%x\n", port);
#endif
		return count;
	}
#endif
#if defined(ENABLE_UART_DMA)
	if (uartobj->UartMode == UART_DMA_MODE)
	{
		uint32_t dataSize = 0;
		uint32_t transferSize = ithReadRegA(ITH_DMA_BASE + ITH_DMA_SIZE_CH(uartobj->readDmaChannel));

		uartobj->gWriteIndex = (DMA_BUFFER_SIZE - transferSize);
		if (uartobj->gWriteIndex > DMA_BUFFER_SIZE)
		{
			//ithUartDisableDmaMode2(uartobj->port);
			return dataSize;
		}
		if (uartobj->gWriteIndex != uartobj->gReadIndex)
		{
			//ithPrintf("[transferSize]:0x%x [gWriteIndex]:0x%x [gReadIndex]:0x%x\n", transferSize, uartobj->gWriteIndex, uartobj->gReadIndex);
			if (uartobj->gWriteIndex < uartobj->gReadIndex)
			{
				dataSize = (DMA_BUFFER_SIZE - uartobj->gReadIndex) + uartobj->gWriteIndex;
				ithInvalidateDCacheRange(uartobj->gpDMABuffer, DMA_BUFFER_SIZE);
				memcpy(ptr, uartobj->gpDMABuffer + uartobj->gReadIndex, DMA_BUFFER_SIZE - uartobj->gReadIndex);
				memcpy(ptr + (DMA_BUFFER_SIZE - uartobj->gReadIndex), uartobj->gpDMABuffer, uartobj->gWriteIndex);
			}
			else
			{
				dataSize = uartobj->gWriteIndex - uartobj->gReadIndex;
				ithInvalidateDCacheRange(uartobj->gpDMABuffer, DMA_BUFFER_SIZE);
				memcpy(ptr, uartobj->gpDMABuffer + uartobj->gReadIndex, dataSize);
			}

			uartobj->gReadIndex = uartobj->gWriteIndex;
			//ithPrintf("dataSize:%d\n", dataSize);
		}

		//ithUartDisableDmaMode2(port);
#ifdef VERIFY_UART_MODE_MSG
		printf("UartRead through DMA at port: 0x%x\n", port);
#endif
		return dataSize;
	}
#endif
#if defined(ENABLE_UART_FIFO)
	if (uartobj->UartMode == UART_FIFO_MODE)
	{
#define TIMEOUT 1000
		int count = 0, timeout = TIMEOUT;
#ifdef VERIFY_UART_MODE_MSG
		if (len > 0)
			ithPrintf("UartRead through Default FIFO at port: 0x%x\n", port);
#endif
		while (count < len)
		{
			// Is a character waiting?
			if (ithUartIsRxReady(port))
			{
				*ptr++ = ithUartGetChar(port);// Read character from uart
				timeout = TIMEOUT;
				count++;
			}
			else if (timeout-- <= 0)
			{
				//ithPrintf("Uart FIFO read timeout!!! count=%d len=%d\n", count, len);
				return count;
			}
		}
		return count;
	}
#endif

	return len;
}

int iteUartWrite(ITHUartPort port, char *ptr, int len)//(int file, char *ptr, int len, void *info)
{
	UART_OBJ *uartobj = &Uartobj[UART_JUDGE_PORT(port)];

#if defined(ENABLE_UART_INTR)
	if (uartobj->UartMode == UART_INTR_MODE)
	{
		int i = 0;
		QueueHandle_t txQueue = uartobj->xCharsForTx;

		do
		{
			/* Place the character in the queue of characters to be transmitted. */
			if (xQueueSend(txQueue, &ptr[i], 0) != pdPASS)
			{
				if (ithUartIsTxEmpty(port))
				{
					signed char cChar = 0;
					if (xQueueReceive(txQueue, &cChar, 0) == pdTRUE)
						ithUartPutChar(port, cChar);
				}
				ithUartEnableIntr(port, ITH_UART_TX_READY);
			}
			else
			{
				i++;
			}
		} while (i < len);

		if (ithUartIsTxEmpty(port))
		{
			signed char cChar = 0;
			if (xQueueReceive(txQueue, &cChar, 0) == pdTRUE)
			{
				ithUartPutChar(port, cChar);
			}
		}
		/* Turn on the Tx interrupt so the ISR will remove the character from the
		queue and send it.   This does not need to be in a critical section as
		if the interrupt has already removed the character the next interrupt
		will simply turn off the Tx interrupt again. */
		ithUartEnableIntr(port, ITH_UART_TX_READY);
#ifdef VERIFY_UART_MODE_MSG
		ithPrintf("UartWrite through Interrput at port: 0x%x\n", port);
#endif
	}
#endif
#if defined(ENABLE_UART_DMA)
	if (uartobj->UartMode == UART_DMA_MODE)
	{
		if (uartobj->tempTxBuf == NULL)
		{
			return 0;
		}

		memcpy(uartobj->tempTxBuf, ptr, len);
		ithDmaSetSrcAddr(uartobj->writeDmaChannel, (uint32_t)uartobj->tempTxBuf);
		ithDmaSetDstAddr(uartobj->writeDmaChannel, port);

		ithDmaSetRequest(uartobj->writeDmaChannel, ITH_DMA_NORMAL_MODE, ITH_DMA_MEM, ITH_DMA_HW_HANDSHAKE_MODE, uartobj->dma_tx_req);

		ithDmaSetSrcParams(uartobj->writeDmaChannel, ITH_DMA_WIDTH_8, ITH_DMA_CTRL_INC, ITH_DMA_MASTER_0);
		ithDmaSetDstParams(uartobj->writeDmaChannel, ITH_DMA_WIDTH_8, ITH_DMA_CTRL_FIX, ITH_DMA_MASTER_1);
		ithDmaSetTxSize(uartobj->writeDmaChannel, len);
		ithDmaSetBurst(uartobj->writeDmaChannel, ITH_DMA_BURST_1);
		//ithUartEnableDmaMode2((ITHUartPort)info);

		ithDmaStart(uartobj->writeDmaChannel);

		while (ithDmaIsBusy(uartobj->writeDmaChannel) /*&& --timeout_ms*/)
		{
			DummySleep();
		}
#ifdef VERIFY_UART_MODE_MSG
		ithPrintf("UartWrite through DMA at port: 0x%x\n", port);
#endif
		//ithUartDisableDmaMode2((ITHUartPort)info);
	}
#endif
#if defined(ENABLE_UART_FIFO)
	if (uartobj->UartMode == UART_FIFO_MODE)
	{
#define TIMEOUT 1000
		int count = 0, timeout = TIMEOUT;
#ifdef VERIFY_UART_MODE_MSG
		if (len > 0)
			ithPrintf("UartWrite through Default FIFO at port: 0x%x\n", port);
#endif
#ifdef CFG_UART_FORCE_FLUSH
		ithEnterCritical();
#endif
		while (count < len)
		{
			// Is transmitter ready?
			if (!ithUartIsTxFull(port))
			{
				ithUartPutChar(port, *ptr++);// Write character from uart
				timeout = TIMEOUT;
				count++;
			}
			else if (timeout-- <= 0)
			{
				//ithPrintf("Uart FIFO write timeout!!! count=%d len=%d\n", count, len);
#ifdef CFG_UART_FORCE_FLUSH
				ithExitCritical();
#endif
				return count;
			}
		}
#ifdef CFG_UART_FORCE_FLUSH
		ithExitCritical();
#endif
	}
#endif
	return len;
}

void iteUartRegisterCallBack(ITHUartPort port, void *ptr)
{
	UART_OBJ *uartobj = &Uartobj[UART_JUDGE_PORT(port)];
	uartobj->itpUartDeferIntrHandler = (ITPPendFunction)ptr;
}

void iteUartRegisterDeferCallBack(ITHUartPort port, void *ptr)
{
	UART_OBJ *uartobj = &Uartobj[UART_JUDGE_PORT(port)];
	uartobj->UartDeferIntrOn = 1;
	uartobj->itpUartDeferIntrHandler = (ITPPendFunction)ptr;
}

void iteUartSetTimeout(ITHUartPort port, uint32_t ptr)
{
	UART_OBJ *uartobj = &Uartobj[UART_JUDGE_PORT(port)];
	uartobj->timeout = ptr;
}

void iteUartSetGpio(ITHUartPort port, ITHUartConfig *pUartConfig)
{
	UART_OBJ *uartobj = &Uartobj[UART_JUDGE_PORT(port)];

	// Reset FIFO
	//ithUartSetRxTriggerLevel(port, ITH_UART_TRGL0);

	uartobj->rxgpio = pUartConfig->RxGpio;
	uartobj->txgpio = pUartConfig->TxGpio;
	// Reset GPIO
	ithUartSetMode(port, ITH_UART_DEFAULT, uartobj->txgpio, uartobj->rxgpio);
}

#if defined(ENABLE_UART_DMA)
void iteUartStopDMA(ITHUartPort port)
{
	UART_OBJ *uartobj = &Uartobj[UART_JUDGE_PORT(port)];

	ithDmaAbort(uartobj->readDmaChannel);
	ithDmaAbort(uartobj->writeDmaChannel);
}
#endif
