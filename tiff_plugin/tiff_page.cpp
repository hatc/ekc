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
#include <basic.h>

#include "tiff_image.h"
#include "tiff_page.h"

#include <vector>

#include <scoped_buf.hpp>
#include <plcl_exception.hpp>

#include <render.hpp>

inline bool libtiff_check_header(cpcl::IOStream *input) {
//#define TIFF_BIGENDIAN       0x4D4D
//#define TIFF_LITTLEENDIAN    0x4949
//#define TIFF_VERSION_CLASSIC 42 // == 0x2A
//#define TIFF_VERSION_BIG     43 // == 0x2B
	unsigned char tiff_version_classic_be[] = { 0x4D, 0x4D, 0x00, 0x2A };
	unsigned char tiff_version_classic_le[] = { 0x49, 0x49, 0x2A, 0x00 };
	unsigned char tiff_version_big_be[] = { 0x4D, 0x4D, 0x00, 0x2B };
	unsigned char tiff_version_big_le[] = { 0x49, 0x49, 0x2B, 0x00 };
	unsigned char *tiff_versions[] = {
		tiff_version_classic_be,
		tiff_version_classic_le,
		tiff_version_big_be,
		tiff_version_big_le
	};
	unsigned char tiff_version[4];

	unsigned long readed = input->Read(tiff_version, sizeof(tiff_version));
	input->Seek(0, SEEK_SET, NULL);

	if (readed != sizeof(tiff_version))
		return false;
	for (size_t i = 0; i < arraysize(tiff_versions); ++i) {
		if (memcmp(tiff_version, tiff_versions[i], sizeof(tiff_version)) == 0)
			return true;
	}
	return false;
}

struct HorizontalFlipper {
	enum { MAX_BYTES_PER_PIXEL = 4 };
	unsigned int const width, half_width, bytes_per_pixel;

	HorizontalFlipper(TiffImage const &img) : width(img.width), half_width(img.width / 2),
		bytes_per_pixel(plcl::RenderingData::BitsPerPixel(img.pixfmt) / 8)
	{
		// DUMBASS_CHECK(bytes_per_pixel <= MAX_BYTES_PER_PIXEL);
	}

	void Flip(unsigned char *scanline) {
		unsigned char buf[MAX_BYTES_PER_PIXEL];
		for (unsigned int i = 0; i < half_width; ++i) {
			ptrdiff_t left(i * bytes_per_pixel), right((width - i - 1) * bytes_per_pixel);
			// memcpy release asm variant use registers for copy values <= 4 bytes
			memcpy(buf, scanline + left, bytes_per_pixel);
			memcpy(scanline + left, scanline + right, bytes_per_pixel);
			memcpy(scanline + right, buf, bytes_per_pixel);
		}
	}
};

struct TiffStripReader {
	unsigned char *strip;
	uint32 strip_height, vertical_subsampling;
	size_t strip_size, row_size, color_channels;
	TiffImage *img;
	HorizontalFlipper flipper;
	unsigned int y, rows_to_read;
	
	void Read(unsigned char *scanline, unsigned int *y_) {
		if (y == (unsigned int)-1)
			y = 0;
		else if (y >= img->height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"TiffStripReader::Read(): current row(%u) >= img->height(%u)",
				y, img->height);
		}

		FetchRow();
		size_t strip_offset = (y % strip_height) * row_size;

		if (vertical_subsampling) {
			//strip_offset -= (y % vertical_subsampling) * row_size;
			img->vertical_subsample = (y % vertical_subsampling);
			strip_offset -= img->vertical_subsample * row_size;
		}

		if (img->planar_separate)
			ReadSeparate(scanline, strip_offset);
		else
			img->libtiff_conv(img, scanline, img->width, strip + strip_offset);

		if (img->flip & TiffImage::FLIP_HORIZONTALLY)
			flipper.Flip(scanline);

