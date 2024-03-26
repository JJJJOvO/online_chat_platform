#include "chat_widget.h"
#include "ui_chat_widget.h"

chat_widget::chat_widget(QTcpSocket *s, QString * user, QString * friends, QString * groups, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chat_widget)
{
    ui->setupUi(this);

    socket = s;
    name = *user;

    this -> setWindowTitle("JJ聊天平台——" + name);
    connect(socket, &QTcpSocket::readyRead, this, &chat_widget::server_reply);

    // 根据 ‘|’ 分割字符串
    friendList = friends -> split('|');

    for(auto iter = friendList.begin(); iter != friendList.end(); iter ++)
    {
        ui -> friend_list -> addItem(*iter);
        isChating[*iter] = false;
    }

    groupList = groups -> split('|');

    for(auto iter = groupList.begin(); iter != groupList.end(); iter ++)
    {
        ui -> group_list -> addItem(*iter);
        isGroupChating[*iter] = false;
    }

}

chat_widget::~chat_widget()
{
    delete ui;
}
void chat_widget::closeEvent(QCloseEvent *event)
{
    QJsonObject obj;
    obj.insert("cmd", "off_line");
    obj.insert("user", name);

    QByteArray msg = QJsonDocument(obj).toJson();
    socket -> write(msg);

    socket -> disconnected();
    emit MyDestroySignal();
}

void chat_widget::server_reply()
{
    QByteArray msg = socket -> readAll();
    QJsonObject obj = QJsonDocument::fromJson(msg).object();

    QString cmd = obj.value("cmd").toString();

    if(cmd == "online_reminder")
    {
        friend_login_handler(obj.value("result").toString());
    }
    else if(cmd == "add_friend_reply")
    {
        add_friend_handler(&obj);
    }
    else if(cmd == "create_group_reply")
    {
        create_group_handler(&obj);
    }
    else if(cmd == "invite_reply")
    {
        add_group_handler(&obj);
    }
    else if(cmd == "offline_reminder")
    {
        friend_offline_handler(obj.value("result").toString());
    }
    else if(cmd == "private_chat")
    {
        private_chat_handler(&obj);
    }
    else if(cmd == "get_members_reply")
    {
        get_group_members_handler(&obj);
    }
    else if(cmd == "group_chat_reply")
    {
        group_chat_handler(&obj);
    }
    else if(cmd == "send_file_reply")
    {
        send_file_handler(obj.value("result").toString());
    }
    else if(cmd == "send_file_port_reply")
    {
        send_file_port_handler(&obj);
    }
    else if(cmd == "recv_file_port_reply")
    {
        recv_file_port_handler(&obj);
    }
}

void chat_widget::friend_login_handler(QString fri)
{
    QMessageBox::information(this, "好友上线提醒", QString("%1 已上线").arg(fri));
}

void chat_widget::friend_offline_handler(QString fri)
{
    QMessageBox::information(this, "好友下线提醒", QString("%1 已下线").arg(fri));
}

void chat_widget::add_friend_handler(QJsonObject *obj)
{
    QString res = obj -> value("result").toString();
    if(res == "success")
    {
        QString fri = obj -> value("friend").toString();
        QMessageBox::information(this, "添加好友提醒",QString("与(%1)成为好友").arg(fri));
        friendList << fri;

        ui -> friend_list -> addItem(fri);
        isChating[fri]=false;
    }
    else if(res == "be_friends")
    {
        QMessageBox::information(this, "添加好友提醒", "添加失败——已经是好友关系");
    }
    else
    {
        QMessageBox::information(this, "添加好友提醒", "添加失败——所加好友不存在");
    }
}

void chat_widget::create_group_handler(QJsonObject *obj)
{
    qDebug()<<"create"<<endl;
    QString res = obj -> value("result") .toString();
    QString group = obj -> value("group").toString();
    qDebug()<<group<<endl;
    if(res == "success")
    {
        qDebug()<<"success";
        QMessageBox::information(this, "创建群组提醒", QString("%1 群创建成功").arg(group));
        groupList << group;
        ui -> group_list -> addItem(group);
        isGroupChating[group]=false;
    }
    else
    {
        QMessageBox::information(this, "创建群组提醒", QString("%1 群已存在").arg(group));
    }
}

