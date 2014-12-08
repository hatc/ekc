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
#include <agg_color_rgba.h>
#include <agg_gamma_lut.h>
#include <agg_pixfmt_gray.h>
#include <agg_pixfmt_rgb.h>
#include <agg_pixfmt_rgba.h>
#include <agg_rendering_buffer.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_scanline_u.h>
#include <agg_renderer_scanline.h>
//#include <agg_span_allocator.h>
//#include <agg_span_interpolator_linear.h>
//#include <agg_image_accessors.h>
#include <agg_conv_transform.h>
#include <agg_trans_affine.h>
#include <agg_trans_viewport.h>

// #include "stdafx.h"
#include <basic.h>
#include "draw_util.h"

#include <exception>
#include <memory>
#include <vector>
#include <algorithm>

#include "scanline_accessor.hpp"
#include "vertex_source.hpp"

#include "plcl_exception.hpp"
#include <timer.h>

//typedef agg::pixfmt_alpha_blend_rgb<agg::blender_rgb<agg::rgba8, agg::order_bgr>, agg::scanline_accessor> pixfmt_type;
//typedef pixfmt_type::color_type                        color_type;
//typedef color_type::value_type                         value_type;
//typedef agg::renderer_base<pixfmt_type>                renderer_base;
//typedef agg::renderer_scanline_aa_solid<renderer_base> renderer_solid;

// agg::rasterizer_scanline_aa - polygon rasterizer, not a polygon(S) rasterizer!!!
// so, not pass polygon(S) to rasterizer, pass one polygon, rasterize, pass another one, etc

//template<class Rasterizer, class Scanline, class BaseRenderer,
//             class SpanAllocator, class SpanGenerator>
//    void render_scanlines_aa(Rasterizer& ras, Scanline& sl, BaseRenderer& ren, 
//                             SpanAllocator& alloc, SpanGenerator& span_gen);
//
//template<class Rasterizer, class Scanline, class BaseRenderer,
//             class ColorT>
//    void render_scanlines_aa_solid(Rasterizer& ras, Scanline& sl, 
//                                   BaseRenderer& ren, const ColorT& color);

//typedef agg::pixfmt_alpha_blend_rgb<agg::blender_rgb<agg::rgba8, agg::order_bgr>, agg::scanline_accessor> pixfmt_type;
template<typename pixfmt_type>
//inline void draw_polygon(unsigned int width, unsigned int height, plcl::RenderingData *to, double *vertexes, unsigned int vertexes_n) {
inline void draw_polygon(unsigned int ren_width, unsigned int ren_height, pixfmt_type &ren_data,
												 unsigned int width, unsigned int height, double *vertexes, unsigned int vertexes_n,
												 unsigned int r, unsigned int g, unsigned int b, unsigned int a) {
	// typedef pixfmt_type::color_type                 color_type;
	// typedef color_type::value_type                  value_type;
	typedef agg::renderer_base<pixfmt_type>         renderer_base;

	agg::trans_viewport viewport;

	viewport.preserve_aspect_ratio(0.5, 0.5, agg::aspect_ratio_meet);

	viewport.world_viewport(0.0, 0.0, width, height);
	viewport.device_viewport(0.0, 0.0, ren_width, ren_height);

	agg::trans_affine mtx = viewport.to_affine();

	agg::rasterizer_scanline_aa<> rasterizer;
	agg::scanline_u8              scanline; /* memory with spans */
	rasterizer.clip_box(0.0, 0.0, width, height);
	rasterizer.filling_rule(agg::fill_even_odd); // default: fill_non_zero
	{
		agg::test_vertex_source vertex_source(vertexes, vertexes_n);
		agg::conv_transform<agg::test_vertex_source> vertex_source_converted(vertex_source, mtx);
		rasterizer.add_path(vertex_source_converted);
	}

	//pixfmt_type pixfmt((agg::scanline_accessor(to))); // function/variable declaration
	renderer_base renderer(ren_data);
	// agg::rgba color(1.0, 0.0, 0.0, a);
	agg::rgba color(double(r) / 255.0, double(g) / 255.0, double(b) / 255.0, double(a) / 255.0);

	agg::render_scanlines_aa_solid(rasterizer, scanline, renderer, color);
}

