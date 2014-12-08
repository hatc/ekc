// Copyright (C) 2011 Yuri Agafonov
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
#include <string>

//#include <windows.h>

/*
C:\Source\disk_analysis\7z920\CPP\Windows
C:\Source\disk_analysis\7z920\CPP\Common
C:\Source\disk_analysis\7z920\CPP\7zip\Common
*/

#include "Common/IntToString.h"
#include "Common/MyInitGuid.h"
#include "Common/StringConvert.h"
#include "Common/Buffer.h"

#include "Windows/FileDir.h"
#include "Windows/FileFind.h"
#include "Windows/FileName.h"
#include "Windows/PropVariant.h"
#include "Windows/PropVariantConversions.h"

#include "7zip/Common/FileStreams.h"

#include "7zip/Archive/IArchive.h"

#include "7zip/IPassword.h"
#include "7zip/ICoder.h"

#include "scopedSharedLibrary.hpp"
#include "temporaryStream.h"
#include "CStreamWrapper.h"
#include "CStreamPretend.h"

//#error "correct && update definition"

/* archive::Open *

int i;
int numFinded = 0;
for (i = 0; i < codecs->Formats.Size(); i++)
 if (codecs->Formats[i].FindExtension(extension) >= 0)
	 orderIndices.Insert(numFinded++, i);
 else
	 orderIndices.Add(i);

for (int i = 0; i < orderIndices.Size(); i++) {    
 RINOK(stream->Seek(0, STREAM_SEEK_SET, NULL));
    
 CMyComPtr<IInArchive> archive;

 FormatIndex = orderIndices[i];
 RINOK(codecs->CreateInArchive(FormatIndex, archive));
 if (!archive)
  continue;

 HRESULT result = archive->Open(stream, &kMaxCheckStartPosition, callback);
 if (result == S_FALSE)
  continue;
 RINOK(result);

 const CArcInfoEx &format = codecs->Formats[FormatIndex];
 if (format.Exts.Size() == 0)
  DefaultName = GetDefaultName2(fileName, L"", L"");
 else {
  int subExtIndex = format.FindExtension(extension);
  if (subExtIndex < 0) 
		subExtIndex = 0;
 
  const CArcExtInfo &extInfo = format.Exts[subExtIndex];
  DefaultName = GetDefaultName2(fileName, extInfo.Ext, extInfo.AddExt);
 }
}
return S_OK;

* CArchiveExtractCallback::SetOperationResult *

...
UString filePath;
RINOK(archiveHandler_->itemInfo(index_, &itemInfo_); // return error code if cannot get item name - i.e. cannot proceed if item name unknown === fileItem name always defined
...

*/

// use another CLSIDs, if you want to support other formats (zip, rar, ...).
// {23170F69-40C1-278A-1000-000110070000}
DEFINE_GUID(CLSID_CFormat7z,
  0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00);


static const GUID usedArchives[] = {
	{ 0x23170F69, 0x40C1, 0x278A, { 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00 } },
	{ 0x23170F69, 0x40C1, 0x278B, { 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0xFF, 0x00 } }
};

using namespace NWindows;

#define kDllName "7z.dll"

typedef UInt32 (WINAPI *GetMethodPropertyFunc)(UInt32 index, PROPID propID, PROPVARIANT *value);
typedef UInt32 (WINAPI *CreateObjectFunc)(const GUID *clsID, const GUID *interfaceID, void **outObject);

typedef UInt32 (WINAPI *GetNumberOfMethodsFunc)(UInt32 *numMethods);
typedef UInt32 (WINAPI *GetNumberOfFormatsFunc)(UInt32 *numFormats);
typedef UInt32 (WINAPI *GetHandlerPropertyFunc)(PROPID propID, PROPVARIANT *value);
typedef UInt32 (WINAPI *GetHandlerPropertyFunc2)(UInt32 index, PROPID propID, PROPVARIANT *value);
typedef UInt32 (WINAPI *SetLargePageModeFunc)();

/***/

static FILE *printFile_ = NULL;

