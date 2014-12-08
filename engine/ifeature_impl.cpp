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
#include <com_ptr.hpp>
#include <dumbassert.h>

#include <memory_rendering_device.h>
#include <render.hpp>
#include <irenderingdevice_wrapper.h>
#include <draw_util.h>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include <irenderingdevice_impl.h>

#include "ifeature_impl.h"

#include <impl_exception_helper.hpp>

#include "istream_util.hpp"

IFeatureImpl::IFeatureImpl()
{}
IFeatureImpl::IFeatureImpl(FaceFeaturesExtractor::FaceFeature face_feature_) : face_feature(face_feature_)
{}
IFeatureImpl::~IFeatureImpl()
{}

// 0EF7B038-7E8B-5463-B74B-5016D34E87EA
GUID const IFeatureImpl::IID_CFeature = { 0x0ef7b038, 0x7e8b, 0x5463, { 0xb7, 0x4b, 0x50, 0x16, 0xd3, 0x4e, 0x87, 0xea } };

STDMETHODIMP IFeatureImpl::get_ID(CLSID *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = IID_CFeature;
		return S_OK;
	} CATCH_EXCEPTION("IFeatureImpl::get_ID()")
}

STDMETHODIMP IFeatureImpl::GetROI(long *count, DWORD *v) {
	try {
		if (!count)
			return E_INVALIDARG;
		if (!v || (*count < 4)) {
			*count = 4;
			return S_FALSE;
		}

		unsigned long left, top, right, bottom;
		face_feature.GetROI(&left, &top, &right, &bottom);
		v[0] = static_cast<DWORD>(left); v[1] = static_cast<DWORD>(top);
		v[2] = static_cast<DWORD>(right); v[3] = static_cast<DWORD>(bottom);
		*count = 4;
		return S_OK;
	} CATCH_EXCEPTION("IFeatureImpl::GetROI()")
}

STDMETHODIMP IFeatureImpl::ClearAttributes() {
	try {
		face_feature.classes.clear();
		return S_OK;
	} CATCH_EXCEPTION("IFeatureImpl::ClearAttributes()")
}

STDMETHODIMP IFeatureImpl::get_AttributeCount(long *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = face_feature.classes.size();
		return S_OK;
	} CATCH_EXCEPTION("IFeatureImpl::get_AttributeCount()")
}

STDMETHODIMP IFeatureImpl::GetAttribute(long index, BSTR *class_name, CLSID *classifier_id) {
	try {
		size_t index_ = static_cast<size_t>(index);
		if (index_ >= face_feature.classes.size())
			return E_INVALIDARG;

		std::map<GUID, std::wstring>::const_iterator it = face_feature.classes.begin();
		std::advance(it, index_);
		/*for (size_t i = 0; i < index_; ++i, ++it)
			continue;*/

		if (class_name)
			*class_name = ::SysAllocStringLen((*it).second.c_str(), (*it).second.size());
		if (classifier_id)
			*classifier_id = (*it).first;
		return S_OK;
	} CATCH_EXCEPTION("IFeatureImpl::GetAttribute()")
}

STDMETHODIMP IFeatureImpl::FindAttribute(wchar_t const *class_name, CLSID classifier_id, long *index) {
	try {
		if (!class_name)
			return E_INVALIDARG;

		std::map<GUID, std::wstring>::const_iterator it = face_feature.classes.find(classifier_id);
		if (it == face_feature.classes.end())
			return S_FALSE;

		if (cpcl::WStringPiece((*it).second) != cpcl::WStringPiece(class_name))
			return S_FALSE;

		if (index) {
			long i(0);
			for (std::map<GUID, std::wstring>::const_iterator head = face_feature.classes.begin(); head != it; ++head, ++i)
				continue;
			*index = i;
		}
		return S_OK;
	} CATCH_EXCEPTION("IFeatureImpl::FindAttribute()")
}

