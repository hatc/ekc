// render.hpp
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

#include <memory>

#include "agg_resample.h"
#include "scanline_reader_adapter.hpp"
#include "color_conv_adapter.hpp"

#include "memory_rendering_device.h"
#include "affine_trans.h"

inline unsigned int calc_zoom(unsigned int &output_width, unsigned int &output_height, void (*change_size)(unsigned int &width, unsigned int &height)) {
	unsigned int r(100);
	if (change_size) {
		unsigned int width = output_width, height = output_height;
		change_size(width, height);
		if (width != output_width) {
			r = (unsigned int)(100.0 * (double(width) / double(output_width)));
			output_height = (unsigned int)(double(output_height * width) / double(output_width));
			output_width = width;
		} else if (height != output_height) {
			r = (unsigned int)(100.0 * (double(height) / double(output_height)));
			output_width = (unsigned int)(double(output_width * height) / double(output_height));
			output_height = height;
		}
	}
	return r;
}

namespace plcl {

struct RenderingDataReader {
	RenderingData *data;
	unsigned int v;
	unsigned int stride;

	RenderingDataReader(RenderingData *data_) : data(data_), v((unsigned int)-1) {
		stride = RenderingData::Stride(data->Pixfmt(), data->Width());
	}
	void Read(unsigned char *scanline, unsigned int *y_) {
		if (v == (unsigned int)-1)
			v = 0;
		else if (v >= data->Height()) {
			plcl_exception::throw_formatted(plcl_exception(),
				"RenderingDataReader::Read(): current row(%u) >= height(%u)",
				v, data->Height());
		}

		//memcpy(scanline, data->Scanline(v++), (size_t)data->Stride()); // data padded
		memcpy(scanline, data->Scanline(v++), stride);
		if (y_)
			*y_ = v - 1;
	}
	void Skip(unsigned int lines) {
		if (v == (unsigned int)-1)
			v = 0;

		if (v + lines >= data->Height()) {
			plcl_exception::throw_formatted(plcl_exception(),
				"RenderingDataReader::Skip(): row(%u) >= height(%u)",
				v + lines, data->Height());
		}
		v += lines;
	}
};

template<class ScanlineReader/*CopyConstructible, - Copyable*/, class ResampleStorage>
inline void Render_(ScanlineReader scanline_reader, unsigned int input_width, unsigned int input_height, int pixfmt,
	RenderingDevice *output, unsigned int output_width, unsigned int output_height, bool flip_y,
	ResampleStorage &resample_buf) {
	if ((input_width != output_width) || (input_height != output_height)) {
		plcl::RenderingData *adapter_ = make_adapter(scanline_reader, input_width, input_height, pixfmt, resample_buf);
		std::auto_ptr<plcl::RenderingData> adapter(adapter_);
		
		plcl::Resample(adapter.get(), output_width, output_height, output, flip_y);
	} else {
		output->Pixfmt(pixfmt);
		output->SetViewport(0, 0, output_width, output_height);
		for (unsigned int y = 0; y < output_height; ++y) {
			unsigned char *scanline;
			output->SweepScanline(flip_y ? output_height - y - 1 : y, &scanline);
			scanline_reader.Read(scanline, NULL);
		}
	}
	output->Render();
}
template<class ScanlineReader/*CopyConstructible, - Copyable*/, class ResampleStorage, class ConvStorage>
inline void Render(ScanlineReader scanline_reader, unsigned int input_width, unsigned int input_height, int pixfmt,
	RenderingDevice *output, unsigned int output_width, unsigned int output_height, bool flip_y,
	ResampleStorage &resample_buf, ConvStorage &conv_buf) {
	unsigned int supported_pixfmt = output->SupportedPixfmt();
	unsigned int pixfmt_ = find_acceptable_pixfmt(pixfmt, supported_pixfmt);
	if (pixfmt_ != pixfmt) {
		Render_(make_conv_adapter(scanline_reader, input_width, pixfmt, pixfmt_, conv_buf), input_width, input_height, pixfmt_,
			output, output_width, output_height, flip_y, resample_buf);
	} else {
		Render_(scanline_reader, input_width, input_height, pixfmt,
			output, output_width, output_height, flip_y, resample_buf);
	}
}

/* recursion with templates a little weird... */
#if 0
template<class ScanlineReader/*CopyConstructible, - Copyable*/, class ResampleStorage, class ConvStorage>
inline void Render(ScanlineReader scanline_reader, unsigned int input_width, unsigned int input_height, int pixfmt,
	RenderingDevice *output, unsigned int output_width, unsigned int output_height, bool flip_y,
	ResampleStorage &resample_buf, ConvStorage &conv_buf) {
	unsigned int supported_pixfmt = output->SupportedPixfmt();
	unsigned int pixfmt_ = find_acceptable_pixfmt(pixfmt, supported_pixfmt);
	if (pixfmt_ != pixfmt) {
		Render(make_conv_adapter(scanline_reader, input_width, pixfmt, pixfmt_, conv_buf), input_width, input_height, pixfmt_,
			output, output_width, output_height, flip_y, resample_buf, conv_buf);
	} else {
		if ((input_width != output_width) || (input_height != output_height)) {
			plcl::RenderingData *adapter_ = plcl::make_adapter(scanline_reader, input_width, input_height, pixfmt, resample_buf);
			std::auto_ptr<plcl::RenderingData> adapter(adapter_);
			
			plcl::Resample(adapter.get(), output_width, output_height, output, flip_y);
		} else {
			output->Pixfmt(pixfmt);
			output->SetViewport(0, 0, output_width, output_height);
			for (unsigned int y = 0; y < output_height; ++y) {
				unsigned char *scanline;
				output->SweepScanline(flip_y ? output_height - y - 1 : y, &scanline);
				scanline_reader.Read(scanline, NULL);
			}
		}
		output->Render();
	}
}
#endif

template<class ScanlineReader/*CopyConstructible, - Copyable*/, class ResampleStorage, class ConvStorage>
inline void Render(ScanlineReader scanline_reader, unsigned int input_width, unsigned int input_height, int pixfmt,
	RenderingDevice *output_, unsigned int output_width, unsigned int output_height, bool flip_y,
	ResampleStorage &resample_buf, ConvStorage &conv_buf,
	bool mirror_x, bool mirror_y, unsigned int angle) {
	if (mirror_y)
		flip_y ^= true;
	MemoryRenderingDevice memory_rendering_device(output_->SupportedPixfmt(), output_->Pixfmt());
	RenderingDevice *output = output_;
	if (mirror_x || mirror_y || angle != 0)
		output = &memory_rendering_device;

	Render(scanline_reader, input_width, input_height, pixfmt,
		output, output_width, output_height, flip_y,
		resample_buf, conv_buf);

	if (mirror_x || mirror_y || angle != 0) {
		output = output_;
		RenderingData *data = memory_rendering_device.Storage();
		if (mirror_x)
			MirrorX(data);
		if (angle != 0) {
			Rotate(data, angle, output); // width, height - original, just use values from RenderingData 
			output->Render();
		}
		else
			Render_(RenderingDataReader(data), data->Width(), data->Height(), data->Pixfmt(),
			output, output_width, output_height, false, resample_buf);
	}
}

} // namespace plcl
