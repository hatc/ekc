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
#include <agg_color_rgba.h>
#include <agg_gamma_lut.h>
#include <agg_pixfmt_gray.h>
#include <agg_pixfmt_rgb.h>
#include <agg_pixfmt_rgba.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_scanline_u.h>
#include <agg_renderer_scanline.h>
#include <agg_trans_affine.h>
#include <agg_span_allocator.h>
#include <agg_span_interpolator_linear.h>
#include <agg_image_accessors.h>
#include <agg_span_image_filter_gray.h>
#include <agg_span_image_filter_rgb.h>
#include <agg_trans_viewport.h>

#include "scanline_accessor.hpp"
#include "rendering_device_accessor.hpp"
#include "vertex_source.hpp"
#include "plcl_exception.hpp"

#include "agg_resample.h"

template<typename SourcePixfmt, typename SpanInterpolator>
struct span_image_filter_type {
	typedef agg::image_accessor_clip<SourcePixfmt> source_type;
	typedef agg::span_image_filter_rgb_2x2<source_type, SpanInterpolator> type;
};
template<typename SpanInterpolator>
struct span_image_filter_type<agg::pixfmt_alpha_blend_gray<agg::blender_gray<agg::gray8>, agg::scanline_accessor>, SpanInterpolator> {
	typedef agg::image_accessor_clip<agg::pixfmt_alpha_blend_gray<agg::blender_gray<agg::gray8>, agg::scanline_accessor> > source_type;
	typedef agg::span_image_filter_gray_2x2<source_type, SpanInterpolator> type;
};
template<typename source_pixfmt_type, typename output_pixfmt_type> /* source_pixfmt_type -> SourcePixfmt, e.t.c. */
inline void resize(plcl::RenderingData *data, unsigned int width, unsigned int height, plcl::RenderingDevice *output, bool flip_y) {
	typedef source_pixfmt_type::color_type                 source_color_type;
	typedef source_color_type::value_type                  source_value_type;

	typedef output_pixfmt_type::color_type                 output_color_type;
	typedef output_color_type::value_type                  output_value_type;
	typedef agg::renderer_base<output_pixfmt_type>         renderer_base;

	typedef agg::image_accessor_clip<source_pixfmt_type> source_type; // from source type

	agg::trans_viewport viewport;
	viewport.preserve_aspect_ratio(0.5, 0.5, agg::aspect_ratio_meet);

	viewport.world_viewport(0.0, 0.0, width, height);
	viewport.device_viewport(0.0, 0.0, data->Width(), data->Height());
	//viewport.world_viewport(0.0, 0.0, data->Width(), data->Height()); // only on Page.ctor() - set raster image size
	//viewport.device_viewport(0.0, 0.0, width, height);
	agg::trans_affine mtx = viewport.to_affine();
	/* if swap world_viewport, device_viewport i.e.
	world_viewport(0.0, 0.0, to->Width(), to->Height());
	viewport.device_viewport(0.0, 0.0, from->Width(), from->Height());
	then matrix invertion not needed*/
	//mtx.invert();
	
	typedef agg::span_interpolator_linear<agg::trans_affine> interpolator_type;
	interpolator_type interpolator(mtx);
	
	/*pixfmt_type   to_pixfmt((agg::scanline_accessor(to)));
	renderer_base rb(to_pixfmt);
	agg::rgba back_color(0, 1, 0, 0.9);*/
	output_pixfmt_type output_pixfmt((agg::rendering_device_accessor(width, height, width * 4, output, flip_y)));
	renderer_base rb(output_pixfmt);
	agg::rgba back_color(1, 1, 1, 0.01);

	source_pixfmt_type source_pixfmt((agg::scanline_accessor(data)));
	source_type source(source_pixfmt, back_color/*background*/);

	agg::span_allocator<source_color_type> span_allocator;

	/*typedef agg::span_image_filter_rgb_bilinear_clip<pixfmt_type, interpolator_type> span_generator_type;
	span_generator_type span_generator(from_pixfmt, back_color, interpolator);*/
	//agg::image_filter_bilinear filter_kernel;
	agg::image_filter_hermite filter_kernel;
	//agg::image_filter_kaiser filter_kernel;
	agg::image_filter_lut filter(filter_kernel, true);
	
	//class span_image_resample_affine : public span_image_filter<Source, span_interpolator_linear<trans_affine> >
	//typedef agg::span_image_resample_rgb_affine<source_type> span_generator_type;
	// typedef agg::span_image_filter_rgb_2x2<source_type, interpolator_type> span_generator_type;
	typedef span_image_filter_type<source_pixfmt_type, interpolator_type>::type span_generator_type;
	span_generator_type span_generator(source, interpolator, filter);
	
	agg::rasterizer_scanline_aa<> rasterizer;
	agg::scanline_u8              scanline; /* memory with spans */

	rasterizer.clip_box(0.0, 0.0, width, height);
	{
		double vertexes[] = {
			0.0, 0.0,
			0.0, height,
			width, height,
			width, 0.0
		};
		agg::test_vertex_source vertex_source(vertexes, 4);
		rasterizer.add_path(vertex_source);
	}
	
	agg::render_scanlines_aa(rasterizer, scanline, rb, span_allocator, span_generator);
}

namespace plcl {

void Resample(RenderingData *input, unsigned int width, unsigned int height, RenderingDevice *output, bool flip_y) {
	unsigned int pixfmt = input->Pixfmt();
	if (pixfmt != output->Pixfmt()) {
		output->Pixfmt(pixfmt);
		if (pixfmt != output->Pixfmt()) {
			plcl_exception::throw_formatted(plcl_exception(),
				"Resample(): output device must support input pixel format %u",
				pixfmt);
		}
	}
	if (!output->SetViewport(0, 0, width, height))
		throw plcl_exception("Resample(): SetViewport() fails");

	if (PLCL_PIXEL_FORMAT_BGRA_32 == pixfmt) {
		typedef agg::pixfmt_alpha_blend_rgba<agg::blender_rgba<agg::rgba8, agg::order_bgra>, agg::scanline_accessor> source_pixfmt_type;
		typedef agg::pixfmt_alpha_blend_rgba<agg::blender_rgba<agg::rgba8, agg::order_bgra>, agg::rendering_device_accessor> output_pixfmt_type;
		
		resize<source_pixfmt_type, output_pixfmt_type>(input, width, height, output, flip_y);
	} else if (PLCL_PIXEL_FORMAT_BGR_24 == pixfmt) {
		typedef agg::pixfmt_alpha_blend_rgb<agg::blender_rgb<agg::rgba8, agg::order_bgr>, agg::scanline_accessor> source_pixfmt_type;
		typedef agg::pixfmt_alpha_blend_rgb<agg::blender_rgb<agg::rgba8, agg::order_bgr>, agg::rendering_device_accessor> output_pixfmt_type;
		
		resize<source_pixfmt_type, output_pixfmt_type>(input, width, height, output, flip_y);
	} else if (PLCL_PIXEL_FORMAT_GRAY_8 == pixfmt) {
		typedef agg::pixfmt_alpha_blend_gray<agg::blender_gray<agg::gray8>, agg::scanline_accessor> source_pixfmt_type;
		typedef agg::pixfmt_alpha_blend_gray<agg::blender_gray<agg::gray8>, agg::rendering_device_accessor> output_pixfmt_type;
		
		resize<source_pixfmt_type, output_pixfmt_type>(input, width, height, output, flip_y);
	} else {
		plcl_exception::throw_formatted(plcl_exception(),
			"Resample(): pixel format(%u) not supported",
			pixfmt);
	}
}

} // namespace plcl
