#include "addfriend.h"
#include "ui_addfriend.h"

addFriend::addFriend(QTcpSocket *s, QString user, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addFriend)
{
    ui->setupUi(this);
    this -> setWindowTitle("添加好友");
    name = user;
    socket = s;

}

addFriend::~addFriend()
{
    delete ui;
}

void addFriend::on_off_clicked()
{
    this -> close();
}

void addFriend::on_ok_clicked()
{
    QString f = ui -> friendLineEdit -> text();

    if(f.count() == 0)
    {
        QMessageBox::information(this, "添加好友提醒", "请输入需要添加的好友");
        return;
    }
    else
    {
        QJsonObject obj;
        obj.insert("cmd", "add_friend");
        obj.insert("user", name);
        obj.insert("tar_friend", f);

        QByteArray msg = QJsonDocument(obj).toJson();
        socket->write(msg);

        this -> close();
    }
}