void chat_widget::add_group_handler(QJsonObject *obj)
{
    QString res = obj -> value("result") .toString();
    QString group = obj -> value("group").toString();

    if(res == "success")
    {
        QString inviter = obj -> value("inviter").toString();
        qDebug()<<inviter <<" "<< name;
        if(inviter != name)
        {
            QMessageBox::information(this, "加入群组提醒", QString("您被 %1 邀请加入群组 %2 ").arg(inviter).arg(group));
            groupList << group;
            ui -> group_list -> addItem(group);
            isGroupChating[group]=false;
        }
        else
        {
            QMessageBox::information(this, "加入群组提醒", QString("创建群 %1 成功 ").arg(group));
            groupList << group;
            ui -> group_list -> addItem(group);
            isGroupChating[group]=false;
        }
    }
    else
    {
        QStringList exit_members = obj -> value("exist_members").toString().split('|');
        int count = exit_members.count();

        if(count == 1 && exit_members.at(0) == name)
        {
            QMessageBox::information(this, "加入群组提醒", "您已在群组 %1 )中", group);
        }
        else
        {
            QMessageBox box(this);
            box.setWindowTitle("加入群组提醒");

            QString str = "你所邀请的成员中有" + QString::number(count) + "位已在群组" + group + "中";
            box.setText(str);

            box.setStandardButtons(QMessageBox::Ok | QMessageBox::Help);
            box.setButtonText(QMessageBox::Ok, QString("确定"));
            box.setButtonText(QMessageBox::Cancel, QString("更多"));

            box.show();
            int ret = box.exec();
            if(ret == QMessageBox::Help)
            {
                QString info = "";
                for(int i=0; i<exit_members.count(); i++)
                {
                    info.append(exit_members.at(i) + "、");
                }

                info.remove(info.length() - sizeof("、") , sizeof("、"));

                QMessageBox::information(box.window(), "已在群组成员", info);
            }
        }
    }
}

void chat_widget::private_chat_handler(QJsonObject *obj)
{
    QString res = obj->value("result").toString();

    if(res == "off_line")
    {
        QMessageBox::information(this, "私聊提醒", QString("好友%1已下线，无法进行聊天").arg(obj -> value("to_user").toString() ) );
    }
    else
    {
        if(res == "send_reply")
        {
            QString msg = "我: " + obj -> value("test").toString();
            emit MySignal(msg);
        }
        else if(res == "accept")
        {
            QString f = obj->value("from_user").toString();

            if(!isChating[f])
            {
                privateChat *privateChatWidget = new privateChat(socket, &name, &f);
                connect(this, &chat_widget::MySignal, privateChatWidget, &privateChat::setMyText);
                connect(privateChatWidget, &privateChat::closeFriChat, this, &chat_widget::setMyHash);
                isChating[f]=true;
                privateChatWidget -> show();
            }
            QString msg = f.append(": ") + obj -> value("test").toString();
            emit MySignal(msg);
        }
    }
}

void chat_widget::get_group_members_handler(QJsonObject *obj)
{
    QString group = obj->value("result").toString();

    if(!isGroupChating[group])
    {
        QString members = obj -> value("members").toString();
        groupChat *groupChatWidget = new groupChat(socket, &group, &members, &name);
        connect(this, &chat_widget::MyGSignal, groupChatWidget, &groupChat::setMyText);
        connect(groupChatWidget, &groupChat::myCloseSignal, this, &chat_widget::setMyGHash);

        isGroupChating[group] = true;
        groupChatWidget -> show();
    }
}

