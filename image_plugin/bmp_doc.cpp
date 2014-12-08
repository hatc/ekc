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

#include <algorithm>

#include "bmp_page.hpp"
#include "bmp_doc.h"
#include "bmp_bitfield_reader.hpp"
#include "bmp_rle_reader.h"
#include "bmp_rgb_reader.hpp"

BmpDoc::BmpDoc(std::auto_ptr<cpcl::IOStream> &input_)
	: Doc(1), input(input_)
{}
BmpDoc::~BmpDoc()
{}

namespace bmp {

// GrayColors also needed for BI_RGB compression, so move it to free inline function, probaly use at Plugin::GetDoc, i.e. check pallete after read
inline bool GrayColors(RGBQUAD *colors, size_t n_colors) {
	for (size_t i = 0, transparent = 0; i < n_colors; ++i) {
		if ((!transparent) && (colors[i].rgbReserved)) {
			cpcl::Warning("BmpPlugin::ReadInfo(): invalid pallete colors: alpha channel specified");
			transparent = 1;
		}
		if (!((colors[i].rgbBlue == colors[i].rgbGreen) && (colors[i].rgbGreen == colors[i].rgbRed)))
			return false;
	}
	return true;
}

}

bool BmpDoc::GetPage(unsigned int page_num, plcl::Page **r) {
	std::auto_ptr<plcl::Page> page;

	cpcl::IOStream *input_ = input->Clone();
	if (!input_) {
		cpcl::Error(cpcl::StringPieceFromLiteral("BmpDoc::GetPage(): IOStream::Clone() fails"));
		return false;
	}
	std::auto_ptr<cpcl::IOStream> input_guard(input_);
	bmp::BmpInfo bmp_info(width, height, bpp, pixel_offset, flip_y);

	// reorder if block - for bpp == 16 we also use BmpBitfieldReader, just with predefined mask 0x7FFF
	if (BI_BITFIELDS == compression) {
		DWORD *masks = (DWORD*)colors;
		/*unsigned int r_mask(masks[0]), g_mask(masks[1]), b_mask(masks[2]);
		size_t r_bits(MaskBitCount(r_mask)), g_bits(MaskBitCount(g_mask)), b_bits(MaskBitCount(b_mask));*/
		size_t n_bits[3];
		std::transform(masks, masks + arraysize(n_bits), n_bits, bmp::MaskBitCount);
		if (*std::max_element(n_bits, n_bits + arraysize(n_bits)) > 16) {
			if (16 == bpp) {
				bmp::BmpBitfieldScaleFunction<WORD> settings(bmp_info, masks[0], masks[1], masks[2], n_bits[0], n_bits[1], n_bits[2]);
				page.reset(new BmpPage<bmp::BmpBitfieldReader<bmp::BmpBitfieldScaleFunction<WORD> > >(input_guard, settings));
			} else {
				bmp::BmpBitfieldScaleFunction<DWORD> settings(bmp_info, masks[0], masks[1], masks[2], n_bits[0], n_bits[1], n_bits[2]);
				page.reset(new BmpPage<bmp::BmpBitfieldReader<bmp::BmpBitfieldScaleFunction<DWORD> > >(input_guard, settings));
			}
		} else {
			if (16 == bpp) {
				bmp::BmpBitfieldScaleLookup<WORD> settings(bmp_info, masks[0], masks[1], masks[2], n_bits[0], n_bits[1], n_bits[2]);
				page.reset(new BmpPage<bmp::BmpBitfieldReader<bmp::BmpBitfieldScaleLookup<WORD> > >(input_guard, settings));
			} else {
				bmp::BmpBitfieldScaleLookup<DWORD> settings(bmp_info, masks[0], masks[1], masks[2], n_bits[0], n_bits[1], n_bits[2]);
				page.reset(new BmpPage<bmp::BmpBitfieldReader<bmp::BmpBitfieldScaleLookup<DWORD> > >(input_guard, settings));
			}
		}
	} else if ((BI_RLE4 == compression) || (BI_RLE8 == compression)) {
		bmp::RLEReader::ReaderSettings settings(bmp_info, input_guard.get()/*BufferedReader not own stream*/, compression, colors, n_colors, bmp::GrayColors(colors, n_colors));
		page.reset(new BmpPage<bmp::RLEReader>(input_guard, settings));
	} else {
		if (bpp < 16) {
			bmp::BmpPaletteSettings settings(bmp_info, colors, n_colors, bmp::GrayColors(colors, n_colors));
			page.reset(new BmpPage<bmp::BmpRgbReader<bmp::BmpPaletteSettings> >(input_guard, settings));
		} else if (bpp > 16) {
			bmp::BmpRgbSettings settings(bmp_info);
			page.reset(new BmpPage<bmp::BmpRgbReader<bmp::BmpRgbSettings> >(input_guard, settings));
		} else {
			// The relative intensities of red, green, and blue are represented with five bits for each color component.
			// The value for blue is in the least significant five bits, followed by five bits each for green and red. The most significant bit is not used.
			bmp::BmpBitfieldScaleLookup<WORD> settings(bmp_info, 0x7C00, 0x3E0, 0x1F, 5, 5, 5);
			page.reset(new BmpPage<bmp::BmpBitfieldReader<bmp::BmpBitfieldScaleLookup<WORD> > >(input_guard, settings));
		}
	}

	if (r)
		*r = page.release();
	return true;
}

BmpDoc* BmpDoc::CreateBmpDoc(std::auto_ptr<cpcl::IOStream> &input) {
	std::auto_ptr<BmpDoc> doc(new BmpDoc(input));
	bmp::ReadInfo(doc->input.get(), &doc->flip_y, &doc->pixel_offset,
		&doc->width, &doc->height, &doc->bpp, &doc->compression,
		doc->colors, &doc->n_colors);
	// check additional arguments for specified doc->compression ?
	// bitfields: popcnt
	// rle: ?
	return doc.release();
}

/*BmpXReader {
 struct ReaderSettings {
  ...
 };
}

BmpRleReader {
 struct ReaderSettings {
  BufferedReader
 }
};

BmpPage<Reader> {
 typedef typename Reader::ReaderSettings ReaderSettings;
 ReaderSettings reader_settings;
 BmpPage(bool flip_y, size_t pixels_offset, ReaderSettings const &reader_settings) : reader_settings(reader_settings)
  reader_settings.width, reader_settings.height ???
  reader_settings.pixfmt ???
 {}
}

BmpDoc {
 GetPage() {
  cpcl::IOStream *input_ = input->Clone();
  if (!input_) {
   cpcl::Error(cpcl::StringPieceFromLiteral("BmpDoc::GetPage(): IOStream::Clone() fails"));
   return false;
  }
  std::auto_ptr<cpcl::IOStream> input_guard(input_);
  {
   ...
   if (rle) {
    BmpRleReader::ReaderSettings reader_settings(input_guard.get()); // BufferedReader not own stream
    BmpPage<BmpRleReader>(input_guard, reader_settings);
   }
  }
 }
}
*/
