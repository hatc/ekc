// plugin_list.h
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

#include <vector>

#include <io_stream.h>
#include <com_ptr.hpp>

#include "plugin_filter.h"

class PluginList {
	/*typedef std::map<std::wstring, cpcl::ComPtr<IPlugin> > Plugins;*/
	//struct WStringPieceLess {
	//	bool operator()(cpcl::WStringPiece const &l, cpcl::WStringPiece const &r) const;
	//};
	//typedef std::map<cpcl::WStringPiece, cpcl::ComPtr<IPlugin>, WStringPieceLess> Plugins;
	void TryLoadPlugin(cpcl::WStringPiece const &dll_path);
	unsigned int LoadPlugins(std::wstring const &folder_path);

	PluginList();
	DISALLOW_COPY_AND_ASSIGN(PluginList);
public:
	// this fields used by IPluginListImpl
	typedef std::vector<cpcl::ComPtr<IPlugin> > Plugins;
	Plugins plugins;
	std::auto_ptr<FileExtensionFilter> file_extension_filter;
	std::auto_ptr<SignatureFilter> file_signature_filter;

	//bool LoadDoc(IStreamWrapper input, IPluginDocument **r, cpcl::WStringPiece const &extension_hint);
	//bool LoadDoc(IStreamWrapper input, IPluginDocument **r, cpcl::WStringPiece const &filepath_hint);
	bool LoadDoc(IStream *input, IPluginDocument **r);
	bool LoadDoc(IStream *input, IPluginDocument **r, cpcl::WStringPiece const &filepath_hint);

	// this methods called by IEngine::LoadPlugins()
	static PluginList* LoadPlugins(cpcl::WStringPiece const &folder_path);
	static PluginList* LoadPlugins();

	// for test
	bool LoadPlugin(cpcl::WStringPiece const &dll_path);
	static PluginList* CreatePluginList();
};
