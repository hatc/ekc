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
#include "irenderingdevice_wrapper.h"
#include "plugin_interface.h"

#include "plcl_exception.hpp"
/*#include <hresult_helper.hpp>
in IXWrapper replace 
FAILED(ptr->get_SupportedPixfmt(&v))
		throw plcl_exception("IRenderingDevice::get_SupportedPixfmt() fails");
with
THROW_FAILED(ptr->get_SupportedPixfmt(&v))
or just
HRESULT hr = ptr->get_SupportedPixfmt(&v)
hresult_exception::throw_formatted(hresult_exception(hr), "IRenderingDevice::get_SupportedPixfmt() fails with code: 0x%08X", hr)
or
#define THROW_FAILED_NAME(name, expr n) long THROW_FAILED_LOCAL_NAME(n) = (expr);\
if (THROW_FAILED_LOCAL_NAME(n) < 0)\
hresult_exception::throw_formatted(hresult_exception(THROW_FAILED_LOCAL_NAME(n)), name " fails with code: 0x%08X", THROW_FAILED_LOCAL_NAME(n))*/

IRenderingDeviceWrapper::IRenderingDeviceWrapper(IRenderingDevice *v)
	: RenderingDevice(PLCL_PIXEL_FORMAT_INVALID, PLCL_PIXEL_FORMAT_INVALID) {
	if (v)
		v->AddRef();
	ptr = v;
}
IRenderingDeviceWrapper::IRenderingDeviceWrapper(const IRenderingDeviceWrapper &r)
	: RenderingDevice(PLCL_PIXEL_FORMAT_INVALID, PLCL_PIXEL_FORMAT_INVALID) {
	if (r.ptr)
		r.ptr->AddRef();
	ptr = r.ptr;
}
IRenderingDeviceWrapper::operator IRenderingDevice*() const {
	return ptr;
}
IRenderingDevice* IRenderingDeviceWrapper::operator=(IRenderingDevice *v) {
	if (v)
		v->AddRef();
	if (ptr)
		ptr->Release();
	ptr = v;
	return ptr;
}
IRenderingDeviceWrapper& IRenderingDeviceWrapper::operator=(const IRenderingDeviceWrapper &r) {
	*this = r.ptr; // i.e. call IRenderingDevice* IRenderingDeviceWrapper::operator=(IRenderingDevice*)
	return *this;
}
IRenderingDeviceWrapper::~IRenderingDeviceWrapper() {
	if (ptr)
		ptr->Release();
}

unsigned int IRenderingDeviceWrapper::SupportedPixfmt() const {
	DWORD v;
	if FAILED(ptr->get_SupportedPixfmt(&v))
		throw plcl_exception("IRenderingDevice::get_SupportedPixfmt() fails");
	return static_cast<unsigned int>(v);
}

unsigned int IRenderingDeviceWrapper::Pixfmt() const {
	DWORD v;
	if FAILED(ptr->get_PixelFormat(&v))
		throw plcl_exception("IRenderingDevice::get_Pixfmt() fails");
	return static_cast<unsigned int>(v);
}
void IRenderingDeviceWrapper::Pixfmt(unsigned int v) {
	if FAILED(ptr->put_PixelFormat(static_cast<DWORD>(v)))
		throw plcl_exception("IRenderingDevice::put_Pixfmt() fails");
}

bool IRenderingDeviceWrapper::SetViewport(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
	HRESULT hr = ptr->SetViewport(x1, y1, x2, y2);
	if FAILED(hr)
		throw plcl_exception("IRenderingDevice::SetViewport() fails");
	return (S_OK == hr);
}

void IRenderingDeviceWrapper::SweepScanline(unsigned int y, unsigned char **scanline) {
	int scanline_ptr(0);
	if FAILED(ptr->SweepScanline(y, &scanline_ptr))
		throw plcl_exception("IRenderingDevice::SweepScanline() fails");
	if (scanline)
		*scanline = (unsigned char*)scanline_ptr;
}

void IRenderingDeviceWrapper::Render() {
	if FAILED(ptr->Render())
		throw plcl_exception("IRenderingDevice::Render() fails");
}
