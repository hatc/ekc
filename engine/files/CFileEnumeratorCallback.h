// CFileEnumeratorCallback.h
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

#ifndef __FILES_CFILEENUMERATORCALLBACK_H
#define __FILES_CFILEENUMERATORCALLBACK_H

#include "fileItemInfo.hpp"

// (IFileEnumerator as IPropertyBasket)[kpidFiltersUseStreams] - use as hint in FindFiles: 
//   if kpidFiltersUseStreams:
//     CArchiveExtractCallback.Init(archive, CFileEnumeratorCallback)
//     if (archive->Extract(NULL, (UInt32)(Int32)(-1), 0, CArchiveExtractCallback) != S_OK):
//       messageSink-> "unable check files in archive " + item_.path;
//     fileContainers.add(CArchiveExtractCallback->containers_);
//   else: 
//     for item in archiveFileIterator(item_):
//       if item.isDirectory:
//         fileContainers.add(item)
//       else:
//         if filterList.Match(item)
//           fileList.add(item)
//         if archiveExtension_hash_set.contain(item.fileExtension):
//           fileContainers.add(item)

namespace files {

/* stack allocated instance of CFileEnumeratorCallbackImpl create in findImpl method of CFileEnumerator*/
class CFileEnumeratorCallback {
public:
	virtual __declspec(nothrow) ~CFileEnumeratorCallback() {}
	/*if IFilterList::Match(fileItem):
	 IFileList::Add(new CFileItem(fileItem))
	 fileContainers.add(fileItem) - i.e. copy add to fileContainers copy of fileItem and fileItem.stream.AddRef() - this stream used in findFile later*/
	virtual __declspec(nothrow) void match(fileItemInfo const &item) = 0;
//impl public:
//	void init(*filterList, *fileList, &fileContainers);
//impl private:
//	filterList;
//	fileList;
};

} // namespace files

#endif // __FILES_CFILEENUMERATORCALLBACK_H
