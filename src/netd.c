struct event_base *base;

void socket_init(void)
{
    

}


void netd_init(void)
{
    config_init();
    log_init();
    socket_init();
    signal_init();

}

void netd_start(void)
{


}

