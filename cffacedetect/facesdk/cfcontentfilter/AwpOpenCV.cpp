//---------------------------------------------------------------------------
#include "_cfcontentfilter.h"

//converts awpImage.awpType to openCV depth
//in the error case returns -1

AWPRESULT awpCopyImageMask(awpImage* pSrc, awpImage* pMask, awpImage** ppDst, int thr)
{
   // проверим указатели
   if (pSrc == NULL)
        return AWP_BADARG;
   if (pMask == NULL)
        return AWP_BADARG;
   if (ppDst == NULL)
        return AWP_BADARG;
   // проверим размеры исходного изображения и маски.
   // они должны совпадать
   if (pSrc->sSizeX != pMask->sSizeX || pSrc->sSizeY != pMask->sSizeY)
        return AWP_BADARG;
   // маска должна  иметь 1 байт на точку
   if (pMask->bChannels != 1 || pMask->dwType != AWP_BYTE)
        return AWP_BADARG;
   AWPRESULT res = AWP_OK;
   res = awpCreateImage(ppDst, pSrc->sSizeX, pSrc->sSizeY, pSrc->bChannels, pSrc->dwType);
   if (res != AWP_OK)
        return res;
   // предполагаем, что pSrc имеет три байта на точку
   awpColor* s = (awpColor*)pSrc->pPixels;
   awpColor* r = (awpColor*)(*ppDst)->pPixels;
   BYTE*     b = (BYTE*)pMask->pPixels;
   for (int i = 0; i < pSrc->sSizeX*pSrc->sSizeY; i++)
   {
        if (b[i] > thr)
            r[i] = s[i];
   }
   return AWP_OK;
}

int awpImageDepthToCvDepth(awpImage* src)
{
    switch(src->dwType)
    {
        case AWP_BYTE:
            return IPL_DEPTH_8U;
        case AWP_SHORT:
            return IPL_DEPTH_8S;
        case AWP_FLOAT:
            return IPL_DEPTH_32F;
		case AWP_DOUBLE:
			return 64;
        default:
            return -1;
    }
}
//converts awpImage.awpType to openCV depth
//in the error case returns -1
static int awpCvDepthToImageDepth(IplImage* src)
{
    switch(src->depth)
    {
        case IPL_DEPTH_8U:
            return AWP_BYTE;
        case IPL_DEPTH_8S:
            return AWP_SHORT;
        case IPL_DEPTH_32F:
            return AWP_FLOAT;
		case 64:
			return AWP_DOUBLE;
        default:
            return -1;
    }
}
//returns line width of awpImage in BYTES
//in the error case returns -1
static int awpWidthStep(awpImage* src)
{
    int c = src->bChannels*src->sSizeX;
    switch(src->dwType)
    {
        case AWP_BYTE:
            return c*sizeof(BYTE);
        case AWP_SHORT:
            return c*sizeof(short);
        case AWP_FLOAT:
            return c*sizeof(float);
		case AWP_DOUBLE:
			return c*sizeof(double);
        default:
            return -1;
    }
}
// awp to openCV image convertion
IplImage* awpConvertToIplImage(awpImage* src)
{
  IplImage* result = NULL;
  if (src == NULL)
    return result;
  CvSize size;
  if (awpCheckImage(src) != AWP_OK)
    return result;
  size.width  = src->sSizeX;
  size.height = src->sSizeY;
  int depth = awpImageDepthToCvDepth(src);
  if (depth == -1)
    return result;
  int awpStep = awpWidthStep(src);
  if (awpStep == -1)
    return result;
  result = cvCreateImage(size, depth, src->bChannels);
  if (result == NULL)
    return result;
  // copy image pixels
  BYTE* awp = (BYTE*)src->pPixels;
  BYTE* ipl = (BYTE*)result->imageData;

  int iplStep = result->widthStep;
  for (int i = 0; i < src->sSizeY; i++)
  {
    memcpy(ipl,awp, awpStep);
    ipl += iplStep;
    awp += awpStep;
  }
  return result;
}

void awpImageToOpenCvImage(awpImage* src, IplImage* dst)
{
  if (awpCheckImage(src) != AWP_OK)
    return;
  int awpStep = awpWidthStep(src);
  int iplStep = dst->widthStep;
  BYTE* awp = (BYTE*)src->pPixels;
  BYTE* ipl = (BYTE*)dst->imageData;
  for (int i = 0; i < src->sSizeY; i++)
  {
    memcpy(ipl,awp, awpStep);
    ipl += iplStep;
    awp += awpStep;
  }
}

//openCV to awp convertion
awpImage* awpConvertToAwpImage(IplImage* src)
{
    awpImage* result = NULL;
    if (src == NULL)
        return result;
//    if (src->nChannels != 1)
//        return result;
    int type = awpCvDepthToImageDepth(src);
    if (type < 0)
        return result;
	if (awpCreateImage(&result, src->width, src->height, src->nChannels, type) != AWP_OK)
        return result;
    BYTE* awp = (BYTE*)result->pPixels;
    BYTE* ipl = (BYTE*)src->imageData;
    int awpStep = awpWidthStep(result);
    int iplStep = src->widthStep;
    for (int i = 0; i < result->sSizeY; i++)
    {
      memcpy(awp, ipl, awpStep);
      ipl += iplStep;
      awp += awpStep;
    }
     return result;

}

//#endif
