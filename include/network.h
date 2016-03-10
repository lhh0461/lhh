#include <event2/event.h>
#include <event2/util.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

typedef struct net_connection_s 
{
    int fd;
    int auth;
    struct bufferevent *bev;
    void *udata;
    
} net_connection_t;

struct sockaddr_in;
int net_convert_addr(const char*ip, unsigned short port, struct sockaddr_in *addr);
int net_set_block(int fd, int *rflag);
int net_set_nonblock(int fd);
int net_connect(const char *ip, unsigned short port, int block);
net_connection_t *net_new_connection(int fd, bufferevent_data_cb conn_read_cb, bufferevent_data_cb conn_write_cb, bufferevent_event_cb conn_error_cb);
int net_send_packet(net_connection_t *conn, void *data, int datalen);
