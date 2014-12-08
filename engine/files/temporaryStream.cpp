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
#include "stdafx.h"

#include <file_stream.h>
#include <file_util.h>
#include <impl_exception_helper.hpp>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include <istream_impl.h>

#include "temporaryStream.h"

namespace files {

HRESULT fileStream(const wchar_t *path, IStream **v) {
	try {
		cpcl::FileStream *stream_;
		if (!cpcl::FileStream::ReadWrite(path, &stream_))
			return E_FAIL;
		
		ScopedComPtr<IStream> stream(new cpcl::IStreamImpl(stream_, true)); // stream_ object leak if new IStreamImpl throw due to low memory
		if (v)
			*v = stream.Detach();
		return S_OK;
	} CATCH_EXCEPTION("fileStream()")
}

HRESULT fileStreamRead(const wchar_t *path, IStream **v) {
	try {
		cpcl::FileStream *stream_;
		if (!cpcl::FileStream::Read(path, &stream_))
			return E_FAIL;
		
		ScopedComPtr<IStream> stream(new cpcl::IStreamImpl(stream_, true)); // stream_ object leak if new IStreamImpl throw due to low memory
		if (v)
			*v = stream.Detach();
		return S_OK;
	} CATCH_EXCEPTION("fileStreamRead()")
}

HRESULT fileStreamCreate(const wchar_t *path, IStream **v) {
	try {
		cpcl::FileStream *stream_;
		if (!cpcl::FileStream::Create(path, &stream_))
			return E_FAIL;
		
		ScopedComPtr<IStream> stream(new cpcl::IStreamImpl(stream_, true)); // stream_ object leak if new IStreamImpl throw due to low memory
		if (v)
			*v = stream.Detach();
		return S_OK;
	} CATCH_EXCEPTION("fileStreamCreate()")
}

// Specify the width of the field in which to print the numbers. 
// The asterisk in the format specifier "%*I64d" takes an integer 
// argument and uses it to pad and right justify the number.
/*#define WIDTH 7*/
static size_t availablePhysicalMemoryMib(void) {
	MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);

	if (!::GlobalMemoryStatusEx(&statex))
		return 0;

	/*_tprintf (TEXT("There are %*I64d free Mibs of physical memory.\n"),
            WIDTH, (statex.ullAvailPhys >> 20));
	_tprintf (TEXT("There are %*I64d free Mibs of virtual memory.\n"),
            WIDTH, (statex.ullAvailVirtual >> 20));*/

	return (size_t)(statex.ullAvailPhys >> 20);
}

static size_t availableDiskSpaceMib(const std::wstring &folder) {
	ULARGE_INTEGER freeBytesAvailable;
	if (!::GetDiskFreeSpaceExW(folder.c_str(), &freeBytesAvailable, NULL, NULL))
		return 0;

	return (size_t)(freeBytesAvailable.QuadPart >> 20);
}

HRESULT temporaryStream(unsigned __int64 estimatedSizeByte/* estimatedSizeMiB && check free memory && disk space */, IStream **v) {
	try {
		const size_t estimatedSizeMiB = (estimatedSizeByte > 0) ? size_t(estimatedSizeByte >> 20) : 64;
		
		//if (availablePhysicalMemoryMib() > (estimatedSizeMiB + 256)) {
		if (estimatedSizeMiB < 1) {
			ScopedComPtr<IStream> stream;
			RINOK(::CreateStreamOnHGlobal(NULL, TRUE, &stream))
			*v = stream.Detach();
			return S_OK;
		}
		
		std::wstring folder;
		if (!cpcl::GetTemporaryDirectory(&folder))
			return E_FAIL;
		if ((estimatedSizeMiB + 512) > availableDiskSpaceMib(folder))
			return STG_E_MEDIUMFULL;

		cpcl::FileStream *stream_;
		if (!cpcl::FileStream::CreateTemporary(&stream_))
			return E_FAIL;
		ScopedComPtr<IStream> stream(new cpcl::IStreamImpl(stream_, true)); // stream_ object leak if new IStreamImpl throw due to low memory
		if (v)
			*v = stream.Detach();
		return S_OK;
	} CATCH_EXCEPTION("temporaryStream()")
}

