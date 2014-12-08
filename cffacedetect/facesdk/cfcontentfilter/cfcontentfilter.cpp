// ֳכאגםûי DLL-פאיכ.

#include "_cfcontentfilter.h"
#include "cfcontentfilter.h"
#include "ccontentfilter.h"
#include "tinyxml.h"
#include "LF.h"

#define CF_ERR_EXIT	goto CLEANUP;


#include <tchar.h>
CF_ERROR __EXPORT_TYPE CFCreateContentFilter (CF_CONTENTFILTER* hContentFilter, char* path )
{
	CF_ERROR status = CF_SUCCESS;
	string dpath(path);
	string dir = dpath.substr( 0, dpath.rfind( "\\" )+1 );

	SetCurrentDirectory( dir.c_str() );

	_CFContentFilter* CFilter = NULL;
	CFilter = new _CFContentFilter();
	if( CFilter == NULL )
	{
		status = CFCR_NOT_ENOUGH_MEMORY;
		return status;
	}
	try
	{
		CFilter->pFilter = new CContentFilter();
	}
	catch(CF_ERROR err)
	{
		status = err;
		return status;
	}
	CFilter->size = sizeof(CContentFilter);
	
	TiXmlDocument config(path);
	if(!config.LoadFile())
	{
		status = CFF_CANNOT_READ_PARAM;
		return status;
	}

	TiXmlHandle hDoc(&config);
	TiXmlHandle hRoot(0);
	TiXmlElement* pElem = NULL;
	TiXmlElement* pColorFilterNode = NULL;
	pElem = hDoc.FirstChildElement().Element();
    if (!pElem)
        return false;
    const char* name = pElem->Value();
    if (strcmp(name, "ContentFilterSettings") != 0)
        return false;
    hRoot=TiXmlHandle(pElem);
	pColorFilterNode = hRoot.FirstChild( "ColorFilter" ).Element();
	if (pColorFilterNode == NULL)
	{
		status = CFF_CANNOT_LOAD_PARAMS;
		return status;
	}

	const char* pName = pColorFilterNode->Attribute("presetFileName");
		
	strcpy (CFilter->pFilter->clrOptions.presetFileName, pName);

	pColorFilterNode->QueryIntAttribute("filterType", &CFilter->pFilter->clrOptions.filter);
	pColorFilterNode->QueryIntAttribute("maxValue", &CFilter->pFilter->clrOptions.maxValue);
	pColorFilterNode->QueryIntAttribute("minValue", &CFilter->pFilter->clrOptions.minValue);
	pColorFilterNode->QueryIntAttribute("Thr", &CFilter->pFilter->clrOptions.cThr);

	TiXmlElement* pFeatureNode = hRoot.FirstChild( "GetFeatures" ).Element();
	if (pFeatureNode == NULL)
	{
		status = CFF_CANNOT_LOAD_PARAMS;
		return status;
	}
	
	pFeatureNode->QueryDoubleAttribute("minSquare", &CFilter->pFilter->ftsOptions.s_min);
	pFeatureNode->QueryIntAttribute("binThreshold", &CFilter->pFilter->ftsOptions.getStrokeThr);
	pFeatureNode->QueryDoubleAttribute("findFaceThreshold", &CFilter->pFilter->ftsOptions.fRecThr);
	pFeatureNode->QueryDoubleAttribute("probThr", &CFilter->pFilter->ftsOptions.pThr);

	TiXmlElement* pDecisionNode = hRoot.FirstChild( "Decision" ).Element();
	if (pDecisionNode == NULL)
	{
		status = CFF_CANNOT_LOAD_PARAMS;
		return status;
	}
	
	pDecisionNode->QueryDoubleAttribute("recThr", &CFilter->pFilter->slvOptions.RecThreshold);
	pDecisionNode->QueryDoubleAttribute("k1", &CFilter->pFilter->slvOptions.k1);
	pDecisionNode->QueryDoubleAttribute("k2", &CFilter->pFilter->slvOptions.k2);
	pDecisionNode->QueryDoubleAttribute("k3", &CFilter->pFilter->slvOptions.k3);

	double tmp_e[] = {1.31660387513455, 0.0315385578145297, 0.0280115573646033,
                0.0123623200714463, 0.0196157618227273, 0.0214929632523185,
                19419.4231996608, 0.000864830422774683, 15.9103951872571,
                0.00432610382009158, 0.00313422523128664, 0.00514583487859667,
                0.00808442850699732, 0.0109912435289376, 642.33378684934, 0.000990851444368826,
                22.2105474038791, 0.00397617588971297, 0.00223121435816136,
                0.00507785489735338, 0.0091309893317468, -0.0525945028202151, 393.895815486356,
                0.00125466081995014, 30.0927066121651, 0.00242850258897393,
                0.00200210205672216, 0.00516817320304062, 0.00885542515761386, -0.141840001636753,
                309.025330052874, 0.00128281670526892, 42.7734916834411,
                0.00231227260184213, 0.00123852137259855, 0.003850262583139, 0.0104900504877214,
                -0.296031222883092, 276.960450366196, 0.00121464638793114, 47.7108972816193};

    memcpy(CFilter->pFilter->slvOptions.x_mean_e, tmp_e, sizeof(tmp_e));
    
    CFilter->pFilter->slvOptions.x_mean_a[0] = 0;
    CFilter->pFilter->slvOptions.x_mean_a[1] = 2.76811265046559;
    CFilter->pFilter->slvOptions.x_mean_a[2] = 8.75199784701053;
    CFilter->pFilter->slvOptions.x_mean_a[3] = 7.49064757579233;
    CFilter->pFilter->slvOptions.x_mean_a[4] = 0;
    CFilter->pFilter->slvOptions.x_mean_a[5] = 0;
	
	
	*hContentFilter = CFilter;
	
	return status;
}

