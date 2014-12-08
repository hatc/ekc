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
#include "CInArchiveImpl.h"
#include <Windows/PropVariant.h>

namespace files {

inline static UInt64 PropVariantToUInt64(const PROPVARIANT &prop)
{
  switch (prop.vt)
  {
    case VT_UI1: return prop.bVal;
    case VT_UI2: return prop.uiVal;
    case VT_UI4: return prop.ulVal;
    case VT_UI8: return prop.uhVal.QuadPart;
    case VT_I2: return prop.iVal;
    case VT_I4: return prop.lVal;
    case VT_I8: return prop.hVal.QuadPart;
    default: return 0;
  }
}

//CInArchiveImpl::CInArchiveImpl(IInArchive *archiveHandler, wchar_t const *defaultName, size_t defaultNameSize) : ... defaultName_(defaultName, defaultNameSize)
CInArchiveImpl::CInArchiveImpl(IInArchive *archiveHandler, std::wstring &defaultName) : archiveHandler_(archiveHandler), itemPathIndex_((UINT32)(INT32)(-1)) {
	SYSTEMTIME v;
	::GetSystemTime(&v);
	::SystemTimeToFileTime(&v, &mtimeDefault_);
	defaultName_.swap(defaultName);
}

HRESULT CInArchiveImpl::getArchiveHandler(IInArchive **r) {
	scopedComPtr<IInArchive> v(archiveHandler_);
	*r = v.Detach();
	return S_OK;
}

UINT64 CInArchiveImpl::itemSizeByte(UINT32 index) {
	UINT64 r;
	{
		// Get uncompressed size of file
		NWindows::NCOM::CPropVariant prop;
		archiveHandler_->GetProperty(index, kpidSize, &prop);

		r = PropVariantToUInt64(prop);
	}
	return r;
}

UINT32 CInArchiveImpl::itemsCount() {
	UInt32 numItems(0);
	archiveHandler_->GetNumberOfItems(&numItems);
	return numItems;
}

HRESULT CInArchiveImpl::itemPath(UINT32 index, wchar_t const **r, size_t *rSize) {
	if (itemPathIndex_ != index) {
		if (!itemPath_.empty())
			itemPath_.erase(0, itemPath_.size());
		{
			NWindows::NCOM::CPropVariant prop;
			RINOK(archiveHandler_->GetProperty(index, kpidPath, &prop))
			if (prop.vt == VT_BSTR)
				itemPath_.assign((wchar_t const*)prop.bstrVal, ::SysStringLen(prop.bstrVal));
			else if (prop.vt != VT_EMPTY)
				return E_FAIL;
		}
		if (itemPath_.empty())
		{
			itemPath_ = defaultName_;
			NWindows::NCOM::CPropVariant prop;
			RINOK(archiveHandler_->GetProperty(index, kpidExtension, &prop))
			if (prop.vt == VT_BSTR) {
				itemPath_ += L'.';
				itemPath_.append((wchar_t const*)prop.bstrVal, ::SysStringLen(prop.bstrVal));
			} else if (prop.vt != VT_EMPTY)
				return E_FAIL;
		}
		itemPathIndex_ = index;
	}

	if (r)
		*r = itemPath_.c_str();
	if (rSize)
		*rSize = itemPath_.size();
	return S_OK;
}

HRESULT CInArchiveImpl::itemIsDirectory(UINT32 index, bool *r) {
	{
		NWindows::NCOM::CPropVariant prop;
		RINOK(archiveHandler_->GetProperty(index, kpidIsDir, &prop))
		if (prop.vt == VT_BOOL) {
			if (r)
				*r = (prop.boolVal != VARIANT_FALSE);
		}
		else if (prop.vt != VT_EMPTY)
			return E_FAIL;
		else { // prop.vt == VT_EMPTY
			if (r)
				*r = false;
		}
	}
	return S_OK;
}

HRESULT CInArchiveImpl::itemInfo(UINT32 index, fileItemInfo *r) {
	/*if (!r) return E_INVALIDARG;*/

	DWORD attrib;
	{
		NWindows::NCOM::CPropVariant prop;
		RINOK(archiveHandler_->GetProperty(index, kpidAttrib, &prop))
		if (prop.vt == VT_EMPTY)
			attrib = 0;
		else {
			if (prop.vt != VT_UI4)
				return E_FAIL;
			attrib = prop.ulVal;
		}
	}
	{
		NWindows::NCOM::CPropVariant prop;
		RINOK(archiveHandler_->GetProperty(index, kpidIsDir, &prop))
		if (prop.vt == VT_BOOL) {
			if (prop.boolVal != VARIANT_FALSE)
				attrib |= FILE_ATTRIBUTE_DIRECTORY;
		}
		else if (prop.vt != VT_EMPTY)
			return E_FAIL;
	}

	FILETIME mtime;
	{
    NWindows::NCOM::CPropVariant prop;
    RINOK(archiveHandler_->GetProperty(index, kpidMTime, &prop));
    switch(prop.vt)
    {
      case VT_EMPTY:
        mtime = mtimeDefault_;
        break;
      case VT_FILETIME:
        mtime = prop.filetime;
        break;
      default:
        return E_FAIL;
		}
  }

	RINOK(itemPath(index, NULL, NULL))
	
	UINT64 size = itemSizeByte(index);

	if (r)
		(*r).reset(itemPath_.c_str(), itemPath_.size(), size, attrib, mtime);

	return S_OK;
}

HRESULT CInArchiveImpl::Extract(const UINT32 *indices, UINT32 numItems, INT32 testMode, IArchiveExtractCallback *extractCallback) {
	return archiveHandler_->Extract(indices, numItems, testMode, extractCallback);
}

} // namespace files
