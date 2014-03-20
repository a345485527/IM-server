#include "./protocol.h"
#include <map>
#include <string>
#define bzero(a,b) memset(a,0,b)

//store online name
extern struct clientVector onlineVec;
extern std::map<std::string,int> sock_map;
extern pthread_mutex_t map_mutex;

extern int listenfd;
extern int sockfd;
extern struct sockaddr_in servaddr;
extern struct sockaddr_in cliaddr;

/* function pointer array */
typedef void (*protocolHandler)(p_base*,int);
extern protocolHandler protocol_handler_array[P_PROTOCOL_NUM];

/* init global variable  */
void init();
/* accept new connection */
void acceptClient();
void *creatLogin(void *);
/* listen server command, only "exit" will be accepted  */
void createListenCmd();
void *listenCmd(void *);

/* add new client to clientVector,and update friend-list  */
void loginHelp(p_cs_login*,std::string,int);
/* delete the logout client socket  */
void logoutHelp(int);
