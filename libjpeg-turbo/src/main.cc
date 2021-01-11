#include <stdio.h>
#include "main.h"
#include "jpeg_codec.h"

int save_nv12(const char *filename, uint8_t *buf, int width, int height, int stride)
{
  FILE *fp = fopen(filename, "wb+");
  if (!fp) {
    cout << "fopen failed" << endl;
    return -1;
  }

  int rowbytes = width;
  uint8_t *ptr = buf;

  //write Y
  for (int i = 0; i < height; ++i) {
    fwrite(ptr, rowbytes, 1, fp);
    ptr += stride;
  }
  //write UV
  for (int i = 0; i < height/2; ++i) {
    fwrite(ptr, rowbytes, 1, fp);
    ptr += stride;
  }

  fclose(fp);
  return 0;
}

void test()
{
  // FileReader file("../test_data/xiyang.jpeg");
  // FileReader file("../test_data/testorig.jpg");
  FileReader file("image.jpg");
  if (file.is_error())
    return;
  
  cout << "file size: " << file.size() << endl;
  
  JpgCodec *codec = JpgCreateCodec();
  JpgDecode(codec, file.data(), file.size());
  save_nv12("nv12.yuv", codec->data, codec->width, codec->height, codec->stride);
  JpgDestroyCodec(codec);
}

int main(int argc, char *argv[])
{
  test();
  return 0;
}