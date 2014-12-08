#include "_cfsymptom.h"

//const char* c_lpRootName = "Symptom";
const char* c_lpParamsName = "SettingsSymptom";
const char* c_lpParamWidth = "SourceWidth";
const char* c_lpParamHeight = "SourceHeight";
const char* c_lpParamRoiLeft = "RoiLeft";
const char* c_lpParamRoiTop = "RoiTop";
const char* c_lpParamRoiWidth = "RoiWidth";
const char* c_lpParamRoiHeight = "RoiHeight";
const char* c_lpParamsDetector = "BlobDetector";
const char* c_lpParamsMinBlobSquare  = "MinBlobSquare";
const char* c_lpParamsMaxBlobSquare = "MaxBlobSquare";
const char* c_lpParamsMinBlobAspect = "MinBlobAspect";
const char* c_lpParamsMaxBlobAspect = "MaxBlobaspect";
const char* c_lpParamsMinBlobAge = "MinBlobAge";
const char* c_lpParamsMinBlobImmovAge = "MinBlobImmovbAge";

/* events 
*/
struct _CF_Symprom_Events
{
	CF_SYMPTOM_SIGNAL m_NewBlobSignal;
	CF_SYMPTOM_SIGNAL m_LostBlobSignal;
	CF_SYMPTOM_SIGNAL m_BlobPosChangedSignal;
	CF_SYMPTOM_SIGNAL m_BlobImmovableSignal;
	CF_SYMPTOM_SIGNAL m_BlobAgeSignal;
	CF_SYMPTOM_SIGNAL m_BlobAgeImmovableSignal;
	_CF_Symprom_Events()
	{
	m_NewBlobSignal = NULL;
	m_LostBlobSignal = NULL;
	m_BlobPosChangedSignal = NULL;
	m_BlobImmovableSignal = NULL;
	m_BlobAgeSignal = NULL;
	m_BlobAgeImmovableSignal = NULL;
	}
};


CF_ERROR __EXPORT_TYPE CFCreateSymptom(CF_SYMPTOM* hSymptom)
{
	CF_ERROR status = CF_SUCCESS;
	if (hSymptom == NULL)
	{
		return CFF_INVALID_HANDLE;
	}

	TSymptom* symptom = NULL;
	symptom = new TSymptom();
	if (symptom == NULL)
	{
		status = CFCR_NOT_ENOUGH_MEMORY;
		return status;		
	}
	_CF_Symprom_Events* events = NULL;
	events = new _CF_Symprom_Events();
	if (events == NULL)
	{
		status = CFCR_NOT_ENOUGH_MEMORY;
		delete symptom;
		*hSymptom = NULL;
		return status;
	}
	symptom->SetUserData(events);
	*hSymptom = symptom;
	return status;
}

