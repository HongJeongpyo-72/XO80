#ifndef __MMP_ISP_H__
#define __MMP_ISP_H__

/** @defgroup ith_isp ISP
 *  @{
 */
 
/**
 * H/W ISP Engine Operation API
 *
 * ISP is an abbreviation for image signal processing. As the name implies, 
 * these APIs are provided to speed up image processing with hardware.
 * @file mmp_isp.h
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "../isp_error.h"
#include "ite/mmp_types.h"

//=============================================================================
//                              Constant Definition
//=============================================================================
#if defined(WIN32)
    #define ISP_API __declspec(dllexport)
#else
    #define ISP_API extern
#endif

//=============================================================================
//                              Constant Definition
//=============================================================================
/**
 * ISP core numbers.
 *
 * @see mmpIspInitialize()
 */
typedef enum MMP_ISP_CORE_TAG
{
    MMP_ISP_CORE_0 = ITH_ISP_CORE0,     /**< VP0 */
    MMP_ISP_CORE_1 = ITH_ISP_CORE1      /**< VP1 */
} MMP_ISP_CORE;

/**
 * ISP pre-scaling settings.
 *
 * @see mmpIspSetCore()
 */
typedef enum MMP_ISP_PRESCALE_SELECT_TAG
{
    MMP_ISP_PRESCALE_PREVIEW,
    MMP_ISP_PRESCALE_NORMAL
} MMP_ISP_PRESCALE_SELECT;

/**
 * ISP modes.
 *
 * @see mmpIspSetMode()
 */
typedef enum MMP_ISP_MODE_TAG
{
    MMP_ISP_MODE_NONE,                  /**< Not used. */
	MMP_ISP_MODE_JPEG_TRANSFORM,        /**< Color transform with scaling for Jpeg engine use. */
    MMP_ISP_MODE_TRANSFORM,             /**< Color transform with scaling. */
    MMP_ISP_MODE_SHOW_IMAGE,            /**< Show image mode. */
    MMP_ISP_MODE_PLAY_VIDEO,            /**< Play video mode. */
    MMP_ISP_MODE_JPEG_RESIZE,           /**< Jpeg re-size mode. */
    MMP_ISP_MODE_MOTION_JPEG,           /**< Motion jpeg paly mode. */
    MMP_ISP_MODE_SOFTWARE_HANDSHAKE,    /**< Software handshaking mode. */
    MMP_ISP_MODE_TRANSFORM_BYPASS,      /**< Color transform with bypass scaling. */
    MMP_ISP_MODE_SHOW_IMAGE_BYPASS,     /**< Show image mode with bypass. */
    MMP_ISP_MODE_PLAY_VIDEO_BYPASS      /**< Play video mode with bypass. */
} MMP_ISP_MODE;

/**
 *  JPEG/MPEG engine fire mode.
 */
typedef enum TRIGGER_MODE_TAG
{
    TRIGGER_MODE_HW,
    TRIGGER_MODE_COMMAND
} TRIGGER_MODE;

/**
 * Display rotation type.
 *
 * @see mmpIspSetDisplayRotateType()
 */
typedef enum MMP_ISP_ROTATE_TYPE_TAG
{
    MMP_ISP_ROTATE_0,
    MMP_ISP_ROTATE_90,                  /**< Rotate the image 90 degrees counter clockwise. Input rotation doesn't support this degree.*/
    MMP_ISP_ROTATE_270,                 /**< Rotate the image 270 degrees counter clockwise. Input rotation doesn't support this degree.*/
    MMP_ISP_ROTATE_180,                 /**< Rotate the image 180 degrees counter clockwise. */
    MMP_ISP_ROTATE_MIRROR,              /**< Flip the image horizontally. */
    MMP_ISP_ROTATE_FLIP                 /**< Flip the image vertically. */
} MMP_ISP_ROTATE_TYPE;

/**
 * SubTitle format.
 *
 * @see mmpIspSetSubTitle()
 */
typedef enum MMP_ISP_SUBTITLE_FORMAT_TAG
{
    MMP_ISP_SUBTITLE_YUVT_6442,
    MMP_ISP_SUBTITLE_YUVT_4444,
    MMP_ISP_SUBTITLE_YUVT_6334,
    MMP_ISP_SUBTITLE_YUVT_5443
} MMP_ISP_SUBTITLE_FORMAT;

