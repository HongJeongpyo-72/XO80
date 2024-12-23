#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "mediastreamer2/mediastream.h"
#include "video_encoder/video_encoder_it970.h"

#include "h264encapi.h"
#include "ite/ith.h"
#include "capture_970/capture_types.h"
#include "capture_970/mmp_capture.h"
#include "mmp_sensor.h"
/*#define SENSOR_WIDTH 0
#define SENSOR_HEIGHT 0
#define SENSOR_BITRATE 0
#define SENSOR_WIDTH_2 0
#define SENSOR_HEIGHT_2 0
#define SENSOR_BITRATE_2 0*/
#include "ite/itp.h"

//=============================================================================
//                Constant Definition
//=============================================================================

//#define CFG_DUAL_STREAM
//#define TEST_MULTISTREAM
//#define VBR_ENABLE

#define CAPTURE_RESULT_QUEUE_SIZE   30
#define ENCODED_RESULT_QUEUE_SIZE   CAPTURE_RESULT_QUEUE_SIZE
#define QUEUE_EMPTY                 (-1)
#define QUEUE_NOT_EMPTY             (0)
#define TIMER_NUM                   4
#if defined(CFG_DUAL_STREAM)

    #define ISP_TRANSFORM_NUM       2
    #define ENCODE_STREAM_NUM       2
    #define VIDEO_STREAM_NUM        2
    #define VIDEO_SAMPLE_NUM        7

#else

    #define ISP_TRANSFORM_NUM       1
    #define ENCODE_STREAM_NUM       1
    #define VIDEO_STREAM_NUM        1
#ifdef MULTI_SLICE
    #define VIDEO_SAMPLE_NUM        30
#else
    #define VIDEO_SAMPLE_NUM        7
#endif

#endif

#ifdef CFG_IPTV_TX
#define CAPTURE_MAXWIDTH 1920
#define CAPTURE_MAXHEIGHT 1080
#else
#define CAPTURE_MAXWIDTH 1280
#define CAPTURE_MAXHEIGHT 720
#endif

#if !defined(CFG_SENSOR_ENABLE)
#ifdef CFG_IPTV_TX
#define SENSOR_WIDTH            CAPTURE_MAXWIDTH
#define SENSOR_HEIGHT			CAPTURE_MAXHEIGHT
#else
#define SENSOR_WIDTH            1280
#define SENSOR_HEIGHT           720
#endif
#define SENSOR_BITRATE          15000
#endif

#define VIDEO_FRAME_RATE_30     30
#define VIDEO_FRAME_RATE_25     25

#if defined (CFG_SENSOR_FLICK_50HZ)
    #define    VIDEO_FRAME_RATE VIDEO_FRAME_RATE_25
#else
    #define    VIDEO_FRAME_RATE VIDEO_FRAME_RATE_30
#endif

#define ISPBUFFERCOUNT 3

//=============================================================================
//                Macro Definition
//=============================================================================
typedef MMP_UINT (*frameCount2TimeStamp)(MMP_UINT framecount);

//=============================================================================
//                Structure Definition
//=============================================================================

typedef enum ENCODER_TYPE_TAG
{
    JPEG_ENCODER,
    H264_ENCODER
} ENCODER_TYPE;

typedef struct CAPTURE_RESULT_TAG
{
    MMP_UINT    resIdx;
    MMP_UINT    frameCount;
    MMP_UINT    timeStamp;
    MMP_UINT    frameIndex;
    MMP_BOOL    bTopField;
    MMP_BOOL    bCapToTran;
    MMP_BOOL    bTranToEnc;
} CAPTURE_RESULT;

typedef MMP_RESULT (*doEncodeDequeue)(CAPTURE_RESULT *ptEntry);

typedef struct AVC_ENCODER_FLOW_TAG
{
    MMP_UINT                baseTimeStamp;
    MMP_UINT                currTimeStamp;
    MMP_UINT                captureFrameCount;
    MMP_UINT                encodedFrameCount;
    MMP_UINT                encodedTimeStamp;
    MMP_CAP_FRAMERATE       frameRate;
    MMP_CAP_FRAMERATE       captureFrameRate;
    MMP_CAP_FRAMERATE       detectFrameRate;
//    FRAME_RATE_SETTING_STATE    frameRateState;
    frameCount2TimeStamp    pfToTimeStamp;
    doEncodeDequeue         pfEnDequeue;
    MMP_CAP_INPUT_INFO      encoderInfo;
    MMP_BOOL                bSkipFrame;
    MMP_UINT                ispFrameRate[ENCODE_STREAM_NUM];
    MMP_UINT                frameRateDiff[ENCODE_STREAM_NUM];
    MMP_UINT                frameCount[ENCODE_STREAM_NUM];
    MMP_UINT                skipCount[ENCODE_STREAM_NUM];
    MMP_UINT                frmRateChkCount;
    MMP_BOOL                encodeQue[5];
    MMP_UINT                maxFrmRateIdx;
} AVC_ENCODER_FLOW;

typedef struct CAPTURE_RESULT_QUEUE_TAG
{
    CAPTURE_RESULT  entry[CAPTURE_RESULT_QUEUE_SIZE];
    MMP_UINT        wIdx;
    MMP_UINT        rIdx;
} CAPTURE_RESULT_QUEUE;

typedef struct ENCODED_RESULT_TAG
{
    MMP_UINT    frameCount;
    MMP_UINT    timeStamp;
    MMP_UINT8   *pData;
    MMP_UINT    dataSize;
    MMP_UINT    InstanceNum;
} ENCODED_RESULT;

typedef struct ENCODED_RESULT_QUEUE_TAG
{
    ENCODED_RESULT  entry[ENCODED_RESULT_QUEUE_SIZE];
    MMP_UINT        wIdx;
    MMP_UINT        rIdx;
} ENCODED_RESULT_QUEUE;

/**
 *  Device Select
 */
typedef enum CAPTURE_DEVICE_TAG
{
    CAPTURE_DEV_UNKNOW          = MMP_CAP_UNKNOW_DEVICE,
    CAPTURE_DEV_HDMIRX          = MMP_CAP_DEV_HDMIRX,
    CAPTURE_DEV_SENSOR          = MMP_CAP_DEV_SENSOR,
} CAPTURE_DEVICE;

typedef struct ISP_TRANSFORM_PARAMETER_TAG
{
    MMP_UINT16  inWidth;
    MMP_UINT16  inHeight;
    MMP_UINT32  inAddrY[3];
    MMP_UINT32  inAddrUV[3];
    MMP_UINT16  outWidth;
    MMP_UINT16  outHeight;
    MMP_UINT32  outAddrY[8];
    MMP_UINT32  outAddrUV[8];
    MMP_BOOL    deinterlaceOn;
    MMP_BOOL    bframeDouble;
    MMP_BOOL    useTranBuf;
    MMP_UINT32  tranBufAddrY[3];
    MMP_UINT32  tranBufAddrUV[3];
} ISP_TRANSFORM_PARAMETER;

typedef struct H264_AVC_ENCODER_TAG
{
    H264EncInst encoder;

    MMP_UINT32  buf_vram_addr;
    MMP_UINT8*  buf_sys_addr;
    MMP_UINT32  hdr_buf_vram_addr;
    MMP_UINT8*  hdr_buf_sys_addr;
    MMP_UINT32  framecount;
    MMP_UINT8*  pHdrBufAddr[2];
    MMP_UINT8*  pStreamBufAdr[VIDEO_SAMPLE_NUM];
    MMP_UINT8*  pSourceBufAdrY[8];
    MMP_UINT8*  pSourceBufAdrU[8];
    MMP_UINT8*  pSourceBufAdrV[8];
    MMP_UINT32  ParaSetHdrSize[2];
    MMP_UINT32  streamBufCount;
    MMP_UINT32  streamBufSize;
    MMP_UINT32  sourceBufCount;
    MMP_UINT32  streamBufSelect;
    MMP_UINT32  sourceBufSelect;
    MMP_UINT32  frameWidth;
    MMP_UINT32  frameHeight;
    MMP_UINT32  framePitchY;
    MMP_UINT32  frameCropTop;
    MMP_UINT32  frameCropBottom;
    MMP_UINT32  frameCropLeft;
    MMP_UINT32  frameCropRight;
    MMP_BOOL    interlaced_frame;
    MMP_BOOL    bIFrame;
    MMP_BOOL    BufTBFieldMark[5];
} H264_AVC_ENCODER;

//=============================================================================
//                Global Data Definition
//=============================================================================

static H264_AVC_ENCODER         *gptAVCEncoder[ENCODE_STREAM_NUM];
static AVC_ENCODER_FLOW         gtAVCEncoderFlow;
static CAPTURE_RESULT_QUEUE     gtCaptureResultQ;
static ENCODED_RESULT_QUEUE     gtEncodedResultQ;
static CAPTURE_RESULT_QUEUE     gtIspResultQ;
static MMP_BOOL                 gbEncodeFire        = MMP_FALSE;
static MMP_BOOL                 gbVideoEncoderInit  = MMP_FALSE;
static MMP_UINT32               gEncodeIdx          = 0;
static MMP_BOOL                 gbStrBufFire[VIDEO_STREAM_NUM][VIDEO_SAMPLE_NUM];
static MMP_UINT32               gVideoSelIdx[VIDEO_STREAM_NUM];
static MMP_UINT32               gVideoWrIdx[VIDEO_STREAM_NUM];
static ENCODE_PARA_CALLBACK     gfpCallback                             = MMP_NULL;
static sem_t                    gpEncoderSem;
static MMP_BOOL                 gbVideoEncoder                          = MMP_FALSE;
static MMP_BOOL                 gbStream0Enable                         = MMP_FALSE;
static MMP_BOOL                 gbStream1Enable                         = MMP_FALSE;
static MMP_BOOL                 gbJPEGEncoder                           = MMP_FALSE;
static MMP_UINT32               gJPEGBufIndex                           = 0;
static MMP_UINT32               gOpenEnginCnt                           = 0;
static pthread_mutex_t          VideoEngineMutex                        = PTHREAD_MUTEX_INITIALIZER;

static ISP_TRANSFORM_PARAMETER  gtIspTransformParm[ISP_TRANSFORM_NUM]   = {0};
static CAPTURE_RESULT           gtIspFireEntry;
static MMP_UINT32               gVideoEnWidth[2];
static MMP_UINT32               gVideoEnHeight[2];
static MMP_UINT32               gVideoEnBitRate[2];
static MMP_BOOL                 gStartgetVideo[VIDEO_STREAM_NUM];
static MMP_BOOL                 gVideoEncoding[VIDEO_STREAM_NUM];
static MMP_UINT8                gVideoUserNum[VIDEO_STREAM_NUM];

static MMP_UINT8*               pISPBufAdrY[8];
static MMP_UINT8*               pISPBufAdrU[8];
static MMP_UINT8*               pISPBufAdrV[8];


MMP_UINT32                      tickStart;
MMP_UINT32                      tickISP;
CAPTURE_HANDLE                  cap0;

#ifdef   LOG_ENCODER_PERFORMANCE
MMP_UINT32                      tickStart, tickEnd;
unsigned                        ticks_per_ms;
MMP_UINT32                      Enidx = 0;
MMP_UINT32                      EnTime;
#endif

static pthread_t                gEncoder_process_tid;
static pthread_t                gIsp_process_tid;
static ISP_DEVICE               gIspDev = 0;
static MMP_UINT32               WriteBufferIndex = 0;
static MMP_BOOL                 gbEncodeStart = MMP_FALSE;

static MMP_UINT32               stream_buf_vram_addr = 0;
static MMP_UINT8*               stream_buf_sys_addr = 0;
static MMP_UINT8*               gLastSlicePtr = 0;
static MMP_UINT32               gSliceNum = 0;         
static VideoStream              *stream;

static MMP_UINT32               gEncode_Width = SENSOR_WIDTH;
static MMP_UINT32               gEncode_Height = SENSOR_HEIGHT;
static MMP_UINT32               gEncode_PitchY = SENSOR_WIDTH;
static MMP_UINT32               gEncode_PitchUV = SENSOR_WIDTH;

static MMP_CAP_FRAMERATE        gEncode_Framerate = MMP_CAP_FRAMERATE_60HZ;
static MMP_UINT32               gEncode_Bitrate = SENSOR_BITRATE;
static MMP_BOOL                 gEncode_Interlanced = MMP_FALSE;
static MMP_INT                  gTopBottomIndex = 0;  /*TOP FIELD :0 BOTTOM FIELD:1 */
static MMP_BOOL                 gEncode_LowLatency = MMP_FALSE;
static MMP_UINT32               gCapQueCnt = 0;
static MMP_BOOL                 gbSkipBotField = MMP_FALSE;
static MMP_CAP_INPUT_INFO       gCapture_info = MMP_CAP_INPUT_INFO_UNKNOWN;
static MMP_BOOL                 gbColorRange = MMP_FALSE; /* 0 : limit range 1: full range*/

#define DROP_DATA_RATE_THRESHOLD    (600*1024)
#define NORMAL_DATA_RATE_THRESHOLD  (2000*1024)
#define COUNT_DOWN_THRESHOLD        3

static MMP_UINT32               gPreVidSize = 0;
static MMP_UINT32               gCountDown = 0;
static MMP_UINT32               gbPrevIFrame = 0;
static MMP_UINT32               gVidSkipCnt = 0;
static MMP_UINT32               gDataSize = 0;
static MMP_UINT32               gbHalfDrop =0;
static MMP_UINT32               gChkDropTime = 0;
static MMP_UINT32               gChkNormalTime = 0;
static MMP_UINT32               gCountDownTime = 0;

static MMP_UINT32               gQpMin = 17;
static MMP_UINT32               gQpMax = 27;
static MMP_INT                  gMbQpAdjustment = -8;

//=============================================================================
//                Private Function Definition
//=============================================================================
static unsigned int PalGetClock(void)
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) != 0)
        printf("gettimeofday failed!\n");
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static unsigned long PalGetDuration(unsigned int clock)
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) != 0)
        printf("gettimeofday failed!\n");
    return (unsigned int)(tv.tv_sec * 1000 + tv.tv_usec / 1000) - clock;
}

// Capture Q function
static void
cap_isr(
    void *arg);

static void
VP_mem_isr(
    void *arg);

static void
VP_onfly_isr(
    void *arg);

static void*
isp_task(
    void *arg);

static void*
encoder_task(
    void *arg);

static MMP_BOOL
_Chk_Skip_Frame(
    MMP_UINT32 InstanceNum);

static int
_Chk_Cap_Fire(
    CAPTURE_HANDLE* ptDev);

static void 
_Update_Field_State(void);

static void 
_Set_Field_Reset(void);

static MMP_BOOL 
_Get_Field_State(void);

static MMP_INLINE void
_CaptureResultQ_Reset(
    void)
{
    gtCaptureResultQ.wIdx = gtCaptureResultQ.rIdx = 0;
}

static MMP_INLINE MMP_RESULT
_Check_CaptureResultQ(
    CAPTURE_RESULT *ptEntry)
{
    if (gtCaptureResultQ.wIdx == gtCaptureResultQ.rIdx)
        return (MMP_RESULT)QUEUE_EMPTY;

    ptEntry->resIdx     = gtCaptureResultQ.entry[gtCaptureResultQ.rIdx].resIdx;
    ptEntry->frameCount = gtCaptureResultQ.entry[gtCaptureResultQ.rIdx].frameCount;
    ptEntry->timeStamp  = gtCaptureResultQ.entry[gtCaptureResultQ.rIdx].timeStamp;
    ptEntry->frameIndex = gtCaptureResultQ.entry[gtCaptureResultQ.rIdx].frameIndex;
    ptEntry->bTopField  = gtCaptureResultQ.entry[gtCaptureResultQ.rIdx].bTopField;
    ptEntry->bCapToTran = gtCaptureResultQ.entry[gtCaptureResultQ.rIdx].bCapToTran;
    ptEntry->bTranToEnc = gtCaptureResultQ.entry[gtCaptureResultQ.rIdx].bTranToEnc;

    return (MMP_RESULT)QUEUE_NOT_EMPTY;
}

