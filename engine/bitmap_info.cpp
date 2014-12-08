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

#include <stdlib.h>
#include <memory>

#include <scoped_buf.hpp>

#include <plcl_exception.hpp>
#include <render.hpp>
#include <memory_rendering_device.h>

#include "bitmap_info.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace plcl {

BitmapInfo::BitmapInfo(unsigned int pixel_format_, unsigned int width_, unsigned int height_)
	: RenderingData(pixel_format_), data(NULL), info(NULL), width(width_), height(height_) {
	unsigned long const stride = width * BitsPerPixel() / 8;
	data_stride = ((stride + BITMAPINFO_PADDING_BYTES - 1) / BITMAPINFO_PADDING_BYTES) * BITMAPINFO_PADDING_BYTES;
	pad = (stride % 4) ? 4 - (stride % 4) : 0; // == data_stride - stride
}
BitmapInfo::~BitmapInfo() {
	data = NULL;
	if (info)
		free(info);
}

unsigned int BitmapInfo::Width() const {
	return width;
}
unsigned int BitmapInfo::Height() const {
	return height;
}
int BitmapInfo::Stride() const {
	return (int)data_stride;
}

unsigned char* BitmapInfo::Scanline(unsigned int y) {
	if (y >= height)
		throw plcl_exception("BitmapInfo::Scanline(): y out of range");
	if (!data)
		data = AllocBitmapInfo();
	if (pad)
		memset(data + (y + 1) * data_stride - pad, 0, pad);
	return data + y * data_stride;
}

tagBITMAPINFO* BitmapInfo::Info() {
	if (!info)
		AllocBitmapInfo();
	return info;
}

unsigned char* BitmapInfo::AllocBitmapInfo() {
	unsigned int const bits_per_pixel = BitsPerPixel();
	if (!((bits_per_pixel == 24) || (bits_per_pixel == 32) || (bits_per_pixel == 8))) {
		plcl_exception::throw_formatted(plcl_exception(),
			"BitmapInfo::AllocBitmapInfo(): BitsPerPixel == %d not supported",
			bits_per_pixel);
	}

	BITMAPINFOHEADER dib_info; memset(&dib_info, 0, sizeof(dib_info));
	dib_info.biSize = sizeof(dib_info);
	dib_info.biWidth = (LONG)width;
	dib_info.biHeight = (LONG)height;
	dib_info.biPlanes = 1;
	dib_info.biBitCount = bits_per_pixel;
	dib_info.biCompression = BI_RGB;
	dib_info.biSizeImage = height * data_stride/* data must be aligned for 4 byte, i.e. add padding */;
	// dib_info.biClrUsed = 0; // already set to zero

	size_t alloc_size = sizeof(BITMAPINFO) + dib_info.biSizeImage;
	if (bits_per_pixel == 8)
		alloc_size += sizeof(RGBQUAD) * 255; // BITMAPINFO already contain 1 RGBQUAD
	info = (BITMAPINFO*)malloc(alloc_size);
	if (!info)
		throw std::bad_alloc();
	memcpy(info, &dib_info, sizeof(dib_info));

	data = (unsigned char*)info + sizeof(BITMAPINFO);

	RGBQUAD *color_table = info->bmiColors;
	if (bits_per_pixel == 8) {
		COMPILE_ASSERT(sizeof(RGBQUAD) == sizeof(unsigned int), color_table_pointer_equal_int);
		for (unsigned int i = 0; i < 256; ++i, ++color_table) {
			*(unsigned int*)color_table = i << 16 | i << 8 | i;
		}

		data += sizeof(RGBQUAD) * 255;
	} else {
		*(int*)color_table = 0;
	}

	return data;
}

BitmapInfo* BitmapInfo::Copy(RenderingData *data) {
	if (!data)
		return NULL;
	unsigned int const bits_per_pixel = data->BitsPerPixel();
	if (!((bits_per_pixel == 24) || (bits_per_pixel == 32) || (bits_per_pixel == 8))) {
		plcl_exception::throw_formatted(plcl_exception(),
			"BitmapInfo::Copy(): BitsPerPixel == %d not supported",
			bits_per_pixel);
	}

	/*unsigned int const height = data->Height();
	unsigned long const stride = abs(data->Stride());
	std::auto_ptr<BitmapInfo> r(new BitmapInfo(data->Pixfmt(), data->Width(), height));
	for (unsigned int y = 0; y < height; ++y)
		memcpy(r->Scanline(y), data->Scanline(y), stride);*/

	std::auto_ptr<BitmapInfo> r(new BitmapInfo(data->Pixfmt(), data->Width(), data->Height()));
	MemoryRenderingDevice rendering_device(r.get());
	cpcl::ScopedBuf<unsigned char, 0> unused, unused_;
	Render(RenderingDataReader(data), data->Width(), data->Height(), data->Pixfmt(),
		&rendering_device, r->Width(), r->Height(), false, unused, unused_);

	return r.release();
}

} // namespace plcl
