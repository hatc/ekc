// plugin_interface.h
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
#pragma once

#ifndef __PLCL_PLUGIN_INTERFACE_H
#define __PLCL_PLUGIN_INTERFACE_H

#include <Objidl.h>

#include <internal_com.h>

//typedef /* [public] */ 
//enum tagCPCL_PLUGIN_PIXEL_FORMAT_290c_61e2_b5e8
//    {	
//	CPCL_PLUGIN_PIXEL_FORMAT_8_INDEXED = 0x1,
//	CPCL_PLUGIN_PIXEL_FORMAT_16_GRAYSCALE = 0x2,
//	CPCL_PLUGIN_PIXEL_FORMAT_24_RGB = 0x4,
//	CPCL_PLUGIN_PIXEL_FORMAT_24_BGR = 0x8,
//	CPCL_PLUGIN_PIXEL_FORMAT_32_RGBA = 0x10,
//	CPCL_PLUGIN_PIXEL_FORMAT_32_ARGB = 0x20,
//	CPCL_PLUGIN_PIXEL_FORMAT_32_BGRA = 0x40
//    }		CPCL_PLUGIN_PIXEL_FORMAT;

CPCL_DECLARE_INTERFACE("5325A8D0-1970-4178-A630-6B0001DEF1FF")
IRenderingData : public IUnknown
{
public:
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Width( 
      /* [retval][out] */ DWORD *v) = 0;
  
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Height( 
      /* [retval][out] */ DWORD *v) = 0;
  
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Stride( 
      /* [retval][out] */ DWORD *v) = 0;
  
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PixelFormat( 
      /* [retval][out] */ DWORD *v) = 0;
  
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_BitsPerPixel( 
      /* [retval][out] */ DWORD *v) = 0;
  
  virtual HRESULT STDMETHODCALLTYPE Scanline( 
      /* [in] */ DWORD y,
      /* [in][out] */ int *scanline_ptr) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-1970-4178-A630-6B0001DEF200")
IRenderingDevice : public IUnknown
{
public:
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_SupportedPixfmt( 
      /* [retval][out] */ DWORD *v) = 0;
  
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PixelFormat( 
      /* [retval][out] */ DWORD *v) = 0;
  
  virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_PixelFormat( 
      /* [in] */ DWORD v) = 0;
  
  virtual HRESULT STDMETHODCALLTYPE SetViewport( 
      /* [in] */ DWORD x1,
      /* [in] */ DWORD y1,
      /* [in] */ DWORD x2,
      /* [in] */ DWORD y2) = 0;
  
  virtual HRESULT STDMETHODCALLTYPE SweepScanline( 
      /* [in] */ DWORD y,
      /* [in][out] */ int *scanline_ptr) = 0;
  
  virtual HRESULT STDMETHODCALLTYPE Render() = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-1970-4178-A630-6B0001DEF201")
IPluginPage : public IUnknown
{
public:
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Width( 
      /* [retval][out] */ DWORD *v) = 0;
  
  virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Width( 
      /* [in] */ DWORD v) = 0;
  
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Height( 
      /* [retval][out] */ DWORD *v) = 0;
  
  virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Height( 
      /* [in] */ DWORD v) = 0;
  
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Zoom( 
      /* [retval][out] */ DWORD *v) = 0;
  
  virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Zoom( 
      /* [in] */ DWORD v) = 0;
  
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Dpi( 
      /* [retval][out] */ double *v) = 0;
  
  virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Dpi( 
      /* [in] */ double v) = 0;
  
  virtual HRESULT STDMETHODCALLTYPE Render( 
      /* [in] */ IRenderingDevice *output) = 0;

  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Orientation( 
      /* [retval][out] */ DWORD *v) = 0;
  
  virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Orientation( 
      /* [in] */ DWORD v) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-1970-4178-B630-6B0001DEF201")
ITextPage : public IPluginPage
{
public:
  virtual HRESULT STDMETHODCALLTYPE Text( 
      /* [out] */ wchar_t const **r,
      /* [out] */ DWORD *r_size) = 0;
  
  virtual HRESULT STDMETHODCALLTYPE CopyText( 
      /* [in] */ wchar_t *v,
      /* [in] */ DWORD v_size) = 0;
  
  virtual HRESULT STDMETHODCALLTYPE Select( 
      /* [in] */ DWORD start,
      /* [in] */ DWORD size) = 0;
  
  virtual HRESULT STDMETHODCALLTYPE RenderSelection( 
      /* [in] */ IRenderingDevice *output) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-1970-4178-A630-6B0001DEF202")
IPluginDocument : public IUnknown
{
public:
  virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PageCount( 
      /* [retval][out] */ DWORD *v) = 0;
  
  virtual HRESULT STDMETHODCALLTYPE GetPage( 
      /* [in] */ DWORD zero_based_page_number,
      /* [retval][out] */ IPluginPage **r) = 0;
};

CPCL_DECLARE_INTERFACE("5325A8D0-1970-3C5D-B05F-F61D7799D9FF")
IPlugin : public IUnknown
{
public:
  virtual HRESULT STDMETHODCALLTYPE DocFileExtension( 
      /* [out] */ wchar_t const **r,
      /* [out] */ DWORD *r_size) = 0;
  
  virtual HRESULT STDMETHODCALLTYPE CopyDocFileExtension( 
      /* [in] */ wchar_t *v,
      /* [in] */ DWORD v_size) = 0;
  
  virtual HRESULT STDMETHODCALLTYPE DocFileFormat( 
      /* [out] */ wchar_t const **r,
      /* [out] */ DWORD *r_size) = 0;
  
  virtual HRESULT STDMETHODCALLTYPE CopyDocFileFormat( 
      /* [in] */ wchar_t *v,
      /* [in] */ DWORD v_size) = 0;
  
  virtual HRESULT STDMETHODCALLTYPE CheckHeader( 
      /* [in] */ IStream *stream) = 0;
  
  virtual HRESULT STDMETHODCALLTYPE GetSignatures( 
      /* [out] */ int *signatures_ptr,
      /* [out] */ DWORD *n_signatures) = 0;
  
  virtual HRESULT STDMETHODCALLTYPE LoadDocument( 
      /* [in] */ IStream *input,
      /* [retval][out] */ IPluginDocument **r) = 0;
};

// IPlugin::LoadDocument returns S_FALSE if stream does not contain a supported document type
// IPlugin::CheckHeader returns S_FALSE if signature not match
// IPlugin::GetSignatures returns E_FAIL if signature informaton not set

#endif // __PLCL_PLUGIN_INTERFACE_H
