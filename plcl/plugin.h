// plugin.h
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

#ifndef __PLCL_PLUGIN_H
#define __PLCL_PLUGIN_H

#include <istream_wrapper.h>
#include "doc.h"

namespace plcl {

class Plugin {
public:
	struct SignatureInfo {
		unsigned int size;
		unsigned int offset;
		unsigned char const *data;
	};
private:
	void AssignDocFileExt(wchar_t const *ext_, wchar_t split);
	void AssignDocFileFormat(wchar_t const *v); // ;)))
protected:
	unsigned int doc_file_extension_size;
	wchar_t *doc_file_extension;
	unsigned int doc_file_format_size;
	wchar_t *doc_file_format;
	SignatureInfo **signatures;
	unsigned int n_signatures;

	void SetSignatures(SignatureInfo const *signatures_, unsigned int n_signatures_);
public:
	Plugin(wchar_t const *doc_file_format_, wchar_t const *doc_file_extension_);
	Plugin(wchar_t const *doc_file_format_, wchar_t const *doc_file_extensions, wchar_t separator);
	virtual ~Plugin();

	virtual bool DocFileExtension(wchar_t const **doc_file_extension_, unsigned int *size) const;
	virtual bool CopyDocFileExtension(wchar_t *doc_file_extension_, unsigned int size) const;
	/*virtual bool LoadDoc(IStreamWrapper input, Doc **r) = 0;*/
	virtual bool LoadDoc(cpcl::IOStream *input, Doc **r) = 0;

	virtual bool DocFileFormat(wchar_t const **doc_file_format_, unsigned int *size) const;
	virtual bool CopyDocFileFormat(wchar_t *doc_file_format_, unsigned int size) const;
	virtual bool CheckHeader(cpcl::IOStream *stream) const;
	virtual bool GetSignatures(SignatureInfo ***signatures_, unsigned int *n_signatures_); // ??? not needed, if public signatures, n_signatures
};

} // namespace plcl

#endif // __PLCL_PLUGIN_H
