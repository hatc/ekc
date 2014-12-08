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
#include "jpeg_stuff.h"

#include <memory>

#include "jpeg_plugin.h"
#include "jpeg_doc.h"
#include <trace.h>

#include "signature_info_helper.hpp"

// X'FF', SOI, X'FF', APP0, <2 bytes to be skipped>, "JFIF", X'00'.
// 0xFF, M_SOI = 0xD8, 0xFF, M_APP0 = 0xE0
/*M_APP1  = 0xe1,
  M_APP2  = 0xe2,
  M_APP3  = 0xe3,
  M_APP4  = 0xe4,
  M_APP5  = 0xe5,
  M_APP6  = 0xe6,
  M_APP7  = 0xe7,
  M_APP8  = 0xe8,
  M_APP9  = 0xe9,
  M_APP10 = 0xea,
  M_APP11 = 0xeb,
  M_APP12 = 0xec,
  M_APP13 = 0xed,
  M_APP14 = 0xee,
  M_APP15 = 0xef,*/
static unsigned char const jfif_signature[] = { 0xFF, 0xD8, 0xFF, 0xE0 };
static unsigned char const jfif_signature_app1[] = { 0xFF, 0xD8, 0xFF, 0xE1 };
static unsigned char const jfif_signature_app2[] = { 0xFF, 0xD8, 0xFF, 0xE2 };
static unsigned char const jfif_signature_app3[] = { 0xFF, 0xD8, 0xFF, 0xE3 };
static unsigned char const jfif_signature_app4[] = { 0xFF, 0xD8, 0xFF, 0xE4 };
static unsigned char const jfif_signature_app5[] = { 0xFF, 0xD8, 0xFF, 0xE5 };
static unsigned char const jfif_signature_app6[] = { 0xFF, 0xD8, 0xFF, 0xE6 };
static unsigned char const jfif_signature_app7[] = { 0xFF, 0xD8, 0xFF, 0xE7 };
static unsigned char const jfif_signature_app8[] = { 0xFF, 0xD8, 0xFF, 0xE8 };
static unsigned char const jfif_signature_app9[] = { 0xFF, 0xD8, 0xFF, 0xE9 };
static unsigned char const jfif_signature_app10[] = { 0xFF, 0xD8, 0xFF, 0xEA };
static unsigned char const jfif_signature_app11[] = { 0xFF, 0xD8, 0xFF, 0xEB };
static unsigned char const jfif_signature_app12[] = { 0xFF, 0xD8, 0xFF, 0xEC };
static unsigned char const jfif_signature_app13[] = { 0xFF, 0xD8, 0xFF, 0xED };
static unsigned char const jfif_signature_app14[] = { 0xFF, 0xD8, 0xFF, 0xEE };
static unsigned char const jfif_signature_app15[] = { 0xFF, 0xD8, 0xFF, 0xEF };
JpegPlugin::JpegPlugin() : plcl::Plugin(L"JPEG JFIF", L".jpg ; .jpeg", L';') {
	plcl::Plugin::SignatureInfo const signatures[] = {
		SIGNATURE_INFO(jfif_signature),
		SIGNATURE_INFO(jfif_signature_app1),
		SIGNATURE_INFO(jfif_signature_app2),
		SIGNATURE_INFO(jfif_signature_app3),
		SIGNATURE_INFO(jfif_signature_app4),
		SIGNATURE_INFO(jfif_signature_app5),
		SIGNATURE_INFO(jfif_signature_app6),
		SIGNATURE_INFO(jfif_signature_app7),
		SIGNATURE_INFO(jfif_signature_app8),
		SIGNATURE_INFO(jfif_signature_app9),
		SIGNATURE_INFO(jfif_signature_app10),
		SIGNATURE_INFO(jfif_signature_app11),
		SIGNATURE_INFO(jfif_signature_app12),
		SIGNATURE_INFO(jfif_signature_app13),
		SIGNATURE_INFO(jfif_signature_app14),
		SIGNATURE_INFO(jfif_signature_app15)
	};
	SetSignatures(signatures, arraysize(signatures));
}
JpegPlugin::~JpegPlugin()
{}

bool JpegPlugin::LoadDoc(cpcl::IOStream *input, plcl::Doc **r) {
	cpcl::IOStream *input_ = input->Clone();
	if (!input_) {
		cpcl::Error(cpcl::StringPieceFromLiteral("JpegPlugin::LoadDoc(): IOStream::Clone() fails"));
		return false;
	}
	std::auto_ptr<cpcl::IOStream> input_guard(input_);
	
	if (!CheckHeader(input_guard.get()))
		return false;
	input_guard->Seek(0, SEEK_SET, NULL);

	JpegStuff jpeg_stuff;
	j_decompress_ptr cinfo = &jpeg_stuff.cinfo;

	JpegInputManager source_manager(input_guard.get());
	cinfo->src = &source_manager;

	if (setjmp(jpeg_stuff.jerr.jexit)) {
		return false;
	}
	if (jpeg_read_header(cinfo, TRUE) != JPEG_HEADER_OK)
		return false;

	std::auto_ptr<JpegDoc> doc(new JpegDoc(cinfo->image_width, cinfo->image_height, input_guard));
	if (r)
		*r = doc.release();

	return true;
}
