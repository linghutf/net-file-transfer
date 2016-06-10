#include "interface.h"

char* RequestInfo(RequestType t)
{
    switch(t){
        case INIT:
            strcpy(__str,"INIT");
            break;
        case HEAD:
            strcpy(__str,"HEAD");
            break;
        case DATA:
            strcpy(__str,"DATA");
            break;
        case FIN:
            strcpy(__str,"FIN");
    }
    return __str;
}

void readfile(struct write_arg *args)
{
    struct fileinfo *finfo = args->finfo;
    struct evbuffer *buf = args->buffer;

    FILE *fp = fopen(finfo->name,"rb");
    assert(fp!=NULL);

    MD5_CTX ctx;
    MD5_Init(&ctx);


    byte data[BUFSIZ];
    size_t total = 0,n;

    evbuffer_lock(buf);
    while((n=fread(data,sizeof(byte),BUFSIZ,fp))>0){
        evbuffer_add(buf,data,n);
        MD5_Update(&ctx,data,n);
        total+=n;
    }
    evbuffer_unlock(buf);

    MD5_Final(finfo->info,&ctx);

    finfo->size = total;
}

//0成功,1失败
int writefile(struct write_arg *args)
{
    int ret = 0;

    struct fileinfo *finfo = args->finfo;
    struct evbuffer *buf = args->buffer;

    //当前目录,后期配置
    char *filename = basename(finfo->name);
    FILE *fp = fopen(filename,"wb");
    assert(fp!=NULL);

    size_t len = evbuffer_get_length(buf);
    MD5_CTX ctx;
    byte *md = (byte*)malloc(MD5_DIGEST_LENGTH);
    assert(md!=NULL);
    memset(md,0,MD5_DIGEST_LENGTH);

    MD5_Init(&ctx);
    byte data[BUFSIZ];
    size_t n=0,total=0;//没有初始化导致最后比较文件大小是随机值
    //边读边清理
    //需要加锁
    evbuffer_lock(buf);
    while((n=evbuffer_remove(buf,data,BUFSIZ))>0){
        MD5_Update(&ctx,data,n);
        total+=fwrite(data,sizeof(byte),n,fp);
    }
    fclose(fp);
    evbuffer_unlock(buf);

    MD5_Final(md,&ctx);

    if(total!=len){
        simplog.writeLog(SIMPLOG_FATAL,"file write ERROR!");
        return 1;
    }
    //文件校验码不同
    if(memcmp(md,finfo->info,MD5_DIGEST_LENGTH)!=0){
        simplog.writeLog(SIMPLOG_FATAL,"file content INCORRECT!");
        for(int i=0;i<MD5_DIGEST_LENGTH;++i){
            simplog.writeLog(SIMPLOG_DEBUG,"[%d]---原始:%2x,复原:%x",i,finfo->info[i],md[i]);
        }
        ret = 1;
    }

    free(md);

    return 0;
}
