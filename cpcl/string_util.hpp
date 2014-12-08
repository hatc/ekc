// string_util.hpp
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
#pragma once

#ifndef __CPCL_STRING_UTIL_HPP
#define __CPCL_STRING_UTIL_HPP

#ifdef _MSC_VER
#include "string_util_win.hpp"
#else
#include "string_util_posix.hpp"
#endif

#include "string_piece.hpp"

/*string_util.hpp content:
 striASCIIequal
 StringCopy
 StringFormat

ConvertXXtoYY and
TryConvertXXtoYY - move to string_convert.h*/

namespace cpcl {

/* 
 * copy string
 * returns number of character copied, exclude NULL character                               
 * StringCopy always append NULL character to output string, if it's size > 0
 */
/*CopyString or StringCopy ???
probably, can interpret as extension method for string - i.e. string.copy,
so name StringCopy seems as more preferable*/
template<class Char>
inline size_t StringCopy(Char *r, size_t r_size, BasicStringPiece<Char> const &s) {
	if (!((r) && r_size > 0))
		return 0;

#if (_MSC_VER >= 1500)
	size_t r_capacity = r_size;
	r_size = (std::min)(r_size - 1, s.size());
	std::char_traits<Char>::_Copy_s(r, r_capacity, s.data(), r_size);
#else
	r_size = (std::min)(r_size - 1, s.size());
	std::char_traits<Char>::copy(r, s.data(), r_size);
#endif
	r[r_size] = 0;
	return r_size;
}
template<class Char>
inline size_t StringCopy(Char *r, size_t r_size, Char const *s) {
	return StringCopy(r, r_size, BasicStringPiece<Char>(s));
}
template<class Char, size_t N>
inline size_t StringCopyFromLiteral(Char *r, size_t r_size, Char const (&s)[N]) {
	return StringCopy(r, r_size, BasicStringPiece<Char>(s, N - 1));
}

template<class Char, size_t K>
inline size_t StringCopy(Char (&r)[K], BasicStringPiece<Char> const &s) {
	return StringCopy(r, K, s);
}
template<class Char, size_t K>
inline size_t StringCopy(Char (&r)[K], Char const *s) {
	return StringCopy(r, K, BasicStringPiece<Char>(s));
}
template<class Char, size_t N, size_t K>
inline size_t StringCopyFromLiteral(Char (&r)[K], Char const (&s)[N]) {
	return StringCopy(r, K, BasicStringPiece<Char>(s, N - 1));
}

template<class Char>
inline size_t StringFormat(Char *r, size_t r_size, Char const *format, ...) {
	va_list arguments;
	va_start(arguments, format);
	size_t n = StringFormatImpl(r, r_size, format, arguments, 0);
	va_end(arguments);
	return n;
}
template<class Char, size_t N>
inline size_t StringFormat(Char (&r)[N], Char const *format, ...) {
	va_list arguments;
	va_start(arguments, format);
	size_t n = StringFormatImpl(r, N, format, arguments, 0);
	va_end(arguments);
	return n;
}

/*std::string StringFormat(char const* format, ...);
std::string& StringFormat(std::string *r, char const* format, ...);*/

template<class Char/*, size_t FormatBufSize = 0x100*/>
inline std::basic_string<Char>& StringFormat(std::basic_string<Char> *r, Char const *format, ...) {
	size_t const FormatBufSize = 0x100;
	Char buf[FormatBufSize];
	va_list arguments;
	va_start(arguments, format);
	/* basic_string::resize: _Ptr = _Mybase::_Alval.allocate(_Newres + 1); 
	   i.e. resize(N) actual allocate N + 1
	r->resize(0x100); Char *buf = &(*r->begin()); */
	r->assign(buf, StringFormatImpl(buf, FormatBufSize, format, arguments, 0));
	va_end(arguments);
	return *r;
}
template<class Char/*, size_t FormatBufSize = 0x100*/>
inline std::basic_string<Char> StringFormat(Char const *format, ...) {
	size_t const FormatBufSize = 0x100;
	Char buf[FormatBufSize];
	va_list arguments;
	va_start(arguments, format);
	std::basic_string<Char> r(buf, StringFormatImpl(buf, FormatBufSize, format, arguments, 0));
	va_end(arguments);
	return r;
}

// bool || size_t StringFind(String where, String what)

} // namespace cpcl

#endif // __CPCL_STRING_UTIL_HPP
