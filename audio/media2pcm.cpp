#include "media2pcm.h"


Media2pcm::Media2pcm(char * inFile,char * outFile)
    :inFilePath(inFile),outFilePath(outFile)
{
    cout<<"Media2pcm begin"<<endl;
    init();
}
int Media2pcm::init()
{
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
    return 0;
}
Media2pcm::~Media2pcm()
{
    avformat_close_input(&fmtCtx);
    cout<<"~Media2pcm()"<<endl;
}

int Media2pcm::media2pcm()
{
    int ret = -1;
    AVCodec * decodec;
    AVCodecContext * decodeCtx = NULL;
    AVFrame * frame = NULL;
    AVPacket * pkt = NULL;
    uint8_t *buffer = NULL;
    int frameSize;
    fp = fopen(outFilePath,"wb");
    if(!fp)
    {
        cout<<"fopen error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"fopen success"<<endl;
    ret = av_find_best_stream(fmtCtx,AVMEDIA_TYPE_AUDIO,-1,-1,NULL,0);
    if(ret < 0)
    {
        cout<<"av_find_best_stream error"<<endl;
        goto end;
    }
    cout<<"av_find_best_stream success"<<endl;
    audio_index = ret;
    audio_stream = fmtCtx->streams[audio_index];
    if(!audio_stream)
    {
        cout<<"audio_stream error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"audio_stream success"<<endl;
    decodec = avcodec_find_decoder(audio_stream->codec->codec_id);
    if(!decodec)
    {
        cout<<"avcodec_find_decoder error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"avcodec_find_decoder success"<<endl;
    decodeCtx = avcodec_alloc_context3(decodec);
    if(!decodeCtx)
    {
        cout<<"avcodec_alloc_context3 error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"avcodec_alloc_context3 success"<<endl;
    ret = avcodec_parameters_to_context(decodeCtx,audio_stream->codecpar);
    if(ret < 0)
    {
        cout<<"avcodec_parameters_to_context error"<<endl;
        goto end;
    }
    cout<<"avcodec_parameters_to_context success"<<endl;
    decodeCtx->pkt_timebase = audio_stream->time_base;
    ret = avcodec_open2(decodeCtx,decodec,NULL);
    if(ret < 0)
    {
        cout<<"avcodec_open2 error"<<endl;
        goto end;
    }
    cout<<"avcodec_open2 success"<<endl;
    frame = av_frame_alloc();
    frame->nb_samples = 1024;
                                    //!!!!!!!!这个地方是null 和 采样点
//    ret = av_samples_get_buffer_size(NULL,channels,sampleRate,sampleFmt,1);


    ret = av_samples_get_buffer_size(NULL,decodeCtx->channels,frame->nb_samples,decodeCtx->sample_fmt,1);
    if(ret < 0)
    {
        cout<<"av_samples_get_buffer_size error"<<endl;
        goto end;
    }
    cout<<"av_samples_get_buffer_size success"<<endl;
    frameSize = ret;
    buffer = (uint8_t *)av_malloc(frameSize);

    ret = avcodec_fill_audio_frame(frame,decodeCtx->channels,decodeCtx->sample_fmt,buffer,frameSize,1);
//    (AVFrame *frame, int nb_channels,
//                                 enum AVSampleFormat sample_fmt, const uint8_t *buf,
//                                 int buf_size, int align);
//    ret = av_samples_fill_arrays(frame->data,frame->linesize,buffer,channels,sampleRate,sampleFmt,1);
    if(ret < 0)
    {
        cout<<"avcodec_fill_audio_frame error"<<endl;
        goto end;
    }
    cout<<"avcodec_fill_audio_frame success"<<endl;
    pkt = av_packet_alloc();

    while (av_read_frame(fmtCtx,pkt) == 0)
    {
        if(pkt->stream_index != audio_index)
        {
            av_packet_unref(pkt);
            continue;
        }
        ret = decode(decodeCtx,pkt,frame);
        if(ret < 0)
            goto end;

        av_packet_unref(pkt);
    }
    decode(decodeCtx,NULL,frame);
end:
    fclose(fp);
    avcodec_free_context(&decodeCtx);
    avcodec_close(decodeCtx);
    av_frame_free(&frame);
    if(pkt)
        av_packet_free(&pkt);
    if(buffer)
        av_free(buffer);
    if(ret < 0)
        return ret;
    return 0;
}

int Media2pcm::decode(AVCodecContext *decodeCtx,AVPacket *pkt,AVFrame * frame)
{
    int ret = avcodec_send_packet(decodeCtx,pkt);
    if(ret < 0)
    {

            return ret;
    }
    while (avcodec_receive_frame(decodeCtx,frame) == 0)
    {

        if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return 0;

        if(ret < 0)
            return ret;
        int size = av_get_bytes_per_sample(decodeCtx->sample_fmt);

        for (int i = 0;i<frame->nb_samples;i++)
        {
            for (int channe = 0;channe < decodeCtx->channels; channe++)
            {
                ret = fwrite(frame->data[channe] + size * i,1,size,fp);
            }
        }


    }
    return 0;
}











