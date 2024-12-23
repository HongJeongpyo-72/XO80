#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "mediastreamer2/mediastream.h"
#include "video_encoder/video_encoder_it970.h"
#include "capture_970/mmp_capture.h"
#include "h264encapi.h"
#include "ite/ith.h"
#include "ite/itp.h"

/*#define SENSOR_WIDTH 0
#define SENSOR_HEIGHT 0
#define SENSOR_BITRATE 0
#define SENSOR_WIDTH_2 0
#define SENSOR_HEIGHT_2 0
#define SENSOR_BITRATE_2 0*/

//=============================================================================
//                Constant Definition
//=============================================================================

//#define CFG_DUAL_STREAM
//#define TEST_MULTISTREAM

#ifdef TEST_MULTISTREAM
#define CFG_DUAL_STREAM
#define SENSOR_WIDTH_2 320
#define SENSOR_HEIGHT_2 240
#define SENSOR_BITRATE_2 400
#endif


#define CAPTURE_RESULT_QUEUE_SIZE   30
#define ENCODED_RESULT_QUEUE_SIZE   CAPTURE_RESULT_QUEUE_SIZE
#define QUEUE_EMPTY                 (-1)
#define QUEUE_NOT_EMPTY             (0)
#define TIMER_NUM                   4
#if defined(CFG_DUAL_STREAM)
    #define ENCODE_STREAM_NUM       2
    #define VIDEO_STREAM_NUM        2
    #define VIDEO_SAMPLE_NUM        7

#else
    #define ENCODE_STREAM_NUM       1
    #define VIDEO_STREAM_NUM        1
#ifdef MULTI_SLICE
    #define VIDEO_SAMPLE_NUM        30
#else
    #define VIDEO_SAMPLE_NUM        7
#endif

#endif

#if !defined(CFG_SENSOR_ENABLE)
#define SENSOR_WIDTH            1280
#define SENSOR_HEIGHT           720
#define SENSOR_BITRATE          10000
#endif

#define VIDEO_FRAME_RATE_30     30
#define VIDEO_FRAME_RATE_25     25

#if defined (CFG_SENSOR_FLICK_50HZ)
    #define    VIDEO_FRAME_RATE VIDEO_FRAME_RATE_25
#else
    #define    VIDEO_FRAME_RATE VIDEO_FRAME_RATE_30
#endif

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
//    VIDEO_ENCODER_INPUT_INFO    encoderInfo;
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
    MMP_UINT8*  pSourceBufAdrY[5];
    MMP_UINT8*  pSourceBufAdrU[5];
    MMP_UINT8*  pSourceBufAdrV[5];
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
} H264_AVC_ENCODER;

pthread_mutex_t h264_mutex = PTHREAD_MUTEX_INITIALIZER;

//=============================================================================
//                Global Data Definition
//=============================================================================

static H264_AVC_ENCODER         *gptAVCEncoder[ENCODE_STREAM_NUM];
static AVC_ENCODER_FLOW         gtAVCEncoderFlow;
static CAPTURE_RESULT_QUEUE     gtCaptureResultQ;
static ENCODED_RESULT_QUEUE     gtEncodedResultQ;
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
static pthread_mutex_t          JPEGEngineMutex                         = PTHREAD_MUTEX_INITIALIZER;


static MMP_UINT32               gVideoEnWidth[2];
static MMP_UINT32               gVideoEnHeight[2];
static MMP_UINT32               gVideoEnBitRate[2];
static MMP_BOOL                 gStartgetVideo[VIDEO_STREAM_NUM];
static MMP_BOOL                 gVideoEncoding[VIDEO_STREAM_NUM];
static MMP_UINT8                gVideoUserNum[VIDEO_STREAM_NUM];

MMP_UINT32                      tickStart;
static MMP_BOOL                 bencoding = MMP_FALSE;

#ifdef   LOG_ENCODER_PERFORMANCE
MMP_UINT32                      tickStart, tickEnd;
unsigned                        ticks_per_ms;
MMP_UINT32                      Enidx = 0;
MMP_UINT32                      EnTime;
#endif

static pthread_t                gEncoder_process_tid;
static MMP_UINT32               WriteBufferIndex = 0;
static MMP_BOOL                 gbEncodeStart = MMP_FALSE;

static MMP_UINT32               stream_buf_vram_addr = 0;
static MMP_UINT8*               stream_buf_sys_addr = 0;
static MMP_UINT8*               gLastSlicePtr = 0;
static MMP_UINT32               gSliceNum = 0;
static ISP_BUFFER_INFO*         gISPBUUFERINFO = 0;          
static VideoStream              *stream;
static MMP_BOOL                 with_preview = MMP_FALSE;

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

static void*
encoder_task(
    void *arg);

static MMP_BOOL
_Chk_Skip_Frame(
    MMP_UINT32 InstanceNum);

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

    if (frameRate <= 15)
        pf = _frameCount2TimeStamp_TABLE[frameRate];

    return pf;
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

	if (gbJPEGEncoder)  // just open for JPEG encode
	{
		if(gOpenEnginCnt == 1)
			WriteBufferIndex = 0;
		
	    if(stream == NULL)
		    stream = cam_playback_start(MMP_FALSE, MMP_FALSE);
        goto o_end;
	}	
    /***************************************************/
    memset (&encIn, 0, sizeof(H264EncIn));
    encIn.pOutBuf = (u32 *)(gptAVCEncoder[0]->pHdrBufAddr[0]);
    encIn.busOutBuf = (u32 *)(gptAVCEncoder[0]->pHdrBufAddr[0]);
    encIn.outBufSize = 2048;

    /* Start stream */
    ret = H264EncStrmStart(gptAVCEncoder[0]->encoder, &encIn, &encOut);
    if(ret != H264ENC_OK)
    {
        printf("H264EncStrmStart(0) failed. %x\n", ret);
        //return -1;
    }

    gptAVCEncoder[0]->ParaSetHdrSize[0] = encOut.pNaluSizeBuf[0];
    gptAVCEncoder[0]->ParaSetHdrSize[1] = encOut.pNaluSizeBuf[1];
    gptAVCEncoder[0]->pHdrBufAddr[1] = gptAVCEncoder[0]->pHdrBufAddr[0] + encOut.pNaluSizeBuf[0];

    printf("SPS PPS stream(0) %d %d %d %x %x\n", encOut.numNalus, encOut.pNaluSizeBuf[0], encOut.pNaluSizeBuf[1], gptAVCEncoder[0]->pHdrBufAddr[0], gptAVCEncoder[0]->pHdrBufAddr[1]);

