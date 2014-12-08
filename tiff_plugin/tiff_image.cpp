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
#include "tiff_image.h"

#include <dumbassert.h>

inline int libtiff_check_colormap(TiffImage const &img) {
	long n = (1L << img.bitspersample);
	uint16 const *r = img.redcolormap, *g = img.greencolormap, *b = img.bluecolormap;
	while (n-- > 0) {
		if (!((*r++ < 256) && (*g++ < 256) && (*b++ < 256)))
			return 16;
	}
	return 8;
}

inline void libtiff_shrink_colormap(TiffImage &img) { // shrink 16 -> 8 bit
	long n = (1L << img.bitspersample);
	uint16 *r = img.redcolormap, *g = img.greencolormap, *b = img.bluecolormap;
	while (n-- > 0) {
		*r++ = *r >> 8;
		*g++ = *g >> 8;
		*b++ = *b >> 8;
	}
}

inline uint32 libtiff_pack_rgba(uint8 r, uint8 g, uint8 b, uint8 a) {
	return (uint32)b | (uint32)g << 8 | (uint32)r << 16 | (uint32)a << 24;
}
inline uint32 libtiff_pack_rgb(uint8 r, uint8 g, uint8 b) {
	return libtiff_pack_rgba(r, g, b, 0xFF);
}
template<typename T>
inline uint32 libtiff_pack_rgba(T r, T g, T b, T a) {
	return libtiff_pack_rgba((uint8)(r & 0xFF), (uint8)(g & 0xFF), (uint8)(b & 0xFF), (uint8)(a & 0xFF));
}
template<typename T>
inline uint32 libtiff_pack_rgb(T r, T g, T b) {
	return libtiff_pack_rgb((uint8)(r & 0xFF), (uint8)(g & 0xFF), (uint8)(b & 0xFF));
}

/* для 1, 2, 4 bitpersample в одном байте содержится соотвественно 8, 4, 2 пикселя
   именно это и отражает (uint32**)malloc(256 * sizeof(uint32*) + 256 * nsamples * sizeof(uint32));
	 т.е. nsamples === number of samples(pixels) per byte
	 следовательно нам нужно распаковать данные - каждый байт можно представить как массив пикселей
	 например для 1 bitpersample байт 00010000 это 8 пикселей [ rgb(colormap[0], colormap[0], colormap[0]),
	  rgb(colormap[0], colormap[0], colormap[0]), rgb(colormap[0], colormap[0], colormap[0]),
		rgb(colormap[1], colormap[1], colormap[1]), rgb(colormap[0], colormap[0], colormap[0]),
		rgb(colormap[1], colormap[1], colormap[1]), rgb(colormap[0], colormap[0], colormap[0]),
		rgb(colormap[1], colormap[1], colormap[1]) ] при palletecolormap
	 или 8 пикселей [ gray(0), gray(0), gray(0), gray(1), gray(0), gray(0), gray(0), gray(0) ] для gray */

/*PHOTOMETRIC_PALLETE:
 bitspersample == 16 - not supported, but why not emulate same as put16bitbwtile ?
 bitspersample <= 8 -> make_palettecolormap

PHOTOMETRIC_CIELAB: - not use gray_map nor palettecolormap nor bwmap
PHOTOMETRIC_RGB: - not use gray_map nor palettecolormap nor bwmap
PHOTOMETRIC_YCBCR: - not use gray_map nor palettecolormap nor bwmap
PHOTOMETRIC_SEPARATED: - not use gray_map nor palettecolormap nor bwmap

PHOTOMETRIC_MINISBLACK:
PHOTOMETRIC_MINISWHITE:
 bitspersample == 16 => 8 // 16 bit input - use only high byte - put16bitbwtile
 bitspersample == 8 -> gray_map // use bw8, bw16
 bitspersample < 8 -> make_bwmap
 
т.е. используется или palettecolormap или bwmap или просто gray_map(если значения не упакованные(т.е. nsamples == 1), bwmap == gray_map) */

inline void* libtiff_make_bwmap(TiffImage &img) {
	//unsigned char **&bwmap = img.bwmap;
	unsigned char (&gray_map)[256] = img.gray_map;

	DUMBASS_CHECK(img.bitspersample < 8);
	uint16 nsamples = 8 / img.bitspersample;
	/*uint16 nsamples = 8 / img.bitspersample; // bitspersample <= 16
	if (nsamples == 0)
		nsamples = 1;*/

	img.bwmap = (unsigned char**)malloc(256 * sizeof(unsigned char*) + 256 * nsamples * sizeof(unsigned char));
	if (!img.bwmap)
		throw std::bad_alloc();

	unsigned char *p = (unsigned char*)(img.bwmap + 256);
	for (unsigned int i = 0; i < 256; i++) {
		// unsigned char v;
		img.bwmap[i] = p;
#ifdef GREY
#error GREY macro name already used
#endif
// #define	GREY(x)	v = (unsigned char)(x); *p++ = gray_map[v]
#define	GREY(x) *p++ = gray_map[(x)]
		switch (img.bitspersample) {
			case 1:
				GREY(i>>7);
				GREY((i>>6)&1);
				GREY((i>>5)&1);
				GREY((i>>4)&1);
				GREY((i>>3)&1);
				GREY((i>>2)&1);
				GREY((i>>1)&1);
				GREY(i&1);
				break;
			case 2:
				GREY(i>>6);
				GREY((i>>4)&3);
				GREY((i>>2)&3);
				GREY(i&3);
				break;
			case 4:
				GREY(i>>4);
				GREY(i&0xF);
				break;
			case 8:
			case 16:
				GREY(i);
				break;
		}
#undef GREY
	}

	return img.bwmap;
}

inline void libtiff_init_graymap(TiffImage &img) {
	size_t range = (1L << img.bitspersample) - 1;
	if (img.bitspersample == 16)
		range = 255;

	unsigned char (&gray_map)[256] = img.gray_map;

	if (PHOTOMETRIC_MINISWHITE == img.photometric) {
		for (size_t i = 0; i <= range; ++i)
			gray_map[i] = (((range - i) * 255) / range);
	} else {
		for (size_t i = 0; i <= range; ++i)
			gray_map[i] = ((i * 255) / range);
	}	
}

