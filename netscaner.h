#ifndef NETSCANER_H
#define NETSCANER_H

#include <QObject>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QJsonArray>
#include <QJsonObject>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

#include "connectedmodel.h"

class NetScaner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ConnectedModel *connected READ connected NOTIFY connectedChanged)
public:
    explicit NetScaner(QObject *parent = nullptr);

    ConnectedModel *connected() { return &connectedModel; }

private:
    QList<int> ports;
    ConnectedModel connectedModel;

    static QHostAddress setSubNetIPv4(QHostAddress ip, int sub);

    static QJsonArray scanSubNets(QHostAddress ip, QList<int> *ports, int start = 0, int end = 255, int msWaitForConnected = 30000);

    int msWaitForConnected = 10000;

    QJsonArray connectedAddresses;
    virtual QList<QHostAddress> filterAddresses(QList<QHostAddress>);//TODO: rewtite to RegExp


public slots:
    void scan();
    void asyncScan();


signals:
    void connectedChanged();

};

#endif // NETSCANER_H
