/*
//     Chaos Face SDK version 1.7     
//	   Copyright(c) 2008-2010 CCT gmbH, all rights reserved.
//	   Third party copyrights are property of their respective owners.
//     
*/
#ifndef _CF_H_
#define _CF_H_
#ifdef WIN32
    #include <windows.h>
#endif

#include "cftypes.h"
#include "cf_error.h"
#include "cfimage.h"
#include "cffacedetect.h"
#include "cfpeopledetect.h"
#include "cfobjectdetect.h"
#include "cfattrfilter.h"
#include "cfsymptom.h"
#include "cfversion.h"

/*CF PARAMETER IDs*/
/*preameters for images*/
#define CF_IMG_IMAGEWIDTH_ID   0x00000010
#define CF_IMG_IMAGEHEIGHT_ID  0x00000011
#define CF_IMG_IMAGETYPE_ID    0x00000012

/*Paremeters for face detector*/
#define CF_LF_DETECTORTHR_ID   0x00000020  /*Threshold for face detector*/
#define CF_LF_USECOLOR_ID      0x00000021  /**/
#define CF_LF_MINEYEDIST_ID    0x00000022
#define CF_LF_MAXEYEDIST_ID    0x00000023
#define CF_LF_USETILT_ID       0x00000024

/*pareameters for attribute detector*/
#define  CF_ATTR_CLASS_NAME1   0x00000030
#define  CF_ATTR_CLASS_NAME2   0x00000031
#define  CF_ATTR_THRESHOLD1    0x00000032
#define  CF_ATTR_THRESHOLD2    0x00000033

/*
#ifdef WIN32
    typedef BITMAPINFO CF_BMP;
#else
       typedef struct CF_tagBITMAPINFOHEADER{
        unsigned long      biSize;
        long               biWidth;
        long               biHeight;
        unsigned short     biPlanes;
        unsigned short     biBitCount;
        unsigned long      biCompression;
        unsigned long      biSizeImage;
        long               biXPelsPerMeter;
        long               biYPelsPerMeter;
        unsigned long      biClrUsed;
        unsigned long      biClrImportant;
       } CF_BITMAPINFOHEADER;

       typedef struct CF_tagRGBQUAD {
        unsigned char    rgbBlue;
        unsigned char    rgbGreen;
        unsigned char    rgbRed;
        unsigned char    rgbReserved;
       } CF_RGBQUAD;

       typedef struct CF_tagBITMAPINFO {
        CF_BITMAPINFOHEADER    bmiHeader;
        CF_RGBQUAD             bmiColors[1];
       } CF_BITMAPINFO;

       typedef CF_BITMAPINFO CF_BMP;
#endif

*/
typedef CF_HANDLE CF_IMAGE;
typedef CF_HANDLE CF_LOCATE_FACE;
typedef CF_HANDLE CF_VIDEO_LOCATE_FACE;
typedef CF_HANDLE CF_ATTR_FILTER;

struct CF_PARAM
{
    long ID;            /*parameter name*/
    int  Type;          /*type of parameter*/
    union
    {
        int     intval;
        double  dblval;
        char*   strval;
        bool    bval;
    } Value;
};



extern "C"
{


/*video*/
CF_ERROR __EXPORT_TYPE CFCreateVideoLocateFace( CF_VIDEO_LOCATE_FACE *hVideoLocateFace, char* path );
CF_ERROR __EXPORT_TYPE CFDestroyVideoLocateFace( CF_VIDEO_LOCATE_FACE *hVideoLocateFace );
CF_ERROR __EXPORT_TYPE CFVStart( CF_VIDEO_LOCATE_FACE hVideoLocateFace, CF_IMAGE hImage/*, CF_BUFFER_CHANGED BuffChangeFunc */);
CF_ERROR __EXPORT_TYPE CFVStop( CF_VIDEO_LOCATE_FACE hVideoLocateFace );
CF_ERROR __EXPORT_TYPE CFVNewFrame( CF_VIDEO_LOCATE_FACE hVideoLocateFace, CF_IMAGE hImage );
//CF_ERROR __EXPORT_TYPE CFVGetFaces( CF_VIDEO_LOCATE_FACE hVideoLocateFace, CF_FACE_POS** Faces );

/*params*/
CF_ERROR __EXPORT_TYPE CFSetParam( CF_HANDLE handle, CF_PARAM param );
CF_ERROR __EXPORT_TYPE CFGetParam( CF_HANDLE handle, CF_PARAM* param );
/*attribute detection*/
/*
CF_ERROR __EXPORT_TYPE CFCreateAttrFilter( CF_ATTR_FILTER *hAFilter, char* path );
CF_ERROR __EXPORT_TYPE CFDestroyAttrFilter( CF_ATTR_FILTER *hAFilter );
CF_ERROR __EXPORT_TYPE CFApplyAttrFilter( CF_ATTR_FILTER hAFilter, CF_IMAGE hImage, CF_FACE_POS Face, int& result );
CF_ERROR __EXPORT_TYPE CFGetAttrParams(CF_ATTR_FILTER hAFilter, CF_ATTR_PARAMS* pParams);
CF_ERROR __EXPORT_TYPE CFSetAttrParams(CF_ATTR_FILTER hAFilter, CF_ATTR_PARAMS* pParams);
*/
}    	
#endif
