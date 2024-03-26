#include "creategroup.h"
#include "ui_creategroup.h"

createGroup::createGroup(QTcpSocket* s, QString *user, QStringList* list, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::createGroup)
{
    ui->setupUi(this);

    this -> setWindowTitle("创建群组");
    socket = s;
    name = *user;

    for(auto iter = list -> begin(); iter != list -> end(); iter ++)
    {
        ui -> invite_friend_list -> addItem(*iter);
    }
}

createGroup::~createGroup()
{
    delete ui;
}



void createGroup::on_offbtn_clicked()
{
    this -> close();
}

void createGroup::on_okbtn_clicked()
{
    QString group_name = ui -> groupLineEdit -> text();
    if(group_name.size() == 0)
    {
        QMessageBox::information(this, "创建群提醒", "请输入需要创建群的名字");
        return;
    }
    if(ui->invite_friend_list->count() <= 1)
    {
        QMessageBox::information(this, "创建群提醒", "邀请群成员数量不足二");
        return;
    }
    QString invite_friend;
    for(int i=0; i < ui->invite_friend_list->count(); i++)
    {
        QListWidgetItem * item = ui -> invite_friend_list -> item(i);
        if(item -> isSelected())
        {
            QString str = item -> text();
            invite_friend.append(str + '|');
        }
    }

    invite_friend = invite_friend.remove(invite_friend.length() - 1, 1);

    QJsonObject obj;
    obj.insert("cmd", "add_group");
    obj.insert("owner", name);
    obj.insert("inviter", name);
    obj.insert("group", group_name);
    obj.insert("members", invite_friend);

    QByteArray msg = QJsonDocument(obj).toJson();
    socket->write(msg);

    this -> close();
}

void createGroup::on_invite_friend_list_itemClicked(QListWidgetItem *item)
{
    if(item -> isSelected())
    {
        item -> setSelected(false);
    }
    else
    {
        item -> setSelected(true);
    }
}
