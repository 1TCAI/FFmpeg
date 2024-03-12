#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "./test.h"
#include "./pcm2wav.h"
#include "./wav2pcm.h"
#include "./av2pcm.h"
#include "./media2aac.h"

using namespace std;

int main()
{
//    char * infile = "E:/QT_Workspace/FFmpeg/media/ande_10s.mp4";
//    Test t(infile);




//    pcm数据转wav数据
//    char * infile = ":/../../media/guang10.pcm";        //qt中相对路径必须先加:/   !!!!!
//    char * outfile = ":/../../media/guang10_s16le.wav";       //？？？？为什么qt要使用全路径
//    PCM2WAV p(infile,outfile,2,44100,16,16);
//    p.Pcm2Wav();

//    wav数据转pcm数据
//    char * infile = ":/../../media/guang10_s16le.wav";
//    char * outfile = ":/../../media/guang10_s16le_wave2pcm.pcm";
//    WAVEFORMATEX wf;
//    Wav2Pcm p(infile,outfile,&wf);
//    p.wav2pcm();

//    aac数据转pcm
//    char * inFilePath = "E:/QT_Workspace/FFmpeg/media/guang10s.aac";
//    char * outFilePath = "E:/QT_Workspace/FFmpeg/media/guang10s_aac2pcm.pcm";
//    Av2Pcm p(inFilePath,outFilePath);
//    p.av2pcm();         //只针对打包模式  平面模式只输出0声道

    //？？？？？？？？为什么声音那么杂？？？？？？？？？？
    // ffplay.exe -f s32le -ar 44100 -ac 1 .\guang10s_aac2pcm.pcm


//    媒体文件提取aac数据
    //必须写adts头数据
    char * infile = "E:/QT_Workspace/FFmpeg/media/ande_10s.mp4";
    char * outfile = "E:/QT_Workspace/FFmpeg/media/ande_10s_media2aac.aac";
    Media2AAC p(infile,outfile);
    int ret = p.media2aac();
    cout<<ret<<endl;




    cout<<"end"<<endl;
    return 0;
}