		if (y_)
			*y_ = y;
		++y;
	}

	void Skip(unsigned int lines) {
		if (y == (unsigned int)-1)
			y = 0;
		if (y + lines >= img->height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"TiffStripReader::Skip(): row(%u) >= img->height(%u)",
				y + lines, img->height);
		}

		y += lines;
		if (rows_to_read < lines)
			rows_to_read = 0;
		else
			rows_to_read -= lines;
	}
	
	TiffStripReader(TiffImage *img_) : y((unsigned int)-1), img(img_), flipper(*img_),
		strip(0), strip_height(0), strip_size(0), row_size(0), color_channels(0), rows_to_read(0),
		vertical_subsampling(0)
	{}

	size_t StripSize() const {
		size_t r = strip_size;
		if (img->planar_separate) {
			r *= color_channels;
			if (img->alpha != TiffImage::ALPHA_UNSPECIFIED)
				r += strip_size;
		}
		return r;
	}
	static TiffStripReader CreateStripReader(TiffImage *img) {
		TiffStripReader r(img);
		TIFF *tif = img->tif;

		TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &r.strip_height);
		if (r.strip_height < 1) {
			tiff_exception::throw_formatted(tiff_exception(),
				"TiffStripReader(): invalid or corrupted tiff header: rowsperstrip = %u",
				r.strip_height);
		}

		switch (img->photometric) {
			case PHOTOMETRIC_MINISWHITE:
			case PHOTOMETRIC_MINISBLACK:
			case PHOTOMETRIC_PALETTE:
				r.color_channels = 1;
				break;
			default:
				r.color_channels = 3;
		}

		/*for 22 subsampling 6 bytes per block, each block Cb, Cr, 4 * Y, i.e. 4 pixel
		for 11 subsampling 3 bytes per block, block Cb, Cr, Y, i.e. 1 pixel
		width 512, 11 subsampling 1536 bytes ScanlineSize, 22 subsampling 768 bytes ScanlineSize
		768 bytes == 6 * 128 == 4 * 128 == 512 pixels

		but data packed interleaved - 2 pixels from y line && 2 pixels from y + 1 line,
		i.e. Cb = pp[4], Cr = pp[5]; out_rgb[0] = YCbCrtoRGB(Cb, Cr, in[0]); out_rgb[1] = YCbCrtoRGB(Cb, Cr, in[1]);
		(out_rgb + 1)[0] = YCbCrtoRGB(Cb, Cr, in[2]); (out_rgb + 1)[1] = YCbCrtoRGB(Cb, Cr, in[3]);*/
		if (PHOTOMETRIC_YCBCR == img->photometric) {
			uint16 horizontal_subsampling, vertical_subsampling;
			TIFFGetFieldDefaulted(tif, TIFFTAG_YCBCRSUBSAMPLING, &horizontal_subsampling, &vertical_subsampling);
			r.vertical_subsampling = static_cast<uint32>(vertical_subsampling);
			/* if (subsampling != 1) в одном блоке данных упакованно несколько фрагментов scanline с разными y:
			 h_subsampling/v_subsampling == кол-во образцов по гор./верт. на каждый блок
			 например 1/1 - каждый блок содержит ровно 1 образец - пиксель, можно читать как обычный scanline
			 если 2/1 - каждый блок содержит два пикселя по горизонтали - можно читать как обычный scanline, если правильно выбран strip_offset

			 TIFFScanlineSize - returns size of packed YCbCr lines / v_subsampling,
			 т.е. TIFFScanlineSize это размер в байтах линии которая содержит h_subsampling * width пикселей
			 значит strip_offset, так же как и rows_to_read, надо скорректировать - 
			 strip_offset = TIFFScanlineSize / h_subsampling ???
			 rows_to_read *= h_subsampling * v_subsampling;

			 или if (PHOTOMETRIC_YCBCR == img->photometric) {
			  img horizontal_sample vertical_sample
				if (???)
				 strip_offset
				if (???)
				 --rows_to_read; 

				 т.е. использовать доп. поля img а параметры Reader оставить как при чтении остальных типов данных
			 }

			 img->vertical_subsampling можно использовать как индекс:
			 ycbcr_conv(img, width, in) {
			  for (x < width)
				 in[img->vertical_sample]
				 in[img->vertical_sample + 1]
				 in[img->vertical_sample + 2]
				 in[img->vertical_sample + 3]
			 }
			 как правильно рассчитать смещение и rows_to_read ??

			 img {
			 uint16 horizontal_subsampling, vertical_subsampling;
			 uint16 horizontal_sample : [0, horizontal_subsampling), vertical_sample : [0, vertical_subsampling);
			 }

			 каждый блок данных YCbCr содержит как минимум два байта - Cb, Cr
			 кол-во байтов Y равно (h_subsampling * v_subsampling)
			 т.е. для 2/1 и 1/2 кол-во байт Y == 2
			 а для 4/1 и 2/2 кол-во байт Y == 4
			 общая длина блока в байтах == (h_subsampling * v_subsampling) + 2  (2 bytes for Cb, Cr)

			 при обработке YCbCr нужно проходить по одному и тому же row, т.е. FetchRow на самом деле не должен fetch, если v_subsampling > 1

			 row_size && width ??? 4/1 - each row contain width / 4 bytes ???
			 можно считать что TileSize && TIFFScanlineSize
			 */
		}

		tmsize_t strip_size = TIFFStripSize(tif);
		tmsize_t row_size = TIFFScanlineSize(tif);
		if ((strip_size < 1) || (row_size < 1)) {
			tiff_exception::throw_formatted(tiff_exception(),
				"TiffStripReader(): invalid or corrupted tiff header: strip_size = %d, scanline_size = %d",
				(int)strip_size, (int)row_size);
		}
		r.strip_size = static_cast<size_t>(strip_size);
		r.row_size = static_cast<size_t>(row_size);

		return r;
	}
