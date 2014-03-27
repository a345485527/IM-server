/*
 * The datagram transport protocol
 */
#include <cstring>

#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MAX_NAME_LEN 15
#define INIT_CLIENT_NUM 10
#define MAX_MES_LEN 40
#define MAX_PACKET_LEN 200

// store online  name
struct client{
    bool isUsed;
    char name[MAX_NAME_LEN];
};
struct clientVector{
    int size;
    struct client* pOnlineClient;
};
enum{
    /*from client to server*/
    P_C2S_LOGIN,
    P_C2S_MES_ALL,
    P_C2S_LOGOUT,
    

    /*from server to client*/
    P_S2C_LOGIN_RESPONE,
    P_S2C_UPDATE_FRIENDLIST,
    P_S2C_NEW_LOGIN,
    P_S2C_LOGOUT,
    P_S2C_MES_ALL,

    /*total protocol*/
    P_PROTOCOL_NUM
};

/*base protocol*/
struct p_base{
    p_base(int name,int len):pname(name),plen(len)
    {
    }
    int pname;
    int plen;
};

/* client to server */
struct p_cs_login:public p_base{
   p_cs_login():p_base(P_C2S_LOGIN,sizeof(p_cs_login))
    {
       memset(name, 0  , sizeof(name)); 
    }
   char name[MAX_NAME_LEN];
};
struct p_cs_logout:public p_base{
    p_cs_logout():p_base(P_C2S_LOGOUT,sizeof(p_cs_logout))
    {
        memset(name,0,sizeof(name));
    }
    char name[MAX_NAME_LEN];
};
struct p_cs_mes_all:public p_base{
    p_cs_mes_all():p_base(P_C2S_MES_ALL,sizeof(p_cs_mes_all))
    {
        memset(src_name, 0,sizeof(src_name));
        memset(message, 0, sizeof(message));
    }
    char src_name[MAX_NAME_LEN];
    char message[MAX_MES_LEN];
};

/* server to client */

struct p_sc_login_respone:public p_base{
    p_sc_login_respone():p_base(P_S2C_LOGIN_RESPONE,sizeof(p_sc_login_respone))
    {
    }
    bool isSuccess;
};

struct p_sc_update_friendList:public p_base{
    p_sc_update_friendList():p_base(P_S2C_UPDATE_FRIENDLIST,sizeof(p_sc_update_friendList))
    {
    }
    int size;
    struct client onlineClient[];
};
struct p_sc_new_login:public p_base{
    p_sc_new_login():p_base(P_S2C_NEW_LOGIN,sizeof(p_sc_new_login))
    {
       memset(name, 0  , sizeof(name)); 
    }
    char name[MAX_NAME_LEN];
};
struct p_sc_logout:public p_base{
    p_sc_logout():p_base(P_S2C_LOGOUT,sizeof(p_sc_logout))
    {
       memset(name, 0  , sizeof(name)); 
    }
    char name[MAX_NAME_LEN];
};
struct p_sc_mes_all:public p_base{
    p_sc_mes_all():p_base(P_S2C_MES_ALL,sizeof(p_sc_mes_all))
    {
        memset(dest_name, 0,sizeof(dest_name));
        memset(mes, 0, sizeof(mes));
    }
    char dest_name[MAX_NAME_LEN];
    char mes[MAX_MES_LEN];
};

#endif
