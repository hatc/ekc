// ifeaturelist_impl.h
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

#ifndef __DLL_IFEATURE_LIST_IMPL_H
#define __DLL_IFEATURE_LIST_IMPL_H

#include "ipersistlist_impl.hpp"

// #include "engine_interface.h"
#include "ifeature_impl.h"

class IFeatureListImpl : public IPersistListImpl<IFeatureList, IFeature>, public CUnknownImp {
	// 7656F6C9-C7D7-5463-95A8-13FB8A453120
	static GUID const IID_CFeatureList;
public:
	IFeatureListImpl();
	template<class InputIterator>
	IFeatureListImpl(InputIterator begin, InputIterator end) {
		for (; begin != end; ++begin) {
			cpcl::ComPtr<IFeature> item(new IFeatureImpl(*begin));
			container.push_back(item);
		}
	}
	~IFeatureListImpl();

	STDMETHOD(GetRange)(long index, long count, IFeatureList **v) {
		try {
			if (!v)
				return E_INVALIDARG;

			cpcl::ComPtr<IFeatureListImpl> r(new IFeatureListImpl());
			RINOK(GetRangeImpl(index, count, (IFeatureListImpl*)r))
			*v = r.Detach();
			return S_OK;
		} CATCH_EXCEPTION("IFeatureListImpl::GetRange()")
	}

	STDMETHOD(Render)(IPluginPage *page,
		unsigned char r, unsigned char g, unsigned char b, IRenderingDevice *output);

	/* IPersist */
	STDMETHOD(GetClassID)(CLSID *pClassID);

	///* IPersistStream */
	//STDMETHOD(IsDirty)();
	//STDMETHOD(Load)(IStream *pStm);
	//STDMETHOD(Save)(IStream *pStm, BOOL fClearDirty);
	//STDMETHOD(GetSizeMax)(ULARGE_INTEGER *pcbSize);

	CPCL_UNKNOWN_IMP2_MT(IFeatureList, IPersistStream)
};

#endif // __DLL_IFEATURE_LIST_IMPL_H
