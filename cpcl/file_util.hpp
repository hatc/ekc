// file_util.hpp
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

#ifndef __CPCL_FILE_UTIL_HPP
#define __CPCL_FILE_UTIL_HPP

#include "string_piece.hpp"

namespace cpcl {

/* 
 * template<class Char>
 * struct os_path_traits<Char> {
 *  static Char Delimiter() { return #if UNIX (Char)‘/’ #elif WINDOWS (Char)‘\\’; }
 * };
 *
 * template<class Char, class Traits = os_path_traits<Char> >
 * inline BasicStringPiece<Char> BaseName(BasicStringPiece<Char> const &s) {
 * …
 * }
 */

/* 
 * strip directory and suffix from filenames  
 * examples:                                  
 * basename(‘/usr/bin/sort’) -> ‘sort’        
 * basename(‘/include/stdio.h .h’) -> ‘stdio’
 */
// also consider BaseFileName: 
//  basefilename(‘/include/source.adl.hpp’) -> ‘source.adl’
template<class Char>
inline BasicStringPiece<Char> BaseName(BasicStringPiece<Char> const &s) {
	if (s.empty())
		return s;

	Char const *path_head = s.data() - 1;
	Char const *i = path_head + s.size();
	Char const *path_tail = i + 1;
	for (; i != path_head; --i) {
		if (*i == (Char)'.')
			path_tail = i;
		else if (*i == (Char)'\\')
			break;
	}

	++i;
	return BasicStringPiece<Char>(i, path_tail - i);
}

template<class Char>
inline BasicStringPiece<Char> BaseName(Char const *s) {
	return BaseName(BasicStringPiece<Char>(s));
}

/* 
 * strip directory and tail suffix from filenames  
 * examples:                                  
 * basefilename(‘/usr/bin/sort’) -> ‘sort’        
 * basefilename(‘/include/stdio.h .h’) -> ‘stdio.h ’
 * basefilename(‘/include/source.adl.hpp’) -> ‘source.adl’
 */
template<class Char>
inline BasicStringPiece<Char> BaseFileName(BasicStringPiece<Char> const &s) {
	if (s.empty())
		return s;

	Char const *path_head = s.data() - 1;
	Char const *i = path_head + s.size();
	Char const *path_tail = i + 1;
	bool strip_suffix(false);
	for (; i != path_head; --i) {
		if (!strip_suffix && (*i == (Char)'.')) {
			path_tail = i;
			strip_suffix = true;
		} else if (*i == (Char)'\\')
			break;
	}

	++i;
	return BasicStringPiece<Char>(i, path_tail - i);
}

template<class Char>
inline BasicStringPiece<Char> BaseFileName(Char const *s) {
	return BaseFileName(BasicStringPiece<Char>(s));
}

/* 
 * strip non-directory suffix from file name  
 * examples:                                  
 * dirname(‘/usr/bin/sort’) -> ‘/usr/bin’
 * dirname(‘stdio.h .h’) -> ‘’
 * ??? dirname(‘stdio.h .h’) -> ‘.’ | ‘’ ‘.’ meaning the current directory
 */
template<class Char>
inline BasicStringPiece<Char> DirName(BasicStringPiece<Char> const &s) {
	if (s.empty())
		return s;

	Char const *path_head = s.data() - 1;
	Char const *i = path_head + s.size();
	bool t = false;
	for (; i != path_head; --i) {
		if (*i == (Char)'\\')
			t = true;
		else if (t)
			break;
	}
	// return (t) ? BasicStringPiece<Char>(s.data(), i - path_head) : s;
	return (t) ? BasicStringPiece<Char>(s.data(), i - path_head) : BasicStringPiece<Char>();
}

template<class Char>
inline BasicStringPiece<Char> DirName(Char const *s) {
	return DirName(BasicStringPiece<Char>(s));
}

/* 
 * return suffix from filenames
 * examples:                                  
 * fileextension(‘/usr/bin/sort’) -> ‘’
 * fileextension(‘stdio.h .h’) -> ‘h’
 */
template<class Char>
inline BasicStringPiece<Char> FileExtension(BasicStringPiece<Char> const &s) {
	if (s.empty())
		return s;

	Char const *path_head = s.data() - 1;
	Char const *i = path_head + s.size();
	Char const *path_tail = i + 1;
	bool t = false;
	for (; i != path_head; --i) {
		if (*i == (Char)'.') {
			t = true;
			break;
		}
	}
	++i;
	return (t) ? BasicStringPiece<Char>(i, path_tail - i) : BasicStringPiece<Char>();
	/*int i = s.size() - 1;
	for (; i >= 0; --i) {
		if ((Char)0x2E == s[(size_t)i])
			break;
	}
	return s.substr((size_t)(i + 1));*/
}

template<class Char>
inline BasicStringPiece<Char> FileExtension(Char const *s) {
	return FileExtension(BasicStringPiece<Char>(s));
}

/*where is no use cases when this function overloads can be useful - transform string literal is meaningless, just define new
template<class Char, size_t N>
inline BasicStringPiece<Char> BaseNameFromLiteral(Char const (&s)[N]) {
	return BaseName(BasicStringPiece<Char>(s, N - 1));
}
template<class Char, size_t N>
inline BasicStringPiece<Char> BaseFileNameFromLiteral(Char const (&s)[N]) {
	return BaseFileName(BasicStringPiece<Char>(s, N - 1));
}
template<class Char, size_t N>
inline BasicStringPiece<Char> DirNameFromLiteral(Char const (&s)[N]) {
	return DirName(BasicStringPiece<Char>(s, N - 1));
}
template<class Char, size_t N>
inline BasicStringPiece<Char> FileExtensionFromLiteral(Char const (&s)[N]) {
	return FileExtension(BasicStringPiece<Char>(s, N - 1));
}*/

} // namespace cpcl

#endif // __CPCL_FILE_UTIL_HPP