/**
 * Clipping type.
 *
 * @see mmpIspSetClipType()
 */
typedef enum MMP_ISP_CLIP_TYPE_TAG
{
    MMP_ISP_CLIP_INSIDE,                /**< Clip the inside region of specified clipping window. */
    MMP_ISP_CLIP_OUTSIDE                /**< Clip the outside region of specified clipping window. */
} MMP_ISP_CLIP_TYPE;

/**
 * Color control type.
 *
 * @remark Calling the mmpIspUpdate() method to active it.
 *
 * @see mmpIspSetColorControl()
 */
typedef enum MMP_ISP_COLOR_CTRL_TAG
{
    /** Hue range is 0 ~ 359, and default value is 0. */
    MMP_ISP_HUE,
    /** Contrast range is -64 ~ 63, and default value is 0. */
    MMP_ISP_CONTRAST,
    /** Saturation range is 0 ~ 255, and default value is 128. */
    MMP_ISP_SATURATION,
    /** Brightness range is -64 ~ 63, and default value is 0, */
    MMP_ISP_BRIGHTNESS,
    /** Temperature range is 4000 ~ 10000, 0 is no color temperature adjustment, */
    MMP_ISP_COLOR_TEMPERATURE,
    /** Sharpness range is -4 ~ 5, 0 is mapping to 1.0f */
    MMP_ISP_SHARPNESS
} MMP_ISP_COLOR_CTRL;

/**
 * ISP capability.
 *
 * @remark When application enable or disable these capability during preview, it must call
 * mmpIspUpdate() to active it.
 *
 * @see mmpIspEnable mmpIspDisable
 */
typedef enum MMP_ISP_CAPS_TAG
{
    //Enable/disable deinterlace
    MMP_ISP_DEINTERLACE,

    //Enable/disable low leve ledge deinterlace
    MMP_ISP_LOWLEVELEDGE,

    //Enable/disable frame function
    MMP_ISP_FRAME_FUNCTION_0,
    MMP_ISP_FRAME_FUNCTION_1,

    //Enable/disable frame function blend constant source data
    MMP_ISP_CONST_DATA_BLEND_0,
    MMP_ISP_CONST_DATA_BLEND_1,

    //Enable/disable subtitle
    MMP_ISP_SUBTITLE_0,
    MMP_ISP_SUBTITLE_1,

    //Enable/disable clipping.
    MMP_ISP_CLIPPING0,
    MMP_ISP_CLIPPING1,
    MMP_ISP_CLIPPING2,

    //Enable/disable video output
    MMP_ISP_DISABLE_VIDEO_OUT,

    //Queue fire mode
    MMP_ISP_QFIRE_MODE,

    //Hardware auto swap address base
    MMP_ISP_AUTO_SWAP_FIELD,

    //Field scaling, (Don't support deinterlace)
    MMP_ISP_FIELD_SCALE_MODE,

    //Hardware auto swap address base
    MMP_ISP_ONFLY_WRITE_MEMORY,

    //Double frame rate
    MMP_ISP_DOUBLE_FRAMERATE_MODE,

    //Negative effect
    MMP_ISP_NEGATIVE_EFFECT_MODE,

    //Software Flip ISP Write Buffer
    MMP_ISP_SOFTWARE_WRITE_FLIP_MODE,

    //Output field mode
    MMP_ISP_OUTPUT_FIELD_MODE,

    //KeepLastField
    MMP_ISP_KEEP_LAST_FIELD_MODE,

    //Preview Video
    MMP_ISP_PREVIEW_VIDEO_MODE,

    //Capture OnFly status
    MMP_ISP_CAP_ONFLY_MODE,

    //LCD OnFly status
    MMP_ISP_LCD_ONFLY_MODE,

    //Fire from ISR
    MMP_ISP_ISR_FIRE_MODE,

#if (CFG_CHIP_FAMILY == 960)
    //Output RGB Alpha
    MMP_ISP_OUTPUT_RGB_ALPHA,
#endif

    //UV repeat mode
    MMP_ISP_UV_REPEAT_MODE
} MMP_ISP_CAPS;

/**
 * Output image format.
 *
 * @see mmpIspSetOutFormat()
 */
