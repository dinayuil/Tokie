#include "mainwindow.h"
#include "./ui_mainwindow.h"

void MainWindow::initConnect()
{
    connect(ui->addBtn, SIGNAL(clicked()), this, SLOT(onAddButtonClicked())) ;
    connect(m_listNameSelcModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onListNameSelcChanged(QItemSelection)));
    connect(m_itemSelcModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onItemSelcChanged(QItemSelection)));
}

void MainWindow::onAddButtonClicked()
{
    QString newTodo = ui->newTodoEdit->text();
    if(!newTodo.isEmpty())
    {
        QStandardItem* item = new QStandardItem(newTodo);
        item->setCheckable(true);
        m_itemModel->appendRow(item);
        ui->newTodoEdit->clear();
    }
}

void MainWindow::onListNameSelcChanged(const QItemSelection &selected)
{
    QModelIndexList selectedIndexes = selected.indexes();
    for(int i = 0; i < selectedIndexes.size(); i++)
    {

        qDebug() << m_listNamesModel->data(selectedIndexes[i]).toString();

        m_itemModel = m_nameToListMap[m_listNamesModel->data(selectedIndexes[i]).toString()];
        ui->todoListView->setModel(m_itemModel);

        m_itemSelcModel->setModel(m_itemModel);
        ui->todoListView->setSelectionModel(m_itemSelcModel);
    }
}

void MainWindow::onItemSelcChanged(const QItemSelection &selected)
{
    QModelIndexList selectedIndexes = selected.indexes();
    for(int i = 0; i < selectedIndexes.size(); i++)
    {
        qDebug() << m_itemModel->data(selectedIndexes[i]).toString();
    }
}

void MainWindow::onActDeleteItem()
{
    m_itemSelcModel->currentIndex();

}

void MainWindow::initLists()
{
    // to keep the order of names  a separate list is needed, hash or map both change the order
    m_allListNames<<"北京"<<"上海"<<"天津"<<"河北"<<"山东"<<"四川"<<"重庆"<<"广东"<<"河南";
    for(const auto& name: m_allListNames)
    {
        m_nameToListMap[name] = new QStandardItemModel(this);
        m_nameToListMap[name]->setColumnCount(1);
    }

    for(const auto& name: m_allListNames)
    {
        QStandardItem* item = new QStandardItem(name);
        m_listNamesModel->appendRow(item);
    }


    for(int i = 0; i < 15; i++)
    {
        QStandardItem* item = new QStandardItem(QString::number(i));
        item->setCheckable(true);
        m_itemModel->appendRow(item);
    }

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // todoListView
    m_itemModel = new QStandardItemModel(this);
    m_itemModel->setColumnCount(1);
    m_itemSelcModel = new QItemSelectionModel(m_itemModel, this);
    ui->todoListView->setModel(m_itemModel);
    ui->todoListView->setSelectionModel(m_itemSelcModel);

    // listNamesView
    m_listNamesModel = new QStandardItemModel(this);
    m_listNamesModel->setColumnCount(1);
    m_listNameSelcModel = new QItemSelectionModel(m_listNamesModel, this);
    ui->listNamesView->setModel(m_listNamesModel);
    ui->listNamesView->setSelectionModel(m_listNameSelcModel);

    initConnect();
    initLists();
}

MainWindow::~MainWindow()
{
    delete ui;
}