static MMP_INLINE MMP_RESULT
_CaptureResultQ_Dequeue(
    CAPTURE_RESULT *ptEntry)
{
    if (gtCaptureResultQ.wIdx == gtCaptureResultQ.rIdx)
        return (MMP_RESULT)QUEUE_EMPTY;

    ptEntry->resIdx     = gtCaptureResultQ.entry[gtCaptureResultQ.rIdx].resIdx;
    ptEntry->frameCount = gtCaptureResultQ.entry[gtCaptureResultQ.rIdx].frameCount;
    ptEntry->timeStamp  = gtCaptureResultQ.entry[gtCaptureResultQ.rIdx].timeStamp;
    ptEntry->frameIndex = gtCaptureResultQ.entry[gtCaptureResultQ.rIdx].frameIndex;
    ptEntry->bTopField  = gtCaptureResultQ.entry[gtCaptureResultQ.rIdx].bTopField;
    ptEntry->bCapToTran = gtCaptureResultQ.entry[gtCaptureResultQ.rIdx].bCapToTran;
    ptEntry->bTranToEnc = gtCaptureResultQ.entry[gtCaptureResultQ.rIdx].bTranToEnc;

    gtCaptureResultQ.rIdx++;
    if (gtCaptureResultQ.rIdx >= CAPTURE_RESULT_QUEUE_SIZE)
        gtCaptureResultQ.rIdx = 0;
    return (MMP_RESULT)QUEUE_NOT_EMPTY;
}

static MMP_INLINE void
_CaptureResultQ_Enqueue(
    CAPTURE_RESULT *ptEntry)
{
    gtCaptureResultQ.entry[gtCaptureResultQ.wIdx].resIdx        = ptEntry->resIdx;
    gtCaptureResultQ.entry[gtCaptureResultQ.wIdx].frameCount    = ptEntry->frameCount;
    gtCaptureResultQ.entry[gtCaptureResultQ.wIdx].timeStamp     = ptEntry->timeStamp;
    gtCaptureResultQ.entry[gtCaptureResultQ.wIdx].frameIndex    = ptEntry->frameIndex;
    gtCaptureResultQ.entry[gtCaptureResultQ.wIdx].bTopField     = ptEntry->bTopField;
    gtCaptureResultQ.entry[gtCaptureResultQ.wIdx].bCapToTran    = ptEntry->bCapToTran;
    gtCaptureResultQ.entry[gtCaptureResultQ.wIdx].bTranToEnc    = ptEntry->bTranToEnc;

    gtCaptureResultQ.wIdx++;
    if (gtCaptureResultQ.wIdx >= CAPTURE_RESULT_QUEUE_SIZE)
        gtCaptureResultQ.wIdx = 0;
}

static MMP_INLINE void
_IspResultQ_Reset(
    void)
{
    gtIspResultQ.wIdx = gtIspResultQ.rIdx = 0;
}

static MMP_INLINE MMP_RESULT
_IspResultQ_Dequeue(
    CAPTURE_RESULT *ptEntry)
{
    if (gtIspResultQ.wIdx == gtIspResultQ.rIdx)
        return (MMP_RESULT)QUEUE_EMPTY;

    ptEntry->resIdx     = gtIspResultQ.entry[gtIspResultQ.rIdx].resIdx;
    ptEntry->frameCount = gtIspResultQ.entry[gtIspResultQ.rIdx].frameCount;
    ptEntry->timeStamp  = gtIspResultQ.entry[gtIspResultQ.rIdx].timeStamp;
    ptEntry->frameIndex = gtIspResultQ.entry[gtIspResultQ.rIdx].frameIndex;
    ptEntry->bCapToTran = gtIspResultQ.entry[gtIspResultQ.rIdx].bCapToTran;
    ptEntry->bTranToEnc = gtIspResultQ.entry[gtIspResultQ.rIdx].bTranToEnc;

    gtIspResultQ.rIdx++;
    if (gtIspResultQ.rIdx >= CAPTURE_RESULT_QUEUE_SIZE)
        gtIspResultQ.rIdx = 0;
    return (MMP_RESULT)QUEUE_NOT_EMPTY;
}

static MMP_INLINE void
_IspResultQ_Enqueue(
    CAPTURE_RESULT *ptEntry)
{
    gtIspResultQ.entry[gtIspResultQ.wIdx].resIdx        = ptEntry->resIdx;
    gtIspResultQ.entry[gtIspResultQ.wIdx].frameCount    = ptEntry->frameCount;
    gtIspResultQ.entry[gtIspResultQ.wIdx].timeStamp     = ptEntry->timeStamp;
    gtIspResultQ.entry[gtIspResultQ.wIdx].frameIndex    = ptEntry->frameIndex;
    gtIspResultQ.entry[gtIspResultQ.wIdx].bCapToTran    = ptEntry->bCapToTran;
    gtIspResultQ.entry[gtIspResultQ.wIdx].bTranToEnc    = ptEntry->bTranToEnc;

    gtIspResultQ.wIdx++;
    if (gtIspResultQ.wIdx >= CAPTURE_RESULT_QUEUE_SIZE)
        gtIspResultQ.wIdx = 0;
}

static MMP_INLINE void
_EncodedResultQ_Reset(
    void)
{
    gtEncodedResultQ.wIdx = gtEncodedResultQ.rIdx = 0;
}

static MMP_INLINE MMP_RESULT
_EncodedResultQ_Dequeue(
    ENCODED_RESULT *ptEntry)
{
    //MMP_INT result;

    if (gtEncodedResultQ.wIdx == gtEncodedResultQ.rIdx)
        return (MMP_RESULT)QUEUE_EMPTY;

    ptEntry->frameCount     = gtEncodedResultQ.entry[gtEncodedResultQ.rIdx].frameCount;
    ptEntry->timeStamp      = gtEncodedResultQ.entry[gtEncodedResultQ.rIdx].timeStamp;
    ptEntry->pData          = gtEncodedResultQ.entry[gtEncodedResultQ.rIdx].pData;
    ptEntry->dataSize       = gtEncodedResultQ.entry[gtEncodedResultQ.rIdx].dataSize;
    ptEntry->InstanceNum    = gtEncodedResultQ.entry[gtEncodedResultQ.rIdx].InstanceNum;

    gtEncodedResultQ.rIdx++;
    if (gtEncodedResultQ.rIdx >= ENCODED_RESULT_QUEUE_SIZE)
        gtEncodedResultQ.rIdx = 0;
    return (MMP_RESULT)QUEUE_NOT_EMPTY;
}

static MMP_INLINE void
_EncodedResultQ_Enqueue(
    ENCODED_RESULT *ptEntry)
{
    gtEncodedResultQ.entry[gtEncodedResultQ.wIdx].frameCount    = ptEntry->frameCount;
    gtEncodedResultQ.entry[gtEncodedResultQ.wIdx].timeStamp     = ptEntry->timeStamp;
    gtEncodedResultQ.entry[gtEncodedResultQ.wIdx].pData         = ptEntry->pData;
    gtEncodedResultQ.entry[gtEncodedResultQ.wIdx].dataSize      = ptEntry->dataSize;
    gtEncodedResultQ.entry[gtEncodedResultQ.wIdx].InstanceNum   = ptEntry->InstanceNum;

    gtEncodedResultQ.wIdx++;
    if (gtEncodedResultQ.wIdx >= ENCODED_RESULT_QUEUE_SIZE)
        gtEncodedResultQ.wIdx = 0;
}

static void
_WaitAllQueue_Empty(
    void)
{
    CAPTURE_RESULT  tEntry  = {0};
    MMP_UINT32      timeOut = 0;

    while (QUEUE_NOT_EMPTY == _CaptureResultQ_Dequeue(&tEntry)) // && !gbEncodeFire)
    {
        usleep(30000);
        if (++timeOut > 10)
        {
            ithPrintf("wait QUEUE_NOT_EMPTY timeout %s() #%d\n", __FUNCTION__, __LINE__);
            break;
        }
    }

    while (QUEUE_NOT_EMPTY == _IspResultQ_Dequeue(&tEntry)) // && !gbEncodeFire)
    {
        usleep(30000);
        if (++timeOut > 10)
        {
            ithPrintf("wait QUEUE_NOT_EMPTY timeout %s() #%d\n", __FUNCTION__, __LINE__);
            break;
        }
    }
}

//Time Stamp function
static MMP_INLINE MMP_UINT
frameRate25Hz_frameCount2TimeStamp(
    MMP_UINT frameCount)
{
    return frameCount * 40;
}

static MMP_INLINE MMP_UINT
frameRate50Hz_frameCount2TimeStamp(
    MMP_UINT frameCount)
{
    return frameCount * 20;
}

static MMP_INLINE MMP_UINT
frameRate29_97Hz_frameCount2TimeStamp(
    MMP_UINT frameCount)
{
    static MMP_UINT timeStamp[30] = {
        0,  33,  66,
        100, 133, 166,
        200, 233, 266,
        300, 333, 366,
        400, 433, 466,
        500, 533, 566,
        600, 633, 666,
        700, 733, 766,
        800, 833, 866,
        900, 933, 967
    };
    return (frameCount / 30) * 1001 + timeStamp[frameCount % 30];
}

static MMP_INLINE MMP_UINT
frameRate59_94Hz_frameCount2TimeStamp(
    MMP_UINT frameCount)
{
    return frameRate29_97Hz_frameCount2TimeStamp(frameCount) / 2;
}

static MMP_INLINE MMP_UINT
frameRate30Hz_frameCount2TimeStamp(
    MMP_UINT frameCount)
{
    return (frameCount / 3) * 100 + (frameCount % 3) * 33;
}

static MMP_INLINE MMP_UINT
frameRate60Hz_frameCount2TimeStamp(
    MMP_UINT frameCount)
{
    return (frameCount / 3) * 50 + (frameCount % 3) * 33 / 2;
}

static MMP_INLINE MMP_UINT
frameRate23_97Hz_frameCount2TimeStamp(
    MMP_UINT frameCount)
{
    static MMP_UINT timeStamp[24] = {
        0,  41,  83,
        125, 166, 208,
        250, 291, 333,
        375, 417, 458,
        500, 542, 583,
        625, 667, 709,
        750, 792, 834,
        875, 917, 959
    };
    return (frameCount / 24) * 1001 + timeStamp[frameCount % 24];
}

static MMP_INLINE MMP_UINT
frameRate24Hz_frameCount2TimeStamp(
    MMP_UINT frameCount)
{
    static MMP_UINT timeStamp[12] = {
        0,  41,  83,
        125, 166, 208,
        250, 291, 333,
        375, 416, 458
    };
    return (frameCount / 12) * 500 + timeStamp[frameCount % 12];
}

static MMP_INLINE MMP_UINT
frameRateVESA30_frameCount2TimeStamp(
    MMP_UINT frameCount)
{
    //return gtAVSyncTimer.curAudTime + 33;
    return (frameCount / 3) * 100 + (frameCount % 3) * 33;
}

static MMP_INLINE MMP_UINT
frameRateVESA60_frameCount2TimeStamp(
    MMP_UINT frameCount)
{
    //return gtAVSyncTimer.curAudTime + 33;
    return (frameCount / 3) * 50 + (frameCount % 3) * 33 / 2;
}

static frameCount2TimeStamp _frameCount2TimeStamp_TABLE[16] =
{
    MMP_NULL,
    frameRate25Hz_frameCount2TimeStamp,
    frameRate50Hz_frameCount2TimeStamp,
    frameRate30Hz_frameCount2TimeStamp,
    frameRate60Hz_frameCount2TimeStamp,
    frameRate29_97Hz_frameCount2TimeStamp,
    frameRate59_94Hz_frameCount2TimeStamp,
    frameRate23_97Hz_frameCount2TimeStamp,
    frameRate24Hz_frameCount2TimeStamp,
    MMP_NULL,
    MMP_NULL,
    MMP_NULL,
    MMP_NULL,
    MMP_NULL,
    frameRateVESA30_frameCount2TimeStamp,
    frameRateVESA60_frameCount2TimeStamp
};

static MMP_INLINE frameCount2TimeStamp
_VIDEO_ENCODER_GetTimeStampConverter(
    MMP_CAP_FRAMERATE frameRate)
{
    frameCount2TimeStamp pf = MMP_NULL;

    printf("framerate:%d\n",frameRate);
    if (frameRate <= 15)
        pf = _frameCount2TimeStamp_TABLE[frameRate];

    return pf;
}

// Sensor and ISP function
MMP_RESULT
_CaptureAndVP_Init(
    void)
{
    MMP_RESULT result = MMP_SUCCESS;

    /* 4. ISP Init */
#ifdef CFG_LCD_PQ_TUNING
    mmpIspInitialize(&gIspDev, MMP_ISP_CORE_1);
#else
    mmpIspInitialize(&gIspDev, MMP_ISP_CORE_0);
#endif

    return result;
}

MMP_RESULT
_CaptureAndVP_Terminate(
    void)
{
    MMP_RESULT result = MMP_SUCCESS;
    mmpIspResetEngine(&gIspDev);
    mmpIspTerminate(&gIspDev);
    WriteBufferIndex = 0;
    _Set_Field_Reset();
    ithCapTerminate();
    return result;
}

