#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include "task.h"
#include "tasklistmodel.h"
#include "TaskDataRoles.h"
#include <QFileInfo>
#include <QDir>
#include "taskviewdelegate.h"

void MainWindow::initUiConnect()
{
    /* task list */
    // add task
    connect(ui->addItemBtn, &QPushButton::clicked, this, &MainWindow::onAddItemBtnClicked);
    // task list right click menu
    connect(ui->taskListView, &QListView::customContextMenuRequested, this, &MainWindow::onRightClickInTodoList);
    // context menu: delete
    connect(ui->actionDeleteItem, &QAction::triggered, this, &MainWindow::onActDeleteItem);


    /* Task details UI */
    connect(ui->taskNameLineEdit, &QLineEdit::editingFinished, this, &MainWindow::onTaskNameLineEditFinished);
    connect(ui->taskReminderChkBox, &QCheckBox::toggled, this, &MainWindow::onTaskReminderChkBoxToggled);
    connect(ui->taskDueChkBox, &QCheckBox::toggled, this, &MainWindow::onTaskDueChkBoxToggled);
    connect(ui->taskReminderDateTimeEdit, &QDateTimeEdit::editingFinished, this, &MainWindow::onTaskReminderDateTimeEditFinished);
    connect(ui->taskDueDateEdit, &QDateEdit::editingFinished, this, &MainWindow::onTaskDueDateEditFinished);

    /* list of task lists*/
    // add list
    connect(ui->addListBtn, &QPushButton::clicked, this, &MainWindow::onAddListBtnClicked);
    // lists right click menu
    connect(ui->listNamesView, &QListView::customContextMenuRequested, this, &MainWindow::onRightClickInListName);
    // remove list
    connect(ui->actionDeleteList, &QAction::triggered, this, &MainWindow::onActDeleteList);
    // list rename
    connect(ui->listNameLineEdit, &QLineEdit::editingFinished, this, &MainWindow::onListNameLineEditFinished);
}

void MainWindow::onAddItemBtnClicked()
{
    QString newTaskName = ui->newTodoEdit->text();
    if(!newTaskName.isEmpty())
    {
        m_taskListModel->addTask(newTaskName);
        // TODO: save data at a proper time
        m_taskListModel->saveModel();
        ui->newTodoEdit->clear();
    }
}

void MainWindow::onListNameSelcChanged(const QItemSelection &selected)
{
    QModelIndexList indexes = selected.indexes();
    if(!indexes.empty() && indexes[0].isValid())
    {
        QString listName = m_listNamesModel->data(indexes[0]).toString();
        qDebug() << listName;
        m_taskListModel = m_nameToListMap[listName];
        m_taskListSelcModel->setModel(m_taskListModel);
        ui->taskListView->setModel(m_taskListModel);
        ui->taskListView->setSelectionModel(m_taskListSelcModel);
        ui->listNameLineEdit->setText(listName);
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

        ui->taskNameLineEdit->setText(m_taskListModel->data(index, TaskNameRole).toString());

        bool enableReminder = m_taskListModel->data(index, TaskEnableReminderRole).toBool();
        ui->taskReminderChkBox->setChecked(enableReminder);

        if(enableReminder)
            ui->taskReminderDateTimeEdit->setDateTime(m_taskListModel->data(index, TaskReminderRole).toDateTime());

        bool enableDue = m_taskListModel->data(index, TaskEnableDueRole).toBool();
        ui->taskDueChkBox->setChecked(enableDue);

        if(enableDue)
            ui->taskDueDateEdit->setDate(m_taskListModel->data(index, TaskDueRole).toDate());

        ui->taskCommentTextEdit->setText(m_taskListModel->data(index, TaskCommentRole).toString());

        enableTaskDetailsUi();

        qDebug() << "onItemSelcChanged: task name: " << m_taskListModel->data(index, TaskNameRole).toString();
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
        m_nameToListMap[listName] = new TaskListModel(listName, this);
    }

}

void MainWindow::onRightClickInListName(const QPoint &pos)
{
    if(ui->listNamesView->indexAt(pos).isValid())
    {
        QMenu* menuList = new QMenu(this);
        menuList->addAction(ui->actionDeleteList);
        menuList->exec(QCursor::pos());
        delete menuList;
    }
}

void MainWindow::onActDeleteList()
{
    QModelIndexList indexes = m_listNameSelcModel->selectedIndexes();
    if(!indexes.empty() && indexes[0].isValid())
    {
        QString listName = m_listNamesModel->data(indexes[0]).toString();
        TaskListModel* taskListModel = m_nameToListMap[listName];
        delete taskListModel;
        m_listNamesModel->removeRow(indexes[0].row());
        m_nameToListMap.remove(listName);
    }
}

void MainWindow::onTaskReminderChkBoxToggled(bool checked)
{
    QModelIndexList indexes = m_taskListSelcModel->selectedIndexes();
    if(!indexes.empty() && indexes[0].isValid())
    {
        m_taskListModel->setData(indexes[0], QVariant(checked), TaskEnableReminderRole);
        ui->taskReminderDateTimeEdit->setEnabled(checked);
        if(checked)
        {
            QDateTime reminder = m_taskListModel->data(indexes[0], TaskReminderRole).toDateTime();
            if(!reminder.isValid())
            {
                reminder = ui->taskReminderDateTimeEdit->dateTime();
                m_taskListModel->setData(indexes[0], QVariant(reminder), TaskReminderRole);
            }
            ui->taskReminderDateTimeEdit->setDateTime(reminder);
        }
        else
        {
            m_taskListModel->setData(indexes[0], QVariant(), TaskReminderRole);
        }
    }
}

