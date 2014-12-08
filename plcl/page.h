// page.h
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
#pragma once

#ifndef __PLCL_PAGE_H
#define __PLCL_PAGE_H

#include "rendering_device.h"

namespace plcl {

class Page {
protected:
	unsigned int const width_dp; // Density-independent pixel - dip ~ dp ))
	unsigned int const height_dp;
	unsigned int const dpi;
	unsigned int width;
	unsigned int height;
	unsigned int zoom;
	double device_dpi;
	unsigned int exif_orientation;
public:
	Page(unsigned int width_dp_, unsigned int height_dp_, unsigned int dpi_);
	virtual ~Page();

	virtual unsigned int Width() const;
	virtual void Width(unsigned int v);
	virtual unsigned int Height() const;
	virtual void Height(unsigned int v);
	virtual unsigned int Zoom() const;
	virtual void Zoom(unsigned int v);
	virtual double Dpi() const;
	virtual void Dpi(double v);

	virtual void Render(RenderingDevice *output) = 0;
	
	virtual unsigned int ExifOrientation() const;
	virtual void ExifOrientation(unsigned int v);
	static void ExifOrientation(unsigned int exif_orientation, bool *mirror_x, bool *mirror_y, unsigned int *angle);
};

} // namespace plcl

/*class TextPage : public Page {
RectContainer selection_info;
std::map<size_t, size_t> selection; // <start, lenght>
public:
Text(wchar_t **, unsigned int*);
RenderSelection();
Select(start, lenght);
};

template<class T, class UpCast>
UpCast* try_cast(T *v) {
 UpCast *r = NULL;
 try {
  r = dynamic_cast<UpCast*>(v);
 } catch(std::bad_cast const&) {
  r = NULL;
 }
 return r;
}

IDocumentImpl::GetPage() {
 Page page = doc->GetPage();
 TextPage *text_page = try_cast<Page, TextPage>(page);
 if (text_page)
  return new ITextPageImpl(text_page);
 else
  return new IPageImpl(page);
}

IPageImpl : IUnknownImpl, IPage
 CPCL_UNKNOWN_IMP1_MT(IPage)
 ... // IPage methods implementation
ITextPageImpl : IPageImpl, ITextPage
 CPCL_UNKNOWN_IMP2_MT(IPage, ITextPage) // overload IPageImpl QueryInterface
 ... // ITextPage methods implementation, not duplicate IPage methods

 but if overload QueryInterface, what about ITextPage as IPage, and then IPage as ITextPage ???
 IPage.QueryInterface - call IPageImpl QueryInterface interface implementation???


ITextPageImpl* ptr = new ITextPageImpl();
IPage *page
ptr->QueryInterface(Iuidof(IPage), &page);
page->QueryInterface(Iuidof(IPage), &page_); ???  IPageImpl::QueryInterface or ITextPageImpl::QueryInterface or abstract i.e. exception ???
*/

#endif // __PLCL_PAGE_H
