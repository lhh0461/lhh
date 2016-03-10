enum transport 
{
    RPC_CLIENT_GATED = 1, 
    RPC_GATED_GAMED, 
    RPC_GAMED_GATED, 
    RPC_GATED_CLIENT, 
};

#define AUTH_ERROR      (-1)
#define AUTH_NONE       (0)
#define AUTH_CLIENT     (1) 
#define AUTH_NETD       (2) 
#define AUTH_GAMED      (3) 
#define AUTH_DBD        (4) 

typedef struct auth_packet_s
{
    int cmd;
    int len;
} auth_packet_t;

typedef struct rpc_packet_s
{
    //协议ID
    int pid;
    //传递方向
    enum transport way; 
    //长度
    int len;
} rpc_packet_t;

