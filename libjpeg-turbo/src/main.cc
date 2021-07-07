#include "main.h"
#include "jpeg_codec.h"
#include "tjpeg_codec.h"
#include <stdio.h>

// play yuv:
//     ffplay -f rawvideo -pixel_format nv12 -video_size 650x850  nv12.yuv 

void jpeg_decode_test()
{
    FileReader file("../test_data/xiyang.jpeg");
    // FileReader file("../test_data/testorig.jpg");
    // FileReader file("image.jpg");
    if (file.is_error())
        return;

    cout << "file size: " << file.size() << endl;
    Frame *frame = nullptr;

    image_alloc_frame(&frame);
    if (jpeg_decode(file.data(), file.size(), frame) == 0) {
      image_save_frame("nv12.yuv", frame);
    }
    image_free_frame(&frame);
}

void tjpeg_encode_test()
{
    FileReader file("../test_data/lena_128x128.yuv");
    if (file.is_error())
        return;
    cout << "file size: " << file.size() << endl;

    tjpeg_encode("out.jpg", file.data(), file.size(), 128, 128, IMAGE_PIXFMT_YUV420P);

    Frame frame;
    frame.fmt = IMAGE_PIXFMT_YUV420P;
    frame.width = 128;
    frame.height = 128;
    frame.plane[0] = file.data();
    frame.plane[1] = file.data() + frame.width * frame.height;
    frame.plane[2] = file.data() + frame.width * frame.height *5/4;
    frame.stride[0] = frame.width;
    frame.stride[1] = frame.width/2;
    frame.stride[2] = frame.width/2;
    tjpeg_encode("out2.jpg", &frame);
}

int main(int argc, char* argv[])
{
    // jpeg_decode_test();
    tjpeg_encode_test();
    return 0;
}