// io_stream.h
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

#ifndef __CPCL_IO_STREAM_H
#define __CPCL_IO_STREAM_H

namespace cpcl {

class IOStream {
public:
	virtual ~IOStream();

	virtual operator bool() const { return true; } // replace operator bool with bool operator!() { return false; }
	virtual IOStream* Clone() = 0; // returns a new stream object with its own seek pointer that references the same bytes as the original stream
	virtual unsigned long CopyTo(IOStream *output, unsigned long size); // return number of bytes written to output
	virtual unsigned long Read(void *data, unsigned long size) = 0;
	virtual unsigned long Write(void const *data, unsigned long size) = 0;
	virtual bool Seek(__int64 move_to, unsigned long move_method, __int64 *position) = 0;
	virtual __int64 Tell() = 0;
	virtual __int64 Size() = 0;
};

} // namespace cpcl

#endif // __CPCL_IO_STREAM_H
