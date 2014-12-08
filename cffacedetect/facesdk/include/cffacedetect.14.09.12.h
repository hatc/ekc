/*
//     Chaos Face SDK version 1.8     
//	   Copyright(c) 2008-2010 CCT gmbH, all rights reserved.
//	   Third party copyrights are property of their respective owners.
//     cffacedetect.h 
//     version 1.0.0	
*/

#ifndef _CFFACEDETECT_H_
#define _CFFACEDETECT_H_
#include "cftypes.h"
#include "cf_error.h"
#include "cfimage.h"

typedef CF_HANDLE CF_LOCATE_FACE;
enum CF_FACE_TILT {CFLEFT, CFFRONT, CFRIGHT};
enum CF_FACE_SIZES  {CFSMALL, CFMEDIUM, CFBIG};
//    Locate Face Params
struct CF_LOCATE_FACE_PARAMS
{
   CF_FACE_SIZES face_sizes;       // face size to find
   bool          use_color;        // use color filter
   bool          fast_computing;   // use fast computing
};
// progress callcback
typedef  void (__stdcall *CF_PROGRESS)(int Progress, const char* str_text);
extern "C"
{
/*creation*/
CF_ERROR __EXPORT_TYPE CFCreateLocateFace( CF_LOCATE_FACE *hLocateFace, char* path );
CF_ERROR __EXPORT_TYPE CFDestroyLocateFace( CF_LOCATE_FACE *hLocateFace );

/*detect faces*/
CF_ERROR __EXPORT_TYPE CFFindFaces( CF_LOCATE_FACE hLocateFace, CF_IMAGE hImage );
CF_ERROR __EXPORT_TYPE CFGetFacesCount( CF_LOCATE_FACE hLocateFace, int* count );

/*options*/
CF_ERROR __EXPORT_TYPE CFGetLocateFaceParams(CF_LOCATE_FACE hLocateFace, CF_LOCATE_FACE_PARAMS* pParams);
CF_ERROR __EXPORT_TYPE CFSetLocateFaceParams(CF_LOCATE_FACE hLocateFace, CF_LOCATE_FACE_PARAMS* pParams);
/*data*/
/*face coordianats*/
CF_ERROR __EXPORT_TYPE CFGetFacePos(CF_LOCATE_FACE hLocateFace, int index, int* px_pos, int* py_pos, int* p_radius);
CF_ERROR __EXPORT_TYPE CFGetEyePos(CF_LOCATE_FACE hLocateFace, int index, CF_POINT* pLe, CF_POINT* pRe);

/*face image*/
CF_ERROR __EXPORT_TYPE CFGetFaceImage(CF_LOCATE_FACE hLocateFace, CF_IMAGE hImage, int index, CF_IMAGE* hFaceImage);
/*utils*/
CF_ERROR __EXPORT_TYPE CFSetProgress( CF_LOCATE_FACE hLocateFace, CF_PROGRESS ProgressFunc);

/*drawing*/
CF_ERROR __EXPORT_TYPE CFDrawFacesPos( CF_LOCATE_FACE hLocateFace, CF_IMAGE hImage, CF_COLOR color, CF_SHAPE shape, float radius, bool draw_eye_pos);

}
#endif//_CFFACEDETECT_H_