HRESULT temporaryFileStream(unsigned __int64 estimatedSizeByte, IStream **v) {
	try {
		if (estimatedSizeByte > 0) {
			const size_t estimatedSizeMiB = size_t(estimatedSizeByte >> 20);
			
			std::wstring folder;
			if (!cpcl::GetTemporaryDirectory(&folder))
				return E_FAIL;
			if ((estimatedSizeMiB + 256) > availableDiskSpaceMib(folder))
				return STG_E_MEDIUMFULL;
		}

		cpcl::FileStream *stream_;
		if (!cpcl::FileStream::CreateTemporary(&stream_))
			return E_FAIL;
		ScopedComPtr<IStream> stream(new cpcl::IStreamImpl(stream_, true)); // stream_ object leak if new IStreamImpl throw due to low memory
		if (v)
			*v = stream.Detach();
		return S_OK;
	} CATCH_EXCEPTION("temporaryFileStream()")
}

} // namespace files

#if 0

#include "temporaryStream.h"

#include <fstream>

#include <atlbase.h>
#include <windows.h>

#include "fileUtil.h"
#include "internalCom.h"

namespace files {

//static bool getTemporaryDirectory(std::wstring *r) {
//	wchar_t buffer[0x1000] = { 0 };
//	if (!::GetTempPathW(sizeof(buffer)/sizeof(*buffer), &buffer[0])) {
//		if (r) 
//			*r = L"c:\\temp";
//		return false;
//	}
//	if (r)
//		*r = &buffer[0];
//	return true;
//}
//static const wchar_t *TEMPORARY_PREFIX = L"^_^";
//static bool createTemporaryFile(const std::wstring &folder, std::wstring *r) {
//	wchar_t buffer[MAX_PATH + 1] = { 0 };
//	//***** The directory path for the file name cannot be longer than MAX_PATH–14 characters, or GetTempFileName will fail. *****//
//	if (!::GetTempFileNameW(folder.c_str(), TEMPORARY_PREFIX, 0, &buffer[0]))
//		return false;
//	if (r)
//		*r = &buffer[0];
//	return true;
//}
//
//static bool exist(const std::wstring &path, bool *isDirectory = 0) {
//	WIN32_FIND_DATAW findData;
//	HANDLE searchHandle = ::FindFirstFileW(path.c_str(), &findData);
//	bool r = (searchHandle != INVALID_HANDLE_VALUE);
//	if (r) {
//		if (isDirectory)
//			*isDirectory = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
//		::FindClose(searchHandle);
//	}
//	return r;
//}
//
//static bool _getFileAttributes(const std::wstring &path, DWORD &attrs) {
//	attrs = ::GetFileAttributesW(path.c_str());
//	return attrs != INVALID_FILE_ATTRIBUTES;
//}
//static bool _setFileAttributes(const std::wstring &path, DWORD attrs) {
//	return ::SetFileAttributes(path.c_str(), attrs) != FALSE;
//}
//static bool removeReadOnlyAttribute(const std::wstring &path) {
//	DWORD attrs;
//	if (!_getFileAttributes(path, attrs))
//		return false;
//
//	if (attrs & FILE_ATTRIBUTE_READONLY) {
//		attrs ^= FILE_ATTRIBUTE_READONLY;
//		return _setFileAttributes(path, attrs);
//	}
//	return true;
//}
//
//static bool deleteFile(const std::wstring &path) {
//	bool isDirectory;
//	if (!exist(path, &isDirectory))
//		return false;
//	/*****
//	To delete a read-only file, first you must remove the read-only attribute. 
//	To delete or rename a file, you must have either delete permission on the file, or delete child permission in the parent directory. 
//	To recursively delete the files in a directory, use the SHFileOperation function. 
//	To remove an empty directory, use the RemoveDirectory function. 
//	*****/
//	if (isDirectory) {
//		SHFILEOPSTRUCT shOperations; ZeroMemory(&shOperations, sizeof(shOperations));
//		SIZE_T kroxa(5); if ((*--path.end()) != L'\\') ++kroxa;
//		_TCHAR *fromBuffer = (_TCHAR*)::LocalAlloc(LMEM_ZEROINIT, (path.size()+kroxa)*sizeof(_TCHAR));
//		if (fromBuffer) {
//			lstrcpyn(fromBuffer, path.c_str(), path.size()+1);
//			fromBuffer[path.size()+kroxa-3] = TEXT('*'); 
//			fromBuffer[path.size()+kroxa-4] = TEXT('.'); 
//			fromBuffer[path.size()+kroxa-5] = TEXT('*'); 
//			fromBuffer[path.size()+kroxa-6] = TEXT('\\');
//
//			shOperations.pFrom = &fromBuffer[0];
//			shOperations.wFunc = FO_DELETE;
//			shOperations.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;
//
//			DWORD dw = GetLastError();
//			bool res = ::SHFileOperation(&shOperations) == 0;
//			//***** shOperations.hwnd == 0 => ERROR_INVALID_HANDLE *****//
//			SetLastError(dw);
//
//			::LocalFree(fromBuffer);
//			if (res)
//				return ::RemoveDirectoryW(path.c_str()) != FALSE;
//		}
//	} else {
//		if (removeReadOnlyAttribute(path)) {
//			return ::DeleteFileW(path.c_str()) != FALSE;
//		}
//	}
//
//	return false;
//}

//class temporaryFileStream : public IStream, public CMyUnknownImp {
class basicFileStream : public IStream, public CMyUnknownImp {
public:
	MY_UNKNOWN_IMP1_MT(IStream)

