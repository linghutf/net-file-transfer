/**
 *
 * length \0\0 filename
 *
 */
#include "fileinfo.h"

size_t numToStr(char *tmp,size_t n)
{
    snprintf(tmp,MAX_FILENAME_LEN,"%lu",n);
    return strlen(tmp);
}

void encode_fileinfo(std::array<char,MAX_FILENAME_LEN>& a,const std::string& filename)
{
    size_t n = filename.size();
    char tmp[MAX_FILENAME_LEN];

    size_t len = numToStr(tmp,n);
    int i;
    for(i=0;i<len;++i){
        a[i]=tmp[i];
    }
    a[i++]=SEP_CHAR;
    a[i++]=SEP_CHAR;

    for(;i<len+n+SEP_LEN;++i){
        a[i]=filename[i-len-SEP_LEN];
    }
}

void decode_fileinfo(std::string& filename,const std::array<char,MAX_FILENAME_LEN>& a)
{
    //应该没有超过int那么大的文件
    int n = atoi(a.data());
    char tmp[MAX_FILENAME_LEN];
    size_t len = numToStr(tmp,n);
    filename = std::string(a.begin()+len+SEP_LEN,a.begin()+len+n+SEP_LEN);
}
