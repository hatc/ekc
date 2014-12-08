// Copyright (C) 2012-2013 Yuri Agafonov
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#include "stdafx.h"

#include "facefeaturesextractor.h"

#include <string_util.h>
#include <file_util.h>
#include <scoped_buf.hpp>
#include <trace.h>

#include <draw_util.h>

#include "cf_error_helper.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cffacedetect.h>
#include <cfattrfilter.h>
#include <cfobjectdetect.h>

/*cf_error_helper.hpp impl:*/
COMPILE_ASSERT(sizeof(CF_ERROR) == sizeof(unsigned char), cf_error_helper_invalid_type);
COMPILE_ASSERT(CF_WARNING == 0x01, cf_error_helper_invalid_value);

#ifdef __EXPORT_TYPE
#undef __EXPORT_TYPE
#endif
#define __EXPORT_TYPE __stdcall 

typedef CF_ERROR (__EXPORT_TYPE *CFCreateLocateFacePtr)(CF_LOCATE_FACE *hLocateFace, char *path);
typedef CF_ERROR (__EXPORT_TYPE *CFDestroyLocateFacePtr)(CF_LOCATE_FACE *hLocateFace);
typedef CF_ERROR (__EXPORT_TYPE *CFFindFacesPtr)(CF_LOCATE_FACE hLocateFace, CF_IMAGE hImage);
typedef CF_ERROR (__EXPORT_TYPE *CFGetFacesCountPtr)(CF_LOCATE_FACE hLocateFace, int *count);
typedef CF_ERROR (__EXPORT_TYPE *CFGetFacePosPtr)(CF_LOCATE_FACE hLocateFace, int index, int *px_pos, int *py_pos, int *p_radius);
typedef CF_ERROR (__EXPORT_TYPE *CFGetEyePosPtr)(CF_LOCATE_FACE hLocateFace, int index, CF_POINT *pLe, CF_POINT *pRe);
typedef CF_ERROR (__EXPORT_TYPE *CFGetLocateFaceParamsPtr)(CF_LOCATE_FACE hLocateFace, CF_LOCATE_FACE_PARAMS *pParams);
typedef CF_ERROR (__EXPORT_TYPE *CFSetLocateFaceParamsPtr)(CF_LOCATE_FACE hLocateFace, CF_LOCATE_FACE_PARAMS *pParams);
typedef CF_ERROR (__EXPORT_TYPE *CFDrawFacesPosPtr)(CF_LOCATE_FACE hLocateFace, CF_IMAGE hImage, CF_COLOR color, CF_SHAPE shape, float radius, bool draw_eye_pos);

typedef CF_ERROR (__EXPORT_TYPE *CFCreateImagePtr)(CF_IMAGE *hImage);
typedef CF_ERROR (__EXPORT_TYPE *CFDestroyImagePtr)(CF_IMAGE *hImage);
typedef CF_ERROR (__EXPORT_TYPE *CFLoadFromDIBPtr)(CF_IMAGE hImage, CF_BMP info, CF_BYTE *DIBPixels);
typedef CF_ERROR (__EXPORT_TYPE *CFLoadImagePtr)(CF_IMAGE hImage, char *path);
typedef CF_ERROR (__EXPORT_TYPE *CFSaveImagePtr)(CF_IMAGE hImage, char *path);

typedef CF_ERROR (__EXPORT_TYPE *CFCreateAttrFilterPtr)(CF_ATTR_FILTER *hAFilter, char *path);
typedef CF_ERROR (__EXPORT_TYPE *CFDestroyAttrFilterPtr)(CF_ATTR_FILTER *hAFilter);
//CF_ERROR __EXPORT_TYPE CFGetAttrParams(CF_ATTR_FILTER hAFilter, CF_ATTR_PARAMS* pParams);
//CF_ERROR __EXPORT_TYPE CFSetAttrParams(CF_ATTR_FILTER hAFilter, CF_ATTR_PARAMS* pParams);
typedef CF_ERROR (__EXPORT_TYPE *CFApplyAttrFilterPtr)(CF_ATTR_FILTER hAFilter, CF_IMAGE hImage, int xPosLe, int yPosLe, int xPosRe, int yPosRe, int& result);

typedef CF_ERROR (__EXPORT_TYPE *CFCreateObjectDetectorPtr)(CF_OBJECTDETECTOR *hObjectDetector, char *path);
typedef CF_ERROR (__EXPORT_TYPE *CFDestroyObjectDetectorPtr)(CF_OBJECTDETECTOR *hObjectDetector);
typedef CF_ERROR (__EXPORT_TYPE *CFDetectObjectPtr)(CF_OBJECTDETECTOR hObjectDetector, CF_IMAGE hImage);
typedef CF_ERROR (__EXPORT_TYPE *CFGetObjectCountPtr)(CF_OBJECTDETECTOR hObjectDetector, int *count);
typedef CF_ERROR (__EXPORT_TYPE *CFGetObjectRectPtr)(CF_OBJECTDETECTOR hObjectDetector, int index, int *r_top, int *r_left, int *r_right, int *r_bottom);

inline HMODULE LoadLibrary(std::wstring const &dll_path) {
	HMODULE hmodule = (HMODULE)0;

	bool is_dir;
	bool dll_found = cpcl::ExistFilePath(dll_path, &is_dir);
	if (!dll_found || is_dir) {
		cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
			"LoadLibrary(): module '%s' not found",
			cpcl::ConvertUTF16_CP1251(dll_path).c_str());
	} else {
		hmodule = ::LoadLibraryW(dll_path.c_str());
		if (!hmodule) {
			cpcl::ErrorSystem(::GetLastError(),
				"LoadLibrary(): unable to load module '%s':",
				cpcl::ConvertUTF16_CP1251(dll_path).c_str());
		}
	}

	return hmodule;
}

struct CFLibraryAPI {
	CFCreateLocateFacePtr CFCreateLocateFace;
	CFDestroyLocateFacePtr CFDestroyLocateFace;
	CFFindFacesPtr CFFindFaces;
	CFGetFacesCountPtr CFGetFacesCount;
	CFGetFacePosPtr CFGetFacePos;
	CFGetEyePosPtr CFGetEyePos;
	CFGetLocateFaceParamsPtr CFGetLocateFaceParams;
	CFSetLocateFaceParamsPtr CFSetLocateFaceParams;
	
	CFCreateImagePtr CFCreateImage;
	CFDestroyImagePtr CFDestroyImage;
	CFLoadFromDIBPtr CFLoadFromDIB;
	
