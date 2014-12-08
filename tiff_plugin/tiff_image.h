// tiff_image.h
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

#include "tiff_stuff.h"

#include <rendering_data.h>

struct TiffImage {
	enum EXTRASAMPLE_ALPHA {
		ALPHA_UNSPECIFIED = EXTRASAMPLE_UNSPECIFIED,
		ALPHA_ASSOCIATED = EXTRASAMPLE_ASSOCALPHA,
		ALPHA_UNASSOCIATED = EXTRASAMPLE_UNASSALPHA
	} alpha; // is_alpha = alpha != ALPHA_UNSPECIFIED;
	enum FLIP_IMAGE {
		FLIP_NONE = 0,
		FLIP_HORIZONTALLY = 1,
		FLIP_VERTICALLY = 1 << 1
	};
	int flip; // Render( flip_y = !!(flip & FLIP_VERTICALLY))

	/*typedef void (*libtiff_conv)(TiffImage const &img, unsigned char *out, unsigned int width, unsigned char *in);
	typedef void (*libtiff_conv_separate)(TiffImage const &img, unsigned char *out, unsigned int width,
		unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a);*/
	void (*libtiff_conv)(TiffImage const *img, unsigned char *out, unsigned int width,
		unsigned char *in);
	void (*libtiff_conv_separate)(TiffImage const *img, unsigned char *out, unsigned int width,
		unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a);

	bool planar_separate, tiled;
	uint32 width, height;
	uint16 bitspersample, samplesperpixel, photometric;
	uint16 *redcolormap, *greencolormap, *bluecolormap; // colormap pallete
	uint32 **palettecolormap; // rgb only, alpha ignored
	unsigned int pixfmt;

	unsigned int vertical_subsample;

	unsigned char gray_map[256];
	unsigned char **bwmap;     /* black&white map */
	//TIFFRGBValue* Map;                      /* sample mapping array */
	//uint32** BWmap;                         /* black&white map */
	//uint32** PALmap;                        /* palette image map */
	TIFFYCbCrToRGB* ycbcr;     /* YCbCr conversion state */
	TIFFCIELabToRGB* cielab;   /* CIE L*a*b conversion state */
	uint8* ualpha_aalpha;      /* Unassociated alpha to associated alpha convertion LUT */
	uint8* bitdepth16_8;       /* LUT for conversion from 16bit to 8bit values */

	TIFF *tif;

	TiffImage(TIFF *tif_) : alpha(ALPHA_UNSPECIFIED), flip(FLIP_NONE),
		width(0), height(0), bitspersample(0), samplesperpixel(0),
		photometric(PHOTOMETRIC_MINISWHITE), planar_separate(false), tiled(false),
		redcolormap(0), greencolormap(0), bluecolormap(0),
		ycbcr(0), cielab(0), ualpha_aalpha(0), bitdepth16_8(0),
		bwmap(0), palettecolormap(0), pixfmt(PLCL_PIXEL_FORMAT_INVALID),
		libtiff_conv(0), libtiff_conv_separate(0), tif(tif_),
		vertical_subsample(0)
	{}

	static TiffImage CreateTiffImage(TiffStuff &tiff_stuff);
};
