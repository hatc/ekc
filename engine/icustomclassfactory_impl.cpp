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

#include <com_ptr.hpp>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include "icustomclassfactory_impl.h"

#include <impl_exception_helper.hpp>

ICustomClassFactoryImpl::ICustomClassFactoryImpl()
{}
ICustomClassFactoryImpl::~ICustomClassFactoryImpl()
{}

STDMETHODIMP ICustomClassFactoryImpl::CreateInstance(REFCLSID class_id, IPersistStream **v) {
	try {
		Ctors::const_iterator i = ctors.find(class_id);
		if (i == ctors.end())
			return E_NOINTERFACE;

		cpcl::ComPtr<IPersistStream> r = (*i).second();

		if (v)
			*v = r.Detach();
		return S_OK;
	} CATCH_EXCEPTION("ICustomClassFactoryImpl::CreateInstance()")
}

HRESULT ICustomClassFactoryImpl::Create(ctor_instance *ctors, size_t ctors_n, ICustomClassFactory **v) {
	try {
		if ((!ctors) || (ctors_n < 1))
			return E_INVALIDARG;

		cpcl::ComPtr<ICustomClassFactoryImpl> r = new ICustomClassFactoryImpl();
		for (size_t i = 0; i < ctors_n; ++i) {
			cpcl::ComPtr<IPersistStream> tmp = ctors[i]();
			IID id;
			THROW_FAILED(tmp->GetClassID(&id));

			Ctors::_Pairib p = r->ctors.insert(std::make_pair(id, ctors[i]));
			if (!p.second)
				cpcl::Warning(cpcl::StringPieceFromLiteral("ICustomClassFactoryImpl::Create(): duplicate IID for ctor_instance"));
		}

		return ((IUnknown*)r)->QueryInterface(v);
	} CATCH_EXCEPTION("ICustomClassFactoryImpl::Create()")
}