//void libtiff_make_palettecolormap(TiffImage &img, unsigned char *colormap_buf);
//template<class Storage>
//inline void libtiff_make_palettecolormap_wrapper(TiffImage &img, Storage &colormap_buf) {
//	uint16 const nsamples = 8 / img.bitspersample;
//	libtiff_make_palettecolormap(img, colormap_buf.Alloc(256 * sizeof(uint32*) + 256 * nsamples * sizeof(uint32)));
//}

inline void* libtiff_make_palettecolormap_16bit(TiffImage &img) {
	uint16 const *r = img.redcolormap, *g = img.greencolormap, *b = img.bluecolormap;
	size_t n_colors = 1L << img.bitspersample;

	uint32 *palettecolormap = (uint32*)malloc(n_colors * sizeof(uint32));
	if (!palettecolormap)
		throw std::bad_alloc();

	for (size_t i = 0; i < n_colors; ++i) {
		palettecolormap[i] = libtiff_pack_rgb(r[i], g[i], b[i]);
	}

	img.palettecolormap = (uint32**)palettecolormap;
	return img.palettecolormap;
}
/*
 * Palette images with <= 8 bits/sample are handled
 * with a table to avoid lots of shifts and masks.  The table
 * is setup so that put*cmaptile (below) can retrieve 8/bitspersample
 * pixel values simply by indexing into the table with one number.
 */
inline void* libtiff_make_palettecolormap(TiffImage &img) { // rgb only, alpha ignored
	//uint32 **&palettecolormap = img.palettecolormap;

	if (img.bitspersample == 16)
		return libtiff_make_palettecolormap_16bit(img);

	uint16 const *r = img.redcolormap, *g = img.greencolormap, *b = img.bluecolormap;
	uint16 nsamples = 8 / img.bitspersample; // bitspersample <= 8

	//palettecolormap = (uint32**)colormap_buf.Alloc(256 * sizeof(uint32*) + 256 * nsamples * sizeof(uint32));
	img.palettecolormap = (uint32**)malloc(256 * sizeof(uint32*) + 256 * nsamples * sizeof(uint32));
	if (!img.palettecolormap)
		throw std::bad_alloc();

	uint32 *p = (uint32*)(img.palettecolormap + 256);
	for (unsigned int i = 0; i < 256; ++i) {
		unsigned char v;
		img.palettecolormap[i] = p;
#ifdef CMAP
#error CMAP macro name already used
#endif
#define	CMAP(x)	v = (unsigned char)(x); *p++ = libtiff_pack_rgb(r[v], g[v], b[v])
		switch (img.bitspersample) {
			case 1:
				CMAP(i>>7);
				CMAP((i>>6)&1);
				CMAP((i>>5)&1);
				CMAP((i>>4)&1);
				CMAP((i>>3)&1);
				CMAP((i>>2)&1);
				CMAP((i>>1)&1);
				CMAP(i&1);
				break;
			case 2:
				CMAP(i>>6);
				CMAP((i>>4)&3);
				CMAP((i>>2)&3);
				CMAP(i&3);
				break;
			case 4:
				CMAP(i>>4);
				CMAP(i&0xF);
				break;
			case 8:
			case 16:
				CMAP(i);
				break;
		}
#undef CMAP
	}
	
	return img.palettecolormap;
}

/* void libtiff_conv_XXX && void libtiff_conv_separate_XXX */

template<size_t BitsPerSample>
void libtiff_conv_bwmap(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->bwmap);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 8);

	for (unsigned int x = 0; x < width;) {
		unsigned char *in_ = img->bwmap[*in++];
		for (size_t i = 0; ((i < (8 / BitsPerSample)) && (x < width)); ++i, ++x)
			*out++ = *in_++;
	}
}
//template<>
//void libtiff_conv_bwmap<8>(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
//	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 8);
//
//	/* why (in += img.samplesperpixel) not required if BitsPerSample < 8 ?
//	if ((planar_config == PLANARCONFIG_CONTIG)
//				&& (img.samplesperpixel != 1)
//				&& (img.bitspersample < 8)) {
//				throw tiff_exception*/
//	for (unsigned int x = 0; x < width; ++x, in += img->samplesperpixel) {
//		*out++ = *img->bwmap[*in];
//	}
//}
void libtiff_conv_gray(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 8);

	/* why (in += img.samplesperpixel) not required if BitsPerSample < 8 ?
	if ((planar_config == PLANARCONFIG_CONTIG)
				&& (img.samplesperpixel != 1)
				&& (img.bitspersample < 8)) {
				throw tiff_exception*/
	for (unsigned int x = 0; x < width; ++x, in += img->samplesperpixel) {
		*out++ = img->gray_map[*in];
	}
}
void libtiff_conv_gray_alpha(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(img->samplesperpixel > 1);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 32);

	uint32 *out_ = (uint32*)out;
	for (unsigned int x = 0; x < width; ++x, in += img->samplesperpixel) {
		uint8 v = img->gray_map[*in];
		uint8 alpha = *(in + 1);
		*out_++ = libtiff_pack_rgba(v, v, v, alpha);
	}
}
//template<>
//void libtiff_conv_bwmap<16>(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
//	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 8);
//
//	uint16 *in_ = (uint16*)in;
//	for (unsigned int x = 0; x < width; ++x, in += img->samplesperpixel) {
//		*out++ = *img->bwmap[*in_ >> 8];
//	}
//}
void libtiff_conv_gray_16(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 8);

	uint16 *in_ = (uint16*)in;
	for (unsigned int x = 0; x < width; ++x, in_ += img->samplesperpixel) {
		*out++ = img->gray_map[*in_ >> 8];
	}
}

