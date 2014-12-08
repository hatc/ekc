// CInArchive.h
// Copyright (C) 2011 Yuri Agafonov
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

#ifndef __FILES_CINARCHIVE_H
#define __FILES_CINARCHIVE_H

#include "fileItemInfo.hpp"

struct IArchiveExtractCallback;

namespace files {

class CInArchive {
public:
	virtual __declspec(nothrow) ~CInArchive() {}

	virtual __declspec(nothrow) UINT64 itemSizeByte(UINT32 index) = 0;
	virtual __declspec(nothrow) UINT32 itemsCount() = 0;
	// virtual __declspec(nothrow) fileItemInfo itemInfo(UINT32 index) = 0;
	// virtual __declspec(nothrow) fileItemInfo* itemInfo(UINT32 index, fileItemInfo *r);
	virtual __declspec(nothrow) HRESULT itemIsDirectory(UINT32 index, bool *r) = 0;
	virtual __declspec(nothrow) HRESULT itemPath(UINT32 index, wchar_t const **r, size_t *rSize) = 0;
	virtual __declspec(nothrow) HRESULT itemInfo(UINT32 index, fileItemInfo *r) = 0;
	/*
	indices must be sorted
  numItems = 0xFFFFFFFF means "all files"
  testMode != 0 means "test files without writing to outStream"
	*/
	virtual __declspec(nothrow) HRESULT Extract(const UINT32 *indices, UINT32 numItems, INT32 testMode, IArchiveExtractCallback *extractCallback) = 0;
	
};

} // namespace files

#endif // __FILES_CINARCHIVE_H