private:
	void ReadStripChecked(unsigned char *buf, uint32 read_encoded_rows, uint16 color_channel) {
		unsigned char const v = *buf;
		if ((TIFFReadEncodedStrip(img->tif, TIFFComputeStrip(img->tif, y, color_channel),
			buf, (y % strip_height + read_encoded_rows) * row_size) == (tmsize_t)(-1))
			&& (v == *buf)) { // i.e. if fail && no new data fetched
			tiff_exception::throw_formatted(tiff_exception(),
				"TiffStripReader::Read(): TIFFReadEncodedStrip fails, read_encoded_rows %u, current row %u",
				read_encoded_rows, y);
		}
	}
	/*void FetchRow() {
		if (rows_to_read < 1) {
			rows_to_read = strip_height - y % strip_height;
			uint32 read_encoded_rows = (y + rows_to_read > img->height ? img->height - y : rows_to_read);
			if ((!img->planar_separate) && (vertical_subsampling != 0)) {
				if ((read_encoded_rows % vertical_subsampling) != 0)
					read_encoded_rows += vertical_subsampling - read_encoded_rows % vertical_subsampling;
			}
			uint16 color_channel(0);
			TIFFReadEncodedStrip(img->tif, TIFFComputeStrip(img->tif, y, color_channel++),
				strip, (y % strip_height + read_encoded_rows) * row_size);
			if (img->planar_separate) {
				size_t strip_buf_offset = strip_size;
				while (color_channel < color_channels) {
					TIFFReadEncodedStrip(img->tif, TIFFComputeStrip(img->tif, y, color_channel++),
						strip + strip_buf_offset, (y % strip_height + read_encoded_rows) * row_size);
					strip_buf_offset += strip_size;
				}
				
				if (img->alpha != TiffImage::ALPHA_UNSPECIFIED) {
					TIFFReadEncodedStrip(img->tif, TIFFComputeStrip(img->tif, y, color_channel),
						strip + strip_buf_offset, (y % strip_height + read_encoded_rows) * row_size);
				}
			}
		}
		--rows_to_read;
	}*/
	void FetchRow() {
		if (rows_to_read < 1) {
			rows_to_read = strip_height - y % strip_height;
			if (y + rows_to_read > img->height)
				rows_to_read = img->height - y;
			unsigned int read_with_subsampling = rows_to_read;
			if ((!img->planar_separate) && (vertical_subsampling != 0)) {
				if ((read_with_subsampling % vertical_subsampling) != 0)
					read_with_subsampling += vertical_subsampling - read_with_subsampling % vertical_subsampling;
			}
			uint16 color_channel(0);
			TIFFReadEncodedStrip(img->tif, TIFFComputeStrip(img->tif, y, color_channel++),
				strip, (y % strip_height + read_with_subsampling) * row_size);
			if (img->planar_separate) {
				size_t strip_buf_offset = strip_size;
				while (color_channel < color_channels) {
					TIFFReadEncodedStrip(img->tif, TIFFComputeStrip(img->tif, y, color_channel++),
						strip + strip_buf_offset, (y % strip_height + read_with_subsampling) * row_size);
					strip_buf_offset += strip_size;
				}
				
				if (img->alpha != TiffImage::ALPHA_UNSPECIFIED) {
					TIFFReadEncodedStrip(img->tif, TIFFComputeStrip(img->tif, y, color_channel),
						strip + strip_buf_offset, (y % strip_height + read_with_subsampling) * row_size);
				}
			}
		}
		--rows_to_read;
	}
	void ReadSeparate(unsigned char *scanline, size_t strip_offset) {
		//DUMBASS_CHECK(color_channels < 4);
		unsigned char *planes[4] = { 0 };

		size_t strip_buf_offset(0);
		for (size_t color_channel = 0; color_channel < color_channels; ++color_channel, strip_buf_offset += strip_size) {
			planes[color_channel] = strip + strip_offset + strip_buf_offset;
		}
		if (img->alpha != TiffImage::ALPHA_UNSPECIFIED)
			planes[3] = strip + strip_offset + strip_buf_offset;

		img->libtiff_conv_separate(img, scanline, img->width, planes[0], planes[1], planes[2], planes[3]);
	}
};

