#ifndef AV2PCM_H
#define AV2PCM_H
#include <iostream>
using namespace std;
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/codec.h>

}

//音频提取pcm
class Av2Pcm
{
public:
    Av2Pcm(char *inFile,char *outFile);
    int av2pcm();

private:
    int open_codec();
    int decode(AVPacket *pkt,AVFrame * frame,FILE *fp);
    char * inFilePath;
    char * outFilePath;
    AVFormatContext *fmtCtx;
    AVCodecContext *codecCtx;
    AVStream * audio_stream;
};

#endif // AV2PCM_H