MMP_RESULT
_CaptureAndVP_Fire(
    void)
{
    MMP_RESULT                result       = MMP_SUCCESS;
    MMP_ISP_SHARE             ispInput     = {0};
    MMP_ISP_CORE_INFO         ISPCOREINFO  = {0};
    MMP_ISP_OUTPUT_INFO       ispOutput    = {0};
    MMP_ISP_OUTPUT_INFO       ispOutput_PV = {0};
    MMP_ISP_WRITE_BUFFER_INFO ispWbInfo    = {0};
    MMP_UINT16              i, j;
    MMP_UINT16              timeout = 0;
    MMP_FLOAT               cap_scale;
    MMP_UINT8   R_V_16Pin[12] = {24,25,26,27,28,29,30,31,100,100,100,100}; 
    MMP_UINT8   G_Y_16Pin[12] = {16,17,18,19,20,21,22,23,100,100,100,100};
    MMP_UINT8   B_U_16Pin[12] = {7,8,9,10,12,13,14,15,100,100,100,100};    

    /* Cap get device information */
    ithCapGetDeviceInfo(&cap0);
    
    if(cap0.cap_info.YUVinfo.InputMode == 0x2)//RGB444
    {
        memcpy(&cap0.cap_info.inmux_info.Y_Pin_Num, &R_V_16Pin,   12);
        memcpy(&cap0.cap_info.inmux_info.U_Pin_Num, &G_Y_16Pin,   12);
        memcpy(&cap0.cap_info.inmux_info.V_Pin_Num, &B_U_16Pin,   12); 
    }
    else if(cap0.cap_info.YUVinfo.InputMode == 0x0)//YUV422
    {
        memcpy(&cap0.cap_info.inmux_info.Y_Pin_Num, &G_Y_16Pin,   12);
        memcpy(&cap0.cap_info.inmux_info.U_Pin_Num, &R_V_16Pin,   12);
        memcpy(&cap0.cap_info.inmux_info.V_Pin_Num, &B_U_16Pin,   12); 
    }
    else//YUV444
    {
        memcpy(&cap0.cap_info.inmux_info.Y_Pin_Num, &G_Y_16Pin,   12);
        memcpy(&cap0.cap_info.inmux_info.U_Pin_Num, &B_U_16Pin,   12);
        memcpy(&cap0.cap_info.inmux_info.V_Pin_Num, &R_V_16Pin,   12);
    }

    /*capture drop 4 frame after fire.*/
    cap0.cap_info.ininfo.VSyncSkip = 4;
    
    /* Cap paramete setting */
    ithCapParameterSetting(&cap0);

    memset(&gtAVCEncoderFlow, 0, sizeof(gtAVCEncoderFlow));
    gtAVCEncoderFlow.baseTimeStamp                      = 0;
    gtAVCEncoderFlow.frameRate                          = gtAVCEncoderFlow.captureFrameRate
                                                        = cap0.cap_info.ininfo.framerate
                                                        = gEncode_Framerate;

    gtAVCEncoderFlow.pfToTimeStamp                      = _VIDEO_ENCODER_GetTimeStampConverter(gtAVCEncoderFlow.frameRate);

#if defined(CFG_DUAL_STREAM)
    gtAVCEncoderFlow.pfEnDequeue                        = _IspResultQ_Dequeue;

    mmpIspResetEngine(gIspDev);
    WriteBufferIndex = 0;
    ISPCOREINFO.EnPreview = false;
    ISPCOREINFO.PreScaleSel = MMP_ISP_PRESCALE_NORMAL;
    mmpIspSetCore(gIspDev, &ISPCOREINFO);
    mmpIspSetMode(gIspDev, MMP_ISP_MODE_PLAY_VIDEO);
    /* Cap Register IRQ */
    ithCapRegisterIRQ(cap_isr, &cap0);
    mmpISPRegisterIRQ(gIspDev, VP_mem_isr);

    #if defined (CFG_SENSOR_FLICK_50HZ)
    gtAVCEncoderFlow.ispFrameRate[0]    = 25;
    gtAVCEncoderFlow.frameRateDiff[0]   = gtAVCEncoderFlow.ispFrameRate[0] - 25;      //EnPara.EnFrameRate;

    gtAVCEncoderFlow.ispFrameRate[1]    = 25;
    gtAVCEncoderFlow.frameRateDiff[1]   = gtAVCEncoderFlow.ispFrameRate[1] - 25;      //EnPara.EnFrameRate;
    #else
    gtAVCEncoderFlow.ispFrameRate[0]    = 30;
    gtAVCEncoderFlow.frameRateDiff[0]   = gtAVCEncoderFlow.ispFrameRate[0] - 30;      //EnPara.EnFrameRate;

    gtAVCEncoderFlow.ispFrameRate[1]    = 30;
    gtAVCEncoderFlow.frameRateDiff[1]   = gtAVCEncoderFlow.ispFrameRate[1] - 30;      //EnPara.EnFrameRate;
    #endif
    gtAVCEncoderFlow.frameCount[0]      = 1;
    gtAVCEncoderFlow.skipCount[0]       = 1;

    gtAVCEncoderFlow.frameCount[1]      = 1;
    gtAVCEncoderFlow.skipCount[1]       = 1;

    cap_scale                           = (MMP_FLOAT)cap0.cap_info.outinfo.OutWidth/ gptAVCEncoder[0]->frameWidth;

    if (cap_scale <= 1.6f)
    {
        cap0.cap_info.outinfo.OutWidth = gptAVCEncoder[0]->frameWidth;
    }

    for (i = 0; i < ISP_TRANSFORM_NUM; i++)
    {
        gtIspTransformParm[i].inWidth   = cap0.cap_info.outinfo.OutWidth;
        gtIspTransformParm[i].inHeight  = cap0.cap_info.outinfo.OutHeight;

        for (j = 0; j < 3; j++)
        {
            gtIspTransformParm[i].inAddrY[j]    = cap0.cap_info.OutAddrY[j];
            gtIspTransformParm[i].inAddrUV[j]   = cap0.cap_info.OutAddrUV[j];
        }
    }

    for (i = 0; i < ISP_TRANSFORM_NUM; i++)
    {
        for (j = 0; j < gptAVCEncoder[i]->sourceBufCount; j++)
        {
            gtIspTransformParm[i].outAddrY[j]   = (MMP_UINT32) gptAVCEncoder[i]->pSourceBufAdrY[j];
            gtIspTransformParm[i].outAddrUV[j]  = (MMP_UINT32) gptAVCEncoder[i]->pSourceBufAdrU[j];
        }

        gtIspTransformParm[i].outWidth      = gptAVCEncoder[i]->frameWidth;
        gtIspTransformParm[i].outHeight     = gptAVCEncoder[i]->frameHeight;

        //gptAVCEncoder[i]->interlaced_frame  = MMP_FALSE;
        gtIspTransformParm[i].deinterlaceOn = MMP_FALSE;
        gtIspTransformParm[i].bframeDouble  = MMP_FALSE;
    }
#else //else defined(CFG_DUAL_STREAM)
    gtAVCEncoderFlow.pfEnDequeue = _CaptureResultQ_Dequeue;

    mmpIspResetEngine(gIspDev);
    WriteBufferIndex = 0;
    ISPCOREINFO.EnPreview = false;
    ISPCOREINFO.PreScaleSel = MMP_ISP_PRESCALE_NORMAL;
    mmpIspSetCore(gIspDev, &ISPCOREINFO);
#ifdef LOW_LATENCY
    mmpIspSetLowLatencySlice(gIspDev, 4);
    mmpIspEnableLowLatency(gIspDev);
#endif
    mmpIspSetMode(gIspDev, MMP_ISP_MODE_PLAY_VIDEO);

    //Register IRQ
    mmpISPRegisterIRQ(gIspDev, VP_onfly_isr);

    #if defined (CFG_SENSOR_FLICK_50HZ)
    gtAVCEncoderFlow.ispFrameRate[0]    = 25;
    gtAVCEncoderFlow.frameRateDiff[0]   = gtAVCEncoderFlow.ispFrameRate[0] - 25;      //EnPara.EnFrameRate;
    #else
	#ifdef CFG_IPTV_TX
	gtAVCEncoderFlow.ispFrameRate[0]    = 60;
	#else
    gtAVCEncoderFlow.ispFrameRate[0]    = 30;
    #endif
	gtAVCEncoderFlow.frameRateDiff[0]   = gtAVCEncoderFlow.ispFrameRate[0] - 30;      //EnPara.EnFrameRate;
    #endif

    gtAVCEncoderFlow.frameCount[0]      = 1;
    gtAVCEncoderFlow.skipCount[0]       = 1;

    //ISP Setting
    ispInput.format = MMP_ISP_IN_NV12;
    ispInput.width = cap0.cap_info.outinfo.OutWidth;
    if(cap0.cap_info.ininfo.Interleave)
        ispInput.height = cap0.cap_info.outinfo.OutHeight / 2;
    else
        ispInput.height = cap0.cap_info.outinfo.OutHeight;
    ispInput.pitchY  = CAPTURE_MAXWIDTH;
    ispInput.pitchUv = CAPTURE_MAXWIDTH;

    // Signal Process Output Parameter
    for (i = 0; i < ISPBUFFERCOUNT; ++i)
    {
        switch (i)
        {
            case 0:
                ispOutput.addrY = (MMP_UINT32) pISPBufAdrY[i];
                ispOutput.addrU = (MMP_UINT32) pISPBufAdrU[i];
                break;

            case 1:
                ispWbInfo.addrYRGB_1 = (MMP_UINT32) pISPBufAdrY[i];
                ispWbInfo.addrU_1    = (MMP_UINT32) pISPBufAdrU[i];
                break;

            case 2:
                ispWbInfo.addrYRGB_2 = (MMP_UINT32) pISPBufAdrY[i];
                ispWbInfo.addrU_2    = (MMP_UINT32) pISPBufAdrU[i];
                break;

            case 3:
                ispWbInfo.addrYRGB_3 =  (MMP_UINT32) pISPBufAdrY[i];
                ispWbInfo.addrU_3    =  (MMP_UINT32) pISPBufAdrU[i];
                break;

            case 4:
                ispWbInfo.addrYRGB_4  = (MMP_UINT32) pISPBufAdrY[i];
                ispWbInfo.addrU_4     =  (MMP_UINT32) pISPBufAdrU[i];
                break;

            default:
                break;
        }
    }

    mmpIspSetWriteBufferInfo(gIspDev, &ispWbInfo);
    //ISP ring buffer should delete default 1 buffer.
    if (ISPBUFFERCOUNT > 0)
        mmpIspSetWriteBufferNum(gIspDev, ISPBUFFERCOUNT - 1);

    ispOutput.startX = 0;
    ispOutput.startY = 0;
    ispOutput.width = gptAVCEncoder[0]->frameWidth / 8;
    
    if(cap0.cap_info.ininfo.Interleave)
        ispOutput.height = gptAVCEncoder[0]->frameHeight / 2;
    else
        ispOutput.height = gptAVCEncoder[0]->frameHeight;

    ispOutput.format = MMP_ISP_OUT_NV12;
    ispOutput.pitchY = gptAVCEncoder[0]->framePitchY;
    ispOutput.pitchUv = gptAVCEncoder[0]->framePitchY;

    mmpIspEnableCAPOnFlyMode(gIspDev);
    mmpIspEnableInterrupt(gIspDev);
    mmpIspDisable(gIspDev, MMP_ISP_DEINTERLACE);
    mmpIspSetOutputWindow(gIspDev, &ispOutput);
    mmpIspSetVideoWindow(gIspDev, ispOutput.startX, ispOutput.startY, ispOutput.width, ispOutput.height);
    if(cap0.cap_info.bMatchResolution)
        mmpIspPlayImageProcess(gIspDev, &ispInput);

#endif //end defined(CFG_DUAL_STREAM)

    /* Cap check timing & fire */
    if(cap0.cap_info.bMatchResolution)
        result = _Chk_Cap_Fire(&cap0);

lab_end:
    return result;
}

// AVC Encoder function
static void
OpenEngine(ENCODER_TYPE type)
{
    MMP_UINT32 i;
    H264EncIn encIn;
    H264EncOut encOut;
    int ret;

    pthread_mutex_lock(&VideoEngineMutex);

    if (type == JPEG_ENCODER)
        gbJPEGEncoder = MMP_TRUE;

    if (type == H264_ENCODER)
        gbVideoEncoder = MMP_TRUE;

    if (!gbVideoEncoder)
        gbStream0Enable = MMP_TRUE;

    gOpenEnginCnt++;

    if (gOpenEnginCnt == 2)  // already open
        goto o_end;

    memset (&encIn, 0, sizeof(H264EncIn));
    encIn.pOutBuf = (u32 *)(gptAVCEncoder[0]->pHdrBufAddr[0]);
    encIn.busOutBuf = (u32 *)(gptAVCEncoder[0]->pHdrBufAddr[0]);
    encIn.outBufSize = 2048;

    /* Start stream */
    ret = H264EncStrmStart(gptAVCEncoder[0]->encoder, &encIn, &encOut);
    if(ret != H264ENC_OK)
    {
        printf("H264EncStrmStart(%d) failed.\n", ret);
        //return -1;
    }

    gptAVCEncoder[0]->ParaSetHdrSize[0] = encOut.pNaluSizeBuf[0];
    gptAVCEncoder[0]->ParaSetHdrSize[1] = encOut.pNaluSizeBuf[1];    
    if (encOut.numNalus == 3)
        gptAVCEncoder[0]->ParaSetHdrSize[1] += encOut.pNaluSizeBuf[2];
    gptAVCEncoder[0]->pHdrBufAddr[1] = gptAVCEncoder[0]->pHdrBufAddr[0] + encOut.pNaluSizeBuf[0];

    printf("SPS PPS stream %d %d %d %d\n", encOut.numNalus, encOut.pNaluSizeBuf[0], encOut.pNaluSizeBuf[1], encOut.pNaluSizeBuf[2]);

#if defined(CFG_DUAL_STREAM)
    memset (&encIn, 0, sizeof(H264EncIn));
    encIn.pOutBuf = (u32 *)(gptAVCEncoder[1]->pHdrBufAddr[0]);
    encIn.busOutBuf = (u32 *)(gptAVCEncoder[1]->pHdrBufAddr[0]);
    encIn.outBufSize = 2048;

    /* Start stream */
    ret = H264EncStrmStart(gptAVCEncoder[1]->encoder, &encIn, &encOut);
    if(ret != H264ENC_OK)
    {
        printf("H264EncStrmStart() failed. %x", ret);
        //return -1;
    }

    gptAVCEncoder[1]->ParaSetHdrSize[0] = encOut.pNaluSizeBuf[0];
    gptAVCEncoder[1]->ParaSetHdrSize[1] = encOut.pNaluSizeBuf[1];
    if (encOut.numNalus == 3)
        gptAVCEncoder[1]->ParaSetHdrSize[1] += encOut.pNaluSizeBuf[2];
    gptAVCEncoder[1]->pHdrBufAddr[1] = gptAVCEncoder[1]->pHdrBufAddr[0] + encOut.pNaluSizeBuf[0];

    printf("SPS PPS stream %d %d %d %d\n", encOut.numNalus, encOut.pNaluSizeBuf[0], encOut.pNaluSizeBuf[1], encOut.pNaluSizeBuf[2]);

#endif

    gbEncodeStart = MMP_TRUE;
    _CaptureAndVP_Fire();

#if defined(CFG_DUAL_STREAM)
    pthread_create(&gIsp_process_tid, NULL, isp_task, NULL);
#endif

#ifdef CFG_IPTV_TX
    pthread_attr_t attr;
    int rev, min_prior, max_prior;
    struct sched_param param;

    pthread_attr_init(&attr);
    pthread_attr_getschedparam(&attr, &param);
    max_prior = sched_get_priority_max(SCHED_RR);
    param.sched_priority = max_prior -2;
    rev = pthread_attr_setschedparam(&attr, &param);
    printf("### video sched_priority:%d\n",param.sched_priority);
    pthread_create(&gEncoder_process_tid, &attr, encoder_task, NULL);
#else
    pthread_create(&gEncoder_process_tid, NULL, encoder_task, NULL);
#endif

#ifdef LOW_LATENCY        //temp solution
    if (!gptAVCEncoder[0]->interlaced_frame)
    {
        CAPTURE_RESULT  tEntry          = {0};
        MMP_UINT32      WBIndex         = WriteBufferIndex;

        WriteBufferIndex++;
        WriteBufferIndex %= gptAVCEncoder[0]->sourceBufCount;

        tEntry.frameIndex = WBIndex;
        tEntry.frameCount   = ++gtAVCEncoderFlow.captureFrameCount;

        if (gtAVCEncoderFlow.pfToTimeStamp)
        {
            tEntry.timeStamp            = gtAVCEncoderFlow.currTimeStamp
                                        = gtAVCEncoderFlow.baseTimeStamp
                                        + gtAVCEncoderFlow.pfToTimeStamp(tEntry.frameCount);
        }

        _CaptureResultQ_Enqueue(&tEntry);
    }
#endif

o_end:
    pthread_mutex_unlock(&VideoEngineMutex);
}

