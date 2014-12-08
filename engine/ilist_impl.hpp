// ilist_impl.hpp
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

#include <algorithm>
#include <vector>

#include <com_ptr.hpp>
#include <impl_exception_helper.hpp>

#include <windows.h>

template<class List, class Item>
class IListImpl : public List {
protected:
	typedef std::vector<cpcl::ComPtr<Item> > Container;

	Container container;
public:
	STDMETHOD(Clear)() {
		try {
			container.clear();
			return S_OK;
		} CATCH_EXCEPTION("IListImpl::Clear()")
	}
	
	STDMETHOD(get_Count)(long *v) {
		try {
			if (!v)
				return E_INVALIDARG;

			*v = static_cast<long>(container.size());
			return S_OK;
		} CATCH_EXCEPTION("IListImpl::get_Count()")
	}
	
	STDMETHOD(GetItem)(long index, Item **v) {
		try {
			size_t index_ = static_cast<size_t>(index);
			if (index_ >= container.size()) {
				cpcl::Debug(cpcl::StringPieceFromLiteral("IListImpl::GetItem(): list index out of range"));
				return E_INVALIDARG;
			}

			cpcl::ComPtr<Item> r = container[index_];
			if (v)
				*v = r.Detach();
			return S_OK;
		} CATCH_EXCEPTION("IListImpl::GetItem()")
	}
	
	STDMETHOD(AddItem)(Item *item, long *index) {
		try {
			if (!item)
				return E_INVALIDARG;

			cpcl::ComPtr<Item> v(item);
			long index_ = static_cast<long>(container.size());
			container.push_back(v);
			if (index)
				*index = index_;
			return S_OK;
		} CATCH_EXCEPTION("IListImpl::AddItem()")
	}
	
	STDMETHOD(DeleteItem)(long index) {
		try {
			size_t index_ = static_cast<size_t>(index);
			if (index_ >= container.size()) {
				cpcl::Debug(cpcl::StringPieceFromLiteral("IListImpl::DeleteItem(): list index out of range"));
				return E_INVALIDARG;
			}

			container.erase(container.begin() + index_);
			return S_OK;
		} CATCH_EXCEPTION("IListImpl::DeleteItem()")
	}

	STDMETHOD(RemoveItem)(Item *item) {
		try {
			if (!item)
				return E_INVALIDARG;

			/*typename*/ Container::const_iterator i = std::find(container.begin(), container.end(), item);
			if (i == container.end()) {
				cpcl::Debug(cpcl::StringPieceFromLiteral("IListImpl::RemoveItem(): unable to find specified item"));
				return S_FALSE;
			}

			container.erase(i);
			return S_OK;
		} CATCH_EXCEPTION("IListImpl::RemoveItem")
	}

	template<class T>
	HRESULT GetRangeImpl(long index, long count, T *list) {
		// create a copy of the empty list
		if (container.empty() && index == count && index == 0)
			return S_OK;

		size_t index_ = static_cast<size_t>(index);
		if (index_ >= container.size()) {
			cpcl::Debug(cpcl::StringPieceFromLiteral("IListImpl::GetRangeImpl(): list index out of range"));
			return E_INVALIDARG;
		}

		Container::const_iterator head = container.begin() + index_;
		// index_ + (std::min)(container.size() - index_, static_cast<size_t>(count));
		Container::const_iterator tail = head + (std::min)(container.size() - index_, static_cast<size_t>(count));
		list->container.insert(list->container.end(), head, tail);
		return S_OK;
	}
};
