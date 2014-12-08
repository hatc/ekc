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
/*!\brief Параметры изображения
   
  Эта струкрута содержит основные параметры
  изображения, с которым работает библиотека
  CF.\n 	
*/
struct CF_IMAGE_PARAMS
{
  /*!\brief ширина изображения*/
  CF_WORD width;         
  /*!\brief высота изображения*/
  CF_WORD height;        
  /*!\brief число бит на один пиксель*/
  CF_WORD bpp;           
};
/*! \brief дескриптор изображения
	
	Дескриптор изображения внутри библиотеки
*/
typedef CF_HANDLE CF_IMAGE;
extern "C"
{
/*!\fn  CF_ERROR __EXPORT_TYPE CFCreateImage( CF_IMAGE *hImage );
   \brief создает изображение
	
	Распределеяет память под изображение и возвращает его дескриптор
	\param hImage указатель на дескриптор изображения

	Возвращаемые значения:\n CF_SUCCESS если дескриптор был успешно создан.\n
	CFF_INVALID_HANDLE  если hImage == NULL \n
	CFCR_NOT_ENOUGH_MEMORY  в случае нехватки или повреждения памяти.\n
	
	Созданный дескриптор должен быть удален с использованием функции \n
	CFDestroyImage\n
*/
CF_ERROR __EXPORT_TYPE CFCreateImage( CF_IMAGE *hImage );
/*!\fn  CF_ERROR __EXPORT_TYPE CFDestroyImage( CF_IMAGE *hImage );
   \brief удаляет  изображениt
	
	Удаляет изображение из памяти и освобождает его дескриптор hImage

    \param hImage указатель на дескриптор изображения 

	Возвращаемые значения:\n CF_SUCCESS если операция была проведена успешно.\n
	CFF_INVALID_HANDLE  если  hImage == NULL или недействительный\n
*/
CF_ERROR __EXPORT_TYPE CFDestroyImage( CF_IMAGE *hImage );
/*!\fn  CF_ERROR __EXPORT_TYPE CFLoadImage( CF_IMAGE hImage, char* path )
   \brief загружает изображение из файла 
	
	Загружает изображение из файла. Поддерживаетмые форматы:\n
    JPEG, AWP.\n
	\param hImage дескриптор изображения.
    \param path   полное имя файла. 

	Возвращаемые значения:\n CF_SUCCESS если операция была проведена успешно.\n
	CFF_INVALID_HANDLE  если  hImage == NULL или недействительный\n
	CFCR_NOT_ENOUGH_MEMORY  в случае недостатка памяти или ее повреждения\n
	CFF_INVALID_PARAM   если path == NULL\n
    CFF_CANNOT_LOAD_IMAGE если изображение не было загружено
*/
CF_ERROR __EXPORT_TYPE CFLoadImage( CF_IMAGE hImage, char* path );
/*!\fn  CF_ERROR __EXPORT_TYPE CFLoadFromDIB(CF_IMAGE hImage, CF_BMP info, CF_BYTE* DIBPixels )
   \brief загружает изображение из Microsoft Windows DIB 
	
	Загружает данные изображения из структуры BITMAPINFO и области памяти, \n
    в которой содержатся пиксели изображения. \n 
    Предназначена для конвертации изображения из формата Microsoft DIB во \n
    внутренний формат библиотеки. 


	\param hImage  дескриптор изображения.
    \param info    описание изображения в формате Windows DIB
    \param DIBPixels пиксели изображения 

	Возвращаемые значения: \n CF_SUCCESS если операция была проведена успешно.\n
	CFF_INVALID_HANDLE  если  hImage == NULL или недействительный\n
	CFCR_NOT_ENOUGH_MEMORY  в случае недостатка памяти или ее повреждения\n
    CFF_INVALID_DIB описание DIB не действительно 
*/
CF_ERROR __EXPORT_TYPE CFLoadFromDIB(CF_IMAGE hImage, CF_BMP info, CF_BYTE* DIBPixels );
/*!\fn  CF_ERROR __EXPORT_TYPE CFLoadFromDump(CF_IMAGE hImage, CF_WORD width, CF_WORD height, CF_WORD bpp, CF_BYTE* pixels, CF_WORD line_width);
   \brief загружает изображение из дампа памяти 
	
	Загружает изображение из дампа памяти. Предназначена для конвертации изображения из одного формата 
    в другой. 

	\param hImage - дескриптор изображения.
    \param width - ширина изображения в дампе
    \param height - высота изображения в дампе
    \param bpp - число бит на один пиксель у изображения в дампе
    \param pixels - указатель на пиксели изображения 
    \param line_width - длина одной строки пикселей в байтах

	Возвращаемые значения:\n CF_SUCCESS если операция была проведена успешно.\n
	CFF_INVALID_HANDLE  если  hImage == NULL или недействительный\n
    CFCR_NOT_ENOUGH_MEMORY  в случае недостатка памяти или ее повреждения\n
	CFF_INVALID_PARAM   в случае, если pixels == NULL

*/
CF_ERROR __EXPORT_TYPE CFLoadFromDump(CF_IMAGE hImage, CF_WORD width, CF_WORD height, CF_WORD bpp, CF_BYTE* pixels, CF_WORD line_width);
/*!\fn  CF_ERROR __EXPORT_TYPE CFSaveToDump(CF_IMAGE hImage, CF_WORD* pWidth, CF_WORD* pHeight, CF_WORD* pBpp, CF_BYTE* buffer);
   \brief сохраняет изображение в дамп памяти
	
	Сохраняет изображение в дамп памяти. Предназначена для конвертации 
    изображения, представленного во внутреннем формате библиотеки, в 
    другие форматы. 

	\param hImage - дескриптор изображения.
    \param pWidth - указатель на ширину изображения в дампе
    \param pHeight - указатель на высоту изображения в дампе
    \param pBpp - указатель на число бит на один пиксель у изображения в дампе
    \param buffer - указатель на пиксели изображения 

	Возвращаемые значения:\n CF_SUCCESS если операция была проведена успешно.\n
	CFF_INVALID_HANDLE  если  hImage == NULL или недействительный\n
    CFCR_NOT_ENOUGH_MEMORY  в случае недостатка памяти или ее повреждения\n
    CFF_INVALID_PARAM   в случае, если один из параметров равен нулю. 
*/
CF_ERROR __EXPORT_TYPE CFSaveToDump(CF_IMAGE hImage, CF_WORD* pWidth, CF_WORD* pHeight, CF_WORD* pBpp, CF_BYTE* buffer);
/*!\fn  CF_ERROR __EXPORT_TYPE CFGetDumpSize(CF_IMAGE hImage, CF_DWORD* pDumpSize);
   \brief размер дампа памяти, необходимый для сохранения изображения  
	
	Возвращает размер дампа памяти, в который можно сохранить изображение hImage. \n
    Предназначена для конвертации изображения, представленного во внутреннем формате библиотеки, в \n
    другие форматы. \n

    \param  hImage - дескриптор изображения.
    \param pDumpSize - размер дампа памяти

	Возвращаемые значения:\n CF_SUCCESS если операция была проведена успешно.\n
	CFF_INVALID_HANDLE  если  hImage == NULL или недействительный\n
    CFF_INVALID_PARAM   в случае, если pDumpSize равен нулю. 
*/
CF_ERROR __EXPORT_TYPE CFGetDumpSize(CF_IMAGE hImage, CF_DWORD* pDumpSize);
/*!\fn  CF_ERROR __EXPORT_TYPE CFSaveImage( CF_IMAGE hImage, char* path )
   \brief сохраняет изображение в файл

   Сохраняет изображение hImage в файл. Поддерживаемые форматы: JPG, AWP
   Полное имя файла path должно содержать расширение .JPG или .AWP

   \param -hImage дескриптор изображения.
   \param -path полное имя файла, в который требуется сохранить изображение. 

	Возвращаемые значения:\n CF_SUCCESS если операция была проведена успешно.\n
	CFF_INVALID_HANDLE  если  hImage == NULL или недействительный\n
	CFCR_NOT_ENOUGH_MEMORY  in case of lack or damage to the memory.
*/
CF_ERROR __EXPORT_TYPE CFSaveImage( CF_IMAGE hImage, char* path );
/*!\fn  CF_ERROR __EXPORT_TYPE CFDIBSize(CF_IMAGE hImage, int* pSize)
   \brief размер памяти, необходимый для сохранения изображения в Windows DIB 

	Возвращает размер дампа памяти, в который можно сохранить изображение hImage. \n
    Предназначена для конвертации изображения, представленного во внутреннем формате библиотеки, в \n
     формат Windows DIB. \n

	\param  hImage - дескриптор изображения.
    \param pSize - размер дампа памяти

	Возвращаемые значения:\n CF_SUCCESS если операция была проведена успешно.\n
	CFF_INVALID_HANDLE  если  hImage == NULL или недействительный\n
    CFF_INVALID_PARAM   в случае, если pSize равен нулю. 
*/
CF_ERROR __EXPORT_TYPE CFDIBSize(CF_IMAGE hImage, int* pSize);
/*!\fn  CF_ERROR __EXPORT_TYPE CFSaveToDIB( CF_IMAGE hImage, CF_BMP* info, CF_BYTE* DIBPixels )
   \brief преобразует изображение в формат Windows DIB
	
	Преобразует изображение в формат Windows DIB.

	\param hImage - дескриптор изображения.
    \param info - описание изображения в формате Windows DIB
    \param DIBPixels - буфер для пикселей изображения. Размер буфера должен\n
    вычисляться с помощью функции CFDIBSize 

	Возвращаемые значения:\n CF_SUCCESS если операция была проведена успешно.\n
	CFF_INVALID_HANDLE  если  hImage == NULL или недействительный\n
    CFF_INVALID_DIB если info == NULL или невозможно выполнить конвертацию. \n
*/
CF_ERROR __EXPORT_TYPE CFSaveToDIB( CF_IMAGE hImage, CF_BMP* info, CF_BYTE* DIBPixels );
/*!\fn CF_ERROR __EXPORT_TYPE CFGetImageParams(CF_IMAGE hImage, CF_IMAGE_PARAMS* pParams)
   \brief возвращает параметры изображения hImage
	
	Возвращает параметры изображения hImage в переменной pParams
	\param hImage - дескриптор изображения.
    \param pParams - параметры изображения

	Возвращаемые значения:\n CF_SUCCESS если операция была проведена успешно.\n
	CFF_INVALID_HANDLE  если  hImage == NULL или недействительный\n
    CFF_INVALID_PARAM  если pParams == NULL 
*/
CF_ERROR __EXPORT_TYPE CFGetImageParams(CF_IMAGE hImage, CF_IMAGE_PARAMS* pParams);
}    	
#endif//_CF_IMAGE_H_