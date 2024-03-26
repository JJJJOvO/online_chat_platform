#include "recv_file_thread.h"

recv_file_thread::recv_file_thread(QJsonObject *obj)
{
    total = 0;
    port = obj->value("port").toString().toInt();
    fileLength = obj->value("length").toInt();

    // 得到的是发送方的路径名，解析路径名获得文件名
    QString pathname = obj->value("filename").toString();
    QStringList list = pathname.split('/');
    filename = list.at(list.size() -1);
    qDebug() << filename << " "<< fileLength << " "<< port;
}

recv_file_thread::~recv_file_thread()
{
    delete file;
    delete recvSocket;
}
void recv_file_thread::run()
{
    file = new QFile(filename); // 已文件名打开，默认存放在.exe
    file -> open(QIODevice::WriteOnly);
    recvSocket = new QTcpSocket;
    // Qt::DirectConnection 将父线程的槽函数连接到子线程的信号
    connect(recvSocket, &QTcpSocket::readyRead, this, &recv_file_thread::recv_file, Qt::DirectConnection);

    recvSocket->connectToHost(QHostAddress("47.115.224.218"), 8080);

    if(!recvSocket -> waitForConnected(10000))
    {
        this -> quit();
    }
    else
    {
       qDebug() << "接收端连接服务器";
    }
    exec(); // 保持 run
}

void recv_file_thread::recv_file()
{
    // 读取一次数据
    qDebug() << "begin----";
    QByteArray msg = recvSocket->readAll();
    qDebug() << sizeof(msg);
    total += file -> write(msg);
    qDebug() << "总共接收" << total <<"数据: ";
    if(total >= fileLength)
    {
        file -> close();
        recvSocket -> close();
        this -> quit();
    }
    qDebug() << "end----";

}
