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
