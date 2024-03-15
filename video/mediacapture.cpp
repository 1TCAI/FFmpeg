#include "mediacapture.h"

MediaCapture::MediaCapture(char * outFile,char * resolut,AVPixelFormat pixfmt):outFilePath(outFile)
  ,resolution(resolut),pixFmt(pixfmt)
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
    av_dict_set(&options,"framerate","30",0);
    ret = avformat_open_input(&fmtCtx,"video=Integrated Camera",inFmat,&options);
    if(ret < 0)
    {
        cout<<"avformat_open_input error"<<endl;
        cout<<ret<<endl;
        return -1;
    }
    cout<<"avformat_open_input success"<<endl;
    return 0;
}


int MediaCapture::decodeVideo(AVCodecContext * decoderCtx,AVPacket *pkt,AVFrame *dstframe,SwsContext * swsCtx)
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
        //yuv422
        ret = sws_scale(swsCtx,frame->data,frame->linesize,0,decoderCtx->height,dstframe->data,dstframe->linesize);
        if(ret < 0)
        {
            av_frame_free(&frame);
            return ret;
        }
        //yuv420p
        fwrite(dstframe->data[0],1,width*height,fp);
        fwrite(dstframe->data[1],1,width*height/4,fp);
        fwrite(dstframe->data[2],1,width*height/4,fp);

        av_frame_unref(frame);
    }
    av_frame_free(&frame);
    return 0;
}

int MediaCapture::Capture()
{
    AVCodecContext * decodeCtx;
    AVCodec * decodec;
    AVFrame * dstFrame = NULL;
    AVPacket * pkt = NULL;
    int ret = -1;
    AVDictionary *options = NULL;
    avdevice_register_all();
    av_parse_video_size(&width,&height,resolution);
    uint8_t * buffer;
    int buffSize;
    struct SwsContext *swsCtx;
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
    av_dict_set(&options,"framerate","30",0);
    ret = avformat_open_input(&fmtCtx,"video=Integrated Camera",inFmat,&options);
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

    ret = av_find_best_stream(fmtCtx,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0);
    if(ret < 0)
    {
        cout<<"av_find_best_stream error"<<endl;
        goto end;
    }
    cout<<"av_find_best_stream success"<<endl;
    video_index = ret;
    video_stream = fmtCtx->streams[video_index];
    if(!video_stream)
    {
        cout<<"video_stream error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"video_stream success"<<endl;
    decodec = avcodec_find_decoder(video_stream->codec->codec_id);
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
    ret = avcodec_parameters_to_context(decodeCtx,video_stream->codecpar);
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
    swsCtx = sws_getContext(decodeCtx->width,decodeCtx->height,decodeCtx->pix_fmt,width,height,pixFmt
                            ,SWS_FAST_BILINEAR,NULL,NULL,NULL);
    if(!swsCtx)
    {
        cout<<"sws_getContext error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"sws_getContext success"<<endl;


    dstFrame = av_frame_alloc();
    ret = av_image_get_buffer_size(pixFmt,width,height,1);
    if(ret < 0)
    {
        cout<<"av_image_get_buffer_size error"<<endl;
        goto end;
    }
    cout<<"av_image_get_buffer_size success"<<endl;
    buffSize = ret;
    buffer =(uint8_t *)av_malloc(buffSize);

    ret = av_image_fill_arrays(dstFrame->data,dstFrame->linesize,buffer,pixFmt,width,height,1);
    if(ret < 0)
    {
        cout<<"av_image_fill_arrays error"<<endl;
        goto end;
    }
    cout<<"av_image_fill_arrays success"<<endl;
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
        if(pkt->stream_index == video_stream->index)
        {
            ret = decodeVideo(decodeCtx,pkt,dstFrame,swsCtx);
            if(ret < 0)
                goto end;
        }
        av_packet_unref(pkt);
    }
    decodeVideo(decodeCtx,NULL,dstFrame,swsCtx);
    return 0;

end:
    if(buffer)
        av_free(buffer);
    fclose(fp);
    av_frame_free(&dstFrame);

    if(pkt)
        av_packet_free(&pkt);
    avcodec_free_context(&decodeCtx);
    avcodec_close(decodeCtx);
    avformat_close_input(&fmtCtx);
    return ret;
}
