

#include "cutvideo.h"

CutVideo::CutVideo(char * inFile,char * outFile,int tstartTime,int tendTime)
    :inFilePath(inFile),outFilePath(outFile),startTime(tstartTime),endTime(tendTime)
{

}
CutVideo::~CutVideo()
{
    avformat_close_input(&fmtCtx);
    avio_closep(&outfmtCtx->pb);
    avio_close(outfmtCtx->pb);          //这俩有什么区别
    avformat_free_context(outfmtCtx);
    cout<<"~CutVideo()"<<endl;
}
int CutVideo::cutVideo()
{
    int ret = -1;
    ret = init();
    if(ret < 0)
        return ret;
    AVPacket *pkt;
    int *streamIndex = (int *)av_malloc_array(fmtCtx->nb_streams,sizeof (int));
    int streamCount = 0;
    int64_t *ptsIndex;
    int64_t *dtsIndex;
    for (int i = 0;i < fmtCtx->nb_streams;i++)
    {

        AVStream * instream = fmtCtx->streams[i];
        if(!instream && instream->codec->codec_type != AVMEDIA_TYPE_AUDIO &&
                instream->codec->codec_type != AVMEDIA_TYPE_VIDEO &&
                instream->codec->codec_type != AVMEDIA_TYPE_SUBTITLE )
        {
            streamIndex[i] = -1;
            continue;
        }
        AVStream * outstream = avformat_new_stream(outfmtCtx,NULL);
        if(!outstream)
        {
            cout<<"avformat_new_stream error"<<endl;
            goto end;
        }
        avcodec_parameters_copy(outstream->codecpar,instream->codecpar);
        streamIndex[i] = streamCount++;
        outstream->codec->codec_tag = 0;
    }
//    ptsIndex = (int64_t *)av_malloc_array(outfmtCtx->nb_streams,sizeof (int64_t));
//    dtsIndex = (int64_t *)av_malloc_array(outfmtCtx->nb_streams,sizeof (int64_t));
    //!!!!!我底下是要判断是不是为0，所以这里需要设为0
    ptsIndex = (int64_t *)av_mallocz_array(outfmtCtx->nb_streams,sizeof (int64_t));
    dtsIndex = (int64_t *)av_mallocz_array(outfmtCtx->nb_streams,sizeof (int64_t));

//    if(!(outfmtCtx->oformat->flags & AVFMT_NOFILE))       //逻辑搞错了，不是文件才要goto end
    if(outfmtCtx->oformat->flags & AVFMT_NOFILE)
        goto end;
    ret = avio_open(&outfmtCtx->pb,outFilePath,AVIO_FLAG_WRITE);
    if(ret < 0)
    if(ret < 0)
    {
        cout<<"avio_open error"<<endl;
        return -1;
    }
    cout<<"avio_open success"<<endl;

    //!!!!!!最重要的函数忘记了
    //! 这个函数是不是直接可以到指定位置。
    ret = av_seek_frame(fmtCtx,-1,startTime/av_q2d(AV_TIME_BASE_Q),AVSEEK_FLAG_ANY);
//    ret = av_seek_frame(fmtCtx,-1,startTime * AV_TIME_BASE,AVSEEK_FLAG_ANY);

    if(ret < 0)
        goto end;
    pkt = av_packet_alloc();
    avformat_write_header(outfmtCtx,NULL);
    //因为前面的seek 所以来的第一个包 就是起始时间的包。
    while (av_read_frame(fmtCtx,pkt) == 0)
    {
        if(streamIndex[pkt->stream_index] < 0)
            continue;
        AVStream *instream = fmtCtx->streams[pkt->stream_index];
        AVStream *outstream = outfmtCtx->streams[streamIndex[pkt->stream_index]];

                                        //这里是输入流的时间基
//        if(endTime < pkt->pts*av_q2d(outstream->time_base))
        if(endTime < pkt->pts * av_q2d(instream->time_base))
        {
            //!!!!!!!break前先释放。
            av_packet_unref(pkt);
            break;
        }
        if(ptsIndex[pkt->stream_index] == 0)        //判断是不是起始时间来的第一个包
        {
            ptsIndex[pkt->stream_index] = pkt->pts; //是的话记录第一包的时间戳。
        }
        if(dtsIndex[pkt->stream_index] == 0)
        {
            dtsIndex[pkt->stream_index] = pkt->dts;
        }
        pkt->stream_index = streamIndex[pkt->stream_index];
        pkt->dts = av_rescale_q(pkt->dts-dtsIndex[pkt->stream_index],instream->time_base,outstream->time_base);
        pkt->pts = av_rescale_q(pkt->pts-ptsIndex[pkt->stream_index],instream->time_base,outstream->time_base);
        pkt->duration = av_rescale_q(pkt->duration,instream->time_base,outstream->time_base);
        //Packet with invalid duration -9223372036854775808 in stream 0
        //为什么持续时间出了问题？？？            忘记写头和尾了

        pkt->pos = -1;
        if(pkt->pts < 0)
            pkt->pts = 0;
        if(pkt->dts < 0)
            pkt->dts = 0;
        if(pkt->dts > pkt->pts)     //!!!!!!!
        {
            av_packet_unref(pkt);
            continue;
        }
        av_interleaved_write_frame(outfmtCtx,pkt);
        av_packet_unref(pkt);
    }
    av_write_trailer(outfmtCtx);

end:
    av_packet_free(&pkt);
    av_freep(streamIndex);
    av_free(ptsIndex);
    av_free(dtsIndex);     //这边报错！！！！ 为什么用avfreep就报错！！！！！！！！ 上面那个都没报错。


    return 0;
}

int CutVideo::init()
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

