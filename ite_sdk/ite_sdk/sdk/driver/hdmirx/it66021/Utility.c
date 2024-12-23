///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Utility.c>
//   @author Jau-Chih.Tseng@ite.com.tw
//   @date   2019/10/31
//   @fileversion: ITE_HDMI1.4_RXSAMPLE_1.27
//******************************************/
#include "hdmirx/it66021/config.h"
//#include "Mcu.h"
#include "hdmirx/it66021/IO.h"
#include "hdmirx/it66021/Utility.h"
#include "ite/itp.h"


#if 0
idata USHORT ucTickCount=0;
idata USHORT loopTicCount = 0;
idata USHORT prevTickCount;
idata USHORT MsdelayCnt =0;

void init_printf(void)
{
    SCON = 0x52;
    T2CON = 0x34;
    TR2 = FALSE;
#ifdef MEGAWIN82516
    TL2 = 0xFD;	//BAUD_SETTING;
    TH2 = 0xFF;	//BAUD_SETTING>>8;
    RCAP2L = 0xFD;	//BAUD_SETTING;
    RCAP2H = 0xFF;	//BAUD_SETTING>>8;
#else
    TL2 = BAUD_SETTING;
    TH2 = BAUD_SETTING>>8;
    RCAP2L = BAUD_SETTING;
    RCAP2H = BAUD_SETTING>>8;
#endif
    TR2 = TRUE;
    TI = 1;
    RI = 0;
    //EA=FALSE;
}

void initialTimer0(void)
{
    TR0=0; // temporarily stop timer 0

    //TMOD &= 0x0F;	// Timer 1, Mode 0, 13 bit
    //TMOD |= 0x10;	// Timer 1, Mode 1, 16 bit

    TMOD &= 0xF0;	// Timer 0, Mode 0, 13 bit
    TMOD |= 0x01;	// Timer 0, Mode 1, 16 bit

    TH0=Tick1ms / 256;
    TL0=Tick1ms % 256;

    TR0	= 1; 	// restart the timer
    ET0	= 1;   	// Enable Timer Interrupt 0

}

void initialTimer1(void)
{
    TR1=0; // temporarily stop timer 1

    //TMOD &= 0x0F;	 //Timer 1, Mode 0, 13 bit
    //TMOD |= 0x11;	 //Timer 1, Mode 1, 16 bit

    TMOD = 0x11;	 //Timer 1, Mode 1, 16 bit

    TH1= Tick1ms / 256;
    TL1= Tick1ms % 256;

    ucTickCount=0;

    TR1	= 1; 	// restart the timer
    ET1	= 1;   	// Enable Timer Interrupt 0
    EA=1;
}

void system_tick(void) interrupt 3 {

    TR1=0; // temporarily stop timer 0

    TH1=Tick1ms / 256;
    TL1=Tick1ms % 256;

    ucTickCount++;

    TR1=1; // restart the timer
}


USHORT getloopTicCount()
{
    idata USHORT loopms;

    if(loopTicCount>ucTickCount)
    {
        loopms =  (0xffff-(loopTicCount-ucTickCount));
    }
    else
    {
        loopms =  (ucTickCount-loopTicCount);
    }
    loopTicCount = ucTickCount;
//    MHLRX_DEBUG_PRINTF(" loop ms  = %u\n",loopms));
    return  loopms;

}
#endif

unsigned int prevTickCount = 0;

#if 0
unsigned short CalTimer(unsigned short SetupCnt)
{
    
    if(SetupCnt>ucTickCount)
    {
        return (0xffff-(SetupCnt-ucTickCount));
    }
    else
    {
        return (ucTickCount-SetupCnt);
    }
}
#endif
unsigned int CalTimer(void)
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) != 0)
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    else
        return 0;
    
}

BOOL TimeOutCheck(unsigned short timer, unsigned short x)
{
    if(CalTimerDuration((unsigned int)timer)>=x)
    {
        return TRUE;
    }
    return FALSE;
}


BOOL IsTimeOut(unsigned short x)
{
    #if 0
    if(CalTimer(prevTickCount) >= x )
    {
        prevTickCount = ucTickCount;
        return TRUE;
    }
    return FALSE;
    #else
    if(CalTimerDuration(prevTickCount) >= x )
    {
        prevTickCount = CalTimer();
        return TRUE;
    }
    return FALSE;   
    #endif
}

unsigned int GetCurrentVirtualTime()
{
    #if 0
    return ucTickCount;
    #else
    return CalTimer();
    #endif
}


void delay1ms(unsigned short ms)
{
    #if 0
    USHORT ucStartTickCount,diff;



    ucStartTickCount=ucTickCount;
        do
        {
            if(ucTickCount<ucStartTickCount)
                {
                    diff=0xffff-(ucStartTickCount-ucTickCount);
                }
            else
                {
                    diff=ucTickCount-ucStartTickCount;
                }
        }while(diff<ms);
    #else
    usleep(ms*1000);
    #endif
}

