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

//#include <algorithm>

#include <split_iterator.hpp>
#include <scoped_buf.hpp>
#include "plugin.h"

#include <trace.h>

#include "signature_info_helper.hpp"

namespace plcl {

/*
Plugin::Plugin(SignatureInfo const *signatures, unsigned int n_signatures, wchar_t const *doc_file_extension_) {
 SetSignatures(signatures, n_signatures);
}

static unsigned char const signature_[] = { 0xFF, 0xFE };
static SignatureInfo const signatures[] = {
	SIGNATURE_INFO(signature_)
};
JpegPlugin::JpegPlugin() : Plugin(signatures, arraysize(signatures), "jpg") {
 ...
}

//-------------------------------------------------------------------------------------------------------------//

JpegPlugin::JpegPlugin() : Plugin("jpg") {
 SignatureInfo const signatures[] = {
  SIGNATURE_INFO(signature_)
 };
 SetSignatures(signatures, arraysize(signatures));
}
*/

Plugin::Plugin(wchar_t const *doc_file_format_, wchar_t const *doc_file_extension_)
	: doc_file_extension(NULL), doc_file_extension_size(0), doc_file_format(NULL), doc_file_format_size(0), signatures(NULL), n_signatures(0) {
	//if (doc_file_extension_) {
	//	doc_file_extension_size = wcslen(doc_file_extension_);
	//	while ((doc_file_extension_size > 0) && (L'.' == *doc_file_extension_)) {
	//		++doc_file_extension_;
	//		--doc_file_extension_size;
	//	}
	//	if (doc_file_extension_size > 0) {
	//		doc_file_extension = new wchar_t[doc_file_extension_size + 1];
	//		wmemcpy(doc_file_extension, doc_file_extension_, doc_file_extension_size);
	//		//wmemcpy_s(doc_file_extension, doc_file_extension_size, doc_file_extension_, doc_file_extension_size);
	//		doc_file_extension[doc_file_extension_size] = 0;

	//		for (wchar_t *s = doc_file_extension; *s; ++s) {
	//			if ((*s >= L'A') && (*s <= L'Z'))
	//				*s = (L'a' + (*s - L'A'));
	//		}
	//	}
	//}
	AssignDocFileExt(doc_file_extension_, '|');
	AssignDocFileFormat(doc_file_format_);
}
Plugin::Plugin(wchar_t const *doc_file_format_, wchar_t const *doc_file_extensions, wchar_t separator)
	: doc_file_extension(NULL), doc_file_extension_size(0), doc_file_format(NULL), doc_file_format_size(0), signatures(NULL), n_signatures(0) {
	AssignDocFileExt(doc_file_extensions, separator);
	AssignDocFileFormat(doc_file_format_);
}
Plugin::~Plugin() {
	delete [] doc_file_extension;
	delete [] doc_file_format;
	if (signatures)
		free(signatures);
}

void Plugin::AssignDocFileExt(wchar_t const *ext_, wchar_t split) {
	cpcl::WStringPiece exts(ext_);
	if (!exts.empty()) {
		std::wstring r; r.reserve(exts.size());
		cpcl::WStringSplitIterator i(exts, split), tail;
		for(;;) {
			r += (*i).trim(cpcl::WStringPieceFromLiteral(L"\t \r\n.")).as_string();
			if (++i != tail)
				r += L'|';
			else
				break;
		}

		doc_file_extension_size = r.size();
		if (doc_file_extension_size > 0) {
			doc_file_extension = new wchar_t[doc_file_extension_size + 1];
			wmemcpy(doc_file_extension, r.c_str(), doc_file_extension_size);
			doc_file_extension[doc_file_extension_size] = 0;
			for (wchar_t *s = doc_file_extension; *s; ++s) {
				if ((*s >= L'A') && (*s <= L'Z'))
					*s = (L'a' + (*s - L'A'));
			}
		}
	}
}

bool Plugin::DocFileExtension(wchar_t const **doc_file_extension_, unsigned int *size) const {
	if (doc_file_extension_size) {
		if (doc_file_extension_)
			*doc_file_extension_ = doc_file_extension;
		if (size)
			*size = doc_file_extension_size;
		return true;
	}
	return false;
}
bool Plugin::CopyDocFileExtension(wchar_t *doc_file_extension_, unsigned int size) const {
	wchar_t const *s(NULL);
	unsigned int n(0);
	if (!DocFileExtension(&s, &n))
		return false;
	
	n = (std::min)(n, size);
	wmemcpy(doc_file_extension_, s, n);
	if ((n + 1) < size)
		doc_file_extension_[n] = 0;
	return true;
}

void Plugin::AssignDocFileFormat(wchar_t const *v) {
	cpcl::WStringPiece doc_file_format_(v);
	if (!doc_file_format_.empty()) {
		doc_file_format_size = doc_file_format_.size();
		doc_file_format = new wchar_t[doc_file_format_size + 1];
		wmemcpy(doc_file_format, v, doc_file_format_size);
		doc_file_format[doc_file_format_size] = 0;
	}
}
bool Plugin::DocFileFormat(wchar_t const **doc_file_format_, unsigned int *size) const {
	if (doc_file_format_size) {
		if (doc_file_format_)
			*doc_file_format_ = doc_file_format;
		if (size)
			*size = doc_file_format_size;
		return true;
	}
	return false;
}
bool Plugin::CopyDocFileFormat(wchar_t *doc_file_format_, unsigned int size) const {
	wchar_t const *s(NULL);
	unsigned int n(0);
	if (!DocFileFormat(&s, &n))
		return false;
	
	n = (std::min)(n, size);
	wmemcpy(doc_file_format_, s, n);
	if ((n + 1) < size)
		doc_file_format_[n] = 0;
	return true;
}

void Plugin::SetSignatures(SignatureInfo const *signatures_, unsigned int n_signatures_) {
	if (signatures) {
		free(signatures);
		signatures = NULL;
	}
	n_signatures = n_signatures_;

	signatures = make_signatures(signatures_, n_signatures);
}

bool Plugin::CheckHeader(cpcl::IOStream *stream) const {
	//if (!SignatureInfo()) // GetSignatureInfo && parse
	if (!signatures) {
		cpcl::Warning(cpcl::StringPieceFromLiteral("Plugin::CheckHeader(): SignatureInfo not set"));
		return false;
	}
	cpcl::ScopedBuf<unsigned char, 0x10> buf;
	for (unsigned int i = 0; i < n_signatures; ++i) {
		stream->Seek(signatures[i]->offset, SEEK_SET, 0);
		//if (stream->Read(buf.Alloc(signatures[i]->size), signatures[i]->size) != signatures[i]->size)
		//	Warning("Plugin::CheckHeader(): unable to read %u bytes at %u offset for check", signatures[i]->size, signatures[i]->offset);
		if (stream->Read(buf.Alloc(signatures[i]->size), signatures[i]->size) == signatures[i]->size) {
			if (memcmp(buf.Data(), signatures[i]->data, signatures[i]->size) == 0)
				return true;
		}
	}
	return false;
}

bool Plugin::GetSignatures(SignatureInfo ***signatures_, unsigned int *n_signatures_) {
	if (!signatures) {
		cpcl::Warning(cpcl::StringPieceFromLiteral("Plugin::GetSignatures(): SignatureInfo not set"));
		return false;
	}
	if (signatures_)
		*signatures_ = signatures;
	if (n_signatures_)
		*n_signatures_ = n_signatures;
	return true;
}

} // namespace plcl
