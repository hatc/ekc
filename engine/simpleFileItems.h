// simpleFileItems.h
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

#ifndef __FILES_SIMPLE_FILE_ITEMS_H
#define __FILES_SIMPLE_FILE_ITEMS_H

#include <basic.h>

#include <algorithm>
#include <vector>

#include <internal_com.h>

#include "attr_id.h"
#include "variantItemStorage.h"

#include "files/fileItemInfo.hpp"

#include "engine_interface.h"

//class simpleFileItem : public IFileItem, public baseVariantItemStorage, 
//	public comUtilityInternal::basePersistStreamImpl, public CUnknownImp {
class simpleFileItem : public baseVariantItemStorage, public CUnknownImp, public IFileItem, public IPersistStream {
	DISALLOW_COPY_AND_ASSIGN(simpleFileItem);

	// F8C62791-9F2D-45f3-A6D1-3225BB4199F0
	static GUID const IID_CSimpleFileItem;
protected:
  virtual void initializeItem(int itemId, comUtility::PropVariantRenamed &item);
public:
	int iconID;
	ScopedComPtr<IStream> stream_;

	simpleFileItem() : iconID(-1), baseVariantItemStorage()
	{}
	~simpleFileItem()
	{}

	void initProperties(files::fileItemInfo const &v);
	//bool processItems() {
	//	VARIANT v; VariantInit(&v);
	//	if SUCCEEDED(GetItem(kpidProcessItem, &v)) {
	//		if (VT_BOOL == v.vt)
	//			return VARIANT_TRUE == v.boolVal;
	//	}
	//	return true;
	//}

	/* IFileItem */

	STDMETHOD(get_Attributes)(IPropertyBasket **attributes);
	STDMETHOD(get_Path)(BSTR *v);
	STDMETHOD(get_Name)(BSTR *v);
	STDMETHOD(get_Stream)(IStream **stream);

	/* IPersist */
	STDMETHOD(GetClassID)(CLSID *pClassID);

	/* IPersistStream */
	STDMETHOD(IsDirty)();
	STDMETHOD(Load)(IStream *pStm);
	STDMETHOD(Save)(IStream *pStm, BOOL fClearDirty);
	STDMETHOD(GetSizeMax)(ULARGE_INTEGER *pcbSize);

	CPCL_UNKNOWN_IMP3_MT(IFileItem, IPropertyBasket, IPersistStream)
};

#endif // __SIMPLE_FILE_ITEMS_H