void chat_widget::group_chat_handler(QJsonObject *obj)
{
    QString res = obj -> value("result").toString();
    QString group = obj -> value("group").toString();
    if(res == "accept")
    {
        QString msg = obj -> value("sender").toString() + ": " + obj->value("text").toString();
        if(!isGroupChating[group])
        {
            QString members = obj -> value("members").toString();
            groupChat *groupChatWidget = new groupChat(socket, &group, &members, &name);
            connect(this, &chat_widget::MyGSignal, groupChatWidget, &groupChat::setMyText);
            connect(groupChatWidget, &groupChat::myCloseSignal, this, &chat_widget::setMyGHash);


            isGroupChating[group] = true;
            groupChatWidget -> show();
        }
        emit MyGSignal(msg);
    }
    else if(res == "send_reply")
    {
        QString msg = "我: " + obj->value("text").toString();
        qDebug() << msg;
        if(!isGroupChating[group])
        {
            QString members = obj -> value("members").toString();
            groupChat *groupChatWidget = new groupChat(socket, &group, &members, &name);
            connect(this, &chat_widget::MyGSignal, groupChatWidget, &groupChat::setMyText);
            connect(groupChatWidget, &groupChat::myCloseSignal, this, &chat_widget::setMyGHash);

            isGroupChating[group] = true;
            groupChatWidget -> show();
        }
        emit MyGSignal(msg);
    }
}

void chat_widget::send_file_handler(QString res)
{
    if(res == "offline")
    {
        QMessageBox::information(this, "发送文件提醒", "朋友不在线");
    }
    else if(res == "time_out")
    {
        QMessageBox::information(this, "发送文件提醒", "连接超时");
    }
}

void chat_widget::send_file_port_handler(QJsonObject *obj)
{
    qDebug() << "获取服务器文件发送 端口";
    send_file_thread *mysendThread = new send_file_thread(obj);
    mysendThread -> start();
}

void chat_widget::recv_file_port_handler(QJsonObject *obj)
{
    qDebug() << "获取服务器文件接收 端口";
    recv_file_thread *myrecvThread = new recv_file_thread(obj);
    myrecvThread -> start();
}

void chat_widget::on_add_friend_button_clicked()
{
    addFriend * addFriendWidget = new addFriend(socket, name);
    addFriendWidget -> show();
}


void chat_widget::on_create_group_button_clicked()
{
    if(friendList.count() <= 1)
    {
        QMessageBox::information(this, "创建群提醒", "好友列表成员数量小于等于1, 无法创建群");
    }
    else
    {
        createGroup * createGroupWidget = new createGroup(socket, &name, &friendList);
        createGroupWidget -> show();
        QMessageBox::information(createGroupWidget, "邀请哦好友进群提示", "双击确定需要邀请的好友，再次双击则为取消");
    }
}

void chat_widget::on_add_group_button__clicked()
{
    addGroup *addGroupWidget = new addGroup(socket, &name);
    addGroupWidget -> show();
}

void chat_widget::on_friend_list_itemDoubleClicked(QListWidgetItem *item)
{
    QString fri = item -> text();
    if(!isChating[fri])
    {
        privateChat *privateChatWidget = new privateChat(socket, &name, &fri);
        connect(this, &chat_widget::MySignal, privateChatWidget, &privateChat::setMyText);
        connect(privateChatWidget, &privateChat::closeFriChat, this, &chat_widget::setMyHash);
        isChating[fri]=true;
        privateChatWidget -> show();
    }
    else
    {
        QMessageBox::information(this, "私聊提醒", QString("与 %1 好友的私聊窗口已打卡").arg(fri));
    }
}

void chat_widget::setMyHash(QString f)
{
    isChating[f] = false;
}

void chat_widget::on_group_list_itemDoubleClicked(QListWidgetItem *item)
{
    QString group_name = item -> text();
    QJsonObject obj;

    obj.insert("cmd", "get_members");
    obj.insert("group", group_name);

    QByteArray msg = QJsonDocument(obj).toJson();
    socket->write(msg);
}

void chat_widget::setMyGHash(QString group)
{
    isGroupChating[group] = false;
}
