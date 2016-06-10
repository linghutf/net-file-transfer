/**
 * 配置自定义从命令解析参数
 */
#ifndef __CONFIG__H__
#define __CONFIG__H__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#define HOST "127.0.0.1"
#define PORT 9000

#define MAXLEN 256

struct parameter{
    short port;
    char host[50];
    char infile[MAXLEN];
    char outfile[MAXLEN];
};

int parse_commandline(struct parameter *p,int argc,char *argv[]);

#endif