typedef enum MMP_ISP_OUTFORMAT_TAG
{
    //ISP Output Format
    MMP_ISP_OUT_DITHER565A,
    MMP_ISP_OUT_DITHER565B,
    MMP_ISP_OUT_DITHER444A,
    MMP_ISP_OUT_DITHER444B,
    MMP_ISP_OUT_RGB565,
    MMP_ISP_OUT_RGB888,
    MMP_ISP_OUT_YUYV,
    MMP_ISP_OUT_YVYU,
    MMP_ISP_OUT_UYVY,
    MMP_ISP_OUT_VYUY,
    MMP_ISP_OUT_YUV422,
    MMP_ISP_OUT_YUV420,
    MMP_ISP_OUT_YUV444,
    MMP_ISP_OUT_YUV422R,
    MMP_ISP_OUT_NV12,
    MMP_ISP_OUT_NV21,
} MMP_ISP_OUTFORMAT;

/**
 * Input image format.
 *
 * @see mmpIspPlayImageProcess mmpIspPlayVideoProcess
 */
typedef enum MMP_ISP_INFORMAT_TAG
{
    //ISP Input Format
    MMP_ISP_IN_YUV422,
    MMP_ISP_IN_YUV420,
    MMP_ISP_IN_YUV444,
    MMP_ISP_IN_YUV422R,
    MMP_ISP_IN_YUYV,
    MMP_ISP_IN_YVYU,
    MMP_ISP_IN_UYVY,
    MMP_ISP_IN_VYUY,
    MMP_ISP_IN_RGB565,
    MMP_ISP_IN_RGB888,
    MMP_ISP_IN_NV12,
    MMP_ISP_IN_NV21,
} MMP_ISP_INFORMAT;

/**
 * ISP deinterlace type.
 *
 * @see mmpIspSetDeinterlaceType()
 */
typedef enum MMP_ISP_DEINTERLACE_TYPE_TAG
{
    MMP_ISP_3D_DEINTERLACE,             /**< 3D deinterlace */
    MMP_ISP_2D_DEINTERLACE              /**< 2D deinterlace */
} MMP_ISP_DEINTERLACE_TYPE;

//=============================================================================
//                              Structure Definition
//=============================================================================
/**
 * ISP core setting.
 *
 * @see mmpIspSetCore()
 */
typedef struct MMP_ISP_CORE_INFO_TAG
{
    MMP_BOOL                EnPreview;
    MMP_ISP_PRESCALE_SELECT PreScaleSel;
} MMP_ISP_CORE_INFO;

/**
 * Share data between JPEG and ISP.
 *
 * @see mmpIspPlayImageProcess mmpIspPlayVideoProcess
 */
typedef struct MMP_ISP_SHARE_TAG
{
    MMP_UINT32       addrY;
    MMP_UINT32       addrV;
    MMP_UINT32       addrU;
    MMP_UINT32       addrYp;
    MMP_UINT32       addrVp;
    MMP_UINT32       addrUp;
    MMP_UINT16       width;                 //!< input image width
    MMP_UINT16       height;                //!< input image height
    MMP_UINT16       pitchY;
    MMP_UINT16       pitchUv;
    MMP_UINT16       sliceCount;
    MMP_UINT16       rawsliceCount;
    MMP_BOOL         isAdobe_CMYK;
    MMP_ISP_INFORMAT format;                //!< input image format
    MMP_UINT16       Mpeg2TopBufferIndex;
    MMP_UINT16       Mpeg2BotBufferIndex;
    MMP_UINT32       top_field_first;
    MMP_UINT32       isProgressive;
    MMP_UINT16       EnableKeepLastField;
    MMP_UINT16       Blank_Buffer_Index;
} MMP_ISP_SHARE;

/**
 * ISP output information.
 *
 * @see mmpIspSetOutputWindow mmpIspSetPVOutputWindow
 */
typedef struct MMP_ISP_OUTPUT_INFO_TAG
{
    // for YUV output
    MMP_UINT32        addrAlloc;
    MMP_UINT32        addrY;
    MMP_UINT32        addrV;
    MMP_UINT32        addrU;
    MMP_UINT16        pitchY;
    MMP_UINT16        pitchUv;

    // for RGB output
    MMP_UINT32        addrRGB;
    MMP_UINT16        pitchRGB;

    // general output info
    MMP_INT           startX;
    MMP_INT           startY;
    MMP_UINT32        width;
    MMP_UINT32        height;

    MMP_ISP_OUTFORMAT format;
} MMP_ISP_OUTPUT_INFO;

