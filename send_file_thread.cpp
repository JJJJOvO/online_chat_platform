#include "send_file_thread.h"

send_file_thread::send_file_thread(QJsonObject *obj)
{
    port = obj->value("port").toString().toInt();
    filename = obj->value("filename").toString();
    fileLength = obj->value("length").toInt();

    qDebug() << filename << " "<< fileLength << " "<< port;
}

// 重写线程函数
void send_file_thread::run()
{
    QTcpSocket sendSocket;

    sendSocket.connectToHost(QHostAddress("47.115.224.218"), 8080);
    if(!sendSocket.waitForConnected(10000))
    {
        qDebug()<<sendSocket.error();
        this->quit();
    }
    else
    {
        QFile file(filename);
        file.open(QIODevice::ReadOnly);

        bool flag = true;
        while(flag)
        {
            QByteArray msg =  file.read(1024);
            if(msg.size() == 0) flag = false;
            else
            {
                sendSocket.write(msg);
                sendSocket.flush();
            }
        }
        sendSocket.close();
        file.close();
        this->quit();
    }
}

