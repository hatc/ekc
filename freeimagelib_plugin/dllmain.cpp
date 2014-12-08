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
#include <algorithm> // std::min
#include <vector>

#include <com_ptr.hpp>
#include <error_handler.h>
#include <trace.h>
#include <dumbassert.h>

#include "freeimagelib_plugin.h"

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include <windows.h>

#include <iplugin_impl.h>
#include <impl_exception_helper.hpp>

#include <FreeImage.h>

static void __stdcall NullHandler(char const* /*s*/, unsigned int /*s_length*/)
{}
static void __stdcall AssertHandler(char const *s, char const *file, unsigned int line) {
	cpcl::Trace(CPCL_TRACE_LEVEL_ERROR, "%s at %s:%d epic fail", s, file, line);
	
	throw std::runtime_error(s);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		cpcl::SetErrorHandler(NullHandler);
		cpcl::SetAssertHandler(AssertHandler);

		FreeImage_Initialise(TRUE);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		FreeImage_DeInitialise();
		break;
	}
	return TRUE;
}

/* FIF_EXR - ".exr" */
static unsigned char const exr_signature[] = { 0x76, 0x2F, 0x31, 0x01 };
plcl::Plugin* exr_ctor() {
	std::auto_ptr<FreeimagelibPlugin> r(new FreeimagelibPlugin(29, L"EXR", L".exr"));
	plcl::Plugin::SignatureInfo const signatures[] = {
		SIGNATURE_INFO(exr_signature)
	};
	r->CopySignatures(signatures);
	return r.release();
}

/* FIF_GIF - ".gif" */
static unsigned char const gif_version[] = { 0x47, 0x49, 0x46, 0x56, 0x45, 0x52 };
static unsigned char const gif_version_87[] = { 0x47, 0x49, 0x46, 0x38, 0x37, 0x61 };
static unsigned char const gif_version_89[] = { 0x47, 0x49, 0x46, 0x38, 0x39, 0x61 };
plcl::Plugin* gif_ctor() {
	std::auto_ptr<FreeimagelibPlugin> r(new FreeimagelibPlugin(25, L"GIF", L".gif"));
	plcl::Plugin::SignatureInfo const signatures[] = {
		SIGNATURE_INFO(gif_version),
		SIGNATURE_INFO(gif_version_87),
		SIGNATURE_INFO(gif_version_89)
	};
	r->CopySignatures(signatures);
	return r.release();
}

/* FIF_BMP - ".bmp" */
/*offset : 14, */static unsigned char const sizeof_bitmapinfoheader[] = { 0x28, 0x00, 0x00, 0x00 };
/*offset : 14, */static unsigned char const sizeof_bitmapv4header[] = { 0x6C, 0x00, 0x00, 0x00 };
/*offset : 14, */static unsigned char const sizeof_bitmapv5header[] = { 0x7C, 0x00, 0x00, 0x00 };
plcl::Plugin* bmp_ctor() {
	std::auto_ptr<FreeimagelibPlugin> r(new FreeimagelibPlugin(0, L"BMP", L".bmp"));
	plcl::Plugin::SignatureInfo const signatures[] = {
		SIGNATURE_INFO_OFFSET(sizeof_bitmapinfoheader, 14),
		SIGNATURE_INFO_OFFSET(sizeof_bitmapv4header, 14),
		SIGNATURE_INFO_OFFSET(sizeof_bitmapv5header, 14)
	};
	r->CopySignatures(signatures);
	return r.release();
}

/* FIF_PICT - ".pct;.pict;.pic" */
static unsigned char const pict_signature[] = { 0x00, 0x11, 0x02, 0xFF, 0x0C, 0x00 };
plcl::Plugin* pict_ctor() {
	std::auto_ptr<FreeimagelibPlugin> r(new FreeimagelibPlugin(33, L"PICT", L".pct;.pict;.pic"));
	plcl::Plugin::SignatureInfo const signatures[] = {
		SIGNATURE_INFO(pict_signature)
	};
	r->CopySignatures(signatures);
	return r.release();
}

/* FIF_PFM - ".pfm" */
static unsigned char const pfm_0_signature[] = { 0x50, 0x46 };
static unsigned char const pfm_1_signature[] = { 0x50, 0x66 };
plcl::Plugin* pfm_ctor() {
	std::auto_ptr<FreeimagelibPlugin> r(new FreeimagelibPlugin(32, L"PFM", L".pfm"));
	plcl::Plugin::SignatureInfo const signatures[] = {
		SIGNATURE_INFO(pfm_0_signature),
		SIGNATURE_INFO(pfm_1_signature)
	};
	r->CopySignatures(signatures);
	return r.release();
}

