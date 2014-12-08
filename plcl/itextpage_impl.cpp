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
#include "itextpage_impl.h"

ITextPageImpl::ITextPageImpl(plcl::TextPage *text_page_) : IBasePageImpl<ITextPage>(text_page_), text_page(text_page_)
{}
ITextPageImpl::~ITextPageImpl()
{}

STDMETHODIMP ITextPageImpl::Text(wchar_t const **r, DWORD *r_size) {
	try {
		if (!r)
			return E_INVALIDARG;

		unsigned int r_size_(0);
		bool ok = text_page->Text(r, &r_size_);

		if (ok && r_size)
			*r_size = static_cast<DWORD>(r_size_);
		return (ok) ? S_OK : S_FALSE;
	} CATCH_EXCEPTION("ITextPageImpl::Text()")
}

STDMETHODIMP ITextPageImpl::CopyText(wchar_t *v, DWORD v_size) {
	try {
		if (!v)
			return E_INVALIDARG;

		return (text_page->CopyText(v, static_cast<unsigned int>(v_size))) ? S_OK : S_FALSE;
	} CATCH_EXCEPTION("ITextPageImpl::CopyText()")
}

STDMETHODIMP ITextPageImpl::Select(DWORD start, DWORD size) {
	try {
		return (text_page->Select(static_cast<unsigned int>(start), static_cast<unsigned int>(size))) ? S_OK : S_FALSE;
	} CATCH_EXCEPTION("ITextPageImpl::Select()")
}

STDMETHODIMP ITextPageImpl::RenderSelection(IRenderingDevice *output) {
	try {
		if (!output)
			return E_INVALIDARG;

		IRenderingDeviceWrapper output_wrapper(output);

		text_page->RenderSelection(&output_wrapper);
		return S_OK;
	} CATCH_EXCEPTION("ITextPageImpl::RenderSelection()")
}
