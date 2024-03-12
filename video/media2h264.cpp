#include "media2h264.h"



Media2H264::Media2H264(char * inFile,char * outFile)
    :inFilePath(inFile),outFilePath(outFile)
{
    av_log_set_level(AV_LOG_INFO);
}
Media2H264::~Media2H264()
{
    fclose(fp);
    avformat_close_input(&fmtCtx);
    cout<<"~Media2AAC()"<<endl;
}

int Media2H264::init()
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
    ret = av_find_best_stream(fmtCtx,AVMEDIA_TYPE_VIDEO,-1,-1,nullptr,0);
    if(ret < 0)
    {
        cout<<"av_find_best_stream error"<<endl;
        return -1;
    }
    cout<<"av_find_best_stream success"<<endl;
    video_index = ret;
    video_stream = fmtCtx->streams[video_index];
    if(!video_stream)
    {
        cout<<"video_stream error"<<endl;
        return -1;
    }
    cout<<"video_stream success"<<endl;
    fp = fopen(outFilePath,"wb");
    if(!fp)
    {
        cout<<"fopen error"<<endl;
        return -1;
    }
    cout<<"fopen success"<<endl;
    return 0;
}



int Media2H264::media2h264()
{
    int ret = -1;
    if(init())
        return -1;
    AVPacket *pkt;

    pkt = av_packet_alloc();
    av_init_packet(pkt);
    AVBSFContext *bsfCtx;

    const AVBitStreamFilter *bsf = av_bsf_get_by_name("h264_mp4toannexb");
    if(!bsf)
    {
        cout<<"av_bsf_get_by_name error"<<endl;
        goto end;
    }
    cout<<"av_bsf_get_by_name success"<<endl;
    av_bsf_alloc(bsf,&bsfCtx);
    ret = avcodec_parameters_copy(bsfCtx->par_in,video_stream->codecpar);
    if(ret < 0)
    {
        cout<<"avcodec_parameters_copy error"<<endl;
        goto end;
    }
    cout<<"avcodec_parameters_copy success"<<endl;
    av_bsf_init(bsfCtx);
    while (av_read_frame(fmtCtx,pkt) == 0)
    {
        if(pkt->stream_index != video_index)
            continue;
                //！！！这里是size 不是data_size
        if(av_bsf_send_packet(bsfCtx,pkt) == 0)
        {
            while (av_bsf_receive_packet(bsfCtx,pkt)==0)
            {
                ret = fwrite(pkt->data,1,pkt->size,fp);
                if(ret != pkt->size)
                {
                    av_packet_unref(pkt);
                    break;
                }
            }
        }
        av_packet_unref(pkt);
    }

end:
//    decode(NULL);           //不涉及编解码不用刷新！！！
    av_bsf_free(&bsfCtx);
    av_packet_free(&pkt);
    cout<<"media2h264 success"<<endl;
    return 0;
}
