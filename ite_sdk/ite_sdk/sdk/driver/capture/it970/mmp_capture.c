#include <string.h> 
#include "capture_config.h"
#include "capture_hw.h"
#include "capture.h"
#include "mmp_capture.h"


//=============================================================================
//                Constant Definition
//=============================================================================
#define CAPCONFIGNUM 64
#define CEA_TIMING_RANG 2
#define VESA_HRANG 0
#define VESA_VRANG 2
#define VESA_WRANG 4

//=============================================================================
//                Macro Definition
//=============================================================================

//=============================================================================
//                Structure Definition
//=============================================================================

//=============================================================================
//                Global Data Definition
//=============================================================================
MMP_UINT32 CAPConfigTable[] =
{
#include "defcap_config.h"
};

MMP_UINT16        gCapinitNum;                                    //record how many device opened
pthread_mutex_t   gCapnum_mutex = PTHREAD_MUTEX_INITIALIZER;      //for gCapinitNum mutex protect
MMP_UINT32*       gCap0Config = CAPConfigTable;                   //cap0 config
MMP_UINT32*       gCap1Config = CAPConfigTable + CAPCONFIGNUM;    //cap1 config


//=============================================================================
//                Private Function Definition
//=============================================================================
MMP_INLINE MMP_BOOL _CHECK_RANGE(MMP_UINT32 IN, MMP_UINT32 STD, MMP_UINT32 range)
{
    if(IN > (STD + range) || IN < (STD - range))
        return MMP_FALSE; 
    else
        return MMP_TRUE;
}

MMP_CAP_INPUT_INFO _CHECK_VESA_TIMING(MMP_UINT32 htotal, MMP_UINT32 width)
{
    MMP_CAP_INPUT_INFO info;
    if (_CHECK_RANGE(htotal,800, VESA_HRANG))
    {
        info = MMP_CAP_INPUT_INFO_640X480_60P;
    }    
    else if (_CHECK_RANGE(htotal,1056, VESA_HRANG))
    {
        info = MMP_CAP_INPUT_INFO_800X600_60P;
    }
    else if (_CHECK_RANGE(htotal,1344, VESA_HRANG))
    {
        info = MMP_CAP_INPUT_INFO_1024X768_60P;
    }
    else if (_CHECK_RANGE(htotal,1664, VESA_HRANG))
    {
        info = MMP_CAP_INPUT_INFO_1280X768_60P;
    }
    else if (_CHECK_RANGE(htotal,1440, VESA_HRANG))
    {
        info = MMP_CAP_INPUT_INFO_1280X800_60P;
    }    
    else if (_CHECK_RANGE(htotal,1800, VESA_HRANG) && _CHECK_RANGE(width,1280, VESA_WRANG))
    {
        info = MMP_CAP_INPUT_INFO_1280X960_60P;
    }
    else if (_CHECK_RANGE(htotal,1688, VESA_HRANG))
    {
        info = MMP_CAP_INPUT_INFO_1280X1024_60P;
    }
    else if (_CHECK_RANGE(htotal,1792, VESA_HRANG))
    {
        info = MMP_CAP_INPUT_INFO_1360X768_60P;
    }
    else if (_CHECK_RANGE(htotal,1500, VESA_HRANG))
    {
        info = MMP_CAP_INPUT_INFO_1366X768_60P;
    }    
    else if (_CHECK_RANGE(htotal,1904, VESA_HRANG))
    {
        info = MMP_CAP_INPUT_INFO_1440X900_60P;
    }  
    else if (_CHECK_RANGE(htotal,1600, VESA_HRANG)) //apple tv
    {
        info = MMP_CAP_INPUT_INFO_1440X900_60P;
    }  
    else if (_CHECK_RANGE(htotal,1864, VESA_HRANG))
    {
        info = MMP_CAP_INPUT_INFO_1400X1050_60P;
    }
    else if (_CHECK_RANGE(htotal,1800, VESA_HRANG) && _CHECK_RANGE(width,1600, VESA_WRANG))
    {
        info = MMP_CAP_INPUT_INFO_1600X900_60P;
    }
    else if (_CHECK_RANGE(htotal,2160, VESA_HRANG))
    {
        info = MMP_CAP_INPUT_INFO_1600X1200_60P;
    }
    else if (_CHECK_RANGE(htotal,2240, VESA_HRANG))
    {
        info = MMP_CAP_INPUT_INFO_1680X1050_60P;
    }
    else if (_CHECK_RANGE(htotal,1840, VESA_HRANG) && _CHECK_RANGE(width,1680, VESA_WRANG)) //apple tv
    {
        info = MMP_CAP_INPUT_INFO_1680X1050_60P;
    }
    else
    {
        info = MMP_CAP_INPUT_INFO_UNKNOWN;
    }
    return info;
}
//=============================================================================
//                Public Function Definition
//=============================================================================
//=============================================================================
/**
 * @brief Cap context initialization.
 * @param   none.
 * @return  MMP_RESULT :init success or fail
 */