#if defined(CFG_DUAL_STREAM)
    memset (&encIn, 0, sizeof(H264EncIn));
    encIn.pOutBuf = (u32 *)(gptAVCEncoder[1]->pHdrBufAddr[0]);
    encIn.busOutBuf = (u32 *)(gptAVCEncoder[1]->pHdrBufAddr[0]);
    encIn.outBufSize = 2048;

    /* Start stream */
    ret = H264EncStrmStart(gptAVCEncoder[1]->encoder, &encIn, &encOut);
    if(ret != H264ENC_OK)
    {
        printf("H264EncStrmStart(1) failed. %x", ret);
        //return -1;
    }

    gptAVCEncoder[1]->ParaSetHdrSize[0] = encOut.pNaluSizeBuf[0];
    gptAVCEncoder[1]->ParaSetHdrSize[1] = encOut.pNaluSizeBuf[1];
    gptAVCEncoder[1]->pHdrBufAddr[1] = gptAVCEncoder[1]->pHdrBufAddr[0] + encOut.pNaluSizeBuf[0];

    printf("SPS PPS stream(1) %d %d %d %x %x\n", encOut.numNalus, encOut.pNaluSizeBuf[0], encOut.pNaluSizeBuf[1], gptAVCEncoder[1]->pHdrBufAddr[0], gptAVCEncoder[1]->pHdrBufAddr[1]);

#endif

    gbEncodeStart = MMP_TRUE;

	memset(&gtAVCEncoderFlow, 0, sizeof(gtAVCEncoderFlow));
    gtAVCEncoderFlow.baseTimeStamp = 0;
    gtAVCEncoderFlow.frameRate                          = gtAVCEncoderFlow.captureFrameRate
#if defined(CFG_SENSOR_FLICK_50HZ)
                                                        = MMP_CAP_FRAMERATE_25HZ;
#else
                                                        = MMP_CAP_FRAMERATE_30HZ;
#endif

    gtAVCEncoderFlow.pfToTimeStamp                      = _VIDEO_ENCODER_GetTimeStampConverter(gtAVCEncoderFlow.frameRate);

#if defined(CFG_DUAL_STREAM)
    gtAVCEncoderFlow.pfEnDequeue = _CaptureResultQ_Dequeue;

    WriteBufferIndex = 0;

    gtAVCEncoderFlow.frameCount[0]      = 1;
    gtAVCEncoderFlow.skipCount[0]       = 1;

    gtAVCEncoderFlow.frameCount[1]      = 1;
    gtAVCEncoderFlow.skipCount[1]       = 1;
#else //else defined(CFG_DUAL_STREAM)
    gtAVCEncoderFlow.pfEnDequeue = _CaptureResultQ_Dequeue;

    WriteBufferIndex = 0;

    gtAVCEncoderFlow.frameCount[0]      = 1;
    gtAVCEncoderFlow.skipCount[0]       = 1;
#endif //end defined(CFG_DUAL_STREAM)

    pthread_create(&gEncoder_process_tid, NULL, encoder_task, NULL);

    if(stream == NULL)
        stream = cam_playback_start(with_preview, MMP_FALSE);

