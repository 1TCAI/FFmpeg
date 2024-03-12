#include "remuxing.h"

Remuxing::Remuxing(char * inFile,char * outFile)
    :inFilePath(inFile),outFilePath(outFile)
{


}

Remuxing::~Remuxing()
{
    avformat_close_input(&fmtCtx);
    avio_closep(&outfmtCtx->pb);
    avio_close(outfmtCtx->pb);
    avformat_free_context(outfmtCtx);

    cout<<"~Remuxing()"<<endl;
}

int Remuxing::init()
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
    ret = avformat_alloc_output_context2(&outfmtCtx,NULL,NULL,outFilePath);
    if(ret < 0)
    {
        cout<<"avformat_alloc_output_context2 error"<<endl;
        return -1;
    }
    cout<<"avformat_alloc_output_context2 success"<<endl;

    return 0;
}



int Remuxing::convert()
{
    int ret = -1;
    ret = init();
    int *streamIndex = (int *)av_mallocz_array(fmtCtx->nb_streams,sizeof(int));
    int streamCount = 0;
    for(int i = 0;i < fmtCtx->nb_streams;i++)
    {
        if(fmtCtx->streams[i]->codec->codec_type != AVMEDIA_TYPE_AUDIO &&
            fmtCtx->streams[i]->codec->codec_type != AVMEDIA_TYPE_VIDEO &&
            fmtCtx->streams[i]->codec->codec_type != AVMEDIA_TYPE_SUBTITLE )
        {
            streamIndex[i] = -1;
            continue;
        }
        streamIndex[i] = streamCount++;     //这里是后++.
        AVStream *stream = avformat_new_stream(outfmtCtx,NULL);
        if(!stream)
        {
            cout<<"avformat_new_stream error"<<endl;
            av_freep(streamIndex);
            return -1;
        }
        cout<<"avformat_new_stream success"<<endl;
        ret = avcodec_parameters_copy(stream->codecpar,fmtCtx->streams[i]->codecpar);
        if(ret < 0)
        {
            cout<<"avcodec_parameters_copy error"<<endl;
            av_freep(streamIndex);
            return -1;
        }
        cout<<"avcodec_parameters_copy success"<<endl;
        //!!!!!!!!！！！！
        stream->codecpar->codec_tag = 0;
    }
//    !!!  不是flag 而是oformat里的flags
//    if(!(outfmtCtx->flags & AVFMT_NOFILE))
    if(!(outfmtCtx->oformat->flags & AVFMT_NOFILE))
    {                                               //!!!!!!!
        ret = avio_open(&outfmtCtx->pb,outFilePath,AVIO_FLAG_WRITE);
        if(ret < 0)
        {
            cout<<"avio_open error"<<endl;
            av_freep(streamIndex);
            return -1;
        }
        cout<<"avio_open success"<<endl;
    }
    //!!!!!!!!犯了很多次错误，这是指针，计算数组大小不能用sizeof。
    for(int i = 0;i<fmtCtx->nb_streams;i++)
    {
        cout<<streamIndex[i]<<endl;
    }
    AVPacket * pkt = av_packet_alloc();
    av_init_packet(pkt);

    avformat_write_header(outfmtCtx,NULL);
    while (av_read_frame(fmtCtx,pkt) == 0)
    {                                                             //小于0不能等于 前++ 还是后++
        if(pkt->stream_index >= fmtCtx->nb_streams || streamIndex[pkt->stream_index] < 0 )
        {
            av_packet_unref(pkt);       //记得unref
            continue;
        }

//        AVStream * instream = fmtCtx->streams[pkt->stream_index];
//        pkt->stream_index = streamIndex[pkt->stream_index];
//        AVStream * outstream = outfmtCtx->streams[pkt->stream_index];



        AVStream * instream = fmtCtx->streams[pkt->stream_index];
        AVStream * outstream = outfmtCtx->streams[pkt->stream_index];
        pkt->stream_index = streamIndex[pkt->stream_index];    //如果都是0的话 这里会出问题啊！！！！

        pkt->pts = av_rescale_q(pkt->pts,instream->time_base,outstream->time_base);
        pkt->dts = av_rescale_q(pkt->dts,instream->time_base,outstream->time_base);
        pkt->duration = av_rescale_q(pkt->duration,instream->time_base,outstream->time_base);
        pkt->pos = -1;
        ret = av_interleaved_write_frame(outfmtCtx,pkt);
        if(ret < 0)
        {
            av_packet_unref(pkt);
            return -1;
        }
        av_packet_unref(pkt);
    }

    av_write_trailer(outfmtCtx);

end:
    av_freep(streamIndex);
    return ret;
}
