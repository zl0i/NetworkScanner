#ifndef NETSCANER_H
#define NETSCANER_H

#include <QObject>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QJsonArray>
#include <QJsonObject>

class NetScaner : public QObject
{
    Q_OBJECT
public:
    explicit NetScaner(QObject *parent = nullptr);

private:
    QList<int> ports;

    QHostAddress setSubNetIPv4(QHostAddress ip, int sub);


public slots:
    QJsonArray scan();

signals:

};

#endif // NETSCANER_H
