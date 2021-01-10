#include "jpeg_codec.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include "jpeglib.h"
#include <setjmp.h>

#define ALIGN_N(n, num)  (((n) + (num) - 1) & (~((num) - 1)))
#define ALIGN_16(n)      ALIGN_N(n, 16)

struct my_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */

  jmp_buf setjmp_buffer;        /* for return to caller */
};

typedef struct my_error_mgr *my_error_ptr;

void yuv444_to_nv12(uint8_t *nv12, int nv12_stride, uint8_t *yuv444, int yuv444_stride, int width, int height)
{
  int yIndex = 0;
  int uvIndex = nv12_stride * height;
  int padding = nv12_stride - width;
  uint8_t y, u, v;

  for (int i; i < height; ++i) {
    for (int j; j < width; ++j) {
      int start = i * yuv444_stride + j * 3;
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

void my_error_exit(j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr)cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

static int do_read_jpeg_data(struct jpeg_decompress_struct *cinfo, JpgCodec *codec, uint8_t *buffer_data, uint64_t buffer_size)
{
  struct my_error_mgr jerr;
  JSAMPARRAY buffer;            /* Output row buffer */
  int row_stride;               /* physical row width in output buffer */

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
    std::cout << "decode error" << std::endl;
    return 0;
  }
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(cinfo);

  /* Step 2: specify data source (eg, a file) */

  jpeg_mem_src(cinfo, buffer_data, buffer_size);

  /* Step 3: read file parameters with jpeg_read_header() */

  if (jpeg_read_header(cinfo, TRUE) == JPEG_SUSPENDED) {
    std::cout << "read header error" << std::endl;
    jpeg_destroy_decompress(cinfo);
    return 0;
  }

  /* Step 4: set parameters for decompression */

  std::cout << "in width: "  << cinfo->image_width << std::endl;
  std::cout << "in height: " << cinfo->image_height << std::endl;
  std::cout << "in components: "  << cinfo->num_components << std::endl;
  std::cout << "in color space: "  << cinfo->jpeg_color_space << std::endl;

  cinfo->out_color_space = JCS_YCbCr;

  /* Step 5: Start decompressor */

  (void)jpeg_start_decompress(cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  std::cout << "out width: "  << cinfo->output_width << std::endl;
  std::cout << "out height: " << cinfo->output_height << std::endl;
  std::cout << "out components: "  << cinfo->output_components << std::endl;
  std::cout << "out color components: "  << cinfo->out_color_components << std::endl;

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */
  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo->output_width * cinfo->output_components;
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo->mem->alloc_sarray)
                ((j_common_ptr)cinfo, JPOOL_IMAGE, row_stride, 1);

  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */

  /* Here we use the library's state variable cinfo->output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */
  int expect_width = (cinfo->output_width % 2 == 0) ? cinfo->output_width : (cinfo->output_width - 1);
  int expect_hight = (cinfo->output_height % 2 == 0) ? cinfo->output_height : (cinfo->output_height - 1);
  int expect_sride = expect_width * cinfo->output_components;
  uint8_t *expect_data = new uint8_t[expect_sride * expect_hight];
  uint8_t *out_ptr = expect_data;

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

    if (cinfo->output_scanline < expect_hight) {
      memcpy(out_ptr, buffer[0], expect_sride);
      out_ptr += expect_sride;
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

  int nv12_stride = ALIGN_16(expect_width);
  int nv12_width = expect_width;
  int nv12_height = expect_hight;
  int nv12_size = nv12_stride * nv12_height * 3 / 2;
  uint8_t *nv12 = new uint8_t[nv12_size];

  std::cout << "nv12 width: "  << nv12_width << std::endl;
  std::cout << "nv12 height: " << nv12_height << std::endl;
  std::cout << "nv12 stride: " << nv12_stride << std::endl;
  std::cout << "nv12 size: " << nv12_size << std::endl;
  std::cout << "nv12 real size: " << (nv12_width * nv12_height * 3 / 2) << std::endl;

  yuv444_to_nv12(nv12, nv12_stride, expect_data, expect_sride, nv12_width, nv12_height);

  codec->data = nv12;
  codec->width = nv12_width;
  codec->height = nv12_height;
  codec->stride = nv12_stride;

  std::cout << "decode ok" << std::endl;
  delete[] expect_data;
  return 0;
}

int JpgDecode(JpgCodec *codec, uint8_t *buffer_data, uint64_t buffer_size)
{
  struct jpeg_decompress_struct cinfo;
  return do_read_jpeg_data(&cinfo, codec, buffer_data, buffer_size);
}

JpgCodec* JpgCreateCodec()
{
  JpgCodec* codec = new JpgCodec;
  codec->data = nullptr;
  return codec;
}

int JpgDestroyCodec(JpgCodec *codec)
{
  if (codec) {
    delete[] codec->data;
    delete codec;
  }
  return 0;
}
