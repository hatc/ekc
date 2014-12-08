// irenderingdevice_wrapper.h
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

#ifndef __PLCL_IRENDERING_DEVICE_WRAPPER_H
#define __PLCL_IRENDERING_DEVICE_WRAPPER_H

#include "rendering_device.h"

struct IRenderingDevice;
struct IRenderingDeviceWrapper : public plcl::RenderingDevice {
	IRenderingDeviceWrapper(IRenderingDevice *v);
  IRenderingDeviceWrapper(const IRenderingDeviceWrapper &r);
	operator IRenderingDevice*() const;
	IRenderingDevice* operator=(IRenderingDevice *v);
	IRenderingDeviceWrapper& operator=(const IRenderingDeviceWrapper &r);
	virtual ~IRenderingDeviceWrapper();

	virtual unsigned int SupportedPixfmt() const;
	virtual unsigned int Pixfmt() const;
	virtual void Pixfmt(unsigned int v);

	virtual bool SetViewport(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
	virtual void SweepScanline(unsigned int y, unsigned char **scanline);
	virtual void Render();
private:
	IRenderingDevice *ptr;

	IRenderingDeviceWrapper();
};

#endif // __PLCL_IRENDERING_DEVICE_WRAPPER_H
