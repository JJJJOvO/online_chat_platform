#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdarg>
#include <condition_variable>

#include <unistd.h>
#include <string.h>
#include <event.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event2/listener.h> 
#include <stdlib.h>
#include <stdio.h>
#include <jsoncpp/json/json.h>

#include "chatinfo.h"
#include "chat_database.h"

#define IP "172.29.211.184"
#define PORT 8000

using namespace std;

class Server
{
private:
    struct event_base *base; // 事件集合
    struct evconnlistener *listener; // 监听事件 
    static ChatInfo *chat_info_list;  // 含有两个链表（群信息 和 在线用户信息 [用户名，fd] ）   
    static ChatDatabase *ChatDB;

private:
    static void syserr(const char *str)
    {
        // perror(str);  
        cout <<str<<endl;
        exit(1);
    }
    // 客户端连接成功后，服务端的回调函数
    static void send_msg(int count, ...);
    static void send_msg(struct bufferevent*, const char*, const char*);
    static void listen_cb(struct evconnlistener *listener, evutil_socket_t fd, 
                        struct sockaddr *addr, int socklen, void *arg);
    static void client_handler(int fd);
    static void send_file_handler(const size_t, const int, int&, int&);
    static void read_cb(struct bufferevent *bev, void *ctx);
    static void event_cb(struct bufferevent *bev, short what, void *ctx);

    // 功能函数
    static void server_register(struct bufferevent *bev, Json::Value val);
    static void server_login(struct bufferevent *bev, Json::Value val);
    static void server_add_friend(struct bufferevent *bev, Json::Value val);
    //static void server_add_group(struct bufferevent *bev, Json::Value val);
    static void server_add_group_and_members(struct bufferevent *bev, Json::Value val);
    static void server_invite_group_members(struct bufferevent *bev, Json::Value val);
    static void server_private_chat(struct bufferevent *bev, Json::Value val);
    static void server_get_group_members(struct bufferevent *bev, Json::Value val);
    static void server_group_chat(struct bufferevent *bev, Json::Value val);
    static void server_offline(struct bufferevent *bev, Json::Value val);
    static void server_send_file(struct bufferevent *bev, Json::Value val);
public:
    Server(const char *ip="127.0.0.1", int port=8000);
    ~Server();       
};
#endif
