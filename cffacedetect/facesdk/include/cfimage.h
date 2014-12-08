/*
//     Chaos Face SDK version 1.8     
//	   Copyright(c) 2008-2010 CCT gmbH, all rights reserved.
//	   Third party copyrights are property of their respective owners.
//     cfimahe.h 
//     version 1.0.0	
*/

#ifndef _CF_IMAGE_H_
#define _CF_IMAGE_H_
#include "cftypes.h"
#include "cf_error.h"
/*!\brief ��������� �����������
   
  ��� ��������� �������� �������� ���������
  �����������, � ������� �������� ����������
  CF.\n 	
*/
struct CF_IMAGE_PARAMS
{
  /*!\brief ������ �����������*/
  CF_WORD width;         
  /*!\brief ������ �����������*/
  CF_WORD height;        
  /*!\brief ����� ��� �� ���� �������*/
  CF_WORD bpp;           
};
/*! \brief ���������� �����������
	
	���������� ����������� ������ ����������
*/

/*! \enum CF_IMAGE_DRAW_OPTIONS 
	\brief ��� ��������� �����������  

*/
enum CF_IMAGE_DRAW_OPTIONS {
	doBestFit = 0,						/** ������� ����������� � ������������� ���� �������� ��������� ������ */
	doStretch,    					    /** ������� ����������� � ������������� ���� �� �������� ��������� ������*/
};
typedef CF_HANDLE CF_IMAGE;            /*���������� �����������*/    
typedef CF_HANDLE CF_MEDIA_SOURCE;	   /*���������� ��������� �����������. ����������� ����������� ����� ����
										- ���������� �����, ����������*/
