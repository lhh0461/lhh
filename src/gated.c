#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <arpa/inet.h>

#include "config.h"
#include "log.h"

extern struct event_base *g_base;

static void libevent_init(void)
{
	g_base = event_base_new();
	if (!g_base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		exit(1);
	}
}

void client_read_cb(struct bufferevent *bev, void *arg)
{
    struct evbuffer *gamed_input = bufferevent_get_input(bev);
    struct evbuffer *gamed_output = bufferevent_get_output(bev);

    evbuffer_add_buffer(gamed_output, gamed_input);
    fprintf(stdout, "client read cb!!!\n");
}

void client_error_cb(struct bufferevent *bev, short what, void *arg)
{
    struct event_base *base = bufferevent_get_base(bev);
    fprintf(stdout, "client error cb!!!\n");
    fprintf(stdout, "gamed error cb!!!\n");
    int err = EVUTIL_SOCKET_ERROR();
    fprintf(stderr, "Got an error %d (%s) on the client_bufferevent_setcb. "
            "Shutting down.\n", err, evutil_socket_error_to_string(err));
    event_base_loopexit(base, NULL);

}

static void client_accept_cb(struct evconnlistener *client_listener, 
    evutil_socket_t fd, struct sockaddr *address, int socklen, 
    void *arg) 
{
    fprintf(stdout, "client accept!!!\n");
    /* We got a new connection! Set up a bufferevent for it. */
    struct event_base *base = evconnlistener_get_base(client_listener);
    struct bufferevent *bev = bufferevent_socket_new(
            base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, client_read_cb, NULL, client_error_cb, NULL);
    bufferevent_enable(bev, EV_READ|EV_WRITE);
}

static void client_accept_error_cb(struct evconnlistener *gamed_listener, void *arg)
{
    //struct event_base *base = evconnlistener_get_base(gamed_listener);
    int err = EVUTIL_SOCKET_ERROR();
    fprintf(stderr, "Got an error %d (%s) on the client_listener. "
            "Shutting down.\n", err, evutil_socket_error_to_string(err));
 
    //event_base_loopexit(base, NULL);
}

void open_client_service(struct event_base *base)
{
    int port = config_get_int("OUTER_GATED_PORT");
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

    struct evconnlistener *client_listener = evconnlistener_new_bind(base, client_accept_cb, (void *)base,
	    LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
	    (struct sockaddr*)&sin,
	    sizeof(sin));

    if (!client_listener) {
        int err = EVUTIL_SOCKET_ERROR();
        fprintf(stderr, "Got an error [%d][%s] on the evconnlistener_new_bind. "
                "Shutting down.\n", err, evutil_socket_error_to_string(err));
        fprintf(stderr, "client listener create error!!!\n");
        exit(1);
    }
    evconnlistener_set_error_cb(client_listener, client_accept_error_cb);
}

void gamed_read_cb(struct bufferevent *bev, void *arg)
{
    struct event_base *base = bufferevent_get_base(bev);
    struct evbuffer *gamed_input = bufferevent_get_input(bev);
    struct evbuffer *gamed_output = bufferevent_get_output(bev);

    //char buff[1024] = {'\0'};
    //evbuffer_remove(gamed_input, buff, sizeof(buff));
    //fprintf(stdout, "gated get data : [%s]!!!\n", buff);
    evbuffer_add_buffer(gamed_output, gamed_input);
    //fprintf(stdout, "gamed read cb!!!\n");
    /* Ok, we can provide service for client ! */ 
    open_client_service(base);
}

void gamed_error_cb(struct bufferevent *bev, short what, void *arg)
{
    struct event_base *base = bufferevent_get_base(bev);
    fprintf(stdout, "gamed error cb!!!\n");
    int err = EVUTIL_SOCKET_ERROR();
    fprintf(stderr, "Got an error %d (%s) on the gamed_bufferevent_setcb. "
            "Shutting down.\n", err, evutil_socket_error_to_string(err));
    event_base_loopexit(base, NULL);
}

static void gamed_accept_cb(struct evconnlistener *gamed_listener, 
    evutil_socket_t fd, struct sockaddr *address, int socklen, 
    void *arg) 
{
    fprintf(stdout, "gamed accept!!!\n");
    /* We got a new connection! Set up a bufferevent for it. */
    struct event_base *base = evconnlistener_get_base(gamed_listener);
    struct bufferevent *bev = bufferevent_socket_new(
            base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, gamed_read_cb, NULL, gamed_error_cb, NULL);
    bufferevent_enable(bev, EV_READ|EV_WRITE);
}

static void gamed_accept_error_cb(struct evconnlistener *gamed_listener, void *arg)
{
    struct event_base *base = evconnlistener_get_base(gamed_listener);
    int err = EVUTIL_SOCKET_ERROR();
    fprintf(stderr, "Got an error %d (%s) on the gamed_listener. "
            "Shutting down.\n", err, evutil_socket_error_to_string(err));
 
    event_base_loopexit(base, NULL);
}

static void socket_init(void)
{
    //先连gamed，再对外提供服务
    int port = config_get_int("INTER_GATED_PORT");
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
    struct evconnlistener * gamed_listener = evconnlistener_new_bind(g_base, gamed_accept_cb, NULL,
	    LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
	    (struct sockaddr*)&sin,
	    sizeof(sin));
    if (!gamed_listener) {
        fprintf(stderr, "gamed listener create error!!!\n");
        exit(1);
    }
    evconnlistener_set_error_cb(gamed_listener, gamed_accept_error_cb);
}

static void signal_init()
{
    
}

void gated_init(void)
{
    libevent_init();
    config_init();
    log_init("gated", 0);
    socket_init();
    signal_init();

}

void gated_stop(void)
{


}

void gated_start(void)
{
    //事件循环
	event_base_dispatch(g_base);

}

