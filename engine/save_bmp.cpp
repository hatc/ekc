// Copyright (C) 2013 Yuri Agafonov
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

#include <memory>

#include <file_stream.h>
#include <file_util.h>
#include <dib_data.h>

#include <trace.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

bool TestSaveBMP(/*cpcl::IOStream *input,*/wchar_t const *file_path, plcl::RenderingData *data) {
	unsigned int const bits_per_pixel = data->BitsPerPixel();
	if (!((bits_per_pixel == 8) || (bits_per_pixel == 24) || (bits_per_pixel == 32))) {
		cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
			"data with BitsPerPixel == %d not supported",
			bits_per_pixel);
		return false;
	}

	cpcl::FileStream *output_;
	if (!cpcl::FileStream::Create(file_path, &output_))
		return false;
	std::auto_ptr<cpcl::FileStream> output(output_);

	unsigned int const width = data->Width(), height = data->Height();
	unsigned long const stride = (unsigned long)abs(data->Stride());
	unsigned char const padding[4] = { 0 };
	unsigned long const pad = (stride % 4) ? 4 - (stride % 4) : 0;

	BITMAPINFOHEADER dib_info; memset(&dib_info, 0, sizeof(dib_info));
	dib_info.biSize = sizeof(dib_info);
	dib_info.biWidth = (LONG)width;
	dib_info.biHeight = (LONG)height;
	dib_info.biPlanes = 1;
	dib_info.biBitCount = bits_per_pixel;
	dib_info.biCompression = BI_RGB;
	dib_info.biSizeImage = height * (stride + pad/* data must be aligned for 4 byte, i.e. add padding */);
	// dib_info.biClrUsed = 0; // already set to zero

	BITMAPFILEHEADER file_info; memset(&file_info, 0, sizeof(file_info));
	file_info.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + dib_info.biSizeImage;
	file_info.bfType = (WORD)(unsigned('B') | (unsigned('M') << 8));
	file_info.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO);

	BITMAPINFO info; memset(&info, 0, sizeof(info));
	memcpy(&info, &dib_info, sizeof(dib_info));

	output->Write(&file_info, sizeof(file_info));
	output->Write(&info, sizeof(info));
	if (bits_per_pixel == 8) {
		COMPILE_ASSERT(sizeof(RGBQUAD) == sizeof(unsigned int), color_table_pointer_equal_int);
		unsigned int color_table_[255]; // BITMAPINFO already contain 1 RGBQUAD
		unsigned int *color_table = color_table_;
		for (unsigned int i = 1; i < 256; ++i, ++color_table)
			*color_table = i << 16 | i << 8 | i;

		output->Write(color_table_, sizeof(color_table_));
	}
	for (unsigned int y = 0; y < height; ++y) {
		output->Write(data->Scanline(y), stride);
		if (pad)
			output->Write(padding, pad);
	}

	return true;
}
bool TestSaveBMP(plcl::RenderingData *data) {
	cpcl::WStringPiece file_path = cpcl::WStringPieceFromLiteral(L"C:\\Source\\c++\\gnu-linux-draw\\test_draw\\tmp_test_draw_util\\exif_orientation\\out\\tmp.bmp");
	DeleteFilePath(file_path);
	return TestSaveBMP(file_path.data(), data);
}
