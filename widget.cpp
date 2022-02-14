#include "widget.h"
#include "ui_widget.h"
#include "about.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    cc = new CacheClean();
    cc->setUp();
    QStringList header;
    header.append("Path");
    header.append("DeleteFolder");
    header.append("Size");
    ui->tableWidget->setHorizontalHeaderLabels(header);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    this->setFixedSize(this->width(),this->height());

    folderList = cc->getFolderList();
    _size = folderList.size();

    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(3);

    for(int i = 0; i<folderList.size(); i++)
        addTabWidgetItem(folderList[i].path,folderList[i].deleteFolder);
    qDebug() << folderList.size();
    ui->tableWidget->setColumnWidth(0,ui->tableWidget->width()*0.65);
    ui->tableWidget->setColumnWidth(1,ui->tableWidget->width()*0.1);

    this->setWindowIcon(QIcon(QStringLiteral(":/ico/mainico")));
}

Widget::~Widget()
{
    delete cc;
    delete ui;
}


void Widget::on_pushButton_about_clicked()
{
    About *about = new About(this);
    about->exec();
    delete about;
}


void Widget::on_pushButton_add_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this,
                                                     QString("Please select a folder"),
                                                     QDir::currentPath(),
                                                     QFileDialog::ShowDirsOnly);
    if(!path.isEmpty())
    {
        addTabWidgetItem(path,false);
        changedFlag = true;
    }
}


void Widget::on_pushButton_change_clicked()
{
    QTableWidgetItem *item = ui->tableWidget->currentItem();
    if(item->column() == 0)
    {
        QString path = QFileDialog::getExistingDirectory(this,
                                                         QString("Please select a folder"),
                                                         QDir::currentPath(),
                                                         QFileDialog::ShowDirsOnly);
        bool flag = false;
        for(int i = 0; i<ui->tableWidget->rowCount(); i++)
        {
            if(ui->tableWidget->item(i,0)->text() == path)
                flag = true;
        }
        if(flag)
            QMessageBox msgBox(QMessageBox::Information,QString("Info:"),QString("The folder is exsiting!"));
        else
            item->setText(path);
    }
    if(item->column() == 1)
    {
        if(item->text() == QString("true"))
            item->setText(QString("false"));
        if(item->text() == QString("false"))
            item->setText(QString("true"));
    }
    changedFlag = true;
}


void Widget::on_pushButton_delete_clicked()
{
    QTableWidgetItem *item = ui->tableWidget->currentItem();
    ui->tableWidget->removeRow(item->row());
    changedFlag = true;
}


void Widget::on_pushButton_save_clicked()
{
    _size = ui->tableWidget->rowCount();
    folderList.clear();
    Folder f;
    for(int i = 0; i<ui->tableWidget->rowCount(); i++)
    {
        f.path = ui->tableWidget->item(i,0)->text();
        if(ui->tableWidget->item(i,1)->text() == QString("true"))
            f.deleteFolder = true;
        if(ui->tableWidget->item(i,1)->text() == QString("false"))
            f.deleteFolder = false;
        folderList.append(f);
    }
    cc->writeCacheFolderConfig(folderList);
    changedFlag = false;
}


void Widget::on_pushButton_start_clicked()
{
    if(changedFlag)
    {
        QMessageBox questionBox(QMessageBox::Question, QString("Info:"),
                                QString("Your settings may not be saved.\nWould you like to save and continue?\nAll cache will be deleted permanently!"),
                                QMessageBox::Yes | QMessageBox::No, this);
        if(questionBox.exec() == QMessageBox::Yes)
        {
            on_pushButton_save_clicked();
            cc->startClean(ui->plainTextEdit);
        }
        else
            cc->startClean(ui->plainTextEdit);
    }
    else
    {
        QMessageBox msgBox(QMessageBox::Information,QString("Confirm:"),
                           QString("All cache will be deleted permanently!"),
                           QMessageBox::Yes | QMessageBox::No, this);
        if(msgBox.exec() == QMessageBox::Yes)
            cc->startClean(ui->plainTextEdit);
    }
}


void Widget::on_pushButton_cancel_clicked()
{
    this->close();
}


void Widget::on_pushButton_reset_clicked()
{
    QMessageBox msgBox(QMessageBox::Warning,QString("Warning:"),
                       QString("Would you like to reset all settings?\nIt will clear all your settings!\n"),
                       QMessageBox::Yes| QMessageBox::No,
                       this);
    if(msgBox.exec() == QMessageBox::Yes)
    {
        cc->reset();
        folderList = cc->getFolderList();
        _size = ui->tableWidget->rowCount();
        qDebug() << _size;
        for(int i = 0; i<_size; i++)
            ui->tableWidget->removeRow(0);

        _size = folderList.size();
        qDebug() << "folderList.size is "<<_size;
        for(int i = 0; i<_size; i++)
            addTabWidgetItem(folderList[i].path,folderList[i].deleteFolder);

    }
}

void Widget::addTabWidgetItem(QString path, bool deleteFolder)
{
    QTableWidgetItem *item0 = new QTableWidgetItem(path);
    QTableWidgetItem *item1;
    QTableWidgetItem *item2 = new QTableWidgetItem(QString("Unknown"));
    if(deleteFolder)
        item1 = new QTableWidgetItem("true");
    else
        item1 = new QTableWidgetItem("false");
    int row = ui->tableWidget->rowCount();

    ui->tableWidget->setRowCount(row+1);
    ui->tableWidget->setItem(row,0,item0);
    ui->tableWidget->setItem(row,1,item1);
    ui->tableWidget->setItem(row,2,item2);
}


void Widget::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    //static QString _content = item->text();
    QString content = item->text();

    if(content == QString("true"))
        item->setText("false");
    if(content == QString("false"))
        item->setText("true");

    qDebug() << "row:" << item->row();
    qDebug() << "col:" << item->column();
}


void Widget::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    qDebug() << "colum:" << item->column();
    qDebug() << "row:" << item->row();
}


void Widget::on_pushButton_scan_clicked()
{
    Folder f;
    _size = ui->tableWidget->rowCount();
    for(int i = 0; i<_size; i++)
    {
        QDir dir(ui->tableWidget->item(i,0)->text());
        if(dir.exists())
        {
            quint64 size = 0;
            foreach(QFileInfo fileInfo, dir.entryInfoList(QDir::Files))
                size += fileInfo.size();
            char unit = 'B';
            quint64 curSize = size;
            if(curSize > 1024)
            {
                curSize /= 1024;
                unit = 'K';
                if(curSize > 1024)
                {
                    curSize /= 1024;
                    unit = 'M';
                    if(curSize > 1024)
                    {
                        curSize /= 1024;
                        unit = 'G';
                    }
                }
            }
            QString text = QString::number(curSize,10);
            text.append(unit);
            ui->tableWidget->item(i,2)->setText(text);
        }
        else
            ui->tableWidget->item(i,2)->setText(QString("Unknown"));

    }
}


void Widget::on_pushButton_clear_clicked()
{
    ui->plainTextEdit->clear();
}

