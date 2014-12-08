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

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include "ifilelist_impl.h"

#include <impl_exception_helper.hpp>

IFileListImpl::IFileListImpl()
{}
IFileListImpl::~IFileListImpl()
{}

// F4A1A7A9-AC1B-492a-82C4-D31259E08CCE
GUID const IFileListImpl::IID_CFileList = { 0xf4a1a7a9, 0xac1b, 0x492a, { 0x82, 0xc4, 0xd3, 0x12, 0x59, 0xe0, 0x8c, 0xce } };

STDMETHODIMP IFileListImpl::GetClassID(CLSID *pClassID) {
	try {
		if (!pClassID)
			return E_INVALIDARG;

		*pClassID = IID_CFileList;
		return S_OK;
	} CATCH_EXCEPTION("IFileListImpl::GetClassID()")
}

//STDMETHODIMP IFileListImpl::IsDirty() {
//	return S_FALSE;
//}
//
//STDMETHODIMP IFileListImpl::Load(IStream *pStm) {
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
//			cpcl::ComPtr<IFileItem> file_item;
//			THROW_FAILED(item->QueryInterface(file_item.GetAddressOf()));
//
//			THROW_FAILED(item->Load(pStm));
//
//			THROW_FAILED(AddItem(file_item, 0));
//
//			--items_n;
//		}
//
//		return S_OK;
//	} CATCH_EXCEPTION("IFileListImpl::Load()")
//}
//
//STDMETHODIMP IFileListImpl::Save(IStream *pStm, BOOL) {
//	try {
//		if (!pStm)
//			return E_INVALIDARG;
//
//		std::vector<cpcl::ComPtr<IPersistStream> > items;
//		items.reserve(container.size());
//		for (size_t i = 0; i < container.size(); ++i) {
//			cpcl::ComPtr<IPersistStream> item;
//			if (container[i]->QueryInterface(item.GetAddressOf()) == S_OK)
//				items.push_back(item);
//		}
//
//		size_t items_n = items.size();
//		THROW_FAILED(cpcl::write_pod(pStm, items_n));
//
//		for (size_t i = 0; i < items_n; ++i) {
//			GUID id;
//			THROW_FAILED(items[i]->GetClassID(&id));
//			THROW_FAILED(cpcl::write_pod(pStm, id));
//			
//			THROW_FAILED(items[i]->Save(pStm, TRUE));
//		}
//
//		//size_t items_n = container.size();
//		//THROW_FAILED(cpcl::write_pod(pStm, items_n));
//
//		//for (size_t i = 0; i < items_n; ++i) {
//		//	cpcl::ComPtr<IPersistStream> item;
//		//	// if (container[i]->QueryInterface(item.GetAddressOf()) == S_OK) {
//		//	THROW_FAILED(container[i]->QueryInterface(item.GetAddressOf())); // if item not IPersistStream, then items_n invalid
//		//		GUID id;
//		//		THROW_FAILED(item->GetClassID(&id));
//		//		THROW_FAILED(cpcl::write_pod(pStm, id));
//
//		//		THROW_FAILED(item->Save(pStm, TRUE));
//		//	// }
//		//}
//
//		return S_OK;
//	} CATCH_EXCEPTION("IFileListImpl::Save()")
//}
//
//STDMETHODIMP IFileListImpl::GetSizeMax(ULARGE_INTEGER *pcbSize) {
//	try {
//		if (!pcbSize)
//			return E_INVALIDARG;
//
//		unsigned __int64 size = sizeof(size_t);
//		for (size_t i = 0, items_n = container.size(); i < items_n; ++i) {
//			cpcl::ComPtr<IPersistStream> item;
//			if (container[i]->QueryInterface(item.GetAddressOf()) == S_OK) {
//			// THROW_FAILED(container[i]->QueryInterface(item.GetAddressOf())); // if item not IPersistStream, then items_n in Save() become invalid
//				ULARGE_INTEGER size_;
//				size_.QuadPart = 0;
//				THROW_FAILED(item->GetSizeMax(&size_));
//
//				size += size_.QuadPart;
//			}
//		}
//
//		(*pcbSize).QuadPart = size;
//		return S_OK;
//	} CATCH_EXCEPTION("IFileListImpl::GetSizeMax()")
//}
