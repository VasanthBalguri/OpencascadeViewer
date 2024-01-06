#include "ocafdelegatemodel.h"


QVariant OcafDelegateModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0)
        return QString("Tag");
    return QVariant();
}

QModelIndex OcafDelegateModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid())
    {
        LabelIndex* rootLabelIndex = indexHelper.getRootLabel();
        if(row == 0 && column == 0)
            return createIndex(0,column,rootLabelIndex);
    }
    else
    {
        LabelIndex* parentLabelIndex = static_cast<LabelIndex*>(parent.internalPointer());
        if(row < parentLabelIndex->getChildCount())
            return createIndex(row,column,parentLabelIndex->getChild(row));
    }
    return QModelIndex();
}

QModelIndex OcafDelegateModel::parent(const QModelIndex &index) const
{
    if(index.isValid())
    {
        LabelIndex* childLabelIndex = static_cast<LabelIndex*>(index.internalPointer());
        LabelIndex* parentLabelIndex = childLabelIndex->getParent();
        if(parentLabelIndex != nullptr)
        {
            return createIndex(parentLabelIndex->getIndex(),0,parentLabelIndex);
        }
    }
    return QModelIndex();

}

QModelIndex OcafDelegateModel::sibling(int row, int column, const QModelIndex &index) const
{
    if(index.isValid())
    {
        LabelIndex* childLabelIndex = static_cast<LabelIndex*>(index.internalPointer());
        LabelIndex* parentLabelIndex = childLabelIndex->getParent();
        if(parentLabelIndex != nullptr)
        {
           LabelIndex* siblingLabelIndex = parentLabelIndex->getChild(row);
           return createIndex(siblingLabelIndex->getIndex(),column,siblingLabelIndex);
        }
    }
    return QModelIndex();
}
int OcafDelegateModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        LabelIndex* parentLabelIndex = static_cast<LabelIndex*>(parent.internalPointer());
        return parentLabelIndex->getChildCount();
    }
    return 1;
}

int OcafDelegateModel::columnCount(const QModelIndex &parent) const
{
    //Need to refine this once we show columns
    if (!parent.isValid())
        return 1;

    return 1;
}

QVariant OcafDelegateModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && role == Qt::DisplayRole)
    {
        LabelIndex* labelIndex = static_cast<LabelIndex*>(index.internalPointer());
        return qint16(labelIndex->getTag());
    }

    return QVariant();
}

