#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QStringListModel>
#include <QSqlTableModel>
#include <QSqlQueryModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QStandardItemModel* m_itemModel;
    QItemSelectionModel* m_itemSelcModel;
    QStandardItemModel* m_listNamesModel;
    QItemSelectionModel* m_listNameSelcModel;
    QHash<QString, QStandardItemModel*> m_nameToListMap;
    QStandardItem* m_beingModifiedItem = nullptr;
    QSqlDatabase m_db;
    QSqlQueryModel* m_queryModel;

    void initConnect();
    void initLists();
    void enableTaskDetailsUi();
    void disableTaskDetailsUi();
    void clearTaskDetailsUiContent();
    QStandardItem* beingModifiedItem();

private slots:
    void onAddItemBtnClicked();
    void onListNameSelcChanged(const QItemSelection &selected);
    void onItemSelcChanged(const QItemSelection &selected);
    void onActDeleteItem();
    void onRightClickInTaskList(const QPoint &pos);
    void onAddListBtnClicked();
    void onRightClickInListNameList(const QPoint &pos);
    void onActDeleteList();
    void onTaskReminderChkBoxToggled(bool checked);
    void onTaskDueChkBoxToggled(bool checked);
    void onTaskNameLineEditFinished();
    void onTaskReminderDateTimeEditFinished();
    void onTaskDueDateEditFinished();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
