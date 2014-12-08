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

#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <memory>

#include "signature_info_helper.hpp"

#include <cassert>

typedef plcl::Plugin::SignatureInfo SignatureInfo;

template<size_t N>
inline void test_make_signatures(SignatureInfo **v, SignatureInfo const (&signatures)[N]) {
	for (size_t i = 0; i < N; ++i, ++v) {
		assert(signatures[i].size == (*v)->size);
		assert(signatures[i].offset == (*v)->offset);
		assert(memcmp(signatures[i].data, (*v)->data, signatures[i].size) == 0);
	}
}
template<class InputIteratorType>
inline void test_merge_signatures(std::pair<SignatureInfo**, unsigned int> signatures, InputIteratorType head, InputIteratorType tail) {
	unsigned int size(0);
	SignatureInfo **si = signatures.first;
	for (InputIteratorType i = head; i != tail; ++i) {
		for (unsigned int j = 0; j < (*i).second; ++j, ++si) {
			assert((*si)->size == (*i).first[j]->size);
			assert((*si)->offset == (*i).first[j]->offset);
			assert(memcmp((*si)->data, (*i).first[j]->data, (*i).first[j]->size) == 0);
		}
		size += (*i).second;
	}
	assert(size == signatures.second);
}

static unsigned char const jif0_signature[] = { 0xFF, 0xD8, 0xFF, 0xE0 };
static unsigned char const jif1_signature[] = { 0xE0, 0xF4, 0x10, 0x34, 0x56, 0x78 };
static unsigned char const img0_signature[] = { 0xC0, 0xCD, 0xEF };
static unsigned char const img1_signature[] = { 0xFF, 0xAB, 0xCD, 0x01 };
void test() {
	SignatureInfo const jfif_signatures[] = {
		SIGNATURE_INFO(jif0_signature),
		SIGNATURE_INFO_OFFSET(jif1_signature, 0x10)
	};
	SignatureInfo const img_signatures[] = {
		SIGNATURE_INFO_OFFSET(img0_signature, 0x30),
		SIGNATURE_INFO(img1_signature)
	};

	SignatureInfo **jfif = plcl::make_signatures(jfif_signatures, arraysize(jfif_signatures));
	test_make_signatures(jfif, jfif_signatures);
	SignatureInfo **jfif_copy = plcl::make_signatures(*jfif, arraysize(jfif_signatures));
	test_make_signatures(jfif_copy, jfif_signatures);

	SignatureInfo **img = plcl::make_signatures(img_signatures, arraysize(img_signatures));
	test_make_signatures(img, img_signatures);
	SignatureInfo **img_copy = plcl::make_signatures(*img, arraysize(img_signatures));
	test_make_signatures(img_copy, img_signatures);

	std::pair<SignatureInfo**, unsigned int> signatures_[2];
	signatures_[0].first = jfif, signatures_[0].second = arraysize(jfif_signatures);
	signatures_[1].first = img, signatures_[1].second = arraysize(img_signatures);
	std::pair<SignatureInfo**, unsigned int> signatures = plcl::merge_signatures(signatures_, signatures_ + arraysize(signatures_));
	test_merge_signatures(signatures, signatures_, signatures_ + arraysize(signatures_));

	free(jfif);
	free(img);
}
