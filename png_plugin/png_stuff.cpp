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

#include <dumbassert.h>
#include <trace.h>

extern "C" void ThrowStdException() {
	cpcl::Error("[!!!]libpng:: abort()");
	DUMBASS_CHECK_EXPLANATION(false, "libpng abort()");
}

#if defined(_USRDLL) && defined(PNG_CONSOLE_IO_SUPPORTED)
#error undef PNG_CONSOLE_IO_SUPPORTED in pnglibconf.h then build as DLL
#endif

static void libpng_read(png_structp png_ptr, png_bytep data, png_size_t size) {
	cpcl::IOStream *stream = (cpcl::IOStream*)png_get_io_ptr(png_ptr);

	unsigned long readed = stream->Read(data, (unsigned long)size);
	//if (size && (0 == readed))
	if (readed != size)
		png_error(png_ptr, "Read error: invalid or corrupted PNG file");
}

static void libpng_error(png_structp png_ptr, png_const_charp msg) {
	cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
		"libpng:: %s",
		msg);
	longjmp(png_jmpbuf(png_ptr), 1);
}

PngStuff::PngStuff(cpcl::IOStream *stream) : png_ptr(NULL), info_ptr(NULL) {
	/* Create and initialize the png_struct with the desired error handler
	* functions. If you want to use the default stderr and longjump method,
	* you can supply NULL for the last three parameters. We also supply the
	* the compiler header file version, so that we know if the application
	* was compiled with a compatible version of the library. */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL, libpng_error, NULL);
	if (png_ptr) {
	/* Allocate/initialize the memory for image information. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr) {
		/* If you are using replacement read functions, instead of calling
		* png_init_io() here you would call: */
		png_set_read_fn(png_ptr, (void *)stream, libpng_read);
    /* where user_io_ptr is a structure you want available to the callbacks */
	} else {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		png_ptr = NULL;
	}
	}
}

bool PngStuff::operator!() const {
	return (NULL == png_ptr) || (NULL == info_ptr);
}

PngStuff::~PngStuff() {
	if (!!(*this))
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}
