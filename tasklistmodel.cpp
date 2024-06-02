#include "tasklistmodel.h"
#include "TaskDataRoles.h"
#include <QFile>


TaskListModel::TaskListModel(QString listName, QObject *parent)
    : m_listName(listName),
    QAbstractListModel{parent}
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

//    qDebug() << index.row();
    int row = index.row();
    assert(row < m_taskList.count());

    switch (role)
    {
    case TaskNameRole:
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

    int row = index.row();
    assert(row < m_taskList.count());

    switch ( role )
    {
    case TaskNameRole:
    {
        m_taskList[row].setName(value.toString());
        return true;
    }
    case TaskEnableDueRole:
    {
        m_taskList[row].setEnableDue(value.toBool());
        return true;
    }
    case TaskDueRole:
    {
        qDebug() << "setData Due row: " << row;
        qDebug() << value.toDate();
        m_taskList[row].setDue(value.toDate());
        return true;
    }
    case TaskEnableReminderRole:
    {
        m_taskList[row].setEnableReminder(value.toBool());
        return true;
    }
    case TaskReminderRole:
    {
        qDebug() << "setData Reminder row: " << row;
        qDebug() << value.toDateTime();
        m_taskList[row].setReminder(value.toDateTime());
        return true;
    }
    case TaskCommentRole:
    {
        m_taskList[row].setComment(value.toString());
        return true;
    }
    case TaskCompleteRole:
    {
        m_taskList[row].setComplete(value.toBool());
        return true;
    }
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
    Task task;
    task.setName(taskName);
    m_taskList.append(task);
    endInsertRows();
}

void TaskListModel::removeTask(const QModelIndex &index)
{
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    m_taskList.removeAt(index.row());
    endRemoveRows();
}

void TaskListModel::saveModel()
{

    QFile file("data/" + m_listName+".dat");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    for(const auto& task : m_taskList)
    {
        out << task;
    }
    file.close();
}

void TaskListModel::loadModel(QDataStream &in)
{
    while(!in.atEnd())
    {
        Task task;
        in >> task;
        m_taskList.append(task);
    }
}

void TaskListModel::setListName(const QString &newListName)
{
    m_listName = newListName;
}
