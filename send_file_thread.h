#ifndef SEND_FILE_THREAD_H
#define SEND_FILE_THREAD_H

#include <QTcpSocket>
#include <QHostAddress> // 包含IP地址
#include <QJsonObject>
#include <QJsonDocument>
#include <QThread>
#include <QFile>

class send_file_thread : public QThread
{
public:
    send_file_thread(QJsonObject*);
    void run();


private:

    QString filename;
    int fileLength;
    int port;
    QTcpSocket *sendSocket;
};

#endif // SEND_FILE_THREAD_H
