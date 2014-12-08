// split_iterator.hpp
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

#ifndef __CPCL_SPLIT_ITERATOR_HPP
#define __CPCL_SPLIT_ITERATOR_HPP

#include <iterator>
#include "string_piece.hpp"

namespace cpcl {

// "|" -> ["", ""]
// "text" -> ["text"]
// "few|strings|" -> ["few", "strings", ""]
// "|||" -> ["", "", "", ""]
// { '|', '|', '|', '|' } {size() - 1} -> ["", "", "", ""]
// { 't', 'e', 'x', 't' } {size() - 1} -> ["tex"]
// "few|strings||" -> ["few", "strings", "", ""]
// "few|strings" {size() - 1} -> ["few", "string"]
// 
// cpcl::StringSplitIterator i(cpcl::StringPieceFromLiteral("\t		text   "), '|'), tail;
// std::cout << "'" << (*i++).trim("\t ") << "'\n";
// assert(i == tail);
// 
//inline void foreach(cpcl::StringPiece const &v, char split) {
//	cpcl::StringSplitIterator i(v, split), tail;
//	while (i != tail)
//		std::cout << "'" << (*i++) << "'\n";
//	std::cout << "/********************************/" << std::endl;
//}
//inline void foreach(char const *data, size_t size, char split) {
//	cpcl::StringSplitIterator i(data, size, split), tail;
//	while (i != tail)
//		std::cout << "'" << (*i++) << "'\n";
//	std::cout << "/********************************/" << std::endl;
//}
/*wchar_t str[] = L"some.file|ext||";
	wchar_t const *s = L".|";

	wchar_t *next_token = NULL;
	wchar_t *token = wcstok_s(str, s, &next_token); 
	// char *token = strtok_s(char *str, char *delimiters, char **context) // <string.h>
	while (token != NULL) {
		wprintf(L"'%s'\n", token);
		
		token = wcstok_s(NULL, s, &next_token);
	}
	// ['some', 'file', 'ext']*/
template<class CharType>
struct BasicSplitIterator : public std::iterator<std::forward_iterator_tag, BasicStringPiece<CharType, std::char_traits<CharType> > > {
	typedef BasicStringPiece<CharType, std::char_traits<CharType> > StringPieceType;
	typedef BasicSplitIterator<CharType> self_type;

	CharType const *head, *tail, *last;
	CharType split;
	BasicSplitIterator() : head(NULL), tail(NULL), last(NULL) {}
	BasicSplitIterator(StringPieceType const &v, CharType split_)
		: head(v.data()), tail(v.data()), last(v.data() + v.size()), split(split_) {
		Init(v.size());
	}
	BasicSplitIterator(CharType const *data, size_t size, CharType split_)
		: head(data), tail(data), last(data + size), split(split_) {
		Init(size);
	}

	self_type& operator++() {
		if (tail != last) {
			for (head = ++tail; tail != last; ++tail) {
				if (*tail == split)
					break;
			}
		} else {
			head = tail = last = NULL;
		}
		return *this;
	}
	self_type operator++(int) {
		self_type r(*this);
		++*this;
		return r;
	}

	value_type operator*() const {
		return StringPieceType((tail == head) ? NULL : head, tail - head);
		//return StringPieceType(head, tail - head);
	}
	bool operator==(self_type const &r) const {
		return (r.head == head)
			&& (r.tail == tail)
			&& (r.last == last);
	}
	bool operator!=(self_type const &r) const {
		return !(r == *this);
	}
private:
	void Init(size_t size) {
		if (size) {
			for (; tail != last; ++tail) {
				if (*tail == split)
					break;
			}
		} else {
			head = tail = last = NULL;
		}
	}
};

typedef BasicSplitIterator<char>
	StringSplitIterator;
typedef BasicSplitIterator<wchar_t>
	WStringSplitIterator;

} // namespace cpcl

#endif // __CPCL_SPLIT_ITERATOR_HPP

#if 0
 test
 // !!! - "|" -> split -> ["", ""]
 std::cout << "'|' -> ['', '']\n";
 foreach(cpcl::StringPieceFromLiteral("|"), '|');

 std::cout << "'text' -> ['text']\n";
 cpcl::StringSplitIterator i(cpcl::StringPieceFromLiteral("\t		text   "), '|'), tail;
 std::cout << "'" << (*i++).trim("\t ") << "'\n";
 std::cout << std::endl;
 assert(i == tail);

 std::cout << "'few|strings|' -> ['few', 'strings', '']\n";
 foreach(cpcl::StringPieceFromLiteral("few|strings|"), '|');

 std::cout << "'|||' -> ['', '', '', '']\n";
 foreach(cpcl::StringPieceFromLiteral("|||"), '|');

 char data_0[] = { '|', '|', '|', '|' };
 std::cout << "{ '|', '|', '|', '|' } {size() - 1} -> ['', '', '', '']\n";
 foreach(data_0, 3, '|');

 std::cout << "{ '|', '|', '|', '|' } -> ['', '', '', '', '']\n";
 foreach(data_0, sizeof(data_0), '|');

 char data_1[] =  { 't', 'e', 'x', 't' };
 std::cout << "{ 't', 'e', 'x', 't' }; -> ['tex']\n";
 foreach(data_1, 3, '|');

 char data_2[] = "few|strings||";
 std::cout << "'few|strings||' -> ['few', 'strings', '', '']\n";
 foreach(data_2, sizeof(data_2) - 1, '|');

 cpcl::StringPiece s = cpcl::StringPieceFromLiteral("few|strings");
 std::cout << "'few|strings' {size() - 1} -> ['few', 'string']\n";
 foreach(s.data(), s.size() - 1, '|');
#endif
