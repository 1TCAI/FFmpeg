#include <stdio.h>

#include "libavformat/avformat.h"
#include "libavutil/file.h"

int main()
{
    printf("%s\n",av_version_info());
    AVFormatContext * fmtCtx ;
//    fmtCtx = avformat_alloc_context();   //？？？？？这里是路径的问题？？？？？？？？
    const char * inFileName = "E:/QT_Workspace/FFmpeg/media/ande_10s.mp4";
//    const char * inFileName = ":/../../media/ande_10s.mp4";
    //????????为什么相对路径又打不开？？？？？？？
    int ret = -1;
    ret = avformat_open_input(&fmtCtx,inFileName,NULL,NULL);
    if(ret < 0)
    {
        printf("%d\n",ret);
        printf("avformat_open_input error\n");
        return -1;
    }
    printf("avformat_open_input success\n");
    avformat_close_input(&fmtCtx);
    return 0;
}



int read_func(void* ptr, uint8_t* buf, int buf_size)
{
    FILE* fp = (FILE*)ptr;
    size_t size = fread(buf, sizeof(uint8_t), buf_size, fp);
    int ret = size;
    printf("Read Bytes:%d\n", size);
    return ret;
}


int64_t seek_func(void *opaque, int64_t offset, int whence)
{
    int64_t ret;
    FILE *fp = (FILE*)opaque;
    if (whence == AVSEEK_SIZE) {
        return -1;
    }
    fseek(fp, offset, whence);
    return ftell(fp);
}


int main2(int argc, char *argv[]){

    int ret = 0;
                    //？？？？？？绝对路径可以用 相对不行？？？？？？
    FILE* fp = fopen("E:/QT_Workspace/FFmpeg/media/ande_10s.mp4", "rb");
    int nBufferSize = 1024;
    unsigned char* pBuffer = (unsigned char*)malloc(nBufferSize);

    AVFormatContext* pFormatCtx = NULL;
    AVInputFormat *piFmt = NULL;


    // Allocate the AVIOContext:
    AVIOContext* pIOCtx = avio_alloc_context(
        pBuffer, nBufferSize,
        0,
        fp,
        read_func,
        0,
        seek_func);
    pFormatCtx = avformat_alloc_context();

    pFormatCtx->pb = pIOCtx;//关联，绑定
    pFormatCtx->flags = AVFMT_FLAG_CUSTOM_IO;


printf("hello,avformat_open_input\n");
    //打开流
    if (avformat_open_input(&pFormatCtx, "", piFmt, NULL) < 0) {
        printf("avformat open failed.\n");
        goto quit;
    }
    else {
        printf("open stream success!\n");
    }



    if (avformat_find_stream_info(pFormatCtx, NULL)<0)
    {
        printf("av_find_stream_info error \n");
        goto quit;
    }
    else {
        printf("av_find_stream_info success \n");
        printf("******nb_streams=%d\n",pFormatCtx->nb_streams);
    }


quit:
    avformat_close_input(&pFormatCtx);
    avformat_free_context(pFormatCtx);

    free(pBuffer);
    return 0;
}

struct data
{
    uint8_t * ptr;
    int size;
};
static int read_packet(void *opaque, uint8_t *buf, int buf_size)
{
    struct data *db = (struct data *)opaque;
    buf_size = FFMIN(buf_size, db->size);//!db的是文件剩余量。
    if (!buf_size) //buf_size已读字节数
        return AVERROR_EOF;
    printf("%p , %d\n",db->ptr, buf_size);
    memcpy(buf, db->ptr, buf_size);
    db->ptr += buf_size;
    db->size -= buf_size;
    return buf_size;
}

int main3()
{
    AVFormatContext *pFmtCon = NULL;
    // const char *filename, uint8_t **bufptr, size_t *size, int log_offset, void *log_ctx
    char * filename = "E:/QT_Workspace/FFmpeg/media/ande_10s.mp4";
    size_t buffer_size;
    uint8_t * buffer = NULL;
    //!这个内存映射 是将文件所有的全部存储，buffer  buffer_size 是返回值。然后一点点分给回调函数的4096buffer。
                                    //!buffer buffer_size是返回值 文件内存的开始地址和文件总大小
    int ret = av_file_map(filename, &buffer, &buffer_size, 0, NULL);
    if (ret < 0)
        return -1;

    struct data db ;            //用于记录文件信息
    db.ptr = buffer;
    db.size = buffer_size;

    size_t avio_buffer_size = 1024000;
                //缓冲区
    uint8_t * avio_buffer = (uint8_t *)av_malloc(avio_buffer_size);
    printf("now :%p, %ld\n", buffer, buffer_size);



    AVIOContext *pIoCon = avio_alloc_context(avio_buffer, avio_buffer_size,
                            0, &db, read_packet, NULL, NULL);
    printf("end :%p, %ld\n", buffer, buffer_size);
    if (!pIoCon)
    {
        return -1;
    }
    //AVIOContext 分配好内容 进行绑定就好了
    pFmtCon = avformat_alloc_context();
    pFmtCon->pb = pIoCon;       //!自定义IO
    pFmtCon->flags = AVFMT_FLAG_CUSTOM_IO;
                        //  自定义了io所以url可以是NULL

    avformat_open_input(&pFmtCon, NULL, NULL, NULL);//!在这里调用了三次回调函数。

    ret =  avformat_find_stream_info(pFmtCon, NULL);

    avio_context_free(&pIoCon);
    if (pIoCon)
        av_freep(&pIoCon->buffer);
    avformat_close_input(&pFmtCon);
    av_file_unmap(buffer, buffer_size);
    return 0;
}

