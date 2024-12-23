#include <string.h> 
#include "mmp_capture.h"
#include "capture/video_device_table.h"
#include "hdmirx/mmp_hdmirx.h"
#include "capture_config.h"

//=============================================================================
//                Constant Definition
//=============================================================================

//=============================================================================
//                Macro Definition
//=============================================================================

//=============================================================================
//                Structure Definition
//=============================================================================

//=============================================================================
//                Global Data Definition
//=============================================================================
static MMP_UINT16   gtHDMIResolution    = 0xFF;
//=============================================================================
//                Private Function Definition
//=============================================================================

//=============================================================================
//                Public Function Definition
//=============================================================================

//=============================================================================
/**
 * Cap Device Tri-State.
 */
//=============================================================================
void
ithCapDeviceAllDeviceTriState(
    void)
{
#if CFG_HDMI_RX_ENABLE
    //Tri-State All Device
    ithHDMIRXOutputPinTriState(MMP_TRUE);
#endif
}

//=============================================================================
/**
 * Cap Device initialization.
 */
//=============================================================================
MMP_RESULT
ithCapDeviceInitialize(
    void)
{
    MMP_RESULT result = MMP_SUCCESS;
#if CFG_HDMI_RX_ENABLE
    gtHDMIResolution    = 0xFF;
    ithHDMIRXInitialize();


    if (result)
        cap_msg_ex(CAP_MSG_TYPE_ERR, "%s error \n", __FUNCTION__);
#endif
    return (MMP_RESULT)result;
}

//=============================================================================
/**
 * Cap Device terminate.
 */
//=============================================================================
void
ithCapDeviceTerminate(
    void)
{
#if CFG_HDMI_RX_ENABLE
    ithCapDeviceAllDeviceTriState();

    ithHDMIRXTerminate();

    gtHDMIResolution    = 0xFF;
#endif
}

//=============================================================================
/**
 * Cap Get Device Signal State.
 */
//=============================================================================
MMP_BOOL
ithCapDeviceIsSignalStable(
    void)
{
    MMP_BOOL isSignalChange = MMP_FALSE;
#if CFG_HDMI_RX_ENABLE
    isSignalChange = ithHDMIRXIsSignalStable();
#endif
    return (MMP_BOOL)isSignalChange;
}

//=============================================================================
/**
 * CapGetDeviceInfo for HDMI souce
 * call sequential : ithCapDeviceInitialize -> wait ithCapDeviceIsSignalStable = true -> ithCapGetDeviceInfo
 */
//=============================================================================

