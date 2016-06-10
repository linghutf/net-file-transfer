#include "fileinfo.h"

void decode(struct fileinfo *finfo,char *buf,size_t n)
{
    memcpy(finfo,buf,n);
}

size_t encode(byte *buf,struct fileinfo *finfo)
{
    size_t n = sizeof(struct fileinfo);
    memcpy(buf,finfo,n);
    return n;
}

byte *calc_md5(byte *data,size_t n)
{
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx,data,n);

   byte *info = (byte*)malloc(sizeof(byte)*MD5_DIGEST_LENGTH);

    if(info==NULL){
        fprintf(stderr,"md5 buffer malloc ERROR!\n");
        return NULL;
    }
    MD5_Final(info,&ctx);

    return info;
}

//没有使用，在数据接收时合并了计算校验
int is_data_correct(struct fileinfo *finfo,byte *data,size_t n)
{
    int ret = 1;
    //数据长度不对
    if(n!=finfo->size){
        ret = 0;
    }else{
        unsigned char *test=NULL;
        while(test==NULL){
            test = calc_md5(data,n);
        }
        if(memcmp(test,finfo->info,sizeof(test))!=0){
            ret = 0;
        }
        free(test);
    }
    return ret;
}

