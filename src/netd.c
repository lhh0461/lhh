#include <event2/event.h>

#include "config.h"

static struct event_base *base;

static int backlog = 5000;

void libevent_init(void)
{
	base = event_base_new();
	if (!base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return 1;
	}
    
}

void gamed_accept_cb(evutil_socket_t gamed_fd, short event, void *arg)
{
    fprintf(stdout, "client accept!!!\n");
}

void open_client_service()
{
    const char *ip config_get_string("OUTER_NETD_IP");
    int port = config_get_int("OUTER_NETD_PORT");
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
    //evutil_socket_t listener_fd = net_listen(ip, port, backlog, 0);
	struct evconnlistener * listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
	    LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
	    (struct sockaddr*)&sin,
	    sizeof(sin));
    struct event *client_accept_event = event_new(base, listener_fd, EV_READ|EV_PERSIST, do_client_accept, (void*)base);
    event_add(client_accept_event, NULL);

}

static void gamed_accept_cb(struct evconnlistener *gamed_listener, 
    evutil_socket_t fd, struct sockaddr *address, int socklen, 
    void *arg); 
{
    fprintf(stdout, "gamed accept!!!\n");
    /* We got a new connection! Set up a bufferevent for it. */
    struct event_base *base = arg;
    //struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(
            base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, echo_read_cb, NULL, echo_event_cb, NULL);
    bufferevent_enable(bev, EV_READ|EV_WRITE);
    
    /* Ok, we can provide service for client ! */ 
    open_client_service();
}

static void gamed_accept_error_cb(struct evconnlistener *gamed_listener, void *arg)
{
    //struct event_base *base = evconnlistener_get_base(gamed_listener);
    struct event_base *base = arg;
    int err = EVUTIL_SOCKET_ERROR();
    fprintf(stderr, "Got an error %d (%s) on the gamed_listener. "
            "Shutting down.\n", err, evutil_socket_error_to_string(err));

    event_base_loopexit(base, NULL);
}

void socket_init(void)
{
    //TODO：先连gamed，再对外提供服务
    const char *gamed_ip config_get_string("GAMED_IP");
    int port config_get_int("PORT");
   // evutil_socket_t gamed_fd = net_listen(ip, port, backlog, 0);
   // struct event *game_accept_event = event_new(base, gamed_fd, EV_READ|EV_PERSIST, do_accept, (void*)base);
   // event_add(game_accept_event, NULL);
	
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
    struct evconnlistener * listener = evconnlistener_new_bind(base, gamed_accept_cb, (void *)base,
	    LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
	    (struct sockaddr*)&sin,
	    sizeof(sin));
    if (!listener) {
        fprintf(stderr, "gamed listener create error!!!\n");
        exit(1);
    }
    evconnlistener_set_error_cb(listener, gamed_accept_error_cb);
    struct event *client_accept_event = event_new(base, listener_fd, EV_READ|EV_PERSIST, do_client_accept, (void*)base);
   
}


void netd_init(void)
{
    libevent_init();
    config_init();
    log_init();
    socket_init();
    signal_init();

}

void netd_stop(void)
{


}

void netd_start(void)
{
    //事件循环
	event_base_dispatch(base);

}

