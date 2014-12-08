// ipersistlist_impl.hpp
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
#pragma once

#include "ilist_impl.hpp"
#include "customclassfactory_list.h"
#include "istream_util.hpp"

/* methods the same for IFileListImpl, IFaceFeatureListImpl - can extract in base template class
template<>
IPersistListImpl : IListImpl<> {
IsDirty
Load
Save
GetSizeMax
}

question is - what do with CATCH_EXCEPTION("<ClassName>::GetClassID()") ??? nothing to do
templates - it's compile time feature, CATCH_EXCEPTION() - macro preprocessor
so, when preprocessor expand macro, type information is not yet not avaliable*/

template<class List, class Item>
class IPersistListImpl : public IListImpl<List, Item>, public IPersistStream {
public:
	STDMETHOD(IsDirty)() {
		return S_FALSE;
	}
	
	STDMETHOD(Load)(IStream *pStm) {
		try {
			if (!pStm)
				return E_INVALIDARG;
			
			size_t items_n;
			THROW_FAILED(cpcl::read_pod(pStm, &items_n));
			container.reserve(container.size() + items_n);
			
			cpcl::ComPtr<ICustomClassFactory> class_factory;
			while (items_n > 0) {
				GUID id;
				THROW_FAILED(cpcl::read_pod(pStm, &id));
				
				cpcl::ComPtr<IPersistStream> item;
				if (!!class_factory) {
					if (class_factory->CreateInstance(id, item.GetAddressOf()) != S_OK)
						class_factory.Release();
				}
				if (!class_factory) {
					THROW_FAILED(CustomClassFactoryList::FindFactoryForCLSID(id, class_factory.GetAddressOf()));
					HRESULT hr = class_factory->CreateInstance(id, item.ReleaseAndGetAddressOf());
					if (hr != S_OK) {
						if FAILED(hr) {
							hresult_exception::throw_formatted(hresult_exception(hr),
								"class_factory->CreateInstance(id, item) fails with code: 0x%08X",
								hr);
						} else {
							hresult_exception::throw_formatted(hresult_exception(E_FAIL),
								"unable to create a item instance, class_factory->CreateInstance(id, item) return code: 0x%08X",
								hr);
						}
					}
				}
				
				cpcl::ComPtr<Item> container_item;
				THROW_FAILED(item->QueryInterface(container_item.GetAddressOf()));
				
				THROW_FAILED(item->Load(pStm));
				
				THROW_FAILED(AddItem(container_item, 0));
				
				--items_n;
			}
			
			return S_OK;
		} CATCH_EXCEPTION("IPersistListImpl::Load()")
	}
	
	STDMETHOD(Save)(IStream *pStm, BOOL) {
		try {
			if (!pStm)
				return E_INVALIDARG;
			
			std::vector<cpcl::ComPtr<IPersistStream> > items;
			items.reserve(container.size());
			for (size_t i = 0; i < container.size(); ++i) {
				cpcl::ComPtr<IPersistStream> item;
				if (container[i]->QueryInterface(item.GetAddressOf()) == S_OK)
					items.push_back(item);
			}
			
			size_t items_n = items.size();
			THROW_FAILED(cpcl::write_pod(pStm, items_n));
			
			for (size_t i = 0; i < items_n; ++i) {
				GUID id;
				THROW_FAILED(items[i]->GetClassID(&id));
				THROW_FAILED(cpcl::write_pod(pStm, id));
				
				THROW_FAILED(items[i]->Save(pStm, TRUE));
			}
			
			return S_OK;
		} CATCH_EXCEPTION("IPersistListImpl::Save()")
	}
	
	STDMETHOD(GetSizeMax)(ULARGE_INTEGER *pcbSize) {
		try {
			if (!pcbSize)
				return E_INVALIDARG;
			
			unsigned __int64 size = sizeof(size_t); // cpcl::write_pod(pStm, items_n)
			for (size_t i = 0, items_n = container.size(); i < items_n; ++i) {
				cpcl::ComPtr<IPersistStream> item;
				if (container[i]->QueryInterface(item.GetAddressOf()) == S_OK) {
					ULARGE_INTEGER size_;
					size_.QuadPart = 0;
					THROW_FAILED(item->GetSizeMax(&size_));
					
					size += size_.QuadPart + sizeof(GUID); // cpcl::write_pod(pStm, id)
				}
			}
			
			(*pcbSize).QuadPart = size;
			return S_OK;
		} CATCH_EXCEPTION("IPersistListImpl::GetSizeMax()")
	}
};
