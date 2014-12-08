// Copyright (C) 2012-2013 Yuri Agafonov
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

#include <memory>

#include <boost/scoped_array.hpp>

#include <file_util.h>
//#include <istream_wrapper.h>
#include <error_handler.h>
#include <hresult_helper.hpp>
#include <trace.h>
#include <dumbassert.h>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2
#include "plugin_list.h"
#include "file_iterator.hpp"

#include <windows.h>
#include <excpt.h>

typedef HRESULT (STDAPICALLTYPE *CreatePluginPtr)(IPlugin **r);
typedef HRESULT (STDAPICALLTYPE *CreatePluginsPtr)(DWORD *count, IPlugin **r);
typedef HRESULT (STDAPICALLTYPE *_InitTracePtr)(wchar_t const *log_path, cpcl::ErrorHandlerPtr error_handler);

static void __stdcall _ErrorHandlerWrapper(char const *s, unsigned int s_length) {
	cpcl::ErrorHandler(s, s_length);
}

static int Check(HMODULE hmodule, IPlugin **r) {
	CreatePluginPtr CreatePlugin = (CreatePluginPtr)::GetProcAddress(hmodule, "CreatePlugin");
	_InitTracePtr _InitTrace = (_InitTracePtr)::GetProcAddress(hmodule, "_InitTrace");
	if (!CreatePlugin || !_InitTrace)
		return 0;

	__try {
		if (_InitTrace(cpcl::TRACE_FILE_PATH, _ErrorHandlerWrapper) != S_OK)
			return 0;
		if (CreatePlugin(r) != S_OK)
			return 0;
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		cpcl::Error(cpcl::StringPieceFromLiteral("PluginList::Check: SEH"));
		return 0;
	}
	return 1;
}

static int CheckPluginsCount(HMODULE hmodule, unsigned int *count) {
	CreatePluginPtr CreatePlugin = (CreatePluginPtr)::GetProcAddress(hmodule, "CreatePlugin");
	CreatePluginsPtr CreatePlugins = (CreatePluginsPtr)::GetProcAddress(hmodule, "CreatePlugins");
	_InitTracePtr _InitTrace = (_InitTracePtr)::GetProcAddress(hmodule, "_InitTrace");
	if (!(!!CreatePlugin || !!CreatePlugins) || !_InitTrace)
		return 0;

	__try {
		if (_InitTrace(cpcl::TRACE_FILE_PATH, _ErrorHandlerWrapper) != S_OK)
			return 0;
		if (!!CreatePlugins) {
			DWORD count_(0);
			if (CreatePlugins(&count_, 0) != S_OK)
				return 0;
			*count = static_cast<unsigned int>(count_);
		} else
			*count = 1;
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		cpcl::Error(cpcl::StringPieceFromLiteral("PluginList::CheckPluginsCount: SEH"));
		return 0;
	}
	return 1;
}
static int CheckPlugins(HMODULE hmodule, unsigned int count, IPlugin **r) {
	CreatePluginPtr CreatePlugin = (CreatePluginPtr)::GetProcAddress(hmodule, "CreatePlugin");
	CreatePluginsPtr CreatePlugins = (CreatePluginsPtr)::GetProcAddress(hmodule, "CreatePlugins");
	if (!(!!CreatePlugin || !!CreatePlugins))
		return 0;

	__try {
		if (!!CreatePlugins) {
			DWORD count_ = static_cast<DWORD>(count);
			if (CreatePlugins(&count_, r) != S_OK)
				return 0;
		} else {
			if (CreatePlugin(r) != S_OK)
				return 0;
		}
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		cpcl::Error(cpcl::StringPieceFromLiteral("PluginList::CheckPlugins: SEH"));
		return 0;
	}
	return 1;
}

PluginList::PluginList()
{}

/*void PluginList::TryLoadPlugin(cpcl::WStringPiece const &dll_path) {
	HMODULE hmodule = ::LoadLibraryW(dll_path.as_string().c_str());
	if (hmodule) {
		ScopedComPtr<IPlugin> plugin;
		if (Check(hmodule, &plugin) == 1) {
			std::wstring hint;
			wchar_t const *extension; DWORD extension_size;
			if SUCCEEDED(plugin->DocFileExtension(&extension, &extension_size)) {
				cpcl::WStringSplitIterator i(extension, extension_size, L'|');
				hint = (*i).as_string();
			}
			if (hint.empty())
				hint = dll_path.as_string();

			Plugins::_Pairib pairib = plugins.insert(Plugins::value_type(hint, cpcl::ComPtr<IPlugin>()));
			//if (!pairib.second)
			(*(pairib.first)).second = (IPlugin*)plugin;
		} else {
			::FreeLibrary(hmodule);
		}
	}
}*/

