#ifndef VIDEOCOLORSPACECHANGE_H
#define VIDEOCOLORSPACECHANGE_H

#include <iostream>
#include <cstring>
extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/log.h>
    #include <libswscale/swscale.h>
    #include <libavutil/parseutils.h>
    #include<libavutil/imgutils.h>
}
using namespace std;
//颜色空间转换并报错rgb图像数据

//#define WORD uint16_t;
//#define DWORD uint32_t
//#define LONG int32_t



//#pragma pack(2)     //预设两个字节对齐
//typedef  struct tabBITMAPFILEHEADER
//{
//    WORD bfType;        //固定0x4d42  BM
//    DWORD bfSize;       //整个bmp文件大小
//    WORD bfreserved1;   //保留字1
//    WORD bfreserved2;
//    DWORD bfOffBits;    //图片位置与bmp文件偏移量 也就是三个部分长度和

//}BITMAPFILEHEADER;

//typedef  struct tagBITMAPINFOHEADER
//{
//    DWORD biSize;            //表示struct tagBITMAPINFOHEADER结构体的长度，设为40
//    LONG biWidth;           //bmp图片宽度
//    LONG biHeight;          //bmp图片高度
//    WORD biPlanes;          //bmp图片平面数，设为1
//    WORD biBitCount;        //bmp图片的颜色位数，即1位图(单色或二值图像)    8位图、16位 24 32
//    DWORD biCompression;        //图片的压缩属性，bmp图片是不压缩的，设不压缩
//    DWORD bisizeInage;          //实际位图数据占用的字节数，着biConpres可省略
//    LONG biXPelsPerMeter;       //X方向分辨率，可省略
//    LONG biYPelsPerMeter;       //Y方向分辨率，可省略
//    DWORD biClrUsed;            ///使用了多少个颜色索引表，如果为6i示默认值(2^b
//    DWORD biClrImportant;       //重要颜色数，如果为0，则表示所有颜负
//}BITMAPINFOHEADER;



class VideoColorSpaceChange
{
public:
    VideoColorSpaceChange(char * inFile,char * outFile, char *resolut);
    ~VideoColorSpaceChange();
    int videoColorSpaceChange();
private:
    int init();
    int decode(AVCodecContext *codeCtx,AVPacket *pkt,AVFrame *dstFrame,struct SwsContext *c);
private:
    char * inFilePath;
    char * outFilePath;
    AVFormatContext * fmtCtx;
    AVStream *video_stream;
    int video_index;
    FILE *fp;
    char *resolution;
    int width,height;
};

#endif // VIDEOCOLORSPACECHANGE_H
