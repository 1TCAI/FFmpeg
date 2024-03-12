#ifndef WAV2PCM_H
#define WAV2PCM_H
#include <iostream>
#include <cstring>
using namespace std;
// define Wave format structure
/// wav格式的subChunk1的“fmt”部分，一般是16字节
typedef struct tWAVEFORMATEX
{
    short wFormatTag;
    short nChannels;
    unsigned int nSamplesPerSec;
    unsigned int nAvgBytesPerSec;
    short nBlockAlign;
    short wBitsPerSample;
    short cbSize;  ///这两个字节，无用
} WAVEFORMATEX, *PWAVEFORMATEX;
class Wav2Pcm
{
public:
    Wav2Pcm(char *inFile,
            char *outFile,
            tWAVEFORMATEX *wf);

    int wav2pcm();
private:
    char * inFilePath;
    char * outFilePath;
    tWAVEFORMATEX *waveFmt;
};

#endif // WAV2PCM_H
