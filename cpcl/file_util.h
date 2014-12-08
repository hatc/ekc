// file_util.h
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

#ifndef __CPCL_FILE_UTIL_H
#define __CPCL_FILE_UTIL_H

// #include "string_piece.hpp"
#include "file_util.hpp"

namespace cpcl {

template<class CharType>
inline CharType* NormalizePath(CharType* path, size_t path_size = 0) { // return pointer at new end of string
	if (!path_size)
		path_size = std::char_traits<CharType>::length(path);
	/*if ((path_size > 4) &&
		((CharType)'\\' == path[0]) &&
		((CharType)'\\' == path[1]) &&
		((CharType)'?' == path[2]) &&
		((CharType)'\\' == path[3])) {
		path_size -= 4;
		std::char_traits<CharType>::_Move_s(path, path_size + 4, path + 4, path_size);
	}*/
	
	for (size_t i = 0; i < path_size; ++i)
		if ((CharType)'/' == path[i])
			path[i] = (CharType)'\\';
	path += path_size - 1;
	while ((CharType)'\\' == *path)
		--path;
	return path;
	// return path.replace('/', '\\').ensure_not_delimiter();
}
template<class CharType>
inline std::basic_string<CharType>& NormalizePath(std::basic_string<CharType>& path) { 
	CharType* path_ = const_cast<CharType*>(path.c_str());
	CharType* path_delimiter = NormalizePath(path_, path.size());
	path.resize(path_delimiter - path_ + 1);
	return path;
}
/*
template<class CharType>
bool GetPathComponents(StringPiece<CharType> const &path,
	std::wstring *basename, std::wstring *name, std::wstring *ext) {
	NormalizePath;
	for (;;) {
	 ...
	}
}
*/

//template<class CharType>
// inline bool IsAbsoluteFilePath(StringPiece<CharType> const &s) { return s match "[a-zA-Z]!\\:.*"); } //?/c:/path/file.ext ???
//template<class CharType>
// inline bool IsAbsoluteFilePath(CharType const *s) { return IsAbsolutePath(StringPiece<CharType>(s)); }

// defined in file_util.hpp
#if 0 
template<class CharType>
inline BasicStringPiece<CharType, std::char_traits<CharType> > FileExtension(BasicStringPiece<CharType, std::char_traits<CharType> > const &s) {
	if (s.empty())
		return s;
	int i = s.size() - 1;
	for (; i >= 0; --i) {
		if ((CharType)0x2E == s[(size_t)i])
			break;
	}
	return s.substr((size_t)(i + 1));
}
template<class CharType>
inline BasicStringPiece<CharType, std::char_traits<CharType> > FileExtension(CharType const *s) {
	return FileExtension(BasicStringPiece<CharType, std::char_traits<CharType> >(s));
}
#endif

bool GetTemporaryDirectory(std::wstring *r);
bool GetModuleFilePath(void *hmodule, std::wstring *r);
bool CreateTemporaryFile(WStringPiece const &folder, std::wstring *r);
bool ExistFilePath(WStringPiece const &path, bool *is_directory);
bool RemoveReadOnlyAttribute(WStringPiece const &path);
bool DeleteFilePath(WStringPiece const &path);
bool GetPathComponents(WStringPiece const &path,
	std::wstring *basename, std::wstring *name, std::wstring *ext);

#if 0
bool GetVirtualPathComponents(WStringPiece const &path,
	std::wstring *disk_designator, std::wstring *real_path, std::wstring *virtual_path);
#endif

size_t AvailableDiskSpaceMib(WStringPiece const &path);

} // namespace cpcl

#endif // __CPCL_FILE_UTIL_H
