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
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include "sharedIconMap.h"

#include <map>

#pragma warning( push )
#pragma warning( disable : 4244 )

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

#pragma warning( pop )

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include <commctrl.h>
#include <shellapi.h>

HIMAGELIST g_ImageList = NULL;

namespace files {

typedef boost::lock_guard<boost::shared_mutex> scoped_lock;
typedef boost::shared_lock<boost::shared_mutex> scoped_shared_lock;

static boost::shared_mutex sharedIconMapSemaphore;

typedef std::map<std::wstring, int> iconMap_t;
typedef iconMap_t::iterator iconMap_i;
typedef iconMap_t::const_iterator iconMap_const_i;

class SharedIconMap {
	typedef std::map<std::wstring, int> iconMap_t;
	typedef iconMap_t::iterator iconMap_i;
	typedef iconMap_t::const_iterator iconMap_const_i;

	iconMap_t iconMap;
	boost::shared_mutex iconMapSemaphore;

	DISALLOW_COPY_AND_ASSIGN(SharedIconMap);
public:
	int noExtIconIndex;

	SharedIconMap() {}
	~SharedIconMap() {}

	bool find(std::wstring const &ext, int *iconIndex) {
		scoped_shared_lock lock(iconMapSemaphore);

		iconMap_const_i i = iconMap.find(ext);
		bool r(i != iconMap.end());
		if (r && iconIndex)
			*iconIndex = (*i).second;
		return r;
	}
	void set(std::wstring const &ext, int iconIndex) {
		scoped_lock lock(iconMapSemaphore);

		iconMap[ext] = iconIndex;
	}
	void setExtNoIconIndex(int v) {
		scoped_lock lock(iconMapSemaphore);

		noExtIconIndex = v;
	}
	int getExtNoIconIndex() {
		scoped_shared_lock lock(iconMapSemaphore);

		return noExtIconIndex;
	}
	void clear() {
		scoped_lock lock(iconMapSemaphore);

		iconMap.clear();
	}
};

static SharedIconMap *sharedIconMap = NULL;

// if name not contain dot { return false; } else { if (ext) *ext = toLower(name[head:tail]); return true; }
//inline bool findNormalizedExt(wchar_t const *name, std::wstring *ext) { return name; } // pass traling spaces, slashes
inline bool findNormalizedExt(wchar_t const *name, std::wstring *ext) {
	if (!name)
		return false;

	wchar_t const *badTralingCharacters = L"\\/\t ";
	wchar_t const *v = name + ::wcslen(name);
	if (v == name)
		return false;
	--v;

	while ((v != name) && (::wcschr(badTralingCharacters, *v)))
		--v;
	if (v == name)
		return false;

	wchar_t const *tail = v;
	while ((v != name) && (L'.' != *v))
		--v;
	if ((L'.' != *v) || (v == tail))
		return false;

	if (ext) {
		ext->assign(++v, ++tail);
		_wcslwr(const_cast<wchar_t*>(ext->c_str()));
		// _wcslwr_s(const_cast<wchar_t*>(ext->c_str()), ext->size() + 1);
	}
	return true;
}

void createSharedIconMap() {
	scoped_lock lock(sharedIconMapSemaphore);

	if (!sharedIconMap)
		sharedIconMap = new SharedIconMap();
}

int getIconIndex(wchar_t const *name) {
	scoped_shared_lock lock(sharedIconMapSemaphore);

	int iconIndex = -1;
	if (!sharedIconMap)
		return iconIndex;

	std::wstring pretendName, ext;
	if (findNormalizedExt(name, &ext)) {
		if (sharedIconMap->find(ext, &iconIndex))
			return iconIndex;

		pretendName.assign(L"e.", 2);
		pretendName += ext;
	} else {
		if (sharedIconMap->getExtNoIconIndex() >= 0)
			return sharedIconMap->getExtNoIconIndex();

		pretendName = L"__File__";
	}

	SHFILEINFOW shFileInfo = { 0 };
	DWORD_PTR res = ::SHGetFileInfoW(pretendName.c_str(),
		FILE_ATTRIBUTE_NORMAL,
		&shFileInfo,
		sizeof(shFileInfo),
		SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX);
	if (res) {
		g_ImageList = (HIMAGELIST) res;
		iconIndex = shFileInfo.iIcon;

		if (ext.empty())
			sharedIconMap->setExtNoIconIndex(iconIndex);
		else
			sharedIconMap->set(ext, iconIndex);
	}

	return iconIndex;
}

void releaseSharedIconMap() {
	scoped_lock lock(sharedIconMapSemaphore);

	if (sharedIconMap) {
		delete sharedIconMap;
		sharedIconMap = NULL;
	}
}

} // namespace files
