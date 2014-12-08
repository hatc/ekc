// cf_error_helper.hpp
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

#include <formatted_exception.hpp>

class awp_exception : public formatted_exception<awp_exception> {
public:
	// CF_ERROR error_code;
	unsigned char error_code;

	awp_exception(unsigned char c) : formatted_exception<awp_exception>(NULL), error_code(c)
	{}

	virtual char const* what() const {
		char const *s = formatted_exception<awp_exception>::what();
		if (*s)
			return s;
		else
			return "awp_exception";
	}
};

#ifdef THROW_CF_ERROR
#undef THROW_CF_ERROR
#endif
#ifdef THROW_CF_ERROR_
#undef THROW_CF_ERROR_
#endif
#ifdef THROW_CF_ERROR_LOCAL_NAME
#undef THROW_CF_ERROR_LOCAL_NAME
#endif

#define THROW_CF_ERROR_LOCAL_NAME(n) cf##n
#define THROW_CF_ERROR_(Expr, n) CF_ERROR THROW_CF_ERROR_LOCAL_NAME(n) = (Expr);\
	if (THROW_CF_ERROR_LOCAL_NAME(n) > 0x01/*CF_WARNING*/)\
	awp_exception::throw_formatted(awp_exception(THROW_CF_ERROR_LOCAL_NAME(n)), #Expr " fails with code: 0x%02X", THROW_CF_ERROR_LOCAL_NAME(n));\
	else if (0x01/*CF_WARNING*/ == THROW_CF_ERROR_LOCAL_NAME(n))\
	cpcl::Warning(cpcl::StringPieceFromLiteral(#Expr " returns CF_WARNING"))
#define THROW_CF_ERROR(Expr) THROW_CF_ERROR_(Expr, __COUNTER__)
