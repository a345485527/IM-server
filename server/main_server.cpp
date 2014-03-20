#include "./server_handler.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <cstdlib>


//store online name
struct clientVector onlineVec;
std::map<std::string,int> sock_map;
pthread_mutex_t map_mutex;

int listenfd;
int sockfd;
struct sockaddr_in servaddr;
struct sockaddr_in cliaddr;

/* function pointer array */
typedef void (*protocolHandler)(p_base*,int);
protocolHandler protocol_handler_array[P_PROTOCOL_NUM];

int main()
{
    init();
    createListenCmd();
    acceptClient();
    
    exit(0);
}