	STDMETHOD(Read)(void* pv, ULONG cb, ULONG* pcbRead) {
		COM_TRY_BEGIN
			
		if (INVALID_HANDLE_VALUE == handle_)
			return E_FAIL;
		if (pcbRead)
			*pcbRead = 0;
		unsigned int processedSize;
		if (!read(pv, (unsigned int)cb, processedSize))
			return E_FAIL;
		if (pcbRead)
			*pcbRead = (ULONG)processedSize;
		return S_OK;
		
		COM_TRY_END
	}
	STDMETHOD(Write)(const void* pv, ULONG cb, ULONG* pcbWritten) {
		COM_TRY_BEGIN
		
		if (INVALID_HANDLE_VALUE == handle_)
			return E_FAIL;
		if (pcbWritten) 
			*pcbWritten = 0;
		unsigned int processedSize;
		if (!write(pv, (unsigned int)cb, processedSize))
			return E_FAIL;
		if (pcbWritten)
			*pcbWritten = (ULONG)processedSize;
		return S_OK;
		
		COM_TRY_END
	}
  STDMETHOD(Seek)(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition) {
		COM_TRY_BEGIN

		if (INVALID_HANDLE_VALUE == handle_)
			return E_FAIL;
		if (dwOrigin > 2)
			return STG_E_INVALIDFUNCTION; // switch(dwOrigin) { case STREAM_SEEK_SET: ... break; case STREAM_SEEK_CUR: ... break; case STREAM_SEEK_END: ... break; default: return STG_E_INVALIDFUNCTION; }
    if (plibNewPosition)
			(*plibNewPosition).QuadPart = 0;
		unsigned __int64 newPosition;
    if (!seek(dlibMove.QuadPart, dwOrigin, newPosition))
			return E_FAIL;
    if (plibNewPosition)
			(*plibNewPosition).QuadPart = newPosition;
		return S_OK;
		
		COM_TRY_END
	}
	STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize) {
		COM_TRY_BEGIN

		if (INVALID_HANDLE_VALUE == handle_)
			return E_FAIL;
    unsigned __int64 length(libNewSize.QuadPart);
		return setLength(length) ? S_OK : E_FAIL;
		
		COM_TRY_END
  }
	STDMETHOD(CopyTo)(IStream *pstm, // A pointer to the destination stream. The stream pointed to by pstm can be a new stream or a clone of the source stream.
    ULARGE_INTEGER cb, // The number of bytes to copy from the source stream.
		ULARGE_INTEGER* pcbRead, // A pointer to the location where this method writes the actual number of bytes read from the source. You can set this pointer to NULL. In this case, this method does not provide the actual number of bytes read.
		ULARGE_INTEGER* pcbWritten) { // A pointer to the location where this method writes the actual number of bytes written to the destination. You can set this pointer to NULL. In this case, this method does not provide the actual number of bytes written.
    COM_TRY_BEGIN

		if (INVALID_HANDLE_VALUE == handle_)
			return E_FAIL;
		/*This method is equivalent to reading cb bytes into memory using ISequentialStream::Read and then immediately 
		writing them to the destination stream using ISequentialStream::Write, although IStream::CopyTo will be more efficient.
		The destination stream can be a clone of the source stream created by calling the IStream::Clone method.*/

		HRESULT hr(S_OK);
		unsigned char buf[0x1000];
		unsigned __int64 toRead = cb.QuadPart;
		unsigned int read = min((unsigned int)toRead, sizeof(buf));
		unsigned int readed_;
		ULONG readed, written;
		unsigned __int64 totalRead(0), totalWritten(0);
		while (readPart(buf, read, readed_)) {
			if (!readed_)
				break;
			readed = readed_;
			totalRead += readed;

			if ((hr = pstm->Write(buf, readed, &written)) != S_OK)
				break;
			totalWritten += written;

			toRead -= readed;
			if (!toRead)
				break;
			read = min((unsigned int)toRead, sizeof(buf));
		}

		if (pcbWritten) {
        pcbWritten->QuadPart = totalWritten;
    }
    if (pcbRead) {
        pcbRead->QuadPart = totalRead;
    }

		/*{
			std::ofstream out("C:\\Source\\tmp\\comp-sdt-rc2.0exe\\bin\\Release\\gdip.log", std::ios_base::out | std::ios_base::app);
			out << "CopyTo()\n\tto copy: " << cb.QuadPart << std::endl;
			out << "\ttotalRead: " << totalRead << std::endl;
			out << "\ttotalWritten: " << totalWritten << std::endl;
		}*/

		return hr;
    /*ULONG written;
    ULONG read = min(cb.LowPart, (ULONG)(m_buffer->size()-m_pos));
    HRESULT hr = pstm->Write(m_buffer->data()+m_pos, read, &written);
    if (pcbWritten) {
        pcbWritten->HighPart = 0;
        pcbWritten->LowPart = written;
    }
    if (pcbRead) {
        pcbRead->HighPart = 0;
        pcbRead->LowPart = read;
    }

    return hr;*/

    COM_TRY_END
  }
  STDMETHOD(Commit)(DWORD /*grfCommitFlags*/) { return S_OK; }
  STDMETHOD(Revert)(void) { return S_OK; }
  STDMETHOD(LockRegion)(/* [in] */ ULARGE_INTEGER /*libOffset*/,
        /* [in] */ ULARGE_INTEGER /*cb*/,
        /* [in] */ DWORD /*dwLockType*/) { return S_OK; }
  //    /* [in] */ DWORD /*dwLockType*/) { return STG_E_INVALIDFUNCTION; }
  STDMETHOD(UnlockRegion)(/* [in] */ ULARGE_INTEGER /*libOffset*/,
        /* [in] */ ULARGE_INTEGER /*cb*/,
        /* [in] */ DWORD /*dwLockType*/) { return S_OK; }
  //    /* [in] */ DWORD /*dwLockType*/) { return STG_E_INVALIDFUNCTION; }
  STDMETHOD(Stat)(/* [out] */ STATSTG* pstatstg,
        /* [in] */ DWORD /*grfStatFlag*/) {
    COM_TRY_BEGIN

		if (INVALID_HANDLE_VALUE == handle_)
			return E_FAIL;
    if (!pstatstg)
			return STG_E_INVALIDPOINTER;
		unsigned __int64 length;
    if (!getLength(length))
			return E_FAIL;

    memset(pstatstg, 0, sizeof(STATSTG));
    pstatstg->type = STGTY_STREAM;
    pstatstg->cbSize.QuadPart = length;
		pstatstg->clsid = CLSID_NULL;
		pstatstg->grfLocksSupported = LOCK_WRITE;

    return S_OK;

    COM_TRY_END
  }
  STDMETHOD(Clone)(/* [out] */ IStream** ppstm) {
		COM_TRY_BEGIN

		if (INVALID_HANDLE_VALUE == handle_)
			return E_FAIL;
		if (!ppstm)
			return STG_E_INVALIDPOINTER;
		// file open with FILE_ATTRIBUTE_TEMPORARY flag - all data can be not flushed to disk
		//::FlushFileBuffers(handle_);
		HANDLE handle = ::CreateFileW(path.c_str(),
			cloneAccessMode_,
			cloneShareMode_,
			(LPSECURITY_ATTRIBUTES)NULL,
			OPEN_EXISTING, // When opening an existing file(dwCreationDisposition set to OPEN_EXISTING)
			FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL); // all FILE_ATTRIBUTE_* specified by dwFlagsAndAttributes ignored
		if (INVALID_HANDLE_VALUE == handle)
			return E_FAIL;
			/*{
			std::ofstream out("C:\\Source\\tmp\\comp-sdt-rc2.0exe\\bin\\Release\\gdip.log", std::ios_base::out | std::ios_base::app);
			out << "\tINVALID_HANDLE_VALUE == handle" << std::endl;
			}*/
		
		CMyComPtr<basicFileStream> stream(new basicFileStream(handle, cloneAccessMode_, cloneShareMode_));
		stream->path = path;

		unsigned __int64 newPosition;
		seek(0, FILE_CURRENT, newPosition);
		stream->seek(static_cast<__int64>(newPosition), FILE_BEGIN, newPosition);

		//{
		//	unsigned char data[0x1024], data_[0x1024];
		//	unsigned __int64 p;
		//	seek(0, FILE_BEGIN, p);
		//	stream->seek(0, FILE_BEGIN, p);
		//	unsigned int a,b;
		//	read(data, sizeof(data), a);
		//	stream->read(data_, sizeof(data), b);
		//	while (a > 0) {
		//		read(data, sizeof(data), a);
		//		stream->read(data_, sizeof(data_), b);
		//	}

		//	seek(static_cast<__int64>(newPosition), FILE_BEGIN, p);
		//	stream->seek(static_cast<__int64>(newPosition), FILE_BEGIN, p);
		//}

		//{
		//	unsigned char data[0x1024], data_[0x1024];
		//	unsigned __int64 p;
		//	seek(0, FILE_BEGIN, p);
		//	stream->seek(0, FILE_BEGIN, p);
		//	unsigned int a,b;
		//	read(data, sizeof(data), a);
		//	stream->read(data_, sizeof(data), b);
		//	while (a > 0) {
		//		if (a == b) {
		//			if (memcmp(data, data_, a) != 0) {
		//				std::ofstream out("C:\\Source\\tmp\\comp-sdt-rc2.0exe\\bin\\Release\\gdip.log", std::ios_base::out | std::ios_base::app);
		//				out << "\tdata in cloned file stream not equal" << std::endl;
		//			}
		//		} else {
		//			std::ofstream out("C:\\Source\\tmp\\comp-sdt-rc2.0exe\\bin\\Release\\gdip.log", std::ios_base::out | std::ios_base::app);
		//			out << "\tcloned file stream not equal" << std::endl;
		//		}

		//		read(data, sizeof(data), a);
		//		stream->read(data_, sizeof(data), b);
		//	}

		//	std::ofstream out("C:\\Source\\tmp\\comp-sdt-rc2.0exe\\bin\\Release\\gdip.log", std::ios_base::out | std::ios_base::app);
		//	out << "\tfile streams equal" << std::endl;

		//	seek(static_cast<__int64>(newPosition), FILE_BEGIN, p);
		//	stream->seek(static_cast<__int64>(newPosition), FILE_BEGIN, p);
		//}

		*ppstm = stream.Detach();
		return S_OK;

		COM_TRY_END
	}
	
