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

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include "ifeaturedetectorlist_impl.h"
#include "bstr_wrapper.hpp"

#include "impl_exception_helper.hpp"

#include "ifeaturedetector_impl.h"
#include "iobjectdetector_impl.h"

IFeatureDetectorListImpl::IFeatureDetectorListImpl()
{}
IFeatureDetectorListImpl::~IFeatureDetectorListImpl()
{}

STDMETHODIMP IFeatureDetectorListImpl::FindItem(BSTR detector_name, IFeatureDetector **v) {
	try {
		if (!detector_name)
			return E_INVALIDARG;

		cpcl::ComPtr<IFeatureDetector> r;
		cpcl::WStringPiece detector_name_(detector_name, ::SysStringLen(detector_name));
		for (size_t i = 0, size = container.size(); i < size; ++i) {
			cpcl::BSTRWrapper s;
			THROW_FAILED(container[i]->get_Name(s.ReleaseAndGetAddressOf()));

			if ((cpcl::WStringPiece)s == detector_name_) {
				r = container[i];
				break;
			}
		}
		if (!r)
			return S_FALSE;

		if (v)
			*v = r.Detach();
		return S_OK;
	} CATCH_EXCEPTION("IFeatureDetectorListImpl::FindItem()")
}

HRESULT IFeatureDetectorListImpl::Create(IFeatureDetectorList **v) {
	cpcl::ComPtr<IFeatureDetectorList> r(new IFeatureDetectorListImpl());

	/*cpcl::ComPtr<IFeatureDetector> item;
	if SUCCEEDED(IFeatureDetectorImpl::Create(item.GetAddressOf())) {
		THROW_FAILED(r->AddItem(item, NULL));
	}*/
	CFLibraryAPI *library_api = ObjectDetector::Load();
	if (!library_api) {
		cpcl::Warning(cpcl::StringPieceFromLiteral("IFeatureDetectorListImpl::Create(): unable to initialize CFLibraryAPI"));
		return E_NOTIMPL;
	}
	std::vector<DetectorInfo> detectors = ObjectDetector::ReadConfig();
	if (detectors.empty()) {
		cpcl::Warning(cpcl::StringPieceFromLiteral("IFeatureDetectorListImpl::Create(): detectors list in \"cf_params\\cf_params.xml\" is empty or file not found"));
	}
	/* first add face detector */
	for (std::vector<DetectorInfo>::const_iterator i = detectors.begin(), tail = detectors.end(); i != tail; ++i) {
		if (DetectorInfo::FACE_DETECTOR == i->type) {
			cpcl::ComPtr<IFeatureDetector> item;
			if SUCCEEDED(IFeatureDetectorImpl::Create(library_api, *i, item.GetAddressOf())) {
				THROW_FAILED(r->AddItem(item, NULL));
			}
			detectors.erase(i);
			break;
		}
	}
	for (std::vector<DetectorInfo>::const_iterator i = detectors.begin(), tail = detectors.end(); i != tail; ++i) {
		/* switch(i->type)
		 case OBJECT_DETECTOR:
		  break;
		 case PEOPLE_DETECTOR:
		  break;
		 default:
		  Warning("unexpected detector type") */
		if (DetectorInfo::OBJECT_DETECTOR == i->type) {
			cpcl::ComPtr<IFeatureDetector> item;
			if SUCCEEDED(IObjectDetectorImpl::Create(library_api, *i, item.GetAddressOf())) {
				THROW_FAILED(r->AddItem(item, NULL));
			}
		}
	}

	if (v)
		*v = r.Detach();
	return S_OK;
}
