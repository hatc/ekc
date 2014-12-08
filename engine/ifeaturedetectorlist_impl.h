// ifeaturedetectorlist_impl.h
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
#pragma once

#ifndef __DLL_IFEATURE_DETECTOR_LIST_IMPL_H
#define __DLL_IFEATURE_DETECTOR_LIST_IMPL_H

#include "ilist_impl.hpp"

#include "engine_interface.h"

class IFeatureDetectorListImpl : public IListImpl<IFeatureDetectorList, IFeatureDetector>, public CUnknownImp {
public:
	IFeatureDetectorListImpl();
	/*virtual - why virtual dtor, then dtor called through Release { delete this } i.e. no vtable dispatch???*/~IFeatureDetectorListImpl();

	STDMETHOD(GetRange)(long index, long count, IFeatureDetectorList **v) {
		try {
			if (!v)
				return E_INVALIDARG;

			cpcl::ComPtr<IFeatureDetectorListImpl> r(new IFeatureDetectorListImpl());
			RINOK(GetRangeImpl(index, count, (IFeatureDetectorListImpl*)r))
			*v = r.Detach();
			return S_OK;
		} CATCH_EXCEPTION("IFeatureDetectorListImpl::GetRange()")
	}

	STDMETHOD(FindItem)(BSTR detector_name, IFeatureDetector **v);

	static HRESULT Create(IFeatureDetectorList **v);

	CPCL_UNKNOWN_IMP1_MT(IFeatureDetectorList)
};

#endif // __DLL_IFEATURE_DETECTOR_LIST_IMPL_H
