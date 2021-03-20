#include "main.h"
#include "jpeg_codec.h"
#include <stdio.h>

// play yuv:
//     ffplay -f rawvideo -pixel_format nv12 -video_size 650x850  nv12.yuv 

void test()
{
    FileReader file("../test_data/xiyang.jpeg");
    // FileReader file("../test_data/testorig.jpg");
    // FileReader file("image.jpg");
    if (file.is_error())
        return;

    cout << "file size: " << file.size() << endl;
    Frame *frame = nullptr;

    image_alloc_frame(&frame);
    if (image_decode(file.data(), file.size(), frame) == 0) {
      image_save_frame("nv12.yuv", frame);
    }
    image_free_frame(&frame);
}

int main(int argc, char* argv[])
{
    test();
    return 0;
}