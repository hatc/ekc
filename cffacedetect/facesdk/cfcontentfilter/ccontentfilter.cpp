#include <math.h>
#include "cv.h"
#include "LF.h"
#include "ccontentfilter.h"
#include "_cfcontentfilter.h"


template< class T >
static inline int __cdecl compare( const void* a1, const void* a2 )
{
	return *(const T*)a1 < *(const T*)a2 ?
                 -1 :
                 ( *(const T*)a1 > *(const T*)a2
                    ? 1: 0);
}

static int __cdecl compareSqPair( const void* a1, const void* a2 )
{
	return ((sqPair*)a1)->square > ((sqPair*)a2)->square ?
                 -1 :
                 ( ((sqPair*)a1)->square < ((sqPair*)a2)->square
                    ? 1: 0);
}

AWPRESULT awpCrossImage( awpImage* src, awpImage* src1, awpImage** dst )
{
    AWPRESULT res = AWP_OK;

    if (src == NULL)
        return AWP_BADARG;
    if (src1 == NULL)
        return AWP_BADARG;
    if (dst == NULL)
        return AWP_BADARG;

    if ((res = awpCheckImage(src)) != AWP_OK)
        return res;
    if ((res = awpCheckImage(src1)) != AWP_OK)
        return res;

    if (src->dwType != AWP_DOUBLE && src1->dwType != AWP_DOUBLE)
        return AWP_BADARG;



    if((res = awpCreateImage(dst, 256, 256, 1, AWP_DOUBLE)) != AWP_OK)
        return res;

    double* srcPix = (double*)src->pPixels;
    double* src1Pix = (double*)src1->pPixels;
    double* dstPix = (double*)(*dst)->pPixels;
    for ( int i = 0; i < 256; i++ )
    {
        for ( int j = 0; j < 256; j++ )
        {
            dstPix[256*i + j] = srcPix[256*i + j]*src1Pix[256*i + j];
        }
    }
    return res;
}


///////////////////////////////////////////////////////////////////////

AWPRESULT awpHSVColorFilterPreset(awpImage* pImage, awpImage* pPreset, awpImage** ppMask, AWPBYTE thr)
{

    //WORD w,h;
    AWPINT i;
    awpColor* hsv = NULL;
    awpColor  c;
    AWPBYTE* mask = NULL;
    AWPBYTE* pres = NULL;
    AWPBYTE  pv;
    awpImage* pHSVImage = NULL;
    AWPRESULT res = AWP_OK;

    if (pImage == NULL)
        return AWP_BADARG;
    if (pPreset == NULL)
        return AWP_BADARG;
    if (ppMask == NULL)
        return AWP_BADARG;

    if ((res = awpCheckImage(pImage)) != AWP_OK)
        return res;
    if ((res = awpCheckImage(pPreset)) != AWP_OK)
        return res;

    if (pImage->bChannels != 3 || pImage->dwType != AWP_BYTE)
        return AWP_BADARG;
    if (pPreset->bChannels != 1 || pPreset->dwType != AWP_BYTE)
        return AWP_BADARG;

//    w = pImage->sSizeX;
//    h = pImage->sSizeY;

//    if ((res = awpResize(pImage, w /4, h /4)) != AWP_OK)
//        return res;
    if ((res = awpCreateImage(ppMask, pImage->sSizeX, pImage->sSizeY, 1, AWP_BYTE)) != AWP_OK)
        return res;

    if ((res = awpRGBtoHSVImage(pImage, &pHSVImage)) != AWP_OK)
        return res;

    hsv = (awpColor*)pHSVImage->pPixels;
    mask = (AWPBYTE*)(*ppMask)->pPixels;
    pres = (AWPBYTE*)pPreset->pPixels;

    // фильтрация
    for (i = 0; i < pImage->sSizeX*pImage->sSizeY; i++)
    {
        c = hsv[i];
        pv = pres[256*c.bGreen + c.bRed];
        if (pv > thr)
            mask[i] = 255;
    }

    if((res = awpFilter(*ppMask, AWP_BLUR, AWP_FILTER_METHOD_ORDINARY)) !=AWP_OK)
        return res;
    if((res = awpFilter(*ppMask, AWP_BLUR, AWP_FILTER_METHOD_ORDINARY)) !=AWP_OK)
        return res;
/*
    if ((res = awpResize(*ppMask, w, h)) != AWP_OK)
        return res;
    if ((res = awpResize(pImage, w, h)) != AWP_OK)
        return res;
*/
    if ((res = awpReleaseImage(&pHSVImage)) != AWP_OK)
        return res;

    return res;
}

