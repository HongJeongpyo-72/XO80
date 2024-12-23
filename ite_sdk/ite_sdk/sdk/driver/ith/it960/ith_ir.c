/*
 * Copyright (c) 2011 ITE Tech. Inc. All Rights Reserved.
 */
/** @file
 * HAL IR functions.
 *
 * @author Jim Tan
 * @version 1.0
 */
#include "ith_cfg.h"

static unsigned int irWidth;

void ResetIR(ITHIrPort port)
{
	if(port == ITH_IR0) {
		ithSetRegBitA(ITH_HOST_BASE+ITH_APB_CLK2_REG, ITH_IR0_RESET_BIT);
	}
	else if(port == ITH_IR1) {
		ithSetRegBitA(ITH_HOST_BASE+ITH_APB_CLK2_REG, ITH_IR1_RESET_BIT);
	}
	else if(port == ITH_IR2) {
		ithSetRegBitA(ITH_HOST_BASE+ITH_APB_CLK2_REG, ITH_IR2_RESET_BIT);
	}
	else if(port == ITH_IR3) {
		ithSetRegBitA(ITH_HOST_BASE+ITH_APB_CLK2_REG, ITH_IR3_RESET_BIT);
	}
}

static inline int MULSHIFT(int x, int y, int shift)
{
    int64_t xext, yext;
    xext = (int64_t)x;
    yext = (int64_t)y;
    xext = ((xext * yext) >> shift);
    printf("	### xy=%x ###\n",(int)xext);
    return (int)xext;
}

static void ithIrSetTxGpio(ITHIrPort port, unsigned int pin)
{
	ithWriteRegMaskA(port + ITH_IR_GPIO_SEL_REG, pin << 8, 0x7F << 8);
	ithSetRegBitA(port + ITH_IR_GPIO_SEL_REG, 15);  //Enable TX GPIO Select
}

void ithIrInit(ITHIrPort port, unsigned int pin, unsigned long extClk, unsigned long sample, unsigned int precision)
{
    unsigned int busClk, prescale;
    unsigned long sampDuration = sample * (1 << precision) / 1000; // sample duration in milliseconds
        
    if (extClk == 0)
    {
        busClk = ithGetBusClock();
        printf("busclk=%d\n",busClk);
    }
    else
    {
        unsigned int outdiv = (ithReadRegH(ITH_APB_CLK1_REG) & ITH_WCLK_RATIO_MASK) + 4;
        busClk = (extClk / outdiv) << 2;
    }
    printf(" ### IR RX Pin=%d #### \n",pin);
    ithGpioSetMode(pin, ITH_GPIO_MODE0);
    ithIrSetGpio(port, pin);
   	ithIrSetCaptureMode(port, ITH_IR_RISING);
   	//ithIrSetCaptureMode(port, ITH_IR_BOTH);  //Test Tx->Rx Local Case
    ithIrCtrlEnable(port, ITH_IR_DEBOUNCE);
    ithIrCtrlEnable(port, ITH_IR_SIGINVESE);
    ithIrCtrlEnable(port, ITH_IR_TMRST);
    ithIrCtrlEnable(port, ITH_IR_WARP);
    ithIrClear(port);

    prescale = MULSHIFT(sampDuration, busClk, precision) / 1000 - 1;
    printf("set IR prescale=%d,sampDuration=%d,busClk=%d,precision=%d\n",prescale,sampDuration,busClk,precision);
    
    ithWriteRegA(port + ITH_IR_CAP_PRESCALE_REG, prescale);
    //while(1);
    irWidth = (ithReadRegA(port + ITH_IR_HWCFG_REG) & ITH_IR_WIDTH_MASK) >> ITH_IR_WIDTH_BIT;
    printf("irWidth=%d\n",irWidth);
}

void ithIrTxInit(ITHIrPort port, unsigned int pin, unsigned long extClk, unsigned long sample, unsigned int precision)
{
    unsigned int busClk, prescale;
    unsigned long sampDuration = sample * (1 << precision) / 1000; // sample duration in milliseconds
        
    if (extClk == 0)
    {
        busClk = ithGetBusClock();
        printf("busclk=%d\n",busClk);
    }
    else
    {
        unsigned int outdiv = (ithReadRegH(ITH_APB_CLK1_REG) & ITH_WCLK_RATIO_MASK) + 4;
        busClk = (extClk / outdiv) << 2;
    }

    printf(" ### IR Tx Pin=%d #### \n",pin);

	//IT960 can use all pin
	ithIrSetTxGpio(port, pin);

    //ithIrTxSetCaptureMode(port, ITH_IRTX_CFG_LENGTH);
    //ithIrTxCtrlEnable(port, ITH_IR_TX_SIGINVESE);
    ithIrTxClear(port);
    
    //ithWriteRegMaskA(port + ITH_IRTX_CAP_CTRL_REG, 1<<1, 1<<1 );	//enable TX loop-back RX mode
		
    //while(1);
    //set IR-TX clock
    prescale = MULSHIFT(sampDuration, busClk, precision) / 1000 - 1;
    printf("set IR prescale=%d,sampDuration=%d,busClk=%d,precision=%d\n",prescale,sampDuration,busClk,precision);
    
    ithWriteRegA(port + ITH_IRTX_CAP_PRESCALE_REG, prescale);
    
    irWidth = (ithReadRegA(port + ITH_IR_HWCFG_REG) & ITH_IR_WIDTH_MASK) >> ITH_IR_WIDTH_BIT;
    printf("irTxWidth=%d\n",irWidth);
}

int ithIrProbe(ITHIrPort port)
{
    uint32_t status = ithReadRegA(port + ITH_IR_CAP_STATUS_REG);
    
    if (status & (0x1 << ITH_IR_DATAREADY_BIT))
        return ithReadRegA(port + ITH_IR_CAP_DATA_REG) & ((0x1 << irWidth) - 1);
    else
    {     
        if(status & (0x1 << ITH_IR_OE_BIT))
            LOG_WARN "IR overrun error: 0x%X\n", status LOG_END

        return -1;
    }
}

void ithClkSampleInit(ITHIrPort port, unsigned int preScale, unsigned int pin)
{
	//Reset
	ResetIR(port);

	//Set preScale
	ithWriteRegA(port + ITH_IRTX_CAP_PRESCALE_REG, preScale);

	//Set Gpio
	ithIrSetGpio(port, pin);
}
