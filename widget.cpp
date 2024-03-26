#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this -> setWindowTitle("JJ聊天平台登录器");

    //设置具体阴影
    QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setOffset(0, 0);

    //阴影颜色
    shadow_effect->setColor(QColor(38, 78, 119, 127));

    //阴影半径
    shadow_effect->setBlurRadius(22);

    ui->myframe->setGraphicsEffect(shadow_effect);


    //setPixmap();
    int width = (this->width() - ui->myframe->width()) / 2;
    int height = (this->height() - ui->myframe->height()) / 2;
    ui -> myframe -> move(width, height);

    socket = new QTcpSocket;
    socket -> connectToHost(QHostAddress("47.115.224.218"), 8000);

    connect(socket, &QTcpSocket::connected, this, &Widget::connect_success);
    connect(socket, &QTcpSocket::readyRead, this, &Widget::server_reply);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::connect_success()
{
    QMessageBox::information(this, "连接提示", "连接服务器成功");
}


void Widget::server_reply()
{
    QByteArray msg = socket -> readAll();
    QJsonObject obj = QJsonDocument::fromJson(msg).object();

    QString cmd = obj.value("cmd").toString();


    if(cmd == "register_reply")
    {
        client_register_handler(obj);
    }
    else if(cmd == "login_reply")
    {
        client_login_handler(obj);
    }
}

void Widget::TCP_Disconnected()
{
    socket -> close();
    this -> close();
}

void Widget::client_register_handler(QJsonObject obj)
{    

    QString res = obj.value("result").toString();

    if(res == "success")
    {
        QMessageBox::information(this,"注册提示", "注册成功");
    }
    else
    {
        QMessageBox::information(this, "注册提示" , "用户名已被注册");
    }
}

void Widget::client_login_handler(QJsonObject obj)
{   

    QString res = obj.value("result").toString();

    if(res == "user_is_not_exist")
    {
        QMessageBox::information(this, "登陆提示", "用户不存在");
    }
    else if(res == "password_is_not_right")
    {
        QMessageBox::information(this, "登陆提示", "密码错误");
    }
    else if(res == "is_online")
    {
        QMessageBox::information(this, "登陆提示", "该账户已被登陆");
    }
    else if(res == "success")
    {
        QString friends = obj.value("friend").toString();
        QString groups = obj.value("group").toString();

        this -> hide();

        socket -> disconnect(SIGNAL(readyRead()));
        chat_widget *c = new chat_widget(socket, &name, &friends, &groups);

        connect(c, &chat_widget::MyDestroySignal, this, &Widget::TCP_Disconnected);
        c -> show();
    }
}

void Widget::on_registerButton_clicked()
{
    QString user = ui -> user_line_edit -> text();
    QString password = ui -> password_line_edit -> text();

    QJsonObject obj;
    obj.insert("cmd", "register");
    obj.insert("user", user);
    obj.insert("password", password);

    QByteArray msg = QJsonDocument(obj).toJson();
    socket -> write(msg);
}

void Widget::on_loginButton_clicked()
{
    QString user = ui -> user_line_edit -> text();
    QString password = ui -> password_line_edit -> text();

    QJsonObject obj;
    obj.insert("cmd", "login");
    obj.insert("user", user);
    obj.insert("password", password);

    QByteArray msg = QJsonDocument(obj).toJson();
    socket -> write(msg);

    name = user;
}
