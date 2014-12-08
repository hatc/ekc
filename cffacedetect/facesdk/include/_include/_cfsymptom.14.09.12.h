#ifndef __CFSYMPTOM_H_
#define __CFSYMPTOM_H_
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

#endif//