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
#include <cassert>

#include <sstream>
#include <iostream>

#include "timer.h"
#include "string_util.h"
#include "formatidiv.hpp"

template<size_t Base, class ValueType>
struct test_formatidiv_impl {
	ValueType v;
	size_t width;

	operator bool() {
		std::stringstream s; s.width(width);
		s << v;

		char buf[0x20];
		*formatidiv<Base>(buf, v, width, ' ') = '\0';

		if (!striASCIIequal(s.str().c_str(), buf)) {
			std::cout << "error: \n" << s.str() << std::endl << buf << std::endl;
			return false;
		}
		return true;
	}

	test_formatidiv_impl(ValueType v_, size_t width_) : v(v_), width(width_) {}
};
template<class ValueType>
struct test_formatidiv_impl<16, ValueType> {
	ValueType v;
	size_t width;

	operator bool() {
		std::stringstream s; s.width(width);
		s << std::hex << v;

		char buf[0x20];
		*formatidiv<16>(buf, v, width, ' ') = '\0';

		if (!striASCIIequal(s.str().c_str(), buf)) {
			std::cout << "error: \n" << s.str() << std::endl << buf << std::endl;
			return false;
		}
		return true;
	}

	test_formatidiv_impl(ValueType v_, size_t width_) : v(v_), width(width_) {}
};

template<size_t Base, class ValueType>
inline test_formatidiv_impl<Base, ValueType> make_test_formatidiv(ValueType v, size_t width) {
	return test_formatidiv_impl<Base, ValueType>(v, width);
}

//void test_formatidiv() {
//	assert(make_test_formatidiv<16>(static_cast<const int>(-1), 8));
//	assert(make_test_formatidiv<16>(static_cast<int>(-1), 8));
//	assert(make_test_formatidiv<10>(static_cast<int>(1), 8));
//	assert(make_test_formatidiv<10>(static_cast<int>(-1), 8));
//	assert(make_test_formatidiv<16>(static_cast<int>(1), 8));
//	// assert(make_test_formatidiv<16>(static_cast<int>(-1), 4)); - formatidiv<16> produce correct value 'FFFF'
//	assert(make_test_formatidiv<10>(static_cast<short>(-1), 4));
//	assert(make_test_formatidiv<10>(static_cast<long>(-123), 8));
//	assert(make_test_formatidiv<10>(static_cast<const unsigned int>(-1), 16));
//	assert(make_test_formatidiv<10>(static_cast<unsigned int>(-1), 16));
//	assert(make_test_formatidiv<16>(static_cast<const unsigned int>(-1), 8));
//	assert(make_test_formatidiv<16>(static_cast<unsigned int>(-1), 8));
//	assert(make_test_formatidiv<10>(static_cast<unsigned int>(1), 8));
//	assert(make_test_formatidiv<16>(static_cast<unsigned __int64>(-1), 16));
//	// assert(make_test_formatidiv<16>(static_cast<char>(-1), 4)); - formatidiv<16> produce correct value 'FF'
//	assert(make_test_formatidiv<16>(static_cast<short>(-1), 4));
//	assert(make_test_formatidiv<16>(static_cast<__int64>(-1), 16));
//}

double bench_formatidiv_idiv_dec(int v) {
	char buf[0x10];
	timer t;
	for (size_t i = 0; i < 0x1000; ++i) {
		*formatidiv<10>(buf, v, 4, '0') = '\0';
	}
	double r = t.elapsed();
	std::cout.width(30);
	std::cout << "bench_formatidiv_idiv_dec" << " : " << buf << std::endl;
	return r;
}
double bench_snprintf_idiv_dec(int v) {
	char buf[0x10];
	timer t;
	for (size_t i = 0; i < 0x1000; ++i) {
		_snprintf_s(buf, _TRUNCATE, "%04d", v);
	}
	double r = t.elapsed();
	std::cout.width(30);
	std::cout << "bench_snprintf_idiv_dec" << " : " << buf << std::endl;
	return r;
}
double bench_formatidiv_hex(int v) {
	char buf[0x10];
	timer t;
	for (size_t i = 0; i < 0x1000; ++i) {
		*formatidiv<16>(buf, v, 8) = '\0';
	}
	double r = t.elapsed();
	std::cout.width(30);
	std::cout << "bench_formatidiv_hex" << " : " << buf << std::endl;
	return r;
}
double bench_snprintf_hex(int v) {
	char buf[0x10];
	timer t;
	for (size_t i = 0; i < 0x1000; ++i) {
		_snprintf_s(buf, _TRUNCATE, "%08X", v);
	}
	double r = t.elapsed();
	std::cout.width(30);
	std::cout << "bench_snprintf_hex" << " : " << buf << std::endl;
	return r;
}
double bench_formatidiv_idiv_oct(int v) {
	char buf[0x10];
	timer t;
	for (size_t i = 0; i < 0x1000; ++i) {
		*formatidiv<8>(buf, (unsigned int)v, 8) = '\0';
	}
	double r = t.elapsed();
	std::cout.width(30);
	std::cout << "bench_formatidiv_idiv_oct" << " : " << buf << std::endl;
	return r;
}
double bench_snprintf_idiv_oct(int v) {
	char buf[0x10];
	timer t;
	for (size_t i = 0; i < 0x1000; ++i) {
		_snprintf_s(buf, _TRUNCATE, "%08o", v);
	}
	double r = t.elapsed();
	std::cout.width(30);
	std::cout << "bench_snprintf_idiv_oct" << " : " << buf << std::endl;
	return r;
}

