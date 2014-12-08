// Copyright (C) 2012-2013 Yuri Agafonov
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

#include <stdexcept>

#include "dumbassert.h"
#include "trace.h"

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOGDICAPMASKS
#define NOMETAFILE
#define NOMINMAX
#define NORASTEROPS
#define NOSCROLL
#define NOSOUND
#define NOVIRTUALKEYCODES
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NOSHOWWINDOW
#define NOCLIPBOARD
#define NOCOLOR
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOCTLMGR
#define NODRAWTEXT
#define NOMB
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOCRYPT
#define NOSERVICE
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NOIME
#include <windows.h>

namespace cpcl {

static void __stdcall _DefaultAssertHandler(char const *s, char const *file, unsigned int line) {
	cpcl::Trace(CPCL_TRACE_LEVEL_ERROR, "%s at %s:%d epic fail, process terminates", s, file, line);
	
	HANDLE hProcess = ::GetCurrentProcess();
	::TerminateProcess(hProcess, (UINT)-1);
	::WaitForSingleObject(hProcess, INFINITE);
}

static AssertHandlerPtr ASSERT_HANDLER = _DefaultAssertHandler;

void SetAssertHandler(AssertHandlerPtr v) {
	ASSERT_HANDLER = v;
}

void dumbassert(char const *s, char const *file, unsigned int line) {
	if (ASSERT_HANDLER)
		ASSERT_HANDLER(s, file, line);
	else
		throw std::runtime_error(s);
}

} // namespace cpcl
