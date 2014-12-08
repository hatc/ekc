

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Fri Dec 20 22:41:19 2013
 */
/* Compiler settings for interface.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __interface_h__
#define __interface_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IRenderingData_FWD_DEFINED__
#define __IRenderingData_FWD_DEFINED__
typedef interface IRenderingData IRenderingData;
#endif 	/* __IRenderingData_FWD_DEFINED__ */


#ifndef __IRenderingDevice_FWD_DEFINED__
#define __IRenderingDevice_FWD_DEFINED__
typedef interface IRenderingDevice IRenderingDevice;
#endif 	/* __IRenderingDevice_FWD_DEFINED__ */


#ifndef __IPluginPage_FWD_DEFINED__
#define __IPluginPage_FWD_DEFINED__
typedef interface IPluginPage IPluginPage;
#endif 	/* __IPluginPage_FWD_DEFINED__ */


#ifndef __ITextPage_FWD_DEFINED__
#define __ITextPage_FWD_DEFINED__
typedef interface ITextPage ITextPage;
#endif 	/* __ITextPage_FWD_DEFINED__ */


#ifndef __IPluginDocument_FWD_DEFINED__
#define __IPluginDocument_FWD_DEFINED__
typedef interface IPluginDocument IPluginDocument;
#endif 	/* __IPluginDocument_FWD_DEFINED__ */


#ifndef __IPlugin_FWD_DEFINED__
#define __IPlugin_FWD_DEFINED__
typedef interface IPlugin IPlugin;
#endif 	/* __IPlugin_FWD_DEFINED__ */


#ifndef __IPropertyBasket_FWD_DEFINED__
#define __IPropertyBasket_FWD_DEFINED__
typedef interface IPropertyBasket IPropertyBasket;
#endif 	/* __IPropertyBasket_FWD_DEFINED__ */


#ifndef __IFileItem_FWD_DEFINED__
#define __IFileItem_FWD_DEFINED__
typedef interface IFileItem IFileItem;
#endif 	/* __IFileItem_FWD_DEFINED__ */


#ifndef __IFileList_FWD_DEFINED__
#define __IFileList_FWD_DEFINED__
typedef interface IFileList IFileList;
#endif 	/* __IFileList_FWD_DEFINED__ */


#ifndef __IFileFilter_FWD_DEFINED__
#define __IFileFilter_FWD_DEFINED__
typedef interface IFileFilter IFileFilter;
#endif 	/* __IFileFilter_FWD_DEFINED__ */


#ifndef __IPluginList_FWD_DEFINED__
#define __IPluginList_FWD_DEFINED__
typedef interface IPluginList IPluginList;
#endif 	/* __IPluginList_FWD_DEFINED__ */


#ifndef __IFileFilterList_FWD_DEFINED__
#define __IFileFilterList_FWD_DEFINED__
typedef interface IFileFilterList IFileFilterList;
#endif 	/* __IFileFilterList_FWD_DEFINED__ */


#ifndef __IProgressEvent_FWD_DEFINED__
#define __IProgressEvent_FWD_DEFINED__
typedef interface IProgressEvent IProgressEvent;
#endif 	/* __IProgressEvent_FWD_DEFINED__ */


#ifndef __IFileEnumeratorProgressEvents_FWD_DEFINED__
#define __IFileEnumeratorProgressEvents_FWD_DEFINED__
typedef interface IFileEnumeratorProgressEvents IFileEnumeratorProgressEvents;
#endif 	/* __IFileEnumeratorProgressEvents_FWD_DEFINED__ */


#ifndef __IIconRetriever_FWD_DEFINED__
#define __IIconRetriever_FWD_DEFINED__
typedef interface IIconRetriever IIconRetriever;
#endif 	/* __IIconRetriever_FWD_DEFINED__ */


#ifndef __IFileEnumerator_FWD_DEFINED__
#define __IFileEnumerator_FWD_DEFINED__
typedef interface IFileEnumerator IFileEnumerator;
#endif 	/* __IFileEnumerator_FWD_DEFINED__ */


#ifndef __IFeature_FWD_DEFINED__
#define __IFeature_FWD_DEFINED__
typedef interface IFeature IFeature;
#endif 	/* __IFeature_FWD_DEFINED__ */


#ifndef __IFeatureList_FWD_DEFINED__
#define __IFeatureList_FWD_DEFINED__
typedef interface IFeatureList IFeatureList;
#endif 	/* __IFeatureList_FWD_DEFINED__ */


#ifndef __IFeatureAttribute_FWD_DEFINED__
#define __IFeatureAttribute_FWD_DEFINED__
typedef interface IFeatureAttribute IFeatureAttribute;
#endif 	/* __IFeatureAttribute_FWD_DEFINED__ */


#ifndef __IFeatureAttributeList_FWD_DEFINED__
#define __IFeatureAttributeList_FWD_DEFINED__
typedef interface IFeatureAttributeList IFeatureAttributeList;
#endif 	/* __IFeatureAttributeList_FWD_DEFINED__ */


#ifndef __IFeatureDetector_FWD_DEFINED__
#define __IFeatureDetector_FWD_DEFINED__
typedef interface IFeatureDetector IFeatureDetector;
#endif 	/* __IFeatureDetector_FWD_DEFINED__ */


#ifndef __IFeatureDetectorList_FWD_DEFINED__
#define __IFeatureDetectorList_FWD_DEFINED__
typedef interface IFeatureDetectorList IFeatureDetectorList;
#endif 	/* __IFeatureDetectorList_FWD_DEFINED__ */


#ifndef __ICustomClassFactory_FWD_DEFINED__
#define __ICustomClassFactory_FWD_DEFINED__
typedef interface ICustomClassFactory ICustomClassFactory;
#endif 	/* __ICustomClassFactory_FWD_DEFINED__ */


#ifndef __IEngine_FWD_DEFINED__
#define __IEngine_FWD_DEFINED__
typedef interface IEngine IEngine;
#endif 	/* __IEngine_FWD_DEFINED__ */


#ifndef __IRenderingData_FWD_DEFINED__
#define __IRenderingData_FWD_DEFINED__
typedef interface IRenderingData IRenderingData;
#endif 	/* __IRenderingData_FWD_DEFINED__ */


#ifndef __IRenderingDevice_FWD_DEFINED__
#define __IRenderingDevice_FWD_DEFINED__
typedef interface IRenderingDevice IRenderingDevice;
#endif 	/* __IRenderingDevice_FWD_DEFINED__ */


#ifndef __IPluginPage_FWD_DEFINED__
#define __IPluginPage_FWD_DEFINED__
typedef interface IPluginPage IPluginPage;
#endif 	/* __IPluginPage_FWD_DEFINED__ */


#ifndef __ITextPage_FWD_DEFINED__
#define __ITextPage_FWD_DEFINED__
typedef interface ITextPage ITextPage;
#endif 	/* __ITextPage_FWD_DEFINED__ */


#ifndef __IPluginDocument_FWD_DEFINED__
#define __IPluginDocument_FWD_DEFINED__
typedef interface IPluginDocument IPluginDocument;
#endif 	/* __IPluginDocument_FWD_DEFINED__ */


#ifndef __IPlugin_FWD_DEFINED__
#define __IPlugin_FWD_DEFINED__
typedef interface IPlugin IPlugin;
#endif 	/* __IPlugin_FWD_DEFINED__ */


#ifndef __IPropertyBasket_FWD_DEFINED__
#define __IPropertyBasket_FWD_DEFINED__
typedef interface IPropertyBasket IPropertyBasket;
#endif 	/* __IPropertyBasket_FWD_DEFINED__ */


#ifndef __IFileItem_FWD_DEFINED__
#define __IFileItem_FWD_DEFINED__
typedef interface IFileItem IFileItem;
#endif 	/* __IFileItem_FWD_DEFINED__ */


#ifndef __IFileList_FWD_DEFINED__
#define __IFileList_FWD_DEFINED__
typedef interface IFileList IFileList;
#endif 	/* __IFileList_FWD_DEFINED__ */


#ifndef __IFileFilter_FWD_DEFINED__
#define __IFileFilter_FWD_DEFINED__
typedef interface IFileFilter IFileFilter;
#endif 	/* __IFileFilter_FWD_DEFINED__ */


#ifndef __IPluginList_FWD_DEFINED__
#define __IPluginList_FWD_DEFINED__
typedef interface IPluginList IPluginList;
#endif 	/* __IPluginList_FWD_DEFINED__ */


#ifndef __IFileFilterList_FWD_DEFINED__
#define __IFileFilterList_FWD_DEFINED__
typedef interface IFileFilterList IFileFilterList;
#endif 	/* __IFileFilterList_FWD_DEFINED__ */


#ifndef __IProgressEvent_FWD_DEFINED__
#define __IProgressEvent_FWD_DEFINED__
typedef interface IProgressEvent IProgressEvent;
#endif 	/* __IProgressEvent_FWD_DEFINED__ */


#ifndef __IFileEnumeratorProgressEvents_FWD_DEFINED__
#define __IFileEnumeratorProgressEvents_FWD_DEFINED__
typedef interface IFileEnumeratorProgressEvents IFileEnumeratorProgressEvents;
#endif 	/* __IFileEnumeratorProgressEvents_FWD_DEFINED__ */


#ifndef __IIconRetriever_FWD_DEFINED__
#define __IIconRetriever_FWD_DEFINED__
typedef interface IIconRetriever IIconRetriever;
#endif 	/* __IIconRetriever_FWD_DEFINED__ */


#ifndef __IFileEnumerator_FWD_DEFINED__
#define __IFileEnumerator_FWD_DEFINED__
typedef interface IFileEnumerator IFileEnumerator;
#endif 	/* __IFileEnumerator_FWD_DEFINED__ */


