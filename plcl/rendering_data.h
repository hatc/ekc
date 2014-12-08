// rendering_data.h
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

#ifndef __PLCL_RENDERING_DATA_H
#define __PLCL_RENDERING_DATA_H

enum PLCL_PIXEL_FORMAT {
	PLCL_PIXEL_FORMAT_INVALID = 0,
	PLCL_PIXEL_FORMAT_GRAY_8 = 1,
	PLCL_PIXEL_FORMAT_RGB_24 = 1 << 1,
	PLCL_PIXEL_FORMAT_BGR_24 = 1 << 2,
	PLCL_PIXEL_FORMAT_RGBA_32 = 1 << 3,
	PLCL_PIXEL_FORMAT_ARGB_32 = 1 << 4,
	PLCL_PIXEL_FORMAT_ABGR_32 = 1 << 5,
	PLCL_PIXEL_FORMAT_BGRA_32 = 1 << 6
};

namespace plcl {

/* RenderingData - each thread own his copy, after call mutex protected shared method IPage::RenderData(**IRenderingData) */
class RenderingData { // ScanlineSource
	RenderingData(RenderingData const&);
	RenderingData& operator=(RenderingData const&);
	/* RenderingData *a; *a = *b; so, private copy ctor not useless at all... */
protected:
	unsigned int pixel_format;
public:
	RenderingData(unsigned int pixel_format_);
	virtual ~RenderingData();

	virtual unsigned int Width() const = 0;
	virtual unsigned int Height() const = 0;
	virtual int Stride() const = 0;
	virtual unsigned int Pixfmt() const;
	virtual unsigned int BitsPerPixel() const;

	virtual unsigned char* Scanline(unsigned int y) = 0;

	static unsigned int BitsPerPixel(unsigned int pixfmt);
	static unsigned int Stride(unsigned int pixfmt, unsigned int width);
};

} // namespace plcl

#endif // __PLCL_RENDERING_DATA_H
