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

#include <stack>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include "simpleFileEnumerator.h"

#include "simpleFileItems.h"
// #include "simpleFileList.h"
#include "ifilelist_impl.h"
// #include "simpleFilterList.h"
#include "ifilefilterlist_impl.h"

#include "files/fileUtil.h"
#include "files/fileIterator.hpp"
#include "files/temporaryStream.h"
#include "files/archiveUtil.h"
#include "files/CFileEnumeratorCallback.h"

#include "files/sharedIconMap.h"

#include <psapi.h> // GetProcessMemoryInfo
#include <commctrl.h>

extern HIMAGELIST g_ImageList;

HICON getIconFromGlobalImageList(int iconID) {
	return (g_ImageList) ? ::ImageList_GetIcon(g_ImageList, iconID, ILD_NORMAL) : (HICON)NULL;
}

typedef std::stack<files::fileItemInfo> stack_t;

size_t PagefileUsageMib() {
	HANDLE hProcess = ::GetCurrentProcess();
	
	PROCESS_MEMORY_COUNTERS pmc;
	if (::GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
		return (pmc.PagefileUsage / (1024 * 1024));
	}
	return 0x401;
}

class CFileEnumeratorCallbackImpl : public files::CFileEnumeratorCallback {
public:
	virtual __declspec(nothrow) ~CFileEnumeratorCallbackImpl() { 
		// printf("%s\n", "CFileEnumeratorCallbackImpl.dtor()");
		::SysFreeString(item_tip_);
	}
	/*if IFilterList::Match(fileItem):
	 IFileList::Add(new CFileItem(fileItem))
	 fileContainers.add(fileItem) - i.e. copy add to fileContainers copy of fileItem and fileItem.stream.AddRef() - this stream used in findFile later*/
	virtual __declspec(nothrow) void match(files::fileItemInfo const &item);

	CFileEnumeratorCallbackImpl(stack_t *fileContainers, IFileList *files, IFileFilterList *filters, files::Client7zHandler *client7zHandler, IFileEnumeratorProgressEvents *sink) 
		: fileContainers_(fileContainers), files_(files), filters_(filters), client7zHandler_(client7zHandler), sink_(sink), berserk_mode(false) {
		item_tip_ = ::SysAllocString(L"");
	}

	inline bool reportProgress(files::fileItemInfo const &item);
private:
	stack_t *fileContainers_;
	ScopedComPtr<IFileList> files_;
	ScopedComPtr<IFileFilterList> filters_;
	IFileEnumeratorProgressEvents *sink_;

	files::Client7zHandler *client7zHandler_;

	bool berserk_mode;
	BSTR item_tip_;
};

inline static bool reportProgress(IFileEnumeratorProgressEvents *fileProgressSink, IFileList *files, files::fileItemInfo const &item, BSTR item_tip) {
	bool r(false);
	if (fileProgressSink) {
		r = true;
		BSTR item_uri = ::SysAllocString(item.path.c_str());
		long fileListSize;
		if SUCCEEDED(files->get_Count(&fileListSize)) {
			VARIANT_BOOL b(VARIANT_FALSE);
			if SUCCEEDED(fileProgressSink->ReportProgress(item_uri, item_tip, LONG_MAX, fileListSize, 0, 0, &b)) {
				r = (b != VARIANT_FALSE);
			}
		}
		
		::SysFreeString(item_uri);
	}
	return r;
}
inline static bool reportItemFound(IFileEnumeratorProgressEvents *fileProgressSink, IFileItem *fileItem, files::fileItemInfo const &item, BSTR item_tip) {
	bool r(false);
	if (fileProgressSink) {
		r = true;
		BSTR item_uri = ::SysAllocString(item.path.c_str());
		
		VARIANT_BOOL b(VARIANT_FALSE);
		if SUCCEEDED(fileProgressSink->ReportItemFound(item_uri, fileItem, item_tip, &b)) {
			r = (b != VARIANT_FALSE);
		}
		
		::SysFreeString(item_uri);
	}
	return r;
}

inline bool CFileEnumeratorCallbackImpl::reportProgress(files::fileItemInfo const &item) {
	return ::reportProgress(sink_, files_, item, item_tip_);
}

