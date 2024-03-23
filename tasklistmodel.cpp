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
        assert(row < m_taskList.count());
        return QVariant(m_taskList[row].name());

    }
    return QVariant();
}

bool TaskListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
    {
        return false;
    }
    if ( role == Qt::DisplayRole)
    {
        int row = index.row();
        assert(row < m_taskList.count());
        m_taskList[row].setName(value.toString());
        return true;
    }
    return false;
}

Qt::ItemFlags TaskListModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
    {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemNeverHasChildren;
}

void TaskListModel::addTask(QString taskName)
{
    beginInsertRows(QModelIndex(), m_taskList.size(), m_taskList.size());
    m_taskList.append(Task(taskName));
    endInsertRows();
}
