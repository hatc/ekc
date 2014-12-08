// com_ptr.h
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

#ifndef __CPCL_COM_PTR_H
#define __CPCL_COM_PTR_H

namespace cpcl {

template <class T>
class ComPtr
{
  T* p;
public:
  ComPtr() { p = NULL;}
  ComPtr(T* p_) { if ((p = p_) != NULL) p->AddRef(); }
	ComPtr(ComPtr<T> const &r) { if ((p = r.p) != NULL) p->AddRef(); }
  ~ComPtr() { if (p) p->Release(); }
  void Release() { if (p) { p->Release(); p = NULL; } }
  operator T*() const {  return (T*)p;  }
	/* C++98/03 prohibited elements of STL containers from overloading their address-of operator.
	This is what classes like CComPtr do, so helper classes like CAdapt were required to shield the STL from such overloads 
	CopyConstructible required for store in stl containers
	http://www.rsdn.ru/forum/cpp/2773658.flat.aspx */
	T** ReleaseAndGetAddressOf() { Release(); return &p; }
	T** GetAddressOf() { /*ASSERT(p == NULL);*/ return &p; }
  T* operator->() const { return p; }
  T* operator=(T *p_)
  {
    if (p_)
      p_->AddRef();
    if (p)
      p->Release();
    return (p = p_);
  }
	T* operator=(ComPtr<T> const &r) { return (*this = r.p); }
	/*ComPtr<T>& operator=(ComPtr<T> const &r) { *this = r.p; return (*this); }*/
  bool operator!() const { return (p == NULL); }
  void Attach(T *p_) { Release(); p = p_; }
  T* Detach()
  {
    T *p_ = p;
    p = NULL;
    return p_;
  }
};

} // namespace cpcl

#endif // __CPCL_COM_PTR_H
