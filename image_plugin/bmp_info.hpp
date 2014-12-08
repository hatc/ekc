// bmp_info.hpp
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
#pragma once

#include <trace.h>
#include <plcl_exception.hpp>

#define WIN32_LEAN_AND_MEAN

#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NOUSER
#define NONLS
#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX

#define NOCRYPT
#define NOIME
#include <windows.h>

namespace bmp {

//
// check BITMAPFILEHEADER fields:
//  bfType, bfOffBits
//
// check BITMAPINFOHEADER fields:
//  biSize(default,v4,v5), biPlanes, biWidth, biHeight, biCompression, biBitCount
//  determine actual number of bmiColors entries == (file_info.bfOffBits - (sizeof(BITMAPFILEHEADER) + dib_info.biSize)) / sizeof(RGBQUAD)
//  if pixel >= bmiColorsN Error("color index out of range") use white/black default
//
// biSizeImage - not used(may be set to zero for BI_RGB bitmaps), so not needed to check

template<class BitmapInfoHeader>
inline void ReadBMPInfo(BitmapInfoHeader const &info, bool *flip_y,
											 unsigned int *width, unsigned int *height, unsigned int *bpp,
											 DWORD *compression, RGBQUAD (&colors)[256], size_t *n_colors_) {
	if (info.biPlanes != 1) {
		/*plcl_exception::throw_formatted(plcl_exception(),
			"ReadInfo(): invalid header biPlanes: %d",
			(int)info.biPlanes);*/
		cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
			"BmpPlugin::ReadInfo(): invalid header biPlanes: %d",
			(int)info.biPlanes);
	}
	if (info.biWidth < 1 || abs(info.biHeight) < 1) {
		plcl_exception::throw_formatted(plcl_exception(),
			"BmpPlugin::ReadInfo(): invalid header biWidth: %d, biHeight: %d",
			info.biWidth, info.biHeight);
	}
	if (!((info.biCompression == BI_RGB) || (info.biCompression == BI_RLE8)
		|| (info.biCompression == BI_RLE4) || (info.biCompression == BI_BITFIELDS)))
	{
		// These compression values are only valid for SetDIBitsToDevice and StretchDIBits when the hdc parameter specifies a printer device.
		if ((info.biCompression == BI_JPEG) || (info.biCompression == BI_PNG)) {
			throw plcl_exception("BmpPlugin::ReadInfo(): invalid header biCompression: BI_JPEG, BI_PNG are only valid when use a printer device");
		} else {
			plcl_exception::throw_formatted(plcl_exception(),
				"BmpPlugin::ReadInfo(): invalid header biCompression: %u",
				info.biCompression);
		}
	}
	// biBitCount == 0 The number of bits-per-pixel is specified or is implied by the JPEG or PNG format, so check biCompression before biBitCount
	if (!((info.biBitCount == 1) || (info.biBitCount == 4) || (info.biBitCount == 8)
		|| (info.biBitCount == 16) || (info.biBitCount == 24) || (info.biBitCount == 32))) {
		plcl_exception::throw_formatted(plcl_exception(),
			"BmpPlugin::ReadInfo(): invalid header biBitCount: %u",
			(unsigned int)info.biBitCount);
	}
	if ((info.biHeight < 0) && (!((info.biCompression == BI_RGB) || (info.biCompression == BI_BITFIELDS))))
		throw plcl_exception("BmpPlugin::ReadInfo(): invalid header for top-down DIB biCompression must be either BI_RGB or BI_BITFIELDS");

	if (width)
		*width = (unsigned int)info.biWidth;
	if (height)
		*height = (unsigned int)abs(info.biHeight);
	if (flip_y)
		*flip_y = info.biHeight < 0;
	if (bpp)
		*bpp = info.biBitCount;
	if (compression)
		*compression = info.biCompression;

