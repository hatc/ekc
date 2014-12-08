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

#include "irenderingdata_wrapper.h"
#include "plugin_interface.h"

#include "plcl_exception.hpp"

IRenderingDataWrapper::IRenderingDataWrapper(IRenderingData *v) : plcl::RenderingData(PLCL_PIXEL_FORMAT_INVALID) {
	if (v)
		v->AddRef();
	data = v;
}
IRenderingDataWrapper::IRenderingDataWrapper(const IRenderingDataWrapper &r) : plcl::RenderingData(PLCL_PIXEL_FORMAT_INVALID) {
	if (r.data)
		r.data->AddRef();
	data = r.data;
}
IRenderingDataWrapper::operator IRenderingData*() const {
	return data;
}
IRenderingData* IRenderingDataWrapper::operator=(IRenderingData *v) {
	if (v)
		v->AddRef();
	if (data)
		data->Release();
	data = v;
	return data;
}
IRenderingDataWrapper& IRenderingDataWrapper::operator=(const IRenderingDataWrapper &r) {
	*this = r.data; // i.e. call IRenderingData* IRenderingDataWrapper::operator=(IRenderingData*)
	return *this;
}
IRenderingDataWrapper::~IRenderingDataWrapper() {
	if (data)
		data->Release();
}

unsigned int IRenderingDataWrapper::Width() const {
	DWORD v;
	if FAILED(data->get_Width(&v))
		throw plcl_exception("IRenderingData::get_Width() fails");
	return static_cast<unsigned int>(v);
}

unsigned int IRenderingDataWrapper::Height() const {
	DWORD v;
	if FAILED(data->get_Height(&v))
		throw plcl_exception("IRenderingData::get_Height() fails");
	return static_cast<unsigned int>(v);
}

int IRenderingDataWrapper::Stride() const {
	DWORD v;
	if FAILED(data->get_Stride(&v))
		throw plcl_exception("IRenderingData::get_Stride() fails");
	return static_cast<int>(v);
}

unsigned int IRenderingDataWrapper::Pixfmt() const {
	DWORD v;
	if FAILED(data->get_PixelFormat(&v))
		throw plcl_exception("IRenderingData::get_PixelFormat() fails");
	return static_cast<unsigned int>(v);
}

///* suck, shit, blyaha */
//void IRenderingDataWrapper::Pixfmt(unsigned int v) {
//	if FAILED(data->put_PixelFormat(static_cast<DWORD>(v)))
//		throw plcl_exception("IRenderingData::put_PixelFormat() fails");
//}

unsigned int IRenderingDataWrapper::BitsPerPixel() const {
	DWORD v;
	if FAILED(data->get_BitsPerPixel(&v))
		throw plcl_exception("IRenderingData::get_BitsPerPixel() fails");
	return static_cast<unsigned int>(v);
}

unsigned char* IRenderingDataWrapper::Scanline(unsigned int y) {
	/*unsigned char *v;
	if FAILED(data->Scanline(static_cast<DWORD>(y), &v))
		throw plcl_exception("IRenderingData::Scanline() fails");*/
	int scanline_ptr;
	if FAILED(data->Scanline(static_cast<DWORD>(y), &scanline_ptr))
		throw plcl_exception("IRenderingData::Scanline() fails");
	return (unsigned char*)scanline_ptr;
}
