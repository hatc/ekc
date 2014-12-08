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
#include "named_os_mutex.hpp"

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

NamedMutexOS::NamedMutexOS(wchar_t const *name) : abadoned(false), mutex_os(NULL), ecode(0) {
	mutex_os = ::CreateMutexW(NULL, FALSE, name);
	if (!mutex_os) {
		ecode = ::GetLastError();
		if (ecode == ERROR_ACCESS_DENIED) {
			mutex_os = ::OpenMutexW(SYNCHRONIZE, FALSE, name);
			if (!mutex_os)
				ecode = ::GetLastError();
		}
	}
}

NamedMutexOS::~NamedMutexOS() {
	if (mutex_os)
		::CloseHandle(mutex_os);
}

bool NamedMutexOS::timed_lock_(unsigned long milliseconds) {
	if (!mutex_os) {
		ErrorSystem(ecode, "NamedMutexOS(): CreateMutexW/OpenMutexW fails:");
		throw boost::thread_resource_error(ecode);
	}

	unsigned long const r = ::WaitForSingleObject(mutex_os, milliseconds);
	if (r == WAIT_OBJECT_0) {
		return true;
	} else if (r == WAIT_ABANDONED) {
		return (abadoned = true);
	} else if (r != WAIT_TIMEOUT) {
		throw boost::thread_resource_error(::GetLastError());
	}

	return false;
}

void NamedMutexOS::unlock() {
	if (!mutex_os) {
		ErrorSystem(ecode, "NamedMutexOS(): CreateMutexW/OpenMutexW fails:");
		throw boost::thread_resource_error(ecode);
	}

	::ReleaseMutex(mutex_os);
}

}  // namespace cpcl
