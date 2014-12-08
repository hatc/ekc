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

#include "png_stuff.h"

#include <memory>

#include "png_plugin.h"
#include "png_doc.h"

#include <trace.h>

#include "signature_info_helper.hpp"

static unsigned char const png_signature[] = { 137, 80, 78, 71, 13, 10, 26, 10 };
PngPlugin::PngPlugin() : plcl::Plugin(L"PNG", L".png") {
	plcl::Plugin::SignatureInfo const signatures[] = {
		SIGNATURE_INFO(png_signature)
	};
	SetSignatures(signatures, arraysize(signatures));
}
PngPlugin::~PngPlugin()
{}

bool PngPlugin::LoadDoc(cpcl::IOStream *input, plcl::Doc **r) {
	cpcl::IOStream *input_ = input->Clone();
	if (!input_) {
		cpcl::Error(cpcl::StringPieceFromLiteral("PngPlugin::Load(): IOStream::Clone() fails"));
		return false;
	}
	std::auto_ptr<cpcl::IOStream> input_guard(input_);
	
	if (!CheckHeader(input_guard.get()))
		return false;

	PngStuff png_stuff(input_guard.get());
	png_structp png_ptr = png_stuff.png_ptr;
	png_infop info_ptr = png_stuff.info_ptr;

	/* Set error handling if you are using the setjmp/longjmp method (this is
	* the normal method of doing things with libpng). REQUIRED unless you
	* set up your own error handlers in the png_create_read_struct() earlier. */
	if (setjmp(png_jmpbuf(png_ptr))) {
		return false;
	}

	// because we have already read the signature...
	png_set_sig_bytes(png_ptr, arraysize(png_signature));

	/* The call to png_read_info() gives us all of the information from the
  * PNG file before the first IDAT (image data chunk) */
	png_read_info(png_ptr, info_ptr);

	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	/* read the IHDR chunk. */
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		&interlace_type, NULL, NULL);

	std::auto_ptr<PngDoc> doc(new PngDoc(width, height, input_guard/*seek pointer in any case set at 0 then Page::Render*/));
	if (r)
		*r = doc.release();

	return true;
}
