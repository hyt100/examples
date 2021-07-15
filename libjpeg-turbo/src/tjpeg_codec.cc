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
    if (format != IMAGE_PIXFMT_YUV420P && format != IMAGE_PIXFMT_RGB24) {
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

    int ret;
    if (format == IMAGE_PIXFMT_YUV420P) {
        ret = tjCompressFromYUV(handle, (const unsigned char *)buf, 
            width, 1, height, TJSAMP_420, &jpegBuf, &jpegSize, DEFAULT_QUALITY, 0);
    } else {
        ret = tjCompress2(handle, (const unsigned char *)buf, 
            width, 0, height, TJPF_RGB, &jpegBuf, &jpegSize, TJSAMP_420, DEFAULT_QUALITY, 0);
    }
    if (ret != 0) {
        printf("Compress failed: %s \n", tjGetErrorStr2(handle));
        tjDestroy(handle);
        return -1;
    }

    image_save_file(filename, jpegBuf, jpegSize);

    tjFree(jpegBuf);
    tjDestroy(handle);
    return 0;
}

int tjpeg_decode(const char *filename, uint8_t *buf, int size, ImagePixFmt format)
{
    if (format != IMAGE_PIXFMT_YUV && format != IMAGE_PIXFMT_RGB24) {
        printf("Not support pixel format. \n");
        return -1;
    }

    tjhandle handle = tjInitDecompress();
    if (!handle) {
        printf("tjInitDecompress failed: %s \n", tjGetErrorStr2(handle));
        return -1;
    }

    int img_width, img_height, img_subsamp, img_colorspace;
    int flags = 0, pixelfmt = TJPF_RGB;

    int ret = tjDecompressHeader3(handle, (unsigned char *)buf, size, &img_width, &img_height, &img_subsamp, &img_colorspace);
    if (0 != ret) {
        tjDestroy(handle);
        printf("tjDecompressHeader3 failed: %s \n", tjGetErrorStr2(handle));
        return -1;
    }
    printf("jpeg width: %d\n", img_width);
    printf("jpeg height: %d\n", img_height);
    printf("jpeg subsamp: %d\n", img_subsamp);
    printf("jpeg colorspace: %d\n", img_colorspace);

    int img_size = img_width * img_height * 3;
    uint8_t *img_data = new uint8_t[img_size];

    if (format == IMAGE_PIXFMT_RGB24) {
        ret = tjDecompress2(handle, (unsigned char *)buf, size, img_data, img_width, 0, img_height, TJPF_RGB, 0);
    } else {
        ret = tjDecompressToYUV2(handle, (unsigned char *)buf, size, img_data, img_width, 1, img_height, 0);
    }

    if (0 != ret) {
        tjDestroy(handle);
        delete[] img_data;
        printf("Decompress failed: %s \n", tjGetErrorStr2(handle));
        return -1;
    }

    if (format == IMAGE_PIXFMT_YUV) {
        // 解压后的YUV的格式，是由JPEG图片的采样格式决定的，如果JPEG本身是YUV420，则解压得到的YUV，就是YUV420格式。
        img_size = tjBufSizeYUV2(img_width, 1, img_height, img_subsamp);
    }

    image_save_file(filename, img_data, img_size);
    
    delete[] img_data;
    tjDestroy(handle);
    return 0;
}
