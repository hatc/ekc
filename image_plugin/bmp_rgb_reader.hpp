// bmp_rgb_reader.hpp
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

#include <basic.h>
#include <io_stream.h>
#include <dumbassert.h>

#include <rendering_data.h>
#include <plcl_exception.hpp>

#include "bmp_info.hpp"
#include "bmp_info.h"

namespace bmp {

struct BmpPaletteSettings : BmpInfo {
	unsigned int pixfmt;
	RGBQUAD colors[256];
	size_t n_colors;

	/*template<bpp> ReaderImpl
	template<4> ReaderImpl
	template<1> ReaderImpl
	BmpDoc::GetPage() {
	 switch(bpp)
	  BmpPalleteSettings<8>
		BmpPalleteSettings<4>
		BmpPalleteSettings<1>
	}*/
	struct ReaderImpl : BmpInfo {
		unsigned char *reader_buf;
		size_t reader_buf_size;
		RGBQUAD const *colors;
		size_t n_colors;
		/*bool color_index_out_of_range;*/
		bool gray_colors;

		ReaderImpl(BmpInfo const &bmp_info) : BmpInfo(bmp_info)/*, color_index_out_of_range(false)*/
		{}
		unsigned char* ReaderBuf(unsigned char *scanline) const {
			return reader_buf;
		}
		RGBQUAD PixelColor(size_t index) const {
			RGBQUAD color = { 0, 0, 0, 0 };
			if (index < n_colors)
				color = colors[index];
			/*else if (!color_index_out_of_range) {
				cpcl::Warning("BmpRgbReader::Read(): color index out of range");
				color_index_out_of_range = true;
			}*/
			return color;
		}
		void ReadBpp1Gray(unsigned char *reader_buf_, unsigned char *scanline) const {
			unsigned char const colors_[] = { PixelColor(0).rgbRed, PixelColor(1).rgbRed };

			for (unsigned int x = 0; x < width - width % 8; x += 8, ++reader_buf_) {
				unsigned int bitmask(*reader_buf_);
				for (size_t i = 0; i < 8; ++i) {
					*scanline++ = colors_[(bitmask >> (7 - i)) & 1];
				}
			}
			// if (width % 8) unsigned int bitmask(*reader_buf_);
			for (unsigned int x = 0; x < width % 8; ++x) {
				*scanline++ = colors_[(static_cast<unsigned int>(*reader_buf_) >> (7 - x)) & 1];
			}
		}
		void ReadBpp1(unsigned char *reader_buf_, unsigned char *scanline) const {
			/*Each bit in the bitmap array represents a pixel.
			If the bit is clear, the pixel is displayed with the color of the first entry in the bmiColors table;
			if the bit is set, the pixel has the color of the second entry in the table.*/
			RGBQUAD const colors_[] = { PixelColor(0), PixelColor(1) };

			for (unsigned int x = 0; x < width - width % 8; x += 8, ++reader_buf_) {
				/*unsigned int bitmask(*reader_buf_);
				for (unsigned char * const tail = scanline + 24; scanline != tail; scanline += 3, bitmask <<= 1) {
					if ((bitmask <<= 1) & (1 << 8))
						;*/
				unsigned int bitmask(*reader_buf_);
				for (size_t i = 0; i < 8; ++i, scanline += 3) {
					RGBQUAD const color = colors_[(bitmask >> (7 - i)) & 1];
					scanline[2] = color.rgbRed;
					scanline[1] = color.rgbGreen;
					scanline[0] = color.rgbBlue;
				}
			}
			// if (width % 8) unsigned int bitmask(*reader_buf_);
			for (unsigned int x = 0; x < width % 8; ++x, scanline += 3) {
				RGBQUAD const color = colors_[(static_cast<unsigned int>(*reader_buf_) >> (7 - x)) & 1];
				scanline[2] = color.rgbRed;
				scanline[1] = color.rgbGreen;
				scanline[0] = color.rgbBlue;
			}
		}
		void ReadBpp4Gray(unsigned char *reader_buf_, unsigned char *scanline) const {
			for (unsigned int x = 0; x < width - width % 2; x += 2, ++reader_buf_) {
				unsigned int bitmask(*reader_buf_);
				*scanline++ = PixelColor((bitmask >> 4) & 0xF).rgbRed;
				*scanline++ = PixelColor(bitmask & 0xF).rgbRed;
			}
			if (width & 1) {
				unsigned int bitmask(*reader_buf_);
				*scanline++ = PixelColor((bitmask >> 4) & 0xF).rgbRed;
			}
		}
		void ReadBpp4(unsigned char *reader_buf_, unsigned char *scanline) const {
			/*Each pixel in the bitmap is represented by a 4-bit index into the color table.
			For example, if the first byte in the bitmap is 0x1F, the byte represents two pixels.
			The first pixel contains the color in the second table entry, and the second pixel contains the color in the sixteenth table entry.*/

			for (unsigned int x = 0; x < width - width % 2; x += 2, ++reader_buf_, scanline += 6) {
				unsigned int bitmask(*reader_buf_);
				RGBQUAD color = PixelColor((bitmask >> 4) & 0xF);
				scanline[2] = color.rgbRed;
				scanline[1] = color.rgbGreen;
				scanline[0] = color.rgbBlue;
				color = PixelColor(bitmask & 0xF);
				scanline[5] = color.rgbRed;
				scanline[4] = color.rgbGreen;
				scanline[3] = color.rgbBlue;
			}
			if (width & 1) {
				unsigned int bitmask(*reader_buf_);
				RGBQUAD color = PixelColor((bitmask >> 4) & 0xF);
				scanline[2] = color.rgbRed;
				scanline[1] = color.rgbGreen;
				scanline[0] = color.rgbBlue;
			}
		}
		void Read(unsigned char *reader_buf_, unsigned char *scanline) const {
			if (1 == bpp) {
				if (gray_colors)
					ReadBpp1Gray(reader_buf_, scanline);
				else
					ReadBpp1(reader_buf_, scanline);
			} else if (4 == bpp) {
				if (gray_colors)
					ReadBpp4Gray(reader_buf_, scanline);
				else
					ReadBpp4(reader_buf_, scanline);
			} else {
				if (gray_colors) {
					for (unsigned int x = 0; x < width; ++x, ++reader_buf_, ++scanline)
						*scanline = PixelColor(*reader_buf_).rgbRed;
				} else {
					for (unsigned int x = 0; x < width; ++x, ++reader_buf_, scanline += 3) {
						RGBQUAD const color = PixelColor(*reader_buf_);
						scanline[2] = color.rgbRed;
						scanline[1] = color.rgbGreen;
						scanline[0] = color.rgbBlue;
					}
				}
			}
		}
	};