/**
 * ISP Write Buffer information.
 *
 * @see mmpIspSetWriteBufferInfo()
 */
typedef struct MMP_ISP_WRITE_BUFFER_INFO_TAG
{
    MMP_UINT32 addrYRGB_1;
    MMP_UINT32 addrV_1;
    MMP_UINT32 addrU_1;
    MMP_UINT32 addrYRGB_2;
    MMP_UINT32 addrV_2;
    MMP_UINT32 addrU_2;
    MMP_UINT32 addrYRGB_3;
    MMP_UINT32 addrV_3;
    MMP_UINT32 addrU_3;
    MMP_UINT32 addrYRGB_4;
    MMP_UINT32 addrV_4;
    MMP_UINT32 addrU_4;
} MMP_ISP_WRITE_BUFFER_INFO;

//=============================================================================
//                Macro Definition
//=============================================================================

//=============================================================================
//                Structure Definition
//=============================================================================
/**
 * ISP instance.
 *
 * @see mmpIspInitialize mmpIspTerminate
 */
typedef void *ISP_DEVICE;

/**
 * The callback function of ISP Interrupt.
 *
 * @see mmpISPRegisterIRQ
 */
typedef void (*ISP_handler)(void *arg);

//=============================================================================
//                Global Data Definition
//=============================================================================

//=============================================================================
//                Private Function Definition
//=============================================================================

//=============================================================================
//                Public Function Definition
//=============================================================================
//=============================================================================
/**
 * ISP initialization.
 *
 * @param ispCore  indicate which ISP core will run.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @remark Application must call this API first when it want to use DSC API.
 *
 * @see MMP_ISP_MODE
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspInitialize(
    ISP_DEVICE   *ptDev,
    MMP_ISP_CORE ispCore);

//=============================================================================
/**
 * ISP powerdown.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @remark Application must call this API when leaving DSC module.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspPowerdown(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * ISP resetengine.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @remark Application must call this API when leaving DSC module.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspResetEngine(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * ISP terminate.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @remark Application must call this API when leaving DSC module.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspTerminate(
    ISP_DEVICE *ptDev);

//=============================================================================
/**
 * Set or change ISP multiple output.
 *
 * @param coreInfo  indicate which core setting will run.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @see MMP_ISP_CORE_INFO
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetCore(
    ISP_DEVICE        ptDev,
    MMP_ISP_CORE_INFO *coreInfo);

//=============================================================================
/**
 * Set or change ISP mode.
 *
 * @param ispMode  indicate which mode will be run.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @remark For snapshot usage, application first set MMP_ISP_MODE_PREVIEW_SNAPSHOT
 * mode to preview, then use MMP_ISP_MODE_SNAPSHOT mode to snapshot and finally
 * back to MMP_ISP_MODE_PREVIEW_SNAPSHOT mode for preview. It is quite possible to
 * transition repeatedly between modes: MMP_ISP_MODE_PREVIEW_SNAPSHOT ->
 * MMP_ISP_MODE_SNAPSHOT -> MMP_ISP_MODE_PREVIEW_SNAPSHOT -> MMP_ISP_MODE_SNAPSHOT
 * -> MMP_ISP_MODE_PREVIEW_SNAPSHOT -> mmpIspTerminate. Video record has the similar
 * behavior. To show image, MMP_ISP_MODE_SHOW_IMAGE should be set, and it will not
 * transition between other modes. To play video, MMP_ISP_MODE_PLAY_VIDEO should be
 * set. The same with show image, MMP_ISP_MODE_PLAY_VIDEO will not transition between
 * other modes.
 *
 * @see MMP_ISP_MODE
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetMode(
    ISP_DEVICE   ptDev,
    MMP_ISP_MODE ispMode);

//=============================================================================
/**
 * Enable specific ISP function.
 *
 * @param cap  Specifies a symbolic constant indicating a ISP function.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @see MMP_ISP_ATTRIBUTE mmpIspDisable
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspEnable(
    ISP_DEVICE   ptDev,
    MMP_ISP_CAPS cap);

//=============================================================================
/**
 * Disable specific ISP function.
 *
 * @param cap  Specifies a symbolic constant indicating a ISP function.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @see MMP_ISP_ATTRIBUTE mmpIspEnable
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspDisable(
    ISP_DEVICE   ptDev,
    MMP_ISP_CAPS cap);

//=============================================================================
/**
 * Get specific ISP function enabled status.
 *
 * @param cap  Specifies a symbolic constant indicating a ISP function.
 * @return MMP_TRUE if function enabled, MMP_FALSE if function disable.
 */
