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
#include <stdio.h>
#include <climits>

#include "memory_stream.h"
#include "trace.h"
#include "memory_limit_exceeded.hpp"

/*MemoryStream
Read():
 precondition: size > 0, file pointer at EOF
 postcondition: state = true, Read() return 0

 !precondition: size > 0, file pointer beyond EOF - file pointer cannot be beyond EOF 
 postcondition: state = false, Read() return 0

Write():
 precondition: memory const
 postcondition: state = false, Write() return 0

 !precondition: size > 0, file pointer beyond EOF - file pointer cannot be beyond EOF 
 postcondition: state = false, Write() return 0

Seek():
 precondition: file pointer valid, move_to beyond EOF
 postcondition: state = true, file pointer unchanged, return false

 precondition: file pointer valid, move_to before begin of file, i.e. move_to < 0 && SEEK_SET
 postcondition: state = true, file pointer unchanged, return false*/

namespace cpcl {

MemoryStream::MemoryStream()
	: p(NULL), p_size(0), p_offset(0), throw_eof_write(false), state(true)
{}
MemoryStream::MemoryStream(unsigned char *p_, size_t p_size_, bool throw_eof_write_)
	: p(p_), p_size(p_size_&INT_MAX), p_offset(0), throw_eof_write(throw_eof_write_), state(true)
{}
MemoryStream::~MemoryStream()
{}

void MemoryStream::Assign(unsigned char *p_, size_t p_size_) {
	p = p_;
	p_size = p_size_&INT_MAX;
	p_offset = 0;
	state = true;
}
unsigned char MemoryStream::ReadByte() {
	if ((p_offset + 1) > p_size) {
		state = false;
		return 0;
	}
	state = true;
	return *(p + p_offset++);
}
void MemoryStream::WriteByte(unsigned char v) {
	if ((p_offset + 1) > p_size) {
		state = false;
		if (throw_eof_write) {
			memory_limit_exceeded e(p_size);
			memory_limit_exceeded::throw_formatted(e, "MemoryStream::WriteByte(): limit(%d bytes) exceeded", p_size);
		}
	} else {
		state = true;
		*(p + p_offset++) = v;
	}
}

IOStream* MemoryStream::Clone() {
	return new MemoryStream(p, p_size, throw_eof_write);
}
unsigned long MemoryStream::CopyTo(IOStream *output, unsigned long size) {
	return output->Write(p + p_offset, (std::min)(size, (unsigned long)(p_size - p_offset)));
}

unsigned long MemoryStream::Read(void *data, unsigned long size) {
	int p_offset_ = (p_offset + size)&INT_MAX; // INT_MAX == (int)((~(unsigned int)0) >> 1)
	if (p_offset_ < p_offset) {
		Trace(CPCL_TRACE_LEVEL_ERROR, "MemoryStream::Read(data = %08X, size = %u): negative offset %d, current offset %d",
			(int)data, size, p_offset_, p_offset);
		state = false;
		return 0;
	}
	int r = (std::min)(p_size, p_offset_) - p_offset;
	if (r > 0) {
		memcpy(data, p + p_offset, r);
		p_offset += r;
		state = true;
	}
	return r;
}

unsigned long MemoryStream::Write(void const *data, unsigned long size) {
	int p_offset_ = (p_offset + size)&INT_MAX;
	if (p_offset_ < p_offset) {
		Trace(CPCL_TRACE_LEVEL_ERROR, "MemoryStream::Write(data = %08X, size = %u): negative offset %d, current offset %d",
			(int)data, size, p_offset_, p_offset);
		state = false;
		return 0;
	}
	int r = (std::min)(p_size, p_offset_) - p_offset;
	if (throw_eof_write) {
		if (r < (int)(size&INT_MAX)) {
			memory_limit_exceeded e(p_size);
			memory_limit_exceeded::throw_formatted(e, "MemoryStream::Write(data = %08X, size = %u): limit(%d bytes) exceeded",
				(int)data, size, p_size);
		}
	}
	if (r > 0) {
		memcpy(p + p_offset, data, r);
		p_offset += r;
		state = true;
	}
	return r;
}

bool MemoryStream::Seek(__int64 move_to, unsigned long move_method, __int64 *position) {
	if (SEEK_CUR == move_method)
		move_to = p_offset + move_to; 
	else if (SEEK_END == move_method)
		move_to = p_size + move_to;
	else if (SEEK_SET != move_method) {
		Error(StringPieceFromLiteral("MemoryStream::Seek(): invalid move_method"));
		return false;
	}

	if (move_to < 0) {
		Error(StringPieceFromLiteral("MemoryStream::Seek(): move_to before the beginning of the memory chunk"));
		return false;
	} else if (move_to > p_size) {
		Error(StringPieceFromLiteral("MemoryStream::Seek(): move_to beyond the memory chunk"));
		return false;
	}
	p_offset = (int)move_to;
	if (position)
		*position = p_offset;
	return true;
}

__int64 MemoryStream::Tell() {
	return p_offset;
}

__int64 MemoryStream::Size() {
	return p_size;
}

} // namespace cpcl
