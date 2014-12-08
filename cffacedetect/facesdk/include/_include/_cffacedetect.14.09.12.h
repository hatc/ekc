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
     �������:
        CFGetFaceImage
     ����:
         ���������� � ���������� hFaceImage �������� �����������, ���������� ����
     ���������:
       [ in ] hImage  - ���������� �������� �����������
       [ in ] ppos    - ��������� �� �������������� ���� �� ����������� hImage
       [ out ] hFaceImage - �����������, ���������� ����
      ������������ ���������:
     CF_SUCCESS - ���������� hFaceImage c������� ����������� ����
     � ��������� ������ ���������� ��� ������.
    �����������:
		���������� ����������� ��������� � ������������ ������� 32x32 ��������
        � ����� ������� 24 ���� �� ���� �����.
        ��� ��������� �������� ����������� ������������ ������� CFImageResize
*/
CF_ERROR CFGetFaceImage1(CF_IMAGE hImage, CF_FACE_POS* ppos, CF_IMAGE* hFaceImage);

#endif //__CFFACEDETECTOR_H_