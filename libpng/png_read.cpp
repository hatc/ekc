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
#include <png.h>

#include <basic.h>

#include <formatted_exception.hpp>
#include <io_stream.h>
#include <scoped_buf.hpp>

#include <rendering_device.h>
#include <plcl_exception.hpp>

#include "dib_data.h"

#include <dumbassert.h>
#include <trace.h>

#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

namespace plcl {

struct RenderingDataReader {
	RenderingData *data;
	unsigned int v;
	unsigned int stride;

	RenderingDataReader(RenderingData *data_) : data(data_), v((unsigned int)-1) {
		stride = data->Width() * data->BitsPerPixel() / 8;
	}
	void Read(unsigned char *scanline, unsigned int *y_) {
		if (v == (unsigned int)-1)
			v = 0;
		else if (v >= data->Height())
			throw 0;

		//memcpy(scanline, data->Scanline(v++), (size_t)data->Stride()); // data padded
		memcpy(scanline, data->Scanline(v++), stride);
		if (y_)
			*y_ = v - 1;
	}
	void Skip(unsigned int lines) {
		v += lines;
	}
};

} // namespace plcl

template<typename ScanlineReader>
inline void render(ScanlineReader reader, unsigned int pixfmt, unsigned int width, unsigned int height, plcl::RenderingDevice *output, bool flip_y) {
	output->Pixfmt(pixfmt);
	output->SetViewport(0, 0, width, height);
	for (unsigned int y = 0; y < height; ++y) {
		unsigned char *scanline;
		output->SweepScanline(flip_y ? height - y - 1 : y, &scanline);
		reader.Read(scanline, NULL);
	}
}

class png_exception : public formatted_exception<png_exception> {
public:
	png_exception(char const *s = NULL) : formatted_exception<png_exception>(s)
	{}

	virtual char const* what() const {
		char const *s = formatted_exception<png_exception>::what();
		if (*s)
			return s;
		else
			return "png_exception";
	}
};

extern "C" void ThrowStdException() {
	cpcl::Error("[!!!]libpng:: abort()");
	DUMBASS_CHECK_EXPLANATION(false, "libpng abort()");
}
/*#undef PNG_CONSOLE_IO_SUPPORTED in pnglibconf.h then build as DLL*/

// check setjmp actualu works - crafty png 

struct PngStuff {
	png_structp png_ptr;
	png_infop info_ptr;

	bool operator!() const;
	PngStuff(cpcl::IOStream *stream);
	~PngStuff();
};

static void libpng_read_fn(png_structp png_ptr, png_bytep data, png_size_t size) {
	cpcl::IOStream *stream = (cpcl::IOStream*)png_get_io_ptr(png_ptr);

	unsigned long readed = stream->Read(data, (unsigned long)size);
	//if (size && (0 == readed))
	if (readed != size)
		png_error(png_ptr, "Read error: invalid or corrupted PNG file");
}

static void libpng_error_fn(png_structp png_ptr, png_const_charp msg) {
	cpcl::Error(msg);
	longjmp(png_jmpbuf(png_ptr), 1);
}

PngStuff::PngStuff(cpcl::IOStream *stream) : png_ptr(NULL), info_ptr(NULL) {
	/* Create and initialize the png_struct with the desired error handler
	* functions. If you want to use the default stderr and longjump method,
	* you can supply NULL for the last three parameters. We also supply the
	* the compiler header file version, so that we know if the application
	* was compiled with a compatible version of the library. */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL, libpng_error_fn, NULL);
	if (png_ptr) {
	/* Allocate/initialize the memory for image information. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr) {
		/* If you are using replacement read functions, instead of calling
		* png_init_io() here you would call: */
		png_set_read_fn(png_ptr, (void *)stream, libpng_read_fn);
    /* where user_io_ptr is a structure you want available to the callbacks */
	} else {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		png_ptr = NULL;
	}
	}
}

bool PngStuff::operator!() const {
	return (NULL == png_ptr) || (NULL == info_ptr);
}

PngStuff::~PngStuff() {
	if (!!(*this))
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}

inline unsigned int SkMulDiv255Round(unsigned int a, unsigned int b) {
	unsigned int prod = (a * b) + 128;
	return (prod + (prod >> 8)) >> 8;
}
inline unsigned int SkPremultiplyARGB(unsigned int a, unsigned int r, unsigned int g, unsigned int b) {
	if (a != 255) {
		r = SkMulDiv255Round(r, a);
		g = SkMulDiv255Round(g, a);
		b = SkMulDiv255Round(b, a);
	}
	return ((b << 24) | (g << 16) | (r << 8) | a);
}
struct PngReader {
	png_structp png_ptr;
	unsigned int y, width, height;
	unsigned int const *color_table;
	unsigned char *skip_scanline_buf;

