#ifndef MEDIA2H264_H
#define MEDIA2H264_H

#include <iostream>
#include <cstring>
extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/log.h>
}
using namespace std;
class Media2H264
{
public:
    Media2H264(char * inFile,char * outFile);

    int media2h264();
    ~Media2H264();

private:

    int init();

private:
    char * inFilePath;
    char * outFilePath;
    AVFormatContext * fmtCtx;
    AVStream *video_stream;
    int video_index;
    FILE *fp;
};

#endif // MEDIA2H264_H
