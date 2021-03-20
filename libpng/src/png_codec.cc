#include <iostream>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <libpng16/png.h>
#include "png_codec.h"

#define ALIGN_N(n, num) (((n) + (num)-1) & (~((num)-1)))
#define ALIGN_4(n)  ALIGN_N(n, 4)
#define ALIGN_8(n)  ALIGN_N(n, 8)
#define ALIGN_16(n) ALIGN_N(n, 16)

#define PNG_BYTES_TO_CHECK   8

typedef struct {
    uint8_t *buffer_data;
    uint32_t buffer_size;
    uint32_t offset;
} PngIOContex;


int image_save_frame(const char* filename, Frame *f)
{
    if (f == nullptr || f->data == nullptr) {
        std::cout << "invalid frame" << std::endl;
        return -1;
    }

    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        std::cout << "fopen failed" << std::endl;
        return -1;
    }

    int rowbytes = f->width;
    uint8_t* ptr;

    //write Y
    ptr = f->plane[0];
    for (int i = 0; i < f->height; ++i) {
        fwrite(ptr, rowbytes, 1, fp);
        ptr += f->stride[0];
    }

    //write UV
    ptr = f->plane[1];
    for (int i = 0; i < f->height / 2; ++i) {
        fwrite(ptr, rowbytes, 1, fp);
        ptr += f->stride[1];
    }

    fclose(fp);
    return 0;
}

static void rgb_to_nv12(uint8_t* nv12, int nv12_stride, int nv12_width, int nv12_height, png_bytepp rgb, int rgb_pixel_size)
{
    int yIndex = 0;
    int uvIndex = nv12_stride * nv12_height;
    int padding = nv12_stride - nv12_width;
    uint8_t y, u, v;
    int Y, U, V, R, G, B;

    for (int i = 0; i < nv12_height; ++i) {
        for (int j = 0; j < nv12_width; ++j) {
            int start = j * rgb_pixel_size;
            R = rgb[i][start];
            G = rgb[i][start + 1];
            B = rgb[i][start + 2];

            // RGB to YUV
            Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
            U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
            V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;

            y = (uint8_t)((Y < 0) ? 0 : ((Y > 255) ? 255 : Y));
            u = (uint8_t)((U < 0) ? 0 : ((U > 255) ? 255 : U));
            v = (uint8_t)((V < 0) ? 0 : ((V > 255) ? 255 : V));

            nv12[yIndex++] = y;
            if (i % 2 == 0 && j % 2 == 0) {
                nv12[uvIndex++] = u;
                nv12[uvIndex++] = v;
            }
        }

        yIndex += padding;
        if (i % 2 == 0) {
            uvIndex += padding;
        }
    }
}

static void png_read_callback(png_structp png_ptr, png_bytep p, size_t size)
{
    if (png_ptr == NULL)
        return;
    
    PngIOContex *ctx = (PngIOContex *)png_get_io_ptr(png_ptr);
    if (ctx->buffer_size - ctx->offset < size) {
        std::cout << "png read buffer out of range." << std::endl;
        png_error(png_ptr, "Read Error"); /* Fatal error in PNG image of libpng - can't continue */
        return;
    }
    memcpy(p, ctx->buffer_data + ctx->offset, size);
    ctx->offset += size;
}

int image_decode(uint8_t *buf, int len, Frame *frame)
{
    // check sig
    if (png_sig_cmp((png_const_bytep)buf, 0, PNG_BYTES_TO_CHECK) != 0) {
        std::cout << "not png file" << std::endl;
        return -1;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        std::cout << "png_create_read_struct failed" << std::endl;
        return -1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        std::cout << "png_create_read_struct failed" << std::endl;
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        return -1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        std::cout << "decode error (trige longjmp)" << std::endl;
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        return -1;
    }

    PngIOContex contex;
    contex.buffer_data = buf;
    contex.buffer_size = len;
    contex.offset = 0;

    png_set_read_fn(png_ptr, &contex, (png_rw_ptr)png_read_callback);
    png_set_sig_bytes(png_ptr, 0);

#if 0
    int transforms = PNG_TRANSFORM_IDENTITY;
#else
    int transforms = PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING;
#endif

    png_read_png(png_ptr, info_ptr, transforms, NULL);

    uint32_t width = png_get_image_width(png_ptr, info_ptr);
    uint32_t height = png_get_image_height(png_ptr, info_ptr);
    uint32_t color_type = png_get_color_type(png_ptr, info_ptr);
    uint32_t bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    uint32_t row_bytes = png_get_rowbytes(png_ptr, info_ptr);

    std::cout << "color type: " << color_type << std::endl;
    std::cout << "bit depth: " << bit_depth << std::endl;
    std::cout << "row bytes: " << row_bytes << std::endl;
    std::cout << "width: " << width << std::endl;
    std::cout << "height" << height << std::endl;

    if (bit_depth != 8 || (color_type != PNG_COLOR_TYPE_RGB && color_type != PNG_COLOR_TYPE_RGBA)) {
        std::cout << "not support png format" << std::endl;
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        return -1;
    }
    int bytes_per_pixel = (color_type == PNG_COLOR_TYPE_RGB) ? 3 : 4;

    int nv12_width = (width % 2 == 0) ? width : width-1;
    int nv12_height = (height % 2 == 0) ? height : height-1;
    int nv12_stride = width;
    int nv12_size = nv12_stride * nv12_height * 3/2;
    uint8_t *nv12 = new uint8_t[nv12_size];

    /* row_pointers(row_pp) is an array of pointers to the pixel data for each row: png_bytep row_pointers[height] */
    png_bytepp row_pp = png_get_rows(png_ptr, info_ptr);

    rgb_to_nv12(nv12, nv12_stride, nv12_width, nv12_height, row_pp, bytes_per_pixel);

    std::cout << "nv12 width: " << nv12_width << std::endl;
    std::cout << "nv12 height: " << nv12_height << std::endl;
    std::cout << "nv12 stride: " << nv12_stride << std::endl;
    std::cout << "nv12 size: " << nv12_size << std::endl;

    frame->fmt = IMAGE_PIXFMT_NV12;
    frame->data = nv12;
    frame->width = nv12_width;
    frame->height = nv12_height;
    frame->stride[0] = nv12_width;
    frame->stride[1] = nv12_width;
    frame->plane[0] = frame->data;
    frame->plane[1] = frame->data + frame->stride[0] * frame->height;

    std::cout << "decode ok" << std::endl;

    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    return 0;
}

int image_alloc_frame(Frame **f)
{
    Frame *frame = new Frame;
    frame->width = 0;
    frame->height = 0;
    frame->data = nullptr;
    for (auto i = 0; i < 4; ++i) {
        frame->plane[i] = nullptr;
        frame->stride[i] = 0;
    }

    *f = frame;
    return 0;
}

int image_free_frame(Frame **f)
{
    if (f == nullptr || *f == nullptr)
        return -1;

    Frame *frame = *f;
    delete frame->data;
    delete frame;

    *f = nullptr;
    return 0;
}