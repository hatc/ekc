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

#include "byte_iostream.h"

ByteIOStream::ByteIOStream(std::auto_ptr<cpcl::IOStream> &stream_) : stream(stream_)
{}
ByteIOStream::~ByteIOStream()
{}

size_t ByteIOStream::read(void *buffer, size_t size) {
	int r = stream->Read(buffer, size);
	if (r < 0)
		G_THROW_EXTERNAL("IOStream::Read() fails");
	return (size_t)r;
}

size_t ByteIOStream::write(const void *buffer, size_t size) {
	int r = stream->Write(buffer, size);
	if (r < 0)
		G_THROW_EXTERNAL("IOStream::Write() fails");
	return (size_t)r;
}

void ByteIOStream::flush(void)
{}

int ByteIOStream::seek(long offset, int whence, bool nothrow) {
	if (!stream->Seek(offset, whence, 0)) {
		if (nothrow)
			return -1;
		G_THROW_EXTERNAL("IOStream::Seek() fails");
	}
	return 0;
}

long ByteIOStream::tell(void) const {
	return static_cast<long>(stream->Tell());
}
