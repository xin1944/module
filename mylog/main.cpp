#include <QCoreApplication>
#include <QTextCodec>
#include <QThread>
#include "Log.h"
 
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
 
    QThread::currentThread()->setObjectName("主线程");
 
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
 
    Log::instance()->init(QCoreApplication::applicationDirPath() + "/" +"log.conf");
    Log::instance()->debug("调试测试日志系统当中...");
    Log::instance()->info("信息测试日志系统当中...");
    Log::instance()->warn("警告测试日志系统当中...");
    Log::instance()->error("错误测试日志系统当中...");
 
    return a.exec();
}