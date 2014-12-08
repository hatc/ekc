// formatted_exception.hpp
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
#include <memory.h>

#include <cstdarg>
#include <exception>

/*template<class T>
class throw_format : public std::exception {
public:
	static void throw_with_format(char const *format, ...) {
		T e;

		va_list args;
		va_start(args, format);
		if (_vsnprintf_s(e.s, _TRUNCATE, format, args) < 0)
			e.s[sizeof(e.s) / sizeof(*e.s) - 1] = 0;
		va_end(args);
		
		throw e;
	}
};

template<class T>
class throw_format_custom : public std::exception {
public:
	static void throw_with_format(T e, char const *format, ...) {
		va_list args;
		va_start(args, format);
		if (_vsnprintf_s(e.s, _TRUNCATE, format, args) < 0)
			e.s[sizeof(e.s) / sizeof(*e.s) - 1] = 0;
		va_end(args);
		
		throw e;
	}
};

template<class T>
class throw_with_format_base : public std::exception {
protected:
	char s[0x100];
public:
	throw_with_format(char const *s_ = NULL) {
		if (!s_)
			*s = 0;
		else {
		size_t len = strlen(s_);
		if (len >= sizeof(s))
			len = sizeof(s) - 1;
		memcpy(s, s_, len);
		s[len] = 0;
		}
	}

	static void throw_with_format(T e, char const *format, ...) {
		va_list args;
		va_start(args, format);
		if (_vsnprintf_s(e.s, _TRUNCATE, format, args) < 0)
			e.s[sizeof(e.s) / sizeof(*e.s) - 1] = 0;
		va_end(args);
		
		throw e;
	}

	virtual char const* what() const {
		return s;
	}
};*/

template<class T>
class formatted_exception : public std::exception {
protected:
	char s[0x100];
public:
	formatted_exception(char const *s_ = NULL) {
		if (s_) {
			size_t len = strlen(s_);
			if (len >= sizeof(s))
				len = sizeof(s) - 1;
			memcpy(s, s_, len);
			s[len] = 0;
		} else
			*s = 0;
	}
	
	virtual char const* what() const {
		return s;
	}

	static void throw_formatted(T e, char const *format, ...) {
		va_list args;
		va_start(args, format);
		if (_vsnprintf_s(e.s, _TRUNCATE, format, args) < 0)
			e.s[sizeof(e.s) - 1] = 0;
		va_end(args);
		
		throw e;
	}
};
