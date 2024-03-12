#ifndef TEST_H
#define TEST_H
#include <iostream>
using namespace std;
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/codec.h>

}

class Test
{
public:
    Test(char *inFile);
private:
    AVFormatContext *fmtCtx;
    char *inFilePath;
};

#endif // TEST_H