#ifdef LOW_LATENCY        //temp solution
    {
    CAPTURE_RESULT  tEntry          = {0};
    MMP_UINT32      WBIndex         = WriteBufferIndex;

    WriteBufferIndex++;
    WriteBufferIndex %= gptAVCEncoder[0]->sourceBufCount;

    tEntry.frameIndex = WBIndex;
    tEntry.frameCount   = ++gtAVCEncoderFlow.captureFrameCount;

    if (gtAVCEncoderFlow.pfToTimeStamp)
    {
        tEntry.timeStamp                = gtAVCEncoderFlow.currTimeStamp
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
        goto c_end;

    if (type == JPEG_ENCODER)
    {
        gbJPEGEncoder = MMP_FALSE;
        if (!gbVideoEncoder)
            gbStream0Enable = MMP_FALSE;
    }

    if (type == H264_ENCODER)
    {
        gbVideoEncoder = MMP_FALSE;
        //wait encoder task finish
#if defined(CFG_DUAL_STREAM)    
        while(gVideoEncoding[0] == MMP_TRUE || gVideoEncoding[1] == MMP_TRUE)
#else        
        while(gVideoEncoding[0] == MMP_TRUE)
#endif    
            usleep(1000);  

        gbStream0Enable = MMP_FALSE;
        gbStream1Enable = MMP_FALSE;
        gbEncodeStart = MMP_FALSE;
    }

    gOpenEnginCnt--;

    if(stream != NULL && gOpenEnginCnt == 0)
    {
	    cam_playback_stop(stream, with_preview);
        stream = NULL;
    }
    
    if(type == JPEG_ENCODER)
    {
        _WaitAllQueue_Empty();
        goto c_end;
    }  

    _WaitAllQueue_Empty();
    WriteBufferIndex = 0;

    memset (&encIn, 0, sizeof(H264EncIn));
    encIn.pOutBuf = (u32 *)(gptAVCEncoder[0]->pHdrBufAddr[0]);
    encIn.busOutBuf = (u32 *)(gptAVCEncoder[0]->pHdrBufAddr[0]);
    encIn.outBufSize = 2048;

    ret = H264EncStrmEnd(gptAVCEncoder[0]->encoder, &encIn, &encOut);
    if(ret != H264ENC_OK)
    {
        printf("H264EncStrmEnd(0) failed. %x\n", ret);
    }
    else
        printf("H264EncStrmEnd(0).\n");

#if defined(CFG_DUAL_STREAM)
    // temp solution
    memset (&encIn, 0, sizeof(H264EncIn));
    encIn.pOutBuf = (u32 *)(gptAVCEncoder[1]->pHdrBufAddr[0]);
    encIn.busOutBuf = (u32 *)(gptAVCEncoder[1]->pHdrBufAddr[0]);
    encIn.outBufSize = 2048;

    ret = H264EncStrmEnd(gptAVCEncoder[1]->encoder, &encIn, &encOut);

    if(ret != H264ENC_OK)
    {
        printf("H264EncStrmEnd(1) failed. %x", ret);
    }
    else
        printf("H264EncStrmEnd(1).\n");
#endif
    pthread_join(gEncoder_process_tid, NULL);
c_end:
    pthread_mutex_unlock(&VideoEngineMutex);
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
    H264EncConfig cfg0, cfg1;
    H264EncCodingCtrl codingCfg0, codingCfg1;
    H264EncRateCtrl rcCfg0, rcCfg1;
    H264EncIn encIn;
    H264EncOut encOut;
    H264EncPreProcessingCfg preProcCfg0, preProcCfg1;

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

    gISPBUUFERINFO = (ISP_BUFFER_INFO*)malloc(sizeof(ISP_BUFFER_INFO));

    gptAVCEncoder[0]->sourceBufCount = 3;
    gptAVCEncoder[0]->streamBufCount = VIDEO_SAMPLE_NUM;

    size = (SENSOR_WIDTH * SENSOR_HEIGHT * 3 * gptAVCEncoder[0]->sourceBufCount) >> 1;

    gptAVCEncoder[0]->buf_vram_addr = itpVmemAlignedAlloc(8, size);
    gptAVCEncoder[0]->buf_sys_addr = (uint8_t*) ithMapVram(gptAVCEncoder[0]->buf_vram_addr, size, /*ITH_VRAM_READ |*/ ITH_VRAM_WRITE);

    for (i = 0; i < gptAVCEncoder[0]->sourceBufCount; i++)
    {
        gptAVCEncoder[0]->pSourceBufAdrY[i] = gptAVCEncoder[0]->buf_sys_addr + ((SENSOR_WIDTH * SENSOR_HEIGHT * 3 * i)/2);
        gptAVCEncoder[0]->pSourceBufAdrU[i] = gptAVCEncoder[0]->pSourceBufAdrY[i] + (SENSOR_WIDTH * SENSOR_HEIGHT);
        gptAVCEncoder[0]->pSourceBufAdrV[i] = gptAVCEncoder[0]->pSourceBufAdrU[i];

        gISPBUUFERINFO->pISPBufAdrY[i]      = gptAVCEncoder[0]->pSourceBufAdrY[i];
        gISPBUUFERINFO->pISPBufAdrU[i]      = gptAVCEncoder[0]->pSourceBufAdrU[i];
        //printf("++++++++init addr=(%x, %x, %x)\n", gptAVCEncoder[0]->pSourceBufAdrY[i], gptAVCEncoder[0]->pSourceBufAdrU[i], gptAVCEncoder[0]->pSourceBufAdrV[i]);
    }
    
#if defined(CFG_DUAL_STREAM)
    //using the same source, scaling by encoding
    gptAVCEncoder[1]->sourceBufCount = 3;
    gptAVCEncoder[1]->streamBufCount = VIDEO_SAMPLE_NUM;

    //size = (SENSOR_WIDTH_2 * SENSOR_HEIGHT_2 * 3 * gptAVCEncoder[1]->sourceBufCount) >> 1;

    //gptAVCEncoder[1]->buf_vram_addr = itpVmemAlignedAlloc(8, size);
    //gptAVCEncoder[1]->buf_sys_addr = (uint8_t*) ithMapVram(gptAVCEncoder[1]->buf_vram_addr, size, /*ITH_VRAM_READ |*/ ITH_VRAM_WRITE);

    for (i = 0; i < gptAVCEncoder[1]->sourceBufCount; i++)
    {
        gptAVCEncoder[1]->pSourceBufAdrY[i] = gptAVCEncoder[0]->pSourceBufAdrY[i];
        gptAVCEncoder[1]->pSourceBufAdrU[i] = gptAVCEncoder[0]->pSourceBufAdrU[i];
        gptAVCEncoder[1]->pSourceBufAdrV[i] = gptAVCEncoder[0]->pSourceBufAdrV[i];

        gISPBUUFERINFO->pISPBufAdrY_2[i]   = gptAVCEncoder[1]->pSourceBufAdrY[i];
        gISPBUUFERINFO->pISPBufAdrU_2[i]   = gptAVCEncoder[1]->pSourceBufAdrU[i];
    }
#endif    

    /*only display capture video , don't  allocate videoencoder  output buffer*/
    if(mode == ONLYPREVIEW_MODE)
        return;

    size = 20 * 1024;

    gptAVCEncoder[0]->hdr_buf_vram_addr = itpVmemAlignedAlloc(8, size);
    gptAVCEncoder[0]->hdr_buf_sys_addr = (uint8_t*) ithMapVram(gptAVCEncoder[0]->hdr_buf_vram_addr, size, /*ITH_VRAM_READ |*/ ITH_VRAM_WRITE);

    gptAVCEncoder[0]->pHdrBufAddr[0] = gptAVCEncoder[0]->hdr_buf_sys_addr;
    gptAVCEncoder[0]->pHdrBufAddr[1] = gptAVCEncoder[0]->hdr_buf_sys_addr + 10 * 1024;
    gptAVCEncoder[0]->ParaSetHdrSize[0] = 10 *1024;
    gptAVCEncoder[0]->ParaSetHdrSize[1] = 10 * 1024;

//#ifdef MULTI_SLICE
    //stream buffer for multi-slice && low latency mode
    size = 500*1024;
    stream_buf_vram_addr = itpVmemAlignedAlloc(8, size);
    stream_buf_sys_addr = (uint8_t*) ithMapVram(stream_buf_vram_addr, size, /*ITH_VRAM_READ |*/ ITH_VRAM_WRITE);
//#endif

#if defined(CFG_DUAL_STREAM)
    size = 20 * 1024;

    gptAVCEncoder[1]->hdr_buf_vram_addr = itpVmemAlignedAlloc(8, size);
    gptAVCEncoder[1]->hdr_buf_sys_addr = (uint8_t*) ithMapVram(gptAVCEncoder[1]->hdr_buf_vram_addr, size, /*ITH_VRAM_READ |*/ ITH_VRAM_WRITE);

    gptAVCEncoder[1]->pHdrBufAddr[0] = gptAVCEncoder[1]->hdr_buf_sys_addr;
    gptAVCEncoder[1]->pHdrBufAddr[1] = gptAVCEncoder[1]->hdr_buf_sys_addr + 10 * 1024;
    gptAVCEncoder[1]->ParaSetHdrSize[0] = 10 *1024;
    gptAVCEncoder[1]->ParaSetHdrSize[1] = 10 * 1024;

#endif

    memset (&cfg0, 0, sizeof(H264EncConfig));

    gptAVCEncoder[0]->frameWidth = gptAVCEncoder[0]->framePitchY = cfg0.width = SENSOR_WIDTH;
    gptAVCEncoder[0]->frameHeight = cfg0.height = SENSOR_HEIGHT;

#if defined(CFG_DUAL_STREAM)
    cfg0.scaledWidth = SENSOR_WIDTH_2;
    cfg0.scaledHeight = SENSOR_HEIGHT_2;
#else    
    cfg0.scaledWidth = 0;
    cfg0.scaledHeight = 0;
#endif

#ifdef CFG_IPTV_TX
    cfg0.frameRateDenom = 1;
    cfg0.frameRateNum = 60;
#else
    if (VIDEO_FRAME_RATE == VIDEO_FRAME_RATE_25)
    {
        cfg0.frameRateDenom = 1;
        cfg0.frameRateNum = 25;
    } else {
        cfg0.frameRateDenom = 1;
        cfg0.frameRateNum = 30;
    }
#endif
    cfg0.viewMode = H264ENC_BASE_VIEW_DOUBLE_BUFFER; /* Two buffers by default */
    cfg0.streamType = H264ENC_BYTE_STREAM; //H264ENC_NAL_UNIT_STREAM;

    tmp = (gptAVCEncoder[0]->frameWidth * gptAVCEncoder[0]->frameHeight) >> 8;

    if (tmp <= 396)
        cfg0.level = H264ENC_LEVEL_2;
    else if (tmp <= 792)
        cfg0.level = H264ENC_LEVEL_2_1;
    else if (tmp <= 1620)
        cfg0.level = H264ENC_LEVEL_3;
    else if (tmp <= 3600)
        cfg0.level = H264ENC_LEVEL_3_1;
    else if (tmp <= 5120)
        cfg0.level = H264ENC_LEVEL_3_2;
    else if (tmp <= 8192)
        cfg0.level = H264ENC_LEVEL_4;

    //cfg0.level = H264ENC_LEVEL_4;

    cfg0.refFrameCompress = 0;
    cfg0.rfcLumBufLimit = 0;
    cfg0.rfcChrBufLimit = 0;
    cfg0.refFrameAmount = 1;

    gbVideoEncoder = MMP_TRUE;
	WriteBufferIndex = 0;

    if((ret = H264EncInit(&cfg0, &gptAVCEncoder[0]->encoder)) != H264ENC_OK)
    {
        printf("H264EncInit(0) failed. %x", ret);
        //return (int)ret;
    }

    /* Encoder setup: rate control */
    if((ret = H264EncGetRateCtrl(gptAVCEncoder[0]->encoder, &rcCfg0)) != H264ENC_OK)
    {
        printf("H264EncGetRateCtrl(0) failed. %x", ret);
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

        rcCfg0.qpHdr = -1;
        rcCfg0.qpMin = 15;
        rcCfg0.qpMax = 50;
        rcCfg0.pictureSkip = 0;
        rcCfg0.pictureRc = 1;
        rcCfg0.mbRc = 1;
#ifdef CFG_IOT_ENABLE
        rcCfg0.bitPerSecond = 2000 * 1024;
#else
        rcCfg0.bitPerSecond = SENSOR_BITRATE * 1024;
#endif
        rcCfg0.hrd = 1;
        rcCfg0.hrdCpbSize = 0;
        rcCfg0.gopLen = VIDEO_FRAME_RATE;
        rcCfg0.intraQpDelta = -3;
        rcCfg0.fixedIntraQp = 0;
        rcCfg0.mbQpAdjustment = 0;
        rcCfg0.longTermPicRate = VIDEO_FRAME_RATE;
        rcCfg0.mbQpAutoBoost = 0;

        //printf("Set rate control: qp %2d [%2d, %2d] %8d bps  "
        //       "pic %d mb %d skip %d  hrd %d\n"
        //       "  cpbSize %d gopLen %d intraQpDelta %2d "
        //       "fixedIntraQp %2d mbQpAdjustment %d "
        //       "mbQpAutoBoost %d\n",
        //       rcCfg.qpHdr, rcCfg.qpMin, rcCfg.qpMax, rcCfg.bitPerSecond,
        //       rcCfg.pictureRc, rcCfg.mbRc, rcCfg.pictureSkip, rcCfg.hrd,
        //       rcCfg.hrdCpbSize, rcCfg.gopLen, rcCfg.intraQpDelta,
        //       rcCfg.fixedIntraQp, rcCfg.mbQpAdjustment, rcCfg.mbQpAutoBoost);

        if((ret = H264EncSetRateCtrl(gptAVCEncoder[0]->encoder, &rcCfg0)) != H264ENC_OK)
        {
            printf("H264EncSetRateCtrl(0) failed. %x", ret);
            //CloseEncoder(encoder);
            //return -1;
        }
    }

    /* Encoder setup: coding control */
    if((ret = H264EncGetCodingCtrl(gptAVCEncoder[0]->encoder, &codingCfg0)) != H264ENC_OK)
    {
        printf("H264EncGetCodingCtrl(0) failed. %x", ret);
        //CloseEncoder(encoder);
        //return -1;
    }
    else
    {
#ifdef MULTI_SLICE
        codingCfg0.sliceSize = 5;
#else
        codingCfg0.sliceSize = 0;
#endif
        codingCfg0.constrainedIntraPrediction = 0;
        codingCfg0.disableDeblockingFilter = 0;
        codingCfg0.enableCabac = 0;
        codingCfg0.cabacInitIdc = 0;
        codingCfg0.transform8x8Mode = 1;
#ifdef CFG_IPTV_TX
        codingCfg0.quarterPixelMv = 0;
#else
        codingCfg0.quarterPixelMv = 1;
#endif
        codingCfg0.videoFullRange = 1;
        codingCfg0.seiMessages = 0;
        codingCfg0.idrHeader = 0;
        codingCfg0.gdrDuration = 0;
        codingCfg0.fieldOrder = 0;
        codingCfg0.cirStart = 0;
        codingCfg0.cirInterval = 0;
#ifdef LOW_LATENCY
        //Low Latency
        codingCfg0.inputLineBufDepth = 2;
        codingCfg0.inputLineBufHwModeEn = 1;
        codingCfg0.inputLineBufEn = 1;
        codingCfg0.inputLineBufLoopBackEn = 1;
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

        if((ret = H264EncSetCodingCtrl(gptAVCEncoder[0]->encoder, &codingCfg0)) != H264ENC_OK)
        {
            printf("H264EncSetCodingCtrl(0) failed. %x", ret);
            //CloseEncoder(encoder);
            //return -1;
        }
    }
    if ((ret = H264EncGetPreProcessing(gptAVCEncoder[0]->encoder, &preProcCfg0)) != H264ENC_OK)
    {
        printf("H264EncGetPreProcessing(0) failed.\n", ret);
    } else {

        preProcCfg0.inputType = H264ENC_YUV420_SEMIPLANAR;
        preProcCfg0.origWidth = cfg0.width;
        preProcCfg0.origHeight = cfg0.height;
        preProcCfg0.videoStabilization = 0;
#if defined(CFG_DUAL_STREAM)        
        preProcCfg0.scaledOutput = 1;
#else
        preProcCfg0.scaledOutput = 0;
#endif        

        if ((ret = H264EncSetPreProcessing(gptAVCEncoder[0]->encoder, &preProcCfg0)) != H264ENC_OK)
        {
            printf("H264EncSetPreProcessing(0) failed.\n", ret);
        }
    }

#if defined(CFG_DUAL_STREAM)
    memset (&cfg1, 0, sizeof(H264EncConfig));
    
    gptAVCEncoder[1]->frameWidth = gptAVCEncoder[1]->framePitchY = cfg1.width = SENSOR_WIDTH_2;
    gptAVCEncoder[1]->frameHeight = cfg1.height = SENSOR_HEIGHT_2;
    cfg1.scaledWidth = 0;
    cfg1.scaledHeight = 0;
    
    if (VIDEO_FRAME_RATE == VIDEO_FRAME_RATE_25)
    {
        cfg1.frameRateDenom = 1;
        cfg1.frameRateNum = 25;
    } else {
        cfg1.frameRateDenom = 1;
        cfg1.frameRateNum = 30;
    }

    cfg1.viewMode = H264ENC_BASE_VIEW_DOUBLE_BUFFER; /* Two buffers by default */
    cfg1.streamType = H264ENC_BYTE_STREAM;;

    tmp = (gptAVCEncoder[1]->frameWidth * gptAVCEncoder[1]->frameHeight) >> 8;
    
    if (tmp <= 396)
        cfg1.level = H264ENC_LEVEL_2;
    else if (tmp <= 792)
        cfg1.level = H264ENC_LEVEL_2_1;
    else if (tmp <= 1620)
        cfg1.level = H264ENC_LEVEL_3;
    else if (tmp <= 3600)
        cfg1.level = H264ENC_LEVEL_3_1;
    else if (tmp <= 5120)
        cfg1.level = H264ENC_LEVEL_3_2;
    else if (tmp <= 8192)
        cfg1.level = H264ENC_LEVEL_4;

    //cfg.level = H264ENC_LEVEL_4;

    cfg1.refFrameCompress = 0;
    cfg1.rfcLumBufLimit = 0;
    cfg1.rfcChrBufLimit = 0;
    cfg1.refFrameAmount = 1;

    //_VIDEO_ENCODER_Init();
    if((ret = H264EncInit(&cfg1, &gptAVCEncoder[1]->encoder)) != H264ENC_OK)
    {
        printf("H264EncInit(1) failed. %x", ret);
        //return (int)ret;
    }

    /* Encoder setup: rate control */
    if((ret = H264EncGetRateCtrl(gptAVCEncoder[1]->encoder, &rcCfg1)) != H264ENC_OK)
    {
        printf("H264EncGetRateCtrl(1) failed. %x", ret);
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

        rcCfg1.qpHdr = -1;
        rcCfg1.qpMin = 15;
        rcCfg1.qpMax = 50;
        rcCfg1.pictureSkip = 0;
        rcCfg1.pictureRc = 1;
        rcCfg1.mbRc = 1;
        rcCfg1.bitPerSecond = SENSOR_BITRATE_2 * 1024;
        rcCfg1.hrd = 0;
        rcCfg1.hrdCpbSize = 0;
        rcCfg1.gopLen = VIDEO_FRAME_RATE;
        rcCfg1.intraQpDelta = -3;
        rcCfg1.fixedIntraQp = 0;
        rcCfg1.mbQpAdjustment = 0;
        rcCfg1.longTermPicRate = VIDEO_FRAME_RATE;
        rcCfg1.mbQpAutoBoost = 1;

        //printf("Set rate control: qp %2d [%2d, %2d] %8d bps  "
        //       "pic %d mb %d skip %d  hrd %d\n"
        //       "  cpbSize %d gopLen %d intraQpDelta %2d "
        //       "fixedIntraQp %2d mbQpAdjustment %d "
        //       "mbQpAutoBoost %d\n",
        //       rcCfg.qpHdr, rcCfg.qpMin, rcCfg.qpMax, rcCfg.bitPerSecond,
        //       rcCfg.pictureRc, rcCfg.mbRc, rcCfg.pictureSkip, rcCfg.hrd,
        //       rcCfg.hrdCpbSize, rcCfg.gopLen, rcCfg.intraQpDelta,
        //       rcCfg.fixedIntraQp, rcCfg.mbQpAdjustment, rcCfg.mbQpAutoBoost);

        if((ret = H264EncSetRateCtrl(gptAVCEncoder[1]->encoder, &rcCfg1)) != H264ENC_OK)
        {
            printf("H264EncSetRateCtrl(1) failed. %x", ret);
            //CloseEncoder(encoder);
            //return -1;
        }
    }

    /* Encoder setup: coding control */
    if((ret = H264EncGetCodingCtrl(gptAVCEncoder[1]->encoder, &codingCfg1)) != H264ENC_OK)
    {
        printf("H264EncGetCodingCtrl(1) failed. %x", ret);
        //CloseEncoder(encoder);
        //return -1;
    }
    else
    {
#ifdef MULTI_SLICE
        codingCfg1.sliceSize = 5;
#else
        codingCfg1.sliceSize = 0;
#endif
        codingCfg1.constrainedIntraPrediction = 0;
        codingCfg1.disableDeblockingFilter = 0;
        codingCfg1.enableCabac = 1;
        codingCfg1.cabacInitIdc = 0;
        codingCfg1.transform8x8Mode = 1;
#ifdef CFG_IPTV_TX
        codingCfg1.quarterPixelMv = 0;
#else
        codingCfg1.quarterPixelMv = 1;
#endif
        codingCfg1.videoFullRange = 1;
        codingCfg1.seiMessages = 0;
        codingCfg1.idrHeader = 0;
        codingCfg1.gdrDuration = 0;
        codingCfg1.fieldOrder = 0;
        codingCfg1.cirStart = 0;
        codingCfg1.cirInterval = 0;
#ifdef LOW_LATENCY
        //Low Latency
        codingCfg1.inputLineBufDepth = 2;
        codingCfg1.inputLineBufHwModeEn = 1;
        codingCfg1.inputLineBufEn = 1;
        codingCfg1.inputLineBufLoopBackEn = 1;

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

        if((ret = H264EncSetCodingCtrl(gptAVCEncoder[1]->encoder, &codingCfg1)) != H264ENC_OK)
        {
            printf("H264EncSetCodingCtrl(1) failed. %x", ret);
            //CloseEncoder(encoder);
            //return -1;
        }
    }
   
    if ((ret = H264EncGetPreProcessing(gptAVCEncoder[1]->encoder, &preProcCfg1)) != H264ENC_OK)
    {
        printf("H264EncGetPreProcessing(1) failed. %x\n", ret);
    } else {

        preProcCfg1.inputType = H264ENC_YUV422_INTERLEAVED_YUYV;
        preProcCfg1.origWidth = cfg1.width;
        preProcCfg1.origHeight = cfg1.height;
        preProcCfg1.videoStabilization = 0;
        preProcCfg1.scaledOutput = 0;

        if ((ret = H264EncSetPreProcessing(gptAVCEncoder[1]->encoder, &preProcCfg1)) != H264ENC_OK)
        {
            printf("H264EncSetPreProcessing(1) failed. %x\n", ret);
        }
    }
#endif

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
    WriteBufferIndex = 0;

    memset(&gtAVCEncoderFlow, 0, sizeof(gtAVCEncoderFlow));
    _CaptureResultQ_Reset();
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
        gbStrBufFire[stramIndex][bufIndex]                  = MMP_FALSE;
        //gptAVCEncoder[stramIndex]->pStreamBufAdr[bufIndex]  = bufAdr;
        //printf("VideoEncoder_SetStreamBuf(%d) %d\n", stramIndex, bufIndex);
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
        return;
    
    pthread_mutex_lock(&JPEGEngineMutex);

    if (stramIndex == 0 && gbStream0Enable == MMP_FALSE)
    {
        gbStream0Enable             = MMP_TRUE;
        gStartgetVideo[stramIndex]  = MMP_TRUE;
        gVideoUserNum[stramIndex]   = 1;
    }
    else if (stramIndex == 0 && gbStream0Enable == MMP_TRUE)
    {
        pthread_mutex_unlock(&JPEGEngineMutex);
        return;
    }
    if (stramIndex == 1)
    {
        gbStream1Enable             = MMP_TRUE;
        gStartgetVideo[stramIndex]  = MMP_TRUE;
        gVideoUserNum[stramIndex]   = 1;
    }
    if (gbVideoEncoder)
    {
        pthread_mutex_unlock(&JPEGEngineMutex);
        return;
    }

    OpenEngine(H264_ENCODER);

    for (i = 0; i < VIDEO_STREAM_NUM; i++)
        for (j = 0; j < VIDEO_SAMPLE_NUM; j++)
            gbStrBufFire[i][j] = MMP_FALSE;

    gbEncodeFire = MMP_FALSE;

    for (i = 0; i < VIDEO_STREAM_NUM; i++)
        gVideoSelIdx[i] = gVideoWrIdx[i] = 0;

    gbVideoEncoder = MMP_TRUE;
    
    pthread_mutex_unlock(&JPEGEngineMutex);
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
VideoEncoder_Pause(
    MMP_UINT32 stramIndex)
{
    //if(bencoding == MMP_TRUE)
    //    usleep(1000);
    printf("VideoEncoder_Pause (%d)\n", stramIndex);
    if (stramIndex == 0)
    {
    	gbStream0Enable = MMP_FALSE;
    	gStartgetVideo[0] = MMP_FALSE;
        gVideoUserNum[0] = 0;
    }
    else {
    	gbStream1Enable = MMP_FALSE;
    	gStartgetVideo[1] = MMP_FALSE;
        gVideoUserNum[1] = 0;
    }
}

void
VideoEncoder_Resume(
    MMP_UINT32 stramIndex)
{
    MMP_UINT32 i;
    //if(bencoding == MMP_TRUE)
    //    usleep(1000);
    printf("VideoEncoder_Resume (%d)\n", stramIndex);
    if (stramIndex == 0)
    {    	
    	//gtAVCEncoderFlow.frameCount[0] = 1;
    	gVideoSelIdx[0] = gVideoWrIdx[0] = 0;
        gStartgetVideo[0] = MMP_TRUE;
        gVideoUserNum[0] = 1;
        
        for (i = 0; i < VIDEO_SAMPLE_NUM; i++)
            gbStrBufFire[0][i] = MMP_FALSE;
        
        gbStream0Enable = MMP_TRUE;
    }
    else {    	
    	//gtAVCEncoderFlow.frameCount[1] = 1;
    	gVideoSelIdx[1] = gVideoWrIdx[1] = 0;
        gStartgetVideo[1] = MMP_TRUE;
        gVideoUserNum[1] = 1;
        
        for (i = 0; i < VIDEO_SAMPLE_NUM; i++)
            gbStrBufFire[1][i] = MMP_FALSE;
               
        gbStream1Enable = MMP_TRUE;
    }
        
}

void
VideoEncoder_Close(
    void)
{
    MMP_UINT32 i;

    if (!gbVideoEncoderInit)
        return;
    CloseEngine(H264_ENCODER);

    memset(&gtAVCEncoderFlow, 0, sizeof(gtAVCEncoderFlow));

    for (i = 0; i < VIDEO_STREAM_NUM; i++)
        gptAVCEncoder[i]->streamBufSelect = 0;

    _CaptureResultQ_Reset();

    gfpCallback         = MMP_NULL;
    for (i = 0; i < VIDEO_STREAM_NUM; i++)
    {
        gStartgetVideo[i]   = MMP_FALSE;
        gVideoUserNum[i]    = 0;
    }
}

MMP_BOOL
VideoEncoder_GetSreamstate(
    MMP_UINT8 stream_id)
{
    return gStartgetVideo[stream_id];
}


void 
VideoEncoder_SetSreamUserNum(MMP_UINT8 stream_id, MMP_BOOL state) 
{ 
    //printf(">>>>>%s, %d, %d\n", __FUNCTION__, stream_id, state); 
    if(state) 
        gVideoUserNum[stream_id]++; 
    else 
        gVideoUserNum[stream_id]--; 
} 

MMP_UINT8
VideoEncoder_GetSreamUserNum(
    MMP_UINT8 stream_id)
{
    //printf(">>>>>%s, %d, %d\n", __FUNCTION__, stream_id, gVideoUserNum[stream_id]);
    return gVideoUserNum[stream_id];
}

void
VideoEncoder_SetWithPreview(
    MMP_BOOL preview)
{
	with_preview = preview;
}

void
VideoEncoder_SetEncodeIdx(
    MMP_UINT8 index)
{	
	CAPTURE_RESULT  tEntry = {0};
    MMP_UINT32      WBIndex         = WriteBufferIndex;
	
	if(!gOpenEnginCnt)
		return;
    
    //ithPrintf("[encoder]%s, %d\n", __FUNCTION__, index);
	WriteBufferIndex++;
    WriteBufferIndex %= gptAVCEncoder[0]->sourceBufCount;

	tEntry.resIdx = index;
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
        sem_post(&gpEncoderSem);
        gbJPEGEncoder = MMP_FALSE;
    }    
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
    
    pthread_mutex_lock(&JPEGEngineMutex);

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
    pthread_mutex_unlock(&JPEGEngineMutex);
    //ithPrintf("Encoding Time %d\n", PalGetDuration(jpgStart));
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
        //printf("#%d:%p:%d %d %d\n", i, strmPtr, slice->sliceSizes[i], slice->slicesReadyPrev, gtAVCEncoderFlow.encodedFrameCount);
        size = slice->sliceSizes[i];
        ithInvalidateDCacheRange(strmPtr, size);

        memcpy(gptAVCEncoder[gEncodeIdx]->pStreamBufAdr[gVideoWrIdx[gEncodeIdx]], strmPtr, size);

        streamInfo.streamIndex  = gEncodeIdx;
        streamInfo.bufIndex     = gVideoWrIdx[gEncodeIdx];
        if (slice->slicesReadyPrev == 0 && ((gtAVCEncoderFlow.encodedFrameCount % VIDEO_FRAME_RATE) == 1))
            streamInfo.bIFrame      = 1;
        else
            streamInfo.bIFrame      = 0;
        streamInfo.dataSize     = size;
        streamInfo.timeStamp    = gtAVCEncoderFlow.encodedTimeStamp;
        streamInfo.bLastSlice   = MMP_FALSE;
#ifdef CFG_IPTV_TX
        streamInfo.pData        = strmPtr;
        streamInfo.frameCount   = gtAVCEncoderFlow.encodedFrameCount;
        streamInfo.InstanceNum  = gEncodeIdx;

        streamInfo.frameRate    = gtAVCEncoderFlow.frameRate;
        streamInfo.EnFrameRate  = gtAVCEncoderFlow.frameRate;
        streamInfo.width        = 1920;//gptAVCEncoder[gEncodeIdx]->frameWidth;
        streamInfo.height       = 1080;//gptAVCEncoder[gEncodeIdx]->frameHeight;
        streamInfo.frameType    = (gptAVCEncoder[gEncodeIdx]->bIFrame) ? 0 : 1;
        streamInfo.bitRate      = 1000*1024;
        streamInfo.bSkipFrame   = gtAVCEncoderFlow.bSkipFrame;
#endif

        gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]]  = MMP_TRUE;

        gVideoSelIdx[gEncodeIdx]                            = (gVideoSelIdx[gEncodeIdx] + 1) % VIDEO_SAMPLE_NUM;

        gfpCallback(&streamInfo);
        gVideoWrIdx[gEncodeIdx] = (gVideoWrIdx[gEncodeIdx] + 1) % VIDEO_SAMPLE_NUM;

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
    MMP_UINT32          dataSize    = 1000 * 1024; // tmp solution
