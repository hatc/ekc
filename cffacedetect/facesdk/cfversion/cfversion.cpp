/*
        This file is a part of ChaosFace project.
        Copyright (c) Controlling Chaos Technologies 2005-2010
*/
#include "cfversion.h"
#include "string.h"

//
static void _no_version_found(CF_VERSION_INFO& cfVersion)
{
   strcpy(cfVersion.CompanyName, "Unknown");
   strcpy(cfVersion.FileDescription, "Unknown");
   strcpy(cfVersion.FileVersion, "Unknown");
   strcpy(cfVersion.InternalName, "Unknown");
   strcpy(cfVersion.LegalCopyright, "Unknown");
   strcpy(cfVersion.LegalTradeMarks, "Unknown");
   strcpy(cfVersion.OriginalFileName, "Unknown");
   strcpy(cfVersion.ProductName, "Unknown");
   strcpy(cfVersion.ProductVersion, "Unknown");
   strcpy(cfVersion.Comments, "Unknown");
}

#ifdef WIN32
#include "windows.h"

const char* c_InfoStr[10] = {"CompanyName","FileDescription","FileVersion","InternalName","LegalCopyright",
"LegalTradeMarks","OriginalFileName","ProductName","ProductVersion","Comments"};

const char* c_VerSubBlock = "StringFileInfo\\040904E4\\";

static void _win32_version_info(char* lpModule, CF_VERSION_INFO& cfVersion)
{
    unsigned long n = GetFileVersionInfoSize(lpModule, &n);
    char* tmp;
    tmp = (char*)malloc(1024*sizeof(char));
    if (n > 0)
    {
        char *pBuf = (char *) malloc(n);
        if (GetFileVersionInfo(lpModule, 0, n, pBuf))
        {
            //���������� ����������
            char* pValue = NULL; // ����� ��� ������ ����������
            unsigned int nLen = 0; // ����� ������ pValue

            // ��������� ���������� � �������� �����.
            tmp = strcpy(tmp, c_VerSubBlock);
            tmp = strncat(tmp, c_InfoStr[0], strlen(c_VerSubBlock));
            if (VerQueryValue(pBuf, tmp, &(void*)pValue, &nLen))
               strcpy(cfVersion.CompanyName, pValue);

            // ��������� �������� �����
            tmp = strcpy(tmp, c_VerSubBlock);
            tmp = strncat(tmp, c_InfoStr[1], strlen(c_VerSubBlock));
            if (VerQueryValue(pBuf, tmp, &(void*)pValue, &nLen))
               strcpy(cfVersion.FileDescription, pValue);

            // ��������� ������ �����
            tmp = strcpy(tmp, c_VerSubBlock);
            tmp = strncat(tmp, c_InfoStr[2], strlen(c_VerSubBlock));
            if (VerQueryValue(pBuf, tmp, &(void*)pValue, &nLen))
               strcpy(cfVersion.FileVersion, pValue);

            // ��������� ����������� �����
            tmp = strcpy(tmp, c_VerSubBlock);
            tmp = strncat(tmp, c_InfoStr[3], strlen(c_VerSubBlock));
            if (VerQueryValue(pBuf, tmp, &(void*)pValue, &nLen))
               strcpy(cfVersion.InternalName, pValue);

            // �������� ������ ���������
            tmp = strcpy(tmp, c_VerSubBlock);
            tmp = strncat(tmp, c_InfoStr[4], strlen(c_VerSubBlock));
            if (VerQueryValue(pBuf, tmp, &(void*)pValue, &nLen))
               strcpy(cfVersion.LegalCopyright, pValue);

            // ��������� �������� �����
            tmp = strcpy(tmp, c_VerSubBlock);
            tmp = strncat(tmp, c_InfoStr[5], strlen(c_VerSubBlock));
            if (VerQueryValue(pBuf, tmp, &(void*)pValue, &nLen))
               strcpy(cfVersion.LegalTradeMarks, pValue);

            // ��������� ������������� ����� �����
            tmp = strcpy(tmp, c_VerSubBlock);
            tmp = strncat(tmp, c_InfoStr[6], strlen(c_VerSubBlock));
            if (VerQueryValue(pBuf, tmp, &(void*)pValue, &nLen))
               strcpy(cfVersion.OriginalFileName, pValue);

            // ��������� �������� ��������
            tmp = strcpy(tmp, c_VerSubBlock);
            tmp = strncat(tmp, c_InfoStr[7], strlen(c_VerSubBlock));
            if (VerQueryValue(pBuf, tmp, &(void*)pValue, &nLen))
               strcpy(cfVersion.ProductName, pValue);

            // ��������� ������ ��������
            tmp = strcpy(tmp, c_VerSubBlock);
            tmp = strncat(tmp, c_InfoStr[8], strlen(c_VerSubBlock));
            if (VerQueryValue(pBuf, tmp, &(void*)pValue, &nLen))
               strcpy(cfVersion.ProductVersion, pValue);

            // ��������� ������������ 
            tmp = strcpy(tmp, c_VerSubBlock);
            tmp = strncat(tmp, c_InfoStr[9], strlen(c_VerSubBlock));
            if (VerQueryValue(pBuf, tmp, &(void*)pValue, &nLen))
                strcpy(cfVersion.Comments, pValue);
        }
        else
             _no_version_found(cfVersion);
    }
    else
     _no_version_found(cfVersion);
    free(tmp);
}
#endif

void CFGetVersion(char* lpModule, CF_VERSION_INFO& cfVersion)
{
#ifdef WIN32
    _win32_version_info(lpModule, cfVersion);
#else
    _no_version_found();
#endif
}

