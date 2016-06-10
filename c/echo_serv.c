/**
 * Echo 服务器
 */

#include "interface.h"

#define PORT 9000

void listen_cb(struct evconnlistener *,evutil_socket_t,struct sockaddr *,size_t socklen,void *ctx);
void read_cb(struct bufferevent *bev,void *ctx);
void read_buffered_cb(struct bufferevent *bev,void *ctx);
void write_cb(struct bufferevent *bev,void *ctx);
void event_cb(struct bufferevent *bev,short events,void *ctx);
void signal_cb(evutil_socket_t,short,void*);

int main(int argc, char *argv[])
{
    struct event_base *base = event_base_new();
    assert(base!=NULL);

    struct evconnlistener *listener;
    struct event *signal_event;
    struct sockaddr_in myAddr;

    bzero(&myAddr,sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(PORT);

    listener = evconnlistener_new_bind(base,(evconnlistener_cb)listen_cb,(void*)base,
            LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,-1,
            (struct sockaddr *)&myAddr,sizeof(myAddr));
    assert(listener!=NULL);

    signal_event = evsignal_new(base,SIGINT,signal_cb,(void*)base);
    assert(signal_event != NULL||event_add(signal_event,NULL)<0);

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

    bufferevent_setcb(bev,read_buffered_cb,NULL,event_cb,NULL);
    bufferevent_enable(bev,EV_WRITE|EV_READ);

}

void read_cb(struct bufferevent *bev,void *ctx)
{
    char data[BUFSIZ];
    size_t n = bufferevent_read(bev,data,BUFSIZ);
    fwrite(data,1,n,stdout);
}

void read_buffered_cb(struct bufferevent *bev,void *ctx)
{
    //读取接收的数据
    struct evbuffer *buf = bufferevent_get_input(bev);
    size_t len = evbuffer_get_length(buf);

    char data[BUFSIZ];
    ev_ssize_t n = evbuffer_remove(buf,(void*)data,BUFSIZ);

   // while(bufferevent_read(bev,data,BUFSIZ,)
    fwrite(data,1,n,stdout);

}
void write_cb(struct bufferevent *bev,void *ctx)
{

}

void event_cb(struct bufferevent *bev,short events,void *ctx)
{
    evutil_socket_t fd = bufferevent_getfd(bev);
    if (events & BEV_EVENT_EOF) {
        printf("[%d]Connection closed.\n",fd);
    } else if (events & BEV_EVENT_ERROR) {
        printf("Got an error on the connection: %s\n",
                strerror(errno));
    }
    bufferevent_free(bev);
}

void signal_cb(evutil_socket_t fd,short events,void *ctx)
{
    struct event_base *base = (struct event_base *)ctx;
    struct timeval delay = {0, 0};
    printf("\nexit...\n");
    event_base_loopexit(base, &delay);
}
