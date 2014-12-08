// text_page.h
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

#ifndef __PLCL_TEXT_PAGE_H
#define __PLCL_TEXT_PAGE_H

#include <memory>
#include <map>

#include "page.h"
#include "rect_container.hpp"

namespace plcl {

struct SelectionScanlineSweeper;
class TextPage : public Page {
	typedef std::map<unsigned int, unsigned int>::iterator Iterator;
	typedef std::map<unsigned int, unsigned int>::_Pairib Pairib;
	typedef std::map<unsigned int, unsigned int>::value_type Value;

	// std::auto_ptr<SelectionScanlineSweeper> sweeper;
protected:
	RectContainer selection_info;
	std::map<unsigned int, unsigned int> selection;
	bool packed_selection;
	bool flip_y;
	std::vector<RectContainer::Rect> rects;
public:
	TextPage(unsigned int width_dp_, unsigned int height_dp_, unsigned int dpi_);
	virtual ~TextPage();

	virtual bool Text(wchar_t const **v, unsigned int *size) = 0;
	virtual bool CopyText(wchar_t *v, unsigned int size);
	virtual bool Select(unsigned int start, unsigned int size);

	virtual void RenderSelection(RenderingDevice *output);
};

} // namespace plcl

#endif // __PLCL_TEXT_PAGE_H
