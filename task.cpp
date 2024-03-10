#include "task.h"

Task::Task(QString name):
    m_name(name)
{

}

QString Task::name() const
{
    return m_name;
}
