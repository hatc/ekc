	/*
//     Chaos Face SDK version 1.8     
//	   Copyright(c) 2008-2012 CCT gmbH, all rights reserved.
//	   Third party copyrights are property of their respective owners.
//     cfprimitives.h 
//     version 1.0.0	
*/

#ifndef _CF_PRIMITIVES_H_
#define _CF_PRIMITIVES_H_
#include "cftypes.h"
#include "cf_error.h"
#include "cfimage.h"

typedef CF_HANDLE CF_RECT_DETECTOR;

enum CF_PRIMITIVES_TYPE {CF_LINE, // �����
CF_VLINE, // ������������� ����� 
CF_HLINE, // �������������� ����� 
CF_RECT   // �������������
};
extern "C"
{/* ������� �������� ����������  
*/
CF_ERROR __EXPORT_TYPE CFCreateRectDetector(CF_RECT_DETECTOR* hDetector);
/* ������� �������� ����������  
*/
CF_ERROR __EXPORT_TYPE CFDestroyRectDetector(CF_RECT_DETECTOR* hDetector);
/* ��������� ����� ��������������� 
*/
CF_ERROR __EXPORT_TYPE CFFindRects(CF_RECT_DETECTOR hDetector, CF_IMAGE hImage);
/* ���������� ����� ��������� ��������������� 
*/
CF_ERROR __EXPORT_TYPE CFGetNumRects(CF_RECT_DETECTOR hDetector, int* NumRects);
/* ���������� ��������� �������������� 
*/
CF_ERROR __EXPORT_TYPE CFGetRect(CF_RECT_DETECTOR* hDetector, CF_POINT* LeftCorner, CF_POINT* RightCorner, int index);
/* ��������� ��������� ���������������
*/
CF_ERROR __EXPORT_TYPE CFDrawRects(CF_RECT_DETECTOR hDetector, CF_IMAGE hImage);

/*
	����� �������� � ������� ��������� �������� 
*/
CF_ERROR __EXPORT_TYPE CFTemplateMatch(CF_IMAGE hImage, void* hTemplate, CF_POINT* LeftCorner, CF_POINT* RightCorner);

}
#endif //_CF_PRIMITIVES_H_
