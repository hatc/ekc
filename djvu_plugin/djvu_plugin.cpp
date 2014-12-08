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

#include <DjVuDocument.h>
#include "byte_iostream.h"
#include "djvu_doc.h"
#include "djvu_plugin.h"
#include "gexception_helper.hpp"

#include <signature_info_helper.hpp>

using DJVU::GP;
using DJVU::ByteStream;
using DJVU::DjVuDocument;

static unsigned char const djvu_signature[] = { 0x41, 0x54, 0x26, 0x54 };
DjvuPlugin::DjvuPlugin() : plcl::Plugin(L"DjVu", L".Djvu ; .djv", L';') {
	plcl::Plugin::SignatureInfo const signatures[] = {
		SIGNATURE_INFO(djvu_signature)
	};
	SetSignatures(signatures, arraysize(signatures));
}
DjvuPlugin::~DjvuPlugin()
{}

bool DjvuPlugin::LoadDoc(cpcl::IOStream *input, plcl::Doc **r) {
	cpcl::IOStream *input_ = input->Clone();
	if (!input_) {
		cpcl::Error(cpcl::StringPieceFromLiteral("DjvuPlugin::LoadDoc(): IOStream::Clone() fails"));
		return false;
	}
	std::auto_ptr<cpcl::IOStream> input_guard(input_);

	if (!CheckHeader(input_guard.get()))
		return false;
	input_guard->Seek(0, SEEK_SET, NULL);

	std::auto_ptr<DjvuDoc> djvu_doc;
	try {
		GP<ByteStream> b = new ByteIOStream(input_guard);
		GP<DjVuDocument> doc = DjVuDocument::create(b);
		doc->wait_for_complete_init();
		if (!(doc->is_init_ok() && (doc->get_pages_num() > 0))) {
			cpcl::Error("DjvuPlugin::LoadDoc(): DjVuDocument initialization failed");
			return false;
		}

		djvu_doc.reset(new DjvuDoc(doc->get_pages_num()));
		djvu_doc->doc = doc;
	} catch (DJVU::GException const &e) {
		TraceGException(e);
		return false;
	}

	if (r)
		*r = djvu_doc.release();
	return true;
}