/* FIF_PBM|FIF_PGM|FIF_PPM - ".pbm;.pgm;.ppm" */
static unsigned char const pbm_0_signature[] = { 0x50, 0x31 };
static unsigned char const pbm_1_signature[] = { 0x50, 0x34 };
static unsigned char const pgm_0_signature[] = { 0x50, 0x32 };
static unsigned char const pgm_1_signature[] = { 0x50, 0x35 };
static unsigned char const ppm_0_signature[] = { 0x50, 0x33 };
static unsigned char const ppm_1_signature[] = { 0x50, 0x36 };
plcl::Plugin* pnm_ctor() {
	std::auto_ptr<FreeimagelibPlugin> r(new FreeimagelibPlugin(14, L"PNM", L".pbm;.pgm;.ppm"));
	plcl::Plugin::SignatureInfo const signatures[] = {
		SIGNATURE_INFO(pbm_0_signature),
		SIGNATURE_INFO(pbm_1_signature),
		SIGNATURE_INFO(pgm_0_signature),
		SIGNATURE_INFO(pgm_1_signature),
		SIGNATURE_INFO(ppm_0_signature),
		SIGNATURE_INFO(ppm_1_signature)
	};
	r->CopySignatures(signatures);
	return r.release();
}

/* FIF_PSD - ".psd" */
static unsigned char const psd_signature[] = { 0x38, 0x42, 0x50, 0x53 };
plcl::Plugin* psd_ctor() {
	std::auto_ptr<FreeimagelibPlugin> r(new FreeimagelibPlugin(20, L"PSD", L".psd"));
	plcl::Plugin::SignatureInfo const signatures[] = {
		SIGNATURE_INFO(psd_signature)
	};
	r->CopySignatures(signatures);
	return r.release();
}

/* FIF_RAS - ".ras" */
static unsigned char const ras_signature[] = { 0x59, 0xA6, 0x6A, 0x95 };
plcl::Plugin* ras_ctor() {
	std::auto_ptr<FreeimagelibPlugin> r(new FreeimagelibPlugin(16, L"RAS", L".ras;.sun"));
	plcl::Plugin::SignatureInfo const signatures[] = {
		SIGNATURE_INFO(ras_signature)
	};
	r->CopySignatures(signatures);
	return r.release();
}

