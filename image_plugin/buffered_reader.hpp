// buffered_reader.hpp
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

#ifndef __CPCL_BUFFERED_READER_HPP
#define __CPCL_BUFFERED_READER_HPP

#include <exception>

#include "io_stream.h"

namespace cpcl {

template<size_t BufferSize/*BUFFER_SIZE*/>
struct BufferedReader {
	class eof : public std::exception {
	public:
		eof() : std::exception("BufferedReader(): EOF", 1)
		{}
	};
	//static size_t const BUFFER_SIZE = 0x1000;

	BufferedReader(IOStream *stream_) : stream(stream_), size(0)
	{}

	unsigned char ReadByte() {
		if (!size)
			FillBuf();
		if (!size)
			throw eof();
		
		--size;
		return *p++;
	}
	void Read(unsigned char *r, size_t N) {
		size_t n = N - TryRead(r, N); // n = n - TryRead(r, n); ???
		while (n) {
			FillBuf();
			if (!size)
				throw eof();

			n -= TryRead(r, n);
		}
	}
	template<size_t N>
	void Read(unsigned char (&r)[N]) {
		Read(r, N);
	}
	template<class Pod>
	// Pod & or Pod * ? using Pod * we are more clearly declare r as function return value [out], rather than argument [in]
	// void ReadPod(Pod &r) {
	void /*enable_if<is_pod<Pod>::value>*/ ReadPod(Pod *r) {
		Read((unsigned char*)r, sizeof(Pod));
	}
private:
	unsigned char buf[BufferSize/*BUFFER_SIZE*/];
	unsigned char *p;
	size_t size;
	IOStream *stream;

	size_t TryRead(unsigned char *&r, size_t n) {
		n = size < n ? size : n; // min(size, n);
		if (n) {
			memcpy(r, p, n);
			r += n;
			p += n;
			size -= n;
		}
		return n;
	}
	void FillBuf() {
		size = stream->Read(buf, BufferSize/*BUFFER_SIZE*/);
		p = buf;
	}
};
// size_t const BufferedReader::BUFFER_SIZE;

} // namespace cpcl

#endif // __CPCL_BUFFERED_READER_HPP
