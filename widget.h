#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QTableWidgetItem>
#include <QFont>

#include "cacheclean.h"
#include "baseType.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    CacheClean *cc;
    QVector<Folder> folderList;
    bool changedFlag = false;

private slots:
    void on_pushButton_about_clicked();
    void on_pushButton_add_clicked();
    void on_pushButton_change_clicked();
    void on_pushButton_delete_clicked();
    void on_pushButton_save_clicked();
    void on_pushButton_start_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_reset_clicked();
    void on_pushButton_scan_clicked();

    void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);
    void on_tableWidget_itemClicked(QTableWidgetItem *item);



    void on_pushButton_clear_clicked();

private:
    Ui::Widget *ui;
    int _size = 0;

    void addTabWidgetItem(QString path, bool deleteFolder);


};
#endif // WIDGET_H
