// named_os_mutex.hpp
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

#ifndef __CPCL_NAMED_OS_MUTEX_HPP
#define __CPCL_NAMED_OS_MUTEX_HPP

#include <boost/thread/thread.hpp>
#include <boost/thread/exceptions.hpp>
#include <boost/thread/locks.hpp>

namespace cpcl {

// named mutex - use from different binaries - plugin && exe

class NamedMutexOS : boost::noncopyable {
	void *mutex_os;
	unsigned long ecode;
	bool timed_lock_(unsigned long milliseconds);

public:
	bool abadoned;

	NamedMutexOS(wchar_t const *name);
	~NamedMutexOS();

	bool try_lock() {
		return timed_lock_(0);
	}
	void lock() {
		timed_lock_(~(unsigned long)0);
	}
	bool timed_lock(boost::posix_time::ptime const &wait_until) {
		return timed_lock_(boost::detail::get_milliseconds_until(wait_until));
	}
	template<typename Duration>
	bool timed_lock(Duration const &timeout/*relative_time*/) {
		boost::int64_t v = timeout.total_milliseconds();
		return timed_lock_(v < 0 ? 0 : (unsigned long)v);
	}

	void unlock();

	typedef boost::unique_lock<NamedMutexOS> scoped_timed_lock;
	typedef boost::detail::try_lock_wrapper<NamedMutexOS> scoped_try_lock;
	typedef scoped_timed_lock scoped_lock;
};

}  // namespace cpcl

#endif  // __CPCL_NAMED_OS_MUTEX_HPP
