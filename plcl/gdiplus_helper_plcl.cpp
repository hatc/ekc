// Copyright (C) 2012 Yuri Agafonov
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

#include <boost/thread/mutex.hpp>

#include <rendering_data.h>

#include <memory> // std::tr1::shared_ptr

#include <windows.h>

#include <gdiplus.h>

#include <plcl_exception.hpp>
//#include <istream_wrapper.h>
#include <istream_impl.h>

#pragma comment(lib, "gdiplus.lib")

struct {
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartupOutput gdiplusStartupOutput;
	ULONG_PTR gdiplusBGThreadToken;
} GdiplusToken;

static unsigned GdiplusRefs = 0;
static boost::mutex GdiplusMutex;
bool GdiplusAcquire() {
	boost::lock_guard<boost::mutex> lock(GdiplusMutex);

	if (0 == GdiplusRefs) {
		using Gdiplus::GdiplusStartup;
		using Gdiplus::Ok;

		GdiplusToken.gdiplusStartupInput.SuppressBackgroundThread = TRUE;
		if (GdiplusStartup(&GdiplusToken.gdiplusToken, &GdiplusToken.gdiplusStartupInput, &GdiplusToken.gdiplusStartupOutput) == Ok) {
			if (GdiplusToken.gdiplusStartupOutput.NotificationHook(&GdiplusToken.gdiplusBGThreadToken) == Ok)
				GdiplusRefs = 1;
		}
	} else
		++GdiplusRefs;

	return (GdiplusRefs > 0);
}
void GdiplusRelease() {
	boost::lock_guard<boost::mutex> lock(GdiplusMutex);
	
	if (GdiplusRefs > 0) {
		if (0 == --GdiplusRefs) {
			using Gdiplus::GdiplusShutdown;

			GdiplusToken.gdiplusStartupOutput.NotificationUnhook(GdiplusToken.gdiplusBGThreadToken);
			GdiplusShutdown(GdiplusToken.gdiplusToken);
		}
	}
}

inline unsigned int PLCLPixelFormat(Gdiplus::PixelFormat v) {
	switch (v) {
		case PixelFormat8bppIndexed :
		case PixelFormat16bppGrayScale :
		case PixelFormat24bppRGB : return PLCL_PIXEL_FORMAT_BGR_24;
		case PixelFormat32bppARGB : return PLCL_PIXEL_FORMAT_BGRA_32;
	}
	return PLCL_PIXEL_FORMAT_INVALID;
}
inline int GdiplusPixelFormat(unsigned int v) {
	if (!v)
		return PixelFormatUndefined;
	else if (v < PLCL_PIXEL_FORMAT_RGBA_32)
		return PixelFormat24bppRGB;
	else
		return PixelFormat32bppARGB;
}

class GdiplusBitmap : public plcl::RenderingData {
	Gdiplus::Bitmap *bitmap;
	Gdiplus::BitmapData *bitmapDataLocked;
	int stride;

	GdiplusBitmap(const GdiplusBitmap&);
  void operator=(const GdiplusBitmap&);

	bool LockPixels(int pxfmt, int *stride, unsigned char** r) {
		return LockPixels(0, 0, Width(), Height(), pxfmt, stride, r);
	}
	bool LockPixels(int left, int top, unsigned width, unsigned height, int pxfmt, int *stride, unsigned char** r);
	bool UnlockPixels();
public:
	virtual unsigned int Width() const;
	virtual unsigned int Height() const;
	virtual int Stride() const;
	virtual void Pixfmt(unsigned int v);

	GdiplusBitmap(Gdiplus::Bitmap *bitmap_) : RenderingData(PLCLPixelFormat(bitmap_->GetPixelFormat())), bitmap(bitmap_), bitmapDataLocked(NULL), stride(0) {}
	virtual ~GdiplusBitmap();

	virtual unsigned char* Scanline(unsigned int y);
	bool GetPixelsCopy(int pxfmt, int *stride, unsigned char** r) {
		return GetPixelsCopy(0, 0, Width(), Height(), pxfmt, stride, r);
	}
	bool GetPixelsCopy(int left, int top, unsigned width, unsigned height, int pxfmt, int *stride, unsigned char** r);
};

GdiplusBitmap::~GdiplusBitmap() {
	UnlockPixels();
	delete bitmap;
	GdiplusRelease();
}

unsigned int GdiplusBitmap::Width() const {
	return bitmap->GetWidth();
}
unsigned int GdiplusBitmap::Height() const {
	return bitmap->GetHeight();
}
int GdiplusBitmap::Stride() const {
	if (!stride) {
		using namespace Gdiplus;

		BitmapData bitmapData;
		Rect rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
		if (bitmap->LockBits(&rect, ImageLockModeRead, GdiplusPixelFormat(RenderingData::Pixfmt()), &bitmapData) == Ok) {
			int stride_ = abs(bitmapData.Stride);
			bitmap->UnlockBits(&bitmapData);
			return stride_;
		}
	}
	return stride;
}
void GdiplusBitmap::Pixfmt(unsigned int v) {
	if (v != RenderingData::Pixfmt())
		throw plcl_exception("GdiplusBitmap::Pixfmt(): not implemented");
}

void add_scanline_requests(unsigned int y);
unsigned char* GdiplusBitmap::Scanline(unsigned int y) {
	add_scanline_requests(y);

	if (!bitmapDataLocked) {
		if (!LockPixels(GdiplusPixelFormat(RenderingData::Pixfmt()), NULL, NULL))
			plcl_exception::throw_formatted(plcl_exception(), "GdiplusBitmap::Scanline(%u): can't LockPixels", y);
			//return NULL;
	}
	
	stride = abs(bitmapDataLocked->Stride);
	if (y >= bitmapDataLocked->Height)
		plcl_exception::throw_formatted(plcl_exception(), "GdiplusBitmap::Scanline(%u): out of range", y);
	unsigned char *scan0 = (unsigned char*)bitmapDataLocked->Scan0;
	if (bitmapDataLocked->Stride > 0)
		y = bitmapDataLocked->Height - y - 1;
	return scan0 + bitmapDataLocked->Stride * y;
}

