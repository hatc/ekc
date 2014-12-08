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
#include "CArchiveCallback.h"
#include <Windows/PropVariant.h>

#include "temporaryStream.h"
#include "CStreamWrapper.h"

namespace files {

//////////////////////////////////////////////////////////////
// Archive Open callback

STDMETHODIMP CArchiveOpenCallback::SetTotal(const UInt64 * /* files */, const UInt64 * /* bytes */)
{
  return S_OK; // progress->
}

STDMETHODIMP CArchiveOpenCallback::SetCompleted(const UInt64 * /* files */, const UInt64 * /* bytes */)
{
  return S_OK; // progress->
}
  
STDMETHODIMP CArchiveOpenCallback::CryptoGetTextPassword(BSTR * /* password */)
{
	return E_ABORT; // message->
}

//static const char *kTestingString    =  "Testing     ";
//static const char *kExtractingString =  "Extracting  ";
//static const char *kSkippingString   =  "Skipping    ";

//static const char *kUnsupportedMethod = "Unsupported Method";
//static const char *kCRCFailed = "CRC Failed";
//static const char *kDataError = "Data Error";
//static const char *kUnknownError = "Unknown Error";

//////////////////////////////////////////////////////////////
// Archive Extracting One Stream callback

void CArchiveExtractOneStreamCallback::Init(CInArchive *archiveHandler, UInt32 index, IStream *oneStream)
{
	archiveHandler_ = archiveHandler;
	extractResult = false;
	index_ = index; currentIndex_ = (UInt32)(Int32)(-1);
	oneStream_ = oneStream;
}

STDMETHODIMP CArchiveExtractOneStreamCallback::SetTotal(UInt64 /* size */)
{
  return S_OK; // progress->
}

STDMETHODIMP CArchiveExtractOneStreamCallback::SetCompleted(const UInt64 * /* completeValue */)
{
  return S_OK; // progress->
}

STDMETHODIMP CArchiveExtractOneStreamCallback::GetStream(UInt32 index,
    ISequentialOutStream **outStream, Int32 askExtractMode)
{
	currentIndex_ = index;
	if (askExtractMode != NArchive::NExtract::NAskMode::kExtract)
		return S_OK;
	if (index != index_)
		// log(Error) << "CArchiveExtractOneStreamCallback:: output stream requested for item with difference index";
		return E_FAIL;
	if ((extractResult) || (!oneStream_))
		// log(Error) << "CArchiveExtractOneStreamCallback:: output stream requested more that once";
		return E_ABORT;
	bool isDirectory(false);
	RINOK(archiveHandler_->itemIsDirectory(index, &isDirectory))
	if (isDirectory) {
		// log(Error) << "CArchiveExtractOneStreamCallback:: output stream requested for item witch actually is a directory";
		*outStream = 0;
		return S_OK;
	}

	CMyComPtr<ISequentialOutStream> outStream_ = new CStreamWrapper(static_cast<IStream*>(oneStream_));
	*outStream = outStream_.Detach();
  return S_OK;
}

STDMETHODIMP CArchiveExtractOneStreamCallback::PrepareOperation(Int32 askExtractMode)
{
  //extractResult = false;
  /*switch (askExtractMode)
  {
    case NArchive::NExtract::NAskMode::kExtract:  message-> kExtractingString ; break;
    case NArchive::NExtract::NAskMode::kTest:  message-> kTestingString ; break;
    case NArchive::NExtract::NAskMode::kSkip:  message-> kSkippingString ; break;
  };*/
  return S_OK;
}

STDMETHODIMP CArchiveExtractOneStreamCallback::SetOperationResult(Int32 operationResult)
{
	if (currentIndex_ != index_)
		return S_OK;

  switch(operationResult)
  {
		case NArchive::NExtract::NOperationResult::kOK:
			extractResult = true;
      break;
    default:
    {
      /*switch(operationResult)
      {
        case NArchive::NExtract::NOperationResult::kUnSupportedMethod:
          // message-> kUnsupportedMethod ;
          break;
        case NArchive::NExtract::NOperationResult::kCRCError:
          // message-> kCRCFailed ;
          break;
        case NArchive::NExtract::NOperationResult::kDataError:
          // message-> kDataError ;
          break;
        default:
          // message-> kUnknownError ;
      }*/
    }
  }
	oneStream_.Release();
  return S_OK;
}

STDMETHODIMP CArchiveExtractOneStreamCallback::CryptoGetTextPassword(BSTR *password)
{
	return E_ABORT; // message->
}

//////////////////////////////////////////////////////////////
// Archive Extracting callback

void CArchiveExtractCallback::Init(CInArchive *archiveHandler, CFileEnumeratorCallback *fileEnumeratorCallback, std::wstring &rootPath)
{
	archiveHandler_ = archiveHandler;
	stream_.Release(); itemInfo_.reset(); index_ = (UInt32)(Int32)(-1);
	fileEnumeratorCallback_ = fileEnumeratorCallback;
	rootPath_.swap(rootPath);
}

STDMETHODIMP CArchiveExtractCallback::SetTotal(UInt64 /* size */)
{
  return S_OK; // progress->
}

STDMETHODIMP CArchiveExtractCallback::SetCompleted(const UInt64 * /* completeValue */)
{
  return S_OK; // progress->
}

STDMETHODIMP CArchiveExtractCallback::GetStream(UInt32 index,
    ISequentialOutStream **outStream, Int32 askExtractMode)
{
	*outStream = 0;
	if (askExtractMode != NArchive::NExtract::NAskMode::kExtract)
		return S_OK;
	bool isDirectory(false);
	RINOK(archiveHandler_->itemIsDirectory(index, &isDirectory))
	if (isDirectory)
		return S_OK;

	UInt64 estimatedSizeByte = archiveHandler_->itemSizeByte(index);
	// RINOK(temporaryFileStream(estimatedSizeByte, &stream_))
	stream_.Release();
	RINOK(temporaryStream(estimatedSizeByte, &stream_))
	CMyComPtr<ISequentialOutStream> outStream_ = new CStreamWrapper(static_cast<IStream*>(stream_));
	*outStream = outStream_.Detach();
	index_ = index;
  return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::PrepareOperation(Int32 askExtractMode)
{
  /*switch (askExtractMode)
  {
    case NArchive::NExtract::NAskMode::kExtract:  message-> kExtractingString ; break;
    case NArchive::NExtract::NAskMode::kTest:  message-> kTestingString ; break;
    case NArchive::NExtract::NAskMode::kSkip:  message-> kSkippingString ; break;
  };*/
  return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::SetOperationResult(Int32 operationResult)
{
  /*switch(operationResult)
  {
		case NArchive::NExtract::NOperationResult::kOK:
			extractResult = true;
      break;
    default:
    {
      switch(operationResult)
      {
        case NArchive::NExtract::NOperationResult::kUnSupportedMethod:
          // message-> kUnsupportedMethod ;
          break;
        case NArchive::NExtract::NOperationResult::kCRCError:
          // message-> kCRCFailed ;
          break;
        case NArchive::NExtract::NOperationResult::kDataError:
          // message-> kDataError ;
          break;
        default:
          // message-> kUnknownError ;
      }
    }
  }*/
	if ((NArchive::NExtract::NOperationResult::kOK == operationResult) && (stream_)) {
		RINOK(archiveHandler_->itemInfo(index_, &itemInfo_))
		itemInfo_.stream = stream_;
		if (itemInfo_.size < 1) {
			STATSTG statstg;
			if (itemInfo_.stream->Stat(&statstg, 0) == S_OK) {
				itemInfo_.size = statstg.cbSize.QuadPart;
			}
		}
		itemInfo_.path.insert(0, rootPath_);
		fileEnumeratorCallback_->match(itemInfo_);
	}

	stream_.Release(); itemInfo_.reset(); index_ = (UInt32)(Int32)(-1);
  return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::CryptoGetTextPassword(BSTR *password)
{
	return E_ABORT; // message->
}

} // namespace files
