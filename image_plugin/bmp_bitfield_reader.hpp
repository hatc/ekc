// bmp_bitfield_reader.hpp
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

#include <rendering_data.h>
#include <plcl_exception.hpp>

#include "bmp_info.h"

namespace bmp {

template<class T>
struct is_signed {
	static bool const value = false;
};
template<>
struct is_signed<int> {
	static bool const value = true;
};

template<class T>
inline std::pair<size_t, size_t> popcnt(T v) {
	size_t r(0), h(0), t(0);
	
	for (size_t i = 0; i < sizeof(T) * 8 - (is_signed<T>::value ? 1 : 0); ++i, v >>= 1) {
		if (v & 1) {
			if (!r)
				h = i + 1;
			t = i + 1;

			++r;
		}
	}
	if (t)
		++t;
	
	return std::make_pair(r, t - h); // return (non-zero bits count, bits span - i.e. most-bit-position - least-bit-position)
}

inline size_t RightShift(unsigned int mask) {
	size_t r(0);
	//for (; !(mask & 1); ++r)
	for (; (mask) && (!(mask & 1)); ++r)
		mask >>= 1;
	return r;
}

inline size_t MaskBitCount(unsigned int mask) {
	std::pair<size_t, size_t> n = popcnt(mask);
	if (!n.first)
		throw plcl_exception("BmpBitfieldReader(): mask bits can't be zero");
	if (n.first != n.second)
		throw plcl_exception("BmpBitfieldReader(): mask bits must be contiguous");
	return n.first;
}

struct BmpBitfieldSettings : BmpInfo {
	/*unsigned int width, height;*/
	unsigned int pixfmt;
	unsigned int r_mask, g_mask, b_mask;
	size_t r_shift, g_shift, b_shift;

	BmpBitfieldSettings(BmpInfo const &bmp_info, unsigned int r_mask_, unsigned int g_mask_, unsigned int b_mask_,
		size_t n_bytes) : BmpInfo(bmp_info), r_mask(r_mask_), g_mask(g_mask_), b_mask(b_mask_),
		pixfmt(PLCL_PIXEL_FORMAT_BGR_24) {
		r_shift = RightShift(r_mask);
		g_shift = RightShift(g_mask);
		b_shift = RightShift(b_mask);

		if ((r_mask & g_mask) || (g_mask & b_mask) || (r_mask & b_mask))
			throw plcl_exception("BmpBitfieldReader(): mask bits can't overlap the bits of another mask");
		
		if (n_bytes < sizeof(unsigned int)) {
		unsigned int bitmask = (r_mask | g_mask) | b_mask;
		if ((bitmask >> 8 * n_bytes) != 0) {
			plcl_exception::throw_formatted(plcl_exception(),
				"BmpBitfieldReader(): mask bits %08X does not fit in %u bytes of pixel",
				bitmask, n_bytes);
		}
		}
	}
};
template<class BitfieldsPixel/*WORD | DWORD*/>
struct BmpBitfieldScaleLookup : BmpBitfieldSettings {
	size_t r_bits, g_bits, b_bits;

	struct BitMask : BmpBitfieldSettings {
		typedef BitfieldsPixel Pixel;

		unsigned char const *r, *g, *b;
		
		BitMask(BmpBitfieldSettings const &r) : BmpBitfieldSettings(r)
		{}
		void Read(Pixel *pixels, unsigned char *scanline) const {
			for (unsigned int y = 0; y < width; ++y, scanline += 3) {
				scanline[2] = r[(pixels[y] & r_mask) >> r_shift];
				scanline[1] = g[(pixels[y] & g_mask) >> g_shift];
				scanline[0] = b[(pixels[y] & b_mask) >> b_shift];
			}
		}
	};

	BmpBitfieldScaleLookup(BmpInfo const &bmp_info, unsigned int r_mask_, unsigned int g_mask_, unsigned int b_mask_,
		size_t r_bits_, size_t g_bits_, size_t b_bits_) : BmpBitfieldSettings(bmp_info, r_mask_, g_mask_, b_mask_, sizeof(BitfieldsPixel)),
		r_bits(r_bits_), g_bits(g_bits_), b_bits(b_bits_)
	{}

	template<class ScopedBuf>
	BitMask CreateBitMask(ScopedBuf &buf, size_t pixels_buf_size) const {
		BitMask bit_mask(*this);

		size_t n_bits[] = { r_bits, g_bits, b_bits };
		unsigned char const **lookup_tables[] = { &bit_mask.r, &bit_mask.g, &bit_mask.b };

		// calc total amount of required memory
		size_t n_buf(0);
		for (size_t i = 0; i < arraysize(n_bits); ++i)
			n_buf += 1 << n_bits[i];

		// its good then memory buffer in same address - cpu can cache entire memory block for r, g, b channels
		// so, if required amount of memory exceed static stack limit, place all lookup tables in one new allocated memory block
		// drop private r,g,b storage, use provided buf - BmpPage cpcl::ScopedBuf<unsigned char, 0x300> reader_buf;
		unsigned char *lookup_buffer = buf.Alloc(n_buf + pixels_buf_size);
		lookup_buffer += pixels_buf_size;
		
		for (size_t i = 0; i < arraysize(lookup_tables); ++i) {
			*lookup_tables[i] = lookup_buffer;
			for (size_t j = 0, n = 1 << n_bits[i]; j < n; ++j, ++lookup_buffer) {
				*lookup_buffer = static_cast<unsigned char>((float(j) * 255.f) / float(n - 1));
			}
		}
		/*
		>>> f = lambda x:int((float(x) * 255.0) / float((1 << 5) - 1))
		>>> g = lambda x:int((float(x) * 256.0) / float(1 << 5))
		>>> f(0b11111)
		255
		>>> g(0b11111)
		248
		>>> f = lambda x:int((float(x) * 255.0) / float((1 << 6) - 1))
		>>> g = lambda x:int((float(x) * 256.0) / float(1 << 6))
		>>> f(0b111111)
		255
		>>> g(0b111111)
		252
		*/
		return bit_mask;
	}
};
template<class BitfieldsPixel>
struct BmpBitfieldScaleFunction : BmpBitfieldSettings {
	// float d, m; (float(v) * m) / d - seems as for used precision and values range, the order of evaluation doesn't effect the accuracy of the result
	float m[3]; // == m / d

