#ifndef DEBUG_H
#define DEBUG_H

#include <QMessageBox>

#include "baseType.h"

class Debug
{
public:
    Debug();

    void static alert(QString content, QString title = QString("Info"), QWidget *widget = nullptr);
    void static alert(QVector<Folder> content);
    void static alert(QJsonObject content);
};

#endif // DEBUG_H
