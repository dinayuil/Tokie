#ifndef TASKLISTMODEL_H
#define TASKLISTMODEL_H

#include <QAbstractListModel>
#include "task.h"

class TaskListModel : public QAbstractListModel
{
public:
    explicit TaskListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    QList<Task> m_taskList;
};

#endif // TASKLISTMODEL_H
