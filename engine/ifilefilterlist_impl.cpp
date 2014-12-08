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

#include <istream_wrapper.h>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include "ifilefilterlist_impl.h"
#include "ifilefilter_impl.h" // IFileFilterImpl::FILTER_COMBINE_MODE
#include "bstr_wrapper.hpp"

#include "impl_exception_helper.hpp"

#include "attr_id.h"

IFileFilterListImpl::IFileFilterListImpl()
{}
IFileFilterListImpl::~IFileFilterListImpl()
{}

STDMETHODIMP IFileFilterListImpl::FindItem(BSTR filter_name, IFileFilter **v) {
	try {
		if (!filter_name)
			return E_INVALIDARG;

		cpcl::ComPtr<IFileFilter> r;
		cpcl::WStringPiece filter_name_(filter_name, ::SysStringLen(filter_name));
		for (size_t i = 0, size = container.size(); i < size; ++i) {
			cpcl::BSTRWrapper s;
			THROW_FAILED(container[i]->get_Name(s.ReleaseAndGetAddressOf()));

			if ((cpcl::WStringPiece)s == filter_name_) {
				r = container[i];
				break;
			}
		}
		if (!r)
			return S_FALSE;

		if (v)
			*v = r.Detach();
		return S_OK;
	} CATCH_EXCEPTION("IFileFilterListImpl::FindItem()")
}

inline bool match(cpcl::ComPtr<IFileFilter> const &filter, IFileItem *item, DWORD *combine_mode) {
	HRESULT hr = filter->Match(item);
	if (hr < 0) {
		hresult_exception::throw_formatted(hresult_exception(hr),
			"filter->Match(item) fails with code: 0x%08X",
			hr);
	}
	bool r = hr == S_OK;

	DWORD combine_mode_;
	THROW_FAILED(filter->get_CombineMode(&combine_mode_));
	if (combine_mode_ & IFileFilterImpl::FILTER_COMBINE_NOT)
		r = !r;

	if (combine_mode)
		*combine_mode = combine_mode_;
	return r;
}
STDMETHODIMP IFileFilterListImpl::Match(IFileItem *item) {
	try {
		if (!item)
			return E_INVALIDARG;

		cpcl::ComPtr<IPropertyBasket> item_properties;
		THROW_FAILED(item->QueryInterface(item_properties.GetAddressOf()));

		VARIANT variant;
		{
			::VariantInit(&variant);
			variant.vt = VT_BOOL; variant.boolVal = VARIANT_TRUE;
			THROW_FAILED(item_properties->SetItem(KPID_CACHE_STREAM, variant));
		}

		bool matched(true);
		Container::const_iterator i = container.begin();
		if (i != container.end()) {
			matched = match(*i++, item, 0);
		}

		for (; i != container.end(); ++i) {
			DWORD combine_mode;
			bool r = match(*i, item, &combine_mode);
			if (combine_mode & IFileFilterImpl::FILTER_COMBINE_AND) {
				matched = matched & r;
				if (!matched)
					break;
			} else if (combine_mode & IFileFilterImpl::FILTER_COMBINE_OR) {
				matched = matched | r;
			} else if (combine_mode & IFileFilterImpl::FILTER_COMBINE_XOR) {
				matched = matched ^ r;
			}
		}

		{
			variant.vt = VT_BOOL; variant.boolVal = VARIANT_FALSE;
			THROW_FAILED(item_properties->SetItem(KPID_CACHE_STREAM, variant));

			cpcl::ComPtr<IStream> cached_stream;
			item->get_Stream(cached_stream.GetAddressOf());
			// DUMBASS_CHECK(!cached_stream && hr = S_FALSE)

			THROW_FAILED(item_properties->RemoveItem(KPID_CACHE_STREAM));
		}

		return (matched) ? S_OK : S_FALSE;
	} CATCH_EXCEPTION("IFileFilterListImpl::Match()")
}
