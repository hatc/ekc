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

#include "bmp_plugin.h"
#include "bmp_doc.h"

#include <signature_info_helper.hpp>

// static unsigned char const bmp_signature[] = { 0x42, 0x4D };
static unsigned char const sizeof_bitmapinfoheader[] = { 0x28, 0x00, 0x00, 0x00 };
static unsigned char const sizeof_bitmapv4header[] = { 0x6C, 0x00, 0x00, 0x00 };
static unsigned char const sizeof_bitmapv5header[] = { 0x7C, 0x00, 0x00, 0x00 };
BmpPlugin::BmpPlugin() : plcl::Plugin(L"BMP", L".bmp") {
	plcl::Plugin::SignatureInfo const signatures[] = {
		SIGNATURE_INFO_OFFSET(sizeof_bitmapinfoheader, 14),
		SIGNATURE_INFO_OFFSET(sizeof_bitmapv4header, 14),
		SIGNATURE_INFO_OFFSET(sizeof_bitmapv5header, 14)
	};
	SetSignatures(signatures, arraysize(signatures));
}
BmpPlugin::~BmpPlugin()
{}

bool BmpPlugin::LoadDoc(cpcl::IOStream *input, plcl::Doc **r) {
	cpcl::IOStream *input_ = input->Clone();
	if (!input_) {
		cpcl::Error(cpcl::StringPieceFromLiteral("BmpPlugin::LoadDoc(): IOStream::Clone() fails"));
		return false;
	}
	std::auto_ptr<cpcl::IOStream> input_guard(input_);

	if (!CheckHeader(input_guard.get()))
		return false;
	input_guard->Seek(0, SEEK_SET, 0);

	std::auto_ptr<BmpDoc> doc;
	try {
		doc.reset(BmpDoc::CreateBmpDoc(input_guard));
	} catch (plcl_exception const &e) {
		cpcl::Error(e.what());
		return false;
	}

	if (r)
		*r = doc.release();
	return true;
}
