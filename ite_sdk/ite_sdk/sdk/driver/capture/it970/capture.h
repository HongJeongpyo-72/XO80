#ifndef __ISP_H__
#define __ISP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <ite/itp.h>
#include "mmp_capture.h"
#include "capture_types.h"

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

//=============================================================================
//                Private Function Definition
//=============================================================================

//=============================================================================
//                Public Function Definition
//=============================================================================
//=============================================================================
/**
 * CAP memory Initialize.
 */
//=============================================================================
MMP_RESULT
Cap_Memory_Initialize(
    CAPTURE_HANDLE *ptDev, CAPTURE_SETTING info);

//=============================================================================
/**
 * CAP memory clear.
 */
//=============================================================================
MMP_RESULT
Cap_Memory_Clear(
    CAPTURE_HANDLE *ptDev);

//=============================================================================
/**
 * Update CAP device.
 */
//=============================================================================
MMP_RESULT
Cap_Update_Reg(
    CAPTURE_HANDLE *ptDev);

//=============================================================================
/**
 * CAP default value initialization.
 */
//=============================================================================
MMP_RESULT
Cap_Initialize(
    CAP_CONTEXT *Capctxt);

void
Cap_SetUserConfig(
    CAPTURE_HANDLE *ptDev, MMP_UINT32* Cptr);

//=============================================================================
/**
 * Update CAP device.
 */
//=============================================================================
MMP_RESULT
Cap_SetSkipMode(
    CAPTURE_HANDLE *ptDev, CAP_SKIP_MODE mode);

//=============================================================================
/**
   // brightness:     -128 ~ 127     default : 0
   // contrast:       0.0 ~ 4.0      default : 1.0
   // hue:            0 ~ 359        default : 0
   // saturation:     0.0 ~ 4.0      default : 1.0
   // colorEffect[2]: -128 ~ 128     default : 0, 0

   // preOff:  S8
   // M:       S4.8
   // postOff: S8
 */
//=============================================================================
#if defined (CAP_USE_COLOR_EFFECT)
void
Cap_SetColorCorrMatrix(
    CAP_COLOR_CORRECTION    *pColorCorrect,
    MMP_INT32               brightness,
    MMP_FLOAT               contrast,
    MMP_INT32               hue,
    MMP_FLOAT               saturation,
    MMP_INT32               colorEffect[2]);

//=============================================================================
/**
 * Update Capture color matrix device.
 *
 * @return MMP_RESULT_SUCCESS if succeed, error codes of Cap_RESULT_ERROR otherwise.
 */
//=============================================================================
MMP_RESULT
Cap_UpdateColorMatrix(
    CAPTURE_HANDLE *ptDev);

#endif

#ifdef __cplusplus
}
#endif

#endif