// internal_com.h
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

#ifndef __CPCL_INTERNAL_COM_H
#define __CPCL_INTERNAL_COM_H

#include "scoped_comptr.h"

#ifndef RINOK
#define RINOK(x) { HRESULT __result_ = (x); if (__result_ != S_OK) return __result_; }
#endif

#define CPCL_DECLARE_INTERFACE(guid) struct __declspec(uuid(guid)) __declspec(novtable)

class CUnknownImp
{
public:
  ULONG __m_RefCount;
  CUnknownImp(): __m_RefCount(0) {}
};

// why { if (iid == __uuidof(i)) ... } work ???
//
//#include <Guiddef.h>
//...
//#ifdef __cplusplus
//__inline int operator==(REFGUID guidOne, REFGUID guidOther)
//{
//    return IsEqualGUID(guidOne,guidOther);
//}
//
//__inline int operator!=(REFGUID guidOne, REFGUID guidOther)
//{
//    return !(guidOne == guidOther);
//}
//#endif

#define CPCL_QUERYINTERFACE_BEGIN STDMETHOD(QueryInterface) \
    (REFGUID iid, void **outObject) {

#define CPCL_QUERYINTERFACE_ENTRY(i) if (iid == __uuidof(i)) \
    { *outObject = (void *)(i *)this; AddRef(); return S_OK; }

#define CPCL_QUERYINTERFACE_ENTRY_UNKNOWN(i) if (iid == __uuidof(IUnknown)) \
    { *outObject = (void *)(IUnknown *)(i *)this; AddRef(); return S_OK; }

#define CPCL_QUERYINTERFACE_BEGIN2(i) CPCL_QUERYINTERFACE_BEGIN \
    CPCL_QUERYINTERFACE_ENTRY_UNKNOWN(i) \
    CPCL_QUERYINTERFACE_ENTRY(i)

#define CPCL_QUERYINTERFACE_END return E_NOINTERFACE; }

#define CPCL_ADDREF_RELEASE_MT \
STDMETHOD_(ULONG, AddRef)() { InterlockedIncrement((LONG *)&__m_RefCount); return __m_RefCount; } \
	STDMETHOD_(ULONG, Release)() { InterlockedDecrement((LONG *)&__m_RefCount); if (__m_RefCount != 0) \
  return __m_RefCount; delete this; return 0; }

#define CPCL_UNKNOWN_IMP_SPEC_MT(i) \
  CPCL_QUERYINTERFACE_BEGIN \
  CPCL_QUERYINTERFACE_ENTRY(IUnknown) \
  i \
  CPCL_QUERYINTERFACE_END \
  CPCL_ADDREF_RELEASE_MT

#define CPCL_UNKNOWN_IMP_SPEC_MT2(i1, i) \
  CPCL_QUERYINTERFACE_BEGIN \
  if (iid == __uuidof(IUnknown)) \
      { *outObject = (void *)(IUnknown *)(i1 *)this; AddRef(); return S_OK; }  i \
  CPCL_QUERYINTERFACE_END \
  CPCL_ADDREF_RELEASE_MT

#define CPCL_UNKNOWN_IMP_MT CPCL_UNKNOWN_IMP_SPEC_MT(;)

#define CPCL_UNKNOWN_IMP1_MT(i) CPCL_UNKNOWN_IMP_SPEC_MT2( \
  i, \
  CPCL_QUERYINTERFACE_ENTRY(i) \
  )

#define CPCL_UNKNOWN_IMP2_MT(i1, i2) CPCL_UNKNOWN_IMP_SPEC_MT2( \
  i1, \
  CPCL_QUERYINTERFACE_ENTRY(i1) \
  CPCL_QUERYINTERFACE_ENTRY(i2) \
  )

#define CPCL_UNKNOWN_IMP3_MT(i1, i2, i3) CPCL_UNKNOWN_IMP_SPEC_MT2( \
  i1, \
  CPCL_QUERYINTERFACE_ENTRY(i1) \
  CPCL_QUERYINTERFACE_ENTRY(i2) \
  CPCL_QUERYINTERFACE_ENTRY(i3) \
  )

#endif // __CPCL_INTERNAL_COM_H
