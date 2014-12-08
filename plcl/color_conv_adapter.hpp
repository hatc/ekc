// color_conv_adapter.hpp
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

#ifndef __PLCL_COLOR_CONV_ADAPTER_HPP
#define __PLCL_COLOR_CONV_ADAPTER_HPP

#include "color_conv.h"
#include "rendering_device.h"
#include "plcl_exception.hpp"

namespace plcl {

template<typename ScanlineReader, class Storage>
class ColorConvAdapter {
	ScanlineReader scanline_reader;
	ColorConv color_conv;
	unsigned int width;
	unsigned char *storage;
public:
	ColorConvAdapter(ScanlineReader scanline_reader_, unsigned int width_, unsigned int pixfmt_from, unsigned int pixfmt_to,
		Storage &storage_) : scanline_reader(scanline_reader_), width(width_), storage(0) {
		color_conv = GetConverter(pixfmt_from, pixfmt_to);
		if (RenderingData::Stride(pixfmt_from, width) != RenderingData::Stride(pixfmt_to, width))
			storage = storage_.Alloc(RenderingData::Stride(pixfmt_from, width)); // storage needed for scanline_reader.Read i.e. for input, then color_conv.storage_needed() returns stride needed for output
	}
	~ColorConvAdapter()
	{}

	void Read(unsigned char *scanline, unsigned int *y) {
		if (storage) {
			scanline_reader.Read(storage, y);
			color_conv.convert(scanline, storage, width);
		} else {
			scanline_reader.Read(scanline, y);
			color_conv.convert(scanline, scanline, width);
		}
	}

	void Skip(unsigned int lines) {
		scanline_reader.Skip(lines);
	}
};

inline unsigned int find_acceptable_pixfmt(unsigned int pixfmt, unsigned int supported_pixfmt) {
	if (supported_pixfmt & pixfmt)
		return pixfmt;
	if ((supported_pixfmt & (PLCL_PIXEL_FORMAT_RGB_24 | PLCL_PIXEL_FORMAT_BGR_24))
		&& (pixfmt & (PLCL_PIXEL_FORMAT_RGB_24 | PLCL_PIXEL_FORMAT_BGR_24)))
		return (PLCL_PIXEL_FORMAT_RGB_24 == pixfmt) ? PLCL_PIXEL_FORMAT_BGR_24 : PLCL_PIXEL_FORMAT_RGB_24;
	if ((PLCL_PIXEL_FORMAT_BGR_24 == pixfmt)
		&& (supported_pixfmt & (PLCL_PIXEL_FORMAT_BGRA_32 | PLCL_PIXEL_FORMAT_ABGR_32)))
		return (supported_pixfmt & PLCL_PIXEL_FORMAT_BGRA_32) ? PLCL_PIXEL_FORMAT_BGRA_32 : PLCL_PIXEL_FORMAT_ABGR_32;
	for (unsigned int i = pixfmt << 1; i <= PLCL_PIXEL_FORMAT_BGRA_32; i <<= 1) {
		if (supported_pixfmt & i)
			return i;
	}
	for (unsigned int i = pixfmt >> 1; i > 0; i >>= 1) {
		if (supported_pixfmt & i)
			return i;
	}
	return PLCL_PIXEL_FORMAT_INVALID;
}
inline unsigned int find_acceptable_pixfmt(unsigned int pixfmt, RenderingDevice *output) {
	return find_acceptable_pixfmt(pixfmt, output->SupportedPixfmt());
}

template<typename ScanlineReader, class Storage>
inline ColorConvAdapter<ScanlineReader, Storage> make_conv_adapter(ScanlineReader scanline_reader,
	unsigned int width, unsigned int pixfmt_from, unsigned int pixfmt_to, Storage &storage) {
	return ColorConvAdapter<ScanlineReader, Storage>(scanline_reader, width, pixfmt_from, pixfmt_to, storage);
}

} // namespace plcl

#endif // __PLCL_SCANLINE_READER_ADAPTER_HPP