/* DNG is tiff, but image data is mosaic, so tifflib fails to read it as image */
/* so, currently DNG not supported */
/* FIF_RAW - ".cr2;.crw;.mrw;.orf;.raf;.raw;.rw2;.rwl;.sr2;.srw;.k25;.kc2;.dc2" */
// Canon (CR2), Intel byte order
static unsigned char const canon_cr2_le[] = { 0x49, 0x49, 0x2A, 0x00, 0x10, 0x00, 0x00, 0x00, 0x43, 0x52, 0x02, 0x00 };
// Canon (CR2), Motorola byte order
// static unsigned char const canon_cr2_be[] = { 0x4D, 0x4D, 0x2A, 0x00, 0x10, 0x00, 0x00, 0x00, 0x43, 0x52, 0x02, 0x00 };
static unsigned char const canon_cr2_be[] = { 0x4D, 0x4D, 0x00, 0x2A, 0x00, 0x10, 0x00, 0x00, 0x52, 0x43, 0x00, 0x02};
// Canon (CRW), Intel byte order
static unsigned char const canon_crw_le[] = { 0x49, 0x49, 0x1A, 0x00, 0x00, 0x00, 0x48, 0x45, 0x41, 0x50, 0x43, 0x43, 0x44, 0x52, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// Minolta (MRW)
static unsigned char const minolta_mrw[] = { 0x00, 0x4D, 0x52, 0x4D, 0x00 };
// Olympus (ORF), Intel byte order
static unsigned char const olympus_orf_rs_le[] = { 0x49, 0x49, 0x52, 0x53, 0x08, 0x00, 0x00, 0x00 }; 
static unsigned char const olympus_orf_ro_le[] = { 0x49, 0x49, 0x52, 0x4F, 0x08, 0x00, 0x00, 0x00 }; 
// Olympus (ORF), Motorola byte order
static unsigned char const olympus_orf_ro_be[] = { 0x4D, 0x4D, 0x4F, 0x52, 0x00, 0x00, 0x00, 0x08 }; 
// Fujifilm (RAF)
static unsigned char const fujifilm_raf[] = { 0x46, 0x55, 0x4A, 0x49, 0x46, 0x49, 0x4C, 0x4D, 0x43, 0x43, 0x44, 0x2D, 0x52, 0x41, 0x57, 0x20, 0x30, 0x32, 0x30, 0x31 };
// Panasonic (RW2) or Leica (RWL)
static unsigned char const panasonic_rw2_rwl[] = { 0x49, 0x49, 0x55, 0x00, 0x18, 0x00, 0x00, 0x00, 0x88, 0xE7, 0x74, 0xD8, 0xF8, 0x25, 0x1D, 0x4D, 0x94, 0x7A, 0x6E, 0x77, 0x82, 0x2B, 0x5D, 0x6A };
plcl::Plugin* raw_ctor() {
	// .nef stored in tiff chunks // std::auto_ptr<FreeimagelibPlugin> r(new FreeimagelibPlugin(34, L"RAW", L".cr2;.crw;.mrw;.orf;.raf;.raw;.rw2;.rwl;.sr2;.srw;.k25;.kc2;.dc2;.nef;.nrw"));
	std::auto_ptr<FreeimagelibPlugin> r(new FreeimagelibPlugin(34, L"RAW", L".cr2;.crw;.mrw;.orf;.raf;.raw;.rw2;.rwl;.sr2;.srw;.k25;.kc2;.dc2;.nrw"));
	plcl::Plugin::SignatureInfo const signatures[] = {
		SIGNATURE_INFO(canon_cr2_le),
		SIGNATURE_INFO(canon_cr2_be),
		SIGNATURE_INFO(canon_crw_le),
		SIGNATURE_INFO(minolta_mrw),
		SIGNATURE_INFO(olympus_orf_rs_le),
		SIGNATURE_INFO(olympus_orf_ro_le),
		SIGNATURE_INFO(olympus_orf_ro_be),
		SIGNATURE_INFO(fujifilm_raf),
		SIGNATURE_INFO(panasonic_rw2_rwl)
	};
	r->CopySignatures(signatures);
	return r.release();
}

std::vector<plcl::Plugin*> CreatePlugins() {
	typedef plcl::Plugin* (*Ctor)();
	Ctor const ctors[] = {
		exr_ctor,
		gif_ctor,
		bmp_ctor,
		pict_ctor,
		pfm_ctor,
		pnm_ctor,
		psd_ctor,
		ras_ctor,
		raw_ctor
	};

	std::vector<plcl::Plugin*> r;
	r.reserve(arraysize(ctors));
	for (size_t i = 0; i < arraysize(ctors); ++i)
		r.push_back(ctors[i]());
	return r;
}

STDAPI CreatePlugins(DWORD *count, IPlugin **r) {
	typedef plcl::Plugin* (*Ctor)();
	Ctor const ctors[] = {
		exr_ctor,
		gif_ctor,
		/*bmp_ctor,*/
		pict_ctor,
		pfm_ctor,
		pnm_ctor,
		psd_ctor,
		ras_ctor,
		raw_ctor
	};

	if (!count)
		return E_INVALIDARG;
	if (!r) {
		*count = static_cast<DWORD>(arraysize(ctors));
		return S_OK;
	}

	std::vector<cpcl::ComPtr<IPlugin> > plugins_ptr;
	try {
	plugins_ptr.reserve(arraysize(ctors));
	for (size_t i = 0, n = (std::min)(arraysize(ctors), static_cast<size_t>(*count)); i < n; ++i)
		plugins_ptr.push_back(new IPluginImpl(ctors[i]()));
	} CATCH_EXCEPTION("freeimagelib_plugin::CreatePlugins()")

	for (size_t i = 0, n = plugins_ptr.size(); i < n; ++i)
		r[i] = plugins_ptr[i].Detach();
	
	*count = static_cast<DWORD>(plugins_ptr.size());
	return S_OK;
}

STDAPI _InitTrace(wchar_t const *log_path, cpcl::ErrorHandlerPtr error_handler) {
	cpcl::SetTraceFilePath(log_path);
	cpcl::SetErrorHandler(error_handler);
	return S_OK;
}