static void
CloseEngine(ENCODER_TYPE type)
{
    MMP_UINT32 i;
    H264EncIn encIn;
    H264EncOut encOut;
    int ret;

    pthread_mutex_lock(&VideoEngineMutex);

    if (gOpenEnginCnt == 0)
    {
        goto c_end;
    }

    if (type == JPEG_ENCODER)
    {
        gbJPEGEncoder = MMP_FALSE;
        if (!gbVideoEncoder)
            gbStream0Enable = MMP_FALSE;
    }

    if (type == H264_ENCODER)
    {
        gbVideoEncoder = MMP_FALSE;
        gbStream0Enable = MMP_FALSE;
        gbStream1Enable = MMP_FALSE;
        //gbEncodeStart = MMP_FALSE;
    }

    gOpenEnginCnt--;

    if (gOpenEnginCnt == 1)
    {
        goto c_end;
    }

#if 1
    //wait encoder task finish
    while(gVideoEncoding[0] == MMP_TRUE)
    {
        printf("Wait video encode finished...\n");
        usleep(1000);
    }
#endif

    if (type == H264_ENCODER)
    {
        gbEncodeStart = MMP_FALSE;
    }
    
    _CaptureAndVP_Terminate();    
    _WaitAllQueue_Empty();

    // temp solution
    memset (&encIn, 0, sizeof(H264EncIn));
    encIn.pOutBuf = (u32 *)(gptAVCEncoder[0]->pHdrBufAddr[0]);
    encIn.busOutBuf = (u32 *)(gptAVCEncoder[0]->pHdrBufAddr[0]);
    encIn.outBufSize = 2048;

    ret = H264EncStrmEnd(gptAVCEncoder[0]->encoder, &encIn, &encOut);
    if(ret != H264ENC_OK)
    {
        printf("H264EncStrmEnd(%d) failed.\n", ret);
    }
    else
        printf("H264EncStrmEnd.\n");

    ret = H264EncRelease(gptAVCEncoder[0]->encoder);
    if(ret != H264ENC_OK)
    {
        printf("H264EncRelease(%d) failed.\n", ret);
    }
    else
        printf("H264EncRelease.\n");

#if defined(CFG_DUAL_STREAM)
    // temp solution
    memset (&encIn, 0, sizeof(H264EncIn));
    encIn.pOutBuf = (u32 *)(gptAVCEncoder[1]->pHdrBufAddr[0]);
    encIn.busOutBuf = (u32 *)(gptAVCEncoder[1]->pHdrBufAddr[0]);
    encIn.outBufSize = 2048;

    ret = H264EncStrmEnd(gptAVCEncoder[1]->encoder, &encIn, &encOut);

    if(ret != H264ENC_OK)
    {
        printf("H264EncStrmEnd() failed. %x", ret);
    }
    else
        printf("H264EncStrmEnd.\n");

    ret = H264EncRelease(gptAVCEncoder[1]->encoder);
    if(ret != H264ENC_OK)
    {
        printf("H264EncRelease(%d) failed.\n", ret);
    }
    else
        printf("H264EncRelease.\n");

#endif
    pthread_join(gEncoder_process_tid, NULL);
c_end:
    pthread_mutex_unlock(&VideoEngineMutex);
}

static int mappingFramerate(MMP_CAP_FRAMERATE framerate)
{
    switch(framerate)
    {
        case MMP_CAP_FRAMERATE_UNKNOW:
            return 0;
        case MMP_CAP_FRAMERATE_25HZ:
            return 25;
        case MMP_CAP_FRAMERATE_50HZ:
            return 50;
        case MMP_CAP_FRAMERATE_30HZ:
            return 30;
        case MMP_CAP_FRAMERATE_60HZ:
            return 60;
        case MMP_CAP_FRAMERATE_29_97HZ:
            return 30;
        case MMP_CAP_FRAMERATE_59_94HZ:
            return 60;
        case MMP_CAP_FRAMERATE_23_97HZ:
        case MMP_CAP_FRAMERATE_24HZ:
            return 24;
        case MMP_CAP_FRAMERATE_56HZ:
            return 56;
        case MMP_CAP_FRAMERATE_70HZ:
            return 70;
        case MMP_CAP_FRAMERATE_72HZ:
            return 72;
        case MMP_CAP_FRAMERATE_75HZ:
            return 75;
        case MMP_CAP_FRAMERATE_85HZ:
            return 85;
        default:
            return 60;
    }
}

