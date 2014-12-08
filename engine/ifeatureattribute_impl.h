// ifeatureattribute_impl.h
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

#ifndef __DLL_IFEATURE_ATTRIBUTE_IMPL_H
#define __DLL_IFEATURE_ATTRIBUTE_IMPL_H

#include "facefeaturesextractor.h"

#include "engine_interface.h"

class IFeatureAttributeImpl : public CUnknownImp, public IFeatureAttribute {
	std::auto_ptr<FaceFeaturesExtractor::AttributeFilter> attribute_filter;

	DISALLOW_IMPLICIT_CONSTRUCTORS(IFeatureAttributeImpl);
public:
	explicit IFeatureAttributeImpl(std::auto_ptr<FaceFeaturesExtractor::AttributeFilter> &attribute_filter_);
	~IFeatureAttributeImpl();

	STDMETHOD(get_ID)(CLSID *v);
	STDMETHOD(get_Name)(BSTR *v);
	STDMETHOD(get_ClassCount)(long *v);
	STDMETHOD(GetClass)(long index, BSTR *class_name);
  STDMETHOD(GetValue)(unsigned char *buffer, int *buffer_size);
  STDMETHOD(SetValue)(unsigned char const *buffer, int buffer_size);
	STDMETHOD(Classify)(IPluginPage *page, IFeature *feature, long *index);

	CPCL_UNKNOWN_IMP1_MT(IFeatureAttribute)
};

#endif // __DLL_IFEATURE_ATTRIBUTE_IMPL_H
