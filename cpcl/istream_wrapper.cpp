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
#include "istream_wrapper.h"

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include <Objidl.h>

#include "com_ptr.hpp"

namespace cpcl {

IStreamWrapper::IStreamWrapper(IStream *v) : ecode(S_OK) {
	if (v)
		v->AddRef();
	stream = v;
}
IStreamWrapper::IStreamWrapper(const IStreamWrapper &r) : ecode(r.ecode) {
	if (r.stream)
		r.stream->AddRef();
	stream = r.stream;
}
IStreamWrapper::operator IStream*() const {
	return stream;
}
IStream* IStreamWrapper::operator=(IStream *v) {
	if (v)
		v->AddRef();
	if (stream)
		stream->Release();
	stream = v;
	ecode = S_OK;
	return stream;
}
/*IStream* IStreamWrapper::operator=(const IStreamWrapper &r) {
	return (*this = r.stream);
}*/IStreamWrapper& IStreamWrapper::operator=(const IStreamWrapper &r) {
	*this = r.stream; // i.e. call IStream* IStreamWrapper::operator=(IStream *v) {}
	return *this;
}
IStreamWrapper::~IStreamWrapper() {
	if (stream)
		stream->Release();
}

inline bool Succeeded(long v) { return v >= 0; }
IStreamWrapper::operator bool() const {
	return Succeeded(ecode);
}

IOStream* IStreamWrapper::Clone() {
	IStreamWrapper *r(NULL);
	ComPtr<IStream> stream_;
	if (Succeeded(ecode = stream->Clone(stream_.GetAddressOf()))) { 
	// i.e. ecode = stream->Clone(stream_.GetAddressOf()); if (*this) {
		r = new IStreamWrapper(stream_);
		stream_.Detach();
	}

	return r;
}

unsigned long IStreamWrapper::Read(void *data, unsigned long size) {
	ULONG processed(0);
	ecode = stream->Read(data, static_cast<ULONG>(size), &processed);
	return static_cast<unsigned long>(processed);
}

unsigned long IStreamWrapper::Write(void const *data, unsigned long size) {
	ULONG processed(0);
	ecode = stream->Write(data, static_cast<ULONG>(size), &processed);
	return static_cast<unsigned long>(processed);
}

#ifndef INT64_MAX
static __int64 const INT64_MAX = (~(unsigned __int64)0) >> 1;
#endif
bool IStreamWrapper::Seek(__int64 move_to, unsigned long move_method, __int64 *position) {
	LARGE_INTEGER offset; offset.QuadPart = move_to;
	ULARGE_INTEGER position_;
	if (Succeeded(ecode = stream->Seek(offset, static_cast<DWORD>(move_method), &position_))) {
		if (position)
			*position = static_cast<__int64>(position_.QuadPart & INT64_MAX);
		return true;
	}
	return false;
}

__int64 IStreamWrapper::Tell() {
	LARGE_INTEGER move_to; move_to.QuadPart = 0;
	ULARGE_INTEGER position;
	return (Succeeded(ecode = stream->Seek(move_to, STREAM_SEEK_CUR, &position))) ? static_cast<__int64>(position.QuadPart & INT64_MAX) : -1;
}

__int64 IStreamWrapper::Size() {
	STATSTG statstg = { 0 };
	return (Succeeded(ecode = stream->Stat(&statstg, STATFLAG_NONAME))) ? static_cast<__int64>(statstg.cbSize.QuadPart & INT64_MAX) : -1;
}

} // namespace cpcl

/* 
not pretty sure what is was about....
T* operator=(const ScopedComPtr<T>& lp) { return (*this = lp._p); } - 
	   TEST if ptr = ptr, then at my opinion ptr myst be copied, not duplicated
		 i.e. AddRef() must be called at assign operation

		 OR IStreamWrapper(const IStreamWrapper &r); - enough for generate right assignment operator ?
		 not seem's so:
		 struct B {
			 explicit B(int v) { std::cout << "explicit B(" << v << ")\n"; }
			private:
			 B(B const&);
			 void operator=(B const&);
			};
		 struct A {
			 char v;
			 B b;
			 
			 A() : v('a'), b(10) { std::cout << "A()\n"; }
			 explicit A(char c) : v(c), b(10) { std::cout << "explicit A(char)\n"; }
			 A(A const &r) : v(r.v), b(10) { std::cout << "A(A const &)\n"; }
			 // A& operator=(A const &r) { v = r.v; return *this; } - error in compiler generated function 'A &A::operator =(const A &)'
			};

			i.e. compiler generate operator= and not use copy ctor A(A const &r)
			and it seem's a reasonable - copy ctor obviously not a assignment operator ))
*/