	basicFileStream() : handle_(INVALID_HANDLE_VALUE), cloneAccessMode_(GENERIC_READ), cloneShareMode_(FILE_SHARE_READ) {}
	basicFileStream(HANDLE handle, DWORD cloneAccessMode, DWORD cloneShareMode) : handle_(handle), cloneAccessMode_(cloneAccessMode), cloneShareMode_(cloneShareMode) {}
	~basicFileStream() { close(); }

	bool createTemporary(wchar_t const *path_) {
		close();
		path = path_;
		cloneAccessMode_ = GENERIC_READ | GENERIC_WRITE; cloneShareMode_ = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
		handle_ = ::CreateFileW(path_, GENERIC_READ | GENERIC_WRITE, cloneShareMode_,
			(LPSECURITY_ATTRIBUTES)NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, (HANDLE)NULL);
		/*if (INVALID_HANDLE_VALUE == handle_)
			if (::GetLastError() == ERROR_FILE_NOT_FOUND) log(Error) << "temporary file not found";*/
		return (handle_ != INVALID_HANDLE_VALUE);
	}
	bool createTemporaryAndSwapPath(std::wstring &path_) {
		close();
		path.swap(path_);
		cloneAccessMode_ = GENERIC_READ | GENERIC_WRITE; cloneShareMode_ = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
		handle_ = ::CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE, cloneShareMode_,
			(LPSECURITY_ATTRIBUTES)NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, (HANDLE)NULL);
		/*if (INVALID_HANDLE_VALUE == handle_)
			if (::GetLastError() == ERROR_FILE_NOT_FOUND) log(Error) << "temporary file not found";*/
		return (handle_ != INVALID_HANDLE_VALUE);
	}
	bool create(wchar_t const *path_) {
		close();
		path = path_;
		cloneAccessMode_ = GENERIC_READ | GENERIC_WRITE; cloneShareMode_ = FILE_SHARE_READ | FILE_SHARE_WRITE;
		handle_ = ::CreateFileW(path_, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			(LPSECURITY_ATTRIBUTES)NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
		/*if (INVALID_HANDLE_VALUE == handle_)
			if (::GetLastError() == ERROR_FILE_NOT_FOUND) log(Error) << "temporary file not found";*/
		return (handle_ != INVALID_HANDLE_VALUE);
	}
	bool open(wchar_t const *path_) {
		close();
		path = path_;
		cloneAccessMode_ = GENERIC_READ | GENERIC_WRITE; cloneShareMode_ = FILE_SHARE_READ | FILE_SHARE_WRITE;
		handle_ = ::CreateFileW(path_, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			(LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
		/*if (INVALID_HANDLE_VALUE == handle_)
			if (::GetLastError() == ERROR_FILE_NOT_FOUND) log(Error) << "temporary file not found";*/
		return (handle_ != INVALID_HANDLE_VALUE);
	}
	//bool open(std::wstring const &path_) {
	//	//path.swap(path_);
	//	return open(path_.c_str());
	//}
	bool openRead(wchar_t const *path_) {
		close();
		path = path_;
		cloneAccessMode_ = GENERIC_READ; cloneShareMode_ = FILE_SHARE_READ;
		handle_ = ::CreateFileW(path_, GENERIC_READ, FILE_SHARE_READ,
			(LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
		return (handle_ != INVALID_HANDLE_VALUE);
	}
	//bool openRead(std::wstring const &path_) {
	//	return openRead(path_.c_str());
	//}
	void close() {
		if (handle_ != INVALID_HANDLE_VALUE) {
			::CloseHandle(handle_);
			// if (::CloseHandle(handle_) == 0) log(Error) << "fail to close handle_ at object";
			handle_ = INVALID_HANDLE_VALUE;
		}
		//if (!path.empty()) {
		//	if ((exist(path)) && (!deleteFile(path))) {
		//		deleteFile(path);
		//		// if (!deleteFile(path)) log(Error) << "unable delete temporary file " << path;
		//	}
		//	std::wstring path_;
		//	path.swap(path_);
		//}
	}
	static unsigned int const kChunkSizeMax = (1 << 22);
	bool readPart(void *data, unsigned int size, unsigned int &processedSize) {
		if (size > kChunkSizeMax)
			size = kChunkSizeMax;
		DWORD processedLoc = 0;
		bool res = (::ReadFile(handle_, data, size, &processedLoc, NULL) != FALSE);
		processedSize = (unsigned int)processedLoc;
		return res;
	}
	bool read(void *data, unsigned int size, unsigned int &processedSize) {
		processedSize = 0;
		do {
			unsigned int processedLoc = 0;
			bool res = readPart(data, size, processedLoc);
			processedSize += processedLoc;
			if (!res)
				return false;
			if (processedLoc == 0)
				return true;
			data = (void *)((unsigned char *)data + processedLoc);
			size -= processedLoc;
		} while (size > 0);
		return true;
	}
	bool writePart(const void *data, unsigned int size, unsigned int &processedSize) {
		if (size > kChunkSizeMax)
			size = kChunkSizeMax;
		DWORD processedLoc = 0;
		bool res = (::WriteFile(handle_, data, size, &processedLoc, NULL) != FALSE);
		processedSize = (unsigned int)processedLoc;
		return res;
	}
	bool write(const void *data, unsigned int size, unsigned int &processedSize) {
		processedSize = 0;
		do {
			unsigned int processedLoc = 0;
			bool res = writePart(data, size, processedLoc);
			processedSize += processedLoc;
			if (!res)
				return false;
			if (processedLoc == 0)
				return true;
			data = (const void *)((const unsigned char *)data + processedLoc);
			size -= processedLoc;
		} while (size > 0);
		return true;
	}
	bool seek(__int64 distanceToMove, DWORD moveMethod, unsigned __int64 &newPosition) const {
		LARGE_INTEGER value;
		value.QuadPart = distanceToMove;
		value.LowPart = ::SetFilePointer(handle_, value.LowPart, &value.HighPart, moveMethod);
		if (value.LowPart == 0xFFFFFFFF)
			if (::GetLastError() != NO_ERROR)
				return false;

		newPosition = value.QuadPart;
		return true;
	}
	bool getLength(unsigned __int64 &length) const {
		DWORD sizeHigh;
		DWORD sizeLow = ::GetFileSize(handle_, &sizeHigh);
		if (sizeLow == 0xFFFFFFFF)
			if (::GetLastError() != NO_ERROR)
				return false;

		length = (((unsigned __int64)sizeHigh) << 32) + sizeLow;
		return true;
	}
	bool setLength(unsigned __int64 length) {
		unsigned __int64 newPosition;
    if (!seek(length, FILE_BEGIN, newPosition))
			return false;
		if (newPosition != length)
			return false;

    return (::SetEndOfFile(handle_) != FALSE);
	}

private:
	HANDLE handle_;
	DWORD cloneAccessMode_;
	DWORD cloneShareMode_;
	std::wstring path;
};

HRESULT fileStream(const wchar_t *path, IStream **v) {
	COM_TRY_BEGIN

	CMyComPtr<basicFileStream> stream(new basicFileStream());
	if (!stream->open(path))
		return E_FAIL;
	*v = stream.Detach();
	return S_OK;

	COM_TRY_END
}

HRESULT fileStreamRead(const wchar_t *path, IStream **v) {
	COM_TRY_BEGIN

	CMyComPtr<basicFileStream> stream(new basicFileStream());
	if (!stream->openRead(path))
		return E_FAIL;
	*v = stream.Detach();
	return S_OK;

	COM_TRY_END
}

HRESULT fileStreamCreate(const wchar_t *path, IStream **v) {
	COM_TRY_BEGIN

	CMyComPtr<basicFileStream> stream(new basicFileStream());
	if (!stream->create(path))
		return E_FAIL;
	*v = stream.Detach();
	return S_OK;

	COM_TRY_END
}

// Specify the width of the field in which to print the numbers. 
// The asterisk in the format specifier "%*I64d" takes an integer 
// argument and uses it to pad and right justify the number.
#define WIDTH 7
static size_t availablePhysicalMemoryMib(void) {
	MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);

	if (!::GlobalMemoryStatusEx(&statex))
		return 0;

	/*_tprintf (TEXT("There are %*I64d free Mibs of physical memory.\n"),
            WIDTH, (statex.ullAvailPhys >> 20));
	_tprintf (TEXT("There are %*I64d free Mibs of virtual memory.\n"),
            WIDTH, (statex.ullAvailVirtual >> 20));*/

	return (size_t)(statex.ullAvailPhys >> 20);
}

static size_t availableDiskSpaceMib(const std::wstring &folder) {
	ULARGE_INTEGER freeBytesAvailable;
	if (!::GetDiskFreeSpaceExW(folder.c_str(), &freeBytesAvailable, NULL, NULL))
		return 0;

	return (size_t)(freeBytesAvailable.QuadPart >> 20);
}

HRESULT temporaryStream(unsigned __int64 estimatedSizeByte/* estimatedSizeMiB && check free memory && disk space */, IStream **v) {
	COM_TRY_BEGIN

	const size_t estimatedSizeMiB = (estimatedSizeByte > 0) ? size_t(estimatedSizeByte >> 20) : 64;
	
	//if (availablePhysicalMemoryMib() > (estimatedSizeMiB + 256)) {
	if (estimatedSizeMiB < 1) {
		CMyComPtr<IStream> stream;
		RINOK(::CreateStreamOnHGlobal(NULL, TRUE, &stream))
		*v = stream.Detach();
		return S_OK;
	}

	//pathUtilities::path_t path;
	std::wstring folder, path;
	if (!getTemporaryDirectory(&folder))
		return E_FAIL;

	if ((estimatedSizeMiB + 512) > availableDiskSpaceMib(folder))
		return STG_E_MEDIUMFULL;

	if (!createTemporaryFile(folder, &path))
		return E_FAIL;
	CMyComPtr<basicFileStream> stream(new basicFileStream());
	if (!stream->createTemporaryAndSwapPath(path))
		return E_FAIL;
	*v = stream.Detach();
	return S_OK;

	COM_TRY_END
}

HRESULT temporaryFileStream(unsigned __int64 estimatedSizeByte, IStream **v) {
	std::wstring folder, path;
	if (!getTemporaryDirectory(&folder))
		return E_FAIL;

	if (estimatedSizeByte > 0) {
		const size_t estimatedSizeMiB = size_t(estimatedSizeByte >> 20);
		if ((estimatedSizeMiB + 256) > availableDiskSpaceMib(folder))
			return STG_E_MEDIUMFULL;
	}

	if (!createTemporaryFile(folder, &path))
		return E_FAIL;
	CMyComPtr<basicFileStream> stream(new basicFileStream());
	if (!stream->createTemporaryAndSwapPath(path))
		return E_FAIL;
	*v = stream.Detach();
	return S_OK;
}

void test() {
	CMyComPtr<IStream> stream;
	temporaryStream(600 * 1024 * 1024, &stream);
	stream = (IStream*)0;
}

#endif
