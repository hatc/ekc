// wrapper_stream.h
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

#include <memory>

#include <poppler/Object.h>
#include <poppler/Stream.h>

#include <io_stream.h>

class WrapperStream : public BaseStream {
	Guint start;
  GBool limited;
  Guint length;
  char buf[fileStreamBufSize];
  char *bufPtr;
  char *bufEnd;
  Guint bufPos;
  int savePos;
  GBool saved;

	std::tr1::shared_ptr<cpcl::IOStream> stream;

	GBool fillBuf();
public:	
	WrapperStream(std::tr1::shared_ptr<cpcl::IOStream> const &stream_, Guint startA, GBool limitedA,
	  Guint lengthA, Object *dictA);
	virtual ~WrapperStream();

  virtual Stream *makeSubStream(Guint startA, GBool limitedA,
		Guint lengthA, Object *dictA);
  virtual StreamKind getKind();
  virtual void reset();
  virtual void close();
  virtual int getChar();
  virtual int lookChar();
  virtual int getPos();
  virtual void setPos(Guint pos, int dir = 0);
  virtual Guint getStart();
  virtual void moveStart(int delta);
	virtual int getUnfilteredChar();
	virtual void unfilteredReset();
};
