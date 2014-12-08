// zero_delimited_iterator.hpp
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

#ifndef __CPCL_ZERO_DELIMITED_ITERATOR_HPP
#define __CPCL_ZERO_DELIMITED_ITERATOR_HPP

#include <iterator>
#include "string_piece.hpp"

namespace cpcl {

// "text" -> ["text"]
// "few\0strings\0" -> ["few", "strings"]
// "\0\0\0" -> ["", "", ""]
// unsigned char data[] = { 0, 0, 0, 0 }; ZeroDelimitedIterator(data, 3) => ["", "", ""]
// unsigned char data[] =  { 't', 'e', 'x', 't' }; ZeroDelimitedIterator(data, 3) => ["tex"] // == StringPiece("tex").as_string()
// unsigned char data[] = "few\0strings\0\0"; ZeroDelimitedIterator(data, sizeof(data) - 1) => ["few", "strings", ""]
struct ZeroDelimitedIterator : public std::iterator<std::forward_iterator_tag, cpcl::StringPiece> {
	unsigned char const *head, *tail, *last;
	ZeroDelimitedIterator() : head(NULL), tail(NULL), last(NULL) {}
	ZeroDelimitedIterator(unsigned char const *data, size_t size)
		: head(data), tail(data), last(data + size)
	{
		if (size) {
			if (*tail)
				++tail;
			for (; tail != last; ++tail) {
				if (0 == *tail)
					break;
			}
		} else {
			head = tail = last = NULL;
		}
	}
	ZeroDelimitedIterator(unsigned char const *first_, unsigned char const *last_)
		: head(first_), tail(first_), last(last_)
	{
		if (first_ != last_) {
			if (*tail)
				++tail;
			for (; tail != last; ++tail) {
				if (0 == *tail)
					break;
			}
		} else {
			head = tail = last = NULL;
		}
	}

	ZeroDelimitedIterator& operator++() {
		if ((tail != last) && ((tail + 1) != last)) {
			for (head = ++tail; tail != last; ++tail) {
				if (0 == *tail)
					break;
			}
		} else {
			head = tail = last = NULL;
		}
		return *this;
	}
	ZeroDelimitedIterator operator++(int) {
		ZeroDelimitedIterator r(*this);
		++*this;
		return r;
	}

	value_type operator*() const {
		return cpcl::StringPiece((char const*)head, tail - head);
	}
	bool operator==(ZeroDelimitedIterator const &r) const {
		return (r.head == head)
			&& (r.tail == tail)
			&& (r.last == last);
	}
	bool operator!=(ZeroDelimitedIterator const &r) const {
		return !(r == *this);
	}
};

} // namespace cpcl

#endif // __CPCL_ZERO_DELIMITED_ITERATOR_HPP
