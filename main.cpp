#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QDirIterator>
#include <QProcess>
#include <QThread>
#include <QDomDocument>
#include "view.h"






int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString ApkToolPath,ApkFilePath,ResultPath,KeyStorePath,KeyStorePassword;


    ////////////////////////////////////////////// .apk file is passing to CFDroid ////////////////////////////////////////////////

    qDebug () << "*********************      Phase : zero =======>>>>>> .apk file is passsing to CFDroid      ***********************";
    qDebug () << "";

    if (argc == 1 )
    {
        qDebug () << "please attention to under points !!!!!";
        qDebug () << "please install java !!!! and set PATH Envirment Variable .... !!!\n";
        qDebug () << "please setup a android virtual device (avd) with android avd manager or with android stadio !!!";
        qDebug () << "usage is ...\n";
        qDebug () << "CFDroid -A apktoolpath -a apkFilePath -o OutputPath -k KeyStorePath -p KeyStorePassword";
        qDebug () << "\nfor example :\n";
        qDebug () << "CFDroid -A C:/apktool.jar -a C:/apk1.apk -o C:/OUT -k C:/test-key.keystore -p 123456";
        return 0;
    }
    else if( (argc > 1 && argc < 11) || argc >11)
    {
        qDebug () << "the usage is incorrect !!!";
        qDebug () << "usage is .... ";
        qDebug () << "CFDroid -A apktoolpath -a apkFilePath -o OutputPath -k KeyStorePath -p KeyStorePassword";
        qDebug () << "\nfor example :\n";
        qDebug () << "CFDroid -A C:/apktool.jar -a C:/apk1.apk -o C:/OUT -k C:/test-key.keystore -p 123456";
        return 0;
    }

    for (qint8 i=1 ;i < argc;i++)
    {
        if (i%2 != 0)
        {
            QString argument=argv[i];
            if(argument == "-A")
            {
                ApkToolPath=argv[i+1];

            }else if (argument == "-a")
            {
                ApkFilePath=argv[i+1];

            }else if (argument == "-o")
            {
                ResultPath=argv[i+1];
            }else if (argument == "-k")
            {
                KeyStorePath=argv[i+1];
            }else if (argument == "-p")
            {
                KeyStorePassword=argv[i+1];
            }
             else
            {
                qDebug () << "your argument is incorrect !!!";
                qDebug () << "usage is .... ";
                qDebug () << "CFDroid -A apktoolpath -a apkFilePath -o OutputPath -k KeyStorePath -p KeyStorePassword";
                qDebug () << "\nfor example :\n";
                qDebug () << "CFDroid -A C:/apktool.jar -a C:/apk1.apk -o C:/OUT -k C:/test-key.keystore -p 123456";
                return 0;
            }

        }
    }


    qDebug () << "\n\nYour Parameter is ok and apk passed to CFDroid !!!\n";
    qDebug () << "Phase zero is done !!! ...\n\nPlease Wait 5 sec ...\n";
    QThread::sleep(5);




    ////////////////////////////////////////////// Decompiling .apk File ////////////////////////////////////////////////

    qDebug () << "*********************      Phase : One =======>>>>>> Decompiling .apk File      ***********************";
    qDebug () << "";
    QString command = "java -jar "+ApkToolPath+" d "+ApkFilePath+" -o "+ResultPath+" -f";
    QProcess APKToolProcess;
    APKToolProcess.start(command);
    APKToolProcess.waitForStarted();
    View v1;
    QObject::connect( &APKToolProcess, SIGNAL(readyReadStandardOutput()),&v1 ,SLOT(ApkToolStandardOutPut()) );
    QObject::connect( &APKToolProcess, SIGNAL(readyReadStandardError()),&v1 ,SLOT(ApkToolStandardError()) );
    APKToolProcess.waitForFinished(-1);
    QObject::disconnect( &APKToolProcess, SIGNAL(readyReadStandardOutput()),&v1 ,SLOT(ApkToolStandardOutPut()) );
    QObject::disconnect( &APKToolProcess, SIGNAL(readyReadStandardError()),&v1 ,SLOT(ApkToolStandardError()) );
    if(v1.Message.isEmpty())
    {
        qDebug () << "Java is not install on your system or maybe java binary file is not in your PATH Envirment Variable !!!!";
        qDebug () << "Exiting ...";
        return -1;
    }
    if (v1.Message.contains("Unable to access jarfile"))
    {
        qDebug () << "Your apktool not found !!!! maybe yourpath is incorrect ...";
        qDebug () << "Exiting ...";
        return -1;
    }
    if(v1.Message.contains("Input file") && v1.Message.contains("was not found or was not readable"))
    {
        qDebug () << "Your apk file is not found !!! maybe yourpath is incorrect !!!!";
        qDebug () << "Exiting ...";
        return -1;
    }
    if(v1.Message.contains("brut.directory.DirectoryException: file must be a directory:"))
    {
        qDebug () << "Your Output Path is not found !!! maybe yourpath is incorrect !!!!";
        qDebug () << "Exiting ...";
        return -1;
    }
    qDebug () << "Phase one is done !!! ...\n\nPlease Wait 5 sec ...\n";
    QThread::sleep(5);

    ///////////////////////////////////////////////////////Extraction Meta Data//////////////////////////////////////////

    qDebug () << "*********************      Phase : tow =======>>>>>> Extraction Meta Data      ***********************";
    qDebug () << "";


    QDomDocument document;
    QFile InputXml(ResultPath+"/AndroidManifest.xml");
    if(!InputXml.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug () << "Manifest.Xml file connot be opened !!!! ";
        return -1;
    }
    if (!document.setContent(&InputXml))
    {
        qDebug () << "Fail to open document !!!! ";
        return -1;
    }
    InputXml.close();
    QDomElement rootEm=document.firstChildElement();
    QString PackageName=rootEm.attribute("package");
    QString SmaliPath=PackageName;
    v1.PackageName=PackageName;
    SmaliPath.replace(".","/");
    qDebug() << "smali path is : "<<SmaliPath;
    qDebug () << "Package Name is : "<< rootEm.attribute("package") << "\n";
    v1.ListElements(rootEm,"activity","android:name");
    qDebug () << "";
    v1.ListElements(rootEm,"receiver","android:name");
    qDebug () << "";
    v1.ListElements(rootEm,"service","android:name");
    qDebug () << "";

    qDebug () << "Phase tow is done !!! ...\n\nPlease Wait 5 sec ...\n";
    QThread::sleep(5);




    ////////////////////////////////////Finding Methods & Details From Smali Files and log.d injected to them///////////////////////////////////

    qDebug () << "****************      Phase : three & four & five  =======>>>>>> Finding Methods & Details From Smali Files and log.d injected to them  ****************";
    qDebug () << "";


    ////////////////////////////////////////////////////

    QString dir=ResultPath+"/smali/"+SmaliPath;
    QDirIterator it(dir, QStringList() << "*.smali", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {

        QString CurrentFile=it.next();
        if ( !CurrentFile.contains("com/google") && !CurrentFile.contains("com/android") && !CurrentFile.contains("android/support"))

        {
            //qDebug () << CurrentFile;
            QFile ReadSmali(CurrentFile);
            if(!ReadSmali.open( QIODevice::ReadOnly | QIODevice::Text ))
            {
                qDebug () << "Fail to open file !! ";
                return -1;
            }
            QTextStream in(&ReadSmali);

            QStringList strlist;
            QString LineInSmali;
            QString PackageandClassName;
            QString PackageandClassandMethodName;
            QString TypeClass;

            while(!in.atEnd())
            {

                LineInSmali = in.readLine();
                strlist << LineInSmali;
                qint8 NumberOfRegisters;
                //////////////////////////////////////////////////////////////////////////////////find class////////////////////////////////////////////////////////////////
                if(LineInSmali.contains(".class"))
                {
                    TypeClass=LineInSmali;
                    QStringList SLTemp=LineInSmali.split(" ",QString::SkipEmptyParts);
                    PackageandClassName=SLTemp.at(SLTemp.length()-1);
                    PackageandClassName.remove(0,1);
                    PackageandClassName.remove(PackageandClassName.length()-1,1);
                    PackageandClassName.replace(QString("/"),QString("."));
                }
                ////////////////////////////////////////////////////////////find start method///////////////////////////////////////////////////////////////

                bool isNative = false;

                if(LineInSmali.contains(".method") && !LineInSmali.contains("abstract") && !TypeClass.contains("interface"))
                {
                    if (!LineInSmali.contains("native"))
                    {
                        QStringList SLTemp=LineInSmali.split(" ",QString::SkipEmptyParts);
                        QString MethodName=SLTemp.at(SLTemp.length()-1);
                        if(LineInSmali.contains("constructor"))
                        { PackageandClassandMethodName = PackageandClassName + ".constructor " + MethodName;}
                        else
                        {PackageandClassandMethodName = PackageandClassName +"."+MethodName;}
                        qDebug (PackageandClassandMethodName.toLatin1());
                        while (!in.atEnd())
                        {
                            LineInSmali = in.readLine();
                            if (LineInSmali.contains(".locals"))
                            {

                                foreach( QString numStr, LineInSmali.split(" ", QString::SkipEmptyParts) )
                                {
                                    bool check = false;
                                    NumberOfRegisters = numStr.toInt(&check, 10);
                                    if( !check )
                                        continue;
                                    else
                                    {
                                        if (NumberOfRegisters <=1 )
                                            LineInSmali=".locals "+QString::number(2);
                                        break;
                                    }

                                }
                                strlist << LineInSmali;
                                strlist << "    const-string v0, \"CFDroid\"";
                                strlist << "    const-string v1, \""+PackageandClassandMethodName+" : Start\"";
                                strlist << "    invoke-static {v0, v1}, Landroid/util/Log;->d(Ljava/lang/String;Ljava/lang/String;)I";
                                break;
                            }

                        }
                    } else isNative = true;
                }
                //////////////////////////////////////////////////////////////////////////////////find end method///////////////////////////////////////////////////////////////
                if(!isNative && LineInSmali.contains(".end method")&& !LineInSmali.contains("abstract") && !LineInSmali.contains("native") && !TypeClass.contains("interface"))
                {
                    qint16 kk=0;
                    kk=0;
                    for (qint64 i= strlist.length() -1 ;i >= 0 ;i--)
                    {
                        QString Temp=strlist.at(i);

                        if(Temp.contains("return-object") || Temp.contains("return-wide") )
                        {
                            if(Temp.contains("v0") || Temp.contains("v1"))
                            {
                                strlist.insert(i,"    invoke-static {v3, v4}, Landroid/util/Log;->d(Ljava/lang/String;Ljava/lang/String;)I");
                                strlist.insert(i,"    const-string v4, \""+PackageandClassandMethodName+" ["+QString::number(++kk)+","+QString::number(i+4)+"] : End\"");
                                strlist.insert(i,"    const-string v3, \"CFDroid\"");

                                for (qint64 j=i;j>=0;j--)
                                {
                                    QString ss=strlist.at(j);
                                    if(ss.contains(".locals"))
                                    {
                                        foreach( QString numStr, ss.split(" ", QString::SkipEmptyParts) )
                                        {
                                            bool check = false;
                                            NumberOfRegisters = numStr.toInt(&check, 10);
                                            if( !check )
                                                continue;
                                            else
                                            {
                                                if (NumberOfRegisters <=4 )
                                                {
                                                    ss=".locals "+QString::number(5);
                                                    strlist.replace(j,ss);
                                                }
                                                break;
                                            }

                                        }
                                        break;
                                    }
                                }
                            }
                            else
                            {

                                strlist.insert(i,"    invoke-static {v0, v1}, Landroid/util/Log;->d(Ljava/lang/String;Ljava/lang/String;)I");
                                strlist.insert(i,"    const-string v1, \""+PackageandClassandMethodName+" ["+QString::number(++kk)+","+QString::number(i+4)+"] : End\"");
                                strlist.insert(i,"    const-string v0, \"CFDroid\"");
                            }
                            break;


                        }else if (Temp.contains("return-void"))
                        {

                            strlist.insert(i,"    invoke-static {v0, v1}, Landroid/util/Log;->d(Ljava/lang/String;Ljava/lang/String;)I");
                            strlist.insert(i,"    const-string v1, \""+PackageandClassandMethodName+" ["+QString::number(++kk)+","+QString::number(i+4)+"] : End\"");
                            strlist.insert(i,"    const-string v0, \"CFDroid\"");
                            break;

                        }else if(Temp.contains("return"))
                        {


                            if(Temp.contains("v0") || Temp.contains("v1"))
                            {
                                strlist.insert(i,"    invoke-static {v2, v3}, Landroid/util/Log;->d(Ljava/lang/String;Ljava/lang/String;)I");
                                strlist.insert(i,"    const-string v3, \""+PackageandClassandMethodName+" ["+QString::number(++kk)+","+QString::number(i+4)+"] : End\"");
                                strlist.insert(i,"    const-string v2, \"CFDroid\"");

                                for (qint64 j=i;j>=0;j--)
                                {
                                    QString ss=strlist.at(j);
                                    if(ss.contains(".locals"))
                                    {
                                        foreach( QString numStr, ss.split(" ", QString::SkipEmptyParts) )
                                        {
                                            bool check = false;
                                            NumberOfRegisters = numStr.toInt(&check, 10);
                                            if( !check )
                                                continue;
                                            else
                                            {
                                                if (NumberOfRegisters <=3 )
                                                {
                                                    ss=".locals "+QString::number(4);
                                                    strlist.replace(j,ss);
                                                }
                                                break;
                                            }

                                        }
                                        break;
                                    }
                                }
                            }
                            else
                            {

                                strlist.insert(i,"    invoke-static {v0, v1}, Landroid/util/Log;->d(Ljava/lang/String;Ljava/lang/String;)I");
                                strlist.insert(i,"    const-string v1, \""+PackageandClassandMethodName+" ["+QString::number(++kk)+","+QString::number(i+4)+"] : End\"");
                                strlist.insert(i,"    const-string v0, \"CFDroid\"");


                            }
                            break;
                        }
                    }
                }
            }
            ReadSmali.close();
            QFile out(CurrentFile);
            if(!out.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                qDebug() << "fail to open for write !!!";
                return -1;
            }
            QTextStream write(&out);
            for (qint64 i=0;i<strlist.length();i++)
            {
                QString Temp=strlist.at(i);
                write<< Temp << "\n";

            }
            out.close();
            strlist.clear();

        }

    }
        qDebug () << "\n\nPhase three & four & five are done !!! ...\n\nPlease Wait 5 sec ...\n";
    QThread::sleep(5);

    /////////////////////////////////////////////////Buliding Injected Smali Codes ///////////////////////////////////////////////

    qDebug () << "*********************      Phase : six =======>>>>>> Buliding Injected Smali Codes      ***********************";
    qDebug () << "";


    try
    {
        command = "java -jar "+ApkToolPath+" b "+ResultPath+" -o "+ResultPath+"/dist/Injected.apk";
        // java -jar E:/apktool.jar
        APKToolProcess.start(command);
        APKToolProcess.waitForStarted();

        QObject::connect( &APKToolProcess, SIGNAL(readyReadStandardOutput()),&v1 ,SLOT(ApkToolStandardOutPut()) );
        QObject::connect( &APKToolProcess, SIGNAL(readyReadStandardError()),&v1 ,SLOT(ApkToolStandardError()) );


        APKToolProcess.waitForFinished(-1);
    }
    catch(QString Error)
    {
       qDebug () << "Error for java !!!! \n CFDroid is Exiting ....";
       return -1;
    }
    qDebug () << "Phase six is done !!! ...\n\nPlease Wait 5 sec ...\n";
    QThread::sleep(5);

    ////////////////////////////////////////////////////Signing Injected APK////////////////////////////////////////////////////

    qDebug () << "*********************      Phase : seven =======>>>>>> Signing Injected APK      ***********************";
    qDebug () << "";

    try
    {
    QString jarSignerCommand="jarsigner -verbose -sigalg MD5withRSA -digestalg SHA1 -keystore "+KeyStorePath+" -storepass 123456 "+ResultPath+"/dist/Injected.apk"+" hasti";
    QProcess JarSign;
    JarSign.start(jarSignerCommand);
    JarSign.waitForFinished(-1);
    qDebug(JarSign.readAllStandardOutput());
    }
    catch(QString Error)
    {
        qDebug() << "JarSigner cannot to be running !!!! \n CFDroid is Exiting ....";
        return -1;
    }

    qDebug () << "Phase seven is done !!! ...\n\nPlease Wait 5 sec ...\n";
    QThread::sleep(5);

    ///////////////////////////////////////////////////// Checking the AVD is Running ////////////////////////////////////

    qDebug () << "*********************      Phase : eight =======>>>>>> Checking the AVD is Running      ***********************";
    qDebug () << "";

    try
    {
        QString TaskListCommand="tasklist";
        QProcess TaskList;
        TaskList.start(TaskListCommand);
        TaskList.waitForFinished(-1);;
        QString Teeee=TaskList.readAllStandardOutput();
        if(!Teeee.contains("qemu-system-i386"))
        {
            qDebug() << "there is no AVD running !!!! \n CFDroid is Exiting ....";
            return -1;
        }
        else
        {
            qDebug() << "The AVD is running !!!! \n Let's go to next phase ....\n";
        }
    }
    catch(QString Error)
    {
        qDebug() << "there is no AVD running !!!! \n CFDroid is Exiting ....";
        return -1;
    }

    qDebug () << "Phase eight is done !!! ...\n\nPlease Wait 5 sec ...\n";
    QThread::sleep(5);




    //////////////////////////////////////////////////Installing Injected APK/////////////////////////////////////////////////

    qDebug () << "*********************      Phase : nine =======>>>>>> Installing Injected APK      ***********************";
    qDebug () << "";

    try
    {
    QProcess UninstallPackage;
    UninstallPackage.start("adb uninstall "+PackageName);
    UninstallPackage.waitForFinished(-1);
    QProcess InstallAPK;
    InstallAPK.start("adb install "+ResultPath+"/dist/Injected.apk");
    InstallAPK.waitForStarted();
    QObject::connect( &InstallAPK, SIGNAL(readyReadStandardOutput()), &v1 ,SLOT(adbOutPut()));

    InstallAPK.waitForFinished(-1);
    //    qDebug ()<< InstallAPK.readAllStandardOutput();


    }
    catch(QString Error)
    {
        qDebug() << "the injected apk file cannot to be execute !!!! \n CFDroid is Exiting ....";
        return -1;
    }
    qDebug () << "Phase nine is done !!! ...\n\nPlease Wait 5 sec ...\n";
    QThread::sleep(5);

    ///////////////////////////////////////////////////Listenning For LogCat//////////////////////////////////////////////////

    qDebug () << "*********************      Phase : ten =======>>>>>> Listenning For LogCat      ***********************";
    qDebug () << "";

    QProcess DateProcess;
    DateProcess.start("adb shell date");
    DateProcess.waitForFinished(-1);
    QString Date=DateProcess.readAllStandardOutput();
    QStringList SplitDate=Date.split(" ",QString::SkipEmptyParts);
    v1.Year=SplitDate.at(SplitDate.length()-1);
    v1.Year.remove(QRegExp("[\n\t\r]"));


    qDebug() << "\nLogCat is Listening .....\n";
    QProcess LogCatProcess;
    LogCatProcess.start("adb logcat -c");
    LogCatProcess.waitForStarted();
    QObject::connect( &LogCatProcess, SIGNAL(readyReadStandardOutput()), &v1 ,SLOT(LogCat()));
    LogCatProcess.waitForFinished(-1);
    LogCatProcess.start("adb logcat -v time");

    return a.exec();
}
