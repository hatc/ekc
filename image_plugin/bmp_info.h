// bmp_info.h
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

namespace bmp {

struct BmpInfo {
	unsigned int width;
	unsigned int height;
	unsigned int bpp;
	size_t data_offset;
	bool flip_y;

	BmpInfo()
	{}
	BmpInfo(unsigned int width_, unsigned int height_, unsigned int bpp_, size_t data_offset_, bool flip_y_)
		: width(width_), height(height_), bpp(bpp_), data_offset(data_offset_), flip_y(flip_y_)
	{}
};

}

/*http://msdn.microsoft.com/en-us/library/windows/desktop/dd183386(v=vs.85).aspx
Bitmap Header Types (Windows)

By default, bitmap data is bottom-up in its format.
Bottom-up means that the first scan line in the bitmap data is the last scan line to be displayed.
For example, the 0th pixel of the 0th scan line of the bitmap data of a 10-pixel by 10-pixel bitmap will be the 0th pixel of the 9th scan line of the displayed or printed image.
Run-length encoded (RLE) format bitmaps and BITMAPCOREHEADER bitmaps cannot be top-down bitmaps.

The scan lines are DWORD aligned, except for RLE-compressed bitmaps.
They must be padded for scan line widths, in bytes, that are not evenly divisible by four, except for RLE compressed bitmaps.
For example, a 10- by 10-pixel 24-bpp bitmap will have two padding bytes at the end of each scan line.*/
