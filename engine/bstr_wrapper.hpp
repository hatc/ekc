// bstr_wrapper.hpp
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

#include <string_piece.hpp>

#include <windows.h> // actually BSTR API declared in <OleAuto.h>, but that header can't be used alone

namespace cpcl {

/* CopyConstructible, Assignable, DefaultConstructible */
struct BSTRWrapper {
	BSTR v;

	BSTRWrapper() : v(0)
	{}

	BSTRWrapper(WStringPiece const &s) {
		// if s.empty() - allocate empty BSTR or SysAllocStringLen returns NULL ???
		v = ::SysAllocStringLen(s.data(), s.size());
	}

	/*!!! - unsafe argument type - BSTR or just wchar_t* ? BSTRWrapper(BSTR s) : v(0) {
	}*/

	// if (len > 0) - empty BSTR != NULL
	BSTRWrapper(BSTRWrapper const &r) {
		UINT const len = ::SysStringLen(r.v);
		if (len > 0) // if (r.v) - alloc empty BSTR
			v = ::SysAllocStringLen(r.v, len);
	}

	BSTRWrapper& operator=(BSTRWrapper const &r) {
		this->Release();

		UINT const len = ::SysStringLen(r.v);
		if (len > 0)
			v = ::SysAllocStringLen(r.v, len);
	}

	operator BSTR() const {
		return v;
	}
	operator WStringPiece() const {
		return WStringPiece(v, ::SysStringLen(v));
	}

	void Release() {
		if (v) {
			::SysFreeString(v);
			v = 0;
		}
	}

	BSTR* ReleaseAndGetAddressOf() {
		// Release();
		this->Release();
		return &v;
	}

	~BSTRWrapper() {
		this->Release();
		/*if (v) {
			::SysFreeString(v);
			v = 0;
		}*/
	}
};

inline bool operator<(BSTRWrapper const &l, BSTRWrapper const &r) {
	return (WStringPiece)l < (WStringPiece)r;
}
inline bool operator>(BSTRWrapper const &l, BSTRWrapper const &r) {
	return r < l;
}
inline bool operator<=(BSTRWrapper const &l, BSTRWrapper const &r) {
  return !(l > r);
}
inline bool operator>=(BSTRWrapper const &l, BSTRWrapper const &r) {
  return !(l < r);
}
inline bool operator==(BSTRWrapper const &l, BSTRWrapper const &r) {
	return (WStringPiece)l == (WStringPiece)r;
}
inline bool operator!=(BSTRWrapper const &l, BSTRWrapper const &r) {
	return !(l == r);
}

inline BSTRWrapper BSTRCopy(BSTR s) {
	BSTRWrapper r;

	UINT const len = ::SysStringLen(s);
	if (len > 0)
		r.v = ::SysAllocStringLen(s, len);

	return r;
}

/*operators prefer to declare in namespaces:

struct A { int v; };

inline bool operator<(cpcl::A const &a, cpcl::A const &b) {
	return a.v < b.v;
}
}

if operator declared in global namespace, then lookup failed for operator resoultion in any(?) namespace - below, if operator< in global
then lookup failed for cpcl::f and std::f
inline bool operator<(cpcl::A const &a, cpcl::A const &b) {
	return a.v < b.v;
}

namespace cpcl {
	bool f(A a, A b) {
		// return a < b;
		BSTRWrapper s, s_;
		return s < s_;
	}
}

bool f(cpcl::A a, cpcl::A b) {
	return a < b;
}

namespace std {
	bool f(cpcl::A a, cpcl::A b) {
		return a < b;
	}
}
*/

}
