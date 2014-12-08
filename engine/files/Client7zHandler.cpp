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

//#include <hash_set>
//#include <set>
#include <map>

#define WIN32_LEAN_AND_MEAN
#define INC_OLE2

#include <Common/MyString.h>
//#include "Common/StringConvert.h" // GetOemString
#include <Common/Buffer.h>

#include <Windows/PropVariant.h>

#include "Client7zHandler.h"
#include "CInArchiveImpl.h"
#include "CArchiveCallback.h"

#include "temporaryStream.h"
#include "CStreamWrapper.h"

namespace files {

static const GUID usedArchives[] = {
	{ 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0x04, 0x00, 0x00 } }, // Arj
	{ 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0x02, 0x00, 0x00 } }, // bzip2
	{ 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0xEF, 0x00, 0x00 } }, // gzip
	{ 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0x06, 0x00, 0x00 } }, // Lzh
	{ 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0x0A, 0x00, 0x00 } }, // lzma
	{ 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0x0B, 0x00, 0x00 } }, // lzma86
	// { 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0xE1, 0x00, 0x00 } }, // Xar
	{ 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0x0C, 0x00, 0x00 } }, // xz
	{ 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0x05, 0x00, 0x00 } }, // Z
	{ 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00 } }, // 7z
	{ 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0x03, 0x00, 0x00 } }, // Rar
	{ 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0xEE, 0x00, 0x00 } }, // tar
	{ 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0x01, 0x00, 0x00 } }  // zip
};

//struct IID_IInArchive_2_Exts_t {
//	GUID id;
//	wchar_t const* ext;
//};
//static const IID_IInArchive_2_Exts_t IID_IInArchive_2_Exts[] = {
//	{ { 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0x02, 0x00, 0x00 } }, L"tar.bz2" }, // bzip2
//	{ { 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0xEF, 0x00, 0x00 } }, L"tar.gz" }, // gzip
//	{ { 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0x0C, 0x00, 0x00 } }, L"tar.xz" }, // xz
//	{ { 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0x05, 0x00, 0x00 } }, L"tar.z" }, // Z
//};

inline static UString GetDefaultName3(const UString &fileName,
    const UString &extension, const UString &addSubExtension)
{
  int extLength = extension.Length();
  int fileNameLength = fileName.Length();
  if (fileNameLength > extLength + 1)
  {
    int dotPos = fileNameLength - (extLength + 1);
    if (fileName[dotPos] == L'.')
      if (extension.CompareNoCase(fileName.Mid(dotPos + 1)) == 0)
        return fileName.Left(dotPos) + addSubExtension;
  }
  int dotPos = fileName.ReverseFind(L'.');
  if (dotPos > 0)
    return fileName.Left(dotPos) + addSubExtension;

  if (addSubExtension.IsEmpty())
    return fileName + L"~";
  else
    return fileName + addSubExtension;
}

std::wstring GetDefaultName2(const UString &fileName, // { name : archive.tar, ext : gz } <- archive.tar.gz
    const UString &extension, const UString &addSubExtension)
{
	UString name = GetDefaultName3(fileName, extension, addSubExtension);
  name.TrimRight();
	return std::wstring((wchar_t const*)name, name.Length());
}

//////////////////////////////////////////////////////////////
// Archive codecs

struct CArchiveExtensionInfo
{
  UString Ext;
  UString AddExt;
  CArchiveExtensionInfo() {}
  CArchiveExtensionInfo(const UString &ext): Ext(ext) {}
  CArchiveExtensionInfo(const UString &ext, const UString &addExt): Ext(ext), AddExt(addExt) {}
};

inline static void SplitString(const UString &srcString, UStringVector &destStrings)
{
  destStrings.Clear();
  UString s;
  int len = srcString.Length();
  if (len == 0)
    return;
  for (int i = 0; i < len; i++)
  {
    wchar_t c = srcString[i];
    if (c == L' ')
    {
      if (!s.IsEmpty())
      {
        destStrings.Add(s);
        s.Empty();
      }
    }
    else
      s += c;
  }
  if (!s.IsEmpty())
    destStrings.Add(s);
}

struct CArchiveExtensionInfoEx
{
  int LibIndex;
  UInt32 FormatIndex;
  CLSID ClassID;
  bool UpdateEnabled;
  UString Name;
  CObjectVector<CArchiveExtensionInfo> Exts;
  CByteBuffer StartSignature;
  bool KeepName;

