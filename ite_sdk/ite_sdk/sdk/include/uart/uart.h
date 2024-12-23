#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "openrtos/queue.h"

typedef enum
{
	UART_INTR_MODE	= 0,
	UART_DMA_MODE	= 1,
	UART_FIFO_MODE	= 2,
} UartMode;

#define UART_JUDGE_PORT(port) ((port - ITH_UART0) >> 8)

#if defined(CFG_UART0_INTR) || defined(CFG_UART1_INTR)\
	|| defined(CFG_UART2_INTR) || defined(CFG_UART3_INTR)\
	|| defined(CFG_UART4_INTR) || defined(CFG_UART5_INTR)
#define ENABLE_UART_INTR
#endif

#if defined(CFG_UART0_DMA) || defined(CFG_UART1_DMA)\
	|| defined(CFG_UART2_DMA) || defined(CFG_UART3_DMA)\
	|| defined(CFG_UART4_DMA) || defined(CFG_UART5_DMA)
#define ENABLE_UART_DMA
#endif

#if defined(CFG_UART0_FIFO) || defined(CFG_UART1_FIFO)\
	|| defined(CFG_UART2_FIFO) || defined(CFG_UART3_FIFO)\
	|| defined(CFG_UART4_FIFO) || defined(CFG_UART5_FIFO)
#define ENABLE_UART_FIFO
#endif

#ifdef CFG_GPIO_UART0_RX
#define UART0_TX CFG_GPIO_UART0_TX
#define UART0_RX CFG_GPIO_UART0_RX
#else
#define UART0_TX -1
#define UART0_RX -1
#endif

#ifdef CFG_UART0_INTR
#define UART0_MODE UART_INTR_MODE
#elif CFG_UART0_DMA
#define UART0_MODE UART_DMA_MODE
#else
#define UART0_MODE UART_FIFO_MODE
#endif

#ifdef CFG_GPIO_UART1_RX
#define UART1_TX CFG_GPIO_UART1_TX
#define UART1_RX CFG_GPIO_UART1_RX
#else
#define UART1_TX -1
#define UART1_RX -1
#endif

#ifdef CFG_UART1_INTR
#define UART1_MODE UART_INTR_MODE
#elif CFG_UART1_DMA
#define UART1_MODE UART_DMA_MODE
#else
#define UART1_MODE UART_FIFO_MODE
#endif

#ifdef CFG_GPIO_UART2_RX
#define UART2_TX CFG_GPIO_UART2_TX
#define UART2_RX CFG_GPIO_UART2_RX
#else
#define UART2_TX -1
#define UART2_RX -1
#endif

#ifdef CFG_UART2_INTR
#define UART2_MODE UART_INTR_MODE
#elif CFG_UART2_DMA
#define UART2_MODE UART_DMA_MODE
#else
#define UART2_MODE UART_FIFO_MODE
#endif

#ifdef CFG_GPIO_UART3_RX
#define UART3_TX CFG_GPIO_UART3_TX
#define UART3_RX CFG_GPIO_UART3_RX
#else
#define UART3_TX -1
#define UART3_RX -1
#endif

#ifdef CFG_UART3_INTR
#define UART3_MODE UART_INTR_MODE
#elif CFG_UART3_DMA
#define UART3_MODE UART_DMA_MODE
#else
#define UART3_MODE UART_FIFO_MODE
#endif

#ifdef CFG_GPIO_UART4_RX
#define UART4_TX CFG_GPIO_UART4_TX
#define UART4_RX CFG_GPIO_UART4_RX
#else
#define UART4_TX -1
#define UART4_RX -1
#endif

#ifdef CFG_UART4_INTR
#define UART4_MODE UART_INTR_MODE
#elif CFG_UART4_DMA
#define UART4_MODE UART_DMA_MODE
#else
#define UART4_MODE UART_FIFO_MODE
#endif

#ifdef CFG_GPIO_UART5_RX
#define UART5_TX CFG_GPIO_UART5_TX
#define UART5_RX CFG_GPIO_UART5_RX
#else
#define UART5_TX -1
#define UART5_RX -1
#endif

#ifdef CFG_UART5_INTR
#define UART5_MODE UART_INTR_MODE
#elif CFG_UART5_DMA
#define UART5_MODE UART_DMA_MODE
#else
#define UART5_MODE UART_FIFO_MODE
#endif