void PrintString(const UString &s)
{
  fprintf(printFile_, "%s", (LPCSTR)GetOemString(s));
}
void PrintString(const AString &s)
{
  fprintf(printFile_, "%s", (LPCSTR)s);
}
//void PrintString(const UString &s)
//{
//  printf("%s", (LPCSTR)GetOemString(s));
//}
//void PrintString(const AString &s)
//{
//  printf("%s", (LPCSTR)s);
//}
void PrintNewLine()
{
  PrintString("\n");
}
void PrintStringLn(const AString &s)
{
  PrintString(s);
  PrintNewLine();
}
void PrintError(const AString &s)
{
  PrintNewLine();
  PrintString(s);
  PrintNewLine();
}

/***/

static HRESULT IsArchiveItemProp(IInArchive *archive, UInt32 index, PROPID propID, bool &result)
{
  NCOM::CPropVariant prop;
  RINOK(archive->GetProperty(index, propID, &prop));
  if (prop.vt == VT_BOOL)
    result = VARIANT_BOOLToBool(prop.boolVal);
  else if (prop.vt == VT_EMPTY)
    result = false;
  else
    return E_FAIL;
  return S_OK;
}

static HRESULT IsArchiveItemFolder(IInArchive *archive, UInt32 index, bool &result)
{
  return IsArchiveItemProp(archive, index, kpidIsDir, result);
}


static const wchar_t *kEmptyFileAlias = L"[Content]";

/***/

//////////////////////////////////////////////////////////////
// Archive Open callback
class CArchiveOpenCallback:
  public IArchiveOpenCallback,
  public ICryptoGetTextPassword,
  public CMyUnknownImp
{
public:
  MY_UNKNOWN_IMP1(ICryptoGetTextPassword)

  STDMETHOD(SetTotal)(const UInt64 *files, const UInt64 *bytes);
  STDMETHOD(SetCompleted)(const UInt64 *files, const UInt64 *bytes);

  STDMETHOD(CryptoGetTextPassword)(BSTR *password);

  bool PasswordIsDefined;
  UString Password;

  CArchiveOpenCallback() : PasswordIsDefined(false) {}
};

STDMETHODIMP CArchiveOpenCallback::SetTotal(const UInt64 * /* files */, const UInt64 * /* bytes */)
{
  return S_OK;
}

STDMETHODIMP CArchiveOpenCallback::SetCompleted(const UInt64 * /* files */, const UInt64 * /* bytes */)
{
  return S_OK;
}
  
STDMETHODIMP CArchiveOpenCallback::CryptoGetTextPassword(BSTR *password)
{
  if (!PasswordIsDefined)
  {
    // You can ask real password here from user
    // Password = GetPassword(OutStream);
    // PasswordIsDefined = true;
    PrintError("Password is not defined");
    return E_ABORT;
  }
  return StringToBstr(Password, password);
}

//////////////////////////////////////////////////////////////
// Archive Extracting callback
static const wchar_t *kCantDeleteOutputFile = L"ERROR: Can not delete output file ";

static const char *kTestingString    =  "Testing     ";
static const char *kExtractingString =  "Extracting  ";
static const char *kSkippingString   =  "Skipping    ";

static const char *kUnsupportedMethod = "Unsupported Method";
static const char *kCRCFailed = "CRC Failed";
static const char *kDataError = "Data Error";
static const char *kUnknownError = "Unknown Error";

class CArchiveExtractCallback:
  public IArchiveExtractCallback,
  public ICryptoGetTextPassword,
  public CMyUnknownImp
{
public:
  MY_UNKNOWN_IMP1(ICryptoGetTextPassword)

  // IProgress
  STDMETHOD(SetTotal)(UInt64 size);
  STDMETHOD(SetCompleted)(const UInt64 *completeValue);

  // IArchiveExtractCallback
  STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream **outStream, Int32 askExtractMode);
  STDMETHOD(PrepareOperation)(Int32 askExtractMode);
  STDMETHOD(SetOperationResult)(Int32 resultEOperationResult);

  // ICryptoGetTextPassword
  STDMETHOD(CryptoGetTextPassword)(BSTR *aPassword);

