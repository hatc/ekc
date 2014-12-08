/*
//     Chaos Face SDK version 1.8     
//	   Copyright(c) 2008-2010 CCT gmbH, all rights reserved.
//	   Third party copyrights are property of their respective owners.
//     
*/

#include "_cfattrfilter.h"
/*
    возвращает тип фильтра 0 - неизвестно, 1 - Haar, 2- Cesus
*/
static int XMLFilterType(char* path)
{
   TiXmlDocument doc(path);
   if (!doc.LoadFile())
    return 0;

   TiXmlHandle hDoc(&doc);
   TiXmlElement* pElem = NULL;
   TiXmlHandle hRoot(0);

   pElem = hDoc.FirstChildElement().Element();
   if (!pElem)
      return 0;
   const char* name = pElem->Value();
   if (strcmp(name, "AttrFilter") != 0)
      return 0;
   hRoot=TiXmlHandle(pElem);

   pElem = hRoot.FirstChild( "Classes" ).Element();
   if (pElem == NULL)
	return 0;
   // class names
   const char* class_name = pElem->Attribute("class_name1");
   class_name = pElem->Attribute("class_name2");
   // AttrNode
   pElem = hRoot.FirstChild("AttrNode").Element();
   if (pElem == NULL)
      return 0;

   class_name = pElem->Attribute("sensor");
   if (strcmp(class_name, "Haar") == 0 )
            return 1;
   if (strcmp(class_name, "CSSeparate") == 0 )
            return 2;
   if (strcmp(class_name, "AttrCSStrong") == 0 )
            return 3;
   return 0;
}

CF_ERROR __EXPORT_TYPE CFCreateAttrFilter( CF_ATTR_FILTER *hAFilter, char* path )
{
    CF_ERROR status = CF_SUCCESS;
    ILFAttrClassifier* AttrFilter = NULL;
    int XMLType = XMLFilterType(path);
    switch (XMLType)
	{
        case 1:
            AttrFilter = new TLFHaarAttrClassifier();
        break;
        case 2:
            AttrFilter = new  TCSSeparate();
        break;
        case 3:
            AttrFilter = new TAttrCSStrong();
        break;
        default:
            return CFF_CANNOT_LOAD_AFILTER;
    }

    //AttrFilter = new TAttrClassifier();
    if( AttrFilter == NULL )
    {
        status = CFCR_NOT_ENOUGH_MEMORY;
        return status;
    }

    if (!AttrFilter->LoadXML(path))
    {
      status = CFF_CANNOT_LOAD_AFILTER;
      delete AttrFilter;
      return status;
    }

//        AttrFilter->Load(path);
	*hAFilter = AttrFilter;

#ifdef _DEBUG
//    AttrFilter->Save("c:\\test.xml");
//    AttrFilter->LoadXML("c:\\test.xml");
#endif

    return status;
}

CF_ERROR __EXPORT_TYPE CFDestroyAttrFilter( CF_ATTR_FILTER *hAFilter )
{
    CF_ERROR status = CF_SUCCESS;
    ILFAttrClassifier* AttrFilter = (ILFAttrClassifier*)*hAFilter;
    if( AttrFilter == NULL)
    {
        status = CFF_INVALID_HANDLE;
        return status;
    }

    delete AttrFilter;
	return status;
}
/*
    CFApplyAttrFilter 
*/
CF_ERROR __EXPORT_TYPE CFApplyAttrFilter1( CF_ATTR_FILTER hAFilter, CF_IMAGE hImage, CF_FACE_POS Face, int& result )
{
   CF_ERROR status = CF_SUCCESS;
   ILFAttrClassifier* AttrFilter = (ILFAttrClassifier*)hAFilter;
    if( AttrFilter == NULL)
	{
        status = CFF_INVALID_HANDLE;
        return status;
    }
    TLFImage* image = (TLFImage*)hImage;
    if(image == NULL)
    {
        status = CFF_INVALID_HANDLE;
        return status;
    }

    awpPoint p, p1;
    p.X = Face.LeftEye.X;
    p.Y = Face.LeftEye.Y;
    p1.X = Face.RightEye.X;
    p1.Y = Face.RightEye.Y;

    TLFRoi* pRoi = new TLFRoi();
    pRoi->SetRoi(p,p1);
    SLFAttrResult res;
    result = AttrFilter->Classify(image, pRoi, res);

    result = res.m_Result;
    delete pRoi;
    return status;
}

CF_ERROR __EXPORT_TYPE CFApplyAttrFilter( CF_ATTR_FILTER hAFilter, CF_IMAGE hImage, int xPosLe, int yPosLe, int xPosRe, int yPosRe, int& result )
{
   CF_ERROR status = CF_SUCCESS;
   ILFAttrClassifier* AttrFilter = (ILFAttrClassifier*)hAFilter;
    if( AttrFilter == NULL)
	{
        status = CFF_INVALID_HANDLE;
        return status;
    }
    TLFImage* image = (TLFImage*)hImage;
    if(image == NULL)
    {
        status = CFF_INVALID_HANDLE;
        return status;
    }

    awpPoint p, p1;
    p.X = xPosLe;
    p.Y = yPosLe;
    p1.X = xPosRe;
    p1.Y = yPosRe;

    TLFRoi* pRoi = new TLFRoi();
    pRoi->SetRoi(p,p1);
    SLFAttrResult res;
    AttrFilter->Classify(image, pRoi, res);
    result = res.m_Result;

	delete pRoi;
    return status;
}


CF_ERROR __EXPORT_TYPE CFGetAttrParams(CF_ATTR_FILTER hAFilter, CF_ATTR_PARAMS* pParams)
{
   CF_ERROR status = CF_SUCCESS;
   ILFAttrClassifier* AttrFilter = (ILFAttrClassifier*)hAFilter;
    if( AttrFilter == NULL)
    {
        status = CFF_INVALID_HANDLE;
		return status;
    }
    pParams->class_name1 = AttrFilter->GetClassName1();
    pParams->class_name2 = AttrFilter->GetClassName2();
    pParams->threshold1  = AttrFilter->GetThreshold1();
    pParams->threshold2  = AttrFilter->GetThreshold2();
    pParams->classificator =  AttrFilter->GetName();
    pParams->fastcomputing = AttrFilter->GetFastComputingFlag();
    return status;
}
CF_ERROR __EXPORT_TYPE CFSetAttrParams(CF_ATTR_FILTER hAFilter, CF_ATTR_PARAMS* pParams)
{
   CF_ERROR status = CF_SUCCESS;
  ILFAttrClassifier* AttrFilter = (ILFAttrClassifier*)hAFilter;
    if( AttrFilter == NULL)
    {
        status = CFF_INVALID_HANDLE;
        return status;
    }
    AttrFilter->SetThreshold1(pParams->threshold1);
    AttrFilter->SetThreshold2(pParams->threshold2);
    AttrFilter->SetFastComputingFlag(pParams->fastcomputing);
    return status;
}
