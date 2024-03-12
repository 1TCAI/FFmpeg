#include "media2yuv.h"

Media2YUV::Media2YUV(char * inFile,char * outFile)
    :inFilePath(inFile),outFilePath(outFile)
{
    init();
    av_log_set_level(AV_LOG_INFO);
}
Media2YUV::~Media2YUV()         //感觉一些释放不能放析构函数内，除非构造函数调用init。
                                //否则如果只生成对象 但不调用init 析构肯定会执行。
{
    avformat_close_input(&fmtCtx);
    cout<<"~Media2YUV()"<<endl;
}
int Media2YUV::init()
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

int Media2YUV::decode(AVCodecContext *codeCtx,AVPacket *pkt)
{
    int ret = -1;
    AVFrame *frame;
    frame = av_frame_alloc();
    ret = avcodec_send_packet(codeCtx,pkt);
    if(ret < 0)
        return ret;

    while (avcodec_receive_frame(codeCtx,frame) == 0)
    {
        fwrite(frame->data[0],1,frame->width*frame->height,fp);
        fwrite(frame->data[1],1,frame->width*frame->height/4,fp);
        fwrite(frame->data[2],1,frame->width*frame->height/4,fp);
    }
    return 0;
}

int Media2YUV::media2yuv()
{
    int ret = -1;
    AVCodec *decodec;
    AVPacket *pkt;
    int count = 0;

    fp = fopen(outFilePath,"wb");
    if(!fp)
    {
        cout<<"fopen error"<<endl;
        return -1;
    }
    cout<<"fopen success"<<endl;
    video_index = av_find_best_stream(fmtCtx,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0);
    video_stream = fmtCtx->streams[video_index];
    if(!video_stream)
        return -1;

    AVCodecContext *decodeCtx = avcodec_alloc_context3(NULL);
    if(!decodeCtx)
    {
        cout<<"avcodec_alloc_context3 error"<<endl;
        return -1;
    }
    cout<<"avcodec_alloc_context3 success"<<endl;
    ret = avcodec_parameters_to_context(decodeCtx,video_stream->codecpar);
    if(ret < 0)
    {
        cout<<"avcodec_parameters_to_context error"<<endl;
        return -1;
    }
    cout<<"avcodec_parameters_to_context success"<<endl;
//    decodec = avcodec_find_decoder(video_stream->codec->codec_id);
    decodec = avcodec_find_decoder(decodeCtx->codec_id);
    if(!decodec)
    {
        cout<<"avcodec_find_decoder error"<<endl;
        return -1;
    }
    cout<<"avcodec_find_decoder success"<<endl;
    ret = avcodec_open2(decodeCtx,decodec,NULL);
    if(ret < 0)
    {
        cout<<"avcodec_open2 error"<<endl;
        goto end;
    }
    cout<<"avcodec_open2 success"<<endl;
    pkt = av_packet_alloc();

    if(!pkt )
        goto end;
    while (av_read_frame(fmtCtx,pkt) == 0)
    {
        if(pkt->stream_index != video_index)
        {
            av_packet_unref(pkt);
            continue;
        }
        cout<<++count<<endl;
        ret = decode(decodeCtx,pkt);
        av_packet_unref(pkt);
        if(ret < 0)
            break;
    }
    decode(decodeCtx,NULL);
end:
//    avio_closep(&outfmtCtx->pb);
//    avio_close(outfmtCtx->pb);          //这俩有什么区别
    avcodec_close(decodeCtx);
    fclose(fp);
    if(ret != 0)
        return ret;
    return 0;
}
