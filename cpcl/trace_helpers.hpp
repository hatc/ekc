// trace_helpers.hpp
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

#ifndef __CPCL_TRACE_HELPERS_HPP
#define __CPCL_TRACE_HELPERS_HPP

#include <boost/scoped_array.hpp>

#include "string_util.h"
#include "timer.h" // move TimedSeed() to rand_util ???

namespace cpcl {

inline unsigned char* UTF16_CP1251_(WStringPiece const &path, boost::scoped_array<unsigned char> &path_cp1251) {
	path_cp1251.reset(new unsigned char[path.size() + 1]);
	*ConvertUTF16_CP1251(path, path_cp1251.get()) = '\0';
	return path_cp1251.get();
}

// move to array_piece.hpp ???
template<class CharType, size_t N>
struct array_piece {
 array_piece(CharType const (&s)[N]) : v(s), size(N - 1) {}
 CharType const& operator[](size_t offset) const { return *(v + offset); }
 
 CharType const * const v;
 size_t const size;
private:
 array_piece();
 void operator=(array_piece<CharType, N> const&);
};

// move to rand_util ???
template<class CharType, size_t N>
inline array_piece<CharType, N + 1> RandNameCharacters(void) {
	CharType InvalidFunction[bool(N) ? -1 : -1];
}
template<>
inline array_piece<char, 0x11> RandNameCharacters<char, 0x10>(void) {
 // return "0123456789ABCDEF"; ok in Visual C++ 2008, but error in gcc: convert const char* to non-scalar type
 return array_piece<char, 0x11>("0123456789ABCDEF");
}
template<>
inline array_piece<wchar_t, 0x11> RandNameCharacters<wchar_t, 0x10>(void) {
 // return L"0123456789ABCDEF"; ok in Visual C++ 2008, but error in gcc: convert const wchar_t* to non-scalar type
 return array_piece<wchar_t, 0x11>(L"0123456789ABCDEF");
}

template<class CharType>
inline CharType* RandName(CharType *buf, size_t const len, cpcl::BasicStringPiece<CharType, std::char_traits<CharType> > const &head, cpcl::BasicStringPiece<CharType, std::char_traits<CharType> > const &tail) {
 if (len < head.size() + tail.size() + 1)
  return *buf = 0,buf;
 
 CharType *s = buf;
 for (size_t i = 0; i < head.size(); ++i, ++s)
  *s = head[i];
 
 unsigned int seed = TimedSeed();
 for (CharType const * const s_ = s + len - head.size() - tail.size() - 1; s != s_; ++s) {
  seed = seed * 214013L + 2531011L;
  *s = RandNameCharacters<CharType, 0x10>()[(unsigned int)(((seed >> 16) & 0x7fff) / 0x800)]; // i.e. (max(rand) == 0x7fff / 0x800) < 0x10
 }
 
 for (size_t i = 0; i < tail.size(); ++i, ++s)
  *s = tail[i];
 *s = 0;
 return buf;
}
template<class CharType, size_t N>
inline CharType* RandName(CharType (&buf)[N], cpcl::BasicStringPiece<CharType, std::char_traits<CharType> > const &head, cpcl::BasicStringPiece<CharType, std::char_traits<CharType> > const &tail) {
 return RandName(buf, N, head, tail);
}
//inline wchar_t* RandName(wchar_t *buf, size_t const len, cpcl::WStringPiece const &head, cpcl::WStringPiece const &tail) {
//	if (len < head.size() + tail.size() + 1)
//		return *buf = 0,buf;
//
//	wchar_t *s = buf;
//	for (size_t i = 0; i < head.size(); ++i, ++s)
//		*s = head[i];
//
//	unsigned int seed = TimedSeed();
//	for (wchar_t const * const s_ = s + len - head.size() - tail.size() - 1; s != s_; ++s) {
//		seed = seed * 214013L + 2531011L;
//		*s = L"0123456789ABCDEF"[(unsigned int)(((seed >> 16) & 0x7fff) / 0x800)]; // i.e. (max(rand) == 0x7fff / 0x800) < 0x10
//	}
//
//	for (size_t i = 0; i < tail.size(); ++i, ++s)
//		*s = tail[i];
//	*s = 0;
//	return buf;
//}
//template<size_t N>
//inline wchar_t* RandName(wchar_t (&buf)[N], cpcl::WStringPiece const &head, cpcl::WStringPiece const &tail) {
//	return RandName(buf, N, head, tail);
//}

} // namespace cpcl

#endif // __CPCL_TRACE_HELPERS_HPP
