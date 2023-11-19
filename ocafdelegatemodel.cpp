#include "ocafdelegatemodel.h"

OcafDelegateModel::OcafDelegateModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant OcafDelegateModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

QModelIndex OcafDelegateModel::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex OcafDelegateModel::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!

}

int OcafDelegateModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int OcafDelegateModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

bool OcafDelegateModel::hasChildren(const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

bool OcafDelegateModel::canFetchMore(const QModelIndex &parent) const
{
    // FIXME: Implement me!
    return false;
}

void OcafDelegateModel::fetchMore(const QModelIndex &parent)
{
    // FIXME: Implement me!
}

QVariant OcafDelegateModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}

void OcafDelegateModel::setupModelData(const QStringList &lines, LabelDelegate *parent)
{

}
