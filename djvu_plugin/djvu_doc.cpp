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

#include <memory>

#include "djvu_page.h"
#include "djvu_doc.h"
#include "gexception_helper.hpp"

using DJVU::GP;
using DJVU::DjVuDocument;

DjvuDoc::DjvuDoc(unsigned int page_count_) : plcl::Doc(page_count_)
{}
DjvuDoc::~DjvuDoc()
{}

bool DjvuDoc::GetPage(unsigned page_num, plcl::Page **r) {
	if (page_num >= page_count)
		return false;

	std::auto_ptr<DjvuPage> page;
	try {
	GP<DjVuImage>	image = doc->get_page(page_num);
	if (!image || (image->wait_for_complete_decode() == false)) {
		cpcl::Trace(CPCL_TRACE_LEVEL_ERROR, "DjvuDoc::GetPage(%d): DjVuImage initialization failed", page_num);
		return false;
	}
	GP<DjVuInfo> info = image->get_info();
	if (!info) {
		cpcl::Trace(CPCL_TRACE_LEVEL_ERROR, "DjvuDoc::GetPage(%d): DjVuInfo unavailable", page_num);
		return false;
	}
	int width_dp(0), height_dp(0), dpi(0);
	if (image->get_rotate() & 1) {
		width_dp = info->height;
		height_dp = info->width;
	} else {
		width_dp = info->width;
		height_dp = info->height;
	}
	if ((width_dp < 1) || (height_dp < 1)) {
		cpcl::Trace(CPCL_TRACE_LEVEL_ERROR, "DjvuDoc::GetPage(%d): DjVuInfo invalid(width_dp = %d, height_dp = %d)",
			page_num, width_dp, height_dp);
		return false;
	}
	if ((dpi = info->dpi) < 1)
		dpi = 300;
	
	if ((image->is_legal_bilevel()
		|| image->is_legal_photo()
		|| image->is_legal_compound()) == 0) {
		cpcl::Trace(CPCL_TRACE_LEVEL_ERROR, "DjvuDoc::GetPage(%d): DjVuImage illegal", page_num);
		return false;
	}

	GP<DjVuText> text;
	GP<DjVuFile> file = doc->get_djvu_file(page_num);
	if (!file) {
		cpcl::Trace(CPCL_TRACE_LEVEL_ERROR, "DjvuDoc::GetPage(%d): DjVuFile unavailable", page_num);
		return false;
	}
	GP<ByteStream> stream = file->get_text();
	if (!!stream) {
		text = DjVuText::create();
		text->decode(stream);
	}

	page.reset(new DjvuPage(width_dp, height_dp, dpi));
	page->image = image;
	page->text = text;
	} catch (DJVU::GException const &e) {
		TraceGException(e);
		return false;
	}

	if (r)
		*r = page.release();
	return true;
}
