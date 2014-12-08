#ifndef __CF_OPENCV_H_
#define __CF_OPENCV_H_
#include "awpipl.h"
#include "cxcore.h"
// awp to openCV image convertion
IplImage* awpConvertToIplImage(awpImage* src);
void awpImageToOpenCvImage(awpImage* src, IplImage* dst);
//openCV to awp convertion
awpImage* awpConvertToAwpImage(IplImage* src);
//AWPRESULT awpCopyImageMask(awpImage* pSrc, awpImage* pMask, awpImage** ppDst, int thr);

inline int iplWidth( IplImage* img )
{
    return !img ? 0 : !(img->roi) ? img->width : img->roi->width;
}

inline int iplHeight( IplImage* img )
{
    return !img ? 0 : !(img->roi) ? img->height : img->roi->height;
}
#endif //__CF_OPENCV_H_