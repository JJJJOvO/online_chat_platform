#include "server.h"


// 实现同步
std::mutex mutexWait;                 // 互斥锁
std::condition_variable condWait;     // 条件变量


// !!! 静态成员变量只能在类的外部初始化
ChatDatabase* Server::ChatDB = new ChatDatabase;
ChatInfo* Server::chat_info_list = new ChatInfo;
// 服务器类构造函数中完成监听客户端连接的准备工作
Server::Server(const char *ip, int port)
{
    // 创建事件集合
    base = event_base_new();

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);
        
    // 创建监听对象
    listener = evconnlistener_new_bind(base, listen_cb, NULL,
            LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, 10, 
            (struct sockaddr*)&server_addr, sizeof(server_addr) 
            );
    if(listener == NULL)
    {
        // 打印错误原因，并退出程序
        syserr("evconnlistener_new_bind error");
    }
    else{
        std :: cout << "-----等待客户端连接-----"<<std :: endl;
        event_base_dispatch(base); // 监听集合
    }
}

Server::~Server(){
    // 创建时已设置自动free，可免去evconnlistener_free(listener);
    event_base_free(base);
}


inline void Server::send_msg(struct bufferevent *bev, const char *cmd, const char *result)
{
    Json::Value msg;
    msg["cmd"] = cmd;
    msg["result"] = result;

    Json::FastWriter writer;
    const char *reply_msg = writer.write(msg).c_str();
    // cout << reply_msg;
    if(bufferevent_write(bev, reply_msg, strlen(reply_msg)) < 0)
    {
        cout << cmd << " bufferevent_write error"<<endl;
    }
    else
    {
        this_thread::sleep_for(chrono::milliseconds(1));
    }
}

void Server::send_msg(int count, ...)
{
    Json::Value msg;
    va_list args;
    va_start(args, count);

    struct bufferevent *bev = va_arg(args, struct bufferevent*);

    const char *cmd = va_arg(args, char*);
    msg["cmd"] = cmd;
    
    const char *res = va_arg(args, char*);
    msg["result"] = res;

    int n = (count - 3)/2;
    for(int i=0; i < n; i++)
    {
        msg[va_arg(args, char*)] = va_arg(args, char*);
    }

    Json::FastWriter writer;
    const char *reply_msg = writer.write(msg).c_str();
    // cout << reply_msg;
    if(bufferevent_write(bev, reply_msg, strlen(reply_msg)) < 0)
    {
        cout << cmd << " bufferevent_write error"<<endl;
    }
    else
    {
        this_thread::sleep_for(chrono::milliseconds(1));
    }

    va_end(args);

    cout << cmd << " " << res<<endl;
}

// 客户端完成连接后，内核自动调用
void Server::listen_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr* addr, int  socklen, void *arg )
{
    cout << " 接受客户端连接，fd = "<<fd<<endl;
    
    // 创建工作线程 来处理该客户端
    // thread trd1(Fn &&fn, Args &&args) 
    thread  client_thread(client_handler, fd);
    client_thread.detach(); // 线程分离，线程任务完成后系统自动回收
       
}

void Server::client_handler(int fd)
{
    // 创建集合
    struct event_base *base = event_base_new();
    // 创建 bufferevent，参1：监听事件集合，参2：与客户端连接的fd，参3：设置自动回收 
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if(bev == NULL)
    {
        syserr("bufferevent_socket_new error");
    }
    // 给bufferevent 设置回调函数，第一个 读回调（默认关），第二个写回调（默认开），第三个异常发生回调函数
    bufferevent_setcb(bev, read_cb, NULL, event_cb, NULL);
    bufferevent_enable(bev, EV_READ);
    
    event_base_dispatch(base); // 监听集合（监听客户端是否发送数据）
    event_base_free(base);
}

void Server::read_cb(struct bufferevent *bev, void *ctx)
{
    char buf[1024]={0};
    int size = bufferevent_read(bev, buf, sizeof(buf));
    if(size<0)
    {
        syserr("bufferevent_read error");
    }

    // read_cb buf 测试
    string str(buf);
    cout <<str<<endl;

    // 读取客户端发送的json数据后，解析并分装
    Json::Reader reader; // 解析 json 对象

    Json::Value val; // json 对象

    // 1. 解析

    // 把 数据流 转换为 json 对象
    if(!reader.parse(buf, val))
    {
        cout << "服务器解析数据失败" <<endl;
    }

    string cmd = val["cmd"].asString(); // val["cmd"]---json对象，asString()---转换为string对象

    if(cmd == "register")
    {   
        // 静态成员函数内不能调用普通成员函数
        server_register(bev, val); 
    }
    else if(cmd == "login")
    {
        server_login(bev, val);
    }
    else if(cmd == "add_friend")
    {
        server_add_friend(bev, val);
    }
    else if(cmd == "add_group")
    {
        server_add_group_and_members(bev, val);
    }
    else if(cmd == "invite_friends")
    {
        server_invite_group_members(bev, val);
    }
    else if(cmd == "private_chat")
    {
        server_private_chat(bev, val);
    }
    else if(cmd == "get_members")
    {
        server_get_group_members(bev, val);
    }
    else if(cmd == "group_chat")
    {
        server_group_chat(bev, val);
    }
    else if(cmd == "off_line")
    {
        server_offline(bev, val);
    }
    else if(cmd == "send_file")
    {
        server_send_file(bev, val);
    }
}

