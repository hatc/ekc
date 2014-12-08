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

#include "freeimagelib_doc.h"
#include "freeimagelib_plugin.h"

#include <formatted_exception.hpp>
#include <trace.h>

FreeimagelibPlugin::FreeimagelibPlugin(int freeimagelib_format_, wchar_t const *format_name, wchar_t const *format_extensions)
	: Plugin(format_name, format_extensions, L';'), freeimagelib_format(freeimagelib_format_)
{}
FreeimagelibPlugin::~FreeimagelibPlugin()
{}

static char const* FREEIMAGELIB_FIF_STRINGS[] = {
	"FIF_BMP",
	"FIF_ICO",
	"FIF_JPEG",
	"FIF_JNG",
	"FIF_KOALA",
	"FIF_LBM",
	"FIF_IFF|FIF_LBM",
	"FIF_MNG"
	"FIF_PBM",
	"FIF_PBMRAW",
	"FIF_PCD",
	"FIF_PCX",
	"FIF_PGM",
	"FIF_PGMRAW",
	"FIF_PNG",
	"FIF_PPM",
	"FIF_PPMRAW",
	"FIF_RAS",
	"FIF_TARGA",
	"FIF_TIFF",
	"FIF_WBMP",
	"FIF_PSD",
	"FIF_CUT",
	"FIF_XBM",
	"FIF_XPM",
	"FIF_DDS",
	"FIF_GIF",
	"FIF_HDR",
	"FIF_FAXG3",
	"FIF_SGI",
	"FIF_EXR",
	"FIF_J2K",
	"FIF_JP2",
	"FIF_PFM",
	"FIF_PICT",
	"FIF_RAW",
	"unknown fif"
};
void DLL_CALLCONV
FreeImage_OutputMessageProc(int fif, const char *fmt, ...) {
	size_t fif_ = (size_t)fif;
	if (fif_ >= arraysize(FREEIMAGELIB_FIF_STRINGS))
		fif_ = arraysize(FREEIMAGELIB_FIF_STRINGS) - 1;
	if (!fmt) {
		cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
			"freeimagelib::%s:: unknown error",
			FREEIMAGELIB_FIF_STRINGS[fif_]);
		return;
	}

	char buf[0x200];
	va_list args;
	va_start(args, fmt);
	_vsnprintf_s(buf, _TRUNCATE, fmt, args);
	/*int n = _vsnprintf_s(buf, _TRUNCATE, fmt, args);
	if (n < 0)
		n = arraysize(buf) - 1;
	cpcl::Warning(cpcl::StringPiece(buf, (size_t)n));*/
	cpcl::Trace(CPCL_TRACE_LEVEL_WARNING,
		"freeimagelib::%s:: %s",
			FREEIMAGELIB_FIF_STRINGS[fif_], buf);
}

class freeimagelib_exception : public formatted_exception<freeimagelib_exception> {
public:
	freeimagelib_exception(char const *s = NULL) : formatted_exception<freeimagelib_exception>(s)
	{}

	virtual char const* what() const {
		char const *s = formatted_exception<freeimagelib_exception>::what();
		if (*s)
			return s;
		else
			return "freeimagelib_exception";
	}
};

struct FreeimagelibStuff {
	FreeImageIO io;
	fi_handle io_handle;

	FreeimagelibStuff(cpcl::IOStream *stream);
};

static unsigned freeimagelib_read_fn(void *buffer, unsigned size, unsigned count, fi_handle handle) {
	cpcl::IOStream *stream = (cpcl::IOStream*)handle;
	return (stream->Read(buffer, size * count) / size);
}

static unsigned freeimagelib_write_fn(void *buffer, unsigned size, unsigned count, fi_handle handle) {
	throw freeimagelib_exception("freeimagelib_write unexpected");
}

static int freeimagelib_seek_fn(fi_handle handle, long offset, int origin) {
	cpcl::IOStream *stream = (cpcl::IOStream*)handle;
	return (stream->Seek(offset, (unsigned long)origin, 0)) ? 0 : -1;
}

static long freeimagelib_tell_fn(fi_handle handle) {
	cpcl::IOStream *stream = (cpcl::IOStream*)handle;
	long r = (long)stream->Tell();
	return (*stream) ? r : -1;
}

FreeimagelibStuff::FreeimagelibStuff(cpcl::IOStream *stream) : io_handle(stream) {
	io.read_proc = freeimagelib_read_fn;
	io.write_proc = freeimagelib_write_fn;
	io.seek_proc = freeimagelib_seek_fn;
	io.tell_proc = freeimagelib_tell_fn;
}

bool FreeimagelibPlugin::LoadDoc(cpcl::IOStream *input, plcl::Doc **r) {
	FREE_IMAGE_FORMAT fif = (FREE_IMAGE_FORMAT)freeimagelib_format;
	FreeimagelibStuff stuff(input);
	boost::shared_ptr<FIBITMAP> fibitmap(FreeImage_LoadFromHandle(fif, &stuff.io, stuff.io_handle), FreeImage_Unload);
	if (!fibitmap) {
		cpcl::Error(cpcl::StringPieceFromLiteral("FreeimagelibPlugin::LoadDoc(): unable to load image"));
		return false;
	}
	
	if (FreeImage_GetImageType(fibitmap.get()) != FIT_BITMAP) {
		boost::shared_ptr<FIBITMAP> tmp(FreeImage_ConvertToType(fibitmap.get(), FIT_BITMAP), FreeImage_Unload);
		if (!tmp || (FreeImage_GetImageType(tmp.get()) != FIT_BITMAP)) {
			cpcl::Error(cpcl::StringPieceFromLiteral("FreeimagelibPlugin::LoadDoc(): unable to convert the image to FIT_BITMAP"));
			return false;
		}
		fibitmap = tmp;
	}

	unsigned int const bpp = FreeImage_GetBPP(fibitmap.get());
	if (!(bpp == 24 || bpp == 32)) {
		boost::shared_ptr<FIBITMAP> tmp((bpp > 32) ? FreeImage_ConvertTo32Bits(fibitmap.get()) : FreeImage_ConvertTo24Bits(fibitmap.get()), FreeImage_Unload);
		if (!tmp) {
			cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
				"FreeimagelibPlugin::LoadDoc(): unable to convert image with bpp %u",
				bpp);
		}
		fibitmap = tmp;
	}

	if (r)
		*r = new FreeimagelibDoc(fibitmap);
	return true;
}