struct TiffTileReader {
	unsigned char **tiles;
	size_t tiles_count;
	uint32 tile_width, tile_height, vertical_subsampling;
	size_t tile_size, row_size, color_channels;
	TiffImage *img;
	HorizontalFlipper flipper;
	unsigned int y, rows_to_read;

	void Read(unsigned char *scanline, unsigned int *y_) {
		if (y == (unsigned int)-1)
			y = 0;
		else if (y >= img->height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"TiffTileReader::Read(): current row(%u) >= img->height(%u)",
				y, img->height);
		}

		FetchRow();
		size_t tile_offset = (y % tile_height) * row_size;

		if (vertical_subsampling) {
			//tile_offset = (y % tile_height - img->vertical_subsample) * row_size;
			img->vertical_subsample = (y % vertical_subsampling);
			tile_offset -= img->vertical_subsample * row_size;
		}

		uint32 x(0); unsigned int scanline_offset(0);
		for (size_t i = 0; i < tiles_count; x += tile_width, ++i) {
			unsigned int width = ((x + tile_width) < img->width) ? tile_width : (img->width - x);

			if (img->planar_separate)
				ReadSeparate(scanline + scanline_offset, width, tiles[i], tile_offset);
			else
				img->libtiff_conv(img, scanline + scanline_offset, width, tiles[i] + tile_offset);

			scanline_offset += plcl::RenderingData::Stride(img->pixfmt, width);
		}

		if (img->flip & TiffImage::FLIP_HORIZONTALLY)
			flipper.Flip(scanline);

