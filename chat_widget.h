#ifndef CHAT_WIDGET_H
#define CHAT_WIDGET_H

#include <QDebug>
#include <QWidget>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QCloseEvent>
#include <QHash>

#include "addfriend.h"
#include "creategroup.h"
#include "addgroup.h"
#include "privatechat.h"
#include "groupchat.h"
#include "send_file_thread.h"
#include "recv_file_thread.h"


namespace Ui {
class chat_widget;
}

class chat_widget : public QWidget
{
    Q_OBJECT
signals:
    void MySignal(QString msg);
    void MyGSignal(QString msg);
    void MyDestroySignal();
public:
    explicit chat_widget(QTcpSocket *, QString *, QString *, QString *, QWidget *parent = nullptr);
    ~chat_widget();

protected:
     void closeEvent(QCloseEvent *event);

private slots:
    void server_reply();
    void setMyHash(QString);
    void setMyGHash(QString);

    void on_add_friend_button_clicked();
    void on_create_group_button_clicked();
    void on_add_group_button__clicked();
    void on_friend_list_itemDoubleClicked(QListWidgetItem *item);
    void on_group_list_itemDoubleClicked(QListWidgetItem *item);

private:
    void friend_login_handler(QString);
    void add_friend_handler(QJsonObject*);
    void create_group_handler(QJsonObject*);
    void add_group_handler(QJsonObject*);
    void friend_offline_handler(QString);
    void private_chat_handler(QJsonObject*);
    void get_group_members_handler(QJsonObject*);
    void group_chat_handler(QJsonObject*);
    void send_file_handler(QString);
    void send_file_port_handler(QJsonObject*);
    void recv_file_port_handler(QJsonObject*);


    Ui::chat_widget *ui;
    friend class privateChat;
    friend class groupChat;

    QTcpSocket *socket;

    QString name;
    QStringList friendList;
    QStringList groupList;

    QHash<QString, bool> isChating;
    QHash<QString, bool> isGroupChating;
};

#endif // CHAT_WIDGET_H
