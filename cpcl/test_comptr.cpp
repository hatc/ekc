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
#include <iostream>
#include <cassert>

#include <com_ptr.hpp>
#include <internal_com.h>

using cpcl::ComPtr;

CPCL_DECLARE_INTERFACE("5325A8D0-1970-4178-A630-6B00E1DEF1FF")
ISome : public IUnknown
{
	virtual int SomeMethod() = 0;
};

class SomeImpl : public CUnknownImp, public ISome {
public:
	SomeImpl() { std::cout << "SomeImpl::ctor(" << std::hex << (int)this << ")\n"; }
	~SomeImpl() { std::cout << "SomeImpl::dtor(" << std::hex << (int)this << ")\n"; }

	virtual int SomeMethod() { std::cout << "SomeImpl::SomeMethod()\n"; return 0; }
	CPCL_UNKNOWN_IMP1_MT(ISome)
};

int f()
{
	{
		std::vector<ComPtr<ISome> > v;
		{
			ComPtr<ISome> a = new SomeImpl();
			ComPtr<ISome> b = new SomeImpl();
			ComPtr<ISome> c = a;
			a = b = c;
			a = a;
			b = b;
			a = b;
			b = a;

			v.push_back(a);
			v.push_back(b);
			v.push_back(c);

			std::cout << ((SomeImpl*)(ISome*)a)->__m_RefCount << std::endl;
		}
		std::cout << v.back()->SomeMethod() << std::endl;
		v.clear();
	}

	return 0;
}
