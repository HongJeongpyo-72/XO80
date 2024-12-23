#ifndef __CAP_CONFIG_H__
#define __CAP_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Constant Definition
//=============================================================================

//#define CAP_USE_COLOR_EFFECT

#ifdef CFG_SENSOR_ENABLE
//NULL
#else
#define VESA_TIMING_SOUPPORT
#endif

/*define use how many capture max = 2*/
#define CAP_DEVICE_ID_MAX 2
/*define use how many memory buffer */

#ifdef CFG_IPTV_TX
#define CAPTURE_MEM_BUF_COUNT 10
#else
#define CAPTURE_MEM_BUF_COUNT 6
#endif
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

#ifdef __cplusplus
}
#endif

#endif