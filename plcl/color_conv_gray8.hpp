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

#ifndef __PLCL_COLOR_CONV_GRAY8
#define __PLCL_COLOR_CONV_GRAY8

#include <agg_basics.h>
#include <util/agg_color_conv_rgb8.h>

namespace agg
{
	template<int R, int B> class color_conv_gray8_rgb24
	{
	public:
		void operator()(int8u* dst, const int8u* src, unsigned width) const
		{
			do
			{
				dst[R] = dst[1] = dst[B] = *src++;
				dst += 3;
			}
			while(--width);
		}
	};

	typedef color_conv_gray8_rgb24<0,2> color_conv_gray8_to_rgb24; //----color_conv_gray8_rgb24
	typedef color_conv_gray8_rgb24<2,0> color_conv_gray8_to_bgr24; //----color_conv_gray8_bgr24
}

#endif // __PLCL_COLOR_CONV_GRAY8
