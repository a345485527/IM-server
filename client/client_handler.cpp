#include "./client_handler.h"
#include "./process_protocol.h"
#include "./protocol.h"
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
using std::cout;
using std::cin;
using std::string;
using std::istringstream;
using std::getline;

void init(int argc,char **argv)
{
    int n;
    if(argc!=2)
    {
        cout<<"usage : client <IPadress>\n";
        exit(-1);
    }
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0)
    {
        cout<<"socket error!\n";
        exit(-1);
    }
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(9877);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    
    n=connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(n<0)
    {
        cout<<"connect error\n";
        exit(-1);
    }
    
    memset(user_name, 0, MAX_NAME_LEN);
    pthread_mutex_init(&mutex, NULL);
    
    //init struct clientVector
    onlineVec.size=INIT_CLIENT_NUM;
    onlineVec.pOnlineClient=(struct client*)malloc(sizeof(struct client)*INIT_CLIENT_NUM);    
    for(int i=0;i<INIT_CLIENT_NUM;i++)
    {
        onlineVec.pOnlineClient[i].isUsed=false;
        onlineVec.pOnlineClient[i].name[0]='\0';
    }

    protocol_handler_array[P_S2C_LOGIN_RESPONE]=onSCLoginResponse;
    protocol_handler_array[P_S2C_UPDATE_FRIENDLIST]=onSCUpdateFriendList;
    protocol_handler_array[P_S2C_NEW_LOGIN]=onSCNewLogin;
    protocol_handler_array[P_S2C_LOGOUT]=onSCLogout;
    protocol_handler_array[P_S2C_MES_ALL]=onSCMesAll;
}


void createInputNameThread()
{
    pthread_t tid;
    pthread_create(&inputNameThreadId, NULL, inputName, NULL);
}

void * inputName(void *arg)
{
    p_cs_login packet;

    cout<<"enter your name:\n";
    cin>>user_name;
    user_name[MAX_NAME_LEN-1]='\0';
    
    strncpy(packet.name, user_name, MAX_NAME_LEN);
    send(sockfd, &packet, packet.plen, 0); 

    pthread_exit((void*)0);
}


void createListenCmdThread()
{
    pthread_t tid;
    pthread_create(&tid, NULL, listenCmd, NULL);
}

void * listenCmd(void* arg)
{
    pthread_detach(pthread_self());
    string cmd,line;
    while(1)
    {
        getline(cin, line);
        istringstream stream(line);
        while(stream>>cmd)
        {
            // exit command
            if(cmd=="-exit")
            {
                 p_cs_logout logout_packet;
                 strncpy(logout_packet.name, user_name, sizeof(user_name));
                 logout_packet.name[MAX_NAME_LEN-1]='\0';
                 send(sockfd, &logout_packet, logout_packet.plen, 0);
                 exit(0);
            } 
            // list online client command
            else if(cmd=="-ls")
            {
                for(int i=0;i<onlineVec.size;i++)
                {
                    if(onlineVec.pOnlineClient[i].isUsed==true)
                    {
                        cout<<onlineVec.pOnlineClient[i].name<<"\n";
                    }
                }
                break;
            }
            // send message command
            else if(cmd=="-sendto")
            {
                // the dest client
                string dest;
                stream>>dest;
                inputMes(dest);
                cin.clear();
                break;
            }
            else
            {
                cout<<"error command!\n";
                break;
            }
        }
    }
        
    pthread_exit((void*)0);
}

void recvPacket()
{
    int n;
    char buf[MAX_PACKET_LEN]={0};
    while(1)
    {
       if((n=recv(sockfd, buf, sizeof(buf), 0))>0)
       {
           p_base *ptr=(p_base*)buf;
           protocol_handler_array[ptr->pname](ptr);
       }
       else
       {
           pthread_mutex_destroy(&mutex);
           free(onlineVec.pOnlineClient);
           close(sockfd);
           break;
       }
    }
}


void inputMes(string dest)
{
    char mes[MAX_MES_LEN];
    // send to all client
    if(dest=="all")
    {
        cout<<"enter here (send to all): \n";
        p_cs_mes_all packet;
        strncpy(packet.src_name, user_name, sizeof(user_name));
        int n=read(STDIN_FILENO, mes, sizeof(mes)-1);
        mes[n]='\0';
        mes[MAX_MES_LEN-1]='\0';
        strncpy(packet.message, mes, MAX_MES_LEN);
        send(sockfd, &packet, packet.plen, 0);
    }
}