void CFileEnumeratorCallbackImpl::match(files::fileItemInfo const &item) {
	if (berserk_mode) {
		while (!(*fileContainers_).empty())
			(*fileContainers_).pop();
		return;
	}

	if ((item.isDirectory()) || (client7zHandler_->isArchiveExtension(item)))
		(*fileContainers_).push(item);

	berserk_mode = ::reportProgress(sink_, files_, item, item_tip_);

	// method also can be called without archive:
	// fileEnumeratorCallbackImpl.match(*fileIt);
	ScopedComPtr<simpleFileItem> fileItem = new simpleFileItem();
	fileItem->initProperties(item);
	HRESULT r(S_OK);
	//if (!item.isDirectory()) {
	if (!item.isDirectory() && !berserk_mode) {
		if (filters_) {
			r = filters_->Match(fileItem);
			if ((r != S_OK) && (r != S_FALSE))
				return;
		}
	} else
		r = S_FALSE;

	if (r == S_OK) {
	 	files_->AddItem(fileItem, NULL);
		// check virtual memory
		if (PagefileUsageMib() > 1024)
			berserk_mode = true;
		else
			berserk_mode = reportItemFound(sink_, fileItem, item, item_tip_);
	}
	//if (r == S_OK) {
	// 	files_->AddItem(fileItem, NULL);
	//	// check virtual memory
	//	if (PagefileUsageMib() > 1024)
	//		berserk_mode = true;
	//	else {
	//		berserk_mode = ::reportProgress(sink_, files_, item, item_tip_);
	//		if (!berserk_mode)
	//			berserk_mode = reportItemFound(sink_, fileItem, item, item_tip_);
	//	}
	//} else {
	//	berserk_mode = ::reportProgress(sink_, files_, item, item_tip_);
	//}

	if (berserk_mode) {
		while (!(*fileContainers_).empty())
			(*fileContainers_).pop();
	}
}

