// circular_iterator.hpp
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

#ifndef __CPCL_CIRCULAR_ITERATOR_HPP
#define __CPCL_CIRCULAR_ITERATOR_HPP

#include <memory>

namespace cpcl {

template<class T>
struct CircularConstIterator {
	typedef CircularConstIterator<T> self_type;

	typedef std::random_access_iterator_tag iterator_category;
	typedef T value_type;
	typedef ptrdiff_t difference_type;
	typedef ptrdiff_t distance_type;
	typedef T const* pointer;
	typedef T const& reference;

	pointer i;
	pointer const head, tail;
	difference_type size;

	CircularConstIterator() : i(0), head(0), tail(0), size(0)
	{}
	CircularConstIterator(T const *data, difference_type size_) : i(data), head(data), tail(data + size_), size(size_)
	{}
	CircularConstIterator(T const *first, T const *last) : i(first), head(first), tail(last), size(last - first)
	{}

	reference operator*() const {
		// return designated object
		return (*i);
	}

	pointer operator->() const {
		// return pointer to class object
		// return (&**this);
		return (i);
	}
	
	self_type& operator++() {
		// preincrement
		i = advance(1);
		return (*this);
	}

	self_type operator++(int) {
		// postincrement
		self_type tmp = *this;
		++*this;
		return (tmp);
	}
	
	self_type& operator--() {
		// predecrement
		i = advance(-1);
		return (*this);
	}
	
	self_type operator--(int) {
		// postdecrement
		self_type tmp = *this;
		--*this;
		return (tmp);
	}

	self_type& operator+=(difference_type offset) {
		// increment by integer
		i = advance(offset);
		return (*this);
	}

	self_type operator+(difference_type offset) const {
		// return this + integer
		self_type tmp = *this;
		return (tmp += offset);
	}

	self_type& operator-=(difference_type offset) {
		// decrement by integer
		return (*this += -offset);
	}

	self_type operator-(difference_type offset) const {
		// return this - integer
		self_type tmp = *this;
		return (tmp -= offset);
	}

	difference_type operator-(self_type const &r) const {
		// return difference of iterators
		return (i - r.i);
	}

	bool operator==(self_type const &r) const {
		// test for iterator equality
		return ((i == r.i) 
			&& (tail == r.tail)
			&& (size == r.size));
	}

	bool operator!=(self_type const &r) const {
		// test for iterator inequality
		return (!(*this == r));
	}

	bool operator<(self_type const &r) const {
		// test if this < r
		return (i < r.i);
	}
	
	bool operator>(self_type const &r) const {
		// test if this > r
		return (r < *this);
	}

	bool operator<=(self_type const &r) const {
		// test if this <= r
		return (!(r < *this));
	}

	bool operator>=(self_type const &r) const {
		// test if this >= r
		return (!(*this < r));
	}

private:
	pointer advance(ptrdiff_t offset) const {
		if (size < 1)
			return i;

		pointer r = i + offset;
		while (r < head) {
			r += size;
		}
		while (r >= tail) {
			r -= size;
		}
		return r;
	}
};

} // namespace cpcl

#endif // __CPCL_CIRCULAR_ITERATOR_HPP
