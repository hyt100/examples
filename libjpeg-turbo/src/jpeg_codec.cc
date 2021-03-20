#include <iostream>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include "jpeg_codec.h"
#include "jpeglib.h"

#define ALIGN_N(n, num) (((n) + (num)-1) & (~((num)-1)))
#define ALIGN_4(n)  ALIGN_N(n, 4)
#define ALIGN_8(n)  ALIGN_N(n, 8)
#define ALIGN_16(n) ALIGN_N(n, 16)

struct my_error_mgr {
    struct jpeg_error_mgr pub; /* "public" fields */

    jmp_buf setjmp_buffer; /* for return to caller */
};

typedef struct my_error_mgr* my_error_ptr;

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

static void rgb24_to_nv12(uint8_t* nv12, int nv12_stride, int nv12_width, int nv12_height, uint8_t* rgb, int rgb_stride)
{
    int yIndex = 0;
    int uvIndex = nv12_stride * nv12_height;
    int padding = nv12_stride - nv12_width;
    uint8_t y, u, v;
    int Y, U, V, R, G, B;

    for (int i = 0; i < nv12_height; ++i) {
        int row_start = i * rgb_stride;

        for (int j = 0; j < nv12_width; ++j) {
            int start = row_start + j * 3;
            R = rgb[start];
            G = rgb[start + 1];
            B = rgb[start + 2];

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

static void yuv444_to_nv12(uint8_t* nv12, int nv12_stride, int nv12_width, int nv12_height, uint8_t* yuv444, int yuv444_stride)
{
    int yIndex = 0;
    int uvIndex = nv12_stride * nv12_height;
    int padding = nv12_stride - nv12_width;
    uint8_t y, u, v;

    for (int i = 0; i < nv12_height; ++i) {
        int row_start = i * yuv444_stride;

        for (int j = 0; j < nv12_width; ++j) {
            int start = row_start + j * 3;
            y = yuv444[start];
            u = yuv444[start + 1];
            v = yuv444[start + 2];

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

static void my_error_exit(j_common_ptr cinfo)
{
    /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
    my_error_ptr myerr = (my_error_ptr)cinfo->err;

    /* Always display the message. */
    /* We could postpone this until after returning, if we chose. */
    (*cinfo->err->output_message)(cinfo);

    /* Return control to the setjmp point */
    longjmp(myerr->setjmp_buffer, 1);
}

static int do_read_jpeg_data(struct jpeg_decompress_struct* cinfo, Frame *frame, uint8_t* buffer_data, uint64_t buffer_size)
{
    struct my_error_mgr jerr;
    JSAMPARRAY buffer; /* Output row buffer */
    int row_stride;    /* physical row width in output buffer */
    J_COLOR_SPACE color_space;
    int align_width, align_height, align_stride, align_size;
    uint8_t* align_data = nullptr;
    uint8_t* out_ptr = nullptr;
    int nv12_width, nv12_height, nv12_stride, nv12_size;
    uint8_t* nv12 = nullptr;

    /* Step 1: allocate and initialize JPEG decompression object */
    /* We set up the normal JPEG error routines, then override error_exit. */
    cinfo->err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jerr.setjmp_buffer)) {
        /* If we get here, the JPEG code has signaled an error.
         * We need to clean up the JPEG object, close the input file, and return.
         */
        jpeg_destroy_decompress(cinfo);
        std::cout << "decode error(trige longjmp)" << std::endl;
        return 0;
    }
    /* Now we can initialize the JPEG decompression object. */
    jpeg_create_decompress(cinfo);

    /* Step 2: specify data source (eg, a file) */
    jpeg_mem_src(cinfo, buffer_data, buffer_size);

    /* Step 3: read file parameters with jpeg_read_header() */
    if (jpeg_read_header(cinfo, TRUE) == JPEG_SUSPENDED) {
        std::cout << "read header error" << std::endl;
        goto ERROUT;
    }
    std::cout << "in width: " << cinfo->image_width << std::endl;
    std::cout << "in height: " << cinfo->image_height << std::endl;
    std::cout << "in components: " << cinfo->num_components << std::endl;
    std::cout << "in color space: " << cinfo->jpeg_color_space << std::endl;

    /* Step 4: set parameters for decompression */
    if (JCS_YCbCr == cinfo->jpeg_color_space) {
        color_space = JCS_YCbCr;
    } else {
        color_space = JCS_RGB;
    }
    cinfo->out_color_space = color_space;

    /* Step 5: Start decompressor */
    (void)jpeg_start_decompress(cinfo);
    /* We can ignore the return value since suspension is not possible
     * with the stdio data source.
     */
    std::cout << "out width: " << cinfo->output_width << std::endl;
    std::cout << "out height: " << cinfo->output_height << std::endl;
    std::cout << "out components: " << cinfo->output_components << std::endl;
    std::cout << "out color components: " << cinfo->out_color_components << std::endl;

    if (cinfo->output_components != 3 || cinfo->out_color_components != 3) {
        std::cout << "output components is not 3" << std::endl;
        goto ERROUT;
    }

    /* We may need to do some setup of our own at this point before reading
     * the data.  After jpeg_start_decompress() we have the correct scaled
     * output image dimensions available, as well as the output colormap
     * if we asked for color quantization.
     * In this example, we need to make an output work buffer of the right size.
     */
    /* JSAMPLEs per row in output buffer */
    row_stride = cinfo->output_width * cinfo->output_components;
    /* Make a one-row-high sample array that will go away when done with image */
    buffer = (*cinfo->mem->alloc_sarray)((j_common_ptr)cinfo, JPOOL_IMAGE, row_stride, 1);

    /* Step 6: while (scan lines remain to be read) */
    /*           jpeg_read_scanlines(...); */

    /* Here we use the library's state variable cinfo->output_scanline as the
     * loop counter, so that we don't have to keep track ourselves.
     */
    // 要输出NV12则宽高必须都为2的倍数，这里将单数的像素去掉
    align_width = (cinfo->output_width % 2 == 0) ? cinfo->output_width : (cinfo->output_width - 1);
    align_height = (cinfo->output_height % 2 == 0) ? cinfo->output_height : (cinfo->output_height - 1);
    align_stride = align_width * cinfo->output_components;
    align_size = align_stride * align_height;
    align_data = new uint8_t[align_size];
    out_ptr = align_data;

    while (cinfo->output_scanline < cinfo->output_height) {
        /* jpeg_read_scanlines expects an array of pointers to scanlines.
         * Here the array is only one element long, but you could ask for
         * more than one scanline at a time if that's more convenient.
         */
        (void)jpeg_read_scanlines(cinfo, buffer, 1);
        /* Assume put_scanline_someplace wants a pointer and sample count. */
        //put_scanline_someplace(buffer[0], row_stride);
        //memcpy(out_ptr, buffer[0], row_stride);
        //out_ptr += row_stride;

        if (cinfo->output_scanline < align_height) {
            memcpy(out_ptr, buffer[0], align_stride);
            out_ptr += align_stride;
        }
    }

    /* Step 7: Finish decompression */
    (void)jpeg_finish_decompress(cinfo);
    /* We can ignore the return value since suspension is not possible
     * with the stdio data source.
     */

    /* Step 8: Release JPEG decompression object */
    /* This is an important step since it will release a good deal of memory. */
    jpeg_destroy_decompress(cinfo);

    nv12_stride = align_width;
    nv12_width  = align_width;
    nv12_height = align_height;
    nv12_size = nv12_stride * nv12_height * 3 / 2;
    nv12 = new uint8_t[nv12_size];

    if (JCS_YCbCr)
        yuv444_to_nv12(nv12, nv12_stride, nv12_width, nv12_height, align_data, align_stride);
    else
        rgb24_to_nv12(nv12, nv12_stride, nv12_width, nv12_height, align_data, align_stride);

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
    delete[] align_data;
    return 0;

ERROUT:
    jpeg_destroy_decompress(cinfo);
    delete[] align_data;
    return -1;
}

int image_decode(uint8_t *buf, int len, Frame *frame)
{
    struct jpeg_decompress_struct cinfo;
    return do_read_jpeg_data(&cinfo, frame, buf, len);
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