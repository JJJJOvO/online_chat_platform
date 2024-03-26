#include "groupchat.h"
#include "ui_groupchat.h"

groupChat::groupChat(QTcpSocket *s, QString *gn, QString *m, QString *u, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::groupChat)
{
    ui->setupUi(this);
    socket = s;
    group_name = *gn;
    group_members = *m;
    me = *u;

    QStringList l = m -> split('|');
    for(auto iter = l.begin(); iter != l.end(); iter++)
    {
        ui -> groupMembersList -> addItem(*iter);
    }

    ui -> readTestEdit -> setWordWrapMode(QTextOption::WrapAnywhere);
    ui -> readTestEdit -> setReadOnly(true);

    this -> setWindowTitle(group_name);
}

groupChat::~groupChat()
{
    delete ui;
}

void groupChat::setMyText(QString msg)
{
    ui -> readTestEdit -> appendPlainText(msg);
}

void groupChat::closeEvent(QCloseEvent *event)
{
    emit myCloseSignal(group_name);
    event->accept();
}


void groupChat::on_send_clicked()
{
    QString text = ui -> sendTestEdit -> toPlainText();

    if(text.size() != 0)
    {
        QJsonObject obj;
        obj.insert("cmd", "group_chat");
        obj.insert("group", group_name);
        obj.insert("text", text);
        obj.insert("members", group_members);
        obj.insert("sender", me);

        QByteArray msg = QJsonDocument(obj).toJson();
        socket->write(msg);
        ui -> sendTestEdit -> clear();
    }
}
