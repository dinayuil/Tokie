#ifndef TASKVIEWDELEGATE_H
#define TASKVIEWDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class TaskViewDelegate : public QStyledItemDelegate
{
public:
    explicit TaskViewDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}



    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // TASKVIEWDELEGATE_H
