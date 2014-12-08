// wrap_except.hpp
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

#include <cstdarg>

#include <boost/function.hpp>

enum SEH_EXCEPTION_TYPE {
	SEH_NONE = 0,
	SEH_STACK_OVERFLOW,
	SEH_ACCESS_VIOLATION,
	SEH_EXCEPTION
};

int WrapExcept(boost::function<void()> *f);
	
class seh_exception {
	char s[0x100];
public:
	seh_exception(char const *s_ = NULL) {
		if (s_) {
			size_t len = strlen(s_);
			if (len >= sizeof(s))
				len = sizeof(s) - 1;
			memcpy(s, s_, len);
			s[len] = 0;
		} else
			*s = 0;
	}
	
	char const* what() const {
		return s;
	}

	static void throw_formatted(char const *format, ...) {
		seh_exception e;
		
		va_list args;
		va_start(args, format);
		if (_vsnprintf_s(e.s, _TRUNCATE, format, args) < 0)
			e.s[sizeof(e.s) - 1] = 0;
		va_end(args);
		
		throw e;
	}
};