	CFCreateAttrFilterPtr CFCreateAttrFilter;
	CFDestroyAttrFilterPtr CFDestroyAttrFilter;
	CFApplyAttrFilterPtr CFApplyAttrFilter;

	CFCreateObjectDetectorPtr CFCreateObjectDetector;
	CFDestroyObjectDetectorPtr CFDestroyObjectDetector;
	CFDetectObjectPtr CFDetectObject;
	CFGetObjectCountPtr CFGetObjectCount;
	CFGetObjectRectPtr CFGetObjectRect;

	bool operator!() const {
		return !CFCreateImage || !CFDestroyImage || !CFLoadFromDIB
			|| !CFCreateLocateFace || !CFDestroyLocateFace || !CFFindFaces
			|| !CFGetFacesCount || !CFGetFacePos || !CFGetEyePos
			|| !CFGetLocateFaceParams || !CFSetLocateFaceParams
			|| !CFCreateAttrFilter || !CFDestroyAttrFilter || !CFApplyAttrFilter
			|| !CFCreateObjectDetector || !CFDestroyObjectDetector
			|| !CFDetectObject || !CFGetObjectCount || !CFGetObjectRect;
	}

	static CFLibraryAPI Load(std::wstring const &dirname);
private:
	struct FreeLibraryGuard {
		HMODULE hmodule;
		
		explicit FreeLibraryGuard(HMODULE hmodule_) : hmodule(hmodule_)
		{}
		~FreeLibraryGuard() {
			if (hmodule)
				::FreeLibrary(hmodule);
		}
		
		void Disarm() {
			hmodule = (HMODULE)0;
		}
		operator HMODULE() const {
			return hmodule;
		}
		bool operator!() const {
			return !hmodule;
		}
	private:
		DISALLOW_COPY_AND_ASSIGN(FreeLibraryGuard);
	};

	CFLibraryAPI() : CFCreateImage(0), CFDestroyImage(0), CFLoadFromDIB(0), CFCreateLocateFace(0), CFDestroyLocateFace(0),
		CFFindFaces(0), CFGetFacesCount(0), CFGetFacePos(0), CFGetEyePos(0), CFGetLocateFaceParams(0),
		CFSetLocateFaceParams(0), CFCreateAttrFilter(0), CFDestroyAttrFilter(0), CFApplyAttrFilter(0),
		CFCreateObjectDetector(0), CFDestroyObjectDetector(0), CFDetectObject(0), CFGetObjectCount(0),
		CFGetObjectRect(0)
	{}
};

static cpcl::WStringPiece const CFIMAGE_MODULE_NAME = cpcl::WStringPieceFromLiteral(L"cfimage.dll");
static cpcl::WStringPiece const CFFACEDETECT_MODULE_NAME = cpcl::WStringPieceFromLiteral(L"cffacedetect.dll");
static cpcl::WStringPiece const CFATTRFILTER_MODULE_NAME = cpcl::WStringPieceFromLiteral(L"cfattrfilter.dll");
static cpcl::WStringPiece const CFOBJECTDETECT_MODULE_NAME = cpcl::WStringPieceFromLiteral(L"cfobjectdetect.dll");
inline std::wstring Path_Combine(std::wstring const &dirname, cpcl::WStringPiece const &basename) {
	std::wstring filepath; filepath.reserve(dirname.size() + basename.size() + 1);
	filepath.append(dirname); filepath.append(basename.data(), basename.size());
	return filepath;
}
inline CFLibraryAPI CFLibraryAPI::Load(std::wstring const &dirname) {
	CFLibraryAPI r;
	
	std::wstring cfimage_dll_path = Path_Combine(dirname, CFIMAGE_MODULE_NAME);
	CFLibraryAPI::FreeLibraryGuard cfimage_module(LoadLibrary(cfimage_dll_path));
	if (!cfimage_module)
		return r;
	{
		r.CFCreateImage = (CFCreateImagePtr)::GetProcAddress(cfimage_module, "CFCreateImage");
		r.CFDestroyImage = (CFDestroyImagePtr)::GetProcAddress(cfimage_module, "CFDestroyImage");
		r.CFLoadFromDIB = (CFLoadFromDIBPtr)::GetProcAddress(cfimage_module, "CFLoadFromDIB");

		if (!r.CFCreateImage || !r.CFDestroyImage || !r.CFLoadFromDIB) {
			cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
				"CFLibraryAPI::Load(): failed to load module '%s'",
				cpcl::ConvertUTF16_CP1251(cfimage_dll_path).c_str());
			return r;
		}
	}

	std::wstring cffacedetect_dll_path = Path_Combine(dirname, CFFACEDETECT_MODULE_NAME);
	CFLibraryAPI::FreeLibraryGuard cffacedetect_module(LoadLibrary(cffacedetect_dll_path));
	if (!cffacedetect_module)
		return r;
	{
		r.CFCreateLocateFace = (CFCreateLocateFacePtr)::GetProcAddress(cffacedetect_module, "CFCreateLocateFace");
		r.CFDestroyLocateFace = (CFDestroyLocateFacePtr)::GetProcAddress(cffacedetect_module, "CFDestroyLocateFace");
		r.CFFindFaces = (CFFindFacesPtr)::GetProcAddress(cffacedetect_module, "CFFindFaces");
		r.CFGetFacesCount = (CFGetFacesCountPtr)::GetProcAddress(cffacedetect_module, "CFGetFacesCount");
		r.CFGetFacePos = (CFGetFacePosPtr)::GetProcAddress(cffacedetect_module, "CFGetFacePos");
		r.CFGetEyePos = (CFGetEyePosPtr)::GetProcAddress(cffacedetect_module, "CFGetEyePos");
		r.CFGetLocateFaceParams = (CFGetLocateFaceParamsPtr)::GetProcAddress(cffacedetect_module, "CFGetLocateFaceParams");
		r.CFSetLocateFaceParams = (CFSetLocateFaceParamsPtr)::GetProcAddress(cffacedetect_module, "CFSetLocateFaceParams");

		if (!r.CFCreateLocateFace || !r.CFDestroyLocateFace || !r.CFFindFaces
			|| !r.CFGetFacesCount || !r.CFGetFacePos || !r.CFGetEyePos
			|| !r.CFGetLocateFaceParams || !r.CFSetLocateFaceParams) {
			cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
				"CFLibraryAPI::Load(): failed to load module '%s'",
				cpcl::ConvertUTF16_CP1251(cffacedetect_dll_path).c_str());
			return r;
		}
	}

	std::wstring cfattrfilter_dll_path = Path_Combine(dirname, CFATTRFILTER_MODULE_NAME);
	CFLibraryAPI::FreeLibraryGuard cfattrfilter_module(LoadLibrary(cfattrfilter_dll_path));
	if (!cfattrfilter_module)
		return r;
	{
		r.CFCreateAttrFilter = (CFCreateAttrFilterPtr)::GetProcAddress(cfattrfilter_module, "CFCreateAttrFilter");
		r.CFDestroyAttrFilter = (CFDestroyAttrFilterPtr)::GetProcAddress(cfattrfilter_module, "CFDestroyAttrFilter");
		r.CFApplyAttrFilter = (CFApplyAttrFilterPtr)::GetProcAddress(cfattrfilter_module, "CFApplyAttrFilter");

		if (!r.CFCreateAttrFilter || !r.CFDestroyAttrFilter || !r.CFApplyAttrFilter) {
			cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
				"CFLibraryAPI::Load(): failed to load module '%s'",
				cpcl::ConvertUTF16_CP1251(cfattrfilter_dll_path).c_str());
			return r;
		}
	}

	std::wstring cfobjectdetect_dll_path = Path_Combine(dirname, CFOBJECTDETECT_MODULE_NAME);
	CFLibraryAPI::FreeLibraryGuard cfobjectdetect_module(LoadLibrary(cfobjectdetect_dll_path));
	if (!cfobjectdetect_module)
		return r;
	{
		r.CFCreateObjectDetector = (CFCreateObjectDetectorPtr)::GetProcAddress(cfobjectdetect_module, "CFCreateObjectDetector");
		r.CFDestroyObjectDetector = (CFDestroyObjectDetectorPtr)::GetProcAddress(cfobjectdetect_module, "CFDestroyObjectDetector");
		r.CFDetectObject = (CFDetectObjectPtr)::GetProcAddress(cfobjectdetect_module, "CFDetectObject");
		r.CFGetObjectCount = (CFGetObjectCountPtr)::GetProcAddress(cfobjectdetect_module, "CFGetObjectCount");
		r.CFGetObjectRect = (CFGetObjectRectPtr)::GetProcAddress(cfobjectdetect_module, "CFGetObjectRect");

		if (!r.CFCreateObjectDetector || !r.CFDestroyObjectDetector || !r.CFDetectObject
			|| !r.CFGetObjectCount || !r.CFGetObjectRect) {
			cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
				"CFLibraryAPI::Load(): failed to load module '%s'",
				cpcl::ConvertUTF16_CP1251(cfobjectdetect_dll_path).c_str());
			return r;
		}
	}

	cfimage_module.Disarm();
	cffacedetect_module.Disarm();
	cfattrfilter_module.Disarm();
	cfobjectdetect_module.Disarm();
	return r;
}

