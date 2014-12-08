// file_istream.h
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

#ifndef __CPCL_FILE_ISTREAM_H
#define __CPCL_FILE_ISTREAM_H

#include "string_piece.hpp"
#include "file_stream.h"

#include "internal_com.h"

#include <windows.h>
#include <Objidl.h>

namespace cpcl {

class FileIStream : public IStream, public CUnknownImp {
	DWORD clone_access_mode; 
	DWORD clone_share_mode;

	FileIStream();
	FileIStream(FileIStream const&);
	void operator=(FileIStream const&);
public:
	FileStream *stream_impl;

	CPCL_UNKNOWN_IMP1_MT(IStream)

	STDMETHOD(Read)(/* [length_is][size_is][out] */ void* pv,
        /* [in] */ ULONG cb,
        /* [out] */ ULONG* pcbRead);
	STDMETHOD(Write)(/* [size_is][in] */ const void* pv,
        /* [in] */ ULONG cb,
        /* [out] */ ULONG* pcbWritten);
	STDMETHOD(Seek)(/* [in] */ LARGE_INTEGER dlibMove,
        /* [in] */ DWORD dwOrigin,
        /* [out] */ ULARGE_INTEGER* plibNewPosition);
	STDMETHOD(SetSize)(/* [in] */ ULARGE_INTEGER libNewSize);
	STDMETHOD(CopyTo)(/* [unique][in] */ IStream *pstm,
        /* [in] */ ULARGE_INTEGER cb,
        /* [out] */ ULARGE_INTEGER* pcbRead,
        /* [out] */ ULARGE_INTEGER* pcbWritten);
	STDMETHOD(Commit)(/* [in] */ DWORD /*grfCommitFlags*/);
	STDMETHOD(Revert)(void);
	STDMETHOD(LockRegion)(/* [in] */ ULARGE_INTEGER /*libOffset*/,
        /* [in] */ ULARGE_INTEGER /*cb*/,
        /* [in] */ DWORD /*dwLockType*/);
  STDMETHOD(UnlockRegion)(/* [in] */ ULARGE_INTEGER /*libOffset*/,
        /* [in] */ ULARGE_INTEGER /*cb*/,
        /* [in] */ DWORD /*dwLockType*/);
  STDMETHOD(Stat)(/* [out] */ STATSTG* pstatstg,
        /* [in] */ DWORD grfStatFlag);
	STDMETHOD(Clone)(/* [out] */ IStream** ppstm);

	virtual ~FileIStream();

	// WStringPiece Path() const; // return WStringPiece(path.c_str() + 4, path.size() - 4);

	static bool Create(WStringPiece const &path, FileIStream **v); // NormalizePath(FileStream.path); FileStream.path.insert(0, L"\\\\?\\");
	static bool CreateTemporary(FileIStream **v); // use CreateGUID as random name
	static bool Read(WStringPiece const &path, FileIStream **v);
	static bool ReadWrite(WStringPiece const &path, FileIStream **v);
/*private:
	HANDLE hFile;
	DWORD access_mode;
	DWORD share_mode;
	std::wstring path;

	FileIStream(HANDLE hFile_, DWORD access_mode_, DWORD share_mode_)
		: hFile(hFile_), access_mode(access_mode_), share_mode(share_mode_) {}*/
};

} // namespace cpcl

#endif // __CPCL_FILE_ISTREAM_H
