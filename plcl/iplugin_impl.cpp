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

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include "iplugin_impl.h"
#include "iplugindocument_impl.h"

#include "impl_exception_helper.hpp"

IPluginImpl::IPluginImpl(plcl::Plugin *plugin_) : plugin(plugin_)
{}
IPluginImpl::~IPluginImpl()
{}

STDMETHODIMP IPluginImpl::DocFileExtension(wchar_t const **r, DWORD *r_size) {
	try {
		unsigned int r_size_(0);
		bool ok = plugin->DocFileExtension(r, &r_size_);
		if (r_size)
			*r_size = static_cast<DWORD>(r_size_);
		return (ok) ? S_OK : S_FALSE;
	} CATCH_EXCEPTION("IPluginImpl::DocFileExtension()")
}

STDMETHODIMP IPluginImpl::CopyDocFileExtension(wchar_t *v, DWORD v_size) {
	try {
		if (!v)
			return E_INVALIDARG;

		return (plugin->CopyDocFileExtension(v, static_cast<unsigned int>(v_size))) ? S_OK : S_FALSE;
	} CATCH_EXCEPTION("IPluginImpl::CopyDocFileExtension()")
}

STDMETHODIMP IPluginImpl::DocFileFormat(wchar_t const **r, DWORD *r_size) {
	try {
		unsigned int r_size_(0);
		bool ok = plugin->DocFileFormat(r, &r_size_);
		if (r_size)
			*r_size = static_cast<DWORD>(r_size_);
		return (ok) ? S_OK : S_FALSE;
	} CATCH_EXCEPTION("IPluginImpl::DocFileExtension()")
}

STDMETHODIMP IPluginImpl::CopyDocFileFormat(wchar_t *v, DWORD v_size) {
	try {
		if (!v)
			return E_INVALIDARG;

		return (plugin->CopyDocFileFormat(v, static_cast<unsigned int>(v_size))) ? S_OK : S_FALSE;
	} CATCH_EXCEPTION("IPluginImpl::CopyDocFileFormat()")
}

STDMETHODIMP IPluginImpl::CheckHeader(IStream *stream) {
	try {
		if (!stream)
			return E_INVALIDARG;

		cpcl::IStreamWrapper stream_(stream);
		return (plugin->CheckHeader(&stream_)) ? S_OK : S_FALSE;
	} CATCH_EXCEPTION("IPluginImpl::CheckHeader()")
}

STDMETHODIMP IPluginImpl::GetSignatures(int *signatures_ptr, DWORD *n_signatures) {
	try {
		if (!signatures_ptr && !n_signatures)
			return E_INVALIDARG;

		plcl::Plugin::SignatureInfo **signatures;
		unsigned int n_signatures_;
		if (!plugin->GetSignatures(&signatures, &n_signatures_))
			return E_FAIL;

		if (signatures_ptr)
			*signatures_ptr = (int)signatures;
		if (n_signatures)
			*n_signatures = static_cast<DWORD>(n_signatures_);
		return S_OK;
	} CATCH_EXCEPTION("IPluginImpl::GetSignatures()")
}

STDMETHODIMP IPluginImpl::LoadDocument(IStream *input, IPluginDocument **r) {
	try {
		if ((!input) || (!r))
			return E_INVALIDARG;

		cpcl::IStreamWrapper input_(input);
		plcl::Doc *doc(0);
		if (!plugin->LoadDoc(&input_, &doc)) {
			*r = 0;
			return S_FALSE;
		}
		ScopedComPtr<IPluginDocument> doc_ptr(new IPluginDocumentImpl(doc));
		*r = doc_ptr.Detach();
		return S_OK;
	} CATCH_EXCEPTION("IPluginImpl::LoadDocument()")
}