  UString GetMainExt() const
  {
    if (Exts.IsEmpty())
      return UString();
    return Exts[0].Ext;
  }
  int FindExtension(const UString &ext) const
  {
    for (int i = 0; i < Exts.Size(); i++)
      if (ext.CompareNoCase(Exts[i].Ext) == 0)
        return i;
    return -1;
  }
  UString GetAllExtensions() const
  {
    UString s;
    for (int i = 0; i < Exts.Size(); i++)
    {
      if (i > 0)
        s += ' ';
      s += Exts[i].Ext;
    }
    return s;
  }
	UString GetAllAdditionExtensions() const
  {
    UString s;
    for (int i = 0; i < Exts.Size(); i++)
    {
			if (i > 0)
				s += '|';
			if (Exts[i].AddExt.Length() > 0)
				s += Exts[i].AddExt;
			else
				s += L"   ";
    }
    return s;
  }

	void AddExts(const wchar_t* ext, const wchar_t* addExt)
	{
		UStringVector exts, addExts;
		if (ext != 0)
			SplitString(ext, exts);
		if (addExt != 0)
			SplitString(addExt, addExts);

		for (int i = 0; i < exts.Size(); i++)
		{
			CArchiveExtensionInfo extInfo;
			extInfo.Ext = exts[i];
			if (i < addExts.Size())
			{
				extInfo.AddExt = addExts[i];
				if (extInfo.AddExt == L"*")
					extInfo.AddExt.Empty();
			}
			Exts.Add(extInfo);
		}
	}

  CArchiveExtensionInfoEx():
    LibIndex(-1),
    UpdateEnabled(false),
    KeepName(false)
  {}
};

inline static std::wstring& toLowerASCII(std::wstring &r) {
	for (std::wstring::size_type i = 0; i < r.size(); ++i) {
		wchar_t &c = r[i];
		if ((c >= L'A') && (c <= L'Z'))
			c += 0x20;
	}
	return r;
}

class CArchiveCodecs {
	typedef UInt32 (WINAPI *GetMethodPropertyFunc)(UInt32 index, PROPID propID, PROPVARIANT *value);
	typedef UInt32 (WINAPI *CreateObjectFunc)(const GUID *clsID, const GUID *interfaceID, void **outObject);
	
	typedef UInt32 (WINAPI *GetNumberOfMethodsFunc)(UInt32 *numMethods);
	typedef UInt32 (WINAPI *GetNumberOfFormatsFunc)(UInt32 *numFormats);
	typedef UInt32 (WINAPI *GetHandlerPropertyFunc)(PROPID propID, PROPVARIANT *value);
	typedef UInt32 (WINAPI *GetHandlerPropertyFunc2)(UInt32 index, PROPID propID, PROPVARIANT *value);
	typedef UInt32 (WINAPI *SetLargePageModeFunc)();

	HRESULT ReadProp(GetHandlerPropertyFunc getProp, GetHandlerPropertyFunc2 getProp2,
		UInt32 index, PROPID propID, NWindows::NCOM::CPropVariant &prop) {
		if (getProp2)
			return getProp2(index, propID, &prop);;
		return getProp(propID, &prop);
	}
	HRESULT ReadBoolProp(GetHandlerPropertyFunc getProp, GetHandlerPropertyFunc2 getProp2,
		UInt32 index, PROPID propID, bool &res) {
		NWindows::NCOM::CPropVariant prop;
		RINOK(ReadProp(getProp, getProp2, index, propID, prop))
		if (prop.vt == VT_BOOL)
			res = (prop.boolVal != VARIANT_FALSE);
		else if (prop.vt != VT_EMPTY)
			return E_FAIL;
		return S_OK;
	}
	HRESULT ReadStringProp(GetHandlerPropertyFunc getProp, GetHandlerPropertyFunc2 getProp2,
		UInt32 index, PROPID propID, UString &res) {
		NWindows::NCOM::CPropVariant prop;
		RINOK(ReadProp(getProp, getProp2, index, propID, prop))
		if (prop.vt == VT_BSTR)
			res = prop.bstrVal;
		else if (prop.vt != VT_EMPTY)
			return E_FAIL;
		return S_OK;
	}
public:
	CreateObjectFunc createObjectFunc_;
	CObjectVector<CArchiveExtensionInfoEx> Formats;
	//stdext::hash_set<std::wstring> exts;
	std::map<std::wstring, int> exts;

	CArchiveCodecs() : createObjectFunc_(NULL) {}
	