CF_ERROR __EXPORT_TYPE CFDestroyContentFilter(CF_CONTENTFILTER* hContentFilter)
{
	CF_ERROR status = CF_SUCCESS;
	_CFContentFilter* CFilter = *(_CFContentFilter**)hContentFilter;
	if( CFilter == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	if(CFilter->pFilter != NULL)
	{
		delete CFilter->pFilter;
		delete CFilter;
	}
	*hContentFilter = NULL;
	return status;
}

/*Content detection*/
CF_ERROR __EXPORT_TYPE CFDoFilter(CF_IMAGE hImage, CF_CONTENTFILTER hContentFilter, int* result)
{
	CF_ERROR status = CF_SUCCESS;
	_CFContentFilter* CFilter = (_CFContentFilter*)hContentFilter;
	if( CFilter == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	if( hImage == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	
	if( result == NULL)
	{
		status = CFF_INVALID_PARAM;
		return status;
	}
	
	awpImage* maskImage = NULL;
    awpImage* maskImage1 = NULL;
    awpImage* maskSourceImage = NULL;
    awpPoint* ellipseCenter = 0;
    float *ellipseAngle = NULL, *ellipseLen = NULL, *ellipseWidth = NULL;
	double* features = 0;
    awpStrokeObj* BigStrokes = NULL;
    int numObj = 0;
	awpRect* fRect = NULL, *interestRect = NULL;
    int numFaces = 0;
	double distance = 0;
	try
	{
		CFilter->pFilter->iColorFilter(&maskImage, &maskImage1, &CFilter->pFilter->clrOptions, hImage);
		awpImage* sourceImage = ((TLFImage*)(hImage))->GetImage();
		awpCopyImageMask( sourceImage, maskImage, &maskSourceImage, 25 );
		
		int res = CFilter->pFilter->iGetFeatures(&features, &CFilter->pFilter->ftsOptions, maskImage, maskSourceImage,
                                maskImage1, hImage, &BigStrokes, &numObj, &ellipseCenter,
                                &ellipseAngle, &ellipseLen, &ellipseWidth, &interestRect, &fRect, &numFaces);
		switch(res)
        {
            case -1:
                *result = NOTCONTENT;
				CF_ERR_EXIT;
            break;
            case -3:
                status = CFF_CANNOT_GET_STROKES;
				CF_ERR_EXIT;
            break;
        }

		res = CFilter->pFilter->iSolve( &distance, &CFilter->pFilter->slvOptions, features );
		switch(res)
        {
            case 0:
                *result = NOTCONTENT;
				CF_ERR_EXIT;
            break;
            case 1:
                *result = CONTENT;
				CF_ERR_EXIT;
            break;
			case 2:
                *result = DONTKNOW;
				CF_ERR_EXIT;
            break;
        }
	}
	catch(CF_ERROR err)
	{
		status = err;
		CF_ERR_EXIT;
	}

CLEANUP:
	if( maskImage != NULL )
		awpReleaseImage(&maskImage);
    if( maskImage1 != NULL )
		awpReleaseImage(&maskImage1);
    if( maskSourceImage != NULL )
		awpReleaseImage(&maskSourceImage);
    if (BigStrokes != NULL)
        awpFreeStrokes(numObj, &BigStrokes);

    if (fRect != NULL)
        free (fRect);

    if (interestRect != NULL)
        free (interestRect);

    if (features != NULL)
        free(features);

    if ( ellipseLen != NULL )
        free ( ellipseLen );

    if ( ellipseWidth != NULL )
        free ( ellipseWidth );

    if ( ellipseAngle != NULL )
        free ( ellipseAngle );

    if ( ellipseCenter != NULL )
        free ( ellipseCenter );

	return status;
}

/*Content filter options*/
CF_ERROR __EXPORT_TYPE CFGetContentFilterOptions(CF_CONTENTFILTER hContentFilter, CF_CONTENT_OPTIONS* pOptions)
{
	CF_ERROR status = CF_SUCCESS;
	_CFContentFilter* CFilter = (_CFContentFilter*)hContentFilter;
	if( CFilter == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	if( pOptions == NULL)
	{
		status = CFF_INVALID_PARAM;
		return status;
	}
	
	(*pOptions).k1 = CFilter->pFilter->slvOptions.k1;
	(*pOptions).k2 = CFilter->pFilter->slvOptions.k2;
	(*pOptions).k3 = CFilter->pFilter->slvOptions.k3;

	return status;
}

CF_ERROR __EXPORT_TYPE CFSetContentFilterOptions(CF_CONTENTFILTER hContentFilter, CF_CONTENT_OPTIONS* pOptions, bool save2xml)
{
	CF_ERROR status = CF_SUCCESS;
	_CFContentFilter* CFilter = (_CFContentFilter*)hContentFilter;
	if( CFilter == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	if( pOptions == NULL)
	{
		status = CFF_INVALID_PARAM;
		return status;
	}


	CFilter->pFilter->slvOptions.k1 = (*pOptions).k1;
	CFilter->pFilter->slvOptions.k2 = (*pOptions).k2;
	CFilter->pFilter->slvOptions.k3 = (*pOptions).k3;

	if(save2xml)
	{
		TiXmlDocument config;

		TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
		config.LinkEndChild( decl );
		
		TiXmlComment* comment = new TiXmlComment();
		string s = " ContentFilter parameters ";
		comment->SetValue(s.c_str());  
		config.LinkEndChild( comment );

		TiXmlElement* pColorFilterNode = new TiXmlElement( "ColorFilter" );
		config.LinkEndChild( pColorFilterNode );
		
		pColorFilterNode->SetAttribute("presetFileName", (const char*)CFilter->pFilter->clrOptions.presetFileName );
		
		pColorFilterNode->SetAttribute("filterType", CFilter->pFilter->clrOptions.filter);
		pColorFilterNode->SetAttribute("maxValue", CFilter->pFilter->clrOptions.maxValue);
		pColorFilterNode->SetAttribute("minValue", CFilter->pFilter->clrOptions.minValue);
		pColorFilterNode->SetAttribute("Thr", CFilter->pFilter->clrOptions.cThr);

		TiXmlElement* pFeatureNode = new TiXmlElement( "GetFeatures" );
		config.LinkEndChild( pFeatureNode );
		
		pFeatureNode->SetDoubleAttribute("minSquare", CFilter->pFilter->ftsOptions.s_min);
		pFeatureNode->SetAttribute("binThreshold", CFilter->pFilter->ftsOptions.getStrokeThr);
		pFeatureNode->SetDoubleAttribute("findFaceThreshold", CFilter->pFilter->ftsOptions.fRecThr);
		pFeatureNode->SetDoubleAttribute("probThr", CFilter->pFilter->ftsOptions.pThr);

		TiXmlElement* pDecisionNode = new TiXmlElement( "Decision" );
		config.LinkEndChild( pDecisionNode );
		
		pDecisionNode->SetDoubleAttribute("recThr", CFilter->pFilter->slvOptions.RecThreshold);
		pDecisionNode->SetDoubleAttribute("k1", CFilter->pFilter->slvOptions.k1);
		pDecisionNode->SetDoubleAttribute("k2", CFilter->pFilter->slvOptions.k2);
		pDecisionNode->SetDoubleAttribute("k3", CFilter->pFilter->slvOptions.k3);

		config.SaveFile( "ContentFilter.xml" );
		
	}

	return status;
}