/*void PluginList::TryLoadPlugin(cpcl::WStringPiece const &dll_path) {
	HMODULE hmodule = ::LoadLibraryW(dll_path.as_string().c_str());
	if (hmodule) {
		ScopedComPtr<IPlugin> plugin;
		if (Check(hmodule, &plugin) == 1) {
			plugins.push_back((IPlugin*)plugin);
		} else {
			::FreeLibrary(hmodule);
		}
	}
}*/

void PluginList::TryLoadPlugin(cpcl::WStringPiece const &dll_path) {
	HMODULE hmodule = ::LoadLibraryW(dll_path.as_string().c_str());
	if (hmodule) {
		unsigned int count(0);
		if (CheckPluginsCount(hmodule, &count) == 1) {
			if (1 == count) {
				ScopedComPtr<IPlugin> plugin;
				if (CheckPlugins(hmodule, 1, &plugin) == 1) {
					plugins.push_back((IPlugin*)plugin);
				} else {
					count = 0;
				}
			} else if (count > 1) {
				boost::scoped_array<IPlugin*> plugins_ptr(new IPlugin*[count]);
				Plugins local_plugins;
				local_plugins.resize(count);
				if (CheckPlugins(hmodule, count, &plugins_ptr[0]) == 1) {
					for (unsigned int i = 0; i < count; ++i) {
						local_plugins[i].Attach(plugins_ptr[i]); // doesn't throw
					}
					plugins.insert(plugins.end(), local_plugins.begin(), local_plugins.end()); // may throw, but then local_plugins dtor release resources
				} else {
					count = 0;
				}
			}
		}

		if (count < 1)
			::FreeLibrary(hmodule);
	}
}

unsigned int PluginList::LoadPlugins(std::wstring const &folder_path) {
	ext::files::FileIterator fileIt(folder_path), fileEnd;
	while (fileIt != fileEnd) {
		ext::files::FileInfo file_info = *fileIt++; // postincrement returns FileIterator(temporary, non-const), indirection returns FileInfo&, then just copy_ctor

		TryLoadPlugin(file_info.path);
	}
	return plugins.size();
}

PluginList* PluginList::LoadPlugins(cpcl::WStringPiece const &folder_path) {
	if (folder_path.empty())
		return (PluginList*)0;
	std::auto_ptr<PluginList> plugin_list(new PluginList());

	std::wstring path = folder_path.as_string();
	cpcl::WStringPiece tail = cpcl::WStringPieceFromLiteral(L"*.dll");
	if (!folder_path.ends_with(tail)) {
		if (L'\\' != *(folder_path.end() - 1))
			path.append(1, L'\\');
		path.append(tail.data(), tail.size());
	}
	
	unsigned int r = plugin_list->LoadPlugins(path);
	if (r < 1) {
		std::string path_ = cpcl::ConvertUTF16_CP1251(path);
		cpcl::Trace(CPCL_TRACE_LEVEL_WARNING,
			"PluginList::LoadPlugins(): no plugins found at '%s'",
			path_.c_str());
		plugin_list.reset();
	}
	return plugin_list.release();
}

void *HANDLE_DLL_MODULE = 0;
PluginList* PluginList::LoadPlugins() {
	std::auto_ptr<PluginList> plugin_list(new PluginList());

	std::wstring path;
	if (!cpcl::GetModuleFilePath(HANDLE_DLL_MODULE, &path))
		path = L".\\";
	else
		path += L'\\';

	/*LoadPlugins(cpcl::WStringPiece const &folder_path)
	unsigned int r = LoadPlugins(path + L"plugins\\*.dll"); // hmmmph - create temporary wstring and create temporary WStringPiece, use temporary wstring
	// All temporaries created as a result of expression evaluation are destroyed at the end of the expression statement (that is, at the semicolon), or at the end of the controlling expressions for for, if, while, do, and switch statements.
	// Initializing const references - This temporary object is destroyed immediately after the reference object to which it is bound is destroyed.*/
	// unsigned int r = plugin_list->LoadPlugins(path + L"plugins\\*.dll");
	unsigned int r = plugin_list->LoadPlugins(path + L"plugins_\\*.dll");
	if (r < 1)
		r = plugin_list->LoadPlugins(path + L"*.dll");
	if (r < 1) {
		std::string path_ = cpcl::ConvertUTF16_CP1251(path);
		cpcl::Trace(CPCL_TRACE_LEVEL_WARNING,
			"PluginList::LoadPlugins(): no plugins found at '%s\\plugins', '%s'",
			path_.c_str());
		plugin_list.reset();
	}
	return plugin_list.release();
}

