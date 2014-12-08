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

#include "ifeatureattributelist_impl.h"

#include <impl_exception_helper.hpp>

IFeatureAttributeListImpl::IFeatureAttributeListImpl()
{}
IFeatureAttributeListImpl::~IFeatureAttributeListImpl()
{}

STDMETHODIMP IFeatureAttributeListImpl::Classify(IPluginPage *page, IFeature *feature) {
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

		std::vector<FaceFeaturesExtractor::AttributeFilter*> classifiers;
		for (size_t i = 0; i < container.size(); ++i) {
			unsigned char buf[FaceFeaturesExtractor::AttributeFilter::DATA_SIZE];

			int size = static_cast<int>(sizeof(buf));
			THROW_FAILED(container[i]->GetValue(buf, &size));
			DUMBASS_CHECK(size == static_cast<int>(sizeof(buf)));

			FaceFeaturesExtractor::AttributeFilter *attribute_filter = FaceFeaturesExtractor::AttributeFilter::Marshal(buf, sizeof(buf));
			if (attribute_filter)
				classifiers.push_back(attribute_filter);
		}

		// clear attributes with same classifiers
		for (size_t i = 0; i < classifiers.size(); ++i) {
			THROW_FAILED(feature->SetAttribute(NULL, classifiers[i]->classifier_id, NULL));
		}

		if (FaceFeaturesExtractor::AttributeFilter::Classify(classifiers, bitmap_info.get(), face_feature)) {
			for (std::map<GUID, std::wstring>::const_iterator i = face_feature.classes.begin(), tail = face_feature.classes.end(); i != tail; ++i) {
				THROW_FAILED(feature->SetAttribute((*i).second.c_str(), (*i).first, NULL));
			}
			return S_OK;
		}
		return S_FALSE;
	} CATCH_EXCEPTION("IFeatureAttributeListImpl::Classify()")
}
