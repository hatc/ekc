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

#include <vector>

#include <boost/thread/mutex.hpp>

#include <com_ptr.hpp>
#include <dumbassert.h>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include "customclassfactory_list.h"

#include "impl_exception_helper.hpp"

typedef std::vector<cpcl::ComPtr<ICustomClassFactory> > FactoryList;
typedef boost::unique_lock<boost::mutex> Lock;

static FactoryList FACTORY_LIST;
static boost::mutex MUTEX;

HRESULT CustomClassFactoryList::RegisterFactory(ICustomClassFactory *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		Lock lock(MUTEX, boost::try_to_lock);
		if ((!lock) && (!lock.timed_lock(boost::posix_time::seconds(3)))) {
			cpcl::Error(cpcl::StringPieceFromLiteral("CustomClassFactoryList::RegisterFactory(): cannot obtain exclusive ownership for the current thread"));
			return E_ABORT;
		}

		FACTORY_LIST.push_back(v);
		return S_OK;
	} CATCH_EXCEPTION("CustomClassFactoryList::RegisterFactory()")
}

HRESULT CustomClassFactoryList::FindFactoryForCLSID(REFCLSID id, ICustomClassFactory **v) {
	try {
		Lock lock(MUTEX, boost::try_to_lock);
		if ((!lock) && (!lock.timed_lock(boost::posix_time::seconds(3)))) {
			cpcl::Error(cpcl::StringPieceFromLiteral("CustomClassFactoryList::FindFactoryForCLSID(): cannot obtain exclusive ownership for the current thread"));
			return E_ABORT;
		}

		cpcl::ComPtr<ICustomClassFactory> r;
		for (FactoryList::const_iterator i = FACTORY_LIST.begin(), tail = FACTORY_LIST.end(); i != tail; ++i) {
			cpcl::ComPtr<IPersistStream> tmp;
			if ((*i)->CreateInstance(id, tmp.GetAddressOf()) == S_OK) {
				DUMBASS_CHECK(!!tmp);
				r = (*i);
				break;
			}
		}
		if (!r)
			return E_NOINTERFACE;

		if (v)
			*v = r.Detach();
		return S_OK;
	} CATCH_EXCEPTION("CustomClassFactoryList::FindFactoryForCLSID()")
}

void CustomClassFactoryList::Clear() {
	Lock lock(MUTEX, boost::try_to_lock);
	if ((!lock) && (!lock.timed_lock(boost::posix_time::seconds(3)))) {
		cpcl::Error(cpcl::StringPieceFromLiteral("CustomClassFactoryList::Clear(): cannot obtain exclusive ownership for the current thread"));
	} else {
		FACTORY_LIST.clear();
	}
}
