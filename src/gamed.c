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
#include "network.h"
#include "protocal.h"

extern struct event_base *g_base;

static net_connection_t *g_gated_conn;

static void libevent_init(void)
{
	g_base = event_base_new();
	if (!g_base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		exit(1);
	}
}

void gated_read_cb(struct bufferevent *bev, void *arg)
{
    fprintf(stdout, "gated read cb!!!\n");
    struct evbuffer *input = bufferevent_get_input(bev);
    char buff[1024] = {'\0'};
    while (evbuffer_get_length(input) > 0) {
        evbuffer_remove(input, buff, sizeof(buff));
        printf("gamed get data: %s\n", buff);
        memset(buff, '\0', sizeof(buff));
    }
    //gamed_rpc_dispatch();
}

void gated_error_cb(struct bufferevent *bev, short what, void *arg)
{
    struct event_base *base = bufferevent_get_base(bev);
    fprintf(stdout, "gated error cb!!!\n");
    int err = EVUTIL_SOCKET_ERROR();
    fprintf(stderr, "Got an error %d (%s) on the gated_error_cb."
            "Shutting down.\n", err, evutil_socket_error_to_string(err));
    event_base_loopexit(base, NULL);
}

static void socket_init(void)
{
    //先连gated
    const char *gated_ip = config_get_string("GATED_IP");
    int port = config_get_int("INTER_GATED_PORT");
	
    int gated_fd = net_connect(gated_ip, port, 0);
    if (gated_fd < 0) {
        printf("fail to connect gated!!!\n");
        exit(1);
    }
    g_gated_conn =  net_new_connection(gated_fd, gated_read_cb, NULL, gated_error_cb);
    auth_packet_t authpack;
    authpack.cmd = AUTH_GAMED;
    authpack.len = sizeof(authpack);
    fprintf(stdout, "authpack.len = %d!!\n", authpack.len);
    int ret = net_send_packet(g_gated_conn, (void *)&authpack, sizeof(authpack));
    fprintf(stdout, "ret = %d!!\n", ret);

}

void gamed_init(void)
{
    libevent_init();
    config_init();
    log_init("gated", 0);
    socket_init();
    vm_init();
    //signal_init();

}

void gamed_stop(void)
{


}

void gamed_start(void)
{
    //事件循环
	event_base_dispatch(g_base);
    //TODO:gamed跑游戏逻辑

}

