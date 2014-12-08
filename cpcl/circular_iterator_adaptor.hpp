// circular_iterator_adaptor.hpp
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

#ifndef __CPCL_CIRCULAR_ITERATOR_ADAPTOR_HPP
#define __CPCL_CIRCULAR_ITERATOR_ADAPTOR_HPP

#include <exception>

namespace cpcl {

template<class ContainerType>
struct CircularIteratorAdaptor {
	typedef CircularIteratorAdaptor<ContainerType> self_type;
	typedef typename ContainerType::size_type size_type;

	typedef typename ContainerType::const_iterator Iterator;
	typedef typename Iterator::iterator_category iterator_category;
	typedef typename Iterator::value_type value_type;
	typedef typename Iterator::difference_type difference_type;
	typedef difference_type distance_type;
	typedef typename Iterator::pointer pointer;
	typedef typename Iterator::reference reference;

	ContainerType const *container;
	Iterator *i;
	unsigned char i_storage[sizeof(Iterator)];

	CircularIteratorAdaptor() : container(NULL), i(NULL) {}
	CircularIteratorAdaptor(ContainerType const &container_, Iterator const &i_) : container(&container_) {
		i = new (i_storage) Iterator(i_);
	}
	CircularIteratorAdaptor(self_type const &r) : container(r.container), i(NULL) {
		if (r.i)
			i = new (i_storage) Iterator(*r.i);
	}
	~CircularIteratorAdaptor() {
		if (i)
			i->~Iterator();
		i = NULL;
		container = NULL;
	}
	self_type& operator=(self_type const &r) {
		if (!i || !r.i) { // one or both not initialized
			if (i) {
				// if (iterator) && (!iterator || !r.iterator) => r.iterator == NULL
				// delete i
				i->~Iterator();
				i = NULL;
			} else if (r.i) {
				// if (r.iterator) && (!iterator || !r.iterator) => iterator == NULL
				// copy r.i
				i = new (i_storage) Iterator(*r.i);
			} // else => both not initialized, no additional steps needed
		} else {
			// both initialized
			// delete i and copy r.i
			i->~Iterator();
			i = new (i_storage) Iterator(*r.i);
		}
		container = r.container;

		return (*this);
	}

	reference operator*() const {
		// return designated object
		//return (*i);
		if (!i)
			throw std::exception("CircularIteratorAdaptor not dereferencable");
		Iterator &iterator = *i;
		return (*iterator);
	}

	pointer operator->() const {
		// return pointer to class object
		return (&**this);
	}
	
	self_type& operator++() {
		// preincrement
		//i = advance(1);
		advance(1);
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
		//i = advance(-1);
		advance(-1);
		return (*this);
	}
	
	self_type operator--(int) {
		// postdecrement
		self_type tmp = *this;
		--*this;
		return (tmp);
	}

	bool operator==(self_type const &r) const {
		if (container != r.container)
			return false;
		if (!i || !r.i)
			return (i == r.i);
		Iterator &iterator = *i;
		Iterator &ri = *r.i;
		// test for iterator equality
		//return ((container == r.container)
		//	&& (i == r.i));
		return (iterator == ri);
	}

	bool operator!=(self_type const &r) const {
		// test for iterator inequality
		return (!(*this == r));
	}

private:
	void advance(ptrdiff_t offset) const {
		if (!i || !container)
			return;
		size_type size = container->size();
		if (size < 1)
			return;

		Iterator &iterator = *i;
		Iterator r = iterator;
		if (container->end() == r)
			--r;
		if (offset < 0) {
			while(offset++ != 0) {
				if (container->begin() == r)
					r = container->end();
				--r;
			}
		} else if (offset > 0) {
			while(offset-- != 0) {
				++r;
				if (container->end() == r)
					r = container->begin();
			}
		}

		iterator = r;
	}
};

} // namespace cpcl

#endif // __CPCL_CIRCULAR_ITERATOR_ADAPTOR_HPP
