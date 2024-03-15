#ifndef PCM2AAC_H
#define PCM2AAC_H
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

class Pcm2Aac
{

public:
    Pcm2Aac(char * inFile,char * outFile);
    ~Pcm2Aac();
    int pcm2aac();

private:
    int init();
    int encode(AVCodecContext *encodeCtx,AVPacket *pkt,AVFrame * frame);
private:
    char * inFilePath;
    char * outFilePath;


    FILE *infp;
    FILE *outfp;
};

#endif // PCM2AAC_H