//=============================================================================
ISP_API MMP_BOOL
mmpIspQuery(
    ISP_DEVICE   ptDev,
    MMP_ISP_CAPS cap);

//=============================================================================
/**
 * Set the value of the specified color control type.
 *
 * @param colorControl  Specifies color control type.
 * @param value         The adjusted value related to specified color control.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @remark Calling the mmpIspUpdate() method to active it.
 *
 * @see MMP_ISP_COLOR_CONTROL
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetColorCtrl(
    ISP_DEVICE         ptDev,
    MMP_ISP_COLOR_CTRL colorControl,
    MMP_INT            value);

//=============================================================================
/**
 * Set Extend Destination.
 *
 * @param TopPixel    extend pixel of top.
 * @param DownPixel   extend pixel of down.
 * @param LeftPixel   extend pixel of left.
 * @param RightPixel  extend pixel of right.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetExtendDestination(
    ISP_DEVICE ptDev,
    MMP_UINT   LeftPixel,
    MMP_UINT   RightPixel,
    MMP_UINT   TopPixel,
    MMP_UINT   DownPixel);

//=============================================================================
/**
 * Set Extend Source.
 *
 * @param TopPixel    extend pixel of top.
 * @param DownPixel   extend pixel of down.
 * @param LeftPixel   extend pixel of left.
 * @param RightPixel  extend pixel of right.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetExtendSource(
    ISP_DEVICE ptDev,
    MMP_UINT   LeftPixel,
    MMP_UINT   RightPixel,
    MMP_UINT   TopPixel,
    MMP_UINT   DownPixel);

//=============================================================================
/**
 * Set display window.
 *
 * @param vramAddr  base address of the destination buffer.
 * @param startX    starting X position at the output buffer.
 * @param startY    starting Y position at the output buffer.
 * @param width     width of content at the output buffer.
 * @param height    height of content at the output buffer.
 * @param pitch     pitch of the destination buffer.
 * @param format    format of the destination buffer.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetOutputWindow(
    ISP_DEVICE          ptDev,
    MMP_ISP_OUTPUT_INFO *outInfo);

//=============================================================================
/**
 * Set preview display window.
 *
 * @param vramAddr  base address of the destination buffer.
 * @param startX    starting X position at the output buffer.
 * @param startY    starting Y position at the output buffer.
 * @param width     width of content at the output buffer.
 * @param height    height of content at the output buffer.
 * @param pitch     pitch of the destination buffer.
 * @param format    format of the destination buffer. only support RGB and YUV Packet.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetPVOutputWindow(
    ISP_DEVICE          ptDev,
    MMP_ISP_OUTPUT_INFO *outInfo);

//=============================================================================
/**
 * Set display window.
 *
 * Display window refers to the output screen size of the display device.
 * If the display device is an LCD, its size refers to the output width and 
 * output height of the LCD.
 *
 * @param startX  starting X position for display.
 * @param startY  starting Y position for display.
 * @param width   width of window.
 * @param height  height of window.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @code
    ISP_DEVICE  isp_dev;
    int32_t     result;
    
    result = mmpIspInitialize(&isp_dev, MMP_ISP_CORE_0);
    if (result != 0)
        printf("mmpIspInitialize() error (0x%x) !!\n", result);
    
    mmpIspSetCore(isp_dev, MMP_NULL);
    mmpIspSetDisplayWindow(isp_dev, 0, 0, ithLcdGetWidth(), ithLcdGetHeight());
    mmpIspTerminate(&isp_dev);
 * @endcode
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetDisplayWindow(
    ISP_DEVICE ptDev,
    MMP_INT    startX,
    MMP_INT    startY,
    MMP_UINT   width,
    MMP_UINT   height);

//=============================================================================
/**
 * Set Video Destination Window.
 *
 * @param startX  starting X position for video.
 * @param startY  starting Y position for video.
 * @param width   width of window.
 * @param height  height of window.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetVideoWindow(
    ISP_DEVICE ptDev,
    MMP_UINT   startX,
    MMP_UINT   startY,
    MMP_UINT   width,
    MMP_UINT   height);

//=============================================================================
/**
 * Set preview display window.
 *
 * @param startX  starting X position for display.
 * @param startY  starting Y position for display.
 * @param width   width of window.
 * @param height  height of window.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetPVVideoWindow(
    ISP_DEVICE ptDev,
    MMP_UINT   startX,
    MMP_UINT   startY,
    MMP_UINT   width,
    MMP_UINT   height);

//=============================================================================
/**
 * Set isp output format.
 *
 * @param format  format of the destination buffer.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 **/
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetOutputFormat(
    ISP_DEVICE        ptDev,
    MMP_ISP_OUTFORMAT format);

