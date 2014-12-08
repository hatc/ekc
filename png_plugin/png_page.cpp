// Copyright (C) 2012-2013 Yuri Agafonov
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
#include <basic.h>

#include "png_stuff.h"
#include "png_page.h"

#include <scoped_buf.hpp>
#include <plcl_exception.hpp>
#include <dib_data.h>

#include <render.hpp>

struct PngReader {
	png_structp png_ptr;
	unsigned int *y;
	unsigned int /*y, */height;
	unsigned char *skip_scanline_buf;

	PngReader(png_structp png_ptr_, unsigned int height_, unsigned int *y_)
		: png_ptr(png_ptr_), y(y_), height(height_), skip_scanline_buf(0) {
		*y = (unsigned int)-1;
	}
	void Read(unsigned char *scanline, unsigned int *y_) {
		if (*y == (unsigned int)-1)
			*y = 0;
		else if (*y >= height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"PngReader::Read(): current row(%u) >= height(%u)",
				*y, height);
		}

		png_bytep scanline_ = scanline;
		png_read_rows(png_ptr, &scanline_, NULL, 1);

		if (y_)
			*y_ = *y;
		++*y;
	}
	void Skip(unsigned int lines) {
		if (*y == (unsigned int)-1)
			*y = 0;

		if (*y + lines >= height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"PngReader::Skip(): row(%u) >= height(%u)",
				*y, height);
		}
		while (lines != 0) {
			png_bytep scanline = skip_scanline_buf;
			png_read_rows(png_ptr, &scanline, NULL, 1);
			++*y;
			--lines;
		}
	}
};

PngPage::PngPage(unsigned int width_, unsigned int height_, std::auto_ptr<cpcl::IOStream> &input_)
	: Page(width_, height_, 96), input(input_)
{}
PngPage::~PngPage()
{}

