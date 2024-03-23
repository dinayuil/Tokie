#include "tasklistmodel.h"

TaskListModel::TaskListModel(QObject *parent)
    : QAbstractListModel{parent}
{

}

int TaskListModel::rowCount(const QModelIndex &parent) const
{
    return m_taskList.size();
}

QVariant TaskListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
    {
        return QVariant();
    }
    if ( role == Qt::DisplayRole)
    {
        qDebug() << index.row();
        int row = index.row();
        if(row < m_taskList.count())
        {
            return QVariant(m_taskList[row].name());
        }
    }
    return QVariant();
}

void TaskListModel::addTask(QString taskName)
{
    beginInsertRows(QModelIndex(), m_taskList.size(), m_taskList.size());
    m_taskList.append(Task(taskName));
    endInsertRows();
}
