#ifndef TASKDATAROLES_H
#define TASKDATAROLES_H

#include "qnamespace.h"

enum TaskDataRoles {
    TaskNameRole = Qt::UserRole + 1,
    TaskEnableDueRole,
    TaskDueRole,
    TaskEnableReminderRole,
    TaskReminderRole,
    TaskCommentRole,
    TaskCompleteRole,
    // Add more custom roles as needed
};

#endif // TASKDATAROLES_H
