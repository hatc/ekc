// string_util_win.hpp
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

#ifndef __CPCL_STRING_UTIL_WIN_HPP
#define __CPCL_STRING_UTIL_WIN_HPP

#include <cstdarg>

namespace cpcl {

namespace string_format_impl_private {

inline int vsnprintf(char *r, size_t r_size, char const *format, va_list arguments) {
	return _vsnprintf_s(r, r_size, _TRUNCATE, format, arguments);
}
inline int vsnprintf(wchar_t *r, size_t r_size, wchar_t const *format, va_list arguments) {
	return _vsnwprintf_s(r, r_size, _TRUNCATE, format, arguments);
}

} // namespace string_format_impl_private

template<class Char>
inline size_t StringFormatImpl(Char *r, size_t r_size, Char const *format, va_list arguments, bool *truncated) {
	if (!((r) && r_size > 0))
		return 0;

	bool r_truncated(false);
	int n = string_format_impl_private::vsnprintf(r, r_size, format, arguments);
	if (n < 0) {
		r_truncated = true;
		r[r_size - 1] = 0;
	}

	if (truncated)
		*truncated = r_truncated;
	return (r_truncated) ? (r_size - 1) : static_cast<size_t>(n);
}

// string_util_posix.hpp
// posix vsnprintf/vswprintf
//template<class Char>
//inline size_t StringFormatImpl(Char *r, size_t r_size, Char const *format, va_list arguments, bool *truncated) {
//	...
//	int n = string_format_impl_private::vsnprintf(r, r_size, format, arguments);
//	if (n < 0 || n >= r_size) // If the resulting string gets truncated due to buf_size limit, function returns the total number of characters (not including the terminating null-byte) which would have been written, if the limit was not imposed
//	...
//}

} // namespace cpcl

#endif // __CPCL_STRING_UTIL_WIN_HPP
