// file_info.hpp
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

#ifndef __FILES_FILE_INFO_HPP
#define __FILES_FILE_INFO_HPP

#include <basic.h>

#include <stdio.h>
#include <tchar.h>

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace ext { // shield engine FileInfo impl

namespace files { 

struct FileInfo {
	UINT64 size;
	FILETIME ctime;
	FILETIME mtime;
	FILETIME atime;
	DWORD attrib;
	UINT32 reparseTag; // If the dwFileAttributes member includes the FILE_ATTRIBUTE_REPARSE_POINT attribute, this member specifies the reparse tag.

	bool retrieved;
	std::wstring path, file_name, name, ext;

	bool matchesMask(UINT32 mask) const { return ((attrib & mask) != 0); }
	bool isArchived() const { return matchesMask(FILE_ATTRIBUTE_ARCHIVE); }
  bool isCompressed() const { return matchesMask(FILE_ATTRIBUTE_COMPRESSED); }
  bool isDirectory() const { return matchesMask(FILE_ATTRIBUTE_DIRECTORY); }
  bool isEncrypted() const { return matchesMask(FILE_ATTRIBUTE_ENCRYPTED); }
  bool isHidden() const { return matchesMask(FILE_ATTRIBUTE_HIDDEN); }
  bool isNormal() const { return matchesMask(FILE_ATTRIBUTE_NORMAL); }
  bool isOffline() const { return matchesMask(FILE_ATTRIBUTE_OFFLINE); }
  bool isReadOnly() const { return matchesMask(FILE_ATTRIBUTE_READONLY); }
  bool hasReparsePoint() const { return matchesMask(FILE_ATTRIBUTE_REPARSE_POINT); }
  bool isSparse() const { return matchesMask(FILE_ATTRIBUTE_SPARSE_FILE); }
  bool isSystem() const { return matchesMask(FILE_ATTRIBUTE_SYSTEM); }
  bool isTemporary() const { return matchesMask(FILE_ATTRIBUTE_TEMPORARY); }

	operator bool() const { return retrieved; }

	FileInfo() : retrieved(false) {}
	FileInfo(WIN32_FIND_DATAW const &v, size_t cFileNameSize) : retrieved(true) {
		size  = (((UINT64)v.nFileSizeHigh) << 32) | v.nFileSizeLow;
		ctime = v.ftCreationTime;
		atime = v.ftLastAccessTime;
		mtime = v.ftLastWriteTime;
		attrib = v.dwFileAttributes;
		reparseTag = (attrib & FILE_ATTRIBUTE_REPARSE_POINT) ? v.dwReserved0 : 0;

		SetFileName(v.cFileName, cFileNameSize);
	}
	FileInfo(wchar_t const *fileName, size_t fileNameSize, UINT64 size_, DWORD attrib_, FILETIME mtime_) : retrieved(true) {
		size  = size_;
		ctime = mtime_;
		atime = mtime_;
		mtime = mtime_;
		attrib = attrib_;
		reparseTag = 0;

		SetFileName(fileName, fileNameSize);
	}
	void reset(WIN32_FIND_DATAW const &v, size_t cFileNameSize) {
		retrieved = true;

		size  = (((UINT64)v.nFileSizeHigh) << 32) | v.nFileSizeLow;
		ctime = v.ftCreationTime;
		atime = v.ftLastAccessTime;
		mtime = v.ftLastWriteTime;
		attrib = v.dwFileAttributes;
		reparseTag = (attrib & FILE_ATTRIBUTE_REPARSE_POINT) ? v.dwReserved0 : 0;

		SetFileName(v.cFileName, cFileNameSize);
	}
	void reset(wchar_t const *fileName, size_t fileNameSize, UINT64 size_, DWORD attrib_, FILETIME mtime_) {
		retrieved = true;

		size  = size_;
		ctime = mtime_;
		atime = mtime_;
		mtime = mtime_;
		attrib = attrib_;
		reparseTag = 0;

		SetFileName(fileName, fileNameSize);
	}
	void reset() {
		retrieved = false;
		// fileNameSize = nameSize = fileExtSize = 0;
		if (!path.empty())
			path.erase(0, path.size()); // === path.clear()
	}
	void SetFileName(wchar_t const *v, size_t len) {
		size_t fileNameSize, nameSize, fileExtSize;
		const wchar_t *fileName, *fileExt(NULL);

		fileNameSize = nameSize = fileExtSize = 0;
		path.assign(v, len);
		wchar_t const *path_(path.c_str() + path.size());

		while ((path_ != path.c_str()) && (*path_ != L'\\')) {
			if ((NULL == fileExt) && (L'.' == *path_)) {
				fileExt = path_ + 1;
			}
			--path_;
		}
		fileName = path_; if (*fileName == L'\\') ++fileName;
		fileNameSize = path.size() - (fileName - path.c_str());
		fileExtSize = (fileExt != NULL) ? (path.size() - (fileExt - path.c_str())) : 0;
		nameSize = fileNameSize - fileExtSize; if (fileExtSize > 0) --nameSize;

		if (fileNameSize > 0)
			file_name.assign(fileName, fileNameSize);
		else if (!file_name.empty())
			file_name.erase(0, file_name.size());

		if (nameSize > 0)
			name.assign(fileName, nameSize);
		else if (!name.empty())
			name.erase(0, name.size());

		if (fileExtSize > 0)
			ext.assign(fileExt, fileExtSize);
		else if (!ext.empty())
			ext.erase(0, ext.size());
	}
};

} // namespace files

} // namespace ext

#endif // __FILES_FILE_INFO_HPP