template<size_t BitsPerSample>
void libtiff_conv_palettecolormap(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->palettecolormap);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 32);

	uint32 *out_ = (uint32*)out;
	/*for (unsigned int x = 0, j = 0; ; ++j) {
		uint32 *in_ = img->palettecolormap[in[j]];*/
	for (unsigned int x = 0; x < width;) {
		uint32 *in_ = img->palettecolormap[*in++];
		for (size_t i = 0; ((i < (8 / BitsPerSample)) && (x < width)); ++i, ++x)
			*out_++ = *in_++;
	}
}
template<>
void libtiff_conv_palettecolormap<8>(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->palettecolormap);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 32);

	uint32 *out_ = (uint32*)out;
	/* why (in += img.samplesperpixel) not required if BitsPerSample < 8 ?
	if ((planar_config == PLANARCONFIG_CONTIG)
				&& (img.samplesperpixel != 1)
				&& (img.bitspersample < 8)) {
				throw tiff_exception*/
	for (unsigned int x = 0; x < width; ++x, in += img->samplesperpixel) {
		*out_++ = *img->palettecolormap[*in];
	}
}
//template<>
//void libtiff_conv_palettecolormap<16>(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
//	DUMBASS_CHECK(!!img->palettecolormap);
//	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 32);
//
//	uint32 *out_ = (uint32*)out;
//	uint16 *in_ = (uint16*)in;
//	for (unsigned int x = 0; x < width; ++x, in_ += img->samplesperpixel) {
//		*out_++ = *img->palettecolormap[*in_ >> 8];
//	}
//}
template<>
void libtiff_conv_palettecolormap<16>(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->palettecolormap);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 32);

	uint32 *palettecolormap = (uint32*)img->palettecolormap;
	uint32 *out_ = (uint32*)out;
	uint16 *in_ = (uint16*)in;
	for (unsigned int x = 0; x < width; ++x, in_ += img->samplesperpixel) {
		*out_++ = palettecolormap[*in_];
	}
}

void libtiff_conv_rgbaa_8bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 32);

	uint32 *out_ = (uint32*)out;
	for (unsigned int x = 0; x < width; ++x, in += img->samplesperpixel) {
		*out_++ = libtiff_pack_rgba(in[0], in[1], in[2], in[3]);
	}
}

void libtiff_conv_rgbaa_16bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->bitdepth16_8);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 32);

	uint32 *out_ = (uint32*)out;
	uint16 *in_ = (uint16*)in;
	for (unsigned int x = 0; x < width; ++x, in_ += img->samplesperpixel) {
		*out_++ = libtiff_pack_rgba(img->bitdepth16_8[in_[0]],
			img->bitdepth16_8[in_[1]],
			img->bitdepth16_8[in_[2]],
			img->bitdepth16_8[in_[3]]);
	}
}

void libtiff_conv_rgbua_8bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->ualpha_aalpha);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 32);

	uint32 *out_ = (uint32*)out;
	for (unsigned int x = 0; x < width; ++x, in += img->samplesperpixel) {
		unsigned int a = in[3];
		uint8 *ualpha_aalpha = img->ualpha_aalpha + (a << 8);
		*out_++ = libtiff_pack_rgba(ualpha_aalpha[in[0]], ualpha_aalpha[in[1]], ualpha_aalpha[in[2]], in[3]);
	}
}

void libtiff_conv_rgbua_16bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->bitdepth16_8);
	DUMBASS_CHECK(!!img->ualpha_aalpha);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 32);

	uint32 *out_ = (uint32*)out;
	uint16 *in_ = (uint16*)in;
	for (unsigned int x = 0; x < width; ++x, in_ += img->samplesperpixel) {
		unsigned int a = img->bitdepth16_8[in_[3]];
		uint8 *ualpha_aalpha = img->ualpha_aalpha + (a << 8);
		*out_++ = libtiff_pack_rgba(ualpha_aalpha[img->bitdepth16_8[in_[0]]],
			ualpha_aalpha[img->bitdepth16_8[in_[1]]],
			ualpha_aalpha[img->bitdepth16_8[in_[2]]],
			img->bitdepth16_8[in_[3]]);
	}
}

void libtiff_conv_rgb_8bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	for (unsigned int x = 0; x < width; ++x, in += img->samplesperpixel) {
		*out++ = in[2];
		*out++ = in[1];
		*out++ = in[0];
	}
}

void libtiff_conv_rgb_16bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->bitdepth16_8);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	uint16 *in_ = (uint16*)in;
	for (unsigned int x = 0; x < width; ++x, in_ += img->samplesperpixel) {
		*out++ = img->bitdepth16_8[in_[2]];
		*out++ = img->bitdepth16_8[in_[1]];
		*out++ = img->bitdepth16_8[in_[0]];
	}
}

void libtiff_conv_separate_rgbaa_8bit(TiffImage const *img, unsigned char *out, unsigned int width,
	unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) {
	DUMBASS_CHECK((img->alpha != TiffImage::ALPHA_UNSPECIFIED) && (!!a));
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 32);

	uint32 *out_ = (uint32*)out;
	for (unsigned int x = 0; x < width; ++x) {
		*out_++ = libtiff_pack_rgba(*r++, *g++, *b++, *a++);
	}
}

void libtiff_conv_separate_rgbaa_16bit(TiffImage const *img, unsigned char *out, unsigned int width,
	unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) {
	DUMBASS_CHECK(!!img->bitdepth16_8);
	DUMBASS_CHECK((img->alpha != TiffImage::ALPHA_UNSPECIFIED) && (!!a));
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 32);

	uint32 *out_ = (uint32*)out;
	uint16 *r_ = (uint16*)r;
	uint16 *g_ = (uint16*)g;
	uint16 *b_ = (uint16*)b;
	uint16 *a_ = (uint16*)a;
	for (unsigned int x = 0; x < width; ++x) {
		*out_++ = libtiff_pack_rgba(img->bitdepth16_8[*r_++],
			img->bitdepth16_8[*g_++],
			img->bitdepth16_8[*b_++],
			img->bitdepth16_8[*a_++]);
	}
}

void libtiff_conv_separate_rgbua_8bit(TiffImage const *img, unsigned char *out, unsigned int width,
	unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) {
	DUMBASS_CHECK(!!img->ualpha_aalpha);
	DUMBASS_CHECK((img->alpha != TiffImage::ALPHA_UNSPECIFIED) && (!!a));
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 32);

	uint32 *out_ = (uint32*)out;
	for (unsigned int x = 0; x < width; ++x) {
		unsigned int a_value = *a++;
		uint8 *ualpha_aalpha = img->ualpha_aalpha + (a_value << 8);
		*out_++ = libtiff_pack_rgba(ualpha_aalpha[*r++], ualpha_aalpha[*g++], ualpha_aalpha[*b++], (uint8)a_value);
	}
}