struct CFImage {
	CFLibraryAPI library_api;
	CF_IMAGE cf_image;

	~CFImage() {
		library_api.CFDestroyImage(&cf_image);
	}
	operator CF_IMAGE() const {
		return cf_image;
	}

	static CFImage* Copy(CFLibraryAPI library_api, BITMAPINFO *bitmap_info, unsigned char *data);
private:
	explicit CFImage(CFLibraryAPI library_api_, CF_IMAGE cf_image_) : library_api(library_api_), cf_image(cf_image_)
	{}
	DISALLOW_COPY_AND_ASSIGN(CFImage);
};

inline CFImage* CFImage::Copy(CFLibraryAPI library_api, BITMAPINFO *bitmap_info, unsigned char *data) {
	CF_IMAGE cf_image = (CF_IMAGE)0;
	THROW_CF_ERROR(library_api.CFCreateImage(&cf_image));
	if (!cf_image) {
		awp_exception::throw_formatted(awp_exception(CF_FAIL),
			"CFImage::Copy(): cf_image is null");
	}

	std::auto_ptr<CFImage> r(new CFImage(library_api, cf_image));

	THROW_CF_ERROR(library_api.CFLoadFromDIB(cf_image, *bitmap_info, data));

	return r.release();
}

struct CFLocateFace {
	enum CF_LOCATE_FACE_SIZE { CF_FACE_SMALL = CFSMALL, CF_FACE_MEDIUM = CFMEDIUM, CF_FACE_BIG = CFBIG };
	CFLibraryAPI library_api;
	CF_LOCATE_FACE cf_locate_face;

	~CFLocateFace() {
		library_api.CFDestroyLocateFace(&cf_locate_face);
	}
	operator CF_LOCATE_FACE() const {
		return cf_locate_face;
	}

	CF_LOCATE_FACE_SIZE FaceSize();
	void FaceSize(CF_LOCATE_FACE_SIZE v);
	bool ColorFilter();
	void ColorFilter(bool v);
	bool FastComputing();
	void FastComputing(bool v);

	static CFLocateFace* Create(CFLibraryAPI library_api, cpcl::StringPiece const &cf_engine_path);
	static bool TryParse(cpcl::StringPiece const &s, CF_LOCATE_FACE_SIZE *r);
	static bool TryParse(cpcl::WStringPiece const &s, CF_LOCATE_FACE_SIZE *r);
private:
	CF_LOCATE_FACE_SIZE face_size;
	bool color_filter;
	bool fast_computing;
	bool params_retrieved;

	explicit CFLocateFace(CFLibraryAPI library_api_, CF_LOCATE_FACE cf_locate_face_) : library_api(library_api_), cf_locate_face(cf_locate_face_),
		params_retrieved(false)
	{}
	DISALLOW_COPY_AND_ASSIGN(CFLocateFace);

	void RetrieveParams();
	void UpdateParams();
};

