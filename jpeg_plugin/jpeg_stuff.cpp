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
#include <basic.h>

#include "jpeg_stuff.h"

#include <trace.h>

/*
 * Error exit handler: must not return to caller.
 *
 * Applications may override this if they want to get control back after
 * an error.  Typically one would longjmp somewhere instead of exiting.
 * The setjmp buffer can be made a private field within an expanded error
 * handler object.  Note that the info needed to generate an error message
 * is stored in the error object, so you can generate the message now or
 * later, at your convenience.
 * You should make sure that the JPEG object is cleaned up (with jpeg_abort
 * or jpeg_destroy) at some point.
 */
METHODDEF(void)
jpeg_error_exit(j_common_ptr cinfo) {
	// always display the message
	(*cinfo->err->output_message)(cinfo);

	// allow JPEG with a premature end of file
	if ((cinfo)->err->msg_parm.i[0] != 13) {
		// let the memory manager delete any temp files before we die
		jpeg_destroy(cinfo);
		
		// throw jpeg_exception();
		JpegErrorManager* err = (JpegErrorManager*)cinfo->err;
		longjmp(err->jexit, 1);
	}
}

/*
 * Actual output of an error or trace message.
 * Applications may override this method to send JPEG messages somewhere
 * other than stderr.
 *
 * On Windows, printing to stderr is generally completely useless,
 * so we provide optional code to produce an error-dialog popup.
 * Most Windows applications will still prefer to override this routine,
 * but if they don't, it'll do something at least marginally useful.
 *
 * NOTE: to use the library in an environment that doesn't support the
 * C stdio library, you may have to delete the call to fprintf() entirely,
 * not just not use this routine.
 */
METHODDEF(void)
jpeg_output_message(j_common_ptr cinfo) {
	char buffer[JMSG_LENGTH_MAX];

	// create the message
	(*cinfo->err->format_message)(cinfo, buffer);
	// send it to user's message proc
	cpcl::Trace(CPCL_TRACE_LEVEL_ERROR,
		"jpeglib:: %s",
		buffer);
}

JpegStuff::JpegStuff() {
	// Step 1: allocate and initialize JPEG decompression object
	cinfo.err = jpeg_std_error(&jerr);
	jerr.error_exit     = jpeg_error_exit;
	jerr.output_message = jpeg_output_message;
	
	jpeg_create_decompress(&cinfo);
}
JpegStuff::~JpegStuff() {
	/* free only data allocated with memory manager, for example with cinfo->mem->alloc_small */
	jpeg_destroy_decompress(&cinfo);
}

// ----------------------------------------------------------
//   Source manager
// ----------------------------------------------------------

/*
 * Initialize source.  This is called by jpeg_read_header() before any
 * data is actually read. Unlike init_destination(), it may leave
 * bytes_in_buffer set to 0 (in which case a fill_input_buffer() call
 * will occur immediately).
 */
METHODDEF(void)
method_init_source(j_decompress_ptr cinfo) {
	JpegInputManager *src = (JpegInputManager*)cinfo->src;
	src->next_input_byte = src->buffer;
	src->bytes_in_buffer = 0;
	src->input->Seek(0, SEEK_SET, NULL);
}

/*
 * This is called whenever bytes_in_buffer has reached zero and more
 * data is wanted.  In typical applications, it should read fresh data
 * into the buffer (ignoring the current state of next_input_byte and
 * bytes_in_buffer), reset the pointer & count to the start of the
 * buffer, and return TRUE indicating that the buffer has been reloaded.
 * It is not necessary to fill the buffer entirely, only to obtain at
 * least one more byte. bytes_in_buffer MUST be set to a positive value
 * if TRUE is returned. A FALSE return should only be used when I/O
 * suspension is desired.
 */
METHODDEF(boolean)
method_fill_input_buffer(j_decompress_ptr cinfo) {
	JpegInputManager *src = (JpegInputManager*)cinfo->src;
	/*if (src->fDecoder != NULL && src->fDecoder->shouldCancelDecode())
		return FALSE;*/
	size_t bytes = src->input->Read(src->buffer, arraysize(src->buffer));
	// note that JPEG is happy with less than the full read,
	// as long as the result is non-zero
	if (bytes == 0)
		return FALSE;
	
	src->next_input_byte = src->buffer;
	src->bytes_in_buffer = bytes;
	return TRUE;
}

/*
 * Skip num_bytes worth of data.  The buffer pointer and count should
 * be advanced over num_bytes input bytes, refilling the buffer as
 * needed. This is used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker). In some applications
 * it may be possible to optimize away the reading of the skipped data,
 * but it's not clear that being smart is worth much trouble; large
 * skips are uncommon. bytes_in_buffer may be zero on return.
 * A zero or negative skip count should be treated as a no-op.
 */
METHODDEF(void)
method_skip_input_data(j_decompress_ptr cinfo, long num_bytes) {
	JpegInputManager *src = (JpegInputManager*)cinfo->src;
	
	if (num_bytes > 0) {
	if (num_bytes > (long)src->bytes_in_buffer) {
		long skip_bytes = num_bytes - src->bytes_in_buffer;
		src->input->Seek(skip_bytes, SEEK_CUR, NULL);

		src->next_input_byte = src->buffer;
		src->bytes_in_buffer = 0;
	} else {
		src->next_input_byte += num_bytes;
		src->bytes_in_buffer -= num_bytes;
	}
	}
}

//METHODDEF(boolean)
//resync_to_restart(j_decompress_ptr cinfo, int desired) {
//	JpegInputManager *src = (JpegInputManager*)cinfo->src;
//	
//	// what is the desired param for???
//	src->input->Seek(0, SEEK_SET, NULL);
//	
//	src->next_input_byte = src->buffer;
//	src->bytes_in_buffer = 0;
//	return TRUE;
//}

/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */
METHODDEF(void)
method_term_source(j_decompress_ptr /*cinfo*/) {}

JpegInputManager::JpegInputManager(cpcl::IOStream *input_) : input(input_) {
	init_source = method_init_source;
  fill_input_buffer = method_fill_input_buffer;
  skip_input_data = method_skip_input_data;
  resync_to_restart = jpeg_resync_to_restart; /* use default method */
  term_source = method_term_source;
  bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	next_input_byte = NULL; /* until buffer loaded */
}