		if (y_)
			*y_ = y;
		++y;
	}

	void Skip(unsigned int lines) {
		if (y == (unsigned int)-1)
			y = 0;
		if (y + lines >= img->height) {
			plcl_exception::throw_formatted(plcl_exception(),
				"TiffTileReader::Skip(): row(%u) >= img->height(%u)",
				y + lines, img->height);
		}

		y += lines;
		if (rows_to_read < lines)
			rows_to_read = 0;
		else
			rows_to_read -= lines;
	}

	TiffTileReader(TiffImage *img_) : y((unsigned int)-1), img(img_), flipper(*img_),
		tiles(0), tiles_count(0), tile_width(0), tile_height(0), tile_size(0), row_size(0),
		color_channels(0), rows_to_read(0), vertical_subsampling(0)
	{}

	size_t TileSize() const {
		size_t r = tile_size;
		if (img->planar_separate) {
			r *= color_channels;
			if (img->alpha != TiffImage::ALPHA_UNSPECIFIED)
				r += tile_size;
		}
		return r;
	}
	static TiffTileReader CreateTileReader(TiffImage *img) {
		TiffTileReader r(img);
		TIFF *tif = img->tif;

		TIFFGetField(tif, TIFFTAG_TILEWIDTH, &r.tile_width);
		TIFFGetField(tif, TIFFTAG_TILELENGTH, &r.tile_height);
		if ((r.tile_width < 1) || (r.tile_height < 1)) {
			tiff_exception::throw_formatted(tiff_exception(),
				"TiffTileReader(): invalid or corrupted tiff header: tile_width = %u, tile_height = %u",
				r.tile_width, r.tile_height);
		}

		switch (img->photometric) {
			case PHOTOMETRIC_MINISWHITE:
			case PHOTOMETRIC_MINISBLACK:
			case PHOTOMETRIC_PALETTE:
				r.color_channels = 1;
				break;
			default:
				r.color_channels = 3;
		}

		tmsize_t tile_size = TIFFTileSize(tif);
		tmsize_t row_size = TIFFTileRowSize(tif);
		if ((tile_size < 1) || (row_size < 1)) {
			tiff_exception::throw_formatted(tiff_exception(),
				"TiffTileReader(): invalid or corrupted tiff header: tile_size = %d, tile_row_size = %d",
				(int)tile_size, (int)row_size);
		}
		r.tile_size = static_cast<size_t>(tile_size);
		r.row_size = static_cast<size_t>(row_size);

		/* TIFFTileRowSize не учитывает YCbCr subsampling
		 так что если (PHOTOMETRIC_YCBCR == img->photometric) && (h_subsampling > 1 || v_subsampling > 1)
		 row_size = tile_size / tile_height; */
		if (PHOTOMETRIC_YCBCR == img->photometric) {
			uint16 horizontal_subsampling, vertical_subsampling;
			TIFFGetFieldDefaulted(tif, TIFFTAG_YCBCRSUBSAMPLING, &horizontal_subsampling, &vertical_subsampling);
			r.vertical_subsampling = static_cast<uint32>(vertical_subsampling);
			if (horizontal_subsampling > 1 || vertical_subsampling > 1)
				r.row_size = r.tile_size / r.tile_height;
		}

		for (uint32 i = 0; i < img->width; i += r.tile_width)
			++r.tiles_count;

		return r;
	}
private:
	void ReadTileChecked(unsigned char *buf, uint32 x_, uint32 y_, uint16 s) {
		unsigned char const v = *buf;
		if ((TIFFReadTile(img->tif, buf, x_, y_, 0, s) == (tmsize_t)(-1))
			&& (v == *buf)) { // i.e. if fail && no new data fetched
			tiff_exception::throw_formatted(tiff_exception(),
				"TiffTileReader::Read(): TIFFReadTile fails, x %u, y %u, current row %u",
				x_, y_, y);
		}
	}
	void FetchRow() {
		if (rows_to_read < 1) {
			rows_to_read = tile_height - y % tile_height;
			// y + rows_to_read > img->height ? redundant check, height already validated in Read

			size_t i(0);
			for (uint32 x = 0; x < img->width; x += tile_width, ++i) {
				DUMBASS_CHECK(i < tiles_count);

				uint16 color_channel(0);
				TIFFReadTile(img->tif, tiles[i], x, y, 0, color_channel++);
				if (img->planar_separate) {
					size_t tile_buf_offset = tile_size;
					while (color_channel < color_channels) {
						TIFFReadTile(img->tif, tiles[i] + tile_buf_offset, x, y, 0, color_channel++);
						tile_buf_offset += tile_size;
					}

					if (img->alpha != TiffImage::ALPHA_UNSPECIFIED)
						TIFFReadTile(img->tif, tiles[i] + tile_buf_offset, x, y, 0, color_channel);
				}
			}
		}
		--rows_to_read;
	}
	void ReadSeparate(unsigned char *scanline, unsigned int width, unsigned char *tile, size_t tile_offset) {
		//DUMBASS_CHECK(color_channels < 4);
		unsigned char *planes[4] = { 0 };

		size_t tile_buf_offset(0);
		for (size_t color_channel = 0; color_channel < color_channels; ++color_channel, tile_buf_offset += tile_size) {
			planes[color_channel] = tile + tile_offset + tile_buf_offset;
		}
		if (img->alpha != TiffImage::ALPHA_UNSPECIFIED)
			planes[3] = tile + tile_offset + tile_buf_offset;

		img->libtiff_conv_separate(img, scanline, width, planes[0], planes[1], planes[2], planes[3]);
	}
};