#else
    MMP_UINT32          dataSize    = 500 * 1024; // tmp solution
#endif
    VIDEO_STREAM_INFO   streamInfo;
    H264EncIn encIn;
    H264EncOut encOut;
    int ret;
    int i;
    uint32_t timeoutCnt;

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
             gLastSlicePtr = pData      = stream_buf_sys_addr;

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

             if ((gtAVCEncoderFlow.encodedFrameCount % VIDEO_FRAME_RATE) == 1)
                 encIn.codingType = H264ENC_INTRA_FRAME;
             else
                 encIn.codingType = H264ENC_PREDICTED_FRAME;

             encIn.busLuma = gptAVCEncoder[gEncodeIdx]->pSourceBufAdrY[gptAVCEncoder[gEncodeIdx]->sourceBufSelect];
             encIn.busChromaU = gptAVCEncoder[gEncodeIdx]->pSourceBufAdrU[gptAVCEncoder[gEncodeIdx]->sourceBufSelect];
             encIn.busChromaV = gptAVCEncoder[gEncodeIdx]->pSourceBufAdrV[gptAVCEncoder[gEncodeIdx]->sourceBufSelect];

             encIn.ipf = H264ENC_REFERENCE_AND_REFRESH;
             encIn.ltrf = H264ENC_REFERENCE;

             //printf("H264 start\n");
             ret = H264EncStrmEncode(gptAVCEncoder[gEncodeIdx]->encoder, &encIn, &encOut, &H264SliceReady, NULL, NULL);

             switch (ret)
             {
                  case H264ENC_FRAME_READY:

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
                              printf("Error Size(1) %d %d %d\n", i, encOut.pNaluSizeBuf[i], encOut.pNaluSizeBuf[i+1]);
                          }


                          ithInvalidateDCacheRange(gLastSlicePtr, encOut.pNaluSizeBuf[i]);
                          memcpy(gptAVCEncoder[gEncodeIdx]->pStreamBufAdr[gVideoWrIdx[gEncodeIdx]], gLastSlicePtr, encOut.pNaluSizeBuf[i]);
                          gLastSlicePtr+=encOut.pNaluSizeBuf[i];
                          //
                          streamInfo.streamIndex  = gEncodeIdx;
                          streamInfo.bufIndex     = gVideoWrIdx[gEncodeIdx];

                          if (i==0)
                              streamInfo.bIFrame      = (encIn.codingType == H264ENC_INTRA_FRAME) ? 1 : 0;
                          else
                              streamInfo.bIFrame      = 0;
                          streamInfo.dataSize     = encOut.pNaluSizeBuf[i];
                          streamInfo.timeStamp    = gtAVCEncoderFlow.encodedTimeStamp;
                          if (i==(encOut.numNalus-2))
                              streamInfo.bLastSlice   = MMP_TRUE;
                          else
                              streamInfo.bLastSlice   = MMP_FALSE;
