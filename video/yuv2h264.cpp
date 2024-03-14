#include "yuv2h264.h"


YUV2h264::YUV2h264(char * inFile,char * outFile, char *resolut,char * encoderName,AVPixelFormat pixFmt,int fps)
    :inFilePath(inFile),outFilePath(outFile),resolution(resolut),encoder(encoderName),pix_fmt(pixFmt),fps(fps)
{
    cout<<"YUV2h264 begin"<<endl;
    av_parse_video_size(&width,&height,resolution);
    cout<<width<<"*"<<height<<endl;
}

YUV2h264::~YUV2h264()
{
    cout<<"~YUV2h264()"<<endl;

}
int YUV2h264::encode(AVFrame * frame, AVPacket * pkt, FILE * fp, AVCodecContext * encodeCtx)
{
    int ret = avcodec_send_frame(encodeCtx,frame);
    if(ret < 0)
    {
        if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return 0;
        else
            return ret;
    }
    while (avcodec_receive_packet(encodeCtx,pkt) == 0)
    {
        ret = fwrite(pkt->data,1,pkt->size,fp);
        if(ret < 0)
            return ret;
        av_packet_unref(pkt);
    }
    return 0;
}

int YUV2h264::yuv2h264()
{
    int ret = -1;
    AVCodecContext *encodeCtx;
    AVFrame * frame;
    AVPacket *pkt;
    uint8_t *buffer;
    int frameSize;
    int ptsCount = 0;
    AVCodec *encodec = avcodec_find_encoder_by_name(encoder);
    if(!encodec)
    {
        ret = -1;
        cout<<"avcodec_find_encoder_by_name error"<<endl;
        goto end;
    }
    cout<<"avcodec_find_encoder_by_name success"<<endl;
    encodeCtx = avcodec_alloc_context3(encodec);
    if(!encodeCtx)
    {
        ret = -1;
        cout<<"avcodec_alloc_context3 error"<<endl;
        goto end;
    }
    cout<<"avcodec_alloc_context3 success"<<endl;
    encodeCtx->pix_fmt = pix_fmt;
    encodeCtx->height = height;
    encodeCtx->width = width;
    encodeCtx->time_base = AVRational{1,fps};
    encodeCtx->bit_rate = 4096000;
    encodeCtx->max_b_frames = 0;
    encodeCtx->gop_size = 10;
    ret = avcodec_open2(encodeCtx,encodec,NULL);
    if(ret < 0)
    {
        cout<<"avcodec_open2 error"<<endl;
        goto end;
    }
    cout<<"avcodec_open2 success"<<endl;
    infp = fopen(inFilePath,"rb");
    if(!infp)
    {
        cout<<"infopen error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"in fopen success"<<endl;
    outfp = fopen(outFilePath,"wb");
    if(!outfp)
    {
        cout<<"out fopen error"<<endl;
        ret = -1;
        goto end;
    }
    cout<<"out fopen success"<<endl;
    frame = av_frame_alloc();
    frameSize = av_image_get_buffer_size(pix_fmt,width,height,1);
    buffer = (uint8_t *)av_malloc(frameSize);
    ret = av_image_fill_arrays(frame->data,frame->linesize,buffer,pix_fmt,width,height,1);
    if(ret < 0)
    {
        cout<<"av_image_fill_arrays error"<<endl;
        goto end;
    }
    cout<<"av_image_fill_arrays success"<<endl;
    frame->height = height;
    frame->width = width;
    frame->format = pix_fmt;    //frame里没有pixfmt属性。
    pkt = av_packet_alloc();

    while (fread(buffer,1,width*height*1.5,infp) == width*height*1.5)
    {
        frame->data[0] = buffer;
        frame->data[1] = buffer + width*height;
        frame->data[2] = buffer + width*height + width*height / 4;
        frame->pts = ptsCount++;
        ret = encode(frame,pkt,outfp,encodeCtx);
        if(ret < 0)
            goto end;
    }
    encode(NULL,pkt,outfp,encodeCtx);

end:
    avcodec_free_context(&encodeCtx);
    avcodec_close(encodeCtx);
    fclose(infp);
    fclose(outfp);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    av_free(buffer);
    if(ret < 0)
        return ret;
    return 0;
}
