// fileIterator.hpp
// Copyright (C) 2011 Yuri Agafonov
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

#ifndef __FILES_FILEITERATOR_HPP
#define __FILES_FILEITERATOR_HPP

#include <iterator>
#include <boost/shared_ptr.hpp>

#include "fileItemInfo.hpp"
#include "temporaryStream.h"

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

struct scopedFindHandle {
	scopedFindHandle() : handle_(INVALID_HANDLE_VALUE) {}
	explicit scopedFindHandle(HANDLE v) : handle_(v) {}
	~scopedFindHandle() { close(); }

	HANDLE get() const { return handle_; }
	void set(HANDLE v) {
		close();
		handle_ = v;
	}
	HANDLE take() {
		HANDLE v = handle_;
		handle_ = INVALID_HANDLE_VALUE;
		return v;
	}
	operator bool() const { return handle_ != INVALID_HANDLE_VALUE; }
	void close() {
		if (handle_ != INVALID_HANDLE_VALUE) {
			::FindClose(handle_);
			handle_ = INVALID_HANDLE_VALUE;
		}
	}
private:
	HANDLE handle_;

	DISALLOW_COPY_AND_ASSIGN(scopedFindHandle);
};

//std::string formatMessage(long ecode) {
//	std::string r;
//	void *text(0);
//	::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//		NULL, ecode, MAKELANGID(LANG_ENGLISH/*LANG_NEUTRAL*/, SUBLANG_ENGLISH_US/*SUBLANG_NEUTRAL*/), (LPSTR)&text, 0, NULL);
//	if (text)
//		r = (LPCSTR)text;
//	::LocalFree((HLOCAL)text);
//
//	return r;
//}
struct fileIterator : public std::iterator<std::input_iterator_tag, fileItemInfo> {
	/* this implementation hides error's that return INVALID_HANDLE_VALUE */
	boost::shared_ptr<scopedFindHandle> searchHandle;
	fileItemInfo currentInfo;
	std::wstring path_;
	fileIterator() {}
	fileIterator(std::wstring const& path) : searchHandle(new scopedFindHandle()) {
		WIN32_FIND_DATAW findData;
		searchHandle->set(::FindFirstFileW(path.c_str(), &findData));
		// If the function fails because no matching files can be found, GetLastError function returns ERROR_FILE_NOT_FOUND.
		// i.e. possibly determine exact situation - file really not found or error occuried - something like this:
		// if (searchHandle == INVALID_HANDLE_VALUE) && (GetLastError() != ERROR_FILE_NOT_FOUND) { log(Error) << "cannot open search handle"; }
		if (searchHandle->get() != INVALID_HANDLE_VALUE) {
			size_t fileNameSize = std::char_traits<wchar_t>::length(findData.cFileName);
			if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && 
				((L'.' == findData.cFileName[0]) && ((1 == fileNameSize) || ((2 == fileNameSize) && (L'.' == findData.cFileName[1])))))
				++*this;
			else
				currentInfo.reset(findData, fileNameSize);

			if (path.size() > 2) {
				if ((path[path.size() - 2] == L'\\') 
					&& (path[path.size() - 1] == L'*')) {
					path_.assign(path.c_str(), path.size() - 1);

					currentInfo.path = path_ + currentInfo.path;
					// currentInfo.setFileName(currentInfo.path.c_str(), currentInfo.path.size());
				}
			}
		} else {
			// std::cout << formatMessage(::GetLastError());
			searchHandle.reset();
		}
	}
	// fileIterator_(const fileIterator_ &v) // compiler generated copy ctor is ok
	
	reference operator*() { return currentInfo; }

	HRESULT currentStream(IStream **r) const {
		if (!searchHandle) {
			return E_FAIL;
			//log(Error) << "attempt to derefernce currentStream on not dereferencable fileIterator";
		}
		std::wstring filePath(path_); filePath += currentInfo.fileName_;
		return files::fileStreamRead(filePath.c_str(), r);
	}
	
	fileIterator& operator++() {
		//if (!searchHandle) {
		//	log(Error) << "attempt to increment end iterator";
		//	throw "iterator not incrementable" or assert(0);
		//}
		if (!searchHandle)
			return *this;
		
		WIN32_FIND_DATAW findData;
		if (::FindNextFileW(searchHandle->get(), &findData) == 0) {
			searchHandle.reset();
			//if (::GetLastError() != ERROR_NO_MORE_FILES)
			//	log(Error) << "cannot get next file for search handle";
		} else {
			size_t fileNameSize = std::char_traits<wchar_t>::length(findData.cFileName);
			if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && 
				((L'.' == findData.cFileName[0]) && ((1 == fileNameSize) || ((2 == fileNameSize) && (L'.' == findData.cFileName[1])))))
				return ++*this;
			else {
				currentInfo.reset(findData, fileNameSize);
				if (!path_.empty()) {
					currentInfo.path = path_ + currentInfo.path;
					// currentInfo.setFileName(currentInfo.path.c_str(), currentInfo.path.size());
				}
			}
		}
		return *this;
	}
	fileIterator operator++(int) {
		/*
		http://www.sgi.com/tech/stl/InputIterator.html
		After executing ++i, it is not required that copies of the old value of i be dereferenceable or that they be in the domain of operator==.
		It is not guaranteed that it is possible to pass through the same input iterator twice.
		*/
		fileIterator tmp(*this); tmp.searchHandle.reset(); 
		++*this;
		return tmp;
	}
	
	bool operator==(const fileIterator &r) const {
		return searchHandle == r.searchHandle;
	}
	bool operator!=(const fileIterator &r) const {
		return !(*this == r);
	}
	/* InputIterator not needed to be LessThanComparable */
	//bool operator<(const fileIterator &_Right) const {
	//	return v < _Right.v;
	//}
	//bool operator>(const fileIterator &_Right) const {
	//	return _Right < *this;
	//}
	//bool operator<=(const fileIterator &_Right) const {
	//	return !(_Right < *this);
	//}
	//bool operator>=(const fileIterator &_Right) const {
	//	return !(*this < _Right);
	//}
};

} // namespace files

#endif // __FILES_FILEITERATOR_HPP
