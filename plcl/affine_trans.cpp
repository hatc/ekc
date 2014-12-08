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
#include "stdafx.h"

#include <agg_color_rgba.h>
// #include <agg_gamma_lut.h>
#include <agg_pixfmt_gray.h>
#include <agg_pixfmt_rgb.h>
#include <agg_pixfmt_rgba.h>
#include <agg_rendering_buffer.h>
#include <agg_rasterizer_scanline_aa.h>
#include <agg_scanline_u.h>
#include <agg_renderer_scanline.h>
#include <agg_span_allocator.h>
#include <agg_span_interpolator_linear.h>
#include <agg_image_accessors.h>
#include <agg_trans_affine.h>

#include <agg_span_image_filter_rgb.h>
#include <agg_span_image_filter_gray.h>

#include <algorithm>

#include "scanline_accessor.hpp"
#include "rendering_device_accessor.hpp"
#include "vertex_source.hpp"

#include "plcl_exception.hpp"

#include "affine_trans.h"
#include <dumbassert.h>

template<class Pixfmt, class Source, class Interpolator>
struct SpanImageFilterTraits {
	typedef agg::span_image_filter_rgb<Source, Interpolator> SpanImageFilter;
};
template<class Source, class Interpolator>
struct SpanImageFilterTraits<agg::pixfmt_alpha_blend_gray<agg::blender_gray<agg::gray8>, agg::scanline_accessor>, Source, Interpolator> {
	// typedef agg::span_image_filter_gray_nn<Source, Interpolator> SpanImageFilter;
	typedef agg::span_image_filter_gray<Source, Interpolator> SpanImageFilter;
};

template<class InputPixfmt, class OutputPixfmt>
inline void AffineTransImpl(InputPixfmt &in, agg::trans_affine const &mtx,
														unsigned int width, unsigned int height, OutputPixfmt &out) {
	// width,height - actual width and height of output viewport, i.e. after affine transformation

	////////////////// input /////////////////////////////////////////////////

	typedef agg::image_accessor_clip<InputPixfmt> Input;
	typedef agg::span_interpolator_linear<agg::trans_affine> Interpolator;
	/*typedef agg::span_image_filter_rgb_2x2<source_type, interpolator_type> span_generator_type;
	agg::image_filter<agg::image_filter_hanning> image_filter;
	span_generator_type span_generator(source, interpolator, image_filter);
	typedef agg::span_image_filter_rgb_bilinear<source_type, interpolator_type> span_generator_type;
	span_generator_type span_generator(source, interpolator);
	typedef agg::span_image_filter_rgb_nn<source_type, interpolator_type> span_generator_type; // nearest neighbor
	span_generator_type span_generator(source, interpolator);*/
	typedef typename SpanImageFilterTraits<InputPixfmt, Input, Interpolator>::SpanImageFilter SpanGenerator; // typedef agg::span_image_filter_rgb<Input, Interpolator> SpanGenerator;
	//typedef agg::span_image_filter_rgb_nn<source_type, interpolator_type> span_generator_type;
	typedef agg::span_allocator<typename InputPixfmt::color_type> SpanAllocator;

	//agg::rgba back_color(1, 1, 1);
	agg::rgba8 back_color(1, 1, 1); // ? not a ? agg::rgba8 back_color(0xFF, 0xFF, 0xFF);
	Input source(in, back_color);
	Interpolator interpolator(mtx);
	agg::image_filter<agg::image_filter_mitchell> image_filter;
	/*agg::image_filter_sinc image_filter(2.0);
	agg::image_filter_lut image_filter_lut(image_filter);*/
	SpanGenerator span_generator(source, interpolator, image_filter);
	//span_generator_type span_generator(source, interpolator);
	SpanAllocator span_allocator;

	////////////////// output /////////////////////////////////////////////////
	
	typedef agg::renderer_base<OutputPixfmt> RendererBase;
	agg::rasterizer_scanline_aa<> rasterizer;
	agg::scanline_u8              scanline; /* memory with spans */

	//rasterizer.clip_box(0.0, 0.0, width, height);
	{
		double vertexes[] = {
			0.0, 0.0,
			0.0, height,
			width, height,
			width, 0.0
		};

		agg::test_vertex_source vertex_source(vertexes, arraysize(vertexes) / 2);
		rasterizer.add_path(vertex_source);
	}

	//pixfmt_type pixfmt((agg::scanline_accessor(to))); // function/variable declaration
	RendererBase renderer(out);
	agg::render_scanlines_aa(rasterizer, scanline, renderer, span_allocator, span_generator);
}

