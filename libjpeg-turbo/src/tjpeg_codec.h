#pragma once

#include "frame.h"

int tjpeg_encode(const char *filename, Frame *f); 
int tjpeg_encode(const char *filename, uint8_t *buf, int size, int width, int height, ImagePixFmt format); 

int tjpeg_decode(const char *filename, uint8_t *buf, int size, ImagePixFmt format); 
