// string_cast.hpp
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

#ifndef __CPCL_STRING_CAST_HPP
#define __CPCL_STRING_CAST_HPP

#include <string_piece.hpp>

#include <boost/lexical_cast.hpp>

namespace cpcl {

template<class T, class CharType>
inline bool TryConvert(BasicStringPiece<CharType, std::char_traits<CharType> > const &s, T *r) {
	try {
		T tmp = boost::lexical_cast<T>(s.as_string());
		if (r)
			*r = tmp;
	} catch(boost::bad_lexical_cast&) {
		return false;
	}
	return true;
}

template<>
inline bool TryConvert<long, char>(StringPiece const &s, long *r) {
	std::string s_ = s.as_string();
	char const *str = s_.c_str();
	char *tail;
	long const n = strtol(str, &tail, 10);

	if ((n == LONG_MAX) || (n == LONG_MIN) || ((n == 0) && (tail == str)))
		return false;
	if (r)
		*r = n;
	return true;
}
template<>
inline bool TryConvert<unsigned long, char>(StringPiece const &s, unsigned long *r) {
	std::string s_ = s.as_string();
	char const *str = s_.c_str();
	char *tail;
	unsigned long const n = strtoul(str, &tail, 10);

	if ((n == ULONG_MAX) || ((n == 0) && (tail == str)))
		return false;
	long const tmp = strtoul(str, NULL, 10);
	if (tmp < 0)
		return false;
	if (r)
		*r = n;
	return true;
}
template<>
inline bool TryConvert<int, char>(StringPiece const &s, int *r) {
	long tmp;
	if (TryConvert(s, &tmp)) {
		if (r)
			*r = static_cast<int>(tmp);
		return true;
	}
	return false;
}
template<>
inline bool TryConvert<unsigned int, char>(StringPiece const &s, unsigned int *r) {
	unsigned long tmp;
	if (TryConvert(s, &tmp)) {
		if (r)
			*r = static_cast<unsigned int>(tmp);
		return true;
	}
	return false;
}
template<>
inline bool TryConvert<double, char>(StringPiece const &s, double *r) {
	std::string s_ = s.as_string();
	char const *str = s_.c_str();
	char *tail;
	double const n = strtod(str, &tail);

	if ((n == -HUGE_VAL) || (n == +HUGE_VAL) || ((n == 0) && (tail == str)))
		return false;
	if (r)
		*r = n;
	return true;
}

template<>
inline bool TryConvert<long, wchar_t>(WStringPiece const &s, long *r) {
	std::wstring s_ = s.as_string();
	wchar_t const *str = s_.c_str();
	wchar_t *tail;
	long const n = wcstol(str, &tail, 10);

	if ((n == LONG_MAX) || (n == LONG_MIN) || ((n == 0) && (tail == str)))
		return false;
	if (r)
		*r = n;
	return true;
}
template<>
inline bool TryConvert<unsigned long, wchar_t>(WStringPiece const &s, unsigned long *r) {
	std::wstring s_ = s.as_string();
	wchar_t const *str = s_.c_str();
	wchar_t *tail;
	unsigned long const n = wcstoul(str, &tail, 10);

	if ((n == ULONG_MAX) || ((n == 0) && (tail == str)))
		return false;
	long const tmp = wcstol(str, NULL, 10);
	if (tmp < 0)
		return false;
	if (r)
		*r = n;
	return true;
}
template<>
inline bool TryConvert<int, wchar_t>(WStringPiece const &s, int *r) {
	long tmp;
	if (TryConvert(s, &tmp)) {
		if (r)
			*r = static_cast<int>(tmp);
		return true;
	}
	return false;
}
template<>
inline bool TryConvert<unsigned int, wchar_t>(WStringPiece const &s, unsigned int *r) {
	unsigned long tmp;
	if (TryConvert(s, &tmp)) {
		if (r)
			*r = static_cast<unsigned int>(tmp);
		return true;
	}
	return false;
}
template<>
inline bool TryConvert<double, wchar_t>(WStringPiece const &s, double *r) {
	std::wstring s_ = s.as_string();
	wchar_t const *str = s_.c_str();
	wchar_t *tail;
	double const n = wcstod(str, &tail);

	if ((n == -HUGE_VAL) || (n == +HUGE_VAL) || ((n == 0) && (tail == str)))
		return false;
	if (r)
		*r = n;
	return true;
}

} // namespace cpcl

#endif // __CPCL_STRING_CAST_HPP
