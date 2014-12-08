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

#include <agg_trans_affine.h>

#include <algorithm> // std::min, std::max

//#include <scoped_buf.hpp>
//#include "color_conv.h"
#include "page.h"

namespace plcl {

/*class TestZoomPage : public Page {
public:
	TestZoomPage(unsigned int width_dp_, unsigned int height_dp_, unsigned int dpi_)
		: Page(width_dp_, height_dp_, dpi_)
	{}
	virtual bool RenderData(RenderingData**) { return false; }
};*/
/*1280_w - 720_h
	900_w  - 506_h

	1280_w - 720_h
	899_w  - 506_h
	TestZoomPage page(1280, 720, 72);
	std::cout << "zoom 100 : " << page.Width() << " : " << page.Height() << " : " << page.Zoom() << std::endl;
	page.Width(900);
	std::cout << "width 900 : " << page.Width() << " : " << page.Height() << " : " << page.Zoom() << std::endl;
	page.Height(506);
	std::cout << "height 506 : " << page.Width() << " : " << page.Height() << " : " << page.Zoom() << std::endl;
	page.Zoom(100);
	std::cout << "zoom 100 : " << page.Width() << " : " << page.Height() << " : " << page.Zoom() << std::endl;
	page.Zoom(200);
	std::cout << "zoom 200 : " << page.Width() << " : " << page.Height() << " : " << page.Zoom() << std::endl;*/

inline void SizeForRotate(unsigned int width, unsigned int height, unsigned int angle, unsigned int *output_width, unsigned int *output_height) {
	agg::trans_affine m;
	// m.rotate((360.0 - angle) * agg::pi / 180.0);
	angle = 360 - angle;
	m.rotate(angle * agg::pi / 180.0);
	m.invert();

	agg::point_d points[] = {
		agg::point_d(0.0,   height), // l t
		agg::point_d(width, height), // r t
		agg::point_d(width, 0.0),    // r b
		agg::point_d(0.0,   0.0)     // l b
	};
	
	m.transform(&points[0].x, &points[0].y);
	double min_x(points[0].x), max_x(points[0].x), min_y(points[0].y), max_y(points[0].y);
	for (size_t i = 1; i < arraysize(points); ++i) {
		m.transform(&points[i].x, &points[i].y);
		
		min_x = (std::min)(min_x, points[i].x);
		min_y = (std::min)(min_y, points[i].y);
		
		max_x = (std::max)(max_x, points[i].x);
		max_y = (std::max)(max_y, points[i].y);
	}
	
	if (output_width)
		*output_width = (unsigned int)floor((max_x - min_x) + 0.5);
	if (output_height)
		*output_height = (unsigned int)floor((max_y - min_y) + 0.5);
}

Page::Page(unsigned int width_dp_, unsigned int height_dp_, unsigned int dpi_)
	: width_dp(width_dp_), height_dp(height_dp_), dpi(dpi_), zoom(100), exif_orientation(1) {
	Dpi(96.0); // calc width, height
}
Page::~Page()
{}

/*1280_w - 720_h
900_w - x_h

x_h = 720_h * 900_w / 1280_w = 506_h

720_h - 1280_h
x_h - 900_h

x_h = 720_h * 900_w / 1280_w

1280_w - 720_h
x_w - 506_h

x_w = 1280_w * 506_h / 720_h = 899

width = width_dp * (zoom / 100.0) * (device_dpi / dpi)
(width / width_dp) * (dpi / device_dpi) * 100 = zoom

(height / height_dp) * (dpi / device_dpi) = zoom*/

unsigned int Page::Width() const {
	unsigned int r(width);

	unsigned int angle;
	Page::ExifOrientation(exif_orientation, (bool*)0, (bool*)0, &angle);
	if (angle != 0)
		SizeForRotate(width, height, angle, &r, (unsigned int*)0);

	return r; // return width;
}
void Page::Width(unsigned int v) {
	unsigned int angle;
	Page::ExifOrientation(exif_orientation, (bool*)0, (bool*)0, &angle);
	if (90 == angle || 270 == angle) {
		if (height != v) {
			/* fit height */
			height = v;
			width = (unsigned int)(double(width_dp * height) / double(height_dp));
			zoom = (unsigned int)(100.0 * (double(height) / double(height_dp)) * (double(dpi) / device_dpi));
		}
		return;
	}

	if (width != v) {
	/* fit width */
	width = v;
	height = (unsigned int)(double(height_dp * width) / double(width_dp));
	zoom = (unsigned int)(100.0 * (double(width) / double(width_dp)) * (double(dpi) / device_dpi));
	}
}

unsigned int Page::Height() const {
	unsigned int r(height);

	unsigned int angle;
	Page::ExifOrientation(exif_orientation, (bool*)0, (bool*)0, &angle);
	if (angle != 0)
		SizeForRotate(width, height, angle, (unsigned int*)0, &r);

	return r; // return height;
}
void Page::Height(unsigned int v) {
	unsigned int angle;
	Page::ExifOrientation(exif_orientation, (bool*)0, (bool*)0, &angle);
	if (90 == angle || 270 == angle) {
		if (width != v) {
			/* fit width */
			width = v;
			height = (unsigned int)(double(height_dp * width) / double(width_dp));
			zoom = (unsigned int)(100.0 * (double(width) / double(width_dp)) * (double(dpi) / device_dpi));
		}
		return;
	}

	if (height != v) {
	/* fit height */
	height = v;
	width = (unsigned int)(double(width_dp * height) / double(height_dp));
	zoom = (unsigned int)(100.0 * (double(height) / double(height_dp)) * (double(dpi) / device_dpi));
	}
}

unsigned int Page::Zoom() const {
	return zoom;
}
void Page::Zoom(unsigned int v) {
	if (zoom != v) {
	double z = (double(v) / 100.0) * device_dpi / double(dpi);
	width = (unsigned int)(double(width_dp) * z);
	height = (unsigned int)(double(height_dp) * z);
	zoom = v;
	}
}

double Page::Dpi() const {
	return device_dpi;
}
void Page::Dpi(double v) {
	if (v - 0.5 < 0)
		v = 96.0;
	/*get_page(doc, page_num, width_dp, height_dp, dpi);
	double z = 96.0 / dpi; // 100% zoom for djvu documents;
	int width = (int)(width_dp * z); int height = (int)(height_dp * z);*/
	device_dpi = v;
	Zoom((zoom += 1) - 1);
}

// IDevice { SupportedPxfmt; Render(scanline); Init; }
// IRenderingData { Scanline(row); Pxfmt { get;set; }; } 
//  for example, ScanlineSource.Bitmap(i.e. ScanlineSource.Pxfmt = gray8)
//  Render(IDevice.SupportedPxfmt == rgb24) { ScanlineSource.Pxfmt = rgb24; ScanlineSource.Scanline() { alloc rgb24, convert row } }
//  Render(IDevice.SupportedPxfmt == gray8) { ScanlineSource.Scanline() { return pixels[row]; } }
// IRenderingData == ScanlineSource - each Scanline call not modify Pixmap || Bitmap - create conversion tmp scanline, if needed
// IPage::Render(IRenderingData, IDevice) IPage::RenderSelection(IRenderingData, IDevice) IPage::Text(**wchar_t) IPage::Select(head, tail)

#if 0
void Page::Render(RenderingData *data, RenderingDevice *output) {
	unsigned int const data_width = data->Width(); unsigned int const data_height = data->Height();
	if ((width != data_width) || (height != data_height))
		throw plcl_exception("Page::Render() invalid rendering data");
	/*if ((width > 3000) || (height > 3000))
		throw plcl_exception("Page::Render() oversized");*/
	if ((data_width > 3000) || (data_height > 3000)) {
			plcl_exception::throw_formatted(plcl_exception(),
				"Page::Render() oversized image(width = %u, height = %u)",
				width, height);
	}

	unsigned int data_pixfmt = data->Pixfmt();
	output->Pixfmt(data_pixfmt);
	if (output->Pixfmt() != data_pixfmt) {
		unsigned int supported_pixfmt = output->SupportedPixfmt();
		if ((PLCL_PIXEL_FORMAT_GRAY_8 == data_pixfmt)
			&& ((supported_pixfmt & PLCL_PIXEL_FORMAT_BGR_24) != 0))
			output->Pixfmt(PLCL_PIXEL_FORMAT_BGR_24);
		else if ((PLCL_PIXEL_FORMAT_GRAY_8 == data_pixfmt)
			&& ((supported_pixfmt & PLCL_PIXEL_FORMAT_RGB_24) != 0))
			output->Pixfmt(PLCL_PIXEL_FORMAT_RGB_24);
		else
			throw plcl_exception("Page::Render() pixel format conversion unavailable");
	}
	
	ColorConv color_conv = GetConverter(data_pixfmt, output->Pixfmt());
	cpcl::ScopedBuf<unsigned char, 3 * 1280> scanline_buf;
	unsigned char *scanline = scanline_buf.Alloc(color_conv.storage_needed(width));

	if (!output->SetViewport(0, 0, width, height))
			plcl_exception::throw_formatted(plcl_exception(), "Page::Render(): SetViewport(0, 0, %d, %d) fails", width, height);
	for (unsigned int y = 0; y < height; ++y)
		output->SweepScanline(color_conv.convert(scanline, data->Scanline(y), width));
	output->Render();
}
#endif

unsigned int Page::ExifOrientation() const {
	return exif_orientation;
}
void Page::ExifOrientation(unsigned int v) {
	exif_orientation = v;
}

struct OrientationInfo {
	bool mirror_x, mirror_y;
	unsigned int angle;
// orientation_info[] = {
//	{ false, false, 0   }, // 1 = Horizontal (normal)
//	{ true,  false, 0   }, // 2 = Mirror horizontal
//	{ false, false, 180 }, // 3 = Rotate 180
//	{ false, true,  0   }, // 4 = Mirror vertical
//	{ true,  false, 270 }, // 5 = Mirror horizontal and rotate 270 CW
//	{ false, false, 270 }, // 6 = Rotate 270 CW
//	{ true,  false, 90  }, // 7 = Mirror horizontal and rotate 90 CW
//	{ false, false, 90  }  // 8 = Rotate 90 CW
//};
} static const ORIENTATION_INFO_FIX[] = {
	{ false, false, 0   }, // 1 = Horizontal (normal)
	{ true,  false, 0   }, // 2 = Mirror horizontal
	{ false, false, 180 }, // 3 = Rotate 180
	{ false, true,  0   }, // 4 = Mirror vertical
	{ false, true,  90  }, // { true,  false, 270 }, // 5 = Mirror horizontal and rotate 270 CW
	{ false, false, 90  }, // 6 = Rotate 270 CW
	{ false, true,  270 }, // 7 = Mirror horizontal and rotate 90 CW
	{ false, false, 270 }  // 8 = Rotate 90 CW
};
void Page::ExifOrientation(unsigned int exif_orientation, bool *mirror_x, bool *mirror_y, unsigned int *angle) {
	if (exif_orientation > 0)
		exif_orientation -= 1;
	if (exif_orientation >= arraysize(ORIENTATION_INFO_FIX))
		exif_orientation = 0;

	if (mirror_x)
		*mirror_x = ORIENTATION_INFO_FIX[exif_orientation].mirror_x;
	if (mirror_y)
		*mirror_y = ORIENTATION_INFO_FIX[exif_orientation].mirror_y;
	if (angle)
		*angle = ORIENTATION_INFO_FIX[exif_orientation].angle;
}

} // namespace plcl
