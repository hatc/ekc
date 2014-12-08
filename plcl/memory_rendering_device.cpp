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

#include "dib_data.h"
#include "memory_rendering_device.h"

namespace plcl {

/*MemoryRenderingDevice::MemoryRenderingDevice()
	: RenderingDevice(PLCL_PIXEL_FORMAT_GRAY_8 | PLCL_PIXEL_FORMAT_BGR_24 | PLCL_PIXEL_FORMAT_BGRA_32, PLCL_PIXEL_FORMAT_BGRA_32),*/
MemoryRenderingDevice::MemoryRenderingDevice(unsigned int supported_pixel_formats_, unsigned int pixel_format_)
	: RenderingDevice(supported_pixel_formats_, pixel_format_), storage(NULL), storage_owner(true), width(0), height(0)
{}
MemoryRenderingDevice::MemoryRenderingDevice(RenderingData *storage_)
	: RenderingDevice(storage_->Pixfmt(), storage_->Pixfmt()), storage(storage_), storage_owner(false), width(0), height(0)
{}
MemoryRenderingDevice::~MemoryRenderingDevice() {
	if ((storage) && storage_owner)
		delete storage;
}

RenderingData* MemoryRenderingDevice::Storage() {
	return storage;
}
/*void MemoryRenderingDevice::Storage(RenderingData *storage_) {
	if ((storage_) && (storage_ != storage)) {
		if (storage_owner) {
			delete storage;
			storage_owner = false;
		}
		storage = storage_;
		supported_pixel_formats = pixel_format = storage->Pixfmt();
	}
}*/
void MemoryRenderingDevice::Storage(RenderingData *storage_) {
	if (storage_ != storage) {
		if (storage_owner)
			delete storage;
		storage = storage_;
		if (!(storage_owner = !storage))
			supported_pixel_formats = pixel_format = storage->Pixfmt();
	}
}

void MemoryRenderingDevice::Render()
{}

bool MemoryRenderingDevice::SetViewport(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
	if (storage) {
		return ((((x2 > x1) ? x2 : x1 - (x2 > x1) ? x1 : x2) == storage->Width())
			&& (((y2 > y1) ? y2 : y1 - (y2 > y1) ? y1 : y2) == storage->Height()));
	}

	width = ((x2 > x1) ? x2 : x1 - (x2 > x1) ? x1 : x2);
	height = ((y2 > y1) ? y2 : y1 - (y2 > y1) ? y1 : y2);
	return true;
}

void MemoryRenderingDevice::SweepScanline(unsigned int y, unsigned char **scanline) {
	if (!storage)
		storage = new DIBData(pixel_format, width, height);
	if (scanline)
		*scanline = storage->Scanline(y);
}

} // namespace plcl
