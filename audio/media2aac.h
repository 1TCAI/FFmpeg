#ifndef MEDIA2AAC_H
#define MEDIA2AAC_H

#include <iostream>
#include <cstring>
extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/log.h>
}
using namespace std;

class Media2AAC
{
public:
    Media2AAC(char * inFile,char * outFile);
    int media2aac();
    ~Media2AAC();

private:
    int getADTSHeader(char *adtsHeader, int packetsize,int profile,
                      int sampleRate, int channels);
    int init();
    char * inFilePath;
    char * outFilePath;
    AVFormatContext * fmtCtx;
    AVStream *audio_stream;
    int audio_index;
    FILE *fp;

};

#endif // MEDIA2AAC_H
