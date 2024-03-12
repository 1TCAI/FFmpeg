#include "videocolorspacechange.h"

VideoColorSpaceChange::VideoColorSpaceChange(char * inFile,char * outFile, char *resolut)
    :inFilePath(inFile),outFilePath(outFile),resolution(resolut)
{
    cout<<"VideoColorSpaceChange begin"<<endl;
    init();
}
int VideoColorSpaceChange::init()
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
VideoColorSpaceChange::~VideoColorSpaceChange()         //感觉一些释放不能放析构函数内，除非构造函数调用init。
                                //否则如果只生成对象 但不调用init 析构肯定会执行。
{
    avformat_close_input(&fmtCtx);
    cout<<"~VideoColorSpaceChange()"<<endl;
}


int VideoColorSpaceChange::videoColorSpaceChange()
{
    int ret = -1;
    AVCodec *codec ;
    AVCodecContext * CodeCtx;
    struct SwsContext * swsCtx;
    AVFrame *dstFrame;
    uint8_t *buffer;
    AVPacket *pkt;
    fp = fopen(outFilePath,"wb+");
    if(!fp)
    {
        cout<<"fopen error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"fopen success"<<endl;
    ret = av_parse_video_size(&width,&height,resolution);
    if(ret < 0)
    {
        cout<<"av_parse_video_size error"<<endl;
        goto end;
    }
    cout<<"av_parse_video_size success"<<endl;
    cout<<width<<"*"<<height<<endl;
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
    codec = avcodec_find_decoder(video_stream->codec->codec_id);
    if(!codec)
    {
        cout<<"avcodec_find_decoder error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"avcodec_find_decoder success"<<endl;
    CodeCtx = avcodec_alloc_context3(codec);
    if(!CodeCtx)
    {
        cout<<"avcodec_alloc_context3 error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"avcodec_alloc_context3 success"<<endl;
    ret = avcodec_parameters_to_context(CodeCtx,video_stream->codecpar);
    if(ret < 0)
    {
        cout<<"avcodec_parameters_to_context error"<<endl;
        goto end;
    }
    cout<<"avcodec_parameters_to_context success"<<endl;
    ret = avcodec_open2(CodeCtx,codec,NULL);
    if(ret < 0)
    {
        cout<<"avcodec_open2 error"<<endl;
        goto end;
    }
    cout<<"avcodec_open2 success"<<endl;
//    swsCtx = sws_getContext(video_stream->codec->width,video_stream->codec->height,video_stream->codec->pix_fmt
//                   ,width,height,CodeCtx->pix_fmt,SWS_FAST_BILINEAR,NULL,NULL,NULL);
    swsCtx = sws_getContext(video_stream->codec->width,video_stream->codec->height,video_stream->codec->pix_fmt
                   ,width,height,AV_PIX_FMT_RGB24,SWS_FAST_BILINEAR,NULL,NULL,NULL);
                                //!!!!!!!!原先是直接输入颜色空间就是输出的。现在将yuv420p改为rgb24

    if(!swsCtx)
    {
        cout<<"sws_getContext error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"sws_getContext success"<<endl;

    dstFrame = av_frame_alloc();
//    buffer = (uint8_t *)av_malloc(avpicture_get_size(CodeCtx->pix_fmt,width,height));
    buffer = (uint8_t *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB24,width,height,1));

    ret = av_image_fill_arrays(dstFrame->data,dstFrame->linesize,buffer,AV_PIX_FMT_RGB24,width,height,1);
    if(ret < 0)
    {
        cout<<"av_image_fill_arrays error"<<endl;
        goto end;
    }
    cout<<"av_image_fill_arrays success"<<endl;
    pkt = av_packet_alloc();

    while (av_read_frame(fmtCtx,pkt) == 0)
    {
        if(pkt->stream_index != video_index)
        {
            av_packet_unref(pkt);
            continue;
        }
        ret = decode(CodeCtx,pkt,dstFrame,swsCtx);
        av_packet_unref(pkt);
        if(ret < 0)
            break;
    }
    decode(CodeCtx,NULL,dstFrame,swsCtx);

end:
    av_packet_free(&pkt);
    av_frame_free(&dstFrame);
    avcodec_free_context(&CodeCtx);
    avcodec_close(CodeCtx);
    av_free(buffer);
    fclose(fp);
    if(ret < 0)
        return -1;
    return 0;
}
int VideoColorSpaceChange::decode(AVCodecContext *codeCtx,AVPacket *pkt,AVFrame *dstFrame,struct SwsContext *c)
{
    int ret = -1;
    AVFrame *frame;
    frame = av_frame_alloc();
    ret = avcodec_send_packet(codeCtx,pkt);
    if(ret < 0)
        return -1;
    while (avcodec_receive_frame(codeCtx,frame) == 0)
    {
        ret = sws_scale(c,frame->data,frame->linesize,0,codeCtx->height,dstFrame->data,dstFrame->linesize);
        if(ret < 0)
            return -1;
//        fwrite(dstFrame->data[0],1,dstFrame->linesize[0]*dstFrame->height,fp);
//        fwrite(dstFrame->data[1],1,dstFrame->linesize[0]*dstFrame->height/4,fp);
//        fwrite(dstFrame->data[2],1,dstFrame->linesize[0]*dstFrame->height/4,fp);      !!!!!!!!
//        fwrite(dstFrame->data[0],1,width*height,fp);
//        fwrite(dstFrame->data[1],1,width*height/4,fp);
//        fwrite(dstFrame->data[2],1,width*height/4,fp);
          fwrite(dstFrame->data[0],1,width*height*3,fp);
          //rgb24 都是打包格式，写入数量注意是三个字节
    }
    return 0;
}