void
VideoEncoder_Init(
    //VIDEO_ENCODE_PARAMETER* enPara)
    VIDEO_ENCODE_MODE mode)
{
    MMP_UINT32 i;
    uint32_t size;
    uint32_t tmp;
    H264EncRet ret;
    H264EncConfig cfg;
    H264EncCodingCtrl codingCfg;
    H264EncRateCtrl rcCfg;
    H264EncIn encIn;
    H264EncOut encOut;
    H264EncPreProcessingCfg preProcCfg;

#ifndef CFG_IPTV_TX
#if !defined(CFG_SENSOR_ENABLE)
    return;
#endif
#endif

    for (i = 0; i < ENCODE_STREAM_NUM; i++)
    {
        if (MMP_NULL == gptAVCEncoder[i])
            gptAVCEncoder[i] = (H264_AVC_ENCODER *)malloc(sizeof(H264_AVC_ENCODER));

        if (MMP_NULL != gptAVCEncoder[i])
            memset(gptAVCEncoder[i], 0, sizeof(H264_AVC_ENCODER));
    }


    gptAVCEncoder[0]->interlaced_frame = gEncode_Interlanced;
    if (gptAVCEncoder[0]->interlaced_frame)
        gptAVCEncoder[0]->sourceBufCount = 5;
    else
        gptAVCEncoder[0]->sourceBufCount = 5;
    
    gptAVCEncoder[0]->streamBufCount = VIDEO_SAMPLE_NUM;

    size = (gEncode_PitchY * gEncode_Height * 3 * ISPBUFFERCOUNT) >> 1;
    
    gptAVCEncoder[0]->buf_vram_addr = itpVmemAlignedAlloc(8, size);
    gptAVCEncoder[0]->buf_sys_addr = (uint8_t*) ithMapVram(gptAVCEncoder[0]->buf_vram_addr, size, /*ITH_VRAM_READ |*/ ITH_VRAM_WRITE);

    for (i = 0; i < ISPBUFFERCOUNT; i++)
    {
        pISPBufAdrY[i] = gptAVCEncoder[0]->buf_sys_addr + ((gEncode_PitchY * gEncode_Height * 3 * i)/2);
        pISPBufAdrU[i] = pISPBufAdrY[i] + (gEncode_PitchY * gEncode_Height);
        pISPBufAdrV[i] = pISPBufAdrU[i];
    }   

    for (i = 0; i < gptAVCEncoder[0]->sourceBufCount; i++)
    {
        gptAVCEncoder[0]->pSourceBufAdrY[i] = cap0.cap_info.OutAddrY[i];
        gptAVCEncoder[0]->pSourceBufAdrU[i] = cap0.cap_info.OutAddrUV[i];
        gptAVCEncoder[0]->pSourceBufAdrV[i] = cap0.cap_info.OutAddrUV[i];
        printf("++++++++init addr=(%x, %x, %x)\n", gptAVCEncoder[0]->pSourceBufAdrY[i], gptAVCEncoder[0]->pSourceBufAdrU[i], gptAVCEncoder[0]->pSourceBufAdrV[i]);
    }

    /*only display capture video , don't  allocate videoencoder  output buffer*/
    if(mode == ONLYPREVIEW_MODE)
        return;

    size = 20 * 1024;

    gptAVCEncoder[0]->hdr_buf_vram_addr = itpVmemAlignedAlloc(8, size);
    gptAVCEncoder[0]->hdr_buf_sys_addr = (uint8_t*) ithMapVram(gptAVCEncoder[0]->hdr_buf_vram_addr, size, /*ITH_VRAM_READ |*/ ITH_VRAM_WRITE);

    gptAVCEncoder[0]->pHdrBufAddr[0] = gptAVCEncoder[0]->hdr_buf_sys_addr;
    gptAVCEncoder[0]->pHdrBufAddr[1] = gptAVCEncoder[0]->hdr_buf_sys_addr + 10 * 1024;
    gptAVCEncoder[0]->ParaSetHdrSize[0] = 10 * 1024;
    gptAVCEncoder[0]->ParaSetHdrSize[1] = 10 * 1024;

#ifdef MULTI_SLICE
    //stream buffer for multi-slice && low latency mode
    size = 500*1024;
    stream_buf_vram_addr = itpVmemAlignedAlloc(8, size);
    stream_buf_sys_addr = (uint8_t*) ithMapVram(stream_buf_vram_addr, size, /*ITH_VRAM_READ |*/ ITH_VRAM_WRITE);
#endif

#if defined(CFG_DUAL_STREAM)
    gptAVCEncoder[1]->sourceBufCount = 3;
    gptAVCEncoder[1]->streamBufCount = VIDEO_SAMPLE_NUM;

    size = (SENSOR_WIDTH_2 * SENSOR_HEIGHT_2 * 3 * gptAVCEncoder[1]->sourceBufCount) >> 1;

    gptAVCEncoder[1]->buf_vram_addr = itpVmemAlignedAlloc(8, size);
    gptAVCEncoder[1]->buf_sys_addr = (uint8_t*) ithMapVram(gptAVCEncoder[1]->buf_vram_addr, size, /*ITH_VRAM_READ |*/ ITH_VRAM_WRITE);

    for (i = 0; i < gptAVCEncoder[1]->sourceBufCount; i++)
    {
        gptAVCEncoder[1]->pSourceBufAdrY[i] = gptAVCEncoder[1]->buf_sys_addr + ((SENSOR_WIDTH_2 * SENSOR_HEIGHT_2 * 3 * i)/2);
        gptAVCEncoder[1]->pSourceBufAdrU[i] = gptAVCEncoder[1]->pSourceBufAdrY[i] + (SENSOR_WIDTH_2 * SENSOR_HEIGHT_2);
        gptAVCEncoder[1]->pSourceBufAdrV[i] = gptAVCEncoder[1]->pSourceBufAdrU[i];
    }

    size = 20 * 1024;

    gptAVCEncoder[1]->hdr_buf_vram_addr = itpVmemAlignedAlloc(8, size);
    gptAVCEncoder[1]->hdr_buf_sys_addr = (uint8_t*) ithMapVram(gptAVCEncoder[1]->hdr_buf_vram_addr, size, /*ITH_VRAM_READ |*/ ITH_VRAM_WRITE);

    gptAVCEncoder[1]->pHdrBufAddr[0] = gptAVCEncoder[1]->hdr_buf_sys_addr;
    gptAVCEncoder[1]->pHdrBufAddr[1] = gptAVCEncoder[1]->hdr_buf_sys_addr + 10 * 1024;
    gptAVCEncoder[1]->ParaSetHdrSize[0] = 10 * 1024;
    gptAVCEncoder[1]->ParaSetHdrSize[1] = 10 * 1024;

#endif

    memset (&cfg, 0, sizeof(H264EncConfig));

    gptAVCEncoder[0]->frameWidth = cfg.width = gEncode_Width;

    gptAVCEncoder[0]->framePitchY = gEncode_PitchY;
    
    if (gptAVCEncoder[0]->interlaced_frame)
        gptAVCEncoder[0]->frameHeight = cfg.height = gEncode_Height;
    else
        gptAVCEncoder[0]->frameHeight = cfg.height = gEncode_Height;


    cfg.scaledWidth = 0;
    cfg.scaledHeight = 0;

#ifdef CFG_IPTV_TX
    cfg.frameRateDenom = 1;
    cfg.frameRateNum = 60;
#else
    if (VIDEO_FRAME_RATE == VIDEO_FRAME_RATE_25)
    {
        cfg.frameRateDenom = 1;
        cfg.frameRateNum = 25;
    } else {
        cfg.frameRateDenom = 1;
        cfg.frameRateNum = 30;
    }
#endif
    if (gptAVCEncoder[0]->interlaced_frame)
        cfg.viewMode = H264ENC_INTERLACED_FIELD;
    else
        cfg.viewMode = H264ENC_BASE_VIEW_DOUBLE_BUFFER; /* Two buffers by default */
    cfg.streamType = H264ENC_BYTE_STREAM; //H264ENC_NAL_UNIT_STREAM;

    tmp = ((((gptAVCEncoder[0]->frameWidth+15)>>4)<<4)* (((gptAVCEncoder[0]->frameHeight+15)>>4)<<4)) >> 8;

    if (tmp <= 396)
        cfg.level = H264ENC_LEVEL_2;
    else if (tmp <= 792)
        cfg.level = H264ENC_LEVEL_2_1;
    else if (tmp <= 1620)
        cfg.level = H264ENC_LEVEL_3;
    else if (tmp <= 3600)
        cfg.level = H264ENC_LEVEL_3_1;
    else if (tmp <= 5120)
        cfg.level = H264ENC_LEVEL_3_2;
    else if (tmp <= 8192)
        cfg.level = H264ENC_LEVEL_4;

    //cfg.level = H264ENC_LEVEL_4;

    cfg.refFrameCompress = 0;
    cfg.rfcLumBufLimit = 0;
    cfg.rfcChrBufLimit = 0;
    cfg.refFrameAmount = 1;

    gbVideoEncoder = MMP_TRUE;

    ithVideoHostReset();

    if((ret = H264EncInit(&cfg, &gptAVCEncoder[0]->encoder)) != H264ENC_OK)
    {
        printf("H264EncInit() failed. %x", ret);
        //return (int)ret;
    }

    /* Encoder setup: rate control */
    if((ret = H264EncGetRateCtrl(gptAVCEncoder[0]->encoder, &rcCfg)) != H264ENC_OK)
    {
        printf("H264EncGetRateCtrl() failed. %x", ret);
        //CloseEncoder(encoder);
        //return -1;
    }
    else
    {
        //printf("Get rate control: qp %2d [%2d, %2d]  %8d bps  "
        //       "pic %d mb %d skip %d  hrd %d\n  cpbSize %d gopLen %d "
        //       "intraQpDelta %2d\n",
        //       rcCfg.qpHdr, rcCfg.qpMin, rcCfg.qpMax, rcCfg.bitPerSecond,
        //       rcCfg.pictureRc, rcCfg.mbRc, rcCfg.pictureSkip, rcCfg.hrd,
        //       rcCfg.hrdCpbSize, rcCfg.gopLen, rcCfg.intraQpDelta);

#ifdef VBR_ENABLE
        rcCfg.qpHdr = 15;
#else
        rcCfg.qpHdr = -1;
#endif        
        if(gEncode_LowLatency)
        {
            if (gEncode_Interlanced)
            {
                rcCfg.qpMin = 18;
                rcCfg.qpMax = 28;
                rcCfg.mbQpAdjustment = -4;
            }
            else
            {
                rcCfg.qpMin = gQpMin;
                rcCfg.qpMax = gQpMax;
                rcCfg.mbQpAdjustment = gMbQpAdjustment;
                //printf("gQpMin: 0x%X, gQpMax: 0x%X, gMbQpAdjustment: 0x%X\n", &gQpMin, &gQpMax, &gMbQpAdjustment);
                //printf("set qpMin: %d, qpMax: %d, gMbQpAdjustment: %d\n", rcCfg.qpMin, rcCfg.qpMax, rcCfg.mbQpAdjustment);
            }
        }
        else
        {
            rcCfg.qpMin = 23;
            rcCfg.qpMax = 50;
            rcCfg.mbQpAdjustment = 0;
        }

        rcCfg.pictureSkip = 0;
#ifdef VBR_ENABLE        
        rcCfg.pictureRc = 0;
#else
        rcCfg.pictureRc = 1;
#endif        
        rcCfg.mbRc = 0;
#ifdef CFG_IOT_ENABLE
        rcCfg.bitPerSecond = 2000 * 1024;
#else
        rcCfg.bitPerSecond = gEncode_Bitrate * 1024;
#endif
        rcCfg.hrd = 0;
        rcCfg.hrdCpbSize = 0;
        rcCfg.gopLen = mappingFramerate(gEncode_Framerate)*2;
        rcCfg.intraQpDelta = -3;
        rcCfg.fixedIntraQp = 0;
        rcCfg.longTermPicRate = mappingFramerate(gEncode_Framerate)*2;
        rcCfg.mbQpAutoBoost = 0;

        //printf("Set rate control: qp %2d [%2d, %2d] %8d bps  "
        //       "pic %d mb %d skip %d  hrd %d\n"
        //       "  cpbSize %d gopLen %d intraQpDelta %2d "
        //       "fixedIntraQp %2d mbQpAdjustment %d "
        //       "mbQpAutoBoost %d\n",
        //       rcCfg.qpHdr, rcCfg.qpMin, rcCfg.qpMax, rcCfg.bitPerSecond,
        //       rcCfg.pictureRc, rcCfg.mbRc, rcCfg.pictureSkip, rcCfg.hrd,
        //       rcCfg.hrdCpbSize, rcCfg.gopLen, rcCfg.intraQpDelta,
        //       rcCfg.fixedIntraQp, rcCfg.mbQpAdjustment, rcCfg.mbQpAutoBoost);

        if((ret = H264EncSetRateCtrl(gptAVCEncoder[0]->encoder, &rcCfg)) != H264ENC_OK)
        {
            printf("H264EncSetRateCtrl() failed. %x", ret);
            //CloseEncoder(encoder);
            //return -1;
        }
    }

    /* Encoder setup: coding control */
    if((ret = H264EncGetCodingCtrl(gptAVCEncoder[0]->encoder, &codingCfg)) != H264ENC_OK)
    {
        printf("H264EncGetCodingCtrl() failed. %x", ret);
        //CloseEncoder(encoder);
        //return -1;
    }
    else
    {
#ifdef MULTI_SLICE
        if(gEncode_LowLatency && !gEncode_Interlanced)
            codingCfg.sliceSize = ((((gptAVCEncoder[0]->frameHeight)+15)>>4)+3)>>1;
        else
            codingCfg.sliceSize = 0;

        printf("\n\n codingCfg.sliceSize:%d\n\n",codingCfg.sliceSize);
#else
        codingCfg.sliceSize = 0;
#endif
        codingCfg.constrainedIntraPrediction = 0;
        codingCfg.disableDeblockingFilter = 0;
        codingCfg.cabacInitIdc = 0;
        codingCfg.transform8x8Mode = 0;
#ifdef CFG_IPTV_TX
        if (gEncode_Interlanced)
        {
            codingCfg.enableCabac = 0;
            codingCfg.quarterPixelMv = 1;
        }
        else
        {
            codingCfg.enableCabac = 0;
            codingCfg.quarterPixelMv = 0;
        }
#else
        codingCfg.enableCabac = 2;
        codingCfg.quarterPixelMv = 1;
#endif
        codingCfg.videoFullRange = 1;
        codingCfg.seiMessages = 0;
        codingCfg.idrHeader = 0;
        codingCfg.gdrDuration = 0;
        if (gptAVCEncoder[0]->interlaced_frame)
            codingCfg.fieldOrder = 1;
        else
            codingCfg.fieldOrder = 0;
        codingCfg.cirStart = 0;
        codingCfg.cirInterval = 0;
#ifdef LOW_LATENCY
        if (!gptAVCEncoder[0]->interlaced_frame)
        {
            //Low Latency
            codingCfg.inputLineBufDepth = 4;
            codingCfg.inputLineBufHwModeEn = 1;
            codingCfg.inputLineBufEn = 1;
            codingCfg.inputLineBufLoopBackEn = 1;
        }
#endif
        //printf
        //    ("Set coding control: SEI %d idrHdr %d Slice %5d   deblocking %d "
        //     "constrained intra %d video range %d\n"
        //     "  cabac %d  cabac initial idc %d  Adaptive 8x8 transform %d"
        //     "  quarter-pixel MV %d\n"
        //     "  adaptiveRoi %d,%d\n",
        //     codingCfg.seiMessages, codingCfg.idrHeader, codingCfg.sliceSize,
        //     codingCfg.disableDeblockingFilter,
        //     codingCfg.constrainedIntraPrediction, codingCfg.videoFullRange,
        //     codingCfg.enableCabac, codingCfg.cabacInitIdc,
        //     codingCfg.transform8x8Mode, codingCfg.quarterPixelMv,
        //     codingCfg.adaptiveRoi, codingCfg.adaptiveRoiColor);

        if((ret = H264EncSetCodingCtrl(gptAVCEncoder[0]->encoder, &codingCfg)) != H264ENC_OK)
        {
            printf("H264EncSetCodingCtrl() failed. %x", ret);
            //CloseEncoder(encoder);
            //return -1;
        }
    }
    if ((ret = H264EncGetPreProcessing(gptAVCEncoder[0]->encoder, &preProcCfg)) != H264ENC_OK)
    {
        printf("H264EncGetPreProcessing() failed.\n", ret);
    } else {

        preProcCfg.inputType = H264ENC_YUV420_SEMIPLANAR;
        preProcCfg.origWidth = cfg.width;
        preProcCfg.origHeight = cfg.height;
        preProcCfg.videoStabilization = 0;
        preProcCfg.scaledOutput = 0;
        
        if (gptAVCEncoder[0]->interlaced_frame)
            preProcCfg.interlacedFrame = 1;
        else
            preProcCfg.interlacedFrame = 0;

        if ((ret = H264EncSetPreProcessing(gptAVCEncoder[0]->encoder, &preProcCfg)) != H264ENC_OK)
        {
            printf("H264EncSetPreProcessing() failed.\n", ret);
        }
    }
#if defined(CFG_DUAL_STREAM)
    gptAVCEncoder[1]->frameWidth = gptAVCEncoder[1]->framePitchY = cfg.width = SENSOR_WIDTH_2;
    gptAVCEncoder[1]->frameHeight = cfg.height = SENSOR_HEIGHT_2;
    cfg.scaledWidth = 0;
    cfg.scaledHeight = 0;

    if (VIDEO_FRAME_RATE == VIDEO_FRAME_RATE_25)
    {
        cfg.frameRateDenom = 1;
        cfg.frameRateNum = 25;
    } else {
        cfg.frameRateDenom = 1;
        cfg.frameRateNum = 30;
    }

    if (gptAVCEncoder[0]->interlaced_frame)
        cfg.viewMode = H264ENC_INTERLACED_FIELD;
    else
        cfg.viewMode = H264ENC_BASE_VIEW_DOUBLE_BUFFER; /* Two buffers by default */
    cfg.streamType = H264ENC_NAL_UNIT_STREAM;

    tmp = (gptAVCEncoder[1]->frameWidth * gptAVCEncoder[1]->frameHeight) >> 8;

    if (tmp <= 396)
        cfg.level = H264ENC_LEVEL_2;
    else if (tmp <= 792)
        cfg.level = H264ENC_LEVEL_2_1;
    else if (tmp <= 1620)
        cfg.level = H264ENC_LEVEL_3;
    else if (tmp <= 3600)
        cfg.level = H264ENC_LEVEL_3_1;
    else if (tmp <= 5120)
        cfg.level = H264ENC_LEVEL_3_2;
    else if (tmp <= 8192)
        cfg.level = H264ENC_LEVEL_4;

    //cfg.level = H264ENC_LEVEL_4;

    cfg.refFrameCompress = 0;
    cfg.rfcLumBufLimit = 0;
    cfg.rfcChrBufLimit = 0;
    cfg.refFrameAmount = 1;

    //_VIDEO_ENCODER_Init();
    if((ret = H264EncInit(&cfg, &gptAVCEncoder[1]->encoder)) != H264ENC_OK)
    {
        printf("H264EncInit() failed. %x", ret);
        //return (int)ret;
    }

    /* Encoder setup: rate control */
    if((ret = H264EncGetRateCtrl(gptAVCEncoder[1]->encoder, &rcCfg)) != H264ENC_OK)
    {
        printf("H264EncGetRateCtrl() failed. %x", ret);
        //CloseEncoder(encoder);
        //return -1;
    }
    else
    {
        //printf("Get rate control: qp %2d [%2d, %2d]  %8d bps  "
        //       "pic %d mb %d skip %d  hrd %d\n  cpbSize %d gopLen %d "
        //       "intraQpDelta %2d\n",
        //       rcCfg.qpHdr, rcCfg.qpMin, rcCfg.qpMax, rcCfg.bitPerSecond,
        //       rcCfg.pictureRc, rcCfg.mbRc, rcCfg.pictureSkip, rcCfg.hrd,
        //       rcCfg.hrdCpbSize, rcCfg.gopLen, rcCfg.intraQpDelta);

        rcCfg.qpHdr = -1;
        if(gEncode_LowLatency)
        {
            rcCfg.qpMin = 15;
            rcCfg.qpMax = 45;
            rcCfg.mbQpAdjustment = -5;
        }
        else
        {
            rcCfg.qpMin = 23;
            rcCfg.qpMax = 50;
            rcCfg.mbQpAdjustment = 0;
        }
        rcCfg.pictureSkip = 0;
        rcCfg.pictureRc = 1;
        rcCfg.mbRc = 1;
        rcCfg.bitPerSecond = SENSOR_BITRATE_2 * 1024;
        rcCfg.hrd = 0;
        rcCfg.hrdCpbSize = 0;
        rcCfg.gopLen = mappingFramerate(gEncode_Framerate)*2;
        rcCfg.intraQpDelta = -3;
        rcCfg.fixedIntraQp = 0;
        rcCfg.mbQpAdjustment = -5;
        rcCfg.longTermPicRate = mappingFramerate(gEncode_Framerate)*2;
        rcCfg.mbQpAutoBoost = 0;

        //printf("Set rate control: qp %2d [%2d, %2d] %8d bps  "
        //       "pic %d mb %d skip %d  hrd %d\n"
        //       "  cpbSize %d gopLen %d intraQpDelta %2d "
        //       "fixedIntraQp %2d mbQpAdjustment %d "
        //       "mbQpAutoBoost %d\n",
        //       rcCfg.qpHdr, rcCfg.qpMin, rcCfg.qpMax, rcCfg.bitPerSecond,
        //       rcCfg.pictureRc, rcCfg.mbRc, rcCfg.pictureSkip, rcCfg.hrd,
        //       rcCfg.hrdCpbSize, rcCfg.gopLen, rcCfg.intraQpDelta,
        //       rcCfg.fixedIntraQp, rcCfg.mbQpAdjustment, rcCfg.mbQpAutoBoost);

        if((ret = H264EncSetRateCtrl(gptAVCEncoder[1]->encoder, &rcCfg)) != H264ENC_OK)
        {
            printf("H264EncSetRateCtrl() failed. %x", ret);
            //CloseEncoder(encoder);
            //return -1;
        }
    }

    /* Encoder setup: coding control */
    if((ret = H264EncGetCodingCtrl(gptAVCEncoder[1]->encoder, &codingCfg)) != H264ENC_OK)
    {
        printf("H264EncGetCodingCtrl() failed. %x", ret);
        //CloseEncoder(encoder);
        //return -1;
    }
    else
    {
#ifdef MULTI_SLICE
        if(gEncode_LowLatency && !gEncode_Interlanced)
            codingCfg.sliceSize = ((((gptAVCEncoder[0]->frameHeight)+15)>>4)+3)>>1;
        else
            codingCfg.sliceSize = 0;

        printf("\n\n codingCfg.sliceSize:%d\n\n",codingCfg.sliceSize);
#else
        codingCfg.sliceSize = 0;
#endif
        codingCfg.constrainedIntraPrediction = 0;
        codingCfg.disableDeblockingFilter = 0;
        codingCfg.enableCabac = 1;
        codingCfg.cabacInitIdc = 0;
        codingCfg.transform8x8Mode = 1;
#ifdef CFG_IPTV_TX
        codingCfg.quarterPixelMv = 0;
#else
        codingCfg.quarterPixelMv = 1;
#endif
        codingCfg.videoFullRange = 1;
        codingCfg.seiMessages = 1;
        codingCfg.idrHeader = 1;
        codingCfg.gdrDuration = 0;
        if (gptAVCEncoder[0]->interlaced_frame)
            codingCfg.fieldOrder = 1;
        else
            codingCfg.fieldOrder = 0;
        codingCfg.cirStart = 0;
        codingCfg.cirInterval = 0;
#ifdef LOW_LATENCY
        if (
!gptAVCEncoder[0]->interlaced_frame)
        {
            //Low Latency
            codingCfg.inputLineBufDepth = 4;
            codingCfg.inputLineBufHwModeEn = 1;
            codingCfg.inputLineBufEn = 1;
            codingCfg.inputLineBufLoopBackEn = 1;
        }
#endif
        //printf
        //    ("Set coding control: SEI %d idrHdr %d Slice %5d   deblocking %d "
        //     "constrained intra %d video range %d\n"
        //     "  cabac %d  cabac initial idc %d  Adaptive 8x8 transform %d"
        //     "  quarter-pixel MV %d\n"
        //     "  adaptiveRoi %d,%d\n",
        //     codingCfg.seiMessages, codingCfg.idrHeader, codingCfg.sliceSize,
        //     codingCfg.disableDeblockingFilter,
        //     codingCfg.constrainedIntraPrediction, codingCfg.videoFullRange,
        //     codingCfg.enableCabac, codingCfg.cabacInitIdc,
        //     codingCfg.transform8x8Mode, codingCfg.quarterPixelMv,
        //     codingCfg.adaptiveRoi, codingCfg.adaptiveRoiColor);

        if((ret = H264EncSetCodingCtrl(gptAVCEncoder[1]->encoder, &codingCfg)) != H264ENC_OK)
        {
            printf("H264EncSetCodingCtrl() failed. %x", ret);
            //CloseEncoder(encoder);
            //return -1;
        }
    }
#endif

    _CaptureAndVP_Init();


    gbEncodeFire = MMP_FALSE;

    for (i = 0; i < VIDEO_STREAM_NUM; i++)
    {
        gVideoSelIdx[i]     = gVideoWrIdx[i] = 0;
        gStartgetVideo[i]   = MMP_FALSE;
        gVideoUserNum[i]    = 0;
    }
    gbVideoEncoderInit  = MMP_TRUE;
    gbJPEGEncoder       = MMP_FALSE;
    gbVideoEncoder      = MMP_FALSE;

    //_WaitAllQueue_Empty();
    mmpIspResetEngine(gIspDev);
    WriteBufferIndex = 0;
    gCapQueCnt = 0;
    gbSkipBotField = MMP_FALSE;
    gPreVidSize = 0;
    gVidSkipCnt = 0;

    memset(&gtAVCEncoderFlow, 0, sizeof(gtAVCEncoderFlow));
    _CaptureResultQ_Reset();
    _IspResultQ_Reset();
    sem_init(&gpEncoderSem, 0, 0);
}

void
VideoEncoder_SetStreamBuf(
    MMP_UINT32  stramIndex,
    MMP_UINT32  bufIndex,
    MMP_UINT8   *bufAdr)
{
    if (bufIndex < gptAVCEncoder[stramIndex]->streamBufCount)
    {
        gbStrBufFire[stramIndex][bufIndex]                  = MMP_FALSE;
        gptAVCEncoder[stramIndex]->pStreamBufAdr[bufIndex]  = bufAdr;
        //printf("VideoEncoder_SetStreamBuf %d\n", bufIndex);
    }
}

void
VideoEncoder_SetStreamBufCallBack(
    ENCODE_PARA_CALLBACK encodeCallback)
{
    gfpCallback = encodeCallback;
}

void
VideoEncoder_Open(
    MMP_UINT32 stramIndex)
{
    MMP_UINT32 i, j;

    if (!gbVideoEncoderInit)
    {
        return;
    }

    if (stramIndex == 0 && gbStream0Enable == MMP_FALSE)
    {
        gbStream0Enable             = MMP_TRUE;
        gStartgetVideo[stramIndex]  = MMP_TRUE;
        gVideoUserNum[stramIndex]   = 1;
    }
    //else if (stramIndex == 0 && gbStream0Enable == MMP_TRUE)
    //    return;

    if (stramIndex == 1)
    {
        gbStream1Enable             = MMP_TRUE;
        gStartgetVideo[stramIndex]  = MMP_TRUE;
        gVideoUserNum[stramIndex]   = 1;
    }
    if (gbVideoEncoder)
        return;

    OpenEngine(H264_ENCODER);

    for (i = 0; i < VIDEO_STREAM_NUM; i++)
        for (j = 0; j < VIDEO_SAMPLE_NUM; j++)
            gbStrBufFire[i][j] = MMP_FALSE;

    gbEncodeFire = MMP_FALSE;

    for (i = 0; i < VIDEO_STREAM_NUM; i++)
        gVideoSelIdx[i] = gVideoWrIdx[i] = 0;

    gbVideoEncoder = MMP_TRUE;
}

