#include "view.h"

View::View(QObject *parent) : QObject(parent)
{


}

void View::LogCat()
{
    QProcess *CurrentProcess = dynamic_cast<QProcess *>( sender() );

    if (CurrentProcess)
    {
        QString Log=Year +"-" +CurrentProcess->readAllStandardOutput();
        Log.replace("\r\r\n","\r\r\n"+Year+"-");

        if(Log.contains(PackageName))
           qDebug(Log.toLatin1()) ;
    }

}
void View::ApkToolStandardOutPut()
{
    QProcess *CurrentProcess = dynamic_cast<QProcess *>( sender() );

    if (CurrentProcess)
    {

        QString Log=CurrentProcess->readAllStandardOutput();
        Message +=Log;
        qDebug(Log.toLatin1());
    }
}
void View::ApkToolStandardError()
{
    QProcess *CurrentProcess = dynamic_cast<QProcess *>( sender() );

    if (CurrentProcess)
    {
        QString Log=CurrentProcess->readAllStandardError();
        Message +=Log;
        qDebug(Log.toLatin1());
    }
}
void View::adbOutPut()
{
    QProcess *CurrentProcess = dynamic_cast<QProcess *>( sender() );

    if (CurrentProcess)
    {
        QString Log=CurrentProcess->readAllStandardOutput();
        if (Log.contains("Failure [INSTALL_FAILED_CONTAINER_ERROR]"))
        {
            qDebug () << "Failed to Install Injected APK On AVD !!! \nCFDroid is Exiting ...";
            exit(-1);
        }
        qDebug(Log.toLatin1());
    }
}





void View::ListElements (QDomElement root , QString tagname,QString attribute)
{
    QDomNodeList items=root.elementsByTagName(tagname);
    qDebug () << tagname << "\nTotal items = " << items.count();

    for (qint16 i=0 ; i< items.count() ; i ++ )
    {
        QDomNode itemnode = items.at(i);
        if(itemnode.isElement())
        {
            QDomElement itemele =itemnode.toElement();
            qDebug () << itemele.attribute(attribute);
        }
    }
}
