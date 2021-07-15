#pragma once

#include <stdint.h>

enum ImagePixFmt {
    IMAGE_PIXFMT_NV12 = 0,
    IMAGE_PIXFMT_RGB24,
    IMAGE_PIXFMT_YUV420P,
    IMAGE_PIXFMT_YUV,  //某种YUV格式
    IMAGE_PIXFMT_MAX
};

struct Frame
{
    ImagePixFmt fmt;
    int width;
    int height;
    int stride[4];
    uint8_t *plane[4];
    uint8_t *data;
};

int image_save_file(const char* filename, uint8_t *buf, int size);

int image_save_frame(const char* filename, Frame *f);  //support: NV12

int image_alloc_frame(Frame **f);
int image_free_frame(Frame **f);