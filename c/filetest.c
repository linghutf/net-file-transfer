/**
 * 文件byte化测试
 *
 */
#include "fileinfo.h"

int main(int argc, char *argv[])
{
    struct fileinfo info;
    strcpy(info.name,"1.c");
    info.size = 4;
    char buf[]={0x1,0x2,0x3,0x4};

    byte *p = calc_md5(buf,info.size);
    memcpy(info.info,p,MD5_DIGEST_LENGTH);
    free(p);
    //fwrite(info.info,1,MD5_DIGEST_LENGTH,stdout);
    for(int i=0;i<MD5_DIGEST_LENGTH;++i){
        printf("%x",info.info[i]);
    }
    printf("\n");


    char data[100];
    size_t len = encode(data,&info);

    struct fileinfo test;
    decode(&test,data,len);

    printf("name:%s\nsize:%lu\ninfo:",test.name,test.size);
    for(int i=0;i<MD5_DIGEST_LENGTH;++i){
        printf("%x",test.info[i]);
    }


    return 0;
}
