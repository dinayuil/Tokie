#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QStringListModel>

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

    void initConnect();
    void initLists();

private slots:
    void onAddItemBtnClicked();
    void onListNameSelcChanged(const QItemSelection &selected);
    void onItemSelcChanged(const QItemSelection &selected);
    void onActDeleteItem();
    void onRightClickItem();
    void onAddListBtnClicked();
    void onRightClickListName();
    void onActDeleteList();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
