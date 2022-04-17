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

QFutureWatcher<void> *NetScanner::createFuture(QHostAddress ip, int start, int end)
{
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
    connect(watcher, &QFutureWatcher<int>::finished, [=] {
        countFinishedFutures += 1;
        if(countFinishedFutures == this->watcher.size()-this->watcher.size()/countThreads) {
            emit finished();
        }
    });
    watcher->setFuture(QtConcurrent::run(scanSubNets, ip, &ports, &model, start, end, msWaitForConnected));
    return watcher;
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
    stop();
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    QList<QHostAddress> targetAddresses = filterAddresses(addresses);
    if(isAsync) {
        for(int i = 0; i < targetAddresses.size(); i++) {
            int count = (255 / countThreads) * targetAddresses.size();
            for(int j = 0; j < (countThreads/targetAddresses.size())-1; j++) {
                watcher.append( createFuture(targetAddresses[i], j*count, j*count+count-1));
            }
            watcher.append(createFuture(targetAddresses[i], (countThreads/targetAddresses.size()-1)*count, 255));
        }
    } else {
        for(int i = 0; i < targetAddresses.size(); i++) {
            QHostAddress current = targetAddresses[i];
            scanSubNets(current, &ports, &model, 0, 255);
        }
    }

}

void NetScanner::stop()
{
    countFinishedFutures = 0;
    for(int i = 0; i < watcher.size(); i++) {
        watcher.at(i)->future().cancel();
        watcher.at(i)->deleteLater();
    }
    watcher.clear();
}
