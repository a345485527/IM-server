#include "./process_protocol.h"
#include "./client_handler.h"
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <pthread.h>
using std::cout;
using std::cin;
using std::string;

void onSCLoginResponse(p_base *ptr)
{
    p_sc_login_respone *res_prt=(p_sc_login_respone*)ptr;

    // fail to login,try to change a name
    if(res_prt->isSuccess==false)
    {
        p_cs_login login_packet;
        cout<<"please change name and try again\n";
        cout<<"enter your name here:\n";
        memset(user_name, 0 , sizeof(user_name));
        cin>>user_name;
        strncpy(login_packet.name, user_name, MAX_NAME_LEN);
        login_packet.name[MAX_NAME_LEN-1]='\0';
        send(sockfd, &login_packet, login_packet.plen, 0); 
    }
    // login successfully
    else
    {
        /* create thread to send message,
         * wait to implement this module
         */
        createListenCmdThread();
        cout<<"login successfully!\n";
    }
}

void onSCUpdateFriendList(p_base *ptr)
{
    cout<<"onSCUpdateFriendList\n";
    p_sc_update_friendList* list_ptr=(p_sc_update_friendList*) ptr;
    int onlineNum=list_ptr->size;

    pthread_mutex_lock(&mutex);
    if(onlineNum<=onlineVec.size)
    {
       memcpy(onlineVec.pOnlineClient, list_ptr->onlineClient, sizeof(struct client)*onlineNum); 
       cout<<"online client :\n";
       for(int i=0;i<onlineNum;i++)
       {
           if(onlineVec.pOnlineClient[i].isUsed==true)
           {
               cout<<onlineVec.pOnlineClient[i].name<<"\n";
           }
       }
    }
    else
    {
        /* realloc memory to hold online client
         * wait to implement
         */
    }
    pthread_mutex_unlock(&mutex);
}

void onSCNewLogin(p_base *ptr)
{
    int i;
    p_sc_new_login * nlptr=(p_sc_new_login*)ptr; 
    pthread_mutex_lock(&mutex);
    for(i=0;i<onlineVec.size;i++)
    {
        if(onlineVec.pOnlineClient[i].isUsed==false)
        {
           strncpy(onlineVec.pOnlineClient[i].name, nlptr->name, MAX_NAME_LEN);
           onlineVec.pOnlineClient[i].name[MAX_NAME_LEN-1]='\0';           
           onlineVec.pOnlineClient[i].isUsed=true;
           break;
        }
    }
    // no space to hold new client,realloc memory
    if(i==onlineVec.size)
    {
        /* realloc memory,
         * wait to implement
         */
    }
    pthread_mutex_unlock(&mutex);
    cout<<nlptr->name<<" login!\n";
}


void onSCLogout(p_base *ptr)
{
    p_sc_logout *logout_ptr=(p_sc_logout*) ptr;

    pthread_mutex_lock(&mutex);
    for(int i=0;i<onlineVec.size;i++)
    {
        //  strncmp will stop when reach to '\0' but not reach MAX_NAME_LEN
        if( strncmp(onlineVec.pOnlineClient[i].name, logout_ptr->name, MAX_NAME_LEN) ==0 )
        {
            onlineVec.pOnlineClient[i].isUsed=false;
            cout<<logout_ptr->name<<" logout!\n";
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
}

void onSCMesAll(p_base *ptr)
{
    p_sc_mes_all *mes_ptr=(p_sc_mes_all*)ptr;
    cout<<mes_ptr->dest_name<<" said: "<<mes_ptr->mes<<"\n";
}
