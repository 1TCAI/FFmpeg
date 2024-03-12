#include "pcm2wav.h"

//一般数值是小端存储
//字符串是大端存储。
int PCM2WAV::Pcm2Wav()
{
    WAV_HEADER pcmHeader;
    WAV_FMT pcmFmt;
    WAV_DATA pcmData;

    memcpy(pcmHeader.chunkid,"RIFF",4);
    pcmHeader.chunksize = 44 + pcmDataSize - 8;
    memcpy(pcmHeader.format,"WAVE",4);

    memcpy(pcmFmt.subformat,"fmt ",4);
    pcmFmt.subsize = 16;
    pcmFmt.audioFormat = 1;
    pcmFmt.numchannels = channels;
    pcmFmt.sampleRate = sampleRate;
    pcmFmt.byteRate = sampleRate * channels * bits/ 8;
    pcmFmt.blockAlign = channels * bits / 8;
    pcmFmt.bitPerSample = bits;

    memcpy(pcmData.wavdata,"data",4);
    pcmData.dataSize = pcmDataSize;
    FILE *ofp = fopen(outFilePath, "wb");      //???为什么要以二进制
    if(!ofp)
    {
        cout<<"ofp fopen error pcm2wav"<<endl;
        return -1;
    }
    fwrite(&pcmHeader, sizeof(pcmHeader), 1, ofp);
    fwrite(&pcmFmt, sizeof(pcmFmt), 1, ofp);
    fwrite(&pcmData,sizeof(pcmData), 1, ofp);


    char * buffer = (char *)malloc(512);
    FILE *ifp = fopen(inFilePath, "rb");            //???为什么要以二进制
    if(!ifp)
    {
        cout<<"ifp fopen error pcm2wav"<<endl;
        return -1;
    }
    int len = 0;
    while ((len =fread(buffer,1,512,ifp)) != 0) {
       fwrite(buffer,len,  1, ofp);
    }
    free(buffer);
    fclose(ofp);
    fclose(ifp);
    cout<<"success"<<endl;
    return 0;
}

PCM2WAV::PCM2WAV(char * infile,
        char * outfile,
        int ch,
        int sr,
        int fsize,
        int bs):
        inFilePath(infile),
        outFilePath(outfile),
        channels(ch),
        sampleRate(sr),
        fmtSize(fsize),         //这俩有什么不同
        bits(bs),               //位深
        pcmDataSize(0)
{

    pcmDataSize = getFileSize();
}
int PCM2WAV::getFileSize()
{
    FILE * fp = fopen(inFilePath, "rb");         //为什么这里又不用二进制？
    if(!fp)                                      //用不用二进制这俩都一样。
    {
        cout<<"fopen error getFileSize"<<endl;
    }
    fseek(fp,0,SEEK_END);
    int size = ftell(fp);
    fclose(fp);
    cout<<size<<endl;       //1769472  1769472
    return size;
}
