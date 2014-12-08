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

#include "freeimagelib_page.h"

#include <scoped_buf.hpp>
#include <dumbassert.h>

#include <render.hpp>

FreeimagelibPage::FreeimagelibPage(unsigned int width_, unsigned int height_, boost::shared_ptr<FIBITMAP> fibitmap_)
	: Page(width_, height_, 96), fibitmap(fibitmap_) {
	unsigned int const bpp = FreeImage_GetBPP(fibitmap.get());
	DUMBASS_CHECK(bpp == 24 || bpp == 32);
}
FreeimagelibPage::~FreeimagelibPage()
{}

struct FibitmapReader {
	FIBITMAP *data;
	unsigned int v, stride, height;

	FibitmapReader(FIBITMAP *data_, unsigned int pixfmt, unsigned int width, unsigned int height_)
		: data(data_), v((unsigned int)-1), height(height_) {
		stride = plcl::RenderingData::Stride(pixfmt, width);
	}
	void Read(unsigned char *scanline, unsigned int *y_) {
		if (v == (unsigned int)-1)
			v = 0;
		else if (v >= height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"FibitmapReader::Read(): current row(%u) >= height(%u)",
				v, height);
		}

		//memcpy(scanline, data->Scanline(v++), (size_t)data->Stride()); // data padded
		memcpy(scanline, FreeImage_GetScanLine(data, (int)v++), stride);
		if (y_)
			*y_ = v - 1;
	}
	void Skip(unsigned int lines) {
		if (v == (unsigned int)-1)
			v = 0;

		if (v + lines >= height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"FibitmapReader::Skip(): row(%u) >= height(%u)",
				v + lines, height);
		}
		v += lines;
	}
};

void FreeimagelibPage::Render(plcl::RenderingDevice *output) {
	cpcl::ScopedBuf<unsigned char, 2 * 3 * 1024> resample_buf;
	cpcl::ScopedBuf<unsigned char, 3 * 1024> conv_buf;

	unsigned int fibitmap_width(FreeImage_GetWidth(fibitmap.get())), fibitmap_height(FreeImage_GetHeight(fibitmap.get()));
	unsigned int pixfmt = (FreeImage_GetBPP(fibitmap.get()) == 32) ? PLCL_PIXEL_FORMAT_BGRA_32 : PLCL_PIXEL_FORMAT_BGR_24;
	FibitmapReader reader(fibitmap.get(), pixfmt, fibitmap_width, fibitmap_height);
	bool mirror_x, mirror_y; unsigned int angle;
	plcl::Page::ExifOrientation(ExifOrientation(), &mirror_x, &mirror_y, &angle);
	/*plcl::Render(reader, fibitmap_width, fibitmap_height, pixfmt, output,
		this->width, this->height, false, resample_buf, conv_buf);*/
	plcl::Render(reader, fibitmap_width, fibitmap_height, pixfmt, output,
		this->width, this->height, false, resample_buf, conv_buf,
		mirror_x, mirror_y, angle);
}
