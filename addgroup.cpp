#include "addgroup.h"
#include "ui_addgroup.h"

addGroup::addGroup(QTcpSocket *s, QString *user, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addGroup)
{
    ui->setupUi(this);

    this -> setWindowTitle("添加群组");
    name = *user;
    socket = s;
}

addGroup::~addGroup()
{
    delete ui;
}

void addGroup::on_off_clicked()
{
    this -> close();
}

void addGroup::on_ok_clicked()
{
    QString group_name = ui -> groupLineEdit -> text();
    if(group_name.count() == 0)
    {
        QMessageBox::information(this, "添加群组提醒", "请输入需要添加的群名");
        return;
    }
    else
    {
        QJsonObject obj;
        obj.insert("cmd", "invite_friends");
        obj.insert("inviter", name);
        obj.insert("members", name);
        obj.insert("group", group_name);

        QByteArray msg = QJsonDocument(obj).toJson();
        socket->write(msg);

        this -> close();
    }
}