	BmpPaletteSettings(BmpInfo const &bmp_info, RGBQUAD *colors_, size_t n_colors_, bool gray_colors) : BmpInfo(bmp_info),
		n_colors(n_colors_), pixfmt(gray_colors ? PLCL_PIXEL_FORMAT_GRAY_8 : PLCL_PIXEL_FORMAT_BGR_24) {
		DUMBASS_CHECK(bpp < 16);
		DUMBASS_CHECK(n_colors <= arraysize(colors));
		memcpy(colors, colors_, n_colors * sizeof(RGBQUAD));

		if (n_colors > size_t(1 << bpp)) {
			cpcl::Trace(CPCL_TRACE_LEVEL_WARNING,
				"BmpRgbReader(): the bitmap has a maximum of %u colors, palette number of colors: %u",
				(1 << bpp), n_colors);
		}
	}

	template<class ScopedBuf>
	ReaderImpl CreateReader(ScopedBuf &buf) const {
		ReaderImpl r(*this);

		r.colors = colors;
		r.n_colors = n_colors;
		r.gray_colors = PLCL_PIXEL_FORMAT_GRAY_8 == pixfmt;

		// ((v + pad - 1) / pad) * pad
		r.reader_buf_size = (bpp * width + 7) / 8; // bytes per scanline
		r.reader_buf_size = ((r.reader_buf_size + 3) / 4) * 4; // The scan lines are DWORD aligned, except for RLE-compressed bitmaps.
		r.reader_buf = buf.Alloc(r.reader_buf_size);

		return r;
	}
};

/*struct BmpRgbSettings : BmpInfo {
	unsigned int pixfmt;

	struct ReaderImpl {
		size_t reader_buf_size;
		size_t padding;

		ReaderImpl() : reader_buf(0)
		{}
		unsigned char* ReaderBuf(unsigned char *scanline) {
			return scanline;
		}
		void Read(unsigned char*, unsigned char*)
		{}
	};

	BmpRgbSettings(BmpInfo const &bmp_info) : BmpInfo(bmp_info),
		pixfmt((bmp_info.bpp == 32) ? PLCL_PIXEL_FORMAT_BGRA_32 : PLCL_PIXEL_FORMAT_BGR_24) {
		DUMBASS_CHECK(bpp == 32 || bpp == 24); // check message in assertion
	}

	template<class ScopedBuf>
	ReaderImpl CreateReader(ScopedBuf&) {
		ReaderImpl r;
		r.reader_buf_size = plcl::RenderingData::Stride(pixfmt, width);
		r.padding = (((r.reader_buf_size + 3) / 4) * 4) - r.reader_buf_size;
		return r;
	}
};*/
struct BmpRgbSettings : BmpInfo {
	unsigned int pixfmt;

