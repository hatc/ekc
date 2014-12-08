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
#include <memory>
#include <stdexcept>

#include <vector>
#include <string>

#pragma warning( push )
#pragma warning( disable : 4996 ) // function call with parameters that may be unsafe - this call relies on the caller to check that the passed values are correct.
#include <boost/iostreams/stream.hpp>
#pragma warning( pop )
#include "container_device.hpp"

template< class T, class _Alloc = std::allocator<T> >
struct vector_pretend {
	typedef _Alloc allocator_type;
	typedef typename _Alloc::size_type size_type;
	typedef typename _Alloc::difference_type difference_type;
	typedef typename _Alloc::pointer pointer;
	typedef typename _Alloc::const_pointer const_pointer;
	typedef typename _Alloc::reference reference;
	typedef typename _Alloc::const_reference const_reference;
	typedef typename _Alloc::value_type value_type;

	vector_pretend(pointer data, std::streamsize data_size) : data_(data), data_size_(data_size) {}
	vector_pretend(vector_pretend<T, _Alloc> const &v) : data_(v.data_), data_size_(v.data_size_) {}

	vector_pretend<T, _Alloc>& operator=(vector_pretend<T, _Alloc> const &r) {
		data_ = r.data_; data_size_ = r.data_size_;
		return (*this);
	}

	size_type size() const { return data_size_; }
	bool empty() const { return 0 == data_size_; }

	const_reference at(size_type _Pos) const {
		// subscript nonmutable sequence with checking
		if (size() <= _Pos)
			throw std::out_of_range("invalid vector<T> subscript");
		return (*(data_ + _Pos));
	}
	reference at(size_type _Pos) {
		// subscript mutable sequence with checking
		if (size() <= _Pos)
			throw std::out_of_range("invalid vector<T> subscript");
		return (*(data_ + _Pos));
	}
	const_reference operator[](size_type _Pos) const {
		return (*(data_ + _Pos));
	}
	reference operator[](size_type _Pos) {
		return (*(data_ + _Pos));
	}

	pointer begin() {
		// return iterator for beginning of mutable sequence
		return (data_);
	}
	const_pointer begin() const {
		// return iterator for beginning of nonmutable sequence
		return (data_);
	}

private:
	pointer data_;
	std::streamsize data_size_;
};

template<class _CharType>
std::vector< std::basic_string<_CharType> > parseData(void* data_, std::streamsize data_size_) {
	typedef vector_pretend<_CharType> data_t;
	typedef boost::iostreams::example::container_source<data_t> vector_source;

	data_t data(reinterpret_cast<_CharType*>(data_), data_size_);
	std::basic_string<_CharType> s;
	std::vector< std::basic_string<_CharType> > r;

	//FILE *f = crt_file_traits<_CharType>::open_read(path);
	//if (f) {
	////fseek(f, 2, SEEK_SET); // FF FE
	//data_t::value_type data_[0x100] = { 0 };
	//for(;;) {
	//	size_t l = fread(&data_[0], sizeof(*data_), sizeof(data_) / sizeof(*data_), f);
	//	size_t t = data.size();
	//	data.resize(t + l);
	//	memcpy(&data[0] + t, &data_[0], l * sizeof(*data_));

	//	if (l != (sizeof(data_) / sizeof(*data_)))
	//		break;
	//}
	//fclose(f);
	//}

	boost::iostreams::stream<vector_source> in(data);
	while(std::getline(in, s)) { r.push_back(s); }

	return r;
}

std::vector<std::string> parseResourceData(void* data, std::streamsize data_size) { return parseData<char>(data, data_size); }
