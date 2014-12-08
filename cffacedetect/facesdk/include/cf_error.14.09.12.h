/*
//     Chaos Face SDK version 1.8     
//	   Copyright(c) 2008-2010 CCT gmbH, all rights reserved.
//	   Third party copyrights are property of their respective owners.
//     
*/
#ifndef _CF_ERROR_H_
#define _CF_ERROR_H_
typedef unsigned char    BYTE;
typedef BYTE    CF_ERROR;

#define CF_SUCCESS  0x00 /*Operation completed successfully*/
#define CF_WARNING  0x01 /*operation is completed with a warning*/
#define CF_FAIL     0x02 /**/
#define CF_CR_FAIL  0x03

#define CFF_CANNOT_CREATE_HANDLE    0x12
#define CFF_INVALID_HANDLE          0x22
#define CFF_INVALID_PARAM           0x32
#define CFF_INVALID_SCALE           0x42
#define CFF_INVALID_VALUE           0x52
#define CFF_CANNOT_LOAD_IMAGE       0x62
#define CFF_CANNOT_SAVE_IMAGE       0x72
#define CFF_INVALID_DIB             0x82
#define CFF_CANNOT_OPEN_DB          0x92
#define CFF_LIB_NOT_INIT            0xA2
#define CFF_CANNOT_READ_PARAM       0xB2
#define CFF_CANNOT_LOAD_DETECTOR    0xC2
#define CFF_FINDFACE_FAILED         0xD2
#define CFF_LF_NOT_INIT             0xE2
#define CFF_CANNOT_GET_STROKES		0xF2
#define CFCR_NOT_ENOUGH_MEMORY		90
#define CFF_CANNOT_LOAD_AFILTER		91
#define CFF_CANNOT_SAVE_PARAMS      92
#define CFF_CANNOT_LOAD_PARAMS		93



#define CFW_EMPTY_DB                0x11

#endif