//=============================================================================
MMP_RESULT
ithCapInitialize(
    void)
{
    MMP_RESULT  result   = MMP_SUCCESS;

    ithCap_Set_Reset();
    //ithCap_Set_Register_Reset();
    ithCapHWICInit();
    pthread_mutex_lock(&gCapnum_mutex);
    gCapinitNum = 0;  
    pthread_mutex_unlock(&gCapnum_mutex);

    if (result)
    {
        cap_msg_ex(CAP_MSG_TYPE_ERR, "%s error %d", __FUNCTION__, __LINE__);
    }
    return result;
}

//=============================================================================
/**
 * @brief Cap connect source and capinfo default init.
 * @param[in]   *ptDev,points to capture_handle structure.
 * @param[in]   CAPTURE_SETTING info
 * @return      MMP_RESULT :connect success or fail.
 *
 * 1st Connect will be  use cap0 hw , can use Onfly mode,MEM mode 
 * 2rd Connect will be  use cap1 hw , Only MEM mode (HW limit)
 */
//=============================================================================
MMP_RESULT ithCapConnect(
    CAPTURE_HANDLE *ptDev, CAPTURE_SETTING info)
{
    MMP_RESULT result = MMP_SUCCESS;
 
    pthread_mutex_lock(&gCapnum_mutex);
 
    if (ptDev == NULL || gCapinitNum >= CAP_DEVICE_ID_MAX)
    {
        result = MMP_RESULT_ERROR;
        goto end;
    }
 
    /* reset caphandle mem */
    memset((void *)ptDev, 0, sizeof(CAPTURE_HANDLE));
 
    /* count capture be init num */
    gCapinitNum++;
    
    /* cap id setting */
    ptDev->cap_id = gCapinitNum - 1;
 
    pthread_mutex_unlock(&gCapnum_mutex);
 
    /* default capinfo setting */
    Cap_Initialize(&ptDev->cap_info);
    
    /* capture set user config*/
    if(ptDev->cap_id == CAP_DEV_ID0)
        Cap_SetUserConfig(ptDev, gCap0Config);
    else
        Cap_SetUserConfig(ptDev, gCap1Config);
    
    /* frontend source id setting */
    ptDev->source_id = info.inputsource;

    /* Hw cap1 Only mem mode */
    if(ptDev->cap_id == CAP_DEV_ID1){
        info.OnflyMode_en = MEMORY_MODE;
        info.Interrupt_en = MMP_TRUE;
    } 
    
    /* capture onflymode flag setting */
    if (info.OnflyMode_en == 0)
    {   
        ptDev->cap_info.EnableOnflyMode = MEMORY_MODE;
        Cap_Memory_Initialize(ptDev, info);//memory mode
    }
    else if(info.OnflyMode_en == 1)
    {
        ptDev->cap_info.EnableOnflyMode = ONFLY_MODE;//onfly mode
    }
    else
    {
        ptDev->cap_info.EnableOnflyMode = MEMORY_WITH_ONFLY_MODE;
        Cap_Memory_Initialize(ptDev, info);//memory mode
    }
 
    /* capture interrupt flag setting */
    if (info.Interrupt_en)
        ptDev->cap_info.EnableInterrupt = MMP_TRUE;
    else
        ptDev->cap_info.EnableInterrupt = MMP_FALSE;
    
    /* setting mem pitchY pitchUV */
    ptDev->cap_info.ininfo.PitchY = info.Max_Width;
    ptDev->cap_info.ininfo.PitchUV = info.Max_Width;
    
end:
    if (result)
        cap_msg_ex(CAP_MSG_TYPE_ERR, "%s error %d", __FUNCTION__, __LINE__);
    return result;
}

//=============================================================================
/**
 * @brief Cap disconnect source include disable engine(unfire) ,free memory ,and reset cap_handle .
 * @param[in]   *ptDev,points to capture_handle structure.
 * @return      MMP_RESULT :disconnect success or fail.
 */