namespace plcl {

void AffineTrans(RenderingData *data, double (&matrix)[6], unsigned int output_width, unsigned int output_height, RenderingDevice *output) {
	agg::trans_affine mtx(matrix);

	unsigned int pixfmt = data->Pixfmt();
	if (pixfmt != output->Pixfmt()) {
		output->Pixfmt(pixfmt);
		if (pixfmt != output->Pixfmt()) {
			plcl_exception::throw_formatted(plcl_exception(),
				"AffineTrans(): output device must support input pixel format %u",
				pixfmt);
		}
	}
	if (!output->SetViewport(0, 0, output_width, output_height))
		throw plcl_exception("AffineTrans(): SetViewport() fails");

	switch (data->Pixfmt()) {
		case PLCL_PIXEL_FORMAT_BGRA_32:
			{
				typedef agg::pixfmt_alpha_blend_rgba<agg::blender_rgba<agg::rgba8, agg::order_bgra>, agg::scanline_accessor> source_pixfmt_type;
				source_pixfmt_type rendering_data((agg::scanline_accessor(data))); // function/variable declaration
				typedef agg::pixfmt_alpha_blend_rgba<agg::blender_rgba<agg::rgba8, agg::order_bgra>, agg::rendering_device_accessor> output_pixfmt_type;
				output_pixfmt_type rendering_device(
					agg::rendering_device_accessor(output_width, output_height, plcl::RenderingData::Stride(output_width, output->Pixfmt()), output, false)
				);

				AffineTransImpl(rendering_data, mtx, output_width, output_height, rendering_device);
			}
			break;
		case PLCL_PIXEL_FORMAT_BGR_24:
			{
				typedef agg::pixfmt_alpha_blend_rgb<agg::blender_rgb<agg::rgba8, agg::order_bgr>, agg::scanline_accessor> source_pixfmt_type;
				source_pixfmt_type rendering_data((agg::scanline_accessor(data)));
				typedef agg::pixfmt_alpha_blend_rgb<agg::blender_rgb<agg::rgba8, agg::order_bgr>, agg::rendering_device_accessor> output_pixfmt_type;
				output_pixfmt_type rendering_device(
					agg::rendering_device_accessor(output_width, output_height, plcl::RenderingData::Stride(output_width, output->Pixfmt()), output, false)
				);

				AffineTransImpl(rendering_data, mtx, output_width, output_height, rendering_device);
			}
			break;
		case PLCL_PIXEL_FORMAT_GRAY_8:
			{
				typedef agg::pixfmt_alpha_blend_gray<agg::blender_gray<agg::gray8>, agg::scanline_accessor> source_pixfmt_type;
				source_pixfmt_type rendering_data((agg::scanline_accessor(data)));
				typedef agg::pixfmt_alpha_blend_gray<agg::blender_gray<agg::gray8>, agg::rendering_device_accessor> output_pixfmt_type;
				output_pixfmt_type rendering_device(
					agg::rendering_device_accessor(output_width, output_height, plcl::RenderingData::Stride(output_width, output->Pixfmt()), output, false)
				);

				AffineTransImpl(rendering_data, mtx, output_width, output_height, rendering_device);
			}
			break;
		default:
			plcl_exception::throw_formatted(plcl_exception(),
				"AffineTrans(): pixel format(%u) not supported",
				pixfmt);
	}
	// output->Render(); // plcl::Render use AffineTrans() and plcl::Render call output->Render() method
}

void Rotate(RenderingData *data, unsigned int angle, RenderingDevice *output) {
	// angle = 360 - angle;

	agg::trans_affine mtx;

	double angle_radians(angle * agg::pi / 180.0);
	unsigned int width(data->Width()), height(data->Height()); // for 90/270 - data access not sequentail, so can't use ScanlineReader
	{
		agg::point_d points[] = {
			agg::point_d(0.0,   height), // l t
			agg::point_d(width, height), // r t
			agg::point_d(width, 0.0),    // r b
			agg::point_d(0.0,   0.0)     // l b
		};

		agg::trans_affine m;
		// m.rotate((360.0 - angle) * agg::pi / 180.0);
		m.rotate(angle_radians);
		m.invert();

		m.transform(&points[0].x, &points[0].y);
		double min_x(points[0].x), max_x(points[0].x), min_y(points[0].y), max_y(points[0].y);
		for (size_t i = 1; i < arraysize(points); ++i) {
			m.transform(&points[i].x, &points[i].y);

			min_x = (std::min)(min_x, points[i].x);
			min_y = (std::min)(min_y, points[i].y);

			max_x = (std::max)(max_x, points[i].x);
			max_y = (std::max)(max_y, points[i].y);
		}

		width = (unsigned int)floor((max_x - min_x) + 0.5);
		height = (unsigned int)floor((max_y - min_y) + 0.5);

		mtx.translate(min_x, min_y);
	}
	mtx.rotate(angle_radians);

	double matrix[6];
	mtx.store_to(matrix);
	AffineTrans(data, matrix, width, height, output);
}

template<size_t PixelSize>
void MirrorScanline(unsigned char *scanline, size_t width) {
	DUMBASS_CHECK(width > 0);

	// unsigned char buf[PixelSize];
	for (unsigned char *head = scanline, *tail = scanline + PixelSize * (width - 1), *half = scanline + PixelSize * (width / 2); head != half; head += PixelSize, tail -= PixelSize) {
		for (size_t i = 0; i < PixelSize; ++i) {
			unsigned char tmp = head[i];
			head[i] = tail[i];
			tail[i] = tmp;
		}
	}
}
template<>
void MirrorScanline<4>(unsigned char *scanline_, size_t width) {
	DUMBASS_CHECK(width > 0);

	int *scanline = (int*)scanline_;
	for (int *head = scanline, *tail = scanline + (width - 1), *half = scanline + width / 2; head != half; ++head, --tail) {
		int tmp = *head;
		*head = *tail;
		*tail = tmp;
	}
}
template<size_t PixelSize>
inline void MirrorXImpl(plcl::RenderingData *data) {
	for (unsigned int y = 0, h = data->Height(), w = data->Width(); y < h; ++y)
		MirrorScanline<PixelSize>(data->Scanline(y), w);
}
void MirrorX(plcl::RenderingData *data) {
	switch (data->Pixfmt()) {
		case PLCL_PIXEL_FORMAT_GRAY_8:
			MirrorXImpl<1>(data);
			break;
		case PLCL_PIXEL_FORMAT_RGB_24:
		case PLCL_PIXEL_FORMAT_BGR_24:
			MirrorXImpl<3>(data);
			break;
		default:
			MirrorXImpl<4>(data);
	}
}

} // namespace plcl
