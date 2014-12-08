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

enum CF_PRIMITIVES_TYPE {CF_LINE, // линия
CF_VLINE, // веритикальная линия 
CF_HLINE, // горизонтальная линия 
CF_RECT   // прямоугольник
};
extern "C"
{/* создает детектор примитивов  
*/
CF_ERROR __EXPORT_TYPE CFCreateRectDetector(CF_RECT_DETECTOR* hDetector);
/* создает детектор примитивов  
*/
CF_ERROR __EXPORT_TYPE CFDestroyRectDetector(CF_RECT_DETECTOR* hDetector);
/* выполняет поиск прямоугольников 
*/
CF_ERROR __EXPORT_TYPE CFFindRects(CF_RECT_DETECTOR hDetector, CF_IMAGE hImage);
/* возвращает число найденных прямоугольников 
*/
CF_ERROR __EXPORT_TYPE CFGetNumRects(CF_RECT_DETECTOR hDetector, int* NumRects);
/* возвращает найденные прямоугольники 
*/
CF_ERROR __EXPORT_TYPE CFGetRect(CF_RECT_DETECTOR* hDetector, CF_POINT* LeftCorner, CF_POINT* RightCorner, int index);
/* отрисовка найденных прямоугольников
*/
CF_ERROR __EXPORT_TYPE CFDrawRects(CF_RECT_DETECTOR hDetector, CF_IMAGE hImage);

/*
	поиск объектов с помощью сравнения шаблонов 
*/
CF_ERROR __EXPORT_TYPE CFTemplateMatch(CF_IMAGE hImage, void* hTemplate, CF_POINT* LeftCorner, CF_POINT* RightCorner);

}
#endif //_CF_PRIMITIVES_H_
