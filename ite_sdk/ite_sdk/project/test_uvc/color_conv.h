#include <stdint.h>

typedef struct _BASE_RECT_TAG
{
    int x;
    int y;
    int w;
    int h;
} BASE_RECT;

void
_ispColorTrans(
    uint8_t         *srcAddr_rgby,
    uint8_t         *srcAddr_u,
    uint8_t         *srcAddr_v,
    DATA_COLOR_TYPE colorType,
    BASE_RECT       *videoWnd,
    BASE_RECT       *srcRect,
    BASE_RECT       *realRect);

void
color_conv_to_yuv420(
    uint8_t         *srcAddr_rgby,
    uint8_t         *srcAddr_u,
    uint8_t         *srcAddr_v,
    BASE_RECT       *srcRect,
    DATA_COLOR_TYPE src_color_type,
    RawImage        *dstImg);