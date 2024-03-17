#include "resamplingaudio.h"

ResamplingAudio:: ResamplingAudio(char * inFile,char * outFile, uint64_t channlayout, int samrate, enum AVSampleFormat sampleFmt)
    :inFilePath(inFile),outFilePath(outFile),channelLayout(channlayout),samRate(samrate),samFmt(sampleFmt)
{
    cout<<"ResamplingAudio begin"<<endl;
}
ResamplingAudio::~ResamplingAudio()
{
    cout<<"ResamplingAudio end"<<endl;
}

int ResamplingAudio::init()
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

int ResamplingAudio::encode(AVCodecContext * decoderCtx,AVPacket*pkt,AVFrame *frame,AVFrame *dstFrame,SwrContext *swrCtx)
{

    int ret = -1,dst_nb_samples,writeSize;
    ret = avcodec_send_packet(decoderCtx,pkt);
    if(ret < 0)
        return -1;
    while (avcodec_receive_frame(decoderCtx,frame) == 0)
    {
        dst_nb_samples = av_rescale_rnd(swr_get_delay(swrCtx,audio_stream->codec->sample_rate)+frame->nb_samples,
                                        samRate,audio_stream->codec->sample_rate,AV_ROUND_UP);
        ret = av_samples_alloc(dstFrame->data,dstFrame->linesize,av_get_channel_layout_nb_channels(channelLayout),dst_nb_samples
                         ,samFmt,0);
        if(ret < 0)
            return ret;
        ret = swr_convert(swrCtx,(uint8_t **)dstFrame->data,dst_nb_samples,(const uint8_t **)frame->data,frame->nb_samples);
        if(ret < 0)
            return ret;
        writeSize = av_samples_get_buffer_size(dstFrame->linesize, av_get_channel_layout_nb_channels(channelLayout),
                                                 ret, samFmt, 1);
        fwrite(dstFrame->data[0],1,writeSize,fp);
    }
    return 0;
}

int ResamplingAudio::resampling()
{
    int ret = -1;
    SwrContext * swrCtx = NULL;
    AVFrame * frame, *dstFrame;
    AVPacket * pkt;
    int writeSize;
    int dst_nb_samples;
    AVCodecContext * decoderCtx;
    AVCodec * decoder;
    ret = init();
    if(ret < 0)
    {
        cout<<"init error"<<endl;
        goto end;
    }
    cout<<"init success"<<endl;

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
    decoder = avcodec_find_decoder(audio_stream->codec->codec_id);
    if(!decoder)
    {
        cout<<"avcodec_find_decoder error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"avcodec_find_decoder success"<<endl;
    decoderCtx = avcodec_alloc_context3(decoder);
    if(!decoderCtx)
    {
        cout<<"avcodec_alloc_context3 error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"avcodec_alloc_context3 success"<<endl;
    ret = avcodec_parameters_to_context(decoderCtx,audio_stream->codecpar);
    if(ret < 0)
    {
        cout<<"avcodec_parameters_to_context error"<<endl;
        goto end;
    }
    cout<<"avcodec_parameters_to_context success"<<endl;
    ret = avcodec_open2(decoderCtx,decoder,NULL);
    if(ret < 0)
    {
        cout<<"avcodec_open2 error"<<endl;
        goto end;
    }
    cout<<"avcodec_open2 success"<<endl;





    swrCtx = swr_alloc();
    if(!swrCtx)
    {
        cout<<"swr_alloc error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"swr_alloc success"<<endl;

    cout<<"swr_alloc success"<<endl;
    cout<<audio_stream->codec->channel_layout<<endl;
    cout<<audio_stream->codec->sample_rate<<endl;
    cout<<audio_stream->codec->sample_fmt<<endl;

    av_opt_set_int(swrCtx,"in_channel_layout",audio_stream->codec->channel_layout,0);
    av_opt_set_int(swrCtx,"in_sample_rate",audio_stream->codec->sample_rate,0);
    av_opt_set_sample_fmt(swrCtx,"in_sample_fmt",audio_stream->codec->sample_fmt,0);

    av_opt_set_int(swrCtx,"out_channel_layout",channelLayout,0);
    av_opt_set_int(swrCtx,"out_sample_rate",samRate,0);
    av_opt_set_sample_fmt(swrCtx,"out_sample_fmt",samFmt,0);

    ret = swr_init(swrCtx);
    if(ret < 0)
    {
        cout<<"swr_alloc error"<<endl;
        goto end;
    }
    frame = av_frame_alloc();
    dstFrame = av_frame_alloc();

    fp = fopen(outFilePath,"wb");
    if(!fp)
    {
        cout<<"fopen error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"fopen success"<<endl;
    frame->nb_samples = 1024;
    ret = av_samples_alloc_array_and_samples((uint8_t***)&frame->data,frame->linesize,audio_stream->codec->channels
                                       ,frame->nb_samples,audio_stream->codec->sample_fmt,0);
    if(ret < 0)
    {
        cout<<"av_samples_alloc_array_and_samples error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"av_samples_alloc_array_and_samples success"<<endl;

    dst_nb_samples = av_rescale_rnd(frame->nb_samples,audio_stream->codec->sample_rate,samRate,AV_ROUND_UP);
    ret = av_samples_alloc_array_and_samples((uint8_t***)&dstFrame->data,dstFrame->linesize,av_get_channel_layout_nb_channels(channelLayout)
                                       ,dst_nb_samples,samFmt,0);
    if(ret < 0)
    {
        cout<<"av_samples_alloc_array_and_samples error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"av_samples_alloc_array_and_samples success"<<endl;

    pkt = av_packet_alloc();
    while (av_read_frame(fmtCtx,pkt) == 0)
    {
        if(pkt->stream_index == audio_index)
        {

            ret = encode(decoderCtx,pkt,frame,dstFrame,swrCtx);
        }
        av_packet_unref(pkt);
        if(ret < 0)
            break;
    }
    encode(decoderCtx,NULL,frame,dstFrame,swrCtx);
end:
    av_frame_free(&frame);
    av_frame_free(&dstFrame);
    av_packet_free(&pkt);
    swr_free(&swrCtx);
    if(fp)
        fclose(fp);
    avformat_close_input(&fmtCtx);
    if(ret < 0)
        return ret;
    return 0;

}
