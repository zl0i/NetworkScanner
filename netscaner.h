#ifndef NETSCANER_H
#define NETSCANER_H

#include <QObject>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QJsonArray>
#include <QJsonObject>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>

class NetScaner : public QObject
{
    Q_OBJECT
public:
    explicit NetScaner(QObject *parent = nullptr);

private:
    QList<int> ports;

    static QHostAddress setSubNetIPv4(QHostAddress ip, int sub);

    static QJsonArray scanSubNets(QHostAddress ip, QList<int> *ports, int start = 0, int end = 255, int msWaitForConnected = 30000);

    int msWaitForConnected = 10000;

    QJsonArray connectedAddresses;

public slots:
    void scan();
    void asyncScan();


signals:

};

#endif // NETSCANER_H