static awpRect GetAOIRect(awpPoint p1, awpPoint p2,double c1, double c2)
{
    double L = (p2.X - p1.X);
    int w = (int)(L / c1);//(roi.EyeDistance();
    int h = w;
    awpRect aoi_r;
    aoi_r.left  = p1.X - (short)(L*c2);
    aoi_r.right = aoi_r.left +w;
    aoi_r.top = p1.Y - (short)(L*c2);
    aoi_r.bottom = aoi_r.top + h;

    return  aoi_r;
}

//Class CContentFilter

CContentFilter::CContentFilter()
{
	CF_ERROR status = CF_SUCCESS;
	hLf	= NULL;

	status = CFCreateLocateFace(&hLf, "engine1.xml"); // locate face engine create objects 
	if (status != CF_SUCCESS)
		throw(status);
}

CContentFilter::~CContentFilter()
{
	if( hLf != NULL )
		CFDestroyLocateFace(&hLf);
}

bool CContentFilter::iColorFilter( awpImage** maskImage, awpImage** maskImage1,
                                colorFilterOptions* clrOptions,
                                CF_IMAGE hImage )
{

    *maskImage = NULL;
    *maskImage1 = NULL;
    awpImage* PresetHistogramm = NULL;
    awpImage* crossHist = NULL;
    awpImage* D2Hist = NULL;
    awpImage* ImageHSV = NULL;
    awpImage* Image = NULL;
    awpImage* CopyPresetHistogramm = NULL;
	awpImage* sourceImage = NULL;
    double* crossPix, *presetPix, *copyPresetPix, *copyCrossPix;
    bool res = true;
    try
    {

            sourceImage = ((TLFImage*)(hImage))->GetImage();
			if(!sourceImage)
				throw 0;
			if (awpCreateImage(&D2Hist, 256, 256, 1, AWP_DOUBLE) != AWP_OK)
                throw 0;
            if (PresetHistogramm != NULL)
                awpReleaseImage( &PresetHistogramm );
            if (awpLoadImage(clrOptions->presetFileName, &PresetHistogramm)!= AWP_OK)
                throw 0;
            if (ImageHSV != NULL)
                awpReleaseImage( &ImageHSV );


            if (Image != NULL)
                awpReleaseImage(&Image);
            if (awpCopyImage( sourceImage, &Image ) != AWP_OK)
                throw 0;
            if (awpRGBtoHSVImage(Image, &ImageHSV)!= AWP_OK)
                throw 0;
            if (awpGet2DHistogramm(ImageHSV, D2Hist, clrOptions->minValue, clrOptions->maxValue, false)!= AWP_OK)
                throw 0;
            if (crossHist != NULL)
                awpReleaseImage( &crossHist );
            if (awpCrossImage( PresetHistogramm, D2Hist, &crossHist )!= AWP_OK)
                throw 0;

            switch( clrOptions->filter )
            {
                case FILTER:
                    if(awpConvert( crossHist, AWP_CONVERT_TO_BYTE_WITH_NORM )!= AWP_OK)
                        throw 0;

                    if(awpHSVColorFilterPreset(Image, crossHist, maskImage, (BYTE)clrOptions->cThr) != AWP_OK)
                        throw 0;

                break;
                case BP_FILTER:
                    if (CopyPresetHistogramm != NULL)
                        awpReleaseImage(&CopyPresetHistogramm);
                    if(awpCopyImage( crossHist, &CopyPresetHistogramm )!= AWP_OK)
                        throw 0;
                    copyCrossPix = (double*)CopyPresetHistogramm->pPixels;
                    crossPix = (double*)crossHist->pPixels;
                    qsort((void *)copyCrossPix, 256*256, sizeof(double), compare< double >);

                    for (int i = 0; i < (256*256); i++)
                        crossPix[i] = 255*crossPix[i]/copyCrossPix[256*256-1];

                    if ( maskImage != NULL )
                        awpReleaseImage( maskImage );
                    if (awpBackProjection2D(ImageHSV, maskImage, crossHist, clrOptions->minValue, clrOptions->maxValue)!= AWP_OK)
                        throw 0;

                    if(awpFilter(*maskImage, AWP_BLUR, AWP_FILTER_METHOD_ORDINARY)!= AWP_OK)
                        throw 0;
                    if(awpFilter(*maskImage, AWP_BLUR, AWP_FILTER_METHOD_ORDINARY)!= AWP_OK)
                        throw 0;

                break;
                case PRESET_FILTER:
                    if (CopyPresetHistogramm != NULL)
                        awpReleaseImage(&CopyPresetHistogramm);
                    if(awpCopyImage( PresetHistogramm, &CopyPresetHistogramm )!= AWP_OK)
                        throw 0;
                    copyPresetPix = (double*)CopyPresetHistogramm->pPixels;
                    presetPix = (double*)PresetHistogramm->pPixels;
                    qsort((void *)copyPresetPix, 256*256, sizeof(double), compare< double >);

                    for (int i = 0; i < (256*256); i++)
                        presetPix[i] = 255*presetPix[i]/copyPresetPix[256*256-1];

                    if ( maskImage != NULL )
                        awpReleaseImage( maskImage );
                    if (awpBackProjection2D(ImageHSV, maskImage, PresetHistogramm, clrOptions->minValue, clrOptions->maxValue)!= AWP_OK)
                        throw 0;

                    if(awpFilter(*maskImage, AWP_BLUR, AWP_FILTER_METHOD_ORDINARY)!= AWP_OK)
                        throw 0;
                    if(awpFilter(*maskImage, AWP_BLUR, AWP_FILTER_METHOD_ORDINARY)!= AWP_OK)
                        throw 0;

                    if( *maskImage1 != NULL )
                        awpReleaseImage(maskImage1);
                    if(awpCopyImage( *maskImage, maskImage1 ) != AWP_OK)
                        throw 0;
                break;

            }
            if ( clrOptions->filter != PRESET_FILTER )
            {
                if (CopyPresetHistogramm != NULL)
                        awpReleaseImage(&CopyPresetHistogramm);
                if(awpCopyImage( PresetHistogramm, &CopyPresetHistogramm )!= AWP_OK)
                    throw 0;
                copyPresetPix = (double*)CopyPresetHistogramm->pPixels;
                presetPix = (double*)PresetHistogramm->pPixels;
                qsort((void *)copyPresetPix, 256*256, sizeof(double), compare< double >);

                for (int i = 0; i < (256*256); i++)
                    presetPix[i] = 255*presetPix[i]/copyPresetPix[256*256-1];

                if ( maskImage1 != NULL )
                    awpReleaseImage( maskImage1 );
                if (awpBackProjection2D(ImageHSV, maskImage1, PresetHistogramm, clrOptions->minValue, clrOptions->maxValue)!= AWP_OK)
                    throw 0;
                if(awpFilter(*maskImage1, AWP_BLUR, AWP_FILTER_METHOD_ORDINARY)!= AWP_OK)
                    throw 0;
                if(awpFilter(*maskImage1, AWP_BLUR, AWP_FILTER_METHOD_ORDINARY)!= AWP_OK)
                    throw 0;

            }
            if(awpFilter(*maskImage1, AWP_BLUR, AWP_FILTER_METHOD_ORDINARY)!= AWP_OK)
                throw 0;
            if(awpFilter(*maskImage1, AWP_BLUR, AWP_FILTER_METHOD_ORDINARY)!= AWP_OK)
                throw 0;
            if(awpFilter(*maskImage, AWP_BLUR, AWP_FILTER_METHOD_ORDINARY)!= AWP_OK)
                throw 0;
            if(awpFilter(*maskImage, AWP_BLUR, AWP_FILTER_METHOD_ORDINARY)!= AWP_OK)
                throw 0;


        
    }
	catch (...)
        {
            res = false;
        }
    if (D2Hist != NULL)
        awpReleaseImage(&D2Hist);
    if (crossHist != NULL)
        awpReleaseImage(&crossHist);
    if (PresetHistogramm != NULL)
        awpReleaseImage(&PresetHistogramm);
    if (CopyPresetHistogramm != NULL)
        awpReleaseImage(&CopyPresetHistogramm);
    if (Image != NULL)
        awpReleaseImage(&Image);
    if (ImageHSV != NULL)
        awpReleaseImage(&ImageHSV);


return res;
}

