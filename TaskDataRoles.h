#ifndef TASKDATAROLES_H
#define TASKDATAROLES_H

#include "qnamespace.h"

enum TaskDataRoles {
    TaskEnableDueRole = Qt::UserRole + 1,
    TaskDueRole,
    TaskEnableReminderRole,
    TaskReminderRole,
    TaskCommentRole,
    TaskCompleteRole,
    // Add more custom roles as needed
};

#endif // TASKDATAROLES_H
