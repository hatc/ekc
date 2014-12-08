/*
//     Chaos Face SDK version 1.8     
//	   Copyright(c) 2008-2010 CCT gmbH, all rights reserved.
//	   Third party copyrights are property of their respective owners.
//     cffacedetect.cpp 
//     version 1.0.0	
*/
#include "_cffacedetect.h"
CF_ERROR __EXPORT_TYPE CFCreateLocateFace( CF_LOCATE_FACE *hLocateFace, char* path )
{
	CF_ERROR status = CF_SUCCESS;
	THLCLocateFace* lf = NULL;
	lf = new THLCLocateFace();
	if( lf == NULL )
	{
		status = CFCR_NOT_ENOUGH_MEMORY;
		return status;
	}
	if (!lf->LoadEngine(path))
	{
		status = CFF_CANNOT_READ_PARAM;
		return status;
	}

//     HLScaleScanner* Scanner = NULL;
//     Scanner = new HLScaleScanner();
//     Scanner->SetDelta(2);
//     Scanner->SetMaxEyeDistance(512);
//     Scanner->SetMinEyeDistance(16);
//     lf->SetScanner(Scanner);
//     TCSDetector* d = new TCSDetector();

//     d->Load(path);
//     lf->SetDetector(d);

	 lf->SetNeedFilter(true);

	 *hLocateFace = lf;

#ifdef _DEBUG
//    lf->SaveEngine("c:\\car_engine1.xml");
//    lf->LoadEngine("c:\\car_engine1.xml");
#endif
	return status;
}

CF_ERROR __EXPORT_TYPE CFDestroyLocateFace( CF_LOCATE_FACE *hLocateFace )
{
	CF_ERROR status = CF_SUCCESS;
	 THLCLocateFace* lf = (THLCLocateFace*)*hLocateFace;
	if( lf == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}

	HLScanner* Scanner = lf->GetScanner();
	if( Scanner != NULL )
		delete Scanner;

	ILFObjectDetector* Detector = lf->GetDetector();
	if( Detector != NULL )
		delete Detector;

	delete lf;
	*hLocateFace = NULL;
	return status;
}

