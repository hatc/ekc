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

#include "bmp_rle_reader.h"

// draw_util.hpp
namespace plcl {

inline void FillColor(unsigned char *scanline, unsigned int width, unsigned int pixfmt,
											unsigned char r, unsigned char g = 0, unsigned char b = 0, unsigned char a = 0) {
	switch (pixfmt) {
		case PLCL_PIXEL_FORMAT_BGRA_32:
		case PLCL_PIXEL_FORMAT_RGBA_32:
			{
				unsigned int pixel = (unsigned int)a << 24 | (unsigned int)g << 8;
				if (pixfmt == PLCL_PIXEL_FORMAT_BGRA_32)
					pixel |= (unsigned int)b | (unsigned int)r << 16;
				else
					pixel |= (unsigned int)r | (unsigned int)b << 16;
				
				for (unsigned int x = 0; x < width; ++x, scanline += 4)
					*(unsigned int*)scanline = pixel;
			}
			break;
		case PLCL_PIXEL_FORMAT_BGR_24:
		case PLCL_PIXEL_FORMAT_RGB_24:
			{
				unsigned int pixel = (unsigned int)g << 8;
				if (pixfmt == PLCL_PIXEL_FORMAT_BGR_24)
					pixel |= (unsigned int)b | (unsigned int)r << 16;
				else
					pixel |= (unsigned int)r | (unsigned int)b << 16;
				
				for (unsigned int x = 0; x < width - 1; ++x, scanline += 3)
					*(unsigned int*)scanline = pixel;
				
				scanline[1] = g;
				if (pixfmt == PLCL_PIXEL_FORMAT_BGR_24)
					scanline[0] = b, scanline[2] = r;
				else
					scanline[0] = r, scanline[2] = b;
			}
			break;
		case PLCL_PIXEL_FORMAT_GRAY_8:
			memset(scanline, r, width);
			break;
		default:
			plcl_exception::throw_formatted(plcl_exception(), "FillColor(): unsupported pixel format %u", pixfmt);
	}
}

} // namespace plcl

