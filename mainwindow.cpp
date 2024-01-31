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
        m_model->appendRow(item);
        ui->newTodoEdit->clear();
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->setupUi(this);
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(1);
    m_selectionModel = new QItemSelectionModel(m_model, this);
    ui->todoListView->setModel(m_model);
    ui->todoListView->setSelectionModel(m_selectionModel);

    initConnect();
}

MainWindow::~MainWindow()
{
    delete ui;
}

