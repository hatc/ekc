// CEngineManager.h
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

#ifndef __FILES_CENGINE_MANAGER_H
#define __FILES_CENGINE_MANAGER_H

#include <basic.h>

#include <internal_com.h>

#include "engine_interface.h"

class CEngineManager : public IEngine, public CUnknownImp {
  ScopedComPtr<IFileEnumerator> fileEnumerator;

	static CEngineManager *ENGINE;

	DISALLOW_IMPLICIT_CONSTRUCTORS(CEngineManager);
public:
  // ~CEngineManager();

	static HRESULT Get(IEngine **r);

	//STDMETHOD(get_FaceFeaturesExtractor)(IFaceFeaturesExtractor **v);
	STDMETHOD(LoadDetectors)(IFeatureDetectorList **detectors);
	STDMETHOD(LoadPlugins)(IPluginList **plugins);
  STDMETHOD(LoadPluginsFromPath)(BSTR file_path, IPluginList **plugins);
	STDMETHOD(get_FileEnumerator)(IFileEnumerator **v);
	STDMETHOD(Serialize)(BSTR file_path, IUnknown *v);
	STDMETHOD(Deserialize)(BSTR file_path, IUnknown **v);
	STDMETHOD(RegisterClassFactory)(ICustomClassFactory *v);
	STDMETHOD(CreateClassInstance)(REFCLSID class_id, IPersistStream **v);

	// CPCL_UNKNOWN_IMP1_MT(IEngine)
	// CEngineManager::Get() and CEngineManager::Release() must be mutual exclusive on access from different threads
	STDMETHOD(QueryInterface)(REFGUID iid, void **outObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
};

#endif // __FILES_CENGINE_MANAGER_H
