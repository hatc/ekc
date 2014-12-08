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

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include <internal_com.h> // RINOK

#include "PropVariantRenamed.h"

namespace comUtility {

template <class T> inline T MyMin(T a, T b)
  {  return a < b ? a : b; }
template <class T> inline T MyMax(T a, T b)
  {  return a > b ? a : b; }

template <class T> inline int MyCompare(T a, T b)
  {  return a < b ? -1 : (a == b ? 0 : 1); }

inline int BoolToInt(bool value)
  { return (value ? 1: 0); }

inline bool IntToBool(int value)
  { return (value != 0); }

PropVariantRenamed::PropVariantRenamed(const PROPVARIANT& varSrc)
{
  vt = VT_EMPTY;
  InternalCopy(&varSrc);
}

PropVariantRenamed::PropVariantRenamed(const VARIANT& varSrc)
{
  vt = VT_EMPTY;
  InternalCopy(&varSrc);
}


PropVariantRenamed::PropVariantRenamed(const PropVariantRenamed& varSrc)
{
  vt = VT_EMPTY;
  InternalCopy(&varSrc);
}

PropVariantRenamed::PropVariantRenamed(BSTR bstrSrc)
{
  vt = VT_EMPTY;
  *this = bstrSrc;
}

PropVariantRenamed::PropVariantRenamed(LPCOLESTR lpszSrc)
{
  vt = VT_EMPTY;
  *this = lpszSrc;
}

PropVariantRenamed& PropVariantRenamed::operator=(const PropVariantRenamed& varSrc)
{
  InternalCopy(&varSrc);
  return *this;
}
PropVariantRenamed& PropVariantRenamed::operator=(const PROPVARIANT& varSrc)
{
  InternalCopy(&varSrc);
  return *this;
}

PropVariantRenamed& PropVariantRenamed::operator=(BSTR bstrSrc)
{
  *this = (LPCOLESTR)bstrSrc;
  return *this;
}

PropVariantRenamed& PropVariantRenamed::operator=(LPCOLESTR lpszSrc)
{
  InternalClear();
  vt = VT_BSTR;
  wReserved1 = 0;
  bstrVal = ::SysAllocString(lpszSrc);
  if (bstrVal == NULL && lpszSrc != NULL)
  {
    vt = VT_ERROR;
    scode = E_OUTOFMEMORY;
  }
  return *this;
}


PropVariantRenamed& PropVariantRenamed::operator=(bool bSrc)
{
  if (vt != VT_BOOL)
  {
    InternalClear();
    vt = VT_BOOL;
  }
  boolVal = bSrc ? VARIANT_TRUE : VARIANT_FALSE;
  return *this;
}

PropVariantRenamed& PropVariantRenamed::operator=(UInt32 value)
{
  if (vt != VT_UI4)
  {
    InternalClear();
    vt = VT_UI4;
  }
  ulVal = value;
  return *this;
}

PropVariantRenamed& PropVariantRenamed::operator=(DATE value)
{
  if (vt != VT_DATE)
  {
    InternalClear();
    vt = VT_DATE;
  }
  date = value;
  return *this;
}

PropVariantRenamed& PropVariantRenamed::operator=(UInt64 value)
{
  if (vt != VT_UI8)
  {
    InternalClear();
    vt = VT_UI8;
  }
  uhVal.QuadPart = value;
  return *this;
}

PropVariantRenamed& PropVariantRenamed::operator=(const FILETIME &value)
{
  if (vt != VT_FILETIME)
  {
    InternalClear();
    vt = VT_FILETIME;
  }
  filetime = value;
  return *this;
}

PropVariantRenamed& PropVariantRenamed::operator=(Int32 value)
{
  if (vt != VT_I4)
  {
    InternalClear();
    vt = VT_I4;
  }
  lVal = value;
  
  return *this;
}

PropVariantRenamed& PropVariantRenamed::operator=(Byte value)
{
  if (vt != VT_UI1)
  {
    InternalClear();
    vt = VT_UI1;
  }
  bVal = value;
  return *this;
}

PropVariantRenamed& PropVariantRenamed::operator=(Int16 value)
{
  if (vt != VT_I2)
  {
    InternalClear();
    vt = VT_I2;
  }
  iVal = value;
  return *this;
}

/*
PropVariantRenamed& PropVariantRenamed::operator=(LONG value)
{
  if (vt != VT_I4)
  {
    InternalClear();
    vt = VT_I4;
  }
  lVal = value;
  return *this;
}
*/

static HRESULT MyPropVariantClear(PROPVARIANT *propVariant)
{
  switch(propVariant->vt)
  {
		case VT_EMPTY:
    case VT_I1:
		case VT_UI1:
    case VT_I2:
    case VT_UI2:
    case VT_BOOL:
    case VT_I4:
    case VT_UI4:
    case VT_INT:
    case VT_UINT:
    case VT_ERROR:
    case VT_FILETIME:
		case VT_I8:
    case VT_UI8:
		case VT_R4:
    case VT_R8:
    case VT_CY:
    case VT_DATE:
      propVariant->vt = VT_EMPTY;
      propVariant->wReserved1 = 0;
      return S_OK;
  }
  // return ::VariantClear((VARIANTARG *)propVariant);
  HRESULT hr = ::VariantClear((VARIANTARG *)propVariant);
  if ((DISP_E_BADVARTYPE == hr) 
    && (VT_ARRAY == propVariant->vt)
    && (propVariant->parray != NULL)) {
    RINOK(::SafeArrayDestroy(propVariant->parray))
    
    propVariant->vt = VT_EMPTY;
    propVariant->wReserved1 = 0;
    propVariant->parray = NULL;
    return S_OK;
  }
  return hr;
}

HRESULT PropVariantRenamed::Clear()
{
  return MyPropVariantClear(this);
}

HRESULT PropVariantRenamed::Copy(const PROPVARIANT* pSrc)
{
  // ::VariantClear((tagVARIANT *)this);
  RINOK(Clear())

  switch(pSrc->vt)
  {
    case VT_I1:
		case VT_UI1:
    case VT_I2:
    case VT_UI2:
    case VT_BOOL:
    case VT_I4:
    case VT_UI4:
    case VT_INT:
    case VT_UINT:
    case VT_ERROR:
    case VT_FILETIME:
		case VT_I8:
    case VT_UI8:
		case VT_R4:
    case VT_R8:
    case VT_CY:
    case VT_DATE:
      memmove((PROPVARIANT*)this, pSrc, sizeof(PROPVARIANT));
      return S_OK;
  }
  return ::VariantCopy((tagVARIANT *)this, (tagVARIANT *)(pSrc));
}
HRESULT PropVariantRenamed::Copy(const VARIANT* pSrc)
{
  // ::VariantClear((tagVARIANT *)this);
  RINOK(Clear())

  switch(pSrc->vt)
  {
    case VT_I1:
		case VT_UI1:
    case VT_I2:
    case VT_UI2:
    case VT_BOOL:
    case VT_I4:
    case VT_UI4:
    case VT_INT:
    case VT_UINT:
    case VT_ERROR:
    case VT_FILETIME:
		case VT_I8:
    case VT_UI8:
		case VT_R4:
    case VT_R8:
    case VT_CY:
    case VT_DATE:
      memmove((PROPVARIANT*)this, pSrc, sizeof(VARIANT));
      return S_OK;
  }
  return ::VariantCopy((tagVARIANT *)this, (tagVARIANT *)(pSrc));
}


HRESULT PropVariantRenamed::Attach(PROPVARIANT* pSrc)
{
  HRESULT hr = Clear();
  if (FAILED(hr))
    return hr;
  memcpy(this, pSrc, sizeof(PROPVARIANT));
  pSrc->vt = VT_EMPTY;
  return S_OK;
}
HRESULT PropVariantRenamed::Detach(PROPVARIANT* pDest)
{
  HRESULT hr = MyPropVariantClear(pDest);
  if (FAILED(hr))
    return hr;
  memcpy(pDest, this, sizeof(PROPVARIANT));
  vt = VT_EMPTY;
  return S_OK;
}
HRESULT PropVariantRenamed::Detach(VARIANT* pDest)
{
  HRESULT hr = ::VariantClear(pDest);
  if (FAILED(hr))
    return hr;
  memcpy(pDest, this, sizeof(VARIANT));
  vt = VT_EMPTY;
  return S_OK;
}

HRESULT PropVariantRenamed::InternalClear()
{
  HRESULT hr = Clear();
  if (FAILED(hr))
  {
    vt = VT_ERROR;
    scode = hr;
  }
  return hr;
}

void PropVariantRenamed::InternalCopy(const PROPVARIANT* pSrc)
{
  HRESULT hr = Copy(pSrc);
  if (FAILED(hr))
  {
    vt = VT_ERROR;
    scode = hr;
  }
}
void PropVariantRenamed::InternalCopy(const VARIANT* pSrc)
{
  HRESULT hr = Copy(pSrc);
  if (FAILED(hr))
  {
    vt = VT_ERROR;
    scode = hr;
  }
}

int PropVariantRenamed::Compare(const PropVariantRenamed &a)
{
  if (vt != a.vt)
    return 0; // it's mean some bug
  switch (vt)
  {
    case VT_EMPTY:
      return 0;
    
    /*
    case VT_I1:
      return MyCompare(cVal, a.cVal);
    */
    case VT_UI1:
      return MyCompare(bVal, a.bVal);

    case VT_I2:
      return MyCompare(iVal, a.iVal);
    case VT_UI2:
      return MyCompare(uiVal, a.uiVal);
    
    case VT_I4:
      return MyCompare(lVal, a.lVal);
    /*
    case VT_INT:
      return MyCompare(intVal, a.intVal);
    */
    case VT_UI4:
      return MyCompare(ulVal, a.ulVal);
    /*
    case VT_UINT:
      return MyCompare(uintVal, a.uintVal);
    */
    case VT_I8:
      return MyCompare(hVal.QuadPart, a.hVal.QuadPart);
    case VT_UI8:
      return MyCompare(uhVal.QuadPart, a.uhVal.QuadPart);

    case VT_BOOL:
      return -MyCompare(boolVal, a.boolVal);

    case VT_FILETIME:
      return ::CompareFileTime(&filetime, &a.filetime);
    case VT_BSTR:
      return 0; // Not implemented
      // return MyCompare(aPropVarint.cVal);

    default:
      return 0;
  }
}

} // namespace comUtility
