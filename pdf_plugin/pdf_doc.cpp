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

#include "pdf_page.h"
#include "pdf_doc.h"

#include <trace.h>

PdfDoc::PdfDoc(unsigned int page_count_) : plcl::Doc(page_count_), owner_pw(0), user_pw(0)
{}
PdfDoc::~PdfDoc() {
	owner_pw = user_pw = 0; // not owner
}

bool PdfDoc::GetPage(unsigned int page_num, plcl::Page **r) {
	if (page_num >= page_count)
		return false;

	Page *page = doc->getCatalog()->getPage(page_num + 1); // page ownership not transfer
	if (!page || (page->isOk() != gTrue)) {
		cpcl::Trace(CPCL_TRACE_LEVEL_ERROR, "PdfDoc::GetPage(%d): unable to get page from catalog", page_num);
		return false;
	}

	int w = (int)page->getCropWidth(), h = (int)page->getCropHeight();
	if ((w <= 0) || (h <= 0)) {
		cpcl::Trace(CPCL_TRACE_LEVEL_ERROR, "PdfDoc::GetPage(%d): invalid crop size(%d, %d)", page_num, w, h);
		return false;
	}
	if ((page->getRotate() == 90) || (page->getRotate() == 270))
		std::swap(w, h);

	std::auto_ptr<PdfPage> pdf_page(new PdfPage((unsigned int)w, (unsigned int)h, 72, page_num + 1)); // i.e. we assume getCropWidth() returns density-independent (dp) units for device with dpi 72
	pdf_page->doc = doc;

	if (r)
		*r = pdf_page.release();
	return true;
}
