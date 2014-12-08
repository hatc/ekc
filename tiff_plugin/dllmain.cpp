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
#include <tchar.h>

#include <error_handler.h>
#include <trace.h>
#include <dumbassert.h>

#include <iplugin_impl.h>
#include "tiff_plugin.h"

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include <windows.h>

//#include <boost/function.hpp>
//
//#define WIN32_LEAN_AND_MEAN
//#define INC_OLE2
//#include <windows.h>
//#include <excpt.h>
//
//// used in render && resize - use boost::function && boost::bind
//int WrapExceptAV(boost::function<void()> *f) {
//	__try {
//		(*f)();
//		return 0;
//	} __except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
//		return 1;
//	}
//}

static void __stdcall NullHandler(char const* /*s*/, unsigned int /*s_length*/) {}
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
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

STDAPI CreatePlugin(IPlugin **r) {
	if (!r)
		return E_INVALIDARG;

	ScopedComPtr<IPlugin> plugin_ptr(new IPluginImpl(new TiffPlugin()));
	*r = plugin_ptr.Detach();
	return S_OK;
}

STDAPI _InitTrace(wchar_t const *log_path, cpcl::ErrorHandlerPtr error_handler) {
	cpcl::SetTraceFilePath(log_path);
	cpcl::SetErrorHandler(error_handler);
	return S_OK;
}