inline void CFLocateFace::RetrieveParams() {
	if (!params_retrieved) {
		CF_LOCATE_FACE_PARAMS params;
		THROW_CF_ERROR(library_api.CFGetLocateFaceParams(cf_locate_face, &params));
		
		switch (params.face_sizes) {
			case CFSMALL:
				face_size = CF_FACE_SMALL;
				break;
			case CFMEDIUM:
				face_size = CF_FACE_MEDIUM;
				break;
			case CFBIG:
				face_size = CF_FACE_BIG;
				break;
			default:
				awp_exception::throw_formatted(awp_exception(CFF_CANNOT_LOAD_PARAMS),
					"CFLocateFace::RetrieveParams(): invalid parameter value for params.face_sizes: %d",
					(int)params.face_sizes);
		}
		color_filter = params.use_color;
		fast_computing = params.fast_computing;

		params_retrieved = true;
	}
}
inline void CFLocateFace::UpdateParams() {
	CF_LOCATE_FACE_PARAMS params;
	params.face_sizes = (CF_FACE_SIZES)face_size;
	params.use_color = color_filter;
	params.fast_computing = fast_computing;
	THROW_CF_ERROR(library_api.CFSetLocateFaceParams(cf_locate_face, &params));
}
inline CFLocateFace::CF_LOCATE_FACE_SIZE CFLocateFace::FaceSize() {
	RetrieveParams();
	return face_size;
}
inline void CFLocateFace::FaceSize(CFLocateFace::CF_LOCATE_FACE_SIZE v) {
	RetrieveParams();
	if (face_size != v) {
		face_size = v;
		UpdateParams();
	}
}
inline bool CFLocateFace::ColorFilter() {
	RetrieveParams();
	return color_filter;
}
inline void CFLocateFace::ColorFilter(bool v) {
	RetrieveParams();
	if (color_filter != v) {
		color_filter = v;
		UpdateParams();
	}
}
inline bool CFLocateFace::FastComputing() {
	RetrieveParams();
	return fast_computing;
}
inline void CFLocateFace::FastComputing(bool v) {
	RetrieveParams();
	if (fast_computing != v) {
		fast_computing = v;
		UpdateParams();
	}
}

template<class Storage>
inline char* copy_str(Storage &buf, cpcl::StringPiece const &str) {
	memcpy(buf.Alloc(str.size() + 1), str.data(), str.size());
	*(buf.Data() + str.size()) = 0;
	return buf.Data();
}
inline CFLocateFace* CFLocateFace::Create(CFLibraryAPI library_api, cpcl::StringPiece const &cf_engine_path) {
	CF_LOCATE_FACE cf_locate_face = (CF_LOCATE_FACE)0;
	cpcl::ScopedBuf<char, 0x100> buf;
	THROW_CF_ERROR(library_api.CFCreateLocateFace(&cf_locate_face, copy_str(buf, cf_engine_path)));
	if (!cf_locate_face) {
		awp_exception::throw_formatted(awp_exception(CF_FAIL),
			"CFLocateFace::Create('%s'): cf_locate_face is null",
			buf.Data());
	}

	return new CFLocateFace(library_api, cf_locate_face);
}

template<class String>
inline bool TryParse_(String const &s, String *head, String *tail, CFLocateFace::CF_LOCATE_FACE_SIZE *r) {
	CFLocateFace::CF_LOCATE_FACE_SIZE values[] = { CFLocateFace::CF_FACE_SMALL, CFLocateFace::CF_FACE_MEDIUM, CFLocateFace::CF_FACE_BIG };
	for (size_t i = 0; (i < arraysize(values)) && (head != tail); ++i, ++head) {
		if (cpcl::striASCIIequal(s, *head)) {
			if (r)
				*r = values[i];
			return true;
		}
	}
	return false;
}
inline bool CFLocateFace::TryParse(cpcl::StringPiece const &s, CFLocateFace::CF_LOCATE_FACE_SIZE *r) {
	cpcl::StringPiece face_size[] = {
		cpcl::StringPieceFromLiteral("small"),
		cpcl::StringPieceFromLiteral("medium"),
		cpcl::StringPieceFromLiteral("big")
	};
	return TryParse_(s, face_size, face_size + arraysize(face_size), r);
}
inline bool CFLocateFace::TryParse(cpcl::WStringPiece const &s, CFLocateFace::CF_LOCATE_FACE_SIZE *r) {
	cpcl::WStringPiece face_size[] = {
		cpcl::WStringPieceFromLiteral(L"small"),
		cpcl::WStringPieceFromLiteral(L"medium"),
		cpcl::WStringPieceFromLiteral(L"big")
	};
	return TryParse_(s, face_size, face_size + arraysize(face_size), r);
}

struct CFAttrFilter {
	CFLibraryAPI library_api;
	CF_ATTR_FILTER cf_attr_filter;

	~CFAttrFilter() {
		library_api.CFDestroyAttrFilter(&cf_attr_filter);
	}
	operator CF_ATTR_FILTER() const {
		return cf_attr_filter;
	}

	static CFAttrFilter* Create(CFLibraryAPI library_api, cpcl::StringPiece const &cf_config_path);
private:
	CFAttrFilter(CFLibraryAPI library_api_, CF_ATTR_FILTER cf_attr_filter_) : library_api(library_api_), cf_attr_filter(cf_attr_filter_)
	{}
	DISALLOW_COPY_AND_ASSIGN(CFAttrFilter);
};

inline CFAttrFilter* CFAttrFilter::Create(CFLibraryAPI library_api, cpcl::StringPiece const &cf_config_path) {
	CF_ATTR_FILTER cf_attr_filter = (CF_ATTR_FILTER)0;
	cpcl::ScopedBuf<char, 0x100> buf;
	THROW_CF_ERROR(library_api.CFCreateAttrFilter(&cf_attr_filter, copy_str(buf, cf_config_path)));
	if (!cf_attr_filter) {
		awp_exception::throw_formatted(awp_exception(CF_FAIL),
			"CFAttrFilter::Create('%s'): cf_attr_filter is null",
			buf.Data());
	}

	return new CFAttrFilter(library_api, cf_attr_filter);
}

double const FaceFeaturesExtractor::FaceFeature::RADIUS_RATIO = 1.2;
// size_t const FaceFeaturesExtractor::FaceFeature::DATA_SIZE = sizeof(unsigned int) * 2 + sizeof(int) * 3;

void FaceFeaturesExtractor::FaceFeature::Draw(plcl::RenderingData *data, unsigned int r, unsigned int g, unsigned int b) {
	if (is_object) {
		plcl::DrawSelectRectDontUpScale(data, page_width, page_height,
		left_eye_x, left_eye_y, // left,top
		right_eye_x, right_eye_y, // right,bottom
		r, g, b);
		return;
	}

	//if (!((int)page_width & (int)page_height & x & y & radius)) {
	if ((0 == page_width) || (0 == page_height) || (0 == x) || (0 == y) || (0 == radius)) {
		cpcl::Warning(cpcl::StringPieceFromLiteral("FaceFeature::Draw(): no data"));
		return;
	}

	//plcl::DrawSelectRect(data, page_width, page_height,
	//	double(x) - RADIUS_RATIO * double(radius), double(y) + RADIUS_RATIO * double(radius), // left,top
	//	double(x) + RADIUS_RATIO * double(radius), double(y) - RADIUS_RATIO * double(radius), // right,bottom
	//	r, g, b);
	plcl::DrawSelectRectDontUpScale(data, page_width, page_height,
		double(x) - RADIUS_RATIO * double(radius), double(y) + RADIUS_RATIO * double(radius), // left,top
		double(x) + RADIUS_RATIO * double(radius), double(y) - RADIUS_RATIO * double(radius), // right,bottom
		r, g, b);
}

