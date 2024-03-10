#ifndef TASK_H
#define TASK_H

#include <QDate>
#include <QDateTime>

class Task
{
public:
    Task();

private:
    bool m_enableReminder;
    QDateTime m_reminder;
    bool m_enableDue;
    QDate m_due;
    QString m_comment;
    QString m_name;
};

#endif // TASK_H
