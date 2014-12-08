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
#include "ipluginpage_impl.h"

IPluginPageImpl::IPluginPageImpl(plcl::Page *page_) : IBasePageImpl<IPluginPage>(page_)
{}
IPluginPageImpl::~IPluginPageImpl()
{}

//STDMETHODIMP IPluginPageImpl::get_Width(DWORD *v) {
//	try {
//		if (!v)
//			return E_INVALIDARG;
//
//		*v = static_cast<DWORD>(page->Width());
//		return S_OK;
//	} CATCH_EXCEPTION("IPluginPageImpl::get_Width()")
//}
//
//STDMETHODIMP IPluginPageImpl::put_Width(DWORD v) {
//	try {
//		page->Width(static_cast<unsigned int>(v));
//		return S_OK;
//	} CATCH_EXCEPTION("IPluginPageImpl::put_Width()")
//}
//
//STDMETHODIMP IPluginPageImpl::get_Height(DWORD *v) {
//	try {
//		if (!v)
//			return E_INVALIDARG;
//
//		*v = static_cast<DWORD>(page->Height());
//		return S_OK;
//	} CATCH_EXCEPTION("IPluginPageImpl::get_Height()")
//}
//
//STDMETHODIMP IPluginPageImpl::put_Height(DWORD v) {
//	try {
//		page->Height(static_cast<unsigned int>(v));
//		return S_OK;
//	} CATCH_EXCEPTION("IPluginPageImpl::put_Height()")
//}
//
//STDMETHODIMP IPluginPageImpl::get_Zoom(DWORD *v) {
//	try {
//		if (!v)
//			return E_INVALIDARG;
//
//		*v = static_cast<DWORD>(page->Zoom());
//		return S_OK;
//	} CATCH_EXCEPTION("IPluginPageImpl::get_Zoom()")
//}
//
//STDMETHODIMP IPluginPageImpl::put_Zoom(DWORD v) {
//	try {
//		page->Zoom(static_cast<unsigned int>(v));
//		return S_OK;
//	} CATCH_EXCEPTION("IPluginPageImpl::put_Zoom()")
//}
//
//STDMETHODIMP IPluginPageImpl::get_Dpi(double *v) {
//	try {
//		if (!v)
//			return E_INVALIDARG;
//
//		*v = page->Dpi();
//		return S_OK;
//	} CATCH_EXCEPTION("IPluginPageImpl::get_Dpi()")
//}
//
//STDMETHODIMP IPluginPageImpl::put_Dpi(double v) {
//	try {
//		page->Dpi(v);
//		return S_OK;
//	} CATCH_EXCEPTION("IPluginPageImpl::put_Dpi()")
//}
//
//STDMETHODIMP IPluginPageImpl::Render(IRenderingDevice *output) {
//	try {
//		if (!output)
//			return E_INVALIDARG;
//
//		IRenderingDeviceWrapper output_wrapper(output);
//
//		page->Render(&output_wrapper);
//		return S_OK;
//	} CATCH_EXCEPTION("IPluginPageImpl::Render()")
//}
