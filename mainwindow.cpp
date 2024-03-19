#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimeZone>

#include "task.h"

void MainWindow::initConnect()
{
    /* task list */
    connect(ui->addItemBtn, SIGNAL(clicked()), this, SLOT(onAddItemBtnClicked()));
    // task list right click menu
    connect(ui->taskTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onRightClickInTaskList(QPoint)));
    connect(ui->actionDeleteItem, SIGNAL(triggered()), this, SLOT(onActDeleteItem()));
    // task list item selection
    connect(m_itemSelcModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(onItemSelcChanged(QItemSelection)));


    /* Task details UI */
    connect(ui->taskNameLineEdit, SIGNAL(editingFinished()), this, SLOT(onTaskNameLineEditFinished()));
    connect(ui->taskReminderChkBox, SIGNAL(toggled(bool)), this, SLOT(onTaskReminderChkBoxToggled(bool)));
    connect(ui->taskDueChkBox, SIGNAL(toggled(bool)), this, SLOT(onTaskDueChkBoxToggled(bool)));
    connect(ui->taskReminderDateTimeEdit, SIGNAL(editingFinished()), this, SLOT(onTaskReminderDateTimeEditFinished()));
    connect(ui->taskDueDateEdit, SIGNAL(editingFinished()), this, SLOT(onTaskDueDateEditFinished()));

    /* lists of task list*/
    connect(ui->addListBtn, SIGNAL(clicked()), this, SLOT(onAddListBtnClicked()));
    connect(m_listNameSelcModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onListNameSelcChanged(QItemSelection)));
    // lists right click menu
    connect(ui->listNamesView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onRightClickInListNameList(QPoint)));
    connect(ui->actionDeleteList, SIGNAL(triggered(bool)), this, SLOT(onActDeleteList()));
}

void MainWindow::onAddItemBtnClicked()
{
    QString newTaskName = ui->newTodoEdit->text();
    if(!newTaskName.isEmpty())
    {
        QString listName = m_listNamesModel->data(m_listNameSelcModel->currentIndex()).toString();
        QString queryString = QString("INSERT INTO \"%1\" (name) VALUES ('%2')").arg(listName, newTaskName);
        QSqlQuery query(queryString, m_db);
        if(query.lastError().isValid())
        {
            qDebug() << query.lastError().text();
        }
        else
        {
            m_queryModel->setQuery(std::move(query));
        }

        ui->newTodoEdit->clear();
    }
}

void MainWindow::onListNameSelcChanged(const QItemSelection &selected)
{
    QModelIndexList selectedIndexes = selected.indexes();
    if(!selected.empty())
    {
        QString listName = m_listNamesModel->data(selectedIndexes[0]).toString();
        qDebug() << "list name: " << listName;

        /*
        QSqlQuery::prepare does not support place holder like
        "SELECT name, id FROM :list"
        */
        QString queryString = QString("SELECT name, id FROM \"%1\"").arg(listName);

        QSqlQuery query(queryString, m_db);
        if(query.lastError().isValid())
        {
            qDebug() << query.lastError().text();
        }

        m_queryModel->setQuery(std::move(query));

        // when there is data in the view then the following settings can work; not work when initialized with empty data
        ui->taskTableView->hideColumn(1);
        ui->taskTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);   // one culumn use entire width

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
        QSqlRecord record = m_queryModel->record(selectedIndexes[0].row());

        if(record.value("id").isNull())
        {
            qDebug() << "no id info";
        }
        else
        {
            QVariant id = record.value("id");
            qDebug() << id;

            QString listName = m_listNamesModel->data(m_listNameSelcModel->currentIndex()).toString();
            QString queryString = QString("SELECT * FROM \"%1\" WHERE id = %2").arg(listName, id.toString());
            QSqlQuery query(queryString, m_db);
            if(query.lastError().isValid())
            {
                qDebug() << query.lastError().text();
            }
            else
            {
                query.first();

                QSqlRecord detailedRec = query.record();

                ui->taskNameLineEdit->setText(detailedRec.value("name").toString());

                ui->taskDueChkBox->setChecked(detailedRec.value("enable_due").toBool());

                QDate dueDate = detailedRec.value("due").toDate();   // only accept Qt::ISODate format
                if(dueDate.isValid()) ui->taskDueDateEdit->setDate(dueDate);

                ui->taskReminderChkBox->setChecked(detailedRec.value("enable_reminder").toBool());

                QDateTime reminderDateTime = detailedRec.value("reminder").toDateTime();    // only accept Qt::ISODateTime format
                if(reminderDateTime.isValid()) ui->taskReminderDateTimeEdit->setDateTime(reminderDateTime);

                ui->taskCommentTextEdit->setText(detailedRec.value("comment").toString());

                qDebug() << "Completed" << detailedRec.value("completed").toBool();

                enableTaskDetailsUi();
            }
        }
    }
}

