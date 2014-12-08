// jpeg_stuff.h
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
#pragma once

#include <stdio.h>
#include <jpeglib.h>

#include <setjmp.h>

#include <formatted_exception.hpp>
#include <io_stream.h>

class jpeg_exception : public formatted_exception<jpeg_exception> {
public:
	jpeg_exception(char const *s = NULL) : formatted_exception<jpeg_exception>(s) {}

	virtual char const* what() const {
		char const *s = formatted_exception<jpeg_exception>::what();
		if (*s)
			return s;
		else
			return "jpeg_exception";
	}
};

struct JpegInputManager : jpeg_source_mgr {
	JpegInputManager(cpcl::IOStream *input_);

	/// source stream
	cpcl::IOStream *input;
	/// data input buffer
	unsigned char buffer[4096];
};

struct JpegErrorManager : jpeg_error_mgr {
	jmp_buf jexit;
};

struct JpegStuff {
	jpeg_decompress_struct cinfo;
	JpegErrorManager jerr;

	JpegStuff();
	~JpegStuff();
};
