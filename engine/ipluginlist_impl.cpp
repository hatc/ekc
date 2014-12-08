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
#include "ipluginlist_impl.h"
#include "ifilefilter_impl.h"

#include "impl_exception_helper.hpp"

IPluginListImpl::IPluginListImpl(PluginList *plugin_list_) : plugin_list(plugin_list_)
{}
IPluginListImpl::~IPluginListImpl()
{}

void IPluginListImpl::ReleaseFilters() {
	plugin_list->file_extension_filter.reset();
	plugin_list->file_signature_filter.reset();
}

void IPluginListImpl::RebuildFilters() {
	ReleaseFilters();

	/*plugin_list->file_extension_filter.reset(FileExtensionFilter::Create(plugin_list->plugins));
	plugin_list->file_signature_filter.reset(SignatureFilter::Create(plugin_list->plugins));*/
}

STDMETHODIMP IPluginListImpl::Clear() {
	try {
		plugin_list->plugins.clear();
		ReleaseFilters();
		return S_OK;
	} CATCH_EXCEPTION("IPluginListImpl::Clear()")
}

STDMETHODIMP IPluginListImpl::get_Count(long *v) {
	try {
		if (!v)
			return E_INVALIDARG;

		*v = static_cast<long>(plugin_list->plugins.size());
		return S_OK;
	} CATCH_EXCEPTION("IPluginListImpl::get_Count()")
}

STDMETHODIMP IPluginListImpl::GetItem(long index, IPlugin **v) {
	try {
		size_t index_ = static_cast<size_t>(index);
		if ((index_ >= plugin_list->plugins.size()) || (!v))
			return E_INVALIDARG;

		cpcl::ComPtr<IPlugin> plugin = plugin_list->plugins[index_];
		*v = plugin.Detach();
		return S_OK;
	} CATCH_EXCEPTION("IPluginListImpl::GetItem()")
}

STDMETHODIMP IPluginListImpl::AddItem(IPlugin *item, long *index) {
	try {
		size_t index_ = plugin_list->plugins.size();
		if (!item)
			return E_INVALIDARG;
		
		plugin_list->plugins.push_back(item);
		if (index)
			*index = static_cast<long>(index_);
		RebuildFilters();
		return S_OK;
	} CATCH_EXCEPTION("IPluginListImpl::AddItem()")
}

STDMETHODIMP IPluginListImpl::DeleteItem(long index) {
	try {
		size_t index_ = static_cast<size_t>(index);
		if (index_ >= plugin_list->plugins.size())
			return E_INVALIDARG;

		plugin_list->plugins.erase(plugin_list->plugins.begin() + index_);
		RebuildFilters();
		return S_OK;
	} CATCH_EXCEPTION("IPluginListImpl::DeleteItem()")
}

STDMETHODIMP IPluginListImpl::RemoveItem(IPlugin *item) {
	try {
		if (!item)
			return E_INVALIDARG;

		PluginList::Plugins::const_iterator i = plugin_list->plugins.begin();
		for (; i != plugin_list->plugins.end(); ++i) {
			IPlugin * const v = (IPlugin*)(*i);
			if (v == item)
				break;
		}
		if (plugin_list->plugins.end() == i)
			return S_FALSE;

		plugin_list->plugins.erase(i);
		RebuildFilters();
		return S_OK;
	} CATCH_EXCEPTION("IPluginListImpl::RemoveItem()")
}

STDMETHODIMP IPluginListImpl::GetRange(long index, long count, IPluginList **v) {
	try {
		if (!v)
			return E_INVALIDARG;
		
		cpcl::ComPtr<IPluginListImpl> r(new IPluginListImpl(PluginList::CreatePluginList()));
		if (plugin_list->plugins.empty() && index == count && index == 0) {
			*v = r.Detach();
			return S_OK;
		}

		size_t index_ = static_cast<size_t>(index);
		if (index_ >= plugin_list->plugins.size()) {
			cpcl::Debug(cpcl::StringPieceFromLiteral("IPluginListImpl::GetRangeImpl(): list index out of range"));
			return E_INVALIDARG;
		}

		PluginList::Plugins::const_iterator head = plugin_list->plugins.begin() + index_;
		// index_ + (std::min)(container.size() - index_, static_cast<size_t>(count));
		PluginList::Plugins::const_iterator tail = head + (std::min)(plugin_list->plugins.size() - index_, static_cast<size_t>(count));
		r->plugin_list->plugins.insert(r->plugin_list->plugins.end(), head, tail);

		r->RebuildFilters();

		*v = r.Detach();
		return S_OK;
	} CATCH_EXCEPTION("IPluginListImpl::GetRange()")
}

STDMETHODIMP IPluginListImpl::LoadDocument(IStream *input, IPluginDocument **r) {
	try {
		if (!input)
			return E_INVALIDARG;

		if (!plugin_list->file_signature_filter.get())
			plugin_list->file_signature_filter.reset(SignatureFilter::Create(plugin_list->plugins));
		return (plugin_list->LoadDoc(input, r)) ? S_OK : S_FALSE;
	} CATCH_EXCEPTION("IPluginListImpl::LoadDocument()")
}

STDMETHODIMP IPluginListImpl::get_FileExtensionFilter(IFileFilter **v) {
	try {
		if (!v)
			return E_INVALIDARG;
		if (!plugin_list->file_extension_filter.get())
			plugin_list->file_extension_filter.reset(FileExtensionFilter::Create(plugin_list->plugins));
		if (!plugin_list->file_extension_filter.get())
			throw std::exception("plugin_list::file_extension_filter not set", 1);

		FileExtensionFilter *filter_ = new FileExtensionFilter(*plugin_list->file_extension_filter.get());
		ScopedComPtr<IFileFilter> filter(new IFileFilterImpl(filter_, 0)); // filter_ object leak if new IFileFilterImpl throw due to low memory
		*v = filter.Detach();
		return S_OK;
	} CATCH_EXCEPTION("IPluginListImpl::get_FileExtensionFilter()")
}

STDMETHODIMP IPluginListImpl::get_FileSignatureFilter(IFileFilter **v) {
	try {
		if (!v)
			return E_INVALIDARG;
		if (!plugin_list->file_signature_filter.get())
			plugin_list->file_signature_filter.reset(SignatureFilter::Create(plugin_list->plugins));
		if (!plugin_list->file_signature_filter.get())
			throw std::exception("plugin_list::file_signature_filter not set", 1);

		SignatureFilter *filter_ = new SignatureFilter(*plugin_list->file_signature_filter.get());
		ScopedComPtr<IFileFilter> filter(new IFileFilterImpl(filter_, 1)); // filter_ object leak if new IFileFilterImpl throw due to low memory
		*v = filter.Detach();
		return S_OK;
	} CATCH_EXCEPTION("IPluginListImpl::get_FileSignatureFilter()")
}
