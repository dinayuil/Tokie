#include "mainwindow.h"
#include "./ui_mainwindow.h"

void MainWindow::initConnect()
{
    connect(ui->addBtn, SIGNAL(clicked()), this, SLOT(onAddButtonClicked())) ;
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

void MainWindow::initLists()
{
    m_allListNames<<"北京"<<"上海"<<"天津"<<"河北"<<"山东"<<"四川"<<"重庆"<<"广东"<<"河南";
    m_listNamesModel->setStringList(m_allListNames);
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
    m_listNamesModel = new QStringListModel(this);
    ui->listNamesView->setModel(m_listNamesModel);

    initConnect();
    initLists();
}

MainWindow::~MainWindow()
{
    delete ui;
}

