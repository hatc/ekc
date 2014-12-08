// file_iterator.hpp
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

#ifndef __FILES_FILE_ITERATOR_HPP
#define __FILES_FILE_ITERATOR_HPP

#include <iterator>
#include <boost/shared_ptr.hpp>

#include <string_util.h>
#include <trace.h>

#include "file_info.hpp"

namespace ext {

namespace files {

struct ScopedFindHandle {
	ScopedFindHandle() : handle_(INVALID_HANDLE_VALUE) {}
	explicit ScopedFindHandle(HANDLE v) : handle_(v) {}
	~ScopedFindHandle() { close(); }

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

	DISALLOW_COPY_AND_ASSIGN(ScopedFindHandle);
};

struct FileIterator : public std::iterator<std::input_iterator_tag, FileInfo> {
	boost::shared_ptr<ScopedFindHandle> searchHandle;
	FileInfo currentInfo;
	std::wstring path_;

	FileIterator() {}
	FileIterator(std::wstring const& path) : searchHandle(new ScopedFindHandle()) {
		WIN32_FIND_DATAW findData;
		searchHandle->set(::FindFirstFileW(path.c_str(), &findData));
		if (searchHandle->get() != INVALID_HANDLE_VALUE) {
			size_t fileNameSize = std::char_traits<wchar_t>::length(findData.cFileName);
			if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && 
				((L'.' == findData.cFileName[0]) && ((1 == fileNameSize) || ((2 == fileNameSize) && (L'.' == findData.cFileName[1])))))
				++*this;
			else
				currentInfo.reset(findData, fileNameSize);

			if (path.size() > 2) {
				size_t offset(path.size() - 1);
				for (; offset != 0; --offset) {
					if (path[offset] == L'\\')
						break;
				}
				path_.assign(path.c_str(), offset + 1);
				currentInfo.path = path_ + currentInfo.path;
				//if ((path[path.size() - 2] == L'\\') 
				//	&& (path[path.size() - 1] == L'*')) {
				//	path_.assign(path.c_str(), path.size() - 1);

				//	currentInfo.path = path_ + currentInfo.path;
				//	// currentInfo.setFileName(currentInfo.path.c_str(), currentInfo.path.size());
				//}
			}
		} else {
			unsigned long error_code = GetLastError();
			if ((error_code != ERROR_FILE_NOT_FOUND) && (error_code != ERROR_PATH_NOT_FOUND))
				cpcl::ErrorSystem(error_code, "FileIterator('%s'): FindFirstFileW fails:", cpcl::ConvertUTF16_CP1251(path).c_str());

			searchHandle.reset();
		}
	}
	
	reference operator*() { return currentInfo; }
	
	FileIterator& operator++() {
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
	FileIterator operator++(int) {
		/*
		http://www.sgi.com/tech/stl/InputIterator.html
		After executing ++i, it is not required that copies of the old value of i be dereferenceable or that they be in the domain of operator==.
		It is not guaranteed that it is possible to pass through the same input iterator twice.
		*/
		FileIterator tmp(*this); tmp.searchHandle.reset(); 
		++*this;
		return tmp;
	}
	
	bool operator==(const FileIterator &r) const {
		return searchHandle == r.searchHandle;
	}
	bool operator!=(const FileIterator &r) const {
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

} // namespace ext

#endif // __FILES_FILE_ITERATOR_HPP
