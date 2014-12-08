// scanline_reader_adapter.hpp
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

#ifndef __PLCL_SCANLINE_READER_ADAPTER_HPP
#define __PLCL_SCANLINE_READER_ADAPTER_HPP

#include "rendering_data.h"
#include "plcl_exception.hpp"

namespace plcl {

template<typename ScanlineReader, class Storage>
class ScanlineReaderAdapter : public RenderingData {
	ScanlineReader scanline_reader;
	unsigned int width, height, stride;
	unsigned int scanline_cache_y[2];
	unsigned char *scanline_cache[2];
	unsigned char *storage;
	enum : unsigned int { EMPTY_SCANLINE = (unsigned int)-1 };

	unsigned char* Read(size_t cache_idx, unsigned int y) {
		/*#include <basic.h>
		COMPILE_ASSERT(arraysize(scanline_cache_y) == arraysize(scanline_cache), cache_size_invalid);
		DYNAMIC_ASSERT(cache_idx < arraysize(scanline_cache), );*/
		unsigned int y_;
		scanline_reader.Read(scanline_cache[cache_idx], &y_);
		if (y != y_) {
			plcl_exception::throw_formatted(plcl_exception(),
				"ScanlineReaderAdapter::Read(%u, %u): scanline_reader desync: %u",
				cache_idx, y, y_);
		}
		scanline_cache_y[cache_idx] = y;
		return scanline_cache[cache_idx];
	}
public:
	/*template<class Storage>
	ScanlineReaderAdapter(ScanlineReader scanline_reader_, unsigned int width_, unsigned int height_, unsigned int pixel_format,
	Storage &storage_)*/
	ScanlineReaderAdapter(ScanlineReader scanline_reader_, unsigned int width_, unsigned int height_, unsigned int pixel_format,
		Storage &storage_) : RenderingData(pixel_format), scanline_reader(scanline_reader_), width(width_), height(height_) {
		stride = width * RenderingData::BitsPerPixel() / 8;
		storage = storage_.Alloc(stride * 2);
		scanline_cache_y[0] = scanline_cache_y[1] = EMPTY_SCANLINE;
		scanline_cache[0] = storage; scanline_cache[1] = storage + stride;
	}
	virtual ~ScanlineReaderAdapter()
	{}

	virtual unsigned int Width() const {
		return width;
	}
	virtual unsigned int Height() const {
		return height;
	}
	virtual int Stride() const {
		return static_cast<int>(stride);
	}

	virtual unsigned char* Scanline(unsigned int y) {
		if (y >= height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"ScanlineReaderAdapter::Scanline(%u): out of range, height == %u)",
				y, height);
		}

		if (scanline_cache_y[0] == y)
			return scanline_cache[0];
		else if (scanline_cache_y[1] == y)
			return scanline_cache[1];
		else {
			if (EMPTY_SCANLINE == scanline_cache_y[0]) {
				unsigned int y_ = (EMPTY_SCANLINE == scanline_cache_y[1]) ? 0 : scanline_cache_y[1] + 1;
				if (y > y_)
					scanline_reader.Skip(y - y_);

				return Read(0, y);
			} else if (EMPTY_SCANLINE == scanline_cache_y[1]) {
				unsigned int y_ = (EMPTY_SCANLINE == scanline_cache_y[0]) ? 0 : scanline_cache_y[0] + 1;
				if (y > y_)
					scanline_reader.Skip(y - y_);

				return Read(1, y);
			} else {
				size_t i = (scanline_cache_y[0] > scanline_cache_y[1]) ? 0 : 1;
				size_t j = (i == 0) ? 1 : 0;
				
				if (y > (scanline_cache_y[i] + 1))
					scanline_reader.Skip(y - scanline_cache_y[i] - 1);
				
				return Read(j, y);
			}
		}
	}
};

template<typename ScanlineReader, class Storage>
inline RenderingData* make_adapter(ScanlineReader scanline_reader,
	unsigned int width, unsigned int height, unsigned int pixel_format, Storage &storage) {
	if (!(pixel_format & (PLCL_PIXEL_FORMAT_GRAY_8 | PLCL_PIXEL_FORMAT_BGR_24 | PLCL_PIXEL_FORMAT_BGRA_32))) {
		plcl_exception::throw_formatted(plcl_exception(),
			"ScanlineReaderAdapter(): pixel format(%u) not supported",
			pixel_format);
	}
	/*unsigned int stride = RenderingData::Stride(pixel_format, width);
	unsigned char *storage = storage.Alloc(stride * 2);*/
	return new ScanlineReaderAdapter<ScanlineReader, Storage>(scanline_reader, width, height, pixel_format, storage);
}

} // namespace plcl

#endif // __PLCL_SCANLINE_READER_ADAPTER_HPP
