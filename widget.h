#ifndef WIDGET_H
#define WIDGET_H

#include <iostream>
#include <QWidget>
#include <QTcpSocket>
#include <QHostAddress> // 包含IP地址
#include <QMessageBox>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QGraphicsDropShadowEffect>
#include "chat_widget.h"




QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void connect_success();
    void server_reply();
    void TCP_Disconnected();


    void on_registerButton_clicked();
    void on_loginButton_clicked();

private:
    Ui::Widget *ui;
    friend class chat_widget;

    QTcpSocket *socket;
    QString name;

    void client_register_handler(QJsonObject);
    void client_login_handler(QJsonObject);
};
#endif // WIDGET_H
