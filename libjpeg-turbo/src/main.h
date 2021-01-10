#pragma once
#include <iostream>
#include <stdio.h>
#include <stdint.h>

using std::cout;
using std::endl;

class FileReader
{
  public:
    FileReader(const char *filename)
      :size_(0), data_(nullptr), error_(true)
    {
      if (filename == nullptr)
        return;
      
      FILE *fp = fopen(filename, "rb");
      if (fp == nullptr) {
        std::cout << "fopen failed" << std::endl;
        return;
      }

      fseek(fp, 0L, SEEK_END);
      int size = ftell(fp);
      fseek(fp, 0L, SEEK_SET);

      uint8_t *data = new uint8_t[size];
      fread(data, size, 1, fp);
      fclose(fp);

      data_ = data;
      size_ = size;
      error_ = false;
    }

    ~FileReader()
    {
      delete[] data_;
    }

    int size() { return size_; }

    uint8_t *data() { return data_; }

    bool is_error() { return error_;  }
  
  private:
    int size_;
    uint8_t *data_;
    bool error_;
};