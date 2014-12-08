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

#include "error_handler.h"

#include <iostream>

namespace cpcl {

static void __stdcall _DefaultErrorHandler(char const *s, unsigned int s_length) {
	std::cout << "ErrorHandler :: \n";
	if (s_length > 2)
		s_length -= 2;
	std::cout.write(s, static_cast<std::streamsize>(s_length));
	std::cout << std::endl;
}
//static void _DefaultErrorHandler(char const *s, unsigned int s_length) {
//	LPSTR s_ = (LPSTR)::HeapAlloc(::GetProcessHeap(), 0, s_length + 1);
//	if (s_) {
//		memcpy(s_, s, s_length);
//		s_[s_length] = '\0';
//		::MessageBoxA(NULL, s_, NULL, MB_ICONERROR | MB_SERVICE_NOTIFICATION);
//		::HeapFree(::GetProcessHeap(), 0, s_);
//	}
//}

//static ErrorHandlerPtr ERROR_HANDLER = (ErrorHandlerPtr)0;
static ErrorHandlerPtr ERROR_HANDLER = _DefaultErrorHandler;

void SetErrorHandler(ErrorHandlerPtr v) {
	ERROR_HANDLER = v;
}

void ErrorHandler(char const *s, unsigned int s_length) {
	if (ERROR_HANDLER)
		ERROR_HANDLER(s, s_length);
}

// test - set_error_handler( std::cout << '\'' << s << '\'' << std::endl << s_length << " : " << char_traits<char>::len(s) << std::endl )

} // namespace cpcl
