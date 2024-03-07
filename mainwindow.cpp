#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>

void MainWindow::initConnect()
{
    /* todo list */
    connect(ui->addItemBtn, SIGNAL(clicked()), this, SLOT(onAddItemBtnClicked()));
    connect(m_itemSelcModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onItemSelcChanged(QItemSelection)));
    // todo list right click menu
    connect(ui->todoListView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onRightClickItem()));
    connect(ui->actionDeleteItem, SIGNAL(triggered()), this, SLOT(onActDeleteItem()));

    /* lists of todo list*/
    connect(ui->addListBtn, SIGNAL(clicked()), this, SLOT(onAddListBtnClicked()));
    connect(m_listNameSelcModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onListNameSelcChanged(QItemSelection)));
    // lists right click menu
    connect(ui->listNamesView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onRightClickListName()));
    connect(ui->actionDeleteList, SIGNAL(triggered(bool)), this, SLOT(onActDeleteList()));
}

void MainWindow::onAddItemBtnClicked()
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
    int currentRow = m_itemSelcModel->currentIndex().row();
    m_itemModel->removeRow(currentRow); // this will release the memory, no need to use `delete`
}

void MainWindow::onRightClickItem()
{
    QMenu* menuList= new QMenu(this);
    menuList->addAction(ui->actionDeleteItem);
    if(!m_itemSelcModel->currentIndex().isValid())
    {
        ui->actionDeleteItem->setEnabled(false);
    }
    else
    {
        ui->actionDeleteItem->setEnabled(true);
    }
    menuList->exec(QCursor::pos());
    delete menuList;
}

void MainWindow::onAddListBtnClicked()
{
    bool validName = false;
    QString dlgTitle= "Input a list name";
    QString txtLabel= "List name";
    QString listName = "Unnamed list";
    while(!validName)
    {
        bool ok= false;
        listName = QInputDialog::getText(this, dlgTitle, txtLabel, QLineEdit::Normal, listName, &ok);
        if(!ok)
        {
            break;
        }
        if (ok && !listName.isEmpty())
        {
            if(!m_nameToListMap.contains(listName))
            {
                validName = true;
            }
            else
            {
                QMessageBox::critical(this, "Invalid name", "List name already exists.");
            }
        }
    }

    if(validName)
    {
        QStandardItem* item = new QStandardItem(listName);
        m_listNamesModel->appendRow(item);
        m_nameToListMap[listName] = new QStandardItemModel(this);
        m_nameToListMap[listName]->setColumnCount(1);
    }

}

void MainWindow::onRightClickListName()
{
    QMenu* menuList= new QMenu(this);
    menuList->addAction(ui->actionDeleteList);
    if(!m_listNameSelcModel->currentIndex().isValid())
    {
        ui->actionDeleteList->setEnabled(false);
    }
    else
    {
        ui->actionDeleteList->setEnabled(true);
    }
    menuList->exec(QCursor::pos());
    delete menuList;
}

void MainWindow::onActDeleteList()
{
    int currentRow = m_listNameSelcModel->currentIndex().row();
    QStandardItem* listNameItem = m_listNamesModel->item(currentRow);
    QStandardItemModel* todoItemModel = m_nameToListMap[listNameItem->text()];
    delete todoItemModel;   // call the model's destructor, and also destroys all its items
    m_nameToListMap.remove(listNameItem->text());
    m_listNamesModel->removeRow(currentRow);
}

void MainWindow::initLists()
{
    // in the future they should be loaded from disk
    QList<QString> allListNames;
    allListNames<<"北京"<<"上海"<<"天津"<<"河北"<<"山东"<<"四川"<<"重庆"<<"广东"<<"河南";
    for(const auto& name: allListNames)
    {
        m_nameToListMap[name] = new QStandardItemModel(this);
        m_nameToListMap[name]->setColumnCount(1);
    }

    for(const auto& name: allListNames)
    {
        QStandardItem* item = new QStandardItem(name);
        m_listNamesModel->appendRow(item);
    }


    for(long i = 0; i < 15; i++)
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
    ui->todoListView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listNamesView->setContextMenuPolicy(Qt::CustomContextMenu);

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

