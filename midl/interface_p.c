

/* this ALWAYS GENERATED file contains the proxy stub code */


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

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "interface.h"

#define TYPE_FORMAT_STRING_SIZE   3                                 
#define PROC_FORMAT_STRING_SIZE   1                                 
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _interface_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } interface_MIDL_TYPE_FORMAT_STRING;

typedef struct _interface_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } interface_MIDL_PROC_FORMAT_STRING;

typedef struct _interface_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } interface_MIDL_EXPR_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const interface_MIDL_TYPE_FORMAT_STRING interface__MIDL_TypeFormatString;
extern const interface_MIDL_PROC_FORMAT_STRING interface__MIDL_ProcFormatString;
extern const interface_MIDL_EXPR_FORMAT_STRING interface__MIDL_ExprFormatString;



#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need a Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const interface_MIDL_PROC_FORMAT_STRING interface__MIDL_ProcFormatString =
    {
        0,
        {

			0x0
        }
    };

static const interface_MIDL_TYPE_FORMAT_STRING interface__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IRenderingData, ver. 0.0,
   GUID={0x5325A8D0,0x1970,0x4178,{0xA6,0x30,0x6B,0x00,0x01,0xDE,0xF1,0xFF}} */


/* Object interface: IRenderingDevice, ver. 0.0,
   GUID={0x5325A8D0,0x1970,0x4178,{0xA6,0x30,0x6B,0x00,0x01,0xDE,0xF2,0x00}} */


/* Object interface: IPluginPage, ver. 0.0,
   GUID={0x5325A8D0,0x1970,0x4178,{0xA6,0x30,0x6B,0x00,0x01,0xDE,0xF2,0x01}} */


/* Object interface: ITextPage, ver. 0.0,
   GUID={0x5325A8D0,0x1970,0x4178,{0xB6,0x30,0x6B,0x00,0x01,0xDE,0xF2,0x01}} */


/* Object interface: IPluginDocument, ver. 0.0,
   GUID={0x5325A8D0,0x1970,0x4178,{0xA6,0x30,0x6B,0x00,0x01,0xDE,0xF2,0x02}} */


/* Object interface: IPlugin, ver. 0.0,
   GUID={0x5325A8D0,0x1970,0x3C5D,{0xB0,0x5F,0xF6,0x1D,0x77,0x99,0xD9,0xFF}} */


/* Object interface: IPropertyBasket, ver. 0.0,
   GUID={0x5325A8D0,0x1980,0x4178,{0xA6,0x29,0x6B,0xE8,0xB8,0xDE,0xF1,0x00}} */


/* Object interface: IFileItem, ver. 0.0,
   GUID={0x5325A8D0,0x1980,0x4178,{0xA6,0x29,0x6B,0xE8,0xB8,0xDE,0xF2,0xB1}} */


/* Object interface: IFileList, ver. 0.0,
   GUID={0x5325A8D0,0x1980,0x4178,{0xA6,0x29,0x6B,0xE8,0xB8,0xDE,0xF2,0xB2}} */


/* Object interface: IFileFilter, ver. 0.0,
   GUID={0x5325A8D0,0x1980,0x4178,{0xA6,0x29,0x6B,0xE8,0xB8,0xDE,0xF2,0xB3}} */


/* Object interface: IPluginList, ver. 0.0,
   GUID={0xB502F1C0,0xDAC0,0x456E,{0xA1,0xB8,0xA6,0x15,0x41,0x69,0xE4,0xB9}} */


/* Object interface: IFileFilterList, ver. 0.0,
   GUID={0x5325A8D0,0x1980,0x4178,{0xA6,0x29,0x6B,0xE8,0xB8,0xDE,0xF2,0xB4}} */


/* Object interface: IProgressEvent, ver. 0.0,
   GUID={0x5325A8D0,0x1980,0x4178,{0xA6,0x29,0x6B,0xE8,0xB8,0xDE,0xF2,0xCA}} */


/* Object interface: IFileEnumeratorProgressEvents, ver. 0.0,
   GUID={0x5325A8D0,0x1980,0x4178,{0xA6,0x29,0x6B,0xE8,0xB8,0xDE,0xF2,0xA7}} */


/* Object interface: IIconRetriever, ver. 0.0,
   GUID={0x5325A8D0,0x2980,0x4178,{0xA6,0x29,0x9B,0xE8,0xB8,0xDE,0xF2,0xC9}} */


/* Object interface: IFileEnumerator, ver. 0.0,
   GUID={0x5325A8D0,0x1980,0x4178,{0xA6,0x29,0x6B,0xE8,0xB8,0xDE,0xF2,0xB5}} */


/* Object interface: IFeature, ver. 0.0,
   GUID={0x5325A8D0,0x5463,0x4178,{0xA6,0x29,0x3D,0xAE,0xE1,0xD9,0x10,0xA0}} */


/* Object interface: IFeatureList, ver. 0.0,
   GUID={0x5325A8D0,0x5463,0x4178,{0xA6,0x29,0x3D,0xAE,0xE1,0xD9,0x10,0xA1}} */


/* Object interface: IFeatureAttribute, ver. 0.0,
   GUID={0x5325A8D0,0x5463,0x4178,{0xA6,0x29,0x3D,0xAE,0xE1,0xD9,0x10,0xA4}} */


/* Object interface: IFeatureAttributeList, ver. 0.0,
   GUID={0x5325A8D0,0x5463,0x4178,{0xA6,0x29,0x3D,0xAE,0xE1,0xD9,0x10,0xA3}} */


/* Object interface: IFeatureDetector, ver. 0.0,
   GUID={0x5325A8D0,0x5463,0x4178,{0xA6,0x29,0x3D,0xAE,0xE1,0xD9,0x10,0xA2}} */


/* Object interface: IFeatureDetectorList, ver. 0.0,
   GUID={0x5325A8D0,0x5463,0x4178,{0xA6,0x29,0x3D,0xAE,0xE1,0xD9,0x10,0xA5}} */


/* Object interface: ICustomClassFactory, ver. 0.0,
   GUID={0x39E25190,0xBD06,0x4A2E,{0xB3,0xE7,0x3F,0xA7,0xBA,0xF3,0x84,0xA0}} */


/* Object interface: IEngine, ver. 0.0,
   GUID={0xEB2A1178,0x8097,0x4CDE,{0xA7,0x04,0x66,0xA1,0xFE,0x82,0x4F,0xDF}} */

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    interface__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x70001f4, /* MIDL Version 7.0.500 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * _interface_ProxyVtblList[] = 
{
    0
};

const CInterfaceStubVtbl * _interface_StubVtblList[] = 
{
    0
};

PCInterfaceName const _interface_InterfaceNamesList[] = 
{
    0
};


#define _interface_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _interface, pIID, n)

int __stdcall _interface_IID_Lookup( const IID * pIID, int * pIndex )
{
    return 0;
}

const ExtendedProxyFileInfo interface_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _interface_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _interface_StubVtblList,
    (const PCInterfaceName * ) & _interface_InterfaceNamesList,
    0, // no delegation
    & _interface_IID_Lookup, 
    0,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