STDMETHODIMP simpleFileEnumerator::FindFromPath(BSTR rootPath, IFileList **files) {
	try {
		if (!files)
			return E_POINTER;
		*files = 0;

		RINOK(client7zHandler->load7zHandler())
			
		stack_t itemStack;
		files::fileIterator fileItTail;
		//ScopedComPtr<simpleFileList> files_ = new simpleFileList();
		ScopedComPtr<IFileList> files_ = new IFileListImpl();
		CFileEnumeratorCallbackImpl fileEnumeratorCallbackImpl(&itemStack, files_, fileFiltersList, client7zHandler, fileProgressSink);
		
		std::wstring d,r,v;
		if (!files::formatPath(rootPath, ::SysStringLen(rootPath), &d, &r, &v))
			return E_INVALIDARG;
		if (!v.empty())
			return E_INVALIDARG;
		
		bool searchOutArchive(false);
		{
			itemBasket_const_i i = itemBasket.find(KPID_PROCESS_ALL_STREAMS_IN_ARCHIVE);
			if (itemBasket.end() != i) {
				comUtility::PropVariantRenamed *item = (*i).second;
				searchOutArchive = ((VT_BOOL == item->vt) && (VARIANT_TRUE == item->boolVal));
			}
		}
		
		if (r.empty()) {
			if ((*--d.end()) == L'\\')
				r = d + L"*";
			else
				r = d + L"\\*";
			
			files::fileIterator fileIt_(r);
			while (fileIt_ != fileItTail) {
				fileEnumeratorCallbackImpl.match(*fileIt_);
				++fileIt_;
			}
		} else {
			files::fileIterator fileIt_(r);
			if (fileIt_ == fileItTail)
				return E_UNEXPECTED;
			
			files::fileItemInfo fileInfo_ = *fileIt_;
			fileInfo_.path = r;
			
			if (fileInfo_.isDirectory())
				itemStack.push(fileInfo_);
			else {
				fileEnumeratorCallbackImpl.match(fileInfo_);
			}
		}
		
		while (!itemStack.empty()) {
			files::fileItemInfo fileInfo_ = itemStack.top();
			itemStack.pop();
			
			if (fileInfo_.isDirectory()) {
				files::fileIterator fileIt(fileInfo_.path + L"\\*");
				while (fileIt != fileItTail) {
					fileEnumeratorCallbackImpl.match(*fileIt);
					++fileIt;
				}
			} else { // archive && (client7zHandler->isArchiveExtension(item) == true)
				if (fileEnumeratorCallbackImpl.reportProgress(fileInfo_))
					break;
				
				if (searchOutArchive)
					files::searchOutArchive(client7zHandler, fileInfo_, &fileEnumeratorCallbackImpl);
				else
					files::wipeOutArchive(client7zHandler, fileInfo_, &fileEnumeratorCallbackImpl);
			}
		}
		
		*files = files_.Detach();
		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}
STDMETHODIMP simpleFileEnumerator::FindFromItem(IFileItem *file, IFileList **files) {
	try {
		if (!file)
			return E_INVALIDARG;
		
		ScopedComPtr<IFileItem> item(file);
		BSTR path;
		RINOK(item->get_Path(&path))
		HRESULT hr = FindFromPath(path, files);
		::SysFreeString(path);
		return hr;
	} catch (...) {
		return E_FAIL;
	}
}
STDMETHODIMP simpleFileEnumerator::Path2File(BSTR filePath, IFileItem **file) {
	try {
		if (!file)
			return E_POINTER;
		*file = 0;

		RINOK(client7zHandler->load7zHandler())
		files::fileItemInfo itemInfo;
		
		std::wstring d,r,v;
		if (!files::formatPath(filePath, ::SysStringLen(filePath), &d, &r, &v))
			return E_INVALIDARG;
		if (r.empty())
			return E_INVALIDARG;
		if (v.empty()) {
			files::fileIterator fileIt(r), fileItTail;
			if (fileIt != fileItTail) {
				itemInfo = *fileIt;
				itemInfo.path = r;
			}
		} else {
			files::fileIterator fileIt(r), fileItTail;
			if (fileIt != fileItTail) {
				if ((*fileIt).isDirectory())
					return E_UNEXPECTED;
				
				itemInfo = *fileIt;
				itemInfo.path = r;
				
				if (client7zHandler->isArchiveExtension(itemInfo)) {
					files::fileItemInfo r_;
					if SUCCEEDED(files::seekOutArchive(client7zHandler, itemInfo, v, &r_)) {
						itemInfo = r_;
					}
				}
			}
		}

		if (!itemInfo)
			return E_FAIL;
		
		ScopedComPtr<simpleFileItem> file_ = new simpleFileItem();
		file_->initProperties(itemInfo);
		*file = file_.Detach();
		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}
STDMETHODIMP simpleFileEnumerator::ItemParent(IFileItem * /*child*/, IFileItem ** /*parent*/) {
	return E_NOTIMPL;
}
STDMETHODIMP simpleFileEnumerator::get_Filters(IFileFilterList **filters) {
	try {
		if (!filters)
			return E_INVALIDARG;
		*filters = 0;

		if (!fileFiltersList)
			// fileFiltersList = new simpleFilterList();
			fileFiltersList = new IFileFilterListImpl();
		ScopedComPtr<IFileFilterList> filters_(fileFiltersList);
		*filters = filters_.Detach();
		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}
STDMETHODIMP simpleFileEnumerator::get_Params(IPropertyBasket **enumProperties) {
	try {
		if (!enumProperties)
			return E_POINTER;
		*enumProperties = 0;
		
		ScopedComPtr<IPropertyBasket> v(this);
		*enumProperties = v.Detach();
		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}
STDMETHODIMP simpleFileEnumerator::get_IconRetriever(IIconRetriever **iconRetriever) {
	try {
		if (!iconRetriever)
			return E_POINTER;
		*iconRetriever = 0;
		
		ScopedComPtr<IIconRetriever> v(this);
		*iconRetriever = v.Detach();
		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}
STDMETHODIMP simpleFileEnumerator::SetProgressEventSink(IFileEnumeratorProgressEvents *sink) {
	try {
		if (fileProgressSink) {
			fileProgressSink->Release();
			fileProgressSink = NULL;
		}
		fileProgressSink = sink;
		if (fileProgressSink) {
			fileProgressSink->AddRef();
		}
		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}

/* IIconRetriever */
STDMETHODIMP simpleFileEnumerator::FindIcon(int iconIndex, int *hIcon) {
	try {
		if (!hIcon)
			return E_POINTER;

		*hIcon = (int)getIconFromGlobalImageList(iconIndex);
		return S_OK;
	} catch (...) {
		return E_FAIL;
	}
}
