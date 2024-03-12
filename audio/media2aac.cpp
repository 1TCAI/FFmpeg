#include "media2aac.h"

const int sampleFrequencyTable[] =
{
    96000,
    88200,
    64000,
    48000,
    44100,
    32000,
    24000,
    22050,
    16000,
    12000,
    11025,
    8000,
    7350
};

//单独提取packet的数据是无法播放的，因为没有ADTS头。
int Media2AAC::getADTSHeader(char *adtsHeader, int packetsize,int profile,
                  int sampleRate, int channels)
{
    int sampleFrequencyIndex = 3;       //默认使用48000
    int adtsLength = packetsize + 7;
    for(int i = 0;i<sizeof(sampleFrequencyTable)/sizeof(sampleFrequencyTable[0]);i++)
    {
        if(sampleFrequencyTable[i] == sampleRate)
        {
            sampleFrequencyIndex = i;
            break;
        }
    }
    adtsHeader[0] = 0xff;
    adtsHeader[1]  = 0xf0;
    adtsHeader[1] |= (0<<3);
    adtsHeader[1] |= (0<<1);
    adtsHeader[1] |= 1;

    adtsHeader[2]  = (profile << 6);
    adtsHeader[2] |= (sampleFrequencyIndex & 0x0f) <<2;
    adtsHeader[2] |= (0 << 1);
    adtsHeader[2] |= ( channels & 0x04) >>2;

    adtsHeader[3]  = ( channels & 0x03) << 6;
    adtsHeader[3] |= (0 << 5);
    adtsHeader[3] |= (0 << 4);
    adtsHeader[3] |= (0 << 3);
    adtsHeader[3] |= (0 << 2);
    adtsHeader[3] |= ((adtsLength & 0x1800) >>11);

    adtsHeader[4] = (uint8_t)((adtsLength & 0x7f8) >>3);
    adtsHeader[5] = (uint8_t)((adtsLength & 0x7)<< 5);
    adtsHeader[5] |= 0x1f;
    adtsHeader[6]= 0xfc;
    return 0;
}



Media2AAC::Media2AAC(char * inFile,char * outFile)
        :inFilePath(inFile),outFilePath(outFile)
{


}

int Media2AAC::init()
{
    av_log_set_level(AV_LOG_DEBUG);
    fmtCtx= avformat_alloc_context();
    int ret = -1;
    ret = avformat_open_input(&fmtCtx,inFilePath, nullptr,nullptr);
    if(ret < 0)
    {
        cout<<"avformat_open_input error"<<endl;
        return -1;
    }
    cout<<"avformat_open_input success"<<endl;

    ret = avformat_find_stream_info(fmtCtx,nullptr);
    if(ret < 0)
    {
        cout<<"avformat_find_stream_info error"<<endl;
        return -1;
    }
    cout<<"avformat_find_stream_info success"<<endl;
    ret = av_find_best_stream(fmtCtx,AVMEDIA_TYPE_AUDIO,-1,-1,nullptr,0);
    if(ret < 0)
    {
        cout<<"av_find_best_stream error"<<endl;
        return -1;
    }
    cout<<"av_find_best_stream success"<<endl;
    audio_index = ret;
    audio_stream = fmtCtx->streams[audio_index];
    if(!audio_stream)
    {
        cout<<"audio_stream error"<<endl;
        return -1;
    }
    cout<<"audio_stream success"<<endl;
    return 0;
}
int Media2AAC::media2aac()
{
    int ret = -1;
    AVPacket *pkt;
    pkt = av_packet_alloc();
    if(init() !=0)
        return -1;
    fp = fopen(outFilePath,"wb");
    if(!fp)
    {
        cout<<"fopen error"<<endl;
        return -1;
    }
    cout<<"fopen success"<<endl;
    av_init_packet(pkt);
    while (av_read_frame(fmtCtx,pkt)==0)
    {

        if(pkt->stream_index != audio_index)
            continue;
        char adtsHeader[7] = {0};
        getADTSHeader(adtsHeader,pkt->size,audio_stream->codec->profile,
                      audio_stream->codec->sample_rate,audio_stream->codec->channels);
        ret = fwrite(adtsHeader,1,sizeof(adtsHeader),fp);
        if(ret != sizeof(adtsHeader) )
            return -1;
        ret = fwrite(pkt->data,1,pkt->size,fp);
        if(ret != pkt->size)
            return -1;
        av_packet_unref(pkt);
    }
    return 0;
}

Media2AAC::~Media2AAC()
{
    fclose(fp);
    avformat_close_input(&fmtCtx);
    cout<<"~Media2AAC()"<<endl;
}






































