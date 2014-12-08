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

/*** fpieee_handler uses **/
#include <fpieee.h>
#include <excpt.h>
#include <float.h>
#include <stddef.h>
/*** fpieee_handler uses **/

#include <scoped_buf.hpp>
#include <trace.h>

#include <render.hpp>

#include "text_hack.h"

#include "pdf_rendering_data.h"
#include "pdf_page.h"

/*** fpieee_handler **/

//void dump_fpieee_record(_FPIEEE_RECORD *what, wchar_t const *then, int r) {
//  FILE *f = _wfopen(then, L"a");
//  if (!f)
//    f = _wfopen(then, L"w");
//  if (f) {
//    fprintf(f, "FP Exception caught\n");
//    if (what->Cause.InvalidOperation)
//      fprintf(f, "\tInvalidOperation\n");
//    if (what->Cause.ZeroDivide)
//      fprintf(f, "\tZeroDivide\n");
//    if (what->Cause.Inexact)
//      fprintf(f, "\tInexact\n");
//    if (what->Cause.Underflow)
//      fprintf(f, "\tUnderflow\n");
//    if (what->Cause.Overflow)
//      fprintf(f, "\tOverflow\n");
//
//    switch(what->Result.Format) {
//      case _FpFormatFp32 : fprintf(f, "\tResultFormat : _FpFormatFp32\n"); break;
//      case _FpFormatFp64 : fprintf(f, "\tResultFormat : _FpFormatFp64\n"); break;
//      case _FpFormatFp80 : fprintf(f, "\tResultFormat : _FpFormatFp80\n"); break;
//      case _FpFormatFp128 : fprintf(f, "\tResultFormat : _FpFormatFp128\n"); break;
//      case _FpFormatI16 : fprintf(f, "\tResultFormat : _FpFormatI16\n"); break;
//      case _FpFormatI32 : fprintf(f, "\tResultFormat : _FpFormatI32\n"); break;
//      case _FpFormatI64 : fprintf(f, "\tResultFormat : _FpFormatI64\n"); break;
//      case _FpFormatU16 : fprintf(f, "\tResultFormat : _FpFormatU16\n"); break;
//      case _FpFormatU32 : fprintf(f, "\tResultFormat : _FpFormatU32\n"); break;
//      case _FpFormatU64 : fprintf(f, "\tResultFormat : _FpFormatU64\n"); break;
//      case _FpFormatBcd80 : fprintf(f, "\tResultFormat : _FpFormatBcd80\n"); break;
//      case _FpFormatCompare : fprintf(f, "\tResultFormat : _FpFormatCompare\n"); break;
//      case _FpFormatString : fprintf(f, "\tResultFormat : _FpFormatString\n"); break;
//      default : fprintf(f, "\tResultFormat : undefined\n");
//    }
//    switch(r) {
//      case EXCEPTION_EXECUTE_HANDLER : fprintf(f, "\tEXCEPTION result : EXCEPTION_EXECUTE_HANDLER\n"); break;
//      case EXCEPTION_CONTINUE_SEARCH : fprintf(f, "\tEXCEPTION result : EXCEPTION_CONTINUE_SEARCH\n"); break;
//      case EXCEPTION_CONTINUE_EXECUTION : fprintf(f, "\tEXCEPTION result : EXCEPTION_CONTINUE_EXECUTION\n"); break;
//      default : fprintf(f, "\tEXCEPTION result : undefined\n");
//    }
//
//    fflush(f); fclose(f);
//  }
//}
int fpieee_handler( _FPIEEE_RECORD *pieee )
{
  // user-defined ieee trap handler routine:
  // there is one handler for all 
  // IEEE exceptions
  int r;

  // DebugBreak();

  // Assume the user wants all invalid 
  // operations to return 0.
  if (pieee->Cause.InvalidOperation) {
    r = EXCEPTION_CONTINUE_EXECUTION;

    switch(pieee->Result.Format) {
      case _FpFormatFp32: 
        pieee->Result.Value.Fp32Value = 0.0F; 
        break;
      case _FpFormatFp64: 
        pieee->Result.Value.Fp64Value = 0.0; 
        break;
      case _FpFormatFp80: 
        pieee->Result.Value.Fp80Value.W[0] = 0;
        pieee->Result.Value.Fp80Value.W[1] = 0;
        pieee->Result.Value.Fp80Value.W[2] = 0;
        pieee->Result.Value.Fp80Value.W[3] = 0;
        pieee->Result.Value.Fp80Value.W[4] = 0;
        break;
      case _FpFormatFp128: 
        pieee->Result.Value.Fp128Value.W[0] = 0;
        pieee->Result.Value.Fp128Value.W[1] = 0;
        pieee->Result.Value.Fp128Value.W[2] = 0;
        pieee->Result.Value.Fp128Value.W[3] = 0;
        break;
      case _FpFormatI16: 
        pieee->Result.Value.I16Value = 0;
        break;
      case _FpFormatI32: 
        pieee->Result.Value.I32Value = 0;
        break;
      case _FpFormatI64: 
        pieee->Result.Value.I64Value.W[0] = 0;
        pieee->Result.Value.I64Value.W[1] = 0;
        break;
      default:
        r = EXCEPTION_EXECUTE_HANDLER;
    }
  }
  else {
    r = EXCEPTION_EXECUTE_HANDLER;
  }

  // dump_fpieee_record(pieee, L"C:\\hlam\\excp.txt", r);
  return r;
}

#define _EXC_MASK    \
    _EM_UNDERFLOW  + \
    _EM_OVERFLOW   + \
    _EM_ZERODIVIDE + \
    _EM_INEXACT

