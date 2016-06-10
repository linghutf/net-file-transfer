/**
 *
 * 文件发送
 */

#include "interface.h"
#include "fileinfo.h"

#include "config.h" //主要是客户端配置

static RequestType Status = INIT; //请求类型
struct evbuffer *buf;//保存文件缓存
struct parameter para; //参数

void read_cb(struct bufferevent *bev,void *ctx);
void writetest_cb(struct bufferevent *bev,void *ctx);
void write_cb(struct bufferevent *bev,void *ctx);
void event_cb(struct bufferevent *bev,short events,void *ctx);


int main(int argc, char *argv[])
{
    //载入日志配置文件,非线程安全
    //simplog.loadConfig("log_config.conf");
    //simplog.setLogFile("client.log");

    if(parse_commandline(&para,argc,argv)){
        simplog.writeLog(SIMPLOG_FATAL,"parameter ERROR!");
        return 1;
    }
    //simplog.writeLog(SIMPLOG_DEBUG,"host:%s,port:%d,infile:%s,outfile:%s",para.host,para.port,para.infile,para.outfile);
    struct event_base *base = event_base_new();
    assert(base!=NULL);

    struct sockaddr_in sAddr;
    struct bufferevent *bev;

    bzero(&sAddr,sizeof(sAddr));
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(para.port);
    inet_aton(para.host,&sAddr.sin_addr);

    //需要非阻塞模式IO
    bev = bufferevent_socket_new(base,-1,BEV_OPT_CLOSE_ON_FREE);

    if(bufferevent_socket_connect(bev,(struct sockaddr*)&sAddr,sizeof(sAddr))<0){
        fprintf(stderr,"Connection Refused.\n");
        bufferevent_free(bev);
        return 1;
    }

    //问题关键:EV_READ
    //对方在建立时写了flag,导致这边引发了EV_READ事件，所以在read回调中调用写事件
    //也可以在connect之后先发一个flag,引发对方read操作，
    //要不然双方相互等待
    //这边的读数据需要异步操作
    simplog.writeLog(SIMPLOG_DEBUG,"设置读事件...");
    bufferevent_setcb(bev,read_cb,NULL,event_cb,NULL);
    //首先发起读事件，在读中写入,所以是EV_READ需要添加
    bufferevent_enable(bev,EV_READ|EV_WRITE|EV_PERSIST);

    simplog.writeLog(SIMPLOG_DEBUG,"开启事件循环...");
    event_base_dispatch(base);

    //simplog.writeLog(SIMPLOG_DEBUG,"done.");
    return 0;
}

void read_cb(struct bufferevent *bev,void *ctx)
{
    //读取请求类型
    bufferevent_read(bev,&Status,sizeof(Status));

    simplog.writeLog(SIMPLOG_DEBUG,"接收请求:%s",RequestInfo(Status));
    if(Status == FIN){
        evbuffer_free(buf);
        Status = INIT;
        //不再有读事件
        //struct event_base *base = (struct event_base*)ctx;
        //退出事件循环
        bufferevent_disable(bev,EV_READ|EV_WRITE|EV_PERSIST);

    }
    //对方读是不是触发这方写事件?
    //emit EV_WRITE

    //开始写,BUG :buf信息无法保存   --->   添加FIN,已解决
    write_cb(bev,ctx);
}

//测试连通性，deprecated
/*void writetest_cb(struct bufferevent *bev,void *ctx)
{
    //input_buf是发送缓冲区
    simplog.writeLog(SIMPLOG_DEBUG,"开始写数据!");
    //struct evbuffer *buf = bufferevent_get_input(bev);
    //evbuffer_add(buf,"hello",5);
    bufferevent_write(bev,"hello",5);
}
*/

void write_cb(struct bufferevent *bev,void *ctx)
{
    //struct evbuffer *buf;// = evbuffer_new();// bufferevent_get_output(bev);
    if(Status==HEAD){
        simplog.writeLog(SIMPLOG_DEBUG,"传输信息...");

        buf = evbuffer_new();
        //发送文件信息
        struct fileinfo finfo;

        strcpy(finfo.name,para.infile);

        struct write_arg args;
        args.finfo = &finfo;
        args.buffer = buf;
        //读文件内容
        pthread_t t;
        int s = pthread_create(&t,NULL,(void*)readfile,(void*)&args);
        assert(s==0);


        //encode
        byte *data =(byte*)malloc(sizeof(finfo));
        assert(data!=NULL);

        s = pthread_join(t,NULL);
        assert(s==0);

        //设置输出文件名
        strcpy(finfo.name,para.outfile);

        size_t n = encode(data,&finfo);

        simplog.writeLog(SIMPLOG_DEBUG,"文件名:%s,文件大小:%lu",finfo.name,finfo.size);
        /*simplog.writeLog(SIMPLOG_DEBUG,"序列化文件信息:")''
        for(int i=0;i<n;++i){
            simplog.writeLog(SIMPLOG_DEBUG,"%x",data[i]);
        }*/

        simplog.writeLog(SIMPLOG_DEBUG,"校验信息大小:%lu",n);

        //发送
        /*
        struct evbuffer *output = bufferevent_get_output(bev);
        bufferevent_write(output,(void*)data,n);
        */
        bufferevent_write(bev,data,n);
    }else if(Status == DATA){
        simplog.writeLog(SIMPLOG_DEBUG,"传输文件...");
        //发送文件内容
        //在一次无法接收完的时候会重新执行，但length已变成0
        simplog.writeLog(SIMPLOG_DEBUG,"文件长度:%lu",evbuffer_get_length(buf));
        //struct evbuffer *output = bufferevent_get_output(bev);
        //size_t n = evbuffer_get_length(output);
        //evbuffer_drain(output,n);

        bufferevent_write_buffer(bev,buf);
        /*
        //bufferevent_write()
        evutil_socket_t fd = bufferevent_getfd(bev);
        //写文件内容
        evbuffer_write(buf,fd);
        */
    }/*else if(Status==FIN){
        //清理,重传时不能清理,TCP保证了数据不丢失，则没有考虑怎么很好的清理这快缓存
        //暂时办法，再接受一个信息2,表示正确收到
        evbuffer_free(buf);

        //终止程序
        Status = INIT;
    }*/
}

void event_cb(struct bufferevent *bev,short events,void *ctx)
{
    struct event_base *base = (struct event_base *)ctx;
    evutil_socket_t fd = bufferevent_getfd(bev);
    if (events & BEV_EVENT_CONNECTED) {
        simplog.writeLog(SIMPLOG_INFO,"[%d]connected.\n",fd);
    } else if(events & BEV_EVENT_ERROR){
        simplog.writeLog(SIMPLOG_FATAL,"Got an error on the connection: %s",
                strerror(errno));
        event_base_loopexit(base,NULL);
    } else if(events & BEV_EVENT_EOF){
        simplog.writeLog(SIMPLOG_INFO,"connection closed.");
        bufferevent_free(bev);
    }
}
/*
void event_cb(struct bufferevent *bev,short events,void *ctx)
{
    if (events & BEV_EVENT_CONNECTED) {
        printf("connected");
    } else if (events & BEV_EVENT_EOF) {
        printf("Connection closed.\n");
    } else if (events & BEV_EVENT_ERROR) {
        printf("Got an error on the connection: %s\n",
                strerror(errno));
    }
}
*/
