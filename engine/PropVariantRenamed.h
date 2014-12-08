// PropVariantRenamed.h
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

#ifndef __WINDOWS_PROPVARIANT_RENAMED_H
#define __WINDOWS_PROPVARIANT_RENAMED_H

#include <initguid.h>
#include <windows.h>
#include <Objidl.h>

namespace comUtility {

typedef DWORD WRes;

typedef unsigned char Byte;
typedef short Int16;
typedef unsigned short UInt16;
typedef int Int32;
typedef unsigned int UInt32;
typedef __int64 Int64;
typedef unsigned __int64 UInt64;
typedef size_t SizeT;

class PropVariantRenamed : public tagPROPVARIANT {
public:
  PropVariantRenamed() { vt = VT_EMPTY; wReserved1 = 0; }
  ~PropVariantRenamed() { Clear(); }
  PropVariantRenamed(const PROPVARIANT& varSrc);
	PropVariantRenamed(const VARIANT& varSrc);
  PropVariantRenamed(const PropVariantRenamed& varSrc);
  PropVariantRenamed(BSTR bstrSrc);
  PropVariantRenamed(LPCOLESTR lpszSrc);
  PropVariantRenamed(bool bSrc) { vt = VT_BOOL; wReserved1 = 0; boolVal = (bSrc ? VARIANT_TRUE : VARIANT_FALSE); };
  PropVariantRenamed(UInt32 value) { vt = VT_UI4; wReserved1 = 0; ulVal = value; }
  PropVariantRenamed(UInt64 value) { vt = VT_UI8; wReserved1 = 0; uhVal = *(ULARGE_INTEGER*)&value; }
  PropVariantRenamed(const FILETIME &value) { vt = VT_FILETIME; wReserved1 = 0; filetime = value; }
  PropVariantRenamed(Int32 value) { vt = VT_I4; wReserved1 = 0; lVal = value; }
  PropVariantRenamed(Byte value) { vt = VT_UI1; wReserved1 = 0; bVal = value; }
  PropVariantRenamed(Int16 value) { vt = VT_I2; wReserved1 = 0; iVal = value; }
  // PropVariantRenamed(LONG value, VARTYPE vtSrc = VT_I4) { vt = vtSrc; lVal = value; }

  PropVariantRenamed& operator=(const PropVariantRenamed& varSrc);
  PropVariantRenamed& operator=(const PROPVARIANT& varSrc);
  PropVariantRenamed& operator=(BSTR bstrSrc);
  PropVariantRenamed& operator=(LPCOLESTR lpszSrc);
  PropVariantRenamed& operator=(bool bSrc);
  PropVariantRenamed& operator=(UInt32 value);
  PropVariantRenamed& operator=(UInt64 value);
	PropVariantRenamed& operator=(DATE value);
  PropVariantRenamed& operator=(const FILETIME &value);

  PropVariantRenamed& operator=(Int32 value);
  PropVariantRenamed& operator=(Byte value);
  PropVariantRenamed& operator=(Int16 value);
  // PropVariantRenamed& operator=(LONG  value);

  HRESULT Clear();
  HRESULT Copy(const PROPVARIANT* pSrc);
	HRESULT Copy(const VARIANT* pSrc);
  HRESULT Attach(PROPVARIANT* pSrc);
  HRESULT Detach(PROPVARIANT* pDest);
	HRESULT Detach(VARIANT* pDest);

  HRESULT InternalClear();
  void InternalCopy(const PROPVARIANT* pSrc);
	void InternalCopy(const VARIANT* pSrc);

  int Compare(const PropVariantRenamed &a1);
};

} // namespace comUtility

#endif // __WINDOWS_PROPVARIANT_RENAMED_H

