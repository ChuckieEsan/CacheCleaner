#ifndef BASETYPE_H
#define BASETYPE_H

#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QDebug>
#include <QFileDialog>

#define DEFAULT_SIZE 4

struct Folder
{
    QStringList fileList;
    bool deleteFolder = false;
    QString path;
};


#endif // BASETYPE_H
