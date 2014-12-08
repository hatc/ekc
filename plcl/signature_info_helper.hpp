// signature_info_helper.hpp
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

#include <exception>
#include <utility>

#include "plugin.h"

#ifdef SIGNATURE_INFO
#error SIGNATURE_INFO macro name already used
#endif
#ifdef SIGNATURE_INFO_OFFSET
#error SIGNATURE_INFO_OFFSET macro name already used
#endif

#define SIGNATURE_INFO(si) { sizeof(si), 0, si }
#define SIGNATURE_INFO_OFFSET(si, offset) { sizeof(si), offset, si }

namespace plcl {

inline Plugin::SignatureInfo** make_signatures(Plugin::SignatureInfo const *signatures_, unsigned int n_signatures) {
	size_t size(0);
	for (unsigned int i = 0; i < n_signatures; ++i) {
		size += signatures_[i].size;
	}
	// first variant:
	// pointer array | (unsigned int * 2 + data) array
	// but SignatureInfo - struct may be padded, so more safe variant: 
	// pointer array | struct array | data, i.e
	// [SignatureInfo* ...] [SignatureInfo] [signature]
	// [SignatureInfo] offset = n * sizeof(SignatureInfo*), [signature] offset = n * (sizeof(SignatureInfo) + sizeof(SignatureInfo*))
	// pointer array obviously unused overhead
	size += n_signatures * (sizeof(Plugin::SignatureInfo*) + sizeof(Plugin::SignatureInfo));
	if (0 == size)
		return 0;

	Plugin::SignatureInfo **signatures = (Plugin::SignatureInfo**)malloc(size);
	if (!signatures)
		throw std::bad_alloc();
	Plugin::SignatureInfo *signature = (Plugin::SignatureInfo*)(signatures + n_signatures);
	unsigned char *data = (unsigned char*)(signature + n_signatures);
	for (unsigned int i = 0; i < n_signatures; ++i) {
		(*signature).size = signatures_[i].size;
		(*signature).offset = signatures_[i].offset;
		(*signature).data = data;

		signatures[i] = signature++;

		memcpy(data, signatures_[i].data, signatures_[i].size);
		data += signatures_[i].size;
	}

	return signatures;
}

template<class ForwardIteratorType/* pair<SignatureInfo**, unsigned int> ForwardIteratorType::value_type */>
inline std::pair<Plugin::SignatureInfo **, unsigned int> merge_signatures(ForwardIteratorType head, ForwardIteratorType tail) {
	size_t size(0);
	unsigned int n_signatures(0); 
	for (ForwardIteratorType si = head; si != tail; ++si) {
		n_signatures += (*si).second;
		for (unsigned int j = 0; j < (*si).second; ++j) {
			size += (*si).first[j]->size;
		}
	}

	size += n_signatures * (sizeof(Plugin::SignatureInfo*) + sizeof(Plugin::SignatureInfo));
	if (0 == size)
		return std::make_pair((Plugin::SignatureInfo **)0, size);
	Plugin::SignatureInfo **r = (Plugin::SignatureInfo**)malloc(size);
	if (!r)
		throw std::bad_alloc();
	
	Plugin::SignatureInfo **signatures = r;
	Plugin::SignatureInfo *signature = (Plugin::SignatureInfo*)(signatures + n_signatures);
	unsigned char *data = (unsigned char*)(signature + n_signatures);
	for (ForwardIteratorType si = head; si != tail; ++si) {
		for (unsigned int j = 0; j < (*si).second; ++j) {
			(*signature).size = (*si).first[j]->size;
			(*signature).offset = (*si).first[j]->offset;
			(*signature).data = data;

			*signatures++ = signature++;
			
			memcpy(data, (*si).first[j]->data, (*si).first[j]->size);
			data += (*si).first[j]->size;
		}
	}
	return std::make_pair(r, n_signatures);
}

template<class ContainerType/* pair<SignatureInfo**, unsigned int> ContainerType::value_type */>
inline std::pair<Plugin::SignatureInfo **, unsigned int> merge_signatures(ContainerType const &container) {
	return merge_signatures(container.begin(), container.end());
}

template<class ForwardIteratorType, typename SelectorType/* projects(select) ForwardIteratorType -> pair<SignatureInfo**, unsigned int> */>
inline std::pair<Plugin::SignatureInfo**, unsigned int> merge_signatures(ForwardIteratorType head, ForwardIteratorType tail, SelectorType selector) {
	size_t size(0);
	unsigned int n_signatures(0); 
	for (ForwardIteratorType i = head; i != tail; ++i) {
		std::pair<Plugin::SignatureInfo**, unsigned int> si = selector(i);
		n_signatures += si.second;
		for (unsigned int j = 0; j < si.second; ++j) {
			size += si.first[j]->size;
		}
	}

	size += n_signatures * (sizeof(Plugin::SignatureInfo*) + sizeof(Plugin::SignatureInfo));
	Plugin::SignatureInfo **r = (Plugin::SignatureInfo**)malloc(size);
	if (!r)
		throw std::bad_alloc();
	
	Plugin::SignatureInfo **signatures = r;
	Plugin::SignatureInfo *signature = (Plugin::SignatureInfo*)(signatures + n_signatures);
	unsigned char *data = (unsigned char*)(signature + n_signatures);
	for (ForwardIteratorType i = head; i != tail; ++i) {
		std::pair<Plugin::SignatureInfo**, unsigned int> si = selector(i);
		for (unsigned int j = 0; j < si.second; ++j) {
			(*signature).size = si.first[j]->size;
			(*signature).offset = si.first[j]->offset;
			(*signature).data = data;

			*signatures++ = signature++;
			
			memcpy(data, si.first[j]->data, si.first[j]->size);
			data += si.first[j]->size;
		}
	}
	return std::make_pair(r, n_signatures);
}

template<class ContainerType, typename SelectorType/* projects(select) ForwardIteratorType -> pair<SignatureInfo**, unsigned int> */>
inline std::pair<Plugin::SignatureInfo**, unsigned int> merge_signatures(ContainerType const &container, SelectorType selector) {
	return merge_signatures(container.begin(), container.end(), selector);
}

} // namespace plcl
