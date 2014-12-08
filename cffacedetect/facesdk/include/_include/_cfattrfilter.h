/*
//     Chaos Face SDK version 1.8     
//	   Copyright(c) 2008-2010 CCT gmbH, all rights reserved.
//	   Third party copyrights are property of their respective owners.
//     
*/

#ifndef __CFATTRFILTER_H_
#define __CFATTRFILTER_H_
#include "cfattrfilter.h"
#include "_awpipl.h"
#include "LF.h"
#include "LFAttrFilter.h"
#include "tinyxml.h"
#include "_cffacedetect.h"
CF_ERROR CFApplyAttrFilter( CF_ATTR_FILTER hAFilter, CF_IMAGE hImage, CF_FACE_POS Face, int& result );

#endif// __CFATTRFILTER_H_