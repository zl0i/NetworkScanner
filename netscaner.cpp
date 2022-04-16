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

QJsonArray NetScaner::scanSubNets(QHostAddress ip, QList<int> *ports, int start, int end, int msWaitForConnected)
{
    QJsonArray connectedAddresses;
    for(int i = start; i <= end; i++) {
        QHostAddress current = setSubNetIPv4(ip, i);
        if(current == ip) {
            continue;
        }
        QJsonArray connectedPorts;
        for(int j = 0; j < ports->size(); j++) {
            QTcpSocket socket;
            socket.connectToHost(current, ports->at(j));
            socket.waitForConnected(msWaitForConnected);
            if(socket.state() == QAbstractSocket::ConnectedState) {
                connectedPorts.append(ports->at(j));
            }
        }
        if(connectedPorts.size() > 0) {
            connectedAddresses.append(QJsonObject {
                                          { "address", current.toString() },
                                          { "ports", connectedPorts }
                                      });
        }
    }
    return connectedAddresses;
}

QList<QHostAddress> NetScaner::filterAddresses(QList<QHostAddress> addresses)
{
    QList<QHostAddress> targets;
    for(int i = 0; i< addresses.size(); i++) {
        if(addresses[i].toString().contains("192.168")) {
            targets.append(addresses[i]);
        }
    }
    return targets;
}



void NetScaner::scan()
{
    connectedModel.clear();
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    QList<QHostAddress> targetAddresses = filterAddresses(addresses);
    for(int i = 0; i < targetAddresses.size(); i++) {
        QHostAddress current = targetAddresses[i];
        scanSubNets(current, &ports, 0, 255);
    }
    connectedModel.append(connectedAddresses);
}

void NetScaner::asyncScan()
{
    connectedModel.clear();
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    QList<QHostAddress> targetAddresses = filterAddresses(addresses);
    for(int i = 0; i < targetAddresses.size(); i++) {
        QFuture<QJsonArray> future = QtConcurrent::run(scanSubNets, targetAddresses[i], &ports, 0, 2, msWaitForConnected);
        future.then([=](QJsonArray res) {
            connectedAddresses.append(res);
            connectedModel.append(res);
        });
    }
}