void MainWindow::onActDeleteItem()
{
    QSqlRecord record = m_queryModel->record(m_itemSelcModel->currentIndex().row());
    QVariant id = record.value("id");
    if(id.isValid())
    {
        QString listName = m_listNamesModel->data(m_listNameSelcModel->currentIndex()).toString();
        QString queryString = QString("DELETE FROM \"%1\" WHERE id = %2").arg(listName, id.toString());
        QSqlQuery query(queryString, m_db);
        if(query.lastError().isValid())
        {
            qDebug() << query.lastError().text();
        }
        else
        {
            m_queryModel->setQuery(std::move(query));
        }
    }
}

void MainWindow::onRightClickInTaskList(const QPoint &pos)
{
    if(ui->taskTableView->indexAt(pos).isValid())
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
        bool ok = false;
        listName = QInputDialog::getText(this, dlgTitle, txtLabel, QLineEdit::Normal, listName, &ok);
        if(!ok)
        {
            break;
        }
        if (ok && !listName.isEmpty())
        {
            QStringList tables = m_db.tables();
            if(!tables.contains(listName))
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
        QString queryString = QString("CREATE TABLE \"%1\" ("
                              "name TEXT NOT NULL,"
                              "id INTEGER,"
                              "enable_due INTEGER NOT NULL DEFAULT 0,"
                              "enable_reminder INTEGER NOT NULL DEFAULT 0,"
                              "due TEXT,"
                              "reminder TEXT,"
                              "completed INTEGER DEFAULT 0,"
                              "comment TEXT,"
                              "PRIMARY KEY(id AUTOINCREMENT)"
                                      ");").arg(listName);
        QSqlQuery query(queryString, m_db);
        if(query.lastError().isValid())
        {
            qDebug() << query.lastError().text();
        }
        else
        {
            QStandardItem* item = new QStandardItem(listName);
            m_listNamesModel->appendRow(item);
        }

    }

}

void MainWindow::onRightClickInListNameList(const QPoint &pos)
{
    if(ui->listNamesView->indexAt(pos).isValid())
    {
        QMenu* menuList= new QMenu(this);
        menuList->addAction(ui->actionDeleteList);
        menuList->exec(QCursor::pos());
        delete menuList;
    }
}

void MainWindow::onActDeleteList()
{
    QString listName = m_listNamesModel->data(m_listNameSelcModel->currentIndex()).toString();
    QString queryString = QString("DROP TABLE \"%1\"").arg(listName);
    QSqlQuery query(queryString, m_db);
    if(query.lastError().isValid())
    {
        qDebug() << query.lastError().text();
    }
    else
    {
        int currentRow = m_listNameSelcModel->currentIndex().row();
        m_listNamesModel->removeRow(currentRow);
    }
}

void MainWindow::onTaskReminderChkBoxToggled(bool checked)
{
    QSqlRecord record = m_queryModel->record(m_itemSelcModel->currentIndex().row());
    QVariant id = record.value("id");
    if(id.isValid())
    {
        QString listName = m_listNamesModel->data(m_listNameSelcModel->currentIndex()).toString();
        QString queryString = QString("SELECT enable_reminder FROM \"%1\" WHERE id = %2").arg(listName, id.toString());
        QSqlQuery query(queryString, m_db);
        if(query.lastError().isValid())
        {
            qDebug() << query.lastError().text();
        }
        else
        {
            query.first();
            record = query.record();
            bool originalEnableDue = record.value("enable_reminder").toBool();
            if(originalEnableDue != checked)
            {
                queryString = QString("UPDATE \"%1\" SET enable_reminder = %2 WHERE id = %3").arg(listName, QString::number(checked), id.toString());
                QSqlQuery updateQuery(queryString, m_db);
                if(updateQuery.lastError().isValid())
                {
                    qDebug() << query.lastError().text();
                }
                else
                {
                    m_queryModel->setQuery(std::move(updateQuery));
                    ui->taskReminderDateTimeEdit->setEnabled(checked);
                    qDebug() << "updated enable_reminder";
                }
            }
        }
    }
}

void MainWindow::onTaskDueChkBoxToggled(bool checked)
{
    QSqlRecord record = m_queryModel->record(m_itemSelcModel->currentIndex().row());
    QVariant id = record.value("id");
    if(id.isValid())
    {
        QString listName = m_listNamesModel->data(m_listNameSelcModel->currentIndex()).toString();
        QString queryString = QString("SELECT enable_due FROM \"%1\" WHERE id = %2").arg(listName, id.toString());
        QSqlQuery query(queryString, m_db);
        if(query.lastError().isValid())
        {
            qDebug() << query.lastError().text();
        }
        else
        {
            query.first();
            record = query.record();
            bool originalEnableDue = record.value("enable_due").toBool();
            if(originalEnableDue != checked)
            {
                queryString = QString("UPDATE \"%1\" SET enable_due = %2 WHERE id = %3").arg(listName, QString::number(checked), id.toString());
                QSqlQuery updateQuery(queryString, m_db);
                if(updateQuery.lastError().isValid())
                {
                    qDebug() << query.lastError().text();
                }
                else
                {
                    m_queryModel->setQuery(std::move(updateQuery));
                    ui->taskDueDateEdit->setEnabled(checked);
                    qDebug() << "updated enable_due";
                }
            }
        }
    }
}

