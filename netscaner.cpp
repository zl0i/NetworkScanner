#include "netscaner.h"

NetScaner::NetScaner(QObject *parent)
    : QObject{parent}
{
    ports.append(80);
}

QHostAddress NetScaner::setSubNetIPv4(QHostAddress ip, int sub)
{
    if(sub > 255)
        return QHostAddress();

    QStringList subs = ip.toString().split(".");
    subs[3] = QString::number(sub);
    return QHostAddress(subs.join("."));
}




QJsonArray NetScaner::scan()
{
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    QList<QHostAddress> targetAddresses;
    for(int i = 0; i< addresses.size(); i++) {
        if(addresses[i].toString().contains("192.168")) {
            targetAddresses.append(addresses[i]);
        }
    }
    qDebug() << targetAddresses;
    QJsonArray connectedAddresses;
    for(int i = 0; i < targetAddresses.size(); i++) {
        QHostAddress currentTarget = targetAddresses[i];
        for(int j = 0; j < 2; j++) {
            QHostAddress current = setSubNetIPv4(currentTarget, j);
            if(current == currentTarget) {
                continue;
            }
            QJsonArray connectedPorts;
            for(int k = 0; k < ports.size(); k++) {
                    QTcpSocket socket;
                    socket.connectToHost(current, ports[k]);
                    socket.waitForConnected();
                    if(socket.state() == QAbstractSocket::ConnectedState) {
                        connectedPorts.append(ports[k]);
                    }
            }
            if(connectedPorts.size() > 0) {
            connectedAddresses.append(QJsonObject {
                                          { "address", current.toString() },
                                          { "ports", connectedPorts }
                                      });
            }
        }
    }
    qDebug() << "All!";
    qDebug() << connectedAddresses;
    return connectedAddresses;
}
