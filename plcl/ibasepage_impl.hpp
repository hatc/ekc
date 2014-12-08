// ibasepage_impl.hpp
// Copyright (C) 2012-2013 Yuri Agafonov
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

#ifndef __PLCL_IBASE_PAGE_IMPL_HPP
#define __PLCL_IBASE_PAGE_IMPL_HPP

#include <basic.h>

#include <memory>
#include "page.h"

#include <internal_com.h>
#include "impl_exception_helper.hpp"
#include "irenderingdevice_wrapper.h"

template<class Base>
class IBasePageImpl : public Base {
	// why not class IBasePageImpl : public Base[, public CUnknownImp] ? because object can use more that one BaseImpl classes, 
	// for example public IListImpl<IList>, public IPropertyBagImpl, so each BaseImpl class must provide only methods relative to it interface
	DISALLOW_IMPLICIT_CONSTRUCTORS(IBasePageImpl);
protected:
	std::auto_ptr<plcl::Page> page;
public:
	IBasePageImpl(plcl::Page *page_) : page(page_)
	{}
	/*virtual - why virtual dtor, then dtor called through Release { delete this } i.e. no vtable dispatch???*/~IBasePageImpl()
	{}

	STDMETHOD(get_Width)(DWORD *v) {
		try {
			if (!v)
				return E_INVALIDARG;
			
			*v = static_cast<DWORD>(page->Width());
			return S_OK;
		} CATCH_EXCEPTION("IBasePageImpl::get_Width()")
	}

	STDMETHOD(put_Width)(DWORD v) {
		try {
			page->Width(static_cast<unsigned int>(v));
			return S_OK;
		} CATCH_EXCEPTION("IBasePageImpl::put_Width()")
	}

	STDMETHOD(get_Height)(DWORD *v) {
		try {
			if (!v)
				return E_INVALIDARG;
			
			*v = static_cast<DWORD>(page->Height());
			return S_OK;
		} CATCH_EXCEPTION("IBasePageImpl::get_Height()")
	}

	STDMETHOD(put_Height)(DWORD v) {
		try {
			page->Height(static_cast<unsigned int>(v));
			return S_OK;
		} CATCH_EXCEPTION("IBasePageImpl::put_Height()")
	}

	STDMETHOD(get_Zoom)(DWORD *v) {
		try {
			if (!v)
				return E_INVALIDARG;
			
			*v = static_cast<DWORD>(page->Zoom());
			return S_OK;
		} CATCH_EXCEPTION("IBasePageImpl::get_Zoom()")
	}

	STDMETHOD(put_Zoom)(DWORD v) {
		try {
			page->Zoom(static_cast<unsigned int>(v));
			return S_OK;
		} CATCH_EXCEPTION("IBasePageImpl::put_Zoom()")
	}

	STDMETHOD(get_Dpi)(double *v) {
		try {
			if (!v)
				return E_INVALIDARG;
			
			*v = page->Dpi();
			return S_OK;
		} CATCH_EXCEPTION("IBasePageImpl::get_Dpi()")
	}

	STDMETHOD(put_Dpi)(double v) {
		try {
			page->Dpi(v);
			return S_OK;
		} CATCH_EXCEPTION("IBasePageImpl::put_Dpi()")
	}

	STDMETHOD(Render)(IRenderingDevice *output) {
		try {
			if (!output)
				return E_INVALIDARG;
			
			IRenderingDeviceWrapper output_wrapper(output);
			
			page->Render(&output_wrapper);
			return S_OK;
		} CATCH_EXCEPTION("IPluginPageImpl::Render()")
	}
	
	STDMETHOD(get_Orientation(DWORD *v)) {
		try {
			if (!v)
				return E_INVALIDARG;
			
			*v = static_cast<DWORD>(page->ExifOrientation());
			return S_OK;
		} CATCH_EXCEPTION("IBasePageImpl::get_Orientation()")
	}

	STDMETHOD(put_Orientation(DWORD v)) {
		try {
			page->ExifOrientation(static_cast<unsigned int>(v));
			return S_OK;
		} CATCH_EXCEPTION("IBasePageImpl::put_Orientation()")
	}
};

#endif // __PLCL_IBASE_PAGE_IMPL_HPP