//=============================================================================
/**
 * Set preview output format of isp.
 *
 * @param format  format of the destination buffer. only support RGB and YUV Packet.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 **/
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetPVOutputFormat(
    ISP_DEVICE        ptDev,
    MMP_ISP_OUTFORMAT format);

//=============================================================================
/**
 * JPEG decoder use this interface to set the buffer information.
 *
 * @param data  share data between JPEG and DSC.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspPlayImageProcess(
    ISP_DEVICE          ptDev,
    const MMP_ISP_SHARE *data);

//=============================================================================
/**
 * MPEG decoder use this interface to set the buffer information.
 *
 * @param data  share data between MPEG and ISP.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspPlayVideoProcess(
    ISP_DEVICE          ptDev,
    const MMP_ISP_SHARE *data);

//=============================================================================
/**
 * MPEG decoder use this interface to set the buffer information.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspUpdateUIProcess(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Set Write Buffer information.
 *
 * @param wbInfo  destination address of Write Buffer
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetWriteBufferInfo(
    ISP_DEVICE                ptDev,
    MMP_ISP_WRITE_BUFFER_INFO *wbInfo);

//=============================================================================
/**
 * Set Write Buffer amount of ISP Flip.
 *
 * @param bufIndex  Set the buffer amount, 00: BufferA 01: BufferB, 10: BufferC.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetWriteBufferNum(
    ISP_DEVICE ptDev,
    MMP_UINT   bufIndex);

//=============================================================================
/**
 * Set Write Buffer number of Software Flip.
 *
 * @param bufIndex  Set the current buffer, 00: BufferA 01: BufferB, 10: BufferC.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetSWWriteBufferNum(
    ISP_DEVICE ptDev,
    MMP_UINT   bufIndex);

//=============================================================================
/**
 * Set frame function background image information & color key. (For Direct Assign VRAM address. Ex.2D input)
 *
 * @param baseAddr       base address of the background image buffer.
 * @param id             DSC capability for frame function identification.
 * @param startX         x position of the background image.
 * @param startY         y position of the background image.
 * @param width          width of the background image.
 * @param height         height of the background image.
 * @param colorKeyR      color key for R channel.
 * @param colorKeyG      color key for G channel.
 * @param colorKeyB      color key for B channel.
 * @param EnAlphaBlend   enable Alpha Blending function.
 * @param constantAlpha  constant Alpha Value.
 * @param format         format of the picture & color key.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @see mmpIspEnable mmpIspDisable
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetFrameFunction(
    ISP_DEVICE       ptDev,
    MMP_ISP_CAPS     id,
    void             *vramAddr,
    MMP_UINT         startX,
    MMP_UINT         startY,
    MMP_UINT         width,
    MMP_UINT         height,
    MMP_UINT         pitch,
    MMP_UINT         colorKeyR,
    MMP_UINT         colorKeyG,
    MMP_UINT         colorKeyB,
    MMP_BOOL         EnAlphaBlend,
    MMP_UINT         constantAlpha,
    MMP_PIXEL_FORMAT format,
    MMP_UINT         uiBufferIndex);

//=============================================================================
/**
 * Set ui decompress frame function background image information & color key. (For Direct Assign VRAM address. Ex.2D input)
 *
 * @param baseAddr       base address of the background image buffer.
 * @param id             DSC capability for frame function identification.
 * @param startX         x position of the background image.
 * @param startY         y position of the background image.
 * @param width          width of the background image.
 * @param height         height of the background image.
 * @param colorKeyR      color key for R channel.
 * @param colorKeyG      color key for G channel.
 * @param colorKeyB      color key for B channel.
 * @param EnAlphaBlend   enable Alpha Blending function.
 * @param constantAlpha  constant Alpha Value.
 * @param format         format of the picture & color key.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @see mmpIspEnable mmpIspDisable
 */
