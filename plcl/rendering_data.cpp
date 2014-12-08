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

#include "rendering_data.h"
#include "plcl_exception.hpp"

namespace plcl {

RenderingData::RenderingData(unsigned int pixel_format_) : pixel_format(pixel_format_) 
{}
RenderingData::~RenderingData()
{}

unsigned int RenderingData::Pixfmt() const {
	return pixel_format;
}

inline unsigned int BytesPerPixel(unsigned int pixel_format) {
	if (static_cast<unsigned int>(PLCL_PIXEL_FORMAT_INVALID) == pixel_format)
		return 0;
	else if (static_cast<unsigned int>(PLCL_PIXEL_FORMAT_GRAY_8) == pixel_format)
		return 1;
	else if (pixel_format < static_cast<unsigned int>(PLCL_PIXEL_FORMAT_RGBA_32))
		return 3;
	else
		return 4;
}

unsigned int RenderingData::BitsPerPixel() const {
	unsigned int v = BytesPerPixel(pixel_format);
	if (!!v)
		return 8 * v;
	else
		throw plcl_exception("RenderingData::BitsPerPixel(): PIXEL_FORMAT_INVALID");
}

unsigned int RenderingData::BitsPerPixel(unsigned int pixfmt) {
	return BytesPerPixel(pixfmt) * 8;
}

unsigned int RenderingData::Stride(unsigned int pixfmt, unsigned int width) {
	return BytesPerPixel(pixfmt) * width;
}

} // namespace plcl
