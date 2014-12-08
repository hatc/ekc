// bmp_rle_reader.h
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

#include <rendering_data.h>

#include "buffered_reader.hpp"
#include "bmp_info.h"
#include "bmp_info.hpp"

namespace bmp {

struct RLEReader {
	typedef cpcl::BufferedReader<0x1000> BufferedReader;
	/*private:*/struct Tokenizer {
		enum TOKEN_TYPE { RLE_TOKEN_END_OF_LINE = 0, RLE_TOKEN_END_OF_BITMAP = 1, RLE_TOKEN_DELTA = 2, RLE_TOKEN_ENCODED, RLE_TOKEN_ABSOLUTE };
		struct Token {
			TOKEN_TYPE type;

			unsigned char n_index;
			unsigned char index[0xFF]; // in encoded mode use index[0]
		} token;
		bool rle4; // info about compression needed in Next() { determine number of bytes to read in absolute mode } and in CopyPixels[Gray]
		BufferedReader *reader;

		Tokenizer(BufferedReader *reader_, bool rle4_) : reader(reader_), rle4(rle4_)
		{}
		Token& Next();
		
		unsigned char* CopyPixels(unsigned char *scanline, RGBQUAD *colors, size_t n_colors) const;
		unsigned char* CopyPixelsGray(unsigned char *scanline, RGBQUAD *colors, size_t n_colors) const;
	private:
		unsigned char* CopyPixelsRle8(unsigned char *scanline, RGBQUAD *colors, size_t n_colors) const;
		unsigned char* CopyPixelsGrayRle8(unsigned char *scanline, RGBQUAD *colors, size_t n_colors) const;
		unsigned char* CopyPixelsRle4(unsigned char *scanline, RGBQUAD *colors, size_t n_colors) const;
		unsigned char* CopyPixelsGrayRle4(unsigned char *scanline, RGBQUAD *colors, size_t n_colors) const;
	} tokenizer;
	struct ReaderSettings : BmpInfo {
		BufferedReader reader;
		RGBQUAD colors[256];
		size_t n_colors;
		unsigned int pixfmt;
		bool rle4;

		ReaderSettings(BmpInfo const &bmp_info, cpcl::IOStream *input, DWORD compression, RGBQUAD *colors_, size_t n_colors_, bool gray_colors)
			: BmpInfo(bmp_info), reader(input), n_colors(n_colors_), rle4(compression == BI_RLE4),
			pixfmt(gray_colors ? PLCL_PIXEL_FORMAT_GRAY_8 : PLCL_PIXEL_FORMAT_BGR_24) {
			memcpy(colors, colors_, n_colors * sizeof(RGBQUAD));
		}
	};
	
	unsigned int const width, height, pixfmt, stride;
	unsigned int y;
	RGBQUAD *colors;
	size_t n_colors;
	size_t x_offset, y_skip; // while (y_skip) FillColor(scanline, default_color)

	template<class ScopedBuf>
	RLEReader(cpcl::IOStream*, ReaderSettings /*const*/ &settings, ScopedBuf&) : tokenizer(&settings.reader, settings.rle4),
		width(settings.width), height(settings.height), pixfmt(settings.pixfmt),
		stride(plcl::RenderingData::Stride(settings.pixfmt, settings.width)),
		colors(settings.colors), n_colors(settings.n_colors),
		y((unsigned int)-1), x_offset(0), y_skip(0)
	{}

	void Read(unsigned char *scanline, unsigned int *y_);
	void Skip(unsigned int lines);

	// static bool GrayColors(RGBQUAD *colors, size_t n_colors);
};

}
