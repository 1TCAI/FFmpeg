#ifndef PCM2WAV_H
#define PCM2WAV_H       //！！！！！！一定不能和类名一样 必须加个_H 否则报错。

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;


typedef struct WAV_HEADER
{
    char chunkid[4];                        //RIFF
    unsigned long chunksize;                //总文件大小 - 8（本身和上面RIFF）
    char format[4];                         //WAVE
}pcmHeader;

typedef struct WAV_FMT
{
    char subformat[4];                      //fmt空格  4个字节。
    unsigned long subsize;                  //24 - 8（本身和上面那4个fmt ）
    unsigned short audioFormat;             //WAVE是1
    unsigned short numchannels;
    unsigned long sampleRate;
    unsigned long byteRate;                 //每秒存储的字节。 采样率*声道数*位深/8；
    unsigned short blockAlign;              //？？？块对齐大小  声道数 * 位深
    unsigned short bitPerSample;            //位深
}pcmFmt;
typedef struct WAV_DATA
{
    char wavdata[4];                        //data
    unsigned long dataSize;                 //pcm裸数据大小。
}pcmData;


class PCM2WAV
{
public:
    PCM2WAV(char * infile,
            char * outfile,
            int ch,
            int sr,
            int fsize,
            int bs);
    int Pcm2Wav();
private:
    int getFileSize();
private:
    char *inFilePath;
    char *outFilePath;
    int channels;
    int sampleRate;
    int fmtSize;                        //？？？？这俩有什么不同
    int bits;                           //？？？？这俩有什么不同
    int pcmDataSize;
};


#endif
