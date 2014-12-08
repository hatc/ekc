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

//#include <boost/scoped_array.hpp>
#ifdef CPCL_USE_BOOST_THREAD_SLEEP
#include <boost/thread/thread.hpp>
#endif

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOGDICAPMASKS
#define NOMETAFILE
#define NOMINMAX
#define NORASTEROPS
#define NOSCROLL
#define NOSOUND
#define NOVIRTUALKEYCODES
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NOSHOWWINDOW
#define NOCLIPBOARD
#define NOCOLOR
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOCTLMGR
#define NODRAWTEXT
#define NOMB
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOCRYPT
#define NOSERVICE
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NOIME
#include <windows.h>
#include <shellapi.h> // SHFileOperation

#include "file_util.h"
#include "trace_helpers.hpp"
#include "trace.h"

namespace cpcl {

//inline char* cp1251_str(WStringPiece const &v, char *r) { std::transform(v.begin(), v.end() + 1, r, cp1251_from_uc); return r; }

//inline boost::shared_array<unsigned char> UTF16_CP1251_array(WStringPiece const &v) {
//	boost::shared_array<unsigned char> path_cp1251(new unsigned char[path.size() + 1]);
//	*UTF16_CP1251(path, path_cp1251.get()) = '\0';
//	return path_cp1251;
//}
//
//ErrorSystem(error_code, "Function('%s'): Win API Function fails:", UTF16_CP1251_array(path).get());

/*inline unsigned char* UTF16_CP1251_(WStringPiece const &path, boost::scoped_array<unsigned char> &path_cp1251) {
	path_cp1251.reset(new unsigned char[path.size() + 1]);
	*UTF16_CP1251(path, path_cp1251.get()) = '\0';
	return path_cp1251.get();
}*/
//ErrorSystem(error_code, "Function('%s'): Win API Function fails:", UTF16_CP1251_(path, path_cp1251));

bool GetTemporaryDirectory(std::wstring *r) {
	wchar_t static_buf[1024];
	boost::scoped_array<wchar_t> dynamic_buf;
	wchar_t *buf = static_buf; /* === &static_buf[0] === &(*static_buf) ;))) */
	DWORD n = ::GetTempPathW(arraysize(static_buf), buf);
	if (n > arraysize(static_buf)) {
		dynamic_buf.reset(new wchar_t[n]);
		buf = dynamic_buf.get();

		n = ::GetTempPathW(n, buf);
	}
	if (0 == n) {
		ErrorSystem(::GetLastError(), "%s: GetTempPathW fails:", "GetTemporaryDirectory()");
		/*if (r)
			*r = L"c:\\temp";*/
		return false;
	}
	if (r) {
		if (L'\\' == buf[n - 1])
			--n;
		(*r).assign(buf, n);
	}
	return true;
}

bool GetModuleFilePath(void *hmodule, std::wstring *r) {
	wchar_t static_buf[1024];
	boost::scoped_array<wchar_t> dynamic_buf;
	wchar_t *buf = static_buf;
	DWORD size = arraysize(static_buf);
	DWORD n = ::GetModuleFileNameW((HMODULE)hmodule, buf, size);
	/*If the function succeeds, then returns path length not including the terminating null character.
	
	If the buffer is too small to hold the module name
	Windows XP: The string is truncated to nSize characters and is not null-terminated. The function returns nSize. ERROR_INSUFFICIENT_BUFFER == GetLastError()
	Vista +   : The string is truncated to nSize characters including the terminating null character. The function returns nSize. ERROR_SUCCESS == GetLastError()*/
	while (n == size) {
		size *= 2;
		dynamic_buf.reset(new wchar_t[size]);
		buf = dynamic_buf.get();

		n = ::GetModuleFileNameW((HMODULE)hmodule, buf, size);
	}
	if (0 == n) {
		ErrorSystem(::GetLastError(), "%s: GetModuleFileNameW fails:", "GetModuleFilePath()");
		return false;
	}
	/*The string returned will use the same format that was specified when the module was loaded. 
	Therefore, the path can be a long or short file name, and can use the prefix "\\?\". 
	
	use GetFullPathName ?*/
	for (; n > 1; --n)
		if (L'\\' != buf[n - 1])
			break;
	for (; n > 0; --n)
		if (L'\\' == buf[n - 1])
			break;

	if (r) {
		if (n > 1 && (L'\\' == buf[n - 1]))
			--n;
		(*r).assign(buf, n);
	}
	return true;
}

/* in CreateTemporaryStream use CreateGuid + CreateFile, that a far more reliable... */
static wchar_t const *TEMPORARY_PREFIX = L"^_^";
bool CreateTemporaryFile(WStringPiece const &folder, std::wstring *r) {
	wchar_t buf[MAX_PATH + 1];
	/* The directory path for the file name cannot be longer than MAX_PATH–14 characters, or GetTempFileName will fail. */
	if (::GetTempFileNameW(folder.data(), TEMPORARY_PREFIX, 0, buf) == 0) {
		unsigned long const error_code = ::GetLastError();
		/*boost::scoped_array<unsigned char> folder_cp1251;*/
		if (error_code == ERROR_BUFFER_OVERFLOW)
			Trace(CPCL_TRACE_LEVEL_ERROR, "CreateTemporaryFile('%s'): The length of the folder path is more than %d characters",
				ConvertUTF16_CP1251(folder).c_str(), (MAX_PATH - 14));
			/*Trace(CPCL_TRACE_LEVEL_ERROR, "CreateTemporaryFile('%s'): The length of the folder path is more than %d characters",
				UTF16_CP1251_(folder, folder_cp1251), (MAX_PATH - 14));*/
		else
			/*ErrorSystem(error_code, "CreateTemporaryFile('%s'): GetTempFileNameW fails:", UTF16_CP1251_(folder, folder_cp1251));*/
			ErrorSystem(error_code, "CreateTemporaryFile('%s'): GetTempFileNameW fails:", ConvertUTF16_CP1251(folder).c_str());
		return false;
	}
	if (r)
		*r = buf;
	return true;
}

bool ExistFilePath(WStringPiece const &path, bool *is_directory) {
	WIN32_FIND_DATAW find_data;
	HANDLE hFindFile = ::FindFirstFileW(path.data(), &find_data);
	if (INVALID_HANDLE_VALUE == hFindFile) {
		unsigned long const error_code = ::GetLastError();
		if (error_code != ERROR_FILE_NOT_FOUND) {
			/*boost::scoped_array<unsigned char> path_cp1251;
			ErrorSystem(error_code, "ExistFilePath('%s'): FindFirstFileW fails:", UTF16_CP1251_(path, path_cp1251));*/
			ErrorSystem(error_code, "ExistFilePath('%s'): FindFirstFileW fails:", ConvertUTF16_CP1251(path).c_str());
		}
		return false;
	}
	if (is_directory)
		*is_directory = ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
	::FindClose(hFindFile);
	return true;
}

bool RemoveReadOnlyAttribute(WStringPiece const &path) {
	DWORD attrs = ::GetFileAttributesW(path.data());
	if (INVALID_FILE_ATTRIBUTES == attrs) {
		/*boost::scoped_array<unsigned char> path_cp1251;
		ErrorSystem(::GetLastError(), "RemoveReadOnlyAttribute('%s'): GetFileAttributesW fails:", UTF16_CP1251_(path, path_cp1251));*/
		ErrorSystem(::GetLastError(), "RemoveReadOnlyAttribute('%s'): GetFileAttributesW fails:", ConvertUTF16_CP1251(path).c_str());
		return false;
	}

	if (attrs & FILE_ATTRIBUTE_READONLY) {
		attrs ^= FILE_ATTRIBUTE_READONLY;
		if (::SetFileAttributes(path.data(), attrs) == FALSE) {
			/*boost::scoped_array<unsigned char> path_cp1251;
			ErrorSystem(::GetLastError(), "RemoveReadOnlyAttribute('%s'): SetFileAttributes fails:", UTF16_CP1251_(path, path_cp1251));*/
			ErrorSystem(::GetLastError(), "RemoveReadOnlyAttribute('%s'): SetFileAttributes fails:", ConvertUTF16_CP1251(path).c_str());
			return false;
		}
	}
	return true;
}

bool DeleteFilePath(WStringPiece const &path) {
	bool is_directory;
	if (!ExistFilePath(path, &is_directory))
		return false;
	/*****
	To delete a read-only file, first you must remove the read-only attribute. 
	To delete or rename a file, you must have either delete permission on the file, or delete child permission in the parent directory. 
	To recursively delete the files in a directory, use the SHFileOperation function. 
	To remove an empty directory, use the RemoveDirectory function. 
	*****/
	if (is_directory) {
		SHFILEOPSTRUCTW sh_operations; memset(&sh_operations, 0, sizeof(sh_operations));
		size_t size = path.size() + 5;
		if (*(path.end() - 1) != L'\\')
			++size;
		wchar_t *buf = (wchar_t*)::LocalAlloc(LMEM_ZEROINIT, size * sizeof(wchar_t));
		if (!buf) {
			/*boost::scoped_array<unsigned char> path_cp1251;
			ErrorSystem(::GetLastError(), "DeleteFilePath('%s'): LocalAlloc(%d) fails:",
				UTF16_CP1251_(path, path_cp1251), size * sizeof(wchar_t));*/
			ErrorSystem(::GetLastError(), "DeleteFilePath('%s'): LocalAlloc(%d) fails:",
				ConvertUTF16_CP1251(path).c_str(), size * sizeof(wchar_t));
			return false;
		}
		
		wmemcpy(buf, path.begin(), path.size());
		buf[size - 2] = L'*';
		buf[size - 3] = L'.';
		buf[size - 4] = L'*';
		buf[size - 5] = L'\\';
		
		sh_operations.pFrom = buf;
		sh_operations.wFunc = FO_DELETE;
		sh_operations.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;

		DWORD dw = ::GetLastError();
		/* Do not use GetLastError with the return values of this function. */
		bool res = ::SHFileOperationW(&sh_operations) == 0; // use IFileOperation
		//***** shOperations.hwnd == 0 => ERROR_INVALID_HANDLE *****//
		::SetLastError(dw);
		
		::LocalFree((HLOCAL)buf);
		if (res) {
			if (::RemoveDirectoryW(path.data()) != FALSE) // RemoveDirectoryW work for both cases: (path || path + '\\')
				return true;
			else {
				/*boost::scoped_array<unsigned char> path_cp1251;
				ErrorSystem(::GetLastError(), "DeleteFilePath('%s'): RemoveDirectoryW fails:", UTF16_CP1251_(path, path_cp1251));*/
				ErrorSystem(::GetLastError(), "DeleteFilePath('%s'): RemoveDirectoryW fails:", ConvertUTF16_CP1251(path).c_str());
			}
		} else {
			/*boost::scoped_array<unsigned char> path_cp1251;
			Trace(CPCL_TRACE_LEVEL_ERROR, "DeleteFilePath('%s'): SHFileOperationW fails:", UTF16_CP1251_(path, path_cp1251));*/
			Trace(CPCL_TRACE_LEVEL_ERROR, "DeleteFilePath('%s'): SHFileOperationW fails:", ConvertUTF16_CP1251(path).c_str());
		}
	} else {
		// replace witn inline deleteFile() { if ::DeleteFileW() == FALSE; return ::DeleteFileW() != FALSE } - file can be unaccesible due function call but then become again online
		if (RemoveReadOnlyAttribute(path)) {
			if (::DeleteFileW(path.data()) != FALSE)
				return true;
#ifdef CPCL_USE_BOOST_THREAD_SLEEP
			boost::this_thread::sleep(boost::posix_time::milliseconds(0x100)); /* yaaaaaaaawn ^_^ */
#else
			::SleepEx(0x100, TRUE); /* yaaaaaaaawn ^_^ */
#endif
			if (::DeleteFileW(path.data()) != FALSE)
				return true;
			
			/*boost::scoped_array<unsigned char> path_cp1251;
			ErrorSystem(::GetLastError(), "DeleteFilePath('%s'): DeleteFileW fails:", UTF16_CP1251_(path, path_cp1251));*/
			ErrorSystem(::GetLastError(), "DeleteFilePath('%s'): DeleteFileW fails:", ConvertUTF16_CP1251(path).c_str());
		}
	}

	return false;
}

#if 0

bool GetVirtualPathComponents(WStringPiece const &path,
	std::wstring *disk_designator, std::wstring *real_path, std::wstring *virtual_path) {
	std::wstring path_ = path.as_string();

	wchar_t *v = const_cast<wchar_t*>(NormalizePath(path_).c_str()), *real_path_(NULL), *virtual_path_(NULL);
	size_t path_size = path_.size();

	if (path_size < 2) {
		Trace(CPCL_TRACE_LEVEL_ERROR, "GetPathComponents(%s) fail: path length is less than 2 characters",
			path.data());
		return false;
	}
	if ((L':' == v[path_size - 1]) && (2 == path_size)) {
		if (disk_designator) {
			(*disk_designator).swap(path_);
			(*disk_designator) += L'\\';
		}
		return true;
	}

	/* isDirectory, empty real path and other special cases must check fileEnumerator, not formatPath function */
	/* ! check isDirectory - if real_path exist && isDirectory log(NOT FOUND); return false - virtual path cannot start at directory */
	for (size_t i = 0; i < path_size; ++i) {
		if (L'\\' == v[i]) {
			if (!real_path_) {
				if ((i) && (L':' == v[i - 1])) {
					if (disk_designator)
						(*disk_designator).assign(v, i + 1);
				} else if (((i + 2) < path_size) && (L'\\' == v[i + 1]) && (L'\\' != v[i + 2])) {
					std::wstring::size_type unc = path_.find('\\', i + 2);
					if (unc != std::wstring::npos) {
						i = static_cast<size_t>(unc);
						if (disk_designator)
							(*disk_designator).assign(v, i + 1);
					} else {
						if (disk_designator) {
							(*disk_designator).swap(path_);
							(*disk_designator) += L'\\';
						}
						return true;
					}
				} else
					return false;
				
				real_path_ = v + i;
				if (exist(v)) {
					if (real_path)
						(*real_path) = path_;
					return true;
				}
			} else {
				v[i] = 0;
				if (!exist(v)) {
					virtual_path_ = real_path_ + 1;
				} else {
					real_path_ = v + i;
				}
				v[i] = L'\\';

				if (virtual_path_)
					break;
			}
		}
	}

	if ((!virtual_path_) && ((real_path_ - v + 1) < path_size))
		virtual_path_ = real_path_ + 1;

	if (!real_path_)
		return false;
	if (real_path)
		(*real_path).assign(v, real_path_ - v);
	if (virtual_path_ && virtual_path)
		(*virtual_path) = virtual_path_;
	return true;
}

#endif

size_t AvailableDiskSpaceMib(WStringPiece const &path) {
	ULARGE_INTEGER freeBytesAvailable;
	if (::GetDiskFreeSpaceExW(path.data(), &freeBytesAvailable, NULL, NULL) == FALSE) {
		/*boost::scoped_array<unsigned char> path_cp1251;
		ErrorSystem(::GetLastError(), "AvailableDiskSpaceMib('%s'): GetDiskFreeSpaceExW fails:", UTF16_CP1251_(path, path_cp1251));*/
		ErrorSystem(::GetLastError(), "AvailableDiskSpaceMib('%s'): GetDiskFreeSpaceExW fails:", ConvertUTF16_CP1251(path).c_str());
		return 0;
	}

	return (size_t)(freeBytesAvailable.QuadPart >> 20);
}

} // namespace cpcl
