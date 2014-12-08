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

#include "rendering_device.h"

namespace plcl {

RenderingDevice::RenderingDevice(unsigned int supported_pixel_formats_, unsigned int pixel_format_)
	: supported_pixel_formats(supported_pixel_formats_), pixel_format(pixel_format_) {
	if ((supported_pixel_formats & pixel_format) == 0)
		pixel_format = PLCL_PIXEL_FORMAT_INVALID;
}
RenderingDevice::~RenderingDevice()
{}

unsigned int RenderingDevice::SupportedPixfmt() const {
	return supported_pixel_formats;
}
unsigned int RenderingDevice::Pixfmt() const {
	return pixel_format;
}
void RenderingDevice::Pixfmt(unsigned int v) {
	if (pixel_format != v) {
	if ((supported_pixel_formats & v) != 0)
		pixel_format = v;
	}
}

} // namespace plcl
