#ifndef MEDIACAPTURE_H
#define MEDIACAPTURE_H

#include <iostream>
#include <cstring>
extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/log.h>
    #include <libavdevice/avdevice.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/parseutils.h>
    #include <libswscale/swscale.h>
}
using namespace std;
class MediaCapture
{
public:
    MediaCapture(char * outFile);
    int init();
    int showDevices();
    ~MediaCapture();
    int Capture();

private:
    int decodeAudio(AVCodecContext * decoderCtx,AVPacket *pkt);
private:
    AVFormatContext *fmtCtx;
    int audio_index;
    int video_index;
    AVStream * audio_stream;
    AVStream * video_stream;
    FILE *fp;
    char * outFilePath;
    char * resolution;
    enum AVPixelFormat pixFmt;
    int width,height;

};

#endif // MEDIACAPTURE_H