#ifdef CFG_IPTV_TX
                          streamInfo.pData        = pData;
                          streamInfo.frameCount   = gtAVCEncoderFlow.encodedFrameCount;
                          streamInfo.InstanceNum  = gEncodeIdx;

                          streamInfo.frameRate    = gtAVCEncoderFlow.frameRate;
                          streamInfo.EnFrameRate  = gtAVCEncoderFlow.frameRate;
                          streamInfo.width        = 1920;//gptAVCEncoder[gEncodeIdx]->frameWidth;
                          streamInfo.height       = 1080;//gptAVCEncoder[gEncodeIdx]->frameHeight;
                          streamInfo.frameType    = (gptAVCEncoder[gEncodeIdx]->bIFrame) ? 0 : 1;
                          streamInfo.bitRate      = 1000*1024;
                          streamInfo.bSkipFrame   = gtAVCEncoderFlow.bSkipFrame;
#endif
                          ////printf("streamsize=%d(%d)<%x, %x, %x, %x, %x>\n", encOut.streamSize, streamInfo.bIFrame,
                          ////                              pData[0], pData[1], pData[2], pData[3], pData[4]);
                          //
                          gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]]  = MMP_TRUE;
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
#if defined(CFG_DUAL_STREAM)
    MMP_UINT32          dataSize    = 500 * 1024;
