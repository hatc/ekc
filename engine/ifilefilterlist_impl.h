// ifilefilterlist_impl.h
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

#ifndef __DLL_IFILE_FILTERS_LIST_IMPL_H
#define __DLL_IFILE_FILTERS_LIST_IMPL_H

#include "ilist_impl.hpp"

#include "engine_interface.h"

class IFileFilterListImpl : public IListImpl<IFileFilterList, IFileFilter>, public CUnknownImp {
public:
	IFileFilterListImpl();
	/*virtual - why virtual dtor, then dtor called through Release { delete this } i.e. no vtable dispatch???*/~IFileFilterListImpl();

	STDMETHOD(GetRange)(long index, long count, IFileFilterList **v) {
		try {
			if (!v)
				return E_INVALIDARG;

			cpcl::ComPtr<IFileFilterListImpl> r(new IFileFilterListImpl());
			RINOK(GetRangeImpl(index, count, (IFileFilterListImpl*)r))
			*v = r.Detach();
			return S_OK;
		} CATCH_EXCEPTION("IFileFilterListImpl::GetRange()")
	}

	STDMETHOD(FindItem)(BSTR filter_name, IFileFilter **v);
  STDMETHOD(Match)(IFileItem *item);

	CPCL_UNKNOWN_IMP1_MT(IFileFilterList)
};

#endif // __DLL_IFILE_FILTERS_LIST_IMPL_H
