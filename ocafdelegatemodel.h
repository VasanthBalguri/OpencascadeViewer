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
#include <TDF_ChildIDIterator.hxx>
#include <QTreeView>

class LabelIndex
{
    friend class LabelIndexFactory;
public:
    ~LabelIndex()
    {
        for(int i = 0;i < children.size(); i++)
        {
            delete children[i];
        }
    }

    int getTag() const {return tag;}
    int getIndex() const {return index;}
    int getChildCount() const {return children.size();}
    LabelIndex* getChild(int index) const {
        if(index < children.size())
            return children[index];
        else
            return nullptr;

    }
    LabelIndex* getParent() const
    {
        return parent;
    }
    LabelIndex* addChild(int tag)
    {
        LabelIndex* child = new LabelIndex(tag,this);
        children.push_back(child);
        child->index = children.size() - 1;
        return child;
    }
    void removeChild(int index)
    {
        delete children[index];
        children.erase(children.begin()+index);
    }

private:
    LabelIndex(int tag,LabelIndex* parent = nullptr)
    {
        if(parent != nullptr)
        {
            this->parent = parent;
            this->tag = tag;
            this->index = 0;
        }
        else
        {
            this->parent = nullptr;
            this->tag=tag;
            this->index = 0;
        }
    }
   LabelIndex*  parent;
   std::vector<LabelIndex*> children;
   int tag;
   int index;
};

class LabelIndexFactory
{
public:
    LabelIndexFactory(opencascade::handle<TDocStd_Document> document)
    {
        TDF_Label rootLabel = document->GetData()->Root();
        root = new LabelIndex(rootLabel.Tag(),nullptr);
        for (TDF_ChildIterator it(rootLabel); it.More(); it.Next()) {
            setupLevel(root->addChild(it.Value().Tag()),it.Value());
        }
    }

    ~LabelIndexFactory()
    {
        delete root;
    }

    LabelIndex* getRootLabel() const
    {
        return root;
    }

private:
    LabelIndex* root;
    void setupLevel(LabelIndex* parent,TDF_Label parentLabel)
    {
        for (TDF_ChildIterator it(parentLabel); it.More(); it.Next()) {
            setupLevel(parent->addChild(it.Value().Tag()),it.Value());
        }
    }
};

class OcafDelegateModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit OcafDelegateModel(opencascade::handle<TDocStd_Document> doc, QObject *parent = nullptr): QAbstractItemModel(parent),document(doc),indexHelper(doc)
    {
    }

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex sibling(int row, int column, const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // Fetch data dynamically:
    /*bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;*/

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    void setupModelData();
    opencascade::handle<TDocStd_Document> document;
    LabelIndexFactory indexHelper;
};

#endif // OCAFDELEGATEMODEL_H
