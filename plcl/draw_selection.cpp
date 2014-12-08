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
#include <math.h>

#include <agg_color_rgba.h>
#include <agg_gamma_lut.h>
#include <agg_pixfmt_rgb.h>
#include <agg_rendering_buffer.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_scanline_u.h>
#include <agg_renderer_scanline.h>
#include <agg_trans_affine.h>
#include <agg_image_accessors.h>
#include <agg_conv_transform.h>
#include <agg_trans_viewport.h>

#include "scanline_accessor.hpp"
#include "draw_selection.h"

//typedef agg::row_accessor<agg::int8u> rendering_buffer;
/*namespace agg {
class scanline_accessor {
public:
	typedef const_row_info<int8u> row_data;

	scanline_accessor() : m_width(0), m_height(0), m_stride(0), scanline_source(0), get_scanline(0)
	{}

	scanline_accessor(void* scanline_source_, unsigned char* (*get_scanline_)(void*, unsigned),
		unsigned width, unsigned height, int stride)
		: scanline_source(scanline_source_), get_scanline(get_scanline_),
		m_width(width), m_height(height), m_stride(stride)
	{}

	//--------------------------------------------------------------------
	AGG_INLINE unsigned width()  const { return m_width;  }
	AGG_INLINE unsigned height() const { return m_height; }
	AGG_INLINE int      stride() const { return m_stride; }
	AGG_INLINE unsigned stride_abs() const 
	{
		return (m_stride < 0) ? unsigned(-m_stride) : unsigned(m_stride);
	}

	//--------------------------------------------------------------------
	
	AGG_INLINE       int8u* row_ptr(int y)       { return get_scanline(scanline_source, y); }
	AGG_INLINE const int8u* row_ptr(int y) const { return get_scanline(scanline_source, y); }
	AGG_INLINE       int8u* row_ptr(int, int y, unsigned) { return row_ptr(y); }
	AGG_INLINE row_data row(int y) const 
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
	void *scanline_source;
	unsigned char* (*get_scanline)(void*, unsigned);
	unsigned      m_width;  // Width in pixels
	unsigned      m_height; // Height in pixels
	int           m_stride; // Number of bytes per row. Can be < 0
};
}*/

//typedef agg::pixfmt_alpha_blend_rgb<agg::blender_rgb<agg::rgba8,  agg::order_bgr>, agg::scanline_accessor> pixfmt_type;
////typedef agg::pixfmt_bgr24                              pixfmt_type;
//typedef pixfmt_type::color_type                        color_type;
//typedef color_type::value_type                         value_type;
//typedef agg::renderer_base<pixfmt_type>                renderer_base;
//typedef agg::renderer_scanline_aa_solid<renderer_base> renderer_solid;

namespace agg {
class test_vertex_source {
	double const *m_polygon;
	unsigned m_num_points;
	unsigned m_vertex;
	bool     m_roundoff;
	bool     m_close;
public:
	test_vertex_source(const double* polygon, unsigned np, 
		bool roundoff = false, bool close = true) : m_polygon(polygon),
		m_num_points(np),
		m_vertex(0),
		m_roundoff(roundoff),
		m_close(close)
	{}
	
	void close(bool f) { m_close = f;    }
	bool close() const { return m_close; }
	
	void rewind(unsigned)
	{
		m_vertex = 0;
	}

	unsigned vertex(double* x, double* y)
	{
		if(m_vertex > m_num_points) return path_cmd_stop;
		if(m_vertex == m_num_points) 
		{
			++m_vertex;
			return path_cmd_end_poly | (m_close ? path_flags_close : 0);
		}
		*x = m_polygon[m_vertex * 2];
		*y = m_polygon[m_vertex * 2 + 1];
		if(m_roundoff)
		{
			*x = floor(*x) + 0.5;
			*y = floor(*y) + 0.5;
		}
		++m_vertex;
		return (m_vertex == 1) ? path_cmd_move_to : path_cmd_line_to;
	}
};
}

