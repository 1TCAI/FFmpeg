#ifndef REMUXING_H
#define REMUXING_H
#include <iostream>
#include <cstring>
extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/log.h>
}
using namespace std;
class Remuxing
{
public:
    Remuxing(char * inFile,char * outFile);

    ~Remuxing();
    int convert();

private:
    int init();

private:
    char * inFilePath;
    char * outFilePath;
    AVFormatContext * fmtCtx;
    AVFormatContext * outfmtCtx;

};

#endif // REMUXING_H
