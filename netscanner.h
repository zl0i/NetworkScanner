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
    Q_PROPERTY(bool async READ async WRITE setAsync NOTIFY asyncChanged)
    Q_PROPERTY(int waitForConnected READ waitForConnected WRITE setWaitForConnected NOTIFY waitForConnectedChanged)
    Q_PROPERTY(int threads READ threads WRITE setThreads NOTIFY threadsChanged)
public:
    explicit NetScanner(QObject *parent = nullptr);

    ConnectedModel *connectedModel() { return &model; }

    void setAsync(bool b) { isAsync = b; }
    bool async() { return isAsync; }
    void setWaitForConnected(int ms) { msWaitForConnected = ms; }
    int waitForConnected() { return msWaitForConnected; }
    void setThreads(int t) { countThreads = t; }
    int threads() { return countThreads; }

protected:
    virtual QList<QHostAddress> filterAddresses(QList<QHostAddress>);//TODO: rewtite to RegExp

private:
    QList<int> ports;
    int countFinishedFutures = 0;
    QList<QFutureWatcher<void>*> watcher;
    ConnectedModel model;
    int countThreads = 6; //if odd number, then true threads = threads + 1
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
    void asyncChanged();
    void waitForConnectedChanged();
    void threadsChanged();

    void started();
    void finished();

};

#endif // NETSCANNER_H
