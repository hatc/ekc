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
#include <stdio.h>

#include "tiff_stuff.h"

#include <trace.h>

/* libtiff error/warning messages handling */

inline void libtiff_trace(CPCL_TRACE_LEVEL trace_level, char const *format, va_list args) {
	size_t const header_size = arraysize("libtiff:: ") - 1;
	char buf[0x200] = "libtiff:: ";
	int n = _vsnprintf_s(buf + header_size, arraysize(buf) - header_size, _TRUNCATE, format, args);
	if (n < 0) {
		n = arraysize(buf) - 1;
		if ('\0' != buf[n]) {
			buf[n] = '\0';
			--n;
		}
	} else
		n += header_size;

	cpcl::TraceString(trace_level, cpcl::StringPiece(buf, (size_t)n));
}
////TIFFSetErrorHandler(TIFFErrorHandler handler);
static void libtiff_error(char const* /*module*/, char const *format, va_list args) {
	if (cpcl::TRACE_LEVEL & CPCL_TRACE_LEVEL_ERROR)
		libtiff_trace(CPCL_TRACE_LEVEL_ERROR, format, args);
}
////TIFFSetWarningHandler(TIFFErrorHandler);
static void libtiff_warning(char const* /*module*/, char const *format, va_list args) {
	/*if (cpcl::TRACE_LEVEL & CPCL_TRACE_LEVEL_WARNING)
		libtiff_trace(CPCL_TRACE_LEVEL_WARNING, format, args);*/
	if (cpcl::TRACE_LEVEL & CPCL_TRACE_LEVEL_DEBUG)
		libtiff_trace(CPCL_TRACE_LEVEL_DEBUG, format, args);
}
//typedef void (*TIFFErrorHandlerExt)(thandle_t, const char*, const char*, va_list);
//TIFFSetErrorHandlerExt(TIFFErrorHandlerExt handler) - TIFFErrorExt(tif->tif_clientdata, where, "Integer overflow in %s", where);
//i.e. TIFFErrorHandlerExt support object specific error handler

extern "C" {

TIFFErrorHandler _TIFFerrorHandler = &libtiff_error;
TIFFErrorHandler _TIFFwarningHandler = &libtiff_warning;

}

/* libtiff I/O handling */

static tmsize_t libtiff_read(thandle_t handle, void *data, tmsize_t size) {
	COMPILE_ASSERT(sizeof(tmsize_t) == sizeof(unsigned long), tmsize_is_long);
	cpcl::IOStream *stream = (cpcl::IOStream*)handle;
	return static_cast<tmsize_t>(stream->Read(data, static_cast<unsigned long>(size)));
}

static tmsize_t libtiff_write(thandle_t, void*, tmsize_t) {
	return 0;
	// DUMBASS_CHECK(false);
}

static toff_t libtiff_seek(thandle_t handle, toff_t off, int whence) {
	cpcl::IOStream *stream = (cpcl::IOStream*)handle;
	__int64 r;
	if (!stream->Seek(static_cast<__int64>(off), static_cast<unsigned long>(whence), &r))
		r = -1;
	return static_cast<toff_t>(r);
}

static int libtiff_close(thandle_t) {
	return 0;
}

static toff_t libtiff_size(thandle_t handle) {
	cpcl::IOStream *stream = (cpcl::IOStream*)handle;
	return static_cast<toff_t>(stream->Size());
}

/* libtiff handler */

TiffStuff::TiffStuff(cpcl::IOStream *stream)
	: tif(NULL), ycbcr(NULL), cielab(NULL), bitdepth16_8(NULL), ualpha_aalpha(NULL), palettecolormap(0), bwmap(0) {
	tif = TIFFClientOpen("", "r", (void *)stream,
		libtiff_read, libtiff_write, libtiff_seek, libtiff_close, libtiff_size,
		NULL, NULL);
}

bool TiffStuff::operator!() const {
	return (NULL == tif);
}

/* NB: the uint32 casts are to silence certain ANSI-C compilers */
#ifndef TIFFhowmany_32
#define TIFFhowmany_32(x, y) (((uint32)x < (0xffffffff - (uint32)(y-1))) ? \
			   ((((uint32)(x))+(((uint32)(y))-1))/((uint32)(y))) : \
			   0U)
