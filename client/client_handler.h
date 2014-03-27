#include "./protocol.h"
#include <string>
#include <pthread.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#define bzero(a,b) memset(a,0,b)

extern char user_name[MAX_NAME_LEN];
extern int sockfd;
extern struct sockaddr_in servaddr;
extern struct clientVector onlineVec;
extern pthread_mutex_t mutex;
extern pthread_t inputNameThreadId;

typedef void (*protocolHandler)(p_base*);
extern protocolHandler protocol_handler_array[P_PROTOCOL_NUM];

void init(int argc,char **argv);
void createInputNameThread();
void * inputName(void*);
void createListenCmdThread();
void * listenCmd(void*);

// receive packet from server
void recvPacket(); 
// user inputs message
void inputMes(std::string);
