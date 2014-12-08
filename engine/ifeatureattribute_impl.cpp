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

#include <com_ptr.hpp>
#include <dumbassert.h>

#include <memory_rendering_device.h>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include <irenderingdevice_impl.h>

#include "ifeatureattribute_impl.h"

#include <impl_exception_helper.hpp>

IFeatureAttributeImpl::IFeatureAttributeImpl(std::auto_ptr<FaceFeaturesExtractor::AttributeFilter> &attribute_filter_)
	: attribute_filter(attribute_filter_)
{}
IFeatureAttributeImpl::~IFeatureAttributeImpl()
{}

STDMETHODIMP IFeatureAttributeImpl::get_ID(CLSID *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = attribute_filter->classifier_id;
		return S_OK;
	} CATCH_EXCEPTION("IFeatureAttributeImpl::get_ID()")
}

STDMETHODIMP IFeatureAttributeImpl::get_Name(BSTR *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = ::SysAllocStringLen(attribute_filter->classifier_name.c_str(), attribute_filter->classifier_name.size());
		return S_OK;
	} CATCH_EXCEPTION("IFeatureAttributeImpl::get_Name()")
}

STDMETHODIMP IFeatureAttributeImpl::get_ClassCount(long *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = static_cast<long>(attribute_filter->classes.size());
		return S_OK;
	} CATCH_EXCEPTION("IFeatureAttributeImpl::get_ClassCount()")
}

STDMETHODIMP IFeatureAttributeImpl::GetClass(long index, BSTR *class_name) {
	try {
		size_t index_ = static_cast<size_t>(index);
		if (index_ >= attribute_filter->classes.size())
			return E_INVALIDARG;

		std::map<int, std::wstring>::const_iterator it = attribute_filter->classes.begin();
		std::advance(it, index_);

		if (class_name)
			*class_name = ::SysAllocStringLen((*it).second.c_str(), (*it).second.size());
		return S_OK;
	} CATCH_EXCEPTION("IFeatureAttributeImpl::GetClass()")
}

STDMETHODIMP IFeatureAttributeImpl::GetValue(unsigned char *buffer, int *buffer_size) {
	try {
		if ((!buffer) || (!buffer_size))
			return E_INVALIDARG;

		HRESULT hr(S_FALSE);
		if (*buffer_size >= static_cast<int>(FaceFeaturesExtractor::AttributeFilter::DATA_SIZE)) {
			DUMBASS_CHECK(attribute_filter->GetData(buffer, FaceFeaturesExtractor::AttributeFilter::DATA_SIZE));
			hr = S_OK;
		}

		*buffer_size = static_cast<int>(FaceFeaturesExtractor::AttributeFilter::DATA_SIZE);
		return hr;
	} CATCH_EXCEPTION("IFeatureAttributeImpl::GetValue()")
}

STDMETHODIMP IFeatureAttributeImpl::SetValue(unsigned char const*, int) {
	return E_INVALIDARG;
}

STDMETHODIMP IFeatureAttributeImpl::Classify(IPluginPage *page, IFeature *feature, long *index) {
	try {
		if ((!page) || (!feature))
			return E_INVALIDARG;

		DWORD value;
		THROW_FAILED(page->get_Width(&value));
		unsigned int width = static_cast<unsigned int>(value);
		THROW_FAILED(page->get_Height(&value));
		unsigned int height = static_cast<unsigned int>(value);

		std::auto_ptr<plcl::BitmapInfo> bitmap_info(new plcl::BitmapInfo(PLCL_PIXEL_FORMAT_BGR_24, width, height));
		plcl::MemoryRenderingDevice *rendering_device_ = new plcl::MemoryRenderingDevice(bitmap_info.get());
		cpcl::ComPtr<IRenderingDevice> rendering_device(new IRenderingDeviceImpl(rendering_device_)); // rendering_device_ object leak if new IRenderingDeviceImpl throw due to low memory
		THROW_FAILED(page->Render(rendering_device));

		unsigned char buf[FaceFeaturesExtractor::FaceFeature::DATA_SIZE];
		int size = static_cast<int>(sizeof(buf));
		THROW_FAILED(feature->GetValue(buf, &size));
		DUMBASS_CHECK(size == static_cast<int>(sizeof(buf)));
		FaceFeaturesExtractor::FaceFeature face_feature(buf, sizeof(buf));
		if (attribute_filter->Classify(bitmap_info.get(), face_feature)) {
			THROW_FAILED(feature->SetAttribute((*face_feature.classes.begin()).second.c_str(), (*face_feature.classes.begin()).first, NULL));
			return S_OK;
		}
		return S_FALSE;
	} CATCH_EXCEPTION("IFeatureAttributeImpl::Classify()")
}
