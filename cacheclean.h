#ifndef CACHECLEAN_H
#define CACHECLEAN_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QPlainTextEdit>

#include "baseType.h"
#include "debug.h"

class CacheClean
{
public:
    explicit CacheClean();

    ~CacheClean();

    QVector<Folder> getFolderList();
    void writeCacheFolderConfig(const QVector<Folder> folders);

    void addCacheFolder(const QString folderPath, const bool deleteFolder = false);
    //assert: this method should be abandoned
    void removeCacheFolder(const QString folderPath);
    //assert: this method should be abandoned
    void updateCacheFolder(const QString folderPath, const bool deleteFolder = false);
    //assert: this method should be abandoned

    void startClean(QPlainTextEdit *logReceiver);
    void setUp();

    void reset();

private:
    const QString logPath = QString(QDir::currentPath()+"/log.log");
    const QString configPath = QDir::currentPath()+"/config.json";
    const QDir curDir = QDir(QDir::currentPath());
    QVector<Folder> cacheFolders;
    //assert: Folder.fileList is used!

    QJsonDocument config;
    //assert: config & cacehFolders are sync

    void readConfigAndCacheFolder();
    //assert: read config from file to object config

    void writeLog(const QString log);
    void writeLog(const char * log);

    void _initDefaultCacheFolders(QVector<Folder> *folders);
    //assert: not use folder.fileList

    QJsonArray getJsonArray(const QJsonObject object, const QString arrayName);
    Folder getFolderFromObject(const QJsonObject object, const bool fileList = false);
    QString getStringFromJson(const QJsonObject object, const QString stringName);
    bool getBoolFromJson(const QJsonObject object, const QString boolName);

    QJsonArray createJsonArray(const QVector<Folder> folderLists);
    //assert: not use folder.fileList
    QJsonObject createFolderObject(const Folder folder);
    //assert: not use folder.fileList


};

#endif // CACHECLEAN_H
