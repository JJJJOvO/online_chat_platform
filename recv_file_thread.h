#ifndef RECV_FILE_THREAD_H
#define RECV_FILE_THREAD_H

#include <QTcpSocket>
#include <QHostAddress> // 包含IP地址
#include <QJsonObject>
#include <QJsonDocument>
#include <QThread>
#include <QFile>


class recv_file_thread : public QThread
{
    Q_OBJECT
public:
    recv_file_thread(QJsonObject*);
    ~recv_file_thread();
    void run(); // 属于子线程，其他都属于父线程

private slots:
    void recv_file();

private:

    QString filename;
    int fileLength;
    int port;
    int total;
    QTcpSocket *recvSocket; // 在子线程连接，不占用父线程
    QFile *file;
};

#endif // RECV_FILE_THREAD_H
