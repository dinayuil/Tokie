#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QStringListModel>
#include "tasklistmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QStandardItemModel* m_listNamesModel;
    QItemSelectionModel* m_listNameSelcModel;
    QHash<QString, TaskListModel*> m_nameToListMap;
    TaskListModel* m_taskListModel;
    QItemSelectionModel* m_taskListSelcModel;

    void initConnect();
    void initLists();
    void enableTaskDetailsUi();
    void disableTaskDetailsUi();

private slots:
    void onAddItemBtnClicked();
    void onListNameSelcChanged(const QItemSelection &selected);
    void onItemSelcChanged(const QItemSelection &selected);
    void onActDeleteItem();
    void onRightClickInTodoList(const QPoint &pos);
    void onAddListBtnClicked();
    void onRightClickInListName(const QPoint &pos);
    void onActDeleteList();
    void onTaskReminderChkBoxToggled(bool checked);
    void onTaskDueChkBoxToggled(bool checked);
    void onTaskNameLineEditFinished();
    void onTaskReminderDateTimeEditFinished();
    void onTaskDueDateEditFinished();
    void onListNamesRowsInserted();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
