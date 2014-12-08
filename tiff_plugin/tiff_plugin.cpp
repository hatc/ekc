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

#include <memory>

#include "tiff_doc.h"
#include "tiff_plugin.h"

#include <trace.h>

#include "signature_info_helper.hpp"

//#define TIFF_BIGENDIAN       0x4D4D
//#define TIFF_LITTLEENDIAN    0x4949
//#define TIFF_VERSION_CLASSIC 42 // == 0x2A
//#define TIFF_VERSION_BIG     43 // == 0x2B
static unsigned char tiff_version_classic_be[] = { 0x4D, 0x4D, 0x00, 0x2A };
static unsigned char tiff_version_classic_le[] = { 0x49, 0x49, 0x2A, 0x00 };
static unsigned char tiff_version_big_be[] = { 0x4D, 0x4D, 0x00, 0x2B };
static unsigned char tiff_version_big_le[] = { 0x49, 0x49, 0x2B, 0x00 };
TiffPlugin::TiffPlugin() : plcl::Plugin(L"TIFF", L".tiff;.tif", L';') {
	plcl::Plugin::SignatureInfo const signatures[] = {
		SIGNATURE_INFO(tiff_version_classic_be),
		SIGNATURE_INFO(tiff_version_classic_le),
		SIGNATURE_INFO(tiff_version_big_be),
		SIGNATURE_INFO(tiff_version_big_le)
	};
	SetSignatures(signatures, arraysize(signatures));
}
TiffPlugin::~TiffPlugin()
{}

bool TiffPlugin::LoadDoc(cpcl::IOStream *input, plcl::Doc **r) {
	cpcl::IOStream *input_ = input->Clone();
	if (!input_) {
		cpcl::Error(cpcl::StringPieceFromLiteral("TiffPlugin::Load(): IOStream::Clone() fails"));
		return false;
	}
	std::auto_ptr<cpcl::IOStream> input_guard(input_);
	
	if (!CheckHeader(input_guard.get()))
		return false;

	input_guard->Seek(0, SEEK_SET, NULL);
	TiffStuff tiff_stuff(input_guard.get());
	if (!tiff_stuff)
		throw tiff_exception("TiffPlugin::Load(): unable to create and initialize libtiff handler");

	TIFF *tif = tiff_stuff.tif;

	unsigned int page_count = 1;
	while (TIFFReadDirectory(tif))
		++page_count;

	std::auto_ptr<TiffDoc> doc(new TiffDoc(input_guard, page_count));
	if (r)
		*r = doc.release();

	return true;
}