bool FaceFeaturesExtractor::FaceFeature::GetData(unsigned char *buf, size_t size) {
	// if (size != (sizeof(unsigned int) * 2 + sizeof(int) * 3))
	if (size != DATA_SIZE) // why not needed FaceFeaturesExtractor::FaceFeature:: i.e. full qualified namespace - because first compiler search in current namespace
		return false; // i.e. identifier 'DATA_SIZE' compiler first search in FaceFeaturesExtractor::FaceFeature::
	
	unsigned int *page_size = (unsigned int*)buf;
	page_size[0] = page_width; page_size[1] = page_height;
	
	int *coord = (int*)(page_size + 2);
	coord[0] = x; coord[1] = y; coord[2] = radius;

	coord[3] = left_eye_x; coord[4] = left_eye_y;
	coord[5] = right_eye_x; coord[6] = right_eye_y;

	*(bool*)(coord + 7) = is_object;

	return true;
}

bool FaceFeaturesExtractor::FaceFeature::SetData(unsigned char const *buf, size_t size) {
	// if (size != (sizeof(unsigned int) * 2 + sizeof(int) * 3))
	if (size != DATA_SIZE)
		return false;

	unsigned int const *page_size = (unsigned int const*)buf;
	page_width = page_size[0]; page_height = page_size[1];

	int const *coord = (int const *)(page_size + 2);
	x = coord[0]; y = coord[1]; radius = coord[2];

	left_eye_x = coord[3]; left_eye_y = coord[4];
	right_eye_x = coord[5]; right_eye_y = coord[6];

	is_object = *(bool const*)(coord + 7);

	return true;
}

void FaceFeaturesExtractor::FaceFeature::GetROI(unsigned long *left, unsigned long *top, unsigned long *right, unsigned long *bottom) {
	if (is_object) {
		if (left)
			*left = static_cast<unsigned long>(left_eye_x);
		if (top)
			*top = static_cast<unsigned long>(left_eye_y);
		if (right)
			*right = static_cast<unsigned long>(right_eye_x);
		if (bottom)
			*bottom = static_cast<unsigned long>(right_eye_y);
		return;
	}

	if (left)
		*left = static_cast<unsigned long>(double(x) - RADIUS_RATIO * double(radius));
	if (top)
		*top = static_cast<unsigned long>(double(y) + RADIUS_RATIO * double(radius));
	if (right)
		*right = static_cast<unsigned long>(double(x) + RADIUS_RATIO * double(radius));
	if (bottom)
		*bottom = static_cast<unsigned long>(double(y) - RADIUS_RATIO * double(radius));
}

FaceFeaturesExtractor::AttributeFilter::AttributeFilter()
{}
FaceFeaturesExtractor::AttributeFilter::~AttributeFilter()
{}

bool FaceFeaturesExtractor::AttributeFilter::Classify(CFImage *image, FaceFeaturesExtractor::FaceFeature &feature) {
	if (feature.is_object)
		return false;

	CFLibraryAPI library_api = attr_filter->library_api;

	int result(INT_MAX);
	THROW_CF_ERROR(library_api.CFApplyAttrFilter(*attr_filter, *image,
		feature.left_eye_x, feature.left_eye_y, feature.right_eye_x, feature.right_eye_y,
		result));

	std::map<int, std::wstring>::const_iterator i = classes.find(result);
	if (i != classes.end()) {
		std::map<GUID, std::wstring>::_Pairib j = feature.classes.insert(std::make_pair(classifier_id, (*i).second));
		if (!j.second)
			(*j.first).second = (*i).second;
		return true;
	}
	return false;
}
bool FaceFeaturesExtractor::AttributeFilter::Classify(plcl::BitmapInfo *data, FaceFeaturesExtractor::FaceFeature &feature) {
	std::auto_ptr<CFImage> image(CFImage::Copy(attr_filter->library_api, data->Info(), data->Scanline(0)));
	return this->Classify(image.get(), feature);
}

bool FaceFeaturesExtractor::AttributeFilter::GetData(unsigned char *buf, size_t size) {
	if (size != DATA_SIZE) // why not needed FaceFeaturesExtractor::AttributeFilter:: i.e. full qualified namespace - because first compiler search in current namespace
		return false; // i.e. identifier 'DATA_SIZE' compiler first search in FaceFeaturesExtractor::FaceFeature::
	
	int *magic_number = (int*)buf;
	magic_number[0] = MAGIC_NUMBER;
	
	AttributeFilter **attribute_filter = (AttributeFilter**)(magic_number + 1);
	attribute_filter[0] = this;

	return true;
}

/*FaceFeaturesExtractor::AttributeFilter* FaceFeaturesExtractor::AttributeFilter::Create(CFLibraryAPI library_api, AttributeInfo const &attribute_info) {
	std::auto_ptr<AttributeFilter> r;
	if (!!library_api) {
		r.reset(new AttributeFilter());
		r->classifier_id = attribute_info.id;
		r->classifier_name = attribute_info.name;
		r->classes = attribute_info.classes;
		r->attr_filter.reset(CFAttrFilter::Create(library_api, cpcl::ConvertUTF16_CP1251(attribute_info.config)));
	}
	return r.release();
}*/

bool FaceFeaturesExtractor::AttributeFilter::Classify(std::vector<FaceFeaturesExtractor::AttributeFilter*> classifiers, plcl::BitmapInfo *data, FaceFeaturesExtractor::FaceFeature &feature) {
	if (classifiers.empty())
		return false;

	std::auto_ptr<CFImage> image(CFImage::Copy(classifiers[0]->attr_filter->library_api, data->Info(), data->Scanline(0)));
	bool r(false);
	for (size_t i = 0; i < classifiers.size(); ++i) {
		r |= classifiers[i]->Classify(image.get(), feature);
	}
	return r;
}

FaceFeaturesExtractor::AttributeFilter* FaceFeaturesExtractor::AttributeFilter::Marshal(unsigned char const *buf, size_t size) {
	if (size != DATA_SIZE)
		return (AttributeFilter*)0;

	int *magic_number = (int*)buf;
	if (magic_number[0] != MAGIC_NUMBER)
		return (AttributeFilter*)0;
	
	AttributeFilter **attribute_filter = (AttributeFilter**)(magic_number + 1);
	return attribute_filter[0];
}