#endif
#ifndef TIFFroundup_32
#define TIFFroundup_32(x, y) (TIFFhowmany_32(x,y)*(y))
#endif
TIFFYCbCrToRGB* TiffStuff::GetYCbCrToRGBConversion() {
	if (!tif)
		throw tiff_exception("TiffStuff::GetYCbCrToRGBConversion(): libtiff handler not initialized");
	if (!ycbcr) {
		ycbcr = (TIFFYCbCrToRGB*)malloc(TIFFroundup_32(sizeof(TIFFYCbCrToRGB), sizeof(long))
			+ 4*256*sizeof(TIFFRGBValue)
			+ 2*256*sizeof(int)
			+ 3*256*sizeof(int32));
		if (!ycbcr)
			throw std::bad_alloc();
	}

	float *luma, *ref_black_white;
	TIFFGetFieldDefaulted(tif, TIFFTAG_YCBCRCOEFFICIENTS, &luma);
	TIFFGetFieldDefaulted(tif, TIFFTAG_REFERENCEBLACKWHITE, &ref_black_white);
	TIFFYCbCrToRGBInit(ycbcr, luma, ref_black_white);

	return ycbcr;
}

static TIFFDisplay const display_sRGB = {
	{   /* XYZ -> luminance matrix */
		{  3.2410F, -1.5374F, -0.4986F },
		{ -0.9692F,  1.8760F,  0.0416F },
		{  0.0556F, -0.2040F,  1.0570F }
	},
	100.0F, 100.0F, 100.0F, /* Light o/p for reference white */
	255, 255, 255,          /* Pixel values for ref. white */
	1.0F, 1.0F, 1.0F,       /* Residual light o/p for black pixel */
	2.4F, 2.4F, 2.4F,       /* Gamma values for the three guns */
};
TIFFCIELabToRGB* TiffStuff::GetCIELabToRGBConversion() {
	if (!tif)
		throw tiff_exception("TiffStuff::GetCIELabToRGBConversion(): libtiff handler not initialized");
	if (!cielab) {
		cielab = (TIFFCIELabToRGB*)malloc(sizeof(TIFFCIELabToRGB));
		if (!cielab)
			throw std::bad_alloc();
	}

	float *white_point;
	TIFFGetFieldDefaulted(tif, TIFFTAG_WHITEPOINT, &white_point);

	float ref_white[3];
	ref_white[1] = 100.0F;
	ref_white[0] = white_point[0] / white_point[1] * ref_white[1];
	ref_white[2] = (1.0F - white_point[0] - white_point[1]) / white_point[1] * ref_white[1];
	TIFFCIELabToRGBInit(cielab, &display_sRGB, ref_white);

	return cielab;
}

uint8* TiffStuff::GetBitDepth16To8Map() {
	size_t const map_size = 1 << 16;
	if (!bitdepth16_8) {
		bitdepth16_8 = (uint8*)malloc(map_size * sizeof(uint8));
		if (!bitdepth16_8)
			throw std::bad_alloc();
	}

	for (size_t i = 0; i < map_size; ++i)
		bitdepth16_8[i] = static_cast<uint8>((i + 128) / 257);
	
	return bitdepth16_8;
}

uint8* TiffStuff::GetUAlphaToAAlphaMap() {
	size_t const map_size = 1 << 16; // ???
	if (!ualpha_aalpha) {
		ualpha_aalpha = (uint8*)malloc(map_size * sizeof(uint8));
		if (!ualpha_aalpha)
			throw std::bad_alloc();
	}

	uint8 *i = ualpha_aalpha;
	for (size_t na = 0; na < 256; ++na) {
		for (size_t nv = 0; nv < 256; ++nv) {
			*i++ = (nv * na + 127) / 255;
		}
	}

	return ualpha_aalpha;
}

TiffStuff::~TiffStuff() {
	if (tif)
		TIFFClose(tif);

	if (ycbcr)
		free(ycbcr);
	if (cielab)
		free(cielab);
	if (bitdepth16_8)
		free(bitdepth16_8);
	if (ualpha_aalpha)
		free(ualpha_aalpha);

	if (palettecolormap)
		free(palettecolormap);
	if (bwmap)
		free(bwmap);
}

/* libtiff auxiliary functions */

extern "C" {

void* _TIFFmalloc(tmsize_t s) {
	return malloc((size_t)s);
}

void _TIFFfree(void *p) {
	free(p);
}

void* _TIFFrealloc(void *p, tmsize_t s) {
	return realloc(p, (size_t)s);
}

void _TIFFmemset(void* p, int v, tmsize_t c) {
	memset(p, v, (size_t)c);
}

void _TIFFmemcpy(void *d, void const *s, tmsize_t c) {
	memcpy(d, s, (size_t)c);
}

int _TIFFmemcmp(void const *p1, void const *p2, tmsize_t c) {
	return memcmp(p1, p2, (size_t)c);
}

}
