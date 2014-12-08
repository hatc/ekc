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

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <cassert>
#include <iostream>

#include <scoped_buf.hpp>
#include <string_util.h>
#include <file_stream.h>
#include <memory_stream.h>
#include <libxml_util.h>
#include <trace.h>

inline bool GetAttribute(cpcl::XmlReader *reader, cpcl::StringPiece name, std::string *r) {
	cpcl::StringPiece v;

	bool has_attr = reader->MoveToFirstAttribute();
	while (has_attr) {
		if (cpcl::striASCIIequal(reader->Name_UTF8(), name)) {
			v = reader->Value_UTF8();
			if (r)
				(*r).assign(v.data(), v.size());
			return true;
		}

		has_attr = reader->MoveToNextAttribute();
	}

	return false;
}

int _tmain(int argc, _TCHAR* argv[])
{
	cpcl::TRACE_FILE_PATH = L"C:\\Source\\hlam\\srv.session\\lsd-updated\\ebook-client.txt";
	cpcl::TRACE_LEVEL = CPCL_TRACE_LEVEL_INFO | CPCL_TRACE_LEVEL_DEBUG | CPCL_TRACE_LEVEL_WARNING | CPCL_TRACE_LEVEL_ERROR;
	cpcl::LibXmlInit();

	cpcl::FileStream *input_;
	if (cpcl::FileStream::Read(L"C:\\Source\\hlam\\srv.session\\lsd-updated\\cpcl.xml", &input_)) {
		std::auto_ptr<cpcl::FileStream> input(input_);

		cpcl::XmlReader *xml_;
		if (cpcl::XmlReader::Create(input.get(), &xml_)) {
			std::auto_ptr<cpcl::XmlReader> xml(xml_);

			while (xml->Read()) {
				if (cpcl::striASCIIequal(xml->Name_UTF8(), "File") && !xml->IsEndElement()) {
					std::string r;
					if (GetAttribute(xml.get(), "RelativePath", &r))
						std::cout << r << std::endl;
				}
			}
		}
	}

	cpcl::LibXmlCleanup();
	_CrtDumpMemoryLeaks();
	return 0;
}
