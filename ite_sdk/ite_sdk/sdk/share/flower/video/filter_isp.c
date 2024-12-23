#include <stdio.h>
#include "flower/flower.h"
#include "flower/fliter_priv_def.h"
#include "isp/it970/mmp_isp.h"

//=============================================================================
//                              Constant Definition
//=============================================================================
#define ISPBUFFERCOUNT 3
#define ISPQUEUESIZE   20

ISP_DEVICE        gIspDev0;
ISPOutPutStream*  gOutputStream	= NULL;
static ISPQUEUEINFO      gISPQueueINFO = {0};
static unsigned short    gInputMode    = false; // false: onfly mode , ture: memory mode
//=============================================================================
//                              Private Function Declaration
//=============================================================================
static void _ISP0ISR(void *arg)
{
    BaseType_t gHigherPriorityTaskWoken = (BaseType_t) 0;
	IteFilter *fptr                  = (IteFilter *) arg;
	IteQueueblk               send_blk0;
	
	gISPQueueINFO.FinishBufferIdx   = gISPQueueINFO.CurrentBufferIdx;
    gISPQueueINFO.CurrentBufferIdx++;
    gISPQueueINFO.CurrentBufferIdx %= gOutputStream->BufferCount;
	send_blk0.private1 = (void*)&gISPQueueINFO.FinishBufferIdx;
	ite_queue_put_fromISR(fptr->output[0].Qhandle, &send_blk0, gHigherPriorityTaskWoken);
	if(fptr->output[1].Qhandle)
	{
		ite_queue_put_fromISR(fptr->output[1].Qhandle, &send_blk0, gHigherPriorityTaskWoken);
	}
    //ithPrintf("ISP0 done (%d)\n",gISPQueueINFO.FinishBufferIdx);
    mmpIspClearInterrupt(gIspDev0);
    portYIELD_FROM_ISR(gHigherPriorityTaskWoken);
}

static void f_isp_init(IteFilter *f)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
    /* ISP init core 0 */
    mmpIspInitialize(&gIspDev0, MMP_ISP_CORE_0);
	
    gISPQueueINFO.CurrentBufferIdx = 0;
    gISPQueueINFO.FinishBufferIdx  = -1;	

}

static void f_isp_uninit(IteFilter *f)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);

	//wait capture terminate
	usleep(20*1000);

#if 1//reset isp all engine 
	ithIspDisableClock();

	ithSetRegBitA(ITH_HOST_BASE + ITH_ISP_CLK_REG, ITH_ISPQ_RST_BIT);
	ithSetRegBitA(ITH_HOST_BASE + ITH_ISP_CLK_REG, ITH_ISP0_REG_RST_BIT);
	ithSetRegBitA(ITH_HOST_BASE + ITH_ISP_CLK_REG, ITH_ISP0_RST_BIT);
	ithSetRegBitA(ITH_HOST_BASE + ITH_ISP_CLK_REG, ITH_ISP_ALLREG_RST_BIT);
	ithSetRegBitA(ITH_HOST_BASE + ITH_ISP_CLK_REG, ITH_ISP1_REG_RST_BIT);
	ithSetRegBitA(ITH_HOST_BASE + ITH_ISP_CLK_REG, ITH_ISP1_RST_BIT);

	usleep(200);
	ithClearRegBitA(ITH_HOST_BASE + ITH_ISP_CLK_REG, ITH_ISPQ_RST_BIT);
	ithClearRegBitA(ITH_HOST_BASE + ITH_ISP_CLK_REG, ITH_ISP0_REG_RST_BIT);
	ithClearRegBitA(ITH_HOST_BASE + ITH_ISP_CLK_REG, ITH_ISP0_RST_BIT);
	ithClearRegBitA(ITH_HOST_BASE + ITH_ISP_CLK_REG, ITH_ISP_ALLREG_RST_BIT);
	ithClearRegBitA(ITH_HOST_BASE + ITH_ISP_CLK_REG, ITH_ISP1_REG_RST_BIT);
	ithClearRegBitA(ITH_HOST_BASE + ITH_ISP_CLK_REG, ITH_ISP1_RST_BIT);	

	ithIspEnableClock();
