#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QCoreApplication> // 获取文件路径
#include <QFile>

#include <QDebug>

namespace Ui {
class privateChat;
}

class privateChat : public QWidget
{
    Q_OBJECT
signals:
    void closeFriChat(QString f);

public:
    explicit privateChat(QTcpSocket*, QString*, QString*, QWidget *parent = nullptr);
    ~privateChat();

protected:
     void closeEvent(QCloseEvent *event);

public slots:
    void setMyText(QString msg);

private slots:
    void on_sendTestBtn_clicked();

    void on_sendFileBtn_clicked();

private:
    Ui::privateChat *ui;

    QTcpSocket *socket;
    QString name;
    QString fri;
};

#endif // PRIVATECHAT_H
