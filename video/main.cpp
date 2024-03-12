#include <iostream>
#include "./media2h264.h"
#include "./remuxing.h"
#include "./cutvideo.h"
#include "./media2yuv.h"
using namespace std;

int main()
{
    int ret = -1;


    //media提取h264文件
    //avi可以提取 mp4不可以。
    //AnnexB每个都有头 适合实时网络传输，avi。AVCC适合存储，只有一个头，mp4。
    //这个头最重要的就是SPS 序列参数集和 PPS 图像参数集
//    char * infile = "E:/QT_Workspace/FFmpeg/media/ballData.avi";
//    char * outfile = "E:/QT_Workspace/FFmpeg/media/ballDatamedia2h264.h264";
//////    char * infile = "E:/QT_Workspace/FFmpeg/media/ande_10s.mp4";
//////    char * outfile = "E:/QT_Workspace/FFmpeg/media/ande_10s_media2h264.h264";
//    Media2H264 p(infile,outfile);
//    ret = p.media2h264();


    //media格式转换
    //mp4 2 flv
//    char * infile = "E:/QT_Workspace/FFmpeg/media/ande_10s.mp4";
//    char * outfile = "E:/QT_Workspace/FFmpeg/media/ande_10s_mp42flv.flv";

    //flv 2 mp4
/*    char * infile = "E:/QT_Workspace/FFmpeg/media/ande_10s_mp42flv.flv";
    char * outfile = "E:/QT_Workspace/FFmpeg/media/ande_10s_flv2mp4.mp4";  */   //以上两个都可以


    //flv 2 avi
//    char * infile = "E:/QT_Workspace/FFmpeg/media/ande_10s_mp42flv.flv";
//    char * outfile = "E:/QT_Workspace/FFmpeg/media/ande_10s_flv2avi.avi";
    //ballData那个avi为什么没法转。
    //为什么avi的都没法转？？？？
//    Remuxing p(infile,outfile);
//    ret = p.convert();          //代码有点奇怪？？？为什么索引数组每个元素都是0.


    //截取文件。输入开始和结束时间。
//    char * infile = "E:/QT_Workspace/FFmpeg/media/ande_10s_mp42flv.flv";
//    char * outfile = "E:/QT_Workspace/FFmpeg/media/ande_10s_cutVideo.mp4";
//    const char * startTime = "1.5";
//    const char * endTime = "5.5";
//    int tstartTime = atoi(startTime);
//    int tendTime = atoi(endTime);       //只能转为整数
//    CutVideo p(infile,outfile,tstartTime,tendTime);
//    ret = p.cutVideo();

//     将媒体视频的YUV数据提取。
    char * infile = "E:/QT_Workspace/FFmpeg/media/ande_10s.mp4";
    char * outfile = "E:/QT_Workspace/FFmpeg/media/ande_10s_media2YUV.yuv";
    Media2YUV m(infile,outfile);
    ret = m.media2yuv();

    cout<<"end: "<<ret<<endl;
    return 0;
}