CF_ERROR __EXPORT_TYPE CFGetLocateFaceParams(CF_LOCATE_FACE hLocateFace, CF_LOCATE_FACE_PARAMS* pParams)
{
	CF_ERROR status = CF_SUCCESS;
	// проверим входные параметры
	if (pParams == NULL)
	{
		return CFF_INVALID_PARAM;
	}
	THLCLocateFace* lf = (THLCLocateFace*)hLocateFace;
	if( lf == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	pParams->use_color = lf->GetUseColor();
	pParams->fast_computing = lf->GetFastComputing();
	HLScanner* s = lf->GetScanner();
	if (s == NULL)
	{
		memset(s, 0, sizeof (CF_LOCATE_FACE_PARAMS));
		return  CFF_INVALID_HANDLE;
	}
	int v = s->GetMinEyeDistance();
	switch (v)
	{
		case 8:
			pParams->face_sizes = CFSMALL;
		break;
		case 16:
			pParams->face_sizes = CFMEDIUM;
		break;
		case 32:
			pParams->face_sizes = CFBIG;
		break;
	}
	return status;
}

CF_ERROR __EXPORT_TYPE CFSetLocateFaceParams(CF_LOCATE_FACE hLocateFace, CF_LOCATE_FACE_PARAMS* pParams)
{
	CF_ERROR status = CF_SUCCESS;
	// проверим входные параметры
	if (pParams == NULL)
	{
		return CFF_INVALID_PARAM;
	}
	THLCLocateFace* lf = (THLCLocateFace*)hLocateFace;
	if( lf == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	HLScanner* s = lf->GetScanner();
	if (s == NULL)
		return  CFF_INVALID_HANDLE;

	lf->SetUseColor(pParams->use_color);
	//lf->SetFastComputing(pParams->fast_computing);
	lf->GetDetector()->SetUseTilt(pParams->fast_computing);
	switch (pParams->face_sizes)
	{
		case CFSMALL:
			s->SetMinEyeDistance(8);
		break;
		case CFMEDIUM:
			s->SetMinEyeDistance(16);
		break;
		case CFBIG:
			s->SetMinEyeDistance(32);
		break;
	}
	return status;
}

CF_ERROR __EXPORT_TYPE CFFindFaces( CF_LOCATE_FACE hLocateFace, CF_IMAGE hImage )
{
	CF_ERROR status = CF_SUCCESS;
	 THLCLocateFace* lf = (THLCLocateFace*)hLocateFace;
	if( lf == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	TLFImage* image = (TLFImage*)hImage;
	if(image == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}

	awpImage* tmp = NULL;
	try
	{
		if(!lf->SetSourceImage(image->GetImage(), true))
			status = CFF_FINDFACE_FAILED;
	}
	catch(...)
	{
		status = CFCR_NOT_ENOUGH_MEMORY;
		if( tmp != NULL )
			awpReleaseImage(&tmp);
		return status;
	}
	if( tmp != NULL )
		awpReleaseImage(&tmp);
	return status;
}
static CF_FACE_TILT _GetTilt(TROI* proi)
{
	if (proi == NULL)
		return CFFRONT;
	if (proi->err[4] == 2)
		return CFLEFT;
	if (proi->err[4] = 3)
		return CFRIGHT;
	return CFFRONT;
}
CF_ERROR  CFGetFaces( CF_LOCATE_FACE hLocateFace, CF_FACE_POS** Faces )
{
	CF_ERROR status = CF_SUCCESS;
	THLCLocateFace* lf = (THLCLocateFace*)hLocateFace;
	if( lf == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	TROI* faces = lf->GetFaces();
	int count = lf->GetFacesCount();
	*Faces = (CF_FACE_POS*)malloc( count*sizeof(CF_FACE_POS) );
	int i;
	for( i = 0; i < count; ++i )
	{
		(*Faces)[i].ID = faces[i].index;
		(*Faces)[i].LeftEye.X = faces[i].p.X;
		(*Faces)[i].LeftEye.Y = faces[i].p.Y;
		(*Faces)[i].RightEye.X = faces[i].p1.X;
		(*Faces)[i].RightEye.Y = faces[i].p1.Y;
		memcpy( (*Faces)[i].Errors, faces[i].err, sizeof(faces[i].err) );
		(*Faces)[i].Tilt = _GetTilt(&faces[i]);
	}
	return status;
}

CF_ERROR __EXPORT_TYPE CFGetFacesCount( CF_LOCATE_FACE hLocateFace, int* count )
{
	CF_ERROR status = CF_SUCCESS;
	 THLCLocateFace* lf = (THLCLocateFace*)hLocateFace;
	if( lf == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	*count = lf->GetFacesCount();
	return status;
}
/*
	Drawing section. 
	drawing of the location of persons

*/


static void _cfDrawFaceCircle(awpImage* pImage, TROI* pRoi, float radius, awpColor cc)
{
	awpPoint c = pRoi->GetRoiCenter();
	float d = (float) pRoi->EyeDistance();
	d *= radius;
	AWPWORD w = (AWPWORD)d;
	
	if (pImage->bChannels == 1)
	{
		//processing of a single-channel image
		double v = (cc.bBlue + cc.bGreen + cc.bRed) / 3.;	
		awpDrawEllipse(pImage, c, w, w, 0, 0, v, 1);  
	}
	else
	{
		//processing of a rgb-type image
		awpDrawCEllipse(pImage, c, w,w, 0, cc.bRed, cc.bGreen, cc.bBlue, 1);
	}
}

static void _cfDrawFaceRect(awpImage* pImage, TROI* pRoi, float radius, awpColor cc)
{
	awpPoint c = pRoi->GetRoiCenter();
	float d = (float) pRoi->EyeDistance();
	d *= radius;
	awpRect r;
	r.left = AWPSHORT(c.X - d); 
	r.right = AWPSHORT(c.X + d); 
	r.bottom = AWPSHORT(c.Y + d);
	r.top = AWPSHORT(c.Y - d);
	if (pImage->bChannels == 1)
	{
		//processing of a single-channel image
		double v = (cc.bBlue + cc.bGreen + cc.bRed) / 3.;	
		awpDrawRect(pImage, &r, 0, v, 0);
	}
	else
	{
		//processing of a rgb-type image
		awpDrawCRect(pImage, &r, cc.bRed, cc.bGreen, cc.bBlue, 1);
	}
}

#define _DRAW_CORNER_ \
	if (pImage->bChannels == 1)\
	{\
		double v = (cc.bBlue + cc.bGreen + cc.bRed) / 3.;	\
		awpDrawLine(pImage, p1, p2, 1, v, 1);\
		awpDrawLine(pImage, p1, p3, 1, v, 1);\
	}\
	else\
	{\
		awpDrawCLine(pImage, p1, p2, cc.bRed, cc.bGreen, cc.bBlue, 1);\
		awpDrawCLine(pImage, p1, p3, cc.bRed, cc.bGreen, cc.bBlue, 1);\
	}\

static void _cfDrawFaceCorners(awpImage* pImage, TROI* pRoi, float radius, awpColor cc)
{
	awpPoint c = pRoi->GetRoiCenter();
	float d = (float) pRoi->EyeDistance();
	d *= radius;
	awpRect r; 

	r.left   = AWPSHORT(c.X - d); 
	r.right  = AWPSHORT(c.X + d); 
	r.bottom = AWPSHORT(c.Y + d);
	r.top	 = AWPSHORT(c.Y - d);
	
	d /=4;

	awpPoint p1, p2 , p3;
	// draw left-top corner 
	p1.X = AWPSHORT(r.left);	p1.Y = AWPSHORT(r.top);
	p2.X = AWPSHORT(r.left + d);p2.Y = AWPSHORT(r.top);
	p3.X = AWPSHORT(r.left);	p3.Y = AWPSHORT(r.top + d);
	// drawing 
	_DRAW_CORNER_ 

	p1.X = AWPSHORT(r.right);	 p1.Y = AWPSHORT(r.top);
	p2.X = AWPSHORT(r.right - d);p2.Y = AWPSHORT(r.top);
	p3.X = AWPSHORT(r.right);	 p3.Y = AWPSHORT(r.top + d);
	// drawing 
	_DRAW_CORNER_ 

	// draw right-bottom corner 
	p1.X = AWPSHORT(r.right);	 p1.Y = AWPSHORT(r.bottom);
	p2.X = AWPSHORT(r.right - d);p2.Y = AWPSHORT(r.bottom);
	p3.X = AWPSHORT(r.right);	 p3.Y = AWPSHORT(r.bottom - d);
	// drawing 
	_DRAW_CORNER_ 

	// draw left-bottom corner 
	p1.X = AWPSHORT(r.left);	p1.Y = AWPSHORT(r.bottom);
	p2.X = AWPSHORT(r.left + d);p2.Y = AWPSHORT(r.bottom);
	p3.X = AWPSHORT(r.left);	p3.Y = AWPSHORT(r.bottom - d);
	// drawing 
	_DRAW_CORNER_ 
}

static void _cfDrawFaceEllipce(awpImage* pImage, TROI* pRoi, float radius, awpColor cc)
{
	awpPoint c = pRoi->GetRoiCenter();
	float d = (float) pRoi->EyeDistance();
	d *= radius;

	AWPWORD width;
	AWPWORD height;
	AWPDOUBLE angle;
	width  = AWPSHORT(d);
	height = AWPSHORT(width*1.2); 
	
	angle = -pRoi->Orientation();

	if (pImage->bChannels == 1)
	{
		//processing of a single-channel image
		double v = (cc.bBlue + cc.bGreen + cc.bRed) / 3.;	
		awpDrawEllipse(pImage, c, width, height, angle, 1, v, 1);
	}
	else
	{
		//processing of a rgb-type image
		awpDrawCEllipse(pImage, c, width, height,angle, cc.bRed, cc.bGreen, cc.bBlue,  1);
	}
 }

/*
	CFDrawFacesPos 

*/
CF_ERROR __EXPORT_TYPE CFDrawFacesPos( CF_LOCATE_FACE hLocateFace, CF_IMAGE hImage, CF_COLOR color, CF_SHAPE shape, float radius,  bool draw_eye_pos)
{
	CF_ERROR status = CF_SUCCESS;
	THLCLocateFace* lf = (THLCLocateFace*)hLocateFace;
	if( lf == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	TLFImage* image = (TLFImage*)hImage;
	if(image == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	TROI* faces = lf->GetFaces();
	int count  = lf->GetFacesCount();

	int i; awpColor lcolor;
	switch( color )
	{
		case CFWHITE:
			lcolor.bRed = 255; lcolor.bGreen = 255; lcolor.bBlue = 255;
		break;
		case CFBLACK:
			lcolor.bRed = 0; lcolor.bGreen = 0; lcolor.bBlue = 0;
		break;
		case CFRED:
			lcolor.bRed = 0; lcolor.bGreen = 0; lcolor.bBlue = 255;
		break;
		case CFGREEN:
			lcolor.bRed = 0; lcolor.bGreen = 255; lcolor.bBlue = 0;
		break;
		case CFBLUE:
			lcolor.bRed = 255; lcolor.bGreen = 0; lcolor.bBlue = 0;
		break;
		case CFYELLOW:
			lcolor.bRed = 0; lcolor.bGreen = 255; lcolor.bBlue = 255;
		break;
	}

	for( i = 0; i < count; ++i )
	{

		switch (shape)
		{
		case CFCIRCLE:
			_cfDrawFaceCircle(image->GetImage(), &faces[i], radius, lcolor);
			break;
		case CFRECT:
			_cfDrawFaceRect(image->GetImage(), &faces[i], radius, lcolor);
			break;
		case CFCORNERS:
			_cfDrawFaceCorners(image->GetImage(), &faces[i], radius, lcolor);
			break;
		case CFELLIPSE:
			_cfDrawFaceEllipce(image->GetImage(), &faces[i], radius, lcolor);
			break;
		}
		if (draw_eye_pos)
			faces[i].DrawROIPoints(image->GetImage(), 1, lcolor);
	}
	return status;
}

CF_ERROR  CFDrawFacePos(CF_IMAGE hImage, CF_FACE_POS* ppos, CF_COLOR color)
{
	CF_ERROR status = CF_SUCCESS;
	TLFImage* image = (TLFImage*)hImage;
	if(image == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	awpColor lcolor;
	switch( color )
	{
		case CFWHITE:
			lcolor.bRed = 255; lcolor.bGreen = 255; lcolor.bBlue = 255;
		break;
		case CFBLACK:
			lcolor.bRed = 0; lcolor.bGreen = 0; lcolor.bBlue = 0;
		break;
		case CFRED:
			lcolor.bRed = 0; lcolor.bGreen = 0; lcolor.bBlue = 255;
		break;
		case CFGREEN:
			lcolor.bRed = 0; lcolor.bGreen = 255; lcolor.bBlue = 0;
		break;
		case CFBLUE:
			lcolor.bRed = 255; lcolor.bGreen = 0; lcolor.bBlue = 0;
		break;
		case CFYELLOW:
			lcolor.bRed = 0; lcolor.bGreen = 255; lcolor.bBlue = 255;
		break;
	}
	awpPoint p1;
	awpPoint p2;

	p1.X = ppos->LeftEye.X;
	p1.Y = ppos->LeftEye.Y;
	p2.X = ppos->RightEye.X;
	p2.Y = ppos->RightEye.Y;

	TROI roi;
	roi.TROI(p1,p2);
	awpRect r_in = roi.r, r_out = roi.r;

	r_in.left += 1;
	r_in.right -= 1;
	r_in.top += 1;
	r_in.bottom -= 1;

	r_out.left -= 1;
	r_out.right += 1;
	r_out.top -= 1;
	r_out.bottom += 1;
	roi.DrawAOIRect(image->GetImage(), 1, lcolor);
	awpDrawCRect(image->GetImage(), &r_in, lcolor.bRed, lcolor.bGreen, lcolor.bBlue, 0);
	awpDrawCRect(image->GetImage(), &r_out, lcolor.bRed, lcolor.bGreen, lcolor.bBlue, 0);

	return status;
}


CF_ERROR __EXPORT_TYPE CFSetProgress( CF_LOCATE_FACE hLocateFace, CF_PROGRESS ProgressFunc)
{
	CF_ERROR status = CF_SUCCESS;
	 THLCLocateFace* lf = (THLCLocateFace*)hLocateFace;
	if( lf == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	lf->SetProgress((THLProgressEvent)ProgressFunc);
	return status;
}

CF_ERROR  CFFreeFacePos( CF_FACE_POS* Face )
{
	CF_ERROR status = CF_SUCCESS;
    free( Face );
    return status;
}

CF_ERROR  CFGetFaceImage1(CF_IMAGE hImage, CF_FACE_POS* ppos, CF_IMAGE* hFaceImage)
{
    CF_ERROR status = CF_SUCCESS;

    // преобразование типов.
    TLFImage* image = (TLFImage*)hImage;
    if (image == NULL)
    {
        status = CFF_INVALID_HANDLE;
        return status;
	}

    // проверка входящего изображения.
    if (awpCheckImage(image->GetImage()) != AWP_OK)
    {
        status = CFF_INVALID_HANDLE;
        return status;
    }

    // проверка местоположения лица и входящего аргрумента
    if (ppos == NULL || hFaceImage == NULL)
    {
        status = CFF_INVALID_PARAM;
		return status;
    }
    // преобразование   CF_FACE_POS  в ROI
    awpPoint p, p1;
    p.X = ppos->LeftEye.X;
    p.Y = ppos->LeftEye.Y;
    p1.X = ppos->RightEye.X;
    p1.Y = ppos->RightEye.Y;

    TROI Roi;
    Roi.TROI(p,p1);
    // проверка Roi
    if (!Roi.IsROIInImage(image->GetImage()))
    {
        status = CFF_INVALID_PARAM;
		return status;
    }
    // создаем изображение
    TLFImage* fragment = NULL;
    fragment = new TLFImage();
    if (fragment == NULL)
    {
        status = CFCR_NOT_ENOUGH_MEMORY;
        return status;
    }

    // вырезаем фрагмент изображения.
    awpImage* pFragment = NULL;
    //pFragment = Roi.GetRoiImageSource(image->GetImage());
    //pFragment = GetAOISource(image->GetImage(), p,  p1);
    if (awpCopyRect(image->GetImage(), &pFragment, &Roi.r) != AWP_OK)
	{
       status = CFCR_NOT_ENOUGH_MEMORY;
       return status;
    }
    if (pFragment == NULL)
    {
       status = CFCR_NOT_ENOUGH_MEMORY;
       delete fragment;
       return status;
    }
    // устанавливаем изображение в дескриптор
    fragment->SetImage(pFragment);
    *hFaceImage = fragment;
	awpReleaseImage(&pFragment);
    return status;
}

CF_ERROR __EXPORT_TYPE CFGetFacePos(CF_LOCATE_FACE hLocateFace, int index, int* px_pos, int* py_pos, int* p_radius)
{
    CF_ERROR status = CF_SUCCESS;
	THLCLocateFace* lf = (THLCLocateFace*)hLocateFace;
	if( lf == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	if (lf->GetFacesCount() < index || index < 0)
	{
		status = CFF_INVALID_PARAM;
		return status;
	}

	if (px_pos == NULL || py_pos == NULL || p_radius == NULL)
	{
		status = CFF_INVALID_PARAM;
		return status;
	}

	TROI* faces = lf->GetFaces();
	if (faces == NULL)
	{
		*px_pos = -1;
		*py_pos = -1;
		*p_radius = -1;

		// todo: set status to warning 
		return status;
	}
	// 
	*px_pos = faces[index].GetRoiCenter().X;
	*py_pos = faces[index].GetRoiCenter().Y;
	*p_radius = (int)faces[index].EyeDistance();
    return status;
}

CF_ERROR __EXPORT_TYPE CFGetEyePos(CF_LOCATE_FACE hLocateFace, int index, CF_POINT* pLe, CF_POINT* pRe)
{
    CF_ERROR status = CF_SUCCESS;
	THLCLocateFace* lf = (THLCLocateFace*)hLocateFace;
	if( lf == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	if (lf->GetFacesCount() < index || index < 0)
	{
		status = CFF_INVALID_PARAM;
		return status;
	}

	if (pLe == NULL || pRe == NULL)
	{
		status = CFF_INVALID_PARAM;
		return status;
	}

	TROI* faces = lf->GetFaces();
	if (faces == NULL)
	{
		pLe->X = -1;	
		pLe->Y = -1;	
		pRe->X = -1;	
		pRe->Y = -1;	
		// todo: set status to warning 
		return status;
	}
	// 
	pLe->X = faces[index].p.X;
	pLe->Y = faces[index].p.Y;
	pRe->X = faces[index].p1.X;
	pRe->Y = faces[index].p1.Y;

	return status;
}


CF_ERROR __EXPORT_TYPE CFGetFaceImage(CF_LOCATE_FACE hLocateFace, CF_IMAGE hImage, int index, CF_IMAGE* hFaceImage)
{
    CF_ERROR status = CF_SUCCESS;

    // преобразование типов.
    TLFImage* image = (TLFImage*)hImage;
    if (image == NULL)
    {
        status = CFF_INVALID_HANDLE;
        return status;
	}
    // проверка входящего изображения.
    if (awpCheckImage(image->GetImage()) != AWP_OK)
    {
        status = CFF_INVALID_HANDLE;
        return status;
    }
	THLCLocateFace* lf = (THLCLocateFace*)hLocateFace;
	if( lf == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	if (lf->GetFacesCount() < index || index < 0)
	{
		status = CFF_INVALID_PARAM;
		return status;
	}
	//------
	TROI* faces = lf->GetFaces();
	
	// проверка местоположения лица и входящего аргрумента
    if (hFaceImage == NULL)
    {
        status = CFF_INVALID_PARAM;
		return status;
    }
    // проверка Roi
    if (!faces[index].IsROIInImage(image->GetImage()))
    {
        status = CFF_INVALID_PARAM;
		return status;
    }
    // создаем изображение
    TLFImage* fragment = NULL;
    fragment = new TLFImage();
    if (fragment == NULL)
    {
        status = CFCR_NOT_ENOUGH_MEMORY;
        return status;
    }

    // вырезаем фрагмент изображения.
    awpImage* pFragment = NULL;
    if (awpCopyRect(image->GetImage(), &pFragment, &faces[index].r) != AWP_OK)
	{
       status = CFCR_NOT_ENOUGH_MEMORY;
       return status;
    }
    if (pFragment == NULL)
    {
       status = CFCR_NOT_ENOUGH_MEMORY;
       delete fragment;
       return status;
    }
    // устанавливаем изображение в дескриптор
    fragment->SetImage(pFragment);
    *hFaceImage = fragment;
	awpReleaseImage(&pFragment);
    return status;
}