	struct BitMask : BmpBitfieldSettings {
		typedef BitfieldsPixel Pixel;

		float r_m, g_m, b_m;

		BitMask(BmpBitfieldSettings const &r) : BmpBitfieldSettings(r)
		{}
		void Read(Pixel *pixels, unsigned char *scanline) const {
			for (unsigned int y = 0; y < width; ++y, scanline += 3) {
				scanline[2] = static_cast<unsigned char>(float((pixels[y] & r_mask) >> r_shift) * r_m);
				scanline[1] = static_cast<unsigned char>(float((pixels[y] & g_mask) >> g_shift) * g_m);
				scanline[0] = static_cast<unsigned char>(float((pixels[y] & b_mask) >> b_shift) * b_m);
			}
		}
	};

	BmpBitfieldScaleFunction(BmpInfo const &bmp_info, unsigned int r_mask_, unsigned int g_mask_, unsigned int b_mask_,
		size_t r_bits, size_t g_bits, size_t b_bits) : BmpBitfieldSettings(bmp_info, r_mask_, g_mask_, b_mask_, sizeof(BitfieldsPixel)) {
		// m = { 255.f / float((1 << r_bits) - 1), 255.f / float((1 << g_bits) - 1), 255.f / float((1 << b_bits) - 1) };
		m[0] = 255.f / float((1 << r_bits) - 1);
		m[1] = 255.f / float((1 << g_bits) - 1);
		m[2] = 255.f / float((1 << b_bits) - 1);
	}

	template<class ScopedBuf>
	// BitMask CreateBitMask(ScopedBuf&/*buf not used*/) const {
	BitMask CreateBitMask(ScopedBuf &buf, size_t pixels_buf_size) const {
		buf.Alloc(pixels_buf_size);

		BitMask bit_mask(*this);
		bit_mask.r_m = m[0];
		bit_mask.g_m = m[1];
		bit_mask.b_m = m[2];
		return bit_mask;
	}
};
/*
float d, m; // (float(v) * m) / d
float m[3]; // (float(v) * m)     // m == m / d

seems as for used precision and values range, the order of evaluation doesn't effect the accuracy of the result:

srand(TimedSeed());
float m = 256.f, d = float(1 << 30);
float m_ = m / d;
for (size_t v = (1 << 10); v < (1 << 30); v <<= 1) {
	for (size_t i = 0; i < 0x1000; ++i) {
		int v_ = v + rand() % 0x1000;
		float a = (float(v_) * m) / d;
		float b = float(v_) * m_;
		if (int(a) != int(b))
			std::cout << std::fixed << a << " : " << b << std::endl;
	}
}
*/

template<class Settings>
struct BmpBitfieldReader {
	typedef Settings ReaderSettings;
	typedef typename Settings::BitMask::Pixel Pixel;

	size_t bitfield_stride; // pixels_buf_size;
	Pixel *bitfield_scanline; // pixels_buf;
	typename Settings::BitMask bit_mask;
	cpcl::IOStream *input;
	unsigned int height, y;

	template<class ScopedBuf>
	BmpBitfieldReader(cpcl::IOStream *input_, Settings const &settings, ScopedBuf &buf) : y((unsigned int)-1),
		/* declaration order dependent initialization */
		bitfield_stride(((settings.width * sizeof(Pixel) + 3) / 4) * 4), // ((v + pad - 1) / pad) * pad
		bit_mask(settings.CreateBitMask(buf, bitfield_stride)), height(settings.height), input(input_) {
		bitfield_scanline = (Pixel*)buf.Data();
		/* bmp suite test images is 127 x 64, so can test if they really aligned on DWORD 
		   stride for 16-bit bitfield == 127 * 2 == 254,
			 stride_with_padding == 256
		 */
	}

	void Read(unsigned char *scanline, unsigned int *y_) {
		if (y == (unsigned int)-1)
			y = 0;
		else if (y >= height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"BmpBitfieldReader::Read(): current row(%u) >= height(%u)",
				y, height);
		}

		if (input->Read(bitfield_scanline, bitfield_stride) != bitfield_stride)
			/* probably more prefer variant is try to recover - i.e. trace warning and fill scanlines with default color */
			throw plcl_exception("BmpBitfieldReader::Read(): unable to read bitmap data: EOF");
		bit_mask.Read(bitfield_scanline, scanline);

		if (y_)
			*y_ = y;
		++y;
	}

	void Skip(unsigned int lines) {
		if (y == (unsigned int)-1)
			y = 0;

		if (y + lines >= height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"BmpBitfieldReader::Skip(): row(%u) >= height(%u)",
				y + lines, height);
		}

		if (!input->Seek(bitfield_stride * lines, SEEK_CUR, 0)) {
			plcl_exception::throw_formatted(plcl_exception(),
				"BmpBitfieldReader::Skip(): IOStream::Seek() fails, current row(%u), offset(%u)",
				y, bitfield_stride * lines);
		}
		
		y += lines;
	}
};

} // namespace bmp