#endif

    mmpIspTerminate(gIspDev0);		
}

static void f_isp_process(IteFilter *f)
{
	MMP_ISP_SHARE             ISPSHARE      = {0};
    IteQueueblk               rec_blk0, send_blk0;
	uint8_t                   writeindex    = 0;
	IteSensorStream*          sensorinfo    = NULL;
    MMP_ISP_CORE_INFO         ISPCOREINFO   = {0};
    MMP_ISP_WRITE_BUFFER_INFO ISPWBINFO     = {0};
    MMP_ISP_OUTPUT_INFO       ISPOUTINFO    = {0};	
	int i = 0;
	DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
	
    ISPCOREINFO.EnPreview   = false;
    ISPCOREINFO.PreScaleSel = MMP_ISP_PRESCALE_NORMAL;
    mmpIspSetCore(gIspDev0, &ISPCOREINFO);
    mmpIspSetMode(gIspDev0, MMP_ISP_MODE_PLAY_VIDEO);
    mmpIspSetOutputFormat(gIspDev0, MMP_ISP_OUT_NV12);

	if(gInputMode == false)
	{
		DEBUG_PRINT("[%s] ISP ONFLY MODE\n", __FUNCTION__);
    	mmpIspEnableCAPOnFlyMode(gIspDev0);
		for (i = 0; i < gOutputStream->BufferCount; i++)
		{
			  switch (i)
			  {
				  case 0:
					  ISPOUTINFO.addrY = gOutputStream->DataAddrY[i];
					  ISPOUTINFO.addrU = gOutputStream->DataAddrUV[i];
					  break;
			
				  case 1:
					  ISPWBINFO.addrYRGB_1 = gOutputStream->DataAddrY[i];
					  ISPWBINFO.addrU_1    = gOutputStream->DataAddrUV[i];
					  break;
			
				  case 2:
					  ISPWBINFO.addrYRGB_2 = gOutputStream->DataAddrY[i];
					  ISPWBINFO.addrU_2    = gOutputStream->DataAddrUV[i];
					  break;
			
				  case 3:
					  ISPWBINFO.addrYRGB_3 = gOutputStream->DataAddrY[i];
					  ISPWBINFO.addrU_3    = gOutputStream->DataAddrUV[i];
					  break;
			
				  case 4:
					  ISPWBINFO.addrYRGB_4 = gOutputStream->DataAddrY[i];
					  ISPWBINFO.addrU_4    = gOutputStream->DataAddrUV[i];
					  break;
			
				  default:
					  break;
			  }
		}
	    mmpIspSetWriteBufferInfo(gIspDev0, &ISPWBINFO);
	    mmpIspSetWriteBufferNum(gIspDev0, gOutputStream->BufferCount - 1);

	    ISPOUTINFO.width   = gOutputStream->Width;
	    ISPOUTINFO.height  = gOutputStream->Height;
	    ISPOUTINFO.pitchY  = gOutputStream->PitchY;
	    ISPOUTINFO.pitchUv = gOutputStream->PitchUV;
	    ISPOUTINFO.format  = MMP_ISP_OUT_NV12;
		
	    mmpIspSetOutputWindow(gIspDev0, &ISPOUTINFO);
	    mmpIspSetVideoWindow(gIspDev0, 0, 0, ISPOUTINFO.width, ISPOUTINFO.height);
	}
    /* ENABLE ISP ISR */
    ithIntrDisableIrq(ITH_INTR_ISP_CORE0);
    ithIntrRegisterHandlerIrq(ITH_INTR_ISP_CORE0, _ISP0ISR, (void*)f);
    ithIntrSetTriggerModeIrq(ITH_INTR_ISP_CORE0, ITH_INTR_EDGE);
    ithIntrSetTriggerLevelIrq(ITH_INTR_ISP_CORE0, ITH_INTR_HIGH_RISING);
    ithIntrEnableIrq(ITH_INTR_ISP_CORE0);

	mmpIspEnableInterrupt(gIspDev0);
	
    while (f->run)
    {
		
		if(ite_queue_get(f->input[0].Qhandle, &rec_blk0) == 0) {
			sensorinfo =(IteSensorStream*) rec_blk0.private1;
			if(gInputMode == false)
			{
				printf("sensor info w = %d, h=%d\n",sensorinfo->Width, sensorinfo->Height);
				/* ISP INPUT SETTING*/
				ISPSHARE.width	 = sensorinfo->Width;
				ISPSHARE.height  = sensorinfo->Height;
				ISPSHARE.pitchY  = sensorinfo->PitchY;
				ISPSHARE.pitchUv = sensorinfo->PitchUV;
				ISPSHARE.format  = MMP_ISP_IN_YUV422;
				mmpIspPlayImageProcess(gIspDev0, &ISPSHARE);
				free(sensorinfo);
			}
			else
			{
                if (sensorinfo->Interlanced)
                {
                    mmpIspEnable(gIspDev0, MMP_ISP_DEINTERLACE);
					mmpIspSetDeinterlaceType(gIspDev0, MMP_ISP_2D_DEINTERLACE);		
                }
                else
                    mmpIspDisable(gIspDev0, MMP_ISP_DEINTERLACE);

				ISPOUTINFO.width   = gOutputStream->Width;
				ISPOUTINFO.height  = gOutputStream->Height;
				ISPOUTINFO.pitchY  = gOutputStream->PitchY;
				ISPOUTINFO.pitchUv = gOutputStream->PitchUV;
				ISPOUTINFO.addrY   = gOutputStream->DataAddrY[gISPQueueINFO.CurrentBufferIdx];
				ISPOUTINFO.addrU   = gOutputStream->DataAddrUV[gISPQueueINFO.CurrentBufferIdx];
				ISPOUTINFO.addrV   = gOutputStream->DataAddrUV[gISPQueueINFO.CurrentBufferIdx];
				ISPOUTINFO.format  = MMP_ISP_OUT_NV12;
                mmpIspSetOutputWindow(gIspDev0, &ISPOUTINFO);
                mmpIspSetVideoWindow(gIspDev0, 0, 0, ISPOUTINFO.width, ISPOUTINFO.height);
				
				ISPSHARE.width	 = sensorinfo->Width;
				ISPSHARE.height  = sensorinfo->Height;
				ISPSHARE.pitchY  = sensorinfo->PitchY;
				ISPSHARE.pitchUv = sensorinfo->PitchUV;
                ISPSHARE.addrY   = sensorinfo->DataAddrY;
                ISPSHARE.addrU   = sensorinfo->DataAddrU;
                ISPSHARE.addrV   = sensorinfo->DataAddrV;
				ISPSHARE.format  = MMP_ISP_IN_NV12;
				mmpIspPlayImageProcess(gIspDev0, &ISPSHARE);	
				free(sensorinfo);
				
			}
		}

		usleep(16*1000);
    }

}

static void f_isp_setoutput(IteFilter *f, void *arg)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
	gOutputStream = (ISPOutPutStream*) arg;
}

static void f_isp_setinputmode(IteFilter *f, void *arg)
{
    DEBUG_PRINT("[%s] Filter(%d)\n", __FUNCTION__, f->filterDes.id);
	gInputMode = (unsigned short*) arg;
}



static IteMethodDes Filter_ISP_methods[] = {
    {ITE_FILTER_ISP_SETOUTPUT , f_isp_setoutput },
    {ITE_FILTER_ISP_SETINPUTMODE, f_isp_setinputmode}
};

IteFilterDes        FilterISPOnfly = {
    ITE_FILTER_ISPONFLY_ID,
    f_isp_init,
    f_isp_uninit,
    f_isp_process,
    Filter_ISP_methods
};
