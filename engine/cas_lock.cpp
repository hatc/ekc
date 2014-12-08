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
#include "stdafx.h"

#include "cas_lock.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <intrin.h> // _InterlockedCompareExchange

#pragma intrinsic(_InterlockedCompareExchange, _InterlockedExchange)

static inline bool CAS(long volatile *lock) {
	return _InterlockedCompareExchange(lock, CASLock::LOCKED, CASLock::FREE) != CASLock::LOCKED;
}

/* // const owns_lock
CASLock::CASLock() : owns_lock(_InterlockedCompareExchange(&CASLock::LOCK, CASLock::LOCKED, CASLock::FREE) != CASLock::LOCKED), lock(&CASLock::LOCK)
{} // non-const owns_lock
CASLock::CASLock() : lock(&CASLock::LOCK) {
	owns_lock = _InterlockedCompareExchange(lock, CASLock::LOCKED, CASLock::FREE) != CASLock::LOCKED;
} 

// const owns_lock
CASLock::CASLock(long volatile *lock_) : owns_lock(_InterlockedCompareExchange(lock_, CASLock::LOCKED, CASLock::FREE) != CASLock::LOCKED), lock(lock_)
{}
// non-const owns_lock
CASLock::CASLock(long volatile *lock_) : lock(lock_) {
	owns_lock = _InterlockedCompareExchange(lock_, CASLock::LOCKED, CASLock::FREE) != CASLock::LOCKED;
}*/

long volatile CASLock::LOCK = CASLock::FREE;

CASLock::CASLock() : owns_lock(CAS(&CASLock::LOCK)), lock(&CASLock::LOCK)
{}
CASLock::CASLock(long volatile *lock_) : owns_lock(CAS(lock_)), lock(lock_)
{}

CASLock::~CASLock() {
	if (owns_lock)
		_InterlockedExchange(lock, CASLock::FREE);
}
