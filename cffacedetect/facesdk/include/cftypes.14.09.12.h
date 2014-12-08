/*
//     Chaos Face SDK version 1.8     
//	   Copyright(c) 2008-2010 CCT gmbH, all rights reserved.
//	   Third party copyrights are property of their respective owners.
//     
*/
#ifndef  _CF_TYPES_H_
#define  _CF_TYPES_H_

#ifdef WIN32
    #include <windows.h>
#endif

#ifdef WIN32 
    #ifdef __BUILDING_THE_DLL
       #ifdef __BCPLUSPLUS__
            #define  __EXPORT_TYPE   __declspec(dllexport) __stdcall
       #else
        #define __EXPORT_TYPE   __stdcall
       #endif
    #else
	#define __EXPORT_TYPE __declspec(dllimport) __stdcall
    #endif
#else
    #define  __EXPORT_TYPE
#endif

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

typedef void*			 CF_HANDLE;
typedef unsigned char    CF_BYTE;
typedef unsigned short   CF_WORD;
typedef unsigned int	 CF_DWORD;
typedef CF_BYTE			 CF_ERROR;

//point  
struct CF_POINT
{
    CF_WORD X;  /*x - coordinate*/
    CF_WORD Y;  /*y - coordinate*/
};
/*
    Colors for drawing
*/
enum CF_COLOR { CFWHITE, CFBLACK, CFRED, CFGREEN, CFBLUE, CFYELLOW};
enum CF_SHAPE {CFCIRCLE, CFRECT, CFCORNERS, CFELLIPSE};


#endif//_CF_TYPES_H_