//=============================================================================
ISP_RESULT
mmpIspSetUiDecFrameFunction(
    ISP_DEVICE       ptDev,
    MMP_ISP_CAPS     id,
    void             *vramAddr,
    MMP_UINT         startX,
    MMP_UINT         startY,
    MMP_UINT         width,
    MMP_UINT         height,
    MMP_UINT         pitch,
    MMP_UINT32       linebytes,
    MMP_UINT32       bitstreambytes,
    MMP_UINT         colorKeyR,
    MMP_UINT         colorKeyG,
    MMP_UINT         colorKeyB,
    MMP_BOOL         EnAlphaBlend,
    MMP_UINT         constantAlpha,
    MMP_PIXEL_FORMAT format,
    MMP_UINT         uiBufferIndex);

//=============================================================================
/**
 * Set blend function of constant value.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetBlenConstData(
    ISP_DEVICE   ptDev,
    MMP_ISP_CAPS id,
    MMP_BOOL     EnableGridData,
    MMP_UINT     GridPixelMode,
    MMP_UINT     ConstColorR0,
    MMP_UINT     ConstColorG0,
    MMP_UINT     ConstColorB0,
    MMP_UINT     ConstColorR1,
    MMP_UINT     ConstColorG1,
    MMP_UINT     ConstColorB1);

//=============================================================================
/**
 * Set subtitle background image information.
 *
 * @param id        frame function 0 or 1.
 * @param baseAddr  base address of the background image buffer.
 * @param width     width of the background image.
 * @param height    height of the background image.
 * @param pitch     pitch of the background image.
 * @param format    format of the picture & color key. only support RGB 888, RGB565.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @see mmpIspEnable mmpIspDisable
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetSubTitle(
    ISP_DEVICE              ptDev,
    MMP_UINT                id,
    void                    *baseAddr,
    MMP_UINT                startX,
    MMP_UINT                startY,
    MMP_UINT                width,
    MMP_UINT                height,
    MMP_ISP_SUBTITLE_FORMAT format);

//=============================================================================
/**
 * Set ui decompress subtitle background image information.
 *
 * @param id        frame function 0 or 1.
 * @param baseAddr  base address of the background image buffer.
 * @param width     width of the background image.
 * @param height    height of the background image.
 * @param pitch     pitch of the background image.
 * @param format    format of the picture & color key. only support RGB 888, RGB565.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @see mmpIspEnable mmpIspDisable
 */
//=============================================================================
ISP_RESULT
mmpIspSetUiDecSubTitle(
    ISP_DEVICE              ptDev,
    MMP_UINT                id,
    void                    *baseAddr,
    MMP_UINT                startX,
    MMP_UINT                startY,
    MMP_UINT                width,
    MMP_UINT                height,
    MMP_UINT32              linebytes,
    MMP_UINT32              bitstreambytes,
    MMP_ISP_SUBTITLE_FORMAT format);

//=============================================================================
/**
 * Set clipping window (only for display).
 *
 * @param id      DSC capability for clipping function identification.
 * @param type    Specify the clip type. Clip inside or outside.
 * @param startX  x coordinate of clipping window.
 * @param startY  y coordinate of clipping window.
 * @param width   width of clipping window.
 * @param height  height of clipping window.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetClipWindow(
    ISP_DEVICE        ptDev,
    MMP_ISP_CAPS      id,
    MMP_ISP_CLIP_TYPE type,
    MMP_UINT          startX,
    MMP_UINT          startY,
    MMP_UINT          width,
    MMP_UINT          height);

//=============================================================================
/**
 * Rotate image +90, -90, 180, flip, or mirror. It will apply to display.
 *
 * @param type  Specify the rotation type.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetDisplayRotateType(
    ISP_DEVICE          ptDev,
    MMP_ISP_ROTATE_TYPE type);

//=============================================================================
/**
 * Set image information in OnFly mode.
 *
 * @param baseAddr  base address of the image buffer.
 * @param startX    x coordinate of clipping window.
 * @param startY    y coordinate of clipping window.
 * @param width     width of clipping window.
 * @param height    height of clipping window.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 *
 * @see mmpIspEnable mmpIspDisable
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetOnFlyWriteMemFunction(
    ISP_DEVICE ptDev,
    void       *vramAddr,
    MMP_UINT   startX,
    MMP_UINT   startY,
    MMP_UINT   width,
    MMP_UINT   height);

//=============================================================================
/**
 * Enable LCD OnFly.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspEnableLCDOnFlyMode(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Disable LCD OnFly.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspDisableLCDOnFlyMode(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Enable Capture OnFly.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspEnableCAPOnFlyMode(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Disable Capture OnFly.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspDisableCAPOnFlyMode(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Wait ISP Engine Idle.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspWaitEngineIdle(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Query ISP Engine Idle.
 *
 * @return MMP_TRUE if engine is idle, MMP_FALSE otherwise.
 */
