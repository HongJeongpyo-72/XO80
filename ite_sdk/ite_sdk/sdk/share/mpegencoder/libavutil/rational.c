/*
 * rational numbers
 * Copyright (c) 2003 Michael Niedermayer <michaelni@gmx.at>
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

/**
 * @file
 * rational numbers
 * @author Michael Niedermayer <michaelni@gmx.at>
 */

//#include "avassert.h"
#include <math.h>
//#include <limits.h>

#include "common.h"
#include "mathematics.h"
#include "rational.h"

int av_reduce_ex(int *dst_num, int *dst_den,
              int64_t num, int64_t den, int64_t max)
{
    AVRational a0 = { 0, 1 }, a1 = { 1, 0 };
    int sign = (num < 0) ^ (den < 0);
    int64_t gcd = av_gcd_ex(FFABS(num), FFABS(den));

    if (gcd) {
        num = FFABS(num) / gcd;
        den = FFABS(den) / gcd;
    }
    if (num <= max && den <= max) {
#ifndef WIN32
        a1 = (AVRational) { num, den };
#else
        a1.num = num;
        a1.den = den;
#endif
        den = 0;
    }

    while (den) {
        uint64_t x        = num / den;
        int64_t next_den  = num - den * x;
        int64_t a2n       = x * a1.num + a0.num;
        int64_t a2d       = x * a1.den + a0.den;

        if (a2n > max || a2d > max) {
            if (a1.num) x =          (max - a0.num) / a1.num;
            if (a1.den) x = FFMIN(x, (max - a0.den) / a1.den);

            if (den * (2 * x * a1.den + a0.den) > num * a1.den)
            {
#ifndef WIN32
                a1 = (AVRational) { x * a1.num + a0.num, x * a1.den + a0.den };
#else
                a1.num = x * a1.num + a0.num;
                a1.den = x * a1.den + a0.den;
#endif
            }
            break;
        }

        a0  = a1;
#ifndef WIN32
        a1  = (AVRational) { a2n, a2d };
#else
        a1.num = a2n;
        a1.den = a2d;
#endif
        num = den;
        den = next_den;
    }
    av_assert2(av_gcd_ex(a1.num, a1.den) <= 1U);

    *dst_num = sign ? -a1.num : a1.num;
    *dst_den = a1.den;

    return den == 0;
}

AVRational av_d2q_ex(double d, int max)
{
    AVRational a;
#define LOG2  0.69314718055994530941723212145817656807550013436025
    int exponent;
    int64_t den;
    if (isnan(d))
    {
#ifndef WIN32
        return (AVRational) { 0,0 };
#else        
        a.num = 0;
        a.den = 0;
        return a;
#endif
    }
//    if (isinf(d))
//    {
//#ifndef WIN32
//        return (AVRational) { d < 0 ? -1 : 1, 0 };
//#else    
//        a.num = d < 0 ? -1 : 1;
//        a.den = 0;
//        return a;
//#endif
//    }
    exponent = FFMAX( (int)(log(fabs(d) + 1e-20)/LOG2), 0);
    den = 1LL << (61 - exponent);
    av_reduce_ex(&a.num, &a.den, (int64_t)(d * den + 0.5), den, max);

    return a;
}
