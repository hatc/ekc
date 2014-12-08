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

#include "wrapper_stream.h"
#include "pdf_doc.h"
#include "pdf_plugin.h"

#include <trace.h>

#include <signature_info_helper.hpp>

void GlobalInit();
void GlobalDone();
static unsigned char const pdf_signature[] = { 0x25, 0x50, 0x44, 0x46 };
PdfPlugin::PdfPlugin() : plcl::Plugin(L"PDF", L".pdf") {
	plcl::Plugin::SignatureInfo const signatures[] = {
		SIGNATURE_INFO(pdf_signature)
	};
	SetSignatures(signatures, arraysize(signatures));

	GlobalInit();
}
PdfPlugin::~PdfPlugin() {
	GlobalDone();
}

bool PdfPlugin::LoadDoc(cpcl::IOStream *input, plcl::Doc **r) {
	cpcl::IOStream *input_ = input->Clone();
	if (!input_) {
		cpcl::Error(cpcl::StringPieceFromLiteral("PdfPlugin::LoadDoc(): IOStream::Clone() fails"));
		return false;
	}
	std::tr1::shared_ptr<cpcl::IOStream> input_guard(input_);

	if (!CheckHeader(input_guard.get()))
		return false;
	input_guard->Seek(0, SEEK_SET, NULL);

	GooString *owner_pw(0), *user_pw(0);
	Object obj; obj.initNull();
	std::auto_ptr<BaseStream> input_stream(new WrapperStream(input_guard, 0, gFalse, 0, &obj));
	std::auto_ptr<PDFDoc> doc(new PDFDoc(input_stream.get(), owner_pw, user_pw));
	// if ctor throw, then PDFDoc.dtor i.e. ~PDFDoc() {...} NOT called, so no delete input_stream, and its safe
	input_stream.release();

	if (doc->isOk() != gTrue) {
		cpcl::Error(cpcl::StringPieceFromLiteral("PdfPlugin::LoadDoc(): unable to open pdf document"));
		return false;
	}

	std::auto_ptr<PdfDoc> pdf_doc(new PdfDoc(doc->getNumPages()));
	pdf_doc->doc.reset(doc.release()); // also because class use dynamic resources, its safe to release pointer, but not pass in ctor
	// because ctor may fail for example if memory allocation fails, and in that case resources become abadoned
	// yeah, BUT if pass reference to auto_ptr, then ownership transfered only AFTER memory allocation to object and BEFORE user ctor
	// so, if memory allocation fails, then ownership not yet transfered and auto_ptr<> doc free resources
	// in other case, if user ctor failed, then ownership transfered to auto_ptr<> PdfDoc::doc, and all constructed class fields be deleted, without user dtor(that not called anyway)

	if (r)
		*r = pdf_doc.release();
	return true;
}