//=============================================================================
MMP_RESULT ithCapDisConnect(CAPTURE_HANDLE *ptDev)
{
    MMP_RESULT result = MMP_SUCCESS;
 
    if (ptDev == NULL)
    {
        result = MMP_RESULT_ERROR;
        goto end;
    }
    
    if(ithCap_Get_IsFire(ptDev->cap_id))
    {
         ithCap_Set_UnFire(ptDev->cap_id);
         result = ithCap_Get_WaitEngineIdle(ptDev->cap_id);
         if (result)
         {
             cap_msg_ex(CAP_MSG_TYPE_ERR, " err 0x%x !\n", result);
             goto end;
         }
         ithCap_Set_Clean_Intr(ptDev->cap_id);
    }
     
    if(ptDev->cap_info.EnableOnflyMode == MEMORY_MODE || ptDev->cap_info.EnableOnflyMode == MEMORY_WITH_ONFLY_MODE)
    {
        Cap_Memory_Clear(ptDev);
    }

    
    /* reset caphandle mem */
    memset((void *)ptDev, 0, sizeof(CAPTURE_HANDLE));
    
 
    pthread_mutex_lock(&gCapnum_mutex);
    if (gCapinitNum > 0)
        gCapinitNum--;
    pthread_mutex_unlock(&gCapnum_mutex);
end:
    if (result)
        cap_msg_ex(CAP_MSG_TYPE_ERR, "%s error %d", __FUNCTION__, __LINE__);
    
    return result;
}

//=============================================================================
/**
 * @brief Cap terminate all engine,include disable engine(unfire)  and reset engine.
 * @param   none.
 * @return  MMP_RESULT,1 => fail 0 => success
 */
//=============================================================================
MMP_RESULT
ithCapTerminate(
    void)
{
    MMP_RESULT  result   = MMP_SUCCESS;
    MMP_UINT16  index   = 0;

    /*Disable Cap all engine*/
    for (index = 0; index < CAP_DEVICE_ID_MAX; index++)
    {
        if(ithCap_Get_IsFire(index))
        {
            ithCap_Set_UnFire(index);
            result = ithCap_Get_WaitEngineIdle(index);
            if (result)
            {
                cap_msg_ex(CAP_MSG_TYPE_ERR, " err 0x%x !\n", result);
                goto end;
            }
        }
        ithCap_Set_Clean_Intr(index);
    }
    
    ithCap_Set_Reset();
end:
    if (result)
        cap_msg_ex(CAP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);

    return result;
}

//=============================================================================
/**
 * @brief Cap current fire status.
 * @param *ptDev,points to capture_handle structure.
 * @return true => fire(running) , false => unfire(stop)
 */
//=============================================================================

MMP_BOOL
ithCapIsFire(
    CAPTURE_HANDLE *ptDev)
{
    return ithCap_Get_IsFire(ptDev->cap_id);
}

//=============================================================================
/**
 * @brief ithCapGetEngineErrorStatus
 * @param MMP_CAP_LANE0_STATUS
 * @return
 * bit[3:0]Stable status:
 * [0]:Hsync stable
 * [1]:Vsync stable
 * [2]:DE stable(X)
 * [3]:DE stable(Y)
 * bit[11:8] Error status:
 * [1]:Hsync loss
 * [2]:Vsync loss
 * [3]:DE loss
 * [4]:frame end error
 * [5]:capture overflow
 * [7]:frame rate change
 * [8]:time out
 */
//=============================================================================
MMP_UINT32
ithCapGetEngineErrorStatus(
    CAPTURE_HANDLE *ptDev, MMP_CAP_LANE_STATUS lanenum)
{
    return ithCap_Get_Lane_status(ptDev->cap_id, lanenum);
}

//=============================================================================
/**
 * @brief Cap Set parameter to hw registers. 
 * @param *ptDev,points to capture_handle structure.
 * @return  MMP_RESULT,1 => fail 0 => success.
 */
//=============================================================================
MMP_RESULT
ithCapParameterSetting(
    CAPTURE_HANDLE *ptDev)
{
    MMP_RESULT  result   = MMP_SUCCESS;

    if (ptDev == MMP_NULL)
    {
        cap_msg_ex(CAP_MSG_TYPE_ERR, "Capture not initialize\n");
        result = MMP_RESULT_ERROR;
        goto end;
    }

    //Update parameter
    result = Cap_Update_Reg(ptDev);

end:
    if (result)
        cap_msg_ex(CAP_MSG_TYPE_ERR, "%s (%d) ERROR !!!!\n", __FUNCTION__, __LINE__);

    return result;
}

//=============================================================================
/**
 * @brief Set Cap fire or not.
 * @param *ptDev,points to capture_handle structure.
 * @param enable,true => fire, false => unfire.
 */
