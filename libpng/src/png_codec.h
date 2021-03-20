#pragma once

#include <stdint.h>

enum ImagePixFmt {
    IMAGE_PIXFMT_NV12 = 0,
    IMAGE_PIXFMT_RGB24
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

int image_alloc_frame(Frame **f);
int image_free_frame(Frame **f);
int image_save_frame(const char* filename, Frame *f);
int image_decode(uint8_t *buf, int len, Frame *f);
