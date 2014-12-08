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

#include <memory>
#include <algorithm>

#include <split_iterator.hpp>
#include <scoped_buf.hpp>
#include <hresult_helper.hpp>
#include <dumbassert.h>
#include <trace.h>

#include <signature_info_helper.hpp>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include "plugin_filter.h"

PluginFilter::PluginFilter()
{}
PluginFilter::~PluginFilter()
{}

IPlugin* PluginFilter::Match(cpcl::WStringPiece const&) const {
	return (IPlugin*)0;
}
IPlugin* PluginFilter::Match(cpcl::IOStream *) const {
	return (IPlugin*)0;
}

bool FileExtensionFilter::WStringPieceLess::operator()(cpcl::WStringPiece const &l, cpcl::WStringPiece const &r) const {
	size_t const a_size = l.size(), b_size = r.size();
	for (size_t i = 0, size = (std::min)(a_size, b_size); i < size; ++i) {
		wchar_t ac = l[i], bc = r[i];
		if ((ac >= L'A') && (ac <= L'Z'))
			ac = (L'a' + (ac - L'A'));
		if ((bc >= L'A') && (bc <= L'Z'))
			bc = (L'a' + (bc - L'A'));

		if (ac != bc)
			return (ac < bc);
	}
	return (a_size < b_size);
}

FileExtensionFilter::FileExtensionFilter()
{}
FileExtensionFilter::~FileExtensionFilter()
{}

IPlugin* FileExtensionFilter::Match(cpcl::WStringPiece const &v) const {
	Plugins::const_iterator i = plugins.find(v);
	if (i != plugins.end()) {
		cpcl::ComPtr<IPlugin> const &plugin = (*i).second;
		return (IPlugin*)plugin;
	}
	return (IPlugin*)0;
}

FileExtensionFilter* FileExtensionFilter::Create(std::vector<cpcl::ComPtr<IPlugin> > const &plugins_) {
	std::auto_ptr<FileExtensionFilter> r(new FileExtensionFilter());
	for (std::vector<cpcl::ComPtr<IPlugin> >::const_iterator plugins_i = plugins_.begin(); plugins_i != plugins_.end(); ++plugins_i) {
		cpcl::ComPtr<IPlugin> plugin(*plugins_i);

		//Plugins::iterator plugin_i = r->plugins.end();
		wchar_t const *extension; DWORD extension_size;
		if SUCCEEDED(plugin->DocFileExtension(&extension, &extension_size)) {
			cpcl::WStringSplitIterator i(extension, extension_size, L'|'), tail;
			while (i != tail) {
				Plugins::_Pairib pairib = r->plugins.insert(Plugins::value_type((*i++), cpcl::ComPtr<IPlugin>()));
				if (pairib.second) {
					//plugin_i = pairib.first;
					//(*plugin_i).second = (IPlugin*)plugin; // if use std::map<WStringPiece, IPlugin*>, i.e. if filter not own IPlugin
					//(*plugin_i).second = plugin;
					(*pairib.first).second = plugin;
				}
			}
		}
	}
	return r.release();
}

SignatureFilter::SignatureFilter() : signatures(0), n_signatures(0), max_data_offset(0)
{}
SignatureFilter::SignatureFilter(SignatureFilter const &r) : signatures(0) { // signatures must be copied
	n_signatures = r.n_signatures;
	max_data_offset = r.max_data_offset;
	indices = r.indices;
	plugins = r.plugins;

	signatures = plcl::make_signatures(*r.signatures, r.n_signatures);
}
SignatureFilter& SignatureFilter::operator=(SignatureFilter const &r) {
	n_signatures = r.n_signatures;
	max_data_offset = r.max_data_offset;
	indices = r.indices;
	plugins = r.plugins;

	signatures = plcl::make_signatures(*r.signatures, r.n_signatures);

	return *this;
}
SignatureFilter::~SignatureFilter() {
	if (signatures)
		free(signatures);
}