void PngPage::Render(plcl::RenderingDevice *output) {
	input->Seek(0, SEEK_SET, NULL);
	png_byte png_check[8];
	input->Read(png_check, arraysize(png_check));
	if (png_sig_cmp(png_check, (png_size_t)0, arraysize(png_check)) != 0)
		throw plcl_exception("PngPage::Render(): png header invalid, probably the underlying stream is corrupted");
	
	PngStuff png_stuff(input.get());
	png_structp png_ptr = png_stuff.png_ptr;
	png_infop info_ptr = png_stuff.info_ptr;

	std::auto_ptr<plcl::DIBData> interlace_handling_data;

	cpcl::ScopedBuf<unsigned char, 256 * 4> buf;
	cpcl::ScopedBuf<unsigned char, 2 * 4 * 256> render_buf_resample;
	cpcl::ScopedBuf<unsigned char, 4 * 256> render_buf_conv;

	/* Set error handling if you are using the setjmp/longjmp method (this is
	* the normal method of doing things with libpng). REQUIRED unless you
	* set up your own error handlers in the png_create_read_struct() earlier. */
	if (setjmp(png_jmpbuf(png_ptr))) {
		throw png_exception("PngPage::Render(): error");
	}

	// because we have already read the signature...
	png_set_sig_bytes(png_ptr, arraysize(png_check));

	// png_set_keep_unknown_chunks

	/* The call to png_read_info() gives us all of the information from the
  * PNG file before the first IDAT (image data chunk) */
	png_read_info(png_ptr, info_ptr);

	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	/* read the IHDR chunk. */
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		&interlace_type, NULL, NULL);

	/* Tell libpng to strip 16-bit/color files down to 8 bits per color. */
	if (bit_depth == 16) {
#ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
		/* Added at libpng-1.5.4. "strip_16" produces the same result that it
		* did in earlier versions, while "scale_16" is now more accurate. */
		png_set_scale_16(png_ptr);
#else
#ifdef PNG_READ_STRIP_16_TO_8_SUPPORTED
   /* If both SCALE and STRIP are required pngrtran will effectively cancel the
    * latter by doing SCALE first.  This is ok and allows apps not to check for
    * which is supported to get the right answer.
    */
	 png_set_strip_16(png_ptr);
#endif
#endif
	}

	/* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
	* byte into separate bytes (useful for paletted and grayscale images). */
	if (bit_depth < 8) {
		png_set_packing(png_ptr);
	}

	/* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
	if ((color_type == PNG_COLOR_TYPE_GRAY) && (bit_depth < 8)) {
		png_set_expand_gray_1_2_4_to_8(png_ptr); // png_ptr->transformations |= PNG_EXPAND
	}

	/* Expand palette images to RGB, low-bit-depth grayscale images to 8 bits,
	* transparency chunks to full alpha channel; and convert grayscale to RGB[A] */
	if ((color_type == PNG_COLOR_TYPE_PALETTE)
		|| (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))) {
		png_set_expand(png_ptr); // png_ptr->transformations |= (PNG_EXPAND | PNG_EXPAND_tRNS);
		if (color_type == PNG_COLOR_TYPE_GRAY)
			color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
	}

	/* Make a grayscale image + 8-bit alpha into 32 bit image. */
	if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
		png_set_gray_to_rgb(png_ptr);
	}
	
	/* Use blue, green, red order for pixels. */
	if ((color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		|| (color_type == PNG_COLOR_TYPE_PALETTE)
		|| (color_type == PNG_COLOR_TYPE_RGB)
		|| (color_type == PNG_COLOR_TYPE_RGB_ALPHA)) {
		png_set_bgr(png_ptr);
	}

	/* Turn on interlace handling. REQUIRED if you are not using
	* png_read_image(). To see how to handle interlacing passes,
	* see the png_read_row() method below: */
	int const num_passes = (interlace_type != PNG_INTERLACE_NONE) ? png_set_interlace_handling(png_ptr) : 1;

	// unlike the example in the libpng documentation, we have *no* idea where
	// this file may have come from - so if it doesn't have a file gamma, don't
	// do any correction ("do no harm")
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_gAMA)) {
		double gamma = 0, screen_gamma = 2.2;
		if (png_get_gAMA(png_ptr, info_ptr, &gamma)) {
			png_set_gamma(png_ptr, screen_gamma, gamma);
		}
	}

	/* Optional call to gamma correct and add the background to the palette
	* and update info structure. REQUIRED if you are expecting libpng to
	* update the palette for you (ie you selected such a transform above). */
	png_read_update_info(png_ptr, info_ptr);

	unsigned int const bit_depth_ = png_get_bit_depth(png_ptr, info_ptr);
	unsigned int const channels_ = png_get_channels(png_ptr, info_ptr);
	//unsigned int pixel_depth = png_get_bit_depth(png_ptr, info_ptr) * png_get_channels(png_ptr, info_ptr);
	if (bit_depth_ != 8) {
		png_exception::throw_formatted(png_exception(),
			"PngPage::Render(): invalid bit_depth(%u), IHDR bit_depth(%d), color_type(%d)",
			bit_depth_, bit_depth, color_type);
	}
	if ((channels_ == 2) || (channels_ > 4)) {
		png_exception::throw_formatted(png_exception(),
			"PngPage::Render(): invalid number of channels(%u), IHDR bit_depth(%d), color_type(%d)",
			channels_, bit_depth, color_type);
	}
	
	// color type may have changed, due to our transformations
	int color_type_ = color_type;
	color_type = png_get_color_type(png_ptr, info_ptr);
	unsigned int pixfmt = PLCL_PIXEL_FORMAT_INVALID;
	switch (color_type) {
		case PNG_COLOR_TYPE_GRAY:
			pixfmt = PLCL_PIXEL_FORMAT_GRAY_8;
			break;
		case PNG_COLOR_TYPE_RGB:
			pixfmt = PLCL_PIXEL_FORMAT_BGR_24;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
		case PNG_COLOR_TYPE_PALETTE:
		case PNG_COLOR_TYPE_RGB_ALPHA:
			pixfmt = PLCL_PIXEL_FORMAT_BGRA_32;
			break;
		default:
			png_exception::throw_formatted(png_exception(),
				"PngPage::Render(): invalid color_type %u, IHDR bit_depth(%d), color_type(%d)",
				color_type, bit_depth, color_type_);
	}

	unsigned int output_width(this->width), output_height(this->height);

	bool mirror_x, mirror_y; unsigned int angle;
	plcl::Page::ExifOrientation(ExifOrientation(), &mirror_x, &mirror_y, &angle);
	if (num_passes > 1) {
		interlace_handling_data.reset(new plcl::DIBData(pixfmt, width, height));
		for (int i = 0; i < num_passes; ++i) {
			for (unsigned int y = 0; y < height; ++y) {
				png_bytep scanline = interlace_handling_data->Scanline(y);
				png_read_rows(png_ptr, &scanline, NULL, 1);
			}
		}

		plcl::RenderingDataReader reader(interlace_handling_data.get());
		// plcl::Render(reader, width, height, pixfmt, output, output_width, output_height, true, render_buf_resample, render_buf_conv);
		plcl::Render(reader, width, height, pixfmt, output, output_width, output_height, true, render_buf_resample, render_buf_conv,
			mirror_x, mirror_y, angle);
	} else {
		unsigned int y;
		PngReader png_reader(png_ptr, height, &y);
		png_reader.skip_scanline_buf = buf.Alloc(plcl::RenderingData::Stride(pixfmt, width));
		// plcl::Render(png_reader, width, height, pixfmt, output, output_width, output_height, true, render_buf_resample, render_buf_conv);
		plcl::Render(png_reader, width, height, pixfmt, output, output_width, output_height, true, render_buf_resample, render_buf_conv,
			mirror_x, mirror_y, angle);

		for (; y < height; ++y) {
			png_bytep scanline = png_reader.skip_scanline_buf;
			png_read_rows(png_ptr, &scanline, NULL, 1);
		}
	}

	/* read rest of file, and get additional chunks in info_ptr - REQUIRED */
	png_read_end(png_ptr, info_ptr);
}