	HRESULT LoadFormats(scopedSharedLibrary const &lib) {
		GetHandlerPropertyFunc getProp = 0;
		GetHandlerPropertyFunc2 getProp2 = (GetHandlerPropertyFunc2)lib.getFunction("GetHandlerProperty2");
		if (getProp2 == NULL)
		{
			getProp = (GetHandlerPropertyFunc)lib.getFunction("GetHandlerProperty");
			if (getProp == NULL)
				return S_OK;
		}

		UInt32 numFormats = 1;
		GetNumberOfFormatsFunc getNumberOfFormats = (GetNumberOfFormatsFunc)lib.getFunction("GetNumberOfFormats");
		if (getNumberOfFormats != NULL)
		{
			RINOK(getNumberOfFormats(&numFormats))
		}
		if (getProp2 == NULL)
			numFormats = 1;

		for (UInt32 i = 0; i < numFormats; i++)
		{
			CArchiveExtensionInfoEx item;
			item.LibIndex = 0;
			item.FormatIndex = i;

			RINOK(ReadStringProp(getProp, getProp2, i, NArchive::kName, item.Name))

			NWindows::NCOM::CPropVariant prop;
			if (ReadProp(getProp, getProp2, i, NArchive::kClassID, prop) != S_OK)
				continue;
			if (prop.vt != VT_BSTR)
				continue;
			item.ClassID = *(const GUID *)prop.bstrVal;
			prop.Clear();

			UString ext, addExt;
			RINOK(ReadStringProp(getProp, getProp2, i, NArchive::kExtension, ext));
			RINOK(ReadStringProp(getProp, getProp2, i, NArchive::kAddExtension, addExt));
			item.AddExts(ext, addExt);

			ReadBoolProp(getProp, getProp2, i, NArchive::kUpdate, item.UpdateEnabled);
			if (item.UpdateEnabled)
				ReadBoolProp(getProp, getProp2, i, NArchive::kKeepName, item.KeepName);

			if (ReadProp(getProp, getProp2, i, NArchive::kStartSignature, prop) == S_OK)
				if (prop.vt == VT_BSTR)
				{
					UINT len = ::SysStringByteLen(prop.bstrVal);
					item.StartSignature.SetCapacity(len);
					memmove(item.StartSignature, prop.bstrVal, len);
				}
				Formats.Add(item);
		}
		return S_OK;
	}
	HRESULT Load(scopedSharedLibrary const &lib) {
		createObjectFunc_ = (CreateObjectFunc)lib.getFunction("CreateObject");
		if (!createObjectFunc_)
			return E_NOINTERFACE;
		return LoadFormats(lib);
	}
	HRESULT CreateArchiveHandler(int formatIndex, IInArchive **archive) {
		if (!createObjectFunc_)
			return E_NOINTERFACE;
		if ((formatIndex < 0) || (formatIndex >= Formats.Size()))
			return E_INVALIDARG;

		CArchiveExtensionInfoEx const &v = Formats[formatIndex];
		return createObjectFunc_(&v.ClassID, &IID_IInArchive, (void**)archive);
	}

	int isArchiveExt(std::wstring const &v) {
		if (exts.empty()) {
			for (int i = 0; i < Formats.Size(); ++i) {
				for (int j = 0; j < Formats[i].Exts.Size(); ++j) {
					std::wstring ext((const wchar_t*)Formats[i].Exts[j].Ext, Formats[i].Exts[j].Ext.Length());
					toLowerASCII(ext);

					exts.insert(std::map<std::wstring, int>::value_type(ext, i));
				}

				//for (int j = 0; j < arraysize(IID_IInArchive_2_Exts); ++j) {
				//	if (::IsEqualGUID(Formats[i].ClassID, IID_IInArchive_2_Exts[j].id) == TRUE)
				//		exts.insert(std::map<std::wstring, int>::value_type(std::wstring(IID_IInArchive_2_Exts[j].ext), i));
				//}
			}
		}

		std::map<std::wstring, int>::const_iterator extIt = exts.find(v);
		return (extIt != exts.end()) ? (*extIt).second : -1;
	}
};

//////////////////////////////////////////////////////////////
// Client 7z handler 

Client7zHandler::Client7zHandler(IUnknown *messageSink) : messageSink_(messageSink) {
	archiveCodecs = new CArchiveCodecs();
	archiveOpenCallback = new CArchiveOpenCallback(messageSink, 0); archiveOpenCallback->AddRef();
}

Client7zHandler::~Client7zHandler() {
	delete archiveCodecs;
	delete archiveOpenCallback;
}

HRESULT Client7zHandler::load7zHandler() {
	if ((client7zLibrary) && (archiveCodecs->Formats.Size() > 0))
		return S_OK;

	client7zLibrary.reset(::LoadLibraryW(L"7z.dll"));
	if (!client7zLibrary)
		return E_NOINTERFACE;
	RINOK(archiveCodecs->Load(client7zLibrary))

	CObjectVector<CArchiveExtensionInfoEx> Formats;
	for (int i = 0; i < archiveCodecs->Formats.Size(); ++i) {
		for (int j = 0; j < arraysize(usedArchives); ++j) {
			if (::IsEqualGUID(archiveCodecs->Formats[i].ClassID, usedArchives[j]) == TRUE) {
				Formats.Add(archiveCodecs->Formats[i]);
				break;
			}
		}
	}
	archiveCodecs->Formats = Formats;

	//for (int i = 0; i < archiveCodecs->Formats.Size(); ++i) {
	//	printf("%s:\n %s\n %s\n", (const char*)GetOemString(archiveCodecs->Formats[i].Name), 
	//	(const char*)GetOemString(archiveCodecs->Formats[i].GetAllExtensions()),
	//	(const char*)GetOemString(archiveCodecs->Formats[i].GetAllAdditionExtensions()));
	//}
	return S_OK;
}

