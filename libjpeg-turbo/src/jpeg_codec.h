#pragma once

#include <stdint.h>

typedef struct {
  uint8_t *data;
  int stride;
  int width;
  int height;
} JpgCodec;

JpgCodec* JpgCreateCodec();
int JpgDecode(JpgCodec *codec, uint8_t *buffer_data, uint64_t buffer_size);
int JpgDestroyCodec(JpgCodec *codec);