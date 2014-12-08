// plugin_filter.h
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

#include <basic.h>

#include <vector>
#include <list>
#include <map>

#include <string_piece.hpp>
#include <io_stream.h>
#include <com_ptr.hpp>

#include <plugin.h>

#include "plugin_interface.h"

class PluginFilter {
public:
	PluginFilter();
	virtual ~PluginFilter();

	virtual IPlugin* Match(cpcl::WStringPiece const &v) const;
	virtual IPlugin* Match(cpcl::IOStream *v) const;
};

// IFileFilterImpl(PluginFilter *, int match_method = [0,1])

class FileExtensionFilter : public PluginFilter {
	struct WStringPieceLess {
		bool operator()(cpcl::WStringPiece const &l, cpcl::WStringPiece const &r) const;
	};

	typedef std::map<cpcl::WStringPiece, cpcl::ComPtr<IPlugin>, WStringPieceLess> Plugins; // WStringPiece points to IPlugin::DocExtension, so keep Plugin objects
	Plugins plugins;

	FileExtensionFilter();
	// why ??? DISALLOW_COPY_AND_ASSIGN(FileExtensionFilter); - plugins copy is ok
public:
	virtual ~FileExtensionFilter();

	virtual IPlugin* Match(cpcl::WStringPiece const &v) const;

	static FileExtensionFilter* Create(std::vector<cpcl::ComPtr<IPlugin> > const &plugins_);
};

class SignatureFilter : public PluginFilter {
	plcl::Plugin::SignatureInfo **signatures;
	unsigned int n_signatures;
	unsigned int max_data_offset;
	std::vector<unsigned int> indices;
	std::vector<cpcl::ComPtr<IPlugin> > plugins;

	SignatureFilter();
	// why ??? DISALLOW_COPY_AND_ASSIGN(SignatureFilter); 
public:
	SignatureFilter(SignatureFilter const &r); // signatures must be copied
	SignatureFilter& operator=(SignatureFilter const &r);
	virtual ~SignatureFilter();
	
	virtual IPlugin* Match(cpcl::IOStream *v) const;

	static SignatureFilter* Create(std::vector<cpcl::ComPtr<IPlugin> > const &plugins_);
};