void
VideoEncoder_GetHdrInfo(
    VIDEO_HEADER_INFO *headInfo)
{
    headInfo->SPSBuf    = gptAVCEncoder[headInfo->streamIndex]->pHdrBufAddr[0];
    headInfo->PPSBuf    = gptAVCEncoder[headInfo->streamIndex]->pHdrBufAddr[1];
    headInfo->SPS_Size  = gptAVCEncoder[headInfo->streamIndex]->ParaSetHdrSize[0];
    headInfo->PPS_Size  = gptAVCEncoder[headInfo->streamIndex]->ParaSetHdrSize[1];
}

void
VideoEncoder_Close(
    void)
{
    MMP_UINT32 i;

    if (!gbVideoEncoderInit)
    {
        return;
    }

    CloseEngine(H264_ENCODER);

    memset(&gtAVCEncoderFlow, 0, sizeof(gtAVCEncoderFlow));

    for (i = 0; i < VIDEO_STREAM_NUM; i++)
        gptAVCEncoder[i]->streamBufSelect = 0;

    _CaptureResultQ_Reset();
    _IspResultQ_Reset();

    gfpCallback         = MMP_NULL;
    for (i = 0; i < VIDEO_STREAM_NUM; i++)
    {
        gStartgetVideo[i]   = MMP_FALSE;
        gVideoUserNum[i]    = 0;
    }

    if (stream_buf_vram_addr)
    {
        free(stream_buf_vram_addr);
        stream_buf_vram_addr = NULL;
    }

    for (i = 0; i < ENCODE_STREAM_NUM; i++)
    {
        if (gptAVCEncoder[i]->buf_vram_addr)
        {
            free(gptAVCEncoder[i]->buf_vram_addr);
            gptAVCEncoder[i]->buf_vram_addr = NULL;
        }

        if (gptAVCEncoder[i]->hdr_buf_vram_addr)
        {
            free(gptAVCEncoder[i]->hdr_buf_vram_addr);
            gptAVCEncoder[i]->hdr_buf_vram_addr = NULL;
        }

        if (gptAVCEncoder[i])
        {
            free(gptAVCEncoder[i]);
            gptAVCEncoder[i] = NULL;
        }
    }

    gbVideoEncoderInit = false;
}

MMP_BOOL
VideoEncoder_GetSreamstate(
    MMP_UINT8 stream_id)
{
    return gStartgetVideo[stream_id];
}

MMP_UINT8
VideoEncoder_GetSreamUserNum(
    MMP_UINT8 stream_id)
{
    //printf(">>>>>%s, %d, %d\n", __FUNCTION__, stream_id, gVideoUserNum[stream_id]);
    return gVideoUserNum[stream_id];
}

void
VideoEncoder_SetEncodeIdx(
    uint8_t index)
{
    printf(">>>>>%s, %d\n", __FUNCTION__, index);
    uint8_t    WBIndex = WriteBufferIndex;
	CAPTURE_RESULT  tEntry = {0};

	WriteBufferIndex++;
    WriteBufferIndex %= gptAVCEncoder[0]->sourceBufCount;

    gJPEGBufIndex       = tEntry.frameIndex = WBIndex;
    tEntry.frameCount   = ++gtAVCEncoderFlow.captureFrameCount;

    if (gtAVCEncoderFlow.pfToTimeStamp)
    {
        tEntry.timeStamp                = gtAVCEncoderFlow.currTimeStamp
                                        = gtAVCEncoderFlow.baseTimeStamp
                                        + gtAVCEncoderFlow.pfToTimeStamp(tEntry.frameCount);
    }

    _CaptureResultQ_Enqueue(&tEntry);

    if (gbJPEGEncoder)
    {
        itpSemPostFromISR(&gpEncoderSem);
        gbJPEGEncoder = MMP_FALSE;
    }
}

void
VideoEncoder_SetWithPreview(
    MMP_BOOL preview)
{
	//todo
}

void
JPEGEncodeFrame(
    JPEG_ENCODE_PARAMETER *enPara)
{
    HJPG            *pHJpeg         = 0;
    JPG_INIT_PARAM  initParam       = {0};
    JPG_STREAM_INFO inStreamInfo    = {0};
    JPG_STREAM_INFO outStreamInfo   = {0};
    JPG_BUF_INFO    entropyBufInfo  = {0};
    JPG_USER_INFO   jpgUserInfo     = {0};
    uint32_t        jpgEncSize      = 0;
    //MMP_UINT32          jpgStart = 0;

    //jpgStart = PalGetClock();

    if (!gbVideoEncoderInit)
        return;

    OpenEngine(JPEG_ENCODER);

    // ------------------------------------------------------
    // encode JPEG
    initParam.codecType     = JPG_CODEC_ENC_JPG;
    initParam.exColorSpace  = JPG_COLOR_SPACE_NV12;
    initParam.outColorSpace = JPG_COLOR_SPACE_YUV420;
    initParam.width         = gptAVCEncoder[0]->frameWidth;
    initParam.height        = gptAVCEncoder[0]->frameHeight;
    initParam.encQuality    = enPara->quality;

#ifndef CFG_IPTV_TX
    iteJpg_CreateHandle(&pHJpeg, &initParam, 0);
#endif

    if (sem_wait(&gpEncoderSem) == 0)
    {
        inStreamInfo.streamIOType           = JPG_STREAM_IO_READ;
        inStreamInfo.streamType             = JPG_STREAM_MEM;

        // Y
        inStreamInfo.jstream.mem[0].pAddr   = (uint8_t *)gptAVCEncoder[0]->pSourceBufAdrY[gJPEGBufIndex];
        inStreamInfo.jstream.mem[0].pitch   = gptAVCEncoder[0]->framePitchY;
        //inStreamInfo.src.mem[0].length = _Get_Lcd_Width() * _Get_Lcd_Height();
        // U
        inStreamInfo.jstream.mem[1].pAddr   = (uint8_t *)gptAVCEncoder[0]->pSourceBufAdrU[gJPEGBufIndex];
        inStreamInfo.jstream.mem[1].pitch   = gptAVCEncoder[0]->framePitchY;
        //inStreamInfo.src.mem[1].length = (_Get_Lcd_Width()/2) * _Get_Lcd_Height();
        // V
        inStreamInfo.jstream.mem[2].pAddr   = (uint8_t *)gptAVCEncoder[0]->pSourceBufAdrV[gJPEGBufIndex];
        inStreamInfo.jstream.mem[2].pitch   = gptAVCEncoder[0]->framePitchY;
        //inStreamInfo.src.mem[2].length = (_Get_Lcd_Width()/2) * _Get_Lcd_Height();

        inStreamInfo.validCompCnt           = 3;

        //if( encName )
        //{
        //    outStreamInfo.streamType = JPG_STREAM_FILE;
        //    outStreamInfo.jstream.path   = (void*)encName;
        //}

        outStreamInfo.streamIOType          = JPG_STREAM_IO_WRITE;
        outStreamInfo.streamType            = JPG_STREAM_MEM;
        //outStreamInfo.jpg_reset_stream_info =  _reset_stream_info;
        outStreamInfo.jstream.mem[0].pAddr  = enPara->strmBuf;
        outStreamInfo.jstream.mem[0].pitch  = enPara->strmBuf_size;
        outStreamInfo.jstream.mem[0].length = enPara->strmBuf_size;
        outStreamInfo.validCompCnt          = 1;

#ifndef CFG_IPTV_TX
        iteJpg_SetStreamInfo(pHJpeg, &inStreamInfo, &outStreamInfo, 0);
#endif
        //iteJpg_GetStatus(pHJpeg, &jpgUserInfo, 0);
        //printf("  (%d, %d) %dx%d, dispMode=%d\r\n",
        //            jpgUserInfo.jpgRect.x, jpgUserInfo.jpgRect.y,
        //            jpgUserInfo.jpgRect.w, jpgUserInfo.jpgRect.h,
        //            initParam.dispMode);
        //printf("  LCD A=0x%x, LCD B=0x%x\n", _Get_Lcd_Addr_A(), _Get_Lcd_Addr_B());

#ifndef CFG_IPTV_TX
        iteJpg_Setup(pHJpeg, 0);

        iteJpg_Process(pHJpeg, &entropyBufInfo, &jpgEncSize, 0);

        iteJpg_GetStatus(pHJpeg, &jpgUserInfo, 0);
#endif
		printf("\n\tresult = %d, encode size = %f KB\n", jpgUserInfo.status, (float)jpgEncSize / 1024);
        enPara->enSize = jpgEncSize;
#ifndef CFG_IPTV_TX
        iteJpg_DestroyHandle(&pHJpeg, 0);
#endif
	}

    CloseEngine(JPEG_ENCODER);

    //ithPrintf("Encoding Time %d\n", PalGetDuration(jpgStart));
}

static void
_DoIsp(
    void)
{
    CAPTURE_RESULT            tEntry       = {0};
    MMP_ISP_SHARE             ispInput     = {0};
    MMP_ISP_OUTPUT_INFO       ispOutput    = {0};
    MMP_ISP_OUTPUT_INFO       ispOutput_PV = {0};
    MMP_ISP_WRITE_BUFFER_INFO ispWbInfo    = {0};

    MMP_UINT16                  inBufferIdx;
    MMP_UINT16                  outBufferIdx;
    static MMP_UINT16           srcCount    = 0;
    MMP_UINT16                  resIdx;

    if (QUEUE_NOT_EMPTY == _CaptureResultQ_Dequeue(&tEntry))
    {
        //ithPrintf("DoISP %d\n", tEntry.resIdx);

        //if (tEntry.resIdx == 1)
        //    return;

        resIdx              = tEntry.resIdx;
        inBufferIdx         = tEntry.frameIndex;

        outBufferIdx        = srcCount; //tEntry.frameIndex;
        gJPEGBufIndex       = outBufferIdx;
        tEntry.frameIndex   = outBufferIdx;

        srcCount            = (srcCount + 1) % gptAVCEncoder[resIdx]->sourceBufCount;

        ispInput.format = MMP_ISP_IN_NV12;
        ispInput.width = gtIspTransformParm[resIdx].inWidth;
        ispInput.height = gtIspTransformParm[resIdx].inHeight;
        ispInput.pitchY = CAPTURE_MAXWIDTH;
        ispInput.pitchUv = CAPTURE_MAXWIDTH;
        ispInput.isAdobe_CMYK = 0;
        ispInput.addrY = gtIspTransformParm[resIdx].inAddrY[inBufferIdx];
        ispInput.addrU = gtIspTransformParm[resIdx].inAddrUV[inBufferIdx];
        //ithPrintf("DoISP (%x,%x), %d, %d, %d, %d)\n", ispInput.addrY, ispInput.addrU, ispInput.width, ispInput.height, ispInput.pitchY, ispInput.pitchUv);
        if (inBufferIdx == 0)
            ispInput.addrYp = gtIspTransformParm[resIdx].inAddrY[2];
        else
            ispInput.addrYp = gtIspTransformParm[resIdx].inAddrY[inBufferIdx - 1];

        ispOutput.addrY = gtIspTransformParm[resIdx].outAddrY[outBufferIdx];
        ispOutput.addrU = gtIspTransformParm[resIdx].outAddrUV[outBufferIdx];
        ispOutput.startX = 0;
        ispOutput.startY = 0;
        ispOutput.width = gptAVCEncoder[resIdx]->frameWidth;
        ispOutput.height = gptAVCEncoder[resIdx]->frameHeight;
        ispOutput.format = MMP_ISP_OUT_NV12;
        ispOutput.pitchY = gptAVCEncoder[resIdx]->framePitchY;
        ispOutput.pitchUv = gptAVCEncoder[resIdx]->framePitchY;
        //ithPrintf("DoISP (%x, %x), (%d, %d, %d, %d)\n", ispOutput.addrY, ispOutput.addrU, ispOutput.width, ispOutput.height, ispOutput.pitchY, ispOutput.pitchUv);
        mmpIspEnableInterrupt(gIspDev);
        mmpIspDisable(gIspDev, MMP_ISP_DEINTERLACE);
        mmpIspSetOutputWindow(gIspDev, &ispOutput);
        mmpIspSetVideoWindow(gIspDev, ispOutput.startX, ispOutput.startY, ispOutput.width, ispOutput.height);
        mmpIspPlayImageProcess(gIspDev, &ispInput);

        memcpy(&gtIspFireEntry, &tEntry, sizeof(CAPTURE_RESULT));
    }
}

static void
_DoEncode(
    void)
{

}

static void
VP_onfly_isr(
    void *arg)
{
    CAPTURE_RESULT  tEntry          = {0};
    MMP_BOOL        bIsIdle         = MMP_FALSE;
    MMP_UINT16      i;
    MMP_UINT16      captureErrState = ithCapGetEngineErrorStatus(&cap0, MMP_CAP_LANE0_STATUS);
    MMP_UINT32      WBIndex         = WriteBufferIndex;
    MMP_BOOL        bBottomField    = MMP_FALSE;

    //ithPrintf("ISP ISR\n");
    mmpIspClearInterrupt(gIspDev);


    if(gptAVCEncoder[0]->interlaced_frame)
    {
        gptAVCEncoder[0]->BufTBFieldMark[WriteBufferIndex] = bBottomField = _Get_Field_State();
        //ithPrintf("TBfield = [%d,%d]\n", WriteBufferIndex, _Get_Field_State());
        if(_Get_Field_State())//if BOTTOM FIELD ,buffer count++
        {
            WriteBufferIndex++;
            WriteBufferIndex %= gptAVCEncoder[0]->sourceBufCount;
        }
        _Update_Field_State(); 
    }
    else
    {
        WriteBufferIndex++;
        WriteBufferIndex %= gptAVCEncoder[0]->sourceBufCount;
    }

    //Error frame not encode
    if (captureErrState & 0x0F00)
    {
        ithPrintf("---Error frame not encode---\n");
        _Set_Field_Reset();
        return;
    }

    //gJPEGBufIndex       = tEntry.frameIndex = mmpVPReturnWrBufIndex();  // TODO: 970 VP not support
    gJPEGBufIndex       = tEntry.frameIndex = WBIndex;
    tEntry.frameCount   = ++gtAVCEncoderFlow.captureFrameCount;

    if (!gptAVCEncoder[0]->interlaced_frame)
    {
        if (gVidSkipCnt != 0)
        {
            gVidSkipCnt--;	
            return;
        }

        if (gbPrevIFrame)
        {
            gChkDropTime = itpGetTickCount();
            gDataSize = 0;
            gPreVidSize = 0;
        }
        else
        {
            gDataSize += gPreVidSize;
        }

        gbPrevIFrame = 0;
        gPreVidSize = 0;
        if (!gbHalfDrop)
        {
            if (itpGetTickDuration(gChkDropTime) > 50)
            {
                if (gDataSize > DROP_DATA_RATE_THRESHOLD)
                {
                    ithPrintf("Start Half drop mode. dur: %d ms, data: %d bytes\n", itpGetTickDuration(gChkDropTime), gDataSize);
                    gbHalfDrop = 1;
                    gDataSize = 0;
                    gCountDown = COUNT_DOWN_THRESHOLD;
                    gChkDropTime = itpGetTickCount();
                    gCountDownTime = itpGetTickCount();
                    return;
                }
                gChkDropTime = itpGetTickCount();
                gDataSize = 0;
            }
        }
        
        if (gbHalfDrop)
        {
            if (itpGetTickDuration(gChkNormalTime) >= 1000)
            {
                if (gDataSize < NORMAL_DATA_RATE_THRESHOLD)
                {
                    if (--gCountDown == 0)
                    {
                        gbHalfDrop = 0;
                        ithPrintf("Back to normal encode frame rate. dur: %d ms, data: %d bytes\n", itpGetTickDuration(gCountDownTime), gDataSize);
                    }
                }
                else
                {
                    if (++gCountDown > COUNT_DOWN_THRESHOLD)
                    {
                        gCountDown = COUNT_DOWN_THRESHOLD;
                    }
                }
                gChkNormalTime = itpGetTickCount();
                gDataSize = 0;
            }
        }

        if (gbHalfDrop)
        {
            gVidSkipCnt = 1;
        }
    }

    //skip interlace source
    if ((gptAVCEncoder[0]->interlaced_frame && gCapQueCnt != 0 && !bBottomField) || gbSkipBotField)
    {
        if (gbSkipBotField)        
            gbSkipBotField = MMP_FALSE;
        else
            gbSkipBotField = MMP_TRUE;
        	
        ithPrintf("Skip Field %d %d\n", bBottomField, gCapQueCnt);
        
        return;
    }
    //skip progressive source	
    #if 0
    if (!gptAVCEncoder[0]->interlaced_frame && gCapQueCnt != 0)
    {
        ithPrintf("Skip Frame %d\n", gCapQueCnt);
        return;
    }
    #endif
    
    gCapQueCnt++;
    
    if (gtAVCEncoderFlow.pfToTimeStamp)
    {
        tEntry.timeStamp                = gtAVCEncoderFlow.currTimeStamp
                                        = gtAVCEncoderFlow.baseTimeStamp
                                        + gtAVCEncoderFlow.pfToTimeStamp(tEntry.frameCount);
    }

    //if (!_Chk_Skip_Frame(0))
    _CaptureResultQ_Enqueue(&tEntry);

    if (gbJPEGEncoder)
    {
        itpSemPostFromISR(&gpEncoderSem);
        gbJPEGEncoder = MMP_FALSE;
    }

}

