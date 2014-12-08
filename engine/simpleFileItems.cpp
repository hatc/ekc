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

#include <dumbassert.h>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include "simpleFileItems.h"

#include "files/fileUtil.h"
#include "files/fileIterator.hpp"
#include "files/temporaryStream.h"
#include "files/Client7zHandler.h"
#include "files/archiveUtil.h"

#include "files/sharedIconMap.h"
#include "files/sharedClient7z.h"

#include <impl_exception_helper.hpp>

static STATPROPSTG simpleFileItemProperties[] =
{
  { NULL, KPID_NAME, VT_BSTR},
	{ NULL, KPID_EXTENSION, VT_BSTR},
  { NULL, KPID_IS_DIR, VT_BOOL},
  { NULL, KPID_SIZE, VT_UI8},
	{ NULL, KPID_MTIME, VT_DATE},
  { NULL, KPID_CTIME, VT_DATE},
  { NULL, KPID_ATIME, VT_DATE},
	{ NULL, KPID_PATH, VT_BSTR},
	{ NULL, KPID_VIEWABLE, VT_BOOL},
	{ NULL, KPID_SUSPICIOUS, VT_BOOL},
	{ NULL, KPID_CONTAINTEXT, VT_BOOL},
	{ NULL, KPID_CONTAINER, VT_BOOL},
	{ NULL, KPID_PROCESSITEM, VT_BOOL},
	{ NULL, KPID_ICONID, VT_I4}
};

/**************************************** IFileItem ****************************************/

inline DATE filetime2date(FILETIME f) {
	DATE res(0);

	FILETIME fLocal;
	if (FileTimeToLocalFileTime(&f, &fLocal)) {
		SYSTEMTIME systime;
		if (FileTimeToSystemTime(&fLocal, &systime)) {
			SystemTimeToVariantTime(&systime, &res);
		}
	}

	return res;
}

void simpleFileItem::initializeItem(int itemId, comUtility::PropVariantRenamed &item) {
	// (itemId), (item); // iconID

	if (KPID_PATH == itemId) {
		DUMBASS_CHECK(item.vt == VT_BSTR);
		// path = (const wchar_t *)item.bstrVal;
	} else if (KPID_NAME == itemId) {
		DUMBASS_CHECK(item.vt == VT_BSTR);
		const wchar_t *fileName_ = (const wchar_t *)item.bstrVal;

		iconID = files::getIconIndex(fileName_);
		if (hasItem(KPID_ICONID)) {
			comUtility::PropVariantRenamed *iconItem = ensureItem(KPID_ICONID);
			*iconItem = iconID;
		}
	} else if (KPID_ICONID == itemId) {
		if (iconID >= 0) {
			item = iconID;
		}
	}
}

void simpleFileItem::initProperties(files::fileItemInfo const &v) {
	stream_ = v.stream;
	iconID = files::getIconIndex(v.fileName_.c_str());

	for (int i = 0; i < sizeof(simpleFileItemProperties) / sizeof(simpleFileItemProperties[0]); ++i) {
		comUtility::PropVariantRenamed *item = ensureItem(static_cast<int>(simpleFileItemProperties[i].propid));
		comUtility::PropVariantRenamed &prop = *item;

		switch(simpleFileItemProperties[i].propid) {
			case KPID_NAME:
				prop = v.fileName_.c_str();
				break;
			case KPID_IS_DIR:
				prop = v.isDirectory();
				break;
			case KPID_EXTENSION:
				if (!(v.isDirectory() || v.fileExt_.empty()))
					prop = v.fileExt_.c_str();
				break;
			case KPID_SIZE:
				prop = static_cast<comUtility::UInt64>(v.size);
				break;
			case KPID_MTIME:
				prop = filetime2date(v.mtime);
				break;
			case KPID_CTIME:
				prop = filetime2date(v.ctime);
				break;
			case KPID_ATIME:
				prop = filetime2date(v.atime);
				break;
			case KPID_PATH:
				prop = v.path.c_str();
				break;
			case KPID_VIEWABLE:
				prop = false; // isViewable;
				break;
			case KPID_SUSPICIOUS:
				prop = false; // isSuspicious;
				break;
			case KPID_CONTAINTEXT:
				prop = false; // isContainText;
				break;
			case KPID_ARCHIVE:
				prop = !!v.stream; // isArchive;
				break;
			case KPID_CONTAINER:
				prop = (v.isDirectory() || !!v.stream);
				break;
			case KPID_PROCESSITEM:
				prop = true;
				break;
			case KPID_ICONID:
				prop = iconID;
				break;
		}
		DUMBASS_CHECK(item->vt != VT_ERROR);
	}
}

STDMETHODIMP simpleFileItem::get_Attributes(IPropertyBasket **attributes) {
	if (!attributes)
		return E_POINTER;

	ScopedComPtr<IPropertyBasket> attributesBasket(this);
	*attributes = attributesBasket.Detach();
  return S_OK;
}

STDMETHODIMP simpleFileItem::get_Path(BSTR *v) {
	try {
		if (!v)
			return E_POINTER;

		itemBasket_const_i i = itemBasket.find(KPID_PATH);
		if (itemBasket.end() == i)
			return E_UNEXPECTED;

		comUtility::PropVariantRenamed *item = (*i).second;
		if (item->vt != VT_BSTR)
			return E_UNEXPECTED;
		
		*v = ::SysAllocString(item->bstrVal);
		return (v != NULL) ? S_OK : E_OUTOFMEMORY;
	} catch (...) {
		return E_FAIL;
	}
}

