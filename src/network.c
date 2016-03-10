// 通用的网络操作

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "network.h"
#include "protocal.h"

extern struct event_base *g_base;

//地址转换函数
int net_convert_addr(const char*ip, unsigned short port, struct sockaddr_in *addr)
{
	struct in_addr inaddr;
	memset(&inaddr, 0, sizeof(inaddr));
	memset(addr, 0, sizeof(*addr));
	int ret = inet_aton(ip, &inaddr);
	if (1 == ret) {
		addr->sin_family = AF_INET;
		addr->sin_port = htons(port);
		addr->sin_addr = inaddr;
	}
	assert(ret == 1);
	return ret;
}

//设置文件描述符为阻塞
int net_set_block(int fd, int *rflag)
{
	int flag = fcntl(fd, F_GETFL, 0);
    if (flag < 0){
        printf("fcntl error\n");
        return -1;
    }
	if (rflag != NULL) {
		*rflag = flag;
	}
	flag = fcntl(fd, F_SETFL, flag & (~O_NONBLOCK));
    if (flag < 0){
        printf("fcntl error\n");
        return -1;
    }
    return 0;
}

//设置文件描述符为阻塞
int net_set_nonblock(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag < 0){
        printf("fcntl error\n");
        return -1;
    }
    flag = fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    if (flag < 0){
        printf("fcntl error\n");
        return -1;
    }

    return 0;
}

//连接网络
int net_connect(const char *ip, unsigned short port, int block)
{
	struct sockaddr_in addr;
	int fd;
	printf("connect to %s:%d\n", ip, port);
	if (1 == net_convert_addr(ip, port, &addr)) {
		fd = socket(PF_INET, SOCK_STREAM, 0);
		if (0 > fd) {
			perror("fs net socket");
			return -1;
		}
		if (!block) {
			if (0 > net_set_nonblock(fd)){
				perror("fs net fcntl noblock");
				close(fd);
				return -1;
			}
		}
		if (0 > connect(fd, (struct sockaddr*)&addr, sizeof(addr))) {
			if (!block && errno == EINPROGRESS) {
				return fd;
			}
			perror("net connect error!!!");
			close(fd);
			return -1;
		}

		return fd;
	}

	return -1;
}

//网络监听
int net_listen(const char *ip, unsigned short port, int backlog, int block)
{
	struct sockaddr_in addr;
	evutil_socket_t fd;
	printf("listen on %s:%d\n", ip, port);
	if (1 == net_convert_addr(ip, port, &addr)) {
		fd = socket(PF_INET, SOCK_STREAM, 0);
		if (0 > fd) {
            fprintf(stderr, "create socket faild!!!\n");
			return -1;
		}
		if (0 > evutil_make_listen_socket_reuseable(fd)) {
            fprintf(stderr, "make socket reuseable faild!!!\n");
			close(fd);
			return -1;
		}
		if (0 > bind(fd, (struct sockaddr*)&addr, sizeof(addr))) {
            fprintf(stderr, "bind socket faild!!!\n");
			close(fd);
			return -1;
		}
		if (0 > listen(fd, backlog)) {
            fprintf(stderr, "listen socket faild!!!\n");
			close(fd);
			return -1;
		}
		if (!block) {
			if (0 > evutil_make_socket_nonblocking(fd)){
                fprintf(stderr, "set socket nonblock faild!!!\n");
				close(fd);
				return -1;
			}
		}

		return fd;
	}

	return -1;
}

//创建连接结构体，每一条连接都有一个连接结构体
net_connection_t *net_new_connection(int fd, bufferevent_data_cb conn_read_cb, 
    bufferevent_data_cb conn_write_cb, bufferevent_event_cb conn_error_cb)
{
    //TODO:改用jmelloc
    net_connection_t *conn = NULL;
    conn = (net_connection_t *)malloc(sizeof(net_connection_t));
    if (conn == NULL) {
        fprintf(stderr, "create connection struct faild!!!\n");
        return NULL;
    }
    conn->fd = fd;
    conn->auth = AUTH_NONE;
    conn->udata = NULL;
    conn->bev = bufferevent_socket_new(g_base, fd, BEV_OPT_CLOSE_ON_FREE); 
    if (!conn->bev) {
        fprintf(stderr, "create socket bufferevent faild!!!\n");
        free(conn);
        return NULL;
    }
    bufferevent_setcb(conn->bev, conn_read_cb, NULL, conn_error_cb, NULL);
    bufferevent_enable(conn->bev, EV_READ|EV_WRITE);
    return conn;
}

//向某个连接发送数据
int net_send_packet(net_connection_t *conn, void *data, int datalen)
{
    int ret = bufferevent_write(conn->bev, data, datalen); 
    if (ret != 0) {
        fprintf(stderr, "net send packet error.bufsize=%lu,errno=%d\n",
                evbuffer_get_length(bufferevent_get_output(conn->bev)), errno);
    }
    return ret;
}
