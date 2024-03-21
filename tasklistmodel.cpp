#include "tasklistmodel.h"

TaskListModel::TaskListModel(QObject *parent)
    : QAbstractListModel{parent}
{

}

int TaskListModel::rowCount(const QModelIndex &parent) const
{
    return m_taskList.count();
}