static void
cap_isr(
    void *arg)
{
#if 0
    CAPTURE_RESULT      tEntry          = {0};
    MMP_UINT32          dur             = 0;
    static MMP_UINT32   initClock       = 0;
    static MMP_UINT32   totalDur        = 0;
    MMP_UINT16          captureErrState = ithCapGetEngineErrorStatus(&cap0,MMP_CAP_LANE0_STATUS);
    MMP_UINT16          i;
    MMP_BOOL            bSkipFrame[ENCODE_STREAM_NUM];
    
    //ithPrintf("cap_isr\n");
    
    tEntry.frameIndex   = ithCapReturnWrBufIndex(&cap0);
    tEntry.frameCount   = ++gtAVCEncoderFlow.captureFrameCount;

    if (gtAVCEncoderFlow.pfToTimeStamp)
    {
        tEntry.timeStamp                = gtAVCEncoderFlow.currTimeStamp
                                        = gtAVCEncoderFlow.baseTimeStamp
                                        + gtAVCEncoderFlow.pfToTimeStamp(tEntry.frameCount);
    }
    //Error frame not encode
    if (captureErrState & 0x0F00)
    {
        return;
    }

    for (i = 0; i < VIDEO_STREAM_NUM; i++)
        bSkipFrame[i]   = MMP_FALSE;//_Chk_Skip_Frame(i);

    for (tEntry.resIdx = 0; tEntry.resIdx < ISP_TRANSFORM_NUM; tEntry.resIdx++)
    {
        if (!bSkipFrame[tEntry.resIdx] &&
            ((tEntry.resIdx == 0 && gbStream0Enable) || (tEntry.resIdx == 1 && gbStream1Enable)))
        {
            _CaptureResultQ_Enqueue(&tEntry);
        }
    }
#endif
}

static void
VP_mem_isr(
    void *arg)
{
    MMP_BOOL bIsIdle = MMP_FALSE;

    mmpIspClearInterrupt(gIspDev);

    //ithPrintf("VP_mem isr(0)\n");
    //ithPrintf("VP_T %d\n", PalGetDuration(tickStart));

    gtAVCEncoderFlow.encodeQue[gtIspFireEntry.frameIndex] = MMP_TRUE;
    _IspResultQ_Enqueue(&gtIspFireEntry);

    if (gbJPEGEncoder && gtIspFireEntry.resIdx == 0)
    {
        itpSemPostFromISR(&gpEncoderSem);
        gbJPEGEncoder = MMP_FALSE;
    }

}

static void*
isp_task(
    void *arg)
{
    while (gbStream0Enable)
    {
        if (mmpIspIsEngineIdle(gIspDev))
        {
            _DoIsp();
        }
        usleep(1000);
    }
    return 0;
}

#ifdef MULTI_SLICE
void H264SliceReady(H264EncSliceReady *slice)
{
    i32 i, size;
    u8 *strmPtr;
    uint32_t timeoutCnt;
    VIDEO_STREAM_INFO   streamInfo;
    //uint32_t time0;
    //static uint32_t time1;

    //time0 = PalGetClock();
    if (slice->slicesReadyPrev == 0)    /* New frame */
        strmPtr = (u8*)slice->pOutBuf;  /* Pointer to beginning of frame */
    else
        strmPtr = (u8*)slice->pAppData; /* Here we store the slice pointer */

    for (i = slice->slicesReadyPrev; i < slice->slicesReady; i++)
    {
        timeoutCnt = 100;
        ithInvalidateDCacheRange(&slice->sliceSizes[i], 32);
        while (slice->sliceSizes[i] == 0 && timeoutCnt > 0)
        {
            ithInvalidateDCacheRange(&slice->sliceSizes[i], 32);
            ithDelay(100);
            timeoutCnt--;
        }
        if (slice->sliceSizes[i] == 0)
            printf("Error Size(0)\n");

        size = slice->sliceSizes[i];
        ithInvalidateDCacheRange(strmPtr, size);

        //memcpy(gptAVCEncoder[gEncodeIdx]->pStreamBufAdr[gVideoWrIdx[gEncodeIdx]], strmPtr, size);

        streamInfo.streamIndex  = gEncodeIdx;
        streamInfo.bufIndex     = gVideoWrIdx[gEncodeIdx];
        if (gbHalfDrop)
        {
            if (slice->slicesReadyPrev == 0 && (((gtAVCEncoderFlow.encodedFrameCount % (mappingFramerate(gEncode_Framerate)*2)) == 1) || ((gtAVCEncoderFlow.encodedFrameCount % (mappingFramerate(gEncode_Framerate)*2)) == 2)))
                streamInfo.bIFrame      = 1;
            else
                streamInfo.bIFrame      = 0;
        }
        else
        {
            if (slice->slicesReadyPrev == 0 && ((gtAVCEncoderFlow.encodedFrameCount % (mappingFramerate(gEncode_Framerate)*2)) == 1))
                streamInfo.bIFrame      = 1;
            else
                streamInfo.bIFrame      = 0;
        }
        streamInfo.dataSize     = size;
        streamInfo.timeStamp    = gtAVCEncoderFlow.encodedTimeStamp;
        streamInfo.bLastSlice   = MMP_FALSE;
#ifdef CFG_IPTV_TX
        streamInfo.pData        = strmPtr;
        streamInfo.frameCount   = gtAVCEncoderFlow.encodedFrameCount;
        streamInfo.InstanceNum  = gEncodeIdx;

        streamInfo.frameRate    = gtAVCEncoderFlow.frameRate;
        streamInfo.EnFrameRate  = gtAVCEncoderFlow.frameRate;
        streamInfo.width        = gptAVCEncoder[gEncodeIdx]->frameWidth;
        streamInfo.height       = gptAVCEncoder[gEncodeIdx]->frameHeight;
        streamInfo.frameType    = (gptAVCEncoder[gEncodeIdx]->bIFrame) ? 0 : 1;
        streamInfo.bitRate      = gEncode_Bitrate;
        streamInfo.bSkipFrame   = gtAVCEncoderFlow.bSkipFrame;
        streamInfo.bColorRange  = gbColorRange;
        streamInfo.encoder_Info = gCapture_info;
        gbPrevIFrame = streamInfo.bIFrame;
#endif

        gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]]  = MMP_FALSE;

        //gVideoSelIdx[gEncodeIdx]                            = (gVideoSelIdx[gEncodeIdx] + 1) % VIDEO_SAMPLE_NUM;

        gfpCallback(&streamInfo);
        //gVideoWrIdx[gEncodeIdx] = (gVideoWrIdx[gEncodeIdx] + 1) % VIDEO_SAMPLE_NUM;

        strmPtr += size;
        gSliceNum++;
    }
    //printf("Time %d %d\n", PalGetDuration(time0), PalGetDuration(time1));
    //time1 = PalGetClock();
    /* Store the slice pointer for next callback */
    gLastSlicePtr = slice->pAppData = strmPtr;

}

static void*
encoder_task(
    void *arg)
{
    MMP_UINT32  streamLen;
    MMP_BOOL    bFrmEnd;
    CAPTURE_RESULT tEntry = {0};
    MMP_UINT8           *pData;
#ifdef CFG_IPTV_TX
    MMP_UINT32          dataSize    = 2000 * 1024; // tmp solution
#else
    MMP_UINT32          dataSize    = 500 * 1024; // tmp solution
#endif
    VIDEO_STREAM_INFO   streamInfo;
    H264EncIn encIn;
    H264EncOut encOut;
    int ret;
    int i;
    uint32_t timeoutCnt;
    uint32_t EnStart;

    gVideoEncoding[0] = MMP_TRUE;
    while (gbStream0Enable && gbVideoEncoder)
    {

         if (QUEUE_NOT_EMPTY == gtAVCEncoderFlow.pfEnDequeue(&tEntry) && gbEncodeStart)
         {
             gEncodeIdx = tEntry.resIdx;

             //if (gEncodeIdx == 1)
             //      return;
             //ithPrintf("gEncodeIdx %d\n", gEncodeIdx);
             if (gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]])
             {
                 ithPrintf("Encoding Buf Full\n");
                 continue;
             }
             gtAVCEncoderFlow.encodedFrameCount                  = tEntry.frameCount;
             gtAVCEncoderFlow.encodedTimeStamp                   = tEntry.timeStamp;

             gptAVCEncoder[gEncodeIdx]->sourceBufSelect          = tEntry.frameIndex;
             //gbEncodeFire                                        = MMP_TRUE;

             //pData      = gptAVCEncoder[gEncodeIdx]->pStreamBufAdr[gVideoWrIdx[gEncodeIdx]];
             gLastSlicePtr = pData = gptAVCEncoder[gEncodeIdx]->pStreamBufAdr[gVideoWrIdx[gEncodeIdx]];

             memset (&encIn, 0, sizeof(H264EncIn));
             encIn.pOutBuf = pData;
             encIn.busOutBuf = pData;
             encIn.outBufSize = dataSize;

#ifndef CFG_IPTV_TX            
#if defined(CFG_CPU_WB) && (!defined(WIN32))
             ithInvalidateDCacheRange(pData, dataSize);
#endif
#endif
             if (gtAVCEncoderFlow.encodedFrameCount == 1)
                 encIn.timeIncrement = 0;
             else
                 encIn.timeIncrement = 1;

             if (gbHalfDrop)
             {
                 if ((gtAVCEncoderFlow.encodedFrameCount % (mappingFramerate(gEncode_Framerate)*2)) == 1
                  || (gtAVCEncoderFlow.encodedFrameCount % (mappingFramerate(gEncode_Framerate)*2)) == 2)
                     encIn.codingType = H264ENC_INTRA_FRAME;
                 else
                     encIn.codingType = H264ENC_PREDICTED_FRAME;
             }
             else
             {
                if (gptAVCEncoder[gEncodeIdx]->interlaced_frame)
                {
                    if((gtAVCEncoderFlow.encodedFrameCount % (mappingFramerate(gEncode_Framerate)*2)) == 2)
                    {
                        encIn.codingType = H264ENC_NONIDR_INTRA_FRAME;
                    }
                    else
                    {
                        if ((gtAVCEncoderFlow.encodedFrameCount % (mappingFramerate(gEncode_Framerate)*2)) == 1)
                        {
                            encIn.codingType = H264ENC_INTRA_FRAME;
                        }
                        else
                        {
                            encIn.codingType = H264ENC_PREDICTED_FRAME;
                        }
                    }
                }
                else
                {
                    if ((gtAVCEncoderFlow.encodedFrameCount % (mappingFramerate(gEncode_Framerate)*2)) == 1)
                    {
                        encIn.codingType = H264ENC_INTRA_FRAME;
                    }
                    else
                    {
                        encIn.codingType = H264ENC_PREDICTED_FRAME;
                    }
                 }
             }

             encIn.busLuma = gptAVCEncoder[gEncodeIdx]->pSourceBufAdrY[gptAVCEncoder[gEncodeIdx]->sourceBufSelect];
             encIn.busChromaU = gptAVCEncoder[gEncodeIdx]->pSourceBufAdrU[gptAVCEncoder[gEncodeIdx]->sourceBufSelect];
             encIn.busChromaV = gptAVCEncoder[gEncodeIdx]->pSourceBufAdrV[gptAVCEncoder[gEncodeIdx]->sourceBufSelect];

             encIn.ipf = H264ENC_REFERENCE_AND_REFRESH;
             encIn.ltrf = H264ENC_REFERENCE;

             gCapQueCnt--;
             
             //EnStart = PalGetClock();
             //printf("H264 start\n");
             ret = H264EncStrmEncode(gptAVCEncoder[gEncodeIdx]->encoder, &encIn, &encOut, &H264SliceReady, NULL, NULL);
             //if (encIn.codingType == H264ENC_INTRA_FRAME || encIn.codingType == H264ENC_NONIDR_INTRA_FRAME)
             //    printf("EnTime %d\n", PalGetDuration(EnStart));

             switch (ret)
             {
                  case H264ENC_FRAME_READY:

                      if (encOut.codingType == H264ENC_NOTCODED_FRAME)
                      {
                          printf("Not Coded Frame %d\n", PalGetDuration(EnStart));                      	  
                          break;
                      }
                      
                      for (i = gSliceNum; i < (encOut.numNalus-1); i++)
                      {
                          timeoutCnt = 100;
                          //if (gSliceNum != 8)
                          //    printf("NALNum %d %d %d\n", encOut.numNalus-1, gSliceNum, i);
                          ithInvalidateDCacheRange(&encOut.pNaluSizeBuf[i], 32);

                          while (encOut.pNaluSizeBuf[i] == 0 && timeoutCnt > 0)
                          {
                            ithInvalidateDCacheRange(&encOut.pNaluSizeBuf[i], 32);
                            ithDelay(100);
                            timeoutCnt--;
                          }

                          if (encOut.pNaluSizeBuf[i] == 0)
                          {
                              printf("Error Size(1) %d %d %d %d\n", i, encOut.pNaluSizeBuf[i], encOut.pNaluSizeBuf[i+1], encOut.streamSize);
                          }


                          ithInvalidateDCacheRange(gLastSlicePtr, encOut.pNaluSizeBuf[i]);
                          //memcpy(gptAVCEncoder[gEncodeIdx]->pStreamBufAdr[gVideoWrIdx[gEncodeIdx]], gLastSlicePtr, encOut.pNaluSizeBuf[i]);
                          //gLastSlicePtr+=encOut.pNaluSizeBuf[i];
                          //
                          streamInfo.streamIndex  = gEncodeIdx;
                          streamInfo.bufIndex     = gVideoWrIdx[gEncodeIdx];
                          streamInfo.dataSize     = encOut.pNaluSizeBuf[i];
                          gPreVidSize             = encOut.streamSize;
                          streamInfo.timeStamp    = gtAVCEncoderFlow.encodedTimeStamp;
                          if (i==(encOut.numNalus-2))
                              streamInfo.bLastSlice   = MMP_TRUE;
                          else
                              streamInfo.bLastSlice   = MMP_FALSE;
#ifdef CFG_IPTV_TX
                          if (gptAVCEncoder[0]->interlaced_frame)
                          {
                              if (i==0)
                              {
                                streamInfo.bIFrame      = (encIn.codingType == H264ENC_INTRA_FRAME) ? 1 : 0;
                                gbPrevIFrame = streamInfo.bIFrame;
                              }
                              else
                              {
                                streamInfo.bIFrame      = 0;
                              }
                          }
                          else
                          {
                              if (!gEncode_LowLatency)
                              {
                                streamInfo.bIFrame      = (encIn.codingType == H264ENC_INTRA_FRAME) ? 1 : 0;
                              }
                              else
                              {
                                streamInfo.bIFrame      = 0;
                              }
                          }
                          streamInfo.pData        = gLastSlicePtr;
                          streamInfo.frameCount   = gtAVCEncoderFlow.encodedFrameCount;
                          streamInfo.InstanceNum  = gEncodeIdx;

                          streamInfo.frameRate    = gtAVCEncoderFlow.frameRate;
                          streamInfo.EnFrameRate  = gtAVCEncoderFlow.frameRate;
                          streamInfo.width        = gptAVCEncoder[gEncodeIdx]->frameWidth;
                          streamInfo.height       = gptAVCEncoder[gEncodeIdx]->frameHeight;
                          streamInfo.frameType    = (gptAVCEncoder[gEncodeIdx]->bIFrame) ? 0 : 1;
                          streamInfo.bitRate      = gEncode_Bitrate;
                          streamInfo.bSkipFrame   = gtAVCEncoderFlow.bSkipFrame;
                          streamInfo.bColorRange  = gbColorRange;
                          streamInfo.encoder_Info = gCapture_info;
#endif
                          //gLastSlicePtr+=encOut.pNaluSizeBuf[i];
                          //printf("streamsize=%d(%d)<%x, %x, %x, %x, %x>\n", encOut.streamSize, streamInfo.bIFrame,
                          //                              pData[0], pData[1], pData[2], pData[3], pData[4]);
                          //
                          gLastSlicePtr+=encOut.pNaluSizeBuf[i];
                          gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]]  = MMP_FALSE;
                          //
                          gVideoSelIdx[gEncodeIdx]                            = (gVideoSelIdx[gEncodeIdx] + 1) % VIDEO_SAMPLE_NUM;

                          gfpCallback(&streamInfo);
                          gVideoWrIdx[gEncodeIdx] = (gVideoWrIdx[gEncodeIdx] + 1) % VIDEO_SAMPLE_NUM;
#ifdef CFG_IPTV_TX
                          gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]]  = MMP_FALSE;