CF_ERROR __EXPORT_TYPE CFDestroySymptom(CF_SYMPTOM* hSymptom)
{
	CF_ERROR status = CF_SUCCESS;
	TSymptom* symptom = (TSymptom*)*hSymptom;
	if (symptom == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	
	delete symptom;
	*hSymptom = NULL;
	return status;
}


/*processing 
*/
CF_ERROR __EXPORT_TYPE CFBeginSurv(CF_SYMPTOM hSymptom, CF_IMAGE hImage)
{
	CF_ERROR status = CF_SUCCESS;
	_GET_SYMPTOM_
	//todo: setup time
	TLFImage* image = (TLFImage*)hImage;
	if (image == NULL)
		return CFF_INVALID_HANDLE;
	symptom->SetImage(image->GetImage());
	return status;
}

static void _cf_lfblob_to_cfblob(TLFBLOBObject* blob, CF_SYMPTOM_BLOB& out_blob)
{
	out_blob.size = sizeof(CF_SYMPTOM_BLOB);
	out_blob.age  = blob->GetAge();
	out_blob.ageos= blob->GetStayCount();
	out_blob.blob_type = sbUnknown;
	out_blob.height = blob->GetBounds().bottom - blob->GetBounds().top;
	out_blob.id = blob->GetId();
	out_blob.left = blob->GetBounds().left;
	out_blob.mass_x = blob->GetCenter().X;
	out_blob.mass_y = blob->GetCenter().Y;
	out_blob.top = blob->GetBounds().top;
	out_blob.width = blob->GetBounds().right - blob->GetBounds().left;
}

CF_ERROR __EXPORT_TYPE CFAddSurvImage(CF_SYMPTOM hSymptom, CF_IMAGE hImage)
{
	CF_ERROR status = CF_SUCCESS;
	
	_GET_SYMPTOM_
	
	TLFImage* image = (TLFImage*)hImage;
	if (image == NULL)
		return CFF_INVALID_HANDLE;
	
	_CF_Symprom_Events* events = (_CF_Symprom_Events*)symptom->GetUserData();
	


	if (events->m_LostBlobSignal != NULL)
	{
		// check for "lost event"
		for (int i = 0; i < symptom->GetNumBlobs(); i++)
		{
			TLFBLOBObject* blob = (TLFBLOBObject*)symptom->GetBlob(i);
			if (blob->Getleave() == true)
			{
				CF_SYMPTOM_BLOB out_blob;
				_cf_lfblob_to_cfblob(blob, out_blob);
				events->m_LostBlobSignal(&out_blob);
			}
			delete blob;
		}
	}

	symptom->ClearObjects();

	// process image
	symptom->SetImage(image->GetImage());
	// send signals 
	for (int i = 0; i < symptom->GetNumBlobs(); i++)
	{
		TLFBLOBObject* blob = symptom->GetBlob(i);
		CF_SYMPTOM_BLOB out_blob;
		_cf_lfblob_to_cfblob(blob, out_blob);
		if (events->m_NewBlobSignal != NULL)
		{
			if (blob->GetAge() == 0)
				events->m_NewBlobSignal(&out_blob);
		}
		if (events->m_BlobPosChangedSignal != NULL)
		{
			if (blob->GetStayCount() == 0 && blob->GetAge() > 0)
				events->m_BlobPosChangedSignal(&out_blob);
		}
		if (events->m_BlobImmovableSignal != NULL)
		{
			if (blob->GetStayCount() > 0)
				events->m_BlobImmovableSignal(&out_blob);
		}
		if (events->m_BlobAgeSignal != NULL)
		{
			if (blob->GetAge() > symptom->GetMinBlobAge())
				events->m_BlobAgeSignal(&out_blob);
		}
		if (events->m_BlobAgeImmovableSignal != NULL)
		{
			if (blob->Alarm())
				events->m_BlobAgeImmovableSignal(&out_blob);
		}
		delete blob;
	}
	return status;
}

CF_ERROR __EXPORT_TYPE CFEndSurv(CF_SYMPTOM hSymptom, CF_IMAGE hImage)
{
	CF_ERROR status = CF_SUCCESS;
	
	_GET_SYMPTOM_
	//todo: end time
	symptom->Reset();
	return status;
}


/* state
*/
CF_ERROR __EXPORT_TYPE CFGetNumBlobs(CF_SYMPTOM hSymptom, int* pNumBlobs)
{
	CF_ERROR status = CF_SUCCESS;
	
	_GET_SYMPTOM_

	if (pNumBlobs == NULL)
		return CFF_INVALID_PARAM;
	*pNumBlobs = symptom->GetNumBlobs();
	return status;
}

CF_ERROR __EXPORT_TYPE CFGetBlob(CF_SYMPTOM hSymptom, int index, CF_SYMPTOM_BLOB* pBlob)
{
	CF_ERROR status = CF_SUCCESS;
	
	_GET_SYMPTOM_
	
	if (pBlob == NULL)
		return CFF_INVALID_PARAM;

	if (pBlob->size != sizeof(CF_SYMPTOM_BLOB))
		return CFF_INVALID_PARAM;

	if (index < 0 || index > symptom->GetNumBlobs())
		return CFF_INVALID_PARAM;

	TLFBLOBObject* bo = symptom->GetBlob(index);
	awpRect bounds = bo->GetBounds();
	
	pBlob->left = bounds.left;
	pBlob->top  = bounds.top;
	pBlob->width = bounds.right - bounds.left;
	pBlob->height = bounds.bottom - bounds.top;
	pBlob->id = bo->GetId(); // todo: change this to true id
	pBlob->mass_x = bo->GetCenter().X;
	pBlob->mass_y = bo->GetCenter().Y;
	//todo: convert age and ageos to sec
	pBlob->age = (float)bo->GetAge();
	pBlob->ageos = (float)bo->GetStayCount();
	//todo: setup blob_type
	pBlob->blob_type = sbUnknown;
	delete bo;
	return status;
}


/* params
*/
CF_ERROR __EXPORT_TYPE CFGetSurvParams(CF_SYMPTOM hSymptom, CF_SYMPTOM_PARAMS* pParams)
{
	CF_ERROR status = CF_SUCCESS;
	
	_GET_SYMPTOM_
	
	if (pParams == NULL)
		return CFF_INVALID_PARAM;
	if (pParams->size != sizeof(CF_SYMPTOM_PARAMS))
		return CFF_INVALID_PARAM;
	
	pParams->detector = sbdAbsDiffBackground; // todo: получать этот параметр из переменной symptom
	pParams->max_blob_aspect_ratio = symptom->GetMaxAspectRatio();
	pParams->max_blob_square = symptom->GetMaxBlobSquare();
	pParams->min_blob_age = symptom->GetMinBlobAge();
	pParams->min_blob_age_immov = symptom->GetMinBlobImmoveableAge();
	pParams->min_blob_aspect_ratio = symptom->GetMinAspectRatio();
	pParams->min_blob_square = symptom->GetMinBlobSquare();
	pParams->roi_height = symptom->GetRoiRect().bottom - symptom->GetRoiRect().top;
	pParams->roi_left = symptom->GetRoiRect().left;
	pParams->roi_top = symptom->GetRoiRect().top;
	pParams->roi_width = symptom->GetRoiRect().right - symptom->GetRoiRect().left;
	pParams->source_width = symptom->GetSourceWidth();
	pParams->source_height = symptom->GetSourceHeight();

	return status;
}

CF_ERROR __EXPORT_TYPE CFSetSurvParams(CF_SYMPTOM hSymptom, CF_SYMPTOM_PARAMS* pParams)
{
	CF_ERROR status = CF_SUCCESS;

	_GET_SYMPTOM_

	if (pParams == NULL)
		return CFF_INVALID_PARAM;
	if (pParams->size != sizeof(CF_SYMPTOM_PARAMS))
		return CFF_INVALID_PARAM;

	symptom->BeginChangeParams();

	// установка Roi
	awpImage* tmp = NULL;
	if (awpCreateImage(&tmp, symptom->GetSourceWidth(), 
		symptom->GetSourceHeight(), 1, AWP_BYTE) == AWP_OK)
	{
		awpRect test_rect;
		test_rect.left = pParams->roi_left;
		test_rect.top = pParams->roi_top;
		test_rect.right = test_rect.left + pParams->roi_width;
		test_rect.bottom = test_rect.top + pParams->roi_height;
		if (awpRectInImage(tmp, &test_rect) == AWP_OK)
		{
			symptom->SetRoiRect(test_rect);
		}
		else
			status = CFF_INVALID_PARAM;
		awpReleaseImage(&tmp);
		if (status != CF_SUCCESS)
			goto cleanup;
	}
	else
		return CFF_INVALID_PARAM;
	// установка параметров тайминга
	if (pParams->min_blob_age < 0)
	{
		status = CFF_INVALID_PARAM;
		goto cleanup;
	}
	symptom->SetMinBlobAge(pParams->min_blob_age);
	if (pParams->min_blob_age_immov < 0)
	{
		status = CFF_INVALID_PARAM;
		goto cleanup;
	}
	symptom->SetMinBlobImmoveableAge(pParams->min_blob_age_immov);
	// установка аспекта 
	if (pParams->min_blob_aspect_ratio < 0 || pParams->min_blob_aspect_ratio > pParams->max_blob_aspect_ratio)
	{
  	    status = CFF_INVALID_PARAM;
    	goto cleanup;
	}
	symptom->SetMinAspectRatio(pParams->min_blob_aspect_ratio);
	symptom->SetMaxAspectRatio(pParams->max_blob_aspect_ratio);
	// установка площади
	if(pParams->min_blob_square < 0 || pParams->min_blob_square > pParams->max_blob_square)
	{
  	    status = CFF_INVALID_PARAM;
    	goto cleanup;
	}
	symptom->SetMaxBlobSquare(pParams->max_blob_square);
	symptom->SetMinBlobSquare(pParams->min_blob_square);
cleanup:
	if (status != CF_SUCCESS)
		symptom->RollBackChangeParams();
	else
		symptom->CommitChangeParams();
	return status;
}

CF_ERROR __EXPORT_TYPE CFLoadSurvParams(const char* lpFileName, CF_SYMPTOM_PARAMS* pParams)
{
	CF_ERROR status = CF_SUCCESS;

	if (lpFileName == NULL || pParams == NULL)
		return CFF_INVALID_PARAM;
	if (pParams->size != sizeof(CF_SYMPTOM_PARAMS))
		return CFF_INVALID_PARAM;
    
	TiXmlDocument doc(lpFileName);
    if (!doc.LoadFile())
		return CFF_CANNOT_LOAD_PARAMS;

   TiXmlHandle hDoc(&doc);
   TiXmlElement* pElem = NULL;
   TiXmlHandle hRoot(0);

    pElem = hDoc.FirstChildElement().Element();
    if (!pElem)
        return CFF_CANNOT_LOAD_PARAMS;
    const char* name = pElem->Value();
    if (strcmp(name, c_lpParamsName) != 0)
        return CFF_CANNOT_LOAD_PARAMS;
//    hRoot=TiXmlHandle(pElem);
//    pElem = hRoot.FirstChild( c_lpParamsName ).Element();
//    if (pElem == NULL)
//        return CFF_CANNOT_LOAD_PARAMS;

	pElem->QueryIntAttribute(c_lpParamWidth, &pParams->source_width);
	pElem->QueryIntAttribute(c_lpParamHeight, &pParams->source_height);
	pElem->QueryIntAttribute(c_lpParamRoiLeft, &pParams->roi_left);
	pElem->QueryIntAttribute(c_lpParamRoiTop, &pParams->roi_top);
	pElem->QueryIntAttribute(c_lpParamRoiWidth, &pParams->roi_width);
	pElem->QueryIntAttribute(c_lpParamRoiHeight, &pParams->roi_height);
	double value = 0;
	pElem->QueryDoubleAttribute(c_lpParamsMinBlobSquare, &value);
	pParams->min_blob_square = (float)value;
	pElem->QueryDoubleAttribute(c_lpParamsMaxBlobSquare, &value);
	pParams->max_blob_square = (float)value;
	pElem->QueryDoubleAttribute(c_lpParamsMinBlobAspect, &value);
	pParams->min_blob_aspect_ratio = (float)value;
	pElem->QueryDoubleAttribute(c_lpParamsMaxBlobAspect, &value);
	pParams->max_blob_aspect_ratio = (float)value;
	pElem->QueryDoubleAttribute(c_lpParamsMinBlobAge, &value);
	pParams->min_blob_age = (float)value;
	pElem->QueryDoubleAttribute(c_lpParamsMinBlobImmovAge, &value);
	pParams->min_blob_age_immov = (float)value;
	pParams->detector = sbdAbsDiffBackground;
	return status;
}

CF_ERROR __EXPORT_TYPE CFSaveSurvParams(const char* lpFileName, CF_SYMPTOM_PARAMS* pParams)
{
	CF_ERROR status = CF_SUCCESS;
	if (lpFileName == NULL || pParams == NULL)
		return CFF_INVALID_PARAM;
	if (pParams->size != sizeof(CF_SYMPTOM_PARAMS))
		return CFF_INVALID_PARAM;
 
	TiXmlDocument doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );
	
    TiXmlElement* settings = new TiXmlElement(c_lpParamsName);
	doc.LinkEndChild(settings);
	if (pParams->source_width > 32 && pParams->source_height > 32)
	{
		settings->SetAttribute(c_lpParamWidth, pParams->source_width);
		settings->SetAttribute(c_lpParamHeight, pParams->source_height);
	}
	else
	{
  	    status = CFF_INVALID_PARAM;
    	goto cleanup;	
	}
	awpImage* tmp = NULL;
	if (awpCreateImage(&tmp, pParams->source_width, 
		pParams->source_height, 1, AWP_BYTE) == AWP_OK)
	{
		awpRect test_rect;
		test_rect.left = pParams->roi_left;
		test_rect.top = pParams->roi_top;
		test_rect.right = test_rect.left + pParams->roi_width;
		test_rect.bottom = test_rect.top + pParams->roi_height;
		if (awpRectInImage(tmp, &test_rect) == AWP_OK)
		{
			settings->SetAttribute(c_lpParamRoiLeft, pParams->roi_left);
			settings->SetAttribute(c_lpParamRoiTop, pParams->roi_top);
			settings->SetAttribute(c_lpParamRoiWidth, pParams->roi_width);
			settings->SetAttribute(c_lpParamRoiHeight, pParams->roi_height);
		}
		else
			status = CFF_INVALID_PARAM;
		awpReleaseImage(&tmp);
		if (status != CF_SUCCESS)
			goto cleanup;	
	}
	else
	{
  	    status = CFCR_NOT_ENOUGH_MEMORY;
    	goto cleanup;	
	}
	//todo: setup true detector
	settings->SetAttribute(c_lpParamsDetector, "dummy");
	if (pParams->min_blob_square > 0 && pParams->max_blob_square > pParams->min_blob_square)
	{
		settings->SetDoubleAttribute(c_lpParamsMinBlobSquare, (double)pParams->min_blob_square);
		settings->SetDoubleAttribute(c_lpParamsMaxBlobSquare, (double)pParams->max_blob_square);
	}
	else
	{
  	    status = CFF_INVALID_PARAM;
    	goto cleanup;	
	}
	if (pParams->min_blob_aspect_ratio > 0 && pParams->max_blob_aspect_ratio > pParams->min_blob_aspect_ratio)
	{
		settings->SetDoubleAttribute(c_lpParamsMinBlobAspect, (double)pParams->min_blob_aspect_ratio);
		settings->SetDoubleAttribute(c_lpParamsMaxBlobAspect, (double)pParams->max_blob_aspect_ratio);
	}
	else
	{
  	    status = CFF_INVALID_PARAM;
    	goto cleanup;	
	}
	if (pParams->min_blob_age > 0)
	{
		settings->SetDoubleAttribute(c_lpParamsMinBlobAge, (double)pParams->min_blob_age);
	}
	else
	{
  	    status = CFF_INVALID_PARAM;
    	goto cleanup;	
	}
	if (pParams->min_blob_age_immov)
	{
		settings->SetDoubleAttribute(c_lpParamsMinBlobImmovAge, (double)pParams->min_blob_age_immov);
	}
	else
	{
  	    status = CFF_INVALID_PARAM;
    	goto cleanup;	
	}

cleanup:
	if (status == CF_SUCCESS)
	{
		if (!doc.SaveFile(lpFileName))
			status = CFF_CANNOT_SAVE_PARAMS;
	}
	return status;
}


