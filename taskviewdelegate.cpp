#include "taskviewdelegate.h"
#include "TaskDataRoles.h"
#include "qdatetime.h"


void TaskViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // Retrieve data for name, reminder, and due
    QString name = index.data(Qt::DisplayRole).toString();
    QDate dueDate = index.data(TaskDueRole).toDate();
    QDate reminderDate = index.data(TaskReminderRole).toDate();

    // Set up style options
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // Draw background
    if(opt.state & QStyle::State_Selected)
    {
        painter->fillRect(opt.rect, option.palette.highlight());
    }

    // Draw texts
    painter->save();
    painter->setPen(option.palette.text().color());
    if(reminderDate.isValid() && dueDate.isValid())
    {
        painter->drawText(opt.rect.adjusted(5, 0, 0, 0), Qt::AlignLeft, name);
        painter->drawText(opt.rect.adjusted(5, 20, 0, 0), Qt::AlignLeft, dueDate.toString());
        painter->drawText(opt.rect.adjusted(100, 20, 0, 0), Qt::AlignLeft, reminderDate.toString());
    }
    else if(dueDate.isValid())
    {
        painter->drawText(opt.rect.adjusted(5, 0, 0, 0), Qt::AlignLeft, name);
        painter->drawText(opt.rect.adjusted(5, 20, 0, 0), Qt::AlignLeft, dueDate.toString());
    }
    else if(reminderDate.isValid())
    {
        painter->drawText(opt.rect.adjusted(5, 0, 0, 0), Qt::AlignLeft, name);
        painter->drawText(opt.rect.adjusted(5, 20, 0, 0), Qt::AlignLeft, reminderDate.toString());
    }
    else
    {
        painter->drawText(opt.rect.adjusted(5, 12, 0, 0), Qt::AlignLeft, name);
    }

    painter->restore();
}

QSize TaskViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(150, 40); // Example size
}
