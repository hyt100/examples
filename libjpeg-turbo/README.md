**解码**

解压输出参数设为`JCS_YCbCr`时，输出的格式为YUV444，每个像素三个字节，存储顺序为YUV，例如4x3存储如下：

```
YUVYUVYUVYUV
YUVYUVYUVYUV
YUVYUVYUVYUV
```
解压输出参数设为`JCS_RGB`时，输出的格式为RGB，每个像素三个字节，存储顺序为RGB，例如4x3存储如下：

```
RGBRGBRGBRGB
RGBRGBRGBRGB
RGBRGBRGBRGB
```

>输入为灰度图(JCS_GRAYSCALE)时，如果输出设为`JCS_YCbCr`时，将会解码失败。经测试，如果输入为`JCS_YCbCr`，输出设为`JCS_YCbCr`，输入为其他时，输出设为`JCS_RGB`，暂时未发现解码失败问题。

**yuv补充像素**

```
白色：Y=0xff  U/V=0x80
黑色：Y=0x00  U/V=0x80
```

**FFMPEG命令**
nv12转jpg:

```shell
ffmpeg -y -s 650x850 -pix_fmt nv12 -i nv12.yuv image.jpg
```
查看nv12文件：
```shell
ffplay -f rawvideo -pixel_format nv12 -video_size 650x850  nv12.yuv 
```

**libjpeg注意事项**

1. 在使用默认错误处理结构jpeg_error_mgr的情况下，程序在遇到错误后将调用exit直接退出程序，用户如果不希望使用这种直接退出的方式处理错误的话就需要自定义错误处理结构；

2. 操作结束后，如果还希望继续重用JPEG对象做另一个编解码操作，则可使用：

​      jpeg_abort(&cinfo);

**libjpeg-turbo移植到android**

可参考libjpeg-turbo源码的BUILDING.md文档。