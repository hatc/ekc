// libxml_util.h
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

#ifndef __CPCL_LIBXML_UTIL_H
#define __CPCL_LIBXML_UTIL_H

#include <memory>

#include "scoped_buf.hpp"
#include "string_piece.hpp"
#include "io_stream.h"

namespace cpcl {

struct LibXmlStuff;

class XmlDocument {
	ScopedBuf<wchar_t, 0x100> conv_buf;
	std::auto_ptr<LibXmlStuff> libxml_stuff;

	XmlDocument();
public:
	~XmlDocument();

	bool TrimResults;

	bool XPath(StringPiece const &v, std::wstring *r);

	static wchar_t const TrimChars[];

	static bool Create(IOStream *input, XmlDocument **r);
};

class XmlReader {
	std::auto_ptr<LibXmlStuff> libxml_stuff;
	bool end_element;

	XmlReader();
	XmlReader(XmlReader const &v);
	void operator=(XmlReader const &v);
public:
	~XmlReader();

	bool Read();
	bool IsEndElement();
	//bool Reset();
	int Depth() const;

	bool HasAttributes();
	bool MoveToFirstAttribute();
	bool MoveToNextAttribute();

	StringPiece Name_UTF8();
	StringPiece Value_UTF8();

	static bool Create(IOStream *input, XmlReader **r);
};

class XmlWriter {
	ScopedBuf<unsigned char, 0x100> conv_buf;
	std::auto_ptr<LibXmlStuff> libxml_stuff;

	XmlWriter();
	XmlWriter(XmlWriter const &v);
	void operator=(XmlWriter const &v);
public:
	~XmlWriter();

	bool StartDocument(StringPiece encoding = StringPiece());
	bool StartElement(StringPiece const &name_utf8);
	bool WriteAttribute(StringPiece const &name_utf8, StringPiece const &v_utf8);
	bool WriteAttribute(StringPiece const &name_utf8, WStringPiece const &v);
	bool WriteAttribute(StringPiece const &name_utf8, double v);
	bool WriteAttribute(StringPiece const &name_utf8, unsigned int v);
	bool WriteAttribute(StringPiece const &name_utf8, unsigned long v);
	bool WriteAttribute(StringPiece const &name_utf8, int v);
	bool WriteString(StringPiece const &v_utf8);
	bool WriteString(WStringPiece const &v);
	bool EndElement();

	bool FlushWriter();
	bool SetEncoding(char const *encoding); /* needed for xml fragment writing */

	static bool Create(IOStream *output/*writer not own output stream, but output stream must be valid until XmlWriter not deleted*/, XmlWriter **r);
};

void LibXmlInit();
void LibXmlCleanup();

} // namespace cpcl

#endif // __CPCL_LIBXML_UTIL_H
