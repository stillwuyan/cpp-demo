/*
 * The MIT License (MIT)
 *
 * Copyright © 2014 faith
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

/*======================================
	Header include
======================================*/

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "convert.h"


#include <jpeglib.h>

/*======================================
	Macro
======================================*/

#define ROUND(val, min, max) do {	\
	if (val < min) val = min;		\
	else if (val > max) val = max;	\
} while(0)


/*======================================
	Public function
======================================*/

bool
convert_yuyv_to_bgrx8888(void *dst, void *src, uint32_t width, uint32_t height, bool use_egl)
{
    if (use_egl) {
        memcpy(dst, src, width * height * 2);
        return true;
    }

	unsigned char *src_p = src;
	unsigned char *dst_p = dst;

	int i, j, k = 0;

	if (!src || !dst || !width || !height)
		return false;

	/*
	 * YUV to RGB
	 *
	 * R = Y +                   1.4020(V - 128)
	 * G = Y - 0.3441(U - 128) - 0.7139(V - 128)
	 * B = Y + 1.7718(U - 128) - 0.0012(V - 128)
	 *
	 * ->
	 * 256R = 256Y +                359(V - 128)
	 * 256G = 256Y -  88(U - 128) - 183(V - 128)
	 * 256B = 256Y + 454(U - 128) -   0(V - 128)
	 *
	 * ->
	 * 256R = 256Y + [                359(V - 128) ]
	 * 256G = 256Y - [  88(U - 128) + 183(V - 128) ]
	 * 256B = 256Y + [ 454(U - 128)                ]
	 *
	 * ->
	 * R = Y + [                359(V - 128) ] >> 8
	 * G = Y - [  88(U - 128) + 183(V - 128) ] >> 8
	 * B = Y + [ 454(U - 128)                ] >> 8
	 *
	 */
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			int y, u, v;
			int r, g, b;

			y = src_p[2 * k];
			u = src_p[1] - 128;
			v = src_p[3] - 128;

			r = y + (             (359 * v)  >> 8);
			g = y - ((( 88 * u) + (183 * v)) >> 8);
			b = y + ( (454 * u)              >> 8);

			ROUND(r, 0, 255);
			ROUND(g, 0, 255);
			ROUND(b, 0, 255);

            dst_p[0] = b;
            dst_p[1] = g;
            dst_p[2] = r;
			dst_p[3] = 0xff;

			dst_p += 4;

			if (k++) {
				k = 0;
				src_p += 4;
			}
		}
	}

	return true;
}


bool convert_jpeg_to_bgrx8888(void *_bgrx_buffer, void *jpeg_data, unsigned char* rgb_buffer, uint32_t jpeg_size, uint32_t width, uint32_t height, bool use_egl)
{
    unsigned char *bgrx_buffer = _bgrx_buffer;

    // 1. JPEG解码
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, jpeg_data, jpeg_size);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    // 为原始像素数据分配内存:
    //unsigned char* rgb_buffer = (unsigned char*)malloc(cinfo.output_width * cinfo.output_height * 3);

    if (width !=  cinfo.output_width || height !=  cinfo.output_height)
    {
        printf("convert_jpeg_to_bgrx8888 error: width:%d height:%d output_width:%d output_height:%d\n", width, height, cinfo.output_width, cinfo.output_height);
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        //free(rgb_buffer);
        return false;
    }

    // 读取像素数据（这通常是RGB或YCbCr格式）
    unsigned char* row_pointer[1];
    while (cinfo.output_scanline < cinfo.output_height) {
        row_pointer[0] = &rgb_buffer[cinfo.output_scanline * cinfo.output_width * 3];
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
        //usleep(1);
    }

    // 2. 像素格式转换（从RGB到BGRX）
    for (int y = 0; y < cinfo.output_height; y++) {
        for (int x = 0; x < cinfo.output_width; x++) {
            int index_rgb = y * cinfo.output_width * 3 + x * 3;
            int index_bgrx = y * cinfo.output_width * 4 + x * 4;
            if (use_egl) {
                bgrx_buffer[index_bgrx + 0] = rgb_buffer[index_rgb + 0]; // B
                bgrx_buffer[index_bgrx + 1] = rgb_buffer[index_rgb + 1]; // G
                bgrx_buffer[index_bgrx + 2] = rgb_buffer[index_rgb + 2]; // R
            } else {
                bgrx_buffer[index_bgrx + 0] = rgb_buffer[index_rgb + 2]; // B
                bgrx_buffer[index_bgrx + 1] = rgb_buffer[index_rgb + 1]; // G
                bgrx_buffer[index_bgrx + 2] = rgb_buffer[index_rgb + 0]; // R
            }
            bgrx_buffer[index_bgrx + 3] = 0xFF; // X 或 Alpha，设为255或其他值
        }
    }

    // 3. 清理和释放内存
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    //free(rgb_buffer);

    return true;
}