//=============================================================================
void
ithCapFire(
    CAPTURE_HANDLE *ptDev, MMP_BOOL enable)
{
    if(enable)
    {
        ithCap_Set_MemThreshold(ptDev->cap_id, ptDev->cap_info.ininfo.WrMergeThresld);
		
        // Update Error Handle mode
		if(ptDev->source_id == MMP_CAP_DEV_ANALOG_DECODER)
			ithCap_Set_Error_Handleing(ptDev->cap_id,0xFF7C);
		else
        	ithCap_Set_Error_Handleing(ptDev->cap_id,0xFF7F);

        ithCap_Set_Wait_Error_Reset(ptDev->cap_id);

        ithCap_Set_Fire(ptDev->cap_id);
    }
    else
    {
        ithCap_Set_UnFire(ptDev->cap_id);
    }
}

//=============================================================================
/**
 * @brief Register  an interrupt handler[Only OPENRTOS]
 * @param[in]   user define caphandler
 * @param[in]   *ptDev,points to capture_handle structure.
 * @return      none.
 */
//=============================================================================
void
ithCapRegisterIRQ(
    ITHIntrHandler caphandler, CAPTURE_HANDLE *ptDev)
{
    // Initialize Capture IRQ
    ithIntrDisableIrq(ITH_INTR_CAPTURE);
    ithIntrClearIrq(ITH_INTR_CAPTURE);
#if defined (__OPENRTOS__)
    // register NAND Handler to IRQ
    ithIntrRegisterHandlerIrq(ITH_INTR_CAPTURE, caphandler, (void *)ptDev);
#endif     // defined (__OPENRTOS__)

    // set IRQ to edge trigger
    ithIntrSetTriggerModeIrq(ITH_INTR_CAPTURE, ITH_INTR_EDGE);

    // set IRQ to detect rising edge
    ithIntrSetTriggerLevelIrq(ITH_INTR_CAPTURE, ITH_INTR_HIGH_RISING);

    // Enable IRQ
    ithIntrEnableIrq(ITH_INTR_CAPTURE);
}

//=============================================================================
/**
 * @brief Cap disable IRQ[Only OPENRTOS]
 * @param none.
 * @return none.
 */
//=============================================================================
void
ithCapDisableIRQ(
    void)
{
    // Initialize Capture IRQ
    ithIntrDisableIrq(ITH_INTR_CAPTURE);
    ithIntrClearIrq(ITH_INTR_CAPTURE);
}

//=============================================================================
/**
 * @brief Clear Interrupt
 * @param *ptDev,points to capture_handle structure.
 * @return 0.
 */
//=============================================================================
MMP_UINT16
ithCapClearInterrupt(
    CAPTURE_HANDLE *ptDev, MMP_BOOL get_err)
{
    if (get_err)
    {
        ithCap_Set_ErrReset(ptDev->cap_id);
        ithCap_Set_Error_Handleing(ptDev->cap_id, 0x0);
    }

    ithCap_Set_Clean_Intr(ptDev->cap_id);
    
    return 0;
}

//=============================================================================
/**
 * @brief  Get current number of memory ring buffer
 * @param[in]   *ptDev,points to capture_handle structure.
 * @return      current buffer number.
 */
//=============================================================================
MMP_UINT16
ithCapReturnWrBufIndex(
    CAPTURE_HANDLE *ptDev)
{
    MMP_UINT16  CapWrBufIndex;
    MMP_UINT16  status   = ithCap_Get_Lane_status(ptDev->cap_id, CAP_LANE0_STATUS);

    CapWrBufIndex = ((status & 0x70) >> 4);

    return CapWrBufIndex;
}

//=============================================================================
/**
 * @brief  Get current input Source Frame Rate(Note: need wait capture 1 frame )
 * @param[in] *ptDev,points to capture_handle structure.     
 * @return      MMP_CAP_FRAMERATE type.
 */
