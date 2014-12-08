// draw_selection.h
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

#ifndef __PLCL_DRAW_SELECTION_H
#define __PLCL_DRAW_SELECTION_H

#include <agg_basics.h>
#include <agg_color_rgba.h>
#include <agg_trans_affine.h>
#include <agg_trans_viewport.h>

#include "page.h"
#include "color_conv.h"
#include "rect_container.hpp"

/*agg { 
class scanline_accessor {
 RenderingData*;
};

typedef agg::pixfmt_alpha_blend_rgb<agg::blender_rgb<agg::rgba8,  agg::order_bgr>, agg::scanline_accessor> pixfmt_type;*/

namespace plcl {

struct SelectionScanlineSweeper {
	unsigned int width_dp, height_dp;
	bool flip_y;

	std::vector<RectContainer::Rect/*agg::rect_i*/> rects;

	SelectionScanlineSweeper(unsigned int width_dp_, unsigned height_dp_, bool flip_y_);

	agg::rgba color;
	void SelectionColor(double r, double g, double b, double a = 0.5);
	void SelectionColor(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 127);
	void Draw(RenderingData *data, bool bgr_order);
private:
	void DrawImpl_bgr(agg::rect_d rect, RenderingData *data, unsigned int width, unsigned int height, int stride,
		agg::trans_affine const &mtx, agg::trans_affine const &viewport_mtx);
	void DrawImpl_rgb(agg::rect_d rect, RenderingData *data, unsigned int width, unsigned int height, int stride,
		agg::trans_affine const &mtx, agg::trans_affine const &viewport_mtx);
};

} // namespace plcl

#endif // __PLCL_DRAW_SELECTION_H