void test_formatidiv_() {
	int v = -100;
	double r_0[9], r_1[9];
	{
		r_0[0] = bench_formatidiv_idiv_dec(v);
		r_1[0] = bench_snprintf_idiv_dec(v);
	}
	{
		v = 1234;
		r_1[1] = bench_snprintf_idiv_dec(v);
		r_0[1] = bench_formatidiv_idiv_dec(v);
	}
	{
		v = -61;
		r_0[2] = bench_formatidiv_idiv_dec(v);
		r_1[2] = bench_snprintf_idiv_dec(v);
	}
	{
		v = -161;
		r_0[3] = bench_formatidiv_idiv_oct(v);
		r_1[3] = bench_snprintf_idiv_oct(v);
	}
	double v_snprintf_s = (r_1[0] + r_1[1] + r_1[2] + r_1[3]) / 4;
	double v_formati = (r_0[0] + r_0[1] + r_0[2] + r_0[3]) / 4;

	std::cout.width(20); std::cout << "snprintf_s" << " : " << std::fixed << v_snprintf_s << std::endl;
	std::cout.width(20); std::cout << "formati" << " : " << std::fixed << v_formati << std::endl; 
	/*
	1550
	58
	*/
}

/*
double bench_formatidiv_idiv_dec(int v)
double bench_snprintf_idiv_dec(int v)
double bench_formatidiv_hex(int v)
double bench_snprintf_hex(int v)
double bench_formatidiv_idiv_oct(int v)
double bench_snprintf_idiv_oct(int v)
*/

//#include <windows.h>
//
//static size_t const header_length = 45;
//double testFormatidivTrace_snprintf_s(SYSTEMTIME t, DWORD processId, DWORD threadId, std::string *r) {
//	char buf[header_length];
//	size_t header_length_ = header_length;
//	
//	timer t_;
//	for (size_t i = 0; i < 0x1000; ++i) {
//		header_length_ = _snprintf_s(buf, header_length, _TRUNCATE, "%u/%02u/%02u %02u:%02u:%02u.%03u %s %04u#%04u:",
//			t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds,
//			"[info]", processId, threadId);
//		buf[header_length_++] = ' ';
//	}
//	double v = t_.elapsed();
//	if (r)
//		r->assign(buf, header_length_);
//	return v;
//}
//double testFormatidivTrace_formati(SYSTEMTIME t, DWORD processId, DWORD threadId, std::string *r) {
//	char buf[header_length];
//	size_t header_length_ = header_length;
//	
//	timer t_;
//	for (size_t i = 0; i < 0x1000; ++i) {
//		char *s = formatidiv<10>(buf, t.wYear, 4);
//		*s = '/';
//		s = formatidiv<10>(s + 1, t.wMonth, 2);
//		*s = '/';
//		s = formatidiv<10>(s + 1, t.wDay, 2);
//		*s = ' ';
//		s = formatidiv<10>(s + 1, t.wHour, 2);
//		*s = ':';
//		s = formatidiv<10>(s + 1, t.wMinute, 2);
//		*s = ':';
//		s = formatidiv<10>(s + 1, t.wSecond, 2);
//		*s = '.';
//		s = formatidiv<10>(s + 1, t.wMilliseconds, 3);
//		*s = ' ';
//		memcpy(s + 1, "[info]", 6); s += 7;
//		*s = ' ';
//		s = formatidiv<10>(s + 1, processId, 4);
//		*s = '#';
//		s = formatidiv<10>(s + 1, threadId, 4);
//		*s = ':';
//		*++s = ' ';
//		header_length_ = s - buf + 1;
//	}
//	double v = t_.elapsed();
//	if (r)
//		r->assign(buf, header_length_);
//	return v;
//}
//
//void test_formatidiv() {
//	double v_a, v_b;
//	{
//		std::string r_a, r_b;
//		double v_a_[3], v_b_[3];
//
//		SYSTEMTIME t;
//		::GetLocalTime(&t);
//		DWORD processId = ::GetCurrentProcessId();
//		DWORD threadId = ::GetCurrentThreadId();
//
//		v_a_[0] = testFormatidivTrace_snprintf_s(t, processId, threadId, &r_a);
//		v_b_[0] = testFormatidivTrace_formati(t, processId, threadId, &r_b);
//		if (r_a != r_b) {
//			std::cout << "ERROR:\n" << "\tsnprintf_s:\n\t" << r_a << "\n\tformati:\n\t" << r_b << std::endl;
//			return;
//		}
//		v_b_[1] = testFormatidivTrace_formati(t, processId, threadId, &r_b);
//		v_a_[1] = testFormatidivTrace_snprintf_s(t, processId, threadId, &r_a);
//		if (r_a != r_b) {
//			std::cout << "ERROR:\n" << "\tsnprintf_s:\n\t" << r_a << "\n\tformati:\n\t" << r_b << std::endl;
//			return;
//		}
//		v_a_[2] = testFormatidivTrace_snprintf_s(t, processId, threadId, &r_a);
//		v_b_[2] = testFormatidivTrace_formati(t, processId, threadId, &r_b);
//		if (r_a != r_b) {
//			std::cout << "ERROR:\n" << "\tsnprintf_s:\n\t" << r_a << "\n\tformati:\n\t" << r_b << std::endl;
//			return;
//		}
//		v_a = (v_a_[0] + v_a_[1] + v_a_[2]) / 3;
//		v_b = (v_b_[0] + v_b_[1] + v_b_[2]) / 3;
//	}
//	
//	std::cout.width(20); std::cout << "snprintf_s" << " : " << std::fixed << v_a << std::endl;
//	std::cout.width(20); std::cout << "formati" << " : " << std::fixed << v_b << std::endl;
//
//	/*
//	8311
//	534
//	*/
//}
