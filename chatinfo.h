#ifndef CHATINFO_H
#define CHATINFO_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <list>

#include <event.h>
#include "chat_database.h"

#define MAXNUM 1024 // 表示群的最大个数

using namespace std;

struct User
{
    string name="";
    //string group="";
    struct bufferevent *bev=nullptr;  

    bool operator == (const User &u) const
    {
        return this->name == u.name;
    }
    bool operator == (const string &s) const
    {
        return this->name == s;
    }
};
typedef struct User User;

struct GroupMember
{
    string name="";
};
typedef struct GroupMember GroupMember;

struct Group
{
    string name="";
    list<GroupMember> *group_menber_info=nullptr; // 存储群成员信息
};
typedef struct Group Group;

class ChatInfo
{
private:
    // 存储在线用户
    unordered_map<string, struct bufferevent*> *to_find_online_users;
    unordered_map<string, unordered_set<string>> *to_find_exist_group_members;

    // list<User> *users;
    // list<Group> *group_info; // 存储群组信息（链表） 
    ChatDatabase *mydatabase;    // 数据库对象

public:
    ChatInfo();
    ~ChatInfo();

    // void init_group_information();
    unordered_set<string> *get_group_members(const string&);
    struct bufferevent* is_online_user(const string&);

    void insert_online_user_information(User *);
    void insert_group_information(const string&);
    void insert_group_members_information(const char*, const char*, string&, string&);
    bool info_group_is_exist(const string&);
    void user_offline(const string&);
};
#endif
