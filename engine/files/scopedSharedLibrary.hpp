// scopedSharedLibrary.hpp
// Copyright (C) 2011 Yuri Agafonov
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

#ifndef __SCOPED_SHARED_LIBRARY_HPP
#define __SCOPED_SHARED_LIBRARY_HPP

#include "stdafx.h"

#include <windows.h>

struct scopedSharedLibrary {
	HMODULE module_;
	
	DISALLOW_COPY_AND_ASSIGN(scopedSharedLibrary);
public:
	scopedSharedLibrary() : module_(NULL) {}
	explicit scopedSharedLibrary(HMODULE module) : module_(NULL) {}
	explicit scopedSharedLibrary(const wchar_t *v) {
		module_ = ::LoadLibraryW(v);
	}
	~scopedSharedLibrary() {
		if (module_ != NULL)
			::FreeLibrary(module_);
	}

	operator bool() {
		return module_ != NULL;
	}

	bool hasFunction(const char *name) const {
		return ::GetProcAddress(module_, name) != NULL;
	}
	/*template<typename T> T getFunction(const char *name) const {
		return (T)::GetProcAddress(module_, name);
	}*/
	void* getFunction(const char *name) const {
		return ::GetProcAddress(module_, name);
	}
	void reset(HMODULE module) {
		if (module_)
			::FreeLibrary(module_);
		module_ = module;
	}
};

#endif // __SCOPED_SHARED_LIBRARY_HPP
