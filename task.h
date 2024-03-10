#ifndef TASK_H
#define TASK_H

#include <QDate>
#include <QDateTime>

class Task
{
public:
    Task(QString name);

    QString name() const;
    void setName(const QString &newName);

    bool enableReminder() const;
    void setEnableReminder(bool newEnableReminder);

    bool enableDue() const;
    void setEnableDue(bool newEnableDue);

    QDateTime reminder() const;
    void setReminder(const QDateTime &newReminder);

    QDate due() const;
    void setDue(const QDate &newDue);

    QString comment() const;
    void setComment(const QString &newComment);

private:
    bool m_enableReminder = false;
    QDateTime m_reminder;
    bool m_enableDue = false;
    QDate m_due;
    QString m_comment = "";
    QString m_name;
};

Q_DECLARE_METATYPE(Task *);

#endif // TASK_H