	PngReader(png_structp png_ptr_, unsigned int width_, unsigned int height_)
		: png_ptr(png_ptr_), y((unsigned int)-1), width(width_), height(height_),
		skip_scanline_buf(0), color_table(0)
	{}
	void Read(unsigned char *scanline_, unsigned int *y_) {
		if (y == (unsigned int)-1)
			y = 0;
		else if (y >= height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"PngReader::Read(): current row(%u) >= height(%u)",
				y, height);
		}

		png_bytep scanline = scanline_;
		if (color_table) {
			scanline = skip_scanline_buf;
			png_read_rows(png_ptr, &scanline, NULL, 1);
			unsigned int *output_scanline = (unsigned int *)scanline_;
			for (unsigned int i = 0; i < width; ++i, ++scanline, ++output_scanline) {
				*output_scanline = color_table[*scanline];
			}
		} else
			png_read_rows(png_ptr, &scanline, NULL, 1);

		if (y_)
			*y_ = y;
		++y;
	}
	void Skip(unsigned int lines) {
		if (y + lines >= height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"PngReader::Skip(): row(%u) >= height(%u)",
				y, height);
		}
		while (lines != 0) {
			png_bytep scanline = skip_scanline_buf;
			png_read_rows(png_ptr, &scanline, NULL, 1);
			++y;
			--lines;
		}
	}
};

//inline unsigned int SkMulDiv255Round(unsigned int a, unsigned int b) {
//	unsigned int prod = (a * b) + 128;
//	return (prod + (prod >> 8)) >> 8;
//}
//inline unsigned int SkPremultiplyARGB(unsigned int a, unsigned int r, unsigned int g, unsigned int b) {
//	if (a != 255) {
//		r = SkMulDiv255Round(r, a);
//		g = SkMulDiv255Round(g, a);
//		b = SkMulDiv255Round(b, a);
//	}
//	return ((b << 24) | (g << 16) | (r << 8) | a);
//}
void PngRead(cpcl::IOStream *input, plcl::RenderingDevice *output) {
	png_byte png_check[8];
	input->Read(png_check, arraysize(png_check));
	if (png_sig_cmp(png_check, (png_size_t)0, arraysize(png_check)) != 0)
		throw plcl_exception("PngRead(): png header invalid, probably the underlying stream is corrupted");

	PngStuff png_stuff(input);
	png_structp png_ptr = png_stuff.png_ptr;
	png_infop info_ptr = png_stuff.info_ptr;

	// для PNG_COLOR_TYPE_PALETTE надо бы создать палитру и использовать её, если pixel_depth = 8 bit i.e. gray
	/* if (count > 256) count = 256; so... */
	unsigned int color_table[256];
	int color_count = 0;

	cpcl::ScopedBuf<unsigned char, 1024 * 4> buf;

	/* Set error handling if you are using the setjmp/longjmp method (this is
	* the normal method of doing things with libpng). REQUIRED unless you
	* set up your own error handlers in the png_create_read_struct() earlier. */
	if (setjmp(png_jmpbuf(png_ptr))) {
		throw png_exception("PngRead(): error");
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
#if 1
	// use if pallete not work
	/* Expand palette images to RGB, low-bit-depth grayscale images to 8 bits,
	* transparency chunks to full alpha channel; and convert grayscale to RGB[A] */
	if ((color_type == PNG_COLOR_TYPE_PALETTE)
		|| (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))) {
		png_set_expand(png_ptr); // png_ptr->transformations |= (PNG_EXPAND | PNG_EXPAND_tRNS);
		if (color_type == PNG_COLOR_TYPE_GRAY)
			color_type = PNG_COLOR_TYPE_GRAY_ALPHA;
	}
#endif

	/* Make a grayscale image + 8-bit alpha into 32 bit image. */
	if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
		//|| (color_type == PNG_COLOR_TYPE_GRAY)) {
		png_set_gray_to_rgb(png_ptr);
	}
	
	/* Use blue, green, red order for pixels. */
	if ((color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		//|| (color_type == PNG_COLOR_TYPE_GRAY)
		|| (color_type == PNG_COLOR_TYPE_PALETTE)
		|| (color_type == PNG_COLOR_TYPE_RGB)
		|| (color_type == PNG_COLOR_TYPE_RGB_ALPHA)) {
		png_set_bgr(png_ptr);
	}

#if 0
	// remove if pallete not work
	/* Add filler (or alpha) byte (before/after each RGB triplet) */
	if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY) {
	// ??? if (color_type == PNG_COLOR_TYPE_GRAY) {
		png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	}

	/*
	SkColorTable* colorTable = NULL;
	SkColorTable::SkColorTable(int count) : f16BitCache(NULL), fFlags(0) {
    if (count < 0)
        count = 0;
    else if (count > 256)
        count = 256;

    fCount = SkToU16(count);
    fColors = (SkPMColor*)sk_malloc_throw(count * sizeof(SkPMColor));
    memset(fColors, 0, count * sizeof(SkPMColor));
	}*/
	// т.е. SkColorTable по умолчанию для всех значений не попавших в палитру использует 0x00000000 - full transparent(alpha == 00), black color(000000)
	if (color_type == PNG_COLOR_TYPE_PALETTE) {
		// array of color entries
		png_colorp palette;
		int num_palette;
		// array of alpha (transparency) entries for palette
		png_bytep trans(0);
		int num_trans(0);
		
		memset(color_table, 0, sizeof(color_table)); // full transparent(alpha == 00), black color(000000)
		png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
		/* BUGGY IMAGE WORKAROUND
		
		We hit some images (e.g. fruit_.png) who contain bytes that are == colortable_count
		which is a problem since we use the byte as an index. To work around this we grow
		the colortable by 1 (if its < 256) and duplicate the last color into that slot. */
		color_count = num_palette + (num_palette < 256);

		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
			png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, NULL);
		} else {
			num_trans = 0;
		}        
		// check for bad images that might make us crash
		if (num_trans > num_palette)
			num_trans = num_palette;
		
		DUMBASS_CHECK(num_trans <= num_palette);
		int index = 0;
		// int transLessThanFF = 0;
		
		for (; index < num_trans; ++index) {
			// transLessThanFF |= (int)*trans - 0xFF;
			color_table[index] = SkPremultiplyARGB(*trans++, palette->red, palette->green, palette->blue);
			++palette;
		}
		// reallyHasAlpha |= (transLessThanFF < 0); проверяли есть ли частично прозрачные пиксели - i.e. alpha < 0xFF 
		
		for (; index < num_palette; ++index) {
			color_table[index] = ((((unsigned int)palette->blue) << 24) //SkPackARGB32(0xFF, palette->red, palette->green, palette->blue);
				| (((unsigned int)palette->green) << 16)
				| (((unsigned int)palette->red) << 8)
				| ((unsigned int)0xFF));
			++palette;
		}

		// see BUGGY IMAGE WORKAROUND comment above
		if (num_palette < 256) {
			if (index > 0) // ;))
				color_table[index] = color_table[index - 1];
		}
	}
