// trace.h
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

#ifndef __CPCL_TRACE_H
#define __CPCL_TRACE_H

#include "string_piece.hpp"

enum CPCL_TRACE_LEVEL {
	CPCL_TRACE_LEVEL_NONE = 0,
	CPCL_TRACE_LEVEL_INFO = 2,
	CPCL_TRACE_LEVEL_DEBUG = 1,
	CPCL_TRACE_LEVEL_ERROR = 8,
	CPCL_TRACE_LEVEL_WARNING = 4
};

namespace cpcl {

extern int TRACE_LEVEL;
extern wchar_t const *TRACE_FILE_PATH;
void SetTraceFilePath(WStringPiece const &v);

void __cdecl Trace(CPCL_TRACE_LEVEL trace_level, char const *format, ...);
void TraceString(CPCL_TRACE_LEVEL trace_level, StringPiece const &s);
inline void Debug(StringPiece const &s) {
	if (TRACE_LEVEL & CPCL_TRACE_LEVEL_WARNING)
		TraceString(CPCL_TRACE_LEVEL_DEBUG, s);
}
inline void Warning(StringPiece const &s) {
	if (TRACE_LEVEL & CPCL_TRACE_LEVEL_WARNING)
		TraceString(CPCL_TRACE_LEVEL_WARNING, s);
}
inline void Error(StringPiece const &s) {
	if (TRACE_LEVEL & CPCL_TRACE_LEVEL_ERROR)
		TraceString(CPCL_TRACE_LEVEL_ERROR, s);
}

void __cdecl ErrorSystem(unsigned long error_code, char const *format, ...);
std::string GetSystemMessage(unsigned long error_code);

} // namespace cpcl

#endif // __CPCL_TRACE_H
