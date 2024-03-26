#ifndef CHAT_DATABASE_H
#define CHAT_DATABASE_H

#include <stdio.h>
// #include <stdlib.h>
#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <mutex>

#include <string.h>

using namespace std;

class ChatDatabase
{
private:
    MYSQL *mysql;
public:
    ChatDatabase();
    ~ChatDatabase();

    void my_database_connect(const char*);
    void my_database_disconnect();

    // 获取群名，保存在string数组传出，并返回群的数量 
    void my_database_get_group_name(vector<string>&); 
    void my_database_get_group_member(const char*, string&);
    bool my_database_user_is_exist(const char*);
    bool my_database_group_is_exist(const char*);
    void my_database_add_user(const char*, const char*);
    void my_database_add_group(const char*, const char*);
    void my_database_add_group_members(const char*, const char*);
    void my_database_add_user_friend(const char*, const char*);
    void my_database_add_users_group(const string&, const char*);
    void my_database_add_user_group(const char*, const char*);
    bool my_database_user_password_is_right(const char*, const char*);
    void my_database_get_user_friend(const char*, string&);
    void my_database_get_user_group(const char*, string&);
    
};

#endif
