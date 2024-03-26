#ifndef GROUPCHAT_H
#define GROUPCHAT_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QCloseEvent>

namespace Ui {
class groupChat;
}

class groupChat : public QWidget
{
    Q_OBJECT
signals:
    void myCloseSignal(QString g);

public:
    explicit groupChat(QTcpSocket*, QString*, QString*, QString*, QWidget *parent = nullptr);
    ~groupChat();

protected:
     void closeEvent(QCloseEvent *event);

public slots:
    void setMyText(QString text);

private slots:

    void on_send_clicked();

private:
    Ui::groupChat *ui;

    QString me;
    QString group_name;
    QString group_members;

    QTcpSocket *socket;
};

#endif // GROUPCHAT_H
