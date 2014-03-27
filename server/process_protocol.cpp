#include <pthread.h>
#include <iostream>
#include <sys/socket.h>
#include <string>
#include <map> 
#include "./server_handler.h"
#include "./process_protocol.h"
#include "./server_handler.h"
using std::cout;
using std::pair;
using std::map;
using std::string;

void onCSLogin(p_base *base_ptr,int sockfd)
{
    p_cs_login *login_ptr=(p_cs_login*) base_ptr;
    p_sc_login_respone respone_packet;

    /* insert the login name to map,
     * so lock the mutex
     */
    pthread_mutex_lock(&map_mutex);

    string name=login_ptr->name;
    pair<map<string,int>::iterator,bool> ret=
        sock_map.insert(make_pair(name,sockfd));
    
    pthread_mutex_unlock(&map_mutex);
    /* fail to insert,
     * because there is a same name in the map,
     * so return a no-success to client
     */
    if(!ret.second){
        respone_packet.isSuccess=false;
        //send the server respone_packet
        send(sockfd, &respone_packet, respone_packet.plen, 0);
    }
    else
    {
        respone_packet.isSuccess=true;
        cout<<login_ptr->name<<" login\n";
        //send the server respone_packet
        send(sockfd, &respone_packet, respone_packet.plen, 0);
        loginHelp(login_ptr, name, sockfd);
    }
}


void onCSMesAll(p_base* base_ptr,int sockfd)
{
    p_cs_mes_all * mes_ptr=(p_cs_mes_all*) base_ptr;
    p_sc_mes_all packet;
    strncpy(packet.dest_name, mes_ptr->src_name, MAX_NAME_LEN);
    strncpy(packet.mes,mes_ptr->message,MAX_MES_LEN);
    pthread_mutex_lock(&map_mutex);
    for(map<string,int>::iterator iter=sock_map.begin();iter!=sock_map.end();iter++)
    {
        if(iter->second!=sockfd)
        {
            send(iter->second, &packet, packet.plen, 0);
        }
    }
    pthread_mutex_unlock(&map_mutex);
    cout<<"send to all\n";

}


void onCSLogout(p_base *ptr,int sockfd)
{
    p_cs_logout *logout_ptr=(p_cs_logout*)ptr;
    string name=logout_ptr->name;
    logoutHelp(name,sockfd);
}