#else    
    MMP_UINT32          dataSize    = 500 * 1024;
#endif    

	VIDEO_STREAM_INFO   streamInfo;
    H264EncIn encIn;
    H264EncOut encOut;
    int ret;
    MMP_UINT32  accSize[2] = {0};

    gVideoEncoding[0] = MMP_TRUE;
#if defined(CFG_DUAL_STREAM)
    gVideoEncoding[1] = MMP_TRUE;
#endif    
    while ((gbStream0Enable || gbStream1Enable) && gbVideoEncoder)
    {    
         //bencoding = MMP_TRUE;
         if (QUEUE_NOT_EMPTY == gtAVCEncoderFlow.pfEnDequeue(&tEntry) && gbEncodeStart)
         {
         	 //if(gbStream0Enable)
	             gEncodeIdx = 0; //tEntry.resIdx;
			 //else
			 //	 gEncodeIdx = 1;
ENCODING_PROC :
             
             if (gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]])
             {
                 ithPrintf("Encoding Buf Full %d %d\n", gEncodeIdx, gVideoSelIdx[gEncodeIdx]);
                 continue;
             }

             if ((gEncodeIdx == 0 && gbStream0Enable) || (gEncodeIdx == 1 && gbStream1Enable))
             {
             gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]]  = MMP_TRUE;
                 //gVideoSelIdx[gEncodeIdx] = (gVideoSelIdx[gEncodeIdx] + 1) % VIDEO_SAMPLE_NUM;
             }

             gtAVCEncoderFlow.encodedFrameCount                  = tEntry.frameCount;
             gtAVCEncoderFlow.encodedTimeStamp                   = tEntry.timeStamp;

             gptAVCEncoder[gEncodeIdx]->sourceBufSelect          = tEntry.frameIndex;
             //gbEncodeFire                                        = MMP_TRUE;