FaceFeaturesExtractor::FaceFeaturesExtractor()
{}
FaceFeaturesExtractor::~FaceFeaturesExtractor()
{}

// A34CDBD5-0EF2-45fb-890A-CEA87E8F4459
GUID const FaceFeaturesExtractor::IID_FaceFeaturesExtractor = { 0xa34cdbd5, 0x0ef2, 0x45fb, { 0x89, 0x0a, 0xce, 0xa8, 0x7e, 0x8f, 0x44, 0x59 } };

std::vector<FaceFeaturesExtractor::FaceFeature> FaceFeaturesExtractor::Find(plcl::BitmapInfo *data) {
	unsigned int const page_width(data->Width()), page_height(data->Height());
	std::vector<FaceFeature> r;
	
	CFLibraryAPI library_api = locate_face->library_api;
	std::auto_ptr<CFImage> image(CFImage::Copy(library_api, data->Info(), data->Scanline(0)));
	
	THROW_CF_ERROR(library_api.CFFindFaces(*locate_face, *image));
	
	int n(0);
	THROW_CF_ERROR(library_api.CFGetFacesCount(*locate_face, &n));
	for (int i = 0; i < n; ++i) {
		int x, y, radius;
		THROW_CF_ERROR(library_api.CFGetFacePos(*locate_face, i, &x, &y, &radius));
		CF_POINT left_eye, right_eye;
		THROW_CF_ERROR(library_api.CFGetEyePos(*locate_face, i, &left_eye, &right_eye));
		
		r.push_back(FaceFeaturesExtractor::FaceFeature(page_width, page_height, x, y, radius,
			left_eye.X, left_eye.Y, right_eye.X, right_eye.Y));
	}
	
	return r;
}

std::vector<FaceFeaturesExtractor::FaceFeature> FaceFeaturesExtractor::Classify(plcl::BitmapInfo *data, std::vector<FaceFeaturesExtractor::AttributeFilter*> classifiers) {
	unsigned int const page_width(data->Width()), page_height(data->Height());
	std::vector<FaceFeature> r;
	
	CFLibraryAPI library_api = locate_face->library_api;
	std::auto_ptr<CFImage> image(CFImage::Copy(library_api, data->Info(), data->Scanline(0)));
	
	THROW_CF_ERROR(library_api.CFFindFaces(*locate_face, *image));
	
	int n(0);
	THROW_CF_ERROR(library_api.CFGetFacesCount(*locate_face, &n));
	for (int i = 0; i < n; ++i) {
		int x, y, radius;
		THROW_CF_ERROR(library_api.CFGetFacePos(*locate_face, i, &x, &y, &radius));
		CF_POINT left_eye, right_eye;
		THROW_CF_ERROR(library_api.CFGetEyePos(*locate_face, i, &left_eye, &right_eye));
		
		r.push_back(FaceFeaturesExtractor::FaceFeature(page_width, page_height, x, y, radius,
			left_eye.X, left_eye.Y, right_eye.X, right_eye.Y));

		for (size_t j = 0; j < classifiers.size(); ++j)
			classifiers[j]->Classify(image.get(), r.back());
	}
	
	return r;
}

extern void *HANDLE_DLL_MODULE;
FaceFeaturesExtractor* FaceFeaturesExtractor::Create() {
	std::auto_ptr<FaceFeaturesExtractor> r;

	std::wstring dll_path;
	if (!cpcl::GetModuleFilePath(HANDLE_DLL_MODULE, &dll_path))
		dll_path = L".\\";
	else
		dll_path += L'\\';

	std::list<DetectorInfo> detectors = ReadClassifierConfig(dll_path + L"cf_params\\cf_params.xml");
	DetectorInfo const *detector(0);
	for (std::list<DetectorInfo>::const_iterator i = detectors.begin(), tail = detectors.end(); i != tail; ++i) {
		if ((*i).id == IID_FaceFeaturesExtractor) {
			detector = &(*i);
			break;
		}
	}
	if (!detector) {
		cpcl::Warning(cpcl::StringPieceFromLiteral("FaceFeaturesExtractor::Create(): detector with id {A34CDBD5-0EF2-45fb-890A-CEA87E8F4459} not found"));
		return (FaceFeaturesExtractor*)0;
	}

	CFLibraryAPI library_api = CFLibraryAPI::Load(dll_path);
	if (!!library_api) {
		r.reset(new FaceFeaturesExtractor());
		r->detector = *detector;
		// r->locate_face.reset(CFLocateFace::Create(library_api, (cpcl::ConvertUTF16_CP1251(dll_path) + "cf_params\\engine1.xml")));
		r->locate_face.reset(CFLocateFace::Create(library_api, cpcl::ConvertUTF16_CP1251(r->detector.config)));
		r->locate_face->FastComputing(false);
	}

	return r.release();
}

FaceFeaturesExtractor::AttributeFilter* FaceFeaturesExtractor::CreateAttributeFilter(AttributeInfo const &attribute_info) {
	std::auto_ptr<AttributeFilter> r;
	CFLibraryAPI library_api = locate_face->library_api;
	if (!!library_api) {
		r.reset(new AttributeFilter());
		r->classifier_id = attribute_info.id;
		r->classifier_name = attribute_info.name;
		r->classes = attribute_info.classes;
		r->attr_filter.reset(CFAttrFilter::Create(library_api, cpcl::ConvertUTF16_CP1251(attribute_info.config)));
	}
	return r.release();
}

/*cf_params_path = $(ModuleDirName)/cf_params/;
cf_engine_path = cf_params_path + xml_doc.XPath(/config/engine)
CFLocateFace.Create(library_api, test.main.full_path_name(cf_engine_path)) // GetFullPathNameW
*/

//HRESULT IFaceFeaturesExtractorImpl::Create(IFaceFeaturesExtractor **r) {
//	try {
//	} catch (awp_exception) {
//		/*move awp_exception in cf_error_helper.hpp, CF_ERROR, CF_WARNING constants write as numeric, and add COMPILE_ASSERT(CF_WARNING = constants)*/
//		return E_NOTIMPL;
//	} CATCH_EXCEPTION
//};

