//TODO:网路socket接口封装

int socket_create()
{
    int fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        log_err("socket creation failed\n");
        return -1;
    }
   
    //设置端口重用
    //服务器重启后能马上起来
    ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &v_on, sizeof (v_on));
    if (ret != 0) {
        log_perror("UDP-UNI-SOCKET:setsockopt(SO_REUSEADDR)");
        goto out_close_socket;
    }

     

}