//=============================================================================
MMP_CAP_FRAMERATE
ithCapGetInputFrameRate(
    CAPTURE_HANDLE *ptDev)
{
    MMP_UINT32  RawVTotal;
    MMP_UINT16  FrameRate_mode;
    MMP_UINT32  MCLK_Freq;
    MMP_UINT32  framerate;
    MMP_UINT32  div;
    MMP_UINT32  interlaced;

    RawVTotal  = ithCap_Get_MRawVTotal(ptDev->cap_id);
    interlaced = ithCap_Get_Detected_Interleave(ptDev->cap_id);
    
    if(RawVTotal > 0)
    {
        if(interlaced)
            RawVTotal = RawVTotal / 2;
        
        div    = ithReadRegA(0xD8000014) & 0xF;
        
        if((ithReadRegA(0xD8000014) & 0x7000) >> 12)
            MCLK_Freq = 972 / div;//PLL1_N2 972MHZ
        else
            MCLK_Freq = 792 / div;//PLL1_N1 792MHZ
            
        framerate = ((3906.25 * MCLK_Freq * 1000) /RawVTotal);
        //printf("RawVTotal = %d MCLK_Freq = %d framerate = %d\n", RawVTotal, MCLK_Freq, framerate);
    }

    if ((23988 > framerate) && (framerate > 23946))       // 23.976fps
    {
        FrameRate_mode = MMP_CAP_FRAMERATE_23_97HZ;
    }
    else if ((24030 > framerate) && (framerate > 23987))  // 24fps
    {
        FrameRate_mode = MMP_CAP_FRAMERATE_24HZ;
    }
    else if ((25030 > framerate) && (framerate > 24970))  // 25fps
    {
        FrameRate_mode = MMP_CAP_FRAMERATE_25HZ;
    }
    else if ((29985 > framerate) && (framerate > 29940))  // 29.97fps
    {
        FrameRate_mode = MMP_CAP_FRAMERATE_29_97HZ;
    }
    else if ((30030 > framerate) && (framerate > 29984))  // 30fps
    {
        FrameRate_mode = MMP_CAP_FRAMERATE_30HZ;
    }
    else if ((51000 > framerate) && (framerate > 49000))  // 50fps
    {
        FrameRate_mode = MMP_CAP_FRAMERATE_50HZ;
    }
    else if ((57000 > framerate) && (framerate > 55000))  // 56fps
    {
        FrameRate_mode = MMP_CAP_FRAMERATE_56HZ;
    }
    else if ((59970 > framerate) && (framerate > 57001))  // 59.94fps
    {
        FrameRate_mode = MMP_CAP_FRAMERATE_59_94HZ;
    }
    else if ((62030 > framerate) && (framerate > 59969))  // 60fps
    {
        FrameRate_mode = MMP_CAP_FRAMERATE_60HZ;
    }
    else if ((70999 > framerate) && (framerate > 69000))  // 70fps
    {
        FrameRate_mode = MMP_CAP_FRAMERATE_70HZ;
    }
    else if ((73000 > framerate) && (framerate > 71000))  // 72fps
    {
        FrameRate_mode = MMP_CAP_FRAMERATE_72HZ;
    }
    else if ((76000 > framerate) && (framerate > 74000))  // 75fps
    {
        FrameRate_mode = MMP_CAP_FRAMERATE_75HZ;
    }
    else if ((86000 > framerate) && (framerate > 84000))  // 85fps
    {
        FrameRate_mode = MMP_CAP_FRAMERATE_85HZ;
    }
    else
    {
        FrameRate_mode = MMP_CAP_FRAMERATE_UNKNOW;
    }
    //printf("frame rate mode = %d \n",FrameRate_mode);
    return FrameRate_mode;
}

//=============================================================================
/**
 * @brief Use Cap HW auto dectected and get input info.
 * @param *ptDev,points to capture_handle structure.
 * @return MMP_CAP_INPUT_INFO.
 */
