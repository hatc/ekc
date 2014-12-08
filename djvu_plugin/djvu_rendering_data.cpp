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

#include "djvu_rendering_data.h"

class DjvuRenderingData::Pixmap : public DJVU::GPixmap {
public:
	unsigned char* Scanline(unsigned short row) {
		return (row < nrows) ? (unsigned char*)&pixels[(nrows - row - 1) * nrowsize] : 0;
	}
	void Invert() {
		for (int r = 0; r < nrows; ++r) {
			unsigned char *pixels_row = (unsigned char*)&pixels[r * nrowsize];
			for (int c = 0; c < ncolumns; ++c) {
				unsigned char r = pixels_row[c * 3 + 2];
				pixels_row[c * 3 + 2] = pixels_row[c * 3];
				pixels_row[c * 3] = r;
			}
		}
	}
};

class DjvuRenderingData::Bitmap : public DJVU::GBitmap {
public:
	unsigned char* Scanline(unsigned short row) {
		if (row >= nrows)
			return 0;
		
		if (!bytes)
			uncompress();
		row = nrows - row - 1;
		return bytes + border + row * bytes_per_row;
	}
	void Invert() {
		if (grays < 256)
			return;
		
		if (!bytes)
			uncompress();
		unsigned char *pixels = bytes + border;
		for (int r = 0; r < nrows; ++r, pixels += bytes_per_row) {
			for (int c = 0; c < ncolumns; ++c) {
				pixels[c] = grays - 1 - pixels[c];
			}
		}
	}
};

void DjvuRenderingData::InvertBitmap() {
	if (!!bitmap)
		((Bitmap*)(DJVU::GBitmap*)bitmap)->Invert();
	else if (!!pixmap)
		((Pixmap*)(DJVU::GPixmap*)pixmap)->Invert();
}

DjvuRenderingData::DjvuRenderingData(unsigned int pixel_format_) : plcl::RenderingData(pixel_format_)
{}
DjvuRenderingData::~DjvuRenderingData()
{}

unsigned int DjvuRenderingData::Width() const {
	if (!!pixmap) {
		return pixmap->columns();
	} else if (!!bitmap) {
		return bitmap->columns();
	}
	return 0;
}
unsigned int DjvuRenderingData::Height() const {
	if (!!pixmap) {
		return pixmap->rows();
	} else if (!!bitmap) {
		return bitmap->rows();
	}
	return 0;
}
int DjvuRenderingData::Stride() const {
	if (!!pixmap) {
		return (int)(pixmap->rowsize() * 3);
	} else if (!!bitmap) {
		return (int)bitmap->rowsize();
	}
	return 0;
}

unsigned char* DjvuRenderingData::Scanline(unsigned int y) {
	if (!!pixmap) {
		return ((Pixmap*)(DJVU::GPixmap*)pixmap)->Scanline(y);
	} else if (!!bitmap) {
		return ((Bitmap*)(DJVU::GBitmap*)bitmap)->Scanline(y);
	}
	return 0;
}
