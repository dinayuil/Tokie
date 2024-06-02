#ifndef TASKVIEWDELEGATE_H
#define TASKVIEWDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class TaskViewDelegate : public QStyledItemDelegate
{
public:
    explicit TaskViewDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}


    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;
    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QRect createCheckBoxRect(const QStyleOptionViewItem& option, const QStyleOptionButton& checkBoxOpt) const;
};

#endif // TASKVIEWDELEGATE_H
