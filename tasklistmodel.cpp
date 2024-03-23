#include "tasklistmodel.h"
#include "TaskDataRoles.h"

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

    qDebug() << index.row();
    int row = index.row();
    assert(row < m_taskList.count());

    switch (role)
    {
    case Qt::DisplayRole:
    {
        return QVariant(m_taskList[row].name());
    }
    case TaskEnableDueRole:
    {
        return QVariant(m_taskList[row].enableDue());
    }
    case TaskDueRole:
    {
        return QVariant(m_taskList[row].due());
    }
    case TaskEnableReminderRole:
    {
        return QVariant(m_taskList[row].enableReminder());
    }
    case TaskReminderRole:
    {
        return QVariant(m_taskList[row].reminder());
    }
    case TaskCommentRole:
    {
        return QVariant(m_taskList[row].comment());
    }
    case TaskCompleteRole:
    {
        return QVariant(m_taskList[row].complete());
    }
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
    // https://doc.qt.io/qt-6.5/qt.html#ItemFlag-enum
    return Qt::ItemIsSelectable|Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemNeverHasChildren;
}

void TaskListModel::addTask(QString taskName)
{
    beginInsertRows(QModelIndex(), m_taskList.size(), m_taskList.size());
    m_taskList.append(Task(taskName));
    endInsertRows();
}
