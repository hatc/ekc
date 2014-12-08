// variantItemStorage.h
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

#ifndef __VARIANT_ITEM_STORAGE_H
#define __VARIANT_ITEM_STORAGE_H

#include <map>
#include <list>
#include <algorithm>

#include "PropVariantRenamed.h"

#include "engine_interface.h"

class baseVariantItemStorage : public IPropertyBasket {
  struct _Finder {
    comUtility::PropVariantRenamed const *v_;

    _Finder(comUtility::PropVariantRenamed const *v) : v_(v) {}
    bool operator()(std::pair<int, comUtility::PropVariantRenamed*> const &v) const {
      return (v.second == v_);
    }
  };

  static size_t const magicHeader;
  static size_t const magicItemHeader;
protected:
  typedef std::map<int, comUtility::PropVariantRenamed*> itemBasket_t;
	typedef itemBasket_t::const_iterator itemBasket_const_i;
	typedef itemBasket_t::iterator itemBasket_i;
	typedef std::list<comUtility::PropVariantRenamed*> itemList_t;
	typedef itemList_t::const_iterator itemList_const_i;
	typedef itemList_t::iterator itemList_i;

  itemBasket_t itemBasket;
	itemList_t itemList;

  void clear();
	void copy(const baseVariantItemStorage &v);

  HRESULT load(IStream *v);
  HRESULT save(IStream *v) const;
	HRESULT calculateSize(size_t *v);
	virtual void initializeItem(int itemId, comUtility::PropVariantRenamed &item) { (itemId),(item); }

  comUtility::PropVariantRenamed* ensureItem(int itemId);

  bool hasItem(int itemId) const { return (itemBasket.find(itemId) != itemBasket.end()); }
  bool idBasketItem(comUtility::PropVariantRenamed const *v, int *r) const;
  bool indexListItem(int itemId, int *r) const;
public:
  /* IPropertyBasket */

	STDMETHOD(Clear)(void);
	STDMETHOD(get_Count)(long *count);

	STDMETHOD(GetItemInfo)(long index, BSTR *itemName, int *itemID, VARTYPE *varType);
	STDMETHOD(GetItem)(int itemID, VARIANT *value);
	STDMETHOD(SetItem)(int itemID, VARIANT value);
	STDMETHOD(GetItemValue)(int itemID, unsigned char *buffer, int *bufferSize);
	STDMETHOD(SetItemValue)(int itemID, const unsigned char *buffer, int bufferSize);

	STDMETHOD(DeleteItem)(long index);
	STDMETHOD(RemoveItem)(int itemID);
  STDMETHOD(FindItem)(int itemID, long *index);

	/* IPropertyBasket */

  baseVariantItemStorage() {}
  baseVariantItemStorage(baseVariantItemStorage const &r) { copy(r); }
  virtual ~baseVariantItemStorage() { clear(); }
};

#endif // __VARIANT_ITEM_STORAGE_H
