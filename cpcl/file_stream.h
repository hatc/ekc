// file_stream.h
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

#ifndef __CPCL_FILE_STREAM_H
#define __CPCL_FILE_STREAM_H

#include "io_stream.h"
#include "string_piece.hpp"

namespace cpcl {

#if 0

class FileStream {
	bool state;

	bool ReadPart(void *data, unsigned long size, unsigned long &processed_size); 
	bool WritePart(void const *data, unsigned long size, unsigned long &processed_size);

	FileStream(FileStream const&);
	void operator=(FileStream const&);
public:
	void *hFile;
	std::wstring path;

	operator bool() const { return state; }
	WStringPiece Path() const;

	FileStream();
	~FileStream();

	unsigned long Read(void *data, unsigned long size);
	unsigned long Write(void const *data, unsigned long size);
	bool Seek(__int64 move_to, unsigned long move_method, __int64 *position);
	__int64 Tell() {
		__int64 r;
		state = Seek(0, 1, &r);
		if (!state)
			r = -1;
		return r;
	}
	__int64 Size();

	static std::wstring ExpandPath(cpcl::WStringPiece const &v); // \\\\?\\v
	static void ExpandPath(std::wstring *v); // \\\\?\\v

	static bool FileStreamCreate(cpcl::WStringPiece const &path,
		unsigned long access_mode, unsigned long share_mode,
		unsigned long disposition, unsigned long attributes,
		FileStream **v);
	static bool Create(cpcl::WStringPiece const &path, FileStream **v);
	static bool CreateTemporary(FileStream **v);
	static bool Read(cpcl::WStringPiece const &path, FileStream **v);
	static bool ReadWrite(cpcl::WStringPiece const &path, FileStream **v);
};

#endif

class FileStream : public IOStream {
	bool state;

	bool ReadPart(void *data, unsigned long size, unsigned long &processed_size); 
	bool WritePart(void const *data, unsigned long size, unsigned long &processed_size);

	FileStream(FileStream const&);
	void operator=(FileStream const&);
public:
	void *hFile;
	std::wstring path;

	virtual operator bool() const { return state; }
	WStringPiece Path() const;

	FileStream();
	virtual ~FileStream();
	
	virtual IOStream* Clone();
	virtual unsigned long Read(void *data, unsigned long size);
	virtual unsigned long Write(void const *data, unsigned long size);
	virtual bool Seek(__int64 move_to, unsigned long move_method, __int64 *position);
	virtual __int64 Tell() {
		__int64 r;
		state = Seek(0, 1, &r);
		if (!state)
			r = -1;
		return r;
	}
	virtual __int64 Size();

	static std::wstring ExpandPath(cpcl::WStringPiece const &v); // \\\\?\\v
	static void ExpandPath(std::wstring *v); // \\\\?\\v

	static bool FileStreamCreate(cpcl::WStringPiece const &path,
		unsigned long access_mode, unsigned long share_mode,
		unsigned long disposition, unsigned long attributes,
		FileStream **v);
	static bool Create(cpcl::WStringPiece const &path, FileStream **v);
	static bool CreateTemporary(FileStream **v);
	static bool Read(cpcl::WStringPiece const &path, FileStream **v);
	static bool ReadWrite(cpcl::WStringPiece const &path, FileStream **v);
};

} // namespace cpcl

#endif // __CPCL_FILE_STREAM_H