	size_t n_colors(0);
	// if ((info.biCompression == BI_BITFIELDS) && (info.biSize == sizeof(BITMAPINFOHEADER))) {
	if (info.biCompression == BI_BITFIELDS) {
		if (!((info.biBitCount == 16) || (info.biBitCount == 32))) {
			plcl_exception::throw_formatted(plcl_exception(),
				"BmpPlugin::ReadInfo(): invalid header for BI_BITFIELDS compression biBitCount: %u",
				(unsigned int)info.biBitCount);
		}
		n_colors = 3;
	} else if (info.biClrUsed > arraysize(colors)) {
		if (info.biBitCount < 16) {
			/*plcl_exception::throw_formatted(plcl_exception(),
				"BmpPlugin::ReadInfo(): invalid header biClrUsed: %u, biBitCount: %u",
				info.biClrUsed, (unsigned int)info.biBitCount);*/
			cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
				"BmpPlugin::ReadInfo(): invalid header biClrUsed: %u, biBitCount: %u",
				info.biClrUsed, (unsigned int)info.biBitCount);
			n_colors = arraysize(colors);
			// ReadInfo() also catch this: if (n_colors > arraysize(colors))
		}
	} else if (info.biBitCount < 16) {
		n_colors = info.biClrUsed;
		// If biClrUsed is zero, the bitmap uses the maximum number of colors corresponding to the value of the biBitCount member 
		// for the compression mode specified by biCompression.
		if (!n_colors && info.biCompression == BI_RGB)
			n_colors = 1 << info.biBitCount;
	}
	if (n_colors_)
		*n_colors_ = n_colors;
}
template<>
inline void ReadBMPInfo<BITMAPV4HEADER>(BITMAPV4HEADER const &info, bool *flip_y,
																			 unsigned int *width, unsigned int *height, unsigned int *bpp,
																			 DWORD *compression, RGBQUAD (&colors)[256], size_t *n_colors) {
	BITMAPINFOHEADER *info_ = (BITMAPINFOHEADER*)&info;
	// TODO: bV4CSType == LCS_CALIBRATED_RGB bV4Endpoints bV4GammaRed bV4GammaGreen bV4GammaBlue
	ReadBMPInfo(*info_, flip_y, width, height, bpp, compression, colors, n_colors);
	if (info.bV4V4Compression == BI_BITFIELDS) {
		if (n_colors)
			*n_colors = 0;
		((DWORD*)&colors)[0] = info.bV4RedMask;
		((DWORD*)&colors)[1] = info.bV4GreenMask;
		((DWORD*)&colors)[2] = info.bV4BlueMask;
	}
}
template<>
inline void ReadBMPInfo<BITMAPV5HEADER>(BITMAPV5HEADER const &info, bool *flip_y,
																			 unsigned int *width, unsigned int *height, unsigned int *bpp,
																			 DWORD *compression, RGBQUAD (&colors)[256], size_t *n_colors) {
	BITMAPV4HEADER *info_ = (BITMAPV4HEADER*)&info;
	// TODO: ICM profile, bV5CSType == LCS_sRGB, LCS_WINDOWS_COLOR_SPACE, PROFILE_LINKED, PROFILE_EMBEDDED bV5Intent bV5ProfileData bV5ProfileSize
	ReadBMPInfo(*info_, flip_y, width, height, bpp, compression, colors, n_colors);
}

