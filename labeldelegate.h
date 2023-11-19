#ifndef LABELDELEGATE_H
#define LABELDELEGATE_H

#include <QVector>
#include <QVariant>

class LabelDelegate
{
public:
    explicit LabelDelegate(const QVector<QVariant> &data, LabelDelegate *parentItem = nullptr);
    ~LabelDelegate();

    void appendChild(LabelDelegate *child);

    LabelDelegate *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    LabelDelegate *parentItem();

private:
    QVector<LabelDelegate*> m_childItems;
    QVector<QVariant> m_itemData;
    LabelDelegate *m_parentItem;
};

#endif // LABELDELEGATE_H
