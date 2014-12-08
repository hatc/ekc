// itextpage_impl.h
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

#ifndef __PLCL_ITEXT_PAGE_IMPL_H
#define __PLCL_ITEXT_PAGE_IMPL_H

#include "ibasepage_impl.hpp"
#include "text_page.h"

#include "plugin_interface.h"

class ITextPageImpl : public IBasePageImpl<ITextPage>, public CUnknownImp {
	plcl::TextPage *text_page; // IBasePageImpl<ITextPage> already own page instance

	DISALLOW_IMPLICIT_CONSTRUCTORS(ITextPageImpl);
public:
	ITextPageImpl(plcl::TextPage *text_page_);
	/*virtual - why virtual dtor, then dtor called through Release { delete this } i.e. no vtable dispatch???*/~ITextPageImpl();
	
	STDMETHOD(Text)(wchar_t const **r, DWORD *r_size);
	STDMETHOD(CopyText)(wchar_t *v, DWORD v_size);
	STDMETHOD(Select)(DWORD start, DWORD size);
	STDMETHOD(RenderSelection)(IRenderingDevice *output);

	CPCL_UNKNOWN_IMP2_MT(IPluginPage, ITextPage)
};

#endif // __PLCL_ITEXT_PAGE_IMPL_H
