/*
 * Copyright (c) 2011 ITE Tech. Inc. All Rights Reserved.
 */
/** @file
 * PAL Remote IR functions.
 *
 * @author Jim Tan
 * @version 1.0
 */
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "openrtos/FreeRTOS.h"
#include "openrtos/queue.h"
#include "itp_cfg.h"

//#define ENABLE_DBG_COMPARE_CODE

typedef struct LLP_CONTEXT_TAG
{
    uint32_t  SrcAddr;
    uint32_t  DstAddr;
    uint32_t  LLP;
    uint32_t  Control;
    uint32_t  TotalSize;
}LLP_CONTEXT;

#define IR_DMA_BUFFER_SIZE      1024
#define IR_DMA_TRANSFER_SIZE    IR_DMA_BUFFER_SIZE/2 //16bit, 1:8bit 2:16bit 4:32bit

static const uint32_t irTypeTable[] =
{
#include "ir_type.inc"
};

#if 1
static const uint32_t irValidTxCodeTable[] =
{
    0x55,0xAA
};
#else
static const uint32_t irValidTxCodeTable[] =
{
#include "ir_TxTable.inc"
};
#endif

#define QUEUE_LEN               256
#define STATE_NUM               (5)
#define MAX_VAL                 ((1<<10)-1)

#define PRECISION               16
#define SAMP_DUR                10    // sample duration in microseconds

#define REPEAT_THRESHOLD_BEGIN  (5)     // accumulate number of repeat-key will start dispatch key
#define REPEAT_THRESHOLD_SPDUP  (2)     // accumulate number of key will change to high speed mode
#define REPEAT_THRESHOLD_HOLD1  (5)     // dispatch a key foreach number of repeat-key at low speed mode
#define REPEAT_THRESHOLD_HOLD2  (1)     // dispatch a key foreach number of repeat-key at high speed mode

enum RCState {
    WAIT_START,
    WAIT_START_REPEAT_KEY,      // hiden state for repeat key
    WAIT_BIT,
    WAIT_BIT_ONE,               // hiden state for a "1" bit
    WAIT_END
};

uint16_t    g_IrTxBuf[]={0x7c, 0x15, 0x15, 0x0a, 0x0a, 0x0b, 0x0b, 0x0a,
                        0x0b, 0x0a, 0x0a, 0x15, 0x15, 0x14, 0x15, 0x15,
                        0x15, 0x14, 0x0b, 0x0a, 0x0a, 0x15, 0x0a, 0x0b,
                        0x0a, 0x0b, 0x14, 0x15, 0x15, 0x0a, 0x15, 0x14,
                        0x15, 0x174,0x68};

static int irThresholds[STATE_NUM][2];     // min & max for a signal stste
static int  g_RxIndex=0;                   // ENABLE_DBG_COMPARE_CODE

typedef struct _IR_OBJ {
	ITHIrPort port;
    ITHIrMode mode;                 // Fifo default, DMA, interrupt mode
	ITHIntr TxIntr;                 // Tx interrupt number
	ITHIntr	RxIntr;                 // Rx interrupt number
	int TxGpio;
	int RxGpio;
	QueueHandle_t TxQueue;
	QueueHandle_t RxQueue;
	enum RCState irCurrState;
	unsigned long irRecvCode;        // LSB received code
	unsigned long irRecvCodeH;       // MSB received code if irTypeTable[5] > 32
	int irRepeatKeyPress;
	int irRepeatKeyHold;
	int irRepeatKeyFast;
	int irRepeatKeyCnt;
	int irRecvBitCount;
	int irWidth;	
/* DMA definaton start */
	char *TxChName;                 // Tx DMA channel name
	char *RxChName;                 // Rx DMA channel name
	int TxChannelReq;
	int RxChannelReq;
	int TxChannel;
	int RxChannel;
	uint16_t *TxBuffer;
	uint16_t *RxBuffer;
	int RxReadIdx;
	int RxWriteIdx;
	LLP_CONTEXT *gLLPCtxt;
/* DMA defination end */
	ITPKeypadEvent irLastEvent;
} IR_OBJ;