//=============================================================================
MMP_CAP_INPUT_INFO
ithCapGetInputSrcInfo(
    CAPTURE_HANDLE *ptDev)
{
    MMP_CAP_INPUT_INFO info = MMP_CAP_INPUT_INFO_UNKNOWN;
    MMP_CAP_FRAMERATE  frameRateMode = MMP_CAP_FRAMERATE_UNKNOW;
    MMP_UINT32 width, height, interlanced, htotal;
    CAP_CONTEXT        *Capctxt = &ptDev->cap_info;

    frameRateMode = ithCapGetInputFrameRate(ptDev);
    width         = ithCap_Get_Detected_Width(ptDev->cap_id);
    height        = ithCap_Get_Detected_Height(ptDev->cap_id);
    interlanced   = ithCap_Get_Detected_Interleave(ptDev->cap_id);
#ifdef VESA_TIMING_SOUPPORT    
    htotal        = ithCap_Get_Detected_Htotal(ptDev->cap_id);
#endif

    switch (frameRateMode)
    {
    case MMP_CAP_FRAMERATE_23_97HZ:
        if (_CHECK_RANGE(width,1920,CEA_TIMING_RANG) && _CHECK_RANGE(height,1080,CEA_TIMING_RANG))
            info = MMP_CAP_INPUT_INFO_1920X1080_23P;
        else
            info = MMP_CAP_INPUT_INFO_UNKNOWN;
        break;

    case MMP_CAP_FRAMERATE_24HZ:
        if (_CHECK_RANGE(width,1920,CEA_TIMING_RANG) && _CHECK_RANGE(height,1080,CEA_TIMING_RANG))
            info = MMP_CAP_INPUT_INFO_1920X1080_24P;
        else
            info = MMP_CAP_INPUT_INFO_UNKNOWN;
        break;

    case MMP_CAP_FRAMERATE_25HZ:
        if (_CHECK_RANGE(width,1920,CEA_TIMING_RANG) && _CHECK_RANGE(height,1080,CEA_TIMING_RANG))
            info = MMP_CAP_INPUT_INFO_1920X1080_25P;
        else
            info = MMP_CAP_INPUT_INFO_UNKNOWN;
        break;

    case MMP_CAP_FRAMERATE_29_97HZ:
        if (_CHECK_RANGE(width,1920,CEA_TIMING_RANG) && _CHECK_RANGE(height,1080,CEA_TIMING_RANG))
            info = MMP_CAP_INPUT_INFO_1920X1080_29P;
        else
            info = MMP_CAP_INPUT_INFO_UNKNOWN;
        break;

    case MMP_CAP_FRAMERATE_30HZ:
        if (_CHECK_RANGE(width,1920,CEA_TIMING_RANG) && _CHECK_RANGE(height,1080,CEA_TIMING_RANG))
            info = MMP_CAP_INPUT_INFO_1920X1080_30P;
        else
            info = MMP_CAP_INPUT_INFO_UNKNOWN;
        break;

    case MMP_CAP_FRAMERATE_50HZ:
        if (_CHECK_RANGE(width,720,CEA_TIMING_RANG) && _CHECK_RANGE(height,576,CEA_TIMING_RANG))
        {
            if (interlanced)
                info = MMP_CAP_INPUT_INFO_720X576_50I;
            else
                info = MMP_CAP_INPUT_INFO_720X576_50P;
        }
        else if (_CHECK_RANGE(width,1280,CEA_TIMING_RANG) && _CHECK_RANGE(height,720,CEA_TIMING_RANG))
        {
            info = MMP_CAP_INPUT_INFO_1280X720_50P;
        }
        else if (_CHECK_RANGE(width,1920,CEA_TIMING_RANG) && _CHECK_RANGE(height,1080,CEA_TIMING_RANG))
        {
            if (interlanced)
                info = MMP_CAP_INPUT_INFO_1920X1080_50I;
            else
                info = MMP_CAP_INPUT_INFO_1920X1080_50P;
        }
        else
        {
            info = MMP_CAP_INPUT_INFO_UNKNOWN;
        }
        break;

    case MMP_CAP_FRAMERATE_59_94HZ:
       
        if (_CHECK_RANGE(width,720,CEA_TIMING_RANG) && _CHECK_RANGE(height,480,CEA_TIMING_RANG))
        {
            if (interlanced)
                info = MMP_CAP_INPUT_INFO_720X480_59I;
            else
                info = MMP_CAP_INPUT_INFO_720X480_59P;
        }
        else if (_CHECK_RANGE(width,1280,CEA_TIMING_RANG) && _CHECK_RANGE(height,720,CEA_TIMING_RANG))
        {
            info = MMP_CAP_INPUT_INFO_1280X720_59P;
        }
        else if(_CHECK_RANGE(width,1920,CEA_TIMING_RANG) && _CHECK_RANGE(height,1080,CEA_TIMING_RANG))
        {
            if (interlanced)
                info = MMP_CAP_INPUT_INFO_1920X1080_59I;
            else
                info = MMP_CAP_INPUT_INFO_1920X1080_59P;
        }
        break;

    case MMP_CAP_FRAMERATE_60HZ:
        if (_CHECK_RANGE(width,720,CEA_TIMING_RANG) && _CHECK_RANGE(height,480,CEA_TIMING_RANG))
        {
            if (interlanced)
                info = MMP_CAP_INPUT_INFO_720X480_60I;
            else
                info = MMP_CAP_INPUT_INFO_720X480_60P;
        }
        else if (_CHECK_RANGE(width,1280,CEA_TIMING_RANG) && _CHECK_RANGE(height,720,CEA_TIMING_RANG))
        {
            info = MMP_CAP_INPUT_INFO_1280X720_60P;
        }     
        else if (_CHECK_RANGE(width,1920,CEA_TIMING_RANG) && _CHECK_RANGE(height,1080,CEA_TIMING_RANG))
        {
            if (interlanced)
                info = MMP_CAP_INPUT_INFO_1920X1080_60I;
            else
                info = MMP_CAP_INPUT_INFO_1920X1080_60P;
        }
        break;

    default:
        info = MMP_CAP_INPUT_INFO_UNKNOWN;

        break;
    }
    
#ifdef VESA_TIMING_SOUPPORT
    if(info == MMP_CAP_INPUT_INFO_UNKNOWN)
        info = _CHECK_VESA_TIMING(htotal, width);        
#endif

    return info;
}

