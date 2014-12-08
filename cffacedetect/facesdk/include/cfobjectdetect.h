#ifndef _CF_OBJECTDETECT_H_ 
#define _CF_OBJECTDETECT_H_ 
#include "cftypes.h"
#include "cf_error.h"
#include "cfimage.h"

typedef CF_HANDLE CF_OBJECTDETECTOR;
extern "C"
{
/*creation*/
CF_ERROR __EXPORT_TYPE CFCreateObjectDetector( CF_OBJECTDETECTOR *hObjectDetector, char* path );
CF_ERROR __EXPORT_TYPE CFDestroyObjectDetector( CF_OBJECTDETECTOR *hObjectDetector );

/*detect objects*/
CF_ERROR __EXPORT_TYPE CFDetectObject( CF_OBJECTDETECTOR hObjectDetector, CF_IMAGE hImage );
CF_ERROR __EXPORT_TYPE CFGetObjectCount( CF_OBJECTDETECTOR hObjectDetector, int* count );

/*data*/
/*objects coordianats*/
CF_ERROR __EXPORT_TYPE CFGetObjectRect(CF_OBJECTDETECTOR hObjectDetector, int index, int* r_top, int* r_left, int* r_right, int* r_bottom);
// 
/*object image*/
CF_ERROR __EXPORT_TYPE CFGetObjectImage(CF_OBJECTDETECTOR hObjectDetector, CF_IMAGE hImage, int index, CF_IMAGE* hPeopleImage);
/*drawing*/
CF_ERROR __EXPORT_TYPE CFDrawObjectPos( CF_OBJECTDETECTOR hObjectDetector, CF_IMAGE hImage, CF_COLOR color, int index);
}

#endif //_CF_OBJECTDETECT_H_ 