// memory_rendering_device.h
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

#ifndef __PLCL_MEMORY_RENDERING_DEVICE_H
#define __PLCL_MEMORY_RENDERING_DEVICE_H

#include "rendering_device.h"

namespace plcl {

class MemoryRenderingDevice : public RenderingDevice {
	unsigned int width, height;
	// DIBData *storage;
	RenderingData *storage;
	bool storage_owner;
public:
	MemoryRenderingDevice(RenderingData *storage_);
	MemoryRenderingDevice(unsigned int supported_pixel_formats_ = PLCL_PIXEL_FORMAT_GRAY_8 | PLCL_PIXEL_FORMAT_BGR_24 | PLCL_PIXEL_FORMAT_BGRA_32,
		unsigned int pixel_format_ = PLCL_PIXEL_FORMAT_BGRA_32);
	virtual ~MemoryRenderingDevice();

	RenderingData* Storage();
	/* free current storage if storage_ == 0 */
	void Storage(RenderingData *storage_);

	virtual bool SetViewport(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
	virtual void SweepScanline(unsigned int y, unsigned char **scanline);
	virtual void Render();
};

} // namespace plcl

#endif // __PLCL_MEMORY_RENDERING_DEVICE_H
