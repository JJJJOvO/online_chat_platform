#include "privatechat.h"
#include "ui_privatechat.h"

privateChat::privateChat(QTcpSocket *s, QString *user, QString *f, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::privateChat)
{
    ui->setupUi(this);

    socket = s;
    name = *user;
    fri = *f;

    this -> setWindowTitle(fri);
    ui -> readTextEdit -> setWordWrapMode(QTextOption::WrapAnywhere);
    ui -> readTextEdit -> setReadOnly(true);
}

privateChat::~privateChat()
{
    delete ui;
}


void privateChat::setMyText(QString msg)
{
    ui -> readTextEdit -> appendPlainText(msg);
}

void privateChat::on_sendTestBtn_clicked()
{
    QString test = ui -> sendTextEdit -> toPlainText();

    if(test.size() == 0)
    {
        return;
    }
    else
    {
        QJsonObject obj;
        obj.insert("cmd", "private_chat");
        obj.insert("to_user", fri);
        obj.insert("from_user", name);
        obj.insert("test", test);

        QByteArray msg = QJsonDocument(obj).toJson();
        socket->write(msg);

        ui -> sendTextEdit -> clear();
    }
}

void privateChat::closeEvent(QCloseEvent *event)
{
    emit closeFriChat(fri);
}



void privateChat::on_sendFileBtn_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "发送文件", QCoreApplication::applicationFilePath() );
    if(filename.isEmpty())
    {
        QMessageBox::warning(this, "发送文件提示", "您未选择要发送的文件");
    }
    else
    {
        QFile file(filename);
        file.open(QIODevice::ReadOnly);
        QJsonObject obj;
        obj.insert("cmd", "send_file");
        obj.insert("from_user", name);
        obj.insert("to_user", fri);
        obj.insert("length", file.size());
        obj.insert("filename", filename);
        QByteArray msg = QJsonDocument(obj).toJson();
        socket->write(msg);
        file.close();
    }
}