	struct ReaderImpl {
		unsigned char *reader_buf;
		size_t reader_buf_size;
		size_t stride;

		ReaderImpl() : reader_buf(0)
		{}
		unsigned char* ReaderBuf(unsigned char *scanline) const {
			return (!!reader_buf) ? reader_buf : scanline;
		}
		void Read(unsigned char *reader_buf_, unsigned char *scanline) const {
			if (reader_buf_ != scanline)
				memcpy(scanline, reader_buf_, stride);
		}
	};

	BmpRgbSettings(BmpInfo const &bmp_info) : BmpInfo(bmp_info),
		pixfmt((bmp_info.bpp == 32) ? PLCL_PIXEL_FORMAT_BGRA_32 : PLCL_PIXEL_FORMAT_BGR_24) {
		DUMBASS_CHECK(bpp == 32 || bpp == 24);
	}

	template<class ScopedBuf>
	ReaderImpl CreateReader(ScopedBuf &buf) const {
		ReaderImpl r;

		r.stride = plcl::RenderingData::Stride(pixfmt, width);
		r.reader_buf_size = (((r.stride + 3) / 4) * 4);
		if (r.stride != r.reader_buf_size)
			r.reader_buf = buf.Alloc(r.reader_buf_size);

		return r;
	}
};

// memory allocation needed if bpp < 24 && !GrayColors or padding
// use provided by OutputDevice scanline as buffer for read from stream
template<class Settings>
struct BmpRgbReader {
	typedef Settings ReaderSettings;

	typename Settings::ReaderImpl reader;
	cpcl::IOStream *input;
	unsigned int height, y;

	template<class ScopedBuf>
	BmpRgbReader(cpcl::IOStream *input_, Settings const &settings, ScopedBuf &buf) : y((unsigned int)-1),
		reader(settings.CreateReader(buf)), height(settings.height), input(input_)
	{}

	void Read(unsigned char *scanline, unsigned int *y_) {
		if (y == (unsigned int)-1)
			y = 0;
		else if (y >= height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"BmpRgbReader::Read(): current row(%u) >= height(%u)",
				y, height);
		}

		unsigned char *reader_buf = reader.ReaderBuf(scanline);
		if (input->Read(reader_buf, reader.reader_buf_size) != reader.reader_buf_size)
			/* probably more prefer variant is try to recover - i.e. trace warning and fill scanlines with default color */
			throw plcl_exception("BmpRgbReader::Read(): unable to read bitmap data: EOF");
		reader.Read(reader_buf, scanline);

		if (y_)
			*y_ = y;
		++y;
	}

	void Skip(unsigned int lines) {
		if (y == (unsigned int)-1)
			y = 0;

		if (y + lines >= height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"BmpRgbReader::Skip(): row(%u) >= height(%u)",
				y + lines, height);
		}

		if (!input->Seek(reader.reader_buf_size * lines, SEEK_CUR, 0)) {
			plcl_exception::throw_formatted(plcl_exception(),
				"BmpRgbReader::Skip(): IOStream::Seek() fails, current row(%u), offset(%u)",
				y, reader.reader_buf_size * lines);
		}
		
		y += lines;
	}
};

} // namespace bmp
