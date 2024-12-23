#include <stdarg.h>
#include <string.h>
#include "risc2/swIRRx/swIRRx.h"
#include "risc2/risc2_utility.h"

#define ENDIAN_SWAP16(x) \
        (((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8))

#define ENDIAN_SWAP32(x) \
        (((x & 0x000000FF) << 24) | \
        ((x & 0x0000FF00) <<  8) | \
        ((x & 0x00FF0000) >>  8) | \
        ((x & 0xFF000000) >> 24))

#define LOWTHRESHOLD	18900
#define ST_HIGH			1
#define ST_LOW			0
#define ST_RESET		0x20
#define ST_START		0x30
#define ST_GETDATA		0x40
#define ST_SWUART_INIT  0x50
#define ST_CPDATATOARM	0x55
#define QSIZE			16
#define NUM				0
#define CARR			1
#define TIMEGAP			2

#define DEBUG_REG0       0xD8000000+0x0200
#define DEBUG_REG1       0xD8000000+0x0204
#define DEBUG_REG2       0xD8000000+0x0208

uint32_t systickToMicroConstant = 0;
#define ticker2us(time) (time/systickToMicroConstant)
#define CAL_CARRIER(ticksum, cnt, sysclk) (1000/(((ticksum/((cnt<<1)-1))<<1)/sysclk))
#define CAL_IRDATATIME(ticksum, sysclk) ticker2us(ticksum)

uint8_t* gpCmdExchangeAddr = NULL;
extern unsigned char risc2_start_addr[];

uint32_t gRiscCpuClock = 0;
uint32_t gRxGpio = 0;

#if 0
static __inline void RegWrite(unsigned long addr, unsigned long data) {
    *(volatile unsigned long *) (addr) = data;
}

static __inline unsigned long RegRead(unsigned long addr) {
    return *(volatile unsigned long *) (addr);
}

static __inline void RegWriteMask(unsigned long addr, unsigned long data, unsigned long mask) {
    RegWrite(addr, ((RegRead(addr) & ~mask) | (data & mask)));
}
#endif

static void risc2SwIRProcessInitCmd(void)
{
    SW_IR_INIT_DATA* ptInitData = (SW_IR_INIT_DATA*) gpCmdExchangeAddr;
    
    gRiscCpuClock =  ENDIAN_SWAP32(ptInitData->cpuClock);
    gRxGpio = ENDIAN_SWAP32(ptInitData->rxGpio);
    systickToMicroConstant = gRiscCpuClock/(1000*1000);

    //Init GPIO
    setGpioDir(gRxGpio, 1); //input mode
    setGpioMode(gRxGpio, 0);    
}


/**************************************************************/
/* NOTICE: This SW IR design flow is only for iptv_rx project */
/**************************************************************/
int main(int argc, char **argv)
{
    int inputCmd = 0;
    struct _risc2_header *ptRisc2Header = (struct _risc2_header*) risc2_start_addr;
    gpCmdExchangeAddr = (uint8_t*) ENDIAN_SWAP32((uint32_t)ptRisc2Header->cmd_exchange_addr);
    SW_IR_READ_DATA *ptIRBuffer = (SW_IR_READ_DATA *)gpCmdExchangeAddr;
    uint32_t IRSig = 0x0000;
    uint16_t Data[IRRAWDATALEN] = {0};
    uint8_t State = ST_START;
    uint32_t FirstLTick = 0;
    uint32_t FirstHTick = 0;
    uint32_t Lefthalf = 0;
    uint32_t Righthalf = 0;
    uint32_t HCount = 0;
    uint32_t Tick = 0;
    uint32_t Diff = 0;
    uint32_t T1 = 0, Timegap = 0, Modtick = 0;
    uint8_t Ready = 0, TickHReset = 0;
    uint16_t Ircntindex = 0, NeedToResetTimer=1;
    uint16_t Carrier = 0x00;   

    while(1)
    {
        inputCmd = RISC2_COMMAND_REG_READ(REQUEST_CMD_REG);
        if (inputCmd && RISC2_COMMAND_REG_READ(RESPONSE_CMD_REG) == 0)
        {
            //need to read data from meomery instead of cache
            dc_invalidate();
            switch(inputCmd)
            {
                case INIT_CMD_ID:
                    risc2SwIRProcessInitCmd();
                    break;
                default:
                    break;
            }
            RISC2_COMMAND_REG_WRITE(RESPONSE_CMD_REG, (uint16_t) inputCmd);
        }

        //Monitor IR signal
        {
            switch(State) {
                case ST_START:
                    startTimer(0);
            
                    memset(ptIRBuffer->pBuffer, 0, IRRAWDATALEN*QSIZE);
                    ptIRBuffer->writeIdx = 0;

                    State = ST_GETDATA;
                    break;
                    
                case ST_RESET:
                    HCount = 0;
                    FirstHTick = 0;
                    FirstLTick = 0;
                    Lefthalf = 0;
                    Righthalf = 0;
                    Ready = 0;
                    
                    State = ST_GETDATA;
                    break;
                    
                case ST_HIGH:
                    Tick = getTimer(0);
                    if(Ready == 1)
                    {
                    	Ready = 0;
                    	if(FirstHTick == 0)
                    	{
                    		++HCount;
                    		FirstHTick = Tick;
                    	}
                    }
                    if(FirstHTick != 0)
                    {
                    	Diff = Tick - FirstHTick;
                    	if(Diff >= 50*LOWTHRESHOLD)
                    	{
                    		NeedToResetTimer = 1;
                    		State = ST_CPDATATOARM;
                    		break;
                    	}
                    	else if(Diff > LOWTHRESHOLD && (Lefthalf == 0))
                    	{
                    		Lefthalf = FirstHTick - FirstLTick;
                    		if(Carrier == 0)
                    		{
                    			Carrier = CAL_CARRIER(Lefthalf, HCount, systickToMicroConstant);
                    		}
                    		Lefthalf += Modtick;
                            //Lefthalf += 1890;  //mibox
                    		Data[Ircntindex]  = (CAL_IRDATATIME(Lefthalf, systickToMicroConstant)/10);
                    		Modtick = (Lefthalf - (Data[Ircntindex] * systickToMicroConstant * 10)); 
                    		
                    		Ircntindex =  (Ircntindex + 1) & 0x7F;
                    	}
                    }

                    State = ST_GETDATA;
                    break;
                    
                case ST_LOW:
                    Tick = getTimer(0);
                    if(Ready == 0)
                    {	
                    	Ready = 1;
                    	if(Lefthalf == 0)
                    	{
                    		FirstHTick = 0; // ready to evaluate next high start
                    		if(FirstLTick == 0) 
                    		{
                    			FirstLTick = Tick; // record first Low time
                    		}                    		
                    	}
                    	else
                    	{                    		
                    		if(Righthalf == 0)
                    		{
                    			Righthalf = Tick - FirstHTick;
                    			Righthalf += Modtick;
                                //Righthalf += 1890; //mibox
                    			Data[Ircntindex]  = (CAL_IRDATATIME(Righthalf, systickToMicroConstant)/10);
                    			Modtick = (Righthalf - (Data[Ircntindex] * systickToMicroConstant * 10));
                    			Ircntindex =  (Ircntindex + 1) & 0x7F;
                    			State = ST_LOW;
                    			HCount = 0;FirstHTick = 0;FirstLTick = 0;
                    			Lefthalf = 0;Righthalf = 0;Ready = 0;
                    			break;
                    		}
                    	}
                    }
                    State = ST_GETDATA;
                    break;
                    
                case ST_GETDATA:
                    IRSig = getGpioValue(gRxGpio, 1);
                    if(IRSig)
                    {
                    	State = ST_HIGH;
                    	Tick = getTimer(0);
                    	if(TickHReset == 0)
                    	{
                    		TickHReset = Tick;
                    	}
                    	else if((Tick - TickHReset)>= 1500*LOWTHRESHOLD )
                    	{
                    		resetTimer(0);
                    		Timegap = T1 = 0;
                    		TickHReset = 0;
                    	}           	
                    }
                    else
                    {
                    	State = ST_LOW;
                    	if(Lefthalf == 0 && FirstLTick == 0)
                    	{
                    		
                    		
                    		if(NeedToResetTimer == 1)
                    		{
                    			if(T1 != 0)
                    			{
                    				Diff = getTimer(0) - T1;
                    				Timegap = (ticker2us(Diff)/1000);
                    			}
                    			resetTimer(0);
                    			T1 = getTimer(0);
                    			NeedToResetTimer = 0;
                    		}
                    	}
                    	
                    }
                    break;
                    
                case ST_CPDATATOARM:
                    ptIRBuffer->pBuffer[ptIRBuffer->writeIdx*IRRAWDATALEN + CARR] = Carrier;
                    ptIRBuffer->pBuffer[ptIRBuffer->writeIdx*IRRAWDATALEN + TIMEGAP] = Timegap;
                    memcpy((void *)&ptIRBuffer->pBuffer[ptIRBuffer->writeIdx*IRRAWDATALEN+(NUM+CARR+TIMEGAP)], Data, Ircntindex*sizeof(uint16_t));
                    ptIRBuffer->pBuffer[ptIRBuffer->writeIdx*IRRAWDATALEN + NUM] = Ircntindex;

                    ++(ptIRBuffer->writeIdx);
                    ptIRBuffer->writeIdx &= (QSIZE-1);

                    memset(Data, 0, Ircntindex);

                    Ircntindex = 0;
                    Carrier = 0;
                    Modtick = 0;
                    State = ST_RESET;
                    break;
                    
                default:
                    State = ST_GETDATA;
                    break;
            }
        }
    }
}
