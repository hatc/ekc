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

#include <hresult_helper.hpp>
#include <file_util.h>
#include <istream_wrapper.h>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include "ifilefilter_impl.h"

#include "impl_exception_helper.hpp"

#include "attr_id.h"

IFileFilterImpl::IFileFilterImpl(PluginFilter *plugin_filter_, int match_method_)
	: plugin_filter(plugin_filter_), match_method(match_method_), combine_mode(IFileFilterImpl::FILTER_COMBINE_OR)
{}
IFileFilterImpl::~IFileFilterImpl()
{}

STDMETHODIMP IFileFilterImpl::get_Name(BSTR *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		if (match_method)
			*v = ::SysAllocString(L"file signature filter");
		else
			*v = ::SysAllocString(L"file extension filter");

		return S_OK;
	} CATCH_EXCEPTION("IFileFilterImpl::get_Name()")
}

STDMETHODIMP IFileFilterImpl::Match(IFileItem *item) {
	try {
		if (!item)
			return E_INVALIDARG;

		ScopedComPtr<IPropertyBasket> properties;
		THROW_FAILED(item->get_Attributes(&properties));
		VARIANT v;
		::VariantInit(&v);
		if SUCCEEDED(properties->GetItem(KPID_IS_DIR, &v)) {
			if ((VT_BOOL == v.vt) && (VARIANT_TRUE == v.boolVal))
				return S_FALSE;
		}
		if (VT_EMPTY != v.vt)
			::VariantClear(&v);

		IPlugin *plugin((IPlugin*)0);
		if (match_method) {
			ScopedComPtr<IStream> stream_;
			THROW_FAILED(item->get_Stream(&stream_));
			cpcl::IStreamWrapper stream(stream_);
			plugin = plugin_filter->Match(&stream);
		} else {
			BSTR file_name;
			THROW_FAILED(item->get_Name(&file_name));
			try {
				plugin = plugin_filter->Match(cpcl::FileExtension(cpcl::WStringPiece(file_name, ::SysStringLen(file_name))));
			} catch (...) {
				::SysFreeString(file_name);
				throw;
			}
			::SysFreeString(file_name);
		}

		return (!!plugin) ? S_OK : S_FALSE;
	} CATCH_EXCEPTION("IFileFilterImpl::Match()")
}

STDMETHODIMP IFileFilterImpl::get_CombineMode(DWORD *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = combine_mode;
		return S_OK;
	} CATCH_EXCEPTION("IFileFilterImpl::get_CombineMode()")
}

STDMETHODIMP IFileFilterImpl::put_CombineMode(DWORD v) {
	try {
		if (((FILTER_COMBINE_AND | FILTER_COMBINE_OR | FILTER_COMBINE_NOT | FILTER_COMBINE_XOR) & v) == 0)
			return E_INVALIDARG;

		combine_mode = v;
		return S_OK;
	} CATCH_EXCEPTION("IFileFilterImpl::put_CombineMode()")
}
