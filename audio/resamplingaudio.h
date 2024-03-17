#ifndef RESAMPLINGAUDIO_H
#define RESAMPLINGAUDIO_H
#include <iostream>
extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/log.h>
    #include <libswresample/swresample.h>
    #include <libavutil/opt.h>
    #include <libavutil/samplefmt.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/error.h>
}
using namespace std;
class ResamplingAudio
{
public:
    ResamplingAudio(char * inFile,char * outFile, uint64_t channlayout, int samrate, enum AVSampleFormat sampleFmt);
    ~ResamplingAudio();
    int resampling();

private:
    int init();
    int encode(AVCodecContext * decoderCtx,AVPacket*pkt,AVFrame *frame,AVFrame *dstFrame,SwrContext *swrCtx);

private:
    AVFormatContext *fmtCtx;
    int audio_index;
    AVStream * audio_stream;
    FILE *fp;
    char * outFilePath;
    char * inFilePath;
    uint64_t channelLayout;
    int samRate;
    enum AVSampleFormat samFmt;
};

#endif // RESAMPLINGAUDIO_H
