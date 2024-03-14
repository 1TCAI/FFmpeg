#ifndef YUV2H264_H
#define YUV2H264_H

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
class YUV2h264
{

public:
    YUV2h264(char * inFile,char * outFile, char *resolut,char * encoderName,AVPixelFormat pixFmt,int fps);
    ~YUV2h264();
    int yuv2h264();
private:
    int encode(AVFrame * frame, AVPacket * pkt, FILE * fp, AVCodecContext * encodeCtx);

private:
    char * inFilePath;
    char * outFilePath;
    FILE *infp;
    FILE *outfp;
    char *resolution;
    int width,height;
    char * encoder;
    enum AVPixelFormat pix_fmt;
    int fps;
};

#endif // YUV2H264_H