void
ithCapGetDeviceInfo(
    CAPTURE_HANDLE *ptDev)
{
    MMP_UINT32          HTotal, VTotal, ColorDepth;
    MMP_UINT16          i, rate, Index;
    MMP_BOOL            matchResolution = MMP_FALSE;
#if CFG_HDMI_RX_ENABLE    
    CAP_CONTEXT         *Capctxt        = &ptDev->cap_info;

    Capctxt->YUVinfo.ColorDepth = COLOR_DEPTH_8_BITS;

    if (ptDev->source_id == MMP_CAP_DEV_HDMIRX && ithHDMIRXIsSignalStable())
    {
    
        Capctxt->ininfo.Interleave  = (MMP_BOOL)ithHDMIRXGetProperty(HDMIRX_IS_INTERLACE);

        if (Capctxt->ininfo.Interleave)
            Capctxt->ininfo.capheight = (MMP_UINT16)ithHDMIRXGetProperty(HDMIRX_HEIGHT) << 1;
        else
            Capctxt->ininfo.capheight = (MMP_UINT16)ithHDMIRXGetProperty(HDMIRX_HEIGHT);

        Capctxt->ininfo.capwidth= (MMP_UINT16)ithHDMIRXGetProperty(HDMIRX_WIDTH);

        HTotal      = ithHDMIRXGetProperty(HDMIRX_HTOTAL);
        VTotal      = ithHDMIRXGetProperty(HDMIRX_VTOTAL);
        /*CEA timing check*/
        for (i = 0; i < CAP_HDMI_INPUT_VESA ; i++)
        {
            if ((HTotal == HDMI_TABLE[i].HTotal) &&
                (VTotal > HDMI_TABLE[i].VTotal - 100 && VTotal < HDMI_TABLE[i].VTotal + 100))
            {
                if (i == 6 || i == 7) //1080p60/1080p50 or 1080p60/1080p25
                {
                    MMP_UINT32 hdmirxPCLK = ithHDMIRXGetProperty(HDMIRX_PCLK);
                    if (hdmirxPCLK < 10000) //100MHz
                        i = i + 6;       //1080p60/1080p25
                }
                /* Set input ROI info */
                Capctxt->ininfo.ROIPosX      = HDMI_TABLE[i].ROIPosX;
                Capctxt->ininfo.ROIPosY      = HDMI_TABLE[i].ROIPosY;
                Capctxt->ininfo.ROIWidth     = HDMI_TABLE[i].ROIWidth;
                Capctxt->ininfo.ROIHeight    = HDMI_TABLE[i].ROIHeight;                
                Capctxt->Skippattern         = HDMI_TABLE[i].Skippattern;
                Capctxt->SkipPeriod          = HDMI_TABLE[i].SkipPeriod;
                Capctxt->ininfo.framerate    = HDMI_TABLE[i].FrameRate;
                Capctxt->inmux_info.UCLKDly  = HDMI_TABLE[i].ClkDelay;
                Capctxt->inmux_info.UCLKInv  = HDMI_TABLE[i].CLKInverse;
                matchResolution              = MMP_TRUE;
                gtHDMIResolution             = i;
                printf("[DEVICE_HDMI]CEA timing \n");
                break;
            }
        }
        /*VESA timing check*/
        if(matchResolution == MMP_FALSE)
        {
            for (i = CAP_HDMI_INPUT_VESA; i < ITH_COUNT_OF(HDMI_TABLE); i++)
            {
                if ((HTotal == HDMI_TABLE[i].HTotal) &&
                    (VTotal > HDMI_TABLE[i].VTotal - 100 && VTotal < HDMI_TABLE[i].VTotal + 100))
                {
                    if(HDMI_TABLE[i].Index == 18)//1280x960 60HZ
                        if(Capctxt->ininfo.capwidth > 1280)
                            continue;
                    
                    /* Set input ROI info */
                    Capctxt->ininfo.ROIPosX      = HDMI_TABLE[i].ROIPosX;
                    Capctxt->ininfo.ROIPosY      = HDMI_TABLE[i].ROIPosY;
                    Capctxt->ininfo.ROIWidth     = HDMI_TABLE[i].ROIWidth;
                    Capctxt->ininfo.ROIHeight    = HDMI_TABLE[i].ROIHeight;                
                    Capctxt->Skippattern         = HDMI_TABLE[i].Skippattern;
                    Capctxt->SkipPeriod          = HDMI_TABLE[i].SkipPeriod;
                    Capctxt->ininfo.framerate    = HDMI_TABLE[i].FrameRate;
                    Capctxt->inmux_info.UCLKDly  = HDMI_TABLE[i].ClkDelay;
                    Capctxt->inmux_info.UCLKInv  = HDMI_TABLE[i].CLKInverse;
                    matchResolution              = MMP_TRUE;
                    gtHDMIResolution             = i;
                    printf("[DEVICE_HDMI]VESA timing \n");
                    break;
                }
            }
        }

        if (ithHDMIRXGetProperty(HDMIRX_OUTPUT_VIDEO_MODE) == 0)
        {
            Capctxt->funen.EnCSFun      = MMP_TRUE;
            
            if(ithHDMIRXGetProperty(HDMIRX_COLOR_FULLRANGE))
            {
                Capctxt->ColorMatrixMode    = RGBTOYUV_ITU601_0_255;
                printf("[DEVICE_HDMI]RGB444 0-255 Output\n");
            }
            else
            {
                Capctxt->ColorMatrixMode    = RGBTOYUV_ITU601_16_235;
                printf("[DEVICE_HDMI]RGB444 16-235 Output\n");
            }
            
            Capctxt->YUVinfo.InputMode  = RGB888;
            Capctxt->YUVinfo.InputWidth = 0x0;
            
        }
        else if(ithHDMIRXGetProperty(HDMIRX_OUTPUT_VIDEO_MODE) == 1)
        {
            Capctxt->funen.EnCSFun      = MMP_FALSE;
            Capctxt->ColorMatrixMode    = BYPASS_MODE;
            Capctxt->YUVinfo.InputMode  = YUV422; 
            Capctxt->YUVinfo.InputWidth = 0x1;
            printf("[DEVICE_HDMI]YUV422 Output\n");
        }
        else
        {
            Capctxt->funen.EnCSFun      = MMP_FALSE;
            Capctxt->ColorMatrixMode    = BYPASS_MODE; 
            Capctxt->YUVinfo.InputMode  = YUV444;
            Capctxt->YUVinfo.InputWidth = 0x0;
            printf("[DEVICE_HDMI]YUV444 Output\n");
        }
        
        if(matchResolution == MMP_FALSE)
            printf("[DEVICE_HDMI][Warring]Not supported timing\n");
        else
            printf("[DEVICE_HDMI]Htotal = %d, Vtotal = %d, w = %d h = %d, res = %d\n", HTotal, VTotal, Capctxt->ininfo.capwidth, Capctxt->ininfo.capheight, gtHDMIResolution);
        
        //ColorDepth      = ithHDMIRXGetProperty(HDMIRX_COLOR_DEPTH);
        ColorDepth        = 24;

        if (ColorDepth == 36)
            Capctxt->YUVinfo.ColorDepth = COLOR_DEPTH_12_BITS;
        else if (ColorDepth == 30)
            Capctxt->YUVinfo.ColorDepth = COLOR_DEPTH_10_BITS;
        else //(ColorDepth == 24)
            Capctxt->YUVinfo.ColorDepth = COLOR_DEPTH_8_BITS;
    }
    else
    {
        i = 0;
        Capctxt->ininfo.capwidth     = HDMI_TABLE[i].HActive;
        Capctxt->ininfo.capheight    = HDMI_TABLE[i].VActive;
        Capctxt->ininfo.ROIPosX      = HDMI_TABLE[i].ROIPosX;
        Capctxt->ininfo.ROIPosY      = HDMI_TABLE[i].ROIPosY;
        Capctxt->ininfo.ROIWidth     = HDMI_TABLE[i].ROIWidth;
        Capctxt->ininfo.ROIHeight    = HDMI_TABLE[i].ROIHeight;                
        Capctxt->Skippattern         = HDMI_TABLE[i].Skippattern;
        Capctxt->SkipPeriod          = HDMI_TABLE[i].SkipPeriod;
        Capctxt->ininfo.framerate    = HDMI_TABLE[i].FrameRate;
        Capctxt->inmux_info.UCLKDly  = HDMI_TABLE[i].ClkDelay;    
        matchResolution = FALSE;
        printf("[DEVICE_HDMI][Warring]hdmi not stable, can't get info\n");
        
    }
    
    /*Y UV Pitch*/
    Capctxt->ininfo.PitchY =  (((Capctxt->ininfo.capwidth +15) >>4) <<4);
    Capctxt->ininfo.PitchUV = (((Capctxt->ininfo.capwidth +15) >>4) <<4);
	
    /*Set output info*/
    Capctxt->outinfo.OutWidth      = Capctxt->ininfo.ROIWidth;
    Capctxt->outinfo.OutHeight     = Capctxt->ininfo.ROIHeight;
    Capctxt->outinfo.OutAddrOffset = 0;
    Capctxt->outinfo.OutMemFormat  = SEMI_PLANAR_420;        
    Capctxt->bMatchResolution  = matchResolution;
#endif
}

//=============================================================================
/**
 * Get Device Property.
 */
//=============================================================================
MMP_UINT16 
ithCapDeviceGetProperty(DEVICESPROPERTYS option)
{
    MMP_UINT16 info = 0;
    switch (option)
    {
        case DEVICES_TABLEINDEX:
            info = gtHDMIResolution;
            break;
        default:
            break;
    } 

    return info;
    
}

//=============================================================================
/**
 * Device LED ON/OFF
 */
//=============================================================================

void 
ithCapDeviceLEDON(
    MMP_BOOL enable)
{
//DO NOTHING
}

void
ithCapDeviceCHSwitch(
    MMP_UINT16 ch)
{
//DO NOTHING
}