//=============================================================================
ISP_API MMP_BOOL
mmpIspIsEngineIdle(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Wait ISP Change Idle.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspWaitChangeIdle(
    void);

//=============================================================================
/**
 * Wait ISP Slice Buffer Empty.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspWaitRawSliceBufferEmpty(
    void);

//=============================================================================
/**
 * Driver Write Raw Data to Slice Buffer.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspWriteDataToSliceBuf(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Wait ISP UI Buffer Change.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspWaitUIBufferChange(
    ISP_DEVICE ptDev,
    MMP_UINT   uiBufferIndex);

//=============================================================================
/**
 * Register ISP IRQ.
 *
 * @param isphandler  The callback function to handle ISP Interrupt.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpISPRegisterIRQ(
    ISP_DEVICE  ptDev,
    ISP_handler isphandler);

//=============================================================================
/**
 * Enable ISP CMDQ.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspEnableCMDQ(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Disable ISP CMDQ.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspDisableCMDQ(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Enable ISP Interrupt.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspEnableInterrupt(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Disable ISP Interrupt.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspDisableInterrupt(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Clear ISP Interrupt.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspClearInterrupt(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Enable ISP Motion Detection.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspEnableMotionDetection(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Disable ISP Motion Detection.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspDisableMotionDetection(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Set deinterlace type. Only support YUV 420 input.
 *
 * @param type  Specify the deinterlace type.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetDeinterlaceType(
    ISP_DEVICE               ptDev,
    MMP_ISP_DEINTERLACE_TYPE type);

//=============================================================================
/**
 * Enable ISP ISR Fire.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspEnableISRFire(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Disable ISP ISR Fire.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspDisableISRFire(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Set Slice amount of H.264 Low Latency.
 *
 * @param sliceCount  Set the slice amount between H.264 and ISP.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetLowLatencySlice(
    ISP_DEVICE ptDev,
    MMP_UINT   sliceCount);

//=============================================================================
/**
 * Enable H.264 Low Latency.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspEnableLowLatency(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Disable H.264 Low Latency.
 *
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspDisableLowLatency(
    ISP_DEVICE ptDev);

//=============================================================================
/**
 * Set blank display window via CMDQ.
 *
 * @param vramAddr  base address of the destination buffer.
 * @param startX    starting X position at the output buffer.
 * @param startY    starting Y position at the output buffer.
 * @param width     width of content at the output buffer.
 * @param height    height of content at the output buffer.
 * @param pitch     pitch of the destination buffer.
 * @param format    format of the destination buffer.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetBlankOutputWindow(
    ISP_DEVICE          ptDev,
    MMP_ISP_OUTPUT_INFO *outInfo,
    MMP_ISP_OUTPUT_INFO *PVoutInfo);

#if (CFG_CHIP_FAMILY == 960)
//=============================================================================
/**
 * Set constant Alpha of RGB output.
 *
 * @param constantAlpha  constant Alpha Value.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspSetOutputRGBAlpha(
    ISP_DEVICE ptDev,
    MMP_UINT   constantAlpha);

//=============================================================================
/**
 * Get Write Buffer index of ISP Flip.
 *
 * @param currentIndex  return current index of Write Buffer that ISP recorded.
 * @param finishIndex   return finish index of Write Buffer that ISP recorded.
 * @return ISP_SUCCESS if succeed, error codes of ISP_ERR_ERROR otherwise.
 */
//=============================================================================
ISP_API ISP_RESULT
mmpIspGetWriteBufferIndex(
    ISP_DEVICE ptDev,
    MMP_UINT   *currentIndex,
    MMP_UINT   *finishIndex);
#endif

/** @} */ // end of ith_isp

#ifdef __cplusplus
}
#endif

#endif