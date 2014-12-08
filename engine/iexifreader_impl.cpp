// Copyright (C) 2013 Yuri Agafonov
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

#include <libexif/exif-loader.h>

#include <hresult_helper.hpp>
#include <file_util.h>
#include <istream_wrapper.h>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include "iexifreader_impl.h"

#include "impl_exception_helper.hpp"

#include "attr_id.h"

IExifReaderImpl::IExifReaderImpl() : combine_mode(IFileFilterImpl::FILTER_COMBINE_OR)
{}
IExifReaderImpl::~IExifReaderImpl()
{}

STDMETHODIMP IExifReaderImpl::get_Name(BSTR *v) {
	try {
		if (!v)
			return E_INVALIDARG;
		
		*v = ::SysAllocString(L"exif reader");
		return S_OK;
	} CATCH_EXCEPTION("IExifReaderImpl::get_Name()")
}

/* Default = 1
1 = The 0th row is at the visual top of the image, and the 0th column is the visual left-hand side.
2 = The 0th row is at the visual top of the image, and the 0th column is the visual right-hand side.
3 = The 0th row is at the visual bottom of the image, and the 0th column is the visual right-hand side.
4 = The 0th row is at the visual bottom of the image, and the 0th column is the visual left-hand side.
5 = The 0th row is the visual left-hand side of the image, and the 0th column is the visual top.
6 = The 0th row is the visual right-hand side of the image, and the 0th column is the visual top.
7 = The 0th row is the visual right-hand side of the image, and the 0th column is the visual bottom.
8 = The 0th row is the visual left-hand side of the image, and the 0th column is the visual bottom.
http://www.cipa.jp/english/hyoujunka/kikaku/pdf/DC-008-2010_E.pdf */

/* returns: 1-8 if in stream contains EXIF_TAG_ORIENTATION, -1 otherwise */
int ExifTagOrientation(cpcl::IOStream *in) {
	int r(-1);
	ExifLoader *loader = exif_loader_new();
	for (unsigned char state = '\1'; state != '\0';) {
		unsigned char buf[0x1000];
		unsigned int n = in->Read(&buf, sizeof(buf));
		if (n < 1)
			state = '\0';
		else
			state = exif_loader_write(loader, buf, n);
	}
	ExifData *data = exif_loader_get_data(loader);
	if (data) {
		ExifByteOrder byte_order = exif_data_get_byte_order(data);
		ExifEntry *entry = exif_data_get_entry(data, EXIF_TAG_ORIENTATION);
		if (entry)
			r = static_cast<int>(exif_get_short(entry->data, byte_order));
		exif_data_unref(data);
	}
	
	exif_loader_unref(loader);
	return r;
}

STDMETHODIMP IExifReaderImpl::Match(IFileItem *item) {
	try {
		if (!item)
			return E_INVALIDARG;

		ScopedComPtr<IPropertyBasket> properties;
		THROW_FAILED(item->get_Attributes(&properties));
		VARIANT v;
		::VariantInit(&v);
		if SUCCEEDED(properties->GetItem(KPID_IS_DIR, &v)) {
			if ((VT_BOOL == v.vt) && (VARIANT_TRUE == v.boolVal))
				return S_OK/* S_FALSE */;
		}
		if (VT_EMPTY != v.vt)
			::VariantClear(&v);
		
		ScopedComPtr<IStream> stream_;
		THROW_FAILED(item->get_Stream(&stream_));
		cpcl::IStreamWrapper stream(stream_);
		int value_ = ExifTagOrientation(&stream);
		if (value_ < 1)
			value_ = 1;
		unsigned long value = static_cast<unsigned long>(value_);
		::VariantInit(&v);
		v.vt = VT_UI4/*VT_I4*/; v.wReserved1 = 0; v.ulVal = value;
		THROW_FAILED(properties->SetItem(KPID_EXIF_TAG_ORIENTATION, v));

		return S_OK;
	} CATCH_EXCEPTION("IExifReaderImpl::Match()")
}

STDMETHODIMP IExifReaderImpl::get_CombineMode(DWORD *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = combine_mode;
		return S_OK;
	} CATCH_EXCEPTION("IExifReaderImpl::get_CombineMode()")
}

STDMETHODIMP IExifReaderImpl::put_CombineMode(DWORD v) {
	try {
		if (((IFileFilterImpl::FILTER_COMBINE_AND | IFileFilterImpl::FILTER_COMBINE_OR | IFileFilterImpl::FILTER_COMBINE_NOT | IFileFilterImpl::FILTER_COMBINE_XOR) & v) == 0)
			return E_INVALIDARG;

		combine_mode = v;
		return S_OK;
	} CATCH_EXCEPTION("IExifReaderImpl::put_CombineMode()")
}