static IR_OBJ IrObj[4] =
{
	{
		ITH_IR0,
        ITH_FIFO_DEFAULT,
		ITH_INTR_IR0TX,
		ITH_INTR_IR0RX,
#ifdef CFG_GPIO_IR0_RX
		CFG_GPIO_IR0_TX,
		CFG_GPIO_IR0_RX,
#else
		-1,
		-1,
#endif
		NULL,
		NULL,
		WAIT_START,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"dma_ir0_write",            // TxChName
		"dma_ir0_read",             // RxChName
		ITH_DMA_IR_CAP_TX,
		ITH_DMA_IR_CAP_RX,
	},
	{
		ITH_IR1,
        ITH_FIFO_DEFAULT,
		ITH_INTR_IR1TX,
		ITH_INTR_IR1RX,
#ifdef CFG_GPIO_IR1_RX
		CFG_GPIO_IR1_TX,
		CFG_GPIO_IR1_RX,
#else
		-1,
		-1,
#endif
		NULL,
		NULL,
		WAIT_START,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"dma_ir1_write",            // TxChName
		"dma_ir1_read",             // RxChName
		ITH_DMA_IR_CAP1_TX,
		ITH_DMA_IR_CAP1_RX,
	},
	{
		ITH_IR2,
        ITH_FIFO_DEFAULT,
		ITH_INTR_IR2TX,
		ITH_INTR_IR2RX,
#ifdef CFG_GPIO_IR2_RX
		CFG_GPIO_IR2_TX,
		CFG_GPIO_IR2_RX,
#else
		-1,
		-1,
#endif
		NULL,
		NULL,
		WAIT_START,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"dma_ir2_write",            // TxChName
		"dma_ir2_read",             // RxChName
		ITH_DMA_IR_CAP2_TX,
		ITH_DMA_IR_CAP2_RX,
	},
	{
		ITH_IR3,
        ITH_FIFO_DEFAULT,
		ITH_INTR_IR3TX,
		ITH_INTR_IR3RX,
#ifdef CFG_GPIO_IR3_RX
		CFG_GPIO_IR3_TX,
		CFG_GPIO_IR3_RX,
#else
		-1,
		-1,
#endif
		NULL,
		NULL,
		WAIT_START,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"dma_ir3_write",            // TxChName
		"dma_ir3_read",             // RxChName
		ITH_DMA_IR_CAP3_TX,
		ITH_DMA_IR_CAP3_RX,
	}
};

static inline int MULSHIFT(int x, int y, int shift)
{
    int64_t xext, yext;
    xext = (int64_t)x;
    yext = (int64_t)y;
    xext = ((xext * yext) >> shift);
    return (int)xext;
}

static int IrProbe(ITHIrPort port, int signal)
{
    int completeACode = 0, code;
    unsigned long getBit;
	IR_OBJ *ir_obj = NULL;

	if(port == ITH_IR0) {
		ir_obj = &IrObj[0];
	}
	else if(port == ITH_IR1) {
		ir_obj = &IrObj[1];
	}
	else if(port == ITH_IR2) {
		ir_obj = &IrObj[2];
	}
	else if(port == ITH_IR3) {
		ir_obj = &IrObj[3];
	}
	else {
		ir_obj = &IrObj[0];
	}

    if(signal != -1)
    {
        printf("IrProbe(),signal=0x%x\n",signal);

        #ifdef  ENABLE_DBG_COMPARE_CODE
        {
            uint32_t modFreq = ithReadRegA(ir_obj->port + ITH_IR_MOD_FILTER_REG);

            if(signal==0x3FFF)
            {
                g_RxIndex = 0;
            }
            else
            {
                int highR,lowR;

                if( 20 > g_IrTxBuf[g_RxIndex] )
                {
                    highR = g_IrTxBuf[g_RxIndex] + 1;
                    if(g_IrTxBuf[g_RxIndex]>1)  lowR = g_IrTxBuf[g_RxIndex] - 1;
                    else                        lowR = 0;
                }
                else
                {
                    highR = g_IrTxBuf[g_RxIndex] + (g_IrTxBuf[g_RxIndex]*5)/100;
                    lowR = g_IrTxBuf[g_RxIndex] - (g_IrTxBuf[g_RxIndex]*5)/100;
                }

                printf(" ### check ir-RX code, signal=%x, g_RxIndex=%x,H&L=[%x,%x], modfrq=%x\n",signal,g_RxIndex,highR,lowR,modFreq);
                if( (signal > highR) || (signal < lowR) )
                //if( signal != g_IrTxBuf[g_RxIndex] )
                {
                    printf("error, IR code RX != TX, index=%x,[%x,%x] \n",g_RxIndex,signal,g_IrTxBuf[g_RxIndex]);
                    //while(1);
                }
                g_RxIndex++;
            }
        }
        #endif

        switch (ir_obj->irCurrState)
        {
        case WAIT_START:
            //printf("WStart\n");
            if ((signal >= irThresholds[WAIT_START][0]) && (signal <= irThresholds[WAIT_START][1]))
            {
                //printf("WB.1\n");
                ir_obj->irCurrState = WAIT_BIT;
            }
            else if ((signal >= irThresholds[WAIT_START_REPEAT_KEY][0]) && (signal <= irThresholds[WAIT_START_REPEAT_KEY][1]))
            {
                code = ir_obj->irRecvCode;

                //printf("WE.0,code=%x\n",irRecvCode);

            #ifdef CFG_IR_REPEAT
                if (ir_obj->irRepeatKeyPress >= REPEAT_THRESHOLD_BEGIN)
                {
                    //ithPrintf("Hold\n");

                    if ((ir_obj->irRepeatKeyFast == 0 && ir_obj->irRepeatKeyHold >= REPEAT_THRESHOLD_HOLD1) ||
                        (ir_obj->irRepeatKeyFast == 1 && ir_obj->irRepeatKeyHold >= REPEAT_THRESHOLD_HOLD2))
                    {
                        //ithPrintf("Send repeat key (%08x)\n", irRecvCode);

                        completeACode = 1;
                        ir_obj->irRepeatKeyHold = 0;
                        if (ir_obj->irRepeatKeyCnt >= REPEAT_THRESHOLD_SPDUP)
                            ir_obj->irRepeatKeyFast = 1;
                        else
                            ir_obj->irRepeatKeyCnt++;
                    }
                    else
                        ir_obj->irRepeatKeyHold++;
                }
                else
                    ir_obj->irRepeatKeyPress++;

            #endif // CFG_IR_REPEAT

                ir_obj->irCurrState = WAIT_END;
            }
            else
            {
                //printf("W_S\n");
                ir_obj->irCurrState = WAIT_START; // error
            }

            break;

        case WAIT_BIT:
            ir_obj->irRepeatKeyPress = 0;
			ir_obj->irRepeatKeyHold = 0;
            ir_obj->irRepeatKeyFast = 0;
			ir_obj->irRepeatKeyCnt = 0;
            if ((signal >= irThresholds[WAIT_BIT][0]) && (signal <= irThresholds[WAIT_BIT][1]))
            {
                // bit "0"
                getBit = 0;
            }
            else if ((signal >= irThresholds[WAIT_BIT_ONE][0]) && (signal <= irThresholds[WAIT_BIT_ONE][1]))
            {
                // bit "1"
                if (irTypeTable[6])
                    getBit = 0x80000000L;
                else
                    getBit = 0x00000001L;
            }
            else
            {
                // error
                ir_obj->irRecvCode = 0;
                ir_obj->irRecvCodeH = 0;
				ir_obj->irRecvBitCount = 0;
                ir_obj->irCurrState = WAIT_START;
                break;
            }

            if (irTypeTable[6])
            {
                ir_obj->irRecvCodeH = (ir_obj->irRecvCodeH >> 1) | ((ir_obj->irRecvCode & 1) << 31);
                ir_obj->irRecvCode  = (ir_obj->irRecvCode  >> 1) | getBit;
            }
            else
            {
                ir_obj->irRecvCodeH = (ir_obj->irRecvCodeH << 1) | (ir_obj->irRecvCode & 0x80000000L);
                ir_obj->irRecvCode  = (ir_obj->irRecvCode  << 1) | getBit;
            }

            ir_obj->irRecvBitCount++;
            if (ir_obj->irRecvBitCount < irTypeTable[5])
                ir_obj->irCurrState = WAIT_BIT;   // not yet complet a code
            else
            {
                ir_obj->irCurrState = WAIT_END;
            }
            break;

        case WAIT_END:
            printf("IR wait end!\n");
            if (signal >= irThresholds[WAIT_END][1]/*(signal >= irThresholds[WAIT_END][0]) && (signal <= irThresholds[WAIT_END][1])*/)
            {
                if (ir_obj->irRecvBitCount >= irTypeTable[5])
                {
                    //printf("WE1:code=%x\n",irRecvCode);
                    completeACode = 1;
                    code = ir_obj->irRecvCode;
                }
                else
                {
                    //printf("WE2\n");
                    completeACode  = 0;
                }

                ir_obj->irRecvBitCount = 0;
                ir_obj->irCurrState = WAIT_START;
            }
            else
            {
                // error
                //printf("WE3\n");
                ir_obj->irRecvBitCount = 0;
                ir_obj->irRepeatKeyPress = 0;
				ir_obj->irRepeatKeyHold = 0;
                ir_obj->irRepeatKeyFast = 0;
				ir_obj->irRepeatKeyCnt = 0;
                completeACode = 0;
                ir_obj->irCurrState = WAIT_START;
            }
            break;

        default:
            printf(" probe_default!!\n");
            break;
        }

        if (completeACode)
        {
            //printf(" got IR code=%x\n",code);
            return code;
        }
    }
    //printf("IR NOT complete!!\n");
    return -1;
}