TiffPage::TiffPage(unsigned int width_, unsigned int height_, std::auto_ptr<cpcl::IOStream> &input_, unsigned int page_num_)
	: Page(width_, height_, 96), input(input_), page_num(page_num_)
{}
TiffPage::~TiffPage()
{}

void TiffPage::Render(plcl::RenderingDevice *output) {
	input->Seek(0, SEEK_SET, NULL);
	if (!libtiff_check_header(input.get()))
		throw plcl_exception("TiffPage::Render(): tiff header invalid, probably the underlying stream is corrupted");

	TiffStuff tiff_stuff(input.get());
	if (!tiff_stuff)
		throw tiff_exception("TiffPage::Render(): unable to create and initialize libtiff handler");

	TIFF *tif = tiff_stuff.tif; // each TiffPage create own TIFF* and call TIFFSetDirectory(page), TiffDoc store only IOStream() and page_count

	uint16 page_num_ = (uint16)page_num;
	if (!TIFFSetDirectory(tif, page_num_)) {
		tiff_exception::throw_formatted(tiff_exception(),
			"TiffPage::Render(): unable to open page %u",
			(unsigned int)page_num_);
	}

	cpcl::ScopedBuf<unsigned char, 4 * 1024> buf;
	cpcl::ScopedBuf<unsigned char, 2 * 1024> render_buf_resample;
	cpcl::ScopedBuf<unsigned char, 2 * 1024> render_buf_conv;
	std::vector<std::vector<unsigned char> > tiles_buf;

	TiffImage img = TiffImage::CreateTiffImage(tiff_stuff);

	unsigned int output_width(this->width), output_height(this->height);

	bool mirror_x, mirror_y; unsigned int angle;
	plcl::Page::ExifOrientation(ExifOrientation(), &mirror_x, &mirror_y, &angle);
	if (img.tiled) {
		TiffTileReader reader = TiffTileReader::CreateTileReader(&img);
		reader.tiles = (unsigned char**)buf.Alloc(reader.tiles_count * sizeof(unsigned char*));

		size_t const tile_buf_size = reader.TileSize();
		tiles_buf.resize(reader.tiles_count);
		std::vector<std::vector<unsigned char> >::iterator tiles_buf_i = tiles_buf.begin();
		for (size_t i = 0; i < reader.tiles_count; ++i, ++tiles_buf_i) {
			(*tiles_buf_i).resize(tile_buf_size); // std::fill ok - unsigned char specialization use memset
			reader.tiles[i] = &(*tiles_buf_i)[0];
		}

		//render(reader, img.pixfmt, img.width, img.height, output, !!(img.flip & TiffImage::FLIP_VERTICALLY));
		/*plcl::Render(reader, img.width, img.height, img.pixfmt,
			output, output_width, output_height, !!(img.flip & TiffImage::FLIP_VERTICALLY),
			render_buf_resample, render_buf_conv);*/
		plcl::Render(reader, img.width, img.height, img.pixfmt,
			output, output_width, output_height, !!(img.flip & TiffImage::FLIP_VERTICALLY),
			render_buf_resample, render_buf_conv,
			mirror_x, mirror_y, angle);
	} else {
		TiffStripReader reader = TiffStripReader::CreateStripReader(&img);
		reader.strip = buf.Alloc(reader.StripSize());
		memset(reader.strip, 0, buf.Size());

		//render(reader, img.pixfmt, img.width, img.height, output, !!(img.flip & TiffImage::FLIP_VERTICALLY));
		/*plcl::Render(reader, img.width, img.height, img.pixfmt,
			output, output_width, output_height, !!(img.flip & TiffImage::FLIP_VERTICALLY),
			render_buf_resample, render_buf_conv);*/
		plcl::Render(reader, img.width, img.height, img.pixfmt,
			output, output_width, output_height, !!(img.flip & TiffImage::FLIP_VERTICALLY),
			render_buf_resample, render_buf_conv,
			mirror_x, mirror_y, angle);
	}
}