#ifndef __IFeature_FWD_DEFINED__
#define __IFeature_FWD_DEFINED__
typedef interface IFeature IFeature;
#endif 	/* __IFeature_FWD_DEFINED__ */


#ifndef __IFeatureList_FWD_DEFINED__
#define __IFeatureList_FWD_DEFINED__
typedef interface IFeatureList IFeatureList;
#endif 	/* __IFeatureList_FWD_DEFINED__ */


#ifndef __IFeatureAttribute_FWD_DEFINED__
#define __IFeatureAttribute_FWD_DEFINED__
typedef interface IFeatureAttribute IFeatureAttribute;
#endif 	/* __IFeatureAttribute_FWD_DEFINED__ */


#ifndef __IFeatureAttributeList_FWD_DEFINED__
#define __IFeatureAttributeList_FWD_DEFINED__
typedef interface IFeatureAttributeList IFeatureAttributeList;
#endif 	/* __IFeatureAttributeList_FWD_DEFINED__ */


#ifndef __IFeatureDetector_FWD_DEFINED__
#define __IFeatureDetector_FWD_DEFINED__
typedef interface IFeatureDetector IFeatureDetector;
#endif 	/* __IFeatureDetector_FWD_DEFINED__ */


#ifndef __IFeatureDetectorList_FWD_DEFINED__
#define __IFeatureDetectorList_FWD_DEFINED__
typedef interface IFeatureDetectorList IFeatureDetectorList;
#endif 	/* __IFeatureDetectorList_FWD_DEFINED__ */


#ifndef __ICustomClassFactory_FWD_DEFINED__
#define __ICustomClassFactory_FWD_DEFINED__
typedef interface ICustomClassFactory ICustomClassFactory;
#endif 	/* __ICustomClassFactory_FWD_DEFINED__ */


#ifndef __IEngine_FWD_DEFINED__
#define __IEngine_FWD_DEFINED__
typedef interface IEngine IEngine;
#endif 	/* __IEngine_FWD_DEFINED__ */


#ifndef __CFileList_FWD_DEFINED__
#define __CFileList_FWD_DEFINED__

#ifdef __cplusplus
typedef class CFileList CFileList;
#else
typedef struct CFileList CFileList;
#endif /* __cplusplus */

#endif 	/* __CFileList_FWD_DEFINED__ */


#ifndef __CFeatureList_FWD_DEFINED__
#define __CFeatureList_FWD_DEFINED__

#ifdef __cplusplus
typedef class CFeatureList CFeatureList;
#else
typedef struct CFeatureList CFeatureList;
#endif /* __cplusplus */

#endif 	/* __CFeatureList_FWD_DEFINED__ */


/* header files for imported files */
#include "Objidl.h"
#include "OaIdl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IRenderingData_INTERFACE_DEFINED__
#define __IRenderingData_INTERFACE_DEFINED__

