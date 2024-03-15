#include "pcm2aac.h"

Pcm2Aac::Pcm2Aac(char * inFile,char * outFile)
    :inFilePath(inFile),outFilePath(outFile)
{
    cout<<"Pcm2Aac begin"<<endl;
    init();
}
Pcm2Aac::~Pcm2Aac()
{
    cout<<"~Pcm2Aac()"<<endl;
}
int Pcm2Aac::init()
{

    return 0;
}


int Pcm2Aac::pcm2aac()
{

    int ret = -1;
    AVCodec * encodec = NULL;
    AVCodecContext * encodeCtx = NULL;
    AVFrame * frame = NULL;
    AVPacket * pkt = NULL;

    infp = fopen(inFilePath,"rb");
    if(!infp)
    {
        cout<<"fopen error"<<endl;
        goto end;
    }
    cout<<"fopen success"<<endl;

    outfp = fopen(outFilePath,"wb");
    if(!outfp)
    {
        cout<<"fopen error"<<endl;
        goto end;
    }
    cout<<"fopen success"<<endl;

    encodec = avcodec_find_encoder_by_name("pcm_s16le");
    if(!encodec)
    {
        cout<<"avcodec_find_encoder_by_name error"<<endl;
        goto end;
    }
    cout<<"avcodec_find_encoder_by_name success"<<endl;
    encodeCtx = avcodec_alloc_context3(encodec);
    if(!encodeCtx)
    {
        cout<<"avcodec_alloc_context3 error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"avcodec_alloc_context3 success"<<endl;
    encodeCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    encodeCtx->channels = 2;
    encodeCtx->sample_rate = 48000;
    encodeCtx->channel_layout =AV_CH_LAYOUT_STEREO;
    ret = avcodec_open2(encodeCtx,encodec,NULL);
    if(ret < 0)
    {
        cout<<"avcodec_open2 error"<<endl;
        goto end;
    }
    cout<<"avcodec_open2 success"<<endl;
    frame = av_frame_alloc();
    frame->nb_samples = 1024;
    frame->channels = 2;
    frame->sample_rate = 48000;
    frame->channel_layout =AV_CH_LAYOUT_STEREO;
    frame->format = AV_SAMPLE_FMT_S16;
    ret = av_frame_get_buffer(frame,0);
    pkt = av_packet_alloc();
    if(ret < 0)
    {
        cout<<"av_frame_get_buffer error"<<endl;
        goto end;
    }
    cout<<"av_frame_get_buffer success"<<endl;
    while (true)
    {
        ret = fread(frame->data[0],1,frame->linesize[0],infp);
        if(ret == 0)
            break;
        ret = encode(encodeCtx,pkt,frame);
    }
    encode(encodeCtx,pkt,NULL);

end:
    fclose(infp);
    fclose(outfp);

    avcodec_free_context(&encodeCtx);
    avcodec_close(encodeCtx);
    av_frame_free(&frame);
    if(pkt)
        av_packet_free(&pkt);
    if(frame)
        av_frame_free(&frame);
    if(ret < 0)
        return ret;
    return 0;
}








int Pcm2Aac::encode(AVCodecContext *encodeCtx,AVPacket *pkt,AVFrame * frame)
{
    int ret = avcodec_send_frame(encodeCtx,frame);
    if(ret < 0)
        return -1;
    while (ret >= 0)
    {
        ret = avcodec_receive_packet(encodeCtx,pkt);
        if(ret < 0)
        {
            if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                return 0;
            else {
                return ret;
            }
        }
        fwrite(pkt->data,1,pkt->size,outfp);
        av_packet_unref(pkt);
    }
    return  0;
}









