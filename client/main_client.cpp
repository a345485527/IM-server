#include "./client_handler.h"


 char user_name[MAX_NAME_LEN];
 int sockfd;
 struct sockaddr_in servaddr;
 struct clientVector onlineVec;
 pthread_mutex_t mutex;
 pthread_t inputNameThreadId;

typedef void (*protocolHandler)(p_base *);
 protocolHandler protocol_handler_array[P_PROTOCOL_NUM];

 int main(int argc, char **argv)
{
    init(argc,argv); 
    createInputNameThread();
    pthread_join(inputNameThreadId, NULL);
    recvPacket();
    return 0;
}
