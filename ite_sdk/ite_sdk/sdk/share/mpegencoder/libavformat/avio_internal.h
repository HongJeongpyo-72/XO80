/*
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVFORMAT_AVIO_INTERNAL_H
#define AVFORMAT_AVIO_INTERNAL_H

#include "avio.h"
//#include "url.h"

int ffio_init_context_ex(
    AVIOContext *s,
    unsigned char *buffer,
    int buffer_size,
    void *opaque,
    int (*write_packet)(void *opaque, uint8_t *buf, int buf_size),
    int64_t (*seek)(void *opaque, int64_t offset, int whence),
    uint64_t (*write_size)(void *opaque));

static av_always_inline void ffio_wfourcc(AVIOContext *pb, const uint8_t *s)
{
    avio_wl32_ex(pb, MKTAG(s[0], s[1], s[2], s[3]));
}

#endif /* AVFORMAT_AVIO_INTERNAL_H */
