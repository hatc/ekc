// scoped_comptr.h
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

#ifndef __CPCL_SCOPED_COM_PTR_H
#define __CPCL_SCOPED_COM_PTR_H

#include <unknwn.h>

template <class T>
class ScopedComPtr /* the class name is confusing - ComPtr or AutoComPtr more likely, but auto_ptr can't be used in std containers, then the ScopedComPtr can... */
{
  T* _p;
public:
  ScopedComPtr() { _p = NULL;}
  ScopedComPtr(T* p) { if ((_p = p) != NULL) p->AddRef(); }
  ScopedComPtr(const ScopedComPtr<T>& lp)
  {
    if ((_p = lp._p) != NULL)
      _p->AddRef();
  }
  ~ScopedComPtr() { if (_p) _p->Release(); }
  void Release() { if (_p) { _p->Release(); _p = NULL; } }
  operator T*() const {  return (T*)_p;  }
  // T& operator*() const {  return *_p; }
	/* C++98/03 prohibited elements of STL containers from overloading their address-of operator.
	This is what classes like CComPtr do, so helper classes like CAdapt were required to shield the STL from such overloads 
	CopyConstructible required for store in stl containers
	http://www.rsdn.ru/forum/cpp/2773658.flat.aspx */
  T** operator&() { return &_p; } // { ASSERT(_p == NULL); return &_p; }
	/* T** operator&() should do:
	Releases the interface associated with this ComPtr and then retrieves the address of the ptr_ data member, 
	which contains a pointer to the interface that was released.
	T** ReleaseAndGetAddressOf();
	or
	T** GetAddressOf() { ASSERT(_p == NULL); return &_p; } */
  T* operator->() const { return _p; }
  T* operator=(T* p)
  {
    if (p != 0)
      p->AddRef();
    if (_p)
      _p->Release();
    _p = p;
    return p;
  }
	/* if uncomment T& operator*() const, code (*this = lp._p); become broken because *this === operator*() { return &_p; } */
	T* operator=(const ScopedComPtr<T>& lp) { return (*this = lp._p); }
  bool operator!() const { return (_p == NULL); }
  void Attach(T* p)
  {
    Release();
    _p = p;
  }
  T* Detach()
  {
    T* p = _p;
    _p = NULL;
    return p;
  }
  template <class Q>
  HRESULT QueryInterface(REFGUID iid, Q** pp) const
  {
    return _p->QueryInterface(iid, (void**)pp);
  }
};

#endif // __CPCL_SCOPED_COM_PTR_H