void Server::event_cb(struct bufferevent *bev, short what, void *ctx)
{

}

void Server::server_register(struct bufferevent *bev, Json::Value val)
{
    ChatDB -> my_database_connect("chatuser");
    bool flag = ChatDB -> my_database_user_is_exist(val["user"].asCString());
    //cout << flag <<endl;
    if(flag)
    {
        // 用户名 已被注册
        send_msg(bev, "register_reply", "failure");
    }
    else
    {
        // 用户名未被注册, 添加进数据库 
        ChatDB -> my_database_add_user(val["user"].asCString(), val["password"].asCString());

        send_msg(bev, "register_reply", "success");
    }
    ChatDB -> my_database_disconnect();
}

void Server::server_login(struct bufferevent *bev, Json::Value val)
{
    // 查看用户是否在线
    if(chat_info_list -> is_online_user(val["user"].asString()) != nullptr)
    {
        send_msg(bev, "login_reply", "is_online");
        return ;
    }

    ChatDB -> my_database_connect("chatuser");

    if(ChatDB -> my_database_user_is_exist(val["user"].asCString()))
    {   
        const char *user = val["user"].asCString();
        // 查看用户是否存在
        if(ChatDB -> my_database_user_password_is_right(user, val["password"].asCString()))
        {
            // 查看密码是否正确

            // 1. 登陆成功，将该用户挂上 在线用户链表 以及 在线用户集合
            User u;
            u.name = string(user);
            u.bev = bev;
            chat_info_list -> insert_online_user_information(&u);
            
            // 2. 返回客户端，好友列表成员，以及该用户的群信息
            string user_friends="";
            ChatDB -> my_database_get_user_friend(user, user_friends);

            string user_groups="";
            ChatDB -> my_database_get_user_group(user, user_groups);
            
            send_msg(7, bev, "login_reply", "success", user_friends.c_str(), "friend", user_groups.c_str(), "group");
        
            // 3. 提醒好友列表成员，该 user 已上线
            unsigned int start = 0, end = 0;
            bool flag = true;
            while(flag)
            {
                end = user_friends.find('|', start);
                if(end == -1)
                {  
                    string f = user_friends.substr(start, user_friends.size() - start);
                    struct bufferevent *f_bev = chat_info_list->is_online_user(f);

                    if(f_bev != nullptr)
                    {
                        // 该好友在线
                        send_msg(f_bev, "online_reminder", user);
                    }
                    flag = false;
                }
                else
                {
                    string f = user_friends.substr(start, end - start);
                    struct bufferevent *f_bev = chat_info_list->is_online_user(f);

                    if(f_bev != nullptr)
                    {
                        // 该好友在线
                        send_msg(f_bev, "online_reminder", user);
                    }
                    
                    start = end + 1;
                }
            }
        }
        else
        {
            // 密码不正确，回复客户端 密码错误
            send_msg(bev, "login_reply", "password_is_not_right");
        }
        
    }
    else
    {
        // 用户不存在，返回用户不存在
        send_msg(bev, "login_reply", "user_is_not_exist");
    }

    ChatDB -> my_database_disconnect();
}

void Server::server_add_friend(struct  bufferevent *bev, Json::Value val)
{
    ChatDB -> my_database_connect("chatuser");
    const char *tar_friend = val["tar_friend"].asCString();
    const char *user = val["user"].asCString();

    if(ChatDB -> my_database_user_is_exist(tar_friend))
    {
        // --- 判断目标好友是否存在
        string my_friends = "";
        ChatDB -> my_database_get_user_friend(user, my_friends);
        if(my_friends.find(tar_friend) == -1)
        {
            // 修改双方数据库，互添加好友
            ChatDB -> my_database_add_user_friend(user, tar_friend);
            ChatDB -> my_database_add_user_friend(tar_friend, user);

            // 发送添加好友成功
            send_msg(5, bev, "add_friend_reply", "success", tar_friend, "friend");

            struct bufferevent * f_bev = chat_info_list -> is_online_user(string(tar_friend));
            if(f_bev != nullptr)
            {
                send_msg(5, f_bev, "add_friend_reply", "success", user, "friend");
            }
        }
        else
        {
            // 目标好友已经是自己的好友
            send_msg(bev, "add_friend_reply", "be_friends");
        }
    }
    else
    {
        // 目标好友不存在
        send_msg(bev, "add_friend_reply", "no_friend");
    }

    ChatDB -> my_database_disconnect();
}

