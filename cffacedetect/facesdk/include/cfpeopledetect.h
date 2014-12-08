#ifndef _CFPEOPLEDETECT_H_
#define _CFPEOPLEDETECT_H_
#include "cftypes.h"
#include "cf_error.h"
#include "cfimage.h"

typedef CF_HANDLE CF_PEOPLEDETECT;

extern "C"
{
/*creation*/
CF_ERROR __EXPORT_TYPE CFCreatePeopleDetector( CF_PEOPLEDETECT *hPeopleDetector, char* path );
CF_ERROR __EXPORT_TYPE CFDestroyPeopleDetector( CF_PEOPLEDETECT *hPeopleDetector );

/*detect people*/
CF_ERROR __EXPORT_TYPE CFDetectPeople( CF_PEOPLEDETECT hPeopleDetector, CF_IMAGE hImage );
CF_ERROR __EXPORT_TYPE CFGetPeopleCount( CF_PEOPLEDETECT hPeopleDetector, int* count );

/*data*/
/*people coordianats*/
CF_ERROR __EXPORT_TYPE CFGetPeoplePos(CF_PEOPLEDETECT hPeopleDetector, int index, int* px_pos, int* py_pos, int* p_radius);
CF_ERROR __EXPORT_TYPE CFGetPeopleRect(CF_PEOPLEDETECT hPeopleDetector, int index, int* r_top, int* r_left, int* r_right, int* r_bottom);


/*face image*/
CF_ERROR __EXPORT_TYPE CFGetPeopleImage(CF_PEOPLEDETECT hPeopleDetector, CF_IMAGE hImage, int index, CF_IMAGE* hPeopleImage);
/*drawing*/
CF_ERROR __EXPORT_TYPE CFDrawPeoplePos( CF_PEOPLEDETECT hPeopleDetector, CF_IMAGE hImage, CF_COLOR color, int index);
}

#endif //_CFPEOPLEDETECT_H_