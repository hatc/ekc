// ifeature_impl.h
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

#ifndef __DLL_IFEATURE_IMPL_H
#define __DLL_IFEATURE_IMPL_H

#include "facefeaturesextractor.h"

#include "engine_interface.h"

class IFeatureImpl : public CUnknownImp, public IFeature, public IPersistStream {
	FaceFeaturesExtractor::FaceFeature face_feature;

	// 0EF7B038-7E8B-5463-B74B-5016D34E87EA
	static GUID const IID_CFeature;
public:
	IFeatureImpl();
	IFeatureImpl(FaceFeaturesExtractor::FaceFeature face_feature_);
	~IFeatureImpl();

	STDMETHOD(get_ID)(CLSID *v);
	STDMETHOD(GetROI)(long *count, DWORD *v);
	STDMETHOD(ClearAttributes)();
	STDMETHOD(get_AttributeCount)(long *v);
	STDMETHOD(GetAttribute)(long index, BSTR *class_name, CLSID *classifier_id);
	STDMETHOD(FindAttribute)(wchar_t const *class_name, CLSID classifier_id, long *index);
	STDMETHOD(SetAttribute)(wchar_t const *class_name, CLSID classifier_id, long *index);
  STDMETHOD(GetValue)(unsigned char *buffer, int *buffer_size);
  STDMETHOD(SetValue)(unsigned char const *buffer, int buffer_size);
	STDMETHOD(Render)(IPluginPage *page,
		unsigned char r, unsigned char g, unsigned char b, IRenderingDevice *output);

	/* IPersist */
	STDMETHOD(GetClassID)(CLSID *pClassID);

	/* IPersistStream */
	STDMETHOD(IsDirty)();
	STDMETHOD(Load)(IStream *pStm);
	STDMETHOD(Save)(IStream *pStm, BOOL fClearDirty);
	STDMETHOD(GetSizeMax)(ULARGE_INTEGER *pcbSize);

	CPCL_UNKNOWN_IMP2_MT(IFeature, IPersistStream)
};

#endif // __DLL_IFEATURE_IMPL_H
