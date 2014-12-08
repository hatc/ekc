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
#include <basic.h>

#include "jpeg_stuff.h"
#include "jpeg_read.h"

#include <scoped_buf.hpp>
#include <plcl_exception.hpp>

#include <render.hpp>

inline unsigned int SkMulDiv255Round(unsigned int a, unsigned int b) {
	unsigned int prod = (a * b) + 128;
	return (prod + (prod >> 8)) >> 8;
}
// Convert a scanline of CMYK samples to RGBX in place.
inline void CMYK_BGR(unsigned char *scanline, unsigned int width) {
	// At this point we've received CMYK pixels from libjpeg. We
	// perform a crude conversion to RGB (based on the formulae 
	// from easyrgb.com):
	//  CMYK -> CMY
	//    C = ( C * (1 - K) + K )      // for each CMY component
	//  CMY -> RGB
	//    R = ( 1 - C ) * 255          // for each RGB component
	// Unfortunately we are seeing inverted CMYK so all the original terms
	// are 1-. This yields:
	//  CMYK -> CMY
	//    C = ( (1-C) * (1 - (1-K) + (1-K) ) -> C = 1 - C*K
	// The conversion from CMY->RGB remains the same
	for (unsigned int x = 0; x < width; ++x, scanline += 4) {
		/*scanline[2] = SkMulDiv255Round(scanline[0], scanline[3]);
		scanline[1] = SkMulDiv255Round(scanline[1], scanline[3]);
		scanline[0] = SkMulDiv255Round(scanline[2], scanline[3]);*/
		unsigned char r = (unsigned char)SkMulDiv255Round(scanline[0], scanline[3]);
		unsigned char g = (unsigned char)SkMulDiv255Round(scanline[1], scanline[3]);
		unsigned char b = (unsigned char)SkMulDiv255Round(scanline[2], scanline[3]);
		scanline[0] = b;
		scanline[1] = g;
		scanline[2] = r;
		scanline[3] = 255;
	}
}
struct JpegReader {
	j_decompress_ptr cinfo;
	//unsigned int y;
	unsigned int *y;
	unsigned char *skip_scanline_buf;

	//JpegReader(j_decompress_ptr cinfo_, unsigned int *output_y_) : cinfo(cinfo_), y((unsigned int)-1)
	JpegReader(j_decompress_ptr cinfo_, unsigned int *y_) : cinfo(cinfo_), y(y_) {
		*y = (unsigned int)-1;
	}
	void Read(unsigned char *scanline, unsigned int *y_) {
		if (*y == (unsigned int)-1)
			*y = 0;
		else if (*y >= cinfo->output_height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"JpegReader::Read(): current row(%u) >= output_height(%u)",
				*y, cinfo->output_height);
		}

		JSAMPROW scanline_ = scanline;
		/* while (scan lines remain to be read) jpeg_read_scanlines(...); */
		if (jpeg_read_scanlines(cinfo, &scanline_, 1) == 0) {
			jpeg_exception::throw_formatted(jpeg_exception(),
				"JpegReader::Read(): jpeg_read_scanlines fails, current row %u",
				*y);
		}
		if (cinfo->out_color_space == JCS_CMYK)
			CMYK_BGR(scanline, cinfo->output_width);

		if (y_)
			*y_ = *y;
		++*y;
	}
	void Skip(unsigned int lines) {
		if (*y == (unsigned int)-1)
			*y = 0;

		if (*y + lines >= cinfo->output_height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"JpegReader::Skip(): row(%u) >= output_height(%u)",
				*y + lines, cinfo->output_height);
		}
		while (lines != 0) {
			JSAMPROW scanline = skip_scanline_buf;
			if (jpeg_read_scanlines(cinfo, &scanline, 1) == 0) {
				jpeg_exception::throw_formatted(jpeg_exception(),
					"JpegReader::Skip(): jpeg_read_scanlines fails, current row %u",
					*y);
			}
			++*y;
			--lines;
		}
	}
};

