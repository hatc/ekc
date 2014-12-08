#ifndef __CFPEOPLEDETECT_H_
#define __CFPEOPLEDETECT_H_
#include "LF.h"
#include "awpipl.h"
#include "cfpeopledetect.h"
#include "cv.h"
#include "cvaux.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// awp to openCV image convertion
IplImage* awpConvertToIplImage(awpImage* src);
void awpImageToOpenCvImage(awpImage* src, IplImage* dst);
//openCV to awp convertion
awpImage* awpConvertToAwpImage(IplImage* src);
AWPRESULT awpCopyImageMask(awpImage* pSrc, awpImage* pMask, awpImage** ppDst, int thr);

inline int iplWidth( IplImage* img )
{
    return !img ? 0 : !(img->roi) ? img->width : img->roi->width;
}

inline int iplHeight( IplImage* img )
{
    return !img ? 0 : !(img->roi) ? img->height : img->roi->height;
}


#endif //__CFPEOPLEDETECT_H_