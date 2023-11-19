#ifndef OCAFDELEGATEMODEL_H
#define OCAFDELEGATEMODEL_H

#include <QAbstractItemModel>
#include <BinDrivers.hxx>
#include <TDataStd_Integer.hxx>
#include <TDF_Label.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDataStd_Name.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDataXtd_Triangulation.hxx>
#include <TDF_Reference.hxx>

#include "labeldelegate.h"

class OcafDelegateModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit OcafDelegateModel(opencascade::handle<TDocStd_Document> doc, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // Fetch data dynamically:
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    void setupModelData(const QStringList &lines, LabelDelegate *parent);

    LabelDelegate *rootItem;
};

#endif // OCAFDELEGATEMODEL_H