//=============================================================================
/**
 * ithAVSyncCounterInit
 */
//=============================================================================
void
ithAVSyncCounterCtrl(CAPTURE_HANDLE *ptDev, AV_SYNC_COUNTER_CTRL mode, MMP_UINT16 divider)
{
    ithAVSync_CounterCtrl(ptDev->cap_id, mode, divider);
}

//=============================================================================
/**
 * ithAVSyncCounterReset
 */
//=============================================================================
void
ithAVSyncCounterReset(CAPTURE_HANDLE *ptDev, AV_SYNC_COUNTER_CTRL mode)
{
    ithAVSync_CounterReset(ptDev->cap_id, mode);
}

//=============================================================================
/**
 * ithAVSyncCounterLatch
 */
//=============================================================================
MMP_UINT32
ithAVSyncCounterLatch(CAPTURE_HANDLE *ptDev, AV_SYNC_COUNTER_CTRL cntSel)
{
    ithAVSync_CounterLatch(ptDev->cap_id, cntSel);
}

//=============================================================================
/**
 * ithAVSyncCounterRead
 */
//=============================================================================
MMP_UINT32
ithAVSyncCounterRead(CAPTURE_HANDLE *ptDev, AV_SYNC_COUNTER_CTRL mode)
{
    return ithAVSync_CounterRead(ptDev->cap_id, mode);
}

//=============================================================================
/**
 * ithAVSyncMuteDetect
 */
//=============================================================================
MMP_BOOL
ithAVSyncMuteDetect(CAPTURE_HANDLE *ptDev)
{
    return ithAVSync_MuteDetect(ptDev->cap_id);
}

//=============================================================================
/**
 * Set color control value.
 */
//=============================================================================
void
ithCapSetColorCtrl(
    CAPTURE_HANDLE *ptDev, const MMP_CAP_COLOR_CTRL *data)
{
#if defined (CAP_USE_COLOR_EFFECT)
    CAP_CONTEXT *Capctxt = &ptDev->cap_info;

    if (data->brightness > 127)
        Capctxt->ColorCtrl.brightness = 127;
    else if (data->brightness < -128)
        Capctxt->ColorCtrl.brightness = -128;
    else
        Capctxt->ColorCtrl.brightness = data->brightness;

    if (data->contrast > 4.0)
        Capctxt->ColorCtrl.contrast = 4.0;
    else if (data->contrast < 0.0)
        Capctxt->ColorCtrl.contrast = 0.0;
    else
        Capctxt->ColorCtrl.contrast = data->contrast;

    if (data->hue > 359)
        Capctxt->ColorCtrl.hue = 359;
    else if (data->hue < 0)
        Capctxt->ColorCtrl.hue = 0;
    else
        Capctxt->ColorCtrl.hue = data->hue;

    if (data->saturation > 4.0)
        Capctxt->ColorCtrl.saturation = 4.0;
    else if (data->saturation < 0.0)
        Capctxt->ColorCtrl.saturation = 0.0;
    else
        Capctxt->ColorCtrl.saturation = data->saturation;

    Cap_SetColorCorrMatrix(
        &Capctxt->CCFun,
        Capctxt->ColorCtrl.brightness,
        Capctxt->ColorCtrl.contrast,
        Capctxt->ColorCtrl.hue,
        Capctxt->ColorCtrl.saturation,
        Capctxt->ColorCtrl.colorEffect);

    Capctxt->UpdateFlags |= CAP_FLAGS_UPDATE_CCMatrix;
#endif
}

//=============================================================================
/**
 * Get color control value.
 */
//=============================================================================
void
ithCapGetColorCtrl(
    CAPTURE_HANDLE *ptDev, MMP_CAP_COLOR_CTRL *data)
{
#if defined (CAP_USE_COLOR_EFFECT)
    CAP_CONTEXT *Capctxt = &ptDev->cap_info;

    data->brightness = Capctxt->ColorCtrl.brightness;
    data->contrast   = Capctxt->ColorCtrl.contrast;
    data->hue        = Capctxt->ColorCtrl.hue;
    data->saturation = Capctxt->ColorCtrl.saturation;
#endif
}

