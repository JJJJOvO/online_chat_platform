#ifndef CREATEGROUP_H
#define CREATEGROUP_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QListWidget>

namespace Ui {
class createGroup;
}

class createGroup : public QWidget
{
    Q_OBJECT

public:
    explicit createGroup(QTcpSocket*, QString*, QStringList*, QWidget *parent = nullptr);
    ~createGroup();

private slots:


    void on_offbtn_clicked();

    void on_okbtn_clicked();


    void on_invite_friend_list_itemClicked(QListWidgetItem *item);

private:
    Ui::createGroup *ui;

    QTcpSocket *socket;
    QString name;
};

#endif // CREATEGROUP_H
