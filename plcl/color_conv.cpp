// Copyright (C) 2012 Yuri Agafonov
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#include "stdafx.h"

#include "color_conv.h"
#include "color_conv_gray8.hpp"

#include "rendering_data.h"
#include "plcl_exception.hpp"

template<class T>
unsigned char* convert(unsigned char *dst, unsigned char *src, unsigned int width) {
	if ((!dst) || (!src))
		throw plcl_exception("ColorConv::convert() dst || src is NULL");
	T()(dst, src, width);
	return dst;
}
struct color_conv_no {};
template<>
unsigned char* convert<color_conv_no>(unsigned char*, unsigned char *src, unsigned int) {
	return src;
}

template<size_t Channels>
size_t storage_needed(unsigned int width) {
	return Channels * width;
}

namespace plcl {

static unsigned char* color_conv_bgr24_bgra32(unsigned char *dst, unsigned char *src, unsigned int width) {
	do {
		// color_conv - bgr/rgb as 4-byte int - not safe... - memory can be aligned to 4-bytes - for example 24 bytes - 8 pixels, 
		// but edge pixel access one extra byte - memory access violation
		// HOWEVER read one extra byte memory not seem as crucial error, and porbaly most of system just ignore them...
		/*
		for (unsigned int x = 0; x < width - 1; ++x, scanline += 3) {
		 *(unsigned int*)scanline = pixel;
		}
		
		scanline[1] = g;
		if (pixfmt == PLCL_PIXEL_FORMAT_BGR_24) 
		 scanline[0] = b, scanline[2] = r;
		else
		 scanline[0] = r, scanline[2] = b;
		*/
		*(unsigned int*)dst = (*(unsigned int*)src) | ((unsigned int)0xFF << 24);

		src += 3;
		dst += 4;
	} while (--width);
	return dst;
}
static unsigned char* color_conv_bgr24_abgr32(unsigned char *dst, unsigned char *src, unsigned int width) {
	do {
		// color_conv - bgr/rgb as 4-byte int - not safe... - memory can be aligned to 4-bytes - for example 24 bytes - 8 pixels, 
		// but edge pixel access one extra byte - memory access violation
		// HOWEVER read one extra byte memory not seem as crucial error, and porbaly most of system just ignore them...
		*(unsigned int*)dst = ((*(unsigned int*)src) << 8) | ((unsigned int)0xFF);

		src += 3;
		dst += 4;
	} while (--width);
	return dst;
}
static unsigned char* color_conv_bgr24_rgb24(unsigned char *dst, unsigned char *src, unsigned int width) {
	do {
		unsigned char v = src[0];
		dst[0] = src[2];
		dst[1] = src[1];
		dst[2] = v;

		src += 3;
		dst += 3;
	} while (--width);
	return dst;
}
static unsigned char* color_conv_bgra32_rgba32(unsigned char *dst, unsigned char *src, unsigned int width) {
	do {
		*(unsigned int*)dst = *(unsigned int*)src;
		unsigned int v = *(unsigned int*)src;
		dst[0] = (unsigned char)(v >> 16);
		dst[2] = (unsigned char)(v & (unsigned int)0xFF);

		src += 4;
		dst += 4;
	} while (--width);
	return dst;
}
static unsigned char* color_conv_gray8_bgra32(unsigned char *dst, unsigned char *src, unsigned int width) {
	do {
		*(unsigned int*)dst = (unsigned int)*src | ((unsigned int)*src << 8) | ((unsigned int)*src << 16) | ((unsigned int)0xFF << 24);
		/*i.e.
		unsigned int v = *src;
		*(unsigned int*)dst = v | (v << 8) | (v << 16) | ((unsigned int)0xFF << 24);*/

		src += 1;
		dst += 4;
	} while (--width);
	return dst;
}
static unsigned char* color_conv_bgra32_abgr32(unsigned char *dst, unsigned char *src, unsigned int width) {
	do {
		unsigned int a = (*(unsigned int*)src >> 24) & (unsigned int)0xFF;
		*(unsigned int*)dst = (*(unsigned int*)src << 8) | a;

		src += 4;
		dst += 4;
	} while (--width);
	return dst;
}
static unsigned char* color_conv_abgr32_bgra32(unsigned char *dst, unsigned char *src, unsigned int width) {
	do {
		unsigned int a = *(unsigned int*)src & (unsigned int)0xFF;
		*(unsigned int*)dst = (*(unsigned int*)src >> 8) | (a << 24);

		src += 4;
		dst += 4;
	} while (--width);
	return dst;
}

static ColorConv converters[] = {
	{ convert<color_conv_no>, storage_needed<0> },
	{ convert<agg::color_conv_gray8_to_bgr24>, storage_needed<3> },
	{ convert<agg::color_conv_gray8_to_rgb24>, storage_needed<3> },
	{ convert<agg::color_conv_bgra32_to_bgr24>, storage_needed<3> },
	// { convert<agg::color_conv_bgr24_to_bgra32>, storage_needed<4> }
	{ color_conv_bgr24_bgra32, storage_needed<4> },
	{ color_conv_bgr24_rgb24, storage_needed<3> },
	{ color_conv_bgra32_rgba32, storage_needed<4> },
	{ color_conv_gray8_bgra32, storage_needed<4> },
	{ color_conv_bgr24_abgr32, storage_needed<4> },
	{ color_conv_bgra32_abgr32, storage_needed<4> },
	{ color_conv_abgr32_bgra32, storage_needed<4> }
};
ColorConv GetConverter(unsigned int from, unsigned int to) {
	if (to == from)
		return converters[0];
	else if ((PLCL_PIXEL_FORMAT_GRAY_8 == from) && (PLCL_PIXEL_FORMAT_BGR_24 == to))
		return converters[1];
	else if ((PLCL_PIXEL_FORMAT_GRAY_8 == from) && (PLCL_PIXEL_FORMAT_RGB_24 == to))
		return converters[2];
	else if ((PLCL_PIXEL_FORMAT_BGRA_32 == from) && (PLCL_PIXEL_FORMAT_BGR_24 == to))
		return converters[3];
	else if ((PLCL_PIXEL_FORMAT_BGR_24 == from) && (PLCL_PIXEL_FORMAT_BGRA_32 == to))
		return converters[4];
	else if ((PLCL_PIXEL_FORMAT_BGR_24 == from) && (PLCL_PIXEL_FORMAT_RGB_24 == to))
		return converters[5];
	else if ((PLCL_PIXEL_FORMAT_RGB_24 == from) && (PLCL_PIXEL_FORMAT_BGR_24 == to))
		return converters[5];
	else if ((PLCL_PIXEL_FORMAT_BGRA_32 == from) && (PLCL_PIXEL_FORMAT_RGBA_32 == to))
		return converters[6];
	else if ((PLCL_PIXEL_FORMAT_RGBA_32 == from) && (PLCL_PIXEL_FORMAT_BGRA_32 == to))
		return converters[6];
	else if ((PLCL_PIXEL_FORMAT_GRAY_8 == from) && (PLCL_PIXEL_FORMAT_BGRA_32 == to))
		return converters[7];
	else if ((PLCL_PIXEL_FORMAT_GRAY_8 == from) && (PLCL_PIXEL_FORMAT_RGBA_32 == to))
		return converters[7];
	else if ((PLCL_PIXEL_FORMAT_BGR_24 == from) && (PLCL_PIXEL_FORMAT_ABGR_32 == to))
		return converters[8];
	else if ((PLCL_PIXEL_FORMAT_BGRA_32 == from) && (PLCL_PIXEL_FORMAT_ABGR_32 == to))
		return converters[9];
	else if ((PLCL_PIXEL_FORMAT_ABGR_32 == from) && (PLCL_PIXEL_FORMAT_BGRA_32 == to))
		return converters[10];

	plcl_exception::throw_formatted(plcl_exception(), "GetConverter(): conversion from(%08X) to(%08X) unavailable", from, to);
	return converters[0];
}

} // namespace plcl