bool PluginList::LoadPlugin(cpcl::WStringPiece const &dll_path) {
	size_t n = plugins.size();
	TryLoadPlugin(dll_path);
	return plugins.size() > n;
}
PluginList* PluginList::CreatePluginList() {
	return new PluginList();
}

//CPCL_DECLARE_INTERFACE("5325A8D0-1970-3C5D-C05F-F01E7781D9FC")
//IPdfPlugin : public IUnknown
//{
//public:
//  virtual HRESULT STDMETHODCALLTYPE SetFilenameHint( 
//      /* [in] */ wchar_t const *v,
//      /* [in] */ DWORD v_size) = 0;
//};
//
//bool PluginList::LoadDoc(IStreamWrapper input, IPluginDocument **r, cpcl::WStringPiece const &filepath_hint/*filename_hint*/) {
//	ScopedComPtr<IPluginDocument> doc;
//	
//	cpcl::WStringPiece extension_hint = cpcl::FileExtension(filepath_hint);
//	if (!extension_hint.empty()) {
//		Plugins::iterator i = plugins.find(extension_hint.as_string());
//		if (i != plugins.end()) {
//			cpcl::ComPtr<IPdfPlugin> plugin;
//			if ((*i).second->QueryInterface(__uuidof(IPdfPlugin), (void**)plugin.GetAddressOf()) == S_OK)
//				plugin->SetFilenameHint(filepath_hint.data(), filepath_hint.size());
//
//			(*i).second->LoadDocument(input, &doc);
//		}
//	}
//
//	if (!doc) {
//		for (Plugins::iterator i = plugins.begin(); i != plugins.end(); ++i) {
//			cpcl::ComPtr<IPdfPlugin> plugin;
//			if ((*i).second->QueryInterface(__uuidof(IPdfPlugin), (void**)plugin.GetAddressOf()) == S_OK)
//				plugin->SetFilenameHint(filepath_hint.data(), filepath_hint.size());
//
//			if ((*i).second->LoadDocument(input, &doc) == S_OK)
//				break;
//		}
//	}
//
//	if (!doc)
//		return false;
//	if (r)
//		*r = doc.Detach();
//	return true;
//}

bool PluginList::LoadDoc(IStream *input, IPluginDocument **r) {
	ScopedComPtr<IPluginDocument> doc;

	if (file_signature_filter.get()) {
		cpcl::IStreamWrapper input_(input);
		cpcl::ComPtr<IPlugin> plugin = file_signature_filter->Match(&input_);
		if (!!plugin) {
			THROW_FAILED(plugin->LoadDocument(input, &doc));
			/* signature match and plugin found, but unable to load document, exit */
			if (!doc)
				return false;
		}
	}

	if (!doc) {
		for (Plugins::const_iterator i = plugins.begin(); i != plugins.end(); ++i) {
			LARGE_INTEGER offset; offset.QuadPart = 0;
			THROW_FAILED(input->Seek(offset, STREAM_SEEK_SET, 0));

			HRESULT const hr = (*i)->LoadDocument(input, &doc);
			if (hr == S_OK)
				break;
			else if (hr < 0) {
				hresult_exception::throw_formatted(hresult_exception(hr),
					"PluginList::LoadDoc(): (*i)->LoadDocument(input, &doc) fails with code: 0x%08X",
					hr);
			}
			/*if ((*i)->LoadDocument(input, &doc) == S_OK)
				break;*/
		}
	}

	if (!doc)
		return false;
	if (r)
		*r = doc.Detach();
	return true;
}

bool PluginList::LoadDoc(IStream *input, IPluginDocument **r, cpcl::WStringPiece const &filepath_hint) {
	ScopedComPtr<IPluginDocument> doc;

	if (file_extension_filter.get()) {
		cpcl::WStringPiece extension_hint = cpcl::FileExtension(filepath_hint);
		if (!extension_hint.empty()) {
			cpcl::ComPtr<IPlugin> plugin = file_extension_filter->Match(extension_hint);
			if (!!plugin) {
				THROW_FAILED(plugin->LoadDocument(input, &doc));
			}
		}
	}

	if (!doc) {
		return LoadDoc(input, r);
	} else {
		if (r)
			*r = doc.Detach();
		return true;
	}
}
