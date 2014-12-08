// bmp_page.hpp
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

#include <scoped_buf.hpp>
#include <io_stream.h>

#include <page.h>
#include <render.hpp>

template<class Reader>
class BmpPage : public plcl::Page {
	std::auto_ptr<cpcl::IOStream> input;
public:
	typedef typename Reader::ReaderSettings Settings;
	Settings settings;

	BmpPage(std::auto_ptr<cpcl::IOStream> &input_, Settings const &settings_) : plcl::Page(settings_.width, settings_.height, 96),
		input(input_), settings(settings_)
	{}
	virtual ~BmpPage()
	{}

	virtual void Render(plcl::RenderingDevice *output) {
		if (!input->Seek(settings.data_offset, SEEK_SET, 0)) {
			plcl_exception::throw_formatted(plcl_exception(),
				"BmpPage::Render(): IOStream::Seek() fails, offset(%u)",
				settings.data_offset);
		}

		cpcl::ScopedBuf<unsigned char, 0x1000> render_buf_resample;
		cpcl::ScopedBuf<unsigned char, 0x800> render_buf_conv;
		cpcl::ScopedBuf<unsigned char, 0x1000> reader_buf;
		Reader reader(input.get(), settings, reader_buf);
		bool mirror_x, mirror_y; unsigned int angle;
		plcl::Page::ExifOrientation(ExifOrientation(), &mirror_x, &mirror_y, &angle);
		// plcl::Render(reader, width_dp, height_dp, settings.pixfmt, output, width, height, settings.flip_y, render_buf_resample, render_buf_conv);
		plcl::Render(reader, width_dp, height_dp, settings.pixfmt, output, width, height, settings.flip_y, render_buf_resample, render_buf_conv,
			mirror_x, mirror_y, angle);
	}
};
