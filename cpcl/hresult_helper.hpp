// hresult_helper.hpp
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

class hresult_exception : public formatted_exception<hresult_exception> {
public:
	long ecode;

	hresult_exception(long ecode_) : ecode(ecode_) {}
	virtual char const* what() const {
		char const *s = formatted_exception<hresult_exception>::what();
		if (*s)
			return s;
		else
			return "hresult_exception";
	}
};

#ifdef THROW_FAILED
#undef THROW_FAILED
#endif
#ifdef THROW_FAILED_
#undef THROW_FAILED_
#endif
#ifdef THROW_FAILED_LOCAL_NAME
#undef THROW_FAILED_LOCAL_NAME
#endif

#define THROW_FAILED_LOCAL_NAME(n) hr##n
#define THROW_FAILED_(Expr, n) long THROW_FAILED_LOCAL_NAME(n) = (Expr);\
	if (THROW_FAILED_LOCAL_NAME(n) < 0)\
	hresult_exception::throw_formatted(hresult_exception(THROW_FAILED_LOCAL_NAME(n)), #Expr " fails with code: 0x%08X", THROW_FAILED_LOCAL_NAME(n))
#define THROW_FAILED(Expr) THROW_FAILED_(Expr, __COUNTER__)
