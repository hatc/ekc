//---------------------------------------------------------------------------
#ifndef __CFCONTENTFILTER_H_
#define __CFCONTENTFILTER_H_
//---------------------------------------------------------------------------

//#include "cxtypes.h"
#include "cxcore.h"
#include "_awpipl.h"
#include "ccontentfilter.h"


struct  _CFContentFilter
{
	int size; // sizeof(_CFContentFilter)
	CContentFilter* pFilter; // указатель на фильтр

	_CFContentFilter(){size = 0; pFilter = NULL;}
};

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


#endif
