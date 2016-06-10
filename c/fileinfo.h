#ifndef __FILEINFO__H__
#define __FILEINFO__H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openssl/md5.h>

typedef unsigned char byte;

#define MAXLEN 256

struct fileinfo{
    char  name[MAXLEN];
    size_t size;
    byte  info[MD5_DIGEST_LENGTH];//md5码
};

void decode(struct fileinfo *finfo,char *buf,size_t n);

size_t encode(byte *buf,struct fileinfo *finfo);

byte *calc_md5(byte *data,size_t n);

int is_data_correct(struct fileinfo *finfo,byte *data,size_t n);

#endif