FaceFeaturesExtractor* FaceFeaturesExtractor::Create(CFLibraryAPI *library_api, DetectorInfo const &detector) {
	if (detector.type != DetectorInfo::FACE_DETECTOR) {
		cpcl::Warning(cpcl::StringPieceFromLiteral("FaceFeaturesExtractor::Create(): invalid detector type"));
		return (FaceFeaturesExtractor*)0;
	}

	std::auto_ptr<FaceFeaturesExtractor> r(new FaceFeaturesExtractor());
	r->detector = detector;
	// r->locate_face.reset(CFLocateFace::Create(library_api, (cpcl::ConvertUTF16_CP1251(dll_path) + "cf_params\\engine1.xml")));
	r->locate_face.reset(CFLocateFace::Create(*library_api, cpcl::ConvertUTF16_CP1251(r->detector.config)));
	r->locate_face->FastComputing(false);
	return r.release();
}

struct CFObjectDetector {
	CFLibraryAPI library_api;
	CF_OBJECTDETECTOR cf_object_detector;

	~CFObjectDetector() {
		library_api.CFDestroyObjectDetector(&cf_object_detector);
	}
	operator CF_OBJECTDETECTOR() const {
		return cf_object_detector;
	}

	static CFObjectDetector* Create(CFLibraryAPI library_api, cpcl::StringPiece const &cf_config_path);
private:
	CFObjectDetector(CFLibraryAPI library_api_, CF_OBJECTDETECTOR cf_object_detector_) : library_api(library_api_), cf_object_detector(cf_object_detector_)
	{}
	DISALLOW_COPY_AND_ASSIGN(CFObjectDetector);
};

inline CFObjectDetector* CFObjectDetector::Create(CFLibraryAPI library_api, cpcl::StringPiece const &cf_config_path) {
	CF_OBJECTDETECTOR cf_object_detector = (CF_OBJECTDETECTOR)0;
	cpcl::ScopedBuf<char, 0x100> buf;
	THROW_CF_ERROR(library_api.CFCreateObjectDetector(&cf_object_detector, copy_str(buf, cf_config_path)));
	if (!cf_object_detector) {
		awp_exception::throw_formatted(awp_exception(CF_FAIL),
			"CFObjectDetector::Create('%s'): cf_object_detector is null",
			buf.Data());
	}

	return new CFObjectDetector(library_api, cf_object_detector);
}

ObjectDetector::ObjectDetector()
{}
ObjectDetector::~ObjectDetector()
{}

CFLibraryAPI* ObjectDetector::Load() {
	std::wstring dll_path;
	if (!cpcl::GetModuleFilePath(HANDLE_DLL_MODULE, &dll_path))
		dll_path = L".\\";
	else
		dll_path += L'\\';

	CFLibraryAPI library_api = CFLibraryAPI::Load(dll_path);
	if (!library_api)
		return (CFLibraryAPI*)0;

	return new CFLibraryAPI(library_api);
}

std::vector<DetectorInfo> ObjectDetector::ReadConfig() {
	std::wstring dll_path;
	if (!cpcl::GetModuleFilePath(HANDLE_DLL_MODULE, &dll_path))
		dll_path = L".\\";
	else
		dll_path += L'\\';

	std::list<DetectorInfo> detectors = ReadClassifierConfig(dll_path + L"cf_params\\cf_params.xml");
	return std::vector<DetectorInfo>(detectors.begin(), detectors.end());
}

ObjectDetector* ObjectDetector::Create(CFLibraryAPI *library_api, DetectorInfo const &detector) {
	if (detector.type != DetectorInfo::OBJECT_DETECTOR) {
		cpcl::Warning(cpcl::StringPieceFromLiteral("ObjectDetector::Create(): invalid detector type"));
		return (ObjectDetector*)0;
	}

	std::auto_ptr<ObjectDetector> r(new ObjectDetector());
	r->detector = detector;
	r->object_detector.reset(CFObjectDetector::Create(*library_api, cpcl::ConvertUTF16_CP1251(r->detector.config)));
	return r.release();
}

bool TestSaveBMP(plcl::RenderingData *data);
std::vector<FaceFeaturesExtractor::FaceFeature> ObjectDetector::Find(plcl::BitmapInfo *data) {
#ifdef _DEBUG
	TestSaveBMP(data);
#endif

	unsigned int const page_width(data->Width()), page_height(data->Height());
	std::vector<FaceFeaturesExtractor::FaceFeature> r;
	
	CFLibraryAPI library_api = object_detector->library_api;
	std::auto_ptr<CFImage> image(CFImage::Copy(library_api, data->Info(), data->Scanline(0)));
	
	THROW_CF_ERROR(library_api.CFDetectObject(*object_detector, *image));
	
	int n(0);
	THROW_CF_ERROR(library_api.CFGetObjectCount(*object_detector, &n));
	for (int i = 0; i < n; ++i) {
		int r_left, r_top, r_right, r_bottom;
		THROW_CF_ERROR(library_api.CFGetObjectRect(*object_detector, i, &r_top, &r_left, &r_right, &r_bottom));

		r.push_back(FaceFeaturesExtractor::FaceFeature(page_width, page_height, 0, 0, 0,
			r_left, r_top, r_right, r_bottom));
		r.back().y = 0; // ))
		r.back().is_object = true;
	}
	
	return r;
}

#if 0

struct CFLibAPI {
	CFCreateLocateFacePtr CFCreateLocateFace;
	CFDestroyLocateFacePtr CFDestroyLocateFace;
	CFFindFacesPtr CFFindFaces;
	CFGetFacesCountPtr CFGetFacesCount;
	CFGetFacePosPtr CFGetFacePos;
	CFGetLocateFaceParamsPtr CFGetLocateFaceParams;
	CFSetLocateFaceParamsPtr CFSetLocateFaceParams;
	CFDrawFacesPosPtr CFDrawFacesPos;
	
	CFCreateImagePtr CFCreateImage;
	CFDestroyImagePtr CFDestroyImage;
	CFLoadFromDIBPtr CFLoadFromDIB;
	CFLoadImagePtr CFLoadImage;
	CFSaveImagePtr CFSaveImage;
};
struct UnloadLibraryGuard {
	HMODULE hmodule;

	explicit UnloadLibraryGuard(HMODULE hmodule_) : hmodule(hmodule_)
	{}
	~UnloadLibraryGuard() {
		if (hmodule)
			::FreeLibrary(hmodule);
	}

