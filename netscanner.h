#ifndef NETSCANNER_H
#define NETSCANNER_H

#include <QObject>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QJsonArray>
#include <QJsonObject>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>

#include "connectedmodel.h"

class NetScanner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ConnectedModel *connectedModel READ connectedModel NOTIFY connectedModelChanged)
public:
    explicit NetScanner(QObject *parent = nullptr);

    ConnectedModel *connectedModel() { return &model; }

protected:
    virtual QList<QHostAddress> filterAddresses(QList<QHostAddress>);//TODO: rewtite to RegExp

private:
    QList<int> ports;
    int countFinishedFutures = 0;
    QList<QFutureWatcher<void>*> watcher;
    ConnectedModel model;
    int threads = 128; // true threads = threads * targetAddresses.count()
    int msWaitForConnected = 10000;
    bool isAsync = true;

    static QHostAddress setSubNetIPv4(QHostAddress ip, int sub);
    static void scanSubNets(QHostAddress ip, QList<int> *ports, ConnectedModel *model, int start = 0, int end = 255, int msWaitForConnected = 30000);
    QFutureWatcher<void> *createFuture(QHostAddress ip, int start, int end);

public slots:
    void scan();   
    void stop();


signals:
    void connectedModelChanged();
    void started();
    void finished();

};

#endif // NETSCANNER_H