// template<class T> inline void IOStream::ReadPod(cpcl::IOStream *input, T *r) ??
template<class T>
inline void ReadBMPInfoPod(cpcl::IOStream *input, T *r) {
	if (input->Read(r, sizeof(T)) != sizeof(T))
		throw plcl_exception("BmpPlugin::ReadInfo(): EOF");
}
// probaly not needed - ReadInfo() move to shared header - used in Doc::CreatePage && Page::Render
// BMPPage may receive stream already positioned on bits data, i.e. BMPDoc read all stats witn ReadInfo, store it's
// and GetPage() { just set all stats on BMPPage instance }
// then ReadInfo may declare only in bmp_doc.cpp
// replace some non-fatal error's with cpcl::Warning - for example ignore biPlanes value
inline void ReadInfo(cpcl::IOStream *input, bool *flip_y, size_t *bits_offset,
										 unsigned int *width, unsigned int *height, unsigned int *bpp,
										 DWORD *compression, RGBQUAD (&colors)[256], size_t *n_colors_) {
	BITMAPFILEHEADER file_info;
	ReadBMPInfoPod(input, &file_info);
	if (file_info.bfType != (WORD)(unsigned('B') | (unsigned('M') << 8))) {
		plcl_exception::throw_formatted(plcl_exception(),
			"BmpPlugin::ReadInfo(): invalid header bfType: %04X",
			(unsigned int)file_info.bfType);
	}

	DWORD biSize;
	ReadBMPInfoPod(input, &biSize);
	if ((file_info.bfOffBits < (sizeof(BITMAPFILEHEADER) + biSize))
		|| (file_info.bfOffBits > (unsigned int)input->Size())) {
		plcl_exception::throw_formatted(plcl_exception(),
			"BmpPlugin::ReadInfo(): invalid header bfOffBits: %u",
			file_info.bfOffBits);
	}
	if (bits_offset)
		*bits_offset = file_info.bfOffBits;
	input->Seek(-(__int64)sizeof(DWORD), SEEK_CUR, 0);
	size_t n_colors(0);
	switch (biSize) {
		case sizeof(BITMAPINFOHEADER):
			{
				BITMAPINFOHEADER info;
				ReadBMPInfoPod(input, &info);
				ReadBMPInfo(info, flip_y, width, height, bpp, compression, colors, &n_colors);
				// check n_colors <= (file_info.bfOffBits - (sizeof(BITMAPFILEHEADER) + biSize)) / sizeof(RGBQUAD)
			}
			break;
		case sizeof(BITMAPV4HEADER):
			{
				BITMAPV4HEADER info;
				ReadBMPInfoPod(input, &info);
				ReadBMPInfo(info, flip_y, width, height, bpp, compression, colors, &n_colors);
				// check n_colors <= (file_info.bfOffBits - (sizeof(BITMAPFILEHEADER) + biSize)) / sizeof(RGBQUAD)
			}
			break;
		case sizeof(BITMAPV5HEADER):
			{
				BITMAPV5HEADER info;
				ReadBMPInfoPod(input, &info);
				ReadBMPInfo(info, flip_y, width, height, bpp, compression, colors, &n_colors);
				// For packed DIBs, the profile data should follow the bitmap bits similar to the file format, so we suppose
				// that color table follow BITMAPV5HEADER
				// check n_colors <= (file_info.bfOffBits - (sizeof(BITMAPFILEHEADER) + biSize)) / sizeof(RGBQUAD)
			}
			break;
		default:
			plcl_exception::throw_formatted(plcl_exception(),
				"BmpPlugin::ReadInfo(): invalid header size: %08X, supported: BITMAPINFOHEADER(%04X), BITMAPV4HEADER(%04X), BITMAPV5HEADER(%04X)",
				biSize, sizeof(BITMAPINFOHEADER), sizeof(BITMAPV4HEADER), sizeof(BITMAPV5HEADER));
	}

	size_t n_max_colors = (file_info.bfOffBits - (sizeof(BITMAPFILEHEADER) + biSize)) / sizeof(RGBQUAD);
	if (n_colors > n_max_colors)
		n_colors = n_max_colors;
	if (n_colors > arraysize(colors)) {
		cpcl::Trace(CPCL_TRACE_LEVEL_WARNING,
			"BmpPlugin::ReadInfo(): color table contain more than %u entries, truncated",
			arraysize(colors));
		n_colors = arraysize(colors);
	}
	if (n_colors) {
		size_t to_read = n_colors * sizeof(RGBQUAD);
		if (input->Read(colors, to_read) != to_read)
			throw plcl_exception("BmpPlugin::ReadInfo(): unable to read colors: EOF");
	}
	if (n_colors_)
		*n_colors_ = n_colors;
}

} // namespace bmp
