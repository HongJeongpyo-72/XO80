#ifndef _VIDEO_ENCODER_H_
#define _VIDEO_ENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define VIDEO_ENCODER_API extern

//#include "capture/capture_970/capture_types.h"
#include "capture/capture_970/mmp_capture.h"
#include "isp/mmp_isp.h"
#include "jpg/ite_jpg.h"
#include <ortp/str_utils.h>

#define MAX_USER_NUM 16
#define MAX_VIDEO_USER_NUM 4

#ifdef CFG_IPTV_TX
//#define LOW_LATENCY
#define MULTI_SLICE
#else
#ifndef CFG_DUAL_STREAM
//#define LOW_LATENCY
//#define MULTI_SLICE
#endif
#endif

typedef struct VIDEO_ENCODE_PARAMETER_TAG
{
    MMP_UINT32  width;
    MMP_UINT32  height;
    MMP_UINT32  frameRate;
    MMP_UINT32  bitRate;
} VIDEO_ENCODE_PARAMETER;

typedef struct VIDEO_HEADER_INFO_TAG
{
    MMP_UINT32  streamIndex;
    MMP_UINT8*  SPSBuf;
    MMP_UINT8*  PPSBuf;
    MMP_UINT32  SPS_Size;
    MMP_UINT32  PPS_Size;
} VIDEO_HEADER_INFO;

typedef struct VIDEO_STREAM_INFO_TAG
{
    MMP_UINT32 streamIndex;
    MMP_UINT32 bufIndex;
    MMP_BOOL   bIFrame;
    MMP_UINT32 dataSize;
    MMP_UINT32 timeStamp;
    MMP_BOOL   bLastSlice;

#ifdef CFG_IPTV_TX
    MMP_UINT8* pData;
    MMP_UINT32 frameCount;
    MMP_UINT32 InstanceNum;
    
    mblk_t     *m_video;
    bool       bGetStream;

    MMP_UINT32 frameRate;
    MMP_UINT32 EnFrameRate;
    MMP_UINT32 width;
    MMP_UINT32 height;
    MMP_UINT32 frameType;
    MMP_UINT32 bitRate;
    MMP_BOOL   bSkipFrame;
    MMP_CAP_INPUT_INFO encoder_Info;
    MMP_BOOL   bColorRange;
#endif
} VIDEO_STREAM_INFO;

typedef struct JPEG_ENCODE_PARAMETER_TAG
{
    MMP_UINT8*  strmBuf;
    MMP_UINT32  strmBuf_size;
    MMP_UINT32  quality;
    MMP_UINT32  enSize;
} JPEG_ENCODE_PARAMETER;

typedef struct ISP_BUFFER_INFO_TAG
{
    MMP_UINT8*  pISPBufAdrY[5];
    MMP_UINT8*  pISPBufAdrU[5];
    MMP_UINT8*  pISPBufAdrY_2[5];
    MMP_UINT8*  pISPBufAdrU_2[5];    
}ISP_BUFFER_INFO;

typedef enum VIDEO_ENCODE_MODE_TAG
{
    ENCODER_WITH_PREVIEW_MODE,
    ONLYPREVIEW_MODE,
}VIDEO_ENCODE_MODE;

typedef void (*ENCODE_PARA_CALLBACK)(VIDEO_STREAM_INFO *streamInfo);
//=============================================================================
//                              Function Declaration
//=============================================================================

VIDEO_ENCODER_API void
VideoEncoder_Init(
   VIDEO_ENCODE_MODE mode);
   //VIDEO_ENCODE_PARAMETER* enPara);

VIDEO_ENCODER_API void
VideoEncoder_SetStreamBuf(
   MMP_UINT32 stramIndex,
   MMP_UINT32 bufIndex,
   MMP_UINT8* bufAdr);
   
VIDEO_ENCODER_API void
VideoEncoder_GetHdrInfo(
   VIDEO_HEADER_INFO* headInfo);

VIDEO_ENCODER_API void
VideoEncoder_SetStreamBufCallBack(
   ENCODE_PARA_CALLBACK encodeCallback);
       
VIDEO_ENCODER_API void
VideoEncoder_Open(
   MMP_UINT32 stramIndex);
   
VIDEO_ENCODER_API void
VideoEncoder_Close(
   void);

VIDEO_ENCODER_API MMP_BOOL
VideoEncoder_GetSreamstate(
    MMP_UINT8 stream_id);

VIDEO_ENCODER_API void 
VideoEncoder_SetSreamUserNum( 
    MMP_UINT8 stream_id,  
    MMP_BOOL state); 

VIDEO_ENCODER_API MMP_UINT8
VideoEncoder_GetSreamUserNum(
	MMP_UINT8 stream_id);

VIDEO_ENCODER_API void
VideoEncoder_SetWithPreview(
    MMP_BOOL preview);
    
VIDEO_ENCODER_API void
VideoEncoder_SetEncodeIdx(
    MMP_UINT8 index);

VIDEO_ENCODER_API void
JPEGEncodeFrame(
   JPEG_ENCODE_PARAMETER* enPara);

VIDEO_ENCODER_API ISP_BUFFER_INFO*
    VideoEncoder_GetISPBUFFERINFO();

VIDEO_ENCODER_API void VideoEncoder_SetWidthHeight(
    MMP_UINT32 width,
    MMP_UINT32 height);

VIDEO_ENCODER_API void VideoEncoder_SetFrameRate(
    MMP_CAP_FRAMERATE framerate);

VIDEO_ENCODER_API void VideoEncoder_SetBitRate(
    MMP_UINT32 bitrate);

VIDEO_ENCODER_API void VideoEncoder_GetWidthHeight(
    MMP_UINT32 *width,
    MMP_UINT32 *height);

VIDEO_ENCODER_API void VideoEncoder_GetFrameRate(
    MMP_CAP_FRAMERATE *framerate);

VIDEO_ENCODER_API MMP_UINT32 VideoEncoder_GetBitRate(
    void);

VIDEO_ENCODER_API void VideoEncoder_SetInterlanced(
    MMP_UINT32 interlanced);

VIDEO_ENCODER_API MMP_BOOL VideoEncoder_GetInterlanced(
    void);

VIDEO_ENCODER_API void VideoEncoder_SetLowLatency(
    MMP_BOOL low_latency);

VIDEO_ENCODER_API MMP_BOOL VideoEncoder_GetLowLatency(
    void);

VIDEO_ENCODER_API void VideoEncoder_SetCap_Info(
    MMP_CAP_INPUT_INFO capture_info);

VIDEO_ENCODER_API void VideoEncoder_Pause(
    MMP_UINT32 stramIndex);
    
VIDEO_ENCODER_API void VideoEncoder_Resume(
    MMP_UINT32 stramIndex); 

VIDEO_ENCODER_API void VideoEncoder_SetColorRange(
    MMP_BOOL color_range);

#endif //_VIDEO_ENCODER_H_   
