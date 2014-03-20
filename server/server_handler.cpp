#include <map>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "./server_handler.h"
#include "./process_protocol.h"
using std::cout;
using std::map;
using std::string;
using std::pair;
void init()
{
    listenfd=socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd<0)
    {
        cout<<"socket error\n";
        exit(-1);
    }
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(9877);
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);

    if((bind(listenfd,(struct sockaddr*) &servaddr, sizeof(servaddr)))<0)
    {
        cout<<"bind error\n";
        exit(-1);
    }
    if((listen(listenfd, 5))<0)
    {
        cout<<"listen error\n";
        exit(-1);
    }
    
    //init struct clientVector
    onlineVec.size=INIT_CLIENT_NUM;
    onlineVec.pOnlineClient=(struct client*)malloc(sizeof(struct client)*INIT_CLIENT_NUM);    
    for(int i=0;i<INIT_CLIENT_NUM;i++)
    {
        onlineVec.pOnlineClient[i].isUsed=false;
        onlineVec.pOnlineClient[i].name[0]='\0';
    }

    //init protocol handler array
    protocol_handler_array[P_C2S_LOGIN]=onCSLogin;
    protocol_handler_array[P_C2S_MES]=onCSMes;
    
    //init mutex
    pthread_mutex_init(&map_mutex, NULL);
    cout<<"server start...\n";
}

void acceptClient()
{
    char buff[1024];
    pthread_t tid;
    socklen_t len;
    while(1)
    {
        len=sizeof(cliaddr);
        sockfd=accept(listenfd, (struct sockaddr*)&cliaddr, &len);
        cout<<"connection from" <<inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff))
            <<"port is " << ntohs(cliaddr.sin_port);

        //if the size of onlineVec full,realloc it and init new space
        pthread_mutex_lock(&map_mutex);
        if(sock_map.size()==onlineVec.size)
        {
            onlineVec.size+=10;
            onlineVec.pOnlineClient=(struct client*)realloc(onlineVec.pOnlineClient, sizeof(struct client*)*onlineVec.size);
            for(int i=sock_map.size();i<onlineVec.size;i++)
            {
                onlineVec.pOnlineClient[i].isUsed=false;
                onlineVec.pOnlineClient[i].name[0]='\0';
            }
        }
        pthread_mutex_lock(&map_mutex);

        pthread_create(&tid, NULL, creatLogin, (void *)sockfd);
    }
}

void* creatLogin(void *arg)
{
    pthread_detach(pthread_self());
    int sockfd=(int)arg;
    int len;
    char buf[400];
    while(1)
    {
        if((len=recv(sockfd, buf, sizeof(buf), 0))>0)
        {
            p_base *ptr=(p_base*)buf;
            protocol_handler_array[ptr->pname](ptr,sockfd);
        }
        // connection closed by client,
        else if(len==0)
        {
            logoutHelp(sockfd);
            break;
        }
    }
    close(sockfd);
    pthread_exit(NULL);
}


void createListenCmd()
{
    pthread_t tid;
    pthread_create(&tid, NULL, listenCmd, (int*)listenfd);
}

void * listenCmd(void* arg)
{
    char cmd[10];
    int sockfd=(int)arg;
    while(1)
    {
        read(STDIN_FILENO, cmd, sizeof(cmd));
        if((strncmp(cmd,"exit",4))==0)
        {
            free(onlineVec.pOnlineClient);
            close(sockfd);
            // all threads will be exit,close their own socked auto
            exit(0);
        }
    }
}
void loginHelp(p_cs_login* login_ptr,string name,int sockfd)
{

    pthread_mutex_lock(&map_mutex);

        for(int i=0;i<onlineVec.size;i++)
        {
            //this space is not used,put the client name here
           if(onlineVec.pOnlineClient[i].isUsed==false)
           {
               strncpy(onlineVec.pOnlineClient[i].name,login_ptr->name , MAX_NAME_LEN);
               onlineVec.pOnlineClient[i].name[MAX_NAME_LEN-1]='\0';
           }
        }

    p_sc_update_friendList *update_packet_ptr;
    update_packet_ptr=(struct p_sc_update_friendList*)
        malloc(sizeof(struct p_sc_update_friendList)+sizeof(client)*onlineVec.size);
    update_packet_ptr->plen=sizeof(struct p_sc_update_friendList)+
        sizeof(client)*onlineVec.size;
    update_packet_ptr->size=onlineVec.size;
    update_packet_ptr->pname=P_S2C_UPDATE_FRIENDLIST;
    memcpy(update_packet_ptr->onlineClient, onlineVec.pOnlineClient,
            sizeof(client)*onlineVec.size);

    // send the online client to the new login client;
    send(sockfd, update_packet_ptr, update_packet_ptr->plen, 0);
    free(update_packet_ptr);

    // send the new login client to the online client
    p_sc_new_login new_login_packet;
    for(map<string,int>::iterator iter=sock_map.begin();iter!=sock_map.end();iter++)
    {
        if(iter->second!=sockfd)
        {
            strncpy(new_login_packet.name, name.c_str(), name.size());
            new_login_packet.name[name.size()]='\0';
            new_login_packet.name[MAX_NAME_LEN-1]='\0';
            send(iter->second, &new_login_packet, new_login_packet.plen, 0);
        }
    }
    pthread_mutex_unlock(&map_mutex);
}


void logoutHelp(int sockfd)
{
    string name;
    p_sc_logout logout_packet;
    pthread_mutex_lock(&map_mutex);

    // find the closed client socket,and delete it from map,and tell other online client
    for(map<string,int>::iterator iter=sock_map.begin();iter!=sock_map.end();iter++)
    {
        if(iter->second==sockfd)
        {
            name=iter->first;
            sock_map.erase(iter);
            break;
        }
        else
        {
            strncpy(logout_packet.name, (iter->first).c_str(), (iter->first).size());
            logout_packet.name[(iter->first).size()]='\0';
            logout_packet.name[MAX_NAME_LEN-1]='\0';
            send(iter->second, &logout_packet, logout_packet.plen, 0);
        }
    }

    // delete it from onlineVec
    for(int i=0;i<onlineVec.size;i++)
    {
        if( (strncmp( name.c_str(), onlineVec.pOnlineClient[i].name, name.size() ) )==0 )        
        {
            onlineVec.pOnlineClient[i].isUsed=false;
            break;
        }
    }
    pthread_mutex_unlock(&map_mutex);
    cout<<name<<" logout\n";
}

