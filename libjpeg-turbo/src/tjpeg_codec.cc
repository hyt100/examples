#include "tjpeg_codec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <turbojpeg.h>

#define DEFAULT_QUALITY  95


int tjpeg_encode(const char *filename, Frame *f)
{
    if (f->fmt != IMAGE_PIXFMT_YUV420P) {
        printf("Not support pixel format. \n");
        return -1;
    }

    tjhandle handle = tjInitCompress();
    if (!handle) {
        printf("tjInitCompress failed. \n");
        return -1;
    }
    unsigned char *jpegBuf = NULL; 
    unsigned long jpegSize = 0;

    int ret = tjCompressFromYUVPlanes(handle, (const unsigned char **)f->plane, 
            f->width, (const int *)f->stride, f->height, TJSAMP_420, &jpegBuf, &jpegSize, DEFAULT_QUALITY, 0);
    if (ret != 0) {
        printf("tjCompressFromYUVPlanes failed: %s \n", tjGetErrorStr2(handle));
        tjDestroy(handle);
        return -1;
    }

    image_save_file(filename, jpegBuf, jpegSize);

    tjFree(jpegBuf);
    tjDestroy(handle);
    return 0;
}

int tjpeg_encode(const char *filename, uint8_t *buf, int size, int width, int height, ImagePixFmt format)
{
    if (format != IMAGE_PIXFMT_YUV420P) {
        printf("Not support pixel format. \n");
        return -1;
    }

    tjhandle handle = tjInitCompress();
    if (!handle) {
        printf("tjInitCompress failed. \n");
        return -1;
    }
    unsigned char *jpegBuf = NULL; 
    unsigned long jpegSize = 0;

    int ret = tjCompressFromYUV(handle, (const unsigned char *)buf, 
            width, 1, height, TJSAMP_420, &jpegBuf, &jpegSize, DEFAULT_QUALITY, 0);
    if (ret != 0) {
        printf("tjCompressFromYUV failed: %s \n", tjGetErrorStr2(handle));
        tjDestroy(handle);
        return -1;
    }

    image_save_file(filename, jpegBuf, jpegSize);

    tjFree(jpegBuf);
    tjDestroy(handle);
    return 0;
}