/*** fpieee_handler **/

PdfPage::PdfPage(unsigned int width_dp_, unsigned int height_dp_, unsigned int dpi_, unsigned int page_num_)
	: plcl::TextPage(width_dp_, height_dp_, dpi_), page_num(page_num_) {
	// this->flip_y = false;
}
PdfPage::~PdfPage()
{}

/*** fpieee_handler **/
bool WrapRetreiveLayout(PDFDoc *pdfDoc, int pageNum, RectContainer *r_) {
	__try {
    // unmask invalid operation exception
    _controlfp_s(NULL, _EXC_MASK, _MCW_EM);
    
    return retreiveLayout(pdfDoc, pageNum, r_);
  } __except(_fpieee_flt(GetExceptionCode(),
    GetExceptionInformation(),
    fpieee_handler)) {
    // if fpieee_handler returns EXCEPTION_EXECUTE_HANDLER
    // code here gets control
		cpcl::Error(cpcl::StringPieceFromLiteral("SEH: WrapRetreiveLayout()"));
    return false;
  }
}

bool WrapDisplayPageSlice(PDFDoc *pdfDoc, SplashOutputDev *output_dev, int page_num,
	double hDPI, double vDPI, int rotate, GBool useMediaBox, GBool crop, GBool printing,
	int sliceX, int sliceY, int sliceW, int sliceH) {
	__try {
    // unmask invalid operation exception
    _controlfp_s(NULL, _EXC_MASK, _MCW_EM);
		
		pdfDoc->displayPageSlice(output_dev, page_num,
			hDPI, vDPI, rotate, useMediaBox, crop, printing,
			sliceX, sliceY, sliceW, sliceH);
		return true;
  } __except(_fpieee_flt(GetExceptionCode(),
    GetExceptionInformation(),
    fpieee_handler)) {
    // if fpieee_handler returns EXCEPTION_EXECUTE_HANDLER
    // code here gets control
		cpcl::Error(cpcl::StringPieceFromLiteral("SEH: WrapDisplayPageSlice()"));
    return false;
  }
}
/*** fpieee_handler **/

void PdfPage::Render(plcl::RenderingDevice *output) {
	SplashColorMode color_mode(splashModeRGB8); // change to BGR8 ???
	SplashColor color; color[0] = color[1] = color[2] = 0xff;
	std::auto_ptr<SplashOutputDev> output_dev(new SplashOutputDev(color_mode, 1, gFalse, color));
	output_dev->startDoc(doc->getXRef());

	/*doc->displayPageSlice(output_dev.get(), page_num,
		((double(width) / double(width_dp)) * (double(dpi) / device_dpi)),
		((double(height) / double(height_dp)) * (double(dpi) / device_dpi)),
		0, gFalse, gTrue, gFalse, 0, 0, width, height);*/
	if (!(WrapDisplayPageSlice(doc.get(), output_dev.get(), (int)page_num,
		double(dpi) * double(width) / double(width_dp), // #$!@~$!!!!!!!!
		double(dpi) * double(height) / double(height_dp), // #%^!#@```$!!!!!!!!!!!!!!!!!!
		0, gFalse, gTrue, gFalse, 0, 0, width, height)))
		throw plcl_exception("PdfPage::Render(): WrapDisplayPageSlice() fails");

	/* reverse y1, y2 - print not all but only selection in txt */

	std::auto_ptr<PdfRenderingData> rendering_data(new PdfRenderingData(PLCL_PIXEL_FORMAT_RGB_24));
	rendering_data->output_dev = output_dev;
	rendering_data->InvertBitmap(color, false); // rendering_data->InvertBitmap(color, rendering_data->Pixfmt() == PLCL_PIXEL_FORMAT_BGR_24);

	if ((rendering_data->Width() < 2) || (rendering_data->Height() < 2)) {
		cpcl::StringPiece e = cpcl::StringPieceFromLiteral("PdfPage::RenderData(): pdfDoc->displayPageSlice fails");
		cpcl::Error(e);
		throw plcl_exception(e.data());
	}
	/*if ((rendering_data->Width() != width) || (rendering_data->Height() != height)) {
		cpcl::Trace(CPCL_TRACE_LEVEL_WARNING, "PdfPage::RenderData(): rendering_data->Size(width = %u, height = %u) != (width = %u, height = %u)",
			rendering_data->Width(), rendering_data->Height(), width, height);
		rendering_data->Resize(width, height);
	}*/

	cpcl::ScopedBuf<unsigned char, 3 * 1024> resample_buf;
	cpcl::ScopedBuf<unsigned char, 3 * 1024> conv_buf;
	plcl::Render(plcl::RenderingDataReader(rendering_data.get()), rendering_data->Width(), rendering_data->Height(), rendering_data->Pixfmt(),
		output, width, height, true,
		resample_buf, conv_buf);
}

bool PdfPage::Text(wchar_t const **v, unsigned int *size) {
	if (!GetSelectionInfo())
		return false;
	if (v)
		*v = selection_info.text.c_str();
	if (size)
		*size = selection_info.text.size();
	return true;
}

bool PdfPage::GetSelectionInfo() {
	if (!selection_info.text.empty())
		return true;

	if (!WrapRetreiveLayout(doc.get(), page_num, &selection_info))
		return false;
	if (!selection_info)
		cpcl::Debug(cpcl::StringPieceFromLiteral("PdfPage::GetSelectionInfo(): text layout can be empty"));
	else
		selection_info.Done();

	return !selection_info.text.empty();
}
