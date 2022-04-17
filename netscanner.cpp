#include "netscanner.h"

NetScanner::NetScanner(QObject *parent)
    : QObject{parent}
{
    ports.append(80);
}

QHostAddress NetScanner::setSubNetIPv4(QHostAddress ip, int sub)
{
    if(sub > 255)
        return QHostAddress();

    QStringList subs = ip.toString().split(".");
    subs[3] = QString::number(sub);
    return QHostAddress(subs.join("."));
}

void NetScanner::scanSubNets(QHostAddress ip, QList<int> *ports, ConnectedModel *model, int start, int end, int msWaitForConnected)
{
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
            model->append(QJsonObject {
                              { "address", current.toString() },
                              { "ports", connectedPorts }
                          });
        }
    }
}

QList<QHostAddress> NetScanner::filterAddresses(QList<QHostAddress> addresses)
{
    QList<QHostAddress> targets;
    for(int i = 0; i< addresses.size(); i++) {
        if(addresses[i].toString().contains("192.168")) {
            targets.append(addresses[i]);
        }
    }
    return targets;
}



void NetScanner::scan()
{
    model.clear();
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    QList<QHostAddress> targetAddresses = filterAddresses(addresses);
    for(int i = 0; i < targetAddresses.size(); i++) {
        QHostAddress current = targetAddresses[i];
        scanSubNets(current, &ports, &model, 0, 255);
    }
}

void NetScanner::asyncScan()
{
    model.clear();
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    QList<QHostAddress> targetAddresses = filterAddresses(addresses);
    for(int i = 0; i < targetAddresses.size(); i++) {
        QFuture<void> future = QtConcurrent::run(scanSubNets, targetAddresses[i], &ports, &model, 0, 255, msWaitForConnected);
    }
}
