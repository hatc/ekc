// fileItemInfo.hpp
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

#ifndef __FILES_FILEITEMINFO_HPP
#define __FILES_FILEITEMINFO_HPP

#include "stdafx.h"
#include <string>

#include <windows.h>
#include <Objidl.h>

#include "scopedComPtr.h"

// (IFileEnumerator as IPropertyBasket)[kpidFiltersUseStreams] - use as hint in FindFiles: 
//   if kpidFiltersUseStreams:
//     CArchiveExtractCallback.Init(archive, CFileEnumeratorCallback)
//     if (archive->Extract(NULL, (UInt32)(Int32)(-1), 0, CArchiveExtractCallback) != S_OK):
//       messageSink-> "unable check files in archive " + item_.path;
//     fileContainers.add(CArchiveExtractCallback->containers_);
//   else: 
//     for item in archiveFileIterator(item_):
//       if item.isDirectory:
//         fileContainers.add(item)
//       else:
//         if filterList.Match(item)
//           fileList.add(item)
//         if archiveExtension_hash_set.contain(item.fileExtension):
//           fileContainers.add(item)

namespace files {

struct fileItemInfo { // -> fileItem
	UINT64 size;
	FILETIME ctime;
	FILETIME mtime;
	FILETIME atime;
	DWORD attrib;
	UINT32 reparseTag; // If the dwFileAttributes member includes the FILE_ATTRIBUTE_REPARSE_POINT attribute, this member specifies the reparse tag.

	bool retrieved;
	std::wstring path, fileName_, fileExt_, name_;
	//size_t fileNameSize, nameSize, fileExtSize;
	// const wchar_t *fileName, *fileExt;

	scopedComPtr<IStream> stream; // use iterator.currentStream()

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

	// fileItemInfo() : retrieved(false), fileNameSize(0), nameSize(0), fileExtSize(0) {}
	fileItemInfo() : retrieved(false) {}
	fileItemInfo(WIN32_FIND_DATAW const &v, size_t cFileNameSize/*, wstring_piece const &rootPath*/) : retrieved(true) {
		size  = (((UINT64)v.nFileSizeHigh) << 32) | v.nFileSizeLow;
		ctime = v.ftCreationTime;
		atime = v.ftLastAccessTime;
		mtime = v.ftLastWriteTime;
		attrib = v.dwFileAttributes;
		reparseTag = (attrib & FILE_ATTRIBUTE_REPARSE_POINT) ? v.dwReserved0 : 0;

		setFileName(v.cFileName, cFileNameSize);
	}
	fileItemInfo(wchar_t const *fileName, size_t fileNameSize, UINT64 size_, DWORD attrib_, FILETIME mtime_) : retrieved(true) {
		size  = size_;
		ctime = mtime_;
		atime = mtime_;
		mtime = mtime_;
		attrib = attrib_;
		reparseTag = 0;

		setFileName(fileName, fileNameSize);
	}
	void reset(WIN32_FIND_DATAW const &v, size_t cFileNameSize) { // use in replace of currentInfo = fileItemInfo(findData, fileNameSize/*, path*/);
		retrieved = true;

		size  = (((UINT64)v.nFileSizeHigh) << 32) | v.nFileSizeLow;
		ctime = v.ftCreationTime;
		atime = v.ftLastAccessTime;
		mtime = v.ftLastWriteTime;
		attrib = v.dwFileAttributes;
		reparseTag = (attrib & FILE_ATTRIBUTE_REPARSE_POINT) ? v.dwReserved0 : 0;

		setFileName(v.cFileName, cFileNameSize);

		stream.Release();
	}
	void reset(wchar_t const *fileName, size_t fileNameSize, UINT64 size_, DWORD attrib_, FILETIME mtime_) {
		retrieved = true;

		size  = size_;
		ctime = mtime_;
		atime = mtime_;
		mtime = mtime_;
		attrib = attrib_;
		reparseTag = 0;

		setFileName(fileName, fileNameSize);

		stream.Release();
	}
	void reset() {
		retrieved = false;
		// fileNameSize = nameSize = fileExtSize = 0;
		if (!path.empty())
			path.erase(0, path.size()); // === path.clear()

		stream.Release();
	}
	void setFileName(wchar_t const *v, size_t len) {
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
			fileName_.assign(fileName, fileNameSize);
		else if (!fileName_.empty())
			fileName_.erase(0, fileName_.size());

		if (nameSize > 0)
			name_.assign(fileName, nameSize);
		else if (!name_.empty())
			name_.erase(0, name_.size());

		if (fileExtSize > 0)
			fileExt_.assign(fileExt, fileExtSize);
		else if (!fileExt_.empty())
			fileExt_.erase(0, fileExt_.size());
	}
	
	//std::wstring getFileName(const wchar_t *fileName, size_t fileNameSize) const {
	//	std::wstring fileName_(L"<empty>");
	//	if (fileNameSize > 0)
	//		fileName_.assign(fileName, fileNameSize); // SysAllocStringLen(const OLECHAR *pch, unsigned int cch);
	//	return fileName_;
	//}
	//std::wstring getName(const wchar_t *fileName, size_t nameSize) const {
	//	std::wstring name_(L"<empty>");
	//	if (nameSize > 0)
	//		name_.assign(fileName, nameSize); // SysAllocStringLen(const OLECHAR *pch, unsigned int cch);
	//	return name_;
	//}
	//std::wstring getExt(const wchar_t *fileExt, size_t fileExtSize) const {
	//	std::wstring fileExt_(L"<empty>");
	//	if (fileExtSize > 0)
	//		fileExt_.assign(fileExt, fileExtSize); // SysAllocStringLen(const OLECHAR *pch, unsigned int cch);
	//	return fileExt_;
	//}
};

//namespace std {
//	ostream& operator<<(ostream& out, fileItemInfo const &v) {
//		if (v && v.isDirectory())
//			out << string(v.path.begin(), v.path.end()) << ", isDirectory" << endl;
//		else if (v)
//			out << string(v.path.begin(), v.path.end()) << ", fileSize: " << v.size << endl;
//		else
//			out << "empty fileItemInfo" << endl;
//
//		return out;
//	}
//
//	ostream& operator<<(ostream& out, wstring const &v) {
//		out << string(v.begin(), v.end());
//		return out;
//	}
//}

} // namespace files

#endif // __FILES_FILEITEMINFO_HPP