IPlugin* SignatureFilter::Match(cpcl::IOStream *v) const {
	cpcl::ScopedBuf<unsigned char, 0x20> buf;
	{
		__int64 stream_offset = v->Tell();
		v->Seek(0, SEEK_SET, NULL);
		v->Read(buf.Alloc(max_data_offset), max_data_offset);
		/*try {
		 v->Read(buf.Alloc(max_data_offset), max_data_offset);
		} catch(std::exception const &e) {
		 v->Seek(stream_offset, SEEK_SET, NULL);
		 throw;
		} */
		v->Seek(stream_offset, SEEK_SET, NULL);

		/*__int64 stream_offset_(0);
		v->Seek(stream_offset, SEEK_SET, &stream_offset_);
		if (stream_offset != stream_offset_)
			cpcl::Warning("stream no seekable");*/
	}

	/*unsigned int i(0);
	for (; i < n_signatures; ++i) {
		if (memcmp(buf.Data() + signatures[i]->offset, signatures[i]->data, signatures[i]->size) == 0)
			break;
	}*/
	unsigned int i(n_signatures), signatures_size(0);
	for (unsigned int k = 0; k < n_signatures; ++k) {
		if (memcmp(buf.Data() + signatures[k]->offset, signatures[k]->data, signatures[k]->size) == 0) {
			if (signatures[k]->size > signatures_size) { 
				// select longest matched signature
				i = k;
				signatures_size = signatures[k]->size;
			}
		}
	}

	/*if (i >= n_signatures)
		return (IPlugin*)0; - additional check not needed - std::upper_bound just return indices.end(), so idx == plugins.size() and return (IPlugin*)0; */
	ptrdiff_t idx = std::upper_bound(indices.begin(), indices.end(), i) - indices.begin();
	if (idx < static_cast<ptrdiff_t>(plugins.size()))
		return (IPlugin*)plugins[idx];
	else
		return (IPlugin*)0;
}

struct SelectSignature { // : public std::unary_function<IPlugin*, std::pair<plcl::Plugin::SignatureInfo**, unsigned int> >
	typedef IPlugin* argument_type;
	typedef std::pair<plcl::Plugin::SignatureInfo**, unsigned int> result_type;

	result_type operator()(argument_type v) const {
		int signatures_ptr; DWORD n_signatures;
		THROW_FAILED(v->GetSignatures(&signatures_ptr, &n_signatures));

		return std::make_pair((plcl::Plugin::SignatureInfo**)signatures_ptr, static_cast<unsigned int>(n_signatures));
	}
};
template<class InputIteratorType, class SelectorType>
inline std::vector<typename SelectorType::result_type> Select(InputIteratorType head, InputIteratorType tail, SelectorType selector) {
	// projects InputIteratorType[head, tail) -> vector<SelectorType::result_type>
	std::vector<typename SelectorType::result_type> r;
	for (; head != tail; ++head) {
		r.push_back(selector(*head));
	}
	return r;
}
template<class ContainerType, class SelectorType>
inline std::vector<typename SelectorType::result_type> Select(ContainerType const &container, SelectorType selector) {
	return Select(container.begin(), container.end(), selector);
}

SignatureFilter* SignatureFilter::Create(std::vector<cpcl::ComPtr<IPlugin> > const &plugins_) {
	if (plugins_.empty()) {
		cpcl::Warning(cpcl::StringPieceFromLiteral("SignatureFilter::Create(): plugins list is empty"));
		return (SignatureFilter*)0;
	}
	std::auto_ptr<SignatureFilter> r(new SignatureFilter());
	SelectSignature::result_type v = plcl::merge_signatures(Select(plugins_, SelectSignature()));
	//SelectSignature::result_type v = plcl::merge_signatures(plugins_, SelectSignature()); // valid
	if ((!v.first) || (v.second < 1)) {
		cpcl::Warning(cpcl::StringPieceFromLiteral("SignatureFilter::Create(): no signatures to merge"));
		return (SignatureFilter*)0;
	}
	r->signatures = v.first;
	r->n_signatures = v.second;

	// calc max loading size
	r->max_data_offset = v.first[0]->offset + v.first[0]->size;
	for (unsigned int i = 1; i < v.second; ++i) {
		unsigned int max_data_offset_ = v.first[i]->offset + v.first[i]->size;
		if (r->max_data_offset < max_data_offset_)
			r->max_data_offset = max_data_offset_;
	}

	r->plugins.reserve(plugins_.size());
	r->indices.reserve(plugins_.size());
	unsigned int i(0);
	//SelectSignature selector;
	for (std::vector<cpcl::ComPtr<IPlugin> >::const_iterator plugins_i = plugins_.begin(); plugins_i != plugins_.end(); ++plugins_i) {
		//selector(*plugins_i);
		cpcl::ComPtr<IPlugin> plugin(*plugins_i);
		DWORD n_signatures;
		THROW_FAILED(plugin->GetSignatures(0, &n_signatures));

		r->plugins.push_back(plugin);
		r->indices.push_back(i += n_signatures);
	}
	return r.release();
}