	void Disarm() {
		hmodule = (HMODULE)0;
	}
	operator HMODULE() const {
		return hmodule;
	}
	bool operator!() const {
		return !hmodule;
	}
private:
	DISALLOW_COPY_AND_ASSIGN(UnloadLibraryGuard);
};
bool Load(CFLibAPI *lib_api) {
	std::wstring dll_path;
	if (!cpcl::GetModuleFilePath(0, &dll_path))
		dll_path = L".\\";
	else
		dll_path += L'\\';
	UnloadLibraryGuard cfimage_module(TryLoad(dll_path + L"cfimage.dll"));
	if (!cfimage_module)
		return false;
	{
		lib_api->CFCreateImage = (CFCreateImagePtr)::GetProcAddress(cfimage_module, "CFCreateImage");
		lib_api->CFDestroyImage = (CFDestroyImagePtr)::GetProcAddress(cfimage_module, "CFDestroyImage");
		lib_api->CFLoadFromDIB = (CFLoadFromDIBPtr)::GetProcAddress(cfimage_module, "CFLoadFromDIB");
		lib_api->CFLoadImage = (CFLoadImagePtr)::GetProcAddress(cfimage_module, "CFLoadImage");
		lib_api->CFSaveImage = (CFSaveImagePtr)::GetProcAddress(cfimage_module, "CFSaveImage");

		if (!lib_api->CFCreateImage || !lib_api->CFDestroyImage || !lib_api->CFLoadFromDIB || !lib_api->CFLoadImage || !lib_api->CFSaveImage) {
			cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
				": failed to load module '%s'",
				cpcl::ConvertUTF16_CP1251(dll_path + L"cfimage.dll").c_str());
			return false;
		}
	}

	UnloadLibraryGuard cffacedetect_module(TryLoad(dll_path + L"cffacedetect.dll"));
	if (!cffacedetect_module)
		return false;
	{
		lib_api->CFCreateLocateFace = (CFCreateLocateFacePtr)::GetProcAddress(cffacedetect_module, "CFCreateLocateFace");
		lib_api->CFDestroyLocateFace = (CFDestroyLocateFacePtr)::GetProcAddress(cffacedetect_module, "CFDestroyLocateFace");
		lib_api->CFFindFaces = (CFFindFacesPtr)::GetProcAddress(cffacedetect_module, "CFFindFaces");
		lib_api->CFGetFacesCount = (CFGetFacesCountPtr)::GetProcAddress(cffacedetect_module, "CFGetFacesCount");
		lib_api->CFGetFacePos = (CFGetFacePosPtr)::GetProcAddress(cffacedetect_module, "CFGetFacePos");
		lib_api->CFGetLocateFaceParams = (CFGetLocateFaceParamsPtr)::GetProcAddress(cffacedetect_module, "CFGetLocateFaceParams");
		lib_api->CFSetLocateFaceParams = (CFSetLocateFaceParamsPtr)::GetProcAddress(cffacedetect_module, "CFSetLocateFaceParams");
		lib_api->CFDrawFacesPos = (CFDrawFacesPosPtr)::GetProcAddress(cffacedetect_module, "CFDrawFacesPos");

		if (!lib_api->CFCreateLocateFace || !lib_api->CFDestroyLocateFace || !lib_api->CFFindFaces
			|| !lib_api->CFGetFacesCount || !lib_api->CFGetFacePos || !lib_api->CFGetLocateFaceParams || !lib_api->CFSetLocateFaceParams
			|| !lib_api->CFDrawFacesPos) {
			cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
				": failed to load module '%s'",
				cpcl::ConvertUTF16_CP1251(dll_path + L"cffacedetect.dll").c_str());
			return false;
		}
	}

	cfimage_module.Disarm();
	cffacedetect_module.Disarm();
	return true;
}

struct DeleteLocateFaceGuard {
	CF_LOCATE_FACE hLocateFace;
	CFLibAPI lib_api;
	
	explicit DeleteLocateFaceGuard(CFLibAPI lib_api_) : hLocateFace((CF_LOCATE_FACE)0), lib_api(lib_api_)
	{}
	~DeleteLocateFaceGuard() {
		if (hLocateFace)
			lib_api.CFDestroyLocateFace(&hLocateFace);
	}

	void Disarm() {
		hLocateFace = (CF_LOCATE_FACE)0;
	}
	operator CF_LOCATE_FACE() const {
		return hLocateFace;
	}
	bool operator!() const {
		return !hLocateFace;
	}
private:
	DISALLOW_COPY_AND_ASSIGN(DeleteLocateFaceGuard);
};

struct DeleteImageGuard {
	CF_IMAGE hImage;
	CFLibAPI lib_api;
	
	explicit DeleteImageGuard(CFLibAPI lib_api_) : hImage((CF_IMAGE)0), lib_api(lib_api_)
	{}
	~DeleteImageGuard() {
		if (hImage)
			lib_api.CFDestroyImage(&hImage);
	}

	void Disarm() {
		hImage = (CF_IMAGE)0;
	}
	operator CF_IMAGE() const {
		return hImage;
	}
	bool operator!() const {
		return !hImage;
	}
private:
	DISALLOW_COPY_AND_ASSIGN(DeleteImageGuard);
};

void TestLoad() {
	CFLibAPI lib_api;
	if (!Load(&lib_api))
		return;

	try {
		DeleteLocateFaceGuard hLocateFace(lib_api);
		char buf[0x100]; strncpy_s(buf, "C:\\Source\\c++\\draw\\bin\\Debug\\engine1.xml", _TRUNCATE);
		THROW_CF_ERROR(lib_api.CFCreateLocateFace(&hLocateFace.hLocateFace, buf));

		DeleteImageGuard hImage(lib_api);
		THROW_CF_ERROR(lib_api.CFCreateImage(&hImage.hImage));
		
		strncpy_s(buf, "C:\\Source\\c++\\draw\\test_exe\\input\\the_new_paper_new_face.jpg", _TRUNCATE);
		THROW_CF_ERROR(lib_api.CFLoadImage(hImage, buf));
		
		CF_LOCATE_FACE_PARAMS params;
		THROW_CF_ERROR(lib_api.CFGetLocateFaceParams(hLocateFace, &params));
		params.use_color = true;
		params.fast_computing = false;
		THROW_CF_ERROR(lib_api.CFSetLocateFaceParams(hLocateFace, &params));

		THROW_CF_ERROR(lib_api.CFFindFaces(hLocateFace, hImage));

		int count;
		THROW_CF_ERROR(lib_api.CFGetFacesCount(hLocateFace, &count));
		if (count > 0) {
			THROW_CF_ERROR(lib_api.CFDrawFacesPos(hLocateFace, hImage, CFRED, CFELLIPSE, 1.2f, true));

			strncpy_s(buf, "C:\\Source\\c++\\draw\\test_exe\\output\\the_new_paper_new_face.jpg", _TRUNCATE);
			THROW_CF_ERROR(lib_api.CFSaveImage(hImage, buf));
		}
	} catch (std::exception const &e) {
		printf("exception: %s\n", e.what());
	}
}

#endif
