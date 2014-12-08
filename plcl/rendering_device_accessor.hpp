// rendering_device_accessor.hpp
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

#include <agg_basics.h>

#include "rendering_device.h"

namespace agg {

class rendering_device_accessor {
public:
	typedef const_row_info<int8u> row_data;

	rendering_device_accessor() : m_width(0), m_height(0), m_stride(0), output(0), flip_y(false)
	{}

	rendering_device_accessor(unsigned int width, unsigned int height, int stride, plcl::RenderingDevice *output_, bool flip_y_)
		: m_width(width), m_height(height), m_stride(stride), output(output_), flip_y(flip_y_)
	{}

	//--------------------------------------------------------------------
	AGG_INLINE       unsigned width()  const { return m_width;  }
	AGG_INLINE       unsigned height() const { return m_height; }
	AGG_INLINE       int      stride() const { return m_stride; }
	AGG_INLINE       unsigned stride_abs() const 
	{
		return (m_stride < 0) ? unsigned(-m_stride) : unsigned(m_stride);
	}

	//--------------------------------------------------------------------
	
	AGG_INLINE       int8u* row_ptr(int y)       
	{
		int8u *scanline;
		unsigned int y_ = static_cast<unsigned int>(y);
		output->SweepScanline(flip_y ? m_height - y_ - 1 : y_, &scanline);
		return scanline;
	}
	AGG_INLINE const int8u* row_ptr(int y) const 
	{
		int8u *scanline;

		unsigned int y_ = static_cast<unsigned int>(y);
		output->SweepScanline(flip_y ? m_height - y_ - 1 : y_, &scanline);
		return scanline;
	}
	AGG_INLINE       int8u* row_ptr(int, int y, unsigned) { return row_ptr(y); }
	AGG_INLINE       row_data row(int y) const 
	{ 
		return row_data(0, m_width-1, row_ptr(y)); 
	}

	//--------------------------------------------------------------------
	template<class RenBuf>
	void copy_from(const RenBuf& src)
	{
		unsigned h = height();
		if (src.height() < h)
			h = src.height();

		unsigned l = stride_abs();
		if (src.stride_abs() < l)
			l = src.stride_abs();

		l *= sizeof(int8u);

		unsigned y;
		unsigned w = width();
		for (y = 0; y < h; y++)
		{
			memcpy(row_ptr(0, y, w), src.row_ptr(y), l);
		}
	}

	//--------------------------------------------------------------------
	void clear(int8u value)
	{
		unsigned y;
		unsigned w = width();
		unsigned stride = stride_abs();
		for (y = 0; y < height(); y++)
		{
			int8u *p = row_ptr(0, y, w);
			unsigned x;
			for(x = 0; x < stride; x++)
			{
				*p++ = value;
			}
		}
	}

private:
	plcl::RenderingDevice *output;
	unsigned      m_width;  // Width in pixels
	unsigned      m_height; // Height in pixels
	int           m_stride; // Number of bytes per row. Can be < 0
	bool          flip_y;
};

}
