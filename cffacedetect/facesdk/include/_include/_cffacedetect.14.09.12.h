/*
//     Chaos Face SDK version 1.8     
//	   Copyright(c) 2008-2010 CCT gmbH, all rights reserved.
//	   Third party copyrights are property of their respective owners.
//     
*/

#ifndef __CFFACEDETECTOR_H_
#define __CFFACEDETECTOR_H_
#include "cffacedetect.h"
#include "awpipl.h"
#include "LF.h"
#include "tinyxml.h"
#include "LFScanners.h"
#include "LFDetector.h"
#include "LFEngine.h"
struct CF_FACE_POS
{
    long ID;                /*object ID*/
    CF_POINT LeftEye;       /*Left eye position*/
    CF_POINT RightEye;      /*Right eye position*/
    double Err;             /*Raiting*/
	CF_FACE_TILT Tilt;		/*tilt feature*/
    long   NumErr;          /*?????*/
    double Errors[8];       /*?????*/
};

CF_ERROR CFGetFaces( CF_LOCATE_FACE hLocateFace, CF_FACE_POS** Faces );
CF_ERROR CFFreeFacePos( CF_FACE_POS* Face );
CF_ERROR CFDrawFacePos(CF_IMAGE hImage, CF_FACE_POS* ppos, CF_COLOR color);
/*
     Функция:
        CFGetFaceImage
     Цель:
         Возвращает в переменной hFaceImage фрагмент изображения, содержащий лицо
     Параметры:
       [ in ] hImage  - дескриптор входного изображения
       [ in ] ppos    - указатель на местоположение лица на изображении hImage
       [ out ] hFaceImage - изображение, содержащее лицо
      Возвращаемые заначения:
     CF_SUCCESS - Переменная hFaceImage cодержит изображение лица
     в противном случае возвращает код ошибки.
    Комментарий:
		Полученное изображение приведено к стандартному размеру 32x32 пискселя
        и имеет глубину 24 бита на одну точку.
        Для изменения размеров изображения используется фцнкция CFImageResize
*/
CF_ERROR CFGetFaceImage1(CF_IMAGE hImage, CF_FACE_POS* ppos, CF_IMAGE* hFaceImage);

#endif //__CFFACEDETECTOR_H_