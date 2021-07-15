#include "frame.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

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
    delete[] frame->data;
    delete frame;

    *f = nullptr;
    return 0;
}

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

int image_save_file(const char* filename, uint8_t *buf, int size)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("fopen %s failed. \n", filename);
        return -1;
    }
    fwrite(buf, size, 1, fp);
    fclose(fp);
    return 0;
}
