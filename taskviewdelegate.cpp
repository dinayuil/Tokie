#include "taskviewdelegate.h"
#include "TaskDataRoles.h"
#include "qdatetime.h"
#include <qapplication.h>
#include <QMouseEvent>

bool TaskViewDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        qDebug() << "editorEvent: option.rect.size()" << option.rect.size();
        QMouseEvent* pME = static_cast<QMouseEvent*>(event);

        qDebug() << "pME->pos()" << pME->pos();
        QStyleOptionButton checkBoxOpt;
        QRect checkBoxRec = createCheckBoxRect(option, checkBoxOpt);
        if (checkBoxRec.contains(pME->pos()))
        {
            bool value = index.data(TaskCompleteRole).toBool();
            model->setData(index, !value, TaskCompleteRole);
        }
        return true;
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void TaskViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    // Retrieve data for name, reminder, and due
    QString name = index.data(TaskNameRole).toString();
    QDate dueDate = index.data(TaskDueRole).toDate();
    QDate reminderDate = index.data(TaskReminderRole).toDate();
    
    // Set up style options
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // Draw background
    //if(opt.state & QStyle::State_Selected)
    //{
    //    painter->fillRect(opt.rect, option.palette.highlight());
    //}

    // Draw checkbox
    QStyleOptionButton checkBoxOpt;
    checkBoxOpt.rect = createCheckBoxRect(option, checkBoxOpt);
    checkBoxOpt.state = QStyle::State_Enabled;

    bool isChecked = index.data(TaskCompleteRole).toBool();
    if (isChecked)
    {
        checkBoxOpt.state |= QStyle::State_On;
    }
    else
    {
        checkBoxOpt.state |= QStyle::State_Off;
    }

    QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkBoxOpt, painter);

    // Draw texts
    painter->save();
    int checkBoxWidth = checkBoxOpt.rect.width();
    if (reminderDate.isValid() && dueDate.isValid())
    {
        painter->drawText(opt.rect.adjusted(checkBoxWidth + 5, 0, 0, 0), Qt::AlignLeft, name);
        painter->drawText(opt.rect.adjusted(checkBoxWidth + 5, 20, 0, 0), Qt::AlignLeft, dueDate.toString());
        painter->drawText(opt.rect.adjusted(checkBoxWidth + 100, 20, 0, 0), Qt::AlignLeft, reminderDate.toString());
    }
    else if (dueDate.isValid())
    {
        painter->drawText(opt.rect.adjusted(checkBoxWidth + 5, 0, 0, 0), Qt::AlignLeft, name);
        painter->drawText(opt.rect.adjusted(checkBoxWidth + 5, 20, 0, 0), Qt::AlignLeft, dueDate.toString());
    }
    else if (reminderDate.isValid())
    {
        painter->drawText(opt.rect.adjusted(checkBoxWidth + 5, 0, 0, 0), Qt::AlignLeft, name);
        painter->drawText(opt.rect.adjusted(checkBoxWidth + 5, 20, 0, 0), Qt::AlignLeft, reminderDate.toString());
    }
    else
    {
        painter->drawText(opt.rect.adjusted(checkBoxWidth + 5, 12, 0, 0), Qt::AlignLeft, name);
    }
    painter->restore();
}

QSize TaskViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(150, 40); // Example size
}

QRect TaskViewDelegate::createCheckBoxRect(const QStyleOptionViewItem& option, const QStyleOptionButton& checkBoxOpt) const
{
    QSize checkBoxSize = QApplication::style()->sizeFromContents(QStyle::CT_CheckBox, &checkBoxOpt, QSize());
    int checkBoxRectTopLeftY = option.rect.topLeft().y() + option.rect.height() / 2 - checkBoxSize.height() / 2;
    QPoint checkBoxRectTopLeft(0, checkBoxRectTopLeftY);
    QRect checkBoxRect(checkBoxRectTopLeft, checkBoxSize);    // checkBoxRect's position should be related to the option of each item,
                                                                // not a fixed value, otherwise all the check box is at the same position
    return checkBoxRect;
}