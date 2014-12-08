// facefeaturesextractor.h
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
#pragma once

#ifndef __DLL_FACE_FEATURES_EXTRACTOR_H
#define __DLL_FACE_FEATURES_EXTRACTOR_H

#include <basic.h>

#include <memory>
#include <vector>

// #include <rendering_data.h>
#include "bitmap_info.h"

#include "classifier_config_reader.h"
#include "guid_util.hpp"

struct CFLibraryAPI;
struct CFImage;
struct CFLocateFace;
struct CFAttrFilter;
class FaceFeaturesExtractor {
	std::auto_ptr<CFLocateFace> locate_face;

	DISALLOW_IMPLICIT_CONSTRUCTORS(FaceFeaturesExtractor);
public:
	struct FaceFeature {
		static double const RADIUS_RATIO;
		static size_t const DATA_SIZE = sizeof(bool) + sizeof(unsigned int) * 2 + sizeof(int) * 3 + sizeof(int) * 4; // FACE_FEATURE_DATA_SIZE ?
		unsigned int page_width;
		unsigned int page_height;
		int x, y;
		int radius;

		int left_eye_x, left_eye_y, right_eye_x, right_eye_y;
		std::map<GUID, std::wstring> classes;

		bool is_object;

		FaceFeature(unsigned int page_width_, unsigned int page_height_, int x_, int y_, int radius_,
			int left_eye_x_, int left_eye_y_, int right_eye_x_, int right_eye_y_)
			: page_width(page_width_), page_height(page_height_), x(x_), y(y_), radius(radius_),
			left_eye_x(left_eye_x_), left_eye_y(left_eye_y_), right_eye_x(right_eye_x_), right_eye_y(right_eye_y_),
			is_object(false) {
			y = page_height - y;
			left_eye_y = page_height - left_eye_y; right_eye_y = page_height - right_eye_y; // agg draw at offset 0 - topmost scanline - top-bottom coords
		}
		FaceFeature() : page_width(0), page_height(0), x(0), y(0), radius(0), // used by IFaceFeatureImpl default ctor
			left_eye_x(0), left_eye_y(0), right_eye_x(0), right_eye_y(0), is_object(false)
		{}
		FaceFeature(unsigned char const *buf, size_t size) : page_width(0), page_height(0), x(0), y(0), radius(0), is_object(false) {
			SetData(buf, size);
		}

		void Draw(plcl::RenderingData *data, unsigned int r, unsigned int g, unsigned int b);
		bool GetData(unsigned char *buf, size_t size);
		bool SetData(unsigned char const *buf, size_t size); // used by IFaceFeatureImpl default ctor

		void GetROI(unsigned long *left, unsigned long *top, unsigned long *right, unsigned long *bottom);
	};
	/* nested, because can be used only with FaceFeature - x/y left_eye,right_eye */
	class AttributeFilter {
		std::auto_ptr<CFAttrFilter> attr_filter;

		friend FaceFeaturesExtractor;
		DISALLOW_IMPLICIT_CONSTRUCTORS(AttributeFilter);
	public:
		static int const MAGIC_NUMBER = 0xea9c2739;
		static size_t const DATA_SIZE = sizeof(int) + sizeof(AttributeFilter*); // FACE_FEATURE_DATA_SIZE ?
		GUID classifier_id;
		std::wstring classifier_name;
		std::map<int, std::wstring> classes;

		~AttributeFilter();

		bool Classify(CFImage *image, FaceFeaturesExtractor::FaceFeature &feature);
		bool Classify(plcl::BitmapInfo *data, FaceFeaturesExtractor::FaceFeature &feature);
		bool GetData(unsigned char *buf, size_t size);

		// static FaceFeaturesExtractor::AttributeFilter* Create(CFLibraryAPI library_api, AttributeInfo const &attribute_info);
		static bool Classify(std::vector<FaceFeaturesExtractor::AttributeFilter*> classifiers, plcl::BitmapInfo *data, FaceFeaturesExtractor::FaceFeature &feature);
		static FaceFeaturesExtractor::AttributeFilter* Marshal(unsigned char const *buf, size_t size);
	};
	// A34CDBD5-0EF2-45fb-890A-CEA87E8F4459
	static GUID const IID_FaceFeaturesExtractor;
	DetectorInfo detector;

	~FaceFeaturesExtractor();

	std::vector<FaceFeaturesExtractor::FaceFeature> Find(plcl::BitmapInfo *data);
	std::vector<FaceFeaturesExtractor::FaceFeature> Classify(plcl::BitmapInfo *data, std::vector<FaceFeaturesExtractor::AttributeFilter*> classifiers);

	static FaceFeaturesExtractor* Create();
	FaceFeaturesExtractor::AttributeFilter* CreateAttributeFilter(AttributeInfo const &attribute_info);
	static FaceFeaturesExtractor* Create(CFLibraryAPI *library_api, DetectorInfo const &detector);
};

struct CFObjectDetector;
class ObjectDetector {
	std::auto_ptr<CFObjectDetector> object_detector;

	DISALLOW_IMPLICIT_CONSTRUCTORS(ObjectDetector);
public:
	DetectorInfo detector;

	~ObjectDetector();

	std::vector<FaceFeaturesExtractor::FaceFeature> Find(plcl::BitmapInfo *data);

	static CFLibraryAPI* Load();
	static std::vector<DetectorInfo> ReadConfig();
	static ObjectDetector* Create(CFLibraryAPI *library_api, DetectorInfo const &detector);
};

#endif // __DLL_FACE_FEATURES_EXTRACTOR_H
