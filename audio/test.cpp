#include "test.h"

Test::Test(char *inFile):inFilePath(inFile)
{
    int ret = -1;
    fmtCtx = avformat_alloc_context();
    ret  = avformat_open_input(&fmtCtx,inFilePath,NULL,NULL);
//    av_dump_format(fmtCtx,-1,NULL,0);
}
