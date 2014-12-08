/*
//     Chaos Face SDK version 1.8     
//	   Copyright(c) 2008-2010 CCT gmbH, all rights reserved.
//	   Third party copyrights are property of their respective owners.
//     
*/

#include "_cfimage.h"

/*
	CFCreateImage
	creates an image class and returns their handler
*/
CF_ERROR __EXPORT_TYPE CFCreateImage( CF_IMAGE *hImage )
{
    CF_ERROR status = CF_SUCCESS;
    TLFImage* image = NULL;
    image = new TLFImage();
    if( image == NULL )
    {
        status = CFCR_NOT_ENOUGH_MEMORY;
        return status;
    }
    *hImage = image;
    return status;
}
/*
	CFDestroyImage
	frees image handler
*/
CF_ERROR __EXPORT_TYPE CFDestroyImage( CF_IMAGE *hImage )
{
    CF_ERROR status = CF_SUCCESS;
    TLFImage* img = (TLFImage*)*hImage;
    try
	{
		delete img;
	}
	catch(...)
	{
		status = CFF_INVALID_HANDLE;
	}
    return status;
}
/*
	CFLoadImage
	Load image from path 
*/
CF_ERROR __EXPORT_TYPE CFLoadImage( CF_IMAGE hImage, char* path )
{
    CF_ERROR status = CF_SUCCESS;
    // �������������� �����.
    TLFImage* image = (TLFImage*)hImage;
    if(image == NULL)
    {
        status = CFF_INVALID_HANDLE;
        return status;
    }

    if(!image->LoadImage( path ))
    {
        status = CFF_CANNOT_LOAD_IMAGE;
        return status;
    }    
	return status;
}
/*
    CFLoadFromDIB
	loads image from Windows DIB 
*/
CF_ERROR __EXPORT_TYPE CFLoadFromDIB( CF_IMAGE hImage, CF_BMP info, BYTE* DIBPixels )
{
    CF_ERROR status = CF_SUCCESS;
    TLFImage* image = (TLFImage*)hImage;
    if(image == NULL)
    {
        status = CFF_INVALID_HANDLE;
        return status;
    }
	awpImage* img = NULL;
    BITMAPINFO lInfo;
    memcpy( &lInfo, &info, sizeof(CF_BMP) );
    if( awpDIBToImage(&lInfo, DIBPixels, &img)!= AWP_OK )
    {
        status = CFF_INVALID_DIB;
        return status;
    }
    try
    {
        image->SetImage( img );
    }
    catch (...)
    {
        status = CFCR_NOT_ENOUGH_MEMORY;
        awpReleaseImage(&img);
        return status;
    }
    awpReleaseImage(&img);
    return status;
}
/*
    �������:
        CFSaveImage
    ����:
        ��������� ����������� � ������� jpeg �� �����
    ���������:
        [in] hImage - ���������� �����������
        [in] path   - ������ ��� ����� ��� ������.
    ������������ ���������:
		CF_SUCCESS - � ������ ������� ������ �����.
        � ��������� ������ ���������� ��� ������.
*/
CF_ERROR __EXPORT_TYPE CFSaveImage( CF_IMAGE hImage, char* path )
{
    CF_ERROR status = CF_SUCCESS;
    TLFImage* image = (TLFImage*)hImage;
    if(image == NULL)
    {
        status = CFF_INVALID_HANDLE;
        return status;
    }
	if(!image->SaveImage( path ))
    {
        status = CFF_CANNOT_SAVE_IMAGE;
        return status;
    }  
    return status;
}
/*
     �������:
        CFSaveToDIB
     ����:
         ��������� ����������� � ������� DIB
     ���������:
       [ in ] hImage  - ���������� �����������
       [ out ] info   - ��������� �� ��������� DIB
       [ out ] DIBPixels - ��������� �� ������� �����������
     ������������ ���������:
     CF_SUCCESS - ������ ����������� ������� �������� � ������.
     � ��������� ������ ���������� ��� ������.
     ���������:
     ������� ������ ���������� ����� ����������� hImage � �������
     DIB, ������� ����� ������� ����� 3 �����
*/
CF_ERROR __EXPORT_TYPE CFSaveToDIB( CF_IMAGE hImage, CF_BMP* info, BYTE* DIBPixels )
{
    CF_ERROR status = CF_SUCCESS;
     if(info == NULL)
    {
        status = CFF_INVALID_DIB;
        return status;
    }

	TLFImage* image = (TLFImage*)hImage;
    if(image == NULL)
    {
        status = CFF_INVALID_HANDLE;
		return status;
    }
    BITMAPINFO lInfo;
    memcpy( &lInfo, info, sizeof( BITMAPINFO ) );
    bool flag = DIBPixels == NULL? false : true;
    if(awpImageToDIB( image->GetImage(), &lInfo, (void**)(&DIBPixels), flag )!=AWP_OK)
	{
        status = CFF_INVALID_DIB;
        return status;
    }
    memcpy( info, &lInfo, sizeof( BITMAPINFO ) );  
    return status;
}
/*
     �������:
        CFDIBSize
     ����:
         ���������� ������ ������, ����������� ��� ����������
         ����������� � ������� DIB
     ���������:
       [ in ] hImage  - ���������� �����������
       [ out ] pSize  - ��������� �� ����������, � ������� ����� ���������
                        �������� ������� ������.
     ������������ ���������:
     CF_SUCCESS - ���������� pSize c������� �������� ������� ������
     � ��������� ������ ���������� ��� ������.
*/
CF_ERROR __EXPORT_TYPE CFDIBSize(CF_IMAGE hImage, int* pSize)
{
    CF_ERROR status = CF_SUCCESS;
    // �������� ������� ���������
    if (pSize == NULL)
    {
        return CFF_INVALID_PARAM;
    }
    TLFImage* image = (TLFImage*)hImage;
    if(image == NULL)
    {
        status = CFF_INVALID_HANDLE;
        *pSize = 0;
		return status;
	}
	*pSize = 0;
	awpImage* pImage = NULL;
	pImage = image->GetImage();
	if (awpCheckImage(pImage) != AWP_OK)
	{
		status = CFF_INVALID_HANDLE;
		*pSize = 0;
		return status;
	}
	int dib_width = ((pImage->sSizeX*24 + 31)/32) * 4; /* aligned to 4 bytes */
	*pSize = dib_width*pImage->sSizeY;
	return status;
}
/*


*/
CF_ERROR __EXPORT_TYPE CFGetImageParams(CF_IMAGE hImage, CF_IMAGE_PARAMS* pParams)
{
	CF_ERROR status = CF_SUCCESS;
	// �������� ������� ���������
	if (pParams == NULL)
	{
		return CFF_INVALID_PARAM;
	}
	TLFImage* image = (TLFImage*)hImage;
	if(image == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	awpImage* pImage = NULL;
	pImage = image->GetImage();
	if (awpCheckImage(pImage) != AWP_OK)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	pParams->width = pImage->sSizeX;
	pParams->height = pImage->sSizeY;
	switch(pImage->bChannels)
	{
		case 1:
			pParams->bpp = 8;
		break;
		case 3:
			pParams->bpp = 24;
		break;
	}
	return status;
}
// ��������� ���������� ����������� �� ������������������ ������. 
CF_ERROR __EXPORT_TYPE CFLoadFromDump(CF_IMAGE hImage, CF_WORD width, CF_WORD height, CF_WORD bpp, CF_BYTE* pixels,  CF_WORD line_width)
{
	CF_ERROR status = CF_SUCCESS;
    // ��������� ��������� �� �����������
	TLFImage* image = (TLFImage*)hImage;
	if(image == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	// �������� ��������� �� �������
	if (pixels == NULL)
	{
		status = CFF_INVALID_PARAM;
		return status;
	}
	// �������� ����� ����� �� �������
	if (bpp != 8 && bpp != 24)
	{
		status = CFF_INVALID_PARAM;
		return status;
	}
	awpImage* tmp = NULL;
	if (awpCreateImage(&tmp, width, height, bpp == 8?1:3, AWP_BYTE) != AWP_OK)
	{
	   status = CFCR_NOT_ENOUGH_MEMORY;
	   return status;
	}
	// ����������� �������� � ����������� tmp
	int bufsize = width*(bpp == 8 ? 1:3);// ����� ���� � ������, � ��������� ������������
	BYTE* b = (BYTE*)tmp->pPixels;
	for (int i = 0; i < height; i++)
	{
		memcpy(b, pixels, bufsize);
		b+= bufsize; 
		pixels += line_width;
	}
	image->SetImage(tmp);
	awpReleaseImage(&tmp);
	return status;
}
// ������ ���������� ����������� � ������������������� ������
CF_ERROR __EXPORT_TYPE CFSaveToDump(CF_IMAGE hImage, CF_WORD* pWidth, CF_WORD* pHeight, CF_WORD* pBpp, CF_BYTE* buffer)
{
	CF_ERROR status = CF_SUCCESS;
	// �������� ������� ��������� ����������� �����������
	TLFImage* image = (TLFImage*)hImage;
	if(image == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	awpImage* pImage = NULL;
	pImage = image->GetImage();
	if (awpCheckImage(pImage) != AWP_OK)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	// �������� ����������� ����������� ����������� 

    //�������� ��������� ���������
	if (pWidth == NULL || pHeight == NULL || pBpp == NULL || buffer == NULL)
	{
		return CFF_INVALID_PARAM;
	}
	// �������� �����������
	*pWidth = pImage->sSizeX;
	*pHeight= pImage->sSizeY;
	*pBpp   = 8*pImage->bChannels;
	int bufsize = pImage->sSizeX*pImage->sSizeY*pImage->bChannels;
	CF_BYTE* b = (CF_BYTE*)pImage->pPixels;
	memcpy(buffer, b, bufsize);
	return status;
}

CF_ERROR __EXPORT_TYPE CFGetDumpSize(CF_IMAGE hImage, CF_DWORD* pDumpSize)
{
	CF_ERROR status = CF_SUCCESS;
	TLFImage* image = (TLFImage*)hImage;
	if(image == NULL)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	awpImage* pImage = NULL;
	pImage = image->GetImage();
	if (awpCheckImage(pImage) != AWP_OK)
	{
		status = CFF_INVALID_HANDLE;
		return status;
	}
	if (pDumpSize == NULL)
		return CFF_INVALID_PARAM;
	//todo: ����� �� ������������, ��� ����������� ����� ���� ���� 
	//������������, ���� ��������, ������� �������� ������ 1 ���� �� ����
	*pDumpSize = pImage->sSizeX*pImage->sSizeY*(pImage->bChannels == 1?1:3);
	return status;
}
