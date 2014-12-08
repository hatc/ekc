// formatidiv.hpp
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

#ifndef __CPCL_FORMATIDIV_HPP
#define __CPCL_FORMATIDIV_HPP

#include <type_traits>

namespace cpcl {

struct formatidiv_signed_tag {};
struct formatidiv_unsigned_tag {};
struct formatidiv_unsigned_hex_tag {};
//struct formatidiv_signed_hex_tag {};
struct formatidiv_unsigned_bin_tag {};

template<size_t Base, bool ValueTypeIsUnsigned>
struct formatidiv_traits {
	typedef formatidiv_signed_tag tag;
};
template<size_t Base>
struct formatidiv_traits<Base, true> {
	typedef formatidiv_unsigned_tag tag;
};
template<>
struct formatidiv_traits<16, true> {
	typedef formatidiv_unsigned_hex_tag tag;
};
//template<>
//struct formatidiv_traits<16, false> {
//	typedef formatidiv_signed_hex_tag tag;
//};
template<>
struct formatidiv_traits<2, true> {
	typedef formatidiv_unsigned_bin_tag tag;
};

//template<class ValueType, size_t Base>
//inline char* formatidiv(formatidiv_signed_tag*, char *s, ValueType v, size_t width, char place) {
//	if (v < 0) {
//		*s++ = '-';
//		v *= -1;
//		--width;
//	}
//	for (char *h = s - 1 + width; h >= s; --h)
//		if (v != 0) { // (v > 0)
//			*h = "0123456789ABCDEF"[v % Base];
//			v /= Base;
//		} else
//			*h = place;
//	return s + width;
//}
// correct variant if place != '0'
//template<class ValueType, size_t Base>
//inline char* formatidiv(formatidiv_signed_tag*, char *s, ValueType v, size_t width, char place) {
//	bool sign = (v < 0);
//	if (sign)
//		v *= -1;
//	for (char *h = s - 1 + width; h >= s; --h)
//		if (v != 0) { // (v > 0)
//			*h = "0123456789ABCDEF"[v % Base];
//			v /= Base;
//		} else {
//			if (sign) {
//				*h = '-';
//				sign = false;
//			} else
//				*h = place;
//		}
//	return s + width;
//}
template<class ValueType, size_t Base>
inline char* formatidiv(formatidiv_signed_tag*, char *s, ValueType v, size_t width, char place) {
	bool sign = false;
	if (v < 0) {
		v *= -1;
		if ('0' == place) {
			*s++ = '-';
			--width;
		} else
			sign = true;
	}
	for (char *h = s - 1 + width; h >= s; --h)
		if (v != 0) { // (v > 0)
			*h = "0123456789ABCDEF"[v % Base];
			v /= Base;
		} else {
			if (sign) {
				*h = '-';
				sign = false;
			} else
				*h = place;
		}
	return s + width;
}
template<class ValueType, size_t Base>
inline char* formatidiv(formatidiv_unsigned_tag*, char *s, ValueType v, size_t width, char place) {
	for (char *h = s - 1 + width; h >= s; --h)
		if (v != 0) {
			*h = "0123456789ABCDEF"[v % Base];
			v /= Base;
		} else
			*h = place;
	return s + width;
}
template<class ValueType, size_t Base>
inline char* formatidiv(formatidiv_unsigned_hex_tag*, char *s, ValueType v, size_t width, char place) {
	for (char *h = s - 1 + width; h >= s; --h)
		if (v != 0) {
			*h = "0123456789ABCDEF"[v & 0xF];
			v >>= 4;
		} else
			*h = place;
	return s + width;
}
//template<class ValueType, size_t Base>
//inline char* formatidiv(formatidiv_signed_hex_tag*, char *s, ValueType v, size_t width, char place) {
//	typedef boost::detail::make_unsigned<ValueType>::type ValueUnsignedType;
//	return formatidiv<ValueUnsignedType, 16>((formatidiv_unsigned_hex_tag *)0, s, static_cast<ValueUnsignedType>(v), width, place);
//}
template<class ValueType, size_t Base>
inline char* formatidiv(formatidiv_unsigned_bin_tag*, char *s, ValueType v, size_t width, char place) {
	for (char *h = s - 1 + width; h >= s; --h)
		if (v != 0) {
			*h = "01"[v & 1];
			v >>= 1;
		} else
			*h = place;
	return s + width;
}

template<size_t Base, class ValueType>
inline char* formatidiv(char *s, ValueType v, size_t width, char place = '0') {
	typedef char BaseInvalid[Base < 17 ? 1 : -1];
	typedef char ValueTypeInvalid[std::tr1::is_integral<ValueType>::value && !(std::tr1::is_same<ValueType, bool>::value || std::tr1::is_same<ValueType, wchar_t>::value) ? 1 : -1];

	return formatidiv<ValueType, Base>((formatidiv_traits<Base, std::tr1::is_unsigned<ValueType>::value>::tag *)0, s, v, width, place);
}

/* и все из-за того что cl v >>= 4; (right shift) для signed типов использует sar - arithmetic right shift, а не shr - logical right shift ...  */
template<>
inline char* formatidiv<16, char>(char *s, char v, size_t width, char place) {
	return formatidiv<unsigned char, 16>((formatidiv_unsigned_hex_tag *)0, s, static_cast<unsigned char>(v), width, place);
}
//template<>
//inline char* formatidiv<16, wchar_t>(char *s, wchar_t v, size_t width, char place) {
//	return formatidiv<unsigned wchar_t, 16>((formatidiv_hex_tag *)0, s, static_cast<unsigned wchar_t>(v), width, place);
//}
template<>
inline char* formatidiv<16, short>(char *s, short v, size_t width, char place) {
	return formatidiv<unsigned short, 16>((formatidiv_unsigned_hex_tag *)0, s, static_cast<unsigned short>(v), width, place);
}
template<>
inline char* formatidiv<16, int>(char *s, int v, size_t width, char place) {
	return formatidiv<unsigned int, 16>((formatidiv_unsigned_hex_tag *)0, s, static_cast<unsigned int>(v), width, place);
}
template<>
inline char* formatidiv<16, long>(char *s, long v, size_t width, char place) {
	return formatidiv<unsigned long, 16>((formatidiv_unsigned_hex_tag *)0, s, static_cast<unsigned long>(v), width, place);
}
template<>
inline char* formatidiv<16, __int64>(char *s, __int64 v, size_t width, char place) {
	return formatidiv<unsigned __int64, 16>((formatidiv_unsigned_hex_tag *)0, s, static_cast<unsigned __int64>(v), width, place);
}

template<>
inline char* formatidiv<2, char>(char *s, char v, size_t width, char place) {
	return formatidiv<unsigned char, 2>((formatidiv_unsigned_bin_tag *)0, s, static_cast<unsigned char>(v), width, place);
}
template<>
inline char* formatidiv<2, short>(char *s, short v, size_t width, char place) {
	return formatidiv<unsigned short, 2>((formatidiv_unsigned_bin_tag *)0, s, static_cast<unsigned short>(v), width, place);
}
template<>
inline char* formatidiv<2, int>(char *s, int v, size_t width, char place) {
	return formatidiv<unsigned int, 2>((formatidiv_unsigned_bin_tag *)0, s, static_cast<unsigned int>(v), width, place);
}
template<>
inline char* formatidiv<2, long>(char *s, long v, size_t width, char place) {
	return formatidiv<unsigned long, 2>((formatidiv_unsigned_bin_tag *)0, s, static_cast<unsigned long>(v), width, place);
}
template<>
inline char* formatidiv<2, __int64>(char *s, __int64 v, size_t width, char place) {
	return formatidiv<unsigned __int64, 2>((formatidiv_unsigned_bin_tag *)0, s, static_cast<unsigned __int64>(v), width, place);
}

} // namespace cpcl

#endif // __CPCL_FORMATIDIV_HPP
