#ifndef MEDIA2YUV_H
#define MEDIA2YUV_H
//将媒体视频的YUV数据提取。
#include <iostream>
#include <cstring>
extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/log.h>
}
using namespace std;
class Media2YUV
{
public:
    Media2YUV(char * inFile,char * outFile);
    ~Media2YUV();
    int media2yuv();
private:
    int init();
    int decode(AVCodecContext *codeCtx,AVPacket *pkt);
private:
    char * inFilePath;
    char * outFilePath;
    AVFormatContext * fmtCtx;
    AVStream *video_stream;
    int video_index;
    FILE *fp;
};

#endif // MEDIA2YUV_H
