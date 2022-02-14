#include "cacheclean.h"

CacheClean::CacheClean()
{
    /* Json 文件架构
     * 其本质上是一个QJsonArray,对象名为 folderLists
     * folderLists 的元对象是 Folder,但是不适用 fileList
     * Folder
     * QStringList fileList
     * bool deleteFolder
     * QString path
     */
    qDebug() << configPath;
}

CacheClean::~CacheClean()
{

}
void CacheClean::setUp()
{
    if(curDir.exists(QString("config.json")))
    {
        qDebug() << "config.json exist";
        qDebug() << "path:" << configPath;
    }
    else
    {
        QMessageBox msgBox(QMessageBox::Information,
                           QString("Hello!"),
                           QString("It may be your first time to run this program.\nClick OK to initialize settings."));
        msgBox.exec();
        //set up config.json
        QVector<Folder> tempFolders;
        _initDefaultCacheFolders(&tempFolders);
        QFile configFile(configPath);
        if(configFile.open(QIODevice::WriteOnly))
        {
            QJsonArray array = createJsonArray(tempFolders);
            QJsonObject obj;
            obj.insert("folderLists",array);
            config.setObject(obj);
            configFile.write(config.toJson());
        }
        else
        {
            QMessageBox error(QMessageBox::Critical,
                              QString("Error:"),
                              QString("Initialize default settings error!\n"));
            error.exec();
            exit(0);
        }
        configFile.close();
    }
    readConfigAndCacheFolder();
}

void CacheClean::writeLog(const QString log)
{
    QFile logFile(logPath);
    QString time = QDateTime::currentDateTime().toString();
    if(logFile.open(QIODevice::Append))
        logFile.write(QString(time+':'+log+'\n').toUtf8());
    logFile.close();
}

void CacheClean::writeLog(const char *log)
{
    writeLog(QString(log));
}

void CacheClean::removeCacheFolder(const QString folderPath)
{
    QFile configFile(configPath);
    for(int i = 0; i < cacheFolders.size(); i++)
    {
        Folder t = cacheFolders[i];
        if(QString::compare(t.path,folderPath) == 0)
        {
            cacheFolders.remove(i);
        }
    }
    // let cacheFolders to be a json object
    if(configFile.open(QIODevice::ReadWrite))
    {
        config = QJsonDocument::fromJson(configFile.readAll());
        if(config.isObject())
        {
            QJsonArray array = getJsonArray(config.object(),"folderLists");
            QJsonObject obj;
            obj.insert("folderLists",array);
            config.setObject(obj);
            configFile.write(config.toJson());
        }     
    }
    configFile.close();

}

void CacheClean::addCacheFolder(const QString folderPath, const bool deleteFolder)
{
    QFile configFile(configPath);
    if(configFile.open(QIODevice::ReadWrite))
    {
        config = QJsonDocument::fromJson(configFile.readAll());
        if(config.isObject())
        {
            QJsonArray array = getJsonArray(config.object(),"folderLists");
            Folder folder;
            folder.deleteFolder = deleteFolder;
            folder.path = folderPath;
            QJsonObject folderObject = createFolderObject(folder);
            array.append(folderObject);
            QJsonObject obj;
            obj.insert("folderLists",array);
            config.setObject(obj);
            configFile.write(config.toJson());
        }
    }
    configFile.close();
}

void CacheClean::updateCacheFolder(const QString folderPath, const bool deleteFolder)
{
    //assert: find current CacheFolder and update info
    QFile configFile(configPath);
    if(configFile.open(QIODevice::ReadWrite))
    {
        config = QJsonDocument::fromJson(configFile.readAll());
        QJsonArray array = getJsonArray(config.object(),"folderLists");
        for(int i = 0; i<array.size(); i++)
        {
            Folder folder = getFolderFromObject(array[i].toObject());
            if(QString::compare(folder.path,folderPath) == 0)
            {
                folder.deleteFolder = deleteFolder;
                QJsonObject t = createFolderObject(folder);
                array.replace(i,t);
            }
        }
        QJsonObject obj;
        obj.insert("folderLists",array);
        config.setObject(obj);
        configFile.write(config.toJson());
    }
    configFile.close();

}

QJsonArray CacheClean::getJsonArray(const QJsonObject object, const QString arrayName)
{
    QJsonObject obj = object;
    if(obj.contains(arrayName) && obj.value(arrayName).isArray())
        return obj.value(arrayName).toArray();
    else
        return QJsonArray();

}

Folder CacheClean::getFolderFromObject(const QJsonObject object, const bool fileList)
{
    QJsonObject obj = object;
    Folder f;
    if(obj.contains("path") && obj.value("path").isString() &&
            obj.contains("deleteFolder") && obj.value("deleteFolder").isBool())
    {
        f.path = obj.value("path").toString();
        f.deleteFolder = obj.value("deleteFolder").toBool();
        if(fileList)
        {
            QDir dir(f.path);
            if(dir.exists())
                f.fileList = dir.entryList();
        }
    }
    return f;
}