namespace plcl {

SelectionScanlineSweeper::SelectionScanlineSweeper(unsigned int width_dp_, unsigned height_dp_, bool flip_y_)
	: width_dp(width_dp_), height_dp(height_dp_), flip_y(flip_y_), color(agg::rgba(0.9, 0.9, 0.1, 0.5))
{}

void SelectionScanlineSweeper::SelectionColor(double r, double g, double b, double a) {
	color = agg::rgba(r, g, b, a);
}
void SelectionScanlineSweeper::SelectionColor(unsigned int r, unsigned int g, unsigned int b, unsigned int a) {
	color = agg::rgba(double(r) / 255.0, double(g) / 255.0, double(b) / 255.0, double(a) / 255.0);
}

void SelectionScanlineSweeper::DrawImpl_bgr(agg::rect_d rect, RenderingData *data, unsigned int width, unsigned int height, int stride,
																				agg::trans_affine const &mtx, agg::trans_affine const &viewport_mtx) {
	//ScanlineSource source(data);

	typedef agg::pixfmt_alpha_blend_rgb<agg::blender_rgb<agg::rgba8,  agg::order_bgr>, agg::scanline_accessor> pixfmt_type;
	typedef pixfmt_type::color_type                        color_type;
	typedef color_type::value_type                         value_type;
	typedef agg::renderer_base<pixfmt_type>                renderer_base;
	typedef agg::renderer_scanline_aa_solid<renderer_base> renderer_solid;

	agg::rasterizer_scanline_aa<> l_rasterizer;
	agg::scanline_u8              l_scanline;

	//pixfmt_type       pixfmt(agg::scanline_accessor(&source, ScanlineSource::get_scanline, width, height, stride));
	pixfmt_type       pixfmt((agg::scanline_accessor(data)));
	renderer_base     rb(pixfmt);

	l_rasterizer.clip_box(0.0, 0.0, width, height);
	l_rasterizer.filling_rule(agg::fill_non_zero);
	
	if (flip_y) {
		mtx.transform(&rect.x1, &rect.y1);
		mtx.transform(&rect.x2, &rect.y2);
	}
	
	double vertexes[] = {
		rect.x1, rect.y2,
		rect.x1, rect.y1,
		rect.x2, rect.y1,
		rect.x2, rect.y2
	};
	
	agg::test_vertex_source vertex_source(vertexes, 4);
	agg::conv_transform<agg::test_vertex_source> vertex_source_converted(vertex_source, viewport_mtx);
	l_rasterizer.add_path(vertex_source_converted);
	
	renderer_solid r(rb);
	{
		r.color(color);
		agg::render_scanlines(l_rasterizer, l_scanline, r);
	}
}
void SelectionScanlineSweeper::DrawImpl_rgb(agg::rect_d rect, RenderingData *data, unsigned int width, unsigned int height, int stride,
																				agg::trans_affine const &mtx, agg::trans_affine const &viewport_mtx) {
	//ScanlineSource source(data);

	typedef agg::pixfmt_alpha_blend_rgb<agg::blender_rgb<agg::rgba8,  agg::order_rgb>, agg::scanline_accessor> pixfmt_type;
	typedef pixfmt_type::color_type                        color_type;
	typedef color_type::value_type                         value_type;
	typedef agg::renderer_base<pixfmt_type>                renderer_base;
	typedef agg::renderer_scanline_aa_solid<renderer_base> renderer_solid;

	agg::rasterizer_scanline_aa<> l_rasterizer;
	agg::scanline_u8              l_scanline;

	//pixfmt_type       pixfmt(agg::scanline_accessor(&source, ScanlineSource::get_scanline, width, height, stride));
	pixfmt_type       pixfmt((agg::scanline_accessor(data)));
	renderer_base     rb(pixfmt);

	l_rasterizer.clip_box(0.0, 0.0, width, height);
	l_rasterizer.filling_rule(agg::fill_non_zero);
	
	if (flip_y) {
		mtx.transform(&rect.x1, &rect.y1);
		mtx.transform(&rect.x2, &rect.y2);
	}
	
	double vertexes[] = {
		rect.x1, rect.y2,
		rect.x1, rect.y1,
		rect.x2, rect.y1,
		rect.x2, rect.y2
	};
	
	agg::test_vertex_source vertex_source(vertexes, 4);
	agg::conv_transform<agg::test_vertex_source> vertex_source_converted(vertex_source, viewport_mtx);
	l_rasterizer.add_path(vertex_source_converted);
	
	renderer_solid r(rb);
	{
		r.color(color);
		agg::render_scanlines(l_rasterizer, l_scanline, r);
	}
}

void SelectionScanlineSweeper::Draw(RenderingData *data, bool bgr_order) {
	unsigned int const width = data->Width(); unsigned int const height = data->Height();
	int const stride = data->Stride();
	
	agg::trans_viewport viewport;
	viewport.preserve_aspect_ratio(0.5, 0.5, agg::aspect_ratio_meet);
	viewport.world_viewport(0.0, 0.0, width_dp, height_dp);
	viewport.device_viewport(0.0, 0.0, width, height);

	agg::trans_affine mtx;
	if (flip_y) {
		mtx.translate(-double(width_dp) / 2, -double(height_dp) / 2);
		mtx.flip_y();
		mtx.translate(double(width_dp) / 2, double(height_dp) / 2);
	}

	agg::trans_affine viewport_mtx = viewport.to_affine();
	for (size_t i = 0; i < rects.size(); ++i) {
		RectContainer::Rect const &rect = rects[i];

		if (bgr_order) {
			DrawImpl_bgr(agg::rect_d(double(rect.x1), double(rect.y1), double(rect.x2), double(rect.y2)),
				data, width, height, stride, mtx, viewport_mtx);
		} else {
			DrawImpl_rgb(agg::rect_d(double(rect.x1), double(rect.y1), double(rect.x2), double(rect.y2)),
				data, width, height, stride, mtx, viewport_mtx);
		}
	}
}

} // namespace plcl
