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

#include <string_util.h>
#include <error_handler.h>
#include <dumbassert.h>
#include <trace.h>

#include "pdf_plugin.h"

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include <iplugin_impl.h>

#include <windows.h>

#include <poppler/Error.h>
#include <poppler/GlobalParams.h>

static void __stdcall NullHandler(char const* /*s*/, unsigned int /*s_length*/) {}
static void __stdcall AssertHandler(char const *s, char const *file, unsigned int line) {
	cpcl::Trace(CPCL_TRACE_LEVEL_ERROR, "%s at %s:%d epic fail", s, file, line);
	
	throw std::runtime_error(s);
}

HMODULE ghModule;
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
		ghModule = hModule;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

static const char* GLOBAL_PARAM_FILE_NAME = "|::|||::|pdf.cfg";
bool load_resource_stream(int res_id, void** data, std::streamsize &data_size) {
	if (NULL == ghModule)
		return false;

	HRSRC hRes = ::FindResourceA(ghModule, MAKEINTRESOURCEA(res_id), "TEXTDATAFILE"); 
	if (NULL == hRes)
		return false;

	if (0 == (data_size = (std::streamsize)::SizeofResource(ghModule, hRes))) 
		return false;

	HGLOBAL hResLoad = ::LoadResource(ghModule, hRes); 
	if (NULL == hResLoad)
		return false;
	
	// Lock the resource in memory.
	return (NULL != (*data = LockResource(hResLoad)));
}
void addParamsWords(std::vector<std::wstring> &v, bool is_error_words);
std::vector<std::string> parseResourceData(void* data, std::streamsize data_size);
struct ConvertCP1251_UTF16 {
	std::wstring operator()(std::string const &v) const {
		return cpcl::ConvertCP1251_UTF16(v);
	}
};
std::vector<std::wstring> loadResourceParamsWords(int resource_id) {
	std::vector<std::wstring> r;
	
	void *data(0); std::streamsize data_size(0);
	if (load_resource_stream(resource_id, &data, data_size)) {
		std::vector<std::string> r_ = parseResourceData(data, data_size);
		r.reserve(r_.size());
		//std::transform(r_.begin(), r_.end(), std::back_inserter(r), cp1251_2_uc());
		std::transform(r_.begin(), r_.end(), std::back_inserter(r), ConvertCP1251_UTF16());
	}

	return r;
}

static cpcl::StringPiece const POPPLER_ERROR_HEADER = cpcl::StringPieceFromLiteral("Poppler Error: ");
static void PopplerErrorHandler(int pos, char *format, va_list args) {
	char buf[0x200];
	int offset = (int)POPPLER_ERROR_HEADER.size();
	if (pos < 0)
		memcpy(buf, POPPLER_ERROR_HEADER.data(), POPPLER_ERROR_HEADER.size());
	else
		offset = _snprintf_s(buf, _TRUNCATE, "Poppler Error (%d): ", pos);
	if (offset < 0) {
		cpcl::Error(cpcl::StringPieceFromLiteral("PopplerErrorHandler(): buf is too small for header"));
		offset = 0;
	}

	int n = _vsnprintf_s(buf + offset, arraysize(buf) - offset, _TRUNCATE, format, args);
	if (n < 0) {
		n = arraysize(buf) - 1;
		if ('\0' == buf[n - 1])
			--n;
	} else
		n += offset;
	cpcl::TraceString(CPCL_TRACE_LEVEL_ERROR, cpcl::StringPiece(buf, static_cast<size_t>(n)));
}

static int globalRefs = 0;
void GlobalInit() {
	if (!globalRefs++) {
		if (!globalParams)
			globalParams = new GlobalParams(GLOBAL_PARAM_FILE_NAME);

		globalParams->setupBaseFonts("");
		globalParams->setBaseDir("");
		{
			/*101           TEXTDATAFILE            "words_win1251.tdt"
			102           TEXTDATAFILE            "words_errors_win1251.tdt"*/
			std::vector<std::wstring> words = loadResourceParamsWords(101);
			addParamsWords(words, false);
			words = loadResourceParamsWords(102);
			addParamsWords(words, true);
		}

    globalParams->setStrokeAdjust(gTrue);
    globalParams->setEnableFreeType("yes");
    globalParams->setAntialias("yes");
    globalParams->setVectorAntialias("yes");

		setErrorFunction(PopplerErrorHandler);
	}
}
void GlobalDone() {
	globalRefs--;
	if (globalRefs==0) {
		delete globalParams; 
		globalParams = NULL;
	}
}

STDAPI CreatePlugin(IPlugin **r) {
	if (!r)
		return E_INVALIDARG;

	ScopedComPtr<IPlugin> plugin_ptr(new IPluginImpl(new PdfPlugin()));
	*r = plugin_ptr.Detach();
	return S_OK;
}

STDAPI _InitTrace(wchar_t const *log_path, cpcl::ErrorHandlerPtr error_handler) {
	cpcl::SetTraceFilePath(log_path);
	cpcl::SetErrorHandler(error_handler);
	return S_OK;
}
