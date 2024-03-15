#include "mediacapture.h"

MediaCapture::MediaCapture(char * outFile):outFilePath(outFile)
{
    cout<<"MediaCapture()"<<endl;
}

MediaCapture::~MediaCapture()
{
    cout<<"~MediaCapture()"<<endl;
}

int MediaCapture::showDevices()
{
    int ret = -1;
    avdevice_register_all();
    AVInputFormat * inFmat = av_find_input_format("dshow");
    if(!inFmat)
    {
        cout<<"av_find_input_format error"<<endl;
        return -1;
    }
    cout<<"av_find_input_format success"<<endl;
    fmtCtx = avformat_alloc_context();
    if(!fmtCtx)
    {
        cout<<"avformat_alloc_context error"<<endl;
        return -1;
    }
    cout<<"avformat_alloc_context success"<<endl;
    AVDictionary *options = NULL;
    av_dict_set(&options,"list_devices","true",0);
    ret = avformat_open_input(&fmtCtx,"audio=耳机 (WF-1000XM5)",inFmat,&options);
    if(ret < 0)
    {
        cout<<"avformat_open_input error"<<endl;
        cout<<ret<<endl;
        return -1;
    }
    cout<<"avformat_open_input success"<<endl;
    return 0;
}


int MediaCapture::decodeAudio(AVCodecContext * decoderCtx,AVPacket *pkt)
{
    int ret = avcodec_send_packet(decoderCtx,pkt);
    if(ret < 0)
    {
        return ret;
    }
    AVFrame * frame;
    frame = av_frame_alloc();
    while (avcodec_receive_frame(decoderCtx,frame) == 0)
    {

        if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            av_frame_free(&frame);
            return 0;
        }
        if(ret < 0)
        {
            av_frame_free(&frame);
            return ret;
        }

        if(ret < 0)
        {
            av_frame_free(&frame);
            return ret;
        }
        fwrite(frame->data[0],1,frame->linesize[0],fp);

        av_frame_unref(frame);
    }
    av_frame_free(&frame);
    return 0;
}

int MediaCapture::Capture()
{
    AVCodecContext * decodeCtx;
    AVCodec * decodec;
    AVPacket * pkt = NULL;
    int ret = -1;
    AVDictionary *options = NULL;
    avdevice_register_all();

    AVInputFormat * inFmat = av_find_input_format("dshow");
    if(!inFmat)
    {
        cout<<"av_find_input_format error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"av_find_input_format success"<<endl;
    fmtCtx = avformat_alloc_context();
    if(!fmtCtx)
    {
        cout<<"avformat_alloc_context error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"avformat_alloc_context success"<<endl;
    ret = avformat_open_input(&fmtCtx,"audio=耳机 (WF-1000XM5)",inFmat,&options);
    if(ret < 0)
    {
        cout<<"avformat_open_input error"<<endl;
        goto end;
    }
    cout<<"avformat_open_input success"<<endl;
    ret = avformat_find_stream_info(fmtCtx,NULL);
    if(ret < 0)
    {
        cout<<"avformat_find_stream_info error"<<endl;
        goto end;
    }
    cout<<"avformat_find_stream_info success"<<endl;

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
    ret = avcodec_open2(decodeCtx,decodec,NULL);
    if(ret < 0)
    {
        cout<<"avcodec_open2 error"<<endl;
        goto end;
    }
    cout<<"avcodec_open2 success"<<endl;




    pkt = av_packet_alloc();
    fp = fopen(outFilePath,"wb");
    if(!fp)
    {
        cout<<"fopen error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"fopen success"<<endl;
    while (av_read_frame(fmtCtx,pkt) == 0)
    {
        if(pkt->stream_index == audio_stream->index)
        {
            ret = decodeAudio(decodeCtx,pkt);
            if(ret < 0)
                goto end;
        }
        av_packet_unref(pkt);
    }
    decodeAudio(decodeCtx,NULL);
    return 0;

end:

    fclose(fp);

    if(pkt)
        av_packet_free(&pkt);
    avcodec_free_context(&decodeCtx);
    avcodec_close(decodeCtx);
    avformat_close_input(&fmtCtx);
    return ret;
}