#if 1
             if (((gtAVCEncoderFlow.frameCount[0] == 1 && gbStream0Enable) || (gtAVCEncoderFlow.frameCount[1] == 1 && gbStream1Enable)) &&
             	 gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]])
             {
             	 H264EncRateCtrl rcCfg;
             	 if((ret = H264EncGetRateCtrl(gptAVCEncoder[gEncodeIdx]->encoder, &rcCfg)) != H264ENC_OK)
                 {
                     printf("H264EncGetRateCtrl(%d) reset failed. %x", gEncodeIdx, ret);
                 }
                 else
                 {
                     if((ret = H264EncSetRateCtrl(gptAVCEncoder[gEncodeIdx]->encoder, &rcCfg)) != H264ENC_OK)
                     {
                         printf("H264EncSetRateCtrl(%d) reset failed. %x", gEncodeIdx, ret);
                     }
                 }                 
             }
#endif             
             if ((gEncodeIdx == 0 && gbStream0Enable) || (gEncodeIdx == 1 && gbStream1Enable))
                 pData = gptAVCEncoder[gEncodeIdx]->pStreamBufAdr[gVideoWrIdx[gEncodeIdx]];      
             else
                 pData = stream_buf_sys_addr;                 

             memset (&encIn, 0, sizeof(H264EncIn));
             encIn.pOutBuf = pData;
             encIn.busOutBuf = pData;
             encIn.outBufSize = dataSize;
           
