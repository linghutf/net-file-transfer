/*
 *
 * 文件接收
 *
 */
#include "interface.h"
#include "fileinfo.h"
#include "config.h"

//配置PORT和HOST
struct parameter para;

//首先接收文件名 -> HEAD
//然后接收内容 -> DATA
//完成-> FIN

static RequestType Status = INIT; //请求类型

void listen_cb(struct evconnlistener *,evutil_socket_t,struct sockaddr *,size_t socklen,void *ctx);
void read_cb(struct bufferevent *bev,void *ctx);
void readtest_cb(struct bufferevent *bev,void *ctx);
void read_buffered_cb(struct bufferevent *bev,void *ctx);

void write_cb(struct bufferevent *bev,void *ctx);
void event_cb(struct bufferevent *bev,short events,void *ctx);
void signal_cb(evutil_socket_t,short,void*);


int main(int argc, char *argv[])
{
    //载入日志配置文件
    simplog.loadConfig("log_config.conf");
    simplog.setLogFile("server.log");

    if(parse_commandline(&para,argc,argv)){
        simplog.writeLog(SIMPLOG_FATAL,"configure ERROR!");
        return 1;
    }

    struct event_base *base = event_base_new();
    assert(base!=NULL);

    struct evconnlistener *listener;
    struct event *signal_event;
    struct sockaddr_in myAddr;

    bzero(&myAddr,sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    inet_aton(para.host,&myAddr.sin_addr);
    myAddr.sin_port = htons(para.port);

    listener = evconnlistener_new_bind(base,(evconnlistener_cb)listen_cb,(void*)base,
            LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,-1,
            (struct sockaddr *)&myAddr,sizeof(myAddr));
    assert(listener!=NULL);

    //挂载信号事件
    signal_event = evsignal_new(base,SIGINT,signal_cb,(void*)base);
    assert(signal_event !=NULL && event_add(signal_event,NULL)>=0);

    event_base_dispatch(base);

    evconnlistener_free(listener);
    event_free(signal_event);
    event_base_free(base);


    return 0;
}

void listen_cb(struct evconnlistener *listener,evutil_socket_t listenfd,struct sockaddr *addr,size_t socklen,void *ctx)
{
    struct event_base *base = (struct event_base *)ctx;
    struct bufferevent *bev = bufferevent_socket_new(base,listenfd,BEV_OPT_CLOSE_ON_FREE);
    assert(bev!=NULL);

    simplog.writeLog(SIMPLOG_INFO,"绑定事件.");
    //数据到来就会引发read_cd,这就是libevent的事件驱动
    bufferevent_setcb(bev,read_cb,NULL,event_cb,NULL);

    bufferevent_enable(bev,EV_READ|EV_WRITE|EV_PERSIST);

    //触发对方的读事件,问题关键
    Status = HEAD;//请求文件信息
    bufferevent_write(bev,&Status,sizeof(Status));
}

//测试连通性,deprecated
/*void readtest_cb(struct bufferevent *bev,void *ctx)
{
    //output是接收的缓冲区
    //struct evbuffer *buf = bufferevent_get_output(bev);
    //char data[10];
    //evbuffer_copyout(buf,data,10);
    char data[100];
    size_t n;
    //需要阻塞读取(IO异步)
    while((n=bufferevent_read(bev,data,100))>0){
        data[n]='\0';
        simplog.writeLog(SIMPLOG_INFO,"[接收数据]%s",data);
    }
}
*/
void read_cb(struct bufferevent *bev,void *ctx)
{
    //simplog.writeLog(SIMPLOG_DEBUG,"开始接受数据.");
    struct fileinfo finfo;
    if(Status==HEAD){//返回HEAD response
        char data[BUFSIZ];
        //清理数据
        memset(&finfo,0,sizeof(finfo));
        memset(data,0,BUFSIZ);
        //读取信息
        size_t n = bufferevent_read(bev,data,BUFSIZ);
        simplog.writeLog(SIMPLOG_DEBUG,"接收数据大小:%lu\n",n);
        //反序列化成fileinfo
        decode(&finfo,data,n);
/*
        simplog.writeLog(SIMPLOG_DEBUG,"哈希码:");
        for(int i=0;i<MD5_DIGEST_LENGTH;++i){
            simplog.writeLog(SIMPLOG_DEBUG,"%x",finfo.info[i]);
        }
*/
        //设置接收文件内容
        simplog.writeLog(SIMPLOG_DEBUG,"文件名:%s,大小:%lu\n",finfo.name,finfo.size);

        //请求DATA
        Status = DATA;
        //再次引发对方read事件
        bufferevent_write(bev,&Status,sizeof(Status));
    }else if(Status==DATA){
        //方案2
        simplog.writeLog(SIMPLOG_DEBUG,"接收文件...");
        //buf需要同步加锁
        //获取
        struct evbuffer *buf = bufferevent_get_input(bev);
        //阻塞到完全接收,空等,性能优化点
        //可以在buf满足条件下发出事件通知
        /*
        size_t nCache = 0;
        while((nCache = evbuffer_get_length(buf))!=finfo.size){
            simplog.writeLog(SIMPLOG_DEBUG,"缓冲区:%lu",nCache);
        }*/
        //一次没有接收完
        if(evbuffer_get_length(buf)==finfo.size){
            simplog.writeLog(SIMPLOG_DEBUG,"准备写入文件...");
            pthread_t t;
            struct write_arg args;
            args.finfo = &finfo;
            args.buffer = buf;

            int s = pthread_create(&t,NULL,(void*)writefile,(void*)&args);
            assert(s==0);
            //等待完成
            void *ret;
            s = pthread_join(t,&ret);
            assert(s==0);
            simplog.writeLog(SIMPLOG_DEBUG,"写入结果:%ld",(long)ret);
            if((long)ret){
                //TCP保证了数据有序到达，这处没有必要，为了double-check
                simplog.writeLog(SIMPLOG_FATAL,"文件传输错误!");
                //申请重传
                Status = DATA;
            }else{
                //无需确认
                Status = FIN;
                simplog.writeLog(SIMPLOG_DEBUG,"文件写入完成.");
            }
            bufferevent_write(bev,&Status,sizeof(Status));
        }
        /*
        struct evbuffer *buf = evbuffer_new();
        char data[BUFSIZ];
        size_t n;
        //文件缓存
        while((n=bufferevent_read(bev,data,BUFSIZ))>0){
            evbuffer_add(buf,data,n);
        }
        size_t total = evbuffer_get_length(buf);
        byte *chunk = (byte*)malloc()
        ev_ssize_t read_len = evbuffer_copyout()
        */

        /*单线程阻塞方式*/
        //可行方案1
        /*
        simplog.writeLog(SIMPLOG_DEBUG,"写文件...");

        FILE *fp = fopen(basename(finfo.name),"wb");
        assert(fp!=NULL);

        //写文件
        size_t n,numWrite=0,total=0;
        byte data[BUFSIZ];
        while((n=bufferevent_read(bev,data,BUFSIZ))>0){
            numWrite+=fwrite(data,1,n,fp);
            total+=n;
        }
        simplog.writeLog(SIMPLOG_DEBUG,"收到文件大小:%lu\n实际写入:%lu",total,numWrite);
        */
        /*
        //接收文件内容
        struct evbuffer *buf = bufferevent_get_output(bev);
        size_t len = evbuffer_get_length(buf);

        //可以写
        if(len>=finfo.size){
            char data[BUFSIZ];
            size_t n = 0;
            ev_ssize_t cnt;
            while(n<finfo.size){
                //copy data
                cnt = evbuffer_copyout(buf,data,BUFSIZ);
                evbuffer_drain(buf,cnt);
                //write
                n+=fwrite(data,sizeof(byte),cnt,fp);
            }
        }
        fclose(fp);
        */
    }
    //再次请求，不能移到外面，否则在数据没有完全到来的时候会重复请求
    //bufferevent_write(bev,&Status,sizeof(Status));
}

void write_cb(struct bufferevent *bev,void *ctx)
{

}

void event_cb(struct bufferevent *bev,short events,void *ctx)
{
    evutil_socket_t fd = bufferevent_getfd(bev);
    if (events & BEV_EVENT_CONNECTED) {
        simplog.writeLog(SIMPLOG_INFO,"[%d]connected.\n",fd);
    } else if(events & (BEV_EVENT_EOF|BEV_EVENT_ERROR)){
        struct event_base *base = (struct event_base *)ctx;
        if(events & BEV_EVENT_ERROR){
            simplog.writeLog(SIMPLOG_FATAL,"Got an error on the connection: %s",
                    strerror(errno));
        }
        simplog.writeLog(SIMPLOG_DEBUG,"[%d] 断开连接!",fd);
        //bufferevent_free(bev);
        //event_base_loopexit(base,NULL);
    }
}

void signal_cb(evutil_socket_t fd,short events,void *ctx)
{
    struct event_base *base = (struct event_base *)ctx;
    struct timeval delay = {0, 0};
    simplog.writeLog(SIMPLOG_INFO,"exit...");
    event_base_loopexit(base, &delay);
}

