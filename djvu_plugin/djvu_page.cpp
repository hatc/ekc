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
#include "stdafx.h"

#include <memory>

#include <string_util.h>
#include <scoped_buf.hpp>

#include "djvu_rendering_data.h"
#include "djvu_page.h"
#include "gexception_helper.hpp"

#include <render.hpp>
#include <plcl_exception.hpp>

using DJVU::DjVuTXT;

DjvuPage::DjvuPage(unsigned int width_dp_, unsigned int height_dp_, unsigned int dpi_)
	: TextPage(width_dp_, height_dp_, dpi_) {
	this->flip_y = false;
}
DjvuPage::~DjvuPage()
{}

void DjvuPage::Render(plcl::RenderingDevice *output) {
	/*if ((width > 3000) || (height > 3000)) {
		cpcl::Trace(CPCL_TRACE_LEVEL_WARNING, "DjvuPage::RenderData(): request for image(width = %u, height = %u) rejected",
			width, height);
		return false;
	}*/

	std::auto_ptr<DjvuRenderingData> data;
	try {
	DJVU::GRect dst_rect(0, 0, width, height);
	if (image->is_legal_bilevel()) {
		data.reset(new DjvuRenderingData(PLCL_PIXEL_FORMAT_GRAY_8));

		data->bitmap = image->get_bitmap(dst_rect, dst_rect, 4);
		if (!data->bitmap) {
			cpcl::StringPiece e = cpcl::StringPieceFromLiteral("DjvuPage::RenderData(): image->get_bitmap return NULL");
			cpcl::Error(e);
			throw plcl_exception(e.data());
		}
		data->InvertBitmap();
	} else {
		// data.reset(new DjvuRenderingData(PLCL_PIXEL_FORMAT_RGB_24));
		data.reset(new DjvuRenderingData(PLCL_PIXEL_FORMAT_BGR_24));

		data->pixmap = image->get_pixmap(dst_rect, dst_rect);
		if (!data->pixmap) {
			cpcl::StringPiece e = cpcl::StringPieceFromLiteral("DjvuPage::RenderData(): image->get_pixmap return NULL");
			cpcl::Error(e);
			throw plcl_exception(e.data());
		}

		// data->InvertBitmap(); // bgr -> rgb
	}
	} catch (DJVU::GException const &e) {
		TraceGException(e);
		plcl_exception::throw_formatted(plcl_exception(),
			"DjvuPage::RenderData(): GException(%s)",
			e.get_cause());
	}

	cpcl::ScopedBuf<unsigned char, 0> resample_buf;
	cpcl::ScopedBuf<unsigned char, 3 * 1024> conv_buf;
	plcl::Render(plcl::RenderingDataReader(data.get()), data->Width(), data->Height(), data->Pixfmt(),
		output, width, height, true,
		resample_buf, conv_buf);
}

bool DjvuPage::Text(wchar_t const **v, unsigned int *size) {
	if (!GetSelectionInfo())
		return false;
	if (v)
		*v = selection_info.text.c_str();
	if (size)
		*size = selection_info.text.size();
	return true;
}

static DjVuTXT::Zone* min_zone(DjVuTXT::Zone *z, int text_start, int text_length) {
	DjVuTXT::Zone *r(NULL);

	if ((z->text_start <= text_start) && (z->text_start + z->text_length >= text_start + text_length)) {
		for (DJVU::GPosition pos = z->children; 0 != (int)pos; ++pos) {
			r = min_zone(&z->children[pos], text_start, text_length);
			if (r)
				return r;
		}
		r = z;
	}
	return r;
}

/*inline void mtx_transform(agg::trans_affine const &mtx, RectContainer::Rect &rect) {
	double x = rect.x1, y = rect.y1;
	mtx.transform(&x, &y);
	rect.x1 = static_cast<int>(x); rect.y1 = static_cast<int>(y);
	
	x = rect.x2; y = rect.y2;
	mtx.transform(&x, &y);
	rect.x2 = static_cast<int>(x); rect.y2 = static_cast<int>(y);
}*/
bool DjvuPage::GetSelectionInfo() {
	if (!selection_info.text.empty())
		return true;

	if (!text)
		return false;
	if (!text->txt) {
		cpcl::Trace(CPCL_TRACE_LEVEL_WARNING, "DjvuPage::Text(): DjVuText can be with <NULL> DjVuTXT");
		return false;
	}
	if ((!text->txt->textUTF8) || (text->txt->page_zone.rect.isempty()))
		return false;

	/*agg::trans_affine mtx;
	mtx.translate(-double(width_dp) / 2, -double(height_dp) / 2);
	mtx.flip_y();
	mtx.translate(double(width_dp) / 2, double(height_dp) / 2);*/

	unsigned char const *page_text = (unsigned char const*)(char const*)text->txt->textUTF8;
	bool is_utf(true);
	
	int length = (int)(text->txt->textUTF8.length()&INT_MAX);
	selection_info.text.reserve((length * 2) / 3);
	DjVuTXT::Zone *z(NULL);
	for (int i = 0; i < length;) {
		int w; int len(1);
		if (is_utf) {
			len = cpcl::utf_to_uc(page_text + i, &w, length - i);
			if (!len) {
				w = cpcl::CP1251_UTF16(page_text[i]);
				is_utf = false;
				len = 1;
			}
		} else
			w = cpcl::CP1251_UTF16(page_text[i]);
		
		DjVuTXT::Zone *page_zone = &text->txt->page_zone;
		if ((z) && ((i + len) <= (z->text_start + z->text_length)))
			page_zone = z;
		
		DjVuTXT::Zone *z_ = min_zone(page_zone, i, len);
		if (z != z_) {
			bool set_rect(true);
			if ((z_) && (z)) {
				set_rect = (z->rect != z->rect) == 0;
			}

			if (set_rect) {
				RectContainer::Rect rect;
				if (z_) {
					rect.x1 = z_->rect.xmin;
					rect.y1 = z_->rect.ymin;
					rect.x2 = z_->rect.xmax;
					rect.y2 = z_->rect.ymax;
				}
				
				/*mtx_transform(mtx, rect);*/

				selection_info.SetRect(rect);
			}
			z = z_;
		}
		
		selection_info.text.append(1, (wchar_t)w);
		i += len;
	}
	selection_info.Done();

	text = DJVU::GP<DJVU::DjVuText>();
	return !selection_info.text.empty();
	//if (selection_info.text.empty()) {
	//	// if (selection_info.rects.empty())
	//	selection_info.rects.push_back(RectContainer::Rect());
	//	return false;
	//}
	//return true;
}
