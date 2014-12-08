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
#include "irenderingdata_impl.h"

#include "impl_exception_helper.hpp"

IRenderingDataImpl::IRenderingDataImpl(plcl::RenderingData *data_) : data(data_)
{}
IRenderingDataImpl::~IRenderingDataImpl()
{}

STDMETHODIMP IRenderingDataImpl::get_Width(DWORD *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = static_cast<DWORD>(data->Width());
		return S_OK;
	} CATCH_EXCEPTION("IRenderingDataImpl::get_Width()")
}

STDMETHODIMP IRenderingDataImpl::get_Height(DWORD *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = static_cast<DWORD>(data->Height());
		return S_OK;
	} CATCH_EXCEPTION("IRenderingDataImpl::get_Height()")
}

STDMETHODIMP IRenderingDataImpl::get_Stride(DWORD *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = static_cast<DWORD>(data->Stride());
		return S_OK;
	} CATCH_EXCEPTION("IRenderingDataImpl::get_Stride()")
}

STDMETHODIMP IRenderingDataImpl::get_PixelFormat(DWORD *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = static_cast<DWORD>(data->Pixfmt());
		return S_OK;
	} CATCH_EXCEPTION("IRenderingDataImpl::get_PixelFormat()")
}

///* suuucckkkkss */
//STDMETHODIMP IRenderingDataImpl::put_PixelFormat(DWORD v) {
//	return E_NOTIMPL;
//	/*try {
//		data->Pixfmt(static_cast<unsigned int>(v));
//		return S_OK;
//	} CATCH_EXCEPTION("IRenderingDataImpl::put_PixelFormat()")*/
//}

STDMETHODIMP IRenderingDataImpl::get_BitsPerPixel(DWORD *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = static_cast<DWORD>(data->BitsPerPixel());
		return S_OK;
	} CATCH_EXCEPTION("IRenderingDataImpl::get_BitsPerPixel()")
}

//STDMETHODIMP IRenderingDataImpl::Scanline(DWORD y, unsigned char **r) {
STDMETHODIMP IRenderingDataImpl::Scanline(DWORD y, int *scanline_ptr) {
	try {
		if (!scanline_ptr)
			return E_INVALIDARG;

		unsigned char *scanline = data->Scanline(static_cast<unsigned int>(y));
		*scanline_ptr = (int)scanline;
		return S_OK;
	} CATCH_EXCEPTION("IRenderingDataImpl::Scanline()")
}
