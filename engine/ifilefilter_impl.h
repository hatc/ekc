// ifilefilter_impl.h
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

#ifndef __DLL_IFILE_FILTER_IMPL_H
#define __DLL_IFILE_FILTER_IMPL_H

#include <memory>

#include "plugin_filter.h"
#include "engine_interface.h"

class IFileFilterImpl : public CUnknownImp, public IFileFilter {
	std::auto_ptr<PluginFilter> plugin_filter;
	int match_method;
	DWORD combine_mode;

	IFileFilterImpl();
	IFileFilterImpl(IFileFilterImpl const&);
	void operator=(IFileFilterImpl const&);
public:
	enum FILTER_COMBINE_MODE {
		FILTER_COMBINE_AND = 1,
		FILTER_COMBINE_OR = 1 << 1,
		FILTER_COMBINE_NOT = 1 << 2,
		FILTER_COMBINE_XOR = 1 << 3
	};
	IFileFilterImpl(PluginFilter *plugin_filter_, int match_method_);
	virtual ~IFileFilterImpl();

	STDMETHOD(get_Name)(BSTR *v);
	STDMETHOD(Match)(IFileItem *item);
	STDMETHOD(get_CombineMode)(DWORD *v);
	STDMETHOD(put_CombineMode)(DWORD v);

	CPCL_UNKNOWN_IMP1_MT(IFileFilter)
};

#endif // __DLL_IFILE_FILTER_IMPL_H
