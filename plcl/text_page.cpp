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

#include <scoped_buf.hpp>

// #include "draw_selection.h"
#include "text_page.h"
#include "plcl_exception.hpp"

#include "draw_util.h"
#include "memory_rendering_device.h"
#include "render.hpp"

namespace plcl {

TextPage::TextPage(unsigned int width_dp_, unsigned int height_dp_, unsigned int dpi_)
	: Page(width_dp_, height_dp_, dpi_), packed_selection(false), flip_y(true)
{}
TextPage::~TextPage()
{}

bool TextPage::CopyText(wchar_t *v, unsigned int size) {
	wchar_t const *v_;
	unsigned int size_;
	if (!Text(&v_, &size_))
		return false;

	size_ = (std::min)(size, size_);
	wmemcpy(v, v_, size_);
	if (size_ < size)
		v[size_] = 0;
	return true;
}

bool TextPage::Select(unsigned int start, unsigned int size) {
	unsigned int text_size;
	if (!Text(NULL, &text_size))
		return false;
	if (start + size > text_size)
		return false;

	Pairib i = selection.insert(Value(start, size));
	if (!i.second) {
		(*i.first).second = (std::max)((*i.first).second, size);
	}
	rects.clear();
	packed_selection = false;
	return true;
}

template<class Rect>
inline Rect unite_rectangles(Rect const &r1, Rect const &r2) {
	Rect r = r1;
	if (r.x2 < r2.x2)
		r.x2 = r2.x2;
	if (r.y2 < r2.y2)
		r.y2 = r2.y2;
	if (r.x1 > r2.x1)
		r.x1 = r2.x1;
	if (r.y1 > r2.y1)
		r.y1 = r2.y1;
	return r;
}
void TextPage::RenderSelection(RenderingDevice *output) {
	if (selection.empty()) {
		this->Render(output); // Page::Render(output);
		return;
	}

	if (!packed_selection) {
		std::map<unsigned int, unsigned int> selection_;
		Iterator i = selection.begin();
		Iterator j = selection_.insert(*i).first;
		while (++i != selection.end()) {
			unsigned int const jh((*j).first), ih((*i).first), jt((*j).first + (*j).second), it((*i).first + (*i).second);
			if (ih <= jt) {
				// [jh..[ih...it]...jt]
				// if (it <= jt)
				// 	continue;
				// [jh....[ih..jt]...it]
				if (it > jt) {
					(*j).second += it - jt;
				}
			} else {
				j = selection_.insert(*i).first;
			}
		}
		selection.swap(selection_);
		
		for (Iterator i = selection.begin(); i != selection.end(); ++i) {
			RectContainer::Rects_i first = selection_info.FindRect((*i).first);
			if (first == selection_info.rects.end())
				throw plcl_exception("TextPage::RenderSelection(): selection_info.FindRect() fails");
			
			RectContainer::Rects_i last = first;
			while ((*last).text_endpos < ((*i).first + (*i).second)) {
				if (++last == selection_info.rects.end()) {
					break;
				}
			}
			
			RectContainer::Rect r = *first;
			while (first != last) {
				RectContainer::Rect const &r_ = *++first;
				// if ((r_.y1 <= r.y2) || (r.y1 <= r_.y2))
				if (r_.x1 >= r.x1)
					r = unite_rectangles(r, r_);
				else {
					rects.push_back(r);
					r = r_;
				}
			}
			
			rects.push_back(r);
		}
		
		if (flip_y) {
			for (size_t i = 0, n = rects.size(); i < n; ++i) {
				rects[i].y1 = height_dp - rects[i].y1;
				rects[i].y2 = height_dp - rects[i].y2;
			}
		}

		packed_selection = true;
	}

	MemoryRenderingDevice rendering_device(PLCL_PIXEL_FORMAT_BGR_24 | PLCL_PIXEL_FORMAT_BGRA_32); // BGR24 | BGRA32
	this->Render(&rendering_device);
	for (size_t i = 0, n = rects.size(); i < n; ++i) {
		double vertexes[] = {
			rects[i].x1, rects[i].y1, // l,t
			rects[i].x2, rects[i].y1, // r,t
			rects[i].x2, rects[i].y2, // r,b
			rects[i].x1, rects[i].y2  // l,b
		};

		DrawPolygon(rendering_device.Storage(),
			width_dp, height_dp, vertexes, arraysize(vertexes) / 2,
			230, 230, 26, 127); // DrawPolygon supports GRAY8 | BGR24 | BGRA32
	}

	cpcl::ScopedBuf<unsigned char, 0> resample_buf;
	cpcl::ScopedBuf<unsigned char, 0> conv_buf;
	plcl::Render(RenderingDataReader(rendering_device.Storage()),
		rendering_device.Storage()->Width(), rendering_device.Storage()->Height(), rendering_device.Storage()->Pixfmt(),
		output, width, height, false,
		resample_buf, conv_buf);
}

#if 0
void TextPage::RenderSelection(RenderingData *data, RenderingDevice *output) {
	if (selection.empty()) {
		Page::Render(data, output);
	} else {
		unsigned int const data_width = data->Width(); unsigned int const data_height = data->Height();
		if ((width != data_width) || (height != data_height))
			throw plcl_exception("Page::Render() invalid rendering data");
		if ((width > 3000) || (height > 3000)) {
			plcl_exception::throw_formatted(plcl_exception(),
				"TextPage::RenderSelection() oversized image(width = %u, height = %u)",
				width, height);
		}

		if (sweeper.get() == 0) {
		if (!packed_selection) {
			std::map<unsigned int, unsigned int> selection_;
			Iterator i = selection.begin();
			Iterator j = selection_.insert(*i).first;
			while (++i != selection.end()) {
				unsigned int const jh((*j).first), ih((*i).first), jt((*j).first + (*j).second), it((*i).first + (*i).second);
				if (ih <= jt) {
					// [jh..[ih...it]...jt]
					// if (it <= jt)
					// 	continue;
					// [jh....[ih..jt]...it]
					if (it > jt) {
						(*j).second += it - jt;
					}
				} else {
					j = selection_.insert(*i).first;
				}
			}

			selection.swap(selection_);
			packed_selection = true;
		}

		sweeper.reset(new SelectionScanlineSweeper(width_dp, height_dp, flip_y));

		for (Iterator i = selection.begin(); i != selection.end(); ++i) {
			RectContainer::Rects_i first = selection_info.FindRect((*i).first);
			if (first == selection_info.rects.end())
				throw plcl_exception("TextPage::RenderSelection(): selection_info.FindRect() fails");
			
			RectContainer::Rects_i last = first;
			while ((*last).text_endpos < ((*i).first + (*i).second)) {
				if (++last == selection_info.rects.end()) {
					break;
				}
			}

			RectContainer::Rect r = *first;
			while (first != last) {
				RectContainer::Rect const &r_ = *++first;
				// if ((r_.y1 <= r.y2) || (r.y1 <= r_.y2))
				if (r_.x1 >= r.x1)
					r = agg::unite_rectangles(r, r_);
				else {
					sweeper->rects.push_back(r);
					r = r_;
				}
			}
			sweeper->rects.push_back(r);
			// sweeper add vertex
		}
		}

		unsigned int supported_pixfmt = output->SupportedPixfmt();
		if (supported_pixfmt & PLCL_PIXEL_FORMAT_BGR_24) {
			output->Pixfmt(PLCL_PIXEL_FORMAT_BGR_24);
			throw 0; // data->Pixfmt(PLCL_PIXEL_FORMAT_BGR_24);
		} else if (supported_pixfmt & PLCL_PIXEL_FORMAT_RGB_24) {
			output->Pixfmt(PLCL_PIXEL_FORMAT_RGB_24);
			throw 0; // data->Pixfmt(PLCL_PIXEL_FORMAT_RGB_24);
		} else {
			throw plcl_exception("TextPage::RenderSelection rendering device must support bgr|rgb pixel format");
		}

		/*ColorConv color_conv = GetConverter(data->Pixfmt(), PLCL_PIXEL_FORMAT_BGR_24);
		cpcl::ScopedBuf<unsigned char, 3 * 1280> scanline_buf;

		sweeper->conv_scanline = scanline_buf.Alloc(color_conv.storage_needed(width));
		sweeper->conv_scanline_size = scanline_buf.Size();

		if (!output->SetViewport(0, 0, width, height))
			plcl_exception::throw_formatted(plcl_exception(), "TextPage::RenderSelection(): SetViewport(0, 0, %d, %d) fails", width, height);
		sweeper->Draw(output, data, color_conv);
		output->Render();*/

		sweeper->Draw(data, (supported_pixfmt & PLCL_PIXEL_FORMAT_BGR_24) != 0);

		if (!output->SetViewport(0, 0, width, height))
			plcl_exception::throw_formatted(plcl_exception(), "TextPage::RenderSelection(): SetViewport(0, 0, %d, %d) fails", width, height);
		for (unsigned int y = 0; y < height; ++y)
			output->SweepScanline(data->Scanline(y));
		output->Render();
	}
}
#endif

} // namespace plcl