private:
  CMyComPtr<IInArchive> _archiveHandler;
  UString _directoryPath;  // Output directory
  UString _filePath;       // name inside arcvhive
  UString _diskFilePath;   // full path to file on disk
  bool _extractMode;
  struct CProcessedFileInfo
  {
    FILETIME MTime;
    UInt32 Attrib;
    bool isDir;
    bool AttribDefined;
    bool MTimeDefined;
  } _processedFileInfo;

  COutFileStream *_outFileStreamSpec;
  CMyComPtr<ISequentialOutStream> _outFileStream;

public:
  void Init(IInArchive *archiveHandler, const UString &directoryPath);

  UInt64 NumErrors;
  bool PasswordIsDefined;
  UString Password;

	CArchiveExtractCallback() : PasswordIsDefined(false), _outFileStreamSpec(NULL) { _outFileStream = (ISequentialOutStream*)0; /*new CSequentialOutStreamPretend();*/ }
};

void CArchiveExtractCallback::Init(IInArchive *archiveHandler, const UString &directoryPath)
{
  NumErrors = 0;
  _archiveHandler = archiveHandler;
  _directoryPath = directoryPath;
  NFile::NName::NormalizeDirPathPrefix(_directoryPath);
}

STDMETHODIMP CArchiveExtractCallback::SetTotal(UInt64 /* size */)
{
  return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::SetCompleted(const UInt64 * /* completeValue */)
{
  return S_OK;
}

UInt64 nothrowConvertPropVariantToUInt64(const PROPVARIANT &prop)
{
  switch (prop.vt)
  {
    case VT_UI1: return prop.bVal;
    case VT_UI2: return prop.uiVal;
    case VT_UI4: return prop.ulVal;
    case VT_UI8: return prop.uhVal.QuadPart;
    case VT_I2: return prop.iVal;
    case VT_I4: return prop.lVal;
    case VT_I8: return prop.hVal.QuadPart;
    default: return 0;
  }
}

STDMETHODIMP CArchiveExtractCallback::GetStream(UInt32 index,
    ISequentialOutStream **outStream, Int32 askExtractMode)
{
	// file order same as:
	//UInt32 numItems = 0;
	//archive->GetNumberOfItems(&numItems);
	//for (UInt32 i = 0; i < numItems; i++) { ... }
	//FILE *f = fopen("list_.txt", "a");
	//{
	//	// Get uncompressed size of file
	//	NCOM::CPropVariant prop;
	//	_archiveHandler->GetProperty(index, kpidSize, &prop);
	//	UString s = ConvertPropVariantToString(prop);
	//	fprintf(f, "%s", (LPCSTR)GetOemString(s));
	//	fprintf(f, "  ");
	//}
	//{
	//	// Get name of file
	//	NCOM::CPropVariant prop;
	//	_archiveHandler->GetProperty(index, kpidPath, &prop);
	//	UString s = ConvertPropVariantToString(prop);
	//	fprintf(f, "%s", (LPCSTR)GetOemString(s));
	//}
	//fprintf(f, "\n");
	//if (f)
	//	fclose(f);

	UInt64 estimatedSizeByte(0);
	{
		// Get uncompressed size of file
		NCOM::CPropVariant prop;
		_archiveHandler->GetProperty(index, kpidSize, &prop);

		estimatedSizeByte = nothrowConvertPropVariantToUInt64(prop);
		
		UString s = ConvertPropVariantToString(prop);
		PrintString(s);
		PrintString("  ");
	}
	{
		// Get name of file
		NCOM::CPropVariant prop;
		_archiveHandler->GetProperty(index, kpidPath, &prop);
		UString s = ConvertPropVariantToString(prop);
		PrintString(s);
	}
	PrintString("\n");

	/*CMyComPtr<IStream> stream_;
	RINOK(files::temporaryStream(estimatedSizeByte, &stream_))
	CMyComPtr<ISequentialOutStream> outStream_ = new files::CStreamWrapper(stream_);*/
	CMyComPtr<ISequentialOutStream> outStream_ = new files::CSequentialOutStreamPretend();
	*outStream = outStream_.Detach();
	return S_OK;

  *outStream = 0;
  _outFileStream.Release();

  {
    // Get Name
    NCOM::CPropVariant prop;
    RINOK(_archiveHandler->GetProperty(index, kpidPath, &prop));
    
    UString fullPath;
    if (prop.vt == VT_EMPTY)
      fullPath = kEmptyFileAlias;
    else
    {
      if (prop.vt != VT_BSTR)
        return E_FAIL;
      fullPath = prop.bstrVal;
    }
    _filePath = fullPath;
  }

  if (askExtractMode != NArchive::NExtract::NAskMode::kExtract)
    return S_OK;

  {
    // Get Attrib
    NCOM::CPropVariant prop;
    RINOK(_archiveHandler->GetProperty(index, kpidAttrib, &prop));
    if (prop.vt == VT_EMPTY)
    {
      _processedFileInfo.Attrib = 0;
      _processedFileInfo.AttribDefined = false;
    }
    else
    {
      if (prop.vt != VT_UI4)
        return E_FAIL;
      _processedFileInfo.Attrib = prop.ulVal;
      _processedFileInfo.AttribDefined = true;
    }
  }

  RINOK(IsArchiveItemFolder(_archiveHandler, index, _processedFileInfo.isDir));

  {
    // Get Modified Time
    NCOM::CPropVariant prop;
    RINOK(_archiveHandler->GetProperty(index, kpidMTime, &prop));
    _processedFileInfo.MTimeDefined = false;
    switch(prop.vt)
    {
      case VT_EMPTY:
        // _processedFileInfo.MTime = _utcMTimeDefault;
        break;
      case VT_FILETIME:
        _processedFileInfo.MTime = prop.filetime;
        _processedFileInfo.MTimeDefined = true;
        break;
      default:
        return E_FAIL;
    }

  }
  {
    // Get Size
    NCOM::CPropVariant prop;
    RINOK(_archiveHandler->GetProperty(index, kpidSize, &prop));
    bool newFileSizeDefined = (prop.vt != VT_EMPTY);
    UInt64 newFileSize;
    if (newFileSizeDefined)
      newFileSize = ConvertPropVariantToUInt64(prop);
  }

  
  {
    // Create folders for file
    int slashPos = _filePath.ReverseFind(WCHAR_PATH_SEPARATOR);
    if (slashPos >= 0)
      NFile::NDirectory::CreateComplexDirectory(_directoryPath + _filePath.Left(slashPos));
  }

  UString fullProcessedPath = _directoryPath + _filePath;
  _diskFilePath = fullProcessedPath;

  if (_processedFileInfo.isDir)
  {
    NFile::NDirectory::CreateComplexDirectory(fullProcessedPath);
  }
  else
  {
    NFile::NFind::CFileInfoW fi;
    if (fi.Find(fullProcessedPath))
    {
      if (!NFile::NDirectory::DeleteFileAlways(fullProcessedPath))
      {
        PrintString(UString(kCantDeleteOutputFile) + fullProcessedPath);
        return E_ABORT;
      }
    }
    
    _outFileStreamSpec = new COutFileStream;
    CMyComPtr<ISequentialOutStream> outStreamLoc(_outFileStreamSpec);
    if (!_outFileStreamSpec->Open(fullProcessedPath, CREATE_ALWAYS))
    {
      PrintString((UString)L"can not open output file " + fullProcessedPath);
      return E_ABORT;
    }
    _outFileStream = outStreamLoc;
    *outStream = outStreamLoc.Detach();
  }
  return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::PrepareOperation(Int32 askExtractMode)
{
  _extractMode = false;
  switch (askExtractMode)
  {
    case NArchive::NExtract::NAskMode::kExtract:  _extractMode = true; break;
  };
  switch (askExtractMode)
  {
    case NArchive::NExtract::NAskMode::kExtract:  PrintString(kExtractingString); break;
    case NArchive::NExtract::NAskMode::kTest:  PrintString(kTestingString); break;
    case NArchive::NExtract::NAskMode::kSkip:  PrintString(kSkippingString); break;
  };
  PrintString(_filePath);
  return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::SetOperationResult(Int32 operationResult)
{
  switch(operationResult)
  {
    case NArchive::NExtract::NOperationResult::kOK:
      break;
    default:
    {
      NumErrors++;
      PrintString("     ");
      switch(operationResult)
      {
        case NArchive::NExtract::NOperationResult::kUnSupportedMethod:
          PrintString(kUnsupportedMethod);
          break;
        case NArchive::NExtract::NOperationResult::kCRCError:
          PrintString(kCRCFailed);
          break;
        case NArchive::NExtract::NOperationResult::kDataError:
          PrintString(kDataError);
          break;
        default:
          PrintString(kUnknownError);
      }
    }
  }

	if (_outFileStreamSpec != NULL) {
  if (_outFileStream != NULL)
  {
    if (_processedFileInfo.MTimeDefined)
      _outFileStreamSpec->SetMTime(&_processedFileInfo.MTime);
    RINOK(_outFileStreamSpec->Close());
  }
  _outFileStream.Release();
  if (_extractMode && _processedFileInfo.AttribDefined)
    NFile::NDirectory::MySetFileAttributes(_diskFilePath, _processedFileInfo.Attrib);
	}
  PrintNewLine();
  return S_OK;
}


STDMETHODIMP CArchiveExtractCallback::CryptoGetTextPassword(BSTR *password)
{
  if (!PasswordIsDefined)
  {
    // You can ask real password here from user
    // Password = GetPassword(OutStream);
    // PasswordIsDefined = true;
    PrintError("Password is not defined");
    return E_ABORT;
  }
  return StringToBstr(Password, password);
}

struct CDllCodecInfo
{
  CLSID Encoder;
  CLSID Decoder;
  bool EncoderIsAssigned;
  bool DecoderIsAssigned;
  int LibIndex;
  UInt32 CodecIndex;
};

static HRESULT GetCoderClass(GetMethodPropertyFunc getMethodProperty, UInt32 index, PROPID propId, CLSID &clsId, bool &isAssigned)
{
  NCOM::CPropVariant prop;
  isAssigned = false;
  RINOK(getMethodProperty(index, propId, &prop));
  if (prop.vt == VT_BSTR)
  {
    isAssigned = true;
    clsId = *(const GUID *)prop.bstrVal;
  }
  else if (prop.vt != VT_EMPTY)
    return E_FAIL;
  return S_OK;
}

//static HRESULT LoadCodecs(NDLL::CLibrary const &lib, CObjectVector<CDllCodecInfo> &Codecs)
static HRESULT LoadCodecs(scopedSharedLibrary const &lib, CObjectVector<CDllCodecInfo> &Codecs)
{
  GetMethodPropertyFunc getMethodProperty = (GetMethodPropertyFunc)lib.getFunction("GetMethodProperty");
  if (getMethodProperty == NULL)
    return S_OK;

  UInt32 numMethods = 1;
  GetNumberOfMethodsFunc getNumberOfMethods = (GetNumberOfMethodsFunc)lib.getFunction("GetNumberOfMethods");
  if (getNumberOfMethods != NULL)
  {
    RINOK(getNumberOfMethods(&numMethods));
  }

  for(UInt32 i = 0; i < numMethods; i++)
  {
    CDllCodecInfo info;
    info.LibIndex = 0;
    info.CodecIndex = i;

    RINOK(GetCoderClass(getMethodProperty, i, NMethodPropID::kEncoder, info.Encoder, info.EncoderIsAssigned));
    RINOK(GetCoderClass(getMethodProperty, i, NMethodPropID::kDecoder, info.Decoder, info.DecoderIsAssigned));

    Codecs.Add(info);
  }
  return S_OK;
}

struct CArcExtInfo
{
  UString Ext;
  UString AddExt;
  CArcExtInfo() {}
  CArcExtInfo(const UString &ext): Ext(ext) {}
  CArcExtInfo(const UString &ext, const UString &addExt): Ext(ext), AddExt(addExt) {}
};

static void SplitString(const UString &srcString, UStringVector &destStrings)
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

struct CArcInfoEx
{
  int LibIndex;
  UInt32 FormatIndex;
  CLSID ClassID;
  bool UpdateEnabled;
  // ??? CreateInArchiveP CreateInArchive;
  // ??? CreateOutArchiveP CreateOutArchive;
  UString Name;
  CObjectVector<CArcExtInfo> Exts;
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
			CArcExtInfo extInfo;
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

  CArcInfoEx():
    LibIndex(-1),
    UpdateEnabled(false),
    // ??? CreateInArchive(0), CreateOutArchive(0),
    KeepName(false)
  {}
};

static HRESULT ReadProp(GetHandlerPropertyFunc getProp, GetHandlerPropertyFunc2 getProp2, UInt32 index, PROPID propID, NCOM::CPropVariant &prop)
{
  if (getProp2)
    return getProp2(index, propID, &prop);;
  return getProp(propID, &prop);
}

static HRESULT ReadBoolProp(GetHandlerPropertyFunc getProp, GetHandlerPropertyFunc2 getProp2, UInt32 index, PROPID propID, bool &res)
{
  NCOM::CPropVariant prop;
  RINOK(ReadProp(getProp, getProp2, index, propID, prop));
  if (prop.vt == VT_BOOL)
    res = VARIANT_BOOLToBool(prop.boolVal);
  else if (prop.vt != VT_EMPTY)
    return E_FAIL;
  return S_OK;
}

static HRESULT ReadStringProp(GetHandlerPropertyFunc getProp, GetHandlerPropertyFunc2 getProp2, UInt32 index, PROPID propID, UString &res)
{
  NCOM::CPropVariant prop;
  RINOK(ReadProp(getProp, getProp2, index, propID, prop));
  if (prop.vt == VT_BSTR)
    res = prop.bstrVal;
  else if (prop.vt != VT_EMPTY)
    return E_FAIL;
  return S_OK;
}

//static HRESULT LoadFormats(NDLL::CLibrary const &lib, CObjectVector<CArcInfoEx> &Formats)
static HRESULT LoadFormats(scopedSharedLibrary const &lib, CObjectVector<CArcInfoEx> &Formats)
{
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
    RINOK(getNumberOfFormats(&numFormats));
  }
  if (getProp2 == NULL)
    numFormats = 1;

  for (UInt32 i = 0; i < numFormats; i++)
  {
    CArcInfoEx item;
    item.LibIndex = 0;
    item.FormatIndex = i;

    RINOK(ReadStringProp(getProp, getProp2, i, NArchive::kName, item.Name));

    NCOM::CPropVariant prop;
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

static HRESULT CreateArchiveHandler(CreateObjectFunc createObjectFunc, const CArcInfoEx &ai, void **archive)
{
	return createObjectFunc(&ai.ClassID, &IID_IInArchive, archive);
}

int client7zip_test(wchar_t const *archiveFilePath) {
	printFile_ = fopen("codecs.txt", "w");

	/*for (size_t i = 0; i < (sizeof(usedArchives) / sizeof(usedArchives[0])); ++i) {
		OLECHAR s[39];
		if (!::StringFromGUID2(usedArchives[i], &s[0], (sizeof(s) / sizeof(s[0]))))
			printf("too small buffer: %d\n", (sizeof(s) / sizeof(s[0])));
		else
			printf("%s - %d\n", (const char*)GetOemString(s), wcslen(s));
	}*/

	/*NDLL::CLibrary lib;
  if (!lib.Load(TEXT(kDllName)))
  {
    PrintError("Can not load 7-zip library");
    return 1;
  }*/
	scopedSharedLibrary lib(TEXT(kDllName));
	if (!lib)
  {
    PrintError("Can not load 7-zip library");
    return 1;
  }

  CreateObjectFunc createObjectFunc = (CreateObjectFunc)lib.getFunction("CreateObject");
  if (createObjectFunc == 0)
  {
    PrintError("Can not get CreateObject");
    return 1;
  }

	CObjectVector<CDllCodecInfo> Codecs;
	if (LoadCodecs(lib, Codecs) != S_OK) {
		PrintError("Can not load Codecs");
    return 1;
	}
	CObjectVector<CArcInfoEx> Formats;
	if (LoadFormats(lib, Formats) != S_OK) {
		PrintError("Can not load Formats");
		return 1;
	}

	for (int i = 0; i < Formats.Size(); ++i) {
		OLECHAR s[39];
		::StringFromGUID2(Formats[i].ClassID, &s[0], (sizeof(s) / sizeof(s[0])));
		std::string s_(s, s + arraysize(s));

		fprintf(printFile_, "%s: %s\n %s\n %s\n", (const char*)GetOemString(Formats[i].Name), s_.c_str(),
		(const char*)GetOemString(Formats[i].GetAllExtensions()),
		(const char*)GetOemString(Formats[i].GetAllAdditionExtensions()));
	}

	/*int i = 0;
	for (; i < Formats.Size(); ++i) {
		printf("%s: %s\n", (LPCSTR)GetOemString(Formats[i].Name), (LPCSTR)GetOemString(Formats[i].GetAllExtensions()));
		if (!Formats[i].Name.CompareNoCase(L"rar"))
			break;
	}
	if (Formats.Size() == i) {
		PrintError("Can not get rar format handler");
		return 1;
	}*/

	CMyComPtr<IInArchive> archive;
	{
		UString name, dot, ext;
		NFile::NName::SplitNameToPureNameAndExtension(archiveFilePath, name, dot, ext);
		if (ext.Length() < 1) {
			printf("Can not get extension from path: %s\n", (LPCSTR)GetOemString(archiveFilePath));
			return 1;
		}
		int i = 0;
		for (; i < Formats.Size(); ++i) {
			if (Formats[i].FindExtension(ext) >= 0)
				break;
		}
		if (Formats.Size() == i) {
			printf("Can not get format for extension: %s\n", (LPCSTR)GetOemString(ext));
			return 1;
		}
		if (CreateArchiveHandler(createObjectFunc, Formats[i], (void **)&archive) != S_OK) {
			printf("Can not get class object for format: %s\n", (LPCSTR)GetOemString(Formats[i].Name));
			return 1;
		}
	}
	//if (CreateArchiveHandler(createObjectFunc, Formats[i], (void **)&archive) != S_OK)
	//// if (createObjectFunc(&CLSID_CFormat7z, &IID_IInArchive, (void **)&archive) != S_OK)
	//{
	//	PrintError("Can not get class object");
	//	return 1;
	//}

	CInFileStream *fileSpec = new CInFileStream;
	CMyComPtr<IInStream> file = fileSpec;

	if (!fileSpec->Open(archiveFilePath))
	{
		PrintError("Can not open archive file");
		return 1;
	}

	{
		CArchiveOpenCallback *openCallbackSpec = new CArchiveOpenCallback;
		CMyComPtr<IArchiveOpenCallback> openCallback(openCallbackSpec);
		
		openCallbackSpec->PasswordIsDefined = false;
		// openCallbackSpec->PasswordIsDefined = true;
		// openCallbackSpec->Password = L"1";
		
		if (archive->Open(file, 0, openCallback) != S_OK)
		{
			PrintError("Can not open archive");
			return 1;
		}
	}

	if (true) {
		// List command
		UInt32 numItems = 0;
		archive->GetNumberOfItems(&numItems);
		for (UInt32 i = 0; i < numItems; i++)
		{
			{
				// Get uncompressed size of file
				NCOM::CPropVariant prop;
				archive->GetProperty(i, kpidSize, &prop);
				UString s = ConvertPropVariantToString(prop);
				PrintString(s);
				PrintString("  ");
			}
			{
				// Get name of file
				NCOM::CPropVariant prop;
				archive->GetProperty(i, kpidPath, &prop);
				UString s = ConvertPropVariantToString(prop);
				PrintString(s);
			}
			PrintString("\n");
		}
	} else {
		// Extract command
		CArchiveExtractCallback *extractCallbackSpec = new CArchiveExtractCallback;
		CMyComPtr<IArchiveExtractCallback> extractCallback(extractCallbackSpec);
		
		extractCallbackSpec->Init(archive, L""); // second parameter is output folder path
		extractCallbackSpec->PasswordIsDefined = false;
		// extractCallbackSpec->PasswordIsDefined = true;
		// extractCallbackSpec->Password = L"1";
		
		HRESULT result = archive->Extract(NULL, (UInt32)(Int32)(-1), false, extractCallback);
		if (result != S_OK)
		{
			PrintError("Extract Error");
			return 1;
		}
	}

	fclose(printFile_);
	return 0;
}
