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
    QStringListModel* m_listNamesModel;
    QList<QString> m_allListNames;
    QHash<QString, QStandardItemModel> m_nameToListMap;

    void initConnect();
    void initLists();

private slots:
    void onAddButtonClicked();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