namespace bmp {

using plcl::FillColor;

#ifndef min
template<class T>
inline T min(T const &a, T const &b) {
	return a < b ? a : b;
}
#endif

inline RLEReader::Tokenizer::Token& RLEReader::Tokenizer::Next() {
	unsigned char c = reader->ReadByte();
	if (!c) {
		c = reader->ReadByte();
		switch (c) {
			case RLE_TOKEN_END_OF_LINE:
				token.type = RLE_TOKEN_END_OF_LINE;
				break;
			case RLE_TOKEN_END_OF_BITMAP:
				token.type = RLE_TOKEN_END_OF_BITMAP;
				break;
			case RLE_TOKEN_DELTA:
				token.type = RLE_TOKEN_DELTA;
				token.index[0] = reader->ReadByte();
				token.index[1] = reader->ReadByte();
				break;
			default:
				token.type = RLE_TOKEN_ABSOLUTE;
				token.n_index = c;
				size_t n_bytes = token.n_index;
				if (rle4)
					n_bytes = token.n_index / 2 + token.n_index % 2;
				reader->Read(token.index, n_bytes);
				if (n_bytes & 1)
					reader->ReadByte();
		}
	} else {
		token.type = RLE_TOKEN_ENCODED;
		token.n_index = c;
		token.index[0] = reader->ReadByte();
	}
	return token;
}

inline unsigned char* RLEReader::Tokenizer::CopyPixels(unsigned char *scanline, RGBQUAD *colors, size_t n_colors) const {
	return (rle4) ? CopyPixelsRle4(scanline, colors, n_colors) : CopyPixelsRle8(scanline, colors, n_colors);
}
inline unsigned char* RLEReader::Tokenizer::CopyPixelsRle8(unsigned char *scanline, RGBQUAD *colors, size_t n_colors) const {
	if (token.type == RLE_TOKEN_ENCODED) {
		if (token.index[0] < n_colors) {
			FillColor(scanline, token.n_index, PLCL_PIXEL_FORMAT_BGR_24,
				colors[token.index[0]].rgbRed, colors[token.index[0]].rgbGreen, colors[token.index[0]].rgbBlue);
		} else {
			cpcl::Warning("RLEReader::CopyPixels(): color index out of range");
			FillColor(scanline, token.n_index, PLCL_PIXEL_FORMAT_BGR_24, 0);
		}
		scanline += 3 * token.n_index;
	} else if (token.type == RLE_TOKEN_ABSOLUTE) {
		bool invalid_color_index(false);
		for (size_t i = 0; i < token.n_index; ++i, scanline += 3) {
			if (token.index[i] < n_colors) {
				scanline[2] = colors[token.index[i]].rgbRed;
				scanline[1] = colors[token.index[i]].rgbGreen;
				scanline[0] = colors[token.index[i]].rgbBlue;
			} else {
				if (!invalid_color_index) {
					cpcl::Warning("RLEReader::CopyPixels(): color index out of range");
					invalid_color_index = true;
				}
				scanline[2] = scanline[1] = scanline[0] = 0;
			}
		}
	} else
		throw plcl_exception("RLEReader::CopyPixels(): token.type must be either RLE_TOKEN_ENCODED or RLE_TOKEN_ABSOLUTE");

	return scanline;
}
inline bool ReadColorRle4(unsigned char index, unsigned char n_index, RGBQUAD *colors, size_t n_colors, RGBQUAD (&color)[2]) {
	bool invalid_color_index(false);
	unsigned char first = index >> 4;
	unsigned char second = index & 0x0F;
	if (first < n_colors)
		color[0] = colors[first];
	else
		invalid_color_index = true;
	if (n_index > 1) {
		if (second < n_colors)
			color[1] = colors[second];
		else
			invalid_color_index = true;
	}
	return !invalid_color_index;
}
inline unsigned char* RLEReader::Tokenizer::CopyPixelsRle4(unsigned char *scanline, RGBQUAD *colors, size_t n_colors) const {
	if (token.type == RLE_TOKEN_ENCODED) {
		RGBQUAD color[2] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
		if (!ReadColorRle4(token.index[0], token.n_index, colors, n_colors, color))
			cpcl::Warning("RLEReader::CopyPixels(): color index out of range");

		if ((color[0].rgbRed == color[1].rgbRed)
			&& (color[0].rgbGreen == color[1].rgbGreen)
			&& (color[0].rgbBlue == color[1].rgbBlue)) {
			FillColor(scanline, token.n_index, PLCL_PIXEL_FORMAT_BGR_24,
				color[0].rgbRed, color[0].rgbGreen, color[0].rgbBlue);
			scanline += 3 * token.n_index;
		} else {
			for (size_t i = 0; i < token.n_index; ++i, scanline += 3) {
				scanline[2] = color[i & 1].rgbRed;
				scanline[1] = color[i & 1].rgbGreen;
				scanline[0] = color[i & 1].rgbBlue;
			}
		}
	} else if (token.type == RLE_TOKEN_ABSOLUTE) {
		bool invalid_color_index(false);
		size_t i(0); // byte index
		for (size_t x = 0, n = token.n_index - (token.n_index & 1); x < n; ++i, x += 2, scanline += 6) {
			RGBQUAD color[2] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
			invalid_color_index |= !ReadColorRle4(token.index[i], token.n_index - x, colors, n_colors, color);

			scanline[5] = color[1].rgbRed;
			scanline[4] = color[1].rgbGreen;
			scanline[3] = color[1].rgbBlue;
			scanline[2] = color[0].rgbRed;
			scanline[1] = color[0].rgbGreen;
			scanline[0] = color[0].rgbBlue;
		}
		if (token.n_index & 1) {
			RGBQUAD color[2] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
			invalid_color_index |= !ReadColorRle4(token.index[i], 1, colors, n_colors, color);

			scanline[2] = color[0].rgbRed;
			scanline[1] = color[0].rgbGreen;
			scanline[0] = color[0].rgbBlue;
			scanline += 3;
		}
		if (invalid_color_index)
			cpcl::Warning("RLEReader::CopyPixels(): color index out of range");
	} else
		throw plcl_exception("RLEReader::CopyPixels(): token.type must be either RLE_TOKEN_ENCODED or RLE_TOKEN_ABSOLUTE");

	return scanline;
}

inline unsigned char* RLEReader::Tokenizer::CopyPixelsGray(unsigned char *scanline, RGBQUAD *colors, size_t n_colors) const {
	return (rle4) ? CopyPixelsGrayRle4(scanline, colors, n_colors) : CopyPixelsGrayRle8(scanline, colors, n_colors);
}
inline unsigned char* RLEReader::Tokenizer::CopyPixelsGrayRle8(unsigned char *scanline, RGBQUAD *colors, size_t n_colors) const {
	if (token.type == RLE_TOKEN_ENCODED) {
		if (token.index[0] < n_colors) {
			FillColor(scanline, token.n_index, PLCL_PIXEL_FORMAT_GRAY_8, colors[token.index[0]].rgbRed);
		} else {
			cpcl::Warning("RLEReader::CopyPixelsGray(): color index out of range");
			FillColor(scanline, token.n_index, PLCL_PIXEL_FORMAT_GRAY_8, 0);
		}
		scanline += token.n_index;
	} else if (token.type == RLE_TOKEN_ABSOLUTE) {
		bool invalid_color_index(false);
		for (size_t i = 0; i < token.n_index; ++i, ++scanline) {
			if (token.index[i] < n_colors) {
				*scanline = colors[token.index[i]].rgbRed;
			} else {
				if (!invalid_color_index) {
					cpcl::Warning("RLEReader::CopyPixelsGray(): color index out of range");
					invalid_color_index = true;
				}
				*scanline = 0;
			}
		}
	} else
		throw plcl_exception("RLEReader::CopyPixelsGray(): token.type must be either RLE_TOKEN_ENCODED or RLE_TOKEN_ABSOLUTE");

	return scanline;
}
inline unsigned char* RLEReader::Tokenizer::CopyPixelsGrayRle4(unsigned char *scanline, RGBQUAD *colors, size_t n_colors) const {
	if (token.type == RLE_TOKEN_ENCODED) {
		RGBQUAD color[2] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
		if (!ReadColorRle4(token.index[0], token.n_index, colors, n_colors, color))
			cpcl::Warning("RLEReader::CopyPixelsGray(): color index out of range");

		if ((color[0].rgbRed == color[1].rgbRed)
			&& (color[0].rgbGreen == color[1].rgbGreen)
			&& (color[0].rgbBlue == color[1].rgbBlue)) {
			FillColor(scanline, token.n_index, PLCL_PIXEL_FORMAT_GRAY_8, color[0].rgbRed);
			scanline += token.n_index;
		} else {
			for (size_t i = 0; i < token.n_index; ++i, ++scanline)
				*scanline = color[i & 1].rgbRed;
		}
	} else if (token.type == RLE_TOKEN_ABSOLUTE) {
		bool invalid_color_index(false);
		size_t i(0); // byte index
		for (size_t x = 0, n = token.n_index - (token.n_index & 1); x < n; ++i, x += 2, scanline += 2) {
			RGBQUAD color[2] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
			invalid_color_index |= !ReadColorRle4(token.index[i], token.n_index - x, colors, n_colors, color);

			scanline[0] = color[0].rgbRed;
			scanline[1] = color[1].rgbRed;
		}
		if (token.n_index & 1) {
			RGBQUAD color[2] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
			invalid_color_index |= !ReadColorRle4(token.index[i], 1, colors, n_colors, color);

			*scanline++ = color[0].rgbRed;
		}
		if (invalid_color_index)
			cpcl::Warning("RLEReader::CopyPixels(): color index out of range");
	} else
		throw plcl_exception("RLEReader::CopyPixelsGray(): token.type must be either RLE_TOKEN_ENCODED or RLE_TOKEN_ABSOLUTE");

	return scanline;
}

// adjust error messages to common format - 'identifier(%u)' or 'identifier : %u'
void RLEReader::Read(unsigned char *scanline, unsigned int *y_) {
	if (y == (unsigned int)-1)
		y = 0;
	else if (y >= height) {
		plcl_exception::throw_formatted(plcl_exception(),
			"RLEReader::Read(): current row(%u) >= height(%u)",
			y, height);
	}

	if (y_)
		*y_ = y;
	++y;

	if (y_skip) {
		FillColor(scanline, width, pixfmt, 0);
		--y_skip;
		return;
	}

	// x_offset may be above width, so ignore all data after filling up scanline - eol / eob / delta
	if (x_offset) {
		FillColor(scanline, x_offset, pixfmt, 0); // x_offset < width because if (x_offset >= width) x_offset = 0;
		scanline += plcl::RenderingData::Stride(pixfmt, x_offset);
	}
	for (size_t x = x_offset, eol = 0; eol != 1;) {
		switch (tokenizer.Next().type) {
			case Tokenizer::RLE_TOKEN_ENCODED:
			case Tokenizer::RLE_TOKEN_ABSOLUTE:
				if (x_offset > x) {
					FillColor(scanline, x_offset - x, pixfmt, 0);
					scanline += plcl::RenderingData::Stride(pixfmt, x_offset - x);
					x = x_offset;
				}
				
				if (x < width) {
					if (x + tokenizer.token.n_index >= width)
						tokenizer.token.n_index = width - x;
					
					if (pixfmt == PLCL_PIXEL_FORMAT_GRAY_8)
						scanline = tokenizer.CopyPixelsGray(scanline, colors, n_colors);
					else
						scanline = tokenizer.CopyPixels(scanline, colors, n_colors);
					
					x += tokenizer.token.n_index;
					x_offset = x;
				}
				break;
			case Tokenizer::RLE_TOKEN_END_OF_LINE:
				x_offset = 0;
				eol = 1;
				break;
			case Tokenizer::RLE_TOKEN_DELTA:
				x_offset += tokenizer.token.index[0];
				y_skip += tokenizer.token.index[1];
				if (y_skip) {
					--y_skip;
					eol = 1;
				}
				break;
			case Tokenizer::RLE_TOKEN_END_OF_BITMAP:
				y_skip = height - y;
				eol = 1;
		}

		if ((eol) && (x < width))
			FillColor(scanline, width - x, pixfmt, 0);
	}
	if (x_offset >= width)
		x_offset = 0;
}

void RLEReader::Skip(unsigned int lines) {
	if (y == (unsigned int)-1)
		y = 0;
	if (y + lines >= height) {
		plcl_exception::throw_formatted(plcl_exception(),
			"RLEReader::Skip(): row(%u) >= height(%u)",
			y, height);
	}

	y += lines;

	if (y_skip) {
		size_t v = min(y_skip, lines);
		lines -= v;
		y_skip -= v;
	}

	while (lines) {
		switch (tokenizer.Next().type) {
			case Tokenizer::RLE_TOKEN_END_OF_LINE:
				x_offset = 0;
				--lines;
				break;
			case Tokenizer::RLE_TOKEN_DELTA:
				x_offset += tokenizer.token.index[0];
				y_skip += tokenizer.token.index[1];
				if (y_skip) {
					size_t v = min(y_skip, lines);
					lines -= v;
					y_skip -= v;
				}
				break;
			case Tokenizer::RLE_TOKEN_END_OF_BITMAP:
				lines = 0;
				y_skip = height - y;
		}
	}
} // namespace bmp

}
