#ifndef __CFSYMPTOM_H_
#define __CFSYMPTOM_H_

#define _OPENCV_
#define HAVE_JPEG
#include "math.h"
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "_awpipl.h"
#include "_cfopencv.h"

#include "cfsymptom.h"
#include "LF.h"
#include "LFSymptom.h"
#include "tinyxml.h"
#define _GET_SYMPTOM_  TSymptom* symptom = (TSymptom*)hSymptom;\
	if (symptom == NULL) \
	{\
		status = CFF_INVALID_HANDLE;\
		return status;\
	}

IplImage* RenderVerticalCameraOut(CF_SYMPTOM_CAMERA_RENDER_OPTIONS* pOptions);
IplImage* renderHorisontalCameraOut(CF_SYMPTOM_CAMERA_RENDER_OPTIONS* pOptions);

#endif//