AWPRESULT awpCopyStrokeObj(awpStrokeObj* pSrc, awpStrokeObj* pDst)
{
    AWPRESULT res = AWP_OK;
    if (pSrc == NULL || pSrc->Num == 0)
        return AWP_BADARG;
    if (pDst == NULL)
        return AWP_BADARG;

    pDst->Num = pSrc->Num;
    pDst->strokes = NULL;
    pDst->strokes = (awpStroke*)malloc(pSrc->Num*sizeof(awpStroke));
    if (pDst->strokes == NULL)
        return AWP_BADMEMORY;
    memcpy(pDst->strokes, pSrc->strokes, pSrc->Num*sizeof(awpStroke));
    return res;
}


int CContentFilter::iGetFeatures(double** features, featuresOptions* ftsOptions,
                                 awpImage* maskImage, awpImage* maskSourceImage,
                                 awpImage* maskImage1, CF_IMAGE hImage,
                                 awpStrokeObj** bigStrokes, int* numObj, awpPoint** Center,
                                 float** angle, float** l, float** w,
                                 awpRect** interestRect, awpRect** fRect, int* numFaces)
{
    // Get features
    int res = 0;
    int Num = 0;
    double s_sum = 0;
    double s_sum_all = 0;
    int bigObjCount = 0;
    *numObj = 0;
    awpStrokeObj*			tmp = NULL;
    awpStrokeObj* strokes = NULL;
    int* Square = 0;
    int *Square4Big = 0;
    awpRect objRect;
    awpImage* probObjImage = 0;
	awpImage* sourceImage = NULL;
    awpContour contour;
    contour.Points=NULL;
	contour.NumPoints=0;
    IplImage* iplImage = NULL;

    CvRect windowIn;
    CvTermCriteria criteria;
	CvConnectedComp comp;
	CvBox2D box;

    BYTE* probObjPix;
    double* prob = 0;
    sqPair* pairs = 0;
    double* perimetr = 0;
    double* kForm = 0;
    *fRect = 0;
    *numFaces = 0;
    int x_min, x_max, y_min, y_max;
    prob = (double*)malloc(OBJECTS_COUNT*sizeof(double));
    perimetr = (double*)malloc(OBJECTS_COUNT*sizeof(double));
    kForm = (double*)malloc(OBJECTS_COUNT*sizeof(double));
    *features = (double*)malloc(FEATURES_COUNT*sizeof(double));
    *Center = (awpPoint*)malloc(OBJECTS_COUNT*sizeof(awpPoint));
    *angle = (float*)malloc(OBJECTS_COUNT*sizeof(float));
    *l = (float*)malloc(OBJECTS_COUNT*sizeof(float));
    *w = (float*)malloc(OBJECTS_COUNT*sizeof(float));
    (*interestRect) = (awpRect*)malloc(sizeof(awpRect));
    try
    {
        //-----------------------------------------------------
        // Get objects and mean objects square
        //----------------------------------------------------
        sourceImage = ((TLFImage*)(hImage))->GetImage();
		if(!sourceImage)
			throw -4;
		int numPix = sourceImage->sSizeX * sourceImage->sSizeY;

        if( strokes != NULL )
        {
            awpFreeStrokes( Num, &strokes );
            Num = 0;
        }
        if(awpGetStrokes( maskSourceImage, &Num, &strokes, ftsOptions->getStrokeThr ) != AWP_OK)
            throw -3;
        if ( !Num )
            throw -1;

        Square = (int*)malloc(Num*sizeof(int));
        memset ( Square, 0, Num*sizeof(int) );

        Square4Big = (int*)malloc( Num*sizeof(int) );
        memset ( Square4Big, 0, Num*sizeof(int) );

        tmp = (awpStrokeObj*)malloc(Num*sizeof(awpStrokeObj));
        pairs = (sqPair*)malloc(Num*sizeof(sqPair));

        for ( int i = 0; i < Num; i++ )
        {
            if(awpStrObjSquare( &strokes[i], &Square[i] ) != AWP_OK)
                throw -3;
            s_sum_all += (double)Square[i];
            if ( (double)Square[i]/numPix >= ftsOptions->s_min )
            {
                s_sum += (double)Square[i]/numPix;
                Square4Big[bigObjCount] = Square[i];

                if(awpCopyStrokeObj(&strokes[i], &tmp[bigObjCount])!=AWP_OK)
                    throw -3;
                pairs[bigObjCount].index = bigObjCount;
                pairs[bigObjCount].square = Square4Big[bigObjCount];
                bigObjCount += 1;
            }
        }
        if ( !bigObjCount )
            throw -1;


        qsort( (void*)pairs, bigObjCount, sizeof(sqPair), compareSqPair );

        x_min = sourceImage->sSizeX;
        x_max = 0;
        y_min = sourceImage->sSizeY;
        y_max = 0;

        for ( int i = 0; i < bigObjCount && i < OBJECTS_COUNT; i++ )
        {
            if( contour.Points != 0 )
                free ( contour.Points );
            if(awpGetObjCountour( &tmp[(pairs[i]).index], &contour ) !=AWP_OK)
                continue;

            if(awpGetContPerim( &contour, &perimetr[i] ) != AWP_OK)
                continue;

            if(awpCalcObjRect(&tmp[(pairs[i]).index], &objRect)!= AWP_OK)
                throw -3;
            kForm[i] = perimetr[i]*perimetr[i]/((objRect.right - objRect.left)*
                                                (objRect.bottom - objRect.top));
            if ( probObjImage != 0 )
                awpReleaseImage( &probObjImage );
            if(awpCopyRect( maskImage1, &probObjImage, &objRect )!=AWP_OK)
                throw -3;
            probObjPix = (BYTE*)probObjImage->pPixels;
            for ( int j = 0; j < probObjImage->sSizeX*probObjImage->sSizeY; j++ )
            {
                if( probObjPix[j] != 0 )
                    prob[i]+=1./(probObjImage->sSizeX*probObjImage->sSizeY*255);
            }


            iplImage = awpConvertToIplImage(maskImage1);


            windowIn.x = objRect.left;

            windowIn.y = objRect.top;

            windowIn.width = objRect.right - objRect.left;

            windowIn.height = objRect.bottom - objRect.top;

            criteria.type = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;

			criteria.max_iter = 10;

            criteria.epsilon = 1;
			
			cvCamShift (iplImage, windowIn, criteria, &comp, &box);

            if ( objRect.left < x_min )
                x_min = objRect.left;
            if ( objRect.top < y_min )
                y_min = objRect.top;
            if ( objRect.right > x_max )
                x_max = objRect.right;
            if ( objRect.bottom > y_max )
                y_max = objRect.bottom;

            (*interestRect)->left = x_min;
            (*interestRect)->top = y_min;
            (*interestRect)->right = x_max;
            (*interestRect)->bottom = y_max;

            (*angle)[i] = box.angle;
            (*l)[i] = box.size.height;
			(*w)[i] = box.size.width;
			((*Center)[i]).X = box.center.x;
			((*Center)[i]).Y = box.center.y;

        }

/*-----------------------------------------------------------------------
        CvSize imSize;

        IplImage* iplCornerImage = NULL;
        IplImage* iplSourceImage = NULL;
        IplImage* iplTmpImage = NULL;
        awpImage* itmp = NULL;
        awpImage* rtmp = NULL;
        awpImage* lMax = NULL;
        CvPoint2D32f* corners;
        awpPoint* cPoints;
        int corner_count = 50;
        imSize.width = sourceImage->sSizeX;
        imSize.height = sourceImage->sSizeY;

        awpCopyImage(sourceImage, &itmp);
        awpCopyImage(sourceImage, &rtmp);
        awpConvert(itmp, AWP_CONVERT_3TO1_BYTE);
        awpConvert(rtmp, AWP_CONVERT_3TO1_BYTE);
        iplSourceImage = awpConvertToIplImage( itmp );
        iplCornerImage = cvCreateImage( imSize, IPL_DEPTH_32F, 1 );
        iplTmpImage = cvCreateImage( imSize, IPL_DEPTH_32F, 1 );
        corners = (CvPoint2D32f*)malloc(corner_count*sizeof(CvPoint2D32f));
        cPoints = (awpPoint*)malloc(corner_count*sizeof(awpPoint));
        awpFilter( rtmp, PREWITT_H, AWP_FILTER_METHOD_ORDINARY );
        awpLocalMax(rtmp, &lMax, 90, _4_NEIGHBOURS);

        if(rtmp != NULL)
            awpReleaseImage(&rtmp);

        if(lMax != NULL)
            awpReleaseImage(&lMax);

        if(iplSourceImage != NULL)
            cvReleaseImage( &iplImage );

        if(iplCornerImage != NULL)
            cvReleaseImage( &iplCornerImage );

        if(iplTmpImage != NULL)
            cvReleaseImage( &iplTmpImage );

        if(itmp != NULL)
            awpReleaseImage(&itmp);

        if( corners != NULL )
            free(corners);

        if( cPoints != NULL )
            free(cPoints);
//-----------------------------------------------------------------*/
        qsort((void*)Square4Big, bigObjCount, sizeof(int), compare<int>);
        *numObj = bigObjCount;
        memset( *features, 0, FEATURES_COUNT*sizeof(double) );
        for ( int i = 0; i < bigObjCount && i < OBJECTS_COUNT; ++i )
        {
            (*features)[1] = (double)bigObjCount;
            (*features)[15*i+2] = (double)(((*Center)[i]).X)/sourceImage->sSizeX;
            (*features)[15*i+3] = (double)(((*Center)[i]).Y)/sourceImage->sSizeY;
            (*features)[15*i+4] = (*l)[i]/sourceImage->sSizeX;
            (*features)[15*i+5] = (*w)[i]/sourceImage->sSizeX;
            (*features)[15*i+6] = (*angle)[i];
            (*features)[15*i+7] = prob[i];
            (*features)[15*i+8] = kForm[i];
            (*features)[15*i+9] = (double)pairs[i].square/((x_max-x_min)*(y_max-y_min));
        }
        iFindFace( hImage, *features, fRect, numFaces, *interestRect, ftsOptions );

      }
      catch( int q )
      {
          switch(q)
          {
            case -1:
                res = -1;
            break;
            case -2:
                res = -2;
            break;
            case -3:
                res = -3;
            break;
			case -4:
                res = -4;
            break;
          }
          free(tmp);
          tmp = NULL;
      }

    if(iplImage != NULL)
        cvReleaseImage( &iplImage );



    if(probObjImage != 0)
        awpReleaseImage(&probObjImage);



    if( strokes != NULL )
        awpFreeStrokes( Num, &strokes );

    if(Square4Big != 0)
        free (Square4Big);

    if(pairs != 0)
        free (pairs);

    if(Square != 0)
        free (Square);

    if(perimetr != 0)
        free (perimetr);

    if(kForm != 0)
        free (kForm);

    if(prob != 0)
        free (prob);

    if( contour.Points != 0 )
        free ( contour.Points );

    *bigStrokes = tmp;

    return res;
}

