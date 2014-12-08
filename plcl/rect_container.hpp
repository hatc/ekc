// rect_container.hpp
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

#include <algorithm>
#include <vector>
#include <string>

struct RectContainer {
	struct Rect {
		int x1, y1, x2, y2;
		size_t text_endpos; // text [0, n - 1], rect.text_endpos = n; i.e. text_endpos cannot be zero - for 1(pos = 0) character, rect.text_endpos = 1

		Rect() : x1(0), y1(0), x2(0), y2(0), text_endpos(0) {}
		Rect(int x1_, int y1_, int x2_, int y2_) : x1(x1_), y1(y1_), x2(x2_), y2(y2_), text_endpos(0) {}

		bool Empty() const { return !!(x1 | y1 | x2 | y2); }
	};
	struct RectTextLess { bool operator()(Rect const &l, Rect const &r) const { return l.text_endpos < r.text_endpos; } };
	typedef std::vector<Rect> Rects;
	typedef Rects::const_iterator Rects_i;

	std::wstring text;
	Rects rects;
	Rect rect;

	RectContainer() {}
	operator bool() const { return !(text.empty() || rects.empty()); }

	void SetRect(Rect v) {
		if (!!(rect.text_endpos = text.size()))
			rects.push_back(rect);
		rect = v;
	}
	void Done() {
		SetRect(Rect());
	}
	Rects_i FindRect(size_t text_pos) const {
		Rect v; v.text_endpos = text_pos;
		return std::upper_bound(rects.begin(), rects.end(), v, RectTextLess());
	}
	/*(size_t text_offset, size_t text_length) {
	text_tail = (text_offset + text_length);
	for (Rects_i i = container.FindRect(text_pos); i != container.rects.end(); ++i) {
	 check && union rects
   ...
	 if ((*i).text_endpos > text_tail)
	  break;
	}
	}*/
	/*int FindRect(size_t text_pos) const {
		Rect v; v.text_endpos = text_pos;
		Rects_i b = rects.begin(), e = rects.end();

		Rects_i it = std::upper_bound(b, e, v, RectTextLess());
		int offset(-1);
		if (it != e)
			offset = it - b;
		return offset;
	}*/
	int SearchRect(size_t text_pos) const {
		Rect v; v.text_endpos = text_pos;
		Rects_i b = rects.begin(), e = rects.end();

		Rects_i it = std::upper_bound(b, e, v, RectTextLess());
		int offset(-1);
		if (it != e)
			offset = it - b;
		return offset;
	}
};
