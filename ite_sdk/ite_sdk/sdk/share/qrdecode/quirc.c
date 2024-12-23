/* quirc -- QR-code recognition library
 * Copyright (C) 2010-2012 Daniel Beer <dlbeer@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include "quirc_internal.h"

const char *quirc_version(void)
{
	return "1.0";
}

struct quirc *quirc_new(void)
{
	struct quirc *q = malloc(sizeof(*q));

	if (!q)
		return NULL;

	memset(q, 0, sizeof(*q));
	return q;
}

void quirc_destroy(struct quirc *q)
{
	free(q->image);
	/* q->pixels may alias q->image when their type representation is of the
	   same size, so we need to be careful here to avoid a double free */
	if (!QUIRC_PIXEL_ALIAS_IMAGE)
		free(q->pixels);

	free(q);
}

int quirc_resize(struct quirc *q, int w, int h)
{
	uint8_t		*image  = NULL;
	quirc_pixel_t	*pixels = NULL;
	int olddim = 0;
	int newdim = 0;
	int min = 0;

	/*
	 * XXX: w and h should be size_t (or at least unsigned) as negatives
	 * values would not make much sense. The downside is that it would break
	 * both the API and ABI. Thus, at the moment, let's just do a sanity
	 * check.
	 */
	if (w < 0 || h < 0)
		goto fail;

	/*
	 * alloc a new buffer for q->image. We avoid realloc(3) because we want
	 * on failure to be leave `q` in a consistant, unmodified state.
	 */
	image = calloc(w, h);
	if (!image)
		goto fail;

	/* compute the "old" (i.e. currently allocated) and the "new"
	   (i.e. requested) image dimensions */
	olddim = q->w * q->h;
	newdim = w * h;
	min = (olddim < newdim ? olddim : newdim);

	/*
	 * copy the data into the new buffer, avoiding (a) to read beyond the
	 * old buffer when the new size is greater and (b) to write beyond the
	 * new buffer when the new size is smaller, hence the min computation.
	 */
	(void)memcpy(image, q->image, min);

	/* alloc a new buffer for q->pixels if needed */
	if (!QUIRC_PIXEL_ALIAS_IMAGE) {
		pixels = calloc(newdim, sizeof(quirc_pixel_t));
		if (!pixels)
			goto fail;
	}

	/* alloc succeeded, update `q` with the new size and buffers */
	q->w = w;
	q->h = h;
	free(q->image);
	q->image = image;
	if (!QUIRC_PIXEL_ALIAS_IMAGE) {
		free(q->pixels);
		q->pixels = pixels;
	}

	return 0;
	/* NOTREACHED */
fail:
	free(image);
	free(pixels);

	return -1;
}

int quirc_count(const struct quirc *q)
{
	return q->num_grids;
}

static const char *const error_table[] = {
	"Success",
	"Invalid grid size",
	"Invalid version",
	"Format data ECC failure",
	"ECC failure",
	"Unknown data type",
	"Data overflow",
	"Data underflow"
};

const char *quirc_strerror(quirc_decode_error_t err)
{
	if (err >= 0 && err < sizeof(error_table) / sizeof(error_table[0]))
		return error_table[err];

	return "Unknown error";
}
