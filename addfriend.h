#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>

namespace Ui {
class addFriend;
}

class addFriend : public QWidget
{
    Q_OBJECT

public:
    explicit addFriend(QTcpSocket *, QString, QWidget *parent = nullptr);
    ~addFriend();

private slots:
    void on_off_clicked();

    void on_ok_clicked();

private:
    Ui::addFriend *ui;
    QString name;
    QTcpSocket *socket;
};

#endif // ADDFRIEND_H
