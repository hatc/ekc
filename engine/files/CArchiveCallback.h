// CArchiveCallback.h
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

#ifndef __FILES_CARCHIVECALLBACK_H
#define __FILES_CARCHIVECALLBACK_H

#include "stdafx.h"

#include <Common/MyCom.h>

#include <7zip/Archive/IArchive.h>
#include <7zip/IPassword.h>

#include "CFileEnumeratorCallback.h"
#include "CInArchive.h"

namespace files {

/*ReportProgress([in] BSTR majorInfo, [in] BSTR minorInfo,
  [in] long majorItemsTotal,
  [in] long majorItemsDone,
  [in] long minorItemsTotal,
  [in] long minorItemsDone,
  [in, out] VARIANT_BOOL *shouldTerminate);

CArchiveOpenCallback::SetTotal(const UInt64 *files, const UInt64 *) {
 files_ = (files) ? *files : 0;
 messageSink_->ReportProgress(0, 0, LONG_MAX, filesFound_, files_, 0);
}
CArchiveOpenCallback::SetCompleted(const UInt64 *files, const UInt64 *) {
 messageSink_->ReportProgress(0, 0, LONG_MAX, filesFound_, files_, (files) ? *files : 0);
}
CArchiveOpenCallback::CryptoGetTextPassword(BSTR *) {
 messageSink_->ReportProgress(itemPath, "password protected archive", LONG_MAX, filesFound_, 0, 0);
}*/

class CArchiveOpenCallback:
  public IArchiveOpenCallback,
  public ICryptoGetTextPassword,
  public CMyUnknownImp
{
public:
	CArchiveOpenCallback(IUnknown *messageSink, UInt64 filesFound) : messageSink_(messageSink), filesFound_(filesFound), files_(0) {}

	// IArchiveOpenCallback
  STDMETHOD(SetTotal)(const UInt64 *files, const UInt64 *bytes);
  STDMETHOD(SetCompleted)(const UInt64 *files, const UInt64 *bytes);

	// ICryptoGetTextPassword
  STDMETHOD(CryptoGetTextPassword)(BSTR *password);

	MY_UNKNOWN_IMP1(ICryptoGetTextPassword)
private:
	CMyComPtr<IUnknown> messageSink_;
	UInt64 files_;
	//UInt64 bytes_;
	UInt64 filesFound_; //fileListSize
};
/*CArchiveOpenCallback call graph:
 IInArchive->Open(stream, 0, CArchiveOpenCallback)
	?CryptoGetTextPassword
  SetTotal
	SetCompleted*/

class CArchiveExtractOneStreamCallback:
  public IArchiveExtractCallback,
  public ICryptoGetTextPassword,
  public CMyUnknownImp
{
public:
	CArchiveExtractOneStreamCallback(IUnknown *messageSink) : extractResult(false), messageSink_(messageSink) {}

	void Init(CInArchive *archiveHandler, UInt32 index_, IStream *oneStream);
	bool streamWriteOut() const { return extractResult; }

  // IArchiveExtractCallback::IProgress
  STDMETHOD(SetTotal)(UInt64 size);
  STDMETHOD(SetCompleted)(const UInt64 *completeValue);

  // IArchiveExtractCallback
  STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream **outStream, Int32 askExtractMode);
  STDMETHOD(PrepareOperation)(Int32 askExtractMode);
  STDMETHOD(SetOperationResult)(Int32 resultEOperationResult);

  // ICryptoGetTextPassword
  STDMETHOD(CryptoGetTextPassword)(BSTR *password);

	MY_UNKNOWN_IMP1(ICryptoGetTextPassword)
private:
	CInArchive *archiveHandler_;
  bool extractResult;
	UInt32 index_;
	UInt32 currentIndex_;
	CMyComPtr<IStream> oneStream_;
	CMyComPtr<IUnknown> messageSink_;
};
/*CArchiveExtractOneStreamCallback call graph:
 CArchiveExtractOneStreamCallback->Init(archive, stream)
 Uint32 archiveItem[1]; archiveItem[0] = index;
 IInArchive->Extract(&archiveItem[0], 1, 0, CArchiveExtractOneStreamCallback);
	?CryptoGetTextPassword
  SetTotal
	SetCompleted
	PrepareOperation
	GetStream
	SetOperationResult*/

class CArchiveExtractCallback:
  public IArchiveExtractCallback,
  public ICryptoGetTextPassword,
  public CMyUnknownImp
{
public:
  CArchiveExtractCallback(IUnknown *messageSink) : messageSink_(messageSink) {}

	void Init(CInArchive *archiveHandler_, CFileEnumeratorCallback *fileEnumeratorCallback, std::wstring &rootPath);

  // IArchiveExtractCallback::IProgress
  STDMETHOD(SetTotal)(UInt64 size);
  STDMETHOD(SetCompleted)(const UInt64 *completeValue);

  // IArchiveExtractCallback
  STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream **outStream, Int32 askExtractMode);
  STDMETHOD(PrepareOperation)(Int32 askExtractMode);
  STDMETHOD(SetOperationResult)(Int32 resultEOperationResult);

  // ICryptoGetTextPassword
  STDMETHOD(CryptoGetTextPassword)(BSTR *password);

	MY_UNKNOWN_IMP1(ICryptoGetTextPassword)
private:
  // CMyComPtr<IInArchive> archiveHandler_;
	CInArchive *archiveHandler_;
	CMyComPtr<IUnknown> messageSink_;
	scopedComPtr<IStream> stream_;
	UInt32 index_;
	CFileEnumeratorCallback *fileEnumeratorCallback_;
	fileItemInfo itemInfo_;
	std::wstring rootPath_;
};
/*CArchiveExtractCallback real call graph:
Init
SetTotal
SetCompleted
GetStream()
PrepareOperation
SetCompleted
SetCompleted
SetOperationResult

CArchiveExtractCallback call graph:
 CArchiveExtractCallback->Init(archive, stream)
 IInArchive->Extract(NULL, (UInt32)(Int32)(-1), 0, CArchiveOpenCallback);
	?CryptoGetTextPassword
  SetTotal
	SetCompleted
	foreach item in archive: // include directories???
	 PrepareOperation
	 GetStream
	 SetOperationResult*/

} // namespace files

#endif // __FILES_CARCHIVECALLBACK_H