static char const* JCS_NAMES[] = {
	"JCS_UNKNOWN"
	"JCS_GRAYSCALE",
	"JCS_RGB",
	"JCS_YCbCr",
	"JCS_CMYK",
	"JCS_YCCK"
};
void JpegRead(cpcl::IOStream *input, plcl::RenderingDevice *output, void (*change_size)(unsigned int &width, unsigned int &height)) {
	JpegStuff jpeg_stuff;
	j_decompress_ptr cinfo = &jpeg_stuff.cinfo;

	JpegInputManager source_manager(input);
	cinfo->src = &source_manager;

	cpcl::ScopedBuf<unsigned char, 3 * 1024> buf;
	cpcl::ScopedBuf<unsigned char, 2 * 3 * 1024> render_buf_resample;
	cpcl::ScopedBuf<unsigned char, 3 * 1024> render_buf_conv;

	// All objects need to be instantiated before this setjmp call so that
	// they will be cleaned up properly if an error occurs.
	if (setjmp(jpeg_stuff.jerr.jexit)) {
		throw jpeg_exception("JpegRead(): error");
	}

	if (jpeg_read_header(cinfo, TRUE) != JPEG_HEADER_OK)
		throw jpeg_exception("JpegRead(): jpeg header invalid, probably the underlying stream is corrupted");

	/* this gives about 30% performance improvement. In theory it may reduce the visual quality */
	cinfo->do_fancy_upsampling = FALSE;
	
	/* this gives another few percents */
	cinfo->do_block_smoothing = FALSE;

	/*supported decompression color conversions:
	cinfo->out_color_space == JCS_RGB:
    cinfo->out_color_components = RGB_PIXELSIZE;
    if (cinfo->jpeg_color_space == JCS_YCbCr) {
      cconvert->pub.color_convert = ycc_rgb_convert;
      build_ycc_rgb_table(cinfo);
    } else if (cinfo->jpeg_color_space == JCS_GRAYSCALE) {
      cconvert->pub.color_convert = gray_rgb_convert;
    } else if (cinfo->jpeg_color_space == JCS_RGB && RGB_PIXELSIZE == 3) {
      cconvert->pub.color_convert = null_convert;
    } else
      ERREXIT(cinfo, JERR_CONVERSION_NOTIMPL);

	so, if (cinfo.jpeg_color_space == JCS_CMYK) {
      // libjpeg cannot convert from CMYK to RGB - here we set up
      // so libjpeg will give us CMYK samples back and we will
      // later manually convert them to RGB
      cinfo.out_color_space = JCS_CMYK;
    } else {
       cinfo.out_color_space = JCS_RGB;
    }
	seems as correct*/
	if ((cinfo->jpeg_color_space == JCS_CMYK) || (cinfo->jpeg_color_space == JCS_YCCK))
		cinfo->out_color_space = JCS_CMYK;
	else if (cinfo->jpeg_color_space == JCS_GRAYSCALE)
		cinfo->out_color_space = JCS_GRAYSCALE;
	else
		cinfo->out_color_space = JCS_RGB;

	unsigned int output_width(cinfo->image_width), output_height(cinfo->image_height);

	cinfo->dct_method = JDCT_IFAST;
	cinfo->scale_num = calc_zoom(output_width, output_height, change_size);
	cinfo->scale_denom = 100; // scale 1/2, 15/8
	// N/M where N[1, 16], M[1, 8]

	// jpeg_core_output_dimensions

	/* start decompressor and calculate output width and height */
	jpeg_start_decompress(cinfo);

	unsigned int pixfmt = PLCL_PIXEL_FORMAT_INVALID;
	if ((JCS_CMYK == cinfo->out_color_space) && (4 == cinfo->out_color_components))
		pixfmt = PLCL_PIXEL_FORMAT_BGRA_32;
	else if ((JCS_RGB == cinfo->out_color_space) && (3 == cinfo->out_color_components))
		pixfmt = PLCL_PIXEL_FORMAT_BGR_24;
	else if ((JCS_GRAYSCALE == cinfo->out_color_space) && (1 == cinfo->out_color_components))
		pixfmt = PLCL_PIXEL_FORMAT_GRAY_8;
	else {
		jpeg_exception::throw_formatted(jpeg_exception(),
			"JpegPage::Render(): invalid out_color_space '%s', out_color_components %d",
			JCS_NAMES[cinfo->out_color_space], cinfo->out_color_components);
	}

	unsigned int y;
	JpegReader jpeg_reader(cinfo, &y);
	jpeg_reader.skip_scanline_buf = buf.Alloc(plcl::RenderingData::Stride(pixfmt, cinfo->output_width));
	plcl::Render(jpeg_reader, cinfo->output_width, cinfo->output_height, pixfmt,
		output, output_width, output_height, true, render_buf_resample, render_buf_conv);
	for (; y < cinfo->output_height; ++y) {
		JSAMPROW scanline = buf.Data();
		jpeg_read_scanlines(cinfo, &scanline, 1);
	}

	/* finish decompression */
	jpeg_finish_decompress(cinfo);
}
