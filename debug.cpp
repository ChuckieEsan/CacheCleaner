#include "debug.h"

Debug::Debug()
{

}

void Debug::alert(QString content, QString title, QWidget *widget)
{
    QMessageBox msgBox;
    if(widget)
        msgBox.setParent(widget);
    msgBox.setWindowTitle(title);
    msgBox.setText(content);
    msgBox.exec();
}

void Debug::alert(QVector<Folder> content)
{
    QStringList paths;
    QStringList deleteFolders;
    for(int i = 0; i<content.size(); i++)
        paths.append(content[i].path);
    for(int i = 0; i<content.size(); i++)
        if(content[i].deleteFolder)
            deleteFolders.append(QString("True"));
        else
            deleteFolders.append(QString("False"));
    QString text;
    for(int i = 0; i<content.size(); i++)
    {
       text.append(paths[i]);
       text.append('\n');
       text.append(deleteFolders[i]);
       text.append('\n');
    }
    alert(text,QString("Folder:"));
}

void Debug::alert(QJsonObject content)
{
    QJsonObject obj = content;

}
