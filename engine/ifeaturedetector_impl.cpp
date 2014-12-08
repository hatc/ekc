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

#include <com_ptr.hpp>
#include <dumbassert.h>

#include <memory_rendering_device.h>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include <irenderingdevice_impl.h>

#include "ifeaturedetector_impl.h"
#include "ifeaturelist_impl.h"
#include "ifeatureattributelist_impl.h"
#include "cf_error_helper.hpp"

#include <impl_exception_helper.hpp>

IFeatureDetectorImpl::IFeatureDetectorImpl()
{}
IFeatureDetectorImpl::~IFeatureDetectorImpl()
{}

STDMETHODIMP IFeatureDetectorImpl::get_ID(CLSID *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = FaceFeaturesExtractor::IID_FaceFeaturesExtractor;
		return S_OK;
	} CATCH_EXCEPTION("IFeatureDetectorImpl::get_ID()")
}

STDMETHODIMP IFeatureDetectorImpl::get_Name(BSTR *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = ::SysAllocStringLen(facefeaturesextractor->detector.name.c_str(), facefeaturesextractor->detector.name.size());
		return S_OK;
	} CATCH_EXCEPTION("IFeatureDetectorImpl::get_Name()")
}

STDMETHODIMP IFeatureDetectorImpl::LoadClassifiers(IFeatureAttributeList **v) {
	try {
		cpcl::ComPtr<IFeatureAttributeList> feature_attribute_list(new IFeatureAttributeListImpl());
		for (std::list<AttributeInfo>::const_iterator i = facefeaturesextractor->detector.attributes.begin(), tail = facefeaturesextractor->detector.attributes.end(); i != tail; ++i) {
			std::auto_ptr<FaceFeaturesExtractor::AttributeFilter> attribute_filter(facefeaturesextractor->CreateAttributeFilter(*i));
			if (attribute_filter.get()) {
				cpcl::ComPtr<IFeatureAttribute> item(new IFeatureAttributeImpl(attribute_filter));
				feature_attribute_list->AddItem(item, NULL);
			}
		}

		if (v)
			*v = feature_attribute_list.Detach();
		return S_OK;
	} CATCH_EXCEPTION("IFeatureDetectorImpl::LoadClassifiers()")
}

STDMETHODIMP IFeatureDetectorImpl::Find(IPluginPage *page, IFeatureList **v) {
	try {
		if (!page)
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

		std::vector<FaceFeaturesExtractor::FaceFeature> faces = facefeaturesextractor->Find(bitmap_info.get());
		cpcl::ComPtr<IFeatureList> feature_list(new IFeatureListImpl(faces.begin(), faces.end()));

		if (v)
			*v = feature_list.Detach();
		return S_OK;
	} CATCH_EXCEPTION("IFeatureDetectorImpl::Find()")
}

STDMETHODIMP IFeatureDetectorImpl::Classify(IPluginPage *page, IFeatureAttributeList *classifiers, IFeatureList **v) {
	try {
		if (!page || !classifiers)
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

		long count(0);
		THROW_FAILED(classifiers->get_Count(&count));
		std::vector<FaceFeaturesExtractor::AttributeFilter*> classifiers_;
		for (size_t i = 0, n = static_cast<size_t>(count); i < n; ++i) {
			cpcl::ComPtr<IFeatureAttribute> feature_attribute;
			THROW_FAILED(classifiers->GetItem(static_cast<long>(i), feature_attribute.GetAddressOf()));

			unsigned char buf[FaceFeaturesExtractor::AttributeFilter::DATA_SIZE];
			int size = static_cast<int>(sizeof(buf));
			THROW_FAILED(feature_attribute->GetValue(buf, &size));
			DUMBASS_CHECK(size == static_cast<int>(sizeof(buf)));

			FaceFeaturesExtractor::AttributeFilter *attribute_filter = FaceFeaturesExtractor::AttributeFilter::Marshal(buf, sizeof(buf));
			if (attribute_filter)
				classifiers_.push_back(attribute_filter);
		}

		std::vector<FaceFeaturesExtractor::FaceFeature> faces = facefeaturesextractor->Classify(bitmap_info.get(), classifiers_);
		cpcl::ComPtr<IFeatureList> feature_list(new IFeatureListImpl(faces.begin(), faces.end()));

		if (v)
			*v = feature_list.Detach();
		return S_OK;
	} CATCH_EXCEPTION("IFeatureDetectorImpl::Classify()")
}

HRESULT IFeatureDetectorImpl::Create(IFeatureDetector **v) {
	try {
		cpcl::ComPtr<IFeatureDetectorImpl> r(new IFeatureDetectorImpl());
		r->facefeaturesextractor.reset(FaceFeaturesExtractor::Create());

		if (r->facefeaturesextractor.get()) {
			if (v)
				*v = r.Detach();

			return S_OK;
		} else {
			cpcl::Warning(cpcl::StringPieceFromLiteral("IFeatureDetectorImpl::Create(): unable to initialize CFLocateFace"));
			return E_NOTIMPL;
		}
	} catch (awp_exception const &e) {
		cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
			"IFeatureDetectorImpl::Create(): unable to initialize CFLocateFace: %s",
			e.what());
		return E_NOTIMPL;
	} CATCH_EXCEPTION("IFeatureDetectorImpl::Create()")
}

HRESULT IFeatureDetectorImpl::Create(CFLibraryAPI *library_api, DetectorInfo const &detector, IFeatureDetector **v) {
	try {
		cpcl::ComPtr<IFeatureDetectorImpl> r(new IFeatureDetectorImpl());
		r->facefeaturesextractor.reset(FaceFeaturesExtractor::Create(library_api, detector));

		if (r->facefeaturesextractor.get()) {
			if (v)
				*v = r.Detach();

			return S_OK;
		} else {
			cpcl::Warning(cpcl::StringPieceFromLiteral("IFeatureDetectorImpl::Create(): unable to initialize CFLocateFace"));
			return E_NOTIMPL;
		}
	} catch (awp_exception const &e) {
		cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
			"IFeatureDetectorImpl::Create(): unable to initialize CFLocateFace: %s",
			e.what());
		return E_NOTIMPL;
	} CATCH_EXCEPTION("IFeatureDetectorImpl::Create()")
}
