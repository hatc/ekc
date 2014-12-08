// memory_stream.h
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

#ifndef __CPCL_MEMORY_STREAM_H
#define __CPCL_MEMORY_STREAM_H

#include <stddef.h>
#include "io_stream.h"

namespace cpcl {

#if 0

struct MemoryStream {
	bool state;
	unsigned char *p;
	int p_size;
	int p_offset;

	operator bool() const { return state; }

	MemoryStream();
	MemoryStream(unsigned char *p_, size_t p_size_);

	void Assign(unsigned char *p_, size_t p_size_);
	unsigned long Read(void *data, unsigned long size);
	unsigned long Write(void const *data, unsigned long size);
	bool Seek(__int64 move_to, unsigned long move_method, __int64 *position);
	__int64 Tell() const { return p_offset; }
	__int64 Size() const { return p_size; }
};

#endif

class MemoryStream : public IOStream {
	bool state;
public:
	unsigned char *p;
	int p_size;
	int p_offset;
	bool throw_eof_write;

	virtual operator bool() const { return state; }
	void Assign(unsigned char *p_, size_t p_size_);
	unsigned char ReadByte();
	void WriteByte(unsigned char v);

	MemoryStream();
	MemoryStream(unsigned char *p_, size_t p_size_, bool throw_eof_write_ = false);
	virtual ~MemoryStream();

	virtual IOStream* Clone();
	virtual unsigned long CopyTo(IOStream *output, unsigned long size);
	
	virtual unsigned long Read(void *data, unsigned long size);
	virtual unsigned long Write(void const *data, unsigned long size);
	virtual bool Seek(__int64 move_to, unsigned long move_method, __int64 *position);
	virtual __int64 Tell();
	virtual __int64 Size();
};

} // namespace cpcl

#endif // __CPCL_MEMORY_STREAM_H
