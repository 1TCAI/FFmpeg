#include "av2pcm.h"

Av2Pcm::Av2Pcm(char *inFile,char *outFile): inFilePath(inFile),outFilePath(outFile)
{

}

int Av2Pcm::open_codec()
{
    fmtCtx= avformat_alloc_context();//奇怪为什么这里需要先分配内存？？

    int ret = -1;
    int audio_index = -1;
    AVCodec *codec;

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

    codec = avcodec_find_decoder(audio_stream->codec->codec_id);
    if(!codec)
    {
        cout<<"avcodec_find_decoder error"<<endl;
        return -1;
    }
    cout<<"avcodec_find_decoder success"<<endl;
    codecCtx = avcodec_alloc_context3(codec);
    if(!codecCtx)
    {
        cout<<"avcodec_alloc_context3 error"<<endl;
        return -1;
    }
    cout<<"avcodec_alloc_context3 success"<<endl;
    //!!!!!!!忘记复制参数了
    //! 而且是avcodec_parameters_to_context  不是    avcodec_parameters_copy()
    ret = avcodec_parameters_to_context(codecCtx,audio_stream->codecpar);
    if(ret < 0)
    {
        cout<<"avcodec_parameters_to_context error"<<endl;
        return -1;
    }
    cout<<"avcodec_parameters_to_context success"<<endl;
    //!!!!!!!第一次这边没写出来 pcm文件0kb
    //! ！！！！！！！！！！也没有打开编解码器
    ret = avcodec_open2(codecCtx,codec,nullptr);
    if(ret < 0)
    {
        cout<<"avcodec_open2 error"<<endl;
        return -1;
    }
    cout<<"avcodec_open2 success"<<endl;
    return 0;
}

int Av2Pcm::decode(AVPacket *pkt,AVFrame * frame,FILE *fp)
{
    int ret = -1;
    ret = avcodec_send_packet(codecCtx,pkt);
    if(ret <0) return ret;

//        ret = avcodec_receive_frame(codecCtx,frame);
//        if(ret <0) break;     应该放在while循环里 不一定一次能receive完
    while(ret  >=0)
    {
        ret = avcodec_receive_frame(codecCtx,frame);
        if(ret < 0)
            break;
        size_t linesize = frame->nb_samples * av_get_bytes_per_sample(codecCtx->sample_fmt);

//            cout<<frame->nb_samples<<"   "<<av_get_bits_per_sample(
//                      audio_stream->codec->codec_id) <<endl;

        fwrite(frame->extended_data[0],1,linesize,fp);
        av_frame_unref(frame);
    }

}

int Av2Pcm::av2pcm()
{
    int ret = -1;
    if(open_codec() != 0)
    {
        cout<<"open_codec error"<<endl;
        goto end;
    }
    cout<<"open_codec success"<<endl;

    AVPacket *pkt;
    pkt = av_packet_alloc();
    av_init_packet(pkt);
    pkt->data = NULL;
    pkt->size = 0;
    AVFrame *frame;
    frame = av_frame_alloc();
    FILE *fp;
    fp = fopen(outFilePath,"wb");
    if(!fp)
    {
        cout<<"fopen error"<<endl;
        goto end;
    }
    cout<<"fopen success"<<endl;

    cout<<codecCtx->channels<<endl;
    cout<<codecCtx->sample_rate<<endl;
    cout<<codecCtx->sample_fmt<<endl;
    while ( av_read_frame(fmtCtx,pkt) >=0)
    {
        //！！！！！！！！没有对pkt的索引进行判断
        if(pkt->stream_index != audio_stream->index)
            continue;
        ret = decode(pkt,frame,fp);
        //这里调试ret = -11  receive失败肯定是负数，不能在下面进行break。
        av_packet_unref(pkt);
        //!!!!!每次循环完没有释放pkt
//        if (ret < 0)
//        break;
    }
    decode(NULL,frame,fp);

end:
    fclose(fp);
    avformat_close_input(&fmtCtx);
    avcodec_free_context(&codecCtx);
    av_frame_free(&frame);
}
































































































