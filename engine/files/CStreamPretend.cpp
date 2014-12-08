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

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2

#include "CStreamPretend.h"

namespace files {

STDMETHODIMP COutStreamPretend::Write(const void * /*data*/, UInt32 size, UInt32 *processedSize) {
	size_ += size; offset_ += size;
  if (processedSize != NULL)
    *processedSize = size;
  return S_OK;
}
#define I_AM_STUPID_OTAKU E_FAIL 
STDMETHODIMP COutStreamPretend::Seek(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition) {
	if (seekOrigin > 2)
    return STG_E_INVALIDFUNCTION;

	return I_AM_STUPID_OTAKU;
	/*op = (offset > 0) ? std::greater : std::less;
	offset = offset * (-1);
	if (op(offset, offset_) < size_)
		return STG_E_SEEKERROR;*/

  if (newPosition != NULL)
		*newPosition = offset_;
  return S_OK;
}

STDMETHODIMP COutStreamPretend::SetSize(UInt64 newSize) {
	size_ = newSize;
	if (offset_ > size_)
		offset_ = size_;
	return S_OK;
}

STDMETHODIMP CSequentialOutStreamPretend::Write(const void * /*data*/, UInt32 size, UInt32 *processedSize) {
  if (processedSize != NULL)
    *processedSize = size;
  return S_OK;
}

} // namespace files
