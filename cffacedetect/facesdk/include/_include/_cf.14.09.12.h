#include "cf.h"
#include "LF.h"
#include "awpipl.h"
#include "LFScanners.h"
#include "LFDetector.h"
#include "LFAttrFilter.h"
#include "LFEngine.h"
#include "tinyxml.h"

#ifdef WIN32 
   #pragma link "awpipl2b.lib"
   #pragma link "JPEGLIB.lib"
   #pragma link "awplflibb.lib"
   #pragma link "TinyXML.lib"
#endif

#define MASK_IMG    0x00000010
#define MASK_LF     0x00000020
#define MASK_VLF    0x00000040
#define MASK_GL     0x00000080

#ifdef WIN32
    #define DEFAULT_PATH    ".\\"
    #define DIR_SEPARATOR '\\'
#else
    #define DEFAULT_PATH    "./"
    #define DIR_SEPARATOR '/'
#endif