QString CacheClean::getStringFromJson(const QJsonObject object, const QString stringName)
{
    QJsonObject obj = object;
    if(obj.contains(stringName) && obj.value(stringName).isString())
        return obj.value(stringName).toString();
    else
        return QString();
}

bool CacheClean::getBoolFromJson(const QJsonObject object, const QString boolName)
{
    QJsonObject obj = object;
    if(obj.contains(boolName) && obj.value(boolName).isBool())
    {
        return obj.value(boolName).toBool();
    }
    else
        return false;
}

QJsonObject CacheClean::createFolderObject(const Folder folder)
{
    //assert: not use Folder.fileList
    QJsonObject obj;
    obj.insert("path",folder.path);
    obj.insert("deleteFolder",folder.deleteFolder);
    return obj;
}

QJsonArray CacheClean::createJsonArray(const QVector<Folder> folderLists)
{
    QJsonArray array;
    for(int i = 0; i < folderLists.size(); i++)
    {
        QJsonObject obj = createFolderObject(folderLists[i]);
        array.append(obj);
    }
    return array;
}

void CacheClean::readConfigAndCacheFolder()
{
    QFile configFile(configPath);
    QJsonParseError jsonParserError;
    if(configFile.open(QIODevice::ReadWrite))
    {
        config = QJsonDocument::fromJson(configFile.readAll(),&jsonParserError);
        if(!config.isNull()&&
                jsonParserError.error == QJsonParseError::NoError)
        {
            if(config.isObject())
            {
                QJsonObject configObject = config.object();
                QJsonArray _folderLists = getJsonArray(configObject,"folderLists");
                if(!_folderLists.isEmpty())
                {
                    for(int i = 0; i < _folderLists.size(); i++)
                    {
                        Folder f = getFolderFromObject(_folderLists[i].toObject(),true);
                        cacheFolders.append(f);
                    }
                }
            }
        }
    }
    configFile.close();
}

void CacheClean::_initDefaultCacheFolders(QVector<Folder> *folders)
{
    Folder f;
    f.fileList = QStringList();
    f.deleteFolder = false;
    f.path = QDir::currentPath() + QString("User Data/Cache/Default/Cache");
    folders->append(f);
    f.path = QDir::currentPath() + QString("User Data/Cache/Default/Media Cache");
    folders->append(f);
    f.path = QDir::currentPath() + QString("User Data/Default/Media Cache");
    folders->append(f);
    f.path = QDir::currentPath() + QString("User Data/Default/Cache");
    folders->append(f);
}

QVector<Folder> CacheClean::getFolderList()
{
    //assert: a public method
    return cacheFolders;
}

void CacheClean::reset()
{
    QVector<Folder> tempFolders;
    _initDefaultCacheFolders(&tempFolders);
    QFile configFile(configPath);
    if(configFile.open(QIODevice::WriteOnly))
    {
        QJsonArray array = createJsonArray(tempFolders);
        QJsonObject obj;
        obj.insert("folderLists",array);
        config.setObject(obj);
        configFile.write(config.toJson());
    }
    configFile.close();
    readConfigAndCacheFolder();
}

void CacheClean::writeCacheFolderConfig(const QVector<Folder> folders)
{
    cacheFolders = folders;
    QFile configFile(configPath);
    if(configFile.open(QIODevice::WriteOnly))
    {
        QJsonArray array = createJsonArray(folders);
        QJsonObject obj;
        obj.insert(QString("folderLists"),array);
        config.setObject(obj);
        configFile.write(config.toJson());
    }
    configFile.close();
}

void CacheClean::startClean(QPlainTextEdit *logReceiver)
{
    logReceiver->appendPlainText("---Start clean---");
    for(int i = 0; i<cacheFolders.size(); i++)
    {
        Folder f = cacheFolders[i];
        QDir dir(f.path);
        QDir d;
        if(dir.exists())
        {
            QStringList cacheList = dir.entryList(QDir::Files);
            logReceiver->appendPlainText("Start clean " + f.path);
            qDebug() << "FileCounts:" << cacheList.size();
            for(int j = 0; j<cacheList.size(); j++)
            {
                QString path = dir.absoluteFilePath(cacheList[j]);
                qDebug() << path;
                if(QFile::remove(path))
                    logReceiver->appendPlainText(cacheList[j] + " deleted.");
                else
                    logReceiver->appendPlainText("Delete " + cacheList[j] + " error");
            }
            logReceiver->appendPlainText(f.path + " cleaned.");
            if(cacheFolders[i].deleteFolder)
                d.rmpath(f.path);
        }
        else
        {
            logReceiver->appendPlainText(f.path + "not exists. Skip.");
        }
    }
    logReceiver->appendPlainText("---Clean end---");
}
