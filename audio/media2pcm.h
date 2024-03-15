#ifndef MEDIA2PCM_H
#define MEDIA2PCM_H
#include <iostream>
#include <cstring>
extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/log.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/audio_fifo.h>
    #include <libavutil/samplefmt.h>
}
using namespace std;

class Media2pcm
{
public:
    Media2pcm(char * inFile,char * outFile);
    ~Media2pcm();
    int media2pcm();

private:
    int init();
    int decode(AVCodecContext *decodeCtx,AVPacket *pkt,AVFrame * frame);
private:
    char * inFilePath;
    char * outFilePath;
    AVFormatContext * fmtCtx;
    AVStream *audio_stream;
    int audio_index;
    FILE *fp;

};

#endif // MEDIA2PCM_H
