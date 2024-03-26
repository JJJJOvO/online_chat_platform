#include "chatinfo.h"

// 互斥访问 链表
std::mutex mutexInfo;

ChatInfo::ChatInfo()
{
    to_find_online_users = new unordered_map<string, struct bufferevent*>;
    to_find_exist_group_members = new unordered_map<string, unordered_set<string>>;

    // users = new list<User>;
    // group_info = new list<Group>;

    // 从数据库读取数据群信息，加载到group链表中
    mydatabase = new ChatDatabase;

    mydatabase -> my_database_connect("chatgroup");   

    vector<string> group_name;
    mydatabase -> my_database_get_group_name(group_name);

    // 初始化group_info信息
    for(unsigned int i=0; i<group_name.size() ; i++)
    {
        
        string str = group_name[i];

        Group g;
        g.name = str;
        // g.group_menber_info = new list<GroupMember>;
        unordered_set<string> my_set;

        string member_info;
        mydatabase->my_database_get_group_member(str.c_str(), member_info);
        
        // 利用 find 和 substr 处理member_info
        unsigned int start=0,end=0; 
        bool flag = true;
        while(flag)
        {
            // GroupMember gm;

            end = member_info.find('|', start);
            if(end == -1)
            {
                // 处理最后一位成员
                string member_name = member_info.substr(start, member_info.size()-start);

                // gm.name = member_name;
                // g.group_menber_info->push_back(gm);
                my_set.insert(member_name);
                // 数据库需保证群成员 两位以上
                flag = false;
            }
            else
            {
                string member_name = member_info.substr(start, end-start);

                // gm.name = member_name;
                // g.group_menber_info->push_back(gm);
                my_set.insert(member_name);

                start = end + 1; // end此时指向‘｜’，不要忘记+1，不然一直寻找的是前面说的 ‘｜’
            }
        }
        
        // group_info->push_back(g);
        (*to_find_exist_group_members)[str] = my_set;
    }

    mydatabase->my_database_disconnect();
    cout << " ----- 数据库 群信息 加载完毕 ------" <<endl;
}

ChatInfo::~ChatInfo()
{
    delete mydatabase;
    delete to_find_exist_group_members;
    delete to_find_online_users;
}


struct bufferevent* ChatInfo::is_online_user(const string &s)
{
    unique_lock<mutex> lockInfo(mutexInfo);

    auto iter = to_find_online_users -> find(s);
    if( iter == to_find_online_users -> end())
    {
        return nullptr;
    }
    else return iter -> second;
}

bool ChatInfo::info_group_is_exist(const string &group_name)
{
    if((*to_find_exist_group_members).find(group_name) != (*to_find_exist_group_members).end()) return true;
    else return false;
}

void ChatInfo::insert_online_user_information(User *u_info)
{
    // users -> push_back(*u_info);
    (*to_find_online_users)[u_info -> name] = u_info -> bev;
}

void ChatInfo::insert_group_information(const string &group_name)
{
    unique_lock<mutex> lockInfo(mutexInfo);
    (*to_find_exist_group_members)[group_name] = unordered_set<string>();
}   

void ChatInfo::insert_group_members_information(const char *group_name, const char *members, string &new_members, string &exist_members)
{
    string gn(group_name);
    unordered_set<string> &my_set = (*to_find_exist_group_members)[gn];


    string m(members);

    unsigned int end=0, start=0;
    // 判断 members 里是否有用户已经在群里
    bool flag = true;

    unique_lock<mutex> lockInfo(mutexInfo);
    while(flag)
    {
        end = m.find('|', start);
        if(end == -1)
        {
            string u = m.substr(start, m.size()-start);
            if(my_set.find(u) == my_set.end())
            {
                my_set.insert(u);

                if(!new_members.empty()) new_members += '|' + u;
                else new_members += u;
            }
            else
            {
                if(!exist_members.empty()) exist_members += '|' + u;
                else exist_members += u;       
            }
            flag = false;
        }
        else
        {
            string u = m.substr(start, end-start);
            if(my_set.find(u) == my_set.end())
            {
                my_set.insert(u);

                if(!new_members.empty()) new_members += '|' + u;
                else new_members += u;
            }
            else
            {
                if(!exist_members.empty()) exist_members += '|' + u;
                else exist_members += u;       
            }
            start = end + 1;
        }
    }
    
}

unordered_set<string>* ChatInfo::get_group_members(const string &gn)
{
    unique_lock<mutex> lockInfo(mutexInfo);
    return &(*to_find_exist_group_members)[gn];
}

void ChatInfo::user_offline(const string &user)
{
    unique_lock<mutex> lockInfo(mutexInfo);
    to_find_online_users -> erase(user);
}