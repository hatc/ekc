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

#include "wrapper_stream.h"

#ifndef INT_MAX
int const INT_MAX = (~(unsigned int)0) >> 1;
#endif

WrapperStream::WrapperStream(std::tr1::shared_ptr<cpcl::IOStream> const &stream_, Guint startA, GBool limitedA,
	Guint lengthA, Object *dictA) : BaseStream(dictA), stream(stream_), start(startA),
	limited(limitedA), length(lengthA) {
  bufPtr = bufEnd = buf;
  bufPos = start;
  savePos = 0;
  saved = gFalse;
}
WrapperStream::~WrapperStream()
{}

GBool WrapperStream::fillBuf() {
	int n;

  bufPos += bufEnd - buf;
  bufPtr = bufEnd = buf;
  if (limited && bufPos >= start + length) {
    return gFalse;
  }
  if (limited && bufPos + fileStreamBufSize > start + length) {
    n = start + length - bufPos;
  } else {
    n = fileStreamBufSize;
  }
	n = (int)stream->Read(buf, n & INT_MAX);
  bufEnd = buf + n;
  if (bufPtr >= bufEnd) {
    return gFalse;
  }
  return gTrue;
}

Stream* WrapperStream::makeSubStream(Guint startA, GBool limitedA,
	Guint lengthA, Object *dictA) {
	return new WrapperStream(stream, startA, limitedA, lengthA, dictA);
}

StreamKind WrapperStream::getKind() {
	return strFile;
}

void WrapperStream::reset() {
	savePos = static_cast<int>(stream->Tell());
	stream->Seek(static_cast<__int64>(start), SEEK_SET, 0);

  saved = gTrue;
  bufPtr = bufEnd = buf;
  bufPos = start;
}

void WrapperStream::close() {
	if (saved) {
    stream->Seek(savePos, SEEK_SET, 0);
    saved = gFalse;
  }
}

int WrapperStream::getChar() {
	return (bufPtr >= bufEnd && !fillBuf()) ? EOF : (*bufPtr++ & 0xff);
}

int WrapperStream::lookChar() {
	return (bufPtr >= bufEnd && !fillBuf()) ? EOF : (*bufPtr & 0xff);
}

int WrapperStream::getPos() {
	return bufPos + (bufPtr - buf);
}

void WrapperStream::setPos(Guint pos, int dir) {
	Guint size;

  if (dir >= 0) {
    stream->Seek(static_cast<__int64>(pos), SEEK_SET, 0);
    bufPos = pos;
  } else {
		__int64 p;
    if (!stream->Seek(0, SEEK_END, &p))
			throw std::exception("WrapperStream::setPos(): IOStream::Seek() fails", 1);
    // size = static_cast<Guint>(stream->Tell());
		size = static_cast<Guint>(p);

    if (pos > size)
      pos = size;

		if (!stream->Seek(-1i64 * static_cast<__int64>(pos), SEEK_END, &p))
			throw std::exception("WrapperStream::setPos(): IOStream::Seek() fails", 1);
    // bufPos = static_cast<Guint>(stream.Tell());
		bufPos = static_cast<Guint>(p);
  }
  bufPtr = bufEnd = buf;
}

Guint WrapperStream::getStart() {
	return start;
}

void WrapperStream::moveStart(int delta) {
	start += delta;
  bufPtr = bufEnd = buf;
  bufPos = start;
}

int WrapperStream::getUnfilteredChar() {
	return getChar();
}

void WrapperStream::unfilteredReset() {
	reset();
}
