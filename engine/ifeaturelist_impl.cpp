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

#include <scoped_buf.hpp>
#include <dumbassert.h>

#include <memory_rendering_device.h>
#include <render.hpp>
#include <irenderingdevice_wrapper.h>
#include <draw_util.h>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include <irenderingdevice_impl.h>

#include "ifeaturelist_impl.h"

#include <impl_exception_helper.hpp>

IFeatureListImpl::IFeatureListImpl()
{}
IFeatureListImpl::~IFeatureListImpl()
{}

// 7656F6C9-C7D7-5463-95A8-13FB8A453120
GUID const IFeatureListImpl::IID_CFeatureList = { 0x7656f6c9, 0xc7d7, 0x5463, { 0x95, 0xa8, 0x13, 0xfb, 0x8a, 0x45, 0x31, 0x20 } };

STDMETHODIMP IFeatureListImpl::Render(IPluginPage *page,
	unsigned char r, unsigned char g, unsigned char b, IRenderingDevice *output) {
	try {
		if ((!page) || (!output))
			return E_INVALIDARG;

		plcl::MemoryRenderingDevice *rendering_device_ = new plcl::MemoryRenderingDevice();
		cpcl::ComPtr<IRenderingDevice> rendering_device(new IRenderingDeviceImpl(rendering_device_)); // rendering_device_ object leak if new IRenderingDeviceImpl throw due to low memory
		THROW_FAILED(page->Render(rendering_device));

		for (size_t i = 0; i < container.size(); ++i) {
			// unsigned char buf[(sizeof(unsigned int) * 2 + sizeof(int) * 3)];
			unsigned char buf[FaceFeaturesExtractor::FaceFeature::DATA_SIZE];

			int size = static_cast<int>(sizeof(buf));
			THROW_FAILED(container[i]->GetValue(buf, &size));
			DUMBASS_CHECK(size == static_cast<int>(sizeof(buf)));

			FaceFeaturesExtractor::FaceFeature face_feature(buf, sizeof(buf));
			face_feature.Draw(rendering_device_->Storage(), r, g, b);
		}

		IRenderingDeviceWrapper output_(output);
		cpcl::ScopedBuf<unsigned char, 0> resample_buf;
		cpcl::ScopedBuf<unsigned char, 0> conv_buf;
		plcl::Render(plcl::RenderingDataReader(rendering_device_->Storage()),
			rendering_device_->Storage()->Width(), rendering_device_->Storage()->Height(), rendering_device_->Storage()->Pixfmt(),
			&output_, rendering_device_->Storage()->Width(), rendering_device_->Storage()->Height(), false,
			resample_buf, conv_buf);
		return S_OK;
	} CATCH_EXCEPTION("IFeatureListImpl::Render()")
}

STDMETHODIMP IFeatureListImpl::GetClassID(CLSID *pClassID) {
	try {
		if (!pClassID)
			return E_INVALIDARG;

		*pClassID = IID_CFeatureList;
		return S_OK;
	} CATCH_EXCEPTION("IFeatureListImpl::GetClassID()")
}

//STDMETHODIMP IFaceFeatureListImpl::IsDirty() {
//	return S_FALSE;
//}
//
//STDMETHODIMP IFaceFeatureListImpl::Load(IStream *pStm) {
//	try {
//		if (!pStm)
//			return E_INVALIDARG;
//
//		size_t items_n;
//		THROW_FAILED(cpcl::read_pod(pStm, &items_n));
//		container.reserve(container.size() + items_n);
//
//		cpcl::ComPtr<ICustomClassFactory> class_factory;
//		while (items_n > 0) {
//			GUID id;
//			THROW_FAILED(cpcl::read_pod(pStm, &id));
//
//			cpcl::ComPtr<IPersistStream> item;
//			if (!!class_factory) {
//				if (class_factory->CreateInstance(id, item.GetAddressOf()) != S_OK)
//					class_factory.Release();
//			}
//			if (!class_factory) {
//				THROW_FAILED(CustomClassFactoryList::FindFactoryForCLSID(id, class_factory.GetAddressOf()));
//				HRESULT hr = class_factory->CreateInstance(id, item.ReleaseAndGetAddressOf());
//				if (hr != S_OK) {
//					if FAILED(hr) {
//						hresult_exception::throw_formatted(hresult_exception(hr),
//							"class_factory->CreateInstance(id, item) fails with code: 0x%08X",
//							hr);
//					} else {
//						hresult_exception::throw_formatted(hresult_exception(E_FAIL),
//							"unable to create a item instance, class_factory->CreateInstance(id, item) return code: 0x%08X",
//							hr);
//					}
//				}
//			}
//
//			cpcl::ComPtr<IFaceFeature> face_feature_item;
//			THROW_FAILED(item->QueryInterface(face_feature_item.GetAddressOf()));
//
//			THROW_FAILED(item->Load(pStm));
//
//			THROW_FAILED(AddItem(face_feature_item, 0));
//
//			--items_n;
//		}
//
//		return S_OK;
//	} CATCH_EXCEPTION("IFaceFeatureListImpl::Load()")
//}
//
//STDMETHODIMP IFaceFeatureListImpl::Save(IStream *pStm, BOOL) {
//	try {
//		if (!pStm)
//			return E_INVALIDARG;
//
//		size_t items_n = container.size();
//		THROW_FAILED(cpcl::write_pod(pStm, items_n));
//
//		for (size_t i = 0; i < items_n; ++i) {
//			cpcl::ComPtr<IPersistStream> item;
//			// if (container[i]->QueryInterface(item.GetAddressOf()) == S_OK) {
//			THROW_FAILED(container[i]->QueryInterface(item.GetAddressOf())); // if item not IPersistStream, then items_n invalid
//				GUID id;
//				THROW_FAILED(item->GetClassID(&id));
//				THROW_FAILED(cpcl::write_pod(pStm, id));
//
//				THROW_FAILED(item->Save(pStm, TRUE));
//			// }
//		}
//
//		return S_OK;
//	} CATCH_EXCEPTION("IFaceFeatureListImpl::Save()")
//}
//
//STDMETHODIMP IFaceFeatureListImpl::GetSizeMax(ULARGE_INTEGER *pcbSize) {
//	try {
//		if (!pcbSize)
//			return E_INVALIDARG;
//
//		unsigned __int64 size = sizeof(size_t);
//		for (size_t i = 0, items_n = container.size(); i < items_n; ++i) {
//			cpcl::ComPtr<IPersistStream> item;
//			// if (container[i]->QueryInterface(item.GetAddressOf()) == S_OK) {
//			THROW_FAILED(container[i]->QueryInterface(item.GetAddressOf())); // if item not IPersistStream, then items_n in Save() become invalid
//				ULARGE_INTEGER size_;
//				size_.QuadPart = 0;
//				THROW_FAILED(item->GetSizeMax(&size_));
//
//				size += size_.QuadPart;
//			// }
//		}
//
//		(*pcbSize).QuadPart = size;
//		return S_OK;
//	} CATCH_EXCEPTION("IFaceFeatureListImpl::GetSizeMax()")
//}