#endif
	
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
			"PngRead(): invalid bit_depth(%u), IHDR bit_depth(%d), color_type(%d)",
			bit_depth_, bit_depth, color_type);
	}
	if ((channels_ == 2) || (channels_ > 4)) {
		png_exception::throw_formatted(png_exception(),
			"PngRead(): invalid number of channels(%u), IHDR bit_depth(%d), color_type(%d)",
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
		//case PNG_COLOR_TYPE_GRAY:
		case PNG_COLOR_TYPE_GRAY_ALPHA:
		case PNG_COLOR_TYPE_PALETTE:
		//case PNG_COLOR_TYPE_RGB:
		case PNG_COLOR_TYPE_RGB_ALPHA:
			pixfmt = PLCL_PIXEL_FORMAT_BGRA_32;
			break;
		default:
			png_exception::throw_formatted(png_exception(),
				"PngPage::Render(): invalid color_type %u, IHDR bit_depth(%d), color_type(%d)",
				color_type, bit_depth, color_type_);
	}

	bool flip_y(true);
	if (num_passes > 1) {
		DIBData data(pixfmt, width, height);
		for (int i = 0; i < num_passes; ++i) {
			for (unsigned int y = 0; y < height; ++y) {
				png_bytep scanline = data.Scanline(y);
				png_read_rows(png_ptr, &scanline, NULL, 1);
			}
		}
		plcl::RenderingDataReader reader(&data);

		render(reader, pixfmt, width, height, output, flip_y);
	} else {

	PngReader png_reader(png_ptr, width, height);
	//if (color_count > 0) || (output_size != size)
	png_reader.skip_scanline_buf = buf.Alloc(plcl::RenderingData::Stride(pixfmt, width));
	if (color_count > 0)
		png_reader.color_table = color_table;

	/*output->Pixfmt(pixfmt);
	output->SetViewport(0, 0, width, height);
	for (unsigned int y = 0; y < height; ++y) {
		unsigned char *scanline;
		output->SweepScanline(flip_y ? height - y - 1 : y, &scanline);
		png_reader.Read(scanline, NULL);
	}*/
	render(png_reader, pixfmt, width, height, output, flip_y);
	}

	/* read rest of file, and get additional chunks in info_ptr - REQUIRED */
	png_read_end(png_ptr, info_ptr);
}