void Server::server_add_group_and_members(struct bufferevent *bev, Json::Value val)
{
    // 创建新群 且 邀请成员
    ChatDB -> my_database_connect("chatgroup");

    const string group_name = val["group"].asString();
    bool flag = chat_info_list -> info_group_is_exist(group_name);
    if(flag)
    {
        // group 已经存在
        send_msg(bev, "create_group_reply", "failure");

        ChatDB -> my_database_disconnect();
    }
    else
    {
        // group 不存在
        const string owner = val["owner"].asString();
        
        ChatDB -> my_database_add_group(group_name.c_str(), owner.c_str());
        chat_info_list -> insert_group_information(group_name);

        Json::Value msg;
        msg["group"] = val["group"];
        msg["members"] = owner + '|' + val["members"].asString();
        msg["inviter"] = val["inviter"];
         

        ChatDB -> my_database_disconnect();

        server_invite_group_members(bev, msg);
        
        // send_msg(5, bev, "create_group_reply", "success", group_name.c_str(), "group");

    }
   
}

void Server::server_invite_group_members(struct bufferevent *bev, Json::Value val)
{
    const char *group_name = val["group"].asCString();
    if(!chat_info_list->info_group_is_exist(string(group_name)))
    {
        send_msg(bev, "add_group_reply", "group_is_not_exist");
        return;
    }
    // 邀请好友加入群
    // 修改群成员字段
    ChatDB -> my_database_connect("chatgroup");

    const char *members = val["members"].asCString();
    string exist_members = "";
    string new_members = "";
    chat_info_list -> insert_group_members_information(group_name, members, new_members, exist_members);
    ChatDB -> my_database_add_group_members(group_name, new_members.c_str());

    ChatDB -> my_database_disconnect();

    // 修改 用户 群信息字段
    ChatDB -> my_database_connect("chatuser");

    ChatDB -> my_database_add_users_group(new_members, group_name);

    if(!exist_members.empty())
    {
        send_msg(7, bev, "invite_reply", "failure", group_name, "group", exist_members.c_str(), "exist_members");
    }
    

    if(!new_members.empty())
    {
        unsigned int start = 0, end = 0;
        bool flag = true;
        while(flag)
        {
            end = new_members.find('|', start);
            if(end == -1)
            {
                struct bufferevent *f_bev = chat_info_list -> is_online_user(new_members.substr(start, new_members.size() - start));

                if(f_bev != nullptr)
                {
                    string inviter = val["inviter"].asString();
                    send_msg(7, f_bev, "invite_reply", "success", group_name, "group", inviter.c_str(), "inviter");
                }
                flag = false;
            }
            else
            {
                struct bufferevent *f_bev = chat_info_list -> is_online_user(new_members.substr(start, end - start));

                if(f_bev != nullptr)
                {
                    string inviter = val["inviter"].asString();
                    send_msg(7, f_bev, "invite_reply", "success", group_name, "group", inviter.c_str(), "inviter");
                }
                start = end + 1;
            }
        }
    }

    ChatDB -> my_database_disconnect();
}

void Server::server_private_chat(struct bufferevent *bev, Json::Value val)
{
    const string &to_user = val["to_user"].asString();
    const string &from_user = val["from_user"].asString();

    struct bufferevent *f_bev = chat_info_list -> is_online_user(to_user);

    if(f_bev == nullptr)
    {
        send_msg(5, bev, "private_chat", "off_line", to_user.c_str(), "user");
    }
    else{
        const char *test = val["test"].asCString();
        send_msg(5, bev, "private_chat", "send_reply", test, "test");

        send_msg(7, f_bev, "private_chat", "accept", from_user.c_str(), "from_user", test, "test");
    }
}

void Server::server_get_group_members(struct bufferevent *bev, Json::Value val)
{
    const string &group_name = val["group"].asString();

    unordered_set<string> *my_set = chat_info_list -> get_group_members(group_name);

    auto iter = my_set -> begin();

    string m = *iter;
    iter++;
    while(iter != my_set -> end())
    {
        m += '|' + *iter;
        iter++;
    }

    send_msg(5, bev, "get_members_reply", group_name.c_str(), m.c_str(), "members");
}

