/*
        This file is a part of ChaosFace project.
        Copyright (c) Controlling Chaos Technologies 2005-2010
*/
#ifndef  _CF_VERSION_
#define  _CF_VERSION_
/*
        The module version info
*/
typedef struct
{
  char CompanyName[255];
  char FileDescription[255];
  char FileVersion[255];
  char InternalName[255];
  char LegalCopyright[255];
  char LegalTradeMarks[255];
  char OriginalFileName[255];
  char ProductName[255];
  char ProductVersion[255];
  char Comments[255];
}CF_VERSION_INFO;

void CFGetVersion(char* lpModule, CF_VERSION_INFO& cfVersion);

#endif //_CF_VERSION_