#endif

                      }

                      gSliceNum = 0;
                      break;

                  case H264ENC_SYSTEM_ERROR:
                      printf("H264ENC_SYSTEM_ERROR\n");
                      break;

                  case H264ENC_OUTPUT_BUFFER_OVERFLOW:
                      printf("H264ENC_OUTPUT_BUFFER_OVERFLOW\n");
                      break;
                  case H264ENC_HW_TIMEOUT:
                      printf("H264ENC_HW_TIMEOUT\n");
                      break;
                  default:
                      printf("H264EncStrmEncode(%d) failed.", ret);
                      break;
             }
         }

         usleep(1000);
    }
    gVideoEncoding[0] = MMP_FALSE;
}

#else

static void*
encoder_task(
    void *arg)
{
#ifdef CFG_IPTV_TX
    MMP_UINT32  streamLen;
    MMP_BOOL    bFrmEnd;
#endif
    CAPTURE_RESULT tEntry = {0};
    MMP_UINT8           *pData;
#ifdef CFG_IPTV_TX
	MMP_UINT32          dataSize    = 2000 * 1024; // tmp solution
#else
    MMP_UINT32          dataSize    = 500 * 1024; // tmp solution
#endif
	VIDEO_STREAM_INFO   streamInfo;
    H264EncIn encIn;
    H264EncOut encOut;
    int ret;
    MMP_UINT32  accSize = 0;

    gVideoEncoding[0] = MMP_TRUE;
    while (gbStream0Enable && gbVideoEncoder)
    {

         if (QUEUE_NOT_EMPTY == gtAVCEncoderFlow.pfEnDequeue(&tEntry) && gbEncodeStart)
         {
             gEncodeIdx = tEntry.resIdx;

             if (gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]])
             {
                 ithPrintf("Encoding Buf Full\n");
                 continue;
             }

             gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]]  = MMP_FALSE;

             gVideoSelIdx[gEncodeIdx]                            = (gVideoSelIdx[gEncodeIdx] + 1) % VIDEO_SAMPLE_NUM;

             gtAVCEncoderFlow.encodedFrameCount                  = tEntry.frameCount;
             gtAVCEncoderFlow.encodedTimeStamp                   = tEntry.timeStamp;

             gptAVCEncoder[gEncodeIdx]->sourceBufSelect          = tEntry.frameIndex;
             //gbEncodeFire                                        = MMP_TRUE;

             pData      = gptAVCEncoder[gEncodeIdx]->pStreamBufAdr[gVideoWrIdx[gEncodeIdx]];

             memset (&encIn, 0, sizeof(H264EncIn));
             encIn.pOutBuf = pData;
             encIn.busOutBuf = pData;
             encIn.outBufSize = dataSize;

#ifndef CFG_IPTV_TX            
#if defined(CFG_CPU_WB) && (!defined(WIN32))
             ithInvalidateDCacheRange(pData, dataSize);
#endif
#endif
             if (gtAVCEncoderFlow.encodedFrameCount == 1)
                 encIn.timeIncrement = 0;
             else
                 encIn.timeIncrement = 1;

             if ((gtAVCEncoderFlow.encodedFrameCount % (mappingFramerate(gEncode_Framerate)*2)) == 1)
                 encIn.codingType = H264ENC_INTRA_FRAME;
             else
                 encIn.codingType = H264ENC_PREDICTED_FRAME;

             if (encIn.codingType == H264ENC_INTRA_FRAME)
                 accSize = 0;
                 
             encIn.busLuma = gptAVCEncoder[gEncodeIdx]->pSourceBufAdrY[gptAVCEncoder[gEncodeIdx]->sourceBufSelect];
             encIn.busChromaU = gptAVCEncoder[gEncodeIdx]->pSourceBufAdrU[gptAVCEncoder[gEncodeIdx]->sourceBufSelect];
             encIn.busChromaV = gptAVCEncoder[gEncodeIdx]->pSourceBufAdrV[gptAVCEncoder[gEncodeIdx]->sourceBufSelect];

             encIn.ipf = H264ENC_REFERENCE_AND_REFRESH;
             encIn.ltrf = H264ENC_REFERENCE;

             //printf("H264 start %d %x\n", gptAVCEncoder[gEncodeIdx]->sourceBufSelect, encIn.pOutBuf);
             ret = H264EncStrmEncode(gptAVCEncoder[gEncodeIdx]->encoder, &encIn, &encOut, NULL, NULL, NULL);

             switch (ret)
             {
                  case H264ENC_FRAME_READY:

                      {
                      //temp solution for network overflow
                      if (encOut.streamSize == 0 || accSize > gEncode_Bitrate * 170)
                      {
                      	  //printf("Skip Frame %d %d\n", encOut.streamSize, accSize);                      	  
                          gVideoSelIdx[gEncodeIdx]                            = (gVideoSelIdx[gEncodeIdx] + VIDEO_SAMPLE_NUM - 1) % VIDEO_SAMPLE_NUM;
                          gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]]  = MMP_FALSE;
                      	  break;
                      }
                      
                      ithInvalidateDCacheRange(pData, encOut.streamSize);

                      streamInfo.streamIndex  = gEncodeIdx;
                      streamInfo.bufIndex     = gVideoWrIdx[gEncodeIdx];
                      streamInfo.bIFrame      = (encIn.codingType == H264ENC_INTRA_FRAME) ? 1 : 0;
                      streamInfo.dataSize     = encOut.streamSize;
                      streamInfo.timeStamp    = gtAVCEncoderFlow.encodedTimeStamp;
                      streamInfo.bLastSlice   = MMP_TRUE;
#ifdef CFG_IPTV_TX
                      streamInfo.pData        = pData;
                      streamInfo.frameCount   = gtAVCEncoderFlow.encodedFrameCount;
                      streamInfo.InstanceNum  = gEncodeIdx;
                       
                      streamInfo.frameRate    = gtAVCEncoderFlow.frameRate;
                      streamInfo.EnFrameRate  = gtAVCEncoderFlow.frameRate;
                      streamInfo.width        = gptAVCEncoder[gEncodeIdx]->frameWidth;
                      streamInfo.height       = gptAVCEncoder[gEncodeIdx]->frameHeight;
                      streamInfo.frameType    = (gptAVCEncoder[gEncodeIdx]->bIFrame) ? 0 : 1;
                      streamInfo.bitRate      = gEncode_Bitrate;
                      streamInfo.bSkipFrame   = gtAVCEncoderFlow.bSkipFrame;

                      streamInfo.encoder_Info = gCapture_info;
#endif          
                      accSize += encOut.streamSize;
                      //printf("streamsize=%d(%d)<%x, %x, %x, %x, %x> timeStamp:%d\n", encOut.streamSize, streamInfo.bIFrame,
                      //						pData[0], pData[1], pData[2], pData[3], pData[4],streamInfo.timeStamp);
                      
                      gfpCallback(&streamInfo);
                      gVideoWrIdx[gEncodeIdx] = (gVideoWrIdx[gEncodeIdx] + 1) % VIDEO_SAMPLE_NUM;           
#ifdef CFG_IPTV_TX
                      gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]]  = MMP_FALSE;
#endif
                      }

                      break;

                  case H264ENC_SYSTEM_ERROR:
                      printf("H264ENC_SYSTEM_ERROR\n");
                      break;

                  case H264ENC_OUTPUT_BUFFER_OVERFLOW:
                      printf("H264ENC_OUTPUT_BUFFER_OVERFLOW\n");
                      break;

                  case H264ENC_HW_TIMEOUT:
                      printf("H264ENC_HW_TIMEOUT\n");
                      break;

                  default:
                      printf("H264EncStrmEncode(%d) failed.", ret);
                      break;
             }
         }

         usleep(1000);
    }
    gVideoEncoding[0] = MMP_FALSE;
    return 0;
}
#endif

static MMP_BOOL
_Chk_Skip_Frame(
    MMP_UINT32 InstanceNum)
{
    MMP_UINT32  skipNum;
    MMP_BOOL    bSkipFrm = MMP_FALSE;

    if (gtAVCEncoderFlow.frameRateDiff[InstanceNum] == 0 ||
        (!gbStream0Enable && InstanceNum == 0) || (!gbStream1Enable && InstanceNum == 1))
    {
        bSkipFrm = MMP_FALSE;
        goto End;
    }

    skipNum = (gtAVCEncoderFlow.ispFrameRate[InstanceNum] / gtAVCEncoderFlow.frameRateDiff[InstanceNum]) * gtAVCEncoderFlow.skipCount[InstanceNum];

    if (gtAVCEncoderFlow.frameCount[InstanceNum]++ == skipNum)
    {
        if (gtAVCEncoderFlow.skipCount[InstanceNum]++ == gtAVCEncoderFlow.frameRateDiff[InstanceNum])
            gtAVCEncoderFlow.skipCount[InstanceNum] = 1;

        if (gtAVCEncoderFlow.frameCount[InstanceNum] == (gtAVCEncoderFlow.ispFrameRate[InstanceNum] + 1))
            gtAVCEncoderFlow.frameCount[InstanceNum] = 1;

        bSkipFrm = MMP_TRUE;
    }
    else
        bSkipFrm = MMP_FALSE;

End:

    //ithPrintf("IsSkip(%d) FC %d SkipCount %d SkipNum %d\n", bSkipFrm, gtAVCEncoderFlow.frameCount[InstanceNum], gtAVCEncoderFlow.skipCount[InstanceNum], skipNum);

    return bSkipFrm;
}

static int
_Chk_Cap_Fire(
    CAPTURE_HANDLE* ptDev)
{
    CAP_CONTEXT *Capctxt = &ptDev->cap_info;
    MMP_UINT32 timeout = 0;
    
    while((ithCapGetEngineErrorStatus(ptDev,MMP_CAP_LANE0_STATUS)&0xf) != 0xf)
    {
        if(timeout++ > 200)
        {
            printf("wait capture stable timeout.\n");
            return 1;
        }
        usleep(10*1000);
    }

    ithCapFire(ptDev, MMP_TRUE);
    printf("Capture Fire(0)\n");
    gptAVCEncoder[0]->framePitchY = Capctxt->ininfo.PitchY;
    gptAVCEncoder[0]->frameWidth = Capctxt->outinfo.OutWidth;
    gptAVCEncoder[0]->frameHeight = Capctxt->outinfo.OutHeight; 
    gptAVCEncoder[0]->interlaced_frame = Capctxt->ininfo.Interleave;

    return 0;
}

static void 
_Update_Field_State(void)
{
    gTopBottomIndex = !gTopBottomIndex;
    //ithPrintf("gTopBottomIndex = %d \n", gTopBottomIndex);
}

static void 
_Set_Field_Reset(void)
{
    gTopBottomIndex = 0;
}

static MMP_BOOL 
_Get_Field_State(void)
{
    return gTopBottomIndex;
}


ISP_BUFFER_INFO* VideoEncoder_GetISPBUFFERINFO()
{
}

void VideoEncoder_SetWidthHeight(MMP_UINT32 width,MMP_UINT32 height)
{
    gEncode_Width = width;
    gEncode_Height = height;
    gEncode_PitchUV = gEncode_PitchY = ((width + 15) >> 4) << 4;
    printf("VideoEncoder_SetWidthHeight w:%d h:%d py = %d, pu =%d\n", gEncode_Width, gEncode_Height, gEncode_PitchY, gEncode_PitchUV);
}

void VideoEncoder_SetFrameRate(
    MMP_CAP_FRAMERATE framerate)
{
    gEncode_Framerate = framerate;
    printf("VideoEncoder_SetFrameRate frate_in:%d frate_out:%d\n", framerate,gEncode_Framerate);
}

VIDEO_ENCODER_API void VideoEncoder_SetBitRate(
    MMP_UINT32 bitrate)
{
    gEncode_Bitrate = bitrate;
    printf("VideoEncoder_SetBitRate bitrate:%d \n", gEncode_Bitrate);
}

VIDEO_ENCODER_API void VideoEncoder_GetWidthHeight(
    MMP_UINT32 *width,
    MMP_UINT32 *height)
{
    *width = gEncode_Width;
    *height = gEncode_Height;
}

VIDEO_ENCODER_API void VideoEncoder_GetFrameRate(
    MMP_CAP_FRAMERATE *framerate)
{
    *framerate = gEncode_Framerate;
}

VIDEO_ENCODER_API MMP_UINT32 VideoEncoder_GetBitRate()
{
    return gEncode_Bitrate;
}

VIDEO_ENCODER_API void VideoEncoder_SetInterlanced(
    MMP_UINT32 interlanced)
{
    gEncode_Interlanced = interlanced; 
}

VIDEO_ENCODER_API MMP_BOOL VideoEncoder_GetInterlanced(
    void)
{
    return gEncode_Interlanced;
}

VIDEO_ENCODER_API void VideoEncoder_SetLowLatency(
    MMP_BOOL low_latency)
{
    gEncode_LowLatency = low_latency;
}

VIDEO_ENCODER_API MMP_BOOL VideoEncoder_GetLowLatency(
    void)
{
    return gEncode_LowLatency;
}

VIDEO_ENCODER_API void VideoEncoder_SetCap_Info(
    MMP_CAP_INPUT_INFO capture_info)
{
    gCapture_info = capture_info;
}


VIDEO_ENCODER_API void VideoEncoder_SetColorRange(
    MMP_BOOL color_range)
{
    gbColorRange = color_range;
    printf("VideoEncoder_SetColorRange :%d \n", gbColorRange);
}

