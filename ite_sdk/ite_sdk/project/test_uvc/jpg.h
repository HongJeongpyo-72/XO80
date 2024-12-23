#ifndef _JPG_H__
#define _JPG_H__

#include <stdint.h>

#ifdef __cplusplus
extern C {
#endif

typedef enum DATA_COLOR_TYPE_TAG
{
    DATA_COLOR_YUV444,
    DATA_COLOR_YUV422,
    DATA_COLOR_YUV422R,
    DATA_COLOR_YUV420,
    DATA_COLOR_YUYV,
    DATA_COLOR_ARGB8888,
    DATA_COLOR_ARGB4444,
    DATA_COLOR_RGB565,
    DATA_COLOR_NV12,
    DATA_COLOR_NV21,

    DATA_COLOR_CNT,
} DATA_COLOR_TYPE;

typedef struct JpegInputPkt {
    uint8_t  *pInputBuffer;
    uint32_t bufferSize;
} JpegInputPkt;

typedef struct JpegOutputPkt {
    uint8_t         *pYBuffer;
    uint8_t         *pUBuffer;
    uint8_t         *pVBuffer;
    uint32_t        ySize;
    uint32_t        uvSize;
    uint32_t        yPitch;
    uint32_t        uvPitch;
    DATA_COLOR_TYPE colorType;
    uint32_t        width;
    uint32_t        height;
} JpegOutputPkt, RawImage;

int32_t
jpg_decode(
    uint8_t *jpegStream,
    uint32_t  streamLength,
    JpegOutputPkt *ptOutputPkt);

void
jpg_encode(
    RawImage *src_img,
    uint8_t  *out_stream,
    uint32_t *out_stream_size);

#ifdef __cplusplus
}
#endif

#endif