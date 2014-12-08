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

#include <dumbassert.h>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include "CStreamWrapper.h"

namespace files {

CStreamWrapper::CStreamWrapper(IStream *stream) : stream_(stream) {
	DUMBASS_CHECK(stream);
}

STDMETHODIMP CStreamWrapper::Write(const void *data, UInt32 size, UInt32 *processedSize) {
	ULONG processedSize_;
	HRESULT result = stream_->Write(data, static_cast<ULONG>(size), &processedSize_);

  if (processedSize != NULL)
    *processedSize = processedSize_;
  return result;
}

STDMETHODIMP CStreamWrapper::Seek(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition) {
	if (seekOrigin > 2)
    return STG_E_INVALIDFUNCTION;

	LARGE_INTEGER offset_; offset_.QuadPart = offset;
  ULARGE_INTEGER newPosition_;
	HRESULT result = stream_->Seek(offset_, seekOrigin, &newPosition_);

  if (newPosition != NULL)
		*newPosition = newPosition_.QuadPart;
  return result;
}

STDMETHODIMP CStreamWrapper::SetSize(UInt64 newSize) {
	ULARGE_INTEGER newSize_; newSize_.QuadPart = newSize;
	return stream_->SetSize(newSize_);
}

STDMETHODIMP CStreamWrapper::Read(void *data, UInt32 size, UInt32 *processedSize) {
	ULONG processedSize_;
	HRESULT result = stream_->Read(data, static_cast<ULONG>(size), &processedSize_);

  if (processedSize != NULL)
    *processedSize = processedSize_;
  return result;
}

STDMETHODIMP CStreamWrapper::GetSize(UInt64 *size) {
	STATSTG stat;
	RINOK(stream_->Stat(&stat, STATFLAG_NONAME))
	if (size != NULL)
		*size = stat.cbSize.QuadPart;
	return S_OK;
}

} // namespace files
