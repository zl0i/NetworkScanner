#include "connectedmodel.h"

ConnectedModel::ConnectedModel(QObject *parent)
    : QAbstractListModel{parent}
{

}

QVariant ConnectedModel::data(const QModelIndex &index, int role) const
{
    if(index.row() > rowCount())
        return QVariant();

    const QJsonObject item = model.at(index.row()).toObject();

    switch (role) {
    case AddressRole:
        return item.value("address");
    case PortsRole:
        return item.value("ports");
    }
    return QVariant();
}

int ConnectedModel::rowCount(const QModelIndex&) const
{
    return model.size();
}

QHash<int, QByteArray> ConnectedModel::roleNames() const
{
    QHash<int, QByteArray> hash;
    hash[AddressRole] = "address";
    hash[PortsRole] = "ports";
    return hash;
}

void ConnectedModel::append(QJsonObject item)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    model.append(item);
    endInsertRows();
}

void ConnectedModel::append(QJsonArray arr)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount()+arr.size()-1);
    for(int i = 0; i < arr.size(); i++) {
        model.append(arr.at(i).toObject());
    }
    endInsertRows();
}

void ConnectedModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount());
    while(model.count()) {
         model.pop_back();
     }
    endRemoveRows();
}
