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
#include "stdafx.h"

#include "fileUtil.h"

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include <windows.h>
#include <shellapi.h> // SHFileOperation

namespace files {
	
bool getTemporaryDirectory(std::wstring *r) {
	wchar_t buffer[0x1000] = { 0 };
	if (!::GetTempPathW(sizeof(buffer)/sizeof(*buffer), &buffer[0])) {
		if (r) 
			*r = L"c:\\temp";
		return false;
	}
	if (r)
		*r = &buffer[0];
	return true;
}
static const wchar_t *TEMPORARY_PREFIX = L"^_^";
bool createTemporaryFile(const std::wstring &folder, std::wstring *r) {
	wchar_t buffer[MAX_PATH + 1] = { 0 };
	//***** The directory path for the file name cannot be longer than MAX_PATH–14 characters, or GetTempFileName will fail. *****//
	if (!::GetTempFileNameW(folder.c_str(), TEMPORARY_PREFIX, 0, &buffer[0]))
		return false;
	if (r)
		*r = &buffer[0];
	return true;
}

bool exist(const std::wstring &path, bool *isDirectory) {
	WIN32_FIND_DATAW findData;
	HANDLE searchHandle = ::FindFirstFileW(path.c_str(), &findData);
	bool r = (searchHandle != INVALID_HANDLE_VALUE);
	if (r) {
		if (isDirectory)
			*isDirectory = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		::FindClose(searchHandle);
	}
	return r;
}

inline static bool _getFileAttributes(const std::wstring &path, DWORD &attrs) {
	attrs = ::GetFileAttributesW(path.c_str());
	return attrs != INVALID_FILE_ATTRIBUTES;
}
inline static bool _setFileAttributes(const std::wstring &path, DWORD attrs) {
	return ::SetFileAttributes(path.c_str(), attrs) != FALSE;
}
bool removeReadOnlyAttribute(const std::wstring &path) {
	DWORD attrs;
	if (!_getFileAttributes(path, attrs))
		return false;

	if (attrs & FILE_ATTRIBUTE_READONLY) {
		attrs ^= FILE_ATTRIBUTE_READONLY;
		return _setFileAttributes(path, attrs);
	}
	return true;
}

bool deleteFile(const std::wstring &path) {
	bool isDirectory;
	if (!exist(path, &isDirectory))
		return false;
	/*****
	To delete a read-only file, first you must remove the read-only attribute. 
	To delete or rename a file, you must have either delete permission on the file, or delete child permission in the parent directory. 
	To recursively delete the files in a directory, use the SHFileOperation function. 
	To remove an empty directory, use the RemoveDirectory function. 
	*****/
	if (isDirectory) {
		SHFILEOPSTRUCT shOperations; ZeroMemory(&shOperations, sizeof(shOperations));
		SIZE_T kroxa(5); if ((*--path.end()) != L'\\') ++kroxa;
		_TCHAR *fromBuffer = (_TCHAR*)::LocalAlloc(LMEM_ZEROINIT, (path.size()+kroxa)*sizeof(_TCHAR));
		if (fromBuffer) {
			lstrcpyn(fromBuffer, path.c_str(), path.size()+1);
			fromBuffer[path.size()+kroxa-3] = TEXT('*'); 
			fromBuffer[path.size()+kroxa-4] = TEXT('.'); 
			fromBuffer[path.size()+kroxa-5] = TEXT('*'); 
			fromBuffer[path.size()+kroxa-6] = TEXT('\\');

			shOperations.pFrom = &fromBuffer[0];
			shOperations.wFunc = FO_DELETE;
			shOperations.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;

			DWORD dw = GetLastError();
			bool res = ::SHFileOperation(&shOperations) == 0;
			//***** shOperations.hwnd == 0 => ERROR_INVALID_HANDLE *****//
			SetLastError(dw);

			::LocalFree(fromBuffer);
			if (res)
				return ::RemoveDirectoryW(path.c_str()) != FALSE;
		}
	} else {
		// replace witn inline deleteFile() { if ::DeleteFileW() == FALSE; return ::DeleteFileW() != FALSE } - file can be unaccesible due function call but then become again online
		if (removeReadOnlyAttribute(path)) {
			return ::DeleteFileW(path.c_str()) != FALSE;
		}
	}

	return false;
}

template<class CharType>
CharType* normalize(CharType* path, size_t pathSize = 0) { // return pointer at new end of string
	if (!pathSize)
		pathSize = std::char_traits<CharType>::length(path);
	for (size_t i = 0; i < pathSize; ++i)
		if ((CharType)'/' == path[i])
			path[i] = (CharType)'\\';
	path += pathSize - 1;
	while ((CharType)'\\' == *path)
		--path;
	return path;
	// return path.replace('/', '\\').ensure_not_delimiter();
}
template<class CharType>
std::basic_string<CharType>& normalize(std::basic_string<CharType>& path) { 
	CharType* path_ = const_cast<CharType*>(path.c_str());
	CharType* path_delimiter = normalize(path_);
	path.resize(path_delimiter - path_ + 1);
	return path;
}

// if (::BSTRlength(path) < 2) return E_INVALIDARG;
// formatPath((OLECHAR*)path, ::BSTRlength(path), ... );
bool formatPath(wchar_t const *path, size_t pathSize,
	std::wstring *diskDesignator, std::wstring *realPath, std::wstring *virtualPath) {
	std::wstring path_(path, (pathSize != 0) ? pathSize : std::char_traits<wchar_t>::length(path));

	wchar_t *v = const_cast<wchar_t*>(normalize(path_).c_str()), *real_path(NULL), *virtual_path(NULL);
	pathSize = path_.size();

	if (pathSize < 2)
		return false;
	if ((L':' == v[pathSize - 1]) && (2 == pathSize)) {
		if (diskDesignator) {
			(*diskDesignator).swap(path_);
			(*diskDesignator) += L'\\';
		}
		return true;
	}

	/* isDirectory, empty real path and other special cases must check fileEnumerator, not formatPath function */
	/* ! check isDirectory - if real_path exist && isDirectory log(NOT FOUND); return false - virtual path cannot start at directory */
	for (size_t i = 0; i < pathSize; ++i) {
		if (L'\\' == v[i]) {
			if (!real_path) {
				if ((i) && (L':' == v[i - 1])) {
					if (diskDesignator)
						(*diskDesignator).assign(v, i + 1);
				} else if (((i + 2) < pathSize) && (L'\\' == v[i + 1]) && (L'\\' != v[i + 2])) {
					std::wstring::size_type unc = path_.find('\\', i + 2);
					if (unc != std::wstring::npos) {
						i = static_cast<size_t>(unc);
						if (diskDesignator)
							(*diskDesignator).assign(v, i + 1);
					} else {
						if (diskDesignator) {
							(*diskDesignator).swap(path_);
							(*diskDesignator) += L'\\';
						}
						return true;
					}
				} else
					return false;
				
				real_path = v + i;
				if (exist(v)) {
					if (realPath)
						(*realPath) = path_;
					return true;
				}
			} else {
				v[i] = 0;
				if (!exist(v)) {
					virtual_path = real_path + 1;
				} else {
					real_path = v + i;
				}
				v[i] = L'\\';

				if (virtual_path)
					break;
			}
		}
	}

	if ((!virtual_path) && ((real_path - v + 1) < static_cast<ptrdiff_t>(pathSize)))
		virtual_path = real_path + 1;

	if (!real_path)
		return false;
	if (realPath)
		(*realPath).assign(v, real_path - v);
	if (virtual_path && virtualPath)
		(*virtualPath) = virtual_path;
	return true;
}

} // namespace files
