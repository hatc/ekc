// adapt_scl.hpp
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

#ifndef __CPCL_ADAPT_SCL_HPP
#define __CPCL_ADAPT_SCL_HPP

#include <exception>

namespace cpcl {

/*DefaultConstructible
CopyConstructible
Assignable
EqualityComparable == Default constructible, Copy constructible, Assignable, equality Comparable == DCAC))*/
template<class IteratorType>
struct DCACAdaptSCL {
	typedef DCACAdaptSCL<IteratorType> self_type;

	typedef typename IteratorType::iterator_category iterator_category;
	typedef typename IteratorType::value_type value_type;
	typedef typename IteratorType::difference_type difference_type;
	typedef difference_type distance_type;
	typedef typename IteratorType::pointer pointer;
	typedef typename IteratorType::reference reference;

	IteratorType *i;
	unsigned char i_storage[sizeof(IteratorType)];

	DCACAdaptSCL() : i(NULL) {}
	DCACAdaptSCL(IteratorType const &i_) {
		i = new (i_storage) IteratorType(i_);
	}
	DCACAdaptSCL(self_type const &r) : i(NULL) {
		if (r.i)
			i = new (i_storage) IteratorType(*r.i);
	}
	~DCACAdaptSCL() {
		if (i)
			i->~IteratorType();
		i = NULL;
	}

	self_type& operator=(self_type const &r) {
		if (!i || !r.i) { // one or both not initalized
			if (i) {
				// if (iterator) && (!iterator || !r.iterator) => r.iterator == NULL
				// delete i
				i->~IteratorType();
				i = NULL;
			} else if (r.i) {
				// if (r.iterator) && (!iterator || !r.iterator) => iterator == NULL
				// copy r.i
				i = new (i_storage) IteratorType(*r.i);
			} // else => both not initalized, no additional steps needed
		} else {
			// both initalized
			// delete i and copy r.i
			i->~IteratorType();
			i = new (i_storage) IteratorType(*r.i);
		}

		return (*this);
	}
	self_type& operator=(IteratorType const &i_) {
		if (i)
			i->~IteratorType();
		i = new (i_storage) IteratorType(i_);
	}

	reference operator*() const {
		// return designated object
		if (!i)
			throw std::exception("DCACAdaptSCL iterator not dereferencable", 1);
		IteratorType &iterator = *i;
		return (*iterator);
	}

	pointer operator->() const {
		// return pointer to class object
		return (&**this);
	}
	
	self_type& operator++() {
		// preincrement
		if (i)
			++*i;
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
		if (i)
			++*i;
		return (*this);
	}
	
	self_type operator--(int) {
		// postdecrement
		self_type tmp = *this;
		--*this;
		return (tmp);
	}

	bool operator==(self_type const &r) const {
		if (!i || !r.i)
			return (i == r.i);
		IteratorType const &iterator = *i;
		IteratorType const &ri = *r.i;
		// test for iterator equality
		return (iterator == ri);
	}

	bool operator!=(self_type const &r) const {
		// test for iterator inequality
		return (!(*this == r));
	}

	bool operator==(IteratorType const &r) const {
		if (!i)
			return false;
		IteratorType const &iterator = *i;
		// test for iterator equality
		return (iterator == r);
	}

	bool operator!=(IteratorType const &r) const {
		// test for iterator inequality
		return (!(*this == r));
	}
};

} // namespace cpcl

#endif // __CPCL_ADAPT_SCL_HPP