extern "C"
{
/*!\fn  CF_ERROR __EXPORT_TYPE CFCreateImage( CF_IMAGE *hImage );
   \brief ������� �����������
	
	������������� ������ ��� ����������� � ���������� ��� ����������
	\param hImage ��������� �� ���������� �����������

	������������ ��������:\n CF_SUCCESS ���� ���������� ��� ������� ������.\n
	CFF_INVALID_HANDLE  ���� hImage == NULL \n
	CFCR_NOT_ENOUGH_MEMORY  � ������ �������� ��� ����������� ������.\n
	
	��������� ���������� ������ ���� ������ � �������������� ������� \n
	CFDestroyImage\n
*/
CF_ERROR __EXPORT_TYPE CFCreateImage( CF_IMAGE *hImage );
/*!\fn  CF_ERROR __EXPORT_TYPE CFDestroyImage( CF_IMAGE *hImage );
   \brief �������  ����������t
	
	������� ����������� �� ������ � ����������� ��� ���������� hImage

    \param hImage ��������� �� ���������� ����������� 

	������������ ��������:\n CF_SUCCESS ���� �������� ���� ��������� �������.\n
	CFF_INVALID_HANDLE  ����  hImage == NULL ��� ����������������\n
*/
CF_ERROR __EXPORT_TYPE CFDestroyImage( CF_IMAGE *hImage );
/*!\fn  CF_ERROR __EXPORT_TYPE CFLoadImage( CF_IMAGE hImage, char* path )
   \brief ��������� ����������� �� ����� 
	
	��������� ����������� �� �����. ��������������� �������:\n
    JPEG, AWP.\n
	\param hImage ���������� �����������.
    \param path   ������ ��� �����. 

	������������ ��������:\n CF_SUCCESS ���� �������� ���� ��������� �������.\n
	CFF_INVALID_HANDLE  ����  hImage == NULL ��� ����������������\n
	CFCR_NOT_ENOUGH_MEMORY  � ������ ���������� ������ ��� �� �����������\n
	CFF_INVALID_PARAM   ���� path == NULL\n
    CFF_CANNOT_LOAD_IMAGE ���� ����������� �� ���� ���������
*/
CF_ERROR __EXPORT_TYPE CFLoadImage( CF_IMAGE hImage, char* path );
/*!\fn  CF_ERROR __EXPORT_TYPE CFLoadFromDIB(CF_IMAGE hImage, CF_BMP info, CF_BYTE* DIBPixels )
   \brief ��������� ����������� �� Microsoft Windows DIB 
	
	��������� ������ ����������� �� ��������� BITMAPINFO � ������� ������, \n
    � ������� ���������� ������� �����������. \n 
    ������������� ��� ����������� ����������� �� ������� Microsoft DIB �� \n
    ���������� ������ ����������. 


	\param hImage  ���������� �����������.
    \param info    �������� ����������� � ������� Windows DIB
    \param DIBPixels ������� ����������� 

	������������ ��������: \n CF_SUCCESS ���� �������� ���� ��������� �������.\n
	CFF_INVALID_HANDLE  ����  hImage == NULL ��� ����������������\n
	CFCR_NOT_ENOUGH_MEMORY  � ������ ���������� ������ ��� �� �����������\n
    CFF_INVALID_DIB �������� DIB �� ������������� 
*/
CF_ERROR __EXPORT_TYPE CFLoadFromDIB(CF_IMAGE hImage, CF_BMP info, CF_BYTE* DIBPixels );
/*!\fn  CF_ERROR __EXPORT_TYPE CFLoadFromDump(CF_IMAGE hImage, CF_WORD width, CF_WORD height, CF_WORD bpp, CF_BYTE* pixels, CF_WORD line_width);
   \brief ��������� ����������� �� ����� ������ 
	
	��������� ����������� �� ����� ������. ������������� ��� ����������� ����������� �� ������ ������� 
    � ������. 

	\param hImage - ���������� �����������.
    \param width - ������ ����������� � �����
    \param height - ������ ����������� � �����
    \param bpp - ����� ��� �� ���� ������� � ����������� � �����
    \param pixels - ��������� �� ������� ����������� 
    \param line_width - ����� ����� ������ �������� � ������

	������������ ��������:\n CF_SUCCESS ���� �������� ���� ��������� �������.\n
	CFF_INVALID_HANDLE  ����  hImage == NULL ��� ����������������\n
    CFCR_NOT_ENOUGH_MEMORY  � ������ ���������� ������ ��� �� �����������\n
	CFF_INVALID_PARAM   � ������, ���� pixels == NULL

*/
CF_ERROR __EXPORT_TYPE CFLoadFromDump(CF_IMAGE hImage, CF_WORD width, CF_WORD height, CF_WORD bpp, CF_BYTE* pixels, CF_WORD line_width);

CF_ERROR __EXPORT_TYPE CFLoadFromJpeg(CF_IMAGE hImage, CF_BYTE* pixels, int DataSize);
/*!\fn  CF_ERROR __EXPORT_TYPE CFSaveToDump(CF_IMAGE hImage, CF_WORD* pWidth, CF_WORD* pHeight, CF_WORD* pBpp, CF_BYTE* buffer);
   \brief ��������� ����������� � ���� ������
	
	��������� ����������� � ���� ������. ������������� ��� ����������� 
    �����������, ��������������� �� ���������� ������� ����������, � 
    ������ �������. 

	\param hImage - ���������� �����������.
    \param pWidth - ��������� �� ������ ����������� � �����
    \param pHeight - ��������� �� ������ ����������� � �����
    \param pBpp - ��������� �� ����� ��� �� ���� ������� � ����������� � �����
    \param buffer - ��������� �� ������� ����������� 

	������������ ��������:\n CF_SUCCESS ���� �������� ���� ��������� �������.\n
	CFF_INVALID_HANDLE  ����  hImage == NULL ��� ����������������\n
    CFCR_NOT_ENOUGH_MEMORY  � ������ ���������� ������ ��� �� �����������\n
    CFF_INVALID_PARAM   � ������, ���� ���� �� ���������� ����� ����. 
*/
CF_ERROR __EXPORT_TYPE CFSaveToDump(CF_IMAGE hImage, CF_WORD* pWidth, CF_WORD* pHeight, CF_WORD* pBpp, CF_BYTE* buffer);

CF_ERROR __EXPORT_TYPE CFSaveToJpeg(CF_IMAGE hImage, CF_BYTE* pixels, int* DataSize);

/*!\fn  CF_ERROR __EXPORT_TYPE CFGetDumpSize(CF_IMAGE hImage, CF_DWORD* pDumpSize);
   \brief ������ ����� ������, ����������� ��� ���������� �����������  
	
	���������� ������ ����� ������, � ������� ����� ��������� ����������� hImage. \n
    ������������� ��� ����������� �����������, ��������������� �� ���������� ������� ����������, � \n
    ������ �������. \n

    \param  hImage - ���������� �����������.
    \param pDumpSize - ������ ����� ������

	������������ ��������:\n CF_SUCCESS ���� �������� ���� ��������� �������.\n
	CFF_INVALID_HANDLE  ����  hImage == NULL ��� ����������������\n
    CFF_INVALID_PARAM   � ������, ���� pDumpSize ����� ����. 
*/
CF_ERROR __EXPORT_TYPE CFGetDumpSize(CF_IMAGE hImage, CF_DWORD* pDumpSize);
/*!\fn  CF_ERROR __EXPORT_TYPE CFSaveImage( CF_IMAGE hImage, char* path )
   \brief ��������� ����������� � ����

   ��������� ����������� hImage � ����. �������������� �������: JPG, AWP
   ������ ��� ����� path ������ ��������� ���������� .JPG ��� .AWP

   \param -hImage ���������� �����������.
   \param -path ������ ��� �����, � ������� ��������� ��������� �����������. 

	������������ ��������:\n CF_SUCCESS ���� �������� ���� ��������� �������.\n
	CFF_INVALID_HANDLE  ����  hImage == NULL ��� ����������������\n
	CFCR_NOT_ENOUGH_MEMORY  in case of lack or damage to the memory.
*/
CF_ERROR __EXPORT_TYPE CFSaveImage( CF_IMAGE hImage, char* path );
/*!\fn  CF_ERROR __EXPORT_TYPE CFDIBSize(CF_IMAGE hImage, int* pSize)
   \brief ������ ������, ����������� ��� ���������� ����������� � Windows DIB 

	���������� ������ ����� ������, � ������� ����� ��������� ����������� hImage. \n
    ������������� ��� ����������� �����������, ��������������� �� ���������� ������� ����������, � \n
     ������ Windows DIB. \n

	\param  hImage - ���������� �����������.
    \param pSize - ������ ����� ������

	������������ ��������:\n CF_SUCCESS ���� �������� ���� ��������� �������.\n
	CFF_INVALID_HANDLE  ����  hImage == NULL ��� ����������������\n
    CFF_INVALID_PARAM   � ������, ���� pSize ����� ����. 
*/
CF_ERROR __EXPORT_TYPE CFDIBSize(CF_IMAGE hImage, int* pSize);
/*!\fn  CF_ERROR __EXPORT_TYPE CFSaveToDIB( CF_IMAGE hImage, CF_BMP* info, CF_BYTE* DIBPixels )
   \brief ����������� ����������� � ������ Windows DIB
	
	����������� ����������� � ������ Windows DIB.

	\param hImage - ���������� �����������.
    \param info - �������� ����������� � ������� Windows DIB
    \param DIBPixels - ����� ��� �������� �����������. ������ ������ ������\n
    ����������� � ������� ������� CFDIBSize 

	������������ ��������:\n CF_SUCCESS ���� �������� ���� ��������� �������.\n
	CFF_INVALID_HANDLE  ����  hImage == NULL ��� ����������������\n
    CFF_INVALID_DIB ���� info == NULL ��� ���������� ��������� �����������. \n
*/
CF_ERROR __EXPORT_TYPE CFSaveToDIB( CF_IMAGE hImage, CF_BMP* info, CF_BYTE* DIBPixels );
/*!\fn CF_ERROR __EXPORT_TYPE CFGetImageParams(CF_IMAGE hImage, CF_IMAGE_PARAMS* pParams)
   \brief ���������� ��������� ����������� hImage
	
	���������� ��������� ����������� hImage � ���������� pParams
	\param hImage - ���������� �����������.
    \param pParams - ��������� �����������

	������������ ��������:\n CF_SUCCESS ���� �������� ���� ��������� �������.\n
	CFF_INVALID_HANDLE  ����  hImage == NULL ��� ����������������\n
    CFF_INVALID_PARAM  ���� pParams == NULL 
*/
CF_ERROR __EXPORT_TYPE CFGetImageParams(CF_IMAGE hImage, CF_IMAGE_PARAMS* pParams);

/*!\fn CF_ERROR __EXPORT_TYPE CFDrawImage(CF_IMAGE hImage)
   \brief ������������ ����������� hImage �� ���������� CanvasHandler
	
	������������ ����������� hImage �� ���������� CanvasHandler � 
	� ������ ����� DrawOptions 

	\param hImage		 - ���������� �����������.
    \param CanvasHandler - ������������� ����������. � ����� Windows ��� hWnd 

	������������ ��������:\n CF_SUCCESS ���� �������� ���� ��������� �������.\n
	CFF_INVALID_HANDLE  ����  hImage == NULL ��� ����������������\n
*/
CF_ERROR __EXPORT_TYPE CFDrawImage(CF_IMAGE hImage, int CanvasHandler, CF_IMAGE_DRAW_OPTIONS DrawOptions);

CF_ERROR __EXPORT_TYPE CFCreateMediaSource(CF_MEDIA_SOURCE* hMedia, const char* path);
CF_ERROR __EXPORT_TYPE CFDestroyMediaSource(CF_MEDIA_SOURCE* hMedia);
CF_ERROR __EXPORT_TYPE CFQueryNextImage(CF_MEDIA_SOURCE hMedia, CF_IMAGE hImage);

}    	
#endif//_CF_IMAGE_H_