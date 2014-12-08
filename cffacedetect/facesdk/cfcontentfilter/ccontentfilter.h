
#ifndef __CCONTENTFILTER_H__
#define __CCONTENTFILTER_H__


#include "cffacedetect.h"
#include "awpipl.h"


#define FEATURES_COUNT 77
#define OBJECTS_COUNT 5
#define FILTER      0
#define BP_FILTER   1
#define PRESET_FILTER   2

struct colorFilterOptions
{
	int filter;
	int minValue;
	int maxValue;
	int cThr;
	char presetFileName[512];
};

struct featuresOptions
{
	double fRecThr;
	double s_min;
	int getStrokeThr;
	double pThr;
};

struct solveOptions
{
	double RecThreshold; 
	double x_mean_a[FEATURES_COUNT];
	double x_mean_e[FEATURES_COUNT];
	double k1;
	double k2;
	double k3; 
};
struct sqPair
{
	int index;
	int square;
};

class CContentFilter
{
public:
	CF_LOCATE_FACE hLf;
	
	//Options
	colorFilterOptions clrOptions;
	featuresOptions ftsOptions;
	solveOptions slvOptions;
	
	
	CContentFilter();
	~CContentFilter();

	bool iColorFilter(awpImage** maskImage, awpImage** maskImage1,
                            colorFilterOptions* clrOptions, CF_IMAGE hImage );
	int iGetFeatures(double** features, featuresOptions* ftsOptions,
									awpImage* maskImage, awpImage* maskSourceImage, 
									awpImage* maskImage1, CF_IMAGE hImage,
									awpStrokeObj** BigStrokes, int* numObj, awpPoint** Center,
									float** angle, float** l, float** w, awpRect** interestRect,
									awpRect** fRect, int* numFaces);
	int iSolve( double* d, solveOptions* slvOptions, double* features );

	bool iFindFace( CF_IMAGE hImage, double* features,
								awpRect** fRect, int* numFaces, awpRect* interestRect,
								featuresOptions* ftsOptions );
};

#endif // __CCONTENTFILTER_H__