#ifdef ENABLE_UART_DMA
#define INIT_UART_OBJECT(port) \
	{\
	ITH_UART##port, \
	ITH_INTR_UART##port, \
	UART##port##_TX, \
	UART##port##_RX, \
	UART##port##_MODE, \
	"dma_uart"#port"_write", \
	"dma_uart"#port"_read", \
	ITH_DMA_UART##port##_TX, \
	ITH_DMA_UART##port##_RX, \
	}
#else
#define INIT_UART_OBJECT(port) \
	{\
	ITH_UART##port, \
	ITH_INTR_UART##port, \
	UART##port##_TX, \
	UART##port##_RX, \
	UART##port##_MODE, \
	}
#endif

#if defined(ENABLE_UART_DMA)
#define DMA_BUFFER_SIZE CFG_UART_DMA_BUF_SIZE
#define DMA_TIMEOUT     10000

typedef struct LLP_CONTEXT_TAG
{
	uint32_t  SrcAddr;
	uint32_t  DstAddr;
	uint32_t  LLP;
	uint32_t  Control;
	uint32_t  TotalSize;
}LLP_CONTEXT;
#endif

typedef struct _UART_OBJ
{
	ITHUartPort		port;
	ITHIntr			Intr;
	int				txgpio;
	int				rxgpio;
	uint8_t			UartMode;
#if defined(ENABLE_UART_DMA)
	char*			wtch_name;
	char*			rdch_name;
	int				dma_tx_req;
	int				dma_rx_req;
	int				writeDmaChannel;
	int				readDmaChannel;
	uint8_t *		tempTxBuf;
	uint8_t *		gpDMABuffer;
	uint32_t		gWriteIndex;
	uint32_t		gReadIndex;
	LLP_CONTEXT *	g_LLPCtxt;
#endif
	QueueHandle_t	xRxedChars;
	QueueHandle_t	xCharsForTx;
	int				RxQueueFull;
	uint8_t			UartDeferIntrOn;
	uint32_t		timeout;
	ITPPendFunction  itpUartDeferIntrHandler;
}UART_OBJ;

/**
* @brief Set debug message callback function.
*
* @param port UART debug port.
*
*/
void iteUartInit(ITHUartPort port);

/**
* @brief Set UART port's parity.
*
* @param port UART device port.
*
* @param Parity UART device port's Parity.
*
*/
void iteUartOpen(ITHUartPort port, ITHUartParity Parity);

/**
* @brief Initialize UART port. All detail are set in Kconfig setting time.
*
* @param port UART device port.
*
* @param baud UART device port's baudrate.
*
*/
void iteUartReset(ITHUartPort port, unsigned int baud);

/**
* @brief Fetch data from receive queue.
*
* @param port UART device port.
*
* @return fetched data length.
*
*/
int iteUartRead(ITHUartPort port, char *ptr, int len);

/**
* @brief Deliver data to transmit queue.
*
* @param port UART device port.
*
* @return delivered data length.
*
*/
int iteUartWrite(ITHUartPort port, char *prt, int len);

/**
* @brief Register interrupt callback function.
*
* @param port UART device port.
*
* @param *ptr potinter of callback function.
*
*/
void iteUartRegisterCallBack(ITHUartPort port, void *ptr);

/**
* @brief Register interrupt defer callback function.
*
* @param port UART device port.
*
* @param *ptr potinter of defer callback function.
*
*/
void iteUartRegisterDeferCallBack(ITHUartPort port, void *ptr);

/**
* @brief Set time out value.
*
* @param port UART device port.
*
* @param ptr timeout value.
*
*/
void iteUartSetTimeout(ITHUartPort port, uint32_t ptr);

/**
* @brief Set UART TX/RX GPIO.
*
* @param port UART device port.
*
* @param *pUartConfig a pointer to a structure of ITHUartConfig.
*
*/
void iteUartSetGpio(ITHUartPort port, ITHUartConfig *pUartConfig);

/**
* @brief Stop specific port's DMA function.
*
* @param port UART device port.
*
*/
void iteUartStopDMA(ITHUartPort port);

#ifdef __cplusplus
}
#endif

#endif
