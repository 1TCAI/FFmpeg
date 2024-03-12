
#ifndef CUTVIDEO_H
#define CUTVIDEO_H
//对一个视频进行截取。指定开始 和 结束。
//只提取 音频 视频 字幕 三个流。
#include <iostream>
#include <cstring>
extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/log.h>
}
using namespace std;


class CutVideo
{
public:
    CutVideo(char * inFile,char * outFile,int tstartTime,int tendTime);
    ~CutVideo();
    int cutVideo();
private:
    int init();

private:
    char * inFilePath;
    char * outFilePath;
    AVFormatContext * fmtCtx;
    AVFormatContext * outfmtCtx;
    int64_t startTime;
    int64_t endTime;
};

#endif // CUTVIDEO_H
