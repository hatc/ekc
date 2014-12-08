// iplugin_wrapper.h
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

#ifndef __PLCL_IPLUGIN_WRAPPER_H
#define __PLCL_IPLUGIN_WRAPPER_H

#include "plugin.h"

struct IPlugin;
struct IPluginWrapper : public plcl::Plugin {
	IPluginWrapper(IPlugin *v);
  IPluginWrapper(const IPluginWrapper &r);
	operator IPlugin*() const;
	IPlugin* operator=(IPlugin *v);
	IPluginWrapper& operator=(const IPluginWrapper &r);
	virtual ~IPluginWrapper();

	virtual bool DocFileExtension(wchar_t const **doc_file_extension_, unsigned int *size) const;
	virtual bool CopyDocFileExtension(wchar_t *doc_file_extension_, unsigned int size) const;
	virtual bool LoadDoc(cpcl::IOStream *input, plcl::Doc **r) = 0;

	virtual bool DocFileFormat(wchar_t const **doc_file_format_, unsigned int *size) const;
	virtual bool CopyDocFileFormat(wchar_t *doc_file_format_, unsigned int size) const;
	virtual bool CheckHeader(cpcl::IOStream *stream) const;
	virtual bool GetSignatures(SignatureInfo ***signatures_, unsigned int *n_signatures_); // ??? not needed, if public signatures, n_signatures
private:
	IRenderingDevice *ptr;

	IRenderingDeviceWrapper();
};

#endif // __PLCL_IPLUGIN_WRAPPER_H