void libtiff_conv_separate_rgbua_16bit(TiffImage const *img, unsigned char *out, unsigned int width,
	unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) {
	DUMBASS_CHECK(!!img->bitdepth16_8);
	DUMBASS_CHECK(!!img->ualpha_aalpha);
	DUMBASS_CHECK((img->alpha != TiffImage::ALPHA_UNSPECIFIED) && (!!a));
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 32);

	uint32 *out_ = (uint32*)out;
	uint16 *r_ = (uint16*)r;
	uint16 *g_ = (uint16*)g;
	uint16 *b_ = (uint16*)b;
	uint16 *a_ = (uint16*)a;
	for (unsigned int x = 0; x < width; ++x) {
		unsigned int a_value = img->bitdepth16_8[*a_++];
		uint8 *ualpha_aalpha = img->ualpha_aalpha + (a_value << 8);
		*out_++ = libtiff_pack_rgba(ualpha_aalpha[img->bitdepth16_8[*r_++]],
			ualpha_aalpha[img->bitdepth16_8[*g_++]],
			ualpha_aalpha[img->bitdepth16_8[*b_++]],
			(uint8)a_value);
	}
}

void libtiff_conv_separate_rgb_8bit(TiffImage const *img, unsigned char *out, unsigned int width,
	unsigned char *r, unsigned char *g, unsigned char *b, unsigned char*) {
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	for (unsigned int x = 0; x < width; ++x) {
		*out++ = *b++;
		*out++ = *g++;
		*out++ = *r++;
	}
}

void libtiff_conv_separate_rgb_16bit(TiffImage const *img, unsigned char *out, unsigned int width,
	unsigned char *r, unsigned char *g, unsigned char *b, unsigned char*) {
	DUMBASS_CHECK(!!img->bitdepth16_8);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	uint16 *r_ = (uint16*)r;
	uint16 *g_ = (uint16*)g;
	uint16 *b_ = (uint16*)b;
	for (unsigned int x = 0; x < width; ++x) {
		*out++ = img->bitdepth16_8[*b_++];
		*out++ = img->bitdepth16_8[*g_++];
		*out++ = img->bitdepth16_8[*r_++];
	}
}

void libtiff_conv_cmyk_8bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	for (unsigned int x = 0; x < width; ++x, in += img->samplesperpixel) {
		unsigned char k = 255 - in[3];
		*out++ = (k * (255 - in[2])) / 255;
		*out++ = (k * (255 - in[1])) / 255;
		*out++ = (k * (255 - in[0])) / 255;
	}
}

void libtiff_conv_cmyk_16bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	uint16 *in_ = (uint16*)in;
	for (unsigned int x = 0; x < width; ++x, in_ += img->samplesperpixel) {
		unsigned char k = 255 - (unsigned char)(in_[3] >> 8);
		*out++ = (k * (255 - (unsigned char)(in_[2] >> 8))) / 255;
		*out++ = (k * (255 - (unsigned char)(in_[1] >> 8))) / 255;
		*out++ = (k * (255 - (unsigned char)(in_[0] >> 8))) / 255;
	}
}

void libtiff_conv_separate_cmyk_8bit(TiffImage const *img, unsigned char *out, unsigned int width,
	unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) {
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	for (unsigned int x = 0; x < width; ++x) {
		unsigned char k = 255 - *a++;
		*out++ = (k * (255 - *b++)) / 255;
		*out++ = (k * (255 - *g++)) / 255;
		*out++ = (k * (255 - *r++)) / 255;
	}
}

void libtiff_conv_separate_cmyk_16bit(TiffImage const *img, unsigned char *out, unsigned int width,
	unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) {
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	uint16 *r_ = (uint16*)r;
	uint16 *g_ = (uint16*)g;
	uint16 *b_ = (uint16*)b;
	uint16 *a_ = (uint16*)a;
	for (unsigned int x = 0; x < width; ++x) {
		unsigned char k = 255 - (unsigned char)(*a_++ >> 8);
		*out++ = (k * (255 - (unsigned char)(*b_++ >> 8))) / 255;
		*out++ = (k * (255 - (unsigned char)(*g_++ >> 8))) / 255;
		*out++ = (k * (255 - (unsigned char)(*r_++ >> 8))) / 255;
	}
}

void libtiff_conv_cielab_8bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->cielab);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	for (unsigned int x = 0; x < width; ++x, in += img->samplesperpixel) {
		float X, Y, Z;
		// unsigned char v = 0xEF; (int)(signed char)v == 0xFFFFFFEF, (int)v = 0x000000EF;
		TIFFCIELabToXYZ(img->cielab, in[0], (signed char)in[1], (signed char)in[2], &X, &Y, &Z);
		uint32 r, g, b;
		TIFFXYZToRGB(img->cielab, X, Y, Z, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
	}
}

void libtiff_conv_cielab_16bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->cielab);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	uint16 *in_ = (uint16*)in;
	for (unsigned int x = 0; x < width; ++x, in_ += img->samplesperpixel) {
		float X, Y, Z;
		// unsigned char v = 0xEF; (int)(signed char)v == 0xFFFFFFEF, (int)v = 0x000000EF;
		TIFFCIELabToXYZ(img->cielab, (in_[0] >> 8), (int16)(in_[1] >> 8), (int16)(in_[2] >> 8), &X, &Y, &Z);
		uint32 r, g, b;
		TIFFXYZToRGB(img->cielab, X, Y, Z, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
	}
}

void libtiff_conv_ycbcr11_8bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->ycbcr);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	for (unsigned int x = 0; x < width; ++x, in += 3) {
		uint32 Cb = in[1];
		uint32 Cr = in[2];
		uint32 r, g, b;
		TIFFYCbCrtoRGB(img->ycbcr, in[0], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
	}
}