void MainWindow::onTaskNameLineEditFinished()
{
    QModelIndexList indexes = m_itemSelcModel->selectedIndexes();   // this will still get the index for the item that is being modified, not the next selected item
    if(!indexes.empty())
    {
        QSqlRecord record = m_queryModel->record(m_itemSelcModel->currentIndex().row());
        QVariant id = record.value("id");
        if(id.isValid())
        {
            QString listName = m_listNamesModel->data(m_listNameSelcModel->currentIndex()).toString();
            QString newTaskName = ui->taskNameLineEdit->text();
            QString queryString = QString("UPDATE \"%1\" SET name = '%2' WHERE id = %3").arg(listName, newTaskName, id.toString());
            QSqlQuery query(queryString, m_db);
            if(query.lastError().isValid())
            {
                qDebug() << query.lastError().text();
            }
            else
            {
                m_queryModel->setQuery(std::move(query));
                qDebug() << "onTaskNameLineEditFinished: " << newTaskName;
            }
        }
    }
}

void MainWindow::onTaskReminderDateTimeEditFinished()
{
    QSqlRecord record = m_queryModel->record(m_itemSelcModel->currentIndex().row());
    QVariant id = record.value("id");
    if(id.isValid())
    {
        QString listName = m_listNamesModel->data(m_listNameSelcModel->currentIndex()).toString();
        QString queryString = QString("SELECT reminder FROM \"%1\" WHERE id = %2").arg(listName, id.toString());
        QSqlQuery query(queryString, m_db);
        if(query.lastError().isValid())
        {
            qDebug() << query.lastError().text();
        }
        else
        {
            query.first();
            record = query.record();
            QDateTime currentReminder = ui->taskReminderDateTimeEdit->dateTime();
            currentReminder.setTimeZone(QTimeZone::systemTimeZone());
            qDebug() << currentReminder.toString(Qt::ISODate);
            QDateTime previousReminder = record.value("reminder").toDateTime();
            if(!previousReminder.isValid() || previousReminder != currentReminder)
            {
                queryString = QString("UPDATE \"%1\" SET reminder = '%2' WHERE id = %3").arg(listName, currentReminder.toString(Qt::ISODate), id.toString());
                if(!query.exec(queryString))
                {
                    qDebug() << query.lastError().text();
                }

            }
        }
    }


}

void MainWindow::onTaskDueDateEditFinished()
{
    QSqlRecord record = m_queryModel->record(m_itemSelcModel->currentIndex().row());
    QVariant id = record.value("id");
    if(id.isValid())
    {
        QString listName = m_listNamesModel->data(m_listNameSelcModel->currentIndex()).toString();
        QString queryString = QString("SELECT due FROM \"%1\" WHERE id = %2").arg(listName, id.toString());
        QSqlQuery query(queryString, m_db);
        if(query.lastError().isValid())
        {
            qDebug() << query.lastError().text();
        }
        else
        {
            query.first();
            record = query.record();
            QDate currentDue = ui->taskDueDateEdit->date();
            qDebug() << currentDue.toString(Qt::ISODate);
            QDate previousDue = record.value("due").toDate();
            if(!previousDue.isValid() || previousDue != currentDue)
            {
                queryString = QString("UPDATE \"%1\" SET due = '%2' WHERE id = %3").arg(listName, currentDue.toString(Qt::ISODate), id.toString());
                if(!query.exec(queryString))
                {
                    qDebug() << query.lastError().text();
                }
            }
        }
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

    ui->taskTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listNamesView->setContextMenuPolicy(Qt::CustomContextMenu);

    // listNamesView
    m_listNamesModel = new QStandardItemModel(this);
    m_listNamesModel->setColumnCount(1);
    m_listNameSelcModel = new QItemSelectionModel(m_listNamesModel, this);
    ui->listNamesView->setModel(m_listNamesModel);
    ui->listNamesView->setSelectionModel(m_listNameSelcModel);

    disableTaskDetailsUi();
    clearTaskDetailsUiContent();

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("../Tokie/data/tasks.db"); // how to deal with the path when release?
    if(!m_db.open())
    {
        qDebug() << "failed to open db";
    }
    else
    {
        QStringList tables = m_db.tables();
        for(const auto& table: tables)
        {
            if(table != "sqlite_sequence")
            {
                QStandardItem* item = new QStandardItem(table);
                m_listNamesModel->appendRow(item);
            }
            qDebug() << "Table name: " << table;
        }
        m_queryModel = new QSqlQueryModel(this);

        ui->taskTableView->setModel(m_queryModel);

        m_itemSelcModel = new QItemSelectionModel(m_queryModel, this);
        ui->taskTableView->setSelectionModel(m_itemSelcModel);

        // TODO: after set new query for queryModel, the view is empty, how to solve?
    }

    initConnect();
}

MainWindow::~MainWindow()
{
    delete ui;
}