#if defined(CFG_CPU_WB) && (!defined(WIN32))
             ithInvalidateDCacheRange(pData, dataSize);
#endif

             if (gtAVCEncoderFlow.frameCount[gEncodeIdx] == 1)
                 encIn.timeIncrement = 0;
             else
                 encIn.timeIncrement = 1;

             if ((gtAVCEncoderFlow.frameCount[gEncodeIdx] % VIDEO_FRAME_RATE) == 1)
                 encIn.codingType = H264ENC_INTRA_FRAME;
             else
                 encIn.codingType = H264ENC_PREDICTED_FRAME;

             if (encIn.codingType == H264ENC_INTRA_FRAME)
                 accSize[gEncodeIdx] = 0;
                 
             encIn.busLuma = gptAVCEncoder[gEncodeIdx]->pSourceBufAdrY[gptAVCEncoder[gEncodeIdx]->sourceBufSelect];
             encIn.busChromaU = gptAVCEncoder[gEncodeIdx]->pSourceBufAdrU[gptAVCEncoder[gEncodeIdx]->sourceBufSelect];
             encIn.busChromaV = gptAVCEncoder[gEncodeIdx]->pSourceBufAdrV[gptAVCEncoder[gEncodeIdx]->sourceBufSelect];

             encIn.ipf = H264ENC_REFERENCE_AND_REFRESH;
             encIn.ltrf = H264ENC_REFERENCE;
             
             pthread_mutex_lock(&h264_mutex);
             ret = H264EncStrmEncode(gptAVCEncoder[gEncodeIdx]->encoder, &encIn, &encOut, NULL, NULL, NULL);
             pthread_mutex_unlock(&h264_mutex);
             gtAVCEncoderFlow.frameCount[gEncodeIdx]++;
#if defined(CFG_DUAL_STREAM)    
             gptAVCEncoder[1]->pSourceBufAdrY[0] = gptAVCEncoder[1]->pSourceBufAdrY[1] = gptAVCEncoder[1]->pSourceBufAdrY[2] = encOut.busScaledLuma;
             gptAVCEncoder[1]->pSourceBufAdrU[0] = gptAVCEncoder[1]->pSourceBufAdrU[1] = gptAVCEncoder[1]->pSourceBufAdrU[2] = encOut.busScaledLuma;
             gptAVCEncoder[1]->pSourceBufAdrV[0] = gptAVCEncoder[1]->pSourceBufAdrV[1] = gptAVCEncoder[1]->pSourceBufAdrV[2] = encOut.busScaledLuma;             
#endif             
             switch (ret)
             {
                  case H264ENC_FRAME_READY:

                      if (((gbStream0Enable && gEncodeIdx == 0) || (gbStream1Enable && gEncodeIdx == 1)) &&
                      	  gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]])
                      {
                      //temp solution for network overflow
                      //if (encOut.streamSize == 0 || accSize[gEncodeIdx] > SENSOR_BITRATE * 170)
                      //{
                      //	  printf("Skip Frame %d %d\n", encOut.streamSize, accSize[gEncodeIdx]);                      	  
                      //        //gVideoSelIdx[gEncodeIdx]                            = (gVideoSelIdx[gEncodeIdx] + VIDEO_SAMPLE_NUM - 1) % VIDEO_SAMPLE_NUM;
                      //    gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]]  = MMP_FALSE;
                      //	  break;
                      //}
                      
                      ithInvalidateDCacheRange(pData, encOut.streamSize);

                      streamInfo.streamIndex  = gEncodeIdx;
                      streamInfo.bufIndex     = gVideoWrIdx[gEncodeIdx];
                      streamInfo.bIFrame      = (encIn.codingType == H264ENC_INTRA_FRAME) ? 1 : 0;
                      streamInfo.dataSize     = encOut.streamSize;
                      streamInfo.timeStamp    = gtAVCEncoderFlow.encodedTimeStamp;
                      streamInfo.bLastSlice   = MMP_TRUE;
          
                      accSize[gEncodeIdx] += encOut.streamSize;
                      //printf("str(%d) streamsize=%d(%d)<%x, %x, %x, %x, %x> timeStamp:%d\n", gEncodeIdx, encOut.streamSize, streamInfo.bIFrame,
                      //						pData[0], pData[1], pData[2], pData[3], pData[4],streamInfo.timeStamp);
                      
                      gfpCallback(&streamInfo);

                      gVideoSelIdx[gEncodeIdx] = (gVideoSelIdx[gEncodeIdx] + 1) % VIDEO_SAMPLE_NUM;
                      gVideoWrIdx[gEncodeIdx] = (gVideoWrIdx[gEncodeIdx] + 1) % VIDEO_SAMPLE_NUM;

					  }else
					  {
                      gbStrBufFire[gEncodeIdx][gVideoSelIdx[gEncodeIdx]]  = MMP_FALSE; 

                      }

                      break;

                  case H264ENC_SYSTEM_ERROR:
                      printf("H264ENC_SYSTEM_ERROR(%d)\n", gEncodeIdx);
                      break;

                  case H264ENC_OUTPUT_BUFFER_OVERFLOW:
                      printf("H264ENC_OUTPUT_BUFFER_OVERFLOW(%d)\n", gEncodeIdx);
                      break;

                  default:
                      printf("H264EncStrmEncode(%d) failed. %x", gEncodeIdx, ret);
                      break;
             }
             
#if defined(CFG_DUAL_STREAM)                          
             if (gEncodeIdx == 0 && gbStream1Enable)
             {
                 gEncodeIdx = 1;
                 dataSize = 200 * 1024;
                 goto ENCODING_PROC;
             }
#endif
         }
         //bencoding = MMP_FALSE;
         usleep(1000);
    }
    gVideoEncoding[0] = MMP_FALSE;
#if defined(CFG_DUAL_STREAM)
    gVideoEncoding[1] = MMP_FALSE;
#endif    
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

ISP_BUFFER_INFO* VideoEncoder_GetISPBUFFERINFO()
{
    return gISPBUUFERINFO;
}
