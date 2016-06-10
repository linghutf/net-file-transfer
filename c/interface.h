#ifndef __INTERFACE__HH__
#define __INTERFACE__HH__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <assert.h>

#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h> //basename

#include <sys/socket.h>
#include <arpa/inet.h>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

#include <openssl/md5.h>

#include "simplog.h"
#include "fileinfo.h"


typedef unsigned char byte;


//pthread参数
struct write_arg{
    struct fileinfo *finfo;
    struct evbuffer *buffer;
};

typedef enum{
    INIT, //初始化
    HEAD, //file header
    DATA, //file data
    FIN   //finish
}RequestType;

char __str[5];
char *RequestInfo(RequestType t);

/*
struct node{
    struct evbuffer *buffer;
    struct node *next,*prev;
};

struct queue{
    struct node *head;
    struct node *tail;
};

struct queue *queue_new()
{
    struct queue *q = (struct queue*)malloc(sizeof(struct queue));
    q->head=q->tail=NULL;
    assert(q!=NULL);
    return q;
}

void queue_free(struct queue *q)
{
    struct node *p = q->head;
    struct node *prev = p;
    while(p!=NULL){
        p = p->next;
        free(prev);
        prev=p;
    }
    q->head=q->tail=NULL;
}

struct queue *queue_add(struct queue *q,struct evbuffer *buffer)
{

    struct node *nod = (struct node *)malloc(sizeof(struct node));
    assert(nod!=NULL);
    nod->buffer = buffer;
    nod->next=NULL;
    nod->prev=NULL;

    if(q->head==NULL&&q->tail==NULL){
        q->head = q->tail = nod;
    }else{
        q->tail->next = nod;
        nod->prev = q->tail;
        q->tail = nod;
    }
    return q;
}

struct queue *queue_pop(struct queue *q,struct evbuffer *deldst)
{
    if((q->head==NULL&&q->tail==NULL)
        return q;
    struct node *delnod = q->tail;
    q->tail = q->tail->prev;

    struct evbuffer *tmp = deldst;
    deldst = delnod->buffer;
    delnod->buffer = tmp;
    free(delnod);
}

//写文件
void writeFile(char *filename,struct queue *q)
{
    FILE *fp = fopen(filename,"wb");
    assert(fp!=NULL);
    size_t n;
    //怎么取出数据evbuffer_write?
    for(struct node *nod = q->head;nod!=NULL;nod=nod->next){
        n = evbuffer_write(nod->buffer,(int)*fp);
    }
    fclose(fp);
}

//
void write_data(FILE *fp,struct evbuffer *buffer)
{
    //会导致fp偏移量改变?
    evbuffer_write(buffer,*fp);
}

*/

void readfile(struct write_arg *args);

int writefile(struct write_arg *args);

#endif
