#ifndef CONNECTEDMODEL_H
#define CONNECTEDMODEL_H

#include <QObject>
#include <QAbstractListModel>

#include <QJsonArray>
#include <QJsonObject>

class ConnectedModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ConnectedModel(QObject *parent = nullptr);

    typedef enum {
        AddressRole = Qt::UserRole+1,
        PortsRole
    }ConnectedRoles;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    void append(QJsonObject);
    void append(QJsonArray);
    void clear();

private:
    QJsonArray model;

signals:

};

#endif // CONNECTEDMODEL_H