void libtiff_conv_ycbcr21_8bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->ycbcr);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	unsigned int const width_ = (width & 1) ? width - 1 : width;

	for (unsigned int x = 0; x < width_; x += 2, in += 4) {
		uint32 Cb = in[2];
		uint32 Cr = in[3];
		uint32 r, g, b;
		TIFFYCbCrtoRGB(img->ycbcr, in[0], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
		TIFFYCbCrtoRGB(img->ycbcr, in[1], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
	}

	if (width_ != width) {
		uint32 Cb = in[2];
		uint32 Cr = in[3];
		uint32 r, g, b;
		TIFFYCbCrtoRGB(img->ycbcr, in[0], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
	}
}

void libtiff_conv_ycbcr12_8bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->ycbcr);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	for (unsigned int x = 0; x < width; ++x, in += 4) {
		uint32 Cb = in[2];
		uint32 Cr = in[3];
		uint32 r, g, b;
		TIFFYCbCrtoRGB(img->ycbcr, in[img->vertical_subsample], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
	}
}

void libtiff_conv_ycbcr22_8bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->ycbcr);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	unsigned int const vertical_subsample = img->vertical_subsample * 2;

	unsigned int const width_ = (width & 1) ? width - 1 : width;

	for (unsigned int x = 0; x < width_; x += 2, in += 6) {
		uint32 Cb = in[4];
		uint32 Cr = in[5];
		uint32 r, g, b;
		TIFFYCbCrtoRGB(img->ycbcr, in[vertical_subsample], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
		TIFFYCbCrtoRGB(img->ycbcr, in[vertical_subsample + 1], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
	}

	if (width_ != width) {
		uint32 Cb = in[4];
		uint32 Cr = in[5];
		uint32 r, g, b;
		TIFFYCbCrtoRGB(img->ycbcr, in[vertical_subsample], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
	}
}

void libtiff_conv_ycbcr41_8bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->ycbcr);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	unsigned int const width_ = (width % 4) ? (width - (width % 4)) : width;

	for (unsigned int x = 0; x < width_; x += 4, in += 6) {
		uint32 Cb = in[4];
		uint32 Cr = in[5];
		uint32 r, g, b;
		TIFFYCbCrtoRGB(img->ycbcr, in[0], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
		TIFFYCbCrtoRGB(img->ycbcr, in[1], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
		TIFFYCbCrtoRGB(img->ycbcr, in[2], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
		TIFFYCbCrtoRGB(img->ycbcr, in[3], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
	}

	if (width_ != width) {
		uint32 Cb = in[4];
		uint32 Cr = in[5];
		for (unsigned int i = 0; i < (width % 4); ++i) {
			uint32 r, g, b;
			TIFFYCbCrtoRGB(img->ycbcr, in[i], Cb, Cr, &r, &g, &b);
			*out++ = b;
			*out++ = g;
			*out++ = r;
		}
	}
}

void libtiff_conv_ycbcr42_8bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->ycbcr);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	unsigned int const vertical_subsample = img->vertical_subsample * 4;

	unsigned int const width_ = (width % 4) ? (width - (width % 4)) : width;

	for (unsigned int x = 0; x < width_; x += 4, in += 10) {
		uint32 Cb = in[8];
		uint32 Cr = in[9];
		uint32 r, g, b;
		TIFFYCbCrtoRGB(img->ycbcr, in[vertical_subsample], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
		TIFFYCbCrtoRGB(img->ycbcr, in[vertical_subsample + 1], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
		TIFFYCbCrtoRGB(img->ycbcr, in[vertical_subsample + 2], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
		TIFFYCbCrtoRGB(img->ycbcr, in[vertical_subsample + 3], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
	}

	if (width_ != width) {
		uint32 Cb = in[8];
		uint32 Cr = in[9];
		for (unsigned int i = 0; i < (width % 4); ++i) {
			uint32 r, g, b;
			TIFFYCbCrtoRGB(img->ycbcr, in[vertical_subsample + i], Cb, Cr, &r, &g, &b);
			*out++ = b;
			*out++ = g;
			*out++ = r;
		}
	}
}

void libtiff_conv_ycbcr44_8bit(TiffImage const *img, unsigned char *out, unsigned int width, unsigned char *in) {
	DUMBASS_CHECK(!!img->ycbcr);
	DUMBASS_CHECK(plcl::RenderingData::BitsPerPixel(img->pixfmt) == 24);

	unsigned int const vertical_subsample = img->vertical_subsample * 4;

	unsigned int const width_ = (width % 4) ? (width - (width % 4)) : width;

	for (unsigned int x = 0; x < width_; x += 4, in += 18) {
		uint32 Cb = in[16];
		uint32 Cr = in[17];
		uint32 r, g, b;
		TIFFYCbCrtoRGB(img->ycbcr, in[vertical_subsample], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
		TIFFYCbCrtoRGB(img->ycbcr, in[vertical_subsample + 1], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
		TIFFYCbCrtoRGB(img->ycbcr, in[vertical_subsample + 2], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
		TIFFYCbCrtoRGB(img->ycbcr, in[vertical_subsample + 3], Cb, Cr, &r, &g, &b);
		*out++ = b;
		*out++ = g;
		*out++ = r;
	}

	if (width_ != width) {
		uint32 Cb = in[16];
		uint32 Cr = in[17];
		for (unsigned int i = 0; i < (width % 4); ++i) {
			uint32 r, g, b;
			TIFFYCbCrtoRGB(img->ycbcr, in[vertical_subsample + i], Cb, Cr, &r, &g, &b);
			*out++ = b;
			*out++ = g;
			*out++ = r;
		}
	}
}

inline void libtiff_image_init(TiffImage &img, TiffStuff &stuff) {
	switch (img.photometric) {
		case PHOTOMETRIC_PALETTE:
			/* Convert 16-bit colormap to 8-bit (unless it looks like an old-style 8-bit colormap). */
			if (libtiff_check_colormap(img) == 16)
				libtiff_shrink_colormap(img);
			//else TIFFWarningExt(img->tif->tif_clientdata, TIFFFileName(img->tif), "Assuming 8-bit colormap");
			// /* Use mapping table and colormap to construct unpacking tables for samples < 8 bits. */
			// if (img->bitspersample <= 8 && !makecmap(img))
			stuff.palettecolormap = libtiff_make_palettecolormap(img);
			
			img.pixfmt = PLCL_PIXEL_FORMAT_BGRA_32;
			switch (img.bitspersample) {
				case 1:
					img.libtiff_conv = &libtiff_conv_palettecolormap<1>;
					break;
				case 2:
					img.libtiff_conv = &libtiff_conv_palettecolormap<2>;
					break;
				case 4:
					img.libtiff_conv = &libtiff_conv_palettecolormap<4>;
					break;
				case 8:
					img.libtiff_conv = &libtiff_conv_palettecolormap<8>;
					break;
				case 16:
					img.libtiff_conv = &libtiff_conv_palettecolormap<16>;
					break;
			}
			
			break;
		case PHOTOMETRIC_RGB:
			img.pixfmt = PLCL_PIXEL_FORMAT_BGRA_32;
			if (img.bitspersample == 16)
				img.bitdepth16_8 = stuff.GetBitDepth16To8Map();
			if (img.alpha == TiffImage::ALPHA_UNASSOCIATED) {
				img.ualpha_aalpha = stuff.GetUAlphaToAAlphaMap();
			} else if (img.alpha == TiffImage::ALPHA_UNSPECIFIED) {
				img.pixfmt = PLCL_PIXEL_FORMAT_BGR_24;
			}
			
			switch (img.bitspersample) {
				case 8:
					if (img.alpha == TiffImage::ALPHA_ASSOCIATED)
						img.libtiff_conv = &libtiff_conv_rgbaa_8bit;
					else if (img.alpha == TiffImage::ALPHA_UNASSOCIATED)
						img.libtiff_conv = &libtiff_conv_rgbua_8bit;
					else
						img.libtiff_conv = &libtiff_conv_rgb_8bit;
					break;
				case 16:
					if (img.alpha == TiffImage::ALPHA_ASSOCIATED)
						img.libtiff_conv = &libtiff_conv_rgbaa_16bit;
					else if (img.alpha == TiffImage::ALPHA_UNASSOCIATED)
						img.libtiff_conv = &libtiff_conv_rgbua_16bit;
					else
						img.libtiff_conv = &libtiff_conv_rgb_16bit;
					break;
			}

			break;
		case PHOTOMETRIC_YCBCR:
			img.ycbcr = stuff.GetYCbCrToRGBConversion();
			img.pixfmt = PLCL_PIXEL_FORMAT_BGR_24;

			{
			uint16 horizontal_subsampling_, vertical_subsampling_;
			TIFFGetFieldDefaulted(img.tif, TIFFTAG_YCBCRSUBSAMPLING, &horizontal_subsampling_, &vertical_subsampling_);
			unsigned int horizontal_subsampling(horizontal_subsampling_), vertical_subsampling(vertical_subsampling_);
			switch ((horizontal_subsampling << 4) | vertical_subsampling) {
				case 0x11:
					img.libtiff_conv = &libtiff_conv_ycbcr11_8bit;
					break;
				case 0x12:
					img.libtiff_conv = &libtiff_conv_ycbcr12_8bit;
					break;
				case 0x21:
					img.libtiff_conv = &libtiff_conv_ycbcr21_8bit;
					break;
				case 0x22:
					img.libtiff_conv = &libtiff_conv_ycbcr22_8bit;
					break;
				case 0x41:
					img.libtiff_conv = &libtiff_conv_ycbcr41_8bit;
					break;
				case 0x42:
					img.libtiff_conv = &libtiff_conv_ycbcr42_8bit;
					break;
				case 0x44:
					img.libtiff_conv = &libtiff_conv_ycbcr44_8bit;
					break;
			}
			}

			break;
		case PHOTOMETRIC_CIELAB:
			img.cielab = stuff.GetCIELabToRGBConversion();
			img.pixfmt = PLCL_PIXEL_FORMAT_BGR_24;

			switch (img.bitspersample) {
				case 8:
					img.libtiff_conv = &libtiff_conv_cielab_8bit;
					break;
				case 16:
					img.libtiff_conv = &libtiff_conv_cielab_16bit;
					break;
			}

			break;
		case PHOTOMETRIC_SEPARATED:
			img.pixfmt = PLCL_PIXEL_FORMAT_BGR_24;

			switch (img.bitspersample) {
				case 8:
					img.libtiff_conv = &libtiff_conv_cmyk_8bit;
					break;
				case 16:
					img.libtiff_conv = &libtiff_conv_cmyk_16bit;
					break;
			}

			break;
		case PHOTOMETRIC_MINISBLACK:
		case PHOTOMETRIC_MINISWHITE:
			libtiff_init_graymap(img);
			if (img.bitspersample < 8) {
				stuff.bwmap = libtiff_make_bwmap(img);
			}

			img.pixfmt = PLCL_PIXEL_FORMAT_GRAY_8;
			switch (img.bitspersample) {
				case 1:
					img.libtiff_conv = &libtiff_conv_bwmap<1>;
					break;
				case 2:
					img.libtiff_conv = &libtiff_conv_bwmap<2>;
					break;
				case 4:
					img.libtiff_conv = &libtiff_conv_bwmap<4>;
					break;
				case 8:
					if ((img.alpha != TiffImage::ALPHA_UNSPECIFIED) && (img.samplesperpixel == 2)) {
						img.libtiff_conv = &libtiff_conv_gray_alpha;
						img.pixfmt = PLCL_PIXEL_FORMAT_BGRA_32;
					} else
						img.libtiff_conv = &libtiff_conv_gray;
					break;
				case 16:
					img.libtiff_conv = &libtiff_conv_gray_16;
					break;
			}

			break;
	}
}

inline void libtiff_image_init_separate(TiffImage &img, TiffStuff &stuff) {
	switch (img.photometric) {
		case PHOTOMETRIC_RGB:
			img.pixfmt = PLCL_PIXEL_FORMAT_BGRA_32;
			if (img.bitspersample == 16)
				img.bitdepth16_8 = stuff.GetBitDepth16To8Map();
			if (img.alpha == TiffImage::ALPHA_UNASSOCIATED) {
				img.ualpha_aalpha = stuff.GetUAlphaToAAlphaMap();
			} else if (img.alpha == TiffImage::ALPHA_UNSPECIFIED) {
				img.pixfmt = PLCL_PIXEL_FORMAT_BGR_24;
			}

			switch (img.bitspersample) {
				case 8:
					if (img.alpha == TiffImage::ALPHA_ASSOCIATED)
						img.libtiff_conv_separate = &libtiff_conv_separate_rgbaa_8bit;
					else if (img.alpha == TiffImage::ALPHA_UNASSOCIATED)
						img.libtiff_conv_separate = &libtiff_conv_separate_rgbua_8bit;
					else
						img.libtiff_conv_separate = &libtiff_conv_separate_rgb_8bit;
					break;
				case 16:
					if (img.alpha == TiffImage::ALPHA_ASSOCIATED)
						img.libtiff_conv_separate = &libtiff_conv_separate_rgbaa_16bit;
					else if (img.alpha == TiffImage::ALPHA_UNASSOCIATED)
						img.libtiff_conv_separate = &libtiff_conv_separate_rgbua_16bit;
					else
						img.libtiff_conv_separate = &libtiff_conv_separate_rgb_16bit;
					break;
			}

			break;
		case PHOTOMETRIC_YCBCR:
			img.ycbcr = stuff.GetYCbCrToRGBConversion();
			break;
		/*case PHOTOMETRIC_CIELAB:
			img.cielab = stuff.GetCIELabToRGBConversion();
			break;*/
		case PHOTOMETRIC_SEPARATED:
			img.alpha = TiffImage::ALPHA_ASSOCIATED; // fake for ReadSeparate - read 4 planes
			img.pixfmt = PLCL_PIXEL_FORMAT_BGR_24;

			switch (img.bitspersample) {
				case 8:
					img.libtiff_conv_separate = &libtiff_conv_separate_cmyk_8bit;
					break;
				case 16:
					img.libtiff_conv_separate = &libtiff_conv_separate_cmyk_16bit;
					break;
			}
			break;
	}
}

inline bool libtiff_is_CCITT_compression(TIFF* tif) {
	uint16 compress(COMPRESSION_NONE);
	TIFFGetField(tif, TIFFTAG_COMPRESSION, &compress);
	return ((compress == COMPRESSION_CCITTFAX3)
		|| (compress == COMPRESSION_CCITTFAX4)
		|| (compress == COMPRESSION_CCITTRLE)
		|| (compress == COMPRESSION_CCITTRLEW));
}

inline int libtiff_get_orientation(TIFF* tif) {
	uint16 orientation;
	TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &orientation);
	switch (orientation) {
		case ORIENTATION_TOPLEFT:
		case ORIENTATION_LEFTTOP:
			return TiffImage::FLIP_VERTICALLY;
		case ORIENTATION_TOPRIGHT:
		case ORIENTATION_RIGHTTOP:
			return TiffImage::FLIP_HORIZONTALLY | TiffImage::FLIP_VERTICALLY;
		case ORIENTATION_BOTRIGHT:
		case ORIENTATION_RIGHTBOT:
			return TiffImage::FLIP_HORIZONTALLY;
		case ORIENTATION_BOTLEFT:
		case ORIENTATION_LEFTBOT:
			return 0;
		default:	/* NOTREACHED */
			return 0;
	}
}

TiffImage TiffImage::CreateTiffImage(TiffStuff &tiff_stuff) {
	TIFF *tif = tiff_stuff.tif;
	TiffImage img(tif);

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &img.width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &img.height);
	if ((img.width < 1) || (img.height < 1)) {
		tiff_exception::throw_formatted(tiff_exception(),
			"TiffImage(): invalid or corrupted tiff header: width = %u, height = %u",
			img.width, img.height);
	}

	TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &img.bitspersample);
	switch (img.bitspersample) {
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
			break;
		default:
			tiff_exception::throw_formatted(tiff_exception(),
				"TiffImage(): unsupported format: bitspersample = %u",
				(unsigned long)img.bitspersample);
	}

	TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &img.samplesperpixel);
	uint16 extrasamples(0);
	uint16* sampleinfo(0);
	TIFFGetFieldDefaulted(tif, TIFFTAG_EXTRASAMPLES, &extrasamples, &sampleinfo);
	if ((extrasamples >= 1) && (sampleinfo)) {
		switch (*sampleinfo) {
			case EXTRASAMPLE_UNSPECIFIED:   /* Workaround for some images without */
				if (img.samplesperpixel > 3)  /* correct info about alpha channel */
					img.alpha = TiffImage::ALPHA_ASSOCIATED;
				break;
			case EXTRASAMPLE_ASSOCALPHA:    /* data is pre-multiplied */
				img.alpha = TiffImage::ALPHA_ASSOCIATED;
				break;
			case EXTRASAMPLE_UNASSALPHA:    /* data is not pre-multiplied */
				img.alpha = TiffImage::ALPHA_UNASSOCIATED;
				break;
		}
	}

	uint16 colorchannels = img.samplesperpixel - extrasamples;

	uint16 compression;
	TIFFGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &compression);
	uint16 planar_config(0);
	TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &planar_config);
	if (!TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &img.photometric)) {
		switch (colorchannels) {
			case 1:
				if (libtiff_is_CCITT_compression(tif))
					img.photometric = PHOTOMETRIC_MINISWHITE;
				else
					img.photometric = PHOTOMETRIC_MINISBLACK;
				break;
			case 3:
				img.photometric = PHOTOMETRIC_RGB;
				break;
			default:
				throw tiff_exception("TiffImage(): missing required tag: PhotometricInterpretation");
		}
	}
	if ((img.photometric == PHOTOMETRIC_RGB)
		&& (img.samplesperpixel == 4)
		&& (extrasamples == 0)) {
		img.alpha = TiffImage::ALPHA_ASSOCIATED;
		extrasamples = 1;
	}

	switch (img.photometric) {
		case PHOTOMETRIC_PALETTE:
			if (planar_config == PLANARCONFIG_SEPARATE) {
				throw tiff_exception("TiffImage(): colormapped images with separated planes are not supported");
			}
			{
				uint16 *redcolormap, *greencolormap, *bluecolormap;
				if (!TIFFGetField(tif, TIFFTAG_COLORMAP, &redcolormap, &greencolormap, &bluecolormap))
					throw tiff_exception("TiffImage(): missing required tag: ColorMap");

				// directory values used only once, why create copy ???
				img.redcolormap = redcolormap;
				img.greencolormap = greencolormap;
				img.bluecolormap = bluecolormap;
				///* copy the colormaps so we can modify them */
				//size_t n_color = (1L << img.bitspersample);
				//img.redcolormap = (uint16*)colormap_auxiliary_buf.Alloc(n_color * 3 * sizeof(uint16));
				//img.greencolormap = img.redcolormap + n_color;
				//img.bluecolormap = img.greencolormap + n_color;

				//memcpy(img.redcolormap, redcolormap, n_color * sizeof(uint16));
				//memcpy(img.greencolormap, greencolormap, n_color * sizeof(uint16));
				//memcpy(img.bluecolormap, bluecolormap, n_color * sizeof(uint16));
			}
			/* fall thru... */
		case PHOTOMETRIC_MINISWHITE:
		case PHOTOMETRIC_MINISBLACK:
			if ((planar_config == PLANARCONFIG_CONTIG) // PLANARCONFIG_SEPARATE greyscale
				&& (img.samplesperpixel != 1)
				&& (img.bitspersample < 8)) {
				tiff_exception::throw_formatted(tiff_exception(),
					"TiffRead(): unsupported single image plane format: bitspersample = %u, samplesperpixel = %u, photometric = %u",
					(unsigned long)img.bitspersample, (unsigned long)img.samplesperpixel, (unsigned long)img.photometric);
			}
			break;
		case PHOTOMETRIC_YCBCR:
			/* It would probably be nice to have a reality check here. */
			if (planar_config == PLANARCONFIG_CONTIG) {
				/* can rely on libjpeg to convert to RGB */
				/* XXX should restore current state on exit */
				switch (compression) {
					case COMPRESSION_JPEG:
						/*
						 * TODO: when complete tests verify complete desubsampling
						 * and YCbCr handling, remove use of TIFFTAG_JPEGCOLORMODE in
						 * favor of tif_getimage.c native handling
						 */
						TIFFSetField(tif, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);
						img.photometric = PHOTOMETRIC_RGB;
						break;
					default:
						/* do nothing */
						break;
				}
			}
			/*
			 * TODO: if at all meaningful and useful, make more complete
			 * support check here, or better still, refactor to let supporting
			 * code decide whether there is support and what meaningfull
			 * error to return
			 */
			if (PHOTOMETRIC_YCBCR == img.photometric) {
				if (!((img.bitspersample == 8) && (img.samplesperpixel == 3))) {
					tiff_exception::throw_formatted(tiff_exception(),
						"TiffRead(): unsupported YCbCr image format: bitspersample = %u, samplesperpixel = %u",
						(unsigned long)img.bitspersample, (unsigned long)img.samplesperpixel);
				}
			}
			break;
		case PHOTOMETRIC_RGB:
			if (colorchannels < 3) {
				tiff_exception::throw_formatted(tiff_exception(),
					"TiffRead(): unsupported RGB image format: bitspersample = %u, samplesperpixel = %u, colorchannels = %u",
					(unsigned long)img.bitspersample, (unsigned long)img.samplesperpixel, (unsigned long)colorchannels);
			}
			if ((img.samplesperpixel < 4) && (img.alpha != TiffImage::ALPHA_UNSPECIFIED)) {
				tiff_exception::throw_formatted(tiff_exception(),
					"TiffRead(): unsupported RGB image format with alpha channel: bitspersample = %u, samplesperpixel = %u, colorchannels = %u",
					(unsigned long)img.bitspersample, (unsigned long)img.samplesperpixel, (unsigned long)colorchannels);
			}
			break;
		case PHOTOMETRIC_SEPARATED:
			if (img.samplesperpixel < 4) {
				tiff_exception::throw_formatted(tiff_exception(),
					"TiffRead(): unsupported separated image format: samplesperpixel = %u",
					(unsigned long)img.samplesperpixel);
			}
			{
				uint16 inkset;
				TIFFGetFieldDefaulted(tif, TIFFTAG_INKSET, &inkset);
				if (inkset != INKSET_CMYK) {
					tiff_exception::throw_formatted(tiff_exception(),
						"TiffRead(): unsupported separated image format: inkset = %u",
						(unsigned long)inkset);
				}
			}
			break;
		case PHOTOMETRIC_LOGL:
			if (compression != COMPRESSION_SGILOG) {
				tiff_exception::throw_formatted(tiff_exception(),
					"TiffRead(): unsupported compression type(%u) for LogL data(support only SGILOG)",
					(unsigned long)compression);
			}
			TIFFSetField(tif, TIFFTAG_SGILOGDATAFMT, SGILOGDATAFMT_8BIT);
			img.photometric = PHOTOMETRIC_MINISBLACK;	/* little white lie */
			img.bitspersample = 8;
			break;
		case PHOTOMETRIC_LOGLUV:
			if ((compression != COMPRESSION_SGILOG) && (compression != COMPRESSION_SGILOG24)) {
				tiff_exception::throw_formatted(tiff_exception(),
					"TiffRead(): unsupported compression type(%u) for LogLuv data(support only SGILOG or SGILOG24)",
					(unsigned long)compression);
			}
			if (planar_config != PLANARCONFIG_CONTIG)
				throw tiff_exception("TiffRead(): unsupported PlanarConfiguration for LogLuv data(support only single image plane)");
			TIFFSetField(tif, TIFFTAG_SGILOGDATAFMT, SGILOGDATAFMT_8BIT);
			img.photometric = PHOTOMETRIC_RGB;		/* little white lie */
			img.bitspersample = 8;
			break;
		case PHOTOMETRIC_CIELAB:
			if (planar_config == PLANARCONFIG_SEPARATE) {
				throw tiff_exception("TiffImage(): CIE L*a*b images with separated planes are not supported");
			}
			if ((img.samplesperpixel < 3) || !((img.bitspersample == 8) || (img.bitspersample == 16))) {
				tiff_exception::throw_formatted(tiff_exception(),
					"TiffRead(): unsupported CIE L*a*b format: bitspersample = %u, samplesperpixel = %u, colorchannels = %u",
					(unsigned long)img.bitspersample, (unsigned long)img.samplesperpixel, (unsigned long)colorchannels);
			}
			break;
		default:
			tiff_exception::throw_formatted(tiff_exception(),
				"TiffRead(): unsupported format: bitspersample = %u, samplesperpixel = %u, photometric = %u",
				(unsigned long)img.bitspersample, (unsigned long)img.samplesperpixel, (unsigned long)img.photometric);
	}

	img.planar_separate = ((planar_config == PLANARCONFIG_SEPARATE) && (img.samplesperpixel > 1));
	img.tiled = (TIFFIsTiled(tif) != 0);
	img.flip = libtiff_get_orientation(tif);

	if (img.planar_separate) {
		libtiff_image_init_separate(img, tiff_stuff);

		if (!img.libtiff_conv_separate) {
			tiff_exception::throw_formatted(tiff_exception(),
				"TiffRead(): unimplemented planar separated format: bitspersample = %u, samplesperpixel = %u, photometric = %u",
				(unsigned long)img.bitspersample, (unsigned long)img.samplesperpixel, (unsigned long)img.photometric);
		}
	} else {
		libtiff_image_init(img, tiff_stuff);

		if (!img.libtiff_conv) {
			tiff_exception::throw_formatted(tiff_exception(),
				"TiffRead(): unimplemented single image plane format: bitspersample = %u, samplesperpixel = %u, photometric = %u",
				(unsigned long)img.bitspersample, (unsigned long)img.samplesperpixel, (unsigned long)img.photometric);
		}
	}

	return img;
}
