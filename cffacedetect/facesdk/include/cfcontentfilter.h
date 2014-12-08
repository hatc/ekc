/*
//     Chaos Face SDK version 1.8     
//	   Copyright(c) 2008-2012 CCT gmbH, all rights reserved.
//	   Third party copyrights are property of their respective owners.
//     cfcontentfilter.h 
//     version 1.0.0	
*/
#ifndef _CFCONTENTFILTER_H_
#define _CFCONTENTFILTER_H_

#include "cftypes.h"
#include "cf_error.h"

typedef CF_HANDLE CF_CONTENTFILTER;
enum CF_CONTENT_RESULT {NOTCONTENT, CONTENT, DONTKNOW};

	
struct CF_CONTENT_OPTIONS
{
	double k1;
	double k2;
	double k3;
	bool age_features;
};

/*Create and destroy filter*/
CF_ERROR __EXPORT_TYPE CFCreateContentFilter (CF_CONTENTFILTER* hContentFilter, char* path );

CF_ERROR __EXPORT_TYPE CFDestroyContentFilter(CF_CONTENTFILTER* hContentFilter);

/*Content detection*/
CF_ERROR __EXPORT_TYPE CFDoFilter(CF_IMAGE hImage, CF_CONTENTFILTER hContentFilter, int* result, int* age_result);

/*Content filter options*/
CF_ERROR __EXPORT_TYPE CFGetContentFilterOptions(CF_CONTENTFILTER hContentFilter, CF_CONTENT_OPTIONS* pOptions);

CF_ERROR __EXPORT_TYPE CFSetContentFilterOptions(CF_CONTENTFILTER hContentFilter, CF_CONTENT_OPTIONS* pOptions, bool save2xml);

#endif
