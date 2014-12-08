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
#include "stdafx.h" /* "stdafx.h" && "targetver.h" should be copied in all !third_party projects */

#include <stdarg.h>
#include <stdio.h>

#ifdef CPCL_USE_BOOST_THREAD_SLEEP
#include <boost/thread/thread.hpp>
#endif
//#include <boost/thread/mutex.hpp>
#include "named_os_mutex.hpp"

#include <windows.h>

#include "trace.h"
#include "formatidiv.hpp"
#include "string_util.h"
#include "error_handler.h"

namespace cpcl {

#ifdef _DEBUG
int TRACE_LEVEL = CPCL_TRACE_LEVEL_ERROR | CPCL_TRACE_LEVEL_WARNING | CPCL_TRACE_LEVEL_DEBUG;
#else
int TRACE_LEVEL = CPCL_TRACE_LEVEL_ERROR | CPCL_TRACE_LEVEL_WARNING;
#endif
wchar_t const *TRACE_FILE_PATH = (wchar_t const *)0;

void SetTraceFilePath(WStringPiece const &v) {
	if (TRACE_FILE_PATH)
		delete [] TRACE_FILE_PATH;

	size_t len = v.size();
	if (len) {
		wchar_t *s = new wchar_t[len + 1];
		wcscpy_s(s, len + 1, v.data());
		TRACE_FILE_PATH = s;
	} else
		TRACE_FILE_PATH = 0;
}

/*
A name specified using the static keyword has internal linkage except for the static members of a class that have external linkage.
That is, it is not visible outside the current translation unit. 
Functions that are declared as inline and that are not class member functions are given the same linkage characteristics 
as functions declared as static.

By default, an object or variable that is defined outside all blocks has static duration and external linkage.
Static duration means that the object or variable is allocated when the program starts and is deallocated when the program ends.
External linkage means that the name of the variable is visible from outside the file in which the variable is declared.
Conversely, internal linkage means that the name is not visible outside the file in which the variable is declared.

When you declare a variable or function at file scope (global and/or namespace scope), the static keyword specifies that the variable or function
has internal linkage.
When you declare a variable, the variable has static duration and the compiler initializes it to 0 unless you specify another value.
*/
//static MutexOS TRACE_FILE_MUTEX;
//static boost::timed_mutex TRACE_FILE_MUTEX;
static NamedMutexOS TRACE_FILE_MUTEX(L"Local\\Trace2394mcw942q3829485043m");
inline void WriteString(CPCL_TRACE_LEVEL l, char const *s, size_t size) {
	DWORD error_code(ERROR_SUCCESS);
	StringPiece error;
	if (TRACE_FILE_PATH) {
		//boost::unique_lock<MutexOS> lock(TRACE_FILE_MUTEX);
		//no reason to use MutexOS, coz shared resource(i.e. hFile) in the case of TerminateThread anyway will not be released
		//therefore, even if we can check for abandoned mutex(thanks to WAIT_ABANDONED_0), shared resource stay locked...
		/*boost::unique_lock<boost::timed_mutex> lock(TRACE_FILE_MUTEX, boost::try_to_lock);
		if ((!lock) && (!lock.timed_lock(boost::posix_time::seconds(1))))
		{
		error = StringPiece("TRACE_FILE_MUTEX locked");
		}
		else
		{*/
		boost::unique_lock<NamedMutexOS> lock(TRACE_FILE_MUTEX, boost::try_to_lock);
		if ((!lock) && (!lock.timed_lock(boost::posix_time::seconds(2)))) {
		error = StringPiece("TRACE_FILE_MUTEX locked");
		}

		if ((!error.empty()) || (TRACE_FILE_MUTEX.abadoned))
		{
		if (error.empty())
			error = StringPiece("TRACE_FILE_MUTEX abadoned");
		}
		else
		{

		HANDLE hFile = ::CreateFileW(TRACE_FILE_PATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
			(LPSECURITY_ATTRIBUTES)NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
		if (INVALID_HANDLE_VALUE == hFile) {
			error_code = ::GetLastError();
			if (ERROR_SHARING_VIOLATION == error_code) {
#ifdef CPCL_USE_BOOST_THREAD_SLEEP
				boost::this_thread::sleep(boost::posix_time::milliseconds(0x100)); /* let's take a nap ^_^ */
#else
				::SleepEx(0x100, TRUE); /* let's take a nap ^_^ */
#endif
				hFile = ::CreateFileW(TRACE_FILE_PATH, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ,
					(LPSECURITY_ATTRIBUTES)NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
				error_code = ERROR_SUCCESS;
			}
		}

		if (INVALID_HANDLE_VALUE == hFile) {
			if (ERROR_SUCCESS == error_code)
				error_code = ::GetLastError();
			error = StringPieceFromLiteral("unable to open log file for writing:");
		} else {
			/*two IO function call with acquired shared resource - hFile
			if thread has terminated with exception at this IO operations, hFile still open - log locked
			so probaly a good idea add scope_guard(CloseHandle, hFile)*/
			LONG unused(0);
			/*If lpDistanceToMoveHigh is NULL and the new file position does not fit in a 32-bit value, 
			the function fails and returns INVALID_SET_FILE_POINTER.*/
			::SetFilePointer(hFile, 0, &unused, FILE_END);
			DWORD bytes_written(0);
			BOOL r = ::WriteFile(hFile, s, size, &bytes_written, NULL);
			if ((FALSE == r) || (bytes_written < size)) {
				error_code = ::GetLastError();
				error = StringPieceFromLiteral("cannot write to log file:");
			}
			::CloseHandle(hFile);
		}

		}
	} else if (l >= CPCL_TRACE_LEVEL_ERROR)
		error = StringPieceFromLiteral("TRACE_FILE_PATH is not set");

	if (!error.empty()) {
		std::string error_buf;
		if (TRACE_FILE_PATH) {
			size_t len = wcslen(TRACE_FILE_PATH);
			error_buf.assign(len + 1, '\n');
			ConvertUTF16_CP1251(WStringPiece(TRACE_FILE_PATH, len), (unsigned char*)(error_buf.c_str() + 1));
		}
		if (error_code != ERROR_SUCCESS) {
			void *text(0);
			DWORD n = ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, error_code,
				MAKELANGID(LANG_ENGLISH/*LANG_NEUTRAL*/, SUBLANG_ENGLISH_US/*SUBLANG_NEUTRAL*/),
				(LPSTR)&text, 0, NULL);
			if ((n > 0) && (text)) {
				error_buf += '\n';
				error_buf.append((char*)text, n);
				::LocalFree((HLOCAL)text);
			}
		}
		if (!error_buf.empty()) {
			error_buf.insert(0, error.data(), error.size());
			error = StringPiece(error_buf);
		}

		if (TRACE_FILE_PATH) /* StringPiece("TRACE_FILE_PATH is not set") should never traced ))) */
			ErrorHandler(error.data(), error.size());
		ErrorHandler(s, size);
	}
}

inline char const* TraceLevelNames(CPCL_TRACE_LEVEL v) {
	if (CPCL_TRACE_LEVEL_INFO == v)
		return "[notice] ";
	else if (CPCL_TRACE_LEVEL_DEBUG == v)
		return "[debug]  ";
	else if (CPCL_TRACE_LEVEL_WARNING == v)
		return "[warning]";
	else if (CPCL_TRACE_LEVEL_ERROR == v)
		return "[error]  ";
	else
		return "  [^_^]  ";
}
static size_t const HEADER_LENGTH = 45;
inline char* FormatHeader(char *s_, CPCL_TRACE_LEVEL l, SYSTEMTIME t, DWORD processId, DWORD threadId) {
	char *s = formatidiv<10>(s_, t.wYear, 4);
	*s = '/';
	s = formatidiv<10>(s + 1, t.wMonth, 2);
	*s = '/';
	s = formatidiv<10>(s + 1, t.wDay, 2);
	*s = ' ';
	s = formatidiv<10>(s + 1, t.wHour, 2);
	*s = ':';
	s = formatidiv<10>(s + 1, t.wMinute, 2);
	*s = ':';
	s = formatidiv<10>(s + 1, t.wSecond, 2);
	*s = '.';
	s = formatidiv<10>(s + 1, t.wMilliseconds, 3);
	*s = ' ';
	memcpy(s + 1, TraceLevelNames(l), arraysize("  [^_^]  ") - 1); s += arraysize("  [^_^]  ");
	*s = ' ';
	s = formatidiv<10>(s + 1, processId, 4);
	*s = '#';
	s = formatidiv<10>(s + 1, threadId, 4);
	*s = ':';
	*++s = ' ';
	// assert(HEADER_LENGTH == (s - s_ + 1));
	return s;
}
void __cdecl Trace(CPCL_TRACE_LEVEL trace_level, char const *format, ...) {
	if ((trace_level & TRACE_LEVEL) == 0)
		return;

	char buf[0x200 + HEADER_LENGTH];
	
	va_list args;
	va_start(args, format);
	int n = _vsnprintf_s(buf + HEADER_LENGTH, arraysize(buf) - HEADER_LENGTH - 2, _TRUNCATE, format, args);
	if (n < 0) {
		n = arraysize(buf) - 2;
		if ('\0' == buf[n - 1])
			--n;
	} else
		n += HEADER_LENGTH;
	size_t size = static_cast<size_t>(n + 2);
	buf[size - 2] = '\r';
	buf[size - 1] = '\n';

	SYSTEMTIME t;
	::GetLocalTime(&t);
	DWORD processId = ::GetCurrentProcessId();
	DWORD threadId = ::GetCurrentThreadId();
	FormatHeader(buf, trace_level, t, processId, threadId);

	WriteString(trace_level, buf, size);
}

/*void TraceString(CPCL_TRACE_LEVEL trace_level, StringPiece const &s) {
	if ((trace_level & TRACE_LEVEL) == 0)
		return;

	char static_buf[0x10 + HEADER_LENGTH];
	boost::scoped_array<char> dynamic_buf;
	char *buf = static_buf;
	if (s.size() > (arraysize(static_buf) - HEADER_LENGTH - 2)) {
		dynamic_buf.reset(new char[s.size() + HEADER_LENGTH + 2]);
		buf = dynamic_buf.get();
	}

	SYSTEMTIME t;
	::GetLocalTime(&t);
	DWORD processId = ::GetCurrentProcessId();
	DWORD threadId = ::GetCurrentThreadId();
	FormatHeader(buf, trace_level, t, processId, threadId);

	memcpy(buf + HEADER_LENGTH, s.data(), s.size());

	buf[HEADER_LENGTH + s.size()] = '\r';
	buf[HEADER_LENGTH + s.size() + 1] = '\n';

	WriteString(trace_level, buf, s.size() + HEADER_LENGTH + 2);
}*/

void TraceString(CPCL_TRACE_LEVEL trace_level, StringPiece const &s) {
	if ((trace_level & TRACE_LEVEL) == 0)
		return;

	char buf[0x200 + HEADER_LENGTH];
	size_t size = min(s.size(), arraysize(buf) - HEADER_LENGTH - 2);

	SYSTEMTIME t;
	::GetLocalTime(&t);
	DWORD processId = ::GetCurrentProcessId();
	DWORD threadId = ::GetCurrentThreadId();
	FormatHeader(buf, trace_level, t, processId, threadId);

	memcpy(buf + HEADER_LENGTH, s.data(), size);

	buf[HEADER_LENGTH + size] = '\r';
	buf[HEADER_LENGTH + size + 1] = '\n';

	WriteString(trace_level, buf, size + HEADER_LENGTH + 2);
}

/*void TraceSystemMessage(CPCL_TRACE_LEVEL trace_level, StringPiece const &routine, unsigned long error_code) {
	if ((trace_level & TRACE_LEVEL) == 0)
		return;

	char static_buf[0x10 + HEADER_LENGTH];
	boost::scoped_array<char> dynamic_buf;
	char *buf = static_buf;

	StringPiece text = StringPieceFromLiteral("Operation completed succesfully");
	std::string text_buf;
	if (error_code != ERROR_SUCCESS) {
		void *text_(0);
		::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, error_code,
			MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
			(LPSTR)&text, 0, NULL);
		if (text_) {
			text_buf = (char*)text_;
			::LocalFree((HLOCAL)text_);
			text = StringPiece(text_buf);
		} else {
			text = StringPieceFromLiteral("TraceSystemMessage(): LocalAlloc fails");
		}
	}

	if ((routine.size() + text.size()) > (arraysize(static_buf) - HEADER_LENGTH - 2)) {
		dynamic_buf.reset(new char[routine.size() + text.size() + HEADER_LENGTH + 2]);
		buf = dynamic_buf.get();
	}

	SYSTEMTIME t;
	::GetLocalTime(&t);
	DWORD processId = ::GetCurrentProcessId();
	DWORD threadId = ::GetCurrentThreadId();
	FormatHeader(buf, trace_level, t, processId, threadId);

	memcpy(buf + HEADER_LENGTH, routine.data(), routine.size());
	memcpy(buf + routine.size() + HEADER_LENGTH, text.data(), text.size());

	buf[HEADER_LENGTH + routine.size() + text.size()] = '\r';
	buf[HEADER_LENGTH + routine.size() + text.size() + 1] = '\n';

	WriteString(trace_level, buf, routine.size() + text.size() + HEADER_LENGTH + 2);
}*/

void __cdecl ErrorSystem(unsigned long error_code, char const *format, ...) {
	if ((CPCL_TRACE_LEVEL_ERROR & TRACE_LEVEL) == 0)
		return;

	char buf[0x300 + 2 * HEADER_LENGTH];
	
	va_list args;
	va_start(args, format);
	int n = _vsnprintf_s(buf + HEADER_LENGTH, arraysize(buf) - 2 * (HEADER_LENGTH + 2) - 8, _TRUNCATE, format, args);
	if (n < 0) {
		n = arraysize(buf) - HEADER_LENGTH - 12;
		if ('\0' == buf[n - 1])
			--n;
	} else
		n += HEADER_LENGTH;
	size_t size = static_cast<size_t>(n + 2);
	buf[size - 2] = '\r';
	buf[size - 1] = '\n';

	typedef char HEADER_LENGTH_SIZE_INVALID[bool(HEADER_LENGTH > 14) ? 1 : -1];
	memset(buf + size, ' ', HEADER_LENGTH - 14);
	{
		char *s = buf + size + HEADER_LENGTH - 14;
		*s++ = '('; *s++ = '0'; *s++ = 'x';
		s = formatidiv<16>(s, error_code, 8);
		*s++ = ')'; *s++ = ':'; *s++ = ' ';
	}
	size += HEADER_LENGTH;

	//throw 0; // check n && offset - "HEADER UserMessage\r\n''.join(' ' for _ in xrange(HEADER_LENGTH)) SystemMessage\r\n"
	DWORD n_ = ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error_code,
		MAKELANGID(LANG_ENGLISH/*LANG_NEUTRAL*/, SUBLANG_ENGLISH_US/*SUBLANG_NEUTRAL*/),
		buf + size, arraysize(buf) - size - 2, NULL);
	if (0 == n_) {
		if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			ErrorHandler("ErrorSystem(): something was definitely wrong...");
		formatidiv<16>(buf + size, error_code, 8);
		n_ = 8;
	} else if ((n_ > 2) 
		&& ('\r' == *(buf + size + n_ - 2))
		&& ('\n' == *(buf + size + n_ - 1)))
		n_ -= 2;
	size += n_ + 2;
	buf[size - 2] = '\r';
	buf[size - 1] = '\n';

	SYSTEMTIME t;
	::GetLocalTime(&t);
	DWORD processId = ::GetCurrentProcessId();
	DWORD threadId = ::GetCurrentThreadId();
	FormatHeader(buf, CPCL_TRACE_LEVEL_ERROR, t, processId, threadId);

	WriteString(CPCL_TRACE_LEVEL_ERROR, buf, size);
}

std::string GetSystemMessage(unsigned long error_code) {
	std::string r;
	void *text(0);
	::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, error_code, 
		MAKELANGID(LANG_ENGLISH/*LANG_NEUTRAL*/, SUBLANG_ENGLISH_US/*SUBLANG_NEUTRAL*/),
		(LPSTR)&text, 0, NULL);
	if (text) {
		r = (LPCSTR)text;
		::LocalFree((HLOCAL)text);
	}

	return r;
}

} // namespace cpcl
