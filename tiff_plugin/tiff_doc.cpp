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

#include "tiff_stuff.h"
#include "tiff_page.h"
#include "tiff_doc.h"

#include <trace.h>

TiffDoc::TiffDoc(std::auto_ptr<cpcl::IOStream> &input_, unsigned int page_count_)
	: Doc(page_count_), input(input_)
{}
TiffDoc::~TiffDoc()
{}

bool TiffDoc::GetPage(unsigned int page_num, plcl::Page **r) {
	if (page_num >= page_count)
		return false;

	cpcl::IOStream *input_ = input->Clone();
	if (!input_) {
		cpcl::Error(cpcl::StringPieceFromLiteral("TiffDoc::GetPage(): IOStream::Clone() fails"));
		return false;
	}
	std::auto_ptr<cpcl::IOStream> input_guard(input_);

	input_guard->Seek(0, SEEK_SET, NULL);
	TiffStuff tiff_stuff(input_guard.get());
	if (!tiff_stuff)
		throw tiff_exception("TiffDoc::GetPage(): unable to create and initialize libtiff handler");

	TIFF *tif = tiff_stuff.tif;

	uint16 page_num_ = (uint16)page_num;
	if (!TIFFSetDirectory(tif, page_num_)) {
		tiff_exception::throw_formatted(tiff_exception(),
			"TiffDoc::GetPage(): unable to open page %u",
			(unsigned int)page_num_);
	}

	uint32 width, height;
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
	if ((width < 1) || (height < 1)) {
		tiff_exception::throw_formatted(tiff_exception(),
			"TiffDoc::GetPage(): invalid or corrupted tiff header: width = %u, height = %u",
			(unsigned int)width, (unsigned int)height);
	}

	std::auto_ptr<TiffPage> page(new TiffPage(width, height, input_guard, page_num));
	
	if (r)
		*r = page.release();
	return true;
}
