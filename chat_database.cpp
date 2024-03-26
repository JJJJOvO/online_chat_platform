#include "chat_database.h"

mutex mutexSQL;
unique_lock<mutex> lockSQL(mutexSQL, defer_lock);

ChatDatabase::ChatDatabase()
{
    
}

ChatDatabase::~ChatDatabase()
{
    mysql_close(mysql);
}

// 参数 name 指定要连接的数据库名称
void ChatDatabase::my_database_connect(const char *name)
{
    // 初始化一个 mysql 对象
    lockSQL.lock();
    mysql = mysql_init(NULL);

    // 连接 本地mysql
    mysql = mysql_real_connect(mysql, "localhost", "root", "123456", name, 0, NULL, 0);


    if(mysql == NULL)
    {
        mysql_error(mysql);
    }

    if(mysql_query(mysql, "set names utf8;") != 0)
    {
        mysql_error(mysql);
    }

}

void ChatDatabase::my_database_disconnect()
{
    // 关闭之前打开的连接
    mysql_close(mysql);
    lockSQL.unlock();
}

void ChatDatabase::my_database_get_group_name(vector<string> &group_name)
{       
    char sql[128]={0};
    sprintf(sql, "show tables;");
    if(mysql_query(mysql, sql) != 0)
    {
        mysql_errno(mysql);
    }

    // 利用mysql.h 自定义类型MYSQL_RES 获取结果
    MYSQL_RES *res = mysql_store_result(mysql);

    //MYSQL_ROW 相当于char** 类型
    // mysql_fetch_row 返回 show tables 的一行数据（即组名）--- 
    MYSQL_ROW row = mysql_fetch_row(res); 
    for(;row != NULL; row = mysql_fetch_row(res))
    {
       group_name.push_back(row[0]);
    }
}

void ChatDatabase::my_database_get_group_member(const char *name, string &member) 
{
    // 保存要执行的sql语句 —— select member from 学习交流群
    char sql[1024] = {0};
    sprintf(sql, "select member from %s;", name);

    if(mysql_query(mysql, sql) != 0)
    {
        mysql_error(mysql);
    }

    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);

    if(row[0] != NULL)  member = string(row[0]);
}

bool ChatDatabase::my_database_user_is_exist(const char* name)
{
    char sql[128]={0};

    sprintf(sql, "show tables like '%s';", name);

    if(mysql_query(mysql, sql) != 0)
    {
        mysql_errno(mysql);
    }
    //cout << name <<endl;
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);
    
    if(row == NULL)
    {
        //cout << "用户不存在"<<endl;
        return false;
    }
    // 用户存在
    else
    {
        //cout << "用户存在"<<endl;
        return true;
    }

}

bool ChatDatabase::my_database_group_is_exist(const char *group_name)
{
    char sql[128]={0};

    sprintf(sql, "show tables like '%s';", group_name);

    if(mysql_query(mysql, sql) != 0)
    {
        mysql_errno(mysql);
    }
    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);

    if(row == NULL)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void ChatDatabase::my_database_add_user(const char *user, const char *password)
{
    char sql[128]={0};

    sprintf(sql, 
    "create table %s (password varchar(16), friend varchar(4096), chat_group varchar(4096)) charset=utf8;", user);

    if(mysql_query(mysql, sql) != 0)
    {
        mysql_errno(mysql);
    }

    memset(sql, 0, sizeof(sql));

    sprintf(sql, "insert into %s (password) values (%s);", user, password);

    if(mysql_query(mysql, sql) != 0)
    {
        mysql_errno(mysql);
    }
}

bool ChatDatabase::my_database_user_password_is_right(const char *user, const char *password)
{
    char sql[128]={0};
    sprintf(sql, "select password from %s", user);

    if(mysql_query(mysql, sql) != 0)
    {
        mysql_errno(mysql);
    }

    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);

    /*
    strcmp : 相等返回0，大于返回1，小于返回-1
    */
    if(strcmp(password, row[0])==0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void ChatDatabase::my_database_get_user_friend(const char *user, string &user_friends)
{
    char sql[128]={0};
    sprintf(sql, "select friend from %s;", user);

    if(mysql_query(mysql, sql) != 0)
    {
        mysql_errno(mysql);
    }

    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);

    if(row[0] != NULL) user_friends = string(row[0]);
}

void ChatDatabase::my_database_get_user_group(const char *user, string &user_group)
{
    char sql[128]={0};
    sprintf(sql, "select chat_group from %s;", user);

    if(mysql_query(mysql, sql) != 0)
    {
        mysql_errno(mysql);
    }

    MYSQL_RES *res = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);

    if(row[0] != NULL) user_group = string(row[0]);
}

void ChatDatabase::my_database_add_user_friend(const char *user, const char *tar_friend)
{
    char sql[128]={0};

    string f = "";
    my_database_get_user_friend(user, f);

    if(!f.empty()) f += "|" + string(tar_friend);
    else f = string(tar_friend);

    sprintf(sql, "UPDATE %s SET friend = '%s';", user, f.c_str());
    if(mysql_query(mysql, sql) != 0)
    {
        mysql_error(mysql);
    }
}

void ChatDatabase::my_database_add_users_group(const string &users, const char *group_name)
{
    unsigned int start=0, end=0;
    bool flag = true;
    while(flag)
    {
        end = users.find('|', start);
        if(end == -1)
        {
            my_database_add_user_group(users.substr(start, users.size() - start).c_str(), group_name);
            flag = false;
        }
        else
        {
            my_database_add_user_group(users.substr(start, end - start).c_str(), group_name);
            start = end + 1;
        }
    }

}

void ChatDatabase::my_database_add_user_group(const char *user, const char *group_name)
{
    cout << user <<endl;
    char sql[128] = {0};

    string old_groups="";
    
    my_database_get_user_group(user, old_groups);
    cout << "1. old_groups: "<<old_groups<<endl;
    if(!old_groups.empty()) old_groups += '|' + string(group_name);
    else old_groups = string(group_name);

    cout << "2. old_groups: "<<old_groups<<endl;
    sprintf(sql, "UPDATE %s SET chat_group = '%s';", user, old_groups.c_str());

    if(mysql_query(mysql, sql) != 0)
    {
        mysql_errno(mysql);
    }
}



void ChatDatabase::my_database_add_group(const char *group_name, const char *owner)
{
    char sql[128]={0};

    sprintf(sql, "create table %s (owner varchar(32), member varchar(4096)) charset=utf8;", group_name);

    if(mysql_query(mysql, sql) != 0)
    {
        mysql_error(mysql);
    }

    memset(sql, 0, sizeof(sql));

    sprintf(sql, "insert into %s (owner) values('%s');", group_name, owner);

    if(mysql_query(mysql, sql) != 0)
    {
        mysql_error(mysql);
    }
    
}

void ChatDatabase::my_database_add_group_members(const char *group_name, const char *new_members)
{
    char sql[128]={0};

    string old_members = "";

    cout << "add_group_members: " << new_members <<endl;

    my_database_get_group_member(group_name, old_members);

    cout << "old_members: " << old_members <<endl;

    if(!old_members.empty()) old_members += '|' + string(new_members);
    else old_members = string(new_members);
    
    sprintf(sql, "UPDATE %s SET member = '%s';", group_name, old_members.c_str());

    if(mysql_query(mysql, sql) != 0)
    {
        mysql_error(mysql);
    }
}