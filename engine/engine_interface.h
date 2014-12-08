// engine_interface.h
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

#ifndef __DLL_ENGINE_INTERFACE_H
#define __DLL_ENGINE_INTERFACE_H

#include <plugin_interface.h>

/* midl also may contain classes in lib block with CLSID FileList, FileItem - i.e. classes implements IPersistStream*/

CPCL_DECLARE_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF100")
IPropertyBasket : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE Clear() = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
      /* [retval][out] */ long *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetItemInfo( 
      /* [in] */ long index,
      /* [out] */ BSTR *item_name,
      /* [out] */ int *item_id,
      /* [out] */ VARTYPE *var_type) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetItem( 
      /* [in] */ int item_id,
      /* [retval][out] */ VARIANT *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetItem( 
      /* [in] */ int item_id,
      /* [in] */ VARIANT v) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetItemValue( 
      /* [in] */ int item_id,
			/* [out] */ byte *buffer,
      /* [in][out] */ int *buffer_size) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetItemValue( 
      /* [in] */ int item_id,
			/* [in] */ byte const *buffer,
      /* [in] */ int buffer_size) = 0;

  virtual HRESULT STDMETHODCALLTYPE DeleteItem( 
      /* [in] */ long index) = 0;

  virtual HRESULT STDMETHODCALLTYPE RemoveItem( 
      /* [in] */ int item_id) = 0;

  virtual HRESULT STDMETHODCALLTYPE FindItem( 
      /* [in] */ int item_id,
      /* [retval][out] */ long *index) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF2B1")
IFileItem : public IUnknown
{
public:
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Attributes( 
      /* [retval][out] */ IPropertyBasket **v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Path( 
      /* [retval][out] */ BSTR *v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
      /* [retval][out] */ BSTR *v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Stream( 
      /* [retval][out] */ IStream **v) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF2B2")
IFileList : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE Clear() = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
      /* [retval][out] */ long *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetItem( 
      /* [in] */ long index,
      /* [retval][out] */ IFileItem **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE AddItem( 
      /* [in] */ IFileItem *item,
      /* [out] */ long *index) = 0;

  virtual HRESULT STDMETHODCALLTYPE DeleteItem( 
      /* [in] */ long index) = 0;

  virtual HRESULT STDMETHODCALLTYPE RemoveItem( 
      /* [in] */ IFileItem *item) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetRange( 
      /* [in] */ long index,
      /* [in] */ long count,
      /* [retval][out] */ IFileList **v) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF2B3")
IFileFilter : public IUnknown
{
public:
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
      /* [retval][out] */ BSTR *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE Match( 
      /* [in] */ IFileItem *item) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_CombineMode( 
      /* [retval][out] */ DWORD *v) = 0;

  virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_CombineMode( 
      /* [in] */ DWORD v) = 0;
};

CPCL_DECLARE_INTERFACE("B502F1C0-DAC0-456E-A1B8-A6154169E4B9")
IPluginList : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE Clear() = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
      /* [retval][out] */ long *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetItem( 
      /* [in] */ long index,
      /* [retval][out] */ IPlugin **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE AddItem( 
      /* [in] */ IPlugin *item,
      /* [out] */ long *index) = 0;

  virtual HRESULT STDMETHODCALLTYPE DeleteItem( 
      /* [in] */ long index) = 0;

  virtual HRESULT STDMETHODCALLTYPE RemoveItem( 
      /* [in] */ IPlugin *item) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetRange( 
      /* [in] */ long index,
      /* [in] */ long count,
      /* [retval][out] */ IPluginList **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE LoadDocument( 
      /* [in] */ IStream *input,
      /* [retval][out] */ IPluginDocument **r) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FileExtensionFilter( 
      /* [retval][out] */ IFileFilter **v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FileSignatureFilter( 
      /* [retval][out] */ IFileFilter **v) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF2B4")
IFileFilterList : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE Clear() = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
      /* [retval][out] */ long *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetItem( 
      /* [in] */ long index,
      /* [retval][out] */ IFileFilter **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE AddItem( 
      /* [in] */ IFileFilter *item,
      /* [out] */ long *index) = 0;

  virtual HRESULT STDMETHODCALLTYPE DeleteItem( 
      /* [in] */ long index) = 0;

  virtual HRESULT STDMETHODCALLTYPE RemoveItem( 
      /* [in] */ IFileFilter *item) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetRange( 
      /* [in] */ long index,
      /* [in] */ long count,
      /* [retval][out] */ IFileFilterList **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE FindItem( 
      /* [in] */ BSTR filter_name,
      /* [retval][out] */ IFileFilter **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE Match( 
      /* [in] */ IFileItem *item) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF2CA")
IProgressEvent : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE ReportProgress( 
      /* [in] */ BSTR major_info,
      /* [in] */ BSTR minor_info,
      /* [in] */ long major_items_total,
			/* [in] */ long major_items_done,
			/* [in] */ long minor_items_total,
			/* [in] */ long minor_items_done,
      /* [in][out] */ VARIANT_BOOL *should_terminate) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF2A7")
IFileEnumeratorProgressEvents : public IProgressEvent
{
public:
  virtual HRESULT STDMETHODCALLTYPE ReportItemFound( 
      /* [in] */ BSTR uri,
      /* [in] */ IFileItem *item,
      /* [in] */ BSTR item_tip,
      /* [in][out] */ VARIANT_BOOL *should_terminate) = 0;

  virtual HRESULT STDMETHODCALLTYPE ReportEnumeratorTip( 
      /* [in] */ BSTR enumerator_tip,
      /* [in][out] */ VARIANT_BOOL *should_terminate) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-2980-4178-A629-9BE8B8DEF2C9")
IIconRetriever : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE FindIcon( 
      /* [in] */ int icon_index,
      /* [retval][out] */ int *h_icon) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF2B5")
IFileEnumerator : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE FindFromPath( 
      /* [in] */ BSTR root_path,
      /* [retval][out] */ IFileList **files) = 0;

  virtual HRESULT STDMETHODCALLTYPE FindFromItem( 
      /* [in] */ IFileItem *item,
      /* [retval][out] */ IFileList **files) = 0;

  virtual HRESULT STDMETHODCALLTYPE Path2File( 
      /* [in] */ BSTR file_path,
      /* [retval][out] */ IFileItem **item) = 0;

  virtual HRESULT STDMETHODCALLTYPE ItemParent( 
      /* [in] */ IFileItem *item,
      /* [retval][out] */ IFileItem **parent) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Filters( 
      /* [retval][out] */ IFileFilterList **v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Params( 
      /* [retval][out] */ IPropertyBasket **v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IconRetriever( 
      /* [retval][out] */ IIconRetriever **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetProgressEventSink( 
      /* [in] */ IFileEnumeratorProgressEvents *v) = 0;
};

#if 0
CPCL_DECLARE_INTERFACE("5325A8D0-1980-4178-A629-3DAEE1D910A0")
IFeature : public IUnknown
{
public:
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ID( 
      /* [retval][out] */ int *v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
      /* [retval][out] */ BSTR *v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RequiredFeaturesCount( 
      /* [retval][out] */ long *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE RequiredFeatureID( 
      /* [in] */ long index,
      /* [retval][out] */ int *v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Weight( 
      /* [retval][out] */ long *v) = 0;

  virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Weight( 
      /* [in] */ long v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Priority( 
      /* [retval][out] */ long *v) = 0;

  virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Priority( 
      /* [in] */ long v) = 0;

  virtual HRESULT STDMETHODCALLTYPE Calculate() = 0;

  virtual HRESULT STDMETHODCALLTYPE Distance( 
      /* [in] */ IFeature *feature,
      /* [retval][out] */ double *v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Data( 
      /* [in][out] */ long *buffer_size,
      /* [retval][out] */ byte *buffer) = 0;

  virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Data( 
      /* [in][out] */ long *buffer_size,
      /* [in] */ byte const *buffer) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-1980-4178-A629-3DAEE1D910A1")
IFeatureList : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE Clear() = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
      /* [retval][out] */ long *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetItem( 
      /* [in] */ long index,
      /* [retval][out] */ IFeature **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE AddItem( 
      /* [in] */ IFeature *item,
      /* [out] */ long *index) = 0;

  virtual HRESULT STDMETHODCALLTYPE DeleteItem( 
      /* [in] */ long index) = 0;

  virtual HRESULT STDMETHODCALLTYPE RemoveItem( 
      /* [in] */ IFeature *item) = 0;

  virtual HRESULT STDMETHODCALLTYPE FindItem( 
      /* [in] */ BSTR feature_name,
      /* [retval][out] */ IFeature **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetItemByID( 
      /* [in] */ int item_id,
      /* [retval][out] */ IFeature **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE Calculate() = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-1980-4178-A629-3DAEE1D910A2")
IFeaturesExtractor : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE Bind( 
      /* [in] */ IUnknown *v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Features( 
      /* [retval][out] */ IFeatureList **v) = 0;
};
#endif

#if 0
CPCL_DECLARE_INTERFACE("5325A8D0-1980-4178-A629-3DAEE1D910A0")
IFeature : public IUnknown
{
public:
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ID( 
      /* [retval][out] */ CLSID *v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RequiredFeaturesCount( 
      /* [retval][out] */ long *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE RequiredFeatureID( 
      /* [in] */ long index,
      /* [retval][out] */ CLSID *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetValue( 
			/* [out] */ byte *buffer,
      /* [in][out] */ int *buffer_size) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetValue( 
			/* [in] */ byte const *buffer,
      /* [in] */ int buffer_size) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-8097-4178-A629-3DAEE1D910A0")
IFaceFeature : public IFeature
{
public:
  virtual HRESULT STDMETHODCALLTYPE Render( 
      /* [in] */ IPluginPage *page,
      /* [in] */ byte r,
			/* [in] */ byte g,
			/* [in] */ byte b,
			/* [in] */ IRenderingDevice *output) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-8097-4178-A629-3DAEE1D910A1")
IFaceFeatureList : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE Clear() = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
      /* [retval][out] */ long *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetItem( 
      /* [in] */ long index,
      /* [retval][out] */ IFaceFeature **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE AddItem( 
      /* [in] */ IFaceFeature *item,
      /* [out] */ long *index) = 0;

  virtual HRESULT STDMETHODCALLTYPE DeleteItem( 
      /* [in] */ long index) = 0;

  virtual HRESULT STDMETHODCALLTYPE RemoveItem( 
      /* [in] */ IFaceFeature *item) = 0;

  virtual HRESULT STDMETHODCALLTYPE Render( 
      /* [in] */ IPluginPage *page,
      /* [in] */ byte r,
			/* [in] */ byte g,
			/* [in] */ byte b,
			/* [in] */ IRenderingDevice *output) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-8097-4178-A629-3DAEE1D910A2")
IFaceFeaturesExtractor : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE Find( 
      /* [in] */ IPluginPage *page,
			/* [retval][out] */ IFaceFeatureList **v) = 0;
};
// returns empty list if no faces found
#endif

CPCL_DECLARE_INTERFACE("5325A8D0-5463-4178-A629-3DAEE1D910A0")
IFeature : public IUnknown
{
public:
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ID( 
      /* [retval][out] */ CLSID *v) = 0;

	virtual HRESULT STDMETHODCALLTYPE GetROI( 
			/* [in][out] */ long *count,
      /* [out] */ DWORD *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE ClearAttributes() = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AttributeCount( 
      /* [retval][out] */ long *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetAttribute( 
      /* [in] */ long index,
			/* [out] */ BSTR *class_name,
      /* [out] */ CLSID *classifier_id) = 0;

  virtual HRESULT STDMETHODCALLTYPE FindAttribute( 
      /* [in] */ wchar_t const *class_name,
			/* [in] */ CLSID classifier_id,
      /* [out] */ long *index) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetAttribute( 
      /* [in] */ wchar_t const *class_name,
			/* [in] */ CLSID classifier_id,
      /* [out] */ long *index) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetValue( 
			/* [out] */ byte *buffer,
      /* [in][out] */ int *buffer_size) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetValue( 
			/* [in] */ byte const *buffer,
      /* [in] */ int buffer_size) = 0;

  virtual HRESULT STDMETHODCALLTYPE Render( 
      /* [in] */ IPluginPage *page,
      /* [in] */ byte r,
			/* [in] */ byte g,
			/* [in] */ byte b,
			/* [in] */ IRenderingDevice *output) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-5463-4178-A629-3DAEE1D910A1")
IFeatureList : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE Clear() = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
      /* [retval][out] */ long *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetItem( 
      /* [in] */ long index,
      /* [retval][out] */ IFeature **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE AddItem( 
      /* [in] */ IFeature *item,
      /* [out] */ long *index) = 0;

  virtual HRESULT STDMETHODCALLTYPE DeleteItem( 
      /* [in] */ long index) = 0;

  virtual HRESULT STDMETHODCALLTYPE RemoveItem( 
      /* [in] */ IFeature *item) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetRange( 
      /* [in] */ long index,
      /* [in] */ long count,
      /* [retval][out] */ IFeatureList **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE Render( 
      /* [in] */ IPluginPage *page,
      /* [in] */ byte r,
			/* [in] */ byte g,
			/* [in] */ byte b,
			/* [in] */ IRenderingDevice *output) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-5463-4178-A629-3DAEE1D910A4")
IFeatureAttribute : public IUnknown
{
public:
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ID( 
      /* [retval][out] */ CLSID *v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
      /* [retval][out] */ BSTR *v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ClassCount( 
      /* [retval][out] */ long *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetClass( 
      /* [in] */ long index,
			/* [out] */ BSTR *class_name) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetValue( 
			/* [out] */ byte *buffer,
      /* [in][out] */ int *buffer_size) = 0;

  virtual HRESULT STDMETHODCALLTYPE SetValue( 
			/* [in] */ byte const *buffer,
      /* [in] */ int buffer_size) = 0;

  virtual HRESULT STDMETHODCALLTYPE Classify( 
      /* [in] */ IPluginPage *page,
			/* [in][out] */ IFeature *feature,
			/* [retval][out] */ long *index) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-5463-4178-A629-3DAEE1D910A3")
IFeatureAttributeList : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE Clear() = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
      /* [retval][out] */ long *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetItem( 
      /* [in] */ long index,
      /* [retval][out] */ IFeatureAttribute **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE AddItem( 
      /* [in] */ IFeatureAttribute *item,
      /* [out] */ long *index) = 0;

  virtual HRESULT STDMETHODCALLTYPE DeleteItem( 
      /* [in] */ long index) = 0;

  virtual HRESULT STDMETHODCALLTYPE RemoveItem( 
      /* [in] */ IFeatureAttribute *item) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetRange( 
      /* [in] */ long index,
      /* [in] */ long count,
      /* [retval][out] */ IFeatureAttributeList **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE Classify( 
      /* [in] */ IPluginPage *page,
			/* [in][out] */ IFeature *feature) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-5463-4178-A629-3DAEE1D910A2")
IFeatureDetector : public IUnknown
{
public:
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ID( 
      /* [retval][out] */ CLSID *v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
      /* [retval][out] */ BSTR *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE LoadClassifiers( 
			/* [retval][out] */ IFeatureAttributeList **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE Find( 
      /* [in] */ IPluginPage *page,
			/* [retval][out] */ IFeatureList **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE Classify( 
      /* [in] */ IPluginPage *page,
			/* [in] */ IFeatureAttributeList *classifiers,
			/* [retval][out] */ IFeatureList **v) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-5463-4178-A629-3DAEE1D910A5")
IFeatureDetectorList : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE Clear() = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
      /* [retval][out] */ long *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetItem( 
      /* [in] */ long index,
      /* [retval][out] */ IFeatureDetector **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE AddItem( 
      /* [in] */ IFeatureDetector *item,
      /* [out] */ long *index) = 0;

  virtual HRESULT STDMETHODCALLTYPE DeleteItem( 
      /* [in] */ long index) = 0;

  virtual HRESULT STDMETHODCALLTYPE RemoveItem( 
      /* [in] */ IFeatureDetector *item) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetRange( 
      /* [in] */ long index,
      /* [in] */ long count,
      /* [retval][out] */ IFeatureDetectorList **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE FindItem( 
      /* [in] */ BSTR detector_name,
			/* [retval][out] */ IFeatureDetector **v) = 0;
};

CPCL_DECLARE_INTERFACE("39E25190-BD06-4A2E-B3E7-3FA7BAF384A0")
ICustomClassFactory : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE CreateInstance( 
      /* [in] */ REFCLSID class_id,
      /* [retval][out] */ IPersistStream **v) = 0;
};

CPCL_DECLARE_INTERFACE("EB2A1178-8097-4CDE-A704-66A1FE824FDF")
IEngine : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE LoadDetectors( 
      /* [retval][out] */ IFeatureDetectorList **detectors) = 0;
#if 0
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FaceFeaturesExtractor( 
      /* [retval][out] */ IFaceFeaturesExtractor **v) = 0;
#endif
#if 0
	virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FeaturesExtractor( 
      /* [retval][out] */ IFeaturesExtractor **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE GetFaceFeaturePattern( 
      /* [in] */ int pattern_type,
      /* [retval][out] */ IFeature **v) = 0;
#endif
  virtual HRESULT STDMETHODCALLTYPE LoadPlugins( 
      /* [retval][out] */ IPluginList **plugins) = 0;

  virtual HRESULT STDMETHODCALLTYPE LoadPluginsFromPath( 
      /* [in] */ BSTR file_path,
      /* [retval][out] */ IPluginList **plugins) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FileEnumerator( 
      /* [retval][out] */ IFileEnumerator **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE Serialize( 
      /* [in] */ BSTR file_path,
      /* [in] */ IUnknown *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE Deserialize( 
      /* [in] */ BSTR file_path,
      /* [retval][out] */ IUnknown **v) = 0;

  virtual HRESULT STDMETHODCALLTYPE RegisterClassFactory( 
      /* [in] */ ICustomClassFactory *v) = 0;

  virtual HRESULT STDMETHODCALLTYPE CreateClassInstance( 
      /* [in] */ REFCLSID class_id,
      /* [retval][out] */ IPersistStream **v) = 0;
};
#if 0
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_EnableLogging( 
      /* [retval][out] */ VARIANT_BOOL *v) = 0;

  virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_EnableLogging( 
      /* [in] */ VARIANT_BOOL v) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_LoggingStream( 
      /* [retval][out] */ IStream **v) = 0;

  virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_LoggingStream( 
      /* [in] */ IStream *v) = 0;
};
#endif

/*PARSER_INFO_BLOCK("library.class.guid - python code for generate ast with midl library class")*/

#endif // __DLL_ENGINE_INTERFACE_H
