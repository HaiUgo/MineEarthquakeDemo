#include <QtCore/QByteArray>
#include <QtCore/QProcess>
#include <QtWidgets/QMessageBox>
#include "assistant.h"

Assistant::Assistant(): proc(0)
{
}

Assistant::~Assistant()
{
   if (proc && proc->state() == QProcess::Running)
   {
       // 试图终止进程
       proc->terminate();
       proc->waitForFinished(3000);
   }
   // 销毁proc
   delete proc;
}

// 显示文档
void Assistant::showDocumentation(const QString &page)
{
   if (!startAssistant())
       return;

   QByteArray ba("SetSource ");
   ba.append("qthelp://MineQuakeClient.myhelp/doc/");
   proc->write(ba + page.toLocal8Bit() + '\n');
}

// 启动Qt Assistant
bool Assistant::startAssistant()
{
   // 如果没有创建进程，则新创建一个
   if (!proc)
       proc = new QProcess();
   // 如果进程没有运行，则运行assistant，并添加参数
   if (proc->state() != QProcess::Running)
   {
       //QString app = QLatin1String("../MMQ3/documentation/assistant.exe");
       QString path = QDir::currentPath();
       QString app = path+"/documentation/assistant.exe";
       qDebug()<<"app:"<<app;
       QStringList args;
       args << ("-collectionFile")
           << (path + "/documentation/myhelp.qhc");

       //     << QLatin1String("../MMQ3/documentation/myhelp.qhc");
       proc->start(app, args); // 相当于执行命令：assistant –collectionFile myHelp.qhc
       if (!proc->waitForStarted())
       {
            QMessageBox::critical(0, QObject::tr("my help"),
                QObject::tr("Unable to launch Qt Assistant (%1)").arg(app));
            return false;
       }
   }
   return true;
}
