#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <event2/util.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

static char *gated_ip = "127.0.0.1";
static int port = 2540;

int main(void)
{
	struct sockaddr_in addr;
	evutil_socket_t fd;
	printf("connect to %s:%d\n", gated_ip, port);
	struct in_addr inaddr;
	memset(&inaddr, 0, sizeof(inaddr));
	memset(&addr, 0, sizeof(addr));
	int ret = inet_aton(gated_ip, &inaddr);
	if (1 == ret) {
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		//addr->sin_addr.s_addr = inaddr.s_addr;
		addr.sin_addr = inaddr;
	}
	assert(ret == 1);
	
    fd = socket(PF_INET, SOCK_STREAM, 0);
    if (0 > fd) {
        perror("fs net socket");
        return -1;
    }
    int block = 0;
    if (!block) {
        if (0 > evutil_make_socket_nonblocking(fd)){
            perror("fs net fcntl noblock");
            close(fd);
            return -1;
        }
    }
    if (0 >evutil_socket_connect(&fd, (struct sockaddr*)&addr, sizeof(addr))) {
        if (!block && errno == EINPROGRESS) {
            return fd;
        }
        perror("net connect error!!!");
        close(fd);
        return -1;
    }

    while(1);
}

