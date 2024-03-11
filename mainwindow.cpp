#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include "task.h"

void MainWindow::initConnect()
{
    /* todo list */
    connect(ui->addItemBtn, SIGNAL(clicked()), this, SLOT(onAddItemBtnClicked()));
//    connect(m_itemSelcModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onItemSelcChanged(QItemSelection)));
    // todo list right click menu
    connect(ui->todoListView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onRightClickItem()));
    connect(ui->actionDeleteItem, SIGNAL(triggered()), this, SLOT(onActDeleteItem()));
    // todo list item selection
    connect(ui->todoListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(onItemSelcChanged(QItemSelection)));

    /* Task details UI */
    connect(ui->taskNameLineEdit, SIGNAL(editingFinished()), this, SLOT(onTaskNameLineEditFinished()));
    connect(ui->taskReminderChkBox, SIGNAL(toggled(bool)), this, SLOT(onTaskReminderChkBoxToggled(bool)));
    connect(ui->taskDueChkBox, SIGNAL(toggled(bool)), this, SLOT(onTaskDueChkBoxToggled(bool)));
    connect(ui->taskReminderDateTimeEdit, SIGNAL(editingFinished()), this, SLOT(onTaskReminderDateTimeEditFinished()));
    connect(ui->taskDueDateEdit, SIGNAL(editingFinished()), this, SLOT(onTaskDueDateEditFinished()));

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
        Task* task = new Task(newTodo);     // TODO: how to delete, manually, smart pointer or use QObject ?
        QVariant variant;
        variant.setValue(task);
        item->setData(variant);
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

        disableTaskDetailsUi();
    }
}

void MainWindow::onItemSelcChanged(const QItemSelection &selected)
{
    QModelIndexList selectedIndexes = selected.indexes();
    if(selectedIndexes.empty())
    {
        disableTaskDetailsUi();
    }
    else
    {
        enableTaskDetailsUi();
        Task* task = m_itemModel->data(selectedIndexes[0], Qt::UserRole+1).value<Task *>();
        ui->taskNameLineEdit->setText(task->name());
        ui->taskReminderChkBox->setChecked(task->enableReminder());
        if(task->enableReminder())
        {
            if(task->reminder().isValid()) ui->taskReminderDateTimeEdit->setDateTime(task->reminder());
        }
        ui->taskDueChkBox->setChecked(task->enableDue());
        if(task->enableDue())
        {
            if(task->due().isValid()) ui->taskDueDateEdit->setDate(task->due());
        }
        ui->taskCommentTextEdit->setText(task->comment());

        qDebug() << "onItemSelcChanged: task name: " << task->name();
    }


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

void MainWindow::onTaskReminderChkBoxToggled(bool checked)
{
    ui->taskReminderDateTimeEdit->setEnabled(checked);
    QModelIndexList indexes = m_itemSelcModel->selectedIndexes();
    if(!indexes.empty())
    {
        Task* task = m_itemModel->data(indexes[0], Qt::UserRole+1).value<Task*>();
        task->setEnableReminder(checked);
    }
}

void MainWindow::onTaskDueChkBoxToggled(bool checked)
{
    ui->taskDueDateEdit->setEnabled(checked);
    QModelIndexList indexes = m_itemSelcModel->selectedIndexes();
    if(!indexes.empty())
    {
        Task* task = m_itemModel->data(indexes[0], Qt::UserRole+1).value<Task*>();
        task->setEnableDue(checked);
    }
}

void MainWindow::onTaskNameLineEditFinished()
{
    QModelIndexList indexes = m_itemSelcModel->selectedIndexes();   // this will still get the index for the item that is being modified, not the next selected item
    if(!indexes.empty())
    {
        Task* task = m_itemModel->data(indexes[0], Qt::UserRole+1).value<Task*>();
        QString newName = ui->taskNameLineEdit->text();
        if(!newName.isEmpty())
        {
            task->setName(newName);
            m_itemModel->setData(indexes[0], newName, Qt::DisplayRole);
        }
        qDebug() << "onTaskNameLineEditFinished: " << newName;
    }
}

void MainWindow::onTaskReminderDateTimeEditFinished()
{
    QModelIndexList indexes = m_itemSelcModel->selectedIndexes();
    if(!indexes.empty())
    {
        Task* task = m_itemModel->data(indexes[0], Qt::UserRole+1).value<Task*>();
        QDateTime dateTime = ui->taskReminderDateTimeEdit->dateTime();
        if(dateTime.isValid())
        {
            task->setReminder(dateTime);
        }
    }
}

void MainWindow::onTaskDueDateEditFinished()
{
    QModelIndexList indexes = m_itemSelcModel->selectedIndexes();
    if(!indexes.empty())
    {
        Task* task = m_itemModel->data(indexes[0], Qt::UserRole+1).value<Task*>();
        QDate date = ui->taskDueDateEdit->date();
        if(date.isValid())
        {
            task->setDue(date);
        }
    }
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

void MainWindow::enableTaskDetailsUi()
{
    ui->taskNameLineEdit->setEnabled(true);
    ui->taskReminderChkBox->setEnabled(true);
    ui->taskReminderDateTimeEdit->setEnabled(ui->taskReminderChkBox->isChecked());
    ui->taskDueChkBox->setEnabled(true);
    ui->taskDueDateEdit->setEnabled(ui->taskReminderChkBox->isChecked());
    ui->taskCommentTextEdit->setEnabled(true);
}

void MainWindow::disableTaskDetailsUi()
{
    ui->taskNameLineEdit->setEnabled(false);
    ui->taskReminderChkBox->setEnabled(false);
    ui->taskReminderDateTimeEdit->setEnabled(false);
    ui->taskDueChkBox->setEnabled(false);
    ui->taskDueDateEdit->setEnabled(false);
    ui->taskCommentTextEdit->setEnabled(false);
}

void MainWindow::clearTaskDetailsUiContent()
{
    ui->taskNameLineEdit->clear();
    ui->taskReminderChkBox->setCheckState(Qt::Unchecked);
    ui->taskReminderDateTimeEdit->clear();
    ui->taskDueChkBox->setCheckState(Qt::Unchecked);
    ui->taskDueDateEdit->clear();
    ui->taskCommentTextEdit->clear();
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
    disableTaskDetailsUi();
    clearTaskDetailsUiContent();
}

MainWindow::~MainWindow()
{
    delete ui;
}