bool CContentFilter::iFindFace( CF_IMAGE hImage, double* features, awpRect** fRect, int* numFaces, awpRect* interestRect, featuresOptions* ftsOptions )
{
    bool res = true;
    int facesCount = 0, i;
    double squareFaceRect = 0;
	awpRect* faces = NULL;
    CF_ERROR status = CF_SUCCESS;
		
	status = CFFindFaces(hLf, hImage);
	if (status != CF_SUCCESS)
	{	
		res = false;
		throw(status);
	}
	
	status = CFGetFacesCount(hLf, &facesCount);
	if (!facesCount || status != CF_SUCCESS)
	{
		*numFaces = 0;
		features[0] = 0;
		res = false;
		return res;
	}
	else
	{
		faces = (awpRect*)malloc(facesCount*sizeof(awpRect));
		*fRect = (awpRect*)malloc(facesCount*sizeof(awpRect));
		for (i = 0; i < facesCount; i++ )
        {
			CF_POINT pLe, pRe;
			CFGetEyePos(hLf, i, &pLe, &pRe);
			awpPoint pAwpLe, pAwpRe; pAwpLe.X = pLe.X; pAwpLe.Y = pLe.Y; pAwpRe.X = pRe.X; pAwpRe.Y = pRe.Y;
			faces[i] = GetAOIRect(pAwpLe, pAwpRe, 0.4, 0.7);
			squareFaceRect += ((faces[i].right - faces[i].left)*
							(faces[i].bottom - faces[i].top));
			(*fRect)[i] = faces[i];
		}
		*numFaces = facesCount;
		features[0] = squareFaceRect/((interestRect->right - interestRect->left)
					*(interestRect->bottom - interestRect->top));
	}
            
	if( faces != NULL )
		free(faces);
    return res;
}

int CContentFilter::iSolve( double* distance, solveOptions* slvOptions, double* features )
{
    int res = 0;

    *distance = 0;
    if( features[5] < slvOptions->k1 )
        res = 0;

    else if ( features[5] >= slvOptions->k2 )
    {
        if( features[0] > slvOptions->k3 )
            res = 0;
        else
            res = 1;
    }

    else
    {
        if( features[0]!=0 )
        {
            if( features[0] <= slvOptions->k3 )
                res = 1;
            else
                res = 0;
        }
        else
            res = 2;
    }
    return res;
}