STDMETHODIMP IFeatureImpl::SetAttribute(wchar_t const *class_name, CLSID classifier_id, long *index) {
	try {
		if (!class_name) {
			std::map<GUID, std::wstring>::const_iterator it = face_feature.classes.find(classifier_id);
			if (it != face_feature.classes.end()) {
				face_feature.classes.erase(it);
				return S_OK;
			} else
				return S_FALSE;
		}

		std::wstring class_name_(class_name);
		std::map<GUID, std::wstring>::_Pairib it = face_feature.classes.insert(std::make_pair(classifier_id, class_name_));
		if (!it.second)
			(*it.first).second = class_name_;

		if (index) {
			long i(0);
			for (std::map<GUID, std::wstring>::const_iterator head = face_feature.classes.begin(); head != it.first; ++head, ++i)
				continue;
			*index = i;
		}
		return S_OK;
	} CATCH_EXCEPTION("IFeatureImpl::SetAttribute()")
}

STDMETHODIMP IFeatureImpl::GetValue(unsigned char *buffer, int *buffer_size) {
	try {
		if ((!buffer) || (!buffer_size))
			return E_INVALIDARG;

		HRESULT hr(S_FALSE);
		if (*buffer_size >= static_cast<int>(FaceFeaturesExtractor::FaceFeature::DATA_SIZE)) {
			DUMBASS_CHECK(face_feature.GetData(buffer, FaceFeaturesExtractor::FaceFeature::DATA_SIZE));
			hr = S_OK;
		}

		*buffer_size = static_cast<int>(FaceFeaturesExtractor::FaceFeature::DATA_SIZE);
		return hr;
	} CATCH_EXCEPTION("IFeatureImpl::GetValue()")
}

STDMETHODIMP IFeatureImpl::SetValue(unsigned char const *buffer, int buffer_size) {
	try {
		if ((!buffer) || (buffer_size != static_cast<int>(FaceFeaturesExtractor::FaceFeature::DATA_SIZE)))
			return E_INVALIDARG;
		
		DUMBASS_CHECK(face_feature.SetData(buffer, FaceFeaturesExtractor::FaceFeature::DATA_SIZE));
		return S_OK;
	} CATCH_EXCEPTION("IFeatureImpl::SetValue()")
}

STDMETHODIMP IFeatureImpl::Render(IPluginPage *page,
	unsigned char r, unsigned char g, unsigned char b, IRenderingDevice *output) {
	try {
		if ((!page) || (!output))
			return E_INVALIDARG;

		plcl::MemoryRenderingDevice *rendering_device_ = new plcl::MemoryRenderingDevice();
		cpcl::ComPtr<IRenderingDevice> rendering_device(new IRenderingDeviceImpl(rendering_device_)); // rendering_device_ object leak if new IRenderingDeviceImpl throw due to low memory
		THROW_FAILED(page->Render(rendering_device));
		
		face_feature.Draw(rendering_device_->Storage(), r, g, b);

		IRenderingDeviceWrapper output_(output);
		cpcl::ScopedBuf<unsigned char, 0> resample_buf;
		cpcl::ScopedBuf<unsigned char, 0> conv_buf;
		plcl::Render(plcl::RenderingDataReader(rendering_device_->Storage()),
			rendering_device_->Storage()->Width(), rendering_device_->Storage()->Height(), rendering_device_->Storage()->Pixfmt(),
			&output_, rendering_device_->Storage()->Width(), rendering_device_->Storage()->Height(), false,
			resample_buf, conv_buf);
		return S_OK;
	} CATCH_EXCEPTION("IFeatureImpl::Render()")
}

STDMETHODIMP IFeatureImpl::GetClassID(CLSID *pClassID) {
	try {
		if (!pClassID)
			return E_INVALIDARG;

		*pClassID = IID_CFeature;
		return S_OK;
	} CATCH_EXCEPTION("IFeatureImpl::GetClassID()")
}

STDMETHODIMP IFeatureImpl::IsDirty() {
	return S_FALSE;
}

#define THROW_INVALID_PARAM(x, fmt) hresult_exception::throw_formatted(hresult_exception(STG_E_READFAULT), "invalid param '" #x "': " fmt, x)