/* interface IRenderingData */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IRenderingData;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-1970-4178-A630-6B0001DEF1FF")
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
            /* [out] */ int *scanline_ptr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRenderingDataVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRenderingData * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRenderingData * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRenderingData * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Width )( 
            IRenderingData * This,
            /* [retval][out] */ DWORD *v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Height )( 
            IRenderingData * This,
            /* [retval][out] */ DWORD *v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Stride )( 
            IRenderingData * This,
            /* [retval][out] */ DWORD *v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_PixelFormat )( 
            IRenderingData * This,
            /* [retval][out] */ DWORD *v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_BitsPerPixel )( 
            IRenderingData * This,
            /* [retval][out] */ DWORD *v);
        
        HRESULT ( STDMETHODCALLTYPE *Scanline )( 
            IRenderingData * This,
            /* [in] */ DWORD y,
            /* [out] */ int *scanline_ptr);
        
        END_INTERFACE
    } IRenderingDataVtbl;

    interface IRenderingData
    {
        CONST_VTBL struct IRenderingDataVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRenderingData_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRenderingData_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRenderingData_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRenderingData_get_Width(This,v)	\
    ( (This)->lpVtbl -> get_Width(This,v) ) 

#define IRenderingData_get_Height(This,v)	\
    ( (This)->lpVtbl -> get_Height(This,v) ) 

#define IRenderingData_get_Stride(This,v)	\
    ( (This)->lpVtbl -> get_Stride(This,v) ) 

#define IRenderingData_get_PixelFormat(This,v)	\
    ( (This)->lpVtbl -> get_PixelFormat(This,v) ) 

#define IRenderingData_get_BitsPerPixel(This,v)	\
    ( (This)->lpVtbl -> get_BitsPerPixel(This,v) ) 

#define IRenderingData_Scanline(This,y,scanline_ptr)	\
    ( (This)->lpVtbl -> Scanline(This,y,scanline_ptr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRenderingData_INTERFACE_DEFINED__ */


#ifndef __IRenderingDevice_INTERFACE_DEFINED__
#define __IRenderingDevice_INTERFACE_DEFINED__

/* interface IRenderingDevice */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IRenderingDevice;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-1970-4178-A630-6B0001DEF200")
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
            /* [out] */ int *scanline_ptr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Render( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRenderingDeviceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRenderingDevice * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRenderingDevice * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRenderingDevice * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_SupportedPixfmt )( 
            IRenderingDevice * This,
            /* [retval][out] */ DWORD *v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_PixelFormat )( 
            IRenderingDevice * This,
            /* [retval][out] */ DWORD *v);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_PixelFormat )( 
            IRenderingDevice * This,
            /* [in] */ DWORD v);
        
        HRESULT ( STDMETHODCALLTYPE *SetViewport )( 
            IRenderingDevice * This,
            /* [in] */ DWORD x1,
            /* [in] */ DWORD y1,
            /* [in] */ DWORD x2,
            /* [in] */ DWORD y2);
        
        HRESULT ( STDMETHODCALLTYPE *SweepScanline )( 
            IRenderingDevice * This,
            /* [in] */ DWORD y,
            /* [out] */ int *scanline_ptr);
        
        HRESULT ( STDMETHODCALLTYPE *Render )( 
            IRenderingDevice * This);
        
        END_INTERFACE
    } IRenderingDeviceVtbl;

    interface IRenderingDevice
    {
        CONST_VTBL struct IRenderingDeviceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRenderingDevice_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRenderingDevice_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRenderingDevice_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRenderingDevice_get_SupportedPixfmt(This,v)	\
    ( (This)->lpVtbl -> get_SupportedPixfmt(This,v) ) 

#define IRenderingDevice_get_PixelFormat(This,v)	\
    ( (This)->lpVtbl -> get_PixelFormat(This,v) ) 

#define IRenderingDevice_put_PixelFormat(This,v)	\
    ( (This)->lpVtbl -> put_PixelFormat(This,v) ) 

#define IRenderingDevice_SetViewport(This,x1,y1,x2,y2)	\
    ( (This)->lpVtbl -> SetViewport(This,x1,y1,x2,y2) ) 

#define IRenderingDevice_SweepScanline(This,y,scanline_ptr)	\
    ( (This)->lpVtbl -> SweepScanline(This,y,scanline_ptr) ) 

#define IRenderingDevice_Render(This)	\
    ( (This)->lpVtbl -> Render(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRenderingDevice_INTERFACE_DEFINED__ */


#ifndef __IPluginPage_INTERFACE_DEFINED__
#define __IPluginPage_INTERFACE_DEFINED__

/* interface IPluginPage */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IPluginPage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-1970-4178-A630-6B0001DEF201")
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
    
#else 	/* C style interface */

    typedef struct IPluginPageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPluginPage * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPluginPage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPluginPage * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Width )( 
            IPluginPage * This,
            /* [retval][out] */ DWORD *v);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Width )( 
            IPluginPage * This,
            /* [in] */ DWORD v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Height )( 
            IPluginPage * This,
            /* [retval][out] */ DWORD *v);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Height )( 
            IPluginPage * This,
            /* [in] */ DWORD v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Zoom )( 
            IPluginPage * This,
            /* [retval][out] */ DWORD *v);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Zoom )( 
            IPluginPage * This,
            /* [in] */ DWORD v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Dpi )( 
            IPluginPage * This,
            /* [retval][out] */ double *v);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Dpi )( 
            IPluginPage * This,
            /* [in] */ double v);
        
        HRESULT ( STDMETHODCALLTYPE *Render )( 
            IPluginPage * This,
            /* [in] */ IRenderingDevice *output);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Orientation )( 
            IPluginPage * This,
            /* [retval][out] */ DWORD *v);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Orientation )( 
            IPluginPage * This,
            /* [in] */ DWORD v);
        
        END_INTERFACE
    } IPluginPageVtbl;

    interface IPluginPage
    {
        CONST_VTBL struct IPluginPageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPluginPage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPluginPage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPluginPage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPluginPage_get_Width(This,v)	\
    ( (This)->lpVtbl -> get_Width(This,v) ) 

#define IPluginPage_put_Width(This,v)	\
    ( (This)->lpVtbl -> put_Width(This,v) ) 

#define IPluginPage_get_Height(This,v)	\
    ( (This)->lpVtbl -> get_Height(This,v) ) 

#define IPluginPage_put_Height(This,v)	\
    ( (This)->lpVtbl -> put_Height(This,v) ) 

#define IPluginPage_get_Zoom(This,v)	\
    ( (This)->lpVtbl -> get_Zoom(This,v) ) 

#define IPluginPage_put_Zoom(This,v)	\
    ( (This)->lpVtbl -> put_Zoom(This,v) ) 

#define IPluginPage_get_Dpi(This,v)	\
    ( (This)->lpVtbl -> get_Dpi(This,v) ) 

#define IPluginPage_put_Dpi(This,v)	\
    ( (This)->lpVtbl -> put_Dpi(This,v) ) 

#define IPluginPage_Render(This,output)	\
    ( (This)->lpVtbl -> Render(This,output) ) 

#define IPluginPage_get_Orientation(This,v)	\
    ( (This)->lpVtbl -> get_Orientation(This,v) ) 

#define IPluginPage_put_Orientation(This,v)	\
    ( (This)->lpVtbl -> put_Orientation(This,v) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPluginPage_INTERFACE_DEFINED__ */


#ifndef __ITextPage_INTERFACE_DEFINED__
#define __ITextPage_INTERFACE_DEFINED__

/* interface ITextPage */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_ITextPage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-1970-4178-B630-6B0001DEF201")
    ITextPage : public IPluginPage
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Text( 
            /* [out][in] */ const wchar_t **r,
            /* [out][in] */ DWORD *r_size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CopyText( 
            /* [in] */ wchar_t *v,
            /* [in] */ DWORD v_size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Select( 
            /* [in] */ DWORD start,
            /* [in] */ DWORD size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RenderSelection( 
            /* [in] */ IRenderingDevice *output) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITextPageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITextPage * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITextPage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITextPage * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Width )( 
            ITextPage * This,
            /* [retval][out] */ DWORD *v);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Width )( 
            ITextPage * This,
            /* [in] */ DWORD v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Height )( 
            ITextPage * This,
            /* [retval][out] */ DWORD *v);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Height )( 
            ITextPage * This,
            /* [in] */ DWORD v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Zoom )( 
            ITextPage * This,
            /* [retval][out] */ DWORD *v);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Zoom )( 
            ITextPage * This,
            /* [in] */ DWORD v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Dpi )( 
            ITextPage * This,
            /* [retval][out] */ double *v);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Dpi )( 
            ITextPage * This,
            /* [in] */ double v);
        
        HRESULT ( STDMETHODCALLTYPE *Render )( 
            ITextPage * This,
            /* [in] */ IRenderingDevice *output);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Orientation )( 
            ITextPage * This,
            /* [retval][out] */ DWORD *v);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Orientation )( 
            ITextPage * This,
            /* [in] */ DWORD v);
        
        HRESULT ( STDMETHODCALLTYPE *Text )( 
            ITextPage * This,
            /* [out][in] */ const wchar_t **r,
            /* [out][in] */ DWORD *r_size);
        
        HRESULT ( STDMETHODCALLTYPE *CopyText )( 
            ITextPage * This,
            /* [in] */ wchar_t *v,
            /* [in] */ DWORD v_size);
        
        HRESULT ( STDMETHODCALLTYPE *Select )( 
            ITextPage * This,
            /* [in] */ DWORD start,
            /* [in] */ DWORD size);
        
        HRESULT ( STDMETHODCALLTYPE *RenderSelection )( 
            ITextPage * This,
            /* [in] */ IRenderingDevice *output);
        
        END_INTERFACE
    } ITextPageVtbl;

    interface ITextPage
    {
        CONST_VTBL struct ITextPageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITextPage_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ITextPage_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ITextPage_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ITextPage_get_Width(This,v)	\
    ( (This)->lpVtbl -> get_Width(This,v) ) 

#define ITextPage_put_Width(This,v)	\
    ( (This)->lpVtbl -> put_Width(This,v) ) 

#define ITextPage_get_Height(This,v)	\
    ( (This)->lpVtbl -> get_Height(This,v) ) 

#define ITextPage_put_Height(This,v)	\
    ( (This)->lpVtbl -> put_Height(This,v) ) 

#define ITextPage_get_Zoom(This,v)	\
    ( (This)->lpVtbl -> get_Zoom(This,v) ) 

#define ITextPage_put_Zoom(This,v)	\
    ( (This)->lpVtbl -> put_Zoom(This,v) ) 

#define ITextPage_get_Dpi(This,v)	\
    ( (This)->lpVtbl -> get_Dpi(This,v) ) 

#define ITextPage_put_Dpi(This,v)	\
    ( (This)->lpVtbl -> put_Dpi(This,v) ) 

#define ITextPage_Render(This,output)	\
    ( (This)->lpVtbl -> Render(This,output) ) 

#define ITextPage_get_Orientation(This,v)	\
    ( (This)->lpVtbl -> get_Orientation(This,v) ) 

#define ITextPage_put_Orientation(This,v)	\
    ( (This)->lpVtbl -> put_Orientation(This,v) ) 


#define ITextPage_Text(This,r,r_size)	\
    ( (This)->lpVtbl -> Text(This,r,r_size) ) 

#define ITextPage_CopyText(This,v,v_size)	\
    ( (This)->lpVtbl -> CopyText(This,v,v_size) ) 

#define ITextPage_Select(This,start,size)	\
    ( (This)->lpVtbl -> Select(This,start,size) ) 

#define ITextPage_RenderSelection(This,output)	\
    ( (This)->lpVtbl -> RenderSelection(This,output) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ITextPage_INTERFACE_DEFINED__ */


#ifndef __IPluginDocument_INTERFACE_DEFINED__
#define __IPluginDocument_INTERFACE_DEFINED__

/* interface IPluginDocument */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IPluginDocument;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-1970-4178-A630-6B0001DEF202")
    IPluginDocument : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_PageCount( 
            /* [retval][out] */ DWORD *v) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPage( 
            /* [in] */ DWORD zero_based_page_number,
            /* [retval][out] */ IPluginPage **r) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPluginDocumentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPluginDocument * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPluginDocument * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPluginDocument * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_PageCount )( 
            IPluginDocument * This,
            /* [retval][out] */ DWORD *v);
        
        HRESULT ( STDMETHODCALLTYPE *GetPage )( 
            IPluginDocument * This,
            /* [in] */ DWORD zero_based_page_number,
            /* [retval][out] */ IPluginPage **r);
        
        END_INTERFACE
    } IPluginDocumentVtbl;

    interface IPluginDocument
    {
        CONST_VTBL struct IPluginDocumentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPluginDocument_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPluginDocument_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPluginDocument_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPluginDocument_get_PageCount(This,v)	\
    ( (This)->lpVtbl -> get_PageCount(This,v) ) 

#define IPluginDocument_GetPage(This,zero_based_page_number,r)	\
    ( (This)->lpVtbl -> GetPage(This,zero_based_page_number,r) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPluginDocument_INTERFACE_DEFINED__ */


#ifndef __IPlugin_INTERFACE_DEFINED__
#define __IPlugin_INTERFACE_DEFINED__

/* interface IPlugin */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IPlugin;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-1970-3C5D-B05F-F61D7799D9FF")
    IPlugin : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE DocFileExtension( 
            /* [out][in] */ const wchar_t **r,
            /* [out][in] */ DWORD *r_size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CopyDocFileExtension( 
            /* [in] */ wchar_t *v,
            /* [in] */ DWORD v_size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DocFileFormat( 
            /* [out][in] */ const wchar_t **r,
            /* [out][in] */ DWORD *r_size) = 0;
        
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
    
#else 	/* C style interface */

    typedef struct IPluginVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPlugin * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPlugin * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPlugin * This);
        
        HRESULT ( STDMETHODCALLTYPE *DocFileExtension )( 
            IPlugin * This,
            /* [out][in] */ const wchar_t **r,
            /* [out][in] */ DWORD *r_size);
        
        HRESULT ( STDMETHODCALLTYPE *CopyDocFileExtension )( 
            IPlugin * This,
            /* [in] */ wchar_t *v,
            /* [in] */ DWORD v_size);
        
        HRESULT ( STDMETHODCALLTYPE *DocFileFormat )( 
            IPlugin * This,
            /* [out][in] */ const wchar_t **r,
            /* [out][in] */ DWORD *r_size);
        
        HRESULT ( STDMETHODCALLTYPE *CopyDocFileFormat )( 
            IPlugin * This,
            /* [in] */ wchar_t *v,
            /* [in] */ DWORD v_size);
        
        HRESULT ( STDMETHODCALLTYPE *CheckHeader )( 
            IPlugin * This,
            /* [in] */ IStream *stream);
        
        HRESULT ( STDMETHODCALLTYPE *GetSignatures )( 
            IPlugin * This,
            /* [out] */ int *signatures_ptr,
            /* [out] */ DWORD *n_signatures);
        
        HRESULT ( STDMETHODCALLTYPE *LoadDocument )( 
            IPlugin * This,
            /* [in] */ IStream *input,
            /* [retval][out] */ IPluginDocument **r);
        
        END_INTERFACE
    } IPluginVtbl;

    interface IPlugin
    {
        CONST_VTBL struct IPluginVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPlugin_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPlugin_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPlugin_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPlugin_DocFileExtension(This,r,r_size)	\
    ( (This)->lpVtbl -> DocFileExtension(This,r,r_size) ) 

#define IPlugin_CopyDocFileExtension(This,v,v_size)	\
    ( (This)->lpVtbl -> CopyDocFileExtension(This,v,v_size) ) 

#define IPlugin_DocFileFormat(This,r,r_size)	\
    ( (This)->lpVtbl -> DocFileFormat(This,r,r_size) ) 

#define IPlugin_CopyDocFileFormat(This,v,v_size)	\
    ( (This)->lpVtbl -> CopyDocFileFormat(This,v,v_size) ) 

#define IPlugin_CheckHeader(This,stream)	\
    ( (This)->lpVtbl -> CheckHeader(This,stream) ) 

#define IPlugin_GetSignatures(This,signatures_ptr,n_signatures)	\
    ( (This)->lpVtbl -> GetSignatures(This,signatures_ptr,n_signatures) ) 

#define IPlugin_LoadDocument(This,input,r)	\
    ( (This)->lpVtbl -> LoadDocument(This,input,r) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPlugin_INTERFACE_DEFINED__ */


#ifndef __IPropertyBasket_INTERFACE_DEFINED__
#define __IPropertyBasket_INTERFACE_DEFINED__

/* interface IPropertyBasket */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IPropertyBasket;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF100")
    IPropertyBasket : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
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
            /* [out][in] */ int *buffer_size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetItemValue( 
            /* [in] */ int item_id,
            /* [in] */ const byte *buffer,
            /* [in] */ int buffer_size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DeleteItem( 
            /* [in] */ long index) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveItem( 
            /* [in] */ int item_id) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FindItem( 
            /* [in] */ int item_id,
            /* [retval][out] */ long *index) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPropertyBasketVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPropertyBasket * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPropertyBasket * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPropertyBasket * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IPropertyBasket * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IPropertyBasket * This,
            /* [retval][out] */ long *v);
        
        HRESULT ( STDMETHODCALLTYPE *GetItemInfo )( 
            IPropertyBasket * This,
            /* [in] */ long index,
            /* [out] */ BSTR *item_name,
            /* [out] */ int *item_id,
            /* [out] */ VARTYPE *var_type);
        
        HRESULT ( STDMETHODCALLTYPE *GetItem )( 
            IPropertyBasket * This,
            /* [in] */ int item_id,
            /* [retval][out] */ VARIANT *v);
        
        HRESULT ( STDMETHODCALLTYPE *SetItem )( 
            IPropertyBasket * This,
            /* [in] */ int item_id,
            /* [in] */ VARIANT v);
        
        HRESULT ( STDMETHODCALLTYPE *GetItemValue )( 
            IPropertyBasket * This,
            /* [in] */ int item_id,
            /* [out] */ byte *buffer,
            /* [out][in] */ int *buffer_size);
        
        HRESULT ( STDMETHODCALLTYPE *SetItemValue )( 
            IPropertyBasket * This,
            /* [in] */ int item_id,
            /* [in] */ const byte *buffer,
            /* [in] */ int buffer_size);
        
        HRESULT ( STDMETHODCALLTYPE *DeleteItem )( 
            IPropertyBasket * This,
            /* [in] */ long index);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveItem )( 
            IPropertyBasket * This,
            /* [in] */ int item_id);
        
        HRESULT ( STDMETHODCALLTYPE *FindItem )( 
            IPropertyBasket * This,
            /* [in] */ int item_id,
            /* [retval][out] */ long *index);
        
        END_INTERFACE
    } IPropertyBasketVtbl;

    interface IPropertyBasket
    {
        CONST_VTBL struct IPropertyBasketVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPropertyBasket_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPropertyBasket_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPropertyBasket_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPropertyBasket_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IPropertyBasket_get_Count(This,v)	\
    ( (This)->lpVtbl -> get_Count(This,v) ) 

#define IPropertyBasket_GetItemInfo(This,index,item_name,item_id,var_type)	\
    ( (This)->lpVtbl -> GetItemInfo(This,index,item_name,item_id,var_type) ) 

#define IPropertyBasket_GetItem(This,item_id,v)	\
    ( (This)->lpVtbl -> GetItem(This,item_id,v) ) 

#define IPropertyBasket_SetItem(This,item_id,v)	\
    ( (This)->lpVtbl -> SetItem(This,item_id,v) ) 

#define IPropertyBasket_GetItemValue(This,item_id,buffer,buffer_size)	\
    ( (This)->lpVtbl -> GetItemValue(This,item_id,buffer,buffer_size) ) 

#define IPropertyBasket_SetItemValue(This,item_id,buffer,buffer_size)	\
    ( (This)->lpVtbl -> SetItemValue(This,item_id,buffer,buffer_size) ) 

#define IPropertyBasket_DeleteItem(This,index)	\
    ( (This)->lpVtbl -> DeleteItem(This,index) ) 

#define IPropertyBasket_RemoveItem(This,item_id)	\
    ( (This)->lpVtbl -> RemoveItem(This,item_id) ) 

#define IPropertyBasket_FindItem(This,item_id,index)	\
    ( (This)->lpVtbl -> FindItem(This,item_id,index) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPropertyBasket_INTERFACE_DEFINED__ */


#ifndef __IFileItem_INTERFACE_DEFINED__
#define __IFileItem_INTERFACE_DEFINED__

/* interface IFileItem */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IFileItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF2B1")
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
    
#else 	/* C style interface */

    typedef struct IFileItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFileItem * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFileItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFileItem * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Attributes )( 
            IFileItem * This,
            /* [retval][out] */ IPropertyBasket **v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            IFileItem * This,
            /* [retval][out] */ BSTR *v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IFileItem * This,
            /* [retval][out] */ BSTR *v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Stream )( 
            IFileItem * This,
            /* [retval][out] */ IStream **v);
        
        END_INTERFACE
    } IFileItemVtbl;

    interface IFileItem
    {
        CONST_VTBL struct IFileItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFileItem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFileItem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFileItem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFileItem_get_Attributes(This,v)	\
    ( (This)->lpVtbl -> get_Attributes(This,v) ) 

#define IFileItem_get_Path(This,v)	\
    ( (This)->lpVtbl -> get_Path(This,v) ) 

#define IFileItem_get_Name(This,v)	\
    ( (This)->lpVtbl -> get_Name(This,v) ) 

#define IFileItem_get_Stream(This,v)	\
    ( (This)->lpVtbl -> get_Stream(This,v) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFileItem_INTERFACE_DEFINED__ */


#ifndef __IFileList_INTERFACE_DEFINED__
#define __IFileList_INTERFACE_DEFINED__

/* interface IFileList */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IFileList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF2B2")
    IFileList : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
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
    
#else 	/* C style interface */

    typedef struct IFileListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFileList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFileList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFileList * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IFileList * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IFileList * This,
            /* [retval][out] */ long *v);
        
        HRESULT ( STDMETHODCALLTYPE *GetItem )( 
            IFileList * This,
            /* [in] */ long index,
            /* [retval][out] */ IFileItem **v);
        
        HRESULT ( STDMETHODCALLTYPE *AddItem )( 
            IFileList * This,
            /* [in] */ IFileItem *item,
            /* [out] */ long *index);
        
        HRESULT ( STDMETHODCALLTYPE *DeleteItem )( 
            IFileList * This,
            /* [in] */ long index);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveItem )( 
            IFileList * This,
            /* [in] */ IFileItem *item);
        
        HRESULT ( STDMETHODCALLTYPE *GetRange )( 
            IFileList * This,
            /* [in] */ long index,
            /* [in] */ long count,
            /* [retval][out] */ IFileList **v);
        
        END_INTERFACE
    } IFileListVtbl;

    interface IFileList
    {
        CONST_VTBL struct IFileListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFileList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFileList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFileList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFileList_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IFileList_get_Count(This,v)	\
    ( (This)->lpVtbl -> get_Count(This,v) ) 

#define IFileList_GetItem(This,index,v)	\
    ( (This)->lpVtbl -> GetItem(This,index,v) ) 

#define IFileList_AddItem(This,item,index)	\
    ( (This)->lpVtbl -> AddItem(This,item,index) ) 

#define IFileList_DeleteItem(This,index)	\
    ( (This)->lpVtbl -> DeleteItem(This,index) ) 

#define IFileList_RemoveItem(This,item)	\
    ( (This)->lpVtbl -> RemoveItem(This,item) ) 

#define IFileList_GetRange(This,index,count,v)	\
    ( (This)->lpVtbl -> GetRange(This,index,count,v) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFileList_INTERFACE_DEFINED__ */


#ifndef __IFileFilter_INTERFACE_DEFINED__
#define __IFileFilter_INTERFACE_DEFINED__

/* interface IFileFilter */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IFileFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF2B3")
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
    
#else 	/* C style interface */

    typedef struct IFileFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFileFilter * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFileFilter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFileFilter * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IFileFilter * This,
            /* [retval][out] */ BSTR *v);
        
        HRESULT ( STDMETHODCALLTYPE *Match )( 
            IFileFilter * This,
            /* [in] */ IFileItem *item);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_CombineMode )( 
            IFileFilter * This,
            /* [retval][out] */ DWORD *v);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_CombineMode )( 
            IFileFilter * This,
            /* [in] */ DWORD v);
        
        END_INTERFACE
    } IFileFilterVtbl;

    interface IFileFilter
    {
        CONST_VTBL struct IFileFilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFileFilter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFileFilter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFileFilter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFileFilter_get_Name(This,v)	\
    ( (This)->lpVtbl -> get_Name(This,v) ) 

#define IFileFilter_Match(This,item)	\
    ( (This)->lpVtbl -> Match(This,item) ) 

#define IFileFilter_get_CombineMode(This,v)	\
    ( (This)->lpVtbl -> get_CombineMode(This,v) ) 

#define IFileFilter_put_CombineMode(This,v)	\
    ( (This)->lpVtbl -> put_CombineMode(This,v) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFileFilter_INTERFACE_DEFINED__ */


#ifndef __IPluginList_INTERFACE_DEFINED__
#define __IPluginList_INTERFACE_DEFINED__

/* interface IPluginList */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IPluginList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B502F1C0-DAC0-456E-A1B8-A6154169E4B9")
    IPluginList : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
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
    
#else 	/* C style interface */

    typedef struct IPluginListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPluginList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPluginList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPluginList * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IPluginList * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IPluginList * This,
            /* [retval][out] */ long *v);
        
        HRESULT ( STDMETHODCALLTYPE *GetItem )( 
            IPluginList * This,
            /* [in] */ long index,
            /* [retval][out] */ IPlugin **v);
        
        HRESULT ( STDMETHODCALLTYPE *AddItem )( 
            IPluginList * This,
            /* [in] */ IPlugin *item,
            /* [out] */ long *index);
        
        HRESULT ( STDMETHODCALLTYPE *DeleteItem )( 
            IPluginList * This,
            /* [in] */ long index);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveItem )( 
            IPluginList * This,
            /* [in] */ IPlugin *item);
        
        HRESULT ( STDMETHODCALLTYPE *GetRange )( 
            IPluginList * This,
            /* [in] */ long index,
            /* [in] */ long count,
            /* [retval][out] */ IPluginList **v);
        
        HRESULT ( STDMETHODCALLTYPE *LoadDocument )( 
            IPluginList * This,
            /* [in] */ IStream *input,
            /* [retval][out] */ IPluginDocument **r);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_FileExtensionFilter )( 
            IPluginList * This,
            /* [retval][out] */ IFileFilter **v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_FileSignatureFilter )( 
            IPluginList * This,
            /* [retval][out] */ IFileFilter **v);
        
        END_INTERFACE
    } IPluginListVtbl;

    interface IPluginList
    {
        CONST_VTBL struct IPluginListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPluginList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPluginList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPluginList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPluginList_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IPluginList_get_Count(This,v)	\
    ( (This)->lpVtbl -> get_Count(This,v) ) 

#define IPluginList_GetItem(This,index,v)	\
    ( (This)->lpVtbl -> GetItem(This,index,v) ) 

#define IPluginList_AddItem(This,item,index)	\
    ( (This)->lpVtbl -> AddItem(This,item,index) ) 

#define IPluginList_DeleteItem(This,index)	\
    ( (This)->lpVtbl -> DeleteItem(This,index) ) 

#define IPluginList_RemoveItem(This,item)	\
    ( (This)->lpVtbl -> RemoveItem(This,item) ) 

#define IPluginList_GetRange(This,index,count,v)	\
    ( (This)->lpVtbl -> GetRange(This,index,count,v) ) 

#define IPluginList_LoadDocument(This,input,r)	\
    ( (This)->lpVtbl -> LoadDocument(This,input,r) ) 

#define IPluginList_get_FileExtensionFilter(This,v)	\
    ( (This)->lpVtbl -> get_FileExtensionFilter(This,v) ) 

#define IPluginList_get_FileSignatureFilter(This,v)	\
    ( (This)->lpVtbl -> get_FileSignatureFilter(This,v) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPluginList_INTERFACE_DEFINED__ */


#ifndef __IFileFilterList_INTERFACE_DEFINED__
#define __IFileFilterList_INTERFACE_DEFINED__

/* interface IFileFilterList */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IFileFilterList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF2B4")
    IFileFilterList : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
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
            /* [in] */ IFileItem *input) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFileFilterListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFileFilterList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFileFilterList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFileFilterList * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IFileFilterList * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IFileFilterList * This,
            /* [retval][out] */ long *v);
        
        HRESULT ( STDMETHODCALLTYPE *GetItem )( 
            IFileFilterList * This,
            /* [in] */ long index,
            /* [retval][out] */ IFileFilter **v);
        
        HRESULT ( STDMETHODCALLTYPE *AddItem )( 
            IFileFilterList * This,
            /* [in] */ IFileFilter *item,
            /* [out] */ long *index);
        
        HRESULT ( STDMETHODCALLTYPE *DeleteItem )( 
            IFileFilterList * This,
            /* [in] */ long index);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveItem )( 
            IFileFilterList * This,
            /* [in] */ IFileFilter *item);
        
        HRESULT ( STDMETHODCALLTYPE *GetRange )( 
            IFileFilterList * This,
            /* [in] */ long index,
            /* [in] */ long count,
            /* [retval][out] */ IFileFilterList **v);
        
        HRESULT ( STDMETHODCALLTYPE *FindItem )( 
            IFileFilterList * This,
            /* [in] */ BSTR filter_name,
            /* [retval][out] */ IFileFilter **v);
        
        HRESULT ( STDMETHODCALLTYPE *Match )( 
            IFileFilterList * This,
            /* [in] */ IFileItem *input);
        
        END_INTERFACE
    } IFileFilterListVtbl;

    interface IFileFilterList
    {
        CONST_VTBL struct IFileFilterListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFileFilterList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFileFilterList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFileFilterList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFileFilterList_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IFileFilterList_get_Count(This,v)	\
    ( (This)->lpVtbl -> get_Count(This,v) ) 

#define IFileFilterList_GetItem(This,index,v)	\
    ( (This)->lpVtbl -> GetItem(This,index,v) ) 

#define IFileFilterList_AddItem(This,item,index)	\
    ( (This)->lpVtbl -> AddItem(This,item,index) ) 

#define IFileFilterList_DeleteItem(This,index)	\
    ( (This)->lpVtbl -> DeleteItem(This,index) ) 

#define IFileFilterList_RemoveItem(This,item)	\
    ( (This)->lpVtbl -> RemoveItem(This,item) ) 

#define IFileFilterList_GetRange(This,index,count,v)	\
    ( (This)->lpVtbl -> GetRange(This,index,count,v) ) 

#define IFileFilterList_FindItem(This,filter_name,v)	\
    ( (This)->lpVtbl -> FindItem(This,filter_name,v) ) 

#define IFileFilterList_Match(This,input)	\
    ( (This)->lpVtbl -> Match(This,input) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFileFilterList_INTERFACE_DEFINED__ */


#ifndef __IProgressEvent_INTERFACE_DEFINED__
#define __IProgressEvent_INTERFACE_DEFINED__

/* interface IProgressEvent */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IProgressEvent;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF2CA")
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
            /* [out][in] */ VARIANT_BOOL *should_terminate) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IProgressEventVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProgressEvent * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProgressEvent * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProgressEvent * This);
        
        HRESULT ( STDMETHODCALLTYPE *ReportProgress )( 
            IProgressEvent * This,
            /* [in] */ BSTR major_info,
            /* [in] */ BSTR minor_info,
            /* [in] */ long major_items_total,
            /* [in] */ long major_items_done,
            /* [in] */ long minor_items_total,
            /* [in] */ long minor_items_done,
            /* [out][in] */ VARIANT_BOOL *should_terminate);
        
        END_INTERFACE
    } IProgressEventVtbl;

    interface IProgressEvent
    {
        CONST_VTBL struct IProgressEventVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProgressEvent_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IProgressEvent_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IProgressEvent_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IProgressEvent_ReportProgress(This,major_info,minor_info,major_items_total,major_items_done,minor_items_total,minor_items_done,should_terminate)	\
    ( (This)->lpVtbl -> ReportProgress(This,major_info,minor_info,major_items_total,major_items_done,minor_items_total,minor_items_done,should_terminate) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IProgressEvent_INTERFACE_DEFINED__ */


#ifndef __IFileEnumeratorProgressEvents_INTERFACE_DEFINED__
#define __IFileEnumeratorProgressEvents_INTERFACE_DEFINED__

/* interface IFileEnumeratorProgressEvents */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IFileEnumeratorProgressEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF2A7")
    IFileEnumeratorProgressEvents : public IProgressEvent
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ReportItemFound( 
            /* [in] */ BSTR uri,
            /* [in] */ IFileItem *item,
            /* [in] */ BSTR item_tip,
            /* [out][in] */ VARIANT_BOOL *should_terminate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReportEnumeratorTip( 
            /* [in] */ BSTR enumerator_tip,
            /* [out][in] */ VARIANT_BOOL *should_terminate) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFileEnumeratorProgressEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFileEnumeratorProgressEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFileEnumeratorProgressEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFileEnumeratorProgressEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *ReportProgress )( 
            IFileEnumeratorProgressEvents * This,
            /* [in] */ BSTR major_info,
            /* [in] */ BSTR minor_info,
            /* [in] */ long major_items_total,
            /* [in] */ long major_items_done,
            /* [in] */ long minor_items_total,
            /* [in] */ long minor_items_done,
            /* [out][in] */ VARIANT_BOOL *should_terminate);
        
        HRESULT ( STDMETHODCALLTYPE *ReportItemFound )( 
            IFileEnumeratorProgressEvents * This,
            /* [in] */ BSTR uri,
            /* [in] */ IFileItem *item,
            /* [in] */ BSTR item_tip,
            /* [out][in] */ VARIANT_BOOL *should_terminate);
        
        HRESULT ( STDMETHODCALLTYPE *ReportEnumeratorTip )( 
            IFileEnumeratorProgressEvents * This,
            /* [in] */ BSTR enumerator_tip,
            /* [out][in] */ VARIANT_BOOL *should_terminate);
        
        END_INTERFACE
    } IFileEnumeratorProgressEventsVtbl;

    interface IFileEnumeratorProgressEvents
    {
        CONST_VTBL struct IFileEnumeratorProgressEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFileEnumeratorProgressEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFileEnumeratorProgressEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFileEnumeratorProgressEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFileEnumeratorProgressEvents_ReportProgress(This,major_info,minor_info,major_items_total,major_items_done,minor_items_total,minor_items_done,should_terminate)	\
    ( (This)->lpVtbl -> ReportProgress(This,major_info,minor_info,major_items_total,major_items_done,minor_items_total,minor_items_done,should_terminate) ) 


#define IFileEnumeratorProgressEvents_ReportItemFound(This,uri,item,item_tip,should_terminate)	\
    ( (This)->lpVtbl -> ReportItemFound(This,uri,item,item_tip,should_terminate) ) 

#define IFileEnumeratorProgressEvents_ReportEnumeratorTip(This,enumerator_tip,should_terminate)	\
    ( (This)->lpVtbl -> ReportEnumeratorTip(This,enumerator_tip,should_terminate) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFileEnumeratorProgressEvents_INTERFACE_DEFINED__ */


#ifndef __IIconRetriever_INTERFACE_DEFINED__
#define __IIconRetriever_INTERFACE_DEFINED__

/* interface IIconRetriever */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IIconRetriever;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-2980-4178-A629-9BE8B8DEF2C9")
    IIconRetriever : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE FindIcon( 
            /* [in] */ int icon_index,
            /* [retval][out] */ int *h_icon) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IIconRetrieverVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IIconRetriever * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IIconRetriever * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IIconRetriever * This);
        
        HRESULT ( STDMETHODCALLTYPE *FindIcon )( 
            IIconRetriever * This,
            /* [in] */ int icon_index,
            /* [retval][out] */ int *h_icon);
        
        END_INTERFACE
    } IIconRetrieverVtbl;

    interface IIconRetriever
    {
        CONST_VTBL struct IIconRetrieverVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IIconRetriever_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IIconRetriever_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IIconRetriever_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IIconRetriever_FindIcon(This,icon_index,h_icon)	\
    ( (This)->lpVtbl -> FindIcon(This,icon_index,h_icon) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IIconRetriever_INTERFACE_DEFINED__ */


#ifndef __IFileEnumerator_INTERFACE_DEFINED__
#define __IFileEnumerator_INTERFACE_DEFINED__

/* interface IFileEnumerator */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IFileEnumerator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-1980-4178-A629-6BE8B8DEF2B5")
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
    
#else 	/* C style interface */

    typedef struct IFileEnumeratorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFileEnumerator * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFileEnumerator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFileEnumerator * This);
        
        HRESULT ( STDMETHODCALLTYPE *FindFromPath )( 
            IFileEnumerator * This,
            /* [in] */ BSTR root_path,
            /* [retval][out] */ IFileList **files);
        
        HRESULT ( STDMETHODCALLTYPE *FindFromItem )( 
            IFileEnumerator * This,
            /* [in] */ IFileItem *item,
            /* [retval][out] */ IFileList **files);
        
        HRESULT ( STDMETHODCALLTYPE *Path2File )( 
            IFileEnumerator * This,
            /* [in] */ BSTR file_path,
            /* [retval][out] */ IFileItem **item);
        
        HRESULT ( STDMETHODCALLTYPE *ItemParent )( 
            IFileEnumerator * This,
            /* [in] */ IFileItem *item,
            /* [retval][out] */ IFileItem **parent);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Filters )( 
            IFileEnumerator * This,
            /* [retval][out] */ IFileFilterList **v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Params )( 
            IFileEnumerator * This,
            /* [retval][out] */ IPropertyBasket **v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_IconRetriever )( 
            IFileEnumerator * This,
            /* [retval][out] */ IIconRetriever **v);
        
        HRESULT ( STDMETHODCALLTYPE *SetProgressEventSink )( 
            IFileEnumerator * This,
            /* [in] */ IFileEnumeratorProgressEvents *v);
        
        END_INTERFACE
    } IFileEnumeratorVtbl;

    interface IFileEnumerator
    {
        CONST_VTBL struct IFileEnumeratorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFileEnumerator_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFileEnumerator_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFileEnumerator_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFileEnumerator_FindFromPath(This,root_path,files)	\
    ( (This)->lpVtbl -> FindFromPath(This,root_path,files) ) 

#define IFileEnumerator_FindFromItem(This,item,files)	\
    ( (This)->lpVtbl -> FindFromItem(This,item,files) ) 

#define IFileEnumerator_Path2File(This,file_path,item)	\
    ( (This)->lpVtbl -> Path2File(This,file_path,item) ) 

#define IFileEnumerator_ItemParent(This,item,parent)	\
    ( (This)->lpVtbl -> ItemParent(This,item,parent) ) 

#define IFileEnumerator_get_Filters(This,v)	\
    ( (This)->lpVtbl -> get_Filters(This,v) ) 

#define IFileEnumerator_get_Params(This,v)	\
    ( (This)->lpVtbl -> get_Params(This,v) ) 

#define IFileEnumerator_get_IconRetriever(This,v)	\
    ( (This)->lpVtbl -> get_IconRetriever(This,v) ) 

#define IFileEnumerator_SetProgressEventSink(This,v)	\
    ( (This)->lpVtbl -> SetProgressEventSink(This,v) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFileEnumerator_INTERFACE_DEFINED__ */


#ifndef __IFeature_INTERFACE_DEFINED__
#define __IFeature_INTERFACE_DEFINED__

/* interface IFeature */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IFeature;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-5463-4178-A629-3DAEE1D910A0")
    IFeature : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ID( 
            /* [retval][out] */ CLSID *v) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetROI( 
            /* [out][in] */ long *count,
            /* [out][in] */ DWORD *v) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ClearAttributes( void) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AttributeCount( 
            /* [retval][out] */ long *v) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAttribute( 
            /* [in] */ long index,
            /* [out] */ BSTR *class_name,
            /* [out] */ CLSID *classifier_id) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FindAttribute( 
            /* [string][in] */ const wchar_t *class_name,
            /* [in] */ CLSID classifier_id,
            /* [out] */ long *index) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAttribute( 
            /* [string][in] */ const wchar_t *class_name,
            /* [in] */ CLSID classifier_id,
            /* [out] */ long *index) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetValue( 
            /* [out] */ byte *buffer,
            /* [out][in] */ int *buffer_size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetValue( 
            /* [in] */ const byte *buffer,
            /* [in] */ int buffer_size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Render( 
            /* [in] */ IPluginPage *page,
            /* [in] */ byte r,
            /* [in] */ byte g,
            /* [in] */ byte b,
            /* [in] */ IRenderingDevice *output) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFeatureVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFeature * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFeature * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFeature * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_ID )( 
            IFeature * This,
            /* [retval][out] */ CLSID *v);
        
        HRESULT ( STDMETHODCALLTYPE *GetROI )( 
            IFeature * This,
            /* [out][in] */ long *count,
            /* [out][in] */ DWORD *v);
        
        HRESULT ( STDMETHODCALLTYPE *ClearAttributes )( 
            IFeature * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_AttributeCount )( 
            IFeature * This,
            /* [retval][out] */ long *v);
        
        HRESULT ( STDMETHODCALLTYPE *GetAttribute )( 
            IFeature * This,
            /* [in] */ long index,
            /* [out] */ BSTR *class_name,
            /* [out] */ CLSID *classifier_id);
        
        HRESULT ( STDMETHODCALLTYPE *FindAttribute )( 
            IFeature * This,
            /* [string][in] */ const wchar_t *class_name,
            /* [in] */ CLSID classifier_id,
            /* [out] */ long *index);
        
        HRESULT ( STDMETHODCALLTYPE *SetAttribute )( 
            IFeature * This,
            /* [string][in] */ const wchar_t *class_name,
            /* [in] */ CLSID classifier_id,
            /* [out] */ long *index);
        
        HRESULT ( STDMETHODCALLTYPE *GetValue )( 
            IFeature * This,
            /* [out] */ byte *buffer,
            /* [out][in] */ int *buffer_size);
        
        HRESULT ( STDMETHODCALLTYPE *SetValue )( 
            IFeature * This,
            /* [in] */ const byte *buffer,
            /* [in] */ int buffer_size);
        
        HRESULT ( STDMETHODCALLTYPE *Render )( 
            IFeature * This,
            /* [in] */ IPluginPage *page,
            /* [in] */ byte r,
            /* [in] */ byte g,
            /* [in] */ byte b,
            /* [in] */ IRenderingDevice *output);
        
        END_INTERFACE
    } IFeatureVtbl;

    interface IFeature
    {
        CONST_VTBL struct IFeatureVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFeature_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFeature_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFeature_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFeature_get_ID(This,v)	\
    ( (This)->lpVtbl -> get_ID(This,v) ) 

#define IFeature_GetROI(This,count,v)	\
    ( (This)->lpVtbl -> GetROI(This,count,v) ) 

#define IFeature_ClearAttributes(This)	\
    ( (This)->lpVtbl -> ClearAttributes(This) ) 

#define IFeature_get_AttributeCount(This,v)	\
    ( (This)->lpVtbl -> get_AttributeCount(This,v) ) 

#define IFeature_GetAttribute(This,index,class_name,classifier_id)	\
    ( (This)->lpVtbl -> GetAttribute(This,index,class_name,classifier_id) ) 

#define IFeature_FindAttribute(This,class_name,classifier_id,index)	\
    ( (This)->lpVtbl -> FindAttribute(This,class_name,classifier_id,index) ) 

#define IFeature_SetAttribute(This,class_name,classifier_id,index)	\
    ( (This)->lpVtbl -> SetAttribute(This,class_name,classifier_id,index) ) 

#define IFeature_GetValue(This,buffer,buffer_size)	\
    ( (This)->lpVtbl -> GetValue(This,buffer,buffer_size) ) 

#define IFeature_SetValue(This,buffer,buffer_size)	\
    ( (This)->lpVtbl -> SetValue(This,buffer,buffer_size) ) 

#define IFeature_Render(This,page,r,g,b,output)	\
    ( (This)->lpVtbl -> Render(This,page,r,g,b,output) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFeature_INTERFACE_DEFINED__ */


#ifndef __IFeatureList_INTERFACE_DEFINED__
#define __IFeatureList_INTERFACE_DEFINED__

/* interface IFeatureList */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IFeatureList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-5463-4178-A629-3DAEE1D910A1")
    IFeatureList : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
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
    
#else 	/* C style interface */

    typedef struct IFeatureListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFeatureList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFeatureList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFeatureList * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IFeatureList * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IFeatureList * This,
            /* [retval][out] */ long *v);
        
        HRESULT ( STDMETHODCALLTYPE *GetItem )( 
            IFeatureList * This,
            /* [in] */ long index,
            /* [retval][out] */ IFeature **v);
        
        HRESULT ( STDMETHODCALLTYPE *AddItem )( 
            IFeatureList * This,
            /* [in] */ IFeature *item,
            /* [out] */ long *index);
        
        HRESULT ( STDMETHODCALLTYPE *DeleteItem )( 
            IFeatureList * This,
            /* [in] */ long index);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveItem )( 
            IFeatureList * This,
            /* [in] */ IFeature *item);
        
        HRESULT ( STDMETHODCALLTYPE *GetRange )( 
            IFeatureList * This,
            /* [in] */ long index,
            /* [in] */ long count,
            /* [retval][out] */ IFeatureList **v);
        
        HRESULT ( STDMETHODCALLTYPE *Render )( 
            IFeatureList * This,
            /* [in] */ IPluginPage *page,
            /* [in] */ byte r,
            /* [in] */ byte g,
            /* [in] */ byte b,
            /* [in] */ IRenderingDevice *output);
        
        END_INTERFACE
    } IFeatureListVtbl;

    interface IFeatureList
    {
        CONST_VTBL struct IFeatureListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFeatureList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFeatureList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFeatureList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFeatureList_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IFeatureList_get_Count(This,v)	\
    ( (This)->lpVtbl -> get_Count(This,v) ) 

#define IFeatureList_GetItem(This,index,v)	\
    ( (This)->lpVtbl -> GetItem(This,index,v) ) 

#define IFeatureList_AddItem(This,item,index)	\
    ( (This)->lpVtbl -> AddItem(This,item,index) ) 

#define IFeatureList_DeleteItem(This,index)	\
    ( (This)->lpVtbl -> DeleteItem(This,index) ) 

#define IFeatureList_RemoveItem(This,item)	\
    ( (This)->lpVtbl -> RemoveItem(This,item) ) 

#define IFeatureList_GetRange(This,index,count,v)	\
    ( (This)->lpVtbl -> GetRange(This,index,count,v) ) 

#define IFeatureList_Render(This,page,r,g,b,output)	\
    ( (This)->lpVtbl -> Render(This,page,r,g,b,output) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFeatureList_INTERFACE_DEFINED__ */


#ifndef __IFeatureAttribute_INTERFACE_DEFINED__
#define __IFeatureAttribute_INTERFACE_DEFINED__

/* interface IFeatureAttribute */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IFeatureAttribute;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-5463-4178-A629-3DAEE1D910A4")
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
            /* [retval][out] */ BSTR *class_name) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetValue( 
            /* [out] */ byte *buffer,
            /* [out][in] */ int *buffer_size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetValue( 
            /* [in] */ const byte *buffer,
            /* [in] */ int buffer_size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Classify( 
            /* [in] */ IPluginPage *page,
            /* [out][in] */ IFeature *feature,
            /* [retval][out] */ long *index) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFeatureAttributeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFeatureAttribute * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFeatureAttribute * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFeatureAttribute * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_ID )( 
            IFeatureAttribute * This,
            /* [retval][out] */ CLSID *v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IFeatureAttribute * This,
            /* [retval][out] */ BSTR *v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_ClassCount )( 
            IFeatureAttribute * This,
            /* [retval][out] */ long *v);
        
        HRESULT ( STDMETHODCALLTYPE *GetClass )( 
            IFeatureAttribute * This,
            /* [in] */ long index,
            /* [retval][out] */ BSTR *class_name);
        
        HRESULT ( STDMETHODCALLTYPE *GetValue )( 
            IFeatureAttribute * This,
            /* [out] */ byte *buffer,
            /* [out][in] */ int *buffer_size);
        
        HRESULT ( STDMETHODCALLTYPE *SetValue )( 
            IFeatureAttribute * This,
            /* [in] */ const byte *buffer,
            /* [in] */ int buffer_size);
        
        HRESULT ( STDMETHODCALLTYPE *Classify )( 
            IFeatureAttribute * This,
            /* [in] */ IPluginPage *page,
            /* [out][in] */ IFeature *feature,
            /* [retval][out] */ long *index);
        
        END_INTERFACE
    } IFeatureAttributeVtbl;

    interface IFeatureAttribute
    {
        CONST_VTBL struct IFeatureAttributeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFeatureAttribute_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFeatureAttribute_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFeatureAttribute_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFeatureAttribute_get_ID(This,v)	\
    ( (This)->lpVtbl -> get_ID(This,v) ) 

#define IFeatureAttribute_get_Name(This,v)	\
    ( (This)->lpVtbl -> get_Name(This,v) ) 

#define IFeatureAttribute_get_ClassCount(This,v)	\
    ( (This)->lpVtbl -> get_ClassCount(This,v) ) 

#define IFeatureAttribute_GetClass(This,index,class_name)	\
    ( (This)->lpVtbl -> GetClass(This,index,class_name) ) 

#define IFeatureAttribute_GetValue(This,buffer,buffer_size)	\
    ( (This)->lpVtbl -> GetValue(This,buffer,buffer_size) ) 

#define IFeatureAttribute_SetValue(This,buffer,buffer_size)	\
    ( (This)->lpVtbl -> SetValue(This,buffer,buffer_size) ) 

#define IFeatureAttribute_Classify(This,page,feature,index)	\
    ( (This)->lpVtbl -> Classify(This,page,feature,index) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFeatureAttribute_INTERFACE_DEFINED__ */


#ifndef __IFeatureAttributeList_INTERFACE_DEFINED__
#define __IFeatureAttributeList_INTERFACE_DEFINED__

/* interface IFeatureAttributeList */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IFeatureAttributeList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-5463-4178-A629-3DAEE1D910A3")
    IFeatureAttributeList : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
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
            /* [out][in] */ IFeature *feature,
            /* [retval][out] */ long *index) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFeatureAttributeListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFeatureAttributeList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFeatureAttributeList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFeatureAttributeList * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IFeatureAttributeList * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IFeatureAttributeList * This,
            /* [retval][out] */ long *v);
        
        HRESULT ( STDMETHODCALLTYPE *GetItem )( 
            IFeatureAttributeList * This,
            /* [in] */ long index,
            /* [retval][out] */ IFeatureAttribute **v);
        
        HRESULT ( STDMETHODCALLTYPE *AddItem )( 
            IFeatureAttributeList * This,
            /* [in] */ IFeatureAttribute *item,
            /* [out] */ long *index);
        
        HRESULT ( STDMETHODCALLTYPE *DeleteItem )( 
            IFeatureAttributeList * This,
            /* [in] */ long index);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveItem )( 
            IFeatureAttributeList * This,
            /* [in] */ IFeatureAttribute *item);
        
        HRESULT ( STDMETHODCALLTYPE *GetRange )( 
            IFeatureAttributeList * This,
            /* [in] */ long index,
            /* [in] */ long count,
            /* [retval][out] */ IFeatureAttributeList **v);
        
        HRESULT ( STDMETHODCALLTYPE *Classify )( 
            IFeatureAttributeList * This,
            /* [in] */ IPluginPage *page,
            /* [out][in] */ IFeature *feature,
            /* [retval][out] */ long *index);
        
        END_INTERFACE
    } IFeatureAttributeListVtbl;

    interface IFeatureAttributeList
    {
        CONST_VTBL struct IFeatureAttributeListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFeatureAttributeList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFeatureAttributeList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFeatureAttributeList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFeatureAttributeList_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IFeatureAttributeList_get_Count(This,v)	\
    ( (This)->lpVtbl -> get_Count(This,v) ) 

#define IFeatureAttributeList_GetItem(This,index,v)	\
    ( (This)->lpVtbl -> GetItem(This,index,v) ) 

#define IFeatureAttributeList_AddItem(This,item,index)	\
    ( (This)->lpVtbl -> AddItem(This,item,index) ) 

#define IFeatureAttributeList_DeleteItem(This,index)	\
    ( (This)->lpVtbl -> DeleteItem(This,index) ) 

#define IFeatureAttributeList_RemoveItem(This,item)	\
    ( (This)->lpVtbl -> RemoveItem(This,item) ) 

#define IFeatureAttributeList_GetRange(This,index,count,v)	\
    ( (This)->lpVtbl -> GetRange(This,index,count,v) ) 

#define IFeatureAttributeList_Classify(This,page,feature,index)	\
    ( (This)->lpVtbl -> Classify(This,page,feature,index) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFeatureAttributeList_INTERFACE_DEFINED__ */


#ifndef __IFeatureDetector_INTERFACE_DEFINED__
#define __IFeatureDetector_INTERFACE_DEFINED__

/* interface IFeatureDetector */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IFeatureDetector;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-5463-4178-A629-3DAEE1D910A2")
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
    
#else 	/* C style interface */

    typedef struct IFeatureDetectorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFeatureDetector * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFeatureDetector * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFeatureDetector * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_ID )( 
            IFeatureDetector * This,
            /* [retval][out] */ CLSID *v);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IFeatureDetector * This,
            /* [retval][out] */ BSTR *v);
        
        HRESULT ( STDMETHODCALLTYPE *LoadClassifiers )( 
            IFeatureDetector * This,
            /* [retval][out] */ IFeatureAttributeList **v);
        
        HRESULT ( STDMETHODCALLTYPE *Find )( 
            IFeatureDetector * This,
            /* [in] */ IPluginPage *page,
            /* [retval][out] */ IFeatureList **v);
        
        HRESULT ( STDMETHODCALLTYPE *Classify )( 
            IFeatureDetector * This,
            /* [in] */ IPluginPage *page,
            /* [in] */ IFeatureAttributeList *classifiers,
            /* [retval][out] */ IFeatureList **v);
        
        END_INTERFACE
    } IFeatureDetectorVtbl;

    interface IFeatureDetector
    {
        CONST_VTBL struct IFeatureDetectorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFeatureDetector_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFeatureDetector_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFeatureDetector_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFeatureDetector_get_ID(This,v)	\
    ( (This)->lpVtbl -> get_ID(This,v) ) 

#define IFeatureDetector_get_Name(This,v)	\
    ( (This)->lpVtbl -> get_Name(This,v) ) 

#define IFeatureDetector_LoadClassifiers(This,v)	\
    ( (This)->lpVtbl -> LoadClassifiers(This,v) ) 

#define IFeatureDetector_Find(This,page,v)	\
    ( (This)->lpVtbl -> Find(This,page,v) ) 

#define IFeatureDetector_Classify(This,page,classifiers,v)	\
    ( (This)->lpVtbl -> Classify(This,page,classifiers,v) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFeatureDetector_INTERFACE_DEFINED__ */


#ifndef __IFeatureDetectorList_INTERFACE_DEFINED__
#define __IFeatureDetectorList_INTERFACE_DEFINED__

/* interface IFeatureDetectorList */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IFeatureDetectorList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5325A8D0-5463-4178-A629-3DAEE1D910A5")
    IFeatureDetectorList : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
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
    
#else 	/* C style interface */

    typedef struct IFeatureDetectorListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFeatureDetectorList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFeatureDetectorList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFeatureDetectorList * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clear )( 
            IFeatureDetectorList * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IFeatureDetectorList * This,
            /* [retval][out] */ long *v);
        
        HRESULT ( STDMETHODCALLTYPE *GetItem )( 
            IFeatureDetectorList * This,
            /* [in] */ long index,
            /* [retval][out] */ IFeatureDetector **v);
        
        HRESULT ( STDMETHODCALLTYPE *AddItem )( 
            IFeatureDetectorList * This,
            /* [in] */ IFeatureDetector *item,
            /* [out] */ long *index);
        
        HRESULT ( STDMETHODCALLTYPE *DeleteItem )( 
            IFeatureDetectorList * This,
            /* [in] */ long index);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveItem )( 
            IFeatureDetectorList * This,
            /* [in] */ IFeatureDetector *item);
        
        HRESULT ( STDMETHODCALLTYPE *GetRange )( 
            IFeatureDetectorList * This,
            /* [in] */ long index,
            /* [in] */ long count,
            /* [retval][out] */ IFeatureDetectorList **v);
        
        HRESULT ( STDMETHODCALLTYPE *FindItem )( 
            IFeatureDetectorList * This,
            /* [in] */ BSTR detector_name,
            /* [retval][out] */ IFeatureDetector **v);
        
        END_INTERFACE
    } IFeatureDetectorListVtbl;

    interface IFeatureDetectorList
    {
        CONST_VTBL struct IFeatureDetectorListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFeatureDetectorList_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IFeatureDetectorList_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IFeatureDetectorList_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IFeatureDetectorList_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define IFeatureDetectorList_get_Count(This,v)	\
    ( (This)->lpVtbl -> get_Count(This,v) ) 

#define IFeatureDetectorList_GetItem(This,index,v)	\
    ( (This)->lpVtbl -> GetItem(This,index,v) ) 

#define IFeatureDetectorList_AddItem(This,item,index)	\
    ( (This)->lpVtbl -> AddItem(This,item,index) ) 

#define IFeatureDetectorList_DeleteItem(This,index)	\
    ( (This)->lpVtbl -> DeleteItem(This,index) ) 

#define IFeatureDetectorList_RemoveItem(This,item)	\
    ( (This)->lpVtbl -> RemoveItem(This,item) ) 

#define IFeatureDetectorList_GetRange(This,index,count,v)	\
    ( (This)->lpVtbl -> GetRange(This,index,count,v) ) 

#define IFeatureDetectorList_FindItem(This,detector_name,v)	\
    ( (This)->lpVtbl -> FindItem(This,detector_name,v) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IFeatureDetectorList_INTERFACE_DEFINED__ */


#ifndef __ICustomClassFactory_INTERFACE_DEFINED__
#define __ICustomClassFactory_INTERFACE_DEFINED__

/* interface ICustomClassFactory */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_ICustomClassFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("39E25190-BD06-4A2E-B3E7-3FA7BAF384A0")
    ICustomClassFactory : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateInstance( 
            /* [in] */ REFCLSID class_id,
            /* [retval][out] */ IPersistStream **v) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICustomClassFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICustomClassFactory * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICustomClassFactory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICustomClassFactory * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateInstance )( 
            ICustomClassFactory * This,
            /* [in] */ REFCLSID class_id,
            /* [retval][out] */ IPersistStream **v);
        
        END_INTERFACE
    } ICustomClassFactoryVtbl;

    interface ICustomClassFactory
    {
        CONST_VTBL struct ICustomClassFactoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICustomClassFactory_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICustomClassFactory_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICustomClassFactory_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICustomClassFactory_CreateInstance(This,class_id,v)	\
    ( (This)->lpVtbl -> CreateInstance(This,class_id,v) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICustomClassFactory_INTERFACE_DEFINED__ */


#ifndef __IEngine_INTERFACE_DEFINED__
#define __IEngine_INTERFACE_DEFINED__

/* interface IEngine */
/* [local][uuid][object] */ 


EXTERN_C const IID IID_IEngine;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EB2A1178-8097-4CDE-A704-66A1FE824FDF")
    IEngine : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE LoadDetectors( 
            /* [retval][out] */ IFeatureDetectorList **detectors) = 0;
        
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
    
#else 	/* C style interface */

    typedef struct IEngineVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEngine * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEngine * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEngine * This);
        
        HRESULT ( STDMETHODCALLTYPE *LoadDetectors )( 
            IEngine * This,
            /* [retval][out] */ IFeatureDetectorList **detectors);
        
        HRESULT ( STDMETHODCALLTYPE *LoadPlugins )( 
            IEngine * This,
            /* [retval][out] */ IPluginList **plugins);
        
        HRESULT ( STDMETHODCALLTYPE *LoadPluginsFromPath )( 
            IEngine * This,
            /* [in] */ BSTR file_path,
            /* [retval][out] */ IPluginList **plugins);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_FileEnumerator )( 
            IEngine * This,
            /* [retval][out] */ IFileEnumerator **v);
        
        HRESULT ( STDMETHODCALLTYPE *Serialize )( 
            IEngine * This,
            /* [in] */ BSTR file_path,
            /* [in] */ IUnknown *v);
        
        HRESULT ( STDMETHODCALLTYPE *Deserialize )( 
            IEngine * This,
            /* [in] */ BSTR file_path,
            /* [retval][out] */ IUnknown **v);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterClassFactory )( 
            IEngine * This,
            /* [in] */ ICustomClassFactory *v);
        
        HRESULT ( STDMETHODCALLTYPE *CreateClassInstance )( 
            IEngine * This,
            /* [in] */ REFCLSID class_id,
            /* [retval][out] */ IPersistStream **v);
        
        END_INTERFACE
    } IEngineVtbl;

    interface IEngine
    {
        CONST_VTBL struct IEngineVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEngine_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IEngine_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IEngine_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IEngine_LoadDetectors(This,detectors)	\
    ( (This)->lpVtbl -> LoadDetectors(This,detectors) ) 

#define IEngine_LoadPlugins(This,plugins)	\
    ( (This)->lpVtbl -> LoadPlugins(This,plugins) ) 

#define IEngine_LoadPluginsFromPath(This,file_path,plugins)	\
    ( (This)->lpVtbl -> LoadPluginsFromPath(This,file_path,plugins) ) 

#define IEngine_get_FileEnumerator(This,v)	\
    ( (This)->lpVtbl -> get_FileEnumerator(This,v) ) 

#define IEngine_Serialize(This,file_path,v)	\
    ( (This)->lpVtbl -> Serialize(This,file_path,v) ) 

#define IEngine_Deserialize(This,file_path,v)	\
    ( (This)->lpVtbl -> Deserialize(This,file_path,v) ) 

#define IEngine_RegisterClassFactory(This,v)	\
    ( (This)->lpVtbl -> RegisterClassFactory(This,v) ) 

#define IEngine_CreateClassInstance(This,class_id,v)	\
    ( (This)->lpVtbl -> CreateClassInstance(This,class_id,v) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IEngine_INTERFACE_DEFINED__ */



#ifndef __Engine_LIBRARY_DEFINED__
#define __Engine_LIBRARY_DEFINED__

/* library Engine */
/* [version][uuid] */ 

























typedef /* [v1_enum] */ 
enum tagPLCL_PIXEL_FORMAT
    {	PLCL_PIXEL_FORMAT_INVALID	= 0,
	PLCL_PIXEL_FORMAT_GRAY_8	= 1,
	PLCL_PIXEL_FORMAT_RGB_24	= 2,
	PLCL_PIXEL_FORMAT_BGR_24	= 4,
	PLCL_PIXEL_FORMAT_RGBA_32	= 8,
	PLCL_PIXEL_FORMAT_ARGB_32	= 16,
	PLCL_PIXEL_FORMAT_ABGR_32	= 32,
	PLCL_PIXEL_FORMAT_BGRA_32	= 64
    } 	PLCL_PIXEL_FORMAT;

typedef /* [v1_enum] */ 
enum tagFILTER_COMBINE_MODE
    {	FILTER_COMBINE_AND	= 1,
	FILTER_COMBINE_OR	= 2,
	FILTER_COMBINE_NOT	= 4,
	FILTER_COMBINE_XOR	= 8
    } 	FILTER_COMBINE_MODE;


EXTERN_C const IID LIBID_Engine;

EXTERN_C const CLSID CLSID_CFileList;

#ifdef __cplusplus

class DECLSPEC_UUID("F4A1A7A9-AC1B-492a-82C4-D31259E08CCE")
CFileList;
#endif

EXTERN_C const CLSID CLSID_CFeatureList;

#ifdef __cplusplus

class DECLSPEC_UUID("7656F6C9-C7D7-5463-95A8-13FB8A453120")
CFeatureList;
#endif
#endif /* __Engine_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