static void IrTxSend(ITHIrPort port, uint16_t *ptr)
{
    int i;
    int IrTxCnt = sizeof(g_IrTxBuf)/sizeof(uint16_t);

    if(ptr) printf("### IrTxSend() ###\n");
    printf("## DO RX clear ##\n");
    ithIrCtrlEnable(port, ITH_IR_EN);
    for(i=0; i<IrTxCnt; i++)
    {
        printf("irTx send:[%x]=%x\n",i,ptr[i]);
        ithIrTxTransmit(port, ptr[i]);
    }
}

static void IrIntrHandler(void* arg)
{
	ITHIrPort port = (ITHIrPort) arg;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    int code = 0, signal = 0;
	IR_OBJ *ir_obj = NULL;

	if(port == ITH_IR0) {
		ir_obj = &IrObj[0];
	}
	else if(port == ITH_IR1) {
		ir_obj = &IrObj[1];
	}
	else if(port == ITH_IR2) {
		ir_obj = &IrObj[2];
	}
	else if(port == ITH_IR3) {
		ir_obj = &IrObj[3];
	}
	else {
		ir_obj = &IrObj[0];
	}	

	signal = ithIrProbe(ir_obj->port);
    code = IrProbe(ir_obj->port, signal);

	if (code != -1)
    {
        //ithPrintf("code: 0x%X\n", code);

        if (((code & 0xFFFF) == CFG_IR_VENDOR_CODE) && (((code >> 16) & 0xFF) == (~((code >> 24) & 0xFF) & 0xFF)))
            code = (code >> 16) & 0xFF;

        //ithPrintf("IR code: 0x%X\n", code);

		xQueueSendFromISR(ir_obj->RxQueue, &code, &xHigherPriorityTaskWoken);
    	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static void IrTxIntrHandler(void *arg)
{
	ITHIrPort port = (ITHIrPort) arg;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	int code = 0;
	IR_OBJ *ir_obj = NULL;

	if(port == ITH_IR0) {
		ir_obj = &IrObj[0];
	}
	else if(port == ITH_IR1) {
		ir_obj = &IrObj[1];
	}
	else if(port == ITH_IR2) {
		ir_obj = &IrObj[2];
	}
	else if(port == ITH_IR3) {
		ir_obj = &IrObj[3];
	}
	else {
		ir_obj = &IrObj[0];
	}

	if(xQueueReceiveFromISR(ir_obj->TxQueue, &code, &xHigherPriorityTaskWoken) == pdTRUE) {
		//ithPrintf("got irTX code=%x\n",code);
		//1.parse event to get IR TX code(code table)
		IrTxSend(port, g_IrTxBuf);

		//IR-TX finish. So turn off IR-TX interrupt.
		ithIrTxCtrlDisable(port, ITH_IR_INT);
	
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

static void
DummySleep(void)
{
    unsigned int idle    = 100;
    unsigned int i       = 0;
    unsigned int nothing = 0;

    for (i = 0; i < idle; i++)
    {
        nothing++;
    }
}

static void IrRxDMAHandler(void *arg)
{
	ITHIrPort port = (ITHIrPort) arg;
    uint32_t transferSize = 0;
    uint32_t dataSize = 0;
    uint16_t tmpBuffer[IR_DMA_BUFFER_SIZE]; //size is IR_DMA_BUFFER_SIZE
    int i;
    int code = 0;
	IR_OBJ *ir_obj = NULL;

	if(port == ITH_IR0) {
		ir_obj = &IrObj[0];
	}
	else if(port == ITH_IR1) {
		ir_obj = &IrObj[1];
	}
	else if(port == ITH_IR2) {
		ir_obj = &IrObj[2];
	}
	else if(port == ITH_IR3) {
		ir_obj = &IrObj[3];
	}
	else {
		ir_obj = &IrObj[0];
	}

    while(1) {
        transferSize = ithReadRegA(ITH_DMA_BASE + ITH_DMA_SIZE_CH(ir_obj->RxChannel));
		//printf("transferSize=%d\n", transferSize);
		
        ir_obj->RxWriteIdx = (IR_DMA_TRANSFER_SIZE - transferSize);

        if(ir_obj->RxWriteIdx > IR_DMA_TRANSFER_SIZE) {
            printf("IR DMA Read fail.\n");
            continue;
        }
        
        if(ir_obj->RxWriteIdx != ir_obj->RxReadIdx) {
            
            //printf("transferSize=%d RxWriteIdx=%d RxReadIdx=%d\n", transferSize, ir_obj->RxWriteIdx, ir_obj->RxReadIdx);
            memset(tmpBuffer, 0, IR_DMA_BUFFER_SIZE);
        
            if(ir_obj->RxWriteIdx < ir_obj->RxReadIdx) {
                dataSize = (IR_DMA_TRANSFER_SIZE - ir_obj->RxReadIdx) + ir_obj->RxWriteIdx;
                ithInvalidateDCacheRange(ir_obj->RxBuffer, IR_DMA_BUFFER_SIZE);
                memcpy(tmpBuffer, ir_obj->RxBuffer + ir_obj->RxReadIdx, (IR_DMA_TRANSFER_SIZE - ir_obj->RxReadIdx)*sizeof(uint16_t));
                memcpy(tmpBuffer + (IR_DMA_TRANSFER_SIZE - ir_obj->RxReadIdx), ir_obj->RxBuffer, ir_obj->RxWriteIdx*sizeof(uint16_t));
            }
            else {
                dataSize = ir_obj->RxWriteIdx - ir_obj->RxReadIdx;
                ithInvalidateDCacheRange(ir_obj->RxBuffer, IR_DMA_BUFFER_SIZE);
                memcpy(tmpBuffer, ir_obj->RxBuffer + ir_obj->RxReadIdx, dataSize*sizeof(uint16_t));
            }
            
            ir_obj->RxReadIdx = ir_obj->RxWriteIdx;
            //printf("dataSize=%d\n", dataSize);

            for(i = 0; i < dataSize; i++) {
                uint32_t tmp_reg = 0x0;
                tmp_reg = tmpBuffer[i] & ((0x1 << ir_obj->irWidth) - 1);
                code = IrProbe(ir_obj->port, tmp_reg);
            
                if (code != -1) {
                    printf("code: 0x%X\n", code);
            
                    if (((code & 0xFFFF) == CFG_IR_VENDOR_CODE) && (((code >> 16) & 0xFF) == (~((code >> 24) & 0xFF) & 0xFF)))
                        code = (code >> 16) & 0xFF;
            
                    //printf("IR code: 0x%X\n", code);                    
            
                    xQueueSend(ir_obj->RxQueue, &code, 0);                    
                }
            }           
        }
        usleep(2000);
    }
}

static void IrTxDMAHandler(void *arg)
{
	ITHIrPort port = (ITHIrPort) arg;
    int code = 0;
	IR_OBJ *ir_obj = NULL;

	if(port == ITH_IR0) {
		ir_obj = &IrObj[0];
	}
	else if(port == ITH_IR1) {
		ir_obj = &IrObj[1];
	}
	else if(port == ITH_IR2) {
		ir_obj = &IrObj[2];
	}
	else if(port == ITH_IR3) {
		ir_obj = &IrObj[3];
	}
	else {
		ir_obj = &IrObj[0];
	}

    while(1)
    {
        code=0;
        if(xQueueReceive(ir_obj->TxQueue, &code, 0))
        {
            int IrTxCnt = sizeof(g_IrTxBuf)/sizeof(uint16_t);
            
            printf("got irTX code=%x\n",code);
			//1.parse event to get IR TX code(code table)
            
            ithIrCtrlEnable(ir_obj->port, ITH_IR_EN);

            memcpy(ir_obj->TxBuffer, g_IrTxBuf, sizeof(g_IrTxBuf));
            
            ithDmaSetSrcAddr(ir_obj->TxChannel, (uint32_t)ir_obj->TxBuffer);
            ithDmaSetDstAddr(ir_obj->TxChannel, ir_obj->port + ITH_IRTX_SEND_DATA_REG);

            ithDmaSetRequest(ir_obj->TxChannel, ITH_DMA_NORMAL_MODE, ITH_DMA_MEM, ITH_DMA_HW_HANDSHAKE_MODE, ir_obj->TxChannelReq);
            ithDmaSetSrcParams(ir_obj->TxChannel, ITH_DMA_WIDTH_16, ITH_DMA_CTRL_INC, ITH_DMA_MASTER_0);
            ithDmaSetDstParams(ir_obj->TxChannel, ITH_DMA_WIDTH_16, ITH_DMA_CTRL_FIX, ITH_DMA_MASTER_1);
            ithDmaSetTxSize(ir_obj->TxChannel, sizeof(g_IrTxBuf));
            ithDmaSetBurst(ir_obj->TxChannel, ITH_DMA_BURST_1);

            ithDmaStart(ir_obj->TxChannel);

            while(ithDmaIsBusy(ir_obj->TxChannel) /*&& --timeout_ms*/)
            {
                DummySleep();
            }
            
        }
        usleep(1000);
    }

}

static void IrDefaultHandler(void* arg)
{
	ITHIrPort port = (ITHIrPort) arg;
    int code;
	IR_OBJ *ir_obj = NULL;

	if(port == ITH_IR0) {
		ir_obj = &IrObj[0];
	}
	else if(port == ITH_IR1) {
		ir_obj = &IrObj[1];
	}
	else if(port == ITH_IR2) {
		ir_obj = &IrObj[2];
	}
	else if(port == ITH_IR3) {
		ir_obj = &IrObj[3];
	}
	else {
		ir_obj = &IrObj[0];
	}

    while(1)
    {
        int signal = 0;
        //printf("start to IrProbe\n");        
        
        signal = ithIrProbe(ir_obj->port);
        code = IrProbe(ir_obj->port, signal);

        if (code != -1)
        {
            printf("code: 0x%X\n", code);

            if (((code & 0xFFFF) == CFG_IR_VENDOR_CODE) && (((code >> 16) & 0xFF) == (~((code >> 24) & 0xFF) & 0xFF)))
                code = (code >> 16) & 0xFF;

            printf("IR code: 0x%X\n", code);

            //xQueueSendFromISR(irQueue, &code, &xHigherPriorityTaskWoken);
            xQueueSend(ir_obj->RxQueue, &code, 0);
        }
        usleep(2000);
    }
}

static void IrTxDefaultHandler(void* arg)
{
	ITHIrPort port = (ITHIrPort) arg;
    int code=0;
	IR_OBJ *ir_obj = NULL;

	if(port == ITH_IR0) {
		ir_obj = &IrObj[0];
	}
	else if(port == ITH_IR1) {
		ir_obj = &IrObj[1];
	}
	else if(port == ITH_IR2) {
		ir_obj = &IrObj[2];
	}
	else if(port == ITH_IR3) {
		ir_obj = &IrObj[3];
	}
	else {
		ir_obj = &IrObj[0];
	}

    while(1)
    {
        code=0;
        if(xQueueReceive(ir_obj->TxQueue, &code, 0))
        {
            printf("got irTX code=%x\n",code);
            //1.parse event to get IR TX code(code table)
            IrTxSend(port, g_IrTxBuf);
        }
        usleep(1000);
    }
}


#define MAX_CODE_INDEX  2
static int IrCheckCodeMapping(int code)
{
    int i;

    for(i=0; i<MAX_CODE_INDEX; i++)
    {
        if(code == irValidTxCodeTable[i])   return 1;
    }
    return 0;
}

static void IrCalcThresholds(void)
{
    int i, f1, f2;
    int durations[STATE_NUM];

    durations[0] = irTypeTable[0] * (1 << PRECISION) / 1000;
    durations[1] = irTypeTable[1] * (1 << PRECISION) / 1000;
    durations[2] = irTypeTable[2] * (1 << PRECISION) / 1000;
    durations[3] = irTypeTable[3] * (1 << PRECISION) / 1000;
    durations[4] = irTypeTable[4] * (1 << PRECISION) / 1000;

    // know the sampling duration
    // set the min & max for each state
    for (i = 0; i < STATE_NUM; i++)
    {
        // 0.92 & 1.08 is experienc value, duration range N is between 0.92*N and 1.08*N.
        f1 = ((durations[i]) << 1) / (SAMP_DUR * (1 << PRECISION) / 1000);
        f2 = MULSHIFT(f1, 0.92f * (1 << PRECISION), PRECISION) >> 1;
        if (f2 >= MAX_VAL)
        {
            LOG_ERR "Out of counter resolution!\n" LOG_END
        }

        irThresholds[i][0] = (f2 < 1) ? 1 : f2; // f1 * 0.92
        irThresholds[i][1] = (i == STATE_NUM - 1) ?
            MAX_VAL :
            ((MULSHIFT(f1, 1.08f * (1 << PRECISION), PRECISION) + 1) >> 1); // f1 * 1.08 + 0.5

        //LOG_DBG "irThresholds[%d][0] = %d, irThresholds[%d][1] = %d\n", i, irThresholds[i][0], i, irThresholds[i][1] LOG_END
        printf("irThresholds[%d][0] = 0x%x, irThresholds[%d][1] = 0x%x\n", i, irThresholds[i][0], i, irThresholds[i][1]);
    }
}

static void IrInit(ITHIrPort port)
{
	IR_OBJ *ir_obj = NULL;

	if(port == ITH_IR0) {
		ir_obj = &IrObj[0];
	}
	else if(port == ITH_IR1) {
		ir_obj = &IrObj[1];
	}
	else if(port == ITH_IR2) {
		ir_obj = &IrObj[2];
	}
	else if(port == ITH_IR3) {
		ir_obj = &IrObj[3];
	}
	else {
		ir_obj = &IrObj[0];
	}

	//IR-RX init
	ithIrInit(ir_obj->port, ir_obj->RxGpio, 0, SAMP_DUR, PRECISION);
	//IR-TX init
	ithIrTxInit(ir_obj->port, ir_obj->TxGpio, 0, SAMP_DUR, PRECISION);
	IrCalcThresholds();

	//Init IR_OBJ structure
	ir_obj->irCurrState = WAIT_START;
	ir_obj->irRecvCode = 0;
	ir_obj->irRecvCodeH = 0;
	ir_obj->irRecvBitCount = 0;
	ir_obj->irRepeatKeyCnt = 0;
	ir_obj->irRepeatKeyFast = 0;
	ir_obj->irRepeatKeyHold = 0;
	ir_obj->irRepeatKeyPress = 0;
	ir_obj->irWidth = (ithReadRegA(ir_obj->port + ITH_IR_HWCFG_REG) & ITH_IR_WIDTH_MASK) >> ITH_IR_WIDTH_BIT;
	ir_obj->irLastEvent.code = -1;	

	ir_obj->RxQueue = xQueueCreate(QUEUE_LEN, (unsigned portBASE_TYPE) sizeof(int));
	ir_obj->TxQueue = xQueueCreate(QUEUE_LEN, (unsigned portBASE_TYPE) sizeof(int));

    if(ir_obj->mode == ITH_INTR_MODE) {
        ithEnterCritical();
        {
        	// Init IR(remote control) interrupt
    	    ithIntrDisableIrq(ir_obj->RxIntr);
    	    ithIntrClearIrq(ir_obj->RxIntr);
    	    ithIntrRegisterHandlerIrq(ir_obj->RxIntr, IrIntrHandler, (void *) ir_obj->port);
    	    ithIntrEnableIrq(ir_obj->RxIntr);

    		ithIrCtrlEnable(ir_obj->port, ITH_IR_INT);
    	    ithIrIntrCtrlEnable(ir_obj->port, ITH_IR_DATA);
        }

        {
            // Init IR-TX(remote control) interrupt
    		ithIntrDisableIrq(ir_obj->TxIntr);
    		ithIntrClearIrq(ir_obj->TxIntr);
    		ithIntrRegisterHandlerIrq(ir_obj->TxIntr, IrTxIntrHandler, (void *) ir_obj->port);
    		ithIntrEnableIrq(ir_obj->TxIntr);

    		ithIrTxIntrCtrlEnable(ir_obj->port, ITH_IR_EMPTY);
        }
        ithExitCritical();
    }
    else if(ir_obj->mode == ITH_DMA_MODE){
        printf("IR DMA Support\n");
        
        {
            LLP_CONTEXT *llpaddr = NULL;
            
            ir_obj->RxReadIdx = 0;
            ir_obj->RxWriteIdx = 0;
        
            ir_obj->RxChannel = ithDmaRequestCh(ir_obj->RxChName, ITH_DMA_CH_PRIO_HIGH_3, NULL, NULL);
            ithDmaReset(ir_obj->RxChannel);
            ir_obj->TxChannel = ithDmaRequestCh(ir_obj->TxChName, ITH_DMA_CH_PRIO_HIGHEST, NULL, NULL);
            ithDmaReset(ir_obj->TxChannel);
        
            ir_obj->RxBuffer = (uint16_t *)itpVmemAlloc(IR_DMA_BUFFER_SIZE);
            ir_obj->TxBuffer = (uint16_t *)itpVmemAlloc(IR_DMA_BUFFER_SIZE);
        
            if(ir_obj->RxBuffer == NULL || ir_obj->TxBuffer == NULL) {
                printf("Alloc IR DMA buffer fail\n");
            }
            else {
                ir_obj->gLLPCtxt = (LLP_CONTEXT *)itpVmemAlloc(sizeof(LLP_CONTEXT) + 32);
                //printf("--- gLLPCtxt addr = 0x%x , sizeof(LLP_CONTEXT) = 0x%x---\n", ir_obj->gLLPCtxt, sizeof(LLP_CONTEXT));
                llpaddr = (LLP_CONTEXT *)(((uint32_t)ir_obj->gLLPCtxt + 0x1F) & ~(0x1F));
                //printf("--- new llpaddr addr = 0x%x\n", llpaddr);
                llpaddr->SrcAddr =  le32_to_cpu(port + ITH_IR_CAP_DATA_REG);
                llpaddr->DstAddr = le32_to_cpu(ir_obj->RxBuffer);
                llpaddr->LLP = le32_to_cpu(llpaddr);
                llpaddr->TotalSize = le32_to_cpu(IR_DMA_TRANSFER_SIZE);
                llpaddr->Control = le32_to_cpu(0x02610000);  //16bit
        
                ithDmaSetSrcAddr(ir_obj->RxChannel, port + ITH_IR_CAP_DATA_REG);
                ithDmaSetDstAddr(ir_obj->RxChannel, (uint32_t)ir_obj->RxBuffer);
                ithDmaSetRequest(ir_obj->RxChannel, ITH_DMA_HW_HANDSHAKE_MODE, ir_obj->RxChannelReq, ITH_DMA_NORMAL_MODE, ITH_DMA_MEM);
        
                ithDmaSetSrcParams(ir_obj->RxChannel, ITH_DMA_WIDTH_16, ITH_DMA_CTRL_FIX, ITH_DMA_MASTER_1);
                ithDmaSetDstParams(ir_obj->RxChannel, ITH_DMA_WIDTH_16, ITH_DMA_CTRL_INC, ITH_DMA_MASTER_0);
                ithDmaSetTxSize(ir_obj->RxChannel, IR_DMA_BUFFER_SIZE);
                ithDmaSetBurst(ir_obj->RxChannel, ITH_DMA_BURST_1);
        
                ithDmaSetLLPAddr(ir_obj->RxChannel, (uint32_t)llpaddr);
                //printf("--- llpaddr:0x%x\n", (uint32_t)llpaddr);
        
                ithDmaStart(ir_obj->RxChannel);
            }
        }

        {
        	int res;
            pthread_t task;
            pthread_attr_t attr;

            printf("Create DMA IR-RX pthread~~\n");

            pthread_attr_init(&attr);
            res = pthread_create(&task, &attr, IrRxDMAHandler, (void *) port);

            if(res)
            {
                printf( "[IR]%s() L#%ld: ERROR, create IrRxDMAHandler thread fail! res=%ld\n", __FUNCTION__, __LINE__, res );
                return -1;
            }
        }

        {
        	int res;
            pthread_t task;
            pthread_attr_t attr;

            printf("Create DMA IR-TX pthread~~\n");

            pthread_attr_init(&attr);
            res = pthread_create(&task, &attr, IrTxDMAHandler, (void *) port);

            if(res)
            {
                printf( "[IR]%s() L#%ld: ERROR, create IrRxDMAHandler() thread fail! res=%ld\n", __FUNCTION__, __LINE__, res );
                return -1;
            }
        }
    }
    else {
        {
        	int res;
    		pthread_t task;
    		pthread_attr_t attr;

    		printf("Create IR-RX pthread~~\n");

    		pthread_attr_init(&attr);
    		res = pthread_create(&task, &attr, IrDefaultHandler, (void *) port);

    		if(res)
    		{
    			printf( "[IR]%s() L#%ld: ERROR, create IrIntrHandler() thread fail! res=%ld\n", res );
    			return -1;
    		}
        }

        {
        	int res;
    		pthread_t task;
    		pthread_attr_t attr;

    		printf("Create IR-TX pthread~~\n");

    		pthread_attr_init(&attr);
    		res = pthread_create(&task, &attr, IrTxDefaultHandler, (void *) port);

    		if(res)
    		{
    			printf( "[IR]%s() L#%ld: ERROR, create IrTxIntrHandler() thread fail! res=%ld\n", res );
    			return -1;
    		}
        }
    }

    // Set Rx Modulation Filter
    {
    	//ithIrRxSetModFilter(ir_obj->port, 0x535, 0xFA0); //20~60k freq.(80M/freq.)
		//ithIrRxSetModFilter(ir_obj->port, 0xCE4, 0x26AC); //20~60k (WCLK/freq.), WCLK=198MHz Minfreq=20KHz Maxfreq=60KHz
		//ithIrRxSetModFilter(ir_obj->port, 0x1, 0x5); //20~60k freq.(80M/PreScale/freq.) for it960, PreScale=798(80M)

		//ithIrRxMode(ir_obj->port, ITH_IR_RX_ModFilter); //Enable modulation filter

		// Enable IR Capture
		ithIrCtrlEnable(ir_obj->port, ITH_IR_EN);
    }

    // Set Tx Modulation Freq.
    {
    	//FixMe: input value SHOULD be changed by BusCLK(WCLK)
		//ithIrTxSetModFreq(ir_obj->port, 0x839); //(0x1e);
		ithIrTxSetModFreq(ir_obj->port, 0x13D5);	//39kHz (WCLK/SampleRate), WCLK=198MHz SampleRate=39KHz

		// Enable IR Capture
		ithIrTxCtrlEnable(ir_obj->port, ITH_IR_EN);
    }
}

static int IrRead(int file, char *ptr, int len, void* info)
{
	ITHIrPort port = (ITHIrPort) info;
    ITPKeypadEvent* ev = (ITPKeypadEvent*)ptr;
	IR_OBJ *ir_obj = NULL;
	ITPKeypadEvent *LastEvent = NULL;

	if(port == ITH_IR0) {
		ir_obj = &IrObj[0];
	}
	else if(port == ITH_IR1) {
		ir_obj = &IrObj[1];
	}
	else if(port == ITH_IR2) {
		ir_obj = &IrObj[2];
	}
	else if(port == ITH_IR3) {
		ir_obj = &IrObj[3];
	}
	else {
		ir_obj = &IrObj[0];
	}

	LastEvent = &ir_obj->irLastEvent;

    if (xQueueReceive(ir_obj->RxQueue, &ev->code, 0))
    {
        gettimeofday(&ev->time, NULL);
        ev->flags = ITP_KEYPAD_DOWN;

        if (LastEvent->code == ev->code)
            ev->flags |= ITP_KEYPAD_REPEAT;

        LastEvent->code = ev->code;
        LastEvent->time.tv_sec = ev->time.tv_sec;
        LastEvent->time.tv_usec = ev->time.tv_usec;

        return sizeof (ITPKeypadEvent);
    }
    else if (LastEvent->code != -1)
    {
        struct timeval now;

        gettimeofday(&now, NULL);
        if (itpTimevalDiff(&LastEvent->time, &now) >= CFG_IR_PRESS_INTERVAL)
        {
            ev->code            = LastEvent->code;
            ev->time.tv_sec     = now.tv_sec;
            ev->time.tv_usec    = now.tv_usec;
            ev->flags           = ITP_KEYPAD_UP;
            LastEvent->code    = -1;
            return sizeof (ITPKeypadEvent);
        }
    }

    return 0;
}

static int IrWrite(int file, char *ptr, int len, void* info)
{
	ITHIrPort port = (ITHIrPort) info;
    ITPKeypadEvent* ev = (ITPKeypadEvent*)ptr;
	IR_OBJ *ir_obj = NULL;

	if(port == ITH_IR0) {
		ir_obj = &IrObj[0];
	}
	else if(port == ITH_IR1) {
		ir_obj = &IrObj[1];
	}
	else if(port == ITH_IR2) {
		ir_obj = &IrObj[2];
	}
	else if(port == ITH_IR3) {
		ir_obj = &IrObj[3];
	}
	else {
		ir_obj = &IrObj[0];
	}
	
    printf("irTx: fd=%x, ptr=%x, ev->code=%x, len=%d, info=%x\n",file,ptr,ev->code,len,info);

    //get event
    if(ev->code)
    {
        if( IrCheckCodeMapping(ev->code) )
        {
            printf("sendQue2IrTx(TxQue=%x,code=%x)\n", ir_obj->TxQueue, ev->code);
            xQueueSend(ir_obj->TxQueue, &ev->code, 0);
            
            if(ir_obj->mode == ITH_INTR_MODE) {
			    // Enable IR-TX Interrupt
			    ithIrTxCtrlEnable(port, ITH_IR_INT);
            }
        }
        else
        {
            return 0;//TODO: maybe return non "0" value??
        }
    }

    return 0;
}

static int IrIoctl(int file, unsigned long request, void* ptr, void* info)
{
	ITHIrPort port = (ITHIrPort) info;
	
    switch (request)
    {
    case ITP_IOCTL_INIT:
        IrInit(port);
        break;

    default:
        errno = (ITP_DEVICE_IR << ITP_DEVICE_ERRNO_BIT) | __LINE__;
        return -1;
    }
    return 0;
}

const ITPDevice itpDeviceIr =
{
    ":ir",
    itpOpenDefault,
    itpCloseDefault,
    IrRead,
    IrWrite,
    itpLseekDefault,
    IrIoctl,
    (void *) ITH_IR0
};
	
const ITPDevice itpDeviceIr1 =
{
	":ir1",
	itpOpenDefault,
	itpCloseDefault,
	IrRead,
	IrWrite,
	itpLseekDefault,
	IrIoctl,
	(void *) ITH_IR1
};
	
const ITPDevice itpDeviceIr2 =
{
	":ir2",
	itpOpenDefault,
	itpCloseDefault,
	IrRead,
	IrWrite,
	itpLseekDefault,
	IrIoctl,
	(void *) ITH_IR2
};
	
const ITPDevice itpDeviceIr3 =
{
	":ir3",
	itpOpenDefault,
	itpCloseDefault,
	IrRead,
	IrWrite,
	itpLseekDefault,
	IrIoctl,
	(void *) ITH_IR3
};