STDMETHODIMP IFeatureImpl::Load(IStream *pStm) {
	try {
		if (!pStm)
			return E_INVALIDARG;

		unsigned char buf[FaceFeaturesExtractor::FaceFeature::DATA_SIZE];
		ULONG readed;
		THROW_FAILED(pStm->Read(buf, static_cast<ULONG>(sizeof(buf)), &readed));
		if (readed != static_cast<ULONG>(sizeof(buf))) {
			hresult_exception::throw_formatted(hresult_exception(STG_E_READFAULT),
				"unable to read %u bytes",
				sizeof(buf));
		}
		DUMBASS_CHECK(face_feature.SetData(buf, sizeof(buf)));

		size_t class_count(0);
		cpcl::read_pod(pStm, &class_count);
		if (class_count > 0x100)
			THROW_INVALID_PARAM(class_count, "%u");
		cpcl::ScopedBuf<wchar_t, 0x100> buf_;
		while (class_count) {
			GUID classifier_id;
			cpcl::read_pod(pStm, &classifier_id);

			size_t class_size(0);
			cpcl::read_pod(pStm, &class_size);
			if (class_size > 0x1000)
				THROW_INVALID_PARAM(class_size, "%u");
			
			buf_.Alloc(class_size);
			class_size = class_size * sizeof(wchar_t);
			THROW_FAILED(pStm->Read(buf_.Data(), static_cast<ULONG>(class_size), &readed));
			if (readed != static_cast<ULONG>(class_size)) {
				hresult_exception::throw_formatted(hresult_exception(STG_E_READFAULT),
					"unable to read %u bytes",
					class_size);
			}
			std::wstring class_name(buf_.Data(), buf_.Size());

			face_feature.classes.insert(std::make_pair(classifier_id, class_name));

			--class_count;
		}

		return S_OK;
	} CATCH_EXCEPTION("IFeatureImpl::Load()")
}

STDMETHODIMP IFeatureImpl::Save(IStream *pStm, BOOL) {
	try {
		if (!pStm)
			return E_INVALIDARG;

		unsigned char buf[FaceFeaturesExtractor::FaceFeature::DATA_SIZE];
		DUMBASS_CHECK(face_feature.GetData(buf, sizeof(buf)));
		ULONG written;
		THROW_FAILED(pStm->Write(buf, static_cast<ULONG>(sizeof(buf)), &written));
		if (written != static_cast<ULONG>(sizeof(buf))) {
			hresult_exception::throw_formatted(hresult_exception(STG_E_WRITEFAULT),
				"unable to write %u bytes",
				sizeof(buf));
		}

		size_t class_count = face_feature.classes.size();
		cpcl::write_pod(pStm, class_count);
		for (std::map<GUID, std::wstring>::const_iterator i = face_feature.classes.begin(), tail = face_feature.classes.end(); i != tail; ++i) {
			cpcl::write_pod(pStm, (*i).first);

			size_t class_size = (*i).second.size();
			cpcl::write_pod(pStm, class_size);

			class_size = class_size * sizeof(wchar_t);
			THROW_FAILED(pStm->Write((*i).second.c_str(), static_cast<ULONG>(class_size), &written));
			if (written != static_cast<ULONG>(class_size)) {
				hresult_exception::throw_formatted(hresult_exception(STG_E_WRITEFAULT),
					"unable to write %u bytes",
					class_size);
			}
		}

		return S_OK;
	} CATCH_EXCEPTION("IFeatureImpl::Save()")
}

STDMETHODIMP IFeatureImpl::GetSizeMax(ULARGE_INTEGER *pcbSize) {
	try {
		if (!pcbSize)
			return E_INVALIDARG;

		(*pcbSize).QuadPart = FaceFeaturesExtractor::FaceFeature::DATA_SIZE;
		(*pcbSize).QuadPart += sizeof(size_t)/* class_count */;
		for (std::map<GUID, std::wstring>::const_iterator head = face_feature.classes.begin(), tail = face_feature.classes.end(); head != tail; ++head)
			(*pcbSize).QuadPart += sizeof(GUID) + sizeof(size_t)/* class_size */ + (*head).second.size() * sizeof(wchar_t);
		return S_OK;
	} CATCH_EXCEPTION("IFeatureImpl::GetSizeMax()")
}
