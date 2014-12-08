/*
//     Chaos Face SDK version 1.8     
//	   Copyright(c) 2008-2010 CCT gmbH, all rights reserved.
//	   Third party copyrights are property of their respective owners.
//     cfattrfilter.h 
//     version 1.0.0	
*/
#ifndef _CFATTRFILTER_H_
#define _CFATTRFILTER_H_
#include "cftypes.h"
#include "cf_error.h"
#include "cfimage.h"
#include "cffacedetect.h"

typedef CF_HANDLE CF_ATTR_FILTER;
/*
//    Attribute filter parameters
*/
struct CF_ATTR_PARAMS
{
   char* class_name1;
   char* class_name2;
   float threshold1;
   float threshold2;
   bool  fastcomputing; //
   char* classificator;
};

extern "C"
{
/*attribute detection*/
CF_ERROR __EXPORT_TYPE CFCreateAttrFilter( CF_ATTR_FILTER *hAFilter, char* path );
CF_ERROR __EXPORT_TYPE CFDestroyAttrFilter( CF_ATTR_FILTER *hAFilter );
CF_ERROR __EXPORT_TYPE CFGetAttrParams(CF_ATTR_FILTER hAFilter, CF_ATTR_PARAMS* pParams);
CF_ERROR __EXPORT_TYPE CFSetAttrParams(CF_ATTR_FILTER hAFilter, CF_ATTR_PARAMS* pParams);
CF_ERROR __EXPORT_TYPE CFApplyAttrFilter( CF_ATTR_FILTER hAFilter, CF_IMAGE hImage, int xPosLe, int yPosLe, int xPosRe, int yPosRe,  int& result );

}
#endif//_CFATTRFILTER_H_ 