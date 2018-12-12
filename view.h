#ifndef VIEW_H
#define VIEW_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QDirIterator>
#include <QProcess>
#include <QThread>
#include <QDomDocument>

class View : public QObject
{
    Q_OBJECT
public:
    explicit View(QObject *parent = nullptr);
    void ListElements (QDomElement root , QString tagname,QString attribute);
    QString PackageName;
    QString Year;
    QString Message="";


signals:

public slots:

    void LogCat();
    void ApkToolStandardOutPut();
    void ApkToolStandardError();
    void adbOutPut();
};

#endif // VIEW_H
