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

#ifndef __TEXT_HACK_H
#define __TEXT_HACK_H

#include <poppler/PDFDoc.h>

#include <rect_container.hpp>

//struct hackRectContainer {
//	// Result text may be NULL
//	//std::string texta;
//	std::wstring textu;
//
//	//CaptureRects rectsa;
//	CaptureRects rectsu;
//
//	CaptureRect  activea;
//	CaptureRect  activeu;
//
//	void drawChar(wchar_t u, char *text, int tLen);
//	void setRect(CaptureRect const &r);
//  void done();
//	
//	// find rect for position (for -1 nothing)
//	int findRect(int pos, CaptureRects &rects);
//
//	// store rects to data for symbols of Text in range [start..start+len-1] 
//  int getRects(int start, int len, TRectFlt* data, CaptureRects &rects);
//};

bool retreiveLayout(PDFDoc *pdfDoc, int pageNum, RectContainer *r_);

#endif // __TEXT_HACK_H
