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

//#include "color_conv.h" only needed for Pixfmt(unsigned int)
#include "plcl_exception.hpp"

#include "dib_data.h"

namespace plcl {

DIBData::DIBData(unsigned int pixel_format_, unsigned int width_, unsigned int height_)
	: RenderingData(pixel_format_), data(NULL), width(width_), height(height_)
{}
DIBData::~DIBData() {
	if (data)
		free(data);
}

unsigned int DIBData::Width() const {
	return width;
}
unsigned int DIBData::Height() const {
	return height;
}
int DIBData::Stride() const {
	return width * BitsPerPixel() / 8;
}

//void DIBData::Pixfmt(unsigned int v) {
//	if ((RenderingData::Pixfmt() == v) || (!data))
//		return;
//
//	plcl::ColorConv color_conv = plcl::GetConverter(RenderingData::Pixfmt(), v);
//	unsigned int stride = color_conv.storage_needed(width);
//	unsigned char *to = data; // rgb24 ->{ok} gray8 ->{Stride() != buffer size, useless memory reallocation} rgb24
//	if (stride > (unsigned int)(abs(Stride()))) // inplace conversion not work // replace with stride != Stride()
//		to = (unsigned char*)malloc(stride * height);
//	if (!to)
//		throw plcl_exception("DIBData::Pixfmt(): no storage for color_conv");
//
//	for (size_t i = 0; i < height; ++i)
//		color_conv.convert(to + i * stride, data + i * Stride(), width);
//
//	if (to != data)
//		free(data);
//	data = to;
//	pixel_format = v;
//}

unsigned char* DIBData::Scanline(unsigned int y) {
	if (y >= height)
		throw plcl_exception("DIBData::Scanline(): y out of range");
	if (!data) {
		data = (unsigned char*)malloc(Stride() * height);
		if (!data)
			throw std::bad_alloc();
	}
	return data + y * Stride();
}

DIBData* DIBData::Copy(RenderingData *src, int pixel_format) {
	//if ((RenderingData::Pixfmt() == v) || (!data))
	//	return;

	//plcl::ColorConv color_conv = plcl::GetConverter(RenderingData::Pixfmt(), v);
	//unsigned int stride = color_conv.storage_needed(width);
	//unsigned char *to = data; // rgb24 ->{ok} gray8 ->{Stride() != buffer size, useless memory reallocation} rgb24
	//if (stride > (unsigned int)(abs(Stride()))) // inplace conversion not work // replace with stride != Stride()
	//	to = (unsigned char*)malloc(stride * height);
	//if (!to)
	//	throw plcl_exception("DIBData::Pixfmt(): no storage for color_conv");

	//for (size_t i = 0; i < height; ++i)
	//	color_conv.convert(to + i * stride, data + i * Stride(), width);

	//if (to != data)
	//	free(data);
	//data = to;
	//pixel_format = v;

	return 0;
}

} // namespace plcl
