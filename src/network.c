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
//TODO:发送数据
//
//


int net_convert_addr(const char*ip, unsigned short port, struct sockaddr_in *addr)
{
	struct in_addr inaddr;
	memset(&inaddr, 0, sizeof(inaddr));
	memset(addr, 0, sizeof(*addr));
	int ret = inet_aton(ip, &inaddr);
	if (1 == ret) {
		addr->sin_family = AF_INET;
		addr->sin_port = htons(port);
		//addr->sin_addr.s_addr = inaddr.s_addr;
		addr->sin_addr = inaddr;
	}

	assert(ret == 1);

	return ret;
}

int net_set_block(int fd, int *rflag)
{
	int flag = fcntl(fd, F_GETFL, 0);
    if (flag < 0){
        log_perror("fcntl");
        return -1;
    }
	if (rflag != NULL) {
		*rflag = flag;
	}
	flag = fcntl(fd, F_SETFL, flag & (~O_NONBLOCK));
    if (flag < 0){
        log_perror("fcntl");
        return -1;
    }
    return 0;
}

int net_set_nonblock(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag < 0){
        log_perror("fcntl");
        return -1;
    }
    flag = fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    if (flag < 0){
        log_perror("fcntl");
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
	int fd;
	printf("listen on %s:%d\n", ip, port);
	if (1 == net_convert_addr(ip, port, &addr)) {
		fd = socket(PF_INET, SOCK_STREAM, 0);
		if (0 > fd) {
			perror("fs net socket");
			return -1;
		}
		int reuse = 1;
		if (0 > setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int))) {
			perror("fs net setsockopt");
			close(fd);
			return -1;
		}
		if (0 > bind(fd, (struct sockaddr*)&addr, sizeof(addr))) {
			perror("fs net bind");
			close(fd);
			return -1;
		}
		if (0 > listen(fd, backlog)) {
			perror("fs net listen");
			close(fd);
			return -1;
		}
		if (!block) {
			if (0 > net_set_nonblock(fd)){
				perror("fs net fcntl noblock");
				close(fd);
				return -1;
			}
		}

		return fd;
	}

	return -1;
}