void Server::server_group_chat(struct bufferevent *bev, Json::Value val)
{
    const string &group_name = val["group"].asString();
    const char *text = val["text"].asCString();
    const char *members = val["members"].asCString();
    const char *sender = val["sender"].asCString();

    unordered_set<string> *my_set = chat_info_list -> get_group_members(group_name);

    for(auto iter = my_set->begin(); iter != my_set->end(); iter++)
    {
        if(*iter != string(sender))
        {
            const string &name = *iter;
            
            struct bufferevent* m_bev = chat_info_list -> is_online_user(name);

            if(m_bev != nullptr)
            {
                send_msg(11, m_bev, "group_chat_reply", "accept", text, "text", group_name.c_str(), "group", members, "members", sender, "sender");
            }
        }
    }
    
    send_msg(11, bev, "group_chat_reply", "send_reply", group_name.c_str(), "group", text, "text", members, "members", sender, "sender");

}

void Server::server_offline(struct bufferevent *bev, Json::Value val)
{
    ChatDB -> my_database_connect("chatuser");
    
    const char *user = val["user"].asCString();

    string friends = "";

    ChatDB -> my_database_get_user_friend(user, friends);
    if(!friends.empty())
    {
        unsigned int end = 0 , start = 0;
        bool flag = true;
        while(flag)
        {
            end = friends.find('|', start);
            if(end == -1)
            {
                struct bufferevent *f_bev = chat_info_list -> is_online_user(friends.substr(start, friends.size() - start));

                if(f_bev != nullptr)
                {
                    send_msg(f_bev, "offline_reminder", user);
                }
                flag = false;
            }
            else
            {
                struct bufferevent *f_bev = chat_info_list -> is_online_user(friends.substr(start, end - start));

                if(f_bev != nullptr)
                {
                    send_msg(f_bev, "offline_reminder", user);
                }
                start = end + 1;
            }
        }
    }

    chat_info_list -> user_offline(string(user));

    bufferevent_free(bev);
    ChatDB -> my_database_disconnect();
}


void Server::send_file_handler(const size_t length, const int file_port, int &from_fd, int &to_fd)
{
    string buff;
    char buf[4096];
    ssize_t size = 0;
    size_t sum = 0;
   
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(sfd == -1)
    {
        return ;
    }

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    bind(sfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(sfd, 10);

    struct sockaddr_in client_addr;
    socklen_t flen = sizeof(client_addr);
    // 接受发送端的连接请求
    from_fd = accept(sfd, (struct sockaddr*)&client_addr, &flen);
    bool flag = true;
    while(flag)
    {
        size = recv(from_fd, buf, sizeof(buf), 0);
        if(size == -1)
        {
            cout << size << " " << errno<<endl;
            return;
        }
        sum =+ size;
        buff.append(buf);
        if(sum >= length) flag= false;
        else memset(buf, 0, strlen(buf));
    }
    cout << buff <<endl;
    // 接受接受端的连接请求
    to_fd = accept(sfd, (struct sockaddr*)&client_addr, &flen);
    send(to_fd, buff.c_str(), sizeof(buff.c_str()), 0);

    close (from_fd);
    close (to_fd);
    close (sfd);
}

void Server::server_send_file(struct bufferevent *bev, Json::Value val)
{
    const string user = val["to_user"].asString();

    struct bufferevent *f_bev = chat_info_list -> is_online_user(user);

    if(f_bev == nullptr)
    {
        send_msg(bev, "send_file_reply", "offline");
        return;
    }
    else
    {
        int port = 8080, from_fd = 0, to_fd = 0;
        size_t length = val["length"].asUInt();

        // 传递 线程方法 引用，需加上ref，显示传递
        thread send_file_thread(send_file_handler, length, port, ref(from_fd), ref(to_fd));

        send_file_thread.detach();

        Json::Value msg;
        msg.clear();
        msg["cmd"] = "send_file_port_reply";
        msg["port"] = "8080";
        msg["filename"] = val["filename"];
        msg["length"] = val["length"];
        Json::FastWriter writer;
        const char *s = writer.write(msg).c_str();

        if(send(bev->ev_read.ev_fd, s, strlen(s), 0) <= 0)
        {
            cout << "send error"<<endl; 
        }

        int count = 0;
        while(from_fd <= 0)
        {
            this_thread::sleep_for(chrono::milliseconds(1000));
            count++;
            if(count == 10)
            {
                send_msg(bev, "send_file_reply", "outtime");
                pthread_cancel(send_file_thread.native_handle());
                return;
            }
        }

        msg.clear();
        
        msg["cmd"] = "recv_file_port_reply";
        msg["port"] = "8080";
        msg["filename"] = val["filename"];
        msg["length"] = val["length"];

        const char *s1 = writer.write(msg).c_str();

        if(send(f_bev->ev_read.ev_fd, s1, strlen(s1), 0) <= 0)
        {
            cout << "(2)send error" <<endl;
        }
        
        count = 0;
        while(from_fd <= 0)
        {
            this_thread::sleep_for(chrono::milliseconds(1000));
            count++;
            if(count == 10)
            {
                send_msg(bev, "send_file_reply", "outtime");
                pthread_cancel(send_file_thread.native_handle());
                return;
            }
        }


    }
}
