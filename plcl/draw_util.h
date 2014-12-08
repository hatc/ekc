// draw_util.h
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

#include "rendering_data.h"

namespace plcl {

void DrawPolygon(RenderingData *data, unsigned int width, unsigned int height, double *vertexes, unsigned int vertexes_n,
								 unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255);
void DrawSelectRect(RenderingData *data, unsigned int width, unsigned int height,
										double left, double top, double right, double bottom,
										unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255, double thickness = 4.0);
/*i.e. if resize to small image, scale rect thickness down, if resize to large image, keep rect thickness same*/
void DrawSelectRectDontUpScale(RenderingData *data, unsigned int width, unsigned int height,
										double left, double top, double right, double bottom,
										unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255, double thickness = 4.0);

// agg_util.h: 
// DrawPolygon(RenderingData, width, height, vertexes);
// DrawPolygon(RenderingDevice, rendering_width, rendering_height, width, height, vertexes);
// DrawSelectRect(RenderingData, width, height, rect);
// DrawSelectRect(RenderingDevice, rendering_width, rendering_height, width, height, rect);
// FillColor(RenderingData, color) { rendering_width = data->Width(); rendering_height = data->Height(); }
// FillColor(RenderingDevice, rendering_width, rendering_height, color);
// template<ScanlineSource> void FillColor(ScanlineReader(RenderingDevice)); { while reader.Next(&scanline) }

} // namespace plcl
