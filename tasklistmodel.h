#ifndef TASKLISTMODEL_H
#define TASKLISTMODEL_H

#include <QAbstractListModel>
#include "task.h"

class TaskListModel : public QAbstractListModel
{
public:
    explicit TaskListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void addTask(QString taskName);
    void removeTask(const QModelIndex& index);

private:
    QList<Task> m_taskList;
};

#endif // TASKLISTMODEL_H
