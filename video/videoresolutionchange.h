#ifndef VIDEORESOLUTIONCHANGE_H
#define VIDEORESOLUTIONCHANGE_H

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
//修改视频分辨率
class VideoResolutionChange
{
public:
    VideoResolutionChange(char * inFile,char * outFile, char *resolut);
    ~VideoResolutionChange();
    int videoResolutionChange();
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
};

#endif // VIDEORESOLUTIONCHANGE_H