//=============================================================================
/**
 * Update Color Matrix.
 */
//=============================================================================
void
ithCapUpdateColorMatrix(
    CAPTURE_HANDLE *ptDev)
{
#if defined (CAP_USE_COLOR_EFFECT)
    Cap_UpdateColorMatrix(ptDev);
#endif
}

//=============================================================================
/**
 * @brief power up cap controller.
 * @param void.
 * @return void.
 */
//=============================================================================
void
ithCapPowerUp(
    void)
{
    ithCap_Set_Reset();
    ithCapEnableClock();
}

//=============================================================================
/**
 * @brief power down cap controller.
 * @param void.
 * @return void.
 */
//=============================================================================
void
ithCapPowerDown(
    void)
{
    MMP_UINT16  index   = 0;
    MMP_RESULT  result   = MMP_SUCCESS;
    
    for (index = 0; index < CAP_DEVICE_ID_MAX; index++)
    {
        if(ithCap_Get_IsFire(index))
        {
            ithCap_Set_UnFire(index);
            result = ithCap_Get_WaitEngineIdle(index);
            if (result)
            {
                cap_msg_ex(CAP_MSG_TYPE_ERR, " err 0x%x !\n", result);
                goto end;
            }
        }
        ithCap_Set_Clean_Intr(index);
    }
    ithCapDisableClock();
    
end:
    if (result)
        cap_msg_ex(CAP_MSG_TYPE_ERR, " %s() err 0x%x !\n", __FUNCTION__, result);    
}

//=============================================================================
/**
 * @brief wait Cap engine idle.
 * @param *ptDev,points to capture_handle structure.
 * @return true or false.
 */
//=============================================================================
MMP_UINT32
ithCapWaitEngineIdle(
    CAPTURE_HANDLE *ptDev)
{
    ithCap_Set_UnFire(ptDev->cap_id);
    
    return ithCap_Get_WaitEngineIdle(ptDev->cap_id);
}

//=============================================================================
/**
 * @brief reset all engine, include Cap 0 & 1.
 * @param void.
 * @return void.
 */
//=============================================================================
void
ithCapEngineReset(
    void)
{
    ithCap_Set_Reset();
}

//=============================================================================
/**
 * @brief Cap Get Detected width
 * @param *ptDev,points to capture_handle structure.
 * @return Detected Width.
 */
//=============================================================================
MMP_UINT32
ithCapGetDetectedWidth(
    CAPTURE_HANDLE *ptDev)
{
    return ithCap_Get_Detected_Width(ptDev->cap_id);
}

//=============================================================================
/**
 * @brief Cap Get Detected height
 * @param *ptDev,points to capture_handle structure.
 * @return Detected Height.
 */
//=============================================================================
MMP_UINT32
ithCapGetDetectedHeight(
    CAPTURE_HANDLE *ptDev)
{
    return ithCap_Get_Detected_Height(ptDev->cap_id);
}

//=============================================================================
/**
 * @brief Cap Get Detected Interleave
 * @param *ptDev,points to capture_handle structure.
 * @return Detected Interleave.
 */
//=============================================================================
MMP_UINT32
ithCapGetDetectedInterleave(
    CAPTURE_HANDLE *ptDev)
{
    return ithCap_Get_Detected_Interleave(ptDev->cap_id);
}

//=============================================================================
/**
 * @brief Set cap interleave mode.
 * @param *ptDev,points to capture_handle structure.
 * @return void.
 */
//=============================================================================
void
ithCapSetInterleave(
    CAPTURE_HANDLE *ptDev, MMP_UINT32 interleave)
{
    ithCap_Set_Interleave(ptDev->cap_id, interleave);
}

//=============================================================================
/**
 * @brief Show the ClkDelay windows.
 * @param *ptDev,points to capture_handle structure.
 * @return void.
 */
//=============================================================================
void
ithCapSearchClkDelay(CAPTURE_HANDLE *ptDev)
{
    MMP_UINT32 delay_index;
    for(delay_index = 0; delay_index < 16; delay_index++)
     {
        ithCap_Set_Reset();
        Cap_Update_Reg(ptDev);
        ithCap_Set_Delay(ptDev->cap_id, delay_index);
        usleep(2000);
        if(ithCap_Get_Lane_status(ptDev->cap_id, CAP_LANE0_STATUS) & (0x1 << 20))
             ithPrintf("delay = %d , stable\n", delay_index);
        else
             ithPrintf("delay = %d , unstable\n", delay_index);
        usleep(1000);
     }

}

