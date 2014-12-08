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

#include <plcl_exception.hpp>

#include <splash/SplashBitmap.h>
#include "pdf_rendering_data.h"

PdfRenderingData::PdfRenderingData(unsigned int pixel_format_) : plcl::RenderingData(pixel_format_)
{}
PdfRenderingData::~PdfRenderingData()
{}

unsigned int PdfRenderingData::Width() const {
	if (output_dev.get() && output_dev->getBitmap()) {
		return (unsigned int)output_dev->getBitmap()->getWidth();
	}
	return 0;
}
unsigned int PdfRenderingData::Height() const {
	if (output_dev.get() && output_dev->getBitmap()) {
		return (unsigned int)output_dev->getBitmap()->getHeight();
	}
	return 0;
}
int PdfRenderingData::Stride() const {
	if (output_dev.get() && output_dev->getBitmap()) {
		return output_dev->getBitmap()->getRowSize();
	}
	return 0;
}

unsigned char* PdfRenderingData::Scanline(unsigned int y) {
	if (output_dev.get() && output_dev->getBitmap()) {
		SplashBitmap *bmp = output_dev->getBitmap();

		unsigned int height = (unsigned int)bmp->getHeight();
		if (y >= height)
			return 0;

		int stride = bmp->getRowSize();
		unsigned char *data = bmp->getDataPtr();

		//return data + ((unsigned int)bmp->getHeight() - y - 1) * stride;
		return data + y * stride;
	}
	return 0;
}

inline unsigned char div255(int x) { return (unsigned char)((x + (x >> 8) + 0x80) >> 8); }
void PdfRenderingData::InvertBitmap(SplashColor color, bool swap_rb) {
	if (output_dev.get() == NULL)
		throw plcl_exception("PdfRenderingData::InvertBitmap(): output_dev is NULL");
	SplashBitmap *bmp = output_dev->getBitmap();
	if (!bmp)
		throw plcl_exception("PdfRenderingData::InvertBitmap(): bmp is NULL");

	unsigned int width = (unsigned int)bmp->getWidth(), height = (unsigned int)bmp->getHeight();
	int stride = bmp->getRowSize();
	unsigned char *data = bmp->getDataPtr(), *alpha = bmp->getAlphaPtr();

	if (alpha) {
		if (swap_rb) {
			for (unsigned int y = 0; y < height; ++y) {
				unsigned char *data_row = data + y * stride;
				unsigned char *alpha_row = alpha + y * width;

				for (unsigned int x = 0; x < width; ++x, ++alpha_row, data_row += 3) {
					unsigned char a = *alpha_row;
					unsigned char a_ = 255 - a;
					unsigned char r = div255(a_ * color[0] + a * data_row[0]);
					data_row[1] = div255(a_ * color[1] + a * data_row[1]);
					data_row[0] = div255(a_ * color[2] + a * data_row[2]);
					data_row[2] = r;
				}
			}
		} else {
			for (unsigned int y = 0; y < height; ++y) {
				unsigned char *data_row = data + y * stride;
				unsigned char *alpha_row = alpha + y * width;

				for (unsigned int x = 0; x < width; ++x, ++alpha_row, data_row += 3) {
					unsigned char a = *alpha_row;
					unsigned char a_ = 255 - a;
					data_row[0] = div255(a_ * color[0] + a * data_row[0]);
					data_row[1] = div255(a_ * color[1] + a * data_row[1]);
					data_row[2] = div255(a_ * color[2] + a * data_row[2]);
				}
			}
		}
	} else if (swap_rb) {
		for (unsigned int y = 0; y < height; ++y) {
			unsigned char *data_row = data + y * stride;
			for (unsigned int x = 0; x < width; ++x, data_row += 3) {
				unsigned char r = data_row[0];
				data_row[0] = data_row[2];
				data_row[2] = r;
			}
		}
	}
}