namespace plcl {

void DrawPolygon(RenderingData *data, unsigned int width, unsigned int height, double *vertexes, unsigned int vertexes_n,
								 unsigned int r, unsigned int g, unsigned int b, unsigned int a) {
	switch (data->Pixfmt()) {
		case PLCL_PIXEL_FORMAT_BGRA_32:
			{
				typedef agg::pixfmt_alpha_blend_rgba<agg::blender_rgba<agg::rgba8, agg::order_bgra>, agg::scanline_accessor> pixfmt_type;
				pixfmt_type rendering_data((agg::scanline_accessor(data))); // function/variable declaration
				draw_polygon(data->Width(), data->Height(), rendering_data, width, height, vertexes, vertexes_n,
					r, g, b, a);
			}
			break;
		case PLCL_PIXEL_FORMAT_BGR_24:
			{
				typedef agg::pixfmt_alpha_blend_rgb<agg::blender_rgb<agg::rgba8, agg::order_bgr>, agg::scanline_accessor> pixfmt_type;
				pixfmt_type rendering_data((agg::scanline_accessor(data)));
				draw_polygon(data->Width(), data->Height(), rendering_data, width, height, vertexes, vertexes_n,
					r, g, b, a);
			}
			break;
		case PLCL_PIXEL_FORMAT_GRAY_8:
			{
				typedef agg::pixfmt_alpha_blend_gray<agg::blender_gray<agg::gray8>, agg::scanline_accessor> pixfmt_type;
				pixfmt_type rendering_data((agg::scanline_accessor(data)));
				draw_polygon(data->Width(), data->Height(), rendering_data, width, height, vertexes, vertexes_n,
					r, g, b, a);
			}
			break;
		default:
			plcl_exception::throw_formatted(plcl_exception(), "DrawPolygon(): unsupported pixel format %u", data->Pixfmt());
	}
}

void DrawSelectRect(RenderingData *data, unsigned int width, unsigned int height,
										double left, double top, double right, double bottom,
										unsigned int r, unsigned int g, unsigned int b, unsigned int a,
										double thickness) {
	double rect_border_half = thickness / 2.0;
	double vertexes[] = {
		left   - rect_border_half, top    + rect_border_half, // l_l,t_t
		right  + rect_border_half, top    + rect_border_half, // r_r,t_t
		right  + rect_border_half, bottom - rect_border_half, // r_r,b_b
		left   - rect_border_half, bottom - rect_border_half, // l_l,b_b
		left   - rect_border_half, top    - rect_border_half, // l_l,t_b
		left   + rect_border_half, top    - rect_border_half, // l_r,t_b
		left   + rect_border_half, bottom + rect_border_half, // l_r,b_t
		right  - rect_border_half, bottom + rect_border_half, // r_l,b_t
		right  - rect_border_half, top    - rect_border_half, // r_l,t_b
		left   - rect_border_half, top    - rect_border_half  // l_l,t_b
	};

	DrawPolygon(data, width, height, vertexes, arraysize(vertexes) / 2, r, g, b, a);
}

void DrawSelectRectFixedThickness(plcl::RenderingData *data, unsigned int width, unsigned int height,
										double left_, double top_, double right_, double bottom_,
										unsigned int r, unsigned int g, unsigned int b, unsigned int a,
										double thickness) {
	double rect_border_half = thickness / 2.0;
	if (rect_border_half < 1.0)
		rect_border_half = 1.0;

	agg::trans_viewport viewport;
	viewport.preserve_aspect_ratio(0.5, 0.5, agg::aspect_ratio_meet);
	viewport.world_viewport(0.0, 0.0, width, height);
	viewport.device_viewport(0.0, 0.0, data->Width(), data->Height());
	double left(left_), top(top_), right(right_), bottom(bottom_);
	viewport.transform(&left, &top);
	viewport.transform(&right, &bottom);

	//double vertexes[] = {
	//	left   - rect_border_half, top    + rect_border_half, // l_l,t_t
	//	right  + rect_border_half, top    + rect_border_half, // r_r,t_t
	//	right  + rect_border_half, bottom - rect_border_half, // r_r,b_b
	//	left   - rect_border_half, bottom - rect_border_half, // l_l,b_b
	//	left   - rect_border_half, top    - rect_border_half, // l_l,t_b
	//	left   + rect_border_half, top    - rect_border_half, // l_r,t_b
	//	left   + rect_border_half, bottom + rect_border_half, // l_r,b_t
	//	right  - rect_border_half, bottom + rect_border_half, // r_l,b_t
	//	right  - rect_border_half, top    - rect_border_half, // r_l,t_b
	//	left   - rect_border_half, top    - rect_border_half  // l_l,t_b
	//};
	//DrawPolygon(data, data->Width(), data->Height(), vertexes, arraysize(vertexes) / 2, r, g, b, a);

	{
	double vertexes[] = {
		left   - rect_border_half, top    + rect_border_half, // l_l,t_t
		right  + rect_border_half, top    + rect_border_half, // r_r,t_t
		right  + rect_border_half, top    - rect_border_half, // r_r,t_b
		left   - rect_border_half, top    - rect_border_half  // l_l,t_b
	};
	DrawPolygon(data, data->Width(), data->Height(), vertexes, arraysize(vertexes) / 2, r, g, b, a);
	}

	{
	double vertexes[] = {
		left   - rect_border_half, top    - rect_border_half, // l_l,t_b
		left   + rect_border_half, top    - rect_border_half, // l_r,t_b
		left   + rect_border_half, bottom + rect_border_half, // l_r,b_t
		left   - rect_border_half, bottom + rect_border_half  // l_l,b_t
	};
	DrawPolygon(data, data->Width(), data->Height(), vertexes, arraysize(vertexes) / 2, r, g, b, a);
	}

	{
	double vertexes[] = {
		right  - rect_border_half, top    - rect_border_half, // r_l,t_b
		right  + rect_border_half, top    - rect_border_half, // r_r,t_b
		right  + rect_border_half, bottom + rect_border_half, // r_r,b_t
		right  - rect_border_half, bottom + rect_border_half  // r_l,b_t
	};
	DrawPolygon(data, data->Width(), data->Height(), vertexes, arraysize(vertexes) / 2, r, g, b, a);
	}

	{
	double vertexes[] = {
		left   - rect_border_half, bottom + rect_border_half, // l_l,b_t
		right  + rect_border_half, bottom + rect_border_half, // r_r,b_t
		right  + rect_border_half, bottom - rect_border_half, // r_r,b_b
		left   - rect_border_half, bottom - rect_border_half  // l_l,b_b
	};
	DrawPolygon(data, data->Width(), data->Height(), vertexes, arraysize(vertexes) / 2, r, g, b, a);
	}
}
//void DrawSelectRectDontUpScale(RenderingData *data, unsigned int width, unsigned int height,
//																double left, double top, double right, double bottom,
//																unsigned int r, unsigned int g, unsigned int b, unsigned int a,
//																double thickness) {
//	double rect_border_half = thickness / 2.0;
//	{
//		agg::trans_viewport viewport;
//		viewport.preserve_aspect_ratio(0.5, 0.5, agg::aspect_ratio_meet);
//		viewport.world_viewport(0.0, 0.0, width, height);
//		viewport.device_viewport(0.0, 0.0, data->Width(), data->Height());
//
//		if (viewport.scale() > 1.0) { // i.e. if (data->Width() > width) unscale thickness
//			DrawSelectRectFixedThickness(data, width, height, left, top, right, bottom, r, g, b, a, thickness);
//			return;
//			/*double x(10.0), y(10.0);
//			double x_ = x + rect_border_half;
//
//			viewport.inverse_transform(&x, &y);
//			viewport.inverse_transform(&x_, &y);
//
//			rect_border_half = x_ - x;*/
//		}
//	}
//	double vertexes[] = {
//		left   - rect_border_half, top    + rect_border_half, // l_l,t_t
//		right  + rect_border_half, top    + rect_border_half, // r_r,t_t
//		right  + rect_border_half, bottom - rect_border_half, // r_r,b_b
//		left   - rect_border_half, bottom - rect_border_half, // l_l,b_b
//		left   - rect_border_half, top    - rect_border_half, // l_l,t_b
//		left   + rect_border_half, top    - rect_border_half, // l_r,t_b
//		left   + rect_border_half, bottom + rect_border_half, // l_r,b_t
//		right  - rect_border_half, bottom + rect_border_half, // r_l,b_t
//		right  - rect_border_half, top    - rect_border_half, // r_l,t_b
//		left   - rect_border_half, top    - rect_border_half  // l_l,t_b
//	};
//
//	DrawPolygon(data, width, height, vertexes, arraysize(vertexes) / 2, r, g, b, a);
//}
void DrawSelectRectDontUpScale(RenderingData *data, unsigned int width, unsigned int height,
																double left_, double top_, double right_, double bottom_,
																unsigned int r, unsigned int g, unsigned int b, unsigned int a,
																double thickness) {
	agg::trans_viewport viewport;
	viewport.preserve_aspect_ratio(0.5, 0.5, agg::aspect_ratio_meet);
	viewport.world_viewport(0.0, 0.0, width, height);
	viewport.device_viewport(0.0, 0.0, data->Width(), data->Height());
	if (viewport.scale() < 1.0) // i.e. data->Width() < width, image smaller than original
		thickness *= viewport.scale();

	double rect_border_half = thickness / 2.0;
	if (rect_border_half < 1.0)
		rect_border_half = 1.0;
	
	double left(left_), top(top_), right(right_), bottom(bottom_);
	viewport.transform(&left, &top);
	viewport.transform(&right, &bottom);

	{
	double vertexes[] = {
		left   - rect_border_half, top    + rect_border_half, // l_l,t_t
		right  + rect_border_half, top    + rect_border_half, // r_r,t_t
		right  + rect_border_half, top    - rect_border_half, // r_r,t_b
		left   - rect_border_half, top    - rect_border_half  // l_l,t_b
	};
	DrawPolygon(data, data->Width(), data->Height(), vertexes, arraysize(vertexes) / 2, r, g, b, a);
	}

	{
	double vertexes[] = {
		left   - rect_border_half, top    - rect_border_half, // l_l,t_b
		left   + rect_border_half, top    - rect_border_half, // l_r,t_b
		left   + rect_border_half, bottom + rect_border_half, // l_r,b_t
		left   - rect_border_half, bottom + rect_border_half  // l_l,b_t
	};
	DrawPolygon(data, data->Width(), data->Height(), vertexes, arraysize(vertexes) / 2, r, g, b, a);
	}

	{
	double vertexes[] = {
		right  - rect_border_half, top    - rect_border_half, // r_l,t_b
		right  + rect_border_half, top    - rect_border_half, // r_r,t_b
		right  + rect_border_half, bottom + rect_border_half, // r_r,b_t
		right  - rect_border_half, bottom + rect_border_half  // r_l,b_t
	};
	DrawPolygon(data, data->Width(), data->Height(), vertexes, arraysize(vertexes) / 2, r, g, b, a);
	}

	{
	double vertexes[] = {
		left   - rect_border_half, bottom + rect_border_half, // l_l,b_t
		right  + rect_border_half, bottom + rect_border_half, // r_r,b_t
		right  + rect_border_half, bottom - rect_border_half, // r_r,b_b
		left   - rect_border_half, bottom - rect_border_half  // l_l,b_b
	};
	DrawPolygon(data, data->Width(), data->Height(), vertexes, arraysize(vertexes) / 2, r, g, b, a);
	}
}

} // namespace plcl

/*FaceFeature::Render(IPage *page, IRenderingDevice *output) {
	MemoryRenderingDevice out(GRAY8 | BGR24 | BGRA32);
	page->Render(&out);
	draw_faces<scanline_reader>(page->Width(), page-Height(), scanline_reader(out.Data()), img_width, img_height, faces);
	plcl::Render(RenderingDataReader(out.Data()), output);
}

FaceFeature::Render(width, height, IRenderingDevice *output) {
	draw_faces<rendering_device_accessor>(width, height, rendering_device_accessor(width, height, RenderingData::Stride(output->Pixfmt), output, flip_y),
	 page->Width, page-Height(), faces);
}*/
