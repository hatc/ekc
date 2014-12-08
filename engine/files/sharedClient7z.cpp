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
#include "sharedClient7z.h"

namespace files {

static Client7zHandler *sharedClient7zHandler = NULL;

HRESULT createSharedClient7z() {
	if (sharedClient7zHandler != NULL)
		return S_OK;

	Client7zHandler *sharedClient7zHandler_ = new Client7zHandler(NULL);
	HRESULT hr = sharedClient7zHandler_->load7zHandler();
	if SUCCEEDED(hr) {
		sharedClient7zHandler = sharedClient7zHandler_;
	} else {
		delete sharedClient7zHandler_;
	}
	return hr;
}
Client7zHandler* getSharedClient7z() {
	DUMBASS_CHECK(sharedClient7zHandler != NULL);
	return sharedClient7zHandler;
}
void releaseSharedClient7z() {
	delete sharedClient7zHandler;
	sharedClient7zHandler = NULL;
}

} // namespace files
