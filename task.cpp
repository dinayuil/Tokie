#include "task.h"

long Task::s_startId=-1;

Task::Task(QString name):
    m_name(name)
{
    if(s_startId == -1)
    {
        s_startId = 1;
    }
    m_id = s_startId;
    s_startId++;
}

QString Task::name() const
{
    return m_name;
}

void Task::setName(const QString &newName)
{
    m_name = newName;
}

bool Task::enableReminder() const
{
    return m_enableReminder;
}

void Task::setEnableReminder(bool newEnableReminder)
{
    m_enableReminder = newEnableReminder;
}

bool Task::enableDue() const
{
    return m_enableDue;
}

void Task::setEnableDue(bool newEnableDue)
{
    m_enableDue = newEnableDue;
}

QDateTime Task::reminder() const
{
    return m_reminder;
}

void Task::setReminder(const QDateTime &newReminder)
{
    m_reminder = newReminder;
}

QDate Task::due() const
{
    return m_due;
}

void Task::setDue(const QDate &newDue)
{
    m_due = newDue;
}

QString Task::comment() const
{
    return m_comment;
}

void Task::setComment(const QString &newComment)
{
    m_comment = newComment;
}

bool Task::complete() const
{
    return m_complete;
}

void Task::setComplete(bool newComplete)
{
    m_complete = newComplete;
}
