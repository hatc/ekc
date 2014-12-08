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
#include <memory> // std::auto_ptr<>

#include <windows.h>

#include "file_stream.h"
#include "trace_helpers.hpp"
#include "file_util.h"
#include "trace.h"

/*FileStream
Read():
 precondition: size > 0, file pointer at EOF
 postcondition: state = true, Read() return 0

 precondition: size > 0, file pointer beyond EOF
 postcondition: state = true, Read() return 0

Write():
 precondition: file opened with read-only access
 postcondition: state = false, Write() return 0

 precondition: size > 0, file pointer beyond EOF
 postcondition: state = true, Write() return number of bytes written, intervening bytes(between EOF and file pointer before Write() called) in file uninitialized

Seek():
 precondition: file pointer valid, move_to beyond EOF
 postcondition: state = true, file pointer beyond EOF, return true

 precondition: file pointer valid, move_to before begin of file, i.e. move_to < 0 && SEEK_SET
 postcondition: state = true, file pointer unchanged, return false
*/

namespace cpcl {

FileStream::FileStream() : state(false), hFile(INVALID_HANDLE_VALUE) {}

FileStream::~FileStream() {
	if (hFile != INVALID_HANDLE_VALUE)
		::CloseHandle(hFile);
}

static unsigned long const CHUNK_SIZE_MAX = (1 << 22);
bool FileStream::ReadPart(void *data, unsigned long size, unsigned long &processed_size) {
	if (size > CHUNK_SIZE_MAX)
		size = CHUNK_SIZE_MAX;
	DWORD processed_size_ = 0;
	bool r = (::ReadFile(hFile, data, size, &processed_size_, NULL) != FALSE);
	if (!r) {
		/*boost::scoped_array<unsigned char> path_cp1251;
		ErrorSystem(::GetLastError(), "FileStream::ReadPart('%s'): ReadFile fails:", UTF16_CP1251_(Path(), path_cp1251));*/
		ErrorSystem(::GetLastError(), "FileStream::ReadPart('%s'): ReadFile fails:", ConvertUTF16_CP1251(Path()).c_str());
	}
	processed_size = (unsigned long)processed_size_;
	return r;
}
bool FileStream::WritePart(void const *data, unsigned long size, unsigned long &processed_size) {
	if (size > CHUNK_SIZE_MAX)
		size = CHUNK_SIZE_MAX;
	DWORD processed_size_ = 0;
	bool r = (::WriteFile(hFile, data, size, &processed_size_, NULL) != FALSE);
	if (!r) {
		/*boost::scoped_array<unsigned char> path_cp1251;
		ErrorSystem(::GetLastError(), "FileStream::WritePart('%s'): WriteFile fails:", UTF16_CP1251_(Path(), path_cp1251));*/
		ErrorSystem(::GetLastError(), "FileStream::WritePart('%s'): WriteFile fails:", ConvertUTF16_CP1251(Path()).c_str());
	}
	processed_size = (unsigned long)processed_size_;
	return r;
}

unsigned long FileStream::Read(void *data, unsigned long size) {
	unsigned long r = 0;
	do {
		unsigned long processed_size = 0;
		state = ReadPart(data, size, processed_size);
		r += processed_size;
		if (!state || (0 == processed_size))
			break;
		data = (void *)((unsigned char *)data + processed_size);
		size -= processed_size;
	} while (size > 0);
	return r;
}

unsigned long FileStream::Write(void const *data, unsigned long size) {
	unsigned long r = 0;
	do {
		unsigned long processed_size = 0;
		state = WritePart(data, size, processed_size);
		r += processed_size;
		if (!state || (0 == processed_size))
			break;
		data = (void const *)((unsigned char const *)data + processed_size);
		size -= processed_size;
	} while (size > 0);
	return r;
}

bool FileStream::Seek(__int64 move_to, unsigned long move_method, __int64 *position) {
	LARGE_INTEGER v;
	v.QuadPart = move_to;
	v.LowPart = ::SetFilePointer(hFile, v.LowPart, &v.HighPart, move_method);
	if (INVALID_SET_FILE_POINTER == v.LowPart) {
		unsigned long const error_code = ::GetLastError();
		if (error_code != NO_ERROR) {
			/*boost::scoped_array<unsigned char> path_cp1251;
			ErrorSystem(error_code, "FileStream::Seek('%s'): SetFilePointer(move_to = %u, move_method = %d) fails:",
				UTF16_CP1251_(Path(), path_cp1251), (unsigned int)move_to, move_method);*/
			ErrorSystem(error_code, "FileStream::Seek('%s'): SetFilePointer(move_to = %u, move_method = %d) fails:",
				ConvertUTF16_CP1251(Path()).c_str(), (unsigned int)move_to, move_method);
			return false;
		}
	}

	if (position)
		*position = v.QuadPart;
	return true;
}

__int64 FileStream::Size() {
	LARGE_INTEGER v;
	state = (::GetFileSizeEx(hFile, &v) != FALSE);
	if (!state) {
		/*boost::scoped_array<unsigned char> path_cp1251;
		ErrorSystem(::GetLastError(), "FileStream::Size('%s'): GetFileSizeEx fails:", UTF16_CP1251_(Path(), path_cp1251));*/
		ErrorSystem(::GetLastError(), "FileStream::Size('%s'): GetFileSizeEx fails:", ConvertUTF16_CP1251(Path()).c_str());
		v.QuadPart = -1;
	}
	return v.QuadPart;

	/*ULARGE_INTEGER v;
	v.LowPart = ::GetFileSize(hFile, &v.HighPart);
	if (INVALID_FILE_SIZE == v.LowPart) {
		unsigned long const error_code = ::GetLastError();
		if (error_code != NO_ERROR) {
			// Trace();
			v.QuadPart = (unsigned __int64)-1;
		}
	}

	return (__int64)v.QuadPart;*/
}

IOStream* FileStream::Clone() {
	FileStream *r;
	if (!FileStream::FileStreamCreate(Path(),
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, &r))
		r = NULL;
	else
		r->Seek(Tell(), FILE_BEGIN, NULL);
	return r;
}

static WStringPiece const prefix = WStringPieceFromLiteral(L"\\\\?\\");
std::wstring FileStream::ExpandPath(cpcl::WStringPiece const &v) {
	std::wstring r;
	if (!v.starts_with(prefix))
		r.reserve(v.size() + prefix.size() + 1);
	r.assign(v.data(), v.size());
	FileStream::ExpandPath(&r);
	return r;
}
//void FileStream::ExpandPath(cpcl::WStringPiece const &v, std::wstring *path) {
//	std::wstring r;
//	if (!v.starts_with(prefix))
//		r.reserve(v.size() + prefix.size() + 1);
//	r.assign(v.data(), v.size());
//	FileStream::ExpandPath(&r);
//	if (path)
//		(*path).swap(r);
//}
void FileStream::ExpandPath(std::wstring *v) {
	if (v && !WStringPiece(NormalizePath(*v)).starts_with(prefix))
		v->insert(0, prefix.data(), prefix.size());
}

cpcl::WStringPiece FileStream::Path() const {
	cpcl::WStringPiece r = path;
	if (r.starts_with(prefix) && (r.size() > prefix.size()))
		r = cpcl::WStringPiece(r.data() + prefix.size(), r.size() - prefix.size());
	return r;
}

/*unsigned long FileStream::FileStreamCreate() : returns GetLastError()*/
bool FileStream::FileStreamCreate(cpcl::WStringPiece const &path,
		unsigned long access_mode, unsigned long share_mode,
		unsigned long disposition, unsigned long attributes,
		FileStream **v) {
	std::auto_ptr<FileStream> r(new FileStream());
	/*std::wstring path_ = FileStream::ExpandPath(path);
	in Release with RVO: only one ctor call*/
	std::wstring path_ = FileStream::ExpandPath(path);
	HANDLE const hFile = ::CreateFileW(path_.c_str(), access_mode, share_mode,
			(LPSECURITY_ATTRIBUTES)NULL, disposition, attributes, (HANDLE)NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	r->hFile = hFile;
	r->path.swap(path_);
	r->state = true;
	if (v)
		*v = r.release();
	return true;
}

bool FileStream::Create(cpcl::WStringPiece const &path, FileStream **v) {
	unsigned long const access_mode = GENERIC_READ | GENERIC_WRITE,
		share_mode = FILE_SHARE_READ,
		disposition = CREATE_NEW,
		attributes = FILE_ATTRIBUTE_NORMAL;
	if (!FileStream::FileStreamCreate(path,
		access_mode, share_mode, disposition, attributes, v)) {
		/*boost::scoped_array<unsigned char> path_cp1251;
		ErrorSystem(::GetLastError(), "FileStream::Create('%s'): CreateFileW fails:", UTF16_CP1251_(path, path_cp1251));*/
		ErrorSystem(::GetLastError(), "FileStream::Create('%s'): CreateFileW fails:", ConvertUTF16_CP1251(path).c_str());
		return false;
	}
	return true;
}

bool FileStream::Read(cpcl::WStringPiece const &path, FileStream **v) {
	unsigned long const access_mode = GENERIC_READ,
		share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		disposition = OPEN_EXISTING,
		attributes = FILE_ATTRIBUTE_NORMAL;
	if (!FileStream::FileStreamCreate(path,
		access_mode, share_mode, disposition, attributes, v)) {
		/*boost::scoped_array<unsigned char> path_cp1251;
		ErrorSystem(::GetLastError(), "FileStream::Read('%s'): CreateFileW fails:", UTF16_CP1251_(path, path_cp1251));*/
		ErrorSystem(::GetLastError(), "FileStream::Read('%s'): CreateFileW fails:", ConvertUTF16_CP1251(path).c_str());
		return false;
	}
	return true;
}

bool FileStream::ReadWrite(cpcl::WStringPiece const &path, FileStream **v) {
	unsigned long const access_mode = GENERIC_READ | GENERIC_WRITE,
		share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		disposition = OPEN_EXISTING,
		attributes = FILE_ATTRIBUTE_NORMAL;
	if (!FileStream::FileStreamCreate(path,
		access_mode, share_mode, disposition, attributes, v)) {
		/*boost::scoped_array<unsigned char> path_cp1251;
		ErrorSystem(::GetLastError(), "FileStream::ReadWrite('%s'): CreateFileW fails:", UTF16_CP1251_(path, path_cp1251));*/
		ErrorSystem(::GetLastError(), "FileStream::ReadWrite('%s'): CreateFileW fails:", ConvertUTF16_CP1251(path).c_str());
		return false;
	}
	return true;
}

static WStringPiece const HEAD = WStringPieceFromLiteral(L"^_^");
static WStringPiece const TAIL = WStringPieceFromLiteral(L".tmp");
bool FileStream::CreateTemporary(FileStream **v) {
	unsigned long const access_mode = GENERIC_READ | GENERIC_WRITE,
		share_mode = FILE_SHARE_READ | FILE_SHARE_DELETE,
		disposition = CREATE_NEW,
		attributes = FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE;

	std::wstring path;
	if (!GetTemporaryDirectory(&path))
		return false;
	FileStream::ExpandPath(&path);
	size_t const len = 0x20 + HEAD.size() + TAIL.size() + 1;
	path.append(len, L'\\');
	wchar_t *buf = const_cast<wchar_t*>(path.c_str() + path.size() - len + 1);
	for (size_t i = 0; i < 0x100; ++i) {
		RandName(buf, len, HEAD, TAIL);
		if (FileStream::FileStreamCreate(path,
			access_mode, share_mode, disposition, attributes, v))
			return true;
		
		unsigned long const error_code = ::GetLastError();
		if (error_code != ERROR_FILE_EXISTS) {
			/*boost::scoped_array<unsigned char> path_cp1251;
			ErrorSystem(error_code, "FileStream::CreateTemporary('%s'): CreateFileW fails:", UTF16_CP1251_(path, path_cp1251));*/
			ErrorSystem(error_code, "FileStream::CreateTemporary('%s'): CreateFileW fails:", ConvertUTF16_CP1251(path).c_str());
			return false;
		}
	}
	Trace(CPCL_TRACE_LEVEL_ERROR, "FileStream::CreateTemporary('%s'): too many temporary files, consider remake pseudo random name generator",
		ConvertUTF16_CP1251(path).c_str());
	return false;
}

} // namespace cpcl
