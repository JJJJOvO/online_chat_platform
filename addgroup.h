#ifndef ADDGROUP_H
#define ADDGROUP_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>


namespace Ui {
class addGroup;
}

class addGroup : public QWidget
{
    Q_OBJECT

public:
    explicit addGroup(QTcpSocket*, QString*, QWidget *parent = nullptr);
    ~addGroup();

private slots:
    void on_off_clicked();

    void on_ok_clicked();

private:
    Ui::addGroup *ui;
    QTcpSocket *socket;
    QString name;
};

#endif // ADDGROUP_H
