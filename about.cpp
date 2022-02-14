#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    ui->label->setWordWrap(true);
    this->setFixedSize(452,264);
}

About::~About()
{
    delete ui;
}