void MainWindow::onTaskDueChkBoxToggled(bool checked)
{
    QModelIndexList indexes = m_taskListSelcModel->selectedIndexes();
    if(!indexes.empty() && indexes[0].isValid())
    {
        m_taskListModel->setData(indexes[0], QVariant(checked), TaskEnableDueRole);
        ui->taskDueDateEdit->setEnabled(checked);
        if(checked)
        {
            QDate due = m_taskListModel->data(indexes[0], TaskDueRole).toDate();
            if(!due.isValid())
            {
                due = ui->taskDueDateEdit->date();
                m_taskListModel->setData(indexes[0], QVariant(due), TaskDueRole);
            }
            ui->taskDueDateEdit->setDate(due);
        }
        else
        {
            m_taskListModel->setData(indexes[0], QVariant(), TaskDueRole);
        }
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
        qDebug() << "reminder edit finish, row: " << indexes[0].row();
        QDateTime dateTime = ui->taskReminderDateTimeEdit->dateTime();
        if(dateTime.isValid())
        {
            m_taskListModel->setData(indexes[0], QVariant(dateTime), TaskReminderRole);
        }
    }
}

void MainWindow::onTaskDueDateEditFinished()
{
    QModelIndexList indexes = m_taskListSelcModel->selectedIndexes();
    if(!indexes.empty() && indexes[0].isValid())
    {
        qDebug() << "due edit finish, row: " << indexes[0].row();
        QDate date = ui->taskDueDateEdit->date();
        if(date.isValid())
        {
            m_taskListModel->setData(indexes[0], QVariant(date), TaskDueRole);
        }
    }
}

void MainWindow::onListNamesRowsInserted()
{
    if(m_listNamesModel->rowCount() > 0)
    {
        ui->taskListView->setEnabled(true);
    }
}

void MainWindow::onListNamesRowsRemoved()
{
    if(m_listNamesModel->rowCount() == 0)
    {
        ui->taskListView->setEnabled(false);
    }
}

void MainWindow::onListNameLineEditFinished()
{
    QString newListName = ui->listNameLineEdit->text();
    QModelIndexList indexes = m_listNameSelcModel->selectedIndexes();
    QString listName = m_listNamesModel->itemFromIndex(indexes[0])->text();
    if(newListName != listName)
    {
        m_listNamesModel->setItemData(indexes[0], QMap<int, QVariant>({{Qt::DisplayRole,QVariant(newListName)}}));
        m_nameToListMap[listName]->setListName(newListName);
        m_nameToListMap[newListName] = m_nameToListMap[listName];
        m_nameToListMap.remove(listName);

        m_nameToListMap[newListName]->saveModel();

        QFile file("data/"+listName+".dat");
        if(file.exists())
        {
            file.remove();
        }

    }
}

void MainWindow::initLists()
{
    // in the future they should be loaded from disk

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

void MainWindow::loadData()
{
    bool dataExists = false;
    QDir dir("data");
    if(!dir.exists())
    {
        QDir().mkdir("data");
    }

    QStringList files = dir.entryList(QDir::Files);
    if(files.empty())
    {
        m_nameToListMap["my day"] = new TaskListModel("my day", this);
        m_listNamesModel->appendRow(new QStandardItem("my day"));
        return;
    }

    for(const QString& fileName : files)
    {
        qDebug() << "fileName: " << fileName;

        QString fileExtension = fileName.right(4);
        if(fileExtension != ".dat")
        {
            continue;
        }

        QString listName = fileName.left(fileName.lastIndexOf("."));
        m_nameToListMap[listName] = new TaskListModel(listName, this);

        // load data from file to list model
        QFile file("data/"+fileName);
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);
        m_nameToListMap[listName]->loadModel(in);
        file.close();

        m_listNamesModel->appendRow(new QStandardItem(listName));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    for(TaskListModel* taskListModel: m_nameToListMap.values())
    {
        taskListModel->saveModel();
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->listNamesView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->taskListView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->taskListView->setItemDelegate(new TaskViewDelegate(this));

    // listNamesView
    m_listNamesModel = new QStandardItemModel(this);
    m_listNamesModel->setColumnCount(1);
    m_listNameSelcModel = new QItemSelectionModel(m_listNamesModel, this);
    ui->listNamesView->setModel(m_listNamesModel);
    ui->listNamesView->setSelectionModel(m_listNameSelcModel);

    // when list selection change
    connect(m_listNameSelcModel, &QItemSelectionModel::selectionChanged, this, &MainWindow::onListNameSelcChanged);

    // when list added/removed
    connect(m_listNamesModel, &QStandardItemModel::rowsInserted, this, &MainWindow::onListNamesRowsInserted);
    connect(m_listNamesModel, &QStandardItemModel::rowsRemoved, this, &MainWindow::onListNamesRowsRemoved);

    loadData();

    initUiConnect();

    m_taskListModel = m_nameToListMap[m_listNamesModel->item(0)->data(Qt::DisplayRole).toString()];
    m_taskListSelcModel = new QItemSelectionModel(m_taskListModel, this);
    ui->taskListView->setModel(m_taskListModel);
    ui->taskListView->setSelectionModel(m_taskListSelcModel);
    // first select when start
    m_listNameSelcModel->select(m_listNamesModel->index(0,0), QItemSelectionModel::SelectCurrent);

    // when task selection change
    connect(m_taskListSelcModel, &QItemSelectionModel::selectionChanged, this, &MainWindow::onItemSelcChanged);

    disableTaskDetailsUi();
}

MainWindow::~MainWindow()
{
    delete ui;
}