STDMETHODIMP simpleFileItem::get_Name(BSTR *v) {
	try {
		if (!v)
			return E_POINTER;
		
		itemBasket_const_i i = itemBasket.find(KPID_NAME);
		if (itemBasket.end() == i)
			return E_UNEXPECTED;
		
		comUtility::PropVariantRenamed *item = (*i).second;
		if (item->vt != VT_BSTR)
			return E_UNEXPECTED;
		
		*v = ::SysAllocString(item->bstrVal);
		return (v != NULL) ? S_OK : E_OUTOFMEMORY;
	} catch (...) {
		return E_FAIL;
	}
}

STDMETHODIMP simpleFileItem::get_Stream(IStream **stream) {
	try {
		if (!stream)
			return E_POINTER;
		*stream = 0;

		/*VARIANT v;
		::VariantInit(&v);
		if SUCCEEDED(GetItem(kpidIsDir, &v)) {
			if ((VT_BOOL == v.vt) && (VARIANT_TRUE == v.boolVal))
				return STG_E_PATHNOTFOUND;
		}*/
		{
			itemBasket_const_i i = itemBasket.find(KPID_IS_DIR);
			if (itemBasket.end() != i) {
				comUtility::PropVariantRenamed *item = (*i).second;
				if ((VT_BOOL == item->vt) && (VARIANT_TRUE == item->boolVal)) {
					return STG_E_PATHNOTFOUND;
				}
			}
		}
		
		{
			itemBasket_const_i i = itemBasket.find(KPID_CACHE_STREAM);
			if (itemBasket.end() != i) {
				comUtility::PropVariantRenamed *item = (*i).second;
				if ((VT_BOOL == item->vt) && (VARIANT_FALSE == item->boolVal)) {
					stream_.Release();
					return S_FALSE;
				}
			}
		}
		
		if (!stream_) {
			itemBasket_const_i i = itemBasket.find(KPID_PATH);
			if (itemBasket.end() == i)
				return E_UNEXPECTED;
			comUtility::PropVariantRenamed *item = (*i).second;
			if (item->vt != VT_BSTR)
				return E_UNEXPECTED;
			
			std::wstring d,r,v;
			if (!files::formatPath(item->bstrVal, ::SysStringLen(item->bstrVal), &d, &r, &v))
				return E_FAIL;
			if (r.empty())
				return E_UNEXPECTED;
			if (v.empty()) {
				RINOK(files::fileStreamRead(r.c_str(), &stream_))
			} else {
				//files::Client7zHandler *client7zHandler = new files::Client7zHandler(NULL);
				//client7zHandler->load7zHandler();
				files::Client7zHandler *client7zHandler = files::getSharedClient7z();
				
				{
					files::fileIterator fileIt(r), fileItTail;
					if (fileIt != fileItTail) {
						files::fileItemInfo itemInfo = *fileIt;
						itemInfo.path = r;
						
						if (client7zHandler->isArchiveExtension(itemInfo)) {
							files::fileItemInfo r;
							if SUCCEEDED(files::seekOutArchive(client7zHandler, itemInfo, v, &r)) {
								stream_ = r.stream;
							}
						}
					}
				}
				//delete client7zHandler;
			}
		}
		
		if (!stream_)
			return E_FAIL;
		{
			ScopedComPtr<IStream> r(stream_);
			*stream = r.Detach();
		}
		
		bool releaseStream(true);
		{
			itemBasket_const_i i = itemBasket.find(KPID_CACHE_STREAM);
			if (itemBasket.end() != i) {
				comUtility::PropVariantRenamed *item = (*i).second;
				if ((VT_BOOL == item->vt) && (VARIANT_TRUE == item->boolVal)) {
					releaseStream = false;
				}
			}
		}
		if (releaseStream)
			stream_.Release();
		
		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}

// F8C62791-9F2D-45f3-A6D1-3225BB4199F0
GUID const simpleFileItem::IID_CSimpleFileItem = { 0xf8c62791, 0x9f2d, 0x45f3, { 0xa6, 0xd1, 0x32, 0x25, 0xbb, 0x41, 0x99, 0xf0 } };

STDMETHODIMP simpleFileItem::GetClassID(CLSID *pClassID) {
	try {
		if (!pClassID)
			return E_INVALIDARG;

		*pClassID = IID_CSimpleFileItem;
		return S_OK;
	} CATCH_EXCEPTION("simpleFileItem::Load()")
}

STDMETHODIMP simpleFileItem::IsDirty() {
	return S_FALSE;
}

STDMETHODIMP simpleFileItem::Load(IStream *pStm) {
	try {
		if (!pStm)
			return E_INVALIDARG;

		return load(pStm);
	} CATCH_EXCEPTION("simpleFileItem::Load()")
}

STDMETHODIMP simpleFileItem::Save(IStream *pStm, BOOL) {
	try {
		if (!pStm)
			return E_INVALIDARG;

		return save(pStm);
	} CATCH_EXCEPTION("simpleFileItem::Save()")
}

STDMETHODIMP simpleFileItem::GetSizeMax(ULARGE_INTEGER *pcbSize) {
	try {
		if (!pcbSize)
			return E_INVALIDARG;

		size_t size;
		THROW_FAILED(calculateSize(&size));

		(*pcbSize).QuadPart = size;
		return S_OK;
	} CATCH_EXCEPTION("simpleFileItem::GetSizeMax()")
}
