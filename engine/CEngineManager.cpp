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

//#include "cas_lock.h"
#include <dumbassert.h>

#include <boost/thread/recursive_mutex.hpp>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include "plugin_list.h"
#include "ipluginlist_impl.h"

#include <impl_exception_helper.hpp>

#include "CEngineManager.h"
#include "simpleFileEnumerator.h"
#include "ifeaturedetectorlist_impl.h"

#include "customclassfactory_list.h"
#include "icustomclassfactory_impl.h"
#include "istream_util.hpp"

#include "ifeaturelist_impl.h"
#include "ifeature_impl.h"
#include "ifilelist_impl.h"
#include "simpleFileItems.h"

#include "files/sharedIconMap.h"
#include "files/sharedClient7z.h"
#include "files/temporaryStream.h"

static boost::recursive_mutex MUTEX; // boost::win32_mutex very similar to CS(CriticalSection) - also first use try-lock in user ring, and if failed go to kernel wait through WaitForObject
CEngineManager* CEngineManager::ENGINE = 0;

CEngineManager::CEngineManager()
{}
//CEngineManager::~CEngineManager() {
//	CASLock lock;
//	DUMBASS_CHECK(lock.owns_lock); // [!!!] CASLock must be on Release() call, before dtor called, because if lock not acquired in dtor, then another thread can get pointer to deleted object
//	// CEngineManager::Release() { lock; ENGINE = 0; delete this; }
//
//	CEngineManager::ENGINE = 0;
//	
//	CustomClassFactoryList::Clear();
//	files::releaseSharedIconMap();
//	files::releaseSharedClient7z();
//}

//CEngineManager::~CEngineManager() {
//	/*
//	gotcha
//	unload libraries first, dtor for object called second - app_crash
//	
//	~CEngineManager() {
//		for (libs_i i=libs.begin(); i!=libs.end(); ++i) {
//			delete (*i); (*i) = NULL;
//		}
//	}
//  ...
//  CMyComPtr<IFileEnumerator>::dtor - fileEnumerator->Release() ??? nani release, module already unloaded ???
//  CMyComPtr<IDocumentManager>::dtor - documentManager->Release() ??? nani release, module already unloaded ???
//  CMyComPtr<IFeaturesExtractor>::dtor - featureExtractor->Release() ??? nani release, module already unloaded ???
//  CMyComPtr<ITextLayoutRetriever>::dtor - textLayoutRetriever->Release() ??? nani release, module already unloaded ???
//  */
//	//textLayoutRetriever = NULL;
//	//fileEnumerator = NULL;
//	//documentManager = NULL;
//	//featureExtractor = NULL;
//
//	/*void *__unused;
//	_fileEnumeratorEntry(3, &__unused);*/
//	
//	//for (libs_i i=libs.begin(); i!=libs.end(); ++i) {
//	//	delete (*i); (*i) = NULL;
//	//}
//}

template<class T>
/*inline - use arrays of function pointer, that the inline ??? */ IPersistStream* ctor_impl() {
	return new T();
}

static ICustomClassFactoryImpl::ctor_instance CTORS[] = {
	ctor_impl<IFileListImpl>,
	ctor_impl<simpleFileItem>,
	ctor_impl<IFeatureListImpl>,
	ctor_impl<IFeatureImpl>
};

inline void CreateShared() {
	THROW_FAILED(files::createSharedClient7z());
	files::createSharedIconMap();
	
	cpcl::ComPtr<ICustomClassFactory> class_factory;
	THROW_FAILED(ICustomClassFactoryImpl::Create(CTORS, arraysize(CTORS), class_factory.GetAddressOf()));
	THROW_FAILED(CustomClassFactoryList::RegisterFactory(class_factory));
}

inline void DeleteShared() {
	CustomClassFactoryList::Clear();
	files::releaseSharedIconMap();
	files::releaseSharedClient7z();
}

