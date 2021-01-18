**解码**

解压输出参数设为`JCS_YCbCr`时，输出的格式为YUV444，每个像素三个字节，存储顺序为YUV，例如4x3存储如下：

```
YUVYUVYUVYUV
YUVYUVYUVYUV
YUVYUVYUVYUV
```
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