#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include "task.h"
#include "tasklistmodel.h"
#include "TaskDataRoles.h"

void MainWindow::initConnect()
{
    /* task list */
    // add task
    connect(ui->addItemBtn, &QPushButton::clicked, this, &MainWindow::onAddItemBtnClicked);
    // selection change
    connect(m_taskListSelcModel, &QItemSelectionModel::selectionChanged, this, &MainWindow::onItemSelcChanged);
    // task list right click menu
    connect(ui->taskListView, &QListView::customContextMenuRequested, this, &MainWindow::onRightClickInTodoList);
    // context menu: delete
    connect(ui->actionDeleteItem, &QAction::triggered, this, &MainWindow::onActDeleteItem);


    /* Task details UI */
    connect(ui->taskNameLineEdit, &QLineEdit::editingFinished, this, &MainWindow::onTaskNameLineEditFinished);
    connect(ui->taskReminderChkBox, &QCheckBox::toggled, this, &MainWindow::onTaskReminderChkBoxToggled);
    connect(ui->taskDueChkBox, &QCheckBox::toggled, this, &MainWindow::onTaskDueChkBoxToggled);
    connect(ui->taskReminderDateTimeEdit, &QDateTimeEdit::editingFinished, this, &MainWindow::onTaskReminderDateTimeEditFinished);
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
    QString newTaskName = ui->newTodoEdit->text();
    if(!newTaskName.isEmpty())
    {
        m_taskListModel->addTask(newTaskName);
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
        QModelIndex index = selectedIndexes[0];

        ui->taskNameLineEdit->setText(m_taskListModel->data(index, Qt::DisplayRole).toString());

        bool enableReminder = m_taskListModel->data(index, TaskEnableReminderRole).toBool();
        ui->taskReminderChkBox->setChecked(enableReminder);

        if(enableReminder)
        {
            QDateTime reminder = m_taskListModel->data(index, TaskReminderRole).toDateTime();
            if(reminder.isValid()) ui->taskReminderDateTimeEdit->setDateTime(reminder);
        }

        bool enableDue = m_taskListModel->data(index, TaskEnableDueRole).toBool();
        ui->taskDueChkBox->setChecked(enableDue);

        if(enableDue)
        {
            QDate due = m_taskListModel->data(index, TaskDueRole).toDate();
            if(due.isValid()) ui->taskDueDateEdit->setDate(due);
        }

        ui->taskCommentTextEdit->setText(m_taskListModel->data(index, TaskCommentRole).toString());

        enableTaskDetailsUi();

        qDebug() << "onItemSelcChanged: task name: " << m_taskListModel->data(index, Qt::DisplayRole).toString();
    }

}

void MainWindow::onActDeleteItem()
{
    QModelIndexList indexes = m_taskListSelcModel->selectedIndexes();
    if(!indexes.empty() && indexes[0].isValid())
    {
        m_taskListModel->removeTask(indexes[0]);
    }
}

void MainWindow::onRightClickInTodoList(const QPoint &pos)
{
    if(ui->taskListView->indexAt(pos).isValid())
    {
        QMenu* menuList= new QMenu(this);
        menuList->addAction(ui->actionDeleteItem);
        menuList->exec(QCursor::pos());
        delete menuList;
    }
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
    QModelIndexList indexes = m_taskListSelcModel->selectedIndexes();
    if(!indexes.empty() && indexes[0].isValid())
    {
        m_taskListModel->setData(indexes[0], QVariant(checked), TaskEnableReminderRole);
        ui->taskReminderDateTimeEdit->setEnabled(checked);
    }
}

void MainWindow::onTaskDueChkBoxToggled(bool checked)
{

    QModelIndexList indexes = m_taskListSelcModel->selectedIndexes();
    if(!indexes.empty() && indexes[0].isValid())
    {
        m_taskListModel->setData(indexes[0], QVariant(checked), TaskEnableDueRole);
        ui->taskDueDateEdit->setEnabled(checked);
    }
}

void MainWindow::onTaskNameLineEditFinished()
{
    QModelIndexList indexes = m_taskListSelcModel->selectedIndexes();   // this will still get the index for the item that is being modified, not the next selected item
    if(!indexes.empty() && indexes[0].isValid())
    {
        QString newName = ui->taskNameLineEdit->text();
        if(!newName.isEmpty())
        {
            m_taskListModel->setData(indexes[0], QVariant(newName), Qt::DisplayRole);
            qDebug() << "onTaskNameLineEditFinished: " << newName;
        }
    }
}

void MainWindow::onTaskReminderDateTimeEditFinished()
{
    QModelIndexList indexes = m_taskListSelcModel->selectedIndexes();
    if(!indexes.empty() && indexes[0].isValid())
    {
        QDateTime dateTime = ui->taskReminderDateTimeEdit->dateTime();
        if(dateTime.isValid())
        {
            m_taskListModel->setData(indexes[0], QVariant(dateTime), TaskReminderRole);
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
    ui->taskDueDateEdit->setEnabled(ui->taskDueChkBox->isChecked());
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
    ui->listNamesView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->taskListView->setContextMenuPolicy(Qt::CustomContextMenu);

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

    // taskListView
    m_taskListModel = new TaskListModel(this);
    m_taskListSelcModel = new QItemSelectionModel(m_taskListModel, this);
    ui->taskListView->setModel(m_taskListModel);
    ui->taskListView->setSelectionModel(m_taskListSelcModel);


    initConnect();
    initLists();
    disableTaskDetailsUi();
    clearTaskDetailsUiContent();
}

MainWindow::~MainWindow()
{
    delete ui;
}