STDMETHODIMP CEngineManager::QueryInterface(REFGUID iid, void **outObject) {
	if (__uuidof(IUnknown) == iid) {
		*outObject = (void *)(IUnknown *)(IEngine *)this;
		AddRef();
		return S_OK;
	}
	if (__uuidof(IEngine) == iid) {
		*outObject = (void *)(IEngine *)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

/* GetEngineObject ~= CEngineManager::AddRef(), but support cases when refs == 0 - create new CEngineManager
	so, GetEngineObject and CEngineManager::Release() must be mutual exclusive on access from different threads
	probaly in AddRef() lock not needed, because not change CEngineManager::ENGINE

	needed SpinLock, or CS on windows - CS is recursive mutex, so we can use it an AddRef(), and call from locked section GetEngineObject(),
	in that case(if use CS on AddRef(), Release()) no need for InterlockedIncrement / InterlockedDecrement
	CEngineManager::Release() {
	 lock;
	 if InterlockedDecrement(refs) == 0 
	  release_shared_objects;
	  delete this;
	}*/
ULONG STDMETHODCALLTYPE CEngineManager::AddRef() {
	boost::recursive_mutex::scoped_lock lock(MUTEX);

	return ++__m_RefCount;
}

ULONG STDMETHODCALLTYPE CEngineManager::Release() {
	boost::recursive_mutex::scoped_lock lock(MUTEX);

	DUMBASS_CHECK(__m_RefCount > 0);
	if (--__m_RefCount != 0)
		return __m_RefCount;

	CEngineManager::ENGINE = 0;
	DeleteShared();
	delete this;
	return 0;
}

HRESULT CEngineManager::Get(IEngine **r) {
	try {
		boost::recursive_mutex::scoped_lock lock(MUTEX);

		//THROW_FAILED(files::createSharedClient7z());
		//files::createSharedIconMap();

		//cpcl::ComPtr<ICustomClassFactory> class_factory;
		//THROW_FAILED(ICustomClassFactoryImpl::Create(CTORS, arraysize(CTORS), class_factory.GetAddressOf()));
		//THROW_FAILED(CustomClassFactoryList::RegisterFactory(class_factory));

		//// if (!fileEnumerator) {
		//fileEnumerator = new simpleFileEnumerator();
		///*if (!featureExtractor) {
		//RINOK(_faceFeatureEntry(1, (void**)&featureExtractor))
		//}*/

		cpcl::ComPtr<CEngineManager> engine(CEngineManager::ENGINE);
		if (!engine) {
			engine = new CEngineManager();
			CreateShared();
			CEngineManager::ENGINE = engine;
			
			engine->fileEnumerator = new simpleFileEnumerator();
			// IFaceFeaturesExtractorImpl::Create(&engine->facefeaturesextractor); // ignore errors - for example module cffindfaces.dll not found
		}

		if (r) {
			THROW_FAILED(engine->QueryInterface(__uuidof(IEngine), (void**)r));
		}
		return S_OK;
	} CATCH_EXCEPTION("CEngineManager::Get()")
}

//STDMETHODIMP CEngineManager::get_FaceFeaturesExtractor(IFaceFeaturesExtractor **v) {
//	try {
//		if (!v)
//			return E_INVALIDARG;
//		
//		cpcl::ComPtr<IFaceFeaturesExtractor> r(facefeaturesextractor);
//		if (!r)
//			return E_NOTIMPL;
//
//		*v = r.Detach();
//		return S_OK;
//	} CATCH_EXCEPTION("CEngineManager::get_FaceFeaturesExtractor()")
//}
STDMETHODIMP CEngineManager::LoadDetectors(IFeatureDetectorList **detectors) {
	try {
		if (!detectors)
			return E_INVALIDARG;
		
		return IFeatureDetectorListImpl::Create(detectors);
	} CATCH_EXCEPTION("CEngineManager::LoadDetectors()")
}

STDMETHODIMP CEngineManager::LoadPlugins(IPluginList **plugins) {
	try {
		if (!plugins)
			return E_POINTER;

		PluginList *r_ = PluginList::LoadPlugins();
		ScopedComPtr<IPluginList> r = new IPluginListImpl(r_); // r_ object leak if new IPluginListImpl throw due to low memory
		*plugins = r.Detach();
		return S_OK;
	} CATCH_EXCEPTION("CEngineManager::LoadPlugins()")
}

STDMETHODIMP CEngineManager::LoadPluginsFromPath(BSTR file_path, IPluginList **plugins) {
	try {
		if (!plugins)
			return E_POINTER;

		PluginList *r_ = PluginList::LoadPlugins(cpcl::WStringPiece((OLECHAR*)file_path, ::SysStringLen(file_path)));
		ScopedComPtr<IPluginList> r = new IPluginListImpl(r_); // r_ object leak if new IPluginListImpl throw due to low memory
		*plugins = r.Detach();
		return S_OK;
	} CATCH_EXCEPTION("CEngineManager::LoadPluginsFromPath()")
}

STDMETHODIMP CEngineManager::get_FileEnumerator(IFileEnumerator **v) {
	try {
		if (!v)
			return E_POINTER;

		ScopedComPtr<IFileEnumerator> r(fileEnumerator);
		*v = r.Detach();
		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}

STDMETHODIMP CEngineManager::Serialize(BSTR path, IUnknown *v) {
	try {
		if (!v)
			return E_POINTER;

		ScopedComPtr<IPersistStream> persistStream;
		// RINOK(v->QueryInterface(IID_IPersistStream, (void**)&persistStream))
		RINOK(v->QueryInterface(&persistStream))

		ScopedComPtr<IStream> stream;
		// RINOK(fileUtility::fileWrite((const wchar_t*)path, &stream))
		RINOK(files::fileStreamCreate((const wchar_t*)path, &stream))

		{
			CLSID classID;
			RINOK(persistStream->GetClassID(&classID))
			RINOK(cpcl::write_pod(stream, classID))
		}
		return persistStream->Save(stream, TRUE);
	} catch (...) {
		return E_FAIL;
	}
}

STDMETHODIMP CEngineManager::Deserialize(BSTR path, IUnknown **v) {
	try {
		if (!v)
			return E_POINTER;
		*v = 0;

    ScopedComPtr<IStream> stream;
		// RINOK(fileUtility::fileRead((const wchar_t*)path, &stream))
    RINOK(files::fileStreamRead((const wchar_t*)path, &stream))

		CLSID classID;
		RINOK(cpcl::read_pod(stream, &classID))

		ScopedComPtr<ICustomClassFactory> classFactory;
		RINOK(CustomClassFactoryList::FindFactoryForCLSID(classID, &classFactory))

		ScopedComPtr<IPersistStream> persistStream;
		RINOK(classFactory->CreateInstance(classID, &persistStream))

		RINOK(persistStream->Load(stream))

		*v = persistStream.Detach();
		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}

STDMETHODIMP CEngineManager::RegisterClassFactory(ICustomClassFactory *v) {
	try {
		return CustomClassFactoryList::RegisterFactory(v);
	} catch (...) {
		return E_FAIL;
	}
}

STDMETHODIMP CEngineManager::CreateClassInstance(REFCLSID classID, IPersistStream **v) {
	try {
		ScopedComPtr<ICustomClassFactory> classFactory;
		RINOK(CustomClassFactoryList::FindFactoryForCLSID(classID, &classFactory))
		return classFactory->CreateInstance(classID, v);
	} catch (...) {
		return E_FAIL;
	}
}
