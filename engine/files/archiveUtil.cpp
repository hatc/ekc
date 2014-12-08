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
#include <vector>

#include <boost/scoped_ptr.hpp>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include "archiveUtil.h"

#include "CInArchiveImpl.h"
#include "CArchiveCallback.h"

#include "temporaryStream.h"

namespace files {

//HRESULT wipeOutArchive(CInArchive *archive, CFileEnumeratorCallback *fileEnumeratorCallback) {
HRESULT wipeOutArchive(Client7zHandler *handler, fileItemInfo &v, CFileEnumeratorCallback *fileEnumeratorCallback) {
	CInArchive *archive;
	RINOK(handler->openArchive(v, &archive))
	boost::scoped_ptr<CInArchive> archiveGuard(archive);

	CMyComPtr<CArchiveExtractCallback> archiveExtractCallback = new CArchiveExtractCallback(NULL);
	//CInArchiveImpl *archive_ = dynamic_cast<CInArchiveImpl*>(archive);
	//CMyComPtr<IInArchive> archiveHandler;
	//RINOK(archive_->getArchiveHandler(&archiveHandler))

	std::wstring rootPath(v.path); rootPath += L"\\";
	archiveExtractCallback->Init(archive, fileEnumeratorCallback, rootPath);
	//RINOK(archiveHandler->Extract(NULL, (UInt32)(Int32)(-1), 0, archiveExtractCallback))
	RINOK(archive->Extract(NULL, (UInt32)(Int32)(-1), 0, archiveExtractCallback))

	return S_OK;
}

class CFileItemInfoCopyCallback : public CFileEnumeratorCallback {
public:
	virtual __declspec(nothrow) ~CFileItemInfoCopyCallback() { 
		// printf("%s\n", "CFileItemInfoCopyCallback.dtor()");
	}
	virtual __declspec(nothrow) void match(files::fileItemInfo const &item) {
		fileEnumeratorCallback_->match(item);
	}

	CFileItemInfoCopyCallback(CFileEnumeratorCallback *fileEnumeratorCallback) : fileEnumeratorCallback_(fileEnumeratorCallback) {}
private:
	CFileEnumeratorCallback *fileEnumeratorCallback_;
};

HRESULT searchOutArchive(Client7zHandler *handler, fileItemInfo &v, CFileEnumeratorCallback *fileEnumeratorCallback, bool extractIfArchiveExtension) {
	CInArchive *archive;
	RINOK(handler->openArchive(v, &archive))
	boost::scoped_ptr<CInArchive> archiveGuard(archive);

	std::vector<UINT32> isArchiveExtensionIndices;
	UInt32 itemsCount(archive->itemsCount());
	fileItemInfo item;
	std::wstring rootPath(v.path); rootPath += L"\\";
	for (UInt32 i = 0; i < itemsCount; ++i) {
		RINOK(archive->itemInfo(i, &item))

		if ((extractIfArchiveExtension) && (handler->isArchiveExtension(item))) {
			isArchiveExtensionIndices.push_back(i);
		} else {
			item.path.insert(0, rootPath);
			fileEnumeratorCallback->match(item);
		}
	}

	if (!isArchiveExtensionIndices.empty()) {
		CFileItemInfoCopyCallback fileItemInfoCopyCallback(fileEnumeratorCallback);

		CMyComPtr<CArchiveExtractCallback> archiveExtractCallback = new CArchiveExtractCallback(NULL);
		archiveExtractCallback->Init(archive, &fileItemInfoCopyCallback, rootPath);

		RINOK(archive->Extract(&isArchiveExtensionIndices[0], isArchiveExtensionIndices.size(), 0, archiveExtractCallback))
	}

	return S_OK;
}

inline static size_t charMatches(wchar_t const *l, size_t ls, wchar_t const *r, size_t rs) {
	size_t s = (std::min)(ls, rs);
	size_t s_ = s;
	while ((s_ > 0) && (*l++ == *r++))
		--s_;
	return s - s_;
}
// if charMatches == virtualPath.lenght && ls == rs => match
// if charMatches == virtualPath.lenght && ls != rs => no result
// if charMatches < virtualPath.lenght => result = max(charMatches)

static HRESULT seekOutArchiveStep(Client7zHandler *handler, CArchiveExtractOneStreamCallback *archiveExtractOneStreamCallback, 
	fileItemInfo v, std::wstring virtualPath, fileItemInfo *r) {
	CInArchive *archive;
	RINOK(handler->openArchive(v, &archive))
	boost::scoped_ptr<CInArchive> archiveGuard(archive);

	//CInArchiveImpl *archive_ = dynamic_cast<CInArchiveImpl*>(archive);
	//CMyComPtr<IInArchive> archiveHandler;
	//RINOK(archive_->getArchiveHandler(&archiveHandler))
		
	UInt32 index(0), itemsCount(archive->itemsCount());
	UInt32 index_(itemsCount);
	fileItemInfo item;
	size_t matches(0);
	for (UInt32 i = 0; i < itemsCount; ++i) {
		RINOK(archive->itemInfo(i, &item))
		index = i;
			
		size_t matches_ = charMatches(virtualPath.c_str(), virtualPath.size(), item.path.c_str(), item.path.size());
		if (matches_ == virtualPath.size()) {
			if (virtualPath.size() == item.path.size()) {
				// match
				index_ = index;
				matches = matches_;
				break;
			}
		} else {
			if (matches_ > matches) {
				index_ = index;
				matches = matches_;
			}
		}
	}
	
	if (index_ == itemsCount) {
		return E_FAIL;
	} else {
		if (index != index_) {
			RINOK(archive->itemInfo(index_, &item))
		}
	}
	
	if (virtualPath.size() > matches) {
		if (virtualPath[matches] == L'\\')
			++matches;
	}
	virtualPath.erase(0, matches);
	
	if (!item.isDirectory()) {
		RINOK(temporaryStream(item.size, &item.stream))
		UInt32 archiveItem[1]; archiveItem[0] = index_;
		archiveExtractOneStreamCallback->Init(archive, index_, item.stream);
		//RINOK(archiveHandler->Extract(&archiveItem[0], 1, 0, archiveExtractOneStreamCallback))
		RINOK(archive->Extract(&archiveItem[0], 1, 0, archiveExtractOneStreamCallback))
		if (!archiveExtractOneStreamCallback->streamWriteOut())
			return E_FAIL;
	} else {
		if (!virtualPath.empty())
			return E_FAIL;
	}

	item.path.insert(0, 1, L'\\');
	item.path.insert(0, v.path);

	if (virtualPath.empty()) {
		if (r)
			*r = item;
		return S_OK;
	}
		
	return seekOutArchiveStep(handler, archiveExtractOneStreamCallback, item, virtualPath, r);
}

HRESULT seekOutArchive(Client7zHandler *handler, fileItemInfo v, std::wstring virtualPath, fileItemInfo *r) {
	if (virtualPath.empty())
		return E_INVALIDARG;

	CMyComPtr<CArchiveExtractOneStreamCallback> archiveExtractOneStreamCallback = new CArchiveExtractOneStreamCallback(NULL);
	return seekOutArchiveStep(handler, archiveExtractOneStreamCallback, v, virtualPath, r);
	// std::stack<fileItemInfo> items; items.push(v);
}

} // namespace files