bool Client7zHandler::isArchiveExtension(fileItemInfo const &v) {
	std::wstring ext = v.fileExt_;
	toLowerASCII(ext);
	return (archiveCodecs->isArchiveExt(ext) >= 0);
}

//HRESULT Client7zHandler::openArchive(fileItemInfo &v, CInArchive **r) {
//	if (!v.stream) {
//		RINOK(fileStreamRead(v.path.c_str(), &v.stream))
//	}
//
//	UString extension(v.fileExt_.c_str()), fileName(v.fileName_.c_str());
//	CIntVector orderIndices;
//	int numFinded = 0;
//	for (int i = 0; i < archiveCodecs->Formats.Size(); ++i) {
//		if (archiveCodecs->Formats[i].FindExtension(extension) >= 0)
//			orderIndices.Insert(numFinded++, i);
//		else
//			orderIndices.Add(i);
//	}
//
//	CMyComPtr<IInStream> stream = new CStreamWrapper(v.stream);
//	for (int i = 0; i < orderIndices.Size(); i++) {
//		RINOK(stream->Seek(0, STREAM_SEEK_SET, NULL))
//			
//		CMyComPtr<IInArchive> archive;
//		int FormatIndex = orderIndices[i];
//		RINOK(archiveCodecs->CreateArchiveHandler(FormatIndex, &archive))
//		if (!archive)
//			continue;
//
//		HRESULT result = archive->Open(stream, 0, archiveOpenCallback);
//		if (result == S_FALSE)
//			continue;
//		RINOK(result)
//
//		std::wstring defaultName;
//		CArchiveExtensionInfoEx const &format = archiveCodecs->Formats[FormatIndex];
//		if (format.Exts.Size() == 0)
//			defaultName = GetDefaultName2(fileName, L"", L"");
//		else {
//			int subExtIndex = format.FindExtension(extension);
//			if (subExtIndex < 0)
//				subExtIndex = 0;
//			
//			CArchiveExtensionInfo const &extInfo = format.Exts[subExtIndex];
//			defaultName = GetDefaultName2(fileName, extInfo.Ext, extInfo.AddExt);
//		}
//		if (defaultName.empty())
//			defaultName = L"[Content]";
//
//		//*r = new CInArchiveImpl(archive, defaultName.c_str(), defaultName.size());
//		*r = new CInArchiveImpl(archive, defaultName);
//		return S_OK;
//	}
//
//	return E_FAIL;
//}
HRESULT Client7zHandler::openArchive(fileItemInfo &v, CInArchive **r) {
	int FormatIndex = archiveCodecs->isArchiveExt(v.fileExt_);
	if (FormatIndex < 0)
		return E_FAIL;

	CMyComPtr<IInArchive> archive;
	RINOK(archiveCodecs->CreateArchiveHandler(FormatIndex, &archive))
	if (!archive)
		return E_FAIL;

	if (!v.stream) {
		RINOK(fileStreamRead(v.path.c_str(), &v.stream))
	}
	CMyComPtr<IInStream> stream = new CStreamWrapper(v.stream);
	RINOK(stream->Seek(0, STREAM_SEEK_SET, NULL))
	
	HRESULT result = archive->Open(stream, 0, archiveOpenCallback);
	if (result == S_FALSE)
		return E_FAIL;
	RINOK(result)
	
	UString fileName(v.fileName_.c_str());
	std::wstring defaultName;
	CArchiveExtensionInfoEx const &format = archiveCodecs->Formats[FormatIndex];
	if (format.Exts.Size() == 0)
		defaultName = GetDefaultName2(fileName, L"", L"");
	else {
		UString extension(v.fileExt_.c_str());
		int subExtIndex = format.FindExtension(extension);
		if (subExtIndex < 0)
			subExtIndex = 0;
		
		CArchiveExtensionInfo const &extInfo = format.Exts[subExtIndex];
		defaultName = GetDefaultName2(fileName, extInfo.Ext, extInfo.AddExt);
	}
	if (defaultName.empty())
		defaultName = L"[Content]";
	
	*r = new CInArchiveImpl(archive, defaultName);
	return S_OK;
}

} // namespace files