bool GdiplusBitmap::LockPixels(int left, int top, unsigned width, unsigned height, int pxfmt, int *stride, unsigned char** r) {
	using namespace Gdiplus;

	if ((0 == width) || (0 == height))
		return false;

	UnlockPixels();
	bitmapDataLocked = new BitmapData;
	Rect rect(left, top, width, height);

	if (bitmap->LockBits(&rect, ImageLockModeRead, (PixelFormat)pxfmt, bitmapDataLocked) == Gdiplus::Ok) {
		if (stride)
			*stride = bitmapDataLocked->Stride;
		if (r)
			*r = (unsigned char*)bitmapDataLocked->Scan0;
	} else {
		delete bitmapDataLocked;
		bitmapDataLocked = NULL;
	}

	return (bitmapDataLocked != NULL);
}
bool GdiplusBitmap::UnlockPixels() {
	bool r(false);
	if (bitmapDataLocked) {
		r = (bitmap->UnlockBits(bitmapDataLocked) == Gdiplus::Ok);

		delete bitmapDataLocked;
		bitmapDataLocked = NULL;
	}
	return r;
}

//PixelFormat24bppRGB  == 0x00021808 === PIXEL_FORMAT_TYPE_BGR_24
//PixelFormat32bppARGB == 0x0026200a === PIXEL_FORMAT_TYPE_BGRA_32

//class BitmapData : public RenderingData {
//	int stride, pixels_stride;
//	unsigned char *pixels;
//	std::tr1::shared_ptr<GdiplusBitmap> bmp; // auto_ptr<Gdiplus::Bitmap> bitmap;
//public:
//	DIBData(GdiplusBitmap *bmp_) : RenderingData(PLCLPixelFormat(bmp_->Pxfmt())), bmp(bmp_), stride(0), pixels(NULL)
//	{}
//	virtual ~DIBData()
//	{}
//
//	virtual unsigned int Width() const {
//		return bmp->Width();
//	}
//	virtual unsigned int Height() const {
//		return bmp->Height();
//	}
//	virtual int Stride() const {
//		if (!stride)
//			stride = bmp->Stride();
//		if (!stride)
//			throw plcl_exception("BitmapData::Stride(): fail to lock bits");
//
//		return stride;
//	}
//	virtual void Pixfmt(unsigned int v) {
//		if (v != RenderingData::Pixfmt())
//			throw plcl_exception("BitmapData::Pixfmt(): not implemented");
//	}
//
//	virtual unsigned char* Scanline(unsigned int y) {
//		if (!pixels) {
//			if (!bmp->LockPixels(0, 0, bmp->Width(), bmp->Height(), GdiplusPixelFormat(pixel_format), &pixels_stride, &pixels))
//				throw plcl_exception("BitmapData::Scanline(): fail to lock pixels");
//		}
//		return pixels + pixels_stride * y;
//	}
//};
//
//bool LoadBitmap(wchar_t const *path, RenderingData **r) {
//	/*if (GdiplusAcquire()) {
//		using Gdiplus::Bitmap;
//		using Gdiplus::Ok;
//
//		Bitmap *bitmap = new Bitmap(stream, 0);
//		if (bitmap->GetLastStatus() == Ok)
//			bitmap_data.reset(new BitmapData(bitmap));
//		else
//			delete bitmap;
//	}
//	if (!!bitmap_data.get())
//	 if (r)
//	  *r = bitmap_data.release();
//	 return true;
//	return false;*/
//	GdiplusBitmap *bmp = GdiplusBitmap::open(path);
//	if (bmp) {
//		std::tr1::shared_ptr<GdiplusBitmap> bmp_guard(bmp);
//
//		if (bmp->GetPixelsCopy(pxfmt, stride, r)) {
//			if (width)
//				*width = bmp->Width();
//			if (height)
//				*height = bmp->Height();
//			return true;
//		}
//	}
//	return false;
//}

plcl::RenderingData* GdiplusLoad(wchar_t const *path) {
	GdiplusBitmap *r(NULL);
	if (GdiplusAcquire()) {
		using Gdiplus::Bitmap;
		using Gdiplus::Ok;

#ifdef TEST_TEMPORARY_STREAM
		ScopedComPtr<IStream> stream;
		if (files::fileStreamRead(path, &stream) == S_OK) {
		Bitmap *bitmap = new Bitmap(stream, 0);
#else
		Bitmap *bitmap = new Bitmap(path, 0);
#endif
		if (bitmap->GetLastStatus() == Ok)
			r = new GdiplusBitmap(bitmap);
		else
			delete bitmap;
#ifdef TEST_TEMPORARY_STREAM
		}
#endif
	}
	return r;
}

plcl::RenderingData* GdiplusLoad(cpcl::IOStream *stream) {
	GdiplusBitmap *r(NULL);
	if (GdiplusAcquire()) {
		using Gdiplus::Bitmap;
		using Gdiplus::Ok;

		cpcl::IStreamImpl stream_(stream); stream_.AddRef();
		Bitmap *bitmap = new Bitmap(&stream_, 0);
		if (bitmap->GetLastStatus() == Ok)
			r = new GdiplusBitmap(bitmap);
		else
			delete bitmap;
	}
	return r;
}
