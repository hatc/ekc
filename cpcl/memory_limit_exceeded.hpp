// memory_limit_exceeded.hpp
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

#include "formatted_exception.hpp"

class memory_limit_exceeded : public formatted_exception<memory_limit_exceeded> {
public:
	size_t memory_limit;

	memory_limit_exceeded(size_t memory_limit_) : memory_limit(memory_limit_) {}
	virtual char const* what() const {
		char const *s = formatted_exception<memory_limit_exceeded>::what();
		if (*s)
			return s;
		else
			return "memory_limit_exceeded";
	}
};

//void memory_limit_exceeded::throw_with_format(size_t memory_limit_, char const *format, ...) {
//	memory_limit_exceeded e(memory_limit_);
//
//	va_list args;
//	va_start(args, format);
//	int n = _vsnprintf_s(e.s, _TRUNCATE, format, args);
//	if (n < 0)
//		e.s[sizeof(e.s) / sizeof(*e.s) - 1] = 0;
//	va_end(args);
//
//	throw e;
//}
