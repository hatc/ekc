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
#include "iplugindocument_impl.h"
#include "ipluginpage_impl.h"
#include "itextpage_impl.h"

#include "impl_exception_helper.hpp"

IPluginDocumentImpl::IPluginDocumentImpl(plcl::Doc *doc_) : doc(doc_)
{}
IPluginDocumentImpl::~IPluginDocumentImpl()
{}

STDMETHODIMP IPluginDocumentImpl::get_PageCount(DWORD *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = static_cast<DWORD>(doc->PageCount());
		return S_OK;
	} CATCH_EXCEPTION("IPluginDocumentImpl::get_PageCount()")
}

STDMETHODIMP IPluginDocumentImpl::GetPage(DWORD zero_based_page_number, IPluginPage **r) {
	try {
		if ((!r) || (static_cast<unsigned int>(zero_based_page_number) >= doc->PageCount()))
			return E_INVALIDARG;

		plcl::Page *page(0);
		if (!doc->GetPage(static_cast<unsigned int>(zero_based_page_number), &page))
			return E_FAIL;

		ScopedComPtr<IPluginPage> page_ptr;
		plcl::TextPage *text_page = dynamic_cast<plcl::TextPage*>(page);
		if (text_page) {
			page_ptr = new ITextPageImpl(text_page);
		} else {
			page_ptr = new IPluginPageImpl(page);
		}

		*r = page_ptr.Detach();
		return S_OK;
	} CATCH_EXCEPTION("IPluginDocumentImpl::GetPage()")
}
