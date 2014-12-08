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

#include <com_ptr.hpp>
#include <dumbassert.h>

#include <memory_rendering_device.h>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include <irenderingdevice_impl.h>

#include "iobjectdetector_impl.h"
#include "ifeaturelist_impl.h"
#include "ifeatureattributelist_impl.h"
#include "cf_error_helper.hpp"

#include <impl_exception_helper.hpp>

IObjectDetectorImpl::IObjectDetectorImpl()
{}
IObjectDetectorImpl::~IObjectDetectorImpl()
{}

STDMETHODIMP IObjectDetectorImpl::get_ID(CLSID *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = object_detector->detector.id;
		return S_OK;
	} CATCH_EXCEPTION("IObjectDetectorImpl::get_ID()")
}

STDMETHODIMP IObjectDetectorImpl::get_Name(BSTR *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = ::SysAllocStringLen(object_detector->detector.name.c_str(), object_detector->detector.name.size());
		return S_OK;
	} CATCH_EXCEPTION("IObjectDetectorImpl::get_Name()")
}

STDMETHODIMP IObjectDetectorImpl::LoadClassifiers(IFeatureAttributeList **v) {
	try {
		if (v) {
			cpcl::ComPtr<IFeatureAttributeList> feature_attribute_list(new IFeatureAttributeListImpl());
			*v = feature_attribute_list.Detach();
		}
		return S_OK;
	} CATCH_EXCEPTION("IObjectDetectorImpl::LoadClassifiers()")
}

STDMETHODIMP IObjectDetectorImpl::Find(IPluginPage *page, IFeatureList **v) {
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

		std::vector<FaceFeaturesExtractor::FaceFeature> faces = object_detector->Find(bitmap_info.get());
		cpcl::ComPtr<IFeatureList> feature_list(new IFeatureListImpl(faces.begin(), faces.end()));

		if (v)
			*v = feature_list.Detach();
		return S_OK;
	} CATCH_EXCEPTION("IObjectDetectorImpl::Find()")
}

STDMETHODIMP IObjectDetectorImpl::Classify(IPluginPage *page, IFeatureAttributeList*, IFeatureList **v) {
	return Find(page, v);
}

HRESULT IObjectDetectorImpl::Create(CFLibraryAPI *library_api, DetectorInfo const &detector, IFeatureDetector **v) {
	try {
		cpcl::ComPtr<IObjectDetectorImpl> r(new IObjectDetectorImpl());
		r->object_detector.reset(ObjectDetector::Create(library_api, detector));

		if (r->object_detector.get()) {
			if (v)
				*v = r.Detach();

			return S_OK;
		} else {
			cpcl::Warning(cpcl::StringPieceFromLiteral("IObjectDetectorImpl::Create(): unable to initialize CFObjectDetector"));
			return E_NOTIMPL;
		}
	} catch (awp_exception const &e) {
		cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
			"IObjectDetectorImpl::Create(): unable to initialize CFObjectDetector: %s",
			e.what());
		return E_NOTIMPL;
	} CATCH_EXCEPTION("IObjectDetectorImpl::Create()")
}