/* signals
*/
CF_ERROR __EXPORT_TYPE  CFSetNewBlobSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc)
{
	CF_ERROR status = CF_SUCCESS;

	_GET_SYMPTOM_

	_CF_Symprom_Events* events = (_CF_Symprom_Events*)symptom->GetUserData();
	events->m_NewBlobSignal = SignalFunc;

	return status;
}

CF_ERROR __EXPORT_TYPE  CFSetLostBlobSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc)
{
	CF_ERROR status = CF_SUCCESS;
	_GET_SYMPTOM_

	_CF_Symprom_Events* events = (_CF_Symprom_Events*)symptom->GetUserData();
	events->m_LostBlobSignal = SignalFunc;

	return status;
}

CF_ERROR __EXPORT_TYPE  CFSetBlobPosChagedSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc)
{
	CF_ERROR status = CF_SUCCESS;
	_GET_SYMPTOM_

	_CF_Symprom_Events* events = (_CF_Symprom_Events*)symptom->GetUserData();
	events->m_BlobPosChangedSignal = SignalFunc;

	return status;
}

CF_ERROR __EXPORT_TYPE  CFSetBlobImmoveableSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc)
{
	CF_ERROR status = CF_SUCCESS;

	_GET_SYMPTOM_

	_CF_Symprom_Events* events = (_CF_Symprom_Events*)symptom->GetUserData();
	events->m_BlobImmovableSignal = SignalFunc;
	
	return status;
}

CF_ERROR __EXPORT_TYPE  CFSetBlobPresentAlarmSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc)
{
	CF_ERROR status = CF_SUCCESS;

	_GET_SYMPTOM_

	_CF_Symprom_Events* events = (_CF_Symprom_Events*)symptom->GetUserData();
	events->m_BlobAgeSignal = SignalFunc;

	return status;
}

CF_ERROR __EXPORT_TYPE  CFSetBlobImmoveabeAlarmSignal(CF_SYMPTOM hSymptom, CF_SYMPTOM_SIGNAL SignalFunc)
{
	CF_ERROR status = CF_SUCCESS;
	_GET_SYMPTOM_

	_CF_Symprom_Events* events = (_CF_Symprom_Events*)symptom->GetUserData();
	events->m_BlobAgeImmovableSignal = SignalFunc;

	return status;
}
