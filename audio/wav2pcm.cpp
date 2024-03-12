#include "wav2pcm.h"

Wav2Pcm::Wav2Pcm(char *inFile,
                char *outFile,
                tWAVEFORMATEX *wf): inFilePath(inFile), outFilePath(outFile),waveFmt(wf)
{
}
int Wav2Pcm::wav2pcm()
{
    FILE* fp = nullptr;
    FILE* fpOutPCM = nullptr;
    char str[32] = {0};
    char *speech = nullptr;
    unsigned int subchunk1size  = 0; // head size
    unsigned int subchunk2size  = 0; // speech data size
    // check format type
    fp = fopen(inFilePath,"r");
    if(!fp)
    {
        fprintf(stderr,"Can not open the wave file: %s.\n",inFilePath);
        return -1;
    }
    fpOutPCM = fopen(outFilePath, "wb");
    if(!fpOutPCM)
    {
        fprintf(stderr,"Can not open the out.pcm file: %s.\n",outFilePath);
        return -1;
    }

    /// 跳过前8个字节
    fseek(fp, 8, SEEK_SET); //WAVE 和 大小
    /// 读接下来的连续的7个字节
    fread(str, sizeof(char), 7, fp);    //对于音频就说 WAVEfmt
    str[7] = '\0';
    if(strcmp(str,"WAVEfmt")){
        fprintf(stderr,"The file is not in WAVE format!\n");
        return -1;
    }
    // read format header
    fseek(fp, 16, SEEK_SET);///定位函数

    /// subChunk1Size: 一般wav文件，这里对应的是16字节。 有的时候不是16字节
    fread((unsigned int*)(&subchunk1size),4,1,fp); ///小端字节序，读4个字节

    printf("subchunk1size=%d\n", subchunk1size);

    fseek(fp, 20, SEEK_SET);
    fread(waveFmt, subchunk1size, 1, fp);       //waveFmt 就出现了这一次
    // read wave data
    fseek(fp, 20+subchunk1size, SEEK_SET);
    fread(str, 1, 4, fp);
    str[4] = '\0';

    if(strcmp(str,"data")){
        fprintf(stderr,"Locating data start point failed!\n");
        return NULL;
    }
                //可以直接44
    fseek(fp, 20+subchunk1size+4, SEEK_SET);
    fread((unsigned int*)(&subchunk2size), 4, 1, fp); //真实的PCM裸数据的大小

    printf("subchunk2size=%d\n", subchunk2size);

    speech = (char*)malloc(sizeof(char)*subchunk2size);

    if(!speech){
        fprintf(stderr, "Memory alloc failed!\n");
        return -1;
    }
                //本来不就是44 怎么还重新定位
    fseek(fp, 20+subchunk1size+8, SEEK_SET);
    fread(speech, 1, subchunk2size, fp);
    fclose(fp);
    fp = nullptr;

    fwrite(speech, 1, subchunk2size, fpOutPCM);
    fclose(fpOutPCM);
    fpOutPCM = nullptr;

    free(speech);
    speech = nullptr;
    cout<<"success"<<endl;
    return 0;
}
